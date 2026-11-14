/* The menu composites its ENTIRE frame on the CPU (PCX decode + palette
 * LUT, .SEQ sprite overlays, 8x8 bitmap font, cursor '>') into one
 * ARGB8888 buffer and hands it to the platform (platform.h, T-01):
 * plat_present_argb uploads exactly those pixels, unchanged. Input
 * arrives as ll_events - keys verbatim ASCII / LLKEY_*, mouse in
 * logical canvas coords (the P-10 /sc math stays deleted). This file
 * contains no SDL.
 * Fixed pacing: event-driven with the platform sleep, no busy loop.
 * See ll_menu.h. */
#include "ll_menu.h"
#include "fs_ci.h"
#include "ll_menu_dac.h"
#include "ll_pcx.h"
#include "ll_seq.h"
#include "ll_level.h"
#include "platform.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SX(v) ((int)(short)(v))

/* 8x8 font, row-major bytes, bit 7 = leftmost pixel. */
static const unsigned char FONT[95][8] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, /* ' ' */
    {0x18,0x18,0x18,0x18,0x18,0x00,0x18,0x00}, /* '!' */
    {0x6C,0x6C,0x00,0x00,0x00,0x00,0x00,0x00}, /* '"' */
    {0x08,0x3E,0x28,0x3E,0x08,0x3E,0x08,0x00}, /* '#' approx */
    {0x08,0x1E,0x28,0x1C,0x0A,0x3C,0x08,0x00}, /* '$' approx */
    {0x62,0x64,0x08,0x10,0x2C,0x4C,0x00,0x00}, /* '%' approx */
    {0x10,0x2A,0x2C,0x10,0x36,0x2A,0x44,0x00}, /* '&' approx */
    {0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x00}, /* '\'' */
    {0x08,0x10,0x20,0x20,0x20,0x10,0x08,0x00}, /* '(' */
    {0x08,0x04,0x02,0x02,0x02,0x04,0x08,0x00}, /* ')' */
    {0x08,0x3E,0x08,0x1C,0x28,0x00,0x00,0x00}, /* '*' approx */
    {0x08,0x08,0x3E,0x08,0x08,0x00,0x00,0x00}, /* '+' */
    {0x00,0x00,0x00,0x00,0x30,0x10,0x20,0x00}, /* ',' */
    {0x00,0x00,0x3E,0x00,0x00,0x00,0x00,0x00}, /* '-' */
    {0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00}, /* '.' */
    {0x02,0x04,0x08,0x10,0x20,0x40,0x00,0x00}, /* '/' */
    {0x1C,0x22,0x2E,0x32,0x3A,0x22,0x1C,0x00}, /* '0' */
    {0x18,0x28,0x08,0x08,0x08,0x08,0x3E,0x00}, /* '1' */
    {0x1C,0x22,0x02,0x0C,0x10,0x20,0x3E,0x00}, /* '2' */
    {0x3E,0x02,0x0C,0x02,0x22,0x22,0x1C,0x00}, /* '3' */
    {0x04,0x0C,0x14,0x24,0x3E,0x04,0x04,0x00}, /* '4' */
    {0x3E,0x20,0x3C,0x02,0x02,0x22,0x1C,0x00}, /* '5' */
    {0x1C,0x20,0x3C,0x22,0x22,0x22,0x1C,0x00}, /* '6' */
    {0x3E,0x22,0x04,0x08,0x10,0x10,0x10,0x00}, /* '7' */
    {0x1C,0x22,0x22,0x1C,0x22,0x22,0x1C,0x00}, /* '8' */
    {0x1C,0x22,0x22,0x1E,0x02,0x02,0x1C,0x00}, /* '9' */
    {0x00,0x00,0x18,0x18,0x00,0x18,0x18,0x00}, /* ':' */
    {0x00,0x00,0x18,0x18,0x00,0x18,0x08,0x10}, /* ';' */
    {0x04,0x08,0x10,0x20,0x10,0x08,0x04,0x00}, /* '<' */
    {0x00,0x3E,0x00,0x00,0x3E,0x00,0x00,0x00}, /* '=' */
    {0x20,0x10,0x08,0x04,0x08,0x10,0x20,0x00}, /* '>' */
    {0x1C,0x22,0x02,0x0C,0x18,0x00,0x18,0x00}, /* '?' */
    {0x1C,0x22,0x2E,0x2A,0x2E,0x20,0x1C,0x00}, /* '@' approx */
    {0x08,0x1C,0x22,0x22,0x22,0x1C,0x08,0x00}, /* 'A' */
    {0x3C,0x22,0x3C,0x22,0x22,0x22,0x3C,0x00}, /* 'B' */
    {0x1C,0x22,0x20,0x20,0x20,0x22,0x1C,0x00}, /* 'C' */
    {0x3C,0x22,0x22,0x22,0x22,0x22,0x3C,0x00}, /* 'D' */
    {0x3E,0x20,0x3C,0x20,0x20,0x20,0x3E,0x00}, /* 'E' */
    {0x3E,0x20,0x3C,0x20,0x20,0x20,0x20,0x00}, /* 'F' */
    {0x1C,0x22,0x20,0x2E,0x22,0x22,0x1C,0x00}, /* 'G' */
    {0x22,0x22,0x3E,0x22,0x22,0x22,0x22,0x00}, /* 'H' */
    {0x1C,0x08,0x08,0x08,0x08,0x08,0x1C,0x00}, /* 'I' */
    {0x0E,0x04,0x04,0x04,0x04,0x24,0x18,0x00}, /* 'J' */
    {0x24,0x28,0x30,0x38,0x28,0x24,0x22,0x00}, /* 'K' approx */
    {0x20,0x20,0x20,0x20,0x20,0x20,0x3E,0x00}, /* 'L' */
    {0x22,0x36,0x36,0x2A,0x22,0x22,0x22,0x00}, /* 'M' */
    {0x22,0x22,0x12,0x08,0x14,0x22,0x22,0x00},  /* 'N' */
    {0x1C,0x22,0x22,0x22,0x22,0x22,0x1C,0x00}, /* 'O' */
    {0x3C,0x22,0x22,0x3C,0x20,0x20,0x20,0x00}, /* 'P' */
    {0x1C,0x22,0x22,0x22,0x2A,0x24,0x1A,0x00}, /* 'Q' */
    {0x3C,0x22,0x22,0x3C,0x28,0x24,0x22,0x00}, /* 'R' */
    {0x1E,0x20,0x1C,0x02,0x02,0x22,0x1C,0x00}, /* 'S' */
    {0x3E,0x08,0x08,0x08,0x08,0x08,0x08,0x00}, /* 'T' */
    {0x22,0x22,0x22,0x22,0x22,0x22,0x1C,0x00}, /* 'U' */
    {0x22,0x22,0x22,0x22,0x14,0x14,0x08,0x00}, /* 'V' */
    {0x22,0x22,0x2A,0x2A,0x36,0x22,0x22,0x00}, /* 'W' */
    {0x22,0x22,0x14,0x08,0x14,0x22,0x22,0x00}, /* 'X' */
    {0x14,0x14,0x14,0x08,0x08,0x08,0x08,0x00}, /* 'Y' */
    {0x3E,0x02,0x04,0x08,0x10,0x20,0x3E,0x00}, /* 'Z' */
    {0x1C,0x10,0x10,0x10,0x10,0x10,0x1C,0x00}, /* '[' */
    {0x40,0x20,0x10,0x08,0x04,0x02,0x00,0x00}, /* '\' */
    {0x1C,0x04,0x04,0x04,0x04,0x04,0x1C,0x00}, /* ']' */
    {0x08,0x14,0x22,0x00,0x00,0x00,0x00,0x00}, /* '^' */
    {0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x00}, /* '_' */
    {0x10,0x08,0x00,0x00,0x00,0x00,0x00,0x00}, /* '`' */
    {0x00,0x1C,0x02,0x1E,0x22,0x22,0x1E,0x00}, /* 'a' */
    {0x30,0x28,0x3C,0x22,0x22,0x22,0x3C,0x00}, /* 'b' */
    {0x1E,0x20,0x38,0x24,0x24,0x24,0x38,0x00}, /* 'c' approx */
    {0x0C,0x12,0x1E,0x12,0x12,0x12,0x1C,0x00}, /* 'd' approx */
    {0x1C,0x22,0x3E,0x20,0x20,0x22,0x1C,0x00}, /* 'e' */
    {0x1C,0x12,0x10,0x38,0x10,0x10,0x10,0x00}, /* 'f' approx */
    {0x1C,0x12,0x12,0x1C,0x02,0x02,0x1C,0x00}, /* 'g' approx */
    {0x22,0x22,0x3E,0x22,0x22,0x22,0x22,0x00}, /* 'h' approx (H) */
    {0x1C,0x08,0x08,0x08,0x08,0x08,0x1C,0x00}, /* 'i' (I) */
    {0x0E,0x04,0x04,0x04,0x04,0x24,0x18,0x00}, /* 'j' (J) */
    {0x24,0x28,0x30,0x38,0x28,0x24,0x22,0x00}, /* 'k' approx */
    {0x20,0x20,0x20,0x20,0x20,0x20,0x3E,0x00}, /* 'l' (L) */
    {0x22,0x36,0x36,0x2A,0x22,0x22,0x22,0x00}, /* 'm' approx */
    {0x22,0x22,0x12,0x08,0x14,0x22,0x22,0x00}, /* 'n' approx */
    {0x1C,0x22,0x22,0x22,0x22,0x22,0x1C,0x00}, /* 'o' (O) */
    {0x3C,0x22,0x22,0x3C,0x20,0x20,0x20,0x00}, /* 'p' approx */
    {0x1C,0x22,0x22,0x22,0x2A,0x24,0x1A,0x00}, /* 'q' approx */
    {0x3C,0x22,0x22,0x3C,0x28,0x24,0x22,0x00}, /* 'r' approx */
    {0x1E,0x20,0x1C,0x02,0x02,0x22,0x1C,0x00}, /* 's' approx */
    {0x08,0x38,0x08,0x08,0x08,0x08,0x0C,0x00}, /* 't' approx */
    {0x12,0x12,0x12,0x12,0x12,0x12,0x0C,0x00}, /* 'u' approx */
    {0x12,0x12,0x12,0x12,0x0C,0x0C,0x08,0x00}, /* 'v' approx */
    {0x22,0x22,0x2A,0x2A,0x36,0x22,0x22,0x00}, /* 'w' approx */
    {0x12,0x14,0x08,0x08,0x14,0x12,0x00,0x00}, /* 'x' approx */
    {0x12,0x12,0x12,0x0C,0x08,0x08,0x08,0x00}, /* 'y' approx */
    {0x1E,0x04,0x08,0x10,0x20,0x3E,0x00,0x00}, /* 'z' approx */
    {0x1C,0x08,0x10,0x20,0x10,0x08,0x1C,0x00}, /* '{' */
    {0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x00}, /* '|' */
    {0x1C,0x10,0x08,0x04,0x08,0x10,0x1C,0x00}, /* '}' */
    {0x30,0x18,0x0C,0x00,0x00,0x00,0x00,0x00}, /* '~' approx */
};

static void draw_glyph(uint32_t *fb, int w, int h, int x, int y, char c,
                       uint32_t col)
{
    if (c < 32 || c > 126)
        c = '?';
    const unsigned char *g = FONT[c - 32];
    for (int r = 0; r < 8; r++)
        for (int b = 0; b < 8; b++)
            if (g[r] & (0x80 >> b)) {
                int px = x + b, py = y + r;
                if (px >= 0 && px < w && py >= 0 && py < h)
                    fb[py * w + px] = col;
            }
}

static void draw_text(uint32_t *fb, int w, int h, int x, int y, const char *s,
                      uint32_t col)
{
    for (; *s; s++, x += 8)
        draw_glyph(fb, w, h, x, y, *s, col);
}

/* 1-px outline (hover highlight on map zones/islands/corners) */
static void draw_rect(uint32_t *fb, int w, int h, int x0, int y0, int x1,
                      int y1, uint32_t col)
{
    for (int x = x0; x < x1; x++)
        for (int y = y0; y < y1; y++)
            if (x >= 0 && x < w && y >= 0 && y < h &&
                (y == y0 || y == y1 - 1 || x == x0 || x == x1 - 1))
                fb[(size_t)y * w + x] = col;
}

/* solid horizontal line (answer-word selection/hover underline: a box
 * over the cursive words covered them, an under-line never does) */
static void draw_hline(uint32_t *fb, int w, int h, int x0, int x1, int y,
                       uint32_t col)
{
    if (y < 0 || y >= h)
        return;
    for (int x = x0; x < x1; x++)
        if (x >= 0 && x < w)
            fb[(size_t)y * w + x] = col;
}

/* Menu timing is millisecond-based on purpose (30 ms redraw ticks,
 * LEO_MENU_SECS deadlines); derive it from the ONE platform clock so
 * there is a single time source (T-07 pattern). */
static uint64_t now_ms(void)
{
    return plat_now_ns() / 1000000ull;
}

static void menu_anims_free(ll_seq *aseq, int nanims)
{
    if (!aseq)
        return;
    for (int i = 0; i < nanims; i++)
        if (aseq[i].els)
            ll_seq_free(&aseq[i]);
    free(aseq);
}

/* Painted extent of one .SEQ element: side-by-side sprite chunks
 * (arr_n-1 x 80 px + b15 px tail), height = patch row count (mirrors
 * the blitter chunk widths; same computation as ll_level.c elem_size). */
typedef struct { int maxrow; } menu_rowctx;
static void menu_rowcount_cb(void *ctx, int row, int gap,
                             const ll_seq_patch_op *ops, int nops)
{
    (void)gap; (void)ops; (void)nops;
    menu_rowctx *rc = ctx;
    if (row + 1 > rc->maxrow)
        rc->maxrow = row + 1;
}
static void elem_extent(const ll_seq *s, const ll_seq_elem *e, int *w, int *h)
{
    int ww = 0, hh = 0;
    for (unsigned k = 0; k < e->arr_n; k++) {
        const ll_seq_ent *en = &s->ents[e->arr_first + k];
        ww += (k + 1 < e->arr_n) ? 80 : (int)(short)e->b15;
        menu_rowctx rc = { 0 };
        ll_seq_patch_walk(s->blob + en->off, en->len, menu_rowcount_cb, &rc);
        if (rc.maxrow > hh)
            hh = rc.maxrow;
    }
    *w = ww;
    *h = hh;
}

/* ---- selected-answer-word bright-green tint ------------------------------
 * Measured 2026-11-15 off ANIMACJE/MENU + the OPTIONS.PCX palette the
 * restart dialog LUTs through: indices 2..22 of that palette are ONE
 * monotonic green ramp (idx2 #010a01 L6.3, idx8 #044f08 L48.5, idx15
 * #0aa712 L103.1, idx16 #0ab311 L110.0, idx22 #10f91b L154.0); idx1
 * #090202 is the sprite's near-black key.  The two answer words differ only
 * by where their ramp sits: TAK.SEQ paints idx3..15 (403 px of 15, so it
 * reads mid-green) while CNIE.SEQ paints idx2..22 (398 px of 22 = bright
 * green, hence "NIE seems active whole time") and X.SEQ tops out at 22 too.
 * So the selection state is the ramp position: blit the selected word's
 * ink shifted up the ramp until its brightest used index is GREEN_HI.
 * TAK shifts +7 (3..15 -> 10..22), CNIE and X shift 0 = byte-exact no-op,
 * i.e. the frame the CI already pins stays identical. */
#define GREEN_LO 2
#define GREEN_HI 22
/* round 7: idle (unselected) words sit at the authored top of the DIMMER
 * word (TAK's idx15): CNIE/X shift DOWN to it, TAK's own idle state is a
 * byte-exact no-op. The 2 frames every answer sprite carries (measured
 * 2026-11-15: identical shape, ink oscillating along the green ramp,
 * period 100 ms in the data) are the original's own pulse: the SELECTED
 * word cycles them (animated button), unselected words freeze on frame 0.
 * The .MNU selector state machines are a CD hole, so this idle/selected
 * split is a labeled reconstruction (PLAN 8b), built ONLY from measured
 * palette/ramp/frame facts. */
#define GREEN_IDLE_TOP 15          /* idx15 #0aa712: TAK's authored top   */

static void patch_hist_cb(void *vctx, int row, int gap,
                          const ll_seq_patch_op *ops, int nops)
{
    (void)row; (void)gap;
    unsigned char *seen = vctx;
    for (int i = 0; i < nops; i++) {
        const ll_seq_patch_op *o = &ops[i];
        if (o->kind == 2)
            continue;                        /* gap paints nothing */
        if (o->kind == 1)
            seen[o->val & 0xffu] = 1;
        else
            for (int k = 0; k < o->cnt; k++)
                seen[o->b[k]] = 1;
    }
}

/* Identity map unless the sprite paints the green ramp below its top:
 * then every ramp index it uses moves up by the same amount (the shading
 * order is preserved). Returns the shift applied (0 = nothing tinted). */
static int menu_tint_build(const ll_seq *s, unsigned char *map)
{
    for (int i = 0; i < 256; i++)
        map[i] = (unsigned char)i;
    if (!s->els)
        return 0;
    unsigned char seen[256] = { 0 };
    for (unsigned fr = 0; fr < s->count; fr++) {
        const ll_seq_elem *e = &s->els[fr];
        for (unsigned k = 0; k < e->arr_n; k++) {
            const ll_seq_ent *en = &s->ents[e->arr_first + k];
            ll_seq_patch_walk(s->blob + en->off, en->len, patch_hist_cb, seen);
        }
    }
    int top = -1;
    for (int i = GREEN_HI; i >= GREEN_LO; i--)
        if (seen[i]) {
            top = i;
            break;
        }
    if (top < 0 || top >= GREEN_HI)
        return 0;                            /* no ramp, or already brightest */
    int sh = GREEN_HI - top;
    for (int i = GREEN_LO; i <= GREEN_HI; i++)
        if (seen[i]) {
            int v = i + sh;
            map[i] = (unsigned char)(v > GREEN_HI ? GREEN_HI : v);
        }
    return sh;
}

/* round 7 (user: "nie dont change"): CNIE/X top the ramp already, so the
 * SELECT shift is +0 for them and selecting them changed nothing. Shift
 * them DOWN to GREEN_IDLE_TOP while UNSELECTED instead: every answer word
 * then changes in BOTH directions (bright + animated when selected, dim +
 * frozen when not), and TAK's idle stays byte-exact (authored top 15). */
static int menu_tint_idle_build(const ll_seq *s, unsigned char *map)
{
    for (int i = 0; i < 256; i++)
        map[i] = (unsigned char)i;
    if (!s->els)
        return 0;
    unsigned char seen[256] = { 0 };
    for (unsigned fr = 0; fr < s->count; fr++) {
        const ll_seq_elem *e = &s->els[fr];
        for (unsigned k = 0; k < e->arr_n; k++) {
            const ll_seq_ent *en = &s->ents[e->arr_first + k];
            ll_seq_patch_walk(s->blob + en->off, en->len, patch_hist_cb, seen);
        }
    }
    int top = -1;
    for (int i = GREEN_HI; i >= GREEN_LO; i--)
        if (seen[i]) {
            top = i;
            break;
        }
    if (top < 0 || top <= GREEN_IDLE_TOP)
        return 0;                    /* no ramp, or authored dim already  */
    int sh = top - GREEN_IDLE_TOP;
    for (int i = GREEN_LO; i <= GREEN_HI; i++)
        if (seen[i]) {
            int v = i - sh;
            map[i] = (unsigned char)(v < GREEN_LO ? GREEN_LO : v);
        }
    return -sh;
}

/* CI hook: dump the composed RGB frame as PPM (P6) */
static int ppm_fb(const char *path, const uint32_t *fb, int w, int h)
{
    FILE *f = ll_fopen_ci(path, "wb");
    if (!f)
        return -1;
    fprintf(f, "P6\n%d %d\n255\n", w, h);
    for (int i = 0; i < w * h; i++) {
        unsigned char c[3] = { (unsigned char)(fb[i] >> 16),
                               (unsigned char)(fb[i] >> 8),
                               (unsigned char)fb[i] };
        if (fwrite(c, 1, 3, f) != 3) { fclose(f); return -1; }
    }
    return fclose(f) == 0 ? 0 : -1;
}

int ll_menu_open(const unsigned char *bg_pcx, unsigned bg_len,
                 const char *const *items, int nitems,
                 const ll_menu_anim *anims, int nanims)
{
    return ll_menu_open_ex(bg_pcx, bg_len, items, nitems, anims, nanims,
                           NULL, 0);
}

int ll_menu_open_ex(const unsigned char *bg_pcx, unsigned bg_len,
                    const char *const *items, int nitems,
                    const ll_menu_anim *anims, int nanims,
                    const ll_menu_zone *zones, int nzones)
{
    if (nitems < 1)
        return -1;

    /* background image (optional) */
    ll_pcx bg;
    memset(&bg, 0, sizeof bg);
    if (bg_pcx && ll_pcx_decode(bg_pcx, bg_len, &bg) != 0)
        fprintf(stderr, "ll_menu: background PCX decode failed\n");
    if (bg_pcx && bg.w)
        printf("menu: background OPTIONS.PCX %ux%u loaded\n", bg.w, bg.h);
    int W = bg.w ? (int)bg.w : 320, H = bg.h ? (int)bg.h : 200;

    /* overlay animations: parsed once; frames cycled off the redraw tick */
    ll_seq *aseq = nanims > 0 ? calloc((size_t)nanims, sizeof *aseq) : NULL;
    int nanim_ok = 0;
    if (aseq) {
        if (!bg.idx) {
            fprintf(stderr, "menu: .SEQ anims need a PCX palette; skipped\n");
            menu_anims_free(aseq, nanims);
            aseq = NULL;
            nanims = 0;
        } else {
            for (int i = 0; i < nanims; i++) {
                if (ll_seq_parse(anims[i].seq, anims[i].len, &aseq[i]) != 0) {
                    fprintf(stderr, "menu: anim %d .SEQ parse failed\n", i);
                    continue;
                }
                if (aseq[i].count == 0) {
                    /* E-04: an accepted-but-empty .SEQ would divide by zero
                     * in the frame cycle and read els[0]; drop it (free()
                     * zeroes els, so the draw loops skip it) */
                    fprintf(stderr, "menu: anim %d: 0-frame .SEQ ignored\n", i);
                    ll_seq_free(&aseq[i]);
                    continue;
                }
                const ll_seq_elem *e0 = &aseq[i].els[0];
                printf("menu: anim %d: %u frames at (%d,%d) period %ums\n",
                       i, aseq[i].count, -SX(e0->f0), -SX(e0->f2),
                       anims[i].period_ms ? anims[i].period_ms : 100);
                nanim_ok++;
            }
            if (!nanim_ok) {
                menu_anims_free(aseq, nanims);
                aseq = NULL;
            }
        }
    }

    /* T-01/T-12: the platform owns the window; the logical viewport is
     * the content size, so mouse events arrive in canvas coordinates
     * (the old /sc math stays deleted). */
    if (plat_init("Lew Leon - menu", W, H) < 0) {
        menu_anims_free(aseq, nanims);
        ll_pcx_free(&bg);
        return -2;
    }
    plat_raise(); /* dialog re-entry: ask the WM for focus */

    uint32_t *fb = (uint32_t *)malloc((size_t)W * H * sizeof *fb);
    unsigned char *cv = malloc((size_t)W * H);
    if (!fb || !cv) {
        free(fb);
        free(cv);
        menu_anims_free(aseq, nanims);
        ll_pcx_free(&bg);
        return -2;
    }

    int sel = 0, quit = 0, ret = -1;
    int hov_z = -1, hov_r = -1;    /* hover indicators (mouse motion)     */
    int zsel = -1;      /* keyboard-selected answer zone (-1 = list row) */
    unsigned tick = 0;   /* fixed 30 ms redraw ticks drive anim frames */

    /* hot-zone rects in canvas px: the union over frames of the bound
     * anim's painted extent (see ll_menu.h zone docs) */
    enum { MAXZ = 8 };
    int nz = zones && nzones > 0 ? (nzones > MAXZ ? MAXZ : nzones) : 0;
    int zrect[MAXZ][4];
    for (int zi = 0; zi < nz; zi++) {
        zrect[zi][0] = zrect[zi][1] = zrect[zi][2] = zrect[zi][3] = 0;
        if (!zones[zi].tag || !aseq)
            continue;
        for (int i = 0; i < nanims; i++) {
            if (!anims[i].tag || strcmp(anims[i].tag, zones[zi].tag) != 0 ||
                !aseq[i].els)
                continue;
            int bx0 = 1 << 20, by0 = 1 << 20, bx1 = -1, by1 = -1;
            for (unsigned fr = 0; fr < aseq[i].count; fr++) {
                const ll_seq_elem *e = &aseq[i].els[fr];
                int ex = -SX(e->f0), ey = -SX(e->f2), ew = 0, eh = 0;
                elem_extent(&aseq[i], e, &ew, &eh);
                if (ex < bx0) bx0 = ex;
                if (ey < by0) by0 = ey;
                if (ex + ew > bx1) bx1 = ex + ew;
                if (ey + eh > by1) by1 = ey + eh;
            }
            if (bx1 > 0 && by1 > 0) {
                zrect[zi][0] = bx0; zrect[zi][1] = by0;
                zrect[zi][2] = bx1; zrect[zi][3] = by1;
                printf("menu: zone %s (%d,%d)-(%d,%d) key '%c' -> %d\n",
                       zones[zi].tag, bx0, by0, bx1, by1,
                       zones[zi].key ? (char)zones[zi].key : '-',
                       zones[zi].ret);
            }
            break;
        }
    }
    /* Selection tint (ll_menu.h round 6/7): two remap tables per zone-bound
     * anim + which zone that anim belongs to: SELECTED = bright ramp
     * (animated, cycles the sprite's own 2 frames), UNSELECTED = dim ramp
     * (frozen frame 0). */
    int *anim_zone = aseq ? calloc((size_t)nanims, sizeof *anim_zone) : NULL;
    unsigned char *tint =
        (aseq && nz) ? calloc((size_t)nanims * 256, 1) : NULL;
    unsigned char *tint_idle =
        (aseq && nz) ? calloc((size_t)nanims * 256, 1) : NULL;
    if (!tint)
        tint_idle = NULL;
    if (anim_zone)
        for (int i = 0; i < nanims; i++) {
            anim_zone[i] = -1;
            if (!anims[i].tag)
                continue;
            for (int zi = 0; zi < nz; zi++)
                if (zones[zi].tag && strcmp(zones[zi].tag, anims[i].tag) == 0) {
                    anim_zone[i] = zi;
                    break;
                }
            if (tint && anim_zone[i] >= 0 && aseq[i].els) {
                int sh = menu_tint_build(&aseq[i], tint + (size_t)i * 256);
                int shi = tint_idle
                    ? menu_tint_idle_build(&aseq[i],
                                           tint_idle + (size_t)i * 256)
                    : 0;
                printf("menu: tint %s: selected %+d / idle %+d (ramp tops %d"
                       " selected, %d idle)\n",
                       anims[i].tag, sh, shi, GREEN_HI, GREEN_IDLE_TOP);
            }
        }

    /* the dialog's default answer is selected on open, so it already reads
     * bright green before the user moves the cursor (restart: CNIE) */
    for (int zi = 0; zi < nz; zi++)
        if (zones[zi].dflt) {
            zsel = zi;
            printf("menu: default answer %s (bright green)\n", zones[zi].tag);
            break;
        }

    /* CI hook: LEO_MENU_SECS=N shows the menu N seconds then auto-quits */
    const char *sece = getenv("LEO_MENU_SECS");
    uint64_t t_deadline = sece ? now_ms() + 1000 * (uint64_t)atof(sece) : 0;
    while (!quit) {
        if (tick == 0 && nz) {      /* CI: LEO_MENU_PICK fires a zone key */
            const char *pk = getenv("LEO_MENU_PICK");
            if (pk && *pk)
                for (int zi = 0; zi < nz; zi++)
                    if (zones[zi].key &&
                        (int)(unsigned char)pk[0] == zones[zi].key) {
                        printf("menu: LEO_MENU_PICK -> zone %s\n",
                               zones[zi].tag);
                        ret = zones[zi].ret;
                        quit = 1;
                        break;
                    }
            /* CI: LEO_MENU_NAV=k = k arrow-DOWN moves + Enter (proves
             * the zone/list keyboard cycle; from the initial (BACK) row
             * the restart dialog cycle is TAK,CNIE,X,(BACK)) */
            const char *nv = getenv("LEO_MENU_NAV");
            if (!quit && nv && *nv && nz + nitems > 0) {
                int vi = (nz + atoi(nv)) % (nz + nitems);
                if (vi < nz) {
                    printf("menu: LEO_MENU_NAV -> zone %s\n",
                           zones[vi].tag);
                    ret = zones[vi].ret;
                } else
                    ret = vi - nz;
                quit = 1;
            }
            /* CI: LEO_MENU_SEL=i pre-selects zone i (highlight only,
             * no activation) so LEO_MENU_PPM frames show the selection
             * underline the desktop user sees */
            const char *sl = getenv("LEO_MENU_SEL");
            if (!quit && sl && *sl)
                zsel = (int)((unsigned)atoi(sl) % (unsigned)nz);
        }
        if (quit)
            break;
        /* compose: background (image or black) then the item list */
        if (bg.idx) {
            /* 8bpp index canvas = PCX background + .SEQ sprite overlays,
             * then ONE palette LUT (the game composites the same way) */
            memcpy(cv, bg.idx, (size_t)W * H);
            for (int a = 0; a < nanims && aseq; a++) {
                if (!aseq[a].els)
                    continue;
                unsigned per = anims[a].period_ms ? anims[a].period_ms : 100;
                unsigned fr = (unsigned)((uint64_t)tick * 30 / per)
                              % aseq[a].count;
                const unsigned char *m = NULL;
                if (anim_zone && anim_zone[a] >= 0) {
                    if (anim_zone[a] == zsel) {
                        /* selected answer: bright ramp + the sprite's own
                         * 2-frame pulse = the animated button */
                        m = tint ? tint + (size_t)a * 256 : NULL;
                    } else {
                        /* unselected: dim ramp, frozen on frame 0 so only
                         * the selected word moves */
                        m = tint_idle ? tint_idle + (size_t)a * 256 : NULL;
                        fr = 0;
                    }
                }
                const ll_seq_elem *e = &aseq[a].els[fr];
                ll_seq_draw_elem_map(&aseq[a], e, cv, W, H,
                                     -SX(e->f0), -SX(e->f2), m);
            }
            uint32_t lut[256];
            for (int i = 0; i < 256; i++)
                lut[i] = 0xFF000000u | ((uint32_t)bg.pal[3 * i] << 16)
                         | ((uint32_t)bg.pal[3 * i + 1] << 8)
                         | (uint32_t)bg.pal[3 * i + 2];
            for (int i = 0; i < W * H; i++)
                fb[i] = lut[cv[i]];
        } else {
            for (int i = 0; i < W * H;
                 i++) /* dark vertical gradient - simple but intentional */
                fb[i] = 0xFF000000u | (uint32_t)(20 + (i / W) / 8);
        }

        /* CI hook: LEO_MENU_PPM=PATH dumps the composited frame (bg +
         * .SEQ sprites + menu text) once at that tick. */
        /* windowed list: long lists (level select has 14 entries) scroll
         * a 10-row window centred on the selection instead of overflowing
         * (named list_rows to avoid shadowing any window variable) */
        int list_rows = nitems > 10 ? 10 : nitems;
        int top = 0;
        if (nitems > list_rows) {
            top = sel - list_rows / 2;
            if (top < 0) top = 0;
            if (top > nitems - list_rows) top = nitems - list_rows;
        }
        int x0 = W / 2 - 64, y0 = H / 2 - 8 * list_rows / 2;
        if (bg.idx) { /* OPTIONS.PCX is a full text screen already */
            x0 = 24;
            y0 = H - 8 * (list_rows + 2);
        }
        for (int j = 0; j < list_rows; j++) {
            int i = top + j;
            draw_text(fb, W, H, x0, y0 + 12 * j, items[i], 0xFFFFFFFFu);
            if (i == sel && zsel < 0)
                draw_text(fb, W, H, x0 - 12, y0 + 12 * j, ">",
                          0xFFFFFF00u);
        }
        if (top > 0)
            draw_text(fb, W, H, x0, y0 - 10, "^", 0xFFAAAAAAu);
        if (top + list_rows < nitems)
            draw_text(fb, W, H, x0, y0 + 12 * list_rows - 2, "v",
                      0xFFAAAAAAu);
        if (hov_r >= 0)            /* row hover: marker even without click */
            draw_text(fb, W, H, x0 - 12, y0 + 12 * hov_r, ">",
                      0xFFFFFF00u);
        /* answer-word indicators UNDER the cursive words (round 4's
         * outline boxes covered them): keyboard selection = solid bright
         * 2-px underline, mouse hover = dim 1-px underline */
        for (int zi = 0; zi < nz; zi++) {
            if (zrect[zi][2] <= zrect[zi][0])
                continue;
            if (zi == zsel)
                for (int t = 0; t < 2; t++)
                    draw_hline(fb, W, H, zrect[zi][0], zrect[zi][2],
                               zrect[zi][3] + 1 + t, 0xFFFFFF00u);
            else if (zi == hov_z)
                draw_hline(fb, W, H, zrect[zi][0], zrect[zi][2],
                           zrect[zi][3] + 1, 0xFFB08000u);
        }

        {
            const char *ppm_e = getenv("LEO_MENU_PPM");
            const char *tick_e = getenv("LEO_MENU_PPM_TICK");
            unsigned want = tick_e ? (unsigned)atoi(tick_e) : 0;
            if (ppm_e && tick == want) {
                if (ppm_fb(ppm_e, fb, W, H) == 0)
                    printf("menu: LEO_MENU_PPM wrote %s (tick %u)\n",
                           ppm_e, tick);
            }
        }

        plat_present_argb(fb, W, H);

        ll_event e;
        uint64_t t_next = now_ms() + 30;
        while (!quit) {
            /* P-04: no per-tick focus poll - the platform re-raises on
             * pointer-enter only. */
            if (t_deadline && now_ms() >= t_deadline) {
                printf("menu: LEO_MENU_SECS expired (auto-quit)\n");
                quit = 1;
                ret = -3;
                break;
            }
            while (plat_poll(&e)) {
                if (e.type == LL_EV_QUIT) {
                    quit = 1;
                    ret = -2;
                } else if (e.type == LL_EV_BUTTON_DOWN) {
                    /* logical viewport: the platform already maps window
                     * pixels to canvas coords (P-10; the old /sc is gone);
                     * only the left button is surfaced (LLBTN_LEFT) */
                    int mx = e.x, my = e.y;
                    for (int j = 0; j < list_rows; j++) {
                        int ry = y0 + 12 * j;
                        if (my >= ry - 2 && my < ry + 10 && mx >= x0 - 14) {
                            ret = top + j;
                            quit = 1;
                            break;
                        }
                    }
                    if (!quit)
                        for (int zi = 0; zi < nz; zi++)
                            if (zrect[zi][2] > zrect[zi][0] &&
                                mx >= zrect[zi][0] && mx < zrect[zi][2] &&
                                my >= zrect[zi][1] && my < zrect[zi][3]) {
                                printf("menu: click zone %s\n",
                                       zones[zi].tag);
                                ret = zones[zi].ret;
                                quit = 1;
                                break;
                            }
                } else if (e.type == LL_EV_MOTION) {
                    int mx = e.x, my = e.y; /* logical coords */
                    hov_r = -1;
                    for (int j = 0; j < list_rows; j++) {
                        int ry = y0 + 12 * j;
                        if (my >= ry - 2 && my < ry + 10 && mx >= x0 - 14) {
                            hov_r = j;
                            break;
                        }
                    }
                    hov_z = -1;
                    for (int zi = 0; zi < nz; zi++)
                        if (zrect[zi][2] > zrect[zi][0] &&
                            mx >= zrect[zi][0] && mx < zrect[zi][2] &&
                            my >= zrect[zi][1] && my < zrect[zi][3]) {
                            hov_z = zi;
                            break;
                        }
                } else if (e.type == LL_EV_KEYDOWN) {
                    int k = e.key;      /* ASCII verbatim or LLKEY_* */
                    int zhit = 0;
                    for (int zi = 0; zi < nz; zi++)
                        if (zones[zi].key && k == (int)zones[zi].key) {
                            ret = zones[zi].ret;
                            quit = 1;
                            zhit = 1;
                            break;
                        }
                    if (zhit)
                        break;
                    /* arrows cycle EVERYTHING the user can pick: answer
                     * zones first (TAK, CNIE, X), then the list rows
                     * (the dialog's "(BACK)"); the highlight (solid
                     * underline / '>') is the visible state. The words
                     * sit side by side, so LEFT/RIGHT is the primary
                     * cycle (user request); UP/DOWN ride the same
                     * cycle so existing lists keep working. */
                    int ncyc = nz + nitems;
                    if ((k == LLKEY_RIGHT || k == LLKEY_LEFT
                         || k == LLKEY_UP || k == LLKEY_DOWN) && ncyc > 0) {
                        int step = (k == LLKEY_RIGHT || k == LLKEY_DOWN)
                                       ? 1 : ncyc - 1;
                        int vi = zsel >= 0 ? zsel : nz + sel;
                        vi = (vi + step) % ncyc;
                        if (vi < nz) {
                            zsel = vi;
                            printf("menu: sel zone %s (Enter=confirm)\n",
                                   zones[vi].tag);
                        } else {
                            zsel = -1;
                            sel = vi - nz;
                        }
                    } else if (k == LLKEY_RETURN || k == ' ') {
                        /* Return, keypad Enter (folded by the backend)
                         * and space all confirm, like the original */
                        quit = 1;
                        if (zsel >= 0) {
                            printf("menu: confirm zone %s\n",
                                   zones[zsel].tag);
                            ret = zones[zsel].ret;
                        } else
                            ret = sel;
                    } else if (k == LLKEY_ESC) {
                        /* E-06: ESC = cancel (-1), never "pick the
                         * highlighted row" - in the LEVELS list that used
                         * to LAUNCH a level. Callers treat -1 like (BACK). */
                        quit = 1;
                        ret = -1;
                        printf("menu: ESC -> cancel\n");
                    } else if (k == 'q') {
                        quit = 1;
                        ret = -2;
                    }
                }
            }
            int64_t left = (int64_t)(t_next - now_ms());
            ll_music_pump();   /* submenu music rides the audio queue */
            if (left > 0)
                plat_sleep_until(plat_now_ns() + (uint64_t)left * 1000000ull);
            else
                break;
        }
        tick++;
    }
    free(fb);
    free(cv);
    free(anim_zone);
    free(tint);
    free(tint_idle);
    menu_anims_free(aseq, nanims);
    ll_pcx_free(&bg);
    return ret;
}

/* ------------------------------------------------------------------ */
/* Chapter map menu (the original title-menu state, 640x400).          */
/* Layout constants from the 2026-11-14 QEMU captures (g781 frame +   */
/* hunt7/hunt9 state traces); see PLAN.md "Menu layout cracked".      */

#define MAP_W 640
#define MAP_H 400

/* The original composites in a 320x200 logical canvas (the .SEQ sprite
 * coords, e.g. AUTORZY.SEQ @ (4,180), are authored in that space) and
 * doubles 2x2 onto the 640x400 CRTC; hunt13's VRAM dump proved the fb is
 * linear 8bpp 320x200.  We mirror that: everything composites at logical
 * scale, presentation + PPM merge to 640x400. */
#define MAP_LW 320
#define MAP_LH 200

/* 1x 8x8 glyph at logical scale (originals are .SEQ sprites/art; this is
 * our documented placeholder font, same space, half the 640-space size) */
static void draw_glyph1(uint32_t *fb, int x, int y, char c, uint32_t col)
{
    if (c < 32 || c > 126)
        c = '?';
    const unsigned char *g = FONT[c - 32];
    for (int r = 0; r < 8; r++)
        for (int b = 0; b < 8; b++)
            if (g[r] & (0x80 >> b)) {
                int px = x + b, py = y + r;
                if (px >= 0 && px < MAP_LW && py >= 0 && py < MAP_LH)
                    fb[py * MAP_LW + px] = col;
            }
}

static void draw_text1(uint32_t *fb, int x, int y, const char *s, uint32_t col)
{
    for (; *s; s++, x += 8)
        draw_glyph1(fb, x, y, *s, col);
}

static void draw_text1_r(uint32_t *fb, int xr, int y, const char *s, uint32_t col)
{
    int n = 0;
    for (const char *p = s; *p; p++)
        n++;
    draw_text1(fb, xr - 8 * n, y, s, col);
}

static uint32_t menu_lut(int i)
{
    /* Real menu DAC recovered from the original's VGA state (capture +
     * QEMU xp dump; see ll_menu_dac.h provenance). Same index space the
     * ANIMACJE/MENU .SEQ sprites are drawn into: idx 15 = green corner
     * words, 22 = bright green title, 90..95 dark / 218..230 bright =
     * BERLO golds, 44/46/51/102-107 sea blues, 199/200/202 island sand. */
    return ll_menu_dac[i & 0xFF];
}

/* bg on the logical canvas; chs anchors are 640-space, halved here */
static void map_bg(uint32_t *fb, const ll_menu_ch *chs, int nchs)
{
    /* Placeholder for the globe+islands art (a CD data hole, PLAN 8b),
     * but using the REAL menu DAC tones recovered from the original
     * (ll_menu_dac.h): deep sea idx102 -> sea idx46 gradient, sea-blue
     * sparkles, green/sand island blobs at the measured anchors. */
    for (int y = 0; y < MAP_LH; y++) {
        int t = y * 255 / (MAP_LH - 1);
        uint32_t a = ll_menu_dac[102], b = ll_menu_dac[46];
        uint32_t c = 0xFF000000u
                   | ((((a >> 16 & 255) * (255 - t) + (b >> 16 & 255) * t)
                       / 255) << 16)
                   | ((((a >> 8 & 255) * (255 - t) + (b >> 8 & 255) * t)
                       / 255) << 8)
                   | (((a & 255) * (255 - t) + (b & 255) * t) / 255);
        for (int x = 0; x < MAP_LW; x++)
            fb[y * MAP_LW + x] = c;
    }
    for (int s = 0; s < 70; s++) {
        unsigned h = (unsigned)(s * 2654435761u);
        int x = (int)(h % MAP_LW), y = (int)((h >> 12) % MAP_LH);
        fb[y * MAP_LW + x] = (h >> 24) & 1 ? ll_menu_dac[121]
                                           : ll_menu_dac[107];
    }
    /* island blobs at each chapter anchor (green top, sand base);
     * rw/rh are the measured land-blob half-extents (fallback 60/26
     * in 640-space = 30/13 logical) */
    for (int i = 0; i < nchs; i++) {
        int cx = chs[i].ix / 2, cy = chs[i].iy / 2;
        int rw = chs[i].rw > 0 ? chs[i].rw / 2 : 30;
        int rh = chs[i].rh > 0 ? chs[i].rh / 2 : 13;
        if (rw < 2)
            rw = 2;
        if (rh < 2)
            rh = 2;
        for (int y = -rh; y <= rh; y++)
            for (int x = -rw; x <= rw; x++) {
                double d = (double)(x * x) / (rw * rw)
                           + (double)(y * y) / (rh * rh);
                if (d > 1.0)
                    continue;
                int px = cx + x, py = cy + y;
                if (px < 0 || px >= MAP_LW || py < 0 || py >= MAP_LH)
                    continue;
                uint32_t c = y < -rh / 3 ? ll_menu_dac[22] : ll_menu_dac[199];
                if (y > rh - (rh / 4 + 1))
                    c = ll_menu_dac[107];  /* waterline */
                fb[py * MAP_LW + px] = c;
            }
    }
}

int ll_menu_map_open(const ll_menu_ch *chs, int nchs,
                     const ll_menu_anim *anims, int nanims)
{
    if (nchs < 1)
        return -1;

    ll_seq *aseq = nanims > 0 ? calloc((size_t)nanims, sizeof *aseq) : NULL;
    int nanim_ok = 0;
    if (aseq) {
        for (int i = 0; i < nanims; i++) {
            if (ll_seq_parse(anims[i].seq, anims[i].len, &aseq[i]) != 0) {
                fprintf(stderr, "map: anim %d .SEQ parse failed\n", i);
                continue;
            }
            printf("map: anim %d: %u frames, period %ums\n",
                   i, aseq[i].count,
                   anims[i].period_ms ? anims[i].period_ms : 100);
            nanim_ok++;
        }
        if (!nanim_ok) {
            menu_anims_free(aseq, nanims);
            aseq = NULL;
        }
    }

    /* T-01/T-12: the platform owns the window; the map's logical
     * viewport IS the 640x400 display frame (island/corner coords
     * already live in it, so the click/hover math stays 1:1). */
    if (plat_init("Lew Leon - chapter map", MAP_W, MAP_H) < 0) {
        menu_anims_free(aseq, nanims);
        return -2;
    }
    plat_raise(); /* dialog re-entry: ask the WM for focus */
    uint32_t *fb = (uint32_t *)malloc((size_t)MAP_LW * MAP_LH * sizeof *fb);
    unsigned char *cv = malloc((size_t)MAP_LW * MAP_LH);
    uint32_t *disp = (uint32_t *)malloc((size_t)MAP_W * MAP_H * sizeof *disp);
    if (!fb || !cv || !disp) {
        free(fb);
        free(cv);
        free(disp);
        menu_anims_free(aseq, nanims);
        return -2;
    }

    uint32_t pal[256];
    for (int i = 0; i < 256; i++)
        pal[i] = menu_lut(i);

    map_bg(fb, chs, nchs);              /* background is static */
    uint32_t *base = malloc((size_t)MAP_LW * MAP_LH * sizeof *base);
    if (base)
        memcpy(base, fb, (size_t)MAP_LW * MAP_LH * sizeof *base);

    int sel = 0, quit = 0, ret = -1;
    int hov = -1;            /* island idx or corner code (-2/-4/-5/-6) */
    unsigned tick = 0;
    const char *sece = getenv("LEO_MENU_SECS");
    uint64_t t_deadline = sece ? now_ms() + 1000 * (uint64_t)atof(sece) : 0;
    while (!quit) {
        if (base)
            memcpy(fb, base, (size_t)MAP_LW * MAP_LH * sizeof *base);

        /* .SEQ overlays composite at logical scale (the original authored
         * its sprite coords in the 320x200 space): anims with x>=0 draw at
         * their fixed origin (AUTORZY.SEQ @ (4,180) = the bottom-left
         * corner word); x<0 parks the BERLO arrow just left of the
         * selected island's measured edge, vertically centred */
        if (aseq) {
            memset(cv, 0xFF, (size_t)MAP_LW * MAP_LH);  /* 0xFF = keep bg */
            int irw = chs[sel].rw > 0 ? chs[sel].rw / 2 : 30;
            for (int a = 0; a < nanims; a++) {
                if (!aseq[a].els)
                    continue;
                unsigned per = anims[a].period_ms ? anims[a].period_ms : 100;
                unsigned fr = (unsigned)((uint64_t)tick * 30 / per)
                              % aseq[a].count;
                const ll_seq_elem *e = &aseq[a].els[fr];
                int x = anims[a].x, y = anims[a].y;
                if (x < 0) {
                    x = chs[sel].ix / 2 - irw - 14;
                    y = chs[sel].iy / 2 - 9;
                }
                ll_seq_draw_elem(&aseq[a], e, cv, MAP_LW, MAP_LH, x, y);
            }
            for (int i = 0; i < MAP_LW * MAP_LH; i++)
                if (cv[i] != 0xFF)
                    fb[i] = pal[cv[i]];
        }

        /* corner words + chapter title at logical scale, colors from the
         * real menu DAC: idx15 green words, idx22 bright green title
         * (art bbox x50..342 y108..158 = logical 25..171 54..79).
         * AUTORZY is NOT font-drawn: it is the real AUTORZY.SEQ sprite
         * overlay (anim @ logical (4,180)). */
        draw_text1(fb, 5, 5, "WYJSCIE", ll_menu_dac[15]);
        draw_text1_r(fb, 315, 3, "RESTART", ll_menu_dac[15]);
        draw_text1_r(fb, 315, 187, "OPCJE", ll_menu_dac[15]);
        draw_text1(fb, 25, 62, chs[sel].title, ll_menu_dac[22]);

        /* presentation = the original's 2x2 CRTC doubling of the logical
         * canvas (VRAM capture: even-aligned block equality 1.000) */
        for (int y = 0; y < MAP_LH; y++) {
            uint32_t *d0 = disp + (size_t)(2 * y) * MAP_W;
            uint32_t *d1 = d0 + MAP_W;
            const uint32_t *s = fb + (size_t)y * MAP_LW;
            for (int x = 0; x < MAP_LW; x++) {
                d0[2 * x] = s[x];
                d0[2 * x + 1] = s[x];
                d1[2 * x] = s[x];
                d1[2 * x + 1] = s[x];
            }
        }

        {   /* CI hook: dump composed frame at LEO_MENU_PPM_TICK */
            const char *ppm_e = getenv("LEO_MENU_PPM");
            const char *tick_e = getenv("LEO_MENU_PPM_TICK");
            unsigned want = tick_e ? (unsigned)atoi(tick_e) : 0;
            if (ppm_e && tick == want)
                if (ppm_fb(ppm_e, disp, MAP_W, MAP_H) == 0)
                    printf("map: LEO_MENU_PPM wrote %s (tick %u)\n",
                           ppm_e, tick);
        }

        if (hov >= 0 && hov < nchs)          /* hover outlines (drawn after
                                              * the PPM hook: CI frames stay
                                              * hover-free) */
            draw_rect(disp, MAP_W, MAP_H,
                      chs[hov].ix - chs[hov].rw - 3,
                      chs[hov].iy - chs[hov].rh - 3,
                      chs[hov].ix + chs[hov].rw + 4,
                      chs[hov].iy + chs[hov].rh + 4, 0xFFFFFF00u);
        else if (hov == -2)
            draw_rect(disp, MAP_W, MAP_H, 2, 2, 130, 26, 0xFFFFFF00u);
        else if (hov == -6)
            draw_rect(disp, MAP_W, MAP_H, 506, 2, MAP_W - 2, 26,
                      0xFFFFFF00u);
        else if (hov == -5)
            draw_rect(disp, MAP_W, MAP_H, 506, MAP_H - 40, MAP_W - 2,
                      MAP_H - 2, 0xFFFFFF00u);
        else if (hov == -4)
            draw_rect(disp, MAP_W, MAP_H, 2, MAP_H - 52, 140, MAP_H - 2,
                      0xFFFFFF00u);

        plat_present_argb(disp, MAP_W, MAP_H);

        ll_event e;
        uint64_t t_next = now_ms() + 30;
        while (!quit) {
            /* P-04: no per-tick focus poll - the platform re-raises on
             * pointer-enter only. */
            if (t_deadline && now_ms() >= t_deadline) {
                printf("map: LEO_MENU_SECS expired (auto-quit)\n");
                quit = 1;
                ret = -3;
                break;
            }
            /* CI hook: LEO_MENU_KEY=a|o|r simulates a corner key at
             * tick 1, once per process (headless coverage of the
             * autorzy/opcje/restart paths without looping forever) */
            {
                static int key_fired;
                const char *ke = getenv("LEO_MENU_KEY");
                if (ke && tick == 1 && !key_fired++) {
                    char kc = ke[0];
                    if (kc == 'o') {
                        printf("map: LEO_MENU_KEY -> opcje\n");
                        quit = 1;
                        ret = -5;
                    } else if (kc == 'a') {
                        printf("map: LEO_MENU_KEY -> autorzy\n");
                        quit = 1;
                        ret = -4;
                    } else if (kc == 'r') {
                        printf("map: LEO_MENU_KEY -> restart\n");
                        quit = 1;
                        ret = -6;
                    }
                }
            }
            while (plat_poll(&e)) {
                if (e.type == LL_EV_QUIT) {
                    quit = 1;
                    ret = -2;
                } else if (e.type == LL_EV_BUTTON_DOWN) {
                    /* logical viewport = the 640x400 display frame, and
                     * island coords (chs[].ix/iy/rw/rh) live in that same
                     * space, so events map 1:1 */
                    int mx = e.x, my = e.y;
                    if (mx < 130 && my < 26) {          /* WYJSCIE */
                        printf("map: click WYJSCIE\n");
                        quit = 1;
                        ret = -2;
                    } else if (mx > 505 && my < 26) {   /* RESTART */
                        printf("map: click RESTART\n");
                        quit = 1;
                        ret = -6;
                    } else if (mx > 505 && my > 360) {  /* OPCJE */
                        printf("map: click OPCJE\n");
                        quit = 1;
                        ret = -5;
                    } else if (mx < 140 && my > 348) {  /* AUTORZY */
                        printf("map: click AUTORZY\n");
                        quit = 1;
                        ret = -4;
                    } else {                            /* island select/launch */
                        for (int i = 0; i < nchs; i++) {
                            if (mx >= chs[i].ix - chs[i].rw
                                && mx <= chs[i].ix + chs[i].rw
                                && my >= chs[i].iy - chs[i].rh
                                && my <= chs[i].iy + chs[i].rh) {
                                sel = i;
                                printf("map: island %d launch (click)\n", i);
                                quit = 1;
                                ret = i;
                                break;
                            }
                        }
                    }
                } else if (e.type == LL_EV_MOTION) {
                    /* hover feedback (user: mouse did nothing until the
                     * click): outline the corner word / island under the
                     * pointer; window is 1:1 (MAP_W x MAP_H) */
                    int mx = e.x, my = e.y;
                    hov = -1;
                    if (mx < 130 && my < 26)
                        hov = -2;                     /* WYJSCIE    */
                    else if (mx > 505 && my < 26)
                        hov = -6;                     /* RESTART    */
                    else if (mx > 505 && my > 360)
                        hov = -5;                     /* OPCJE      */
                    else if (mx < 140 && my > 348)
                        hov = -4;                     /* AUTORZY    */
                    else
                        for (int i = 0; i < nchs; i++)
                            if (mx >= chs[i].ix - chs[i].rw
                                && mx <= chs[i].ix + chs[i].rw
                                && my >= chs[i].iy - chs[i].rh
                                && my <= chs[i].iy + chs[i].rh) {
                                hov = i;
                                break;
                            }
                } else if (e.type == LL_EV_KEYDOWN) {
                    int k = e.key;      /* ASCII verbatim or LLKEY_* */
                    if (k == LLKEY_UP || k == LLKEY_LEFT)
                        sel = (sel + nchs - 1) % nchs;
                    else if (k == LLKEY_DOWN || k == LLKEY_RIGHT)
                        sel = (sel + 1) % nchs;
                    else if (k == LLKEY_RETURN || k == ' ') {
                        quit = 1;
                        ret = sel;
                    } else if (k == LLKEY_ESC || k == 'q') {
                        printf("map: wyjście\n");
                        quit = 1;
                        ret = -2;
                    } else if (k == 'a') {
                        printf("map: autorzy\n");
                        quit = 1;
                        ret = -4;
                    } else if (k == 'o') {
                        printf("map: opcje\n");
                        quit = 1;
                        ret = -5;
                    } else if (k == 'r') {
                        printf("map: restart\n");
                        quit = 1;
                        ret = -6;
                    }
                }
            }
            int64_t left = (int64_t)(t_next - now_ms());
            ll_music_pump();
            if (left > 0)
                plat_sleep_until(plat_now_ns() + (uint64_t)left * 1000000ull);
            else
                break;
        }
        tick++;
    }
    free(base);
    free(fb);
    free(cv);
    free(disp);
    menu_anims_free(aseq, nanims);
    return ret;
}
