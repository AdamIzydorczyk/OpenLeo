/* main.c - shared native entry for BOTH the linux and windows targets: the
 * SDL2 runner for Lew Leon .IS data, plus a headless --ppm mode that decodes
 * and dumps a single frame with no SDL/display/audio. --ppm is the
 * byte-exactness oracle used by every CI check (vs tools/is_decode.py).
 * Scene chaining (M3): --then FILE.IS plays scenes back-to-back; ESC mid
 * scene skips it and advances to the NEXT scene (never freezes, never
 * kills the app). --menu DATA_DIR shows the title menu after the last
 * scene (background from the original ANIMACJE container when present);
 * --demo [DATA_DIR] plays the original boot scene chain then the menu.
 * This file is platform-neutral: the only per-variant differences are the
 * Makefile (compiler + SDL path) and the -DPLATFORM_* knobs in ll_config.h.
 * Usage: ll_cutscene FILE.IS [--ppm N OUT.ppm] [--from N] [--skip-at N]
 *          [--no-audio] [--then FILE.IS] [--menu DATA_DIR] [--demo [DATA_DIR]]
 */
#include "ll_config.h"
#include "is_stream.h"
#include "ll_level.h"
#include "ll_menu.h"
#include "ll_pcx.h"
#include "llrs.h"
#include "platform_sdl/is_player.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> /* strcasecmp for container record names */

#define MAX_SCENES 16

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

/* read ANIMACJE/MENU/OPTIONS.PCX payload (malloc'd) or NULL */
static unsigned char *menu_bg(const char *data, unsigned *len)
{
    char p[4096];
    llrs a;
    snprintf(p, sizeof p, "%s/ANIMACJE", data);
    if (llrs_open(&a, p))
        return NULL;
    unsigned char *out = NULL;
    const llrs_slot *menu = llrs_find(&a, "MENU");
    if (menu && menu->id == 2) {
        llrs_slot *recs = NULL;
        unsigned n = 0;
        if (llrs_block(&a, menu, &recs, &n) == 0) {
            for (unsigned k = 0; k < n; k++) {
                const char *nm = recs[k].name;
                if (recs[k].id == 1 &&
                    (strcmp(nm, "OPTIONS.PCX") == 0 ||
                     strcmp(nm, "options.pcx") == 0)) {
                    long sz = llrs_read(&a, &recs[k], &out);
                    if (sz < 0)
                        out = NULL;
                    else
                        *len = (unsigned)sz;
                    break;
                }
            }
            llrs_block_free(recs);
        }
    }
    llrs_close(&a);
    return out;
}

/* read ANIMACJE/MENU title-screen .SEQ overlays (X, CNIE, BERLO) in
 * container order; *nanim = count found. Caller frees payload + array. */
static ll_menu_anim *menu_anims(const char *data, int *nanim)
{
    static const char *want[] = { "X.SEQ", "CNIE.SEQ", "BERLO.SEQ" };
    char p[4096];
    llrs a;
    *nanim = 0;
    snprintf(p, sizeof p, "%s/ANIMACJE", data);
    if (llrs_open(&a, p))
        return NULL;
    ll_menu_anim *out = NULL;
    const llrs_slot *menu = llrs_find(&a, "MENU");
    if (menu && menu->id == 2) {
        llrs_slot *recs = NULL;
        unsigned n = 0;
        if (llrs_block(&a, menu, &recs, &n) == 0) {
            out = calloc(n ? n : 1, sizeof *out);
            for (unsigned k = 0; k < n && out; k++) {
                if (recs[k].id != 1)
                    continue;
                for (unsigned w = 0; w < sizeof want / sizeof want[0]; w++) {
                    if (strcasecmp(recs[k].name, want[w]) != 0)
                        continue;
                    unsigned char *buf = NULL;
                    long sz = llrs_read(&a, &recs[k], &buf);
                    if (sz > 0) {
                        out[*nanim].seq = buf;
                        out[*nanim].len = (unsigned)sz;
                        out[*nanim].period_ms = 100;
                        (*nanim)++;
                    }
                    break;
                }
            }
            llrs_block_free(recs);
        }
    }
    llrs_close(&a);
    return out;
}

static void run_menu(const char *data)
{
    unsigned blen = 0;
    unsigned char *bg = menu_bg(data, &blen);
    int nanim = 0;
    ll_menu_anim *anims = menu_anims(data, &nanim);
    static const char *items[] = { "NEW GAME  (RAN.01)",
                                   "CREDITS   (AUTORZY.IS)",
                                   "QUIT" };
    const char *aut = getenv("LEO_MENU_AUTO"); /* "1" = pick NEW GAME, quit */
    int auto_i = aut ? atoi(aut) : 0;
    const char *dem = getenv("LEO_DEMO");      /* demo: auto-pick NEW GAME */
    for (;;) {
        int sel;
        if (auto_i) {
            sel = auto_i - 1;
            printf("menu: LEO_MENU_AUTO -> picked item %d\n", sel);
        } else if (dem && !auto_i) {
            sel = 0;
            printf("menu: demo auto-picks NEW GAME (title shown for %.1f s)\n",
                   (atof(dem)));
            SDL_Delay((Uint32)(atof(dem) * 1000.0));
        } else {
            sel = ll_menu_open(bg, blen, items, 3, anims, nanim);
        }
        if (sel < 0)
            break;
        if (sel == 0) {
            /* TUNGA.11 has a tlo= PCX background on the CD; RAN.01's
             * background is .PCM-only (layer decode pending, PLAN.md). */
            ll_level_show(data, "TUNGA.11", 8.0, 0);
            break;
        } else if (sel == 1) {
            if (auto_i)
                break; /* chained runs do not re-play FMVs twice */
            char p[4096];
            snprintf(p, sizeof p, "%s/AUTORZY.IS", data);
            is_player_run(p, 1, 0, 0);
        } else {
            break;
        }
    }
    for (int i = 0; i < nanim; i++)
        free((void *)anims[i].seq);
    free(anims);
    free(bg);
}

int main(int argc, char **argv)
{
    const char *file = NULL;
    const char *then[MAX_SCENES];
    int nthen = 0;
    const char *menu_data = NULL, *demo_data = NULL;
    ppm_job ppm = { 0, NULL, 0 };
    unsigned from = 0;
    unsigned skip_at = 0;
    int audio = 1, demo = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--ppm") == 0 && i + 2 < argc) {
            ppm.want = (unsigned)atoi(argv[++i]);
            ppm.out = argv[++i];
        } else if (strcmp(argv[i], "--from") == 0 && i + 1 < argc) {
            from = (unsigned)atoi(argv[++i]);
        } else if (strcmp(argv[i], "--skip-at") == 0 && i + 1 < argc) {
            skip_at = (unsigned)atoi(argv[++i]);
        } else if (strcmp(argv[i], "--no-audio") == 0) {
            audio = 0;
        } else if (strcmp(argv[i], "--then") == 0 && i + 1 < argc) {
            if (nthen < MAX_SCENES)
                then[nthen++] = argv[++i];
        } else if (strcmp(argv[i], "--menu") == 0 && i + 1 < argc) {
            menu_data = argv[++i];
        } else if (strcmp(argv[i], "--demo") == 0) {
            demo = 1;
            if (i + 1 < argc && argv[i + 1][0] != '-')
                demo_data = argv[++i];
        } else {
            file = argv[i];
        }
    }

    if (demo) {
        /* original boot chain (LOGO/INTRO section of ll_main) */
        const char *d = demo_data ? demo_data : ".";
        static const char *boot[] = { "/LOGO.IS", "/PRAWA.IS", "/INTRO_P.IS" };
        unsigned dsf = 0;
        const char *e = getenv("LEO_DEMO_SKIP_FRAMES"); /* CI: fast-forward */
        if (e)
            dsf = (unsigned)atoi(e);
        for (unsigned k = 0; k < sizeof boot / sizeof *boot; k++) {
            char *p = (char *)malloc(strlen(d) + 32);
            sprintf(p, "%s%s", d, boot[k]);
            int rc = is_player_run(p, audio, 0, dsf);
            free(p);
            if (rc != 0 || dsf)
                break; /* skipped or error: advance straight to menu */
        }
        run_menu(d);
        return 0;
    }

    if (!file) {
        fprintf(stderr,
                "usage: %s FILE.IS [--ppm N OUT.ppm] [--from N] [--no-audio]"
                " [--then FILE.IS] [--menu DATA_DIR] [--demo [DATA_DIR]]\n",
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

    /* scene chain: first file, then --then files; a skip (rc=2) breaks out */
    int rc = is_player_run(file, audio, from, skip_at);
    for (int k = 0; k < nthen && rc == 0; k++)
        rc = is_player_run(then[k], audio, 0, 0);
    if (rc == 2)
        rc = 0; /* skipped forward == normal handoff in chained mode */
    if (menu_data)
        run_menu(menu_data);
    return rc;
}
