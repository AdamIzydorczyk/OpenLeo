# Improvement proposals - native ports (linux + windows, src/)

Analysis of the current builds (2026-11-14) and concrete proposals, ranked
by impact. Current measurements are from the verified cutscene-demo state:
shared `src/` codec + `src/platform_sdl/is_player.c`, exercised on INTRO_P
(640x480, 5 fps, 2388 chunks) and KARA (320x200 Mode X, 8 fps).

## 0. Measured baseline (what already works)

| item | result |
|---|---|
| codec correctness | C output byte-identical vs python ref on all 15 .IS (linux + win exe via wine) |
| decode speed | full INTRO_P decode 0.12 s (~30x python, ~2000x faster than needed) |
| pacing | wall-clock exact: 41f@5fps=8.2s, 24f@8fps=3.0s (fixed timestep from .IS header fps) |
| CPU while playing | ~1.5 % (sleep-driven, not busy) |
| win11 artifact | static single PE32+ exe 12.9 MB, no DLLs, cross-built from Linux |

## 1. Pacing & A/V sync (highest impact for "game works correctly")

- **Audio master clock.** Today the video clock is a free-running
  QPC deadline loop and ESC-audio is queued per chunk independently: over a
  5-minute cutscene the two can drift visibly (audio device clock vs host
  timer). Propose: derive each frame deadline from consumed audio
  (`SDL_GetQueuedAudioSize` delta / 8000 B/s = elapsed) and keep QPC only
  as fallback for `--no-audio`. Guarantees sync-by-construction.
- **Pause must freeze the video clock too** (currently SPACE pauses audio
  only; the deadline schedule keeps running).
- **Catch-up instead of hard resync.** The drift clamp resets `t0` after a
  >2-period stall; better: decode-forward without presenting (frame skip)
  so late frames never display, then resume normally.
- Integer nanosecond scheduling (`counter * 1e9 / freq` in uint64/`__int128`
  helper) instead of `double` - exact, no 104-day edge cases, no FP per frame.
- Engine phase (M3+): keep the two clocks separate and documented -
  logic tick 22 Hz (`TICK_HZ`, PLAN S6 calibration), presentation at
  header-fps (cutscenes) or display-rate with tick-aligned frame skipping
  (gameplay). Never interpolate (the original has no sub-tick motion).

## 2. Presentation / rendering

- **Palette LUT + direct texture write.** Present currently = malloc'd
  `is_linearize` + per-pixel 3-byte RGB24 staging + `SDL_UpdateTexture`.
  Propose precomputed 256-entry `Uint32` LUT + `SDL_PIXELFORMAT_ARGB8888` +
  `SDL_LockTexture` write-through (zero staging), persistent scratch
  buffers, Mode X uninterleave fused into the LUT pass. Est. 3-5x present
  cost reduction and no per-frame allocation - matters for handheld
  battery/thermals during long FMVs.
- Integer scaling modes (2x/3x/4x centered viewport), Alt+Enter fullscreen,
  `SDL_HINT_RENDER_SCALE_QUALITY=nearest` for the 320x200 files.
- vsync policy: keep off for 2-9 fps content (timer-driven is correct);
  for 22 Hz gameplay present tick-latest frame (no tearing concerns at
  non-matching rates, or cap at 22 with vsync on for zero-tear FMV).

## 3. Audio

- **Gapless region concatenation.** ESC PCM regions are queued one SDL call
  each; joins can click. Propose: coalesce a chunk's ESC blocks into one
  buffer (they are contiguous in the file anyway) and queue once.
- **Resample 8k -> 48k** (`SDL_AudioCVT` to F32SYS) instead of presenting
  8 kHz U8 to the device: consistent behavior across backends (WASAPI
  dislikes exotic formats), optional linear interpolation to reduce images.
- **Latency**: `want.samples=1024@8kHz = 128 ms` queue granularity; drop to
  256 (32 ms) with pre-queue of the next chunk's audio (already know the
  whole file up-front - could even queue the entire payload audio at once
  and use it as the master clock, see 1.).
- **MOD music** (`mapka.mod` string proves module files exist in the data):
  integrate mikmod/libmodplug (portable C) mixed via `SDL_AudioStream` -
  required for gameplay (M5), irrelevant for cutscene demo.
- Volume control + persisted config (INI next to exe / $XDG_CONFIG).

## 4. Portability & UX

- **Data filename casing**: the game itself uses lowercase names
  (`status.leo`, `mapka.mod`, `animacje` - confirmed in the binary's
  string table), but users' ISO extracts are often UPPERCASE. Implement
  the `FS_CASE_INSENSITIVE` knob (dir-scan fallback) in shared src.
- Data dir resolution order: argv > `LEO_DATA` env > `<exe>/leo/` >
  `LONGSOFT.CFG` drive byte (retail parity only). No CD path anywhere.
- win11: version/manifest resource (DPI-aware, `longPathAware`), icon,
  `.bat`/`.ps1` launcher next to the exe; keep static single-file build.
- linux: link against distro SDL2 (already) and provide a `.desktop` +
  optional AppImage later for portability.
- **Gamepad support** (the Ally X is a gamepad!): SDL_GameController
  mapping (d-pad/left stick = move, A=jump, Start=pause) with keyboard
  fallback; matters from M3 (gameplay) onward, cheap to add now to the
  input layer shape.
- Console output codepage on Windows (`SetConsoleOutputCP(CP_UTF8)`);
  in-game Polish text stays CP437/ISO-8859-2 until the engine text layer
  (M5) decides internally-UTF8 vs legacy bytes.

## 5. Robustness & CI

- Keep the byte-exact `--ppm` diff as the regression test for both ports:
  one script = linux `make verify` + win11 cross-build + run under wine +
  `cmp` vs python reference; run on every codec change (already scripted
  in this session, move to `tools/ci_verify.sh`).
- ASan/UBSan build target for `is_stream.c` + a quick fuzz pass (mutated
  chunk headers) - decoder is bounds-checked but prove it under fuzzing.
- `--bench` self-check: assert presented ~= elapsed x fps within +/-2 %
  (turns the pacing verification into an automated test).

## 6. Known gaps to close next (roadmap)

1. On-device win11 GUI test - handheld asleep at transfer time; procedure:
   wake device -> `win put --verify` exe + a sample .IS -> run GUI, then
   also `--ppm` under SSH for headless byte-exact proof on real hardware.
2. llrs container reader in C (tools/llrs_unpack.py port) for ANIMACJE/
   ETAPY/SOUND - prerequisite for gameplay (game reads via container).
3. STATUS.LEO: 300 B = 75 LE dwords, per-dword XOR with `0x7707xxxx`
   constants (writer FUN_0005dcc0); plan one deterministic QEMU-icount
   capture (INT21 markers + INT 1Ah BIOS-tick timestamps - which also
   yields the exact gameplay tick Hz for free, PLAN S6).
4. Engine M3-M5 (renderer blit path, input, main loop, one playable level)
   - shared plan in PLAN.md; native variants inherit all of the above.

## 7. Performance budget (Ally X / any modern machine)

Decode: ~0.2 ms/frame (measured aggregate) -> trivial. Present after LUT
rework: ~0.3 ms/frame. Audio: 8 kB/s -> negligible. Expected total CPU
during FMV <5 %, matching today's measured ~1.5 % idle-paced behavior.
The original would run this content ~1000x too fast on the same hardware;
the fixed clock (header-fps + 22 Hz tick knob) is and stays the fix.
