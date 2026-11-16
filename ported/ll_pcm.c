/* ll_pcm.c - PCM parallax layer reader; format and RE notes in ll_pcm.h. */
#include "ll_pcm.h"

#include <stdlib.h>
#include <string.h>

#define ROW_DWORDS 20u                      /* 80 px per strip row          */
#define MAX_DIM    16384u                   /* sanity bound, > any layer    */

static unsigned rd16(const unsigned char *p)
{
    return (unsigned)p[0] | ((unsigned)p[1] << 8);
}

static uint32_t rd32(const unsigned char *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

/* One tile row: run opcodes from data[*pos] until the row-end opcode.
 * px/op point at the row's 80 pixels. 0 = the row covered exactly 20
 * dwords, -1 = malformed (ran out of data, past 20 dwords, bad opcode). */
static int row_run(const unsigned char *d, size_t n, size_t *pos,
                   unsigned char *px, unsigned char *op)
{
    size_t i = *pos;
    unsigned dw = 0;

    for (;;) {
        if (i >= n)
            return -1;
        unsigned code = d[i];
        int end = 0;
        if (code >= 0x11) {
            code -= 0x11;
            end = 1;
        }
        if (code > 0x10)
            return -1;
        unsigned b1 = i + 1 < n ? d[i + 1] : 0;
        unsigned b2 = i + 2 < n ? d[i + 2] : 0;
        unsigned b3 = i + 3 < n ? d[i + 3] : 0;
        size_t len;
        unsigned char *q = px + (size_t)dw * 4;
        unsigned char *o = op + (size_t)dw * 4;

        if (code == 0) {                       /* skip b1 dwords           */
            len = 2;
            dw += b1;
        } else if (code == 0xf) {              /* b1 raw dwords            */
            len = 2 + 4 * (size_t)b1;
            if (dw + b1 > ROW_DWORDS || i + len > n)
                return -1;
            memcpy(q, d + i + 2, 4 * (size_t)b1);
            memset(o, 1, 4 * (size_t)b1);
            dw += b1;
        } else if (code == 0x10) {             /* b1 dwords of b2 b3 b2 b3 */
            len = 4;
            if (dw + b1 > ROW_DWORDS || i + len > n)
                return -1;
            for (unsigned k = 0; k < b1; k++)
                for (unsigned p = 0; p < 4; p++) {
                    q[(size_t)k * 4 + p] = (unsigned char)(p & 1 ? b3 : b2);
                    o[(size_t)k * 4 + p] = 1;
                }
            dw += b1;
        } else {                               /* one dword, masked write  */
            /* src[op][p] = which of b1..b3 lands on pixel p (-1: pixel
             * not written), read off the 14 handlers of FUN_0001d58c */
            static const signed char src[16][4] = {
                { 0 }, { -1, -1, -1, 1 }, { -1, -1, 1, -1 },
                { -1, -1, 2, 3 }, { -1, 1, -1, -1 }, { -1, 1, -1, 3 },
                { -1, 1, 2, -1 }, { -1, 1, 2, 3 }, { 1, -1, -1, -1 },
                { 1, -1, -1, 2 }, { 1, -1, 3, -1 }, { 1, -1, 2, 3 },
                { 2, 3, -1, -1 }, { 2, 3, -1, 1 }, { 1, 2, 3, -1 } };
            unsigned char bb[4] = { 0, (unsigned char)b1, (unsigned char)b2,
                                    (unsigned char)b3 };
            len = (code == 1 || code == 2 || code == 4 || code == 8) ? 2 : 4;
            if (dw >= ROW_DWORDS || i + len > n)
                return -1;
            for (unsigned p = 0; p < 4; p++) {
                int s = src[code][p];
                if (s >= 0) {
                    q[p] = bb[s];
                    o[p] = 1;
                }
            }
            dw++;
        }
        if (dw > ROW_DWORDS)
            return -1;
        i += len;
        if (end) {
            *pos = i;
            return dw == ROW_DWORDS ? 0 : -1;
        }
    }
}

int ll_pcm_decode(const unsigned char *payload, size_t size, ll_pcm *out)
{
    memset(out, 0, sizeof *out);
    if (size < 6)
        return -1;
    unsigned h = rd16(payload), ns = rd16(payload + 2), tile = rd16(payload + 4);
    if (!h || !ns || !tile || h > MAX_DIM || ns > MAX_DIM / 80 * 4 ||
        tile > 256)
        return -1;
    unsigned groups = (h + tile - 1) / tile;
    size_t tabs = 4 * (size_t)ns + 4 * (size_t)ns * (groups - 1);
    size_t hdr = 6 + tabs;
    if (hdr > size)
        return -1;
    const unsigned char *data = payload + hdr;
    size_t dlen = size - hdr;
    size_t w = (size_t)ns * 80;
    if (w * h > ((size_t)256 << 20))
        return -1;

    unsigned char *px = calloc(w * h, 1), *op = calloc(w * h, 1);
    if (!px || !op) {
        free(px);
        free(op);
        return -1;
    }
    int contiguous = 1;
    for (unsigned s = 0; s < ns; s++) {
        uint32_t base = rd32(payload + 6 + 4 * (size_t)s);
        const unsigned char *tl = payload + 6 + 4 * (size_t)ns +
                                  4 * (size_t)s * (groups - 1);
        for (unsigned g = 0; g < groups; g++) {
            uint32_t toff = g ? rd32(tl + 4 * (size_t)(g - 1)) : 0;
            size_t pos = (size_t)base + toff;
            unsigned rows = h - g * tile < tile ? h - g * tile : tile;
            if (pos > dlen)
                goto bad;
            for (unsigned r = 0; r < rows; r++) {
                size_t off = ((size_t)g * tile + r) * w + (size_t)s * 80;
                if (row_run(data, dlen, &pos, px + off, op + off) != 0)
                    goto bad;
            }
            if (g + 1 < groups) {
                uint32_t nx = rd32(tl + 4 * (size_t)g);
                if (pos != (size_t)base + nx)
                    contiguous = 0;
            }
        }
    }
    out->h = h;
    out->nstrips = ns;
    out->w = (unsigned)w;
    out->tile = tile;
    out->groups = groups;
    out->px = px;
    out->op = op;
    out->tiles_contiguous = contiguous;
    return 0;
bad:
    free(px);
    free(op);
    return -1;
}

void ll_pcm_release(ll_pcm *p)
{
    free(p->px);
    free(p->op);
    memset(p, 0, sizeof *p);
}

uint64_t ll_pcm_fnv(const ll_pcm *p)
{
    uint64_t h = 0xCBF29CE484222325ull;
    size_t n = (size_t)p->w * p->h;
    for (size_t i = 0; i < n; i++) {
        h ^= p->op[i] ? p->px[i] : 0;
        h *= 0x100000001B3ull;
    }
    for (size_t i = 0; i < n; i++) {
        h ^= p->op[i];
        h *= 0x100000001B3ull;
    }
    return h;
}

void ll_pcm_blit(const ll_pcm *p, unsigned char *canvas, int cw, int ch,
                 int sx, int sy, int dx, int dy)
{
    if (!p->px || p->w == 0)
        return;
    for (int y = 0; y < ch; y++) {
        int ly = sy + (y - dy);
        int cy = y;
        if (cy < dy || ly < 0 || ly >= (int)p->h)
            continue;
        const unsigned char *srow = p->px + (size_t)ly * p->w;
        const unsigned char *orow = p->op + (size_t)ly * p->w;
        unsigned char *drow = canvas + (size_t)cy * (size_t)cw;
        for (int x = dx < 0 ? 0 : dx; x < cw; x++) {
            long lx = ((long)sx + (x - dx)) % (long)p->w;
            if (lx < 0)
                lx += (long)p->w;
            if (orow[lx])
                drow[x] = srow[lx];
        }
    }
}
