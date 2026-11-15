# OpenLeo — Windows/Linux port deep dive (Nov 2026)

**Snapshot:** `4c48dd0` ("docs: PR-24 landed green")
**Scope:** `ported/` (SDL2 backend, view/audio/session, cutscene player, level/menu loops, filesystem/paths, entry point), build and CI (`Makefile`, `CMakeLists.txt`, `.github/workflows/ci.yml`, `resources/app.manifest`).
**Method:** I re-read the earlier review documents (`OpenLeo_native_port_review.md`, `OpenLeo_porting_tips.md`, `NEXT_STEPS.md`, `IMPROVEMENTS.md`), checked each "DONE" claim against the current code, and audited the platform-facing code paths. SDL2 behaviour was checked against the vendored 2.32.10 source. Nothing was built or run on Windows for this review, so the runtime claims below are inferred from reading the code. Each item has a verification step.

ID scheme: **X-** cross-platform, **W-** Windows-specific, **L-** Linux-specific, **B-** build/CI. Severity: HIGH / MED / LOW.

**Document layout:**
- **Part A (§1–§6)** describes the problems: findings, status, roadmap and manual test matrix.
- **Part B (§7 onward)** contains the **agent-ready fix specifications (FIX-xx)**. Each spec is self-contained, so an implementing agent or engineer can work from §1 (constraints) plus the one FIX section, without reading Part A.

> **Corrections after the second pass** (these supersede any wording in Part A):
> - **L-01:** `plat_prepare_video` *does* fall back to `wayland` when no X server is available, so the "pure-Wayland fails to open" claim is wrong. The remaining issue is blurry XWayland output and a stated rationale that no longer holds once FIX-02 lands. Severity is lowered to LOW-MED.
> - **B-01:** a wine leg exists (`make verify-win`, Makefile:105), but it is data-backed and only runs locally. CI still never executes the `.exe`.
> - **W-04:** CMake already embeds `resources/ll_cutscene.rc`, which includes the manifest (CMakeLists.txt:48–52). Only the SDL2.dll copy step and MSVC compatibility remain.
> - **X-04:** mapping keys by scancode is **not** recommended. Letter shortcuts (`q`, `p`, `t`/`n`/`x`) are label-based, so keysyms are correct. Only the repeat handling is a bug.
> - **X-03:** the raise sites are `ll_view_create` (every scene switch after the first), `ll_view_handle_event` (pointer ENTER), and `plat_raise` (which includes `SDL_SetWindowInputFocus`), called from `ll_menu.c:413`, `ll_menu.c:932` and `ll_level.c:1595`.

---

## 1. Constraints that every fix must respect

- **API surfaces:** the `LEO_*` env hooks and return codes are API: `is_player_run` 0/1/2, menu −2..−6, dialog −10..−12, and usage exit 2. New codes are allowed, but existing ones must keep their meaning.
- **Pixel-exact `.IS` decoding** and the `golden`/oracle legs must not change. Timing fixes must not change *what* is drawn, only *when*.
- **verify-arch:** only `platform_sdl.c`, `platform_sdl/is_player.c`, `ll_view.c/h` and `ll_audio.c/h` may include SDL. All proposals below keep SDL inside those files and add `plat_*` hooks elsewhere.
- **Warnings:** builds must stay warning-free under `-std=gnu11 -Wall -Wextra` (Linux gcc and mingw).
- **Game data:** no game data may be committed. Data-backed legs keep SKIPping in CI.

---

## 2. Re-verification of earlier "DONE" claims

| ID | Claim | Status at 4c48dd0 | Notes |
|---|---|---|---|
| P-01 | Single window / session | ✅ correct | `ll_view` owns one window; SDL init happens implicitly via `SDL_CreateWindow` (refcounted, OK). |
| P-02 | Pause doesn't skew clock | ✅ correct | `pause_shift` applied consistently in `is_player.c`. |
| P-04 | Don't steal focus | ⚠️ **regressed** | `ll_view_handle_event` calls `SDL_RaiseWindow` on `SDL_WINDOWEVENT_ENTER`, and `plat_raise` (which includes `SDL_SetWindowInputFocus`) runs on every screen init. See X-03. |
| P-05 | Wayland → X11 forcing | ❌ open | `plat_prepare_video` still forces `x11`. See L-01. |
| P-07 | 64-bit file offsets | ✅ mostly | `ll_fseek64/ll_ftell64` used in `is_stream.c` and `llrs.c`. `llrs.c:97` still uses plain `fseek(…, 0x14, …)`; this is harmless because the offset is small, but it is inconsistent. |
| P-13 | Log the real audio spec | ⚠️ **broken** | Table lookup uses `have.format & 0x800F`, so S16 (0x8010) maps to 0x8000 and the log always prints `?`. See X-07. |
| E-05 | Autoscroll speed | ⚠️ **partially done** | `cam += 50*(int)dt/1000` still truncates per frame (≈1 px/frame regardless of the intended rate). See X-08. |
| E-08 / T-14 | Palette handling | ❌ open | Unchanged from the earlier review. |
| T-03 | Config knobs wired | ❌ open | `WANT_CD_GATE`, `DATA_DIR_FROM_CFG` and `TICK_HZ` in `ll_config.h` are unused. |
| T-05 / T-06 | Fixed-step timing, precise sleep | ❌ open | See X-08 and X-09. |
| T-09 | Single audio device + resampling | ❌ open | See X-10. |
| T-11 | 4:3 aspect | ❌ open | |
| T-15 | Scancodes / key-repeat filtering | ❌ open | See X-04. |
| T-16 | Polish text/font | ❌ open | |
| T-26 | Single main loop / state machine | ❌ open | This is the root cause of X-01. |
| fs_ci relative paths | ✅ correct | Verified. |
| Music thread lifetime | ✅ correct | Pump thread is joined before its state is freed. SDL locks the audio queue internally. |
| Skip path leak | ✅ correct | The buffer is freed on skip. |

---

## 3. Findings

### X-01 — HIGH — Quit (window close / Alt+F4) is not propagated across screens

**Where:** `platform_sdl/is_player.c` (end of `is_player_run`), `main.c` (`run_demo_chain`, the chapter launch in `run_menu`), `ll_level.c` (event handling near line 1649).

**Evidence:**
- `is_player_run` returns `p.user_skip ? 2 : 0`. A window close and a normal end both return `0`, so the caller cannot tell them apart.
- `run_demo_chain` therefore moves on to the next scene and then the menu.
- In the chapter launch path the player result is ignored, so the level starts anyway.
- `ll_level.c` treats `LL_EV_QUIT` the same as ESC, which goes back to the map.

**Effect:** clicking the window X may need several clicks, one per screen, before the app actually exits. On Windows this looks like a hang during shutdown or session logoff.

**Fix:**
1. In `platform_sdl.c`, keep a sticky flag `static int g_quit;`. Set it on `SDL_QUIT` and on `SDL_WINDOWEVENT_CLOSE`, and expose it as `int plat_quit_requested(void);`. `platform_dummy.c` returns 0, or the value of `LEO_QUIT_AFTER` if set, for tests.
2. Add a new player return code `3 = quit`; keep 0/1/2 unchanged. Document it next to the existing codes.
3. Make every loop honour the flag:
   - `run_demo_chain`: `if (rc == 3 || plat_quit_requested()) return QUIT;`
   - chapter launch: check the flag before entering the level.
   - level loop: `LL_EV_QUIT` returns a distinct level result instead of the ESC path.
   - menu and map loops: return a new code (e.g. −7) mapped to exit in `main`.
4. `main` checks `plat_quit_requested()` after every top-level step.

**Verify:** in a `LEO_*` scripted run, inject `SDL_QUIT` (`SDL_PushEvent`) in the demo, in a level and in the map; in each case the process exits with code 0 within one tick. Add a dummy-backend selftest that sets the flag and asserts that `run_demo_chain` returns immediately.

---

### X-02 — HIGH (risk) — Cutscene audio is not the master clock and is under-buffered

**Where:** `platform_sdl/is_player.c` main loop (~380–470), `wait_until` (~220).

**Evidence:**
- Each chunk's PCM is queued only one chunk ahead of its deadline. The lead is the previous chunk's hold time, 28–57 ms for LOGO.
- The device is opened at 22050 Hz with `samples=0`. SDL then picks about 1024 frames, which is about 46 ms per callback period.
- The lead is therefore often shorter than one device period, so underruns are likely. Each underrun inserts silence, which permanently delays all later audio.
- Video runs on the wall clock (`t0_ns`), so A/V drift accumulates and never corrects. The "audio master clock" from IMPROVEMENTS §1 is not implemented.
- The effect is worst with WASAPI shared mode and Bluetooth on Windows, and with PipeWire quantum changes on Linux.

**Fix:**
1. **Pre-roll:** before showing frame 0, queue at least 120 ms of audio (several chunks), then keep `SDL_GetQueuedAudioSize()` at or above the target lead each iteration.
2. **Audio-mastered clock:** `played_bytes = queued_total - SDL_GetQueuedAudioSize(dev)`; `audio_ms = played_bytes * 1000 / (rate*ch*2)`. Present frame *n* when `audio_ms >= frame_deadline[n]`. Use the wall clock only when there is no audio or the device is paused, and keep `pause_shift` semantics.
3. Clamp the drift correction (for example, drop or repeat at most one frame per correction) so pixel output stays deterministic for the oracle legs. Frame *content* is unchanged; only presentation time changes.

**Verify:** add `LEO_AV_DEBUG=1` to log the minimum queued bytes per second and the A/V offset. The acceptance target is no underruns and |offset| < 40 ms through LOGO, INTRO and AUTORZY on Linux (PipeWire and ALSA) and on Windows (WASAPI, plus a Bluetooth headset if available).

---

### X-03 — MED — Focus stealing on pointer enter (P-04 regression)

**Where:** `ll_view.c` `ll_view_handle_event` (`SDL_WINDOWEVENT_ENTER` → `SDL_RaiseWindow`); `plat_raise` called from every screen init.

**Effect:**
- On Windows, the taskbar flashes or the game window jumps in front whenever the mouse crosses it.
- On X11/KWin/Mutter, focus is stolen from whatever the user is typing in.

**Fix:** remove the ENTER handler. Call `plat_raise` once, after the first window creation, and never use `SDL_SetWindowInputFocus`; `SDL_RaiseWindow` alone is enough.

**Verify:** start the game, move focus to another window, then move the mouse over the game window. Focus must stay where it was.

---

### X-04 — MED — Key repeat: F11 / Alt+Enter toggle repeatedly, and the engine receives repeats

**Where:** `ll_view.c` fullscreen toggle; `platform_sdl.c` `plat_poll`.

**Fix:**
- `if (e.key.repeat) break;` before the F11 and Alt+Enter handling.
- In `plat_poll`, drop repeats for KEYDOWN, or mark them with a flag in `ll_ev` so the menu can keep auto-repeat for arrow keys only.
- Map keys by scancode (`e.key.keysym.scancode`) for gameplay keys so AZERTY/QWERTZ layouts behave the same (T-15).

**Verify:** holding F11 for 2 s produces exactly one toggle.

---

### X-05 — MED — No re-present on expose, resize or fullscreen while the game is idle

**Where:** `platform_sdl.c` `plat_poll`.

**Effect:**
- Several states present nothing until the next frame: pause, the audio drain loop, the end-hold, and modal waits.
- Toggling fullscreen, resizing, or uncovering the window in those states leaves black or garbage contents. This is common on Windows (DWM) and on X11 without a compositor.

**Fix:**
- On `SDL_WINDOWEVENT_EXPOSED`, `SIZE_CHANGED` or `RESTORED`, call a backend-internal `present_last()` that does `SDL_RenderClear` + `SDL_RenderCopy(last texture)` + `SDL_RenderPresent`.
- The texture already holds the last frame. Also handle `SDL_RENDER_TARGETS_RESET` / `SDL_RENDER_DEVICE_RESET` (D3D9/D3D11 device loss on Windows) by recreating the texture and re-uploading the last indexed frame.

**Verify:** pause a cutscene, toggle F11 twice, and resize. The frame must stay visible.

---

### X-06 — LOW — `plat_now_ns` uses floating-point conversion

**Where:** `platform_sdl.c`.

**Fix:** use integer math:
`ticks / freq * 1e9 + (ticks % freq) * 1e9 / freq` with `uint64_t`, which avoids overflow and precision loss after long uptimes.

---

### X-07 — LOW-MED — Audio spec log always prints `?` (P-13)

**Where:** `ll_audio.c` `ll_audio_open`.

**Fix:** use `switch (have.format) { case AUDIO_S16LSB: … case AUDIO_F32LSB: … }`, or print `SDL_AUDIO_BITSIZE(f)`, `SDL_AUDIO_ISFLOAT(f)` and `SDL_AUDIO_ISSIGNED(f)`. Also log `have.samples` and `SDL_GetCurrentAudioDriver()`, which are the most useful fields when users report crackling.

---

### X-08 — MED — Level, menu and map timing drift (T-05)

**Where:**
- `ll_level.c` level loop (~1640–1700): `plat_sleep_until(now + 20ms)` runs *after* the frame's work, and autoscroll uses `cam += 50*(int)dt/1000`.
- `ll_menu.c` menu loop (~630–775) and map loop (~1040–1201): `t_next = now + 30ms`, also set after rendering.

**Effect:**
- The effective rate is `1000/(20+work)` Hz, which is below 50 and depends on the machine. Slow machines and debug builds run the game slower.
- Integer truncation in the autoscroll makes camera speed depend on frame rate (E-05 is not fully fixed).
- `TICK_HZ=70` in `ll_config.h` is still unused.

**Fix:**
- Use a fixed-step accumulator:
  ```c
  uint64_t next = plat_now_ns();
  for (;;) {
      next += STEP_NS;                       /* deadline-based, no drift */
      update(STEP_MS);                       /* simulation at fixed step */
      render();
      if (plat_now_ns() > next + 5*STEP_NS)  /* hitch: resync, don't spiral */
          next = plat_now_ns();
      plat_sleep_until(next);
  }
  ```
- For autoscroll, keep a sub-pixel accumulator: `acc += 50*dt; cam += acc/1000; acc %= 1000;`.
- Either wire up `TICK_HZ` or delete it.

**Verify:** add `LEO_TIMING_LOG` to print the measured loop Hz. Expect 50.0 ± 0.2 Hz with an artificial 10 ms load (`LEO_SLOW=10`), and autoscroll pixels/second within 1% across load levels.

---

### X-09 — LOW-MED — `plat_sleep_until` burns CPU (T-06)

**Where:** `platform_sdl.c`.

**Evidence:** the last 2 ms of every wait spin on `SDL_Delay(0)`. At a 20–30 ms tick that is about 7–10% of a core, all the time, which noticeably affects battery life on handhelds (Steam Deck, ROG Ally) and laptops.

**Fix:**
- Sleep `remaining - 1ms` with `SDL_Delay`, then yield-spin for less than 1 ms.
- On Windows, SDL already calls `timeBeginPeriod(1)` (via `SDL_TicksInit`), so a 1 ms tolerance is safe.
- Optionally, on Windows ≥ 1803 use `CreateWaitableTimerExW(..., CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, ...)` behind `#ifdef _WIN32` in `platform_sdl.c`.
- On Linux, `clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME)` is precise to well under 1 ms and needs no spin.

**Verify:** `top`/Task Manager shows ≤ 2% CPU in the menu, and the X-08 Hz log stays at 50.0.

---

### X-10 — MED — Audio device is reopened per scene with different rates (T-09)

**Where:** `ll_audio.c`, `is_player.c` (22050 Hz), `ll_level.c` music (44100 Hz).

**Effect:**
- Each scene change closes and reopens the device, which adds 50–300 ms latency on WASAPI and Bluetooth and produces audible pops.
- `plat_audio_open` returns −1 when a device is already open, and the caller then prints a stale `SDL_GetError()`, which is misleading.

**Fix:**
- Open one device for the session at 48000 Hz (or the device's preferred rate) using `SDL_AUDIO_ALLOW_FREQUENCY_CHANGE`.
- Feed each source through an `SDL_AudioStream` that converts it to the device rate. Cutscene and MOD sources only swap the stream.
- Make "already open" return 0 with the existing spec, or a distinct error code with a clear message.

**Verify:** chaining the demo into the menu and a level produces no pops (record with loopback), and the log shows exactly one "audio open".

---

### X-11 — LOW — `obj_advance` can hang on corrupt `.MOV` data

**Where:** `ll_level.c` `obj_advance` (~518–550).

**Evidence:** if every frame in every state has `ms == 0` (with `nfr > 0`), the `for (;;)` loop never subtracts anything from `o->t`, so the state cycle repeats forever. The E-03 hardening covered frame *indices* but not durations.

**Fix:** clamp `f->ms` to at least 1 at load time, or bound the loop with an iteration cap (for example `mov.nst * max_nfr + 1`).

**Verify:** add a fuzz/selftest case with a `.MOV` whose durations are all 0.

---

### X-12 — LOW — `main.c` usability issues

| Issue | Fix |
|---|---|
| `--console` attaches *after* save-load output, so early messages are lost | Parse `--console` first, before any output. |
| stdout is block-buffered when redirected or attached, so logs are lost on crash | `setvbuf(stdout, NULL, _IOLBF, 0)` early, or `_IONBF` on Windows when attached. |
| Unknown flags (`--help`, `-v`) are treated as the FILE argument | Reject args starting with `-` that are unknown, print usage, exit 2 (existing code). Add `--help` (exit 0) and `--version`. |
| `--then` silently drops entries beyond 16 | Warn on stderr, or allocate dynamically. |
| A bare launch prints usage even when data is found | Print usage only on bad args. |
| A missing `AUTORZY.IS` or chapter `.IS` fails silently in the GUI build | Show `SDL_ShowSimpleMessageBox` through a `plat_error_box()` hook (GUI subsystem only). |

---

### W-01 — MED — Windows path handling is inconsistent (ANSI vs wide)

**Where:**
- `main.c`: `stat()` in `have_sub` and in the file check; `opendir()` in `suggest_sub`.
- `ll_paths.c`: `stat()` in `exists`, `fopen()` in `ll_copy_file`.
- `fs_ci.c`: `_wfopen` fallback with a fixed `wchar_t buf[4096]`.

**Evidence:**
- Only `fopen` has a UTF-8 → UTF-16 fallback. Every other call is an ANSI CRT call.
- They work for non-ASCII paths (for example `C:\Users\Łukasz\…`) only because `app.manifest` sets `activeCodePage=UTF-8`, which needs **Windows 10 1903+**.
- On older Windows, or if the exe is launched without its embedded manifest (for example the CMake/MSVC build without windres), user directories with Polish characters break data discovery and save import.
- The manifest also declares `longPathAware`, but the 4096-wchar buffer and any `PATH_MAX` buffers contradict that.

**Fix:** pick one of:
- **A (recommended):** add `ll_fs_stat`, `ll_fs_opendir/readdir` and `ll_fs_fopen` in `fs_ci.c`. On `_WIN32` these use `MultiByteToWideChar(CP_UTF8)` + `_wstat64` / `_wfindfirst` / `_wfopen`; elsewhere they pass through. Route every call above through them, and size buffers dynamically (`MultiByteToWideChar(..., NULL, 0)` first).
- **B:** formally declare Win10 1903+ as the minimum in README, and check at startup that `GetACP() == CP_UTF8` (log a warning otherwise).

**Verify:** run from a directory named `C:\Gry\Lew Łeon ąę\` and from a path longer than 260 characters. Data discovery, save and import must all succeed. CI: add a unit test under wine (see B-01) that creates a UTF-8 directory.

---

### W-02 — LOW — Comment says Windows uses "directsound"

**Where:** `ll_audio.c`.

SDL2 2.32 defaults to **WASAPI**. Correct the comment. If DirectSound is preferred for latency reasons, set it explicitly with `SDL_SetHint(SDL_HINT_AUDIODRIVER, "directsound")` and document why.

---

### W-03 — LOW — Modal move/resize freezes the game (T-08)

Dragging the window title bar enters the Win32 modal loop, so cutscene video stops while audio keeps playing (and with X-02 the audio may underrun).

**Fix:** use `SDL_SetEventFilter` / `SDL_AddEventWatch` on `SDL_WINDOWEVENT_EXPOSED` during the move to re-present the last frame, and treat a wall-clock jump of more than 250 ms as an implicit pause (shift `pause_shift`) instead of fast-forwarding frames.

---

### W-04 — LOW — CMake Windows build differs from `make win`

- `make win` links statically, embeds the manifest and uses `-mwindows`.
- CMake links `SDL2.dll` dynamically, and it is unclear whether `app.manifest` is embedded in all generators. Without the manifest, W-01 goes away only on 1903+, and DPI awareness (PerMonitorV2) is lost, which makes the window blurry at 150% scaling.

**Fix:** in CMake, add `resources/app.rc` to the target sources for both MinGW and MSVC, and add a post-build step that copies `SDL2.dll` next to the exe. Document that MSVC also needs a `compat.h` (missing, see §4).

---

### L-01 — MED — Forced X11 on Wayland (P-05)

**Where:** `platform_sdl.c` `plat_prepare_video`.

**Effect:**
- On a Wayland session the game runs via XWayland, so it is blurry with fractional scaling and has no proper fullscreen on some compositors.
- On a pure-Wayland system without XWayland it fails to open a window at all.

**Fix:**
- Only force x11 if `SDL_VIDEODRIVER` is unset *and* the Wayland init fails. Simplest form: try the default first, and if `SDL_CreateWindow` fails, `SDL_setenv("SDL_VIDEODRIVER","x11")` and retry.
- If there was a specific reason for forcing (for example a decoration bug), keep it behind `LEO_FORCE_X11=1`.
- With libdecor present, SDL 2.32 handles decorations.

**Verify:** run on GNOME Wayland and KDE Wayland at 125% and 150% scaling, and with `XDG_SESSION_TYPE=wayland` and no `DISPLAY`.

---

### L-02 — MED — PipeWire → ALSA forcing is fragile

**Where:** `ll_audio.c`.

**Evidence:**
- The code checks only that `SDL_InitSubSystem(SDL_INIT_AUDIO)` succeeds with the alsa driver, not that a device actually opens.
- On systems without `pipewire-alsa` / `pulseaudio-alsa`, inside Flatpak, or with an exclusive device, this results in silence.
- `SDL_setenv` leaks into the environment of child processes.
- The comment mentions "SDL 2.32.74". That version number is **sdl2-compat** running over SDL3, so the "squeak" being worked around is probably a compat-layer resampling bug rather than a PipeWire bug.

**Fix:**
- Default to SDL's own choice.
- If `SDL_OpenAudioDevice` fails, *or* `LEO_AUDIO_DRIVER` is set, retry with the next driver in the list `pipewire → pulseaudio → alsa`.
- Use `SDL_SetHint(SDL_HINT_AUDIODRIVER, …)` instead of `setenv`.
- If sdl2-compat is detected (`SDL_GetVersion` patch ≥ 50) and the squeak reproduces, open the device at 48000 Hz and resample (which X-10 provides anyway). That is likely the real fix.

**Verify:** test with Fedora (PipeWire, no pipewire-alsa), Ubuntu 22.04 (PulseAudio), Arch with sdl2-compat, and inside Flatpak.

---

### L-03 — LOW — Packaging

The Linux build has no `.desktop` file, icon install, AppImage or Flatpak manifest. Add `resources/openleo.desktop` and a 256×256 PNG, plus `make install PREFIX=…` that follows XDG (`ll_paths` already uses `XDG_DATA_HOME`, so this is consistent).

---

### B-01 — MED — CI does not run the Windows binary

**Current:** the CI cross-compiles with mingw and runs Linux tests.

**Gaps:**
- Nothing ever *executes* the `.exe`.
- There is no macOS job, although the code compiles there with Homebrew SDL2.
- The SDL2 mingw tarball is downloaded without a checksum, which is a supply-chain risk.
- No artifacts are uploaded.
- No ASan/UBSan run covers the player or demo paths (only the fuzz leg).

**Fix:**
1. Add a `windows-latest` job using MSYS2 (`msys2/setup-msys2`, `mingw-w64-x86_64-SDL2`). Run `make verify-all` minus the data legs, plus a headless smoke test: `SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy openleo.exe --selftest`.
2. Alternatively, run `wine64` on Ubuntu with `SDL_VIDEODRIVER=dummy` for the selftest.
3. Pin the tarball with `sha256sum -c` against a recorded hash.
4. `actions/upload-artifact` for `openleo.exe` and the Linux binary.
5. Add an `asan` leg: build with `-fsanitize=address,undefined` and run the dummy-backend selftests and the demo chain under `SDL_VIDEODRIVER=dummy` with synthetic `.IS` fixtures from `tools/`.
6. Optionally, a `macos-latest` build-only job.

---

### B-02 — LOW — Unused configuration knobs (T-03)

`ll_config.h`: `WANT_CD_GATE`, `DATA_DIR_FROM_CFG` and `TICK_HZ` are defined but never referenced. Either wire them up (TICK_HZ via X-08; DATA_DIR via the existing but unwired `openleo.ini` helper) or delete them, so readers don't assume they do anything.

---

## 4. Still open from earlier docs (unchanged, listed for completeness)

- T-26 single main loop / state machine (would structurally fix X-01, X-05 and W-03)
- T-11 4:3 aspect correction (`SDL_RenderSetLogicalSize(320, 240)` with a 320×200 source and non-square scaling)
- T-16 Polish font/text rendering
- E-08 / T-14 palette fade/cycle accuracy
- Gamepad support (`SDL_GameController`; there is no controller code at all today)
- Config file (`openleo.ini` helper exists but is not wired)
- MSVC `compat.h` (`strcasecmp`, `ssize_t`, `__attribute__`, `dirent`)
- The `golden.*` capture half of the regression suite

---

## 5. Proposed PR roadmap

| PR | Contents | Risk | Depends on |
|---|---|---|---|
| **PR-25** | X-01 quit propagation (sticky flag, player code 3, menu −7) + selftest | Low | — |
| **PR-26** | X-03 remove raise-on-enter, X-04 key-repeat filter, X-05 re-present on expose/reset, X-07 audio log fix, W-02 comment | Low | — |
| **PR-27** | X-08 fixed-step loops + autoscroll accumulator, X-09 sleep, X-06 integer clock | Med (feel) | — |
| **PR-28** | X-02 cutscene pre-roll + audio-mastered clock, `LEO_AV_DEBUG` | Med | — |
| **PR-29** | X-10 single audio device + `SDL_AudioStream`; L-02 driver fallback | Med | PR-28 |
| **PR-30** | W-01 UTF-8 wide-path wrappers; W-04 CMake manifest/DLL | Low | — |
| **PR-31** | L-01 Wayland default with X11 fallback | Low-Med | — |
| **PR-32** | B-01 CI: Windows/MSYS2 job, checksum, artifacts, ASan leg | Low | — |
| **PR-33** | X-11 `.MOV` duration clamp, X-12 CLI polish, B-02 knobs | Low | — |
| later | T-26 state machine, W-03 modal-drag handling, T-11 aspect, gamepad, L-03 packaging | — | PR-25..27 |

**Suggested order:** PR-25 → PR-26 → PR-32 (so the remaining PRs get Windows runtime coverage) → PR-27 → PR-28 → PR-29 → the rest.

---

## 6. Manual test matrix for the PRs above

| Scenario | Windows 10 22H2 / 11 | Linux X11 | Linux Wayland | Steam Deck |
|---|---|---|---|---|
| Window X / Alt+F4 in demo, menu, map, level → exits | ☐ | ☐ | ☐ | ☐ |
| Hold F11 → single toggle; pause + toggle → frame visible | ☐ | ☐ | ☐ | ☐ |
| LOGO/INTRO A/V offset < 40 ms, no underrun log | ☐ (WASAPI + BT) | ☐ | ☐ | ☐ |
| Level loop 50.0 Hz, CPU ≤ 2 % in menu | ☐ | ☐ | ☐ | ☐ |
| Non-ASCII + >260-char data path | ☐ | ☐ | ☐ | — |
| Mouse crossing window does not steal focus | ☐ | ☐ | ☐ | — |
| 150 % scaling sharp (PerMonitorV2 / native Wayland) | ☐ | — | ☐ | — |

---
---

# Part B — Agent-ready fix specifications

## 7. How to use Part B (read first)

These instructions are for an implementing agent or engineer.

- **One FIX = one PR**, unless the spec says the fixes can be combined. Apply them in the order of §7.3. Each spec lists exact files, anchors (function names and current line numbers at `4c48dd0`), before/after code, the invariants it must not break, and acceptance checks.
- **Line numbers drift.** Always locate code by the quoted `before` snippet or the function name, not by line number alone.
- **Do not refactor beyond the spec.** Keep the existing comment style: block comments that cite review IDs (P-xx/T-xx). Mention the new FIX-id in your new comments.
- **Do not change** existing `LEO_*` hook names or meanings, existing return codes, or existing log lines that start with `main:`, `menu:`, `map:`, `is_player:`, `ll_level:` or `ll_audio:` (users and docs quote them), *except* where a spec explicitly says so.
- **SDL includes** are allowed only in `ported/platform_sdl.c`, `ported/platform_sdl/*`, `ported/ll_view.[ch]` and `ported/ll_audio.[ch]` (`make verify-arch` enforces this). Engine files reach the platform only through `ported/platform.h`.
- **Both backends:** every new `plat_*` function must be added to **both** `ported/platform_sdl.c` and `ported/platform_dummy.c`, declared in `ported/platform.h`, and get at least one check in `tools/platform_selftest.c`.

### 7.1 Build and test commands

| Purpose | Command |
|---|---|
| Linux build (all binaries) | `make` |
| Windows cross build | `make win` (needs mingw + vendored SDL2) |
| Fast structural + headless legs | `make verify-arch verify-platform verify-fuzz` |
| Full suite (data legs SKIP without `DATA=`) | `make verify-all` |
| Data-backed run (local only, never in CI) | `make verify-all DATA=/path/to/game` |
| Wine legs (local, data-backed) | `make verify-win DATA=/path/to/game` |

Every PR must keep `make` and `make win` warning-free (`-std=gnu11 -Wall -Wextra`) and `make verify-all` green.

### 7.2 Shared facts the specs rely on

- `ported/platform.h` declares `ll_event { int type, key, x, y; }` and the event types `LL_EV_NONE, LL_EV_KEYDOWN, LL_EV_KEYUP, LL_EV_QUIT, LL_EV_BUTTON_DOWN, LL_EV_MOTION`.
- In the SDL backend, `plat_poll` translates `SDL_QUIT` into `LL_EV_QUIT`. SDL generates `SDL_QUIT` when the last window is closed (window X button, Alt+F4, WM close).
- The dummy backend's events come from `plat_dummy_inject()`; its state is the `static struct {...} d;` at the top of `platform_dummy.c`.
- `is_player_run` returns 0 (finished **or** quit), 1 (error) or 2 (user ESC skip).
- `ll_menu_open*` returns −2 on QUIT or `q`, −3 when `LEO_MENU_SECS` expires, and −1 on ESC.
- `main.c` structure:
  - `run_menu()` is a `for (;;)` loop (main.c:333) that dispatches the title, map, chapter and level screens.
  - `run_demo_chain()` plays LOGO → PRAWA → INTRO_P.
  - `main()` calls `run_demo_chain()` and then `run_menu()`.

### 7.3 Order and dependencies

```
FIX-01 quit ───────────────┐
FIX-02 focus               │ independent, low risk: can be one PR ("PR-26 window hygiene")
FIX-03 key repeat          │
FIX-04 re-present          │
FIX-05 audio log           ┘
FIX-06 clock + sleep       ── before FIX-07 (the loops use plat_sleep_until)
FIX-07 fixed-step loops
FIX-08 cutscene audio lead ── before FIX-09
FIX-09 audio device reuse + driver fallback
FIX-10 Windows paths
FIX-11 Wayland policy      ── after FIX-02
FIX-12 CI
FIX-13 robustness / CLI polish
```

---

## FIX-01 — Propagate window close / Alt+F4 as an application quit (X-01, HIGH)

**Problem:** closing the window during a cutscene, level or chapter movie only ends *that screen*. The next scene, the level or the map then opens again, so the user has to close the window several times.

**Design:** add a *sticky* quit flag in the backend. Keep all existing return codes unchanged. Callers check the flag after every screen and unwind.

### Steps

1. **`ported/platform.h`**: add the declaration directly after `int plat_poll(ll_event *ev);`:
   ```c
   /* FIX-01: sticky application-quit request. Set by the backend the
    * moment it surfaces LL_EV_QUIT (window close / Alt+F4 / WM close)
    * or by plat_request_quit(); never cleared. Screens return as they
    * do today; the app glue (main.c) checks this after every screen and
    * unwinds instead of opening the next one. */
   int plat_quit_requested(void);
   void plat_request_quit(void);
   ```
2. **`ported/platform_sdl.c`**:
   - Add `int quit_req;` to the `static struct { ... } v;` block.
   - In `plat_poll`, change
     ```c
     if (e.type == SDL_QUIT) {
         o.type = LL_EV_QUIT;
     ```
     to
     ```c
     if (e.type == SDL_QUIT) {
         o.type = LL_EV_QUIT;
         v.quit_req = 1;                 /* FIX-01: sticky app quit */
     ```
   - Add these functions next to `plat_poll`:
     ```c
     int plat_quit_requested(void) { return v.quit_req; }
     void plat_request_quit(void)  { v.quit_req = 1; }
     ```
3. **`ported/platform_dummy.c`**:
   - Add `int quit_req;` to `d`.
   - In `plat_poll`, after `*ev = d.q[d.qh];`, add `if (ev->type == LL_EV_QUIT) d.quit_req = 1;`.
   - Add the same two functions using `d.quit_req`.
4. **`ported/main.c`**: add these checks.
   - `run_demo_chain`: change `if (rc == 1 || dsf)` to `if (rc == 1 || dsf || plat_quit_requested())`.
   - `run_menu`: as the **first statement inside** `for (;;) {` (main.c:333), add:
     ```c
     if (plat_quit_requested())
         break;                          /* FIX-01: window closed */
     ```
   - `run_menu`, chapter-launch block (`if (map_mode && sel >= 0) {`): **after** the `for (unsigned k ...)` loop that calls `is_player_run(p, audio, 0, 0)` and **before** `const char *lv = getenv("LEO_DEMO_LEVEL");`, add:
     ```c
     if (plat_quit_requested())
         break;                          /* FIX-01: closed during movie */
     ```
     Note: the `break` must be outside the inner `for (k)` loop so that it leaves `for (;;)`.
   - `main()`, both places where `run_demo_chain(d, audio);` is followed by `run_menu(d, audio);`: wrap the menu call as `if (!plat_quit_requested()) run_menu(d, audio);`.
   - `main()`, CLI scene chain: change `for (int k = 0; k < nthen && rc != 1; k++)` to `for (int k = 0; k < nthen && rc != 1 && !plat_quit_requested(); k++)`, and `if (menu_data)` to `if (menu_data && !plat_quit_requested())`.
5. **Audit every other input loop.** Run `grep -n "plat_poll(" ported/*.c ported/platform_sdl/*.c`. For each loop, confirm that `LL_EV_QUIT` makes the **function** return, not just leave an inner phase. Known loops:
   - `is_player.c` `pump_input` sets `p->quit`, and the main loop, drain and end-hold all stop on `p.quit`. OK, no change.
   - `ll_level.c` help page (~1034, `quit = 2`) and level loop (~1649, `esc = 1`). Both return; FIX-01 handles the caller. Change no behaviour here.
   - `ll_menu.c` menu (~658) and map (~1097) return −2. OK.
6. **Optional (needs owner sign-off; do not do this by default):** make `q` in the cutscene player (`pump_input`, `e.key == 'q'`) also call `plat_request_quit()`, because the window title says "q=quit". Today `q` only ends the current scene.

### Tests

- In `tools/platform_selftest.c`, in the "engine-level input contract" block:
  - **before** the `plat_dummy_inject(&kd);` line, add `check("no quit request before QUIT event", plat_quit_requested() == 0);`
  - **after** the "poll delivers injected events in order" check, add `check("QUIT is sticky (FIX-01)", plat_quit_requested() == 1);`
- **Optional automated SDL leg:** add the hook `LEO_QUIT_AFTER_MS=N`. In `platform_sdl.c` `plat_poll`, if the env var is set and N ms have elapsed since the first `plat_poll` call, call `SDL_PushEvent` once with `SDL_QUIT`. A Makefile leg can then run `SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy LEO_QUIT_AFTER_MS=500 timeout 10 ./ll_cutscene --demo $(DATA)` and assert exit status 0 and **no** `menu:` line in stdout.

### Acceptance

- [ ] `make verify-all` is green; the platform selftest shows the two new checks passing.
- [ ] Manual check: `./ll_cutscene --demo DATA`. Closing the window during LOGO, during the map and during a level each exits the process within ~100 ms. The log ends with `quit=1 (window close)` or the matching menu/level line, with no further scene starting.
- [ ] ESC behaviour is unchanged: ESC in LOGO still advances to PRAWA (P-09).

---

## FIX-02 — Stop stealing focus (X-03, P-04 regression, MED)

**Problem:**
- The window raises itself when the mouse pointer enters it.
- It also raises on every scene switch.
- `plat_raise` additionally forces input focus (`SDL_SetWindowInputFocus`), which on X11 is a hard grab. On Windows this flashes the taskbar or pops the window over the user's work.

**Design:** raise **once**, when the window is created. Remove all other raises. Keep `plat_raise()` in the API (callers stay), but make it a raise-once no-op.

### Steps

1. **`ported/ll_view.c` `ll_view_handle_event`**: delete the whole `SDL_WINDOWEVENT_ENTER` block. After the change the function starts like this:
   ```c
   int ll_view_handle_event(const SDL_Event *e, SDL_Window *win)
   {
       if (!win)
           return 0;
       if (e->type != SDL_KEYDOWN)
           return 0;
       ...
   ```
2. **`ported/ll_view.c` `ll_view_create`**:
   - In the `else` (scene switch) branch, delete `SDL_RaiseWindow(g.win);` and update the comment ("Raised once at creation (FIX-02)").
   - In the first-creation branch, after the renderer is successfully created, add `SDL_RaiseWindow(g.win);`. Do **not** call `SDL_SetWindowInputFocus`.
3. **`ported/platform_sdl.c` `plat_raise`**: replace the body with:
   ```c
   void plat_raise(void)
   {
       /* FIX-02: the window is raised once at creation (ll_view_create).
        * Dialog/screen re-entry happens in the SAME window, so raising
        * again can only steal focus from whatever the user switched to.
        * Kept as an API no-op for the existing call sites. */
   }
   ```
   Remove the `#pragma GCC diagnostic` block that was only there for `SDL_SetWindowInputFocus`.
4. **Update stale comments** that mention re-raising on pointer-enter: `ll_menu.c` (~649 and ~1065) and `ll_level.c` (~1644, "the backend re-raises on pointer-enter only"). Replace them with "focus: raised once at window creation (FIX-02)".
5. **`ported/platform.h`**: update the `plat_raise` doc comment to match.

### Must not change

- `plat_raise` stays declared and callable; `tools/platform_selftest.c` still calls it.

### Acceptance

- [ ] `make verify-all` is green.
- [ ] Manual check on Windows 11, KDE/X11 and GNOME: start the game, click into another application, then move the mouse over the game window. Focus stays in the other application and the taskbar does not flash.
- [ ] Opening a chapter (map → movie → level) while the game window is focused keeps keyboard input working without a click.

---

## FIX-03 — Key auto-repeat handling (X-04, MED)

**Problem:**
- Holding F11 or Alt+Enter toggles fullscreen many times.
- Holding Space in a cutscene toggles pause rapidly.
- Holding Enter or ESC can fall through several dialogs.

Arrow-key auto-repeat **is** wanted: the level camera scrolls while an arrow is held, and menu cursors move.

### Steps

1. **`ported/ll_view.c` `ll_view_handle_event`**: after the `if (k != SDLK_F11 && !(k == SDLK_RETURN && alt)) return 0;` line, insert:
   ```c
   if (e->key.repeat)
       return 1;            /* FIX-03: held key - consume, toggle once */
   ```
   It must still return 1 for repeats; otherwise a repeated Alt+Enter leaks into the engine as `LLKEY_RETURN`.
2. **`ported/platform.h`**: append a field to `ll_event`:
   ```c
   typedef struct {
       int type;
       int key;
       int x, y;
       int repeat;   /* FIX-03: 1 = OS auto-repeat of a held key (KEYDOWN) */
   } ll_event;
   ```
   Existing initialisers such as `{ LL_EV_NONE, 0, -1, -1 }` stay valid because the new field is zero-initialised. Grep for `ll_event` initialisers and confirm `-Wextra` gives no "missing initializer" warning. If it does, add an explicit `, 0` to those initialisers.
3. **`ported/platform_sdl.c` `plat_poll`**: in the `SDL_KEYDOWN` branch, set `o.repeat = e.key.repeat != 0;`.
4. **Engine consumers:** ignore repeats for *toggle and confirm* keys only.
   - `platform_sdl/is_player.c` `pump_input`: wrap the ESC, Space and `q` handling with `if (!e.repeat)`.
   - `ll_menu.c`, menu and map loops: for `LLKEY_RETURN`, `LLKEY_ESC`, `'q'` and the dialog letters `'t'`, `'n'` and `'x'`, add `if (e.repeat) continue;` (or the equivalent) before acting. Arrow keys keep acting on repeats.
   - `ll_level.c` level loop: ignore repeats for ESC, `q` and `p` (the autoscroll toggle). Keep them for LEFT/RIGHT.
   - `ll_level.c` help page: ignore repeats for ESC (otherwise holding ESC closes the help page and also ESCs the level).

### Do not

- Do not switch to scancodes. Letter shortcuts are label-based (keysym is correct on AZERTY/QWERTZ).

### Tests

- In `tools/platform_selftest.c`, inject `{ LL_EV_KEYDOWN, ' ', -1, -1, 1 }` and assert that `plat_poll` returns it with `repeat == 1`. This guards the field passthrough in the dummy backend; the dummy copies the whole struct, so no backend change is needed there.

### Acceptance

- [ ] Holding F11 for 2 s gives exactly one toggle.
- [ ] Holding Space in LOGO gives exactly one pause.
- [ ] Holding the right arrow in a level scrolls continuously.

---

## FIX-04 — Re-present the last frame on expose, resize and fullscreen (X-05, MED)

**Problem:** during pause, the audio drain, the 2 s end-hold, and any loop that only re-presents on change, nothing calls `SDL_RenderPresent`. Toggling fullscreen, resizing or uncovering the window then leaves it black or with garbage.

### Steps (all in `ported/platform_sdl.c`)

1. Add a helper above `plat_present`:
   ```c
   /* FIX-04: re-show the backend's current texture (the last presented
    * frame) - used when the window system invalidates the surface while
    * the engine is idle (pause, drain, end-hold). Pixels are unchanged. */
   static void present_last(void)
   {
       if (!v.tex || !v.view.ren)
           return;
       SDL_RenderClear(v.view.ren);
       SDL_RenderCopy(v.view.ren, v.tex, NULL, NULL);
       SDL_RenderPresent(v.view.ren);
   }
   ```
2. Replace the identical three-line tails of `plat_present` and `plat_present_argb` (`SDL_RenderClear` / `SDL_RenderCopy` / `SDL_RenderPresent`) with `present_last();`.
3. In `plat_poll`'s `while (SDL_PollEvent(&e))` loop, replace `if (ll_view_handle_event(&e, v.view.win)) continue;` with:
   ```c
   if (ll_view_handle_event(&e, v.view.win)) {
       present_last();                 /* FIX-04: after fullscreen toggle */
       continue;
   }
   if (e.type == SDL_WINDOWEVENT &&
       (e.window.event == SDL_WINDOWEVENT_EXPOSED ||
        e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
        e.window.event == SDL_WINDOWEVENT_RESTORED)) {
       present_last();                 /* FIX-04: surface invalidated */
       continue;
   }
   ```
4. **Optional (device loss on D3D, rare):** on `SDL_RENDER_DEVICE_RESET`, destroy and recreate `v.tex` with the same `w`/`h` (as in `plat_init`). The content stays black until the next engine present; that is acceptable.

### Must not change

- PPM/golden outputs. They are produced from engine buffers, not from the texture.

### Acceptance

- [ ] Pause a cutscene (Space), press F11 twice and resize the window: the paused frame stays visible.
- [ ] On Windows, minimise and restore during the end-hold: the frame is visible.

---

## FIX-05 — Correct the audio spec log and driver comment (X-07 + W-02, LOW)

### Steps (`ported/ll_audio.c`)

1. Replace the `fmt[]` table and the `f = have.format & 0x800F` lookup with:
   ```c
   static const char *fmt_name(SDL_AudioFormat f)
   {
       switch (f) {                  /* FIX-05: real AUDIO_* values */
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
   ```
   Do **not** add `*SYS` cases; they alias the LSB/MSB values and would produce duplicate-case errors.
2. Change the log to:
   ```c
   fprintf(stderr, "ll_audio: device %s %uHz %uch %u samples (driver %s)\n",
           fmt_name(have.format), (unsigned)have.freq,
           (unsigned)have.channels, (unsigned)have.samples,
           SDL_GetCurrentAudioDriver() ? SDL_GetCurrentAudioDriver() : "?");
   ```
   The `ll_audio: device` prefix is unchanged; no test greps the rest of the line (checked at `4c48dd0`).
3. In the `ll_audio_session_init` comment, replace "Windows (directsound)" with "Windows (wasapi by default in SDL 2.32)".

### Acceptance

- [ ] A run with audio prints `ll_audio: device S16LSB 22050Hz 2ch 1024 samples (driver …)` or similar, never `?`.

---

## FIX-06 — Integer clock and low-CPU precise sleep (X-06 + X-09, LOW-MED)

**Problem:**
- `plat_sleep_until` spins with `SDL_Delay(0)` for the last 2 ms of every wait, about 7–10% of a core at a 20–30 ms tick.
- `plat_now_ns` converts through `double`.

The sleep is also called from the music thread (`ll_level.c:93`), so it **must be thread-safe** (no shared static handles).

### Steps (`ported/platform_sdl.c`)

1. Replace `plat_now_ns`:
   ```c
   uint64_t plat_now_ns(void)
   {
       /* FIX-06: exact integer conversion (no double rounding); safe for
        * freq up to ~1.8e10 (Windows QPC = 1e7, Linux SDL = 1e9) */
       uint64_t f = SDL_GetPerformanceFrequency();
       uint64_t c = SDL_GetPerformanceCounter();
       return (c / f) * 1000000000ull + (c % f) * 1000000000ull / f;
   }
   ```
2. Add an OS sleep helper above `plat_sleep_until` and rewrite the function:
   ```c
   #if defined(_WIN32)
   #include <windows.h>
   #ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
   #define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
   #endif
   /* FIX-06: Win10 1803+ high-resolution waitable timer (~0.5 ms);
    * per-call handle = thread-safe (music thread sleeps too). Older
    * Windows: SDL_Delay, whose 1 ms period SDL set via timeBeginPeriod. */
   static void os_sleep_ns(uint64_t ns)
   {
       HANDLE t = CreateWaitableTimerExW(NULL, NULL,
                      CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
       if (t) {
           LARGE_INTEGER due;
           due.QuadPart = -(LONGLONG)(ns / 100ull);   /* relative, 100 ns */
           if (SetWaitableTimer(t, &due, 0, NULL, NULL, FALSE))
               WaitForSingleObject(t, INFINITE);
           CloseHandle(t);
           return;
       }
       SDL_Delay((Uint32)(ns / 1000000ull));
   }
   #else
   #include <time.h>
   static void os_sleep_ns(uint64_t ns)
   {
       struct timespec ts;
       ts.tv_sec = (time_t)(ns / 1000000000ull);
       ts.tv_nsec = (long)(ns % 1000000000ull);
       nanosleep(&ts, NULL);            /* EINTR: caller loop re-checks */
   }
   #endif

   void plat_sleep_until(uint64_t ns)
   {
       /* FIX-06: sleep in the OS until 0.3 ms before the deadline, then
        * yield-spin; never returns early (platform.h contract). */
       static const uint64_t SPIN = 300000ull;
       for (;;) {
           uint64_t now = plat_now_ns();
           if (now >= ns)
               return;
           uint64_t left = ns - now;
           if (left > SPIN)
               os_sleep_ns(left - SPIN);
           else
               SDL_Delay(0);
       }
   }
   ```
   - Including `<windows.h>` in `platform_sdl.c` is allowed; verify-arch only restricts SDL includes.
   - If mingw's `windows.h` conflicts with an existing macro (`min`/`max`), add `#define WIN32_LEAN_AND_MEAN` and `#define NOMINMAX` before the include.
3. Leave `platform_dummy.c` alone; it already uses `clock_nanosleep` with an absolute deadline.

### Acceptance

- [ ] `make` and `make win` are warning-free; `make verify-all` is green.
- [ ] CPU use of the idle title/map screen is ≤ 3% of one core (Linux `pidstat -p <pid> 5 1`; Windows Task Manager), down from about 8–10%.
- [ ] Cutscene timing is unchanged: the `LEO_IS_WAV` output is byte-identical before and after (`cmp`), and the LOGO wall-clock duration is 7.97 s ± 50 ms.

---

## FIX-07 — Deadline-based ticks for the level, menu and map, plus exact autoscroll (X-08, MED)

**Problem:**
- Each loop sleeps a fixed 20 or 30 ms **after** its work, so the period is 20 ms + work and drifts with machine speed.
- The level `dt` is truncated to whole ms each frame, losing up to 1 ms per frame, so animations run 2–5% slow.
- Autoscroll `cam += 50*(int)dt/1000` truncates to 0 or 1 px per frame, so the speed depends on frame rate.

**Rule:** change *when* frames happen, never *what* a given tick draws. Screens that use `tick` counters (the menu and map anims) then run at their nominal rate.

### Steps

1. **`ported/ll_level.c` level loop** (`while (!esc && (!t_end || plat_now_ns() < t_end))`, ~1643):
   - Before the loop, replace `uint64_t t_prev = plat_now_ns();` with:
     ```c
     uint64_t t_prev_ms = plat_now_ns() / 1000000ull;  /* FIX-07: ms clock, */
     uint64_t t_tick = plat_now_ns();                   /* no per-frame loss */
     int scroll_acc = 0;                                /* FIX-07: sub-px */
     ```
   - Replace the dt computation:
     ```c
     uint64_t now_ms = plat_now_ns() / 1000000ull;
     unsigned dt = (unsigned)(now_ms - t_prev_ms);
     t_prev_ms = now_ms;
     if (dt > 200)
         dt = 200;
     ```
   - Replace the autoscroll body:
     ```c
     if (autoscroll) {
         scroll_acc += 50 * (int)dt;          /* 50 px/s, exact over time */
         cam += scroll_acc / 1000;
         scroll_acc %= 1000;
         if (cam > cam_max)
             cam = 0;
     }
     ```
   - Replace `plat_sleep_until(plat_now_ns() + 20000000ull);  /* 20 ms tick */` with:
     ```c
     t_tick += 20000000ull;                   /* FIX-07: fixed 50 Hz grid */
     {
         uint64_t n = plat_now_ns();
         if (n > t_tick + 100000000ull)       /* >100 ms behind: resync, */
             t_tick = n;                      /* never fast-forward      */
     }
     plat_sleep_until(t_tick);
     ```
2. **`ported/ll_level.c` help page loop** (~1072, `/* 20 ms, as before */`): apply the same `t_tick` pattern. Declare `uint64_t t_tick = plat_now_ns();` before its `while`.
3. **`ported/ll_menu.c` menu loop** (outer `while (!quit)` at ~509, inner wait at ~647) **and map loop** (outer ~958, inner ~1063):
   - Before each **outer** loop, add `uint64_t t_sched = now_ms();  /* FIX-07: 30 ms grid */`.
   - Replace `uint64_t t_next = now_ms() + 30;` with:
     ```c
     t_sched += 30;
     if (now_ms() > t_sched + 100)            /* stalled: resync */
         t_sched = now_ms();
     uint64_t t_next = t_sched;
     ```
   - The inner wait loop (`left = t_next - now_ms(); ... else break;`) stays as it is.
4. **`ported/ll_config.h`**: remove the unused `TICK_HZ` knob, or add a comment that it documents the *original* PIT rate (70 Hz) and is intentionally unused. Do not wire it to these loops: the 20/30 ms periods are measured port behaviour.

### Must not change

- PPM dump hooks keyed by tick count, and frame content per tick.
- `LEO_LEVEL_SECS`, `LEO_MENU_SECS` and `LEO_HELP_SECS` deadlines. They are time-based; with FIX-07 more ticks fit into the same seconds. Check that no test asserts an exact tick count over a time window: `grep -rn "tick" tools/*.py Makefile`.

### Acceptance

- [ ] `make verify-all` (with `DATA=` locally) is green; the PPM/golden legs are byte-identical.
- [ ] With a temporary local instrument (count ticks over 10 s and print), the level runs at 50.0 ± 0.3 Hz and the menu at 33.3 ± 0.3 Hz, both on an idle machine and under `stress -c $(nproc)`. Remove the instrument before committing, or keep it behind `LEO_TIMING_LOG=1`.
- [ ] Autoscroll moves 500 ± 5 px in 10 s.

---

## FIX-08 — Cutscene audio look-ahead and drift correction (X-02, HIGH risk)

**Problem:**
- Audio for chunk *n* is queued only when chunk *n−1* is presented, so the queue lead is about one chunk hold (28–57 ms).
- SDL pulls one device period per callback (1024 frames ≈ 46 ms at 22050 Hz with `samples=0`), so the queue can run dry. SDL then plays silence for the gap, and audio permanently lags video.
- Video runs on the wall clock and never re-syncs to audio.

**Design:**
- **Step A (required):** decouple *queuing* audio from *presenting* video. Keep ≥ 150 ms queued ahead.
- **Step B (recommended):** a slow drift correction that nudges the video anchor toward the audio position.
- Both are disabled in the legacy period modes (`p.period != 0`), which must keep their current behaviour.

### Step A — look-ahead queue (`ported/platform_sdl/is_player.c`)

1. Split the audio callback in two:
   - `count_region(void *u, const uint8_t *pcm, unsigned len)`: only does `((player *)u)->last_region += len;`. It sets the hold time of the chunk being presented.
   - `enqueue_audio` (existing): **remove** the `p->last_region += len;` line. It stays responsible for padding, `ascratch`, the `LEO_IS_WAV` dump and the `p->live` gate.
2. Add to `player`:
   ```c
   unsigned  ci_audio;    /* FIX-08: next chunk whose audio is queued */
   uint64_t  q_total;     /* FIX-08: bytes ever queued (live)         */
   ```
3. Add a constant and a helper:
   ```c
   #define AUDIO_LEAD_NS 150000000ull   /* FIX-08: >= 3 device periods */

   static void queue_ahead(player *p, unsigned ci)
   {
       /* never queue a chunk before it is known to be live (start_frame
        * gating needs the decoded frame count), never in legacy period
        * modes, never past the end */
       if (!p->audio || p->period || !p->live)
           return;
       if (p->ci_audio < ci)
           p->ci_audio = ci;                /* first live chunk */
       while (p->ci_audio < p->is.nchunks) {
           uint64_t queued_ns = (uint64_t)plat_audio_queued_bytes()
                                * 1000000000ull / p->bps;
           if (p->ci_audio > ci && queued_ns >= AUDIO_LEAD_NS)
               break;
           size_t before = p->alen;
           is_walk_audio_range(&p->is, p->ci_audio, 1, enqueue_audio, p);
           p->q_total += p->alen - before;
           flush_audio(p);
           p->ci_audio++;
       }
   }
   ```
   `p->alen` is the coalescing length; check its type in `player` and adjust the `before` type to match.
4. In the main loop of `is_player_run`, replace
   ```c
   p.last_region = 0;
   is_walk_audio_range(&p.is, ci, 1, enqueue_audio, &p);
   flush_audio(&p); /* one queue call per chunk (gapless) */
   ```
   with
   ```c
   p.last_region = 0;
   is_walk_audio_range(&p.is, ci, 1, count_region, &p);  /* hold time */
   if (p.period || !p.audio) {
       /* legacy/no-audio path: queue exactly this chunk, as before */
       is_walk_audio_range(&p.is, ci, 1, enqueue_audio, &p);
       flush_audio(&p);
   } else {
       queue_ahead(&p, ci);                                /* FIX-08 */
   }
   ```
   Also call `queue_ahead(&p, ci)` inside `wait_until`'s loop, right after `pump_input(p);`, so the lead is topped up while waiting. Skip it while paused (`if (!p->pause_at)`). `wait_until` therefore needs the current `ci`: add a `unsigned ci` parameter.
5. **Exactly-once invariant:** every live chunk's audio must be queued **once**, in order. The `LEO_IS_WAV` dump must therefore be byte-identical to before.
   - In the legacy branch (`p.period || !p.audio`), `queue_ahead` never runs.
   - In the look-ahead branch, chunk audio is only queued via `ci_audio`.
   - When `p.live` first becomes 1 at chunk `ci`, `ci_audio` starts at `ci`, so pre-start chunks are never queued. This matches today's `p->live` gate.
6. **Skip path:** the existing `plat_audio_clear(); p.alen = 0;` is still correct. Additionally set `p.ci_audio = p.is.nchunks;` so nothing more is queued.

### Step B — drift correction (same file; recommended)

After `sched_ns += hold;` in the `if (p.live)` block, add:
```c
if (p.audio && !p.period && !p.pause_at && p.q_total) {
    /* FIX-08: audio position = bytes the device has consumed. A
     * device underrun stalls this count, so video waits for audio
     * instead of drifting ahead. Correct half the error per chunk,
     * at most 10 ms, so the motion stays smooth. */
    uint64_t played = p.q_total - (uint64_t)plat_audio_queued_bytes();
    int64_t a_ns = (int64_t)(played * 1000000000ull / p.bps);
    int64_t v_ns = (int64_t)(now_ns() - p.t0_ns - p.pause_shift);
    int64_t err = v_ns - a_ns - AV_LATENCY_NS;  /* >0: video ahead */
    if (err > 20000000 || err < -20000000) {
        int64_t c = err / 2;
        if (c > 10000000) c = 10000000;
        if (c < -10000000) c = -10000000;
        p.t0_ns += (uint64_t)c;                 /* shift the schedule */
    }
}
```
- Define `AV_LATENCY_NS` as `46000000` (one default device period at 22050 Hz). Make it overridable with `LEO_AV_LATENCY_MS` for tuning, since this is a new additive hook.
- `p.t0_ns` is `uint64_t`, so the signed add relies on two's-complement wraparound. Write it explicitly as `p.t0_ns = (uint64_t)((int64_t)p.t0_ns + c);`.

### Diagnostics (new additive hook)

With `LEO_AV_DEBUG=1`, print once per second:
`is_player: av min_queued=%ums err=%+dms`
where `min_queued` is the minimum of `plat_audio_queued_bytes()*1000/bps` seen during the second.

### Acceptance

- [ ] `LEO_IS_WAV` output for LOGO, INTRO_P and AUTORZY is byte-identical before and after (`cmp`).
- [ ] The existing `verify-skip`, `verify-sel` and `verify-golden` legs are green (PPM unchanged).
- [ ] With `LEO_AV_DEBUG=1` on Linux (PipeWire and ALSA) and Windows (WASAPI): `min_queued` stays ≥ 60 ms after the first second, and `|err|` ≤ 40 ms through the whole of INTRO_P.
- [ ] Pause and resume keep sync (P-02), and ESC still skips instantly with no stale audio.

---

## FIX-09 — Keep one audio device open and fall back safely between drivers (X-10 + L-02, MED)

### Part 1 — make the ALSA pin safe (small; do this first)

In `ported/ll_audio.c`:

1. Replace `SDL_setenv("SDL_AUDIODRIVER", ...)` with `SDL_SetHint(SDL_HINT_AUDIODRIVER, ...)`.
   - The hint does not leak into child processes.
   - A user's `SDL_AUDIODRIVER` env var still wins because SDL reads the environment before non-override hints.
   - Before relying on this, check in the SDL2 2.32 source (`src/audio/SDL_audio.c`, `SDL_AudioInit`) that the hint is consulted, and note the result in the commit message.
2. Add a `static int s_forced_alsa;`, set to 1 when the pin succeeds.
3. Make the pin skippable with `LEO_KEEP_PIPEWIRE=1` (new additive hook).
4. In `ll_audio_open`, when `SDL_OpenAudioDevice` fails **and** `s_forced_alsa` is set, fall back once:
   ```c
   if (!dev && s_forced_alsa) {
       /* FIX-09: alsa initialised but cannot open a device (no
        * pipewire-alsa/pulse-alsa plugin, Flatpak...): go back to
        * SDL's own choice once instead of running silent. */
       fprintf(stderr, "ll_audio: alsa open failed (%s), "
                       "retrying with default driver\n", SDL_GetError());
       s_forced_alsa = 0;
       SDL_QuitSubSystem(SDL_INIT_AUDIO);
       SDL_SetHint(SDL_HINT_AUDIODRIVER, NULL);
       if (SDL_InitSubSystem(SDL_INIT_AUDIO) == 0)
           dev = SDL_OpenAudioDevice(NULL, 0, &w, &have, 0);
   }
   ```
   This is safe because the one-device-at-a-time contract means no other device is open at this point.
5. Correct the comment: "SDL 2.32.74" is **sdl2-compat** running over SDL3. Note that the squeak is most likely in the compat layer's resampler, and that Part 2 (opening at the device's native rate) is expected to remove the need for the pin. Re-test before deleting it.

### Part 2 — one device for the session (larger; separate PR)

**Goal:** no close/reopen of the device per scene (WASAPI/Bluetooth reopen costs 50–300 ms and pops), and no 22050 Hz device.

**Invariants:**
- The `plat_audio_*` API and its semantics stay exactly as they are; the dummy-backend selftest is unchanged and must stay green:
  - open returns −1 while a *logical* stream is open;
  - queue, queued_bytes, pause and clear behave as today;
  - **queued_bytes is counted in the caller's source format**.

**Design, in `ported/platform_sdl.c` (SDL calls stay in the backend):**
- Add `SDL_AudioStream *as; int src_bps, dev_bps; SDL_AudioSpec dev_have; int logical_open;` to `v`.
- **First `plat_audio_open`:** open the device once with `want.freq = 48000`, `AUDIO_S16SYS`, 2 channels, and `allowed_changes = SDL_AUDIO_ALLOW_FREQUENCY_CHANGE`. Keep `have`.
- **Every `plat_audio_open(rate, ch)`:** if `logical_open`, return −1, and call `SDL_SetError("plat_audio_open: stream already open")` first so `plat_last_error` is not stale. Otherwise create `v.as = SDL_NewAudioStream(AUDIO_S16SYS, ch, rate, have.format, have.channels, have.freq)`, set `src_bps = rate*ch*2` and `dev_bps = have.freq*have.channels*SDL_AUDIO_BITSIZE(have.format)/8`, then unpause the device.
- **`plat_audio_queue`:** `SDL_AudioStreamPut(v.as, bytes, len)`, then move everything available (`SDL_AudioStreamAvailable` → `SDL_AudioStreamGet` into a scratch buffer) into `SDL_QueueAudio`.
- **`plat_audio_queued_bytes`:** return `SDL_GetQueuedAudioSize(dev) * src_bps / dev_bps + SDL_AudioStreamAvailable(v.as) * src_bps / dev_bps`, computed in `uint64_t`.
- **`plat_audio_clear`:** `SDL_ClearQueuedAudio` + `SDL_AudioStreamClear`.
- **`plat_audio_pause`:** unchanged (pauses the device).
- **`plat_audio_close`:** free the stream, `SDL_ClearQueuedAudio`, `SDL_PauseAudioDevice(dev, 1)`, set `logical_open = 0`. **Do not close the device.**
- **Session end** (where `ll_audio_session_close` is called today): close the device first.
- FIX-08's `p.bps` remains the source byte rate, so its arithmetic is unchanged.

**Acceptance:**
- [ ] `make verify-all` is green, including `verify-platform` and `verify-music`.
- [ ] The log shows exactly one `ll_audio: device …` line per process run (demo → menu → level).
- [ ] A loopback recording on Windows (WASAPI) shows no click at scene changes.
- [ ] `LEO_IS_WAV` is unchanged (it dumps the source bytes).

---

## FIX-10 — Windows UTF-8 and long paths (W-01, MED)

### Part 1 — required, small

1. **`ported/fs_ci.c` `wopen_utf8`**: replace the fixed `wchar_t pbuf[4096]` with a heap conversion, so the `longPathAware` manifest is honoured:
   ```c
   static wchar_t *utf8_to_wide(const char *s)
   {
       int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s, -1,
                                   NULL, 0);
       if (n <= 0)
           return NULL;
       wchar_t *w = (wchar_t *)malloc((size_t)n * sizeof *w);
       if (w && MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s, -1,
                                    w, n) <= 0) {
           free(w);
           w = NULL;
       }
       return w;
   }
   static FILE *wopen_utf8(const char *path, const char *mode)
   {
       wchar_t *pw = utf8_to_wide(path), *mw = utf8_to_wide(mode);
       FILE *f = (pw && mw) ? _wfopen(pw, mw) : NULL;
       free(pw);
       free(mw);
       return f;
   }
   ```
2. **`ported/main.c`**: in `main()`, right after the `--console` block (PLATFORM_WIN only), warn when the process code page is not UTF-8. That happens before Windows 10 1903, or when the exe lacks its manifest.
   ```c
   #if defined(PLATFORM_WIN) && PLATFORM_WIN
       if (GetACP() != CP_UTF8)
           fprintf(stderr, "main: ANSI code page %u is not UTF-8 "
                   "(Windows < 10 1903 or missing manifest): data paths "
                   "with non-ASCII characters may not be found\n",
                   (unsigned)GetACP());
   #endif
   ```
3. **`README.md`**: state the supported baseline: "Windows 10 1903+ (UTF-8 code page via manifest); older Windows works for ASCII-only paths."

### Part 2 — optional, full wide-char coverage (only if pre-1903 support is a goal)

Add these wrappers in `fs_ci.[ch]`:
- `int ll_path_exists(const char *utf8, int *is_dir)` (`_wstat64` on Windows, `stat` elsewhere);
- `ll_dir *ll_dir_open(const char *utf8)`, `const char *ll_dir_next(ll_dir *)` returning UTF-8 names (via `_wfindfirst64`/`_wfindnext64` + `WideCharToMultiByte(CP_UTF8)`), and `void ll_dir_close(ll_dir *)`.

Then route these call sites through them:
- `main.c:163` (`have_sub` stat), `main.c:170` (`suggest_sub` opendir) and `main.c:746` (file check);
- `ll_paths.c:73` (`exists`) and the `fopen` in `ll_copy_file` (→ `ll_fopen_ci` or `wopen_utf8`);
- `fs_ci.c:79/107/153/170`.

### Acceptance

- [ ] `make win` is warning-free; `make verify-win DATA=…` (wine, local) is green; `verify-utf8` is green.
- [ ] Manual check on Windows 11: data in `C:\Gry\Lew Łeon ąę\` starts via double-click; the save import works from that directory.
- [ ] Manual check: with `LongPathsEnabled=1`, data under a path longer than 260 characters opens (Part 1 covers `fopen`; `stat` needs Part 2 or the 1903+ ACP).

---

## FIX-11 — Wayland policy (L-01, LOW-MED; after FIX-02)

**Context:**
- `plat_prepare_video` forces `x11` whenever `WAYLAND_DISPLAY` is set, and falls back to `wayland` when no X server is available.
- Its stated reason ("SDL wayland windows can never get keyboard focus") was about *programmatic* raising, which FIX-02 removes. A newly mapped Wayland window receives keyboard focus normally.
- Classic SDL2 already prefers X11 by default when `DISPLAY` is set; verify the bootstrap order in `src/video/SDL_video.c`. The forcing therefore mostly matters under **sdl2-compat**, where SDL3 prefers Wayland, and there XWayland costs sharpness under fractional scaling.

### Steps (`ported/platform_sdl.c` `plat_prepare_video`)

1. Gate the forcing behind an opt-in: run the existing X11 block only if `SDL_getenv("LEO_FORCE_X11")` is set to `"1"`. Keep its log lines byte-identical.
2. Otherwise do nothing and let SDL choose; `SDL_VIDEODRIVER` from the environment still wins.
3. Update the comment and the `main.c` comment above `plat_prepare_video()` ("P-04 focus policy …").
4. Document `LEO_FORCE_X11=1` in README next to the other `LEO_*` hooks.

### Acceptance

- [ ] On GNOME Wayland (sdl2-compat) at 150% scaling, the game opens natively on Wayland (`SDL_GetCurrentVideoDriver()` reports `wayland`; add it to the existing startup log if there is one), is sharp, and keyboard input works without clicking.
- [ ] `LEO_FORCE_X11=1` restores today's behaviour and log line.
- [ ] On KDE/X11 the behaviour is unchanged.

---

## FIX-12 — CI: run the Windows binary, pin downloads, upload artifacts, add ASan (B-01, MED)

All changes go in `.github/workflows/ci.yml`. Keep the existing `ubuntu-latest` job unchanged.

1. **Pin the SDL2 mingw tarball.** After the `curl -fsSLo /tmp/sdl.tar.gz` step, add
   ```yaml
   echo "<sha256>  /tmp/sdl.tar.gz" | sha256sum -c -
   ```
   Compute `<sha256>` once from the official `SDL2-devel-2.32.10-mingw.tar.gz` release asset and record it in the YAML with a comment naming the URL.
2. **Upload artifacts.** After the build, add `actions/upload-artifact@v4` with `name: openleo-${{ github.sha }}`, the Linux binaries (`ll_cutscene`, `ll_level`) and the `.exe` files, and `if-no-files-found: error`.
3. **New job `windows-smoke` (`runs-on: windows-latest`):**
   - `msys2/setup-msys2@v2` with `msystem: MINGW64` and `install: make mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 python`.
   - Build the headless selftests natively (`make platform_selftest llrs_selftest pcx_selftest seq_selftest mod_selftest` or the matching `.exe` targets; check the Makefile for the right names under MSYS2).
   - Run `make verify-platform verify-fuzz verify-arch`.
   - Smoke-run the real exe headless with **no arguments** and no game data: `SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy ./ll_cutscene.exe`. With no data found, `main` prints the usage text and exits 2. Assert `exit code == 2` and that stderr contains `usage:`. This proves the exe starts, links SDL and runs `SDL_main`.
     - Do **not** pass `--help` before FIX-13 lands. Today it is treated as FILE, and `fatal_msg` would open a message box that blocks the headless runner.
     - After FIX-13, switch the smoke test to `--help` with an expected exit code of 0.
4. **New job `asan` (`ubuntu-latest`):**
   - `make clean && make CFLAGS="-std=gnu11 -Wall -Wextra -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer" LDFLAGS="-fsanitize=address,undefined"`. Check how the Makefile composes CFLAGS/LIBS and pass them the same way the fuzz leg does.
   - Run `make verify-platform verify-fuzz verify-llrs verify-pcx verify-seq verify-pcm verify-mod` with `ASAN_OPTIONS=detect_leaks=1:abort_on_error=1` and `UBSAN_OPTIONS=halt_on_error=1`.
5. **Optional `macos-latest` build-only job:** `brew install sdl2 pkg-config && make`.

### Acceptance

- [ ] All jobs are green on a PR. Deliberately breaking the checksum fails the Linux job. The artifacts are downloadable.

---

## FIX-13 — Robustness and CLI polish (X-11, X-12, B-02; LOW)

These are independent small changes and can be one PR.

1. **`.MOV` zero-duration hang** (`ported/ll_level.c` `obj_advance`, ~518): in the `for (;;)` loop, before `if (o->t < f->ms) break;`, add a loop budget. A whole cycle of zero-length frames would otherwise spin forever.
   ```c
   /* FIX-13: corrupt .MOV with all-zero frame durations would cycle
    * states forever; one full pass without consuming time = stop */
   if (++guard > 4096)
       break;
   ```
   Declare `unsigned guard = 0;` before the loop. Add a fuzz/selftest case with a `.MOV` whose frames all have `ms == 0` and assert that it terminates.
2. **`--console` first** (`ported/main.c`): move the `if (console_wanted) { … AttachConsole … }` block to directly after the argument-parsing loop and **before** the T-17 save-resolution block, so the save-load messages are visible. On Windows, after the `freopen` calls, add `setvbuf(stdout, NULL, _IONBF, 0);`.
3. **Line-buffered stdout everywhere:** at the top of `main()`, add `setvbuf(stdout, NULL, _IOLBF, BUFSIZ);`. Logs then survive a crash and interleave correctly with stderr in CI.
4. **Unknown flags:** in the arg loop's final `else`, if `argv[i][0] == '-'` and `argv[i]` is not `-`, print `main: unknown option %s` plus the usage text to stderr and `return 2;` (usage exit code 2 is existing API). Add `--help` / `-h`, which prints the usage text to **stdout** and returns 0, and `--version`, which prints the version and returns 0 (use an existing version string if there is one; otherwise add `#define LEO_VERSION "dev"` in `ll_config.h`).
5. **`--then` overflow:** when `nthen == MAX_SCENES`, print `main: --then limit (%d) reached, ignoring %s` to stderr instead of dropping the entry silently.
6. **Bare launch with data found:** in the `!file` branch, print the usage text only when `d == NULL`; with data, keep only the existing `[leo] no FILE.IS given …` line.
7. **Missing movie in the GUI build:** in `run_menu`, when `is_player_run` returns 1 for `AUTORZY.IS` or a chapter `.IS`, call `plat_message_box("Lew Leon", "<file> not found in the game data")` (existing API in platform.h:168). Only do this when not in a CI hook run (`!auto_i && !secs_hook`), so headless legs never block.
8. **`ll_config.h` knobs:** delete `WANT_CD_GATE` and `DATA_DIR_FROM_CFG` if `grep -rn` shows no use outside `ll_config.h`, or mark them `/* reserved, unused */`. For `TICK_HZ`, see FIX-07 step 4.
9. **`llrs.c:97`:** change `fseek(c->f, 0x14, SEEK_SET)` to `ll_fseek64(c->f, 0x14, SEEK_SET)` for consistency (P-07).

### Acceptance

- [ ] `make verify-all` is green.
- [ ] `./ll_cutscene --bogus` exits 2 with `unknown option`; `--help` exits 0.
- [ ] On Windows, `ll_cutscene.exe --console` launched from cmd shows the `ll_save` lines.

---

## 8. Definition of done (all FIX specs)

- [ ] The code changes match the spec, with no unrelated refactors. Comments cite the FIX-id and the original review ID.
- [ ] `make`, `make win` and `make verify-all` are green and warning-free; `verify-arch` passes.
- [ ] New `plat_*` functions exist in both backends and are covered in `tools/platform_selftest.c`.
- [ ] New `LEO_*` hooks are documented in README (`LEO_QUIT_AFTER_MS`, `LEO_AV_DEBUG`, `LEO_AV_LATENCY_MS`, `LEO_KEEP_PIPEWIRE`, `LEO_FORCE_X11`, `LEO_TIMING_LOG`, whichever were added).
- [ ] `NEXT_STEPS.md` gets a status line per landed FIX (same format as PR-14..PR-24).
- [ ] The manual checks listed under each spec are recorded in the PR description (OS, version, result).
