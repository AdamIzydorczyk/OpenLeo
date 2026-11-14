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
            src/llrs.c src/ll_pcx.c src/ll_menu.c src/ll_level.c src/ll_seq.c \
            src/ll_pcm.c src/ll_mod.c
HDRS     := src/ll_config.h src/is_stream.h src/platform_sdl/is_player.h \
            src/llrs.h src/ll_pcx.h src/ll_menu.h src/ll_level.h src/ll_seq.h \
            src/ll_pcm.h src/ll_mod.h

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
verify-win: ll_cutscene.exe ll_level.exe llrs_selftest.exe pcx_selftest.exe seq_selftest.exe seq_patchdump.exe pcm_selftest.exe
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
	env -u DISPLAY WINEPREFIX=$(WINEPREFIX) WINEDEBUG=-all \
	  python3 tools/verify_seq.py $(DATA) --grammar "wine ./seq_selftest.exe" \
	  --patch "wine ./seq_patchdump.exe"
	@python3 tools/verify_pcm.py $(DATA) "wine ./pcm_selftest.exe" \
	  Z:/tmp/opencode/pcm_verify.tmp \
	  | tr -d '\r' | tail -1 | grep -q '4/4 payloads byte-exact' \
	  && echo "win PCM: C == python (exact)"
	@for F in ANIMACJE ETAPY SOUND LEVELDES; do \
	  if env -u DISPLAY WINEPREFIX=$(WINEPREFIX) WINEDEBUG=-all \
	      wine ./llrs_selftest.exe $(DATA)/$$F 2>/dev/null | tr -d '\r' \
	      | diff - <(python3 tools/llrs_unpack.py list $(DATA)/$$F 2>/dev/null) \
	      >/dev/null; \
	  then echo "win $$F: C == python (exact)"; \
	  else echo "win $$F: MISMATCH"; exit 1; fi; done
	@env -u DISPLAY SDL_VIDEODRIVER=dummy LEO_DEMO_SKIP_FRAMES=3 LEO_SKIP_USER=1 \
	  LEO_MENU_SECS=1 LEO_MENU_PPM=/tmp/opencode/lmenu_t2.ppm LEO_MENU_PPM_TICK=2 \
	  timeout 90 ./ll_cutscene --demo $(DATA) --no-audio >/dev/null 2>&1 || true
	@env -u DISPLAY WINEPREFIX=$(WINEPREFIX) WINEDEBUG=-all SDL_VIDEODRIVER=dummy \
	  LEO_DEMO_SKIP_FRAMES=3 LEO_SKIP_USER=1 LEO_MENU_SECS=1 \
	  LEO_MENU_PPM=Z:/tmp/opencode/wmenu_t2.ppm LEO_MENU_PPM_TICK=2 \
	  timeout 120 wine ./ll_cutscene.exe --demo Z:$(DATA) --no-audio >/dev/null 2>&1 || true
	@env -u DISPLAY WINEPREFIX=$(WINEPREFIX) WINEDEBUG=-all SDL_VIDEODRIVER=dummy \
	  LEO_DEMO_SKIP_FRAMES=3 LEO_SKIP_USER=1 LEO_MENU_SECS=1 \
	  LEO_MENU_PPM=Z:/tmp/opencode/wmenu_t8.ppm LEO_MENU_PPM_TICK=8 \
	  timeout 120 wine ./ll_cutscene.exe --demo Z:$(DATA) --no-audio >/dev/null 2>&1 || true
	@if cmp -s /tmp/opencode/wmenu_t2.ppm /tmp/opencode/wmenu_t8.ppm; \
	then echo "win menu anim FAILED: t2 == t8"; exit 1; \
	else echo "win menu anim: BERLO cycles under wine (t2 != t8, ok)"; fi
	@cmp /tmp/opencode/lmenu_t2.ppm /tmp/opencode/wmenu_t2.ppm \
	  && echo "win menu frame == linux frame (byte-exact, overlays composited)"
	@env -u DISPLAY SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
	  LEO_HD_DIR=/tmp/opencode/hd_files LEO_LEVEL_PPM=/tmp/opencode/lvlv_t2.ppm \
	  LEO_LEVEL_PPM_TICK=2 timeout 60 ./ll_level $(DATA) TUNGA.11 >/dev/null 2>&1 || true
	@env -u DISPLAY WINEPREFIX=$(WINEPREFIX) WINEDEBUG=-all SDL_VIDEODRIVER=dummy \
	  SDL_AUDIODRIVER=dummy LEO_HD_DIR=Z:/tmp/opencode/hd_files \
	  LEO_LEVEL_PPM=Z:/tmp/opencode/wlv_t2.ppm LEO_LEVEL_PPM_TICK=2 \
	  timeout 120 wine ./ll_level.exe Z:$(DATA) TUNGA.11 >/dev/null 2>&1 || true
	@env -u DISPLAY WINEPREFIX=$(WINEPREFIX) WINEDEBUG=-all SDL_VIDEODRIVER=dummy \
	  SDL_AUDIODRIVER=dummy LEO_HD_DIR=Z:/tmp/opencode/hd_files \
	  LEO_LEVEL_PPM=Z:/tmp/opencode/wlv_t8.ppm LEO_LEVEL_PPM_TICK=8 \
	  timeout 120 wine ./ll_level.exe Z:$(DATA) TUNGA.11 >/dev/null 2>&1 || true
	@if cmp -s /tmp/opencode/wlv_t2.ppm /tmp/opencode/wlv_t8.ppm; \
	then echo "win level anim FAILED: tick2 == tick8"; exit 1; \
	else echo "win level anim: platform moves under wine (t2 != t8, ok)"; fi
	@cmp /tmp/opencode/lvlv_t2.ppm /tmp/opencode/wlv_t2.ppm \
	  && echo "win level frame == linux frame (byte-exact scene compositing)"
	@env -u DISPLAY SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
	  LEO_LEVEL_PPM=/tmp/opencode/ran_lv.ppm LEO_LEVEL_PPM_TICK=2 \
	  timeout 60 ./ll_level $(DATA) RAN.01 >/dev/null 2>&1 || true
	@env -u DISPLAY WINEPREFIX=$(WINEPREFIX) WINEDEBUG=-all SDL_VIDEODRIVER=dummy \
	  SDL_AUDIODRIVER=dummy LEO_LEVEL_PPM=Z:/tmp/opencode/ran_wv.ppm \
	  LEO_LEVEL_PPM_TICK=2 timeout 120 wine ./ll_level.exe Z:$(DATA) RAN.01 \
	  >/dev/null 2>&1 || true
	@cmp /tmp/opencode/ran_lv.ppm /tmp/opencode/ran_wv.ppm \
	  && echo "win PCM level canvas == linux (byte-exact parallax view)"

ll_level.exe: tools/ll_level.c src/ll_level.c src/llrs.c src/ll_pcx.c \
            src/ll_pcm.c src/ll_seq.c src/ll_mod.c \
            src/ll_seq.h src/ll_pcm.h src/ll_mod.h src/ll_config.h
	$(XC) $(XCFLAGS) $(XLD) -o $@ tools/ll_level.c src/ll_level.c \
	    src/llrs.c src/ll_pcx.c src/ll_pcm.c src/ll_seq.c src/ll_mod.c $(XLIBS)

# src/ll_pcm.c byte-for-byte agreement with tools/verify_pcm.py on every
# PCM parallax layer payload in ETAPY (byte-RLE plane, stride = w+80).
verify-pcm: pcm_selftest
	python3 tools/verify_pcm.py $(DATA) ./pcm_selftest

# src/llrs.c byte-for-byte agreement with tools/llrs_unpack.py on all four
# containers, checked under ASan/UBSan.
llrs_selftest.exe: tools/llrs_selftest.c src/llrs.c src/llrs.h
	$(XC) -O1 -std=gnu11 -Wall -Wextra -Isrc -o $@ \
	    tools/llrs_selftest.c src/llrs.c

pcx_selftest.exe: tools/pcx_selftest.c src/ll_pcx.c src/ll_pcx.h
	$(XC) -O1 -std=gnu11 -Wall -Wextra -Isrc -o $@ \
	    tools/pcx_selftest.c src/ll_pcx.c

seq_selftest.exe: tools/seq_selftest.c src/ll_seq.c src/ll_seq.h
	$(XC) -O1 -std=gnu11 -Wall -Wextra -Isrc -o $@ \
	    tools/seq_selftest.c src/ll_seq.c

seq_patchdump.exe: tools/seq_patchdump.c src/ll_seq.c src/ll_seq.h
	$(XC) -O1 -std=gnu11 -Wall -Wextra -Isrc -o $@ \
	    tools/seq_patchdump.c src/ll_seq.c

llrs_selftest: tools/llrs_selftest.c src/llrs.c src/llrs.h
	$(CC) -O1 -g -std=gnu11 -Wall -Wextra -fsanitize=address,undefined \
	    -Isrc -o $@ tools/llrs_selftest.c src/llrs.c

pcm_selftest: tools/pcm_selftest.c src/ll_pcm.c src/ll_pcm.h
	$(CC) -O1 -g -std=gnu11 -Wall -Wextra -fsanitize=address,undefined \
	    -Isrc -o $@ tools/pcm_selftest.c src/ll_pcm.c

pcm_selftest.exe: tools/pcm_selftest.c src/ll_pcm.c src/ll_pcm.h
	$(XC) -O1 -std=gnu11 -Wall -Wextra -Isrc -o $@ \
	    tools/pcm_selftest.c src/ll_pcm.c

# ll_level: level-scene viewer - prints a level's .DES wiring, composites
# its tlo= PCX background + ANIMACJE leaf .SEQ object sprites driven by the
# HD-install .MOV state machines (LEO_HD_DIR=<dir with ANIMACJE/ETAPY>).
ll_level: tools/ll_level.c src/ll_level.c src/llrs.c src/ll_pcx.c src/ll_pcm.c \
        src/ll_seq.c src/ll_mod.c \
        src/ll_seq.h src/ll_pcm.h src/ll_mod.h src/ll_config.h
	$(CC) -O2 -std=gnu11 -Wall -Wextra -Isrc $(CFLAGS) -o $@ \
	    tools/ll_level.c src/ll_level.c src/llrs.c src/ll_pcx.c src/ll_pcm.c \
	    src/ll_seq.c src/ll_mod.c $(LIBS)

level: ll_level
	./ll_level $(DATA) $(LEVEL) $(ARGS)

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

# src/ll_seq.c byte-for-byte agreement with tools/seq_parse.py on every
# .SEQ payload in ANIMACJE/ETAPY/LEVELDES (172 blobs, exact consumption),
# plus the frame-patch row decode (ll_seq_patch_walk vs tools/seq_patch.py).
seq_selftest: tools/seq_selftest.c src/ll_seq.c src/ll_seq.h
	$(CC) -O1 -g -std=gnu11 -Wall -Wextra -fsanitize=address,undefined \
	    -Isrc -o $@ tools/seq_selftest.c src/ll_seq.c

seq_patchdump: tools/seq_patchdump.c src/ll_seq.c src/ll_seq.h
	$(CC) -O1 -g -std=gnu11 -Wall -Wextra -fsanitize=address,undefined \
	    -Isrc -o $@ tools/seq_patchdump.c src/ll_seq.c

verify-seq: seq_selftest seq_patchdump
	python3 tools/verify_seq.py $(DATA) --grammar ./seq_selftest \
	  --patch ./seq_patchdump

# Level scene: TUNGA.11 = PLAN3.PCX bg + MOVEPLAT leaf .SEQ sprite animated
# by the HD-install ED000002.MOV state machine (LEO_HD_DIR). Fixed-tick PPM
# dumps must differ (motion) and must be composited over the bare background.
# LEO_HELP_SECS=0 skips the help overlay for the pure-scene dumps.
verify-level: ll_level
	@for T in 2 8; do env -u DISPLAY SDL_VIDEODRIVER=dummy \
	  SDL_AUDIODRIVER=dummy LEO_HD_DIR=/tmp/opencode/hd_files \
	  LEO_HELP_SECS=0 LEO_LEVEL_SECS=2 \
	  LEO_LEVEL_PPM=/tmp/opencode/lv_tick$$T.ppm LEO_LEVEL_PPM_TICK=$$T \
	  ./ll_level $(DATA) TUNGA.11 > /tmp/opencode/lv_tick$$T.log 2>&1; \
	  rc=$$?; if [ $$rc -ne 0 ]; then echo "level scene FAILED (rc=$$rc)"; \
	    cat /tmp/opencode/lv_tick$$T.log; exit 1; fi; done
	@grep -q 'show TUNGA.11: bg PLAN3.PCX 640x359, 1 animated' /tmp/opencode/lv_tick8.log \
	  && grep -q 'obj MOVEPLAT/ED000002.SEQ.*\[mov\]' /tmp/opencode/lv_tick8.log \
	  && grep -q 'tick 8: ED000002.SEQ state 1 at' /tmp/opencode/lv_tick8.log \
	  && echo "level scene: wiring + .SEQ sprite + .MOV state machine (ok)" \
	  || { echo "level scene grep FAILED"; cat /tmp/opencode/lv_tick8.log; exit 1; }
	@grep -q 'placement ANI3.DES' /tmp/opencode/lv_tick8.log \
	  && grep -q 'ANIBACK/strum1.mov not on these containers' /tmp/opencode/lv_tick8.log \
	  && echo "placement join: animacje= -> ANI3.DES -> ANIMACJE leaf grammar (ok; strum art absent on this CD)" \
	  || { echo "placement join FAILED"; cat /tmp/opencode/lv_tick8.log; exit 1; }
	@if cmp -s /tmp/opencode/lv_tick2.ppm /tmp/opencode/lv_tick8.ppm; \
	then echo "level anim FAILED: tick2 == tick8"; exit 1; \
	else echo "level anim: platform moves on screen (tick2 != tick8, ok)"; fi
	@env -u DISPLAY SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
	  LEO_HD_DIR=/tmp/opencode/hd_files LEO_HELP_SECS=0 LEO_LEVEL_SECS=2 \
	  LEO_LEVEL_PPM=/tmp/opencode/lv_ran.ppm LEO_LEVEL_PPM_TICK=2 \
	  ./ll_level $(DATA) RAN.01 > /tmp/opencode/lv_ran.log 2>&1; rc=$$?; \
	  if [ $$rc -ne 0 ]; then echo "PCM level scene FAILED (rc=$$rc)"; \
	    cat /tmp/opencode/lv_ran.log; exit 1; fi
	@grep -q 'PCM PLAN1.PCM.*-> scene canvas' /tmp/opencode/lv_ran.log \
	  && grep -q 'show RAN.01: bg PLAN1.PCM 400x200' /tmp/opencode/lv_ran.log \
	  && python3 tools/extract_record.py $(DATA)/ETAPY RAN.01 PLAN1.PCM \
	      /tmp/opencode/ran_plan1.pcm \
	  && h=$$(./pcm_selftest /tmp/opencode/ran_plan1.pcm PLAN1.PCM \
	      2>/dev/null | grep -o 'view.*fnv=[0-9a-f]*' | grep -o 'fnv=.*') \
	  && grep -q "PCM view PLAN1.PCM 400x40 $$h" /tmp/opencode/lv_ran.log \
	  && echo "PCM canvas: tlo stack picks PLAN1.PCM; scene view == ll_pcm oracle ($$h)" \
	  || { echo "PCM canvas check FAILED"; cat /tmp/opencode/lv_ran.log; exit 1; }

# Level help overlay (POMOC flow): HD .MNU page list + CD ETAPY .SEQ page
# art. JUNCTION carries page art AND the whole scene is one .PCX, so the
# help frame dump must be the bare bg plus exactly the page-0 window
# (element anchor (-f0,-f2)), with a pixel count matching the python oracle.
verify-help: ll_level llrs_selftest
	@env -u DISPLAY SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
	  LEO_HD_DIR=/tmp/opencode/hd_files LEO_HELP_SECS=0.5 LEO_LEVEL_SECS=0.5 \
	  timeout 120 ./ll_level $(DATA) TUNGA.11 \
	  > /tmp/opencode/help_tunga.log 2>&1; \
	if grep -q 'help: POMOC1.MNU page 0 pomoc00.seq at (99,3)' /tmp/opencode/help_tunga.log \
	   && grep -q 'page 1 POMOC32.SEQ at (43,44)' /tmp/opencode/help_tunga.log; \
	then echo "help wiring: .MNU static= pages + CD POMOC<NN>.SEQ join (ok)"; \
	else echo "help wiring FAILED"; cat /tmp/opencode/help_tunga.log; exit 1; fi
	@env -u DISPLAY SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
	  LEO_HD_DIR=/tmp/opencode/hd_files LEO_HELP_SECS=0.5 LEO_LEVEL_SECS=0.5 \
	  LEO_HELP_PPM=/tmp/opencode/help_junction.ppm timeout 120 \
	  ./ll_level $(DATA) JUNCTION >/dev/null 2>&1 || true
	@env -u DISPLAY SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
	  LEO_HD_DIR=/tmp/opencode/hd_files \
	  LEO_LEVEL_PPM=/tmp/opencode/bare_junction.ppm LEO_LEVEL_PPM_TICK=0 \
	  timeout 120 ./ll_level $(DATA) JUNCTION >/dev/null 2>&1 || true
	python3 tools/verify_help.py /tmp/opencode/help_junction.ppm \
	  /tmp/opencode/bare_junction.ppm $(DATA)/ETAPY JUNCTION POMOC00.SEQ \
	  99 215 3 33

verify-menu: ll_cutscene
	@env -u DISPLAY LEO_DEMO_SKIP_FRAMES=5 LEO_SKIP_USER=1 LEO_MENU_AUTO=1 \
	    LEO_LEVEL_SECS=1 timeout 180 \
	    ./ll_cutscene --demo $(DATA) > /tmp/opencode/menu_chain.log 2>&1; \
	rc=$$?; \
	if [ $$rc -eq 0 ] && grep -q 'menu: LEO_MENU_AUTO' /tmp/opencode/menu_chain.log \
	   && grep -q 'll_level: show' /tmp/opencode/menu_chain.log; \
	then echo "demo chain: cutscene -> menu -> level intro (hooked, ok)"; \
	else echo "demo chain FAILED (rc=$$rc)"; cat /tmp/opencode/menu_chain.log; exit 1; fi
	@env -u DISPLAY LEO_DEMO_SKIP_FRAMES=3 LEO_SKIP_USER=1 LEO_MENU_SECS=2 \
	    timeout 120 ./ll_cutscene --demo $(DATA) > /tmp/opencode/menu_bg.log 2>&1; \
	rc=$$?; \
	if [ $$rc -eq 0 ] && grep -q 'background OPTIONS.PCX 320x200 loaded' /tmp/opencode/menu_bg.log \
	   && grep -q 'menu: anim 2: 30 frames at (130,38)' /tmp/opencode/menu_bg.log; \
	then echo "menu render: OPTIONS.PCX background + 3 .SEQ anims attached (hooked, ok)"; \
	else echo "menu render FAILED (rc=$$rc)"; cat /tmp/opencode/menu_bg.log; exit 1; fi
	@env -u DISPLAY LEO_DEMO_SKIP_FRAMES=3 LEO_SKIP_USER=1 LEO_MENU_SECS=1 \
	    LEO_MENU_PPM=/tmp/opencode/menu_t2.ppm LEO_MENU_PPM_TICK=2 \
	    timeout 60 ./ll_cutscene --demo $(DATA) >/dev/null 2>&1 || true
	@env -u DISPLAY LEO_DEMO_SKIP_FRAMES=3 LEO_SKIP_USER=1 LEO_MENU_SECS=1 \
	    LEO_MENU_PPM=/tmp/opencode/menu_t8.ppm LEO_MENU_PPM_TICK=8 \
	    timeout 60 ./ll_cutscene --demo $(DATA) >/dev/null 2>&1 || true
	@if cmp -s /tmp/opencode/menu_t2.ppm /tmp/opencode/menu_t8.ppm; \
	then echo "menu anim FAILED: frames t2 == t8 (nothing animates)"; exit 1; \
	else echo "menu anim: BERLO frame cycles on screen (t2 != t8, ok)"; fi

# MOD music: src/ll_mod.c parses the Longsoft repack layout (ll_mod.h)
# and renders deterministically; linux vs wine hash parity is the oracle.
# Payloads come straight out of the SOUND llrs container.
mod_selftest: tools/mod_selftest.c src/ll_mod.c src/ll_mod.h
	$(CC) -O1 -g -std=gnu11 -Wall -Wextra -fsanitize=address,undefined \
	    -Isrc -o $@ tools/mod_selftest.c src/ll_mod.c

mod_selftest.exe: tools/mod_selftest.c src/ll_mod.c src/ll_mod.h
	$(XC) -O1 -std=gnu11 -Wall -Wextra -Isrc -o $@ \
	    tools/mod_selftest.c src/ll_mod.c

verify-mod: mod_selftest mod_selftest.exe
	@for M in TUNGA RAN ENA2; do \
	  python3 tools/pull_record.py $(DATA)/SOUND $$M.MOD /tmp/opencode/$$M.mod && \
	  ./mod_selftest /tmp/opencode/$$M.mod 3000 > /tmp/opencode/mod_$${M}_l.txt && \
	  WINEPREFIX=$(WINEPREFIX) WINEDEBUG=-all \
	    wine ./mod_selftest.exe Z:/tmp/opencode/$$M.mod 3000 \
	    | tr -d '\r' > /tmp/opencode/mod_$${M}_w.txt && \
	  diff /tmp/opencode/mod_$${M}_l.txt /tmp/opencode/mod_$${M}_w.txt >/dev/null && \
	  grep -q 'render 3000ms.*fnv=[0-9a-f]\{16\} peak=[1-9]' /tmp/opencode/mod_$${M}_l.txt && \
	  echo "MOD $$M: parse+render linux == wine ($$(grep -o 'fnv=[0-9a-f]*' /tmp/opencode/mod_$${M}_l.txt))" || \
	  { echo "MOD $$M FAILED"; cat /tmp/opencode/mod_$${M}_l.txt /tmp/opencode/mod_$${M}_w.txt 2>/dev/null; exit 1; }; \
	done

verify-sel: ll_cutscene
	@env -u DISPLAY LEO_DEMO_SKIP_FRAMES=5 LEO_SKIP_USER=1 LEO_MENU_AUTO=2 \
	    LEO_LEVELSEL_AUTO=3 LEO_LEVEL_SECS=1 timeout 120 \
	    ./ll_cutscene --demo $(DATA) > /tmp/opencode/sel_chain.log 2>&1; \
	rc=$$?; \
	if [ $$rc -eq 0 ] && grep -q 'menu: LEO_MENU_AUTO -> picked item 1' /tmp/opencode/sel_chain.log \
	   && grep -q 'll_music: playing MAPKA.MOD' /tmp/opencode/sel_chain.log \
	   && grep -q 'LEO_LEVELSEL_AUTO -> picked level 2' /tmp/opencode/sel_chain.log \
	   && grep -q 'TUNGA2.DES wiring' /tmp/opencode/sel_chain.log; \
	then echo "level select: MAPKA submenu + mapka.mod + TUNGA2.12 open (hooked, ok)"; \
	else echo "level select FAILED (rc=$$rc)"; cat /tmp/opencode/sel_chain.log; exit 1; fi

verify-all: verify verify-skip verify-win verify-llrs verify-pcx verify-seq verify-pcm verify-mod verify-level verify-help verify-menu verify-sel
	@echo "ALL TESTS PASSED (linux + windows + llrs)"

clean:
	rm -f ll_cutscene ll_cutscene.exe llrs_selftest llrs_selftest.exe \
	      pcx_selftest pcx_selftest.exe seq_selftest seq_selftest.exe \
	      seq_patchdump seq_patchdump.exe pcm_selftest pcm_selftest.exe \
	      mod_selftest mod_selftest.exe ll_level ll_level.exe *.o *.obj

.PHONY: run win verify verify-skip verify-win verify-llrs verify-pcx \
        verify-seq verify-pcm verify-mod verify-level verify-help verify-menu \
        verify-sel verify-all level clean
