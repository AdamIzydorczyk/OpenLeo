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
    /* engine window height: what the blitter can actually show at rest
     * (blit height is clamped to the load-window row count, hdr u16@2) */
    unsigned nh = p.win_rows && p.win_rows < p.rows ? p.win_rows : p.rows;
    if (nh > 200)
        nh = 200;
    unsigned char *view = malloc((size_t)nh * 400);
    if (view) {
        ll_pcm_view(&p, 0, nh, view);
        h = 0xCBF29CE484222325ull;
        for (size_t i = 0; i < (size_t)nh * 400; i++) {
            h ^= view[i];
            h *= 0x100000001B3ull;
        }
        printf("view %s h=%u fnv=%016llx\n", argv[2], nh, h);
        free(view);
    }
    ll_pcm_release(&p);
    return 0;
}
