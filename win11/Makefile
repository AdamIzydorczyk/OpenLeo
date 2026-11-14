# win11/ variant: cross-build from Linux with MinGW-w64 (x86_64-w64-mingw32).
# Same shared sources as linux/ (../src + the thin ../linux/main.c).
# Self-contained static .exe: no SDL2.dll at runtime.
# Toolchain (run once): sudo pacman -S --needed mingw-w64-toolchain
XC       ?= x86_64-w64-mingw32-gcc
SDL      ?= ../vendor/SDL2-2.32.10/x86_64-w64-mingw32
INC      ?= ../vendor/SDL2-2.32.10/include
CFLAGS   ?= -O2 -std=gnu11 -Wall -Wextra -DPLATFORM_WIN=1 -Dmain=SDL_main \
            -I../src -I$(INC)
SRC      := ../linux/main.c ../src/is_stream.c ../src/platform_sdl/is_player.c
LDFLAGS  := -static -static-libgcc -Wl,--dynamicbase -Wl,--nxcompat \
            -Wl,--high-entropy-va
LIBS     := -L$(SDL)/lib -lmingw32 -lSDL2main -lSDL2 -mwindows \
            -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm \
            -limm32 -lole32 -loleaut32 -lshell32 -lsetupapi -lversion -luuid

ll_cutscene.exe: $(SRC) ../src/is_stream.h ../src/ll_config.h \
                 ../src/platform_sdl/is_player.h
	$(XC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC) $(LIBS)

# copy to ADAM-HANDHELD with the .IS data dir, run:
#   ll_cutscene.exe INTRO_P.IS
clean:
	rm -f ll_cutscene.exe *.obj

.PHONY: clean
