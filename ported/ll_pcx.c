/* ll_pcx.c - see ll_pcx.h.

 * Header layout was TRUTH-PINNED 2026-11-15 (PR-36), not eyeballed: the
 * game PCXs carry the STANDARD 128-byte PCX header (0x0A,0x05,0x01,8 @0,
 * window @4..12, byte-per-line u16 @66 == width for every even-width
 * game file, zeros @120..127) and their RLE stream starts at offset 128.
 * Proof: ANIMACJE/MENU/BACK.PCX decoded from 128 reproduces 92.58% of
 * the captured menu VRAM index plane byte-identically and all 64000
 * bytes expand cleanly; decoding from 64 (the old rule) shifts every
 * pixel by 64 bytes = 0.2 row and matched the capture only 3.8%.
 * The palette is the raw LAST 768 bytes (no 0x0C marker); a few junk
 * bytes may trail the RLE stream (e.g. MFAKE.PCX's stray 0x0C), which
 * the row-counted loop never reaches.
 * Rows are counted at the header stride, keeping the first w bytes -
 * standard PCX pads each scanline to an even byte count (MFAKE.PCX,
 * 13x13, is the only odd-width file: stride 14, decodes exactly
 * 13x14 with 13 pad bytes + 1 junk byte after). */
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
    if (len < 128 + 768 || src[0] != 0x0A || src[2] != 1)
        return 1;                       /* magic / RLE encoding / too short */
    unsigned x1 = rd16(src + 8), y1 = rd16(src + 10);
    if (src[3] != 8 || x1 > 4095 || y1 > 4095)
        return 2;                       /* only 8bpp supported */
    unsigned w = x1 + 1, h = y1 + 1;
    unsigned padw = (w + 1) & ~1u;      /* PCX scanlines pad to even bytes */
    unsigned stride = rd16(src + 66);   /* byte-per-line field */
    if (stride < padw)
        return 2;

    const unsigned char *p  = src + 128;
    const unsigned char *end = src + (len - 768); /* data stops before tail */
    unsigned char *idx = (unsigned char *)malloc((size_t)w * h);
    if (!idx)
        return 3;

    for (unsigned r = 0; r < h; r++) {
        unsigned got = 0;
        unsigned char *dst = idx + (size_t)r * w;
        while (got < stride && p < end) {
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
            while (n--) {
                if (got < w)               /* keep w, drop the pad bytes */
                    dst[got] = (unsigned char)v;
                got++;
            }
        }
        if (got < stride) {                /* truncated stream */
            free(idx);
            return 4;
        }
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
