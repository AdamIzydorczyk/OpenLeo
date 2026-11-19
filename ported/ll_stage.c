/* ll_stage.c - the stage-driver exit contract; the contract, the engine
 * addresses and the counted-unknown list are in ll_stage.h (PR-194).
 * Deliberately small and PURE: this is the engine's decision table, not a
 * game loop. The caller (the level harness in tools/ll_level.c's play
 * leg) is what performs the reset, the stop or the continue. */
#include "ll_stage.h"

/* Nothing here produces queue entries, and the engine's own producer of
 * the ds:0xcf464 count is unread (ll_stage.h, "what is NOT known"). An
 * empty queue is not a stub either: it is one of the engine's own cases -
 * 0x5d308 reads the count, 0x5d30e/0x5d310 `test/je` jumps over the whole
 * body to 0x5d38e `xor ebx,ebx`, and 0x5d390 returns it. */
int ll_stage_queue_len(void)
{
    return 0;
}

int ll_stage_outcome(int exit_code, int queue_len, int *did_reset)
{
    if (did_reset)
        *did_reset = 0;

    /* The engine compares UNSIGNED: 0x5d3b8 `cmp edx,0x3` with `jb` /
     * `jbe`, then `cmp edx,0x4`, `je` / `jmp`. So a negative value (as
     * unsigned: very large) falls in the same leg as 0 and 1. */
    switch (exit_code) {
    case 3:
        /* 0x5d3c6: the reset pair runs FIRST (eax = the stage object,
         * `call 0x5da60` at 0x5d3ca, `call 0x5d050` at 0x5d3d7), and the
         * code then falls through to 0x5d3dc `mov eax,0x1`. The pair's
         * internals are not read; what the caller DOES is reset + again. */
        if (did_reset)
            *did_reset = 1;
        return LL_STAGE_AGAIN;
    case 2:
        /* 0x5d3f0 `cmp edx,0x2` / 0x5d3f3 `je 0x5d3e3` -> `call 0x5d2f0`,
         * and the wrapper returns WHAT THAT RETURNS: 2 after it ran one
         * queued item (0x5d368 `mov ebx,0x2` + the pop at
         * 0x5d375-0x5d378), 0 when the queue was empty (0x5d38e) or when
         * the run verb answered 0 for the head item (0x5d385, which pops
         * nothing and leaves ebx 0). */
        return queue_len > 0 ? LL_STAGE_QUEUED : LL_STAGE_STOP;
    case 4:
        /* 0x5d3ec `xor eax,eax` -> 0: the caller chain stops. */
        return LL_STAGE_STOP;
    default:
        /* 0x5d3dc `mov eax,0x1`, reached from 0x5d3f0 (codes 0 and 1) and
         * from the `jmp` after the compare with 4 (codes > 4): continue,
         * and above all NO reset. This leg and the code-3 leg return the
         * same value; the reset is the whole difference, which is why
         * *did_reset is part of the answer. */
        return LL_STAGE_AGAIN;
    }
}
