/* ll_level.c - shared level viewer (see ll_level.h). Resolves the level's
 * plain-text .DES wiring from LEVELDES (name mapping: ETAPY uses the level
 * dir 'RAN.01', LEVELDES uses the stem 'RAN.DES'; the .DES itself carries
 * dir=), then walks the level's ETAPY index block: PCX payloads are shown
 * with their embedded VGA palette, the first PCM payload is queued as raw
 * unsigned-8-bit audio (parallax track). Missing formats (.plt/.pal/.blk
 * sprite grids are not in the CD containers, PLAN.md M3) are reported,
 * not fatal.
 */
#include "ll_level.h"
#include "ll_pcx.h"
#include "llrs.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char key[32], val[256]; } des_kv;

/* case-insensitive compare */
static int ci_eq(const char *a, const char *b)
{
    for (; *a && *b; a++, b++) {
        char x = *a, y = *b;
        if (x >= 'A' && x <= 'Z') x = (char)(x - 'A' + 'a');
        if (y >= 'A' && y <= 'Z') y = (char)(y - 'A' + 'a');
        if (x != y)
            return 0;
    }
    return *a == *b;
}

/* chars before the first '.' (level dir 'RAN.01' -> stem 'RAN') */
static void stem_of(const char *s, char *out, size_t n)
{
    size_t k = 0;
    while (s[k] && s[k] != '.' && k + 1 < n) {
        out[k] = s[k];
        k++;
    }
    out[k] = '\0';
}

/* find printable-name slot, then fall back to stem match ('RAN.01'->'RAN') */
static const llrs_slot *find_or_stem(const llrs *c, const char *lvl)
{
    const llrs_slot *s = llrs_find(c, lvl);
    if (s)
        return s;
    char stem[16];
    stem_of(lvl, stem, sizeof stem);
    for (unsigned k = 0; k < c->cap; k++) {
        char s2[16];
        if (!c->slots[k].id || !c->slots[k].name[0])
            continue;
        stem_of(c->slots[k].name, s2, sizeof s2);
        if (ci_eq(s2, stem))
            return &c->slots[k];
    }
    return NULL;
}

/* value of key `k` in a parsed .DES table ("" if absent) */
static const char *des_get(const des_kv *kvs, int n, const char *k)
{
    for (int i = 0; i < n; i++)
        if (ci_eq(kvs[i].key, k))
            return kvs[i].val;
    return "";
}

/* Parse the "key=value" .DES text (CRLF); returns lines filled. */
static int des_parse(const char *txt, des_kv *kvs, int max)
{
    int n = 0;
    const char *p = txt;
    while (*p && n < max) {
        const char *eol = p;
        while (*eol && *eol != '\n' && *eol != '\r')
            eol++;
        const char *eq = memchr(p, '=', (size_t)(eol - p));
        if (eq) {
            size_t kl = (size_t)(eq - p), vl = (size_t)(eol - eq - 1);
            if (kl < sizeof kvs[n].key && vl < sizeof kvs[n].val) {
                memcpy(kvs[n].key, p, kl);
                kvs[n].key[kl] = '\0';
                memcpy(kvs[n].val, eq + 1, vl);
                kvs[n].val[vl] = '\0';
                n++;
            }
        }
        p = eol;
        while (*p == '\n' || *p == '\r')
            p++;
    }
    return n;
}

/* Lists the level's ANIMACJE animations: the level's id=2 block holds
 * id=1 scripts (SEQ cutscene scripts) and id=2 leaf tables keyed by
 * object name (SCHODY, CAMEL...), each leaf a mini index whose id=1
 * records are that object's animation-frame .SEQ scripts. */
int ll_level_anims(const char *dir, const char *level)
{
    char p[4096];
    llrs an;
    snprintf(p, sizeof p, "%s/ANIMACJE", dir);
    if (llrs_open(&an, p)) {
        printf("ll_level: no ANIMACJE container\n");
        return 1;
    }
    const llrs_slot *blk = find_or_stem(&an, level);
    if (!blk) {
        printf("ll_level: no ANIMACJE block for '%s'\n", level);
        llrs_close(&an);
        return 1;
    }
    llrs_slot *recs = NULL;
    unsigned nrec = 0;
    if (llrs_block(&an, blk, &recs, &nrec)) {
        llrs_close(&an);
        return 1;
    }
    printf("ll_level: ANIMACJE '%s' animations:\n", blk->name);
    for (unsigned k = 0; k < nrec; k++) {
        if (!recs[k].id)
            continue;
        const char *nm = recs[k].name[0] ? recs[k].name : "<binary-key>";
        if (recs[k].id == 1) {
            printf("  script  %-14s %5u B\n", nm, recs[k].size);
        } else if (recs[k].id == 2) {
            llrs_slot *fr = NULL;
            unsigned nfr = 0;
            if (llrs_block(&an, &recs[k], &fr, &nfr) == 0) {
                unsigned nf = 0;
                char first[96] = "";
                for (unsigned j = 0; j < nfr; j++)
                    if (fr[j].id == 1) {
                        if (nf == 0)
                            snprintf(first, sizeof first, "%s",
                                     fr[j].name[0] ? fr[j].name : "?");
                        nf++;
                    }
                printf("  object  %-14s %2u frame scripts (first: %s)\n",
                       nm, nf, nf ? first : "-");
                llrs_block_free(fr);
            }
        }
    }
    llrs_block_free(recs);
    llrs_close(&an);
    return 0;
}

int ll_level_show(const char *dir, const char *level, double secs, int audio)
{
    const char *hs = getenv("LEO_LEVEL_SECS"); /* CI override */
    if (hs)
        secs = atof(hs);
    char p[4096];
    llrs leveldes, etapy;
    snprintf(p, sizeof p, "%s/LEVELDES", dir);
    if (llrs_open(&leveldes, p)) {
        fprintf(stderr, "ll_level: cannot open %s\n", p);
        return 1;
    }
    snprintf(p, sizeof p, "%s/ETAPY", dir);
    if (llrs_open(&etapy, p)) {
        fprintf(stderr, "ll_level: cannot open %s\n", p);
        llrs_close(&leveldes);
        return 1;
    }

    /* 1) .DES text: exact name, "<level>.DES", stem ".DES" */
    char stem[16];
    stem_of(level, stem, sizeof stem);
    const llrs_slot *des = llrs_find(&leveldes, level);
    if (!des) {
        char cand[32];
        snprintf(cand, sizeof cand, "%s.DES", level);
        des = llrs_find(&leveldes, cand);
    }
    if (!des) {
        char cand[32];
        snprintf(cand, sizeof cand, "%s.DES", stem);
        des = llrs_find(&leveldes, cand);
    }
    unsigned char *txt = NULL;
    if (des && llrs_read(&leveldes, des, &txt) < 0)
        txt = NULL;
    des_kv kvs[64];
    int nk = 0;
    if (txt) {
        printf("ll_level: %s wiring:\n", des->name);
        nk = des_parse((char *)txt, kvs, 64);
        for (int k = 0; k < nk; k++)
            printf("  %-12s = %s\n", kvs[k].key, kvs[k].val);
    } else {
        printf("ll_level: no .DES for '%s' (showing ETAPY block only)\n",
               level);
    }

    /* 2) level block in ETAPY: exact, stem, then dir= from the .DES */
    const llrs_slot *blk = find_or_stem(&etapy, level);
    if (!blk && nk) {
        const char *d = des_get(kvs, nk, "dir");
        if (d[0])
            blk = find_or_stem(&etapy, d);
    }
    llrs_slot *recs = NULL;
    unsigned nrec = 0;
    if (blk)
        llrs_block(&etapy, blk, &recs, &nrec);

    ll_level_anims(dir, level);

    int shown = 0, rc = 0;
    SDL_AudioDeviceID adev = 0;
    SDL_Window *win = NULL;
    SDL_Renderer *ren = NULL;
    int esc = 0;

    for (unsigned k = 0; k < nrec && !esc; k++) {
        const char *nm = recs[k].name;
        const char *dot = strrchr(nm, '.');
        if (!recs[k].id || !dot)
            continue;
        unsigned char *raw = NULL;
        if (llrs_read(&etapy, &recs[k], &raw) < 0)
            continue;
        if (ci_eq(dot, ".PCX")) {
            ll_pcx img;
            if (ll_pcx_decode(raw, (size_t)recs[k].size, &img) == 0) {
                if (!win) {
                    int sc = img.w < 640 ? 2 : 1;
                    win = SDL_CreateWindow("Lew Leon - level intro",
                                           SDL_WINDOWPOS_CENTERED,
                                           SDL_WINDOWPOS_CENTERED,
                                           (int)img.w * sc, (int)img.h * sc,
                                           SDL_WINDOW_SHOWN);
                    ren = SDL_CreateRenderer(win, -1,
                                             SDL_RENDERER_ACCELERATED);
                    if (!ren)
                        ren = SDL_CreateRenderer(win, -1,
                                                 SDL_RENDERER_SOFTWARE);
                }
                if (ren) {
                    SDL_Texture *tex = SDL_CreateTexture(
                        ren, SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STREAMING, (int)img.w,
                        (int)img.h);
                    void *px;
                    int pitch;
                    if (tex && SDL_LockTexture(tex, NULL, &px, &pitch) == 0) {
                        Uint32 lut[256];
                        for (int i = 0; i < 256; i++)
                            lut[i] = 0xFF000000u |
                                     ((Uint32)img.pal[3 * i] << 16) |
                                     ((Uint32)img.pal[3 * i + 1] << 8) |
                                     img.pal[3 * i + 2];
                        for (unsigned y = 0; y < img.h; y++) {
                            Uint32 *row = (Uint32 *)((char *)px +
                                                     (size_t)y * pitch);
                            const unsigned char *sp = img.idx +
                                                      (size_t)y * img.w;
                            for (unsigned x = 0; x < img.w; x++)
                                row[x] = lut[sp[x]];
                        }
                        SDL_UnlockTexture(tex);
                        SDL_RenderClear(ren);
                        SDL_RenderCopy(ren, tex, NULL, NULL);
                        SDL_RenderPresent(ren);
                        shown++;
                        printf("ll_level: show %s (%ux%u)\n", nm, img.w,
                               img.h);
                        /* fixed hold, ESC cancels */
                        Uint64 t_end = SDL_GetPerformanceCounter() +
                            (Uint64)(secs * (double)SDL_GetPerformanceFrequency());
                        while (!esc && SDL_GetPerformanceCounter() < t_end) {
                            SDL_Event e;
                            while (SDL_PollEvent(&e)) {
                                if (e.type == SDL_QUIT ||
                                    (e.type == SDL_KEYDOWN &&
                                     (e.key.keysym.sym == SDLK_ESCAPE ||
                                      e.key.keysym.sym == SDLK_q)))
                                    esc = 1;
                            }
                            SDL_Delay(20);
                        }
                    }
                    if (tex)
                        SDL_DestroyTexture(tex);
                }
                ll_pcx_free(&img);
            } else {
                printf("ll_level: PCX decode failed for %s\n", nm);
            }
        } else if (ci_eq(dot, ".PCM") && audio && !adev) {
            SDL_AudioSpec want;
            memset(&want, 0, sizeof want);
            want.freq = 22050;
            want.format = AUDIO_U8;
            want.channels = 1;
            want.samples = 1024;
            adev = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0);
            if (adev) {
                SDL_QueueAudio(adev, raw, recs[k].size);
                SDL_PauseAudioDevice(adev, 0);
                printf("ll_level: play %s (%u B u8@22050)\n", nm,
                       recs[k].size);
            }
        }
        free(raw);
    }
    if (!shown)
        printf("ll_level: no viewable PCX in '%s' block "
               "(level art formats pending - PLAN.md M3)\n", level);
    rc = shown ? 0 : 1;

    /* keep the last image until audio drains or secs elapse (ESC cancels) */
    Uint64 hold = SDL_GetPerformanceCounter() +
                  (Uint64)(secs * (double)SDL_GetPerformanceFrequency());
    while (!esc && adev && SDL_GetQueuedAudioSize(adev) > 0) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT ||
                (e.type == SDL_KEYDOWN &&
                 (e.key.keysym.sym == SDLK_ESCAPE ||
                  e.key.keysym.sym == SDLK_q)))
                esc = 1;
        }
        SDL_Delay(50);
    }
    if (win && !esc) { /* brief visible hold even without audio */
        Uint64 t_end = hold;
        while (!esc && SDL_GetPerformanceCounter() < t_end) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT ||
                    (e.type == SDL_KEYDOWN &&
                     (e.key.keysym.sym == SDLK_ESCAPE ||
                      e.key.keysym.sym == SDLK_q)))
                    esc = 1;
            }
            SDL_Delay(20);
        }
    }
    if (esc)
        printf("ll_level: ESC -> advance\n");

    if (adev)
        SDL_CloseAudioDevice(adev);
    if (ren)
        SDL_DestroyRenderer(ren);
    if (win)
        SDL_DestroyWindow(win);
    llrs_block_free(recs);
    free(txt);
    llrs_close(&leveldes);
    llrs_close(&etapy);
    return rc;
}
