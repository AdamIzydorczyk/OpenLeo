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
                               * menu = park on the selected island; the
                               * BERLO cursor composites there as the
                               * FULL accumulated chain sprite (see the BERLO_
                               * note in ll_menu.c), not a single delta frame. */
} ll_menu_anim;

/* bg_pcx: raw PCX payload (ll_pcx input) or NULL for black background.
 * anims/nanims: overlay animations or NULL/0. Lists longer than 10
 * items scroll as a 10-row window centred on the selection (^/v edge
 * markers), so e.g. the 14-level MAPKA select list fits 320x200. */
int ll_menu_open(const unsigned char *bg_pcx, unsigned bg_len,
                 const char *const *items, int nitems,
                 const ll_menu_anim *anims, int nanims);

/* Hot-zone answer buttons (restart-confirm dialog): a zone is bound to
 * an anim tag; pressing `key` (printable key, ASCII code, 0 = none)
 * returns `ret` (negative, e.g. -10/-11/-12) instead of an item index.
 * Input is keyboard-only (owner directive 2026-11-15); the zone rects
 * stay as the drawn-extent bounds used for
 * the selection underline (the original hit-testing came from the
 * missing .MNU scripts; the rects are the sprite bboxes at the authored
 * origins, reconstruction, PLAN 8b).
 * Keyboard selection (round 5): LEFT/RIGHT cycle EVERYTHING pickable -
 * answer zones first (in caller order, restart passes TAK, CNIE, X),
 * then the list rows; UP/DOWN ride the same cycle (the words sit side
 * by side so left/right is the primary control, user request). The
 * selected word gets a solid bright underline UNDER the sprite (mouse
 * hover: dim 1-px underline; boxes over the words covered them), rows
 * get the '>' marker; Enter/Space confirms the highlighted entry (Esc =
 * cancel, returns -1, E-06), t/n/x remain direct zone shortcuts. (The
 * former mouse hover underline and row/zone clicks were removed
 * 2026-11-15: keyboard-only, owner directive.)
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
    int key;           /* printable key (ASCII) or 0 */
    int dflt;          /* non-zero = this zone is selected on open */
} ll_menu_zone;
int ll_menu_open_ex(const unsigned char *bg_pcx, unsigned bg_len,
                    const char *const *items, int nitems,
                    const ll_menu_anim *anims, int nanims,
                    const ll_menu_zone *zones, int nzones);

/* Original title menu is a 640x400 chapter MAP (measured 2026-11-14 in
 * the QEMU baseline: corner words wyjście/restart/autorzy/opcje drawn by
 * the engine's own cursive text, green chapter title, BERLO red-arrow
 * sprite pointing at an island; keyboard advances/activates the chapter
 * at the arrow).  The base layer is back_pcx/back_len = the engine's
 * CLEAN landscape record ANIMACJE/MENU/BACK.PCX (proved 2026-11-15 by
 * diff vs the captured VRAM: it carries no text/sprites; indices live in
 * the captured menu DAC's index space, so it is colored through
 * ll_menu_dac.h).  back_pcx=NULL falls back to the captured composed
 * frame embedded in the port (ll_menu_art.h, text baked in;
 * LEO_MENU_BG=placeholder selects the procedural stand-in).  Corner
 * words are the ORIGINAL cursive text layer recovered from the
 * captured VRAM (ll_menu_text.h; skipped on the baked-art fallback -
 * their letters are already in that art, drawing both was the double-
 * text defect); the words[] records add the SELECTION EMPHASIS swap
 * (see ll_menu_word).  chs[] gives each chapter's title + island
 * anchor (640x400 coords); more than 6 chapters wrap onto extra anchor
 * rings.  A chapter MAY carry its real cursive title image: seq/
 * seq_len = a raw ANIMACJE/MENU/LEVELxx.SEQ payload (owner-mapped to
 * the chapter 2026-11-15; the original MENU.mnu binding is a CD data
 * hole).  Then the map composites ALL its elements at their authored
 * (-f0,-f2) positions instead of font-drawing the title; seq=NULL or
 * an unparseable blob keeps the font-text fallback (RAN.01/JUNCTION
 * have no image; dataless mode always falls back).
 * anims = .SEQ overlays (the BERLO arrow cycles on its
 * period_ms).  Keyboard input ONLY, TWO-AXIS model (owner directive
 * 2026-11-15): Up/Down SWITCH the selection mode between the chapter
 * map and the corner options; Left/Right cycle WITHIN the active
 * group (chapters among chapters, the four corner words
 * WYJSCIE/RESTART/OPCJE/AUTORZY among themselves).  The selection
 * cursor (BERLO, the original's menu pointer) MOVES ONTO the chosen
 * word and the word pulses its .SEQ variants (words[]) - bright bar
 * at the recovered bbox only as fallback without the word records.
 * Enter/Space activates, a/o/r stay direct corner
 * shortcuts.  Returns selected chapter index,
 * -2 = wyjście/Esc, -3 = LEO_MENU_SECS deadline (CI hook),
 * -4/-5/-6 = the autorzy/opcje/restart corner words. */
typedef struct {
    const char *title;
    int ix, iy;              /* island centre the arrow points at */
    int rw, rh;              /* island half-extent in 640x400 px
                              * (0 = placeholder default 60/26) */
    const unsigned char *seq; /* title image .SEQ payload or NULL */
    unsigned seq_len;
} ll_menu_ch;

/* The four corner-WORD records ANIMACJE/MENU/WYJSCIE/RESTART/OPCJE/
 * AUTORZY.SEQ in that order (same index as the corner cycle and the
 * return codes): measured 2026-11-15 these ARE the cursive words -
 * two nearly-identical variants each (the bigger one's authored anchor
 * lands exactly on the recovered text bbox); while a word is SELECTED
 * the map alternates the variants at its authored (-f0,-f2) anchors -
 * a bold PULSE, the only animation the word records hold and what
 * the owner remembers as the word "animating" under the bulawa
 * cursor. NULL payload = no pulse, bright-bar marker fallback. */
typedef struct {
    const unsigned char *seq;
    unsigned len;
} ll_menu_word;
int ll_menu_map_open(const ll_menu_ch *chs, int nchs,
                     const ll_menu_anim *anims, int nanims,
                     const unsigned char *back_pcx, unsigned back_len,
                     const ll_menu_word words[4]);

#ifdef __cplusplus
}
#endif
#endif /* LL_MENU_H */
