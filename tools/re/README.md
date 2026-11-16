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

## Disassembly

`make_text_asm.sh` - objdump of the LE image's `.text`; VA = file offset + 0xf000.

## Reference video (outside git; yt-dlp + ffmpeg + numpy + Pillow)

| file | what it does |
| --- | --- |
| `video_track_lion.py` | per-frame lion mask (colour) -> bbox/centroid, saved as `track.npy`. |
| `video_camera_shift.py` | per-frame global background shift by phase correlation (`cam.npy`). Drifts over long runs. |
| `video_jump_cam.py` | camera path + lion shorts/feet in world units for a short window: `video_jump_cam.py FILE.mp4 START_S DURATION_S`. |
| `video_shorts.py`, `video_feet.py` | the same measurements without the camera (`START DURATION`). |

The game picture is cropped from the 640x480 video as `crop=640:400:0:40`
(black bars 40 px top and bottom) and is the 320x200 world view shown 2x.
