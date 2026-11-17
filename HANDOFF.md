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
| objects | 827+ placements, art by each state's `seq=`, `.MOV` state machines (loop until a transition fires; `GEN K_GENn`, `AC_BREAK_NEXT`); PR-93: the 3-comma static image lines (`<LEAF>,<name>.seq,x,y` -> that CD leaf's `.SEQ`, drawn at the authored x,y behind the actors) and the 0,0 entries = attached children (joined+counted, never instantiated at the world origin) |
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
| `TARGET(N)=COND` entry | ll_play.c/ll_play.h `ll_mov_tr.entry` | numeric (N) = start ROW (1-based) in the target's frame list; non-numeric = top | PR-86 evidence chain in the struct comment (PRE_JUMP skips A_JUMP's impulse row; A_WALK(7) mid-cycle re-join pinned on real data) |
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
     `[obj+0xbe] != 0` guard, while modes 2/3 sit behind
     `[obj+0xba] != 0` - so BY_TY is not a typo alias of BY_TIME
     (BY_TX: 0 files; BY_TY: 7 files, all lion climb/hang states). And
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
two cheap tests PR-102 points at, both data-supplied and neither done:
an entry with a large row number (the authored data asks to START a
state at a row - `A_DEAD(97)=...`, `A_JUMP(14)=...`, 37 states are
entered at several different rows) against the port, which always
starts at row 0, and a lion climb state (the 7 BY_TY states) against
the port, which would test both the vertical-feed reading of mode 4
and its `[obj+0xbe]` guard. The table itself is now read end to end:
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
row, and no other scheduler exists;
"100 units" of the level global's +0x388 feed is half-closed
(PR-100: the single writer is `mov [eax+0x388],0x64` at 0x4c8b7) and
PR-102 added its gate: that feed only runs when `[obj+0xba]` (modes
2/3) or `[obj+0xbe]` (mode 4) is non-zero; and PR-102 found the
concrete mechanism the pacing tension was missing - the row's threshold
is stored pre-scaled at parse time, `[node+0x10] = T * time_scale /
1000` (0x51660), and 0x52320 compares that against the accumulator, so
a threshold of 200 with the 1000 default is 2 feed steps and the same
row under `time_scale=2000` is 4. What a row's duration is, in seconds,
is now exactly one unread number: how often the +0x14 slot (0x536f0
and its wrappers) is called during play - PR-105 found the pump
(0x4bb30, which steps every object in the level's four lists at
[X+0x58]/[X+0x64]) but only its setup-time callers, not a frame loop.
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
fields PR-99 left unread.

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
