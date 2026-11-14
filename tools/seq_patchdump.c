/* seq_patchdump.c - dump the decoded frame-patch rows of every entry blob
 * in a .SEQ payload with src/ll_seq.c in the exact text format of
 * tools/seq_patch.py so make verify-seq can diff them.
 *
 * Format per entry blob:  patch len L rows R / prow <gap> <toks...> / pend
 * tokens: copy = c<hex>, fill = f<val2>.<cnt>; empty row tok = '-',
 * gap -1 = empty row. Decode failure prints "patch len L ... pend FAIL". */
#include <stdio.h>
#include <stdlib.h>

#include "ll_seq.h"

static void rownoop(void *ctx, int row, int gap,
                    const ll_seq_patch_op *ops, int nops)
{
    (void)ctx; (void)row; (void)gap; (void)ops; (void)nops;
}

static void rowprint(void *ctx, int row, int gap,
                     const ll_seq_patch_op *ops, int nops)
{
    (void)ctx; (void)row;
    printf("prow %d", gap);
    if (nops == 0)
        printf(" -");
    for (int i = 0; i < nops; i++) {
        const ll_seq_patch_op *o = &ops[i];
        if (o->kind == 0) {
            printf(" c");
            for (int k = 0; k < o->cnt; k++) printf("%02x", o->b[k]);
        } else {
            printf(" f%02x.%d", o->val, o->cnt);
        }
    }
    putchar('\n');
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: seq_patchdump FILE.SEQ [more.SEQ ...]\n");
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
        for (uint32_t i = 0; i < s.count; i++) {
            const ll_seq_elem *e = &s.els[i];
            for (uint32_t j = 0; j < e->arr_n; j++) {
                const ll_seq_ent *t = &s.ents[e->arr_first + j];
                const unsigned char *blob = s.blob + t->off;
                int r = ll_seq_patch_walk(blob, t->len, rownoop, NULL);
                if (r < 0) {
                    printf("patch len %u pend FAIL\n", t->len);
                    rc = 1;
                } else {
                    printf("patch len %u rows %d\n", t->len, r);
                    ll_seq_patch_walk(blob, t->len, rowprint, NULL);
                    printf("pend\n");
                }
            }
        }
        ll_seq_free(&s);
        free(data);
    }
    return rc;
}
