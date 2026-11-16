/* ll_play.c - playable-level core (M4-B, PR-43). See ll_play.h for the
 * data provenance and the documented simplifications.
 *
 * Format facts this code is built on (all verified on real payloads,
 * 2026-11-16):
 *   RAN.PLT (HD ETAPY RAN.01, 4935 B): "153" count line, then
 *     "1 126,314;302,314;" one-segment shelves and multi-vertex terrain
 *     lines, world coords (y down). TUNGA.PLT likewise 7 lines.
 *   LEW.MOV (HD ANIMACJE RAN.01/LEW.RAN, 8666 B): state preamble list,
 *     then [STATE] blocks: control= flags (BY_TIME/BY_X/CHECK_LN/CH_DIR/
 *     CHECK_ALL/CHECK_ALL_PLANE/FRAME_REV), F=/T=/G= numbers, per-state
 *     seq= art, transitions like "A_STAY(1)=!K_FWD", frames "n,ms[, M
 *     dx,dy]". Walk: F=400 T=90; grounded G=0 + CHECK_LN; real fall
 *     state A_START_LAD1 G=0,200 with A_LADOW(1)=K_GEN2 on landing.
 */
#include "ll_play.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

/* ------------------------------------------------------------- .plt ---- */

void ll_plt_free(ll_plt *o)
{
    for (int k = 0; k < o->n; k++)
        free(o->line[k].p);
    free(o->line);
    o->line = NULL;
    o->n = 0;
}

int ll_plt_parse(const char *text, ll_plt *out)
{
    out->line = NULL;
    out->n = 0;
    const char *p = text;
    int header_skipped = 0;
    while (*p) {
        const char *eol = p;
        while (*eol && *eol != '\n' && *eol != '\r')
            eol++;
        /* the first non-empty line is the polyline count header (real
         * files lead with "153"/"7"); everything after it is geometry */
        if (!header_skipped && eol > p) {
            int probe = 0;
            if (sscanf(p, "%d", &probe) == 1 && probe >= 0) {
                header_skipped = 1;
                p = eol;
                while (*p == '\n' || *p == '\r')
                    p++;
                continue;
            }
            header_skipped = 1;         /* malformed: don't eat lines forever */
        }
        if (eol > p) {
            /* "n x,y;x,y;..." - n counts SEGMENTS (points = n+1); we scan
             * every x,y pair and ignore n, so a wrong count can't cut
             * usable geometry (same lesson as the PR-36 sparse tables). */
            ll_xy pts[LL_PLT_MAXPT];
            int npts = 0;
            const char *q = p;
            char *end = NULL;
            strtoll(q, &end, 10);               /* skip the segment count   */
            q = end;
            while (q < eol && npts < LL_PLT_MAXPT) {
                long a = strtol(q, &end, 10);
                if (end == q || *end != ',')
                    break;
                long b = strtol(end + 1, &end, 10);
                if (end == q || b > 0x7fffffff || a > 0x7fffffff)
                    break;
                pts[npts].x = (int)a;
                pts[npts].y = (int)b;
                npts++;
                q = end;
                /* point terminators: ';' is the normal one, '!' appears in
                 * real files as an equivalent separator inside the SAME
                 * polyline (ENA.PLT: "1 1,560!7500,570;", ZARIA_A.PLT has
                 * 10 of them) - the segment count prefix counts SEGMENTS
                 * either way, so it is the same separator with a different
                 * keystroke; treated identically. */
                while (q < eol && (*q == ';' || *q == '!' || *q == ',' ||
                                   *q == ' '))
                    q++;
            }
            if (npts >= 2) {
                ll_poly *nl = realloc(out->line,
                                      sizeof(ll_poly) * (size_t)(out->n + 1));
                if (nl) {
                    /* commit the grown array BEFORE allocating points: if
                     * the point malloc fails the (empty) array stays
                     * reachable through out->line for ll_plt_free */
                    out->line = nl;
                    ll_poly *pl = &nl[out->n];
                    pl->p = malloc(sizeof(ll_xy) * (size_t)npts);
                    pl->n = 0;
                    if (pl->p) {
                        memcpy(pl->p, pts, sizeof(ll_xy) * (size_t)npts);
                        pl->n = npts;
                        out->n++;
                    }
                }
            }
        }
        p = eol;
        while (*p == '\n' || *p == '\r')
            p++;
    }
    return out->n > 0 ? 0 : 1;
}

/* interpolate y of segment k at x (integer math, deterministic: the real
 * .plt coordinates are all small ints, products fit in long long) */
static int seg_y_at(const ll_poly *pl, int k, int x)
{
    int x1 = pl->p[k].x, x2 = pl->p[k + 1].x;
    if (x2 == x1)
        return pl->p[k].y < pl->p[k + 1].y ? pl->p[k].y : pl->p[k + 1].y;
    long long dy = (long long)pl->p[k + 1].y - pl->p[k].y;
    return pl->p[k].y + (int)(dy * (x - x1) / (x2 - x1));
}

/* does any segment cover x? (inclusive endpoints) */
static int seg_cover(const ll_poly *pl, int k, int x)
{
    int lo = pl->p[k].x < pl->p[k + 1].x ? pl->p[k].x : pl->p[k + 1].x;
    int hi = pl->p[k].x < pl->p[k + 1].x ? pl->p[k + 1].x : pl->p[k].x;
    return x >= lo && x <= hi;
}

int ll_plt_surface_at(const ll_plt *o, int x, int want_y, int tol, ll_xy *hit)
{
    int found = 0;
    int best = 0x7fffffff;
    for (int li = 0; li < o->n; li++) {
        const ll_poly *pl = &o->line[li];
        for (int k = 0; k + 1 < pl->n; k++) {
            if (!seg_cover(pl, k, x))
                continue;
            int y = seg_y_at(pl, k, x);
            int d = y > want_y ? y - want_y : want_y - y;
            if (d <= tol && d < best) {
                best = d;
                found = 1;
                if (hit) {
                    hit->x = x;
                    hit->y = y;
                }
            }
        }
    }
    return found;
}

int ll_plt_first_below(const ll_plt *o, int x, int from_y, ll_xy *hit)
{
    int found = 0;
    int best = 0x7fffffff;
    for (int li = 0; li < o->n; li++) {
        const ll_poly *pl = &o->line[li];
        for (int k = 0; k + 1 < pl->n; k++) {
            if (!seg_cover(pl, k, x))
                continue;
            int y = seg_y_at(pl, k, x);
            if (y >= from_y && y < best) {
                best = y;
                found = 1;
                if (hit) {
                    hit->x = x;
                    hit->y = y;
                }
            }
        }
    }
    return found;
}

/* ------------------------------------------------------- .MOV parse ---- */

int ll_mov_find(const ll_mov_t *m, const char *name)
{
    for (int k = 0; k < m->nst; k++)
        if (strcasecmp(m->st[k].name, name) == 0)
            return k;
    return -1;
}

static int cond_id(const char *tok)
{
    static const struct { const char *n; int id; } tbl[] = {
        { "K_FWD", LL_COND_K_FWD },
        { "K_BKWD", LL_COND_K_BKWD },
        { "K_UP", LL_COND_K_UP },
        { "K_DOWN", LL_COND_K_DOWN },
        { "K_GEN1", LL_COND_K_GEN1 },
        { "K_GEN2", LL_COND_K_GEN2 },
        { "K_GEN3", LL_COND_K_GEN3 },
        { "K_ANY", LL_COND_K_ANY },
        { "AC_BREAK_NEXT", LL_COND_AC_BREAK_NEXT },
    };
    for (unsigned k = 0; k < sizeof tbl / sizeof tbl[0]; k++)
        if (strcasecmp(tbl[k].n, tok) == 0)
            return tbl[k].id;
    return LL_COND_OTHER;
}

static void ctrl_flag(const char *tok, unsigned *f)
{
    static const struct { const char *n; unsigned bit; } tbl[] = {
        { "BY_TIME", LL_CTRL_BY_TIME },
        { "BY_X", LL_CTRL_BY_X },
        { "CHECK_LN", LL_CTRL_CHECK_LN },
        { "CH_DIR", LL_CTRL_CH_DIR },
        { "CHECK_ALL_PLANE", LL_CTRL_CHECK_ALL_PLANE },  /* before CHECK_ALL */
        { "CHECK_ALL", LL_CTRL_CHECK_ALL },
        { "FRAME_REV", LL_CTRL_FRAME_REV },
    };
    for (unsigned k = 0; k < sizeof tbl / sizeof tbl[0]; k++)
        if (strcasecmp(tbl[k].n, tok) == 0) {
            *f |= tbl[k].bit;
            return;
        }
    /* unknown flag names (LOCK, ldir, ...) parse but do nothing (PLAN 8f) */
}

/* "a,b" -> two ints; scalar form ("G=400", seen in real data) duplicates
 * the value - the only scalar seen is G=400 in a scripted balloon state */
static void parse_pair(const char *v, int *a, int *b)
{
    char *end = NULL;
    *a = (int)strtol(v, &end, 10);
    *b = *a;
    if (end && *end == ',')
        *b = (int)strtol(end + 1, NULL, 10);
}

/* "A|!B|C" -> OR parts into tr->c (exact-match tokens, '|' separated;
 * an empty part from a leading '|' is skipped - seen as "A(66)=|K_ANY") */
static void parse_cond(ll_mov_tr *tr, const char *expr)
{
    const char *q = expr;
    while (*q && tr->nc < LMOV_MAXCON) {
        const char *bar = strchr(q, '|');
        const char *end = bar ? bar : q + strlen(q);
        while (q < end && *q == ' ')
            q++;
        int neg = 0;
        if (q < end && *q == '!') {
            neg = 1;
            q++;
        }
        size_t n = 0;
        char tok[24];
        while (q + n < end && n + 1 < sizeof tok) {
            tok[n] = q[n];
            n++;
        }
        tok[n] = '\0';
        if (n) {
            tr->c[tr->nc].cond = cond_id(tok);
            tr->c[tr->nc].neg = neg;
            tr->nc++;
        }
        q = bar ? bar + 1 : end;
    }
}

int ll_mov_parse(const char *text, ll_mov_t *m)
{
    memset(m, 0, sizeof *m);
    const char *p = text;
    while (*p) {
        const char *eol = p;
        while (*eol && *eol != '\n' && *eol != '\r')
            eol++;
        size_t len = (size_t)(eol - p);
        char line[160];
        if (len && len < sizeof line) {
            memcpy(line, p, len);
            line[len] = '\0';
            if (line[0] == '[') {                     /* [STATE] opens block */
                if (m->nst < LMOV_MAXST) {
                    ll_mov_st *s = &m->st[m->nst++];
                    const char *gt = strchr(line, ']');
                    size_t nl = gt ? (size_t)(gt - line - 1) : 0;
                    if (nl >= sizeof s->name)
                        nl = sizeof s->name - 1;
                    memcpy(s->name, line + 1, nl);
                    s->name[nl] = '\0';
                }
            } else if (m->nst > 0) {
                ll_mov_st *s = &m->st[m->nst - 1];
                int f, ms;
                if (sscanf(line, "%d,%d", &f, &ms) == 2) { /* frame line   */
                    if (s->nfr < LMOV_MAXFR) {
                        ll_mov_fr *mf = &s->fr[s->nfr++];
                        mf->frame = f;
                        /* M4-C (FIX-13): non-positive durations would make
                         * obj_advance / the player spin forever: clamp.
                         * Positive author values (walk frames say 6 ms ->
                         * a 72 ms 12-frame cycle) are kept as-is, exactly
                         * matching the viewer's mov_parse. */
                        mf->ms = ms > 0 ? (unsigned)ms : 100u;
                        mf->dx = mf->dy = 0;
                        const char *mvt = strstr(line, "M ");
                        if (mvt)
                            sscanf(mvt, "M %d,%d", &mf->dx, &mf->dy);
                    }
                } else if (strncmp(line, "seq=", 4) == 0) {
                    const char *v = line + 4;
                    while (*v == ' ')
                        v++;
                    size_t k = 0;
                    while (v[k] && k + 1 < sizeof s->seqkey) {
                        s->seqkey[k] = v[k];
                        k++;
                    }
                    s->seqkey[k] = '\0';
                    if (strlen(m->seqkey) < sizeof m->seqkey - 1)
                        strcpy(m->seqkey, s->seqkey);   /* viewer compat */
                } else if (strncmp(line, "control=", 8) == 0) {
                    char tmp[128];
                    snprintf(tmp, sizeof tmp, "%.*s", (int)sizeof tmp - 1,
                             line + 8);
                    char *save = NULL;
                    for (char *tok = strtok_r(tmp, "| \t", &save); tok;
                         tok = strtok_r(NULL, "| \t", &save))
                        ctrl_flag(tok, &s->ctrl);
                } else if (line[0] == 'F' && line[1] == '=' &&
                           strlen(line) > 2) {
                    parse_pair(line + 2, &s->fx, &s->fy);
                } else if (line[0] == 'T' && line[1] == '=') {
                    parse_pair(line + 2, &s->tx, &s->ty);
                } else if (line[0] == 'G' && line[1] == '=') {
                    parse_pair(line + 2, &s->gx, &s->gy);
                } else if (line[0] != ';' && strncmp(line, "time=", 5) != 0 &&
                           strchr(line, '=') && s->ntr < LMOV_MAXTR) {
                    /* transition: TARGET[(p)]=COND */
                    ll_mov_tr *tr = &s->tr[s->ntr++];
                    tr->tidx = -1;
                    const char *eq = strchr(line, '=');
                    const char *par = strchr(line, '(');
                    size_t n = 0;
                    const char *te = par && par < eq ? par : eq;
                    for (const char *q = line; q < te && n + 1 < sizeof tr->target;
                         q++)
                        tr->target[n++] = *q;
                    tr->target[n] = '\0';
                    parse_cond(tr, eq + 1);
                }
                /* time=, POS, LOCK, unlock, ldir and other payload keys
                 * parse but do nothing yet (PLAN 8f); the state preamble
                 * before the first '[' is ignored, matching mov_parse */
            }
        }
        p = eol;
        while (*p == '\n' || *p == '\r')
            p++;
    }
    for (int k = 0; k < m->nst; k++) {
        ll_mov_st *s = &m->st[k];
        for (int t = 0; t < s->ntr; t++)
            s->tr[t].tidx = ll_mov_find(m, s->tr[t].target);
    }
    return m->nst;
}

/* ------------------------------------------------------ player sim ---- */

void ll_player_start(ll_player *p, const ll_mov_t *mov, double x, double y,
                     int grounded)
{
    memset(p, 0, sizeof *p);
    p->mov = mov;
    p->x = x;
    p->y = y;
    p->grounded = grounded;
    p->walkdir = 1;
    int stay = ll_mov_find(mov, "A_STAY");
    p->sti = stay >= 0 ? stay : 0;
}

static int cond_true(const ll_player *p, const ll_play_keys *k,
                     const ll_mov_tr *tr)
{
    int any = k->k_fwd || k->k_bkwd || k->k_up || k->k_down;
    for (int i = 0; i < tr->nc; i++) {
        int v;
        switch (tr->c[i].cond) {
        case LL_COND_K_FWD:  v = k->k_fwd;  break;
        case LL_COND_K_BKWD: v = k->k_bkwd; break;
        case LL_COND_K_UP:   v = k->k_up;   break;
        case LL_COND_K_DOWN: v = k->k_down; break;
        case LL_COND_K_GEN1: v = p->gen[0]; break;
        case LL_COND_K_GEN2: v = p->gen[1]; break;
        case LL_COND_K_GEN3: v = p->gen[2]; break;
        case LL_COND_K_ANY:  v = any;       break;
        case LL_COND_AC_BREAK_NEXT: v = p->broke; break;
        default:             v = 0;         break;   /* OTHER: not simulated */
        }
        if (tr->c[i].neg)
            v = !v;
        if (v)
            return 1;                                    /* OR semantics */
    }
    return 0;
}

int ll_player_step(ll_player *p, const ll_plt *ground,
                   const ll_play_keys *keys, double xmin, double xmax,
                   unsigned dt_ms)
{
    if (!p->mov || p->sti >= p->mov->nst || !dt_ms)
        return 0;
    const ll_mov_st *st = &p->mov->st[p->sti];
    int ev = 0;
    double dt = (double)dt_ms / 1000.0;

    /* 1) transitions, in the order the real .MOV lists them (first match
     * wins; K_FWD/K_BKWD carry the facing change the CH_DIR flag names) */
    for (int t = 0; t < st->ntr; t++) {
        if (!cond_true(p, keys, &st->tr[t]))
            continue;
        for (int i = 0; i < st->tr[t].nc; i++) {
            if (st->tr[t].c[i].neg)
                continue;
            if (st->tr[t].c[i].cond == LL_COND_K_FWD)
                p->walkdir = 1;
            else if (st->tr[t].c[i].cond == LL_COND_K_BKWD)
                p->walkdir = -1;
        }
        /* A transition that targets the state it is written in is REAL
         * data (KADMAR2's marching state: "A_MASZERUJ(x)=K_FWD") and
         * means KEEP this state: re-entering it would restart the frame
         * list every tick (measured: the walk froze on frame 1 while the
         * lion slid along at the state's speed). So a self-target leaves
         * the state running untouched - only the facing update above
         * applied. (Note the priority field takes non-numeric tokens
         * too: "(x)".) */
        if (st->tr[t].tidx >= 0 && st->tr[t].tidx != p->sti) {
            p->sti = st->tr[t].tidx;
            p->fi = 0;
            p->ft = 0;
            ev |= LL_PLAY_EV_STATE;
        }
        break;
    }
    st = &p->mov->st[p->sti];
    for (int i = 0; i < 3; i++)                        /* one-tick pulses */
        p->gen[i] = 0;
    p->broke = 0;

    /* 2) horizontal physics from the real F=/T= numbers: approach the
     * state's target velocity (BY_X states walk at +/-T; everything else
     * brakes toward 0 at its own F - A_STAY F=600 stops hard, matching
     * the real data) */
    double tv = (st->ctrl & LL_CTRL_BY_X)
                    ? p->walkdir * (double)(st->tx > 0 ? st->tx : 0)
                    : 0.0;
    double acc = (st->fx >= 0 ? (double)st->fx : -(double)st->fx) * dt;
    double dv = tv - p->vx;
    if (dv > acc)
        dv = acc;
    if (dv < -acc)
        dv = -acc;
    p->vx += dv;
    if (st->tx > 0) {                                  /* the real T= cap  */
        double cap = (double)st->tx;
        if (p->vx > cap)
            p->vx = cap;
        if (p->vx < -cap)
            p->vx = -cap;
    }

    /* 3) vertical: authored G= wins; the documented project fallback
     * LL_PLAY_GRAVITY applies only while airborne in a CHECK_LN state
     * that has no authored fall route (stepped off a .plt edge). */
    if (!p->grounded) {
        double g = (double)st->gy;
        if (g == 0.0 && (st->ctrl & LL_CTRL_CHECK_LN))
            g = LL_PLAY_GRAVITY;
        p->vy += g * dt;
    }

    /* 4) integrate (double accumulators; the DRAW position is (int) in
     * ll_level.c, integer steps are deterministic on the fixed grid) */
    p->x += p->vx * dt;
    p->y += p->vy * dt;

    /* 5) ground logic (.plt lines, feet-on-top) */
    int xi = (int)(p->x >= 0 ? p->x + 0.5 : p->x - 0.5);
    if (ground) {
        if (p->grounded && (st->ctrl & LL_CTRL_CHECK_LN)) {
            ll_xy hit;
            int yi = (int)(p->y >= 0 ? p->y + 0.5 : p->y - 0.5);
            if (ll_plt_surface_at(ground, xi, yi, LL_PLAY_STEP, &hit)) {
                p->y = (double)hit.y;                    /* slope following */
                p->vy = 0;
            } else {
                p->grounded = 0;
                p->air_from = p->y;
                ev |= LL_PLAY_EV_AIR;
            }
        } else if (!p->grounded) {
            /* Landing search. The start row sits LL_PLAY_STEP ABOVE the
             * feet (y-down: minus = up): a CHECK_LN walker that briefly
             * left the ground at a line END and sank a few px below the
             * continuation line (real RAN.PLT: shelf 126..302@314 ->
             * 323..485@313 across a 20 px gap) steps back UP onto it -
             * symmetric with the step-down tolerance in the grounded
             * branch. A fall from higher up is unaffected: its surface
             * is below the feet either way. */
            ll_xy hit;
            int yi = (int)(p->y >= 0 ? p->y : p->y - 1) - LL_PLAY_STEP;
            if (ll_plt_first_below(ground, xi, yi, &hit) &&
                p->y >= (double)hit.y - 1.0) {
                p->y = (double)hit.y;
                p->vy = 0;
                p->grounded = 1;
                p->gen[LL_COND_K_GEN2 - LL_COND_K_GEN1] = 1; /* landing   */
                ev |= LL_PLAY_EV_GROUND;
            }
        }
    }

    /* 6) lew_rect bounds */
    if (p->x < xmin) {
        p->x = xmin;
        p->vx = 0;
    } else if (p->x > xmax) {
        p->x = xmax;
        p->vx = 0;
    }

    /* 7) frame advance on the same ms clock (identical semantics to
     * obj_advance: wrap at the end, apply M dx,dy deltas; AC_BREAK_NEXT
     * is armed by a wrap and consumed by transitions next tick) */
    if (st->nfr > 0) {
        p->ft += dt_ms;
        unsigned guard = 0;
        while (p->ft >= st->fr[p->fi].ms && guard++ < 4u * (unsigned)st->nfr) {
            p->ft -= st->fr[p->fi].ms;
            p->x += st->fr[p->fi].dx;
            p->y += st->fr[p->fi].dy;
            if (p->fi + 1 >= st->nfr) {
                p->fi = 0;
                p->broke = 1;
            } else {
                p->fi++;
            }
        }
        if (p->ft >= st->fr[p->fi].ms)                /* guard hit (data */
            p->ft = 0;                                /* safety): resync  */
    }
    return ev;
}

int ll_player_frame(const ll_player *p)
{
    if (!p->mov || p->sti >= p->mov->nst)
        return 0;
    const ll_mov_st *s = &p->mov->st[p->sti];
    if (!s->nfr)
        return 0;
    if (p->fi >= s->nfr)
        return s->fr[s->nfr - 1].frame;
    return s->fr[p->fi].frame;
}

/* ---------------------------------------------------------- selftest --- */

/* Mini .MOV with the REAL RAN.01 walk constants (values copied from
 * LEW.MOV verbatim: F=400 cap T=90, STAY brake F=600, frame durations
 * clamped by M4-C) and the real transition grammar. */
static const char ST_MOV[] =
    "A_STAY\r\nA_WALK_FWD\r\nA_WALK_BKWD\r\n\r\n"
    "[A_STAY]\r\ncontrol=FRAME_REV|BY_TIME|CHECK_ALL|CHECK_LN\r\n"
    "F=600,0\r\nT=90,90\r\nG=0,0\r\n"
    "A_WALK_BKWD(1)=K_BKWD\r\nA_WALK_FWD(1)=K_FWD\r\nseq=uj09\r\n1,6\r\n\r\n"
    "[A_WALK_FWD]\r\ncontrol=FRAME_REV|BY_X|CHECK_ALL|CH_DIR|CHECK_LN\r\n"
    "F=400,0\r\nT=90,90\r\nG=0,0\r\nA_STAY(1)=!K_FWD\r\n"
    "A_WALK_FWD(x)=K_FWD\r\nseq=uj09\r\n"
    "1,6\r\n2,6\r\n\r\n"
    "[A_WALK_BKWD]\r\ncontrol=FRAME_REV|BY_X|CHECK_ALL|CH_DIR|CHECK_LN\r\n"
    "F=400,0\r\nT=90,90\r\nG=0,0\r\nA_STAY(1)=!K_BKWD\r\nseq=uj09\r\n"
    "1,2\r\n2,2\r\n";

/* two shelves: walk from x=500 to the edge at x=701, fall ~50 px onto the
 * lower shelf (landing ~x=765, well inside it), stop, short reverse
 * (geometry invented; the line format is copied from RAN.PLT) */
static const char ST_PLT[] =
    "2\r\n1 0,100;700,100;\r\n1 651,150;3000,150;\r\n";

#define CHECK(cond)                                                    \
    do {                                                               \
        if (!(cond)) {                                                 \
            fprintf(stderr, "ll_play selftest FAILED (line %d): %s\n", \
                    __LINE__, #cond);                                  \
            rc = 1;                                                    \
        }                                                              \
    } while (0)

int ll_play_selftest(void)
{
    int rc = 0;
    ll_plt plt;
    ll_mov_t m;
    ll_xy hit;

    /* .plt grammar: header, segment-count prefix, interpolation, queries */
    CHECK(ll_plt_parse(ST_PLT, &plt) == 0 && plt.n == 2);
    CHECK(ll_plt_surface_at(&plt, 250, 100, LL_PLAY_STEP, &hit) &&
          hit.y == 100);
    CHECK(!ll_plt_surface_at(&plt, 760, 100, LL_PLAY_STEP, &hit));
    CHECK(ll_plt_first_below(&plt, 760, 120, &hit) && hit.y == 150);
    CHECK(ll_plt_first_below(&plt, 250, 40, &hit) && hit.y == 100);
    ll_plt_free(&plt);

    /* '!' inside one polyline = the same separator (real ENA.PLT writes
     * "1 1,560!7500,570;", ZARIA_A.PLT uses it 10 times): without it the
     * ENA.03 ground parsed to 0 lines and the level looked data-less. */
    ll_plt plt2;
    CHECK(ll_plt_parse("1\r\n1 0,100!700,100;\r\n", &plt2) == 0 &&
          plt2.n == 1 && plt2.line[0].n == 2 &&
          plt2.line[0].p[1].x == 700 && plt2.line[0].p[1].y == 100);
    ll_plt_free(&plt2);

    /* .MOV extended grammar: physics numbers, flags, transitions, per-state
     * seq, M4-C clamp (frames say 6 ms -> 100) */
    CHECK(ll_mov_parse(ST_MOV, &m) == 3);
    CHECK(ll_mov_find(&m, "A_WALK_FWD") == 1);
    CHECK(m.st[1].fx == 400 && m.st[1].tx == 90 &&
          (m.st[1].ctrl & LL_CTRL_BY_X) && (m.st[1].ctrl & LL_CTRL_CHECK_LN));
    CHECK(strcmp(m.st[1].seqkey, "uj09") == 0);
    CHECK(m.st[1].ntr == 2 && m.st[1].tr[0].tidx == 0 &&
          m.st[1].tr[0].c[0].cond == LL_COND_K_FWD && m.st[1].tr[0].c[0].neg == 1);
    /* the self-target line parses too (its priority is the non-numeric
     * token "x" - real KADMAR2 data), and matches its own state */
    CHECK(m.st[1].tr[1].tidx == 1 &&
          m.st[1].tr[1].c[0].cond == LL_COND_K_FWD &&
          m.st[1].tr[1].c[0].neg == 0);
    CHECK(m.st[1].fr[0].ms == 6);                    /* author values kept */
    ll_mov_t m2;
    ll_mov_parse("X\n[A]\n0,0\n1,-5\n", &m2);
    CHECK(m2.nst == 1 && m2.st[0].fr[0].ms == 100 &&
          m2.st[0].fr[1].ms == 100);                 /* M4-C clamp */
    CHECK(strcmp(m.seqkey, "uj09") == 0);

    /* runtime: STAY -> WALK_FWD, T=90 cap respected, edge fall, landing
     * fires the K_GEN2 pulse, walk then reverse and stop */
    ll_plt pl;
    CHECK(ll_plt_parse(ST_PLT, &pl) == 0);
    ll_player p;
    ll_player_start(&p, &m, 500, 100, 1);
    CHECK(p.grounded && strcmp(m.st[p.sti].name, "A_STAY") == 0);

    ll_play_keys k = { 0, 0, 0, 0 };
    k.k_fwd = 1;
    int ev = ll_player_step(&p, &pl, &k, 0, 2000, 20);
    CHECK((ev & LL_PLAY_EV_STATE) && p.sti == 1);
    int fell = 0, landed = 0, stev = 1, maxfi = 0;
    for (int t = 0; t < 400; t++) {                 /* walk until the pit */
        ev = ll_player_step(&p, &pl, &k, 0, 2000, 20);
        if (ev & LL_PLAY_EV_AIR)
            fell = t;
        if (ev & LL_PLAY_EV_STATE)
            stev++;
        if (p.fi > maxfi)
            maxfi = p.fi;
        if (ev & LL_PLAY_EV_GROUND) {
            landed = t;
            break;
        }
        CHECK(p.vx <= 90.0000001);
        if (p.grounded && t > 2)
            CHECK(p.y == 100);
    }
    CHECK(fell && landed > fell);
    /* KADMAR2's real "A_MASZERUJ(x)=K_FWD" pattern (a state that
     * re-targets ITSELF while its own key is held; here added to the
     * mini .MOV as A_WALK_FWD(x)=K_FWD): keeping the state must neither
     * restart its frame list nor report a transition - measured before
     * the fix: one A_MASZERUJ -> A_MASZERUJ event per tick, the walk
     * animation frozen on frame 1. */
    CHECK(stev == 1);
    CHECK(maxfi > 0);
    CHECK(p.grounded && p.y == 150);
    CHECK(p.vx > 50);                                /* kept momentum  */
    CHECK(p.gen[LL_COND_K_GEN2 - LL_COND_K_GEN1] == 1); /* landing pulse */

    k.k_fwd = 0;
    ev = ll_player_step(&p, &pl, &k, 0, 2000, 20);
    CHECK((ev & LL_PLAY_EV_STATE) && p.sti == 0);   /* !K_FWD -> STAY   */
    for (int t = 0; t < 60; t++)
        ll_player_step(&p, &pl, &k, 0, 2000, 20);
    CHECK(p.vx == 0);                               /* F=600 brake      */

    k.k_bkwd = 1;
    ev = ll_player_step(&p, &pl, &k, 0, 2000, 20);
    CHECK((ev & LL_PLAY_EV_STATE) && p.sti == 2 && p.walkdir == -1);
    double x0 = p.x;
    for (int t = 0; t < 20; t++)                    /* short reverse: stay
                                                     * on the lower shelf
                                                     * (edge sits at x=500,
                                                     * we land ~13 px past
                                                     * it) */
        ll_player_step(&p, &pl, &k, 0, 2000, 20);
    CHECK(p.x < x0 - 10 && p.y == 150 && p.grounded);
    for (int t = 0; t < 40; t++) {                  /* frame cursor valid */
        int fi = p.fi;
        CHECK(fi >= 0 && fi < m.st[p.sti].nfr);
        CHECK(ll_player_frame(&p) >= 0);
        ll_player_step(&p, &pl, &k, 0, 2000, 20);
    }
    ll_plt_free(&pl);
    if (!rc)
        printf("play selftest: .plt physics (+ the '!' separator variant),"
               " extended .MOV transitions walk/turn/stop, self-transition"
               " keeps its state (frames keep running), edge fall, K_GEN2"
               " landing pulse (ok)\n");
    return rc;
}
