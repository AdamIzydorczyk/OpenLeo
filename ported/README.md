# ported — the native port (Linux + Windows)

This directory IS the port: C sources written from the reverse-engineered
formats and logic of the 1997 game. (`original/` in the repo root holds the
recovered DOS material and is not built.) The root `Makefile` compiles
these files into `ll_cutscene` (Linux ELF) with `make`, into
`ll_cutscene.exe` (static Windows x86-64) with `make win`, and builds the
level viewer `ll_level` / `ll_level.exe` with `make ll_level` / `make win`
- same sources, only compiler flags and libraries differ.

Layout (engine/app code is SDL-free; `make verify-arch` pins that):

| file(s) | what |
|---|---|
| `main.c` | entry point + boot chain (logo -> intro -> title menu -> level) |
| `platform.h`, `platform_common.c`, `platform_dummy.c`, `platform_sdl.c`, `platform_sdl/is_player.*` | the platform interface, the headless backend and the one SDL2 backend (window, audio, input, pacing) |
| `ll_view.*`, `ll_audio.*` | the SDL window/renderer and audio session (backend-only headers) |
| `is_stream.*` | the `.IS` cutscene stream decoder |
| `llrs.*` | the LLRS container reader (ANIMACJE/ETAPY/SOUND/LEVELDES, sparse tables) |
| `ll_pcx.*`, `ll_pcm.*` | PCX images; PCM parallax layers (format partly unresolved, see NEXT_STEPS PR-50) |
| `ll_seq.*` | `.SEQ` sprite scripts: grammar, the two row codecs, anchored/mirrored element draw, shape geometry |
| `ll_menu.*`, `ll_menu_*.h` | the chapter map, restart dialog and help-adjacent screens |
| `ll_level.*` | the level scene: `.des` wiring, the authored object scene, `.MOV` object scripts, camera, help overlay |
| `ll_play.*` | the `.plt` ground, the `.MOV` state machine and the playable lion |
| `ll_mod.*` | the ProTracker `.MOD` music renderer |
| `ll_save.*`, `ll_paths.*`, `fs_ci.*`, `ll_config.h` | save files, per-user directories, case-insensitive/UTF-8 file opener, build constants |

Every decoder has an independent python reference in `../tools/` and a
`make verify-*` target that compares the two byte for byte; see the top
level `README.md` for the list and `NEXT_STEPS.md` for what each PR proved.
