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

/* --- frame-patch decoder (grammar of FUN_0001dc80 draw blitter) -------- */

int ll_seq_patch_walk(const uint8_t *blob, uint32_t len,
                      ll_seq_patch_row_cb cb, void *ctx)
{
    uint32_t pos = 0;
    int nrows = 0;

    while (pos < len) {
        uint8_t h = blob[pos];
        if (!(h & 0x80))
            return -1;                       /* row header high bit */
        uint32_t rlen = h & 0x7F;
        pos++;
        if (rlen > len - pos)
            return -1;                       /* row overruns blob */
        const uint8_t *c = blob + pos;
        pos += rlen;
        ll_seq_patch_op ops[128];
        int nops = 0;
        int gap = -1;
        if (rlen) {
            gap = c[0];
            const uint8_t *q = c + 1, *end = c + rlen;
            int at_op = 1, last_fill = 0;
            while (q < end) {
                if (at_op) {
                    uint8_t op = *q++;
                    if (op >= 0x80) {        /* fill op + value byte */
                        if (q >= end) return -1;
                        ops[nops].kind = 1;
                        ops[nops].b = NULL;
                        ops[nops].val = *q++;
                        ops[nops].cnt = op & 0x7F;
                        nops++;
                        last_fill = 1;
                    } else {                 /* copy op, op bytes */
                        if (q + op > end) return -1;
                        ops[nops].kind = 0;
                        ops[nops].b = q;
                        ops[nops].val = 0;
                        ops[nops].cnt = op;
                        nops++;
                        q += op;
                        last_fill = 0;
                    }
                    at_op = 0;
                } else {                     /* marker position (FUN_0001dc80
                                              * 0x1dd59/0x1dd83 + tools/
                                              * seq_patch.py reference):
                                              * <0x40 = dst gap advance,
                                              * 0x40..0x7f after COPY run =
                                              * short fill, after FILL run =
                                              * short copy */
                    uint8_t b = *q;
                    if (b >= 0x80) return -1;      /* mid-content header */
                    if (b < 0x40) {          /* gap: advance dst, draw nothing */
                        ops[nops].kind = 2;
                        ops[nops].b = NULL;
                        ops[nops].val = 0;
                        ops[nops].cnt = b;
                        nops++;
                        q++;
                        at_op = 1;
                        continue;
                    }
                    if (last_fill) {         /* short copy */
                        uint8_t cnt = b & 0x3F;
                        if (q + 1 + cnt > end) return -1;
                        ops[nops].kind = 0;
                        ops[nops].b = q + 1;
                        ops[nops].val = 0;
                        ops[nops].cnt = cnt;
                        nops++;
                        q += 1 + cnt;
                        last_fill = 0;
                    } else {                 /* short fill */
                        if (q + 2 > end) return -1;
                        ops[nops].kind = 1;
                        ops[nops].b = NULL;
                        ops[nops].val = q[1];
                        ops[nops].cnt = b & 0x3F;
                        nops++;
                        q += 2;
                        last_fill = 1;
                    }
                    /* stays at marker position (runs chain); only a gap
                     * byte returns to an op position (seq_patch.py 83-99) */
                }
            }
        }
        cb(ctx, nrows, gap, ops, nops);
        nrows++;
    }
    return nrows;
}

/* --- frame-patch blitter (draw semantics: gap advance + run paints) ---- */

typedef struct {
    uint8_t *canvas;
    const uint8_t *map;                      /* palette-index remap, NULL = identity */
    int cw, ch, x, y;
} patch_paint;

static void patch_row_paint(void *vctx, int row, int gap,
                            const ll_seq_patch_op *ops, int nops)
{
    patch_paint *pp = vctx;
    if (gap < 0)
        return;                              /* empty row: nothing drawn */
    int px = pp->x + gap, py = pp->y + row;
    for (int i = 0; i < nops; i++) {
        const ll_seq_patch_op *o = &ops[i];
        if (o->kind == 2) {                  /* gap: advance, draw nothing */
            px += o->cnt;
            continue;
        }
        for (int k = 0; k < o->cnt; k++) {
            uint8_t v = o->kind == 0 ? o->b[k] : (uint8_t)o->val;
            if (pp->map)
                v = pp->map[v];
            if (px >= 0 && px < pp->cw && py >= 0 && py < pp->ch)
                pp->canvas[(size_t)py * pp->cw + px] = v;
            px++;
        }
    }
}

int ll_seq_patch_draw(const uint8_t *blob, uint32_t len, uint8_t *canvas,
                      int cw, int ch, int x, int y)
{
    return ll_seq_patch_draw_map(blob, len, canvas, cw, ch, x, y, NULL);
}

/* Same blit with every painted palette index put through map[] (identity
 * when map == NULL, so the plain path is bit-for-bit what it was). */
int ll_seq_patch_draw_map(const uint8_t *blob, uint32_t len, uint8_t *canvas,
                          int cw, int ch, int x, int y, const uint8_t *map)
{
    patch_paint pp = { canvas, map, cw, ch, x, y };
    return ll_seq_patch_walk(blob, len, patch_row_paint, &pp);
}

/* Elements are animation frames: entries are sprite chunks side by side,
 * chunks 0..arr_n-2 are 80 px wide and the last is b15 px wide (blitter
 * called with w=0x50 per chunk, w=int16(b15) for the tail). Screen pos of
 * the frame = (-f0, -f2) from the element bbox (FUN_000420b0). */
int ll_seq_draw_elem(const ll_seq *s, const ll_seq_elem *e,
                     uint8_t *canvas, int cw, int ch, int x, int y)
{
    return ll_seq_draw_elem_map(s, e, canvas, cw, ch, x, y, NULL);
}

int ll_seq_draw_elem_map(const ll_seq *s, const ll_seq_elem *e,
                         uint8_t *canvas, int cw, int ch, int x, int y,
                         const uint8_t *map)
{
    for (uint32_t i = 0; i < e->arr_n; i++) {
        const ll_seq_ent *t = &s->ents[e->arr_first + i];
        if (ll_seq_patch_draw_map(s->blob + t->off, t->len, canvas, cw, ch,
                                  x + 80 * (int)i, y, map) < 0)
            return -1;
    }
    return 0;
}
