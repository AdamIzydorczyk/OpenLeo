# HANDOFF - state of the Lew Leon native port and the plan from here

Written 2026-11-16 at commit `8769434` (PR-74) for whoever continues - a person
or a smaller model. Read sections 1-4 first (15 minutes), then pick ONE task
from section 8. Everything else in this repo (NEXT_STEPS.md = per-PR record,
PLAN.md = early milestones, VERIFICATION_2026-11-16.md = audit) is history;
this file is the map.

## 1. What this project is, and where it stands

A native (Linux + Windows, SDL2) port of the 1997 Polish DOS game "Lew Leon"
(Longsoft). The original game files are NOT in git; the owner supplies them.
The goal is a faithful, playable port whose every claim about the original
is traceable to its machine code, its data, or a reference capture.

What exists and works (all verified by `make verify-all`, section 3):

| area | state |
| --- | --- |
| containers (`ANIMACJE`, `ETAPY`, `SOUND`, `LEVELDES`) | read byte-exact vs a python reader |
| movies (`.IS`, 15 files) | decoded and played; codec twins in C and python; two real bugs fixed in PR-55/56 (LUT1 polarity, Mode X EOB); data-free synthetic test `verify-is-synth` |
| `.SEQ` sprites | two row codecs, anchors, frame-number grammar (1-based, negative = mirrored), shape margins; oracle over 1656 payloads |
| `.PCM` parallax layers | format decoded (tile-opcode overlays), oracle 23/23 |
| music (`.MOD`) | played; parse oracle 22/22; the mixer has NO independent oracle |
| title menu + chapter map | drawn from data (11 chapters, titles from `LEVELnn.SEQ`, arrow colours); progress/"start" variants not done |
| level scene | 320x200 world view shown 2x; world size from `back_rect` or the reference layer; per-layer parallax (`speed=`, `main_back=`) for PCM layers, the PCX backdrop and every object; HUD from `BELKA*.DES` |
| objects | 827+ placements, art by each state's `seq=`, `.MOV` state machines (loop until a transition fires; `GEN K_GENn`, `AC_BREAK_NEXT`); PR-93: the 3-comma static image lines (`<LEAF>,<name>.seq,x,y` -> that CD leaf's `.SEQ`, drawn at the authored x,y behind the actors) and the 0,0 entries = attached children (joined+counted, never instantiated at the world origin); PR-107: a `TARGET(N)=<cond>` line carries an authored START ROW and the target state is entered at that row (same rule for the lion and scene objects) |
| lion (play mode) | walks on the level's own ground (`.PLT`) with his current ANIMATION's shape-box bottom as the contact line (PR-97: `ll_player.contact`, `ll_seq_shape_contact`, `mov_state_contact`), jumps (ESTIMATED units), keys facing-relative (Model M, PR-85), camera follows (lead + grounded contact-line-at-178 anchor), HUD shows energy/lives |
| collision | push-out ported from the machine code (`FUN_00042de0/42cf0`), solid/semi-solid response, enemy contact damage from the placement files, the enemy's own ATC-window attack when armed (PR-90, off unless `LEO_PLAY_DANGER=1`), balloon pickup; ON by default except RAN.01 |

What does NOT exist yet (the real gap to "a playable game"): combat
motion beyond the hits (the lion's attack landed in PR-89 and enemies
land through their own windows in PR-90 stage 1 - what is missing is
the engine's per-class AI driver and the attack/charge motion), the signal bus's
routing between objects and its engine-internal raisers (the bit table and
named `GEN`/condition wiring landed in PR-88), pickups/inventory,
ladders/vines, level flow (end of level, exchange between
TUNGA I and II), RAN's balloon mode, sound effects, save/progress semantics.
Section 8 lists them as tasks.

## 2. Ground rules (non-negotiable)

1. **Never guess data.** A claim about the original must match the machine
   code (`build/leo/leo.elf`), the data files, or a capture. Unknown things are
   counted/reported, or implemented as a clearly named ESTIMATE (section 6)
   that prints a log line and is listed in NEXT_STEPS. If you cannot decide a
   number, write down what is missing and stop.
2. **No game data and no game frames in git.** Not the CD files, not
   screenshots, not video frames, not the reference video. The reference
   captures themselves live in `reference/video/` (gitignored, section 3)
   so they survive a reboot; everything cut from them - frames, crops,
   scripts - stays under `/tmp/opencode/...`. Pins in tests are hashes/strings.
3. **Every behaviour change ships with a test** that cannot pass by the C and
   python twins agreeing on the same mistake: hand-derived expected values in
   a data-free self-test, and a mutation check (break the code, see it fail).
   The twins already hid two real bugs (PR-55).
4. **PR discipline** (one logical step per commit, "PR-NN" numbering continues
   at 75): `make` and `make win` warning-free; `make -k verify-all` EXIT=0 with
   the fixtures AND fixture-free (`DATA=/tmp/opencode/nope HD=/tmp/opencode/nope`
   -> EXIT=0 with named SKIPs); a new `LEO_*` hook is documented in README; a
   record in NEXT_STEPS.md (what was found, what was ported, what is
   estimated, what is open); push; check CI (`gh run list --workflow=ci.yml`).
   EXCEPTION (owner, 2026-11-17): for a **pure-docs commit** - only `.md`
   files, nothing under `.gitignore`, `Makefile`, `tools/`, `ported/` or
   `.github/` - `make`, `make win`, `make ll_level` and `verify-all` are
   NOT run; record the reason instead (which files changed, and why no
   leg can read them). `gh` still runs the suite on its own, so the CI
   result is quoted as usual. The owner may widen the exception later
   (e.g. to `.gitignore` edits); until then a `.gitignore` change is
   still gated in full, because a pattern there can hide a fixture.
5. `gh` token has no `workflow` scope: a commit touching `.github/workflows/*`
   cannot be pushed.
6. The owner supplies reference material (screenshots, video timestamps).
   Ask for a specific clip rather than inferring.
7. **This working tree is local.** Every build, test and measurement of
   this project runs on the machine that holds this repo (`make`,
   `make win`, `ll_level`, the python readers, `gh`). Do NOT use the
   remote-host skills here - `debian-server` (the `lin` wrapper) and
   `openwrt-router` (the `wrt` wrapper): those hosts hold neither this
   repo nor the game data, so a command run there checks nothing about
   the port. (`windows-handheld` / `win` is different and stays in use:
   ADAM-HANDHELD is where the WASAPI audio measurement of PR-34/35 was
   taken.)

## 3. Machine layout and commands

Paths on the development machine (edit for another one; tools hard-code them):

| what | path |
| --- | --- |
| repo | `/home/aizy/Projects/LL_RECOMPILE` |
| CD data (`DATA`) | `/tmp/opencode/iso_stage/LEO` (ANIMACJE, ETAPY, SOUND, LEVELDES, the `.IS` movies) |
| HD install (`HD`, scripts/placement text) | `/tmp/opencode/hd_files` (ANIMACJE, ETAPY, LEO.EXE, STATUS.LEO ...) |
| golden captures / owner screenshots | `/tmp/opencode/golden/` (`user_refs/` = the owner's screenshots; never in git) |
| reference captures | `reference/video/` (in the repo tree but **gitignored**, PR-111): the full playthrough `OvbbJTmp79I`, playlist ep.1 `xPiXBmPEdXg`, and the ladder clip `ladder-DomCTOtx2rg` - ids, URLs, formats and what each is good for are under "Reference captures" below |
| scratch cut from them | `/tmp/opencode/yt/` (frames, crops, measurement scripts; older `-f 135` copies of ep.1/ep.2 live there too) |
| disassembly | `/tmp/opencode/vis/text.asm` (make it with `tools/re/make_text_asm.sh`) |

### Reference captures (never in git, but kept on disk)

The three captures the port's footage-based claims rest on are stored in
`reference/video/` in the working tree, so they survive a reboot. Nothing
there is ever staged: `.gitignore` excludes the whole directory (the
videos, their `.info.json` and their `.description` are all copyrighted).
Fill any of them in again with the exact `yt-dlp` line under its entry.

**R1. `reference/video/xPiXBmPEdXg.mkv` - playlist ep.1, the standing
reference (it used to be `/tmp/opencode/yt/gp.mp4`).**
`https://www.youtube.com/watch?v=xPiXBmPEdXg` - "Let's play with Zax37: Lew
Leon (Leo the Lion) PL - Pierwsze Kroki", uploader Zax37, uploaded 2010-12-21.
414.82 s, h264 640x480 at 25 fps + opus, 41 034 044 bytes, matroska. It is
episode 1 of `PL6B7604DEF5B643F9` (ep.2 is `ti6IYfhh1bI`; the other ids and
the measurement method are in section 9).

    ~/.local/bin/yt-dlp --no-playlist --write-info-json --write-description \
      -f "bv*[height<=480]+ba/b[height<=480]" -o "xPiXBmPEdXg.%(ext)s" \
      "https://www.youtube.com/watch?v=xPiXBmPEdXg"

GEOMETRY, re-measured on this file (a 6 s window, t=314-320, per-row
motion profile): motion spans the full frame width - columns 0..639, no
pillarbox - and rows 40..439 (the profile is flat at rows 0..37 and
442..479, steps up at 40-41 and down at 439-440). So a GAMEPLAY view is
the whole width at rows 40..439, exactly 2x, no overlay: the old recipe
`crop=640:400:0:40` is exactly right, and section 9's method needs no
change for gameplay. One caveat found while measuring: that box is NOT
right for the other kinds of segment in the same file. In the window
t=118-124 the black bands reach to column 36 and row 65, and the frame
there is one of the game's own animated cutscenes (the `.IS` movies) - a
rounded cave-interior picture with the lion, a dog and a spotted big cat
around a table, on black; it measures 4:3 inside the game-view band and
whether that rounded picture is the movie decoder's whole frame or a
letterboxed copy of it is NOT pinned, and would be worth checking
against PR-55/56's movie decoder. The level title cards ("ZARIA II",
"ENA I / underwater journey") are black-background text, and their band
is not pinned either. So: measure the band per segment, never carry one
crop across kinds.
PICTURE IDENTITY with the old `/tmp/opencode/yt/gp.mp4`: 75 consecutive
frames decoded at t=300-303 from both files are md5-identical (mean abs
difference 0.0), both are h264 640x480 at 25 fps, so every measurement
ever made on gp.mp4 is a measurement on this file. The containers differ:
gp.mp4 reports 414.680 s, this one 414.821 s, the extra ~0.14 s is the
audio stream's length in matroska. Take timestamps from frame counts, not
from the container duration.

**R2. `reference/video/ladder-DomCTOtx2rg.webm` - the rung-ladder clip PR-109
decided the row unit from.** `https://www.youtube.com/watch?v=DomCTOtx2rg`,
481.633 s, vp9 576x360 at 30 fps + opus, 26 902 029 bytes. NOT the same
bytes as PR-109's `/tmp/opencode/yt/ladder.webm` (same total size,
different sha256), but the SAME encode of the same video: the extracted
video streams are both 17 868 921 bytes with identical codec parameters,
and 90 consecutive frames decoded at t=46-49 - the climb - from the two
files are md5-identical, mean absolute difference 0.0. The audio streams
differ. So PR-109's numbers (climb from about 46.6 s, off the top at
about 48.3 s, pose changes every 2-4 video frames) apply here unchanged.

    ~/.local/bin/yt-dlp --no-playlist --write-info-json --write-description \
      -f "bv*[height<=480]+ba/b" -o "ladder-DomCTOtx2rg.%(ext)s" \
      "https://www.youtube.com/watch?v=DomCTOtx2rg"

GEOMETRY: the whole 576x360 frame is the game view, at exactly 1.8x, with no
bars and no overlay (`crop` is a no-op; `scale=320:200` puts it back at game
resolution). 1.8x is not an integer, and 30 fps is the coarsest sampling of
the three - use it for period/sequence claims, not for positions.

**R3. `reference/video/OvbbJTmp79I.mkv` - the whole game, uncut (new in
PR-111).** `https://www.youtube.com/watch?v=OvbbJTmp79I` - "Lew Leon ( Leo
the Lion ) - DOS (Playthrough)", uploader NontimetisStreams, uploaded
2025-03-05, described as "Streamed 26.1.2024". 20389.02 s = 5 h 39 m 49 s,
h264 1280x720 at 60 fps + opus, 4 774 166 785 bytes, matroska. 95 views on
YouTube; it is the only long-form capture of the game the project has.

    ~/.local/bin/yt-dlp --no-playlist --write-info-json --write-description \
      -f "bv*[height<=720]+ba/b[height<=720]" -o "OvbbJTmp79I.%(ext)s" \
      "https://www.youtube.com/watch?v=OvbbJTmp79I"

GEOMETRY, measured (not taken from the uploader): the game's 320x200 view is
x 0..1151, y 0..719 - exactly 3.6x, no letterbox - so
`crop=1152:720:0:0,scale=320:200` puts it back at game resolution. 3.6 is not
an integer: 1 video px = 0.28 game px of position. 60 fps is the finest
sampling the project has - 16.7 ms, finer than the 28-33 ms frame-loop
iteration of PR-109, which is what makes this file worth having.

THE BAR ON THE RIGHT - KEEP IT IN MIND. The owner asked for this to be noted
(2026-11-17, quoted): "on comment in that 5+ hrs video is some bar on right
site, worth to note it to keep it in mind". It is real, and it is not game
art: x 1152..1279, a 128 px column, is the uploader's decoration and must be
cropped off before ANY measurement on this file. Measured twice over; both
agree.
- First pass: per-column temporal std over 2 s bursts falls from 19-21 to
  2.3-2.8 at x=1152, the same at t=120, 3600, 7200, 14400 and 20040.
- Stronger pass (40 consecutive frames, 0.67 s, at t=120, 7200 and 20040):
  the mean per-column temporal std drops from 12.3 to 0.1 at x=1152 and
  stays at 0.0-0.1 to the right edge, while columns inside the game window
  range 3-24. So those 128 columns carry no motion anywhere in the 5 h 39 m.
- It is opaque art, not a black bar: mean luma 128.9 and spatial std 65.0,
  byte-for-byte the same statistics at all three times, and 718 of 720 rows
  have luma > 8 (only the top and bottom row are dark). That is why an
  "all-black columns" test would MISS it - test for temporal std, not for
  blackness.
- The comment theory does not hold up: this video has no comments at all
  (`--write-comments` extracts 0, `comment_count` 0), and a keyword scan of
  the 34 comments on ep.1 (`xPiXBmPEdXg`) found no mention of a bar. So the
  note is the owner's own observation, recorded as such.
- Practical consequence for any other upload from this channel: do not
  assume the boundary is at 1152, find it - take ~40 frames from a few
  times in the file, compute per-column temporal std, and crop at the last
  column that moves. A plain per-column brightness or std scan can also
  mislead inside the game window (at t=7200 columns x 544..704 sit at
  std 3.2-4.0, low but clearly not a bar), so require "static at every
  sampled time", not merely "low".

WHAT IT SHOWS. The owner supplied a level-by-level time map for this
video (2026-11-17), transcribed exactly as given, one entry per line:

    0:00 zaria 1     7:77 zaria 2     13:17 ENA 1     26:27 ENA 2
    31:43 MENU      32:11 ZARIA 1     39:13 ZARIA 2   45:10 ENA 1
    43:59 ENA 2     1:14:32 LONA      1:22:28 SADES   1:23:36 MENU
    1:24:24 LONA    1:36:47 SADES     2:00:33 KADMAR 1
    2:23:42 KADMAR 2                  4:16:20 RAN
    4:23:31 KARA    4:31:36 TUNGA 1   4:42:29 TUNGA 2 5:37:54 MENU

Read it as approximate minute marks, not frames. Two entries need care:
`7:77` is not a valid mm:ss; and `43:59 ENA 2` sits BEFORE `45:10 ENA 1`.
That second one was read frame by frame: at 43:59 the lion stands on a
tree stump in jungle, with the same HUD art as at 0:00 and 43:30; 74 s
later, at 45:13, the game shows its own "ENA I / underwater journey"
card, and that level has a swimming bird where the lion was. So the ENA
I boundary is 45:13 (matching the map's `45:10`) and `43:59` is not an
ENA boundary as written. Check a boundary against a title card before
pinning anything to it.

WHY THE LABELS ARE CREDIBLE: the names are the game's own. The 13 level
blocks in `ETAPY` (census in the PR-46 game-wide measurement,
NEXT_STEPS.md:2016 and :2024-2029) are ZARIA.01, ZARIA.02, ENA.03, ENA2,
KADMAR.07, KADMAR2, KARA.07, LONA.05, RAN.01, SADES.06, TUNGA.11,
TUNGA2.12 and JUNCTION, and `LEVELDES` records are named after the
CHAPTER (`ZARIA1.DES`, `ENA1.DES`, `KADMAR1.DES` ...) while `ETAPY`
blocks are named after the stage - so "zaria 1", "ENA 2", "KADMAR 1" are
the engine's words, not a viewer's guess. Only `JUNCTION` has no entry in
the map.

WHAT WAS CHECKED AGAINST THE FILE: three boundaries were read directly
off the capture, and all three agree with the owner's marks to about a
second - frame 2353 (39:13) shows the card "ZARIA II", frame 2713
(45:13) shows "ENA I / underwater journey" against the map's 45:10,
frame 7233 (2:00:33) shows "KADMAR I / ancient mysteries" against
2:00:33. The detector below also flags a card-shaped frame at 790 s
(13:10), against the map's `13:17 ENA 1` - a statistical hit, its frame
not read. Twelve more marks were probed by reading the
brightest frame within +-25 s of each one (`/tmp/opencode/refcap/
sheet_marks.png`); none of those twelve happened to land on a card, but
their neighbourhoods agree with the map everywhere except one entry -
see the `43:59` note above. The rest of my independent evidence is a
sampling of one frame every 5 minutes (68 frames,
`/tmp/opencode/refcap/tiles/`, contact sheets `sheet_0.png` ..
`sheet_3.png`), which does not contradict the map either: penguins and
snow from about 75:30 (inside `LONA`), the KADMAR I card at 120:30, a
green rope-ladder level from about 280:00 to past 335:00 (inside
`TUNGA 2`).
Level starts carry the game's own full-screen title card, which is the
cleanest level-start marker the project has. There is a first-pass
detector for them (`/tmp/opencode/refcap/cards.sh` + `cards.py`: one
320x200 grayscale frame per 10 s of the 1 152 px game column - 2039
frames - then per-frame black/bright fractions), but it is a candidate
finder, not a verdict: it flagged 104 card-shaped frames in 68 windows
across the whole file, many of them plain fade-to-black, and it MISSED
the cards at most of the marks above. A card still has to be read frame
by frame.

WHAT IT DOES NOT GIVE YOU: YouTube itself carries no chapters and no
subtitles for it (`chapters` is null in the `.info.json`) - the map above
is the owner's, supplied out of band; it is one player's route at one
difficulty, not a set of deliberate measurements; the stream's own overlay
covers part of the frame (above); and it was captured through an
emulator, so anything about real hardware timing (the PIT, the display
rate) is out of scope, as it was for the other two.

`/tmp` is a tmpfs: after a reboot restage the data (recipe in NEXT_STEPS.md,
"restaging"). The reference captures are NOT in /tmp any more - they live
in `reference/video/` on disk; if they are ever lost, the `yt-dlp` lines
under "Reference captures" rebuild them (pip install yt-dlp).

Build and test:

    make                      # ll_cutscene (linux);  make ll_level  builds the level tool
    make win                  # Windows .exe (MinGW, static)
    make -k verify-all DATA=/tmp/opencode/iso_stage/LEO HD=/tmp/opencode/hd_files
    make -k verify-all DATA=/tmp/opencode/nope HD=/tmp/opencode/nope   # fixture-free
    make verify-play verify-level ...                                 # single legs

Headless run of a level (CI style; frames are dumped as PPM at the logical
320x200):

    env -u DISPLAY SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy LEO_HD_DIR=$HD \
      LEO_HELP_SECS=0 LEO_LEVEL_SECS=2 LEO_PLAY_SCRIPT="1:FWD 60:JUMP 63:FWD" \
      LEO_LEVEL_PPM=/tmp/opencode/f.ppm LEO_LEVEL_PPM_TICK=200 \
      ./ll_level $DATA ZARIA.01 --play --no-audio

`ll_level DATA LEVEL` takes DATA as argv[1] and the HD install via
`LEO_HD_DIR`; set `LEO_HELP_SECS=0` for headless runs. A live run closes itself after 5 s unless
`-t 0` is given. Live viewer: arrows walk,
Space jumps, Up/Down/Left/Right also pan in preview mode.

Pitfalls seen:
- The Makefile did not track `ll_play.h`/`llrs.h`/`ll_pcx.h` for `ll_level` until
  PR-75; if a header edit "has no effect", `touch` a `.c` file.
- `verify-win` has one flaky Wine leg (`win restart-confirm`); rerun before
  suspecting your change.
- peak `ll_level` RSS is ~1.1 GB (RAN.01); run one level at a time.

## 4. Architecture map

    ported/main.c            entry, CLI, demo chain, menu -> level hand-off (NEW GAME = ZARIA.01)
    ported/ll_menu.c         title menu + chapter map (reads LEVELnn.SEQ titles, arrow colours)
    ported/ll_level.c        THE LEVEL: wiring (.DES), layers, scene objects, play loop, HUD
                             (the big file ~4400 lines; selftests at the bottom of its first third)
    ported/ll_play.c/.h      .MOV parser (states, transitions, frame tails) + the lion's simulation
    ported/ll_seq.c/.h       .SEQ codec, anchors, shape margins, push-out (ll_seq_shape_push)
    ported/ll_pcm.c, ll_pcx.c, llrs.c, ll_mod.c, is_stream.c   formats
    ported/platform_sdl*.c, ll_view.c, ll_audio.c   the only files allowed to touch SDL (verify-arch)
    tools/*.py, *.c          python reference twins, oracles, self-tests (verify-* legs)
    tools/re/                RE and video helpers (scratch, see its README)
    Makefile                 every verify-* leg and its pins; README.md hooks list

Key data flow of a level (ll_level.c, function that begins "4) the authored
scene"): `LEVELDES` block -> key/value pairs (`des_get`) -> CD `ETAPY` block
(art: PCX/PCM/SEQ, HUD art) + HD `ETAPY` block (text: placement files,
`BELKA`, `.PLT`) + CD/HD `ANIMACJE` leaf tables (object art / `.MOV` scripts).

## 5. Facts established about the original (all with a source)

Machine code facts use virtual addresses (VA = file offset + 0xf000 in
`build/leo/leo.elf`). "FUN_" names are the addresses.

Level data
- Order: 01 ZARIA I, 02 ZARIA II, 03 ENA I, 04 ENA II, 05 LONA, 06 SADES,
  07 KADMAR I, 08 KADMAR II, 09 RAN, 10 KARA, 11 TUNGA I, 12 TUNGA II (no title
  image). `JUNCTION` is a hub screen. Map arrows are red 1-4, green 5-8, blue 9-11.
  Names in the archives: ZARIA.01/.02, ENA.03, ENA2, LONA.05, SADES.06,
  KADMAR.07, KADMAR2, RAN.01, KARA.07, TUNGA.11, TUNGA2.12.
- `LEVELDES` keys: `tlo` (layers back to front), `speed=x,y;...` (percent per
  layer, default 100*(i+1)/n), `main_back` (reference layer = the world),
  `back_rect`, `lew=class,art leaf,mov,des`, `lew_rect`, `pozycja` (spawn),
  `plany`, `zwierzaki/animacje/ani_plat` (comma lists of placement files; entry
  i is on layer i; each key makes one object list), `belka` (HUD file),
  `platformy` (ground `.PLT`), `muzyka`, `paleta`. `danger_power` is read by
  the loader but authored in no level.
- No `back_rect` (SADES, TUNGA2, ZARIA.01/.02): the world is the reference
  layer's own art (2240x700, 2560x800, 960x3000).
- Camera/view: 320x200 world units shown 2x (640x400 inside the 640x480 mode).
  Layer scroll: x_i = cam*X[i]/X[R]; y_i = cam_y*Y[i]/Y[R] + (H_i-200) -
  (H_R-200)*Y[i]/100 (FUN_00057460/57500/57530).
- Placement files: `class,dummy,script.mov,x,y` lines, then `damage=`,
  `bounds=x0,x1`, `scope=`, `energy=` lines for enemies.
- Placement grammar, the two extra forms (PR-93, census RE): the 3-comma
  `<LEAF>,<name>.seq,x,y` line places a STATIC image - the first field is
  the same class-named ANIMACJE leaf table as a 5-field entry, the second
  names the single `.SEQ` to draw at x,y (84 lines game-wide, 69 join on
  the CD; the 15 misses are all LONA.05 WODOSPAD2 asking elem26/27/29.seq
  the install lacks; HD ANIMACJE has no PICT/TABLICA blocks - this art is
  CD-only). And 5-field entries authored at exactly 0,0 (130 of them, all
  shipped) always follow another entry or its properties - they are
  children the engine spawns from their parent (the vulture's egg, the
  saw-fish on each log, drops on leaves, demo text), never placements at
  the world origin; the spawn trigger is unresolved.
- HUD (`BELKA`): `x,y` + `[N ]name.seq` items: energy head (10 frames), lives
  digits, tools icons (4), count digits, item frame, crown.

Engine (objects have a vtable at +8; the common slots. PR-94 caveat:
that slot table is the layout of the CORE 0x52000-0x55xxx object family
only - 111 factory-installed vtable bases exist; the answer-A/touch
pair keeps a constant +0x58 spacing in 56 of them but sits at different
absolute slots per family, and +0xd0 resolves to different targets
(hp-sub 0x54550 in 19, a 0x23xxx family's 0x23840 in 19, the generic
0x50ed0 in 10, enemy-AI 0x41180 in 8, lion 0x3dae0 in 5). Full census
in the PR-94 NEXT_STEPS record.)
- +0x10 message handler (`{next, kind, a, b}` nodes; Lew: 0x1e130 -> 0x50d60
  jump table; kind 9 = balloon heal +32 and the sender's signal bit; 13 =
  velocity add; 1 = displace), +0x3c "is Lew", +0x40 "is enemy", +0x44 flag
  word, +0x48 shape element, +0x4c rect, +0x50 facing, +0x68 position,
  +0x78 touch, +0x9c filter, +0xd0 hit(source, ., damage).
- Contact/answer protocol (PR-94, verbatim, all addresses in /tmp/opencode/
  vis/text.asm): the touch method 0x50ed0(obj, other, damage) gates on
  [obj+0x11a] type/NULL/[obj+0x126], keeps a per-object contact list at
  [obj+0xd2] (0x551f0 pops it; empty + [vtable+0x3c] -> [obj+0x106]=1),
  and applies damage ONLY when the [obj+0x102] invulnerability countdown is
  0 (sink 0x54550 `sub [obj+0xad],ebx`; else [vtable+0xe4](1)). The lion's
  contact dispatch 0x340b5 (id compare vs global 0xced04 = the player)
  stomps: hp[other]=-1 when feet land within 0..20 above its top and
  [lion+0x5c]!=2, else calls 0x50ed0(receiver=object, other=lion, 30).
  The answer protocol = 13-byte packets (12-byte named set + flag byte at
  +0xc; build 0x749a0, OR 0x49580, rep-movsd copy) from three vtable-only
  handlers: 0x50f80/0x55232/0x55130 - answer-A first runs the SOLID
  responder 0x53ae0 (raises AC_BLOK 0x53bc8 / AC_BLOK_Y 0x53c36, push
  0x52b10 kind 1 + velocity kind 13 depth*30, SEMI_SOLID special), then
  optional hit via [vtable+0xd0], then merges AC_ENEMY unconditionally;
  answer-C raises AC_ENEMY_ATC iff the routed hit actually reduced hp;
  answer-B handles ANTI_POCISK (hit with damage 0) and DANGER. The class
  name table at 0x90f3d holds the .des class tokens verbatim (ANITAB..
  BERLO) and the entry-parsing helpers 0x325b0/0x328c0 split with patterns
  pointing INTO it.
- Flag word (token parser 0x51b30..0x51dc3): SOLID 0x400, SOLID2 0x20400,
  SEMI_SOLID 0x40400, ANTI_SOLID 0x10000, ANTI_POCISK 0x80000, DANGER 0x2000,
  CHECK_ANI 0x40, CHECK_ZW 0x80, CHECK_LN 0x100, CHECK_LEW 0x200, CHECK_ALL 0x1c0,
  CH_DIR 0x800, INV_V 0x1000, CHECK_BASE_LN 0x4000, CHECK_ALL_PLANE 0x8000.
- Signals (TEventSet, PR-88): the name->bit function at 0x48000 maps the
  name block at 0x919ec..0x91c40 onto explicit bit constants of the
  object's set at [obj+0x60] (full table in HANDOFF 7.2 / ll_play.h
  LL_SIG_*; K_GEN1 = 54 CONFIRMS the PR-70 assumption). The condition
  grammar itself: clause parser 0x47d60 scans tokens with format
  ' %[^, \r\n\t]', folds a ',' run into ONE node bit-mask (AND - the
  engine's design matches PR-87's model choice), '|' starts a new node
  (OR), '!' negates, '*' = and-with-another-set (unused in shipped
  condition texts); K_ANY/K_NEVER (+AC_ aliases) are op special cases,
  not bits; unknown names print "TEventSet::match :Bledna operacja".
- Touch dispatch (FUN_0004b500, FUN_00052780): the mover's CHECK_ANI/CHECK_ZW
  pick the animacje/zwierzaki list; objects are tested only on the mover's layer
  unless CHECK_ALL_PLANE; the touch method (slot 0x78) runs the push-out
  (FUN_00053ae0 -> 00042de0) and then the hit effects (FUN_00055230).
- Damage: energy -= damage (FUN_00054550); enemies hit with their own
  `damage=`; DANGER objects with the level's `danger_power`. Lion energy starts
  at 100 (0x503ac), lives = global 0xa94f0.
- Frame tails in `.MOV` (the engine's directive token table at
  0x917c0..0x919c7, matched case-insensitively - the comparison at
  0x76a69 lowercases A-Z first, so the table's lowercase spellings match
  the data's upper ones): `atc_on`/`atc_off`, `lock`/`lock2`/`unlock`/
  `unlock2`, `ground_on`/`ground_off`, `chdir`, `ldir`/`rdir`,
  `pos i,i`, `goto N`/`gotorel`/`goto_rel N`, `I i,i`, `F`, `IC`, `FC`,
  `M i,i`, `MOVE`, `GEN` (names from the separate 0x919ec block), `Wait`,
  `end_level i`, sound words `smp i`, `smp2 i,i`, `smp3 i,i,i,i`,
  `smpauto`, `smpauto2`, `smpprior`, `vol`, `smp_loop`, `smp_unloop`,
  `smp_stop`; state/other words `rev`, `zero_v`, `zero`, `zero_t`,
  `vol_music`; an unknown token prints "Bledna komenda %s w pliku %s".
  Old note: `GEN <sig>` (any signal-table name - PR-88),
  `I vx,vy`, `F`, `FC`, `M dx,dy`, `WAIT`,
  `lock/unlock`, `ATC_ON/OFF`, `GOTO_REL`, sound words (survey in NEXT_STEPS PR-69).

Walk speed: the `.MOV` caps (T=90) match a measured ~75-125 units/s on video.

## 6. Estimates and assumptions in the code (revisit when evidence arrives)

| what | where | value / rule | evidence |
| --- | --- | --- | --- |
| jump impulse unit | ll_play.h `LL_PLAY_JUMP_DIV` | `I`/4 u/s | ONE measured jump (vine, ep.1 318.6 s); PR-86 re-confirmed on ZARIA live: A_JUMP `I 150,-1200` -> apex ~24 u / 0.36 s, matching the video's jump population (gp.mp4 feet-track scan: clean candidates 23-30 u over 8-13 frames) |
| jump gravity | `LL_PLAY_JUMP_G` | 1650 u/s^2 (= G.y 55 x 30) | same jump |
| jump-state ending | ll_play.c | a state that DECLARES an AC_BREAK_NEXT exit ends at its frame-list WRAP (the landing no longer cuts it); forced ->A_STAY/A_DRAP kept ONLY as fallback for states with no such line | PR-86: real A_JUMP/A_JUMP_S declare A_WALK(7)/A_DRAP(1)=AC_BREAK_NEXT; ZARIA.01 leg pins it |
| `TARGET(N)=COND` entry | ll_play.c `ll_mov_row_start` (used by BOTH the player path and ll_level.c `obj_transition`) | numeric (N) = start ROW (1-based) in the target's frame list; non-numeric = top; an N past the end clamps to the top | PR-86 evidence chain in the struct comment (PRE_JUMP skips A_JUMP's impulse row; A_WALK(7) mid-cycle re-join pinned on real data); PR-107 read the store (`entry-1` at 0x51e3b..0x51e49) and the use: the state record's live row field `rec+0x4`, written raw at 0x523cb by the row-setter, which is the record's own vtable slot +0x1c in the table every record stores at rec+0x44 (0xa25d4, from its constructors 0x524cf/0x52508), and which the transition step 0x535a2 calls on the new state (0x535ea). Two MODEL CHOICES, recorded in the helper's comment: the engine writes the value RAW there, so clamping an over-long row is ours; and "absent / non-numeric" is -1 = "leave the row alone" (0x51e3e, 0x52381), which only COINCIDES with the top because a row list wraps. One row counter per object (`o->fi`) vs the engine's one per state record is also ours |
| The unit of the row clock, and the BY_* mode feed (0 BY_X / 1 BY_Y / 2 BY_TIME / 3 BY_TX / 4 BY_TY) | ll_play.c `ll_player_step` sections 4 and 7 + `LL_ITER_MS` (ll_play.h), ll_level.c `obj_row_tick` / `obj_advance` | MODELLED since PR-110, as ONE accumulator per moving thing fed BY MODE, mirroring the dispatcher 0x543a2: a BY_X state with a non-zero state `F=` adds travelled distance (`|vx| * dt`) and its time accumulator stays at 0; every time mode adds 100 per ITERATION and never any distance, an iteration being `LL_ITER_MS` = 28 ms of wall time, fed through a whole-millisecond carry (`row_rem`) so no step size drifts; the engine gates mode 3 on `[obj+0xba] != 0` and mode 4 on `[obj+0xbe] != 0` (0x543f2 / 0x5441b, PR-108) and the port does NOT model that guard, nor any vertical force — a BY_TY state is simply time-fed, and the climb's vertical DISTANCE law stays open (T5). A row stays current while `acc < thresh`; the compare is AFTER the feed and the threshold is SUBTRACTED on a hit, so the remainder carries into the next row. `time_scale=` is a parse-time scale on the stored thresholds (`thresh * time_scale / 1000`, default 1000, rows before the line unscaled), NOT a duration | The dispatcher and its mode jump table are PR-99's (NEXT_STEPS.md:5439-5447); the feed constant 100 has exactly one writer, 0x4c8b7 (PR-100), and the row comparison at 0x52320-0x5232b hands `acc - thresh` to the row handler rather than zeroing the accumulator (PR-100). PR-108 read the mode-3/4 guards above and that `[obj+0xbe]` is also v_y's force register (0x542cd). PR-109 decided the unit from the owner's clip (the `[A_WSP]` rows of 200/300 measure 66-100 ms on screen, the pacer 0x4b0e0 -> 0x4a120/0x1dba0 holds one iteration to two display frames, the engine's own fallback setpoint is 27) and corrected PR-106's "55-unit steps" and "sound driver's 70.0 Hz PIT" (that write is 7.00 Hz; during play the counter is a real 1 ms clock, 0x5b9e0 set-period called at 0x5d195); PR-110 takes 28 ms, the low end of that bracket, and implements the mode split. OPEN, all recorded in the PR-110 record: whether a row fires AT `acc == thresh` (13932 of 14881 authored BY_TIME thresholds are exact multiples of the 100 feed); the engine admits ONE row per record step, the port loops; the engine's mode-0 feed is the TRUNCATED `v_x/100` of the tick while the port feeds `|vx| * dt`; the `fx != 0` half of the walk gate (30 shipped BY_X blocks with `F=0,0`, no leg covers one); the AC_TIME pending-flag lifetime; the climb's vertical DISTANCE law. PINNED by: the RC fixture walked on two grids with the exact carry identity and `facc` read at every step (0 in a BY_TIME state), the `time_scale=2000` leg plus its control, the ZM leg reading both accumulators at both ends of the mode gate (no key in a BY_X state: everything stays 0; key held: the time accumulator stays 0 while the list runs on distance), 7 mutations caught, plus one recorded survivor (`mut_nogate4`, section 4's feed-site condition removed: `facc` is only read through the walk gate, so no leg can see it), and every verify leg re-derived by hand (the derivation is written beside each leg in the Makefile) |
| jump key | ll_level.c | Space | viewer choice |
| fire keys | ll_level.c `play loop` + platform_sdl `map_key` | Ctrl=K_FIRE, F1..F4=K_F1..4 (weapon selectors), Alt=K_ALT; script tokens FIRE/F1..F4/ALT comma-combinable | PR-87; names from the walkthrough, the .MOV conditions are the contract |
| `,` in transition COND = AND | ll_play.c `parse_cond`/`cond_true` (groups) | `|` ORs groups, `,` ANDs tokens within one; OTHER poisons only its group | PR-87 MODEL CHOICE (evidence in ll_play.h: AC_ONAIR/AC_DRECT pairs, every K_FIRE,K_Fn attack line); A_FIGHT entry pinned real |
| hurt lock | ll_level.c `play_collide` | 30 ticks of 20 ms | 0x50304 constant, unit assumed |
| `AC_TIME` | ll_level.c `obj_cond_true`, ll_play.c | obj: = end of frame list OR a raised bit; lion: raised by frame `GEN AC_TIME` (bit 9 = LL_SIG_AC_TIME) | `time=min,max` unit unknown |
| signal bits | ll_play.h `LL_SIG_*`, `ll_sig_id()` | engine table 0x48000: AC_BLOK 0..AC_EV_MAX 68, K_GEN1 54; `GEN <name>` raises any, conditions test the object's own set | PR-88 (verified constants); pulse-vs-latch + cross-object routing OPEN |
| lives / energy HUD | ll_level.c | 6 lives, energy frames (100-e)/10 | video start shows 6 |
| camera | ll_level.c `follow_camx/camy` | lead 100/220, ease 6 u/tick, grounded contact-line-at-178 anchor (PR-97: the anchor point is `p.y + p.contact`, the shape-box bottom of the animation being shown (max over its state's frames) = the paws PR-84 measured; y-ease 2 u/tick invented) | video + owner frame comparison (PR-84) |
| contact pass default | ll_level.c | on except RAN.01 | RAN is the balloon level |
| SEMI_SOLID | `play_collide` | only the r/5 shift | velocity add (kind 13) unit unknown |
| SOLID push polarity | `obj_shape_elem` | shape uses the DRAW polarity incl. CH_DIR facing | consistency, not decompile-pinned (PR-81) |
| player facing model | ll_play.c `face`, ll_level.c `play_draw/play_collide` | face follows walkdir only in CH_DIR states; draw/shape = authored XOR face, face sticks | flags real (LEW census, PR-82); the always-XOR rule for no-CH_DIR states is a model-consistency choice, NOT decompile- or video-pinned (HANDOFF Q0) |
| K_FWD/K_BKWD key model | ll_play.c `cond_true` (MODEL CHOICE note) | facing-relative: K_FWD = arrow along current face; walkdir = the literal arrow; a left-facing turn settles via the state pair or A_TURN+AC_BREAK | PR-85; forced by data (one A_WALK, no BKWD-walk state); NOT yet confirmed on video |
| lion attack damage / object hit rule | ll_level.c `LL_LION_HIT_DAMAGE` 30, `obj_hit`, `LL_OBJ_HIT_CD` 14, `ll_play.h` `atc`/`atc_hit` | damage 30 = the 0x34103 dispatcher default; hp<0 forces A_DEAD by name; energy -1 never hit-able; countdown 14 (0x27264); window open from `ATC_ON` until `ATC_OFF`, one hit per window, cleared on state change for the 18 blocks that never close it | addresses verified; the 30 stays until the factory-template id array (0x44350 -> ds:0xcecf0..0xced04; the proportional hit at 0x34037 is energy*30/100+10, NOT *45 — see PR-96) maps to a .des type; the window-clear rule and the corpse-deal-contact rule are model choices (PR-89) |
| enemy attack (T3 st. 1) | ll_level.c `obj_arm_atak`, `play_collide` window block; `ll_play.h` `LL_CTRL_DANGER` | a current state carrying the authored DANGER flag arms the script's OWN A_ATAK by name when the lion is inside abs(dx) < 40 AND abs(dy) < 50 (strict); the enemy lands ONE hit through its own ATC window (authored damage=, 30-tick lion lock) before/instead of the contact block; OFF unless LEO_PLAY_DANGER=1 | the box numbers are RE-pinned FOR THE CLASS "LICHA" (class chosen from the .des token at 0x616d0; box at 0x61b6e) - the flag-as-precondition, A_ATAK-by-name entry and window-hit mirror are the port's stand-in (engine driver open, §7 item 10) (PR-90) |
| AC_ENEMY_ATC raiser | ll_level.c `obj_hit`, `play_collide` window block | the hit raises it on the HIT object's own set (PR-88 mechanism) so its `A_DOSTAL(1)=AC_ENEMY_ATC` line fires; PR-91 mirrors the raise onto the LION for the enemy's WINDOW hit (never contact) - his authored A_DOSTAL chain then runs itself | INFERENCE (0 of 279 shipped GEN lines raise it; the combat code must be the raiser); NOW PINNED (PR-94): answer-C 0x55130 raises bit 16 (byte2 |= 1, 0x5518c) exactly when the routed hit reduced the target's hp, and bit 7 (0x551b0) when it did not; answer-A/B merge {bit 7} unconditionally (0x51050/0x52be). The plain contact method (0x50ed0) raises NO signal - matching the port's "contact does not raise bit 16" |
| AC_ENEMY raiser (PR-92) | ll_level.c `obj_touch_enemy`, called from `play_collide` (solid loop + post-loop non-solid pass) | the lion's touch raises bit 7 on the TACHED object's own set and its OWN table reacts (raise-consume like the balloon path); gated by `hit_cd` (the [obj+0x102] countdown), default-on like contact | PINNED raise sites (PR-94 verbatim): answer-A 0x51050 and answer-B 0x52be merge {bit 7} unconditionally into a 13-byte answer packet (12-byte set + flag byte) copied to the answer buffer ARGUMENT (0x51062..0x51083); the answer handlers 0x50f80/0x55232/0x55130 are separate functions reached only through family vtables (0x50ed0 rets at 0x50f79 and raises nothing); which object's head finally reads the buffer is OPEN, so feeding the object's own set is MODEL, the second non-solid pass is data-forced (no consumer state carries SOLID); a pure touch NOT setting hit_cd=14 is an open item (item 10) |
| static image draw + order (PR-93) | ll_level.c `obj_static_get`, `objs_draw` (two-pass) | frame-one draw: first patch-art element of the named .SEQ at the authored x,y through the element's anchor; statics compose BEFORE animated objects | the join and positions are RE (census in the code comment); frame-one (no motion data exists) and the behind-actors order (matches the ZARIA footage) are model choices; whether the original animated any static is open (item 11) |
| shape payloads for lion-vs-object tests | ll_seq.c `ll_seq_shape_push2`, ll_level.c `play_collide` | each element's margin lists read from its OWN .SEQ payload (paired form) | model choice: how the original resolved two elements into its single-payload routine is unpinned; same-payload pairings stay byte-identical (selftest-pinned, PR-89) |

## 7. Open questions (ranked by how much they block)

0. SCENE OBJECT MOTION (SOLVED in PR-79, 2026-11-16; basis re-checked
    2026-11-16 with direct vision - model unchanged, claims relabelled):
    the 640x480 gameplay video IS a uniform 2.0x of a 320x240 game
    screen (re-verified by eye: the on-screen worm is 38-40 game px =
    its 38 px art x 2.0 on a 1-game-px grid; 2.4x is visibly too big).
    DATA-INTERNAL (no video): the frame list's 2nd field is MOVEMENT
    UNITS per animation frame for BY_X states and ms elsewhere - the
    same LICHA3 .MOV authors 4 in its BY_X A_RUN list and 400/200 in
    its BY_TIME states. (RETRACTED by PR-83: the 2nd field is ms in
    BOTH engines - the player scripts prove it, e.g. ZARIA A_WALK rows
    "10,6" while T=20 px/s does the moving; the 4-vs-400 observation
    stays as data, not as a motion unit.) Model: speed = |F|/4 game
    px/s, frames advance on the row clock (PR-110: that clock is the
    engine's ITERATION clock, and a BY_X state's accumulator is fed by
    TRAVELLED DISTANCE instead - see the section 6 row above),
    bounds= patrol
    flip, no time consumed while walking;
    BY_X with F=0 falls back to time-driven frames without moving
    (labelled estimate). CORROBORATED on video: the ZARIA.01 worm's
    walk bursts average ~20-30 px/s with F=100 -> 25 px/s expected,
    pause-then-walk bursts consistent with A_RUN vs A_ATAK state
    gating; its 9x4-unit stride = 1.44 s matches the observed pose
    rhythm; walk range consistent with the authored bounds. WITHDRAWN
    (re-check): the per-frame "worm 3.0-3.75 px/frame / rhino 7-10 px"
    numbers (pose deformation tracked, not locomotion), the camera-band
    shift speeds (parallax + animation; the ground layer must be the
    reference, and automated tracking there failed), and the "lion
    25 px/s via camera follow" anchor. bounds= key is authored patrol
    data. SOLVED in PR-81 (the census lead is closed): walkers DO turn
    to face their direction. The frame-number sign is the authored
    BASE facing (ZARIA.01's worm art faces left - eye pixels 11-20%
    from the left edge in all 9 poses - and its walk list is authored
    all-NEGATIVE, so the game draws it head-right), and CH_DIR is what
    turns that facing at the bounds flip: bounds= appears only on
    placements with a CH_DIR state (8/8 vs 0/14) and matching the art
    in BOTH polarities on video, the winning polarity flips with the
    travel direction (head-right at t=204.4/211.2/212.4, head-left at
    t=206.9/208.0/213.2; both at x~330 one second apart = one instance
    turned around). Initial walk direction +1 is therefore NOT an
    estimate any more: it is what the authored sign draws. T's
    semantics stay unresolved (ll_play.c's older "walk at +/-T cap,
    F=accel" reading still contradicts the object data and must be
    revisited in T1).
    EXTENDED in PR-82 (owner request "all moving enemies and the lion
    too"): facing is now instance state for EVERY mover - obj_walk sets
    it from the travel direction on every step (same result today),
    and the LION got the same field: ll_player.face is set from
    walkdir only while a CH_DIR state runs (census: every LEW state
    carries CH_DIR except the direction-baked ones - A_GO_TO_BALON_R
    authored +1.. vs A_GO_TO_BALON_L authored -1.. with NO CH_DIR,
    A_WALK_FWD/BKWD carry identical all-positive lists + CH_DIR), and
    play_draw/play_collide draw+collide him authored-XOR-face, exactly
    like objects. Pinned by a new verify-play leg (BKWD-tap back to
    A_STAY at tick 2: same state/frame/pos in both logs, only the
    lion's sprite redraws flipped - tools/ppm_flip_check.py) and by
    --play-selftest. STILL OPEN (unresolved either way, both readings
    agree everywhere we can test): for a state WITHOUT CH_DIR, does
    the engine still XOR the instance facing (the shipped choice,
    consistent with the one video-pinned case whose states all carry
    CH_DIR), or is the authored sign absolute there? Settling test: a
    dual-polarity template match on the RAN crown/balloon cutscene if
    any recording has the lion entering A_GO_TO_BALON_L while last
    facing left, or any left-facing STANDING lion on video, or the
    worm standing still after a turn (PR-81's test). The ZARIA.01
    recording cannot answer it: montages over its whole 204-386 s span
    show only right-facing walking (rightward walkthrough), and the
    115-160 s part of that video is a different demo, not Lew Leon.
    SEQ_REV_X (a control name not ported) appears only on the 7
    A_TURN states, whose frame lists flip polarity mid-sequence = the
    turn-around animation; how it drives facing mid-sequence is T1/T3
    work.
    The former hypotheses (a)/(b) are both confirmed: (b) landed in PR-78.
1. F/FC frame directives (jump distance; also enemy/projectile motion) -
   parser evidence now narrows this: `F` and `FC` each parse an `(x,y)` pair
   into a directive record; the handler accumulates both coordinates into
   two caller-provided integers when its selector matches. Keep frame-tail
   `F x,y` distinct from the state header `F=x,y`: the latter is parsed into
   a physics-state vector beside `T=` and `G=`. The engine packs those three
   state vectors into a callback context and has a separate output pair for
   frame directives; how that output pair changes world motion remains
   unresolved. Do not port a velocity or impulse interpretation yet. Evidence
   and addresses are in the PR-76 investigation entry in NEXT_STEPS.md. Data
   statistics: FC values 20..120 (100: 17, 90: 14), F values e.g.
   `F -300,0,GEN K_GEN3` (enemies), `F 0,350`, `F 1200,-300`.
2. The signal-bit table: RESOLVED in PR-88, ported as `LL_SIG_*` +
   `ll_sig_id()` (ll_play.h/ll_play.c). The name block is at
   0x919ec..0x91c40 (not 0x92180 - that is the control= flags); the
   name->bit function is at 0x48000 (not 0x51b30) and sets an EXPLICIT
   constant per name: AC_BLOK 0, AC_BLOK_Y 1, AC_BREAK_NEXT 2,
   AC_BREAK_PREV 3, AC_DUP 4, AC_OFF 5, AC_DEADDUP 6, AC_ENEMY 7,
   AC_EDGE 8, AC_TIME 9, AC_DRAB 10, AC_DRABK 11, AC_DRAB_CATCH 12,
   AC_DUP_CD 13, AC_STONE 14, AC_PUSH 15, AC_ENEMY_ATC 16, AC_RECT 17,
   AC_ONAIR 18, AC_DRECT 19, AC_DRECT2 20, AC_LINA 21, AC_LINA_CATCH 22,
   AC_LEAVE 23, AC_ATAK 24, AC_UP_DUP 25, AC_SPOKUJ 26, AC_BUTLA 27,
   AC_BECZKA 28, AC_START 29, AC_MRECT 30, AC_TABLICZKA 31, AC_KILOF 32,
   AC_KOREK 33, AC_KOLO 34, K_BKWD 35, K_FWD 36, K_UP 37, K_DOWN 38,
   K_FIRE 39, K_ALT 40, K_F1..K_F12 41..52, K_JUMP 53, K_GEN1 54..
   K_GEN5 58 (the PR-70 "bit 54" assumption CONFIRMED), AC_PLAT_OFF 59,
   AC_PLAT_OUT 60, AC_XCHG_LEVEL 61, AC_SHOW_CROWN 62, AC_MSG_PUSH 63,
   AC_WALK_UP 64, AC_WALK_DOWN 65, AC_APPEAR 66, AC_DISAPPEAR 67,
   AC_EV_MAX 68. K_ANY/AC_K_ANY and K_NEVER/AC_K_NEVER are NOT bits:
   the clause parser at 0x47d60 special-cases them into node op tags
   ('!' negate, '*' and-set, '|' separate node = OR); a ',' keeps folding
   bits into the SAME node mask - the engine's own design behind PR-87's
   OR-of-AND reading, and its token format is ' %[^, \r\n\t]' exactly as
   we parse. STILL OPEN inside this area: (a) who ROUTES a signal across
   objects (ENA2's raft frames raise AC_PLAT_OUT on the RAFT's set while
   the LION's A_JUMP_PLAT(1)=AC_PLAT_OUT tests the lion's - a message or
   broadcast connects them, unresearched); (b) who raises the AC_*
   engine-internal bits (AC_ENEMY_ATC 279 conditions, AC_ONAIR 211: no
   frame GENs them - physics/combat code must) - PARTIALLY ANSWERED for
   AC_ENEMY_ATC in PR-89: the hit code raises it on the HIT object's own
   set so its `A_DOSTAL(1)=AC_ENEMY_ATC` line reacts (flagged INFERENCE
   in ll_level.c; the raise on the LION when he takes contact damage is
   deliberately not shipped - it needs its own step and would re-pin the
   walking fixtures); AC_ONAIR and the rest stay un-raiser'd; (c) whether
   the set is ever CLEARED (ours pulses one tick, the engine's OR is
   latching; no shipped data discriminates yet).
3. `AC_DUP`, `AC_ONAIR`, `AC_DRECT`, `AC_EDGE`, `AC_DRAB_CATCH`, `AC_ENEMY_ATC`,
   `AC_BLOK`: meaning (they gate jumps, falls, ladders, enemy attacks).
   PR-94 found the ENGINE-side raiser of `AC_ENEMY_ATC` (bit 16):
   answer-C 0x55130 raises it (byte2 |= 1 at 0x5518c) exactly when a
   routed hit REDUCED the target's hp; the "no hp change" branch raises
   AC_ENEMY instead (this complements PR-91's lion-side raise - the
   per-hit decision rule is now pinned). The object-side consumer is
   still unmodelled. `AC_BLOK`/`AC_BLOK_Y` (bits 0/1) got their engine
   raisers in PR-94: the SOLID responder 0x53ae0 raises them into the
   answer packet (0x53bc8/0x53c36) - the port's shape-contact model is
   the same event; their condition-side use by object scripts stays
   unmodelled.
4. `danger_power` default: CLOSED by PR-94. The DANGER branches
   (0x3da7c/0x5102b/0x5529f) all read [[ds:0xcef40]+0x54], and
   0xcef40's config parser (0x325b0) WRITES it zero (0x3266b) while no
   config key ever sets it: shipped DANGER-route hits carry damage 0.
   (The PR-90/91/92 model is unaffected - authored enemy damage comes
   from the placement `damage=`, not this field.)
5. `STATUS.LEO` field meaning (shipped file is a fresh save: no progress); the
   map's progress arrows/"start" variant depend on it. Needs a paired capture.
6. The camera's catch-up rate and dead zone (measure with a clean clip).
   Anchor SOLVED in PR-84 (grounded contact-line-at-178 - PR-97 makes the
   anchor point explicit as `p.y + p.contact`, the shape-box bottom of the
   animation being shown (max over its state's frames), which is the paw line
   PR-84 measured; view holds while
   AIRBORNE - the airborne model choice is NOT settled: PR-73's "the
   view rose 45 u" during a ledge jump points at a slower follow; both
   readings live in the follow_camy comment).
7. Vertical `y` camera of the PCX backdrop uses the engine formula now; confirm
   against a capture of a tall level (SADES shaft). PR-84 note:
   for the NO-back_rect levels the camera clamp = reference-layer height - 200
   (PR-65 rule) matches the original exactly (ZARIA.01: 700-200=500 = the
   original's camy on the ground) - the clamp source there is settled.
8. PLAYER MODEL details PR-83 implemented but did NOT settle (all flagged
   in code; the two user-reported bugs themselves are fixed and pinned):
   - `time=<n[,m]>` state field: sits next to AC_TIME transitions, values
     read like ranges (32,64 / 90,120 / 220 / 350). Acting on it as ms was
     WRONG (it pulled ENA.03's lion out of A_SWIM two ticks after spawn),
     so it stays parsed-and-counted only. The only proven AC_TIME source
     is the frame `GEN AC_TIME` tail (ZARIA's A_WALK names it on 11 frame
     rows). Settling RE: a state with time= and no GEN frames whose exit
     is visible on video - ZARIA A_DRAP time=350 -> A_TO_BOR -> A_BORRING
     is exactly that test.
   - Spawn state rule shipped is "A_STAY where present, else the .MOV's
     declared first name, else block 0". Every one of the 13 player .MOV
     declares its level's start pose FIRST (ENA A_SWIM, KADMAR2
     A_MASZERUJ, JUNCTION A_IDLE, SADES A_STAND, ZARIA/TUNGA A_WALK;
     TUNGA's old A_GIVE_EGG-at-spawn pin was a block-0 artifact -
     A_GIVE_EGG is a K_FIRE,K_F6 action state), but RAN declares
     A_FLY_PTAKI1 (a glider) first while its pinned spawn stands in
     A_STAY. Does the original force standing, or start at the declared
     name? Settling test: a RAN.01 level-start capture.
   - K_FWD/K_BKWD: RESOLVED AS MODEL CHOICE in PR-85 (recorded in
     ll_play.c cond_true as MODEL CHOICE): FACING-RELATIVE (K_FWD = the
     arrow along the current facing). Data proof: ZARIA authors ONE walk
     state (entered only via K_FWD) and no backward-walk state - literal
     keys make left-walking impossible, while video obviously shows the
     player walking left (PR-72's facing-left band 200-230). The
     AC_BREAK_NEXT exit at the A_TURN list wrap is the player's turn
     completion (real .MOV lists only AC_BREAK_NEXT/PREV as A_TURN
     exits); AC_BREAK_PREV stays unmodeled (false). Still UNPROVEN on
     video: the settling test (facing-left lion + RIGHT key on a
     capture) would CONFIRM rather than deny - keeping it listed.
   - `I ix,iy` impulses multiply by walkdir (A_TURN entered via BKWD
     slides left; probe and the verify leg agree with that). Nothing in
     the original directly confirms the mirroring.
   - F=0 states FREEZE momentum (dv clamps to 0): no coast, no decay.
     Whether the original coasts at F=0 is open.
   - K_FIRE / K_F1..K_F4 / K_ALT: PORTED IN PR-87 (ll_play_keys fields;
     live keys Ctrl / F1..F4 / Alt - viewer names from the walkthrough;
     script tokens FIRE/F1..F4/ALT, comma-combinable "FIRE,F1";
     LEO_TEST_KEY names ctrl/alt/f1..f4). The comma GROUPS they appear
     in are now AND (MODEL CHOICE, argued in ll_play.h: `AC_ONAIR,
     AC_DRECT`-style pairs and every `K_FIRE,K_Fn` attack line are
     meaningless as OR/no-op) - A_FIGHT is entered+played on real
     ZARIA data (zfight leg + negative FIRE-alone leg). Still open
     there: A_FIGHT's ending (its own AC_BREAK_NEXT wrap works; what
     AC_ENEMY_ATC/ATC_ON "attack hit" mechanics do = T2), K_F6..F12
     stay OTHER (7 uses total), and whether the original treats F-keys
     as HOLDED (our model) or TOGGLED weapon state (unproven; the
     comma-gate behavior is identical for held-vs-selected as long as
     one weapon is always current).
   - "Jumps are very low" (owner report, same session as the position
     one): NOT reproduced on the current binary. Measured honestly: our
     A_JUMP/A_JUMP_S apex ~24 u, air 0.36 s (live probes ZARIA.01) vs
     the original gp.mp4 feet-track (clean jump candidates 23-30 u over
     8-13 frames) and vs the authored I/4 + g=1650 arithmetic (~27 u).
     The game's jump IS a ~12%-of-screen hop. If the owner sees lower
     jumps, most likely an older binary again (as with the sitting-low
     report) - ASK/verify before touching the constants; PR-86 pinned
     the height instead of changing it.
   - What still ends the jump EARLY, if anything: PR-86 kept the
     authored behavior (state runs its list, wrap exit). The undecoded
     `WAIT 0ms` rows + `AC_DUP` self-lines (A_JUMP(11)=AC_DUP,
     A_JUMP_S(9)=AC_DUP) are the engine's obvious "loop the tail while
     the player waits / chain another press" mechanism, and the
     900 ms A_JUMP recovery row plays out grounded in our model - if
     the original cancels recovery on the next input, AC_DUP is where
     that lives. Settling: a landing-into-immediate-walk on video, or
     the decompile's frame-tail handler. The gp.mp4 feet-track scan
     (10367 frames, /tmp/opencode/feet_track.pkl) has the candidates
     for that test (clean short lifts 23-30 u, e.g. idx 6347/6376/
     6430); several raw candidates are camera/terrain artifacts and
     need per-window checking first. F=/FC= accumulator tails stay T1.
9. PR-89 OPEN ITEMS (each flagged in code too):
   - How the original resolved TWO .SEQ payloads into the ONE payload
     its shape-collision routine (FUN_00042de0/42cf0) takes: one shared
     address space at load time, or per-record blocks like the port.
     Model choice shipped (ll_seq_shape_push2 reads each element from
     its own record; same-record pairings stay byte-identical).
   - The 18 .MOV state blocks that open the ATC window (ATC_ON) and
     never close it: what clears it in the engine? The port clears on
     state change (model choice; "attack hits forever after one swipe"
     contradicts the video).
   - Which factory template instance is which .des TYPE. PR-96 traced the
     structure: these ids are a STRIDE-4 startup array built by one routine
     (~0x33bc..0x33c4a) that calls a factory (0x44350) repeatedly and stores
     each returned type-id into ds:0xcecdc,0xcece0,...,0xcecf0,...,0xcecfc,
     0xced00,0xced04,0xced08,... The +0xd0 contact-receiver 0x34020 compares
     four of them against the ARGUMENT's class id: 0xced00/0xcecf0 -> a
     PROPORTIONAL hit energy*30/100+10 (CORRECTION: it is *30, not the *45
     these notes said - the 0x34037 `lea edx,[eax+eax*1]` is x2; verified
     byte-exact, full trace in the PR-96 record), 0xcecf8 -> sets the
     receiver's [obj+0xad]=-1, 0xced04 -> the PR-89 lion stomp; 0x340b5 is
     0x34020's fall-through second half, not a separate method. The class id
     comes from a per-class vtable +0xc getter (thunks at 0x35d50..0x35eb5:
     return literal 1 for ordinary classes, or one specific global for the
     "special" ones). 0xcecfc has NO comparator anywhere - the old
     "0xcecfc = 45%/never-dies" attribution was unfounded; 0x34020 alone
     explains the lion/object hit. Still OPEN: which 0x44350 call index / which
     class vtable is which .des TYPE (needs the data-segment vtables).
     The port ships the 0x34103 dispatcher default 30 until that decodes.
   - Does the engine's corpse (A_DEAD state) still deal contact damage?
     The port says no (model choice).
   - Raising AC_ENEMY_ATC on the LION when he takes contact damage
     (his many `A_DOSTAL(1)=AC_ENEMY_ATC` lines) - the WINDOW hit
     raises it (PR-91 shipped the whole authored reaction); a plain
     touch still does not - now SUPPORTED by pinned code, the
     engine's contact tail raises AC_ENEMY (bit 7, 0x51050) into the
     answer packet, never bit 16 (post-PR-91 RE addendum in
     NEXT_STEPS). What the 14 object-side AC_ENEMY consumers do
     (A_DEAD(1)/A_AWAY/A_SHOW) is the next feature candidate.
10. ENEMY ATTACK AI, PR-90/91 OPEN ITEMS (each flagged in code too):
   - The engine's per-class AI DRIVER: how the behavior records
     (MAIN/WALK/ATACK/TURN at [x+0x106], method tables 0xa30a8/
     0xa308c/0xa3070/0xa3054) are evaluated each tick and how a
     satisfied check APPLIES the pushed state-name records (which
     check case arms A_ATAK, what the jump-table case indices mean,
     what the [x+0x1a] and 0x3fdb0/[x+0x16] cases are). The port
     ships the pinned BOX only, as a stand-in behind LEO_PLAY_DANGER.
   - ATACK::check (0x61cd2): the lion's side vs the facing getter
     (vtable +0x70) semantics, and the 0.5-argument random gate
     (0x61d26 -> 0x3fee0). Not modelled.
   - The other classes' boxes/parameters (SNAKE 0x64830, PAJAK
     0x626c0, NOSOROG 0x63750, PAPUGA 0x53ff0 constructors build
     their own classes with their own immediates): only LICHA's
     |dx| < 40, |dy| < 50 (0x61b6e) is pinned.
   - What the engine's scope= pair ([obj+0xfa]/+0xfe, chosen at
     0x3bde0, passed at 0x3bc1d/0x3bcb9, compared at 0x3d70c) is
     FOR. Measured NOT to be the attack box.
   - The engine's consumer of the frame atc_on window (0xa10f0
     directive objects, runtime templates 0xa0e60/0xa0e74), and how
     the "toucher moving toward touched" gate (0x550c0) and the
     table-referenced attack dispatcher (0x34020 only as a pointer
     at 0x9e204) fit the hit path. The port's enemy-window hit is
     the lion-side mirror, not the engine's pipeline. PR-94 read
     0x550c0 verbatim: it is an ORDERING predicate (0x52d30 lookups,
     first-dword difference signed by the [x.vt+0x70] getter) gating
     answer-C 0x55130, not a movement test - what it orders stays
     open. The 0x9e204 pointer RESOLVED: it is base 0x9e134's +0xd0
     slot, and 0x340b5 is the second half of the 0x34020 function
     (id-compare branch, full reading in the next record): the
     receiver of 0x50ed0 is the OBJECT (edx=lion, ebx=30), and the
     stomp writes [other+0xad] = -1 directly when the feet land
     within 0..20 above its top and [lion+0x5c]!=2 (PR-89's numbers
     stand).
   - DANGER semantics: the touch branch gated by `test ah,0x20`
     (0x5528b/0x51019) loads [level+0x54] (the never-authored
     danger_power) - is the authored control flag really an AI
     precondition or only a contact-hazard marker? The port uses it
     as the arming precondition because every shipped enemy walk
     state carries it; MODEL. CLOSED for the damage side by PR-94:
     that branch (0x51019 -> hit method vtable+0xd0) raises NO
     signal and applies the field that the level parser forces to
     0 - on the engine side DANGER IS a contact-hazard damage route
     with zero shipped damage; whether the
     compiled AI also reads the flag for arming stays unpinned, so
     the port's stand-in is unchanged.
   - The attack's MOTION: what the armed A_ATAK drives in the engine
     (charge? T=/F= only?). Measured on data: ZARIA LICHA3's A_ATAK
     frames carry NO motion directives - the worm STANDS and swings
     (the port plays it identically); the engine's compiled AI may
     still walk it (unpinned). The lion-side raise landed with PR-91
     (the authored A_DOSTAL reaction: hit art, `I -320,-250`
     pullback, recovery through A_DRAP at the wrap).
   - PR-92 SHIPPED the object-side half: a touch raises AC_ENEMY (bit
     7) on the touched object (0x51050) and its own A_ATAK(1)/
     A_DEAD(1)=AC_ENEMY lines react (selftest with the shipped
     RBLOK/GPTAK chain texts; the two contact legs pin the raise
     log). Two model choices stay open: (a) the engine writes the
     set as a MAILBOX at [obj+0x0..0xb] (0x51074) - the port feeds
     the object's own sig set instead, since no [obj+0x0] field is
     modelled and only its own table can read the bit; (b) the
     [obj+0x102] countdown GATES the raise via hit_cd but a pure
     touch does not SET 14 (the engine's handler does - shipping
     that would re-pin the PR-89 kill fixtures: a touched object
     would reject the lion's next hit for 14 ticks).
   - Live object-side CONSUMPTION is pinned offline only: the shipped
     bit-7 consumers sit where no play leg reaches (ENA.03 RBLOK x3
     at (1600..1640,562) - the level spawns A_SWIM and the port has
     no swimming locomotion; RAN.01 GPTAK at (200,200) with dy 98 >
     the 40x50 box, BY_TIME; LONA.05 FOKA at (893,480), level never
     played). The next live proof wants swimming/flight motion work,
     not more signal plumbing.
11. PR-93 OPEN ITEMS (the placement grammars; each flagged in code too):
   - Are any PICT/`.SEQ` statics ANIMATED by the original? The port
     draws the first patch-art element only (frame one) - the lines
     carry no motion data, so this is the absence of data, not a
     guess; the video's ZARIA shelves look static, but that segment
     cannot settle the other levels.
   - The engine TRIGGER that spawns attached children (every 0,0 entry)
     from their parents - which event, which offset from the parent.
     Until then those scripts are joined+counted but never
     instantiated (the corner fruits are gone; their live behaviour
     is not modelled).
   - RESOLVED (PR-95): the ZARIA.01 `PICT,panc.seq,20,29` line that
     directly follows the NOSOROG entry is the rhino's CHILD, not a
     top-level placement. The grammar discriminator is the property
     block: a 3-comma static that sits inside a BOUNDED patrol actor's
     `damage=/bounds=/scope=/energy=` block (the actor has `bounds=` ->
     b0>=0, or `scope=`) is drawn at the parent + that offset, exactly
     where the 5-field 0,0 children are authored. So `NOSOROG,...,1700,600
     / bounds=1680,1950 / scope=40,150 / energy=60 / PICT,panc.seq,20,29`
     puts the mat at (1720,629), not the world corner (20,29) - which is
     why the rhino "stood on nothing". The gate is deliberately the
     bounded-actor test: a static that merely follows some OTHER object's
     property line (`XCHG ... xchg=5,2` then a standalone `TABLICA` sign
     in TUNGA ANI2) keeps its absolute coordinates, as do the ANI*.DES
     shelf lines and the ANI4 WODOSPAD waterfalls (siblings, no property
     block before them). Verified by full census: only the two ZARIA rhino
     mats attach; every other level is unchanged. Still open: whether the
     mat then FOLLOWS the rhino as it walks (it is placed at the parent's
     spawn position; the follow/trigger is the same open question as the
     0,0 children above).
   - `ZAR.BLK` fields and the GRP format (WODOSPAD.GRP/KIJ.GRP/
     KILOF.GRP in GRAB*/ZWODOSPD leaves) stay unread - the placement
     lines that name them (`GRABG,2113,350,kilof.grp`) are counted as
     group headers, not placed.
   - RESOLVED in PR-97: the lion's feet sank ~30 units below the drawn grass
     tops on ZARIA.01 because the port settled the player's POSITION POINT on
     the .plt line. The frame anchors of an animation keep the element's
     SHAPE-BOX BOTTOM at a fixed offset below the position point (proof in
     ll_seq.h's ll_seq_shape_contact note) and the engine's terrain-side
     geometry is that shape box, so the contact line is the shape bottom -
     for the player and for ground-bound placements (obj_settle) alike. Not a
     camera or anchor problem: PR-84's feet-at-178 anchor was right.

12. PR-97 OPEN ITEMS (the contact line; each flagged in code too):
   - The engine's grounded-y bookkeeping: the shape-box bottom is what the
     data pins to the ground line and what the game's hit/push code uses, but
     the instruction that turns it into the grounded y is unread. The port's
     `pos = line - contact` is an inference (OPEN, marked in ll_seq.h).
   - Elements with no margin lists, and the 34 all-empty-margin ones, get
     contact 0 (their position point on the line). No evidence either way.
   - Re-grounding after an `M dx,dy` frame delta or a `P oz,px,py` position
     write is untested: the legs using them (PR-74/76/77) have states with no
     `.SEQ` art in the fixtures, so contact is 0 there.
   - Only `obj_settle` follows the terrain, once at spawn. No other object is
     re-grounded continuously (BY_X patrols walk flat at their settled y).
     Whether the engine re-tests every object every tick is unread; the
     `G=`/`CHECK_LN` pair suggests it does.
   - The five drawn-anchor legs use a colour mask in a window around the
     logged position, which also catches same-coloured scenery (the berry
     cluster at x 75..115 on ZARIA.01), so they are regression tripwires, not
     measurements of the lion. Fix: window the sprite from its own bbox.

13. PR-98/PR-99/PR-100 OPEN ITEMS (the F=/FC=/I/IC motion model; each
    flagged in code too, full lists in the PR-98, PR-99 and PR-100
    NEXT_STEPS records - PR-100 closes PR-98's item 6 and half of
    PR-99's item 11):
   - NOT "REFUTED" - PR-99 CORRECTS PR-98 HERE. The engine loop's
     steady state is v_eq = 5*F_eff*s/T, speed proportional to F/T.
     PR-98 recorded that as dead on PR-79's measurements; the samples
     do not say that. The worm (F=100,T=40) and the rhino (F=180,T=40)
     share T, so they show only speed proportional to F at a fixed T -
     and at T=40 the two laws differ by exactly the constant PR-79
     fitted as /4. The T=20 sample is the lion, i.e. the player, whose
     path through that code is unverified (item 8). Worked the other
     way, with s=10 at a 20Hz tick, the same algebra reproduces BOTH
     object samples AND their per-row distances (PR-99's CORRECTION
     section: 25 and 45 px/s, 4 and 7 px per row). Nothing was
     re-pinned: what separates the laws is a BY_X OBJECT at T other
     than 40, and the data is full of them (F=100 with 11 different T
     pairs, per `tools/re/mov_channels.py`) while gp.mp4 is ZARIA only.
   - THE CHANNELS ARE NOW READ, THE MECHANISM IS NOT. `v` is hundredths of
     a px per tick (the /100 at 0x5435d), and 25 such ticks/s turns the
     authored pairs into exactly the two divisors the port fitted from the
     video: F/100 px/tick -> F/4 px/s (PR-79's walk law) and `I 150,-1200`
     -> (37.5, -300) px/s = LL_PLAY_JUMP_DIV=4 (PR-74's jump). The struct
     mapping was verified from the image (0x52590: `F=` = rec+0x50/+0x54,
     `T=` = +0x58/+0x5c capped at 130, `G=` = +0x48/+0x4c), and it rules
     out the one escape that could have made the loop fit: the header `F=`
     feeds ONLY the increment channel, 0x54170's target comes from the row
     `F`/`I` nodes alone. PR-99 has meanwhile read the `control=`
     dispatch that PR-98 left as its open item 2, and it goes the
     other way: the dispatcher is 0x543a0 (PR-99 printed 0x543a1; PR-100
     corrects that from the aligned prologue and 155 method-table
     entries at slot +0xa0), its BY_X branch feeds the
     row distance accumulator [obj+0xa1] from `[obj+0xca]`, which only
     0x54250 writes (`v_x/100` at 0x54365) - so a BY_X walk DOES run
     through that velocity loop, and "a position mechanism with no
     integrator" is closed. The mode enum is now known and
     cross-confirmed by the jump table at 0x54380: movrec+0x12 bits
     0..2 = BY_X 0 / BY_Y 1 / BY_TIME 2 / BY_TX 3 / BY_TY 4, and
     91% of the shipped states (2121 of 2318) are BY_TIME, which
     advances that accumulator by the global `ds:0xcef40 -> +0x388`
     instead of by velocity. PR-100 closes half of what that field is:
     it has exactly one write in the image, `mov [eax+0x388],0x64` at
     0x4c8b7 - a constant 100 - and the same global is the level's own
     record object (its +0x3a0 array is what the state parser indexes).
     PR-100 also found the dispatcher's CALLER: the state record's step
     at 0x521f0 calls it as slot +0xa0 of the owner object's method
     table, then compares the current row's threshold field against
     [obj+0xa1] and admits at most one row per step, handing the
     remainder to the row handler; that also closes PR-98's item 6
     ([obj+0x95] = "the row advanced on this record step"). What stays
     open is the RATE of that step: PR-100 found the caller - `[rec+0x44]`
     is a second method table inside the record object (16 copies, same
     layout, installed by the constructors, e.g. `mov [eax+0x44],0x97c9c`
     at 0x1e119), the step is its slot +0xc, and `call [[rec+0x44]+0xc]`
     exists exactly once, at 0x5240c inside 0x523f0, a key builder whose
     only direct caller is 0x5371c. Same record found the row handler
     (slot +0x18, 0x520c0, no direct caller anywhere) writing the leftover
     back into [obj+0xa1] at 0x52118, which is why a row advance does not
     clear the accumulator; and it found the record row-set method
     0x52360, which reverses the row order from movrec bits 0x10/0x20 -
     the same two bits 0x520c0 tests, the strongest lead yet that
     SEQ_REV_X / SEQ_REV_Y are row-order flags and not motion at all.
     What 100 units of stays open too.
   - PR-101 READS THE ROW-NODE AND KEY LAYER (RE only, nothing pinned).
     The row entries at `[[rec+0x2e]]` are NODES with a 6-slot class
     table at `[node+4]`, and 31 such tables exist - `python3
     tools/re/node_classes.py` censuses them: 30 in a stride-0x18 run
     from 0xa0f28, plus one at 0xa2638 which is also the tail of the
     record sub-table 0xa260c. Slot +0x10 is a CHANNEL QUERY
     `(eax=node, ebx=outA, ecx=outB, edx=id)`: 28 classes only walk to
     `[node+0]`, three answer - 0x45d50 matches the node's own id byte
     at [node+0x10] and adds its pair, 0x45d90/0x45dc0 answer id 6.
     That is how authored row directives are consumed: 0x52470 (the
     sub-table's +0x28) asks id 4, and 0x520c0 asks id 6 with
     outA = `&[rec+4]` - an id-6 node adds its authored number straight
     into the row index, which replaces PR-50's "highest matching index"
     reading of the row move. The key is the same mechanism:
     0x45800/0x45870 walk the chain and return 13 zero bytes at its
     end, and the two exceptions (0x45970, 0x45920) OR
     `[node+8..0x10]` into what the chain returned, so a key is the OR
     of the nodes on a chain and is never stored - PR-100's "what is
     the key" is closed that way. 0x520c0's sequential path is now
     complete: the row list WRAPS (forward at `[rec+0x36]-1`, back at
     0) and the wrap is reported as bits 2/3 of the caller's key byte,
     so "the list ended" arrives in the key system; the reversal is
     `([rec+0x12]&0x10 and [[obj+8]+0x70]()==-1) ^ ([rec+0x12]&0x20
     and [obj+0x71]==-1) ^ ([obj+0x99]==-1)`, which settles the
     SEQ_REV_X/SEQ_REV_Y question as row order. On the caller side
     0x52000 (the sub-table's +0x04 slot) scans a table of 21-byte
     entries at `[rec+0x1a]` - a 13-byte key plus a RECORD index and a
     ROW index - with 0x47e40 for each entry, so a matched key means
     "go to record A, row B": that is the state-transition table, and
     the `+0x1c` argument PR-100 flagged is that row index, not a key
     pointer. One correction to PR-100's own text: it swapped the two
     branches of 0x521f0 - the EMPTY-key row is the one that runs the
     feed dispatcher and the threshold compare, a keyed row is the one
     that must match 0x47e40 or nothing happens.
   - PR-102 FINDS THE WRITER OF THAT TRANSITION TABLE AND THE RECORD
     CLASS BEHIND IT (RE only, nothing pinned). The base record class
     table is 0xa260c (PR-101 read its +0x0c row step and +0x18 row
     handler) and its +0x00 parse slot is 0x51910; inside it 0x51e07
     builds the entries, and the entry ctor 0x49610 is called from
     exactly one place, so only the `.MOV` parse path fills the table.
     It matches `NAME ( number ) =` (the format string at 0x9225c) and
     pushes a 21-byte entry into the vector AT `[rec+0x1a]` (`[+0x1e]`
     end, `[+0x22]` capacity, grow via 0x22010): the key from the
     expression parser 0x47d50, the TARGET STATE index from 0x53440 (a
     name search over `[obj+0x3c]`, stride 8, count `[obj+0x44]`), and
     the number as the ROW index. So the .MOV line
     `A_WAIT(1)=AC_BREAK_NEXT` means "go to state A_WAIT starting at
     row 1 when key AC_BREAK_NEXT is raised", and the port parses that
     number (PR-86, `ll_mov_tr.entry`) and then throws it away: a
     census of the shipped data finds 2572 such lines, the left-hand
     name differs from the enclosing state in 2447 of them, and 37
     states are entered at several different rows (A_DEAD at rows up to
     97 - the documented maximum frames in one state). Also closed:
     0x47e40's four cases are EXACT (len 3) / subset via AND (len 2) /
     any-common-bit (len 1) / degenerate (len 0), and it prints
     `TEventSet::match` for a too-long key; and the control-token
     table reproduces the port's PR-59/60/90 bit values exactly. That
     parse method is the record's whole base vocabulary and it
     confirms the layout: `time=N,M` two WORDs at `[rec+0x8]`/
     `[rec+0xa]`, `time_scale` a dword at +0xe, `sample` a dword at
     +0x16, `seq` a name through a name->index map at
     `[[0xcef40]+0x398]`, `control` the dword at +0x12; the class
     table's +0x14 slot (0x518f0) is a factory - `new(0x48)` then the
     copy-ctor 0x517e0, which ends with `mov [rec+0x44],0xa260c`: the
     first hard proof of what `[rec+0x44]` is. PR-100's "0xa25d4 is
     this record class's method table" is now refined: 0xa25d4 is a
     DERIVED record class - `new(0x60)` and a ctor (0x52500) that
     calls the base copy-ctor and writes `[rec+0x44] = 0xa25d4` - and
     its parse slot 0x52590 reads `F=` into `[rec+0x50]/+0x54`, `T=`
     into `+0x58/+0x5c` with the 130 clamp, `G=` into `+0x48/+0x4c`,
     then delegates every other line to the base parser (0x5264b:
     `call 0x51910`). Those are exactly the port's `s->fx/fy`,
     `s->tx/ty`, `s->gx/gy` fields (ll_play.c) and the cap it does not
     model. Corrections to port notes: the mode field has FIVE values
     (BY_X 0, BY_Y 1, BY_TIME 2, BY_TX 3, BY_TY 4) and the
     dispatcher's jump table (0x54380) feeds mode 1 from `|v_y|` at
     [obj+0xce] and mode 4 from the level constant behind a
     `[obj+0xbe] != 0` guard (0x5441b), while mode 3 (0x543f2) sits
     behind `[obj+0xba] != 0` and mode 2, BY_TIME, is 0x543fb with NO
     guard at all - PR-108 corrects the "modes 2/3 behind that gate"
     wording of this paragraph, which had the two entry addresses
     swapped - so BY_TY is not a typo alias of BY_TIME
     (BY_TX: 0 files; BY_TY: 12 state blocks in 7 files, three state
     names, all lion climb/hang states). And
     those two gates are assigned, not derived: 0x549f9/0x54a08 copy
     them from a caller-supplied 2-dword structure, after which the
     same method rewrites `[obj+0xba]` to +10/-10/0 from a distance
     comparison with +/-5 margins against `[[obj+0xd2]+8]` when
     `[[obj+0xd2]+4] == 1` - the best lead yet on the PR-99-open
     fields [obj+0xba]/[obj+0xbe]. Row nodes are built at PARSE time:
     the derived ctor 0x52500 calls 0x51eb0 (which clears `[rec+0x0c]`
     and loops on the vector at `[rec+0x2e]`), the node ctor 0x515e0
     writes `mov [node+0x4],0xa2638` - that IS a row-node class table,
     which explains PR-101's "stray" node tables (they sit inside the
     record class table's slots +0x2c..+0x44) - and its reader 0x51660
     reads the row's number with `"%hi"` and then scales it:
     `[node+0x10] = T * time_scale / 1000`. So `time_scale` is not an
     unmodelled curiosity: it is a parse-time scale on EVERY row
     threshold, and all 120 shipped `time_scale=` lines are multiples of
     100 (400..2000). `KADMAR.07_BAL_KBALONIK.MOV` has
     `time_scale=2000` and rows `1,200`, `2,200` ..., i.e. the engine
     compares 400 against an accumulator that PR-100 showed grows by 100
     per record step - 4 feed steps per row, 2 with the 1000 default.
     This port counts `time_scale=` as unmodelled and compares `f->ms`
     as written (ll_level.c:1136), so that whole balloon state runs
     2.0x off in this port's reading. Nothing pinned; the cadence
     number (how often 0x521f0 runs) is now the only thing between this
     and a frame rate.
   - PR-103 NAMES THE OBJECT CLASSES (RE only, nothing pinned). The
     class map is a name factory: 0x4df10, called from exactly two
     places (0x4aac3, 0x4dcdc), both right after a `' %[^,\r\n] , '`
     read - the first field of the placement lines this port already
     parses. Each candidate is a case block: case-insensitive compare
     (0x76a69 folds A-Z), `new(size)`, base ctor, then the pair write
     `[obj+8]=T+8; [obj+0xc]=T`. It first tries a generic 31-name
     chain (ANIBACK ZWIERZ POCISK MOVEPLAT GRUPA BERLO ...), then
     calls eleven per-level sub-factories: ZARIA ENA ENA2 KADMAR
     KADMAR2 SADES RAN KARA TUNGA1 TUNGA2 LONA. Census: 394 pair
     writes / 163 distinct class-table pairs; 107 class names captured
     at name-tested sites; 157/163 pairs have `[obj+8] = [obj+0xc]+8`
     exactly (one method list, two base headers 2 slots apart; the
     exceptions include two classes where [obj+0xc] is a scalar 0/1/
     0xff and three wide multiple-inheritance pairs). Every level
     group ends with a TLew<level> class (TLewZaria ... TLewJunction)
     - the engine's lion is a per-level C++ class, and the groups sit
     next to per-level `.mod` model names and A_DEAD state names.
     Slot meanings are per class: 0x543a0 sits at [[obj+8]+0xa0] in
     118 classes, and PR-100's 0x536f0 at [[obj+8]+0x14] in 24 - so
     the port's [[obj+8]+off] hook offsets belong to the measured
     families. Also corrected from PR-102: entry+0x11 = N-1 (or -1
     when `( N )` is absent), the TARGET name resolves at PARSE time
     through the case-insensitive 0x53440, and 0x531f0 (a ctor in the
     chain 0x3d8c0->0x53ff0->0x531f0->0x52860) sets [obj+0x5c]=1 -
     state indices are 1-based. Still open: who appends to the
     [obj+0x3c] state-name table, the upstream placement callers,
     eleven placement names that match no literal (GLON, ROZGWIAA/B/C,
     STULB_D, and SEP/PARROT/SNAKE/LICHA/PAJAK/NOSOROG in an unmapped
     block at 0x9320b), and what the per-class methods actually do.
   - PR-104 READS THE LION CLASSES (RE only, nothing pinned). All
     thirteen TLew<level> factory cases and their class tables:
     sizes 0x12a..0x162, two shared bases (0x500b0 -> pair 0xa2218/
     0xa2210 for ENA2/KADMAR2/KARA/TUNGA1/JUNCTION, 0x1e0a0 ->
     0x97970/0x97968 for both ZARIA classes), per-class fields like
     `[obj+0xf6]` = 2000/1500 defaults carried IN the class, not in
     the data. The headline: every level's lion has a death-theme
     method - read `[[rec]+4]` (the CURRENT state's row index), if 0
     and global 0xcf074 is 0, play a hard-coded .MOD through the
     music singleton 0xcf068 (the port's own mapka.mod anchor), then
     tail-call the base hook 0x52750. LOOSER.MOD ZARIA, ENA2ED,
     KADMDEAD, K2DEAD, SADESDEA, RAN1, KARADE, lonade LONA, and the
     default DEATD.MOD gated additionally by `[[obj]+0x126]==0` in
     class 0x97bc0. Those themes exist NOWHERE in the authored files
     - `muzyka=` names only the level tune - so no data-only port
     can know them. The default version of the slot is 0x50300 at
     [[obj+8]+0xe4] in every lion table measured; the overrides sit
     at +0x128/+0x1d4/+0x1f0/+0xe4/+0x20c, confirming slot numbers
     are per-class. Census correction: PR-103's pair scan saw only
     `[reg+0x8]/[reg+0xc]`; lion ctors whose `this` is not the
     object base write their pair at other offsets (TLewEna at
     [obj+0x1c], TLewSades [obj+0x3c], TLewTunga2 [obj+0x0]), so
     the class count is above 163 and the "0x98278 scalar [obj+0xc]"
     exception reads better as an embedded object used as a member.
     PR-103 open item 4 closes: the 0x4e9fe stub and its callees are
     per-level global-state zeroing (e.g. `mov [0xcee24],0; ret`),
     not factory chains. Still open: the `[[obj]+0x126]`/0xcf074/
     `[[0xcef40]+0x394]`+0x106 flags, why the guard is "row 0", the
     rest of the lion method bodies, the owner table of the ZARIA
     method, and the identity of class 0x97bc0.
   - PR-105 CLOSES THE STATE-STEP CHAIN (RE only, nothing pinned). The
     record step 0x521f0 has exactly ONE call site: 0x5240c, inside
     0x523f0, the record-step driver (key builder [[rec+0x44]+0x08],
     the step at +0x0c, then TWO OWNER-OBJECT slots +0xac/+0xa8, then
     the key-out at +0x10). 0x523f0 has one call site too: 0x5371c,
     inside 0x536f0 - so ONE CALL TO ONE VIRTUAL SLOT (0x536f0, class
     slot +0x14, reached as [[obj+8]+0x14] or [[obj+0xc]+0xc], 37
     sites) = one key composition + one feed update + at most one row
     + one transition lookup and apply. PR-98/99/100/102's machinery
     is one method. Implementations: bare 0x536f0 in 24 tables, the
     default wrapper 0x400b0 in 45, the lion base 0x550a0 in both
     bases, per-class wrappers (e.g. 0x27d60) - all ending in the
     same chain; [obj+0xee] is decremented there but NEVER branched
     on, so it is not a step gate. FEED EXCLUSIVITY: the accumulator
     dispatcher 0x543a0 (+0xa0) has exactly ONE call site, 0x52317
     inside 0x521f0 itself, so the accumulator grows exactly once per
     state step and there is no other scheduler. The pump 0x4bb30
     walks the level's four object lists at [X+0x58]x3 + [X+0x64],
     steps every item's +0x14 and delivers messages to slot +0x10;
     its callers are inside 0x4c8a0 (PR-100's feed-constant
     installer) and two unreferenced functions - the steady-state
     per-frame caller is STILL not found, so "one step per video
     frame" stays an assumption, narrowed to "who calls the pump or
     +0x14 during play". Measured driver instead: lion slot +0xd0 =
     0x50ed0, gated on [obj+0x11a] not in {6,7}, [obj+0x126]==0 and
     both [[self+8]+0x40] and [[arg+8]+0x40] non-zero, runs the
     TARGET's +0x14 and caches its key at [obj+0x10a] - a controlled
     object's state machine is driven by whoever holds it. FIVE COR-
     RECTIONS TO PR-104: the death themes are the +0x08 KEY-BUILDER
     slot of the RECORD-class sub-tables (0x97c9c+8=0x4ffb0 DEATD,
     0x99338+8=0x26470 ENA2ED, 0x9b5f4+8=0x2c360 K2DEAD, 0x9d53c+
     8=0x31fa0 RAN1, 0x9e254+8=0x33c60 KARADE, 0x9f460+8=0x361c0
     lonade - exactly PR-100's key-builder census list), NOT lion
     object methods; PR-104's object-slot attributions were range
     artifacts, made worse by a tool bug (the objfactory.out class-
     name regex required single spaces; the file aligns with several,
     so class names were silently dropped - now fixed). 0x52750's
     receiver is the record (this=rec), calling +0x2c/+0x30/+0x34 and
     the OWNER's +0xc8 slot. The row-0 guard reads the owner's
     current-state row, evaluated once per step. [obj+0x126]==0 gates
     DEATD AND the +0xd0 chain AND the +0x10 message handler. And
     PR-100's "0x1e104 record ctor" is the body of class slot +0x84
     (0x1e0f0), which special-cases the state NAME 'A_DEAD' to give
     per-level death states the base sub-table 0x97c9c. PR-104's
     direct probe [[obj+8]+0xe4]=0x50300 stands (body unread).
   - PR-106 FINDS THE FRAME LOOP AND THE GATE (RE only, nothing
     pinned). The level frame loop is 0x4abd0, one caller: 0x5d251
     inside the level-run wrapper 0x5d170. Its think pass 0x4ba90
     walks the four lists (three groups at [[level+0/4/8]+0x58] with
     counts at +0x44, plus [level+0x64]) and per item calls slot
     +0x60, stepping the object (0x52b80 -> +0x14) ONLY when that
     returns non-zero; in 132 of the 162 measured class tables +0x60
     is `mov eax,[eax+0x2c]; ret`, and the flag it reads is written
     by the OTHER pass: slot +0x64 (0x238e0 in 111 tables) sets
     [obj+0x2c] from a rect-in-view test (slot +0x08 = 0x23e00 in
     157 tables, the variant 0x53690 asks 0x534e0 for its rect). So
     off-camera objects are never stepped, and the drawing pass, not
     the state machine, decides what thinks; the lion is stepped
     every iteration (0x4aed6). CORRECTS PR-105: 0x4bb30 is a
     FILTERED MESSAGE BROADCAST - its `[[item+8]+0xc]` call is a
     predicate compared against a filter argument, not the state
     step, and delivery goes to slot +0x10 with a 4-dword stack
     message. The loop's own period: 0x4b0e0 -> 0x4a120, which
     spins on the clock ds:0xcf1b2 until a setpoint [0xcef38] is
     reached, then waits ONE VGA retrace (0x1dba0: in al,0x3DA, bit
     3); that setpoint is authored at startup by 0x4a190 as 2 x
     measured-retrace-ms - 1, fallback 27 = 2*14-1, replacing the
     reset value 1000 from 0x4a0e0. The clock advances only through
     the INT 8 ISR chain 0x1c6d0 -> 0x1c680 -> the dispatcher
     0x1c5b0, adding ds:0xcf1b6 = 55 (set by the clock ctor
     0x5b860), and the level wrapper starts it (0x5d18b). The mode
     is INT 10h AX=0x13 over an internal 400-pitch buffer (0x4a230,
     0x4a170), lives=9 is set on the single-level path (0x5eae0 ->
     0x5d170), and the sound init reprograms PIT counter 0 to 70.0
     Hz (0x18870).
   - PR-107 APPLIES THE AUTHORED START ROW ON BOTH SIDES (behaviour
     change, selftest leg added; the RE facts are in the NEXT_STEPS
     PR-107 record). The row field of a transition entry is N-1 and is -1
     for an absent/non-numeric token (0x57db0 `"%ld"` at 0x92310,
     0x57fd0 + `dec` at 0x51e3b, `mov eax,-1` at 0x51e3e); the lookup
     hands it out separately from the state index (0x52000 at
     0x52044-0x5204d) and the row-write target is the record's live row
     counter at **rec+0x4** - proven from the row stepper 0x520c0, which
     reads it (0x520d2), steps it (0x521d7), wraps it at the last row and
     raises key bit 2 there (0x521c1-0x521c5), and mirrors it with bit 3
     at row 0 (0x5219a-0x521a9); the row-set method 0x52360 refuses a
     negative value (0x52381) and otherwise writes it raw with NO upper
     bound (0x523cb). That setter IS the state record's own vtable slot
     +0x1c: every state record stores the fixed table 0xa25d4 at
     **rec+0x44** (its two constructors, 0x524cf and 0x52508), and that
     table's +0x00 is the state-record parser 0x52590, +0x0c the step
     PR-105 found one call site for (0x5240c = `call [esi+0xc]`,
     `esi=[rec+0x44]`), +0x18 the row stepper, +0x1c this row-setter,
     +0x20 a leave-state method that clears bit 0x20 of [obj+0xb6]; the
     transition step calls +0x20 on the OLD record (0x535bf) and +0x1c on
     the NEW one (0x535e7-0x535ea), and those methods keep edx for their
     caller (`push edx` 0x52660 / `pop edx` 0x5267a), which is how the
     row argument survives them. The same-index path needs no setter: it
     writes `mov [eax+0x4],edx` itself (0x53616). CORRECTS PR-101: only
     the same-index path of the switch 0x535a2 reaches 0x53626 - the
     different-index path returns at 0x53601 after zeroing [obj+0x9d]
     (0x535c2), and [obj+0x9d] is NOT
     the row counter. And PR-102's "the engine applies it thousands of
     times" needs its scope: of the 2572 numeric lines **2299 are `(1)` =
     row 0**; 273 ask for another row, 101 of those are self-targets, so
     172 are cross-state, and only **25** are in a scene-object script
     (KADMAR.07 22, KADMAR2 1, ENA2 1, SADES.06 1) - the rest are the
     lion's own scripts, honoured since PR-86. Port side: one helper
     `ll_mov_row_start` for both sides, and three model choices (clamp an
     over-long row to the top where the engine bounds nothing; treat "no
     number" as the top where the engine leaves rec+0x4 alone - it
     coincides only because a finished list wraps to 0 at 0x521c5; no
     SEQ_REV mirror); the port has ONE frame counter per object where the
     engine keeps one per state record, which is the open item this change
     exposes.
   - PR-108 READS THE BY_TY VERTICAL FEED AND ITS GUARD, AND PINS THE
     PORT'S CURRENT CLIMB MODEL (RE + regression leg, no behaviour
     change; the facts are in the NEXT_STEPS PR-108 record). The frame
     loop's slot +0xcc IS 0x549f0 in 102 of 158 class tables, and 14
     (the lion family) use the derived override 0x50340, which zeroes the
     whole 12-byte input block when `[obj+0x106] == 0`. That method
     writes `[obj+0xba]`/`[obj+0xbe]` from the block and then from a
     contact record at `[obj+0xd2]` (kinds 1/2/4 at 0x54a1b/0x54b65/
     0x54aa2; the authored contact array at `[obj+0x136]` is what builds
     the kind-4 one, 0x24681/0x24688), and the same pair is the vertical
     force in 0x54250 (0x542cd). So the engine ties the climb animation
     to vertical contact force: its mode-4 feed (0x5441b) adds the level
     constant only while `[obj+0xbe] != 0`. Same tail writes
     SEQ_REV_Y's `[obj+0x71]` guard and the per-frame `[obj+0xb6]` bit
     0x10 that drives the facing flip 0x53670. The census: BY_TY is 12
     state blocks in 7 files and three state names, all vine/rope hangs,
     and **none declares `time_scale=`, so the engine's row thresholds ARE
     the authored ms numbers** - the same figures this port consumes as
     ms, which is why data cannot separate the two models. Port side
     unchanged (plain ms clock, no vertical feed) and now pinned by a
     `--play-selftest` leg shown non-vacuous by four mutations. The one
     thing left is footage: see the clip request under T5.
   - PR-109 ANSWERS THAT REQUEST, AND THE ROW CLOCK'S UNIT IS NOW
     DECIDED (one measurement on the owner's clip + RE; no behaviour
     change; the arithmetic is in the NEXT_STEPS PR-109 record). The clip
     (a rung-ladder climb, 46.6-48.3 s of a 481 s walkthrough) was
     measured frame by frame: the on-screen pose of the climb changes
     every 2-4 video frames at 30 fps, i.e. an authored row of 200 is on
     screen 66-100 ms, NOT 200 ms, and no run of 5 frames or more
     appears anywhere in the 5 s window. So an authored threshold T is
     displayed for T/100 FRAME-LOOP ITERATIONS: the engine feeds 100 per
     iteration (0x4c8b7's constant - PR-100's question 11, answered) and
     its pacer holds one iteration to TWO display frames (0x4b0e0 ->
     0x4a120 -> 0x1dba0, setpoint = 2 measured retraces - 1, with the
     27 fallback = 2*14-1), i.e. 28-33 ms per iteration. The port's
     `p->ft += dt_ms` row clock is 3.0-3.5x too slow in wall time.
     PR-106's clock paragraph needed two corrections on the way: during
     play the PIT counter is reprogrammed to a real 1 ms step (0x5b9e0
     set-period, called at 0x5d195 in the level-run wrapper), so it does
     NOT "only move in 55-unit steps", and the sound path's channel-0
     write is divisor 17045 = 7.00 Hz, not 70.0 Hz. PR-108's own
     estimate ("about 3.5x faster than the port's 200 ms") is confirmed,
     and its ~57 ms figure for a 200-unit row holds within the noise.
     What the clip did NOT answer is T5's freeze question: it shows no
     hang without travel. Two wording fixes this evidence forces: the
     BY_TY states are the RUNG-LADDER family (A_WSP/A_WSP_LIN/A_DRAB,
     `seq=climb`/`climb_r`, the DRABINKA object) - ENA2's rope state
     A_CLIMB is BY_Y mode 1 with a |v_y| feed and thresholds of 4, a
     velocity-coupled animation, and it is not what the clip shows - and
     `ll_mov_fr.ms` is a misnomer, to be renamed only together with
     PR-110, the behaviour change that makes the port count iterations
     (law, `LL_ITER_MS`, the data-free 22-step leg and its three
     mutations are named in the PR-109 record). The climb's vertical
     DISTANCE law stays open: measured ~100 source px/s, and no reading
     of `F=0,100 T=160,50` in this port produces it.
   - PR-110 MAKES THAT CLOCK THE PORT'S CLOCK (behaviour change; the
     law, the constant, every moved pin and where each came from are in
     the NEXT_STEPS PR-110 record). `LL_ITER_MS` = 28 ms - the low end
     of the clip's 28-33 ms bracket, and the closest value inside that
     bracket to the engine's own 27 fallback setpoint - with the
     accumulator counting 1/100-iteration units fed through a
     whole-millisecond carry, so the ROW counts no longer depend on the
     step size at all. The mode split is now in both engines: a BY_X
     state with a non-zero `F=` feeds its row accumulator from travelled
     distance and its time accumulator stays at 0 (0x543a2's mode-0
     branch REPLACES the 100-per-iteration feed, it does not add to it) -
     measured difference on the ZM fixture: at HEAD, 3 rows per 20 ms
     step, AC_TIME inside tick 1 and A_DRAP at tick 2; on this tree
     nothing moves and both accumulators read exactly 0. `time_scale=`
     is a parse-time scale on the thresholds (0x51679 `imul`, 0x5168d
     `idiv 0x3e8`) with the rows-before-the-line rule, and `ll_mov_fr.ms`
     became `.thresh` as PR-109 said it would. Every leg quoting a row
     duration was re-derived from the row list, never scaled, with the
     arithmetic written beside each leg in the Makefile; the new tests
     are data-free (a 5-row 1200-unit fixture walked on two grids under
     the exact carry identity, a `time_scale=2000` leg plus its control,
     and the ZM leg reading both accumulators at both ends of the mode
     gate, `facc` read at every step of the RC loops) and 8 mutations
     run, 7 caught; the one survivor is `mut_nogate4` (section 4's
     feed-site condition removed) and its reason is in that record - the
     observable half of the mode split is the read site, which is what
     `mut_bothfeed` catches. Two corrections in PR-109's plan's wake: a
     data-free test cannot quote the authored A_WSP table, so its 22-step
     cycle is right arithmetic that shipped nothing - the shipped fixture
     is the 12-feed one - and PR-104's "`time_scale=` ... an unmodelled
     counter" sentence is superseded. Still open, with counts in that
     record: the boundary at exact equality (13932 of 14881 authored
     BY_TIME thresholds are exact multiples of the 100 feed), one row per
     record step vs the port's loop, the engine's truncated `v_x/100`
     mode-0 feed vs the port's `|vx| * dt`, and the 30 shipped BY_X
     blocks with `F=0,0` that the `fx != 0` half of the gate covers and
     no test does.
   - A CAP THE PORT DOES NOT MODEL: the engine's .MOV parser clamps the
     `T=` pair to 130 (0x525f8/0x52610) and 172 authored components in 64
     files are above it (the lion's TUNGA A_WSP_LIN `T=160,50`, LONA.05's
     penguin `T=1600`). Today the port never reads T= for those (BY_TY)
     states, so nothing is wrong - but any future drag model must apply
     the cap; `python3 tools/re/mov_channels.py` prints the count.
   - A SHIPPED DISCREPANCY, now recorded on purpose: the port drives the
     player's walk to the authored cap T=20 -> 20 px/s on ZARIA while the
     measurement is 25 px/s, whereas its object-side F/4 law gives 25.
     PR-79 itself ended with "re-resolve player physics in T1 with the
     video as referee"; a PR-99 candidate is exactly that, one model for
     both sides, decided by PR-98's tests.
   - Whether the player object goes through the step at 0x54250 at all (its
     three call sites are lion-adjacent but do not check the class).
   - The `obj+0x95` flag that turns `I`/`IC` rows on, the `[vt+0xbc]` test
     and byte +0xb6 bit 2 behind the drag floor 20, and the caller of the
     row-list dispatcher 0x45df4 (outside the dump).
   - `ds:0xcef40 -> +0x388` is added into the frame-advance distance
     accumulator when the terrain scalar is non-zero (0x543f2..0x5443d): a
     global time/tick scalar, meaning unread.
14. THE `.IS` CUTSCENES' FRAMING IN THE VIDEO, not pinned by any record
    (found while measuring for PR-111). In ep.1 (`R1` in section 3) the
    gameplay picture is the full 640 px width at rows 40..439, exactly
    2x, but in the window t=118-124 s, which shows one of the game's own
    animated cutscenes, the black bands reach to column 36 and row 65
    and the picture inside is a rounded cave interior (the lion, a dog
    and a spotted big cat at a table). Whether that rounded picture IS
    the whole frame the movie decoder writes in Mode X or a letterboxed
    copy of it decides what "the same picture" means for every movie
    comparison, and PR-55/56's decoder plus PR-19/21's capture rig could
    answer it. Frame: `/tmp/opencode/refcap/ep1_120.png` (never in git).

## 8. Plan: tasks, in order. One task = one or a few PRs.

Each task: goal / where / how / done when. Do not start a task whose "how"
needs a number you cannot source - do the RE step first.

**T1. Close the jump (open question 1).**
Goal: running jumps cross the first chasm of ZARIA.01. How: (a) finish tracing
how the engine consumes the `F`/`FC` accumulators (parser and accumulation are
documented; interpretation still open), (b) parse the directives into
`ll_mov_fr`, (c) apply only the RE-confirmed effect, (d) check against the video: ep.1
~316-317 s and ep.2 ~465 s contain running jumps; ZARIA_A.PLT gives the gap
widths (`ll_plt` polylines). Done when: a hand-derived data-free test pins the
horizontal range for one state, and a video measurement agrees within ~15%.
STATUS after PR-98: (a) is CLOSED for the velocity chain - the engine's
whole F=/FC=/I/IC path is now read, at instruction level and with its data
links read out of the image (merge at 0x526df..0x5273d: the current row's
`FC`/`IC` pairs fold ONTO the state's `F=` pair, which then enters a
per-tick velocity increment scaled by a terrain scalar /20; `T=` is a
per-tick PERCENT drag taken from the pre-add velocity; frame `F`/`I` pairs
are a velocity that 0x54170 SETS, never past the target, and it runs after
the drag; the PR-79 frame-advance-by-distance mechanism is confirmed at
0x543a0 + 0x52317..0x52338; and the header keys map to rec+0x50/+0x54
(`F=`, +0x58/+0x5c (`T=`, clamped to 130 by the parser) and +0x48/+0x4c
(`G=`)). (b)/(c) still cannot be applied: that loop's steady state couples
walking speed to F/T while the measured walk law is F/4 px/s - but the
two coincide at T=40, which is where every object sample in this repo
sits, so the video does not separate them. PR-99 corrects PR-98's
"the measured walks refute it" wording on exactly that point (see §7
item 13). The header-`F`-as-velocity escape stays closed: 0x54170's
target comes only from the row nodes. The units by contrast look close
to pinned: v is hundredths of a px per tick and 25 ticks/s reproduces
this port's own two fitted /4 constants. What is left of (a) is now
narrow and explicit: the engine's law and the port's law differ only in
the T dependence, and only a BY_X object at T other than 40 can say
which one is right. Next steps, in order: (1) that video test, which
the authored data supplies in quantity (F=100 against 11 different T
pairs) but which needs footage of a level other than ZARIA; (2) the
two cheap tests PR-102 points at: the first is DONE (PR-107 applies the
authored start row on both sides; measured through the port's own parser,
of 2572 numeric transition lines 2299 ask for row 0 and 273 do not, 172
of them cross-state and only 25 in a scene-object script), and what the
data still owes is FOOTAGE of one of those 25 to say whether the port's
row is the engine's. The other test is CLOSED, by PR-109: the lion climb
states (BY_TY: 12 blocks in 7 files, 3 state names) were read end to end
by PR-108 - the mode-4 feed, its `[obj+0xbe]` guard, the contact record
that writes that register, and the fact that no BY_TY state declares
`time_scale=`, so the engine's row thresholds ARE the authored numbers -
and because that left the data unable to choose, PR-108 asked the owner
for one clip; PR-109 measured it and the answer is the ITERATION model:
an authored threshold T is displayed for T/100 frame-loop iterations, not
T milliseconds, so the port's row clock is 3.0-3.5x too slow and PR-110
is the behaviour change, with its plan named in the PR-109 record. Item
(1) above still needs footage of a level other than ZARIA. The table
itself is now read end to end:
PR-102 found its writer (the base record parse method 0x51910 ->
0x51e07, entries of {key, target state index, start row} pushed into
the vector at `[rec+0x1a]`, and the derived record parser 0x52590 that
reads `F=`/`T=`/`G=` and delegates every other line to 0x51910) and
its authored form (`TARGET(N)=<key expression>` inside a state
section), and 0x47e40's four key-match cases. This port parses that
row number (PR-86) and ignores it, and PR-103 corrects the row number
itself: the engine stores `( N )` as N-1, or -1 when the part is
absent, and resolves the TARGET name to a state index at PARSE time
through the case-insensitive 0x53440. PR-103 then read the class map
PR-100 item 5 asked for: it is a name factory at 0x4df10 keyed on the
first field of the placement lines (case-insensitive), 163 class-table
pairs, 107 named classes including a TLew<level> lion class per stage,
and it shows the `[[obj+8]+off]` hook offsets to be per-class rather
than universal (0x543a0 sits at +0xa0 in 118 classes, 0x536f0 at +0x14
in 24). What is left of that line of work is now narrower: who
APPENDS to `[obj+0x3c]` and fills `[obj+0x50]` (the ctor chain only
initialises them; PR-103 open item 2), what the per-class methods do
- PR-104 read the first whole method of the TLew* lion classes (a
death-theme hook that plays a hard-coded `.MOD` through the music
singleton when the current state's row index is 0; the themes are
named NOWHERE in the authored data) and the rest of their bodies is
still open - PR-105 corrected where that hook lives (the +0x08 key
builder of the RECORD sub-tables, not a lion object slot) and closed
the pacing chain end to end: the step 0x521f0 has ONE call site
(0x5240c, in the record-step driver 0x523f0), which has one call
site (0x5371c, inside 0x536f0, class slot +0x14), the feed
dispatcher 0x543a0 has one call site inside 0x521f0 itself, so ONE
+0x14 CALL = one key composition + one feed update + at most one
row, and no other scheduler exists; PR-107 read what that one row
advance touches: the live row counter is the state record's own
`rec+0x4` (0x520c0 reads, wraps and mirrors it; the row-setter 0x52360
is the state record's vtable slot +0x1c in the table every record stores
at rec+0x44 - 0xa25d4, written by its constructors 0x524cf/0x52508 - and
the transition step calls it on the NEW record at 0x535ea), so the
engine keeps one row counter per state record while the port has one
`o->fi` per object shared by all its states;
"100 units" of the level global's +0x388 feed is half-closed
(PR-100: the single writer is `mov [eax+0x388],0x64` at 0x4c8b7) and
PR-102 found the gate; PR-108 corrects its wording and reads its
producer: that feed runs unguarded for mode 2/BY_TIME (0x543fb), only
mode 3 is behind `[obj+0xba] != 0` (0x543f2) and mode 4 is a separate
body behind `[obj+0xbe] != 0` (0x5441b), and both registers are written
by the input-apply method 0x549f0 - the frame loop's slot +0xcc - from
the input block and the contact record at `[obj+0xd2]`. And PR-102 found the
concrete mechanism the pacing tension was missing - the row's threshold
is stored pre-scaled at parse time, `[node+0x10] = T * time_scale /
1000` (0x51660), and 0x52320 compares that against the accumulator, so
a threshold of 200 with the 1000 default is 2 feed steps and the same
row under `time_scale=2000` is 4. What a row's duration is, in
seconds, used to be exactly one unread number: how often the +0x14
slot (0x536f0 and its wrappers) runs during play. PR-106 read that
caller - the level frame loop 0x4abd0 (one caller, 0x5d251 in the
level-run wrapper 0x5d170) calls the think pass 0x4ba90, which steps
an object through 0x52b80 -> +0x14 only when its slot +0x60 answers
non-zero, and 132 of 162 measured class tables have +0x60 = `return
[obj+0x2c]`, a flag written by the drawing pass at 0x4b970 through
slot +0x64 (a rect-in-view test). PR-105's "the pump 0x4bb30 steps
every object in the level's four lists" is corrected: it is a
filtered message broadcast. What is left is the loop's own period,
and that is now a measurement question, not an RE one: the pacer
0x4a120 (called from the loop at 0x4aef5 via 0x4b0e0) waits for a
setpoint at [0xcef38] that startup code 0x4a190 authors as 2 x
measured-retrace-ms - 1 with a 27 ms fallback (= 2*14-1) and then
syncs to one VGA retrace (0x1dba0: port 3DAh bit 3); the ms clock it
spins on moves in 55-unit BIOS ticks (the INT 8 chain 0x1c6d0 ->
0x1c680 -> 0x1c5b0, the clock started by 0x5b860 from the level
wrapper at 0x5d18b); the video mode is 320x200 via INT 10h AX=0x13
(0x4a230) and the sound init sets PIT counter 0 to 70.0 Hz
(0x18870). So the row-to-seconds factor depends on which of those
rates a real machine actually ran at (NEXT_STEPS PR-106 open item 2)
- the port's 20 ms tick is not re-pinned here.
(3) what writes
[obj+0xba]/[obj+0xbe] (0x54a00..0x54ab0) - they are now known to be
the gate of the time feed, but their meaning is unread - and where the
full-precision velocity moves the coordinates (PR-99 item 10; only
`v/100` is stored);
(4) the other video tests PR-98 lists (the jump's x track against the
port's one-shot `ix/4`; an `FC` window in A_FIGHT), then a PR that puts
both sides on one measured law with every play pin re-pinned from it.
DONE since PR-98: PR-99 read the `control=` dispatch and the BY_* mode
enum (its entry address: that record printed 0x543a1, and PR-100
corrects it to 0x543a0 from the aligned prologue and 155 method-table
entries at slot +0xa0; the jump table at 0x54380 stands), closed
PR-98's item 2, and showed the BY_X branch feeding the row distance
accumulator from 0x54250's own `v/100`. PR-100 then found that
dispatcher's caller - the state record's step at 0x521f0, reached as
slot +0xa0 of the owner object's method table - and with it the whole
gate: the current row's threshold field is compared against [obj+0xa1]
once per record step, at most one row is admitted per step, and the
remainder is handed to the row handler at `[[rec+0x44]+0x18]`. PR-100
then went one level deeper: `[rec+0x44]` is a SECOND method table
inside the record object itself (96-byte records, installed by the
constructors, 16 copies with one shared slot map), the row handler is
its slot +0x18 = 0x520c0 which writes the leftover straight back into
[obj+0xa1] at 0x52118, the step is slot +0xc and is called from exactly
one place, 0x5240c inside the key builder 0x523f0 (a 13-byte key made
from the record class, the owner and the frame array), and slot +0x1c
is a row-set method, 0x52360 in four classes, which reverses the row
order from movrec control bits 0x10/0x20 - the same two bits 0x520c0
tests, and the strongest lead yet that SEQ_REV_X / SEQ_REV_Y are
row-order flags rather than motion. Following that chain one step
further, PR-100 also settled the object side of the same struct:
[obj+0x50]/[obj+0x5c] is the object's RECORD ARRAY plus a current
index - PR-77's "frame array" - so each frame carries its own motion
record, and the method that switches it (0x535a0, the third
[obj+0x95] setter) calls the old record's +0x20 with the object's key,
sets [obj+0x95], runs the 0x239c0 hook, calls the new record's +0x1c
row-set method and writes the new index. PR-98's item 6 is closed too
([obj+0x95] means "the row advanced on this record step"), and the row
thresholds are now censused per mode by `tools/re/mov_channels.py`: 94%
of the BY_TIME values are exact multiples of that 100 feed, the BY_X
values are small counts of px.
PR-101 then read the layer under the rows (RE only, no pin): the row
entries are nodes with a 6-slot class table at `[node+4]` (31 of them,
`python3 tools/re/node_classes.py`), slot +0x10 of that table is a
query-by-id that every node on the chain answers by adding its own pair
- which is how authored row directives are consumed, and an id-6 answer
adds straight into the row index through `&[rec+4]` - and the key is
the OR of the nodes on that chain, computed on demand and never stored.
0x520c0's sequential half is complete now: the row list wraps and the
wrap is published as key bits, and the reversal is the XOR of the two
SEQ_REV control bits with their owner-side guards, which settles those
bits as row order. Two things PR-101 corrects in PR-100's own text: the
two branches of 0x521f0 are swapped there (the empty-key row is the one
that runs the feed and the threshold compare), and the `+0x1c` argument
it flagged as inconsistent is a row index after all - the index comes
from the transition table at `[rec+0x1a]` (key, record index, row
index), which is the engine's own version of `obj_transition` and the
new head of this list.
PR-102 then wrote that table's story (RE only, no pin): its writer is
the base record class's own parse method (the class table 0xa260c
holds 0x51910 at its +0x00 parse slot, and inside it 0x51e07 builds
the entries - the entry ctor 0x49610 has exactly one caller), it
matches the authored form `TARGET(N)=<key expression>` (format string
at 0x9225c) and pushes {key, target-state index resolved from
`[obj+0x3c]` by name, row index N} into the vector at `[rec+0x1a]`.
The record class itself is now mapped: a base of 0x48 bytes (factory
0x518f0, copy-ctor 0x517e0, which writes `[rec+0x44] = 0xa260c`) and
a derived class of 0x60 (0x52570, ctor 0x52500 writing `[rec+0x44] =
0xa25d4`) whose parser reads `F=`/`T=`/`G=` into `[rec+0x50]` and
beyond, with the 130 clamp on `T=`, and delegates every other line to
the base parser. So a shipped line like `A_WAIT(1)=AC_BREAK_NEXT`
means "go to state A_WAIT starting at row 1 when AC_BREAK_NEXT is
raised" - the engine's transition picks the START ROW, this port
parses it (PR-86) and then sets fi = 0, and the
data asks for other rows thousands of times (2572 lines, 37 states
entered at several rows, up to 97). 0x47e40 is the match and its four
length-byte cases are exact / subset-by-AND / any-common-bit /
degenerate; the control-token table re-derives to the port's own
PR-59/60/90 bit values exactly; and the mode field turns out to have
five values, with BY_Y feeding `|v_y|` (0x543dc) and BY_TY being a
separate guarded feed (0x5441b, gate `[obj+0xbe] != 0`) - so BY_TY is
not a typo alias of BY_TIME, and those two gates
([obj+0xba]/[obj+0xbe]) turn out to be *assigned*, not derived:
0x549f9/0x54a08 copy them from a caller-supplied 2-dword structure and
the same method then rewrites `[obj+0xba]` to +10/-10/0 from a
distance comparison with +/-5 margins - the best lead yet on the
fields PR-99 left unread. PR-108 then closed that lead's remaining half:
the method IS the frame loop's slot +0xcc (0x549f0, 102 of 158 class
tables; the lion family overrides it at 0x50340), `[obj+0xbe]` is the
contact record's vertical force (kind 4 at 0x54aa2, built from the
authored array at `[obj+0x136]`, 0x24681-0x24688), and BY_TIME is NOT
gated at all - 0x543fb is entered straight from the jump table.

**T2. The lion's attack and enemy energy.**
Goal: kill the first LICHA. Where: `LEW.MOV` states `A_FIGHT*`, `A_FG_PACKA*`
(conditions `K_FIRE`+`K_F1`/`K_F2`, `K_ALT`); enemy `.MOV` death states;
`energy=` from the placement. How: add the fire keys (Ctrl=fire per the
walkthrough, F1..F4 weapons; map as viewer choices — LANDED in PR-87:
K_FIRE/K_F1..F4/K_ALT on all three input surfaces, comma groups as AND,
A_FIGHT reachable and pinned on the real ZARIA script), find where the lion's hit
box (`AC_RECT`) and the damage to enemies are decided (enemy hit method slot
+0xd0, class 0x41180 / 0x3dae0), apply, play the enemy's dead state, remove
the contact damage. Done when: a scripted run kills one LICHA and a pin shows it.
(Partially met: the keys and the A_FIGHT entry exist since PR-87; the hit,
the energy decrement and the death state do not. MET IN PR-89: the ATC
window (ATC_ON/ATC_OFF frame flags), the hit through the SAME shape
contact as the push-out, damage 30 (the 0x34103 default), the
[obj+0x102]=14 hit countdown, the engine's hp<0 -> A_DEAD-by-name gate,
and dead objects stop hurting the lion - ZARIA.01's first LICHA dies at
tick 184 in a pinned scripted run. PR-89 also root-caused the
lion-vs-object shape reads (each element's margin lists come from its
OWN .SEQ payload - the old single-payload read used wrong bytes, so the
PR-59/PR-68 contact pins were re-pinned). MET IN PR-90/91 (T3 st. 1/2):
the enemies' OWN attacks through their ATC windows (their A_ATAK ATC_ON
frames vs the lion) and the lion-side AC_ENEMY_ATC raise with his
authored A_DOSTAL reaction. PR-94 UPDATE to the `AC_RECT` plan: the
census over the shipped .MOV corpus shows AC_RECT is authored ONLY in
lion-class scripts (ZARIA A_KILOF(25)=AC_RECT,K_GEN1, SADES A_HOLE,
LONA A_WALKINTOIGLO, TUNGA/TUNGA2 A_EXIT=AC_RECT,AC_BLOK, KADMAR
A_LISC_FALL/END, ZOLW A_JUMP) and NONE of the contact/answer handlers
raises it - it is a lion-side state-entry signal (stone-push/hole/
igloo/exit), not the object hit box; the raiser code and any object-
side consumer stay open. The hit-method slot +0xd0 is family-specific
(111 factory-installed vtable bases, no shared layout) - full protocol
in the PR-94 NEXT_STEPS record. Still open for T2: the proportional-hit
template identity (the energy*30/100+10 id array, PR-96) - see open question 9.)

**T3. Enemy behaviour.**
Patrol between `bounds=` (LANDED in PR-79: BY_X states walk at F/4 px/s and
flip at the bounds range; CH_DIR landed in PR-81 as the facing flip - the
census pairs bounds= with CH_DIR and the video's winning art polarity flips
with the travel direction), detect via the class AI's compiled box
(PR-90 pinned LICHA's; `scope=`'s consumer still open - measurably NOT
that box), `AC_ENEMY_ATC` attack, the
`ATC_ON/OFF` directives and the `F` motions in their scripts. Needs T1's `F`
semantics and open question 3. Done when: ZARIA.01's LICHA/PAJAK/NOSOROG move
and attack in a headless run (log pins), and the contact damage uses the state.
(Partially met: they MOVE and TURN - the verify-play contact leg now hits a
WALKING LICHA (the worm is in its BY_X A_RUN state at the first touch, tick
170 since PR-97 settled both shape boxes on the ground), and PR-82 generalized
facing to follow the travel
direction of every CH_DIR state for objects AND the lion, so a future
charge/push source turns the sprite with it automatically. PR-89 adds
the missing attack MECHANICS: the ATC window is parsed for every script
(the LICHA's A_ATAK opens it at frame -4, closes at -8) and obj_hit is
symmetric. PR-90 lands T3 STAGE 1 - the ENEMY ATTACKS THE LION: the
engine's enemy AI is compiled per class and its class is the placement
token ("LICHA" at 0x9321c vs the .des field 1, 0x616d0), so A_ATAK is
entered by CODE, never by .MOV data; the port's stand-in (OFF unless
LEO_PLAY_DANGER=1) arms the script's OWN A_ATAK by name from a
DANGER-flagged state when the lion enters the pinned LICHA MAIN box
(abs(dx) < 40 && abs(dy) < 50, 0x61b6e), and the enemy lands one hit
through its OWN ATC window (verify-play pins: arm 171, window hit 265,
energy 55). PR-91 lands T3 STAGE 2 - the lion ANSWERS: the window hit
raises AC_ENEMY_ATC on the lion's own set, and ZARIA's authored
A_DOSTAL reaction runs with zero new mechanism (entry row 1 at tick
266, seq=hit art, the `I -320,-250` pullback impulse, wrap exit
A_DRAP(1)=AC_BREAK_NEXT at tick 356; selftest + play legs pin the
whole chain; contact hits deliberately do NOT raise it - open question
10). PR-92 lands the OBJECT-SIDE half of the contact protocol: the
touch raises AC_ENEMY (bit 7) on the touched object - the engine
packet bit (0x51050 `or dh,0x80`) - and the object's OWN A_ATAK(1)/
A_DEAD(1)=AC_ENEMY lines react (obj_touch_enemy; a second non-solid
shape pass outside the SOLID loop, because no consumer state carries
SOLID; the selftest pins the shipped RBLOK/GPTAK chains, the two
contact legs pin the raise at 170 in both (PR-97 re-pinned these; every
older pin in the same legs is
byte-identical). What remains for T3: the engine's own driver and ATACK-side test
(open question 10), the other classes' gates, live object-side
consumption (swimming/flight locomotion - see open question 10), and
`scope=`'s real consumer (measurably NOT the attack box); the attack
itself has NO authored motion - the worm stands and swings, matching
the frames. PR-97 UPDATE to those tick numbers: every grounded y in the
play legs is now the shown animation's shape-box bottom standing ON the
.plt line, so the ZARIA.01 chain re-pinned to first touch 170, arm 171,
window hit 264, A_DOSTAL 265, A_DRAP 355, the kill at 183; the per-pin
history in the Makefile carries each number and its reason.)

Scene placement (M4-C line, closed further by PR-93): the user's
ZARIA.01 report led to the audit - placement coordinates WERE already
read; what was missing were the two grammars the census proved (§5):
PR-93 places and draws the static `.SEQ` images (the stone shelves,
14/14 in ZARIA.01, 69 join game-wide, the LONA.05 WODOSPAD2 holes
named) behind the actors, and stops instantiating the 0,0 attached
children (the corner fruits) while joining their scripts unchanged. The
before/after probe method (byte-deterministic pinned camera,
LEO_LEVEL_CAM/CAMY/PPM_TICK) is recorded in the PR-93 NEXT_STEPS
record. Open follow-ups are §7 item 11 (statics animation, the child
spawn trigger, panc.seq's coordinate reading, ZAR.BLK/GRP formats).

**T4. Signal bus, pickups, inventory.**
Resolve open question 2 (RESOLVED in PR-88: the full name->bit table decoded
from 0x48000/0x919ec, K_GEN1=54 confirmed), then: the `GEN`/signal delivery
between objects (OPEN: who routes a signal raised on one object's set into
another's conditions), the
`AC_*` actions (`PLAT_OFF/OUT`, `XCHG_LEVEL`, `SHOW_CROWN`, `MSG_PUSH`, `LEAVE`),
pickups (`GRABOBJ` handler at slot 0x78 = 0x3db90, `KORONA`, `SURPRISE`, stars
`GWIAZDKI`), the item box (`TOOLS.SEQ` icons; the video shows the held item),
tools use (`K_F3/F4`: stone/pickaxe/stick). Done when: ZARIA.01 can be played to
the stone-corking puzzle (walkthrough in NEXT_STEPS PR record / the Polish
walkthrough page) and each step has a log pin.

**T5. Ladders, vines, climbing.** States `A_WSP`, `A_WSP_LIN`, `A_PODCIAG`,
`A_SCIAG`, objects `DRABINKA`, `LINA`; conditions `AC_DRAB_CATCH`,
`AC_LINA_CATCH`, `AC_EDGE`. Depends on T4's conditions.
STATUS after PR-109: the engine's side is read - all 12 authored BY_TY
states (A_WSP `climb`/`climb_r`, A_WSP_LIN, TUNGA2's A_DRAB, in
ZARIA.01/ZARIA.02 LEW.ZAR + LEW.ZARPL + LEW2.ZAR + LEW2.ZARPL, KARA.07,
TUNGA.11, TUNGA2.12), the mode-4 feed 0x5441b with its `[obj+0xbe]`
guard, the input-apply 0x549f0 that writes that register from a contact
record, and the fact that the entry/exit conditions are
AC_DRAB/AC_DRAB_CATCH/AC_LINA/AC_EDGE, which no port code raises. The
port's model AT THAT POINT (plain ms clock, no vertical feed) was pinned
by a `--play-selftest` leg.
DECIDED, by the owner's clip (PR-109): the row clock is the frame-loop
ITERATION count, NOT milliseconds - an authored threshold T is displayed
for T/100 iterations (measured 66-100 ms on screen for the authored
200/300 of `[A_WSP]`, and no pose held 5 video frames or more in 5 s),
so the port's ms row clock is 3.0-3.5x too slow and PR-110 is the
change that makes it count iterations, with its test plan named in that
record. The clip also fixes what these states are: rung-ladder /
DRABINKA climbs (`seq=climb`/`climb_r`), not vine or rope hangs; ENA2's
rope state A_CLIMB is BY_Y (mode 1) with a |v_y| feed and thresholds of
4, a velocity-coupled animation and a different mechanism.
PR-110 SHIPPED THAT CHANGE: the port's row clock now counts
1/100-iteration units with one iteration = `LL_ITER_MS` = 28 ms, fed
through a whole-millisecond carry, and the mode split of 0x543a2 is in
both the player and the object path - a BY_X state is fed by travelled
distance alone, every time mode (BY_TY included) by 100 per iteration.
The BY_TY selftest leg was re-derived on that grid (a 1700-unit list =
17 feeds), and the legs of every other task that quoted a row duration
moved with it, each with its derivation in the Makefile. The port still
does NOT model the `[obj+0xbe]` guard of mode 4, nor any vertical force.
STILL OPEN: the vertical DISTANCE law (measured ~100 source px/s, and
nothing in the repo turns `F=0,100 T=160,50` into it), which conditions
the port should raise to enter and leave a climb, and PR-108's question
1. **THE CLIP STILL WANTED: a hang with no vertical travel - a hold at a
rung, or the pause at the top before the step off** - which says directly
whether the animation freezes without vertical force. The clip that
answered question 2 shows continuous climbing, so it could not answer
that one, and it could not be tied to a level (the `[A_WSP]` block is
identical in ZARIA.01/ZARIA.02/TUNGA.11, so the timing conclusion does
not depend on which). Full wording in the PR-109 record.

**T6. Level flow.** `end_level` directive, `AC_LEAVE`, the vulture ride of
ZARIA I, the map -> level -> map loop, TUNGA I <-> II exchange
(`AC_XCHG_LEVEL`), lives/energy persistence, game over. STATUS.LEO progress is
blocked by open question 5 - ask the owner for a paired capture (menu before
and after finishing a level) or leave progress unimplemented and say so.

**T7. RAN's balloon mode.** Episode 9 of the video shows it (you fly right,
birds attack the canopy, vulture boss throws bombs). Lion states with scripted
gravity (`G=400`), the `BAL_RAN`/`GPTAK_RA`/`SEP_RAN`/`PIOR_RAN` objects. Until
done the contact pass is OFF for RAN.01.

**T8. Sound effects.** `sample=` lines and `smpauto` frame words -> the `SOUND`
container (WAV samples); the audio path exists for music (`ll_audio.c`).

**T9. Menu/map polish.** Progress arrows, the "start" variant (element 1 of
`LEVELnn.SEQ`), the grey element 2, the mouse pointer (`MOUSE.SEQ`).

**T10. Fidelity harness.** A script that renders a level at a given state and
lays it next to a reference frame (the owner's timestamps) - the manual
procedure in section 9 automated. Never commit frames.

**T11. Cleanup (low risk, do when blocked).** Independent MOD mixer oracle;
the duplicated sparse-table scans (verify_pcm/verify_help); hard-coded
`/tmp/opencode` paths in tools; four tools without a Makefile target; the flaky
Wine restart-confirm leg; stale README/PLAN statements; NEXT_STEPS.md is 3500
lines - add a table of contents or split.

## 9. Method notes

Reading the machine code (how every engine fact above was found):
1. `tools/re/make_text_asm.sh`; find a string address in `leo.elf`
   (python `re.finditer`), grep the asm for `0x<addr>` to find the code using it.
1b. For anything beyond one literal grep, build the index first:
   `python3 tools/re/leodb.py build` then `callers`/`refs`/`slots`/`tables`/
   `sql` (see tools/re/README.md). It indexes the disassembly, every dword of
   the image (with an `aligned` column, so instruction bytes masquerading as a
   pointer are visible as such), branch xrefs, instruction immediates and the
   class tables. It is for FINDING, never for READING: the exact bytes at a VA
   still come from objdump (`leodb.py dis`, identical to `dat.py`), and sweep
   rows that another instruction overlaps are flagged `[SUSPECT]`. PR-106's
   method (call graph, not grep) is what this tool automates.
2. Class methods are virtual: find the class by its factory (0x4df10 maps the
   first field of a placement line to a vtable), read the vtable (dwords at
   the vtable address in `leo.elf`, VA - 0xf000) and the slot you need.
3. Decompiler output (`build/decompiled/all`) loses register arguments; trust
   the disassembly, state the operand identities in the NEXT_STEPS record.
4. Prove it: a hand-derived data-free test + a mutation check.

Measuring on the reference video (episodes 1-11 of the playlist
`PL6B7604DEF5B643F9`, ids: 1 xPiXBmPEdXg, 2 ti6IYfhh1bI, 3 uSZ9HekkBiw,
4 HJLaOqiLeys, 5 fxJrvBwtL-g, 6 v2LElzvk_pQ, 7 v-cJ0HcyHfg, 8 af0puAqzcMA,
9 HGmAqEa5FpQ, 10 O6-GbC1ZVmk, 11 p8TWx_PXanM):
- crop the game picture `crop=640:400:0:40`; it is the 320x200 view at 2x.
  Measured again in PR-111 (HANDOFF section 3, R1): for GAMEPLAY that is
  exact - rows 40..439 of a 640x480 frame at 2x, full width (motion
  profile over a 6 s window at t=314-320). It is NOT the right box for
  the rest of the file: in the window t=118-124, which shows one of the
  game's own `.IS` cutscenes (an animated cave interior with the lion, a
  dog and a spotted big cat at a table), the black bands reach to column
  36 and row 65; the level title cards are black-background text and
  their band is not pinned. Never reuse one crop across segment kinds -
  measure the band per segment (section 3).
- The full playthrough (section 3, R3) carries the game's own title cards,
  "ENA I / underwater journey", "KADMAR I / ancient mysteries" and the
  rest, and the owner has since supplied its level-by-level timestamps:
  those cards are the cleanest level-start markers the project has.
- That same file has the uploader's decoration baked into the picture: a
  static 128 px column at x 1152..1279, which the owner asked to keep in
  mind (section 3, R3, "THE BAR ON THE RIGHT"). Always crop
  `crop=1152:720:0:0` first; it is opaque art, so it is found by
  per-column TEMPORAL std (0.0-0.1 there, 3-24 inside the game window), not
  by looking for black.
- 480p compression and a scrolling camera limit precision; prefer segments the
  owner points out ("jump in place", camera not scrolling). Scripts: tools/re.
- The owner's timestamps so far: ep.1 5:16-5:19 (vine jump, used), ep.2
  0:53-0:55, 0:56-0:58, 7:13-7:16, 7:44-7:47 (all with a scrolling camera).

## 10. Checklist for every PR

- [ ] one logical step, evidence recorded (machine code address / data file /
      timestamp), estimates named in code AND in NEXT_STEPS
- [ ] data-free hand-derived test + mutation check
- [ ] `make`, `make win` warning-free
- [ ] `make -k verify-all` EXIT=0 with fixtures and fixture-free - not run
      for a pure-docs commit (section 2 rule 4's exception, 2026-11-17)
- [ ] README hook list updated if a `LEO_*` variable was added
- [ ] NEXT_STEPS.md record; this file's sections 1, 6, 7, 8 updated
- [ ] commit "PR-NN: ..." ; `git push`; `gh run list --workflow=ci.yml` green
- [ ] nothing copyrighted staged (`git status`)
