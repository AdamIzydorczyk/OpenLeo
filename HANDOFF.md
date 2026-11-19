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
| lion (play mode) | walks on the level's own ground (`.PLT`) with his current ANIMATION's shape-box bottom as the contact line (PR-97: `ll_player.contact`, `ll_seq_shape_contact`, `mov_state_contact`), runs the engine's OWN velocity law once per 50 ms iteration - `v -= T*v/100` then `v += F*s/100` with the key intent `s` = ±10 (PR-135, replacing the fitted target-velocity model) - and his row list admits AT MOST ONE row per iteration, fed by travelled distance in a BY_X state and by the engine's constant 100 otherwise; jumps (ESTIMATED units), keys facing-relative (Model M, PR-85), camera follows (lead 100/220 - PR-136's absolute camera read CONFIRMS the right lead at 95-108; PR-137 APPLIED the grounded anchor at 128 + the world clamp (the 175..180 band was the clamp, not the anchor), pinned hermetically by `ll_level --follow-selftest`), HUD shows energy/lives |
| collision | push-out ported from the machine code (`FUN_00042de0/42cf0`), solid/semi-solid response, damage from an enemy ONLY through its own ATC window (the authored `atc_on` rows) and the 0x550c0 clash rule - a bare touch never hurts the lion (PR-139 deleted the port's unconditional contact damage); the enemy's own ATC-window attack when armed (PR-90, off unless `LEO_PLAY_DANGER=1`), balloon pickup; ON by default except RAN.01 |

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
   NOTE (2026-11-19): the Windows halves of this gate - `make win`, the
   wine legs - are deferred to the end-of-implementation pass by rule 8.
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
8. **Linux-first: the Windows version is not part of the day-to-day gate**
   (owner ruling, 2026-11-19). Implementation targets Linux; the Windows
   target is verified ONCE, at the end of implementation, deliberately:

   - end-of-implementation pass: `make -k verify-windows` (the Makefile
     moved the pure-Windows legs - `verify-win`, `verify-mod` - out of
     `verify-all` and into that target; `make -k verify-all WINTEST=1`
     still expands to the old full gate, byte for byte), plus `make win`
     warning-free and the `.exe`/MSYS2 job in
     `.github/workflows/ci-windows.yml` ("Run workflow", or push a
     `windows-gate-*` tag). `make -k verify-all-windows` = both at once.
   - per-PR gate that REMAINS: `make` warning-free; `make -k
     -j"$(nproc)" verify-all` EXIT=0 with the fixtures AND fixture-free
     (parallel measured 2026-11-19: 124-159 s serial -> 29-40 s, identical
     verdict sets; the Windows/wine half stays serial); `ci.yml` (now
     `linux` + `asan` only) stays green. This rule supersedes the
     `make win` and Windows-leg halves of rule 4 for day-to-day work.
   - NOT deferred, and this bit is load-bearing for CI: Windows
     BUILDING wherever a Linux leg's prerequisite chain asks for it.
     The Makefile names .exe targets as prerequisites of the MIXED
     legs - `verify-menu: ll_cutscene ll_cutscene.exe` (Makefile:1744;
     the same shape in verify-skip/-music/-pcm/-help/-cmake) - so a
     Linux-only CI job still cross-compiles Windows binaries and still
     needs the pinned vendor SDL2 (`ci.yml`'s "Provision SDL2" step,
     3 s). Measured: dropping that step failed the LINUX gate with
     `fatal error: SDL2/SDL.h` at ported/ll_audio.h:38 and
     ported/ll_view.h:41, then `make: *** [Makefile:78:
     ll_cutscene.exe] Error 1` (run 44536198608; reproduced locally
     over all 18 TUs of $(SRC) with SDL_WIN=/nonexistent, and 0 errors
     with the pinned tarball). Rule 8 changes what is TESTED per push,
     not what make is allowed to build.

   Measured effect of this rule, for the record (2026-11-19, runs
   44523679146 before / 44536707174 after): the per-push `linux` job
   went 304s -> 59s, because the 247s `Install toolchain` step is now
   a 25-30s image pull; `asan` 46s -> 48s; and the end-of-implementation
   gate `ci-windows.yml` is proven green when dispatched (run
   44536359825: cross 54s, native MSYS2 smoke 75s).

   Measured reason (2026-11-19, fixtures staged, every leg timed, all
   EXIT=0 except as noted): the local suite is 307 s and `verify-win`
   alone is 164.8 s = 53.7 % of it. The cost is neither the compiler nor
   wine machinery - a wine start measures 0.1 s and the .exe decode of
   1020 frames 0.1 s - it is real-time-paced demo/menu/level playback.
   The port is ONE shared codebase, so every engine path those legs
   exercise is already exercised by the Linux legs; what the Windows half
   adds is second-target build proof, which does not change while the
   port changes. Honest note on the same run: one full-gate run failed at
   `Makefile:139` (the wine `--skip-at 10` half of `verify-win`), a leg
   that had passed ~25 min earlier when run on its own - it is the
   suite's most expensive and wall-clock-paced leg, which is itself an
   argument for running it on purpose, once, instead of per PR.

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

THE LEVEL INDEX - WHERE EACH LEVEL CAN BE WATCHED (2026-11-18, at the
owner's request: "should be references to levels", "on videos",
"names"). The 13 names are the `ETAPY` stage ids, and the stage-to-
chapter correspondence is not a guess: every LEVELDES record carries a
`dir=` key giving its stage, and the port already reads that key
(`ported/ll_level.c:5724`, `:5758`, `:6023`). All 13 pairs, from the
extracted records in `/tmp/opencode/lvdes`:

    ZARIA1.DES   dir=zaria.01    ZARIA2.DES   dir=zaria.02
    ENA1.DES     dir=ena.03      ENA2.DES     dir=ena2
    KADMAR1.DES  dir=kadmar.07   KADMAR2.DES  dir=kadmar2
    KARA.DES     dir=kara.07     LONA.DES     dir=lona.05
    RAN.DES      dir=ran.01      SADES.DES    dir=sades.06
    TUNGA1.DES   dir=tunga.11    TUNGA2.DES   dir=tunga2.12
    JUNCTION.DES dir=junction

Each level's own title card was READ frame by frame at the times below.
The card's first line is the chapter name the LEVELDES record is named
after. Its second line is a subtitle, and R3's cards are drawn in
ENGLISH while the Zax37 episodes' are in POLISH: the game ships ENG,
GER and POL tables next to the LEVELDES records (`/tmp/opencode/lvdes/
ENG`, `GER`, `POL`, `POL.ORG`), which is why both appear. The two
agree word for word - R3 "LONA / land of penguins" is ep.5 "Kraina
Pingwinow" (= Kraina Pingwinow), "SADES / deep down" is ep.6 "Segajac
Dna" (= Siegajac Dna), "KADMAR II / egyptian dark" is ep.8 "Egipskie
Ciemnosci", "ENA II / pirate treasure" is ep.4 "Pirackie Skarby", and
ep.2's own card was read at t=49 s of what is now
`reference/video/zax37/ZARIA.02.mkv`:
"ZARIA II / zar tropikow" (= zar tropikow). (This file drops the Polish
diacritics to keep the columns narrow; the `.info.json` files beside
the videos keep the upstream titles exactly as upstream has them.)

The Zax37 chapter files themselves now sit in a folder of the same
names: `reference/video/zax37/`, named by the stage ids above (owner's
request, PR-127 - they used to be `epNN_<id>`), `.mkv` or `.mp4`;
each file's original YouTube id lives on in its paired `.info.json`.
They are edited and cover more than one level each - see the episode
notes below the R3 ranges; they are references for behaviour, never
for a level's length.

RANGES IN R3 (`OvbbJTmp79I.mkv`). Times are seconds in the file; at
60 fps a frame number is seconds x 60. All of them are +-2 s, because
the detector samples once a second and an accurate-seek extraction lands
up to 2 s away. The split files are `reference/video/levels/<stage
id>.mkv`, made from R3 with `-c copy` at exactly these boundaries (each
starts about 2 s before its card); the 4.77 GB master is KEPT and stays
the source of record for any measurement. A range listed twice means the
player entered that level twice in one sitting.

  ZARIA.01    "ZARIA I / first steps"      0-458, 1924-2344. The 0-458
            range rests on the owner's map alone: no card was detected
            there. The card at 1931 was read.
  ZARIA.02    "ZARIA II"                   458-786, 2344-2699. 2353 was
            read by PR-111; at 462 there is only a black hold.
  ENA.03      "ENA I / underwater journey" 786-1577, 2699-3229
  ENA2        "ENA II / pirate treasure"   1577-1899, 3229-4455
  LONA.05     "LONA / land of penguins"   4455-4937, 5062-5792
  SADES.06    "SADES / deep down"          4937-5012, 5792-7170
  KADMAR.07   "KADMAR I / ancient mysteries" 7170-8619. The card is
            re-shown at 7637: a second attempt at the same level.
            Censused end to end by PR-126 and PR-128: the placed stage-1
            BOSS object is on screen in NO window of this file. The boss
            FIGURE that repeats here (283-293, 358-369, 411-419, 565-576,
            762-774, 790-802, 1162-1172, 1334-1344 s) stands in a black
            room between two white-outlined wardrobe doors with no HUD
            drawn - a scripted scene, not that placed object.
  KADMAR2     "KADMAR II / egyptian dark"  8619-15363
  RAN.01      "RAN / two weeks in a baloon" 15363-15764 - "baloon" is
            the game's own spelling. A still picture of a pink balloon
            over a night desert at 15651 is in-level art, not a card.
  KARA.07     "KARA / magic castle"        15764-16280
  TUNGA.11    "TUNGA I / so close"         16280-16944
  TUNGA2.12   "TUNGA II / the last battle" 16944-20263. The two TUNGA
            cards re-appear at 16933, 17111, 17718, 17759, 17795,
            17907, 18889, 19284: repeated attempts, not new levels, and
            none of them is in the owner's map.
  JUNCTION    NOT IN THIS CAPTURE - no card and no candidate anywhere
            in the 5 h 39 m; the owner's map has no entry for it
            either. Recorded as absent from this file, not as absent
            from the game.
  not a level: 1899-1924 and 5012-5062 are menus (the map's `31:43`
            and `1:23:36` MENU marks), 20263-20389 is the ending and
            the end screens.
  The 24 files, named exactly as listed here: the stage id for the
            first visit, `<stage id>_b` for the second visit in the
            same sitting, `TUNGA_replay` and `TUNGA2_replay` for the
            stretches where a TUNGA card is re-shown but no new level
            starts, `UNRESOLVED_19282_20265` for the stretch after the
            last identified card, and `MENU_intro`, `MENU_2`, `ENDING`.
            Cut with `-ss <start> -i OvbbJTmp79I.mkv -t <length>
            -c copy`, so each starts at the keyframe at or before its
            start second; 4.5 GB total, master kept, nothing staged
            (`reference/video/` is gitignored).
   BEHAVIOUR MEASUREMENT INSIDE THESE FILES NEEDS A STILL WINDOW. The
            game camera does NOT scroll smoothly in R3: it holds dead
            still for 20-30 s stretches and jumps discretely between
            them (PR-125, tools/re/video_stills.py: ENA.03 20.5-50.0 s
            at mean drift 0.54 px/s, LONA.05_b 480.0-504.5 s at 0.35,
            KADMAR.07 still runs at 712.5-728.5 / 763.0-774.0 /
            791.0-802.0; drift is game px/s of the 640x400 frame, and
            across a whole cut its distribution is BIMODAL - runs at
            0, then jumps saturating the search window, a p50 that is
            contamination from moving decor, not a camera speed).
            Screen dx equals world dx only inside a window
            like those; `tools/re/video_lane_shift.py` then measures
            the band translation. Numbers in the PR-125 record.
            PR-128 adds the other half of the same question, whole-file:
            tools/re/video_blob_scan.py labels every sampled frame "HUD
            drawn" or "picture" (on KADMAR.07 the two classes separate
            with a 34x margin, no ambiguous frame) and lists where one
            chosen RGB appears, so a named class can be searched across
            24 minutes in a 23 s pass. On that file the 93 still windows
            split 78 gameplay + 15 picture windows, one per no-HUD
            segment, and the class its .des places is in none of them. A
            colour names a CANDIDATE only: one palette value serves the
            boss robe, the HUD lion portrait, the jungle decor and the
            player's own robe costume.

HOW THE BOUNDARIES WERE FOUND. `/tmp/opencode/stillscan.py` samples one
cropped 320x200 grayscale frame per second and reports runs of 3 or
more BYTE-IDENTICAL samples. A title card is a 3-5 s pure-black hold
(black fraction 1.0000, bright pixels 0) followed by 4-8 s of static
text (black 0.90-0.94, bright 1100-1830, text band at rows 50..146 of
the cropped window); gameplay is never identical for 3 s, because the
lion idles even when nobody moves. Over the whole file that gives 63
runs, of which about 25 are cards; requiring at least 200 bright pixels
in the band is what separates cards from fades. Two cards are missed by
it, "ZARIA I" at 1931 and "ZARIA II" near 2353, because those cards are
not static - both were read anyway, so a negative from this detector is
not evidence of absence. The same pass finds NOTHING on the 480p
episodes, because YouTube's re-encode makes a still card differ by
0.4-0.6 mean luma every second; for those files the usable test is
`/tmp/opencode/cardshape.py` (black fraction plus a compact bright
band), and its hits include the uploader's own "LEW LEON" intro, which
appears at 9-13 s of EVERY episode - it was read, it is not a game
card. Neither detector is a verdict: every name above came from reading
the frame.

THE MAP IS NOW CONFIRMED, including the two entries PR-111 flagged as
bad. Every mark in the time map above is within a few seconds of a
detected card except `43:59 ENA 2`, which stays wrong exactly as
PR-111 read it. And `7:77` reads as 7 min 77 s = 477 s: there is a
black hold at 462, consistent with a level change there. That is an
inference about a malformed mark, not a read card, and it is the only
remaining doubt about the map.

ADDITIONS TO THE THREE ENTRIES ABOVE (new records, not edits of shipped
wording):
- `frame 2353`, `frame 2713` and `frame 7233` in the "WHAT WAS CHECKED
  AGAINST THE FILE" paragraph are SECONDS (39:13, 45:13, 2:00:33), not
  frame numbers: this file is 60 fps, so those cards are at frames
  141180, 162780 and 433980. The paragraph's own conclusions are
  unaffected.
- R1's footage is also on disk as `reference/video/zax37/ZARIA.01.mkv`
  (it came down with the playlist; PR-127 renamed it from
  `ep01_xPiXBmPEdXg`). Same container size 41 034 044 B,
  different container sha256, and the h264 elementary streams are BYTE-
  IDENTICAL: both 34 046 474 B, sha256
  4fbab8d52dc75cc14f49e9b45a9bf8a29403cd66de33c3cc4a52456f0a3399e3.
  The audio streams differ (6 917 407 B each, sha256 6895526a04b46836
  against 1eac4609fbcf3c6a). So they are the same encode of the same
  video and every measurement in this file applies to either. R1's path
  stays the name of record, because the entry above cites it; the
  duplicate is kept, not deleted.
- R2 (`ladder-DomCTOtx2rg`) is inside the ZARIA chapter. The owner
  supplied the mark `https://www.youtube.com/watch?v=DomCTOtx2rg&t=50s`
  with the words "here only zaria" (2026-11-18), so PR-109's climb at
  46.6-48.3 s is in a ZARIA level. It is still unfit for position
  claims (1.8x, 30 fps), so nothing measured on it changes.
- The 11 playlist episodes are all on disk, kept whole and named by
  the level each one's title names - the stage ids of the index above
  (owner's request; PR-127 renamed them from `epNN_<id>`), as
  `reference/video/zax37/`: ZARIA.01, ZARIA.02, ENA.03, ENA2,
  LONA.05, SADES.06, KADMAR.07, KADMAR2, RAN.01, KARA.07,
  TUNGA2.12. 546 468 452 B in total, byte-identical to the count
  PR-124 took: 8 are `.mkv` and 3 are `.mp4` (SADES.06, KADMAR.07,
  KADMAR2), all h264 640x480 at 25 fps, so they belong to R1's
  geometry class, not R3's, and R1's `crop=640:400:0:40` warning
  about measuring the band per segment kind applies to each of them.
  Their `.info.json` files keep yt-dlp's pairing rule
  (`<basename-without-extension>.info.json`, NOT `<file>.info.json`
  with the extension - that is why a first extraction printed "?" for
  every field), and each id and upstream title lives on INSIDE its
  `.info.json`, so the rename loses nothing. The two playlist-level
  metadata files are kept as `playlist_PL6B7604DEF5B643F9.info.json`
  and `.description`. To refill the whole set: PR-124's download line
  (whole episodes, playlist order), then the rename table:

    ~/.local/bin/yt-dlp --ignore-errors --write-info-json \
      --write-description -f "bv*[height<=480]+ba/b[height<=480]" \
      -o "ep%(autonumber)02d_%(id)s.%(ext)s" \
      "https://www.youtube.com/watch?v=xPiXBmPEdXg&list=PL6B7604DEF5B643F9"

    ep01_xPiXBmPEdXg -> zax37/ZARIA.01    (same extension; rename
    ep02_ti6IYfhh1bI -> zax37/ZARIA.02     the .info.json and
    ep03_uSZ9HekkBiw -> zax37/ENA.03       .description sidecars
    ep04_HJLaOqiLeys -> zax37/ENA2         to the same base name,
    ep05_fxJrvBwtL-g -> zax37/LONA.05      or yt-dlp stops seeing
    ep06_v2LElzvk_pQ -> zax37/SADES.06     them; the ep00_/ep11_
    ep07_v-cJ0HcyHfg -> zax37/KADMAR.07    playlist metadata goes
    ep08_af0puAqzcMA -> zax37/KADMAR2      to playlist_PL...*)
    ep09_HGmAqEa5FpQ -> zax37/RAN.01
    ep10_O6-GbC1ZVmk -> zax37/KARA.07
    ep11_p8TWx_PXanM -> zax37/TUNGA2.12

  Which level each episode is about comes from its title: the titles
  are the game's own card subtitles (proved by reading ep.2's card -
  `zax37/ZARIA.02.mkv` - and cross-checked against R3's English
  cards for ep.4, ep.5, ep.6, ep.8; "ep.N" below means the Nth
  line of that table, upstream playlist order).
  Reading the cards inside the rest is still open: the shape test found
  candidates at 181/203 s of ep.1, 114/193 s of ep.4, 106/396 s of
  ep.5, 62-282 s of ep.6, 67/265/294 s of ep.8, 146-420 s of ep.9,
  54/300 s of ep.10 and 124/298/506 s of ep.11, and it finds NO card
  in ep.3 or ep.7 at all. Episodes cover more than one level and are
  edited, so they are references for behaviour, never for a level's
  length.

`/tmp` is a tmpfs: after a reboot restage the data (recipe in NEXT_STEPS.md,
"restaging"). The reference captures are NOT in /tmp any more - they live
in `reference/video/` on disk; if they are ever lost, the `yt-dlp` lines
under "Reference captures" rebuild them (pip install yt-dlp).

Build and test (day-to-day, Linux-first - rule 8):

    make                      # ll_cutscene (linux);  make ll_level  builds the level tool
    make -k -j"$(nproc)" verify-all \
         DATA=/tmp/opencode/iso_stage/LEO HD=/tmp/opencode/hd_files
    make -k verify-all DATA=/tmp/opencode/nope HD=/tmp/opencode/nope   # fixture-free
    make verify-play verify-level ...                                 # single legs

The day-to-day gate is run IN PARALLEL (owner, 2026-11-19, measured):
serial 124-159 s vs `-j22` 29-40 s and `-j4` 38 s, every run rc=0 with
the SAME verdict lines. Safe because the legs' scratch paths are disjoint -
the one shared file is the read-only py-refs oracle frame, produced by a
prerequisite. Caveat stated honestly: raw log TEXT is not bit-reproducible
under load (one music-pump verdict line prints a wall-clock-derived
duration; its assertions, e.g. peak=12706, do not move), so the gate is
rc FIRST (that is authoritative: `make -k` returns non-zero if any leg
failed) and then the legs' own verdict lines. Do NOT reduce that to a
count of `(ok)` lines - measured 2026-11-19, that pattern undercounts,
because verdict wording is per-leg: verify-mod prints `MOD <name>:
parse+render linux == wine (fnv=<hash>)`, others print "byte-exact" or
"== python reference". A pattern that covers the fixture-backed proofs is
`grep -cE "\(ok\)|byte-exact|== (python|wine)|render 3000ms"`, and the
real check is that the SET of verdict lines matches the previous run.
The Windows half stays SERIAL: verify-win and verify-mod share one
WINEPREFIX.

The Windows pass, run ONCE at the end of implementation (rule 8, owner
ruling 2026-11-19 - not part of the day-to-day gate):

    make win                                        # cross build, warning-free
    make -k verify-windows DATA=... HD=...          # verify-win + verify-mod,
                                                    # WITH the game data
    make -k verify-all-windows                      # both halves at once
    make -k verify-all WINTEST=1                    # the old full gate

    # No extra MOD staging is needed for that pass - verify-mod pulls the
    # three records out of the SOUND container itself (the same
    # tools/pull_record.py CONTAINER KEY OUTFILE the Makefile calls, into
    # /tmp/opencode/<NAME>.mod). Checked by running it here 2026-11-19:
    # with DATA staged it takes 4.5s and prints three cross-target verdicts
    # (MOD TUNGA/RAN/ENA2: parse+render linux == wine (fnv=...)).
    # An earlier note here claimed it "proves nothing, 0.2s, zero
    # verdicts" - that was MY grep, not the suite: see the gating note
    # directly below, which is the correction.
    # CI side of the same pass: .github/workflows/ci-windows.yml (cross .exe
    # build + the MSYS2 native smoke), run from Actions or by pushing a
    # windows-gate-* tag. ci.yml itself is linux + asan only now.

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
- A SECOND flaky leg, now counted: CI's `windows-smoke` job, step "Run them
  (committed fixtures only; no game-data staging)", check `sleep_until
  honors the deadline (>=20ms)`. Out of all 125 `ci` runs the workflow has
  ever had, exactly 2 failed on that one check: run 101 / 44043011593 at
  07:59 UTC and run 124 / 44176302862 at 21:06 UTC, both on
  `windows-latest` msys2 MINGW64 (the three other failures in the history
  - runs 1, 2, 8 - are real setup breakage: SDL2 headers absent, the
  data-backed fixtures absent, a link error). Rerunning the failed job of
  run 124 passed on attempt 2. Both failing trees had byte-identical leg
  code (`git diff f1afa64..HEAD --stat -- ported/ tools/ Makefile
  cmake/` empty for 124), so it is a timing assertion, not a regression.
  Fixed in PR-112: the headless backend trusted the CRT's
  absolute-deadline sleep instead of re-checking its own clock (the SDL
  backend has always re-checked); the selftest now samples the contract
  20x at zero tolerance and prints the raw CRT call's undershoot as a
  diagnostic. The mechanism is now SEEDED by a direct measurement, two
  runs after the fix: that diagnostic on a real Windows runner reads
  `diag raw-CRT clock_nanosleep 20ms x20: early=1 unsupported=0
  worst_undershoot_us=40` (`ci.yml` run 127 / 44180333781, job
  134382654217, log line 444) - a raw `clock_nanosleep(CLOCK_MONOTONIC,
  TIMER_ABSTIME)` call returned 40 us BEFORE its absolute deadline. It
  has printed `early=1 worst_undershoot_us=11` again on run 128 /
  44181397463 (job 134386350669) and `early=0` on run 126 /
  44179955442; all three glibc legs print `early=0`. Two of three
  Windows runs, none of three glibc runs: exactly the intermittent,
  Windows-only, sub-millisecond shape the two failures had. Not proof
  they were caused that way (11 and 40 us are small samples), but the
  old code was wrong to trust that call, and the fix covers it whatever
  the true magnitude. If the leg ever fails again, the diag line in that
  job's log is the first thing to read.
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
| jump impulse unit | ll_play.h `LL_PLAY_JUMP_DIV` | `I`/4 u/s | ONE measured jump (vine, ep.1 318.6 s); PR-86 re-confirmed on ZARIA live: A_JUMP `I 150,-1200` -> apex ~24 u / 0.36 s, matching the video's jump population (gp.mp4 feet-track scan: clean candidates 23-30 u over 8-13 frames). PR-135 note, counted open: this pair is per-second arithmetic and the vertical half of the law was NOT re-fitted to the 50 ms iteration grid, so the peak the port reaches is now 20 px above take-off (min y 628 vs the 648/649 walk line) - 4 px below the ~24 u PR-73/86 fitted against, though within 4% of the 21 u single jump PR-74 measured (the hermetic jump leg's own comment). The constants are unchanged on purpose: one measured frame cannot fit four constants |
| jump gravity | `LL_PLAY_JUMP_G` | 1650 u/s^2 (= G.y 55 x 30) | same jump; same PR-135 note as the row above |
| jump-state ending | ll_play.c | a state that DECLARES an AC_BREAK_NEXT exit ends at its frame-list WRAP (the landing no longer cuts it); forced ->A_STAY/A_DRAP kept ONLY as fallback for states with no such line | PR-86: real A_JUMP/A_JUMP_S declare A_WALK(7)/A_DRAP(1)=AC_BREAK_NEXT; ZARIA.01 leg pins it |
| `TARGET(N)=COND` entry | ll_play.c `ll_mov_row_start` (used by BOTH the player path and ll_level.c `obj_transition`) | numeric (N) = start ROW (1-based) in the target's frame list; non-numeric = top; an N past the end clamps to the top | PR-86 evidence chain in the struct comment (PRE_JUMP skips A_JUMP's impulse row; A_WALK(7) mid-cycle re-join pinned on real data); PR-107 read the store (`entry-1` at 0x51e3b..0x51e49) and the use: the state record's live row field `rec+0x4`, written raw at 0x523cb by the row-setter, which is the record's own vtable slot +0x1c in the table every record stores at rec+0x44 (0xa25d4, from its constructors 0x524cf/0x52508), and which the transition step 0x535a2 calls on the new state (0x535ea). Two MODEL CHOICES, recorded in the helper's comment: the engine writes the value RAW there, so clamping an over-long row is ours; and "absent / non-numeric" is -1 = "leave the row alone" (0x51e3e, 0x52381), which only COINCIDES with the top because a row list wraps. One row counter per object (`o->fi`) vs the engine's one per state record is also ours |
| The unit of the row clock, and the BY_* mode feed (0 BY_X / 1 BY_Y / 2 BY_TIME / 3 BY_TX / 4 BY_TY) | ll_play.c `ll_player_step` sections 4 and 7 + `LL_ITER_MS` (ll_play.h), ll_level.c `obj_row_tick` / `obj_advance` | MODELLED since PR-110, as ONE accumulator per moving thing fed BY MODE, mirroring the dispatcher 0x543a2: a BY_X state with a non-zero state `F=` adds travelled distance (`|vx| * dt`) and its time accumulator stays at 0; every time mode adds 100 per ITERATION and never any distance, an iteration being `LL_ITER_MS` = 50 ms of wall time (PR-134 measured that edge, PR-135 set it), fed through a whole-millisecond carry (`row_rem`) so no step size drifts, and AT MOST ONE row admitted per iteration on BOTH sides (0x521f0 / 0x5232b / 0x523f0) - PR-135 removed the port's multi-admit loop and the `4 * nfr` cap; the engine gates mode 3 on `[obj+0xba] != 0` and mode 4 on `[obj+0xbe] != 0` (0x543f2 / 0x5441b, PR-108) and the port does NOT model that guard, nor any vertical force — a BY_TY state is simply time-fed, and the climb's vertical DISTANCE law stays open (T5). A row stays current while `acc < thresh`; the compare is AFTER the feed and the threshold is SUBTRACTED on a hit, so the remainder carries into the next row. `time_scale=` is a parse-time scale on the stored thresholds (`thresh * time_scale / 1000`, default 1000, rows before the line unscaled), NOT a duration | The dispatcher and its mode jump table are PR-99's (NEXT_STEPS.md:5439-5447); the feed constant 100 has exactly one writer, 0x4c8b7 (PR-100), and the row comparison at 0x52320-0x5232b hands `acc - thresh` to the row handler rather than zeroing the accumulator (PR-100). PR-108 read the mode-3/4 guards above and that `[obj+0xbe]` is also v_y's force register (0x542cd). PR-109 decided the unit from the owner's clip (the `[A_WSP]` rows of 200/300 measure 66-100 ms on screen, the pacer 0x4b0e0 -> 0x4a120/0x1dba0 holds one iteration to two display frames, the engine's own fallback setpoint is 27) and corrected PR-106's "55-unit steps" and "sound driver's 70.0 Hz PIT" (that write is 7.00 Hz; during play the counter is a real 1 ms clock, 0x5b9e0 set-period called at 0x5d195); PR-110 takes 28 ms, the low end of that bracket, and implements the mode split. OPEN, from the PR-110 record except where marked: whether a row fires AT `acc == thresh` (13932 of 14881 authored BY_TIME thresholds are exact multiples of the 100 feed) - PR-117 pinned the object side at exact equality and PR-135 keeps `>=` after the feed on both sides; the engine's mode-0 feed is the TRUNCATED `v_x/100` of the tick while the port feeds the travelled distance (PR-135 CLOSED the multi-admit loop and the cap, the truncation stays open); the `fx != 0` half of the walk gate (30 shipped BY_X blocks with `F=0,0`, no leg covers one); the AC_TIME pending-flag lifetime; the climb's vertical DISTANCE law. PINNED by: the RC fixture walked on two grids with the exact carry identity and `facc` read at every step (0 in a BY_TIME state), the `time_scale=2000` leg plus its control, the ZM leg reading both accumulators at both ends of the mode gate (no key in a BY_X state: everything stays 0; key held: the time accumulator stays 0 while the list runs on distance), 7 mutations caught, plus one recorded survivor (`mut_nogate4`, section 4's feed-site condition removed: `facc` is only read through the walk gate, so no leg can see it), and every verify leg re-derived by hand (the derivation is written beside each leg in the Makefile) |
| The player's OWN velocity register, and the three numbers in it that bytes do not give | ll_play.h `LL_PLAY_DRAG_DEN` 100, `LL_PLAY_INTENT_FULL` 10, `LL_PLAY_V_EPS` 0.2, `LL_ITER_MS` 50; ll_play.c sections 2 and 7; ll_level.c `obj_walk` | per iteration `v -= clamp(T,.,130)*v/100` (sign flip or a remainder under one register unit -> 0), then `v += F*s/100` with s = +/-10 only while a facing-relative direction key is held in a BY_X state, or for an auto_walk state; NO lower clamp on `T`; NO register on the object side, which steps straight to the steady rate | PR-129/130 read the chain, PR-133 the passes and PR-134 the clock; the steady state F*s/T units/s reproduces PR-134's measured lion (50 u/s at ZARIA's F=100 T=20) and worm (25 u/s at F=100 T=40) inside those brackets; the epsilon, the partial-intent values of s, the engine's raise of T to >= 20, the object register and 50 ms as PR-134's video-chosen edge of PR-109's 28-33 ms code bracket are all named in the PR-135 record |
| jump key | ll_level.c | Space | viewer choice |
| fire keys | ll_level.c `play loop` + platform_sdl `map_key` | Ctrl=K_FIRE, F1..F4=K_F1..4 (weapon selectors), Alt=K_ALT; script tokens FIRE/F1..F4/ALT comma-combinable | PR-87; names from the walkthrough, the .MOV conditions are the contract |
| `,` in transition COND = AND | ll_play.c `parse_cond`/`cond_true` (groups) | `|` ORs groups, `,` ANDs tokens within one; OTHER poisons only its group | PR-87 MODEL CHOICE (evidence in ll_play.h: AC_ONAIR/AC_DRECT pairs, every K_FIRE,K_Fn attack line); A_FIGHT entry pinned real |
| hurt lock | ll_level.c `play_collide` | 30 ITERATIONS of the engine clock (PR-135: that function runs once per iteration of `LL_ITER_MS` = 50 ms, and `hit_cd` counts the same way) | 0x50304 `[+0x102]=0x1e` constant, unit read in PR-133/135 |
| `AC_TIME` | ll_level.c `obj_cond_true`, ll_play.c | obj: = end of frame list OR a raised bit; lion: raised by frame `GEN AC_TIME` (bit 9 = LL_SIG_AC_TIME) | `time=min,max` unit unknown |
| signal bits | ll_play.h `LL_SIG_*`, `ll_sig_id()` | engine table 0x48000: AC_BLOK 0..AC_EV_MAX 68, K_GEN1 54; `GEN <name>` raises any, conditions test the object's own set | PR-88 (verified constants); pulse-vs-latch + cross-object routing OPEN |
| lives / energy HUD | ll_level.c | 6 lives, energy frames (100-e)/10 | video start shows 6 |
| camera | ll_level.c `follow_camx/camy` | lead 100/220, ease 6 u/tick, grounded contact-line-at-128 anchor since PR-137 (178 before it; PR-97: the anchor point is `p.y + p.contact`, the shape-box bottom of the animation being shown (max over its state's frames) = the paws PR-84 measured; y-ease 2 u/tick invented). PR-136 MEASURED the camera in world units from the level's own static placements: leads HELD frame-for-frame 95-108 right (n=57, CAM_LEAD_R=100 confirmed) and 199-251 left (n=19, CAM_LEAD_L=220 inside the band, weak); catch-up rate p50 5 / p95 60 / max 230 u/s vs the port's 6 u/iteration = 120 u/s; the grounded paws row sits at 128 (mode, n=3990 surface frames; 128 unclamped) and 180 only where camy is at the world clamp - one constant 128 plus the existing clamp reproduces both; the view does NOT hold rigidly through jump arcs (camy change p05 -26 / p50 0 / p95 +12, n=139). PR-137 APPLIED the anchor: `CAM_FEET_Y` 178 -> 128, and the old comment's claim that the 175..180 band "is NOT the world clamp" was WRONG - it is, at 681-500 = 181; one constant plus the clamp reproduces both observed rows. No pixel pin moved (the ground-line case clamps either way); every unclamped surface now shows the lion at row 128 as the game does. The leads stay 100/220 (right CONFIRMED, left a weak n=19 band); the eases (6 x / 2 y per iteration) stay models with the measured band beside them, and `./ll_level --follow-selftest` now pins the whole law hermetically - 9 mutation controls, all caught | PR-136 record (tools/re/level_landmarks.py + video_camera_read.py + camera_law.py, /tmp/opencode/pr136/law_v3.txt); PR-137 record |
| ground source on the first screens (G-13) | ll_level.c `platformy=` -> `ZARIA_A.PLT` | DECIDED PR-138: the engine's ground records ARE the authored `.PLT` text - the loader makes one object per polyline line and the line parses itself into the 13-byte records the segment lookup (0x45380) then searches; class 0xa0c78 has exactly one builder in the image, no second ground-record builder exists. PR-136's "surfaces 7-51 units ABOVE at x 128-392" is RETRACTED: those were screen-x values from false-matched single-landmark (tier-2) cameras - frame 4370 proves it (the authored balloon at (1270,460) cannot be on view at the reader's camx 173; the port's own render matches at cam ~(1100,335), where the paws sit on line "1 1171,506;1305,503" within the estimate). Every VERIFIED (tier-1) frame - and they exist only for camera x 1..122 - stands the lion on a .PLT line at d+0 (125 samples, y 681). The remaining ground item is a NAMED PORT gap: the port places ani_plat= objects but never parses their local ground lines (ANIPL2.DES KATAPULT/PALMA entries; engine entity-record tables 0x34850 [obj+0x48]/+0x4c and the MOVEPLAT provider chain unread) | PR-136, PR-138 |
| contact pass default | ll_level.c | on except RAN.01 | RAN is the balloon level |
| SEMI_SOLID | `play_collide` | only the r/5 shift | velocity add (kind 13) unit unknown |
| SOLID push polarity | `obj_shape_elem` | shape uses the DRAW polarity incl. CH_DIR facing | consistency, not decompile-pinned (PR-81) |
| player facing model | ll_play.c `face`, ll_level.c `play_draw/play_collide` | face follows walkdir only in CH_DIR states; draw/shape = authored XOR face, face sticks | flags real (LEW census, PR-82); the always-XOR rule for no-CH_DIR states is a model-consistency choice, NOT decompile- or video-pinned (HANDOFF Q0) |
| K_FWD/K_BKWD key model | ll_play.c `cond_true` (MODEL CHOICE note) | facing-relative: K_FWD = arrow along current face; walkdir = the literal arrow; a left-facing turn settles via the state pair or A_TURN+AC_BREAK | PR-85; forced by data (one A_WALK, no BKWD-walk state); NOT yet confirmed on video |
| lion attack damage / object hit rule | ll_level.c `LL_LION_HIT_DAMAGE` 30, `obj_hit`, `LL_OBJ_HIT_CD` 30, `ll_play.h` `atc`/`atc_hit` | damage 30 = the fixed `mov ebx,0x1e` of the stomp branch (0x34103, reached ONLY when the attacker's class id is ds:0xced04 - 0x340c8/0x340db; PR-139 read the whole receiver 0x34020, and its default at 0x340ce forwards the incoming damage UNCHANGED); hp<0 forces A_DEAD by name; energy -1 never hit-able; the COUNTDOWN IS 0x1e = 30, NOT 14 (PR-139): `[obj.vt+0xe4]` is 0x50300 in 14 of the 15 answer-A tables and 0x50300 is `edx ? [obj+0x102]=0x1e : [obj+0x102]=edx` (0x50300..0x50315); the lion's own class table 0x96960 ("TLewZaria", factory 0x1fef2) is one of them, so `pl->lock = 30` is exact, while PR-89's 14 is 0x27260 - reached by exactly one +0xe4 slot (table 0x9a54c); the RATE (one per record step, 0x50720) stays PR-135's pin; the window bit IS bit 5 of `[obj+0xb6]`, written only by the authored `atc_on` record through its applier 0x45c20 (parser 0x4636e -> vtable 0xa10f0+4), and the getter 0x23860 is the +0x40 slot of 155 sane class tables while 64 others return a constant 0; one hit per ARMED episode, `atc_hit` consumed only when a hit lands, and 0x550c0 cancels the object's hit exactly while the lion's own window is open AND he is turned toward it (equal x does not cancel) | addresses verified; the 30 stays until the factory-template id array (0x44350 -> ds:0xcecf0..0xced04; the proportional hit at 0x34037 is energy*30/100+10, NOT *45 - see PR-96) maps to a .des type; what the engine really passes is `[deliverer+0xb1]` (its only setter 0x1e540) and the .des `damage=` grammar into it is unpinned; the OBJECT-side countdown for ZWIERZ-family animals is a NAMED stand-in (their +0xd0/+0xe4 pairs are 0x1ecd0/0x4ffb0 for `SEP`, 0x41c30/0x41460 for the ZWIERZ base, and neither writes 0x1e); the window-clear rule and the corpse-deal-contact rule stay model choices (PR-89) |
| enemy attack (T3 st. 1) | ll_level.c `obj_arm_atak`, `play_collide` window block; `ll_play.h` `LL_CTRL_DANGER` | a current state carrying the authored DANGER flag arms the script's OWN A_ATAK by name when the lion is inside abs(dx) < 40 AND abs(dy) < 50 (strict); the enemy lands ONE hit through its own ATC window (authored damage=, 30-step lion lock) and NOTHING otherwise - the engine's DANGER damage is the level's global `danger_power` (`[ds:0xcef40+0x54]`, read at 0x55290/0x5529f in the animal answer 0x55230 and at 0x5102d in answer-A), which no shipped level authors, so the receiver's `damage == 0` gate (0x50eef) answers "nothing"; OFF unless LEO_PLAY_DANGER=1 | the box numbers are RE-pinned FOR THE CLASS "LICHA" (class chosen from the .des token at 0x616d0; box at 0x61b6e) - the flag-as-precondition, A_ATAK-by-name entry and window-hit mirror are the port's stand-in (engine driver open, §7 item 10) (PR-90). PR-139 names where the engine reads that authored flag: `[obj.vt+0x44]` = 0x23920 returns the CURRENT row record's flag word `[[obj+0x50]+[obj+0x5c]*4]+0x12`, and both answers test its 0x2000 bit (0x5528b / 0x51019); the same word's 0x80000 bit makes the animal call the hit with damage 0 (0x55245) |
| AC_ENEMY_ATC raiser | ll_level.c `obj_hit`, `play_collide` window block | the hit raises it on the HIT object's own set (PR-88 mechanism) so its `A_DOSTAL(1)=AC_ENEMY_ATC` line fires; PR-91 mirrors the raise onto the LION for the enemy's WINDOW hit (never contact) - his authored A_DOSTAL chain then runs itself | PINNED, no longer an inference: answer-C 0x55130 saves `[obj+0xad]` at 0x55140 and compares it at 0x5516a - hp DROPPED -> bit 16 (`or bl,0x1` 0x5518c), unchanged -> bit 7 (`or dl,0x80` 0x551ad), read verbatim again by PR-139 together with the delivery itself (0x5514f..0x55164 call the receiver's `[+0xd0]` with `[deliverer+0xb1]` as the damage); the answer entries merge the plain bit for any shape meet (PR-92/94). PR-139 corrects which family owns which answer: answer-A 0x50f80 is in 15 tables only, the animals' entry is answer-B 0x552e0 -> 0x55230 (78 tables), and BOTH reach the same answer-C |
| AC_ENEMY raiser (PR-92) | ll_level.c `obj_touch_enemy`, called from `play_collide` (solid loop + post-loop non-solid pass) | the lion's touch raises bit 7 on the TACHED object's own set and its OWN table reacts (raise-consume like the balloon path); gated by `hit_cd` (the [obj+0x102] countdown), default-on like contact | PINNED raise sites (PR-94 verbatim): answer-A 0x51050 and answer-B 0x52be merge {bit 7} unconditionally into a 13-byte answer packet (12-byte set + flag byte) copied to the answer buffer ARGUMENT (0x51062..0x51083); the answer handlers are reached through family vtables (PR-139: the entry slots are +0x78 = 0x50f80 in 15 tables and 0x552e0 in 78; 0x50ed0 is a RECEIVER at +0xd0, rets at 0x50f79 and raises nothing); which object's head finally reads the buffer is OPEN, so feeding the object's own set is MODEL, the second non-solid pass is data-forced (no consumer state carries SOLID); a pure touch NOT setting the hit_cd countdown is still an open item (item 10) |
| static image draw + order (PR-93) | ll_level.c `obj_static_get`, `objs_draw` (two-pass) | frame-one draw: first patch-art element of the named .SEQ at the authored x,y through the element's anchor; statics compose BEFORE animated objects | the join and positions are RE (census in the code comment); frame-one (no motion data exists) and the behind-actors order (matches the ZARIA footage) are model choices; whether the original animated any static is open (item 11) |
| shape payloads for lion-vs-object tests | ll_seq.c `ll_seq_shape_push2`, ll_level.c `play_collide` | each element's margin lists read from its OWN .SEQ payload (paired form) | model choice: how the original resolved two elements into its single-payload routine is unpinned; same-payload pairings stay byte-identical (selftest-pinned, PR-89) |

CORRECTION TO THE ROW-ABOVE CLAIM, added 2026-11-17 by PR-114 and
quoted here rather than edited in the table (the row still says "the
`fx != 0` half of the walk gate (30 shipped BY_X blocks with `F=0,0`,
no leg covers one)"): there is now a test in front of that half in both
engines, and the count is a tool output - `tools/re/mov_byx_gate.py`
reads rows the way `ll_mov_parse` does and measures 105 blocks naming
`BY_X` in 69 files, 30 of them `F=0,0` (290 rows, 21 files), 29 of
those 30 with no motion source of their own, and 97 states / 357 rows /
80 files where the engine's mode bit and the port's gate disagree
(62 of them by getting mode 0 by default, with no `control=` line in
the text at all). What the legs pin is the port's model, not the
engine's: `A_PUSH`/`A_COAST` in `ll_play_selftest` (velocity frozen at
the entry value, rows on the iteration clock, one 100-unit row per
feed, the sixth wrapping the list, `facc` exactly 0.0) and `glide`/
`snaps` in `obj_motion_selftest` (one row per call, `facc`/`xa`
zeroed, no anchor snap at the wrap because 1212 tests the mode bit -
the BY_TIME control does snap). Five mutations caught; the recorded
survivor is that `obj_motion`'s leg-3 "F=0 does not move" check still
passes when the object gate's `fx` term is deleted, which is the gap
PR-114's leg 6 closes. Which half the engine keys on stays OPEN.
NEXT_STEPS.md, PR-114 record.

CORRECTION TO THE ROW-CLOCK ROW ABOVE, added 2026-11-18 by PR-115 and quoted
here rather than edited in the table (the row still says "the engine admits ONE
row per record step, the port loops"): the engine half is right and the port
half is about ONE of the port's two loops. The compare at 0x52329 `39 d8` /
0x5232b `7d 27` admits at most one row per record step, and 0x52334 `29 c3`
hands `acc - thresh` to the row handler in registers only - the only writes to
[obj+0xa1] anywhere are the feeds at 0x543d1 / 0x543e7 / 0x54412 / 0x5443b, so
the accumulator is monotonic and the engine has no cap and no resync. The
port's player loop (ll_play.c:1286-1288) does cap a step at `4 * nfr` closures
and then zeroes what the cap leaves over (:1318-1319, "guard hit (data safety):
resync") - that resync DROPS a debt the engine keeps; the object loop has no
cap and no resync, only the FIX-13 4096-iteration guard (ll_level.c:1188-1189),
and it closed all 17 rows of a 1700-unit feed in one call in the new leg. And
the threshold table the row's OPEN list leans on needs its own correction: 170
rows at authored threshold 0 in 73 files is a count of the port's parse-time
clamp (ll_play.c:481 turns any authored value <= 0 into 100 before the scaling
at :735, so the port never stores a 0, and for those rows both engines close
exactly one row per fed step); 0 thresholds are negative anywhere. What
actually lets one feed close two rows is a threshold STORED under 100: 108
time-fed states in 77 files whose stored minimum is <= 50, and 1682 rows in
1..99 as written, where PR-110's record quotes "1715 ... after scaling (111
files)" from its own scaling rule. Two data-free legs now pin the port's
current behaviour in both engines without changing it (`MULTI`/`BOUND` in
`ll_play_selftest`, leg 7 of `obj_motion_selftest`), and six mutations of the
loop bounds, the resync and the parse-time clamp are all caught. Census and
arithmetic: `tools/re/mov_byx_gate.py`, NEXT_STEPS.md, PR-115 record. Whether
the game closes one row per step or all that fit is STILL open - that needs a
capture, not a compare.
CORRECTION, added 2026-11-18 by PR-135, for the last sentence above: that
question IS settled from the machine code, and the port now matches it.
PR-105 read the record-step driver (0x521f0 has ONE call site, 0x5240c,
inside the driver 0x523f0, which itself has one call site at 0x5371c): one
key composition + one feed + at most ONE row admit per step, and PR-133
confirmed no other scheduler exists. PR-135 REMOVED the port's multi-admit
loop, its `4 * nfr` cap and its `resync` on the player side, and the
multi-admit loop and the 4096-iteration guard on the object side, and
re-derived the `MULTI`/`BOUND` legs and `obj_motion` leg 7 for that shape
(one 1706 ms call is 34 iterations, 34 admits, each moved by its authored
`M` delta). What a capture would still decide is the SECOND half of the
same paragraph: whether a row fires AT `acc == thresh`, and what the
engine's own iteration length is inside a running game (PR-109's bracket,
28-33 ms, against the port's 50).
CORRECTION TO THE PR-115 PARAGRAPH ABOVE, added 2026-11-18 by PR-116, for
the same reason (that paragraph still says what it said when it shipped): it
quotes "0x52334 `29 c3` hands `acc - thresh` to the row handler in registers
only - the only writes to [obj+0xa1] anywhere are the feeds at 0x543d1 /
0x543e7 / 0x54412 / 0x5443b, so the accumulator is monotonic". Part of that
is wrong. The handler writes the remainder back: it saves its entry ebx at
0x520cb (`89 5c 24 14`), reloads it at 0x52114 (`8b 54 24 14`) and stores it
at 0x52118 (`89 90 a1 00 00 00`, `mov [eax+0xa1],edx`). And 0x54412 /
0x5443b are `add reg,eax` on a loaded copy, not stores: the mode-2 feed is
0x54400 load, 0x5440c `add ebx,0xa1`, 0x54412 add, 0x54414 `89 3b`
`mov [ebx],edi`, and mode 4's is 0x54429 load, 0x54435 `add ebx,0xa1`,
0x5443b add, 0x5443d `89 0b` `mov [ebx],ecx`. Those stores carry no
displacement at all, which is why a text search for `0xa1]` missed them.
Every instruction that does mention that displacement, listed in
`/tmp/opencode/pr116_write_set.txt`: WRITES 0x52118 (the remainder),
0x5236b / 0x533b0 / 0x5389e (the three zeroes PR-100 already listed),
0x543d1 / 0x543e7 (the mode-0 and mode-1 `add` feeds), plus the two
computed-pointer stores 0x54414 (ungated) and 0x5443d (gated on
[obj+0xbe] != 0 at 0x5441b); READS 0x52323, 0x54400, 0x54429. So the
accumulator is NOT monotonic - every row that runs rewrites it and three
paths zero it - and PR-115's "no cap and no resync in that path" is still
true but was not justified by that sentence.

THE OPEN HALF OF THE ROW-CLOCK ROW ("whether a row fires AT `acc == thresh`
(13932 of 14881 authored BY_TIME thresholds are exact multiples of the 100
feed)") is answered at the machine-code level, 2026-11-18, PR-116: it does
not. The compare at 0x52329 `39 d8` / 0x5232b `7d 27` is threshold minus
accumulator and jumps to the epilogue when the accumulator has only REACHED
the threshold, so a row runs only when the accumulator is strictly past it;
at equality the step keeps the accumulator, and the next feed runs the row
100 iterations later with a remainder of a whole feed instead of 0. The
reading that goes the other way needs the operands swapped and contradicts
itself, and PR-110's "which also makes the `sub ebx,eax` remainder negative"
belongs to that swapped reading, not to the bytes. Related wording in the row
above: "A row stays current while `acc < thresh`" should read `acc <= thresh`
for the engine. Measured population: 15317 of the 18617 row lines store a
threshold that is an exact positive multiple of the 100-unit feed (BY_TIME
15142 of 17011 rows, in 754 files), in 2012 states. `Wait`, the one
directive whose node skips the compare, is spelled `WAIT` in this text and
the name compare at 0x46b59 is 0x76a69, which lower-cases both sides, so
251 row lines do reach it and the honest figure is a bracket: 15066 to
15317. The port keeps `>=` (this PR changed no behaviour): flipping the
gates breaks 41 play legs and the two object legs that pin the current
rule, and no clip in the repo separates the two, so the flip stands as an
owner decision with a number on it. PR-116 record; census section 5 of
`tools/re/mov_byx_gate.py`.
PR-117 added 2026-11-18, and it closes a gap PR-116 named about its own
fixtures, quoted above: "nothing at all breaks if only the object's
walking half is flipped: no leg puts a distance-fed accumulator exactly
on a threshold, which is the fixture gap PR-116 records" That sentence is
no longer current. A leg 8 of `obj_motion_selftest` (ported/ll_level.c,
before the `rc = 0;` of that function) puts a distance-fed accumulator
exactly on a threshold: fixture
`[A_RUN] control=BY_X / F=100,0` with one 100-unit row, and `obj_walk`'s
feed is `|F| * dt_ms / 4000.0` (ll_level.c:1101-1104), so two dt=2000
calls give 50.0 and then exactly 100.0 - both products are exact in
binary64, so it is a real equality. The first call closes nothing, moves
x 20 -> 70 and leaves `row_t`/`row_rem` at 0, because a walking state
never runs `obj_row_tick` (:1169-1181); the second is the boundary, and
the port's rule closes that one row, lands `facc` on 0.0, wraps the row
list without an anchor snap (:1212 skips it for a walking state) and ends
at x 120. PR-116's mutation program (g2b, `<` -> `<=` at :1230) now fails
exactly that leg and nothing else, with a clean control run first; the
mutant leaves `facc` at 100.0 while x still reaches 120, because the walk
moves the object independently of this gate. No behaviour changed - not
one logic line of ported/ was touched. The play half of the same question
is still open and was measured, not assumed: a probe at the player's row
loop (ll_play.c:1286) printed 0 equalities over the whole play selftest,
and that feed is `p->vx * (dt_ms / 1000.0)` (:913, :1100, :1119), which
cannot hand an integer threshold exactly at the shipped 28 ms step.
PR-117 record in NEXT_STEPS.md.
PR-118 added 2026-11-18, and it qualifies the sentence above, quoted
verbatim: "Measured population: 15317 of the 18617 row lines store a
threshold that is an exact positive multiple of the 100-unit feed (BY_TIME
15142 of 17011 rows, in 754 files), in 2012 states." That count is right
and its arithmetic is unchanged - but "the 100-unit feed" is the feed of a
TIME-fed state only, and the sentence as written covers every mode. The
port distance-feeds a state when it carries the `BY_X` token AND a
non-zero first `F=` (the `walk` expression, ll_level.c:1169); measured
with that rule, the corpus has 75 such states, 992 rows, in 57 files, and
their stored thresholds are 2, 3, 4, 5, 6, 7, 9 or 10 units - so the
multiple-of-100 test selects NONE of them (0 of 992), and the 67 rows the
census's BY_X line counts are all time-fed ones. A distance-fed row does
have the same `cmp`/`jge` boundary, at a multiple of its OWN per-call
distance, which is `|F| * dt / 4000` (obj_walk, ll_level.c:1101-1104):
measured per grid this port really calls `obj_advance` with, it is reached
for 0 of 992 rows at dt=28 (`LL_ITER_MS`, the selftest grid: the feed is
`7|F|/1000` and the reachable thresholds are the multiples of
`7|F|/gcd(7|F|,1000)`, which never coincide with any (|F|, threshold)
pair this corpus authors), 713 of 992 at dt=20 (the play/PPM grid,
:6425: there |F|=100 feeds exactly 1.0, so every integer threshold is a
whole number of calls), 689 at dt=40, and 0 at the dt=2000 PR-117's leg
had to invent. So "flipping the two gates moves 15066-15317 authored rows
by one whole feed of 100 iterations" is the cost for the time-fed rows; a
complete figure would add a distance-fed population that depends on the
caller's dt, and PR-116's headline stands only as the time-fed count.
New in the same section 6: at 2 states the port's double accumulator
drifts OFF a landing its own feed law says is exact (`LONA.05_LEW`,
|F|=30 at dt=20 feeds 0.15, a 3-unit row needs 20 calls, and repeated
addition reaches 2.9999999999999991 at call 20 and crosses only on call
21), while the engine's accumulator is an integer dword (0x543d1 `add
[obj+0xa1], eax`) and cannot drift - untested anywhere, since no
selftest runs a distance-fed row at a grid that reaches a threshold. No
behaviour changed and no code changed: `tools/re/mov_byx_gate.py` section
6 only, output `/tmp/opencode/pr118_census.txt`, whose sections 1-5 are
byte-identical to `/tmp/opencode/pr116_census.txt`. PR-118 record in
NEXT_STEPS.md.
PR-119 added 2026-11-18, and it closes the last sentence of the paragraph
above, quoted verbatim: "untested anywhere, since no selftest runs a
distance-fed row at a grid that reaches a threshold." Leg 9 of
`obj_motion_selftest` is that fixture: `control=BY_X`, `F=30,0`, one row
`1,3`, then 20 calls of `obj_advance(&e, 20)` and a 21st - the same
(|F|, dt, threshold) the census found in `LONA.05_LEW`, on the real 20 ms
grid (:6425) rather than a 28 ms one. What it pins is what the port
already does: the row is held back at call 20 with `facc` at
2.9999999999999991 and closes on call 21, and the subtract at :1233
carries 0.14999999999999902 into the next row - checked as a band
(`2.99 < facc < 3.0`, then `0.14 < facc < 0.15`), so both an exact
accumulator and one that drops the remainder fail it. Two mutations catch
it and one control does not (transcript /tmp/opencode/pr119_mut.txt):
`o->facc = 0.0` at :1233 and quantising the feed to exact thousandths
each fail one check of leg 9 alone, while the g2b flip of :1230 still
fails only leg 8. Writing the leg exposed a second mechanism, now asserted
too: by call 20 the object has already moved the full three units
(`x` 20 -> 23) while the three-unit row has not run, because `obj_walk`
adds the same `dist` to `facc` and to `xa` (:1103-1104) and only `xa` has
its whole part taken away (:1105-1107), and that subtraction re-draws the
rounding at every pixel boundary - on call 20 the position residual hit
1.0000000000000002 and paid its third whole pixel (leaving
2.2204460492503131e-16) in the step where `facc` came out at
2.9999999999999991. Which of those two accumulators the engine keeps for
a walking state is unread, so this is recorded as PR-119 open item 1 and
the leg makes no engine claim. No behaviour changed: the leg and the
summary clause that names it are the only lines added under ported/.
PR-119 record in NEXT_STEPS.md.
PR-120 added 2026-11-18: a byte-level reading of the engine's own row
feed, no code change. It answers PR-119 open item 1 (whether the dword
the row gate's accumulator consumes is the pixel counter or a second
one): NEITHER. `[obj+0xa1]` is a field whose only consumer anywhere in
the object code is the row gate at 0x52323, the load behind the compare
at 0x52329; its writer set is the four feed sites PR-116 listed, the set
at 0x52118 and the three zeroes, and the mode-2/mode-3 branches write it
through a register offset (`0x5440c/0x54414`, 0x54435/0x5443d), which a
scan for the text `+0xa1]` misses. What the feed adds is `abs()` of
`[obj+0xca]` (`cdq`/`xor`/`sub` at 0x543cc..0x543cf), and that dword is
made by an `idiv` by 100 a few instructions earlier (0x5434d..0x54365,
y twin 0x5436b..0x54372) - NEW here is that the remainder is dropped AT
THE DIVIDE (0x5435f overwrites `edx`; the y remainder dies at the
epilogue), so the engine counts a sum of truncated steps, never a
truncated total. Two consequences recorded in the PR-120 record: the
engine-side set of reachable distance-fed thresholds is the multiples of
that per-step integer (so PR-118's 713/689 figures are the PORT's
boundary count, and its own law is quoted there), and the `F=` to v_x
relation stays the missing input for any engine-side count. PR-99 item 9
said the same quantization "is worth a test of its own": it has one now,
as the QUANT mutation (`o->facc += (double)(int)dist`, transcript
/tmp/opencode/pr120_quant.txt) - it breaks exactly one leg in either
chain, PR-119's leg 9, at its first check, because 0.15 a call truncates
to 0; leg 8 passes it because that fixture's step is a whole 50. So the
port's fractional feed, which the code comment at :1073-1077 calls a
deliberate difference, is pinned by a leg. Unchanged: PR-99 item 10's
"the position writer is still unread", now narrowed to the three unread
readers of `[obj+0xca]` at 0x54810/0x54830/0x54836. PR-120 record in
NEXT_STEPS.md.
PR-121 added 2026-11-18: the position writer, found at byte
level, no code change. It closes what PR-120 left open, in its own words,
quoted:
"now narrowed to the three unread readers of `[obj+0xca]` at
0x54810/0x54830/0x54836",
Those readers are one operation: the coordinate advance at
0x54804..0x5484a. It reads `[obj+0x0]`/`[obj+0x4]`, adds the
SAME truncated dwords PR-120 described (0x54827 `add edi,edx`, 0x54829
`add ebp,eax`), stores them back (0x5482b/0x5482d), then clears the delta
fields by self-subtract through a register offset (0x54830/0x54836/0x5483c
and the y twin at 0x5483e/0x54844/0x5484a) - which is the proof that no
sub-pixel remainder is carried: the engine quantises an object's POSITION
exactly as its row gate quantises the feed. The row gate takes `abs()` of
that dword; this writer takes it signed. So the port, which accumulates
the fraction in `xa` and pays whole pixels out of it (:1104-1106), moves
at the exact velocity while the engine moves at that velocity truncated
every tick: they can only agree at a velocity that is a whole multiple of
100 hundredths per tick, and turning that into pixels still needs the
`.MOV` `F=` to v_x relation PR-118 item 2 holds open. New with it:
`[obj+0xea]` is a one-shot y amount added into the y delta and zeroed in
the same breath (0x5480a/0x5481b), and the only computed store to it in
the image is 0x454a8, two instructions after that block links an object
into `[obj+0xe6]` (0x454a2) - the very field this function copies to
`[obj+0xa9]` at 0x547bc and calls through (0x547c2/0x547c6). Within that
tail the coordinate write is unconditional (four transfer targets); the
flag bytes and the linked-object handoff are the conditional parts. Not
settled: what the `[obj+0x0]`/`[obj+0x4]` pair IS. The method-table
alternative is ruled out for this class, whose table is `[obj+0x8]`
(0x547eb then 0x547fa calls slot +0xb8 through it), but offset 0 is a
table in the linked object's class and the note at NEXT_STEPS.md:6871
shows a ctor writing a pair at `[obj+0x0]` in another one. No behaviour
changed and no port comment touched; the coordinate half of the
difference is not modelled and not stated anywhere in the code. PR-121
record in NEXT_STEPS.md.
PR-122 added 2026-11-18: where the one-shot vertical amount comes from,
a reading, no code change. It takes PR-121's own closing words here,
quoted:
"the `[obj+0xea]` one-shot vertical amount and the `[obj+0xe6]` link as its
open items",
Those two fields are now read. 0x455a0 is 21 instructions of one
formula: `P.y + trunc((R.x - P.x) * (Q.y - P.y) / (Q.x - P.x)) - R.y`,
the signed vertical gap from a point R up to the line through P and Q,
and like PR-120's divide its remainder is dropped (0x455c3 `idiv`,
overwritten at 0x455c5). Its caller at 0x45400 builds P and Q as
adjacent records 13 bytes apart - 0x4543c/0x45440/0x45481/0x45485 are
four `imul ...,0xd`, and 0x4540c..0x45419 read a count byte at +0xc of
the same table - indexes them through slot +0x6c of the second argument's
table (0x45434/0x45439), and stores the result only past two guards
(0x45496..0x454a0: the gap must be <= 0 and <= what is already there),
0x454a2 `mov [edi+0xe6],esi` and 0x454a8 `mov [edi+0xea],eax` in the
same breath. So the link and the correction are one transaction: a
surface provider is attached, a non-positive gap to a two-point segment is
computed against it, and PR-121's tail applies that gap to the y delta
once and clears it. Not claimed: that the records are terrain. The shape
fits and PR-99 item 10 guessed terrain, but 0x45380 (0x45423) and the
+0x6c provider are unread, and there is no zero-denominator guard in
those 21 instructions. The port-side contrast is stated: it has no
geometric y path at all - y comes from the row's authored `dy=`
(:1238) and is reset to the base y (:831, :927, :1001, :1214) - so no
shipped leg could detect a missing slope push. PR-122 record in
NEXT_STEPS.md, with reading 0x45380 and the +0x6c provider as its first
open item.
PR-123 added 2026-11-18: the segment lookup that goes with PR-122's gap
function, and the port's own copy of the same query. It takes PR-122's
closing words here, quoted:
"with reading 0x45380 and the +0x6c provider as its first open item."
It answers the first half. 0x45380 is a 43-byte lookup over the table
PR-122 described: `eax` = the owner (its base pointer at +0x4, its record
count as a DWORD at +0x8, 13 bytes each), `edx` = a pointer to the x to
find. It returns 0 from three bail-outs - an empty table (0x45388), x
below `[base]` (0x45393/0x45395), x above the dword at `base + count*13`
(0x4539f/0x453a1) - and otherwise scans the records in order
(0x453a3..0x453bb, `cmp x,[base + k*13]`/`jg`) and returns the index of
the first record whose x is >= x. So the answer is always in 1..count-1
and index 0 is the not-found value, which is how both listed callers
treat it (0x4542c/0x4542e and 0x453db/0x453dd) and why 0x45400's pair
`i` and `i-1` is always two real records. The bounds only make sense if
the table is x-sorted, and they do not exclude two records sharing an x,
which would make 0x455a0's divide by zero: assumed ordering, unproven
either way. Sixteen bytes later 0x453d0 is a sibling client of the same
table - lookup, bail to `mov eax,0x7fffffff` at 0x453f5, else build
base+i*13 and base+(i-1)*13 (0x453df..0x453eb), `call 0x455a0` and hand
the gap back to its caller; the index labels that region as still inside
FUN_00045380, which is a hint. 21 sites in 0x40000..0x46000 scale an
index by 13 (first 0x45012), so this lookup is module-wide. The port
already asks the same question, in the open: `seg_y_at` (`ll_play.c:123`)
is 0x455a0's first term in C, its divide truncating towards zero the same
way and its vertical-segment case at :126 existing precisely because a
zero denominator is not allowed; `ll_plt_surface_at` (:140) and
`ll_plt_first_below` (:164) scan every polyline for a covering segment
and report not-found with `best = 0x7fffffff` - the sibling's sentinel.
The `.PLT` ground they read is documented as x right, y DOWN, feet on top
(`ll_play.h:54`) and parsed from the HD ETAPY block (`ll_play.h:10`).
Who gets the answer: the player, every step (`ll_player_step` :905, the
slope-following call at :1149, the fall check at :1173); a scene object
once, at placement (`obj_settle`, `ll_level.c:1693`, snap at :1700/
:1705), after which its y is only `dy=` (:1238) and base-y resets. So
what PR-122 wrote here - that the port "models no geometric y at all" -
holds only of the per-tick object path, and that correction is recorded
in PR-123 rather than in an edited record. Still unproven: that these
records are `.PLT` ground data, which class answers slot +0x6c with an
index, and what writes a 13-byte record. Shape agreed, identity not.
PR-123 record in NEXT_STEPS.md, with the +0x6c provider and the record
writer as its open items.









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
   PR-131 (ZARIA.01 audit): the y anchor re-confirmed at R3 spawn
   (ground band at 350-360 video px = camy 470-500 exactly as the
   port computes); the x lead/dead zone stays UNMEASURED and the
   video's lead is NOT constant (~50 units at the walk start, ~280
   mid-canopy - levels/ZARIA.01.mkv t=2-17), so CAM_LEAD_* is an
   even worse guess than flagged. Referee windows named in the
   PR-131 record, G-1/G-2.
   PR-134 measured those same windows as far as they go: t=3.0-6.0 is
   a VERIFIED camera lock (four independent checks incl. direct vision
   on sheet_lock_00.png) and the camera holds world x ~0 there - the
   anchor is pinned to +0..+42 units by the placed worm's own bounds
   box - while the lion ranges over 39-140 units, so screen 1 has NO
   tight lead. Past 6.5 s NO camera number is trustworthy: video_stills
   says still, the column-sum estimator says 83-145 u/s, the 2D
   matcher finds no match at all, and the self-animated ANIBACK
   background bands are the named cause - a landmark anchor (a named
   static .DES object read per frame) is the method that would decide
   it, so the lead stays UNMEASURED. Also measured: the A clip is
   edited, with cuts at 6.35-7.35 and 11.0-11.35 that no later probe
   may integrate camera drift across.
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
    - PR-125 RAN THE FIRST TWO VIDEO PROBES AND NEITHER COULD REFEREE,
      with three facts found on the way. (1) The referee objects chosen
      from the BY_X census (75 states with F!=0 among 2318, per
      /tmp/opencode/pr125_byx.py) - RYBKA R1-R6 A_RUN (BY_X, F=100
      T=20 in all six files) and PINGWIN A_RUN (BY_X, F=75 T=50) -
      ANIMATE BUT DO NOT TRANSLATE until the lion is adjacent: inside
      camera-locked windows (tools/re/video_stills.py: ENA.03
      20.5-50.0 s, LONA.05_b 480.0-504.5 s at <1 px/s drift)
      the fish bands and penguin bands score best lane shift 0
      (tools/re/video_lane_shift.py: 304 pairs sad 3.16 at 0 vs 5.07
      at 1 px; 277 pairs sad 9.38 vs 15.83), the schools materialize
      on screen as the lion approaches, and the penguins' only motion
      is an in-place wave; when the lion closes in, penguins flee at
      ~55-65 px/s (ESTIMATE, multiplicative error - whether that is
      A_RUN translation or the LONASUR surprise objects is untested).
      Both A_RUN control lines carry CHECK_LEW (RYBKA R1.MOV line 5,
      PINGWIN.MOV line 13), and RYBKA declares ONLY A_RUN and
      A_ESCAPE, so the sitting fish IS in its A_RUN state: the
      inference (named as such) is a per-object "lion near" gate on
      the feed side holding the BY_X velocity contribution at 0.
      (2) R3's camera holds dead still for 20-30 s and then JUMPS:
      whole-cut drift is bimodal (ENA.03 5-785 s: runs at 0, then
      jumps saturating the search at +/-96 px/s, p50 16 - a
      contaminated median, not a speed): world speeds are only
      measurable inside a still window. (3) ZOLW's
      A_RUN is BY_TIME and KADMAR.07_0_0.MOV is a lion variant, so
      neither is a candidate; the lion's own T!=40 A_WALK rows stay
      rejected (item 8: the player's path through 0x54250 is
      unverified). The referee PR-125 named (KADMAR.07's NINJA1-6
      A_RUN/A_START2) is WITHDRAWN by PR-126: no `.des` of any
      level places that class, and no `.LVM` script or other
      `.MOV` references it - complete data that nothing
      instantiates can never reach the screen. The join is now a
      tool (tools/re/des_census.py): of the 34 placed BY_X F!=0
      rows, 29 have T=40 and the whole remaining law surface is 5
      rows over 4 classes. The named referee becomes KADMAR.07's
      BOSS A_RUNA/A_RUNB (BY_X, F=75 T=20: A 18.8 vs B 37.5),
      ONE instance (`BOSS,xxx,boss.mov,1328,243` in ZWIERZ1.DES,
      energy=200) - the biggest gap left; backup LONA.05's YETI
      (F=100 T=50, bounds 2000-2350).
      The KADMAR.07 line is now CLOSED as a source. PR-128 swept the
      whole 1451.7 s cut at the widest detector setting - 93
      camera-still windows, 3.0-17.5 s long - and looked at a frame from
      the middle of every one. The file is also split by HUD presence
      with tools/re/video_blob_scan.py, and on this file that test is
      clean by a 34x margin (the WEAKER of the two fixed corner boxes
      never exceeds 126 non-black px in a picture frame, never falls
      below 4 337 in a gameplay one); joining it with the window list is
      exact: 15 windows sit inside the file's 15 no-HUD segments, the
      other 78 are gameplay. What PR-126 measured at 793.7-798.9 s (a
      boss standing and animating, zero translation) is one of those
      fifteen: the scene is a black room between two white-outlined
      wardrobe doors, HUD absent. No camera-still window with the HUD
      drawn holds a boss-sized blob, in either attempt, so this capture
      CANNOT decide the law - the referee is LONA.05's YETI or the code
      path. Full record: the PR-125, PR-126 and PR-128 sections of
      NEXT_STEPS.md.
   - PR-129 READ THE MOTION CODE END TO END and closed three open
     ends of this thread. (1) 0x54250 (velocity method) and 0x549f0
     (contact method) are the STANDARD pair at class-table slots
     +0xc8/+0xcc in 137 class method tables - the engine's default
     law, not one class's trick; 13 further tables point at the same
     pair from shifted slots, and only ONE table replaces it with a
     wrapper thunk (0x24700: adds 60/30 to the command block when
     [obj+0x0] > 1600 and [obj+0x5c] in {1,3,4,0x10}). The
     integrator, worked out: velocity is 100x fixed point at
     [obj+0xc2]/[obj+0xc6]; force step v +=
     (block+0x0*[obj+0xba])/20 + [obj+0x6d]*block+0x10 (y:
     block+0x4*[obj+0xbe]/20 + block+0x14); drag step v -=
     v_before*block+0x8/100 - the authored T IS the percent drag per
     tick (block+0xc for y) - and a drag that crosses zero zeros the
     axis; 0x54170 pulls a velocity toward the limits
     block+0x18*[obj+0x6d] / block+0x1c but never reduces one past
     them; the integer px/tick is v/100 at [obj+0xca]/[obj+0xce].
     Steady state px/tick = F/(2T) for [obj+0xba] = +/-10: PR-99's
     5*F*s/T in register units, CONFIRMED line by line; 10F/T px/s
     at a 20 Hz tick = law B, and F/4 px/s at T=40 = law A - so all
     29 T=40 rows agreed with BOTH laws for that reason and PR-79's
     /4 fit is now explained from code. (2) 0x549f0 IS the PR-125
     "lion near" gate: it seeds [obj+0xba]/[obj+0xbe] from the
     record's own default factors, keeps them while the object's
     contact list [obj+0xd2] is empty, and otherwise rewrites them
     to +/-10/0 from whether the contact sits past 5 px (kind 1;
     kind 4: 8 px, and it writes the y factor too), popping the
     contact through 0x551f0 when it comes inside. A record whose
     default is 0 - the reading the video forced - gives force
     exactly 0 and steady speed exactly 0: the sitting fish, and the
     sign of a flee comes from which side of the band was crossed.
     (3) THE MOTION PIPELINE: all 15 literals of 0x52790 sit at
     slot +0x34 of vtables constructors store into [obj+0x44], and
     [obj+0x44] is an INTERIOR POINTER INTO the object's own method
     table (measured [class]+0xe0 for two classes, whose class
     tables hold the same triplet at +0x110/+0x114; +0xdc for one
     other; the delta varies per class). 0x52750 (10 direct call
     sites) is the pipeline through that table: slot +0x2c = 0x52680
     builds a command block from the object's live fields
     [obj+0x48..0x5c] and its indexed record table, slot +0x30 =
     the provider adjusts it (0x4ff80 x10 writes force 60/60 and
     drag 7/7 behind a [[obj+0x0]+0x126] test; 0x2b4d0 x3, 0x61250
     x2), [[obj+0x0]+8]+0xc8 runs the integrator on the stack
     block, and slot +0x34 = 0x52790 is the tail: it merges three
     spatial candidate lists (0x4bd30/0x4b890/0x4b870) gated on the
     wire bits the port names (PR-60): [obj+0x13] bit 2 = 0x200 =
     CHECK_LEW, [obj+0x12] 0x40 = CHECK_ANI, 0x80 = CHECK_ZW. A
     second, blunter path: the dispatcher 0x50970 calls
     [class+0x114] when [class+0x104] returns non-zero AND
     [obj+0x9d] == 1 - the merger only for the +0xe0-delta classes,
     other table content for the shifted ones (OPEN). OPEN after
     this: naming the 15 classes (the constructor sites are the
     way); WHO WRITES [obj+0x48..0x5c] - ANSWERED by the next
     bullet (0x52590 IS the class's own vtable slot +0x00); the
     object tick rate - narrowed by that bullet to the (rate, s)
     pair; [obj+0x6d]/[obj+0x9d]/[obj+0x126]. Full record: the
     PR-129 section of NEXT_STEPS.md.
   - PR-130 CLOSED PR-129's open item (b) AT THE BYTE LEVEL and
     corrected its clock wording. 0x52680 (the block builder,
     vtable slot +0x2c) is a pure field copy: block+0x8/[+0xc] =
     [obj+0x58]/[obj+0x5c], block+0x0/[+0x4] =
     [obj+0x50]/[obj+0x54], block+0x10/[+0x14] =
     [obj+0x48]/[obj+0x4c] - exactly PR-99's `T=`/`F=`/`G=`
     layout; then it runs the state record's evaluator
     ([[obj+0x2e]+[obj+4]*4]'s [rec+0x4] slot +0x10, modes
     edx=0/1/2/3) against block+0x18/+0x1c, the values 0x54170
     soft-pulls toward - confirming PR-99's "the header F= feeds
     only the increment channel, 0x54170's target comes from the
     row F/I nodes". The six fields' lifecycle: 0x524a0 zeroes all
     six and then stores the [obj+0x44] vtable (14 direct sites -
     fresh objects start force 0 / drag 0 / const 0); 0x52500
     copies all six off a template; and 0x52590 - the F=/T=/G=
     parser PR-99 read (" %i,%i", T capped at 130 AT PARSE) - is
     slot +0x00 of every one of the 15 [obj+0x44] method blocks.
     So the authored F= pair IS the force word, T= IS the
     percent-drag word (130 cap at parse, >=20 runtime raise), G=
     the every-tick constant. THE CLOCK CORRECTION: PR-129 wrote
     that the published reproduces "assume 20 Hz (PR-99)" - they
     do not: the jump reproduction implies 25 ticks/s, the walk
     numbers fit 20 Hz only at T=40 with s=10, and the only READ
     clock is the iteration clock (PR-109 bracket 22-50 ms,
     PR-110's LL_ITER_MS=28). What is open is now one equation,
     px/s = (5*F*s/T)/100 x rate, two unread unknowns: no read
     link ties the pipeline to one call per iteration (0x52750's
     10 sites are per-class entries, e.g. 0x4ffb6, whose guards
     [[obj+0x0]+0x126] / [[obj+0x0]+0x50]+[[obj+0x0]+0x5c]*4+0x4
     / ds:0xcf074 are unread), and s = the record's OWN default
     factors [rec+0]/[rec+4] (seeded into [obj+0xba]/[obj+0xbe]
     by 0x549f0) are authored values never censused. Both closable
     from code+data without video: an iteration-driver read and an
     s census. Full record: the PR-130 section of NEXT_STEPS.md.
   - PR-132 CLOSED THE s SIDE, and its premise. The "record default
     factors" are not a record: 0x549f0's edx is a caller-built
     STACK scratch pair, and the pair's every producer is read -
     a pursuit scaling round(10.0*dx/(|dx|+|dy|)) with the double
     10.0 at 0x909ef/0x909f7 (0x27d60 = +0x14 of class 0x9a778,
     its twin 0x281ab the same shape), exactly 10*facing via
     getter +0x70 =
     0x239f0 ([obj+0x6d]) at 0x3448d/0x400ad, zeroing when
     [obj+0x106]==0 at the shared 0x50340 (12+ tables), and the
     +0.cc contact recomputes, which only ever write 0xa /
     0xfffffff6 / 0. A write census over the whole listing finds
     NO other writer of [obj+0xba]/[obj+0xbe] (13 sites, all in
     0x549f0; 0x54e00's pair of zeroes is a stop/reset helper).
     0x549f0 is the +0.cc handler at ~140 method-table entries;
     classes whose +0.cc is another function never seed the
     factors (force 0 - the PR-125 fish as a general rule). So
     |s| <= 10 always, s = +/-10 IS full one-axis intent, the
     worm fit stands with s read, and no authored field can
     carry s. One unknown left: the rate - 0x52750's entry
     guards and the per-iteration driver (PR-133; the site map
     is /tmp/opencode/pr131/slots.py). Full record: the PR-132
     section of NEXT_STEPS.md.
   - PR-133 CLOSED THE RATE SIDE, and the premise behind it. There
     is NO object tick separate from the frame loop. The byte
     census of every `FF modrm disp32` call form in .text is
     exhaustive for slots >= 0x90 (a disp8 cannot encode them) and
     shows the state-step chain: a class's +0x14 bodies tail
     0x550a0 -> 0x536f0 -> 0x53520, which dispatches [cls+0x90]
     at 0x5357d - once per iteration while on-camera (PR-106's
     cadence). The pipeline entries are STATE-RECORD members: the
     record tables are in the image (rec = [[obj+0x50]+[obj+0x5c]
     *4, indexed at 0x50bda), each re-binds the physics triplet
     0x55020/0x54250/0x549f0 at rec+0x1c..+0x24 and binds its
     own pipeline members from +0x2c; 10 of the 13 +0xe0 dispatch
     containers are found as record values, 3 of the 5 +0xe8
     sites sit inside a condition stub right after `call 0x3e060`
     (the stubs set the record condition bytes, e.g. 0x27e40 sets
     rec+07 |= 0x40 when the player is near). The 4 classes with
     +0x90 = 0x50970 run the physics-method sequence instead
     (+0x104/+0x114/+0x108/+0x110/+0x10c/+0x100 + a direct
     0x54450 call), the +0x114 leg gated on [obj+0x9d] == 1.
     So rate = iterations/s x passes-per-record-per-state-step,
     at most 1 pass for the moving families: the "20 Hz" IS the
     iteration clock at 50 ms, the top edge of PR-109's bracket
     (the 28 ms design point would put the worm at 44.6 px/s,
     which the video refutes; at 50 ms YETI 20 px/s and worm
     25 px/s both match). T1's scale test is now a two-factor
     fit: the iteration clock (G-1/G-2 windows, and a worm-speed
     window from the other side) and the per-record pass count.
     The 0x52750 entry map is complete with every table base
     anchor named (0x26fd0 = class 0x9a778 +0xec = the ZARIA
     worm). Full record: the PR-133 section of NEXT_STEPS.md.
   - PR-134 MEASURED THE CLOCK and picked the 50 ms edge of PR-109's
     bracket. Inside a camera lock verified four ways (video_stills, a
     column-sum estimator, a new named-pair 2D matcher shift2d.py, and
     direct vision on sheet_lock_00.png), the lion's ground run at a
     held key is 38-76 world units/s and the ZARIA worm's rate is
     <= 30.3 units/s tracked by its head eye at 60 fps (a head
     landmark, not a body centroid - PR-79's failure mode). Dividing
     by the PR-129/130 law constant (2.5 u/iteration for A_WALK
     F=100 T=20, 1.25 for the worm) gives 15-24 iterations/s from
     both families, 20/s inside both and 35.7/s - the 28 ms design
     point - outside both, so the engine's iteration clock is 50 ms
     with one pass per state step. G-1(b) is answered from the data:
     LEWPL has NO A_RUN state at all - the fast gait IS A_WALK, whose
     seq=walk cycle is 12 poses each gated by row field 2 = 6 units
     travelled, and whose airborne poses are the gallop art. Also
     found: the missing rhino referee line
     (`NOSOROG,dummy,nos.mov,1700,600`, `bounds=1680,1950` - a
     45-vs-80 u/s discriminator, never on camera in a sampled window)
     and that the A clip is EDITED (cuts at 6.35-7.35 and 11.0-11.35,
     so camera drift must never be integrated across them). For G-2:
     in that lock the camera holds world x ~0 while the lion ranges
     39-140 units, so the first screen has no tight lead; no lead
     constant is quoted, because past 6.5 s the three estimators
     disagree (the self-animated ANIBACK parallax bands fool them) and
     a landmark anchor is needed. Full record: the PR-134 section of
     NEXT_STEPS.md.
   - PR-135 APPLIED THAT LAW ON THE PORT SIDE, both actors. The player
     and every scene object now run the engine's own record step: one
     key composition, one feed, AT MOST ONE row admit per iteration of
     LL_ITER_MS = 50 (0x521f0 / 0x5232b / 0x523f0), the velocity
     register stepped once per iteration (`v -= clamp(T,.,130)*v/100`
     with the engine's sign-flip-to-zero and its one-unit register
     floor, then `v += F*s/100` at the key intent s = ±10), and ONE
     row accumulator fed by TRAVELLED DISTANCE in a BY_X state or by
     0x4c8b7's constant 100 otherwise; the object's [obj+0x102] hurt
     cooldown counts iterations, not ms. Every play and scene position
     pin in verify-play / verify-level is now that arithmetic (the
     closed forms are in the Makefile's PR-135 re-pin note and in the
     leg comments). What this PR deliberately did NOT model, counted
     here: the vertical constants are NOT re-fitted, so the apex the
     authored jump reaches is now 20 px against the ~24 px PR-73/86
     fitted against (within 4% of the 21 px single jump PR-74
     measured) - a REGRESSION against what the old 20 ms grid
     produced, which the zjump leg comment states instead of hiding;
     the engine's conditional raise of T to >= 20; the partial-intent
     values of `s` between 0 and ±10; the object's own velocity
     register (objects step straight to the steady rate, because every
     measured object speed is a mid-walk number); whether a state
     change consumes its step's feed (the object side skips the new
     state's rows, the player side feeds them); the force/gate-fed row
     lists inside A_JUMP and A_DOSTAL, whose frame pacing is this
     run's and not a derivation; the velocity carried across a state
     change; PR-83's pending-AC_TIME lifetime (a key release at
     iteration 10 reaches A_DRAP, at 40 it does not); the object-side
     `nfr == 0` guard, which admits nothing at all; and the
     one-point-per-iteration terrain test against the engine's
     shape-box geometry (FUN_00042a70 / FUN_00042de0) - that is why
     RAN.01's first ungrounded x moved from 314 walking BACK to 303
     walking RIGHT; the one-register-unit floor of the velocity register
     (LL_PLAY_V_EPS = 0.2 u/s, a MODEL CHOICE in port units); the ground
     snap's new `vy >= 0` guard, which NO leg in this repo distinguishes
     (PR-135's mutation run proved that, and says why - an upward `I`
     clears `grounded` as it is applied, so `grounded && vy < 0` is not a
     state the model can reach today); and the fact that the 50 ms
     iteration is PR-134's video-chosen edge of PR-109's 28-33 ms code
     bracket, not a read of the engine's own loop. Full record: the
     PR-135 section of NEXT_STEPS.md.
    - PR-136 MEASURED G-2 IN ABSOLUTE WORLD UNITS. The level's OWN static
      .SEQ placements are camera anchors: tools/re/level_landmarks.py cuts
      a template for each from the port's own renders (anchored to its
      authored world point), tools/re/video_camera_read.py reads one
      camera (x, y) per frame by matching them (each match only PROPOSES
      a camera; the proposal is judged by what every other visible
      landmark scores at that exact offset; ties break toward the previous
      frame and are flagged ambiguous), and tools/re/camera_law.py turns
      the read into law numbers. Validated twice against cameras that were
      SET: 14 of 16 port-rendered frames read with error (0,0), the other
      two have no anchor in view and say so; 12/12 frames with coverage
      read exact through the capture path. The level's own geometry (the
      camera cannot leave 0..world-view) is what filters the false
      matches: the level reuses its static art for animated objects and
      its backdrop. ZARIA.01's 460 s clip: 21205 frames give a camera;
      1070 reads are off the level's camera range by >8 units and are
      dropped. The lead the game HELD frame-for-frame: 95-108 walking
      right (57 of 76 held-window frames; the other 19 are mostly at the
      world's left edge where no lead can hold) - CAM_LEAD_R=100
      CONFIRMED; 199-251 left (n=19) - CAM_LEAD_L=220 sits inside a weak
      band. Catch-up rate p50 5 / p95 60 / max 230 u/s, slower than the
      port's 120 u/s except in the tail. The grounded vertical anchor
      (the paws row, the feature CAM_FEET_Y is defined on) is 128 in
      mode, never a rigid 178: row 180 occurs only where camy is at the
      world clamp, and one constant 128 plus that existing clamp
      reproduces both. The view does NOT hold rigidly through jump arcs
      (camy change p05 -26 / p50 0 / p95 +12). AND G-13, a NEW gap this
      found: the first screen's walking surfaces sit 7-51 units ABOVE the
      ZARIA_A.PLT lines the port stands the lion on, while at the ground
      line the read matches the polyline "-50,680!408,684" in 108 of 108
      frames - the placed platform images (plan4.seq@70,480,
      plan6.seq@287,430) have their top edges at exactly those heights.
      Coverage holes at camera_x 900-1100 and 1400-1500 are the level
      offering no static anchor there, not the clip never going there.
      None of the measured numbers is applied to the code yet - that is
      the follow-up PR. Ten open items are counted in the record. Full
      record: the PR-136 section of NEXT_STEPS.md.
    - PR-137 APPLIED WHAT PR-136 MEASURED, only where the measurement
      was clear: `CAM_FEET_Y` 178 -> 128. The port's own comment had
      argued the 175..180 band "is NOT the world clamp" - at 21k frames
      it demonstrably is (ground 681, cam_ymax 500 -> row 181), and the
      anchor under it is 128; one constant plus the existing clamp now
      reproduces BOTH observed row populations, which is why not one
      existing pixel pin moved (the ground-line case clamps either way)
      while every previously unclamped surface now shows the lion at row
      128 as the clip does. The leads stay 100/220 - CAM_LEAD_R is
      CONFIRMED by held-frame leads 95..108, CAM_LEAD_L sits inside a
      19-frame band - and the eases (6 x, 2 y per iteration) stay named
      models with the measured band (p50 0.25 / p95 1.2 / max 11.5
      u/iteration) written beside them: replacing one unmeasured number
      with another would be less honest than testing the model. NEW
      HERMETIC LEG `./ll_level --follow-selftest` (verify-play):
      follow_camx/follow_camy had NO test before this PR. 9 mutation
      controls, all caught - and the honest note: the x-ease mutant
      (6->7) SURVIVED the first run until the turn case gained a
      mid-flight assertion at call 10; that near-miss is in the record.
      Six open items counted (eases still invented; CAM_LEAD_L pinned by
      test not evidence; the airborne hold is a model the data doubts;
      G-13 still stands - this fixed the ANCHOR, not the GROUND). Full
      record: the PR-137 section of NEXT_STEPS.md.
    - PR-138 CLOSED G-13 AS A READING RECORD AND RETRACTED ITS HEADLINE
      NUMBER. Engine side (transcript /tmp/opencode/pr138/engine_read.txt
      - the leodb image index now rebuilds in one command): the
      platformy= loader (0x4dc00) makes ONE OBJECT PER POLYLINE LINE of
      the .plt text and the line PARSES ITSELF (method +0x20, 0x45120:
      leading `%d` = extra-vertex count -> [obj+8], then (cnt+1)
      13-byte records at [obj+4]: `%d,%d` -> rec x,y, and a ` %c` peek
      sets rec+0xc (';' -> 1, '#' -> 2)) - that closes PR-123's
      "records do not match .PLT text": the segment lookup 0x45380
      indexes exactly that array. Class 0xa0c78 is referenced ONLY by
      that loader: no second ground-record builder exists in the
      image. Entity twin 0x34850 holds its own 13-byte table at
      [obj+0x48]/+0x4c and asks a global provider (level +0x394)
      built in the subsystem chain that names MOVEPLAT/MOVERECT
      (0x91f42) - who fills that table is the open hole, and it is
      the path by which the ANIPL objects' LOCAL ground lines
      (ANIPL2.DES: KATAPULT@300,588 "1 7,45;40,45 -6300,-1350",
      PALMA tops at world 475/380/320) become queryable relative to
      the object (the 0x45320 base adds [this+0xca]/[this+0xce]).
      Data side: the full 60 fps re-read with the reader's own
      confidence tiers found 951 unambiguous tier-1 (two distinct
      landmarks agreeing) lion frames, ALL at camera x 1..122 - the
      first screen only. On them the ground match is now d+0 EXACTLY
      (125 samples, y 681 vs "-50,680!408,684"), and the "7-51
      above" populations are TIER-2 ONLY: all 19 clusters sit in one
      world-x band 133..391 - the signature of screen-x wearing
      world-x clothes after a false single-landmark match. Frame
      4370 proves the false matches: the reader said cam (173,320),
      but ANI2.DES's balloon at (1270,460) measures view-centre
      (177,118) in the frame - on view only for camx ~1105 - and
      the port's own render at (1079,335) matches the frame; with
      THAT camera the paws land on authored line "1 1171,506;1305,503"
      within +-20. The one other tier-1 "surface" (y 580, frames
      466..475) is a JUMP APEX my constant-y filter accepted -
      settled by viewing frames 460/466/472. Net: the clip asks for
      no ground source beyond the authored .PLT where it can be
      verified, and mid-level camera is UNVERIFIABLE by landmarks
      (no two statics co-visible beyond x ~122) - every mid-level
      claim must now say tier-2. NEW PORT GAP, named: the port
      places ani_plat= objects but never parses their local ground
      lines (ll_level.c loads ground from platformy= only). Tool
      promoted: tools/re/ground_surfaces.py (world-coord,
      tier-filtered clusters vs the .PLT and the atlas). 8 open
      items counted, including the jump-apex LIMIT of the filter and
      the fact that every artefact was REGENERATED after /tmp was
      wiped mid-session. Full record: the PR-138 section of
      NEXT_STEPS.md.
    - PR-139 CLOSED G-5, THE CONTACT-DAMAGE GAP, AND CHANGED THE
      BEHAVIOUR. The port had run since PR-89 on the rule that a lion/
      object shape contact deals damage; the engine has no such rule.
      What an authored `damage=` goes through is the OBJECT's own attack
      window: the animal answer entry `[obj.vt+0x78]` is 0x552e0 (78
      tables) -> 0x55230, whose body is exactly three branches - a
      damage-0 hit when the current row record's flag word carries
      0x80000, answer-C 0x55130 only while the window bit is set, and
      the level's global `danger_power` (`[ds:0xcef40+0x54]`) when that
      word carries 0x2000. Answer-A 0x50f80 (15 tables only, not
      "every family" as PR-96 shorthand had it) is the same two laws at
      0x50ff1/0x51011. The window bit is bit 5 of `[obj+0xb6]`, and the
      only writer found is the authored `atc_on` row record's applier
      0x45c20 - which closes PR-90's open item about the missing
      consumer of `atc_on`. Answer-C's hit call passes
      `[deliverer+0xb1]` as the damage behind the 0x550c0 clash rule
      (suppressed exactly while the LION's own window is open and he is
      turned toward the object; equal x is not suppression), and the
      receiver 0x50ed0 then gates on `[+0x11a]`, damage == 0,
      `[+0x126]`, the window-pairing rule and its countdown before
      0x54550 subtracts `[obj+0xad]`. The countdown arm is `[+0xe4]` =
      0x50300 writing 0x1e = 30, and the LION's own class table 0x96960
      ("TLewZaria", matched by the factory 0x1fef2) carries exactly
      that pair - so PR-89's 14 is corrected to 30 and the 14 is
      identified as 0x27260, reached by one +0xe4 slot (0x9a54c). The
      port's unconditional block is deleted, `clash_suppressed()` is
      added, `atc_hit` is consumed only when a hit lands, the two play
      legs are re-pinned (a bump now asserts NO damage at any tick), and
      a data-free `contact_model_selftest` covers the truth table.
      13 open items counted - the biggest being that an ANIMAL's own
      receiver/arm pair (0x1ecd0/0x4ffb0, 0x41c30/0x41180/0x41460) is
      unread, so the object-side hp/countdown law stays a named
      stand-in, and that the `.des damage=` grammar feeding `[obj+0xb1]`
      is unpinned. Full record: the PR-139 section of NEXT_STEPS.md.

    - PR-140 RAN AND PINNED G-9'S TRAVERSE PROBE and corrected the
      PR-131 wording. No behaviour change (no C touched): a new
      data-backed `verify-play` leg runs ZARIA.01 for 12 s (300
      iterations) with `LEO_PLAY_SOLID=1 LEO_PLAY_SCRIPT="1:FWD"` and
      pins: the DRABINKA placement at its authored (180,510) with its
      3 states, the tick-50 pinned SEMI push-out line, >=30
      `solid LICHA ... SEMI` lines, the positions tick 91 = 180,
      tick 200 = 435, tick 300 = 685 (A_WALK, grounded), and NO
      damage or reaction-state line anywhere. Measured: the worms
      answer the contact with the push-out ALONE across ticks 37..135
      (70 lines) and that costs DELAY, not a stop (0.8 units/tick
      pressed vs 2.5 unpressed, ~40 lost iterations). MUTATION
      CONTROLS: C1 = the pre-PR-139 source reaches the SAME positions
      while taking three 15-damage hits (ticks 37/67/109, energy
      100 -> 55) - so the leg detects the old model ONLY through its
      damage negatives, and the audit's "dies before x=180" wording
      is retracted as a positional claim; C2 (no window required) and
      C3 (SEMI push never moves the lion) are both detected; base
      green before and after. THE LADDER CHAIN WAS READ end to end
      except the answer bits: ANI2.DES places DRABINKA at 180,510
      (with local ground lines `1 33,-3` / `23,30;53,160` / `30,50` -
      G-13's remainder, instance named) and LINA at 125,292 and
      525,525; drab/lina .MOV scripts idle at `1,0,WAIT K_GEN1` then
      unroll downward (rows 500/400/300/200) and park at `WAIT
      K_NEVER`; LEW.ZAR enters A_WSP/A_WSP_LIN via
      `A_TURN_BACK(1)=K_UP,AC_DRAB_CATCH` then
      `A_WSP(1)=AC_BREAK_NEXT,AC_DRAB` (air entries at row 3 =
      `AC_DRAB_CATCH,AC_ONAIR`), and `A_FALL_DOWN(1)=!AC_DRAB` is
      letting go. 8 new open items counted - above all who raises
      AC_DRAB/AC_LINA and what fires K_GEN1 (the objects' answer
      side, PR-141), the K_UP key (the port has no up/down key), and
      the BY_TY `F=0,100 T=160,50` + SEQ_REV_Y feed law of A_WSP
      (still PR-109's open law). Gates: make/make win zero warnings,
      verify-play rc=0, verify-all 56 `(ok)` = PR-139's 55 + this
      leg, fixture-free the same 23 named SKIPs. Full record: the
      PR-140 section of NEXT_STEPS.md.

    - PR-141 READ THE LADDER'S OWN ANSWER SIDE (a reading record, no
      code change - PR-138 precedent). The condition vocabulary is
      NOT new - PR-88 read that same parser and the port's LL_SIG_*
      matches it (0..68, keys in one bit space); this re-read
      confirmed it end to end and added only the parser's node-op
      constants (K_ANY zeroes the record set then ORs 0x2001f8 into
      [rec+4]; K_NEVER ORs 0x60 at 0x4806f). What is new: the
      DRABINKA factory branch 0x1f8ac (literal 0x906ff) builds class
      table 0x97cdc - +0x40 is the CONSTANT-ZERO window getter, so
      a ladder can never have an ATC window, and the constructor
      defaults a 1x1 RECT at [obj+0xb1..0xbd] - and its answer
      entry 0x3f1c0 is the raise site PR-140 hunted: gated on the
      ladder's own current row ([obj+0x5c] = the K_GEN1-admitted
      unroll rows 1/2), it probes that rectangle via the arithmetic
      helper 0x49fb0 with n=70, asks the toucher [vt+0x8], and
      writes into the toucher's 13 bytes (flag at +0xc): bit 8
      AC_EDGE when it does not fit, bit 10 AC_DRAB alone (branch B,
      0x3f355), or AC_DRAB | AC_DRAB_CATCH (bits 10|12) merged
      through 0x49580 and copied by a repnz-movs that STOPS AT THE
      FIRST ZERO WORD. PR-140 open item 2 is CLOSED for DRAB.
      Plain `LINA` has NO class-name token (0x910ca unreferenced) -
      the "climbable variant" family is K-prefixed (KLINA, KDRABINA
      ... at 0x910a7..0x910fb, matching AC_DRABK=11 and ZARIA.02's
      DRABK.MOV); which class a plain LINA gets is open. [ANSWERED
      by PR-149: the search checked ONE literal, not the file - a
      second `LINA` at 0x92005 IS a factory branch (0x4e81d, the
      same token-compare idiom, a 0xd9 ctor with the same rect/
      block setup as DRABINKA's); which of the two literals is
      the .des token stays counted.] 7 new open
      items counted, the first two being the engine's oldest open
      mechanism - the transition EVALUATOR and what the 13-byte
      key/set actually is (PR-88 labelled [obj+60] a TEventSet,
      PR-101 read it as the object's identity key: PR-142 picks up
      PR-101's chain 0x536f0/0x523f0/0x47e40/0x52251) - then
      K_GEN1's raiser, LINA's class path, the [obj+0x66] bit-6 /
      probe units, the ladder's receiver pair 0x3e5e0/0x53d80, and
      the answer-chain slots [vt+0x3c/0x4c/0x68/0x8]. [PR-149
      PINS those slots and that pair: 0x23c10 / 0x23a20 (add
      eax,0x75) / 0x23b60 (ret-eax; the lion's +0x68 is 0x23900) /
      0x23e00 across the 24 tables, and 0x3e5e0 is an INIT-AND-
      PRINT entry that raises NOTHING. No mutants:
      no behaviour changed; the honesty mechanism here is
      attribution (PR-88's prior claim is named, not hidden). Full
      record: the PR-141 section of NEXT_STEPS.md (cond_map.txt
      carries the re-derived table with every parser block address).

    - PR-142 READ THE EVALUATOR ITSELF - the repo's oldest open
      mechanism - and CLOSED it (reading record, no code change:
      PR-138/141 precedent). Answer to the PR-88-vs-PR-101
      question: [obj+0x60] IS the live signal set (13 bytes), not
      an identity key - the object family's "identity" providers
      are plain `ret` stubs (0x23930/0x23940) and its
      standing-bits getter 0x23a60 returns a zeroed set; signals
      enter ONLY through the class +0x10 event entry 0x53770 (a
      record of type 0x10, magic 0x8138764, whose byte +0xc is a
      bit index from PR-88's table, ORed into [obj+0x60] via
      0x49580; 23 class tables carry this entry, DRABINKA's
      included). 0x47e40 is NOT a lookup: it dispatches on the
      caller key's flag byte (jump table 0x47e30): flag 0 ->
      return 0, flag 1 -> (record's own set AND caller key) any-
      bit - the ONLY non-trivial mode and the authored-condition
      test - flag 2 -> 12-byte equality, flag 3 -> negated
      equality; flag > 3 asserts. The flag byte is authored
      grammar (0x47db2..0x47dd0): ordinary conditions default to
      flag 1, '!'/'*' -> 2, '|' -> 3; K_NEVER = an EMPTY set +
      word-3 bits 96/97 (an empty set under flag 1 IS never),
      K_ANY = bits 3,4,7,8,9,10,21 + marker 0x2001f8 (honour
      open). THE MATCH IS AN ORDERED SCAN: each row record keeps
      transition records at [rec+0x1a]/[rec+0x22], stride 21 =
      13-byte authored condition + target state [rec+0xd] +
      target row [rec+0x11]; 0x52000 tries each via 0x47e40
      against the caller key := authored OR live (explicit merge
      at 0x52279..0x522bc) and takes the FIRST match - the OR-
      of-ANDs IS the scan, the AND IS the node mask. 0x52033 and
      0x522e6 are the only two 0x47e40 calls in the image: the
      scan and the record-step path (0x521f0: wrapper [ebp+0x0]=
      entity / +0x4=index / +0x1c=authored key / +0x44=record
      array, entity back-link +0x2e, latch [entity+0x95], and a
      second authored threshold [rec+0x10] tested against
      [obj+0xa1]+[0xcef40]+0x388 through the class +0xa0/+0xa4
      pair 0x239d0/0x239c0). THE HEADLINE IS A CONFIRMATION: the
      port's shipped condition model - comma-AND against o->sig,
      per-name wiring, GEN raising name bits (PR-88's selftest) -
      is the engine's law, quoted from the code; no port behaviour
      was touched. 8 new open items counted (PR-141's 1/2/7
      closed or narrowed): the K_ANY marker honour, the '!'
      complement representation, whether the ladder answer packet
      reaches the lion's live set as a signal record, who POSTS
      signal records, the per-frame tag contribution ([obj+0x89]),
      the [entity+0x95] latch consumer, the [rec+0x10] threshold
      units vs PR-134/135's row feeding, and class +0x10 entries
      that are not 0x53770 (the LION not yet checked). No
      mutants: a confirmation claim is the one thing a mutation
      cannot test - it is instead quoted against both shipped
      consumers. Full record: the PR-142 section of NEXT_STEPS.md
      (mech_map.txt carries the whole address map).

    - PR-143 READ THE SIGNAL DELIVERY MACHINERY (reading record,
      no code change: PR-138/141/142 precedent). Signals travel as
      records: the GLOBAL BROADCASTER 0x4bc40 (world object
      ds:0xcef40 walks its object lists; ebx = the signal bit
      index from PR-88's table; ecx = a class magic 0x8138764 or
      0, filtered via [obj.vt+0xc]()) - ALL 22 call sites resolved
      and censused: only bits 7 AC_ENEMY x2, 16 AC_ENEMY_ATC x4,
      18 AC_ONAIR x5, 19 AC_DRECT x3, 25 AC_UP_DUP x7, 26
      AC_SPOKUJ x1 - NO key bits and NO AC_DRAB/AC_DRAB_CATCH are
      ever broadcast (a first window pass misgrouped 0x1f468 as
      bit 12; the resolved census corrects it, stated). Per-object
      delivery: 0x52b10 appends a 16-byte payload node to the
      list rooted at [obj+0x10]; the drain 0x52b52 pops each node
      and calls the CLASS +0x10 ENTRY (0x53770 for this family)
      per node - queue field and receive slot are a pair; the
      payload TYPE numbers share the condition index space
      (0x52bb0's arms 0xe/0xf/0x21: the type-33/AC_KILO arm is a
      MOVE record adding deltas to x/y). THE EVALUATION TRIGGER IS
      PINNED: 0x53520 copies [obj+0x60] VERBATIM as the scan's
      caller key and has exactly ONE caller - inside PR-101's
      compositor 0x536f0 right after the live-set recompose - so
      signal raise + transition test are ONE pass (PR-142 open
      item 3 CLOSED: the key is the live set itself, no merge);
      0x535a0 IS THE ROW-SWITCH EXECUTOR (fires only when target
      != [obj+0x5c]: old-record exit [vt+0x20], [obj+0x9d]:=0,
      [obj+0x95]:=1 - PR-142's latch writer, narrowed -,
      [obj.vt+0xa4] read, new-record entry [vt+0x1c], store
      [obj+0x5c]) - the engine's exit/entry/store law. THE
      ANSWER-CHAIN CALLER IS FOUND: the collision handler 0x50f80
      calls 0x53ae0 (PR-139's hunted caller), whose entry gate is
      the object's row flags `test ah,0x4` = 0x4000 = the port's
      LL_CTRL_CHECK_ANI, skipped when the toucher carries 0x10000
      (ANTI_SOLID analogue the port already reads); the push arm
      re-derives PR-133's ported law verbatim (0x40400 gate,
      kinds 5/13, idiv 5). PR-140 open item 8 is CLOSED for the
      law half: an object answers a contact only from rows
      flagged 0x4000, and ZARIA.01's drab/lina rows carry none -
      PR-140's untouched-column run was consistent. HONEST
      NEGATIVES: the answer/contact result bits provably flow only
      to the COLLISION CALLER's buffer (0x51062..0x5107a), never
      into a live set - and AC_DRAB/AC_DRAB_CATCH are raised by
      ~40 direct 0x49580 mask merges and state-gated posts whose
      LION instance is not yet identified (open item 1: the
      instance hunt, starting from the LION's own class table and
      the collision output's consumers). 5 new open items counted
      (drain caller; key-bit posting path - the [vt+0xc](0x13)
      lead at 0x34a35 -; the LION +0x10 entry; 0x52bb0's other
      arms; the AC_DRAB instance), and PR-142's items 3/6 plus
      PR-140's item 8 close or narrow. No mutants (no behaviour
      changed); the census is enumerated, not sampled, and the
      misgrouping correction is recorded rather than hidden. Full
      record: the PR-143 section of NEXT_STEPS.md (sig_map.txt
      carries the delivery map + full censuses).

    - PR-144 READ THE LION'S OWN OBJECT SIDE (reading record, no
      code change: PR-138..143 precedent). THE PLAYER'S CLASS
      TABLES: token 'LewZaria' 0x9078b (a length-prefixed token,
      byte 0x84 = 8 chars), factory branch 0x1fef2 -> alloc
      0x12e, ctor 0x1e0a0, [obj+0x8]=0x96960, [obj+0xc]=
      0x96958 (the same +8-alias table pair as DRABINKA);
      sibling per-game player tokens LewZariaB/LewEna/LewEna2/
      LewKadmar1/2/LewSades/LewRan (0x90795..0x90fe7). Table
      0x96960: +0x10 = 0x1e130 (PR-143 item 4 CLOSED - the
      lion's OWN receive front door: arms for type 5 and 0xc,
      generic tail 0x50d60 which itself holds live-set merges
      0x50ab7/0x50e24; the type-0x10+magic bit-raise lives in
      the shared 0x53770 pattern the lion does NOT use), +0x78 =
      0x50f80 = PR-142's contact handler (the same +0x78 slot
      the ladder answers through with 0x3f1c0; 0x50f80 appears
      at +0x78 in 15 class tables). NEW MECHANISM, not modelled
      by the port and not needed: the player REWRITES its class-
      table pair at runtime (0x1ec36..0x1ec3f: [obj+0x8]=
      0x978dc/[obj+0xc]=0x978d4, in the health/death branch that
      also broadcasts AC_ENEMY). THE CONTACT-PAIR PROBE: 4 sites
      invoke +0x78 in the whole image (0x4b5be, 0x4bda9,
      0x4be09, 0x55421); 0x4bda9 belongs to 0x4bd32 - it walks
      the world chain, per object gates on [vt+0x60]() and a
      [obj+0x24] layer filter, skips self, then calls
      [other.vt+0x78](other, self, &result): ONE pass gives
      BOTH members of a contact pair their turn, aggregates
      their 13-byte outputs into one union set, ANY-BIT tests
      it (0x495b0) and copies the union to its caller. PR-142/
      143's "collision caller's buffer" IS this union. THE
      LIVE-SET WRITE CENSUS: of the 69 0x49580 call sites,
      exactly 14 OR into a <obj>+0x60 live set - 0x244a5,
      0x244ea, 0x2451e, 0x24552, 0x26738, 0x27f7d, 0x28bc7,
      0x2fbf0, 0x36cf8, 0x3dfbb, 0x50ab7, 0x50e24, 0x537d4
      (the signal raise, PR-142) and 0x54b84; 0x26738 is a
      TWIN of the raise idiom in an unread function (index from
      0x26670). Combined with PR-142 (standing bits = zero):
      those are the ONLY ways a bit enters a live set. THE ONE
      MISSING LINK is named precisely: 0x4bd32 AND PR-143's
      queue drain 0x52b52 have ZERO direct references anywhere
      in the image - one shared, not-yet-found entry convention
      for the per-object dispatch passes (PR-134/135's tick
      dispatcher is where to look). 5 new open items counted
      (the dispatch entry; the 12 merge masks + the 0x26738
      twin; the class-swap pairs; the lion receive arms; the
      [vt+0x60] participation gate), PR-143 item 4 CLOSED,
      PR-143 item 1 narrowed to that one link. No mutants (no
      behaviour changed); all counts are whole-image (4 +0x78
      invokers, 14 live-set writes, 15 contact tables, 23
      receive slots, 22 broadcasts), and one scan slip (a 13-vs-
      14 address list) was caught by self-check and corrected
      before shipping. Full record: the PR-144 section of
      NEXT_STEPS.md (lion_map.txt carries the tables + censuses).

    - PR-145 READ THE KEY PROVIDERS AND THE INPUT FEED (reading
      record, no code change: PR-138..144 precedent). THE
      STANDING-BITS PROVIDER IS THE GENERIC KEY-FEED: 0x54910 at
      class slot +0x8c (dozens of object tables - 0x97c4c is
      +0x8c of PR-139's 0x97bc0; DRABINKA's 0x97cdc +0x8c = the
      zero stub 0x23a60 - so PR-142's "standing bits are zero"
      was measured on an OVERRIDE and is CORRECTED here). It
      returns a 13-byte buffer through the +0x8c step of the
      compositor, and its bit sources are verbatim: AUTHORED
      state flags [obj+0xa5] bit0/4/5 -> K_FIRE/K_ALT/K_JUMP;
      the signed INTENT field [obj+0xba] plus the open-window
      flag [obj+0xb6] bit 4 -> K_BKWD (window closed) / K_FWD
      (open); [obj+0xbe] > 0 / < 0 -> K_UP (37) / K_DOWN (38).
      THE LION'S provider (0x96960 +0x8c = 0x504e0) builds no
      bits itself: it calls [vt+0xf8] (ZARIA 0x1e2f0: the
      per-game input-reader family, 0x1e350 reads message modes
      4/9 and calls PR-133's facing flip 0x53670) and honours
      the authored flags against globals ds:0xcef5c... THE INPUT
      FEED IS NOT A KEYBOARD POLL: the consumer 0x549f0 writes
      the intent fields with +/-10 from message records - the
      engine's OWN constant, confirming PR-139's s=10 ESTIMATE -
      mode 1 targets the contact target [obj+0xd2] through the
      [vt+0x68] getter with a +-8 dead band, mode 4 raw-x; the
      frame loop copies a 12-byte block every iteration and
      feeds the lion via [lion.vt+0xcc] (PR-106 read the shape;
      the target is pinned here), and the 0xcf400 "input
      singleton" is an object with a mode at +0xb and record
      arrays (0x58690). A CONTACT PAIR SHARES ITS LIVE SETS:
      under a row guard ([rec+0x8] == current row), the contact
      target's OWN live set [[obj+0xd2]+0x10 +0x10 = [partner+
      0x60]] is ORed into this object's (0x54b65..0x54b84 - the
      context of PR-144's 14th census site is now read); the
      contact-list pop 0x551f0 sets [obj+0xd2] and flags
      [obj+0x106] when it empties. K_GEN1 HAS THREE RAISERS: an
      HP<100-gated MESSAGE TYPE (0x50df4..0x50e24 - and 0x50df4
      is a value AT the 0x50d20 base-receiver jump table, PR-
      105), two lion-family posts (0x244a5/0x24552), and a
      RANDOM K_GEN1/2/3 poster through a Borland-LCG PRNG
      (0x76bcc: imul 0x41c64e6d, +0x3039; 0x26670 selects 54/
      55/56; raised at 0x26738). THE FULL-GENRE RAISE IDIOM NOW
      HAS FOUR INSTANCES (0x537d4, 0x26738, 0x2fbf0 payload-fed,
      0x54964). HONEST CORRECTIONS, recorded not hidden: (i) PR-
      143 open item 2 (the drain's missing caller) was OUR
      mis-anchored function start - 0x52b50 IS the entry, and
      its callers were ALREADY RECORDED in PR-106 (the pump
      0x52b80 from the frame loop 0x4aed6 and the think pass
      0x4baed/0x4bb12, plus 0x55398/0x55696); (ii) the mask at
      0x2451e is K_FIRE, not a second K_GEN1; (iii) PR-142's
      zero-standing-bits generalization, above. [PR-149 ADDS TWO
      NEGATIVES to item 3 (what arms K_GEN1 at a full-HP ladder):
      the touch-damage route does not arm it - the ladder's +0xd0
      receiver 0x3e5e0 consumes no argument (init-and-print) and
      the +0xd0 sibling 8 of the 23 family tables use is `ret`
      (0x23c60) - and no signal-record poster in PR-147's closed
      census names bit 54 at a ladder. The item STAYS OPEN.]
      STILL GENUINELY
      UNREFERENCED (byte-level any-alignment scan over both
      segments): the contact-pass fragments 0x4bd32/0x4bdf6 (the
      lion-specific half: it calls [lion.vt+0x78] with the LION
      from [[level]+0x394])/0x4b500/0x4b602 - the climb blocker
      is now exactly ONE named gap: that pass's entry + the
      per-frame setter of the lion's contact target + the
      signal-record poster for AC_DRAB (delivery forms fully read:
      raise idiom + pair share; no direct site posts bits 10/12,
      so they can only arrive as signal records or via the
      share). 6 new open items counted; PR-142's 7 -> 6, PR-144
      item 2 half-closed, PR-108's 6 CLOSED, PR-106's 1
      narrowed. No mutants (no behaviour changed). Full record:
      the PR-145 section of NEXT_STEPS.md (key_map.txt carries
      the provider/bit/census map).

    - PR-146 CLOSED THE CONTACT DELIVERY AND THE SIGNAL POSTER
      QUESTION (reading record, no code change: PR-138..145
      precedent - but the gates were re-run fresh for this PR:
      make rc=0 / make win rc=0 zero warnings; verify-all with
      fixtures rc=0, 56 (ok), zero FAILED; fixture-free rc=0 with
      the same 23 named SKIPs). THE SIGNAL-RECORD BUILDERS ARE A
      CLOSED SET: the tag immediate 0x8138764 occurs SEVEN times
      in the image - six builders plus the receiver test at
      0x53780. THE AC_DRAB POSTER IS FOUND: 0x287a0 gates on
      [vt+0x3c](), takes the target from the indexed array
      [[obj+0x30]+0x40][1] via the getter 0x55760, requires the
      current record index [obj+0x5c]==0x12 and that record's
      type <=0x19, then calls THE TARGET'S +0x10 receive entry
      DIRECTLY (0x2885a) with a signal record whose bit index is
      0xa = 10. The other five: three broadcasts (0x4bc40) posting
      AC_KOREK(33)/AC_BECZKA(28)/AC_SPOKUJ(27), one direct queue
      push into THE LION (0x2c9a4, [[global]+0x394]) posting
      AC_LINA(21), and a loop over that same array posting
      K_GEN4(57) - the first key-bit poster. COUNTED NEGATIVE:
      bits 12 (AC_DRAB_CATCH) and 22 have no signal poster
      anywhere. The push primitive 0x52b10 is read at exact bytes
      (its entry is at 0x52b10; the linear sweep mis-decodes one
      byte early and hides it - the fourth mis-anchor of this
      series): node+0 link, +4 = type (edx), +8 = tag (ebx),
      +0xc = bit index (ecx) - matching the stack form and the
      receiver's +0x8 test. THE CONTACT LOOP: the pair-walk's
      entry is 0x4bd30 (PR-144 searched 0x4bd32 - a mis-anchor,
      PR-130 had already named this function), called once at
      0x527cb from the motion-pipeline merger 0x52790, which also
      calls the animacje/zwierzaki lists at 0x527f5/0x5281f; the
      merger returns an ANSWER-SET UNION (13 bytes), not a
      candidate list (PR-130's wording refined). And the contact
      entry IS the contact-list writer: 0x3d440 (class tables
      0x96a88/0x99e64 hold it at +0x78) appends a node (alloc
      0x1d = 16-byte header + a 13-byte answer set at node+0x10,
      built at 0x53f05..0x53f2a) onto THE ASKER's list [A+0xd2]
      via 0x53f00/0x53fe0; 0x549f0 merges that stored set into
      [A+0x60] under a node-kind+record guard (0x54b65..0x54b84,
      kind 2), and the lion's key provider 0x504e0 pops the head
      with 0x551f0. THAT IS THE ROUTE by which the ladder's
      0x3f1c0 answer (10/12) reaches the lion's live set - no
      signal record needed, and PR-145's "[[obj+0xd2]+0x10]+0x10
      = [partner+0x60]" is corrected to &node+0x10. One table
      block per class family carries the object entries AND the
      record-step/motion methods (0x97c9c: +0x0c 0x521f0, +0x2c
      0x52680, +0x30 0x4ff80, +0x34 0x52790, +0x44 0x53770 - and
      its +0x40 alias is PR-141's ladder table); the [obj+0x44]
      indexed-call contract is enumerated over all 17 sites, the
      merger slot has exactly ONE invoker. CORRECTIONS SHIPPED:
      PR-143's broadcast "bit census" was the RECORD-TYPE
      argument (the type census is now given: 7x2, 0x10x4, 0x12x5,
      0x13x3, 0x19x7, 0x1ax1; only 3 carry the tag, payloads
      33/28/27) - its conclusions stand; PR-145's intent-feed
      paragraph corrected again ([obj+0xbe] = record word +4 too,
      the branch selector is the head node's kind word
      [[obj+0xd2]+4], not a record mode or a partner field). 6
      new open items counted (the node builder's entry and
      [n+8]/[n+0xc]; the poster-table families and the [obj+0x30]
      array meaning - "index 1" is NOT proven to be the lion; the
      broadcast record types and the untagged type-0x10 record at
      0x247e7; the K_GEN4 loop's type; the two other list paths;
      whether 0x3f1c0 appends nodes + the class naming). No
      mutants (no behaviour changed). Full record: the PR-146
      section of NEXT_STEPS.md (contact_map.txt carries the
      builder table, the two censuses and the node map).

    - PR-147 CLOSED THE CLIMB FEED LAW ON THE ENGINE SIDE (reading
      record, no code change: PR-138..146 precedent; gates re-run
      fresh: make rc=0 / make win rc=0 zero warnings; verify-all
      with fixtures rc=0, 56 (ok), zero FAILED; fixture-free rc=0
      with the same 23 named SKIPs). THE CONTROL-KEYWORD ENCODING
      IS NOW BYTE-EXACT (the parser chain 0x51b35..0x51da0 over
      the 23 strings 0x9216e..0x9222f): BY_X 0x1, BY_Y 0x2,
      BY_TIME 0x3, BY_TX 0x4, BY_TY 0x5, FRAME_REV 0x8, SEQ_REV_X
      0x10, SEQ_REV_Y 0x20, CHECK_ANI 0x40, CHECK_ZW 0x80,
      CHECK_LN/CHECK_ALL 0x1c0 - PR-108's five-value mode note
      confirmed item for item; the dispatcher's jump table 0x54380
      is FIVE dwords ending at idx4 (mode 0 = the IMPLICIT no-
      keyword mode, fed by |v_x|; BY_TIME guarded on [obj+0xba]
      and falling through into the unguarded BY_TX body). THE
      CONSTANT 100 HAS EXACTLY ONE WRITER image-wide: 0x4c8b7
      `mov [eax+0x388],0x64` inside the init entry 0x4c8a2; the
      grep `+0x388]` has 4 hits total. THE GUARD WORD IS THE
      PLAYER'S OWN VERTICAL INTENT: the frame loop calls the
      LION's class-table slot +0xcc with its 12-byte input block
      EVERY frame before stepping it (0x4ae82 gate global ds:
      0xa94f0 - zero makes the loop return 2; 0x4aeb3 [world+
      0x394] = the lion; 0x4aeca `call [ebx+0xcc]`; then the step
      pump 0x52b80), and that slot has EXACTLY 7 call sites in
      the image (0x27e1d 0x282a1 0x344c8 0x4010e 0x4110c 0x41c01
      0x4aeca - a closed set). THE CAPTURE READER 0x582f0 maps
      the keyboard onto the block: word0 x = Home/Left 0x24b ->
      -20, Right 0x24d -> +20; WORD1 y = Down 0x250 -> +20, Up
      0x248 -> -20 (y+ = down, PR-99's sign confirmed); word2 +
      flag bytes carry Insert/Delete, F1..F10, Tab, Esc, Space,
      Ctrl, Shift, NumLock (full map in feed_map.txt). Words 0/1
      are only ever 0/+/-20. 0x58690 has EXACTLY ONE call site
      (the frame loop, 0x4ac6c), and its playback mode proves THE
      DEMO RECORD FORMAT: 16-byte records (three input words + a
      length at [rec+0xc]), cursor [x+0x2f] wrapping at that
      length - closing PR-145 open item 6 (the merge helper 0x225a0
      and fields [x+0x33]/[x+0x8] stay open). THE INTENT CHAIN,
      WITH CLOSED WRITE CENSUSES: 0x549f0 stores block word0 ->
      [obj+0xba] and word1 -> [obj+0xbe] UNCONDITIONALLY (0x549f9/
      0x54a08), then the head-node kind chain re-decides: kind 1
      = the x arm (+/-10 around node+8 with a +-8 dead band),
      KIND 4 = THE VERTICAL ARM (+/-10 toward node+0x1d, 0x54b16/
      0x54b3b/0x54b47 - PR-146's "raw-x variant" label CORRECTED:
      it reads the getter's +4, it is a y), kind 2 = the
      answer-set merge. [obj+0xbe] is written at exactly four
      sites and read at five: the mode-4 guard 0x5441b, the
      integrator's force 0x542cd, the K_UP/K_DOWN provider 0x5499d
      and 0x54030's copy - so WHILE A CONTACT NODE IS PRESENT the
      intent is the engine's own +/-10 toward the partner's y,
      otherwise the player's +/-20. PR-145's "s = 10, never any
      other magnitude" NARROWS to the contact path; the input
      path's constant is 20. AUTHORED DATA (byte census over
      ANIMACJE): BY_TY 12, BY_TX 0; every A_WSP/A_WSP_LIN block
      is `control=BY_TY|CHECK_ALL|SEQ_REV_Y` (wire word 0x205),
      F=0,100, T=160,50, rows 300/200.../300 - so under the read
      law a BY_TY row advances after 3 and then 2 frames WHERE
      THE VERTICAL INTENT IS HELD, and stops the instant it is
      not; the port's row model already matches this shape and
      lacks only the guard and the intent values. STRUCTURAL
      ODDITY COUNTED, NOT SMOOTHED: node+0x1d (the compared y) is
      beyond the node builder's 0x1d-byte allocation (whose answer
      set ends at +0x1c) - so who builds kinds 1/2/4 (PR-146 open
      item 1) now has teeth; node+0xc IS the ID field compared at
      0x54a24 (PR-108), which half-resolves that item. [CLOSED by
      PR-149: the builders are pinned (0x53eb0/0x53f00 kind 1,
      the answer-copying 0x53f50 kind 2, the kind-4 patch chain)
      and the fit question was no oddity - kind-4 nodes are
      allocated 0x21 at the one 0x21 site in the chain itself.]
      3 new open
      items counted (the node builders and the allocation fit; the
      ds:0xa94f0 loop gate; the singleton's merge/second-cursor
      fields). Zero open laws remain for the climb: what is left
      is the implementation decision (PR-148 SHIPPED its first
      half - the guard and the intent word are code; the
      contact-answer route is what remains). No mutants (no
      behaviour changed). Full record: the
      PR-147 section of NEXT_STEPS.md (feed_map.txt carries the
      keyword encoding, the keyboard map, the two censuses and the
      authored census).
    - PR-148 MADE THE INPUT/GUARD LAW BEHAVIOUR (the first CODE
      change since PR-140's afefe73; gates re-run fresh: make
      rc=0 / make win rc=0 zero warnings; verify-all with
      fixtures rc=0, 56 (ok), zero FAILED - the count is
      unchanged because the PR-108 leg was RE-PINNED IN PLACE
      rather than added to; fixture-free rc=0, the same 23 named
      SKIPs). THE PORT NOW CARRIES THE ENGINE'S TWO FACTS: a
      model bit for `BY_TY` (LL_CTRL_BY_TY, parser-mapped) and
      the player's `vert_intent` word - the engine's [obj+0xbe]
      with its sign semantics (Up negative, y+ = down, the
      0x582f0 signs) at the port's single validated intent scale
      (the 20-vs-10 tension is NAMED at LL_PLAY_INTENT_FULL, and
      mutation M8 shows the shared scale is observable on the x
      gait legs). Section 7's mode-4 feed guard copies 0x5441b
      byte-for-byte in SHAPE: feed 100 only while the word is
      non-zero, row compare runs regardless, so a BY_TY row
      closes only on QUALIFYING iterations and the partial
      accumulator carries across the gap (the engine's [obj+0xa1]
      does the same) - while the held-intent spans stay EXACTLY
      PR-108's, which proves the guard only subtracts feeds. The
      re-pinned leg: no-keys = entered and frozen for 25
      iterations, no wrap, and `!AC_DRAB` still fires from a
      state whose list never ticked (transition pass vs row pass,
      as in the engine); Up held = the PR-108 spans; release
      mid-row keeps the carried 100, Down restarts on it, the
      wrap lands at t=19 not t=17; Up/Down qualify alike (the
      guard is sign-blind like the cmp/je). NOT copied, counted:
      the BY_TIME guard 0x543f2 on x-intent (a regression study,
      PR-102's reading stands) and the word's use as a v_y
      force. THE SESSION'S CLAIMED RISE LAW AT 0x50252 WAS
      CAUGHT DEAD BEFORE IT COULD BE CODED: the LION's table
      0x96960 +0x74 is 0x50320 (byte-exact dump; its other
      slots match PR-144's anchors), 0x50252 has ZERO references
      image-wide (rel32 scan AND dword scan - the neighbour
      0x502b0 proves the scan pair is sound, it is reached by a
      direct call from 0x5032c), and what the frame loop's
      [lion.vt+0x74] call at 0x4afea really runs is 0x50320 ->
      an oddness test on [obj+0x102] -> the friction/ground gate
      0x54c00. So NO vertical motion was added, and 0x50252
      appears in no shipped record - nothing needed re-editing.
      One PR-147 sentence NARROWED, not retracted: the y-force
      multiplier at 0x542cd is `[param+0x4]` of the struct whose
      +0x8 PR-132 read as the raised T word - the instruction
      shape (intent * param/20 into v_y) stands, the "-80 at
      T=160" figure is conditional on the unread field map. 4
      new open items counted (the y-force path and param field
      map; the BY_TIME guard study; the dead region 0x5024f..;
      the scene-side mirror). A sharper BLOCK-level census (split
      the archive by [NAME] block, not string occurrence): 12
      BY_TY blocks firm across three names - A_WSP x5,
      A_WSP_LIN x6, A_DRAB x1 - but MEMBER attribution is
      anchoring-dependent, and this CORRECTS PR-147's list: the
      block-start anchor puts the last four near F4ZDYBYA/DEMO/
      BALONIK, not KLADKA1/MIOTLA_W/XKALUZA (first eight sit
      under 2LEW.MOV x4 / LEWPL.MOV x4 both ways; the member
      table is the authority and is unparsed - named gap, joined
      PR-147 item 1). Any of those blocks on a SCENE member
      parses the bit now while obj_advance ignores it - mirror
      counted, not silently half-done. 10
      mutations run: M1-M8 all DETECTED (guard deleted, inverted,
      wrong axis, parser-bit dropped, carry-zeroed, guard
      extended to all modes, admit-gated-instead-of-feed, scale
      20); M9 (intent sign) and M10 (keys-not-word) SURVIVE and
      are reported - they are equivalent by construction until
      the y-force path (M9) and the kind-4 contact override
      (M10) exist, which is exactly why the port carries a WORD
      and not a key test. The climb at ZARIA.01 x=180 still does
      not climb: its blocker is the contact-answer route, not
      the guard - that is PR-149. Full record: the PR-148
      section of NEXT_STEPS.md; battery driver at
      /tmp/opencode/pr148/run_mut.sh (kept out of git with the
      scratch tree).
    - PR-149 (READING, no code, no new test - the PR-138..148
      precedent): THE NODE BUILDERS ARE PINNED AND THE 0x1d
      QUESTION IS CLOSED. Kind-1 nodes: builder entries 0x53eb0
      and 0x53f00 (zero the 13-byte set at [n+0x10..0x1c]).
      Kind-2: entry 0x53f50 BUILDS kind=2 and COPIES the
      caller's 13-byte answer set in (rep movs, 0x53f6e..
      0x53f88) - the answer-carrying builder, its only rel32
      caller 0x3d66d. Kind-4: the kind-1 builder (0x24678) then
      patches [rec+4]=4 (0x24681) and [rec+0x1d]=edx (0x24688).
      THE ALLOC CENSUS: 0x5c320 is the general allocator (617
      call sites), and a whole-image scan finds exactly THREE
      sites sized 0x1d (0x1ef4c, 0x3d5fc, 0x3d64f) and exactly
      ONE sized 0x21 - 0x2465f, three instructions before the
      kind-4 patch. So the +0x1d y field sits INSIDE the kind-4
      block; and the +0x1d access census shows one store
      (0x24688) and only kind-4-handler reads (0x54b11/0x54b36).
      PR-147 open item 1 CLOSED. The chain's function (entry
      0x24440) is reached from one class-table dword (0x989ec)
      and iterates [obj+0x136] - which data feeds it stays open.
      [CORRECTED by PR-151 F1/F3: the chain is NOT inside
      0x24440 (that method ends at its ret 0x4, 0x244c0) -
      0x24678 sits inside the CLASS HOOK 0x245b0 = TLewEna
      +0x90 (0x989ec is +0x100 of that table), the slot the
      post-transition pipeline calls (exactly two call sites
      image-wide). And the array's only writer is the SADES
      hook 0x2e6a0: runtime per-level state, NOT authored
      attach data - PR-108's question ANSWERED NO.]
      ALSO: the ladder's answer method 0x3f1c0 is now read as a
      three-predicate tree with the OPERANDS pinned: P1 = the
      TOUCHER's block at [q+0xc5] (q = the +0x68 getter's
      result; 23/24 family tables return self via the ret-eax
      getter 0x23b60, the lion's returns &self+0xd6 = 0x23900)
      vs the LADDER's stored block at self+0xc5, through the
      class rect-INTERSECTION slot +0x8 (0x23e00, all 24
      tables) - true -> AC_EDGE bit 8; false -> the 70% INSET
      probe (0x49fb0, n=0x46, over a stack copy of
      [self+0xb1..0xbd]); P2 vs the inset rect (false ->
      inert); true -> P3 vs the ladder's FULL rect (0x3f2af) -
      true -> AC_DRAB_CATCH+AC_DRAB merged through 0x49580,
      false -> AC_DRAB alone. [CORRECTED by PR-150, fixed in
      place: P2 and P3 are SWAPPED - the FIRST +0x8 test
      (0x3f29b `mov edx,ecx` - ecx = self+0xb1 via the
      `add ecx,0xb1` at 0x3f283) is the FULL extent, and the
      probe is the SECOND (0x3f2ad `mov edx,esp`); the 70%
      inset is SYMMETRIC (0x49fde/0x49fe0 share one half =
      trunc((w - trunc(0.7w))/2) - the probe keeps the
      CENTRAL 70%, so P3-true-with-P2-false never happens);
      in P1 the bounds side is the LADDER's [self+0xc5] only
      (the toucher's getter block is the POINT operand,
      `lea edx,[ecx+0xc5]` reads ecx = the answering object);
      and the compared row values are 1-BASED ROW POSITIONS
      (the authored (N) field is copied into the cursor at
      0x52046..0x5204d; drab.MOV's `A_DROP(1)` targets that
      state's FIRST line `2,500`, so the row lines' leading
      numbers are frame selectors): position 1 = A_UP's idle
      row (flag branch; a fresh child list is empty, so
      inert), position 2 - the drop's `3,400` line - is the
      ONLY answer row. PR-150's legs pin all of this.]
      [FIELD IDENTITY SUPERSEDED by PR-157 F3 + PR-158 F2:
      the gate reads the STATE slot ([ecx+0x5c], ecx = SELF
      at 0x3f1c6), and `(N)` lands in the state record's cur-
      sor [[rec]+4] (0x53616); the legs and the port's fi-
      model survive both readings - see the restored PR-150
      record in NEXT_STEPS.md.]
      PR-141's four outcomes CONFIRMED,
      operands now the record's, and every slot in the chain is
      a base-class family slot - the ladder-specific content of
      its answer is its DATA only. The lea-pair idiom (lea ebx,
      [eax+0xb1] ... lea eax,[ebx-0xb1]; add eax,0xc5) exists
      at exactly TWO ctor sites: DRABINKA 0x1f8f3 and LINA
      0x4e871 (ctor 0x4e831, same 0xd9 alloc) - and this
      session's earlier self-pointer reading of it is CORRECTED:
      it computes the block base &obj+0xc5 for the ctors'
      zero-stores, and direct +0xc5 accesses are exactly ONE
      instruction image-wide (0x3f228).
      PR-141 ITEM 4 ANSWERED: a second `LINA` literal (0x92005)
      IS the factory branch (token compare at 0x4e81d) - and its
      chain-mates name factory branches for SURPRISE (0x4e62f -
      the G-12 route), TRASA/TRASA2 and more (counted leads).
      The attribute vocabulary is now enumerated from the
      engine's own trace strings (0x9142c..0x9161x: bounds,
      number, sample, effects, count, div, damage, width, dist,
      offset, speed, prior, loop, scope, energy, hit_time,
      coef...); and the ladder's +0xd0 entry 0x3e5e0 proves out
      as an INIT-AND-PRINT entry that raises NOTHING - plus a
      second negative for the K_GEN1 arm (the base sibling 8 of
      the 23 family tables use, 0x23c60, is `ret`). The K_GEN1
      arm stays the named blocker for the in-game climb chain,
      so PR-150's climb probe raises the signal directly.
      PR-150 is now better specified: the candidate walk gated
      by the state-token wire bits, the asked object's +0x78
      answer entry, an answer-carrying node (this record's
      builder/alloc shapes) into [obj+0xd2], the kind-2 merge
      into the live set - with the ladder predicate tree as the
      first authored answer. 6 new open items counted. Full
      record: the PR-149 section of NEXT_STEPS.md (raw pass
      notes at /tmp/opencode/pr149/nodes_notes.md, kept out of
      git).
   - PR-150 (CODE + READING): THE LADDER'S ANSWER IS NOW
      BEHAVIOUR - AND THE PASS CORRECTED ITS OWN PR-149 WORDING
      THREE TIMES, IN PLACE, NOT HIDDEN. (1) THE ROW GATE'S
      INDEX SPACE CLOSED: [obj+0x5c] is the 1-BASED ROW POSITION
      in the current state's row-record table - the transition
      apply 0x52000 writes the authored (N) field verbatim into
      the cursor (0x52046..0x5204d), the +0x44 getter 0x23920
      indexes the records with it, and drab.MOV's `A_DROP(1)`
      (its only exit, K_GEN1-gated) targets the FIRST line
      `2,500` - so the row lines' leading numbers are FRAME
      selectors (drab art is one frame per drop step), not row
      ids. [IDENTITY SUPERSEDED by PR-157 F3 + PR-158 F2 -
      the field is the rows-array/STATE index and `(N)` goes
      to the record cursor [[rec]+4]; the ZARIA.01 legs and
      the port's model hold under both readings.] The port's
      cursor is the 0-based twin: the FLAG
      branch = position 1 (A_UP idle, drop's first row, park),
      the ONLY answer row = position 2 (the drop's `3,400`
      line, 500 ms in), positions >= 3 inert. A base method at
      0x238e4 (row==2 or the rect test) corroborates. (2) The
      0x49fb0 probe is a SYMMETRIC inset keeping the CENTRAL
      70% - 0x49fde/0x49fe0 share one eax = trunc((w -
      trunc(0.7w))/2); PR-149's "asymmetric [p]+=inset/[p+8]-=
      half" wording was wrong. (3) P2/P3 ORDER SWAPPED: the
      first +0x8 test is the FULL extent (edx = self+0xb1 via
      0x3f283; false -> inert 0x3f364), the SECOND is the 70%
      PROBE (edx = esp, 0x3f2ad; true -> CATCH|DRAB merged,
      false -> DRAB ALONE at 0x3f355) - with 0x23e00 re-read
      whole (bounds test, inclusive, x-words = the same pair
      0x49fb0 insets), all four outcomes are consistent with
      ONE rectangle format. CODE: ladder_class/pt_in_blk/
      blk_probe70/ladder_answer_core/blk_of_obj/pt_of/
      ladder_answer_for in ll_level.c plus two play_collide
      call sites (solid and non-solid PR-92 passes, both
      post-shape-meet post-push); merged straight into the
      lion's pulse set (no node list - item counted), read by
      the NEXT iteration's transitions. The rung block is
      sourced from the object's own CURRENT ART BBOX through
      the same obj_shape_elem path the contact passes use
      (clamps + drawn polarity included); with no art/shape,
      the ctor-shaped 1x1 at the object; the edge block
      ([self+0xc5]'s role) is a MODEL CHOICE, a 1x1 at the
      object too (the ctor never stores +0xc5 at all). TEST:
      ladder_answer_selftest in the mov chain - probe law at
      every hand-computed truncation edge (incl. w<=3 = probe
      equals rung, mirrored words), the decision table with the
      edge-first priority and inclusive bounds, class scope,
      and the row gate end to end (the ZARIA-shaped idle row
      answers nothing at ANY asker; the clamp leg). ZARIA.01
      changes NOTHING observable, both reasons pinned: no CHECK
      bit on A_WALK's list gate, and the ladder never leaves
      A_UP (its only exit is K_GEN1-gated; no raiser at the
      object per PR-149 F5). 10 mutations, ALL DETECTED, none
      survive - M4/M6 are exactly this session's two earlier
      wrong readings, caught by the legs. 7 open items counted
      (authored rectangle format; LINA-family class table -
      its .MOV shape is now read identical, drab scripts fully
      read incl. ZARIA.02's DRABK pair; object-asked answers
      and mid-air asking; node lifetime + 0x54b6a merge guard;
      the [obj+0xa5]/[self+0x66] flag feature unread; WAIT
      row-gate + K_GEN1 arm STILL the blocker for any in-game
      answer; the 0x238e4/0x98274 small gaps). Gates: make and
      make win zero warnings; verify-all rc=0 both modes (56
      (ok) with fixtures; 10 (ok) + the same 23 named SKIPs
      without); full record: the PR-150 section of NEXT_STEPS.
      Next: the kind-4 contact node and its +/-10 y-override
      (PR-149 built the node shape; PR-147 read the override;
      answers can now PLANT one).
      [CORRECTED by PR-151 before a line was written: answers
      plant kind-2 nodes (PR-149 F1 - 0x53f50 is the answer-
      copying builder); kind-4 nodes come only from the per-
      class post-transition hook +0x90, and ZARIA-era hooks
      supply none - the y-override code pass is DEFERRED to
      an ENA/SADES/TUNGA-era controller. See the PR-151
      bullet next.]
   - PR-151 (READING, no code, no new test - the PR-138..150
     precedent): THE POST-TRANSITION HOOK +0x90 IS THE PER-LEVEL
     BEHAVIOUR LAYER, AND THE kind-4 SUPPLY QUESTION IS CLOSED -
     which also corrects the framing PR-150's Next shipped with.
     WHY: before writing the kind-4 code pass, this pass checked
     its premise against the bytes ("answers plant the kind-4
     node") - WRONG: answers plant kind-2 nodes (PR-149 F1);
     kind-4 nodes come from a class-table hook this project had
     never read. F1: the controller pipeline 0x53520..0x5359a
     copies the live set, runs the 0x52000 transition scan
     (writing the (row, N) pair to two out slots), calls
     [[obj+8]+0x90] with (self, &out1, &out2), applies the
     rewritten pair (0x535a0, PR-133's record/row switch), THEN 0x749a0-clears
     the live set - the port's evaluate-then-clear ordering
     CONFIRMED; the +0x90 call sites are exactly two (0x5357d,
     0x3edaf). F2: ZARIA's hook 0x1e250 is a TRANSITION
     REDIRECTOR read whole (0x1e250..0x1e2ef): facing flip via
     0x53670 when out1==4 and [obj+0x10e]*facing>0; -30 from
     [obj+0xad] on a row-0xa redirect; ONE-SHOT rewrite of any
     fired transition to (9,0) when [obj+0x12a] is armed (no
     live ZARIA-path writer of that flag found - counted);
     [obj+0x116]=1 at row 9 with live-set bit 2; CANCEL of
     out1==0xc/0xb under globals ds:0xcef64/0xcef68; tail-call
     of the base hook 0x61670 (whose first arm: flag byte bit
     0x40 -> force (2,0)). F3: the kind-4 builder IS the ENA
     hook - TLewEna 0x988ec +0x90 = 0x245b0 (identity
     cross-checked through its +0xcc = 0x50340), and the ONLY
     writer of its [obj+0x136] array is the SADES hook 0x2e6a0
     - so the array is runtime state per-level code maintains,
     NOT PR-93's authored attach data: PR-108's question
     ANSWERED NO. ZARIA-era controllers supply no kind-4 node
     -> the override code pass is DEFERRED to an ENA-era
     controller. F4: the strongest K_GEN1 lead yet - the ZARIA
     ctor links a COMPANION [obj+0x112] via 0x4dcc0 over a
     0x497b0-built query (seed ptr [ds:0x961fc] -> 0x1034c,
     grammar unread), and the +0x100 hand-off method's ENA
     variant 0x24440 raises K_GEN1 (byte 6 bit 6) into the
     RESOLVED COMPANION's live set - the first bit-54 raiser
     outside GEN row tails and kind-9 messages; ZARIA's tables
     hold the BASE +0x100 (0x50c70) with no visible raise
     (counted), and the two call sites (0x50c2a, 0x41b6a) are
     unread: a LEAD with named bodies, not an answer.
     [PR-152 read the hand-off's WHEN: it is gated by TWO
     per-class getters - a +0x104 row window (ZARIA: row
     POSITIONS 7-8 only; base classes: NEVER) and a +0x108
     constant match against the current row-record's +0x4
     word (ZARIA: the value 6, which is EXACTLY the field-2
     value on LEW.MOV's A_WALK/A_ZABLOK rows 7-8, the same
     gate field PR-100 measured). And 0x4dcc0/the query were
     mis-framed here: it is a placement-line name reader and
     a KEY-REGISTRY resolution (the sub-line factory, cf.
     ZWIERZ1's POCISK/PICT children) - ZARIA statically names
     NO companion; which object resolves is a runtime
     question.] F5:
     PR-129's "0x52cc0 ... all with edx=9" CORRECTED - the
     list (14 sites, recorded) is right, 0x3f77e passes
     edx=[self+0x4c]; its method 0x3f760 (from the table
     dword 0xa0790, TUNGA region) is a TYPE-0x1c message
     receiver. 9 open items counted (base-hook body; the
     [obj+0x12a] arm question; the +0x90 census; globals
     0xcef4c/64/68; the two +0x100 bodies; the query grammar +
     seed; the message vocabulary + the 0xa0790 class; the
     0x2e6a0 cadence; who zeroes (out1,out2) on no-match).
     NEXT: the K_GEN1 arm reading pass with that lead order;
     then the y-force/0xcf400 block path; then the WAIT law.
     Full record: the PR-151 section of NEXT_STEPS.md (scratch
     at /tmp/opencode/pr151/, kept out of git).
   - PR-152 (READING, no code, no new test - the PR-138..151
     precedent): THE COMPANION HAND-OFF GATES ARE NOW NAMED -
     A PER-CLASS +0x104 ROW WINDOW (ZARIA: row POSITIONS 7-8,
     which in LEW.MOV are exactly the `4,6,GEN AC_TIME` lines
     of A_WALK and `4,6`/`5,6` of A_ZABLOK) AND A +0x104
     CONSTANT MATCH (ZARIA -6, i.e. the record word equal to
     6 = that same field value; SADES -4; base/ENA2 +1 via
     [obj+0x9d]; TUNGA2 -14; ENA computed), and the base
     pair's +0x104 is `xor eax,eax` - NO hand-off for plain
     base classes, a per-level feature. WHY: PR-151's lead (b)
     ("what makes +0x100 fire"). F1: 0x4dcc0 is PR-103's
     placement-line name reader (PR-151's "level-object
     SEARCH" was wrong, bracketed) - the sub-line children in
     ZWIERZ1.DES (`POCISK,jajo.ani...` under SEP, `PICT,
     panc.seq...` under NOSOROG) are exactly its one-line
     input. F2: the ZARIA ctor "query" is a token-list builder
     over an EMPTY name; resolution is by KEY REGISTRY -
     [ds:0xcef14] is the static singleton built at 0x5bf10
     over the descriptor root 0x961fc, whose first descriptor
     is (0x1034c, 0x57a00) with 0x1034c an EXECUTABLE match
     stub IN .data (the "SQV" bytes are code); the list's
     later entries are signal-number/bit triples (0x9627c:
     5/K_F5 with 0x10000/0x70000 = AC_RECT bits). So ZARIA
     names its companion NOTHING statically. Negative while
     hunting: 'LEWPL' is absent from the whole image - "class
     LEWPL" was always a session label for the player's own
     .MOV. ZARIA1.DES read whole (`lew=` = the controller
     class + LEW.ZAR + lew.mov + lew.des, no sub-lines).
     F3: the hand-off machinery 0x50b30..0x50c2c read end to
     end + the getter censuses (constants above); the scan
     compares the current row-record's +0x4 word; on match +
     [obj+0x112]!=0 + reentry-flag ds:0xcef4c==0 it sets the
     flag, calls +0x110 (ZARIA's = a bare `ret`) and +0x100;
     no companion or [ds:0xa94f4]!=0 instead ZEROES the
     (out1,out2) pair (PR-151 item 9 answered in part).
     F4: the base +0x100 tail = companion +0x118 (ZARIA's =
     0x50c50: enqueue to the global message queue 0x52b10
     [CORRECTED by PR-153 F2: NOT global - the append lands
     on the COMPANION's own list at [companion+0x10]]
     carrying the controller's FACING; "type 5" in PR-151 was
     my misread of the enqueue's edx) + the 0xb1/20 copy +
     [companion+0xee] in {2,3} branch + [[companion+8]+0xe0]
     or 0x531b0([ds:0xa94ec],...) ELSE the companion's +0x80
     (ZARIA: 0x501e0 -> 0x4aa60([ds:0xcef40],arg), unread).
     F5: THE SIX-LINK CANDIDATE CHAIN to K_GEN1, all links
     now named (rows 7-8 -> scan -> hand-off -> queue node ->
     receiver 0x50d60 whose type-9 arm 0x50df4 is PR-70's
     bit-54 setter [VERIFIED by PR-153 F5; but the queue is
     PER-OBJECT at [obj+0x10], the node is TYPE 5 not 9, the
     receiver named is the BASE's - ZARIA's own 0x1e130 has
     the live type-5 arm - and no static type-9 feeder exists
     (F6); corrected brackets in the PR-153 record] -> ladder
     A_UP(1)=K_GEN1 / player climb
     rows); the deciding unknowns are the record layout (is
     [record+4] the row line's second field?) and the queue
     consumer + node type. The dead 0x1e132 handler pairs
     with live code for the first time: its type-5 arm bumps
     [ds:0xa94f4], which is exactly the counter that DISABLES
     hand-offs (0x50b6a) [FLIPPED by PR-153 F3/F4: the pair-
     ing is LIVE-to-LIVE and the counter is the one-shot hand-
     off latch; the "dead handler" is ZARIA's own +0x10 slot
     0x1e130 - a listing boundary artifact; and 0x52cc0 is a
     hit pipeline, not a queue post]. 8 open items counted (queue
     consumer + node type; the key-chain slots +0xc/+0x14,
     0x57d30, 0x9620c.. entries, the stub's code-range
     branch; 0x52b10 head; 0x4aa60 / +0xe0; the [obj+0x160]
     machine; the type-9..21 arms; the record layout + which
     states reach rows 7-8; the 0x50b30.. method entry).
     NEXT: the chain trace (record layout first), then the
     WAIT row-gate law - now DOUBLY load-bearing since field
     2 = 6 IS the hand-off constant - then the y-force path.
     Full record: the PR-152 section of NEXT_STEPS.md
     (scratch incl. the LL.iso LEVELDES extraction at
     /tmp/opencode/pr152/, kept out of git).
   - PR-153 (READING, no code, no new test - the PR-138..152
     precedent): THE K_GEN1 CHAIN TRACE - TWO DECISIVE GAPS
     CLOSED AND THE "DEAD HANDLER" IS RESURRECTED: ZARIA'S
     MESSAGE RECEIVER 0x1e130 IS TLewZaria/TLewZariaB's OWN
     +0x10 SLOT (PR-129's "UNREFERENCED - dead override" was
     an objdump BOUNDARY ARTIFACT - the entry sits 2 bytes
     before the listing started at 0x1e132; a +0x10-slot
     census found 0x96970/0x97980/0x97aa8). WHY: PR-152's
     two deciding gaps - the record layout and the queue con-
     sumer. F1 ROW LAYOUT CLOSED: the [obj+0x50] rows are
     72-byte records (0x518f0 alloc, 0x517e0 copy grammar:
     +0x0 = field 1 = frame selector, +0x4 = field 2 = THE
     PR-100 GATE VALUE, vtable at +0x44 with enter/exit
     slots +0x1c/+0x20 used by 0x535a0) - so the hand-off
     scan's [record+0x4] IS the authored second field: ZARIA
     matches 6 = the A_WALK/A_ZABLOK rows at positions 7-8
     and nothing else in the +0x104 window. Row records vs
     21-byte transition records vs the GEN/'Wait'-token nodes
     (0xa11e0/0xa11c8 vtables at +0x4, name-registered via
     0x47d60 into the key registry, token grammar 0x919bc)
     are now cleanly separated; PR-126's WAIT-node question
     half-answered (keyed nodes, getters open). F2 QUEUE
     CLOSED: 0x52b10 appends 16-byte nodes {+4 type, +8 pay-
     load, +c aux} to a per-object list at [obj+0x10];
     0x52b50 drains each through [[obj+8]+0x10] = the RE-
     CEIVER; pump 0x52b80 = drain + tick +0x14 + conditional
     +0x0(edx=2) on [obj+0x28], driven by the container loop
     0x55380; the hand-off node is TYPE 5 with the facing
     sign as payload. F3 the hand-off RECEIVER arm (type 5:
     payload .+0 call, [ds:0xa94f4]++ = the one-shot hand-off
     latch (0x50b6a), a 2-instruction stamp 0x44900 onto the
     .des-built singleton [ds:0xcef40]+0x38c ([t+0x5c]=55,
     [t+0x6c]=count), then 0x52cc0(self,9)); sibling receivers
     in 2 other eras (0x27470 from table slot 0x9a55c, kind
     0x31; 0x33edb); type-0xc arm = the LIVE armer of [obj+
     0x12a] (PR-151's open item corrected). F4: 0x52cc0 is a
     HIT PIPELINE (slot +0x30 getter; +0x24 = 0x52a50 asks
     THE PLAYER [[0xcef40]+0x394]+0x68; +0x28 = 0x52a90; then
     0x56600 -> [[self+8]+0x18], bare-ret in ALL lion tables,
     0x3f0f0 geometry in the ladder's; [self+0x20]=1) - not a
     queue post; the stamp object is created by the main-.des
     key handler (region 0x4d02a 'sample'/0x4d067 'lew'...,
     0x8c-byte alloc, ctor 0x443b0 stores the table pointer
     0x98278 = PR-150's 0x98274 gap family, referenced from
     ZARIA lion code). F5: the transition pipeline has ONE
     entry - 0x536f0, the ladder-table tick - and the LION
     tables' +0x14 0x550a0 calls it too (0x53520/0x535a0/
     0x523f0 all single-callers inside it): slot order +0x8c
     feed (ladder = EMPTY-set stub 0x23a60; lion = 0x504e0),
     row-key OR 0x523f0, +0x54 threshold nodes, 0x53520 scan
     (context = OWN live set), +0x90 hook, 0x749a0 fill-0
     clear (PR-151 byte-confirmed); bit-54 arm 0x50df4 BYTE-
     VERIFIED: [obj+0xad]<100 guard, buffer[6] |= 0x40 via
     the 0x749a0 fill, 0x49580-OR into [obj+0x60], [obj+
     0xad] += 32 (same field as ZARIA's hook -30 bite - one
     countdown resource). F6 the gap fully characterized:
     drab.MOV + lina.MOV read WHOLE - ZERO GEN tokens (full-
     script negative), ladder +0x8c = empty stub, and a 66-
     site 0x52b10 edx census shows NO static type 9 (type 5
     at 6 sites incl. the hand-off; 4 dynamic-edx sites open).
     So nothing static raises bit 54 on the ladder; the bridge
     candidates are named (PR-154 Next: the 0x98274/0x98278
     class table, the dynamic-enqueue sites, the ds:0xcf0ac
     kind-table builders, the WAIT-node getters) [PR-154 OUT-
     COME: the 0x98278 lead is FALSE (an unclassed two-dword-
     record list object; 0x98274 is a vector END-bound), the
     dynamic sites are all static, the kind tables are runtime-
     only BSS, the WAIT getters chain and COMPOSE keys - the
     PR-154 bullet below carries the details]. 8 new open
     items counted. Gates: docs-only; every added line <= 79;
     baselines unchanged (NEXT_STEPS 71, HANDOFF 72); no game
     data in git; image unchanged. Full record: the PR-153
     section of NEXT_STEPS.md (scratch /tmp/opencode/pr153/).
   - PR-154 (READING, no code, no new test - the PR-138..153
     precedent): THE BIT-54 BRIDGE, PASS 1 - TWO BRIDGE HYPO-
     THESES CLOSED NEGATIVE AND THREE PR-153 OPEN ITEMS CLOSED.
     WHY: PR-153 narrowed the K_GEN1 gap to "who writes bit 54
     (or the row-55 stamp) into the right object" and named
     threads (a)-(d). F1 THE STAMP IS BOOKKEEPING, NOT A SIG-
     NAL: the 0x98278 "class table" thread was a false lead -
     0x98278 is the class of an 8-byte TWO-DWORD RECORD (its
     +0x10 = a free-path receiver (bit2 frees the EMPTY STRING
     at 0x96854/0x96858, in the TLewZaria table tail; bit1 =
     0x5c370 free-self), +0x14 = a plain two-dword copy, +0x18
     = 0x23e00), and the 0x8c singleton [[0xcef40]+0x38c] is a
     LIST OBJECT (ctor 0x443b0 stamps {0,0x98278,0} 12-byte
     slots from [t+0x14] to [t+0x83] and NEVER writes a class
     pair at +0x8/+0xc) built in the main-.des key chain after
     the 'lew' compare (0x4d06f/0x91e59) in the 'demolist' re-
     gion (0x91e82), guarded by a registry search for the li-
     teral 'none' (0x4d5d6) with capacity 0x42; 0x98274 itself
     is just the END BOUND of a static 17-pointer method-pro-
     totype vector (0x98230..0x98274) borrowed+trimmed by 0x4a050
     (a vector tail-trim, 13 sites) at 5 ZARIA-controller sites
     - so PR-150's counted "0x98274 gap" DISSOLVES and the
     "row-55 stamp" bridge alternative is CLOSED (the stamped
     fields [t+0x5c]=55/[t+0x6c]=count live in an unclassed
     list; a 4TH 0x44900 site, inside the lion-base hit method
     0x50720 (hit countdown [obj+0x102] decrement + bit-0x40/
     0x40 tests + [obj+0xc2]/3 + 0x54860), confirms it as a
     bookkeeping sink). F2 THE kind-9 CENSUS IS COMPLETE: of
     the 14 0x52cc0 sites the kind is EXACTLY 9 at TWO - both
     hand-off receiver arms (0x1e185, 0x274b1); others 1/2/
     0x31/0x18/static-field reads; so the ONLY functional arm
     of a hand-off's hit path is the kind-object passed into
     the companion's +0x18 (bare-ret in all lion tables; 0x3f0f0
     in the ladder's). F3 THE KIND TABLES ARE RUNTIME-ONLY:
     ds:0xcf0ac/0xcf0b4 are BSS zeros with EXACTLY two ref-
     erences image-wide (0x56604/0x5660e inside 0x56600, entry
     0x56602) and no builder among static references - the
     census cannot decide kind-9; a runtime table can. F4/F5
     THE ENQUEUE CENSUS CORRECTED: all 4 "dynamic-edx" sites
     re-read at raw width - 0x1ef22 is STATIC edx=0xc (set at
     0x1ef07, outside PR-153's 40-byte window): a type-0xc
     node onto THE PLAYER ([[0xcef40]+0x394]) guarded by [self+
     0x128], payload [ds:0xcef40] - PR-153 item 7 CLOSED, the
     [obj+0x12a] armer now has a named static poster (method
     0x1eee0, which tests the received node's +4 == 2 first);
     0x330a7 edx=5 onto a freshly-CLONED object (alloc 0xf6,
     class pair 0x9d088/0x9d080, field copies via 0x54030) - a
     second type-5 hand-off-shaped site with the [self+0x6d]
     POINTER payload (PR-153 item 4 half-resolved); 0x4bcd7/
     0x4bd13 are one list-broadcast loop (method 0x4bc40, walks
     [level+0x44]+0x58 lists with the +0x34 chain and a +0xc
     filter) whose node +4 slot carries the CALLER POINTER - a
     pointer-valued second usage of the "type" field (counted).
     The truly-dynamic set is EMPTY and the bit-54 receiver arm
     still has no static feeder. F6 NODE GETTERS CHAIN - PR-126
     item 3 CLOSED: the node vtable BASES are 0xa11c0/0xa11d8
     (the PR-153 0xa11c8/0xa11e0 pair is a boundary artifact
     again); the key getters 0x457e0/0x45800 delegate to the
     node's own child ([[child+4]+4]), 0x45970 copies the 12-
     byte authored payload at node+8, 0x45870 chains +0xc and
     ORs - childless nodes answer EMPTY, so authored GEN and
     WAIT rows compose their key through the chain and the row
     gate is now purely a CODE question. 7 new open items count-
     ed (the list object's +0x5c/+0x6c semantics; the runtime
     kind-table writer; the +4 type-vs-pointer duality and the
     0x9d088 family; the prototype vectors; the 2 unread 0x52cc0
     sites; the exact key behind the 0x38c build; the 0x50720
     identity + [obj+0xc6]/+0xf6). PR-155 Next: direct receiver
     calls with edx=9 (the +0x10 call-site walk), the kind-9
     object's fields in 0x3f0f0 + the kind-table writer, the
     last two live-set mask windows (0x28bc7/0x50ab7), the
     0x9d088 family - then the WAIT row-gate CODE pass (the
     engine law is now COMPLETE) [PR-155 OUTCOME: NO static
     edx=9 exists - the kind sits at [record+4] and the only
     static kind-9 poster is the BAL touch 0x3d3fb (PR-70
     confirmed); the mask windows are CLOSED (0x28af0 bit-54
     self-raise, 0x50ab7 = bit 21); the kind-9 +0x18 arm is
     rung-rect geometry; the TYPE-5 hand-off node is INERT;
     the generic kind-0x10 magic-record law found - the PR-155
     bullet below carries it]. Gates: docs-only; every added
     line <= 79; baselines unchanged (NEXT_STEPS 71, HANDOFF
     72); no game data in git; image unchanged. Full record:
     the PR-154 section of NEXT_STEPS.md (scratch /tmp/opencode/
     pr154/).
   - PR-155 (READING, no code, no new test - the PR-138..154
     precedent): THE BIT-54 BRIDGE, PASS 2 - THE STATIC RAISER
     SET IS BOUNDED AND THE GENERIC SIGNAL-DELIVERY LAW IS
     FOUND. WHY: PR-154's threads (a)-(d). F1 THE DIRECT-RECEI-
     VER WALK, DONE TWICE: the first pass read edx before each
     of the 64 `call DWORD PTR [reg+0x10]` sites - WRONG, the
     kind sits AT [record+4]; re-walk: static kinds are 1/8/0xe
     or dynamic, and EXACTLY ONE kind-9: 0x3d3fb->0x3d418 in-
     side the BAL touch method 0x3d370 - PR-70's balloon mes-
     sage byte-confirmed as the ONLY static type-9 poster. F2
     THE RECEIVER CHAIN DECODED WHOLE: 0x50d60 (entry 0x50d62)
     swallows kind 1 when [obj+0x126]!=0, jump-tables 9..21,
     and passes EVERYTHING ELSE to 0x54560 (entry 0x54562):
     kind 2 sets the [obj+0xc2]/+0xc6 pair, 3/4 OR [obj+0xb6],
     6 copies facing [rec+8]->[obj+0x6d], 0x1b splices the
     +0xa9 list, and kinds 5/7../0xd/else go to 0x53770 (entry
     0x53772) whose default tail is 0x52bb0 (kind 1 = position
     delta via [[pos+8]+0x54], 0xe = ATTACH [obj+0x30]=pay-
     load, 0x21 = pump-on, else RET) - THE HAND-OFF'S TYPE-5
     NODE IS INERT (a wake-up, not a data message); the ZARIA
     receiver 0x1e130's own kind-5..11 arm calls the payload's
     +0 slot with edx=2, stamps the demolist (ds:0xa94f4++,
     0x44900 - PR-154's sink gains named producers) and runs
     0x52cc0(self,9): the hit pipeline = +0x30/+0x24/+0x28
     verbs then 0x56600 -> kind-object ds:0xcf0ac[9] -> the
     companion's +0x18 (LADDER 0x3f0f0 = RUNG-RECT geometry -
     NEVER a live set) then [self+0x20]=1 - the hand-off ter-
     minates in geometry+pump, not signal. F3 THE LAW: kind
     0x10 + magic token 0x8138764 at [rec+8] + bit index at
     [rec+0xc] -> one-bit set ORed into [self+0x60] - ANY re-
     ceiver reachable through 0x53770 can be handed ANY signal
     bit, no HP guard (PR-144 item 3 CLOSED: full-HP K_GEN1 is
     reachable). F4 THE FIRST STATIC kind-0x10 POSTER: method
     0x287a0 (MOVEPLAT-family main+0x9FC) tests the assoc's
     record array [[assoc]+0x50][i]+4 == 6 (the SAME constant-
     6 field gate as the authored rows - the engine's own
     cross-check of PR-152's model!) then sends {0,0x10,
     0x8138764,0xa} = AC_DRAB to the assoc object; 0x55760 is
     NOT a signal builder but the [[obj+0x40][i]] assoc GETTER
     (PR-154's mislabel corrected); sibling 0x282e0 ORs bit 56
     into [[obj+0x30]+0x40][0]+0x60. F5 THE 31-BRANCH PLACE-
     MENT FACTORY (0x4d5d0..0x4ec20): tokens+allocs+pairs for
     ANITAB..TRASA2 - SURPRISE pair 0x97d94 (PR-149), TRASA2
     EMBEDS THE LADDER PAIR 0x97cdc/0x97cd4, BAL pair 0xa17dc
     = PR-70's vtable (method validated); ZARIA's data places
     SURPRISE x2 + BAL x6; 'MOVEPLAT' x3 in ANIMACJE (attribu-
     tion open). F6 The PRNG raiser class = TLewEna2 ('ena2ed.
     mod', 'E2BOSS/ECOIN/EBECZKA') main+0x3b4; its [obj+0x124]
     target = 0x4dcc0(level, authored position) = THE OBJECT AT
     THAT POSITION; ZARIA's own tables arm +0x120/+0x124 the
     same way (0x1ed90/0x1edb0 - the armer 0x1eee0 calls firs-
     t; [obj+0x124] carries pointers AND small ints). F7 mask
     windows CLOSED: 0x28bc7 = bit 54 self-raise inside MOVE-
     PLAT method 0x28af0 (after an inert type-0xf wake-up
     post), 0x50ab7 = bit 21 under [obj+0x11a]==8. THE BOUNDED
     STATIC K_GEN1 SET: ENA +0x100 blocks, MOVEPLAT 0x28af0,
     the HP<100 kind-9 arm (fed only by BAL), the TLewEna2
     PRNG, the kind-0x10 law. DECIDING QUESTION NEXT: does
     ANY code write ds:0xcf0ac/0xcf0b4 - if not, every 0x56600
     kind-dispatch is a no-op and kind-0x10 is the ONLY cross-
     object signal path (the 0x567xx family touches the BSS
     neighbours). 10 new open items counted; PR-144 items (2
     remainder)/(3)/(4) CLOSED; the TYPE-5 verdict and the
     record+4 method rule are the pass's reusable laws. Gates:
     docs-only; every added line <= 79; baselines unchanged
     (NEXT_STEPS 71, HANDOFF 72); no game data in git; image
     unchanged. Full record: the PR-155 section of NEXT_STEPS.
     md (scratch /tmp/opencode/pr155/).
     [PR-156 OUTCOME: the deciding question is ANSWERED -
     nothing ever writes the kind tables, kind-dispatch is
     DEAD CODE; the 0x28af0/0x287a0 guards are READ (the
     platform K_GEN1 law, the row-array law x4); ZARIA.01
     places ZERO MOVEPLAT - the bridge candidates there are
     DRABINKA/LINA self-raising and the kind-0x10 law - the
     PR-156 bullet below carries it].
   - PR-156 (READING, no code, no new test - the PR-138..155
     precedent): kind-DISPATCH IS PROVEN DEAD CODE; THE ROW-
     ARRAY LAW IS ENGINE TRUTH; THE ZARIA-ERA PLACEMENT CHAIN
     AND ROSTER ARE OUT. WHY: PR-155's deciding question +
     the MOVEPLAT bodies + the ANIMACJE attribution. F1 A di-
     word census over the WHOLE image for every immediate in
     0xcf000..0xcf100: ds:0xcf0ac/0xcf0b4 (the kind tables)
     occur ONLY as the two reads inside 0x56600 - nothing ever
     writes them, so the count stays 0 and EVERY kind-dispatch
     (both hand-off kind-9 arms, the ladder +0x18 0x3f0f0)
     NEVER RUNS; the PR-152/153 chain links (4)-(6) are VOID
     as behavior; live signal paths: kind-0x10 records, queue
     types, self-raisers, the pair share. (The other 0xcf0xx:
     0xcf000 init-written via 0x44350; 0xcf068/74 = the 56-
     site .MOD module registration list ('looser.mod' first);
     0xcf088 = stage-index byte.) F2 THE PLATFORM K_GEN1 LAW
     (0x28af2 whole): state [[ctrl]+0x5c]==4, lion present (
     [[obj+8]+0x3c]), lion rect INSIDE the platform rect minus
     a (20,0) reach constant -> type-0xf wake-up post to the
     LION + byte6|0x40 (bit 54) into the CONTROLLER's live
     set. F3 THE ROW-ARRAY LAW (0x287a0 whole): [[assoc]+0x5
     0][[[assoc]+0x5c]*4]+4 == 6 -> kind-4 message (payload
     |=4 -> the literal [obj+0xb6]|=4 OR at 0x545c6 - PR-151
     lead); state 0x12 with field <= 0x19 -> the kind-0x10
     AC_DRAB record; with 0x1edb0/0x1eee0 this is the FOURTH
     independent engine reading of state-indexed row arrays by
     field +4 - PR-152's model stands [PR-157 F3: the +4 word
     is the state record's ROW CURSOR - the law stands as a
     ROW-INDEX gate; PR-152's field reading corrected]. F4
     THE SECOND PLACE-
     MENT CHAIN (0x1ed00..0x20000, 23 branches) places DRA-
     BINKA (0xd9, stores the LADDER pair at 0x1f94d), ZARIA's
     own controllers (TLewZaria 0x96960, TLewZariaB 0x97a98/
     0x97a90 - the ledger's 0x97970 attribution CORRECTED:
     0x97970 is the ctor-0x1e0a0 swap-variant only), plus RYBA/
     WODA/ZARROW/CROCODIL/TREE... F4b: in the first chain the
     TABLICA branch (token 0x4e89f) stores the LADDER pair at
     0x4e8c4 then OVERWRITES it with 0xa1dfc/0xa1df4 at con-
     struction (method note: token-vs-store positions - the
     LINA/TABLICA attribution was corrected in-pass) [CORREC-
     TED AGAIN by PR-157 F6: the branch IS LINA's (token
     0x4e822) writing its OWN final pair 0xa1dfc/0xa1df4;
     TABLICA starts at 0x4e8e4]; 'LINA'
     (0xd9, base-ctor 0x531f0, main 0x97f50) SHARES size+base
     ctor with DRABINKA - the character embeds the same lad-
     der block. F5 ANI2.DES IS PLAINTEXT (corrects PR-152's
     assumption for this file): the full ZARIA.01 roster of
     35 objects - DRABINKA x1 + LINA x2 are the ONLY entries
     with extra PARAM LINES (`23,30;53,160`...), BAL x6 (co-
     ordinates exactly as PR-70 read - cross-validated), POCIS
     K x17, WODA/ZARROW/GRABG/RECT/KILOF_PL/ANITAB/KORONA -
     and ZERO MOVEPLAT: the F2 platform law is NOT ZARIA's
     bridge; ZARIA's bridge candidates are the DRABINKA/LINA
     ladder machinery itself. 7 new open items (row-array bui
     lder + [obj+0x5c] state writers - the row-gate SPEC;
     states 4/0x12 identity; param-lines landing format; the
     0xa1dfc swap class; the 0x28bf0 arm; 0xcf068 module list
     contents; ZARROW/SEP2 semantics). CLOSED: PR-155's deci-
     ding question (kind-dispatch DEAD) + items (1)/(3), the
     0x28af0/0x287a0 guards, the LINA-family item. Gates:
     docs-only; every added line <= 79; baselines unchanged
     (NEXT_STEPS 71, HANDOFF 72); no game data in git (autho-
     red EXCERPTS quoted only); image unchanged. Full record:
     the PR-156 section of NEXT_STEPS.md (scratch /tmp/openge
     de/pr156/).
   - PR-157 (READING, no code, no new test - the PR-138..156
     precedent): THE ROW-STATE MACHINE IS BUILT AT PLACEMENT
     AND THE ROW-GATE WORD IS A ROW CURSOR (PR-152 ITEM 7
     CLOSED). WHY: PR-156's row-gate SPEC questions. F1 THE
     +0x7C CLASS SLOT (0x537f0) IS THE STATE-MACHINE CTOR -
     present on EVERY base-family class (141 slots; direct ta-
     ble reads confirm TLewZaria/TLewZariaB/LINA 0xa1dfc/LAD-
     DER 0x97cdc/BAL 0xa17dc all carry +0x7C=0x537f0, +0x2c=
     0x53d80); base-ctor 0x531f2 leaves [obj+0x50]=0 and sets
     [obj+0x5c]=1 (PR-70's start state, named); 0x537f0 reads
     the placement tail with the class format ' %[^,], %[^,],
     %d,%d' via the custom scanf 0x49920, registers the object's
     own PARAMETER ENTRIES, then BUILDS the rows: count from
     the entry list, alloc via 0x76a64, per-entry KEY from the
     name (' %[^[\r
] ' + ' [%[^]]]' grammar) looked up in the
     ds:0xceff8 record registry (0x31f20) - rows[i] := the au-
     thored state RECORD, bound BY NAME, consumed as bound.
     F2 THE REGISTRY IS FILLED BY 'seq' KEY HANDLING in the
     main-.des chain ('sample'/'time_scale'/'seq' - the 's'
     branch reads a NAME and inserts) - 'seq' = ZERO in every
     extracted file: ZARIA's sequences are in the encrypted
     main data (the next big decode project). F3 THE GATE WORD
     [state record+4] IS THE ROW CURSOR: 0x53520 composes the
     transition key from the object's own LIVE SET, walks the
     current state's 21-byte transition table at [rec+0x1a]
     (0x52000 - entry +0xd = STATE INDEX, +0x11 = ROW INDEX,
     PR-101's law now grounded), 0x535a2 applies: same state =
     `mov [rec+4],rowB` + verb +0xac; different = EXIT verb,
     ENTER verb, [obj+0x5c] := recA (the ONLY state-index wri-
     ter), + a step counter [obj+0x9d]. All gate consumers
     (0x287ea==6, 0x2881d<=0x19, 0x1edb0 0/6, 0x28af2 state-4)
     test CURSOR VALUES, not authored fields. F4 THE "6 COIN-
     CIDENCE" RESOLVES: A_WALK's rows at INDEX 6/7 ARE the
     `4,6,GEN AC_TIME`/`5,6,...` poses (cycle 10,11,12,1,..) -
     the threshold-6 reading (PR-100) and the row index 6 ha-
     ppen to match; PR-152's static "holds the value 6" claim
     is superseded. F5 THE PARAM-STREAM LAW: the ANI2.DES
     param lines ARE the object's own stream read through the
     [obj+0x1] getter by the scanf family (0x49870/0x49920/
     0x49950 - ' %1[;]' is the ';' list stepper; ENA2's ' pos
     = '/' dist = %d' reads use the SAME mechanism). F6 COR-
     RECTION: the 0x4e8c4-d9 ladder-pair-then-overwrite stores
     are in the LINA branch (token 0x4e822, ret 0x4e8e0), NOT
     TABLICA (starts 0x4e8e4, alloc 0x40) - LINA's final pair
     = 0xa1dfc/0xa1df4 (PR-156's label corrected; the restore
     question CLOSED: no restore needed); DRABINKA's final =
     LADDER pair via [obj+0xc5]-0xbd stores. F7 the [obj+0x5c]
     writer census: 1 initial, 55 stamp, 0x82 clamp, 0x300dd
     resets a state's ROW CURSOR - the "reset row 1" idiom
     [RE-IDENTIFIED by PR-159 F1: 0x30083-0x300f8 is the
     CLONE/Ctor path; its tail is new-object INITIALIZATION
     (index 1, rows[1] cursor 0), not a mid-game restart;
     and no row-advancement writer exists at all - the com-
     plete writer law is a transition match (0x53616) or
     that initialization (0x300e6).]
     8 new open items (the +0x7C invoker, the 'seq' source,
     param-line token mapping, 0x524xx/0x525xx + [obj+0x58]
     dual role, initial cursor, 0x446de, 0x57d30 modes, con-
     sumption semantics). CLOSED: PR-152 item 7, the [rec+4]
     identity question (cursor for state records), PR-156 F4's
     label + its 0xa1dfc item, the "states 4/0x12" framing.
     NEXT: the WAIT row-gate CODE pass is FULLY SPECCED (cursor
     + row-targeted transitions + gate tests; ZARIA.01 output
     must not change) - then the encrypted-pool 'seq' decode.
     Gates: docs-only; every added line <= 79; baselines un-
     changed (NEXT_STEPS 71, HANDOFF 72); no game data in git;
     image unchanged. Full record: the PR-157 section of
     NEXT_STEPS.md (scratch /tmp/opencode/pr157/).
   - PR-158 (LEDGER RECOVERY + READING; ported/ll_level.c
     edits COMMENT/DIAGNOSTIC-STRING ONLY, no new test - no
     behavior change): THE MISSING PR-150/PR-151 RECORDS ARE
     RESTORED VERBATIM - a per-revision census shows each
     survived ONLY in its own commit blob (c947e90 / 0081b1b;
     swallowed by the Next-swaps of the PR-151/PR-152 eras;
     later restores re-spliced from a HEAD that had already
     lost them; the HANDOFF bullets survived), with correction
     brackets at the superseded claims only. THE LADDER GATE,
     RE-READ (0x3f1c2..0x3f24d): ecx = SELF (0x3f1c6) and
     0x3f1dc reads [ecx+0x5c] DIRECTLY - by PR-157's law a
     STATE/rows-array index - so the gate is a STATE test:
     1 = the initial state (first bound record, A_UP's role)
     FLAG branch, answers NOTHING; 2 = the state entered by
     `A_UP -> A_DROP` (K_GEN1-gated) the ONLY answering state;
     >= 3 inert (incl. the 55 stamp); 0 = zeroed fields - NOT
     a row-position test: `( N )` lands in the state record's
     cursor [[rec]+4] (0x53616). PR-150's getter formula, its
     frame-selector proof, the PR-86/110 row-start law and
     ALL ZARIA.01 legs STAND under both readings. The ledger,
     the HANDOFF and the port's OWN comment carry the cor-
     rection (the PR-150 block re-worded with KEEP-fi-GATE
     legs, the 0x54b6a guard re-termed a STATE guard, the
     "unmodelled WAIT law" sentence retired, three e_case
     labels fi-worded). THE ROW-GATE CODE PASS RESOLVES TO
     NO-CODE BY INVENTORY: the port already models the cursor
     (o->fi, set at ll_level.c:835 = the (N)-start law), has
     row-targeted transitions (ll_mov_tr.entry) and row
     thresholds; every cross-object cursor reader belongs to
     a class never instantiated at T5 (PR-156 F5). PR-150
     item (6) CLOSED as named+modelled+consumer-blocked. NEW
     OPEN: the row-ADVANCEMENT writer of [[rec]+4] - whose
     row clock and 0/1-BASED nature; if 1-based (the (N)-
     verbatim copy + "0 = before any admit" + 0x300dd's zero-
     reset all point that way), PR-157 F4's consumer-6 row
     identity shifts to the `3,6` line (the numeric gate-6/
     threshold-6 coincidence unaffected) - PR-159's first
     read [CLOSED by PR-159 F1: no incrementer exists; base
     = authored 1-BASED proven; consumer 6 IS the `3,6` line;
     0x300dd is clone/ctor-init, not a restart]; the ladder-
     gate fi-vs-state decision waits for
     DRABINKA's binding order (record-pool decode); the
     FLAG-branch +0x4c getter. PROCESS RULE: Next-swaps re-
     splice records from the LAST revision that carried them
     (census recipe in the record). Gates: full build gates
     anyway - make/make win ZERO warnings, ll_level relinked,
     verify-all 56 / 10+23 both modes, diff verified comment/
     string-only; every added .md line <= 79; baselines UN-
     CHANGED (71/72); image unchanged. Full record: the
     PR-158 section of NEXT_STEPS.md (scratch /tmp/opencode/
     pr158/).
   - PR-159 (READING, no code, no new test - the PR-138..158
     precedent): THE ROW CURSOR'S COMPLETE WRITER LAW IS
     CLOSED - [[rec]+4] has EXACTLY TWO writers and NO incre-
     menter: a transition match writes it verbatim (0x53616;
     0x53612 skips the write + the +0xac verb when unchanged),
     or the CLONE/ctor path initializes it (0x300d7-0x300e6 -
     the window PR-157 F7 called a "state restart" idiom is
     actually object CREATION: alloc + 0x54030 ctor + field
     copies + class pair 0x9c05c/0x9c054, a base-family table
     with the +0x2c/+0x7c pair). Row progress inside a state
     runs entirely in the threshold/accumulator entry-walk
     (PR-100/101, re-ground verbatim: [rec-class]+0x18 iter-
     ates entries by pointer chain, reading [entry+4] class,
     [cls+0xc] set-OR into the key, threshold vs [obj+0xa1])
     and NEVER touches the cursor [NOT a chain and no loop -
     PR-161 F2 re-read the walk: single-shot per tick via the
     object-verb +0x44 at 0x53b18; and no [rec+0x4] increment
     form exists anywhere in the image]. THEREFORE the cursor's base
     IS the authored 1-based (N) (verbatim copy; 0 = the init
     value) - PR-157 F4's consumer-6 row resolves to the
     `3,6` line; the gate-6/threshold-6 numeric coincidence is
     untouched. PR-158 item 2 and its F4 question CLOSED; F7's
     idiom label corrected in ledger/HANDOFF. NEW ENGINE LAW:
     the same-state arm SELF-RAISES AC_TIME - every match
     (even write-skipped) increments [obj+0x9d] (zeroed at
     state change/ctor/list-parent 0x535c2/0x538c3/0x52077),
     and WORD [current-state-record+0xc] != 0 && <= count ORs
     live-set bit 9 (0x53626-0x5365c; PR-145 bit law +
     ll_play.h cross-check). Not a timer: SELF-TRANSITION
     MATCHES since state entry, vs a record word COMPUTED by
     0x52072 as an interpolation of WORDs +0x8/+0xa (authoring
     source = open; a WAIT-style always-matching entry would
     make it an effective step-timer - the port's PR-69 frame-
     list approximation was directionally right) [CLOSED by
     PR-160: word = a RANDOM roll in the authored time=min,
     max pair, taken at ENTER (0x76bcc = Borland rand LCG);
     no time= line => never raises - and ZARIA.01 authors no
     time= at all, so no leg moves; the exact law is now re-
     placeable port-side, specced in Next]. BINDING
     HELPER 0x53480: record via class verb +0x84 (fallback
     +0x88), store index via 0x53440 = name scan of the ob-
     ject's own [obj+0x3c] param-entry list, INC BEFORE com-
     pare -> 1-BASED, 0 = not found; with (count+1)*4 zeroed
     alloc: rows[0]=NULL, [obj+0x5c]=1 = first bound param
     entry - THE STATE-INDEX SPACE PROVEN 1-BASED (PR-158
     F2's gate needs no unit correction; the fi-vs-state de-
     cision is now PURELY data-blocked on the param-token
     map). RECORDS HAVE OWNER BACK-POINTERS AT +0 (0x52072
     zeroes [[self]+0x9d]; the walk reads [entry+0]->[owner+8]
     at 0x52311) - and an entry-kind tension stays OPEN: the
     walk's threshold DWORD [entry+0x10] vs the 0x15-stride
     transition layout's +0xd/+0x11 out-slots - resolved only
     by the unread [cls+0x18] body (PR-160's first read); the
     [[owner]+8]+0xa0] call right before the threshold read
     DROPS its result (candidate: side-effect computes the
     threshold). [obj+0x9d] consumer census: 11 sites - and
     the PR-152 hand-off "scan" is NOT a loop: 0x50bd4-0x50be4
     compares the CURRENT record's cursor [record+4] ONCE
     against -M (the direct consumer proving the cursor holds
     authored (N)s), plus a missed step (0): 0x50b9b tests
     steps==1 then calls +0x114. 5 new open items counted + 1
     narrowed (ladder-gate no longer base-blocked); PR-158
     item 2 closed. ll_level.c edits are COMMENT-ONLY (the
     0x300dd "state restart" clause corrected; the AC_TIME
     approximation comment annotated with the real self-
     raiser law); full gates anyway: ZERO-warning builds, ll_
     level relinked, verify-all 56 / 10+23 both modes; added
     .md lines <= 79; baselines UNCHANGED (71/72); image un-
     changed. Full record: the PR-159 section of NEXT_STEPS.
     md (scratch /tmp/opencode/pr159/).
   - PR-160 (READING, no code, no new test - the PR-138..159
     precedent): THE STATE-BLOCK DIRECTIVE PARSER DECODED END
     TO END - 0x51910, a record-class verb (+0x38; driver
     0x52640): time= writes the authored min/max WORDs at
     [rec+0x8]/+0xa ('%hi', ',%hi'); sample= -> [rec+0x16];
     time_scale= -> [rec+0xe]; 'seq NAME' REGISTERS the block
     into the global pool ds:0xceff8 (via the [[ds:0xcef40]+
     0x398] singleton's +0x4 verb - PR-157's seq-registration
     mechanism FOUND); 's NAME,N' looks a name up in that
     pool and appends a 0x18-byte row entry (ctor 0x515e0,
     class 0xa2638 - a WRAPPER forwarding to the payload's
     0xa1000 class) to the rows vector at [rec+0x2e];
     control= enumerates 21 flags (BY_X..ANTI_POCISK, exact
     strings) with a Polish error naming the source FILE.
     AC_TIME FULLY CLOSES: ENTER (record-class +0x1c, base
     0x52360 -> 0x52072) zeroes the owner's [obj+0x9d] AND
     rolls [rec+0xc] = min+(max-min)*rand()/32767 where
     0x76bcc is the Borland LCG rand (0x41c64e6d/0x3039/>>16
     &0x7fff) - a random match-count threshold per state
     entry; no time= line => gate 0 => never raises. ZARIA.01
     authored texts have ZERO time= lines (ENA-era ones show
     time=10,20/30,60/50,100 and a single-value time=350
     form) - every ZARIA.01 leg unchanged; the port's PR-69
     approximation is now replaceable by the exact law (Next:
     CODE pass fully specced). RECORD MODEL: [rec+0x44] =
     record-class table (base 0xa25d4; LADDER family 0x97c9c
     inside the LADDER object-table blob, +0x40 view offset);
     factories = object-class +0x88 (base 0x55360 alloc+ctor
     0x524d0; LADDER 0x534c0 alloc 0x48 + ctor 0x51770, ctor
     init: owner back-pointer, cursor 0, time_scale DEFAULT
     1000 = 1x, flags/sample 0); +0x84 = NULL stub for both
     families (records built fresh, not name-fetched). The
     base ctor region's 0x52500 copies the F/T/G header from
     the PARAM ENTRY (PR-157 F7's "contradictory copy block"
     resolved). ROWS MODEL: [[rec+0x2e]+[rec+4]*4] indexed by
     the ROW CURSOR (PR-159's "pure set-position register"
     refined); walk 0x521f0 = keyed-merge/0x47e40 gate OR
     per-row threshold gate [entry+0x10] vs [obj+0xa1] (for-
     mer PR-100/101 re-verified, hold-keeps-debt); the
     dropped-result [[owner]+8+0xa0] call is 0x543a0 = the
     frame-advance mode dispatcher feeding [obj+0xa1] (PR-100
     mechanism NAMED; PR-159's "computes [entry+0x10]" candi-
     date rejected); helper +0x18 = 0x520c0 runs [entry-cls+
     0x4] and re-sets the accumulator (0x52118 = PR-100's
     cite ✓); after entries: [[owner]+8+0xa4] + [owner+0x95]
     = 1 (PR-142's latch ✓). 3 new open items (row-line par-
     ser for plain rows - the gap PR-157's 'seq' hunt implie-
     d; walk-arm attribution given 0xa2638's +0x18 metadata;
     derived ENTER overrides + single-value time= case) + 2
     narrowed (+0x84/+0x88 partial, cursor-phrasing); CLOSED:
     PR-159 item 2 fully, item 3 half (semantics; WAIT-key
     open), F4's pre-scale, PR-157 F1's pool-binding mechan-
     ism (seq/s). Gates: docs-only, nothing compiled (no ll_
     level.c touch); added lines <= 79; baselines UNCHANGED
     (71/72); image unchanged; ledger census run (HEAD = last
     carrier). Full record: the PR-160 section of NEXT_STEPS.
     md (scratch /tmp/opencode/pr160/).
   - PR-161 (READING, no code, no new test - the PR-138..160
     precedent): THE ROW-LINE PARSER IS FOUND AND THE ROW MO-
     DEL CLOSES END TO END. The state-block parse driver is
     0x52b80 (param-entry verb +0x14 = 0x51eb0, then a +0x00
     edx=2 completion pass if [entry+0x28]). The 0x51eb0 loop
     tokenizes every line with ' %[^[=,( ]' and dispatches:
     numeric first token (0x57db0 = sscanf "%ld") -> THE ROW
     LINE PATH: row lines are ID,THRESHOLD pairs - the ID is
     resolved with strtol (0x57fd0; the isdigit routine 0x76fa3
     is a class-table member at 0xa68b4) through the pool
     singleton's result slot ds:0xceff4, and an 0x18-byte WRAP-
     PER entry (ctor 0x515e0, class 0xa2638: +0x0 payload, +4
     class, +8/+c reserved, +0x10 threshold, +0x14; the ctor's
     0x3e8 arg = the record's CURRENT time_scale default) is
     appended to the rows vector [rec+0x2e] (count +0x36).
     PR-160's 's NAME,N' was the NAMED variant of the SAME
     builder. Non-numeric lines -> record verb +0x00 (the F/T/
     G handler 0x52590 delegating to the 0x51910 directive
     parser). THE WALK IS SINGLE-SHOT PER TICK: called via the
     OBJECT-CLASS +0x44 from the tick at 0x53b18, its body has
     NO LOOP, and an IMAGE-WIDE census finds NO increment form
     for [rec+0x4] anywhere - so rows advance ONLY by authored
     transition row-outputs (0x53616), exactly PR-159's writer
     law, now with the negative proof [SUPERSEDED by PR-162:
     the grep idiom missed the lea+store step in the helper's
     tail (0x520c0 continues at 0x5211e; step at 0x521d4);
     ordinary row progression is advance-on-threshold-pass -
     the port's model corroborated; PR-107's old comment was
     right all along]. THE WAIT LAW: the key
     test is an INTERSECTION test (0x495b0 over a 13-byte dword-
     wise OR merge, 0x49580); the wrapper verb +0xc (0x45870)
     returns a ZERO-FILLED key when the payload is NULL - an
     unresolvable row ID is ALWAYS an always-passing entry, so
     its threshold gates the mode-dispatcher-fed accumulator
     every tick and same-state matches keep counting toward the
     AC_TIME roll. PR-159 item-(3) and PR-160 items (1)/(2)
     CLOSED. THE 21-BYTE LAYOUT TENSION DISSOLVES: rows are
     pointer slots at [rec+0x2e], transitions are 0x15-byte
     nodes at [rec+0x1a] - two distinct arrays; and the class
     tables live at TWO VIEWS (0xa25d4 record view / 0xa260c
     entry view at -0x28: +0x14 = this row builder, +0x28 =
     the record factory 0x518f0 which stores view-2 class at
     0x51851) - every earlier slot-arith discrepancy came from
     mixing views. ZARIA.01 grounding: its rows (1,0,WAIT /
     2,500 / ...) register no pool names (only 'seq=drab' re-
     gistrations exist), so their payloads are NULL there -
     the port's threshold-gated row model stands corroborated
     end-to-end IF the pool holds no numeric-ID keys (item 2:
     the pool read is now the named edge of the K_GEN1 data
     blocker). BONUS LAW: the placement ctor runs ENTER on the
     initial record at 0x539fb - the AC_TIME rand roll fires
     AT SPAWN for state 1 (feeds the specced CODE pass). Also
     corrected: PR-159/160's "entries iterated by pointer
     chain" phrasing - no chain exists; the no-intersection
     path re-dispatches the same entry's verb. 4 new open
     items (walk return-value encoding + the [rec+0c] 13B view
     block; pool contents/registrations; transition-tail parse
     owner; completion-pass + payload-class 0xa1000 semantics)
     + closures. Gates: docs-only, nothing compiled; added
     lines <= 79; baselines UNCHANGED (71/72); image un-
     changed; ledger census run (HEAD = last carrier). Full
     record: the PR-161 section of NEXT_STEPS.md (scratch
     /tmp/opencode/pr161/).
   - PR-162 (READING + LEDGER CORRECTION, no code - the PR-138.
     .161 precedent): THE ROW CURSOR DOES STEP - PR-161'S NE-
     GATIVE CENSUS GREPPED THE WRONG IDIOM. Grounding the AC_
     TIME code pass against the port re-opened PR-107's conti-
     nuing comment ("the row stepper 0x520c0 advances [rec+4]
     (0x521d7) and wraps at the last row (0x521c5)") - and
     the bytes PROVE IT: the helper verb 0x520c0 CONTINUES at
     0x5211e (no prologue) into a row-step tail: a mode-6 que-
     ry through entry verb +0x10 (0x458e0 forwarder) can SKIP
     the step (0x52140-0x52145, exhausted-sequence semantics
     open); forward = lea+store cursor+1 (0x521d4-0x521d7);
     wrap at last-row-to-0 ORs value 4 into DWORD [[owner]+0]
     (object vtable slot +0x4) at 0x521c1-0x521c5; reverse =
     wrap 0-to-last with |= 8, or cursor-1 (0x5219a-0x521b1),
     gated by flag bits 0x10/0x20 paired with position senti-
     nels [[owner]+8]+0x70]/+0x71] == -1 (0x52158/0x52171).
     THE CORRECTED LAW: the transition applier still SETS the
     cursor only from authored row-outputs (PR-159 UNCHANGED),
     but ORDINARY row progression is the STEPWISE advance in
     the helper, reached ONLY on a threshold PASS (the walk's
     paid path calls record verb +0x18 at 0x52338) - ADVANCE-
     ON-THRESHOLD-PASS, i.e. the port's model (obj_row_tick)
     CORROBORATED end to end; PR-100/101's measured law is
     exactly right. Also corrected: PR-161's arm story - BOTH
     arms reach the gate (the intersection path only does key-
     merge bookkeeping; conditions never hold a row inert at
     this level), so every row's threshold gates payment+step-
     ping in practice (the practical WAIT-law conclusion stands).
     SECOND MAJOR CLOSED: the COMPLETE control= flag-word map,
     chain-traced arm-by-arm through 0x76a69 (je = skip own
     store): flag window [rec+0x12..+0x15]; BY_X = NO FLAG (ac-
     cepted-and-ignored); BY_Y=0x1, BY_TIME=0x2, BY_TX=0x3
     (composite), BY_TY=0x4, FRAME_REV=0x8, SEQ_REV_X=0x10,
     SEQ_REV_Y=0x20, CHECK_ANI=0x40, CHECK_ZW=0x80 (+0x12);
     CHECK_LN=0x1, CHECK_ALL=WORD 0x1c0, CHECK_LEW=0x2, CHECK_
     BASE_LN=0x40, SOLID=0x4, CH_DIR=0x8, INV_V=0x10, DANGER=
     0x20, CHECK_ALL_PLANE=0x80 (all +0x13); ANTI_SOLID=+
     0x14|0x1, ANTI_POCISK=+0x14|0x8; composites SOLID2=0x204
     00, SEMI_SOLID=0x40400 (an unnamed +0x14 bit 0x4 sits in
     side both composites - flagged). SE-
     Q_REV_X/Y = 0x10/0x20 CONFIRMS PR-95's behavioral obser-
     vation exactly; PR-160's "[+0x15] bit 0x8 = face-flip" is
     RE-ATTRIBUTED (that bit = ANTI_POCISK; PR-108's flip BE-
     HAVIOR stands, its bit attribution reopened - CH_DIR and
     the reversed-step path are the leads). 5 new open items
     incl. THE MECHANISM BOUNDARY (where is the transition ap-
     plier 0x535a2 invoked from - the port's model depends on
     it) and the port-enum-vs-engine-bit mapping; 2 old closes.
     AC_TIME code-pass spec UNCHANGED and next up. Gates: docs-
     only, nothing compiled; added lines <= 79; baselines UN-
     CHANGED (71/72); image unchanged; ledger census run (HEAD
     = last carrier). Full record: the PR-162 section of NEXT_
     STEPS.md (scratch /tmp/opencode/pr162/).
   - PR-163 (CODE + TESTS): THE AC_TIME LAW IS SHIPPED BE-
     HAVIOUR. The engine law PR-159/160/162 closed now runs
     in the port: time_lo/time_hi parse (single values pair
     to lo==hi, the engine scanf's own shape), obj_enter rolls
     gate = lo+(hi-lo)*rand/32767 with the Borland LCG verba-
     tim at EVERY state-change site - a LAZY placement ENTER
     (the ctor ENTER 0x539e6/0x539fb), the transition change
     arm (0x535c2), the death force, the A_ATAK force; obj_-
     transition's same-state arm (the engine's match arm at
     0x53626-0x5365c) counts matches and raises bit 9 exactly
     when gate && count >= gate; no time= = never. UNIFIED
     ENTER STATED IN CODE: record-table +0x1c (0x52360) is
     ONE body - PR-107's row-set half AND PR-160's gate half
     - and the port splits it exactly where the applier splits
     it (ll_mov_row_start + obj_enter); the carried ll_level.c
     comments are FIXED per PR-162 (row step 0x521d4, wrap+
     OR-4 0x521c1-0x521c5, SEQ_REV_X/Y reverse, brk || labeled
     as the remaining approximation, raise additive). Five-leg
     obj_actime_selftest, all hand-derived: the fresh-object
     LCG stream (0, 21468) pins the rolls ((2,50) -> 2; the
     single-value deterministic gate; lerp(1,10) at 21468 =
     exactly 6); the state change ITSELF proves the raise (the
     consuming line tested at brk=0); the death force clears a
     live gate+count. MUTATION BATTERY: 11 mutants, 9 KILLED
     naming their legs; 2 SURVIVORS STATED: cross-match count-
     ing (unobservable BY CONSTRUCTION - the change ENTER zero-
     es) and the A_ATAK-force ENTER (unreachable with current
     content). ZARIA.01 output: UNCHANGED - zero time= lines;
     proved both ways: verify-all WITH fixtures 56 (ok) and
     fixture-free 10 (ok) + the same 23 SKIPs, all rc=0; make
     + make win ZERO warnings; ll_level force-relinked. New
     open items 6 (headline: in-walk visibility of the raise
     = PR-162's mechanism boundary = PR-164's read; per-object
     vs the engine's single 0x76bc6 LCG state, documented). CI
     note: PR-160/161's runs had been cancelled by toolchain-
     install hangs - gh run rerun issued; PR-162's run is CONF-
     IRMED GREEN. Full record: the PR-163 section of NEXT_
     STEPS.md (scratch /tmp/opencode/pr163/).
   - PR-164 (READING + LEDGER CORRECTION, no code - the PR-
     138..163 precedent): THE MECHANISM BOUNDARY IS CLOSED,
     AND THE CLOSE CORRECTS THE LEDGER. PR-162 item 5 ("the
     applier's driver is unpinned") REGRESSED PR-105's own re-
     cord: the state-step chain was CLOSED end to end by PR-
     105/106 - class-table slot +0x14 (two views: [[obj+8]+
     0x14], [[obj+0xc]+0xc]) chains through wrappers (LION
     BASE 0x550a0 = `zero [obj+0xe6],[obj+0xea]; call
     0x536f0`; DEFAULT WRAPPER 0x400b0; per-class wrappers)
     into ONE pump method 0x536f0 whose body tonight's inde-
     pendent byte read matches line for line: key getter +0x8c
     + merge 0x49580 -> WALK 0x523f0 at 0x5371c on the CURRENT
     record [[obj+0x50]+[obj+0x5c]*4] (= the object's LIVE
     STATE: vector + state index, the engine's o->sti) -> key
     merge -> hook +0x54 -> LOOKUP 0x53520 (13-byte live-key
     copy, PR-102's 0x52000 table walk, [[obj+8]+0x90] writes
     target-row+row args) -> APPLY 0x535a0 at 0x5374b -> hook
     +0x28. PR-106's pump (0x4bb30 via 0x4c8a0) stands. NEW
     FACT 1: THE APPLIER'S ENTRY IS 0x535a0, NOT the objdump-
     listing 0x535a2 - prologue 51 56 57 55 at 0x535a0-0x535a3
     (four pushes), own returns pop four (0x535fd-0x53601,
     0x5365c-0x53660) - the one-byte trap class again (0x543a0,
     0x52360); interior addresses unaffected; the three port
     comments citing 0x535a2 ride the NEXT code PR. NEW FACT 2
     (the big one): the applier's ARMS CONFIRM PR-163'S SHIP-
     PED LAW VERBATIM - cross path zeroes [obj+0x9d] WITHOUT
     counting (so M10 is now ENGINE-WRONG, not just unobserv-
     able) then NEW record +0x1c (unified ENTER+row-set, re-
     confirming PR-163 F1) then [obj+0x5c]:=idx = the state
     change; same-index path: authored row == current row SKIP-
     PS the set but STILL reaches the count arm (0x53614 je ->
     0x53626: WAIT self-matches count every match); count arm
     = [obj+0x9d]+=1; gate=WORD [rec+0xc] sign-extended; skip
     iff gate==0 or gate>count; ELSE [obj+0x61] |= 2 = LIVE-
     SET BIT 9 - the exact bit LL_SIG_AC_TIME(9) and the exact
     `gate && count>=gate` predicate. Port-model: pump order =
     walk-then-apply vs the port's apply-then-walk - the only
     two observable differences (same-step vs next-step row-end
     via the vtable+0-word OR 4/8 whose consumer is unknown;
     in-call raise visibility) are exactly PR-163 open item (1)
     NARROWED, neither observable in shipped content - the
     port's shape stands (it matches PR-100/101's measured
     laws). 4 new open items (vtable+0-word consumer; 0x53520
     arg plumbing; [obj+0x95] reader; 31-vs-24 +0x14-slot delta);
     PR-162 item 5 CLOSED with a bracket in its record. ZARIA.01
     output: UNCHANGED (docs-only, nothing compiled). Gates:
     added lines <= 79; baselines UNCHANGED (71/72); image un-
     changed; ledger census run (HEAD 2e43bc1 = last carrier).
     CI: PR-160's RERUN GREEN. Full record: the PR-164 section
     of NEXT_STEPS.md (scratch /tmp/opencode/pr164/).
   - PR-165 (READING + LEDGER CORRECTION, no code - the PR-
     138..164 precedent): THE FLAG-CONSUMER PASS. THE
     BYTE-TEST CENSUS OF [rec+0x12..+0x15] IS EXHAUSTIVE BUT
     ITS SCOPE WAS THE WRONG IDIOM - the BY_* bits' consumer
     is the FEED DISPATCHER ITSELF: the record step calls
     owner-slot +0xa0 (0x543a0) at 0x52317; its body re-
     derives the current record ([[obj+0x50]+[obj+0x5c]*4]),
     loads DWORD [rec+0x12], and, and 7 + a five-entry jump
     table 0x54380: mode 0 = NO FLAG = BY_X -> [obj+0xa1] +=
     abs([obj+0xca]) (PR-98's |v_x| law VERBATIM at the
     field level, horizontal speed field confirmed); mode 1 =
     BY_Y -> abs([obj+0xce]) (vertical twin, NEW - the port
     has no BY_Y feed); mode 2 = BY_TIME -> a [[obj+0xa1]+
     0xc] dereference + jle into a full sub-record feed
     (semantics open, headline item); mode 3 = BY_TX = the
     composite exactly as PR-162's map said, guarded by
     [obj+0xba]; mode 4 = BY_TY -> [obj+0xa1] += pool scalar
     [ds:0xcef40+0x388] (a runtime-set constant rate, static
     0) guarded by [obj+0xbe]; mode >=5 (incl. FRAME_REV bit
     3) = NO feed. FRAME_REV itself: zero consumers in any
     form; DANGER/CH_DIR: no consumers (port-derived labels
     stand as such); the byte-test hits are exactly SEQ_REV_X
     /Y (0x52158/0x52171 + 0x52385/0x523a0 inside ENTER -
     third corroboration) and CHECK_LEW/ANI/ZW inside the key
     builder. SECOND CLOSE: THE KEY BUILDER = RECORD SLOT
     +0x08 = 0x52750 (verbs +0x2c/+0x30 + owner getter +0xc8
     + tail verb +0x34 = 0x52790) - PR-162 item 4 CLOSED.
     0x52790 = FLAG-DRIVEN KEY MERGE: CHECK_LEW/ANI/ZW each
     run a POOL QUERY (0x4bd30/0x4b890/0x4b870 over pools
     ds:0xcef40/0xa94ec/0xcef44, with a 0x8000 CHECK_ALL_
     PLANE sub-mode arg) ORed into the 13-byte key (count
     byte at +0xc) - CHECK_* rows ADD KEY BITS, they do not
     gate. 0x4b890's listing start misaligned again (0x4b88f
     = push ecx) - fifth member of the one-byte trap class.
     THIRD CLOSE: DERIVED ENTER OVERRIDES ARE WRAPPERS:
     0x4fef0 (table 0x97c9c +0x1c) = pool prelude + TAIL
     CALL of base 0x52360; 0x50040 (0x9a894) = base call +
     slot +0xfa tag; 0x50060 wraps LEAVE - the base ENTER/
     LEAVE bodies run IN FULL, the port's single obj_enter
     core stands for derived classes. FOURTH: THE POOLS HAVE
     A STARTUP REGISTRY (0x4fe02: 0x5c320(0x14) + 0x51120
     with class ids 2,9,8,10,11 + obfuscated name strings at
     0x9209a.. - decode open) feeding exactly the globals
     F3's queries use. 6 new open items (headline: the
     BY_TIME/BY_TX/BY_TY arm semantics - the [[obj+0xa1]+
     0xc] deref and the [obj+0xba]/[obj+0xbe] guards touch
     the ladder-gate block; pool name decode; 0x4b500 body).
     ANSWERED: PR-162 item 1 (bracketed - BY_* bits select
     the feed mode exactly as the port models, NO port change
     indicated; the enum stays port-internal) and item 4's
     0x52750 body (bracketed). ZARIA.01 output: UNCHANGED
     (docs-only). Gates: added lines <= 79; baselines held
     (71/72); image unchanged; ledger census run (HEAD
     0509ce9 = last carrier). Full record: the PR-165 section
     of NEXT_STEPS.md (scratch /tmp/opencode/pr165/).
   - PR-166 (READING + LEDGER CORRECTION, no code - the PR-
     138..165 precedent): THE POOL CONTENTS ARE READ - THEY
     ARE THE LEVEL'S OWN PLACED OBJECTS. The pools the CHECK_
     * queries hit ATTACH TO THE CONTAINERS AT LOAD BY THEIR
     PR-60 SOURCE NAMES: the level loader (0x4cd6c-0x4cdca)
     case-insensitively compares the container name against
     'zwierzaki'/'animacje'/'ani_plat' (0x91dda/0x91de4/
     0x91ded) and stores the container's pool INDEX field
     [container+0x40] into ds:0xcef44/0xa94ec/0xa94e8 - the
     CHECK_ZW/ANI pool handles PR-165 found. The container IS
     the pool array base: the container's own cleanup (0x4ce0a
     -0x4ce24) uses the identical layout ([[c]+0x40]*4+c+0x58
     chain heads), and ds:0xcef40 at query time = the current
     container. QUERY BODIES CLOSED (PR-165 item 3): 0x4b500
     = walk chain heads at [poolObj+0x58] ([poolObj+0x44]
     count), entry predicate verb +0x60, CLASS FILTER [entry+
     0x24]==[owner+0x24] OR the pushed 0x8000 CHECK_ALL_PLANE
     switch (CONFIRMED semantics), verb +0x78 BUILDS the en-
     try's 13-byte key, OR-merged via 0x49580; 0x4bd30
     (CHECK_LEW) instead walks the OWNER's own list at [obj+
     0x64] and INTERSECTS with the object key (0x495b0) and
     COPIES the matching entry key out, no OR. The wrappers
     0x4b870/0x4b88f are stack-arg adapters (entry 0x4b88f -
     sixth one-byte misalignment). CLASS ID LOCATION CONFIRMED
     AT [obj+0x24]. THE STARTUP REGISTRY FULLY TRANSCRIBED:
     14 registrations (type 0x14 + 0x51120), class ids 6,6,
     11,(11),7,3,5,12,2,9,8,10,11,(11) (two inherit), names
     0x9201c-0x920e4, handles ds:0xcefbc..0xceff0 - and a
     NEGATIVE: NONE of those handle globals is read anywhere;
     the queried handles are the load-time ones. NAME DECODE
     STILL OPEN with negatives banked: no single-byte XOR key
     works (256-key scan), pairs position-independent ('XtXoTm'
     prefix shared by two names), and the block sits EXACTLY
     after the readable LEVELS.DES keys + factory-name table
     ('ANITAB'...'BERLO') in .rodata. NAME-KEYED ACCESS IS
     REAL: 0x4dcc0(ds:0xcef40, name) tokenizes (' %[^,
     \r\n] , ') and calls 0x4df10 (its head special-cases
     'ANITAB', creates type 0xa5); the factory clone path
     stores the result at [obj+0x120] (PR-104's prototype
     cache); the row resolver's result slot ds:0xceff4 is the
     very next global after the registry's last - ds:0xcef40.
     .0xceff4 is ONE pool-singleton block: PR-161's sequence
     pool and today's container pools are ONE subsystem. ZARIA
     .01 rows still resolve NULL - the K_GEN1 data block
     STANDS, now precisely narrowed (key-path pools need no
     names at all). SIDE: pool manager singleton at [contai-
     ner+0x394] (verbs +0xd8 register/+0xe4), BY_TY rate at
     [container+0x388] (setter unread), 0x4b600 = the ani_
     plat pool position/velocity scan (0x4b6ba; callers 0x546
     d7/0x54719) = platform-ground lead; 0x749a0 = the 13-byt
     e-local memset. 6 new items (adder side + the +0x60/+0x7
     8 entry class = PR-167's headline; name decode; 0x4dcc0
     contract; 0x4b600; ds:0xcef58/what writes ds:0xcef40; p
     ool-manager slots). 3 brackets in the PR-165 record (F3
     argument roles - LEW is OWN-LIST INTERSECT not pool-OR;
     F5 class-id sample corrected to the full sequence; item
     3 CLOSED). ZARIA.01 output: UNCHANGED (docs-only).
     Gates: added lines <= 79; baselines held (71/72); image
     unchanged; ledger census run (HEAD 1c22089 = last carr
     ier). Full record: the PR-166 section of NEXT_STEPS.md
     (scratch /tmp/opencode/pr166/).
   - PR-167 (READING + LEDGER CORRECTION, no code - the PR-
     138..166 precedent): THE POOL ENTRIES ARE THE GAME OB-
     JECTS THEMSELVES - the last leg of the CHECK_* key law
     closes at the CLASS-VERB level. (1) ONE TABLE CARRIES THE
     GENERIC VERB FAMILY AT THE EXACT PREDICTED OFFSETS: the
     base object class at 0xa2550 (71 slots) - +0x08 class-id
     getter 0x23e00, +0x14 the walk, +0x24 midpoint-distance
     helper via the manager, +0x60 the predicate, +0x70/+0x71
     the position getters, +0x9c the key gate, +0xa0/+0xd8 =
     ENTER 0x52360, +0xe4 = base LEAVE 0x52470; every value
     PR-162..166 derived from call sites sits exactly where
     the arithmetic said; the 0x23bxx bodies are one-instruction
     getters/setters (0x23b80 = `return [eax+0x2c]`, 0x23b90 =
     the class-id setter). Caution kept: the base table's own
     +0x1c/+0x20 are 0x52c70/0x52a00, NOT the base ENTER/
     LEAVE (open). (2) THE WALK'S VERBS ARE THE CLASS TABLE'S
     OWN SLOTS: 130/155 class tables hold 0x23b80 at +0x60 -
     membership = [obj+0x2c]; +0x78 = a three-variant key-
     provider family (default 0x552e0: clear 13-byte local,
     0x53ae0 gate, class verb +0x9c gate, 0x55230 builder,
     emit EXACTLY 12+count bytes; 0x3df00 x23, 0x50f80 x15).
     Entries are the objects, linked at [obj+0x34] (store site
     unread = the one open half). (3) THE ADDER IS THE PER-
     CLASS VERB +0xd8 CALLED FROM FACTORY CHAINS AT LOAD
     (0x249b0..0x3055c census): no-op 0x40f90 in dozens of
     classes, real bodies (0x3def0/0x507e0/0x3d6c0) in the
     pooling ones, and 0x3df00 PROPAGATES registration to
     related objects - PR-60's placement-category model at the
     verb level. (4) THE BASE ENTER BODY FIELD-BY-FIELD:
     [obj+0xa1] (the FEED ACCUMULATOR) ZEROED at ENTER - PR-
     163's counter-clear gains its engine twin for the feed;
     [obj+0x99] := 1 (the writer of the carried sentinel);
     the SEQ_REV arms + BOTH cursor stores to the row vector
     ((row+8)-edx reversed / edx plain) - PR-162's row-cursor
     law CONFIRMED inside ENTER. (5) The +0x1c=0x52360 census
     = exactly 4 tables: PR-100's two named views + 0xa2e9c/
     0xa2ed4 (already in PR-100's 16-table census - the non-
     factored-view gap narrows to selection-code). 5 new items
     (chain-link store; 0x53ae0/+0x9c internals = where the
     0x600000/0x800000 CHECK_LN constants must appear; the
     base +0x1c override; the 25-class +0x60 minority; [obj+
     0x99] readers). CLOSED with brackets: PR-166 item 1; its
     F3 bracket gains entries-are-objects. ZARIA.01 output:
     UNCHANGED (docs-only). Gates: added lines <= 79; baselines
     held (71/72); image unchanged; ledger census run (HEAD
     c00c1ce = last carrier). Full record: the PR-167 section
     of NEXT_STEPS.md (scratch /tmp/opencode/pr167/).
   - PR-168 (READING + LEDGER CORRECTION, no code - the PR-
     138..167 precedent): THE FEED LAW IS CLOSED END TO END -
     AND THE THREE "DIFFERENT" BY_* ARMS ARE ONE ARMS. Raw-
     byte dumps at the five jump-table targets: modes 2/3/4
     (BY_TIME/BY_TX/BY_TY) are BYTE-IDENTICAL feeds of the
     pool scalar [container+0x388] into the accumulator [obj+
     0xa1], the last two merely PRE-GUARDED by `cmp [obj+0xba
     or 0xbe],0; je`; the famous "[obj+0xa1]+0xc dereference,
     cmp +0xc, jle" NEVER EXISTED - a one-byte objdump split
     of `mov eax,[eax+0x388]` (SEVENTH member of the listing
     trap class: 0x543a0, 0x52360, 0x535a0, 0x4b890, 0x4bd30,
     0x4b88f, now 0x543fb). THE RATE IS A LITERAL 100/TICK:
     the ONLY writer of [x+0x388] is 0x4c8b7 `mov DWORD [x+
     0x388],0x64` at container init. THE GUARDS ARE THE STATE
     ITSELF: [obj+0xba]/[obj+0xbe] are the authored MOTION
     PARAMETERS (PR-98's physics fields) - 0x549f0 loads them
     as param block [obj+0xd2] words 0/1; the only other
     writers are the param CLAMP family (0x54a57..0x54b47,
     literal 10/-10/0 under comparisons against the param
     block) and zeroing at 0x54e2e/0x54e38. So BY_TX/BY_TY
     = the constant rate feed WHILE the authored motion para-
     meters are live - not a third speed source. THE CONSUMER
     SIDE RE-CONFIRMED: [obj+0xa1] is read by the step at
     0x52323 and written by the row-step helper at 0x52118 -
     PR-98/162's accumulator model IS the engine's (feeder =
     the dispatcher, zeroer = ENTER per PR-167 F4, consumer =
     the row step). KEY-BIT MECHANISM FOUND: the second main
     key-provider family +0x78 = 0x54860 (99 class tables,
     paired 1:1 with the +0x9c gate 0x549f0) contains the
     body where THE PARAM SIGN SELECTS KEY BITS: `cmp [obj+
     0xbe],0; jge -> or BYTE [key+4],0x20 / jle -> or 0x40`
     (plus a 32-bucket bitmap loop, full map open) - and
     0x54a24 compares [paramblk+0xc] against [obj+0x6d] the
     FACING: the flip-bit question gets a named lead, and the
     port's DANGER/CH_DIR-labelled behaviours now have their
     engine counterpart named (param-sign key bits, not flag
     bits). NO port change indicated; one narrow audit item
     opened (does the port's BY_TIME row-gating match the
     constant-100 feed - its fixtures literally have '1,100/
     2,100' rows). 4 new items (BY_TIME audit; full 0x54860
     key-bit map = the last place the 0x600000/0x800000
     CHECK_LN constants can live; param-clamp family context;
     +0x9c per-class gate variants). CLOSED with brackets:
     PR-165 item (1) (its F2 arm text corrected in place) and
     PR-166's carried mention. ZARIA.01 output: UNCHANGED
     (docs-only). Gates: added lines <= 79; baselines held
     (71/72); image unchanged; ledger census run (HEAD
     f7d7552 = last carrier). Full record: the PR-168 section
     of NEXT_STEPS.md (scratch /tmp/opencode/pr168/).
   - PR-169 (READING + LEDGER CORRECTION, no code - the PR-
     138..168 precedent): THE MEMBERSHIP PREDICATE GETS ITS
     PRODUCER AND THE CHAIN-INSERT HUNT IS RE-SCOPED. (1) THE
     PREDICATE FIELD IS AN ACCESSOR PRODUCT: 0x23b70 (`mov
     ebx,eax; mov ecx,[obj+8]; call [ecx+0x8]; mov [obj+0x2c]
     ,eax`) CALLS the class verb +0x08 and stores its product
     at [obj+0x2c] - the exact field the 130-copy predicate
     0x23b80 reads: membership is a NULL-TEST of a COMPUTED
     handle, not a state flag (PR-167 F2a corrected with a
     bracket). The base +0x08 verb 0x23e00 is a BOUNDS-CONTAIN
     MENT compare of [obj+0]/+4/+8 - re-confirming PR-162's
     rectangle identity. (2) THE CHAIN-INSERT CENSUS IS EXHAST
     EDIVE AND NEGATIVE for displacement stores: the only +0x58
     store is the container's heads-array alloc (0x4cdf8); +0x34
     stores are constructors (+0x34=3), non-object struct use
     (WORD coord arithmetic at 0x1427a, data-table walks at
     0x1579c) and class-table installs ([eax-0x34]=TABLE at
     [obj+0x64]). Yet the chains ARE walked and provably al-
     located. THE ENGINE'S ANSWER IS VERB DISPATCH: 0x53db0
     calls a neighbour's verb +0x14 with edx=2 (that class's
     = LEAVE 0x52a00) and relinks [x+0x54] - list maintenance
     goes through manager/class verbs via computed pointers.
     Re-scoped lead PR-170: read the manager's method table at
     [[ds:0xcef40]+0x394]+0x8 and the 0x5c320 pool-object
     class. (3) THE +0x60 MINORITY MAPPED (PR-167 item 4
     CLOSED): of the 25 non-generic predicates, TWELVE are
     constant-true `mov eax,1; ret` (0x26310/0x26330/0x26350/
     0x260b0/0x26000/0x26110/0x26e10/0x30720/0x30950/0x3baa0/
     0x2fb00/0x22d10 - class FAMILIES that are always match-
     able without being pooled), the rest conditional (unread,
     carried). (4) The 0x507e0 +0xd8 body decoded for one
     family: a 13-way jump managing a GLOBAL counter ds:0xa94f0
     clamped 1..9 - counts, not chains: +0xd8 registration and
     chain linking are DIFFERENT mechanisms (the 14 startup
     type-0x14 pools are name-registered DATA pools, PR-161's
     family). 3 new items + 1 re-scoped (manager verb census;
     [obj+0x2c] producer contract; the conditional minority).
     1 bracket in PR-167's record (F2a product, not flag; item
     (1) re-scoped). ZARIA.01 output: UNCHANGED (docs-only).
     Gates: added lines <= 79; baselines held (71/72); image
     unchanged; ledger census run (HEAD 5e123e1 = last carr
     ier). Full record: the PR-169 section of NEXT_STEPS.md
     (scratch /tmp/opencode/pr169/).
   - PR-170 (READING + LEDGER CORRECTION, no code - the PR-
     138..169 precedent): THE POOL MANAGER IS THE LEVEL CON-
     TROLLER ITSELF. [container+0x394] is written by the con-
     tainer's OWN construction (0x4de22-0x4dea4: nine sequen-
     tial `call <level ctor>; test; mov [edi+0x394],eax` -
     one per PR-104 level family: 0x1fef0/0x25510/0x269a0/
     0x29b00/...): the TLewZaria ctor 0x1fef0 compares the
     name ('TLewZaria' at 0x9078b), calls 0x5c320(0x12e) +
     0x1e0a0 (shared base ctor) and installs the controller's
     TWO class tables ([+8]=0x96960, [+c]=0x96958; the Ena
     variant adds [obj+0xf6]=0x2710). The manager's "verbs"
     are ordinary class verbs of a game object. SECOND: THE
     +0x68 HANDLE GETTER IS GENERIC FIELD ACCESS - 0x23900 =
     `add eax,0xd6; ret`, a POINTER to the object's own +0xd6
     pair, which the controller's ctor reset block (0x54e02,
     right after base 0x52c70) fills as a POSITION SNAPSHOT
     (copy of [obj+0]/+4); the same block initializes the
     whole motion state (+0xa5/+0xba/+0xbe/+0xc2/+0xca/+0xce
     zero, +0xad := 100, +0xb1 := 22 - PR-98's default phy-
     sics pair!). Pinned generics too: 0x23920 = current-
     record FLAG getter ([[obj+0x50]+[obj+0x5c]*4]+0x12) and
     0x238f0 = the BOOLEAN membership writer (test/setne/mov
     [obj+0x2c]) - PR-169's predicate field has TWO producer
     shapes (computed handle OR 0/1 flag). THIRD: THE JOIN
     PATH IS A HANDSHAKE, NOT A LIST INSERT: controller se-
     cond-table +0xd8 = 0x50ed0 (guards: [obj+0x11a] in {6,7}
     or [obj+0x126] set refuse; both participants' verb +0x40
     must pass; reads the participant's verb +0xc -> [ctl+
     0x10a] and +0x70 position -> [ctl+0x10e]) then calls the
     GAME-CLASS verb +0xe4 (default 0x50300: sets a 30-TICK
     field [obj+0x102], read as a busy guard by 0x502f0) and
     0x54550 - registration = handshake + bounded active win-
     dow, confirming PR-169's "registration counts, it does
     not chain". FOURTH: THE WALK LOOP FULLY VERBATIM (0x4b552
     -0x4b5cc: count +0x44, head slot [[x]+0x58], NEXT [entry
     +0x34], predicate +0x60, class filter [entry+0x24] vs
     [[rec+0]+0x24] with the [esp+0x54] CHECK_ALL_PLANE swit-
     ch, self-skip vs the owner, key provider +0x78 + OR-
     merge) - PR-166 F1's law stands EXACTLY; the chain-store
     hunt is RE-POINTED (not closed) at the verb +0x10 family
     (0x54ed6 passes a 4-dword block with 0/0x1b) and the
     shared ctor 0x1e0a0. 2 new items (+0xd6/+0x102 lifecy-
     cles - the +0x102 timeout looks like contact work) + 1
     re-pointed; 1 bracket (PR-169's manager-table sentence
     corrected). ZARIA.01 output: UNCHANGED (docs-only).
     Gates: added lines <= 79; baselines held (71/72); image
     unchanged; ledger census run (HEAD 643f13a = last carr-
     ier). Full record: the PR-170 section of NEXT_STEPS.md
     (scratch /tmp/opencode/pr170/).
   - PR-171 (READING + LEDGER CORRECTION, no code - the PR-
     138..170 precedent): THE KEY-BIT MAP IS COMPLETE AND A
     TREASURE OF ROLES FALLS. (1) THE BUILDER ENTRY IS 0x54912
     (ninth member of the one-byte listing trap: PR-168's
     "0x54900" was interior - proven at byte level: padding +
     pop/ret at 0x54905, fresh prologue at 0x54912). Full
     bit map: [obj+0xa5] state-flag bits 0x1/0x10/0x20 ->
     key[+4]|0x80 / key[+5]|0x01 / key[+6]|0x20; if [obj+
     0xba] != 0: `test BYTE [obj+0xb6],0x10; sete al; add
     al,0x23` -> set bit (35 or 36 mod 32) of the key dword
     at (idx/32)*4 = key[+4]|0x08 or |0x10 (the "32-bucket
     bitmap loop" is this ONE-index idiom); the param-sign
     bits (PR-168 F3) run ONLY on the [obj+0xba]==0 path ->
     key+4|0x20/|0x40 (both when zero: key+4 = 0x60). (2)
     0x54860 (the 99-table +0x78 value) IS NOT A PROVIDER -
     a key-CONSUMING counter-decay verb: reads the caller's
     key bytes, and when [key+2]&4 gates and [key]&0x10 +
     record-flag&0x10, decays [obj+0xc6] by -2*[obj+0xc6]/3
     (idiv 3 at 0x5489f); other key bits reset [obj+0xc2]/
     [obj+0xc6]. Those counters are SET/ADDed/ZEROED by the
     CONTROL-MESSAGE verb +0x10 = 0x54560 (87/155 tables;
     switch [block+4]: id<=5 sets facing [obj+0x6d]/[obj+
     0x71]; arms set/add/zero the counters, OR status bits
     [obj+0xb6]|4/8) - the loop closes: messages load, the
     key verb decays. WHO BUILDS KEYS FOR THOSE 99 CLASSES:
     0x54912 is referenced NOWHERE (triple census: no table
     slot, no call, no branch) - new named mystery + top
     lead. (3) THE +0x10 LEAD OF PR-170 IS DEAD BUT PAID:
     the 0x54eb2 caller sends msg {0,0x1b,0,0} to SELF then
     a guarded RE-ANCHOR: pos := arg - [[obj+0xd6],[obj+
     0xd6+4]] + [[obj+0x89],[obj+0xda]] - the snapshot pair
     is the anchor origin (ride law; PR-170 item (2) first
     half answered); [obj+0xb6] = a named STATUS-BYTE. (4)
     THE CLAMP FAMILY = A DEADZONE POSITION CONTROLLER (PR-
     168 item 3 closed): [pb+4] mode (1=x,4=y), threshold
     [pb+0xc] vs facing setne*15 (the flip-bit test 0x54a24
     NAMED), snapshot dword +-5 window vs authored param ->
     force 10/-10/0 per axis; flag bit-4 update law at
     0x54bc1. (5) DECISIVE NEGATIVE: 0x600000/0x800000 occur
     ONLY as byte-table misalignment artifacts (0xa8a86 char
     map, 0x7c404/0x7c42d shift patterns, 0xa9aaa byte pair
     table) - NO code immediate uses them: PR-161's CHECK_LN/
     BASE_LN constants need re-derivation (labeled hypothesis:
     they are key-BYTE sequences 0x60/0x80 in a hex reading -
     exactly F1's bits). (6) THE LOAD POOL PATH ENDS EMPTY:
     heads array alloc'd via 0x76a64 + stored at [poolObj+
     0x58] (0x4cdf5-0x4cdf8 - an awk range bug had hidden it
     from earlier passes), zero-filled, [poolObj+0x44] :=
     count; [c+0x40] increments PER NAME GROUP (slot counter,
     PR-166 F2 refined); insertion is runtime-only; chain-
     store item stays open (third re-point: +0x10 minority
     bodies, unread msg arms, the family-B producer hunt).
     0x52070 read + RULED OUT (ENTER preamble, not an inse-
     rt). 2 new items (family-B producer/reachability; msg-
     id map + [obj+0x102] reader); CLOSED: PR-168 items (2)
     (map complete) and (3) (deadzone law); PR-170 item (2)
     half. 5 brackets. ZARIA.01 output: UNCHANGED (docs-
     only). Gates: added lines <= 79; baselines held (71/
     72); image unchanged; ledger census run (HEAD 0ce1036 =
     last carrier). Full record: the PR-171 section of
     NEXT_STEPS.md (scratch /tmp/opencode/pr171/).
   - PR-172 (CODE PR + TEST): THE BY_TIME AUDIT (PR-168 item
     1) FOUND A REAL PORT DIVERGENCE AND FIXED IT. The port's
     feed (100/iteration, 0x4c8b7), hold-gate and subtract-
     carry ARE the engine's law; what was missing is the
     register's zeroing at transitions: the applier's state-
     change arm (entry 0x535a0) calls the NEW record's ENTER at
     [[rec+0x44]+0x1c] (the ZARIA class tables install the
     wrapper 0x4fef0 there - 0x97c9c/0x98e7c/0x99338/0x9a85c
     re-cited, variants stand - and its body CALLS the row-set
     method 0x52360 at 0x4ff6b, which zeroes [obj+0xa1] at
     0x5236b and sets [obj+0x99] := 1: ROW WRITE AND ACCUMU-
     LATOR ZEROING ARE ONE METHOD, always run on entry). The
     same-state match arm writes rec+0x4 INLINE (0x53616) and
     calls no ENTER - the carry SURVIVES matches. OBSERVABI-
     LITY CENSUS (authored texts, scratch): plain-BY_TIME's
     1128 ZARIA.01 rows carry 12 distinct gates and SEVEN
     (1/40/75/80/120/150/225) are not multiples of 100 - a
     mid-row exit leaves a non-zero remainder and the new
     state's first row would open an iteration early. The
     port zeroed facc but NOT row_t at the applier arms:
     `o->row_t = 0;` shipped at BOTH (obj_transition + the
     forced A_DEAD in obj_hit), engine law cited at each site;
     the two-register split stays observationally identical to
     the one register (fresh at entry + mode-fixed feed within
     a state). NEW TEST obj_rowzero_selftest: gates 70/240/
     120/150 (no multiples of 100), hand-derived timeline,
     legs = carry-within-state, same-state-match CARRY (and
     the next iteration feeding on from 90), transition zero-
     ing observable at the first row of 120, death-force zero-
     ing observable at 150; battery M1-M5 killed at their
     exact legs; ONE SURVIVOR stated (M6 zero row_rem too:
     row_rem is the port's ms-splitter, always 0 at an applier
     in this fixture, no engine counterpart - accepted un-
     pin). RIDE PAID: the three 0x535a2 comments -> 0x535a0
     (PR-164 item CLOSED). NEW ITEM: BY_TY feed unmodelled (4
     shipped states, port runs them as plain time - the 0x543a0
     mode arm unread). UPDATED: the PR-107 self-target row
     item (the object path now counts those lines but still
     never applies their row seek - small code PR candidate
     after the entry-token census); PR-170's "tail call" at
     0x4fef0 corrected to a call at 0x4ff6b. CLOSED with
     brackets: PR-168 item (1); the ride item. 5 brackets.
     ZARIA.01 output: pinned frame checks ALL pass unchanged.
     Gates: make/make win ZERO warnings, ll_level re-linked,
     verify-all 56 (ok) with fixtures / 10 (ok)+23 SKIP with-
     out, rc=0 both; baselines held (71/72); census run (HEAD
     2872f45 = last carrier). Full record: the PR-172 section
     of NEXT_STEPS.md (scratch /tmp/opencode/pr172/).
   - PR-173 (READING + LEDGER CORRECTION + ONE CODE COMMENT
     FIX): THE KEY-LITERAL RE-DERIVATION CLOSED THE HUNT AND
     UNIFIED THE KEY WITH THE SIGNAL SET. The 0x919ec name
     block dumped whole (bounds CORRECTED: 0x919ec..0x91c68,
     73 entries): 4 non-bit node-op specials + 68 signal names
     in the port enum's EXACT order + the 'AC_EV_MAX' sentinel;
     law bit N = index N+4; machine-verified 68/68 against the
     port LL_SIG_* enum AND the port's 68-pair token table
     (complete, zero mismatches). The 13-byte key = 3-dword
     TEventSet mask + count at +0xc (0x5533c/0x49580): row
     keys, condition masks, live set, built keys = ONE system.
     The pump 0x536f0 verbatim: class verb +0x8c = the self
     key getter, merged into [obj+0x60] BEFORE the 0x523f0
     chain merge — matching tests live ∪ getter ∪ chain. Slot
     censuses (178 marker tables): +0x8c = 0x54910 x115 (PR-
     171's 0x54912 = listing trap + wrong search value; tables
     store 0x54910 — entry ground truth lesson), base 0x52750
     = PR-165's CHECK_LEW/ANI/ZW pool-query merge slot-
     identified, 0x23a60 x38, 0x23a20 = &[obj+0x75] (a SECOND
     13-byte set!), 0x504e0 (TLewZaria, partial); the 99
     family-B tables pair 0x54860/+0x9c 99/99 and carry the
     BOOLEAN 0x23870 ([obj+0xa9]!=0) at +0x8c — two contracts,
     call-sites now the open half of PR-171's family-B item
     (re-scoped, producer hunt CLOSED). 0x54910 decoded:
     [obj+0xa5]->K_FIRE/K_ALT/K_JUMP, [obj+0xba]->K_BKWD/
     K_FWD, [obj+0xbe] sign->K_UP/K_DOWN — the engine
     synthesizes the input-key vocabulary from motion state.
     Strict .MOV token census: ZARIA.01's 118 K-input-family
     condition lines are ONLY in the two LEW (lion) scripts ->
     no ZARIA.01 object reaches a synthesized bit, NO port
     change (corpus: 578 lines / 16 files — every stage's LEW
     pair + KADMAR camel + KADMAR.07_0_0: future-stage
     reachability item, counted). 0x600000/0x800000 CLOSED as
     LEDGER ARTIFACTS (git-proven: zero hits in PR-161, first
     entry PR-166's Next, no derivation anywhere); the real
     flag->key bridge named: CHECK_LEW/ANI/ZW pool queries +
     ANTI_POCISK/DANGER tests in the 0x55230 relation builder
     (contact gate 0x53ae0, +0xd0 relation verbs, [x+0xad]
     ordinals, AC_ENEMY always ORed, AC_ENEMY_ATC on the
     ordinal branch); CHECK_LN/CHECK_BASE_LN occur in NO key
     path (negative stated). 5 new items (+0x8c call-sites;
     [obj+0xa9]/[0xad]/[0x75] readers; per-class +0x78 single-
     ton family incl. slot-identified 0x28af0 = bit-54 raiser;
     KADMAR reachability; 0x504e0/0x551f0). 5 brackets (PR-88
     bounds + verification upgraded; PR-171 F1 entry 0x54910 +
     reachability CLOSED; its hex-dump thesis SUPERSEDED; its
     F5 "PR-161" attribution corrected with git provenance;
     its family-B item re-scoped). ll_play.h comment fixed
     (block bounds + law). ZARIA.01 output unchanged (comment-
     only). Gates: make/make win ZERO warnings, ll_level re-
     linked, mov-selftest rc=0; baselines 71/72 held; census
     run (HEAD af3e3ff = last carrier, 23 records). Full
     record: the PR-173 section of NEXT_STEPS.md (scratch
     /tmp/opencode/pr173/).
   - PR-174 (CODE + TEST): THE SAME-STATE MATCH NOW WRITES
     THE AUTHORED START ROW in the object path, exactly as
     the engine's arm does (0x5360f-0x53616: read the current
     record's row counter, store only if it differs, then the
     0x53626 count arm; no ENTER, the register carried -
     PR-172). The CENSUS (C tool linking ported/ll_play.c,
     PR-107's method; /tmp/opencode/pr174/) reproduced PR-
     107's numbers EXACTLY - 2767 lines, self-target buckets
     92 absent + 24 "(1)" + 101 seeks (12 over-range) - and
     added the split PR-107 left open: LION 77 / SCENE 24.
     ZARIA.01: ALL 20 seek lines are the lion's own scripts
     (ZAR_LEW/ZAR_LEWPL: A_BORRING/A_DEAD/A_FALL_DOWN/
     A_FG_PACKA/A_FIGHT/A_JUMP/A_JUMP_S/A_KILOF), and NO
     ZARIA.01 scene .MOV has ANY self-target line - so the
     fix is provably unobservable here; the verify-all fix-
     tures (56 ok, byte-identical ZARIA.01 output) prove it
     from the other side; the fix is what makes the 24 KAD-
     MAR/KADMAR2/LONA lines behave at their stages. The im-
     plementation is the SHARED ll_mov_row_start rule (fourth
     call site; "(1)"/absent/over-long = the recorded PR-107
     model choices, already settled for this exact path).
     obj_rowseek_selftest (7 legs: seek lands live, empty
     token re-seeks from a non-top row, (1)-top, over-long
     clamp, row-only (register carried), cross-state regress-
     ion): mutation battery 6/7 KILLED (no-write, raw-store,
     skip-empty, no-clamp, gate-on-AC_TIME, zero-register);
     SURVIVOR STATED: the engine's literal equality guard -
     equivalent by definition; its only unmodelled half is
     class verb +0xac (NEW counted item: called on state
     change at 0x535f7 and only on changing same-state writes
     at 0x5361c-0x53620; body unread). The PLAYER-side half
     stays open WITH ITS FULL DATA (new counted item): the
     20 lion lines' conditions are AC_TIME/AC_DUP/|K_ANY/
     K_FIRE,K_GEN2/AC_RECT,K_GEN1 - the player path models
     none of those signal legs; PR-86's keep-running stays
     shipped until the signal-side model + fresh frames run.
     PR-107 item (2) CLOSED for the object path (bracketed,
     re-scoped to the player side); the PR-172 row-seek cand-
     idate CLOSED. 2 new items; 2 brackets (both closures).
     Gates: make/make win ZERO warnings, ll-
     _level re-linked, mov-selftest rc=0, verify-all rc=0
     BOTH ways (56 ok; 10 ok + 23 named SKIPs); added lines
     <= 79; baselines 71/72 held; census (HEAD carries PR-
     150..PR-173, last carrier). Full record: the PR-174
     section of NEXT_STEPS.md (scratch /tmp/opencode/pr174/).
   - PR-175 (READING; no port code, no behaviour change): THE
     KEY-FOLLOW-UP INTERLOCK PASS. +0x14 IS THE PER-OBJECT
     STATE-STEP VERB SLOT: over the 178 marker-anchored class
     tables it holds 0x550a0 x79 (zero [obj+0xe6]/[0xea] then
     `call 0x536f0` - 0x550b4 is the ONLY direct pump call in
     the text), 0x536f0 direct x26, 0x400b0 x45 (position
     path), 0x23c70 x18 (delegates to class verb +0x28),
     0x55380 x4, singletons; the pump runs for exactly 105/
     178 classes and LION (0x96960) is one of them - PR-153's
     "every LION tick" HOLDS. THE 99 "family-B" tables are a
     DISJOINT lineage: none carries the 0x23b80 marker (their
     +0x60 = 0x54450/0x40030/other getters) and all 99 hold
     0x23920 at +0x14 - "return the record's [rec+0x12]
     control-flags word" - the passive class lineages with
     the 0x23870 boolean at +0x8c: PR-173 F3's joint presen-
     tation CONFLATED two table populations (bracketed). The
     +0x8c slot has exactly TWO call sites (pump 0x536ff; the
     0x3ed00-0x3ee27 owner-merge path ending at 0x3ee18,
     which sets AC_BREAK_NEXT bit 2 in its built key) and
     DISCARDS the return value at both - the "TWO contracts"
     framing dissolves into the lineage separation. +0xac IS
     A KEY-EXTENSION HOOK, NOT A ROW VERB: called only at
     0x535f7 (state change, always) and 0x5361c-0x53620
     (same-state, only on changing writes), always with
     edx = &[obj+0x60] the LIVE KEY SET (ebx from 0x535ad) -
     PR-174 F2's "edx = the row value" CORRECTED (bracketed;
     its item (1) closed in part - the (step,+0xac) pairs
     co-vary exactly, (0x550a0,0x546a0)x79 etc., 0x23930 =
     empty `ret` for the 26 direct-pump classes; 0x546a0's
     head read: +0x12 word test ah,0x40 -> the 0x4b600 pool
     query (which reads [obj+0xa9] at 0x4b64f!) -> 0x49580
     live merge, [obj+0xa9] RMW; full body carried). [obj+
     +0xad] IS THE ENERGY FIELD - PR-173's "ordinal pair"
     corrected (bracketed): 0x5516a `cmp [self+0xad],[other+
     +0xad]` -> greater builds an AC_ENEMY_ATC key, jle zero-
     key with AC_ENEMY always set - the same field PR-89
     named. [obj+0xa9] site map COMPLETE (24 text sites: msg-
     id READ 0x5460e/WRITE 0x54624, the pool-query feed at
     0x4b64f, the animal trio, zeroers; semantics open).
     [obj+0x75] has ZERO direct accessors - only the 0x23a20
     pointer getter; NEW: 0x23a20 is also slot +0x4c's default
     (the carried FLAG-getter item, slot-identified; counts
     need a careful recount). TWELVE tables carry the 0x504e0
     getter with the FULL LION controller shape (+0x14=0x550a0,
     +0x54=0x55070) - the camel item (PR-173 (4)) gains that
     grounding, twelve-class identity = PR-176's named task.
     Pump-105 slot map recorded (+0x8c: 0x54910 x64/0x23a60
     x26/0x504e0 x12; the +0x54=0x55070 set is DISJOINT from
     the pump set - 49 each way). Docs: 7 brackets,
     0 port
     lines, baselines 71/72 held; census (HEAD carries PR-
     150..PR-174, 25 records, last carrier). Full record: the
     PR-175 section of NEXT_STEPS.md (scratch /tmp/opencode/
     pr175/).
   - PR-176 (READING; no port code, no behaviour change): THE
     CONTROLLER GETTER READ WHOLE. 0x504e0 - the +0x8c getter
     of exactly twelve classes - WRAPS the motion synth: gate
     (class verb +0xf8 && [obj+0xd2] != 0 && [obj+0xa5]&0x2)
     calls 0x551f0 (a queue-node consumer), then a ten-leg
     priority chain over ([obj+0xa5] 0x40/0x80, [obj+0xa6]
     0x01/0x02/0x40/0x80, [obj+0xa7] 0x01/0x02/0x04/0x08),
     each leg gated by global words ds:0xcef5c..0xcef80
     (stride 4, written only by stage-load ctors 0x1e0dc/
     0x2dfe9/0x3b1f2/0x50162), picks ordinal [obj+0xee] 0..9;
     a 12-entry jump table at 0x504b0 sets bit 41+k = K_F1..
     K_F12 - EXACTLY the port's LL_SIG_* enum, a new
     independent confirmation of the name block; the tail
     calls 0x54910 and merges both keys via 0x49580 into the
     caller's buffer - getter = signal key OR motion key,
     standard +0x8c contract. THE TWELVE IDENTIFIED by exact
     factory-chain attribution (name literal -> call 0x76a69
     -> match branch's first [eax+0x8] store): SIX TLew stage
     lions (TLewZaria -> 0x96960 = THE ZARIA-stage lion IS
     the controller class; TLewZariaB/TLewEna2/TLewKadmar2/
     TLewKara/TLewJunction) + FIVE death-module classes
     ("looser.mod"/A_DEAD -> 0x988ec + rec-class 0x98e7c;
     sadesdea/ran1/lonade/deatd .mod) + the SHARED PARENT
     0x97970 (base ctor 0x1e0a0). Bonus: PURCHAWA -> 0x9c05c
     (closes PR-173 item (e)); LEW_SWT -> 0x9ac90; LONAKULA
     -> 0x9e740; TLewTunga1 -> 0x9f668; TLewRan -> 0x9cfd0.
     CAMEL NARROWS: CAMEL -> 0x98278, a generic decor/mover
     class in NEITHER key lineage (third packed format) - the
     KADMAR.07_CAMEL K_* rows cannot come from the twelve
     controllers. 5 new open items, 2 closed; docs: 3
     brackets, 0 port lines, baselines 71/72 held; census
     (HEAD carries PR-150..PR-175, 26 records, last carrier).
     Next -> PR-177 = the +0x78 singleton family pass. Full
     record: PR-176 section of NEXT_STEPS.md (scratch /tmp/
     opencode/pr176/).
   - PR-177 (READING; no port code, no behaviour change):
     THE +0x78 SLOT IS THE RELATIONSHIP-KEY PROVIDER. Its
     call sites are EXACTLY FOUR (0x4b5be pool-walk-by-id
     fn 0x4b500; 0x4bda9+0x4be09 walk fn 0x4bd30 - called
     from the BASE +0x8c getter 0x52750 gated by
     [rec+0x12]&0x8000, NEW fact, with a global fallback
     via [mgr+0x394]; 0x55421 inside the child-aggregator
     0x553e0 = OR of children +0x78 at [obj+0x3c]/[obj+
     0x40]); a fifth consumer, the bounds test 0x4b430 (in
     LION +0xac 0x546a0), returns AC_BLOK (byte 0 bit 0)
     on an [obj+0x70]-array hit vs a ctx bounds rect.
     CENSUS CORRECTED (bracket): inside the 178, +0x78 has
     9 SHARED values x122 tables + 56 singletons (NOT
     "~76", and NOT the 99-passive-lineage - third time
     two populations were conflated; that item is CLOSED).
     ALL NINE shared providers read whole and classified:
     zero (0x23b10); relation merges onto PR-173's known
     bridge (0x552e0 x65: 0x53ae0 && +0x9c, fill 0x55230;
     0x3df00 x19 prefix +0xd8/0x45320; 0x3e500 x2 sets
     K_GEN1 INTO THE LIVE SET [obj+0x66]|=0x40 when
     record #1 + ctx gates pass); enemy/facing (0x50f80
     x15 = LION's own, 0x53c90, 0x3d990: +0xd0 verbs,
     0x55130 energy key, AC_ENEMY byte0|=0x80 ALWAYS and
     AC_ENEMY_ATC on the contact arm - PR-175's energy
     reading reproduced IN the providers); decor-rect
     (0x3d440: point-in-rect vs the 0x23a20 format, the
     CAMEL table 0x98278 HARDCODED at 0x3d49d/0x28b47,
     0x98274 at 0x4b441 - PR-176's camel bracket updated);
     child-aggregator (0x553e0); consumer (the 99's
     0x54860, PR-171's decay confirmed). The named lead
     singleton 0x28af0 whole: its table 0x9a36c is a full
     ACTIVE pump class; record-#4 gate, one-time 0x28bf0
     init via [obj+0xf6], rect test, then merges K_GEN1
     into [self+0x60]. ZARIA.01 GROUNDING, no port gap:
     the .MOVs gate on exactly these bits (AC_ENEMY_ATC/
     AC_BLOK in ZAR_LEW; K_GEN1/K_LEW/K_LN in KAMIEN/
     BALONIK/KATAPULT/KORONA) and the port already models
     those tokens - every shipped raise rule now has its
     engine counterpart NAMED. 5 new open items; 1 closed;
     docs: 3 brackets, 0 port lines, baselines 71/71 held;
     NEW OWNER GATE truth recorded (rule 8): make clean,
     parallel verify-all rc=0 = 52 (ok) fixtures / 10 (ok)
     + 18 named SKIPs fixture-free. Next -> PR-178 = the
     BY_TY feed pass. Full record: PR-177 section of
     NEXT_STEPS.md (scratch /tmp/opencode/pr177/).
   - PR-178 (READING; no port code, no behaviour change):
     THE BY_TY FEED IS THE SAME CONSTANT 100 BEHIND A
     `cmp [obj+0xbe],0` GATE - PR-172's feed-source item
     CLOSED, NO PORT GAP. The 4 shipped BY_TY states are
     exactly ZAR_LEW/ZAR_LEWPL [A_WSP]/[A_WSP_LIN] (lion
     climbs; no non-lion ZARIA.01 state uses BY_TY) - the
     port has modelled guarded-feed+constant-100 there
     since PR-148, and the object path's plain feed is
     unobserved by any authored BY_TY row (settled
     negative). The dispatcher 0x543a0 read byte-exact
     whole (entry 0x543a0 - PR-98 was right, the 0x543a1
     was a desync): [rec+0x12]&7 modes -> mode0 += |[obj+
     0xca]|, mode1 += |[obj+0xce]| where [obj+0xca/ce] =
     |[obj+0xc2/c6]|/100 (the walk-feed units PINNED =
     the port's facc law); modes 2/3 = the constant 100
     (mgr 0xcef40+0x388) behind the [obj+0xba] gate; mode
     4 = the SAME constant behind the [obj+0xbe] gate;
     modes >=5 no feed. +0xa0 census over the 178: 130
     dispatcher + 26 unguarded 0x239d0 feeders + 22 odd
     installs (incl. ONE false marker table, +0xa0 =
     0x255d000 - counted); ALL 15 LION tables install the
     dispatcher (the ZARIA.01 LION is table 0x96960). The
     gate producer chain traced one step up: 0x549f0 (one
     caller, inside getter 0x50340 at LION-table+0xcc)
     assigns +0xba/+0xbe from a 3-dword buffer (zeroed
     when [obj+0x106] = 0; dword 0 negated when [obj+
     0x11a] = 5) and re-decides them via the [obj+0xd2]
     attach object: kind 1 = the X window (PR-147 law,
     re-read), kind 4 = SIGN WINDOWS on BOTH axes (be =
     the y-window vs [d2]+0x1d - the climb gate's real
     meaning), NULL = signs feed [obj+0x71]/[obj+0xb6]
     bits instead; 0x55070 (the 0x28af0-lineage +0x8c
     getter) is a 3-call chain (+0xb0, 0x541f0's v-channel
     with the PR-9x [obj+0xba]/20 formula, +0x98) called
     DIRECTLY from the PR-177 relation providers. NEW tool
     fact: indirect `call [r+disp]` sites need the ModRM
     ff /2 byte scan (callscan.py) - text.asm prints none
     over the desynced clusters. 5 new items; 1 closed
     (PR-172 item (1), with the honesty note that "the
     port runs them as plain time states" was true of the
     object path only); 2 brackets. Next -> PR-179 = the
     LION +0xac/identity pass. Full record: PR-178 section
     of NEXT_STEPS.md (scratch /tmp/opencode/pr178/).
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
     0x97970/0x97968 for both ZARIA classes [CORRECTED by
     PR-156 F4: the ZARIA-era placement chain writes TLew-
     Zaria 0x96960/0x96958 and TLewZariaB 0x97a98/0x97a90;
     0x97970/0x97968 is referenced ONLY by ctor 0x1e0a0 - a
     runtime swap variant]), per-class fields like
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
   - THAT LAST PHRASE IS NOW SUPERSEDED, the question is not (added by
     PR-114, quoting the sentence above instead of editing it). The
     count is a tool output now: `python3 tools/re/mov_byx_gate.py`
     (it reads rows the way `ll_mov_parse` does; `mov_channels.py`'s
     row regex misses 2621 of the 18617 row lines) measures 105 blocks
     naming `BY_X` in 69 files, 30 of them `F=0,0` with 290 rows in 21
     files, 29 of those declaring no motion source of their own, 5 of
     those 29 having no exit except a wrap-armed one, and 97 states /
     357 rows / 80 files where the engine's mode bit and the port's
     gate disagree. And "no test does" was too strong: three fixtures
     ARE `BY_X` with `fx == 0` (ll_play.c:1798's RAN `A_GO_TO_BALON_L`
     copy, ll_level.c:2196's `gnd`, ll_level.c:2248's `halt`), but
     none of them checks the row clock - leg 3's own comment
     (ll_level.c:2364) writes the fallback as an estimate: "the frame
     list falls back to time - estimate". PR-114's two legs turn that
     estimate into a pinned model choice without changing behaviour;
     which half of the gate the engine keys on is still open, and the
     mode-only reading now has a concrete counter-example in the corpus
     (those 5 no-exit states would neither advance a row nor leave the
     state under it). Counts, arithmetic and the five mutations:
     NEXT_STEPS.md, PR-114 record.
   - "ONE ROW PER RECORD STEP VS THE PORT'S LOOP" IS MEASURED NOW, and the
     sentence two bullets up is wrong about the port (added by PR-115,
     quoting it instead of editing it). The engine half stands: 0x52329 `39
     d8` / 0x5232b `7d 27` admit at most one row per record step. The port
     half describes one of its two loops. `ll_player_step` section 7 caps
     one step at `4 * nfr` closures (ll_play.c:1288) and then zeroes what
     the cap leaves over (:1318-1319), so the port DROPS a row-debt the
     engine keeps - nothing in that path ever rewrites `[obj+0xa1]`, whose
     only writers are the feeds at 0x543d1 / 0x543e7 / 0x54412 / 0x5443b, so
     the engine's accumulator is monotonic and has neither a cap nor a
     resync. `obj_advance`'s loop matches the engine there: no cap, no
     resync, only the FIX-13 4096 guard (ll_level.c:1188-1189) - leg 7 gave
     it a 1700-unit feed and all 17 rows of a one-row list closed in that
     one call. The threshold table PR-110 quoted needs its own correction:
     170 rows at authored threshold 0 in 73 files counts the port's parse-
     time clamp (ll_play.c:481 makes any authored <= 0 into 100 BEFORE the
     `time_scale` scaling at :735, so no stored 0 exists), 0 thresholds are
     negative anywhere, and as-authored rows in 1..99 number 1682 in 109
     files against that record's 1715 in 111. The set where "one per step"
     and "all that fit" really differ is 108 time-fed states in 77 files
     whose stored minimum threshold is <= 50, and 0 in the distance-fed half
     under both per-step feed estimates. Two data-free legs
     (`MULTI`/`BOUND`, `objm`/`one`) pin the port's current behaviour
     without changing it, and six mutations of the cap, the resync and the
     parse clamp are all caught - the one asymmetry worth keeping: bounding
     the OBJECT loop is caught by PR-110's older legs, not by the new one.
     Which of the two laws the game runs is still open and needs a capture,
     not a disassembly (T5/T10); the exact-equality boundary is the next
     question in line. NEXT_STEPS.md, PR-115 record.
   - THE EXACT-EQUALITY BOUNDARY, ANSWERED 2026-11-18 by PR-116, whose
     bullet above named it open ("the exact-equality boundary is the next
     question in line"). As a reading question it is closed: `39 d8` at
     0x52329 is `cmp eax,ebx` - eax is the row's stored threshold (loaded
     at 0x52320), ebx the accumulator (loaded at 0x52323) - and the flags
     come from the first minus the second in EITHER syntax, so 0x5232b
     `7d 27 jge 0x52354` returns without running the row when the
     accumulator is exactly AT the threshold. The engine fires a row only
     STRICTLY past its threshold: at equality nothing happens, the
     accumulator is kept, and the next feed runs the row 100 iterations
     later with a remainder of a whole feed, which the row handler then
     writes back to [obj+0xa1] at 0x52118 (`89 90 a1 00 00 00`) - so the
     subtraction is committed after all, and the accumulator is not
     monotonic. The reading that goes the other way needs the operands
     swapped and contradicts itself, and PR-110's "which also makes the
     `sub ebx,eax` remainder negative" belongs to that swapped reading, not
     to the bytes. The population, measured: 15317 of the 18617 row lines
     store a threshold that is an exact positive multiple of the 100-unit
     feed (BY_TIME 15142 of 17011 rows in 754 files), in 2012 states.
     `Wait` - the one directive whose node skips the compare and matches
     through 0x47e40 instead - is spelled `WAIT` in this text, and the
     name compare at 0x46b59 is 0x76a69, which lower-cases BOTH sides, so
     it is reached after all: 255 standalone `WAIT` tokens, 251 of them on
     row lines (the `WAIT K_NEVER` / `WAIT AC_*` tails PR-88's table names,
     which the port counts but does not implement). Whether those become
     keyed ROW nodes or separate event nodes
     is unread, so the honest figure is a bracket: 15066 at the bottom of
     the boundary population, 15317 at the top - and either way most of the
     corpus's rows. The port still runs `acc >= thresh` and this PR changed
     no behaviour, so what is left is a decision with a number on it, not an
     unknown: flipping the two gates moves 15066-15317 authored rows by one
     whole feed of 100 iterations, and 41 play legs plus the two object legs
     that pin the current rule break when it is flipped (nothing at all
     breaks if only the object's walking half is flipped: no leg puts a
     distance-fed accumulator exactly on a threshold, which is the fixture
     gap PR-116 records). No clip in the repo separates the two, exactly as
     PR-110 said. PR-116 record; `tools/re/mov_byx_gate.py` section 5,
     output `/tmp/opencode/pr116_census.txt`.
   - THE FIXTURE GAP IN THAT BULLET, CLOSED 2026-11-18 BY PR-117. Its
     parenthetical - that flipping only the object's WALKING half of the
     row gate breaks nothing, because no leg puts a distance-fed
     accumulator exactly on a threshold - is no longer true. Leg 8 of
     `obj_motion_selftest` puts `facc` exactly on a 100-unit row's
     threshold (fixture `control=BY_X` `F=100,0`, one row `1,100`, two
     dt=2000 calls; the walk feed `|F| * dt_ms / 4000.0` is exact in
     binary64, so 50.0 then exactly 100.0), and the same g2b mutation -
     `<` -> `<=` at ll_level.c:1230 - now fails exactly that leg and
     nothing else, with a clean control run before it. The section-6
     paragraph above quotes the wording it supersedes and gives the
     arithmetic. What is still open is the PLAY side of the same
     boundary: a probe at the player's row loop (ll_play.c:1286) printed
     0 equalities over the whole play selftest, and that feed is
     `p->vx * (dt_ms / 1000.0)` (:913, :1100, :1119), which cannot hand
     an integer threshold exactly at the shipped 28 ms step - PR-117 open
     item 1. No behaviour changed: not one logic line of ported/ was
     touched.

   - THE DIVISOR IN THAT BULLET'S NUMBER, QUALIFIED 2026-11-18 BY PR-118.
     The phrase - "flipping the two gates moves 15066-15317 authored rows
     by one whole feed of 100 iterations" - is the cost for the TIME-fed
     rows only, though it is stated for the whole population. A
     distance-fed row's feed is its own per-call distance, `|F| * dt /
     4000` (obj_walk, ll_level.c:1101-1104), never that 100. Measured
     with the port's own `walk` rule (:1169), the corpus has 75 walking
     states, 992 rows in 57 files, and their stored thresholds are 2..10
     units, so the multiple-of-100 test the census uses selects not one
     of them (0 of 992) - the BY_X line's 67 rows are all time-fed. The
     same `cmp`/`jge` boundary does exist for those rows, at a multiple of
     their own feed: 0 of 992 on the 28 ms grid every selftest uses (the
     feed is `7|F|/1000`; reachable thresholds are the multiples of
     `7|F|/gcd(7|F|,1000)`, and no (|F|, threshold) pair here matches -
     |F|=100 could land a 7-unit row in 10 calls, but no such row is
     written), 713 of 992 on the 20 ms play/PPM grid (:6425), 689 at
     dt=40, 0 at the dt=2000 PR-117's leg invented. Section 6 of
     `tools/re/mov_byx_gate.py` prints all of it
     (`/tmp/opencode/pr118_census.txt`; sections 1-5 byte-identical to
     `/tmp/opencode/pr116_census.txt`), and the section-6 paragraph above
     quotes the wording this supersedes. It also found two states where
     the port's double accumulator drifts off a landing its own feed law
     says is exact (`LONA.05_LEW`, |F|=30 at dt=20: 20 additions of 0.15
     reach 2.9999999999999991, so the row runs on call 21 with a
     remainder, where exact arithmetic and the engine's integer dword
     accumulator - 0x543d1 `add [obj+0xa1], eax` - would land on 3.0 at
     call 20); nothing in either selftest reaches that case, PR-118 open
     item 1. No behaviour changed: the only file touched is the census
     script, which no build or test leg runs.

   - THE CASE THAT SUB-BULLET CALLED UNREACHED, TESTED 2026-11-18 BY
     PR-119. Its closing clause, quoted verbatim:
     "nothing in either selftest reaches that case, PR-118 open item 1."
     is no longer true. Leg 9 of
     `obj_motion_selftest` is the fixture it named: F=30,0, one 3-unit
     row, 20 calls of dt=20 and a 21st, and it asserts the landing the
     census predicted (row held back at call 20 with facc 2.9999999999999991,
     row closing on call 21, remainder 0.14999999999999902 carried into
     the next row), as a band, so an exact accumulator fails the first
     check and a loop that drops the remainder fails the second.
     Mutations: /tmp/opencode/mut/run119.sh with prog/norem.py and
     prog/exactf.py each break ONLY that leg, and g2b.py still breaks
     only leg 8 - the control that the new leg is not a copy of it. The
     leg also surfaced a second accumulator story (obj_walk's `xa` pays
     its whole pixels early and re-draws the rounding at each boundary,
     so the object has moved 3 px by call 20 while facc says 2.99...),
     which is pinned by the same leg and recorded as PR-119 item 1,
     because which accumulator the engine keeps for a walking state is
     still unread. No behaviour changed: test-only.

   - THE QUANTISATION PR-99 ITEM 9 CALLED "WORTH A TEST OF ITS OWN" HAS
     THAT TEST NOW, 2026-11-18, PR-120. Its words, quoted verbatim:
     "That quantization is visible at low speeds and is worth a test of
     its own."
     It is a mutation, not new code: `prog/quant.py` feeds
     `o->facc += (double)(int)dist`, the engine's whole-pixel-per-step
     shape, and against the shipped bytes it fails exactly one leg in
     either chain - PR-119's leg 9, first check, `facc 0` where the
     drift value belongs - with `--play-selftest` clean
     (/tmp/opencode/pr120_quant.txt). Leg 8 passes it: that fixture's
     per-step feed is a whole 50, so truncation cannot reach it. The
     reading that came with it: the row accumulator `[obj+0xa1]` is a
     field whose only consumer is the row gate (0x52323), so there is no
     engine-side twin of the port's `facc`/`xa` pair - the second
     mechanism PR-119 pinned is port-internal - and the sub-pixel part
     of the engine's feed is dropped at the divide (0x5435f), which
     bounds PR-118's reachable-threshold counts as port-side figures.
     No behaviour changed and no code was touched by this PR.

   - THE ENGINE MOVES THE OBJECT BY THAT SAME TRUNCATED AMOUNT,
     2026-11-18, PR-121. It reads PR-120's own closing words in section 6
     as the open item and answers them, quoted verbatim:
     "now narrowed to the three unread readers of `[obj+0xca]` at
     0x54810/0x54830/0x54836":
     The writer is 0x54804..0x5484a: `[obj+0x0]`/`[obj+0x4]` are read,
     the truncated dwords are added to them (0x54827/0x54829) and stored
     back (0x5482b/0x5482d), and the delta fields are then cleared by
     self-subtract (0x5483c/0x5484a) - so nothing carries, and the
     position quantises exactly like the row gate. The port's `xa`
     (:1104-1106) does carry, so at any velocity that is not a whole
     multiple of 100 hundredths per tick the two paths cannot agree:
     a model difference, not drift, and the code comment at :1073-1077
     states only the feed half. Not modelled, not tested, no code or
     comment changed by this PR: whether to reproduce it is the owner's
     call, alongside the PR-116 strict-greater item. PR-121 record in
     NEXT_STEPS.md, with the `[obj+0xea]` one-shot vertical amount and
     the `[obj+0xe6]` link as its open items.

   - THE ONE-SHOT VERTICAL AMOUNT IS A GAP TO A TWO-POINT SEGMENT,
     2026-11-18, PR-122. It reads the two fields named there, quoted:
     "the `[obj+0xea]` one-shot vertical amount and the `[obj+0xe6]` link
     as its open items."
     Those two fields are now read: 0x455a0 returns the signed vertical
     gap from a point
     to the line through two records 13 bytes apart (0x45400 builds them
     as base+i*13 and base+(i-1)*13 at 0x45481..0x4548f), truncated at
     the divide like every other engine quantity in this chain, and
     0x454a2/0x454a8 store the link and the gap together only when the
     gap is <= 0 and no larger than the stored one. Joined with PR-121's
     tail: attach a provider, compute a one-shot correction, apply it to
     the y delta once, clear it. Not terrain, not yet: 0x45380 and the
     slot +0x6c provider are unread and there is no guard against a zero
     denominator. The port models no geometric y at all (`dy=` directives
     at :1238, base-y resets at :831/:927/:1001/:1214), so this has
     nothing to be tested against yet; whether to model it is the owner's
     call. No code, no comment, no behaviour changed here.

   - THE ENGINE HAS AN INDEX LOOKUP FOR THAT GAP AND THE PORT HAS ITS
     OWN COPY OF THE QUERY, 2026-11-18, PR-123. It reads the open item
     named there, quoted:
     "with reading 0x45380 and the +0x6c provider as its first open item",
     and answers its first half: 0x45380 is a linear search over the
     x-sorted 13-byte records that returns the segment index, never 0 -
     index 0 is its not-found value, which is why 0x45400's `i`/`i-1`
     pair is always two real records; three bail-outs return it (empty
     table, x below the first record, x above the dword at
     `base + count*13`). A sibling at 0x453d0 does the same lookup,
     bails to INT_MAX, else builds the pair and calls 0x455a0 and
     returns the gap to its caller. The port side is the same query in
     the open: `seg_y_at` (`ll_play.c:123`, truncating, with a zero
     denominator handled at :126), `ll_plt_surface_at` (:140) and
     `ll_plt_first_below` (:164), both reporting not-found through
     `best = 0x7fffffff`. The player is stepped against that ground every
     tick (`ll_player_step` :905, :1149, :1173), an object only at
     placement (`obj_settle`, `ll_level.c:1693`), so PR-122's words here
     about no geometric y in the port are true only of the per-tick
     object path - recorded as a correction in PR-123, no shipped record
     edited. What the 13-byte records are, and who writes them, stays
     unread: no code, no comment, no behaviour changed here.

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
   - PR-131 (ZARIA.01 footage audit) adds the two referee windows a
     rate measurement needs in this stage: levels/ZARIA.01.mkv
     t=2-13 (a straight walk from spawn x=50 to the ladder x=180,
     both fixed landmarks) and t=55-62 (ZWODOSPD@992 column in
     view). Also: the video's fast gait is the LEW.ZAR gallop
     (A_RUN art) and the port's FWD produces only A_WALK - the
     A_WALK->A_RUN entry condition is an .MOV row question, answer
     it from data, not from the video. Eyeball walk numbers
     (10-25 u/s there vs the port's 20) stay ESTIMATE until
     video_camera_shift.py / video_lane_shift.py say otherwise.
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
15. ZARIA.01 STAGE AUDIT GAPS (PR-131; the full G-1..G-12 list with
    cited frames and port logs is the PR-131 section of
    NEXT_STEPS.md; PR-136 added G-13 and closed G-2's method - the
    landmark anchor now reads an absolute camera (x, y) per frame). The
    ones no other item owns: the KAMIEN sixth
    field (1308/1218/1115 decreasing toward the ZAR.BLK corridor
    1299..1620 - read ZAR.BLK like a .PLT) and what it and the
    corridor DO (stone puzzle, G-3); the water surface as a floor
    (lion walks WODA at pool level, zax37 t=305-307, G-7); the
    level-end scene (purple-robed NPC + bird + ball tosses,
    levels/ZARIA.01.mkv t=440-457) and the mid-level bird
    sequences (zax37 t=306-315, 385-413): no placement line names
    them and the ZARIA.01 ANIMACJE tree has NO bird leaf (census:
    PTAK.MOV exists only under KADMAR.07/PTAK, TUNGA2.12/PTAKPLAT,
    RAN.01/GPTAK_RA) - scripted chain reusing other levels' leaves,
    or engine code; find which (G-10/G-11); HUD lives: both R3
    clips start at "5", the port prints a hard-coded 6 - map the
    STATUS.LEO fields instead (G-8); contact-damage reality (G-5),
    CLOSED by PR-139: an authored `damage=` hurts the lion only
    through the object's own ATC window behind the 0x550c0 clash
    rule, a bare touch costs nothing, and the hit countdown is 30,
    not 14; and G-9's traverse probe RAN and is PINNED by PR-140 -
    a plain walk reaches x=685 (the ladder column x=180 at tick 91)
    at ZERO energy cost, the worms answer with the SEMI push-out
    alone and cost delay, not a stop - and PR-141 FOUND the
    AC_DRAB/AC_DRAB_CATCH raise site (the ladder's own answer
    entry 0x3f1c0 writes them into the toucher); and PR-142 CLOSED
    the evaluator/key question - the 13-byte set IS the live
    signal set at [obj+0x60] (PR-88 right, PR-101's identity-key
    wording superseded) and an authored condition is an AND-
    against-live inside the ordered row scan 0x52000, which IS
    the port's shipped model (confirmed, no change); PR-143 then
    read the DELIVERY: signals ride as records - global broadcasts
    (censused: never key bits, never AC_DRAB/AC_DRAB_CATCH), per-
    object queues at [obj+0x10] drained through the +0x10 receive
    raising bits into [obj+0x60]; the scan's caller key is a
    VERBATIM live-set copy (0x53520, one call site, right after
    the recompose), 0x535a0 is the row-switch executor, and the
    answer chain runs only from rows flagged 0x4000 =
    LL_CTRL_CHECK_ANI (ZARIA.01 ladder rows: none - the untouched
    column in PR-140's run is consistent). PR-144 then read the
    LION SIDE: its class tables are 0x96960/0x96958 (token
    LewZaria 0x9078b), its receive entry is its own 0x1e130
    (generic tail 0x50d60), and its contact entry +0x78 IS
    0x50f80 - the same slot the ladder answers through - with
    the pair-probe 0x4bd32 giving both contact members their
    turn and unioning their 13-byte outputs; the only ways bits
    enter a live set are now fully enumerated (14 sites). What
    the CLIMB still
    needs (PR-145 read the KEY FEED: the generic provider
    0x54910 at class slot +0x8c builds K_UP/K_DOWN from the
    signed intent fields [obj+0xbe], K_BKWD/K_FWD/K_JUMP from
    the intent + the open-window flag, K_FIRE/K_ALT from the
    AUTHORED state flags [obj+0xa5..]; the intents are written
    with the engine's own ±10 by the message consumer 0x549f0;
    PR-142's "standing bits zero" was DRABINKA's zero OVERRIDE,
    corrected): the ONE named gap - the contact-pass entry
    (0x4bd32/0x4bdf6/0x4b500/0x4b602 are zero-reference
    fragments; the queue-drain entry 0x52b50 and its PR-106
    callers were our mis-anchor, resolved), the per-frame setter
    of the lion's contact target [obj+0xd2] (contact partners
    SHARE live sets through it: 0x54b84 ORs the partner's own
    [obj+0x60] in under a row guard), and the signal-record
    poster for AC_DRAB (no direct site posts bits 10/12 - they
    can only arrive as signal records or via the share; K_GEN1
    itself has three raisers, one HP<100-gated message type at
    0x50d20's jump table, one PRNG-random at 0x26670) - and the
    A_WSP feed law (PR-146); SURPRISE
    "?" firing (G-12); and the
    ground SOURCE of the first screens (G-13, PR-136) IS DECIDED IN
    PR-138: the engine's ground records ARE the authored .PLT text,
    and the 7-51 offset was screen-x from false-matched
    single-landmark cameras - every verified frame stands the lion on
    a .PLT line at d+0, and no second ground-record builder exists
    in the image. The open remainder is named there: the port never
    parses the ani_plat= LOCAL ground lines (ANIPL2: KATAPULT/PALMA
    entries), and engine-side the entity record table (0x34850,
    [obj+0x48]/+0x4c) and its MOVEPLAT-chain provider are unread.
    Read that chain first, then parse the local lines behind a
    test-and-mutation pair; no scene re-pin is needed at the ground
    line.

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
narrow and explicit: PR-135 REPLACED the port's fitted target-velocity
law with the engine's own F/T register, so the two sides no longer
disagree in code - the open question is purely which T dependence the
video shows, and only a BY_X object at T other than 40 can say. Next steps, in order: (1) that video test, which
the authored data supplies in quantity (F=100 against 11 different T
pairs); PR-125 ran the first two probes on R3 footage and both referee
objects (RYBKA A_RUN F=100 T=20, PINGWIN A_RUN F=75 T=50) animated
without translating until the lion was adjacent - §7 item 13's
PR-125 bullet names the next fixture; (2)
the two cheap tests PR-102 points at: the first is DONE (PR-107 applies the
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
(1) above is now THREE negative probes and three facts, recorded in §7
item 13's PR-125 bullet, and PR-126's join: the fixture named
there, KADMAR.07's NINJA1-6, is WITHDRAWN - no `.des` places it
and no `.LVM` or `.MOV` references it - and the named referee is
now the KADMAR.07 BOSS A_RUNA/A_RUNB (BY_X, F=75 T=20: A 18.8
vs B 37.5), one instance at x=1328. PR-128 closes THIS file as a source:
its whole 1451.7 s is swept at the widest setting (93 camera-still
windows, 3.0-17.5 s) and a frame from the middle of every one has been
looked at - no window with the HUD drawn contains the boss, and the boss
figure that repeats through the cut is a scripted no-HUD scene (a black
room between two white-outlined wardrobe doors), which is also what the
763-810 s windows PR-125 and PR-126 measured are. The code side is now
read end to end (PR-129/PR-130): the integrator at 0x54250 - the
standard class-table slot +0xc8 - settles at (5*F*s/T)/100 px/tick,
where F and T ARE the authored `F=`/`T=` clauses (0x52680 copies the
object fields at +0x50/+0x58 into the command block verbatim; the
parser 0x52590 is the class's own vtable slot +0x00) and s =
[obj+0xba]/[obj+0xbe] is seeded from the record's own default factors
by 0x549f0 [s CLOSED IN PR-132: that "record" is a caller-built stack
pair, every producer is read, |s| <= 10 and s = +/-10 exactly at full
one-axis intent; no authored field can carry s]. What is open is the
PAIR (rate, s) [RATE CLOSED IN PR-133, CLOCK MEASURED IN PR-134]:
the only clock read in this repo is the ~28 ms iteration clock
(PR-109/110), while the published reproduces split between 20 Hz
(T=40 walks, s=10) and 25
ticks/s (the jump /100 reading), and the record default factors were
never censused. The video referee - LONA.05's YETI (F=100 T=50, one
instance at x=2100) - can only decide once rate AND s are fixed; an
iteration-driver read plus an s census settle it from code and data
first [s census DONE IN PR-132; the iteration-driver read is
PR-133, and it CLOSED the rate side: no separate object tick -
rate = iterations/s x passes-per-record-per-state-step, at most
1 pass for the moving families, so "20 Hz" is the iteration
clock at 50 ms, the top edge of PR-109's bracket]. The two-factor
fit is MEASURED IN PR-134 and its clock factor is settled: inside a
four-ways-verified camera lock the lion's ground run at a held key
is 38-76 units/s and the ZARIA worm's rate is <= 30.3 units/s (its
head eye tracked at 60 fps), so the pass rate comes out 15-24/s
from both families - 20/s, the 50 ms edge at one pass per state
step, inside both; 35.7/s, the 28 ms design point LL_ITER_MS,
outside both (it predicts 44.6 and 89 u/s against those bounds).
The pass factor's mechanism is read and its bound is 1. The clock and
the port's own speed constants are APPLIED IN PR-135: LL_ITER_MS = 50,
and the port's lion and objects now step the engine's own velocity
register once per iteration (`v -= clamp(T,.,130)*v/100`, then
`v += F*s/100` at the key intent s = ±10) with AT MOST ONE row admit
per iteration, so the port's ground run is that arithmetic rather than
a fitted target velocity. What is still open, all named in the PR-135
record: the vertical constants are not re-fitted (the authored jump
apex is now 20 px against the ~24 px measured), the engine's
conditional raise of T to >= 20, the partial-intent values of s below
±10, the object's own velocity register, whether a state change
consumes its step's feed, the force/gate-fed row lists inside A_JUMP
and A_DOSTAL, the velocity carried across a state change, PR-83's
pending-AC_TIME lifetime, the object `nfr == 0` guard, and the one-
point-per-iteration terrain test. Also still open: the rhino
referee - now placed,
`NOSOROG,dummy,nos.mov,1700,600`, bounds 1680,1950, but never on
camera in a sampled window - the state that plays the airborne leap
poses. The G-2 camera position IS MEASURED IN PR-136 - the landmark
anchor PR-134 asked for is built from the level's own static .SEQ
placements, and the whole 460 s clip now reads a camera (x, y) in world
units: the right-facing lead 95-108 confirms CAM_LEAD_R=100, the
grounded vertical anchor is ~128 plus the existing world clamp (not
178) - AND IT IS APPLIED IN PR-137, with the whole camera law pinned
hermetically by --follow-selftest; the catch-up ease stays a model
slower than the port's in the data, and the view does not hold rigidly
through jumps (both counted open). PR-136 opened G-13 and PR-138
CLOSED it: the "7-51 above" offset was a measurement artifact
(screen-x from false-matched single-landmark cameras); the engine's
ground records ARE the authored .PLT text, and every verified frame
stands the lion on a .PLT line at d+0. What remains ground-side is
the ANIPL item named in the PR-138 record (ani_plat= local lines
never parsed; the entity-table registration chain unread). See the
PR-130, PR-132, PR-133, PR-134, PR-135, PR-136, PR-137 and PR-138
bullets in §7 item 13.
tools/re/des_census.py prints the whole joined census. The
table
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
STATUS after PR-131 (footage audit of THIS stage): the first-chasm
crossing is STILL not observed in any sampled window, and the
leading reading changed - the player most likely BYPASSED the gap
above the water (up the ladder at x=180, along the canopy, down at
t=25-28 of levels/ZARIA.01.mkv), so do not treat a running jump at
that chasm as the reference until re-watched; T1's zax37 ~315 s
running jump is a separate jump (and the zax37 player does walk the
water surface at 305-307). The measurement windows are named in
§7 item 13's PR-131 bullet. The port's own probe walks die to LICHA
contact damage long before x=408 (G-5, CLOSED by PR-139 - the
corrected contact model is what lets him reach x=180). The probe is run
again as PR-140.

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
flip at the bounds range; PR-135 restates that rate as the engine's own law -
`F*s*LL_ITER_MS/(1000*T)` px per iteration at s = ±10, the SAME number at
T=40, which is where every shipped patrol sits, and 25% slower at T=50);
CH_DIR landed in PR-81 as the facing flip - the
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
STATUS after PR-131 (superseded on the port side by PR-139/PR-140 -
read the PR-140 record before quoting the old probe wording): the
ZARIA.01 ladder AT 180 is confirmed usable in the original (ridden
down at levels/ZARIA.01.mkv t=13.3-15.7 and mid-climb at
ZARIA.01_b.mkv t=200.0-200.7). The old claim that the port's probe
"dies at the LICHA worms around x~153" is NOT what the port does: on
the pre-PR-139 source the same walk reached the SAME positions
(tick 91 x=180, tick 300 x=685) while PAYING 15 per touch (three
contact hits, energy 100 -> 55) - the old model blocked the traverse
by energy cost, not by a position stop, and no probe configuration
here reproduced a death before x=180. PR-140 runs the probe under the
corrected model and pins it as a verify-play leg: a 12 s FWD walk
crosses both worms and stands x=685 at tick 300 with ZERO damage,
zero state changes outside A_WALK, and the worm answer being the SEMI
push-out alone (cost: ~40 lost iterations, 0.8 vs 2.5 units/tick).
The CLIMB at x=180 still does not work, and the blocker list has
narrowed repeatedly (PR-141, PR-142, PR-144/145; PR-146 closed
the last reading gap - the contact delivery is read end to end -
and PR-147 closed the last reading law: the A_WSP feed law is
now read at byte level, so ONLY the implementation decision is
left; PR-148 shipped its first half - the guard and the intent
word are CODE, and what blocks the climb now is solely the
contact-answer route). CLOSED by PR-141: the
AC_DRAB/AC_DRAB_CATCH answer bits themselves - DRABINKA's own
answer entry 0x3f1c0 raises exactly those into the toucher, gated
on its unroll rows, and the ladder's +0x40 getter is constant-zero
(no ATC window, ever). CLOSED by PR-142: the EVALUATOR/KEY
mechanism - the 13-byte field a transition ANDs against IS the
live signal set at [obj+0x60] (PR-88's TEventSet reading stands,
PR-101's identity-key wording is superseded: the object family's
identity providers are `ret` stubs), 0x47e40 is a set-operation
dispatcher not a lookup, and an authored condition matches by
(authored node mask AND live) != 0 inside the ordered row scan
0x52000 - first match wins - which is EXACTLY the port's shipped
condition model (a confirmation, no port change). Signals enter the
live set through the class +0x10 entry 0x53770 (signal record, bit
index at its byte +0xc), so K_GEN1's MECHANISM is found and only
its POSTER is open. PR-145 read the KEY FEED and PR-146 CLOSED
the CONTACT DELIVERY end to end, so the climb line now has one
open law and one implementation decision - no unread mechanism
left (PR-146 also corrected three readings, including two of this
series' own):
THE CONTACT LOOP, READ: object A's motion pipeline (the [obj+0x44]
interior vtable: 0x52750 -> slot +0x34 = the merger 0x52790 ->
the Lew / animacje / zwierzaki list paths 0x4bd30 / 0x4b890 /
0x4b870, gated on the object's own flag bytes) gives the other
side's +0x78 contact entry a turn; that entry answers with a
13-byte set AND appends a node (alloc 0x1d = a 16-byte header +
the answer set at node+0x10) onto THE ASKER's contact list
[A+0xd2]. That append inside a contact entry (0x3d440, held at
+0x78 by the tables 0x96a88/0x99e64) is the ONLY non-zero writer
of [obj+0xd2] (0x5415b/0x54e88 write zero, 0x551fc pops). A's
contact method 0x549f0 then ORs the head node's STORED answer
set into [A+0x60] under a node-kind + record-index guard (0x54b65
..0x54b84; PR-145's "[partner+0x60]" expression corrected), and
the lion's own key provider pops the head (0x504e0 -> 0x551f0).
That is how DRABINKA's 0x3f1c0 answer (AC_DRAB 10, AC_DRAB_CATCH
12, PR-141) reaches the lion's live set - no signal record.
PR-144's "zero-reference contact fragments" were a mis-anchor (the
entry is 0x4bd30, called once at 0x527cb from 0x52790, which
PR-130 already named); the queue drain 0x52b50/0x52b80 was
already PR-106's.
THE SIGNAL POSTERS ARE A CLOSED SET: the tag immediate 0x8138764
appears SEVEN times image-wide - six builders plus the receiver
test at 0x53780. AC_DRAB is posted by ONE direct receive call
(0x28835 builds bit 10, 0x2885a calls the target's +0x10 entry)
into [[obj+0x30]+0x40][1] - that index is NOT proven to be the
lion; AC_KOREK(33)/AC_BECZKA(28)/AC_SPOKUJ(27) ride the three
0x4bc40 broadcasts; AC_LINA(21) is pushed straight into the
lion's queue (0x2c9a4); K_GEN4(57) is posted in a loop over that
array. Bits 12/22 have NO signal poster - a counted negative, and
consistent with the answer-set route above. PR-143's broadcast
"bit census" was the RECORD-TYPE argument (the type census is in
the PR-146 record); its conclusions - no broadcast posts key bits
or 10/12 - stand on the stronger basis.
THE KEY FEED (PR-145, two sentences corrected by PR-146): the
generic standing-bits provider 0x54910 (class slot +0x8c;
DRABINKA overrides it with a zero stub - PR-142 measured the
override) builds K_UP/K_DOWN from the signed intent fields
[obj+0xbe], K_BKWD/K_FWD/K_JUMP from the intent + the open-window
flag [obj+0xb6] bit 4, K_FIRE/K_ALT from the authored state flags
[obj+0xa5..]. The intents come straight from the state record
(record word 0 -> [obj+0xba] at 0x549f9, word +4 -> [obj+0xbe]
at 0x54a08), with the engine's own +/-10 overrides written by
0x549f0 when the head contact node's KIND is 1 (target-relative
through the [vt+0x68] getter, +-8 dead band) or 4 (raw x); the
selector is the node's kind word [[obj+0xd2]+4], not a record
mode. The 0xcf400 "input singleton" is itself an object (records
via 0x58690). K_GEN1's posters: an HP<100-gated message type (the
0x50d20 jump-table entry 0x50df4), 0x244a5/0x24552, and a
PRNG-random K_GEN1/2/3 (0x76bcc LCG -> 0x26670 -> the raise at
0x26738). PR-144's 14-site live-set write census is resolved
except two mask windows (0x28bc7, 0x50ab7); 0x2451e is K_FIRE
(corrected).
WHAT REMAINS FOR THE CLIMB: ZERO open laws (PR-147 closed the
last one at byte level, and corrected one of PR-146's labels -
the kind-4 contact chain is the VERTICAL intent arm, not a
raw-x variant). THE LAW, END TO END: the authored A_WSP/
A_WSP_LIN blocks (12 in ANIMACJE, BY_TX zero - a byte census)
carry wire control word 0x205 = BY_TY mode 4 + CHECK_ALL +
SEQ_REV_Y, F=0,100, T=160,50, rows 300/200.../300. A BY_TY row
is fed EXACTLY 100 per frame while [obj+0xbe] != 0 (0x5441b;
the 100 has exactly one writer image-wide, 0x4c8b7 in an init
entry), then the compare against the row threshold [rec+0x10]
runs AFTER the feed with one admit and the remainder carried
(0x52320..0x52338 -> the row advance 0x52470 writes it back at
0x52118) - the shape the port already ships. THE GUARD WORD IS
THE PLAYER'S OWN VERTICAL INTENT: the frame loop calls the
LION's class-table slot +0xcc with its 12-byte input block every
frame before stepping it (0x4aeca; that slot has exactly 7 call
sites; the loop is gated on ds:0xa94f0 - zero makes it return
2), 0x549f0 stores the block's words into [obj+0xba]/[obj+0xbe]
unconditionally (0x549f9/0x54a08), and the KEYBOARD puts +/-20
on the two axes (Up 0x248 -> -20, Down 0x250 -> +20, Home/Left
0x24b -> -20, Right 0x24d -> +20; reader 0x582f0; words 0/1 are
only ever 0/+/-20). THEN the contact chain overrides, in the
same call: a head-node of kind 4 moves the vertical intent to
+/-10 toward the node's y ([head+0x1d], a +-8 dead band,
0x54b16/0x54b3b/0x54b47); kind 1 does the same on x (PR-108's
arm, +-8 confirmed); kind 2 merges the stored answer set (PR-
146). [obj+0xbe] has exactly four writers and five readers (the
guard 0x5441b, the integrator's force 0x542cd, K_UP/K_DOWN at
0x5499d) - so while a contact node exists the intent is the
engine's +/-10 toward the partner, otherwise the player's
+/-20; PR-145's "s = 10, never any other magnitude" NARROWS to
the contact path. The keyword encoding itself is now byte-exact
(0x51b35..0x51da0: BY_X 1, BY_Y 2, BY_TIME 3, BY_TX 4, BY_TY 5,
FRAME_REV 8, SEQ_REV_X 0x10, SEQ_REV_Y 0x20, ...) confirming
PR-108's five-value mode item for item; the demo record format
is a 12-byte input block + a length (0x58690 playback,
PR-145 item 6 CLOSED). THE IMPLEMENTATION DECISION IS HALF
SHIPPED (PR-148): the port now HAS the guard - a BY_TY row is
fed the 100 only while the player's vertical-intent word
(p->vert_intent, the engine's [obj+0xbe] shape with its sign
semantics) is non-zero, the compare still runs, and the partial
accumulator carries across the gap (0x5441b's order); the held-
intent spans are UNCHANGED from PR-108's, so the guard only
subtracts feeds. What the port still lacks: the contact-answer
route (the node payload -> the live set - the blocker of the
ZARIA.01 climb, NOT the guard), the word's use as a v_y force
(0x542cd reads [param+0x4], field map unread - PR-148 open
item 1), and any vertical MOTION: the session's claimed rise
law at 0x50252 is DEAD CODE (zero references by both scan
kinds; the lion's frame-loop +0x74 is 0x50320 -> 0x54c00, a
friction/ground gate), so the climb's rise waits on the y-force
path, not on this PR. OPEN ODDITY COUNTED: the kind-4 chain
compares node+0x1d, the 30th byte, while the node builder's
allocation is 0x1d bytes (whose answer set ends at +0x1c) -
who builds kinds 1/2/4 (PR-146 item 1) has teeth now; node+0xc
IS the ID compared at 0x54a24 (PR-108), which half-resolves it.
CLOSED by PR-143 additionally: the answer-
chain entry gate (row flags 0x4000 = the port's
LL_CTRL_CHECK_ANI; ZARIA.01's drab/lina rows carry none, so the
ladder answering nothing in PR-140's run matches the engine's
law), the row-switch executor 0x535a0 (exit/entry/index-store),
and the scan's caller key = a verbatim copy of [obj+0x60]
evaluated in the same pass as the signal raise. CLOSED by
PR-144 additionally: the LION's own side - class tables
0x96960/0x96958 (token LewZaria 0x9078b, factory 0x1fef2), its
receive front door 0x1e130 with generic tail 0x50d60 (PR-143's
item 4: the +0x10 slot DOES differ per family), its contact
entry +0x78 = 0x50f80 (15 tables carry it; the ladder's 0x3f1c0
is the same slot on the other side), the pair-probe 0x4bd32
that gives BOTH members their contact turn and unions their
outputs (the "collision caller's buffer" of PR-142/143 IS this
union), and the exhaustive live-set-write census above - the
AC_DRAB instance hunt is down to one link, not a search. The
player also rewrites its class-table pair at runtime
(0x1ec36..0x1ec3f) - named, not modelled, not needed. Reading
order in
the PR-144 Next entry; nothing was invented for any of it.

STATUS after PR-147 (the feed law, CLOSED): the 12 authored
BY_TY blocks run mode 4 - guard `[obj+0xbe] != 0`, feed exactly
100 per frame, threshold = the row word [rec+0x10] (the port
reads it from the 300/200 row lists), compare AFTER the feed,
one admit, remainder carried - so PR-109's operational rule
(T/100 iterations) is now RE-DERIVED from the engine with its
word identity pinned. The guard word is the PLAYER'S VERTICAL
INTENT: the frame loop hands the LION the 12-byte keyboard block
through its +0xcc contact method every frame (0x4aeca), the
keyboard writes the axes at +/-20 (Up -20 / Down +20, 0x584a9/
0x584c3), and a kind-4 contact node overrides it to +/-10
toward the node's y with a +-8 dead band (0x54b16..0x54b47) -
the same word the integrator uses as the v_y force (0x542cd).
A BY_TY row therefore advances only while the vertical intent
is held, and stops the instant it is not; the port's row model
matches the shape and lacks the guard and the magnitudes. What
remains is the implementation decision (PR-148: the input/guard
law with the engine's numbers, then the contact-answer route),
plus one counted oddity (node+0x1d vs the 0x1d allocation -
CLOSED by PR-149: kind-4 nodes are allocated 0x21, one 0x21
site image-wide, in the kind-4 chain itself).
STATUS after PR-148 (the guard, SHIPPED): that decision's first
half is CODE now - LL_CTRL_BY_TY + the parser mapping, the
player's vert_intent word (Up negative, y+ down, at the port's
validated intent scale; the 20-vs-10 keyboard constant tension
is NAMED at LL_PLAY_INTENT_FULL), and section 7's mode-4 feed
guard in 0x5441b's exact order (feed gated, compare not, carry
kept). The re-pinned PR-108 leg runs all three behaviours and
the 8/10 mutation battery pins the scope (extending the guard
to every non-walk mode starves the BY_TIME legs across the
whole selftest, first at the pre-existing walk/jump legs; the
BY_TIME guard 0x543f2 itself is a counted study, not copied).
NO vertical
motion was added: the 0x50252 "rise law" in the working notes
is dead code (zero references, both scan kinds; the lion's
frame-loop +0x74 is 0x50320 -> 0x54c00), and the y-force path
(0x542cd, [param+0x4] field map unread) is open item 1 - the
climb's rise is a contact-era question, not this PR's. M9 (the
intent sign) and M10 (keys-not-word) survive by construction
until the y-force path and the kind-4 override exist - the
reason the port carries a WORD. The ZARIA.01 climb blocker is
unchanged: the contact-answer route, PR-149.
STATUS after PR-149 (the blocker's spec, CLOSED AS A READING
GAP): the contact-answer route's every input is now pinned at
byte level - the node builders and their allocation sizes
(kind 1 at 0x53eb0/0x53f00, the answer-COPYING kind-2 builder
0x53f50, the kind-4 patch chain allocating 0x21), and the
ladder's answer 0x3f1c0 as an operand-pinned predicate tree
(the toucher's block vs the ladder's +0xc5 block, then its
70%-inset rect, then its full [self+0xb1..0xbd] rect - all
through the shared rect-intersection slot 0x23e00; the row-1
branch only FLAGS, row 0 never answers). [Corrections shipped
by PR-150: the order is FULL extent FIRST, then the 70%
probe; the inset is a SYMMETRIC central-70% cut; and the
row values are 1-BASED POSITIONS - position 1 FLAGS (a fresh
ladder's child list is empty, so it is inert in practice),
position 2 - the drop's `3,400` line - is the ONLY answer
row, positions >= 3 inert.] So PR-150 was a CODE
question, not a reading one - and it is now shipped, below.
The remaining NAMED in-game
blocker moved one link down the chain: the ladder's own live
set never receives K_GEN1 (bit 54) to admit its A_UP out of
`1,0,WAIT K_GEN1` - PR-149 adds two negatives there (the
+0xd0 receiver and the signal-record census) and the probe
leg must raise the signal directly until it resolves.
STATUS after PR-150 (the answer, SHIPPED): the ladder's +0x78
answer is port behaviour now - ladder_class / pt_in_blk /
blk_probe70 / ladder_answer_core / blk_of_obj /
ladder_answer_for in ll_level.c, called from BOTH play_collide
passes (solid and PR-92 non-solid) after the shape meet and
the push, merged straight into the lion's pulse set (no node
list, no merge-delay: counted open item) and read by the NEXT
iteration's transitions. The row gate is the 1-BASED row
position (the port's fi + 1, clamped like obj_shape_elem):
only the drop's SECOND line answers. The RUNG block is sourced
from the object's own current art bbox through the same
obj_shape_elem path the contact passes use; with no art/shape
it is the ctor-shaped 1x1 at the object, and the EDGE block is
modelled as that same 1x1 (the ctor never stores +0xc5 at all
- a named model choice, pinned by legs). The three PR-149
mis-reads (row index space, symmetric inset, P2/P3 order) were
corrected in place, not hidden. ZARIA.01 output is UNCHANGED
and pinned by legs: no CHECK bit gates the walk's list, and no
K_GEN1 raiser means the port's ladder never leaves A_UP - the
K_GEN1/WAIT arm remains THE named in-game blocker (fresh
leads: the 0x238e4 row-2 base method, the 0x52607/0x52610
clamp family). Test: ladder_answer_selftest wired into the mov
chain (probe law at every hand-pinned truncation edge, the
decision table, class scope, the ZARIA-shaped idle-row and
clamp legs); 10 mutations, ALL DETECTED, none survive. Next
code PR: the kind-4 contact node's +/-10 y-override - the
answer path can now PLANT a node.
STATUS after PR-151 (docs-only, and it corrects that sentence
before a line of kind-4 code was written): the premise is
WRONG - answers plant kind-2 nodes, and kind-4 nodes come
only from the per-class post-transition hook +0x90 (the
pipeline 0x53520 calls it right after the 0x52000 scan and
before the live-set clear - the port's evaluate-then-clear
ordering CONFIRMED; the +0x90 call sites are exactly two,
0x5357d/0x3edaf). ZARIA's hook (0x1e250) is a transition
REDIRECTOR (facing flip, a 30-tick [obj+0xad] bite, a
one-shot rewrite to row 9 when [obj+0x12a] arms - no live
ZARIA-path armer found, counted - transition CANCELs under
globals ds:0xcef64/0xcef68, then the base hook); the only
kind-4 supplier image-wide is the ENA hook 0x245b0, building
zones on the LION from the runtime array [obj+0x136] whose
only writer is the SADES hook 0x2e6a0 (so PR-108's "authored
attach data?" is ANSWERED NO). ZARIA.01 supplies no kind-4
contact - the y-override CODE pass waits for an ENA/SADES/
TUNGA-era controller; producer AND consumer are now fully
read, so that future PR is implementation. For the named
K_GEN1 blocker this pass found the strongest lead yet:
ZARIA's ctor links a COMPANION [obj+0x112] through
0x4dcc0(<level>, &0x497b0 query, seed ptr [ds:0x961fc] ->
0x1034c), and the +0x100 hand-off method's ENA variant
0x24440 raises K_GEN1 into the RESOLVED COMPANION's live
set - the first bit-54 raiser outside GEN row tails and
kind-9 messages (ZARIA's tables hold the BASE +0x100 with no
visible raise; the two call sites 0x50c2a/0x41b6a are the
named unread bodies). PR-129's "0x52cc0 ... all edx=9" was
also corrected (0x3f77e passes [self+0x4c]; its method is a
type-0x1c message receiver at a TUNGA-region table dword).
ZARIA.01 output unchanged (no code). Next: the K_GEN1 arm
reading pass with the companion-query grammar FIRST, then the
+0x100 bodies, then the 0x52cc0 sites; then the y-force/
0xcf400 block path, then the WAIT law. 9 open items counted.
STATUS after PR-152 (docs-only; the K_GEN1 chain now has SIX
NAMED LINKS): the hand-off that runs +0x100 is gated per
class by TWO getters now read - a +0x104 row-cursor window
(TLewZaria: true only at row POSITIONS 7-8; ENA 5-6; SADES
29-30; TUNGA2 7/9; the BASE class getter is `xor eax,eax` -
plain base classes NEVER hand off) - and a +0x108 constant
(ZARIA -6, SADES -4, base/ENA2 +1 via [obj+0x9d], TUNGA2
-14): the current row's transition-record array is scanned
for a record whose +0x4 word equals the constant 6 [VERIFIED
by PR-153 F1: row records are 72 bytes and field 2 IS at
record +0x4 - the layout question PR-152 named is CLOSED] -
and in
LEW.MOV the row lines AT POSITIONS 7-8 of A_WALK/A_ZABLOK
are exactly `4,6,GEN AC_TIME`/`5,6,GEN AC_TIME` and `4,6`/
`5,6` - the SAME field-2 = 6 gate PR-100 measured for every
A_WALK pose (the unmodelled WAIT row-gate field, PR-150 item
6, IS the hand-off match constant). The chain: those rows ->
scan fires -> +0x100 resolves the companion (a KEY-REGISTRY
resolution, [obj+0x112], no authored name - the 0x497b0/
0x4dcc0 "search" was mis-framed, corrected; 0x4dcc0 is the
sub-line factory reader, cf. ZWIERZ1's POCISK/PICT children)
-> ZARIA's +0x118 enqueues to the global message queue
[CORRECTED by PR-153 F2: per-object at [companion+0x10],
drained by 0x52b50 through the owner's +0x10 RECEIVER slot]
with the controller's facing -> the receiver 0x50d60's
type-9 arm 0x50df4 is PR-70's bit-54 (K_GEN1) setter [BYTE-
VERIFIED by PR-153 F5 - but the node is TYPE 5, ZARIA's own
receiver 0x1e130 (not 0x50d60) owns the type-5 arm, and the
66-site enqueue census shows NO static type-9: the LINK IS
VERIFIED, THE BRIDGE IS STILL OPEN - PR-153 F6] -> the
ladder's `A_UP(1)=K_GEN1` and the player's climb rows. THE
TWO DECIDING GAPS: the record layout (is [record+4] the row
line's second field?) and the queue consumer + the posted
node's type [BOTH CLOSED by PR-153 F1/F2: yes, and the con-
sumer is 0x52b50 -> the receiver slot, node type 5].
ZARIA.01 output unchanged (no code). Next: the
chain trace (record layout first, then the queue consumer),
then the WAIT law DOUBLY load-bearing now, then the y-force
path. 8 open items counted.

STATUS after PR-153 (docs-only; the chain trace ran and both
deciding gaps CLOSED, with the pass's biggest finding being a
CORRECTION): ZARIA's message receiver 0x1e130 IS the live
+0x10 slot of TLewZaria/TLewZariaB (PR-129's "UNREFERENCED -
dead override" was a listing BOUNDARY ARTIFACT - the entry
sits 2 bytes before the address the objdump listing shows; a
+0x10-slot census found the refs 0x96970/0x97980/0x97aa8).
THE RECORD LAYOUT: CLOSED - row records are 72 bytes (0x518f0
alloc, 0x517e0 grammar): +0x0 = field 1 = the frame selector,
+0x4 = field 2 = THE gate value, vtable at +0x44, row enter/-
exit = slots +0x1c/+0x20 called by 0x535a0 - so the hand-off
scan's [record+4] IS the authored second field (ZARIA matches
the 6 of the A_WALK/A_ZABLOK rows at positions 7-8 and noth-
ing else in the +0x104 window). THE QUEUE CONSUMER: CLOSED -
0x52b10 appends 16-byte nodes {+4 type, +8 payload, +c aux}
to a PER-OBJECT list at [obj+0x10] (the companion's, not a
global queue); 0x52b50 drains each node through [[obj+8]+0x10]
= the owner's receiver; the pump 0x52b80 = drain + tick +0x14
+ conditional +0x0(edx=2) on [obj+0x28], run by the container
loop 0x55380; the hand-off node is TYPE 5 with the facing
sign as payload. THE TYPE-5 RECEIVER ARM (live in ZARIA's own
0x1e130, siblings 0x27470/0x33edb in 2 other eras): payload
.+0 call with edx=2, [ds:0xa94f4]++ - the dead-pairing is
resolved, both sides LIVE and the counter is the one-shot
hand-off latch - then a 2-instruction stamp 0x44900 onto the
main-.des-built singleton [[ds:0xcef40]+0x38c] ([t+0x5c]=55,
[t+0x6c]=count; ctor 0x443b0 stores the class-table pointer
0x98278, in PR-150's counted 0x98274-gap family, referenced
from ZARIA lion code), then 0x52cc0(self,9) - which is NOT a
queue post but a HIT pipeline (slot +0x30 getter, +0x24 =
0x52a50 asking THE PLAYER at [[0xcef40]+0x394]+0x68, +0x28 =
0x52a90, 0x56600 -> [[self+8]+0x18] = bare `ret` in every
lion table, then [self+0x20]=1). FURTHER: the transition
pipeline has ONE entry - 0x536f0, the ladder-table tick - and
the LION tables' +0x14 0x550a0 calls it too (0x53520/0x535a0/
0x523f0 all single-callers inside it), slot order +0x8c feed
(the ladder's is the EMPTY-set stub 0x23a60) -> 0x523f0 row-
key OR -> +0x54 threshold nodes -> 0x53520 scan (context =
OWN live set) -> +0x90 hook -> 0x749a0 fill-0 clear; and
PR-70's bit-54 arm 0x50df4 is now BYTE-VERIFIED ([obj+0xad]
< 100 guard, fill 0, buffer[6] |= 0x40, 0x49580-OR, [obj+
0xad] += 32 - the same countdown field as ZARIA's hook -30
bite). BUT the bridge stays OPEN: drab.MOV/lina.MOV read
WHOLE carry ZERO GEN tokens, the ladder's +0x8c feed is the
empty stub, and the 66-site 0x52b10 census has NO static
type-9 - so nothing static raises bit 54 on the ladder; the
named candidates are the 4 dynamic-edx enqueue sites, the
0x38c stamp-object path and its class table, the ds:0xcf0ac
kind-table builders, and the WAIT-node getters (0xa11c8).
The gap is now exactly: who writes bit 54 (or the row-55
stamp) into the right object. ZARIA.01 output unchanged (no
code). Next: that bridge, in the PR-154 order named in NEXT_
STEPS; the WAIT row-gate CODE pass is next after it - field
2 = the hand-off constant CONFIRMED, doubly load-bearing.
8 new open items counted.

STATUS after PR-154 (docs-only; two bridge hypotheses CLOSED
NEGATIVE, the named K_GEN1 blocker is narrower and still open):
the row-55-stamp alternative is dead - the [[ds:0xcef40]+0x38c]
stamp object is an unclassed LIST of 8-byte two-dword records
(class 0x98278: free-path receiver + a plain copy verb; ctor
0x443b0 never gives it a class pair; built in the main-.des
'demolist' region, and a 4th stamp site sits inside the lion
base's hit-countdown method 0x50720) - bit 54 must come from
a live-set writer, and PR-70's type-9 arm 0x50df4 still has
NO static feeder: the 66-site enqueue census is now raw-
corrected (no dynamic types; one NEW static type-5 site at
0x330a7 onto a fresh 0x9d088 clone, and the [obj+0x12a] type-
0xc chain CLOSED at 0x1ef22). The hand-off's only functional
arm is now exactly: the kind-9 OBJECT (ds:0xcf0ac[9], a run-
time table) passed into the companion's +0x18 - the ladder's
0x3f0f0 consumes one for geometry; finding its builder or a
direct receiver call with edx=9 is THE named next step (the
PR-154 Next list, PR-155). And the WAIT row-gate CODE pass is
fully unblocked: node getters compose the authored row key
through the chain (0x457e0/0x45870/0x45970), so the engine
law is complete and only the port-side implementation is
missing. ZARIA.01 output unchanged (no code). 7 new open items
counted.

STATUS after PR-155 (docs-only; the named K_GEN1 blocker is
now a BOUNDED question, not a search): the engine's generic
cross-object signal law is the kind-0x10 magic-record ({0,
0x8138764, bit-index}) handled at 0x53772 - any reachable re-
ceiver gets ANY signal bit, no HP guard (so the ladder's A_UP
K_GEN1 row and the player's climb rows CAN be gated by design,
and PR-144's full-HP question is closed YES). The hand-off's
type-5 node is INERT on the base receiver chain; its only ac-
tion happens when the companion's receiver is a ZARIA-family
0x1e130 (kind-5..11 arm -> 0x52cc0 hit pipeline -> kind-9 ->
the LADDER's +0x18 = RUNG-RECT GEOMETRY + pump, never a live
set). The only static type-9 poster is the BAL touch (0x3d3fb,
PR-70's port path confirmed). The static K_GEN1 raiser set is
enumerated and era-classified; AC_DRAB (bit 10) gained its
first static poster (0x287a0, with the engine's OWN [[assoc]+
0x50][i]+4 == 6 field-gate - a direct cross-check of PR-152's
row-gate model, which strengthens the WAIT row-gate CODE pass
now). The ONE deciding question for PR-156: whether ds:0xcf0ac/
0xcf0b4 (kind tables) are ever written - if not, kind-dispatch
is dead code and kind-0x10 is the only signal path. ZARIA.01
output unchanged (no code). 10 new open items counted.

STATUS after PR-156 (docs-only; the K_GEN1 search is now a
CONSTRUCTION question, not a signal question): kind-dispatch
is DEAD CODE - nothing ever writes ds:0xcf0ac/0xcf0b4, so
every 0x56600 kind arm (including both hand-off kind-9 arms
and the ladder +0x18 rung-geometry call) never runs; the only
cross-object signal paths alive are the kind-0x10 magic-record
law, queue-typed messages, self-raisers, and the pair share.
For ZARIA.01 that means the authored K_GEN1 (the ladder's
`WAIT K_GEN1` row) must come from one of: a kind-0x10 record,
a queue message, the object's OWN self-raiser, or the pair
share - the DRABINKA/LINA ladder machinery (the only roster
entries with param lines, the only classes embedding the lad-
der pair) is the prime self-raiser candidate, exactly like the
MOVEPLAT 0x28af2 law PR-156 F2 read (state-4 + lion-rect-in-
minus-20 -> bit 54 into self). The ROW-GATE spec is now un-
blocked: the engine indexes [[obj+0x50] by [[obj]+0x5c] and
tests field +4 vs per-consumer constants (2/6/0x19) in FOUR
independent methods - PR-150 item 6's UNMODELLED gate field
becomes a parseable, feedable field once the row-builder and
ANI-STATE reader (PR-157) names the format. ZARIA.01 output
unchanged (no code). 7 new open items counted.

STATUS after PR-157 (docs-only; the row-gate is now a COM-
PLETE ENGINE SPEC, not a question): the tested word is the
current state's ROW CURSOR [record+4], written only by the
transition applier 0x535a2 (same-state: cursor := rowB; new
state: EXIT/ENTER verbs + [obj+0x5c] := recA); rows [[obj+
0x50] are per-state authored RECORDS bound BY NAME at place-
ment (slot +0x7C, 0x537f0) from the object's own parameter
entries through the ds:0xceff8 registry, which is fed by au-
thored 'seq NAME,...' lines - present in NO extracted file,
so ZARIA's controller rows exist only inside the encrypted
main data. The K_GEN1 bridge keeps its one unbound link for
the honest reason: the controller object whose A_WALK cursor
the hand-off machinery would test is never instantiated in
anything readable. For T5 the practical consequence: the WAIT
row-gate CODE pass can be written NOW (cursor + row-targeted
transitions + the (key,recA,rowB) transition-table law ground-
ed at 0x52000/0x535a2) with ZARIA.01 output required un-
changed - a pure model-correctness pass, no visible behavior
risk. PR-152 open item 7 and the [rec+4] identity are
CLOSED (cursor; the "authored 6" coincidence resolves to row
INDEX 6 = the `4,6,GEN AC_TIME` pose). 8 new open items.
[PR-158 F4: the cursor's 0/1-based nature is OPEN - if 1-
based (the (N)-verbatim + "0 = before any admit" + 0x300dd
zero-reset readings), consumer 6 names the `3,6` line and the
row-identity sentence shifts; the numeric coincidence stands
either way.] [RESOLVED by PR-159 F1/F3: the base IS the au-
thored 1-based (N) - the only non-zero writer copies it ver-
batim, rows[] binding scans names with inc-before-compare,
and 0 is exactly the clone-init value; consumer 6 = the `3,6`
line.]

STATUS after PR-158 (ledger recovered; the row-gate CODE
question CLOSED as a question, by inventory): PR-150's and
PR-151's full records are back in NEXT_STEPS.md (they had
been swallowed by old Next-swaps and survived only inside
their own commit blobs - the census recipe in the record is
the audit rule for every future swap). The ladder's answer
gate was re-read: 0x3f1dc reads [ecx+0x5c] with ecx = SELF,
so by PR-157's law it tests the STATE slot - 1 = the initial
state (FLAG branch, answers nothing), the K_GEN1-transition
target = the only answering state, >= 3 inert - and PR-150's
"1-based row position" attribution is corrected in the ledger,
the HANDOFF and the port's own comment, while EVERY ZARIA.01
leg and the port's fi-model survive both readings (the one
authored transition never fires). No new cursor machinery is
needed or shipped: o->fi IS the modelled cursor (set via
ll_mov_row_start, ll_level.c:835) and every cross-object
cursor reader belongs to a class the port never instantiates
at T5. One honest crack stays open and is now PR-159's first
read: WHO advances [[rec]+4] during ordinary row playback
(the applier only SETS it; 0x300dd resets it) - until that
owner is known the cursor's 0/1-based nature is unresolved,
PR-157 F4's row-identity arithmetic is flagged, and the lad-
der-gate fi-vs-state decision waits on the record-pool de-
code. [CLOSED by PR-159 F1/F3: no incrementer exists - the
writer law is a transition match (0x53616) or the clone/ctor
initialization (0x300e6, the 0x300dd window CORRECTED from a
"restart" to object creation); the base = the authored 1-
based (N), and rows[] binding is name-position-based and 1-
based; only the param-token mapping stays data-blocked.]
ZARIA.01 output: UNCHANGED (the ll_level.c edits are
comment/diagnostic-string only; full gates pass - 56 with
fixtures, 10 + the same 23 SKIPs without). 3 new open items
counted + 1 process rule; PR-150 item (6) closed.

STATUS after PR-159 (the row-cursor question CLOSED as a law,
and a new engine law counted): [[rec]+4] has NO incrementer -
its complete writer set is a transition match (0x53616, ver-
batim from the authored (N), skipped-when-equal at 0x53612)
or the CLONE/ctor initialization at 0x300d7-0x300e6 (the
"state restart" reading was wrong; it is object creation,
class pair 0x9c05c). Row progress lives in the threshold/
accumulator walk (PR-100/101) and never reads the cursor, so
the cursor's base IS the authored 1-based (N): PR-157 F4's
consumer-6 row resolves to the `3,6` line and PR-158 item 2
closes. The transition pipeline itself raises AC_TIME: self-
transition matches counted in [obj+0x9d] against WORD [state-
record+0xc] set live-set bit 9 (0x53626-0x5365c) - the port's
frame-list approximation was directionally right; the exact
gate waits on the [rec+0xc] authoring decode. [DECODED by
PR-160 F2/F3: the time= directive pair + a rand() roll at
ENTER; zero time= lines in ZARIA.01 keep every leg un-
changed; the exact roll-and-count law is fully specced for
a CODE pass.] The rows[] bind-
ing helper (0x53480/0x53440) PROVES the state-index space
1-based (rows[0]=NULL, init=first param entry): the ladder
gate's unit reading stands, and the fi-vs-state decision is
now purely data-blocked on the param-token map. The PR-152
hand-off scan is a ONE-SHOT cursor compare (0x50be2), which
corroborates the whole law; its base-class gate stays FALSE
at T5 so no port action is due. One tension stays honestly
open: the walk's threshold [entry+0x10] vs the transition-
entry layout - the [cls+0x18] body is PR-160's first read.
ZARIA.01 output: UNCHANGED (comment-only ll_level.c edits;
full gates pass - 56 with fixtures, 10 + the same 23 SKIPs
without). 5 new open items counted + 1 narrowed; PR-158 item
2 and the F4 base question closed.

STATUS after PR-160 (the AC_TIME question fully CLOSED with the
directive parser decode): the engine's state-block DIRECTIVE
layer is read end to end - time= is the authored min/max pair
that ENTER turns into a random AC_TIME match-threshold via the
Borland rand() LCG; seq= registers a block into the global
name pool and s= inserts a named-sequence row; control= is
enumerated verbatim (21 flags) and the parser cites source
file names. The record model is decoded: class tables (base
0xa25d4 / LADDER 0x97c9c), per-class record factories via the
+0x88 verb, ctor defaults (time_scale 1000 = 1x), and the
rows vector indexed BY THE ROW CURSOR - the walk verb, the
[entry+0x10] threshold gate on the [obj+0xa1] accumulator
(the pre-scale call is the frame-advance dispatcher that
FEEDS it - PR-100's mechanism, now named), and the PR-142
latch writes all sit in named verbs. ZARIA.01: authored texts
carry zero time= lines, so AC_TIME never self-raises there -
every ZARIA.01 leg stands; the port's frame-list approxima-
tion is now replaceable by the exact roll-and-count law (Next
names the fully-specced CODE pass). Honest gaps counted: the
row-line parser for plain rows (what fills rows[] from au-
thored `N,T` lines - the last piece of the row model and the
decider for the 21-byte layout and the WAIT-key question),
the walk-arm attribution, and derived ENTER overrides. ZARIA.
01 output: UNCHANGED (docs-only PR, nothing compiled). PR-159
items 2/3 close (half of 3), F4 closes; 3 new items counted.

STATUS after PR-161 (the row model is CLOSED end to end): the
row-line parser is found (param-entry verb +0x14 = 0x51eb0 via
driver 0x52b80) - row lines are ID,THRESHOLD pairs resolved
through the sequence pool into 0x18-byte wrapper entries appen-
ded to [rec+0x2e]; the walk verb is SINGLE-SHOT per tick (via
object-verb +0x44 at 0x53b18) and the image has NO row-cursor
incrementer - rows advance exactly and only by authored transi-
tion row-outputs (PR-159's writer law with a negative proof);
an unresolvable row ID means an EMPTY KEY that always passes
the intersection test - the WAIT law - so its threshold gates
the accumulator every tick and matches keep counting toward the
AC_TIME roll (PR-159 item-3 closed). The transition-vs-row la-
yout tension dissolved (two distinct arrays) and the two-VIEW
class tables (0xa25d4/0xa260c) explain every earlier slot dis-
crepancy. The placement ctor ENTERs the initial record at
0x539fb - the AC_TIME roll fires at spawn, the last detail the
specced CODE pass needed. ZARIA.01: rows register no pool names,
so the port's threshold-gated row model stands corroborated,
pending the pool-contents read (now the NAMED edge of the K_
GEN1 data blocker). ZARIA.01 output: UNCHANGED (docs-only PR,
nothing compiled). PR-159 item-(3) + PR-160 items (1)/(2)
closed; 4 new items counted; one earlier phrasing corrected.

STATUS after PR-162 (the row model is fully corrected and the
control= flag word is fully decoded): PR-161's "no incrementer
exists" was the wrong grep idiom - the helper verb 0x520c0
continues at 0x5211e into a row-step tail (step 0x521d4/0x521
d7, wrap+|=4 at 0x521c1-0x521c5, reverse 0x521a1-0x521b1 via
SEQ_REV bits paired with position sentinels, mode-6 skip), so
ORDINARY row progression is advance-on-threshold-pass through
the helper the walk calls on every paid row: the port's model
(obj_row_tick advancing fi on threshold passes; PR-100/101's
measured law) is now CORROBORATED end to end, while the tran-
sition applier's authored-row-output SETS stand unchanged.
Both walk arms reach the gate (conditions are key-merge book-
keeping only at this level) - every row's threshold gates pay-
ment+stepping in practice, so the practical WAIT law stands.
The COMPLETE 21-name flag map is verbatim now (window [rec+
0x12..+0x15]; BY_X has NO bit; SEQ_REV_X/Y = 0x10/0x20 con-
firming PR-95 exactly; composites CHECK_ALL=0x1c0, SOLID2=
0x20400, SEMI_SOLID=0x40400; the +0x15 bit-0x8 that PR-160
called face-flip is the ANTI_POCISK bit - PR-108's flip beha-
vior stands, its bit attribution reopened). The AC_TIME CODE
pass spec is UNCHANGED and is PR-163. 5 new open items, most
importantly the mechanism boundary: where the transition ap-
plier is invoked from (the port's per-row transition model de-
pends on it). ZARIA.01 output: UNCHANGED (docs-only, nothing
compiled). Baselines 71/72 held; ledger census run.

STATUS after PR-163 (THE AC_TIME LAW IS NOW PORT CODE, and
ZARIA.01 output is UNCHANGED): the time= pair lives as
time_lo/time_hi (parse_pair pairs single values exactly like
the engine scanf leaves max unset=min); obj_enter zeroes the
match counter and rolls gate = lo + (hi-lo)*rand/32767 with
the Borland LCG verbatim; obj_transition's SAME-STATE arm -
the port's copy of the engine's match arm at 0x53626-0x5365c -
counts matches and raises LL_SIG_AC_TIME (bit 9) exactly when
gate && count >= gate; no time= = gate 0 = never. ENTER runs
at all four state-change sites mirroring the engine: a LAZY
placement ENTER at the first scripted tick (the 0x539e6/0x539fb
ctor ENTER), obj_transition's change arm, the death force, and
the A_ATAK force. The unified ENTER is now stated IN THE CODE:
record-table slot +0x1c (0x52360) is ONE body - PR-107's row-
set half and PR-160's gate half - and the port splits it the
same way the applier reaches it (ll_mov_row_start + obj_enter).
CARRIED CODE COMMENTS FIXED: the row step 0x521d4, wrap+OR-4
at 0x521c1-0x521c5, reverse via SEQ_REV_X/Y 0x10/0x20, and
the brk || approximation is now labeled as exactly that (the
raise is additive). obj_actime_selftest adds FIVE hand-derived
legs (fresh-object LCG stream 0, 21468 pins the roll: gate
(2,50)=2, the single-value deterministic gate, lerp(1,10) at
r=21468 = exactly 6; the state change itself proves the raise,
since the consuming line is tested at brk=0; the death force
clears a live gate+count). MUTATION BATTERY 11 mutants: 9
killed; 2 SURVIVORS STATED - M10 cross-match counting (unob-
servable by construction: the change ENTER zeroes) and M11
(A_ATAK-force ENTER unreachable with current content). DIVER-
GENCE DOCUMENTED: per-object LCG state vs the engine's single
0x76bc6. 6 open items counted, the in-walk-visibility one
merged with PR-162's mechanism boundary = PR-164's read. Gates
ALL RUN: make + make win zero warnings, ll_level relinked,
mov/play selftests rc=0, verify-all BOTH modes unchanged (56
/ 10+23), no over-79 added, baselines 71/72 held. CI note:
PR-160/161's cancelled runs re-issued via gh run rerun; PR-
162's run confirmed green.

STATUS after PR-164 (the mechanism boundary is CLOSED, with a
ledger correction to PR-162): PR-105/106 ALREADY had the whole
chain - slot +0x14 (two views) -> wrappers (LION BASE 0x550a0:
zero [obj+0xe6]/[obj+0xea], call 0x536f0) -> pump 0x536f0 =
key getter/merge, WALK 0x523f0 at 0x5371c on the current re-
cord of [[obj+0x50]+[obj+0x5c]*4] (the object's live state
vector+index), LOOKUP 0x53520 (live-key copy + 0x52000 walk),
APPLY at 0x5374b, hooks around - and PR-162 item 5 re-opened
what was closed. Tonight's independent byte reads CONFIRM it
and add two facts. (1) The applier's ENTRY is 0x535a0 (four-
push prologue 51 56 57 55 at 0x535a0, own four-pop returns) -
"0x535a2" is the objdump misalignment, the one-byte trap class
again; three port comments cite it and ride the next code PR.
(2) The applier's arms CONFIRM PR-163's shipped law VERBATIM:
cross path zeroes [obj+0x9d] WITHOUT counting (M10 is engine-
wrong), runs the new record's +0x1c (unified ENTER+row-set -
re-confirming PR-163 F1) and writes [obj+0x5c]=idx (the state
change); same-index path counts even when the authored row is
already current (0x53614 je into the count arm - the WAIT-law
count confirmed), and the arm is EXACTLY [obj+0x9d]+=1; gate
= WORD [rec+0xc]; skip iff gate==0 or gate>count; else [obj+
0x61] |= 2 = live-set bit 9 = LL_SIG_AC_TIME. The port's ob-
ject-state model (o->sti/o->act_*) is now structurally GROUN-
DED ([obj+0x50]+[obj+0x5c] = the engine's state vector+index).
apply-before-walk vs walk-before-apply reduces to TWO unob-
servable-now differences (the vtable+0-word OR 4/8 consumer;
in-call raise visibility) = PR-163 item (1) narrowed; the port
shape stands. 4 new items (word consumer, 0x53520 arg plumbing,
[obj+0x95] reader, 31-vs-24 slot delta); PR-162 item 5 CLOSED
(bracketed in its record). ZARIA.01 output: UNCHANGED (docs-
only). Baselines 71/72 held; census run; PR-160's rerun green.

STATUS after PR-165 (the flag bits get their CONSUMERS, and
the largest one was hidden behind a WORD read): the exhaustive
byte-test census of [rec+0x12..+0x15] finds ONLY the SEQ_REV
pair (0x52158/0x52171 + inside ENTER at 0x52385/0x523a0 -
third corroboration of PR-95/162) and CHECK_LEW/ANI/ZW inside
the key builder - but the BY_* bits' real consumer is the
FEED DISPATCHER the record step calls at 0x52317: owner slot
+0xa0 (0x543a0) re-derives the current record, loads DWORD
[rec+0x12], and,7, five-way jump table 0x54380 - mode 0 = NO
FLAG = BY_X feeds [obj+0xa1] += abs([obj+0xca]) (PR-98's
|v_x| law VERBATIM), mode 1 = BY_Y feeds abs([obj+0xce]) (a
VERTICAL twin the port does not model), mode 2 = BY_TIME runs
a [[obj+0xa1]+0xc] deref + jle full-feed (open semantics -
the headline new item, touching the ladder-gate block), mode
3 = BY_TX is the guarded composite ([obj+0xba]) exactly as
PR-162's map read it, mode 4 = BY_TY adds a POOL-RUNTIME
constant rate [ds:0xcef40+0x388] (static 0, set at startup),
mode >=5 incl. FRAME_REV bit 3 = NO feed. FRAME_REV has zero
consumers anywhere; DANGER/CH_DIR still none - the port's
labels for those stay PORT-DERIVED semantics, now labeled as
such. NO port behaviour change indicated: the BY_* modes work
exactly as the port's feed models (the enum stays port-
internal). SECOND: THE KEY BUILDER IS RECORD SLOT +0x08
(0x52750 -> verbs +0x2c/+0x30 + owner getter +0xc8 -> flag
merge +0x34 = 0x52790, PR-162 item 4 CLOSED): CHECK_LEW/ANI/
ZW run POOL QUERIES (0x4bd30/0x4b890/0x4b870 over pools ds:
0xcef40/0xa94ec/0xcef44, 0x8000 CHECK_ALL_PLANE sub-mode)
ORed into the 13-byte key + count byte - CHECK_* rows ADD
KEY BITS, they do not gate. THIRD: DERIVED ENTER/LEAVE
OVERRIDES ARE WRAPPERS (0x4fef0/0x50040 tail-call the base
0x52360; 0x50060 wraps 0x52660) - the port's single obj_enter
core stands for derived classes. FOURTH: THE POOLS' STARTUP
REGISTRY (0x4fe02: 0x5c320/0x51120 with class ids + obfus-
cated names at 0x9209a.. - decode open) feeds the very glo-
bals the queries use - the pool-contents read is PR-166's
first target. One-byte listing trap found again (0x4b890 ->
0x4b88f). 6 new open items (BY_TIME deref semantics, pool
name decode, 0x4b500 body, +0x2c/+0x30 bodies, the 0x4fef0
pool side-effects, the second-view slots); PR-162 items 1
and 4 CLOSED with brackets. ZARIA.01 output: UNCHANGED
(docs-only). Baselines 71/72 held; census run; no code built.

STATUS after PR-166 (the pool subsystem is READ THROUGH and it
is the level's own placed objects, not an abstract sequence
store): the CHECK_* key queries hit CONTAINER-ATTACHED POOLS.
The loader names them at load by the PR-60 source keys - 'zwy
erzaki'/'animacje'/'ani_plat' comparisons at 0x4cd6c-0x4cdca
store the container's pool INDEX [container+0x40] into ds:0x
cef44/0xa94ec/0xa94e8, and the container object IS the pool
array base (its own cleanup uses the identical layout at 0x4c
e0a-0x4ce24; ds:0xcef40 at query time = the current contain-
er). QUERY BODIES CLOSED: 0x4b500 = chain-head walk ([poolOb
j+0x44] count, +0x58 heads, [entry+0x34] next), predicate v
erb +0x60, CLASS FILTER [entry+0x24]==[owner+0x24] with the
pushed 0x8000 CHECK_ALL_PLANE cross-class switch (confirmed
meaning), key-provider verb +0x78, OR-merge via 0x49580 into
the 13-byte key; 0x4bd30 (CHECK_LEW) = the OWNER's own list
at [obj+0x64], INTERSECT (0x495b0 idiom) + copy-out, no OR.
Class id field CONFIRMED at [obj+0x24]. Wrappers 0x4b870/0x4
b88f = stack-arg adapters (0x4b890 listing misaligned again)
. The STARTUP REGISTRY is fully transcribed - 14 type-0x14 p
ools, class ids 6,6,11,(11),7,3,5,12,2,9,8,10,11,(11), name
s 0x9201c-0x920e4, handles ds:0xcefbc..0xceff0 - and NONE o
f those handles is read by any code (negative): the registra
tion is startup plumbing; the QUERYED pools are the load-tim
e ones. Pool names: decode still open, negatives banked (no
 single-byte XOR works; pairs position-independent; block si
ts right after the readable LEVELS.DES keys + 'ANITAB'..'BE
RLO' factory-name table). NAME-KEYED ACCESS REAL: 0x4dcc0(ds
:0xcef40,name) -> 0x4df10 ('ANITAB' special case creates ty
pe 0xa5) -> factory clone path stores the result at [obj+0x1
20] (PR-104's prototype cache); the row resolver's result sl
ot ds:0xceff4 abuts the registry globals - ds:0xcef40..0xcef
f4 is ONE pool-singleton block, so PR-161's sequence pool an
d the container pools are ONE subsystem. ZARIA.01 rows still
 resolve NULL - the K_GEN1 data block STANDS but is precisely
 narrowed (the key-path pools need no names). SIDE FACTS: po
ol manager singleton at [container+0x394] (verb +0xd8 regist
ers, +0xe4); BY_TY feed rate = [container+0x388] (setter un
read - narrows PR-165 item 1); 0x4b600/0x4b6ba = the ani_pl
at pool position/velocity scan (platform-ground lead, caller
s 0x546d7/0x54719). 6 new open items (headline: WHO inserts
 into the container pools + the entry class carrying verbs +
0x60/+0x78 = PR-167; name decode; 0x4dcc0 contract; 0x4b600
; ds:0xcef58/0xcef40 writers; manager slots). PR-165 item 3
CLOSED, its F3/F5 corrected with brackets. ZARIA.01 output:
UNCHANGED (docs-only). Baselines 71/72 held; census run; no
code built.

STATUS after PR-167 (the pool subsystem's last leg closes at the
class-verb level: the pool entries ARE the game objects): the
walk's verbs are the OBJECT CLASS TABLE's own slots - 130/155
class tables carry 0x23b80 at +0x60 (`return [entry+0x2c]`:
membership = the word at [obj+0x2c]) and +0x78 is a three-
variant key-provider family (default 0x552e0 = clear a 13-byte
local, gate via 0x53ae0, gate via class verb +0x9c, build via
0x55230, emit EXACTLY 12 bytes + count byte). THE BASE OBJECT
CLASS TABLE IS AT 0xa2550 (71 slots): every generic verb the
PR-162..166 call-site readings predicted sits at its predicted
offset (class-id getter 0x23e00 at +0x08, the walk at +0x14,
ENTER 0x52360 at +0xa0/+0xd8, base LEAVE 0x52470 at +0xe4);
the 0x23bxx bodies are one-instruction accessors. THE ADDER IS
THE PER-CLASS VERB +0xd8 CALLED FROM FACTORY CHAINS AT LOAD -
no-op (0x40f90) for most classes, real bodies (0x3def0 family,
which PROPAGATES registration to related objects) for the
pooling ones: PR-60's placement-category model exists at the
VERB level. FIELD IDENTITIES PINNED from the ENTER body (ver-
batim 0x52361-0x523d0): [obj+0xa1] = the feed accumulator and
ENTER ZEROES IT (PR-163's gate/counter clear gains its engine
twin for the feed side); [obj+0x99] := 1 at every ENTER (the
writer of the long-carried sentinel); the SEQ_REV pair plus
BOTH row-cursor stores (reversed (row+8)-edx at 0x523c5, plain
edx at 0x523cb) - PR-162's row-cursor law CONFIRMED inside
ENTER. The +0x1c=0x52360 census = exactly 4 tables: PR-100's
two named views + 0xa2e9c/0xa2ed4, already in PR-100's 16-
table census - what remains is the view-SELECTION code, not a
missing class. One open half stays honest: the chain-link/
head STORE is still uncensored (constructors init +0x34=3; no
direct chain store found) - the real +0xd8 bodies are the
lead. 5 new items (chain store; 0x53ae0/+0x9c key internals -
where the 0x600000/0x800000 CHECK_LN constants MUST appear, or
PR-161's constants get re-derived; the base table's own +0x1c/
+0x20 overrides 0x52c70/0x52a00; the 25-class +0x60 minority;
[obj+0x99] reader semantics). PR-166 item 1 CLOSED with a
bracket; its F3 bracket strengthened. ZARIA.01 output: UN-
CHANGED (docs-only). Baselines 71/72 held; census run; no
code built.

STATUS after PR-168 (the feed law is CLOSED end to end, and a
treasure of misalignments falls): the three "distinct" BY_*
feed arms are ONE arms - modes 2/3/4 are byte-identical feeds
of [container+0x388] into the accumulator [obj+0xa1] (BY_TX/
BY_TY pre-guarded by cmp [obj+0xba]/[obj+0xbe],0;je), and
the long-quoted "[obj+0xa1]+0xc dereference, cmp +0xc, jle
full-feed" NEVER EXISTED: one-byte objdump split of `mov eax,
[eax+0x388]` at 0x543fb (seventh listing-trap member). THE
RATE IS A LITERAL 100/TICK (sole writer 0x4c8b7 `mov [x+
0x388],0x64` at container init). THE GUARDS ARE THE STATE'S
OWN MOTION PARAMETERS: [obj+0xba]/[obj+0xbe] = PR-98's physi-
cs fields, loaded by the gate 0x549f0 from the param block
[obj+0xd2] words 0/1, clamped by the 0x54a57-0x54b47 family
(literals 10/-10/0 against param-block compares), zeroed at
0x54e2e/0x54e38 - so BY_TX/BY_TY = the constant feed while
authored motion is live. The consumer side re-confirms PR-
98/162's accumulator model as the engine's (read at 0x52323,
written at 0x52118, zeroed by ENTER, fed by the dispatcher).
KEY-BIT MECHANISM FOUND: the second main key-provider family
(+0x78 = 0x54860 in 99 tables, paired with +0x9c = 0x549f0)
turns the PARAM SIGNS into key bits (`cmp [obj+0xbe],0; jge
-> or BYTE [key+4],0x20`, jle -> 0x40, plus a 32-bucket bit-
map loop - full map open, and STILL the last place the
0x600000/0x800000 CHECK_LN constants can live); 0x54a24 com-
pares [paramblk+0xc] against [obj+0x6d] the FACING - the
flip-bit question finally has a named lead, and the port's
DANGER/CH_DIR behaviours now have a named engine counterpart
(param-sign key bits, not flag bits). The ladder-gate block
NARROWS again: with the deref hypothesis dead, the gate is
PURELY fi-vs-state parameter selection (still data-blocked -
do NOT touch the fi gate). One narrow audit opened: whether
the port's BY_TIME row-gating is observationally equivalent
to a constant-100/tick feed (its fixtures have '1,100/2,100'
rows). 4 new items (the audit; the 0x54860 full map; the
clamp family; +0x9c variants). PR-165 item (1) CLOSED with
its F2 text corrected in place; PR-166's carried mention
closed. ZARIA.01 output: UNCHANGED (docs-only). Baselines
71/72 held; census run; no code built.

STATUS after PR-169 (the predicate gets its producer; the
chain hunt turns out to be a VERB-DISPATCH hunt): the member-
ship field [obj+0x2c] is written by the generic accessor
0x23b70 - `call the class verb +0x08; mov [obj+0x2c],eax` -
so the 130-copy predicate 0x23b80 is a NULL-TEST of a COMPU-
TED class-verb product, not a state flag (PR-167 F2a bracket-
corrected). The base +0x08 verb (0x23e00) is a bounds-contain
ment compare of the object's [0]/+4/+8 rect - PR-162's rect-
angle identity re-confirmed as a live verb. THE CHAIN-INSERT
CENSUS IS EXHAUSTIVE AND NEGATIVE for displacement stores (a
full-pattern sweep of +0x34/+0x58 direct/indexed/lea forms):
the container's heads-array alloc is the only +0x58 store;
+0x34 stores are constructors, non-object structs, and class-
table installs at [obj+0x64]. Yet chains are walked and prov-
ably allocated - because THIS ENGINE LINKS THROUGH VERBS:
0x53db0 calls a neighbour's verb +0x14 with edx=2 (its LEAVE)
then relinks [x+0x54]. PR-170 = the pool manager's method
table ([[ds:0xcef40]+0x394]+0x8) and the 0x5c320 pool-object
class - where list maintenance actually lives. THE +0x60 MI-
NORITY IS MAPPED (PR-167 item 4 closed): 12 of the 25 are
constant-true `return 1` class families (always matchable,
never pooled - the 0x26xxx ENA/KADMAR cluster among them);
the rest conditional (carried). The 0x507e0 +0xd8 body is a
13-way jump managing a global counter ds:0xa94f0 clamped to
1..9 - registration verbs count, they do not chain: the two
mechanisms are separate (the 14 startup type-0x14 pools are
name-registered data pools = PR-161's family). 3 new items
(manager verbs; the [obj+0x2c] producer contract - 0x23b70 is
itself only reachable as a verb body; the conditional mino-
rity) + PR-167 item (1) re-scoped with a bracket. ZARIA.01
output: UNCHANGED (docs-only). Baselines 71/72 held; census
run; no code built.

STATUS after PR-170 (the pool "manager" loses its mystery - it
IS the level controller): [container+0x394] is written by the
container's own construction, nine sequential level-ctor at-
tempts at 0x4de22-0x4dea4 (one per PR-104 level family). The
TLewZaria ctor 0x1fef0 compares the name, builds via the sha-
red 0x5c320(0x12e)+0x1e0a0 and installs TWO class tables
(0x96960/0x96958; the two-table layout of PR-95) - the pool
manager is an ordinary game object, the level controller. The
mythical "+0x68 pool-handle getter" = 0x23900 `add eax,0xd6`
- a pointer to the object's own +0xd6 pair, which the ctor's
reset block (0x54e02) fills as a POSITION SNAPSHOT ([obj+0]/
+4 copy) and initializes the whole motion state (+0xad := 100,
+b1 := 22 = PR-98's defaults!). Two generics pinned in pass-
ing: 0x23920 = current-record flag getter (the [[obj+0x50]+
[obj+0x5c]*4]+0x12 idiom as a REUSABLE VERB), and 0x238f0 =
the boolean membership writer for [obj+0x2c] - PR-169's mem-
bership field has two producer shapes (computed handle via
0x23b70, boolean via 0x238f0). THE JOIN IS A HANDSHAKE: con-
troller second-table +0xd8 = 0x50ed0 (guard: [obj+0x11a] not
6/7, [obj+0x126] clear, both verbs +0x40 pass; captures the
participant's +0xc id and +0x70 position into [ctl+0x10a]/+
0x10e) then calls the game-class +0xe4 (0x50300: sets the 30-
tick busy field [obj+0x102], read by 0x502f0) - registration
counts/times out, it does NOT chain, exactly as PR-169 fore-
cast. The walk loop is now verbatim-pinned (count +0x44, head
slot [[x]+0x58], next [entry+0x34], predicate/class-filter/
self-skip/order as PR-166 said) - the law stands; the chain-
STORE hunt is re-pointed, not closed: the verb +0x10 family
(the 0x54ed6 call with the 4-dword 0/0x1b arg block) and the
shared ctor 0x1e0a0 are the remaining candidates, and they
sit INSIDE the key-builder code - so PR-171 merges the key-
bit map (PR-168 item 2, the last place for the 0x600000/
0x800000 CHECK_LN constants) with the +0x10 census. New li-
fetime items: the +0xd6 snapshot readers (0x54efa delta math
vs [obj+0x89]/[obj+0xda]) and the +0x102 30-tick timeout (a
CONTACT-work lead). 1 bracket: PR-169's manager-sentence cor-
rected. ZARIA.01 output: UNCHANGED (docs-only). Baselines
71/72 held; census run; no code built.

STATUS after PR-171 (the key law is complete at the bit level,
and three roles fall): THE MAIN KEY BUILDER IS AT 0x54912
(ninth one-byte-trap member - PR-168's "0x54900" was inte-
rior; byte-level proof: padding+ret at 0x54905, fresh pro-
logue at 0x54912). COMPLETE BIT MAP: [obj+0xa5] state-flag
bits -> key[+4]|0x80, key[+5]|0x01, key[+6]|0x20; nonzero
motion param [obj+0xba] -> key[+4]|0x08 or |0x10 selected by
status bit [obj+0xb6]&0x10 (the `sete; add al,0x23; idiv 32`
one-index "bitmap" idiom); param-sign bits key+4|0x20/|0x40
ONLY when [obj+0xba]==0 (key+4 = 0x60 exactly at zero). THE
99-TABLE +0x78 VALUE 0x54860 IS A KEY CONSUMER, NOT A PRO-
VIDER: it decays [obj+0xc6] by -2/3 (idiv) when the walked
key bytes say so - and those counters [obj+0xc2]/[obj+0xc6]
are SET/ADDED/ZEROED by the CONTROL-MESSAGE verb +0x10 =
0x54560 (87/155 tables; switch on [block+4] id: facing
writes id<=5, counter arms, status-bit ORs [obj+0xb6]|4/8):
messages load counters, the key walk decays them - one closed
loop. PR-170's +0x10 chain-store lead is DEAD but paid: its
0x54eb2 caller sends msg {0,0x1b,0,0} then RE-ANCHORS posi-
tion from the [obj+0xd6] snapshot pair (pos := arg - snapshot
+ [obj+0x89]/[obj+0xda] - the ride/re-anchor law; PR-170
item (2) first half ANSWERED). THE "CLAMP" FAMILY IS A DEAD-
ZONE POSITION CONTROLLER (PR-168 item 3 CLOSED): param mode
[pb+4] (1=x/4=y), threshold [pb+0xc] vs facing setne*15 -
the long-carried flip-bit test 0x54a24 is now NAMED - and a
+-5 window vs the snapshot forces the motion params to 10/-
10/0. DECISIVE NEGATIVE: 0x600000/0x800000 exist ONLY as
byte-table misalignment artifacts - no code immediate uses
them: PR-161's CHECK_LN/BASE_LN "constants" MUST be re-de-
rived (labeled hypothesis: key-BYTE sequences 0x60/0x80 in a
hex-dump reading = exactly F1's bit map). NEW MYSTERY, TOP
LEAD: the 99 family-B classes have NO statically-referenced
key builder - 0x54912 has zero table slots, zero calls, zero
branches (triple census). POOL LOAD PATH READ TO ITS END AND
ENDS EMPTY: heads array allocated+zeroed at pool-slot assign-
ment ([poolObj+0x58] = 0x76a64 buffer, the 0x4cdf8 store
CONFIRMED as alloc - an awk range had hidden it), [poolObj+
0x44] := count, [c+0x40] increments per name group (SLOT
COUNTER - PR-166 F2 refined); runtime insert remains the one
open chain-store item (third re-point: +0x10 minority bodies
0x3dcb0 x27, unread msg arms 0x5460e/0x5463a, the family-B
producer hunt). 0x52070 RULED OUT (ENTER preamble). 2 new
items (family-B reachability; msg-id map + [obj+0x102] read-
er); CLOSED with brackets: PR-168 items (2) and (3); PR-170
item (2) half; 5 brackets total. ZARIA.01 output: UNCHANGED
(docs-only). Baselines 71/72 held; census run; no code built.

STATUS after PR-172 (a real BY_TIME divergence found and fixed
- the audit PR became a code PR): THE FEED AND GATE LAWS WERE
ALREADY RIGHT (100/iteration, hold-while-below, subtract-carry);
what the port lacked is the engine's ZEROING of its one feed
register [obj+0xa1] at EVERY state change - the applier (entry
0x535a0) calls the new record's ENTER at [[rec+0x44]+0x1c]
(the class tables install the 0x4fef0 wrapper there; its body
calls the row-set method 0x52360 at 0x4ff6b - a call, not the
tail call PR-170 said - and 0x5236b writes [obj+0xa1] := 0),
while the same-state match arm writes rec+0x4 inline (0x53616)
and calls no ENTER, so a match CARRIES the register. Row-write
and zeroing are ONE method; registers are fresh at every state
entry. OBSERVABILITY PROVEN ON AUTHORED DATA: seven of the
twelve distinct plain-BY_TIME gates in ZARIA.01 (1/40/75/80/
120/150/225 across 1128 rows) are not multiples of 100 - mid-
row exits leave non-zero remainders. The port zeroed facc but
not row_t at its applier arms; `o->row_t = 0;` now ships at
BOTH (keyed transition + forced A_DEAD), and the row_t/facc
split remains observationally identical to the engine's single
register (mode-fixed feed within a state + fresh at entry).
NEW TEST obj_rowzero_selftest (hand-derived, gates 70/240/120/
150): five targeted mutants killed at their exact legs (no-
zero-at-transition, zero-at-match, no-zero-at-death, no-sub-
tract-remainder, zero-at-wrap); one honest survivor (M6, zero
row_rem too - the ms carry has no engine counterpart and is
always 0 at an applier in this fixture). THE PR-164 RIDE IS
PAID (0x535a2 -> 0x535a0 at the three comment sites). CLOSED:
PR-168 item (1) (the BY_TIME audit). NEW ITEM: the BY_TY mode
feed is unmodelled (4 shipped states; the 0x543a0 mode arm is
the lead). UPDATED: the self-target row item (engine writes
the authored row on every same-state match - the object path
counts but never re-seeks; candidate code PR after the entry-
token census). ZARIA.01 output: the pinned frame comparisons
ALL pass UNCHANGED with the fix in. Gates: zero warnings both
platforms, verify-all 56 (ok) / 10 (ok)+23 SKIP rc=0 both
modes; baselines 71/72 held.

STATUS after PR-173 (the KEY-LITERAL re-derivation CLOSED the
literal hunt and UNIFIED the key with the signal set): the
0x919ec block is fully dumped — 73 NUL names ending at 0x91c68
(PR-88's 0x91c40 was short): 4 non-bit node-op specials, then
EXACTLY 68 signal names in the port LL_SIG_* enum's exact
order, then the 'AC_EV_MAX' sentinel; bit N = block index N+4,
machine-verified 68/68 against the port enum AND the port's
name->enum token table (complete, zero mismatches — PR-88's
table was sampled, now total). KEY = TEventSet (3 dwords +
count at +0xc; 0x5533c stores it, 0x49580 ORs the dwords):
row keys, condition masks, the live set and built keys are ONE
vocabulary. The pump 0x536f0 verbatim: class verb +0x8c = the
self key GETTER, its key ORed into the live set [obj+0x60]
before the 0x523f0 chain-walk merge — transition matching
tests live ∪ getter ∪ chain-key. Slot censuses (marker
+0x60=0x23b80, 178 tables): +0x8c holds 0x54910 x115 — PR-
171's "0x54912, zero table slots" was the listing trap plus a
search for the mis-parsed value (bytes at 0x54910 = 53 51 56
57, the prologue itself; tables store 0x54910) — plus the
base 0x52750 (PR-165's CHECK_LEW/ANI/ZW pool-query merge,
slot-identified), 0x23a60 x38, 0x23a20 = &[obj+0x75] (a
SECOND 13-byte set beside the live set), TLewZaria's 0x504e0;
the 99 family-B tables pair 0x54860/+0x9c=0x549f0 EXACTLY
99/99 and carry 0x23870 = bool [obj+0xa9]!=0 at +0x8c — a
SECOND contract, call-sites open. 0x54910 DECODED through the
block: [obj+0xa5] bits -> K_FIRE/K_ALT/K_JUMP; [obj+0xba] ->
K_BKWD/K_FWD; [obj+0xbe] sign -> K_UP/K_DOWN (0x60 = idle
both) — THE ENGINE SYNTHESIZES ITS INPUT-KEY VOCABULARY FROM
MOTION STATE: K_* tokens are AI-addressable motion tests. The
strict authored-data census: ZARIA.01's K-input-family lines
(118) live ONLY in the two LEW (lion) scripts — the port's
player path feeds those from real input, and no ZARIA.01
object path reaches any synthesized bit: NO port change (the
obj_cond_true default-false comment stays honest). The
0x600000/0x800000 "CHECK_LN constants" are CLOSED as pure
ledger artifacts: git-proven zero occurrences in PR-161, first
appearance PR-166's Next unrecorded; the REAL flag->key
bridge is named (CHECK_LEW/ANI/ZW pool queries at 0x52750;
ANTI_POCISK 0x80000 / DANGER 0x2000 tests in 0x55230 — the
entry-key language is relations: contact via 0x53ae0, push-
resistance, danger, [x+0xad] ordinals, always ORing AC_ENEMY).
5 new counted items (+0x8c contracts' call-sites; [obj+0xa9]/
[obj+0xad]/[obj+0x75] readers; per-class +0x78 singleton
family incl. slot-identified 0x28af0 = the bit-54 raiser;
KADMAR camel K_* reachability at its stage; 0x504e0/0x551f0).
ZARIA.01 output: nothing executed differently (comment-only
edit in ll_play.h). Gates: zero warnings both platforms,
ll_level re-linked, mov-selftest rc=0; baselines 71/72 held.

STATUS after PR-174 (the SAME-STATE ROW SEEK shipped for the
object path): obj_transition's same-state arm now opens as the
engine's does - it writes the authored start row into the cur-
rent state's row counter BEFORE the PR-163 count legs (0x5360f-
0x53616: store only when it differs; the guard's other half is
the +0xac verb call, unread, counted), calls no ENTER, and the
feed register stays carried (PR-172's law, mutant M7 pins it
again). The mapping is the shared ll_mov_row_start - the fourth
call site of the one rule (N>=2 -> N-1; "(1)"/absent/over-long
-> the top, all already settled for this path by PR-107). OBSER-
VABILITY (census, C tool linking ported/ll_play.c - PR-107's
method, numbers reproduce PR-107 EXACTLY: 2767 lines, 92 absent
+ 24 "(1)" + 101 seeks, 12 over-range): the 101 split LION 77 /
SCENE 24 (KADMAR.07 19, KADMAR2 3, LONA.05 2); ZARIA.01 has
ALL 20 in the lion's ZAR_LEW/ZAR_LEWPL and NOT ONE self-target
line in any ZARIA.01 scene script - shipped ZARIA.01 object out-
put is provably unchanged, and the verify-all fixtures (56 ok,
byte-identical) confirm it. The fix is law-completion: it is
what makes the 24 KADMAR/LONA-class lines behave at their T6+
stages. TEST: obj_rowseek_selftest, 7 hand-derived legs (seek
lands and is the LIVE row; empty token re-seeks to the top from
row 3; "(1)" = top; over-long clamp; row-write-only with the
carried 150 opening row 2's gate from 250; cross-state PR-107
regression through A_TWO's (4) line). MUTATIONS: 6/7 killed -
no-write, raw-store, skip-empty, no-clamp, gate-on-AC_TIME,
zero-register (M2/M7 also die in PR-172's obj_rowzero_selftest
- cross-coverage); the SURVIVOR is the engine's own equality
guard (observationally equivalent; the unmodelled half is +0xac
- counted item 1). THE PLAYER SIDE STAYS OPEN with its full
data (item 2): the 20 lion lines (A_BORRING(10), A_DEAD(33) x2,
A_FALL_DOWN(3), A_FG_PACKA(17), A_FIGHT(20), A_JUMP(11),
A_JUMP_S(9), A_KILOF(13)/(25)) key on AC_TIME/AC_DUP/|K_ANY/
K_FIRE,K_GEN2/AC_RECT,K_GEN1 - the player path models none of
those signal legs, and lion row changes re-open T2/T3 animation
claims, so PR-86's keep-running stays shipped until the signal-
side model + fresh frames run (the applier is class-independent
- PR-107's fixed record table 0xa25d4 - so the engine DOES re-
seek the lion's rows: this is a real divergence, bounded and
listed, not hand-waved). ZARIA.01 output: nothing executed dif-
ferently. Gates: zero warnings both platforms, ll_level re-
linked, mov-selftest rc=0 with the new test in the chain, ver-
ify-all rc=0 both ways (56 ok; 10 ok + the same 23 named SKIPs);
baselines 71/72 held.

STATUS after PR-175 (the KEY-FOLLOW-UP INTERLOCK PASS; docs
reading - ZERO port files touched, so T5's shipped behaviour
is EXACTLY PR-174's, and ZARIA.01 output is unchanged by
construction): what the pass settles FOR the port's model:
(1) the engine's per-object state-step verb is slot +0x14,
and the key pump 0x536f0 runs for exactly 105 of the 178
marker-anchored classes through it (0x550a0 x79 wrapper + the
26 direct + nothing else; 0x550b4 is the only direct pump
call in the image) - the port's KEY/AC_* vocabulary has a
named, counted producer set, and LION (0x96960, +0x14 =
0x550a0) is pumped every tick exactly as PR-153 assumed;
(2) the 99 "family-B" tables are a SEPARATE, passive lineage
(+0x14 = 0x23920 returns the record's control-flags word;
their +0x60 is never the 0x23b80 marker) - PR-173's joint
presentation corrected, so no port reading of the pump/boolean
"two contracts" was ever at stake; (3) the +0xac verb the
PR-174 equality guard skips is a live-KEY-SET extension hook
(edx = &[obj+0x60], never the row) - PR-174's shipped row-
seek model stands unchanged, its one unmodelled half re-
scoped and partially read (0x546a0 head: pool-query + live-
set merge + [obj+0xa9] RMW); (4) [obj+0xad] IS the energy
counter PR-89 named (0x5516a compares self vs other; the
winning branch builds the AC_ENEMY_ATC key, the losing one
AC_ENEMY) - the port's energy field and the engine's key-
building relation verb are the SAME field, which grounds the
port's atc windows (PR-89/obj_rowzero law) on real engine
semantics; (5) [obj+0xa9]'s site map is complete (msg-id
write 0x54624, pool-query feed 0x4b64f, animal family, zero-
ers; semantics open), [obj+0x75] is pointer-only through the
0x23a20 getter - and 0x23a20 is slot +0x4c's default (the
carried FLAG-getter item, slot-identified). Twelve LION-
shape controller classes (the 0x504e0 getter) are census-
listed for PR-176. Nothing claimed here changes a shipped
behaviour: all three carried items it closes were model-
blocking questions, and every closure says so. Gates: docs-
only (PR-138..171 precedent); added lines <= 79; baselines
71/72 held; PR-174's binary+verify state stands (56 ok / 10
ok + 23 named SKIPs).

STATUS after PR-176 (the CONTROLLER-GETTER pass; docs
reading - ZERO port lines changed, the ZARIA.01 output image
is byte-identical to PR-175's, and the build state remains
PR-174's green binary with verify-all rc=0 both ways).

What this pass GROUNDS for T5 (no shipped behaviour changed,
so no new test was required - the reading's payoffs are
confirmations and exclusions):
(1) The controller getter CONFIRMS the key model end-to-end:
    its jump table builds bits 41..52 and those are EXACTLY
    K_F1..K_F12 in PR-173 F5's name block and in the port's
    LL_SIG_* enum - a second, independent confirmation of
    the bit->name law (on top of the 68/68 row census).
(2) The getter = signal-ordinal key OR the 0x54910 motion
    key, merged via 0x49580 into the standard +0x8c buffer
    the pump 0x536f0 already reads - the port's pump-side
    model needs no change for controllers.
(3) THE TWELVE 0x504e0-carriers are IDENTIFIED: the six TLew
    stage-lion classes (TLewZaria -> 0x96960 - THE ZARIA.01
    lion's class), five per-stage death-module classes, and
    the shared parent 0x97970. The lion object the port
    models belongs to a named, enumerated family - no unknown
    class remains among the controller-getter tables.
(4) The KADMAR camel rows are EXCLUDED from the controller
    lineage (CAMEL -> 0x98278, a decor class in a third
    table format with no +0x14 pump and no +0x8c getter in
    any read map) - the camel K_* row mystery narrows to one
    decor-class format question, still T6+ data-blocked; the
    port's ZARIA.01 output is unaffected either way.
Five new open items are counted in the record (five unnamed
TLew classes; [obj+0xee] >= 10; the queue producers/verbs;
the button-bit map; the switch-array source + decor format);
two closed (PR-175 F6 twelve-identity; PR-173 item (e)).
Gates: docs-only (PR-138..171, PR-175 precedent); added lines
<= 79; baselines 71/72 held; PR-174's binary+verify state
stands (56 ok / 10 ok + 23 named SKIPs).

STATUS after PR-177 (the RELATIONSHIP-KEY PROVIDER pass;
docs reading - ZERO port lines changed, the ZARIA.01 output
image is byte-identical to PR-176's, and the build state is
the OWNER GATE of 2026-11-19: make clean + parallel
verify-all rc=0 both ways, 52 (ok) fixtures / 10 (ok) +
18 named SKIPs fixture-free).

What this pass GROUNDS for T5 (nothing shipped changed, so
no new test was required; the payoff is that every shipped
raise rule now has its engine counterpart named):
(1) THE GENERATED SIGNALS ZARIA.01 ACTUALLY USES - K_GEN1
    (KAMIEN1-3, BALONIK, DRABINKA, KATAPULT, KORONA),
    AC_ENEMY_ATC/AC_ENEMY, AC_BLOK (ZAR_LEW) - all flow
    through the class +0x78 provider slots, driven by the
    base +0x8c getter 0x52750 (gated by [rec+0x12]&0x8000,
    new fact tying the record-flag law of PR-159/160 to
    signal raising), the pump's +0xac hook (LION's 0x546a0
    calls the bounds test 0x4b430 and the pool query
    0x4b600), and child aggregation 0x553e0.
(2) PR-175's energy/AC_ENEMY reading is REPRODUCED inside
    the providers themselves (AC_ENEMY always ORed,
    AC_ENEMY_ATC on the contact arm) - the key law holds
    end to end, now with the raising code named.
(3) The census is CORRECTED (9 shared x122 + 56 singletons
    inside the 178, disjoint from the 99 passive lineage -
    the THIRD conflation of those two populations closed by
    bracket), so any future claim about "+0x78 families"
    has exact numbers.
(4) The decor tables from the camel item (0x98278/0x98274)
    are REFERENCED INSIDE the provider code (0x3d49d,
    0x28b47, 0x4b441) - the camel reachability item
    updates again, still data-blocked, now with concrete
    code contact points.
Five new open items are counted in the record (relation-
helper bodies; unread provider tails; the other 55
singletons; the array/field producers; entry-boundary
honesty); one closed (PR-173 item (3)).
Gates: docs-only (PR-138..171, PR-175, PR-176 precedent);
added lines <= 79; baselines 71/71 held; the Windows half
stays in the end pass (rule 8).

STATUS after PR-178 (the BY_TY FEED pass; docs reading -
ZERO port lines changed, the ZARIA.01 output image is byte-
identical to PR-177's, and the build state is the OWNER
GATE of 2026-11-19 measured on 7b4d736: make clean +
parallel verify-all rc=0 both ways, 52 (ok) fixtures /
10 (ok) + 18 named SKIPs fixture-free; only docs have
moved since).

What this pass GROUNDS for T5 (nothing shipped changed):
(1) THE HIGHEST-CARRIED PORT-SIDE QUESTION IN ZARIA.01 IS
    CLOSED AS NO GAP: the 4 BY_TY states ([A_WSP]/
    [A_WSP_LIN] of ZAR_LEW/ZAR_LEWPL) are fed by the SAME
    constant 100 the port feeds (byte-exact at 0x5441b/
    0x543fb), and the port's player path has modelled the
    guard since PR-148. The object path needs no BY_TY arm:
    no authored ZARIA.01 non-lion state uses the mode.
    The PR-148 intent-proxy guard STANDS unchanged - this
    pass found no data contradicting it; what would pin
    the real gate is the buffer filler (counted item 1).
(2) THE FEED UNITS ARE PINNED: walking states are fed by
    |[obj+0xc2/c6]|/100 - 1/100-pixel units, exactly the
    port's facc law; time modes by the 100-per-iteration
    constant, exactly the port's row_t law. The PR-109
    unit measurement (the on-screen 200/300 ladder-clip
    rows) now has its engine mechanism named end to end.
(3) THE ZARIA.01 LION CLASS IS TABLE 0x96960 (of the 15,
    all sharing +0x14 = 0x550a0, +0xac = 0x546a0, +0xa0 =
    0x543a0, +0x78 = 0x50f80) - every future ZARIA.01
    claim about "the lion's code" now has an address.
(4) The climb gate's real meaning is a VERTICAL WINDOW
    against the attach object ([obj+0xd2] kind-4, [obj+
    0xbe] = +10/-10/0 vs [d2]+0x1d) - if a future pass
    pins the buffer filler, the object-path guard becomes
    a code-PR candidate with a hand-derived test.
Five new open items are counted (buffer filler + [obj+
0x106]; the [obj+0x11a] mode field; 0x549f0 tails + [obj+
0x71] readers; the +0xa0 census oddities; 0x55070's chain);
one closed (PR-172 item (1)).
Gates: docs-only (PR-138..177 precedent); added lines <= 79;
baselines 71/71 held; the Windows half stays in the end
pass (rule 8).

**T6. Level flow.** `end_level` directive, `AC_LEAVE`, the vulture ride of
ZARIA I, the map -> level -> map loop, TUNGA I <-> II exchange
(`AC_XCHG_LEVEL`), lives/energy persistence, game over. STATUS.LEO progress is
blocked by open question 5 - ask the owner for a paired capture (menu before
and after finishing a level) or leave progress unimplemented and say so.
STATUS after PR-131: the ZARIA I end is now filmed in detail (the
level-end interaction scene with a purple-robed NPC + the grey-blue
bird, ball tosses included, levels/ZARIA.01.mkv t=440.3-457.3, the
ZARIA.02 cut at 458) and the vulture's mid-level sequences are
(zax37 t=306-315 and 385-413, on close view swoop-and-dodge, no
frame clearly shows the lion carried). Neither the NPC nor the bird
has a placement line, and the ZARIA.01 ANIMACJE tree carries NO bird
leaf (PTAK.MOV exists only in KADMAR.07/PTAK, TUNGA2.12/PTAKPLAT,
RAN.01/GPTAK_RA): the scenes are a scripted chain reaching into
other levels' leaves, or engine code - find which (G-10/G-11).

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
- [ ] `make` warning-free (`make win` and the wine legs belong to the
      end-of-implementation Windows pass, section 2 rule 8)
- [ ] `make -k -j"$(nproc)" verify-all` EXIT=0 with fixtures and
      fixture-free (serial is fine too; the Windows/wine legs stay serial) -
      not run for a pure-docs commit (section 2 rule 4's exception,
      2026-11-17)
- [ ] end-of-implementation only: `make win` warning-free,
      `make -k verify-windows`, and ci-windows.yml green
- [ ] README hook list updated if a `LEO_*` variable was added
- [ ] NEXT_STEPS.md record; this file's sections 1, 6, 7, 8 updated
- [ ] commit "PR-NN: ..." ; `git push`; `gh run list --workflow=ci.yml` green
- [ ] nothing copyrighted staged (`git status`)
