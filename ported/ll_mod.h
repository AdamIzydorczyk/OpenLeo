/* ll_mod.h - Module (MOD) music player for the Lew Leon engine variant.
 *
 * The SOUND llrs container (LEO/SOUND) carries 22 level/menu tracks as
 * '<name>.MOD' payloads (level .des files select them via `muzyka=` -
 * verified: RAN.DES->ran.mod, TUNGA1.DES->tunga.mod, KADMAR1.DES->
 * piramid.mod, ...).  The files are standard Amiga .MOD headers
 * (title@0, 31x30B instrument dir@20 with STANDARD fields len-words-BE
 * @rec+22, fine@+24, vol@+25, loop-start-words@+26, loop-len@+28,
 * songlen byte@950, 128-entry pattern table@952, id string@1080).
 * The body follows the engine loader exactly (Ghidra MODGAP_13000 /
 * FUN_00012d14 RE 2026-11-14, TUNGA byte-exact verification):
 *   - DAT_000aee99 = max(pattern table)+1, and the file stores the
 *     WHOLE numbered pattern set back-to-back at 1084 (nch*256 B per
 *     slot, standard 4-byte note words); positions index slots direct,
 *   - sample data follows immediately, contiguous in directory order;
 *     the game clamps reads to the remaining file bytes and zero-fills
 *     the rest.  TUNGA proves the layout: 1084+31*1024 = 32828 and the
 *     31 directory lengths sum to exactly 171396 = 204224-32828,
 *   - no extension directory in any shipped track (earlier claims of a
 *     unique-pattern remap and 29-byte ext dir were parsing artifacts;
 *     the engine's FUN_000132b0 ext init never fires on these files).
 * Verified per-file: songlen byte@950 (TUNGA 38, RAN 28, ZARIA 43,
 * KARA 58, LEW 33, RAN1 2, ENA2 40, MAPKA 10); engine period->note
 * table 61 entries 1712..56 at VA 0xa7272 (Amiga); signature table
 * M.K./M!K!/FLT4/4CHN/6CHN/8CHN/CD81/OKTA/16CN/32CN at 0xa7204.
 *
 * Playback semantics: ProTracker 2.3 tick model (review M-01..M-12):
 * ticks are 110250/tempo audio frames kept fractionally (Fxx<0x20 =
 * speed in ticks/row, Fxx>=0x20 = tempo - KARA uses tempo changes);
 * effects 0-9,A-F plus E1,E2,E5,E6,E9,EA,EB,EC,ED,EE with per-channel
 * parameter memory on continuation rows; B/D land at the END of the row
 * (Dxx row digits are decimal); sample loops wrap at
 * loop_start+loop_len keeping the fractional overshoot; sample playback
 * 8-bit signed with linear interpolation across the loop edge; Amiga
 * period clock inc = 5270959/period in 16.16; the mixer accumulates in
 * int32 and clamps once with Amiga L,R,R,L panning.  Tremolo (7xy) and
 * the E6 loop-counter forms appear in no shipped song and stay
 * unimplemented.  Instrument default volumes: see ll_mod.c smp_vol()
 * (the engine defaults 0 to full; FUN_00015484 voice handlers).
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
    int          npat_stored;      /* numbered pattern slots in file    */
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
        uint8_t e0, e1;        /* effect + resolved param of the row
                                * (effects continue for the whole row)  */
        uint8_t pmem[16];      /* ProTracker per-channel parameter
                                * memory: a row naming an effect with a
                                * 0 param reuses the last one (M-06)   */
        uint8_t porta_speed;   /* 3xx speed, remembered by 5xy (M-06)  */
        uint8_t vib_speed, vib_depth; /* 4xy nibbles, remembered      */
        uint8_t off9;          /* 9xx sample offset memory (M-03)      */
        uint8_t fine_set;      /* E5 finetune override, 0xFF = none    */
        uint8_t cut;           /* EC note-cut flag                     */
        uint8_t ed_act;        /* ED: pending delayed note trigger     */
        uint8_t ed_ins;
        uint8_t ed_at;         /*   ... fired at this tick             */
        int16_t ed_per;
        int8_t  porta;         /* running porta 0/1/2/3                */
        int16_t vibpos;        /* vibrato phase 0..63 (M-09)           */
    } v[LL_MOD_MAXCH];
    unsigned tick;             /* 0..speed-1                            */
    unsigned speed, tempo;     /* ProTracker semantics (tempo 125=50Hz) */
    unsigned row, order;
    unsigned break_row, jump_order; /* pending D/B applied at row END
                                     * (M-04); 0x7F = none pending      */
    unsigned row_repeat;       /* EE: extra repeats of this row left    */
    uint32_t subtick;          /* frames elapsed in current tick (16.16,
                                * fractional carry-over = M-01)         */
    uint8_t  gvol;             /* 0..64 global                          */
    int      end;
} ll_mod_state;

void ll_mod_start(ll_mod_state *st, const ll_mod *m);
/* renders `frames` stereo s16 pairs into dst (dst = frames*2 int16).
 * Owns dst: every frame is written from scratch, so callers may reuse a
 * buffer without zeroing it (getting this wrong made the in-game music
 * stack chunk on chunk = clipping squeak).                      */
void ll_mod_mix(ll_mod_state *st, int16_t *dst, size_t frames);

/* CI oracle: deterministic offline render of first `ms` milliseconds
 * into a freshly sized s16 stereo buffer; returns bytes (frames*4).   */
size_t ll_mod_render(const ll_mod *m, unsigned ms, int16_t **out);

#endif
