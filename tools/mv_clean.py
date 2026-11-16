"""mv_clean.py - temporal de-ghost filter for .LVM re-conversion (PR-41).

STATUS 2026-11-16: PR-41 was SHELFED by the owner (player stays .IS-
only; see NEXT_STEPS.md PR-41 (SHELVED) record). Kept as a standalone
offline tool - NOT run by CI.

Measured fact (tools/mv_pattern.py + per-run metric tables, 2026-11-15,
all 15 .IS): the 1997 encoder often delivers a new picture in SEVERAL
consecutive payloads, each updating part of the frame - 3835 of 6838
inter-frame steps are such partial repaints. The intermediate states
are half-old/half-new (the "ghost" the owner asked to remove); the
ORIGINAL machine showed them too, so this filter is a deliberate,
opt-in NON-faithful cleanup that shows only the completed frames
(owner: "convert all movies to a less problematic format" -> option 1,
"all movies", 2026-11-15).

Two forms of partial-repaint runs exist (measured): a geometric sweep
with a traveling head (ZARIA1, ENA1 - monoX = 1.00), and CONVERGING
refinements of one new scene with no geometry at all (KADMAR: per-step
new-pixel counts decay 17k -> 8k -> 7k -> ... ). Geometry tests fit
only the first form, and the second form has no reliable geometry;
the real signature is TEMPORAL:

    a paint run is ADDITIVE - once a pixel leaves the run's base
    value it never returns to it;
    real motion RESTORES - a moving object uncovers background again,
    so changed pixels return to the base value mid-run.

Batch planner over the movie's decoded frame sequence (no invented
pixels: every emitted frame is one the real decoder produced):

  classify each step t (frame t vs t-1) by changed fraction:
      SAME  0            (already merged into hold-only records)
      FULL  frac >= 0.30   complete update - keep
      PART  0.005..0.30   progressive-delivery candidate
      NOISE < 0.005       small REAL change - keep (AUTORZY's
                          1-px-per-frame text scroll lives here)

  a maximal PART run frames[s..e] (base = s-1) is MERGED - all of
  s..e-1 suppressed, final frame e kept, their hold times stay in
  place so total duration is byte-for-byte identical - only when:
    (1) run length >= 3;
    (2) net persistence: >= 70% of the pixels changed in the run
        STILL differ from the base at the run's final frame;
    (3) additivity: <= 40% of the changed pixels ever RETURN to the
        base value at an intermediate step (real motion / dithered
        dissolves restore -> rejected -> kept untouched).
    Measured separation across all 15 movies: motion runs persist
    0.03-0.63 / restore 0.50-0.99; paint runs persist 0.70-1.00 /
    restore 0.00-0.36; the 0.40 cut sits in the empty gap. PRAWA's
    live beach, INTRO_P/LOGO's real animation, walk cycles and text
    scrolls all stay byte-identical by these rules.

python is the CI oracle; ported/lvm.c only replays the result.
"""
import numpy as np

FULL = 0.30
NOISE = 0.005
MIN_RUN = 3
NET_PERSIST = 0.70
MAX_RESTORED = 0.40


def classify_fracs(frames):
    """frames: list of np.uint8 (h, w). Returns float fracs[1..]."""
    out = []
    prev = None
    for f in frames:
        if prev is not None:
            out.append(float(np.count_nonzero(f != prev)) / f.size)
        prev = f
    return out


def _paint_run(frames, s, e):
    """Decide run frames[s..e] over base frames[s-1]: additive paint?"""
    if e - s + 1 < MIN_RUN:
        return False
    base = frames[s - 1]
    changed = np.zeros(frames[s].shape, bool)
    away = np.zeros(frames[s].shape, bool)
    restored = np.zeros(frames[s].shape, bool)
    for t in range(s, e + 1):
        now = frames[t] != base
        restored |= away & ~now      # left the base, came back -> motion
        away |= now
        changed |= now
    nchanged = int(np.count_nonzero(changed))
    if not nchanged:
        return False
    # (2) net persistence at the run's final frame
    persist = int(np.count_nonzero(away & (frames[e] != base)))
    if persist < NET_PERSIST * nchanged:
        return False
    # (3) additivity: pixels never return to the base value
    if int(np.count_nonzero(restored)) > MAX_RESTORED * nchanged:
        return False
    return True


def plan(frames):
    """Returns (suppress: set of frame indices to drop, stats dict)."""
    fr = classify_fracs(frames)
    suppress = set()
    st = {'runs': 0, 'frames_tested': 0, 'frames_merged': 0,
          'runs_rejected': 0, 'part_steps': 0, 'full_steps': 0,
          'noise_steps': 0}
    t = 1
    n = len(frames)
    while t < n:
        f = fr[t - 1]
        if NOISE <= f < FULL:
            st['part_steps'] += 1
            s = t
            while t + 1 < n and NOISE <= fr[t] < FULL:
                t += 1
                st['part_steps'] += 1
            e = t
            st['frames_tested'] += e - s + 1
            if _paint_run(frames, s, e):
                st['runs'] += 1
                st['frames_merged'] += e - s
                suppress.update(range(s, e))
            elif e - s + 1 >= MIN_RUN:
                st['runs_rejected'] += 1
        else:
            if f >= FULL:
                st['full_steps'] += 1
            elif f > 0:
                st['noise_steps'] += 1
        t += 1
    return suppress, st
