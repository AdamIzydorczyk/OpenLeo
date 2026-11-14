/* ll_view.h - one window/renderer convention AND one SDL window
 * session for every screen (cutscene player, title menu, chapter map,
 * level scene).
 *
 * Review P-03: one window for the whole program. ll_view_create hands
 * each screen its own *view handle*, but behind it sits a single
 * process-wide window + renderer:
 *   - first call creates them (the video subsystem comes up lazily
 *     here, so --ppm and other headless paths never touch SDL);
 *   - later calls RECONFIGURE the same window (title, logical
 *     viewport, default window size when not fullscreen), so switching
 *     scenes swaps content instead of tearing the window down;
 *   - a window killed by an external SDL_Quit() is detected
 *     (SDL_WasInit) and re-created transparently.
 * ll_view_release() is what a screen calls when it is done: the window
 * stays up for the next screen. Screens own and free their textures.
 *
 * Review P-04 (focus stealing): the window is raised once at
 * (re)configuration; screens must NOT poll focus every tick.
 * ll_view_handle_event() re-raises only when the pointer re-enters
 * the window - clear user intent - and never while the user has
 * alt-tabbed away.
 *
 * Review P-10 / tips T-12, T-13:
 *   - logical-size presentation: the renderer owns a logical viewport
 *     of the EXACT content size (320x200 / 640x400 / level view), the
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

/* Obtain this screen's view: first call creates the session window,
 * later calls reconfigure it (see header). Installs the logical
 * viewport and nearest scaling. Returns 0, -1 on failure (nothing to
 * release then). */
int ll_view_create(ll_view *v, const char *title, int logical_w,
                   int logical_h);

/* Screen finished: keeps the shared window up for the next screen
 * (P-03). Does NOT free the screen's own texture - the screen owns
 * those. */
void ll_view_release(const ll_view *v);

/* Feed EVERY event here before the screen's own handling: F11 or
 * Alt+Enter toggles desktop-fullscreen (event consumed, returns 1);
 * WINDOWEVENT_ENTER re-raises focus (P-04, returns 0). */
int ll_view_handle_event(const SDL_Event *e, SDL_Window *win);

/* Tear the session down explicitly (tools / reset paths). SDL_Quit()
 * is also detected and healed, see header. */
void ll_view_session_close(void);

#endif /* LL_VIEW_H */
