# original — the engine

Recovered original-engine material: the faithful-DOS reconstruction of
the 1997 engine (Ghidra-derived sources, with the decompiled shape
kept). It is NOT BUILT - the root `Makefile` says so in its header
("original/ is the faithful-DOS blueprint (Ghidra output, not
buildable)") and no recipe compiles this directory; it is read as
the blueprint the port is measured against. The built native port is
`ported/` (Linux ELF via `make`, static Windows x86-64 .exe via
`make win`). Build/run/test instructions: `README.md` at the repo
root.

Sources:

- `is_stream.{c,h}` — `.IS` cutscene video codec (v2.01), byte-exact vs the
  python reference `tools/is_decode.py`.
- `llrs.{c,h}` — "llrs" resource container reader (ANIMACJE/ETAPY/SOUND/
  IMAGES blocks), reference `tools/llrs_unpack.py`.
- `ll_pcx.{c,h}` — container PCX image reader, reference `tools/pcx_dump.py`.
- `ll_seq.{c,h}` — `.SEQ` animation-script reader (title menu, level
  intro, map anims).
- `ll_pcm.{c,h}` — PCM parallax image layers (level-intro art, map).
- `ll_mod.{c,h}` — MOD music player (title menu, map screen).
- `ll_menu.{c,h}` (+ `ll_menu_dac.h`) — title menu: 320x200 compositor,
  hot zones, restart dialog with two-directional answer states (bright +
  pulsing when selected, dim + frozen when not), keyboard navigation,
  SDL_QueueAudio mixing.
- `ll_level.{c,h}` — level-intro viewer: `.DES` scene composer, `.SEQ`/
  `.MOV` wiring, scroll camera.
- `ll_save.{c,h}` — STATUS.LEO savegame + LONGSOFT.CFG settings codec
  (byte-exact round-trip).
- `ll_config.h` — variant knobs shared with the port.

The modules above are the recovered original's parts. The native
port in `ported/` reimplements each of them, and it is the port's
side that carries the byte-exact oracles in `tools/`, run by
`make verify-all`; this directory is the blueprint they are measured
against, not a build input.