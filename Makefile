# Lew Leon recompile - ONE shared codebase (src/) for both native targets:
#   make          -> ll_cutscene       (native Linux ELF, SDL2 via pkg-config)
#   make win      -> ll_cutscene.exe   (Windows x86-64, cross-built with MinGW
#                                        or the vendored llvm-mingw clang)
# Same sources, same SDL2 platform layer; only compiler flags/libs differ.
# original/ is the faithful-DOS blueprint (Ghidra output, not buildable).
SHELL    := /bin/bash   # verify-llrs uses process substitution
DATA     ?= /tmp/opencode/iso_stage/LEO
PYREF    ?= tools/is_decode.py

SRC      := src/main.c src/is_stream.c src/platform_sdl/is_player.c \
            src/llrs.c src/ll_pcx.c src/ll_menu.c src/ll_level.c
HDRS     := src/ll_config.h src/is_stream.h src/platform_sdl/is_player.h \
            src/llrs.h src/ll_pcx.h src/ll_menu.h src/ll_level.h

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
verify-win: ll_cutscene.exe llrs_selftest.exe pcx_selftest.exe
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
	env -u DISPLAY WINEPREFIX=$(WINEPREFIX) WINEDEBUG=-all \
	  python3 tools/verify_pcx.py $(DATA) ./llrs_selftest.exe \
	  "wine ./pcx_selftest.exe"
	@for F in ANIMACJE ETAPY SOUND LEVELDES; do \
	  if env -u DISPLAY WINEPREFIX=$(WINEPREFIX) WINEDEBUG=-all \
	      wine ./llrs_selftest.exe $(DATA)/$$F 2>/dev/null | tr -d '\r' \
	      | diff - <(python3 tools/llrs_unpack.py list $(DATA)/$$F 2>/dev/null) \
	      >/dev/null; \
	  then echo "win $$F: C == python (exact)"; \
	  else echo "win $$F: MISMATCH"; exit 1; fi; done

# src/llrs.c byte-for-byte agreement with tools/llrs_unpack.py on all four
# containers, checked under ASan/UBSan.
llrs_selftest.exe: tools/llrs_selftest.c src/llrs.c src/llrs.h
	$(XC) -O1 -std=gnu11 -Wall -Wextra -Isrc -o $@ \
	    tools/llrs_selftest.c src/llrs.c

pcx_selftest.exe: tools/pcx_selftest.c src/ll_pcx.c src/ll_pcx.h
	$(XC) -O1 -std=gnu11 -Wall -Wextra -Isrc -o $@ \
	    tools/pcx_selftest.c src/ll_pcx.c

llrs_selftest: tools/llrs_selftest.c src/llrs.c src/llrs.h

# ll_level: M3 viewer - prints a level's .DES wiring, shows its PCX art
# and plays its PCM parallax track from the ETAPY block.
ll_level: tools/ll_level.c src/ll_level.c src/llrs.c src/ll_pcx.c src/ll_config.h
	$(CC) -O2 -std=gnu11 -Wall -Wextra -Isrc $(CFLAGS) -o $@ \
	    tools/ll_level.c src/ll_level.c src/llrs.c src/ll_pcx.c $(LIBS)

level: ll_level
	./ll_level $(DATA) $(LEVEL) $(ARGS)
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

# src/ll_pcx.c byte-for-byte agreement with tools/pcx_dump.py on every PCX
# payload inside ANIMACJE/ETAPY.
pcx_selftest: tools/pcx_selftest.c src/ll_pcx.c src/ll_pcx.h
	$(CC) -O1 -g -std=gnu11 -Wall -Wextra -fsanitize=address,undefined \
	    -Isrc -o $@ tools/pcx_selftest.c src/ll_pcx.c

verify-pcx: pcx_selftest llrs_selftest
	python3 tools/verify_pcx.py $(DATA) ./llrs_selftest ./pcx_selftest

verify-menu: ll_cutscene
	@env -u DISPLAY LEO_DEMO_SKIP_FRAMES=5 LEO_SKIP_USER=1 LEO_MENU_AUTO=1 \
	    LEO_LEVEL_SECS=1 timeout 180 \
	    ./ll_cutscene --demo $(DATA) > /tmp/opencode/menu_chain.log 2>&1; \
	rc=$$?; \
	if [ $$rc -eq 0 ] && grep -q 'menu: LEO_MENU_AUTO' /tmp/opencode/menu_chain.log \
	   && grep -q 'll_level: show' /tmp/opencode/menu_chain.log; \
	then echo "demo chain: cutscene -> menu -> level intro (hooked, ok)"; \
	else echo "demo chain FAILED (rc=$$rc)"; cat /tmp/opencode/menu_chain.log; exit 1; fi

verify-all: verify verify-skip verify-win verify-llrs verify-pcx verify-menu
	@echo "ALL TESTS PASSED (linux + windows + llrs)"

clean:
	rm -f ll_cutscene ll_cutscene.exe llrs_selftest llrs_selftest.exe \
	      pcx_selftest pcx_selftest.exe ll_level *.o *.obj

.PHONY: run win verify verify-skip verify-win verify-llrs verify-pcx \
        verify-menu verify-all level clean
