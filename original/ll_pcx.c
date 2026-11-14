/* ll_pcx.c - see ll_pcx.h. Decoder semantics were pinned against the real
 * ANIMACJE/MENU OPTIONS.PCX and ETAPY payloads (320x200 8bpp): expanded
 * stream = 200*320 + 65 trailing junk bytes, palette = raw last 768 bytes
 * (no 0x0C marker; last byte region may contain noise - we ignore trailing
 * junk after the last row and read the palette positionally). */
#include "ll_pcx.h"

#include <stdlib.h>
#include <string.h>

static unsigned rd16(const unsigned char *p)
{
    return (unsigned)p[0] | ((unsigned)p[1] << 8);
}

int ll_pcx_decode(const unsigned char *src, size_t len, ll_pcx *out)
{
    memset(out, 0, sizeof *out);
    if (len < 64 + 768 || src[0] != 0x0A || src[2] != 1)
        return 1;                       /* magic / RLE encoding / too short */
    /* Non-standard game headers: bytes 12..15 deviate from the spec; the
     * reliable fields are the window (4..12), bpp@3, the RLE stream and
     * the raw 768B palette tail. */
    unsigned x1 = rd16(src + 8), y1 = rd16(src + 10);
    if (src[3] != 8 || x1 > 4095 || y1 > 4095)
        return 2;                       /* only 8bpp supported */
    unsigned w = x1 + 1, h = y1 + 1;

    const unsigned char *p  = src + 64;
    const unsigned char *end = src + (len - 768); /* data stops before tail */
    unsigned char *idx = (unsigned char *)malloc((size_t)w * h);
    if (!idx)
        return 3;

    size_t row = 0;
    while (p < end && row < (size_t)w * h) {
        unsigned b = *p++;
        unsigned n, v;
        if (b >= 0xC0) {
            if (p >= end)
                break;
            n = b & 0x3F;
            v = *p++;
        } else {
            n = 1;
            v = b;
        }
        while (n-- && row < (size_t)w * h)
            idx[row++] = (unsigned char)v;
    }
    if (row < (size_t)w * h) {          /* truncated stream */
        free(idx);
        return 4;
    }
    memcpy(out->pal, src + (len - 768), 768);
    out->w   = w;
    out->h   = h;
    out->idx = idx;
    return 0;
}

void ll_pcx_free(ll_pcx *p)
{
    free(p->idx);
    memset(p, 0, sizeof *p);
}
