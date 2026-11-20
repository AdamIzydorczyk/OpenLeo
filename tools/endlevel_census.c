/* endlevel_census.c - PR-199's data leg: count the `end_level <n>` rows
 * of the shipped .MOV scripts THROUGH THE PORT'S OWN PARSER.
 *
 * Why this exists: the row tail `end_level <n>` is the engine's
 * end-of-level producer (PR-198 read class 0xa1048's apply method
 * 0x45750 and the level loop's compare at 0x4ac55), and the port now
 * implements that path. What must stay measured rather than remembered
 * is where the shipped data actually puts the directive: the row counts,
 * their values, the states they sit in, and the fact that the levels the
 * play legs run carry none - so the new path is inert on every existing
 * frame pin. This tool reads the HD ANIMACJE container and parses every
 * .MOV member with ll_mov_parse (ported/ll_play.c, the parser the play
 * leg uses), one line per level block. Nothing is inferred: a member
 * that cannot be read or parsed is reported, never dropped in silence.
 *
 * usage: endlevel_census <ANIMACJE-container>
 */
#include "ll_play.h"
#include "llrs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VAL_MAX 8

static int n_members, n_unread, n_rows, n_members_with;
/* PR-217: the same walk also counts the `Wait <signal>` tails the parser
 * CAPTURES (ll_mov_fr.gsig_id/gneg). Nothing acts on them yet, so the
 * count is what the next PR's gate model will be measured against. */
static int n_wait_rows, n_wait_members, n_wait_nobit, n_wait_neg;
static int val_count[VAL_MAX];
static int level_rows;            /* rows in the level block being walked */
static int level_waits;           /* PR-217: same walk, `Wait` rows */

static void scan_member(const llrs *c, const llrs_slot *s, const char *lvl,
                        const char *leaf)
{
    unsigned char *txt = NULL;
    long len = llrs_read(c, s, &txt);
    ll_mov_t *m = calloc(1, sizeof *m);
    int k, found = 0, w_here = 0;

    if (len <= 0 || !m) {
        printf("  %s/%s: READ FAILED for a .MOV member (not counted, not"
               " skipped silently)\n", lvl, leaf);
        n_unread++;
        free(txt);
        free(m);
        return;
    }
    n_members++;
    if (ll_mov_parse((char *)txt, m) <= 0) {
        printf("  %s/%s: ll_mov_parse found no state block (not"
               " counted)\n", lvl, leaf);
        free(txt);
        free(m);
        return;
    }
    for (k = 0; k < m->nst; k++) {
        const ll_mov_st *st = &m->st[k];
        int j;
        for (j = 0; j < st->nfr; j++) {
            int v = st->fr[j].endlv;
            if (st->fr[j].gsig_id != -1) {
                n_wait_rows++;
                level_waits++;
                w_here++;
                if (st->fr[j].gsig_id == LL_SIG_NOBIT)
                    n_wait_nobit++;
                if (st->fr[j].gneg)
                    n_wait_neg++;
            }
            if (!v)
                continue;
            n_rows++;
            level_rows++;
            found++;
            if (v >= 0 && v < VAL_MAX)
                val_count[v]++;
            printf("  %s/%s state %d [%s] row %d (authored frame %d,"
                   " threshold %u) end_level %d\n", lvl, leaf, k, st->name,
                   j, st->fr[j].frame, st->fr[j].thresh, v);
        }
    }
    if (w_here)
        n_wait_members++;
    if (found)
        n_members_with++;
    free(txt);
    free(m);
}

static void walk_leaf(const llrs *c, const llrs_slot *s, const char *lvl,
                      const char *leaf, int *members_here)
{
    llrs_slot *r = NULL;
    unsigned n = 0, i;
    if (llrs_block(c, s, &r, &n))
        return;
    for (i = 0; i < n; i++) {
        size_t l;
        if (!r[i].name[0] || r[i].id != 1)
            continue;
        l = strlen(r[i].name);
        if (l > 4 && strcasecmp(r[i].name + l - 4, ".MOV") == 0) {
            scan_member(c, &r[i], lvl, leaf);
            (*members_here)++;
        }
    }
    llrs_block_free(r);
}

int main(int argc, char **argv)
{
    llrs c;
    unsigned i;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <ANIMACJE-container>\n", argv[0]);
        return 2;
    }
    if (llrs_open(&c, argv[1])) {
        fprintf(stderr, "endlevel_census: cannot open %s\n", argv[1]);
        return 1;
    }
    printf("end_level census, every .MOV member parsed with"
           " ported/ll_play.c's ll_mov_parse (the parser the play leg"
           " uses)\n");
    for (i = 0; i < c.cap; i++) {
        llrs_slot *leaves = NULL;
        unsigned nl = 0, j;
        int members_here = 0;
        if (c.slots[i].id != 2 || !c.slots[i].name[0])
            continue;
        if (llrs_block(&c, &c.slots[i], &leaves, &nl))
            continue;
        level_rows = 0;
        level_waits = 0;
        for (j = 0; j < nl; j++) {
            size_t l;
            if (!leaves[j].name[0])
                continue;
            l = strlen(leaves[j].name);
            if (leaves[j].id == 2)
                walk_leaf(&c, &leaves[j], c.slots[i].name, leaves[j].name,
                          &members_here);
            else if (leaves[j].id == 1 && l > 4 &&
                     strcasecmp(leaves[j].name + l - 4, ".MOV") == 0) {
                /* a .MOV sitting in the level block itself, not under a
                 * leaf: counted too, so nothing hides in the structure */
                scan_member(&c, &leaves[j], c.slots[i].name, "(direct)");
                members_here++;
            }
        }
        printf("LEVEL %s: %d .MOV member(s), %d end_level row(s),"
               " %d Wait-clause row(s) captured\n",
               c.slots[i].name, members_here, level_rows, level_waits);
        llrs_block_free(leaves);
    }
    llrs_close(&c);

    printf("census total: %d .MOV member(s) parsed, %d unreadable,"
           " %d member(s) carry the tail, %d row(s) with end_level\n",
           n_members, n_unread, n_members_with, n_rows);
    printf("Wait-clause capture (PR-217): %d member(s), %d row(s) carry a"
           " captured `Wait` clause, %d of them a name with NO bit in the"
           " engine's table (K_NEVER/K_ANY/unknown), %d negated with `!`;"
           " captured, and no model acts on it yet\n",
           n_wait_members, n_wait_rows, n_wait_nobit, n_wait_neg);
    for (i = 0; i < VAL_MAX; i++)
        if (val_count[i])
            printf("value %d: %d row(s)\n", i, val_count[i]);
    return n_unread ? 1 : 0;
}
