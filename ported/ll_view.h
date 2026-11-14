/* ll_view.h - one window/renderer convention for every screen
 * (cutscene player, title menu, chapter map, level scene).
 *
 * Review P-10 / tips T-12, T-13:
 *   - logical-size presentation: the renderer owns a logical viewport of
 *     the EXACT content size (320x200 / 640x400 / level view), the
 *     window is freely resizable, and SDL does the scaling + mouse
 *     mapping (screens therefore use raw event coords, no "/ sc").
 *   - default window geometry matches the old fixed behaviour: 2x for
 *     sub-640-wide content, 1x otherwise, centered - nothing changes
 *     until the user drags, presses F11/Alt+Enter, or passes
 *     --scale/--fullscreen (main.c fills ll_opt_*).
 *   - SDL_WINDOW_ALLOW_HIGHDPI + the app.manifest PerMonitorV2 entry
 *     give sharp output on scaled displays (no bitmap stretch).
 *   - fullscreen is always FULLSCREEN_DESKTOP (borderless desktop
 *     resolution), never exclusive mode changes (T-13).
 */
#ifndef LL_VIEW_H
#define LL_VIEW_H

#include <SDL2/SDL.h>

typedef struct {
    SDL_Window   *win;
    SDL_Renderer *ren;
    int logical_w, logical_h;
} ll_view;

/* CLI overrides (main.c); 0 = defaults. ll_opt_scale > 0 forces the
 * initial window multiple even under the logical viewport. */
extern int ll_opt_scale;
extern int ll_opt_fullscreen;

/* Creates window + renderer (accelerated, software fallback) sized
 * logical*scale, installs the logical viewport, and nearest scaling.
 * Returns 0, -1 on failure (nothing to destroy then). */
int ll_view_create(ll_view *v, const char *title, int logical_w,
                   int logical_h);

/* F11 or Alt+Enter toggles desktop-fullscreen; feed it EVERY event
 * from the screen's pump before its own key handling (it consumes the
 * event on a hit). Returns 1 when the event was the toggle. */
int ll_view_fullscreen_key(const SDL_Event *e, SDL_Window *win);

void ll_view_destroy(ll_view *v);

#endif /* LL_VIEW_H */
