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
     2388 INTRO_P payloads). ESC-skipped bytes = **raw signed 16-bit
     little-endian PCM at 22050 Hz, mono or stereo per movie** (the earlier
     "8-bit unsigned ~8 kHz" reading was wrong and is what made cutscene
     audio crunch - see the audio bullet).
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
   * Movie "leftover fragments" (owner report 2026-11-15, ZARIA1 intro):
     DATA-INHERENT, not a port defect - verified by C==is_decode.py
     byte-exact on ZARIA1.IS every 5th frame (0..240), by the 184,855
     SKIP ops + stale-pixel persistence (~10-20 frames) measured in the
     decode, and by a Gemini vision read of the decoded frames ("a
     flying creature moves right leaving textbook delta-codec ghost
     fragments that clear later"). The original showed the same trails.
     Owner chain (2026-11-15/16): first "convert all movies to a less
     problematic format" -> landed as PR-41 (.LVM re-conversion +
     opt-in de-ghost, all green); then a QEMU ground-truth probe
     proved pixels AND frame sequence are byte-identical to the
     original and the films are natively 7-14 fps progressive content;
     then PR-42 smoothing (BLEND/MOTION to 60 fps) was prototyped -
     owner review: "all [variants] have same artifacts" (they live in
     the 1997 source frames). FINAL owner call 2026-11-16: "back to
     version before LVM format" - product is .IS-only again (PR-40
     state); converter/cleaner/prototype scripts kept in tools/ as
     offline tooling, feature SHELVED. Full records: NEXT_STEPS.md
     (PR-40 thread follow-up; PR-41 and PR-42 (SHELVED) sections).
   * **Audio (SOLVED 2026-11-14 — this was the "horrible logo video sound"
     bug):** the ESC region is raw **s16 little-endian at 22050 Hz**, MONO or
     STEREO per movie: every region is a whole number of DMA blocks, 1260 B
     stereo or 630 B mono, exactly matching the RPT8/RPT16 repeat that
     precedes it (region = repeat x block, verified for every payload of
     LOGO/ENA1/KONIEC/INTRO_P - INTRO_P: all 3150=5x630 with RPT8=5, and
     RPT8=65 -> 40950=65x630; LOGO (2,2520)...(5,6300)). Channel detector:
     gcd of region sizes (odd multiple of 630 -> mono, i.e. INTRO_P; else
     stereo) - agrees with autocorrelation (LOGO r(2)=0.96>>r(1)=0.55 =
     interleaved uncorrelated stereo; INTRO_P r(1)=0.94 = single channel).
     The engine silence-pads once a region is exhausted (the 0x80 fill in
     `build/decompiled/all/FUN_000185c0.c` - that function is the DMA IRQ
     handler (EOI out(0x20,0x20)) AND the video chunk advancer via the
     DAT_000c410c countdown, see the pacing bullet; its 4 input modes are
     mono-s16->u8 msb, stereo-s16 avg, stereo-u8 avg, straight copy, while
     `FUN_00019a60.c` sets rate 0x5622=22050 and takes the 16-bit DMA
     channel from BLASTER `H=` - 4..7, else none). Totals vs player memory:
     LOGO 703080 B @ 88200 B/s = 7.97 s ("the whole logo audio is about 8
     seconds"), ENA1 14.5 s, KONIEC 4617900 B = 52.4 s, INTRO_P 8497810 B
     @ 44100 = 192.7 s; byte statistics (as u8 mean|delta| == rms = noise,
     as s16 0.55*rms = audio, byte mean 129 = zero-mean s16); and A/B on
     the user's speakers (s16/22050 recognisable, u8/8000 = the reported
     crunch).
   * Ground-truth capture rig (diagnostics only, capture data never
     committed): `-device sb16,audiodev=a,irq=7,dma=1,dma16=5 -audiodev
     alsa,id=a,out.dev=tap` with `build/lltap.conf` (ALSA file plugin ->
     build/qemu_*.raw; define `pcm.tap`, never `pcm.!default`, and set
     `defaults.pcm.file_truncate false`). Note `out.dev`, not `out.device`.
     The plugin is unthrottled behind a null slave, so it writes much faster
     than realtime (2.6 GB in minutes): keep captures on disk, trim, clean up.
     The guest needs `SET BLASTER=A220 I7 D1 H5 T6` in AUTOEXEC.BAT or the
     engine never programs the DSP and the capture stays all-zero.
   * RAM forensics (reading engine globals from a snapshot): `pmemsave 0
     0x2000000 "file"` — HMP needs the path QUOTED; locate the loaded image by
     voting on `pos-off` over 32-byte windows of `build/leo/leo.img` (the code
     copy lands at `vaddr + ~0x2BCA00`, varying per boot, but data segments are
     relocated separately). Then read the **absolute operands out of the
     fixupped code**: e.g. `8b 3d <abs32>` in `FUN_000185c0` gives the live
     address of DAT_000c3fdc. .IS chunk payloads are streamed from the CD, so
     their bytes are generally NOT resident — searching RAM for file bytes is a
     dead end.
- `STATUS.LEO`: 300 B SOLVED 2026-11-15 (see 8f): 75 LE dwords, NOT
  pointers - each of three 21-word blocks stores ONE byte per dword:
  stored = (j|0x77072400) ^ (var|(j&0xffffff00)), j = index WITHIN block;
  reader keeps only (stored ^ j) & 0xff. High bits = variable-derived
  noise. Field semantics still want a paired QEMU capture (two shipped
  copies differ only in word63 DAT_000a94f0: 4 vs 5).
- `LONGSOFT.CFG`: 273 bytes, mostly zero; byte 0 = ASCII data-drive
  letter (FUN_0005bf10: DAT_000cf2be -> DAT_000cf254 = byte0-0x40,
  prefixes the ":\leo\" path sprintf at FUN_00047a70). Rest of header
  raw (SB port/IRQ/DMA-looking bytes 2-13 = hypothesis, unconfirmed);
  write path is in SETUP/CONFIG.EXE (16-bit). Blob round-trip in
  src/ll_save.c is byte-exact.

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
REVISED 2026-11-14 (no-CD rig, T-24): the 2-byte NOP is NOT deterministic -
the chain does `test eax,eax; je fail`, so NOPing the test leaves the `je`
reading the CALL's stale ZF (it happened to work in the UDVD2 rig, and fails
without any ASPI manager). `0x5bd5c: eb029090` (jmp past the test+je) is
correct in isolation but still fails in the no-CD rig: without any ASPI
manager the chain's later INT21 IOCTL 4408h/4409h probes (ll_cd_ioctl_
4408/4409, run against the drive from LONGSOFT.CFG byte0) are reached and
must also return 0 to reach the pass branch. The deterministic no-CD bypass
is therefore at the chain ENTRY, one byte in so the function's own
`push edx` (at 0x5bd50) / `pop edx:ret` (at 0x5bd8c) pair stays balanced:
`0x5bd51: b8 01 00 00 00 eb 32` = `mov eax,1; jmp 0x5bd8a` (the pass-counter
`add [0x3f1c0],eax`, identical to what the natural pass branch at 0x5bd83
does). Verified against raw bytes 0x5bd50..: 52 b8.. eb32, tail 01 05 c0 f1
03 00 5a c3. Patching at 0x5bd50 instead (dropping the push) unbalances the
frame and the game dies with "DOS/4GW error (2001): exception 06h"; the
chain's return path is `5a c3` (pop edx; ret) at 0x5bd90. See
tools/build_data_disk.sh header for the same reasoning.

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
equivalence. QEMU-monitor `sendkey` DOES reach the game (verified
2026-11-14: BIOS keyboard ring buffer @phys 0x41A/0x41C advances, and
`down` at the map menu actually selects + starts a chapter).

**Attract chain measured (2026-11-14, hunt9 = down@menu then 370 samples):**
chapter .IS movie (70 Hz, 480-line) -> 640x400 chapter map menu (corner
words wyjście/restart/autorzy/opcje, green chapter title "ZARIA I", BERLO
red-arrow sprite on an island; 40:6C counter FREEZES - the definitive
detector for this state; ONE static art frame (hunt12 dense capture: 16
identical frames ~0.9 s, then black fade - the hunt7 "two-screen swap"
was art<->black: |g781-black| mean|d| = 43.5 == hunt7's 43.68 pair), no
animation at all inside the window) -> `down` (or timeout) launches
the selected chapter: studio cards ("LERYX LONGSOFT CD-PROJEKT",
"produkcje LERYX LONGSOFT") + "LEW LEON" title ~55 s of 480-line movies ->
~68 s 480-line scrolling flyover (new art band y172-300 every ~0.285 s)
-> PLAYABLE LEVEL at 640x480: camera pans right, 70 Hz PIT clock stays
running, character enters at the bottom edge, scripted encounters (tree
hollow + crocodile). So menu is the interactive chapter selector and
`down` = activate, exactly like Enter.
NOTE (2026-11-14/15, T-24 no-CD rig): that whole attract chain was measured
in the CD-attached rig above. The scripted no-CD rig (tools/golden_build_
rig.sh + 0x5bd51 gate bypass, HDD-only, no MSCDEX/ASPI) reaches the game
and runs the .IS chain at the real 70 Hz audio clock, but has been seen
STAYING in the 480-line movie loop without the counter ever freezing
(three runs with 0.25 s tick sampling: 871 s, 931+ s, and a decisive
1500 s run on 2026-11-15); esc/spc/ret changed nothing and the timeout
screendumps show only 480-line movie frames (the DOS text buffer is empty
- graphics mode, not an error prompt). The CD-attached route was then
built and scripted (golden_build_rig.sh CD=1: CFG drive D + ATAPI CD
driver + MSCDEX on the floppy, QEMU -cdrom LL.iso, the 0x5bd51 patch
instead of the ASPI-dependent NOP - exactly the recipe above), and even
WITH the CD attached it stayed in the movie loop for 1500 s (max_rate
96-107.9 Hz) without ever freezing. So CD presence alone is NOT the gate
either - what the scripted runs lacked and the S6 GUI sessions had is
INPUT: "intro FMV plays, Esc opens the menu" (the verified-live note
above). golden_qemu_run.py now injects keys via the monitor
(--sendkey-at "T=KEY,"); the first Esc visibly re-phases the movie
chain (rate profile differs from the unattended run), so input lands.
The control experiment that separates "CD medium" from "a D: path that
exists" is the DISK-D rig (golden_build_rig.sh DDIR=1 + --ddisk: D: =
plain second FAT disk holding \LEO\, no CD, no MSCDEX, no driver) - it
boots to the game in ~5 s and behaves IDENTICALLY to the CD rig (same
movie chain, same key responses): confirmed, the route needs no CD.
FRAME-DOMAIN FINDINGS (2026-11-15, 7 scripted runs L1-L7 with the
frame-classifying driver - frame_journal.jsonl is the full state trail):
the CRTC geometry is a reliable state fingerprint (640x480 = .IS movie
chain, 720x400 = DOS text, 640x400 = the game's own canvas - the map
menu's mode, per the chapter-launch chain below: "the black 400-line
mode marks menu entry/exit"). Every early Esc (8-70 s after game start)
flips to 640x400 for ~1 s and back (attract logic returns to the movie
chain); in one run (L1) such an episode HELD - 640x400 from t=46.2 to
t=70.8, black, then an ART frame (187 colors) after the next Esc. That
is the live-verified "Esc opens the menu" behaviour reproduced under
script; the frozen-40:6C counter never appeared in ANY run and is NOT a
reliable menu detector on this rig (frame geometry replaced it as the
primary trigger). THE CAPTURE LANDED (runs L10+L12 then
A4+A5, 2026-11-15, byte-identical artifacts each time): one Esc@30 and
the ART frame itself as the trigger. PAGE CORRECTION (A4/A5): the menu
cycles by flipping the CRTC DISPLAY START between two resident pages -
the full-landscape ART page is at VRAM +0x0000 (read via LFB BAR
0xfd000000; hunt13's "page at vram+0" was RIGHT for it) and the
near-black overlay half is at VRAM +0x20000 (BAR 0xfd020000) - so a
single "page offset" claim was wrong either way; read_vram and the new
--want-art path pick the page by CONTENT-IDENTITY: the page whose
indices reproduce the frozen screendump with zero index->color
violations (page_reproduces_screen). Heuristics are foolable: a
distinct-byte scan took a wrong page once (L11) and content thresholds
qualified a mid-fade blend (A3 try 11, colors=109 mean=17), so the
golden target is the exact frame hash b880e203ce52 (sha1 of the
screendump, byte-identical in every observation ever made). Index->color bijection holds with ZERO violations; golden.*
is pinned in tools/golden_expect.json (161 used indices). The menu
FLIPS COMPOSED PAGES (full-landscape art frame / near-black frame)
via display-start - not a palette fade; the pinned snapshot is the
near-black half with 26 palette entries visible, all 26 exactly
matching ll_menu_dac.h (independent confirmation of the PR-19 DAC
recovery). Measured port-vs-original
best against the ART frame: 0.43% RGB / 6.4% index - the original's
map-menu page is a full static landscape, the port composes only the
overlay poses; that gap is the T-24 target from here on. ASSET HUNT
ROUND 2 (settled on the corrected bytes): palette-free permutation
sweep of the art page vs EVERY frame of all 15 .IS movies (5619
frames; a hit = zero index-pair collisions, i.e. same picture under
any re-palette) - best collision 0.55 (LOGO f107) = far from a
match; combined with the verbatim byte-grep 0-hits (still valid, it
used the right page) the art is NOT in any container: runtime-
synthesized or lost. DECISION (2026-11-15, user-approved): embed the
recovered picture as generated source - tools/menu_art_gen.py turns a
--want-art capture into ported/ll_menu_art.h (authentic index plane,
integrity-checked against the pinned idx sha), and map_bg() paints it
through ll_menu_dac.h by DEFAULT (LEO_MENU_BG=placeholder selects the
old procedural stand-in). MEASURED: port best vs the ORIGINAL art
frame went 0.43% -> 98.43% RGB/idx, off-palette px 0. The residual
~1.6% is COMPOSITION, not background: the original's page has its
corner words/titles burned into the static frame at placements that
differ from the port's .SEQ overlays and text draws; raising the
score means matching those, not more art work. The runtime-
synthesizer RE (iNFOSTREAM module / MENU.MNU) stays a deferred
deep-dive. The capture pipeline re-proved bit-stable after a full
host restart (2026-11-15: fresh rig build -> identical 4cb7a311).

**Chapter launch chain, full-res (2026-11-14 hunt10, 449 frames @ ~2 Hz
after `down` on the map menu):** chapter .IS conversation cutscenes
(character groups seated around a table) ALTERNATE with PLAYABLE level
segments (checkered interior hall, lion character bottom-center);
character walks right through the static camera at ~50 px/s (change-box
advances x88->x292 in ~1 s, y-band 228..478 = the sprite near the floor
in level mode); scene changes are hard cuts/fades (full-screen
mean|d| 45-90 between segments, never mid-scroll); camera scroll is
STEPWISE - long holds (0.5-2 s of mean|d| < 1) then a jump (cross-
correlation at 2 Hz resolves no sub-48 px slides). Title/credits-style
cards ("gra LARYS MODRZEJEWSKIEGO") appear mid-chain, and the black
400-line mode (fade) marks menu entry/exit (f1033/1034 pure black).

**Speed/pacing model.** CORRECTED 2026-11-14 (direct measurement, replaces the
earlier "40:6C stays frozen / ~22 Hz private counter" reading, which was a
host-side artifact of the throttled QEMU baseline): the game DOES use the
hardware clock - it reprograms PIT channel 0 to 70 Hz for the whole ISplay
subsystem. FUN_00018c40 (IS init) calls FUN_00018870: out(0x43,0x36);
out(0x40,0x95); out(0x40,0x42) = count 0x4295 = 17045 -> 1193182/17045 =
70.00 Hz, and FUN_00018ce0 (IS teardown) calls FUN_000188c0 which writes
count 0 (=65536 -> BIOS 18.2 Hz) back. Measured in QEMU (plain TCG, virtual
clock = wall): the BIOS tick counter 40:6C climbs at 69.8 ticks/s while an
.IS movie plays and 18.2/s during boot and in the non-movie states, exactly
matching the .IS audio block rate (22050/315 = 70 Hz). It *also* climbs at
69.8/s in every interactive post-boot state measured so far (attract,
flyover scroll, playable level: hunt5 median 69.6/s over a 126 s span) -
the only exception found is the chapter map menu, where the counter
FREEZES outright (delta 0 across the whole ~7-8 s window) while the game
polls the keyboard. 18.2/s returns only at boot and in 1-2 s
mode-switch dips. The non-movie one-shot timer class defaults to a 55 ms
period (FUN_0005b860 sets +0xe = 0x37; FUN_0005b9e0 converts ms -> PIT
counts with 0x1d216 = 119318*10, i.e. 1193182/10). So the engine beat is
70 Hz nearly everywhere; TICK_HZ=70 in ll_config.h.
Game logic/animation *loops* are still execution-rate bound (busy pacing),
so on stock TCG the CPU-bound parts run wildly fast while the
timer/DMA-paced parts (all .IS playback) run in real time.
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
 - cutscenes: pacing is the .IS codec's audio DMA clock (REV 2026-11-14,
   supersedes the earlier "byte 0x5D x 1/22 Hz" estimate, which ran
   cutscenes 2-5x too slow): ISplay plays the ESC audio at a fixed
   22050 Hz s16 (stereo or mono per movie, see the audio bullet) in DMA
   blocks of 315 source frames = 14.29 ms (FUN_00019a60: DAT_000c40b4 =
   0x5622, DAT_000c40b6 = 0x13b). Its DMA IRQ handler (FUN_000185c0, EOI
   out(0x20,0x20)) fetches the next video chunk every `repeat` blocks
   (DAT_000c410c countdown; RPT8/RPT16 ops reload DAT_000c41d0/c410c,
   header byte 0x5D = initial value). Empirically every payload's ESC
   region == repeat x block bytes (1260 stereo / 630 mono): LOGO
   (2,2520),(3,3780),(4,5040),(5,6300); ENA1 (4,2520),(6,3780),(8,5040),
   (10,6300); KONIEC 6300 up to 13860; INTRO_P 3150=5x630 with RPT8=5,
   RPT8=65 -> 40950=65x630. => each chunk sits on screen exactly as long
   as its region's audio (hold = bytes/88200 or /44100); region-less
   chunks hold repeat x 14.29 ms with silence padding. Totals: LOGO
   7.97 s (matches the user's memory "about 8 seconds"), ENA1 14.5 s,
   KONIEC 52.4 s, INTRO_P 192.7 s. LEO_IS_PERIOD_MS=N forces a uniform
   hold (tests); LEO_IS_FPS_DIRECT=1 keeps the very first naive reading.
- engine tick: REV 2026-11-14 (direct `x /1wd 0x46c` sampling in plain-TCG
  QEMU). The old "ZERO clock sync / frozen 40:6C / 22 Hz private counter"
  claim was a THROTTLED-icount artifact; the decompile does contain PIT
  writers: FUN_00018870 programs ch0 count 0x4295 (=70.00 Hz) on ISplay
  init (via FUN_00018c40), FUN_000188c0 restores count 0 (BIOS 18.2 Hz) on
  teardown (via FUN_00018ce0); the non-movie timer class FUN_0005b820/
  FUN_0005b860 defaults to 55 ms (+0xe = 0x37, ms->counts via 0x1d216 =
  1193182/10 in FUN_0005b9e0). Measured: 40:6C climbs at 69.8 ticks/s for
  the WHOLE span of every .IS movie (three INTRO_P replays: 194.45 /
  194.30 / 194.58 s wall vs 194.48 s predicted by the audio-block model,
  0.1% error) and drops to ~18.2 Hz only at boot and in 1-2 s inter-state
  dips. So: movies = 70 Hz audio-block/DMA beat (implemented); menu +
  gameplay beat = the 55 ms timer class (~18.2 Hz); CPU-bound loops stay
  execution-rate-paced (that part of the old claim stands). TICK_HZ=22 in
  ll_config.h is being replaced by these two classes. QEMU plain-TCG (no
  icount) is the real-time oracle: PIT and SB16 DMA run on virtual clock =
  wall, only CPU-bound loops run fast; icount shift=10,align=on is NOT a
  usable absolute-time base on this host ("guest is late by 0.0-1.0 s").

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
               [n B left-margin][n B right-margin]
                                      when w0&3, n=int16(b17),
                                       n<=0 reads nothing (game's
                                       negative-ESI loop)
               [for i=0..int16(b19): u16 L; L B]   unless w0&2 && !(w0&1)
  (CORRECTED 2026-11-16, PR-46: those two byte lists were first labelled
  "[palette][sprite]" - they are the per-row collision MARGINS: row i of
  the element shape is solid from x=left[i] to x=total_w-right[i], with
  total_w=int16(b19)*0x50+int16(b15) and byte 0xFF = empty row. w0&1 is
  the patch-art flag, w0&2 the shape flag. The BYTE grammar above always
  held and verify-seq never changed; see 8h for the geometry RE.)
  (PR-48: the [for i=0..int16(b19)] patch arrays decode with one of TWO
  row codecs chosen by w0&8 - clear = FUN_0001d9b8 (gap (cnt bytes gap)*,
  no fills, 0x40..0x7f are long gaps), set = FUN_0001dc80, the grammar in
  ll_seq.h. 106 w0=1 elements use the first; see NEXT_STEPS PR-48.)
  All 172 container .SEQ payloads (8 ANIMACJE/MENU + 33 ETAPY nested +
  131 leaf-block frame scripts; the 49-file recon corpus = 41 + 8
  byte-identical dupes) are consumed EXACTLY (parser end == payload size,
  zero leftover). Ported to
  `src/ll_seq.c`, oracle `tools/seq_parse.py`, CI `make verify-seq`
  (ASan/UBSan, C==python dump diff, also under wine in verify-win).
  Derived object fields (obj+0x0c..0x12 bbox, obj+0x14 = w0&8) are
  recomputed by FUN_000420b0 when w0&3 carries lists, from the words
  directly otherwise (FUN_00041f60), not stored in the blob. Text token
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

**Menu layout cracked + bg art CONFIRMED DATA HOLE (2026-11-14).**
ANIMACJE/MENU nested block (39 slots, 9 valid): LEVEL06/08.SEQ (chapter
island anims, 68x20/204x35), RESTGRY.SEQ (156x29 text sprite @ (82,86)),
AUTORZY.SEQ (64x19 @ (4,180) = bottom-left corner word), TAK.SEQ (60x35
@ (83,129)), X.SEQ (14x14 @ (155,54)), CNIE.SEQ, BERLO.SEQ, OPTIONS.PCX
(44141 B). All .SEQ coords are in a 320x200 logical space doubled onto
the 640x400 mode-10h screen (AUTORZY y=180*2=360 = bottom-left matches
the on-screen corner word). The globe+islands map background shown by
the original is NOT in any container or EXE overlay: full 1521-frame
pixel sweep of INTRO_P gives best mean|d| 33.9 vs the captured menu
frame (different picture), OPTIONS.PCX is a desert/pyramid scene that
belongs to the opcje screen (vision-confirmed), and no PCX/BMP headers
exist in ANIMACJE/ETAPY/SOUND or the LEO overlays. RAN.IS frame 0 looked
compositionally identical under the vision model but pixel forensics
refutes it: the captured frame is a perfect 2x2-doubled 320x200 indexed
image (even-aligned block equality = 1.000), and an index-plane
bijection sweep over all 15 .IS movies (4900+ frames; best per-index
agreement against the menu's 2x-halved downsample) peaks at 33% =
chance level. So RAN f0 similarity is palette/statistics coincidence.
Then hunt13 CRACKED THE MENU RENDER STATE: QEMU `stop` at a menu content
frame + `xp /65536b 0xfd000000` x4 -> the menu framebuffer is LINEAR
8bpp 320x200 at vga.vram+0 (NOT planar, NOT 640x400), displayed 2x2
doubled on the 640x400 CRTC: blob[0:64000] vs the screendump gives a
perfect bijective index->color map (209 used indices, zero ambiguity).
Consequences: (a) the AUTHENTIC art index bytes are now known (sha256
ba16421b...) and a byte-grep of the full 64000 B + sample rows across
every iso_stage file, leo_hd.img, leo.iso and the LEO.EXE linear image
still returns 0 hits -> the art is runtime-synthesized (probably via
the proprietary "iNFOSTREAM real-time decompression module V 2.01 BETA"
whose string sits at VA 0x904e4, or MENU.MNU-driven) or lost; our build
keeps a documented placeholder. (b) the REAL menu DAC is recovered ->
src/ll_menu_dac.h (generated by tools/menu_dac_gen.py from the capture;
regenerate to verify). Same index space as the .SEQ sprites: idx15 =
green corner words, idx22 = bright green title (art bbox x50..342
y108..158 in 640 space), BERLO golds 90..95 dark/218..230 bright (the
"red robe" guess was wrong: all olive/gold), island sand 199/200/202,
sea blues 44/46/51/102..107, idx1 = near-black key. (c) island anchors
are now the measured land-blob centroids (640sp): (188,132) (500,89)
(351,233) (470,259) (589,281) (83,348). menu_lut()'s CGA/BERLO guess LUT
is deleted in favour of ll_menu_dac[]; placeholder bg/words/title now
use the real tones. VL QA of the new 640x400 frame: all 4 corners,
title, crowned BERLO beside island 0, sea gradient + sparkles ok.
Then the COMPOSITING SPACE was fixed: the original composites in the
320x200 logical canvas (AUTORZY.SEQ @ (4,180) only makes sense there)
and doubles 2x2 on the CRTC; ll_menu_map_open now builds a 320x200
canvas + a 2x2 merge to the 640x400 window/PPM (ll_menu_anim gained
x/y = fixed logical origins; x<0 = park on selected island for BERLO).
AUTORZY.SEQ is wired as map-menu anim 1 at (4,180) and renders the
real bottom-left word (font-drawn AUTORZY deleted).
**ll_seq patch-row GRAMMAR fix (2026-11-14)**: the blitter state
machine (FUN_0001dc80 0x1dd59/0x1dd83) treats marker bytes <0x40 as a
DESTINATION GAP ADVANCE - the C painter previously ate them without
moving px, which sheared AUTORZY.SEQ into streaks. ll_seq_patch_walk
now emits kind=2 gap ops (painter advances px, seq_patchdump skips
them so the verify-seq C==py dump stays byte-identical); short
copy/fill runs CHAIN at marker position (only a gap byte returns to an
op position), matching tools/seq_patch.py. With this, AUTORZY decodes
to a legible 8-colour word (measured in-frame ASCII art), and
verify-seq stays green at 172/172 payloads. BERLO.SEQ decodes
byte-exact; per-element measurement (2026-11-15) later showed its 30
elements are COMPLETE one-per-tick POSES, not delta patches. The
PR-35 map path composited the current element over one that was
still partly on screen = a muddy olive-grey mix (the owner's PR-40
defect report "arrow is grey not red" + "arrow from previous state
still visible"; fixed in PR-40 v3 by rebuilding the canvas from the
base each tick - see the next paragraph).
RESOLVED PR-40 (measurements + a vision-model read, 2026-11-15,
three iterations, all recorded in NEXT_STEPS.md): the object at
logical x147..205 y94..136 is island DECORATION (sand + two
stepped-pyramid shapes + palms) baked into BACK.PCX, NOT a mace -
Gemini (gemini-3.8-flash) read the crop and said so; both original
captures (menu_art/menu_art2/vram.bin) show ZERO engine-drawn pixels
over it. Per-element measurement (seq_parse + seq_patch) settles the
cursor: each of the 30 BERLO elements is a COMPLETE pose of the mace
(~200-310 px, 15..27 x 35-37, width varies per pose = the authored
rotation, anchors sway ~1 px; golds idx223/224/218/230 + reds/browns
idx90..95; each element also carries a 36-byte palette override, not
applied - raw indices already hit the DAC golds). It is a normal
one-pose-per-tick animation script, NOT a delta chain (the "union =
complete cursor" reading came from measuring the union, not the
elements). v1 filled a silhouette from BACK art at a "(164,95)
home" fit = pyramid/sand patches inside the cursor; v2 baked the
union of 30 poses = "mace copying itself during rotating"; v3 is
correct: draw ONLY the current element per tick, anchored el0-at-the
park (authored sway reads as one mace moving), canvas rebuilt from
base each tick so poses never overlap. The island decoration stays:
authentic art in every original capture (owner's preference asked).
Golden: poses re-pinned, best 92.4594% (a ~200px pose overlaps the
static original capture better than the pre-cursor baseline 92.4516);
min_best_rgb 92 -> 90 stands.
**Menu dialog sprites placed (2026-11-14)**: none of the remaining
ANIMACJE/MENU .SEQ sprites match the captured chapter-map frame at
their element (-f0,-f2) origins (exact-index ratios 0.00-0.09, shift
search no better), so they belong to OTHER menu states driven by the
missing .MNU scripts. VL OCR of the decoded sprite bitmaps cracked
their semantics: RESTGRY.SEQ is cursive "restart gry?", TAK.SEQ "tak",
CNIE.SEQ cursive "nie", X.SEQ a 14x14 cross - i.e. the RESTART
confirmation dialog over OPTIONS.PCX (the Egyptian scene): "restart the
game? tak/nie/x". VL OCR of the composed frame reads all four words
cleanly. We reconstruct that state: restart corner (LEO_MENU_KEY=r / key r) opens ll_menu_open over OPTIONS.PCX with RESTGRY/TAK/CNIE/X
composited at their authored origins (frames cycle like the original),
and the (o)pcje corner shows OPTIONS.PCX alone. Verified numerically:
every painted sprite pixel is RGB-exact vs OPTIONS.PCX palette[idx]
(RESTGRY 1006/1006, TAK el1 1049/1049, X 120/120, CNIE el1 1184/1184)
and VL QA sees clean legible cursive, no streaks. LEVEL06/LEVEL08.SEQ
(20/28-row diagonal wedge sprites, 4 elements each) stay unwired -
their .MNU state is unknown (probably island/title animations);
placing them would be guesswork, so they are documented as available.
**Dialog answer buttons (2026-11-14, user-feedback fix)**: the cursive
words ARE the controls - ll_menu_open_ex hot-zones bind TAK/CNIE/X to
returns -10/-11/-12 with rects = union of painted sprite extents over
frames ((82,128)-(142,163) / (169,145)-(233,183) / (155,54)-(169,68)
canvas px) and keys t/n/x; click or key on TAK restarts the game via
the NEW GAME level chain (restart semantics = documented reconstruction,
the .MNU driver is the same CD hole). Map-menu island clicks now launch
instantly (a click = activation everywhere: corners act, islands launch);
LEO_MENU_PICK=<key> is the CI one-shot dialog hook (verify-menu TAK step
+ wine twin). OPTIONS screen stays art-only: the real settings UI was
SETUP/CONFIG.EXE territory, no in-game option assets exist on the CD.
**Desktop usability round 3 (2026-11-15, user feedback)**: (a) second
window keyboard focus under Plasma/Wayland - SDL_RaiseWindow alone was
not enough (mouse clicks worked, keys never arrived): both menu window
creations and the level scene window now call win_force_focus()
(= SDL_SetWindowInputFocus, single-arg SDL2 form under a
-Wdeprecated-declarations pragma) after SDL_RaiseWindow, AND the menu
dialog/map wait loops plus the level live loop poll
`if (SDL_GetKeyboardFocus() != win) SDL_RaiseWindow(win)` each tick.
The SDL2 wayland backend ignores both focus calls (KDE policy); if keys
still do not land, run with SDL_VIDEODRIVER=x11 (XWayland honors them).
(b) level scenes no longer exit the app: ll_level_show(secs<=0) = run
unlimited until ESC/q, and the map chapter-launch plus restart-TAK paths
pass 0.0 and CONTINUE - ESC in the scene returns to the chapter map.
CI: menu_tak/wmenu_tak pin LEO_MENU_SECS=1 and grep 'map: LEO_MENU_SECS
expired' as proof the chain cycled back to the map before ending.
**Desktop usability round 4 (2026-11-15)**: (a) keyboard focus was still
dead on the KDE dialog (SDL2's wayland backend ignores focus grants
outright) - main() now PINS the x11 backend when WAYLAND_DISPLAY is set
and SDL_VIDEODRIVER is unset (probe SDL_InitSubSystem, same pattern as
the pipewire->alsa pin in ll_audio_init; prints 'using X11/XWayland',
pins back to wayland if no X server). X11/XWayland honors RaiseWindow +
SetWindowInputFocus, so the whole focus chain works there. (b) hover
feedback added (user: mouse only acted on clicks): ll_menu_open_ex
tracks SDL_MOUSEMOTION - hov_z draws a yellow outline around the cursive
answer-word zone, hov_r draws the '>' row marker; ll_menu_map_open
outlines the hovered island (chs rect) or corner word (fixed rects
matching the click hitboxes) on disp AFTER the LEO_MENU_PPM hook so CI
frames stay hover-free. (c) 'improper render left side' re-verified:
dialog PPM byte-identical to the pre-hover probe (cmp rst1.ppm ==
rst_hv.ppm) and VL side-by-side vs standalone OPTIONS.PCX: same artwork,
left temple column authentic, no defect.
**Desktop usability round 5 (2026-11-15)**: user: the hover OUTLINE
COVERED the cursive words and there was no visible answer selection
("NIE seems active whole time", "clicking works but animations not").
Replaced: dialog hover = dim 1-px UNDERLINE under the word's sprite
bbox; keyboard selection = solid bright 2-px underline (ll_menu.c
draw_hline; draw_rect kept for map hover only). Answers are first-class
nav entries: LEFT/RIGHT cycle zones first then list rows (words sit
side by side, user asked left/right not up/down; UP/DOWN ride the same
cycle so lists keep working); Enter/Space confirms the highlighted
zone/item, Esc stays = cancel to the row; t/n/x direct keys unchanged.
Zone order pinned TAK, CNIE, X in main.c (was anim scan order TAK, X,
CNIE). New CI hooks: LEO_MENU_NAV=k = k cycle steps + Enter (verify-menu
step 'keyboard nav': proves the cycle lands on TAK and Enter -> -10 ->
new game -> back to map); LEO_MENU_SEL=i pre-highlights zone i for
LEO_MENU_PPM frames. Pixel proof: neutral dialog frame byte-IDENTICAL
to round-4 probe (rst1.ppm == rst_r5.ppm, zero regression); SEL=1 frame
diff = exactly 2x64 underline px at y=184/185 under CNIE rect
(169,145)-(233,183) + the (BACK) '>' marker removal; VL: underline fully
below 'nie', no overlap, no boxes, high contrast.
**Desktop usability round 6 (2026-11-15, user feedback)**: user: "TAK (YES)
should be bright green on select, as NIE (NO) by default". Measured why the
dialog looked one-sided: in the OPTIONS.PCX palette the restart sprites live
in, indices 2..22 are ONE monotonic green ramp (idx2 #010a01 L6.3, idx8
#044f08 L48.5, idx15 #0aa712 L103.1, idx16 #0ab311 L110.0, idx22 #10f91b
L154.0; idx1 #090202 is the sprite key), and the two answer words differ
ONLY by where their ramp sits: TAK.SEQ paints idx3..15 (403 px of idx15),
CNIE.SEQ paints idx2..22 (398 px of idx22), X.SEQ tops out at 22 as well.
So "bright green" IS the original's selection state, held by NIE by default
- no palette invention needed. Selection now blits the selected word's ink
shifted up that ramp until its brightest used index is 22 (src/ll_menu.c
GREEN_LO/GREEN_HI + menu_tint_build, measured per anim from its own patch
histogram): TAK +7 (idx3..15 -> 10..22), CNIE and X shift 0 = byte-exact
no-op. ll_seq gained index-mapping blits (ll_seq_patch_draw_map /
ll_seq_draw_elem_map; the plain entry points pass NULL, so every other
sprite and the byte-exact verify-seq path are untouched) - only the sprite's
own painted indices move, the OPTIONS.PCX indices under it do not.
ll_menu_zone gained `dflt`; main.c marks CNIE, so the dialog opens with NIE
already bright green + underlined and LEFT/RIGHT carries that state onto
TAK. CI: verify-menu dumps the dialog frame in three selection states (open
/ LEO_MENU_SEL=0 / LEO_MENU_SEL=1) and tools/verify_green.py proves the
change is (a) open frame == CNIE-selected frame byte-for-byte, (b) 1096 px
changed on selecting TAK, 848 of them inside the TAK rect
(82,128)-(142,163) plus the two underline strips and NOTHING elsewhere, (c)
the 403 idx15 ink px become 403 idx22 px and the word's mean luminance goes
76.0 -> 128.8. Wine twin selected-TAK frame byte-identical to linux
(cmp rst_tak.ppm == wrst_tak.ppm). Local extra (not in CI, the baseline is
not reproducible from the repo): the new open frame vs a build of the
previous commit (`git archive HEAD^` -> /tmp/opencode/base_build) differs by
CNIE's 128 underline px + the (BACK) '>' marker removal only - zero
regression on the frame the earlier rounds pinned.
**Desktop usability round 7 (2026-11-15, user feedback)**: user: "now yes
TAK changes color, but NIE dont change, and i guess that buttons should be
animated". (a) NIE/X could never change on select: round 6 shifts the
selected word UP the ramp, and CNIE/X already top idx22 (+0). Fix:
UNSELECTED answers shift DOWN to GREEN_IDLE_TOP = idx15 (#0aa712, top of
TAK's authored ramp; TAK idle stays byte-exact, CNIE/X -7) -> every answer
changes both arriving and leaving selection. (b) Animation facts measured:
each answer .SEQ carries 2 frames, identical shape, ink oscillating along
the green ramp, 100 ms period in the data - the authors' own pulse; the
frame diff (TAK rect 762 px, CNIE rect 995 px between ticks) had always
been cycling, which reads as uniform flicker, not state. New compose rule
(reconstruction labeled under the .MNU CD-hole banner, built ONLY from
measured ramp/frame facts): SELECTED word = bright ramp + cycles its 2
frames (animated button); UNSELECTED = dim ramp + frozen frame 0. Proofs
(tools/verify_green.py rewritten, wired in verify-menu): DEF==SEL_CNIE
byte-identical (NIE default); SEL_TAK vs DEF confined to TAK rect + CNIE
rect + underline strips, TAK idx22 ink 0->403 px lum 58.9->76.1, NIE idx22
398->0 (idx15 398 arrive) lum 61.8->47.4; freeze proof tick13->14 with NIE
selected: CNIE rect 995 px (pulse) vs TAK rect 0 px and X rect 0 px. VL
two-state side-by-side: brightness flip + underline tracking unambiguous,
no defects. Wine twin frame cmp unchanged-green.

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
removed). DECODE + DISPLAY SOLVED 2026-11-14 (src/ll_pcm.c +
tools/verify_pcm.py, `make verify-pcm`, wine-parity in verify-win): hdr
u16 w, u16 win_rows, u16 16, zeros; 40 x u32 table at 0x06 = absolute
payload offsets, each a valid mid-stream RLE resync point, monotonic until
padding restarts - NOT row-aligned, never seeked by the row loop, role
unresolved (the earlier "7 x 24-entry tables" claim was bogus:
0x06 + 40*4 = 0xa6 = data start); from 0xa6 ONE continuous byte-RLE stream
(same codec as .SEQ: byte >= 0xC0 -> repeat next byte C&0x3F times, else
literal) decodes to EOF exactly (tiny tail = partial final row); plane =
row-major stride w+80 (ctor FUN_00056b20 allocates win_rows*(w+0x50); the
80-px row tail is the parallax gutter, FUN_00057460 shifts each 80-px
strip by (strip_x-200)*shift/100 about the window centre). Display model
(blitter FUN_00056c72 + copy FUN_0001d528): one blit paints a 400-px
window = 5 screen strips of 80 px; screen strip k shows plane strip
(scroll+k) mod (w/80) - identity at rest - height clamped to <=200 rows;
`ll_pcm_view()` renders that 400-px rest view. The blitter recomputes the
pitch as (w/80)*80+80 which only agrees with the ctor pitch for
w-multiples of 80 (RAN 400 yes; TUNGA 600/ZARIA 700 = engine quirk; we
frame at the ctor pitch). Layers: RAN 400x1494, ZARIA.01 700x1491,
ZARIA.02 700x1714, TUNGA 600x2896. The w/80 idiv-0 SIGFPE at level start
= a MISSING PCM payload -> strips 0 (stock LEO_OUT.LOG), not a decode bug.
Visual reality (checked with the free remote VL model opencode/nemotron-
3.5-super-vl-free - no local GPU needed, works via subagent read tool):
the RAN win_rows band with the PIORUN3.PCX palette reads as a plausible
muddy ground strip (6/10), but deeper plane rows at any start offset
(0..0x500 swept) or pitch hypothesis decode to banded noise for BOTH
RAN and TUNGA - the window itself is honest data, but a row-interleave
inside the RLE stream likely remains unresolved (open question, not
blocking: the engine only shows the window at rest).

**src/ll_level.c is now a scene composer**: main .DES resolved by exact ->
<name>.DES -> <stem>.DES -> dir= scan of every LEVELDES .DES payload
(TUNGA.11 is carried by TUNGA1.DES); bg = the tlo= LAYER STACK
(`tlo=plan3.pcx,plan2.pcm,plan1.pcm` - whole stack, top layer first): the
first listed layer present in the ETAPY block owns the canvas; a .PCM
there renders its engine rest window (exactly `win_rows` rows - ctor
allocates win_rows*(w+80) and blitter FUN_00056c72 clamps blit height to
that window; deeper plane rows stream in only while scrolling, the
40-entry u32 table holds mid-stream resync offsets for that - as the
ground band at the canvas bottom, palette from the level block's own
.PCX donor (RAN.01: PIORUN3.PCX; grayscale documented fallback), a .PCX
composites normally; fallback any .PCX in the block.
RAN.01 on this CD resolves to PLAN1.PCM (plan3.pcx/plan2.pcm are CD holes)
- demo NEW GAME now shows the real first level RAN.01 on the PCM canvas.
objects = ANIMACJE/<level>/<LEAF> .SEQ sprites (ll_seq blitter)
with the HD .MOV driving frame+delta motion (LEO_HD_DIR=<dir with
ANIMACJE/ETAPY>); one script per leaf (prefers the .SEQ the .MOV seq= names,
ED000012-style unreferenced alternates dropped). Anchor heuristic until the
mov-name->ED-key join is known: x=|f0|, tall sprites bottom-aligned (MENU
scripts use negative f0/f2 = exact (-f0,-f2), level leaves carry positive
world-space values). CI:
`make verify-level` (fixed-tick PPM dumps tick2 != tick8, state-machine
grep, compositing proven vs bare bg: 1885 painted px, 4018 differing bytes
after the +4px bob; RAN.01 PCM canvas: scene `PCM view` FNV == ll_pcm
oracle hash, tools/extract_record.py + pcm_selftest).

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

**Scroll camera DELIVERED (2026-11-14, closes the "no scroll cam" gap)**:
the .DES `back_rect=x0,y0,w,h` keys are the true world extents (RAN
28800x400, ENA1 5169x574, TUNGA.11 2560x600, KARA 1600x1000, KADMAR2
2560x1800, LONA 2480x600, JUNCTION 320x200 = static one-screen; absent
on SADES/TUNGA2/ZARIA* = data hole -> static view). `ll_level` now pans a
horizontal camera over that world: .des placement X is validated against
back_rect (strum2 @ x=1494 now places, art still a documented hole),
objects draw at x-cam, and backgrounds pan per layer type - PCM through
the engine strip wrap (ll_pcm_view_at, screen strip k = plane strip
(cam/80+k) mod strips, FUN_00056c72), .PCX through SYNTHESISED tiling
((cam+c) mod img.w; the plan-layer .PCX blitter is unrecovered, the mod
choice keeps seams continuous - VL pixel-diff confirmed). Vertical pan
not modelled (world heights exceed the view; lew_rect hints exist). The
FUN_00057460 per-strip parallax SHEAR ((strip_x-200)*shift/100) stays
unmodelled: `shift` is a camera-velocity term we never observed (levels
never run on stock CD). Hooks: LEO_LEVEL_CAM initial pan; interactive
Left/Right pan, Home/End jump, p toggles 50 px/s autoscroll (original
walk pace). CI verify-level greps the world/cam line + strum2 placement
line + cam300 PPM != rest PPM; verify-win adds the byte-exact wine twin
(lvlv_cam300 == wlv_cam300); the rest-view PCM oracle hash is unchanged.

Still open: PCM .pal/.plt palette layers, and level bulk
data (.blk/.suf, most sub-.des)
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

## 8d. M5 music: MOD player (delivered 2026-11-14)

Level tracks are chosen by `.des` `muzyka=<stem>.mod` (verified map:
RAN->ran, TUNGA1/2+JUNCTION->tunga, KARA->kara, ZARIA2->lew, KADMAR1->
piramid, ENA1->podwoda, ENA2->ena2, KADMAR2->kadmar2, LONA->lona,
SADES->sades, ZARIA1->zaria); the payload is `<STEM>.MOD` in the SOUND
container (22 tracks, all parse).

**MOD files are STANDARD, not repacked** (CORRECTED 2026-11-14 - the
earlier "unique-pattern remap + 29 B ext dir + clamped/truncated
samples" model below was a parsing artifact and is WRONG; it made the
loader place sample data inside pattern bytes):
- standard 1084 B header: songlen byte @950, filler @951, 128-entry
  pattern table @952, id string @1080; instrument directory @20 is 31 x
  STANDARD 30 B records (len-words BE @rec+22, fine @+24, vol @+25,
  loopstart-words @+26, looplen-words @+28) - exactly the fields the
  engine copies at +0x16..+0x1C in MODGAP_13000;
- engine sets DAT_000aee99 = max(pattern table)+1 and FUN_00012d14
  loads that many NUMBERED pattern slots back-to-back at 1084
  (nch*256 B each); positions index slots directly (TUNGA max id 30
  -> 31 slots);
- sample data follows immediately after the pattern set, contiguous in
  directory order; every shipped track fits BYTE-EXACT with no clamp:
  TUNGA 1084+31*1024 = 32828, sum of dir lengths 171396 = 204224-32828;
  RAN 1084+14*1024 = 15420, sum 111842 = 127262-15420; ENA2 (npat 30)
  and MAPKA (6CHN, npat 10) likewise exact;
- directory volumes are real (64/48/42/24...) - the old "vol byte 0"
  reading was the wrong field offset;
- note words are the STANDARD 4-byte encoding: per=(b0&0xF)<<8|b1,
  inst=(b0&0xF0)|(b2>>4), eff=b2&0xF, param=b3; period histograms
  120..856 are valid ProTracker values, instruments 1..30 all name-
  matched in the directory ('crash_cymbal_2_b-3', 'square_wave_c4'...);
- the one engine quirk: every pattern row0 ch0 carries F0x SPEED
  (TUNGA/RAN/ENA2 F04, some TUNGA patterns F02/F06; MAPKA F03/F04);
  no Fxx>=0x20 tempo effect exists in any file, B only B1/B2, MAPKA
  has D0 row jumps;
- engine tables: 61-entry period table 1712..56 @0xa7272 (Amiga
  fine-0), 10-id signature table @0xa7204 (M.K./M!K!/FLT4/4CHN/6CHN/
  8CHN/CD81/OKTA/16CN/32CN).

**Player** `src/ll_mod.c`: 50 Hz tick model (tick_len = tempo*44100/
6250 = exactly 882 frames at the never-overridden default tempo 125),
speed from F0x -> 80 ms rows at F04; effects 0-9,A-F + E1,E2,E5,E9,EA,
EB; s8 samples, loop, LINEAR interpolation, alternating L/R pan, song
loops. Amiga period clock inc = 5278235/period (16.16) - the first
version used 52782351/period (10x too fast!) and the wrong sample
region; together they put fundamentals at 2.6-18 kHz instead of
200-600 Hz, which is exactly the "sounds strange / noisy" report.
Spectral check vs ffmpeg's own TUNGA.mod decode: top peaks identical
(349/350/442/587/589 Hz), band energy 20 Hz-12 kHz within +-2 points.
Deterministic pure-integer 44.1 kHz stereo mix -> `ll_mod_render()` is
the CI oracle: `make verify-mod` renders 3 s of TUNGA/RAN/ENA2,
FNV-1a-64 linux == wine byte-exact (TUNGA fnv=e21ee1d2b63508a5,
RAN b030b60db7c8c54c, ENA2 eea5e519512fb8ef).
Wired into `ll_level_show()`: block "5c" resolves `muzyka=` -> SOUND ->
SDL audio; LEO_NO_MUSIC or --no-audio disables. NOTE SDL_AUDIO: SDL2 >=
2.32 auto-picks its native "pipewire" backend which on the dev desktop
(SDL 2.32.74) corrupted EVERY callback-delivered stream (squeak/silence
on pipewire, pulseaudio AND alsa backends; alsa callback was clean at
neither 1024 nor 2048+ sample buffers) while SDL_QueueAudio+alsa was
byte-perfect and mpv clean; SDL's own disk-driver dump proved our mixer
bytes correct all along.  Music therefore never uses want.callback:
ll_music_play opens the device silent and ll_music_pump() feeds
SDL_QueueAudio (125 ms chunks, <68 ms left) from the menu loop and the
level scene loop.  ll_audio_init() (ll_level.c) additionally reopens
audio on the alsa backend whenever SDL would auto-pick pipewire
(Windows/directsound and explicit SDL_AUDIODRIVER= untouched). `mod_selftest FILE.MOD [MS] [OUT.WAV]` renders
audible WAVs for listening. MAPKA.MOD is 6CHN and probably not real
music (menu map ambience?) - not in the CI set, still parses.

## 8e. MAPKA level-select screen (delivered 2026-11-14)

Engine RE (ModFns.java / CallersOf.java): FUN_0005dec0 = map-screen
entry; it loads "mapka.mod" (string @0xa98b0, `MOV EDX` + play-mod
helper) and is called from 0x5e4b7/0x5e958 (the MAPKA submenu region);
FUN_0005df20 = stop-music, called from all 13 exit paths of that region.
So the original plays mapka.mod ONLY during level select - our LEVELS
submenu mirrors that (ll_music_play/ll_music_stop singleton in
src/ll_level.c).

- Level list = top-level id==2 blocks of ETAPY in container order; the
  CD carries 14: KADMAR2, KADMAR.07, TUNGA2.12, SADES.06, ZARIA.01,
  ENA2, JUNCTION, RAN.01, CHEATS, ZARIA.02, KARA.07, LONA.05, TUNGA.11,
  ENA.03 (ll_level_names()).
- MAPKA screen ART is a data hole: no MAPKA.* payload exists in
  ANIMACJE/ETAPY/SOUND/LEVELDES (CD or HD tree) -> submenu reuses
  ll_menu with NULL background + "(BACK)" sentinel (ESC == Enter in
  ll_menu, so a real back item is required). ll_menu now windows lists
  >10 rows (10-row scroll window + ^/v markers) so 14 levels fit 320x200.
- CI: make verify-sel - LEO_MENU_AUTO=2 + LEO_LEVELSEL_AUTO=N auto-picks
  the submenu and level N-1; greps LEVELS pick + 'll_music: playing
  MAPKA.MOD' + level pick + TUNGA2.DES wiring.

Related RE (deferred features, documented data situation):
- Death songs are per-lion-class strings in the actor-class tables
  (@0x81700-0x82400): TLewRan->ran1.mod, TLewKara->karade.mod,
  TLewZaria->looser.mod, TLewEna->ena2ed.mod, TLewKadmar1->kadmdead.mod,
  +k2dead/sadesdea/lonade/deatd.mod, paired with A_DEAD states. Cannot
  fire yet: no player lion exists in demo levels (no A_DEAD reachable).
- SFX: `.des sample=sound.des` (logical-name -> WAV map) is absent from
  this CD; .MOV SFX commands are smp2=/smp3=/smpauto=/smpprior=/
  smp_loop=/smp_stop= + debug `sample = %d` (strings @0x82884-0x828e9).
  WAV payloads exist in SOUND but the mapping file is a hole.

## 8f. Saves: STATUS.LEO + LONGSOFT.CFG (delivered 2026-11-15)

Byte-compatible codec `src/ll_save.c/.h` + CLI `tools/ll_save.c`
(`make ll_save`, `ll_save.exe`), in-game read peek via `LEO_SAVE=<path>`
(dumps decoded blocks at boot; nothing written back - the original
save/load sites are indirect vtable dispatches, findcall shows no direct
callers of FUN_0005dae0/0x5dcc0/0x5bf10).

STATUS.LEO grammar, re-pinned exactly from FUN_0005dae0 (reader) /
FUN_0005dcc0 (writer); open modes rb/w+b (strings @0x8369d/0x836a0),
name string "status.leo" @0x9a8a4: 75 LE u32 words, three 21-word
blocks A(state A)/B(base+0x54)/C(DAT_000cf46c) each encoded per slot
`stored = (j|0x77072400) ^ (var | (j&0xffffff00))` with j = index
WITHIN the block (0..20); the reader keeps only `(stored ^ j) & 0xff`
=> each dword carries ONE byte of a game variable, upper bits are
variable-derived noise never read. word63 -> DAT_000a94f0, word64 ->
DAT_000cf464 (both `((stored ^ 0x77072400)>>8)&0xff`), words 65..74
plain. The earlier "0x77XXXXXX tagged DSEG pointers" reading was wrong -
it is byte-per-dword obfuscation (0x770724xx noise prefix). Shipped
saves decode: A = B = [0,12..31], C = zeros, plain = [128,200,1,0x7].
The two on-disk copies (repo-root vs hd_files) differ ONLY in word63
(4 vs 5): DAT_000a94f0 is a progress-ish counter - first taste of field
semantics; full map still needs a paired QEMU capture (fresh-menu vs
mid-level saves). save() echoes raw words verbatim => load/save round
trip is byte-identical (CI `verify-save`: cmp + decode greps; wine twin
ll_save.exe byte-exact; SKIPs cleanly when the user-supplied originals
are absent - they are never in git).

LONGSOFT.CFG: exact 273 B; only field pinned in LEO.EXE is byte 0 =
ASCII data-drive letter (FUN_0005bf10: first stream byte -> DAT_000cf2be,
DAT_000cf254 = that - 0x40; drives the ":\leo\" sprintf FUN_00047a70,
cf. 6 baseline env fix). Bytes 2-13 look like Sound Blaster setup
(0x0220 port, IRQ 7, DMA 1 pattern - HYPOTHESIS, unconfirmed; writer is
16-bit SETUP/CONFIG.EXE, not RE'd). C codec round-trips the blob
byte-exact; ll_cfg_set_drive mirrors tools/cfg_ll.py (patch a COPY,
originals untouched - hd_files copy carries 'D', original 'H').

Closes the M5 "saves" box (load + byte-compatible rewrite + field
decode); remaining save tasks are semantics-only: paired QEMU capture
for the A/B byte map, then real save-at-exit wiring in ll_level.

## 8g. Playable level prototype - M4-B (delivered 2026-11-16, PR-43)

The level viewer became playable: `ll_level <DATA> RAN.01 --play` (or
LEO_PLAY=1) runs the LION with the level's own data - no invented
gameplay constants beyond one documented fallback (below). Code split:
SDL-free `ported/ll_play.c/.h` owns the .plt ground parser, the
EXTENDED .MOV parser (ll_mov_t = viewer mov_t + physics/transitions)
and the player physics; `ll_level.c` owns loading (containers) and
drawing (.SEQ anchor), which keeps every leg testable.

Data facts pinned 2026-11-16 (full CD + HD tree, sparse-correct scan):
- the RAN.01/TUNGA.11 PLAYER data is COMPLETE on our media - RAN.01's
  CD ANIMACJE carries the LEW.RAN leaf (8 .SEQ: UJ09 = 12-element walk
  cycle, UJ11 = A_LADOW 46 elements...), HD ANIMACJE RAN.01/LEW.RAN
  carries LEW.MOV (8666 B, 30 states) + LEWRAN.DES, HD ETAPY RAN.01
  carries RAN.PLT (4935 B, 153 ground lines). The PR-42 note about
  "borrowing ZARIA.02 lion art" was a PROBE BUG: ad-hoc python probes
  scanned nested index tables only `count` slots while llrs free-tables
  are SPARSE (PR-36 rule: scan (size-4)/26). tools/extract_record.py
  fixed the same way llrs_block() was (PR-36); the substitution is
  moot - both levels run 100% real data (TUNGA.11: LEW.TUN leaf, own
  36-state LEW.MOV opening with the scripted A_GIVE_EGG egg intro,
  TUNGA.PLT 7 lines).
- .plt grammar (plain text): line 1 = polyline count; then
  "<segments> x,y;x,y;..." world coords, y DOWN, feet ride the top of
  the lines. The leading count counts SEGMENTS not points - the parser
  scans every pair and ignores it. Point separators: ';' normally, but
  '!' occurs in real files as an EXACT equivalent inside the same
  polyline (ENA.PLT "1 1,560!7500,570;", ZARIA_A.PLT 10 times) -
  treating them the same was needed for ENA.03/ZARIA.01 to wire at all.
- .MOV EXTENDED grammar (superset of 8c's viewer subset): per state
  `control=BY_TIME|BY_X|CHECK_LN|CH_DIR|CHECK_ALL[_PLANE]|FRAME_REV|SOLID`
  flag lists ('|' separates, 2256 control lines in 775 of the 777 HD
  .MOV records, measured 2026-11-16). 13 further tokens exist in the
  data and stay UNMODELED (counted as ctrl_other, never invented into
  behaviour): CHECK_LEW 535 lines/205 scripts, DANGER 385/195,
  CHECK_ANI 168/59, SOLID2 152/132, ANTI_POCISK 121/121, SEMI_SOLID
  92/25, ANTI_SOLID 87/30, CHECK_ZW 47/22, SEQ_REV_Y 22/10, BY_TY
  12/7, CHECK_BASE_LN 8/8, SEQ_REV_X 7/7, INV_V 3/3 (415 scripts carry
  at least one). The SOLID family names the object-collision behaviour
  the unmodeled push-out (8h) feeds, so they are one line of work.
  `F=ax,ay` accel px/s^2, `T=x,y` speed cap
  px/s (scalar form exists too: "G=400"), `G=gx,gy` gravity px/s^2
  (grounded states G=0 + CHECK_LN), `seq=<stem>` PER-STATE art (one
  leaf's .SEQ set is shared by all states), extra keys `sample=`,
  `time=`, `time_scale=`; transitions `TARGET[(prio)]=COND[|COND...]`
  where prio is usually a number (1..97) but CAN BE NON-NUMERIC ("x",
  "X") and may be absent, CONDS K_FWD/K_BKWD/K_UP/K_DOWN/K_GEN1..3/
  K_ANY/AC_BREAK_NEXT are implemented, and the data uses much more:
  K_FIRE (253 uses), K_JUMP (76), K_ALT, K_F1..K_F12, K_GEN4 and a
  family of engine-internal AC_* signals (AC_ONAIR, AC_ENEMY_ATC,
  AC_LINA, AC_DRAB, AC_TIME, ...); comma-separated cond groups exist
  (`A_ZAPAL(1)=K_FIRE,K_F3`). Unimplemented names evaluate FALSE (never
  fire) - nothing guesses. `!` negates, OR semantics, first match wins.
  A transition that targets its OWN state (KADMAR2's real
  `A_MASZERUJ(x)=K_FWD`) means KEEP running: re-entering it restarts
  the frame list (measured: animation frozen on frame 1 while the lion
  slid) - self-targets therefore change nothing. Real values: walk
  F=400 T=90, A_STAY brake F=600, fall state A_START_LAD1 G=0,200 and
  its landing route A_LADOW(1)=K_GEN2.
- player wiring per level (.des, zero level special-casing): `lew=`
  token 2 names the art leaf (ANIMACJE) + token 3 the .MOV,
  `platformy=` the ground polylines (HD ETAPY, two real forms - see the
  inventory note below), `pozycja=` spawn feet (snapped
  DOWN onto the first surface: RAN.01 40,200 -> shelf 0..135@298),
  `lew_rect=` bounds feet x and defines world-bottom death -> respawn.
  `speed=` is ignored: the real .MOV T= caps are the authored speeds
  (RAN.01 speed=40,80;... contradicts - .MOV T=90 wins in practice).
  ALL 13 level .des files carry lew=/platformy=/pozycja=/lew_rect= -
  the wiring is uniform game-wide (measured 2026-11-16).
- GAME-WIDE player inventory (measured 2026-11-16 over the full CD +
  HD-install trees, sparse-correct scan) - 12 of the 13 level blocks
  run their OWN lion data in ll_level --play today:
  RAN.01 (30 states), TUNGA.11 (36), TUNGA2.12 (33 - reuses the
  LEW.TUN art leaf), ENA.03 (23, A_SWIM over the
  single 1,560->7,500 ENA.PLT line), ENA2 (17), KARA.07 (23),
  KADMAR.07 (37 - its .des names leaf "0", and ANIMACJE really has a
  leaf called "0" with TURN/LOOKUP/JUMPS/LAZY2... .SEQ), KADMAR2 (14,
  A_MASZERUJ marches), LONA.05 (40), SADES.06 (37), ZARIA.01 (37,
  opens A_DRAP), ZARIA.02 (31, opens A_SCIAG on the lift).
- `platformy=` has TWO real forms, and the ground grammar is identical
  in both (measured on the level blocks' own records):
  * `platformy=<stem>.plt` (11 levels) -> the level block's
    <stem>.PLT record, e.g. RAN.01's RAN.PLT "153\r\n1 0,298;...".
  * `platformy=<name>.des` (TUNGA2.12, SADES.06, JUNCTION) -> a
    SUB-.DES record of the SAME level block whose TEXT is that exact
    polyline grammar, count header and all: TUNGA2.12's PLATF.DES is
    "66\r\n1 1495,147;1529,147;..." (66 lines), SADES.06's PLATF.DES
    is "8\r\n1 863,166;..." (8 lines). One parser (ll_plt_parse) reads
    both.
  * the THIRD answer is an authored EMPTY ground: JUNCTION says
    `platformy=none.des` and its NONE.DES record is literally "0" -
    that level has no floor. ll_plt_parse accepts a header that
    DECLARES 0 polylines (rc 0, n 0) so "authored empty" stays
    distinguishable from "nothing usable here" (no count, or a count
    with no geometry), and the level says "has NO ground in its own
    data" instead of the "lacks ground" data-hole note.
- a bare `none` inside a .des list (JUNCTION `animacje=none`,
  RAN.01 `zwierzaki=chmury.des,ptaki.des,none`, `sample=none`,
  `ani_plat=none,...`) is the game's OWN keyword for "nothing here",
  NOT a missing record: it is skipped without a data-hole note and it
  counts as an answer, so the fallback scan never invents placements a
  level never asked for. `none.des` is a different thing - a real
  record name (JUNCTION's block really carries NONE.DES).
- The player wiring resolves EVERYWHERE: checked 2026-11-16, all 13
  .des `lew=`
  name a leaf that exists AND a .MOV stem that exists (RAN.01
  Lew.ran/LEW.MOV, ..., KADMAR.07 leaf "0"/0.MOV, TUNGA2.12 reuses
  Lew.tun/LEW.MOV, SADES.06 Lew.sad, JUNCTION Lew.jun) - JUNCTION is
  the only level that stays a preview, and that is its own authored
  data (no floor), not a gap. Note the two name
  spaces differ: LEVELDES records are named after the CHAPTER (RAN.DES,
  ENA1.DES, TUNGA1.DES, ZARIA1/2.DES, KADMAR1.DES) while the
  ANIMACJE/ETAPY blocks are named after the STAGE (RAN.01, ENA.03,
  TUNGA.11, ZARIA.01/.02, KADMAR.07) - the CLI takes the stage name,
  as the preview path already did. .PLT files exist ONLY in the HD
  install (the CD ETAPY carries .PCX/.PCM/.SEQ, never .PLT).
- .SEQ draw anchor CONFIRMED from data (UJ09 f0=0x23/f2=0x45): f0,f2
  is the anchor offset INSIDE the sprite (feet at bottom), so the draw
  origin = (feet.x - cam - (short)f0, feet.y - (short)f2) - same
  (-f0,-f2) convention ll_menu.c uses; the viewer's anchor_of() stays
  for objects only.

Documented simplifications (first, honest ones):
- LL_PLAY_GRAVITY = 200 px/s^2 applies ONLY to a CHECK_LN state that
  loses its ground with no authored fall route (RAN.01's authored fall
  chain A_START_LAD1 -> A_LADOW is an 18.4 s scripted waterfall, not a
  curb step; landing still fires the one-tick K_GEN2 pulse, which
  routes those states authentically). Same magnitude as the real
  A_START_LAD1 G=.
- ground following is line-continuation with a +-16 px snap window
  (LL_PLAY_STEP, both directions) - a 20 px gap at the same height
  (RAN.PLT 302->323) is walked; deeper falls fall. v1 controls: walk
  fwd/bkwd, fall, land, world-bottom respawn; no jump/crouch yet.

CI (verify-play, part of verify-all): hermetic `ll_level
--play-selftest` always runs (synthetic .plt + real walk constants);
PX_DATA legs run the scripted LEO_PLAY_SCRIPT demo on REAL data and
pin: LEW.RAN/LEW.MOV/RAN.PLT load lines, spawn snap (40,298),
lew_rect bounds, the A_STAY->A_WALK_FWD->A_WALK_BKWD->A_STAY
transition chain, the gap fall -> lew_rect death -> pozycja respawn
(the .plt gap near x=314 is real authored geometry), a render
differential (same tick count, script NONE vs FWD -> lion pixels move,
NONE frame stays at spawn), a LIVE-session leg (the platform key
injector gained arrow names: LEO_TEST_KEY=right pushes a real
SDL_KEYDOWN through map_key into the held-key tracking, so the live
substep + camera-follow path is proven headless), a MULTI-LEVEL matrix
(ENA.03, ENA2, KARA.07, KADMAR.07, KADMAR2, LONA.05, ZARIA.01,
ZARIA.02 - each must wire its own LEW.* leaf + .MOV + .PLT and run a
state by tick 30; ENA.03 additionally pins the ENA.PLT `!`-separator
ground and the A_SWIM state), a GROUND-FROM-.DES leg (TUNGA2.12,
SADES.06: platformy= names a sub-.des of the level block - must read
PLATF.DES 66 / 8 polylines, snap the spawn onto it (50,320)/(20,166)
and run a state), an AUTHORED-EMPTY-GROUND leg (JUNCTION:
platformy=none.des really holds "0", so the log must say
"NONE.DES: 0 polyline(s)" + "has NO ground in its own data" and must
NOT contain the 'lacks ground' or 'not in the HD level block' data-hole
wording, nor any placement the level never asked for), plus TUNGA.11
proving zero level special-casing. The live window path
tracks held arrows (platform KEYDOWN/KEYUP), steps physics on the same
fixed 20 ms grid and follows the player with the camera.

## 8h. Authored scene objects - M4-C (in progress, PR-45..PR-46, 2026-11-16)

The level viewer's scene is now the level's OWN: ll_level reads the place-
ment files each level's main .des names and runs their real .MOV scripts,
drawing the .SEQ art each script selects for the state it plays. The PR-36
fallback (inventing one object per ANIMACJE leaf .SEQ, joined by .mov stem)
is gone, and with it its false data holes.

Data facts pinned 2026-11-16 (full CD + HD-install trees, sparse-correct
scan; probes /tmp/opencode/pr45_census.py, pr45_join.py, pr45_forms.py):
- THREE .des keys name the scene-object placement files: `zwierzaki=`
  (18 references), `animacje=` (38), `ani_plat=` (12) - 68 files, every
  reference resolving to a real record (0 missing). The other .des keys
  own everything else (lew= the player, platformy= the ground, belka= the
  HUD bar, sample=/sufity=/blokady=/demolist=). Every .des record inside
  an HD level block is named by exactly one key of that level's main .des
  (0 orphans game-wide), so this list is complete.
- Placement-file grammar (68 files, 2188 entry lines): a count line, then
  one entry per line "<LEAF>,<class>,<SCRIPT>.<ext>,<x>,<y>" (world x,y,
  y down). The count line is NOT trusted - it disagrees with the real
  entry count in 38 of the 68 files (ENA.03 ZWIERZ3.DES declares 88,
  holds 134; RAN.01 DYMY.DES declares 186, holds 188), so every line is
  read and the real number is what gets reported.
- The game's own formatting variants, all measured, all parsed as the
  same names or counted verbatim - never guessed at:
  * spaces around the fields ("JROPE, dummy, jrope.mov, 490, 230" in
    ZARIA.02 anipl2.des; ", 16500, 360" in RAN.01 dymy.des): 109 lines.
    A space INSIDE a field means it is not a name - that is what keeps
    polyline lines ("1 1495,147;...") out of the entry rule;
  * 4 entries carry parameters after the coordinates (KADMAR.07
    "KPALMA,xxx,kpal.mov,640,640       -37,-7  -1 100  0", ZARIA.01
    "KAMIEN,dummy,kamien1.mov,1360,250,1308" x3): placed at their
    authored x,y, parameters printed and counted;
  * 1 entry names a script with no extension (KADMAR.07 ANI1.DES
    "BERLO,dummy,sdfsdf,61,54"): a hole in the placement itself,
    reported as such, nothing resolved from it;
  * per-entry property lines (damage= / energy= / bounds= / scope=),
    group headers, ';' and '//' comment lines, polyline lines: counted,
    named in the summary;
  * longest line in any of the 68 files is 207 bytes (buffer 512, any
    longer line would be counted, not truncated in silence).
- The scene join is LEAF -> SCRIPT -> PER-STATE ART, never the stem:
  field 1 names the ANIMACJE leaf table on BOTH installs; the HD one
  holds <LEAF>/<SCRIPT>.MOV, and every [STATE] block of a .MOV carries
  "seq=<name>" naming <NAME>.SEQ in the SAME leaf on the CD. Measured
  over the 708 distinct (level, leaf, script) triples the 2188 entries
  use: 681 have their .MOV on the HD install (27 real data holes, named
  exactly in the report); those 681 scripts name 960 distinct
  (leaf, seq=) art pairs, 953 on this CD, 7 true art holes (e.g.
  TUNGA.11 DNGANI/ogien_m.SEQ). The .mov stem matches one of its OWN
  seq= names in only 439 of the 681 - joining by stem (PR-36) invented
  the art of the other 242 and its 469 "data hole" notes were FALSE.
- .SEQ element flags split ART from COLLISION GEOMETRY (RE 2026-11-16,
  PR-46: decompilation of FUN_00041f60 (loader branch), FUN_000420b0
  (box math), FUN_00042a70 (hit test), FUN_00042de0 + FUN_00042cf0
  (push-out), FUN_000437a0 (mirror blitter), plus a machine-code re-read
  of the LE image where the decompiler lost register arguments). This
  replaces PR-45's "inline palette+sprite" reading, which was WRONG: the
  two w0&3 byte lists are per-row LEFT/RIGHT collision margins.
  w0&1 = the patch-art arrays this viewer draws, w0&2 = the shape; no
  real element is neither (w0=0/4 never occurs in the data). Row i of a
  shape is solid from x = left[i] to x = total_w - right[i] inside the
  element frame, total_w = int16(b19)*0x50 + int16(b15), rows =
  int16(b17), byte 0xFF = empty row; positioning adds the anchor
  (x -= f0', y -= f2', f' = 0 when the field is the 0x7fff ctor
  sentinel) and facing < 0 mirrors (x shift becomes tw - f0', the two
  lists swap). Of the 953 art tables the levels actually name, 784 carry
  patch art (drawn; most ALSO carry shape margins) and 169 are shape-
  only - single-element invisible solids that the ORIGINAL game never
  drew either (every draw path needs a patch array), including all 48 of
  RAN.01 ANIBACK's EDGE*.SEQ. They are reported with their measured
  geometry ("shape frame tw 28, 19 rows, bbox 1 0 28 20") as what they
  are - not data holes, not draw gaps; the viewer still draws nothing
  from them because it has no object-collider model to use a shape with
  (M4-C).
- The shape math is ported (ported/ll_seq.c: ll_seq_shape_wh/_bbox/
  _hit/_empty, twin tools/shape_geom.py + tools/seq_shapedump.c): the
  C-vs-python oracle is byte-exact over all 1656 container payloads
  (14306 shape elements; 34 of them declare rows whose margin lists are
  BOTH wholly the 0xFF marker - the game's own box math then stores
  w = 1 - total_w, h = 1 - rows, which the port reproduces and flags
  shape_empty). The hit test's row window was re-read from the machine
  code (FUN_00042a70 0x42b85..0x42be6, FUN_00042de0 0x42f21..0x42f7c):
  the element with the LOWER adjusted y is indexed from row 0, the other
  from the distance between them, over the shorter remaining span - the
  window PR-45 shipped was transposed and is fixed, with direction pins
  in --mov-selftest (a synthetic 9-element .SEQ, constants derived twice
  independently). The push-out (42de0 + 42cf0) is NOT modeled: its
  machine code shows the structure (per-side edge sums 6*x + 3*tw +
  three margin-row deltas, a max-penetration scan of w - list1[i] -
  (list2[i] + reg), an interpolator, a final clamp) but the operand
  identities and the two output pointers are unresolved through unmapped
  vtable slots (0x530d1/0x53184 call through +0x48/+0x50); a shipped
  value would be a guess, per the ll_seq.h "NOT MODELED" note (M4-C).
- Per-state art selection is pinned in --mov-selftest: a state gets
  exactly the table its own seq= names; a state that names none, a name
  that is not on these containers, and a shape-only (no-patch) table all
  draw NOTHING (no other state's table is ever substituted).
- Coordinates are checked against the view the level's OWN data defines
  (back_rect, else the single screen its bg gives): 1857 of 2188
  entries place; the 331 rejects are real - SADES.06 172, TUNGA2.12 83
  (their .des carry NO back_rect, so the fallback is the bg width),
  ZARIA.01 24, ZARIA.02 41, TUNGA.11 8 (outside its real 2560 world).
- Current per-level scene totals (tool output, matches the python
  census exactly; placed-scripts / art-tables / shape-only / script
  holes): RAN.01 827 placed, 82 scripts, 92 art (48 shape-only); ENA.03
  218, 55, 94; ENA2 57, 24, 23 (1 script hole); SADES.06 247, 164, 246
  (61 shape-only, 5 script holes); TUNGA.11 69, 43, 50 (1 art hole:
  ogien_m.SEQ); TUNGA2.12 35, 23, 35 (5 script holes); KADMAR.07 70,
  32, 55 (6 script holes + the no-extension entry); KADMAR2 50, 22,
  24; KARA.07 107, 46, 74; LONA.05 126, 30, 41 (4 script holes);
  ZARIA.01 29, 11, 21; ZARIA.02 22, 14, 31; JUNCTION 0 (its lists all
  say the bare none keyword - an authored empty scene, printed as 0
  files / 0 entries).
- Proof stays bounded: 24 placement lines then "... N more
  placement(s) of those scripts", 12 tick lines then "tick N: M more
  placement(s) advanced"; LEO_OBJ_ALL=1 prints every placement (CI
  proves it on TUNGA.11: MOVEPLAT/ed000002.mov[mov] art ED000002.SEQ
  5 states at its authored 599,164 - the one script the old invented
  model showed, now from the level's own ani_plat=).

CI (verify-level): .MOV selftest extended to pin the per-state seq= art
selection AND the shape geometry (a data-free synthetic .SEQ leg, runs in
every configuration: window direction, anchor + 0x7fff sentinel, facing
list swap, degenerate all-empty box); the TUNGA.11 leg
now pins the authored scene end to end (7 placement files, 77 entries ->
69 placed + 8 outside its own world, stale count lines reported as
stale, the one true art hole named, the bare 'none' keyword never a
fake hole), the fixed-grid cycle advance naming the SCRIPT
("tick 8: ANIBACK/strum1.mov state 0 frame 0") and its bounded proof,
and the RAN.01 canvas pins 827 placed objects + the shape-only
distinction by exact name with measured geometry (EDGE0000.SEQ:
tw 28, 19 rows, bbox 1 0 28 20).

## 9. Legal/ethical
Abandonware; user owns the copy. RE for personal rebuild/interoperability. Don't redistribute original binaries/assets; data files stay user-supplied.
