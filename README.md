# Lew Leon (Longsoft, 1997) — recompile project

Source-project replica of the Polish DOS game "Lew Leon". Original game
files are **never** in this repo (user-supplied, see .gitignore); we only
store our source, tools and RE reference material.

Layout:

- `src/` — the whole native codebase, shared by every target:
  `.IS` codec (`is_stream.*`, byte-exact vs `tools/is_decode.py`),
  `llrs` container reader (`llrs.*`, byte-exact vs `tools/llrs_unpack.py`),
  SDL2 player/engine (`platform_sdl/`), shared entry (`main.c`),
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
```

## Tests (both platforms)

```
make verify-all
# verify       linux --ppm frame 1020 == python reference (byte-exact)
# verify-skip  linux ESC-skip final frame == python reference
# verify-win   same two proofs for the .exe, run under wine
# verify-llrs  src/llrs.c == tools/llrs_unpack.py on all 4 containers
# verify-pcx   src/ll_pcx.c == tools/pcx_dump.py on all 6 container PCX payloads
```

## Naming

`is_*` = .IS cutscene format (oracle `tools/is_decode.py`); `llrs_*` = the
"llrs" resource container format (oracle `tools/llrs_unpack.py`); `ll_*` =
project glue that is neither wire format. One SDL2 platform layer
(`src/platform_sdl/`) for both targets - no per-platform source forks.

## Both native builds are no-CD and use fixed-timestep pacing (the original's
execution-rate-bound busy loop would run thousands of times too fast on
modern hardware).
