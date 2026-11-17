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
    SDL_AudioDeviceID adev;       /* FIX-09 Part 2 (X-10): ONE session
                                   * device, opened at its native rate on
                                   * first use, never closed per scene */
    SDL_AudioSpec dev_have;       /* the accepted spec of that device */
    SDL_AudioStream *as;          /* per-playback source->device stream */
    size_t src_bps, dev_bps;      /* byte rates: source, device side */
    int logical_open;             /* a stream is open (the old "device
                                   * open" the plat_audio_* contract sees) */
    ll_event       q[64];
    unsigned       qh, qt;        /* event ring: [qh, qt)               */
    int            quit_req;      /* FIX-01: sticky app-quit (X-01)     */
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

/* FIX-04 (deep dive X-05): re-show the backend's CURRENT texture - the
 * last presented frame - without touching a single pixel. The window
 * system invalidates the surface on expose/restore/resize and a fullscreen
 * toggle swaps the surface outright; while the engine is idle (pause,
 * audio drain, 2 s end-hold) nothing presents, and the window would stay
 * black or show garbage. Pixels are unchanged by construction. */
static void present_last(void)
{
    if (!v.tex || !v.view.ren)
        return;
    SDL_RenderClear(v.view.ren);
    SDL_RenderCopy(v.view.ren, v.tex, NULL, NULL);
    SDL_RenderPresent(v.view.ren);
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
    present_last();   /* the shared upload->clear->copy->flip tail (FIX-04) */
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
    present_last();   /* the shared upload->clear->copy->flip tail (FIX-04) */
}

void plat_raise(void)
{
    /* FIX-02 (deep dive X-03): API no-op. The window is raised EXACTLY
     * once at creation (ll_view_create); every screen re-entry happens
     * in that SAME window, so raising again here could only steal focus
     * from whatever the user switched to - and SDL_SetWindowInputFocus
     * was a hard X11 grab that flashed the Windows taskbar. Callers
     * (screens, selftest) stay; the request is simply ignored. */
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
    /* PR-87 (play-mode fire family). Both Ctrls, both Alts. */
    case SDLK_LCTRL: case SDLK_RCTRL:     return LLKEY_LCTRL;
    case SDLK_LALT:  case SDLK_RALT:      return LLKEY_LALT;
    case SDLK_F1:                         return LLKEY_F1;
    case SDLK_F2:                         return LLKEY_F2;
    case SDLK_F3:                         return LLKEY_F3;
    case SDLK_F4:                         return LLKEY_F4;
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
    /* FIX-01 automated test hooks (additive; default off = shipped
     * behaviour unchanged):
     * LEO_QUIT_AFTER_MS=N injects a window close N ms after the FIRST
     * poll (not process start) - lets a headless leg prove the whole
     * quit-unwind path: main unwinds, no further scene opens.
     * LEO_TEST_KEY_AFTER_MS=N + LEO_TEST_KEY=<k> injects one KEYDOWN
     * N ms after the first poll, pushed through SDL's own queue so it
     * travels the exact same translation path (map_key, repeat filter)
     * as a physical press. <k> = a printable ASCII char (matched
     * case-insensitively, 'q' for the FIX-01/C3 q-quits-the-app leg),
     * or the names "esc" / "space" / "left" / "right" / "up" / "down"
     * (the arrows: PR-43's live-play leg needs a HELD-key arrow, which
     * the .des-driven LEO_PLAY_SCRIPT path does not exercise), or
     * "ctrl" / "alt" / "f1".."f4" (PR-87's fire-family keys).
     * LEO_TEST_KEY_UP_AFTER_MS=M (PR-83) additionally pushes the
     * matching SDL_KEYUP for the same key M ms after the first poll -
     * the held-then-released path a real hand produces. */
    {
        static uint64_t t_first;
        static uint64_t nms, kup_ms;
        static int fired = 0, kup_fired = 0, armed = -1;
        static SDL_Keycode tkey;
        if (armed < 0) {
            const char *qam = getenv("LEO_QUIT_AFTER_MS");
            nms = qam ? (uint64_t)atof(qam) : 0;
            armed = nms > 0;
            const char *kam = getenv("LEO_TEST_KEY_AFTER_MS");
            const char *knm = getenv("LEO_TEST_KEY");
            const char *kum = getenv("LEO_TEST_KEY_UP_AFTER_MS");
            uint64_t kn = kam ? (uint64_t)atof(kam) : 0;
            kup_ms = kum ? (uint64_t)atof(kum) : 0;
            if (kn > 0 && knm && knm[0]) {
                if (!strcmp(knm, "esc"))
                    tkey = SDLK_ESCAPE;
                else if (!strcmp(knm, "space"))
                    tkey = ' ';
                else if (!strcmp(knm, "left"))
                    tkey = SDLK_LEFT;
                else if (!strcmp(knm, "right"))
                    tkey = SDLK_RIGHT;
                else if (!strcmp(knm, "up"))
                    tkey = SDLK_UP;
                else if (!strcmp(knm, "down"))
                    tkey = SDLK_DOWN;
                else if (!strcmp(knm, "ctrl"))       /* PR-87: the fire */
                    tkey = SDLK_LCTRL;                /* key leg         */
                else if (!strcmp(knm, "alt"))
                    tkey = SDLK_LALT;
                else if (!strcmp(knm, "f1"))
                    tkey = SDLK_F1;
                else if (!strcmp(knm, "f2"))
                    tkey = SDLK_F2;
                else if (!strcmp(knm, "f3"))
                    tkey = SDLK_F3;
                else if (!strcmp(knm, "f4"))
                    tkey = SDLK_F4;
                else {
                    int c = (unsigned char)knm[0];
                    if (c >= 'A' && c <= 'Z')
                        c += 'a' - 'A';      /* map_key is case-exact */
                    tkey = (SDL_Keycode)c;
                }
                if (!armed) {               /* share the first-poll clock */
                    nms = kn;
                    armed = 1;
                }
            }
        }
        if (armed) {
            if (!t_first)
                t_first = (uint64_t)SDL_GetTicks();
            if (!fired && (uint64_t)SDL_GetTicks() >= t_first + nms) {
                SDL_Event x;
                SDL_memset(&x, 0, sizeof x);
                if (tkey) {
                    x.type = SDL_KEYDOWN;
                    x.key.keysym.sym = tkey;
                    x.key.repeat = 0;
                } else
                    x.type = SDL_QUIT;
                SDL_PushEvent(&x);
                fired = 1;
            }
            if (tkey && kup_ms && !kup_fired &&
                (uint64_t)SDL_GetTicks() >= t_first + kup_ms) {
                SDL_Event x;
                SDL_memset(&x, 0, sizeof x);
                x.type = SDL_KEYUP;
                x.key.keysym.sym = tkey;
                x.key.repeat = 0;
                SDL_PushEvent(&x);
                kup_fired = 1;
            }
        }
    }
    if (v.qh == v.qt) {
        /* Translate SDL events until one maps to an engine event (same
         * per-frame drain contract as the old screens' SDL_PollEvent
         * loops). Window-management keys (F11/Alt+Enter, P-10) and
         * focus policy (P-04) are consumed by ll_view_handle_event and
         * never surfaced. Mouse x,y pass through as LOGICAL viewport
         * coordinates: SDL already un-scaled them for us (P-10). */
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (ll_view_handle_event(&e, v.view.win)) {
                present_last();  /* FIX-04 (X-05): F11/Alt+Enter swapped
                                  * the surface (fullscreen toggle) - the
                                  * current frame must come straight back
                                  * even while the engine is idle */
                continue;
            }
            ll_event o = { LL_EV_NONE, 0, -1, -1, 0 };
            if (e.type == SDL_QUIT) {
                o.type = LL_EV_QUIT;
                v.quit_req = 1;             /* FIX-01: sticky app quit */
            } else if (e.type == SDL_KEYDOWN) {
                o.key = map_key(e.key.keysym.sym);
                if (!o.key)
                    continue;
                o.type = LL_EV_KEYDOWN;
                o.repeat = e.key.repeat != 0;   /* FIX-03 (X-04): the
                     * OS auto-repeat flag rides along; consumers decide
                     * per key (toggle/confirm keys act first-press only) */
            } else if (e.type == SDL_KEYUP) {
                /* PR-83 (user-reported): the KEYUP half of the pair was
                 * never translated - LL_EV_KEYUP existed in the contract
                 * but no backend ever emitted it, so a held-key consumer
                 * (the play-mode arrows) latched ON forever: one press
                 * kept "pressing in a loop". Same map_key filter as the
                 * KEYDOWN path: an unmapped key releases nothing it never
                 * pressed. */
                o.key = map_key(e.key.keysym.sym);
                if (!o.key)
                    continue;
                o.type = LL_EV_KEYUP;
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
            } else if (e.type == SDL_WINDOWEVENT) {
                /* Window bookkeeping, never surfaced to the engine:
                 * CLOSE latches the FIX-01 sticky quit (belt-and-braces -
                 * SDL posts SDL_QUIT alongside today, but a WM close must
                 * latch even if that pairing changes); EXPOSED /
                 * SIZE_CHANGED / RESTORED mean the surface was
                 * invalidated while the engine may be idle (pause, audio
                 * drain, end-hold) - re-show the current texture, pixels
                 * unchanged (FIX-04, X-05). Everything else (incl.
                 * ENTER: FIX-02 raised-once policy) is ignored. */
                switch (e.window.event) {
                case SDL_WINDOWEVENT_CLOSE:
                    v.quit_req = 1;
                    break;
                case SDL_WINDOWEVENT_EXPOSED:
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                case SDL_WINDOWEVENT_RESTORED:
                    present_last();
                    break;
                default:
                    break;
                }
                continue;
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

/* FIX-01: sticky quit accessors (platform.h contract). */
int plat_quit_requested(void) { return v.quit_req; }
void plat_request_quit(void)  { v.quit_req = 1; }

/* Monotonic ns clock - the ONE source the engine derives pacing from
 * (tips T-07). The old double-based conversion (the player's original)
 * is gone; see FIX-06 inside. */
uint64_t plat_now_ns(void)
{
    /* FIX-06 (deep dive X-06): exact integer conversion - the old
     * double path lost ns precision above 2^53 counters. Safe for any
     * frequency up to ~1.8e10 (Windows QPC = 1e7, Linux clock = 1e9):
     * c%f < f, so (c%f)*1e9 < f*1e9 must stay < UINT64_MAX = 1.8e19. */
    uint64_t f = SDL_GetPerformanceFrequency();
    uint64_t c = SDL_GetPerformanceCounter();
    return (c / f) * 1000000000ull + (c % f) * 1000000000ull / f;
}

#if defined(_WIN32)
#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif
#include <windows.h>
/* FIX-06 (X-09): Win10 1803+ high-resolution waitable timer (~0.5 ms
 * accuracy). The handle is per-call, so this is thread-safe - the ll_level
 * music thread sleeps here too (ll_level.c). Older Windows falls back to
 * SDL_Delay, whose ~1 ms period SDL2 already improves via timeBeginPeriod. */
static void os_sleep_ns(uint64_t ns)
{
    HANDLE t = CreateWaitableTimerExW(NULL, NULL,
                   CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
    if (t) {
        LARGE_INTEGER due;
        due.QuadPart = -(LONGLONG)(ns / 100ull);   /* relative, 100 ns units */
        if (SetWaitableTimer(t, &due, 0, NULL, NULL, FALSE))
            WaitForSingleObject(t, INFINITE);
        CloseHandle(t);
        return;
    }
    SDL_Delay((Uint32)(ns / 1000000ull));
}
#else
#include <time.h>
static void os_sleep_ns(uint64_t ns)
{
    struct timespec ts;
    ts.tv_sec = (time_t)(ns / 1000000000ull);
    ts.tv_nsec = (long)(ns % 1000000000ull);
    nanosleep(&ts, NULL);        /* EINTR: the caller loop re-checks */
}
#endif

void plat_sleep_until(uint64_t ns)
{
    /* FIX-06 (deep dive X-09): sleep IN THE OS until 0.3 ms before the
     * deadline, then yield-spin to hit it tightly. The old form spun
     * SDL_Delay(0) for a fixed 2 ms window of every 20-30 ms tick
     * (~8-10% of a core on an idle screen); the 300 us window is ~7x
     * cheaper and frame accuracy is unchanged (platform.h contract:
     * never returns before the deadline). */
    static const uint64_t SPIN = 300000ull;
    for (;;) {
        uint64_t now = plat_now_ns();
        if (now >= ns)
            return;
        uint64_t left = ns - now;
        if (left > SPIN)
            os_sleep_ns(left - SPIN);
        else
            SDL_Delay(0);
    }
}

int plat_audio_open(int rate_hz, int channels)
{
    /* FIX-09 Part 2 (deep dive X-10): ONE device for the whole process,
     * opened at the DEVICE's native rate on first use; every playback is
     * a logical stream whose bytes SDL_AudioStream converts to the
     * device spec (no more 22050 Hz device opens: WASAPI/Bluetooth cost
     * 50-300 ms per reopen and pop). The plat_audio_* contract the
     * engine sees is UNCHANGED: one stream at a time (-1 while one is
     * open, as the old code returned with a device held), queue/
     * pause/clear as before, and queued_bytes still counts in the
     * CALLER's source format (ll_level low-water + the FIX-08 A/V clock
     * keep reading their own bytes). A stream close clears every queued
     * byte, so no scene tail leaks into the next one.
     * Diagnostics contract: 'll_audio: device ...' now appears EXACTLY
     * once per process (the device is opened once) - the acceptance
     * line for this fix. */
    if (v.logical_open) {
        SDL_SetError("plat_audio_open: stream already open");
        return -1;
    }
    if (rate_hz <= 0 || (channels != 1 && channels != 2))
        return -1;
    if (!v.adev) {
        if (ll_audio_session_init() != 0)
            return -1;                /* reason via plat_last_error */
        SDL_AudioSpec want;
        memset(&want, 0, sizeof want);
        want.freq = 48000;            /* a REQUEST: FREQUENCY_CHANGE lets
                                       * the backend answer its native rate */
        want.format = AUDIO_S16SYS;
        want.channels = 2;
        /* samples=0 + queue-only contract + accepted-spec log (P-13):
         * enforced inside ll_audio_open. */
        v.adev = ll_audio_open(&want, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE,
                               &v.dev_have);
        if (!v.adev)
            return -2;                /* subsystem ok, device failed */
    }
    v.as = SDL_NewAudioStream(AUDIO_S16SYS, (Uint8)channels, rate_hz,
                              v.dev_have.format, v.dev_have.channels,
                              v.dev_have.freq);
    if (!v.as)
        return -2;                    /* subsystem+device fine, stream died */
    v.src_bps = (size_t)rate_hz * (size_t)channels * 2u;
    v.dev_bps = (size_t)v.dev_have.freq * (size_t)v.dev_have.channels
              * (size_t)(SDL_AUDIO_BITSIZE(v.dev_have.format) / 8u);
    v.logical_open = 1;
    SDL_PauseAudioDevice(v.adev, 0);  /* playing; engine feeds bytes */
    return 0;
}

void plat_audio_close(void)
{
    if (!v.logical_open)
        return;
    if (v.as) {
        SDL_FreeAudioStream(v.as);
        v.as = NULL;
    }
    /* Fresh start for the next stream: drop every queued byte, then hold
     * the device silent. The DEVICE STAYS OPEN (FIX-09 Part 2) - the
     * per-scene close/reopen with its WASAPI latency and pop is gone. */
    SDL_ClearQueuedAudio(v.adev);
    SDL_PauseAudioDevice(v.adev, 1);
    v.logical_open = 0;
    v.src_bps = v.dev_bps = 0;
    SDL_ClearError();                 /* no stale error for later
                                       * unrelated SDL calls (as the old
                                       * ll_audio_close did) */
}

void plat_audio_queue(const void *bytes, size_t len)
{
    if (!v.logical_open || !v.as || !bytes || !len)
        return;
    if (SDL_AudioStreamPut(v.as, bytes, (int)len) < 0)
        return;
    /* Convert on the feeding thread: the caller's bytes reach the device
     * queue right away, exactly like the old direct SDL_QueueAudio, so
     * "queued" stays a truthful measure of what the device must play. */
    char scratch[4096];
    for (;;) {
        int avail = SDL_AudioStreamAvailable(v.as);
        if (avail <= 0)
            break;
        int want = avail < (int)sizeof scratch ? avail : (int)sizeof scratch;
        int got = SDL_AudioStreamGet(v.as, scratch, want);
        if (got <= 0)
            break;
        SDL_QueueAudio(v.adev, scratch, (Uint32)got);
    }
}

size_t plat_audio_queued_bytes(void)
{
    /* In the caller's SOURCE format (FIX-09 Part 2 invariant): device-
     * side bytes scale back by the exact integral byte-rate ratio, and
     * so do the stream's buffered converted bytes. uint64 math. */
    if (!v.logical_open || !v.as || !v.src_bps || !v.dev_bps)
        return 0;
    uint64_t dev_q = (uint64_t)SDL_GetQueuedAudioSize(v.adev) * v.src_bps;
    uint64_t str_q = (uint64_t)(Uint32)SDL_AudioStreamAvailable(v.as)
                   * v.src_bps;
    return (size_t)((dev_q + str_q) / v.dev_bps);
}

void plat_audio_pause(int paused)
{
    if (v.adev && v.logical_open)
        SDL_PauseAudioDevice(v.adev, paused ? 1 : 0);
}

void plat_audio_clear(void)
{
    if (!v.logical_open)
        return;
    if (v.as)
        SDL_AudioStreamClear(v.as);
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
    /* FIX-11 (deep dive L-01): the pin's stated reason ("SDL wayland
     * windows can never get keyboard focus") was about PROGRAMMATIC
     * raising - and FIX-02 removed every programmatic raise except the
     * one at creation, which Wayland compositors honour by giving a
     * newly mapped window focus normally. Default is now SDL's own
     * choice (classic SDL2 2.32 prefers X11 when DISPLAY is set - its
     * bootstrap list in src/video/SDL_video.c is ordered x11 before
     * wayland; under sdl2-compat/SDL3 the core prefers Wayland, which
     * is the modern path we want). The old X11/XWayland block runs
     * ONLY behind LEO_FORCE_X11=1 (XWayland costs sharpness under
     * fractional scaling on either core); its conditions and byte-
     * pinned log lines are unchanged. An explicit SDL_VIDEODRIVER
     * still wins (env beats every non-OVERRIDE hint). */
    if (SDL_getenv("SDL_VIDEODRIVER"))
        return;
    const char *fx = SDL_getenv("LEO_FORCE_X11");
    if (!fx || strcmp(fx, "1") != 0)
        return;
    if (SDL_getenv("WAYLAND_DISPLAY")) {
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
