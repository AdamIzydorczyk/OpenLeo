# Lew Leon (1997) — native port

Native port of the 1997 Polish DOS game "Lew Leon", written in plain C for
modern hardware. The repo holds exactly what is needed to build and run
the application: the engine, the port, and its data.

Layout:

- `original/` — the engine: the whole C codebase of the game (format
  codecs, title menu, level intro, save codec - full file list in
  `original/README.md`).
- `ported/` — the native port: the entry point (`main.c`) plus the single
  SDL2 platform layer (`platform_sdl/`) that runs the engine on both
  targets (see `ported/README.md`).
- `data/` — the game data the application runs on (cutscene `.IS` movies
  and the resource containers: `ANIMACJE`, `ETAPY`, `SOUND`).
- `Makefile` (root) — builds `original/` + `ported/` into both native
  targets: `make` → `ll_cutscene` (Linux ELF, SDL2 via pkg-config),
  `make win` → `ll_cutscene.exe` (Windows x86-64, MinGW-w64 or a vendored
  cross toolchain; static, no DLLs).
- `README.md` + the two directory READMEs — how to build, run, and
  extend.

## Build

```
make          # linux  (needs SDL2: pkg-config, e.g. pacman -S sdl2)
make win      # windows .exe (needs an x86_64-w64-mingw32 toolchain;
              # see the Makefile notes / vendor/ layout)
```

## Run

The application takes its data directory, the bundled `data/` included:

```
./ll_cutscene --demo data            # full boot chain on the bundled data
./ll_cutscene data/INTRO_P.IS        # a single cutscene
./ll_cutscene --demo /path/to/LEO    # or on any copy of the game data:
                                      # boot chain: LOGO.IS -> PRAWA.IS ->
                                      # INTRO_P.IS -> title menu -> level
                                      # intro (NEW GAME = RAN.01)
./ll_cutscene A.IS --then B.IS       # manual scene chain
```

The other game scenes (KARA, AUTORZY, ENA1/2, KADMAR1/2, KONIEC, LONA,
RAN, SADES, ZARIA1/2) and the `LEVELDES` container are part of the game's
data set, too: copy them into your data directory and they play the same
way (`ll_cutscene /path/to/KARA.IS`).

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

## Naming

`is_*` = .IS cutscene format; `llrs_*` = the "llrs" resource container
format; `ll_*` = project glue that is neither wire format. The engine
(`original/`) and the port (`ported/`) share one codebase: the two builds
differ only in compiler flags, not in source files.

## Both native builds are no-CD and use fixed-timestep pacing (the
original's execution-rate-bound busy loop would run thousands of times too
fast on modern hardware).