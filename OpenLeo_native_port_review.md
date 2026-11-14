# OpenLeo – Native Linux/Windows Port Review (AI-consumable work order)

> **Audience:** another AI coding agent that will implement fixes.
> **Repo:** https://github.com/AdamIzydorczyk/OpenLeo (snapshot commit `a428cef`, "Lew Leon (1997) native port - public snapshot").
> **Format:** each finding has a stable ID, a severity, its location, evidence, root cause, a fix (with a code sketch), and verification steps. IDs are referenced by the roadmap in §6.
> Line numbers refer to commit `a428cef`. Re-locate them by symbol name if the code has moved.

---

## 1. Project overview

| Path | Role |
|---|---|
| `original/is_stream.{c,h}` | `.IS` cutscene container and codec decoder (pure C, no SDL) |
| `original/llrs.{c,h}` | Resource-container reader (`ANIMACJE`, `ETAPY`, `SOUND`, `LEVELDES`) |
| `original/ll_pcx.c`, `ll_seq.c`, `ll_pcm.c` | PCX image, `.SEQ`/`.MOV` animation-sequence, and PCM sound parsers |
| `original/ll_menu.c` | Main menu, YES/NO dialogs, and the island **map** (`ll_menu_open_ex`, `ll_menu_map_open`). **Uses SDL directly.** |
| `original/ll_level.c` | Audio init, music singleton (`ll_audio_init`, `ll_music_pump`), level screen (`ll_level_show`), help overlay. **Uses SDL directly.** |
| `original/ll_mod.c` | ProTracker MOD player (`do_tick`, `ll_mod_mix`, `ll_mod_render`) |
| `original/ll_save.c`, `ll_config.h` | Save/status file and constants |
| `ported/main.c` | Entry point: CLI parsing, Wayland→X11 forcing, demo boot chain (LOGO→PRAWA→INTRO→menu), `run_menu` flows |
| `ported/platform_sdl/is_player.{c,h}` | SDL2 cutscene player (video and audio sync, pause, skip, `resolve_ci`) |
| `Makefile` | `make` builds the Linux binary `ll_cutscene` (pkg-config sdl2, `-DPLATFORM_LINUX=1`). `make win` builds `ll_cutscene.exe` (MinGW, static, `-mwindows`, `-Dmain=SDL_main`, SDL from `vendor/SDL2-2.32.10/x86_64-w64-mingw32`) |
| `data/` | Ships only `LOGO.IS`, `PRAWA.IS`, `INTRO_P.IS`, `ANIMACJE`, `ETAPY`, `SOUND`. **No `LEVELDES`, no chapter `.IS` files, no `AUTORZY.IS`.** |

The READMEs mention `tools/*.py` oracles and `make verify-all`. **Neither exists in the snapshot.** There is no CI and no tests directory.

Architectural note: the "platform layer" is leaky. `ll_menu.c` and `ll_level.c` under `original/` each create their own `SDL_Window`/`SDL_Renderer` and run their own event loops. Only the cutscene player lives under `ported/platform_sdl/`.

### Return-code conventions (do not break these)
- `is_player_run`: `0` = finished, `2` = user skipped (ESC), `1` = error.
- Map menu: `-2` = quit, `-3` = timer expired, `-4` = autorzy (credits), `-5` = opcje (options), `-6` = restart. Dialog: `-10` = tak (yes), `-11` = nie (no), `-12` = x (close).

### Test/CI env hooks already present (reuse them in CI)
`LEO_MENU_AUTO, LEO_DEMO, LEO_MENU_SECS, LEO_DEMO_LEVEL, LEO_LEVELSEL_AUTO, LEO_SAVE, LEO_DEMO_SKIP_FRAMES, LEO_IS_WAV, LEO_IS_PERIOD_MS, LEO_IS_FPS_DIRECT, LEO_SKIP_USER, LEO_DUMP_SKIP, LEO_NO_MUSIC, LEO_MUSIC_WAV, LEO_HD_DIR, LEO_LEVEL_SECS, LEO_LEVEL_CAM, LEO_LEVEL_PPM, LEO_LEVEL_PPM_TICK, LEO_HELP_SECS, LEO_HELP_PPM, LEO_MENU_PICK, LEO_MENU_NAV, LEO_MENU_SEL, LEO_MENU_PPM, LEO_MENU_PPM_TICK, LEO_MENU_KEY`, plus the CLI flags `--ppm`, `--no-audio`, `--skip-at N`, `--demo [dir]`.

---

## 2. Verified build and run results (Docker, Debian trixie, gcc, SDL2 2.32.4, mingw-w64)

| Check | Result |
|---|---|
| `make` (Linux) | ✅ clean, 0 warnings |
| Extra warnings (`-Wshadow -Wformat=2 -Wconversion`-lite syntax check) | ✅ no output |
| `make win XC=x86_64-w64-mingw32-gcc` with SDL2-devel-2.32.10-mingw | ✅ builds. PE32+ GUI x86-64, fully static (imports only system DLLs plus **msvcrt.dll**, not UCRT). **13.4 MB unstripped.** |
| ASan+UBSan build, `--ppm` decode of LOGO.IS | ✅ 51 frames / 56 chunks, no reports |
| ASan, `./ll_asan /w/data/logo.is --no-audio --skip-at 3` (absolute path) | ✅ but the log shows `data path resolved as: //w/data/LOGO.IS` (double slash, cosmetic) |
| ASan, `./ll_asan data/logo.is` (relative, lowercase) | ❌ `cannot open data/logo.is` (see **P-01**) |
| ASan, demo chain with `LEO_DEMO_SKIP_FRAMES=5 LEO_MENU_SECS=1` | ✅ no UBSan/ASan errors. ESC on LOGO jumped straight to the map (see **P-09**) |
| ASan, `LEO_MENU_AUTO=1` (launch chapter 0) | ❌ `ll_level: cannot open data/LEVELDES`, then back to the map (see **E-01**) |
| `LEO_MENU_KEY=r LEO_MENU_PICK=t` (restart → tak) | ✅ restart works, returns to the map (it hung only because no `LEO_MENU_SECS` was set; expected) |
| Leak check (first demo run) | ⚠️ 768 B direct leak, 1 object. 768 = 256×3, which points to a **palette buffer** (see **P-12**) |

Reproduce headless:
```sh
docker run --rm -it debian:trixie bash
apt-get update && apt-get install -y build-essential pkg-config libsdl2-dev gcc-mingw-w64-x86-64 curl
# copy repo to /w, then:
make
make CC=gcc CFLAGS="-O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -std=gnu11 $(pkg-config --cflags sdl2) -Ioriginal -Iported -DPLATFORM_LINUX=1" ...   # or edit Makefile; output ll_asan
export SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy
LEO_DEMO_SKIP_FRAMES=5 LEO_MENU_SECS=1 ./ll_asan --demo data
# Windows:
mkdir -p vendor && curl -L https://github.com/libsdl-org/SDL/releases/download/release-2.32.10/SDL2-devel-2.32.10-mingw.tar.gz | tar xz -C vendor
make win XC=x86_64-w64-mingw32-gcc
```
**Do not use `stdbuf` with ASan.** Its LD_PRELOAD triggers "ASan runtime does not come first".

---

## 3. Findings – Platform / port layer (`ported/`, SDL usage)

### P-01 [HIGH] `resolve_ci` breaks every relative path and is POSIX-only
- **Where:** `ported/platform_sdl/is_player.c:22` `resolve_ci()`, line 45 `snprintf(probe, sizeof probe, "%s/%s", out, comp);`
- **Evidence:** `./ll_cutscene data/logo.is` → `cannot open data/logo.is`. With an absolute path the log shows `//w/data/LOGO.IS`.
- **Root cause:** `out` starts as `""`, so the first probe becomes `"/data"`, which is absolute. Lookup then happens at the filesystem root. The function splits only on `/` and has no `\`, drive letter (`C:`), or UNC handling. It is also used **only** by `is_player_run`. Containers (`llrs_open`), `AUTORZY.IS`, chapter `.IS`, and save files go through a plain `fopen` with hard-coded uppercase names, so lowercase extractions fail on Linux.
- **Fix:**
  ```c
  /* new: ported/platform_sdl/fs_ci.c, exported from a header used by original/ too */
  FILE *ll_fopen_ci(const char *path, const char *mode);
  /* algorithm: if fopen(path) works -> return it.
     else split path on '/' (and '\\' on Windows); keep a prefix:
       absolute "/"     -> out = "/"
       "C:\" or "C:/"   -> out = "C:/"
       relative         -> out = ""  (probe = comp when out is empty, never "/comp")
     for each component: opendir(out[0]?out:"."), strcasecmp match, append.
     On Windows the FS is already case-insensitive -> just fopen (see P-07 for UTF-8). */
  ```
  Replace every `fopen` of game data (`llrs.c`, `is_stream.c` open, `ll_level.c`/`main.c` `.IS` paths) with `ll_fopen_ci`. Normalize the double slash.
- **Verify:** `./ll_cutscene data/logo.is`, `./ll_cutscene ./DATA/Logo.Is`, and an absolute path all play. Lowercase-rename `data/*` and run `--demo data` → reaches the map.

### P-02 [HIGH] Space-pause does not pause video
- **Where:** `is_player.c` `pump_input` (~line 232: `p->t0 += SDL_GetPerformanceCounter() - p->pause_at;`) and `is_player_run` (line 416: `Uint64 t0_ns = ...p.t0...` computed **once**, then used by `wait_until(&p, t0_ns + sched_ns)` at line 475 and by drift correction at 506-508).
- **Root cause:** pause shifts `p->t0`, but the scheduler uses the stale local `t0_ns`. Video keeps running against the original clock (audio is paused through `SDL_PauseAudioDevice`), and the drift clamp later "resyncs" by jumping. With no audio device (`--no-audio` or a missing device), pause has no effect at all.
- **Fix:** store `t0_ns` in the player struct (`p->t0_ns`) and make it the only clock. On resume, `p->t0_ns += paused_ns`. `wait_until` must loop on `pump_input` while `p->paused` (render the last frame, `SDL_Delay(10)`) and recompute the deadline from `p->t0_ns` after it returns. Remove `p->t0` or derive it.
- **Verify:** play INTRO_P.IS, press Space for 5 s, resume. Video and audio continue from the same frame and no frames are dropped (the presented counter is unchanged across the pause). Repeat with `--no-audio`.

### P-03 [HIGH] One SDL window per screen; `SDL_Init`/`SDL_Quit` per cutscene
- **Where:** `is_player_run` (`SDL_Init` line 343, `SDL_Quit` lines 536/541). `ll_menu_open_ex`, `ll_menu_map_open`, and `ll_level_show` each `SDL_CreateWindow`/`SDL_DestroyWindow`.
- **Impact:** the window flickers and closes/reopens between every scene, moves position, loses focus, and briefly appears in the taskbar on Windows. This is the root cause of the Wayland/X11 focus hacks (P-04, P-05). Re-initializing audio per scene also restarts the device (pops, and music cannot continue across screens).
- **Fix:** add an app context created once in `main`:
  ```c
  typedef struct { SDL_Window *win; SDL_Renderer *ren; SDL_Texture *tex; int tw, th;
                   SDL_AudioDeviceID adev; } ll_app;
  ll_app *ll_app_get(void);                 /* init once in main, quit at exit */
  SDL_Texture *ll_app_target(int w, int h); /* (re)create streaming tex if size changes,
                                               SDL_RenderSetLogicalSize(ren, w, h) */
  ```
  Pass or get it in `is_player_run`, the menu, the map, and the level. Remove all per-screen create/destroy calls and the `SDL_Init`/`SDL_Quit` pair in the player. Use `SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_EVENTS` once. Keep the window title updated per screen.
- **Verify:** run `--demo data`. Exactly one window exists for the whole session (`xwininfo -root -tree`, or Spy++ on Windows), and no focus changes happen between scenes.

### P-04 [HIGH UX] Focus stealing: `SDL_RaiseWindow` every tick while unfocused
- **Where:** `ll_menu.c:404, 669, 940, 1078`, `ll_level.c:1535, 1602`.
- **Impact:** on Windows and X11 the user cannot alt-tab away. The window keeps jumping back on top or flashing in the taskbar.
- **Fix:** delete the per-tick raise loops. Raise once at window creation if needed. After P-03 they are unnecessary. While unfocused, consider pausing music and the level (`SDL_WINDOWEVENT_FOCUS_LOST`).
- **Verify:** alt-tab during the menu; the window stays in the background.

### P-05 [MEDIUM] Wayland is forced to X11
- **Where:** `ported/main.c:402-414`.
- **Issue:** a probe init/quit sets `SDL_VIDEODRIVER=x11` whenever `WAYLAND_DISPLAY` is set. This breaks systems without XWayland, and it is a workaround for P-03/P-04.
- **Fix:** after P-03, remove it. If it is kept, make it opt-in (`LEO_FORCE_X11=1` or `--x11`). An explicit `SDL_VIDEODRIVER` must still win.

### P-06 [MEDIUM] Windows `-mwindows` hides all diagnostics
- **Where:** `Makefile` win target, plus every `fprintf(stderr, ...)` and `printf`.
- **Issue:** fatal errors such as "cannot open data/…" are invisible. The exe just exits.
- **Fix:** (a) on fatal paths call `SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Lew Leon", msg, NULL)`. (b) Log to `SDL_GetPrefPath("OpenLeo","LewLeon")/log.txt`, or use `SDL_LogSetOutputFunction`. (c) Add a `--console` flag on Windows: `if (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole()) { freopen("CONOUT$","w",stdout); freopen("CONOUT$","w",stderr); }`. (d) Optionally add a `make win-console` target without `-mwindows`.

### P-07 [MEDIUM] Windows: non-ASCII paths and 32-bit `long` file offsets
- **Issue:** the binary links **msvcrt** (verified via objdump), so `fopen` uses the ANSI code page. A UTF-8 path from SDL_main argv or `SDL_GetBasePath()` (e.g. `C:\Users\Łukasz\...`) fails. `ftell`/`fseek` use `long`, which is 32-bit on Win64. That is OK for the current file sizes, but it is fragile.
- **Fix:** on Windows, implement `ll_fopen_ci` (P-01) as `MultiByteToWideChar(CP_UTF8, …)` + `_wfopen`, or switch file I/O to `SDL_RWFromFile` (which is UTF-8 aware) and `SDL_RWseek`/`SDL_RWtell` (Sint64). Alternatively, build against UCRT with an app manifest that sets `activeCodePage=UTF-8`.
- **Verify:** copy the game to `C:\Gry\Łódź\` and run it. It loads.

### P-08 [MEDIUM] `--demo` without a dir uses CWD
- **Where:** `main.c:469` `const char *d = demo_data ? demo_data : ".";`
- **Issue:** double-clicking the exe or starting it from a desktop shortcut gives a CWD that is not the data dir, so the game fails silently (see P-06).
- **Fix:** fallback order: CLI arg → `$LEO_DATA` → `SDL_GetBasePath()+"data"` → `"./data"` → `"."`. Also consider making `--demo` the **default** when no args are given, so the game is double-click friendly.

### P-09 [MEDIUM] ESC on the first logo skips the whole boot chain
- **Where:** `main.c:478-480` `int rc = is_player_run(p, audio, 0, dsf); if (rc != 0 || dsf) break;`
- **Issue:** `rc==2` (user skip) breaks the loop, so ESC on LOGO skips PRAWA and INTRO and goes straight to the menu. The README says ESC advances to the next scene. Either the code or the README is wrong.
- **Fix:** `if (rc == 1) break; /* error */` and continue on `rc == 2`. Optionally a second key (e.g. `Q` or holding ESC) skips the whole chain. Keep the `dsf` (CI skip) behavior.
- **Verify:** ESC once on LOGO → PRAWA plays.

### P-10 [MEDIUM] No resize, fullscreen, HiDPI, or vsync
- **Issue:** fixed 2× window scale, not resizable, no Alt+Enter/F11, no `SDL_HINT_WINDOWS_DPI_AWARENESS` (blurry or tiny output on 150% Windows scaling), no `SDL_RENDERER_PRESENTVSYNC`. Menu mouse hit-testing assumes window = W×scale.
- **Fix:** `SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI`, `SDL_RenderSetLogicalSize(ren, 320, 200)` (or the actual content size), `SDL_RenderSetIntegerScale` optional, `SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"0")`. Toggle `SDL_WINDOW_FULLSCREEN_DESKTOP` on Alt+Enter/F11. With logical size, SDL maps mouse events to logical coordinates, so **remove the manual `/ sc` division in menu mouse handling.** Add `--scale N` and `--fullscreen` CLI options. Keep vsync optional because the cutscene clock is audio-driven.

### P-11 [LOW] Stale documentation in `is_player.h`
- The comment claims `SDL_GetTicks64` and `period = 1000/fps`. The actual clock is the audio block of 315 samples @ 22050 Hz = 1/70 s, using `SDL_GetPerformanceCounter`. `TICK_HZ` is unused by the player. Update the comment.

### P-12 [LOW] Small leaks and per-frame allocations
- 768 B direct leak observed under ASan in the demo chain. 768 = 256×3 bytes, which is a VGA palette. Find the `malloc(768)`/palette copy in the skip/menu path (`skipctx` palette or `ll_pcx` palette in the menu/map) and free it on all return paths. Re-run with `ASAN_OPTIONS=detect_leaks=1` to get the stack.
- `present()` → `is_linearize` mallocs every frame. Allocate once per player.
- `LEO_IS_WAV`: `getenv` on every audio region, and the static `FILE*` is never `fclose`d (the WAV header sizes are never patched). Close it and patch the header at the end of `is_player_run`.
- `run_menu` with `LEO_DEMO` calls `SDL_Delay` with no window shown.
- `ll_level_show` leaks on the early `!canvas` return (E-05).

### P-13 [LOW] Audio-spec handling
- `SDL_OpenAudioDevice(..., &want, &have, 0)`: with `allowed_changes=0`, SDL converts, which is fine. However, `have` is never logged or checked. Log `have.freq/format/channels`. If opening fails, continue silently with video, which the code already does.

### P-14 [LOW] Windows binary size and metadata
- 13.4 MB. Add `-s` (or `strip`) to the win LDFLAGS to get ~2-3 MB. Add a `.rc` resource (icon, VERSIONINFO) via `x86_64-w64-mingw32-windres`. Add an `app.manifest` (DPI-aware PerMonitorV2, UTF-8 code page, which also solves P-07/P-10).

---

## 4. Findings – Engine (`original/`)

### E-01 [HIGH] Every chapter fails: `LEVELDES` is not shipped
- **Where:** `ll_level.c:1032` `snprintf(p, sizeof p, "%s/LEVELDES", dir);` in `ll_level_show`.
- **Evidence:** `LEO_MENU_AUTO=1` → `ll_level: cannot open data/LEVELDES`, then silent return to the map. Chapter cutscenes (`RAN.IS`, …) and `AUTORZY.IS` (credits) are also absent and fail silently.
- **Fix:** (a) at startup, probe for the required files and log or show which are missing. (b) Grey out or mark unavailable chapters on the map and unavailable menu entries (credits). (c) Show an in-window message ("Missing data file LEVELDES – copy it from the original CD") instead of silently returning. (d) Optionally, when LEVELDES is missing, render the ETAPY background only, in a degraded "preview" mode. Document the required data files and their case-insensitive names in the README.

### E-02 [MEDIUM] Help overlay auto-closes after 3 s in production
- **Where:** `ll_level.c:834` comment, `ll_level.c:970` `getenv("LEO_HELP_SECS")` with a default of 3.0.
- **Fix:** default to infinite (close on key or click). Apply the timeout only when `LEO_HELP_SECS` is set.

### E-03 [MEDIUM] Negative frame index causes an out-of-bounds read in `obj_elem`
- **Where:** `ll_level.c:519` `if ((int)idx >= (int)o->seq.count)`.
- **Issue:** a negative `idx` (from corrupt `.MOV` data or the animation arithmetic) passes the check.
- **Fix:** `if (idx < 0 || idx >= (int)o->seq.count) return NULL;` (use signed int consistently).

### E-04 [MEDIUM] Division by zero when a `.SEQ` has `count == 0`
- **Where:** `ll_menu.c:564` and `ll_menu.c:994`, `... % aseq[a].count`. `ll_seq_parse` accepts `count == 0`. The menu also reads `els[0]` unconditionally.
- **Fix:** reject `count == 0` in `ll_seq_parse`, or guard every `%` with `count ? x % count : 0` and skip drawing.

### E-05 [LOW] `ll_level_show` correctness and quality issues
- Leak on the early `!canvas` return. Use a single `goto out` cleanup.
- Autoscroll uses an integer px per frame: `50*dt/1000` truncates to 0 when dt < 20 ms. Use a float or 16.16 accumulator.
- Help and level loops use `SDL_Delay(20)` (`ll_level.c:1015, 1649`) instead of the 70 Hz engine tick. Use a fixed-step accumulator on `SDL_GetPerformanceCounter` (or vsync) for stable speed.
- PCM-driven scroll moves in 80-px strip steps (jerky). Interpolate.

### E-06 [LOW] ESC behaves like Enter in list menus
- In `ll_menu_open_ex`, ESC returns the highlighted row. In the LEVELS list, ESC therefore **launches** a level. Return `-1` (cancel) on ESC and handle it in callers.

### E-07 [LOW] `llrs` robustness
- `malloc(size + 1)` can overflow on 32-bit with a corrupt `size`. Check `size < SIZE_MAX` and that `size <= file_len - offset`.
- Sub-block records are not range-validated against the container length.
- `llrs_block` with `cnt == 0` calls `calloc(0)`, which may return NULL and be treated as an error. Handle 0 explicitly.
- `is_stream.c`, `ll_pcx.c`, `ll_seq.c`, and `ll_pcm.c` look properly bounds-checked. A fuzz harness (§5) would confirm this.

### E-08 [LOW] Palette expansion
- 6-bit VGA → 8-bit uses `v << 2` (max 252, slightly dark). Use `(v << 2) | (v >> 4)` for a full 0..255 range. **This is optional:** it changes the pixel output and any golden PPMs.

### E-09 [MEDIUM] Music pump is tied to UI loops, so underruns happen
- **Where:** `ll_music_pump` is called only from the menu and level loops. The queue refills when below ~12000 B (≈68 ms) using 125 ms chunks.
- **Issue:** any stall underruns: dragging or resizing the window on Windows (a modal loop blocks the event pump), slow loads, or `SDL_Delay`s in other screens. Music also stops during cutscenes and between screens.
- **Fix:** move the mixing to a dedicated thread (`SDL_CreateThread`) that keeps `SDL_GetQueuedAudioSize` above ~250 ms, or use an `SDL_AudioStream` filled by that thread. The code deliberately avoids SDL audio callbacks because of PipeWire issues (`ll_audio_init` swaps pipewire→alsa). Keep that on Linux, but on Windows (WASAPI) the callback is safe. Increase the low-water mark to ≥150 ms.

---

## 5. Findings – MOD player (`original/ll_mod.c`)

All of these are verifiable against the ProTracker 2.3 / OpenMPT behavior. Write a unit harness that renders a known MOD and compares row timing.

| ID | Sev | Location | Bug | Fix |
|---|---|---|---|---|
| M-01 | HIGH | `ll_mod_mix` line 368 `tick_len = tempo*44100/6250` | Tick length is **proportional** to tempo. It is correct only at 125 (882 frames), so `F96` (150 BPM) plays slower (1058 instead of 735). | `tick_len = (44100u * 5u) / (2u * st->tempo);` (i.e. `110250/tempo`). Keep the fractional remainder in an accumulator. |
| M-02 | HIGH | `ll_mod_mix` ~386-392 | Loops ignore **loop end**. A loop is taken only when `idx >= dlen`, and `pos` resets to `loop_start` (losing the fractional overshoot). Samples whose loop ends before the sample end play their tail. | `uint32_t lend = loop_start+loop_len; if (loop_len > 2 && idx >= lend) pos -= (loop_len << 16);` (wrap, keep the fraction). Also clamp `lend <= dlen` at load (line ~128). For interpolation, `s1` at `lend-1` should read `loop_start`. |
| M-03 | MED | `do_tick` line 300 | `9xx` offset is `q << 16` (q samples). It should be q×256 samples. | `pos = (uint32_t)q << 24;` (with `9xx` memory; if beyond the end, stop the voice or wrap to the loop). |
| M-04 | HIGH | `do_tick` (B/D) | `Dxx` sets the row in the **same** pattern, then `row++` runs. `Bxx` keeps the row. | Use the pending fields `jump_order`/`break_row`, applied at the end of the row: `Dxx` → `order+1, row = x*10+y` (decimal); `Bxx` → `order = xx, row = 0`; combined `B`+`D` → `order = xx, row = dd`. |
| M-05 | MED | `do_tick` 246-262 | `1xx`/`2xx` work only with a note and suppress the note trigger. Only `3xx`/`5xy` should suppress it. `1xx` without a note does nothing. | Trigger notes for all effects except 3/5. Apply 1/2 on ticks > 0 regardless of the note. |
| M-06 | MED | `porta_spd` (line 215, 269-288) | One shared `porta_spd` holds the 3xx speed **and** is reused as the vibrato params for `6xy` and as the speed for `5xy`. `5xy`/`6xy` overwrite it with their own vol-slide param. `300`/`400` have no memory. | Separate `porta_speed` (set by 3xx when xx≠0) from `vib_speed`/`vib_depth` (set by 4xy per nibble ≠0). `5xy`/`6xy` use those memories plus `xy` as the vol-slide. |
| M-07 | MED | `do_tick` E-cmds 312-340 | E-commands are misnumbered: `E5` is implemented as retrigger (it is set-finetune), `E9` as note cut (E9 is retrigger, **EC** is cut). `ED` (note delay) is missing. `E1`/`E2`/`EA`/`EB` apply on every non-zero tick instead of **once at tick 0**. | Re-map per the PT spec: E1/E2 fine porta (tick 0), E5 finetune, E6 loop, E9 retrigger every x ticks, EA/EB fine volume (tick 0), EC cut at tick x, ED delay to tick x, EE pattern delay. |
| M-08 | LOW | arpeggio (234) | A residual `inc` offset remains after the effect ends. | Recompute `inc` from the base period every tick when no arpeggio is active. |
| M-09 | LOW | vibrato (262, 283-288) | Depth is ~4× too shallow (a 126-amplitude table /256 vs PT's 255/128). The table has 32 positions instead of 64. | Use the PT sine table (64 entries, 0..255), `delta = table*depth/128`, and `pos += speed` (mod 64). |
| M-10 | LOW | mixer (`c & 1`) | Panning is L,R,L,R. Amiga is **L,R,R,L**. Hard pan with no headroom clips. | `pan = ((c & 3) == 0 \|\| (c & 3) == 3) ? L : R`. Mix with ~75% stereo separation and a ~-6 dB master (or use an int32 accumulator then clamp once). |
| M-11 | LOW | `period_inc` line 58 | The constant 5278235 is ~0.14% sharp. | `7093789.2/2/44100*65536 = 5270959`. Use 64-bit math with rounding. |
| M-12 | LOW | `Axy`, `Cxx` | `Axy` with both nibbles set uses both (PT uses x only). `Cxx > 64` is ignored instead of clamped. | Use `x` if x≠0, else `-y`. Clamp C to 64. |

Also: the mixer adds into `int16` per channel with saturating adds. Accumulate in `int32` and clamp once to avoid order-dependent clipping.

---

## 6. Prioritized roadmap (suggested PR sequence)

1. **PR-1 "Data path robustness":** P-01, P-07 (Windows UTF-8 via `_wfopen`/`SDL_RWops`), P-08, E-01 (missing-data messaging). *Biggest user-visible win: the game starts from anywhere on both OSes.*
2. **PR-2 "Single window app context":** P-03, P-04, P-05, P-10. This is a refactor; touch the menu, map, level, and player together.
3. **PR-3 "Player correctness":** P-02, P-09, P-11, P-12 (leaks, per-frame malloc).
4. **PR-4 "Windows polish":** P-06, P-14 (strip, icon, manifest), with optional console mode.
5. **PR-5 "Engine safety":** E-03, E-04, E-07, E-05 cleanup leak, E-06 ESC semantics, E-02 help default.
6. **PR-6 "Audio":** E-09 (music thread), then M-01, M-02, M-04 (high) and M-03, M-05–M-12.
7. **PR-7 "CI & tooling":** see §7.

Each PR must keep `make` and `make win` warning-free and keep the env hooks working.

---

## 7. CI / testing proposal

GitHub Actions matrix:
- **linux-gcc:** `apt install libsdl2-dev`, `make`, `make CFLAGS+=-Werror`.
- **linux-asan:** ASan+UBSan build. Run headless with `SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy ASAN_OPTIONS=detect_leaks=1`:
  - `./ll_asan data/LOGO.IS --ppm 50 out.ppm` (decode)
  - `./ll_asan data/logo.is --no-audio --skip-at 3` (case-insensitive path, P-01)
  - `LEO_DEMO_SKIP_FRAMES=5 LEO_MENU_SECS=1 ./ll_asan --demo data` (boot chain and map)
  - `LEO_DEMO_SKIP_FRAMES=1 LEO_MENU_KEY=r LEO_MENU_PICK=t LEO_MENU_SECS=2 ./ll_asan --demo data` (restart dialog)
  - `LEO_MENU_AUTO=1 LEO_LEVEL_SECS=1 ...` (level; expect a graceful missing-LEVELDES message, E-01)
  - Compare `--ppm`/`LEO_MENU_PPM`/`LEO_LEVEL_PPM` outputs against committed golden hashes.
- **windows-cross:** Ubuntu + `gcc-mingw-w64-x86-64`, download SDL2-devel-2.32.10-mingw, `make win`. Upload the artifact.
- **windows-native (optional):** `msys2/setup-msys2` with `mingw-w64-ucrt-x86_64-SDL2`, build, and smoke-run with `SDL_VIDEODRIVER=dummy` (exercises real Windows path code, P-07).
- **macOS (optional):** `brew install sdl2`, `make` (pkg-config works).
- **Fuzzing (optional):** libFuzzer harnesses for `is_stream` decode, `llrs_open` (from memory), `ll_pcx`, `ll_seq_parse`, and `ll_mod_load`.
- **MOD unit test:** render 10 s of a MOD with `F96`, and assert the row count = `10 / (6*2.5/150)` = 100 rows ±1 (M-01).

Also restore or add the `tools/` and `verify-all` targets referenced in the README, or remove those references.

---

## 8. Feature suggestions (non-bug)
- Config file in `SDL_GetPrefPath` (scale, fullscreen, volume, key bindings, data dir).
- Gamepad support via `SDL_GameController` (D-pad/A/B → arrows/Enter/ESC).
- Volume controls (music vs SFX) and mute while unfocused.
- CMake build alongside the Makefile (MSVC support, IDE integration). `sdl2-config` fallback when pkg-config is absent.
- Optional integer scaling, CRT/scanline shader (SDL_gpu or a simple overlay).
- Release packaging: Linux AppImage/Flatpak, Windows zip with the exe + `data/` README of required files.

---

## 9. Constraints for the implementing agent
- Do not change the `.IS` codec output (pixel-exact) unless a finding explicitly says so (E-08 is optional and changes golden images).
- Preserve all `LEO_*` env hooks and return codes (§1). They are the only automated test surface.
- Keep the Linux audio path callback-free (queue API) unless it is verified on PipeWire.
- Keep `-std=gnu11` and building with both gcc and mingw without warnings (`-Wall -Wextra`).
- Data files are copyrighted game assets. Do not add new ones; handle their absence gracefully.
