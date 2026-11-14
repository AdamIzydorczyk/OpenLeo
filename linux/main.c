/* main.c - linux/ variant entry: native SDL2 runner for Lew Leon data.
 * Usage: ll_cutscene FILE.IS [--ppm N OUT.ppm] [--from N] [--no-audio]
 * --ppm mode is headless (no SDL): decodes and dumps frame N, used to
 * prove byte-exactness vs tools/is_decode.py.
 */
#include "../src/ll_config.h"
#include "../src/is_stream.h"
#include "../src/platform_sdl/is_player.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned want;
    const char *out;
    int found;
} ppm_job;

static void ppm_cb(void *u, unsigned idx, const is_stream *s)
{
    ppm_job *j = (ppm_job *)u;
    if (idx == j->want) {
        j->found = is_write_ppm(s, j->out) == 0;
    }
}

int main(int argc, char **argv)
{
    const char *file = NULL;
    ppm_job ppm = { 0, NULL, 0 };
    unsigned from = 0;
    int audio = 1;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--ppm") == 0 && i + 2 < argc) {
            ppm.want = (unsigned)atoi(argv[++i]);
            ppm.out = argv[++i];
        } else if (strcmp(argv[i], "--from") == 0 && i + 1 < argc) {
            from = (unsigned)atoi(argv[++i]);
        } else if (strcmp(argv[i], "--no-audio") == 0) {
            audio = 0;
        } else {
            file = argv[i];
        }
    }
    if (!file) {
        fprintf(stderr,
                "usage: %s FILE.IS [--ppm N OUT.ppm] [--from N] [--no-audio]\n",
                argv[0]);
        return 2;
    }

    if (ppm.out) {
        is_stream s;
        if (is_open(&s, file) != 0) {
            fprintf(stderr, "cannot open %s\n", file);
            return 1;
        }
        int rc = is_decode_all(&s, ppm_cb, &ppm, ppm.want + 1);
        printf("%s: frame %u -> %s (%s) rc=%d decoded=%u chunks=%u\n", file,
               ppm.want, ppm.out, ppm.found ? "ok" : "NOT REACHED", rc,
               s.st.frames, s.st.chunks);
        is_close(&s);
        return ppm.found ? 0 : 1;
    }
    return is_player_run(file, audio, from);
}
