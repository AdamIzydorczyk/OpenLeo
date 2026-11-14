/* ll_mod.c - MOD player for the Longsoft repack variant (see ll_mod.h
 * for the verified container layout: standard header, full numbered
 * pattern set, contiguous samples).  ProTracker 2.3 playback semantics
 * (review M-01..M-12), integer-only math, fixed 44.1 kHz output.
 * The tick/tickless effect split, per-channel parameter memory and
 * loop-boundary handling follow the ProTracker model; the effect scan of
 * all 22 shipped SOUND containers confirmed which groups matter (tempo
 * Fxx
 * in KARA, Dxx breaks everywhere, 9xx offsets, 4xy vibrato with
 * continuation rows, E1/EA/EB/E9, porta 1/2/3 with parameter memory).
 * Tremolo (7xy) and the E-chained patternloop counter (E60 variants)
 * appear in none of these songs and stay unimplemented. */
#include "ll_mod.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ProTracker period tables: [finetune][semitone], octave 0 = 1712..
 * (engine walks its 61-entry fine-0 table 1712..56 in FUN_00012c40). */
static const int16_t ptune[16][12] = {
    { 1712,1616,1524,1440,1356,1280,1208,1140,1076,1016, 960, 906 },
    { 1627,1536,1450,1369,1292,1220,1152,1087,1027, 970, 915, 864 },
    { 1548,1461,1380,1303,1229,1162,1097,1036, 979, 925, 873, 824 },
    { 1473,1390,1312,1238,1169,1103,1041, 983, 929, 878, 829, 783 },
    { 1399,1321,1247,1177,1111,1048, 990, 934, 883, 834, 787, 743 },
    { 1327,1253,1183,1117,1055, 996, 940, 888, 839, 792, 748, 706 },
    { 1258,1188,1121,1058,1000, 944, 892, 843, 796, 752, 710, 670 },
    { 1191,1124,1062,1003, 947, 895, 845, 798, 754, 712, 672, 635 },
    { 1127,1064,1005, 949, 897, 848, 800, 756, 714, 675, 638, 603 },
    { 1065,1006, 950, 897, 847, 800, 755, 713, 674, 637, 602, 568 },
    { 1005, 950, 897, 847, 800, 755, 713, 673, 636, 601, 567, 536 },
    {  948, 897, 847, 800, 755, 713, 673, 635, 600, 566, 535, 505 },
    {  895, 846, 799, 754, 712, 673, 635, 600, 566, 535, 505, 477 },
    {  845, 798, 754, 712, 673, 635, 600, 566, 534, 505, 477, 451 },
    {  798, 754, 712, 673, 635, 600, 566, 534, 504, 477, 450, 425 },
    {  755, 713, 673, 635, 600, 566, 534, 504, 476, 450, 425, 401 },
};

/* map a raw period to the engine note index (0..59); -1 if between
 * table rows (portamento targets keep the raw period instead). */
static int note_index(int period)
{
    for (int oct = 0; oct < 5; oct++)
        for (int s = 0; s < 12; s++) {
            int p = ptune[0][s] >> oct;
            if (period > p) continue;
            if (period == p) return oct * 12 + s;
            return -1;
        }
    return -1;
}

static int16_t note_period(int idx, int fine)
{
    if (idx < 0 || idx >= 60) return 0;
    int p = ptune[fine & 15][idx % 12] >> (idx / 12);
    return (int16_t)(p < 32 ? 32 : p);
}

/* sample-frame step in 16.16 for the Amiga clock (M-11):
 * inc = 7093789.2/(2*period)/44100 * 65536 = 5270959/period.
 * (The former 5278235 constant was 0.14% sharp.) */
static uint32_t period_inc(int period)
{
    if (period < 32) period = 32;
    return (uint32_t)(5270959u / (uint32_t)period);
}

/* M-09: ProTracker vibrato = one sine sampled at 64 positions per
 * cycle, amplitude 255 (period delta = table * depth / 128, i.e. up to
 * +-2*depth semitones-ish; the old 32-position/126 table was ~4x too
 * shallow). */
static const int16_t sin64[64] = {
      0,  25,  50,  74,  98, 120, 142, 162, 180, 197, 212, 225,
    236, 244, 250, 254, 255, 254, 250, 244, 236, 225, 212, 197,
    180, 162, 142, 120,  98,  74,  50,  25,   0, -25, -50, -74,
    -98,-120,-142,-162,-180,-197,-212,-225,-236,-244,-250,-254,
   -255,-254,-250,-244,-236,-225,-212,-197,-180,-162,-142,-120,
    -98, -74, -50, -25,
};

#define CLAMP64(v) ((v) < 0 ? 0 : ((v) > 64 ? 64 : (v)))

int ll_mod_open(ll_mod *m, const unsigned char *img, size_t len,
                char *err, size_t errsz)
{
    static const struct { const char *id; int nch; } ids[] = {
        { "M.K.", 4 }, { "M!K!", 4 }, { "FLT4", 4 }, { "4CHN", 4 },
        { "6CHN", 6 }, { "8CHN", 8 }, { "CD81", 8 }, { "OKTA", 8 },
    };
    memset(m, 0, sizeof *m);
    if (len < 1084) { snprintf(err, errsz, "short header"); return 1; }
    m->img = img;
    m->img_len = len;
    memcpy(m->id, img + 1080, 4);
    m->id[4] = 0;
    for (unsigned i = 0; i < sizeof ids / sizeof ids[0]; i++)
        if (memcmp(img + 1080, ids[i].id, 4) == 0)
            m->nch = (unsigned char)ids[i].nch;
    if (!m->nch) { snprintf(err, errsz, "unknown id '%.4s'", img + 1080); return 1; }
    m->nseq = img[950];
    if (!m->nseq || m->nseq > 128) {
        snprintf(err, errsz, "bad songlen %u", m->nseq); return 1;
    }
    /* Pattern table at 952 (standard place): these repacks store the
     * numbered pattern set whole, slots 0..max+1, one after another --
     * the engine loader (MODGAP_13000) sets DAT_000aee99 = max(table)+1
     * and FUN_00012d14 reads exactly that many patterns, so positions
     * index the slots directly.  TUNGA: max id 30 -> 31 slots, and the
     * sample region then starts at 1084 + 31*1024 = 32828, matching
     * the byte-exact sample boundaries at EOF. */
    int npat = 0;
    for (unsigned i = 0; i < 128; i++)
        if (img[952 + i] + 1 > npat) npat = img[952 + i] + 1;
    size_t patbytes = (size_t)npat * m->nch * 256;
    if (!npat || 1084 + patbytes > len) {
        snprintf(err, errsz, "patterns exceed file (%d x %u ch)", npat, m->nch);
        return 1;
    }
    m->npat_stored = npat;
    m->pat = img + 1084;
    int *map = (int *)malloc(sizeof(int) * m->nseq);
    if (!map) { snprintf(err, errsz, "oom"); return 1; }
    for (unsigned i = 0; i < m->nseq; i++) map[i] = img[952 + i];
    m->pos2pat = map;

    /* Sample data: contiguous right after the pattern set, in directory
     * order, using the STANDARD 30-byte instrument records at 20:
     * len (words BE) @+22, finetune @+24, volume @+25, loop start (words
     * BE) @+26, loop length (words BE) @+28 -- exactly the fields the
     * engine copies at +0x16..+0x1C.  The engine reads min(len, file
     * remaining) and zero-fills, so dlen clamps at EOF; TUNGA fits the
     * region exactly: 32828 + 171396 = 204224 = file size. */
    size_t p = 1084 + patbytes, off = 0;
    m->nsmp = 31 > LL_MOD_MAXSMP ? LL_MOD_MAXSMP : 31;
    size_t region = len - p;
    for (int i = 0; i < m->nsmp; i++) {
        const unsigned char *e = img + 20 + 30 * i;
        uint32_t L = (uint32_t)((e[22] << 8) | e[23]) * 2u;
        m->smp[i].len = L;
        m->smp[i].fine = e[24] & 0x0F;
        m->smp[i].vol  = e[25] > 64 ? 64 : e[25];
        m->smp[i].loop_start = (uint32_t)((e[26] << 8) | e[27]) * 2u;
        m->smp[i].loop_len   = (uint32_t)((e[28] << 8) | e[29]) * 2u;
        size_t avail = off < region ? region - off : 0;
        m->smp[i].dlen = L < avail ? L : (uint32_t)avail;
        /* M-02 (load half): a loop whose end runs past the sample must
         * end AT the sample end, never beyond - otherwise the wrap math
         * in ll_mod_mix misbehaves on truncated tails. */
        if (m->smp[i].loop_start >= m->smp[i].dlen)
            m->smp[i].loop_len = 0;
        else if (m->smp[i].loop_len > m->smp[i].dlen - m->smp[i].loop_start)
            m->smp[i].loop_len = m->smp[i].dlen - m->smp[i].loop_start;
        m->smp[i].data = img + p + off;
        off += L;
    }
    snprintf(m->title, sizeof m->title, "%.20s", (const char *)img);
    return 0;
}

void ll_mod_close(ll_mod *m)
{
    free((void *)m->pos2pat);
    memset(m, 0, sizeof *m);
}

void ll_mod_start(ll_mod_state *st, const ll_mod *m)
{
    memset(st, 0, sizeof *st);
    st->m = m;
    st->speed = 6;
    st->tempo = 125;
    st->gvol = 64;
    st->break_row = st->jump_order = 0x7F;   /* no pending (M-04) */
    for (int c = 0; c < LL_MOD_MAXCH; c++)
        st->v[c].fine_set = 0xFF;            /* E5 memory: no override */
}

/* note word (standard 4-byte entry): b0 = inst-hi4 | period-hi4,
 * b1 = period-lo, b2 = inst-lo4 | effect, b3 = param. */
static void rd_entry(const ll_mod *m, int pat, int row, int chn,
                     int *period, int *inst, int *eff, int *par)
{
    const unsigned char *e =
        m->pat + (size_t)pat * m->nch * 256 + (size_t)(row * m->nch + chn) * 4;
    *period = ((e[0] & 0x0F) << 8) | e[1];
    *inst = (e[0] & 0xF0) | (e[2] >> 4);
    *eff = e[2] & 0x0F;
    *par = e[3];
}

/* these repacks leave the directory volume byte 0 on most instruments
 * (1..13 in TUNGA); the engine's voice kick defaults those to full
 * volume, otherwise the songs would be silent.  1..64 used as-is. */
static uint8_t smp_vol(const ll_mod *m, int idx)
{
    uint8_t v = m->smp[idx - 1].vol;
    return v == 0 ? 64 : (v > 64 ? 64 : v);
}

/* effective finetune of a voice: E5 override first, else the sample's */
static int voice_fine(const ll_mod_state *st, int c)
{
    if (st->v[c].fine_set != 0xFF)
        return st->v[c].fine_set;
    if (st->v[c].smp)
        return st->m->smp[st->v[c].smp - 1].fine;
    return 0;
}

/* kick a voice with the sample for `inst` at period p */
static void voice_kick(ll_mod_state *st, int c, int p, int inst)
{
    const ll_mod *m = st->m;
    if (inst > 0 && inst <= m->nsmp) {
        st->v[c].smp = (uint8_t)inst;
        st->v[c].vol = smp_vol(m, inst);
    }
    st->v[c].period = (int16_t)p;
    st->v[c].pos = 0;
    st->v[c].inc = period_inc(p);
    st->v[c].act = 1;
    st->v[c].cut = 0;
}

/* one 50 Hz tick: row-0 handling on tick==0, effects every tick */
static void do_tick(ll_mod_state *st)
{
    const ll_mod *m = st->m;
    if (st->order >= m->nseq) { st->end = 1; return; }
    int pat = m->pos2pat[st->order];
    for (int c = 0; c < (int)m->nch; c++) {
        int p, ins, ef, pr;
        rd_entry(m, pat, (int)st->row, c, &p, &ins, &ef, &pr);

        /* ProTracker effect-parameter memory (per channel): a row that
         * names an effect but leaves the parameter at 0 reuses the last
         * one. B/D/F have no memory; a fully blank entry is "no effect"
         * (so arpeggio does NOT continue through blanks). */
        int memo = (ef <= 0x0A || ef == 0x0C || ef == 0x0E);
        if (memo && pr)
            st->v[c].pmem[ef] = (uint8_t)pr;
        int q = pr;
        if (ef && !q && memo && !(ef == 0x00))
            q = st->v[c].pmem[ef];

        if (st->tick == 0) {
            st->v[c].e0 = (uint8_t)ef;
            st->v[c].e1 = (uint8_t)q;
            if (ins > 0 && ins <= m->nsmp) {
                st->v[c].smp = (uint8_t)ins;
                if (p == 0)          /* instrument change alone:
                                        only retune on next note */
                    st->v[c].vol = smp_vol(m, ins);
            }
            /* 4xy/6xy memories land at row start (per nibble != 0) */
            if (ef == 0x04) {
                if (q >> 4) st->v[c].vib_depth = (uint8_t)(q >> 4);
                if (q & 0x0F) st->v[c].vib_speed = (uint8_t)(q & 0x0F);
            }
            if (ef == 0x03 && q)
                st->v[c].porta_speed = (uint8_t)q;   /* remembered by 5 */
            if (ef == 0x09 && q)
                st->v[c].off9 = (uint8_t)q;          /* M-03 memory */
            /* M-04: B/D are remembered for the END of this row */
            if (ef == 0x0B) st->jump_order = (unsigned)pr;
            if (ef == 0x0D) {
                int br = (pr >> 4) * 10 + (pr & 0x0F); /* DECIMAL digits */
                st->break_row = (unsigned)(br > 0x7F ? 0x7F : br);
            }
            if (ef == 0x0F && pr) {
                if (pr < 0x20) st->speed = (unsigned)pr;
                else st->tempo = (unsigned)(pr < 32 ? 32 : pr);
            }
            /* note trigger: everything EXCEPT porta (3/5) and delayed
             * notes kicks (M-05; 1/2 retrigger at the old period and
             * merely move the porta target). */
            if (p > 0) {
                int ni = note_index(p);
                int ep = ni >= 0 ? note_period(ni, voice_fine(st, c))
                                 : (int16_t)p;
                int ed = (ef == 0x0E && (q >> 4) == 0x0D);
                if (ef == 3 || ef == 5) {
                    st->v[c].tgt_period = (int16_t)ep;
                    st->v[c].porta = 3;
                } else if (ed) {
                    st->v[c].ed_per = (int16_t)ep;
                    st->v[c].ed_ins = (uint8_t)(ins > 0 && ins <= m->nsmp
                                                ? ins : 0);
                    st->v[c].ed_act = 1;
                } else if (ef == 1 || ef == 2) {
                    int oldp = st->v[c].period ? st->v[c].period : ep;
                    voice_kick(st, c, oldp, ins);
                    st->v[c].tgt_period = (int16_t)ep;
                    st->v[c].porta = (int8_t)ef;
                } else {
                    voice_kick(st, c, ep, ins);
                    st->v[c].porta = 0;
                    if (ef == 4 || ef == 6)
                        st->v[c].vibpos = 0;
                }
            }
            /* row-only E effects (M-07 renumbering: E1/E2 fine porta,
             * E5 set finetune, EA/EB fine volume = tick 0 ONLY) */
            if (ef == 0x0E) switch (q >> 4) {
                int np;
            case 1: /* E1x: fine porta up, once */
                if (st->v[c].period) { np = st->v[c].period - (q & 0x0F);
                    if (np < 32) np = 32;
                    st->v[c].period = (int16_t)np;
                    st->v[c].inc = period_inc(np); }
                break;
            case 2: /* E2x: fine porta down, once */
                if (st->v[c].period) { np = st->v[c].period + (q & 0x0F);
                    st->v[c].period = (int16_t)np;
                    st->v[c].inc = period_inc(np); }
                break;
            case 5: /* E5x: set finetune (applies to following notes) */
                st->v[c].fine_set = (uint8_t)(q & 0x0F);
                break;
            case 6: /* E6x: pattern loop at row 0: jump back x orders
                     * (no shipped song uses it; PT-faithful minimum) */
                if (st->row == 0 && (q & 0x0F))
                    st->order = st->order > (unsigned)(q & 0x0F)
                                  ? st->order - (unsigned)(q & 0x0F) : 0;
                break;
            case 0xA: /* EAx: fine volume up, once */
                st->v[c].vol = (uint8_t)CLAMP64(st->v[c].vol + (q & 0x0F));
                break;
            case 0xB: /* EBx: fine volume down, once */
                st->v[c].vol = (uint8_t)CLAMP64(st->v[c].vol - (q & 0x0F));
                break;
            case 0xE: /* EEx: repeat this row x extra times (EE) */
                if (q & 0x0F) st->row_repeat = (unsigned)(q & 0x0F);
                break;
            default: break;
            }
            if (ef == 0x0C)                       /* set volume, clamped */
                st->v[c].vol = (uint8_t)(q > 64 ? 64 : q);
            if (ef == 0x09 && st->v[c].off9)     /* 9: tick 0, q<<24 (M-03) */
                st->v[c].pos = (uint32_t)st->v[c].off9 << 24;
            st->v[c].ed_at = (ef == 0x0E && (q >> 4) == 0x0D)
                                      ? (uint8_t)(q & 0x0F) : 0xFF;
        }

        /* per-tick effects (continuations use the row's e0/e1) */
        int e = st->v[c].e0, qq = st->v[c].e1;
        /* ED delayed note fires at its tick (M-07: the 12th E command) */
        if (st->v[c].ed_act && st->tick == st->v[c].ed_at) {
            voice_kick(st, c, st->v[c].ed_per, st->v[c].ed_ins);
            st->v[c].ed_act = 0;
            if (st->v[c].off9)
                st->v[c].pos = (uint32_t)st->v[c].off9 << 24;
        }
        switch (e) {
        case 0x0: /* arpeggio (only with a parameter); M-08: with no
                   * arpeggio the pitch reverts to the base period. */
            if (qq) {
                int semi = (st->tick % 3 == 1) ? (qq >> 4) :
                           (st->tick % 3 == 2) ? (qq & 0x0F) : 0;
                if (st->v[c].period && st->v[c].smp) {
                    int ni = note_index(st->v[c].period);
                    if (ni >= 0)
                        st->v[c].inc = period_inc(
                            note_period(ni + semi, voice_fine(st, c)));
                }
            } else if (st->v[c].period) {
                st->v[c].inc = period_inc(st->v[c].period);
            }
            break;
        case 0x1: /* porta up */
        case 0x2: /* porta down */
        case 0x3: /* porta to note (plain porta; no volume slide) */
            if (st->v[c].porta && st->v[c].tgt_period && st->tick) {
                int d = e == 3 ? st->v[c].porta_speed : qq;
                if (!d) break;
                int np = st->v[c].period;
                if (e == 0x1 || (e == 0x3 && np > st->v[c].tgt_period)) {
                    np -= d; if (np < st->v[c].tgt_period) np = st->v[c].tgt_period;
                } else {
                    np += d; if (np > st->v[c].tgt_period) np = st->v[c].tgt_period;
                }
                st->v[c].period = (int16_t)np;
                st->v[c].inc = period_inc(np);
            }
            break;
        case 0x4: /* vibrato: 64-position sine (M-09), memories (M-06);
                   * a note on the same row reset the phase at the kick */
        case 0x6: /* 6: xy is the VOLUME slide; vibrato speed/depth come
                   * from the last 4xy (M-06) */
            st->v[c].vibpos = (int16_t)((st->v[c].vibpos +
                                         st->v[c].vib_speed) & 63);
            if (st->v[c].period) {
                int mod = sin64[(unsigned)st->v[c].vibpos] *
                          (int)st->v[c].vib_depth / 128;
                st->v[c].inc = period_inc(st->v[c].period + mod);
            }
            if (e == 6) goto volslide;
            break;
        case 0x5: /* porta (speed from last 3xy, M-06) + volume slide */
            if (st->v[c].porta && st->v[c].tgt_period && st->tick) {
                int d = st->v[c].porta_speed;
                int np = st->v[c].period;
                if (d) {
                    if (np > st->v[c].tgt_period) {
                        np -= d;
                        if (np < st->v[c].tgt_period) np = st->v[c].tgt_period;
                    } else {
                        np += d;
                        if (np > st->v[c].tgt_period) np = st->v[c].tgt_period;
                    }
                    st->v[c].period = (int16_t)np;
                    st->v[c].inc = period_inc(np);
                }
            }
            goto volslide;
        volslide:
            if (st->tick) {
                int x = qq >> 4, y = qq & 0x0F;
                /* M-12: FastTracker rule - up wins when both set */
                int v = st->v[c].vol + (x ? x : -y);
                st->v[c].vol = (uint8_t)CLAMP64(v);
            }
            break;
        case 0x9: /* sample offset: applied at tick 0 above (M-03) */
            break;
        case 0xA: /* volume slide */
            goto volslide;
        case 0xC: /* set volume: tick 0 only (applied above) */
            break;
        case 0xE:
            switch (qq >> 4) {
            case 9: /* E9x: sample retrigger every x ticks (M-07: E9 is
                     * retrigger; note CUT is EC) */
                if ((qq & 0x0F) && st->tick && st->v[c].period &&
                    st->tick % (unsigned)(qq & 0x0F) == 0 &&
                    st->v[c].act)
                    st->v[c].pos = 0;
                break;
            case 0xC: /* ECx: note cut at tick x */
                if (st->tick == (unsigned)(qq & 0x0F)) st->v[c].cut = 1;
                break;
            default: break;   /* 1,2,5,6,A,B,D,E handled at tick 0 */
            }
            break;
        default: break;
        }
    }

    st->tick++;
    if (st->tick < st->speed)
        return;
    /* ---- end of row (M-04: pending B/D apply HERE, not in-row) ---- */
    st->tick = 0;
    st->row++;
    if (st->row_repeat) {          /* EEx pattern repeat */
        st->row_repeat--;
        st->row--;
        return;
    }
    if (st->row >= 64 || st->break_row != 0x7F || st->jump_order != 0x7F) {
        if (st->jump_order != 0x7F)
            st->order = st->jump_order;
        else
            st->order++;
        st->row = st->break_row != 0x7F ? (st->break_row < 64
                                           ? st->break_row : 0) : 0;
        st->break_row = st->jump_order = 0x7F;
        if (st->order >= m->nseq) st->order = 0;   /* songs loop */
        if (st->row >= 64) st->row = 0;
    }
}

void ll_mod_mix(ll_mod_state *st, int16_t *dst, size_t frames)
{
    const ll_mod *m = st->m;
    for (size_t i = 0; i < frames; i++) {
        /* M-01: one tick = 110250/tempo audio frames (tempo BPM ->
         * tempo*2/5 ticks/s; 125 -> 882 = 1/50 s). Kept in 16.16 so a
         * tempo like 140 (787.5 frames) stays exact instead of drifting.
         * (The former tempo*44100/6250 was only right at 125.) */
        uint32_t tempo = st->tempo < 32 ? 32 : st->tempo;
        uint32_t tick_fp = ((uint32_t)110250u << 16) / tempo;
        if (st->subtick == 0 || st->subtick >= tick_fp) {
            if (st->subtick >= tick_fp)
                st->subtick -= tick_fp;   /* fractional carry-over (M-01) */
            if (st->end) { dst[2*i] = dst[2*i+1] = 0; continue; }
            do_tick(st);
        }
        int32_t mixL = 0, mixR = 0;  /* int32 accumulator, clamp once
                                      * (review 5.x close + M-10: the old
                                      * per-channel int16 saturating add
                                      * order-dependent-clipped) */
        for (int c = 0; c < (int)m->nch; c++) {
            int sidx = st->v[c].smp;
            if (!st->v[c].act || st->v[c].cut || !st->v[c].period ||
                sidx <= 0 || sidx > m->nsmp)
                continue;
            const ll_mod_smp *s = &m->smp[sidx - 1];
            if (!s->dlen || !st->v[c].inc) continue;
            uint32_t pos = st->v[c].pos;
            uint32_t idx = pos >> 16;
            /* M-02: loop at loop_start..loop_start+loop_len, wrap by
             * loop_len so the fractional overshoot survives (the old
             * code looped at dlen and reset pos, playing tails of
             * looped samples and glitching the wrap phase). */
            int looping = s->loop_len > 2 && s->loop_start < s->dlen &&
                          s->loop_start + s->loop_len <= s->dlen;
            if (looping) {
                uint32_t lend = s->loop_start + s->loop_len;
                if (idx >= lend) {
                    if (pos < ((uint32_t)s->loop_len << 16)) {
                        st->v[c].act = 0;   /* offset landed past the loop */
                        continue;
                    }
                    pos -= (uint32_t)s->loop_len << 16;
                    st->v[c].pos = pos;
                    idx = pos >> 16;
                    if (idx >= lend) continue;   /* degenerate guard */
                }
            } else if (idx >= s->dlen) {
                st->v[c].act = 0;
                continue;
            }
            uint32_t nx = idx + 1;
            if (looping && nx >= s->loop_start + s->loop_len)
                nx = s->loop_start;       /* interpolate ACROSS the loop
                                             edge, not into the tail */
            if (nx >= s->dlen) nx = idx;
            int s0 = (int)(signed char)s->data[idx] * 256; /* s8 -> s16 */
            int s1 = (int)(signed char)s->data[nx] * 256;
            int samp = s0 + (int)(((int64_t)(s1 - s0) *
                                   (int)(pos & 0xFFFF)) >> 16); /* linear */
            int vol = st->v[c].vol * st->gvol;               /* 0..4096 */
            int32_t v = (int32_t)samp * vol / 4096;
            /* M-10: Amiga 4-voice panning L,R,R,L (the old L,R,L,R put
             * two adjacent melody channels on the same side). Channels
             * beyond 4 (6/8-channel repacks) repeat the pattern.
             * ~75% separation = 25% cross-feed, and the master runs at
             * ~-6 dB (half): four full-volume voices can sum to 4x
             * full-scale, and without that headroom the mix flattened
             * (measured before this gain: 2.4% of RAN.MOD samples hit
             * the clamp). */
            int32_t vl = 0, vr = 0;
            if ((c & 3) == 1 || (c & 3) == 2)
                vr = v;
            else
                vl = v;
            mixL += vl - vl / 4 + vr / 4;
            mixR += vr - vr / 4 + vl / 4;
            st->v[c].pos += st->v[c].inc;
        }
        mixL /= 2;                      /* master ~-6 dB (M-10) */
        mixR /= 2;
        dst[2*i]   = (int16_t)(mixL > 32767 ? 32767 :
                               mixL < -32768 ? -32768 : mixL);
        dst[2*i+1] = (int16_t)(mixR > 32767 ? 32767 :
                               mixR < -32768 ? -32768 : mixR);
        st->subtick += 0x10000u;            /* one audio frame elapsed */
    }
}

size_t ll_mod_render(const ll_mod *m, unsigned ms, int16_t **out)
{
    size_t frames = (size_t)ms * LL_MOD_RATE / 1000;
    int16_t *buf = (int16_t *)calloc(frames * 2, sizeof(int16_t));
    if (!buf) { *out = NULL; return 0; }
    ll_mod_state st;
    ll_mod_start(&st, m);
    ll_mod_mix(&st, buf, frames);
    *out = buf;
    return frames * 2 * sizeof(int16_t);
}
