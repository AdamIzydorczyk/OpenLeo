/* ll_pcx.h - PCX image reader for Lew Leon asset payloads (OPTIONS.PCX,
 * JUNCTION.PCX, PLAN3.PCX, M15.PCX, KARA.PCX, PIORUN3.PCX - all six in the
 * containers verified byte-exact vs tools/pcx_dump.py). Game files are
 * standard 8bpp RLE BUT with a non-standard header: bytes 12..15 deviate
 * (planes byte = 64), there is no 0x0C palette marker - the VGA palette is
 * simply the LAST 768 bytes of the payload, and a few junk bytes may trail
 * the RLE stream. Verified decode rule: window fields @4..12, bpp@3, RLE
 * from offset 64 up to len-768, take exactly (x1+1)*(y1+1) pixels.
 * Output: 8-bit indexed pixels + RGB palette, ready for the SDL present
 * path (same palette-LUT trick as the .IS player).
 */
#ifndef LL_PCX_H
#define LL_PCX_H

#include <stddef.h>

typedef struct {
    unsigned w, h;
    unsigned char *idx;    /* w*h 8-bit indices (malloc'd)   */
    unsigned char pal[768];/* RGB, from payload tail         */
} ll_pcx;

/* Decode RLE PCX image from memory (src, len). 0 = ok. Free with ll_pcx_free. */
int  ll_pcx_decode(const unsigned char *src, size_t len, ll_pcx *out);
void ll_pcx_free(ll_pcx *p);

#endif /* LL_PCX_H */
