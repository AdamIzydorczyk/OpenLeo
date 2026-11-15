# Lew Leon (Longsoft, 1997) — recompile project

Source-project replica of the Polish DOS game "Lew Leon". Original game
files stay out of day-to-day development (user-supplied CD, see
.gitignore); the upstream public snapshot additionally tracks a small
vendored core under `data/` (see below) - never add more copyrighted
game data.

Layout (mirrors the upstream repo at github.com/AdamIzydorczyk/OpenLeo):

- `ported/` — the whole native codebase, shared by every target:
  `.IS` codec (`is_stream.*`, byte-exact vs `tools/is_decode.py`),
  `llrs` container reader (`llrs.*`, byte-exact vs `tools/llrs_unpack.py`),
  container PCX reader (`ll_pcx.*`, byte-exact vs `tools/pcx_dump.py`),
  SDL2 cutscene player (`platform_sdl/`), title menu (`ll_menu.*`),
  level intro viewer (`ll_level.*`), shared entry (`main.c`),
  variant knobs (`ll_config.h`).
- `Makefile` (root) — single build for both native platforms:
  `make` → `ll_cutscene` (Linux ELF, SDL2 via pkg-config),
  `make win` → `ll_cutscene.exe` (Windows x86-64, MinGW-w64 or vendored
  `vendor/llvm-mingw`, static, no SDL2.dll needed).
- `original/` — faithful-DOS variant blueprint (Ghidra decompilation,
  frozen, not buildable).
- `data/` — the vendored CD core tracked by the public snapshot
  (ANIMACJE/ETAPY/SOUND/LEVELDES containers + LOGO/PRAWA/INTRO_P.IS);
  the full playable data set stays local/user-supplied.
- `tools/` — python reference implementations + oracle/self-test harnesses.
- `build/` — RE artifacts (leo.img, Ghidra project/logs; regenerate-only).

## Build

```
make          # linux
make win      # windows .exe (needs vendor/SDL2-2.32.10-mingw + llvm-mingw,
              # or a system x86_64-w64-mingw32-gcc; both gitignored)
```

Supported Windows baseline: **Windows 10 1903+** — the embedded
`resources/app.manifest` activates the UTF-8 code page, so game-data
paths with non-ASCII characters (Polish folders etc.) resolve; on older
Windows the game runs but only ASCII-only data paths work (the binary
logs a `main: ANSI code page …` warning in that case).

## Run

```
./ll_cutscene /path/to/LEO/INTRO_P.IS      # data dir = user's original files
./ll_cutscene --demo /path/to/LEO          # boot chain: LOGO.IS -> PRAWA.IS
                                           # -> INTRO_P.IS -> title menu ->
                                           # level intro (NEW GAME = RAN.01)
./ll_cutscene A.IS --then B.IS --menu /path/to/LEO   # manual scene chain
./ll_level /path/to/LEO RAN.01             # level wiring (.DES) + intro art
```

CLI contract (FIX-13): `--help`/`-h` prints usage (exit 0), `--version`
prints the version (exit 0); unknown `--flags` are rejected with
`main: unknown option` + usage (exit 2 — as are "nothing to run" bare
launches and `--demo <dir>` on a directory without ANIMACJE/ETAPY;
more than 16 `--then` entries warn on stderr and the extras are
ignored). stdout is line-buffered from startup (logs survive a crash);
on Windows `--console` attaches before any output and uses an
unbuffered stream. A missing cutscene in an interactive menu surfaces
as a message box.

q mid-cutscene QUITS THE APP (owner ruling, FIX-01/C3): the scene chain
and the menu never come back - same exit as closing the window.
ESC mid-cutscene SKIPS it and the chain advances to the next scene or the
title menu (original semantics): no frozen mid-intro frame, the app never
dies. Menu keys: up/down, Enter picks; ESC = Enter. The restart-confirm
dialog answers (tak/nie/x) are selected with LEFT/RIGHT (the words sit
side by side; up/down ride the same cycle) - the selected word is blitted
at the BRIGHT end of the palette's green ramp (idx22 #10f91b), gets a
bright underline, and pulses its authored 2-frame animation; unselected
words sit dim (top idx15) and frozen. nie starts out selected (the safe
default answer; also the word the original art ships bright). Enter
confirms, t/n/x are direct keys.

## Tests (both platforms)

```
make verify-all
# verify       linux --ppm frame 1020 == python reference (byte-exact)
# verify-skip  linux ESC-skip final frame == python reference
# verify-win   same two proofs for the .exe, run under wine
# verify-llrs  ported/llrs.c == tools/llrs_unpack.py on all 4 containers
# verify-pcx   ported/ll_pcx.c == tools/pcx_dump.py on all 6 container PCX payloads
# verify-seq   ported/ll_seq.c == tools/seq_parse.py on all 172 container .SEQ payloads
# verify-pcm   ported/ll_pcm.c == tools/verify_pcm.py on every PCM parallax layer
# verify-mod   MOD music renderer: deterministic render linux == wine
# verify-level level scene: .DES wiring + .SEQ/.MOV anims + scroll camera
# verify-help  help overlay: POMOC pages == python oracle
# verify-menu  hooked demo chain: cutscene -> skip -> menu -> restart dialog
#              + bright-green answer selection (tools/verify_green.py oracle)
# verify-sel   LEVELS/MAPKA submenu + mapka.mod
# verify-save  STATUS.LEO/LONGSOFT.CFG byte-exact round-trip (skips when
#              the user-supplied originals are absent; never in git)
# verify-music MOD player behind plat_audio (threaded session, dummy-safe)
# verify-arch  structural pin: only platform_sdl*/ll_view/ll_audio may
#              include SDL (T-01 end-state; compile-free grep)
# verify-golden T-24: ORIGINAL game in the QEMU rig vs the port's chapter
#              map; the repo pins (tools/golden_expect.json) are HASHES
#              only - never frames. Skips entirely when the game fixture
#              is absent; without a QEMU capture it still checks the port
#              pose pins (make golden_rig builds the original-side capture)
```

CI hooks used by verify-menu (env): `LEO_DEMO_SKIP_FRAMES` (auto-skip demo
scenes), `LEO_SKIP_USER=1` (auto-skip reported as a user ESC skip),
`LEO_MENU_AUTO=N` (auto-pick menu item), `LEO_LEVEL_SECS` (intro hold),
`LEO_MENU_KEY=<char>` (auto-press a map corner: r = restart dialog),
`LEO_MENU_SEL=i` (pre-select answer zone i, highlight only),
`LEO_MENU_PPM=PATH` + `LEO_MENU_PPM_TICK=N` (dump the composed 320x200
frame), `LEO_MENU_PPM_DIR=DIR` + `LEO_MENU_PPM_MAX=N` (dump every composed
map frame map_NNNN.ppm for the golden-master pose scan),
`LEO_MENU_PICK=<char>` / `LEO_MENU_NAV=k` (fire / cycle + Enter),
`LEO_QUIT_AFTER_MS=N` (inject a window close N ms after the first frame —
proves the FIX-01 quit-unwind path headlessly; used by verify-quit),
`LEO_TEST_KEY_AFTER_MS=N` + `LEO_TEST_KEY=q` (inject one synthetic KEYDOWN
N ms after the first poll — pushed through SDL's queue, so it travels the
full map_key/repeat-filter path; the other half of verify-quit proves q
quits the app (FIX-01/C3)).

Runtime policy hooks: `LEO_FORCE_X11=1` (under a Wayland session pin the
video backend to X11/XWayland — the pre-FIX-11 default; default now is
SDL's own choice, X11-first on SDL2 classic), `LEO_PIN_ALSA=1` (opt in
to pinning the audio backend to alsa when SDL would pick its native
pipewire backend — the pre-PR-35 default; the default is now SDL's own
choice, which the owner's A/B ear test showed is clean on the
native-rate device. `LEO_KEEP_PIPEWIRE=1` is accepted as a no-op for
compatibility),
`LEO_AV_DEBUG=1` (per-second `is_player: av min_queued/err` A/V sync
diagnostics) + `LEO_AV_LATENCY_MS=N` (nominal output latency used in
that comparison, default 46 ms), `SDL_AUDIODRIVER`/`SDL_VIDEODRIVER`
(user pins always win over our hints/defaults).

Audio runs as ONE device for the whole process, opened at the hardware's
native rate (FIX-09 Part 2): each cutscene/MOD playback is a stream
converted on the fly, scene changes never close/reopen the device (no
WASAPI/Bluetooth reopen latency, no pop). The `ll_audio: device …` log
line therefore appears exactly once per run. `LEO_IS_WAV`/`LEO_MUSIC_WAV`
dumps stay byte-identical — they hold the un-converted source bytes.

## Naming

`is_*` = .IS cutscene format (oracle `tools/is_decode.py`); `llrs_*` = the
"llrs" resource container format (oracle `tools/llrs_unpack.py`); `ll_*` =
project glue that is neither wire format. One SDL2 platform layer
(`ported/platform_sdl/`) for both targets - no per-platform source forks.

## Both native builds are no-CD and use fixed-timestep pacing (the original's
execution-rate-bound busy loop would run thousands of times too fast on
modern hardware).
