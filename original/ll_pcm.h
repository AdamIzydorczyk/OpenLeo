/* ll_pcm.h - PCM parallax IMAGE layer reader for Lew Leon (Longsoft 1997).
 *
 * Format recovered 2026-11-14 from LEO.EXE (layer ctor FUN_00056b20,
 * blitter FUN_00056c72, copy primitive FUN_0001d528, parallax updater
 * FUN_00057460, strip manager ctor FUN_00057250; stream reader FUN_00044b80
 * = the same byte-RLE codec that carries .SEQ patch streams):
 *
 *   hdr  := u16 w       (layer width in px; strips = w/80.  The
 *                        divide-by-0x50 crash = a missing PCM payload
 *                        making that quotient 0, then the blitter's
 *                        idiv faults)
 *         u16 rows      (ctor field +0x02: per-load window row count,
 *                        the blitter's height clamp)
 *         u16 c16       (observed 16; unclassified)
 *   tbl   := 40 x u32  (monotonic absolute payload offsets into the RLE
 *                       stream - each entry is a valid mid-stream resync
 *                       point - until the padding tail restarts near 0;
 *                       NOT row-aligned and never seeked by the row loop,
 *                       so role unresolved; we decode continuously)
 *   data  := one continuous byte-RLE stream from 0xa6 to EOF:
 *               byte >= 0xC0 -> repeat next byte (byte & 0x3F) times
 *               else         -> literal
 *   plane := row-major, pitch = w + 80 bytes (ctor allocates exactly
 *            win_rows * (w + 0x50); the 80-byte tail of each row is the
 *            gutter).  w is the visible layer width; the layer is
 *            strips = w/80 strips of 80 px.  (The blitter recomputes the
 *            pitch as (w/80)*80 + 80, which agrees exactly for w = 400;
 *            for w = 600/700 the two disagree - an engine quirk on
 *            non-80-multiple widths.  We frame at the ctor pitch.)
 *
 * Display model (FUN_00056c72 + FUN_0001d528): one blit paints a
 * 400-px-wide window (5 strips of 80 px) whose height is clamped to the
 * load window win_rows (the ctor allocates win_rows*(w+0x50) and decodes
 * exactly that many rows per load; deeper rows stream in with scrolling,
 * the 40-entry offset table holds resync points for that).  Screen
 * strip k shows plane strip (scroll_x + k) mod strips.  At rest that is
 * the identity, i.e. the view is plane columns 0..399.  Parallax
 * (FUN_00057460) shifts each strip by (strip_x - 200)*shift/100 about
 * the 400-px window centre (RAN: 5 strips * 80 = 400 = one full view).
 * ll_pcm_view() shows plane rows [y0, y0+nh) of the fully decoded plane;
 * ll_level() renders only the win_rows-tall rest window.  NOTE: the
 * plane decodes byte-exactly but looks dithered/garbled at depth on this
 * CD; row-interleaving inside the stream stays unresolved (see PLAN.md).
 */
#ifndef LL_PCM_H
#define LL_PCM_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    unsigned char *plane;   /* owned; rows * stride bytes                */
    unsigned w;             /* nominal layer width from header           */
    unsigned strips;        /* w/80: 80-px column strips per plane row   */
    unsigned win_rows;      /* header field +0x04 (load window height)   */
    unsigned stride;        /* blit pitch = strips*80 + 80               */
    unsigned rows;          /* decoded row count (partial last row incl.)*/
    int last_partial;       /* 1 if the last row is shorter than stride  */
} ll_pcm;

/* Decode a whole PCM payload. 0 = ok (ll_pcm_release REQUIRED),
 * -1 = malformed (too small / RLE runs off the end). */
int ll_pcm_decode(const unsigned char *payload, size_t size, ll_pcm *out);

/* Rest view: the 400-px blit window at world_x=0, rows [y0, y0+nh) of
 * the plane.  FUN_00056c72 at rest maps screen strip k to plane strip
 * k mod strips, i.e. plane column == view column for the first 400 px.
 * dst is 400*nh bytes; rows beyond `rows` read as zero. */
void ll_pcm_view(const ll_pcm *p, unsigned y0, unsigned nh,
                 unsigned char *dst);

/* Scrolled blit window (FUN_00056c72): screen strip k shows plane strip
 * (scroll_strips + k) mod strips - the engine's strip wrap as the level
 * camera advances by whole 80-px strips.  The per-strip parallax shear of
 * FUN_00057460 ((strip_x-200)*shift/100 about the window centre) is NOT
 * modelled: `shift` (a camera velocity term) is unrecovered. */
void ll_pcm_view_at(const ll_pcm *p, unsigned y0, unsigned nh,
                    unsigned scroll_strips, unsigned char *dst);

void ll_pcm_release(ll_pcm *p);

#ifdef __cplusplus
}
#endif
#endif /* LL_PCM_H */
