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
- build: `Makefile.mgw` -> `leo.exe` (static-linked SDL2 for portability);
  `Makefile` for MSVC/CL or CMake later if wanted
