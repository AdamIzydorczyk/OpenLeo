# Full verification — OpenLeo port at `dfb2bf2` (after PR-46), 2026-11-16

Scope: the build, the whole test suite from a **fresh clone**, sanitizers on
real data, a review of `tools/` and of every document, and a frame-capture
sweep of **all 13 levels** (3 camera positions + no-object + 2 play scripts
each = 91 runs) plus menu/map/restart/help/cutscene frames, inspected for
graphic glitches. Every finding below is either **VERIFIED** (reproduced or
read in code/decompile/machine code) or **SUSPECTED** (strong evidence, the
original's behaviour not yet proven by RE).

Artifacts (scratch, not committed): `/tmp/opencode/vis/` (frames, sheets,
probes `codec_probe.py`, `codec_render.py`, `sweep.sh`), `/tmp/opencode/
review_tools/`, `/tmp/opencode/review_docs/`, suite log `/tmp/opencode/v_all.log`.

## 1. Summary

| Area | Result |
|---|---|
| `make` + `make win` from a fresh clone | clean, 0 warnings |
| `make -k verify-all` (fixtures) from a fresh clone | **EXIT=2**: everything passes except `verify-cmake` (harness bug F-1) |
| fixture-free run | EXIT=0, 21 named SKIPs, data-free legs `(ok)` |
| CI (run 43845936475) | green, all three jobs |
| ASan/UBSan/LSan on real data (5 levels × view/play) | 10/10 rc=0, 0 sanitizer lines |
| all 13 levels, 91 headless runs | 91/91 rc=0 |
| **Visual correctness** | **several real rendering bugs (section 2)** |
| Oracle/tool coverage | gaps that let some of those bugs through (section 3) |

The port is memory-safe and the suite is green, but the suite proves much
less about **what is drawn** than it seems to. The biggest visible defects
are below.

## 2. Graphic defects found (ranked)

### G-1 [VERIFIED, high] Wrong row codec for elements with w0&8 clear
The game picks the patch blitter per element by `+0x14` = `w0&8` (loader
`FUN_00041f60`: `*(byte*)(psVar2+10) = w0 & 8`; draw `FUN_000426a0`:
`+0x14 == 0` -> `FUN_0001d9b8`, else `FUN_0001dc80`). The two row codecs
differ: `d9b8` has **no fill runs**, a byte `< 0x80` in the marker position is
always a gap (so 0x40..0x7f = a 64..127 px gap), and `dc80` reads 0x40..0x7f
as a short fill/copy. `ll_seq_patch_walk` implements only `dc80` and applies it
to every element, and `tools/seq_patch.py` shares the assumption, so
verify-seq cannot see it.
Measured: 106 elements have arrays with w0&8 clear (all w0=1); **48 of their
106 blobs render different pixels**, in 5 payloads: `LEW.ZAR/I.SEQ`
(ZARIA.01), `LEW2.ZAR/I.SEQ` (ZARIA.02) — **live player art** — and `BERLO.SEQ`
in KADMAR.07/KADMAR2/KARA.07. Side-by-side render (`vis/codec_sheet.png`): the
port's decode folds the lion's hind legs and tail into the torso; the `d9b8`
decode gives a coherent figure.

### G-2 [VERIFIED data / SUSPECTED semantics, high] .MOV frame numbers are 1-based, negative = mirrored
Port: `obj_elem`/`play_draw` use the frame number as a 0-based element index,
clamp out-of-range to the last (objects) or 0 (player), never mirror
(`ll_play.h:74` asserts "frame number indexes the .SEQ element list directly"
without RE). Data, over every HD .MOV joined to its CD .SEQ:
- 4266 of 18432 frame references (23%) are >= the element count, nearly all
  exactly **N of N**;
- per sequence: **750 use 1..N and never 0**, only **8 use 0..N-1**; 179 use
  both 0 and N (0 = blank frame?);
- negative frames are pervasive and almost always exactly **-N..-1**.
The game's draw entry `0x422f0` takes a signed value in `ecx` and
**`ecx <= 0` selects the mirrored draw** (machine code; push 1 -> 426a0's
mirror flag). If the sign comes from the frame number, the port is
off by one on every animation frame, draws the last frame where the author
wrote 0 (blank), and never mirrors. **Visible consequence, verified:** the
three solid black 40x80 rectangles on KADMAR.07 (`vis/k7_max.png`) are
`KOLUMNA/box40.mov` (`control=SOLID`, `;0,5000` commented out, `1,5000` used) —
its only element is drawn because of the clamp. Under 1-based reading frame 1 =
element 0, so the RE of the frame->element mapping (and of palette index 252
on that level) decides this case; the clamp itself is an invented draw either
way (the project's "data never guessed" rule).

### G-3 [VERIFIED game code / SUSPECTED for objects, high] Element anchors not applied to scene objects
The game's element draw (`0x422f0` and the sibling entry before it)
subtracts the anchor: `x -= f0`, `y -= f2` when `f0 != 0x7fff`, and
`x -= tw` for the mirrored draw. `play_draw` applies the anchor; `obj_draw`
deliberately does not (its comment calls anchors a menu/player convention).
Data: 11848 drawn elements have real anchors, **11740 of them >= 5 px, 8883
>= 20 px**; within one animation the anchor moves in **868 of 907**
multi-frame sequences (418 by >= 20 px, e.g. ENA.03 BA0002 f2 119..191). With
the anchors applied the frames form the authored motion (ENA.03 BA0005: a
bubble burst rising steadily from its vent; `vis/anchor_demo.png`); without
them that motion is lost. The direct caller of `0x422f0` for scene objects is
vtable-reached and not yet located — that is the one missing proof.

### G-4 [VERIFIED, high] PCM layers blank two levels - RESOLVED in PR-53 (wrong layer class had been read; see NEXT_STEPS)
- **KADMAR2** (PCM-only `tlo = plan_.pcm`): the 400x200 view shows a 32-row
  strip of noise and 168 never-written (white) rows on every camera position
  (`vis/sheet_b.png`, top row). The layer decodes as "1800x1485 rows (window 32
  rows, stride 1880)" — stride != width.
- **LONA.05**: rows 200-600 of `PLAN3_.PCX` are 100% key color (0,0,255,
  832000/832000 px); the layer that fills it, `PLAN12.PCM`, decodes as
  "0x0 rows", so 2/3 of the screen is flat blue.
The PCM row-window question is recorded as open (RAN.01 pin note), but no doc
says that it blanks whole levels.

### G-5 [VERIFIED, medium] Player and objects below the view on 7 of 12 levels
The camera pans horizontally only. Spawn feet-y vs view height: ENA.03 560/420,
ENA2 993/600, TUNGA.11 402/359, KADMAR.07 687/500, KADMAR2 1504/200,
ZARIA.01 680/475, ZARIA.02 690/475 — the lion is invisible on those levels
(`vis/play_cmp.png`: visible on RAN.01, absent on ZARIA.01/TUNGA.11). Scene
objects near the bottom are cut too (a bird on ZARIA.01). "No vertical pan" is
documented; its effect on 7 levels is not.

### G-6 [VERIFIED, medium] Hard seams where the camera wraps the background
`bg_at` tiles the PCX by `(cam + c) % img->w`. On KADMAR.07 (world 2240, bg
1920 wide) and KARA.07 the sky gradient, mountains and pyramids are cut at a
hard vertical line (KADMAR.07 cam 1280: exactly x=640). The original almost
certainly scrolls the bg at a parallax rate instead of tiling a non-tileable
image (not yet RE'd).

### G-7 [VERIFIED data / SUSPECTED semantics, medium] 149 runtime-spawned objects parked at (0,0)
Authored entries with exactly `0,0` (e.g. `POCISK,none,jajo.mov,0,0`, 56
projectile pools; `lew_swt,none,swiatlo.mov,0,0`, the lion's light effect;
93 others) are drawn permanently in the top-left corner (an egg/explosion
sprite visible on RAN.01, ZARIA.01, SADES.06, ENA2...). The parser is faithful;
the objects are spawn pools or attached effects the original activates at
runtime.

### G-8 [VERIFIED, low] View equals the background width on LONA.05
`cw = img.w` gives a 2080x600 window (no pan inside the bg), unlike every
other level.

### G-9 [SUSPECTED, nit] "(BOCK)" on the restart screen
Probably "(BACK)" drawn with a mis-substituted glyph (the PR-37 font recovery
is incomplete by record).

Clean, by inspection: the RAN.01 scene and lion walk, TUNGA.11/ENA.03/
TUNGA2.12/SADES.06/KARA.07/JUNCTION backgrounds, menu map, restart dialog,
help overlay, and the cutscene frame (byte-exact vs the python reference).

## 3. Tools and verification harness

Bugs (VERIFIED):
- **F-1 `verify-cmake` (Makefile:1094)**: the data branch reuses the cache at
  the fixed path `/tmp/opencode/cmbuild` (only the no-data branch `$(RM)`s it),
  so any other checkout fails ("CMakeLists.txt does not match the source used
  to generate cache"). Passing after clearing the cache. This is why the
  fresh-clone suite ends EXIT=2.
- **F-2 verify-seq never compares inner gap values** (`seq_patch.py:83-85`,
  `seq_patchdump.c:28` skips kind-2 ops): 630,533 gap markers in 14,448 of
  19,116 blobs are unchecked. With G-1 this is why the patch oracle proves less
  than "byte-exact" suggests.
- **F-3 `verify_pcm.py:28` scans `range(cnt)` (the PR-36 sparse-table bug)**:
  4 of 23 PCM payloads compared ("4/4" is pinned), missing e.g. RAN.01/
  PLAN2.PCM — the very layers of G-4. C and python agree on all 23 when forced
  (the reviewer's scratch run), so this is lost coverage, not a hidden
  mismatch. `verify_help.py:41` has the same loop (latent loud failure).
- **F-4 vacuous passes**: `verify-llrs` prints "C == python (exact)" for a
  missing container (both sides empty — reproduced); several `cmp -s A B`
  "frames differ = ok" legs pass when neither frame was written (outputs not
  removed first, producers end in `|| true`), e.g. Makefile:745, :157, :196.
- **F-5 `PX_WINREF` needs `/tmp/opencode/py_last/f01520.ppm`, which no target
  creates**: verify-win runs here only because of a file left by old manual
  staging; a truly clean machine skips it even with data.
- **F-6 `seq_render.py` (help oracle) ignores inner gaps** too: same pixel
  count, 970 of 1768 positions differ on POMOC00 element 0; `verify_help.py`
  only checks the count + a window, so it passes.

Risks: `ported/ll_mod.c` has no independent oracle (linux-vs-wine runs the
same C twice); most python oracles transliterate the C (shared bugs pass);
`verify-arch` greps only `include <SDL2` (`<SDL.h>` would pass); ~20
Makefile legs hard-code `LEO_HD_DIR=/tmp/opencode/hd_files` instead of
`$(HD)`; the sparse-table walk exists in 5 drifting copies; `clean` does not
remove `seq_shapedump*`; `llrs_unpack.py` and `llrs.c` disagree on slot-name
validity (no shipped data triggers it).

## 4. Documents

Wrong / missing (VERIFIED):
- 18 `LEO_*` hooks read by the code are undocumented in README, incl. the
  user-facing `LEO_HD_DIR` (required for the README's own `--play` example),
  `LEO_DATA`, `LEO_USER_DIR`, `LEO_SAVE`, `LEO_NO_MUSIC`, `LEO_DEMO*`.
  No README hook is stale.
- README: verify-pcx says "6 payloads" (65 measured; also PLAN:572,
  TEST_REPORT:49); 6 verify-all targets unlisted (quit/utf8/fuzz/platform/
  userdir/cmake); `ll_level` run examples without `make ll_level`; README links
  to no other document.
- NEXT_STEPS PR-45 record (2162-2173) still states "inline palette+sprite"
  with no in-place pointer to the PR-46 correction; PLAN 596/648/677/747 still
  say "172 .SEQ payloads" (1656); Makefile:347 comment says 172 although the
  PR-46 record says that wording was fixed; NEXT_STEPS header "through PR-19",
  PR-21 "IN PROGRESS", "golden capture half open", P-05 "pipewire->alsa swap"
  are stale.
- Anchor/sentinel wording (PLAN 8h, NEXT_STEPS PR-46, ll_seq.h): "x shift
  becomes tw - f0', f0' = 0 for 0x7fff" is wrong — the game gates the WHOLE
  shift (x and y) on `f0 != 0x7fff` and subtracts f2 raw. The port uses a
  per-field gate; equivalent on all shipped data (14126 elements both real,
  180 both sentinel, 0 mixed), a latent divergence otherwise.
- "RSS pinned 1114" (NEXT_STEPS) — no such pin exists in Makefile/CI.
- `ported/README.md` says the Makefile builds `original/`; it does not.
- TEST_REPORT.md, IMPROVEMENTS.md, PORT_DEEP_DIVE, IMPLEMENTATION_PLAN are
  historical (PR-19-era / pre-PR-7 / PR-25 work orders) with no banner.
Spot-checks that match the code: the PR-46 shape math (tw, bbox h, list swap,
0xFF handling, window counts, API set), the PR-45 census numbers across PLAN/
NEXT_STEPS/ll_level.c.

## 5. Recommended order

1. **G-1** codec by w0&8 (small, fully RE'd; add the `d9b8` decode to the C,
   an independent python twin, and the inner-gap comparison F-2).
2. **G-2 + G-3** frame numbering / mirroring / anchors: one RE task (find the
   object update -> `0x422f0` call, read how the frame value becomes element +
   facing), then fix both draw paths together; replace the clamp with "draw
   nothing" for references the game would not resolve.
3. **F-1, F-3, F-4, F-5** harness fixes (cheap; they make the suite honest).
4. **G-4** PCM layer row-window RE (blanks KADMAR2, LONA.05).
5. **G-5/G-6** vertical camera + bg scroll rate (RE of the camera).
6. Docs pass (section 4).

## Addendum (after PR-47..PR-53)
All items of section 2 are resolved or recorded as viewer policy; G-4 turned out to be the biggest win: the PCM layers are transparent tile-opcode overlays (23/23 layers decoded, every row 20 dwords, every tile contiguous) and every level now shows its real scenery. Still open: the engine's per-layer parallax factors, the control= flags, and an independent MOD mixer oracle.
