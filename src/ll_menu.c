/* ll_pcx-style present path (palette LUT into locked ARGB8888 texture).
 * Menu text: built-in 8x8 bitmap font (ASCII 32..126), drawn as 1px white;
 * cursor '>' in gold. Optional .SEQ overlay animations (ll_seq_patch_draw
 * into an 8bpp index canvas over the PCX indices, then one palette LUT).
 * Fixed pacing: event-driven with SDL_Delay, no busy loop. See ll_menu.h. */
#include "ll_menu.h"
#include "ll_pcx.h"
#include "ll_seq.h"

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
