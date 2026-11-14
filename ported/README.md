# ported — the native port (Linux + Windows)

The port that runs the engine (`original/`) on modern hardware:

- `main.c` — shared entry point + boot chain (logo → intro → title menu
  → level intro), the one place the engine is driven.
- `platform_sdl/is_player.{c,h}` — the single SDL2 platform layer
  (window, audio, input, pacing loop) used by **both** native targets.

One port, two binaries: the root `Makefile` compiles `original/` +
`ported/` into `ll_cutscene` (Linux ELF) with `make` and into
`ll_cutscene.exe` (static Windows x86-64) with `make win` — same
sources, only compiler flags and libraries differ.