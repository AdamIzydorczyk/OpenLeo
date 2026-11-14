/* platform_selftest.c - headless proof of the platform.h contract
 * (tips T-01). Links the DUMMY backend only: it decodes real .IS
 * frames and feeds them to plat_present with no display and no audio
 * device, then exercises the clock, the audio-queue accounting, and the
 * scripted event queue. This is exactly what the review said was
 * impossible before ("can't test the engine without SDL").
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

    /* ---- audio-queue accounting (T-09 pump contract) ---- */
    static int16_t s16[4410 * 2];                      /* 200 ms stereo */
    plat_audio_queue(s16, 2205, 22050, 2);             /* 100 ms */
    plat_audio_queue(s16, 4410, 22050, 2);             /* +200 ms */
    size_t qms = plat_audio_queued_ms();
    check("audio_queued_ms reflects queued audio (300ms)", qms == 300);
    plat_audio_queue(NULL, 10, 22050, 2);              /* invalid: ignored */
    check("invalid audio ignored", plat_audio_queued_ms() == 300);

    /* ---- engine-level input contract ---- */
    ll_event e;
    check("empty queue -> poll returns 0", plat_poll(&e) == 0);
    ll_event kd = { LL_EV_KEYDOWN, 42, -1, -1 };
    ll_event qt = { LL_EV_QUIT, 0, 0, 0 };
    plat_dummy_inject(&kd);
    plat_dummy_inject(&qt);
    ll_event got1, got2;
    check("poll delivers injected events in order",
          plat_poll(&got1) == 1 && got1.type == LL_EV_KEYDOWN &&
          got1.key == 42 &&
          plat_poll(&got2) == 1 && got2.type == LL_EV_QUIT);
    check("queue drains -> poll returns 0", plat_poll(&e) == 0);

    /* ---- decode -> present, with no display (the headline test) ---- */
    is_stream s;
    if (is_open(&s, is) != 0) {
        printf("platform_selftest: cannot open %s (test data missing?)\n", is);
        return 1;
    }
    rc = plat_init("headless-selftest", s.w, s.h);
    check("plat_init for .IS content", rc == 0);
    is_decode_all(&s, present_frames, NULL, 5);        /* cap 5 frames */
    unsigned reached = plat_dummy_presented();
    check("frames flowed through plat_present headlessly",
          reached >= 3 && reached == presented_cb_count);
    char m[128];
    snprintf(m, sizeof m, "decoded+presented %u/%dx%d frames (0 display)",
             reached, s.w, s.h);
    printf("platform_selftest: %s (ok)\n", m);
    is_close(&s);

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
