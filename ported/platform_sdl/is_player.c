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
    unsigned     ci_audio;  /* FIX-08 (X-02): next chunk whose audio has
                             * NOT been queued yet (look-ahead pointer) */
    uint64_t     q_total;   /* FIX-08: bytes ever queued (live chunks) -
                             * together with queued_bytes this is the
                             * AUDIO position clock for drift correction */
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
    /* FIX-08 (X-02): the hold-time accounting moved to count_region -
     * this callback now ONLY feeds bytes (padding, coalescing, WAV dump,
     * live gate), so queuing audio can be decoupled from presenting. */
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

/* FIX-08 (X-02) split half of the old enqueue_audio: ONLY counts the
 * chunk's ESC region size, which is what sets its presentation hold
 * (hold = last_region / bps). Runs per chunk at present time; feeding
 * bytes is enqueue_audio's job now, which runs on the look-ahead. */
static void count_region(void *u, const uint8_t *pcm, unsigned len)
{
    (void)pcm;
    ((player *)u)->last_region += len;
}

/* FIX-08 (X-02): keep >= 150 ms (~3 device periods at 46 ms) of audio
 * queued ahead of the video position. SDL pulls one period per callback;
 * with only the current chunk queued (lead = one chunk hold, 14-157 ms)
 * a scheduling hiccup drains the queue, SDL plays silence and audio
 * permanently lags video. Audio and video still decode ONE pass in
 * chunk order - every live chunk's bytes are queued exactly once, in
 * order - so the LEO_IS_WAV dump is byte-for-byte what it was. */
#define AUDIO_LEAD_NS 150000000ull

static void queue_ahead(player *p, unsigned ci)
{
    /* never queue before live (start_frame gating needs the decoded frame
     * count), never in legacy period modes, never past the end. The
     * CURRENT chunk ci is always topped up first (ci_audio == ci), so a
     * present deadline always finds its own audio queued. */
    if (!p->audio || p->period || !p->live)
        return;
    if (p->ci_audio < ci)
        p->ci_audio = ci;                /* first live chunk */
    while (p->ci_audio < p->is.nchunks) {
        uint64_t queued_ns = (uint64_t)plat_audio_queued_bytes()
                             * 1000000000ull / p->bps;
        if (p->ci_audio > ci && queued_ns >= AUDIO_LEAD_NS)
            break;
        unsigned before = p->alen;       /* matches player.alen (unsigned) */
        is_walk_audio_range(&p->is, p->ci_audio, 1, enqueue_audio, p);
        if (p->alen == before) {
            /* Silent chunk (no ESC regions): the video hold is repeat
             * ticks at 70 Hz (see is_player_run below) - the ORIGINAL
             * keeps the DMA ticking with silence for exactly that long.
             * Queue the same duration of silence here so the audio
             * position clock tracks the video schedule; without it the
             * drift correction would compress video after long pauses. */
            unsigned rep = p->is.st.repeat ? p->is.st.repeat
                              : (unsigned)(p->is.fps > 0 ? p->is.fps : 2);
            uint64_t sil_bytes = (uint64_t)rep * p->bps / 70ull;
            static const uint8_t zeros[1260];   /* one DMA block */
            while (sil_bytes >= sizeof zeros) {
                enqueue_audio(p, zeros, (unsigned)sizeof zeros);
                sil_bytes -= sizeof zeros;
            }
            if (sil_bytes)
                enqueue_audio(p, zeros, (unsigned)sil_bytes);
        }
        p->q_total += p->alen - before;
        flush_audio(p);                  /* one queue call per chunk */
        p->ci_audio++;
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
                /* FIX-01 / C3 ruling (2026-11-15): q quits the APPLICATION.
                 * Same sticky flag the window close sets (platform.h); the
                 * gates in main.c then unwind the scene chain AND the menu
                 * - the program ends instead of advancing to the next
                 * scene. ESC stays the original two-step (skip this
                 * cutscene, continue): only q got the one-step semantics. */
                plat_request_quit();
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
 * time instead of rewinding the schedule (P-02). ci = the chunk about to
 * be presented: its look-ahead audio is topped up while waiting (FIX-08). */
static void wait_until(player *p, uint64_t deadline_ns, unsigned ci)
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
        if (!p->pause_at)
            queue_ahead(p, ci);  /* FIX-08: keep the lead >= AUDIO_LEAD_NS
                                  * across the whole wait; skipped while
                                  * paused (device held, nothing drains) */
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
    /* FIX-08 (X-02) tuning + diagnostics (additive hooks):
     * LEO_AV_LATENCY_MS overrides the nominal output latency subtracted in
     * the A/V comparison (default 46 ms = one device period at 22050 Hz,
     * samples=0); LEO_AV_DEBUG=1 prints 'is_player: av ...' once a second. */
    const int av_debug = getenv("LEO_AV_DEBUG") != NULL;
    uint64_t av_latency_ns = 46000000ull;
    {
        const char *alm = getenv("LEO_AV_LATENCY_MS");
        if (alm && atof(alm) > 0.0)
            av_latency_ns = (uint64_t)(atof(alm) * 1000000.0);
    }
    uint64_t av_next_ns = p.t0_ns + 1000000000ull; /* first second boundary */
    unsigned  av_minq_ms = (unsigned)-1;

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
            p.ci_audio = p.is.nchunks;  /* FIX-08: nothing more to queue */
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
        is_walk_audio_range(&p.is, ci, 1, count_region, &p);  /* hold time */
        if (p.period || !p.audio) {
            /* legacy/no-audio path: queue exactly this chunk, as before */
            is_walk_audio_range(&p.is, ci, 1, enqueue_audio, &p);
            flush_audio(&p);
        } else {
            queue_ahead(&p, ci);           /* FIX-08: look-ahead queue */
        }
        if (p.live)
            wait_until(&p, p.t0_ns + sched_ns, ci); /* this chunk's presentation
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
            if (p.audio && !p.period && !p.pause_at && p.q_total) {
                /* FIX-08 (X-02): audio position = bytes the device has
                 * ALREADY consumed. A device underrun STALLS that count,
                 * so video waits for audio instead of drifting ahead of a
                 * gap permanently. Correct half the error per chunk, at
                 * most 10 ms, so the motion stays smooth. */
                size_t q_bytes = plat_audio_queued_bytes();
                uint64_t played = p.q_total - (uint64_t)q_bytes;
                int64_t a_ns = (int64_t)(played * 1000000000ull / p.bps);
                uint64_t nown = now_ns();
                int64_t v_ns = (int64_t)(nown - p.t0_ns - p.pause_shift);
                int64_t err = v_ns - a_ns - (int64_t)av_latency_ns;
                                            /* >0: video ahead of audio */
                if (err > 20000000 || err < -20000000) {
                    int64_t c = err / 2;
                    if (c > 10000000)
                        c = 10000000;
                    if (c < -10000000)
                        c = -10000000;
                    /* t0_ns is uint64: the shift is written explicitly
                     * as signed wraparound, not an unsigned += */
                    p.t0_ns = (uint64_t)((int64_t)p.t0_ns + c);
                }
                if (av_debug) {
                    unsigned q_ms = (unsigned)(q_bytes * 1000ull / p.bps);
                    if (q_ms < av_minq_ms)
                        av_minq_ms = q_ms;
                    if (nown >= av_next_ns) {
                        printf("is_player: av min_queued=%ums err=%+dms\n",
                               av_minq_ms, (int)(err / 1000000));
                        av_minq_ms = q_ms;
                        av_next_ns = nown + 1000000000ull;
                    }
                }
            }
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
