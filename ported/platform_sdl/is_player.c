/* is_player.c - cutscene playback on top of the verified codec, talking
 * ONLY to the platform interface (platform.h, tips T-01): window +
 * texture + audio device come from the backend session, input arrives
 * as ll_events, pacing uses plat_now_ns/plat_sleep_until. This file
 * contains no SDL.
 * See is_player.h for the pacing contract; do NOT replace the clock with
 * a free-running loop (the original's bug is exactly that).
 * Path robustness: all game-data opening goes through ll_fopen_ci /
 * ll_resolve_ci (fs_ci.h) - case-insensitive on POSIX, UTF-8-aware on
 * Windows (review P-01/P-07; the old in-file resolve_ci is removed). */
#include "is_player.h"
#include "../fs_ci.h"
#include "../is_stream.h"
#include "../ll_config.h"
#include "../ll_level.h"
#include "../platform.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The one ns clock source (tips T-07): every pacing decision derives from
 * this, so pause and drift-clamp can share a single unit (ns). Backed by
 * the platform layer (T-01) - same monotonic counter as before. */
static uint64_t now_ns(void)
{
    return plat_now_ns();
}

typedef struct {
    is_stream   is;
    int          audio;      /* 0 = audio off/unavailable. The window,
                              * texture and audio DEVICE are owned by the
                              * platform session (P-03/P-13), not by this
                              * screen - T-01. */
    uint8_t     *ascratch; /* coalesced per-chunk audio (gapless queue)    */
    unsigned     alen, acap;
    uint8_t     *lbuf;     /* linearized-frame scratch, reused every frame
                            * (P-12: no full-frame malloc in the present path) */
    size_t       lcap;
    unsigned     presented;
    unsigned     start_frame; /* skip presenting frames below this */
    uint64_t     pause_at;    /* ns stamp when paused (0=live); works with
                               * OR without an audio device (T-07) */
    uint64_t     pause_shift; /* total paused ns: wait_until and the drift
                               * clamp apply it to every deadline, so a
                               * pause holds the scene and never rewinds
                               * or fast-forwards it (P-02, T-07) */
    const char  *quit_reason; /* diagnostics: what requested quit */
    int          skip;        /* ESC: fast-skip cutscene to its end */
    int          user_skip;   /* skip requested by a real keypress (not test) */
    unsigned     skip_at;     /* auto-skip after N presented (test hook) */
    int          idle;        /* cutscene done/skipped: hold window open */
    int          quit;
    uint64_t     t0_ns;       /* the ONE presentation clock (T-07): set once
                               * at start, rebased only by the drift clamp;
                               * deadlines are always t0_ns + schedule,
                               * never any other copy (P-02) */
    uint64_t     period;   /* uniform hold override (LEO_IS_PERIOD_MS);
                            * 0 = audio-block clock (the real engine model) */
    int          live;     /* chunk past start_frame: present + play audio */
    unsigned     last_region; /* ESC bytes seen in the current chunk */
    int          stereo;   /* movie channels (gcd of region sizes) */
    unsigned     bps;      /* audio bytes/s: 88200 stereo / 44100 mono */
} player;

/* LEO_IS_WAV dump file: opened on the first audio byte by enqueue_audio,
 * closed by close_wav() at every is_player_run exit - the review-era ASan
 * leak check flags nothing from this hook any more. */
static FILE *s_wav;
static void close_wav(void)
{
    if (s_wav) {
        fclose(s_wav);
        s_wav = NULL;
    }
}

/* ESC-region PCM: raw signed 16-bit little-endian at 22050 Hz, MONO or
 * STEREO depending on the movie (LOGO/ENA1/KONIEC stereo, INTRO_P mono), so
 * region bytes go to SDL untouched once the device matches the movie.
 * Evidence (2026-11-14, see also the pacing comment in is_player_run):
 *   * every payload's ESC region is exactly N x 1260 B (stereo) or N x 630 B
 *     (mono), where N is the repeat value set by the RPT8/RPT16 opcode that
 *     precedes it (verified pairing across LOGO/ENA1/KONIEC/INTRO_P - e.g.
 *     INTRO_P: RPT8=65 -> region 40950 = 65*630, RPT8=5 -> 3150 = 5*630);
 *   * 1260 B = 315 stereo frames and 630 B = 315 mono samples - both exactly
 *     one Sound Blaster DMA block (DAT_000c40b6 = 0x13b, rate 0x5622 = 22050
 *     in build/decompiled/all/FUN_00019a60.c);
 *   * LOGO region autocorrelation r(2)=0.96 >> r(1)=0.55 = interleaved
 *     uncorrelated stereo pairs; INTRO_P r(1)=0.94 = single smooth channel;
 *   * total lengths match remembered cutscene lengths exactly: LOGO 703080 B
 *     @ 88200 B/s = 7.97 s ("the whole logo audio is about 8 seconds").
 * Regions are sample-aligned blocks, but we still pad a stray odd byte so it
 * can never desync every later sample pair.
 * Coalesced into ascratch and queued once per chunk -> gapless joins between
 * the many small ESC blocks (proposal 3. in IMPROVEMENTS). */
static void enqueue_audio(void *u, const uint8_t *pcm, unsigned len)
{
    player *p = (player *)u;
    p->last_region += len;     /* chunk hold time = this region's audio (ns) */
    if (!p->audio || !p->live || len == 0)
        return;
    unsigned pad = p->stereo ? ((4u - (len & 3u)) & 3u) : (len & 1u);
    if (p->alen + len + pad > p->acap) {
        unsigned nc = (p->alen + len) * 3 / 2 + 4096;
        uint8_t *nb = (uint8_t *)realloc(p->ascratch, nc);
        if (!nb)
            return;
        p->ascratch = nb;
        p->acap = nc;
    }
    memcpy(p->ascratch + p->alen, pcm, len);
    p->alen += len;
    while (pad--)                     /* keep every later sample aligned */
        p->ascratch[p->alen++] = 0;
    if (!s_wav) {
        const char *wvp = getenv("LEO_IS_WAV"); /* CI/diagnostic hook: raw
            * s16 LE dump, listen with ffplay -f s16le -ar 22050 -ac 1 FILE.
            * Opened once here, closed by close_wav() at every run end. */
        if (wvp)
            s_wav = ll_fopen_ci(wvp, "wb");
    }
    if (s_wav)
        fwrite(pcm, 1, len, s_wav);
}

static void flush_audio(player *p)
{
    if (p->audio && p->alen) {
        plat_audio_queue(p->ascratch, p->alen);
        p->alen = 0;
    }
}

/* Presentation fast path: this screen owns the CODEC side of the frame
 * (line up the Mode X planes - same order as is_stream's PPM writer,
 * reused across frames, P-12: no full-frame malloc in the present
 * path), then hands the INDEXED frame + raw palette to the platform:
 * the 6-bit DAC expansion (v<<2), the 256-entry LUT and the ARGB blit
 * live in the backend (T-02), operating on identical values so the
 * pixels on screen are bit-for-bit what this function drew before. */
static void present(player *p)
{
    size_t fn = (size_t)p->is.w * (size_t)p->is.h;
    if (p->lcap < fn) {
        uint8_t *nb = (uint8_t *)realloc(p->lbuf, fn);
        if (!nb)
            return;
        p->lbuf = nb;
        p->lcap = fn;
    }
    uint8_t *lin = p->lbuf; /* reused across frames (P-12) */
    is_linearize_into(&p->is, lin);
    plat_present(lin, p->is.pal, p->is.w, p->is.h);
    p->presented++;
}

static void pump_input(player *p)
{
    ll_event e;
    while (plat_poll(&e)) {
        if (e.type == LL_EV_QUIT) {
            p->quit = 1;
            p->quit_reason = "window close";
        } else if (e.type == LL_EV_KEYDOWN) {
            if (e.repeat)
                continue;   /* FIX-03 (X-04): every key the player acts
                             * on is a toggle/confirm (q, ESC-skip, Space
                             * pause): act on the first press only */
            if (e.key == 'q') {
                p->quit = 1;
                p->quit_reason = "q";
            } else if (e.key == LLKEY_ESC && !p->idle) {
                /* original semantics: ESC SKIPS the intro/cutscene and the
                 * game continues - never freeze a mid-intro frame, never
                 * quit. Demo continues to the cutscene's last frame; with
                 * scene chaining (--then/--demo) the caller then advances
                 * to the next scene / the title menu (M3). */
                p->skip = 1;
                p->user_skip = 1;
            } else if (e.key == ' ') {
                /* Pause holds the ONE clock (T-07) and works with or
                 * without an audio device; with audio the device is held
                 * too, so A/V stays in step (proposal 1. IMPROVEMENTS).
                 * While held, wait_until shifts every deadline by the
                 * live paused time; at resume that time joins
                 * pause_shift, so the scene never "catches up". The
                 * platform call is a no-op when no device is open. */
                if (!p->pause_at) {
                    p->pause_at = now_ns();
                    plat_audio_pause(1);
                } else {
                    p->pause_shift += now_ns() - p->pause_at;
                    p->pause_at = 0;
                    plat_audio_pause(0);
                }
            }
        }
    }
}

/* fixed-timestep wait until absolute frame deadline (ns, on the t0_ns
 * schedule), drift-clamped; a pause offsets the deadline with the paused
 * time instead of rewinding the schedule (P-02) */
static void wait_until(player *p, uint64_t deadline_ns)
{
    static const uint64_t SLACK = 2000000ull; /* 2 ms: spin-wait instead */
    for (;;) {
        uint64_t now = now_ns();
        uint64_t eff = deadline_ns + p->pause_shift +
                       (p->pause_at ? now - p->pause_at : 0);
        if (now >= eff)
            return;
        /* same policy as before: far deadlines sleep coarsely, near
         * ones resolve exactly at the effective deadline (the platform
         * sleep keeps that tightness) */
        if (eff - now > SLACK)
            plat_sleep_until(now + SLACK);
        else
            plat_sleep_until(eff);
        pump_input(p);
        if (p->quit || p->skip)
            return;
    }
}

/* frame callback: fires at every frame boundary inside decode.  Pacing is
 * chunk-level and applied by the main loop (deadline already awaited
 * before decode), so this only presents; every sampled movie payload holds
 * 0 or 1 frames, so per-frame subdivision never comes up. */
static void on_frame(void *u, unsigned idx, const is_stream *s)
{
    player *p = (player *)u;
    (void)idx;
    if (s->st.frames <= p->start_frame)
        return; /* decode only (delta chain), no present */
    present(p);
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

/* Channel detection without a header field: every ESC region is a whole
 * number of DMA blocks, 630 B mono or 1260 B stereo at 22050 s16.  If the
 * gcd of all region sizes is a multiple of 1260 the movie is stereo; an odd
 * multiple of 630 (INTRO_P: all 3150 = 5*630) proves mono.  Verified against
 * autocorrelation: stereo LOGO r(2)=0.96>>r(1)=0.55, mono INTRO_P r(1)=0.94.
 * (Residual risk: a mono movie that only ever uses an EVEN block count would
 * read as stereo - none of the 15 shipped movies does.) */
static unsigned igcd(unsigned a, unsigned b)
{
    while (b) {
        unsigned t = a % b;
        a = b;
        b = t;
    }
    return a;
}
static void gcd_audio_cb(void *u, const uint8_t *pcm, unsigned len)
{
    (void)pcm;
    *(unsigned *)u = igcd(*(unsigned *)u, len);
}

int is_player_run(const char *path, int audio, unsigned start_frame,
                  unsigned skip_at)
{
    player p;
    memset(&p, 0, sizeof p);
    p.skip_at = skip_at;
    if (is_open(&p.is, path) != 0) { /* is_open itself falls back to
                                        ll_fopen_ci (fs_ci.h, P-07) */
        fprintf(stderr, "is_player: cannot open %s\n", path);
        return 1;
    }
    unsigned g = 0;
    is_walk_audio(&p.is, gcd_audio_cb, &g);
    p.stereo = g && g % 1260u == 0;
    p.bps = p.stereo ? 88200u : 44100u;
    char *rpath = ll_resolve_ci(path); /* diagnostic only: the open above
                                          already accepted a CI match */
    if (rpath && strcmp(rpath, path) != 0)
        printf("is_player: data path resolved as: %s\n", rpath);
    free(rpath);
    /* T-01: the window + texture + audio device now come from the
     * platform session. P-03's lifecycle (one window for the whole
     * program, scene switches RECONFIGURE it), P-04's focus policy,
     * P-10's logical viewport, the F11/Alt+Enter handling and
     * P-13's spec contract + accepted-spec log all live behind plat_*
     * (in ll_view/ll_audio). The title string is unchanged. */
    int prc = plat_init("Lew Leon - cutscene (esc=skip, q=quit, space=pause,"
                        " f11/alt-enter=fullscreen)", p.is.w, p.is.h);
    if (prc == -2) {
        fprintf(stderr, "renderer: %s\n", plat_last_error());
        goto err;
    }
    if (prc != 0) {
        fprintf(stderr, "window: %s\n", plat_last_error());
        goto err;
    }

    if (audio) {
        /* The pipewire->alsa audio pin and the device open live inside
         * plat_audio_open (ll_audio.h). A subsystem failure (-1) also
         * implies the device failed - the old code printed both lines
         * in exactly that case, kept verbatim. */
        int arc = plat_audio_open(22050, p.stereo ? 2 : 1);
        if (arc == -1)
            fprintf(stderr, "is_player: audio init failed: %s\n",
                    plat_last_error());
        if (arc != 0)
            fprintf(stderr, "is_player: audio off (%s)\n", plat_last_error());
        else
            p.audio = 1;
    }

    /* Pacing (RE-PINNED 2026-11-14 - the audio block clock): ISplay plays
     * the ESC stream at a fixed 22050 Hz s16 in DMA blocks of 315 source
     * frames (DAT_000c40b6 = 0x13b, rate DAT_000c40b4 = 0x5622 in
     * build/decompiled/all/FUN_00019a60.c) = 14.29 ms = 1/70 s per block
     * (630 B mono / 1260 B stereo).  Its DMA IRQ handler (FUN_000185c0,
     * ends with out(0x20,0x20) PIC EOI) counts DAT_000c410c down per block
     * and fetches the next video chunk when it reaches 0; RPT8/RPT16 ops
     * reload it (FUN_0001b0d9), header byte 0x5D is the initial value.
     * Empirically every payload's ESC region == repeat x block_bytes, so
     * each chunk is exactly one frame on screen for its region's audio:
     * hold = region_bytes / bps.  Verified pairings: LOGO (2,2520),(3,3780),
     * (4,5040),(5,6300); ENA1 (4,2520),(6,3780),(8,5040),(10,6300); KONIEC
     * (5,6300),(6,7560)... up to 13860; INTRO_P hdr 5 -> 3150=5*630 and
     * RPT8=65 -> 40950=65*630.  Region-less chunks hold repeat*14.29 ms
     * (the engine silence-pads, cf. the 0x80 fill in FUN_000185c0).
     * Totals now match human memory exactly: LOGO 703080 B @ 88200 B/s =
     * 7.97 s ("the logo audio is over in about 8 seconds") - the previous
     * period-byte x 1/22 Hz model ran it at 15.5 s, which is why our
     * cutscenes crawled at up to half speed.  Legacy test hooks:
     * LEO_IS_PERIOD_MS=N forces a uniform N ms hold, LEO_IS_FPS_DIRECT the
     * oldest reading (byte = fps); with either set, audio no longer paces
     * video and the queue absorbs the difference. */
    int ticks = p.is.fps > 0 ? p.is.fps : 2;
    const char *ovr = getenv("LEO_IS_PERIOD_MS");
    if (ovr && atoi(ovr) > 0)
        p.period = (uint64_t)atoi(ovr) * 1000000ull;
    else if (getenv("LEO_IS_FPS_DIRECT"))
        p.period = 1000000000ull / (uint64_t)ticks;
    p.start_frame = start_frame;
    p.t0_ns = now_ns();             /* the ONE clock anchor (P-02) */
    uint64_t sched_ns = 0;          /* this chunk's presentation deadline */

    for (unsigned ci = 0; ci < p.is.nchunks && !p.quit; ci++) {
        if (p.skip_at && p.presented >= p.skip_at) {
            p.skip = 1;
            /* LEO_SKIP_USER=1: treat the auto-skip as a real keypress
             * (test hook for the interactive ESC->scene-chain handoff) */
            if (getenv("LEO_SKIP_USER"))
                p.user_skip = 1;
        }
        if (p.skip) {
            /* ESC: fast-forward the rest of the stream silently (the delta
             * chain needs every frame decoded), then show the true last
             * frame - looks like a real skip, never a frozen mid-frame */
            plat_audio_clear();
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
        p.live = p.is.st.frames + 1 > start_frame;
        p.last_region = 0;
        is_walk_audio_range(&p.is, ci, 1, enqueue_audio, &p);
        flush_audio(&p); /* one queue call per chunk (gapless) */
        if (p.live)
            wait_until(&p, p.t0_ns + sched_ns); /* this chunk's presentation
                                                   deadline */
        if (p.quit || p.skip)
            continue;
        is_decode_chunk(&p.is, ci, on_frame, &p, NULL);
        if (p.is.st.ok) {
            fprintf(stderr, "is_player: desync at chunk %u\n", ci);
            break;
        }
        /* audio-block clock: hold this chunk for exactly its region's
         * audio; silent chunks hold repeat * 1/70 s (engine keeps the
         * DMA ticking with silence, video keeps advancing - FUN_000185c0) */
        uint64_t hold;
        if (p.period)
            hold = p.period;
        else if (p.last_region)
            hold = (uint64_t)p.last_region * 1000000000ull / p.bps;
        else {
            unsigned rep = p.is.st.repeat ? p.is.st.repeat
                                          : (unsigned)ticks;
            hold = (uint64_t)rep * 1000000000ull / 70ull;
        }
        if (p.live) {
            /* pre-start (LEO_DEMO_SKIP_FRAMES) chunks decode but consume no
             * wall time: the clock anchors at the first live frame, as the
             * old per-frame model did */
            sched_ns += hold;
            /* drift clamp: >2 holds late (debugger, compose stall) =
             * resync the anchor instead of trying to catch up */
            uint64_t now = now_ns();
            if (!p.pause_at &&
                now > p.t0_ns + sched_ns + p.pause_shift + 2 * hold) {
                /* re-anchor the schedule so it lands now; while paused
                 * this must not fire, the pause legitimately puts "now"
                 * far behind the deadline (P-02) */
                p.t0_ns = now - sched_ns - p.pause_shift;
            }
        }
    }
    if (p.audio && !p.skip) {
        /* natural finish: let queued audio drain (bounded) */
        while (plat_audio_queued_bytes() > 0 && !p.quit && !p.skip) {
            pump_input(&p);
            plat_sleep_until(now_ns() + 50000000ull);
        }
    }
    if (!p.quit && !p.skip) {
        /* natural finish: hold the last frame briefly (the original always
         * advances to the next scene; a bare single-file run shows the end
         * for a moment before exiting) - 2 s on the ONE ns clock (T-07) */
        uint64_t t_end = now_ns() + 2000000000ull;
        while (!p.quit && now_ns() < t_end) {
            pump_input(&p);
            plat_sleep_until(now_ns() + 30000000ull);
        }
    }
    printf("is_player: presented=%u decoded=%u quit=%d (%s)\n", p.presented,
           p.is.st.frames, p.quit, p.quit_reason ? p.quit_reason : "finished");
    free(p.ascratch);
    free(p.lbuf);
    close_wav();
    plat_audio_close();
    /* T-01/P-03: no window/texture teardown here at all - the window and
     * the backend's texture belong to the platform session and are
     * reused by the next scene / the menu; the process teardown is
     * main's business (no SDL_Quit from a screen). */
    is_close(&p.is);
    return p.user_skip ? 2 : 0;
err:
    free(p.ascratch);
    free(p.lbuf);
    close_wav();
    is_close(&p.is);
    return 1;
}
