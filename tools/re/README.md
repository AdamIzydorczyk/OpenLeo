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
| `mov_channels.py` | `mov_channels [DIR]` - per state, the header `F=`/`T=`/`G=` pairs plus the sums of the row-level `F`/`FC`/`I`/`IC` pairs, i.e. the two motion channels the engine merges (NEXT_STEPS PR-98); decodes every state's `control=` word into the engine's own mode+flag bits and counts the modes and tokens (NEXT_STEPS PR-99); counts the `T=` values over the 130 cap the engine's parser applies; lists every force the BY_X states pair with more than one duration, which is the fixture that would separate an F law from an F/T one; and (NEXT_STEPS PR-100) censuses the second field of every row - the threshold the engine at 0x52320 compares against the row accumulator `[obj+0xa1]` - grouped by movement mode, which is where the "a row threshold is in its own mode's feed unit" reading comes from (94% of the BY_TIME values are exact multiples of the 100 the engine adds per step, the BY_X values are small counts of px). Reads `/tmp/opencode/mov` by default. |

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

The game picture is cropped from the 640x480 video as `crop=640:400:0:40`
(black bars 40 px top and bottom) and is the 320x200 world view shown 2x.
