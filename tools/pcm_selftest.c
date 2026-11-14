/* pcm_selftest.c - decode a PCM payload and print geometry + plane
 * FNV-1a-64 for tools/verify_pcm.py. usage: pcm_selftest FILE|'-' NAME */
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
        return 1;
    }
    free(raw);
    unsigned long long h = 0xCBF29CE484222325ull;
    for (size_t i = 0; i < (size_t)p.rows * p.stride; i++) {
        h ^= p.plane[i];
        h *= 0x100000001B3ull;
    }
    printf("decode %s w=%u win=%u stride=%u rows=%u partial=%d fnv=%016llx\n",
           argv[2], p.w, p.win_rows, p.stride, p.rows, p.last_partial, h);
    ll_pcm_release(&p);
    return 0;
}
