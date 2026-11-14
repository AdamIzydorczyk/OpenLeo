/* ll_seq.h - .SEQ animation-script reader for Lew Leon (Longsoft 1997).
 * Format recovered 2026-11-14 from LEO.EXE raw disassembly: the game loads
 * a whole .SEQ record payload from the llrs container into a buffered
 * stream (FUN_00043e70 + FUN_0005c6b0), reads u16 element count, then
 * parses `count` elements (FUN_00041ef0 -> FUN_00041f60, leaf readers
 * FUN_00041d40 / FUN_00041da0 / FUN_00041e00; every stream read goes
 * through vtable slot +0x1c = read(dest, count-BYTES)):
 *
 *   blob    := u16 count; count x element
 *   element := u16 w0                        flag word (low nibble used)
 *              u16 b19, b15, b17             sub-block obj+0x19/0x15/0x17
 *              u16 f0, f2, f4, f6            obj+0x00..0x06 (f0 = id,
 *                                            0x7fff = ctor sentinel)
 *              [u16 f8, u16 fA]              obj+0x08/0x0a, when w0&4
 *              [n bytes palette][n bytes sprite]   when w0&3,
 *                                            n = int16(b17), skipped n<=0
 *              [for i=0..int16(b19): u16 L; L bytes]  unless w0&2 && !(w0&1)
 *
 * This grammar consumes ALL 49 .SEQ payloads in ANIMACJE/ETAPY/LEVELDES
 * exactly (consumed == payload size). Reference: tools/seq_parse.py;
 * make verify-seq checks byte-exact agreement. The derived fields the
 * game computes at load (obj+0x0c..0x12 bbox, obj+0x14 = w0&8) are NOT
 * part of the blob; FUN_000420b0 recomputes them from parsed data.
 */
#ifndef LL_SEQ_H
#define LL_SEQ_H

#include <stddef.h>
#include <stdint.h>

typedef struct {                 /* one variable-length byte array entry  */
    uint32_t off, len;           /* byte offset into the blob             */
} ll_seq_ent;

typedef struct {
    uint32_t off;                /* element start (the w0 word) in blob   */
    uint16_t w0;
    uint16_t b19, b15, b17;      /* sub-block words                       */
    uint16_t f0, f2, f4, f6;     /* object header words                   */
    uint16_t f8, fA;             /* valid only when (w0 & 4)              */
    int32_t  n;                  /* palette/sprite length, 0 = absent     */
    uint32_t pal, spr;           /* byte offsets, valid when n > 0        */
    uint32_t arr_first, arr_n;   /* window into ll_seq.ents; arr_n = 0 +
                                  * arr_present for empty arrays          */
    uint8_t  arr_present;
} ll_seq_elem;

typedef struct {
    const uint8_t *blob;         /* not owned                             */
    uint32_t len, count;
    uint32_t consumed;           /* == len for all known files            */
    ll_seq_elem *els;            /* malloc'd [count]                      */
    ll_seq_ent  *ents;           /* malloc'd array entries (flat)         */
    uint32_t n_ents, cap_ents;
} ll_seq;

/* Parse a .SEQ blob (payload bytes straight from an llrs id=1 record).
 * 0 = ok, all bytes consistent; -1 = malformed/truncated. Free with
 * ll_seq_free. No side tables: element data stays in the blob. */
int  ll_seq_parse(const uint8_t *blob, uint32_t len, ll_seq *out);
void ll_seq_free(ll_seq *s);

/* --- frame-patch rendering (M4-A, from blitter FUN_0001dc80 raw disasm) ---
 * Entry blobs (ll_seq_ent .off/.len) are length-prefixed sprite rows:
 *   row := (0x80|len) content[len];  content := gap run*
 *   op pos: C < 0x80 copy C bytes | F >= 0x80 fill F&0x7f bytes, 1 value B
 *   marker pos (after any run): B < 0x40 dst gap advance (kind 2, paints
 *   nothing, no dump token) | 0x40..0x7f after COPY run = fill (B&0x3f)
 *   bytes + value byte, after FILL run = copy B&0x3f bytes
 * The game blits rows onto a 400-byte pitch; chunks: entries 0..arr_n-2 are
 * 80 px wide, the last entry is b15 px wide, x cursor += 80. Element screen
 * position = (-f0, -f2) (bbox derived from negated f0/f2, FUN_000420b0).
 *
 * ll_seq_patch_walk decodes one entry blob, invoking cb per row (gap -1 =
 * empty row; ops array valid only during the call). Returns rows decoded,
 * -1 malformed (cb may have seen earlier rows already). Marker bytes seen
 * between ops (FUN_0001dc80 @0x1dd59/0x1dd83): <0x40 = dst gap advance
 * (kind 2; the painter MUST advance px — missing that garbled AUTORZY.SEQ
 * word rendering), 0x40..0x7f alternates fill/copy on the previous run.
 * ll_seq_patch_draw paints the decoded palette indices into an 8bpp canvas
 * (clip to cw/ch). ll_seq_draw_elem paints all chunks of an element with
 * origin (x,y). */
typedef struct {
    int kind;                /* 0 = copy (b), 1 = fill (val), 2 = gap skip */
    const uint8_t *b;        /* copy payload (points into blob) */
    int val;                 /* fill value */
    int cnt;                 /* run length in pixels */
} ll_seq_patch_op;

typedef void (*ll_seq_patch_row_cb)(void *ctx, int row, int gap,
                                    const ll_seq_patch_op *ops, int nops);

int ll_seq_patch_walk(const uint8_t *blob, uint32_t len,
                      ll_seq_patch_row_cb cb, void *ctx);
int ll_seq_patch_draw(const uint8_t *blob, uint32_t len, uint8_t *canvas,
                      int cw, int ch, int x, int y);
int ll_seq_draw_elem(const ll_seq *s, const ll_seq_elem *e,
                     uint8_t *canvas, int cw, int ch, int x, int y);

#endif /* LL_SEQ_H */
