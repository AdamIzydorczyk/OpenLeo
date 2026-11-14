/* ll_seq.c - .SEQ animation-script parser (byte-exact port of the game's
 * loader chain FUN_00043e70/FUN_00041f60 and of tools/seq_parse.py).
 * All 49 container .SEQ payloads parse with consumed == len. */
#include "ll_seq.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
    const uint8_t *p;
    uint32_t n, pos;
    int err;
} rd;

static uint16_t u16(rd *r)
{
    if (r->pos + 2 > r->n) { r->err = 1; return 0; }
    uint16_t v = (uint16_t)(r->p[r->pos] | (r->p[r->pos + 1] << 8));
    r->pos += 2;
    return v;
}

static int take(rd *r, uint32_t k, uint32_t *off)
{
    if (r->err) return -1;
    if (r->pos + k > r->n) { r->err = 1; return -1; }
    if (off) *off = r->pos;
    r->pos += k;
    return 0;
}

static int add_ent(ll_seq *s, uint32_t off, uint32_t len)
{
    if (s->n_ents == s->cap_ents) {
        uint32_t cap = s->cap_ents ? s->cap_ents * 2 : 16;
        ll_seq_ent *e = realloc(s->ents, cap * sizeof(*e));
        if (!e) return -1;
        s->ents = e;
        s->cap_ents = cap;
    }
    s->ents[s->n_ents].off = off;
    s->ents[s->n_ents].len = len;
    s->n_ents++;
    return 0;
}

int ll_seq_parse(const uint8_t *blob, uint32_t len, ll_seq *out)
{
    rd r = { blob, len, 0, 0 };
    ll_seq s;

    memset(&s, 0, sizeof s);
    s.blob = blob;
    s.len = len;

    uint16_t count = u16(&r);
    if (r.err) { ll_seq_free(&s); return -1; }
    s.count = count;
    s.els = calloc(count ? count : 1, sizeof *s.els);
    if (!s.els) { ll_seq_free(&s); return -1; }

    for (uint32_t i = 0; i < count; i++) {
        ll_seq_elem *e = &s.els[i];
        e->off = r.pos;
        e->w0  = u16(&r);
        e->b19 = u16(&r);
        e->b15 = u16(&r);
        e->b17 = u16(&r);
        e->f0  = u16(&r);
        e->f2  = u16(&r);
        e->f4  = u16(&r);
        e->f6  = u16(&r);
        uint16_t w0 = e->w0;
        if (w0 & 4) {
            e->f8 = u16(&r);
            e->fA = u16(&r);
        }
        if (w0 & 3) {
            int16_t n = (int16_t)e->b17;    /* signed like the game's MOVSX */
            if (n <= 0) {
                /* game reads with a negative ESI: loop runs zero times */
                e->n = 0;
            } else {
                e->n = n;
                if (take(&r, (uint32_t)n, &e->pal) < 0) break;
                if (take(&r, (uint32_t)n, &e->spr) < 0) break;
            }
        }
        if (!(w0 & 2 && !(w0 & 1))) {
            e->arr_present = 1;
            e->arr_first = s.n_ents;
            int16_t m = (int16_t)e->b19;
            if (m >= 0) {
                for (int32_t j = 0; j <= m; j++) {
                    uint16_t L = u16(&r);
                    if (r.err) break;
                    uint32_t off;
                    if (take(&r, L, &off) < 0) break;
                    if (add_ent(&s, off, L) < 0) { r.err = 1; break; }
                    e->arr_n++;
                }
            }
        }
        if (r.err) break;
    }
    if (r.err) { ll_seq_free(&s); return -1; }

    s.consumed = r.pos;
    *out = s;
    return 0;
}

void ll_seq_free(ll_seq *s)
{
    free(s->els);
    free(s->ents);
    memset(s, 0, sizeof *s);
}
