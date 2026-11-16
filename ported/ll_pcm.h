/* ll_pcm.h - PCM parallax layer reader for Lew Leon (Longsoft 1997).
 *
 * A .PCM record is a TRANSPARENT 8bpp overlay layer, stored as column
 * strips of compiled-sprite tiles. Recovered 2026-11-16 (PR-53) from the
 * machine code of the layer loader FUN_00056d40 (file header, offset
 * tables, data blob) and of its blitter FUN_0001d58c (the tile opcode
 * interpreter); the earlier reading ("byte-RLE plane, stride w+80")
 * mixed this class up with the PCX layer class FUN_00056b20 and decoded
 * opcode bytes as pixels - hence the "dithered noise".
 *
 *   hdr   := u16 H        total height in rows
 *            u16 N        number of 80-px strips (layer width = N*80)
 *            u16 T        tile height in rows (16 in every shipped file)
 *   strip := N x u32      byte offset of strip s inside the data blob
 *   tiles := for each strip s: (G-1) x u32, G = ceil(H/T); with an
 *            implicit tile offset 0 for tile 0 these are the offsets of
 *            the strip's tiles relative to the strip's own base
 *   data  := the rest of the file, raw (NOT RLE): tile g of strip s starts
 *            at data[strip[s] + tiles[s][g]] and holds min(T, H-g*T) rows
 *   row   := a stream of opcodes covering EXACTLY 20 dwords (80 px)
 *
 * Opcode byte op with following bytes b1 b2 b3 (a dword = 4 pixels p0..p3,
 * p0 leftmost); pixels not written stay transparent:
 *   0     skip b1 dwords                               (2 bytes)
 *   1/2/4/8   one pixel p3/p2/p1/p0 = b1               (2 bytes)
 *   3     p2,p3 = b2,b3        5  p1,p3 = b1,b3
 *   6     p1,p2 = b1,b2        7  p1,p2,p3 = b1,b2,b3
 *   9     p0,p3 = b1,b2        a  p0,p2 = b1,b3
 *   b     p0,p2,p3 = b1,b2,b3  c  p0,p1 = b2,b3
 *   d     p0,p1,p3 = b2,b3,b1  e  p0,p1,p2 = b1,b2,b3  (4 bytes each)
 *   f     copy b1 raw dwords (2 + 4*b1 bytes)
 *   10    b1 dwords of the pattern b2 b3 b2 b3          (4 bytes)
 *   11+   the opcode (op-0x11) and then the ROW ENDS
 * Self-checks that need no ground truth: on all 23 shipped layers every
 * row of every tile covers exactly 20 dwords and every tile ends exactly
 * where the next one starts (tools/verify_pcm.py asserts both).
 *
 * The game draws layers as an overlay over the .PCX background, strips
 * wrapping modulo N horizontally; per-layer parallax factors live in
 * FUN_00057460 and are not recovered (ll_level documents the viewer rule).
 */
#ifndef LL_PCM_H
#define LL_PCM_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    unsigned h;             /* rows                                      */
    unsigned nstrips;       /* 80-px strips                              */
    unsigned w;             /* nstrips * 80                              */
    unsigned tile;          /* tile height (rows)                        */
    unsigned groups;        /* ceil(h / tile) tiles per strip            */
    unsigned char *px;      /* w*h palette indices (0 where transparent) */
    unsigned char *op;      /* w*h: 1 = painted, 0 = transparent         */
    int tiles_contiguous;   /* every tile ends where the next one starts */
} ll_pcm;

/* Decode a whole PCM payload. 0 = ok (ll_pcm_release REQUIRED), -1 =
 * malformed: header out of range, an offset outside the data, an opcode
 * running past the data, or a row that does not cover exactly 20 dwords.
 * Never reads or writes out of bounds on any input. */
int ll_pcm_decode(const unsigned char *payload, size_t size, ll_pcm *out);
void ll_pcm_release(ll_pcm *p);

/* FNV-1a-64 over px (transparent pixels as 0) then op: the CI oracle. */
uint64_t ll_pcm_fnv(const ll_pcm *p);

/* Paint the layer onto an 8bpp canvas (cw x ch), transparent pixels skipped.
 * Layer pixel (sx + x, sy + y) goes to canvas (dx + x, dy + y) for every
 * canvas pixel in range; the layer WRAPS horizontally (the engine's strip
 * modulo) and is empty above row 0 and below row h. */
void ll_pcm_blit(const ll_pcm *p, unsigned char *canvas, int cw, int ch,
                 int sx, int sy, int dx, int dy);

#ifdef __cplusplus
}
#endif
#endif
