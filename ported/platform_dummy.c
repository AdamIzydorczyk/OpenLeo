/* platform_dummy.c - headless platform backend (platform.h, tips T-01).
 *
 * Presents to nowhere, polls a scripted event queue, keeps a monotonic
 * clock, and accounts queued audio as a cumulative backlog (there is no
 * device to drain it - the point is the API contract + the engine being
 * runnable with no display). Compiled only when PLAT_BACKEND_DUMMY=1,
 * which is exactly the headless selftest target; the game never links
 * it. plat_fopen is backend-agnostic and lives in platform_common.c. */
#include "platform.h"

#include <errno.h>
#include <string.h>
#include <time.h>

static struct {
    int      init;
    unsigned presented;
    size_t   audio_ms;          /* cumulative accepted, no device to drain */
    int      a_rate, a_ch;
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

void plat_audio_queue(const int16_t *s, size_t frames, int rate, int ch)
{
    if (!s || frames == 0 || rate <= 0 || (ch != 1 && ch != 2))
        return;
    d.a_rate = rate;
    d.a_ch = ch;
    d.audio_ms += frames * 1000 / (size_t)rate;   /* cumulative backlog */
}

size_t plat_audio_queued_ms(void)
{
    return d.audio_ms;
}

#if defined(PLAT_BACKEND_DUMMY) && PLAT_BACKEND_DUMMY
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
#endif
