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
 * What is NOT session-owned, deliberately: the per-playback STREAM. As
 * of FIX-09 Part 2 (deep dive X-10) the platform backend keeps ONE
 * device open for the whole process, opened at the DEVICE's native
 * rate (a 48000-request + ALLOW_FREQUENCY_CHANGE answer); each
 * playback (22050 cutscenes, 44100 MOD) rides an SDL_AudioStream that
 * converts its s16 bytes to the device spec. That removes the
 * per-scene close/reopen - 50-300 ms plus an audible pop on
 * WASAPI/Bluetooth - while keeping the streams byte-faithful: the
 * engine's own dumps (LEO_IS_WAV, LEO_MUSIC_WAV) still hold the
 * UNresampled source bytes; only the bytes going to the hardware are
 * converted (exactly what SDL's docs prescribe for shared devices).
 * The plat_audio_* surface (platform.h) still presents the old model:
 * one stream at a time, fresh empty queue per stream, queued_bytes in
 * the caller's source format.
 *
 * Device contract (A/B-probed 2026-11-14, still true for the session
 * device):
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

/* Open the session device (call after session_init; the platform
 * backend calls it ONCE and keeps the device). want + allowed_changes
 * go to SDL_OpenAudioDevice verbatim except samples=0 / no callback
 * (see header contract); on success the accepted spec is logged (P-13)
 * and copied to *have_out (may be NULL). Returns the device id, 0 on
 * failure with the SDL error still fetchable. The device is PAUSED:
 * the owner calls SDL_PauseAudioDevice(dev, 0) when its first data is
 * queued. */
SDL_AudioDeviceID ll_audio_open(const SDL_AudioSpec *want,
                                int allowed_changes,
                                SDL_AudioSpec *have_out);

/* Close a device (FIX-09 Part 2: only session teardown may call this;
 * plat_audio_close keeps the device and frees just the stream) and
 * clear any stale SDL error state so a later SDL call cannot read
 * this close's residue. */
void ll_audio_close(SDL_AudioDeviceID dev);

/* Tear the audio subsystem down explicitly (process exit / tool
 * harnesses). Safe when the subsystem is not up. */
void ll_audio_session_close(void);

#endif /* LL_AUDIO_H */
