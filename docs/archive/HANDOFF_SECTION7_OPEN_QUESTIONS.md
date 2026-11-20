# HANDOFF.md section 7 - the open-questions ledger, FROZEN

Extracted VERBATIM by PR-202 (doc step 4) from HANDOFF.md at
commit
71a2262080f70ca70f60d3886e41c5e481840355
(the section-7 text: heading line through the last line before
section 8), items 0-15 included, nothing edited or trimmed.
It is history: it is NEVER edited after this commit.
Citations of the form "HANDOFF 7 item N", "HANDOFF Q0" or
"G-nn" resolve HERE, at the records each item cites, and
through the origin column of docs/OPEN_ITEMS.md, which this
ledger seeded. Corrections do NOT go here: they go in register
rows and in journal records (the archive README states the
rule for every frozen file).

---

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
      change since PR-140's 81eee9b; gates re-run fresh: make
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
     survived ONLY in its own commit blob (7626bb3 / ecf3e9d;
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
     changed; ledger census run (HEAD 2968da8 = last carrier).
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
     61bb0cf = last carrier). Full record: the PR-165 section
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
     unchanged; ledger census run (HEAD 7f64eea = last carr
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
     ec199d6 = last carrier). Full record: the PR-167 section
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
     5eb7c4d = last carrier). Full record: the PR-168 section
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
     unchanged; ledger census run (HEAD a92bbb5 = last carr
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
     unchanged; ledger census run (HEAD 5058e67 = last carr-
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
     72); image unchanged; ledger census run (HEAD ff84394 =
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
     68c85d0 = last carrier). Full record: the PR-172 section
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
     run (HEAD 6926676 = last carrier, 23 records). Full
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
   - PR-179 (READING; no port code, no behaviour change):
     LION'S +0xac VERB AND MSG HANDLER READ AT LAST - the
     two longest-carried engine items (since PR-174/PR-175)
     CLOSE. +0xac 0x546a0: gates ([obj+0xb6] bit 2;
     [rec+0x12]&0x4000 -> the 0x4b600 pool query + the
     AC_BLOK bounds test 0x4b430 + 0x4b7f0 merged into the
     key; else mode>=4 -> the same triple with the ecx=2
     filter) then the ATTACH MACHINE: [obj+0xe6] = per-
     step candidate (the pump 0x550a0 CLEARS it - it is
     refilled every step), [obj+0xa9] = attached object
     (slot 0 = ENTER on it, slot 4 = LEAVE); candidate
     transitions raise AC_DUP (byte0|0x10 = bit 4, the
     PR-88-verified enum) and AC_DUP_CD (byte1|0x20 = bit
     13) - PR-150's ladder-overlap AC_DUP model now HAS
     its engine mechanism named; a vy>=0x58c gate skips the
     attach while falling. Tail: position commit consumes
     and zeroes [obj+0xca/ce] + [obj+0xea]. MSG HANDLER
     0x54560 (87 installs, slot varies): full id map - 2 =
     SET v, 3/4 = OR 4/8 into [obj+0xb6], 6 = SET facing
     ([obj+0x6d]/[obj+0x71]), 8 = ZERO v, 13 = IMPULSE,
     27 = SET attached (LEAVE old, ENTER new); else ->
     0x53770; the "0x5463a arm" was the else-default, the
     two carried items were ONE map. THE RESET METHOD
     0x54e00 = the field-family map: zeroes both gate
     scalars, both attach fields, v and deltas; energy
     [obj+0xad] := 100 and [obj+0xb1] := 22 are DEFAULTS
     (new facts). CORRECTION (bracket): 0x55070 is a +0x54
     PHYSICS HOOK, not a +0x8c getter (PR-178 F4 was
     wrong; LION +0x54 = 0x55070, and the 0x55070 chain's
     +0x98 call lands in the 0x23a2x getter cluster, post-
     detach +0xb8 = 0x23880 - the hook and passive-lineage
     families touch). [obj+0x11a] = a LION mode field
     (==5 negates the feed-gate scalar, ==7 arms the
     energy half; 14 sites mapped). 5 new items (the
     candidate-filler 0x54eb2/0x2c6f0/0x5413e is the
     named LEAD for PR-180; + [obj+0xb6] bit law; the
     0x54790-0x547b5 CFG gap; the msg senders; the mode
     map); 3 closed/updated; 3 brackets. Next -> PR-180 =
     the attach/gate-field PRODUCER pass. Full record:
     PR-179 section of NEXT_STEPS.md (scratch
     /tmp/opencode/pr179/).
   - PR-180 (READING; no port code, no behaviour change):
     THE FEED-GATE WORD HAS ITS PRODUCER LAW. The +0xcc
     slot is a SENSOR-WORD CHANNEL: every class's step
     driver computes a 2-dword word and calls +0xcc then
     its pump (7 sites; 6 driver shapes found - distance-
     ratio x2 with constants 0x909ef/0x909f7 = doubles
     10.0, a grid-index word, a TILE-QUERY word that
     DOUBLE-dereferences [obj+0x106], two buffer-shape
     drivers). The LION's driver, whole: 0x27d60 at LION-
     table +0x240 (single install) - anchor pair [obj+
     0x126/0x12a] (setter helper at 0x27d40), deltas vs
     the +0x68 position, QUADRANT GATE (compute only if
     dX>0 or dY<0, else zeros), then buf0=round((|dX|+
     |dY|)/(10|dX|)), buf1=(...)/(10|dY|) - pure vertical
     motion gives ZERO gate words (integer floors;
     gate_driver.py verifies the |dX|<9|dY| floor) - then
     call +0xcc (0x50340 -> 0x549f0 window re-decide per
     PR-178) and the pump 0x550a0 (0x27e25). The dispatch
     INTO 0x27d60 is NOT statically resolvable (negative
     scans for any +0x240 access; only `lea` table-walk
     mentions at 0x49e89/0x49ec0 - counted). PR-178 item
     (1) CLOSED (the filler is the class driver; LION's
     law pinned; dispatch + other drivers counted). THE
     CANDIDATE [obj+0xe6] IS WIRED BY ADDRESS AT LEVEL
     INIT: 0x2c6f0 = pool-wiring (mgr+0x390 -> 0x57500 ->
     ds:0xcec74; 0x57530 CLASS QUERY for table 0x98274 ->
     ds:0xcec78; then &found+0xe6 and [obj-0x1e] parked
     into a cfg block) - stores through the linked pointer
     are statically invisible, which explains why no
     per-step writer was ever found; PR-179 item (1)
     re-scoped (0x54eb2 = a TELEPORT/ATTACH-HERE method,
     not a writer; 0x5413e = a READ in a state-COPY
     helper 0x54120-0x5416d). NO PORT CHANGE: gate words
     are engine-internal; the port's intent-proxy is
     neither confirmed nor contradicted - the behavioural
     reading of the ratio law is video-gated like the
     ladder-gate item. 4 new items; 1 closed, 1 re-scoped;
     2 brackets. Next -> PR-181 = the pool-wiring pass
     (0x2c6d3 entry+callers, 0x57500/0x57530 pool queries,
     the +0xe6 holder - also the pool-name decode's home).
     Full record: PR-180 section of NEXT_STEPS.md (scratch
     /tmp/opencode/pr180/).
   - PR-181 (READING; no port code, no behaviour change):
     THE POOL MACHINERY DECODED END TO END - TWO LONG
     CARRIED ITEMS CLOSE. THE POOL LAYOUT: mgr 0xcef40
     +0x390 -> pool P with member array P+0x10, index
     arrays P+0x20/P+0x24, count P+0x1c (setters 0x574d0/
     0x574e0/0x574f0, enumeration idiom at 0x4cf0a); a
     class-RANGE energy rule at 0x54458 (nameWord+500 vs
     class id -> energy -1, counted item). THE CANDIDATE-
     WRITER FOUND: the ONLY static [reg+0xe6] store is
     0x454a2 inside the relationship-nearest SELECTOR
     0x45400 (entry; 0x45401 again an entry-desync): it
     scores the PR-177 rel-list (0x45380 count, 13-byte
     records, positions via +0x6c, distances via 0x455a0)
     and stores [obj+0xe6]=candidate, [obj+0xea]=distance,
     [obj+0xe2]=CANDIDATE CLASS BYTE (descriptor+0xc, the
     PR-172 layout) - so the +0xac attach gates are class-
     based. Sole caller = wrapper 0x45320 (physics via
     0x541f0 with the CAMEL-variant class table 0x98278 as
     cfg, velocity pair [obj+0xca/ce] added); 6 call sites
     inside the provider clusters PR-177 F1 enumerated;
     the pair installs at LION-table windows (0x96960 reads
     +0x2cc/+0x2d0) - dispatch unresolved with the +0x240
     family. PR-180 F3 CORRECTED: 0x57500/0x57530 are
     hash ARITHMETIC over the pool fields, ds:0xcec74/78/
     7c are one function's private scratch, and 0x2c670 is
     a placement/randomize method of the camel-family
     classes (rng-hash membership vs 12-stride spawn slots
     at [obj+0xbf/0xc3] +-50). POOL-NAME DECODE CLOSED:
     the grammar block 0x91e01-0x92060 = level-file
     property names (danger_power...maski, incl. 'lew'),
     31 OBJECT FAMILY NAMES = the .MOV stems (LINA,
     GWIAZDKI, KATPLAT, BAL, POCISK...), two-letter
     object codes, the engine's own error string 'Bledny
     wpis w pliku levels: %s'; 0x49871 resolves a name via
     the class slot +0x00 -> name table 0xa11fc ('$'-split
     + '*' flags, cfg+0x34 failure flag); 0x49920 = the
     150-site property/pool reader. 5 new items; 3 closed/
     updated-partial; 4 brackets. Next -> PR-182 = the
     msg-sender pass (ids 2/6/13/27 emitters - the input
     protocol). Full record: PR-181 section of
     NEXT_STEPS.md (scratch /tmp/opencode/pr181/).
   - PR-182 (READING; no port code, no behaviour change):
     THE 'MSG' SURFACE IS THE EVENT SYSTEM - PR-179 item
     (4) CLOSED. EVERY OBJECT HAS A QUEUE AT [obj+0x10];
     a 16-byte node is +0 next, +4 ID, +8 SENDER
     identity, +0xc ARG, appended by 0x52b10 (66 sites,
     ALL argument-driven - 0 literal ids near the calls);
     drained by 0x52b50 through CLASS SLOT +0x10 (the
     0x54560 install histogram 73/87 at +0x10 confirms
     the slot); the pump 0x52b80 = drain, then verb
     +0x14 (LION = 0x550a0 - PR-179's pump), conditional
     +0x00 - EVENTS-BEFORE-VERB every step, the frame
     PR-179's per-step candidate clear needed. The LION
     tables use +0x10 = 0x1e130 instead (consumer body
     counted). THE GENERIC SENDER IS THE BROADCAST
     0x4bc40 (22 callers): walks the level object lists
     (mgr+0x58/+0x64 chains via +0x34), filters via
     class slot +0x0c identity getters, enqueues id-16
     nodes with sender magic 0x8138764 + arg = THE
     SIGNAL NUMBER; the consumer 0x53770 (PR-179's
     'generic' - UPDATED) raises bit N into the live key
     set [obj+0x60] (word N>>5, bit N&31 - the PR-88
     numbering EXACTLY) + merge 0x49580: RAISING AN AC_*
     SIGNAL = SET BIT N. The magic occurs at 4 sites:
     the camel methods 0x2c5e1/0x2c613/0x2c653 raise
     bits 27 = AC_BUTLA / 28 = AC_BECZKA at record arms
     ([rec+4]==0xa / ==1 guards), 0x3eacf raises 57 =
     K_GEN4. PR-179's ids 2/6/13/27 = ONE consumer
     implementation (non-LION classes). The port matches
     (same bit numbers, events-before-verb): NO port
     change. 5 new items (enqueue-site id values, the
     0x1e130/0x52bb0 bodies, the identity-getter +
     magic semantics, the [obj+0x28]/0xcf2c8 registry,
     the [rec+4] guards); 3 closed; 3 brackets. Next ->
     PR-183 = the event-consumer BODY pass (0x1e130,
     0x52bb0, the identity law, the enqueue-site id
     values) + the player-side row seek (PR-174 item 2).
     Full record: PR-182 section of NEXT_STEPS.md
     (scratch /tmp/opencode/pr182/).
   - PR-183 (READING; no port code, no behaviour change):
     THE EVENT CONSUMER BODIES ARE READ. THE LION-SHAPE
     CONSUMER 0x1e130 (+0x10 at LION tables): id<5 ->
     0x50d60; ids 5..11 -> ENTER on the SENDER; id 12 ->
     [obj+0x12a]:=1. 0x50d60: anchored ([obj+0x126]!=0)
     objects DROP id-1 (move-by) events; a jump table at
     0x50d20 covers ids 9..21: id 9 = THE FEED LAW -
     energy [obj+0xad] += 32 clamped to 100 (its THIRD
     writer) after a live-key-set merge (the 0x749a0 +
     0x40-byte/0x49580 shape); 10 -> [obj+0x14/0x18] :=
     (sender,arg); 11/15 -> sender verbs +0xd8/+0xfc;
     21 -> [obj+0x116]:=1; ids 12,13,14,16..20 + all
     others -> DELEGATE to 0x54560. THE CORRECTED 0x54560
     MAP (replaces PR-179 F2 labels): 2 = [obj+0xc2/0xc6]
     PAIR SET, 3/4 = [obj+0xb6] |= 4/8 (PR-179 item (2)'s
     SETters), 6 = [obj+0x6d/0x71] setter, 8 = pair CLR,
     13 = pair ADD, 27 = ATTACH (old +0x04 LEAVE, [obj+
     0xa9] := node+8, ENTER), else 0x53770. THE 0x52bb0
     FALLBACK (whole): 1 AND 13 = [obj+0/4] delta + the
     [class+0x54] hook (PR-17x hooks = event-triggered!),
     14 = [obj+0x28]:=1 (the pump gate's setter), 15 =
     [obj+0x30]:=sender, 33 = [obj+0x108]:=1, all else
     dropped - A PER-FAMILY VOCABULARY (PR-182 item (3)
     answered). The 66-site literal-id histogram (90-byte
     window; 64/66 literal): 1x7, 2x5, 5x7, 8x5, 13x4,
     15x5, 21x4, 25x11 (most-sent, NO consumer found -
     open), 27x5, 16/17/18x2/20/29/30/33/... THE IDENTITY
     LAW: 178/178 class tables' +0x0c getter = `mov eax,
     ds:<tok>; ret` - per-family TOKENS initialized by
     runs of `call 0x44350; mov ds:<tok>,eax` (0x44350 =
     a counter ++ds:0xa90ac): the magic 0x8138764 is a
     runtime token, not a hardcoded identity (owner family
     = open item); the 0x4bc40 contract = (ebx=ID,
     ecx=TARGET TOKEN, push=ARG). CORRECTED camel raises
     (PR-182 F2 bracket): 0x2c5b0 pushes 33=AC_KOREK
     (one-shot flag ds:0xcec70, [obj+0x5c]==2), 0x2c5f0
     pushes 28=AC_BECZKA ([rec+4]==1), 0x2c630 pushes
     27=AC_BUTLA ([rec+4]==0xa) - CORK/BARREL/BOTTLE,
     the authored camel-props signals. Bonus: 0x52c10/
     0x52c40 = the +0x34/+0x38 list insert/remove (the
     broadcast walks); 0x52cc0 (PR-177 item (1) helper)
     READ WHOLE - sender verbs +0x30/+0x24/+0x28(0x80) +
     the 0x56600 indexed `call [class+0x18](ds:0xcf0ac[
     edx])` dispatch + [obj+0x20]:=1. 5 new items, 6
     closed/part, 6
     brackets; the row-seek's model precondition now
     complete (frames stay blocked). Next -> PR-184 =
     the +0x10 consumer census over 178 tables (naming
     the id-17/18/20/25/29/30 families = the PR-177
     item (3) table work) + the token-init order. Full
     record: PR-183 section of NEXT_STEPS.md (scratch
     /tmp/opencode/pr183/).
   - PR-184 (READING; no port code, no behaviour change):
     THE +0x10 CONSUMER CENSUS IS COMPLETE + THE TOKEN/
     STAMP CORRECTION. CENSUS: 41 distinct consumer impls
     over 178 tables (0x54560 x73, 0x3dcb0 x22, 0x53770 x21
     INSTALLED DIRECT - no id map, pure named-raise!,
     0x40ca0 x7, 0x52bb0 x5, 0x50d60 x5, 0x3d8f0 x4,
     0x55720 x4, 0x1e130 x3, 0x3db50 x3, 31 singletons;
     89 distinct (consumer,+0x78-family) pairs - the PR-177
     item (3) table work joined). NEW CONSUMER LAWS:
     0x3dcb0 forwards id-1 move-by events to the LINKED
     object [obj+0x112] (its class +0x10) before the
     generic map; 0x55720 forwards the SAME NODE to every
     CHILD ([obj+0x3c] count/[obj+0x40] array) - event
     propagation down groups; 0x3db50 RELAYS id 28
     (AC_BECZKA) into an id-33 enqueue on the object at
     [obj+0x30]; 0x3d8f0 id-5 -> [obj+0x66] |= 0x40 +
     [obj+0x6d]:=sender; 0x40ca0 id-1 -> [obj+0x3c..]
     block arithmetic. THE id-25 ANSWER: SEVEN same-
     cluster consumer families (0x2f460/0x2f5b0/0x2f840/
     0x3b6b0/0x3bab0/0x3c220/0x3c310) delegate then use
     node+8 as control - their OWN broadcast senders
     (0x2f1ef..0x3c1c5, token-filtered) pass the sender's
     [obj+0xf2]-LINK in ECX: link-equality guards, small-
     code dispatch, byte-form key-bit raises [obj+0x64..]
     |= 0x40 etc. + [obj+0xee] gate sets - a PAIR/
     ATTACHMENT SYNC SIGNAL; [obj+0xf2] reads as one
     coherent SHARED record-array pointer (also the
     0x45200 array base); the `cmp [reg+4],imm8` decode
     scan also pins
     id 7 (the 0x1ec90/0x1f110 families - AND their
     families SEND id 7 via 0x4bc40 with edx = TOKEN
     GLOBALS ds:0xcebd4/0xcebc8: a complete targeted send/
     consume pair), 14/16/18/19/20/26/28/31/32 to named
     families; ids 17/29/30 have no static consumer under
     that form (open). CORRECTION (self-caught, 3
     brackets on PR-183 F3): the 135 `call 0x44350; mov
     ds:<tok>,eax` init pairs (17 runs) start from counter
     base 1 (ds:0xa90ac static = 1, no other writer) ->
     TOKENS ARE SMALL INTS 2..136; the magic 0x8138764 is
     NOT a token (appears nowhere as data) - it is the
     CONSTANT NAMED-RAISE STAMP; the real filter argument
     is EDX (0 = NO filter - the camel trio broadcast
     UNFILTERED to every object in both chains; non-zero =
     compared vs the +0x0c token), ECX goes to node+8, and
     0x53770's magic gate is a STAMP discriminator, so
     named raises reach ALL objects (consistent with PR-15x
     per-object signal listening). 5 new items (deeper-form
     scan for ids 17/29/30; link/group field semantics;
     the token->family bookkeeping map; the id-7/18/19/...
     arm bodies; the byte-form raise bit-numbering); 3
     items part-closed (PR-183 (1)/(3), PR-177 item (3));
     PR-176 items (2)/(3) extended/partially closed (the
     [obj+0xe6]/[ea] pair's role CLOSED as PR-181's
     candidate law; 3 [obj+0xee] contexts now known). Next
     -> PR-185 = the controller/input half (PR-176 items
     (4)+(2) remainder: the [obj+0xa5/a6/a7] button bits,
     who writes [obj+0xee]>=10) + the ids-17/29/30 deeper-
     form scan. Full record: PR-184 section of NEXT_STEPS.
     md (scratch /tmp/opencode/pr184/).
   - PR-185 (READING; no port code, no behaviour change):
     THE CONTROLLER IS THE STAGE LION. 12 class tables
     install the +0x8c getter 0x504e0 (= PR-176 F3's twelve;
     all share +0x14=0x550a0 (79/178 tables carry it),
     +0x3c=0x230a0, +0x78=0x50f80); the ZARIA.01 one is
     0x96960 (imm-loads at 0x1ff18/0x20498 inside the
     0x1dff-0x204xx ZARIA provider cluster). Its vtable =
     74 methods INCLUDING the PR-181 relation/selector
     helpers - the relation machinery IS controller methods.
     THE CONTROLLER SENDS EXACTLY ONE EVENT: 0x50c60
     enqueues id 5 with sender := its own [obj+0x6d] link;
     ZERO broadcasts - so the id 1/2/13 literal senders are
     NOT the player-controller path (answered). THE
     [mgr+0x394] ANSWER: +0x20/+0x24 verbs = sensors
     reading the manager's TRACKED object via 0x52d30:
     0x52a00 = Manhattan distance self-vs-tracked, 0x52a50
     = call [class(tracked)+0x68]. PR-176 ITEM (4) CLOSED:
     the button bytes ARE hardware state - the 0x54910
     motion-synth tail (0x54be6-0x54be9) copies `[obj+
     0xa5..a8] := [[input obj]+8]` every call; the getter
     has exactly 11 F6 bit-tests (10 chain legs + the
     bit-0x2 queue gate to 0x551f0). The 12 gate words ds:
     0xcef5c..0xcef88 are static 0 and written ONLY by
     level-provider functions (ZARIA's own 0xcef5c/60 set
     at 0x1e0dc/0x1e0e4 in its provider cluster; hard-set
     1 at 0x3b1f2; xor/or-imm at 0x39e33/0x3b4dc...) =
     PER-LEVEL AUTHORING of which K_F* signal ordinals
     exist. PORT VERDICT: no rule missing, no change (the
     port's ll_play_keys keyword channel = the engine's
     synthesized-keys half; the gated ordinal half adds
     signal names; ZARIA's weapon keys are authored on).
     PR-176 ITEM (2) CLOSED: the >=10 writer is a getter
     WRAPPER, +0x8c=0x613d0 on exactly 2 classes
     (0x9f668/0xa0624, also controller-shaped): four arms
     pre-set [obj+0xee] := 10/11 from button bits
     ([a6]&0x02/0x01/0x40, [a7]&0x04) gated by NEW words
     ds:0xcef84/0xcef88, clear the bits, then call the
     chain (which may override with a higher-priority
     ordinal); K_F11/K_F12 become reachable. Their arming
     0x6149f = a signal-PAIR COMBO handler (K_F4+K_F9 ->
     arm 0xcef84; K_F5+K_F3 -> arm 0xcef88; zeroes the
     pair words, calls 0x44910 edx=10/11). PR-184 ITEM (1)
     CLOSED via the deeper-form scan (`mov reg,[node+4]` +
     `cmp reg,imm` - the form PR-184's 83-scan misses):
     id 17 -> 0x24170 (+0x10 of table 0x988ec: id-18 with
     sender=arg=0 sets [obj+0x12a]:=1 - the PR-183 LION
     id-12 field - gated by [obj+0x5c] not in {22,23},
     then [obj+0x132]:=1), id 29 -> 0x32130 (+0x10 of
     0x9d338: [obj+0x12e]:=1, else delegates 0x50d60);
     id 30 DISSOLVES (all hits are distance/audio
     arithmetic - no event consumer of id 30 exists).
     CORRECTION (1 bracket on PR-184 F1): an EIGHTH id-25
     arm in that form - 0x2e8e0 (+0x10 of controller table
     0x9c958) delegates 0x50d60 then decodes 25 into an
     energy arm; the seven-family COUNT was form-partial
     (the pairing LAW stands). [obj+0xee] is family-
     multirole: ordinal / record-array handle / pair-sync
     gate (writer census 127/47/32 for [ee]/[f2]/[f6]).
     PR-183 item (1) now CLOSED WHOLLY; PR-176 item (5)
     PART (writer law found, per-level values open),
     PR-177 item (4) PART ([mgr+0x394] half), PR-184 item
     (2) PART. 5 new items (LION flag family [obj+0x12a/
     12e/132] + the whole 0x106..0x132 state block; input-
     source identity; exact gate values per level; the 2
     wrapped classes' names; the id-13/15/21/23 arms).
     Next -> PR-186 = the LION-flag/input-source follow-up.
     Full record: PR-185 section of NEXT_STEPS.md (scratch
     /tmp/opencode/pr185/).
   - PR-186 (READING; no port code, no behaviour change):
     THE LION FLAG BLOCK IS A SIGNAL-ROUTING STATE MACHINE
     AND THE INPUT BOTTOM IS THE DEVICE-MANAGER SAMPLE
     RING. THE SIGNAL VERB +0x90: every class installs it
     (178/178, 55 targets) but ONLY 0x3edaf and the state
     pump 0x5357d dispatch it - a state row can raise a
     signal on the LINKED object ([class+0x74], PR-183's
     selector law). ZARIA's verb 0x1e250: signal 4 gated by
     [obj+0x10e]*[obj+0x6d] -> 0x53670; signal 10/mode 0xa
     -> ENERGY [obj+0xad] -= 30; flag [obj+0x12a] ->
     out-signal 9 + self-clear; mode 9 + live-key bit 2 ->
     [obj+0x116] := 1; signals 0xb/0xc <-> the K_F4/K_F3
     gate words. The 0x988ec-family verb 0x245b0: latch
     [obj+0x12e] + arm [obj+0x12a] guards, then reads an
     AUTHORED SIGNAL LIST [obj+0x136]{array,count@[+8]},
     12-byte {x,y} entries -> 0x5c320(0x21) check -> kind-4
     nodes via 0x53eb0 ONTO THE PLAYER'S [obj+0xd2] queue
     (node+0x1d := y) + clears [tracked+0x106] (= zeroes
     the player's next input block, PR-106 law); pending
     [obj+0x132] -> broadcast id 19 (edx = ds:0xcec2c) then
     clear; out-mode := 0x16. Flag chain pinned end to end:
     id-18 (sender=arg=0) sets [obj+0x12a] (0x2419d);
     id-17's arm 0x241e2 dispatches node+8: ==1 -> mode-
     not-{22,23}+armed -> [obj+0x132] := 1, ==2 -> 0x241cd
     (body counted); id-29 latches [obj+0x12e] (PR-185) -
     since 0x9d338 is 0x988ec's death-double, THE FLAG
     BLOCK IS SHARED ACROSS THE LIFE/DEATH CLASS PAIR of
     one object: death latches off the authored-signal
     machinery. Census: [obj+0x12a] 60 refs, [0x12e] 38,
     [0x132] 20, [0x136] 8; block 0x106-0x132 + mode enum
     (PR-179) + queue flag (PR-104/106) = THE LION STATE
     BLOCK, now connected. PR-185 F2 SELF-CORRECTED (1
     bracket): the input-write tail 0x54be6-0x54be9 belongs
     to the INPUT-APPLY VERB 0x549f0 (+0xcc, 115 tables) -
     NOT the 0x54910 motion synth, which ENDS at 0x549e6
     (118 installs elsewhere) - as PR-106 already read.
     INPUT BOTTOM: the frame loop reads the device block
     (0x4ac65 `mov eax,0xcf400; call 0x58690`) - device
     manager 0xcf400: state gate [+0xb]==2, driver array
     [+0xf], ring indices [+0x2b]/[+0x2f] mod the driver's
     count [[drv]+0xc], samples from [[drv]+0]+idx*0x10 -
     SAMPLE RINGS (capture hook = PR-106 item (1)
     remainder); all 14 controller tables install the
     WRAPPER 0x50340 at +0xcc (15th install: NON-controller
     0x9a54c, its getter slot 0x27280 - identity counted):
     [obj+0x106]==0 -> input ZEROED (not-switched-on = no
     input), [obj+0x11a]==5 -> dx NEGATED (mode 5 =
     mirrored controls), then tail-call 0x549f0. PORT
     VERDICT unchanged: the port's keyboard channel lands
     at the same key-bit layer. PR-185 item (4) CLOSED
     (input source = the device sample block; no input
     object). PR-185 item (1) CLOSED + PR-176 item (5)
     CLOSED AS LAW: the gate values are CODE-AUTHORED in
     provider tails - the provider after `ret` at 0x1e092
     (0x1e0a0) IS THE PLAYER-LION CREATOR: call 0x500b0,
     class := PAIRED TABLES 0x97970/0x97968 (always
     primary/primary-8), [obj+0x12a] := 0, [obj+0xb6] &=
     ~4, ENERGY [obj+0xad] := 500 (THIRD writer - init
     500 vs respawn 100, video-gated check), ds:0xcef5c/
     0xcef60 := 1 HARD-CODED (ZARIA's K_F1/K_F2 legs ON);
     its 4 callers are the PR-176 name-match factories
     overriding [obj+0x8/0xc] := 0x96960/0x96958
     (TLewZaria) or 0x97a98/0x97a90 + [obj+0xf6] := 10000
     (TLewZariaB). PR-185 F1's 0x96960 confirmed,
     mechanism completed. 11 new items (0x9a54c; the
     [obj+0xc] header law; the capture hook; the 0x3edaf
     dispatch; id-17/19 remainders; flag-block field
     producers; the energy video checks; per-level gate
     enumeration...). Next -> PR-187 = the hardware-bottom/
     controller-family pass (0x9a54c, the class-record
     header, the capture hook). Full record: PR-186 section
     of NEXT_STEPS.md (scratch /tmp/opencode/pr186/).
   - PR-187 (READING; no port code, no behaviour change):
     THE 15TH CONTROLLER IS THE KADMAR-STAGE LION; THE
     INPUT HARDWARE BOTTOM IS RAW PORT I/O. 0x9a54c =
     TLewKadmar1's controller class (PR-186 item (7)
     CLOSED; PR-176 item (1) PART-CLOSED - the unresolved
     name was TLewKadmar1): factory 0x29b00 = the SECOND
     COMPARE FORM PR-176 suspected (`mov edx,'TLewKadmar1'
     call 0x76a69` THEN 0x5c320(0x12c) record-presence
     check) -> 0x27030 creates class pair 0x9a54c/0x9a544,
     WORD [obj+0x12a] := 0, [obj+0xf6] := 1500 (the
     0x45200 count, like TLewZariaB's 10000); the loader
     0x26ff4 loads 'kadmdead.mod' (via 0x562e0) just
     before the factory + re-arms [[0xcef40]+0x394]+0x106
     := 1; create-dispatcher
     0x27420 branches on cl bit 4 to sibling class 0x99500
     (whose getter is PR-184's wrapped 0x3dcb0) with the
     'patrz zrodla kad-1/2/3' error strings between. ITS
     GETTER 0x27280 IS THE ONLY DECORATED ONE: call the
     standard chain 0x504e0, then if [obj+0xee] >= 3:
     [obj+0xee] := movsx(WORD [obj+0x12a]) + 2 - THE PR-186
     FLAG DOUBLES AS THE KEY ORDINAL (flag >= 8 -> ee >=
     10 = the K_F11/K_F12 cases: a SECOND >=10 writer,
     PR-176 item (2) EXTENDED); clears key bit 0x10, sets
     it iff ee==6. +0x14 = 0x550a0, +0xcc = the same
     0x50340 input wrapper - the gate law is NOT ZARIA-
     specific. THE +0x84 owner hooks ARE NOW READ (2
     examples): state-name record ctors matching A_DEAD/
     A_LOOKUP/A_LOOKDOWN by name (two string blocks),
     requiring the 0x60 record type, creating via 0x524e0,
     storing state SUB-TABLES at [rec+0x44] (0x9b5f4/
     0x9a8cc/0x9a894; 0x9a8cc/0x9a894/0x9a85c). CLASS
     SWITCH-ON-OBJECT IS REAL via reclass sites like
     0x2bfc2 ([obj+0x12a] := -1, [obj+0x12e] := 1, pair
     := 0x9b4d4/0x9b4cc). THE [obj+0xc] PAIR-STORE IS AN
     ALIAS NOT A HEADER (PR-186 item (9) CLOSED): base-8/
     base-4 are that class's OWN method slots in all six
     sampled tables; ZERO dispatch sites through [obj+0xc]
     (5 scan hits, all 3-dword field-copy false positives
     hitting the CAMEL tables at 0x98274). PR-186 F1
     CORRECTED (1 bracket): 0x988ec (looser.mod/ZARIA) vs
     0x9d338 (ran1.mod/RAN) = same-role classes of
     DIFFERENT stages (20 slot-diffs, provider clusters
     0x241xx vs 0x31fxx) - NOT one object's life/death
     pair; flags are PER-OBJECT; cross-class flag semantics
     via shared handler code (0x24170 -> 0x32130) and
     reclass sites. HARDWARE BOTTOM (PR-186 item (4)
     closed to HW level; PR-106 item (1) answered in kind):
     boot 0x4d769 -> 0x58560 = keyboard-layer init 0x582d0
     (flush loop 0x1c4d0 + key-state memsets 0x1c490 over
     the 0xcde70 layer, 128-entry tables + xlat ds:0x9636c)
     + joystick presence probe 0x58130(dev+0x33); the
     sample readers are PORT I/O: 0x1ded4 = `in eax,0x50`
     (8253 counter clock) + `mov dx,0x201; out dx,al` PPI
     one-shot + `cli` `in al,dx` axis-timing loop (masks
     [ebp+0x10]/[0x14], cap 0x3fff) `sti`; 0x1df34 =
     `in al,0x201` button byte. The rings are FILLED in
     the 0x585e2 open body: ctor 0x582f0 zeroes [drv+0..8]
     + device presence probes 0x1c550(0xcde70, codes
     0x13b/0x202..) -> bits [drv+0x8/0x9]; 0x58010 opens
     the joystick sub-record; 0x57fe0 pulls samples ->
     `inc [drv+0xc]` + 16-byte ring writes (0x5872b/
     0x5876f-0x5877b). HARDWARE CAPTURE, not playback -
     but the ENTRY to 0x585e2 is unresolved (zero refs
     anywhere; ds:0xa97c0 device-pointer global written at
     shutdown - PR-188 item (1)). PORT VERDICT unchanged.
     9 new items (device-mgr entry + state law; flag/
     ordinal semantics; 0xcde70 format + 0x1c550 codes;
     reclass census; 0x99500; ...). Next -> PR-188 = the
     input-stack completion + flag-remainder pass. Full
     record: PR-187 section of NEXT_STEPS.md (scratch
     /tmp/opencode/pr187/).
   - PR-188 (READING; no port code, no behaviour change):
     THE INPUT STATE MACHINE, THE KEY->FLAG LAW, THE DEMO
     SYSTEM. 0x58690 IS A THREE-LEG MACHINE on [dev+0xb]
     (PR-106 item (1) CLOSED end to end): state != 2 =
     LIVE CAPTURE (0x582f0 zeroes the block + presence
     probes, 0x58010 reads the ports: buttons 0x10/0x20/
     0x40/0x80 -> bits 0x1/0x20, axis one-shots -> steps
     -0x14/+0x14 vs the calibration words [sub+0x00..0xc]
     - the +/-20 model at its producer); state == 1 =
     RECORDING, run-length coded: 0x57fe0 dedups against
     the last 16-byte record, match -> `inc [rec+0xc]`
     (0x5872b), else capacity-check (0x225a0 growth) +
     4x movs append - [rec+0xc] IS THE REPEAT COUNT, not
     a stride (PR-106 guess corrected); state == 2 = DEMO
     PLAYBACK walking [dev+0x2b] records, [dev+0x2f]
     mod [rec+0xc]. DEMO AUTHORING: level start 0x4d890
     builds a record NAME ('%s\%s' + '%d' at 0x91f19 via
     0x4a910), 0x56250 registers it into the 0xcf068
     subsystem table + 0x57a00 x3 probes -> [level+0x3ac]
     := 1, pumped by the frame loop through 0x561a0
     (registry [[0xcf068]+0x48], [entry.vt+0x0], edx=2)
     then zeroed. CORRECTION x3 (brackets): the capture/
     record legs live in 0x58690, NOT in 0x585e2 - 0x585e2
     is the open/enumerate entry (0x49b20 list builder +
     driver vtable+0x20); 'capture, not playback' was
     half right, BOTH run per state. [dev+0xb] := 1/2:
     NO writer in .text (full store-form scan) - external
     entry, counted; 0xa97c0 is a never-read shutdown
     counter (dead lead). KEY->FLAG LAW (PR-187 item (1)
     CLOSED): a Kadmar-2 arm tests LIVE KEY WORDS [obj+
     0x66],0x40 / [obj+0x67],0x08 / [obj+0x64],0x02 ->
     flag [obj+0x12a] := 2/1/0 + [obj+0x12e] toggle
     (0x2c27x); verb-3 handler 0x27300 INCREMENTS the
     WORD flag (throw counter) then calls 0x52c80(0x80,
     0xff,0x21)+0x50e60 - the 'patrz zrodla kad-1/2/3'
     strings are throw-limit errors; the 31-store flag
     setter census shows NO value >= 8, so the decorated
     getter's >=10 ordinal arm has no static [obj+0x12a]
     producer. QUERY CODES (PR-187 item (6) half-closed):
     0x1c530(9) = scancode&0x7f -> [0xcde70+edx*4+0xa]
     key-state table -> OR [block+8],8 ('any key down');
     the 0x808 leg is DEAD CODE - 0x58550 is an xor-eax
     stub. ds:0xa94f0 FULL CENSUS (PR-147 item (2)
     closed at setter level): a stage/player index bounded
     1..9 by its own 7 stores (inc/clamp, dec/clamp, :=5,
     :=9, dec-at-death); zero -> the frame loop skips the
     input push. CLASS-STORE CENSUS (PR-187 item (9)
     closed): 316 [obj+8],TABLE stores, exactly 14 cou-
     pled to the 0x500b0 allocator (providers + the re-
     class blocks + wrapped-class factories); the other
     302 are constructor bodies entered by the generic
     spawner.
     +0x100 VERB = PAIR/CONTACT/CARRY (0x50c70 for all 10
     controllers; fields [0x112]/0x118/0xe0/0xb1==0x14
     energy-copy/[other+0xee]:=self/ds:0xa94ec pair via
     0x531b0) - new counted law. PORT VERDICT unchanged.
     9 new items (+0x100 field law; registry law; level-
     meta tables; live-key producers; ...). Next ->
     PR-189 = the interaction-verb + subsystem-registry
     pass. Full record: PR-188 section of NEXT_STEPS.md
     (scratch /tmp/opencode/pr188/).
   - PR-189 (READING; no port code, no behaviour change):
      THE +0x100 VERB FIELD LAW COMPLETE: exactly TWO
      dispatch sites whole-image (0x41b6a in a jump-table
      arm, 0x50c2a in the key-state tail); THE DISPATCHER
      FOUND AT 0x41a50 (its only ref = data slot 0x9803c
      in the 0x980xx handler vector): [vt+0xe8], then the
      [obj.vt+0x104] TARGET-LOOKUP verb, FP cmp [obj+
      0x10b] vs [obj+0x12f] below -> `or BYTE [[obj+
      0x60]+1],1` (synthetic key injection), gate [obj+
      0x5c]==1, toggle [obj+0x106] 1<->2, selector BYTE
      [obj+0x160] -> `jmp cs:[eax*4+0x41a40]` (0x41af5);
      all four arms end with a 0x49580 record append.
      0x50c70 FIELD LAW: partner = [[self+0x112].vt+
      0xdc]() - the [obj+0x112] store census is EXACTLY
      16 sites, all constructor-family (AUTHORED PER-
      CLASS, never runtime); [self.vt+0x118] call; the
      0x14 energy-copy CONFIRMED (PR-186's [obj+0xb1]
      := 0x14 init IS the trigger); [other+0xee] := self
      is a POINTER; [other+0xb5] byte copy; [partner.vt+
      0xe0] call; ds:0xa94ec = THE DEFAULT-PARTNER
      GLOBAL with ONE writer (0x4cda9 inside the config
      parser, gated by 0x76d20(x,'animacje')) + 34 loads
      (33 `mov ebx` + one `mov ecx`, 0x4b897) - passed
      as the third arg to 0x531b0(self,partner,G), whose
      0x4b140 walk counts 'A'-records in [mgr+0x30] when
      self == [mgr+0x394], then 0x4aa70. REGISTRY LAW
      (PR-188 item (3) CLOSED): 0x56360 = event post,
      EXACTLY 2 senders (0x4aa07 edx=10 + 0x5df2b
      dynamic); 0x57990 = named-entry ctor, 116 callers,
      UTF-16 names byte-by-byte; the 0x4c9xx block
      registers EIGHT 'none' + '[' + ']' entries (config
      grammar); most internal registrations pass EMPTY
      strings; the 0x55e00 open contract installs the
      DEFAULT pair 0x96200/0x961fc + 0x57c60(name,
      'none') cmp -1. CORRECTION: the 0x561a0 pump
      vector is [[0xcf068]+0x44]/count +0x4c, NOT +0x48
      (bracketed at all 3 ledger mentions). LEVEL-META
      AUTHORING (item (4) CLOSED): [0x398]/[0x3a0]/
      [0x3a8]/[0x3b0] have EXACTLY 3 stores each;
      [0x3a8]/[0x3a0] are an on/off command pair
      (0x4ff3d/0x4ff49 vs 0x50416/0x50464); THE FRAME
      GATE 0x4ac76: [0x3a0]!=0 skips world construction
      straight into the input push; [0x3b0] is a ONE-SHOT
      deferred request consumed once (setter thunk
      0x51187); [0x3b8] is built by 0x4d91a via the
      0x961fc/0x96200 pair + the 'ANITAB' chunk. KEY-
      STATE METHOD 0x50970 = TLewEna2's +0x90 SIGNAL
      VERB (7 callers, bit-fed writes). LIVE-KEY WRITE
      LAW: ZERO direct stores at +0x60..0x67 (only the
      MANAGER ctor zero 0x4a613) - writes are 37 pointer
      handoffs + TWO synthetic writers (0x41a50 OR-1,
      id-17 OR-0x20); the 0x549f0 lead is DEAD (writes
      [obj+0xba]/[0xbe], not the key words). FLAG-BLOCK
      UNION (PR-186 item (5) CLOSED): [obj+0x118] has
      ZERO object disp refs (class-table verb slot only,
      0x50c50 x11); [0x116/0x11a/0x11e/0x126/0x12a]
      double as config-array POINTERS on the parser
      class (0x4cb92-0x4cbb6) - same-offset different-
      class; [0x120]/[0x124] pair, 21/12 store census.
      id-17==2 = state-0x17 key-bit injection |0x20;
      id-19 = range-clear ([obj+0xad] := -1); 0x99500 =
      EMPTY STRING (0x2742b print leg); 0x9f668/0xa0624
      = TLewTunga1/2 ALREADY NAMED in PR-104 (citation).
      7 new items counted. PORT VERDICT unchanged. Next
      -> PR-190 = the frame-loop + record-chain +
      handler-vector pass. Full record: PR-189 section of
      NEXT_STEPS.md (scratch /tmp/opencode/pr189/).
   - PR-190 (READING; no port code, no behaviour change):
      THE LEVEL LOOP 0x4abd0 IS ONE FUNCTION WITH EXACTLY
      ONE CALLER (0x5d251): head 0x4ac3b re-reads the
      player + SAMPLES INPUT (0x58690 into a 12-byte block)
      EVERY ITERATION; the three hotkey legs test the
      sampled bytes (bit [0x1f9].0x20 / [0x1f8].0x04 /
      [0x1f8].0x08) and run the FILE/STREAM family (name
      builders 0x5e030/0x5e0c0/0x5e100 + the 0x60xxx
      stream API); the end-of-frame label 0x4ae82 =
      stage test (ds:0xa94f0 == 0 -> exit ebx=2) else the
      input-push chain + 0x563d0 registry service + the
      plan-script step 0x4af10 + 0x4b0e0 + `inc
      ds:0xcef3c` (frame counter, 9th in this ledger's
      0xcef40-family globals). THE EXIT LAW (PR-147 item
      (2) CLOSED): ebx=3 = [[player.vt]+0x116]!=0 (the
      key-state field!), ebx=1 = [mgr+0x4]!=0 (AN EXIT
      WORD the world writes), ebx=2 = the stage global
      zeroed, ebx=4 = the hotkey path; the exit tail
      0x4ae53 stops the devices (0x1cfd0/0x1dba0/0x1cfe5)
      + pumps THE DEMO ONCE AT LEVEL END (0x4a9f0 census
      now EXACTLY 2: 0x4a857 level-start + 0x4ae6e) +
      `mov eax,ebx`; the caller stores CODE->[ebp-0xc]
      and [mgr+0x4]->[ebp-0x8] = THE LEVEL-CHANGE
      CONTRACT, then the level teardown (manager +0x3e6
      verb(2), ds:0xcef40 := 0, key-layer reset 0x1c490).
      THE 'HANDLER VECTORS' ARE CLASS TABLES (0x9686c-
      0x98288 = consecutive vtables; install census):
      0x9803c = the +0xec VERB SLOT of CLASS TABLE
      0x97f50 (4 ctor installs 0x3f570/0x44e5d/0x4573d/
      0x4e85b); the 0x41a50 jump-table arms are ITS OWN
      slots +0xf4/+0xf8/+0xfc/+0x100 = 0x41410/0x41460/
      0x41480/0x41600 (FP cmp [obj+0x10b] vs [obj+0x12f]
      = the +0x100 arm; 0x418f0 = [obj.vt+0xf0]-based);
      THE SELECTOR [obj+0x160] HAS EXACTLY ONE WRITER
      whole-image (0x415d3, inside the +0xfc arm 0x41480
      after its [obj.vt+0xec] self-call) - PR-189 item
      (2) CLOSED; 0x98274/0x98278 = THE UNIVERSAL
      BOUNDS-METHOD CLASS (extent / print-leg / point-
      writer / rect-contains, INSTALLED BY 85 CTORS) -
      PR-189's 'animation-table' READING CORRECTED (the
      0x27170 leg: out-of-bounds REQUEST msg via
      [obj.vt+0x10] against EXTENTS 0x143/0x18b, then
      live-key bit test). 0x49580 = THE LIVE-KEY
      PRODUCER: 69 E8 callers, 31 with +0x60-region
      targets (12 add-form + 19 lea-form) - verb results
      are OR-MERGED into [obj+0x60..0x6b]; NOT an
      appender (PR-88/101 OR-INTO law RESTORED; PR-189's
      'record append' phrasing CORRECTED x5 sites) -
      PR-189 item (1) CLOSED. [mgr+0x30] IS THE LEVEL-
      PLAN SCRIPT: 0x4af10 executes ONE CHAR/FRAME,
      alphabet 'A','B','L','T' ONLY else print 'Bledny
      opis poziomu - plany'; 'A' -> 0x4b360 spawn
      handler, 'L' -> [[player.vt]+0x74] + 0x4b2f0
      entity-chain tick ([mgr+0x64]->[node+0x34],
      [node.vt+0x60]/+0x74), 'T' -> camera scroll ops,
      'B' -> 0x448c0/0x4a170/0x49bd0 leg ([esp+4] =
      [mgr+0x3ce]); the 0x4b140 walk = count AUTHORED
      'A' spawn-requests up to 'L' + membership test
      0x4b0f0 (the manager's 3 object vectors
      [+0x44/48/4c]x[+0x58/5c/60] + [node+0x34] chains,
      cap 5) - the pair law's 'do I own my pair?' test;
      ZERO disp-form stores to [reg+0x30]/[reg+0x34] -
      author pointer-fed, counted. THE CAMERA ([mgr+
      +0x390]): 0x576e0 SET-SCROLL to player minus
      0xa0/0x64 = THE 160/100 CENTER RUNS EVERY
      ITERATION; 0x57600 APPLY-scroll (0x50 tile stride)
      - and 0x54450 (PR-189 F4's tail) = A SCREEN-
      POSITION GUARD vs [rec+0x2]+0x1f4, re-framed. 8
      new items counted (file-hotkey semantics, plan 'B'-
      leg + the [mgr+0x4] writer, class identities first).
      PORT VERDICT unchanged. Next -> PR-191 = the level-
      plan authoring + file-hotkey + class-identity pass.
      Full record: PR-190 section of NEXT_STEPS.md
      (scratch /tmp/opencode/pr190/).
   - PR-191 SHIPPED (docs-only): THE HOTKEY LEGS ARE
      LOCALIZED '.mnu' MENU-SCRIPT RUNNERS - the level
      loop's three legs read KEY-EVENT bits (0x582f0
      feeds: 0x13b=F1 -> [block+9].0x20, 0x50='P' ->
      [block+8].0x04, 0x1b=ESC -> [block+8].0x08;
      0x1c550 = the latch CONSUME), build names from ONE
      22-entry table at 0xa9a50 = 7 groups x 3 languages
      (help/to-menu/pause/reset/continue/options/
      calibrate, PL/EN/DE, selector ds:0xa9aa8), and RUN
      the files through THE SAME SCRIPT ENGINE as levels
      (0x603b0 -> [stream.vt+0xc] + the 0x5f310 chunk
      interpreter). ESC opens 'domenu.mnu'; THE MENU'S
      RESULT == 1 IS THE ONLY EXIT-4 SOURCE; F1 re-opens
      'pomoc1.mnu' until it returns 0 (modal help); P
      runs 'pauza.mnu' synchronously. THE PLAN MODEL IS
      CORRECTED: 0x4af10 REPLAYS THE WHOLE SCRIPT EVERY
      CALL (the cursor is local; ZERO stores to [mgr+
      +0x30] whole-image in every disp form), dispatch is
      TOUPPER-gated (either case), and THE 'A' LEG IS AN
      INDEXED TICK OF THE THREE AUTHORED OBJECT VECTORS
      (0x4b360(mgr,k): k-th entry of [+0x58/5c/60] past
      counts [+0x44/48/4c], +0x34 chains, predicate
      [vt+0x60] -> action [vt+0x74]) - NOT a spawn.
      0x97f50 = A SHARED AUTHORED-OBJECT CLASS TABLE (4
      install sites, zero static refs = computed dispatch;
      the level manager is one of its instances via its
      +0xcc/+0xd0 verbs), its +0x58 verb 0x4e710 IS ITSELF
      A CHUNK-TAG CHAIN, and THE LEVEL FORMAT'S 31-TAG
      CHUNK VOCABULARY IS RECOVERED WHOLE (ANITAB..CHEATS
      at 0x91dc0-0x92120 + 'PLANT' 0x90755: GRABOBJ,
      MOVEPLAT, ZWIERZ, TABLICA, BERLO, LINA, DZWIGNIA,
      WODOSPAD...) = the data-spine anchor for every
      format PR. THE 'B'-LEG DATA FLOW IS READ (WORD
      records at 8-byte stride -> globals ds:0xcef18/
      0xcef1c; center-cell target 0x4a170; both data
      objects authored at level load 0x4d56f/0x4d620).
      PR-190 items (1)/(7) CLOSED, (2)/(3)/(5) half-
      closed, (6) measured-and-counted; 8 bracketed
      ledger edits (7 correction sites, 4 closures,
      2 half-closures, 2 extensions); 8 new items counted (the
      .mnu stream classes, the tag->ctor map, the exit-
      word writer first). PORT VERDICT unchanged (no
      stage behaviour touched). Next -> PR-192 = the .mnu
      menu engine + the tag-factory map + the level-load
      file sequence. Full record: PR-191 section of
      NEXT_STEPS.md (scratch /tmp/opencode/pr191/).
- PR-192 SHIPPED (docs + fixture reading): THE TAG-FACTORY
  MAP IS DONE - 0x76a69 (strcmp) has EXACTLY 386 E8 sites,
  361 are tag arms (`mov edx,<tag>` within 24B) in EXACTLY
  16 factory chains (per-stage vocabularies 0x2f8xx..
  0x716a4 + the script-command table 0x55e7d ('end_level'
  = AN AUTHORED SCRIPT COMMAND, one init 0x45770) + the
  .MOV action/key vocabulary 0x58009 ('AC_*','K_FWD',
  'K_F1'..'K_F12' - the engine twin of the port's .MOV
  mirror)). Match arm law: `mov eax,<size>; call 0x5c320`
  (617 sites) + a per-type initializer; THREE UNIVERSAL
  INITIALIZERS carry the types (0x52860 geometry 62
  callers / 0x531f0 static props 70 / 0x53ff0 movers 188)
  + per-type ctors 0x61fb0..0x73e50 installing per-type
  class tables directly. CLASS-TABLE LAW: exactly 503
  `mov [reg+8],<table>` sites in 176 tables (all mod-01;
  PR-188's 316 was a partial family - superseded; THE
  PR-18x 302-SPLIT RESOLVED: dispatch = the 16 chains,
  table stores live in per-type registration builders).
  TABLE FAMILIES: 0x9815c = ZONE/RECT class (20 installs:
  GRABOBJ*/SPAD/WODOSPAD/TABLICZKALONA/PICT), 0x981e8 =
  GROUP family (GRUPA/GRABG), 0x98078 = a 4-site family.
  FILE MODEL: FOUR root path buffers 'ANIMACJE\','ETAPY\',
  'SOUND\','LEVELDES\' + boot init 0x5c600 (reached ONLY
  from a {code,code,flags} registry table at 0xad8c0 -
  computed dispatch again); 0x4a910 = open(ETAPY\name)
  (15 callers: menu opens + level-load opens). THE .des
  LEVEL-DESCRIPTION FORMAT: parser 0x4c723 (27 key legs,
  24 keys incl. 'plany'/'lew'/'animacje'/'belka', zero
  static refs) + THE [mgr+0x30] PLAN FILL RECOVERED (the
  'plany=' scan writes straight into [builder+0x30] via
  0x4cba1 `lea [esi+0x30]`) - PR-190/191's plan-author
  CLOSED; fixture: LEVELDES holds 67 'plany=' records with
  THREE '|'-separated plan slots ('plany=T|TALA|ATAB') =
  the three vectors of the 'A'-tick law, and 'lew=TLewEna,
  ...' meets the factory chains' 'TLew*' arms. THE 31 TAGS
  RECLASSIFIED: they are the ANIMACJE object-description/
  authored-type vocabulary (fn 0x5dcc5 opens ANIMACJE;
  20/31 tags appear as plaintext in the member) - level
  DATA = ETAPY\*.IS BINARY (zero plaintext tags); .mnu
  files open under ETAPY but are NOT in the stage fixture
  (ETAPY carries the .seq sound family: 'pomocNN.seq' up
  to 42, 'pauza/energia/crown/cyfry_*'). STREAM CLASSES
  DECODED (0xa2d90/0xa2d6c differ only at slot +0x20 =
  0x60960 vs 0x4be20 THE LEVEL LOOP; run verb at [tbl+
  +0x1c], PR-191's call [edx+0xc] CONFIRMED by bytes) -
  PR-191 items (2)/(8) CLOSED, (5)/(6) half-closed; exit-
  word verb set re-measured (0 register-form [reg+4]
  stores across 15 fns - stays counted). 8 bracketed
  ledger edits (1 reclass, 1 census-resolve, 2 closures,
  2 half-closures, 1 confirmation, 1 fixture note); 6 new
  items counted (.IS binary chunk format, ANIMACJE record
  format, the 'levels' config chain first). PORT VERDICT
  unchanged (the .des/'plany' three-slot law MATCHES the
  port's level model; no discrepancy; no code change).
  Next -> PR-193 = the .IS chunk format + the ANIMACJE
  record format + the 'levels' config chain. Full record:
  PR-192 section of
  NEXT_STEPS.md (scratch /tmp/opencode/pr192/).
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

