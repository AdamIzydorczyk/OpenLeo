/* ll_pcx-style present path (palette LUT into locked ARGB8888 texture).
 * Menu text: built-in 8x8 bitmap font (ASCII 32..126), drawn as 1px white;
 * cursor '>' in gold. Optional .SEQ overlay animations (ll_seq_patch_draw
 * into an 8bpp index canvas over the PCX indices, then one palette LUT).
 * Fixed pacing: event-driven with SDL_Delay, no busy loop. See ll_menu.h. */
#include "ll_menu.h"
#include "ll_menu_dac.h"
#include "ll_pcx.h"
#include "ll_seq.h"
#include "ll_level.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SX(v) ((int)(short)(v))

/* 8x8 font, row-major bytes, bit 7 = leftmost pixel. */
static const unsigned char FONT[95][8] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, /* ' ' */
    {0x18,0x18,0x18,0x18,0x18,0x00,0x18,0x00}, /* '!' */
    {0x6C,0x6C,0x00,0x00,0x00,0x00,0x00,0x00}, /* '"' */
    {0x08,0x3E,0x28,0x3E,0x08,0x3E,0x08,0x00}, /* '#' approx */
    {0x08,0x1E,0x28,0x1C,0x0A,0x3C,0x08,0x00}, /* '$' approx */
    {0x62,0x64,0x08,0x10,0x2C,0x4C,0x00,0x00}, /* '%' approx */
    {0x10,0x2A,0x2C,0x10,0x36,0x2A,0x44,0x00}, /* '&' approx */
    {0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x00}, /* '\'' */
    {0x08,0x10,0x20,0x20,0x20,0x10,0x08,0x00}, /* '(' */
    {0x08,0x04,0x02,0x02,0x02,0x04,0x08,0x00}, /* ')' */
    {0x08,0x3E,0x08,0x1C,0x28,0x00,0x00,0x00}, /* '*' approx */
    {0x08,0x08,0x3E,0x08,0x08,0x00,0x00,0x00}, /* '+' */
    {0x00,0x00,0x00,0x00,0x30,0x10,0x20,0x00}, /* ',' */
    {0x00,0x00,0x3E,0x00,0x00,0x00,0x00,0x00}, /* '-' */
    {0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00}, /* '.' */
    {0x02,0x04,0x08,0x10,0x20,0x40,0x00,0x00}, /* '/' */
    {0x1C,0x22,0x2E,0x32,0x3A,0x22,0x1C,0x00}, /* '0' */
    {0x18,0x28,0x08,0x08,0x08,0x08,0x3E,0x00}, /* '1' */
    {0x1C,0x22,0x02,0x0C,0x10,0x20,0x3E,0x00}, /* '2' */
    {0x3E,0x02,0x0C,0x02,0x22,0x22,0x1C,0x00}, /* '3' */
    {0x04,0x0C,0x14,0x24,0x3E,0x04,0x04,0x00}, /* '4' */
    {0x3E,0x20,0x3C,0x02,0x02,0x22,0x1C,0x00}, /* '5' */
    {0x1C,0x20,0x3C,0x22,0x22,0x22,0x1C,0x00}, /* '6' */
    {0x3E,0x22,0x04,0x08,0x10,0x10,0x10,0x00}, /* '7' */
    {0x1C,0x22,0x22,0x1C,0x22,0x22,0x1C,0x00}, /* '8' */
    {0x1C,0x22,0x22,0x1E,0x02,0x02,0x1C,0x00}, /* '9' */
    {0x00,0x00,0x18,0x18,0x00,0x18,0x18,0x00}, /* ':' */
    {0x00,0x00,0x18,0x18,0x00,0x18,0x08,0x10}, /* ';' */
    {0x04,0x08,0x10,0x20,0x10,0x08,0x04,0x00}, /* '<' */
    {0x00,0x3E,0x00,0x00,0x3E,0x00,0x00,0x00}, /* '=' */
    {0x20,0x10,0x08,0x04,0x08,0x10,0x20,0x00}, /* '>' */
    {0x1C,0x22,0x02,0x0C,0x18,0x00,0x18,0x00}, /* '?' */
    {0x1C,0x22,0x2E,0x2A,0x2E,0x20,0x1C,0x00}, /* '@' approx */
    {0x08,0x1C,0x22,0x22,0x22,0x1C,0x08,0x00}, /* 'A' */
    {0x3C,0x22,0x3C,0x22,0x22,0x22,0x3C,0x00}, /* 'B' */
    {0x1C,0x22,0x20,0x20,0x20,0x22,0x1C,0x00}, /* 'C' */
    {0x3C,0x22,0x22,0x22,0x22,0x22,0x3C,0x00}, /* 'D' */
    {0x3E,0x20,0x3C,0x20,0x20,0x20,0x3E,0x00}, /* 'E' */
    {0x3E,0x20,0x3C,0x20,0x20,0x20,0x20,0x00}, /* 'F' */
    {0x1C,0x22,0x20,0x2E,0x22,0x22,0x1C,0x00}, /* 'G' */
    {0x22,0x22,0x3E,0x22,0x22,0x22,0x22,0x00}, /* 'H' */
    {0x1C,0x08,0x08,0x08,0x08,0x08,0x1C,0x00}, /* 'I' */
    {0x0E,0x04,0x04,0x04,0x04,0x24,0x18,0x00}, /* 'J' */
    {0x24,0x28,0x30,0x38,0x28,0x24,0x22,0x00}, /* 'K' approx */
    {0x20,0x20,0x20,0x20,0x20,0x20,0x3E,0x00}, /* 'L' */
    {0x22,0x36,0x36,0x2A,0x22,0x22,0x22,0x00}, /* 'M' */
    {0x22,0x22,0x12,0x08,0x14,0x22,0x22,0x00},  /* 'N' */
    {0x1C,0x22,0x22,0x22,0x22,0x22,0x1C,0x00}, /* 'O' */
    {0x3C,0x22,0x22,0x3C,0x20,0x20,0x20,0x00}, /* 'P' */
    {0x1C,0x22,0x22,0x22,0x2A,0x24,0x1A,0x00}, /* 'Q' */
    {0x3C,0x22,0x22,0x3C,0x28,0x24,0x22,0x00}, /* 'R' */
    {0x1E,0x20,0x1C,0x02,0x02,0x22,0x1C,0x00}, /* 'S' */
    {0x3E,0x08,0x08,0x08,0x08,0x08,0x08,0x00}, /* 'T' */
    {0x22,0x22,0x22,0x22,0x22,0x22,0x1C,0x00}, /* 'U' */
    {0x22,0x22,0x22,0x22,0x14,0x14,0x08,0x00}, /* 'V' */
    {0x22,0x22,0x2A,0x2A,0x36,0x22,0x22,0x00}, /* 'W' */
    {0x22,0x22,0x14,0x08,0x14,0x22,0x22,0x00}, /* 'X' */
    {0x14,0x14,0x14,0x08,0x08,0x08,0x08,0x00}, /* 'Y' */
    {0x3E,0x02,0x04,0x08,0x10,0x20,0x3E,0x00}, /* 'Z' */
    {0x1C,0x10,0x10,0x10,0x10,0x10,0x1C,0x00}, /* '[' */
    {0x40,0x20,0x10,0x08,0x04,0x02,0x00,0x00}, /* '\' */
    {0x1C,0x04,0x04,0x04,0x04,0x04,0x1C,0x00}, /* ']' */
    {0x08,0x14,0x22,0x00,0x00,0x00,0x00,0x00}, /* '^' */
    {0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x00}, /* '_' */
    {0x10,0x08,0x00,0x00,0x00,0x00,0x00,0x00}, /* '`' */
    {0x00,0x1C,0x02,0x1E,0x22,0x22,0x1E,0x00}, /* 'a' */
    {0x30,0x28,0x3C,0x22,0x22,0x22,0x3C,0x00}, /* 'b' */
    {0x1E,0x20,0x38,0x24,0x24,0x24,0x38,0x00}, /* 'c' approx */
    {0x0C,0x12,0x1E,0x12,0x12,0x12,0x1C,0x00}, /* 'd' approx */
    {0x1C,0x22,0x3E,0x20,0x20,0x22,0x1C,0x00}, /* 'e' */
    {0x1C,0x12,0x10,0x38,0x10,0x10,0x10,0x00}, /* 'f' approx */
    {0x1C,0x12,0x12,0x1C,0x02,0x02,0x1C,0x00}, /* 'g' approx */
    {0x22,0x22,0x3E,0x22,0x22,0x22,0x22,0x00}, /* 'h' approx (H) */
    {0x1C,0x08,0x08,0x08,0x08,0x08,0x1C,0x00}, /* 'i' (I) */
    {0x0E,0x04,0x04,0x04,0x04,0x24,0x18,0x00}, /* 'j' (J) */
    {0x24,0x28,0x30,0x38,0x28,0x24,0x22,0x00}, /* 'k' approx */
    {0x20,0x20,0x20,0x20,0x20,0x20,0x3E,0x00}, /* 'l' (L) */
    {0x22,0x36,0x36,0x2A,0x22,0x22,0x22,0x00}, /* 'm' approx */
    {0x22,0x22,0x12,0x08,0x14,0x22,0x22,0x00}, /* 'n' approx */
    {0x1C,0x22,0x22,0x22,0x22,0x22,0x1C,0x00}, /* 'o' (O) */
    {0x3C,0x22,0x22,0x3C,0x20,0x20,0x20,0x00}, /* 'p' approx */
    {0x1C,0x22,0x22,0x22,0x2A,0x24,0x1A,0x00}, /* 'q' approx */
    {0x3C,0x22,0x22,0x3C,0x28,0x24,0x22,0x00}, /* 'r' approx */
    {0x1E,0x20,0x1C,0x02,0x02,0x22,0x1C,0x00}, /* 's' approx */
    {0x08,0x38,0x08,0x08,0x08,0x08,0x0C,0x00}, /* 't' approx */
    {0x12,0x12,0x12,0x12,0x12,0x12,0x0C,0x00}, /* 'u' approx */
    {0x12,0x12,0x12,0x12,0x0C,0x0C,0x08,0x00}, /* 'v' approx */
    {0x22,0x22,0x2A,0x2A,0x36,0x22,0x22,0x00}, /* 'w' approx */
    {0x12,0x14,0x08,0x08,0x14,0x12,0x00,0x00}, /* 'x' approx */
    {0x12,0x12,0x12,0x0C,0x08,0x08,0x08,0x00}, /* 'y' approx */
    {0x1E,0x04,0x08,0x10,0x20,0x3E,0x00,0x00}, /* 'z' approx */
    {0x1C,0x08,0x10,0x20,0x10,0x08,0x1C,0x00}, /* '{' */
    {0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x00}, /* '|' */
    {0x1C,0x10,0x08,0x04,0x08,0x10,0x1C,0x00}, /* '}' */
    {0x30,0x18,0x0C,0x00,0x00,0x00,0x00,0x00}, /* '~' approx */
};

static void draw_glyph(Uint32 *fb, int w, int h, int x, int y, char c,
                       Uint32 col)
{
    if (c < 32 || c > 126)
        c = '?';
    const unsigned char *g = FONT[c - 32];
    for (int r = 0; r < 8; r++)
        for (int b = 0; b < 8; b++)
            if (g[r] & (0x80 >> b)) {
                int px = x + b, py = y + r;
                if (px >= 0 && px < w && py >= 0 && py < h)
                    fb[py * w + px] = col;
            }
}

static void draw_text(Uint32 *fb, int w, int h, int x, int y, const char *s,
                      Uint32 col)
{
    for (; *s; s++, x += 8)
        draw_glyph(fb, w, h, x, y, *s, col);
}

static void menu_anims_free(ll_seq *aseq, int nanims)
{
    if (!aseq)
        return;
    for (int i = 0; i < nanims; i++)
        if (aseq[i].els)
            ll_seq_free(&aseq[i]);
    free(aseq);
}

/* CI hook: dump the composed RGB frame as PPM (P6) */
static int ppm_fb(const char *path, const Uint32 *fb, int w, int h)
{
    FILE *f = fopen(path, "wb");
    if (!f)
        return -1;
    fprintf(f, "P6\n%d %d\n255\n", w, h);
    for (int i = 0; i < w * h; i++) {
        unsigned char c[3] = { (unsigned char)(fb[i] >> 16),
                               (unsigned char)(fb[i] >> 8),
                               (unsigned char)fb[i] };
        if (fwrite(c, 1, 3, f) != 3) { fclose(f); return -1; }
    }
    return fclose(f) == 0 ? 0 : -1;
}

int ll_menu_open(const unsigned char *bg_pcx, unsigned bg_len,
                 const char *const *items, int nitems,
                 const ll_menu_anim *anims, int nanims)
{
    if (nitems < 1)
        return -1;

    /* background image (optional) */
    ll_pcx bg;
    memset(&bg, 0, sizeof bg);
    if (bg_pcx && ll_pcx_decode(bg_pcx, bg_len, &bg) != 0)
        fprintf(stderr, "ll_menu: background PCX decode failed\n");
    if (bg_pcx && bg.w)
        printf("menu: background OPTIONS.PCX %ux%u loaded\n", bg.w, bg.h);
    int W = bg.w ? (int)bg.w : 320, H = bg.h ? (int)bg.h : 200;

    /* overlay animations: parsed once; frames cycled off the redraw tick */
    ll_seq *aseq = nanims > 0 ? calloc((size_t)nanims, sizeof *aseq) : NULL;
    int nanim_ok = 0;
    if (aseq) {
        if (!bg.idx) {
            fprintf(stderr, "menu: .SEQ anims need a PCX palette; skipped\n");
            menu_anims_free(aseq, nanims);
            aseq = NULL;
            nanims = 0;
        } else {
            for (int i = 0; i < nanims; i++) {
                if (ll_seq_parse(anims[i].seq, anims[i].len, &aseq[i]) != 0) {
                    fprintf(stderr, "menu: anim %d .SEQ parse failed\n", i);
                    continue;
                }
                const ll_seq_elem *e0 = &aseq[i].els[0];
                printf("menu: anim %d: %u frames at (%d,%d) period %ums\n",
                       i, aseq[i].count, -SX(e0->f0), -SX(e0->f2),
                       anims[i].period_ms ? anims[i].period_ms : 100);
                nanim_ok++;
            }
            if (!nanim_ok) {
                menu_anims_free(aseq, nanims);
                aseq = NULL;
            }
        }
    }

    int scale = W < 640 ? 2 : 1;
    SDL_Window *win = SDL_CreateWindow("Lew Leon - menu",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED, W * scale,
                                       H * scale, SDL_WINDOW_SHOWN);
    if (!win) {
        menu_anims_free(aseq, nanims);
        ll_pcx_free(&bg);
        return -2;
    }
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (!ren)
        ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888,
                                         SDL_TEXTUREACCESS_STREAMING, W, H);
    if (!ren || !tex) {
        SDL_DestroyWindow(win);
        menu_anims_free(aseq, nanims);
        ll_pcx_free(&bg);
        return -2;
    }
    SDL_SetTextureScaleMode(tex, scale > 1 ? SDL_ScaleModeNearest
                                           : SDL_ScaleModeLinear);

    Uint32 *fb = (Uint32 *)malloc((size_t)W * H * sizeof *fb);
    unsigned char *cv = malloc((size_t)W * H);
    if (!fb || !cv) {
        free(fb);
        free(cv);
        SDL_DestroyTexture(tex);
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        menu_anims_free(aseq, nanims);
        ll_pcx_free(&bg);
        return -2;
    }

    int sel = 0, quit = 0, ret = -1;
    unsigned tick = 0;   /* fixed 30 ms redraw ticks drive anim frames */
    /* CI hook: LEO_MENU_SECS=N shows the menu N seconds then auto-quits */
    const char *sece = getenv("LEO_MENU_SECS");
    Uint64 t_deadline = sece ? SDL_GetTicks64() + 1000 * (Uint64)atof(sece) : 0;
    while (!quit) {
        /* compose: background (image or black) then the item list */
        if (bg.idx) {
            /* 8bpp index canvas = PCX background + .SEQ sprite overlays,
             * then ONE palette LUT (the game composites the same way) */
            memcpy(cv, bg.idx, (size_t)W * H);
            for (int a = 0; a < nanims && aseq; a++) {
                if (!aseq[a].els)
                    continue;
                unsigned per = anims[a].period_ms ? anims[a].period_ms : 100;
                unsigned fr = (unsigned)((Uint64)tick * 30 / per)
                              % aseq[a].count;
                const ll_seq_elem *e = &aseq[a].els[fr];
                ll_seq_draw_elem(&aseq[a], e, cv, W, H,
                                 -SX(e->f0), -SX(e->f2));
            }
            Uint32 lut[256];
            for (int i = 0; i < 256; i++)
                lut[i] = 0xFF000000u | ((Uint32)bg.pal[3 * i] << 16)
                         | ((Uint32)bg.pal[3 * i + 1] << 8)
                         | (Uint32)bg.pal[3 * i + 2];
            for (int i = 0; i < W * H; i++)
                fb[i] = lut[cv[i]];
        } else {
            for (int i = 0; i < W * H;
                 i++) /* dark vertical gradient - simple but intentional */
                fb[i] = 0xFF000000u | (Uint32)(20 + (i / W) / 8);
        }

        /* CI hook: LEO_MENU_PPM=PATH dumps the composited frame (bg +
         * .SEQ sprites + menu text) once at that tick. */
        /* windowed list: long lists (level select has 14 entries) scroll
         * a 10-row window centred on the selection instead of overflowing */
        int win = nitems > 10 ? 10 : nitems;
        int top = 0;
        if (nitems > win) {
            top = sel - win / 2;
            if (top < 0) top = 0;
            if (top > nitems - win) top = nitems - win;
        }
        int x0 = W / 2 - 64, y0 = H / 2 - 8 * win / 2;
        if (bg.idx) { /* OPTIONS.PCX is a full text screen already */
            x0 = 24;
            y0 = H - 8 * (win + 2);
        }
        for (int j = 0; j < win; j++) {
            int i = top + j;
            draw_text(fb, W, H, x0, y0 + 12 * j, items[i], 0xFFFFFFFFu);
            if (i == sel)
                draw_text(fb, W, H, x0 - 12, y0 + 12 * j, ">",
                          0xFFFFFF00u);
        }
        if (top > 0)
            draw_text(fb, W, H, x0, y0 - 10, "^", 0xFFAAAAAAu);
        if (top + win < nitems)
            draw_text(fb, W, H, x0, y0 + 12 * win - 2, "v", 0xFFAAAAAAu);

        {
            const char *ppm_e = getenv("LEO_MENU_PPM");
            const char *tick_e = getenv("LEO_MENU_PPM_TICK");
            unsigned want = tick_e ? (unsigned)atoi(tick_e) : 0;
            if (ppm_e && tick == want) {
                if (ppm_fb(ppm_e, fb, W, H) == 0)
                    printf("menu: LEO_MENU_PPM wrote %s (tick %u)\n",
                           ppm_e, tick);
            }
        }

        void *px;
        int pitch;
        if (SDL_LockTexture(tex, NULL, &px, &pitch) == 0) {
            for (int y = 0; y < H; y++)
                memcpy((char *)px + (size_t)y * pitch,
                       fb + (size_t)y * W, (size_t)W * 4);
            SDL_UnlockTexture(tex);
        }
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);

        SDL_Event e;
        Uint64 t_next = SDL_GetTicks64() + 30;
        while (!quit) {
            if (t_deadline && SDL_GetTicks64() >= t_deadline) {
                printf("menu: LEO_MENU_SECS expired (auto-quit)\n");
                quit = 1;
                ret = -3;
                break;
            }
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    quit = 1;
                    ret = -2;
                } else if (e.type == SDL_KEYDOWN) {
                    SDL_Keycode k = e.key.keysym.sym;
                    if (k == SDLK_UP)
                        sel = (sel + nitems - 1) % nitems;
                    else if (k == SDLK_DOWN)
                        sel = (sel + 1) % nitems;
                    else if (k == SDLK_RETURN || k == SDLK_KP_ENTER
                             || k == SDLK_ESCAPE) {
                        quit = 1;
                        ret = sel;
                    } else if (k == SDLK_q) {
                        quit = 1;
                        ret = -2;
                    }
                }
            }
            Sint64 left = (Sint64)(t_next - SDL_GetTicks64());
            ll_music_pump();   /* submenu music rides the SDL queue */
            if (left > 0)
                SDL_Delay((Uint32)left);
            else
                break;
        }
        tick++;
    }
    free(fb);
    free(cv);
    menu_anims_free(aseq, nanims);
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    ll_pcx_free(&bg);
    return ret;
}

/* ------------------------------------------------------------------ */
/* Chapter map menu (the original title-menu state, 640x400).          */
/* Layout constants from the 2026-11-14 QEMU captures (g781 frame +   */
/* hunt7/hunt9 state traces); see PLAN.md "Menu layout cracked".      */

#define MAP_W 640
#define MAP_H 400

/* The original composites in a 320x200 logical canvas (the .SEQ sprite
 * coords, e.g. AUTORZY.SEQ @ (4,180), are authored in that space) and
 * doubles 2x2 onto the 640x400 CRTC; hunt13's VRAM dump proved the fb is
 * linear 8bpp 320x200.  We mirror that: everything composites at logical
 * scale, presentation + PPM merge to 640x400. */
#define MAP_LW 320
#define MAP_LH 200

/* 1x 8x8 glyph at logical scale (originals are .SEQ sprites/art; this is
 * our documented placeholder font, same space, half the 640-space size) */
static void draw_glyph1(Uint32 *fb, int x, int y, char c, Uint32 col)
{
    if (c < 32 || c > 126)
        c = '?';
    const unsigned char *g = FONT[c - 32];
    for (int r = 0; r < 8; r++)
        for (int b = 0; b < 8; b++)
            if (g[r] & (0x80 >> b)) {
                int px = x + b, py = y + r;
                if (px >= 0 && px < MAP_LW && py >= 0 && py < MAP_LH)
                    fb[py * MAP_LW + px] = col;
            }
}

static void draw_text1(Uint32 *fb, int x, int y, const char *s, Uint32 col)
{
    for (; *s; s++, x += 8)
        draw_glyph1(fb, x, y, *s, col);
}

static void draw_text1_r(Uint32 *fb, int xr, int y, const char *s, Uint32 col)
{
    int n = 0;
    for (const char *p = s; *p; p++)
        n++;
    draw_text1(fb, xr - 8 * n, y, s, col);
}

static Uint32 menu_lut(int i)
{
    /* Real menu DAC recovered from the original's VGA state (capture +
     * QEMU xp dump; see ll_menu_dac.h provenance). Same index space the
     * ANIMACJE/MENU .SEQ sprites are drawn into: idx 15 = green corner
     * words, 22 = bright green title, 90..95 dark / 218..230 bright =
     * BERLO golds, 44/46/51/102-107 sea blues, 199/200/202 island sand. */
    return ll_menu_dac[i & 0xFF];
}

/* bg on the logical canvas; chs anchors are 640-space, halved here */
static void map_bg(Uint32 *fb, const ll_menu_ch *chs, int nchs)
{
    /* Placeholder for the globe+islands art (a CD data hole, PLAN 8b),
     * but using the REAL menu DAC tones recovered from the original
     * (ll_menu_dac.h): deep sea idx102 -> sea idx46 gradient, sea-blue
     * sparkles, green/sand island blobs at the measured anchors. */
    for (int y = 0; y < MAP_LH; y++) {
        int t = y * 255 / (MAP_LH - 1);
        Uint32 a = ll_menu_dac[102], b = ll_menu_dac[46];
        Uint32 c = 0xFF000000u
                   | ((((a >> 16 & 255) * (255 - t) + (b >> 16 & 255) * t)
                       / 255) << 16)
                   | ((((a >> 8 & 255) * (255 - t) + (b >> 8 & 255) * t)
                       / 255) << 8)
                   | (((a & 255) * (255 - t) + (b & 255) * t) / 255);
        for (int x = 0; x < MAP_LW; x++)
            fb[y * MAP_LW + x] = c;
    }
    for (int s = 0; s < 70; s++) {
        unsigned h = (unsigned)(s * 2654435761u);
        int x = (int)(h % MAP_LW), y = (int)((h >> 12) % MAP_LH);
        fb[y * MAP_LW + x] = (h >> 24) & 1 ? ll_menu_dac[121]
                                           : ll_menu_dac[107];
    }
    /* island blobs at each chapter anchor (green top, sand base);
     * rw/rh are the measured land-blob half-extents (fallback 60/26
     * in 640-space = 30/13 logical) */
    for (int i = 0; i < nchs; i++) {
        int cx = chs[i].ix / 2, cy = chs[i].iy / 2;
        int rw = chs[i].rw > 0 ? chs[i].rw / 2 : 30;
        int rh = chs[i].rh > 0 ? chs[i].rh / 2 : 13;
        if (rw < 2)
            rw = 2;
        if (rh < 2)
            rh = 2;
        for (int y = -rh; y <= rh; y++)
            for (int x = -rw; x <= rw; x++) {
                double d = (double)(x * x) / (rw * rw)
                           + (double)(y * y) / (rh * rh);
                if (d > 1.0)
                    continue;
                int px = cx + x, py = cy + y;
                if (px < 0 || px >= MAP_LW || py < 0 || py >= MAP_LH)
                    continue;
                Uint32 c = y < -rh / 3 ? ll_menu_dac[22] : ll_menu_dac[199];
                if (y > rh - (rh / 4 + 1))
                    c = ll_menu_dac[107];  /* waterline */
                fb[py * MAP_LW + px] = c;
            }
    }
}

int ll_menu_map_open(const ll_menu_ch *chs, int nchs,
                     const ll_menu_anim *anims, int nanims)
{
    if (nchs < 1)
        return -1;

    ll_seq *aseq = nanims > 0 ? calloc((size_t)nanims, sizeof *aseq) : NULL;
    int nanim_ok = 0;
    if (aseq) {
        for (int i = 0; i < nanims; i++) {
            if (ll_seq_parse(anims[i].seq, anims[i].len, &aseq[i]) != 0) {
                fprintf(stderr, "map: anim %d .SEQ parse failed\n", i);
                continue;
            }
            printf("map: anim %d: %u frames, period %ums\n",
                   i, aseq[i].count,
                   anims[i].period_ms ? anims[i].period_ms : 100);
            nanim_ok++;
        }
        if (!nanim_ok) {
            menu_anims_free(aseq, nanims);
            aseq = NULL;
        }
    }

    SDL_Window *win = SDL_CreateWindow("Lew Leon - chapter map",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED, MAP_W, MAP_H,
                                       SDL_WINDOW_SHOWN);
    if (!win) {
        menu_anims_free(aseq, nanims);
        return -2;
    }
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (!ren)
        ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888,
                                         SDL_TEXTUREACCESS_STREAMING,
                                         MAP_W, MAP_H);
    Uint32 *fb = (Uint32 *)malloc((size_t)MAP_LW * MAP_LH * sizeof *fb);
    unsigned char *cv = malloc((size_t)MAP_LW * MAP_LH);
    Uint32 *disp = (Uint32 *)malloc((size_t)MAP_W * MAP_H * sizeof *disp);
    if (!ren || !tex || !fb || !cv || !disp) {
        free(fb);
        free(cv);
        free(disp);
        if (tex) SDL_DestroyTexture(tex);
        if (ren) SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        menu_anims_free(aseq, nanims);
        return -2;
    }

    Uint32 pal[256];
    for (int i = 0; i < 256; i++)
        pal[i] = menu_lut(i);

    map_bg(fb, chs, nchs);              /* background is static */
    Uint32 *base = malloc((size_t)MAP_LW * MAP_LH * sizeof *base);
    if (base)
        memcpy(base, fb, (size_t)MAP_LW * MAP_LH * sizeof *base);

    int sel = 0, quit = 0, ret = -1;
    unsigned tick = 0;
    const char *sece = getenv("LEO_MENU_SECS");
    Uint64 t_deadline = sece ? SDL_GetTicks64() + 1000 * (Uint64)atof(sece) : 0;
    while (!quit) {
        if (base)
            memcpy(fb, base, (size_t)MAP_LW * MAP_LH * sizeof *base);

        /* .SEQ overlays composite at logical scale (the original authored
         * its sprite coords in the 320x200 space): anims with x>=0 draw at
         * their fixed origin (AUTORZY.SEQ @ (4,180) = the bottom-left
         * corner word); x<0 parks the BERLO arrow just left of the
         * selected island's measured edge, vertically centred */
        if (aseq) {
            memset(cv, 0xFF, (size_t)MAP_LW * MAP_LH);  /* 0xFF = keep bg */
            int irw = chs[sel].rw > 0 ? chs[sel].rw / 2 : 30;
            for (int a = 0; a < nanims; a++) {
                if (!aseq[a].els)
                    continue;
                unsigned per = anims[a].period_ms ? anims[a].period_ms : 100;
                unsigned fr = (unsigned)((Uint64)tick * 30 / per)
                              % aseq[a].count;
                const ll_seq_elem *e = &aseq[a].els[fr];
                int x = anims[a].x, y = anims[a].y;
                if (x < 0) {
                    x = chs[sel].ix / 2 - irw - 14;
                    y = chs[sel].iy / 2 - 9;
                }
                ll_seq_draw_elem(&aseq[a], e, cv, MAP_LW, MAP_LH, x, y);
            }
            for (int i = 0; i < MAP_LW * MAP_LH; i++)
                if (cv[i] != 0xFF)
                    fb[i] = pal[cv[i]];
        }

        /* corner words + chapter title at logical scale, colors from the
         * real menu DAC: idx15 green words, idx22 bright green title
         * (art bbox x50..342 y108..158 = logical 25..171 54..79).
         * AUTORZY is NOT font-drawn: it is the real AUTORZY.SEQ sprite
         * overlay (anim @ logical (4,180)). */
        draw_text1(fb, 5, 5, "WYJSCIE", ll_menu_dac[15]);
        draw_text1_r(fb, 315, 3, "RESTART", ll_menu_dac[15]);
        draw_text1_r(fb, 315, 187, "OPCJE", ll_menu_dac[15]);
        draw_text1(fb, 25, 62, chs[sel].title, ll_menu_dac[22]);

        /* presentation = the original's 2x2 CRTC doubling of the logical
         * canvas (VRAM capture: even-aligned block equality 1.000) */
        for (int y = 0; y < MAP_LH; y++) {
            Uint32 *d0 = disp + (size_t)(2 * y) * MAP_W;
            Uint32 *d1 = d0 + MAP_W;
            const Uint32 *s = fb + (size_t)y * MAP_LW;
            for (int x = 0; x < MAP_LW; x++) {
                d0[2 * x] = s[x];
                d0[2 * x + 1] = s[x];
                d1[2 * x] = s[x];
                d1[2 * x + 1] = s[x];
            }
        }

        {   /* CI hook: dump composed frame at LEO_MENU_PPM_TICK */
            const char *ppm_e = getenv("LEO_MENU_PPM");
            const char *tick_e = getenv("LEO_MENU_PPM_TICK");
            unsigned want = tick_e ? (unsigned)atoi(tick_e) : 0;
            if (ppm_e && tick == want)
                if (ppm_fb(ppm_e, disp, MAP_W, MAP_H) == 0)
                    printf("map: LEO_MENU_PPM wrote %s (tick %u)\n",
                           ppm_e, tick);
        }

        void *px;
        int pitch;
        if (SDL_LockTexture(tex, NULL, &px, &pitch) == 0) {
            for (int y = 0; y < MAP_H; y++)
                memcpy((char *)px + (size_t)y * pitch,
                       disp + (size_t)y * MAP_W, (size_t)MAP_W * 4);
            SDL_UnlockTexture(tex);
        }
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);

        SDL_Event e;
        Uint64 t_next = SDL_GetTicks64() + 30;
        while (!quit) {
            if (t_deadline && SDL_GetTicks64() >= t_deadline) {
                printf("map: LEO_MENU_SECS expired (auto-quit)\n");
                quit = 1;
                ret = -3;
                break;
            }
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    quit = 1;
                    ret = -2;
                } else if (e.type == SDL_KEYDOWN) {
                    SDL_Keycode k = e.key.keysym.sym;
                    if (k == SDLK_UP || k == SDLK_LEFT)
                        sel = (sel + nchs - 1) % nchs;
                    else if (k == SDLK_DOWN || k == SDLK_RIGHT)
                        sel = (sel + 1) % nchs;
                    else if (k == SDLK_RETURN || k == SDLK_KP_ENTER
                             || k == SDLK_SPACE) {
                        quit = 1;
                        ret = sel;
                    } else if (k == SDLK_ESCAPE || k == SDLK_q) {
                        printf("map: wyjście\n");
                        quit = 1;
                        ret = -2;
                    } else if (k == SDLK_a) {
                        printf("map: autorzy\n");
                        quit = 1;
                        ret = -4;
                    } else if (k == SDLK_o) {
                        printf("map: opcje\n");
                        quit = 1;
                        ret = -5;
                    } else if (k == SDLK_r) {
                        printf("map: restart\n");
                        quit = 1;
                        ret = -6;
                    }
                }
            }
            Sint64 left = (Sint64)(t_next - SDL_GetTicks64());
            ll_music_pump();
            if (left > 0)
                SDL_Delay((Uint32)left);
            else
                break;
        }
        tick++;
    }
    free(base);
    free(fb);
    free(cv);
    free(disp);
    menu_anims_free(aseq, nanims);
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    return ret;
}
