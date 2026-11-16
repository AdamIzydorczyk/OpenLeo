/* seq_selftest.c - dump one .SEQ payload blob with ported/ll_seq.c in the
 * exact text format of tools/seq_parse.py so make verify-seq can diff them. */
#include <stdio.h>
#include <stdlib.h>

#include "ll_seq.h"

#define SX(v) ((int)(short)(v))

static void phex(const unsigned char *p, unsigned n)
{
    for (unsigned i = 0; i < n; i++) printf("%02x", p[i]);
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: seq_selftest FILE.SEQ [more.SEQ ...]\n");
        return 2;
    }
    int rc = 0;
    for (int a = 1; a < argc; a++) {
        FILE *f = fopen(argv[a], "rb");
        if (!f) { fprintf(stderr, "cannot open %s\n", argv[a]); return 2; }
        fseek(f, 0, SEEK_END);
        long flen = ftell(f);
        fseek(f, 0, SEEK_SET);
        unsigned char *data = malloc((size_t)flen ? (size_t)flen : 1);
        if (!data || fread(data, 1, (size_t)flen, f) != (size_t)flen) {
            fprintf(stderr, "read error %s\n", argv[a]);
            return 2;
        }
        fclose(f);

        ll_seq s;
        if (ll_seq_parse(data, (unsigned)flen, &s) != 0) {
            printf("PARSE-FAIL\n");
            free(data);
            rc = 1;
            continue;
        }

    printf("blob %u count %u\n", s.len, s.count);
    for (uint32_t i = 0; i < s.count; i++) {
        const ll_seq_elem *e = &s.els[i];
        printf("elem %u off %u w0 %04x b19 %d b15 %d b17 %d f0 %d f2 %d "
               "f4 %d f6 %d",
               i, e->off, e->w0, SX(e->b19), SX(e->b15), SX(e->b17),
               SX(e->f0), SX(e->f2), SX(e->f4), SX(e->f6));
        if (e->w0 & 4)
            printf(" f8 %d fA %d", SX(e->f8), SX(e->fA));
        if (e->w0 & 3) {
            int n = SX(e->b17);
            printf(" n %d", n);
            if (n > 0) {
                printf(" ml "); phex(s.blob + e->ml, (unsigned)n);
                printf(" mr "); phex(s.blob + e->mr, (unsigned)n);
            }
        }
        if (e->arr_present) printf(" arr %u", e->arr_n);
        putchar('\n');
        for (uint32_t j = 0; j < e->arr_n; j++) {
            const ll_seq_ent *t = &s.ents[e->arr_first + j];
            printf("ent %u len %u data ", j, t->len);
            phex(s.blob + t->off, t->len);
            putchar('\n');
        }
    }
    printf("end %u\n", s.consumed);
    ll_seq_free(&s);
    free(data);
    }
    return rc;
}
