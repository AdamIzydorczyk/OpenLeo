/* main.c - shared native entry for BOTH the linux and windows targets:
 * runs every screen through the platform interface (platform.h; the
 * SDL2 backend is a build choice), plus a headless --ppm mode that decodes
 * and dumps a single frame with no display/audio. --ppm is the
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
 *          [--console (windows: attach/alloc a console for diagnostics)]
 *          [--help | -h | --version]
 */
#include "ll_config.h"
#include "is_stream.h"
#include "ll_level.h"
#include "ll_menu.h"
#include "ll_paths.h"
#include "ll_pcx.h"
#include "ll_save.h"
#include "llrs.h"
#include "platform.h"
#include "platform_sdl/is_player.h"

#if defined(PLATFORM_WIN) && PLATFORM_WIN
#include <windows.h> /* P-06: AttachConsole/AllocConsole for --console */
#endif
#include <dirent.h>
#include <stdarg.h> /* fatal_msg */
#include <stdint.h> /* uint64_t for the demo title hold (plat clock) */
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

/* read one ANIMACJE/MENU block record payload by exact name (malloc'd)
 * or NULL. Case-insensitive: container names are matched case-insensitively
 * everywhere else (llrs_find / llrs_open). */
static unsigned char *menu_record(const char *data, const char *want,
                                 unsigned *len)
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
                if (recs[k].id == 1 &&
                    strcasecmp(recs[k].name, want) == 0) {
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

/* read ANIMACJE/MENU/OPTIONS.PCX payload (malloc'd) or NULL */
static unsigned char *menu_bg(const char *data, unsigned *len)
{
    return menu_record(data, "OPTIONS.PCX", len);
}

/* read ANIMACJE/MENU/BACK.PCX payload - the engine's CLEAN chapter-map
 * landscape (proved 2026-11-15: diffed against the captured menu VRAM
 * the only pixels that differ are the engine's own drawn text). Found
 * only via the fixed sparse-table block scan (it sits at slot 80 of the
 * MENU table, past the old parser's header-count cutoff). */
static unsigned char *menu_back(const char *data, unsigned *len)
{
    return menu_record(data, "BACK.PCX", len);
}

/* read ANIMACJE/MENU title-screen .SEQ overlays in container order;
 * *nanim = count found. Caller frees payload + array.  x/y are fixed
 * logical-canvas origins for the map menu; x=-1 = park on the selected
 * island (BERLO arrow).  RESTGRY/TAK/CNIE/X are the "restart gry?"
 * confirmation-dialog cursive sprites (VL OCR reads "restart gry?" and
 * "tak" on them; CNIE is the ornate cursive answer word, X the cross):
 * they compose over OPTIONS.PCX at their authored (-f0,-f2) origins on
 * the restart corner (reconstruction, PLAN 8b: the .MNU placement is a
 * CD data hole).
 * The corner-WORD .SEQ files (WYJSCIE/OPCJE/RESTART/AUTORZY) are NOT
 * part of this list either: they are cursive WORD sprites (two variants
 * each - measured 2026-11-15: the bigger variant's authored anchor
 * lands exactly on the captured corner-text bbox; the earlier "button
 * plate" reading was wrong). They load separately as ll_menu_word[]
 * for the map's selection-emphasis swap; OPCJEBK.SEQ (the options
 * button background) stays unused. */
static ll_menu_anim *menu_anims(const char *data, int *nanim)
{
    static const char *want[] = { "X.SEQ", "CNIE.SEQ", "BERLO.SEQ",
                                  "RESTGRY.SEQ", "TAK.SEQ" };
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
                        out[*nanim].x = -1;  /* dialog sprites compose at
                                              * their authored (-f0,-f2)
                                              * origins; the BERLO arrow
                                              * parks on the selected
                                              * island (ll_menu.c) */
                        out[*nanim].y = -1;
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
 * $XDG_DATA_HOME/openleo (or ~/.local/share/openleo) ->
 * /usr/share/openleo -> ./data -> . (the classic cwd default). Returns
 * NULL when nothing contains game data. (The per-user WRITABLE dir for
 * saves/config is separate: ported/ll_paths.h.)
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

/* P-06: one fatal-error channel for both platforms: stderr always,
 * plus a real message box on the -mwindows build (double-click
 * launch has no console; `--console` or a --no-subsystem build gets
 * the text on stderr). */
static void fatal_msg(const char *fmt, ...)
{
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);
    fprintf(stderr, "%s\n", buf);
    /* P-06: the box is shown by the backend on Windows only (there a
     * double-click launch has no console); on POSIX this stderr line
     * IS the report. */
    plat_message_box("Lew Leon", buf);
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
    fatal_msg("Game data not found in\n%s\n\nCopy the original game "
              "directories (ANIMACJE, ETAPY, SOUND, LEVELDES) there, or "
              "pass the DATA directory on the command line.", dir);
}

/* Shared silent DATA-candidate scan (tips T-17 search order):
 * <dir of executable>/data -> $XDG_DATA_HOME/openleo (or ~/.local/
 * share/openleo) -> /usr/share/openleo (POSIX installs) -> ./data -> .
 * Fills `out` and returns 1 on the first candidate that holds game
 * data, 0 otherwise. Prints nothing (pick_data_dir adds the logging);
 * $LEO_DATA is handled by pick_data_dir so its log line is unchanged. */
static int find_data_candidate(char *out, size_t n)
{
    char cands[6][4096];
    int nc = 0;
    char *base = plat_base_path();      /* may be NULL; no init needed */
    if (base) {
        size_t bl = strlen(base);
        while (bl && (base[bl - 1] == '/' || base[bl - 1] == '\\'))
            base[--bl] = '\0';
        snprintf(cands[nc++], sizeof cands[0], "%s/data", base);
        free(base);
    }
#if !PLATFORM_WIN && !PLATFORM_DOS
    const char *xdg = getenv("XDG_DATA_HOME");
    const char *home = getenv("HOME");
    if (xdg && *xdg)
        snprintf(cands[nc++], sizeof cands[0], "%s/openleo", xdg);
    else if (home && *home)
        snprintf(cands[nc++], sizeof cands[0], "%s/.local/share/openleo",
                 home);
    snprintf(cands[nc++], sizeof cands[0], "/usr/share/openleo");
#endif
    snprintf(cands[nc++], sizeof cands[0], "./data");
    snprintf(cands[nc++], sizeof cands[0], ".");
    for (int i = 0; i < nc; i++) {
        if (have_sub(cands[i], "ANIMACJE") || have_sub(cands[i], "ETAPY")) {
            size_t l = strlen(cands[i]);
            if (l >= n)
                continue;                 /* absurdly long: skip candidate */
            memcpy(out, cands[i], l + 1);
            return 1;
        }
    }
    return 0;
}

static const char *pick_data_dir(void)
{
    static char chosen[4096];
    static int resolved;
    static const char *result;
    if (resolved)
        return result;

    const char *env = getenv("LEO_DATA");
    if (env && *env) {
        snprintf(chosen, sizeof chosen, "%s", env);
        fprintf(stderr, "[leo] DATA from $LEO_DATA: %s\n", chosen);
        resolved = 1;
        result = chosen;
        return result;
    }
    if (find_data_candidate(chosen, sizeof chosen)) {
        printf("[leo] DATA auto-detected: %s\n", chosen);
        resolved = 1;
        result = chosen;
        return result;
    }
    fprintf(stderr, "[leo] no DATA found: tried <exe>/data, $XDG_DATA_HOME/"
                    "openleo, /usr/share/openleo, ./data and $PWD\n");
    resolved = 1;
    result = NULL;
    return result;
}

/* Silent auto-detected DATA dir for the T-17 legacy-save import (see
 * the save block in main): never logs, so single-file runs keep their
 * exact current output. */
static const char *detect_data_dir(void)
{
    static char d[4096];
    static int done;
    if (!done) {
        done = 1;
        if (!find_data_candidate(d, sizeof d))
            d[0] = '\0';
    }
    return d[0] ? d : NULL;
}

/* FIX-13 (X-12): a missing cutscene in the GUI build surfaced NOTHING
 * (the stderr line is invisible in a -mwindows launch). Call sites gate
 * on !auto_i && !secs_hook: CI-hook runs must never reach a modal box.
 * (The dummy backend's box is a no-op anyway; belt and braces.) */
static void missing_movie_box(const char *path)
{
    char msg[4300];
    snprintf(msg, sizeof msg, "%s not found in the game data", path);
    plat_message_box("Lew Leon", msg);
}

static void run_menu(const char *data, int audio)
{
    data_probe(data);
    unsigned blen = 0;
    unsigned char *bg = menu_bg(data, &blen);
    int nanim = 0;
    ll_menu_anim *anims = menu_anims(data, &nanim);
    static const char *items[] = { "NEW GAME  (ZARIA.01)",
                                   "LEVELS    (MAPKA)",
                                   "CREDITS   (AUTORZY.IS)",
                                   "QUIT" };
    const char *aut = getenv("LEO_MENU_AUTO"); /* "1" = pick NEW GAME, quit */
    int auto_i = aut ? atoi(aut) : 0;
    const char *dem = getenv("LEO_DEMO");      /* demo: auto-pick NEW GAME */
    int secs_hook = getenv("LEO_MENU_SECS") != NULL;
    for (;;) {
        /* FIX-01 (deep dive X-01): the sticky app-quit flag unwinds the
         * menu loop when the window was closed during any screen here
         * (.IS corner, level, chapter movie). Checked at the top so
         * every continue path re-enters this test first. */
        if (plat_quit_requested())
            break;
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
            plat_sleep_until(plat_now_ns()
                             + (uint64_t)(atof(dem) * 1000.0) * 1000000ull);
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
                /* PR-54: chapter -> cursive title image mapping READ FROM
                 * THE DATA. The 11 ANIMACJE/MENU/LEVELnn.SEQ records each
                 * carry the level's name in their last element (decoded
                 * 2026-11-16: 01 ZARIA I, 02 ZARIA II, 03 ENA I, 04 ENA II,
                 * 05 LONA, 06 SADES, 07 KADMAR I, 08 KADMAR II, 09 RAN,
                 * 10 KARA, 11 TUNGA I) and the number IS the level order:
                 * ZARIA I is the first level, which a screenshot of the
                 * original at game start confirms ("start" on its arrow).
                 * The earlier owner-given table (name readings of
                 * 2026-11-15) was wrong for 7 of 11 files. TUNGA2.12
                 * ("TUNGA II") has no title image: font fallback. */
                static const struct { const char *chapter, *rec; }
                    ch_title[] = {
                        { "ZARIA.01",  "LEVEL01.SEQ" },
                        { "ZARIA.02",  "LEVEL02.SEQ" },
                        { "ENA.03",    "LEVEL03.SEQ" },
                        { "ENA2",      "LEVEL04.SEQ" },
                        { "LONA.05",   "LEVEL05.SEQ" },
                        { "SADES.06",  "LEVEL06.SEQ" },
                        { "KADMAR.07", "LEVEL07.SEQ" },
                        { "KADMAR2",   "LEVEL08.SEQ" },
                        { "RAN.01",    "LEVEL09.SEQ" },
                        { "KARA.07",   "LEVEL10.SEQ" },
                        { "TUNGA.11",  "LEVEL11.SEQ" },
                        { "TUNGA2.12", NULL },
                    };
                /* the map cycles the chapters in GAME order (the order of
                 * the title images, then the rest as they were) */
                {
                    int ord[32], no = 0;
                    for (unsigned k = 0; k < sizeof ch_title / sizeof *ch_title;
                         k++)
                        for (int i = 0; i < n; i++)
                            if (!strcasecmp(names[keep[i]], ch_title[k].chapter))
                                ord[no++] = keep[i];
                    for (int i = 0; i < n; i++) {
                        int dup = 0;
                        for (int q = 0; q < no; q++)
                            dup |= ord[q] == keep[i];
                        if (!dup)
                            ord[no++] = keep[i];
                    }
                    for (int i = 0; i < n; i++)
                        keep[i] = ord[i];
                }
                unsigned char *tload[32];
                int ntload = 0;
                for (int i = 0; i < n; i++) {
                    chs[i].title = names[keep[i]];
                    chs[i].ix = anch[i % 6][0];
                    chs[i].iy = anch[i % 6][1];
                    chs[i].rw = anch[i % 6][2];
                    chs[i].rh = anch[i % 6][3];
                    chs[i].seq = NULL;
                    chs[i].seq_len = 0;
                    for (unsigned k = 0;
                         k < sizeof ch_title / sizeof *ch_title; k++)
                        if (ch_title[k].rec &&
                            !strcasecmp(chs[i].title, ch_title[k].chapter)) {
                            unsigned l = 0;
                            unsigned char *b =
                                menu_record(data, ch_title[k].rec, &l);
                            if (b && l) {
                                chs[i].seq = b;
                                chs[i].seq_len = l;
                                tload[ntload++] = b;
                            }
                            break;
                        }
                }
                printf("map: %d chapters, %d cursive title images loaded "
                       "(owner mapping; rest font fallback), arrow starts "
                       "on island 0\n", n, ntload);
                printf("map: keyboard only - Up/Down switch map/corner "
                       "mode, Left/Right cycle within, Enter/Space "
                       "activates, a/o/r corner shortcuts, q/Esc quit\n");
                /* corner-word cursive records (WYJSCIE/RESTART/OPCJE/
                 * AUTORZY order = the map's corner cycle order): the
                 * map swaps the SELECTED word to its emphasis variant
                 * (see ll_menu_word in ll_menu.h) */
                ll_menu_word words[4];
                static const char *const wrec[4] = { "WYJSCIE.SEQ",
                    "RESTART.SEQ", "OPCJE.SEQ", "AUTORZY.SEQ" };
                memset(words, 0, sizeof words);
                for (int k = 0; k < 4; k++) {
                    unsigned l = 0;
                    unsigned char *b = menu_record(data, wrec[k], &l);
                    if (b && l) {
                        words[k].seq = b;
                        words[k].len = l;
                    }
                }
                ll_menu_anim m2[1];
                int m2n = 0;
                for (int i = 0; i < nanim; i++)
                    if (anims[i].tag && strcmp(anims[i].tag, "BERLO.SEQ") == 0) {
                        m2[m2n++] = anims[i];
                        break;
                    }
                /* the map's real base layer: ANIMACJE/MENU/BACK.PCX (may
                 * be NULL -> ll_menu.c falls back to the captured art) */
                unsigned blen = 0;
                unsigned char *back = menu_back(data, &blen);
                sel = ll_menu_map_open(chs, n, m2, m2n, back, blen, words);
                free(back);
                for (int i = 0; i < ntload; i++)
                    free(tload[i]);
                for (int k = 0; k < 4; k++)
                    free((void *)words[k].seq);
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
            if (is_player_run(p, audio, 0, 0) == 1 && !auto_i && !secs_hook)
                missing_movie_box(p);      /* FIX-13 (X-12) */
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
                       lv && *lv ? lv : "ZARIA.01");
                ll_level_show(data, lv && *lv ? lv : "ZARIA.01", 0.0, audio);
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
                    if (is_player_run(p, audio, 0, 0) == 1 && !auto_i
                        && !secs_hook)
                        missing_movie_box(p);  /* FIX-13 (X-12) */
                    break;
                }
            if (plat_quit_requested())
                break;   /* FIX-01: window closed during the chapter .IS:
                          * the level must NOT open after it */
            const char *lv = getenv("LEO_DEMO_LEVEL");
            ll_level_show(data, lv && *lv ? lv : lvlname, 0.0, audio);
            continue; /* ESC in the scene: back to the chapter map */
        }
        if (sel == 0 && !map_mode) {
            /* ZARIA I is the game's first level (PR-54: the map's "start"
             * arrow sits on it in a screenshot of the original, and it is
             * LEVEL01 in the data); the old RAN.01 default was a guess -
             * RAN is the 9th level. LEO_DEMO_LEVEL=<NAME> demos another. */
            const char *lv = getenv("LEO_DEMO_LEVEL");
            ll_level_show(data, lv && *lv ? lv : "ZARIA.01", 8.0, audio);
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
            if (is_player_run(p, audio, 0, 0) == 1 && !auto_i && !secs_hook)
                missing_movie_box(p);      /* FIX-13 (X-12) */
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
        if (rc == 1 || dsf || plat_quit_requested())
            break; /* error, CI fast-forward, or FIX-01 window close (X-01):
                    * stop the chain - main skips the menu on the same flag */
    }
}

/* FIX-13 (X-12): ONE usage text for all three users - unknown option
 * (stderr), --help/-h (stdout, exit 0) and the bare-launch error
 * (stderr, exit 2; 2 is the existing usage-exit contract). Keep the
 * "usage:" prefix: the windows CI smoke greps for it. */
static const char usage_text[] =
    "usage: %s FILE.IS [--ppm N OUT.ppm] [--from N] [--skip-at N]"
    " [--no-audio] [--then FILE.IS] [--menu DATA_DIR] [--demo [DATA_DIR]]"
    " [--console] [--scale N] [--fullscreen] [--help] [--version]\n";

int main(int argc, char **argv)
{
    /* FIX-13 (X-12): line-buffer stdout from the first moment: logs
     * survive a crash, and stdout/stderr interleave correctly when
     * both go to one CI pipe. (Windows --console switches its own
     * attached stream to unbuffered below.) */
    setvbuf(stdout, NULL, _IOLBF, BUFSIZ);

    /* FIX-11: video backend selection now lives entirely in the
     * backend; default is SDL's own choice (X11-first on classic
     * SDL2, Wayland-first on the modern stack), LEO_FORCE_X11=1
     * restores the old X11/XWayland pin. An explicit
     * SDL_VIDEODRIVER still wins, headless is a no-op. */
    plat_prepare_video();

    const char *file = NULL;
    const char *then[MAX_SCENES];
    int nthen = 0;
    const char *menu_data = NULL, *demo_data = NULL;
    ppm_job ppm = { 0, NULL, 0 };
    unsigned from = 0;
    unsigned skip_at = 0;
    int audio = 1, demo = 0, console_wanted = 0;

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
            const char *v = argv[++i]; /* consumed even on overflow: the
                                        * value must never become FILE */
            if (nthen < MAX_SCENES)
                then[nthen++] = v;
            else /* FIX-13 (X-12): overflow used to drop silently */
                fprintf(stderr, "main: --then limit (%d) reached, "
                                "ignoring %s\n", MAX_SCENES, v);
        } else if (strcmp(argv[i], "--menu") == 0 && i + 1 < argc) {
            menu_data = argv[++i];
        } else if (strcmp(argv[i], "--demo") == 0) {
            demo = 1;
            if (i + 1 < argc && argv[i + 1][0] != '-')
                demo_data = argv[++i];
        } else if (strcmp(argv[i], "--console") == 0) {
            console_wanted = 1;
        } else if (strcmp(argv[i], "--scale") == 0 && i + 1 < argc) {
            /* P-10 display options (consumed by the backend via
             * platform.h); default is the historical fixed geometry
             * (2x sub-640-wide content) */
            plat_opt_scale = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--fullscreen") == 0) {
            plat_opt_fullscreen = 1;
        } else if (strcmp(argv[i], "--help") == 0 ||
                   strcmp(argv[i], "-h") == 0) {
            /* FIX-13 (X-12): help/version answer on stdout and exit 0;
             * matched BEFORE the unknown-flag rejection below */
            printf(usage_text, argv[0]);
            return 0;
        } else if (strcmp(argv[i], "--version") == 0) {
            printf("OpenLeo ll_cutscene %s\n", LEO_VERSION);
            return 0;
        } else if (argv[i][0] == '-' && argv[i][1]) {
            /* FIX-13 (X-12): unknown flags used to become FILE.IS. A
             * lone "-" still means the file (dash convention kept). */
            fprintf(stderr, "main: unknown option %s\n", argv[i]);
            fprintf(stderr, usage_text, argv[0]);
            return 2;
        } else {
            file = argv[i];
        }
    }

    /* P-06: the windows build links -mwindows, so a plain launch has no
     * console at all and every diagnostic is invisible. --console attaches
     * to the parent console (launched from cmd) or allocates one, and
     * points stdout/stderr at it; fatal errors additionally surface as a
     * message box (fatal_msg).
     * FIX-13 (X-12): this now runs DIRECTLY after the argument loop and
     * before anything that prints (the T-17 save block below, the demo
     * notices) - previously those early lines went nowhere. */
    if (console_wanted) {
#if defined(PLATFORM_WIN) && PLATFORM_WIN
        if (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole()) {
            freopen("CONOUT$", "w", stdout);
            freopen("CONOUT$", "w", stderr);
            /* FIX-13 (X-12): an attached console writes through - no
             * buffering, so diagnostics land before any later crash */
            setvbuf(stdout, NULL, _IONBF, 0);
        }
#else
        (void)0; /* linux: stdout/stderr already go somewhere */
#endif
    }

    /* FIX-10 (deep dive W-01): ll_cutscene.exe embeds activeCodePage=
     * UTF-8 (resources/app.manifest via the .rc), so normally GetACP()
     * is CP_UTF8 and msvcrt paths are UTF-8. Warn once when that is
     * not true - Windows < 10 1903 without the ACP switch, or an
     * unpacked exe run without its manifest - instead of letting the
     * first non-ASCII data path fail with a bare "not found". */
#if defined(PLATFORM_WIN) && PLATFORM_WIN
    if (GetACP() != CP_UTF8)
        fprintf(stderr, "main: ANSI code page %u is not UTF-8 "
                "(Windows < 10 1903 or missing manifest): data paths "
                "with non-ASCII characters may not be found\n",
                (unsigned)GetACP());
#endif

    /* T-17: resolve the save in the per-user WRITABLE dir, keeping the
     * LEO_SAVE hook as an absolute override (API preserved). A legacy
     * STATUS.LEO sitting next to the game data is imported (COPIED,
     * original untouched) into the user dir exactly once, then loaded
     * from there. Field semantics still await a paired QEMU capture, so
     * nothing is ever written back: the original save sites are indirect
     * vtable dispatches. */
    {
        char sp[4096];
        int imported = 0;
        if (ll_resolve_user_file(sp, sizeof sp, getenv("LEO_SAVE"),
                                 detect_data_dir(), "STATUS.LEO", &imported)) {
            ll_status st;
            if (ll_status_load(sp, &st) == 0) {
                if (imported)
                    printf("[leo] imported legacy save into user dir:"
                           " %s\n", sp);
                ll_status_dump(&st, sp);
            } else
                printf("ll_save: %s not loadable (need exactly %d B)\n",
                       sp, LL_STATUS_WORDS * 4);
        }
    }

    if (demo) {
        const char *d = demo_data;
        if (!d) {
            d = pick_data_dir();
            if (!d)
                d = "."; /* classic cwd default; data_probe reports the gaps */
        }
        /* FIX-13 (X-12): --demo on a directory with NO game data used to
         * land in an empty interactive menu - the "hangs forever in
         * headless runs" report. Same predicate as find_data_candidate,
         * same usage+exit-2 contract as the bare launch below. */
        if (!have_sub(d, "ANIMACJE") && !have_sub(d, "ETAPY")) {
            fprintf(stderr, "main: no game data in %s (need an ANIMACJE/"
                            " or ETAPY/ subdirectory)\n", d);
            fprintf(stderr, usage_text, argv[0]);
            return 2;
        }
        run_demo_chain(d, audio);
        if (!plat_quit_requested())  /* FIX-01: a window close during the
                                      * boot chain quits, it does NOT open
                                      * the menu (was: menu reopened) */
            run_menu(d, audio);
        return 0;
    }

    if (!file) {
        if (menu_data) {
            run_menu(menu_data, audio); /* menu-only run (no scene first) */
            return 0;
        }
        /* P-09: a bare launch (double-click, no CLI habits) behaved in the
         * original as "start the boot demo": do that when game data is
         * findable (T-17 chain); the usage error only remains when there
         * is truly nothing to run.
         * FIX-13 (X-12): the notice line is enough WITH data - the usage
         * block now prints only on the d == NULL error. */
        const char *d = pick_data_dir();
        if (!d) {
            fprintf(stderr, usage_text, argv[0]);
            return 2;
        }
        fprintf(stderr, "[leo] no FILE.IS given: starting the original boot"
                        " demo chain from DATA %s\n", d);
        run_demo_chain(d, audio);
        if (!plat_quit_requested())  /* FIX-01: close during boot chain quits */
            run_menu(d, audio);
        return 0;
    }

    if (ppm.out) {
        is_stream s;
        if (is_open(&s, file) != 0) {
            fatal_msg("cannot open %s", file);
            return 1;
        }
        int rc = is_decode_all(&s, ppm_cb, &ppm, ppm.want + 1);
        printf("%s: frame %u -> %s (%s) rc=%d decoded=%u chunks=%u\n", file,
               ppm.want, ppm.out, ppm.found ? "ok" : "NOT REACHED", rc,
               s.st.frames, s.st.chunks);
        is_close(&s);
        return ppm.found ? 0 : 1;
    }

    /* P-06: the fatal "cannot open" path belongs to main, not the
     * library, so it can surface as a message box on -mwindows runs
     * (is_player_run keeps its own guard for library users). */
    {
        struct stat st;
        if (stat(file, &st) != 0) {
            fatal_msg("cannot open %s", file);
            return 1;
        }
    }

    /* scene chain: first file, then --then files. P-09: ESC (rc==2) means
     * "skip this scene, continue with the next one" - the same handoff the
     * original demo chain makes; only an error (rc==1) ends the chain. */
    int rc = is_player_run(file, audio, from, skip_at);
    for (int k = 0; k < nthen && rc != 1 && !plat_quit_requested(); k++)
        rc = is_player_run(then[k], audio, 0, 0);
    if (rc == 2)
        rc = 0; /* skipped forward == normal handoff in chained mode */
    if (menu_data && !plat_quit_requested())
        run_menu(menu_data, audio);
    return rc;
}
