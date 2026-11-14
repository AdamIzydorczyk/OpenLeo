/* is_player.c - SDL2 cutscene playback on top of the verified codec.
 * See is_player.h for the pacing contract; do NOT replace the clock with
 * a free-running loop (the original's bug is exactly that).
 */
#include "is_player.h"
#include "../is_stream.h"
#include "../ll_config.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    is_stream   is;
    SDL_Window  *win;
    SDL_Renderer *ren;
    SDL_Texture *tex;
    uint8_t     *rgb;      /* w*h*3 presentation buffer */
    SDL_AudioDeviceID dev; /* 0 = audio off/unavailable */
    unsigned     presented;
    unsigned     start_frame; /* skip presenting frames below this */
    Uint64       pause_at;    /* perf-counter stamp when paused (0=live) */
    const char  *quit_reason; /* diagnostics: what requested quit */
    int          idle;        /* cutscene done/skipped: hold window open */
    int          quit;
    Uint64       t0;
    Uint64       period;   /* ns per frame */
} player;

/* ESC-region PCM: 8 kHz unsigned 8-bit mono (pitch-verified in M2). */
static void enqueue_audio(void *u, const uint8_t *pcm, unsigned len)
{
    player *p = (player *)u;
    if (p->dev)
        SDL_QueueAudio(p->dev, pcm, len);
}

static void present(player *p)
{
    uint8_t rgb[768];
    is_palette_rgb8(&p->is, rgb);
    size_t n = (size_t)p->is.w * (size_t)p->is.h;
    uint8_t *lin = is_linearize(&p->is);
    if (!lin)
        return;
    for (size_t i = 0; i < n; i++) {
        uint8_t c = lin[i];
        p->rgb[3 * i] = rgb[3 * c];
        p->rgb[3 * i + 1] = rgb[3 * c + 1];
        p->rgb[3 * i + 2] = rgb[3 * c + 2];
    }
    free(lin);
    SDL_UpdateTexture(p->tex, NULL, p->rgb, p->is.w * 3);
    SDL_RenderClear(p->ren);
    SDL_RenderCopy(p->ren, p->tex, NULL, NULL);
    SDL_RenderPresent(p->ren);
    p->presented++;
}

static void pump_input(player *p)
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            p->quit = 1;
            p->quit_reason = "window close";
        } else if (e.type == SDL_KEYDOWN) {
            SDL_Keycode k = e.key.keysym.sym;
            if (k == SDLK_q) {
                p->quit = 1;
                p->quit_reason = "q";
            } else if (k == SDLK_ESCAPE && !p->idle) {
                /* original semantics: ESC interrupts the intro/cutscene,
                 * it does NOT quit the game. Demo state after skip = hold
                 * last frame (a menu/game comes with M3); further ESC is
                 * a no-op, matching "intro stopped, game still running". */
                p->quit = 1;
                p->quit_reason = "esc (skip cutscene)";
            } else if (k == SDLK_SPACE && p->dev) {
                int playing = SDL_GetAudioDeviceStatus(p->dev)
                              == SDL_AUDIO_PLAYING;
                SDL_PauseAudioDevice(p->dev, playing); /* space=pause toggle */
                /* freeze/resume the video clock WITH the audio clock,
                 * otherwise pausing desyncs A/V (proposal 1. in IMPROVEMENTS) */
                if (playing) /* was playing, now pausing */
                    p->pause_at = SDL_GetPerformanceCounter();
                else if (p->pause_at)
                    p->t0 += SDL_GetPerformanceCounter() - p->pause_at;
            }
        }
    }
}

/* fixed-timestep wait until absolute frame deadline (ns), drift-clamped */
static void wait_until(player *p, Uint64 deadline_ns)
{
    static const Uint64 SLACK = 2000000ull; /* 2 ms: spin-wait instead */
    for (;;) {
        Uint64 now = SDL_GetPerformanceCounter();
        double scale = 1e9 / (double)SDL_GetPerformanceFrequency();
        if ((Uint64)(now * scale) >= deadline_ns)
            return;
        if (deadline_ns - (Uint64)(now * scale) > SLACK)
            SDL_Delay(2);
        else
            SDL_Delay(0);
        pump_input(p);
        if (p->quit)
            return;
    }
}

/* frame callback: fires at every frame boundary inside decode, where fb
 * holds a coherent full frame; present exactly one per frame (chunks may
 * carry several frames), paced by the fixed clock */
static void on_frame(void *u, unsigned idx, const is_stream *s)
{
    player *p = (player *)u;
    (void)idx;
    if (s->st.frames <= p->start_frame)
        return; /* decode only (delta chain), no present */
    Uint64 frame = p->presented;
    double perf_scale = (double)SDL_GetPerformanceFrequency() / 1e9;
    Uint64 t0_ns = (Uint64)((double)p->t0 / perf_scale);
    Uint64 deadline_ns = t0_ns + frame * p->period;
    wait_until(p, deadline_ns);
    if (p->quit)
        return;
    present(p);
    /* drift clamp: if a frame is >2 periods late, resync the clock */
    Uint64 now_ns = (Uint64)((double)SDL_GetPerformanceCounter() / perf_scale);
    if (now_ns > deadline_ns + 2 * p->period)
        p->t0 = (Uint64)((double)SDL_GetPerformanceCounter()
                         - (double)(frame * p->period) * perf_scale);
}

int is_player_run(const char *path, int audio, unsigned start_frame)
{
    player p;
    memset(&p, 0, sizeof p);
    if (is_open(&p.is, path) != 0) {
        fprintf(stderr, "is_player: cannot open %s\n", path);
        return 1;
    }
    if (SDL_Init(SDL_INIT_VIDEO | (audio ? SDL_INIT_AUDIO : 0)) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        is_close(&p.is);
        return 1;
    }
    int scale = p.is.w < 640 ? 2 : 1;
    p.win = SDL_CreateWindow(
        "Lew Leon - cutscene (esc=skip, q=quit, space=pause)",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, p.is.w * scale,
        p.is.h * scale, 0);
    if (!p.win) {
        fprintf(stderr, "window: %s\n", SDL_GetError());
        goto err;
    }
    p.ren = SDL_CreateRenderer(p.win, -1, SDL_RENDERER_ACCELERATED);
    if (!p.ren)
        p.ren = SDL_CreateRenderer(p.win, -1, SDL_RENDERER_SOFTWARE);
    p.tex = SDL_CreateTexture(p.ren, SDL_PIXELFORMAT_RGB24,
                              SDL_TEXTUREACCESS_STREAMING, p.is.w, p.is.h);
    if (!p.tex || !p.ren) {
        fprintf(stderr, "renderer: %s\n", SDL_GetError());
        goto err;
    }
    SDL_SetTextureScaleMode(p.tex, scale > 1 ? SDL_ScaleModeNearest
                                             : SDL_ScaleModeLinear);
    p.rgb = (uint8_t *)malloc((size_t)p.is.w * (size_t)p.is.h * 3);
    if (!p.rgb)
        goto err;

    if (audio) {
        SDL_AudioSpec want, have;
        memset(&want, 0, sizeof want);
        want.freq = 8000;
        want.format = AUDIO_U8;
        want.channels = 1;
        want.samples = 1024;
        p.dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
        if (!p.dev)
            fprintf(stderr, "is_player: audio off (%s)\n", SDL_GetError());
        else
            SDL_PauseAudioDevice(p.dev, 0);
    }

    /* pacing: authoritative per-file fps from the .IS header; TICK_HZ only
     * as fallback for bogus/zero values. */
    int fps = p.is.fps > 0 ? p.is.fps : TICK_HZ;
    p.period = 1000000000ull / (Uint64)fps;
    p.start_frame = start_frame;
    p.t0 = SDL_GetPerformanceCounter();

    for (unsigned ci = 0; ci < p.is.nchunks && !p.quit; ci++) {
        /* frames before start_frame must still DECODE (frame deltas),
         * only their presentation and audio are suppressed */
        int live = p.is.st.frames + 1 > start_frame;
        if (live)
            is_walk_audio_range(&p.is, ci, 1, p.dev ? enqueue_audio : NULL,
                                &p);
        is_decode_chunk(&p.is, ci, on_frame, &p, NULL);
        if (p.is.st.ok) {
            fprintf(stderr, "is_player: desync at chunk %u\n", ci);
            break;
        }
    }
    if (p.quit && p.quit_reason && p.quit_reason[0] == 'e') {
        /* ESC skip: drop pending audio, hold the last frame like the
         * original does when the intro is interrupted (M3 replaces the
         * hold with menu/gameplay handoff) */
        if (p.dev)
            SDL_ClearQueuedAudio(p.dev);
        p.idle = 1;
        p.quit = 0;
        SDL_SetWindowTitle(p.win, "Lew Leon - cutscene skipped (q = quit)");
        printf("is_player: cutscene skipped, window held (q quits)\n");
        fflush(stdout);
        while (!p.quit) {
            pump_input(&p);
            SDL_Delay(30);
        }
    } else if (p.dev) { /* let queued audio drain (bounded) */
        while (SDL_GetQueuedAudioSize(p.dev) > 0 && !p.quit) {
            pump_input(&p);
            SDL_Delay(50);
        }
    }
    printf("is_player: presented=%u decoded=%u quit=%d (%s)\n", p.presented,
           p.is.st.frames, p.quit, p.quit_reason ? p.quit_reason : "finished");
    free(p.rgb);
    if (p.dev)
        SDL_CloseAudioDevice(p.dev);
    is_close(&p.is);
    SDL_Quit();
    return 0;
err:
    free(p.rgb);
    is_close(&p.is);
    SDL_Quit();
    return 1;
}
