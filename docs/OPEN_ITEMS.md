# docs/OPEN_ITEMS.md - the open-items register

One row per open thread that matters across PRs: stable id, status,
origin, evidence pointers, and what would close it. `make verify-docs`
P6 pins the mechanics: every `OI-nnn` cited in the live docs must
resolve HERE, ids are appended-only (never renumbered, never deleted -
closing flips the status line), and every row carries a status.

STATUS VOCABULARY: `OPEN` (still a question), `PARTIAL` (a named
remainder of an answered whole - the row lists what remains), `CLOSED
BY PR-nnn` (kept so citations keep resolving), `FROZEN (owner)`
(owner ruled: do not touch), `DEFERRED (owner)` (a decision with a
number on it, waiting for the owner), `DEFERRED (ruled)` (parked by a
standing ruling, e.g. the Windows end pass).

WHAT IS NOT A ROW: per-PR counted items that live and die inside one
record - that record IS their register (records since PR-196 carry an
OPENS: line; older records list counted items in body). A row here
means a thread that spans or survived several PRs, or a decision
someone must eventually make. Task queues (HANDOFF section 8, the
plan) are plans, not questions. Method notes and measurement traps
(the one-byte listing trap, edited-clip cut points) stay with the
method notes where they are already written.

SEEDING (PR-201 pass, measured): HANDOFF section 7 items 0-15 -
that ledger is now FROZEN verbatim at
docs/archive/HANDOFF_SECTION7_OPEN_QUESTIONS.md (PR-202), and the
"HANDOFF 7 item N" origins below resolve there -, the journal's
`## Deferred big items` list, the OPENS/CLOSED fields and carried
lists of records PR-185..PR-200, the ZARIA.01 audit gaps
G-1..G-13, and the owner rulings of 2026-11-19. PR-195 estimated
"about 40 rows"; the measured seed is larger because the earlier
estimate never enumerated section 7's sub-threads.
ORIGIN ids that are not OI ids resolve at stable archive paths:
P/E/M- and T- at docs/archive/open-leo_native_port_review.md,
T- also at docs/archive/open-leo_porting_tips.md, X/W/L/B- at
docs/archive/PORT_DEEP_DIVE_2026-11.md, "HANDOFF 7 item N" /
"Q0" / "G-nn" at the frozen ledger named above (the
history-override table maps every superseded sha).

CITING RULE (for every writer): when a record or the plan restates a
thread that has a row, cite the id instead of re-describing it. A new
thread gets a row in the SAME commit, id = next number, and the
record's OPENS: line says so.

=== engine RE: motion, frame law, row gate ========================

OI-001: instance facing XOR for states WITHOUT a CH_DIR state header
  status: OPEN
  origin: HANDOFF 7 item 0.4 (PR-79/81/82 era)
  notes: both readings agree everywhere testable; the named settling
    tests (RAN crown/balloon dual-polarity, a left-facing STANDING
    lion, a worm standing after a turn) are all capture work.

OI-002: object state-header `T=` semantics against the object
  velocity law that was read later
  status: OPEN
  notes: the law itself is read (PR-99/100/129/130/133/135/168/178);
    the header-word interpretation PR-83 flagged "must be revisited
    in T1" was never re-pinned against it. THE VIDEO TEST IS RUN
    IN PR-206 and the register model is REFUTED on it: LONA.05's
    YETI (BY_X, A_RUN only F nonzero `F=100,0` `T=50,5`) walks on
    film through camera-locked windows; the shipped arithmetic
    predicts 19.8-20.0 px/s and every measured burst exceeds it
    (row 2 of OI-107). So the video does NOT settle T semantics
    through the register law - the contradiction is its own row,
    OI-107, whose first lead is the un-read second value of each
    pair, which PR-98's map leaves at rec+0x54/+0x5c.

OI-003: the F/FC frame-directive output pair: what it does to world
  motion (keep it distinct from the state-header force word)
  status: OPEN
  origin: HANDOFF 7 item 1; PR-98/99/100
  notes: do NOT port a velocity/impulse reading yet; PR-121/122 read
    parts of the machinery ([obj+0xea] one-shot amount, the
    two-point gap model) - modeling asks are OI-009.

OI-004: the row-admit gate: which half of the BY_X / `F=0,0` gate
  the engine keys on, and the row-cursor consumption semantics
  status: PARTIAL
  origin: PR-110/114 (HANDOFF 13.77); PR-130..164 cursor law
  notes: the row cursor and its two writers are read (PR-130/132/
    159/162); the mode-only reading has a concrete counter-example
    (5 no-exit states) and the final consumption semantics of a
    completed cursor stay counted.

OI-005: one row per record step vs the port's loop: the engine
  keeps row-debt, the port drops it
  status: OPEN
  origin: PR-115 (HANDOFF 13.78)
  notes: needs a capture referee (T5/T10), not a disassembly.

OI-006: strict-past row firing: `acc >= thresh` vs the engine's
  strict-greater
  status: DEFERRED (owner)
  origin: PR-116 (HANDOFF 13.79)
  notes: reading closed (the engine fires strictly past); flipping
    the two gates moves 15066-15317 authored rows by one whole feed.
    The decision is the owner's, with the number on it.

OI-007: the port's double accumulator drifts off its own exact-
  landing law (LONA.05_LEW, |F|=30, dt=20)
  status: OPEN
  origin: PR-118 (HANDOFF 13.81)
  notes: no selftest in either family reaches that case.

OI-008: which accumulator the engine keeps for a WALKING state
  status: OPEN
  origin: PR-119 (HANDOFF 13.82)

OI-009: engine fidelity asks, measured and awaiting an owner call:
  reproduce the truncated move-by-dx step (sub-pixel dropped at the
  divide 0x5435f), and the one-shot vertical amount as a gap to a
  two-point segment (0x45380, slot +0x6c provider, no zero-denom
  guard)?
  status: DEFERRED (owner)
  origin: PR-121/PR-122 (HANDOFF 13.84/13.85); PR-120's quantisation
    test measured the drop before the ask

OI-010: the `.MOV` parser clamps `T=` to 130 while 172 authored
  components sit above it
  status: OPEN
  notes: any future drag/velocity change must apply the cap (the cap
    itself was never re-examined); tools/re/mov_channels.py counts.

OI-011: shipped discrepancy: the port drives the player walk to
  T=20 (= 20 px/s) while the measured walk is ~25 px/s; the worm's
  4-unit poses reproduce at 25 (PR-135) but the lion leg disagrees
  status: OPEN
  origin: PR-79/99/131/135 (HANDOFF 13.88); G-1 referee windows
    (PR-131/134: LEWPL has no A_RUN state - the A_WALK->A_RUN entry
    is an .MOV row question, answer it from data not video)

OI-012: does the PLAYER object go through the 0x54250 step at all?
  PR-135 applies the law to both actors as the model; the three
  call sites do not check class
  status: PARTIAL

OI-013: the [obj+0x95] flag that turns `I`/`IC` rows on; the
  [vt+0xbc] test and byte +0xb6 bit 2 behind the drag floor 20;
  the caller of the row-list dispatcher 0x45df4; the [entity+0x95]
  latch consumer
  status: OPEN
  origin: PR-98/99 (HANDOFF 13.90); the [obj+0x95] reader was
    re-counted by PR-142/161/164 - still no reader found

OI-014: ds:0xcef40 +0x388 as a frame-advance distance accumulator
  status: CLOSED BY PR-100
  notes: a constant 100, re-confirmed by PR-168/178 (the rate is a
    literal 100/tick). Row kept for citation stability.

OI-015: the PR-135 non-modelled list: vertical constants not
  re-fitted (jump apex 20 px vs ~24 px - a known regression), the
  engine's conditional raise of T to >=20, partial `s` between 0 and
  +/-10, the object's own velocity register, velocity across state
  change, state-change feed consumption, the `nfr == 0` guard,
  LL_PLAY_V_EPS as a model choice, the `vy >= 0` ground-snap guard
  no leg distinguishes; and the 50 ms iteration being PR-134's
  video-chosen edge of PR-109's 28-33 ms code bracket, not a read
  of the engine's loop
  status: OPEN
  origin: PR-135 (HANDOFF 13.8)

OI-016: the BY_TY vertical feed law
  status: CLOSED BY PR-147/PR-148/PR-168/PR-178
  notes: the same constant 100 behind a `cmp [obj+0xbe],0` gate;
    shipped as LL_CTRL_BY_TY/vert_intent; no port gap. The climb
    BEHAVIOR remainder is OI-078.

=== engine RE: signals, conditions, event system ===================

OI-017: is the live signal set EVER CLEARED (pulse vs latching)?
  status: CLOSED BY PR-151
  notes: 0x749a0 clears it; the port's evaluate-then-clear ordering
    confirmed. Row kept for citation stability.

OI-018: raisers and semantics of AC_ONAIR, AC_DUP, AC_DRECT,
  AC_EDGE, AC_DRAB_CATCH
  status: PARTIAL
  notes: AC_ENEMY_ATC raised and ruled (PR-91/94/139); AC_BLOK/
    AC_BLOK_Y raisers PR-94 + relation providers PR-177; AC_EDGE
    via the ladder answer PR-141/149; AC_DRAB/AC_DRAB_CATCH raise
    PR-141, poster PR-146, and bits 12/22 have NO poster anywhere
    (counted negative, PR-146). AC_ONAIR's raiser and the semantic
    modeling of all five remain.

OI-019: what the object-side AC_ENEMY consumers do (A_DEAD(1)/
  A_AWAY/A_SHOW class conditions)
  status: PARTIAL
  notes: raise and delivery are read (PR-94/143/146); consumer
    behavior is the named next feature candidate (PR-89 item 9.5).

OI-020: signal-record posting and delivery: the key-bit posting
  path ([vt+0xc](0x13), the 0x34a35 lead) and the queue-drain
  caller of 0x52b52
  status: PARTIAL
  notes: broadcast census, poster 0x287a0, pair-walk entry 0x4bd30,
    node builders all read (PR-143..149); the key-bit path and the
    drain caller were still counted when the ledger stopped (the
    climb chain is OI-078).

OI-021: the evaluator's remaining counts: [rec+0x10] threshold
  units vs row feeding, K_ANY marker honour, the `!` complement
  representation
  status: OPEN
  origin: PR-142 counted items, carried in PR-143/144 lists

OI-022: who fills the ds:0xcf464 queue and with what records - and
  whether the port ever should have one
  status: OPEN
  origin: PR-193 item (1), re-listed in PR-194 `## Next`

OI-023: event-id remainders: consumers for ids 17/29, the deep-form
  arms for ids 13/15/21/23, the id-9 FEED sender, the byte-form
  raise bit-numbering, targeted-arm bodies
  status: OPEN
  origin: PR-183/184/185 counts
  notes: id 30 dissolved (PR-185), id-25's pair-sync meaning was
    named (PR-184); the rest never re-mined.

OI-024: msg/event plumbing remainders: the [obj+0x102] reader, the
  [obj+0x28]/ds:0xcf2c8 registry, the [rec+4] guards
  status: OPEN
  origin: PR-171/172/182 counts

OI-025: link/group/pool field semantics: the [obj+0x30] array
  (index 1 was never proven to be the lion), [obj+0x112]/
  ds:0xcef8c, the token->family bookkeeping map
  status: OPEN
  origin: PR-146/184 counts

OI-026: energy: [obj+0xa9] semantics (site map complete), the
  [obj+0xad]/[obj+0x75] readers, the THIRD writer [obj+0xad] :=500
  vs init 500 / respawn 100, and the -30 energy signal - all
  video-gated checks, NOT port-changed
  status: OPEN
  origin: PR-173/186 counts

OI-027: lion flag-block field producers: [obj+0x10e] and the
  0xb/0xc passthrough, the [obj+0x120]/[obj+0x124] pair role
  status: OPEN
  origin: PR-185/186/189 counts; PR-190 carried "[0x120]/[0x124]
    role (lead extended)"

OI-028: flag values 4..7 and what triggers the 0x27410 :=3 arm
  status: OPEN
  origin: PR-188 item (7), carried PR-189/190/192

OI-029: input-table remainders: the key-bit 0x10/ee==6 names, the
  rest of the 0x582f0 key->bit map (gameplay keys)
  status: OPEN
  origin: PR-187/191 counts

OI-030: the [obj+0xf6] authoring map (1500/10000 per stage; the
  0x45200 counts)
  status: OPEN
  origin: PR-187 item (7), never re-listed

OI-031: the ds:0xa94ec load-sites' per-consumer semantics
  status: OPEN
  origin: PR-189 item (3), carried PR-190/192

OI-032: level-meta remainders: the 0x43fb0 result stored to
  [level+0x398] (7 readers, none individually read), the 0x5df2b
  dynamic event code
  status: OPEN
  origin: PR-188/189 counts, carried unchanged into PR-190

OI-033: the +0x240/+0x2cc computed dispatch: no static entry into
  0x27d60 (negative scans for any +0x240 access)
  status: OPEN
  origin: PR-180 headline count; carried by PR-192's list

OI-034: the [[obj]+0x126] flag and the [mgr+0x394] sensor anchor
  (its producers / 0x1eb9a "stay" per the ledger)
  status: OPEN
  origin: PR-104 (HANDOFF 13.70), PR-177 item (4) remainder

=== engine RE: objects, AI, placement, classes =====================

OI-035: the engine's compiled per-class AI DRIVER: the [obj+0x106]
  MAIN/WALK/ATACK/TURN tables (0xa3054-0xa30a8), ATACK::check
  0x61cd2, the facing getter, the random gate 0x3fee0, the other
  classes' boxes (SNAKE 0x64830, PAJAK 0x626c0, NOSOROG 0x63750,
  PAPUGA 0x53ff0); whether the compiled AI reads DANGER for arming;
  whether armed A_ATAK walks the worm; the mailbox at [obj+0x0..0xb]
  vs the port feeding the object's own sig set; the [obj+0x102]
  countdown gating the raise
  status: PARTIAL
  notes: the port ships the pinned LICHA box (|dx|<40, |dy|<50) as
    a stand-in behind LEO_PLAY_DANGER. PR-90/91/92/94 counts.

OI-036: which factory-template instance is which .des TYPE (class
  vtables -> type numbers)
  status: PARTIAL
  notes: PR-96 traced the structure; many classes named PR-155/156/
    157/176/185/187; the port still ships the 0x34103 dispatcher
    default 30 until the decode completes (PR-89 item 9.3).

OI-037: the 18 .MOV state blocks that open ATC_ON and never close
  it: what clears it in the engine
  status: PARTIAL
  notes: PR-139 named the applier 0x45c20 + the 30-tick countdown
    [+0xe4]; the per-state authoring question stays (PR-89 9.2).

OI-038: the class apply end pass: the other 22 class apply methods,
  the 6 pure-walk classes, and the second apply site 0x51101 - a
  DIFFERENT object shape whose type is unidentified (the top lead
  inside this row)
  status: OPEN
  origin: PR-198/PR-199 OPENS

OI-039: the engine TRIGGER that spawns attached children (every
  `0,0` placement entry: corner fruits, the ZARIA mat following the
  rhino, the two POCISK/jajo children - G-4's data half)
  status: PARTIAL
  notes: child machinery read (PR-156 F1 sub-lines, PR-167 child
    aggregator 0x553e0); the trigger/instance side and the follow-
    while-parent-walks question remain; the port joins and counts
    those scripts but never instantiates them (PR-93/95).

OI-040: are PICT/.SEQ statics ANIMATED?
  status: OPEN
  origin: PR-93 item 11.1
  notes: the absence of data in the fixtures is the constraint;
    other levels' trees would settle it.

OI-041: class-name remainders: 0x98078/0x9815c/0x981e8 semantics
    mapped but names pending, the 'XxXx' pair strings, parser-class
    table 0xa1310, TLewEna/TLewSades/TLewLona/TLewTunga2 class
    tables, the 15 class names, the CAMEL/0x98278 role (a generic
    decor/mover in NEITHER key lineage)
  status: OPEN
  origin: PR-176/187/190/191/192 counts

OI-042: the full 0x49580 field map (38 non-+0x60 fields; "a full
  map needs per-caller data flow")
  status: OPEN
  origin: PR-190/191 counts, carried PR-192

OI-043: the [mgr+0x3ce]/[mgr+0x3d6] writers through the 0x4af36
  pointer
  status: OPEN
  origin: PR-191 item (4), carried PR-192

OI-044: the config-parser object class owning the 0x4cb92-0x4cbb6
  pointer bindings (table 0xa1310 mechanics were read PR-192 F4;
  the owning class stays counted)
  status: OPEN
  origin: PR-189 item (5)

OI-045: the carried-list remainder (each item last appears in a
  PR-185..193 CARRIED list; resolve the detail at those records):
  the class-byte/name map, the 0x54458 class-range energy rule, the
  0x2c670 placement, the state-copy helper, the [obj+0xb6] bit-2
  setter, the CFG windows 0x54790-0x547b5, the +0x54 hook bodies,
  0x550c0's ordering target, the 0x55130 tail, the +0x4c/+0x1c
  recount, the 73 dead 0x54910 tables, the +0xa0 census oddities,
  targeted-arm bodies, 0x45380/0x455a0, the pool chain-store hunt
  (third re-point), the ratio-law study (video-gated), the other
  +0xcc drivers, [rec+4]/ds:0xcec70 semantics
  status: OPEN

=== engine RE: level loop, exit, menu, plan =======================

OI-046: the end_level contract: authored node -> apply method
  0x45750 -> the exit word at [mgr+0x4] -> the level loop's return
  -> the reset/continue selection
  status: CLOSED BY PR-198/PR-199
  notes: engine side read end to end (PR-198, correcting PR-194's
    ruled-out claim); the port implements the contract (PR-199).
    Row kept: several records cite this thread.

OI-047: what the reset pair 0x5da60/0x5d050 does, and what the
  per-stage edx 0/1 (wrappers 0x5d3c8/0x5d436) selects - until
  then the port's reset stays the named ESTIMATE it is
  status: OPEN
  origin: PR-194 `## Next` (3); the combat fields were left out of
    the port's reset precisely because no reading covers them

OI-048: what raises code 0 (the driver's stop): who zeroes
  ds:0xa94f0 / what requests the stop
  status: OPEN
  origin: PR-198 OPENS

OI-049: what feeds the 0x603b0 menu-result leg (the ESC-menu
  result == 1 is named as the exit-4 source; the producer of the
  RESULT is not)
  status: OPEN
  origin: PR-190/PR-193/PR-198

OI-050: the `.mnu` opcode format and its interpreter: 0x5f310 is
  out; 0x603b0's [obj+0x2c]/[obj+0x34] object walk is the
  remaining candidate
  status: OPEN
  origin: PR-193 RETRACTION; the `.mnu` family is NOT shipped with
  the stage data (PR-192 F3/F5, standing measured fact)

OI-051: 0x5eb11's caller, and why it discards the two-dword pair
  after setting the stage index to 9
  status: OPEN
  origin: PR-193 FINDING 1, PR-198 OPENS

OI-052: what consumes the 0x23ea0 two-view cursor (the point
  writer itself is read whole); neighbours 0x23eb0/0x23ed0 noted,
  not followed
  status: OPEN
  origin: PR-198 OPENS

OI-053: the stage RE-ENTER path: the engine's caller loops with a
  NEW level object (0x5d1f5 allocates, 0x5d233 runs the ctor that
  zeroes the exit slot); the port has no re-entry and ends the
  level run at the guard
  status: OPEN
  origin: PR-199 OPENS

OI-054: a SCENE OBJECT's row admit cannot reach the exit-word slot
  (the port's owner is the LION; all shipped rows are LEW.MOV -
  counted, unmodelled design call)
  status: OPEN
  origin: PR-199 OPENS

OI-055: whether `end_level`'s 1/2 in A_TUNGA1/A_TUNGA2 is the
  TUNGA I <-> II exchange of T6 (AC_XCHG_LEVEL)
  status: PARTIAL
  origin: PR-194 `## Next` (5)
  notes: answered out-of-band by PR-198 (the values are behaviorally
    identical; code 1 is NOT the exit-4 leg) - the authored number
    selects nothing, but no record says CLOSED in those words and
    the T6 mechanism itself stays unread.

OI-056: the plan stream: 0x44xxx/0x49exx plan-data class record
  bodies and the level-load 0x4d5xx per-file open order
  status: OPEN
  origin: PR-191 item (5), half-closed by PR-192 F4, carried

OI-057: the boot registry table 0xad8c0-0xad91c entries:
  0x514e0/0x59dc0/0x5c260/0x611e0 unattributed
  status: OPEN
  origin: PR-192 item (5), never re-listed

OI-058: the 0x1c550 key-latch consume is read (PR-191 F1); the
  full 29-feed census's gameplay feeds stay counted
  status: OPEN
  origin: PR-187/191

=== engine RE: input, devices, language ===========================

OI-059: the device sample's third dword: axes vs buttons naming
  status: OPEN
  origin: PR-186 item (8), renamed PR-187 item (8), never resolved

OI-060: the 0xcde70 key-table format, xlat 0x9636c, 0x9a85c
  status: OPEN
  origin: PR-187 item (6) remainders (the 0x1c530 128-entry table
    itself closed by PR-188 F3)

OI-061: why the SECOND handle exists (the consumer of the [obj+0xc]
  alias) - the header-layout question itself closed as a NON-LAW
  status: OPEN
  origin: PR-186 item (9) remainder, PR-187 item (4)

OI-062: the language-selector writer (ds:0xa9aa8) and the runtime
  assembly of the `.TLO` names
  status: OPEN
  origin: PR-191 item (1), re-listed PR-193 item (7)

=== engine RE: data formats =======================================

OI-063: the `.IS` level-DATA binary chunk format (the container IS
  decoded - ported/is_stream.c; the chunk layout is not)
  status: OPEN
  origin: PR-192 item (1); PR-193 re-listed: "STAYS OPEN"

OI-064: the ANIMACJE record format: the per-tag description layout
  inside the member - bounded since PR-193: every flag-2 instance
  is 5200 bytes
  status: OPEN
  origin: PR-192 item (2), carried bounded PR-193

OI-065: ZAR.BLK fields + the GRP format (WODOSPAD/KIJ/KILOF), the
  KAMIEN sixth field (1308/1218/1115) and the stone corridor
  1299..1620 (G-3) - read ZAR.BLK like a .PLT
  status: OPEN
  origin: PR-93 item 11.4; the placement lines naming them are
    counted as group headers, not placed

OI-066: the 84-byte STATUS.LEO record: field meaning AND the field
  map vs the port's ll_save codec (no test verdict yet); the map's
  progress arrows and 'start' variant depend on it - needs a PAIRED
  capture (progress made + save read back)
  status: OPEN
  origin: PR-193 item (3) / FINDING 4; HANDOFF 7 item 5; T-17
    territory; G-8's map half

OI-067: the six stream classes' identity: what differs at
  +0x04..+0x18 and which file kind each carries; the +0x00 slot as
  a base-link (0x5f330) and the +0x24.. triple repeat
  status: OPEN
  origin: PR-191 item (2) -> PR-192 item (4) -> PR-193 item (4)

=== camera + ground ===============================================

OI-068: camera x lead: CAM_LEAD_R=100 CONFIRMED by landmark
  anchors; CAM_LEAD_L=220 sits in a weak band and is pinned by TEST
  not evidence
  status: PARTIAL
  origin: G-2; PR-136/137

OI-069: camera catch-up RATE and the eases (6 x / 2 y per
  iteration) are invented models; measured video says p50 5 /
  p95 60 / max 230 u/s against the port's 120
  status: OPEN
  origin: PR-136/137

OI-070: the airborne camera hold is "a model the data doubts"
  status: OPEN
  origin: PR-73/136/137

OI-071: the PCX-backdrop vertical camera against a TALL level
  (needs an SADES shaft capture)
  status: OPEN
  origin: HANDOFF 7 item 7.1 (the engine formula was adopted PR-65/
    84; the no-back_rect clamp source is settled)

OI-072: the ground SOURCE remainder (G-13): the port loads ground
  from `platformy=` only and never parses the ani_plat= LOCAL
  ground lines (ANIPL2: KATAPULT/PALMA); engine-side the entity-
  record table chain (0x34850, [obj+0x48]/[obj+0x4c]) and its
  MOVEPLAT-chain provider are unread
  status: OPEN
  origin: PR-136/138/140

OI-073: the engine's grounded-y bookkeeping: the instruction that
  turns the shape bottom into the grounded y is unread; the port's
  `pos = line - contact` is an inference
  status: OPEN
  origin: PR-97 item 12.1

OI-074: elements with no margin lists (the 34 all-empty-margin
  ones): contact 0 or something else - "no evidence either way"
  status: OPEN
  origin: PR-97 item 12.2

OI-075: re-grounding after an `M dx,dy` delta or `P oz,px,py`
  write: untested (the legs using them have states with no `.SEQ`
  art in the fixtures)
  status: OPEN
  origin: PR-97 item 12.3

OI-076: does the engine re-test EVERY object every tick (continuous
  re-grounding; the `G=`/`CHECK_LN` pair suggests it does; only
  obj_settle follows terrain in the port)
  status: OPEN
  origin: PR-97 item 12.4

OI-077: fixture hygiene: the five drawn-anchor test legs use a
  colour mask that catches same-coloured scenery - they are
  regression tripwires, not measurements; window the sprite from
  its own bbox
  status: OPEN
  origin: PR-97 item 12.5

=== gameplay behavior + capture-needed ============================

OI-078: the in-game ZARIA.01 climb: what still blocks it - the
  K_GEN1 arm + WAIT row-gate, the per-frame setter of the lion's
  contact target [obj+0xd2], the authored rectangle format, node
  lifetime + the 0x54b6a merge guard, object-asked answers and
  mid-air asking
  status: PARTIAL
  notes: poster, pair-walk entry, node builders, feed law, guard,
    evaluator, ladder answer all read and shipped (PR-142..150);
    the named remainder lives in those records' counted items.
    ZARIA.01 x=180 still does not climb.

OI-079: AC_DUP: the engine's attach machine is named (PR-179) -
  model it? what ends a JUMP early (does the original cancel
  recovery on the next input?)
  status: OPEN
  origin: PR-86 item 8.8, PR-150's ladder-overlap model, PR-179

OI-080: SURPRISE '?' firing (G-12); the factory branch lead is
  0x4e62f
  status: OPEN
  origin: PR-131/G-12; PR-149 named the route

OI-081: the level-end scene (purple-robed NPC + bird + ball,
  levels/ZARIA.01.mkv t=440-457) and the mid-level bird sequences
  (zax37 t=306-315, 385-413): no placement line names them and the
  ZARIA.01 ANIMACJE tree has NO bird leaf - scripted chain reusing
  other levels' leaves, or engine code: find which
  status: OPEN
  origin: G-10/G-11

OI-082: the water surface as a floor (the lion walks WODA at pool
  level; zax37 t=305-307)
  status: OPEN
  origin: G-7

OI-083: HUD display truth: both R3 clips start at 5 lives (the
  port prints a hard-coded 6) and the HUD energy at level start is
  unknown - both are video-gated checks
  status: OPEN
  origin: G-8; PR-186 item (10)
  notes: the lives half resolves through OI-066 (STATUS.LEO) rather
    than a HUD read.

OI-084: is the rounded cave picture the WHOLE Mode X frame or a
  letterboxed copy? It decides what "the same picture" means for
  every movie comparison
  status: OPEN
  origin: HANDOFF 7 item 14 (found measuring for PR-111; frame
    /tmp/opencode/refcap/ep1_120.png, never in git; the PR-55/56
    decoder + PR-19/21 capture rig could answer it)

OI-085: the SEP life cycle (G-6: fight burst at t=299.5, kill not
  reproduced) + SEP2's missing script + the ZARROW/SEP semantics
  from the carried lists (G-4's script half)
  status: OPEN
  origin: PR-131 audit (G-4/G-6 exist only in that record); the
    jajo child-spawn part of G-4 is under OI-039

OI-086: spawn-state rule: does the original force standing, or
  start at the `.MOV`-declared first name (RAN.01 A_FLY_PTAKI1)?
  The port ships the declared-name spawn
  status: OPEN
  origin: PR-83 item 8.2; settling test = a RAN.01 level-start
    capture

OI-087: K_FWD/K_BKWD facing-relative vs literal: shipped as MODEL
  CHOICE (PR-85), still UNPROVEN on video (settling: facing-left
  lion + RIGHT key on a capture); AC_BREAK_PREV stays unmodeled
  (false)
  status: PARTIAL

OI-088: `I ix,iy` impulses multiplied by walkdir: nothing in the
  original confirms the mirroring
  status: OPEN
  origin: PR-83 item 8.4

OI-089: do F=0 states FREEZE momentum, or does the original coast?
  status: CLOSED BY PR-129/PR-130/PR-133/PR-135
  notes: superseded by the velocity+drag law itself (v decays to
    v_eq; F=0 => v_eq=0). The residual non-modelled halves are
    OI-015. Row kept: PR-83's text cites the question.

OI-090: K_F6..K_F12 stay OTHER (7 uses total); whether the original
  treats F-keys as HOLDED (the port's model) or TOGGLED weapon
  state; what ENDS A_FIGHT early; the comma-group AND as model
  choice
  status: PARTIAL
  origin: PR-87 item 8.6

OI-091: does the corpse (A_DEAD) still deal contact damage? The
  port says no (model choice)
  status: OPEN
  origin: PR-89 item 9.4
  notes: the contact-damage RULE for live objects is closed (G-5 by
    PR-139: authored `damage=` through the ATC window, 30-tick
    countdown); only the corpse case stays.

OI-092: the PLAYER-side row seek + signal legs: the object path
  ships the authored start row (PR-174); the player path models
  none of those signal legs (frames-blocked remainder)
  status: OPEN
  origin: PR-107/172/174

=== port-side + archived-review ids ===============================

OI-093: P-03 single window / single SDL lifecycle, P-04 focus
  stealing
  status: CLOSED BY PR-10/PR-12

OI-094: P-05 Wayland/X11: the automatic pipewire->alsa pin was
  removed; SDL's own choice is the default, LEO_PIN_ALSA=1 opts in
  status: CLOSED BY PR-35

OI-095: E-08/T-14 palette codec fidelity
  status: FROZEN (owner)
  notes: pixel output must not change.

OI-096: WINDOWS PASS: `make -k verify-windows` + one ci-windows
  dispatch + `make win` warning-free, ONCE at the end of
  implementation; do NOT fold them back into a per-PR gate (they
  measured 53.7% of suite time)
  status: DEFERRED (ruled)
  origin: owner ruling 2026-11-19 (HANDOFF section 2 rule 8)

OI-097: T-25 parser fuzzing
  status: CLOSED BY PR-11
  notes: the extension named at close (more kinds (.des text) + a
    longer seed list) is not a row until someone claims it.

OI-098: T-17 remainder: wire a config file to the PR-13 user-dir
  helper once a settings subsystem exists
  status: PARTIAL
  notes: the STATUS.LEO codec itself is done; its 84-byte field map
    is OI-066.

OI-099: T-24 remainder: the golden-master tool + port pins are
  DONE (PR-19/37/40); a same-frame comparison against the ORIGINAL
  engine under QEMU remains unbuilt
  status: PARTIAL

OI-100: the still-untouched items of the archived review list:
  T-03 config knobs cleanup, T-04 globals, T-05/T-06 fixed-step
  timing + precise sleep, T-07 clock ownership, T-16 Polish text,
  T-19 startup data validation, T-23 structured RE traces, T-26
  engine state machine
  status: OPEN
  notes: each origin-id resolves at
    docs/archive/open-leo_native_port_review.md; split this row
    when a first PR claims one item. (T-05/T-07 were noted as
    falling out for free in PR-18 - never verified.)

OI-101: the 'levels' config file loader (the 0x91ee5 error text
  names it)
  status: CLOSED BY PR-193
  notes: closed WITH A CORRECTION - there is no 'levels' file; the
    mechanism is the driver's named-entry lookup (0x5c320(0x3ea) +
    0x57990 + 0x4a4c0), keyed by the stage name.

OI-102: re_index's correction pointers stay prose-co-occurrence
  until records carry a CORRECTS: line the generator can read
  status: OPEN
  origin: PR-197 OPENS
  notes: records DO carry CORRECTS: lines since PR-198; the
    generator upgrade never happened, so the thread stands.

=== documentation program + tooling ===============================

OI-103: doc step 4: HANDOFF sections 5/6/7 become pointers (this
  register is their target; removes the 41 over-79 table rows)
  status: CLOSED BY PR-202
  notes: measured before doing it: the "41 over-79 table rows" sat
    10/6/25 across HANDOFF sections 1/3/6, not all in section 6 -
    this step removed only its own 30 lines (25 table rows reflowed
    into ESTIMATE blocks, 4 section-5 lines re-wrapped, 1 section-7
    line frozen into the archive); the ratchet dropped 71->41 and
    the 16 section-1/3 rows stay under it.

OI-104: doc step 5: NEXT_STEPS becomes journal-only: the stale
  status block -> a pointer, `## Next` moves to its one home
  (HANDOFF section 8) with doc_census updated in the SAME commit,
  `## Deferred big items` joins THIS register, standing rules join
  the rules
  status: CLOSED BY PR-203
  notes: done as measured: the P2 inversion (journal must carry
    NONE of the four section headings; HANDOFF one-homes pinned to
    exist) was red first on the old file; the live half of
    `## Next` became the section-8 lead, its shipped summaries were
    left to the records (one home each), STOP CONDITIONS joined
    HANDOFF section 10, the Standing bullets joined section 2 as
    rule 10 verbatim, and the Deferred list's live parts were
    already rows OI-093..OI-101/OI-095/OI-096 - deleted, not
    restated.

OI-105: retrofit: records PR-7..PR-195 carry no field block, so
  register sourcing for their threads leans on body prose
  status: OPEN
  notes: deliberately not retrofitted (PR-196's ruling); this row
    exists so the limitation stays visible.

OI-106: verify-utf8 needs the python-oracle frames that only
  `make py-refs` produces, and does not list it as a prerequisite
  (counted, NOT fixed here)
  status: OPEN
  origin: PR-193 GATES note

OI-107: the measured YETI ground run does not match the shipped F/T
  register: three camera-locked bursts measured 22.0-24.9 px/s while
  the register law with s=10 predicts 19.8-20.0 px/s for the same
  authored pair; two bursts land within 0.4% of |F|/4 = 25.0 px/s,
  the pre-PR-135 law
  status: OPEN
  notes: the factor 1.25 has exactly two candidate homes in the
    authored row, both un-read: the second value of each pair,
    `T=50,5` and `F=100,0` (PR-98 mapped the pairs to rec+0x50/54/
    58/5c and only the firsts were ever used), or a partial-intent
    s above 10 (PR-132 bounded |s| <= 10 for the lion's input path;
    a ground-walk gate for a creature is un-read). The 2.5-2.6 s
    bursts have a constant feet row (centroid y constant within the
    tracker's noise, blob height 55-60), so this is not the jump
    arc. The 22.0 burst is the one the record must explain; the
    24.9/24.9/25.0 agreement with |F|/4 is a second unexplained
    spread inside 13%.
  origin: PR-206 referee measurement

OI-108: object draw anchor: does the original draw `.MOV` placement
  art at the .SEQ record's anchor/margin offset rather than at the
  authored placement point the port draws at - measured offsets at
  verified cameras
  status: OPEN
  notes: measured at PR-205's verified ZARIA.01 cameras: the port
    draws a red-fruit pair at world (100,305)+(120,305) at the
    camera of t=22.0s where the video shows fruit at (68,275)+
    (75,275) and (102,283); at t=31.25s the port draws (349,304)+
    (361,304) where the video shows (319,323)/(327,322)/(329,305)
    plus fruit the port never places. Same-frame statics match
    (MAD 9.9-15.4), so this is the object layer only. The
    `level_landmarks.py` header already refuses to template objects
    because "their world anchor is NOT the authored point" - nobody
    has read which offset it is. The instrument is the THREE ZARIA
    I references, cross-checked at verified cameras; named as plan
    step 5a/5b in HANDOFF section 8.
  origin: PR-205/206 measurement
