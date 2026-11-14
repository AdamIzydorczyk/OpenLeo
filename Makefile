# Lew Leon (1997): engine (original/) + native port (ported/) - one shared
# codebase, two native targets:
#   make          -> ll_cutscene       (Linux ELF, SDL2 via pkg-config)
#   make win      -> ll_cutscene.exe   (Windows x86-64, cross-built with
#                                       MinGW-w64 or a vendored llvm-mingw)
# Same sources, one SDL2 layer; only compiler flags/libs differ.
SHELL    := /bin/bash
DATA     ?= data      # game data dir: bundled in data/, or any LEO copy

# original/ = engine sources (format codecs, menu, level, saves)
# ported/   = native port (entry point + the one SDL2 platform layer)
SRC      := original/is_stream.c original/llrs.c original/ll_pcx.c \
            original/ll_menu.c original/ll_level.c original/ll_seq.c \
            original/ll_pcm.c original/ll_mod.c original/ll_save.c \
            ported/main.c ported/platform_sdl/is_player.c
HDRS     := original/ll_config.h original/is_stream.h original/llrs.h \
            original/ll_pcx.h original/ll_menu.h original/ll_level.h \
            original/ll_seq.h original/ll_pcm.h original/ll_mod.h \
            original/ll_save.h ported/platform_sdl/is_player.h

# ---------------- native Linux ----------------
CC       ?= gcc
CFLAGS   ?= -O2 -std=gnu11 -Wall -Wextra -DPLATFORM_LINUX=1 \
            $(shell pkg-config --cflags sdl2)
LIBS     := $(shell pkg-config --libs sdl2) -lm

ll_cutscene: $(SRC) $(HDRS)
	$(CC) $(CFLAGS) -Ioriginal -Iported -o $@ $(SRC) $(LIBS)

run: ll_cutscene
	./ll_cutscene --demo $(DATA)

# ---------------- Windows 11 x86-64 (cross) ----------------
# Toolchain (optional, kept out of this repo): system x86_64-w64-mingw32-gcc
# (pacman -S mingw-w64-toolchain / apt -S gcc-mingw-w64-x86-64), or a
# vendored llvm-mingw + SDL2 MinGW tree under vendor/ (gitignored).
XC        ?= $(shell command -v x86_64-w64-mingw32-gcc 2>/dev/null || \
                    echo vendor/llvm-mingw/bin/x86_64-w64-mingw32-clang)
SDL_WIN   ?= vendor/SDL2-2.32.10/x86_64-w64-mingw32
XCFLAGS   := -O2 -std=gnu11 -Wall -Wextra -DPLATFORM_WIN=1 -Dmain=SDL_main \
             -Ioriginal -Iported -I$(SDL_WIN)/include
XLD       := -static -static-libgcc -Wl,--dynamicbase -Wl,--nxcompat \
             -Wl,--high-entropy-va
XLIBS     := -L$(SDL_WIN)/lib -lmingw32 -lSDL2main -lSDL2 -mwindows \
             -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm \
             -limm32 -lole32 -loleaut32 -lshell32 -lsetupapi -lversion -luuid

win: ll_cutscene.exe
ll_cutscene.exe: $(SRC) $(HDRS)
	$(XC) $(XCFLAGS) $(XLD) -o $@ $(SRC) $(XLIBS)

clean:
	rm -f ll_cutscene ll_cutscene.exe *.o *.obj