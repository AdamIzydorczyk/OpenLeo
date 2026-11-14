/* ll_view.c - shared session window (see ll_view.h). One window + one
 * renderer for the whole program (review P-03): screens get view
 * handles from ll_view_create; the first call builds the window, every
 * later call reconfigures it in place. */
#include "ll_view.h"
#include "platform.h"  /* plat_opt_* declarations (defined here) */
#include <string.h>

/* The initial window multiple / fullscreen overrides (plat_opt_*) are
 * declared by platform.h and defined in this file (backend-side
 * consumption, P-10). */
int plat_opt_scale;
int plat_opt_fullscreen;

static ll_view g;                  /* the one window (P-03) */

static int auto_scale(int lw)
{
    return lw < 640 ? 2 : 1;      /* the historical fixed geometry */
}

int ll_view_create(ll_view *v, const char *title, int logical_w,
                   int logical_h)
{
    int sc = plat_opt_scale > 0 ? plat_opt_scale : auto_scale(logical_w);
    memset(v, 0, sizeof *v);
    if (g.win && !SDL_WasInit(SDL_INIT_VIDEO))
        memset(&g, 0, sizeof g);  /* external SDL_Quit(): window is gone */

    if (!g.win) {
        /* T-12: nearest neighbour everywhere (pixel art; also the right
         * choice under fractional HiDPI scale factors). The hint must be
         * set before window/renderer creation, where SDL latches it; the
         * renderer is then reused for every screen. */
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
        Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE |
                       SDL_WINDOW_ALLOW_HIGHDPI;
        g.win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED,
                                 logical_w * sc, logical_h * sc, flags);
        if (!g.win)
            return -1;
        if (plat_opt_fullscreen)
            SDL_SetWindowFullscreen(g.win, SDL_WINDOW_FULLSCREEN_DESKTOP);
        g.ren = SDL_CreateRenderer(g.win, -1, SDL_RENDERER_ACCELERATED);
        if (!g.ren)
            g.ren = SDL_CreateRenderer(g.win, -1, SDL_RENDERER_SOFTWARE);
        if (!g.ren) {
            SDL_DestroyWindow(g.win);
            memset(&g, 0, sizeof g);
            return -1;
        }
    } else {
        /* scene switch in the SAME window (P-03): new title + default
         * geometry; in desktop-fullscreen the display mode rules.
         * Raised once here; per-tick focus polling is gone (P-04). */
        SDL_SetWindowTitle(g.win, title);
        if (!(SDL_GetWindowFlags(g.win) & SDL_WINDOW_FULLSCREEN))
            SDL_SetWindowSize(g.win, logical_w * sc, logical_h * sc);
        SDL_RaiseWindow(g.win);
    }
    SDL_RenderSetLogicalSize(g.ren, logical_w, logical_h);
    g.logical_w = logical_w;
    g.logical_h = logical_h;
    *v = g;
    return 0;
}

void ll_view_release(const ll_view *v)
{
    (void)v;                      /* the window stays up (P-03) */
}

void ll_view_session_close(void)
{
    if (g.ren)
        SDL_DestroyRenderer(g.ren);
    if (g.win)
        SDL_DestroyWindow(g.win);
    memset(&g, 0, sizeof g);
}

int ll_view_handle_event(const SDL_Event *e, SDL_Window *win)
{
    if (!win)
        return 0;
    if (e->type == SDL_WINDOWEVENT &&
        e->window.windowID == SDL_GetWindowID(win)) {
        /* P-04: re-grab focus only when the pointer re-enters the
         * window (user came back); never while the user is alt-tabbed
         * away. Replaces the old per-tick SDL_RaiseWindow poll. */
        if (e->window.event == SDL_WINDOWEVENT_ENTER)
            SDL_RaiseWindow(win);
        return 0;
    }
    if (e->type != SDL_KEYDOWN)
        return 0;
    SDL_Keycode k = e->key.keysym.sym;
    int alt = (e->key.keysym.mod & (KMOD_LALT | KMOD_RALT)) != 0;
    if (k != SDLK_F11 && !(k == SDLK_RETURN && alt))
        return 0;
    int on = (SDL_GetWindowFlags(win) & SDL_WINDOW_FULLSCREEN) != 0;
    SDL_SetWindowFullscreen(win, on ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
    return 1;
}
