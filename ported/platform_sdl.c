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

int plat_poll(ll_event *ev)
{
    if (!ev)
        return 0;
    if (v.qh == v.qt) {
        /* Translate one SDL event at a time (same pacing as the old
         * per-screen SDL_PollEvent loops). */
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            /* Window-management keys (F11/Alt+Enter fullscreen, P-10)
             * and focus policy (P-04) are consumed here and never
             * reach the engine. */
            if (ll_view_handle_event(&e, v.view.win))
                continue;
            if (e.type == SDL_QUIT) {
                ll_event o = { LL_EV_QUIT, 0, 0, 0 };
                if (((v.qt + 1) % (sizeof v.q / sizeof v.q[0])) == v.qh)
                    return 0;     /* ring full: retry after consumer drains */
                v.q[v.qt++] = o;
                v.qt %= (sizeof v.q / sizeof v.q[0]);
                break;
            }
            if (e.type == SDL_KEYDOWN) {
                SDL_Keycode k = e.key.keysym.sym;
                int mapped = 0;
                if (k >= 32 && k < 127)
                    mapped = (int)k;       /* printable: ASCII verbatim */
                else if (k == SDLK_ESCAPE)
                    mapped = LLKEY_ESC;
                if (!mapped)
                    continue;              /* engine ignores other keys */
                ll_event o = { LL_EV_KEYDOWN, mapped, 0, 0 };
                if (((v.qt + 1) % (sizeof v.q / sizeof v.q[0])) == v.qh)
                    return 0;
                v.q[v.qt++] = o;
                v.qt %= (sizeof v.q / sizeof v.q[0]);
                break;
            }
            /* key-up and all other event types are dropped: the engine
             * input model is keypress-based (T-15), exactly like the
             * old per-screen loops that only checked KEYDOWN/QUIT. */
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

#endif /* !PLAT_BACKEND_DUMMY */
