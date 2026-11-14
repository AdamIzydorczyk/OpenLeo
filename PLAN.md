# Lew Leon (Longsoft 1997) — decompile → rebuild plan

## 0. What we have

| File | What it is |
|---|---|
| `LEO.EXE` (840 KB) | The game engine. **Watcom C/C++11 32-bit, LE/LX linear exe (magic `LE`, CPU=386, DOS/4GW), statically linked CRT, NOT packed, debug info stripped.** obj1=code 0x10000..0x82b51 (~470 KB), obj2=data 0x90000..0xd76d0. Entry `0x74cec`. Resident name `leo`. |
| `SETUP.EXE` | Same toolchain/config UI. code 0x10000..0x315ad. |
| `CONFIG.EXE` (RATUNKU) | 16-bit Watcom C utility (post-install config writer). |
| `DOS4GW.EXE` | DOS/4G (Watcom v7.5) extender, 1996. |
| `ANIMACJE`, `ETAPY`, `SOUND`, `LEVELDES`, `*.IS` | game content in `llrs` container + `NFOSTREAM` cutscenes |
| `LONGSOFT.CFG`, `STATUS.LEO` | binary settings + savegame (STATUS.LEO = packed game-state snapshot with 0x77XXXXXX-coded pointers) |

Game = **"Lew Leon" (LEO), Longsoft/Leryx 1996-97**, Polish side-scrolling action game. Engine is data-driven:
`ANIMACJE` = animation/sequence definitions (strings seen: `TURN_LEW, A_ATAK, A_RUN_UP, ATACK_NEAR, WALKINTOIGLO, OPCJE.MNU, OPTIONS.PAL`),
`ETAPY` = level scripts (`KADMAR.07, TUNGA2.12, JUNCTION, CHEATS` + `.seq` references), `SOUND` = 500 entries (`*.WAV`, `*.MOD` -> tracker music),
`*.IS` = `NFOSTREAM` full-motion video ("Lew Leon" (c) 1996 Longsoft, custom codec, 431 MB `LEVELDES` = level design data on CD).
Error strings found in EXE: Watcom runtime + custom: `Blad zakresu w ...::check BOOL`, `Program needs XMS memory!`, `Blad Programu. Program wykryl zmiany w swoim kodzie...` (self-checksum, see §6).

## 1. Toolchain status (all under ~/tools, no root)

- **Ghidra 12.2** `~/tools/ghidra_12.2_PUBLIC` + Temurin **JDK 25** `~/tools/jdk/jdk-25.0.5+8` (required, set `JAVA_HOME`).
  - Ghidra 12.2 has **no LE loader** -> import the ELF produced by `tools/le_dump.py` (loads as x86:LE:32, base/segments/entry preserved).
- **Open Watcom V2** (2026-11-01 build) `~/tools/watcom-20261101` (+env `owenv.sh`). NOTE:
  - the `Current-build`/`Last-CI-build` snapshot has a **broken `-bt` parser** (driver crashes); use `2026-11-01-Build` snapshot tar.
  - use `-bt=dos` (equals form; `-bt dos` misparses in these builds);
  - `wcl386` driver defaults to Linux link even with `-bt=dos` -> **drive it manually**:
    ```sh
    source ~/tools/watcom/owenv.sh
    WATCOM=$HOME/tools/watcom-20261101; export WATCOM; PATH=$WATCOM/binl64:$PATH
    wpp386 -bt=dos -I=$WATCOM/h file.cpp -fo=file.obj
    wlink f file.obj name game.exe system dos4g op quiet \
          libpath $WATCOM/lib386/dos library clib3r
    ```
    -> produces an **LE/DOS4GW exe identical in kind to LEO.EXE** (verified: `file` says "MS-DOS executable, LE", parses with our own le_dump). C++ front-end: `wpp386`, libs: `clib3r` (+`math387r`), stub needs `DOS4GW.EXE` beside/on PATH.
- **dosbox 0.74-3** (installed) — works headless for automation; see §6 for the baseline blocker.
- optional (needs sudo, user runs): `sudo pacman -S radare2 r2ghidra rizin` for scriptable second opinion.

## 2. Decompile pipeline (working now)

1. `python3 tools/le_dump.py LEO.EXE build/leo/leo`
   -> `leo.img` (flat 0x10000 image, **all 27k LE fixups applied**), `leo.elf` (Ghidra-able), `leo.json` (stats).
2. Ghidra headless (running): `analyzeHeadless build/ghidra LEOPROJ -import build/leo/leo.elf`.
3. Name/structure pass guided by the string pool (`tools/strings_xrefs.py` — TODO):
   Watcom C++ near-code calls = rel32/OFF32 fixups (already resolved); the string pool identifies
   class names (`TExtSeqence`, `WALK::check`, `TURN::check`...), file loaders (`%i,%i`, `%[^,]` sscanf formats),
   subsystem init errors -> anchor functions without symbols.
4. Iterative: decompile module -> rewrite in C++ under `src/` -> compile w/ Open Watcom -> diff behavior in dosbox.

## 3. Data-format reversing (parallel track, mostly offline from the binaries)

- `llrs` container (ANIMACJE/ETAPY/SOUND/LEVELDES) — **CRACKED 2026-11-14**,
  reader `tools/llrs_unpack.py` (list/extract; 269/291 SOUND WAVs verify with
  exact RIFF sizes). Layout (LE): `llrs` magic, u32 ver=1, u32 capacity(slot
  count), u32 0, u32 actual at 0x10; table of `capacity` x 26-byte slots at
  0x14: `char name[14]` (NUL-padded), u32 id (1=data, 2=block/text, 0=free),
  u32 dataOff, u32 size; data region starts exactly at 0x14+26*capacity
  (SOUND 0x32dc, ETAPY 0x1464, LEVELDES 0x19b9b194 — all verified). Unused
  slots hold zeros OR deterministic archiver padding (identical junk bytes at
  same slot indices across archives — NOT encryption, no key needed).
  Contents: SOUND=269 WAV (mono 8-bit mostly, 6-22 kHz) + 22 MOD; ANIMACJE /
  ETAPY=14 each, all blocks size 0x1450 (5200B) named after level `dir=`
  values (RAN.01, KADMAR.07...); LEVELDES=13 `*.DES` level descriptors +
  ENG/GER/POL/POL.ORG language override tables (same slot format inside).
  Each 5200B ETAPY/ANIMACJE block is itself a nested mini-llrs: u32 count +
  26B slots + data, holding `*.SEQ` scripts, `*.PCX/PCM` layers, `*.SUF`,
  `*.PAL` etc. (33 entries in ZARIA.01).
  `.DES` = INI-style plain text, Polish keys: dir, intro(x.pcx), pozycja,
  tlo(layer list), muzyka(x.mod), sample, lew(class,actor,anim,des), main_back,
  back_rect, lew_rect, sufity, speed, plany(T|TA|TALAB), platformy(.plt),
  blokady(.blk), paleta(.pal), zwierzaki, animacje, ani_plat, belka. This is
  the level-schema for the rebuild (M4).
- `.IS` NFOSTREAM container — **CRACKED 2026-11-14** (static walk reaches EOF
  byte-exact on all 15 files + dynamic verify via hbreak trace of ISplay's
  chunk-record fn vaddr 0x1b7b0): 0x5f-byte header (text block; fixed
  `0a 73 19 00 01` @0x51; u24@0x56; u16 w@0x59; u16 h@0x5b; u8 fps@0x5d),
  then a contiguous chunk chain to EOF: `[u8 type=0][u32 len incl hdr][u16
  tail]` + payload. ISplay cache table (DAT 0xc41d4, 10000-slot ring,
  rd@0xcde18 wr@0xcde1c) records each chunk's PAYLOAD ptr 1:1 in file order
  (verified: table[rd..wr] -> file chunk_start+7, delta 0). Video chunk
  payloads start `b0 4e 0c 00 00` + i16 delta streams; chunk0 is PCM audio;
  tail=1/len=777 chunks are still-frames. Codec (v2.01 BETA, Paweł
  Leśniowski) = decompression fns vaddr ~0x1a000-0x1e000 — next task.
  Tool: `tools/is_unpack.py` (list/extract). Attract loop plays INTRO_P.IS
  on repeat: 2388 chunks per pass.
 - `.IS` codec v2.01 — **CRACKED + LIVE-VERIFIED 2026-11-14**, decoder
   `tools/is_decode.py`. Full spec in that file's header; highlights:
   * Chunk payloads carry a continuous per-movie opcode stream (control ops
     interleaved with whole frames); framebuffer+palette persist across
     chunks. Each payload starts with an ESC(0xb0) skip of the audio tail:
     `resume = payload_pos + 5 + u32` (0 crossings of payload end in all
     2388 INTRO_P payloads). ESC-skipped bytes = 8-bit unsigned PCM ~8kHz.
   * Band opcode stream, 4 segments per band (bands = h/4): opcode header
     `B`: len=B&7 (|2nd byte if B&0x40), mode=B&0xb8: SOLID 0x00, LUT1 0x08,
     LUT2 0x10, LUT4 0x18, RAW 0x20, SKIP 0x90, EOB 0x88 (ends band early;
     frame band-counter advances by len). Budget per band = width px.
   * Control ops: ESC 0xb0; RPT8 0x98 / RPT16 0xa8 set [c410c]=n-1,
     [cde2c]=1 — a player/decoder handshake, NOT read by the band drivers,
     so they do not duplicate decoded frames; PAL 0xa0 = 768B straight to
     DAC 3c8/3c9 (all 256 entries); END 0xb8 sits ~1 byte before payload
     end (remaining tail bytes are not opcodes).
   * Grammar **byte-exact** vs live codec: 137,165/137,165 traced leaf
     events whose cache window is file-confirmed decode to the logged
     (mode,len) — 97,525 1-byte + 39,640 2-byte headers, zero header-bad.
   * Screen end-to-end confirmed: idle freeze (bandctr==h/4, budget==0,
     mid-movie) -> QEMU screendump vs last decoded frame = **99.05%** exact
     at rd=1020 (frame 704); at rd=1956 (frame 1263) content is correct
     (top rows exact; bottom diff is pure DAC rounding — live DAC ~= my
     6-bit table `v<<2` with occasional +3 per entry, consistent per entry).
   * All 15 .IS decode to EOF, zero desync (INTRO_P: 2388 chunks -> 1521
     frames, 189 PAL ops, 2363 ESC ops).
- `STATUS.LEO`: 300 bytes, values `0x77XXXXXX` = tagged/delta pointers into DSEG; reverse against save/load functions in EXE.
- `LONGSOFT.CFG`: 273 bytes, mostly zero; write path is in SETUP/CONFIG.EXE (16-bit) — parse with TDR/RATUNKU CONFIG.EXE strings.

## 4. Engine subsystems to reproduce (from strings + data)

1. llrs archive reader (used for everything).
2. VGA/SVGA renderer (VESA likely via DPMI int31 callback; palette ops OPTIONS.PAL).
3. Animation/sequence interpreter ("seq", sequence classes WALK/TURN/ATACK/IDLE/..., `ANIMACJE` tables, "Blad zakresu w %s::check").
4. Level runner (ETAPY + LEVELDES, `*.seq` scripts with sscanf `%i,%i`, `%[^,]`).
5. Audio: Sound Blaster (WAV PCM) + GUS (`ULTRASND`) + **MOD modules** (TUNGADEA.MOD...) — MOD player choice: reimplement with a simple protracker replayer.
6. Input (keyboard + mouse via DPMI), game menu (OPCJE.MNU).
7. Save/load (STATUS.LEO) — keep byte-compatible so the provided saves load.
8. FMV (.IS) player (optional last phase).
9. Anti-tamper self-check (§6) — decide: reproduce or omit in our build.

## 5. Build harness (source project layout)

```
src/           engine sources (C++ with Open Watcom)
tools/         le_dump.py, llrs_probe.py, (todo) strings_xrefs.py, unis.py
build/         ghidra projects, disassembly exports, baseline dumps
data/ -> game content (keep original files, never modified)
Makefile.wc    watcom makefile: wpp386 -bt=dos + wlink system dos4g
run_dosbox.sh  mounts ./dist + LL_CD, runs dist/LEO.EXE in dosbox, screenshots/logs
```

## 6. Baseline status (SOLVED 2026-11-13)

(Previous version of this section described a dosbox-0.74 baseline; it was
built on a wrong premise and has been replaced 2026-11-13 with the verified
QEMU story below.)

**Baseline platform: QEMU, not DOSBox.**
`qemu-system-i386 -m 32 -boot a -fda boot.img -fdb msdos622/dos-622-disk2.img
-hda leo_hd.img -cdrom leo.iso -vga std -display none -monitor unix:...`
- MS-DOS 6.22 (A: boot floppy, HIMEM + UIDE.SYS + UDVD2.SYS dev `UDVD1`,
  `MSCDEX /D:UDVD1` -> CD on D:), HD image with real MBR partition
  (mtools offset `leo_hd.img@@32256`), CD image `leo.iso` (ISO9660 vol "LEO",
  game files under `LEO\` mirroring the real disc layout).
- **Pristine LEO.EXE reproduces the user's original failure here**: it prints
  "Blad Programu. Program wykryl zmiany w swoim kodzie." - and the project's
  original-install artifacts (LEO.LOG / LEOOUT.TXT) show the same message
  fired on the user's real machine. So the "tamper" check is not a checksum:
  it is a CD-presence failure that misreports itself as code-tampering.

**Gate 1 (tamper message) - root cause + minimal workaround.**
`ll_main` (0x5c90a) -> `ll_cdcheck_chain` (0x5bd50) -> `ll_cd_int2f` (0x5bbd0):
INT2F AX=1500 requires returned manager count != 0; for each manager,
AX=150B must return signature `[esp+4]==0xADAD` (UDVD2/UniASPI does: yes)
AND a non-zero CD drive word in AX (`[esp]`) - UDVD2's ASPI shim answers
AX=0 -> `ll_cd_int2f` returns 0 -> chain fails -> ll_main site 0x5c921 shows
message idx2 (the "tamper" string, table `[0xa9a00+((lang-1)*7+idx)*4]`).
Workaround (patch on COPY via `tools/le_patch.py`, never on pristine):
`0x5bd5c: 9090` = NOP the `test eax,eax` after the `call 0x5bbd0`; the rest of
the chain (FUN_0004408/FUN_0004409/stub) passes unmodified. A legit fix would
need a real ASPI layer that reports the CD drive word (UDVD2 does not).

**Gate 2 ("Nie mozna znalezc plyty z gra LEW LEON.") - solved without patches.**
ll_main call site 0x5c9bc: opens FMV stream `koniec.is` via FUN_0007607b ->
FUN_00076042 (mode `rb`) -> FUN_00075f7e; the full path is sprintf'd at
FUN_00047a70 from placeholder string 0x9246d `":\leo\"` + name, where the
drive letter = `DAT_000cf254 + 0x40`. `DAT_000cf254` is set in FUN_0005bf10
from **byte 0 of LONGSOFT.CFG** (ASCII drive letter minus 0x40; the user's
original cfg says 'H' - their CD drive). Two environment fixes, no binary
changes: (a) VM copy of LONGSOFT.CFG byte0 `H`->`D` (originals untouched);
(b) rebuilt `leo.iso` with files under `LEO\` (my first image had them at
root; the real CD, cf. LL.iso, keeps them in a `LEO` dir). Result: gate 2
passes; game opens `D:\leo\intro_p.is` + `D:\leo\SOUND` (heap strings at
0x157f450/0x157fac8/0x157fb58), 7 live FH entries in table 0xa9c00,
QEMU `info blockstats` shows ~48 MB CD reads by t=220 s, and `screendump`
frames show continuous full-screen FMV playback (intro movie).

**Baseline FULLY VERIFIED LIVE (2026-11-13/14, user at the QEMU GUI window).**
The game is playable in the rig: intro FMV plays, Esc opens the menu, Enter
enters actual gameplay with working character control. Patch set to date =
**one 2-byte NOP** (`0x5bd5c:9090`); everything else is install-layout
equivalence. QEMU-monitor `sendkey` does NOT reach the game (use the real
GUI window for input testing; do not trust monitor key injection).

**Attract loop explained:** the title attractor is movie->menu->movie; the
menu auto-timeout back to intro is a game timer whose *perceived* length
scales with engine execution rate, which is why the loop seemed broken at
full TCG speed.

**Speed/pacing model.** The engine replaces the BIOS timer chain (0x0040:006C
stays frozen; game keeps private counters, e.g. heap dword at 0x168d1b4 ticks
~22 Hz while playing) and its animation/loop pacing is execution-rate bound
(busy pacing, no PIT calibration), so on stock TCG it runs wildly fast.
Control levers, best -> worst:
1. `-icount shift=N`: QEMU 11 validates the shift (shift=11 rejected on this
   host; 9 and 10 work). Runtime `icount shift=` changes via monitor wedge the
   monitor - always set at launch and relaunch instead.
2. cgroup v2 user-scope quota (`systemd-run --user --scope` wrapper or move
   QEMU into a scope), `cpu.max = <q> <p>` in us; kernel min quota 1000 us, so
   sub-1% needs long periods (e.g. `1000 800000` = 0.125% of a core). At
   shift=10 QEMU needs only ~7% of a core, so quotas below that DO bite and
   are the smooth way to slow down.
3. SIGSTOP/SIGCONT duty-cycle loops work but stutter unless pulse freq stays
   >=~100 Hz; below ~1% duty they are meaningless (signal overhead) -> use 2.
Working recipe in `/tmp/opencode`: QEMU at `-icount shift=10` inside scope
`run-...scope` with `cpu.max 1000 50000` (=2% of one core).
Exact period-accuracy tuning was explicitly de-prioritized by the user
("not important, important that game works").
NATIVE PORTS (src/, root Makefile): this busy-pacing model must NOT be inherited.
Modern CPUs make it run thousands of times too fast. The recompiled engine
needs a fixed-timestep clock instead, driven from SDL_GetTicks64 + SDL_Delay
(QueryPerformanceCounter optional on win11), present vsync-capped - all game
logic on tick boundaries, so behavior matches the slowed QEMU baseline, not
the wall execution rate. EXACT pacing sources, pinned 2026-11-14:
- cutscenes: .IS header byte 0x5D (fps) is authoritative per-file: measured
  2 fps (LOGO/PRAWA/AUTORZY?) ... 9 fps (ZARIA2); 640x480 files 2-5 fps,
  Mode X 320x200 files 5-9 fps. Player uses 1000/fps ms directly.
- engine tick: game has ZERO clock sync (no PIT port I/O, no INT 1Ah, no
  BIOS 40:6C reads anywhere in decompiled/all) - pure fixed-work-per-frame.
  QEMU docs confirm icount: "one instruction every 2^N ns virtual time",
  so shift=10 = 1024ns/instr ~ 0.977 MIPS = the retail-correct calibration;
  gameplay tick 22 Hz => ~44k instr/tick. To refine 22 Hz exactly: INT21
  marker probe + INT 1Ah AH=0 CX:DX BIOS-tick timestamps under icount are
  fully deterministic and host-speed-independent.

**Method notes.** Probe = 4-byte stub `b0 <code>; cd 21` at a vaddr on
COPY named `LEOP.EXE` (must be 8.3); empty `C:\LEO_OUT.LOG` = game exited at
the stub. Runtime layout deterministic: DOS4GW base 0x15DB000, code linear =
vaddr+0x15CB000, data linear = vaddr+0x15BE000, entry 0x74cec. QEMU monitor:
`memsave <lin> <size> "quoted-path"`, `screendump`, `info blockstats`;
`-d int`/`-d exec` logs are enormous for this game (80 MB/s) - filter or
window them. This model has no vision: frames are analyzed numerically
(color counts, per-pixel diff%) or via a vision subagent.

Notes for the source project (do NOT replicate as code):
* DOS4GW runtime is embedded in LEO.EXE (LE + WATCOM stub); its init at
  0x74d64..0x74f6f probes DOS/DPMI/Win3.x (INT21/30, 'XD'/'CB', INT21/FF00,
  INT31/06) and sets mode globals [0xa9bee..0xa9bf8]; on DOSBox: mode 0.
* Heap = WATCOM: list [0xa9b58], ceiling [0xa9bc0] (from es:[0x5c] of the
  env segment at 0x74db7), stack top [0xa9bd4]; allocator wrapper 0x7521f
  (eax=-1 -> free path), grow paths 0x791d4/0x79284/0x7944d, DPMI alloc
  0x7c0e6 (INT31/0100), max-block query INT21 AX=4800.
* Exception handler installed in main at 0x5c98f (handler 0x5c7b0, strings
  `Second throw!`/`DOS4GW error (2001)`); after that, crashes print
  `Second throw! Bledne zwolnienie zalockowanego bloku` instead of dumps.

Visual confirmation still TODO (headless dosbox: CAPTURE/screenshot needs
GUI keypress; QEMU FreeDOS boot image attempt parked - no local DOS boot
image, several download sources unreachable).

## 7. Milestones

- M0 toolchain ready (done today): le_dump.py ✓, Open Watcom -> DOS4GW EXE ✓, Ghidra analysis of leo.elf ✓/running.
- M1 (1-2 w): Ghidra function pass: name ~all 1-2k functions by anchors; export annotated C pseudo-source (`DecompileHeadless` script).
- M2 (1-2 w): `llrs` reader verified against SOUND entries (extract WAVs byte-exact) + .IS header parse.
- M3 (2-4 w): renderer + input + main loop boot to title/menu in our build (run in dosbox side-by-side).
- M4 (4-8 w): sequence interpreter + one full level playable in our build.
- M5 (8-16 w): all levels, audio (WAV+MOD), saves, menus.
- M6 (later): cutscene codec (.IS) + anti-tamper decision + packaging `dist/`.

### 7.1 Repo layout plan (ONE branch, three variant directories)  [updated 2026-11-14]
Single branch `main`, top-level variant directories instead of branches -
all versions visible at once, shared code single-copy. DECISION 2026-11-14
(user): DOS no-CD interim variant DROPPED - the no-CD requirement is served
by the native ports; the CD-gate chain analysis (S6, probe-verified) feeds
`original/` fidelity and is simply not compiled into native builds.
`.gitignore` MUST exclude original artifacts (LEO.EXE, LL.iso, SETUP.EXE,
DOS4GW.EXE, ANIMACJE, ETAPY, LONGSOFT.CFG, STATUS.LEO, *.IMG/ISO) - only our
source/tools/build scripts are versioned.

```
main/
  PLAN.md
  src/        shared engine C/C++ (single copy; all 3 variants compile it)
  tools/      python RE tooling (llrs/is unpackers, le_dump/le_patch, ...)
  build/      RE artifacts: leo.img, Ghidra decompiled/all (frozen reference,
              regenerate-only, never hand-edited)
  original/   variant 1: faithful DOS build - CD checks intact, same behavior
              as retail (Watcom wpp386 + DOS4GW; Makefile.wc)
  src/ + root Makefile
              variant 2+3 (merged): ONE shared native codebase compiled to two
              targets - `make` -> Linux ELF, `make win` -> Windows x86-64
              .exe (MinGW-w64 cross-build or vendored llvm-mingw; also
              buildable natively on Windows). SDL2 platform layer
              (src/platform_sdl/), libc/Windows startup, no DOS/DPMI, no CD
              (gate code not compiled). Data path from argv. Tests:
              `make verify-all` (byte-exact --ppm + skip-frame on both
              targets under wine + llrs oracle).
```
Native targets share everything (`src/main.c` entry, codec, SDL2 backend);
only the Makefile rule adds per-target flags/libs. Variant knobs
(`WANT_CD_GATE`, `DATA_DIR_FROM_CFG`, `PLATFORM_DOS/LINUX/WIN`) come from
`src/ll_config.h` defaults plus `-D` on the compiler line; engine code in
`src/` is shared and variant-agnostic except behind those flags.

## 8. M3 findings (asset format recon, 2026-11-14)

llrs containers (reader ported+verified: `src/llrs.c`, `make verify-llrs`):
- ETAPY/ANIMACJE level slots (id=2) -> 5200B nested index blocks; id=1
  records = named payloads, id=2 = leaf blocks (5200B sprite data), id=0 =
  padding. The "junk" records are padding only - all real payloads are
  plain (no encryption anywhere in the containers).
  LEAF BLOCKS DECODED 2026-11-14: id=2 records inside ANIMACJE level blocks
  are themselves mini index tables (u32 count + 26B records, offsets stay
  ABSOLUTE in the container at every nesting depth) whose id=1 entries are
  the per-object ANIMATION FRAME .SEQ scripts: leaf key = object name
  (SCHODY stairs, PLANT, CAMEL tank, PTAK bird, TABLICA, PIASEK...), inner
  keys NN.SEQ frame numbers (CAMEL/UJ04.SEQ = 60KB script, 24 elements).
  40 leaf blocks / 131 frame scripts in ANIMACJE; all 131 parse with the
  same .SEQ grammar, byte-exact (verify-seq recurses 3 levels). The
  per-object sprite animation assets were never missing - they are these
  leaf tables; ETAPY blocks carry no id=2 leaves (PCX/PCM/SEQ only).
  NB: wine truncates large piped stdout (~120KB) - batch captures go to a
  file (see tools/verify_seq.py).
- ANIMACJE/MENU block (slot 190): 39 recs, 9 payloads = menu .SEQ scripts
  (LEVEL06/LEVEL08/RESTGRY/AUTORZY/TAK/CNIE/BERLO/X.SEQ) + OPTIONS.PCX.
- ETAPY level blocks carry per-level resources: POMOC00/POMOC32/TAK/NEXT
  .SEQ, intro/plan .PCX (JUNCTION.PCX 320x200, PLAN3.PCX 640x359,
  M15/KARA/PIORUN3...), plus id=2 sprite leaf blocks.
- LEVELDES: .DES level descriptors are PLAIN TEXT key=value
  (dir/intro=x.pcx/pozycja/tlo=planX.pcx,pcm/muzyka/lew/animacje=...).des...);
  POL/ENG/GER/POL.ORG slots are nested index blocks of the per-language
  text copies. This is the level wiring table for gameplay.
- PCX payloads: standard 8bpp RLE but non-standard headers (planes byte
  garbage, no 0x0C marker, palette = last 768B, junk tail). `src/ll_pcx.c`
  verified byte-exact vs `tools/pcx_dump.py` on all 6 container PCXs
  (`make verify-pcx`).
- .SEQ payloads: GRAMMAR DECODED 2026-11-14 (raw disasm of loader
  FUN_00043e70 -> element chain FUN_00041ef0/41f60, leaf readers
  FUN_00041d40/41da0/41e00; every stream read = vtable slot +0x1c,
  count in BYTES). Grammar:
    blob    := u16 count; count x element
    element := u16 w0 (flags)
               u16 b19,b15,b17        (sub-block obj+0x19/0x15/0x17)
               u16 f0,f2,f4,f6        (obj+0x00..0x06; f0 "type" IS
                                       stream-read, ctor 0x7fff is only
                                       a default that gets overwritten)
               [u16 f8,fA]            when w0&4
               [n B palette][n B sprite]  when w0&3, n=int16(b17),
                                       n<=0 reads nothing (game's
                                       negative-ESI loop)
               [for i=0..int16(b19): u16 L; L B]   unless w0&2 && !(w0&1)
  All 172 container .SEQ payloads (8 ANIMACJE/MENU + 33 ETAPY nested +
  131 leaf-block frame scripts; the 49-file recon corpus = 41 + 8
  byte-identical dupes) are consumed EXACTLY (parser end == payload size,
  zero leftover). Ported to
  `src/ll_seq.c`, oracle `tools/seq_parse.py`, CI `make verify-seq`
  (ASan/UBSan, C==python dump diff, also under wine in verify-win).
  Derived object fields (obj+0x0c..0x12 bbox, obj+0x14 = w0&8) are
  recomputed by FUN_000420b0, not stored in the blob. Text token
  strings (BY_X/SOLID/CHECK_*) belong to .mnu text parsing (FUN_00051913),
  not .SEQ.
- STATUS.LEO map pinned from FUN_0005dcc0 (writer) / FUN_0005dae0 (reader):
  75 LE dwords = [0..20] state block A, [21..41] block B (var base +0x54),
  [42..62] block C (DAT_000cf46c, zero in stock save), [63]=DAT_000a94f0<<8,
  [64]=DAT_000cf464<<8, [65..74] plain dwords (this save: 128,200,1,...);
  obfuscation word = (i|0x77072400)^(var|(i&0xffffff00)), i = byte1 index.
  Field semantics need a paired QEMU capture (menu vs mid-level save).

## 8a. M3-A scene flow (delivered 2026-11-14)

`ll_cutscene` now chains scenes and owns the boot flow (the original .mnu
menu scripts are absent from the CD containers, so menu wiring is
project-owned):

- `--demo DATA`: LOGO.IS -> PRAWA.IS -> INTRO_P.IS -> title menu ->
  level intro (NEW GAME = RAN.01 via `ll_level_show`).
- `--then FILE.IS` / `--menu DATA`: manual chaining.
- ESC mid-cutscene: `is_player_run` returns 2 (user skip); the chain
  ADVANCES to the next scene/menu instead of holding a frozen frame -
  matches the original ("esc skips intro, game continues") and the app
  never dies. Natural finish holds the last frame ~2s then advances.
- `src/ll_menu.c`: title menu, built-in 8x8 bitmap font, background =
  original ANIMACJE/MENU/OPTIONS.PCX through the verified llrs+pcx
  readers. `src/ll_level.c`: shared level intro viewer (also behind
  `make level` / tools/ll_level CLI). .DES name mapping: LEVELDES uses
  stem names (RAN.DES) while ETAPY slots use level dirs (RAN.01);
  resolution order exact -> <name>.DES -> <stem>.DES -> dir= from .DES.
- CI: `make verify-menu` runs the whole chain headless through env hooks
  (LEO_DEMO_SKIP_FRAMES / LEO_SKIP_USER / LEO_MENU_AUTO / LEO_LEVEL_SECS)
  and greps the log; part of verify-all (15 checks green with the new
  verify-seq, incl. wine).
- Still open for gameplay: .mnu menu scripts (absent from CD containers -
  SETUP-install output after all, see 8c), .plt/.pal/.blk/.suf (present in
  the HD install tree, see 8c; not on the CD), PCM = parallax IMAGE layer
  (the old "22050 u8 audio" guess was WRONG - removed; geometry decode
  pending, see 8c). .SEQ grammar AND leaf-block sprite recon are DONE;
  ll_seq playback is now wired into the title menu (see 8b); next step
  is .SEQ playback in the level-intro renderer.

## 8b. M4 animation engine (delivered 2026-11-14)

**Frame-patch blob format CRACKED** (tools/seq_patch.py, 750/750 entry
blobs of the 172 container .SEQ payloads decode with exact consumption):

```
blob    := row*
row     := (0x80|len) content[len]    length-prefixed rows; the >=0x80
                                        header doubles as the blitter's
                                        row-END marker (end-of-row =
                                        "next byte >= 0x80") and lets the
                                        clip prologue SKIP whole rows.
content := gap run*
  gap        := byte added to dst x
  op pos     : C < 0x80 copy C bytes | F >= 0x80 fill F&0x7f + value byte
  marker pos (after any run): B < 0x40 gap | 0x40..0x7f after COPY =
  fill (B&0x3f)+value, after FILL = copy B&0x3f bytes
```

Blitter RE (raw disasm build/seq_blitters.txt): FUN_0001dc80 draw
(400-byte pitch, view base = buf + 0x190, 320x200 clip, p5 = rows to
skip), FUN_0001d9b8 erase (copy-only rows, advance 0x190-w),
FUN_0001dda8 = STD-direction variant. Renderer FUN_000426a0: element =
one frame, entries = 80px sprite chunks + b15px tail chunk, x += 80;
screen pos = (-f0, -f2). A Watcom "dirty upper EAX" reading of the gap
add was ruled out empirically (chain variant corrupts the BERLO crown,
clean per-row gap model renders it intact).

C port (src/ll_seq.c): `ll_seq_patch_walk` (row callback = ONE grammar
impl), `ll_seq_patch_draw` painter, `ll_seq_draw_elem` chunk model.
Proof: tools/seq_patchdump.c dumps decoded rows in the exact text format
of seq_patch.py `dump_payload`; verify_seq.py diffs grammar AND patch
rows (`--grammar/--patch`) on all 172 payloads, under ASan/UBSan natively
and under wine. 172/172 byte-exact.

**On-screen payoff**: title menu now composites the original overlays -
main.c `menu_anims()` pulls X.SEQ/CNIE.SEQ/BERLO.SEQ from ANIMACJE/MENU;
ll_menu_open builds an 8bpp index canvas (OPTIONS.PCX idx + sprite
indices via ll_seq_draw_elem at (-f0,-f2)), one palette LUT, frames
cycled from fixed 30ms redraw ticks (`ll_menu_anim.period_ms`).
BERLO = 30-frame swaying crown-sceptre at (130,38), CNIE = 2-frame
shadow (171,146), X = static glyph (155,54). CI: LEO_MENU_PPM[_TICK]
dumps the composed frame; verify-menu proves attach + frame cycling.

## 8c. Level scene + .MOV motion scripts (delivered 2026-11-14)

**SETUP-install tree cracked**: INSTALL.BAT copies `LEO_HDD\*.*` (shipped
in LL_CD, pruned pre-SETUP copies identical to the installed HD tree) then
runs CONFIG+SETUP (SETUP.EXE carries the "leveldes" container-tool strings).
The HD ETAPY/ANIMACJE containers hold the TEXT payloads the CD lacks:
per-level POMOC1.MNU / CHEAT04.MNU, <level>.plt platform polylines, .pal
tables, and sub-.des placement files - read as extra llrs containers,
originals never modified. Grammars (all plain text, CRLF):
  sub-.des  := "count CRLF" then "TYPE,name,mov,X,Y" lines (ANIBACK/IGLO/
               SURPRISE/PILAGEN/BAL/BERLO/KORONA/KONIK/ZONE/KRY/GRUPA/
               SOUNDSOURCE...) + per-object blocks (sample=/effects=/
               count=/id=/prior=/loop= and "x,y;x,y;" point lists);
  .plt      := "N" then "n x,y;x,y;..." platform polylines (world coords,
               back_rect up to 28800x600);
  .MNU      := background=/palette=/cursor=/button=x,y,seq,f/key=.../
               anime=x,y,seq,period/canime=/static=x,y,seq/default=.

**.MOV = OBJECT STATE MACHINE (the missing motion half)**: HD ANIMACJE leaf
records carry `<KEY>.MOV` text where the CD carries `<KEY>.SEQ` art - SAME
record key (ED000002.SEQ art <-> ED000002.MOV script), 1:1 join. Grammar:

```
state-names-list; blank line; then [STATE] blocks:
  control=BY_TIME
  TARGET(n)=ACTION          transitions (viewer cycles states in order)
  seq=edNNNNNN              art .SEQ key the state shows
  <frame>,<period_ms>[, M dx,dy]*   show frame, then move object by delta
```

MOVEPLAT ED000002: A_IDLE/A_MOVE_DN/A_STOP/A_MOVE_UP/A_DEAD, 6x "M 0,2"
down + 6x "M 0,-2" up = the bobbing platform, net zero per cycle.

**PCM is a parallax image layer, NOT audio** (old ll_level audio path
removed). DECODE SOLVED 2026-11-14 (src/ll_pcm.c + tools/verify_pcm.py,
`make verify-pcm`, wine-parity in verify-win): hdr u16 w, u16 win_rows,
u16 16, 2 zero u16s; 40 x u32 monotonic table at 0x06 (chunk
decompressed-SIZE targets: cumulative, base 0 - the ctor reads it but the
row loop never seeks); 7 x 24-entry u32 tables (0x200000-stepped, role
unknown); from 0xa6 ONE continuous byte-RLE stream (same codec as .SEQ:
byte >= 0xC0 -> repeat next byte C&0x3F times, else literal) decodes to
EOF exactly (tiny tail = partial final row); plane = row-major stride
w+80 (FUN_00056bcd: stride = w + 0x50; ctor FUN_00056b20 loads win_rows
rows/pass, blitter FUN_00056c72 maps screen y % win_rows and advances
stride + 0x50 per screen row = 80-px gutter for the per-column parallax
shift FUN_00057460). Layers: RAN 400x1494, ZARIA.01 700x1491, ZARIA.02
700x1714, TUNGA 600x2896 (win 32/40). Vertical adjacency coherence of the
decoded planes is weak (byte-match ~0.07 at row+1 vs 0.014 baseline):
tiled/gutter interpretation still open - pixel display pending, and note
the stock CD hits the w/80 idiv trap at level start anyway, so stock PCM
display was never baselined.

**src/ll_level.c is now a scene composer**: main .DES resolved by exact ->
<name>.DES -> <stem>.DES -> dir= scan of every LEVELDES .DES payload
(TUNGA.11 is carried by TUNGA1.DES); bg = first tlo= .PCX present in the
ETAPY block; objects = ANIMACJE/<level>/<LEAF> .SEQ sprites (ll_seq blitter)
with the HD .MOV driving frame+delta motion (LEO_HD_DIR=<dir with
ANIMACJE/ETAPY>); one script per leaf (prefers the .SEQ the .MOV seq= names,
ED000012-style unreferenced alternates dropped). Anchor heuristic until the
mov-name->ED-key join is known: x=|f0|, tall sprites bottom-aligned (MENU
scripts use negative f0/f2 = exact (-f0,-f2), level leaves carry positive
world-space values). Demo NEW GAME now renders TUNGA.11 (RAN.01 has no
background PCX on the CD - its tlo is plan3.pcx,.pcm,.pcm). CI:
`make verify-level` (fixed-tick PPM dumps tick2 != tick8, state-machine
grep, compositing proven vs bare bg: 1885 painted px, 4018 differing bytes
after the +4px bob).

**`.des` placement join CRACKED (verified grammar)**: the main .DES's
`animacje=` key names the per-level placement files (`ani4.des,ani3.des,
ani2.des,ani1.des,animdemo.des`); the HD level block ships ANI3.DES (ENA.03
also ZWIERZ3.DES) - the rest are data holes on this CD. Line fields:
**TYPE = the ANIMACJE/<level> leaf block name** (ANIBACK/IGLO/MOVEPLAT/...),
**mov stem = the record stem inside that leaf** (`IGLO,xxx,elem04.mov,317,36`
-> ANIMACJE/LONA.05/IGLO/ELEM04.SEQ - same-container join, no hash),
**X,Y = world coords** (world = back_rect, 2560x600 on TUNGA; `ANIBACK,
dumny,strum2.mov,1494,503` is off the 640-px view - no scroll camera yet,
so off-view lines are logged, not drawn). `src/ll_level.c des_placement()`
applies every resolvable line (retargets the fallback object of the same
stem, else instantiates another copy from the leaf with the HD .MOV
attached); unresolvable ones print the exact data hole (TUNGA's ANIBACK
leaf holds no records at all - strum art absent). CI verify-level greps
the join log.

Still open: PCM tiling, and level bulk data (.blk/.suf, most sub-.des)
absent from this CD image (LEVELDES slots end at 0x19ba40bb; only 15 .DES
shipped) - stock game hits the idiv-0x50 trap at level start (LEO_OUT.LOG),
so original level rendering was never baselined.

**Help overlay (POMOC flow) DELIVERED**: the HD level block's POMOC1.MNU
lists page art as `static=x,y,pomocNN.seq` (x,y are 0,0 placeholders there);
the real anchor is the .SEQ element header (screen pos = (-f0,-f2), same as
menu leaves; POMOC00 -> (99,3) top strip on all levels). The MNU's generic
static= list (pomoc00..pomoc10) is only partly on the CD; per-level pages
are keyed by page number instead (TUNGA.11/JUNCTION carry POMOC32.SEQ) and
are collected from the CD block by POMOC<NN> name. Rendered as bg + first
patched element per page; Left/Right/Enter page, ESC closes, LEO_HELP_SECS
auto-close, LEO_HELP_PPM dumps the first help frame. CI `make verify-help`:
JUNCTION (pure-PCX scene) help dump == bare bg + exactly the oracle's 1768
touched pixels inside the page-0 window (C blitter == python oracle again).
cursor=mouse.seq / back.pcx / <stem>.pal payloads referenced by .MNU exist
in neither tree (data hole) - bg palette kept, no custom cursor.

## 9. Legal/ethical
Abandonware; user owns the copy. RE for personal rebuild/interoperability. Don't redistribute original binaries/assets; data files stay user-supplied.
