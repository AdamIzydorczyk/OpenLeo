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
#include <sys/stat.h>
#include <dirent.h>

/* Data files: the engine's internal names are lowercase (status.leo,
 * glowne.mnu, mapka.mod) while users' ISO extracts are usually UPPERCASE
 * (proposal 4. in IMPROVEMENTS). Resolve path component-by-component
 * case-insensitively when the exact path does not exist. Returns a malloc'd
 * resolved path, or NULL if no case-insensitive match exists. */
static char *resolve_ci(const char *path)
{
    struct stat st;
    if (stat(path, &st) == 0)
        return strdup(path);
    char out[4096];
    out[0] = '\0';
    const char *p = path;
    while (*p) {
        const char *slash = strchr(p, '/');
        size_t len = slash ? (size_t)(slash - p) : strlen(p);
        if (len == 0) { /* leading/duplicate slash */
            if (strlen(out) + 1 >= sizeof out) return NULL;
            strcat(out, "/");
            p++;
            continue;
        }
        if (len >= 512)
            return NULL;
        char comp[512];
        memcpy(comp, p, len);
        comp[len] = '\0';
        char probe[4096];
        snprintf(probe, sizeof probe, "%s/%s", out, comp);
        char chosen[512];
        if (stat(probe, &st) == 0) {
            memcpy(chosen, comp, len + 1);
        } else {
            const char *dir = out[0] ? out : ".";
            DIR *d = opendir(dir);
            int found = 0;
            if (d) {
                struct dirent *de;
                while ((de = readdir(d)) != NULL) {
                    size_t j = 0;
                    for (; j < len; j++) {
                        char a = de->d_name[j], b = comp[j];
                        if (a >= 'A' && a <= 'Z') a = (char)(a - 'A' + 'a');
                        if (b >= 'A' && b <= 'Z') b = (char)(b - 'A' + 'a');
                        if (a != b) break;
                    }
                    if (j == len && de->d_name[j] == '\0') {
                        snprintf(chosen, sizeof chosen, "%s", de->d_name);
                        found = 1;
                        break;
                    }
                }
                closedir(d);
            }
            if (!found)
                return NULL;
        }
        if (strlen(out) + 1 + strlen(chosen) >= sizeof out)
            return NULL;
        strcat(out, "/");
        strcat(out, chosen);
        p += len;
        if (*p == '/')
            p++;
    }
    if (stat(out, &st) != 0 || out[0] == '\0')
        return NULL;
    return strdup(out);
}

typedef struct {
    is_stream   is;
    SDL_Window  *win;
    SDL_Renderer *ren;
    SDL_Texture *tex;
    SDL_AudioDeviceID dev; /* 0 = audio off/unavailable */
    uint8_t     *ascratch; /* coalesced per-chunk audio (gapless queue)    */
    unsigned     alen, acap;
    unsigned     presented;
    unsigned     start_frame; /* skip presenting frames below this */
    Uint64       pause_at;    /* perf-counter stamp when paused (0=live) */
    const char  *quit_reason; /* diagnostics: what requested quit */
    int          skip;        /* ESC: fast-skip cutscene to its end */
    unsigned     skip_at;     /* auto-skip after N presented (test hook) */
    int          idle;        /* cutscene done/skipped: hold window open */
    int          quit;
    Uint64       t0;
    Uint64       period;   /* ns per frame */
} player;

/* ESC-region PCM: 8 kHz unsigned 8-bit mono (pitch-verified in M2).
 * Coalesced into ascratch and queued once per chunk -> gapless joins
 * between the many small ESC blocks (proposal 3. in IMPROVEMENTS). */
static void enqueue_audio(void *u, const uint8_t *pcm, unsigned len)
{
    player *p = (player *)u;
    if (!p->dev || len == 0)
        return;
    if (p->alen + len > p->acap) {
        unsigned nc = (p->alen + len) * 3 / 2 + 4096;
        uint8_t *nb = (uint8_t *)realloc(p->ascratch, nc);
        if (!nb)
            return;
        p->ascratch = nb;
        p->acap = nc;
    }
    memcpy(p->ascratch + p->alen, pcm, len);
    p->alen += len;
}

static void flush_audio(player *p)
{
    if (p->dev && p->alen) {
        SDL_QueueAudio(p->dev, p->ascratch, p->alen);
        p->alen = 0;
    }
}

/* Presentation fast path (proposal 2. in IMPROVEMENTS): precomputed
 * 256-entry palette LUT written straight into a locked ARGB8888 texture
 * (no RGB24 staging buffer, no per-pixel 3-byte indexing, no per-frame
 * allocation in the hot loop). */
static void present(player *p)
{
    uint8_t rgb[768];
    is_palette_rgb8(&p->is, rgb);
    Uint32 lut[256];
    for (int i = 0; i < 256; i++)
        lut[i] = 0xFF000000u | ((Uint32)rgb[3 * i] << 16)
                 | ((Uint32)rgb[3 * i + 1] << 8) | (Uint32)rgb[3 * i + 2];
    uint8_t *lin = is_linearize(&p->is);
    if (!lin)
        return;
    void *px;
    int pitch;
    if (SDL_LockTexture(p->tex, NULL, &px, &pitch) == 0) {
        int w = p->is.w, h = p->is.h;
        for (int y = 0; y < h; y++) {
            Uint32 *row = (Uint32 *)((uint8_t *)px + (size_t)y * pitch);
            const uint8_t *src = lin + (size_t)y * w;
            for (int x = 0; x < w; x++)
                row[x] = lut[src[x]];
        }
        SDL_UnlockTexture(p->tex);
    }
    free(lin);
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
                /* original semantics: ESC SKIPS the intro/cutscene and the
                 * game continues - never freeze a mid-intro frame, never
                 * quit. Demo continues to the cutscene's last frame (M3
                 * will hand over to the title menu instead). */
                p->skip = 1;
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
        if (p->quit || p->skip)
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
    if (p->quit || p->skip)
        return;
    present(p);
    /* drift clamp: if a frame is >2 periods late, resync the clock */
    Uint64 now_ns = (Uint64)((double)SDL_GetPerformanceCounter() / perf_scale);
    if (now_ns > deadline_ns + 2 * p->period)
        p->t0 = (Uint64)((double)SDL_GetPerformanceCounter()
                         - (double)(frame * p->period) * perf_scale);
}

/* ESC skip helper: remember the last frame whose palette is not all-zero,
 * so a skipped cutscene lands on the last VISIBLE frame instead of the
 * stream's by-design fade-to-black tail (verified: INTRO_P ends with a
 * zeroed PAL block after the final frame, framebuffer content intact). */
typedef struct {
    uint8_t *fb;
    uint8_t  pal[768];
    size_t   n;
    int      have;
} skipctx;

static void on_skip_frame(void *u, unsigned idx, const is_stream *s)
{
    skipctx *k = (skipctx *)u;
    (void)idx;
    int bright = 0;
    for (int i = 0; i < 768; i++)
        if (s->pal[i]) {
            bright = 1;
            break;
        }
    if (bright) {
        memcpy(k->fb, s->fb, k->n);
        memcpy(k->pal, s->pal, 768);
        k->have = 1;
    }
}

int is_player_run(const char *path, int audio, unsigned start_frame,
                  unsigned skip_at)
{
    player p;
    memset(&p, 0, sizeof p);
    p.skip_at = skip_at;
    char *rpath = resolve_ci(path);
    if (is_open(&p.is, rpath ? rpath : path) != 0) {
        fprintf(stderr, "is_player: cannot open %s\n", path);
        free(rpath);
        return 1;
    }
    if (rpath && strcmp(rpath, path) != 0)
        printf("is_player: data path resolved as: %s\n", rpath);
    free(rpath);
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
    p.tex = SDL_CreateTexture(p.ren, SDL_PIXELFORMAT_ARGB8888,
                              SDL_TEXTUREACCESS_STREAMING, p.is.w, p.is.h);
    if (!p.tex || !p.ren) {
        fprintf(stderr, "renderer: %s\n", SDL_GetError());
        goto err;
    }
    SDL_SetTextureScaleMode(p.tex, scale > 1 ? SDL_ScaleModeNearest
                                             : SDL_ScaleModeLinear);

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
        if (p.skip_at && p.presented >= p.skip_at)
            p.skip = 1;
        if (p.skip) {
            /* ESC: fast-forward the rest of the stream silently (the delta
             * chain needs every frame decoded), then show the true last
             * frame - looks like a real skip, never a frozen mid-frame */
            if (p.dev)
                SDL_ClearQueuedAudio(p.dev);
            p.alen = 0; /* drop un-flushed coalesced audio too */
            printf("is_player: ESC at frame %u -> skipping %u chunks\n",
                   p.presented, p.is.nchunks - ci);
            {
                size_t fnsz = (size_t)p.is.w * (size_t)p.is.h;
                skipctx sc;
                memset(&sc, 0, sizeof sc);
                sc.fb = (uint8_t *)malloc(fnsz);
                sc.n = fnsz;
                for (; ci < p.is.nchunks; ci++) {
                    is_decode_chunk(&p.is, ci,
                                    sc.fb ? on_skip_frame : NULL,
                                    sc.fb ? &sc : NULL, NULL);
                    if (p.is.st.ok)
                        break;
                }
                if (sc.have) { /* land on last visible frame */
                    memcpy(p.is.fb, sc.fb, fnsz);
                    memcpy(p.is.pal, sc.pal, 768);
                }
                free(sc.fb);
            }
            {
                const char *dp = getenv("LEO_DUMP_SKIP");
                if (dp) {
                    is_write_ppm(&p.is, dp);
                    /* CI convention: a skip-dump implies headless test
                     * mode -> quit instead of holding the window. */
                    p.quit = 1;
                    p.quit_reason = "skip-dump test";
                }
            }
            present(&p);
            break;
        }
        /* frames before start_frame must still DECODE (frame deltas),
         * only their presentation and audio are suppressed */
        int live = p.is.st.frames + 1 > start_frame;
        if (live) {
            is_walk_audio_range(&p.is, ci, 1, p.dev ? enqueue_audio : NULL,
                                &p);
            flush_audio(&p); /* one queue call per chunk (gapless) */
        }
        is_decode_chunk(&p.is, ci, on_frame, &p, NULL);
        if (p.is.st.ok) {
            fprintf(stderr, "is_player: desync at chunk %u\n", ci);
            break;
        }
    }
    if (p.dev && !p.skip) {
        /* natural finish: let queued audio drain (bounded) */
        while (SDL_GetQueuedAudioSize(p.dev) > 0 && !p.quit && !p.skip) {
            pump_input(&p);
            SDL_Delay(50);
        }
    }
    if (p.skip && !p.quit) {
        p.idle = 1;
        SDL_SetWindowTitle(p.win, "Lew Leon - cutscene skipped (q = quit)");
    }
    if (!p.quit) {
        /* natural finish too: hold final frame until q / window close,
         * so playback never vanishes on its own */
        if (!p.idle) {
            p.idle = 1;
            SDL_SetWindowTitle(p.win,
                               "Lew Leon - cutscene ended (q = quit)");
        }
        while (!p.quit) {
            pump_input(&p);
            SDL_Delay(30);
        }
    }
    printf("is_player: presented=%u decoded=%u quit=%d (%s)\n", p.presented,
           p.is.st.frames, p.quit, p.quit_reason ? p.quit_reason : "finished");
    free(p.ascratch);
    if (p.dev)
        SDL_CloseAudioDevice(p.dev);
    is_close(&p.is);
    SDL_Quit();
    return 0;
err:
    free(p.ascratch);
    is_close(&p.is);
    SDL_Quit();
    return 1;
}
