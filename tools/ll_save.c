/* tools/ll_save.c - CLI for src/ll_save.c: dump + byte-exact round-trip
 * of the shipped STATUS.LEO / LONGSOFT.CFG (the originals are never
 * modified: the patched copy for the VM drive letter is written to OUT).
 *
 *   ll_save status FILE                 decode dump
 *   ll_save status FILE --roundtrip OUT re-write (must cmp byte-exact)
 *   ll_save cfg FILE [--drive D --out OUT]  show / patch drive byte0
 */
#include "ll_save.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "usage: ll_save status FILE [--roundtrip OUT]\n"
                        "       ll_save cfg FILE [--drive D] [--out OUT]\n");
        return 2;
    }
    const char *what = argv[1];
    const char *path = argv[2];

    if (strcmp(what, "status") == 0) {
        ll_status st;
        if (ll_status_load(path, &st)) {
            fprintf(stderr, "ll_save: cannot load %s (need exactly %d B)\n",
                    path, LL_STATUS_WORDS * 4);
            return 1;
        }
        ll_status_dump(&st, path);
        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "--roundtrip") == 0 && i + 1 < argc) {
                if (ll_status_save(argv[++i], &st)) {
                    fprintf(stderr, "ll_save: cannot write %s\n", argv[i]);
                    return 1;
                }
                printf("ll_save: wrote %s (%d B, raw words verbatim)\n",
                       argv[i], LL_STATUS_WORDS * 4);
            }
        }
        return 0;
    }
    if (strcmp(what, "cfg") == 0) {
        ll_cfg c;
        if (ll_cfg_load(path, &c)) {
            fprintf(stderr, "ll_save: cannot load %s (need exactly %d B)\n",
                    path, LL_CFG_SIZE);
            return 1;
        }
        int dr = ll_cfg_drive(&c);
        printf("ll_save: %s: %d B, byte0=%02x drive='%c' (FUN_0005bf10:"
               " data drive)\n", path, LL_CFG_SIZE, c.b[0],
               dr ? dr : '?');
        printf("ll_save: header:");
        for (int i = 0; i < 16; i++)
            printf(" %02x", c.b[i]);
        printf("  (rest: %d B)\n", LL_CFG_SIZE - 16);
        char drive = 0;
        const char *out = NULL;
        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "--drive") == 0 && i + 1 < argc)
                drive = argv[++i][0];
            else if (strcmp(argv[i], "--out") == 0 && i + 1 < argc)
                out = argv[++i];
        }
        if (drive)
            ll_cfg_set_drive(&c, drive);
        if (out) {
            if (ll_cfg_save(out, &c)) {
                fprintf(stderr, "ll_save: cannot write %s\n", out);
                return 1;
            }
            printf("ll_save: wrote copy %s (%d B)\n", out, LL_CFG_SIZE);
        }
        return 0;
    }
    fprintf(stderr, "ll_save: unknown subcommand '%s'\n", what);
    return 2;
}
