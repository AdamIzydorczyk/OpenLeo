/* ll_menu.h - minimal title-menu screen for the merged codebase (M3).
 * The original menu scripts (.mnu) are not present in the CD containers
 * (PLAN.md M3), so the recompiled game owns its menu wiring: callers pass
 * the item labels; the optional PCX background comes from the original
 * ANIMACJE container (OPTIONS.PCX, verified format). Keys: up/down move,
 * Enter selects (returns index), Q quits (-2), ESC cancels (-1, never
 * picks the highlighted row - E-06; callers treat -1 like (BACK)).
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
    const char *tag;          /* container name (X.SEQ/BERLO.SEQ/...), or NULL */
    int x, y;                 /* origin in the 320x200 logical canvas the
                               * original authored .SEQ coords in (doubled
                               * 2x2 on the 640x400 CRTC).  x<0 in the map
                               * menu = park on the selected island. */
} ll_menu_anim;

/* bg_pcx: raw PCX payload (ll_pcx input) or NULL for black background.
 * anims/nanims: overlay animations or NULL/0. Lists longer than 10
 * items scroll as a 10-row window centred on the selection (^/v edge
 * markers), so e.g. the 14-level MAPKA select list fits 320x200. */
int ll_menu_open(const unsigned char *bg_pcx, unsigned bg_len,
                 const char *const *items, int nitems,
                 const ll_menu_anim *anims, int nanims);

/* Hot-zone answer buttons (restart-confirm dialog): a zone is bound to
 * an anim tag; clicking the sprite's painted rect (canvas px, union over
 * frames) or pressing `key` (SDL_Keycode as int, 0 = none) returns `ret`
 * (negative, e.g. -10/-11/-12) instead of an item index.  The original
 * hit-testing came from the missing .MNU scripts, so these rects are the
 * sprite bboxes at the authored origins (reconstruction, PLAN 8b).
 * Keyboard selection (round 5): LEFT/RIGHT cycle EVERYTHING pickable -
 * answer zones first (in caller order, restart passes TAK, CNIE, X),
 * then the list rows; UP/DOWN ride the same cycle (the words sit side
 * by side so left/right is the primary control, user request). The
 * selected word gets a solid bright underline UNDER the sprite (mouse
 * hover: dim 1-px underline; boxes over the words covered them), rows
 * get the '>' marker; Enter/Space confirms the highlighted entry (Esc =
 * cancel, returns -1, E-06), t/n/x remain direct zone shortcuts.
 * Round 6 (user request: "TAK (YES) should be bright green on select, as
 * NIE (NO) by default"): the selected word is ALSO re-tinted along the
 * background palette's green ramp - its own sprite indices are blitted
 * shifted up the ramp so its brightest ink lands on the ramp's top index
 * (ll_menu.c GREEN_* provenance; TAK +7, CNIE/X shift 0 = byte-exact
 * unchanged). `dflt` marks the zone that starts selected, i.e. the word
 * the dialog shows bright green before the user moves anything (restart
 * passes CNIE, which is also how the original art ships it).
 * Round 7 (user: "now TAK changes color, but NIE dont, and i guess that
 * buttons should be animated"): selection state is now two-directional -
 * UNSELECTED answer words are re-blitted DOWN the ramp to top idx15 and
 * FROZEN on sprite frame 0, while the SELECTED word keeps/gets the top
 * idx22 and cycles the 2 frames its .SEQ data already carries (measured:
 * identical shape, ink oscillating on the ramp, 100 ms period = the
 * original's own pulse). Every button now visibly changes when selection
 * enters AND leaves it, and the selected button is the animated one.
 * CI hook: LEO_MENU_PICK=<key char> auto-fires the matching zone at
 * tick 0 (one-shot). */
typedef struct {
    const char *tag;   /* ll_menu_anim.tag this zone covers */
    int ret;           /* value returned when the zone fires */
    int key;           /* SDL_Keycode (int) or 0 */
    int dflt;          /* non-zero = this zone is selected on open */
} ll_menu_zone;
int ll_menu_open_ex(const unsigned char *bg_pcx, unsigned bg_len,
                    const char *const *items, int nitems,
                    const ll_menu_anim *anims, int nanims,
                    const ll_menu_zone *zones, int nzones);

/* Original title menu is a 640x400 chapter MAP (measured 2026-11-14 in
 * the QEMU baseline: corner words wyjście/restart/autorzy/opcje, green
 * chapter title, BERLO red-arrow sprite pointing at an island; keyboard
 * advances/activates the chapter at the arrow).  The globe+islands bg
 * art is a confirmed CD data hole (PLAN 8b) so the map is composed
 * procedurally at the measured island anchors.  chs[] gives each
 * chapter's title + island anchor (640x400 coords); more than 6
 * chapters wrap onto extra anchor rings.  anims = .SEQ overlays (the
 * BERLO arrow cycles on its period_ms).  Returns selected chapter index,
 * -2 = wyjście/Esc, -3 = LEO_MENU_SECS deadline (CI hook), -4/-5/-6 =
 * the autorzy/opcje/restart corner words (a/o/r keys). */
typedef struct {
    const char *title;
    int ix, iy;              /* island centre the arrow points at */
    int rw, rh;              /* island half-extent in 640x400 px
                              * (0 = placeholder default 60/26) */
} ll_menu_ch;
int ll_menu_map_open(const ll_menu_ch *chs, int nchs,
                     const ll_menu_anim *anims, int nanims);

#ifdef __cplusplus
}
#endif
#endif /* LL_MENU_H */
