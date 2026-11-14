/* ll_menu.h - minimal title-menu screen for the merged codebase (M3).
 * The original menu scripts (.mnu) are not present in the CD containers
 * (PLAN.md M3), so the recompiled game owns its menu wiring: callers pass
 * the item labels; the optional PCX background comes from the original
 * ANIMACJE container (OPTIONS.PCX, verified format). Keys: up/down move,
 * Enter selects (returns index), Q quits (-2), Enter on the first item +
 * ESC is just Enter (ESC = advance semantics, never kills the app).
 */
#ifndef LL_MENU_H
#define LL_MENU_H

#ifdef __cplusplus
extern "C" {
#endif

/* bg_pcx: raw PCX payload (ll_pcx input) or NULL for black background. */
int ll_menu_open(const unsigned char *bg_pcx, unsigned bg_len,
                 const char *const *items, int nitems);

#ifdef __cplusplus
}
#endif
#endif /* LL_MENU_H */
