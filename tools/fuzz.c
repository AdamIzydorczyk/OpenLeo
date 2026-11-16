/* fuzz.c - seeded in-process parser fuzzer (tips T-25).
 *
 *   ./fuzz DATA_DIR SEED ITERS [TMPDIR]     (build with ASan+UBSan)
 *
 * Mutates REAL game payloads (not random noise): the corpus is built at
 * startup from the real containers/streams under DATA_DIR -
 *   llrs      : first 2 MB of the ANIMACJE / ETAPY / SOUND / LEVELDES
 *               containers (index + data head; range guards see the
 *               truncation),
 *   .IS       : up to three stream files <= 8 MB (decode + linearize),
 *   pcx/seq/pcm/mod : payload records extracted via llrs_read from the
 *               real containers (by extension),
 * then applies 1..6 byte-level mutations (flip / zero / 0xff / truncate)
 * and re-parses. Every run must return cleanly: no crash, no leak
 * (ASan), no UB. accepted/rejected counts prove the corpus actually
 * reaches the decoders.
 *
 * Not fuzzed: ll_save (fixed-size raw word copy, no parsing) and the
 * .des/.MNU text scanners (ll_level wiring; a text grammar fuzz is its
 * own project - see NEXT_STEPS).
 */
#include "fs_ci.h"
#include "is_stream.h"
#include "ll_mod.h"
#include "ll_pcx.h"
#include "ll_pcm.h"
#include "ll_seq.h"
#include "llrs.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { T_LLRS, T_IS, T_PCX, T_SEQ, T_PCM, T_MOD, T_NKIND };
static const char *kindname[] = { "llrs", "is", "pcx", "seq", "pcm", "mod" };

typedef struct {
    unsigned char *b;
    size_t         len;
    int            kind;
} blob_t;

static blob_t corpus[512];
static int    ncorpus;
static long   accepted[T_NKIND], rejected[T_NKIND], iters_by_kind[T_NKIND];
static char   tmpdir[1024] = "/tmp/opencode";

/* deterministic prng: xorshift64* */
static unsigned long long rs;
static unsigned long long rnd(void)
{
    rs ^= rs >> 12;
    rs ^= rs << 25;
    rs ^= rs >> 27;
    return rs * 2685821657736338717ull;
}
static size_t rndn(size_t n) { return n ? (size_t)(rnd() % n) : 0; }

static void addcorpus(const unsigned char *b, size_t len, int kind)
{
    if (ncorpus >= (int)(sizeof corpus / sizeof corpus[0]) || !b || len < 4)
        return;
    corpus[ncorpus] = (blob_t){ (unsigned char *)malloc(len), len, kind };
    memcpy(corpus[ncorpus].b, b, len);
    ncorpus++;
}

static unsigned char *slurp(const char *path, size_t *len, size_t cap)
{
    FILE *f = fopen(path, "rb");
    if (!f)
        return NULL;
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (n <= 0) {
        fclose(f);
        return NULL;
    }
    /* cap = truncate, not reject: for llrs the first megabytes (index +
     * data head) are the interesting fuzz corpus of a 400 MB container;
     * for IS files it bounds the per-iteration decode cost. */
    size_t want = (size_t)n < cap ? (size_t)n : cap;
    unsigned char *b = (unsigned char *)malloc(want);
    if (fread(b, 1, want, f) != want) {
        free(b);
        fclose(f);
        return NULL;
    }
    fclose(f);
    *len = want;
    return b;
}

static int has_ext(const char *name, const char *ext)
{
    const char *dot = strrchr(name, '.');
    return dot && !strcasecmp(dot + 1, ext);
}

static void add_is_files(const char *data)
{
    DIR *d = opendir(data);
    if (!d)
        return;
    struct dirent *de;
    int added = 0;
    while ((de = readdir(d)) && added < 3) {
        if (!has_ext(de->d_name, "IS"))
            continue;
        char path[2048];
        snprintf(path, sizeof path, "%s/%s", data, de->d_name);
        size_t len;
        unsigned char *b = slurp(path, &len, 8u << 20); /* <= 8 MB */
        if (b) {
            addcorpus(b, len, T_IS);
            free(b);
            added++;
        }
    }
    closedir(d);
}

static void feed_containers(const char *data)
{
    static const char *cont[] = { "ANIMACJE", "ETAPY", "SOUND", "LEVELDES" };
    for (unsigned c = 0; c < sizeof cont / sizeof *cont; c++) {
        char path[2048];
        snprintf(path, sizeof path, "%s/%s", data, cont[c]);
        /* 1) head bytes for the llrs target itself */
        size_t len;
        unsigned char *head = slurp(path, &len, 2u << 20);
        if (head) {
            addcorpus(head, len, T_LLRS);
            free(head);
        }
        /* 2) real payloads for the sub-format targets. Most records live
         * INSIDE the id=2 index blocks (verify_pcx.py does the same). */
        llrs x;
        if (llrs_open(&x, path) != 0)
            continue;
        int taken = 0;
        for (unsigned k = 0; k < x.cap && taken < 48; k++) {
            const llrs_slot *s = &x.slots[k];
            if (!s->id)
                continue;
            llrs_slot *recs = NULL;
            unsigned nrec = 0;
            if (s->id == 2)
                llrs_block(&x, s, &recs, &nrec);   /* nested records */
            for (unsigned r = 0; r <= nrec && taken < 48; r++) {
                const llrs_slot *p = r < nrec ? &recs[r] : s;
                if (!p->id)
                    continue;
                int kind = -1;
                if (has_ext(p->name, "PCX"))  kind = T_PCX;
                else if (has_ext(p->name, "SEQ")) kind = T_SEQ;
                else if (has_ext(p->name, "PCM")) kind = T_PCM;
                else if (has_ext(p->name, "MOD")) kind = T_MOD;
                if (kind < 0 || p->size > (6u << 20))
                    continue;
                unsigned char *b = NULL;
                if (llrs_read(&x, p, &b) > 0) {
                    addcorpus(b, (size_t)p->size, kind);
                    free(b);
                    taken++;
                }
            }
            llrs_block_free(recs);
        }
        llrs_close(&x);
    }
}

static void write_tmp(const char *name, const unsigned char *b, size_t len)
{
    char path[1200];
    snprintf(path, sizeof path, "%s/%s", tmpdir, name);
    FILE *f = fopen(path, "wb");
    if (f) {
        fwrite(b, 1, len, f);
        fclose(f);
    }
}

static void tmppath(char *out, size_t n, const char *name)
{
    snprintf(out, n, "%s/%s", tmpdir, name);
}

static void noop_esc(void *u, const uint8_t *p, unsigned n)
{
    (void)u; (void)p; (void)n;
}

/* -------- target drivers: each returns 1 = parsed, 0 = cleanly
   rejected, crashes/UB abort the process (that is the point) ------- */

static int run_llrs(const unsigned char *b, size_t len)
{
    char path[1200];
    tmppath(path, sizeof path, "fuzz_llrs.bin");
    write_tmp("fuzz_llrs.bin", b, len);
    llrs c;
    if (llrs_open(&c, path) != 0)
        return 0;
    int n = 0;
    for (unsigned k = 0; k < c.cap && n < 6; k++)
        if (c.slots[k].id) {
            unsigned char *pl = NULL;
            if (llrs_read(&c, &c.slots[k], &pl) > 0) {
                free(pl);
                n++;
            }
        }
    llrs_close(&c);
    return 1;
}

static int run_is(const unsigned char *b, size_t len)
{
    char path[1200];
    tmppath(path, sizeof path, "fuzz_is.bin");
    write_tmp("fuzz_is.bin", b, len);
    is_stream s;
    if (is_open(&s, path) != 0)
        return 0;
    is_walk_audio(&s, noop_esc, NULL);
    uint8_t *lin = is_linearize(&s);
    free(lin);
    is_close(&s);
    return 1;
}

static int run_pcx(const unsigned char *b, size_t len)
{
    ll_pcx p;
    if (ll_pcx_decode(b, len, &p) != 0)
        return 0;
    ll_pcx_free(&p);
    return 1;
}

static void seq_rowcb(void *ctx, int row, int gap,
                      const ll_seq_patch_op *ops, int nops)
{
    (void)ctx; (void)row; (void)gap; (void)ops; (void)nops;
}

static int run_seq(const unsigned char *b, uint32_t len)
{
    ll_seq s;
    int ok = ll_seq_parse(b, len, &s) == 0;
    if (ok)
        ll_seq_free(&s);
    if (ll_seq_patch_walk(b, len, seq_rowcb, NULL) >= 0)
        ok = 1;
    if (ll_seq_patch_walk_codec(b, len, LL_SEQ_CODEC_D9B8, seq_rowcb,
                                NULL) >= 0)
        ok = 1;
    return ok;
}

static int run_pcm(const unsigned char *b, size_t len)
{
    ll_pcm p;
    if (ll_pcm_decode(b, len, &p) != 0)
        return 0;
    unsigned char dst[64 * 8];
    memset(dst, 0, sizeof dst);
    ll_pcm_blit(&p, dst, 64, 8, -5, -2, 3, 1);   /* wrap + clip paths */
    ll_pcm_blit(&p, dst, 64, 8, (int)p.w - 7, (int)p.h - 3, 0, 0);
    (void)ll_pcm_fnv(&p);
    ll_pcm_release(&p);
    return 1;
}

static int run_kind(int kind, unsigned char *buf, size_t len);

static int run_mod(const unsigned char *b, size_t len)
{
    ll_mod m;
    char err[96];
    if (ll_mod_open(&m, b, len, err, sizeof err) != 0)
        return 0;
    int16_t *pcm = NULL;
    size_t n = ll_mod_render(&m, 80, &pcm);   /* 80 ms of mixer output */
    free(pcm);
    ll_mod_close(&m);
    return n > 0;
}

static int run_kind(int kind, unsigned char *buf, size_t len)
{
    switch (kind) {
    case T_LLRS: return run_llrs(buf, len);
    case T_IS:   return run_is(buf, len);
    case T_PCX:  return run_pcx(buf, len);
    case T_SEQ:  return run_seq(buf, (uint32_t)len);
    case T_PCM:  return run_pcm(buf, len);
    default:     return run_mod(buf, len);
    }
}

int main(int argc, char **argv)
{
    if (argc < 4) {
        fprintf(stderr, "usage: %s DATA_DIR SEED ITERS [TMPDIR]\n", argv[0]);
        return 2;
    }
    rs = strtoull(argv[2], NULL, 10) * 2862933555777941757ull + 3037000507ull;
    long iters = atol(argv[3]);
    if (argc > 4)
        snprintf(tmpdir, sizeof tmpdir, "%s", argv[4]);

    add_is_files(argv[1]);
    feed_containers(argv[1]);
    if (ncorpus < 20) {
        fprintf(stderr, "fuzz: corpus too small (%d blobs) - is DATA_DIR ok?\n",
                ncorpus);
        return 2;
    }
    printf("fuzz: seed=%s corpus=%d blobs iters=%ld\n", argv[2], ncorpus,
           iters);

    /* PR-53: every corpus blob runs once UNMUTATED first. The decoders are
     * strict (a flipped byte in a multi-megabyte PCM layer breaks a row), so
     * a short random run can reject every mutant of a kind; the "corpus
     * NEVER parsed" guard below must test the corpus, not the dice. */
    for (int ci = 0; ci < ncorpus; ci++) {
        blob_t *c = &corpus[ci];
        unsigned char *buf = (unsigned char *)malloc(c->len + 64);
        memcpy(buf, c->b, c->len);
        int ok = run_kind(c->kind, buf, c->len);
        free(buf);
        iters_by_kind[c->kind]++;
        (ok ? accepted : rejected)[c->kind]++;
    }

    for (long it = 0; it < iters; it++) {
        /* pick a blob at random; its kind defines the target */
        int ci = (int)rndn((size_t)ncorpus);
        blob_t *c = &corpus[ci];
        unsigned char *buf = (unsigned char *)malloc(c->len + 64);
        memcpy(buf, c->b, c->len);
        size_t len = c->len;
        int k = 1 + (int)rndn(6);
        for (int m = 0; m < k; m++) {
            switch (rndn(4)) {
            case 0: if (len) buf[rndn(len)] ^= (unsigned char)(1u << rndn(8)); break;
            case 1: if (len) buf[rndn(len)] = 0x00; break;
            case 2: if (len) buf[rndn(len)] = 0xff; break;
            default: len = rndn(len); break;      /* truncate */
            }
        }
        int ok = run_kind(c->kind, buf, len);
        free(buf);
        iters_by_kind[c->kind]++;
        (ok ? accepted : rejected)[c->kind]++;
    }

    int any_accepted = 0;
    for (int t = 0; t < T_NKIND; t++) {
        printf("fuzz %-4s: iters=%-5ld accepted=%-5ld rejected=%ld\n",
               kindname[t], iters_by_kind[t], accepted[t], rejected[t]);
        if (accepted[t])
            any_accepted = 1;
        if (iters_by_kind[t] > 0 && accepted[t] == 0) {
            fprintf(stderr, "fuzz: %s corpus NEVER parsed - corpus broken\n",
                    kindname[t]);
            return 2;
        }
    }
    if (!any_accepted)
        return 2;
    printf("fuzz: clean (no crash/UB/leak reported by ASan)\n");
    return 0;
}
