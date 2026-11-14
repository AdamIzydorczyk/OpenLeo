/* platform_dummy.c - headless platform backend (platform.h, tips T-01).
 *
 * Presents to nowhere, polls a scripted event queue, keeps a monotonic
 * clock, and mirrors the audio device contract as a byte FIFO (there is
 * no device to drain it - the point is the API contract + the engine
 * being runnable with no display). The whole backend compiles ONLY when
 * PLAT_BACKEND_DUMMY=1 (the headless selftest target) - the same
 * compile-time backend switch described in platform.h; a game target
 * that globs its sources simply compiles this file to nothing.
 * plat_fopen is backend-agnostic and lives in platform_common.c. */
#include "platform.h"

#if defined(PLAT_BACKEND_DUMMY) && PLAT_BACKEND_DUMMY

#include <errno.h>
#include <string.h>
#include <time.h>

static struct {
    int      init;
    unsigned presented;
    int      a_open;            /* one-device-at-a-time model, no device */
    int      a_paused;
    size_t   a_queued;          /* bytes "in the FIFO" (nothing drains) */
    ll_event q[64];
    unsigned qh, qt;            /* ring: [qh,qt) */
} d;

int plat_init(const char *title, int w, int h)
{
    (void)title;
    if (w <= 0 || h <= 0)
        return -1;
    d.init = 1;                 /* idempotent re-init (matches ll_view) */
    return 0;
}

void plat_present(const uint8_t *idx8, const uint8_t pal[768], int w, int h)
{
    if (!idx8 || !pal || w <= 0 || h <= 0)
        return;                 /* sink ignores invalid input */
    d.presented++;
}

int plat_poll(ll_event *ev)
{
    if (!ev || d.qh == d.qt)
        return 0;
    *ev = d.q[d.qh];
    d.qh = (d.qh + 1) % (sizeof d.q / sizeof d.q[0]);
    return 1;
}

uint64_t plat_now_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

void plat_sleep_until(uint64_t ns)
{
    struct timespec req;
    req.tv_sec  = (time_t)(ns / 1000000000ull);
    req.tv_nsec = (long)(ns % 1000000000ull);
    while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &req, NULL) != 0)
        ;                        /* EINTR: just retry, absolute deadline */
}

/* Audio: a byte-FIFO model of the SDL backend's contract - open/close
 * one device at a time, queue bytes against it, watch the backlog,
 * pause/clear. Nothing drains (no device); the point is the engine
 * uses the API correctly (open before queue, one device at a time). */
int plat_audio_open(int rate_hz, int channels)
{
    if (d.a_open || rate_hz <= 0 || (channels != 1 && channels != 2))
        return -1;
    d.a_open = 1;
    d.a_paused = 0;
    d.a_queued = 0;
    return 0;
}

void plat_audio_close(void)
{
    d.a_open = 0;
    d.a_paused = 0;
    d.a_queued = 0;              /* the FIFO goes with the device */
}

void plat_audio_queue(const void *bytes, size_t len)
{
    if (!d.a_open || !bytes || len == 0)
        return;
    d.a_queued += len;
}

size_t plat_audio_queued_bytes(void)
{
    return d.a_queued;
}

void plat_audio_pause(int paused)
{
    if (d.a_open)
        d.a_paused = paused ? 1 : 0;
}

void plat_audio_clear(void)
{
    if (d.a_open)
        d.a_queued = 0;
}

const char *plat_last_error(void)
{
    return "";
}

/* test hooks: declared in platform.h under the same PLAT_BACKEND_DUMMY
 * switch (script events in, presented-frame count out). */
void plat_dummy_inject(const ll_event *ev)
{
    if (!ev)
        return;
    unsigned next = (d.qt + 1) % (sizeof d.q / sizeof d.q[0]);
    if (next == d.qh)
        return;                 /* full: drop, tests keep the queue small */
    d.q[d.qt] = *ev;
    d.qt = next;
}

unsigned plat_dummy_presented(void)
{
    return d.presented;
}

#endif /* PLAT_BACKEND_DUMMY */
