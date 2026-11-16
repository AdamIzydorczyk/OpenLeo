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
 *                 K_UP/K_DOWN/K_GEN1..3/K_ANY/AC_BREAK_NEXT, "!" negates,
 *                 "|" ORs; the priority token may be non-numeric - real
 *                 KADMAR2 writes "A_MASZERUJ(x)=K_FWD" - and a transition
 *                 targeting its OWN state means KEEP running, no frame
 *                 reset and no event) and per-state "seq=<stem>" art
 *                 selection. Condition names the data uses but this
 *                 runtime does not model (K_FIRE, K_JUMP, K_ALT,
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
 * `BY_TY` is the data's own typo for BY_TIME (12 scripts). Names without a
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
    LL_CTRL_CHECK_LEW       = 0x8000u
};
#define LL_CTRL_NAMES \
    X(BY_TIME) X(BY_X) X(BY_Y) X(BY_TY) X(CHECK_LN) X(CHECK_LEW) \
    X(CHECK_ALL) X(CHECK_ALL_PLANE) X(CHECK_ANI) X(CHECK_ZW) X(CHECK_BASE_LN) \
    X(CH_DIR) X(DANGER) X(SOLID) X(SOLID2) X(SEMI_SOLID) X(ANTI_SOLID) \
    X(ANTI_POCISK) X(FRAME_REV) X(SEQ_REV_X) X(SEQ_REV_Y) X(INV_V)

/* transition condition ids (K_ANY = any held key; OTHER = recognized
 * token we do not simulate - never true, logged once by the caller).
 * The authored expressions also use ',' groups (466 of 2911), a leading
 * '|' (49) and ';' inside a group (3); those parse as one OTHER token
 * and are counted, not guessed into AND or OR. */
enum {
    LL_COND_NONE = 0,
    LL_COND_K_FWD, LL_COND_K_BKWD, LL_COND_K_UP, LL_COND_K_DOWN,
    LL_COND_K_GEN1, LL_COND_K_GEN2, LL_COND_K_GEN3,
    LL_COND_K_ANY, LL_COND_AC_BREAK_NEXT,
    LL_COND_K_GEN4, LL_COND_AC_TIME,   /* PR-69: scene objects (not the lion) */
    LL_COND_K_JUMP,                    /* PR-74: the jump key               */
    LL_COND_OTHER
};

typedef struct { int frame; unsigned ms; int dx, dy; unsigned gen;
                 int has_i, ix, iy; } ll_mov_fr;   /* has_i: `I ix,iy` impulse tail */   /* gen: bit n-1 = the frame's `GEN K_GENn` tail */
typedef struct {
    char target[24];                        /* resolved at parse time      */
    int tidx;                               /* state index, -1 unresolved  */
    struct { int cond; int neg; } c[LMOV_MAXCON];
    int nc;
} ll_mov_tr;
typedef struct {
    char name[24];
    char seqkey[16];                        /* per-state seq= art select   */
    int fx, fy, tx, ty, gx, gy;             /* F= T= G= (px/s^2, px/s)    */
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
typedef struct { int k_fwd, k_bkwd, k_up, k_down, k_jump; } ll_play_keys;

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
    double x, y;                /* feet position, world px (y down)       */
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
    int gen[3];                 /* K_GEN1..3 one-tick pulses (script
                                 * triggers, set by gameplay logic)       */
    int broke;                  /* AC_BREAK_NEXT pending (frame list just
                                 * completed)                             */
    double air_from;            /* feet y when ground was lost            */
    int jumping;                /* PR-74: an impulse put the lion in the air */
    int support;                /* a solid object holds the feet up (set by
                                 * the collision pass, PR-59)              */
} ll_player;

enum {                                              /* ll_player_step ev */
    LL_PLAY_EV_STATE  = 1,                          /* state transition   */
    LL_PLAY_EV_GROUND = 2,                          /* landed             */
    LL_PLAY_EV_AIR    = 4                           /* stepped/swept off  */
};

/* Start (or respawn): feet at (x, y); caller resolves the initial
 * ground (ll_plt_first_below) and passes grounded accordingly. Initial
 * state: A_STAY when the .MOV defines it, else state 0. */
void ll_player_start(ll_player *p, const ll_mov_t *mov, double x, double y,
                     int grounded);

/* One fixed dt_ms tick (CI grid: 20 ms). xmin/xmax clamp feet x
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
