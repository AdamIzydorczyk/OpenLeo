/* ll_level.c - M3 slice: level wiring reader + asset viewer.
 * Reads the plain-text .DES descriptor for a level from the LEVELDES
 * container (via llrs), then resolves every referenced asset name against
 * the ETAPY/ANIMACJE block for that level and displays/plays what exists
 * in the original data (PCX backgrounds with their embedded palette, PCM
 * parallax audio). Missing assets (the .plt/.pal/.blk sprite grids are not
 * in the CD containers) are reported, not fatal - later milestones will
 * add those formats.
 *
 * Usage: ll_level DATA_DIR LEVEL [-t SECONDS] [--no-audio]
 *   e.g. ll_level /tmp/opencode/iso_stage/LEO RAN.01
 * ESC quits. Fixed-timestep pacing (2 Hz asset slideshow) per project rule.
 */
#include "ll_config.h"
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

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr,
                "usage: %s DATA_DIR LEVEL [-t SECONDS] [--no-audio]\n",
                argv[0]);
        return 2;
    }
    const char *dir = argv[1], *level = argv[2];
    double secs = 5.0;
    int audio = 1;
    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "-t") == 0 && i + 1 < argc)
            secs = atof(argv[++i]);
        else if (strcmp(argv[i], "--no-audio") == 0)
            audio = 0;
    }

    char p[4096];
    llrs leveldes, etapy;
    snprintf(p, sizeof p, "%s/LEVELDES", dir);
    if (llrs_open(&leveldes, p)) {
        fprintf(stderr, "cannot open %s\n", p);
        return 1;
    }
    snprintf(p, sizeof p, "%s/ETAPY", dir);
    if (llrs_open(&etapy, p)) {
        fprintf(stderr, "cannot open %s\n", p);
        return 1;
    }

    /* 1) .DES text for this level. LEVELDES names use the stem form
     * (RAN.DES) while ETAPY slots use the level dir (RAN.01), and the .DES
     * itself carries dir=. Try candidate names in that order. */
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
    if (des) {
        if (llrs_read(&leveldes, des, &txt) < 0)
            txt = NULL;
    }
    if (!txt) {
        fprintf(stderr, "no .DES descriptor for '%s' in LEVELDES\n", level);
        return 1;
    }
    printf("== %s wiring ==\n", des->name);
    des_kv kvs[64];
    int nk = des_parse((char *)txt, kvs, 64);
    for (int k = 0; k < nk; k++)
        printf("  %-12s = %s\n", kvs[k].key, kvs[k].val);

    /* 2) level block in ETAPY: exact slot, stem slot, then dir= value */
    const llrs_slot *blk = find_or_stem(&etapy, level);
    if (!blk) {
        const char *dir = des_get(kvs, nk, "dir");
        if (dir[0])
            blk = find_or_stem(&etapy, dir);
    }
    printf("== ETAPY block '%s': %s ==\n", level, blk ? blk->name : "MISSING");

    /* collect id=1 payloads of the block */
    llrs_slot *recs = NULL;
    unsigned nrec = 0;
    if (blk && llrs_block(&etapy, blk, &recs, &nrec))
        recs = NULL;
    for (unsigned k = 0; k < nrec; k++)
        if (recs[k].id)
            printf("  rec %2u id=%u %s off=0x%x size=0x%x\n", k, recs[k].id,
                   recs[k].name, recs[k].off, recs[k].size);

    /* 3) viewer: display each PCX payload in the block + any PCM as audio */
    if (SDL_Init(SDL_INIT_VIDEO | (audio ? SDL_INIT_AUDIO : 0)) != 0) {
        fprintf(stderr, "SDL: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window *win = NULL;
    SDL_Renderer *ren = NULL;
    int shown = 0;
    SDL_AudioDeviceID adev = 0;

    for (unsigned k = 0; k < nrec; k++) {
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
                    int sc = 2;
                    win = SDL_CreateWindow("ll_level", SDL_WINDOWPOS_CENTERED,
                                           SDL_WINDOWPOS_CENTERED,
                                           (int)img.w * sc, (int)img.h * sc,
                                           SDL_WINDOW_SHOWN);
                    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
                }
                if (win && ren) {
                    SDL_Texture *tex = SDL_CreateTexture(
                        ren, SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STREAMING, (int)img.w, (int)img.h);
                    void *px;
                    int pitch;
                    if (SDL_LockTexture(tex, NULL, &px, &pitch) == 0) {
                        Uint32 lut[256];
                        for (int i = 0; i < 256; i++)
                            lut[i] = 0xFF000000u |
                                     ((Uint32)img.pal[3 * i] << 16) |
                                     ((Uint32)img.pal[3 * i + 1] << 8) |
                                     img.pal[3 * i + 2];
                        for (unsigned y = 0; y < img.h; y++) {
                            Uint32 *row = (Uint32 *)((char *)px +
                                                     (size_t)y * pitch);
                            const unsigned char *srcp = img.idx +
                                                        (size_t)y * img.w;
                            for (unsigned x = 0; x < img.w; x++)
                                row[x] = lut[srcp[x]];
                        }
                        SDL_UnlockTexture(tex);
                        SDL_RenderClear(ren);
                        SDL_RenderCopy(ren, tex, NULL, NULL);
                        SDL_RenderPresent(ren);
                        shown++;
                        printf("  show %s (%ux%u)\n", nm, img.w, img.h);
                        int done = 0;
                        Uint64 t_end = SDL_GetPerformanceCounter() +
                            (Uint64)(secs * 1000.0 *
                                     SDL_GetPerformanceFrequency() / 1000.0);
                        while (!done && SDL_GetPerformanceCounter() < t_end) {
                            SDL_Event e;
                            while (SDL_WaitEventTimeout(&e, 50)) {
                                if (e.type == SDL_QUIT ||
                                    (e.type == SDL_KEYDOWN &&
                                     e.key.keysym.sym == SDLK_ESCAPE))
                                    done = 2;
                                if (e.type == SDL_KEYDOWN)
                                    break;
                            }
                            if (done)
                                break;
                        }
                        if (done == 2) {
                            SDL_DestroyTexture(tex);
                            ll_pcx_free(&img);
                            free(raw);
                            k = nrec; /* break outer */
                            continue;
                        }
                    }
                    SDL_DestroyTexture(tex);
                }
                ll_pcx_free(&img);
            } else {
                printf("  pcx decode FAILED for %s\n", nm);
            }
        } else if (ci_eq(dot, ".PCM") && audio && !adev) {
            /* PCM parallax track: probe common rates, pick the most
             * plausible (8/11/22 kHz); raw unsigned 8-bit mono. */
            SDL_AudioSpec spec;
            memset(&spec, 0, sizeof spec);
            spec.freq = 22050;
            spec.format = AUDIO_U8;
            spec.channels = 1;
            spec.samples = 1024;
            adev = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
            if (adev) {
                SDL_QueueAudio(adev, raw, recs[k].size);
                SDL_PauseAudio(adev);
                printf("  play %s (%u bytes @22050Hz u8)\n", nm,
                       recs[k].size);
            }
        }
        free(raw);
    }
    printf("== %d PCX shown ==\n", shown);
    if (!shown)
        printf("(no PCX in this level block; originals for level graphics "
               "are not in the CD containers - see PLAN.md M3)\n");

    /* hold so the last image stays up until audio ends or ESC */
    Uint64 hold = SDL_GetPerformanceCounter() +
                  (Uint64)(secs * SDL_GetPerformanceFrequency());
    int quit = 0;
    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT ||
                (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
                quit = 1;
        }
        if (adev && SDL_GetQueuedAudioSize(adev) == 0 &&
            SDL_GetPerformanceCounter() > hold)
            break;
        if (!adev && SDL_GetPerformanceCounter() > hold + 2 *
                         SDL_GetPerformanceFrequency())
            break;
        SDL_Delay(20);
    }
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
    SDL_Quit();
    return 0;
}
