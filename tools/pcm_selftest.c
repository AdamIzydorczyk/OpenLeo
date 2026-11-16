/* pcm_selftest.c - decode a PCM payload and print its geometry + the
 * plane FNV-1a-64 for tools/verify_pcm.py. usage: pcm_selftest FILE|'-' NAME */
#include "ll_pcm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "usage: %s FILE|'-' NAME\n", argv[0]);
        return 2;
    }
    FILE *f = strcmp(argv[1], "-") ? fopen(argv[1], "rb") : stdin;
    if (!f) {
        fprintf(stderr, "cannot open %s\n", argv[1]);
        return 2;
    }
    size_t cap = 1 << 16, len = 0;
    unsigned char *raw = malloc(cap);
    if (!raw)
        return 2;
    size_t got;
    while ((got = fread(raw + len, 1, cap - len, f)) > 0) {
        len += got;
        if (len == cap) {
            cap *= 2;
            unsigned char *nb = realloc(raw, cap);
            if (!nb) {
                free(raw);
                return 2;
            }
            raw = nb;
        }
    }
    if (f != stdin)
        fclose(f);

    ll_pcm p;
    if (ll_pcm_decode(raw, len, &p)) {
        printf("decode %s FAILED\n", argv[2]);
        free(raw);
        return 1;
    }
    free(raw);
    size_t painted = 0;
    for (size_t i = 0; i < (size_t)p.w * p.h; i++)
        painted += p.op[i];
    printf("decode %s strips=%u h=%u tile=%u groups=%u contiguous=%d "
           "painted=%zu fnv=%016llx\n", argv[2], p.nstrips, p.h, p.tile,
           p.groups, p.tiles_contiguous, painted,
           (unsigned long long)ll_pcm_fnv(&p));
    ll_pcm_release(&p);
    return 0;
}
