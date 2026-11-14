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
#include "ll_save.h"
#include "llrs.h"
#include "platform_sdl/is_player.h"

#include <SDL2/SDL.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> /* strcasecmp for container record names */
#include <sys/stat.h> /* stat() for the DATA probe (containers are files) */

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

/* read ANIMACJE/MENU title-screen .SEQ overlays in container order;
 * *nanim = count found. Caller frees payload + array.  x/y = fixed
 * logical-canvas origins for the map menu (AUTORZY.SEQ @ (4,180) is the
 * real bottom-left corner word sprite); x=-1 = park on the selected
 * island (BERLO arrow).  RESTGRY/TAK/CNIE/X are the "restart gry?"
 * confirmation-dialog cursive sprites (VL OCR reads "restart gry?" and
 * "tak" on them; CNIE is the ornate cursive answer word, X the cross):
 * they compose over OPTIONS.PCX at their authored (-f0,-f2) origins on
 * the restart corner (reconstruction, PLAN 8b: the .MNU placement is a
 * CD data hole). */
static ll_menu_anim *menu_anims(const char *data, int *nanim)
{
    static const char *want[] = { "X.SEQ", "CNIE.SEQ", "BERLO.SEQ",
                                  "AUTORZY.SEQ", "RESTGRY.SEQ", "TAK.SEQ" };
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
                        out[*nanim].tag = want[w];
                        out[*nanim].x =
                            strcasecmp(want[w], "AUTORZY.SEQ") == 0 ? 4 : -1;
                        out[*nanim].y =
                            strcasecmp(want[w], "AUTORZY.SEQ") == 0 ? 180 : -1;
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

/*
 * DATA directory discovery (review P-08, tips T-17): the original EXE
 * found ANIMACJE/ next to itself; a modern launcher may start us from
 * anywhere. Search order: $LEO_DATA -> <dir of executable>/data ->
 * ./data -> . (the classic cwd default). Returns NULL when nothing
 * contains game data.
 */
/* ANIMACJE, ETAPY, SOUND, LEVELDES are single container FILES (LLRS
 * archives), not directories - existence check only. */
static int have_sub(const char *dir, const char *sub)
{
    const char *slash = (dir[0] && dir[strlen(dir) - 1] == '/') ? "" : "/";
    char probe[4096];
    snprintf(probe, sizeof probe, "%s%s%s", dir, slash, sub);
    struct stat st;
    return stat(probe, &st) == 0;
}

/* P-08 "did you mean" hint: an under- or mis-cased sibling directory
 * (data/animacje vs ANIMACJE) is a very common CD-rip accident. */
static void suggest_sub(const char *dir, const char *want)
{
    DIR *d = opendir(dir);
    if (!d) return;
    struct dirent *de;
    while ((de = readdir(d))) {
        if (de->d_name[0] == '.') continue;
        if (!strcasecmp(de->d_name, want)) {
            fprintf(stderr, " (closest match: %s/%s)\n", dir, de->d_name);
            closedir(d);
            return;
        }
    }
    closedir(d);
    fprintf(stderr, "\n");
}

/* E-01 / tips T-19: one clear report of missing DATA directories at
 * startup, instead of a failure buried in the first scene. Prints nothing
 * when the set is complete except the one-line confirmation; a Windows
 * double-click launch (no console) also gets a real message box (P-06).
 * Runs once per process; the game then continues in its degraded mode. */
static void data_probe(const char *dir)
{
    static int done;
    static const char *need[] = { "ANIMACJE", "ETAPY", "SOUND", "LEVELDES" };
    int missing = 0;
    if (done)
        return;
    done = 1;
    for (unsigned k = 0; k < sizeof need / sizeof *need; k++) {
        if (have_sub(dir, need[k]))
            continue;
        fprintf(stderr, "[leo] DATA %s: missing %s/", dir, need[k]);
        suggest_sub(dir, need[k]);
        missing = 1;
    }
    if (!missing) {
        printf("[leo] DATA %s: complete\n", dir);
        return;
    }
#if defined(PLATFORM_WIN) && PLATFORM_WIN
    char msg[512];
    snprintf(msg, sizeof msg,
             "Game data not found in\n%s\n\nCopy the original game "
             "directories (ANIMACJE, ETAPY, SOUND, LEVELDES) there, or "
             "pass the DATA directory on the command line.", dir);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Lew Leon", msg, NULL);
#endif
}

static const char *pick_data_dir(void)
{
    static char chosen[4096];
    static char base_data[4096];
    const char *cands[3];
    int n = 0;

    const char *env = getenv("LEO_DATA");
    if (env && *env) {
        snprintf(chosen, sizeof chosen, "%s", env);
        fprintf(stderr, "[leo] DATA from $LEO_DATA: %s\n", chosen);
        return chosen;
    }
    char *base = SDL_GetBasePath(); /* SDL_free'd below; no init needed.
                                     * SDL guarantees a trailing separator. */
    if (base) {
        size_t bl = strlen(base);
        while (bl && (base[bl - 1] == '/' || base[bl - 1] == '\\'))
            base[--bl] = '\0';
        snprintf(base_data, sizeof base_data, "%s/data", base);
        SDL_free(base);
        cands[n++] = base_data;
    }
    cands[n++] = "./data";
    cands[n++] = ".";
    for (int i = 0; i < n; i++) {
        if (have_sub(cands[i], "ANIMACJE") || have_sub(cands[i], "ETAPY")) {
            snprintf(chosen, sizeof chosen, "%s", cands[i]);
            printf("[leo] DATA auto-detected: %s\n", chosen);
            return chosen;
        }
    }
    if (base_data[0])
        fprintf(stderr, "[leo] no DATA found: tried %s, ./data and $PWD\n",
                base_data);
    else
        fprintf(stderr, "[leo] no DATA found: tried ./data and $PWD\n");
    return NULL;
}

static void run_menu(const char *data, int audio)
{
    data_probe(data);
    unsigned blen = 0;
    unsigned char *bg = menu_bg(data, &blen);
    int nanim = 0;
    ll_menu_anim *anims = menu_anims(data, &nanim);
    static const char *items[] = { "NEW GAME  (RAN.01)",
                                   "LEVELS    (MAPKA)",
                                   "CREDITS   (AUTORZY.IS)",
                                   "QUIT" };
    const char *aut = getenv("LEO_MENU_AUTO"); /* "1" = pick NEW GAME, quit */
    int auto_i = aut ? atoi(aut) : 0;
    const char *dem = getenv("LEO_DEMO");      /* demo: auto-pick NEW GAME */
    int secs_hook = getenv("LEO_MENU_SECS") != NULL;
    for (;;) {
        int sel;
        int map_mode = 0;
        char lvlname[32] = "";
        if (auto_i) {
            sel = auto_i - 1;
            printf("menu: LEO_MENU_AUTO -> picked item %d\n", sel);
        } else if (dem && !auto_i) {
            sel = 0;
            printf("menu: demo auto-picks NEW GAME (title shown for %.1f s)\n",
                   (atof(dem)));
            SDL_Delay((Uint32)(atof(dem) * 1000.0));
        } else {
            /* Interactive title menu = the original 640x400 chapter MAP
             * (measured 2026-11-14: chapter .IS -> map menu -> arrow +
             * key launches the chapter: studio cards -> flyover ->
             * playable level).  Chapter list = ETAPY id==2 blocks; the
             * six island anchors are measured off the captured menu
             * VRAM index plane (land-blob centroids, PLAN 8b); >6
             * chapters reuse the anchor set (the map page art is a CD
             * data hole, PLAN 8b). */
            static char names[32][32];
            int n = ll_level_names(data, names, 32);
            static const int anch[][4] = {
                /* ix, iy, rw, rh: land-blob centroids + robust
                 * half-extents measured off the menu VRAM capture */
                { 188, 132, 14, 12 }, { 500, 89, 17, 15 },
                { 351, 233, 29, 17 }, { 470, 259, 12, 30 },
                { 589, 281, 22, 7 },  { 83, 348, 19, 29 } };
            ll_menu_ch chs[32];
            int keep[32], m = 0;
            for (int i = 0; i < n; i++)
                if (strcasecmp(names[i], "CHEATS") != 0)
                    keep[m++] = i;
            n = m;
            if (n > 0) {
                for (int i = 0; i < n; i++) {
                    chs[i].title = names[keep[i]];
                    chs[i].ix = anch[i % 6][0];
                    chs[i].iy = anch[i % 6][1];
                    chs[i].rw = anch[i % 6][2];
                    chs[i].rh = anch[i % 6][3];
                }
                printf("map: %d chapters, arrow starts on island 0\n", n);
                printf("map: click = activate (island launches, corner "
                       "acts); arrows + Enter/Space chapter; a/o/r corner "
                       "keys; q/Esc quit\n");
                ll_menu_anim m2[2];
                int m2n = 0;
                for (int i = 0; i < nanim; i++)
                    if (anims[i].tag && strcmp(anims[i].tag, "BERLO.SEQ") == 0) {
                        m2[m2n++] = anims[i];
                        break;
                    }
                for (int i = 0; i < nanim && m2n < 2; i++)
                    if (anims[i].tag &&
                        strcmp(anims[i].tag, "AUTORZY.SEQ") == 0)
                        m2[m2n++] = anims[i];
                sel = ll_menu_map_open(chs, n, m2n ? m2 : anims,
                                       m2n ? m2n : nanim);
            } else {
                fprintf(stderr, "map: no chapters in %s/ETAPY, "
                        "falling back to list menu\n", data);
                sel = ll_menu_open(bg, blen, items, 4, anims, nanim);
            }
            map_mode = 1;
            if (sel >= 0 && sel < n)
                snprintf(lvlname, sizeof lvlname, "%s", names[keep[sel]]);
        }
        if (map_mode && sel == -4) {   /* autorzy corner */
            char p[4096];
            snprintf(p, sizeof p, "%s/AUTORZY.IS", data);
            is_player_run(p, audio, 0, 0);
            continue;
        }
        if (map_mode && sel == -5) {   /* opcje corner: OPTIONS.PCX screen.
                                        * The real OPTIONS.PCX art (Egyptian
                                        * scene) is a full screen already;
                                        * the .MNU script is a CD hole, so
                                        * it shows with just a (BACK) item.
                                        * The RESTGRY/TAK/CNIE/X cursive
                                        * sprites belong to the restart
                                        * confirmation (VL reads "restart
                                        * gry?" + "tak" on them), wired
                                        * under the restart corner. */
            static const char *back[] = { "(BACK)" };
            int r2 = ll_menu_open(bg, blen, back, 1, NULL, 0);
            if (r2 == -3 || r2 == -2) { /* LEO_MENU_SECS hook / q: end the
                                         * whole menu run (CI probe + quits
                                         * the app as the user expects) */
                printf("menu: OPTIONS exit ends menu chain\n");
                break;
            }
            continue;
        }
        if (map_mode && sel == -6) {   /* restart corner: the original
                                        * "restart gry?" confirmation over
                                        * OPTIONS.PCX (reconstruction, PLAN
                                        * 8b): real cursive RESTGRY word +
                                        * TAK / CNIE answer words + X cross
                                        * at their authored (-f0,-f2)
                                        * origins.  The cursive words ARE
                                        * the answer buttons: click them
                                        * or press t (tak) / n (nie) / x.
                                        * tak = restart the game -> the
                                        * new-game level chain (original
                                        * .MNU semantics are a CD hole;
                                        * documented reconstruction). */
            static const char *back[] = { "(BACK)" };
            ll_menu_anim m4[4];
            int m4n = 0;
            for (int i = 0; i < nanim && m4n < 4; i++)
                if (anims[i].tag &&
                    (strcmp(anims[i].tag, "RESTGRY.SEQ") == 0 ||
                     strcmp(anims[i].tag, "TAK.SEQ") == 0 ||
                     strcmp(anims[i].tag, "CNIE.SEQ") == 0 ||
                     strcmp(anims[i].tag, "X.SEQ") == 0))
                    m4[m4n++] = anims[i];
            ll_menu_zone z3[3];
            int nz3 = 0;
            /* fixed answer order (TAK, CNIE, X) = the arrow-key cycle
             * order in the dialog, independent of anim scan order */
            static const char *ztg[3] = { "TAK.SEQ", "CNIE.SEQ", "X.SEQ" };
            static const int zret[3] = { -10, -11, -12 };
            static const char zk[3] = { 't', 'n', 'x' };
            /* nie is the dialog's default answer: it is also the word the
             * original authors shipped in the BRIGHT end of the palette's
             * green ramp (CNIE.SEQ paints idx22 #10f91b, TAK.SEQ only up
             * to idx15 #0aa712), so the selected word's bright-green state
             * starts here and moves to TAK when the user cycles onto it. */
            static const int zdfl[3] = { 0, 1, 0 };
            for (int s = 0; s < 3; s++)
                for (int i = 0; i < m4n; i++)
                    if (m4[i].tag && strcmp(m4[i].tag, ztg[s]) == 0) {
                        z3[nz3++] = (ll_menu_zone){ m4[i].tag, zret[s],
                                                    zk[s], zdfl[s] };
                        break;
                    }
            int r2 = ll_menu_open_ex(bg, blen, back, 1,
                                     m4n ? m4 : NULL, m4n, z3, nz3);
            if (r2 == -10) {   /* tak: restart -> the NEW GAME chain; the
                                * scene runs until ESC, then back to the
                                * map (secs 0; CI pins LEO_LEVEL_SECS) */
                const char *lv = getenv("LEO_DEMO_LEVEL");
                printf("menu: restart confirmed (tak) -> new game (%s)\n",
                       lv && *lv ? lv : "RAN.01");
                ll_level_show(data, lv && *lv ? lv : "RAN.01", 0.0, audio);
                continue;
            }
            if (r2 == -3 || r2 == -2) {
                printf("menu: restart-confirm exit ends menu chain\n");
                break;
            }
            continue;          /* nie / x / (BACK): back to the map */
        }
        if (sel < 0)
            break;
        if (map_mode && sel >= 0) {
            /* chapter launch chain measured 2026-11-14: chapter .IS
             * movie -> flyover scroll -> playable level.  IS names are
             * the CD counterparts where they exist (TUNGA/JUNCTION .IS
             * are CD holes); LEO_DEMO_LEVEL still overrides the level. */
            static const struct { const char *lvl, *is; } ch_is[] = {
                { "RAN.01", "RAN.IS" },     { "ZARIA.01", "ZARIA1.IS" },
                { "ZARIA.02", "ZARIA2.IS" }, { "SADES.06", "SADES.IS" },
                { "KADMAR.07", "KADMAR1.IS" }, { "KADMAR2", "KADMAR2.IS" },
                { "ENA2", "ENA2.IS" },
            };
            for (unsigned k = 0; k < sizeof ch_is / sizeof *ch_is; k++)
                if (strcmp(ch_is[k].lvl, lvlname) == 0) {
                    char p[4096];
                    snprintf(p, sizeof p, "%s/%s", data, ch_is[k].is);
                    is_player_run(p, audio, 0, 0);
                    break;
                }
            const char *lv = getenv("LEO_DEMO_LEVEL");
            ll_level_show(data, lv && *lv ? lv : lvlname, 0.0, audio);
            continue; /* ESC in the scene: back to the chapter map */
        }
        if (sel == 0 && !map_mode) {
            /* RAN.01 is the game's first level ("NEW GAME (RAN.01)"): its
             * tlo= stack on this CD resolves to the PLAN1.PCM parallax
             * plane (plan3.pcx/plan2.pcm AND the whole ANIMACJE leaf are
             * CD holes, so it stays skeletal).  LEO_DEMO_LEVEL=<NAME>
             * demos a richer level instead, e.g. TUNGA.11 (full PLAN3.PCX
             * background + animated platforms). */
            const char *lv = getenv("LEO_DEMO_LEVEL");
            ll_level_show(data, lv && *lv ? lv : "RAN.01", 8.0, audio);
            break;
        } else if (sel == 1 && !map_mode) {
            /* LEVELS = the original MAPKA screen: the engine plays
             * mapka.mod on entry (FUN_0005dec0) and stops it on every
             * exit path (FUN_0005df20).  The MAPKA screen art payload
             * is a CD/HD data hole (no MAPKA.* anywhere), so the
             * submenu reuses ll_menu with no background; the level list
             * comes from the ETAPY container's id==2 blocks.
             * LEO_LEVELSEL_AUTO=N auto-picks level N-1 (CI hook). */
            char names[32][32];
            int n = ll_level_names(data, names, 32);
            if (n <= 0) {
                printf("menu: LEVELS: no level blocks in %s/ETAPY\n", data);
                continue;
            }
            if (audio)
                ll_music_play(data, "MAPKA.MOD");
            static const char back[] = "(BACK)";
            const char *labels[33];
            for (int i = 0; i < n; i++)
                labels[i] = names[i];
            labels[n] = back;
            int li;
            const char *lsaut = getenv("LEO_LEVELSEL_AUTO");
            if (auto_i && lsaut) {
                li = atoi(lsaut) - 1;
                if (li < 0 || li >= n)
                    li = -1; /* out-of-range hook behaves like (BACK) */
                printf("menu: LEO_LEVELSEL_AUTO -> picked level %d\n", li);
            } else {
                li = ll_menu_open(NULL, 0, labels, n + 1, NULL, 0);
            }
            ll_music_stop();
            if (li >= 0 && li < n) {
                ll_level_show(data, names[li], 8.0, audio);
                break; /* one level per run, like NEW GAME */
            }
            if (secs_hook && li == -3)
                break; /* CI deadline (LEO_MENU_SECS): end the run */
            continue; /* (BACK) / ESC: back to the title menu */
        } else if (sel == 2 && !map_mode) {
            if (auto_i)
                break; /* chained runs do not re-play FMVs twice */
            char p[4096];
            snprintf(p, sizeof p, "%s/AUTORZY.IS", data);
            is_player_run(p, audio, 0, 0);
        } else {
            break;
        }
    }
    for (int i = 0; i < nanim; i++)
        free((void *)anims[i].seq);
    free(anims);
    free(bg);
}

/* original boot chain (LOGO/INTRO section of the DOS ll_main): each scene
 * hands off to the next one. P-09: only an ERROR (rc==1) breaks the
 * chain - an ESC skip (rc==2) means "skip to the NEXT scene", exactly
 * like pressing a key in the original. LEO_DEMO_SKIP_FRAMES (CI hook)
 * still fast-forwards straight to the menu. */
static void run_demo_chain(const char *d, int audio)
{
    static const char *boot[] = { "/LOGO.IS", "/PRAWA.IS", "/INTRO_P.IS" };
    unsigned dsf = 0;
    const char *e = getenv("LEO_DEMO_SKIP_FRAMES"); /* CI: fast-forward */
    if (e)
        dsf = (unsigned)atoi(e);
    data_probe(d);
    for (unsigned k = 0; k < sizeof boot / sizeof *boot; k++) {
        char *p = (char *)malloc(strlen(d) + 32);
        sprintf(p, "%s%s", d, boot[k]);
        int rc = is_player_run(p, audio, 0, dsf);
        free(p);
        if (rc == 1 || dsf)
            break; /* error (or CI fast-forward): straight to the menu */
    }
}

int main(int argc, char **argv)
{
    /* Wayland sessions (KDE Plasma desktop reports): SDL's wayland backend
     * windows can NEVER get keyboard focus (WM policy: raise/focus-grant
     * calls are ignored, keys reach the window only after a user click on
     * X11, never on wayland). Same remedy as the pipewire audio pin in
     * ll_audio_init: pin X11 (XWayland) where SDL_RaiseWindow +
     * SetWindowInputFocus work; fall back to the default if no X server.
     * An explicit SDL_VIDEODRIVER always wins. */
    if (!SDL_getenv("SDL_VIDEODRIVER") && SDL_getenv("WAYLAND_DISPLAY")) {
        SDL_setenv("SDL_VIDEODRIVER", "x11", 1);
        int ok = SDL_InitSubSystem(SDL_INIT_VIDEO) == 0;
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        if (ok)
            printf("main: Wayland session detected: using X11/XWayland "
                   "video backend (window keyboard focus works there)\n");
        else {
            printf("main: no X server under Wayland (%s): keeping default "
                   "backend (keyboard focus may not follow windows)\n",
                   SDL_GetError());
            SDL_setenv("SDL_VIDEODRIVER", "wayland", 1);
        }
    }

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

    /* LEO_SAVE=<path>: load + decode the original STATUS.LEO savegame at
     * boot (read-only proof of byte-compatible loading; see ll_save.h.
     * Field semantics await a paired QEMU capture, so nothing is written
     * back: the original save sites are indirect vtable dispatches). */
    {
        const char *sv = getenv("LEO_SAVE");
        if (sv && *sv) {
            ll_status st;
            if (ll_status_load(sv, &st) == 0)
                ll_status_dump(&st, sv);
            else
                printf("ll_save: %s not loadable (need exactly %d B)\n",
                       sv, LL_STATUS_WORDS * 4);
        }
    }

    if (demo) {
        const char *d = demo_data;
        if (!d) {
            d = pick_data_dir();
            if (!d)
                d = "."; /* classic cwd default; data_probe reports the gaps */
        }
        run_demo_chain(d, audio);
        run_menu(d, audio);
        return 0;
    }

    if (!file) {
        static const char *usage =
            "usage: %s FILE.IS [--ppm N OUT.ppm] [--from N] [--no-audio]"
            " [--then FILE.IS] [--menu DATA_DIR] [--demo [DATA_DIR]]\n";
        if (menu_data) {
            run_menu(menu_data, audio); /* menu-only run (no scene first) */
            return 0;
        }
        /* P-09: a bare launch (double-click, no CLI habits) behaved in the
         * original as "start the boot demo": do that when game data is
         * findable (T-17 chain); the usage error only remains when there
         * is truly nothing to run. */
        const char *d = pick_data_dir();
        fprintf(stderr, usage, argv[0]);
        if (!d)
            return 2;
        fprintf(stderr, "[leo] no FILE.IS given: starting the original boot"
                        " demo chain from DATA %s\n", d);
        run_demo_chain(d, audio);
        run_menu(d, audio);
        return 0;
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

    /* scene chain: first file, then --then files. P-09: ESC (rc==2) means
     * "skip this scene, continue with the next one" - the same handoff the
     * original demo chain makes; only an error (rc==1) ends the chain. */
    int rc = is_player_run(file, audio, from, skip_at);
    for (int k = 0; k < nthen && rc != 1; k++)
        rc = is_player_run(then[k], audio, 0, 0);
    if (rc == 2)
        rc = 0; /* skipped forward == normal handoff in chained mode */
    if (menu_data)
        run_menu(menu_data, audio);
    return rc;
}
