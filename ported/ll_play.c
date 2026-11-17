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
    int declared = -1;            /* the header line's polyline count     */
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
                declared = probe;
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
    /* A file that DECLARES zero polylines is complete and valid, not
     * garbage: JUNCTION's platformy=none.des carries literally "0" -
     * that level simply has no floor. Reporting it as n==0 (rc 0) keeps
     * the difference between "authored empty" and "nothing usable here"
     * (declared<0, or a count with no geometry) visible to the caller. */
    return out->n > 0 || declared == 0 ? 0 : 1;
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
        { "K_GEN4", LL_COND_K_GEN4 },
        { "K_JUMP", LL_COND_K_JUMP },
        { "AC_TIME", LL_COND_AC_TIME },
        { "K_ANY", LL_COND_K_ANY },
        { "AC_BREAK_NEXT", LL_COND_AC_BREAK_NEXT },
    };
    for (unsigned k = 0; k < sizeof tbl / sizeof tbl[0]; k++)
        if (strcasecmp(tbl[k].n, tok) == 0)
            return tbl[k].id;
    return LL_COND_OTHER;
}

static void ctrl_flag(const char *tok, unsigned *f, int *other)
{
    static const struct { const char *n; unsigned bit; } tbl[] = {
        { "BY_TIME", LL_CTRL_BY_TIME },
        { "BY_X", LL_CTRL_BY_X },
        { "BY_Y", LL_CTRL_BY_Y },
        { "CHECK_LN", LL_CTRL_CHECK_LN },
        { "CH_DIR", LL_CTRL_CH_DIR },
        { "CHECK_ALL_PLANE", LL_CTRL_CHECK_ALL_PLANE },  /* before CHECK_ALL */
        { "CHECK_ALL", LL_CTRL_CHECK_ALL | LL_CTRL_CHECK_ANI | LL_CTRL_CHECK_ZW },
        { "CHECK_ANI", LL_CTRL_CHECK_ANI },
        { "CHECK_ZW", LL_CTRL_CHECK_ZW },
        { "CHECK_LEW", LL_CTRL_CHECK_LEW },
        { "FRAME_REV", LL_CTRL_FRAME_REV },
        { "SOLID", LL_CTRL_SOLID },
        { "SOLID2", LL_CTRL_SOLID | LL_CTRL_SOLID2 },
        { "SEMI_SOLID", LL_CTRL_SOLID | LL_CTRL_SEMI_SOLID },
        { "ANTI_SOLID", LL_CTRL_ANTI_SOLID },
        { "ANTI_POCISK", LL_CTRL_ANTI_POCISK },
    };
    for (unsigned k = 0; k < sizeof tbl / sizeof tbl[0]; k++)
        if (strcasecmp(tbl[k].n, tok) == 0) {
            *f |= tbl[k].bit;
            return;
        }
    /* Named in the ll_play.h vocabulary but without a bit (CHECK_LEW,
     * DANGER, ...), or a name we have never seen:
     * counted so a missing behaviour is a known gap, never a silent one. */
    if (other)
        (*other)++;
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
                } else {
                    m->drop_st++;
                }
            } else if (m->nst == 0) {
                /* before the first [STATE]: the declared state list; its
                 * first name is the starting state */
                if (!m->first_decl[0] && line[0] != ';' && line[0] != '/' &&
                    !strchr(line, '=')) {
                    size_t k = 0;
                    while (line[k] > ' ' && k < sizeof m->first_decl - 1) {
                        m->first_decl[k] = line[k];
                        k++;
                    }
                    m->first_decl[k] = '\0';
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
                        mf->gen = 0;
                        mf->has_i = 0;
                        /* A frame line may carry further fields. The one the
                         * model acts on is the authored motion of THIS frame,
                         * "N,ms, M dx,dy" (285 of 18617 shipped frame lines
                         * carry it; there are no standalone "M" lines).
                         * Everything else names game signals the scene model
                         * has no bus for yet - GEN <signal>, ATC_ON/ATC_OFF,
                         * WAIT <signal>, lock/unlock, smpauto/smp, GOTO_rel,
                         * inline F= - so it is counted (measured 1568 such
                         * lines, PLAN 8g), never dropped in silence. */
                        const char *fc = strchr(line, ',');
                        const char *rest = fc ? strchr(fc + 1, ',') : NULL;
                        if (rest) {
                            const char *q = rest + 1;
                            int other_directive = 0;
                            while (*q) {
                                while (*q == ' ' || *q == ',')
                                    q++;
                                if (!*q)
                                    break;
                                char word[16];
                                size_t wn = 0;
                                while (*q && *q != ',' && *q != ' ' &&
                                       wn + 1 < sizeof word)
                                    word[wn++] = *q++;
                                word[wn] = '\0';
                                if (!wn)
                                    continue;
                                if (wn == 1 && (word[0] == 'M' ||
                                                word[0] == 'm')) {
                                    while (*q == ' ')
                                        q++;
                                    sscanf(q, "%d,%d", &mf->dx, &mf->dy);
                                    continue;
                                }
                                int digits = 1;
                                for (size_t k = 0; k < wn; k++)
                                    if (word[k] < '0' || word[k] > '9')
                                        digits = 0;
                                if (digits)        /* tail of the M pair */
                                    continue;
                                if (wn == 1 && (word[0] == 'I' || word[0] == 'i')) {
                                    int ix, iy;
                                    while (*q == ' ')
                                        q++;
                                    if (sscanf(q, "%d,%d", &ix, &iy) == 2) {
                                        mf->has_i = 1;
                                        mf->ix = ix;
                                        mf->iy = iy;
                                        while (*q && *q != ' ' && *q != ',')
                                            q++;       /* tail of the I pair */
                                        continue;
                                    }
                                }
                                if (strcasecmp(word, "GEN") == 0) {
                                    /* PR-69: `GEN K_GENn` = the frame raises
                                     * that signal when it completes.
                                     * PR-83: `GEN AC_TIME` is the same
                                     * mechanism for AC_TIME (the real
                                     * ZARIA A_WALK idle chain: 11 such
                                     * lines across that script) */
                                    while (*q == ' ')
                                        q++;
                                    if (strncasecmp(q, "K_GEN", 5) == 0 &&
                                        q[5] >= '1' && q[5] <= '4') {
                                        mf->gen |= 1u << (q[5] - '1');
                                        q += 6;
                                        continue;
                                    }
                                    if (strncasecmp(q, "AC_TIME", 7) == 0) {
                                        mf->gen |= LMOV_GEN_AC_TIME;
                                        q += 7;
                                        continue;
                                    }
                                }
                                other_directive = 1;
                            }
                            if (other_directive)
                                s->directive++;
                        }
                    } else {
                        m->drop_fr++;
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
                    /* '|' is the separator (1389 control lines); ',' appears
                     * in 9 of them, so both split. */
                    for (char *tok = strtok_r(tmp, "|, \t", &save); tok;
                         tok = strtok_r(NULL, "|, \t", &save))
                        ctrl_flag(tok, &s->ctrl, &s->ctrl_other);
                } else if (line[0] == 'F' && line[1] == '=' &&
                           strlen(line) > 2) {
                    parse_pair(line + 2, &s->fx, &s->fy);
                } else if (line[0] == 'T' && line[1] == '=') {
                    parse_pair(line + 2, &s->tx, &s->ty);
                } else if (line[0] == 'G' && line[1] == '=') {
                    parse_pair(line + 2, &s->gx, &s->gy);
                } else if (line[0] != ';' && strchr(line, '(') &&
                           strchr(line, '=')) {
                    /* transition: TARGET[(p)]=COND (all 2911 shipped ones
                     * carry the parentheses) */
                    if (s->ntr < LMOV_MAXTR) {
                        ll_mov_tr *tr = &s->tr[s->ntr++];
                        tr->tidx = -1;
                        const char *eq = strchr(line, '=');
                        const char *par = strchr(line, '(');
                        size_t n = 0;
                        const char *te = par && par < eq ? par : eq;
                        for (const char *q = line;
                             q < te && n + 1 < sizeof tr->target; q++)
                            tr->target[n++] = *q;
                        tr->target[n] = '\0';
                        parse_cond(tr, eq + 1);
                    } else {
                        m->drop_tr++;
                    }
                } else if (strncmp(line, "time=", 5) == 0) {
                    /* PR-83: `time=<n>` (or an `n,m` range: seen as 32,64
                     * / 90,120 / 220 / 350) sits next to several AC_TIME
                     * transitions, but its UNIT is undetermined and acting
                     * on it changed real behavior wrongly (as ms: ENA.03's
                     * A_SWIM left state 2 ticks after spawn). So it is
                     * parsed for the record only, and stays counted: the
                     * player's proven AC_TIME source is the frame's
                     * `GEN AC_TIME` tail. OPEN QUESTION (recorded). */
                    int t1 = 0, t2 = 0;
                    parse_pair(line + 5, &t1, &t2);
                    if (t1 > 0)
                        s->time_ms = t1;
                    s->field_other++;
                } else if (line[0] != ';' && strchr(line, '=')) {
                    /* state payload fields the model does not act on yet:
                     * sample= (487, sound ids), time_scale= (115).
                     * Counted, never dropped in silence (PLAN 8g). */
                    s->field_other++;
                }
                /* the bare state names before the first '[' stay ignored,
                 * matching mov_parse; ';' lines are the game's own comment
                 * marker (774 of them, including an author note) */
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
    if (m->first_decl[0]) {
        int k = ll_mov_find(m, m->first_decl);
        m->start_st = k > 0 ? k : 0;
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
    p->face = 1;
    /* PR-83 (user-reported, ZARIA.01 started crouched in state index 0):
     * the .MOV grammar's declared state list is authoritative for the
     * starting state - its first name (ll_mov_parse's start_st; measured
     * 2026-11-16 over all 13 player scripts: ZARIA.01 block index 0 is
     * A_JUMP_DOWN but its declared first name is A_WALK, ZARIA.02's is
     * A_SCIAG vs A_WALK, KARA's A_WSP_LIN vs A_WALK). A_STAY still wins
     * where it exists (RAN.01's pinned standing spawn), because that
     * script's declared first name (A_FLY_PTAKI1, a glider state) is not
     * a standing state; whether the original forces standing or starts at
     * the declared name is OPEN (recorded in HANDOFF). */
    int stay = ll_mov_find(mov, "A_STAY");
    if (stay >= 0)
        p->sti = stay;
    else if (mov->start_st > 0 && mov->start_st < mov->nst)
        p->sti = mov->start_st;
    else
        p->sti = 0;
}

static int cond_true(const ll_player *p, const ll_play_keys *k,
                     const ll_mov_tr *tr)
{
    int any = k->k_fwd || k->k_bkwd || k->k_up || k->k_down;
    /* PR-85 MODEL CHOICE (recorded, evidence-based): K_FWD/K_BKWD are
     * FACING-RELATIVE - K_FWD = the arrow in the direction the lion
     * faces, K_BKWD = the opposite arrow. DATA PROOF: ZARIA's LEW.ZAR
     * has exactly ONE walk state (A_WALK, entered only by K_FWD from
     * A_DRAP/A_WALK) and NO backward-walk state anywhere; under
     * literal-key semantics its lion could never walk left at all
     * (BKWD lines lead only into A_TURN, whose only exits are
     * AC_BREAK_* back to the hub). On video the player obviously walks
     * left (PR-72 measured the facing-left screen band 200-230).
     * RAN's A_WALK_FWD/A_WALK_BKWD pair is consistent too: entered
     * facing-relative, CH_DIR keeps face aligned, the pair is the
     * same state written twice for a right-facing spawn. K_UP/K_DOWN
     * stay literal (ladders/vines do not mirror). */
    int face = p->face >= 0 ? 1 : -1;
    int fwd_rel  = face  > 0 ? k->k_fwd : k->k_bkwd;
    int bkwd_rel = face  > 0 ? k->k_bkwd : k->k_fwd;
    for (int i = 0; i < tr->nc; i++) {
        int v;
        switch (tr->c[i].cond) {
        case LL_COND_K_FWD:  v = fwd_rel;  break;
        case LL_COND_K_BKWD: v = bkwd_rel; break;
        case LL_COND_K_UP:   v = k->k_up;   break;
        case LL_COND_K_DOWN: v = k->k_down; break;
        case LL_COND_K_GEN1: v = p->gen[0]; break;
        case LL_COND_K_GEN2: v = p->gen[1]; break;
        case LL_COND_K_GEN3: v = p->gen[2]; break;
        case LL_COND_K_ANY:  v = any;       break;
        case LL_COND_K_JUMP: v = k->k_jump; break;
        case LL_COND_AC_BREAK_NEXT: v = p->broke; break;
        case LL_COND_AC_TIME: v = p->ac_time; break;  /* PR-83: player-side
             * arm = the frame list wrapped (ZARIA A_WALK->A_DRAP idle chain) */
        default:             v = 0;         break;   /* OTHER: not simulated */
        }
        if (tr->c[i].neg)
            v = !v;
        if (v)
            return 1;                                    /* OR semantics */
    }
    return 0;
}

/* PR-74: entering a frame that carries `I ix,iy` throws the lion: the
 * horizontal part follows his facing, the vertical part is up (negative). */
static void apply_impulse(ll_player *p, const ll_mov_st *st, int fi)
{
    if (fi < 0 || fi >= st->nfr || !st->fr[fi].has_i)
        return;
    static int told;

    if (!told++)
        printf("ll_play: jump impulse units are ESTIMATED (I/%d u/s, g %.0f u/s^2;"
               " one measured jump, see NEXT_STEPS PR-74)\n", LL_PLAY_JUMP_DIV,
               LL_PLAY_JUMP_G);
    p->vx = (double)st->fr[fi].ix / LL_PLAY_JUMP_DIV * (p->walkdir ? p->walkdir : 1);
    p->vy = (double)st->fr[fi].iy / LL_PLAY_JUMP_DIV;
    /* PR-83: only an UPWARD (iy<0) impulse lifts the lion. ZARIA's
     * A_TURN frame 0 is `I 100,0` - a purely horizontal push (the turn's
     * little slide). Lifting for it put the lion into the landing pass,
     * whose jump-end cancel (the WAIT AC_DUP model) then killed the turn
     * state in one tick - A_TURN/A_DRAP flickered forever while BKWD was
     * held. Horizontal impulses keep the feet down. */
    if (st->fr[fi].iy < 0) {
        p->grounded = 0;
        p->jumping = 1;
    }
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
        int has_dir = 0;
        for (int i = 0; i < st->tr[t].nc; i++) {
            if (st->tr[t].c[i].neg)
                continue;
            /* PR-85: the conditions are facing-relative (see cond_true),
             * so a matched K_FWD names the arrow ALONG the current face
             * and K_BKWD the opposite one; walkdir is the literal key
             * direction (that is what physics and impulses need), and
             * CH_DIR below re-aligns the draw with it. */
            int fd = p->face >= 0 ? 1 : -1;
            if (st->tr[t].c[i].cond == LL_COND_K_FWD) {
                p->walkdir = fd;
                has_dir = 1;
            } else if (st->tr[t].c[i].cond == LL_COND_K_BKWD) {
                p->walkdir = -fd;
                has_dir = 1;
            }
        }
        /* PR-83: a BY_X state whose CURRENT state was reached (or kept)
         * through a direction condition walks on the direction key (below);
         * one reached through anything else (K_GENn/AC_*: the cutscene
         * walkers) walks on its own. Re-evaluated every tick a transition
         * matches, self-targets included - the self-target line of a held
         * walk state ("A_WALK(x)=K_FWD") re-arms the key-gated mode. */
        p->auto_walk = !has_dir;
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
            apply_impulse(p, &p->mov->st[p->sti], 0);
        }
        break;
    }
    st = &p->mov->st[p->sti];
    /* PR-82: CH_DIR ("change direction") is the flag whose meaning PR-81
     * pinned for scene walkers - the DRAW turns with the movement
     * direction (video + art evidence there). Every player state that can
     * go either way carries it (measured: A_WALK_FWD/A_WALK_BKWD yes, with
     * all-positive frame lists; the direction-baked A_GO_TO_BALON_L no,
     * with its frames authored negative). So: while a CH_DIR state runs,
     * facing follows walkdir; states that bake their direction into the
     * frame sign never touch it, and facing stays as it was (the lion who
     * walked left and stopped in A_STAY - no CH_DIR - keeps facing left;
     * the .MOV data has no turn animation for that, A_TURN belongs to
     * SEQ_REV_X turns, not to stopping). */
    if (st->ctrl & LL_CTRL_CH_DIR)
        p->face = p->walkdir < 0 ? -1 : 1;
    for (int i = 0; i < 3; i++)                        /* one-tick pulses */
        p->gen[i] = 0;
    p->broke = 0;
    p->ac_time = 0;

    /* 2) horizontal physics from the real F=/T= numbers: approach the
     * state's target velocity (BY_X states walk at +/-T; everything else
     * brakes toward 0 at its own F - A_STAY F=600 stops hard, matching
     * the real data).
     * PR-83 (user-reported: ZARIA kept walking forever after one press):
     * a key-driven BY_X state - one entered/kept through K_FWD or
     * K_BKWD - only has its target velocity while the matching key is
     * actually held. ZARIA's A_WALK is the game's combined idle+walk
     * state (it has NO !K_FWD exit anywhere; its F=100,0 is the authored
     * brake-to-standstill); RAN never reaches this gate at key release
     * because its !K_FWD transition to A_STAY wins the same tick.
     * auto_walk states (K_GEN1 cutscene walkers: GO_TO_BALON_*, EXIT,
     * CHANGE_LEVEL) keep walking with no key held. */
    int key_walk = p->walkdir > 0 ? keys->k_fwd : keys->k_bkwd;
    double tv = (st->ctrl & LL_CTRL_BY_X) && (key_walk || p->auto_walk)
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
        if (p->jumping)
            g = LL_PLAY_JUMP_G;
        else if (g == 0.0 && (st->ctrl & LL_CTRL_CHECK_LN))
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
            } else if (p->support) {
                p->vy = 0;                    /* standing on a solid object */
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
            if (p->vy >= 0 && ll_plt_first_below(ground, xi, yi, &hit) &&
                p->y >= (double)hit.y - 1.0) {
                p->y = (double)hit.y;
                p->vy = 0;
                p->grounded = 1;
                p->gen[LL_COND_K_GEN2 - LL_COND_K_GEN1] = 1; /* landing   */
                ev |= LL_PLAY_EV_GROUND;
                if (p->jumping) {      /* viewer rule: a landing ends the jump
                                        * state (the authored `WAIT AC_DUP`
                                        * hold is not decoded) */
                    p->jumping = 0;
                    int idle = ll_mov_find(p->mov, "A_STAY");
                    if (idle < 0)
                        idle = ll_mov_find(p->mov, "A_DRAP");
                    if (idle >= 0 && idle != p->sti) {
                        p->sti = idle;
                        p->fi = 0;
                        p->ft = 0;
                        p->vx = 0;
                        ev |= LL_PLAY_EV_STATE;
                        st = &p->mov->st[p->sti];   /* step 7 uses the NEW state */
                    }
                }
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
     * is armed by a wrap and consumed by transitions next tick.
     * PR-83: `GEN` tails raise their signal as the frame completes
     * (same rule the object engine follows, ll_level.c obj_advance) -
     * that is the player's AC_TIME mechanism. The state time= timer is
     * NOT acted on: its unit is undetermined (pairs like 32,64 / 90,120
     * read as ranges; acting on it as ms pulled ENA.03's lion out of
     * A_SWIM two ticks after spawn), so it stays counted as a field
     * until RE pins it down. (An earlier model choice - arming AC_TIME
     * at the list WRAP - was wrong for the player: the real scripts
     * name their own source.) */
    if (st->nfr > 0) {
        p->ft += dt_ms;
        unsigned guard = 0;
        while (p->ft >= st->fr[p->fi].ms && guard++ < 4u * (unsigned)st->nfr) {
            p->ft -= st->fr[p->fi].ms;
            p->x += st->fr[p->fi].dx;
            p->y += st->fr[p->fi].dy;
            unsigned gf = st->fr[p->fi].gen;
            if (gf & LMOV_GEN_AC_TIME)
                p->ac_time = 1;
            for (int gi = 0; gi < 3; gi++)   /* K_GEN4 has no player field */
                if (gf & (1u << gi))
                    p->gen[gi] = 1;
            if (p->fi + 1 >= st->nfr) {
                p->fi = 0;
                p->broke = 1;
            } else {
                p->fi++;
                apply_impulse(p, st, p->fi);
            }
        }
        if (p->ft >= st->fr[p->fi].ms)                /* guard hit (data */
            p->ft = 0;                                /* safety): resync  */
    }
    return ev;
}

int ll_player_mirror_of(const ll_player *p, int authored)
{
    return authored ^ (p->face < 0 ? 1 : 0);
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

ll_frame_ref ll_mov_frame_ref(int n)
{
    ll_frame_ref r = { 0, 0, 0 };
    if (n > 0)
        r.idx = n - 1;
    else if (n < 0) {
        r.idx = -n - 1;
        r.mirror = 1;
    } else
        r.blank = 1;
    return r;
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

    /* an AUTHORED-EMPTY ground is data, not a parse failure: JUNCTION's
     * platformy=none.des carries literally "0" (its level has no floor).
     * A count with no geometry, or no count at all, stays a failure. */
    ll_plt plt3;
    CHECK(ll_plt_parse("0", &plt3) == 0 && plt3.n == 0);
    ll_plt_free(&plt3);
    CHECK(ll_plt_parse("3\r\n", &plt3) == 1 && plt3.n == 0);
    CHECK(ll_plt_parse("nope\r\n1 0,0;5,0;\r\n", &plt3) == 0 && plt3.n == 1);
    ll_plt_free(&plt3);

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

    ll_play_keys k = { 0, 0, 0, 0, 0 };
    k.k_fwd = 1;
    int ev = ll_player_step(&p, &pl, &k, 0, 2000, 20);
    CHECK((ev & LL_PLAY_EV_STATE) && p.sti == 1);
    CHECK(p.face == 1);                                /* PR-82: walking right */
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
    CHECK(p.face == -1);                               /* PR-82: walking left  */
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
    /* PR-82: facing follows the walk (the fixture's control flags are
     * the REAL RAN LEW ones: both walk states carry CH_DIR, A_STAY does
     * not; all frames positive = the art faces right). While walking
     * left in a CH_DIR state face is -1, and it STAYS -1 in A_STAY
     * (instance state, like lvl_obj.face): a lion that walked left and
     * stopped keeps facing left - no state in the real data flips back.
     * The draw/collision mirror = authored sign XOR face. */
    CHECK(p.face == -1);
    CHECK(ll_player_mirror_of(&p, 0) == 1);          /* plain frame -> mirrored */
    CHECK(ll_player_mirror_of(&p, 1) == 0);          /* authored mir -> plain   */
    k.k_bkwd = 0;
    ev = ll_player_step(&p, &pl, &k, 0, 2000, 20);   /* !K_BKWD -> A_STAY    */
    CHECK((ev & LL_PLAY_EV_STATE) && p.sti == 0);
    CHECK(p.face == -1);            /* no CH_DIR there: facing untouched    */
    /* PR-85 (facing-relative keys, see cond_true): the bear now FACES
     * LEFT, so the RIGHT key is his K_BKWD. A_STAY's line
     * A_WALK_BKWD(1)=K_BKWD matches: walkdir = -face = RIGHT (walkdir is
     * always the literal arrow), CH_DIR re-aligns face to the movement.
     * A frame later that key's MEANING flipped with the facing, so
     * A_WALK_BKWD's only line A_STAY(1)=!K_BKWD holds: the model
     * flickers through A_STAY and settles into A_WALK_FWD - the state
     * pair whose name tracks the facing, and the only one carrying the
     * self-sustaining line. That one-tick flicker is how RAN's data
     * turns around (it has no turn state); ZARIA authors A_TURN for it,
     * see the ZM fixture. The physics stay clean: the arrow walks him
     * along itself. */
    k.k_fwd = 1;
    ev = ll_player_step(&p, &pl, &k, 0, 2000, 20);
    CHECK((ev & LL_PLAY_EV_STATE) && p.sti == 2);    /* A_WALK_BKWD         */
    CHECK(p.walkdir == 1 && p.face == 1);            /* RIGHT arrow, CH_DIR */
    ev = ll_player_step(&p, &pl, &k, 0, 2000, 20);
    CHECK((ev & LL_PLAY_EV_STATE) && p.sti == 0);    /* !K_BKWD now TRUE    */
    ev = ll_player_step(&p, &pl, &k, 0, 2000, 20);
    CHECK((ev & LL_PLAY_EV_STATE) && p.sti == 1);    /* settled A_WALK_FWD  */
    CHECK(p.walkdir == 1 && p.face == 1);
    double x1 = p.x;
    for (int t = 0; t < 25; t++)                     /* 0.5 s: the flicker  */
        ll_player_step(&p, &pl, &k, 0, 2000, 20);    /* ticks brake (F=600) */
    CHECK(p.x > x1 + 10);           /* then re-accelerate: walks RIGHT,    */
    CHECK(p.face == 1);             /* facing RIGHT again                  */
    CHECK(ll_player_mirror_of(&p, 0) == 0);          /* plain frame, plain draw */
    CHECK(ll_player_mirror_of(&p, 1) == 1);          /* authored mir -> mirrored */
    /* Release + press LEFT: the mirror-symmetric flow (A_STAY's K_BKWD
     * line at face=+1 is now the literal LEFT key - one A_WALK_BKWD
     * tick, back through A_STAY, settled in A_WALK_FWD walking LEFT
     * with face=-1: the same state runs the walk either way, RAN's
     * state NAMES bake the author's frame polarity (frames positive =
     * art faces right) while the CH_DIR engine flag tracks the
     * MOVEMENT, exactly as PR-82 measured. A left-moving bear in
     * A_WALK_FWD draws left-facing: the plain frames render mirrored. */
    k.k_fwd = 0;
    ev = ll_player_step(&p, &pl, &k, 0, 2000, 20);   /* !K_FWD -> A_STAY    */
    CHECK((ev & LL_PLAY_EV_STATE) && p.sti == 0);
    k.k_bkwd = 1;
    ev = ll_player_step(&p, &pl, &k, 0, 2000, 20);
    CHECK((ev & LL_PLAY_EV_STATE) && p.sti == 2);    /* A_WALK_BKWD tick    */
    ev = ll_player_step(&p, &pl, &k, 0, 2000, 20);
    CHECK((ev & LL_PLAY_EV_STATE) && p.sti == 0);    /* !K_BKWD holds now   */
    ev = ll_player_step(&p, &pl, &k, 0, 2000, 20);
    CHECK((ev & LL_PLAY_EV_STATE) && p.sti == 1);    /* settled A_WALK_FWD  */
    CHECK(p.walkdir == -1 && p.face == -1);          /* walking LEFT        */
    CHECK(ll_player_mirror_of(&p, 0) == 1);          /* mirrored again      */
    CHECK(ll_player_mirror_of(&p, 1) == 0);          /* authored mir -> plain   */
    ll_plt_free(&pl);
    {   /* PR-74: the jump impulse (hand-derived, ESTIMATED units): `I 120,-1200`
         * -> vy = -300 u/s, vx = +30 u/s; g = 1650, 20 ms ticks, gravity
         * applied before integration: after k ticks vy = -300 + 33k, rise =
         * 0.02 * sum(-300 + 33j, j=1..9) = -24.3 at k = 9 (vy = -3), then it
         * falls back to the ground at y = 100 (about 18 ticks in the air) and
         * the viewer rule returns to the idle state. */
        static const char JM[] =
            "A_STAY\nA_JUMP\n\n[A_STAY]\ncontrol=BY_TIME|CHECK_LN\n"
            "F=600,0\nT=90,90\nG=0,0\nA_JUMP(1)=K_JUMP\nseq=a\n1,100\n\n"
            "[A_JUMP]\ncontrol=BY_TIME\nF=180,0\nT=20,1\nG=0,55\nseq=a\n"
            "1,20\n2,20,I 120,-1200\n3,2000\n";
        static const char JP[] = "1\n1 0,100;2000,100;\n";
        ll_mov_t *jm = calloc(1, sizeof *jm);
        ll_plt jp;
        ll_player q;
        ll_play_keys jk = { 0, 0, 0, 0, 0 };
        double top = 100;
        int air = 0, back = 0;

        CHECK(jm && ll_plt_parse(JP, &jp) == 0);
        ll_mov_parse(JM, jm);
        CHECK(jm->st[1].fr[1].has_i && jm->st[1].fr[1].ix == 120 &&
              jm->st[1].fr[1].iy == -1200);
        ll_player_start(&q, jm, 500, 100, 1);
        jk.k_jump = 1;
        ll_player_step(&q, &jp, &jk, 0, 2000, 20);       /* STAY -> JUMP */
        jk.k_jump = 0;
        CHECK(q.sti == 1);
        for (int t = 0; t < 80; t++) {
            ll_player_step(&q, &jp, &jk, 0, 2000, 20);
            if (!q.grounded)
                air++;
            if (q.y < top)
                top = q.y;
            if (q.grounded && air > 3 && !back)
                back = t;
        }
        CHECK(top > 100 - 26.5 && top < 100 - 22.5);     /* apex ~24.3 */
        CHECK(air >= 16 && air <= 20);
        CHECK(back > 0 && q.sti == 0 && q.y == 100 && q.vx == 0);
        ll_plt_free(&jp);
        free(jm);
    }
    {   /* PR-82: states whose DIRECTION IS AUTHORED stay out of facing.
         * The real shape is RAN LEW's A_GO_TO_BALON_L: control=BY_X with
         * NO CH_DIR and frames authored NEGATIVE (-1..-12), while the
         * walking states (A_WALK_FWD/BKWD, all-positive frames) all carry
         * CH_DIR. So the authored sign is what the author used to aim
         * those states, and facing is left exactly as the CH_DIR walks
         * set it (instance state, like lvl_obj.face).
         * ASSERTS THE CHOSEN MODEL's consequence, not a measured pixel:
         * the draw mirror is always authored XOR face (the one
         * video-pinned case, PR-81's worm, works that way - its states
         * all carry CH_DIR). Whether the engine skips the instance facing
         * ENTIRELY for a no-CH_DIR state is the open question recorded
         * in HANDOFF (test that would settle it: a mirrored-polarity
         * match on the RAN crown/balloon cutscene, or any left-facing
         * standing lion on video - this game video walks right-only). */
        static const char GL[] =
            "A_STAY\nA_WALK_BKWD\nA_GO_TO_BALON_L\n\n[A_STAY]\n"
            "control=BY_TIME\nF=600,0\nG=0,0\nA_WALK_BKWD(1)=K_BKWD\n"
            "seq=uj09\n1,100\n\n[A_WALK_BKWD]\ncontrol=BY_X|CH_DIR\n"
            "F=400,0\nT=90,90\nG=0,0\nA_GO_TO_BALON_L(1)=K_GEN1\n"
            "seq=uj09\n1,100\n2,100\n\n"
            "[A_GO_TO_BALON_L]\ncontrol=BY_X\nF=0,0\nT=90,90\nG=400,0\n"
            "seq=uj09\n-1,1\n-2,1\n-3,1\n";
        static const char GP[] = "1\n1 0,100;2000,100;\n";
        ll_mov_t *gm = calloc(1, sizeof *gm);
        ll_plt gp;
        ll_player q;
        ll_play_keys gk = { 0, 0, 0, 0, 0 };

        CHECK(gm && ll_plt_parse(GP, &gp) == 0);
        CHECK(ll_mov_parse(GL, gm) == 3);
        CHECK(!(gm->st[2].ctrl & LL_CTRL_CH_DIR) &&
              (gm->st[1].ctrl & LL_CTRL_CH_DIR));
        ll_player_start(&q, gm, 500, 100, 1);
        gk.k_bkwd = 1;
        ll_player_step(&q, &gp, &gk, 0, 2000, 20);   /* -> walk left     */
        CHECK(q.sti == 1 && q.face == -1);
        gk.k_bkwd = 0;
        q.gen[0] = 1;                                /* K_GEN1 trigger   */
        ll_player_step(&q, &gp, &gk, 0, 2000, 20);   /* -> the baked state */
        CHECK(q.sti == 2);
        CHECK(q.walkdir == -1);                      /* the transition set it */
        CHECK(q.face == -1);                         /* but facing is untouched */
        {   /* and what gets drawn: authored mirror (1) XOR face (1) = 0
             * under the chosen always-XOR model */
            CHECK(ll_player_frame(&q) == -1);         /* the authored -1 frame */
            ll_frame_ref fr = ll_mov_frame_ref(ll_player_frame(&q));
            CHECK(fr.idx == 0 && fr.mirror == 1 && !fr.blank);
            CHECK(ll_player_mirror_of(&q, fr.mirror) == 0);
        }
        ll_plt_free(&gp);
        free(gm);
    }
    {   /* PR-83 (the two user reports, data-free regression on the shapes
         * measured from ZARIA LEW.ZAR): the declared list - not block
         * order - names the spawn state; A_WALK is the combined
         * idle+walk (NO !K_FWD exit anywhere: stopping comes from the
         * key-gated target velocity + the authored F brake, and the walk
         * state's own GEN AC_TIME tail is what drops to the idle hub);
         * BKWD plays A_TURN once and it STAYS while the key is held;
         * and the frame `I 100,0` horizontal impulse (A_TURN frame 0)
         * never lifts the feet. Also guards: `time=` is parsed but NOT
         * acted on, and the frame-list wrap does not arm AC_TIME. */
        static const char ZM[] =
            "A_WALK\nA_DRAP\nA_TURN\nA_JUMP_DOWN\n\n"
            "[A_JUMP_DOWN]\ncontrol=BY_TIME\nF=0,0\nG=0,0\nseq=a\n1,100\n\n"
            "[A_WALK]\ncontrol=BY_X|CH_DIR\nF=100,0\nT=20,5\nG=0,0\n"
            "A_TURN(1)=K_BKWD\nA_WALK(x)=K_FWD\nA_DRAP(1)=AC_TIME\n"
            "seq=a\n1,50\n2,50,GEN AC_TIME\n\n"
            "[A_DRAP]\ncontrol=BY_TIME\nF=100,0\nG=0,0\n"
            "A_TURN(1)=K_BKWD\nA_WALK(1)=K_FWD\nseq=a\n1,100\n\n"
            "[A_TURN]\ncontrol=BY_TIME|CH_DIR\nF=100,0\nG=0,0\n"
            "A_DRAP(1)=AC_BREAK_NEXT\nA_DRAP(1)=AC_BREAK_PREV\n"
            "seq=a\n1,50,I 100,0\n2,100\n3,100\n";
        static const char ZP[] = "1\n1 0,100;2000,100;\n";
        ll_mov_t *zm = calloc(1, sizeof *zm);
        ll_plt zp;
        ll_player s;
        ll_play_keys sk = { 0, 0, 0, 0, 0 };

        CHECK(zm && ll_plt_parse(ZP, &zp) == 0);
        CHECK(ll_mov_parse(ZM, zm) == 4);
        /* the header names A_WALK first but its .MOV block is SECOND here
         * (A_JUMP_DOWN block leads, exactly like ZARIA's file): start_st
         * follows the declaration, and there is no A_STAY to prefer. */
        CHECK(zm->start_st == 1 && ll_mov_find(zm, "A_JUMP_DOWN") == 0);
        ll_player_start(&s, zm, 500, 100, 1);
        CHECK(s.sti == 1 && strcmp(zm->st[1].name, "A_WALK") == 0);
        /* zero input: the A_WALK frames' GEN AC_TIME tail (never the
         * frame-list wrap, never a timer) drops to the idle hub after a
         * few frames - and the lion never drifts on the way. */
        double zx = s.x;
        int idle_at = 0;
        for (int t = 1; t <= 12 && !idle_at; t++) {
            ll_player_step(&s, &zp, &sk, 0, 2000, 20);
            if (s.sti == 2)
                idle_at = t;
        }
        CHECK(idle_at >= 4 && idle_at <= 10);          /* frame cycle is 100ms */
        CHECK(s.x == zx);                              /* zero input never creeps */
        for (int t = 0; t < 5; t++)
            ll_player_step(&s, &zp, &sk, 0, 2000, 20);
        CHECK(s.x == zx);
        /* press FWD: A_DRAP -> A_WALK, walking up to the authored T=20 */
        sk.k_fwd = 1;
        ll_player_step(&s, &zp, &sk, 0, 2000, 20);
        CHECK(s.sti == 1);
        zx = s.x;
        for (int t = 0; t < 25; t++) {
            ll_player_step(&s, &zp, &sk, 0, 2000, 20);
            CHECK(s.vx <= 20.000001);
        }
        CHECK(s.x > zx + 5);
        /* RELEASE (this is where the SDL KEYUP fix enters the picture -
         * a stuck key would keep walking forever): NO !K_FWD exit exists,
         * so the walk state stays - the key-gated target velocity plus
         * F=100 brake him to a standstill within a few px, then the next
         * AC_TIME pulse moves to the idle. */
        double xr = s.x;
        int rel_idle = 0;
        sk.k_fwd = 0;
        for (int t = 1; t <= 30 && !rel_idle; t++) {
            ll_player_step(&s, &zp, &sk, 0, 2000, 20);
            if (s.sti == 2)
                rel_idle = t;
        }
        CHECK(rel_idle >= 1 && rel_idle <= 8);
        /* the exact key-gated model: velocity has been braking at
         * F=100 (2 u/s per 20 ms tick) since the release; a non-gated
         * BY_X state would still sit at the T=20 target here */
        CHECK(s.vx <= 20.0 - 2.0 * rel_idle + 1e-9);
        for (int t = 0; t < 10; t++)
            ll_player_step(&s, &zp, &sk, 0, 2000, 20);
        CHECK(s.sti == 2 && s.vx == 0);
        CHECK(s.x - xr < 8.0);                         /* braked, not coasted */
        double xs = s.x;
        for (int t = 0; t < 5; t++)
            ll_player_step(&s, &zp, &sk, 0, 2000, 20);
        CHECK(s.x == xs);
        /* BKWD (the left key, the user's "turns but never walks left"):
         * A_TURN plays, and the REAL exit is AC_BREAK_NEXT - armed by
         * the frame-list WRAP (250 ms here) - back to the A_DRAP hub;
         * with the key STILL held the hub's A_WALK(1)=K_FWD matches
         * (facing-relative: LEFT is K_FWD once face=-1) and the lion
         * WALKS LEFT. Under the old literal-key model this was a dead
         * end: A_TURN's F=0 froze the turn slide forever - exactly the
         * reported "slides more than he walks, and slow". The frame-0
         * impulse I 100,0 keeps the FEET DOWN (iy==0 lifts nothing - a
         * forced airborne state made A_TURN/A_DRAP flicker while BKWD
         * was held, the earlier reported loop); wrap still arms
         * AC_BREAK_NEXT but NOT AC_TIME (the removed model choice; the
         * time= guard is the PK fixture below). */
        sk.k_bkwd = 1;
        ll_player_step(&s, &zp, &sk, 0, 2000, 20);
        CHECK(s.sti == 3);                             /* A_TURN            */
        CHECK(s.walkdir == -1 && s.face == -1);        /* the LEFT key      */
        CHECK(s.grounded && !s.jumping && s.y == 100); /* horizontal impulse */
        /* the impulse IS applied (I/4 = 25 u/s), mirrored by walkdir as
         * everywhere in this model: A_TURN slides LEFT with it; F=100
         * brakes one tick of it in the same step */
        CHECK(s.vx <= -20.0);
        for (int t = 1; t <= 12; t++)                  /* 250 ms: frames
                                                         * 1,2,3 then wrap  */
            ll_player_step(&s, &zp, &sk, 0, 2000, 20);
        CHECK(s.sti == 3);                             /* no early exit:
                                                        * wrap != AC_TIME  */
        for (int t = 0; t < 4; t++)                    /* the wrap armed
                                                        * AC_BREAK_NEXT    */
            ll_player_step(&s, &zp, &sk, 0, 2000, 20);
        CHECK(s.sti == 1);                             /* A_WALK, walking  */
        CHECK(s.walkdir == -1 && s.face == -1);        /* LEFT, drawn LEFT */
        double xw = s.x;
        for (int t = 0; t < 25; t++)                   /* 0.5 s at the   */
            ll_player_step(&s, &zp, &sk, 0, 2000, 20); /* T=20 u/s cap   */
        CHECK(s.x < xw - 5);                           /* walks LEFT: ~10 u */
        ll_plt_free(&zp);
        free(zm);
    }
    {   /* PR-83: a frame's `GEN K_GEN1` tail reaches the player's signal
         * bus exactly like the landing pulse does - AND the state it
         * names becomes an AUTO WALKER: entered with no direction
         * condition, its BY_X target velocity applies with no key held
         * (the cutscene walkers: RAN's A_GO_TO_BALON_*, A_EXIT). The
         * state also carries time=50 with an AC_TIME exit: it must NOT
         * act (unit undetermined; acting on it pulled ENA.03's lion out
         * of A_SWIM two ticks after spawn). */
        static const char PK[] =
            "A_RUN\nA_PULL\n\n[A_RUN]\ncontrol=BY_TIME\nF=0,0\nG=0,0\n"
            "A_PULL(1)=K_GEN1\nseq=a\n1,50,GEN K_GEN1\n2,50\n\n"
            "[A_PULL]\ncontrol=BY_X\nF=100,0\nT=30,30\nG=0,0\n"
            "time=50\nA_RUN(1)=AC_TIME\nseq=a\n1,100\n";
        static const char PP[] = "1\n1 0,100;2000,100;\n";
        ll_mov_t *pm = calloc(1, sizeof *pm);
        ll_plt pp;
        ll_player q;
        ll_play_keys qk = { 0, 0, 0, 0, 0 };

        CHECK(pm && ll_plt_parse(PP, &pp) == 0);
        CHECK(ll_mov_parse(PK, pm) == 2);
        CHECK(pm->st[1].time_ms == 50);                /* parsed, counted  */
        ll_player_start(&q, pm, 500, 100, 1);
        int pull_at = 0;
        for (int t = 1; t <= 8 && !pull_at; t++) {
            ll_player_step(&q, &pp, &qk, 0, 2000, 20);
            if (q.sti == 1)
                pull_at = t;
        }
        CHECK(pull_at >= 3 && pull_at <= 5);           /* frame 1 ends at 50 ms */
        CHECK(q.auto_walk && !q.gen[0]);               /* pulse consumed   */
        double qx = q.x;
        for (int t = 0; t < 40; t++) {
            ll_player_step(&q, &pp, &qk, 0, 2000, 20);
            CHECK(q.vx <= 30.000001);
        }
        CHECK(q.x > qx + 15 && q.sti == 1);           /* walks keylessly,
                                                        * AC_TIME exit stayed dark */
        ll_plt_free(&pp);
        free(pm);
    }
    if (!rc)
        printf("play selftest: .plt physics (+ the '!' separator variant,"
               " + an authored-empty '0' ground is data not an error),"
               " extended .MOV transitions walk/turn/stop, self-transition"
               " keeps its state (frames keep running), edge fall, K_GEN2"
               " landing pulse, PR-83: declared-name spawn, key-gated walk"
               " stops on release, GEN frame pulses, wrap/time= arm nothing,"
               " horizontal impulse keeps the feet down, auto-walk stays (ok)\n");
    return rc;
}
