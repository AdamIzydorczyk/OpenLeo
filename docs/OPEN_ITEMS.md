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

OI-078: the in-game ZARIA.01 climb: what raises K_GEN1 on a
  DRABINKA (the one named blocker left), the authored rectangle
  format ([obj+0]==2 as a runtime gate vs [obj+0]/[obj+4] as
  rect x/y, and what the row-start numbers change on screen),
  node lifetime + the 0x54b6a merge guard, object-asked answers
  and mid-air asking
  status: PARTIAL
  notes: poster, pair-walk entry, node builders, feed law, guard,
    evaluator, ladder answer all read and shipped (PR-142..150);
    the named remainder lives in those records' counted items.
    ZARIA.01 x=180 still does not climb.
    PR-216 CLOSED the gate half of this row. The answer branch
    at 0x3f1c0 is gated on [obj+0x5c], the STATE index, and the
    loader says what numbers it: 0x53905 reads the list a script
    declares before its blocks into the vector [obj+0x3c],
    0x53440 resolves a state name to that list's ONE-BASED
    position (0 = not declared, 0x53471), 0x534b6 stores the
    block's record at that slot of [obj+0x50] (allocated
    length+1 at 0x5397a), the record-ctor loop starts at slot 1
    (0x533de) and the object ctor writes [obj+0x5c] = 1
    (0x538ab). Block order is not that order: ZARIA.01's
    DRAB.MOV declares A_UP,A_DOWN,A_DROP but writes the blocks
    A_UP,A_DROP,A_DOWN - so a placed ladder sits at index 1 =
    the child-ANY branch (0x3f1e4/0x44ec0, sets [self+0x66]
    bit 6, signals nothing) and the ONLY state that answers the
    lion is index 2 = A_DOWN; A_DROP, the unrolling rope, is
    index 3 = inert. PR-150's row-cursor gate is gone, and the
    "binding order / encrypted pool" blocker of PR-157/158 is
    gone with it. Census: 172 of the 777 shipped .MOV texts
    differ between the two orders, 54 sites compare [obj+0x5c]
    with a literal (21 of them `== 1`, the initial state), and
    the pinned ZARIA.01 walk probe is unchanged, as expected:
    the placed ladder answers in neither the old nor the new
    model. What is left is ONE question: what raises K_GEN1 on
    a DRABINKA, whose only authored exit is `A_DROP(1)=K_GEN1`.
    The rest of this row's items are unchanged.
    PR-214 ANSWERED the "per-frame setter of the lion's
    contact target [obj+0xd2]" lead NEGATIVE: nothing writes
    it per frame. Full writer census (ELF sweep): the only
    appends are 0x3d618 (PR-146's asker-append inside the
    +0x78 entry 0x3d440, held at +0x78 in exactly two NON-
    lion tables 0x96a88/0x99e64) and 0x1ef71, inside the
    handler whose address appears exactly once in the image -
    0x97050 = LION base 0x96874 + 0x7dc - so the lion's
    list is written by the lion's own class at pair-touch
    time, gated on the counterpart's [obj+0]==2 and appending
    through 0x53f00 (kind 1). Heads are only zeroed
    (0x5415b/0x54e88) or popped (0x551fc/0x551f0). The
    reader 0x28590 (0x9a390 + 0x134) runs only when both
    sides' [obj+0x5c] == 4. TLewZaria 0x96960 overrides that
    slot with 0x531b0, which appends nothing - see OI-110.
    PR-217 touched no behaviour here. It captured the
    `Wait <signal>` clause ZARIA.01's DRAB.MOV authors
    (`[A_UP]` row 1 is `WAIT K_GEN1`, `[A_DOWN]` row 11
    `WAIT K_NEVER`), which is the other half of the same
    ladder, and named the ordinal the clause would use:
    K_GEN1 is signal bit 54 in the engine's own table, and the
    port's LL_SIG_* ordinals match all 68 numbered names name
    for name. How that clause is COMPILED and gated is
    OI-111; the "what raises K_GEN1" question stays here.

    PR-218 turned that one question into a MEASURED data fact
    with a named shape: `GEN K_GEN1` is authored in 20 of the
    777 texts and EVERY one of them is the lion's own script
    (ZARIA.01 LEW.MOV and LEWPL.MOV 36 rows each, TUNGA2.12
    36, TUNGA.11 32, ZARIA.02 32, KADMAR.07 29, KARA.07 26,
    SADES.06 10, LONA.05 9, RAN.01 8, KADMAR2 8, fewer below);
    ZERO DRABINKA member raises it, and ZARIA.01's DRAB.MOV has
    no `GEN` line at all, yet its `[A_UP]` row 1 is
    `1,0,WAIT K_GEN1`. Under a per-object signal set the bit
    can never be raised there, so the raiser is either engine
    code (the CATCH/DRAB contact of PR-216, which was read as
    setting [self+0x66] bit 6 and NOT a signal - worth
    re-reading with this in hand) or a set shared through the
    key registry the clause parser registers names into
    (0x57d30). Two more leads are closed: the lion's answer
    slot at its class table 0x96948+0xd4 is 0x544d0, an EMPTY
    override (the PR-153 boundary-artifact trap), so the
    `state index == 2` + AC_BREAK_NEXT case at 0x54482 is a
    no-op for the lion; and PR-218's read of the row step shows
    the `Wait K_GEN1` clause does not gate the row either - no
    shipped clause does (OI-111).
    PR-219 adds one FOCUS 1 datum about those very rows: in the
    engine a row whose SECOND authored number is 0 does not get
    a zero time - 0x516af-0x516c2 reads a key string at
    [scene+0x398] and substitutes a value looked up through
    0x44110 in the same name tables the clause parser uses
    (0x961fc/0x96200). ZARIA.01's ladder rows are exactly that
    shape (`[A_UP]` row `1,0,WAIT K_GEN1`, `[A_DOWN]` row
    `11,0,WAIT K_NEVER`), so their row time is a level-config
    value the port never reads, and which authored number is the
    row's time at all is now OI-112.
    PR-220 narrows that `,0` case further: the engine does not
    invent a constant there. The state record's row builder
    0x51eb0 loads ebx from the global 0xceff4 (0x51f48) and
    passes it to the row ctor 0x51f54; 0x516bd's registry lookup
    (through 0x44110) fills the same field when the authored
    value is 0. 0xceff4 is written at 0x51a35 by the `seq=`
    branch 0x51b0d: it registers the sequence name (0x57d30),
    then calls the settings block's +0x4 verb (0x51a32) with the
    name-table global 0xceff8, i.e. it looks the sequence's own
    `time=` default up. So ZARIA.01's `1,0` / `11,0` ladder rows
    carry the sequence's authored default time, a value the port
    never reads, and the row's own time lives at [row+0x8]/
    [row+0xc] with a sign flag - whose consumer PR-220 could not
    find (OI-112).
    PR-221 retracts one thing PR-220 briefly suggested here:
    the `,0` in ZARIA.01's `1,0,WAIT K_GEN1` / `11,0,WAIT
    K_NEVER` is NOT read by the engine as a row time and then
    replaced by the sequence default. The row ctor parses one
    number (0x51660, one destination at [row+0x10]); [row+0x8]
    is the sequence default or a settings-block name lookup at
    EVERY row regardless of the line, and the second authored
    number has no named engine consumer (OI-112).

    PR-224 names the engine's one-bit raiser: the handler
    0x53772 tests a message of type 0x10 with the tag
    0x8138764, takes the payload byte at [msg+0xc] as a SIGNAL
    INDEX and OR-s `1 << (idx % 32)` into
    [obj+0x60 + 4*(idx >> 5)] (0x53790-0x537d2) - the TEventSet
    of PR-88, raised one bit at a time. The engine's own key
    sends match this port's ordinals exactly: 0x2c5cd sends 0x21,
    0x2c611 sends 0x1c, 0x2c651 sends 0x1b = LL_SIG_K_FWD /
    K_UP / K_DOWN. NO sender passes 54 = K_GEN1: the 22 call
    sites of the sender 0x4bc40 use small AC_* indexes (7, 0x12,
    0x13, 0x19, 0x1a, 0x21) or values read from object fields
    ([obj+0xee] at 0x34a7f, [obj+0xf2] at 0x3a1cf, [obj+0x122]
    at 0x1f45c), and the broadcast loop 0x3eac6-0x3eaf4 sends
    0x39 to a list of objects. So the drabinka's K_GEN1 comes
    from one of those field-fed senders, from a different message
    type, or from the non-matching branch of the same handler
    (0x52bb0 at 0x537dd) - the named next read.

    PR-225 closes PR-224's "field-fed sender" candidate and
    closes the sender set itself: the magic signal tag 0x8138764
    occurs at EXACTLY six producer sites in the image (0x2882d,
    0x2c5e1, 0x2c613, 0x2c653, 0x2c99a, 0x3eacf) plus the one
    check at 0x53780. A signal raise is a message with type 0x10
    AND that tag; the payload is the signal index. The raisers:
    0x2c5cd/0x2c611/0x2c651 broadcast K_FWD/K_UP/K_DOWN (33/28/
    27) through 0x4bc40 (its parameter order is scene, TYPE in
    edx, TAG in ebx, selector in ecx, signal index pushed);
    0x2c99f raises index 21 on [scene+0x394]; 0x3a1dc raises 30
    on self; 0x2882d-0x2885a synthesizes the message on the
    stack and calls vtable+0x10 directly (index 10); the
    collision loop 0x3eac6-0x3eaf4 raises index 57 on every
    object of the collided object's neighbour list. NO sender
    passes 53/54/57-as-source/58 = K_JUMP/K_GEN1/K_NEVER, and
    the field-fed sites put a SUBSYSTEM constant in the TYPE
    slot (ds:0xcebfc, 0x967f4) with tag 0, so they never raise.
    Two non-message writers remain, and one is named: a
    collision ORs 4 and the set at [[obj+0x3c]+0x60] into
    [obj+0x60] (0x3edff-0x3ee11) - THE CURRENT STATE OBJECT
    CARRIES A TEventSet at +0x60 - and vtable+0x8c (base
    0x536f0, merged at 0x536ff-0x53725) adds a per-frame set.
    Next read: what the state's +0x60 set holds and how it is
    authored, or whether the shipped `.MOV` name lookup (0x57d30
    vs the block 0x961fc/0x96200) maps the WORD `K_GEN1` to an
    index the engine really raises.

    PR-226 names the raiser mechanism: the ordinary scene-object
    classes implement vtable slot +0x8c as the signal provider
    0x54910 (102 class tables hold it; the frame refresh 0x536f0
    calls it every frame and merges its result into [obj+0x60]).
    Its law, from machine code: [obj+0xa5] bit 0x1 raises 33 =
    K_FWD, bit 0x10 raises 41, BIT 0x20 RAISES 54 = K_GEN1; if
    [obj+0xba]!=0 then [obj+0xb6] bit 0x10 (kept as the SIGN of
    the speed word [obj+0x6d], 0x54bb0-0x54bd4) chooses 36 =
    K_FWD vs 35 = K_BKWD; [obj+0xbe]<0 raises 29, >0 raises 30.
    So the drabinka's WAIT K_GEN1 needs no message and no GEN
    row - the provider re-evaluates it continuously, which is
    why no sender passes 54. The ONE question left: who SETS
    bit 0x20 of [obj+0xa5]? The ctor zeroes it (0x2b6a0), the
    clone copies it (0x2b840), other code reads [obj+0xa5] as an
    index into [obj+0xa9] (0x2b727); the only dword writers are
    0x2b840 (copy) and 0x54be9 (stores [[ecx+0x8]], another
    class). Also named: state flag word +0x12 bit 0x800 = state
    MIRROR, entered by `neg [obj+0x6d]` + vtable+0x54 (0x53670),
    and the row word `rev` calls that same flip (0x459d0 ->
    0x53670) - two FOCUS 1 rules to port next.

    PR-227 completes both sides of the provider. The ONLY code
    that writes the provider's fields is the block 0x549f2-
    0x54bf2 (its entry is not referenced by any static xref or
    vtable - finding that dispatch is what is left): it stores
    [obj+0xa5] = [[arg+0x8]] - the CLASS ID of the related
    object handed in the argument's third slot - so bit 0x20
    (K_GEN1) means "attached to a class whose vtable word has
    bit 0x20"; it also ORs the related object's record set at
    [[[obj+0x10]-chain]+0x60] into [obj+0x60] (0x54b78-0x54b84)
    and keeps [obj+0xb6] bit 0x10 as sign([obj+0xba]*[obj+
    0x6d]). NEW GATE: 0x47e40 match is called from exactly two
    sites - PR-217's row step (0x522e6) and 0x52000, a first-
    pitch row scan inside the refresh (0x536f0 -> 0x53520 ->
    0x52000): walk the STATE's row table [S+0x1a] (count +0x22,
    stride 0x15, clause set at entry+0), first entry whose set
    intersects a copy of [obj+0x60] wins; its +0xd/+0x11 dwords
    are returned and fed to 0x535a0. The port models one gate;
    the engine has two, in series.

    PR-228 finds the entry PR-227 missed: the block starts at
    0x549f0 and is vtable slot +0xfc of 15 class tables (the
    dwords 0x96a2c/0x9a618/0xa22e4 hold 0x50340, which zeroes
    the triple when [obj+0x106]==0, negates it when
    [obj+0x11a]==5, then calls the block). Its callers are two
    switches: 0x41ae2 (on byte [obj+0x160]) whose arms call
    vtable+0xf4/+0xf8/+0xfc and OR the RETURNED TEventSet into
    [obj+0x60] (0x49580) - more signal providers like +0x8c -
    and 0x50d70, a message-type switch over types 9..0x15 (jump
    table 0x50d20, misaligned; best read: type 0xf routes to the
    +0xfc arm with (msg+8, msg+0xc) as the pair). Inside the
    block, [obj+0xa5]=[[second]] - the CLASS ID of the object
    the message carries - so K_GEN1 machine-semantics: attached
    to a class whose vtable low byte has bit 0x20 (true for the
    0x9a778-style tables); the provider re-tests it every frame.
    Also: 0x54c00 is a sound-effect method of the same class
    family - signed byte [obj+0xb5] indexes a name via
    vtable+0x50 and resolves it through [[0xcef40]+0x390] and
    0x57530 (PR-222's settings lookup); [obj+0xee]<3 gates it
    (0x1f100) - PR-225's "field-fed sender" objects are this
    player class.

    PR-229 names the last link. AC_RECT (type 0xf) senders:
    0x28654/0x28b9b/0x3740c/0x374c5/0x3dc7b - each right after
    the 4-comparison rectangle test, tag = THE OTHER OBJECT'S
    POINTER, payload 2 or 3. The +0xfc arm (0x50dc7) takes that
    pointer as the argument and the entry 0x549f0 stores
    [other+0]->[obj+0xba], [other+4]->[obj+0xbe], and
    [other+8] (0x54be6-0x54be9) -> [obj+0xa5] - the byte the
    provider tests. [other+8] is the KIND METHOD TABLE pointer
    (0x9a754 = a table of the TEventSet/interval verbs 0x45120/
    0x45250/... - the very "class id" PR-221/PR-225 compared to
    the class globals). Bit law from measured low bytes:
    0x97970/0x978dc/0x96874 have none of bits 0x1/0x10/0x20;
    0x96fd8 has 0x18 (raises 41 only); the 0x9a74c/0x9a754/
    0x9a778 family (0x54 = bits 0x10+0x20) raises 41 AND
    K_GEN1. So: "my rectangle overlaps a 0x9a754-family kind ->
    the provider raises K_GEN1 every frame" - no GEN row, no
    message. Final question: which authored kind the ZARIA.01
    DRABINKA carries; then port the overlap rule.
    PR-233 ANSWERS it, NEGATIVE: the class token is authored
    level text (factory 0x4df10 chain); 'DRABINKA' matches at
    0x1f8ac and its kind word is 0xa2498 (0x531f0 tail
    0x532a6; low byte 0x98 = bit 0x10 only), KDRABINA's is
    0x9d8a8 (0x34e39) - neither carries bit 0x20, and the
    drabinka kind table's provider slot +0x8c is the NULL
    provider 0x23a60 (empty TEventSet, nothing raised). The
    0x9a778-at-+8 stores (0x29a51/0x2a18d) are mid-chain ctor
    stores overridden one instruction later (0x29a69 ->
    0x99828) - PR-229 read them as final kinds. So the ladder
    `WAIT K_GEN1` can only be fed via [drab+0x60]; PR-226's
    bit-0x20 writer of [obj+0xa5] is still the unnamed
    question (word writers measured: 0x2b6a0/0x2b840/0x54be9/
    0x54e24 + the LION keyed-list region 0x1e5xx).
    PR-234: the name block 0x91b40-0x91c68 confirms the port's
    68 ordinals (K_GEN1 = bit 54 stands); K_NEVER is a parser
    special, NOT bit 57 (no contradiction with the collision
    raise of 57); no raiser of 54 exists anywhere. FOUND: the
    AC_DRAB raise on the LION - 0x287a0 (called only from the
    collision loop 0x3eac6), gated on the state record's
    authored +0x4 ID == 6 or 0x12, synthetic type-0x10 tag-
    0x8138764 payload-10 message to the lion's +0x10. The
    climb's missing named pieces now: that raise (PORT), the
    GEN-tail admission effect (READ), the PR-227 attach-merge
    reachability for [drab+0x10] (READ).
    PR-235 SHIPPED that raise: ll_mov_st.block_pos is the
    record-ID word (ds:0xceff8 counter, 0x5150b/0x51a2a,
    record +0x4, object +0x8d via 0x5386c, read by the +0x6c
    accessor 0x23a10), and play_collide raises AC_DRAB on the
    lion when its state ID is 6/0x12 (= A_DEAD/A_DRAP by the
    authored block order) and the other's ID <= 0x19. Open
    here: the [[other]+0x3c] probe + 0x28820's second clause
    word, K_GEN1 delivery to the drabinka, AC_DRAB_CATCH's
    raiser.
    PR-236 READ the feed machinery: the refresh calls
    [[kind]+0x8c] (0x536ff) - the rope classes carry the
    NULL provider 0x23a60; where the slot holds 0x549f0 it
    evaluates the [obj+0xd2] ATTACHMENT-entry queue: kind
    1/4 proximity factors to the entry point, kind 2 = OR
    the entry's own 12-byte key (COPIED at enqueue,
    0x53f50) into [obj+0x60] when entry+8 is 0 or the
    object's state ID, then pop. The enqueuer 0x3d440
    (PR-214's +0x78 entry) needs the player at state 1, a
    parent at [obj+0x30] (a type-0xe effect, 0x52bef) and
    the receiver's K_FIRE bit (provider 0x54910 synthesizes
    it from [obj+0xa5] bit 1; it also synthesizes
    K_BKWD/K_FWD from the factors). So rope-inherits-
    lion's-K_GEN1 is an ATTACH path, keyed at enqueue and
    state-gated. Unread, and the named blockers: who runs
    0x28590 (table 0x9a390), who sends the type-0xe
    message, who sets [obj+0xa5] bit 1, and whether any
    provider synthesizes K_GEN1 itself.
    PR-237 PINNED the ask itself: kind-table slot +0x78 IS
    the ASK/answer verb - 0x553e0/0x4b500 walk objects,
    call it, and OR the returned TEventSets. A class-table
    anchor (slot +0x6c = the record accessor 0x23a10) names
    every table and slot: PR-150/216's ladder answer 0x3f1c0
    is that verb for exactly ONE class, 0x97cdc, while the
    DRABINKA table's own +0x78 is 0x53c90 (shared by 14
    tables) - reading 0x53c90 is the next named step here.
    0x3d440 (the PR-214 pair 0x96a88/0x99e64, CONFIRMED)
    answers only when the asked is at state 1 and the
    requester has a parent and K_FIRE; AC_STONE is its
    true-branch answer; it is what queues PR-236's attach
    entries. 0x28590 = class 0x9a44c's +0x78 override
    (PR-214's 0x9a390+0x134 arithmetic corrected). Open:
    what kinds 0x97cdc/0x9a44c are, who reaches 0x1eba0/
    0x380d0's ask, 0x552e0's 0x53ae0/0x55230 gates.
    PR-238 NAMES the tables through the engine's own
    name->object factory (0x1f870, strcmp chain over the
    class-name block 0x906fa-0x9078b): the PLACED
    DRABINKA's kind word is 0x97cdc, not the ctor default
    0xa2498 - so 0x3f1c0 IS the placed rope's answer verb
    (the port's ladder_answer_for stands) and its +0x8c
    provider slot IS the null one: the PR-236 attach-merge
    is RULED OUT as the rope's K_GEN1 feed. 0x96a88 =
    KAMIEN, 0x9d8a8 = KDRABINA (same factory); 0x1eba0 is
    the 0x96874 family's ask-all method (vtable slot
    0x97954): if the answer union contains K_GEN1 it sets
    [self+0x28] - PR-225's frame-step gate. 0x53ae0 is
    the block-response gate and answers {AC_BLOK}/
    {AC_BLOK_Y}. Feed candidates now named: (i) a signal
    message with payload 54 (none found, PR-234); (ii)
    0x536f0's own ORs into the rope's row (next read);
    (iii) a class answering asks WITH K_GEN1. Still open:
    the authored name of class 0x9a44c.
    PR-239 READ the whole per-step composition and RULED
    OUT four feed candidates by measurement: the placed
    rope's owner kind-table +0xa8/+0xac verbs are bare
    `ret`s; its ASK channel (0x52790 -> 0x4bd30/0x4b890/
    0x4b870) is gated on the record's `control=` bits and
    ZARIA's rope declares only BY_TIME, so it is off; the
    row's own `WAIT K_GEN1` clause adds NOTHING to the key
    (0x521f0 uses it only to pick the row path, and a Wait
    node's execute answers empty - the "row key feeds the
    transition" model was refuted before it could ship);
    and no code writes bit 54 or sends a signal message
    with payload 54 (the five builders send 0xa/0x21/0x1c/
    0x1b/0x39), though 0x53770 shows a signal message ORs
    its payload bit straight into [obj+0x60]. Named next:
    0x2b4d0 (record class +0x30) and 0x54560 (the placed
    rope's +0xc8) - both handed the context struct, so the
    question is whether either writes [obj+0x60] itself -
    then the control-name to bit map, 0x4b890/0x4b870, and
    how 0x520c0 spends the row budget.
    PR-240 ANSWERS the headline (what raises K_GEN1 on a
    DRABINKA): the ladder's OWN ask answer does. The answer
    verb 0x3f1c0 at the asked's first declared state (A_UP,
    [asked+0x5c] == 1) gates on the requester having a
    PARENT (0x3f1e9) and 0x44ec0 - an ANY-test over the
    12-byte-element vector at [[asked+0xa5]], count
    [asked+0xa9], each element's class verb +0x8 against
    the requester's [[kind]+0x4c] object (0x23a20) - and on
    true 0x3f203-0x3f20c sets byte 6 mask 0x40 of the
    asked's OWN live set: bit 54 = K_GEN1 (the store the
    port has long counted as "sets [self+0x66] bit 6"), and
    the answer itself is empty. The next step's composition
    (0x53725) puts it in [obj+0x60], where the authored
    `A_DROP(1)=K_GEN1` line matches: the chain A_UP ->
    A_DROP -> A_DOWN is reachable and needs no external
    sender. Ruled out here: the record-class +0x30 verb
    0x2b4d0 is a bare `ret`; the rope's kind-table +0xc8
    0x54560 is a message handler that never writes
    [obj+0x60]. NEXT: the PORT of the self-raise, and the
    paired read of the ask that triggers it (whose ask
    channel is on, and the rope's [obj+0xa5] contact
    vector); also open, who sends type 0xe (the parent
    store, an ARM of the base handler 0x52bb0 at 0x52bef,
    not a table slot) and type 0x21 (which sets the
    frame-step flag [obj+0x28] - a second feed of it).
    PR-241 PORTS that self-raise, test first: the ladder
    answer selftest gains leg L7, which builds the authored
    `[A_UP]` pair (`A_DROP(1)=K_GEN1` + row `1,0,WAIT
    K_GEN1`), asks a placed ladder at its first declared
    state, and pins that it answers NOTHING, raises K_GEN1 on
    ITSELF, and reaches the A_DROP block in the following
    record steps - unasked, it stays put. The leg fails on
    the stub and passes with the raise; no fixture moves and
    the pinned ZARIA.01 walk is unchanged because it never
    reaches the rope's pixel (180,510 vs ground y=680 ending
    at x=408). NOT MODELLED, counted: the [obj+0xa5] vector
    (PR-150's pair contact stands for the 0x44ec0 ANY-test),
    the requester's parent gate, and the engine's one-record-
    step delay. NEXT: who writes [obj+0xa5] and what its
    elements' +0x8 verb tests; whose ask channel reaches a
    placed DRABINKA at all; and the other half of the climb,
    the lion getting to a ladder (OI-072/081/082/083).
    PR-242 finds the gate is AUTHORED DATA the port never
    reads. The placement record continues past its five
    fields with three more lines: ZARIA.01's ladder carries
    `1 33,-3` (ONE contact point, relative to the place),
    `23,30;53,160` (a rectangle) and `30,50`. Census over
    hd_files/ETAPY: 2076 entry lines, 21 with those lines
    (11 points+rect+pair, 10 rect only), SIX classes -
    DRABINKA 4, LINA 7, KLINA 3, XCHG 3, PILAGEN 2, GUARD 2.
    The kind-table +0x18 verb reads them (the rope's =
    0x3f0f0): 0x44f10 parses the count into [obj+0xa9] and
    allocates 12-byte elements at [obj+0xa5] - a header that
    is itself an object, its method table being the 0x97f50
    block PR-239 read as a value, which also holds 0x44ec0 -
    and 0x44e90 adds the placement to every point (relative
    -> absolute, once, at build time). The element class
    0x98274's verb is 0x23e00 = POINT-IN-RECTANGLE, so the
    0x44ec0 ANY-test asks: is any of my authored points
    inside the asker's box - that box being the plain field
    [asker+0x75..0x84] (+0x4c = 0x23a20 in all 186 tables).
    The other gate is a CLASS gate, not a parent: +0x3c is
    `xor eax,eax` in 171 tables and `mov eax,1` in 15, and
    the lion is one of the 15 (TLewZaria -> kind table
    0x96960). So the authored chain is: the placed rope at
    A_UP, its one point at (213,507), asked by the lion,
    raises K_GEN1 on itself when the lion's box covers that
    point - 3 px above the placement, i.e. only a lion up at
    the ladder top, never one walking the ground at y about
    680. NEXT: who writes [obj+0x75]; parsing the point list
    in the port, which now counts those three lines as
    poly/shortline noise (ll_level.c 8135-8141); and the
    authored rectangle it ignores - this ladder's is x
    203..233, y 540..670, down to the ground line, while the
    port collides with the art bbox 42x48 at (180,510).

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

OI-092: the transition-apply SIGNAL legs stay unread: the
  AC_TIME count arm 0x53626 (the player models it through its
  own frame-list wrap) and the +0xac verb 0x53619-0x53620.
  The row-seek half is CLOSED (objects PR-174, the lion
  PR-215). New named question from PR-215: the engine's
  same-state arm stores the raw -1 of an absent token with no
  negative test (0x53616; the refusal at 0x52381 is the
  cross-state setter), so what a -1 cursor does on the next
  step (wrap 0x521c5) is unread; the player and object paths
  differ there today
  status: OPEN
  origin: PR-107/172/174/215

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
  status: OPEN - PR-211 read BOTH named leads and both are
    NEGATIVE; the row now stands or falls on one named lead,
    the physics-step call rate
  notes: the two leads this row carried are ANSWERED NO by
    PR-211's machine-code read. (1) The pairs are (X,Y): the
    parser 0x52590 -> merge 0x52680 -> integrator 0x54250
    chain takes the x legs from the FIRST of each F/T/G pair
    and the y legs from the second, so the seconds of
    T=50,5 / F=100,0 are the Y axis and cannot scale the
    ground x walk. (2) s ([obj+0xba]/[obj+0xbe], read at
    0x542ab/0x542cd) has exactly ONE writer in the image, the
    input-apply 0x549f0, and every IN path into it is a
    multiple of 5 with |s| <= 10 on x: the chase driver
    0x27d60 passes round(10*dx/(|dx|+|dy|)) (the 10.0 double
    at ds:0x909ef; =10 only when dy=0), the terrain driver
    0x34495 passes 10*[obj+0x6d], the dead-band branches
    store only 10/-10/0. Also dead: the integrator's T-raise
    gate needs T_x < 20 (authored 50), and A_RUN authors no
    FC row so the 0x54170 target leg is 0 for the walk. A
    ground walk at 22.0-24.9 needs s = 11.0..12.5 - no such
    s exists on any path; the register law itself matches the
    shipped port exactly (steady x = 5*F_x*s/T_x per step).
    THE ONE LEAD LEFT is steps per second: the engine's step
    wrapper 0x52750 integrates through the single +0xc8 slot
    call at 0x52774 and has exactly 10 call sites (the
    per-class frame drivers 0x23fc8, 0x264a8, 0x2701d,
    0x2c3ad, 0x2df08, 0x31fd8, 0x33c98, 0x3620d, 0x4fff1,
    0x61238); how often those run per display frame, against
    the display cadence PR-134 bounded, is the unread that
    1.1-1.25 steps-per-frame would explain. The 2.5-2.6 s
    bursts have a constant feet row (centroid y constant
    within the tracker's noise, blob height 55-60), so this
    is not the jump arc. The 22.0 burst is the one the record
    must explain; the 24.9/24.9/25.0 agreement with |F|/4 is
    a second unexplained spread inside 13%, still unstated by
    any law read so far.
  origin: PR-206 referee measurement

OI-108: ZARIA's `animacje=` objects drew through the WRONG PLANE in
  the port; PR-208 READ THE LAW (the level's own `plany=` key) and
  fixed it; PR-210 re-checked the corrected scene against the film
  and the contradiction FELL - the film AGREES with the shipped law
  status: CLOSED BY PR-210 (the plane law ANSWERED and shipped,
  PR-208:
  the [obj+0x24] plane of an `animacje=` placement is the layer whose
  planies spec carries that cell's A letter; ZARIA's balloons,
  ladder, ropes and coconut chain now draw at 1:1, main_back 2, as
  their own .des says. The re-check, PR-210, is DONE and POSITIVE:
  PR-207's scan pipeline resized the full 1280x720 frame to
  320x200 - 0.9y SQUASH, an artifact - and its cameras
  (28..37,150)/(240..260,207) were that artifact; with the correct
  /4+pad pipeline the locks are t=22.0 -> (30,162) sharp and
  law-free (no-obj and obj-on curves agree within 0.1 MAD across
  the whole y sweep), and PR-207's VALID disc box at t=22.0
  (279..287.8,131.5..141.8) sits ON the shipped law's design point
  POCISK (310,298) -> (280..292,136..148): left edge 1 px, top
  edge 4.5 px, the bullet chain moves. At t=31.25 the lock is
  (235..242,214..216) (shallow) and both discs sit within chain
  travel of the design points. The claimed "no single plane +
  in-range camera explains both frames" is RETRACTED.)
  notes: THE LAW, PR-208, from the shipped .des files themselves
  (12 of 13 levels; CHEATS has no .des): `plany=<spec>|<spec>|...`
  is ONE spec per tlo= layer (the spec count always equals the tlo
  count), letters ONLY {T,A,L,B}: T once per spec = that layer's
  own art, L once = the lew= lion and it sits on the main_back=
  spec on all 12 (that spec is the 100,100 speed pair), B once =
  the belka= belt and it too sits on a 100,100 spec, and A appears
  EXACTLY as often as the `animacje=` list has cells (3,3,3,2,3,3,
  5,4,4,4,3,0 vs list lengths 3,3,3,2,3,3,5,4,4,4,3,1-whose-only-
  cell-is-a-bare-none): the k-th A in reading order is the k-th
  cell and rides the layer of its spec. A bare `none` in the
  MIDDLE of a list is a cell and consumes a letter (RAN.01); a list
  that is ONLY `none` names no cell (JUNCTION). Port side:
  plan_t/plany_parse/des_list_cells + the `ll_level: planies: ...`
  report + one mapping in the placement loop; pinned by a two-run
  verify-level leg (lv_plany/lv_plany2) and by plany_selftest in
  the always-on --mov-selftest bundle. Engine side, raw machine
  code: 0x57500/0x57530 take the layer in EDX (0x5750a indexes
  [scene+0x20] by it, 0x57516 takes the divisor row from
  [scene+0x1c] = main_back); 0x52d67/0x52d72 load it from
  [obj+0x24]; 0x23b90 (`mov [eax+0x24],edx; ret`) is verb +0x04 of
  211 class tables. The un-lettered keys are OI-109.
      SUPERSEDED PORT FACT (PR-207 probe, pre-fix): every ANI2
    object - POCISK x16 fruit, BAL x6 balloons, the DRABINKA
    ladder, LINA ropes - reached obj_draw with `layer=1` and the
    85/66 draw-cam (printed: (26,169) at camera (31,150), (207,206)
    at (244,207)); that was PR-61's entry-index INFERENCE, printed
    as a camera. PR-208's fixture prints the same objects at
    layer=2 draw-cam=(37,250) and ANI3's ANIBACK at layer=1
    draw-cam=(1020,254).
      FILM FACTS, re-measured PR-207 at NATIVE resolution (the
    level clip is 1280x720 = exactly 4x the 320x200 view; every
    number below is render-space = video px / 4): SUPERSEDED BY
    PR-210 FOR THE CAMERAS (their scans resized 1280x720
    straight to 320x200: /4 horizontal but /3.6 vertical, a 0.9y
    squash that drags the picture's black bottom rows in - the
    y-locks 150/207/501..540 are that artifact). The disc boxes
    and the traps below STAND - they were measured in native /4
    space. background-locked
    cameras (full MAD scan of `ll_level LEO_LEVEL_NO_OBJ=1` renders):
    t=22.0 -> x in 28..37 flat (30.4; x=0 37.4, x=44 37.4), y=150
    sharp (30.4; 147:35.2, 153:35.0); t=31.25 -> x in 240..260 flat
    (30.2; 288:39.6), y=207 sharp (30.2; 204:31.4, 213:31.4);
    t=6.0 (level start) -> (4..24, 501..540) plateau, and at that
    camy both y-laws coincide - no discrimination. THE TRAP: the
    ZARIA recording carries a SCREEN-FIXED red graphics column
    (x 289..320 y 53..128) plus red in the bottom-left corner -
    identical in every frame (diff-tested), and the column sits
    EXACTLY where fruit would fall in view at those cameras. Red
    blobs surviving the overlay diff: t=22.0 exactly ONE KOKOS-sized
    disc (279..287.8,131.5..141.8); t=31.25 two (52.5..65.2,
    100.8..113.8) and (77..90.2,99..106.8). Predicted disc boxes
    from the authored points: at 1:1 with the locked cameras
    (279..288,148..159)+(291..302,148..159) at t=22, (66..75,
    91..102)+(78..87,91..102) at t=31.25; the port's 85/66 draws
    measured in its own renders (284..307,129..140)+(69..80,151..
    162)+(89..100,151..162) at t=22's cam, (103..126,92..103) at
    t=31.25's. The film's discs match NEITHER set consistently:
    t=31.25's second disc sits within ~1 px of the 1:1 x of
    POCISK (322,298) (y +7; the two y-laws coincide at camy 207 so
    y cannot discriminate there), while t=22's disc is 16 px ABOVE
    any 1:1 prediction and fits the 85/66 x only if the camera sits
    at the far end of its flat range (x~37). The contradiction has
    an observed source: POCISK/kokos.mov is the bullet chain (`
    pocisk` = bullet; jajo children spawn from it) - the film's
    fruit MOVES, and at t=22 the pair at design (95,320)/(115,320)
    is absent from the film at every plane. The set the port draws
    is definitively NOT what the film shows at either locked
    camera; the exact law is not pin-able from moving fruit.
      RETRACTION: the numbers in PR-206's OI-108 row (the "(310..
    319,146..157) match", the x=1/x=231 locks) were computed from
    intermediate extracted frames in /tmp (576x360) that no longer
    exist; re-extraction from the git reference at t=22.0 shows the
    x-lock was NOT sharp at x=1 and the disc coordinates differ.
    Use native 1280x720 = /4 extraction and the overlay diff.
      PR-210 CLOSED IT (the corrected pipeline, the disc match,
    the ANI3 waterfall window): the pipeline is scale /4 THEN PAD
    the 20 black rows (tools/re/video_camera_read.py --native
    encodes exactly this); cameras come from full-frame MAD with
    the two screen-fixed overlay boxes masked - t=22.0 y-curve at
    x=30: 22.83@162 MIN (28.46@150, 25.22@156/168), and the
    no-obj and obj-on curves agree within 0.1 across the whole
    sweep - a placed object is worth ~0.05 MAD on the full frame,
    so a full-frame lock cannot be a law artifact; t=31.25: x
    24.69@235/24.72@240, y 23.76@216 (shallow). The t=22.0 disc
    sits ON the POCISK (310,298) design box (280..292,136..148)
    - 1 px x, 4.5 px y - and the "absent at every plane"
    (95,320)/(115,320) pair is exactly what a bullet chain that
    already fired looks like. The waterfall window (t=70.5/72.5/
    75) puts ANI3's WAZ column on screen: the port draws its e0
    at (168..199,28..61) at cam (1725,520), the film's blue
    runs are 257..284 / 257..284 / 185..233 (width ~ the 32 px
    art); the shipped L1 law predicts the 1:1 lisc4 crown
    235.75-0.15C px RIGHT of the water edge = adjacent in the
    film, and the water's 72 px run = 0.85 x the implied camera
    run; while PR-61's entry-index (ANI3 -> layer 0) puts that
    water at 1634-0.39C, on screen only for camx 3315..4190 -
    impossible here - so under the old rule the waterfall could
    NEVER have been in this window. SUPPORTING, not the close:
    that window's camera never locked sharply by a second method.
    LEADS: the 1:1 lisc y-anchor there implies film camy ~520,
    above the port's YMAX=500 clamp, while ZARIA.01 authors
    lisc2.seq at y=1546, far outside its own 700-row world -
    unmeasured, start there if level-bottom geometry is ever
    revisited; ZARIA.02/03 clips are unmined (ZARIA.02 has only
    5 usable static landmarks).
      PR-207's LEADS FOR THE LAW, NOW RESOLVED AND CORRECTED:
    FUN_0005b040 `case 0x10` writing [rec+0x24] 0/1 on token 0x5e
    and FUN_0005da60 testing [rec+0x24]==0 are NOT this field -
    their +0x24 is a 0/1 flag paired with a bit-mask test at
    0x5b040+0x276, in a text-run record. The real field's getter/
    user pair is 0x52d67 -> 0x57530 and its setter is 0x23b90
    (above). PR-209 read the planies CONSUMER (0x4af10: a
    per-invocation letter PROGRAM driving tlo/lion/belt/pool
    cells; it never writes [obj+0x24]) - the plane law above is
    unaffected, and the engine-side writer of the plane is still
    open (OI-109). The film plan (the three ZARIA I references + the
    stateless ANI1 PICTs, template-matched by their own art) is
    what a re-check must use; the fruit chain moves.
  origin: PR-205/206 opened; PR-207 re-measured (the film facts
  below ARE that measurement, cameras excepted - see the PR-210
  block); PR-208 answered the law and fixed the port; PR-210
  re-checked the corrected scene against the film and CLOSED it

OI-109: the plane of the two placement keys that `plany=` NEVER
  NAMES - `zwierzaki=` and `ani_plat=` - is still not read from the
  engine; the port keeps PR-61's entry-index model there
  status: CLOSED BY PR-213 - the law was read by PR-212 (below)
    and the port now applies it; the still-unread leads moved
    to OI-110
  notes: THE LAW (PR-212, exact bytes): one level-load pass,
    0x4b1d0 (its ONLY call site 0x4d764, in the same flow
    that builds the lion at 0x4d668 and sets the lion's
    plane at 0x4d68b). A key list's entry i chains at cell i
    RAW ('none' occupies a head and shifts nothing;
    0x4ce52/0x4dd50/0x4aaa0 - neither builder writes +0x24).
    For cell c of each pool id 0..2 the pass counts the '|'
    bytes before the (c+1)-th 'A' in the level's planies
    program ([[level+0x30]] - the same string the letters
    run, PR-209's consumer 0x4af10) and dispatches that
    count as the plane to EVERY entry in that cell through
    class-table slot +0x5c (0x23b90 `mov [eax+0x24],edx` in
    150 of the 177 tables long enough; propagation variants
    0x1ecb0/0x332f0/0x554f0/0x531b0 - the image's ten
    `call [reg+0x5c]` sites are all accounted for in the
    PR). When the A's run out the plane is the LAST segment
    (total '|'). The lion rides separately: [lion+0x24] =
    main_back (0x4d68b); children created at runtime inherit
    their owner's plane at once. The letters never named a
    key's plane - the program's SEGMENTS name it for all
    three keys at once - which is why no shipped spec
    carries a letter for those keys. PR-209's own note
    mis-named 0x23e00 as a +0x24 getter; it is PR-106's
    bounds test (corrected in the record).
      DATA: ZARIA.01 `T|TA|TALAB` -> cells 0,1,2 -> planes
    1,2,2, so KAMIEN (cell 1), ZWIERZ1 and ANIPL2 all ride
    plane 2 = main_back = the lion's plane - their collision
    agreement with the lion is a CONSEQUENCE of the law, not
    evidence. LONA.05 `T|TAAALAB` -> every cell of every
    pool -> plane 1 (FOKA/PINGWIN/YETI and ANIPL2/22/3 ride
    the lion's own plane 1). KADMAR2 `TALABA` -> everything
    0. The engine's cell maps equal the port's PR-208
    animacje maps everywhere checked (TUNGA.11 1,2,2,2,2;
    ENA2 2,2,3) - the port's animacje law IS the engine's
    law, extended verbatim to the other two keys.
      THE PORT NOW RIDES THE LAW (PR-213): plany_parse fills
    cell_plane[] = the raw segment law for every cell of every
    pool (independent of `ok` - the engine never refuses a
    spec) and the placement loop takes cur_layer from it for
    ALL THREE keys. Measured after the change: ZARIA.01
    KAMIEN layer 1 -> 2 (with ZWIERZ1/ANIPL2 unchanged at
    2), LONA.05 PINGWIN/IGLO layer 2 -> 1 = the lion's own
    plane, KADMAR2 unchanged at 0. The lv_plany2 KAMIEN pin
    and the planies selftest were re-derived honestly to the
    engine values BEFORE the code changed (failing test
    first); verify-play's ZARIA LICHA legs are unmoved (that
    entry sits in cell 2, the one plane where the old index
    model already agreed). All 12 shipped levels author a
    plany= key, so the no-spec fallback never fires on
    shipped data.
      origin: PR-208; consumer read PR-209; law read PR-212;
      port PR-213

OI-110: the planies ACTIVATION chain is unread: what feeds the
  [level+0x64] chain the 'L' program walks, what sets the
  [obj+0x2c] flag the +0x60 predicate reads, and what the +0x74
  verb performs
  status: OPEN (opened by PR-213, which moved the named
    remainders here when OI-109 closed)
  notes: PR-212 mapped the surface exact-bytes: the k-th 'A'
    calls 0x4b360, which walks cell k of ALL THREE pools and
    runs +0x60 (0x23b80, `mov eax,[eax+0x2c]` - the
    predicate) then +0x74 on entries whose flag is set; 'L'
    runs the same pair on the [level+0x64] chain through
    0x4b2f0; [level+0x64] is zeroed at level load (0x4d75d),
    so whatever feeds it does so after that point (PR-105
    item 7). The placed animals move without any 'A' ever
    firing for their benefit, so creature motion does not
    route through that pair - the pair is specifically the
    per-cell-index activation path for placement entries.
    Also named: nothing besides the draw-list insert (PR-208:
    0x52d67 -> 0x57530 keyed on [obj+0x24]) and the
    collision-plane gate is known to consume [obj+0x24]; the
    propagation verbs +0x5c variants (0x1ecb0 -> [obj+0x120],
    0x332f0 -> [obj+0xee], 0x554f0 -> child array
    [obj+0x40]x[obj+0x3c]) imply the plane flows to attached
    and child objects for some consumer still unnamed.
    PR-214 NAMEDS THE FIRST SUCH CONSUMER: the class slot
    +0x7dc pair-answer responder. TLewZaria (0x96960) holds
    0x531b0 there, which calls [[asker+8]+0x5c] with
    edx = [self+0x24] (the plane), then 0x4b140 - a second
    reader of the planies string [level+0x30]: it returns the
    count of 'A' before the first 'L' when the object is the
    lion ([level+0x394]), else the first index whose pool
    cell exists (0x4b0f0: i < [level+0x44+k*4] and
    [[level+0x58+k*4]+i*4] != 0, k = 0..2) - and 0x4aa70
    resolves that (key, cell) through 0x52c10. So the ZARIA
    lion answers pair-asks by plane-matching against the
    planies segment that carries the 'L' (= 2 for ZARIA, the
    same number PR-213's cell_plane gives). Still unread
    here: the [level+0x64] feeder, what sets [obj+0x2c], and
    the 13-byte answer sets the +0x78 entries build (the
    0x3d990 variant at 0x3d9e5, held at +0x78 by the five
    lion-base tables 0x96874/0x991dc/0x9a1a4/0x9ad48/
    0x9e740).
  origin: PR-212 remainders; row opened PR-213


OI-111: which row-step path the port's row clock models -
  a `Wait` clause SELECTS the path, it never refuses the row
  status: OPEN (opened by PR-217; REWRITTEN by PR-218, whose
    read settled the half this row asked)
  notes: SETTLED by PR-218, on the engine and on the shipped
    data, so the clause stays captured and un-actioned:
    (1) The clause's MODE byte at [set+0xc] is written by the
    clause parser: 0x47d73 clears it, 0x47db2-0x47dd0 write 1
    for `!`, 2 for `*`, 3 for `|`, and a comma writes nothing
    (it adds a second signal to the same clause). (2)
    TEventSet::match 0x47e40 (its name: the error string
    0x919ca) switches on that byte through 0x47e30: mode 0
    (0x47e63) is true when (self AND other) EQUALS self byte
    for byte, i.e. the clause is a SUBSET of the set handed
    to it; mode 1 (0x47edc) when the AND is EMPTY (shares
    nothing); mode 2 (0x47f42) when it is NOT empty; mode 3
    (0x47fb2) when the two sets are byte-identical. (3) The
    row step 0x521f0 asks the ROW object ([rec+0x2e][rec+0x4],
    class 0xa2638) through its test slot +0x8, which IS the
    chain delegate 0x45800; the ctor 0x45780 stores each
    directive node's operand at [node+0x0] under the base class
    word 0xa1000, 0x45800/0x45870 answer with the operand's
    answer (0x45800 EMPTY if there is none), and a clause node
    COMPOSES - 0x45970 and 0x45920 call the chain getter
    0x45870 and OR their own [node+0x8] in (0x49580 at
    0x4593c). (4) 0x5223c then tests that answer with 0x495b0
    (true only if all three data dwords are zero) and
    `je 0x522fc` sends the NON-EMPTY answer to the execute slot
    +0xc and OR-s its result into the CALLER's set (0x5230c);
    the row then runs only if its authored frame [row+0x10] is
    BELOW the object's clock [obj+0xa1] (0x52320-0x5232b), the
    difference being its budget (0x52338). Only an EMPTY answer
    falls through to 0x52251, which OR-s the caller's key with
    the object's own EventSet [obj+0x60] and calls match at
    0x522e6: 0 skips the row (0x522ed), 1 runs it with a ZERO
    budget. (5) On shipped data the match test is INERT: 230 of
    the 251 captured clauses are mode 0 and 21 mode 1, `*`/`|`
    appear in zero rows, and every clause leaves a NON-empty
    set (K_NEVER OR-s 0x60 into data byte 4 at 0x4806f-0x4807d;
    K_ANY OR-s the mask 0x2001f8 into dword 1 at
    0x48037-0x48048), so no shipped row reaches match WITH a
    clause, and the clause-less rows that do reach it carry an
    empty clause, which is a subset of anything. The row clock
    for a row that has no clause is therefore not consulted at
    all.
    WHAT IS LEFT, and it is now a single question: the port's
    row clock (PR-163's record step, fed 100 by the state
    record's reset 0x52362 at [rec+0xa1], and PR-216's index
    gate) corresponds to WHICH of the two branches - the
    frame/clock compare at 0x52320 that a clause-bearing row
    takes, or the zero-budget immediate path that a clause-less
    row takes? Every ZARIA.01 ladder row is of one of those two
    shapes (`[A_UP]` row 1 `1,0,WAIT K_GEN1`, `[A_DOWN]` row 11
    `11,0,WAIT K_NEVER`), so the answer decides the climb. The
    port keeps the clause (ll_mov_fr.gsig_id/gneg/gop) and acts
    on none of it, so no frame moves until that is read.
    Census through the port's own parser (pinned in
    verify-endlevel): 251 Wait rows in 133 of the 777 members,
    4 tails NOT captured (1 tokenless `Wait ;`, 3 two-token
    incl. `WAIT |K_ANY,rdir`), 21 negated rows, 161 no-bit
    operands - all 161 K_NEVER after PR-218's terminator fix,
    0 K_ANY, 0 unknown. The LION's answer slot at its class
    table 0x96948+0xd4 is 0x544d0, an EMPTY override (the
    PR-153 boundary-artifact trap), so the `state index == 2` +
    AC_BREAK_NEXT case at 0x54482 is a no-op for the lion.
    PR-219 named the clock itself, and CORRECTS the two claims
    above about it: the field is the OBJECT's [obj+0xa1], not
    [rec+0xa1]; the row reset entry is 0x52360 (0x52362 is a
    2-byte dump artifact) and at 0x5236b it writes ZERO, not
    100. Its writers are the row-run verb 0x520c0 at
    0x52114-0x52118 (it sets [obj+0xa1] to the budget the row
    step handed over: clock - frame on the clause path, 0 on the
    clause-less path) and its readers are the row step at
    0x52323 and the accumulator 0x543a0, which is its FEEDER:
    it reads the state's control bits [rec+0x12] & 7, jumps the
    table 0x54380, and adds |[obj+0xca]| (0x543c6-0x543d1),
    |[obj+0xce]| (0x543dc-0x543e7) or the scene frame delta
    [scene+0x388] gated by [obj+0xba]/[obj+0xbe]
    (0x543f2-0x54414, 0x5441b-0x5443d) - the port's own
    distance-fed vs wall-time-fed split (PR-110/PR-114), now
    with names. 0x520c0 is the record class 0xa260c's +0x18,
    reached as `call [esi+0x18]` with esi = [rec+0x44] = the
    record's own class word (the vtables here are the base+8
    kind, which is also why 0xa11c8 = 0xa11c0+8 and
    0xa11e0 = 0xa11d8+8); it clones the row, composes the key
    with the global 0x98278, runs the row through its class's
    +0x10 with edx = 6 (0x5213d), tests the answer against 6
    (0x52140), and does the list-end/wrap: 0x521b6-0x521d4
    compares [list+0x8] - 1 with the cursor and OR-s bits 4 and
    8 into the CALLER's set byte 0 (0x521a1, 0x521c1) -
    PR-135's list-end bit, now named. What the row class's
    +0x10 verb DOES, and which authored number the row step's
    compare actually gates on, is OI-112.
  origin: PR-217 record; rewritten from the PR-218 record; the
    ladder chain stays OI-078
    PR-220 names the control bits that switch picks between:
    the `control=` parser 0x51b0d takes the first token of the
    line (` %[^,| \r\n\t]` at 0x9215d) and OR-s it into the
    record byte [rec+0x12]: BY_X = 1 (0x51b6f), BY_Y = 2
    (0x51b89), BY_TIME = 4 (0x51ba3), BY_TX = 8, BY_TY = 0x10,
    then FRAME_REV 0x20, SEQ_REV_X 0x40, SEQ_REV_Y 0x80, with
    more at [rec+0x13] (0x51c59). So 0x543a0's switch is
    case 0 -> |[obj+0xca]| (walked X), 1 -> |[obj+0xce]|
    (walked Y) (0x54380-0x54390), 2 -> 0x543fb, 3 -> 0x543f2
    and 4 -> 0x5441b, both scene-frame-delta paths gated by
    [obj+0xba]/[obj+0xbe]. NOTE for the port: its
    LL_CTRL_BY_TIME = 1 / LL_CTRL_BY_X = 2 constants are its
    own vocabulary and do NOT equal these engine bit values.
    PR-224 splits the clause verbs cleanly and narrows this
    row again: a clause node's TEST (+0x08) answers with its own
    authored set (0x45970 copies [node+0x8..0xc]) and its
    EXECUTE (+0x0c) is the base delegate 0x45870, EMPTY because
    a clause node has no operand - the node ctor 0x45780 writes
    0 at [node+0] and only clones an operand it is given, while
    the clause parser 0x47d60 fills the set INLINE at
    [node+0x8]. The `GEN` clause 0xa11e0 keeps the base test and
    its execute 0x45920 ORs [node+0x8] in through 0x49580 - the
    engine's raise path for `GEN <signal>`, OR-ed into the
    CALLER's set at 0x5230c. The clause's CHAIN verb is also the
    base walker with no operand, so a `Wait` clause cannot be
    what holds a row in the answer-6 repeat loop: the answer-6
    holders are the SIXTH (+0x14) verbs of the per-directive
    classes - 0xa1048 (`goto` family) = 0x45d90, jump on the row
    cursor, 0xa1030 = 0x45dc0, delta, and 0xa1090 (`F`) =
    0x45d50, gated on the node's own trigger byte. The clause
    selects the PATH; a directive holds the row.

OI-112: which authored number a row's TIME is - the engine
  scales and gates on the value it scans FIRST, the port
  scales and gates on the value written SECOND
  status: OPEN (opened by PR-219)
  notes: PR-219 read the row object end to end. The ctor entry
    is 0x515e0 (the 0x515e2 PR-217 quoted is a 2-byte dump
    artifact): push esi/edi, call the node ctor 0x45770, then
    +0x8 = 0, +0xc = 0, +0x10 = 1, +0x14 = 0, the class word
    0xa2638 at +0x4, then the line parser 0x51660 on the
    pushed text. 0x51660's first act is a scan of the line with
    the format at 0x92115, whose bytes are `%hi` (its
    neighbours are `time` 0x92119, `,%hi` 0x92122, `sample`
    0x92127, `time_scale` 0x92131), writing the FIRST number of
    the row line straight into [row+0x10]; 0x51679-0x5168f then
    multiply that value by a factor taken from the caller's
    frame (`imul edx,ebp`) and divide by 1000 (`mov ebp,0x3e8`,
    `idiv ebp`) and store the result back at [row+0x10] - that
    is PR-110's `* time_scale / 1000`, the only such site in the
    image. And [row+0x10] is what the row step compares against
    the object clock at 0x52320-0x5232b. The SECOND value is
    handled at 0x51692-0x516d2: its magnitude goes to [row+0xc]
    with a sign flag at [row+0x14], and when it is ZERO the
    engine does not store 0 - 0x516af-0x516c2 reads a key
    string held at [scene+0x398] and looks it up through 0x44110
    (the same name tables 0x961fc/0x96200 the clause parser
    uses) and stores THAT at [row+0x8]. The clone 0x51622-0x51655
    copies +0x8/+0xc/+0x10/+0x14.
    WHY THIS IS A QUESTION AND NOT A FACT: the shipped row
    grammar puts an INDEX first and a TIME second - KADMAR.07
    `_0_0.MOV` line 121-124 is `time_scale=500` then
    `0,100` and `1,1000,gen K_GEN1`, and line 231 is
    `-1,200,I 100,0` (NEGATIVE first values exist, and they are
    PR-107's authored start-row numbers) - while the port parses
    `.frame` from the first and `.thresh` from the second,
    scales `.thresh` by time_scale (ported/ll_play.c:896,
    PR-110) and gates the row on `.thresh`
    (ported/ll_level.c:1506-1514, `o->row_t < f->thresh`).
    Either the engine's [row+0x10] is NOT the row's main gate
    (a delay, with the real gate inside the row-class verb the
    row-run verb calls at 0x5213d with edx = 6, which PR-219
    did not read), or the port's field mapping is mirrored.
    DO NOT change behaviour from this row until that verb is
    read: PR-110/PR-135's fixtures pin the second-number model
    and pass, and the row step's compare is the one place the
    first number is known to be used. The experiment that
    settles it is named in the PR-219 record: a KADMAR.07-style
    block with time_scale far from 1000 and a first number that
    is not 0, measured against the port's row clock.

    PR-220 resolves the FIRST half of this row and narrows the
    rest. The two callers of the row ctor are 0x51ac1 and
    0x51f54, both inside the state record's row-list builder
    0x51eb0 (called from the state ctor 0x517e0 at 0x517c9 and
    from the row rebuild at 0x52568). 0x51f54 is the one that
    builds authored rows: it passes edx = the row line, ecx = a
    string copy of it (0x57fd0 -> 0x76fa3) and ebx = the global
    0xceff4, and pushes `[rec+0xe]` as the ctor's scale factor
    (0x51f37-0x51f3e) - [rec+0xe] is the block's `time_scale`
    (written at 0x519c3 by the block-settings parser 0x51910).
    So time_scale multiplies the value scanned into [row+0x10],
    the FIRST authored number, and nothing scales the second.
    That gate is a compare, not a subtraction: 0x52320 tests
    `[row+0x10] >= [obj+0xa1]` and if the clock has NOT reached
    it the step runs nothing; when it has, the row runs with the
    OVERSHOOT `[obj+0xa1] - [row+0x10]` (0x52334) and the clock
    is not decremented. On KADMAR.07 `[A_DEAD]` (`time_scale=500`
    then `0,100`, `1,1000`, `2,400`...) the engine's gate is
    0, 0, 1, 2... on a monotonic clock while the port's is
    50, 500, 200... subtracted from the accumulator: the two
    agree only by accident of the authored first column counting
    0,1,2,3. The second number's consumer is still unnamed:
    PR-220 found no reader of [row+0x8]/[row+0xc] anywhere on
    the step path (the row's own +0x8/+0xc verbs are PR-218's
    chain delegates, its +0x10 is a third chain delegate).
    PR-220 also ruled out the obvious places: dumping the class
    tables (the node base 0xa1000, the row class 0xa2638, the
    record class 0xa260c) shows no verb reading [row+0x8] or
    [row+0xc]. Same PR named 0x52072, the record's `time=`
    bounds consumer, which answers PR-163's open half: with
    max != 0 it stores [rec+0xc] = min + rand * (max - min) /
    0x7fff (0x52081-0x520aa); with max = 0 - the shipped
    single-value `time=220` form - it stores min.
    PR-221 CORRECTS the premise of that search: 0x51660's scan
    has exactly ONE parse destination ([row+0x10], three pushes
    and one `add esp,0xc`), so the row ctor never parses a
    second authored number; [row+0xc] receives +2/-2/-1 with a
    flag (0x5169d-0x516d2) and [row+0x8] the caller's default
    (ds:0xceff4 at 0x51f48, or a settings-block name lookup at
    0x51a92) - not a row-line value. Shipped data: all 18617
    row lines carry two numbers, 0 carry one, the first column
    reaches at most |82| and walks up and down (ENA.03_ROZGW
    `-5,4` ... `-8,4` ... `-6,4`). Where the second number goes
    is now the question, and the row's LENGTH is separately
    explained without it: 0x52140 skips the list walk entirely
    while the chain answers 6, so the row is re-run (PR-135's
    bit-setting walk is the else branch, 0x52195-0x521d4).
    Candidates not settled: the clause/directive node family
    (which makes PR-218's "the clause selects the path, never
    refuses the row" worth rereading) and state kept in the row
    CLONE 0x520e1. DO NOT change behaviour on any of this: the
    port's `.thresh` model is unpinned by the engine, not
    refuted - its claimed source (`%hi` into node+0x10) is the
    FIRST number, and the second has no named engine site.
    PR-222 adds the negative datum that nothing COULD read it:
    a row line's third field is always a directive word from the
    engine's own keyword table (0x91847: `rev`, `zero_v`,
    `zero`, `zero_t`, `vol_music`, `smp`/`smp2`/`smp3`,
    `smpauto`/`smpauto2`/`smpprior`, `vol`, `smp_loop`,
    `smp_unloop`, `smp_stop`, `atc_on`, `atc_off`, `lock`,
    `lock2`, `unlock`, `unlock2`, `ground_on`, `ground_off`,
    `chdir`, `ldir`, `rdir`, `pos`, `goto`, `gotorel`,
    `goto_rel`, `I`, `F`, `IC`, `FC`, `M`, `MOVE`, `GEN`,
    `Wait`), an unknown word ends the tail (0x46c1c reports it
    and returns 0), and ZERO shipped row lines carry a bare
    number there. The row is re-run while one of ITS OWN
    directive nodes answers 6 (0xa1038/0xa1040 verbs at
    0x45dc0/0x45d90 write [node+0x8] into the caller's row
    cursor and answer 6), so no row duration field is needed
    by the engine at all. PR-222 also found where the rolled
    `time=` number IS used as an index: the record's row-reset
    verb 0x52360 (class +0x1c) rolls it through 0x52070 and
    writes the row cursor [rec+0x4] = k, or count-1-k when the
    state's 0x10/0x20 bits say so (0x523bc-0x523cb) - entering
    a state starts at a row chosen by that number, which is a
    row-sequence rule, not a row duration.

    PR-224 adds one named consumer of a row tail's argument,
    which shows the answer-6 mechanism working on data: the
    `end_level <n>` node 0xa1048 has 0x45d90 as its SIXTH verb
    (+0x14) and the chain walk calls exactly that slot -
    `*[ebx] = [node+0x8]` with ebx = the row cursor [rec+0x4],
    answer <- 6 - so the authored number is written into the ROW
    CURSOR and the row is re-run instead of advanced. The same
    number also reaches the scene's exit-word field through
    0x45750 (PR-194/PR-199). TUNGA.11's lion rows
    `10,6,end_level 1` ... `12,6,end_level 1` are that shape.

OI-113: the engine control bit table is read (PR-230) and the
  port's ctrl table lags it: CHECK_ALL must also set the
  CHECK_LN ground-rule bit (engine 0x1c0), and
  CHECK_BASE_LN/INV_V/BY_TX/SEQ_REV_X/SEQ_REV_Y have no
  entries or bits; separately the row-order word [obj+0x99]
  (reset 1 at 0x52377, negated by `rev` 0x459b0, backward
  cursor walk 0x521a1-0x521b4) and the ldir/rdir/chdir apply
  verbs writing the +/-1 direction word [obj+0x6d] are
  unmodelled - ZARIA.01's lion [A_WALK] uses bare CHECK_ALL
  and its A_DEAD row 181 and A_LOOKUP row 629 use rdir/rev
  status: OPEN (opened by PR-230)
  origin: PR-230
  notes: half one IS DONE (PR-231): ctrl_flag now sets the
    CHECK_LN rule bit from CHECK_ALL (engine 0x1c0), and the
    ZARIA.01 walk legs re-pinned to the authored slope and its
    edge at x=408. Half two stays open: no entries/bits for
    CHECK_BASE_LN/INV_V/BY_TX/SEQ_REV_X/Y; the [obj+0x99]
    row-order word, the backward cursor walk, and the
    ldir/rdir/chdir/rev apply verbs are still unmodelled.
