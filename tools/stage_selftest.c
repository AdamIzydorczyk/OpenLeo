/* stage_selftest.c - pins the stage-driver exit contract (ported/ll_stage.h,
 * PR-194). No game data, no window: this is the decision table ONLY, so it
 * runs in the fixture-free half of the gate too. Every case below is the
 * engine's own split as read in PR-193 F3/F4 (NEXT_STEPS.md); the test
 * exists so no later PR can quietly collapse 3 into "continue" or turn the
 * empty-queue leg into "continue".
 * usage: stage_selftest        exit 0 + one "(ok)" line, or exit 1 with
 * the first mismatch on stderr. */
#include "ll_stage.h"

#include <stdio.h>

struct tcase {
    int code;        /* what the wrapper read out of its result pair   */
    int queue;       /* ll_stage_queue_len() input (-1 = "not looked at" */
    int want;        /* the value the wrapper returns                   */
    int want_reset;  /* whether the reset pair runs                     */
    const char *why;
};

static const struct tcase cases[] = {
    /* code 3 -> 0x5d3c6: reset pair first, then 0x5d3dc mov eax,1 */
    { 3, 0, LL_STAGE_AGAIN, 1, "3 = reset pair then again (0x5d3ca/0x5d3d7)" },
    { 3, 4, LL_STAGE_AGAIN, 1, "3 never takes the queue leg" },
    /* code 2 -> call 0x5d2f0, whose OWN value is returned */
    { 2, 0, LL_STAGE_STOP, 0, "2 + empty queue = the 0x5d38e leg" },
    { 2, 1, LL_STAGE_QUEUED, 0, "2 + one entry = it ran, 0x5d368" },
    { 2, 9, LL_STAGE_QUEUED, 0, "2 runs ONE entry per call, not the lot" },
    /* code 4 -> 0x5d3ec xor eax,eax */
    { 4, 0, LL_STAGE_STOP, 0, "4 = stop" },
    { 4, 7, LL_STAGE_STOP, 0, "4 stops even with a queue full of items" },
    /* anything else -> 0x5d3dc mov eax,1, and NO reset */
    { 0, 0, LL_STAGE_AGAIN, 0, "0 = the default leg, no reset" },
    { 1, 0, LL_STAGE_AGAIN, 0, "1 = the default leg, no reset" },
    { 5, 3, LL_STAGE_AGAIN, 0, "above 4 = the same default leg" },
    { -1, 0, LL_STAGE_AGAIN, 0, "the compare is unsigned: negative joins it" },
};

int main(void)
{
    const int n = (int)(sizeof cases / sizeof cases[0]);
    for (int i = 0; i < n; i++) {
        const struct tcase *t = &cases[i];
        int did_reset = -1;
        int got = ll_stage_outcome(t->code, t->queue, &did_reset);
        if (got != t->want || did_reset != t->want_reset) {
            fprintf(stderr,
                    "stage selftest: FAILED exit %d with queue %d: got"
                    " %d (reset %d), the engine gives %d (reset %d) - %s\n",
                    t->code, t->queue, got, did_reset, t->want,
                    t->want_reset, t->why);
            return 1;
        }
    }

    /* the port's own queue: nothing produces entries here, and that is
     * the engine's empty-queue answer, not a stub (see ll_stage.h) */
    if (ll_stage_queue_len() != 0) {
        fprintf(stderr, "stage selftest: FAILED the port's queue length is"
                        " %d, expected 0 (no producer exists yet)\n",
                ll_stage_queue_len());
        return 1;
    }

    printf("stage selftest: the exit-code split exactly as the engine's"
           " per-stage wrapper does it (0x5d3b5-0x5d409): 3 = reset pair"
           " THEN again, 2 = the queue runner (2 after one item, 0 when the"
           " queue is empty), 4 = stop, every other value = again with NO"
           " reset; %d cases (ok)\n", n);
    return 0;
}
