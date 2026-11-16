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
 *                 RAN.PLT 153 lines, TUNGA.11: TUNGA.PLT 7 lines).
 *   LEW.MOV       plain text (HD ANIMACJE player leaf, 8666 B in RAN.01):
 *                 the lion's real state machine - 30 states; each block
 *                 adds F=/T=/G= physics (acceleration px/s^2, speed cap
 *                 px/s, gravity px/s^2 - walk states F=400 T=90, grounded
 *                 states G=0 + CHECK_LN, the real fall state A_START_LAD1
 *                 G=0,200), "TARGET(p)=COND" transitions (K_FWD/K_BKWD/
 *                 K_UP/K_DOWN/K_GEN1..3/K_ANY/AC_BREAK_NEXT, "!" negates,
 *                 "|" ORs) and per-state "seq=<stem>" art selection.
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

/* text: first non-empty line = polyline count, rest = "n x,y;x,y;..." .
 * 0 = parsed (garbage lines are skipped), 1 = no usable line. */
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
 * semantics (frame number indexes the .SEQ element list directly,
 * non-positive ms clamped to 100 - the M4-C rule), plus the physics /
 * transition grammar above. Caps sized for the real LEW.MOV (29 states,
 * <= 60 frames). */
#define LMOV_MAXST 64
#define LMOV_MAXFR 128
#define LMOV_MAXTR 6
#define LMOV_MAXCON 4                       /* OR parts per transition     */

/* control= flags we act on; unknown names (FRAME_REV, BY_TIME details)
 * parse but change nothing yet (documented, PLAN 8f). */
enum {
    LL_CTRL_BY_TIME         = 1u,
    LL_CTRL_BY_X            = 2u,           /* walk states: move at speed  */
    LL_CTRL_CHECK_LN        = 4u,           /* keep-ground / fall-off rule */
    LL_CTRL_CH_DIR          = 8u,
    LL_CTRL_CHECK_ALL       = 0x10u,
    LL_CTRL_CHECK_ALL_PLANE = 0x20u,
    LL_CTRL_FRAME_REV       = 0x40u
};

/* transition condition ids (K_ANY = any held key; OTHER = recognized
 * token we do not simulate - never true, logged once by the caller). */
enum {
    LL_COND_NONE = 0,
    LL_COND_K_FWD, LL_COND_K_BKWD, LL_COND_K_UP, LL_COND_K_DOWN,
    LL_COND_K_GEN1, LL_COND_K_GEN2, LL_COND_K_GEN3,
    LL_COND_K_ANY, LL_COND_AC_BREAK_NEXT, LL_COND_OTHER
};

typedef struct { int frame; unsigned ms; int dx, dy; } ll_mov_fr;
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
} ll_mov_st;
typedef struct {
    ll_mov_st st[LMOV_MAXST];
    int nst;
    char seqkey[16];                        /* last seq= seen (viewer uses
                                             * this, matching old mov_t)   */
} ll_mov_t;

int ll_mov_parse(const char *text, ll_mov_t *m);   /* returns m->nst      */
int ll_mov_find(const ll_mov_t *m, const char *name); /* state idx or -1  */

/* ------------------------------------------------------- player sim ---- */
/* Held-key view of the input (platform.h translates the arrows; the CI
 * drives the same struct from LEO_PLAY_SCRIPT). */
typedef struct { int k_fwd, k_bkwd, k_up, k_down; } ll_play_keys;

/* Project constant, see header note; used only when airborne with a
 * CHECK_LN state that carries no authored fall route. */
#define LL_PLAY_GRAVITY 200.0
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
    int gen[3];                 /* K_GEN1..3 one-tick pulses (script
                                 * triggers, set by gameplay logic)       */
    int broke;                  /* AC_BREAK_NEXT pending (frame list just
                                 * completed)                             */
    double air_from;            /* feet y when ground was lost            */
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

/* Current .SEQ element index for drawing (frame number, unclamped -
 * caller clamps against its .SEQ, same convention as obj_elem). */
int ll_player_frame(const ll_player *p);

/* Data-free selftest for CI (ll_level --play-selftest): synthetic .plt +
 * a mini .MOV using the REAL walk constants (F=400, T=90, G=200): walks,
 * hits the T cap, falls off an edge, lands, turns around, stops.
 * 0 = ok, 1 = failed (reasons to stderr). */
int ll_play_selftest(void);

#ifdef __cplusplus
}
#endif
#endif /* LL_PLAY_H */
