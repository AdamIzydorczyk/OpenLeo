/* ll_save.h - STATUS.LEO savegame + LONGSOFT.CFG settings readers/writers
 * for Lew Leon (Longsoft 1997), byte-compatible with the shipped files.
 *
 * STATUS.LEO (exactly 300 B = 75 LE dwords), layout pinned from LEO.EXE
 * FUN_0005dae0 (reader) + FUN_0005dcc0 (writer), opened mode "rb"/"w+b"
 * (strings @0x8369d/0x836a0), filename string "status.leo" @0x9a8a4:
 *
 *   words [0..20]  block A  -> game var array (state block A)
 *   words [21..41] block B  -> state block B (var base +0x54)
 *   words [42..62] block C  -> DAT_000cf46c[0..20] (all zero in the
 *                              shipped save)
 *   word  [63]     -> DAT_000a94f0 (low byte survives)
 *   word  [64]     -> DAT_000cf464 (low byte survives)
 *   words [65..74] plain dwords (shipped save: 128, 200, 1, 0 x7)
 *
 * Obfuscation (writer, j = index WITHIN its 21-word block):
 *   stored = (j | 0x77072400) ^ (var | (j & 0xffffff00))
 * and the reader only keeps the low byte:
 *   value  = (stored ^ j) & 0xff        (per block; j in 0..20)
 * so each slot stores ONE byte of a game variable; the high 24 bits of
 * the stored word are variable-derived noise the engine never reads.
 * Word 63/64: value = ((stored ^ 0x77072400) >> 8) & 0xff.
 * Field semantics (which byte is lives/level/score) still need a paired
 * QEMU capture (menu save vs mid-level save) - documented open task;
 * the point here is byte-exact load/round-trip of the provided saves.
 *
 * LONGSOFT.CFG (exactly 273 B): 15-byte header + zero padding. Only
 * byte 0 is pinned in LEO.EXE: FUN_0005bf10 takes the first read byte
 * (DAT_000cf2be) and sets the data drive DAT_000cf254 = byte0 - 0x40,
 * i.e. byte 0 is the ASCII drive letter the game prefixes to "\leo\"
 * paths (shipped save: 'H' = the user's CD drive; the SETUP/CONFIG.EXE
 * writer is 16-bit, rest of the header unrecovered - tools/cfg_ll.py
 * patches the drive letter in a COPY, originals untouched).
 */
#ifndef LL_SAVE_H
#define LL_SAVE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LL_STATUS_WORDS 75                /* 300 bytes                 */
#define LL_CFG_SIZE     273               /* LONGSOFT.CFG is exact     */

typedef struct {
    unsigned int w[LL_STATUS_WORDS];      /* raw little-endian words   */
    int loaded;                           /* 1 = filled from a file    */
} ll_status;

/* load: file must be exactly 300 bytes; 0 = ok, -1 = open/size fail */
int ll_status_load(const char *path, ll_status *st);
/* save: writes the raw words verbatim (byte-exact round-trip)        */
int ll_status_save(const char *path, const ll_status *st);

/* decoded accessors mirror FUN_0005dae0 (blocks = 21-word groups):   */
int ll_status_byte(const ll_status *st, int block, int j);  /* A/B/C   */
int ll_status_word63(const ll_status *st);                  /* byte    */
int ll_status_word64(const ll_status *st);                  /* byte    */
unsigned int ll_status_plain(const ll_status *st, int k);   /* k 0..9  */

/* human-readable decode to stdout (CI grep target)                   */
void ll_status_dump(const ll_status *st, const char *path);

/* LONGSOFT.CFG: blob round-trip + the one pinned field (drive letter) */
typedef struct {
    unsigned char b[LL_CFG_SIZE];
    int loaded;
} ll_cfg;
int ll_cfg_load(const char *path, ll_cfg *c);
int ll_cfg_save(const char *path, const ll_cfg *c);
int ll_cfg_drive(const ll_cfg *c);              /* 'A'..'Z', 0 = weird  */
void ll_cfg_set_drive(ll_cfg *c, char drive);   /* uppercase letter     */

#ifdef __cplusplus
}
#endif
#endif
