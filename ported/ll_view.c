/* ll_view.c - shared window/renderer creation (see ll_view.h). */
#include "ll_view.h"

int ll_opt_scale;
int ll_opt_fullscreen;

static int auto_scale(int lw)
{
    return lw < 640 ? 2 : 1;      /* the historical fixed geometry */
}

int ll_view_create(ll_view *v, const char *title, int logical_w,
                   int logical_h)
{
    int sc = ll_opt_scale > 0 ? ll_opt_scale : auto_scale(logical_w);
    Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE |
                   SDL_WINDOW_ALLOW_HIGHDPI;
    v->win = NULL;
    v->ren = NULL;
    v->logical_w = logical_w;
    v->logical_h = logical_h;
    v->win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, logical_w * sc,
                              logical_h * sc, flags);
    if (!v->win)
        return -1;
    if (ll_opt_fullscreen)
        SDL_SetWindowFullscreen(v->win, SDL_WINDOW_FULLSCREEN_DESKTOP);
    /* T-12: nearest neighbour everywhere (pixel art; also the right
     * choice under fractional HiDPI scale factors). Set before the
     * renderer is created, where SDL latches it. */
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    v->ren = SDL_CreateRenderer(v->win, -1, SDL_RENDERER_ACCELERATED);
    if (!v->ren)
        v->ren = SDL_CreateRenderer(v->win, -1, SDL_RENDERER_SOFTWARE);
    if (!v->ren) {
        SDL_DestroyWindow(v->win);
        v->win = NULL;
        return -1;
    }
    SDL_RenderSetLogicalSize(v->ren, logical_w, logical_h);
    return 0;
}

int ll_view_fullscreen_key(const SDL_Event *e, SDL_Window *win)
{
    if (!win || e->type != SDL_KEYDOWN)
        return 0;
    SDL_Keycode k = e->key.keysym.sym;
    int alt = (e->key.keysym.mod & (KMOD_LALT | KMOD_RALT)) != 0;
    if (k != SDLK_F11 && !(k == SDLK_RETURN && alt))
        return 0;
    int on = (SDL_GetWindowFlags(win) & SDL_WINDOW_FULLSCREEN) != 0;
    SDL_SetWindowFullscreen(win, on ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
    return 1;
}

void ll_view_destroy(ll_view *v)
{
    if (v->ren)
        SDL_DestroyRenderer(v->ren);
    if (v->win)
        SDL_DestroyWindow(v->win);
    v->ren = NULL;
    v->win = NULL;
}
