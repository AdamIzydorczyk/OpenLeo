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
| objects | 827+ placements, art by each state's `seq=`, `.MOV` state machines (loop until a transition fires; `GEN K_GENn`, `AC_BREAK_NEXT`) |
| lion (play mode) | walks on the level's own ground (`.PLT`), jumps (ESTIMATED units), keys facing-relative (Model M, PR-85), camera follows (lead + grounded feet-at-178 anchor), HUD shows energy/lives |
| collision | push-out ported from the machine code (`FUN_00042de0/42cf0`), solid/semi-solid response, enemy contact damage from the placement files, balloon pickup; ON by default except RAN.01 |

What does NOT exist yet (the real gap to "a playable game"): the lion's attack
and other moves, enemy behaviour (patrol/detect/attack), pickups/inventory and
the signal bus, ladders/vines, level flow (end of level, exchange between
TUNGA I and II), RAN's balloon mode, sound effects, save/progress semantics.
Section 8 lists them as tasks.

## 2. Ground rules (non-negotiable)

1. **Never guess data.** A claim about the original must match the machine
   code (`build/leo/leo.elf`), the data files, or a capture. Unknown things are
   counted/reported, or implemented as a clearly named ESTIMATE (section 6)
   that prints a log line and is listed in NEXT_STEPS. If you cannot decide a
   number, write down what is missing and stop.
2. **No game data and no game frames in git.** Not the CD files, not
   screenshots, not video frames, not the reference video. Reference material
   lives in `/tmp/opencode/...` (section 3). Pins in tests are hashes/strings.
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
5. `gh` token has no `workflow` scope: a commit touching `.github/workflows/*`
   cannot be pushed.
6. The owner supplies reference material (screenshots, video timestamps).
   Ask for a specific clip rather than inferring.

## 3. Machine layout and commands

Paths on the development machine (edit for another one; tools hard-code them):

| what | path |
| --- | --- |
| repo | `/home/aizy/Projects/LL_RECOMPILE` |
| CD data (`DATA`) | `/tmp/opencode/iso_stage/LEO` (ANIMACJE, ETAPY, SOUND, LEVELDES, the `.IS` movies) |
| HD install (`HD`, scripts/placement text) | `/tmp/opencode/hd_files` (ANIMACJE, ETAPY, LEO.EXE, STATUS.LEO ...) |
| golden captures / owner screenshots | `/tmp/opencode/golden/` (`user_refs/` = the owner's screenshots; never in git) |
| reference video | `/tmp/opencode/yt/gp.mp4` (episode 1), `ep2.mp4` (episode 2); the playlist is below |
| disassembly | `/tmp/opencode/vis/text.asm` (make it with `tools/re/make_text_asm.sh`) |

`/tmp` is a tmpfs: after a reboot restage the data (recipe in NEXT_STEPS.md,
"restaging") and redownload the video with
`~/.local/bin/yt-dlp --no-playlist -f 135 -o gp.mp4 URL` (pip install yt-dlp).

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
- HUD (`BELKA`): `x,y` + `[N ]name.seq` items: energy head (10 frames), lives
  digits, tools icons (4), count digits, item frame, crown.

Engine (objects have a vtable at +8; the common slots)
- +0x10 message handler (`{next, kind, a, b}` nodes; Lew: 0x1e130 -> 0x50d60
  jump table; kind 9 = balloon heal +32 and the sender's signal bit; 13 =
  velocity add; 1 = displace), +0x3c "is Lew", +0x40 "is enemy", +0x44 flag
  word, +0x48 shape element, +0x4c rect, +0x50 facing, +0x68 position,
  +0x78 touch, +0x9c filter, +0xd0 hit(source, ., damage).
- Flag word (token parser 0x51b30..0x51dc3): SOLID 0x400, SOLID2 0x20400,
  SEMI_SOLID 0x40400, ANTI_SOLID 0x10000, ANTI_POCISK 0x80000, DANGER 0x2000,
  CHECK_ANI 0x40, CHECK_ZW 0x80, CHECK_LN 0x100, CHECK_LEW 0x200, CHECK_ALL 0x1c0,
  CH_DIR 0x800, INV_V 0x1000, CHECK_BASE_LN 0x4000, CHECK_ALL_PLANE 0x8000.
- Touch dispatch (FUN_0004b500, FUN_00052780): the mover's CHECK_ANI/CHECK_ZW
  pick the animacje/zwierzaki list; objects are tested only on the mover's layer
  unless CHECK_ALL_PLANE; the touch method (slot 0x78) runs the push-out
  (FUN_00053ae0 -> 00042de0) and then the hit effects (FUN_00055230).
- Damage: energy -= damage (FUN_00054550); enemies hit with their own
  `damage=`; DANGER objects with the level's `danger_power`. Lion energy starts
  at 100 (0x503ac), lives = global 0xa94f0.
- Frame tails in `.MOV`: `GEN <sig>`, `I vx,vy`, `F`, `FC`, `M dx,dy`, `WAIT`,
  `lock/unlock`, `ATC_ON/OFF`, `GOTO_REL`, sound words (survey in NEXT_STEPS PR-69).

Walk speed: the `.MOV` caps (T=90) match a measured ~75-125 units/s on video.

## 6. Estimates and assumptions in the code (revisit when evidence arrives)

| what | where | value / rule | evidence |
| --- | --- | --- | --- |
| jump impulse unit | ll_play.h `LL_PLAY_JUMP_DIV` | `I`/4 u/s | ONE measured jump (vine, ep.1 318.6 s); PR-86 re-confirmed on ZARIA live: A_JUMP `I 150,-1200` -> apex ~24 u / 0.36 s, matching the video's jump population (gp.mp4 feet-track scan: clean candidates 23-30 u over 8-13 frames) |
| jump gravity | `LL_PLAY_JUMP_G` | 1650 u/s^2 (= G.y 55 x 30) | same jump |
| jump-state ending | ll_play.c | a state that DECLARES an AC_BREAK_NEXT exit ends at its frame-list WRAP (the landing no longer cuts it); forced ->A_STAY/A_DRAP kept ONLY as fallback for states with no such line | PR-86: real A_JUMP/A_JUMP_S declare A_WALK(7)/A_DRAP(1)=AC_BREAK_NEXT; ZARIA.01 leg pins it |
| `TARGET(N)=COND` entry | ll_play.c/ll_play.h `ll_mov_tr.entry` | numeric (N) = start ROW (1-based) in the target's frame list; non-numeric = top | PR-86 evidence chain in the struct comment (PRE_JUMP skips A_JUMP's impulse row; A_WALK(7) mid-cycle re-join pinned on real data) |
| jump key | ll_level.c | Space | viewer choice |
| fire keys | ll_level.c `play loop` + platform_sdl `map_key` | Ctrl=K_FIRE, F1..F4=K_F1..4 (weapon selectors), Alt=K_ALT; script tokens FIRE/F1..F4/ALT comma-combinable | PR-87; names from the walkthrough, the .MOV conditions are the contract |
| `,` in transition COND = AND | ll_play.c `parse_cond`/`cond_true` (groups) | `|` ORs groups, `,` ANDs tokens within one; OTHER poisons only its group | PR-87 MODEL CHOICE (evidence in ll_play.h: AC_ONAIR/AC_DRECT pairs, every K_FIRE,K_Fn attack line); A_FIGHT entry pinned real |
| hurt lock | ll_level.c `play_collide` | 30 ticks of 20 ms | 0x50304 constant, unit assumed |
| `AC_TIME` | ll_level.c `obj_cond_true` | = end of frame list | `time=min,max` unit unknown |
| lives / energy HUD | ll_level.c | 6 lives, energy frames (100-e)/10 | video start shows 6 |
| camera | ll_level.c `follow_camx/camy` | lead 100/220, ease 6 u/tick, grounded feet-at-178 anchor (y-ease 2 u/tick invented) | video + owner frame comparison (PR-84) |
| contact pass default | ll_level.c | on except RAN.01 | RAN is the balloon level |
| SEMI_SOLID | `play_collide` | only the r/5 shift | velocity add (kind 13) unit unknown |
| SOLID push polarity | `obj_shape_elem` | shape uses the DRAW polarity incl. CH_DIR facing | consistency, not decompile-pinned (PR-81) |
| player facing model | ll_play.c `face`, ll_level.c `play_draw/play_collide` | face follows walkdir only in CH_DIR states; draw/shape = authored XOR face, face sticks | flags real (LEW census, PR-82); the always-XOR rule for no-CH_DIR states is a model-consistency choice, NOT decompile- or video-pinned (HANDOFF Q0) |
| K_FWD/K_BKWD key model | ll_play.c `cond_true` (MODEL CHOICE note) | facing-relative: K_FWD = arrow along current face; walkdir = the literal arrow; a left-facing turn settles via the state pair or A_TURN+AC_BREAK | PR-85; forced by data (one A_WALK, no BKWD-walk state); NOT yet confirmed on video |

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
    px/s, frames advance on ms, bounds= patrol
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
2. The signal-bit table: which bit of the object's set at +0x60 is K_GEN1..4
   and each `AC_*` (bit 54 was assumed K_GEN1). Read it from the condition
   parser like the flag table was (strings near 0x92180, parser near 0x51b30).
3. `AC_DUP`, `AC_ONAIR`, `AC_DRECT`, `AC_EDGE`, `AC_DRAB_CATCH`, `AC_ENEMY_ATC`,
   `AC_BLOK`: meaning (they gate jumps, falls, ladders, enemy attacks).
4. `danger_power` default (level object +0x54 is never initialised in 0x4a5d4).
5. `STATUS.LEO` field meaning (shipped file is a fresh save: no progress); the
   map's progress arrows/"start" variant depend on it. Needs a paired capture.
6. The camera's catch-up rate and dead zone (measure with a clean clip).
   Anchor SOLVED in PR-84 (grounded feet-at-178; view holds while
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

**T2. The lion's attack and enemy energy.**
Goal: kill the first LICHA. Where: `LEW.MOV` states `A_FIGHT*`, `A_FG_PACKA*`
(conditions `K_FIRE`+`K_F1`/`K_F2`, `K_ALT`); enemy `.MOV` death states;
`energy=` from the placement. How: add the fire keys (Ctrl=fire per the
walkthrough, F1..F4 weapons; map as viewer choices), find where the lion's hit
box (`AC_RECT`) and the damage to enemies are decided (enemy hit method slot
+0xd0, class 0x41180 / 0x3dae0), apply, play the enemy's dead state, remove
the contact damage. Done when: a scripted run kills one LICHA and a pin shows it.

**T3. Enemy behaviour.**
Patrol between `bounds=` (LANDED in PR-79: BY_X states walk at F/4 px/s and
flip at the bounds range; CH_DIR landed in PR-81 as the facing flip - the
census pairs bounds= with CH_DIR and the video's winning art polarity flips
with the travel direction), detect within `scope=`, `AC_ENEMY_ATC` attack, the
`ATC_ON/OFF` directives and the `F` motions in their scripts. Needs T1's `F`
semantics and open question 3. Done when: ZARIA.01's LICHA/PAJAK/NOSOROG move
and attack in a headless run (log pins), and the contact damage uses the state.
(Partially met: they MOVE and TURN - the verify-play pin now hits a WALKING
LICHA at tick 199, and PR-82 generalized facing to follow the travel
direction of every CH_DIR state for objects AND the lion, so a future
charge/push source turns the sprite with it automatically. Still open: the
pause-in-place vs walk-burst (DANGER) state gating, `scope=`, and the
DANGER charge's own motion, which needs T1's `F`/`I` semantics.)

**T4. Signal bus, pickups, inventory.**
Resolve open question 2, then: the `GEN`/signal delivery between objects, the
`AC_*` actions (`PLAT_OFF/OUT`, `XCHG_LEVEL`, `SHOW_CROWN`, `MSG_PUSH`, `LEAVE`),
pickups (`GRABOBJ` handler at slot 0x78 = 0x3db90, `KORONA`, `SURPRISE`, stars
`GWIAZDKI`), the item box (`TOOLS.SEQ` icons; the video shows the held item),
tools use (`K_F3/F4`: stone/pickaxe/stick). Done when: ZARIA.01 can be played to
the stone-corking puzzle (walkthrough in NEXT_STEPS PR record / the Polish
walkthrough page) and each step has a log pin.

**T5. Ladders, vines, climbing.** States `A_WSP`, `A_WSP_LIN`, `A_PODCIAG`,
`A_SCIAG`, objects `DRABINKA`, `LINA`; conditions `AC_DRAB_CATCH`,
`AC_LINA_CATCH`, `AC_EDGE`. Depends on T4's conditions.

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
- 480p compression and a scrolling camera limit precision; prefer segments the
  owner points out ("jump in place", camera not scrolling). Scripts: tools/re.
- The owner's timestamps so far: ep.1 5:16-5:19 (vine jump, used), ep.2
  0:53-0:55, 0:56-0:58, 7:13-7:16, 7:44-7:47 (all with a scrolling camera).

## 10. Checklist for every PR

- [ ] one logical step, evidence recorded (machine code address / data file /
      timestamp), estimates named in code AND in NEXT_STEPS
- [ ] data-free hand-derived test + mutation check
- [ ] `make`, `make win` warning-free
- [ ] `make -k verify-all` EXIT=0 with fixtures and fixture-free
- [ ] README hook list updated if a `LEO_*` variable was added
- [ ] NEXT_STEPS.md record; this file's sections 1, 6, 7, 8 updated
- [ ] commit "PR-NN: ..." ; `git push`; `gh run list --workflow=ci.yml` green
- [ ] nothing copyrighted staged (`git status`)
