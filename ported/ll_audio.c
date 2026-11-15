/* ll_audio.c - shared audio session (see ll_audio.h). Moved out of
 * ll_level.c in PR-12 so the audio subsystem lifecycle, the driver
 * pin, and the device/spec contract live in ONE place - the same
 * shape ll_view gives video (review P-03). */
#include "ll_audio.h"

#include <stdio.h>
#include <string.h>

/* Initialize the audio subsystem. History: SDL2 >= 2.32's native
 * "pipewire" backend on the dev desktop (2026-11-14) delivered
 * squeaking or silent streams - but that was observed opening the
 * DEVICE at 22050 Hz through sdl2-COMPAT, and the culprit is now
 * attributed to that path, not to PipeWire. With FIX-09 Part 2 (ONE
 * session device at the hardware's native rate, conversion through
 * SDL_AudioStream) the owner A/B ear test (2026-11-15, LOGO played
 * back-to-back pinned vs unpinned: "seems identical") retired the
 * automatic pin: the default is SDL's own choice. The pin survives as
 * an opt-in escape hatch for environments where a bare pipewire stream
 * still behaves badly: LEO_PIN_ALSA=1. It only fires when SDL would
 * auto-pick pipewire, so Windows (wasapi by default in SDL 2.32) and
 * user-pinned setups are untouched, and a user's SDL_AUDIODRIVER in
 * the environment always wins. Verified against SDL 2.32.10 sources
 * (release-2.32.10): SDL_AudioInit consults
 * SDL_GetHint(SDL_HINT_AUDIODRIVER) - src/audio/SDL_audio.c:969 - and
 * SDL_GetHint prefers the ENV VAR over a non-OVERRIDE hint
 * (src/SDL_hints.c:159), which SDL_SetHint sets. */
/* FIX-05 (deep dive X-07): name the REAL SDL AUDIO_* format values (no
 * *SYS aliases: they duplicate the LSB/MSB cases). The old fmt[] table
 * indexed a made-up 0x800F mask and always printed "?". */
static const char *fmt_name(SDL_AudioFormat f)
{
    switch (f) {
    case AUDIO_U8:     return "U8";
    case AUDIO_S8:     return "S8";
    case AUDIO_S16LSB: return "S16LSB";
    case AUDIO_S16MSB: return "S16MSB";
    case AUDIO_U16LSB: return "U16LSB";
    case AUDIO_U16MSB: return "U16MSB";
    case AUDIO_S32LSB: return "S32LSB";
    case AUDIO_S32MSB: return "S32MSB";
    case AUDIO_F32LSB: return "F32LSB";
    case AUDIO_F32MSB: return "F32MSB";
    default:           return "?";
    }
}

/* FIX-09 Part 1 (deep dive L-02): 1 whenever the alsa pin below won, so
 * ll_audio_open knows a device-open failure is likely "no alsa plugin
 * for this server" rather than "no audio at all", and may retry once on
 * SDL's own choice. Cleared by that retry (one attempt per process). */
static int s_forced_alsa;

int ll_audio_session_init(void)
{
    if (SDL_WasInit(SDL_INIT_AUDIO))
        return 0;
    if (SDL_getenv("SDL_AUDIODRIVER"))
        return SDL_InitSubSystem(SDL_INIT_AUDIO) == 0 ? 0 : -1;
    /* PR-35 (FIX-09 pin retirement, owner A/B ear test): default is
     * SDL's own choice. LEO_PIN_ALSA=1 restores the automatic alsa pin
     * for environments where a bare pipewire stream misbehaves. The
     * former opt-out LEO_KEEP_PIPEWIRE=1 is the default now; it stays
     * accepted as a no-op so nobody's setup breaks. */
    const char *pin = SDL_getenv("LEO_PIN_ALSA");
    if (!pin || strcmp(pin, "1") != 0)
        return SDL_InitSubSystem(SDL_INIT_AUDIO) == 0 ? 0 : -1;
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
        return -1;
    const char *drv = SDL_GetCurrentAudioDriver();
    if (drv && strcmp(drv, "pipewire") == 0) {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        /* FIX-09: a HINT, not setenv - it does not leak into child
         * processes, and a user's SDL_AUDIODRIVER still wins (see the
         * precedence proof above). */
        SDL_SetHint(SDL_HINT_AUDIODRIVER, "alsa");
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) == 0) {
            s_forced_alsa = 1;
            printf("ll_audio: alsa driver pinned (LEO_PIN_ALSA=1)\n");
            return 0;
        }
        /* alsa absent: drop our hint so SDL picks its own default again
         * (fixes the old leak where "pipewire" became the user's env). */
        SDL_SetHint(SDL_HINT_AUDIODRIVER, NULL);
        return SDL_InitSubSystem(SDL_INIT_AUDIO) == 0 ? 0 : -1;
    }
    return 0;
}

SDL_AudioDeviceID ll_audio_open(const SDL_AudioSpec *want,
                                int allowed_changes,
                                SDL_AudioSpec *have_out)
{
    /* Enforce the device contract (ll_audio.h) rather than trusting
     * callers: default-size buffer, queue API only. */
    SDL_AudioSpec w = *want;
    w.callback = NULL;
    w.samples = 0;
    SDL_AudioSpec have;
    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &w, &have,
                                                allowed_changes);
    if (!dev && s_forced_alsa) {
        /* FIX-09 (deep dive L-02): alsa initialised but cannot open a
         * device (no pipewire-alsa/pulse-alsa plugin, Flatpak sandbox,
         * exclusive-mode device): go back to SDL's own choice ONCE
         * instead of running silent the whole session. Safe to restart
         * the subsystem here because of the one-device-at-a-time
         * contract - nothing else holds a device at this point. */
        fprintf(stderr, "ll_audio: alsa open failed (%s), "
                        "retrying with default driver\n", SDL_GetError());
        s_forced_alsa = 0;
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        SDL_SetHint(SDL_HINT_AUDIODRIVER, NULL);
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) == 0)
            dev = SDL_OpenAudioDevice(NULL, 0, &w, &have, allowed_changes);
    }
    if (!dev)
        return 0;
    if (have_out)
        *have_out = have;
    /* P-13: allowed_changes=0 means SDL converts silently to what the
     * backend really runs; log the accepted spec so capture/diagnostic
     * runs can be trusted. */
    /* Print the accepted spec + the driver that actually answered, so
     * capture/diagnostic lines can be trusted (FIX-05; the 'll_audio:
     * device' prefix is what diagnostics key on - only its content got
     * honest). */
    fprintf(stderr, "ll_audio: device %s %uHz %uch %u samples (driver %s)\n",
            fmt_name(have.format), (unsigned)have.freq,
            (unsigned)have.channels, (unsigned)have.samples,
            SDL_GetCurrentAudioDriver() ? SDL_GetCurrentAudioDriver()
                                        : "?");
    return dev;
}

void ll_audio_close(SDL_AudioDeviceID dev)
{
    if (!dev)
        return;
    SDL_CloseAudioDevice(dev);
    SDL_ClearError(); /* no stale error for later unrelated SDL calls */
}

void ll_audio_session_close(void)
{
    if (SDL_WasInit(SDL_INIT_AUDIO))
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
