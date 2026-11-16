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
                if (take(&r, (uint32_t)n, &e->ml) < 0) break;
                if (take(&r, (uint32_t)n, &e->mr) < 0) break;
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



/* --- element shape geometry ----------------------------------------------
 * Statement-for-statement ports of the game's element-geometry functions
 * (full decompilation of 2026-11-16: build/decompiled/all/FUN_000420b0,
 * FUN_00042a70, FUN_00042de0, FUN_00042cf0; the loader side is the
 * if/else in FUN_00041f60). Margin bytes are read UNSIGNED where the
 * game reads unsigned (span tests and bbox values: 0xFF participates as
 * the value 255, only the 0xFF RUNS AT ROW EDGES are skipped as empty)
 * and SIGNED where the game reads signed (row counts, widths).
 *
 * The bounds matter, and no read ever leaves the lists: the left scan
 * skips a LEADING run of the empty marker and takes its minimum over the
 * rows after it; the right scan walks DOWN from the last row through a
 * TRAILING run of the marker, then takes its minimum over the rows below
 * that point. Inside those ranges the marker takes part in the minimum as
 * the plain value 255 - the game reads it unsigned, and so does this port.
 * 34 of the 14306 shape elements here declare rows whose margins are the
 * marker EVERYWHERE (both lists; measured 2026-11-16, no element has only
 * one list empty): both scans then end having found nothing, and the
 * game's box math stores w = 1 - total_w, h = 1 - rows. That is a fact
 * about its arithmetic, not a gap in the data, and the port reproduces it
 * exactly. ll_seq_shape_empty() flags those elements, so the C-vs-python
 * cross-check shows those lines agree too. */

#define SHAPE_EMPTY 0xff             /* the game's "empty row" byte      */
#define SENT_7FFF  0x7fff            /* the ctor sentinel the game tests */

static int marg_x(const ll_seq *s, uint32_t off, int i)
{
    uint32_t p = off + (uint32_t)i;
    if (i < 0 || p >= s->len)
        return SHAPE_EMPTY;          /* a parsed element's lists are inside
                                      * the payload, so this never fires;
                                      * the guard keeps every read where the
                                      * game's own read is */
    return s->blob[p];
}

/* which = 0: the +0x27 list, 1: the +0x2f list (names as in the game's
 * runtime element struct) */
static int marg(const ll_seq *s, const ll_seq_elem *e, int which, int i)
{
    return marg_x(s, which ? e->mr : e->ml, i);
}

/* the game's *element != 0x7fff anchor tests (FUN_00041f60 no-lists
 * branch, and the identical adjustments in FUN_00042a70/42de0) */
static int anch_x(const ll_seq_elem *e)
{
    return e->f0 == SENT_7FFF ? 0 : (int)(short)e->f0;
}

static int anch_y(const ll_seq_elem *e)
{
    return e->f2 == SENT_7FFF ? 0 : (int)(short)e->f2;
}

void ll_seq_shape_wh(const ll_seq_elem *e, int *w, int *h, int *n)
{
    *w = (int)(short)e->b19 * 0x50 + (int)(short)e->b15;
    *h = (int)(short)e->b17;
    *n = (e->w0 & 3) ? *h : 0;
}

/* FUN_000420b0, statement for statement (short truncations included).
 * The loader calls it exactly when w0&3 != 0; an element WITHOUT lists
 * keeps the implicit rect FUN_00041f60 derives from the words instead. */
void ll_seq_shape_bbox(const ll_seq *s, const ll_seq_elem *e,
                       int *x, int *y, int *w, int *h)
{
    int rows = (int)(short)e->b17;
    int tw = (int)(short)e->b19 * 0x50 + (int)(short)e->b15;
    int t, b, l, r, i;

    for (t = 0; t < rows && marg(s, e, 0, t) == 0xff; t++)
        ;
    l = tw;
    for (i = t; i < rows; i++) {
        int v = marg(s, e, 0, i);
        if (v < l)
            l = v;
    }
    b = rows - 1;
    while (b > -1) {
        if (marg(s, e, 1, b) != 0xff)
            break;
        b--;
    }
    r = tw;
    for (i = b; i > -1; i--) {
        int v = marg(s, e, 1, i);
        if (v < r)
            r = v;
    }
    /* game: +0x0c/+0x0e/+0x10/+0x12 = those four short values */
    *x = (short)(l - anch_x(e));
    *y = (short)(t - anch_y(e));
    *w = (short)(((tw - r) - l) + 1);
    *h = (short)((b - t) + 2);
}

/* 1 when the element declares margin rows but every margin byte of BOTH
 * lists is the empty marker: no solid span at all (34 of the 14306 shape
 * elements here; measured 2026-11-16, no element has only ONE list empty).
 * This is the input of the degenerate case in the header note: the game's
 * box math then stores w = 1 - total_w, h = 1 - rows, and this port
 * reproduces that arithmetic exactly. The flag only names the case for
 * the cross-check. */
static int marg_all_empty(const ll_seq *s, uint32_t off, int n)
{
    int i;

    for (i = 0; i < n; i++)
        if (s->blob[off + (uint32_t)i] != SHAPE_EMPTY)
            return 0;
    return 1;
}

int ll_seq_shape_empty(const ll_seq *s, const ll_seq_elem *e)
{
    if (!(e->w0 & 3) || e->n <= 0
        || e->ml + (uint32_t)e->n > s->len
        || e->mr + (uint32_t)e->n > s->len)
        return 0;
    return marg_all_empty(s, e->ml, e->n) && marg_all_empty(s, e->mr, e->n);
}

/* the row-overlap window, identical in FUN_00042a70 (0x42b85..0x42be6) and
 * FUN_00042de0 (0x42f21..0x42f7c) - read from the machine code of the LE
 * image (build/leo/leo.elf) on 2026-11-16, because the decompiled text of
 * these two functions loses the register arguments: with dy = ay2 - by2
 * (the positions AFTER the anchor/facing adjustments) the game indexes the
 * element whose y is lower from row 0 and the other one from the distance
 * between them, and compares the shorter remaining span:
 *   dy < 0  -> A from -dy, B from 0, count = min(rows_a + dy, rows_b)
 *   dy >= 0 -> A from 0, B from dy,  count = min(rows_b - dy, rows_a)
 * (0x42b97: eax = -dy, [esp+0x1c] = 0, then rows_a - eax vs rows_b;
 * 0x42bb9: [esp+0x1c] = dy, eax = 0, then rows_b - dy vs rows_a; a count
 * of 0 or less takes the "no overlap" return.)
 * Rows a=[*oa, *oa+*m), b=[*ob, *ob+*m); *m <= 0 = no overlap. */
static void shape_window(const ll_seq_elem *a, int ay2,
                         const ll_seq_elem *b, int by2, int *oa, int *ob,
                         int *m)
{
    int ah = (int)(short)a->b17, bh = (int)(short)b->b17;
    int dy = ay2 - by2;

    if (dy < 0) {
        *oa = -dy;                  /* A's top rows are above B's: they   */
        *ob = 0;                    /* take no part in the compare        */
        *m = ah + dy;
        if (*m > bh)
            *m = bh;
    } else {
        *oa = 0;
        *ob = dy;
        *m = bh - dy;
        if (*m > ah)
            *m = ah;
    }
}

/* FUN_00042a70. */
int ll_seq_shape_hit(const ll_seq *s,
                     const ll_seq_elem *a, int ax, int ay, int afacing,
                     const ll_seq_elem *b, int bx, int by, int bfacing)
{
    int aw, ah, an, bw, bh, bn;
    int ax2, ay2, bx2, by2, al, bl;
    int oa, ob, m, i;

    ll_seq_shape_wh(a, &aw, &ah, &an);
    ll_seq_shape_wh(b, &bw, &bh, &bn);
    if (an <= 0 || bn <= 0)       /* the game tests its +0x27 pointer */
        return 0;
    if (afacing < 0) {            /* facing mirrors: the two lists swap */
        al = 1;
        if (a->f0 != SENT_7FFF) {
            ay2 = ay - anch_y(a);
            ax2 = ax - (aw - anch_x(a));
        } else {
            ay2 = ay;
            ax2 = ax;
        }
    } else {
        al = 0;
        ay2 = ay - anch_y(a);
        ax2 = ax - anch_x(a);
    }
    if (bfacing < 0) {
        bl = 1;
        if (b->f0 != SENT_7FFF) {
            by2 = by - anch_y(b);
            bx2 = bx - (bw - anch_x(b));
        } else {
            by2 = by;
            bx2 = bx;
        }
    } else {
        bl = 0;
        by2 = by - anch_y(b);
        bx2 = bx - anch_x(b);
    }
    shape_window(a, ay2, b, by2, &oa, &ob, &m);
    if (m < 1)
        return 0;
    for (i = 0; i < m; i++) {
        int aml = marg(s, a, al, oa + i);
        int amr = marg(s, a, 1 - al, oa + i);
        int bml = marg(s, b, bl, ob + i);
        int bmr = marg(s, b, 1 - bl, ob + i);
        if (aml != 0xff && bml != 0xff &&
            ax2 + aml < bx2 + bw - bmr &&
            bx2 + bml < ax2 + aw - amr)
            return 1;             /* the game returns true at the first row */
    }
    return 0;
}
