/* platform.h - thin platform interface (tips T-01 / T-02).
 *
 * The engine talks ONLY to this surface: an 8-bit indexed framebuffer is
 * its output (T-02), input comes back as engine-level ll_events, timing
 * and audio-queue control go through plat_*, and file opens go through
 * plat_fopen. No engine screen includes SDL directly.
 *
 * Why now: the concrete pain T-01 named (one window per scene, SDL_Init
 * per cutscene, OS quirks repeated in 4 screens) was already collapsed
 * into the ll_view (PR-8/10) and ll_audio (PR-12) sessions. This header
 * is the engine-facing façade over those. Two backends exist so the same
 * engine runs headless (tests) or on a real window (the game):
 *   PLAT_BACKEND_SDL   - platform_sdl.c, delegating to ll_view/ll_audio.
 *   PLAT_BACKEND_DUMMY - platform_dummy.c, headless (tests only).
 * Exactly one is compiled per target (same knob style as PLATFORM_* in
 * ll_config.h). The cutscene player is migrated to this surface in
 * PR-15; the title menu / chapter map / level scene still drive
 * ll_view/ll_audio directly and migrate in a later PR.
 *
 * The audio API is intentionally byte-based and carries open/pause/clear
 * (the review's T-01 sketch showed only queue+queued_ms). The real
 * player opens one device at a fixed spec, pauses it together with its
 * clock during pause (A/V stay in step, P-02), clears the queue on an
 * ESC skip, and drains it to know when a cutscene has really finished.
 * Byte units preserve that pacing contract exactly (no rate conversions
 * in the engine).
 *
 * Framebuffer + palette are the engine's NATIVE form: idx8 is w*h bytes
 * already LINEARISED to plane order (is_linearize_into), pal is 768
 * bytes of raw 6-bit DAC triples (the .IS layout); the backend applies
 * the 8-bit expansion (v<<2) and blits. Audio buffers are s16 already
 * produced by the engine's ll_mod/is decoders - plat only owns the
 * device, its queue, and its backlog.
 */
#ifndef PLATFORM_H
#define PLATFORM_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/* Engine-level input (T-01). Backend-agnostic: the SDL backend maps
 * SDL keyboard/mouse/quit events here (printable keys pass through as
 * their ASCII code; special keys get LLKEY_* ids; mouse x,y arrive in
 * LOGICAL viewport coordinates - the backend already un-scaled them,
 * P-10), the dummy backend injects scripted events for tests. */
typedef struct {
    int type;
    int key;
    int x, y;
} ll_event;

enum {
    LL_EV_NONE = 0,
    LL_EV_KEYDOWN,
    LL_EV_KEYUP,
    LL_EV_QUIT,
    LL_EV_BUTTON_DOWN,   /* mouse press; `key` = LLBTN_* */
    LL_EV_MOTION         /* mouse move; x,y updated */
};

/* Mouse buttons the engine handles (only the primary today; the menu
 * hit-tests zones with the left button, matching the original). */
enum {
    LLBTN_LEFT = 1
};

/* Non-printable keys the engine handles (printable keys are just their
 * ASCII code in ll_event.key, including space 0x20). Values are
 * engine-defined, NOT SDL keysyms (T-15 keeps the keycode-based model:
 * arrows/Enter/etc. navigate menus in every backend). */
enum {
    LLKEY_ESC = 1,
    LLKEY_UP, LLKEY_DOWN, LLKEY_LEFT, LLKEY_RIGHT,
    LLKEY_RETURN,          /* Return AND keypad Enter */
    LLKEY_HOME, LLKEY_END, LLKEY_PGUP, LLKEY_PGDN
};

/* Bring the platform up for content of this title/size. 0 = ok, -1 =
 * failure. Idempotent: repeated calls reconfigure the session (mirrors
 * the ll_view window session). */
int plat_init(const char *title, int w, int h);

/* P-10 display overrides (set by the app before plat_init; 0 = the
 * historical default geometry). The backend consumes them at window
 * creation; declared here so the app sets them through the interface
 * rather than reaching into ll_view.h. plat_opt_scale > 0 forces the
 * initial window multiple; plat_opt_fullscreen opens fullscreen. */
extern int plat_opt_scale;
extern int plat_opt_fullscreen;

/* Present one indexed frame (T-02). idx8 is w*h LINEARISED indexed
 * bytes, pal is 768 raw 6-bit triples; the backend expands + blits. */
void plat_present(const uint8_t *idx8, const uint8_t pal[768], int w,
                  int h);

/* Present one ARGB8888 frame: fb is w*h little-endian 0xAARRGGBB
 * pixels. This is the NATIVE form of the compositing screens (the
 * chapter map composites PCX + .SEQ sprites + text on the CPU into
 * exactly this buffer); the backend just uploads and flips, no
 * conversion, so pixels are unchanged by definition. */
void plat_present_argb(const uint32_t *fb, int w, int h);

/* Ask the window manager for focus (dialog/screen re-entry). On
 * X11/XWayland this is a hard input grab, on Wayland the WM decides -
 * callers must therefore keep pointer/keyboard handling tolerant.
 * No-op when no window exists. */
void plat_raise(void);

/* Pop the next engine event into *ev. Returns 1 if an event was filled,
 * 0 when the queue is empty. Window-management shortcuts the engine must
 * not see (F11 / Alt+Enter fullscreen, review P-10) are consumed by the
 * backend here and never surfaced. */
int plat_poll(ll_event *ev);

/* Monotonic clock (ns) + an absolute wake-up, for fixed-step pacing
 * (tips T-05/T-06). plat_now_ns never goes backwards; plat_sleep_until
 * returns no earlier than the given timestamp. */
uint64_t plat_now_ns(void);
void plat_sleep_until(uint64_t ns);

/* Audio device owned by the platform. plat_audio_open(rate,ch) opens one
 * device (0 = ok, device left playing; -1 = audio subsystem unavailable;
 * -2 = subsystem up but the device could not be opened - the player
 * keeps both old diagnostics distinct). plat_audio_close() releases it
 * (no-op when none is open). The engine mixes s16 already, so the
 * platform only moves bytes: queue bytes, ask how many are still
 * outstanding, pause/resume (for A/V-locked pausing), clear (skip). */
int    plat_audio_open(int rate_hz, int channels);
void   plat_audio_close(void);
void   plat_audio_queue(const void *bytes, size_t len);
size_t plat_audio_queued_bytes(void);
void   plat_audio_pause(int paused);
void   plat_audio_clear(void);

/* Open a game-data file: UTF-8 + case-insensitive (delegates to the
 * fs_ci opener, tips T-18). NULL if unresolvable. */
FILE *plat_fopen(const char *path, const char *mode);

/* One platform-owned worker thread (review E-09: the music pump must
 * top the audio queue independently of the UI loops). Exactly one may
 * run at a time - matching the music singleton's own invariant.
 * plat_thread_start runs fn(ctx) and returns 0; on failure it returns
 * -1 (reason via plat_last_error) and the engine falls back to its
 * documented UI-loop pump. The caller owns the stop protocol (an
 * atomic flag in ctx); plat_thread_join just waits for exit.
 * The headless dummy backend reports "no threads here" = -1, which
 * exercises exactly that fallback path in tests. */
int  plat_thread_start(int (*fn)(void *), void *ctx, const char *name);
void plat_thread_join(void);

/* ---- host glue (T-01 phase 5: the entry code keeps NO direct SDL) ---- *
 * These four cover everything ported/main.c and ported/ll_paths.c used
 * to call SDL for. They are deliberately NOT in any engine contract:
 * they belong to the app bootstrap, so they stay out of the screen
 * code and selftest keeps their headless answers pinned. */

/* Directory containing the running executable, malloc'd (caller frees
 * with free()); NULL when the platform cannot say. May run before any
 * plat_init (the SDL backend delegates to SDL_GetBasePath, which is
 * documented to work uninitialized). */
char *plat_base_path(void);

/* Modal user-facing error report (review P-06: a double-click launch
 * has no console). Policy lives backend-side: the SDL backend shows a
 * real box on Windows only - elsewhere the caller's stderr line IS
 * the report, and an extra box would just block the shell. */
void plat_message_box(const char *title, const char *text);

/* Per-user WRITABLE directory, created if missing (the SDL backend
 * delegates to SDL_GetPrefPath("LongSoft","LewLeon"): %APPDATA% on
 * Windows, ~/.local/share on Linux). 0 = path written into out (a
 * NUL-terminated string within cap), -1 = none available. */
int plat_pref_dir(char *out, size_t cap);

/* One-time video backend selection (P-04 focus policy): under a
 * Wayland session prefer X11/XWayland, where the focus-grant calls
 * actually work; an explicit SDL_VIDEODRIVER always wins. Call once
 * at startup, before any plat_init. Headless: no-op. The two
 * diagnostic lines are user-facing and byte-pinned by the port. */
void plat_prepare_video(void);

/* Last backend error as a printable string (diagnostics only - the
 * engine must not branch on its content). The SDL backend returns
 * SDL_GetError(), so existing "window: ..." / "audio off (...)"
 * diagnostics stay byte-identical; never NULL. */
const char *plat_last_error(void);

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
