/* ll_level.c - shared level-scene composer (see ll_level.h).
 *
 * Resolves a level's plain-text wiring, then composites its gameplay scene
 * exactly the way the data says the game does it:
 *   background = first tlo= layer that exists in the level's ETAPY block
 *                (a .PCX: 8bpp indexed + embedded VGA palette);
 *   objects    = ANIMACJE/<level>/<LEAF> leaf records: each id=1 .SEQ is one
 *                object's frame-patch sprite script (ll_seq.c blitter);
 *   motion     = the matching HD-install .MOV state machine (same record
 *                key, e.g. ED000002.SEQ art <-> ED000002.MOV script):
 *                per state a frame list "<frame>,<period ms>[, M dx,dy]" -
 *                the game walks states in transition order, each frame line
 *                moves the object by its delta. The CD containers carry the
 *                .SEQ art, the SETUP-installed HD containers the .MOV text
 *                (found via LEO_HD_DIR=<dir with ANIMACJE/ETAPY>), never the
 *                other way round: keys join them 1:1.
 *   placement  = sub-.des files ("TYPE,name,mov,X,Y" lines) exist only for
 *                some levels; object X,Y join needs SETUP's mov->ED key
 *                mapping, still open (PLAN.md). Until then sprites anchor by
 *                their element header (x=|f0|, tall sprites bottom-aligned).
 * PCM payloads are parallax IMAGE layers (400-byte pitch chunked RLE,
 * FUN_00056b20) - NOT audio; the old "u8 audio" interpretation was wrong
 * and PCM geometry decode is pending (PLAN.md). They are listed, not played.
 */
#include "ll_level.h"
#include "ll_pcx.h"
#include "ll_pcm.h"
#include "ll_seq.h"
#include "llrs.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> /* strncasecmp for container record names */

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

/* ---------------- .MOV object state machine (HD-install payload) ---------
 * Plain text (verified 2026-11-14 on HD ANIMACJE ED000002.MOV):
 *   STATE1 \n STATE2 ... \n \n [STATE] \n control=BY_TIME \n
 *   NEXT(n)=ACTION \n seq=ed000002 \n <frame>,<ms>[, M dx,dy]* \n ...
 * Each frame line = show <frame> of seq=<key> for <ms>, then move M dx,dy.
 * ------------------------------------------------------------------------*/
#define MOV_MAXST 16
#define MOV_MAXFR 64

typedef struct { int frame; unsigned ms; int dx, dy; } mov_fr;
typedef struct { char name[24]; mov_fr fr[MOV_MAXFR]; int nfr; } mov_st;
typedef struct {
    mov_st st[MOV_MAXST];
    int nst;
    char seqkey[16];
} mov_t;

static int mov_parse(const char *txt, mov_t *m)
{
    memset(m, 0, sizeof *m);
    const char *p = txt;
    while (*p) {
        const char *eol = p;
        while (*eol && *eol != '\n' && *eol != '\r')
            eol++;
        size_t len = (size_t)(eol - p);
        char line[128];
        if (len && len < sizeof line) {
            memcpy(line, p, len);
            line[len] = '\0';
            if (line[0] == '[') {                /* [STATE] opens a block */
                if (m->nst < MOV_MAXST) {
                    mov_st *s = &m->st[m->nst++];
                    const char *gt = strchr(line, ']');
                    size_t nl = gt ? (size_t)(gt - line - 1) : 0;
                    if (nl >= sizeof s->name)
                        nl = sizeof s->name - 1;
                    memcpy(s->name, line + 1, nl);
                    s->name[nl] = '\0';
                }
            } else if (m->nst > 0 && strchr(line, '=')) {
                if (strncmp(line, "seq=", 4) == 0) {
                    const char *v = line + 4;
                    while (*v == ' ')
                        v++;
                    size_t k = 0;
                    while (v[k] && k + 1 < sizeof m->seqkey) {
                        m->seqkey[k] = (char)v[k];
                        k++;
                    }
                    m->seqkey[k] = '\0';
                }
                /* control=/transitions: states cycle in declaration order
                 * for the viewer, exact ACTION routing is gameplay work */
            } else if (m->nst > 0) {
                mov_st *s = &m->st[m->nst - 1];
                int f, ms;
                if (sscanf(line, "%d,%d", &f, &ms) == 2 &&
                    s->nfr < MOV_MAXFR) {
                    mov_fr *mf = &s->fr[s->nfr++];
                    mf->frame = f;
                    mf->ms = ms > 0 ? (unsigned)ms : 100u;
                    mf->dx = mf->dy = 0;
                    const char *mvt = strstr(line, "M ");
                    if (mvt)
                        sscanf(mvt, "M %d,%d", &mf->dx, &mf->dy);
                }
            }
        }
        p = eol;
        while (*p == '\n' || *p == '\r')
            p++;
    }
    return m->nst;
}

/* ---------------- scene objects ---------------------------------------- */

typedef struct {
    unsigned char *blob;      /* .SEQ payload (owned)                       */
    ll_seq seq;
    mov_t mov;                /* nst==0: cycle elements at 100 ms           */
    unsigned char *movtxt;    /* owned, or NULL                             */
    int eidx0;                /* first element with patch data              */
    int sw, sh;               /* sprite bbox size (from element chunks)     */
    int bx, by;               /* base canvas anchor                         */
    int x, y;                 /* current position (base + M deltas)         */
    int sti, fi;              /* state / frame cursor                       */
    unsigned t;               /* ms within current frame                    */
    char key[16], leaf[16];
} lvl_obj;

typedef struct { int maxrow; } row_count_ctx;

static void row_count_cb(void *ctx, int row, int gap,
                         const ll_seq_patch_op *ops, int nops)
{
    (void)gap; (void)ops; (void)nops;
    row_count_ctx *rc = ctx;
    if (row + 1 > rc->maxrow)
        rc->maxrow = row + 1;
}

/* sprite size of an element: chunks 0..arr_n-2 are 80 px wide, last is b15;
 * height = rows in the patch stream (ll_seq_patch_walk row counter). */
static void elem_size(const ll_seq *s, const ll_seq_elem *e, int *w, int *h)
{
    *w = 0;
    *h = 0;
    for (unsigned k = 0; k < e->arr_n; k++) {
        const ll_seq_ent *en = &s->ents[e->arr_first + k];
        *w += (k + 1 < e->arr_n) ? 80 : (int)(short)e->b15;
        row_count_ctx rc = { 0 };
        ll_seq_patch_walk(s->blob + en->off, en->len, row_count_cb, &rc);
        if (rc.maxrow > *h)
            *h = rc.maxrow;
    }
}

/* Anchor heuristic until the SETUP mov->ED key join lets us use the .des
 * X,Y placement: x = |f0|; f2 >= 0 is a world-below coordinate, so tall
 * level sprites bottom-align in the view (the MENU scripts all carry
 * negative f0/f2 and draw at exactly (-f0,-f2), see ll_menu.c). */
static void anchor_of(const ll_seq_elem *e, int sh, int ch, int *ax, int *ay)
{
    int ex = (int)(short)e->f0, ey = (int)(short)e->f2;
    *ax = ex < 0 ? -ex : ex;
    if (ey < 0)
        *ay = -ey;
    else
        *ay = sh < ch ? ch - sh : 0;
}

static void obj_advance(lvl_obj *o, unsigned dt_ms)
{
    o->t += dt_ms;
    if (!o->mov.nst) {                        /* no script: cycle elements */
        int n = (int)o->seq.count;
        if (n <= 0)
            return;
        while (o->t >= 100) {
            o->t -= 100;
            o->fi = (o->fi + 1) % n;
        }
        return;
    }
    for (;;) {
        mov_st *s = &o->mov.st[o->sti];
        if (s->nfr == 0 || o->fi >= s->nfr) { /* state end -> next state    */
            o->sti = (o->sti + 1) % o->mov.nst;
            o->fi = 0;
            o->x = o->bx;
            o->y = o->by;                     /* motion deltas are per state */
            s = &o->mov.st[o->sti];
            if (s->nfr == 0)
                break;
        }
        mov_fr *f = &s->fr[o->fi];
        if (o->t < f->ms)
            break;
        o->t -= f->ms;
        o->x += f->dx;                        /* "M dx,dy" of the frame just  */
        o->y += f->dy;                        /* held                        */
        o->fi++;
    }
}

/* current drawable element of an object (frame index from .MOV, clamped) */
static const ll_seq_elem *obj_elem(const lvl_obj *o)
{
    if (o->seq.count == 0)
        return NULL;
    unsigned idx = (unsigned)o->eidx0;
    if (o->mov.nst) {
        idx = (unsigned)o->mov.st[o->sti].fr[o->fi].frame;
        if ((int)idx >= (int)o->seq.count)
            idx = o->seq.count - 1;
    } else if (o->fi >= 0) {
        idx = (unsigned)o->fi % o->seq.count;
    }
    return &o->seq.els[idx];
}

/* find "<stem>.<ext>" (case-insensitive) in a block record list */
static const llrs_slot *rec_find(const llrs_slot *recs, unsigned n,
                                 const char *stem, const char *ext)
{
    for (unsigned k = 0; k < n; k++) {
        if (recs[k].id != 1 || !recs[k].name[0])
            continue;
        char st[16];
        stem_of(recs[k].name, st, sizeof st);
        const char *dot = strrchr(recs[k].name, '.');
        if (ci_eq(st, stem) && dot && ci_eq(dot, ext))
            return &recs[k];
    }
    return NULL;
}

/* PPM dump of the composited scene (CI hook, LEO_LEVEL_PPM) */
static void scene_ppm(const char *path, int w, int h, const unsigned char *idx,
                      const unsigned char pal[768])
{
    FILE *f = fopen(path, "wb");
    if (!f) {
        fprintf(stderr, "ll_level: cannot write %s\n", path);
        return;
    }
    fprintf(f, "P6\n%d %d\n255\n", w, h);
    for (int i = 0; i < w * h; i++) {
        unsigned p = idx[i] * 3;
        fputc(pal[p], f);
        fputc(pal[p + 1], f);
        fputc(pal[p + 2], f);
    }
    fclose(f);
    printf("LEO_LEVEL_PPM wrote %s\n", path);
}

/* paint one 8bpp canvas through the palette LUT (shared present path) */
static void paint(SDL_Renderer *ren, SDL_Texture *tex,
                  const unsigned char *canvas, const Uint32 lut[256],
                  int w, int h)
{
    void *px;
    int pitch;
    if (SDL_LockTexture(tex, NULL, &px, &pitch) == 0) {
        for (int y = 0; y < h; y++) {
            Uint32 *row = (Uint32 *)((char *)px + (size_t)y * pitch);
            const unsigned char *sp = canvas + (size_t)y * w;
            for (int x = 0; x < w; x++)
                row[x] = lut[sp[x]];
        }
        SDL_UnlockTexture(tex);
    }
    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, tex, NULL, NULL);
    SDL_RenderPresent(ren);
}

/* ---------------- level help overlay (original POMOC flow) --------------
 * POMOC1.MNU (HD-install ETAPY block) wires the help: static=x,y,pomocNN.seq
 * lines list page art (text baked into .SEQ frame patches); the .SEQ
 * payloads themselves ship in the CD ETAPY level block (incomplete CD:
 * only some pages present). Left/Right/Enter cycle pages, ESC closes, and
 * LEO_HELP_SECS (default 3 s) auto-closes for CI/flow; the scene window
 * starts when the help overlay closes.
 * cursor=mouse.seq / palette=<stem>.pal refer to payloads absent from both
 * trees (documented data hole) - bg palette is kept.                   */
typedef struct { int x, y; ll_seq seq; unsigned char *blob; } help_page;

static void help_pages(SDL_Renderer *ren, SDL_Texture *tex,
                       const llrs *hd, const llrs_slot *hdrecs, unsigned nhdrec,
                       const llrs *cd, const llrs_slot *cdrecs, unsigned ncdrec,
                       const unsigned char *bg, unsigned char *canvas,
                       const Uint32 lut[256], int w, int h)
{
    const llrs_slot *mnu = NULL;
    for (unsigned k = 0; k < nhdrec; k++) {
        const char *dot = strrchr(hdrecs[k].name, '.');
        if (hdrecs[k].id == 1 && dot && ci_eq(dot, ".MNU")) {
            mnu = &hdrecs[k];
            break;
        }
    }
    if (!mnu)
        return;
    unsigned char *raw = NULL;
    if (llrs_read(hd, mnu, &raw) < 0)
        return;
    help_page pages[16];
    char mnu_seq[16][16];
    int nms = 0;
    int np = 0;
    /* line scan (repeated "static=" keys - a key=table would collapse them) */
    const char *p = (const char *)raw;
    while (*p) {
        const char *eol = p;
        while (*eol && *eol != '\n' && *eol != '\r')
            eol++;
        if (eol - p > 7 && strncmp(p, "static=", 7) == 0) {
            int x, y;
            char seq[16] = "";
            if (sscanf(p + 7, "%d,%d,%15[a-zA-Z0-9.]", &x, &y, seq) == 3) {
                if (nms < 16)
                    snprintf(mnu_seq[nms++], sizeof mnu_seq[0], "%s", seq);
                const llrs_slot *ps = NULL;
                for (unsigned j = 0; j < ncdrec; j++)
                    if (cdrecs[j].id == 1 && ci_eq(cdrecs[j].name, seq)) {
                        ps = &cdrecs[j];
                        break;
                    }
                if (ps && np < 16) {  /* other pages absent from this CD */
                    pages[np].blob = NULL;
                    if (llrs_read(cd, ps, &pages[np].blob) >= 0 &&
                        ll_seq_parse(pages[np].blob, (uint32_t)ps->size,
                                     &pages[np].seq) == 0) {
                        const ll_seq_elem *fe = NULL;
                        for (unsigned el = 0; el < pages[np].seq.count; el++)
                            if (pages[np].seq.els[el].arr_n > 0) {
                                fe = &pages[np].seq.els[el];
                                break;
                            }
                        /* MNU x,y are placeholders (0,0) here; the .SEQ
                         * element header carries the real canvas anchor,
                         * screen pos = (-f0,-f2), like the menu leaves. */
                        if (fe) {
                            int ex = -(int)(short)fe->f0;
                            int ey = -(int)(short)fe->f2;
                            if (ex > 0 || ey > 0) {
                                x = ex > 0 ? ex : x;
                                y = ey > 0 ? ey : y;
                            }
                        }
                        pages[np].x = x;
                        pages[np].y = y;
                        printf("ll_level: help: %s page %d %s at (%d,%d)\n",
                               mnu->name, np, seq, x, y);
                        np++;
                    } else {
                        free(pages[np].blob);
                        pages[np].blob = NULL;
                    }
                }
            }
        }
        p = eol;
        while (*p == '\n' || *p == '\r')
            p++;
    }
    /* This CD's .MNU static= list is the generic set (pomoc00..pomoc10);
     * per-level help art is keyed by page number instead (TUNGA.11 ships
     * POMOC32.SEQ), so also collect POMOC<NN>.SEQ payloads present in the
     * CD block that the MNU does not name (deterministic numeric order). */
    for (unsigned j = 0; j < ncdrec && np < 16; j++) {
        const char *nm = cdrecs[j].name;
        if (cdrecs[j].id != 1)
            continue;
        char stem[16];
        stem_of(nm, stem, sizeof stem);
        if (strncasecmp(stem, "POMOC", 5) != 0)
            continue;
        int dup = 0;
        for (int k = 0; k < nms; k++)
            if (ci_eq(mnu_seq[k], stem) || ci_eq(mnu_seq[k], nm)) {
                dup = 1;
                break;
            }
        if (dup)
            continue;
        pages[np].blob = NULL;
        if (llrs_read(cd, &cdrecs[j], &pages[np].blob) < 0 ||
            ll_seq_parse(pages[np].blob, (uint32_t)cdrecs[j].size,
                         &pages[np].seq) != 0) {
            free(pages[np].blob);
            pages[np].blob = NULL;
            continue;
        }
        const ll_seq_elem *fe = NULL;
        for (unsigned el = 0; el < pages[np].seq.count; el++)
            if (pages[np].seq.els[el].arr_n > 0) {
                fe = &pages[np].seq.els[el];
                break;
            }
        pages[np].x = pages[np].y = 0;
        if (fe) {
            int ex = -(int)(short)fe->f0, ey = -(int)(short)fe->f2;
            if (ex > 0)
                pages[np].x = ex;
            if (ey > 0)
                pages[np].y = ey;
        }
        printf("ll_level: help: %s page %d %s at (%d,%d) [CD]\n",
               mnu->name, np, nm, pages[np].x, pages[np].y);
        np++;
    }
    free(raw);
    if (!np)
        return;

    const char *hs = getenv("LEO_HELP_SECS");
    double lim = hs ? atof(hs) : 3.0;    /* scene timer starts after help */
    Uint64 freq = SDL_GetPerformanceFrequency();
    Uint64 t_end = SDL_GetPerformanceCounter() +
                   (Uint64)(lim * (double)freq);
    int page = 0, quit = 0;
    while (!quit && SDL_GetPerformanceCounter() < t_end) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = 2;
            else if (e.type == SDL_KEYDOWN) {
                SDL_Keycode k = e.key.keysym.sym;
                if (k == SDLK_ESCAPE)
                    quit = 1;
                else if (k == SDLK_RIGHT || k == SDLK_RETURN ||
                         k == SDLK_SPACE || k == SDLK_PAGEDOWN)
                    page = (page + 1) % np;
                else if (k == SDLK_LEFT || k == SDLK_PAGEUP)
                    page = (page + np - 1) % np;
            }
        }
        if (quit)
            break;
        memcpy(canvas, bg, (size_t)w * (size_t)h);
        const ll_seq *s = &pages[page].seq;
        for (unsigned el = 0; el < s->count; el++)
            if (s->els[el].arr_n > 0) {
                ll_seq_draw_elem(s, &s->els[el], canvas, w, h,
                                 pages[page].x, pages[page].y);
                break;
            }
        paint(ren, tex, canvas, lut, w, h);
        const char *hppm = getenv("LEO_HELP_PPM"); /* CI: help frame dump */
        static int hdumped = 0;
        if (hppm && !hdumped) {
            hdumped = 1;
            unsigned char pal[768];
            for (int i = 0; i < 256; i++) {
                pal[3 * i] = (unsigned char)(lut[i] >> 16);
                pal[3 * i + 1] = (unsigned char)(lut[i] >> 8);
                pal[3 * i + 2] = (unsigned char)lut[i];
            }
            scene_ppm(hppm, w, h, canvas, pal);
        }
        SDL_Delay(20);
    }
    for (int k = 0; k < np; k++) {
        ll_seq_free(&pages[k].seq);
        free(pages[k].blob);
    }
    if (quit == 1)
        printf("ll_level: help: ESC\n");
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

    /* 1) main .DES: exact name, "<level>.DES", stem ".DES", then a scan of
     *    all *.DES payloads matching their own dir= value (TUNGA.11 is
     *    carried by TUNGA1.DES: dir=tunga.11). */
    const llrs_slot *des = llrs_find(&leveldes, level);
    if (!des) {
        char cand[32];
        snprintf(cand, sizeof cand, "%s.DES", level);
        des = llrs_find(&leveldes, cand);
    }
    if (!des) {
        char stem[16];
        stem_of(level, stem, sizeof stem);
        char cand[32];
        snprintf(cand, sizeof cand, "%s.DES", stem);
        des = llrs_find(&leveldes, cand);
    }
    unsigned char *txt = NULL;
    if (des && llrs_read(&leveldes, des, &txt) < 0)
        txt = NULL;
    if (!txt) {                       /* dir= scan over every .DES payload */
        for (unsigned k = 0; k < leveldes.cap && !txt; k++) {
            const llrs_slot *s = &leveldes.slots[k];
            if (s->id != 1 || !s->name[0])
                continue;
            const char *dot = strrchr(s->name, '.');
            if (!dot || !ci_eq(dot, ".DES"))
                continue;
            unsigned char *cand = NULL;
            if (llrs_read(&leveldes, s, &cand) < 0)
                continue;
            des_kv tmp[8];
            int nt = des_parse((char *)cand, tmp, 8);
            if (nt && ci_eq(des_get(tmp, nt, "dir"), level)) {
                txt = cand;
                des = s;
            } else
                free(cand);
        }
    }
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

    /* 3) background: first tlo= layer present in the block (.PCX), else any
     *    .PCX in the block (KADMAR2's M15.PCX etc. are intro art). */
    char bgname[32] = "";
    const llrs_slot *bg = NULL;
    {
        char tlo[512];
        snprintf(tlo, sizeof tlo, "%s", des_get(kvs, nk, "tlo"));
        char *save = NULL;
        for (char *tok = strtok_r(tlo, ",", &save); tok && !bg;
             tok = strtok_r(NULL, ",", &save)) {
            while (*tok == ' ')
                tok++;
            const char *dot = strrchr(tok, '.');
            if (!dot || !ci_eq(dot, ".PCX"))
                continue;
            char stem[16];
            stem_of(tok, stem, sizeof stem);
            for (unsigned k = 0; k < nrec; k++) {
                char st2[16];
                stem_of(recs[k].name, st2, sizeof st2);
                if (recs[k].id == 1 && ci_eq(st2, stem)) {
                    bg = &recs[k];
                    break;
                }
            }
        }
    }
    if (!bg)
        for (unsigned k = 0; k < nrec; k++) {
            const char *dot = strrchr(recs[k].name, '.');
            if (recs[k].id == 1 && dot && ci_eq(dot, ".PCX")) {
                bg = &recs[k];
                break;
            }
        }
    if (bg)
        snprintf(bgname, sizeof bgname, "%s", bg->name);

    ll_pcx img;
    memset(&img, 0, sizeof img);
    int esc = 0;
    if (bg) {
        unsigned char *raw = NULL;
        if (llrs_read(&etapy, bg, &raw) >= 0) {
            if (ll_pcx_decode(raw, (size_t)bg->size, &img) != 0)
                printf("ll_level: PCX decode failed for %s\n", bg->name);
            free(raw);
        }
    }
    for (unsigned k = 0; k < nrec; k++) {
        const char *dot = strrchr(recs[k].name, '.');
        if (recs[k].id == 1 && dot && ci_eq(dot, ".PCM")) {
            unsigned char *raw = NULL;
            if (llrs_read(&etapy, &recs[k], &raw) >= 0) {
                ll_pcm pcml;
                if (ll_pcm_decode(raw, (size_t)recs[k].size, &pcml) == 0) {
                    printf("ll_level: PCM %s: parallax layer %ux%u rows"
                           " (window %u rows, stride %u); tiling pending\n",
                           recs[k].name, pcml.w, pcml.rows, pcml.win_rows,
                           pcml.stride);
                    ll_pcm_release(&pcml);
                } else
                    printf("ll_level: PCM %s: decode failed\n", recs[k].name);
                free(raw);
            }
        }
    }
    if (!img.idx) {
        printf("ll_level: no background PCX for '%s' "
               "(wiring wants '%s' - missing from these containers)\n",
               level, des_get(kvs, nk, "tlo"));
        llrs_block_free(recs);
        free(txt);
        llrs_close(&leveldes);
        llrs_close(&etapy);
        return 1;
    }

    /* 4) objects: ANIMACJE/<level> leaf .SEQ sprites + HD-install .MOV
     *    state machines (LEO_HD_DIR); missing HD is fine (static sprites).*/
    lvl_obj objs[24];
    int nobj = 0;
    llrs an;
    memset(objs, 0, sizeof objs);
    snprintf(p, sizeof p, "%s/ANIMACJE", dir);
    llrs hd_an;
    int have_hd = 0;
    const char *hd = getenv("LEO_HD_DIR");
    if (hd) {
        char hp[4096];
        snprintf(hp, sizeof hp, "%s/ANIMACJE", hd);
        have_hd = llrs_open(&hd_an, hp) == 0;
    }
    const llrs_slot *hblk = NULL;
    llrs_slot *hrecs = NULL;
    unsigned nhrec = 0;
    if (have_hd) {
        hblk = find_or_stem(&hd_an, level);
        if (hblk)
            llrs_block(&hd_an, hblk, &hrecs, &nhrec);
    }
    if (llrs_open(&an, p) == 0) {
        const llrs_slot *ablk = find_or_stem(&an, level);
        llrs_slot *arecs = NULL;
        unsigned narec = 0;
        if (ablk && llrs_block(&an, ablk, &arecs, &narec) == 0) {
            for (unsigned k = 0; k < narec && nobj < 24; k++) {
                if (arecs[k].id != 2 || !arecs[k].name[0])
                    continue;
                llrs_slot *fr = NULL;
                unsigned nfr = 0;
                if (llrs_block(&an, &arecs[k], &fr, &nfr))
                    continue;
                for (unsigned j = 0; j < nfr && nobj < 24; j++) {
                    const char *dot = strrchr(fr[j].name, '.');
                    if (fr[j].id != 1 || !fr[j].name[0] || !dot ||
                        !ci_eq(dot, ".SEQ"))
                        continue;
                    lvl_obj *o = &objs[nobj];
                    o->blob = NULL;
                    if (llrs_read(&an, &fr[j], &o->blob) < 0 ||
                        ll_seq_parse(o->blob, (uint32_t)fr[j].size,
                                     &o->seq) != 0) {
                        free(o->blob);
                        o->blob = NULL;
                        continue;
                    }
                    int first = -1;
                    for (unsigned e = 0; e < o->seq.count; e++)
                        if (o->seq.els[e].arr_n > 0) {
                            first = (int)e;
                            break;
                        }
                    if (first < 0) {
                        ll_seq_free(&o->seq);
                        free(o->blob);
                        o->blob = NULL;
                        continue;
                    }
                    o->eidx0 = first;
                    snprintf(o->key, sizeof o->key, "%s", fr[j].name);
                    snprintf(o->leaf, sizeof o->leaf, "%s", arecs[k].name);
                    elem_size(&o->seq, &o->seq.els[first], &o->sw, &o->sh);
                    anchor_of(&o->seq.els[first], o->sh, (int)img.h,
                              &o->bx, &o->by);
                    o->x = o->bx;
                    o->y = o->by;

                    /* matching .MOV from the HD install (same record key) */
                    if (hrecs) {
                        const llrs_slot *hleaf = NULL;
                        for (unsigned m = 0; m < nhrec; m++)
                            if (hrecs[m].id == 2 && hrecs[m].name[0] &&
                                ci_eq(hrecs[m].name, arecs[k].name))
                                hleaf = &hrecs[m];
                        if (hleaf) {
                            llrs_slot *mrecs = NULL;
                            unsigned nmrec = 0;
                            if (llrs_block(&hd_an, hleaf, &mrecs,
                                           &nmrec) == 0) {
                                char stem[16];
                                stem_of(fr[j].name, stem, sizeof stem);
                                const llrs_slot *mv = rec_find(mrecs, nmrec,
                                                               stem, ".MOV");
                                if (mv &&
                                    llrs_read(&hd_an, mv, &o->movtxt) > 0)
                                    mov_parse((char *)o->movtxt, &o->mov);
                                llrs_block_free(mrecs);
                            }
                        }
                    }
                    nobj++;
                }
                llrs_block_free(fr);
            }
            llrs_block_free(arecs);
        }
        llrs_close(&an);
    }
    if (hrecs)
        llrs_block_free(hrecs);
    if (have_hd)
        llrs_close(&hd_an);

    /* One visible script per leaf object: the game shows one .SEQ per object
     * (its .MOV state machine names it via seq=), so collapse each leaf
     * group to the script the HD .MOV references, else the first one.
     * (Objects were collected grouped by leaf.) */
    for (int a = 0; a < nobj;) {
        int b = a;
        while (b < nobj && !strcmp(objs[b].leaf, objs[a].leaf))
            b++;
        int keep = a;
        for (int m = a; m < b; m++) {
            char stem[16];
            stem_of(objs[m].key, stem, sizeof stem);
            if (objs[m].mov.nst && ci_eq(stem, objs[m].mov.seqkey)) {
                keep = m;
                break;
            }
        }
        if (keep != a) {
            lvl_obj tmp = objs[a];
            objs[a] = objs[keep];
            objs[keep] = tmp;
        }
        for (int m = a + 1; m < b; m++) {
            free(objs[m].movtxt);
            free(objs[m].blob);
        }
        if (b > a + 1)
            memmove(&objs[a + 1], &objs[b],
                    sizeof(lvl_obj) * (size_t)(nobj - b));
        nobj -= b - a - 1;
        a++;
    }

    printf("ll_level: show %s: bg %s %ux%u, %d animated object(s)\n", level,
           bgname, img.w, img.h, nobj);
    for (int k = 0; k < nobj; k++)
        printf("ll_level: obj %s/%s: %dx%d at (%d,%d)%s\n", objs[k].leaf,
               objs[k].key, objs[k].sw, objs[k].sh, objs[k].x, objs[k].y,
               objs[k].mov.nst ? " [mov]" : "");

    /* 5) composite canvas + present loop */
    size_t csz = (size_t)img.w * img.h;
    unsigned char *canvas = malloc(csz);
    if (!canvas)
        return 1;
    memcpy(canvas, img.idx, csz);
    for (int k = 0; k < nobj; k++) {
        const ll_seq_elem *e = obj_elem(&objs[k]);
        if (e)
            ll_seq_draw_elem(&objs[k].seq, e, canvas, (int)img.w, (int)img.h,
                             objs[k].x, objs[k].y);
    }

    const char *ppm = getenv("LEO_LEVEL_PPM");   /* CI: headless dump */
    if (ppm) {
        int ticks = getenv("LEO_LEVEL_PPM_TICK")
                        ? atoi(getenv("LEO_LEVEL_PPM_TICK"))
                        : 0;
        for (int k = 0; k < nobj; k++) {   /* advance fixed-step then redraw */
            for (int m = 0; m < ticks; m++)
                obj_advance(&objs[k], 40);
            printf("ll_level: tick %d: %s state %d at (%d,%d)\n", ticks,
                   objs[k].key, objs[k].sti, objs[k].x, objs[k].y);
        }
        memcpy(canvas, img.idx, csz);      /* same order as the live loop */
        for (int k = 0; k < nobj; k++) {
            const ll_seq_elem *e = obj_elem(&objs[k]);
            if (e)
                ll_seq_draw_elem(&objs[k].seq, e, canvas, (int)img.w,
                                 (int)img.h, objs[k].x, objs[k].y);
        }
        scene_ppm(ppm, (int)img.w, (int)img.h, canvas, img.pal);
        free(canvas);
        goto done;
    }

    {
        SDL_Window *win = NULL;
        SDL_Renderer *ren = NULL;
        int sc = img.w < 640 ? 2 : 1;
        win = SDL_CreateWindow("Lew Leon - level scene",
                               SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               (int)img.w * sc, (int)img.h * sc,
                               SDL_WINDOW_SHOWN);
        if (win) {
            ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
            if (!ren)
                ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
        }
        if (!ren) {
            fprintf(stderr, "ll_level: no renderer: %s\n", SDL_GetError());
            free(canvas);
            goto done;
        }
        SDL_Texture *tex = SDL_CreateTexture(
            ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
            (int)img.w, (int)img.h);
        Uint32 lut[256];
        for (int i = 0; i < 256; i++)
            lut[i] = 0xFF000000u | ((Uint32)img.pal[3 * i] << 16) |
                     ((Uint32)img.pal[3 * i + 1] << 8) | img.pal[3 * i + 2];

        /* 5b) help overlay first, like the original POMOC flow: the HD
         *     level block's POMOC1.MNU names page art (.SEQ) that ships in
         *     the CD level block; ESC closes, Left/Right page through. */
        if (hd) {
            char hp[4096];
            snprintf(hp, sizeof hp, "%s/ETAPY", hd);
            llrs hd_ety;
            if (llrs_open(&hd_ety, hp) == 0) {
                const llrs_slot *hblk2 = find_or_stem(&hd_ety, level);
                if (!hblk2 && nk) {
                    const char *d = des_get(kvs, nk, "dir");
                    if (d[0])
                        hblk2 = find_or_stem(&hd_ety, d);
                }
                llrs_slot *hrecs2 = NULL;
                unsigned nhrec2 = 0;
                if (hblk2)
                    llrs_block(&hd_ety, hblk2, &hrecs2, &nhrec2);
                if (nhrec2)
                    help_pages(ren, tex, &hd_ety, hrecs2, nhrec2,
                               &etapy, recs, nrec, img.idx, canvas, lut,
                               (int)img.w, (int)img.h);
                llrs_block_free(hrecs2);
                llrs_close(&hd_ety);
            }
        }

        Uint64 freq = SDL_GetPerformanceFrequency();
        Uint64 t_end = SDL_GetPerformanceCounter() +
                       (Uint64)(secs * (double)freq);
        Uint64 t_prev = SDL_GetPerformanceCounter();
        while (!esc && SDL_GetPerformanceCounter() < t_end) {
            SDL_Event e;
            while (SDL_PollEvent(&e))
                if (e.type == SDL_QUIT ||
                    (e.type == SDL_KEYDOWN &&
                     (e.key.keysym.sym == SDLK_ESCAPE ||
                      e.key.keysym.sym == SDLK_q)))
                    esc = 1;
            Uint64 now = SDL_GetPerformanceCounter();
            unsigned dt = (unsigned)((now - t_prev) * 1000 / (double)freq);
            t_prev = now;
            if (dt > 200)
                dt = 200;
            for (int k = 0; k < nobj; k++)
                obj_advance(&objs[k], dt);
            memcpy(canvas, img.idx, csz);
            for (int k = 0; k < nobj; k++) {
                const ll_seq_elem *el = obj_elem(&objs[k]);
                if (el)
                    ll_seq_draw_elem(&objs[k].seq, el, canvas, (int)img.w,
                                     (int)img.h, objs[k].x, objs[k].y);
            }
            paint(ren, tex, canvas, lut, (int)img.w, (int)img.h);
            SDL_Delay(20);
        }
        if (tex)
            SDL_DestroyTexture(tex);
        if (ren)
            SDL_DestroyRenderer(ren);
        if (win)
            SDL_DestroyWindow(win);
    }
    if (esc)
        printf("ll_level: ESC -> advance\n");
    free(canvas);

done:
    for (int k = 0; k < nobj; k++) {
        free(objs[k].movtxt);
        free(objs[k].blob);
    }
    ll_pcx_free(&img);
    llrs_block_free(recs);
    free(txt);
    llrs_close(&leveldes);
    llrs_close(&etapy);
    (void)audio;
    return 0;
}
