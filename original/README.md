# original — the engine

All engine sources (plain C, no platform dependency). The
native port in `ported/` runs them; the root `Makefile` compiles this
directory + `ported/` into both targets (`make` → Linux ELF,
`make win` → static Windows x86-64 .exe). Build/run/test instructions:
`README.md` at the repo root.

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

Every module above is covered by a byte-exact or numeric oracle in
`tools/`, all run by `make verify-all`.