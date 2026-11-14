/* platform.h - thin platform interface (tips T-01 / T-02).
 *
 * The engine talks ONLY to this surface: an 8-bit indexed framebuffer is
 * its output (T-02), events come back as engine-level ll_events, timing
 * and audio-queue accounting go through plat_*, and file opens go
 * through plat_fopen. Nothing in the engine includes SDL directly.
 *
 * Why now: the concrete pain T-01 named (one window per scene, SDL_Init
 * per cutscene, OS quirks repeated in 4 screens) was already collapsed
 * into the ll_view (PR-8/10) and ll_audio (PR-12) sessions. This header
 * is the engine-facing façade over those, and the load-bearing piece is
 * the SECOND backend: a headless "dummy" platform (platform_dummy.c)
 * that lets the decode->present->audio path run with no display and no
 * audio device - i.e. real headless engine tests (T-01's named benefit),
 * which today are impossible because every screen hard-wires SDL.
 *
 * Backends are selected at COMPILE time (exactly one per target), the
 * same knob style as PLATFORM_* in ll_config.h:
 *   PLAT_BACKEND_SDL   - platform_sdl.c, delegating to ll_view/ll_audio.
 *   PLAT_BACKEND_DUMMY - platform_dummy.c, headless (tests only).
 * The game currently drives ll_view/ll_audio directly; migrating the
 * screens to this interface is the follow-up PR (see NEXT_STEPS). Until
 * then only the DUMMY backend is compiled into a target (the headless
 * selftest), so this is purely additive.
 *
 * Framebuffer + palette are the engine's NATIVE form: idx8 is w*h
 * indexed bytes and pal is 768 bytes of RGB triples (the .IS 6-bit DAC
 * layout; backends apply their own 8-bit conversion, see
 * is_palette_rgb8). Audio is s16 interleaved, already mixed by the
 * engine's ll_mod/is decoders - plat only owns the device + backlog.
 */
#ifndef PLATFORM_H
#define PLATFORM_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/* Engine-level input (T-01). Deliberately backend-agnostic: the SDL
 * backend maps SDL_KeyboardEvent/mouse here; the dummy backend injects
 * scripted events for tests. `key` is an engine scancode (T-15 keeps the
 * keycode-based input model), x/y are logical-viewport coordinates. */
typedef struct {
    int type;
    int key;
    int x, y;
} ll_event;

enum {
    LL_EV_NONE = 0,
    LL_EV_KEYDOWN,
    LL_EV_KEYUP,
    LL_EV_QUIT
};

/* Bring the platform up for content of this title/size. 0 = ok, -1 =
 * failure. Idempotent: repeated calls reconfigure (mirror the ll_view
 * session). */
int plat_init(const char *title, int w, int h);

/* Present one indexed frame (T-02). idx8 is w*h, pal is 768 triples. */
void plat_present(const uint8_t *idx8, const uint8_t pal[768], int w,
                  int h);

/* Pop the next engine event into *ev. Returns 1 if an event was filled,
 * 0 when the queue is empty. */
int plat_poll(ll_event *ev);

/* Monotonic clock (ns) + an absolute wake-up, for fixed-step pacing
 * (tips T-05/T-06). plat_now_ns never goes backwards; plat_sleep_until
 * returns no earlier than the given timestamp. */
uint64_t plat_now_ns(void);
void plat_sleep_until(uint64_t ns);

/* Queue s16 interleaved audio (frames = total sample frames across ch
 * channels, rate in Hz). plat_audio_queued_ms() reports how much is
 * still outstanding, so the engine keeps the pump full (T-09). */
void plat_audio_queue(const int16_t *s, size_t frames, int rate, int ch);
size_t plat_audio_queued_ms(void);

/* Open a game-data file: UTF-8 + case-insensitive (delegates to the
 * fs_ci opener, tips T-18). NULL if unresolvable. */
FILE *plat_fopen(const char *path, const char *mode);

#if defined(PLAT_BACKEND_DUMMY) && PLAT_BACKEND_DUMMY
/* Headless test hooks (not part of the real SDL backend):
 *   plat_dummy_inject  - append a scripted event so plat_poll has
 *                        something to deliver (exercises the input
 *                        contract headlessly);
 *   plat_dummy_presented - how many plat_present calls the headless
 *                        sink accepted, so a test can prove frames
 *                        actually flowed through the interface. */
void plat_dummy_inject(const ll_event *ev);
unsigned plat_dummy_presented(void);
#endif

#endif /* PLATFORM_H */
