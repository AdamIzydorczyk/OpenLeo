/* platform_sdl.c - the real platform backend (platform.h, tips T-01).
 *
 * A THIN delegation layer, by design: everything session-shaped already
 * exists and is battle-tested -
 *   video:  ll_view session window/renderer (PR-8/10, P-03/P-04/P-10),
 *   audio:  ll_audio session + per-playback device (PR-12, P-13 spec
 *           logging and the samples=0/queue-only contract in one place),
 *   files:  fs_ci (platform_common.c -> plat_fopen).
 * This file only adds what a per-screen consumer used to own: the
 * streaming texture + indexed->ARGB blit (moved verbatim from
 * is_player's present(), review P-12: no per-frame allocation), the
 * SDL->ll_event input mapping (P-10 fullscreen keys consumed here and
 * never surfaced), and the ns clock. No new session semantics.
 *
 * Compiled when PLAT_BACKEND_SDL=1 (the default for game targets, see
 * the Makefile/CMakeLists). platform_dummy.c is the headless twin used
 * by tests; plat_fopen lives in platform_common.c for both.
 *
 * The two backends switch on the same macro (PLAT_BACKEND_DUMMY): this
 * file compiles unless the dummy backend is selected, so a target that
 * globs its sources still links exactly one backend. */
#include "platform.h"

#if !(defined(PLAT_BACKEND_DUMMY) && PLAT_BACKEND_DUMMY)
#include "ll_audio.h"
#include "ll_view.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>

static struct {
    ll_view        view;          /* session window/renderer (P-03)     */
    SDL_Texture   *tex;           /* this backend owns its texture      */
    int            tex_w, tex_h;
    SDL_AudioDeviceID adev;       /* 0 = no device (per-playback, P-13) */
    ll_event       q[64];
    unsigned       qh, qt;        /* event ring: [qh, qt)               */
} v;

int plat_init(const char *title, int w, int h)
{
    if (w <= 0 || h <= 0)
        return -1;
    /* ll_view_create: first call creates the session window, later
     * calls reconfigure title + logical viewport (P-03/P-10). */
    if (ll_view_create(&v.view, title, w, h) != 0)
        return -1;
    if (v.tex && (v.tex_w != w || v.tex_h != h)) {
        SDL_DestroyTexture(v.tex);
        v.tex = NULL;
    }
    if (!v.tex)
        v.tex = SDL_CreateTexture(v.view.ren, SDL_PIXELFORMAT_ARGB8888,
                                  SDL_TEXTUREACCESS_STREAMING, w, h);
    if (!v.tex)
        return -2;                /* window ok, texture failed */
    v.tex_w = w;
    v.tex_h = h;
    return 0;
}

void plat_present(const uint8_t *idx8, const uint8_t pal[768], int w, int h)
{
    if (!v.tex || !idx8 || !pal || w <= 0 || h <= 0)
        return;
    /* Presentation fast path (proposal 2. in IMPROVEMENTS / P-12):
     * precomputed 256-entry palette LUT written straight into the
     * locked ARGB8888 texture. Palette expansion (v<<2) is the live
     * DAC model (is_palette_rgb8); the caller has already LINEARISED
     * the framebuffer (is_linearize_into) - same order as is_stream's
     * own PPM writer, so pixels are bit-for-bit what the player drew
     * before this file existed. */
    uint8_t rgb[768];
    for (int i = 0; i < 768; i++)
        rgb[i] = (uint8_t)(pal[i] << 2);
    Uint32 lut[256];
    for (int i = 0; i < 256; i++)
        lut[i] = 0xFF000000u | ((Uint32)rgb[3 * i] << 16)
                 | ((Uint32)rgb[3 * i + 1] << 8) | (Uint32)rgb[3 * i + 2];
    void *px;
    int pitch;
    if (SDL_LockTexture(v.tex, NULL, &px, &pitch) == 0) {
        for (int y = 0; y < h; y++) {
            Uint32 *row = (Uint32 *)((uint8_t *)px + (size_t)y * pitch);
            const uint8_t *src = idx8 + (size_t)y * w;
            for (int x = 0; x < w; x++)
                row[x] = lut[src[x]];
        }
        SDL_UnlockTexture(v.tex);
    }
    SDL_RenderClear(v.view.ren);
    SDL_RenderCopy(v.view.ren, v.tex, NULL, NULL);
    SDL_RenderPresent(v.view.ren);
}

void plat_present_argb(const uint32_t *fb, int w, int h)
{
    if (!v.tex || !fb || w != v.tex_w || h != v.tex_h)
        return;                 /* caller must plat_init() for this size */
    /* Row-for-row upload of the already-composited frame (verbatim the
     * old ll_menu/ll_level upload loop): no conversion, so the pixels
     * shown are exactly the ones the screen computed (its PPM hooks
     * read the very same fb). */
    void *px;
    int pitch;
    if (SDL_LockTexture(v.tex, NULL, &px, &pitch) == 0) {
        for (int y = 0; y < h; y++)
            memcpy((char *)px + (size_t)y * pitch,
                   (const char *)fb + (size_t)y * w * 4, (size_t)w * 4);
        SDL_UnlockTexture(v.tex);
    }
    SDL_RenderClear(v.view.ren);
    SDL_RenderCopy(v.view.ren, v.tex, NULL, NULL);
    SDL_RenderPresent(v.view.ren);
}

void plat_raise(void)
{
    if (!v.view.win)
        return;
    SDL_RaiseWindow(v.view.win);
    /* X11/XWayland hard input grab; the pure Wayland backend ignores
     * it (WM decides) - callers keep input handling tolerant (P-04). */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    SDL_SetWindowInputFocus(v.view.win);
#pragma GCC diagnostic pop
}

/* SDL keysym -> engine key id (T-15: printables stay ASCII verbatim,
 * navigation keys get LLKEY_* ids, everything else is not for the
 * engine). Returns 0 for "ignore". */
static int map_key(SDL_Keycode k)
{
    if (k >= 32 && k < 127)
        return (int)k;
    switch (k) {
    case SDLK_ESCAPE:                     return LLKEY_ESC;
    case SDLK_RETURN: case SDLK_KP_ENTER: return LLKEY_RETURN;
    case SDLK_UP:                         return LLKEY_UP;
    case SDLK_DOWN:                       return LLKEY_DOWN;
    case SDLK_LEFT:                       return LLKEY_LEFT;
    case SDLK_RIGHT:                      return LLKEY_RIGHT;
    case SDLK_HOME:                       return LLKEY_HOME;
    case SDLK_END:                        return LLKEY_END;
    case SDLK_PAGEUP:                     return LLKEY_PGUP;
    case SDLK_PAGEDOWN:                   return LLKEY_PGDN;
    default:                              return 0;
    }
}

static void qpush(const ll_event *o)
{
    unsigned n = (unsigned)(sizeof v.q / sizeof v.q[0]);
    unsigned next = (v.qt + 1) % n;
    if (next == v.qh)
        return;                 /* 64 deep: far more than one frame */
    v.q[v.qt] = *o;
    v.qt = next;
}

int plat_poll(ll_event *ev)
{
    if (!ev)
        return 0;
    if (v.qh == v.qt) {
        /* Translate SDL events until one maps to an engine event (same
         * per-frame drain contract as the old screens' SDL_PollEvent
         * loops). Window-management keys (F11/Alt+Enter, P-10) and
         * focus policy (P-04) are consumed by ll_view_handle_event and
         * never surfaced. Mouse x,y pass through as LOGICAL viewport
         * coordinates: SDL already un-scaled them for us (P-10). */
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (ll_view_handle_event(&e, v.view.win))
                continue;
            ll_event o = { LL_EV_NONE, 0, -1, -1 };
            if (e.type == SDL_QUIT) {
                o.type = LL_EV_QUIT;
            } else if (e.type == SDL_KEYDOWN) {
                o.key = map_key(e.key.keysym.sym);
                if (!o.key)
                    continue;
                o.type = LL_EV_KEYDOWN;
            } else if (e.type == SDL_MOUSEBUTTONDOWN
                       && e.button.button == SDL_BUTTON_LEFT) {
                o.type = LL_EV_BUTTON_DOWN;
                o.key = LLBTN_LEFT;
                o.x = e.button.x;
                o.y = e.button.y;
            } else if (e.type == SDL_MOUSEMOTION) {
                o.type = LL_EV_MOTION;
                o.x = e.motion.x;
                o.y = e.motion.y;
            } else {
                /* key-up and everything else: the engine input model
                 * is press-based (T-15), exactly like the old pumps. */
                continue;
            }
            qpush(&o);
            break;
        }
    }
    if (v.qh == v.qt)
        return 0;
    *ev = v.q[v.qh];
    v.qh = (v.qh + 1) % (sizeof v.q / sizeof v.q[0]);
    return 1;
}

/* Monotonic ns clock - the ONE source the engine derives pacing from
 * (tips T-07); same conversion the player used before this file. */
uint64_t plat_now_ns(void)
{
    return (uint64_t)(((double)SDL_GetPerformanceCounter())
                      * (1e9 / (double)SDL_GetPerformanceFrequency()));
}

void plat_sleep_until(uint64_t ns)
{
    /* Same 2 ms spin-window as the old is_player wait_until: far away
     * sleeps coarsely (cheap), near deadlines poll tightly (frame
     * accuracy), and neither ever overshoots the deadline. */
    static const uint64_t SLACK = 2000000ull;
    for (;;) {
        uint64_t now = plat_now_ns();
        if (now >= ns)
            return;
        if (ns - now > SLACK)
            SDL_Delay(2);
        else
            SDL_Delay(0);
    }
}

int plat_audio_open(int rate_hz, int channels)
{
    if (v.adev || rate_hz <= 0 || (channels != 1 && channels != 2))
        return -1;
    if (ll_audio_session_init() != 0)
        return -1;                /* reason via plat_last_error */
    SDL_AudioSpec want;
    memset(&want, 0, sizeof want);
    want.freq = rate_hz;
    want.format = AUDIO_S16SYS;   /* s16 interleaved, engine-mixed */
    want.channels = (Uint8)channels;
    /* samples=0 + queue-only contract + accepted-spec log (P-13):
     * enforced inside ll_audio_open. */
    v.adev = ll_audio_open(&want);
    if (!v.adev)
        return -2;                /* subsystem ok, device failed */
    SDL_PauseAudioDevice(v.adev, 0);   /* playing; engine feeds bytes */
    return 0;
}

void plat_audio_close(void)
{
    if (!v.adev)
        return;
    ll_audio_close(v.adev);       /* + ClearError, ll_audio.h */
    v.adev = 0;
}

void plat_audio_queue(const void *bytes, size_t len)
{
    if (v.adev && bytes && len)
        SDL_QueueAudio(v.adev, bytes, (Uint32)len);
}

size_t plat_audio_queued_bytes(void)
{
    return v.adev ? (size_t)SDL_GetQueuedAudioSize(v.adev) : 0;
}

void plat_audio_pause(int paused)
{
    if (v.adev)
        SDL_PauseAudioDevice(v.adev, paused ? 1 : 0);
}

void plat_audio_clear(void)
{
    if (v.adev)
        SDL_ClearQueuedAudio(v.adev);
}

const char *plat_last_error(void)
{
    return SDL_GetError();
}

/* ---- platform worker thread (review E-09: music pump) ---- */
static SDL_Thread *s_thr;

int plat_thread_start(int (*fn)(void *), void *ctx, const char *name)
{
    if (s_thr || !fn)
        return -1;
    /* Exactly the SDL_CreateThread call music used to make itself:
     * same SDLCALL ABI (the cast is a no-op on both our targets), so
     * the E-09 thread semantics and the failure diagnostic
     * (SDL_GetError via plat_last_error) are unchanged. */
    s_thr = SDL_CreateThread((SDL_ThreadFunction)fn,
                             name && *name ? name : "ll_plat", ctx);
    if (!s_thr)
        return -1;
    return 0;
}

void plat_thread_join(void)
{
    if (!s_thr)
        return;
    SDL_WaitThread(s_thr, NULL);
    s_thr = NULL;
}

/* ---- host glue (see platform.h contract) ---- */

char *plat_base_path(void)
{
    /* Exactly what main.c used to call: works before any SDL_Init,
     * may legitimately fail. Copied out so the caller can plain-free
     * it regardless of SDL's allocator. */
    char *p = SDL_GetBasePath();
    if (!p)
        return NULL;
    char *r = strdup(p);
    SDL_free(p);
    return r;
}

void plat_message_box(const char *title, const char *text)
{
#if defined(PLATFORM_WIN) && PLATFORM_WIN
    /* P-06: a Windows double-click launch has no console; on POSIX
     * the caller's stderr line is the report and a box would only
     * block the shell (the policy that lived in main.c). */
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, text, NULL);
#else
    (void)title;
    (void)text;
#endif
}

int plat_pref_dir(char *out, size_t cap)
{
    if (!out || cap < 2)
        return -1;
    char *p = SDL_GetPrefPath("LongSoft", "LewLeon");  /* creates the dir */
    if (!p)
        return -1;
    snprintf(out, cap, "%s", p);
    SDL_free(p);
    return 0;
}

void plat_prepare_video(void)
{
    /* Verbatim move from main.c (T-01 phase 5): same conditions, same
     * byte-identical diagnostics ("main: ..." prefixes kept - users
     * and the docs quote them). Wayland sessions: SDL's wayland
     * windows can NEVER get keyboard focus (WM policy), so pin X11
     * (XWayland) where SDL_RaiseWindow + SetWindowInputFocus work;
     * fall back to the default if no X server. An explicit
     * SDL_VIDEODRIVER always wins. */
    if (!SDL_getenv("SDL_VIDEODRIVER") && SDL_getenv("WAYLAND_DISPLAY")) {
        SDL_setenv("SDL_VIDEODRIVER", "x11", 1);
        int ok = SDL_InitSubSystem(SDL_INIT_VIDEO) == 0;
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        if (ok)
            printf("main: Wayland session detected: using X11/XWayland "
                   "video backend (window keyboard focus works there)\n");
        else {
            printf("main: no X server under Wayland (%s): keeping default "
                   "backend (keyboard focus may not follow windows)\n",
                   SDL_GetError());
            SDL_setenv("SDL_VIDEODRIVER", "wayland", 1);
        }
    }
}

#endif /* !PLAT_BACKEND_DUMMY */
