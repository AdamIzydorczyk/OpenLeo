# Lew Leon recompile - ONE shared codebase (src/) for both native targets:
#   make          -> ll_cutscene       (native Linux ELF, SDL2 via pkg-config)
#   make win      -> ll_cutscene.exe   (Windows x86-64, cross-built with MinGW
#                                        or the vendored llvm-mingw clang)
# Same sources, same SDL2 platform layer; only compiler flags/libs differ.
# original/ is the faithful-DOS blueprint (Ghidra output, not buildable).
SHELL    := /bin/bash   # verify-llrs uses process substitution
DATA     ?= /tmp/opencode/iso_stage/LEO
PYREF    ?= tools/is_decode.py

SRC      := src/main.c src/is_stream.c src/platform_sdl/is_player.c
HDRS     := src/ll_config.h src/is_stream.h src/platform_sdl/is_player.h \
            src/llrs.h

# ---------------- native Linux ----------------
CC       ?= gcc
CFLAGS   ?= -O2 -std=gnu11 -Wall -Wextra -DPLATFORM_LINUX=1 \
            $(shell pkg-config --cflags sdl2)
LIBS     := $(shell pkg-config --libs sdl2) -lm

ll_cutscene: $(SRC) $(HDRS)
	$(CC) $(CFLAGS) -Isrc -o $@ $(SRC) $(LIBS)

run: ll_cutscene
	./ll_cutscene $(DATA)/INTRO_P.IS

# ---------------- Windows 11 x86-64 (cross) ----------------
# Toolchain (optional, run once): sudo pacman -S --needed mingw-w64-toolchain
# XC := pacman-installed mingw-w64 gcc if present, else the vendored
# llvm-mingw cross toolchain (vendor/llvm-mingw, gitignored).
XC        ?= $(shell command -v x86_64-w64-mingw32-gcc 2>/dev/null || \
                    echo vendor/llvm-mingw/bin/x86_64-w64-mingw32-clang)
SDL_WIN   ?= vendor/SDL2-2.32.10/x86_64-w64-mingw32
XCFLAGS   := -O2 -std=gnu11 -Wall -Wextra -DPLATFORM_WIN=1 -Dmain=SDL_main \
             -Isrc -I$(SDL_WIN)/include
XLD       := -static -static-libgcc -Wl,--dynamicbase -Wl,--nxcompat \
             -Wl,--high-entropy-va
XLIBS     := -L$(SDL_WIN)/lib -lmingw32 -lSDL2main -lSDL2 -mwindows \
             -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm \
             -limm32 -lole32 -loleaut32 -lshell32 -lsetupapi -lversion -luuid

win: ll_cutscene.exe
ll_cutscene.exe: $(SRC) $(HDRS)
	$(XC) $(XCFLAGS) $(XLD) -o $@ $(SRC) $(XLIBS)

# ---------------- tests (both platforms) ----------------
# Byte-exactness oracle: headless --ppm output vs tools/is_decode.py.
verify: ll_cutscene
	./ll_cutscene $(DATA)/INTRO_P.IS --ppm 1020 /tmp/opencode/c_linux.ppm
	python3 $(PYREF) frames $(DATA)/INTRO_P.IS /tmp/opencode/c_py --ppm \
	    --only 1020
	cmp /tmp/opencode/c_linux.ppm /tmp/opencode/c_py/f01020.ppm \
	  && echo "linux .elf == python reference (byte-exact)"

# ESC-skip UX test: auto-terminates (LEO_DUMP_SKIP sets CI mode) and the
# final held frame must equal the python reference of the last frame.
verify-skip: ll_cutscene
	LEO_DUMP_SKIP=/tmp/opencode/c_skip.ppm timeout 120 \
	  ./ll_cutscene $(DATA)/INTRO_P.IS --skip-at 10 --no-audio
	cmp /tmp/opencode/c_skip.ppm /tmp/opencode/py_last/f01520.ppm \
	  && echo "linux skip-frame == python reference (byte-exact)"

# Same two proofs for the Windows .exe, run under wine.
# Z: = wine drive mapped to /, so $(DATA) is addressed as Z:$(DATA).
WINEPREFIX ?= /tmp/opencode/wineprefix
verify-win: ll_cutscene.exe
	WINEPREFIX=$(WINEPREFIX) WINEDEBUG=-all \
	  wine ./ll_cutscene.exe Z:$(DATA)/INTRO_P.IS --ppm 1020 \
	  Z:/tmp/opencode/c_win.ppm
	cmp /tmp/opencode/c_win.ppm /tmp/opencode/c_py/f01020.ppm \
	  && echo "win .exe == python reference (byte-exact)"
	WINEPREFIX=$(WINEPREFIX) WINEDEBUG=-all LEO_DUMP_SKIP=Z:/tmp/opencode/c_wskip.ppm \
	  timeout 180 wine ./ll_cutscene.exe Z:$(DATA)/INTRO_P.IS \
	  --skip-at 10 --no-audio
	cmp /tmp/opencode/c_wskip.ppm /tmp/opencode/py_last/f01520.ppm \
	  && echo "win skip-frame == python reference (byte-exact)"

# src/llrs.c byte-for-byte agreement with tools/llrs_unpack.py on all four
# containers, checked under ASan/UBSan.
llrs_selftest: tools/llrs_selftest.c src/llrs.c src/llrs.h
	$(CC) -O1 -g -std=gnu11 -Wall -Wextra -fsanitize=address,undefined \
	    -Isrc -o $@ tools/llrs_selftest.c src/llrs.c

verify-llrs: llrs_selftest
	@for F in ANIMACJE ETAPY SOUND LEVELDES; do \
	  if diff <(./llrs_selftest $(DATA)/$$F) \
	      <(python3 tools/llrs_unpack.py list $(DATA)/$$F 2>/dev/null) >/dev/null; \
	  then echo "$$F: C == python (exact)"; \
	  else echo "$$F: MISMATCH"; exit 1; fi; done
	@echo "MENU index block (39 records):"
	./llrs_selftest --block $(DATA)/ANIMACJE MENU | head -3

verify-all: verify verify-skip verify-win verify-llrs
	@echo "ALL TESTS PASSED (linux + windows + llrs)"

clean:
	rm -f ll_cutscene ll_cutscene.exe llrs_selftest *.o *.obj

.PHONY: run win verify verify-skip verify-win verify-llrs verify-all clean
