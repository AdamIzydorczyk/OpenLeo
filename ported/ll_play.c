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

/* PR-107: the row a transition's `( N ) ` starts the TARGET state at, as
 * a 0-based index into its frame list. One rule for both sides that read
 * it - the lion (ll_player_step) and the scene objects (obj_transition) -
 * so they cannot drift apart again. The engine's own, verbatim: the
 * transition line parse matches `" ( %[^) ] %*[)] = "` (the format at
 * 0x9225c, called from 0x51e12), tries `"%ld"` on that token (0x57db0,
 * which calls the scanner at 0x74cc8 with the format at 0x92310) and on
 * a number stores `strtol` MINUS ONE (0x57fd0 + `dec eax` at 0x51e3b);
 * an absent or non-numeric token stores -1 (0x51e3e). That value is the
 * transition entry's row field at +0x11, and it reaches the live row
 * counter of the TARGET state record by both of the engine's paths. The
 * walk 0x52000 hands it out (0x52046-0x5204d) to the transition apply
 * step 0x535a0 (0x535a2 is listed one byte in), which takes the
 * target state index in edx and the start row in ebx: a same-index
 * line writes the row into the current record straight out (0x53616,
 * and only if it differs - 0x53612 skips the
 * write otherwise); a cross-state line reaches the same field through
 * the state record's method table, because every record stores the fixed
 * table 0xa25d4 at rec+0x44 (its constructors, 0x524cf and 0x52508) and
 * that step calls slot +0x1c of it on the NEW record (0x535e7-0x535ea),
 * and +0x1c IS the row-set method 0x52360: it writes rec+0x4 raw
 * (0x523cb) after refusing a negative value (`test edx,edx` + `jl`,
 * 0x52381). The value stays live in edx through the methods in between
 * - they save and restore it themselves (`push edx` 0x52660 /
 * `pop edx` 0x5267a, and 0x52072 / 0x520b4), the way this binary
 * carries its second register argument.
 * rec+0x4 IS the row index: the row-advance method 0x520c0 steps it
 * (`[rec+0x4] = ecx+1`, 0x521d7) and wraps it at the last row
 * (`[rec+0x4] = 0`, 0x521c5). The engine has NO upper test on
 * the value, and mirrors it (`[rec+0x36] - n - 1`, 0x523bc-0x523c5) when
 * the SEQ_REV reversal of PR-95 applies - neither of which the port
 * models: an over-long row is clamped to the top here (the rule the
 * player side shipped in PR-86, and what keeps a state with a short
 * frame list from being skipped whole), and -1 (the port's entry 0:
 * absent or `"(x)"`) starts from the top instead of leaving the row
 * where it was. Both are MODEL CHOICES, recorded in NEXT_STEPS PR-107. */
int ll_mov_row_start(const ll_mov_st *s, int entry)
{
    int i = entry > 0 ? entry - 1 : 0;      /* the authored N is 1-based */

    if (i >= s->nfr)
        return 0;                           /* over-long row: from the top */
    return i;
}

/* PR-88: name -> LL_SIG_* index. The table is the engine's own (name
 * block 0x919ec, bit constants from the comparison chain at 0x48000 -
 * see the ll_play.h enum comment). K_ANY / K_NEVER and their AC_ aliases
 * are NOT bits there (the parser turns them into node op tags), and our
 * cond_id already models them, so they return -1 here. */
int ll_sig_id(const char *name)
{
    static const struct { const char *n; int id; } tbl[] = {
        { "AC_BLOK", LL_SIG_AC_BLOK },        { "AC_BLOK_Y", LL_SIG_AC_BLOK_Y },
        { "AC_BREAK_NEXT", LL_SIG_AC_BREAK_NEXT },
        { "AC_BREAK_PREV", LL_SIG_AC_BREAK_PREV },
        { "AC_DUP", LL_SIG_AC_DUP },          { "AC_OFF", LL_SIG_AC_OFF },
        { "AC_DEADDUP", LL_SIG_AC_DEADDUP },  { "AC_ENEMY", LL_SIG_AC_ENEMY },
        { "AC_EDGE", LL_SIG_AC_EDGE },        { "AC_TIME", LL_SIG_AC_TIME },
        { "AC_DRAB", LL_SIG_AC_DRAB },        { "AC_DRABK", LL_SIG_AC_DRABK },
        { "AC_DRAB_CATCH", LL_SIG_AC_DRAB_CATCH },
        { "AC_DUP_CD", LL_SIG_AC_DUP_CD },    { "AC_STONE", LL_SIG_AC_STONE },
        { "AC_PUSH", LL_SIG_AC_PUSH },        { "AC_ENEMY_ATC", LL_SIG_AC_ENEMY_ATC },
        { "AC_RECT", LL_SIG_AC_RECT },        { "AC_ONAIR", LL_SIG_AC_ONAIR },
        { "AC_DRECT", LL_SIG_AC_DRECT },      { "AC_DRECT2", LL_SIG_AC_DRECT2 },
        { "AC_LINA", LL_SIG_AC_LINA },        { "AC_LINA_CATCH", LL_SIG_AC_LINA_CATCH },
        { "AC_LEAVE", LL_SIG_AC_LEAVE },      { "AC_ATAK", LL_SIG_AC_ATAK },
        { "AC_UP_DUP", LL_SIG_AC_UP_DUP },    { "AC_SPOKUJ", LL_SIG_AC_SPOKUJ },
        { "AC_BUTLA", LL_SIG_AC_BUTLA },      { "AC_BECZKA", LL_SIG_AC_BECZKA },
        { "AC_START", LL_SIG_AC_START },      { "AC_MRECT", LL_SIG_AC_MRECT },
        { "AC_TABLICZKA", LL_SIG_AC_TABLICZKA },
        { "AC_KILOF", LL_SIG_AC_KILOF },      { "AC_KOREK", LL_SIG_AC_KOREK },
        { "AC_KOLO", LL_SIG_AC_KOLO },        { "K_BKWD", LL_SIG_K_BKWD },
        { "K_FWD", LL_SIG_K_FWD },            { "K_UP", LL_SIG_K_UP },
        { "K_DOWN", LL_SIG_K_DOWN },          { "K_FIRE", LL_SIG_K_FIRE },
        { "K_ALT", LL_SIG_K_ALT },            { "K_F1", LL_SIG_K_F1 },
        { "K_F2", LL_SIG_K_F2 },              { "K_F3", LL_SIG_K_F3 },
        { "K_F4", LL_SIG_K_F4 },              { "K_F5", LL_SIG_K_F5 },
        { "K_F6", LL_SIG_K_F6 },              { "K_F7", LL_SIG_K_F7 },
        { "K_F8", LL_SIG_K_F8 },              { "K_F9", LL_SIG_K_F9 },
        { "K_F10", LL_SIG_K_F10 },            { "K_F11", LL_SIG_K_F11 },
        { "K_F12", LL_SIG_K_F12 },            { "K_JUMP", LL_SIG_K_JUMP },
        { "K_GEN1", LL_SIG_K_GEN1 },          { "K_GEN2", LL_SIG_K_GEN2 },
        { "K_GEN3", LL_SIG_K_GEN3 },          { "K_GEN4", LL_SIG_K_GEN4 },
        { "K_GEN5", LL_SIG_K_GEN5 },          { "AC_PLAT_OFF", LL_SIG_AC_PLAT_OFF },
        { "AC_PLAT_OUT", LL_SIG_AC_PLAT_OUT },
        { "AC_XCHG_LEVEL", LL_SIG_AC_XCHG_LEVEL },
        { "AC_SHOW_CROWN", LL_SIG_AC_SHOW_CROWN },
        { "AC_MSG_PUSH", LL_SIG_AC_MSG_PUSH },
        { "AC_WALK_UP", LL_SIG_AC_WALK_UP },  { "AC_WALK_DOWN", LL_SIG_AC_WALK_DOWN },
        { "AC_APPEAR", LL_SIG_AC_APPEAR },    { "AC_DISAPPEAR", LL_SIG_AC_DISAPPEAR },
    };
    for (unsigned i = 0; i < sizeof tbl / sizeof tbl[0]; i++)
        if (strcasecmp(tbl[i].n, name) == 0)
            return tbl[i].id;
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
        { "K_GEN5", LL_COND_K_GEN5 },
        { "K_JUMP", LL_COND_K_JUMP },
        { "K_FIRE", LL_COND_K_FIRE },
        { "K_F1", LL_COND_K_F1 },
        { "K_F2", LL_COND_K_F2 },
        { "K_F3", LL_COND_K_F3 },
        { "K_F4", LL_COND_K_F4 },
        { "K_ALT", LL_COND_K_ALT },
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
        { "BY_TY", LL_CTRL_BY_TY },   /* PR-148: the mode-4 feed guard,
                                       * ll_play.c section 7 (the engine's
                                       * 0x5441b); BY_TX (wire mode 3) has
                                       * zero authored uses and still no
                                       * bit (PR-147's byte census). */
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
        { "DANGER", LL_CTRL_DANGER },
    };
    for (unsigned k = 0; k < sizeof tbl / sizeof tbl[0]; k++)
        if (strcasecmp(tbl[k].n, tok) == 0) {
            *f |= tbl[k].bit;
            return;
        }
    /* Named in the ll_play.h vocabulary but without a bit (SEQ_REV_X,
     * INV_V, ...), or a name we have never seen:
     * counted so a missing behaviour is a known gap, never a silent one.
     * PR-99/100/101 read what SEQ_REV_X (bit 4) and SEQ_REV_Y (bit 5)
     * of the state's control word do in the engine: they are ROW-ORDER
     * flags, not motion. Each is only in force while an owner-side
     * guard holds ([[obj+8]+0x70]() == -1 for bit 4, [obj+0x71] == -1
     * for bit 5) and the row handler XORs the two, together with
     * `[obj+0x99] == -1`, to pick forward or mirrored row order
     * (0x5214b-0x52193, and 0x52360's mirror index `[[rec+0x2e]+8] -
     * row - 1`). Not modelled here: no shipped sample pairs them with a
     * row list whose order a video could tell apart. */
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
/* PR-217: a `Wait`/condition token in NO table (LL_COND_OTHER with no
 * signal-table hit). LL_SIG_* indexes never collide with the LL_COND_*
 * ids (every LL_SIG_* value is >= 0 and the LL_COND_* enum starts at 0),
 * so the two share the -2 slot: it means "no bit to test", never a bit. */
#define LL_COND_NOBIT  LL_COND_OTHER   /* LL_SIG_NOBIT is in ll_play.h */

static void parse_cond(ll_mov_gate *c, int *ncp, int max,
                       const char *expr)
{
    const char *q = expr;
    int grp = 0;
    while (*q && *ncp < max) {
        const char *bar = strchr(q, '|');
        const char *bend = bar ? bar : q + strlen(q);
        /* PR-87: a bar segment is one AND-group; ',' splits its tokens
         * (see the ll_mov_tr.c comment). ';' inside stays inside one
         * token -> OTHER (counted, never true). */
        for (;;) {
            const char *comma = strchr(q, ',');
            const char *end = (comma && comma < bend) ? comma : bend;
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
                c[*ncp].cond = cond_id(tok);
                c[*ncp].neg = neg;
                c[*ncp].grp = grp;
                /* PR-88: remember the signal-table index of names the
                 * model does not model as direct conditions (AC_ONAIR,
                 * AC_EDGE, ...): the evaluator tests them against the
                 * object's own signal set instead of never-true OTHER.
                 * PR-217: the SAME name is a BIT in the engine's signal
                 * table, so a modeled condition token gets its LL_SIG_*
                 * index too (K_DOWN -> 38); the transition evaluator never
                 * reads sig for those (its switch answers first), only the
                 * `Wait`-clause capture does. */
                c[*ncp].sig = (short)ll_sig_id(tok);
                if (c[*ncp].sig < 0)
                    c[*ncp].sig = (short)LL_SIG_NOBIT;
                (*ncp)++;
            }
            if (end == bend)
                break;
            q = end + 1;
        }
        grp++;
        q = bar ? bar + 1 : bend;
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
                    s->time_scale = 1000;   /* PR-110: the engine's row */
                    s->scale_at = 0;        /* ctor passes 1000 when no */
                                            /* time_scale line exists   */
                } else {
                    m->drop_st++;
                }
            } else if (m->nst == 0) {
                /* before the first [STATE]: the declared state list.
                 * Its first name is the state the object starts in,
                 * and its ORDER is the engine's state index space:
                 * 0x53905 reads it into the vector [obj+0x3c] and
                 * every state name afterwards is resolved through
                 * 0x53440, which returns its 1-based position (see
                 * ll_mov_st.decl_idx, PR-216). strcasecmp because the
                 * engine's comparator 0x76a69 folds A-Z (0x76a75). */
                if (line[0] != ';' && line[0] != '/' && !strchr(line, '=')) {
                    char nm[24];
                    size_t k = 0;
                    while (line[k] > ' ' && k < sizeof nm - 1) {
                        nm[k] = line[k];
                        k++;
                    }
                    nm[k] = '\0';
                    if (nm[0]) {
                        if (!m->first_decl[0])
                            memcpy(m->first_decl, nm, sizeof m->first_decl);
                        if (m->nddecl < LMOV_MAXST) {
                            memcpy(m->decl[m->nddecl], nm, sizeof m->decl[0]);
                            m->nddecl++;
                        }
                    }
                }
            } else if (m->nst > 0) {
                ll_mov_st *s = &m->st[m->nst - 1];
                int f, ms;
                if (sscanf(line, "%d,%d", &f, &ms) == 2) { /* frame line   */
                    if (s->nfr < LMOV_MAXFR) {
                        ll_mov_fr *mf = &s->fr[s->nfr++];
                        mf->frame = f;
                        /* M4-C (FIX-13): non-positive thresholds would
                         * make obj_advance / the player spin forever:
                         * clamp. Positive author values (walk rows say
                         * 6 units) are kept as-is before the block's
                         * time_scale is applied below, exactly as the
                         * engine reads %hi into node+0x10 first
                         * (0x51679), matching the viewer's mov_parse. */
                        mf->thresh = ms > 0 ? (unsigned)ms : 100u;
                        mf->dx = mf->dy = 0;
                        mf->gsig = 0;
                        mf->atc = 0;
                        mf->has_i = 0;
                        mf->endlv = 0;
                        mf->gsig_id = -1;   /* PR-217: no `Wait` tail */
                        mf->gneg = 0;
                        /* A frame line may carry further fields. The ones the
                         * model acts on are the authored motion of THIS frame,
                         * "N,ms, M dx,dy" (285 of 18617 shipped frame lines
                         * carry it; there are no standalone "M" lines), the
                         * `GEN <signal>` tails (PR-88: the engine's own
                         * directive table is at 0x917c0, the names it can
                         * raise are the 0x919ec block) and ATC_ON/ATC_OFF
                         * (PR-89, the attack-connect window).
                         * Everything else the table names (ground_on/off,
                         * lock/unlock, chdir, ldir/rdir, pos, goto/gotorel,
                         * F/IC/FC, MOVE, the smp and vol sound
                         * words, inline F=) is counted -
                         * measured 1568 such lines before this PR took
                         * the GEN and ATC ones out of that count, PLAN
                         * 8g - never dropped in silence. `end_level`
                         * joined the modeled set in PR-199 once PR-198
                         * had read its apply: it is 26 lines in the
                         * whole shipped set (three LEW.MOV members), so
                         * this parser meets it in those three only.
                         * `Wait`/`WAIT <signal>` joined the CAPTURED set
                         * in PR-217 (255 tails in 133 files): the clause
                         * is kept in ll_mov_fr.gsig_id/gneg and NO model
                         * acts on it yet, so those rows leave the counted
                         * total and the report names them as captured,
                         * not as unread. */
                        const char *fc = strchr(line, ',');
                        const char *rest = fc ? strchr(fc + 1, ',') : NULL;
                        if (rest) {
                            const char *q = rest + 1;
                            int other_directive = 0;
                            int captured_clause = 0;   /* PR-217 census */
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
                                    /* PR-69: `GEN <signal>` = the frame raises
                                     * that signal when it completes.
                                     * PR-83: `GEN AC_TIME` is the same
                                     * mechanism for AC_TIME (the real
                                     * ZARIA A_WALK idle chain: 11 such
                                     * lines across that script).
                                     * PR-88: ANY name in the engine's
                                     * signal table (0x48000) can be
                                     * raised - the shipped scripts do:
                                     * K_GEN1 315x, AC_PLAT_OFF 181x,
                                     * K_GEN3 139x, AC_TIME 60x ... The
                                     * bits land at their LL_SIG_*
                                     * indexes; a name the table has
                                     * (but at index >= 64, none in the
                                     * data) or no name at all stays a
                                     * counted, unmodeled directive. */
                                    while (*q == ' ')
                                        q++;
                                    char nm[24];
                                    size_t nn = 0;
                                    while (q[nn] &&
                                           ((q[nn] >= 'A' && q[nn] <= 'Z') ||
                                            (q[nn] >= 'a' && q[nn] <= 'z') ||
                                            (q[nn] >= '0' && q[nn] <= '9') ||
                                            q[nn] == '_') &&
                                           nn + 1 < sizeof nm) {
                                        nm[nn] = q[nn];
                                        nn++;
                                    }
                                    nm[nn] = '\0';
                                    int sid = nn ? ll_sig_id(nm) : -1;
                                    if (sid >= 0 && sid < 64) {
                                        mf->gsig |= 1ull << sid;
                                        q += nn;
                                        continue;
                                    }
                                }
                                if (strcasecmp(word, "ATC_ON") == 0) {
                                    mf->atc |= LMOV_ATC_ON;
                                    continue;
                                }
                                if (strcasecmp(word, "ATC_OFF") == 0) {
                                    mf->atc |= LMOV_ATC_OFF;
                                    continue;
                                }
                                if (strcasecmp(word, "end_level") == 0) {
                                    /* PR-199: `end_level <n>` - this row
                                     * ENDS THE LEVEL RUN. Kept as the
                                     * authored number (ll_mov_fr.endlv);
                                     * the row admit below is what puts
                                     * it in the level's exit-word slot,
                                     * the copy of the engine's apply
                                     * 0x45750, and the only reader of
                                     * that slot is the level loop's
                                     * compare against zero (0x4ac55),
                                     * which returns the loop code 1 for
                                     * ANY value. The number is therefore
                                     * never interpreted here, and a tail
                                     * with no number after it stays a
                                     * counted directive, never a guess.
                                     * The shipped spelling is lowercase
                                     * in all 26 rows (JUNCTION, TUNGA.11,
                                     * TUNGA2.12); strcasecmp matches the
                                     * other word arms of this parser. */
                                    int v = 0;
                                    while (*q == ' ')
                                        q++;
                                    if (sscanf(q, "%d", &v) == 1) {
                                        mf->endlv = v;
                                        continue;
                                    }
                                }
                                if (strcasecmp(word, "Wait") == 0) {
                                    /* PR-217: `Wait <signal>` - the row's
                                     * own signal clause, KEPT as DATA
                                     * (ll_mov_fr.gsig_id/gneg), not acted
                                     * on yet. The arm mirrors the engine's
                                     * 0x46b62-0x46ba2: a 0x15-byte node
                                     * (class word 0xa11c8) whose operand
                                     * the clause parser 0x47d60 reads into
                                     * the node's 12-byte TEventSet, the
                                     * same ctor and parser the GEN arm
                                     * above uses (0x46af4, class 0xa11e0).
                                     * Measured over the 777
                                     * shipped texts: 255 tails in 133
                                     * files, 254 exactly one token, 22
                                     * `!`-negated; see the ll_play.h
                                     * field note. A tail with no token
                                     * (1 of 255: `Wait ;`) stays a
                                     * counted directive, never a guess. */
                                    char arg[64];
                                    size_t an = 0;
                                    const char *qa = q;
                                    while (*qa == ' ' || *qa == ',')
                                        qa++;
                                    while (*qa && *qa != ';' &&
                                           an + 1 < sizeof arg)
                                        arg[an++] = *qa++;
                                    arg[an] = '\0';
                                    while (an && (arg[an - 1] == ' ' ||
                                                  arg[an - 1] == ','))
                                        an--;
                                    if (an) {
                                        ll_mov_gate g[4];
                                        int gn = 0;
                                        parse_cond(g, &gn, 4, arg);
                                        if (gn == 1) {
                                            mf->gneg = (short)g[0].neg;
                                            mf->gsig_id = g[0].sig;
                                            captured_clause = 1;
                                            continue;
                                        }
                                    }
                                }
                                other_directive = 1;
                            }
                            if (other_directive)
                                s->directive++;
                            if (captured_clause)
                                s->wait_cap++;
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
                        tr->entry = 0;
                        const char *eq = strchr(line, '=');
                        const char *par = strchr(line, '(');
                        size_t n = 0;
                        const char *te = par && par < eq ? par : eq;
                        for (const char *q = line;
                             q < te && n + 1 < sizeof tr->target; q++)
                            tr->target[n++] = *q;
                        tr->target[n] = '\0';
                        /* PR-86: the numeric (...) = START ROW (1-based)
                         * in the target's frame list; non-numeric ("(x)")
                         * starts from the top like an absent one. See the
                         * ll_mov_tr.entry evidence comment. */
                        if (par && par < eq) {
                            const char *pe = strchr(par + 1, ')');
                            if (pe && pe < eq)
                                tr->entry = (int)strtol(par + 1, NULL, 10);
                        }
                        parse_cond(tr->c, &tr->nc, LMOV_MAXCON, eq + 1);
                    } else {
                        m->drop_tr++;
                    }
                } else if (strncmp(line, "time=", 5) == 0) {
                    /* PR-83 parsed this as counted-only; PR-163 MODELS
                     * the engine law (PR-159 F2/PR-160 F2-F3): `time=<min>
                     * [,max]` is the AC_TIME BOUNDS PAIR - ENTER rolls the
                     * gate uniformly in it (Borland rand), and same-state
                     * transition matches raise AC_TIME when the count
                     * reaches the roll; no line = never. The engine's
                     * scanf pair ('%hi' / ',%hi') leaves max=min when the
                     * comma is absent (the single-value `time=350` form,
                     * ENA.03_SURPRISE) - parse_pair does the same, so the
                     * form needs no special case HERE; what the engine's
                     * lerp computes with max=min stays its open item (the
                     * port's reading: gate=min, a deterministic N-match
                     * wait; PR-163 selftest pins it). Still counted so no
                     * report number moves. */
                    int t1 = 0, t2 = 0;
                    parse_pair(line + 5, &t1, &t2);
                    if (t1 > 0) {
                        s->time_lo = t1;
                        s->time_hi = t2;
                    }
                    s->field_other++;
                } else if (strncmp(line, "time_scale", 10) == 0) {
                    /* PR-110 IMPLEMENTS this, it used to be counted.
                     * The engine stores the value at rec+0xe (its row-
                     * node ctor 0x51660 takes it as an argument) and
                     * multiplies EVERY row threshold by it at parse
                     * time: 0x51679 `mov ebp,[esi+0x10]` (the %hi
                     * value just read), `imul edx,ebp`, 0x51683/
                     * 0x5168d `idiv 1000`, 0x5168f `mov [esi+0x10],
                     * eax` - `threshold = value * time_scale / 1000`,
                     * and only rows constructed AFTER the line saw it
                     * (PR-104's ctor-order reading). The census of all
                     * 777 .MOV: 116 active `time_scale=` lines in 38
                     * files, and not one row sits before its line, so
                     * scale_at records the position rule without
                     * changing any shipped result; a commented line
                     * (`;time_scale=`) never matches. The values run
                     * 60..4000. Applied to the block at the end of
                     * ll_mov_parse below. The value starts right after
                     * the '=' (the shipped form is `time_scale=700` and
                     * its 16 spellings, every one of them `=`-joined). */
                    const char *sv = strchr(line, '=');

                    s->time_scale = sv ? (int)strtol(sv + 1, NULL, 10) : 1000;
                    s->scale_at = s->nfr;
                } else if (line[0] != ';' && strchr(line, '=')) {
                    /* state payload fields the model does not act on yet:
                     * sample= (487, sound ids). Counted, never dropped in
                     * silence (PLAN 8g).
                     * PR-102 RE note, no behaviour change here: the
                     * engine stores `time=N,M` as two 16-bit halves at
                     * record +0x8/+0xa and `time_scale` at +0xe
                     * (parser 0x51910), and every row's threshold is
                     * then pre-scaled at parse time by
                     * `T * time_scale / 1000` (0x51660). PR-110 CORRECTS
                     * the census sentence PR-102 left here ("all 120
                     * shipped values are multiples of 100, 400..2000"):
                     * 120 lines is right, but 4 of them are commented
                     * out (`;time_scale=`), and the 116 active values
                     * run 60..4000, 26 distinct values, 78 of them
                     * BELOW 1000 - the shipped scale mostly makes rows
                     * SHORTER, and it is not all round hundreds (150,
                     * 550, 650, 750, 820, 850 all appear). See the
                     * time_scale= branch above, modelled in PR-110.
                     * So this counter is an unmodelled SCALE on frame
                     * timing, not just an unused field. */
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
        /* PR-110: apply the block's time_scale to its row thresholds at
         * parse time, as the engine does inside each row node's ctor
         * (0x51679 imul / 0x5168d idiv 0x3e8): only rows read AFTER
         * the `time_scale=` line are scaled (scale_at). KADMAR.07's
         * balloon: authored 200 with time_scale=2000 stores 400, i.e.
         * FOUR iterations per row, not two. */
        for (int r = s->scale_at; r < s->nfr; r++)
            s->fr[r].thresh = (unsigned)((long)s->fr[r].thresh *
                                         s->time_scale / 1000);
        for (int t = 0; t < s->ntr; t++)
            s->tr[t].tidx = ll_mov_find(m, s->tr[t].target);
    }
    if (m->first_decl[0]) {
        int k = ll_mov_find(m, m->first_decl);
        m->start_st = k > 0 ? k : 0;
    }
    /* PR-216: the engine's state index of each block, [obj+0x5c]
     * while it runs = 1 + the position of its name in the declared
     * list (0x53440 returns exactly that, 0 when the name is not in
     * the list, and the loader stores the block's record at that slot
     * of [obj+0x50]: 0x534b6). Block order is NOT that order: 172 of
     * the 777 shipped scripts differ. */
    for (int i = 0; i < m->nst; i++) {
        int k = 0;

        for (int d = 0; d < m->nddecl; d++)
            if (strcasecmp(m->decl[d], m->st[i].name) == 0) {
                k = d + 1;
                break;
            }
        m->st[i].decl_idx = k;
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

/* PR-97: see the ll_player.contact note. The viewer calls this with the
 * shape-box bottom of the animation it is drawing, every tick. */
void ll_player_set_contact(ll_player *p, int below_pos)
{
    p->contact = below_pos > 0 ? below_pos : 0;
}

static int cond_true(const ll_player *p, const ll_play_keys *k,
                     const ll_mov_tr *tr)
{
    int any = k->k_fwd || k->k_bkwd || k->k_up || k->k_down ||
              k->k_jump || k->k_fire || k->k_f1 || k->k_f2 ||
              k->k_f3 || k->k_f4 || k->k_alt;
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
    for (int i = 0; i < tr->nc; ) {                 /* PR-87: OR of AND- */
        int g = tr->c[i].grp, all = 1;              /* groups ('|' then  */
        for (; i < tr->nc && tr->c[i].grp == g; i++) { /* ',')           */
            int v;
            switch (tr->c[i].cond) {
            case LL_COND_K_FWD:  v = fwd_rel;  break;
            case LL_COND_K_BKWD: v = bkwd_rel; break;
            case LL_COND_K_UP:   v = k->k_up;   break;
            case LL_COND_K_DOWN: v = k->k_down; break;
            case LL_COND_K_GEN1: v = (p->sig >> LL_SIG_K_GEN1) & 1; break;
            case LL_COND_K_GEN2: v = (p->sig >> LL_SIG_K_GEN2) & 1; break;
            case LL_COND_K_GEN3: v = (p->sig >> LL_SIG_K_GEN3) & 1; break;
            case LL_COND_K_GEN4: v = (p->sig >> LL_SIG_K_GEN4) & 1; break;
            case LL_COND_K_GEN5: v = (p->sig >> LL_SIG_K_GEN5) & 1; break;
            case LL_COND_K_ANY:  v = any;       break;
            case LL_COND_K_JUMP: v = k->k_jump; break;
            case LL_COND_K_FIRE: v = k->k_fire; break;   /* PR-87 */
            case LL_COND_K_F1:   v = k->k_f1;   break;
            case LL_COND_K_F2:   v = k->k_f2;   break;
            case LL_COND_K_F3:   v = k->k_f3;   break;
            case LL_COND_K_F4:   v = k->k_f4;   break;
            case LL_COND_K_ALT:  v = k->k_alt;  break;
            case LL_COND_AC_BREAK_NEXT: v = p->broke; break;
            case LL_COND_AC_TIME: v = p->ac_time; break;  /* PR-83: player-side
                 * arm = the frame list wrapped (ZARIA A_WALK->A_DRAP idle chain) */
            case LL_COND_OTHER:
                v = tr->c[i].sig >= 0
                    ? (int)((p->sig >> (unsigned)tr->c[i].sig) & 1)
                    : 0;               /* PR-88: a bare signal NAME (AC_ONAIR,
                                        * AC_EDGE, ...) tests the lion's own
                                        * set - false until something raises
                                        * it; an unknown token stays never-true
                                        * OTHER (counted, not guessed) */
                break;
            default:             v = 0;         break;   /* not simulated */
            }
            if (tr->c[i].neg)
                v = !v;
            if (!v)
                all = 0;
        }
        if (all)
            return 1;
    }
    return 0;
}

/* PR-74: entering a frame that carries `I ix,iy` throws the lion: the
 * horizontal part follows his facing, the vertical part is up (negative).
 * PR-98 RE (no change): the engine does NOT set the velocity here. That
 * row's `I` pairs are summed (0x5273d) into a target that 0x54170 moves v
 * to and never past - a SET from rest or the same sign, a climb of |target|
 * per tick from the opposite sign - and the pair runs after the T= drag, so
 * it holds that axis for as long as the row runs. So a one-shot assignment
 * is the port's model and its /4 divisor is a fitted unit, not an
 * instruction (the engine's scale is unresolved: NEXT_STEPS PR-98). */
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

/* PR-86: does this state declare an AC_BREAK_NEXT exit of its own? The
 * frame-list wrap arms that condition (PR-83), so such a state ENDS BY
 * ITSELF and the landing must not force it out; a state without one
 * (minimal fixtures) keeps the FALLBACK viewer rule below the jump. */
static int st_has_break_next_exit(const ll_mov_st *st)
{
    for (int t = 0; t < st->ntr; t++) {
        if (st->tr[t].tidx < 0)
            continue;
        for (int i = 0; i < st->tr[t].nc; i++)
            if (!st->tr[t].c[i].neg &&
                st->tr[t].c[i].cond == LL_COND_AC_BREAK_NEXT)
                return 1;
    }
    return 0;
}

/* PR-135: ONE STATE STEP + ONE PHYSICS PASS per frame-loop iteration
 * (PR-133's chain: the state-step chain dispatches the record once per
 * iteration and these families pass at most once). ll_player_step below
 * is only the clock: it feeds dt_ms into the iteration accumulator and
 * runs this body once per completed LL_ITER_MS, carrying the remainder
 * Bresenham-style exactly like PR-110's row clock did - so the port is
 * grid-independent (dt == LL_ITER_MS is one step; 2*50+16 is two; the
 * selftests pin both). */
static int player_iterate(ll_player *p, const ll_plt *ground,
                          const ll_play_keys *keys, double xmin, double xmax)
{
    const ll_mov_st *st = &p->mov->st[p->sti];
    int ev = 0;
    double dt = (double)LL_ITER_MS / 1000.0;      /* exactly one iteration */

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
         * the STATE running untouched - no ENTER, no impulse, no event -
         * but PR-215 ships the one thing the engine's arm DOES do: it
         * writes the authored start row into the CURRENT state's row
         * counter. The applier's same-index entry (0x53602-0x53626,
         * re-read by PR-174 for the object side) opens with exactly
         * that - `mov esi,[eax+0x4]` (0x5360f), the skip-if-equal at
         * 0x53612, `mov [eax+0x4],edx` (0x53616) - and PR-172 settled
         * that this arm inlines ONLY the row write: the row feed and
         * its carry [obj+0xa1] survive the match (p->row_t, p->facc
         * untouched). The value is the raw authored one and the shared
         * ll_mov_row_start mapping is the engine's observable
         * behaviour, absent token and over-range included (PR-107).
         * WHY THIS IS THE ZARIA HALF: PR-174's own census counted the
         * 101 authored self-target seeks with entry >= 2 as 77
         * LION-class and 24 scene-object lines, and all 20 of
         * ZARIA.01's are in the lion's own scripts (A_BORRING(10),
         * A_DEAD(33), A_JUMP(11)=AC_DUP, A_KILOF(13)/(25)...) - the
         * object-side fix could not reach one of them. OI-092.
         * (Note the (...) token takes non-numeric values too: "(x)" -
         * PR-86 decoded the NUMERIC one as the start row, see
         * ll_mov_tr.entry. An ABSENT or non-numeric token is the
         * engine's -1 and the row-set method refuses a negative value
         * (0x52381): NO seek. PR-215's census settles it: 19 of the
         * 22 authored self-target lines that a HELD direction key
         * re-matches every iteration carry no numeric token, and the
         * video-measured walk gait does not restart its row list.) */
        if (st->tr[t].tidx == p->sti && st->tr[t].entry > 0)
            p->fi = ll_mov_row_start(&p->mov->st[p->sti],
                                     st->tr[t].entry);
        if (st->tr[t].tidx >= 0 && st->tr[t].tidx != p->sti) {
            p->sti = st->tr[t].tidx;
            const ll_mov_st *nt = &p->mov->st[p->sti];
            p->fi = ll_mov_row_start(nt, st->tr[t].entry);
                                     /* PR-107: the shared rule, now the same
                                      * one the scene objects use */
            p->row_t = 0;
            p->facc = 0.0;             /* PR-110: a state change zeroes
                                        * both feeds of the accumulator,
                                        * as obj_transition does */
            p->atc = 0;                /* PR-89: an attack window belongs *
                                        * to the state that opened it.    */
            p->atc_hit = 0;
            ev |= LL_PLAY_EV_STATE;
            apply_impulse(p, nt, p->fi);
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
    p->sig = 0;                                      /* one-tick pulses */
    p->broke = 0;
    p->ac_time = 0;

    /* HISTORICAL RECORD OF THE FITTED MODEL (the model in force from
     * PR-79 until PR-135; the code that implements the engine's own law
     * is the block below, marked "2) horizontal physics: the ENGINE
     * LAW"). It is kept here because the paragraphs below carry the RE
     * addresses and the old measurements, and PR-98/99's corrections of
     * each other are only readable in order.
     *
     * 2) horizontal physics from the real F=/T= numbers: approach the
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
     * CHANGE_LEVEL) keep walking with no key held.
     * PR-98 (RE, no behaviour changed): the ENGINE does not parameterize
     * these two numbers this way. Its velocity step at 0x54250 is, per tick,
     *   v -= state T= * v / 100      (a PERCENT drag, computed from the OLD
     *                                 v first; a sign flip across the
     *                                 update sets the component to 0;
     *                                 0x54266 also floors the x drag at
     *                                 20, and the .MOV parser caps the
     *                                 authored T= at 130 - 172 shipped
     *                                 components are above that cap)
     *   v += (state F= + that row's `FC` pairs, + its `IC` pairs when
     *         obj+0x95 is set) * [obj+0xba|be] / 20 + state G= * facing
     * and separately that row's `F`/`I` pairs (summed at 0x526df..0x5273d)
     * are a velocity TARGET: 0x54170 moves v to it and never past it (from
     * rest or the same sign it is a SET, from the opposite sign it climbs by
     * the target per tick until it lands on it), and it runs AFTER the drag,
     * so a row that carries a target pins that axis for as long as it runs.
     * x is multiplied by facing; y is not, and y is DOWN-positive there -
     * the jump rows aim `F 0,350` (falling) and `I 150,-1200` (upward).
     * So F= is a sustained force, T= is a drag, and `FC` is load-bearing:
     * ZARIA's A_FIGHT header is `F=0,0` and its rows carry `FC 100,0`,
     * `FC 90,0`..., i.e. without FC the punch has no authored motion at all.
     * NOT APPLIED then, deliberately: PR-98 and PR-99 carry the detail,
     * and the next lines are the reason. [APPLIED IN PR-135 - what was
     * missing here was the measurement the paragraph below asks for, a
     * BY_X object at T other than 40 on camera; PR-133/134 supply it.
     * See the engine-law block below and NEXT_STEPS PR-135.] That
     * loop's steady state is v_eq = 5*F_eff*s/T, so px/s =
     * F_eff*s*fps/(20*T) - a speed that depends on T, on the terrain
     * scalar s and on a tick rate this port does not run at (60 ms) -
     * while the walks this repo measured appeared to track F alone: 25
     * px/s for the lion's A_WALK (F=100,T=20), 25 px/s for the worm's
     * BY_X A_RUN (F=100,T=40), 40-50 px/s for the rhino (F=180,T=40);
     * PR-99 corrects what that proves, below. Note also that
     * the units half of the question may already be answered by this
     * file's own fitted constants: v is hundredths of a px per tick (the
     * /100 at 0x5435d), and at 25 such ticks/s an `I 150,-1200` pair is
     * (37.5, -300) px/s - exactly what LL_PLAY_JUMP_DIV=4 below produces.
     * What is now settled, from the image itself (0x52590 parses the
     * header keys): `F=` = rec+0x50/+0x54, `T=` = +0x58/+0x5c, capped by
     * that parser at 130, `G=` = +0x48/+0x4c - and the merge copies
     * exactly those, so the header F= feeds ONLY the increment channel
     * (0x54170's target comes from the row-node sums, never from the
     * header). That much stands. What does NOT stand is PR-98's word
     * "refuted" - PR-99 corrects it (NEXT_STEPS PR-99, CORRECTION): the
     * two OBJECT samples share T=40, where this law and the F-only law
     * differ only by the constant PR-79 fitted as /4, and the T=20
     * sample is the lion, whose path through 0x54250 is unverified.
     * Worked the other way (s=10, 20 ticks/s) the same algebra lands on
     * both object samples AND their per-row distances. PR-99 also read
     * the `control=` dispatch, PR-98's open item 2: it is 0x543a0 (the
     * address PR-99 printed as 0x543a1; PR-100 corrects it from the
     * aligned prologue and the method-table entries at slot +0xa0) with
     * the jump table at 0x54380, mode 0 is BY_X and its branch feeds
     * the row distance accumulator [obj+0xa1] with [obj+0xca], i.e.
     * |v_x|/100 from the loop above. PR-100 has that feed's owner: the
     * state record's step at 0x521f0 calls this dispatcher as slot
     * +0xa0 of its owner object's method table, then compares the
     * current row's authored threshold with [obj+0xa1], admits at most
     * one row per step and hands the remainder to the row handler -
     * the same shape obj_walk/obj_advance have, with a different
     * per-tick feed. There is no separate position
     * mechanism, and PR-98's other escape (re-reading the header `F=`
     * as 0x54170's velocity target) stays closed: that target is built
     * only from the row-node sums. So this model stayed as it was: the
     * samples on hand did not decide between the two laws, and the
     * measurement that would is a BY_X object at T other than 40,
     * which the authored data supplies in quantity but ZARIA's footage
     * did not. [RESOLVED IN PR-133/134: that footage does put a BY_X
     * worm at T=40 and a lion stand-in at T=20 on camera, and their
     * rates decide in favour of this law; PR-135 applies it.] */
    /* 2) horizontal physics: the ENGINE LAW, one pass per iteration
     * (PR-135; the RE that describes it is PR-98's, quoted in the
     * historical note below and confirmed byte-for-byte by PR-129/132/
     * 133: the velocity step 0x54250 is, per iteration,
     *   v -= state T= * v / 100      (a PERCENT drag computed from the
     *                                 OLD v; T is the engine's drag
     *                                 percent word and 0x52590 clamps
     *                                 it at PARSE into [20,130]; a sign
     *                                 flip across the update sets the
     *                                 component to 0 - 0x5426x)
     *   v += (state F= + that row's FC pairs ...) * s / 20
     * with v in hundredths of a px per iteration and s the intent
     * factor at [obj+0xba], |s| <= 10, s = +/-10 exactly at a full
     * one-axis intent (PR-132, CLOSED). Steady state is therefore
     * v_eq = 5*F*s/T hundredths/iteration = F*s*1000/(20*T*LL_ITER_MS)
     * units/s - 50 u/s for ZARIA's [A_WALK] (F=100,T=20,s=10) and
     * 25 u/s for the worm's (F=100,T=40), inside PR-134's measured
     * brackets [38,76] and <=30.3 on the verified 50 ms clock.
     * The OLD model (PR-79/83: a target velocity of +/-T capped by a
     * clamp and an F-as-brake accel limit) is REPLACED here: T is the
     * drag, not a velocity cap, and the brake on key release IS the
     * drag with s = 0 (A_STAY's T=90 stops in a few iterations;
     * A_WALK's T=20 glides - the engine has no authored hard brake
     * for it). The row-node `F I` velocity-TARGET channel (0x54170)
     * and the FC/IC sums stay unmodeled, as before this PR (0x54170's
     * target is built only from the row-node sums - PR-98/99).
     * PR-98/99's full historical note, kept where it was written:
     * the merge copies the header F=/T=/G= into the record fields
     * [obj+0x50..0x5c] and the header F= feeds ONLY the increment
     * channel; the samples that once "refuted" the F/T law all share
     * T=40, where it and the F-only fit differ only by the constant
     * PR-79 fitted as /4.
     * PR-83 (user-reported: ZARIA kept walking forever after one
     * press): a key-driven BY_X state only gets its intent while the
     * matching key is held; auto_walk states (K_GENn cutscene
     * walkers: GO_TO_BALON_*, EXIT, CHANGE_LEVEL) walk with s at
     * full intent and no key. */
    int key_walk = p->walkdir > 0 ? keys->k_fwd : keys->k_bkwd;
    int s = ((st->ctrl & LL_CTRL_BY_X) && (key_walk || p->auto_walk))
                ? (p->walkdir < 0 ? -LL_PLAY_INTENT_FULL
                                  : LL_PLAY_INTENT_FULL)
                : 0;                       /* no key held: the engine's
                                            * [obj+0xba] intent IS zero,
                                            * so no force is added and
                                            * the drag does the braking */
    {
        int td = (int)st->tx;
        if (td > 130)                       /* 0x52590 caps T at PARSE */
            td = 130;
        /* No lower clamp is modeled: the engine's raise of T to >= 20
         * is conditional ("only when the +0xbc guard fires", PR-132's
         * reading of PR-129 step 1) and WHICH objects carry that guard
         * is still open, so an authored `T=0` - no drag word at all -
         * stays no drag here. Counted as an open item, not guessed. */
        double d = (double)p->vx * (double)td / LL_PLAY_DRAG_DEN;
        if ((p->vx > 0.0) != (p->vx - d > 0.0) ||
            (p->vx - d > -LL_PLAY_V_EPS && p->vx - d < LL_PLAY_V_EPS))
            p->vx = 0.0;         /* sign flip across the update, or what
                                  * is left is below the ONE unit the
                                  * engine's integer register can hold
                                  * (0x5426x zeroes the component the
                                  * same way); see LL_PLAY_V_EPS */
        else
            p->vx -= d;
    }
    p->vx += (double)st->fx * (double)s / LL_PLAY_DRAG_DEN;
                                            /* the engine adds F*s/20 in
                                             * its own units, hundredths
                                             * of a px per iteration; at
                                             * LL_ITER_MS = 50 that is
                                             * F*s/100 units/s per
                                             * iteration, and with the
                                             * drag above the steady
                                             * state comes out at
                                             * F*s/T units/s either way
                                             * of how the axes are
                                             * labelled */
    if (p->vx > 20000.0)                   /* the engine's own guard has */
        p->vx = 20000.0;                   /* no cap in this chain; this */
    else if (p->vx < -20000.0)             /* one only bounds runaway    */
        p->vx = -20000.0;                  /* authored F/T (no data does)*/

    /* PR-148: the engine's y-intent word, [obj+0xbe], set every
     * iteration from the held Up/Down keys exactly where 0x549f0 (the
     * frame loop's per-frame call of the lion's input method, 0x4aeca,
     * PR-147) stores the input block's word1 into it: scancode 0x248
     * (Up) writes the negative value, 0x250 (Down) the positive one -
     * y+ is DOWN, PR-99's sign, re-confirmed at byte level by the
     * capture reader 0x582f0. The magnitude question is named at
     * LL_PLAY_INTENT_FULL (the keyboard's own constant is 20, the
     * contact chain writes 10, the port's one validated scale is 10).
     * The contact-node override (the kind-4 arm, 0x54b16) is PR-149's
     * route. The consumer modelled here is section 7's mode-4 row
     * feed guard (the engine's 0x5441b); the y-FORCE path (this word
     * into v_y, 0x542cd) is counted open in the PR-148 record and
     * not modelled: no shipped leg exercises it and the engine's own
     * parameter word at [ebx+0x4] is unread there. */
    p->vert_intent = keys->k_up ? -LL_PLAY_INTENT_FULL
                                : (keys->k_down ? LL_PLAY_INTENT_FULL : 0);

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
    double dxv = p->vx * dt;              /* the distance THIS step moves */
    p->x += dxv;
    p->y += p->vy * dt;
    /* PR-110: the row accumulator's feed depends on the state's control
     * mode, and this is the other half of it. The engine's dispatcher
     * 0x543a0 (PR-98 item 2, its owner read by PR-100) feeds mode 0 =
     * BY_X from |[obj+0xca]|, the distance the step travelled, and the
     * time modes from the level's 100-per-iteration constant;
     * obj_walk + obj_advance have modelled exactly that split on the
     * scene-object side since PR-98 (`walk ? facc : ...`). The player
     * side had only the time half, which was harmless while the clock
     * ran ms and is not harmless now: a real BY_X walk list (RAN.01
     * 12 rows x 6, ZARIA.01 the same) totals 66 units, BELOW the 100
     * one iteration feeds, so on the iteration feed alone it would
     * cycle inside a single iteration. Both accumulators run here, and
     * section 7's gate picks by mode, as obj_advance's does. The feed
     * is the velocity's own distance, before the bounds clamp in (6) -
     * [obj+0xca] is the velocity register, not the post-clip position. */
    if ((st->ctrl & LL_CTRL_BY_X) && st->fx != 0)
        p->facc += dxv < 0 ? -dxv : dxv;

    /* 5) ground logic (.plt lines, the shape bottom on top).
     * PR-97: the tested point is `y + p->contact` - the contact line the
     * viewer resolved for the animation being shown (ll_player.contact,
     * ll_seq_shape_contact) - not the position point. DATA: the frame
     * anchors of an animation are authored so that its shape-box bottom sits
     * at one fixed offset below the position point over the whole cycle -
     * ZARIA LEW.ZAR WALK.SEQ 12 frames, f2 = 31 on every one and the shape
     * box bottom at pos+30..+32; the same lion's END.SEQ runs f2 from 5 to
     * 54 while the bottom stays at EXACTLY pos+58; ZARIA's worm WAZ.SEQ
     * swings f0 -2..-22 and f2 -9..-16 with its bottom fixed at pos+46..49.
     * Only the shape box is that stable, and the game's own terrain-side
     * geometry (the hit test FUN_00042a70, the push-out FUN_00042de0) is
     * shape-box geometry, so the contact line is the shape bottom.
     * The view-side arithmetic against the original: ZARIA.01's camera clamp
     * is cam_ymax 500 and its .plt main ground is world y=680-684, so that
     * line renders at view row 180 - the paw line PR-84 measured on the video
     * (paws at 175..180). Parking the SHAPE BOTTOM there keeps the lion the
     * same height as the original. Parking the POSITION POINT there (what
     * this port did until now) puts the same shape bottom at world 710 =
     * view 210, outside the 200-row view: the "Lew is below the grass, his
     * legs are cut" report. */
    int xi = (int)(p->x >= 0 ? p->x + 0.5 : p->x - 0.5);
    int cont = p->contact > 0 ? p->contact : 0;
    double ground_y = p->y + (double)cont;     /* where this frame touches */
    int gyi = (int)(ground_y >= 0 ? ground_y + 0.5 : ground_y - 0.5);
    if (ground) {
        if (p->grounded && (st->ctrl & LL_CTRL_CHECK_LN)) {
            ll_xy hit;
            if (ll_plt_surface_at(ground, xi, gyi, LL_PLAY_STEP, &hit) &&
                p->vy >= 0.0) {
                /* PR-135: this snap now also requires a body that is
                 * not moving up - the same sign test the landing branch
                 * below already makes, and the same reason the engine's
                 * ground contact is a CONTACT and not a leash: its
                 * purpose here is slope following and the bounding
                 * curbs, and every real .plt case in the measured set
                 * is a body travelling ALONG a surface, not away from
                 * it. The hazard that motivated it is arithmetic: on a
                 * 50 ms grid one take-off iteration moves 15 px
                 * (vy = -300 u/s), inside this branch's 16 px
                 * tolerance, so a snap that ignored the direction would
                 * glue a jumping lion back to the floor.
                 * HONEST LIMIT, from PR-135's own mutation run: NO leg
                 * in this repo distinguishes the guard. Dropping it
                 * leaves both selftests green AND the whole of
                 * verify-play and verify-level green
                 * (/tmp/opencode/pr135/mut2/M12_play_no_vy_guard.*.log).
                 * The reason is structural: a row's upward `I` clears
                 * p->grounded as it is applied (apply_impulse), and a
                 * body in the air does not reach this branch at all, so
                 * `grounded && vy < 0` is not a state this model can
                 * currently produce. The guard is therefore defensive
                 * bookkeeping, counted as an open item; the fixture
                 * that would pin it is a CHECK_LN state that is walked
                 * off a step edge while a row's `I` points up. The
                 * second guard below, on `p->support`, is NOT covered by
                 * that mutation either. */
                p->y = (double)hit.y - (double)cont;  /* slope following */
                p->vy = 0;
            } else if (p->support && p->vy >= 0.0) {
                p->vy = 0;                    /* standing on a solid object */
            } else {
                p->grounded = 0;
                p->air_from = p->y;
                ev |= LL_PLAY_EV_AIR;
            }
        } else if (!p->grounded) {
            /* Landing search. The start row sits LL_PLAY_STEP ABOVE the
             * contact line (y-down: minus = up): a CHECK_LN walker that
             * briefly left the ground at a line END and sank a few px below
             * the continuation line (real RAN.PLT: shelf 126..302@314 ->
             * 323..485@313 across a 20 px gap) steps back UP onto it -
             * symmetric with the step-down tolerance in the grounded
             * branch. A fall from higher up is unaffected: its surface
             * is below the contact line either way. */
            ll_xy hit;
            /* the old code truncated away from zero here (one px lower
             * than the grounded branch's nearest rounding for a negative
             * y); keeping that difference, only the tested point moved */
            int start = (int)(ground_y >= 0 ? ground_y : ground_y - 1.0);
            if (p->vy >= 0 && ll_plt_first_below(ground, xi,
                                                 start - LL_PLAY_STEP, &hit) &&
                ground_y >= (double)hit.y - 1.0) {
                p->y = (double)hit.y - (double)cont;
                p->vy = 0;
                p->grounded = 1;
                p->sig |= 1ull << LL_SIG_K_GEN2;        /* landing pulse  */
                ev |= LL_PLAY_EV_GROUND;
                if (p->jumping && !st_has_break_next_exit(st)) {
                    /* FALLBACK viewer rule ONLY when the state declares
                     * no AC_BREAK_NEXT exit of its own (the JM-style
                     * minimal file). PR-86: the authored jump states DO
                     * declare one - ZARIA/TUNGA's A_JUMP ends with
                     * A_WALK(7)=AC_BREAK_NEXT, A_JUMP_S with
                     * A_DRAP(1)=AC_BREAK_NEXT - and PR-86 decoded the
                     * (N) token as the target's START ROW, which makes
                     * those lines re-join the walk cycle MID-PHASE (row
                     * 7 of ZARIA's 12-row cycle) and the PRE_JUMP route
                     * skip its own impulse row. The old "landing ends
                     * the jump state, go idle" reading contradicted the
                     * data (no authored jump exits A_STAY anywhere); it
                     * stays only as this fallback. The jump frame-list
                     * WRAP arms AC_BREAK_NEXT (PR-83 mechanism) a beat
                     * after the physics landing - the long recovery rows
                     * (A_JUMP row 6: 900 ms) then play out inside the
                     * state, as authored. Whether the original cancels
                     * the recovery early (the undecoded `WAIT 0ms` row +
                     * `AC_DUP`) stays OPEN - see HANDOFF 7.8.
                     * jumping=0 stays cleared here either way: the flag
                     * only selects LL_PLAY_JUMP_G while airborne, and
                     * post-landing rows must fall on the state's own G. */
                    p->jumping = 0;
                    int idle = ll_mov_find(p->mov, "A_STAY");
                    if (idle < 0)
                        idle = ll_mov_find(p->mov, "A_DRAP");
                    if (idle >= 0 && idle != p->sti) {
                        p->sti = idle;
                        p->fi = 0;
                        p->row_t = 0;
                        p->facc = 0.0;
                        p->vx = 0;
                        ev |= LL_PLAY_EV_STATE;
                        st = &p->mov->st[p->sti];   /* step 7 uses the NEW state */
                    }
                } else if (p->jumping) {
                    p->jumping = 0;   /* authored exit pending: clear the
                                       * flag, keep the state running    */
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

    /* 7) frame advance on the ITERATION clock (PR-110; same semantics
     * here as obj_advance: wrap at the end, apply M dx,dy deltas;
     * AC_BREAK_NEXT is armed by a wrap and consumed by transitions next
     * tick). PR-109 measured the unit: the authored row numbers are
     * thresholds in 1/100-iteration units (the engine feeds its row
     * accumulator exactly 100 per frame-loop iteration, 0x4c8b7, and
     * its pacer 0x4b0e0 -> 0x4a120 -> 0x1dba0 holds one iteration to
     * two display frames), and on the owner's ladder clip the rows of
     * 200/300 ran 66-100 ms, NOT the 200/300 ms the old wall-ms clock
     * gave. So `row_t` now counts the same 1/100-iteration units the
     * thresholds are in, fed in whole batches of 100 whenever
     * `row_rem` - the same millisecond grid as before - completes an
     * iteration of LL_ITER_MS. The compare stays AFTER the add and the
     * threshold is subtracted on a hit, which is the engine's rule
     * (0x521f0 compares [obj+0xa1] with the current row's node+0x10
     * and the accumulator keeps the remainder): a row of 200 is
     * current for exactly 2 iterations whatever the step size, and
     * short rows (20, 40 - real A_JUMP rows) all land inside one
     * iteration, as they do there. The carry makes the port
     * grid-independent: at dt == LL_ITER_MS the feed is exactly 100
     * per step; at the CI's 20 ms grid it is 100 per 28 ms with the
     * leftover ms carried (Bresenham), never a drifted fraction.
     * The BY_X branch (PR-110, and the reason `walk` is in the gate):
     * the engine's dispatcher 0x543a0 feeds the SAME accumulator by
     * mode - mode 0 (BY_X) adds the distance travelled, |[obj+0xca]|,
     * and ONLY the time modes add 100 per iteration (PR-98 item 2,
     * PR-100's owner reading; ll_level.c's obj_walk/obj_advance have
     * modelled that split since PR-98). So a walking state's row clock
     * does not run at all - its rows are fed by section 4's distance
     * alone - and a time-driven state's distance accumulator stays 0.
     * One accumulator, one feed source, chosen by the state's mode:
     * that is the whole gate below.
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
        int walk = (st->ctrl & LL_CTRL_BY_X) && st->fx != 0;
        int by_ty = (st->ctrl & LL_CTRL_BY_TY) && !p->vert_intent;
        /* PR-148, the mode-4 feed guard, byte-exact at 0x5441b: the
         * dispatcher's BY_TY branch adds nothing while [obj+0xbe] is
         * zero, and the row compare runs on regardless - a BY_TY row
         * CLOSES ONLY ON ITERATIONS WHERE THE VERTICAL INTENT IS
         * HELD, and the accumulator carries the partial feed across
         * the gap exactly like the engine's [obj+0xa1]. The guard is
         * on the feed, not the admit. Not copied: the same branch's
         * cousin at 0x543f2 guards BY_TIME (mode 2) on the X-intent
         * word - PR-102 recorded mode 2 as unguarded, the model has
         * run that way through every video-verified leg since, and
         * PR-147's own reading of that guard's data source (word0 =
         * the arrows) makes it a behavioural regression study, not a
         * copy. Counted as open item 2 in the PR-148 record. */
        if (!walk && !by_ty)
            p->row_t += 100;          /* one iteration = exactly one
                                       * feed of the engine's constant
                                       * (0x4c8b7): no millisecond
                                       * arithmetic is left here, the
                                       * clock is ll_player_step */
        if ((walk ? p->facc : (double)p->row_t) >=
                (double)st->fr[p->fi].thresh) {
            /* AT MOST ONE ROW PER ITERATION (PR-100's reading of the
             * state record's step at 0x521f0: compare, subtract the
             * authored threshold, hand the remainder over, and the step
             * ends). With the feed of 100 per iteration that is what
             * makes a row of 6 (the real A_FIGHT and A_STAY lists) run
             * one pose per iteration = 50 ms, and a row of 200/300 run
             * 2-3 iterations = 100-150 ms, which is the 66-100 ms
             * PR-109 measured on the ladder clip (its own clock was
             * then fitted at 28 ms; at the 50 ms iteration 2-3 of them
             * is 100-150 ms, inside the same retrace design point).
             * The gait arithmetic closes on the video the same way:
             * the lion's 12 poses x 6 units at 2.5 units/iteration is
             * 2.4 iterations per pose = 120 ms, and PR-134 measured
             * 72-110 video frames per pose cycle = 120-183 ms; the
             * worm's 9 x 4 at 1.25 is 3.2 iterations = 160 ms per pose
             * = 1.44 s per cycle, PR-79's measured period exactly.
             * A row's own `dx,dy` delta, its `GEN` signals and its
             * ATC window are applied on the admit, as before. */
            if (walk)
                p->facc -= (double)st->fr[p->fi].thresh;
            else
                p->row_t -= st->fr[p->fi].thresh;
            p->x += st->fr[p->fi].dx;
            p->y += st->fr[p->fi].dy;
            unsigned long long gf = st->fr[p->fi].gsig;
            p->sig |= gf;                            /* PR-88: any named  */
            if (gf & (1ull << LL_SIG_AC_TIME))       /* signal raises on  */
                p->ac_time = 1;                      /* frame completion  */
            if (gf & (1ull << LL_SIG_AC_BREAK_NEXT))
                p->broke = 1;          /* TUNGA's A_FIGHT rows raise this
                                        * directly (`GEN AC_BREAK_NEXT`);
                                        * the frame-list wrap below keeps
                                        * arming it the PR-83 way. */
            unsigned char ac = st->fr[p->fi].atc;
            if (ac & LMOV_ATC_ON)                  /* PR-89: the frame's */
                p->atc = 1;                        /* `ATC_ON` opens the */
            if (ac & LMOV_ATC_OFF)                 /* attack-connect     */
                p->atc = 0;                        /* window. */
            /* PR-199: a row that carries `end_level <n>` is the engine's
             * apply method 0x45750 running on that node:
             * `mov ebx,ds:0xcef40; mov ecx,[node+0x8]; mov [ebx+0x4],
             * ecx`. The number goes to the level's exit-word slot and
             * NOTHING else happens - the apply ignores its second
             * argument and still walks the row's own chain, which is
             * what the two engine legs above already model. The value is
             * not examined here and never cleared here: PR-198 measured
             * that the loop body has no store to that slot and that the
             * slot's only test is `cmp [ecx+0x4],0x0` at the TOP of an
             * iteration, so the end lands one boundary after this admit. */
            if (st->fr[p->fi].endlv)
                p->exit_word = st->fr[p->fi].endlv;
            if (p->fi + 1 >= st->nfr) {
                p->fi = 0;
                p->broke = 1;
            } else {
                p->fi++;
                apply_impulse(p, st, p->fi);
            }
        }
    }
    return ev;
}

/* PR-135: the clock. dt_ms feeds an iteration accumulator of
 * LL_ITER_MS and each completed iteration runs one state step + one
 * physics pass (PR-133's rate answer); the remainder is carried
 * Bresenham-style so the port stays grid-independent - the fixed-grid
 * legs that step at dt == LL_ITER_MS get one step per call, a 116 ms
 * call gets two, and the 20/40 ms substeps of the live loop add up to
 * the same iteration boundaries. */
int ll_player_step(ll_player *p, const ll_plt *ground,
                   const ll_play_keys *keys, double xmin, double xmax,
                   unsigned dt_ms)
{
    int ev = 0;
    if (!p->mov || p->sti >= p->mov->nst || dt_ms == 0)
        return 0;
    p->row_rem += dt_ms;
    while (p->row_rem >= LL_ITER_MS) {
        p->row_rem -= LL_ITER_MS;
        ev |= player_iterate(p, ground, keys, xmin, xmax);
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

    /* The port's rate, pinned (ll_play.h): one state step per
     * LL_ITER_MS, the PR-133/134 answer for the frame-loop pacer.
     * Every fixed-grid leg here, the PPM probe and the live loop's
     * substeps use this constant, so a change to it is a change to the
     * port's speed and gait arithmetic, not a refactor - which is why
     * the header says this selftest asserts it. */
    CHECK(LL_ITER_MS == 50u);

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
    /* the self-target line parses too (its (..) token is the non-numeric
     * "x" - real KADMAR2 data - so entry stays 0 = top), and matches its
     * own state */
    CHECK(m.st[1].tr[1].entry == 0);                 /* "(x)": not numeric */
    CHECK(m.st[0].tr[0].entry == 1);                 /* "(1)": start row   */
    CHECK(m.st[1].tr[1].tidx == 1 &&
          m.st[1].tr[1].c[0].cond == LL_COND_K_FWD &&
          m.st[1].tr[1].c[0].neg == 0);
    CHECK(m.st[1].fr[0].thresh == 6);                /* authored values kept */

    /* PR-87: ',' groups are AND ('|' alternatives stay as before): the
     * unmodeled token poisons only its own group; a leading '|' is an
     * empty group (no-op). The reading of ',' is argued in ll_play.h. */
    ll_mov_t mfg;
    ll_mov_parse("A_STAY\nA_FIGHT\n[A_STAY]\r\n"
                 "A_FIGHT(1)=K_FIRE,K_F1\r\n"
                 "A_FIGHT(2)=K_F2|K_FIRE,K_ZOO\r\n"
                 "A_FIGHT(3)=|K_ALT\r\n", &mfg);
    CHECK(mfg.st[0].ntr == 3);
    CHECK(mfg.st[0].tr[0].nc == 2 &&
          mfg.st[0].tr[0].c[0].cond == LL_COND_K_FIRE &&
          mfg.st[0].tr[0].c[1].cond == LL_COND_K_F1 &&
          mfg.st[0].tr[0].c[0].grp == mfg.st[0].tr[0].c[1].grp);
    CHECK(mfg.st[0].tr[1].nc == 3 &&
          mfg.st[0].tr[1].c[0].grp != mfg.st[0].tr[1].c[1].grp &&
          mfg.st[0].tr[1].c[1].cond == LL_COND_K_FIRE &&
          mfg.st[0].tr[1].c[2].cond == LL_COND_OTHER);
    CHECK(mfg.st[0].tr[2].nc == 1 &&
          mfg.st[0].tr[2].c[0].cond == LL_COND_K_ALT);

    ll_mov_t m2;
    ll_mov_parse("X\n[A]\n0,0\n1,-5\n", &m2);
    CHECK(m2.nst == 1 && m2.st[0].fr[0].thresh == 100 &&
          m2.st[0].fr[1].thresh == 100);                 /* M4-C clamp */
    CHECK(strcmp(m.seqkey, "uj09") == 0);

    /* runtime: STAY -> WALK_FWD, the F/T steady state of the law, edge
     * fall, landing fires the K_GEN2 pulse, walk then reverse and stop.
     * PR-135 RE-DERIVED THE NUMBERS OF THIS LEG at the iteration grid
     * (one call = one iteration) and under the engine law:
     *   A_WALK_FWD is F=400, T=90 (the real RAN LEW.MOV values), so
     *   per iteration the force adds F*s/100 = 400*10/100 = +40 u/s and
     *   the drag takes T/100 = 90% of what the speed already is:
     *     v1 = 40, v2 = 40*0.1 + 40 = 44, v3 = 44.4, ... -> 400*10/90
     *   = 44.44 u/s, the law's own steady state (PR-134's lion ground
     *   run bracket [38,76] units/s, and inside it). At the 50 ms
     *   iteration that is 2.22 units of travel per step, and ST_PLT's
     *   first line ends at x=700 - 200 units from the spawn at 500 -
     *   so the edge arrives about 90 iterations in, inside the leg's
     *   400-step bound. */
    ll_plt pl;
    CHECK(ll_plt_parse(ST_PLT, &pl) == 0);
    ll_player p;
    ll_player_start(&p, &m, 500, 100, 1);
    CHECK(p.grounded && strcmp(m.st[p.sti].name, "A_STAY") == 0);

    ll_play_keys k = { 0 };
    k.k_fwd = 1;
    int ev = ll_player_step(&p, &pl, &k, 0, 2000, LL_ITER_MS);
    CHECK((ev & LL_PLAY_EV_STATE) && p.sti == 1);
    CHECK(p.face == 1);                                /* PR-82: walking right */
    int fell = 0, landed = 0, stev = 1, maxfi = 0;
    for (int t = 0; t < 400; t++) {                 /* walk until the pit */
        ev = ll_player_step(&p, &pl, &k, 0, 2000, LL_ITER_MS);
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
        /* the law's own ceiling: F*s/T = 400*10/90 u/s */
        CHECK(p.vx <= 400.0 * LL_PLAY_INTENT_FULL / 90.0 + 0.000001);
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
    /* the momentum kept across the fall IS the walking steady state:
     * the x physics runs every iteration, in the air as on the ground,
     * and 90 of them converged on F*s/T = 44.44 u/s */
    CHECK(p.vx > 400.0 * LL_PLAY_INTENT_FULL / 90.0 - 0.05);
    CHECK((p.sig >> LL_SIG_K_GEN2) & 1);               /* landing pulse */

    k.k_fwd = 0;
    ev = ll_player_step(&p, &pl, &k, 0, 2000, LL_ITER_MS);
    CHECK((ev & LL_PLAY_EV_STATE) && p.sti == 0);   /* !K_FWD -> STAY   */
    for (int t = 0; t < 60; t++)
        ll_player_step(&p, &pl, &k, 0, 2000, LL_ITER_MS);
    CHECK(p.vx == 0);          /* s = 0: the authored T=90 drag is the
                               * brake, and below one register unit the
                               * speed is zero (LL_PLAY_V_EPS) */

    k.k_bkwd = 1;
    ev = ll_player_step(&p, &pl, &k, 0, 2000, LL_ITER_MS);
    CHECK((ev & LL_PLAY_EV_STATE) && p.sti == 2 && p.walkdir == -1);
    CHECK(p.face == -1);                               /* PR-82: walking left  */
    double x0 = p.x;
    for (int t = 0; t < 20; t++)                    /* short reverse: stay
                                                     * on the lower shelf
                                                     * (edge sits at x=500,
                                                     * we land ~13 px past
                                                     * it) */
        ll_player_step(&p, &pl, &k, 0, 2000, LL_ITER_MS);
    CHECK(p.x < x0 - 10 && p.y == 150 && p.grounded);
    for (int t = 0; t < 40; t++) {                  /* frame cursor valid */
        int fi = p.fi;
        CHECK(fi >= 0 && fi < m.st[p.sti].nfr);
        CHECK(ll_player_frame(&p) >= 0);
        ll_player_step(&p, &pl, &k, 0, 2000, LL_ITER_MS);
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
    ev = ll_player_step(&p, &pl, &k, 0, 2000, LL_ITER_MS);   /* !K_BKWD -> A_STAY    */
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
    ev = ll_player_step(&p, &pl, &k, 0, 2000, LL_ITER_MS);
    CHECK((ev & LL_PLAY_EV_STATE) && p.sti == 2);    /* A_WALK_BKWD         */
    CHECK(p.walkdir == 1 && p.face == 1);            /* RIGHT arrow, CH_DIR */
    ev = ll_player_step(&p, &pl, &k, 0, 2000, LL_ITER_MS);
    CHECK((ev & LL_PLAY_EV_STATE) && p.sti == 0);    /* !K_BKWD now TRUE    */
    ev = ll_player_step(&p, &pl, &k, 0, 2000, LL_ITER_MS);
    CHECK((ev & LL_PLAY_EV_STATE) && p.sti == 1);    /* settled A_WALK_FWD  */
    CHECK(p.walkdir == 1 && p.face == 1);
    double x1 = p.x;
    for (int t = 0; t < 25; t++)                     /* 0.5 s: the flicker  */
        ll_player_step(&p, &pl, &k, 0, 2000, LL_ITER_MS);    /* the drag brakes */
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
    ev = ll_player_step(&p, &pl, &k, 0, 2000, LL_ITER_MS);   /* !K_FWD -> A_STAY    */
    CHECK((ev & LL_PLAY_EV_STATE) && p.sti == 0);
    k.k_bkwd = 1;
    ev = ll_player_step(&p, &pl, &k, 0, 2000, LL_ITER_MS);
    CHECK((ev & LL_PLAY_EV_STATE) && p.sti == 2);    /* A_WALK_BKWD tick    */
    ev = ll_player_step(&p, &pl, &k, 0, 2000, LL_ITER_MS);
    CHECK((ev & LL_PLAY_EV_STATE) && p.sti == 0);    /* !K_BKWD holds now   */
    ev = ll_player_step(&p, &pl, &k, 0, 2000, LL_ITER_MS);
    CHECK((ev & LL_PLAY_EV_STATE) && p.sti == 1);    /* settled A_WALK_FWD  */
    CHECK(p.walkdir == -1 && p.face == -1);          /* walking LEFT        */
    CHECK(ll_player_mirror_of(&p, 0) == 1);          /* mirrored again      */
    CHECK(ll_player_mirror_of(&p, 1) == 0);          /* authored mir -> plain   */
    ll_plt_free(&pl);
    {   /* PR-135 RE-DERIVED THIS LEG at the iteration grid (one call =
         * one iteration). The fixture is unchanged: `I 120,-1200` ->
         * vy = -1200/4 = -300 u/s and vx = +120/4 = +30 u/s (the
         * ESTIMATED /4 and LL_PLAY_JUMP_G = 1650 u/s^2 of PR-74, kept
         * exactly as shipped), and the upward component of the impulse
         * clears `grounded` at the moment it is applied - that is what
         * makes a jump leave the ground.
         * THE ROW PART runs on the iteration clock: A_JUMP's list is
         * 20,20,2000, and the first feed of 100 lands in the SAME
         * iteration that entered the state (transitions run first), so
         * row 0 closes and row 1's impulse fires right there; with one
         * row admitted per iteration the 20-unit rows are one
         * iteration each and the 2000-unit row never ends before the
         * landing ends the state (this fixture declares no
         * AC_BREAK_NEXT exit, so PR-86's fallback is the route).
         * THE ARC, per iteration: gravity is added, then integrated
         * (y grows down), 82.5 = LL_PLAY_JUMP_G*dt = 1650*0.05 is the
         * per-iteration velocity step, as the engine adds its G= per
         * iteration:
         *   step 0 (the call that enters JUMP): vy = -300, grounded = 0
         *   step 1: vy = -217.5, y = 89.125                        air 1
         *   step 2: vy = -135,   y = 82.375                        air 2
         *   step 3: vy = -52.5,  y = 79.75  <- APEX, 20.25 px up    air 3
         *   step 4: vy = +30,    y = 81.25                          air 4
         *   step 5: vy = +112.5, y = 86.875                        air 5
         *   step 6: vy = +195,   y = 96.625                        air 6
         *   step 7: vy = +277.5, y = 110.5 -> the landing test (vy >=
         *           0, first surface below the contact line) snaps y
         *           back to 100 and reports GROUND; the leg counts the
         *           step it lands on as grounded, so AIR = 6 steps and
         *           the first grounded step after the air is 7th call
         * Apex 20.25 px up is within 4% of the one jump PR-74 measured
         * (21 units) - closer than the 24.3 the 20 ms grid produced.
         * The /4 and 1650 fit stays as shipped; PR-134's "the vertical
         * was never re-fitted at this clock" stays counted open. */
        static const char JM[] =
            "A_STAY\nA_JUMP\n\n[A_STAY]\ncontrol=BY_TIME|CHECK_LN\n"
            "F=600,0\nT=90,90\nG=0,0\nA_JUMP(1)=K_JUMP\nseq=a\n1,100\n\n"
            "[A_JUMP]\ncontrol=BY_TIME\nF=180,0\nT=20,1\nG=0,55\nseq=a\n"
            "1,20\n2,20,I 120,-1200\n3,2000\n";
        static const char JP[] = "1\n1 0,100;2000,100;\n";
        ll_mov_t *jm = calloc(1, sizeof *jm);
        ll_plt jp;
        ll_player q;
        ll_play_keys jk = { 0 };
        double top = 100;
        int air = 0, back = 0;

        CHECK(jm && ll_plt_parse(JP, &jp) == 0);
        ll_mov_parse(JM, jm);
        CHECK(jm->st[1].fr[1].has_i && jm->st[1].fr[1].ix == 120 &&
              jm->st[1].fr[1].iy == -1200);
        ll_player_start(&q, jm, 500, 100, 1);
        jk.k_jump = 1;
        ll_player_step(&q, &jp, &jk, 0, 2000, LL_ITER_MS);       /* STAY -> JUMP */
        jk.k_jump = 0;
        CHECK(q.sti == 1);
        for (int t = 0; t < 80; t++) {
            ll_player_step(&q, &jp, &jk, 0, 2000, LL_ITER_MS);
            if (!q.grounded)
                air++;
            if (q.y < top)
                top = q.y;
            if (q.grounded && air > 3 && !back)
                back = t;
        }
        CHECK(top > 79.74 && top < 79.76);           /* apex = 100 - 20.25 */
        CHECK(air == 6);
        CHECK(back == 6 && q.sti == 0 && q.y == 100 && q.vx == 0);
        ll_plt_free(&jp);
        free(jm);
    }
    {   /* PR-86: the (N) token = START ROW (1-based) of the TARGET's
         * frame list, and a state that DECLARES an AC_BREAK_NEXT exit
         * ends by itself - the landing does not cut it (that was the
         * old fallback viewer rule). A state whose list runs LONGER than
         * the physics air keeps playing through the landing, and only
         * the list WRAP arms AC_BREAK_NEXT -> back to the walk state
         * mid-phase ("A_WALK(7)" after ZARIA's 12-row cycle).
         * PR-135 RE-DERIVED EVERY NUMBER here for the iteration grid,
         * dt = LL_ITER_MS: one call is one feed, one row admit.
         * Row timeline (leg step: what closes) - the entry call itself
         * feeds, because the transition runs first inside the iteration.
         * The authored rows are 75,75,75,100,150,150,225,900,225,0,150
         * and M4-C raises that 0 to 100, so the list is 2225 units:
         *   1 enter A_JUMP(1) = row index 0; closes row 0 (75)
         *   2 closes row 1 (75) -> row 2 becomes CURRENT, and its
         *     `I 150,-1200` impulse fires on that admit: vy = -300,
         *     vx = +37.5, grounded = 0
         *   3 closes row 2 (75); 4 closes 3 (100); 5 closes 4 (150);
         *   7 closes 5 (150); 9 closes 6 (225) -> the 900-unit row
         *     (index 7) holds steps 9..18
         *   18 closes it (900); 20 closes 8 (225); 21 closes 9 (100 =
         *     the M4-C clamp of the authored 0); 23 closes 10 (150)
         *     and the leftover 75 is kept as the debt against the next
         *     pass - the engine's state step admits ONE row per step,
         *     so the wrap lands on the NEXT step, 24 - which arms
         *     AC_BREAK_NEXT and the match comes at the top of step 24.
         * Cumulative units closed: 75, 150, 225, 325, 475, 625, 850,
         * 1750, 1975, 2075, 2225 - and the 75 still owed.
         * The jump physics are the same ESTIMATED units (PR-74, kept
         * as shipped): vy = -1200/4 = -300 u/s from the end of step 2,
         * g = 1650, so 82.5 u/s per iteration and the arc of the JM
         * leg above: apex 20.25 px up (y = 79.75) at step 5, the
         * landing search snaps him back at step 9 - INSIDE the 900-unit
         * row, exactly the PR-86 shape. air = the seven loop calls that
         * report grounded 0 (t = 0..6), land = t = 7 (leg step 9).
         * The state stays A_JUMP from there to step 24 - that is the
         * whole point of the leg, 15 steps of the authored jump list
         * after the physics landing. A_WALK(3) sets index 2 (row
         * `3,100`), and that same step's feed closes it, hence
         * exit_fi = 3. (The JM fixture above keeps covering the
         * FALLBACK branch: its A_JUMP declares no exit, so its landing
         * still goes idle.) */
        static const char JE[] =
            "A_WALK\nA_JUMP\n\n[A_WALK]\ncontrol=BY_TIME|CHECK_LN\n"
            "F=600,0\nT=90,90\nG=0,0\nA_JUMP(1)=K_JUMP\n"
            "A_WALK(3)=AC_BREAK_NEXT\nseq=a\n1,100\n2,100\n3,100\n4,100\n\n"
            "[A_JUMP]\ncontrol=BY_TIME\nF=180,0\nT=20,1\nG=0,55\nseq=a\n"
            "1,75,F 0,350\n2,75,F 0,350,smpauto\n2,75,I 150,-1200\n2,100\n"
            "3,150,FC 30,0\n4,150,GEN K_GEN1\n5,225,GEN K_GEN1\n"
            "6,900,GEN K_GEN1\n7,225,GEN K_GEN1\n8,0,WAIT AC_DUP\n"
            "9,150,F 0,100\n"
            "A_WALK(3)=AC_BREAK_NEXT\n";
        ll_mov_t *je = calloc(1, sizeof *je);
        static const char JP2[] = "1\n1 0,100;2000,100;\n";
        ll_plt jp2;
        ll_player q;
        ll_play_keys jk = { 0 };
        double top2 = 100;
        int air2 = 0, land = 0, held = 1, exit_t = 0, exit_fi = -1;
        int first_exit = 0;

        CHECK(je && ll_plt_parse(JP2, &jp2) == 0);
        ll_mov_parse(JE, je);
        CHECK(je->st[1].nfr == 11);
        CHECK(je->st[1].fr[2].has_i && je->st[1].fr[2].ix == 150 &&
              je->st[1].fr[2].iy == -1200);
        CHECK(je->st[1].fr[7].thresh == 900u);      /* the 900-unit middle */
        CHECK(je->st[1].fr[9].thresh == 100u);      /* authored 0 -> 100   */
        CHECK(je->st[1].tr[0].tidx == 0 && je->st[1].tr[0].entry == 3);
        ll_player_start(&q, je, 500, 100, 1);
        CHECK(q.sti == 0);                           /* no A_STAY: the
                                                      * declared first   */
        jk.k_jump = 1;
        ll_player_step(&q, &jp2, &jk, 0, 2000, LL_ITER_MS); /* WALK -> JUMP(1)  */
        jk.k_jump = 0;
        CHECK(q.sti == 1);
        CHECK(q.fi == 1);           /* the entry iteration's own feed of
                                     * 100 closes row 0 (75) there */
        for (int t = 0; t < 40; t++) {              /* t = leg step - 2    */
            ll_player_step(&q, &jp2, &jk, 0, 2000, LL_ITER_MS);
            if (!q.grounded)
                air2++;
            if (q.y < top2)
                top2 = q.y;
            if (q.grounded && air2 > 3 && !land)
                land = t;
            if (!first_exit && land && t >= land && q.sti != 1)
                first_exit = t;
            if (land && t >= land + 1 && !first_exit && q.sti != 1)
                held = 0;                            /* landing must NOT */
            if (!exit_t && q.sti == 0) {             /* first re-entry   */
                exit_t = t + 2;
                exit_fi = q.fi;
            }
        }
        CHECK(top2 > 79.74 && top2 < 79.76);         /* apex = 100 - 20.25 */
        CHECK(air2 == 7);
        CHECK(land == 7);                            /* landing, leg step 9 */
        CHECK(land && held == 1);                    /* stays authored   */
        CHECK(first_exit == 22 && exit_t == 24);     /* wrap arms the    */
        CHECK(exit_fi == 3);                         /* OWN exit at row  */
        CHECK(q.sti == 0 && q.grounded && q.y == 100);/* cycle (mid-phase)*/
        ll_plt_free(&jp2);
        free(je);
    }
    {   /* OI-092, PR-215: THE PLAYER-SIDE ROW SEEK. PR-174 shipped
         * the same-state match arm for scene OBJECTS; the engine arm
         * (0x53602-0x53626, re-read there) belongs to the record
         * applier and runs for the lion's own record exactly the same
         * way: the arm OPENS with the row write (`mov esi,[eax+0x4]`
         * 0x5360f, `cmp`, `je skip` 0x53612, `mov [eax+0x4],edx`
         * 0x53616) and calls NO ENTER, so the carry SURVIVES the
         * match (PR-172). PR-174's own census sized who this affects:
         * of the 101 authored self-target seeks with entry >= 2, 77
         * are LION-class lines and all 20 of ZARIA.01's are the
         * lion's own scripts (A_BORRING(10), A_DEAD(33), A_JUMP(11),
         * A_KILOF(13)/(25)...) - the object-path fix could not reach
         * a single one of them. So this is the half that can move
         * ZARIA. The port's player arm kept the state but never wrote
         * the row: the model choice this PR retires. */
        static const char RS[] =
            "A_ONE\nA_TWO\n\n[A_ONE]\ncontrol=BY_TIME\n"
            "F=600,0\nT=90,90\nG=0,0\n"
            "A_ONE()=K_GEN4\nA_ONE(3)=K_GEN2\nA_ONE(2)=K_GEN3\n"
            "A_ONE(9)=K_GEN5\nA_TWO(2)=K_GEN1\nseq=a\n"
            "1,70\n2,240\n3,70\n4,70\n5,70\n6,70\n\n[A_TWO]\n"
            "control=BY_TIME\nF=0,0\nT=0,0\nG=0,0\nseq=a\n"
            "1,300\n2,300\n";
        static const char RP[] = "1\n1 0,100;2000,100;\n";
        ll_mov_t *rm = calloc(1, sizeof *rm);
        ll_plt rp;
        ll_player q;
        ll_play_keys rk = { 0 };

        CHECK(rm && ll_plt_parse(RP, &rp) == 0);
        CHECK(ll_mov_parse(RS, rm) == 2 && rm->st[0].ntr == 5 &&
              rm->st[0].nfr == 6 && rm->st[1].nfr == 2);
        ll_player_start(&q, rm, 500, 100, 1);
        CHECK(q.sti == 0);
        /* EVERY STEP HERE IS ONE ITERATION (dt == LL_ITER_MS): the
         * legs pin exact carry arithmetic, which is the whole point
         * of the row write (the engine's arm writes the row and
         * hands the SAME accumulator over - PR-172). */
        ll_player_step(&q, &rp, &rk, 0, LL_ITER_MS, LL_ITER_MS);
        /* the opening iteration: row 0 (gate 70) closes, fi 1,
         * remainder 30 */
        CHECK(q.fi == 1 && q.row_t == 30u);
        /* LEG A: the ABSENT/NON-NUMERIC token is NO WRITE at all. The
         * engine stores -1 for it (0x51e3e) and its row-set method
         * REFUSES a negative value (`test edx,edx` + `jl` at 0x52381,
         * PR-107). The data settles which reading is the engine's: of
         * the 217 authored self-target lines, 22 carry a HELD
         * DIRECTION condition (K_FWD/K_BKWD) and 19 of those 22 have
         * no numeric token (census over HD ANIMACJE, this PR) - "write
         * the top" would restart the walk list on every iteration of a
         * held arrow, and the walk gait PR-79/PR-134 measured on
         * video (9-12 poses per cycle at ~120 ms per pose) does not
         * do that. So no numeric token, no seek; the row clock runs
         * on: fi 1, t 130. */
        q.sig = 1ull << LL_SIG_K_GEN4;
        ll_player_step(&q, &rp, &rk, 0, LL_ITER_MS, LL_ITER_MS);
        CHECK(q.sti == 0 && q.fi == 1 && q.row_t == 130u);
        /* LEG B: the seek to (3) lands on row 2 and KEEPS the state.
         * The row write is the arm's FIRST act (0x5360f-0x53616),
         * with no ENTER and no state-change bookkeeping: no event, no
         * impulse, no feed reset. The row clock runs on from there:
         * the carried 130 plus this iteration's 100 = 230 admits and
         * closes the SEEKED row (gate 70), so fi lands 3 with the
         * carry 160. WITHOUT the write the state still sits on row 1,
         * the 240-unit row: 230 < 240, and the step lands fi 1/t 230
         * (measured, this PR's mutation run). */
        q.sig = 1ull << LL_SIG_K_GEN2;
        ll_player_step(&q, &rp, &rk, 0, LL_ITER_MS, LL_ITER_MS);
        CHECK(q.sti == 0 && q.fi == 3 && q.row_t == 160u);
        /* LEG C: the seek is LIVE afterwards too - from row 3 (gate
         * 70) the carried 160 + 100 closes again, fi 4, t 190. A
         * state-change reset of the carry would land t 30; the
         * no-write port stood on row 1 and lands fi 2/t 90. */
        ll_player_step(&q, &rp, &rk, 0, LL_ITER_MS, LL_ITER_MS);
        CHECK(q.sti == 0 && q.fi == 4 && q.row_t == 190u);
        /* LEG D: a (2) re-seeks to row 1, the 240-unit row, from row
         * 4, and that row is LIVE at once: the carried 190 + 100 =
         * 290 closes it, so fi lands 2 with the carry 50. The
         * no-write port was on row 2 at t 90 and lands fi 3/t 120. */
        q.sig = 1ull << LL_SIG_K_GEN3;
        ll_player_step(&q, &rp, &rk, 0, LL_ITER_MS, LL_ITER_MS);
        CHECK(q.sti == 0 && q.fi == 2 && q.row_t == 50u);
        /* LEG E: an OVER-RANGE seek (9 with 6 rows) keeps the port's
         * recorded PR-107 clamp - the top. It is a real write here
         * too: from row 2 at t 50 it re-seeks to row 0, whose 70
         * this iteration closes (150-70 = 80), landing fi 1/t 80.
         * The no-write port was on row 3 and lands fi 4/t 150. */
        q.sig = 1ull << LL_SIG_K_GEN5;
        ll_player_step(&q, &rp, &rk, 0, LL_ITER_MS, LL_ITER_MS);
        CHECK(q.sti == 0 && q.fi == 1 && q.row_t == 80u);
        /* LEG F: the clamped row stays LIVE - held at fi 1, the
         * 240-unit row, because 80+100 = 180 does not reach it, so
         * the carry only accumulates to 180. The no-write port
         * carries THE SAME 180 here but on row 5: the row index is
         * the whole content of the difference. */
        ll_player_step(&q, &rp, &rk, 0, LL_ITER_MS, LL_ITER_MS);
        CHECK(q.sti == 0 && q.fi == 1 && q.row_t == 180u);
        /* LEG G: a CROSS-state seek still takes the TARGET's row
         * (the PR-107 rule: A_TWO(2) -> index 1, not its top) and
         * the cross-state arm's reset of the feed carry (PR-172). */
        q.sig = 1ull << LL_SIG_K_GEN1;
        ll_player_step(&q, &rp, &rk, 0, LL_ITER_MS, LL_ITER_MS);
        CHECK(q.sti == 1 && q.fi == 1 && q.row_t == 100u);
        ll_plt_free(&rp);
        free(rm);
    }
    {   /* PR-87: the ',' AND really ANDs. A_FIGHT(1)=K_FIRE,K_F1 is
         * ZARIA's exact attack-entry line: FIRE alone never matches;
         * plus F1 it does. (The same AND-gate on REAL ZARIA.01 data
         * - FIRE alone / F1 alone stay idle, FIRE,F1 fights - is
         * pinned by the verify-play zfight leg.) K_Fn and K_ALT are now
         * simulated keys, so the old "whole comma line is one OTHER,
         * never true" is gone for these tokens. */
        static const char FK[] =
            "A_STAY\nA_FIGHT\n\n[A_STAY]\ncontrol=BY_TIME|CHECK_LN\n"
            "F=600,0\nT=90,90\nG=0,0\nA_FIGHT(1)=K_FIRE,K_F1\nseq=a\n"
            "1,100\n\n[A_FIGHT]\ncontrol=BY_TIME\nF=0,0\nT=0,0\nG=0,0\n"
            "seq=a\n1,300\n2,300\n";
        ll_mov_t *fk = calloc(1, sizeof *fk);
        ll_player q;
        ll_play_keys kk = { 0 };
        CHECK(fk);
        ll_mov_parse(FK, fk);
        CHECK(fk->st[0].tr[0].nc == 2 &&
              fk->st[0].tr[0].c[1].cond == LL_COND_K_F1);
        ll_player_start(&q, fk, 500, 100, 1);
        kk.k_fire = 1;
        ll_player_step(&q, NULL, &kk, 0, 2000, LL_ITER_MS);
        CHECK(q.sti == 0);                  /* FIRE alone: no   */
        kk.k_f1 = 1;
        ll_player_step(&q, NULL, &kk, 0, 2000, LL_ITER_MS);
        CHECK(q.sti == 1);                  /* AND holds: fight */
        free(fk);
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
        ll_play_keys gk = { 0 };

        CHECK(gm && ll_plt_parse(GP, &gp) == 0);
        CHECK(ll_mov_parse(GL, gm) == 3);
        CHECK(!(gm->st[2].ctrl & LL_CTRL_CH_DIR) &&
              (gm->st[1].ctrl & LL_CTRL_CH_DIR));
        ll_player_start(&q, gm, 500, 100, 1);
        gk.k_bkwd = 1;
        ll_player_step(&q, &gp, &gk, 0, 2000, LL_ITER_MS);   /* -> walk left     */
        CHECK(q.sti == 1 && q.face == -1);
        gk.k_bkwd = 0;
        q.sig |= 1ull << LL_SIG_K_GEN1;                /* K_GEN1 trigger   */
        ll_player_step(&q, &gp, &gk, 0, 2000, LL_ITER_MS);   /* -> the baked state */
        CHECK(q.sti == 2);
        CHECK(q.walkdir == -1);                      /* the transition set it */
        CHECK(q.face == -1);                         /* but facing is untouched */
        {   /* and what gets drawn: authored mirror (1) XOR face (1) = 0
             * under the chosen always-XOR model. PR-135 re-derived the
             * frame index: this state's rows are `-1,1 / -2,1 / -3,1`,
             * so the entry iteration's own feed of 100 (a BY_X state
             * with F=0 is not a walking one, and its rows run on the
             * time feed - PR-114) closes row 0 right there and row 1's
             * authored -2 frame is what is current. */
            CHECK(ll_player_frame(&q) == -2);        /* the row-1 frame */
            ll_frame_ref fr = ll_mov_frame_ref(ll_player_frame(&q));
            CHECK(fr.idx == 1 && fr.mirror == 1 && !fr.blank);
            CHECK(ll_player_mirror_of(&q, fr.mirror) == 0);
        }
        ll_plt_free(&gp);
        free(gm);
    }
    {   /* PR-83 (the two user reports, data-free regression on the shapes
         * measured from ZARIA LEW.ZAR): the declared list - not block
         * order - names the spawn state; A_WALK is the combined
         * idle+walk (NO !K_FWD exit anywhere: stopping comes from the
         * key-gated target velocity + the authored F brake); BKWD plays
         * A_TURN once and it STAYS while the key is held; and the frame
         * `I 100,0` horizontal impulse (A_TURN frame 0) never lifts the
         * feet. Also guards: `time=` is parsed but NOT acted on, and the
         * frame-list wrap does not arm AC_TIME.
         * PR-110 CHANGED THE ROW TIMING HERE, and the row values used
         * are now ZARIA's real ones (A_WALK: 12 rows of 6, `GEN
         * AC_TIME` on list indices 0,1,5,6,7; A_TURN: 5 rows of 200 =
         * 1000 units, the `I 100,0` on the first, F=0,0). A_WALK is
         * BY_X, so its rows are gated by DISTANCE (0x543a0 mode 0,
         * PR-98/PR-100 - the feed ll_player_step section 4 now has):
         * no distance, no row. That replaces the old ms-clock claim
         * "standing in A_WALK cycles the 100 ms frame list and the
         * GEN AC_TIME tail drops to the idle hub", which was a
         * property of the ms clock, not of the data: with the key held
         * the state's own `A_WALK(x)=K_FWD` line comes first in the
         * list and wins (first match wins), and with the key released
         * the rows do not advance at all. Whether AC_TIME has another
         * source in the engine, or those five ZARIA rows are
         * vestigial, is a new open item (NEXT_STEPS PR-110); what is
         * NOT open is the feed. */
        static const char ZM[] =
            "A_WALK\nA_DRAP\nA_TURN\nA_JUMP_DOWN\n\n"
            "[A_JUMP_DOWN]\ncontrol=BY_TIME\nF=0,0\nG=0,0\nseq=a\n1,100\n\n"
            "[A_WALK]\ncontrol=BY_X|CH_DIR\nF=100,0\nT=20,5\nG=0,0\n"
            "A_TURN(1)=K_BKWD\nA_WALK(x)=K_FWD\nA_DRAP(1)=AC_TIME\n"
            "seq=a\n10,6,GEN AC_TIME\n11,6,GEN AC_TIME\n12,6\n1,6\n2,6\n"
            "3,6,GEN AC_TIME\n4,6,GEN AC_TIME\n5,6,GEN AC_TIME\n6,6\n"
            "7,6\n8,6\n9,6\n\n"
            "[A_DRAP]\ncontrol=BY_TIME\nF=100,0\nG=0,0\n"
            "A_TURN(1)=K_BKWD\nA_WALK(1)=K_FWD\nseq=a\n1,100\n\n"
            "[A_TURN]\ncontrol=BY_TIME|CH_DIR\nF=0,0\nG=0,0\n"
            "A_DRAP(1)=AC_BREAK_NEXT\nA_DRAP(1)=AC_BREAK_PREV\n"
            "seq=a\n-5,200,I 100,0\n-4,200\n3,200\n4,200\n5,200\n";
        static const char ZP[] = "1\n1 0,100;2000,100;\n";
        ll_mov_t *zm = calloc(1, sizeof *zm);
        ll_plt zp;
        ll_player s;
        ll_play_keys sk = { 0 };

        CHECK(zm && ll_plt_parse(ZP, &zp) == 0);
        CHECK(ll_mov_parse(ZM, zm) == 4);
        /* the header names A_WALK first but its .MOV block is SECOND here
         * (A_JUMP_DOWN block leads, exactly like ZARIA's file): start_st
         * follows the declaration, and there is no A_STAY to prefer. */
        CHECK(zm->start_st == 1 && ll_mov_find(zm, "A_JUMP_DOWN") == 0);
        ll_player_start(&s, zm, 500, 100, 1);
        CHECK(s.sti == 1 && strcmp(zm->st[1].name, "A_WALK") == 0);
        /* zero input: A_WALK is BY_X, so NOTHING feeds its row
         * accumulator (0x543a0 mode 0 adds the distance travelled, and
         * the lion is not travelling) - the 12-row list does not move,
         * no row completes, no `GEN AC_TIME` is raised, no idle hub is
         * reached, and he never drifts. PR-110's non-twin test for the
         * player-side mode feed: MEASURED against HEAD f86280c with the
         * scratch probe /tmp/opencode/zm_probe.c, that same fixture on
         * the ms clock ran 3 rows per 20 ms step (6 "ms" each), raised
         * AC_TIME inside tick 1 and entered A_DRAP at tick 2. Under the
         * mode feed the leg's 12 steps of 20 ms (240 ms) advance no row
         * at all AND accrue no wall time: the mode-0 branch of the
         * dispatcher replaces the 100-per-iteration feed, it does not
         * add to it, so the engine's one accumulator stays exactly 0 -
         * row_t and row_rem say so. A port that fed both clocks would
         * read 1200 units and a wrapped list here. */
        double zx = s.x;
        int idle_at = 0;
        for (int t = 1; t <= 12; t++) {
            ll_player_step(&s, &zp, &sk, 0, 2000, LL_ITER_MS);
            if (s.sti == 2)
                idle_at = t;
        }
        CHECK(idle_at == 0 && s.sti == 1 && s.fi == 0);
        CHECK(s.facc == 0.0 && s.row_t == 0u && s.row_rem == 0u);
        CHECK(s.x == zx);                              /* zero input never creeps */
        for (int t = 0; t < 5; t++)
            ll_player_step(&s, &zp, &sk, 0, 2000, LL_ITER_MS);
        CHECK(s.x == zx);
        CHECK(s.row_t == 0u && s.row_rem == 0u && s.facc == 0.0);
                                                       /* 17 steps = 340 ms
                                                        * in a mode-0 state:
                                                        * no wall feed, ever */
        /* PRESS FWD. The state's own A_WALK(x)=K_FWD line re-arms the
         * key-gated mode (a self-target, no reset). ZM's [A_WALK] is
         * the REAL ZARIA.01 [A_WALK]: F=100, T=20, twelve rows of 6
         * units. PR-135 makes this fixture the port's measured law leg,
         * because (F,T)=(100,20) is exactly the pair PR-134 measured on
         * video as the lion's ground run, [38,76] units/s.
         * THE RECURSION, from the constants in this fixture:
         *   v_n = v_(n-1)*(1 - T/100) + F*s/100 = 0.8 v_(n-1) + 10
         *   closed form v_n = 50*(1 - 0.8^n) -> 50 u/s = F*s/T, inside
         *   the measured bracket, and 2.5 units per iteration = the
         *   2.5 px/iteration PR-134's table gives this pair.
         *   distance after N iterations D(N) = sum 0.05*v_n
         *     = 2.5N - 10*(1 - 0.8^N)
         * THE GAIT, one row of 6 units admitted per iteration (the
         * one-admit rule of 0x521f0) and D growing under 2.5/iteration,
         * so at most one row can be due per iteration and the count is
         * exactly floor(D/6): rows close at N =
         *   6, 9, 11, 14, 16, 19, 21, 24, 26 ...
         * an alternating 3,2,3,2,... cadence, i.e. 2.44 iterations per
         * pose - the 2.4 iterations/pose PR-134's own arithmetic gives
         * for this pair (6 units at 2.5 units/iteration) and the
         * 72-110 video frames per pose it measured. That is the point
         * of this leg now: the authored row distance and the authored
         * F/T, through the engine's law, reproduce the measured gait. */
        sk.k_fwd = 1;
        ll_player_step(&s, &zp, &sk, 0, 2000, LL_ITER_MS);
        CHECK(s.sti == 1);
        zx = s.x;
        int badlaw = 0;
        static const int ZM_ROW_AT[] = { 6, 9, 11, 14, 16, 19, 21, 24, 26 };
        double p8 = 0.8;                     /* 0.8^n: iteration 1 was the
                                              * press call above, the loop
                                              * runs n = 2..26 */
        for (int t = 0; t < 25; t++) {
            int n = t + 2;                  /* iterations since the key went down */
            ll_player_step(&s, &zp, &sk, 0, 2000, LL_ITER_MS);
            p8 *= 0.8;
            double want_v = 50.0 * (1.0 - p8);
            if (s.vx > want_v + 1e-9 || s.vx < want_v - 1e-9)
                badlaw = 1;                 /* the closed form above, not a
                                             * copy of the model's recursion */
            CHECK(s.vx <= 50.0 + 1e-9);     /* F*s/T, the steady state */
            if (s.row_t != 0u || s.row_rem != 0u)
                badlaw = 1;             /* section 7 must NOT feed a walking
                                         * BY_X state: 0x543a2's mode-0
                                         * branch replaces the 100-per-
                                         * iteration feed */
            int nrow = 0;
            for (unsigned q = 0; q < sizeof ZM_ROW_AT / sizeof ZM_ROW_AT[0]; q++)
                if (ZM_ROW_AT[q] <= n)
                    nrow++;
            if (s.fi != nrow)
                badlaw = 1;                 /* the gait cadence, row by row */
        }
        CHECK(!badlaw);
        CHECK(s.sti == 1);                   /* the rows raised AC_TIME, but
                                              * this state's own A_WALK(x)
                                              * line is earlier in the list
                                              * and first match wins */
        CHECK(s.fi == 9);                    /* nine of the twelve rows: D(26)
                                              * = 55.03 units at 6 per row */
        CHECK(s.x > zx + 5);
        /* RELEASE (this is where the SDL KEYUP fix enters the picture -
         * a stuck key would keep walking forever). NO !K_FWD exit
         * exists, so the walk state stays and the ONLY thing that stops
         * him is the authored drag with s = 0: v_t = v_26 * 0.8^t,
         * where v_26 = 50*(1-0.8^26) = 49.764 u/s is the closed form
         * the hold loop above just pinned. The engine's integer
         * velocity register runs out at t=25: v_24 = 0.235 is above
         * one unit of it and survives, v_25 = 0.188 is below and is
         * zeroed (LL_PLAY_V_EPS - the port's model of that, counted
         * with the open item). The distance of that slide is the same
         * closed form times dt:
         *   0.05 * v_26 * (0.8 + 0.8^2 + ... + 0.8^24)
         *   = 0.05 * 49.849 * 4 * (1 - 0.8^24) = 9.9227 units,
         * which closes the TENTH row (1.03 units of debt + 9.923 at 6
         * per row) and no eleventh - and the tenth row's frame is
         * `7,6`, which raises no AC_TIME, so no idle hub is reached.
         * Under the old model this leg read differently in kind, not in
         * decimals: a target velocity of T=20 braked by F=100 to a stop
         * in 10 steps and 4.4 px, which stopped the list mid-cycle.
         * What the data says instead is that a released key leaves a
         * 25-iteration slide of 9.9 px - and ZARIA's own A_WALK has no
         * authored brake at all, exactly as PR-83 reported. */
        double xr = s.x;
        double vbrk = s.vx;                   /* = 50*(1-0.8^26), from above */
        double travel = 0.0;
        int rel_idle = 0;
        sk.k_fwd = 0;
        for (int t = 1; t <= 30; t++) {
            vbrk *= 0.8;                      /* the law with s = 0 */
            if (vbrk > -LL_PLAY_V_EPS && vbrk < LL_PLAY_V_EPS)
                vbrk = 0.0;
            travel += 0.05 * (vbrk < 0.0 ? -vbrk : vbrk);
            ll_player_step(&s, &zp, &sk, 0, 2000, LL_ITER_MS);
            if (s.sti == 2)
                rel_idle = t;
            if (s.vx > vbrk + 1e-9 || s.vx < vbrk - 1e-9)
                badlaw = 1;                   /* a non-gated BY_X state
                                               * would sit at the F*s/T
                                               * target through all of this */
        }
        CHECK(rel_idle == 0 && s.sti == 1);
        CHECK(!badlaw && s.vx == 0.0);
        CHECK(s.fi == 10);                     /* ten of twelve: the 1.03
                                                * units of debt plus the
                                                * 9.923-unit slide */
        CHECK(s.x > xr + 9.92 && s.x < xr + 9.93);
        CHECK(s.x - xr > travel - 1e-9 && s.x - xr < travel + 1e-9);
                                               /* the same distance as the
                                                * closed form above, summed
                                                * independently here */
        double xs = s.x;
        for (int t = 0; t < 15; t++)
            ll_player_step(&s, &zp, &sk, 0, 2000, LL_ITER_MS);
        CHECK(s.sti == 1 && s.vx == 0);
        CHECK(s.x == xs);                      /* the slide is over: a held
                                                * nothing means no motion */
        /* BKWD (the left key, the user's "turns but never walks left"):
         * A_TURN plays, and the REAL exit is AC_BREAK_NEXT - armed by
         * the frame-list WRAP. The real A_TURN list is 5 rows of 200 =
         * 1000 units = 10 iteration feeds, and on the iteration clock,
         * one row admitted per feed, that is exactly 10 iterations of
         * this state: the entry iteration's own feed is feed 1 (the
         * state change handed row_t = 0 over), the 200-unit rows close
         * on feeds 2, 4, 6, 8 and 10, the 10th wraps the list and arms
         * AC_BREAK_NEXT, and the transition matches at the top of feed
         * 11 - back to the A_DRAP hub, whose own single 100-unit row
         * closes in its entry iteration; with the key STILL held its
         * A_WALK(1)=K_FWD matches (facing-relative: LEFT is K_FWD once
         * face=-1) and the lion WALKS LEFT. Under the old literal-key
         * model this was a dead end: A_TURN's F=0 froze the turn slide
         * forever - exactly the reported "slides more than he walks,
         * and slow". The frame-0 impulse I 100,0 keeps the FEET DOWN
         * (iy==0 lifts nothing - a forced airborne state made
         * A_TURN/A_DRAP flicker while BKWD was held, the earlier
         * reported loop); wrap still arms AC_BREAK_NEXT but NOT
         * AC_TIME (the removed model choice; the time= guard is the PK
         * fixture below). */
        sk.k_bkwd = 1;
        ll_player_step(&s, &zp, &sk, 0, 2000, LL_ITER_MS);
        CHECK(s.sti == 3);                             /* A_TURN            */
        CHECK(s.walkdir == -1 && s.face == -1);        /* the LEFT key      */
        CHECK(s.grounded && !s.jumping && s.y == 100); /* horizontal impulse */
        /* the impulse IS applied (I/4 = 25 u/s), mirrored by walkdir as
         * everywhere in this model: A_TURN slides LEFT with it, and the
         * real A_TURN's F=0,0 with no T= authored takes NOTHING away -
         * no force term and no drag term (ZARIA's own turn slide runs
         * until its row list ends it; the behaviour PR-83 recorded as
         * "froze the turn slide forever"). A_DRAP's block is the same:
         * BY_TIME, so s = 0, and with no T= the slide holds through
         * it too. */
        CHECK(s.vx == -25.0);
        for (int t = 1; t <= 9; t++)                   /* A_TURN feeds 2..10:
                                                        * the rows close on
                                                        * the even feeds and
                                                        * the 10th ends the
                                                        * list (its wrap arms
                                                        * AC_BREAK_NEXT) */
            ll_player_step(&s, &zp, &sk, 0, 2000, LL_ITER_MS);
        CHECK(s.sti == 3);                             /* no early exit:
                                                        * wrap != AC_TIME  */
        ll_player_step(&s, &zp, &sk, 0, 2000, LL_ITER_MS);
        CHECK(s.sti == 2);                             /* feed 11 matches the
                                                        * armed BREAK_NEXT */
        ll_player_step(&s, &zp, &sk, 0, 2000, LL_ITER_MS);
        CHECK(s.sti == 1);                             /* A_WALK, walking  */
        CHECK(s.walkdir == -1 && s.face == -1);        /* LEFT, drawn LEFT */
        /* The walk LEFT from the slide's -25 u/s is the same recursion
         * as before with the sign of s reversed (facing left):
         *   v_n = 0.8 v_(n-1) - 10, v_0 = -25
         *   v_n = -(50 - 25*0.8^n)  -> the same F*s/T = -50 u/s
         *   distance of iteration n = 0.05*|v_n|
         * The call that ENTERED A_WALK is iteration 1 (a transition
         * runs before the physics of the same iteration, and it already
         * puts v to -30), the 25 calls below are n = 2..26, and
         *   sum_{n=2..26} 0.05*(50 - 25*0.8^n)
         *     = 2.5*25 - 1.25*(0.64*(1-0.8^25)/0.2)
         *     = 62.5 - 3.985 = 58.515 units.
         * The row accumulator also saw iteration 1's 1.5, so 60.015 of
         * distance at 6 units per row closes TEN rows and leaves the
         * eleventh 0.015 short. */
        double xw = s.x;
        for (int t = 0; t < 25; t++)
            ll_player_step(&s, &zp, &sk, 0, 2000, LL_ITER_MS);
        CHECK(s.x > xw - 58.52 && s.x < xw - 58.51);  /* the closed form */
        CHECK(s.fi == 10);                             /* ten of the twelve
                                                        * rows closed */
        ll_plt_free(&zp);
        free(zm);
    }
    {   /* PR-135's second measured-law leg: the WORM pair. ZARIA.01's
         * ZWIERZ1 worm is (F,T) = (100,40) with a 9-row list of 4-unit
         * pitch, and PR-134's table gives that pair 1.25 px per
         * iteration at the iteration constant. PR-79 measured a 1.44 s
         * walk cycle for that creature = 28.8 iterations = 9 poses x
         * 3.2 iterations, and that 3.2 is what the law produces here.
         * Hand-derived, no game data:
         *   v_n = v_(n-1)*(1 - 40/100) + F*s/100 = 0.6 v_(n-1) + 10
         *       -> F*s/T = 1000/40 = 25 u/s = 1.25 units per iteration
         *   D(N) = sum 0.05*v_n = 1.25N - 1.875*(1 - 0.6^N)
         * so the steady-state cadence is 4 units / 1.25 units per
         * iteration = 3.2 iterations per pose, and the nine rows of
         * the 36-unit cycle close at iterations
         *   5, 8, 12, 15, 18, 21, 24, 28, 31
         * The 3,3,4,3,... alternation is 3.2 expressed in whole
         * iterations, and the first cycle additionally pays the
         * 1.875-unit debt of the speed ramp (1.5 iterations), which is
         * why its ninth row lands at 31 rather than 29: 1.55 s against
         * the 1.44 s of a full-speed cycle, and from the ramp on the
         * port's closes are exactly 3.2 iterations apart. The ZM leg
         * above pins the lion's 2.4-iteration pose the same way;
         * together those are the two gaits PR-134 measured, derived
         * from the authored F/T and row pitch through the engine's own
         * law - which is the point of this PR.
         * MUTATION: any change to the per-iteration drag divisor or to
         * the F*s/100 force term breaks the closed form checked inside
         * the loop, and a row clock fed in ms instead of per iteration
         * breaks the close table. */
        static const char WM[] =
            "A_WORM\n\n[A_WORM]\ncontrol=BY_X|CH_DIR\nF=100,0\nG=0,0\n"
            "T=40,0\nseq=w\n1,4\n2,4\n3,4\n4,4\n5,4\n6,4\n7,4\n8,4\n9,4\n";
        static const int WM_CLOSE[] = { 5, 8, 12, 15, 18, 21, 24, 28, 31 };
        ll_mov_t *wnm = calloc(1, sizeof *wnm);
        ll_player wp;
        ll_play_keys wk = { 0 };
        int badw = 0;

        CHECK(wnm && ll_mov_parse(WM, wnm) == 1);
        CHECK(wnm->st[0].fx == 100 && wnm->st[0].tx == 40 &&
              wnm->st[0].nfr == 9 && wnm->st[0].fr[0].thresh == 4u);
        ll_player_start(&wp, wnm, 500, 100, 1);
        CHECK(wp.sti == 0 && wp.fi == 0);
        wk.k_fwd = 1;
        double p6 = 1.0;                          /* 0.6^n, independent of
                                                   * the port's own sum */
        for (int t = 1; t <= 31; t++) {
            p6 *= 0.6;
            ll_player_step(&wp, NULL, &wk, 0, 200000, LL_ITER_MS);
            double want_v = 25.0 * (1.0 - p6);
            if (wp.vx > want_v + 1e-9 || wp.vx < want_v - 1e-9)
                badw = 1;
            if (wp.sti != 0 || wp.grounded != 1 || wp.y != 100.0)
                badw = 1;                         /* G=0,0: the walk stays on
                                                   * its line */
            int nrow = 0;               /* rows due by iteration t, from
                                       * the table, not from the model */
            for (unsigned q = 0; q < sizeof WM_CLOSE / sizeof WM_CLOSE[0]; q++)
                if (WM_CLOSE[q] <= t)
                    nrow++;
            if (wp.fi != nrow % 9)
                badw = 1;                         /* the gait table, row by
                                                   * row; the 9th wraps */
        }
        CHECK(!badw);
        CHECK(wp.fi == 0 && wp.broke == 1);       /* the cycle closed on
                                                   * iteration 31 */
        CHECK(wp.x > 500 + 36.87 && wp.x < 500 + 36.88); /* D(31) = 36.875;
                                                         * 36 of it spent on
                                                         * nine rows and
                                                         * 0.875 still owed */
        free(wnm);
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
        ll_play_keys qk = { 0 };

        CHECK(pm && ll_plt_parse(PP, &pp) == 0);
        CHECK(ll_mov_parse(PK, pm) == 2);
        CHECK(pm->st[1].time_lo == 50 && pm->st[1].time_hi == 50);
        /* PR-163: the single-value form pairs to (50,50) exactly as the
         * engine's scanf pair leaves max=min (the `time=350` shape) */
        ll_player_start(&q, pm, 500, 100, 1);
        /* PR-135 RE-DERIVED THE TIMING AND THE RATE: one call is one
         * iteration, every feed is 100 units, and [A_RUN]'s first row
         * is 50 units with the `GEN K_GEN1` tail. The entry iteration's
         * own feed closes that row and raises the pulse, the match
         * comes at the top of the next iteration - pull_at = 2, where
         * the 28 ms grid needed 3 feeds of 28 to reach 50 and the pin
         * was a window.
         * A_PULL is BY_X F=100, T=30 and AUTO (entered with no
         * direction condition, so the walker's intent is held), which
         * under the engine law is v_n = 0.7 v_(n-1) + 10 ->
         * F*s/T = 1000/30 = 33.33 u/s, not the 30 of the old
         * "T = target velocity" reading. Its own row runs on distance,
         * 33.33 u/s * 0.05 = 1.67 units per iteration, so the 100-unit
         * row needs 60 iterations and stays open through the 40 of
         * this leg - no AC_TIME anywhere (and this is a BY_X state,
         * whose mode never runs the time feed at all). */
        int pull_at = 0;
        for (int t = 1; t <= 8 && !pull_at; t++) {
            ll_player_step(&q, &pp, &qk, 0, 2000, LL_ITER_MS);
            if (q.sti == 1)
                pull_at = t;
        }
        CHECK(pull_at == 2);               /* the GEN row closes on the
                                            * entry feed, exit next */
        CHECK(q.auto_walk && !((q.sig >> LL_SIG_K_GEN1) & 1)); /* pulse consumed */
        double qx = q.x;
        for (int t = 0; t < 40; t++) {
            ll_player_step(&q, &pp, &qk, 0, 2000, LL_ITER_MS);
            CHECK(q.vx <= 100.0 * LL_PLAY_INTENT_FULL / 30.0 + 1e-9);
        }
        CHECK(q.x > qx + 15 && q.sti == 1);           /* walks keylessly,
                                                        * AC_TIME exit stayed dark */
        ll_plt_free(&pp);
        free(pm);
    }
    {   /* PR-91 (T3 stage 2): an enemy attack raises AC_ENEMY_ATC on
         * the LION's own set; everything after that is authored data.
         * The reaction state and its exit line are ZARIA LEW.ZAR's own
         * (A_WALK's `A_DOSTAL(1)=AC_ENEMY_ATC`, A_DOSTAL's 9 rows of
         * 200 with the `I -320,-250` pullback on the first and
         * `A_DRAP(1)=AC_BREAK_NEXT`), the CURRENT state's line drives
         * the reaction, and the reaction ends at its OWN wrap - no new
         * mechanism anywhere (the raise itself is play_collide's,
         * covered by the Makefile play legs).
         * PR-135 RE-DERIVED EVERY NUMBER here: the fixture's row
         * values are unchanged (the real 200-unit hit rows), the clock
         * is now one call = one iteration of 50 ms with a feed of 100
         * units and ONE row admit, so a 200-unit row is exactly TWO
         * iterations and the whole 9-row list is 18 of them. The state
         * headers stay the fixture's own: the T-as-cap and G= readings
         * are PR-98/PR-99's open physics questions and this leg is not
         * the place to pin them.
         * - idle 4 iterations: A_WALK is BY_X with no key held, so
         *   NOTHING feeds its rows (the mode-0 branch feeds the
         *   distance travelled and there is none); its 3 rows of 6 need
         *   6 units of travel, row_t and row_rem stay 0, no row closes.
         * - leg step 5: the raise makes the transition match at the top
         *   of the iteration; the entry row (index 0) STAYS current and
         *   its `I -320,-250` fires: vx = -320/4 = -80, vy = -250/4 =
         *   -62.5. The feed of 100 that same iteration leaves
         *   row_rem = 100 against the 200-unit row, so nothing closes.
         *   -80 u/s then runs UNCHANGED for the rest of the leg: this
         *   state's header has F=0,0 and no T= at all, so the law has
         *   neither a force nor a drag term (PR-83's reading, kept).
         *   The lift is gone by the end of that same iteration: the
         *   shipped G constant adds 82.5 u/s before integration, which
         *   overshoots -62.5, and the landing search re-seats him
         *   (vy >= 0, the surface inside the 16 px step tolerance). At
         *   the 20 ms grid he was visibly airborne for one step
         *   (vy = -29.5, y = 99.41) - the difference is the coarser
         *   clock, not a change to the vertical fit, which stays
         *   exactly as shipped and stays an open item.
         * - rows close on the EVEN steps of the state: 6, 8, 10, ...,
         *   22, the 9th close (step 22) wraps the list and arms
         *   AC_BREAK_NEXT, and the exit matches at the top of step 23 -
         *   18 iterations = 900 ms for the whole authored reaction.
         *   `A_DRAP(1)` = the hub's row 1 (index 0) and it stays: the
         *   state change zeroes row_t and that entry iteration's feed
         *   of 100 is below its 200.
         * - the pullback through step 31 is 26 more iterations of
         *   -80 u/s = -4 units each = 104 units. */
        static const char HR[] =
            "A_WALK\nA_DOSTAL\nA_DRAP\n\n"
            "[A_WALK]\ncontrol=BY_X|CH_DIR\nF=100,0\nT=20,5\nG=0,0\n"
            "A_DOSTAL(1)=AC_ENEMY_ATC\nseq=walk\n1,6\n2,6\n3,6\n\n"
            "[A_DOSTAL]\ncontrol=BY_TIME\nF=0,0\nG=0,0\n"
            "A_DRAP(1)=AC_BREAK_NEXT\nseq=hit\n"
            "1,200,I -320,-250\n2,200\n3,200\n4,200\n"
            "5,200,WAIT !AC_ENEMY_ATC\n6,200\n7,200\n8,200\n9,200\n\n"
            "[A_DRAP]\ncontrol=BY_TIME\nF=0,0\nG=0,0\nseq=idle\n"
            "1,200\n2,200\n";
        static const char HP[] = "1\n1 0,100;2000,100;\n";
        ll_mov_t *hm = calloc(1, sizeof *hm);
        ll_plt hp;
        ll_player u;
        ll_play_keys uk = { 0 };

        CHECK(hm && ll_plt_parse(HP, &hp) == 0);
        CHECK(ll_mov_parse(HR, hm) == 3);
        CHECK(hm->st[1].nfr == 9 && hm->st[1].fr[0].thresh == 200u &&
              hm->st[1].fr[0].has_i && hm->st[1].fr[0].ix == -320 &&
              hm->st[1].fr[0].iy == -250);
        CHECK(hm->st[1].tr[0].tidx == 2 && hm->st[1].tr[0].entry == 1);
        ll_player_start(&u, hm, 500, 100, 1);
        CHECK(u.sti == 0);
        for (int t = 1; t <= 4; t++)                  /* nobody attacked:  */
            ll_player_step(&u, &hp, &uk, 0, 2000, LL_ITER_MS); /* the line stays dark */
        CHECK(u.sti == 0 && u.fi == 0);
        CHECK(u.facc == 0.0 && u.row_t == 0u && u.row_rem == 0u);
        u.sig |= 1ull << LL_SIG_AC_ENEMY_ATC;         /* the hit raises it */
        ll_player_step(&u, &hp, &uk, 0, 2000, LL_ITER_MS);    /* leg step 5       */
        CHECK(u.sti == 1 && u.fi == 0);               /* A_DOSTAL, row 1   */
        CHECK(u.vx == -80.0);                         /* -320/4 pulls back, and
                                                       * nothing brakes it */
        CHECK(u.y == 100.0 && u.grounded);            /* the -250/4 lift is
                                                       * re-seated within the
                                                       * one iteration (the
                                                       * 82.5 u/s the shipped
                                                       * G adds first) */
        double ux0 = u.x;
        int rec_at = 0, badlaw = 0;
        static const int close_at[] = { 6, 8, 10, 12, 14, 16, 18, 20, 22 };
        for (int t = 6; t <= 31; t++) {               /* rows are two feeds  */
            ll_player_step(&u, &hp, &uk, 0, 2000, LL_ITER_MS); /* each, so every even step */
            if (t == 6 && (!u.grounded || u.y != 100.0))
                badlaw = 1;                           /* back on his feet   */
            if (u.sti == 1) {
                int nc = 0;
                for (int k = 0; k < 9; k++)
                    if (close_at[k] <= t)
                        nc++;
                if (u.fi != nc % 9)                   /* the 9th close wraps */
                    badlaw = 1;                       /* back to row 1    */
            }
            if (!rec_at && u.sti == 2) {
                rec_at = t;
                if (u.fi != 0)
                    badlaw = 1;                       /* A_DRAP(1): the entry
                                                       * row stays standing */
            }
        }
        CHECK(rec_at == 23 && !badlaw);          /* AC_BREAK_NEXT at wrap */
        CHECK(u.x < ux0 - 103.9 && u.x > ux0 - 104.1);  /* the pullback: 26
                                                         * iterations x -4 */
        ll_plt_free(&hp);
        free(hm);
    }
    {   /* PR-108's BY_TY climb leg, RE-PINNED by PR-148: the leg
         * now tests the engine's input/guard law, not the old
         * ungated time clock. A census of the shipped files (tools
         * note in NEXT_STEPS PR-108, script kept out of git at
         * /tmp/opencode/pr108_census.py) finds BY_TY in 12 state
         * blocks in 7 .MOV files and three state names only: A_WSP
         * (seq climb / climb_r), A_WSP_LIN (the rope variant of the
         * same block) and TUNGA2's A_DRAB - the DRABINKA
         * rung-ladder family (PR-109 corrected the earlier "vine or
         * rope": ENA2's rope state A_CLIMB is BY_Y mode 1, a
         * different mechanism).
         * They all read `F=0,100 T=160,50` (A_DRAB `T=1000,50`),
         * all `G=0,0`, all `control=BY_TY|CHECK_ALL|SEQ_REV_Y`
         * (KARA's adds ANTI_SOLID|FRAME_REV), they are entered by
         * AC_DRAB/AC_DRAB_CATCH/AC_ONAIR and left by
         * `!AC_DRAB`/`!AC_LINA`. PR-147 read the rest of the law at
         * byte level and PR-148 implements it: the mode-4 branch
         * 0x5441b feeds the row accumulator 100 ONLY while
         * [obj+0xbe], the player's vertical intent, is non-zero -
         * the frame loop hands the LION the keyboard block through
         * its input method every iteration (0x4aeca, PR-147) and
         * 0x549f0 stores its word1 (Up/Down, y+ down) into that
         * word. The port's p->vert_intent and section 7's feed
         * guard are exactly that fact; the kind-4 contact override
         * (0x54b16) is PR-149's route, and so is the word's use
         * as a v_y force (0x542cd) - see the open items in the
         * PR-148 record.
         * None of it is reachable from shipped data in the player
         * sim - no port code raises AC_DRAB/AC_DRAB_CATCH/AC_ONAIR/
         * AC_LINA/AC_EDGE (PR-50 showed those are engine-raised) -
         * so the signals are raised directly, the way PR-91's leg
         * does. THE ARITHMETIC, RE-DERIVED BY HAND FOR PR-148:
         * with the intent held every iteration the spans are
         * exactly the PR-108 ones (the guard passes, so the held
         * climb's clock is unchanged), and the NEW fact is the
         * no-intent freeze and the partial accumulator surviving
         * the gap. The authored frame list is 300,200x8,300 with
         * no time_scale line; one call = one iteration = one feed
         * of 100 units and one row admit; `A_WSP(3)=` enters at
         * list index 2 (entry-1 = row 3, PR-108).
         * LEG 1, no keys held: the guard fails every iteration, so
         * the feed is zero FOREVER and 25 iterations change
         * NOTHING beyond the entry itself: rows never advance, the
         * list never wraps, AC_BREAK_NEXT never arms, the
         * accumulator stays 0, and x, y, v_x, v_y never move (the
         * model has no vertical motion for this state and this PR
         * adds none: the word's use as a v_y FORCE at 0x542cd is
         * counted open in the record, and the earlier session's
         * 0x50252 "rise law" candidate was caught dead - a byte
         * census shows NO call site and no table entry points at
         * it anywhere in the image; what the frame loop's
         * [lion.vt+0x74] call at 0x4afea really runs is 0x50320).
         * Under the old ungated model this exact loop advanced and
         * wrapped at t=17; the mutations that delete or invert the
         * guard land here. The final step lets go of AC_DRAB:
         * `A_FALL_DOWN(1)=!AC_DRAB` wins from a state whose list
         * NEVER ticked - transitions are a separate pass (section
         * 1), exactly as the engine evaluates transitions and rows
         * separately.
         * LEG 2, a fresh climb on a fresh player, Up held from the
         * first step to t=15: every iteration qualifies, so the
         * row spans are the PR-108 ones -
         *   t=1  enter at index 2, rem 100 < 200, nothing closes
         *   t=2  rem 200  -> index 3      t=4 -> 4   t=6 -> 5
         *   t=8  -> 6                    t=10 -> 7  t=12 -> 8
         *   t=14 -> 9 (the last 200-row closes, the 300-row opens)
         *   t=15 rem 100 against the 300-row, still index 9
         * - and the held climb does NOT move x, y, v_x or v_y:
         * holding Up on a ladder only drives the row clock in this
         * model, the y-force path being the counted open item.
         * LEG 3, release mid-row: t=16,17 hold NO key. The row
         * clock freezes at index 9 - no feed - but the carried
         * accumulator (100 of the 300-row) survives untouched,
         * because the guard feeds nothing while the row compare
         * still runs, the engine's own order (feed 0x52317, THEN
         * compare 0x52320). No wrap at t=17 either: a mutation
         * that deletes the guard wrapped HERE (the PR-108 spans),
         * and one that zeroes the accumulator on a non-qualifying
         * iteration loses the carry and wraps at t=20, not t=19.
         *   t=18 Down held: the clock restarts ON THE CARRIED 100
         *   (the engine's [obj+0xa1] does the same): 100 + 100 =
         *   200 < 300, the row does NOT close.
         *   t=19 still Down: 200 + 100 = 300 -> the list WRAPS to
         *   index 0 and arms AC_BREAK_NEXT (readable only on the
         *   step that armed it, `broke` clears at iteration head).
         *   t=20 still Down: index 0's 300-row takes a fresh 100,
         *   no close.
         * The clock is sign-blind, Up and Down qualifying alike -
         * the guard tests `!= 0`, exactly like 0x5441b /
         * `cmp [obj+0xbe],0; je` - and only the unmodelled force
         * path would care which way. */
        static const char CE[] =
            "A_JUMP\nA_WSP\nA_FALL_DOWN\nA_PRE_JUMP\nA_TURN_BACK\n"
            "A_PODCIAG\n\n[A_JUMP]\ncontrol=BY_TIME|CHECK_ALL\nF=0,100\n"
            "T=160,50\nG=0,0\nA_WSP(3)=AC_DRAB_CATCH,AC_ONAIR\nseq=climb\n"
            "1,900\n2,900\n\n[A_WSP]\ncontrol=BY_TY|CHECK_ALL|SEQ_REV_Y\n"
            "F=0,100\nT=160,50\nG=0,0\nA_PODCIAG(1)=AC_EDGE,K_UP\n"
            "A_TURN_BACK(1)=AC_DUP,K_DOWN\nA_FALL_DOWN(1)=!AC_DRAB\n"
            "A_PRE_JUMP(1)=K_JUMP\nseq=climb\n1,300\n2,200\n3,200\n4,200\n"
            "5,200\n6,200\n7,200\n8,200\n9,200\n10,300\n\n[A_FALL_DOWN]\n"
            "control=BY_TIME|CHECK_ALL\nF=0,100\nT=160,50\nG=0,0\n"
            "seq=spad\n1,400\n2,400\n\n[A_PRE_JUMP]\ncontrol=BY_TIME\n"
            "F=0,100\nT=160,50\nG=0,0\nseq=uj30\n1,200\n\n[A_TURN_BACK]\n"
            "control=BY_TIME\nF=0,100\nT=160,50\nG=0,0\nseq=turn_bk\n"
            "1,300\n\n[A_PODCIAG]\ncontrol=BY_TIME\nF=0,100\nT=160,50\n"
            "G=0,0\nseq=podciag\n1,400\n";
        ll_mov_t *cm = calloc(1, sizeof *cm);
        ll_player r, u;
        ll_play_keys rk = { 0 };
        static const int at[] = { 1, 2, 4, 6, 8, 10, 12, 14 };
        static const int want[] = { 2, 3, 4, 5, 6, 7, 8, 9 };
        int nx = 0;
        int bad = 0;

        CHECK(cm && ll_mov_parse(CE, cm) == 6);
        CHECK(cm->st[1].nfr == 10 && cm->st[1].fr[2].thresh == 200 &&
              cm->st[1].fr[9].thresh == 300);
        /* PR-148: BY_TY has a model bit now (the 0x5441b feed
         * guard) and reaches ctrl through the parser; SEQ_REV_Y
         * alone is the name left counted in ctrl_other - the PR-
         * 108 pin of ctrl_other == 2 re-pins to 1 here, honestly,
         * and the one flag the model already knew (CHECK_ALL)
         * still reaches ctrl. */
        CHECK(cm->st[1].ctrl_other == 1 &&
              (cm->st[1].ctrl & LL_CTRL_CHECK_ALL) != 0 &&
              (cm->st[1].ctrl & LL_CTRL_BY_TY) != 0 &&
              (cm->st[1].ctrl & (LL_CTRL_BY_X | LL_CTRL_BY_TIME)) == 0);
        /* the authored exit list, in order, with the negated signal */
        CHECK(cm->st[1].tr[2].entry == 1 && cm->st[1].tr[2].nc == 1 &&
              cm->st[1].tr[2].c[0].cond == LL_COND_OTHER &&
              cm->st[1].tr[2].c[0].sig == LL_SIG_AC_DRAB &&
              cm->st[1].tr[2].c[0].neg == 1);
        CHECK(cm->st[0].tr[0].entry == 3 &&
              cm->st[0].tr[0].c[0].sig == LL_SIG_AC_DRAB_CATCH);
        /* LEG 1: the guard. No key ever held. */
        ll_player_start(&r, cm, 500, 500, 1);
        CHECK(r.sti == 0);                            /* the declared name  */
        CHECK(r.vert_intent == 0);
        for (int t = 1; t <= 25; t++) {
            r.sig |= 1ull << LL_SIG_AC_DRAB;   /* the ladder stays held   */
            if (t == 1)
                r.sig |= (1ull << LL_SIG_AC_DRAB_CATCH) |
                         (1ull << LL_SIG_AC_ONAIR);
            ll_player_step(&r, NULL, &rk, 0, 2000, LL_ITER_MS);
            if (r.sti != 1 || r.fi != 2)
                bad = 1;        /* entered, then frozen: no feed at all   */
            if (r.row_t != 0 || r.facc != 0.0)
                bad = 1;        /* zero feeds; mode 4 is not distance-fed */
        }
        CHECK(!bad);
        CHECK(r.broke == 0);                  /* never wrapped, never armed */
        CHECK(r.y == 500 && r.x == 500 && r.vx == 0 && r.vy == 0);
        r.sig &= ~(1ull << LL_SIG_AC_DRAB);   /* the ladder lets go     */
        ll_player_step(&r, NULL, &rk, 0, 2000, LL_ITER_MS);
        CHECK(r.sti == 2 && r.fi == 0);       /* the exit works from a     */
                                              /* state whose list NEVER    */
                                              /* ticked (separate passes)  */
        /* LEG 2/3: a fresh climb, Up held t=1..15 (PR-108's spans
         * while the guard passes), no key t=16..17 (frozen, carry
         * kept, NO wrap at 17), Down t=18..20 (restart on the
         * carry, wrap at 19). */
        ll_player_start(&u, cm, 500, 500, 1);
        rk.k_up = 1;
        for (int t = 1; t <= 15; t++) {
            u.sig |= 1ull << LL_SIG_AC_DRAB;
            if (t == 1)
                u.sig |= (1ull << LL_SIG_AC_DRAB_CATCH) |
                         (1ull << LL_SIG_AC_ONAIR);
            ll_player_step(&u, NULL, &rk, 0, 2000, LL_ITER_MS);
            if (nx < (int)(sizeof at / sizeof at[0]) && t == at[nx]) {
                if (u.sti != 1 || u.fi != want[nx] ||
                    ll_player_frame(&u) != want[nx] + 1)
                    bad = 1;
                nx++;
            }
        }
        CHECK(!bad && nx == (int)(sizeof at / sizeof at[0]));
        CHECK(u.sti == 1 && u.fi == 9 && u.row_t == 100);
        CHECK(u.y == 500);        /* no vertical MOTION is modelled for   */
        CHECK(u.x == 500 && u.vx == 0 && u.vy == 0);   /* this state yet */
        rk.k_up = 0;
        for (int t = 16; t <= 17; t++) {
            u.sig |= 1ull << LL_SIG_AC_DRAB;
            ll_player_step(&u, NULL, &rk, 0, 2000, LL_ITER_MS);
            CHECK(u.sti == 1 && u.fi == 9 && u.row_t == 100);
            CHECK(u.y == 500 && u.vy == 0);
            CHECK(u.broke == 0);        /* NO wrap at t=17: the guard     */
        }
        rk.k_down = 1;
        u.sig |= 1ull << LL_SIG_AC_DRAB;
        ll_player_step(&u, NULL, &rk, 0, 2000, LL_ITER_MS);       /* t=18 */
        CHECK(u.sti == 1 && u.fi == 9 && u.row_t == 200);
        u.sig |= 1ull << LL_SIG_AC_DRAB;
        ll_player_step(&u, NULL, &rk, 0, 2000, LL_ITER_MS);       /* t=19 */
        CHECK(u.sti == 1 && u.fi == 0 && u.broke == 1);
        CHECK(u.row_t == 0);            /* the wrap: index 0, BREAK_NEXT  */
        u.sig |= 1ull << LL_SIG_AC_DRAB;
        ll_player_step(&u, NULL, &rk, 0, 2000, LL_ITER_MS);       /* t=20 */
        CHECK(u.sti == 1 && u.fi == 0 && u.row_t == 100);
        CHECK(u.y == 500 && u.vy == 0); /* Down qualifies the clock like  */
                                        /* Up: the guard is sign-blind    */
        rk.k_down = 0;
        free(cm);
    }
    {   /* PR-110's own non-twin test: the row clock counts ITERATIONS,
         * not wall ms, and one authored list must keep the same
         * ITERATION spans on two different play grids. The fixture is
         * this test's own - deliberately, since what is under test is
         * the clock, not a shipped script.
         * Hand-derived, no game data. `A_ONE` is a BY_TIME list of
         * 300,200,200,200,300 = 1200 units = 12 iteration feeds
         * (a feed is 100 units, ll_play.h). Section 7 adds a feed,
         * then subtracts a row's threshold when the accumulator
         * reaches it, so a 300-unit row is current for 3 feeds and a
         * 200-unit row for 2: spans of 3,2,2,2,3 however the grid is
         * cut (MUT-A: hard-wiring "a row is 2 iterations" would break
         * the 300s).
         * Grid 1, dt = LL_ITER_MS: exactly one feed per step, so the
         * rows close at steps 3,5,7,9,12 and the 1200th unit WRAPS the
         * list in step 12 - 12 steps for the whole cycle, where the
         * wall-ms model took 1200 ms = 43 steps of 28 ms. Because 12 x
         * 100 is exactly the list, the accumulator and the ms carry are
         * both 0 after every step; that is the sharpest form of the
         * claim, and it fails at once on a mutated LL_ITER_MS, a
         * subtract-once rule, or a feed that does not carry.
         * Grid 2, dt = 20 ms (a sub-iteration grid, which is the
         * point of running it): the m-th feed lands at step
         * ceil(50m/20), i.e. 3,5,8,10,13,15,18,20,23,25,28,30 for
         * m = 1..12, so the SAME list closes its rows at steps
         * 8,13,18,23,30 - the same 12 feeds, 30 grid steps (600 ms,
         * this grid quantising the 600 ms the list is worth at 50 ms
         * a feed), and the row spans in feeds are still 3,2,2,2,3.
         * The arithmetic inside the loop is the MUT-C trip-wire:
         * after every step the accumulator must be exactly 100 x
         * floor(20t/50) minus the thresholds closed so far, and
         * row_rem exactly (20t) mod 50 - a feed that drifted would
         * show a residue, a feed in ms would show 20t. PR-135
         * re-derived this table for the new LL_ITER_MS; at the old
         * 28 ms iteration the closes were at steps 5,7,10,13,17.
          * Both loops also read `facc`, which section 4 only feeds in a
          * BY_X state with a non-zero state F= (0x543a2's mode-0 branch
          * REPLACES the 100-per-iteration feed, it does not add to it):
          * in this BY_TIME state it stays exactly 0 at every step.
         * time_scale= is the engine's own pre-scale of the authored
         * row values: `threshold = value * time_scale / 1000` inside
         * each row node's ctor (0x51679 `imul`, 0x5168d `idiv 0x3e8`),
         * so a block with time_scale=2000 runs its rows at DOUBLE the
         * iteration count - the same list is then 1100 units and 11
         * feeds, not 7. PR-104's ctor-order reading (only rows built
         * after the line see the scale) is asserted too; no shipped
         * .MOV has a row before its line (116 active lines in 38
         * files, none with a preceding row), and the block without the
         * line is the MUT-B control: 300,200,200 unchanged, the default
         * scale 1000. */
        static const char RC[] =
            "A_ONE\n\n[A_ONE]\ncontrol=BY_TIME\nF=0,0\nG=0,0\nseq=a\n"
            "1,300\n2,200\n3,200\n4,200\n5,300\n";
        static const unsigned RC_THR[] = { 300, 200, 200, 200, 300 };
        static const int RC_CLOSE50[] = { 3, 5, 7, 9, 12 };
        static const int RC_CLOSE20[] = { 8, 13, 18, 23, 30 };
        ll_mov_t *rcm = calloc(1, sizeof *rcm);
        ll_player v, w;
        ll_play_keys vk = { 0 };

        CHECK(rcm && ll_mov_parse(RC, rcm) == 1);
        CHECK(rcm->st[0].nfr == 5 && rcm->st[0].fr[0].thresh == 300u &&
              rcm->st[0].fr[1].thresh == 200u);
        CHECK(rcm->st[0].time_scale == 1000 && rcm->st[0].scale_at == 0);
        ll_player_start(&v, rcm, 500, 100, 1);
        for (int t = 1; t <= 12; t++) {         /* grid 1: one feed/step */
            unsigned closed = 0;
            int want = 0;
            for (int k = 0; k < 5; k++)
                if (RC_CLOSE50[k] <= t) {
                    closed += RC_THR[k];
                    want = k + 1;
                }
            if (want == 5)
                want = 0;                      /* the 12th feed wraps */
            ll_player_step(&v, NULL, &vk, 0, 2000, LL_ITER_MS);
            CHECK(v.fi == want &&
                  v.row_t == 100u * (unsigned)t - closed &&
                  v.row_rem == 0u && v.facc == 0.0);
            CHECK(v.x == 500 && v.y == 100);    /* rows with no M dx,dy */
        }
        CHECK(v.broke == 1);                    /* the wrap armed it */
        ll_player_start(&w, rcm, 500, 100, 1);
        for (int t = 1; t <= 30; t++) {         /* grid 2: 100 per 50 ms  */
            unsigned closed = 0;
            int want = 0;
            for (int k = 0; k < 5; k++)
                if (RC_CLOSE20[k] <= t) {
                    closed += RC_THR[k];
                    want = k + 1;
                }
            if (want == 5)
                want = 0;
            ll_player_step(&w, NULL, &vk, 0, 2000, 20);
            CHECK(w.fi == want &&
                  w.row_t == 100u * (unsigned)((20 * t) / 50) - closed &&
                  w.row_rem == (unsigned)((20 * t) % 50) &&
                  w.facc == 0.0);
        }
        static const char TS[] =
            "A_ONE\n\n[A_ONE]\ncontrol=BY_TIME\nF=0,0\nG=0,0\nseq=a\n"
            "1,300\ntime_scale=2000\n2,200\n3,200\n";
        static const char TS0[] =
            "A_ONE\n\n[A_ONE]\ncontrol=BY_TIME\nF=0,0\nG=0,0\nseq=a\n"
            "1,300\n2,200\n3,200\n";
        static const unsigned TS_THR[] = { 300, 400, 400 };
        static const int TS_CLOSE[] = { 3, 7, 11 };
        ll_mov_t *tsm = calloc(1, sizeof *tsm), *ts0 = calloc(1, sizeof *ts0);
        ll_player u;

        CHECK(tsm && ts0 && ll_mov_parse(TS, tsm) == 1 &&
              ll_mov_parse(TS0, ts0) == 1);
        CHECK(tsm->st[0].nfr == 3 && tsm->st[0].time_scale == 2000 &&
              tsm->st[0].scale_at == 1);
        CHECK(tsm->st[0].fr[0].thresh == 300u &&     /* before the line  */
              tsm->st[0].fr[1].thresh == 400u &&     /* after it: x2     */
              tsm->st[0].fr[2].thresh == 400u);
        CHECK(ts0->st[0].fr[1].thresh == 200u &&     /* MUT-B control    */
              ts0->st[0].time_scale == 1000 && ts0->st[0].scale_at == 0);
        ll_player_start(&u, tsm, 500, 100, 1);
        for (int t = 1; t <= 11; t++) {         /* 1100 units = 11 feeds */
            unsigned closed = 0;
            int want = 0;
            for (int k = 0; k < 3; k++)
                if (TS_CLOSE[k] <= t) {
                    closed += TS_THR[k];
                    want = k + 1;
                }
            if (want == 3)
                want = 0;
            ll_player_step(&u, NULL, &vk, 0, 2000, LL_ITER_MS);
            CHECK(u.fi == want &&
                  u.row_t == 100u * (unsigned)t - closed &&
                  u.row_rem == 0u);
        }
        free(rcm);
        free(tsm);
        free(ts0);
    }
    {   /* PR-114: the `fx != 0` half of the BY_X gate - the half no
         * leg was ever in. Section 4's feed and section 7's `walk`
         * flag both read `(st->ctrl & LL_CTRL_BY_X) && st->fx != 0`;
         * the second term is the port's own addition, since 0x543a0
         * keys mode 0 on the MODE alone (PR-100's owner reading). The
         * corpus count is now a tool output, not a claim:
         * tools/re/mov_byx_gate.py over the 777 .MOV measures 105
         * BY_X state blocks in 69 files, and 30 of them (290 rows,
         * 21 files) say F=0,0 - 29 of those 30 declare no motion
         * source of their own, so for them this is the whole law.
         * Three fixture states in this repo are BY_X with fx == 0
         * (here: GL's A_GO_TO_BALON_L; in ll_level.c: obj_motion's
         * `halt`, and a CHECK_LN ground probe with no F= line), but
         * none of them says anything about the row clock: A_GO_TO_
         * BALON_L's rows are 1 unit and its CHECKs read the drawn
         * frame; `halt`'s leg only checks that the object does not
         * move, and its comment marks the frame list "falls back to
         * time - estimate". So the choice was unpinned, not absent.
         * This leg pins it and changes nothing: what the engine does
         * with these 30 states is the open question, recorded as
         * NEXT_STEPS PR-114.
         * Hand-derived, no game data, every step at dt = LL_ITER_MS,
         * which is the grid where one step is exactly one feed.
         * A_PUSH is the control half: BY_X, F=1000,0, T=40,0. Under the
         * engine law a held direction key is the full intent s=10, so
         * each iteration drags T/100 = 40% off the speed and then adds
         * F*s/100 = +100 u/s:
         *   v1 = 100, v2 = 100*0.6 + 100 = 160, -> F*s/T = 10000/40
         *   = 250 u/s, i.e. v_n = 250*(1 - 0.6^n) exactly.
         * The distance those two iterations travel (100*0.05 + 160*0.05
         * = 13 units) feeds `facc` ONLY: one row of 1000 units stays
         * unclosed and row_t/row_rem stay exactly 0 - the mode-0 branch
         * replaces the wall clock.
         * The K_GEN1 pulse then moves him into A_COAST: BY_X with
         * F=0,0 and NO T= at all, the shape of 29 of the 30. Both
         * terms of the gate are now false, and two things follow:
         * - the velocity is FROZEN at the entry value, 160 u/s: the law
         *   has neither term to change it - no F= word means no force
         *   and no T= word means no drag (the same reading PR-83
         *   recorded for ZARIA's own A_TURN, and the reason the old
         *   "target velocity 0" vs "auto_walk" disagreement this leg
         *   kept on purpose no longer exists). It does not brake on
         *   release either, for the same reason.
         * - the rows run on the ITERATION clock instead: section 7
         *   feeds 100 per step, so six rows of 100 close in six steps,
         *   one per step, the sixth closing index 5 of 5 and wrapping
         *   (fi back to 0, `broke` armed by the wrap alone, which is
         *   why the loop below must end on that step). `facc` starts at
         *   0 with the new state and stays 0.0 the whole way.
         * MUT-1 (delete `st->fx != 0`, i.e. key the feed on the mode
         * as the dispatcher does): A_COAST becomes a walking state,
         * its wall clock never runs and 6 steps travel 48 against a
         * 100-unit row, so the leg fails at its first step - and no
         * other leg changes, which IS the coverage claim. MUT-2
         * (delete the MODE half instead): this leg stands, PR-110's
         * BY_TIME legs fail. The mode half was covered; the fx half
         * was not. */
        static const char BX[] =
            "A_PUSH\nA_COAST\n\n[A_PUSH]\ncontrol=BY_X|CH_DIR\n"
            "F=1000,0\nT=40,0\nG=0,0\nA_COAST(1)=K_GEN1\nseq=a\n"
            "1,1000\n\n[A_COAST]\ncontrol=BY_X\nF=0,0\nT=0,0\nG=0,0\n"
            "seq=a\n-1,100\n-2,100\n-3,100\n-4,100\n-5,100\n-6,100\n";
        ll_mov_t *bxm = calloc(1, sizeof *bxm);
        ll_player bx;
        ll_play_keys bk = { 0 };
        int bad = 0;

        CHECK(bxm && ll_mov_parse(BX, bxm) == 2);
        CHECK((bxm->st[1].ctrl & LL_CTRL_BY_X) && bxm->st[1].fx == 0 &&
              bxm->st[1].tx == 0 && bxm->st[1].nfr == 6 &&
              bxm->st[1].fr[0].thresh == 100u);
        ll_player_start(&bx, bxm, 500, 100, 1);
        CHECK(bx.sti == 0 && bx.walkdir == 1 && bx.vx == 0.0);
        bk.k_fwd = 1;
        for (int t = 1; t <= 2; t++) {          /* the distance-fed half */
            double want = t == 1 ? 100.0 : 160.0; /* v_n = 250*(1-0.6^n)  */
            ll_player_step(&bx, NULL, &bk, 0, 2000, LL_ITER_MS);
            if (bx.sti != 0 || bx.fi != 0 ||
                bx.vx < want - 1e-9 || bx.vx > want + 1e-9 ||
                bx.row_t != 0u || bx.row_rem != 0u)
                bad = 1;
        }
        CHECK(!bad);
        CHECK(bx.facc > 12.99 && bx.facc < 13.01);    /* fed, never enough  */
        CHECK(bx.x > 500 + 12.99 && bx.x < 500 + 13.01);  /* 100*.05 + 160*.05 */
        double bxc = bx.x;                           /* where the walk got  */
        bx.sig |= 1ull << LL_SIG_K_GEN1;         /* the shipped exit shape:
                                                  * a pulse, not a key */
        bad = 0;
        for (int s = 1; s <= 6; s++) {
            ll_player_step(&bx, NULL, &bk, 0, 2000, LL_ITER_MS);
            if (bx.sti != 1)
                bad = 1;                            /* the pulse fired once */
            if (bx.fi != (s == 6 ? 0 : s))
                bad = 1;                            /* one row per step, and
                                                     * the 6th wraps it */
            if (bx.row_t != 0u || bx.row_rem != 0u)
                bad = 1;                            /* 100 in, 100 spent */
            if (bx.facc != 0.0)
                bad = 1;                            /* section 4's feed does
                                                     * not run at all here */
            if (bx.vx != 160.0)
                bad = 1;                            /* frozen: no F= term, and
                                                     * no T= term to change it */
            if (bx.broke != (s == 6))
                bad = 1;                           /* the wrap armed it */
            double want_x = bxc + s * 8.0;         /* 160 u/s * 0.05 s  */
            if (bx.x < want_x - 1e-9 || bx.x > want_x + 1e-9)
                bad = 1;
        }
        CHECK(!bad);
        CHECK(bx.broke == 1);                      /* the wrap armed it */
        CHECK(bx.y == 100 && bx.vy == 0.0);        /* G=0,0: no fall of its
                                                    * own either */
        bad = 0;
        bk.k_fwd = 0;
        for (int s = 1; s <= 3; s++) {              /* and the same law is
                                                     * why it never brakes */
            ll_player_step(&bx, NULL, &bk, 0, 2000, LL_ITER_MS);
            if (bx.vx != 160.0 || bx.fi != s || bx.facc != 0.0)
                bad = 1;
        }
        CHECK(!bad);
        free(bxm);
    }
    {   /* PR-115: PR-110 open items 1 and 8 - how many ROWS ONE STEP may
         * close. The engine admits exactly one. 0x52320-0x5232b, the
         * exact bytes read again for this record: `8b 43 10` (eax = the
         * row's threshold, [node+0x10]), `8b 9a a1 00 00 00` (ebx = the
         * accumulator [obj+0xa1]), `39 d8` (cmp eax,ebx), `7d 27`
         * (jge 0x52354: threshold >= accumulator -> the epilogue, no row
         * this step) - and `29 c3` at 0x52334 subtracts in REGISTERS
         * only, handing `acc - thr` to the row handler at
         * [[rec+0x44]+0x18]. Nothing in that path writes [obj+0xa1],
         * and the only writes to it in the dispatcher are feeds:
         * `add [obj+0xa1], eax` at 0x543d1 (mode 0: the truncated
         * v_x/100 of the tick), 0x543e7 (the constant 100) and 0x54412
         * / 0x5443b (the gated modes 3/4). So the accumulator is
         * monotonic and a covered threshold is admitted once per step.
         * PR-135 REMOVED that loop, its `4 * nfr` cap and its resync: the
         * state step now admits AT MOST ONE row per iteration and hands the
         * remainder over, which is the shape those bytes describe. The two
         * fixtures still separate the shapes, on arithmetic re-derived by
         * hand for the new clock (one call = one iteration per
         * LL_ITER_MS, a feed of 100, one admit, remainder carried in ms).
         * MULTI's rows parse to 100,30,30 (the port's clamp of the authored
         * `1,0` stays, ll_play.c:481) and it starts at x=100, fi=0:
         * - step 1, dt = 116 ms = 2 x LL_ITER_MS + 16: TWO iterations, so
         *   exactly two closures - row 0 (`M 100,0`, x 200) and then row 1
         *   (`M 30,0`, x 230) - row_t = 100 + 100 - 100 - 30 = 70,
         *   row_rem = 16, and no wrap happened, so `broke` is 0. Under the
         *   old loop this same step closed SIX rows (twice round the list,
         *   x 420, row_t 80) and the cap never fired; the two shapes are
         *   furthest apart exactly here.
         * - step 2, dt = LL_ITER_MS: one iteration, row_rem stays 16
         *   (16 + 50 - 50), row_t becomes 70 + 100 = 170, which covers
         *   row 2 - the last of the list, so the admit WRAPS it to index 0
         *   and arms `broke` - and its `M 30,0` puts x at 260, with 140 of
         *   the feed still owed. That leftover is the debt the engine keeps
         *   too: nothing in its path writes [obj+0xa1], so the remainder of
         *   one step is the head start of the next, and no feed of 100 can
         *   land the accumulator on 0 when a threshold is not a divisor of
         *   100.
         * BOUND is the same clock with a two-row list whose rows move by
         * different amounts, so the COUNT of closures is visible in x: rows
         * of 100 with `M 100,0` and `M 1,0`, nfr = 2, and dt = 1706 ms =
         * 34 x 50 + 6 feeds 34 iterations. Each admits exactly one row, so
         * 34 closures, alternating 17 of each row: x = 100 + 17*100 +
         * 17*1 = 1817, back at index 0 after an even number of steps with
         * the last one a wrap, so `broke` is 1 - it is cleared at the head
         * of every iteration (:992) and only a wrap arms it. row_t is 0
         * (each 100-feed drains exactly against a 100 threshold) and
         * row_rem is 6. Call 2, dt = LL_ITER_MS, feeds 100 more and closes
         * exactly one row: x +100 = 1917, index 1, and `broke` 0, because
         * this step did not wrap. The M deltas are what make the count
         * readable: a single-row list cannot tell "one closure per step"
         * from "many", because a feed of 100k drains exactly to 0 either
         * way - which is why the first version of this leg let M3 through.
         * The bounds clamp at :1228 is why the deltas here are positive and
         * the window is wide.
         * MUTATIONS THIS LEG NOW COVERS (PR-135's run: the harness is
         * /tmp/opencode/pr135_mut2.py and each mutant's full selftest
         * output is kept in /tmp/opencode/pr135/mut2/). Admitting two
         * rows per iteration - the old loop - breaks this leg at its
         * FIRST step: the two closures it asks for become four, the
         * three-row list wraps inside the call instead of on the next
         * one, and every later count of the MULTI half fails with it
         * (M1_play_two_admits.play.log, 8 checks; it also breaks two
         * frame-index CHECKs in the frame-lookup leg above). Zeroing the
         * accumulator instead of carrying it - PR-110's old resync -
         * fails the row_t/row_rem CHECKs of BOTH halves here and 13
         * checks in the walk-law and distance legs
         * (M2_play_zero_carry.play.log, 15 in all). Feeding the row
         * clock per millisecond instead of per iteration is the widest
         * failure of the set: 77 checks (M4_play_per_ms_feed.play.log).
         * Dropping the parser's clamp so an authored 0 threshold stays 0,
         * as the engine stores it, still fails this leg's first fixture
         * CHECK - the 30/30 rows the parser must raise to 100 first
         * (M14_parser_no_thresh_clamp.play.log). */
        static const char RRW[] =
            "MULTI\nBOUND\n\n[MULTI]\ncontrol=BY_TIME\n"
            "1,0, M 100,0\n2,30, M 30,0\n3,30, M 30,0\n"
            "\n[BOUND]\ncontrol=BY_TIME\n1,100, M 100,0\n"
            "2,100, M 1,0\n";
        ll_mov_t *rm = calloc(1, sizeof *rm);
        ll_player rp;
        ll_play_keys rk = { 0 };

        CHECK(rm && ll_mov_parse(RRW, rm) == 2);
        CHECK(rm->st[0].nfr == 3 && rm->st[0].fr[0].thresh == 100u &&
              rm->st[0].fr[1].thresh == 30u && rm->st[0].fr[2].thresh == 30u);
        CHECK(rm->st[0].fr[0].dx == 100 && rm->st[0].fr[1].dx == 30 &&
              rm->st[1].nfr == 2 && rm->st[1].fr[0].dx == 100 &&
              rm->st[1].fr[1].dx == 1);
        ll_player_start(&rp, rm, 100, 100, 1);
        ll_player_start(&rp, rm, 100, 100, 1);
        CHECK(rp.sti == 0 && rp.fi == 0 && rp.row_t == 0u);
        ll_player_step(&rp, NULL, &rk, 0, 200000, 116);
        CHECK(rp.sti == 0 && rp.fi == 2 && !rp.broke);  /* TWO iterations =
                                                         * two closures only */
        CHECK(rp.row_t == 70u && rp.row_rem == 16u);
        CHECK(rp.x == 230.0);        /* 100 + +100 (row 0) + +30 (row 1) */
        ll_player_step(&rp, NULL, &rk, 0, 200000, LL_ITER_MS);
        CHECK(rp.fi == 0 && rp.broke == 1);              /* the last row of
                                                         * the list: a wrap */
        CHECK(rp.row_t == 140u && rp.row_rem == 16u);   /* the debt is carried */
        CHECK(rp.x == 260.0);       /* +30 (row 2)                        */
        ll_player_start(&rp, rm, 100, 100, 1);
        rp.sti = 1;
        rp.fi = 0;
        ll_player_step(&rp, NULL, &rk, 0, 200000, 1706);
        CHECK(rp.fi == 0 && rp.broke == 1);              /* 34 closures, the
                                                         * last one a wrap */
        CHECK(rp.row_t == 0u && rp.row_rem == 6u);      /* each feed drains
                                                         * exactly: no cap,
                                                         * no resync */
        CHECK(rp.x == 1817.0);      /* 17 x +100 and 17 x +1            */
        ll_player_step(&rp, NULL, &rk, 0, 200000, LL_ITER_MS);
        CHECK(rp.fi == 1 && rp.broke == 0);              /* one closure, no
                                                         * wrap */
        CHECK(rp.row_t == 0u && rp.row_rem == 6u);
        CHECK(rp.x == 1917.0);
        free(rm);
    }
    if (!rc)
        printf("play selftest: .plt physics (+ the '!' separator variant,"
               " + an authored-empty '0' ground is data not an error),"
               " extended .MOV transitions walk/turn/stop, self-transition"
               " keeps its state (frames keep running), edge fall, K_GEN2"
               " landing pulse, PR-83: declared-name spawn, key-gated walk"
               " stops on release, GEN frame pulses, wrap/time= arm nothing,"
               " horizontal impulse keeps the feet down, auto-walk stays,"
               " PR-85: facing-relative K_FWD/K_BKWD settle through the"
               " state pair / A_TURN+AC_BREAK, PR-86: (N) = target start"
               " row, authored AC_BREAK_NEXT ends a jump at its list wrap"
               " (landing no longer cuts it; fallback kept only without"
               " such a line), PR-87: comma groups AND (K_FIRE,K_F1"
               " attack entry - FIRE alone never fires it), PR-91: a raised"
               " AC_ENEMY_ATC drives the authored hit reaction (entry row,"
               " pullback impulse, its OWN AC_BREAK_NEXT wrap ends it),"
               " PR-108/148: the BY_TY climb runs the engine's input/"
               " guard law (mid-list entry row 3; with NO key held the"
               " 0x5441b mode-4 feed never runs - rows freeze, no"
               " wrap, no BREAK_NEXT; Up held gives the PR-108 spans"
               " 3,2,2,2,2,2,2,2,3; release mid-row keeps the carried"
               " 100 and wraps only when Down restarts it - the clock"
               " is sign-blind like the engine's cmp/je; x, y, v_x,"
               " v_y never move (the v_y-force path is counted open);"
               " !AC_DRAB drops him into A_FALL_DOWN from a state"
               " whose list never ticked), "
               "PR-110: the row clock is that iteration"
               " clock (a 300-unit row is 3 feeds, a 200-unit row 2, on"
               " the 28 ms AND the 20 ms grid, with the exact carry"
               " arithmetic checked), a mode-0 state's rows are fed by"
               " its distance only, and time_scale= pre-scales the rows"
               "PR-114: a BY_X state whose F= is 0,0 is"
               " NOT a walking one either - its velocity is frozen at the"
               " entry value (the law has no force term and, with no T="
               " authored, no drag term either; the same reading is why"
               " it never brakes on key release), its rows run on the"
               " ITERATION clock (one 100-unit row per feed, the sixth"
               " wraps the list and only that wrap arms BREAK_NEXT), and"
               " the distance accumulator stays exactly 0, PR-115: the"
               " state step admits ONE row per iteration, exactly as the"
               " engine's record step does (a 116 ms call is two"
               " iterations and closes exactly two rows of a"
               " 100/30/30 list, and the 70-unit remainder plus the 16"
               " ms carry are the debt the next call inherits; there is"
               " no cap and no resync any more, so a 1706 ms call is 34"
               " iterations and 34 rows, moved by their authored M"
               " deltas), and PR-135: the clock IS the constant"
               " LL_ITER_MS (asserted above), the velocity is the"
               " engine's own law - per iteration drag v -= v*T/100 then"
               " force v += F*s/100 for a held direction key, steady at"
               " F*s/T, below one register unit it is zero - and the two"
               " gaits PR-134 measured come out of the authored F/T and"
               " row pitch (the lion's 6-unit poses at 2.4 iterations"
               " each = 50 u/s, the worm's 4-unit poses at 3.2 ="
               " 25 u/s), with a released key leaving the authored drag"
               " to run out over 25 iterations (ok)\n");

    /* PR-199: the `end_level <n>` row tail, end to end INSIDE the
     * player. The engine side is PR-198's read (the parse arm
     * 0x46beb-0x46c01; the apply 0x45750 of class 0xa1048 writing the
     * level object's +0x4; the loop's compare against zero at 0x4ac55
     * that returns the constant 1 for any nonzero). What is pinned here
     * is the port's half: the number is parsed as DATA, the row admit
     * copies it into the exit-word slot, the frame's own motion is
     * unchanged by carrying it, and NOTHING here reads the value as a
     * number or clears it again - the loop's test is the only reader,
     * and ll_level.c's play leg is what turns a nonzero into the loop
     * return 1 (the contract half, code 1 = run on with no reset, is
     * pinned by stage_selftest). */
    {
        static const char EL[] =
            "A_WALK\nA_EXIT\n\n[A_WALK]\ncontrol=BY_TIME\nF=0,0\n"
            "T=0,0\nG=0,0\nA_EXIT(1)=K_JUMP\nseq=a\n1,100\n"
            "2,100,end_level 2\n3,100\n\n[A_EXIT]\ncontrol=BY_TIME\n"
            "F=0,0\nT=0,0\nG=0,0\nseq=a\n0,200\n";
        static const char ELP[] = "1\n1 0,100;2000,100;\n";
        ll_mov_t *em = calloc(1, sizeof *em);
        ll_plt ep;
        ll_player eq;
        ll_play_keys ek = { 0 };
        int ekk, fired = -1;

        CHECK(em && ll_plt_parse(ELP, &ep) == 0);
        CHECK(ll_mov_parse(EL, em) == 2);
        CHECK(em->st[0].nfr == 3);
        CHECK(em->st[0].fr[0].endlv == 0 &&
              em->st[0].fr[1].endlv == 2 &&        /* the authored number, */
              em->st[0].fr[2].endlv == 0);         /* data, and ONLY there */
        ll_player_start(&eq, em, 500, 100, 1);
        CHECK(eq.exit_word == 0);                  /* the ctor's zero      */
        for (ekk = 0; ekk < 10 && !eq.exit_word; ekk++) {
            ll_player_step(&eq, &ep, &ek, 0, 2000, LL_ITER_MS);
            if (eq.exit_word && fired < 0)
                fired = ekk;
        }
        CHECK(eq.exit_word == 2);                  /* the admit set it     */
        CHECK(fired == 1);                         /* exactly when the     */
                                                  /* state's 2nd row became */
                                                  /* current, never before */
        for (ekk = 0; ekk < 60; ekk++)            /* and NOTHING clears   */
            ll_player_step(&eq, &ep, &ek, 0, 2000, LL_ITER_MS);
                                                  /* it: the engine's loop */
                                                  /* body has no store to */
                                                  /* the slot either      */
        CHECK(eq.exit_word == 2);
        CHECK(eq.sti == 0);                        /* the tail is not an  */
                                                  /* arm: the state never */
                                                  /* left A_WALK         */
        ll_plt_free(&ep);
        free(em);
    }

    return rc;
}
