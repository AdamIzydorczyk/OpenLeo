/* ll_mod.h - Module (MOD) music player for the Lew Leon engine variant.
 *
 * The SOUND llrs container (LEO/SOUND) carries 22 level/menu tracks as
 * '<name>.MOD' payloads (level .des files select them via `muzyka=` -
 * verified: RAN.DES->ran.mod, TUNGA1.DES->tunga.mod, KADMAR1.DES->
 * piramid.mod, ...).  The files are standard Amiga .MOD headers
 * (title@0, 31x30B instrument dir@20, songlen byte@950, 128-entry
 * pattern table@1268, id string@1080) but a LONGSOFT-REPACKED body
 * (Ghidra ModPlayer RE 2026-11-14, loader FUN_00012d14/FUN_000131b8):
 *   - pattern table entries are original pattern IDs; the loader
 *     REMAPS them to first-appearance order and stores only the UNIQUE
 *     patterns back-to-back at offset 1084 (nch*256 B each, note words
 *     in the 6/8-channel layout: p hi/lo + 5-bit inst + effect byte),
 *   - after the patterns an optional extension instrument directory:
 *     1 count byte + count 29-byte records (instruments 32+; the game
 *     reads extra entries until the count byte, FUN_000132b0),
 *   - then sample data in directory order; the game clamps each read
 *     to the remaining file bytes (FUN_00013300), so these shipped
 *     tracks have truncated sample data and the buffer is zero-filled
 *     (CALL 0x749a0 after alloc).
 * Verified per-file: songlen byte@950 (TUNGA 38, RAN 28, ZARIA 43,
 * KARA 58, LEW 33, RAN1 2, ENA2 40, MAPKA 10); engine period->note
 * table 61 entries 1712..56 at VA 0xa7272 (Amiga); signature table
 * M.K./M!K!/FLT4/4CHN/6CHN/8CHN/CD81/OKTA/16CN/32CN at 0xa7204.
 *
 * Playback semantics: standard ProTracker tick model (50 Hz ticks,
 * speed = ticks/row, Fxx tempo), effect subset 0-9,A-F + E1,E2,E5,E6,
 * E9,EA,EB; sample playback 8-bit unsigned with loop; note cut-off and
 * volume on the engine voice handlers (FUN_00015484 etc).
 *
 * Deterministic: pure integer mix at fixed rate; ll_mod_render() is the
 * CI oracle (FNV of the first N ms of the song), used by tools/
 * mod_selftest.c and the verify-mod target.
 */
#ifndef LL_MOD_H
#define LL_MOD_H

#include <stdint.h>
#include <stddef.h>

#define LL_MOD_MAXCH   8
#define LL_MOD_MAXSMP  64
#define LL_MOD_RATE    44100

typedef struct {
    uint8_t  vol;          /* 0..64 */
    uint8_t  fine;         /* finetune nibble 0..15 */
    uint32_t len;          /* bytes (engine: BE-u16 header words * 2)   */
    uint32_t loop_start, loop_len;
    const unsigned char *data; /* into the caller-owned image           */
    uint32_t dlen;         /* bytes actually present (clamped)          */
} ll_mod_smp;

typedef struct {
    unsigned char nch;
    unsigned nseq;                 /* song positions                    */
    const int   *pos2pat;          /* nseq -> stored pattern index      */
    int          npat_stored;      /* unique patterns = pattern blocks  */
    int          nsmp;
    ll_mod_smp   smp[LL_MOD_MAXSMP];
    const unsigned char *pat;      /* npat_stored * nch*256 bytes       */
    const unsigned char *img;      /* whole module image (borrowed)     */
    size_t       img_len;
    char         title[21];
    char         id[5];
} ll_mod;

/* parse a .MOD image; 0 = ok.  err gets a short reason on failure. */
int ll_mod_open(ll_mod *m, const unsigned char *img, size_t len,
                char *err, size_t errsz);
void ll_mod_close(ll_mod *m);

/* ---- mixer state (separate from the parsed image so the same module
 * can restart) ---- */
typedef struct {
    const ll_mod *m;
    struct {
        int16_t period;        /* current effective period (0 = silent) */
        int16_t tgt_period;    /* porta target                          */
        uint8_t smp;           /* sample index (0 = none)               */
        uint8_t vol;           /* 0..64                                 */
        uint32_t pos;          /* sample frame position (16.16 fixed)   */
        uint32_t inc;          /* frame step (16.16)                    */
        int8_t  act;           /* currently audible                     */
        uint8_t e0, e1;        /* effect params for this row            */
        uint8_t porta_spd;     /* remembered speed for 5/6 continuations */
        int8_t  porta;         /* running porta direction 0/1/2         */
        int8_t  vibpos;
        int8_t  cut;           /* note-cut flag                         */
        uint8_t retrig_cnt;
    } v[LL_MOD_MAXCH];
    unsigned tick;             /* 0..speed-1                            */
    unsigned speed, tempo;     /* ProTracker semantics (tempo 125=50Hz) */
    unsigned row, order;
    uint32_t subtick;          /* frames elapsed in current tick        */
    uint8_t  gvol;             /* 0..64 global                          */
    int      end;
} ll_mod_state;

void ll_mod_start(ll_mod_state *st, const ll_mod *m);
/* mix `frames` stereo s16 pairs into dst (dst frames*2 int16)          */
void ll_mod_mix(ll_mod_state *st, int16_t *dst, size_t frames);

/* CI oracle: deterministic offline render of first `ms` milliseconds
 * into a freshly sized s16 stereo buffer; returns bytes (frames*4).   */
size_t ll_mod_render(const ll_mod *m, unsigned ms, int16_t **out);

#endif
