# linux — native Linux build (SDL2), no CD

Shared engine (`../src`) compiled natively (gcc/clang, 64-bit, libc startup,
no DOS/DPMI). Inherits the no-CD design: zero CD/DVD code paths, data
directory from argv/env pointing at user-supplied original data files.

Platform layer:
- video: SDL2 surface (640x480x8 pal + 320x200 Mode X equivalents), DAC
  palette from `.IS` PAL ops (6-bit -> 8-bit `v<<2`)
- audio: SDL2 audio callback for the 8-bit ~8 kHz PCM + WAV/MOD from SOUND
- input: SDL events (keyboard/gamepad -> the engine's 0x1b-style key states)
- files: POSIX open/read; llrs + .IS readers from `../tools` semantics,
  reimplemented portable-C in `../src`

`.IS` codec spec = `../tools/is_decode.py` header (live-verified byte-exact).
Build: `Makefile` (SDL2 via pkg-config). Original binaries/data never in git;
user supplies the data dir.
