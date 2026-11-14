/* pcx_selftest.c - decode a raw PCX payload to PPM (oracle: tools/pcx_dump.py). */
#include "../ported/ll_pcx.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s PAYLOAD.pcx OUT.ppm\n", argv[0]);
        return 2;
    }
    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        perror(argv[1]);
        return 1;
    }
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char *buf = (unsigned char *)malloc((size_t)n);
    if (!buf || fread(buf, 1, (size_t)n, f) != (size_t)n) {
        fprintf(stderr, "short read\n");
        return 1;
    }
    fclose(f);
    ll_pcx p;
    if (ll_pcx_decode(buf, (size_t)n, &p)) {
        fprintf(stderr, "ll_pcx_decode failed\n");
        return 1;
    }
    FILE *o = fopen(argv[2], "wb");
    if (!o) {
        perror(argv[2]);
        return 1;
    }
    fprintf(o, "P6\n%u %u\n255\n", p.w, p.h);
    for (unsigned i = 0; i < p.w * p.h; i++) {
        unsigned char v = p.idx[i];
        fputc(p.pal[3 * v], o);
        fputc(p.pal[3 * v + 1], o);
        fputc(p.pal[3 * v + 2], o);
    }
    fclose(o);
    ll_pcx_free(&p);
    free(buf);
    return 0;
}
