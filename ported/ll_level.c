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
 * PCM payloads are parallax IMAGE layers (one continuous byte-RLE plane,
 * FUN_00056b20) - NOT audio; the old "u8 audio" interpretation was wrong.
 * They join the scene as the tlo= background stack: when a level has no
 * .PCX among its tlo= layers (RAN.01: plan1.pcm only on this CD), the
 * 400x200 rest view (ll_pcm_view) becomes the scene canvas.
 */
#include "ll_level.h"
#include "fs_ci.h"
#include "ll_pcx.h"
#include "ll_pcm.h"
#include "ll_seq.h"
#include "llrs.h"
#include "ll_mod.h"
#include "ll_view.h"

#include <SDL2/SDL.h>

/* SDL audio for music.  IMPORTANT (dev desktop, 2026-11-14): SDL2's
 * callback delivery is corrupt on this system with EVERY backend
 * (pipewire/pulse squeeze or go silent; even alsa squeaks at 1024
 * samples and dies at 2048+), while SDL_QueueAudio on the alsa backend
 * plays byte-perfect.  So music never uses want.callback: the device is
 * opened silent and the queue is topped by a dedicated pump THREAD
 * (review E-09 / tips T-09) - keeping ~250 ms queued independently of
 * the UI loops, because window drags (Windows modal loop), slow loads
 * and SDL_Delays in the screens underran the old in-loop pump. */

/* Initialize the audio subsystem, dodging SDL2 >= 2.32's native
 * "pipewire" backend, which on the dev desktop (SDL 2.32.74 + PipeWire,
 * 2026-11-14) delivered squeaking or silent streams for BOTH the
 * callback and QueueAudio paths while the alsa backend played the very
 * same bytes clean.  Only fires when SDL would auto-pick pipewire, so
 * Windows (directsound) and older/user-pinned setups are untouched;
 * LEO/SDL_AUDIODRIVER in the environment always wins. */
int ll_audio_init(void)
{
    if (SDL_WasInit(SDL_INIT_AUDIO))
        return 0;
    if (SDL_getenv("SDL_AUDIODRIVER"))
        return SDL_InitSubSystem(SDL_INIT_AUDIO) == 0 ? 0 : -1;
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
        return -1;
    const char *drv = SDL_GetCurrentAudioDriver();
    if (drv && strcmp(drv, "pipewire") == 0) {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        SDL_setenv("SDL_AUDIODRIVER", "alsa", 1);
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) == 0) {
            printf("ll_audio: SDL pipewire backend unreliable, "
                   "audio on alsa driver\n");
            return 0;
        }
        SDL_setenv("SDL_AUDIODRIVER", drv, 1);   /* restore, alsa absent */
        return SDL_InitSubSystem(SDL_INIT_AUDIO) == 0 ? 0 : -1;
    }
    return 0;
}

/* ---- standalone music playback (menu/map screens), singleton ---- *
 * Engine anchor: FUN_0005dec0 opens+plays "mapka.mod" for the MAPKA
 * (level-select) screen; FUN_0005df20 stops it on every exit path.
 * Same SOUND container + ll_mod player as the level path. */
static llrs s_mus_snd;
static unsigned char *s_mus_img;
static ll_mod s_mus;
static ll_mod_state s_mst;
static SDL_AudioDeviceID s_mus_dev;
static int s_mus_snd_open;
static FILE *s_mus_wf;                 /* LEO_MUSIC_WAV diagnostic dump */
static char s_mus_name[32];
static SDL_Thread *s_mus_thr;          /* E-09 dedicated pump thread    */
static SDL_atomic_t s_mus_quit;

/* E-09: while it lives this thread owns s_mst/s_mus_wf/s_mus_dev
 * (ll_music_stop joins it before touching any of them, ll_music_play
 * stops first), so no lock is needed. Queue kept >= ~250 ms with 125 ms
 * chunks (old in-loop low-water was 12000 B ~= 68 ms, review E-09). */
static int SDLCALL music_pump_thread(void *unused)
{
    (void)unused;
    static int16_t buf[(LL_MOD_RATE / 8) * 2];      /* 125 ms stereo s16 */
    const Uint32 low_water = (Uint32)LL_MOD_RATE / 4 * 4; /* 250 ms bytes */
    while (!SDL_AtomicGet(&s_mus_quit)) {
        if (s_mus_dev &&
            SDL_GetQueuedAudioSize(s_mus_dev) < low_water) {
            size_t frames = LL_MOD_RATE / 8;
            ll_mod_mix(&s_mst, buf, frames);
            if (s_mus_wf) {
                fwrite(buf, 2 * sizeof(int16_t), frames, s_mus_wf);
                fflush(s_mus_wf);       /* CI hook: LEO_MUSIC_WAV stays
                                         * tail-able while the game runs */
            }
            SDL_QueueAudio(s_mus_dev, buf,
                           (Uint32)(frames * 2 * sizeof(int16_t)));
        }
        SDL_Delay(25);                  /* first pass runs immediately,
                                         * so even very short plays get
                                         * their first chunk */
    }
    return 0;
}

/* first 125 ms chunk mixed+queued synchronously (play context): the
 * thread start races ll_music_stop in very short plays, and the old
 * in-loop pump always produced its first chunk before anything else
 * could touch the state - LEO_MUSIC_WAV consumers rely on >=1 chunk
 * per play. */
static void music_first_chunk(void)
{
    static int16_t buf[(LL_MOD_RATE / 8) * 2];
    size_t frames = LL_MOD_RATE / 8;
    ll_mod_mix(&s_mst, buf, frames);
    if (s_mus_wf) {
        fwrite(buf, 2 * sizeof(int16_t), frames, s_mus_wf);
        fflush(s_mus_wf);
    }
    if (s_mus_dev)
        SDL_QueueAudio(s_mus_dev, buf,
                       (Uint32)(frames * 2 * sizeof(int16_t)));
}

/* the LEO_MUSIC_WAV diagnostic: one raw file per play */
static void music_wav_open(void)
{
    static unsigned seq;
    char path[512];
    snprintf(path, sizeof path, "%s.%02u.%s.raw",
             getenv("LEO_MUSIC_WAV"), seq++, s_mus_name);
    s_mus_wf = ll_fopen_ci(path, "wb");
}

int ll_music_play(const char *data_dir, const char *modname)
{
    if (getenv("LEO_NO_MUSIC"))
        return 1;
    ll_music_stop();
    snprintf(s_mus_name, sizeof s_mus_name, "%s", modname);
    char p[4096];
    snprintf(p, sizeof p, "%s/SOUND", data_dir);
    if (llrs_open(&s_mus_snd, p) != 0)
        return 1;
    s_mus_snd_open = 1;
    const llrs_slot *sl = llrs_find(&s_mus_snd, modname);
    long n = sl ? llrs_read(&s_mus_snd, sl, &s_mus_img) : -1;
    char err[96] = "";
    if (n <= 0 || ll_mod_open(&s_mus, s_mus_img, (size_t)n, err,
                              sizeof err) != 0) {
        printf("ll_music: %s: %s\n", modname, err[0] ? err : "missing");
        ll_music_stop();
        return 1;
    }
    ll_mod_start(&s_mst, &s_mus);
    if (ll_audio_init() != 0) {
        ll_music_stop();
        return 1;
    }
    SDL_AudioSpec want, have;
    memset(&want, 0, sizeof want);
    want.freq = LL_MOD_RATE;
    want.format = AUDIO_S16SYS;
    want.channels = 2;
    /* want.samples MUST stay 0 here: any explicit small buffer (1024,
     * 2048) makes SDL2.32's alsa backend deliver squeaking or silent
     * streams on the dev desktop, while SDL's own default (reported
     * back as 0 for queue-mode devices) plays byte-perfect.  Verified
     * by A/B probe 2026-11-14: samples=0+pump clean, samples=2048
     * silent, samples=1024 squeak. */
    s_mus_dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (!s_mus_dev) {
        fprintf(stderr, "ll_music: device: %s\n", SDL_GetError());
        ll_music_stop();
        return 1;
    }
    {   /* P-13: log the accepted spec (see is_player.c) */
        static const char *const fmt[] = {
            "U8", "S8", "S16LSB", "S16MSB", "S16SYS", "S32LSB",
            "S32MSB", "S32SYS", "F32LSB", "F32MSB", "F32SYS",
        };
        unsigned f = have.format & 0x800F;
        fprintf(stderr, "ll_music: audio %s %uHz %uch %u samples\n",
                f < sizeof fmt / sizeof fmt[0] ? fmt[f] : "?",
                (unsigned)have.freq, (unsigned)have.channels,
                (unsigned)have.samples);
    }
    SDL_PauseAudioDevice(s_mus_dev, 0);
    /* E-09: dedicated pump thread; if it can't be created we fall
     * back to ll_music_pump() being called from the UI loops (the
     * pre-E-09 behaviour). */
    if (getenv("LEO_MUSIC_WAV"))
        music_wav_open();
    music_first_chunk();
    SDL_AtomicSet(&s_mus_quit, 0);
    s_mus_thr = SDL_CreateThread(music_pump_thread, "ll_music_pump", NULL);
    if (!s_mus_thr)
        printf("ll_music: pump thread failed (%s), using UI-loop pump\n",
               SDL_GetError());
    printf("ll_music: playing %s: '%s' %upos %uuniqpat %uch\n", modname,
           s_mus.title, s_mus.nseq, s_mus.npat_stored, s_mus.nch);
    return 0;
}

void ll_music_stop(void)
{
    if (s_mus_thr) {                    /* join first: the thread owns
                                           s_mst/s_mus_wf/s_mus_dev */
        SDL_AtomicSet(&s_mus_quit, 1);
        SDL_WaitThread(s_mus_thr, NULL);
        s_mus_thr = NULL;
    }
    if (s_mus_wf) {
        fclose(s_mus_wf);
        s_mus_wf = NULL;
    }
    if (s_mus_dev)
        SDL_CloseAudioDevice(s_mus_dev);
    s_mus_dev = 0;
    ll_mod_close(&s_mus);
    free(s_mus_img);
    s_mus_img = NULL;
    if (s_mus_snd_open)
        llrs_close(&s_mus_snd);
    s_mus_snd_open = 0;
}

/* Fallback queue top-up (pre-E-09 path): only used when the pump
 * thread could not be created; the UI loops keep calling it blindly.
 * No-op when no music device is open or the thread is running. */
void ll_music_pump(void)
{
    if (!s_mus_dev || s_mus_thr)
        return;
    if (SDL_GetQueuedAudioSize(s_mus_dev) > 12000)
        return;
    static int16_t buf[(LL_MOD_RATE / 8) * 2];      /* 125 ms stereo s16 */
    size_t frames = LL_MOD_RATE / 8;
    ll_mod_mix(&s_mst, buf, frames);
    if (s_mus_wf)
        fwrite(buf, 2 * sizeof(int16_t), frames, s_mus_wf);
    SDL_QueueAudio(s_mus_dev, buf, (Uint32)(frames * 2 * sizeof(int16_t)));
}

int ll_level_names(const char *data_dir, char names[][32], int max)
{
    char p[4096];
    snprintf(p, sizeof p, "%s/ETAPY", data_dir);
    llrs et;
    memset(&et, 0, sizeof et);
    if (llrs_open(&et, p) != 0)
        return 0;
    int n = 0;
    for (unsigned k = 0; k < et.cap && n < max; k++) {
        if (et.slots[k].id != 2)
            continue;
        char key[15] = "";
        memcpy(key, et.slots[k].key, 14);
        snprintf(names[n], 32, "%s", key);
        n++;
    }
    llrs_close(&et);
    return n;
}
#include <stdint.h>
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

/* Parse the "key=value" .DES text (CRLF); returns lines filled.
 * `len` bounds the scan: container payloads are not NUL-terminated. */
static int des_parse(const char *txt, size_t len, des_kv *kvs, int max)
{
    int n = 0;
    const char *p = txt, *end = txt + len;
    while (p < end && *p && n < max) {
        const char *eol = p;
        while (eol < end && *eol && *eol != '\n' && *eol != '\r')
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
/* bottom-aligned y for tall level sprites (world y is a below-anchor) */
static int bottom_y(int sh, int ch)
{
    return sh < ch ? ch - sh : 0;
}

static void anchor_of(const ll_seq_elem *e, int sh, int ch, int *ax, int *ay)
{
    int ex = (int)(short)e->f0, ey = (int)(short)e->f2;
    *ax = ex < 0 ? -ex : ex;
    *ay = ey < 0 ? -ey : bottom_y(sh, ch);
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
    int idx = o->eidx0;                        /* signed throughout (E-03):
                                                * corrupt .MOV frame data can
                                                * be negative */
    if (o->mov.nst) {
        idx = o->mov.st[o->sti].fr[o->fi].frame;
        if (idx < 0 || idx >= (int)o->seq.count)
            idx = (int)o->seq.count - 1;
    } else if (o->fi >= 0) {
        idx = o->fi % (int)o->seq.count;
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

/* PPM dump of the composited scene (CI hook, LEO_LEVEL_PPM; the parent
 * directory must already exist - CI recipes use /tmp/opencode directly) */
static void scene_ppm(const char *path, int w, int h, const unsigned char *idx,
                      const unsigned char pal[768])
{
    FILE *f = ll_fopen_ci(path, "wb");
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

/* Rebuild the background window for camera column `cam`:
 *  - PCM layer owner: engine strip wrap - screen strip k shows plane
 *    strip (cam/80 + k) mod strips (ll_pcm_view_at, FUN_00056c72); only
 *    the pcm_band ground rows change (upper rows are a documented CD art
 *    hole).  The FUN_00057460 per-strip parallax shear is not modelled.
 *  - .PCX background owner: window columns [cam, cam+cw) mod img.w.
 *    The tiling wrap is SYNTHESISED: the plan-layer .PCX blitter was not
 *    recovered (plan3.pcx carriers are partial on this CD); the mod keeps
 *    the scene seamless vs an edge smear.
 * cam == 0 is the rest view already present in canvas (no-op). */
static void bg_at(const ll_pcm *pcml, const ll_pcx *img, int pcm_owner,
                  unsigned pcm_band, int cam, int cw, int ch,
                  unsigned char *scratch, unsigned char *canvas)
{
    if (!cam)
        return;
    if (pcm_owner && scratch) {
        ll_pcm_view_at(pcml, 0, pcm_band, (unsigned)cam / 80u, scratch);
        memcpy(canvas + (size_t)(ch - (int)pcm_band) * (size_t)cw,
               scratch, (size_t)cw * pcm_band);
    } else if (!pcm_owner && img->idx) {
        for (int y = 0; y < ch; y++) {
            const unsigned char *srow = img->idx + (size_t)y * img->w;
            unsigned char *drow = canvas + (size_t)y * (size_t)cw;
            for (int c = 0; c < cw; c++)
                drow[c] = srow[(unsigned)((cam + c) % (int)img->w)];
        }
    }
}
/* 4b) .des placement join: HD-install sub-.des (ANI*.DES payloads in the
 *     level's ETAPY block) carry one "TYPE,name,mov,X,Y" object line per
 *     level object (verified grammar on TUNGA.11/ZARIA/LONA). TYPE names
 *     the ANIMACJE/<level> leaf block and the <mov> stem names one record
 *     inside that leaf (ELEM04.SEQ-style stems; TUNGA's strum1.mov art is
 *     absent from both trees - documented CD hole), X,Y is the world
 *     placement. Apply it for every line whose art exists: instantiate a
 *     copy at X,Y (bottom-align vertically like the anchor heuristic when
 *     Y exceeds the view) and retarget the fallback heuristic object with
 *     the same stem. Lines whose art is missing are logged and skipped. */
static void des_place_lines(llrs *an, const llrs_slot *arecs, unsigned narec,
                            llrs *hd_an,
                            const llrs_slot *hrecs, unsigned nhrec,
                            const char *txt, size_t len,
                            lvl_obj *objs, int *nobj, int maxobj,
                            int world_w, int ch, const char *level)
{
    const char *p = txt, *end = txt + len;
    while (p < end) {
        const char *eol = p;
        while (eol < end && *eol != '\n' && *eol != '\r')
            eol++;
        char typ[16] = "", nm[24] = "", mov[24] = "";
        int x = 0, y = 0, used = 0;
        int nf = sscanf(p, "%15[^,],%23[^,],%23[^,],%d,%d%n",
                        typ, nm, mov, &x, &y, &used);
        /* glibc scan sets swallow newlines into [^,] fields, so a bare
         * count line can bleed into the next fields: reject any field or
         * span that contains whitespace */
        if (nf == 5 && (strpbrk(typ, " \t\r\n") || strpbrk(nm, " \t\r\n")
                        || strpbrk(mov, " \t\r\n") || used > eol - p))
            nf = 4;
        if (!(nf == 5 && used > 8 && strchr(mov, '.'))
            || x < 0 || x >= world_w) {
            if (nf == 5 && strchr(mov, '.'))  /* world coord outside the
                                               * back_rect extent: bad data */
                printf("ll_level: des %s: %s/%s at (%d,%d) beyond back_rect"
                       " (%d)\n", level, typ, mov, x, y, world_w);
            p = eol;
            while (p < end && (*p == '\n' || *p == '\r'))
                p++;
            continue;
        }
        {
            char stem[16];
            stem_of(mov, stem, sizeof stem);
            int done = 0;
            for (int m = 0; m < *nobj; m++)   /* retarget fallback object */
                if (objs[m].leaf[0] && ci_eq(objs[m].leaf, typ)) {
                    char st2[16];
                    stem_of(objs[m].key, st2, sizeof st2);
                    if (ci_eq(st2, stem)) {
                        objs[m].bx = objs[m].x = x;
                        if (y + objs[m].sh <= ch)
                            objs[m].by = objs[m].y = y;
                        printf("ll_level: des-place %s/%s at (%d,%d)\n",
                               typ, objs[m].key, objs[m].x, objs[m].y);
                        done = 1;
                    }
                }
            if (!done && arecs) {             /* instantiate from the leaf */
                const llrs_slot *lf = NULL;
                for (unsigned k = 0; k < narec; k++)
                    if (arecs[k].id == 2 && arecs[k].name[0] &&
                        ci_eq(arecs[k].name, typ))
                        lf = &arecs[k];
                if (!lf)
                    printf("ll_level: des %s: leaf %s (%s) not in ANIMACJE"
                           " (data hole)\n", level, typ, mov);
                else if (*nobj < maxobj) {
                    llrs_slot *lrec = NULL;
                    unsigned nlrec = 0;
                    const llrs_slot *sr = NULL;
                    if (llrs_block(an, lf, &lrec, &nlrec) == 0)
                        for (unsigned k = 0; k < nlrec; k++) {
                            char st2[16];
                            stem_of(lrec[k].name, st2, sizeof st2);
                            if (lrec[k].id == 1 && ci_eq(st2, stem)) {
                                sr = &lrec[k];
                                break;
                            }
                        }
                    if (!sr)
                        printf("ll_level: des %s: %s/%s not on these"
                               " containers (data hole)\n", level, typ, mov);
                    else {
                        lvl_obj *o = &objs[*nobj];
                        memset(o, 0, sizeof *o);
                        if (llrs_read(an, sr, &o->blob) >= 0 &&
                            ll_seq_parse(o->blob, (uint32_t)sr->size,
                                         &o->seq) == 0) {
                            int first = -1;
                            for (unsigned e = 0; e < o->seq.count; e++)
                                if (o->seq.els[e].arr_n > 0) {
                                    first = (int)e;
                                    break;
                                }
                            if (first < 0) {
                                free(o->blob);
                                o->blob = NULL;
                            } else {
                                o->eidx0 = first;
                                snprintf(o->key, sizeof o->key, "%s",
                                         sr->name);
                                snprintf(o->leaf, sizeof o->leaf, "%s",
                                         lf->name);
                                elem_size(&o->seq, &o->seq.els[first],
                                          &o->sw, &o->sh);
                                o->x = o->bx = x;
                                o->y = o->by = (y + o->sh <= ch)
                                                   ? y : bottom_y(o->sh, ch);
                                if (hrecs) {
                                    llrs_slot *mr = NULL;
                                    unsigned nmr = 0;
                                    const llrs_slot *hl = NULL;
                                    for (unsigned k = 0; k < nhrec; k++)
                                        if (hrecs[k].id == 2 &&
                                            hrecs[k].name[0] &&
                                            ci_eq(hrecs[k].name, typ))
                                            hl = &hrecs[k];
                                    if (hl &&
                                        llrs_block(hd_an, hl, &mr,
                                                   &nmr) == 0) {
                                        const llrs_slot *mv =
                                            rec_find(mr, nmr, stem, ".MOV");
                                        if (mv &&
                                            llrs_read(hd_an, mv,
                                                      &o->movtxt) > 0)
                                            mov_parse((char *)o->movtxt,
                                                      &o->mov);
                                        llrs_block_free(mr);
                                    }
                                }
                                printf("ll_level: des-place %s/%s at (%d,%d)"
                                       " (new copy)\n", typ, o->key,
                                       o->x, o->y);
                                (*nobj)++;
                            }
                        }
                    }
                    llrs_block_free(lrec);
                }
            }
        }
        p = eol;
        while (p < end && (*p == '\n' || *p == '\r'))
            p++;
    }
}

static void des_placement(llrs *an, const llrs_slot *arecs, unsigned narec,
                          llrs *hd_an,
                          const llrs_slot *hrecs, unsigned nhrec,
                          llrs *hd_ety, const llrs_slot *hd_blk2,
                          const des_kv *kvs, int nk,
                          lvl_obj *objs, int *nobj, int maxobj,
                          int world_w, int ch, const char *level)
{
    if (!hd_ety || !hd_blk2)
        return;
    llrs_slot *hrec2 = NULL;
    unsigned nh2 = 0;
    if (llrs_block(hd_ety, hd_blk2, &hrec2, &nh2))
        return;

    /* the main .DES names the placement files: animacje=ani4.des,ani3.des */
    char list[512];
    snprintf(list, sizeof list, "%s", des_get(kvs, nk, "animacje"));
    char *save = NULL;
    int touched = 0;
    for (char *tok = strtok_r(list, ",", &save); tok;
         tok = strtok_r(NULL, ",", &save)) {
        while (*tok == ' ')
            tok++;
        if (!tok[0])
            continue;
        const llrs_slot *dr = NULL;
        for (unsigned k = 0; k < nh2; k++)
            if (hrec2[k].id == 1 && hrec2[k].name[0] &&
                ci_eq(hrec2[k].name, tok))
                dr = &hrec2[k];
        if (!dr) {
            printf("ll_level: des %s: %s not in the HD level block"
                   " (data hole)\n", level, tok);
            continue;
        }
        unsigned char *txt = NULL;
        if (llrs_read(hd_ety, dr, &txt) < 0)
            continue;
        printf("ll_level: placement %s (%u B)\n", dr->name, dr->size);
        des_place_lines(an, arecs, narec, hd_an, hrecs, nhrec,
                        (const char *)txt, (size_t)dr->size,
                        objs, nobj, maxobj, world_w, ch, level);
        free(txt);
        touched = 1;
    }
    if (!touched) {                    /* no animacje= list: try any .DES */
        for (unsigned k = 0; k < nh2; k++) {
            const char *dot = strrchr(hrec2[k].name, '.');
            if (hrec2[k].id != 1 || !dot || !ci_eq(dot, ".DES"))
                continue;
            unsigned char *txt = NULL;
            if (llrs_read(hd_ety, &hrec2[k], &txt) < 0)
                continue;
            printf("ll_level: placement %s (%u B)\n", hrec2[k].name,
                   hrec2[k].size);
            des_place_lines(an, arecs, narec, hd_an, hrecs, nhrec,
                            (const char *)txt, (size_t)hrec2[k].size,
                            objs, nobj, maxobj, world_w, ch, level);
            free(txt);
        }
    }
    llrs_block_free(hrec2);
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
 * only some pages present). Left/Right/Enter cycle pages, ESC or any
 * click closes and the page otherwise stays (original behavior, E-02);
 * LEO_HELP_SECS=<sec> adds an auto-close timeout for CI/flow only. The
 * scene window starts when the help overlay closes.
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
    /* line scan (repeated "static=" keys - a key=table would collapse them);
     * bounded by the payload size: container payloads are not NUL-terminated */
    const char *p = (const char *)raw, *mend = (const char *)raw + mnu->size;
    while (p < mend && *p) {
        const char *eol = p;
        while (eol < mend && *eol && *eol != '\n' && *eol != '\r')
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
    /* E-02: production behavior is the original's - the page stays until
     * a key or click closes it. The auto-close timeout exists only for
     * CI/headless flow and applies only when explicitly set. */
    double lim = hs ? atof(hs) : -1.0;
    int auto_close = lim >= 0.0;              /* scene timer starts after */
    Uint64 freq = SDL_GetPerformanceFrequency();
    Uint64 t_end = SDL_GetPerformanceCounter() +
                   (Uint64)((auto_close ? lim : 0.0) * (double)freq);
    int page = 0, quit = 0;
    while (!quit && (!auto_close || SDL_GetPerformanceCounter() < t_end)) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = 2;
            else if (e.type == SDL_MOUSEBUTTONDOWN)
                quit = 1;                     /* click closes, like a key */
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
    int rc = 0;                 /* every failure path still runs `done:` */
    char p[4096];
    llrs leveldes, etapy;
    snprintf(p, sizeof p, "%s/LEVELDES", dir);
    if (llrs_open(&leveldes, p)) {
        /* E-01: name the DATA problem, not just the file: a bare
         * "cannot open" with no context is where support threads start. */
        fprintf(stderr, "ll_level: cannot open %s - %s is not a DATA "
                        "directory (no LEVELDES container there)\n", p, dir);
        return 1;
    }
    snprintf(p, sizeof p, "%s/ETAPY", dir);
    if (llrs_open(&etapy, p)) {
        fprintf(stderr, "ll_level: cannot open %s - %s has LEVELDES but no "
                        "ETAPY container\n", p, dir);
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
            int nt = des_parse((char *)cand, (size_t)s->size, tmp, 8);
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
        nk = des_parse((char *)txt, (size_t)des->size, kvs, 64);
        for (int k = 0; k < nk; k++)
            printf("  %-12s = %s\n", kvs[k].key, kvs[k].val);
    } else {
        printf("ll_level: no .DES for '%s' (showing ETAPY block only)\n",
               level);
    }

    /* World extent: back_rect=x0,y0,w,h (RAN 28800x400, TUNGA.11
     * 2560x600, ENA1 5169x574, JUNCTION 320x200 = static one-screen).
     * back_rect is absent on SADES/TUNGA2/ZARIA* (CD hole) -> no camera
     * (world = one view). */
    int world_x0 = 0, world_y0 = 0, world_w = 0, world_h = 0;
    {
        const char *br = des_get(kvs, nk, "back_rect");
        if (br && *br)
            sscanf(br, "%d,%d,%d,%d", &world_x0, &world_y0,
                   &world_w, &world_h);
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

    /* 3) background layer stack: tlo= lists the layers in stack order
     * ("plan3.pcx,plan2.pcm,plan1.pcm").  Pick the FIRST listed layer
     * present in the level's ETAPY block: a .PCX wins the canvas; a
     * .PCM (parallax plane, ll_pcm) becomes the canvas only when no
     * .PCX precedes it in the stack (RAN.01 on this CD: plan3.pcx and
     * plan2.pcm are absent, plan1.pcm is present -> PCM canvas).
     * Fallback: any .PCX in the block (KADMAR2's M15.PCX intro art). */
    char bgname[32] = "";
    char pcm_t[16] = "";            /* tlo stem of the canvas PCM layer */
    const llrs_slot *bg = NULL;
    int bgpref = -1;                /* block index of first present tlo layer */
    {
        char tlo[512];
        snprintf(tlo, sizeof tlo, "%s", des_get(kvs, nk, "tlo"));
        char *save = NULL;
        for (char *tok = strtok_r(tlo, ",", &save); tok;
             tok = strtok_r(NULL, ",", &save)) {
            while (*tok == ' ')
                tok++;
            const char *dot = strrchr(tok, '.');
            if (!dot || !(ci_eq(dot, ".PCX") || ci_eq(dot, ".PCM")))
                continue;
            char stem[16];
            stem_of(tok, stem, sizeof stem);
            for (unsigned k = 0; k < nrec; k++) {
                char st2[16];
                stem_of(recs[k].name, st2, sizeof st2);
                if (recs[k].id == 1 && ci_eq(st2, stem)) {
                    if (bgpref < 0)
                        bgpref = (int)k;
                    if (!bg && ci_eq(dot, ".PCX"))
                        bg = &recs[k];
                    if (ci_eq(dot, ".PCM") && !pcm_t[0])
                        snprintf(pcm_t, sizeof pcm_t, "%s", stem);
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
    if (bg && (bgpref < 0 || ci_eq(bg->name, recs[bgpref].name)))
        pcm_t[0] = '\0';            /* a tlo .PCX owns the canvas */
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
    /* tlo= names the whole background layer stack (.pcx + .pcm).  PCM
     * layers are decoded for display; when the block carries no tlo .PCX
     * (RAN.01 on this CD: plan1.pcm only) the first tlo= .PCM becomes the
     * scene canvas - the 400-px blit window of FUN_00056c72 at rest.
     * PCM payloads are NOT audio (parallax image planes, see ll_pcm.h). */
    ll_pcm pcml;
    memset(&pcml, 0, sizeof pcml);
    int have_pcm = 0;
    for (unsigned k = 0; k < nrec; k++) {
        const char *dot = strrchr(recs[k].name, '.');
        if (recs[k].id != 1 || !dot || !ci_eq(dot, ".PCM"))
            continue;
        int want = 0;
        if (!have_pcm && pcm_t[0]) {
            char stem[16];
            stem_of(recs[k].name, stem, sizeof stem);
            if (ci_eq(stem, pcm_t))
                want = 1;
        }
        unsigned char *raw = NULL;
        ll_pcm p2;
        memset(&p2, 0, sizeof p2);
        int rc2 = -1;
        if (llrs_read(&etapy, &recs[k], &raw) >= 0) {
            rc2 = ll_pcm_decode(raw, (size_t)recs[k].size, &p2);
            if (rc2 == 0) {
                if (want) {
                    pcml = p2;
                    have_pcm = 1;
                    snprintf(bgname, sizeof bgname, "%s", recs[k].name);
                } else
                    ll_pcm_release(&p2);
                printf("ll_level: PCM %s: parallax layer %ux%u rows"
                       " (window %u rows, stride %u)%s\n", recs[k].name,
                       p2.w, p2.rows, p2.win_rows, p2.stride,
                       want ? " -> scene canvas" : "");
            } else
                printf("ll_level: PCM %s: decode failed rc=%d size=%d\n",
                       recs[k].name, rc2, recs[k].size);
            free(raw);
        }
    }

    int cw = (int)img.w, ch = (int)img.h;
    int pcm_owner = have_pcm && (!bg || bgpref < 0
                                 || !ci_eq(bg->name, recs[bgpref].name));
    unsigned char *view = NULL;
    unsigned pcm_band = 0;
    if (pcm_owner) {
        /* The engine can only show its current load window: the ctor
         * allocates and decodes exactly win_rows rows (hdr u16@2), and
         * blitter FUN_00056c72 clamps blit height to that window.  Deeper
         * plane rows belong to further scroll stages streamed in later.
         * At rest the window is the ground band (plan1 = nearest layer)
         * at the bottom of the 400x200 screen; above it stays black
         * because this CD lacks plan3.pcx/plan2.pcm (documented hole). */
        cw = 400;
        ch = 200;
        pcm_band = pcml.win_rows ? pcml.win_rows : 200;
        if (pcm_band > pcml.rows)
            pcm_band = (unsigned)pcml.rows;
        if (pcm_band > 200)
            pcm_band = 200;
        view = calloc((size_t)cw, (size_t)ch);
        if (view)
            ll_pcm_view(&pcml, 0, pcm_band,
                        view + (size_t)(ch - pcm_band) * (size_t)cw);
    }
    if (!img.idx && !view) {
        printf("ll_level: no background PCX for '%s' "
               "(wiring wants '%s' - missing from these containers)\n",
               level, des_get(kvs, nk, "tlo"));
        free(view);
        ll_pcm_release(&pcml);
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
    llrs hd_ety;
    int have_hd_ety = 0;
    const llrs_slot *hblk2 = NULL;
    if (hd) {
        char hp[4096];
        snprintf(hp, sizeof hp, "%s/ETAPY", hd);
        have_hd_ety = llrs_open(&hd_ety, hp) == 0;
        if (have_hd_ety) {
            hblk2 = find_or_stem(&hd_ety, level);
            if (!hblk2 && nk) {
                const char *d = des_get(kvs, nk, "dir");
                if (d[0])
                    hblk2 = find_or_stem(&hd_ety, d);
            }
        }
    }
    llrs_slot *arecs = NULL;
    unsigned narec = 0;
    int an_open = 0;
    if (llrs_open(&an, p) == 0) {
        const llrs_slot *ablk = find_or_stem(&an, level);
        an_open = 1;
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
        }
    }

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
            ll_seq_free(&objs[m].seq);
            free(objs[m].movtxt);
            free(objs[m].blob);
        }
        if (b > a + 1)
            memmove(&objs[a + 1], &objs[b],
                    sizeof(lvl_obj) * (size_t)(nobj - b));
        nobj -= b - a - 1;
        a++;
    }

    /* 4b) .des X,Y placement join (HD sub-.des -> ANIMACJE leaf records);
     *     X is a WORLD coordinate validated against back_rect */
    des_placement(&an, arecs, narec, &hd_an, hrecs, nhrec,
                  have_hd_ety ? &hd_ety : NULL, hblk2, kvs, nk,
                  objs, &nobj, (int)(sizeof objs / sizeof objs[0]),
                  world_w > 0 ? world_w : (int)img.w, (int)img.h, level);
    if (arecs)
        llrs_block_free(arecs);
    if (an_open)
        llrs_close(&an);
    if (hrecs)
        llrs_block_free(hrecs);
    if (have_hd)
        llrs_close(&hd_an);
    /* hd_ety stays open: the help overlay (5b) reads the same HD block */

    printf("ll_level: show %s: bg %s %dx%d, %d animated object(s)\n", level,
           bgname, cw, ch, nobj);
    for (int k = 0; k < nobj; k++)
        printf("ll_level: obj %s/%s: %dx%d at (%d,%d)%s\n", objs[k].leaf,
               objs[k].key, objs[k].sw, objs[k].sh, objs[k].x, objs[k].y,
               objs[k].mov.nst ? " [mov]" : "");

    /* Scroll camera over the back_rect world (horizontal only; the world
     * height exceeds the view vertically - no vertical pan recovered,
     * documented).  PCM layers pan through the engine strip wrap; .PCX
     * bgs pan by synthesised tiling (bg_at).  LEO_LEVEL_CAM = initial
     * camera x (CI); interactive: Left/Right pan, Home/End jump,
     * p toggles a 50 px/s autoscroll (the original walk speed). */
    int cam_max = world_w > cw ? world_w - cw : 0;
    int cam = getenv("LEO_LEVEL_CAM") ? atoi(getenv("LEO_LEVEL_CAM")) : 0;
    if (cam < 0)
        cam = 0;
    if (cam > cam_max)
        cam = cam_max;
    int autoscroll = 0;
    if (world_w > 0)
        printf("ll_level: world %dx%d (back_rect %d,%d), view %dx%d, cam"
               " %d/%d [Left/Right pan, Home/End, p autoscroll]\n",
               world_w, world_h, world_x0, world_y0, cw, ch, cam, cam_max);
    else
        printf("ll_level: no back_rect (data hole): static one-screen view"
               " %dx%d\n", cw, ch);
    unsigned char *scratch = NULL;   /* PCM scroll band scratch (below) */

    /* 5) composite canvas + present loop.  bg source: PCX plane, or the
     * PCM 400x200 rest view when the level is PCM-driven. */
    size_t csz = (size_t)cw * (size_t)ch;
    unsigned char *canvas = malloc(csz);
    if (!canvas) {
        rc = 1;                 /* E-05: OOM must still release everything
                                 * else allocated (done: below) */
        goto done;
    }
    scratch = malloc((size_t)cw * (pcm_band ? pcm_band : 200));
    const unsigned char *base = pcm_owner ? view : img.idx;
    memcpy(canvas, base, csz);
    bg_at(&pcml, &img, pcm_owner, pcm_band, cam, cw, ch, scratch, canvas);
    for (int k = 0; k < nobj; k++) {
        const ll_seq_elem *e = obj_elem(&objs[k]);
        if (e)
            ll_seq_draw_elem(&objs[k].seq, e, canvas, cw, ch,
                             objs[k].x - cam, objs[k].y);
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
        memcpy(canvas, base, csz);         /* same order as the live loop */
        bg_at(&pcml, &img, pcm_owner, pcm_band, cam, cw, ch, scratch, canvas);
        for (int k = 0; k < nobj; k++) {
            const ll_seq_elem *e = obj_elem(&objs[k]);
            if (e)
                ll_seq_draw_elem(&objs[k].seq, e, canvas, cw, ch,
                                 objs[k].x - cam, objs[k].y);
        }
        if (pcm_owner) {
            /* PCM canvas: CI hash of the bare window band (win_rows rows,
             * top-anchored like verify_pcm.py's `view`) must equal the
             * ll_pcm oracle hash (level-integration check) */
            uint64_t hv = 0xCBF29CE484222325ull;
            const unsigned char *band =
                view + (size_t)(ch - pcm_band) * (size_t)cw;
            for (size_t i = 0; i < (size_t)cw * pcm_band; i++) {
                hv ^= band[i];
                hv *= 0x100000001B3ull;
            }
            printf("ll_level: PCM view %s %dx%u fnv=%016llx\n", bgname,
                   cw, pcm_band, (unsigned long long)hv);
            unsigned char ppal[768];
            if (img.idx)
                memcpy(ppal, img.pal, 768);   /* level block's PCX palette */
            else
                for (int i = 0; i < 256; i++) /* documented fallback: gray */
                    ppal[3 * i] = ppal[3 * i + 1] = ppal[3 * i + 2] =
                        (unsigned char)i;
            scene_ppm(ppm, cw, ch, canvas, ppal);
        } else
            scene_ppm(ppm, cw, ch, canvas, img.pal);
        goto done;                          /* canvas freed at done: (E-05) */
    }

    {
        /* P-10/T-12: shared window convention (logical viewport = the
         * scene view size cw x ch; resizable, HiDPI-ready, F11/Alt+Enter
         * desktop-fullscreen). */
        ll_view view;
        SDL_Window *win = NULL;
        SDL_Renderer *ren = NULL;
        if (ll_view_create(&view, "Lew Leon - level scene", cw, ch) != 0) {
            fprintf(stderr, "ll_level: no window: %s\n", SDL_GetError());
            goto done;
        }
        win = view.win;
        ren = view.ren;
        SDL_RaiseWindow(win);
        /* hard focus grab over the menu window that had it; Wayland
         * ignores it, the live loop polls SDL_RaiseWindow as fallback
         * (see ll_menu.c win_force_focus) */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        SDL_SetWindowInputFocus(win);
#pragma GCC diagnostic pop
        SDL_Texture *tex = SDL_CreateTexture(
            ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
            cw, ch);
        Uint32 lut[256];
        if (!pcm_owner || img.idx)
            for (int i = 0; i < 256; i++)
                lut[i] = 0xFF000000u | ((Uint32)img.pal[3 * i] << 16) |
                         ((Uint32)img.pal[3 * i + 1] << 8) | img.pal[3 * i + 2];
        else
            for (int i = 0; i < 256; i++)   /* no palette donor in the block:
                                             * grayscale (documented hole) */
                lut[i] = 0xFF000000u | 0x010101u * (Uint32)i;

        /* 5b) help overlay first, like the original POMOC flow: the HD
         *     level block's POMOC1.MNU names page art (.SEQ) that ships in
         *     the CD level block; ESC closes, Left/Right page through. */
        if (have_hd_ety && hblk2) {
            llrs_slot *hrecs2 = NULL;
            unsigned nhrec2 = 0;
            if (llrs_block(&hd_ety, hblk2, &hrecs2, &nhrec2) == 0 && nhrec2)
                help_pages(ren, tex, &hd_ety, hrecs2, nhrec2,
                           &etapy, recs, nrec, base, canvas, lut,
                           cw, ch);
            llrs_block_free(hrecs2);
        }

        /* 5c) level music: the .des `muzyka=<stem>.mod` names a SOUND
         *     container '<STEM>.MOD' payload (standard MOD layout, see
         *     ll_mod.h).  Same singleton as the MAPKA screen; the scene
         *     loop below pumps it (SDL queue path, see note above
         *     ll_audio_init - callbacks are unreliable on SDL2+PipeWire). */
        if (audio && !getenv("LEO_NO_MUSIC")) {
            const char *muz = des_get(kvs, nk, "muzyka");
            if (muz && *muz) {
                char stem[16], mname[24];
                stem_of(muz, stem, sizeof stem);
                snprintf(mname, sizeof mname, "%s.MOD", stem);
                ll_music_play(dir, mname);
            }
        }

        Uint64 freq = SDL_GetPerformanceFrequency();
        /* secs <= 0 = no time limit: the scene runs until ESC/q (desktop
         * behaviour; CI pins the window with LEO_LEVEL_SECS>0). */
        Uint64 t_end = secs > 0 ? SDL_GetPerformanceCounter() +
                                  (Uint64)(secs * (double)freq)
                                : 0;
        Uint64 t_prev = SDL_GetPerformanceCounter();
        while (!esc && (!t_end || SDL_GetPerformanceCounter() < t_end)) {
            if (win && SDL_GetKeyboardFocus() != win)
                SDL_RaiseWindow(win); /* Wayland re-raise fallback */
            SDL_Event e;
            while (SDL_PollEvent(&e))
                if (ll_view_fullscreen_key(&e, win))
                    ;                   /* P-10: F11 / Alt+Enter */
                else if (e.type == SDL_QUIT ||
                    (e.type == SDL_KEYDOWN &&
                     (e.key.keysym.sym == SDLK_ESCAPE ||
                      e.key.keysym.sym == SDLK_q)))
                    esc = 1;
                else if (e.type == SDL_KEYDOWN && cam_max > 0) {
                    SDL_Keycode k = e.key.keysym.sym;
                    if (k == SDLK_LEFT)
                        cam = cam > 20 ? cam - 20 : 0;
                    else if (k == SDLK_RIGHT)
                        cam = cam + 20 <= cam_max ? cam + 20 : cam_max;
                    else if (k == SDLK_HOME)
                        cam = 0;
                    else if (k == SDLK_END)
                        cam = cam_max;
                    else if (k == SDLK_p) {
                        autoscroll = !autoscroll;
                        printf("ll_level: autoscroll %s\n",
                               autoscroll ? "on (50 px/s)" : "off");
                    }
                }
            Uint64 now = SDL_GetPerformanceCounter();
            unsigned dt = (unsigned)((now - t_prev) * 1000 / (double)freq);
            t_prev = now;
            if (dt > 200)
                dt = 200;
            if (autoscroll) {
                cam += 50 * (int)dt / 1000;
                if (cam > cam_max)
                    cam = 0;
            }
            for (int k = 0; k < nobj; k++)
                obj_advance(&objs[k], dt);
            memcpy(canvas, base, csz);
            bg_at(&pcml, &img, pcm_owner, pcm_band, cam, cw, ch, scratch,
                  canvas);
            for (int k = 0; k < nobj; k++) {
                const ll_seq_elem *el = obj_elem(&objs[k]);
                if (el)
                    ll_seq_draw_elem(&objs[k].seq, el, canvas, cw, ch,
                                     objs[k].x - cam, objs[k].y);
            }
            paint(ren, tex, canvas, lut, cw, ch);
            ll_music_pump();
            SDL_Delay(20);
        }
        ll_music_stop();
        if (tex)
            SDL_DestroyTexture(tex);
        if (ren)
            SDL_DestroyRenderer(ren);
        if (win)
            SDL_DestroyWindow(win);
    }
    if (esc)
        printf("ll_level: ESC -> advance\n");

done:                                 /* single cleanup path (E-05): every
                                       * way out of the scene runs this */
    free(canvas);
    for (int k = 0; k < nobj; k++) {
        ll_seq_free(&objs[k].seq);
        free(objs[k].movtxt);
        free(objs[k].blob);
    }
    ll_pcx_free(&img);
    free(view);
    free(scratch);
    ll_pcm_release(&pcml);
    llrs_block_free(recs);
    free(txt);
    if (have_hd_ety)
        llrs_close(&hd_ety);
    llrs_close(&leveldes);
    llrs_close(&etapy);
    return rc;
}
