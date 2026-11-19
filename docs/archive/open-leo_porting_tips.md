# open-leo – Porting Tips for Modern Windows & Linux (AI-consumable guide)

> **Audience:** an AI or a developer continuing the native port of *Lew Leon* (1997, LongSoft, DOS) in https://github.com/AdamIzydorczyk/open-leo (commit `a428cef`).
> **Companion document:** `open-leo_native_port_review.md` covers concrete bugs (IDs P-xx, E-xx, M-xx). This guide covers **how to port well**: patterns, pitfalls, and design advice derived from the code base. References to the review use its IDs.
> Each tip has an **evidence** section (what the code does today) and a **recommendation** section (what to do), and some include a code sketch.

---

## 0. Baseline facts about the code base

| Aspect | State in `a428cef` |
|---|---|
| Size | ~6.6 k lines of C (`original/` engine + `ported/` SDL glue) |
| Language | C11 (`-std=gnu11`), builds warning-free with gcc and mingw-w64 |
| Origin | Reverse-engineered from the DOS binary. 65 references to `FUN_xxxxx`/`DAT_`/VA addresses (Ghidra names) are kept in comments, e.g. `ll_config.h:48` PIT reprogram `FUN_00018870`. |
| Platform layer | **Leaky.** Only the cutscene player is in `ported/platform_sdl/`. `original/ll_menu.c` and `original/ll_level.c` call SDL directly (39 SDL input references, own windows, own loops). |
| Binary formats | Parsed via explicit byte readers (`rd16`/`rd32`/`rd_le32` in `is_stream.c:14`, `llrs.c:9`, `ll_pcx.c:11`, `ll_save.c:15`). ✅ endian- and alignment-safe, no `#pragma pack`, no struct `fread`. |
| Config knobs | `ll_config.h` defines `PLATFORM_*`, `WANT_CD_GATE`, `DATA_DIR_FROM_CFG`, `FS_CASE_INSENSITIVE`, `TICK_HZ`. **None of the last four is referenced anywhere outside `ll_config.h`** (verified with grep). They are documentation, not behavior. |
| Test hooks | 39 `getenv("LEO_*")` call sites. These are the de-facto test API. |

---

## 1. Architecture tips

### T-01 Finish the platform abstraction before adding features
**Evidence:** `ll_menu.c:395, 932` and `ll_level.c:1530` each `SDL_CreateWindow`, `is_player.c:343` does `SDL_Init`/`SDL_Quit` per scene (review P-03). The engine therefore can't be tested without SDL, and every new OS quirk (Wayland focus, Windows modal drag, DPI) must be fixed in 4 places.

**Recommendation:** introduce a thin platform interface and make `original/` SDL-free:
```c
/* ported/platform.h */
typedef struct { int key; int x, y; int type; } ll_event;     /* engine-level events */
int   plat_init(const char *title);
void  plat_present(const uint8_t *idx8, const uint8_t pal[768], int w, int h); /* 8-bit indexed in */
int   plat_poll(ll_event *ev);                /* returns 0 when queue empty */
uint64_t plat_now_ns(void);
void  plat_sleep_until(uint64_t ns);
void  plat_audio_queue(const int16_t *s, size_t frames, int rate, int ch);
size_t plat_audio_queued_ms(void);
FILE *plat_fopen(const char *path, const char *mode);  /* UTF-8 + case-insensitive */
```
Then the engine screens become pure state machines: `update(dt, events)` → `render(indexed fb)`. Benefits include headless unit tests (dummy platform), an easy future backend (SDL3, libretro core, Emscripten), and one place to fix OS quirks.

### T-02 Keep the 8-bit indexed framebuffer as the engine's output
**Evidence:** the DOS original drew into VGA memory (mode 13h 320×200 and a 640×400 menu mode, `ll_menu.c:803-810`). The port converts to ARGB per screen in several places (`ll_level.c:1559` packs the palette into `Uint32`).

**Recommendation:** have every screen render into `uint8_t fb[w*h] + uint8_t pal[768]`, and convert to ARGB **once** in `plat_present` with a 256-entry LUT (rebuilt only when the palette changes). This gives:
- pixel-exact golden tests (hash the indexed fb + palette, not GPU output),
- free palette effects (fades, cycling, the menu tint ramps `ll_menu.c` already emulates),
- a later GPU-shader path (upload the index texture + palette texture and do the lookup in a shader).

### T-03 Turn the unused `ll_config.h` knobs into real behavior, or delete them
**Evidence:** `FS_CASE_INSENSITIVE`, `TICK_HZ`, `WANT_CD_GATE`, `DATA_DIR_FROM_CFG` are unused (§0). The comment at `ll_config.h:11` claims "target differences live only in the root Makefile + the PLATFORM_* knobs", but the real differences are runtime `#ifdef`-less hacks (`main.c:403` Wayland).

**Recommendation:** wire `TICK_HZ` into the fixed-step loop (T-05) and `FS_CASE_INSENSITIVE` into `plat_fopen`. Drop `WANT_CD_GATE`/`DATA_DIR_FROM_CFG` for native targets, or document them as DOS-rebuild-only. Unused knobs mislead future contributors and AI agents.

### T-04 Remove global/static engine state
**Evidence:** the music singleton in `ll_level.c:78-85` (`s_mus_snd`, `s_mus`, `s_mst`, `s_mus_snd_open`, `s_mus_name`). `LEO_IS_WAV` uses a static `FILE*`.

**Recommendation:** gather it into an `ll_game` struct passed explicitly. This is required for an audio thread (T-09: shared state needs a mutex or a lock-free queue), for clean restart (`-6` restart flow), and for save/load.

---

## 2. Timing tips (most important for "feels like the original")

### T-05 Emulate the 70 Hz PIT with a fixed-step accumulator
**Evidence:** `ll_config.h:48-58` documents that the original reprograms PIT channel 0 to **70.00 Hz** for the whole session, and that the `.IS` movie clock is the same (22050/315). But the native loops use `SDL_Delay(20)` (`ll_level.c:1015, 1649`), `SDL_GetTicks64()+30` (`ll_menu.c:666`), or integer px/frame math (review E-05). Game speed therefore depends on OS timer granularity: Windows' default timer resolution is **15.6 ms**, so `SDL_Delay(20)` often sleeps ~31 ms.

**Recommendation:**
```c
const uint64_t STEP = 1000000000ull / TICK_HZ;   /* 14.285714 ms */
uint64_t acc = 0, prev = plat_now_ns();
for (;;) {
    uint64_t now = plat_now_ns(); acc += now - prev; prev = now;
    if (acc > 8 * STEP) acc = 8 * STEP;          /* avoid spiral after window drag/suspend */
    while (acc >= STEP) { game_tick(); acc -= STEP; }   /* logic exactly 70/s */
    render(acc / (double)STEP);                   /* optional interpolation */
    plat_sleep_until(prev + STEP - acc);          /* or rely on vsync */
}
```
- Keep **all** logic (scroll, animations, menu blink, the `.SEQ` period) in ticks, not ms.
- The "55 ms timer class" one-shot waits (`ll_config.h:53`) equal ≈4 ticks. Express them in ticks too.
- With vsync on a 60/144 Hz display, logic stays at 70 Hz and rendering shows the latest state, so there's no speed drift.

### T-06 Get precise sleep on each OS
- **Windows:** call `timeBeginPeriod(1)` at start and `timeEndPeriod(1)` at exit (link `winmm`, which is already linked), or use a high-resolution waitable timer (`CreateWaitableTimerExW(..., CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, ...)`, Win10 1803+). Sleep until ~1 ms before the deadline, then spin on `SDL_GetPerformanceCounter`. SDL2 ≥2.0.18 has `SDL_HINT_TIMER_RESOLUTION` (default 1 ms) but only while SDL is initialized. Note that `is_player_run` calls `SDL_Quit` per scene, which resets it (review P-03).
- **Linux:** `clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, ...)` is accurate to well under 1 ms. Avoid relative `usleep` drift by always sleeping to an absolute deadline.

### T-07 Keep cutscene sync audio-master, but own the clock
**Evidence:** the `.IS` player derives video time from audio blocks (`is_player.h` comment is stale, review P-11). Pause is broken because two clocks exist (review P-02).

**Recommendation:** use a single `media_clock` struct `{t0_ns, paused, pause_start_ns}`. When audio is present, correct the clock from `SDL_GetQueuedAudioSize` (bytes consumed → samples → ns). When there is no audio, run it from the wall clock. Never compute deadlines from a local copy.

### T-08 Handle the window-drag and suspend stall
- **Windows:** dragging or resizing the window enters a modal loop, so `SDL_PollEvent` isn't called and the game freezes, and queued audio underruns (review E-09). Options: an `SDL_SetEventFilter` / `SDL_AddEventWatch` on `SDL_WINDOWEVENT_EXPOSED` to keep rendering, and the audio thread (T-09).
- On resume from suspend or a long stall, clamp `acc` (T-05) so the game doesn't fast-forward.

---

## 3. Audio tips

### T-09 Decouple audio from the UI loop
**Evidence:** `ll_level.c:174` `SDL_QueueAudio` is fed only from `ll_music_pump`, called in menu/level loops. `ll_level.c:39` documents why callbacks are avoided (PipeWire/ALSA problems; `ll_audio_init` swaps pipewire→alsa).

**Recommendation:**
- Use one **audio thread** that mixes music (MOD) + SFX (PCM) + cutscene PCM into one 44.1 kHz stereo stream, and keeps ≥100-200 ms queued via `SDL_QueueAudio`. Communicate with the game through a lock-free command ring (`play_sfx(id)`, `music(name)`, `volume(...)`).
- Open **one** audio device for the whole session. Currently the player and the music each open their own device, at 22050 Hz (cutscene) and 44100 Hz (`LL_MOD_RATE`, `ll_mod.h:46`). Resample the 22050 Hz `.IS` PCM by 2× (exact) or let `SDL_AudioStream` convert.
- Don't force `SDL_AUDIODRIVER=alsa` on modern distros. SDL ≥2.30's PipeWire backend is stable, and forcing ALSA breaks Flatpak sandboxes. Test with `SDL_AUDIODRIVER=pipewire|pulseaudio|alsa` and only work around it if it's actually broken.
- **Windows:** WASAPI shared mode. Callback or queue both work; latency is ~10-30 ms.

### T-10 Volume, mixing, and clipping
- Accumulate in `int32`, apply the master gain, and clamp once (today the code saturates per channel in `int16`; review M-10).
- Add separate music/SFX volume and mute-on-focus-loss. These are standard on modern platforms.
- MOD playback: compare against OpenMPT/libxmp (review M-01…M-12). Consider optionally linking **libxmp-lite** (MIT) or **micromod** (BSD) as a reference or replacement mixer, behind an option, to validate the hand-written player.

---

## 4. Video / display tips

### T-11 Aspect ratio: DOS pixels were not square
**Evidence:** content is 320×200 (scaled ×2 → 640×400) and 640×400 menu/map (`ll_menu.c:803`), level canvas 400×200 (`ll_level.c:1238`). Windows are created at `W*scale × H*scale`, which gives a **16:10** image. On a 1997 CRT, both 320×200 and 640×400 filled a **4:3** screen, so pixels were 1.2× taller than wide.

**Recommendation:** add `--aspect=4:3|square` (default 4:3 for authenticity). Implement it with `SDL_RenderSetLogicalSize(ren, 640, 480)` and a destination rect stretched vertically (200→240), or do an integer pre-scale (e.g. ×5 horizontally, ×6 vertically, then filter) to avoid uneven nearest-neighbour rows. Note that the 400×200 level canvas is not a VGA mode: check what the original displayed (likely 320 wide viewport + scroll) before choosing a scale.

### T-12 Scaling quality and HiDPI
- `SDL_HINT_RENDER_SCALE_QUALITY "0"` (nearest) for the pixel look. Optionally offer "sharp bilinear": integer-upscale to the largest multiple, then do a linear scale for the remainder.
- **Windows:** declare DPI awareness (PerMonitorV2) via the manifest or `SDL_HINT_WINDOWS_DPI_AWARENESS="permonitorv2"` **before** `SDL_Init`. Otherwise Windows bitmap-stretches the window (blurry).
- **Linux Wayland:** with SDL ≥2.26, `SDL_WINDOW_ALLOW_HIGHDPI` + `SDL_GetRendererOutputSize`. Remove the X11 forcing (`main.c:403`, review P-05) once there is a single window.
- **Mouse:** after logical-size rendering, SDL reports logical coordinates, so remove the manual `/sc` math (`ll_menu.c:683, 706, 1115`).

### T-13 Fullscreen
Use `SDL_WINDOW_FULLSCREEN_DESKTOP` (borderless) rather than exclusive mode changes. 320×200 modes don't exist on modern monitors, and exclusive mode switches cause long black screens on Windows. Toggle it with Alt+Enter/F11 and remember the choice in the config (T-17).

### T-14 Palette fidelity
VGA DAC values are 6-bit. The code uses `v<<2` (max 252). Prefer `(v<<2)|(v>>4)` for a true 0-255 white (review E-08). Make it switchable and keep golden hashes for both. Palette fades in the original operated on 6-bit values, so emulate fades in 6-bit space and then expand, otherwise fades show different banding.

---

## 5. Input tips

### T-15 Use scancodes for game keys, keycodes for text
**Evidence:** 39 `SDLK_`/`SDL_SCANCODE`/event references spread across the menu, level, and player. The DOS original read raw keyboard scancodes (INT 9 hook). Corner keys `a/o/r` (`ll_menu` map), `t/n` (tak/nie), and arrows are used.

**Recommendation:**
- Use **scancodes** for positional game controls (arrows, space, enter, esc) so they work on AZERTY/QWERTZ/Polish-programmer layouts.
- Use **keycodes** for mnemonic letters (`t`=tak, `n`=nie, `a`=autorzy, `o`=opcje, `r`=restart), since those are semantic.
- Centralize input in an action map (`ACT_UP, ACT_OK, ACT_BACK, ACT_PAUSE, ...`) with defaults, configurable via the config file. Map `SDL_GameController` (A=OK, B=BACK, Start=PAUSE, D-pad) onto the same actions.
- Ignore key-repeat events (`ev.key.repeat`) for actions that were edge-triggered in DOS.
- Windows: Alt+F4 → `SDL_QUIT` should behave like quit (`-2`), not crash mid-save.

### T-16 Text and Polish characters
**Evidence:** `ll_menu.c:2,19-20` uses a placeholder built-in 8×8 ASCII font (glyphs 32..126). The original game is Polish and uses diacritics (ą, ć, ę, ł, ń, ó, ś, ź, ż), most likely in **CP852** or **Mazovia** encoding in the data files.

**Recommendation:**
- Find out which encoding the original strings use (look for bytes ≥0x80 in the resources), and convert at load time to UTF-8 for logs and the window title, but keep the original byte values for rendering with the original bitmap font.
- Extract and use the **original game font** from the resources if one exists, instead of the placeholder.
- Never `printf` CP852 bytes to a Windows console. Convert to UTF-8 and `SetConsoleOutputCP(CP_UTF8)`.

---

## 6. File system / data tips

### T-17 Separate read-only data from user-writable state
**Evidence:** `ll_save.c` reads and writes the save/status file next to the data. `LEO_SAVE` overrides it. On Windows, `C:\Program Files\...` is read-only (UAC virtualization redirects writes to `VirtualStore`, which confuses users). Linux distro packages install to `/usr/share`.

**Recommendation:**
- **Data dir** (read-only) search order: `--data` CLI → `$LEO_DATA` → `SDL_GetBasePath()/data` → `$XDG_DATA_HOME/openleo` → `/usr/share/openleo` (Linux) → CWD.
- **User dir** (`SDL_GetPrefPath("LongSoft","LewLeon")`): saves, config (`openleo.ini`), logs, screenshots. On Windows this resolves to `%APPDATA%\LongSoft\LewLeon\`, on Linux to `~/.local/share/LongSoft/LewLeon/`.
- On first run, if a save exists in the data dir (from an original install), import it once.

### T-18 Case-insensitive, UTF-8 aware open in one place
The DOS data uses 8.3 uppercase names (`ANIMACJE`, `ETAPY`, `LOGO.IS`). Users copying from a CD to Linux often get lowercase names, and Windows users have non-ASCII home paths. Implement `plat_fopen` (T-01) once:
- Linux: exact `fopen` → per-component `opendir` + `strcasecmp` fallback (fixing review P-01), with the results cached in a small hash map.
- Windows: UTF-8 → UTF-16 + `_wfopen` (or `SDL_RWFromFile`). Case-insensitivity is free.
- Use 64-bit offsets (`fseeko`/`_fseeki64`) or `SDL_RWops` (`Sint64`) everywhere `long`/`ftell` is used today (`is_stream.c:27`, `llrs.c:80-82,170`).

### T-19 Validate the data set at startup
Print or show a table of the required files with found/missing status (LEVELDES, chapter `.IS`, AUTORZY.IS, …; review E-01). Optionally checksum known retail versions (CRC32/SHA-1 of each container) to detect corrupt CD rips. Show a clear message box on Windows (since there's no console, review P-06).

---

## 7. Build, packaging and distribution tips

### T-20 Builds
- Keep the Makefile, and add **CMake** (`find_package(SDL2 CONFIG)`) for MSVC, CLion, and VS Code users, and for vcpkg/Conan. Example: `cmake -B build -DCMAKE_TOOLCHAIN_FILE=$VCPKG/scripts/buildsystems/vcpkg.cmake`.
- **MSVC compatibility:** avoid GNU-only features in engine code. Check `strtok_r` (→ `strtok_s`), `strcasecmp` (→ `_stricmp`), `<dirent.h>` (not on MSVC), `__attribute__`, and VLAs. Put the shims in a `compat.h`.
- Add `-fstack-protector-strong -D_FORTIFY_SOURCE=2` (Linux release) and the existing `--dynamicbase --nxcompat --high-entropy-va` (already in `make win`, good).
- Strip release binaries: the Windows exe is 13.4 MB unstripped (review P-14).
- Consider an **SDL3** migration path. SDL3 (stable since 2025) has a better audio-stream API (fits T-09), built-in pixel-perfect logical presentation (`SDL_SetRenderLogicalPresentation` with `INTEGER_SCALE`), and better HiDPI. If T-01 is done, it's a ~1-file change. `sdl2-compat` lets SDL2 binaries run on SDL3 on distros dropping SDL2.

### T-21 Windows packaging
- An `app.manifest` with: `dpiAwareness PerMonitorV2`, `activeCodePage UTF-8` (Win10 1903+; makes `fopen` UTF-8, though this only takes effect with a UCRT-linked build, not msvcrt), `longPathAware`, and a `supportedOS` list.
- An icon and `VERSIONINFO` `.rc` via `windres`.
- Choose static vs `SDL2.dll`. Static (current) gives a single exe, but users can't drop in an updated SDL for fixes. Shipping `SDL2.dll` next to the exe is the common choice.
- Prefer a **UCRT** toolchain (MSYS2 `ucrt64`) over msvcrt (current build links `msvcrt.dll`). UCRT gives UTF-8 locale support and C99/C11 conformant printf.
- Code-sign if possible. Unsigned exes trigger SmartScreen warnings.
- Zip layout: `LewLeon.exe`, `README.txt` (where to copy the original data), `data/` (empty with a placeholder).

### T-22 Linux packaging
- **AppImage** (simple) or **Flatpak** (sandboxed; data via `--filesystem` or a file-chooser portal for the data dir). Avoid forcing ALSA in Flatpak (T-09).
- A `.desktop` file + icon + AppStream metainfo.
- Respect `XDG_*` dirs (T-17).
- Build on an older glibc baseline (e.g. Ubuntu 22.04 container) for portable binaries, or link SDL statically.
- Check both X11 and Wayland sessions (GNOME needs client-side decorations: SDL ≥2.30 uses libdecor if available).

---

## 8. Reverse-engineering hygiene (porting the remaining game logic)

### T-23 Keep RE traceability, but structured
65 `FUN_`/address references are useful. Standardize them as `/* RE: FUN_00056c72 @0x56c72 – blit window */` so a script can produce a cross-reference table (address → C function). Store the Ghidra project export or a symbol map (`tools/symbols.csv`) in the repo (no binary data).

### T-24 Golden-master testing against the original
**Evidence:** the README references `tools/is_decode.py`, `tools/llrs_unpack.py` "oracles" and `make verify-all`, which are missing from the snapshot (review §1).

**Recommendation:** restore them. Re-implementing every format in Python and comparing against the C output is an excellent cross-check. Extend this to **DOSBox-X / QEMU frame captures** of the original at known tick counts (the comment in `ll_config.h:49` shows QEMU was already used as a timing oracle), and compare with `LEO_MENU_PPM_TICK` / `LEO_LEVEL_PPM_TICK` dumps from the port. Store only hashes in the repo, not frames (copyright).

### T-25 Fuzz the parsers
The format parsers (`is_stream`, `llrs`, `ll_pcx`, `ll_seq`, `ll_pcm`, `ll_mod`) take untrusted input (user-supplied data files). Add libFuzzer targets (`clang -fsanitize=fuzzer,address`) with the shipped files as seed corpus. Known weak spots: `llrs` sizes (review E-07), `.SEQ count==0` (E-04), and negative `.MOV` index (E-03).

### T-26 Use the original's state machine, not ad-hoc loops
The DOS game is a single main loop with screen states (logo → intro → map → level → …). The port has nested blocking loops (`run_menu` calls `ll_menu_map_open`, which loops; that calls `ll_level_show`, which loops). Flatten into one loop with a `screen` stack:
```c
typedef struct screen { void (*enter)(struct screen*); int (*tick)(struct screen*, const ll_event*, int n);
                        void (*render)(struct screen*, uint8_t *fb, uint8_t *pal); void (*leave)(struct screen*); } screen;
```
This enables the single window (P-03), the continuous audio pump (T-09), a global pause/fullscreen/screenshot hotkey, and Emscripten (which requires a non-blocking main loop via `emscripten_set_main_loop`).

---

## 9. Modern-OS checklist (quick reference)

| # | Item | Linux | Windows |
|---|---|---|---|
| 1 | Single window, created once | ✓ | ✓ |
| 2 | Fixed 70 Hz logic, absolute-deadline sleep | `clock_nanosleep` | `timeBeginPeriod(1)` / high-res waitable timer |
| 3 | Audio thread, one device, 44.1 kHz | PipeWire/Pulse default, no ALSA forcing | WASAPI |
| 4 | HiDPI | `ALLOW_HIGHDPI` | manifest PerMonitorV2 |
| 5 | Fullscreen desktop toggle | ✓ | ✓ (Alt+Enter) |
| 6 | 4:3 aspect option | ✓ | ✓ |
| 7 | UTF-8 paths | native | `_wfopen` / SDL_RWops / UCRT + manifest |
| 8 | Case-insensitive data lookup | dir scan | native |
| 9 | User dir for saves/config/logs | XDG via `SDL_GetPrefPath` | `%APPDATA%` via `SDL_GetPrefPath` |
| 10 | Visible errors | stderr | MessageBox + log file |
| 11 | No focus stealing | ✓ | ✓ |
| 12 | Gamepad | `SDL_GameController` | same |
| 13 | Packaging | AppImage/Flatpak + .desktop | zip/installer, icon, VERSIONINFO, signed |
| 14 | CI | gcc + ASan/UBSan headless | mingw cross + MSYS2 native smoke |

---

## 10. Suggested order of work
1. T-01/T-26 (platform interface + screen state machine). This unlocks most of the rest.
2. T-05/T-06/T-07 (timing) and T-09 (audio thread).
3. T-17/T-18/T-19 (file system and data validation).
4. T-11…T-14 (display), T-15/T-16 (input and Polish text).
5. T-20…T-22 (packaging), T-23…T-25 (RE hygiene, oracles, fuzzing) in parallel as CI work.

Constraints: preserve the `LEO_*` env hooks and the return-code conventions (listed in the review document §1), keep pixel-exact `.IS` decoding, and never commit copyrighted game data.
