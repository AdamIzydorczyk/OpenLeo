# linux/ variant: native SDL2 build from shared ../src.
CC       ?= gcc
CFLAGS   ?= -O2 -std=gnu11 -Wall -Wextra -DPLATFORM_LINUX=1 \
            $(shell pkg-config --cflags sdl2)
LIBS     := $(shell pkg-config --libs sdl2) -lm
SRC      := main.c ../src/is_stream.c ../src/platform_sdl/is_player.c
HDRS     := ../src/ll_config.h ../src/is_stream.h \
            ../src/platform_sdl/is_player.h

ll_cutscene: $(SRC) $(HDRS)
	$(CC) $(CFLAGS) -I../src -o $@ $(SRC) $(LIBS)

# data dir = dir holding the user's original *.IS files (never in git)
DATA ?= /tmp/opencode/iso_stage/LEO
run: ll_cutscene
	./ll_cutscene $(DATA)/INTRO_P.IS

verify: ll_cutscene
	./ll_cutscene $(DATA)/INTRO_P.IS --ppm 1020 /tmp/f_c.ppm
	python3 ../tools/is_decode.py frames $(DATA)/INTRO_P.IS /tmp/f_py --ppm \
	    --only 1020
	cmp /tmp/f_c.ppm /tmp/f_py/f01020.ppm && echo "C == python reference (byte-exact)"

clean:
	rm -f ll_cutscene *.o

.PHONY: run verify clean
