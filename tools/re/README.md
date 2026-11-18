# tools/re - reverse-engineering and reference-video helpers (scratch tools)

These are the throw-away tools used to read the game's machine code and its
data containers and to measure behaviour on a reference playthrough video.
They are NOT part of the build or of `make verify-all`; paths are hard-coded
to `/tmp/opencode/...` (see HANDOFF.md section "Machine layout") - edit them
for another machine. No game data and no video frames live in git.

## Containers (C, build with the port's own reader)

    cc -O1 -Iported -o /tmp/opencode/llrs_tree tools/re/llrs_tree.c ported/llrs.c ported/fs_ci.c

| file | what it does |
| --- | --- |
| `llrs_tree.c` | `llrs_tree CONTAINER` - list every block/leaf (3 levels) with ids and sizes (ANIMACJE/ETAPY/SOUND). |
| `llrs_cat1.c` | `llrs_cat1 CONTAINER BLOCK RECORD` - write one record of a block to stdout (e.g. a `BELKA.DES`). |
| `llrs_cat2.c` | `llrs_cat2 CONTAINER BLOCK LEAF RECORD` - same for a record inside a leaf (e.g. `LEW.MOV`). |
| `llrs_extract_mov.c` | dump every `.MOV` of an HD ANIMACJE container to `/tmp/opencode/mov/` (for statistics). |
| `seq_sheet.c` | `seq_sheet PCX SEQ OUT.ppm` - draw every element of a `.SEQ` in a grid (used to look at the HUD art). |

## .MOV text (python, stdlib only)

| file | what it does |
| --- | --- |
| `mov_channels.py` | `mov_channels [DIR]` - per state, the header `F=`/`T=`/`G=` pairs plus the sums of the row-level `F`/`FC`/`I`/`IC` pairs, i.e. the two motion channels the engine merges (NEXT_STEPS PR-98); decodes every state's `control=` word into the engine's own mode+flag bits and counts the modes and tokens (NEXT_STEPS PR-99); counts the `T=` values over the 130 cap the engine's parser applies; lists every force the BY_X states pair with more than one duration, which is the fixture that would separate an F law from an F/T one; and (NEXT_STEPS PR-100) censuses the second field of every row - the threshold the engine at 0x52320 compares against the row accumulator `[obj+0xa1]` - grouped by movement mode, which is where the "a row threshold is in its own mode's feed unit" reading comes from (94% of the BY_TIME values are exact multiples of the 100 the engine adds per step, the BY_X values are small counts of px). Reads `/tmp/opencode/mov` by default. Its row pattern needs a
two-digit frame number, so it sees 15996 of the 18617 row lines that
`ll_mov_parse` would read (`mov_byx_gate.py` prints both numbers and
attributes the whole 2621-row gap to rows with a one-digit or negative
frame field). |
| `mov_byx_gate.py` | `mov_byx_gate [DIR]` - the census for the walk gate
that `ll_player_step` sections 4/7 and `obj_row_tick` share: the engine's
dispatcher 0x543a0 keys the row accumulator's feed on the control word's
mode bits 0..2 alone, while the port also requires a non-zero state `F=`
(`(ctrl & BY_X) && fx != 0`). Prints, as measured counts: rows read the
way the parser reads them (`sscanf("%d,%d")`) alongside this repo's
narrower row regex; how many states name each mode in `control=` versus
get mode 0 by default (no `control=` line at all, or a list with no mode
token); the port's split on the same states and the number where the two
readings disagree; every state block that names `BY_X` and writes
`F=0,0`, with its rows, `T=`/`G=` values, whether the block declares a
motion source of its own, and its exits; the states for which that gate
is the whole law (no motion source and no exit except a wrap-armed one);
and the threshold table for PR-110 items 1, 2 and 8 under four readings
- as written, as `ll_mov_parse` stores it (a non-positive row value
becomes 100, so a stored threshold of 0 does not exist), with the block's
`time_scale=` applied, and as PR-110 counted. Also counts active and
commented `time_scale=` lines. Stdlib only, inventories only, no game
data in git. PR-115 adds a section, the one items 1 and 8
actually need: it reads the row threshold AS AUTHORED (the engine's
`%hi` read at 0x51667/0x51679 into `[node+0x10]`, scaled at
0x51680-0x5168f, and NO clamp anywhere) against what the port stores
(its `ms > 0 ? ms : 100u` at `ll_play.c:481` runs BEFORE the
`time_scale` scaling at `:735`), per mode: rows under 100 as stored,
the states whose stored minimum one 100-unit iteration crosses TWICE,
the same question for the distance-fed half under both per-step feed
estimates (`|T| * 0.028` for the player, `|F| * 0.028 / 4000` for a
scene object, both labelled estimates), and the loops' own bounds -
`4 * nfr` closures plus the accumulator resync in the player, nothing
but the 4096-iteration guard in the object engine. PR-116 adds section
5, the boundary PR-110 item 2 asks about: rows whose threshold AS
STORED is an exact positive multiple of the 100-unit feed (that is
where the port's `acc >= thresh` and the engine's `cmp`/`jge` at
0x52329/0x5232b differ by one whole feed), per mode and per state
whose MINIMUM sits on that boundary, and the count of `Wait` directive
lines - `Wait` (0x91989, handler 0x46b62) is the only row form whose
node carries a key and so takes the 0x52251 branch, which matches
through 0x47e40 and never compares a threshold. The engine's name compare
at 0x46b59 is 0x76a69, which lower-cases BOTH sides, so the tool prints
both counts - 0 tokens with the exact case the table stores, 255 with any
case, 251 of them on row lines - and prints the boundary population as a
bracket: 15066 at the bottom, 15317 at the top, in 2012 states.
PR-118 adds section 6, which asks that boundary question for the
it measures the walking states by the port's own rule (the `walk`
expression at `ll_level.c:1169`), prints their stored thresholds and how
many of them are an exact multiple of 100 (none: those rows are 2..10
units, and a multiple-of-100 test never selects one), then per dt value
this port really calls `obj_advance` with (20 ms, 40 ms, 28 ms =
`LL_ITER_MS`, and the 2000 ms PR-117's test had to invent) reports
whether any whole number of identical calls lands a threshold exactly in
exact arithmetic versus the port's own double accumulator, prints the
divisibility law behind the answer (`k * 7|F| = 1000t` on the 28 ms grid,
so the smallest reachable threshold is above every authored row here),
shows the one case where the port's float accumulator drifts off a
landing exact arithmetic says it has, and states plainly that the
section-5 multiple-of-100 count is the boundary for the TIME-fed rows
only while the distance-fed rows have a boundary of their own at a
multiple of their own per-call distance. Stdlib only, inventories only,
no game data in git. NEXT_STEPS.md PR-114, PR-115, PR-116, PR-118. |
| `des_census.py` | `des_census [DESDIR] [MOVDIR]` - joins the two halves of the walk-law question: the `.MOV` side `mov_channels.py` censused (BY_X states with `F!=0`, and the law speeds A = |F|/4 and B = 10F/T, which coincide exactly at T=40) and the side nobody had counted - the level `.des` placement files, the ONLY thing that instantiates a class on screen. Prints the placed (level, class, state) rows and the complete law-discriminating surface with T!=40 folded away, each with the verbatim `.des` line that places it; and the never-placed classes, split into the engine-spawned lion variants and true orphans (measured: 34 placed rows, 5 with T!=40 across 4 classes; 2 true orphans, the dummy class `0` and KADMAR.07's NINJA1-6, complete ANIMACJE data referenced by no `.des`, no `.LVM` script and no other `.MOV` - an orphan cannot be filmed, so PR-125's NINJA fixture pick is void and the named referee is now KADMAR.07's BOSS A_RUNA/A_RUNB, F=75 T=20, A 18.8 vs B 37.5 px/s). Reads the `.DES` text dumped by `llrs_cat1` from every ETAPY level block and `/tmp/opencode/mov` by default. Stdlib only, inventories only, no game data in git. NEXT_STEPS.md PR-126. |

## Disassembly

`make_text_asm.sh` - objdump of the LE image's `.text`; VA = file offset + 0xf000.

`node_classes.py` - `python3 tools/re/node_classes.py [LEO.ELF]` - census of
the engine's attribute-node class tables, the 6-slot method tables the nodes
point at through `[node+4]`: where they live in the image, which methods the
whole family shares (the two chain getters 0x45800/0x45870, the channel-query
walk 0x458e0) and which are per class (0x45d50/0x45d90/0x45dc0 answer a
channel id; slots +0x00/+0x14 are unique to each class) - NEXT_STEPS PR-101.
Reads `build/leo/leo.elf` (not in git; see HANDOFF.md section 3).

## Indexed search over the image (`leodb.py`)

`python3 tools/re/leodb.py build` once (about 1.5 s), then indexed lookups
instead of grep-and-scan. It builds `/tmp/opencode/leo.sqlite` (51 MB, a
derived artifact, never in git) from `build/leo/leo.img`, stamped with its
sha256: the sweep disassembly (201k rows, indexed by mnemonic), every byte
offset of the image as a dword indexed by VALUE with an `aligned` column,
branch targets as xrefs, trailing immediates of instructions, the class
tables, printable runs, and the Ghidra entry names from
`build/decompiled/all/index.csv`. Commands: `callers`, `refs`, `callees`,
`dis`, `func`, `slots` (class-table slot census), `tables` (who installs a
table), `grep`, `sql`, `schema`, `find`.

It is a SEARCH index, not an authority. The instruction rows come from one
linear sweep, which drifts; overlapping rows are flagged `[SUSPECT]` and
every listing command prints the flag, but the only authoritative
disassembly stays `dis` (objdump over the exact bytes at the exact VA) and
`func` body ranges are entry-list hints. Its class-table detector is
mechanical - the `[obj+8]=T` / `[obj+0xc]=T-8` pattern read out of the
code, the PR-106 method - and measures 158 tables where the call-site
census measures 162, so `slots` prints counts over detected tables
(+0x60 = 0x23b80 in 129/158, and 26 distinct non-default targets, the same
26 PR-106 recorded). NEXT_STEPS.md, tooling addendum.

## Reference video (outside git; yt-dlp + ffmpeg + numpy + Pillow)

| file | what it does |
| --- | --- |
| `video_track_lion.py` | per-frame lion mask (colour) -> bbox/centroid, saved as `track.npy`. |
| `video_camera_shift.py` | per-frame global background shift by phase correlation (`cam.npy`). Drifts over long runs. |
| `video_jump_cam.py` | camera path + lion shorts/feet in world units for a short window: `video_jump_cam.py FILE.mp4 START_S DURATION_S`. |
| `video_shorts.py`, `video_feet.py` | the same measurements without the camera (`START DURATION`). |
| `video_stills.py` | camera-still windows of an R3 capture, drift in game px/s (PR-125). |
| `video_lane_shift.py` | band x-shift inside a still window -> game px/s (PR-125). |

The game picture is cropped from the 640x480 video as `crop=640:400:0:40`
(black bars 40 px top and bottom) and is the 320x200 world view shown 2x.
