/* ll_mod.c - MOD player for the Longsoft repack variant (see ll_mod.h
 * for the verified container layout).  Standard ProTracker playback
 * semantics, integer-only math, fixed 44.1 kHz output. */
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

/* sample-frame step in 16.16 for the Amiga clock:
 * inc = 7093789.2/(2*period)/44100 * 65536 = 52782351/period/10      */
static uint32_t period_inc(int period)
{
    if (period < 32) period = 32;
    return (uint32_t)(52782351 / (uint32_t)period);
}

static const int8_t sin32[32] = {
      0, 30, 60, 86,107,121,126,121,107, 86, 60, 30,  0,-30,-60,-86,
   -107,-121,-126,-121,-107,-86,-60,-30,  0, 30, 60, 86,107,121,126,121
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
    /* pattern table -> first-appearance unique remap (loader FUN_000131b8):
     * the file stores only unique pattern IDs, once each, in order. */
    int ids_seen[128], stored[128], npat = 0;
    for (unsigned i = 0; i < m->nseq; i++) {
        int id = img[1268 + i], j;
        for (j = 0; j < npat; j++)
            if (ids_seen[j] == id) break;
        if (j == npat && npat < 128) { ids_seen[npat++] = id; }
        stored[i] = j;
    }
    size_t patbytes = (size_t)npat * m->nch * 256;
    if (1084 + patbytes > len) {
        snprintf(err, errsz, "patterns exceed file (%d x %u ch)", npat, m->nch);
        return 1;
    }
    m->npat_stored = npat;
    m->pat = img + 1084;
    int *map = (int *)malloc(sizeof(int) * m->nseq);
    if (!map) { snprintf(err, errsz, "oom"); return 1; }
    for (unsigned i = 0; i < m->nseq; i++) map[i] = stored[i];
    m->pos2pat = map;

    /* extension instrument directory: count byte + 29-byte records
     * (engine FUN_000132b0); ext instruments are sized but not played
     * in v1 (the on-screen game used instruments 1..31 on these songs). */
    size_t pd = 1084 + patbytes, p = pd;
    int ext = 0;
    if (p < len) {
        int c = img[p];
        if (c > 0 && c <= LL_MOD_MAXSMP - 31 && p + 1 + (size_t)c * 29 <= len)
            ext = c;
    }
    if (ext) p += 1 + (size_t)ext * 29;

    /* sample data: contiguous in directory order, clamped at EOF
     * (engine reads min(len, remaining) and zero-fills the buffer) */
    m->nsmp = 31 + ext;
    if (m->nsmp > LL_MOD_MAXSMP) m->nsmp = LL_MOD_MAXSMP;
    size_t region = len - p, off = 0;
    for (int i = 0; i < m->nsmp; i++) {
        uint32_t L = 0;
        if (i < 31) {
            const unsigned char *e = img + 20 + 30 * i;
            L = (uint32_t)((e[0] << 8 | e[1])) * 2u;
            m->smp[i].fine = (e[6] >> 4) & 0x0F;
            m->smp[i].vol  = e[7] > 64 ? 64 : e[7];
            m->smp[i].loop_start = (uint32_t)((e[8] << 8 | e[9])) * 2u;
            m->smp[i].loop_len   = (uint32_t)((e[10] << 8 | e[11])) * 2u;
        } else {
            const unsigned char *e = img + pd + 1 + (size_t)(i - 31) * 29;
            L = (uint32_t)((e[0] << 8 | e[1])) * 2u;   /* sized only */
            m->smp[i].vol = 64;
        }
        m->smp[i].len = L;
        size_t avail = off < region ? region - off : 0;
        m->smp[i].dlen = L < avail ? L : (uint32_t)avail;
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
}

/* note word (6/8CHN layout, engine FUN_00012c40): b0 = inst-bit5 |
 * period-hi4, b1 = period-lo, b2 = inst-lo4 | effect, b3 = param. */
static void rd_entry(const ll_mod *m, int pat, int row, int chn,
                     int *period, int *inst, int *eff, int *par)
{
    const unsigned char *e =
        m->pat + (size_t)pat * m->nch * 256 + (size_t)(row * m->nch + chn) * 4;
    *period = ((e[0] & 0x0F) << 8) | e[1];
    *inst = (e[0] & 0x10) | (e[2] >> 4);
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
    if (st->row >= 64) {          /* advance position */
        st->row = 0;
        st->order++;
        if (st->order >= m->nseq) st->order = 0;   /* songs loop */
        pat = m->pos2pat[st->order];
    }
    for (int c = 0; c < (int)m->nch; c++) {
        int p, ins, ef, pr;
        rd_entry(m, pat, (int)st->row, c, &p, &ins, &ef, &pr);
        if (st->tick == 0 && ins > 0 && ins <= m->nsmp) {
            st->v[c].smp = (uint8_t)ins;
            if (p == 0)                  /* instrument change alone:
                                            only retune on next note */
                st->v[c].vol = smp_vol(m, ins);
        }
        if (st->tick == 0) {
            st->v[c].e0 = (uint8_t)ef;
            st->v[c].e1 = (uint8_t)pr;
            if (ef >= 0x01 && ef <= 0x06)
                st->v[c].porta_spd = (uint8_t)pr; /* remembered by 5/6 */
            if (p > 0) {                                  /* new note */
                int ni = note_index(p);
                int fine = st->v[c].smp ? m->smp[st->v[c].smp - 1].fine : 0;
                int ep = ni >= 0 ? note_period(ni, fine) : (int16_t)p;
                if (ef == 1 || ef == 2 || ef == 3) {
                    st->v[c].tgt_period = (int16_t)ep;
                    st->v[c].porta = (int8_t)ef;
                } else {
                    voice_kick(st, c, ep, ins);
                    st->v[c].porta = 0;
                    st->v[c].vibpos = 0;
                }
                st->v[c].retrig_cnt = 0;
            }
        }
        /* per-tick effects */
        int e = st->v[c].e0, q = st->v[c].e1;
        switch (e) {
        case 0x0: /* arpeggio (only with a parameter) */
            if (q) {
                int semi = (st->tick % 3 == 1) ? (q >> 4) :
                           (st->tick % 3 == 2) ? (q & 0x0F) : 0;
                if (st->v[c].period && st->v[c].smp) {
                    int ni = note_index(st->v[c].period);
                    int fine = m->smp[st->v[c].smp - 1].fine;
                    if (ni >= 0)
                        st->v[c].inc = period_inc(note_period(ni + semi, fine));
                }
            }
            break;
        case 0x1: /* porta up */
        case 0x2: /* porta down */
        case 0x3: /* porta to note */
            if (st->v[c].porta && st->v[c].tgt_period && st->tick) {
                int d = q;
                int np = st->v[c].period;
                if (e == 0x1 || (e == 0x3 && np > st->v[c].tgt_period)) {
                    np -= d; if (np < st->v[c].tgt_period) np = st->v[c].tgt_period;
                } else {
                    np += d; if (np > st->v[c].tgt_period) np = st->v[c].tgt_period;
                }
                st->v[c].period = (int16_t)np;
                st->v[c].inc = period_inc(np);
            }
            if (e == 3) goto volslide;
            break;
        case 0x4: /* vibrato */
            st->v[c].vibpos = (int8_t)((st->v[c].vibpos + (q & 0x0F)) & 31);
            if (st->v[c].period) {
                int mod = sin32[(unsigned)st->v[c].vibpos & 31] * (q >> 4) / 256;
                st->v[c].inc = period_inc(st->v[c].period + mod);
            }
            break;
        case 0x5: /* porta + volume slide (porta speed from last 3xy) */
            if (st->v[c].porta && st->v[c].tgt_period && st->tick) {
                int np = st->v[c].period;
                if (np > st->v[c].tgt_period) {
                    np -= st->v[c].porta_spd;
                    if (np < st->v[c].tgt_period) np = st->v[c].tgt_period;
                } else {
                    np += st->v[c].porta_spd;
                    if (np > st->v[c].tgt_period) np = st->v[c].tgt_period;
                }
                st->v[c].period = (int16_t)np;
                st->v[c].inc = period_inc(np);
            }
            goto volslide;
        case 0x6: /* vibrato + volume slide (speeds from last 4xy) */
            st->v[c].vibpos = (int8_t)((st->v[c].vibpos +
                                        (st->v[c].porta_spd & 0x0F)) & 31);
            if (st->v[c].period) {
                int mod = sin32[(unsigned)st->v[c].vibpos & 31] *
                          (st->v[c].porta_spd >> 4) / 256;
                st->v[c].inc = period_inc(st->v[c].period + mod);
            }
            goto volslide;
        volslide:
            if (st->tick) {
                int up = (q >> 4), dn = (q & 0x0F);
                int v = st->v[c].vol + up - dn;
                st->v[c].vol = (uint8_t)CLAMP64(v);
            }
            break;
        case 0x9: /* sample offset (row start) */
            if (st->tick == 0) st->v[c].pos = (uint32_t)q << 16;
            break;
        case 0xA: /* volume slide */
            if (st->tick) {
                int up = (q >> 4), dn = (q & 0x0F);
                int v = st->v[c].vol + up - dn;
                st->v[c].vol = (uint8_t)CLAMP64(v);
            }
            break;
        case 0xC: /* set volume */
            if (st->tick == 0 && q <= 64) st->v[c].vol = (uint8_t)q;
            break;
        case 0xE:
            switch (q >> 4) {
            case 1: if (st->tick) { int np = st->v[c].period - (q & 0x0F);
                        if (np >= 32) { st->v[c].period = (int16_t)np;
                                        st->v[c].inc = period_inc(np); } } break;
            case 2: if (st->tick) { int np = st->v[c].period + (q & 0x0F);
                        st->v[c].period = (int16_t)np;
                        st->v[c].inc = period_inc(np); } break;
            case 5: /* retrigger + volume slide */
                if (st->tick && st->v[c].period &&
                    st->tick % ((q >> 4) ? (q >> 4) : 1) == 0) {
                    st->v[c].pos = 0;
                    st->v[c].inc = period_inc(st->v[c].period);
                }
                if (st->tick) {
                    int v = st->v[c].vol + ((q & 0x0F) >= 8 ? 0 : (q & 0x0F))
                            - ((q & 0x0F) >= 8 ? 0 : 0);
                    if ((q & 0x0F) >= 8) v -= (q & 0x0F) - 8;
                    st->v[c].vol = (uint8_t)CLAMP64(v);
                }
                break;
            case 9: /* note cut */
                if (st->tick == (unsigned)(q & 0x0F)) st->v[c].cut = 1;
                break;
            case 0xA: if (st->tick) { int v = st->v[c].vol + (q & 0x0F);
                        st->v[c].vol = (uint8_t)CLAMP64(v); } break;
            case 0xB: if (st->tick) { int v = st->v[c].vol - (q & 0x0F);
                        st->v[c].vol = (uint8_t)CLAMP64(v); } break;
            default: break;
            }
            break;
        default: break;
        }
    }

    /* global effects on this row's channels (B/D/F handled at tick 0) */
    if (st->tick == 0) {
        for (int c = 0; c < (int)m->nch; c++) {
            int p, ins, ef, pr;
            rd_entry(m, pat, (int)st->row, c, &p, &ins, &ef, &pr);
            if (ef == 0x0B) { st->order = (unsigned)pr; if (st->order >= m->nseq) st->order = 0; }
            else if (ef == 0x0D) { st->row = (unsigned)(pr >> 4) * 10 + (pr & 0x0F); }
            else if (ef == 0x0F) {
                if (pr && pr < 0x20) st->speed = (unsigned)pr;
                else if (pr >= 0x20) st->tempo = (unsigned)pr;
            }
        }
    }

    st->tick++;
    if (st->tick >= st->speed) { st->tick = 0; st->row++; }
}

void ll_mod_mix(ll_mod_state *st, int16_t *dst, size_t frames)
{
    const ll_mod *m = st->m;
    uint32_t tick_len = (st->tempo * 44100u) / 6250u; /* tempo*7.056 fr */
    if (tick_len < 1) tick_len = 1;
    for (size_t i = 0; i < frames; i++) {
        if (st->subtick == 0) {
            if (st->end) { dst[2*i] = dst[2*i+1] = 0; continue; }
            do_tick(st);
        }
        for (int c = 0; c < (int)m->nch; c++) {
            int sidx = st->v[c].smp;
            if (!st->v[c].act || st->v[c].cut || !st->v[c].period ||
                sidx <= 0 || sidx > m->nsmp)
                continue;
            const ll_mod_smp *s = &m->smp[sidx - 1];
            if (!s->dlen || !st->v[c].inc) continue;
            uint32_t pos = st->v[c].pos >> 16;
            if (pos >= s->dlen) {
                if (s->loop_len > 2 && s->loop_start < s->dlen) {
                    st->v[c].pos = (uint32_t)s->loop_start << 16;
                    pos = s->loop_start;
                } else { st->v[c].act = 0; continue; }
            }
            if (pos >= s->dlen) continue;
            int samp = (int)(signed char)s->data[pos] * 256; /* s8 -> s16 */
            int vol = st->v[c].vol * st->gvol;               /* 0..4096 */
            int v = samp * vol / 4096;
            /* saturating add (never signed-overflow; CI runs under UBSan) */
            if (c & 1) {
                int a = dst[2*i+1] + v;
                dst[2*i+1] = (int16_t)(a > 32767 ? 32767 :
                                       a < -32768 ? -32768 : a);
            } else {
                int a = dst[2*i] + v;
                dst[2*i] = (int16_t)(a > 32767 ? 32767 :
                                     a < -32768 ? -32768 : a);
            }
            st->v[c].pos += st->v[c].inc;
        }
        if (++st->subtick >= tick_len) st->subtick = 0;
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
