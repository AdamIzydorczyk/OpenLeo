/* ll_audio.h - the audio counterpart of the ll_view session (review
 * P-03 "one SDL lifecycle", completed for audio in PR-12).
 *
 * What IS session-owned (exactly once per process):
 *   - the SDL audio subsystem: ll_audio_session_init() brings it up
 *     lazily and idempotently, applies the pipewire->alsa driver pin
 *     (was ll_audio_init in ll_level.c), and nothing ever calls
 *     SDL_QuitSubSystem(SDL_INIT_AUDIO) except ll_audio_session_close;
 *   - the driver policy and the accepted-spec logging (review P-13):
 *     every device open goes through ll_audio_open, so the spec
 *     contract below is stated in ONE place.
 *
 * What is NOT persistent, deliberately: the device itself. Unlike a
 * window, an SDL audio device is (a) spec-specific - the cutscene
 * stream is 22050 Hz (mono/stereo), the MOD player is 44100 Hz
 * stereo, SDL applies no resampling to queued bytes, and neither
 * stream may be resampled (output is byte-pinned by CI); (b) a stateful
 * FIFO - a kept-open device would leak one scene's queue tail into the
 * next. So the device is acquired per playback (ll_audio_open) and
 * released right after (ll_audio_close); scenes never overlap in
 * playback, so there is still exactly one live device at a time.
 *
 * Device contract (both call sites, A/B-probed 2026-11-14):
 *   want.samples MUST stay 0 - explicit small buffers (1024 squeak,
 *   2048 silent) break SDL2.32's alsa backend; SDL's default plays
 *   byte-perfect. Queue API only, never want.callback (callbacks are
 *   unreliable on SDL2+pipewire/alsa here - see ll_level.c E-09).
 */
#ifndef LL_AUDIO_H
#define LL_AUDIO_H

#include <SDL2/SDL.h>

/* Bring up the audio subsystem once (idempotent; applies the
 * pipewire->alsa pin only when SDL would auto-pick pipewire; an
 * explicit SDL_AUDIODRIVER always wins). 0 = ok, -1 = unavailable. */
int ll_audio_session_init(void);

/* Open a playback device (call after session_init). want is the
 * caller's spec (samples must stay 0, see header); on success the
 * accepted spec is logged (P-13) and the device id is returned; 0 on
 * failure with the SDL error still fetchable. The device is PAUSED:
 * the owner calls SDL_PauseAudioDevice(dev, 0) when its first data is
 * queued. */
SDL_AudioDeviceID ll_audio_open(const SDL_AudioSpec *want);

/* Release a playback device and clear any stale SDL error state so a
 * later SDL call cannot read this close's residue. */
void ll_audio_close(SDL_AudioDeviceID dev);

/* Tear the audio subsystem down explicitly (process exit / tool
 * harnesses). Safe when the subsystem is not up. */
void ll_audio_session_close(void);

#endif /* LL_AUDIO_H */
