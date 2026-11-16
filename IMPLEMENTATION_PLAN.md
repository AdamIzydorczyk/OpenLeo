# IMPLEMENTATION_PLAN.md — executing the Nov-2026 deep dive (PR-25+)

> **EXECUTED.** The PR-25+ work order below was carried out in PR-25..PR-35
> (records in NEXT_STEPS.md); it is kept as the rationale behind those PRs.

Input: `PORT_DEEP_DIVE_2026-11.md` (at `4c48dd0`). Every load-bearing
claim was spot-checked against the code at HEAD before planning: ENTER
raise (ll_view.c:92-93), SetWindowInputFocus (platform_sdl.c:127), the
2ms sleep-spin (SLACK/SDL_Delay(0)), `have.format & 0x800F` (ll_audio.c:59),
`cam += 50 * (int)dt / 1000` (ll_level.c:1676), post-work 20/30ms ticks
(ll_level.c:1072,1693 / ll_menu.c:647,1063), the three unused knobs in
ll_config.h, fixed wchar_t[4096] (fs_ci.c:28), SDL_setenv x11/wayland/alsa
pins, `is_player_run` returning 0 for quit (is_player.c:494), plain fseek
(llrs.c:97). CONFIRMED — the specs' anchors are real.

Part B says "one FIX = one PR unless stated"; this plan groups exactly as
the doc's §5 roadmap proposes, keeps this repo's PR-24 discipline (every
PR: `make` + `make win` warning-free, `make -k verify-all` green locally
WITH data and green in CI WITHOUT data, verify-arch green, no game data
committed, LEO_* hooks additive).

## Conflict rulings first (against decisions already recorded in this repo)

OWNER CALL (2026-11-15): the conflicted items (C1 Part 2, C2 default flip,
C3) are LEFT UNDECIDED AND DEFERRED TO THE END of the PR-25..32 series;
execute everything conflict-free first, present the three decisions together
at the end with the evidence the landed PRs produced.

| # | Doc item | Past decision it touches | Ruling |
|---|---|---|---|
| C1 | FIX-09 **Part 2** (one 48 kHz device for the session) | PR-12/P-03 note: "the device itself is per-playback **by design**" | **CONFLICT — needs owner sign-off.** Part 1 (SetHint instead of setenv, open-failure driver fallback) is uncontroversial and lands. Part 2 reverses the recorded design; the doc gives real reasons (WASAPI/BT reopen pops 50-300 ms, 22050 device, sdl2-compat resampler). Ship Part 1 in PR-29; park Part 2 until owner re-approves. |
| C2 | FIX-11 (native Wayland becomes the DEFAULT, `LEO_FORCE_X11=1` opt-in) | P-05: "current pipewire→alsa swap works; revisit if users report issues" | **SOFT CONFLICT — revisit condition is now legitimately met** (the stated X11 rationale was programmatic raising, which FIX-02 removes). Still flips user-visible default → needs owner sign-off; implement behind the flag so the flip is a one-line default change. |
| C3 | FIX-01 step 6 (`q` quits the whole app) | doc itself: "needs owner sign-off; do not do by default" | **Skip by default.** One-line add later if desired. |
| C4 | Part A X-04 third bullet (scancode key mapping, T-15) | doc's own correction + existing LEO_MENU_KEY-style hooks are label/keysym-based | **No scancodes** — Part B (FIX-03 "Do not") governs. Also protects existing env-hook semantics. |
| C5 | B-02 (wire `DATA_DIR_FROM_CFG` via openleo.ini) | T-17 remainder: "needs a settings subsystem first" | **Keep config-file work deferred**; FIX-13 step 8 only marks/deletes the knobs. TICK_HZ: comment it documents the ORIGINAL 70 Hz PIT rate; not wired (FIX-07 itself says so). |
| C6 | FIX-07 fixed-step loops | suite pins at fixed PPM ticks (verify-menu/sel/verify-golden) | **No conflict** — per-tick content unchanged; re-run those data legs locally as the gate. |
| C7 | FIX-12 ASan CI leg | known ASan report: all third-party one-time allocations, appears only with a REAL audio device | No conflict — CI leg runs `SDL_AUDIODRIVER=dummy` (the documented leak-clean config). |
| C8 | FIX-02 (remove raise-on-enter) | P-04 was already "don't steal focus" | Not a conflict — this is a regression fix of an existing decision. |

No other item conflicts: FIX-08's audio-master implements the already-stated
IMPROVEMENTS §1 goal under the "pixels only by when, never what" guard
(LEO_IS_WAV `cmp` byte-identical + PPM legs unchanged are its gates).

## Execution order

Ordered by risk-to-value as the doc suggests (PR-25 → PR-26 → CI → the
rest), with the CI work moved up so later PRs get native-Windows runtime
coverage:

1. **PR-25 — FIX-01 quit propagation** (X-01, HIGH). Sticky `quit_req` in
   both backends + `plat_quit_requested/plat_request_quit`, `is_player_run`
   new code 3 documented next to 0/1/2, main.c unwinds after every screen,
   loops audited per spec step 5. Two platform_selftest checks + optional
   `LEO_QUIT_AFTER_MS` Makefile leg. Low risk, all existing codes kept.
2. **PR-26 — window hygiene bundle** (FIX-02 + FIX-03 + FIX-04 + FIX-05,
   incl. W-02 comment). One PR per the doc's own note. All low risk;
   FIX-03 touches only repeat-handling, arrows keep auto-repeat.
3. **PR-27 — FIX-12 CI hardening** (was doc PR-32, moved earlier on purpose):
   sha256-pin the SDL2 tarball, upload-artifact, `windows-smoke` job
   (MSYS2 MINGW64 + pacman mingw-w64-x86_64-SDL2; note the Makefile's
   native path uses pkg-config - on MSYS2 pass CC=x86_64-w64-mingw32-gcc),
   no-data smoke = expect exit 2 + `usage:` (NOT `--help` until PR-32),
   `asan` job on ubuntu with dummy audio + the fixture-free SKIP-safe suite
   (already proven green headless in CI). This closes §7.3's "nothing ever
   executes the .exe" without touching the handheld ticket.
4. **PR-28 — FIX-06 + FIX-07 timing core** (X-06 integer clock, X-09 OS
   sleep incl. Win high-res timer, deadline grids in level/help/menu/map,
   sub-pixel autoscroll accumulator; E-05 truly closes, T-05/T-06 land).
   Med risk (feel): gate = LEO_IS_WAV byte-identical, all PPM/golden legs
   byte-identical locally, new `LEO_TIMING_LOG=1` shows 50.0/33.3 Hz under
   load, idle CPU ≤3%.
5. **PR-29 — FIX-08 cutscene A/V** (pre-roll ≥150 ms via `ci_audio`
   walker, optional slow drift-correction with `LEO_AV_DEBUG` /
   `LEO_AV_LATENCY_MS`). Highest-risk PR; exactly-once queueing invariant
   is the design core. Hard gate: `LEO_IS_WAV` cmp identical for
   LOGO/INTRO_P/AUTORZY; verify-skip/sel/golden green.
6. **PR-30 — FIX-09 Part 1 + FIX-10 Part 1 + W-04 tail** (SetHint audio
   pin + one-shot default-driver fallback + `LEO_KEEP_PIPEWIRE`; heap
   UTF-8→wide conversion (no 4096 cap) + GetACP warning + README baseline
   statement; CMake: verify manifest via existing .rc, add SDL2.dll
   post-build copy). Low risk; verify-utf8 + wine legs stay green.
   **Part 2 of FIX-09 (session device) held for C1 sign-off.**
7. **PR-31 — FIX-11 Wayland policy** — implement `LEO_FORCE_X11=1` opt-in
   now, keep the forcing as the DEFAULT until C2 sign-off flips it. Safe
   by construction either way.
8. **PR-32 — FIX-13 robustness/CLI** (obj_advance guard + fuzz case,
   --console first, _IOLBF, unknown-flag exit 2, --help/--version,
   --then overflow warning, bare-launch usage only without data,
   plat_message_box for missing movie guarded off headless, knobs
   cleanup per C5, llrs fseek consistency). Then PR-27's smoke may switch
   to `--help`.

## Held / still-deferred (unchanged by this plan)

T-26 state machine (would structurally absorb X-01/X-05/W-03), W-03
modal-drag, T-11 aspect, gamepad, L-03 packaging (T-22), config file /
DATA_DIR (C5), MSVC compat.h (only if an MSVC build is ever requested),
FIX-10 Part 2 (pre-1903 wide paths — only if old-Windows support becomes a
goal), E-08/T-14 (frozen: pixel output must not change), the 2c4c0583
variant capture, residual-1.6% menu-overlay polish, real-Windows run on
the handheld (still the only genuinely-unverifiable leg; PR-27 narrows it).

## Standing gates (every PR in the series)

`make` + `make win` warning-free under -std=gnu11 -Wall -Wextra (gcc +
vendored clang, CI's system mingw); `make -k verify-all` green locally with
fixtures AND in CI without (SKIP design intact — new legs must be probe-
safe); verify-arch green (no new SDL includes outside the four allowed
files); every new plat_* in BOTH backends + platform_selftest; new LEO_*
documented in README; NEXT_STEPS status line per landed FIX; the deep
dive's §6 manual matrix ticked where a desktop check is possible.
