/* ll_pcm.c - PCM parallax layer decoder (see ll_pcm.h). Mirrors
 * FUN_00056b20 + FUN_00044b80 byte-for-byte on the emitted plane; checked
 * against tools/verify_pcm.py (make verify-pcm). */
#include "ll_pcm.h"

#include <stdlib.h>
#include <string.h>

#define PCM_PITCH    80               /* stride = w + 0x50 (FUN_00056bcd)*/

int ll_pcm_decode(const unsigned char *p, size_t size, ll_pcm *out)
{
    memset(out, 0, sizeof *out);
    if (size < 0x1c4)                 /* hdr + tables must fit           */
        return -1;
    out->w = p[0] | (p[1] << 8);
    out->win_rows = p[2] | (p[3] << 8);
    if (!out->w)
        return -1;                    /* the idiv-0x50 trap in ctor      */
    out->stride = out->w + PCM_PITCH;

    /* stream reader FUN_00044b80: one continuous byte-RLE run to EOF */
    size_t i = 0xa6;                  /* first byte after the 40-entry
                                       * table (game seeks there: the
                                       * 0x06-table entries start at 0) */
    size_t dec = 0;
    size_t cap = (size - i) * 32 + out->stride;  /* <=63 decoded per 2 in */
    unsigned char *buf = malloc(cap ? cap : 1);
    if (!buf)
        return -1;
    while (i < size) {
        unsigned c = p[i++];
        if ((c & 0xc0) == 0xc0) {
            if (i >= size)            /* truncated repeat: stream ends   */
                break;
            unsigned n = c & 0x3f;
            unsigned char v = p[i++];
            if (dec + n > cap) {
                free(buf);
                return -1;
            }
            memset(buf + dec, v, n);
            dec += n;
        } else {
            if (dec + 1 > cap) {
                free(buf);
                return -1;
            }
            buf[dec++] = (unsigned char)c;
        }
    }

    /* plane framing: row r at r*stride; trailing partial row zero-padded */
    unsigned rows = (unsigned)((dec + out->stride - 1) / out->stride);
    out->last_partial = (dec % out->stride) != 0;
    out->plane = calloc((size_t)rows * out->stride, 1);
    if (!out->plane) {
        free(buf);
        return -1;
    }
    for (unsigned r = 0; (size_t)r * out->stride < dec; r++) {
        size_t off = (size_t)r * out->stride;
        size_t n = dec - off < out->stride ? dec - off : out->stride;
        memcpy(out->plane + off, buf + off, n);
    }
    free(buf);
    out->rows = rows;
    return 0;
}

void ll_pcm_release(ll_pcm *p)
{
    free(p->plane);
    memset(p, 0, sizeof *p);
}
