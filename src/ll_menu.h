/* ll_menu.h - minimal title-menu screen for the merged codebase (M3).
 * The original menu scripts (.mnu) are not present in the CD containers
 * (PLAN.md M3), so the recompiled game owns its menu wiring: callers pass
 * the item labels; the optional PCX background comes from the original
 * ANIMACJE container (OPTIONS.PCX, verified format). Keys: up/down move,
 * Enter selects (returns index), Q quits (-2), Enter on the first item +
 * ESC is just Enter (ESC = advance semantics, never kills the app).
 * Env LEO_MENU_SECS=N: show the menu N seconds then auto-quit (returns -3;
 * CI hook so the menu render is covered by headless runs).
 */
#ifndef LL_MENU_H
#define LL_MENU_H

#ifdef __cplusplus
extern "C" {
#endif

/* Optional .SEQ sprite animations composited onto the background before
 * the palette LUT: each anim cycles its elements as frames (element i
 * drawn at (-f0,-f2), frame-patch rows painted as 8bpp indices), like the
 * BERLO/CNIE/X animations over OPTIONS.PCX in the original title menu. */
typedef struct {
    const unsigned char *seq; /* raw .SEQ container payload */
    unsigned len;
    unsigned period_ms;       /* ms per frame, 0 = default 100 */
} ll_menu_anim;

/* bg_pcx: raw PCX payload (ll_pcx input) or NULL for black background.
 * anims/nanims: overlay animations or NULL/0. */
int ll_menu_open(const unsigned char *bg_pcx, unsigned bg_len,
                 const char *const *items, int nitems,
                 const ll_menu_anim *anims, int nanims);

#ifdef __cplusplus
}
#endif
#endif /* LL_MENU_H */
