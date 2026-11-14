/* llrs_selftest.c - prints every used slot of each container in the same
 * format as `python3 tools/llrs_unpack.py list FILE`, so
 *   diff <(./llrs_selftest DIR FILE...) <(python3 tools/llrs_unpack.py list ...)
 * is the equivalence proof for src/llrs.c. Also walks nested index blocks
 * (ANIMACJE/ETAPY level slots) and prints their records. */
#include "../src/llrs.h"

#include <stdio.h>
#include <string.h>

static void dump(llrs *c, const char *path)
{
    if (llrs_open(c, path)) {
        fprintf(stderr, "llrs_open failed: %s\n", path);
        return;
    }
    for (unsigned k = 0; k < c->cap; k++) {
        const llrs_slot *s = &c->slots[k];
        if (!s->id)
            continue;
        printf("%8d  id=%u  off=0x%x  size=0x%x  %s\n", k, s->id, s->off,
               s->size, s->name);
    }
}

static void dump_block(llrs *c, const char *path, const char *slotname)
{
    if (llrs_open(c, path)) {
        fprintf(stderr, "llrs_open failed: %s\n", path);
        return;
    }
    const llrs_slot *s = llrs_find(c, slotname);
    if (!s) {
        fprintf(stderr, "no slot %s in %s\n", slotname, path);
        return;
    }
    llrs_slot *recs = NULL;
    unsigned n = 0;
    if (llrs_block(c, s, &recs, &n)) {
        fprintf(stderr, "%s/%s: not an index block\n", path, slotname);
        return;
    }
    printf("# %s / %s: %u records\n", path, slotname, n);
    for (unsigned k = 0; k < n; k++)
        if (recs[k].id)
            printf("%8u  id=%u  off=0x%x  size=0x%x  %s\n", k, recs[k].id,
                   recs[k].off, recs[k].size, recs[k].name);
    llrs_block_free(recs);
}

int main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "--block") == 0) {
        if (argc != 4) {
            fprintf(stderr, "usage: %s --block FILE SLOT\n", argv[0]);
            return 2;
        }
        llrs c;
        dump_block(&c, argv[2], argv[3]);
        llrs_close(&c);
        return 0;
    }
    if (argc < 2) {
        fprintf(stderr, "usage: %s FILE...\n", argv[0]);
        return 2;
    }
    for (int i = 1; i < argc; i++) {
        llrs c;
        dump(&c, argv[i]);
        llrs_close(&c);
    }
    return 0;
}
