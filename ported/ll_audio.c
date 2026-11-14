/* ll_audio.c - shared audio session (see ll_audio.h). Moved out of
 * ll_level.c in PR-12 so the audio subsystem lifecycle, the driver
 * pin, and the device/spec contract live in ONE place - the same
 * shape ll_view gives video (review P-03). */
#include "ll_audio.h"

#include <stdio.h>
#include <string.h>

/* Initialize the audio subsystem, dodging SDL2 >= 2.32's native
 * "pipewire" backend, which on the dev desktop (SDL 2.32.74 +
 * PipeWire, 2026-11-14) delivered squeaking or silent streams for
 * BOTH the callback and QueueAudio paths while the alsa backend
 * played the very same bytes clean. Only fires when SDL would
 * auto-pick pipewire, so Windows (directsound) and older/user-pinned
 * setups are untouched; LEO/SDL_AUDIODRIVER in the environment
 * always wins. */
int ll_audio_session_init(void)
{
    if (SDL_WasInit(SDL_INIT_AUDIO))
        return 0;
    if (SDL_getenv("SDL_AUDIODRIVER"))
        return SDL_InitSubSystem(SDL_INIT_AUDIO) == 0 ? 0 : -1;
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
        return -1;
    const char *drv = SDL_GetCurrentAudioDriver();
    if (drv && strcmp(drv, "pipewire") == 0) {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        SDL_setenv("SDL_AUDIODRIVER", "alsa", 1);
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) == 0) {
            printf("ll_audio: SDL pipewire backend unreliable, "
                   "audio on alsa driver\n");
            return 0;
        }
        SDL_setenv("SDL_AUDIODRIVER", drv, 1);   /* restore, alsa absent */
        return SDL_InitSubSystem(SDL_INIT_AUDIO) == 0 ? 0 : -1;
    }
    return 0;
}

SDL_AudioDeviceID ll_audio_open(const SDL_AudioSpec *want)
{
    /* Enforce the device contract (ll_audio.h) rather than trusting
     * callers: default-size buffer, queue API only. */
    SDL_AudioSpec w = *want;
    w.callback = NULL;
    w.samples = 0;
    SDL_AudioSpec have;
    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &w, &have, 0);
    if (!dev)
        return 0;
    /* P-13: allowed_changes=0 means SDL converts silently to what the
     * backend really runs; log the accepted spec so capture/diagnostic
     * runs can be trusted. */
    static const char *const fmt[] = {
        "U8", "S8", "S16LSB", "S16MSB", "S16SYS", "S32LSB",
        "S32MSB", "S32SYS", "F32LSB", "F32MSB", "F32SYS",
    };
    unsigned f = have.format & 0x800F;
    fprintf(stderr, "ll_audio: device %s %uHz %uch %u samples\n",
            f < sizeof fmt / sizeof fmt[0] ? fmt[f] : "?",
            (unsigned)have.freq, (unsigned)have.channels,
            (unsigned)have.samples);
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
