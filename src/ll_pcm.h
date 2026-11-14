/* ll_pcm.h - PCM parallax IMAGE layer reader for Lew Leon (Longsoft 1997).
 *
 * Format recovered 2026-11-14 from LEO.EXE (layer ctor FUN_00056b20,
 * blitter FUN_00056c72, parallax FUN_00057460; stream reader FUN_00044b80
 * = the same byte-RLE codec that carries .SEQ patch streams):
 *
 *   hdr  := u16 w    (layer width; game field +0x02; the divide-by-0x50
 *                     crash = w / 80 tile-count check)
 *         u16 rows  (game field +0x00: per-load window row count AND the
 *                     blitter's vertical wrap modulus, see FUN_00056c72:
 *                     bufrow = (syrcreen % rows)*stride + x)
 *         u16 c16   (observed 16; unclassified)
 *         u16 x2, u16 x2  (zeros)
 *   tbl   := 40 x u32  (monotonic; base 0 -> observed ~payload size; role
 *                     unclear - the ctor copies the first 5 entries into a
 *                     40-entry stack array but the row loop never seeks)
 *   then 7 x 24-entry u32 tables (0x200000-stepped; unclassified)
 *   data  := one continuous byte-RLE stream (NOT chunked; every PCM
 *             payload decodes end-to-end from here, tail = partial row):
 *               byte >= 0xC0 -> repeat next byte (byte & 0x3F) times
 *               else         -> literal
 *   plane := row-major, stride = w + 80 bytes/row (0x50 slack/gutter),
 *            row r lives at r*stride, visible pixels = first w bytes.
 *
 * The game loads `rows` rows per pass (allocs 0x400 + rows*(w+80)); the
 * full stream decodes to ~1460-2850 rows, so the 40-row window is a
 * scrolling view, not the whole layer (full height = stream length).
 */
#ifndef LL_PCM_H
#define LL_PCM_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    unsigned char *plane;   /* owned; rows_full * stride bytes           */
    unsigned w;             /* layer width from header                   */
    unsigned win_rows;      /* header field +0x00 (window/wrap modulus)  */
    unsigned stride;        /* w + 80                                    */
    unsigned rows;          /* decoded row count (partial last row incl.)*/
    int last_partial;       /* 1 if the last row is shorter than stride  */
} ll_pcm;

/* Decode a whole PCM payload. 0 = ok (ll_pcm_release REQUIRED),
 * -1 = malformed (too small / RLE runs off the end). */
int ll_pcm_decode(const unsigned char *payload, size_t size, ll_pcm *out);

void ll_pcm_release(ll_pcm *p);

#ifdef __cplusplus
}
#endif
#endif /* LL_PCM_H */
