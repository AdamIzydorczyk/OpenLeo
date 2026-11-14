/* mod_selftest.c - CI harness for ported/ll_mod.c (MOD player).
 *
 * usage: mod_selftest FILE.MOD [MS] [OUT.WAV]
 * Prints the parse summary (standard MOD layout with the whole numbered
 * pattern set and contiguous samples) and an FNV-1a-64 hash of the
 * first MS ms (default 3000) of deterministic 44.1 kHz stereo render.
 * The hash must match byte-exact between linux and wine runs.
 * OUT.WAV writes the render for human listening.
 */
#include "ll_mod.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s FILE.MOD [MS] [OUT.WAV]\n", argv[0]);
        return 2;
    }
    unsigned ms = argc > 2 ? (unsigned)atoi(argv[2]) : 3000;
    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        fprintf(stderr, "open: %s\n", argv[1]);
        return 1;
    }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char *img = (unsigned char *)malloc((size_t)len);
    if (!img || fread(img, 1, (size_t)len, f) != (size_t)len) {
        fprintf(stderr, "read failed\n");
        return 1;
    }
    fclose(f);

    ll_mod m;
    char err[128] = "";
    if (ll_mod_open(&m, img, (size_t)len, err, sizeof err) != 0) {
        printf("PARSE-FAIL %s: %s\n", argv[1], err);
        return 1;
    }
    const char *bn = strrchr(argv[1], '/');
    bn = bn ? bn + 1 : argv[1];
    printf("parse %s: '%s' id=%s %uch nseq=%u npat=%d nsmp=%d\n",
           bn, m.title, m.id, m.nch, m.nseq, m.npat_stored, m.nsmp);
    for (int i = 0; i < m.nsmp; i++)
        if (m.smp[i].len)
            printf("  smp%02d len=%u dlen=%u loop=%u/%u vol=%u fine=%u\n",
                   i + 1, m.smp[i].len, m.smp[i].dlen, m.smp[i].loop_start,
                   m.smp[i].loop_len, m.smp[i].vol, m.smp[i].fine);

    int16_t *buf = NULL;
    size_t bytes = ll_mod_render(&m, ms, &buf);
    if (!buf) {
        printf("RENDER-FAIL\n");
        return 1;
    }
    /* FNV-1a-64 over the interleaved sample words */
    uint64_t hv = 1469598103934665603ull;
    const uint16_t *w = (const uint16_t *)buf;
    size_t n = bytes / 2;
    int peak = 0;
    long nz = -1;
    for (size_t i = 0; i < n; i++) {
        hv ^= w[i];
        hv *= 0x100000001B3ull;
        int s = (int)(int16_t)w[i];
        if (s < 0) s = -s;
        if (s > peak) peak = s;
        if (nz < 0 && w[i]) nz = (long)i;
    }
    printf("render %ums frames=%zu fnv=%016llx peak=%d firstnz=%ld\n",
           ms, n / 2, (unsigned long long)hv, peak, nz);
    if (argc > 3) {
        FILE *w = fopen(argv[3], "wb");
        if (w) {
            uint32_t dlen = (uint32_t)bytes, rate = LL_MOD_RATE;
            uint32_t slen = dlen + 36;
            fwrite("RIFF", 1, 4, w); fwrite(&slen, 4, 1, w);
            fwrite("WAVEfmt ", 1, 8, w);
            uint32_t csz = 16; uint16_t fmt = 1, ch = 2, bits = 16;
            uint32_t brate = rate * 4, align = 4;
            fwrite(&csz, 4, 1, w); fwrite(&fmt, 2, 1, w); fwrite(&ch, 2, 1, w);
            fwrite(&rate, 4, 1, w); fwrite(&brate, 4, 1, w);
            fwrite(&align, 2, 1, w); fwrite(&bits, 2, 1, w);
            fwrite("data", 1, 4, w); fwrite(&dlen, 4, 1, w);
            fwrite(buf, 1, bytes, w);
            fclose(w);
            printf("wrote %s (%zu bytes)\n", argv[3], (size_t)bytes + 44);
        }
    }
    free(buf);
    ll_mod_close(&m);
    free(img);
    return 0;
}
