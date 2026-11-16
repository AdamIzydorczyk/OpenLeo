#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "llrs.h"
static void walk(const llrs *c, const llrs_slot *s, int depth, const char *path)
{
    llrs_slot *r = NULL; unsigned n = 0;
    if (llrs_block(c, s, &r, &n)) return;
    for (unsigned i = 0; i < n; i++) {
        if (!r[i].name[0]) continue;
        char p[256]; snprintf(p, sizeof p, "%s/%s", path, r[i].name);
        printf("%d %s id=%d size=%d\n", depth, p, r[i].id, r[i].size);
        if (r[i].id == 2 && depth < 3) walk(c, &r[i], depth + 1, p);
    }
    llrs_block_free(r);
}
int main(int argc, char **argv)
{
    llrs c; if (llrs_open(&c, argv[1])) return 1;
    for (int i = 0; i < c.cap; i++) {
        if (c.slots[i].id == 2 && c.slots[i].name[0]) {
            printf("0 %s\n", c.slots[i].name);
            walk(&c, &c.slots[i], 1, c.slots[i].name);
        }
    }
    return 0;
}
