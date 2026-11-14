# win11 — native Windows 11 build, no CD

Same shared engine (`../src`) as `../linux`, built natively for modern
Windows 11 x86-64 — no DOS, no DOSBox, no CD. Cross-compiled from Linux with
MinGW-w64 (`x86_64-w64-mingw32-gcc`), also buildable on Windows directly.

- video/audio/input: the **same SDL2 platform layer** as `../linux`
  (SDL2 is cross-platform); only the small OS glue differs (paths, console,
  UTF-8/ANSI, case-insensitive filesystem)
- pacing: fixed-timestep game clock (~22 Hz tick) off QueryPerformanceCounter
  + sleep - never execution-rate bound like the original (modern CPUs would
  make it run thousands of times too fast)
- data: user-supplied game files in a directory, path from argv/env
  (WANT_CD_GATE=0: CD gate code simply not compiled in)
- build (cross from Linux): `sudo pacman -S --needed mingw-w64-toolchain`
  (Arch/CachyOS; Debian: `apt install gcc-mingw-w64-x86-64`), then fetch the
  SDL2 MinGW libs once: `curl -LO https://www.libsdl.org/release/SDL2-devel-2.32.10-mingw.tar.gz
  && mkdir -p ../vendor && tar xzf SDL2-devel-*-mingw.tar.gz -C ../vendor/`
  (vendored dir is gitignored); then `make` -> `ll_cutscene.exe`,
  a static-linked single .exe (no SDL2.dll needed at runtime)
