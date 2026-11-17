/* ll_play.h - playable-level core (M4-B, PR-43): .plt ground polylines,
 * the extended .MOV state-machine parser (physics + transitions), and the
 * player physics runtime driven by real LEW.MOV data.
 *
 * Pure CPU, no SDL, no .SEQ decoding, no container access: ll_level.c owns
 * loading and drawing; this module owns the platform grammar and the
 * simulation, so every leg of it is hermetic and CI-testable.
 *
 * Data provenance (verified 2026-11-16 on LL.iso + the HD-install tree):
 *   <level>.plt   plain text (HD ETAPY block): a polyline count line, then
 *                 "n x,y;x,y;..." world-coordinate ground lines (RAN.01:
 *                 RAN.PLT 153 lines, TUNGA.11: TUNGA.PLT 7 lines). Point
 *                 separators: ';' normally, '!' occurs as an EXACT
 *                 equivalent inside the same polyline (ENA.PLT
 *                 "1 1,560!7500,570;", ZARIA_A.PLT 10 times) - treated
 *                 identically, the leading count still counts SEGMENTS.
 *   LEW.MOV       plain text (HD ANIMACJE player leaf, 8666 B in RAN.01):
 *                 the lion's real state machine - 30 states; each block
 *                 adds F=/T=/G= physics (acceleration px/s^2, speed cap
 *                 px/s, gravity px/s^2 - walk states F=400 T=90, grounded
 *                 states G=0 + CHECK_LN, the real fall state A_START_LAD1
 *                 G=0,200), "TARGET(p)=COND" transitions (K_FWD/K_BKWD/
 *                 K_UP/K_DOWN/K_GEN1..3/K_ANY/K_JUMP/AC_TIME/AC_BREAK_NEXT,
 *                 "!" negates,
 *                 "|" ORs; the (..) token is the target's START ROW
 *                 1-based when numeric (PR-86, see ll_mov_tr.entry),
 *                 non-numeric - real KADMAR2 writes "A_MASZERUJ(x)=K_FWD" -
 *                 means the top; and a transition
 *                 targeting its OWN state means KEEP running, no frame
 *                 reset and no event) and per-state "seq=<stem>" art
 *                 selection. PR-85 MODEL CHOICE (recorded, see cond_true):
 *                 K_FWD/K_BKWD are FACING-RELATIVE (K_FWD = the arrow in
 *                 the facing direction) - the only reading under which
 *                 ZARIA-style data (one A_WALK, no backward-walk state)
 *                 can walk left at all. Condition names the data uses
 *                 but this runtime does not model (K_FIRE, K_ALT,
 *                 K_F1..F12, the engine-internal AC_* signals) evaluate
 *                 FALSE - nothing is guessed.
 * Physics constants therefore come from the data; the single project-owned
 * constant is LL_PLAY_GRAVITY (200 px/s^2, the same magnitude as the real
 * A_START_LAD1 G=) used only when a state marked CHECK_LN loses its ground
 * without an authored fall state to route into (documented simplification:
 * RAN.01's authored chain A_START_LAD1 -> A_LADOW is an 18.4 s scripted
 * waterfall landing, not a curb step; see NEXT_STEPS.md PR-43).
 */
#ifndef LL_PLAY_H
#define LL_PLAY_H

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ .plt
 * Ground polylines in world coordinates (x right, y DOWN, feet on top).
 * Parse allocates; free with ll_plt_free. */
#define LL_PLT_MAXPT 128                    /* points kept per polyline    */
typedef struct { int x, y; } ll_xy;
typedef struct { ll_xy *p; int n; } ll_poly;
typedef struct { ll_poly *line; int n; } ll_plt;

/* text: first non-empty line = polyline count, rest = "n x,y;x,y;..." with
 * ';' or '!' point separators (equivalents in the real files); the leading
 * per-line number counts SEGMENTS, so every coordinate pair is read.
 * 0 = parsed (garbage lines are skipped; a header that DECLARES 0
 * polylines is a complete file that simply has no ground - JUNCTION's
 * platformy=none.des is literally "0"), 1 = nothing usable. */
int  ll_plt_parse(const char *text, ll_plt *out);
void ll_plt_free(ll_plt *o);

/* Ground CONTINUATION while walking: nearest segment covering x whose y is
 * within tol of want_y (slope/step following). 0 = none (stepped off an
 * edge). hit.y is the interpolated surface y. */
int  ll_plt_surface_at(const ll_plt *o, int x, int want_y, int tol,
                       ll_xy *hit);
/* FALLING: first surface at or below from_y (highest candidate reached
 * first while falling). 0 = none (continue falling). */
int  ll_plt_first_below(const ll_plt *o, int x, int from_y, ll_xy *hit);

/* ------------------------------------------------- .MOV state machine ---
 * EXTENDED version of the viewer's mov_t (ll_level.c PR-38): same frame
 * semantics (a frame number is 1-based, its sign the mirror flag, see
 * ll_mov_frame_ref below;
 * non-positive ms clamped to 100 - the M4-C rule), plus the physics /
 * transition grammar above. Caps are sized from the shipped data, not
 * guessed (measured over all 777 .MOV, PLAN 8g): 40 states in one file,
 * 97 frames in one state, 30 transitions in one state, 4 condition parts
 * in one expression. ll_mov_parse counts anything it had to drop. */
#define LMOV_MAXST 64
#define LMOV_MAXFR 128
#define LMOV_MAXTR 32
#define LMOV_MAXCON 4                       /* parts per condition expr    */

/* control= flags the model acts on. The real vocabulary is a | separated
 * (rarely , separated) list; measured use counts (PLAN 8g):
 *   BY_TIME 2101  FRAME_REV 798  CH_DIR 552  CHECK_LEW 535  DANGER 385
 *   CHECK_ALL 356  CHECK_LN 340  CHECK_ANI 168  SOLID2 152  SOLID 141
 *   ANTI_POCISK 121  BY_X 105  SEMI_SOLID 92  ANTI_SOLID 87
 *   CHECK_ALL_PLANE 62  CHECK_ZW 47  SEQ_REV_Y 22  BY_Y 13  BY_TY 12
 *   CHECK_BASE_LN 8  SEQ_REV_X 7  INV_V 3
 * `BY_TY` and `BY_Y` are NOT typos/aliases in the engine, whatever this
 * comment used to say (PR-102): the mode field at bits 0..2 of the
 * state's control word has five values - BY_X 0 (sets nothing), BY_Y 1,
 * BY_TIME 2, BY_TX 3, BY_TY 4 - and the dispatcher's jump table
 * (0x54380) gives mode 1 its own feed of |v_y| (0x543dc: `mov
 * eax,[obj+0xce]` -> abs -> `add [obj+0xa1],eax`), while mode 4 is a
 * separate body (0x5441b) guarded by `[obj+0xbe] != 0` that feeds the
 * same level constant 100 as modes 2/3, which are guarded by
 * `[obj+0xba] != 0`. BY_TX appears in 0 shipped files and every BY_TY
 * state is a climb/hang state of the lion, which fits the vertical
 * reading; neither is modelled here (no LL_CTRL bit acts on either) and
 * nothing is pinned from this. Names without a
 * bit below are parsed, named here for the record, and counted in
 * ctrl_other - never silently dropped, never invented into a behaviour. */
enum {
    LL_CTRL_BY_TIME         = 1u,
    LL_CTRL_BY_X            = 2u,           /* walk states: move at speed  */
    LL_CTRL_CHECK_LN        = 4u,           /* keep-ground / fall-off rule */
    LL_CTRL_CH_DIR          = 8u,
    LL_CTRL_CHECK_ALL       = 0x10u,
    LL_CTRL_CHECK_ALL_PLANE = 0x20u,
    LL_CTRL_FRAME_REV       = 0x40u,
    LL_CTRL_BY_Y            = 0x80u,
    LL_CTRL_SOLID           = 0x100u,
    /* PR-59: the game's own flag word (token parser 0x51b30..0x51dc3, flag
     * word at +0x12 of the state): SOLID = 0x400, SOLID2 = 0x20400,
     * SEMI_SOLID = 0x40400 (both include SOLID's bit), ANTI_SOLID = 0x10000,
     * ANTI_POCISK = 0x80000. FUN_00053ae0 tests exactly these. */
    LL_CTRL_SOLID2          = 0x200u,       /* also sets LL_CTRL_SOLID    */
    LL_CTRL_SEMI_SOLID      = 0x400u,       /* also sets LL_CTRL_SOLID    */
    LL_CTRL_ANTI_SOLID      = 0x800u,
    LL_CTRL_ANTI_POCISK     = 0x1000u,
    /* PR-60: the mover's own list selectors (FUN_00052780 0x527b3..0x5282f):
     * CHECK_ANI (0x40) = test against the animacje= objects, CHECK_ZW (0x80)
     * = the zwierzaki= objects, CHECK_LEW (0x200) = against Lew; with
     * CHECK_ALL_PLANE the plane gate is dropped. CHECK_ALL = ANI|ZW|LN in the game (0x1c0); here it
     * keeps its own bit (the port's LN physics is unchanged) plus ANI|ZW. */
    LL_CTRL_CHECK_ANI       = 0x2000u,
    LL_CTRL_CHECK_ZW        = 0x4000u,
    LL_CTRL_CHECK_LEW       = 0x8000u,
    /* PR-90 (T3): DANGER. The game's own flag word carries it as 0x2000:
     * the touch path tests exactly that bit of the state word at
     * 0x5528b/0x51019 (`test ah,0x20`) and that branch then reads the
     * level's danger_power at [level+0x54]. The authored data carries it
     * on every enemy walk state, and the port uses it as the attack
     * arming PRECONDITION - a model: the engine's per-class AI is
     * compiled and reads no control flag (NEXT_STEPS PR-90 record,
     * HANDOFF §7 item 10). Here it is an internal bit again (the
     * enum's values never are the wire values). */
    LL_CTRL_DANGER          = 0x10000u
};
#define LL_CTRL_NAMES \
    X(BY_TIME) X(BY_X) X(BY_Y) X(BY_TY) X(CHECK_LN) X(CHECK_LEW) \
    X(CHECK_ALL) X(CHECK_ALL_PLANE) X(CHECK_ANI) X(CHECK_ZW) X(CHECK_BASE_LN) \
    X(CH_DIR) X(DANGER) X(SOLID) X(SOLID2) X(SEMI_SOLID) X(ANTI_SOLID) \
    X(ANTI_POCISK) X(FRAME_REV) X(SEQ_REV_X) X(SEQ_REV_Y) X(INV_V)

/* transition condition ids (K_ANY = any held key; OTHER = recognized
 * token we do not simulate - never true, logged once by the caller).
 * The authored expressions separate ALTERNATIVES with '|' (49 lines
 * start with one) and CONJUNCTIONS with ',' (466 of 2911 lines carry
 * comma groups); a ';' inside a group (3 lines) stays one OTHER token.
 * PR-87 MODEL CHOICE, data-motivated: ',' = AND. Proof it cannot be a
 * no-op or OR: `A_WPAD(1)=AC_ONAIR,AC_DRECT` and `A_WSP(3)=
 * AC_DRAB_CATCH,AC_ONAIR` pair a catch trigger with the state of being
 * airborne (an OR there makes the pair meaningless), and every player
 * attack line is `K_FIRE,K_Fn` - fire WHILE weapon n is selected (the
 * game's F1..F4 weapon keys + Ctrl attack, per the walkthrough). */
enum {
    LL_COND_NONE = 0,
    LL_COND_K_FWD, LL_COND_K_BKWD, LL_COND_K_UP, LL_COND_K_DOWN,
    LL_COND_K_GEN1, LL_COND_K_GEN2, LL_COND_K_GEN3,
    LL_COND_K_ANY, LL_COND_AC_BREAK_NEXT,
    LL_COND_K_GEN4, LL_COND_AC_TIME,   /* PR-69: scene objects (not the lion) */
    LL_COND_K_JUMP,                    /* PR-74: the jump key               */
    /* PR-87: the fire family. K_FIRE = the attack button (the walkthrough
     * names Ctrl); K_F1..K_F4 are the WEAPON SELECTOR keys the comma
     * groups gate attacks with (K_F1 81, K_F2 38, K_F3 11, K_F4 6 uses
     * in the player .MOV files; K_F6..F12 also exist, stay OTHER);
     * K_ALT = the alternate-modifier condition (28 uses: A_FIGHT_END /
     * throw routes). */
    LL_COND_K_FIRE, LL_COND_K_F1, LL_COND_K_F2, LL_COND_K_F3,
    LL_COND_K_F4, LL_COND_K_ALT, LL_COND_K_GEN5,
    LL_COND_OTHER
};

/* PR-88: the engine's signal bit table. The object's signal bitset at
 * [obj+0x60] is a TEventSet (error string "TEventSet::match" at 0x919cd);
 * the name->bit function at 0x48000 compares a condition/GEN token against
 * the name block at 0x919ec..0x91c40 and sets an EXPLICIT bit constant per
 * name (verified constants: AC_BLOK mov al,0x0; AC_BLOK_Y 0x1;
 * AC_BREAK_NEXT 0x2; AC_ENEMY mov dl,0x7; AC_ONAIR mov dl,0x12;
 * K_GEN1 mov al,0x36 = 54; K_GEN2 0x37 - PR-70's "bit 54 = K_GEN1"
 * assumption CONFIRMED). K_ANY (and the AC_K_ANY alias) and K_NEVER /
 * AC_K_NEVER are NOT bits: the clause parser at 0x47d60 special-cases them
 * into node op tags ('!' = negate, '*' = and-with-set, '|' = separate node
 * = OR), and a ',' keeps folding bits into the SAME node mask - the
 * engine's own evidence for PR-87's OR-of-AND-groups reading. AC_EV_MAX =
 * 68 bounds the table, so the node/set masks are 3 dwords. */
enum {
    LL_SIG_AC_BLOK = 0,     LL_SIG_AC_BLOK_Y,     LL_SIG_AC_BREAK_NEXT,
    LL_SIG_AC_BREAK_PREV,   LL_SIG_AC_DUP,        LL_SIG_AC_OFF,
    LL_SIG_AC_DEADDUP,      LL_SIG_AC_ENEMY,      LL_SIG_AC_EDGE,
    LL_SIG_AC_TIME,         LL_SIG_AC_DRAB,       LL_SIG_AC_DRABK,
    LL_SIG_AC_DRAB_CATCH,   LL_SIG_AC_DUP_CD,     LL_SIG_AC_STONE,
    LL_SIG_AC_PUSH,         LL_SIG_AC_ENEMY_ATC,  LL_SIG_AC_RECT,
    LL_SIG_AC_ONAIR,        LL_SIG_AC_DRECT,      LL_SIG_AC_DRECT2,
    LL_SIG_AC_LINA,         LL_SIG_AC_LINA_CATCH, LL_SIG_AC_LEAVE,
    LL_SIG_AC_ATAK,         LL_SIG_AC_UP_DUP,     LL_SIG_AC_SPOKUJ,
    LL_SIG_AC_BUTLA,        LL_SIG_AC_BECZKA,     LL_SIG_AC_START,
    LL_SIG_AC_MRECT,        LL_SIG_AC_TABLICZKA,  LL_SIG_AC_KILOF,
    LL_SIG_AC_KOREK,        LL_SIG_AC_KOLO,
    LL_SIG_K_BKWD = 35,     LL_SIG_K_FWD,         LL_SIG_K_UP,
    LL_SIG_K_DOWN,          LL_SIG_K_FIRE,        LL_SIG_K_ALT,
    LL_SIG_K_F1,            LL_SIG_K_F2,          LL_SIG_K_F3,
    LL_SIG_K_F4,            LL_SIG_K_F5,          LL_SIG_K_F6,
    LL_SIG_K_F7,            LL_SIG_K_F8,          LL_SIG_K_F9,
    LL_SIG_K_F10,           LL_SIG_K_F11,         LL_SIG_K_F12,
    LL_SIG_K_JUMP = 53,     LL_SIG_K_GEN1,        LL_SIG_K_GEN2,
    LL_SIG_K_GEN3,          LL_SIG_K_GEN4,        LL_SIG_K_GEN5,
    LL_SIG_AC_PLAT_OFF = 59, LL_SIG_AC_PLAT_OUT,  LL_SIG_AC_XCHG_LEVEL,
    LL_SIG_AC_SHOW_CROWN,   LL_SIG_AC_MSG_PUSH,   LL_SIG_AC_WALK_UP,
    LL_SIG_AC_WALK_DOWN,    LL_SIG_AC_APPEAR,     LL_SIG_AC_DISAPPEAR,
    LL_SIG_AC_EV_MAX = 68
};

/* Table index of a signal name (case-insensitive), or -1 for unknown
 * names and for the non-bit specials K_ANY / K_NEVER (+ AC_ aliases). */
int ll_sig_id(const char *name);

typedef struct { int frame; unsigned ms; int dx, dy;
                 unsigned long long gsig; unsigned char atc;
                 int has_i, ix, iy; } ll_mov_fr;   /* has_i: `I ix,iy` impulse tail */   /* gsig: the frame's `GEN <signal>` tail(s), one bit per name
                 at its LL_SIG_* index (PR-88: any table name, not just
                 K_GENn/AC_TIME - the engine's own GEN survey raises
                 AC_PLAT_OFF 181x, AC_TIME 60x ...). Names at index >= 64
                 (AC_WALK_UP/.., AC_EV_MAX) never appear in a GEN tail in
                 any shipped script, so one u64 covers all raised signals;
                 such a name would be counted unmodeled, not guessed.
                 The old K_GENn bit n-1 / AC_TIME bit 4 encodings are
                 gone: the same bits now live at their table indices.
                 atc: this frame's `ATC_ON`/`ATC_OFF` tail (the engine's
                 frame-directive table 0x917c0 has `atc_on`/`atc_off`):
                 bit 0 = the frame OPENS the attack-connect window,
                 bit 1 = the frame CLOSES it. The window decides when the
                 state's shape can land a hit (ZARIA's A_FIGHT opens it
                 at frame 8, right before its `FC 100,0`; the LICHA's
                 A_ATAK opens at -4 and closes at -8). */
#define LMOV_ATC_ON  1u
#define LMOV_ATC_OFF 2u
typedef struct {
    char target[24];                        /* resolved at parse time      */
    int tidx;                               /* state index, -1 unresolved  */
    int entry;               /* PR-86: the numeric (N) = START ROW (1-based)
                              * in the TARGET state's frame list; 0 = from
                              * the top. Evidence: A_PRE_JUMP's impulse row
                              * precedes `A_JUMP(4)=AC_BREAK_NEXT` and
                              * A_JUMP row 4 is its FIRST row after the
                              * `I 150,-1200` row - entering at (4) skips
                              * re-applying the impulse A_PRE_JUMP already
                              * gave; A_JUMP's `A_WALK(7)=AC_BREAK_NEXT`
                              * re-joins ZARIA's 12-row walk cycle at row
                              * 7 (mid-phase, seamless landing); A_KILOF's
                              * `A_JUMP(9)` re-joins the jump mid-air. The
                              * token can be non-numeric (KADMAR2 "(x)"):
                              * then it starts from the top. */
    struct { int cond; int neg; int grp; short sig; } c[LMOV_MAXCON];
    /* PR-87: tokens sharing a grp number are ANDed; the line is true
     * when ANY group is fully true ('|' starts a new group, ',' splits
     * one; see the enum comment). A single-token line is one group -
     * identical behavior to the old OR-everything model. PR-88: sig is
     * the LL_SIG_* table index when the token is a signal NAME that is
     * not one of the modeled conditions (-1 otherwise) - the evaluator
     * tests it against the object's own signal set. */
    int nc;
} ll_mov_tr;
typedef struct {
    char name[24];
    char seqkey[16];                        /* per-state seq= art select   */
    int fx, fy, tx, ty, gx, gy;             /* F= T= G= (px/s^2, px/s)    *
                                             * ENGINE SEMANTICS, PR-98 RE'd
                                             * (NEXT_STEPS PR-98; the port's
                                             * parameterization below is NOT
                                             * the engine's): per tick the
                                             * engine drags first
                                             * (`v -= T= * v/100`, a PERCENT,
                                             * sign flip stops it, x drag
                                             * floored at 20 at 0x54266) and
                                             * then increments
                                             * `v += F_eff*s/20 + G=*facing`,
                                             * F_eff = the state F= plus the
                                             * current row's `FC` pairs (+
                                             * `IC` when obj+0x95 is set),
                                             * s = an unread per-object
                                             * terrain scalar. A row's `F`/
                                             * `I` pairs are instead a
                                             * velocity TARGET (0x54170 sets
                                             * v to it, never past it, x by
                                             * facing, y down-positive, after
                                             * the drag). Its steady state
                                             * v_eq = 5*F_eff*s/T. PR-98 called
                                             * that refuted by the measured
                                             * walks; PR-99 corrects the
                                             * wording. The two OBJECT samples
                                             * (worm, rhino) share T=40, where
                                             * this law and the port's F/4 law
                                             * differ only by a constant, and
                                             * the T=20 sample is the player,
                                             * whose path through 0x54250 is
                                             * unverified (NEXT_STEPS PR-99).
                                             * The mode dispatch PR-99 did read
                                             * is 0x543a0 - PR-100 corrects
                                             * that record's 0x543a1; jump
                                             * table 0x54380 - and PR-100
                                             * found its caller: the state
                                             * record's own step at 0x521f0,
                                             * which compares the current
                                             * row's authored threshold with
                                             * [obj+0xa1] and takes at most
                                             * one row per step. It
                                             * shows a BY_X walk DOES go
                                             * through that loop: mode 0 is
                                             * BY_X, and it feeds the row
                                             * distance accumulator [obj+0xa1]
                                             * from this loop's v/100 (PR-98
                                             * item 2 CLOSED). So the two laws
                                             * differ only in the T dependence,
                                             * and what decides between them is
                                             * a BY_X object at T other than
                                             * 40: the data has F=100 against
                                             * 11 different T pairs, but only
                                             * ZARIA is on video. The struct
                                             * mapping is not the open part:
                                             * 0x52590 parses F= to +0x50/54,
                                             * T= to +0x58/5c (capped at 130)
                                             * and G= to +0x48/4c, and the
                                             * header F= reaches only the
                                             * increment channel. The port
                                             * keeps the px/s reading until
                                             * that T test exists. */
    int time_ms;                            /* time=<ms?>: parsed and
                                             * counted but NOT acted on -
                                             * unit undetermined (ranges
                                             * like 32,64 / 90,120; see
                                             * ll_player_step's note)     */
    unsigned ctrl;
    ll_mov_tr tr[LMOV_MAXTR]; int ntr;
    ll_mov_fr fr[LMOV_MAXFR]; int nfr;
    /* authored content the model reads but does not act on (reported): */
    int ctrl_other;                         /* control= names without a bit */
    int directive;                          /* frame-line GEN/ATC/WAIT/...  */
    int field_other;                        /* sample= time= time_scale=    */
} ll_mov_st;
typedef struct {
    ll_mov_st st[LMOV_MAXST];
    int nst;
    char seqkey[16];                        /* last seq= seen (viewer uses
                                             * this, matching old mov_t)   */
    int drop_st, drop_fr, drop_tr;          /* cap overflows: reported     */
    /* the state LIST every script declares before its first [STATE] (777 of
     * 777 do): its first name is the state the object starts in. 68 scripts
     * write a terminal [A_DEAD] block before the declared start (PR-51:
     * the port used to start those in the dead state). start_st = index of
     * that name among the blocks, 0 when absent/unmatched. */
    char first_decl[24];
    int start_st;
} ll_mov_t;

int ll_mov_parse(const char *text, ll_mov_t *m);   /* returns m->nst      */
int ll_mov_find(const ll_mov_t *m, const char *name); /* state idx or -1  */

/* ------------------------------------------------------- player sim ---- */
/* Held-key view of the input (platform.h translates the arrows; the CI
 * drives the same struct from LEO_PLAY_SCRIPT). */
/* PR-87: the fire family. k_fire = the attack key (viewer mapping:
 * Ctrl, per the walkthrough); k_f1..k_f4 = the weapon-selector keys the
 * data's comma groups gate with (viewer mapping: F1..F4 held); k_alt =
 * the ALT modifier condition (throw/secondary routes). */
typedef struct { int k_fwd, k_bkwd, k_up, k_down, k_jump,
                 k_fire, k_f1, k_f2, k_f3, k_f4, k_alt; } ll_play_keys;

/* Project constant, see header note; used only when airborne with a
 * CHECK_LN state that carries no authored fall route. */
#define LL_PLAY_GRAVITY 200.0
/* PR-74 ESTIMATES (one measured jump, NEXT_STEPS PR-73/74): a frame's `I x,y`
 * impulse is divided by LL_PLAY_JUMP_DIV to get world units/s, and a lion in
 * the air after an impulse falls at LL_PLAY_JUMP_G units/s^2 (the state's
 * G.y=55 x 30). Both reproduce the vine jump measured on video (apex ~21 u,
 * v0 ~262 u/s, g ~1640) within 5%, from a single jump. */
#define LL_PLAY_JUMP_DIV 4
#define LL_PLAY_JUMP_G 1650.0
/* Vertical snap tolerance while walking, BOTH directions (slope
 * following; the real .plt slopes need far less, this bounds curbs and
 * short same-height line gaps - RAN.PLT's 302->323 shelf step is 1 px
 * up across a 20 px gap - without teleporting across real drops). */
#define LL_PLAY_STEP 16

typedef struct {
    const ll_mov_t *mov;
    /* PR-97: x,y is the POSITION POINT the frame art is anchored from (the
     * element's own anchor f0/f2 is measured from it), NOT the spot that
     * touches the terrain. What touches the ground is `y + contact` below.
     * Proof in ll_play.c's ground-logic note: every animation's frame
     * anchors are authored so its SHAPE bottom sits at one fixed offset
     * under the position point (ZARIA LEW.ZAR WALK.SEQ: f2=31 constant for
     * all 12 frames, shape bottom at pos+30..+32; its END.SEQ moves f2
     * from 5 to 54 while the shape bottom stays at exactly pos+58; the
     * ZARIA worm WAZ.SEQ swings f0 from -2 to -22 and f2 from -9 to -16
     * while its bottom stays at pos+46..+49). */
    double x, y;
    double vx, vy;              /* px/s                                   */
    int sti;                    /* current state in mov                   */
    int fi; unsigned ft;        /* frame cursor within the state          */
    int grounded;
    int walkdir;                /* -1/1, set by K_BKWD/K_FWD transitions
                                 * (the real CH_DIR flag's effect)        */
    signed char face;           /* PR-82: instance facing, -1/1, like
                                 * lvl_obj.face (PR-81). Updated from walkdir
                                 * only while the state carries CH_DIR;
                                 * states that bake their direction into the
                                 * frame sign (A_GO_TO_BALON_L) never touch
                                 * it. Starts +1 = what the frames ask for. */
    unsigned long long sig;   /* PR-88: the lion's signal set, a mirror of
                               * the engine's TEventSet at [obj+0x60] with
                               * the LL_SIG_* bit table. One-tick pulses
                               * (cleared every step): raised by a frame's
                               * `GEN <signal>` tail, by gameplay logic
                               * (K_GEN2 = bit 54+1 on landing), and read
                               * by name conditions. The old gen[3] array
                               * is the same K_GENn pulses at bits 54..56. */
    int broke;                  /* AC_BREAK_NEXT pending (frame list just
                                 * completed)                             */
    int ac_time;                /* PR-83: AC_TIME pending - raised by a
                                 * frame's `GEN AC_TIME` tail (measured:
                                 * ZARIA's player .MOV names it on 11
                                 * frame lines); consumed by transitions
                                 * next tick. The state time= timer is
                                 * NOT a source yet: unit undetermined.  */
    int atc;                    /* PR-89: the attack-connect WINDOW is    *
                                 * open. The engine's frame-directive     *
                                 * table (0x917c0) carries `atc_on`/      *
                                 * `atc_off`, and the scripts pair them:  *
                                 * measured over the extracted .MOV texts, *
                                 * 267 state blocks open AND close the    *
                                 * window (ZARIA's A_FIGHT: ATC_ON frame  *
                                 * 8 - just before its `FC 100,0` lunge,  *
                                 * ATC_OFF at 13, re-open at 17; the      *
                                 * LICHA's A_ATAK opens at -4, closes at  *
                                 * -8), 18 open without closing. So the   *
                                 * window is a frame flag that STAYS open *
                                 * until a frame closes it; for those 18  *
                                 * it also ends when the state does: this *
                                 * model clears it on every state change  *
                                 * (the engine's own clearing rule is not *
                                 * pinned; an attack that hit forever     *
                                 * after one swipe is not what the video  *
                                 * shows). The window is what lets the    *
                                 * lion's shape land a hit (0x552e0: a    *
                                 * touch runs the push-out and only then  *
                                 * the hit effects, FUN_00055230). */
    int atc_hit;                /* this window has already landed a hit   */
    int auto_walk;              /* PR-83: the current state was entered
                                 * WITHOUT a K_FWD/K_BKWD condition, so its
                                 * BY_X target velocity is not key-gated
                                 * (cutscene walkers: RAN A_GO_TO_BALON_*
                                 * walks on K_GEN1 with no key held)     */
    double air_from;            /* position-point y when ground was lost  */
    int jumping;                /* PR-74: an impulse put the lion in the air */
    int support;                /* a solid object holds the feet up (set by
                                 * the collision pass, PR-59)              */
    int contact;                /* PR-97: the y BELOW this position point at
                                  * which the animation being shown is solid -
                                  * the deepest shape-box bottom over its
                                  * frame list: ll_seq_shape_bbox's y + h - 2
                                  * (the game's obj+0x0e + obj+0x12 - 2) per
                                  * element, maxed over the state's frames by
                                  * the viewer (ll_level.c mov_state_contact:
                                  * the per-frame value wobbles a few px over
                                  * a cycle, which is the walk cycle's own
                                  * art, not a place to stand).
                                  * The terrain tests are made there: a
                                  * grounded lion stands with his paws on the
                                  * .plt line, not with his position point on
                                  * it. 0 = the animation carries no shape box
                                  * (the game's hit test and push-out skip
                                  * those elements too), which is the old
                                  * position-is-feet behavior. The viewer sets
                                  * it from the state it is showing
                                  * (ll_player_set_contact); ll_player_start
                                  * leaves it 0. */
} ll_player;

enum {                                              /* ll_player_step ev */
    LL_PLAY_EV_STATE  = 1,                          /* state transition   */
    LL_PLAY_EV_GROUND = 2,                          /* landed             */
    LL_PLAY_EV_AIR    = 4                           /* stepped/swept off  */
};

/* Start (or respawn): position point at (x, y); caller resolves the initial
 * ground (ll_plt_first_below) and passes grounded accordingly. Initial
 * state: A_STAY when the .MOV defines it (pinned standing spawn), else
 * the .MOV's declared first state (PR-83: the declared list, not the
 * block order, names the starting state - ZARIA.01), else state 0.
 * p->contact starts at 0 (ll_player_start zeroes the struct); a viewer that
 * drives a real .SEQ must set it every tick - see ll_player_set_contact. */
void ll_player_start(ll_player *p, const ll_mov_t *mov, double x, double y,
                     int grounded);

/* PR-97: tell the sim where the animation it is showing is solid BELOW the
 * position point (the max over its frames of each element's shape box
 * bottom, ll_seq_shape_bbox's y + h - 2; see ll_level.c mov_state_contact).
 * Every terrain test in ll_player_step is made at y + contact;
 * with 0 the position point itself is the feet, as before PR-97. */
void ll_player_set_contact(ll_player *p, int below_pos);

/* One fixed dt_ms tick (CI grid: 20 ms). xmin/xmax clamp the position x
 * (lew_rect). Returns the LL_PLAY_EV_* bitmask of what happened. */
int  ll_player_step(ll_player *p, const ll_plt *ground,
                    const ll_play_keys *keys, double xmin, double xmax,
                    unsigned dt_ms);

/* PR-82: the DRAW mirror of the current frame - the authored frame sign
 * (ll_mov_frame_ref .mirror) XORed with the facing, exactly as
 * ll_level's obj_mirror_of does for scene objects. */
int  ll_player_mirror_of(const ll_player *p, int authored);

/* Current .MOV frame NUMBER of the player (raw, as authored - NOT an element
 * index: feed it through ll_mov_frame_ref). */
int ll_player_frame(const ll_player *p);

/* What a .MOV frame number means (PR-49; the game's own frame-line parser,
 * FUN_00051660, machine code 0x51692..0x516d5): 1-based, the sign is the
 * mirror flag. n > 0: element n-1; n < 0: element -n-1 drawn MIRRORED
 * (frame record +0x14 = 1); n == 0: no element index - the game pulls a
 * placeholder from a global resource (FUN_00044110), which this port does
 * not model, so it draws nothing (blank = 1). Before PR-49 the number was
 * used as a 0-based index and out-of-range numbers were clamped to the last
 * element - 4266 of 18432 authored frame numbers are out of range under
 * that reading, 0 are (but the real blanks) under the game's. */
typedef struct { int idx; int mirror; int blank; } ll_frame_ref;
ll_frame_ref ll_mov_frame_ref(int n);

/* Data-free selftest for CI (ll_level --play-selftest): synthetic .plt +
 * a mini .MOV using the REAL walk constants (F=400, T=90, G=200): walks,
 * hits the T cap, falls off an edge, lands, turns around, stops.
 * 0 = ok, 1 = failed (reasons to stderr). */
int ll_play_selftest(void);

#ifdef __cplusplus
}
#endif
#endif /* LL_PLAY_H */
