# Lew Leon (Longsoft, 1997) — recompile project

Source-project replica of the Polish DOS game "Lew Leon". Original game
files are **never** in this repo (user-supplied, see .gitignore); we only
store our source, tools and RE reference material.

Layout:

- `src/` — the whole native codebase, shared by every target:
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
- `tools/` — python reference implementations + oracle/self-test harnesses.
- `build/` — RE artifacts (leo.img, Ghidra project/logs; regenerate-only).

## Build

```
make          # linux
make win      # windows .exe (needs vendor/SDL2-2.32.10-mingw + llvm-mingw,
              # or a system x86_64-w64-mingw32-gcc; both gitignored)
```

## Run

```
./ll_cutscene /path/to/LEO/INTRO_P.IS      # data dir = user's original files
./ll_cutscene --demo /path/to/LEO          # boot chain: LOGO.IS -> PRAWA.IS
                                           # -> INTRO_P.IS -> title menu ->
                                           # level intro (NEW GAME = RAN.01)
./ll_cutscene A.IS --then B.IS --menu /path/to/LEO   # manual scene chain
./ll_level /path/to/LEO RAN.01             # level wiring (.DES) + intro art
```

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
# verify-llrs  src/llrs.c == tools/llrs_unpack.py on all 4 containers
# verify-pcx   src/ll_pcx.c == tools/pcx_dump.py on all 6 container PCX payloads
# verify-seq   src/ll_seq.c == tools/seq_parse.py on all 172 container .SEQ payloads
# verify-pcm   src/ll_pcm.c == tools/verify_pcm.py on every PCM parallax layer
# verify-mod   MOD music renderer: deterministic render linux == wine
# verify-level level scene: .DES wiring + .SEQ/.MOV anims + scroll camera
# verify-help  help overlay: POMOC pages == python oracle
# verify-menu  hooked demo chain: cutscene -> skip -> menu -> restart dialog
#              + bright-green answer selection (tools/verify_green.py oracle)
# verify-sel   LEVELS/MAPKA submenu + mapka.mod
# verify-save  STATUS.LEO/LONGSOFT.CFG byte-exact round-trip (skips when
#              the user-supplied originals are absent; never in git)
```

CI hooks used by verify-menu (env): `LEO_DEMO_SKIP_FRAMES` (auto-skip demo
scenes), `LEO_SKIP_USER=1` (auto-skip reported as a user ESC skip),
`LEO_MENU_AUTO=N` (auto-pick menu item), `LEO_LEVEL_SECS` (intro hold),
`LEO_MENU_KEY=<char>` (auto-press a map corner: r = restart dialog),
`LEO_MENU_SEL=i` (pre-select answer zone i, highlight only),
`LEO_MENU_PPM=PATH` + `LEO_MENU_PPM_TICK=N` (dump the composed 320x200
frame), `LEO_MENU_PICK=<char>` / `LEO_MENU_NAV=k` (fire / cycle + Enter).

## Naming

`is_*` = .IS cutscene format (oracle `tools/is_decode.py`); `llrs_*` = the
"llrs" resource container format (oracle `tools/llrs_unpack.py`); `ll_*` =
project glue that is neither wire format. One SDL2 platform layer
(`src/platform_sdl/`) for both targets - no per-platform source forks.

## Both native builds are no-CD and use fixed-timestep pacing (the original's
execution-rate-bound busy loop would run thousands of times too fast on
modern hardware).
