/* platform_selftest.c - headless proof of the platform.h contract
 * (tips T-01). Links the DUMMY backend only: it decodes real .IS
 * frames and feeds them to plat_present with no display and no audio
 * device, then exercises the clock, the audio device contract
 * (open/queue/pause/clear/close), and the scripted event queue. This
 * is exactly what the review said was impossible before ("can't test
 * the engine without SDL").
 *
 *   ./platform_selftest [SOME.IS]      (default: data/INTRO_P.IS)
 *
 * Every check prints an "(ok)" line on success; any failure prints a
 * FAIL line and exits 1. CI (verify-platform) greps the ok lines.
 */
#include "is_stream.h"
#include "platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned presented_cb_count;

static void present_frames(void *user, unsigned idx, const is_stream *s)
{
    (void)idx; (void)user;
    /* Feed the engine's native output straight through the interface
     * (T-02: indexed framebuffer + palette, backend-agnostic). */
    plat_present(s->fb, s->pal, s->w, s->h);
    presented_cb_count++;
}

/* thread contract probe: never runs when the platform has no threads
 * (headless dummy); must simply be a valid plat_thread_start target. */
static int selftest_never_fn(void *ctx)
{
    (void)ctx;
    return 0;
}

static int fails;
static void check(const char *what, int cond)
{
    if (cond) {
        printf("platform_selftest: %s (ok)\n", what);
    } else {
        printf("platform_selftest: %s FAILED\n", what);
        fails++;
    }
}

int main(int argc, char **argv)
{
    const char *is = argc > 1 ? argv[1] : "data/INTRO_P.IS";
    int rc;

    /* ---- clock + monotonicity (T-05/T-06 pacing primitives) ---- */
    uint64_t t0 = plat_now_ns();
    uint64_t t1 = plat_now_ns();
    check("now_ns is monotonic", t1 >= t0);
    plat_sleep_until(t0 + 20ull * 1000 * 1000);       /* 20 ms floor */
    uint64_t t2 = plat_now_ns();
    check("sleep_until honors the deadline (>=20ms)",
          t2 >= t0 + 20ull * 1000 * 1000);

    /* ---- audio device contract (T-09 pump + the player's model) ---- */
    static uint8_t s16[4410 * 2 * 2];         /* 200 ms of s16 stereo */
    check("queue before open is ignored",
          (plat_audio_queue(s16, 8820), plat_audio_queued_bytes() == 0));
    check("audio_open ok", plat_audio_open(22050, 2) == 0);
    check("one device at a time", plat_audio_open(22050, 1) == -1);
    plat_audio_queue(s16, 8820);                          /* 100 ms */
    plat_audio_queue(s16, 17640);                         /* +200 ms */
    check("queued_bytes counts queued bytes",
          plat_audio_queued_bytes() == 26460);
    plat_audio_queue(NULL, 10);                           /* ignored */
    plat_audio_queue(s16, 0);                             /* ignored */
    check("invalid audio ignored", plat_audio_queued_bytes() == 26460);
    plat_audio_pause(1);                                  /* A/V hold */
    plat_audio_pause(0);
    check("pause does not drain", plat_audio_queued_bytes() == 26460);
    plat_audio_clear();                                   /* skip */
    check("clear empties the queue", plat_audio_queued_bytes() == 0);
    plat_audio_close();
    check("close releases the device (reopen works)",
          plat_audio_open(44100, 2) == 0 && plat_audio_queued_bytes() == 0);
    plat_audio_close();

    /* ---- engine-level input contract (keys + mouse) ---- */
    ll_event e;
    check("empty queue -> poll returns 0", plat_poll(&e) == 0);
    ll_event kd = { LL_EV_KEYDOWN, LLKEY_RETURN, -1, -1 };
    ll_event bt = { LL_EV_BUTTON_DOWN, LLBTN_LEFT, 142, 97 };
    ll_event mo = { LL_EV_MOTION, 0, 260, 31 };
    ll_event qt = { LL_EV_QUIT, 0, -1, -1 };
    plat_dummy_inject(&kd);
    plat_dummy_inject(&bt);
    plat_dummy_inject(&mo);
    plat_dummy_inject(&qt);
    ll_event g1, g2, g3, g4;
    check("poll delivers injected events in order (incl. mouse coords)",
          plat_poll(&g1) == 1 && g1.type == LL_EV_KEYDOWN &&
          g1.key == LLKEY_RETURN &&
          plat_poll(&g2) == 1 && g2.type == LL_EV_BUTTON_DOWN &&
          g2.key == LLBTN_LEFT && g2.x == 142 && g2.y == 97 &&
          plat_poll(&g3) == 1 && g3.type == LL_EV_MOTION &&
          g3.x == 260 && g3.y == 31 &&
          plat_poll(&g4) == 1 && g4.type == LL_EV_QUIT);
    check("queue drains -> poll returns 0", plat_poll(&e) == 0);
    plat_raise();                              /* must not crash */
    check("plat_raise is safe headless", 1);

    /* ---- worker-thread contract (music pump, E-09) ---- *
     * Headless there are no platform threads: start must fail cleanly,
     * which is exactly what routes the engine to its documented
     * UI-loop pump fallback; join stays safe. */
    check("headless thread start fails cleanly (UI-pump fallback)",
          plat_thread_start(selftest_never_fn, NULL, "selftest") == -1);
    plat_thread_join();
    check("thread join safe when none ran", 1);

    /* ---- decode -> present, with no display (the headline test) ---- */
    is_stream s;
    if (is_open(&s, is) != 0) {
        printf("platform_selftest: cannot open %s (test data missing?)\n", is);
        return 1;
    }
    rc = plat_init("headless-selftest", s.w, s.h);
    check("plat_init for .IS content", rc == 0);
    check("plat_init rejects bad geometry", plat_init("x", 0, -5) == -1);
    check("plat_init is idempotent (reconfigure)",
          plat_init("headless-selftest", s.w, s.h) == 0);
    check("plat_last_error is usable (never NULL)",
          plat_last_error() != NULL);
    unsigned pbase = plat_dummy_presented();
    is_decode_all(&s, present_frames, NULL, 5);        /* cap 5 frames */
    unsigned reached = plat_dummy_presented() - pbase;
    check("frames flowed through plat_present headlessly",
          reached >= 3 && reached == presented_cb_count);
    char m[128];
    snprintf(m, sizeof m, "decoded+presented %u/%dx%d frames (0 display)",
             reached, s.w, s.h);
    printf("platform_selftest: %s (ok)\n", m);
    is_close(&s);

    /* ---- ARGB compositing sink (the menu/level native form) ---- */
    static uint32_t argb[64u * 48u];
    unsigned abase = plat_dummy_presented();
    plat_present_argb(argb, 64, 48);
    check("argb present accepted", plat_dummy_presented() == abase + 1);
    plat_present_argb(NULL, 64, 48);
    plat_present_argb(argb, 0, 48);
    check("invalid argb ignored", plat_dummy_presented() == abase + 1);

    /* ---- plat_fopen delegates to the fs_ci opener (T-18) ---- */
    FILE *f = plat_fopen(is, "rb");
    int opened = f != NULL;
    if (f) {
        unsigned char b;
        opened = fread(&b, 1, 1, f) == 1;              /* readable bytes */
        fclose(f);
    }
    check("plat_fopen opens + reads game data", opened);
    check("plat_fopen returns NULL for a missing file",
          plat_fopen("does/not/exist.IS", "rb") == NULL);

    if (fails) {
        printf("platform_selftest: %d check(s) FAILED\n", fails);
        return 1;
    }
    printf("platform_selftest: engine path runs with NO SDL display/audio\n");
    return 0;
}
