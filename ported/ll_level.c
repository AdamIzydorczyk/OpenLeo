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
 * PCM payloads are TRANSPARENT overlay image layers (PR-53: column strips
 * of compiled-sprite tiles, ll_pcm.h) - NOT audio, and NOT the byte-RLE plane
 * earlier PRs decoded. They join the scene as the tlo= background stack: the
 * first .PCX is the backdrop, every .PCM is painted over it in tlo order;
 * a level without a backdrop .PCX (KADMAR2) shows them over an empty canvas
 * with the block's BACK.PCX palette.
 *   player     = with LEO_PLAY (M4-B, PR-43) the scene additionally runs
 *                the real lion: `lew=` leaf .SEQ art + that leaf's HD
 *                .MOV (physics/transitions), ground from `platformy=`
 *                (a .PLT, or a sub-.des whose text carries the same
 *                polyline grammar), spawn `pozycja=`, bounds `lew_rect=`.
 *                Parsing and physics live in ll_play.c; this file only
 *                loads the data and draws the lion at the .SEQ anchor
 *                (-f0,-f2).
 */
#include "ll_level.h"
#include "fs_ci.h"
#include "ll_pcx.h"
#include "ll_pcm.h"
#include "ll_play.h"
#include "ll_seq.h"
#include "llrs.h"
#include "ll_mod.h"
#include "platform.h"

#include <stdatomic.h>  /* pump-thread stop flag (portable C11; the
                         * thread itself is plat_thread_start, T-01).
                         * This file contains no SDL: the queue-only
                         * audio contract and its samples=0 rationale
                         * live with the SDL backend (ported/ll_audio.h,
                         * ported/platform_sdl.c). */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> /* strncasecmp for container record names */

/* Music audio (E-09): the device is plat_audio_open (queue-only, s16
 * interleaved - same call the cutscene player uses); the queue is kept
 * >= ~250 ms by a dedicated pump thread (plat_thread_start) so window
 * drags (Windows modal loop), slow loads and screen delays never
 * underrun it - the old in-loop pump did. When the platform has no
 * threads (headless dummy backend) plat_thread_start fails cleanly and
 * the UI loops keep topping the queue via ll_music_pump (the pre-E-09
 * path); both paths produce the same bytes. */

/* ---- standalone music playback (menu/map screens), singleton ---- *
 * Engine anchor: FUN_0005dec0 opens+plays "mapka.mod" for the MAPKA
 * (level-select) screen; FUN_0005df20 stops it on every exit path.
 * Same SOUND container + ll_mod player as the level path. */
static llrs s_mus_snd;
static unsigned char *s_mus_img;
static ll_mod s_mus;
static ll_mod_state s_mst;
static int s_mus_aopen;                /* platform audio device is ours */
static int s_mus_snd_open;
static FILE *s_mus_wf;                 /* LEO_MUSIC_WAV diagnostic dump */
static char s_mus_name[32];
static int s_mus_thr;                  /* plat_thread running (E-09)   */
static atomic_int s_mus_quit;

/* E-09: while it lives this thread owns s_mst/s_mus_wf/the audio
 * queue (ll_music_stop joins it before touching any of them,
 * ll_music_play stops first), so no lock is needed. Queue kept
 * >= ~250 ms with 125 ms chunks (old in-loop low-water was 12000 B
 * ~= 68 ms, review E-09). */
static int music_pump_thread(void *unused)
{
    (void)unused;
    static int16_t buf[(LL_MOD_RATE / 8) * 2];      /* 125 ms stereo s16 */
    const size_t low_water = (size_t)LL_MOD_RATE;   /* 250 ms bytes */
    while (!atomic_load(&s_mus_quit)) {
        if (plat_audio_queued_bytes() < low_water) {
            size_t frames = LL_MOD_RATE / 8;
            ll_mod_mix(&s_mst, buf, frames);
            if (s_mus_wf) {
                fwrite(buf, 2 * sizeof(int16_t), frames, s_mus_wf);
                fflush(s_mus_wf);       /* CI hook: LEO_MUSIC_WAV stays
                                         * tail-able while the game runs */
            }
            plat_audio_queue(buf, frames * 2 * sizeof(int16_t));
        }
        plat_sleep_until(plat_now_ns() + 25000000ull); /* first pass runs
                                         * immediately, so even very short
                                         * plays get their first chunk */
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
    if (s_mus_aopen)
        plat_audio_queue(buf, frames * 2 * sizeof(int16_t));
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
    /* Queue-only device (samples=0 contract + accepted-spec log P-13
     * enforced inside the backend; the samples A/B probe results are
     * documented at ported/ll_audio.h): -1 = audio subsystem
     * unavailable -> play silent, exactly like the old silent
     * ll_audio_session_init failure path; -2 = subsystem up, device
     * failed -> keep the old diagnostic. */
    int arc = plat_audio_open(LL_MOD_RATE, 2);
    if (arc == -2) {
        fprintf(stderr, "ll_music: device: %s\n", plat_last_error());
        ll_music_stop();
        return 1;
    }
    if (arc != 0) {
        ll_music_stop();
        return 1;
    }
    s_mus_aopen = 1;
    /* E-09: dedicated pump thread; if the platform cannot give us one
     * we fall back to ll_music_pump() being called from the UI loops
     * (the pre-E-09 behaviour). */
    if (getenv("LEO_MUSIC_WAV"))
        music_wav_open();
    music_first_chunk();
    atomic_store(&s_mus_quit, 0);
    if (plat_thread_start(music_pump_thread, NULL, "ll_music_pump") == 0)
        s_mus_thr = 1;
    else
        printf("ll_music: pump thread failed (%s), using UI-loop pump\n",
               plat_last_error());
    printf("ll_music: playing %s: '%s' %upos %uuniqpat %uch\n", modname,
           s_mus.title, s_mus.nseq, s_mus.npat_stored, s_mus.nch);
    return 0;
}

void ll_music_stop(void)
{
    if (s_mus_thr) {                    /* join first: the thread owns
                                            s_mst/s_mus_wf/audio queue */
        atomic_store(&s_mus_quit, 1);
        plat_thread_join();
        s_mus_thr = 0;
    }
    if (s_mus_wf) {
        fclose(s_mus_wf);
        s_mus_wf = NULL;
    }
    if (s_mus_aopen)
        plat_audio_close();
    s_mus_aopen = 0;
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
    if (!s_mus_aopen || s_mus_thr)
        return;
    if (plat_audio_queued_bytes() > 12000)
        return;
    static int16_t buf[(LL_MOD_RATE / 8) * 2];      /* 125 ms stereo s16 */
    size_t frames = LL_MOD_RATE / 8;
    ll_mod_mix(&s_mst, buf, frames);
    if (s_mus_wf)
        fwrite(buf, 2 * sizeof(int16_t), frames, s_mus_wf);
    plat_audio_queue(buf, frames * 2 * sizeof(int16_t));
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

/* ---------------- .MOV object state machine -------------------------------
 * The parser and structs moved to ll_play.c (PR-43, ll_mov_*): IDENTICAL
 * frame semantics for scene objects (a frame number is 1-based and its sign
 * is the mirror flag - ll_mov_frame_ref, PR-49; non-positive durations clamped to 100 ms = the M4-C rule),
 * plus the player-facing extension documented in ll_play.h (F=/T=/G=
 * physics, TARGET(n)=COND transitions, control= flags, per-state seq=).
 * Scene objects here keep using only the frame-list semantics through
 * obj_advance/obj_elem below. */

/* ---------------- scene objects (PR-45: the authored scene) ------------
 * A level's own placement data (the sub-.des files its main .des names
 * through zwierzaki= / animacje= / ani_plat=) says which object scripts
 * stand where: "<LEAF>,<class>,<script>.mov,<x>,<y>". The script is the
 * HD ANIMACJE record of that exact name (ANIMACJE/<level>/<LEAF>/
 * <SCRIPT>.MOV), and the ART is whatever that script selects for the
 * state it is in: every [STATE] block carries "seq=<name>", naming
 * ANIMACJE/<level>/<LEAF>/<NAME>.SEQ on the CD (measured over the 708
 * distinct (level, leaf, script) triples the 2188 placement entries use,
 * PLAN 8g - NOT the .mov stem, which matches one of its own seq= names in
 * only 439 of the 681 scripts that exist).
 * So the model is: one obj_script per distinct (<LEAF>,<SCRIPT>.MOV) the
 * level references (art + parsed script loaded ONCE), and one lvl_obj per
 * placement (RAN.01 places the same 2 cloud scripts 184 times). Nothing
 * is instantiated that the level's data does not name. */

#define OBJ_MAXART 16      /* distinct seq= names one .MOV selects; 10 is
                            * the measured maximum (PLAN 8g) */

typedef struct {           /* one .SEQ art table, owned by its script    */
    char name[24];                    /* the seq= name, .SEQ form        */
    unsigned char *blob;
    ll_seq seq;                       /* parsed (owns the element table) */
    int found;                        /* 0 = the record is not on these
                                       * containers (a data hole)        */
    int eidx0;                        /* first element with patch art; -1
                                       * with found=1 = the table carries
                                       * NO patch art at all, only shape
                                       * geometry (w0&2 without w0&1). The
                                       * game's own draw paths all gate on
                                       * its patch pointer array, so such a
                                       * table was never drawn THERE either
                                       * - invisible, and drawn as nothing
                                       * here too (RE 2026-11-16, PLAN 8h) */
    int sw, sh;                       /* sprite bbox (element chunks)    */
    int rx, ry;                       /* worst-case draw reach around the
                                       * object position over ALL elements
                                       * (anchor + width, both facings)   */
    /* the table's SHAPE facts (measured from its own elements, never
     * invented): the first element that carries the per-row margin lists
     * (the game's hit/push geometry, ll_seq_shape_*). */
    int sidx0;                        /* first shape-bearing element, -1  */
    int nshape;                       /* elements with margin lists       */
    int stw, srows;                   /* that element's shape frame      */
    int sbx, sby, sbw, sbh;           /* ... its FUN_000420b0 bbox      */
    int nomargins;                    /* 1 = that element declares rows
                                       * whose margin lists are BOTH wholly
                                       * the empty marker: the game's own
                                       * box math stores w = 1 - total_w,
                                       * h = 1 - rows there (arithmetic,
                                       * not a data gap)                */
} obj_art;

typedef struct {                     /* one authored object script       */
    char leaf[16], movname[24];      /* exactly as the placement names it */
    unsigned char *movtxt;           /* owned, or NULL                    */
    ll_mov_t *mov;                   /* owned, NULL = no script on these
                                      * containers (a real data hole)     */
    obj_art art[OBJ_MAXART];         /* distinct seq= art, deduped       */
    int nart;
    int ast[LMOV_MAXST];             /* state -> art index, -1 = missing */
    int artmiss[OBJ_MAXART][24];     /* seq= names not on the CD, once   */
    int nartmiss;                    /* ... counted                      */
    int nartshape;                   /* art tables carrying shape only    */
    int nartcarry;                   /* ... art AND shape (w0&1|w0&2)    */
    int nartover;                    /* seq= names beyond OBJ_MAXART     */
    int ninst;                       /* placements using this script     */
    /* what the script authors but the scene model does not act on yet
     * (counts only: reported, never invented; see PLAN 8g) */
    int ntrans, nflag_other, ndirect, nfield_other;
} obj_script;

typedef struct {                     /* one placement of a script        */
    obj_script *sc;
    int bx, by;                      /* authored anchor                  */
    int x, y;                        /* current (anchor + authored M)    */
    int sti, fi;                     /* state / frame cursor             */
    unsigned t;                      /* ms within current frame          */
} lvl_obj;

typedef struct {                     /* one level's authored scene: the
                                      * scripts its placement data names
                                      * (shared art) + its placements    */
    obj_script **set;                /* heap array of owned scripts     */
    int nsc, capsc;
    lvl_obj *objs;                   /* heap array, grows as needed     */
    int nobj, capobj;
} scene_owner;                       /* no fixed cap: the measured maxima in
                                      * the shipped data are 164 scripts and
                                      * 246 art tables (SADES.06) and 827
                                      * placements (RAN.01) */

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
        ll_seq_patch_walk_codec(s->blob + en->off, en->len,
                                ll_seq_elem_codec(e), row_count_cb, &rc);
        if (rc.maxrow > *h)
            *h = rc.maxrow;
    }
}

/* The old bottom-anchor heuristic (x = |f0|, tall sprites bottom-aligned in
 * the view) is GONE with PR-45: the .des placement coordinates ARE the
 * authored position of the object, so nothing has to be guessed any more.
 * (ll_menu.c keeps its own rule for MENU scripts, which really are drawn at
 * (-f0,-f2).) */

/* The art of the state an object is in: exactly the .SEQ that state names
 * through seq=. If it names none, or names one this install does not carry,
 * the object draws NOTHING - taking another state's table instead would be a
 * guess, and the missing name is already reported once per script. An art
 * table whose elements carry only shape geometry (no patch art at all) IS
 * returned: the draw path skips it through its element index, and the report
 * says why - the original game's draw paths gate on the patch array too, so
 * it drew nothing from such a table either. */
static const obj_art *obj_art_of(const lvl_obj *o)
{
    const obj_script *sc = o->sc;
    if (!sc || o->sti < 0 || o->sti >= LMOV_MAXST)
        return NULL;
    int idx = sc->ast[o->sti];
    if (idx < 0 || idx >= sc->nart)
        return NULL;
    const obj_art *a = &sc->art[idx];
    return a->found ? a : NULL;
}

/* current drawable element of an object (frame number from .MOV through
 * ll_mov_frame_ref: 1-based, negative = mirrored, 0 or out of range = none;
 * the element table is the art the CURRENT state selected, PR-45) */
static const ll_seq_elem *obj_elem(const lvl_obj *o, int *mirror)
{
    const obj_art *a = obj_art_of(o);
    *mirror = 0;
    if (!a || a->seq.count == 0 || a->eidx0 < 0)
        return NULL;
    int n = a->eidx0 + 1;                      /* no script: the first
                                                * drawable element, as its
                                                * 1-based frame number */
    if (o->sc->mov && o->sti >= 0 && o->sti < o->sc->mov->nst) {
        const ll_mov_st *s = &o->sc->mov->st[o->sti];
        if (s->nfr > 0) {                      /* the cycle guard of
                                                * obj_advance can stop with
                                                * the cursor at the end of a
                                                * state: hold the last frame */
            int fi = o->fi < s->nfr ? o->fi : s->nfr - 1;
            n = s->fr[fi].frame;
        }
    }
    /* the game's frame-line grammar (ll_mov_frame_ref): 1-based, the sign is
     * the mirror flag, 0 has no element. A number that names an element
     * this table does not have draws NOTHING - the old clamp to the last
     * element invented art the data never asked for */
    ll_frame_ref r = ll_mov_frame_ref(n);
    if (r.blank || r.idx >= (int)a->seq.count)
        return NULL;
    *mirror = r.mirror;
    return &a->seq.els[r.idx];
}

/* Is this placement worth a draw pass in the view window? With RAN.01's 827
 * cloud placements (a 28800 px world, most of them off screen) the pass must
 * skip them, and skipping must match what ll_seq_draw_elem would do: patches
 * are 80 px column strips drawn right and down from (x,y), so the real
 * extent is the element bbox (elem_size), not a guess. */
static int obj_visible(const lvl_obj *o, int cw, int ch, int cam, int camy)
{
    const obj_art *a = obj_art_of(o);      /* NULL: no art for this state */
    if (!a || a->eidx0 < 0)             /* < 0: shape-only, nothing to draw */
        return 0;
    int x = o->x - cam, y = o->y - camy;
    /* the drawn rectangle is the OBJECT position moved by the element's own
     * anchor and possibly mirrored (ll_seq_draw_elem_anchor): a->rx / a->ry
     * bound that over every element of the table, so the cull cannot drop
     * a frame that would have reached the view */
    if (x - a->rx >= cw || y - a->ry >= ch || x + a->rx < 0 || y + a->ry < 0)
        return 0;
    return 1;
}

/* Draw one placement. The authored .des x,y is the OBJECT's position in
 * world coordinates (the PR-36/PR-44 scroll legs pin that: seen at x - cam);
 * the element subtracts its own anchor f0/f2 and a negative frame number
 * mirrors it - the game's element draw methods do exactly that and nothing
 * in the actor adds the anchor back (PR-49, machine code of the four draw
 * methods, ll_seq_draw_elem_anchor). The art is the table the object's
 * CURRENT state selected (obj_art_of). */
static void obj_draw(const lvl_obj *o, unsigned char *canvas, int cw,
                     int ch, int cam, int camy)
{
    if (!obj_visible(o, cw, ch, cam, camy))
        return;
    const obj_art *a = obj_art_of(o);
    int mirror;
    const ll_seq_elem *e = obj_elem(o, &mirror);
    if (!a || !e)
        return;
    ll_seq_draw_elem_anchor(&a->seq, e, canvas, cw, ch, o->x - cam,
                            o->y - camy, mirror, NULL);
}

static void obj_advance(lvl_obj *o, unsigned dt_ms)
{
    ll_mov_t *mov = (o->sc && o->sc->mov) ? o->sc->mov : NULL;

    o->t += dt_ms;
    if (!mov || !mov->nst)          /* no script on these containers: stop */
        return;
    unsigned guard = 0;
    for (;;) {
        /* FIX-13 (X-11): corrupt .MOV with all-zero frame durations would
         * cycle states forever; one full pass without consuming time =
         * stop. ll_mov_parse() clamps non-positive ms to 100 (M4-C), so
         * this is a belt-and-braces bound on the CYCLE itself (a real
         * dt <= 200 ms consumes at most a few hundred ms-granular
         * frames; 4096 = way beyond any legitimate pass). The E-03
         * hardening covered frame indices; durations end here. */
        if (++guard > 4096)
            break;
        ll_mov_st *s = &mov->st[o->sti];
        if (s->nfr == 0 || o->fi >= s->nfr) {   /* state end -> next state   */
            o->sti = (o->sti + 1) % mov->nst;
            o->fi = 0;
            o->x = o->bx;
            o->y = o->by;                     /* motion deltas are per state */
            s = &mov->st[o->sti];
            if (s->nfr == 0)
                break;
        }
        ll_mov_fr *f = &s->fr[o->fi];
        if (o->t < f->ms)
            break;
        o->t -= f->ms;
        o->x += f->dx;             /* the authored "N,ms, M dx,dy" motion  */
        o->y += f->dy;             /* directive of the frame being held    */
        o->fi++;
    }
}

/* PR-49: the .MOV frame-number grammar and the anchored/mirrored element
 * draw, proven without game data. One element: shape+art (w0=9: dc80 codec,
 * margin lists), b19=0, b15=4 (tw 4), b17=1, anchor f0=3 f2=2, a single row
 * {gap 0, copy 1 2 3 4}. Expected (hand-derived from the game's draw
 * methods, ll_seq.c comment):
 *   plain    at (10,10): origin (7,8)  -> row y=8,  x=7..10  = 1 2 3 4
 *   mirrored at (10,10): origin (9,8)  -> row y=8,  x=9..12  = 4 3 2 1
 *   sentinel f0=0x7fff:  plain (10,10) -> row y=10, x=10..13 = 1 2 3 4
 *                        mirrored      -> row y=10, x=6..9   = 4 3 2 1 */
static int seq_anchor_selftest(void)
{
    static const unsigned char blob[] = {
        0x01, 0x00,                                 /* count 1             */
        0x09, 0x00, 0x00, 0x00, 0x04, 0x00, 0x01, 0x00,   /* w0 b19 b15 b17 */
        0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,   /* f0 f2 f4 f6    */
        0x00, 0x00,                                 /* ml {0}, mr {0}      */
        0x08, 0x00,                                 /* array 0: 8 bytes    */
        0x86, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04, 0x80,
    };
    unsigned char sent[sizeof blob];
    ll_seq sq;
    unsigned char cv[40 * 20];
    int mir, x, ok = 1;
    static const struct { int n, idx, mir, blank; } refs[] = {
        { 1, 0, 0, 0 }, { 5, 4, 0, 0 }, { -1, 0, 1, 0 }, { -7, 6, 1, 0 },
        { 0, 0, 0, 1 },
    };

    for (unsigned i = 0; i < sizeof refs / sizeof refs[0]; i++) {
        ll_frame_ref r = ll_mov_frame_ref(refs[i].n);
        if (r.idx != refs[i].idx || r.mirror != refs[i].mir ||
            r.blank != refs[i].blank) {
            fprintf(stderr, "anchor selftest: FAILED frame number %d means"
                            " idx %d mirror %d blank %d (want %d %d %d)\n",
                    refs[i].n, r.idx, r.mirror, r.blank, refs[i].idx,
                    refs[i].mir, refs[i].blank);
            return 1;
        }
    }
    memcpy(sent, blob, sizeof blob);
    sent[10] = 0xff;                                /* f0 = 0x7fff         */
    sent[11] = 0x7f;
    for (int variant = 0; variant < 2 && ok; variant++) {
        if (ll_seq_parse(variant ? sent : blob, (uint32_t)sizeof blob,
                         &sq) != 0) {
            fprintf(stderr, "anchor selftest: FAILED synthetic .SEQ did not"
                            " parse\n");
            return 1;
        }
        for (mir = 0; mir < 2 && ok; mir++) {
            int row = variant ? 10 : 8;
            int x0 = variant ? (mir ? 6 : 10) : (mir ? 9 : 7);
            static const unsigned char fwd[4] = { 1, 2, 3, 4 };
            memset(cv, 0, sizeof cv);
            ll_seq_draw_elem_anchor(&sq, &sq.els[0], cv, 40, 20, 10, 10, mir,
                                    NULL);
            for (x = 0; x < 40; x++) {
                int k = x - x0;
                unsigned char want = (k >= 0 && k < 4)
                                         ? fwd[mir ? 3 - k : k] : 0;
                if (cv[row * 40 + x] != want) {
                    fprintf(stderr, "anchor selftest: FAILED %s %s pixel"
                                    " (%d,%d) = %u (want %u)\n",
                            variant ? "sentinel" : "anchored",
                            mir ? "mirrored" : "plain", x, row,
                            cv[row * 40 + x], want);
                    ok = 0;
                    break;
                }
            }
            for (x = 0; x < 40 * 20 && ok; x++)     /* nothing else drawn */
                if (cv[x] && (x / 40 != row)) {
                    fprintf(stderr, "anchor selftest: FAILED stray pixel"
                                    " at row %d\n", x / 40);
                    ok = 0;
                }
        }
        ll_seq_free(&sq);
    }
    return ok ? 0 : 1;
}

/* PR-48: the TWO patch row codecs (ll_seq.h), proven without game data.
 * One row, written for FUN_0001d9b8 (w0&8 clear):
 *     gap 2, copy 1 byte {7}, gap 0x45, copy 1 byte {9}
 * The 0x45 is a plain 69-pixel advance there. FUN_0001dc80 (w0&8 set)
 * reads the same byte in the marker position as a SHORT FILL (0x40..0x7f
 * after a copy run), so the two codecs paint different pixels: this is
 * exactly what moved 48 shipped blobs (the lion's LEW*.ZAR/I.SEQ, the BERLO
 * map cursor) when PR-45/46 decoded every element with the dc80 rule. */
static int seq_codec_selftest(void)
{
    static const unsigned char row[] = {
        0x86, 0x02, 0x01, 0x07, 0x45, 0x01, 0x09,   /* the row            */
        0x80,                                       /* end marker row     */
    };
    unsigned char a[100], b[100];
    ll_seq_elem e1 = { 0 }, e15 = { 0 }, e12 = { 0 };
    int i;

    e1.w0 = 1;
    e15.w0 = 15;
    e12.w0 = 12;
    if (ll_seq_elem_codec(&e1) != LL_SEQ_CODEC_D9B8 ||
        ll_seq_elem_codec(&e15) != LL_SEQ_CODEC_DC80 ||
        ll_seq_elem_codec(&e12) != LL_SEQ_CODEC_DC80) {
        fprintf(stderr, "codec selftest: FAILED w0&8 does not pick the"
                        " game's codec\n");
        return 1;
    }
    memset(a, 0, sizeof a);
    memset(b, 0, sizeof b);
    if (ll_seq_patch_draw_codec(row, sizeof row, LL_SEQ_CODEC_D9B8, a, 100, 1,
                                0, 0, NULL) < 0) {
        fprintf(stderr, "codec selftest: FAILED d9b8 row rejected\n");
        return 1;
    }
    for (i = 0; i < 100; i++) {
        unsigned char want = i == 2 ? 7 : i == 72 ? 9 : 0;
        if (a[i] != want) {
            fprintf(stderr, "codec selftest: FAILED d9b8 pixel %d = %u"
                            " (want %u)\n", i, a[i], want);
            return 1;
        }
    }
    /* the SAME bytes under the other codec must NOT give that picture
     * (it may even be malformed there: both outcomes prove the split) */
    if (ll_seq_patch_draw_codec(row, sizeof row, LL_SEQ_CODEC_DC80, b, 100, 1,
                                0, 0, NULL) >= 0 && memcmp(a, b, sizeof a) == 0) {
        fprintf(stderr, "codec selftest: FAILED dc80 decoded the d9b8 row"
                        " identically (the codecs are not distinct)\n");
        return 1;
    }
    return 0;
}

/* PR-46: the element SHAPE geometry (ll_seq_shape_*), proven without any
 * game data or window, from a 9-element .SEQ payload built by hand below.
 * The bytes and every expected number were derived by hand from the game's
 * own functions (FUN_000420b0, FUN_00042a70, and the loader's grammar) and
 * independently reproduced by tools/shape_geom.py over the same payload -
 * the port is checked against BOTH, never against itself. Pinned:
 *   - the shape frame int16(b19)*0x50 + int16(b15) (E7: 1*0x50+8 = 88) and
 *     the participation gate: an element without margin lists (E8, w0=1,
 *     b17=0) has NO shape - it takes part in no hit test (H13);
 *   - the box math: the edge-marker runs (E1: rows of 0xFF skipped at the
 *     ends, so t=2/b=0 and h = b - t + 2 comes out 0), the all-empty
 *     degenerate case (E2: BOTH lists wholly 0xFF -> the game stores
 *     w = 1 - tw = -19, h = 1 - rows = -2, ll_seq_shape_empty = 1), the
 *     anchor shift x -= f0, y -= f2 and its 0x7fff ctor sentinel (E3 vs
 *     E4: the same box, shifted only for the non-sentinel element), and a
 *     per-row RIGHT margin (E5: w = (tw - r) - l + 1 = 6);
 *   - the hit test: the x and y adjacency bounds of a full-rectangle
 *     shape (H1-H4: x=19 hits, x=20 does not; y=3 hits, y=4 does not -
 *     the bounds are strict, exactly as the game's), the row window
 *     (H5/H6: E6 sits one row ABOVE E7, so the game's window pairs E6's
 *     rows 1..2 - left margin 19, right margin 5, i.e. x edges 19 and 15
 *     - with E7's rows 0..1, and E7 overlaps at x=14 but only touches
 *     those edges at x=15 (the game's compares are strict). With the
 *     transposed window PR-46 fixes, the same positions paired E6's wide
 *     row 0 with E7's row 1 instead, and answered hit at x=15 - wrong),
 *     the anchor adjustments
 *     (H7-H9b: same geometry, hit only when the anchor shift was applied,
 *     and the sentinel element behaves as unshifted, H8), the facing
 *     mirror (H11/H12: facing < 0 swaps the two margin lists and shifts
 *     x by -(tw - f0'), so the mirrored narrow side faces the other way
 *     and the hit disappears), and the empty-row skip (H14: E2's rows are
 *     all the marker - no row takes part, no hit at any position). */
static int seq_shape_selftest(void)
{
    /* built by tools/pr46_synth twin (kept in the PR record): 9 elements,
     * 186 bytes, exact consumption asserted below through parse success
     * plus the element count; shape-only elements (w0=2) carry no patch
     * sub-blocks, E8 carries one empty array (its single sub-block). */
    static const unsigned char blob[] = {
        0x09, 0x00, 0x02, 0x00, 0x00, 0x00, 0x14, 0x00, 0x04, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x14, 0x00, 0x03, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x02, 0x01, 0xff, 0xff,
        0x02, 0x00, 0x00, 0x00, 0x14, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x02, 0x00,
        0x00, 0x00, 0x0a, 0x00, 0x01, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x01, 0x00,
        0xff, 0x7f, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
        0x00, 0x00, 0x0a, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x05, 0x02, 0x00, 0x00, 0x00, 0x14, 0x00, 0x04, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x13, 0x00,
        0x00, 0x05, 0x05, 0x00, 0x02, 0x00, 0x01, 0x00, 0x08, 0x00, 0x02, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    /* per element: shape frame, participation count, FUN_000420b0 box,
     * shape_empty flag. E8 has no lists: box values are not applicable
     * (ll_seq_shape_bbox is only called for elements WITH lists). */
    static const struct {
        int tw, rows, part;
        int bx, by, bw, bh;
        int empty;
    } exp[] = {
        {  20, 4, 4,   0,  0,  21,  5, 0 },   /* E0 full-rectangle     */
        {  20, 3, 3,   2,  2,  18,  0, 0 },   /* E1 edge 0xFF runs     */
        {  20, 3, 3,  20,  3, -19, -2, 1 },   /* E2 both lists empty   */
        {  10, 1, 1,  -3, -2,  11,  2, 0 },   /* E3 anchor 3,2         */
        {  10, 1, 1,   0,  0,  11,  2, 0 },   /* E4 anchor 0x7fff      */
        {  10, 1, 1,   0,  0,   6,  2, 0 },   /* E5 right margin 5     */
        {  20, 4, 4,   0,  0,  21,  5, 0 },   /* E6 narrow middle rows */
        {  88, 2, 2,   0,  0,  89,  3, 0 },   /* E7 tw = 1*0x50 + 8    */
        {   8, 0, 0,   0,  0,   0,  0, 0 },   /* E8 no lists: no shape */
    };
    /* a, ax, ay, fa, b, bx, by, fb, expected hit */
    static const struct {
        int a, ax, ay, fa, b, bx, by, fb, want;
    } hits[] = {
        { 0, 0, 0,  1, 0, 19, 0, 1, 1 },   /* H1  x edge, adjacent    */
        { 0, 0, 0,  1, 0, 20, 0, 1, 0 },   /* H2  x edge, touching    */
        { 0, 0, 0,  1, 0,  0, 3, 1, 1 },   /* H3  y window, 1 row     */
        { 0, 0, 0,  1, 0,  0, 4, 1, 0 },   /* H4  y window, none left */
        { 6, 0, -1, 1, 7, 15, 0, 1, 0 },   /* H5  window pairing      */
        { 6, 0, -1, 1, 7, 14, 0, 1, 1 },   /* H6  window pairing      */
        { 3, 0, 2,  1, 0,  8, 0, 1, 0 },   /* H7  anchor x shift      */
        { 4, 0, 2,  1, 0,  8, 0, 1, 1 },   /* H8  sentinel: no shift  */
        { 3, 0, 0,  1, 7,  0, 2, 1, 0 },   /* H9  anchor y: no rows   */
        { 3, 0, 2,  1, 7,  0, 2, 1, 0 },   /* H9b anchor y puts A     */
        { 3, 0, 4,  1, 7,  0, 2, 1, 1 },   /*     above, then below B */
        { 5, 0, 0,  1, 0,  1, 0, 1, 1 },   /* H11 facing +: [0,5)     */
        { 5, 0, 0, -1, 0,  1, 0, 1, 0 },   /* H12 facing -: [-10,0)   */
        { 0, 0, 0,  1, 8,  0, 0, 1, 0 },   /* H13 no lists: no shape  */
        { 2, 0, 0,  1, 0,  0, 0, 1, 0 },   /* H14 all rows empty      */
    };
    ll_seq sq;
    unsigned i;

    if (ll_seq_parse(blob, (uint32_t)sizeof blob, &sq) != 0 ||
        sq.count != sizeof exp / sizeof exp[0]) {
        fprintf(stderr, "shape selftest: FAILED the synthetic .SEQ did not"
                        " parse as built\n");
        return 1;
    }
    for (i = 0; i < sq.count; i++) {
        const ll_seq_elem *e = &sq.els[i];
        int tw, rows, part, bx = 0, by = 0, bw = 0, bh = 0, empty;

        ll_seq_shape_wh(e, &tw, &rows, &part);
        if (tw != exp[i].tw || rows != exp[i].rows || part != exp[i].part) {
            fprintf(stderr, "shape selftest: FAILED E%u shape frame "
                            "tw %d rows %d part %d (want %d %d %d)\n",
                    i, tw, rows, part, exp[i].tw, exp[i].rows, exp[i].part);
            ll_seq_free(&sq);
            return 1;
        }
        if ((e->w0 & 3) && rows > 0)
            ll_seq_shape_bbox(&sq, e, &bx, &by, &bw, &bh);
        empty = ll_seq_shape_empty(&sq, e);
        if (bx != exp[i].bx || by != exp[i].by || bw != exp[i].bw ||
            bh != exp[i].bh || empty != exp[i].empty) {
            fprintf(stderr, "shape selftest: FAILED E%u box %d %d %d %d"
                            " empty %d (want %d %d %d %d empty %d)\n",
                    i, bx, by, bw, bh, empty, exp[i].bx, exp[i].by,
                    exp[i].bw, exp[i].bh, exp[i].empty);
            ll_seq_free(&sq);
            return 1;
        }
    }
    for (i = 0; i < sizeof hits / sizeof hits[0]; i++) {
        int got = ll_seq_shape_hit(&sq, &sq.els[hits[i].a],
                                   hits[i].ax, hits[i].ay, hits[i].fa,
                                   &sq.els[hits[i].b],
                                   hits[i].bx, hits[i].by, hits[i].fb);
        if (got != hits[i].want) {
            fprintf(stderr, "shape selftest: FAILED E%d@%d,%d facing %d vs"
                            " E%d@%d,%d facing %d: hit %d (want %d)\n",
                    hits[i].a, hits[i].ax, hits[i].ay, hits[i].fa,
                    hits[i].b, hits[i].bx, hits[i].by, hits[i].fb,
                    got, hits[i].want);
            ll_seq_free(&sq);
            return 1;
        }
    }
    ll_seq_free(&sq);
    return 0;
}

/* FIX-13 (X-11) selftest, test-only entry point (ll_level.h): proves the
 * two .MOV duration invariants without any window or game data:
 *   1. ll_mov_parse clamps non-positive durations (the M4-C rule,
 *      pinned);
 *   2. obj_advance terminates on an all-zero-ms cycle (the guard). The
 *      proof is that this RETURNS - the CI legs run it under a timeout,
 *      so a regression HANGS (caught) instead of passing silently. */
int ll_mov_selftest(void)
{
    ll_mov_t *m = calloc(1, sizeof *m);
    if (!m) {
        fprintf(stderr, "mov selftest: FAILED out of memory\n");
        return 1;
    }
    ll_mov_parse("[A]\n0,0\n1,0\n2,-5\n[B]\n0,5\n", m);
    if (m->nst != 2 || m->st[0].nfr != 3 || m->st[1].nfr != 1) {
        fprintf(stderr, "mov selftest: FAILED parse shape "
                        "(nst=%d nfr=%d/%d)\n",
                m->nst, m->st[0].nfr, m->st[1].nfr);
        free(m);
        return 1;
    }
    /* PR-51: the declared state list picks the start state (box40.mov
     * shape: header A_RUN, A_DEAD but the [A_DEAD] block written first) */
    ll_mov_parse("A_RUN\nA_DEAD\n\n[A_DEAD]\n0,100\n\n[A_RUN]\n1,50\n", m);
    if (m->nst != 2 || m->start_st != 1 ||
        strcmp(m->first_decl, "A_RUN") != 0) {
        fprintf(stderr, "mov selftest: FAILED declared start state"
                        " (start_st=%d decl='%s')\n", m->start_st,
                m->first_decl);
        free(m);
        return 1;
    }
    ll_mov_parse("[A]\n0,0\n1,0\n2,-5\n[B]\n0,5\n", m);
    if (m->start_st != 0 || m->first_decl[0]) {
        fprintf(stderr, "mov selftest: FAILED no declaration must mean"
                        " state 0\n");
        free(m);
        return 1;
    }
    for (int k = 0; k < 3; k++)
        if (m->st[0].fr[k].ms != 100u) {
            fprintf(stderr, "mov selftest: FAILED zero/negative duration "
                            "not clamped (frame %d ms=%u)\n",
                    k, m->st[0].fr[k].ms);
            free(m);
            return 1;
        }
    if (m->st[1].fr[0].ms != 5u) {
        fprintf(stderr, "mov selftest: FAILED positive duration changed "
                        "(ms=%u)\n", m->st[1].fr[0].ms);
        free(m);
        return 1;
    }
    /* The corrupt shape X-11 describes, BUILT DIRECTLY to bypass the
     * parser clamp (a future patch path or caller must not be able to
     * hang the scene loop either): */
    obj_script sc;
    memset(&sc, 0, sizeof sc);
    for (int k = 0; k < LMOV_MAXST; k++)
        sc.ast[k] = -1;
    sc.mov = calloc(1, sizeof *sc.mov);
    if (!sc.mov) {
        fprintf(stderr, "mov selftest: FAILED out of memory\n");
        free(m);
        return 1;
    }
    sc.mov->nst = 1;
    sc.mov->st[0].nfr = LMOV_MAXFR;
    for (int k = 0; k < LMOV_MAXFR; k++)
        sc.mov->st[0].fr[k].ms = 0;
    lvl_obj o;
    memset(&o, 0, sizeof o);
    o.sc = &sc;
    obj_advance(&o, 60000u);       /* without the guard: spins forever */
    if (o.sti != 0) {              /* single state: must not wander     */
        fprintf(stderr, "mov selftest: FAILED guard left state %d\n",
                o.sti);
        free(sc.mov);
        free(m);
        return 1;
    }

    /* PR-45: the art an object draws is exactly the table its CURRENT state
     * names through seq=. Built directly here (not through the parser):
     * state 1 must get slot 1; a state that names no art, or names one this
     * install does not carry (a reported data hole), must draw NOTHING -
     * never another state's table. And an art table that IS there but whose
     * selected element carries only the SHAPE margins, no patch art (an
     * invisible solid, PLAN 8h), must be reported as that table and still
     * never drawn. */
    sc.mov->nst = 2;
    sc.nart = 2;
    sc.ast[0] = 0;
    sc.ast[1] = -1;                       /* state names no seq= art */
    sc.art[0].found = sc.art[1].found = 1;
    sc.art[0].eidx0 = sc.art[1].eidx0 = 0;
    sc.art[1].sw = sc.art[1].sh = 8;
    o.sti = 1;
    if (obj_art_of(&o) != NULL) {
        fprintf(stderr, "mov selftest: FAILED a state without seq= drew"
                        " another state's art (%p)\n", (void *)obj_art_of(&o));
        free(sc.mov);
        free(m);
        return 1;
    }
    sc.ast[1] = 1;
    if (obj_art_of(&o) != &sc.art[1]) {
        fprintf(stderr, "mov selftest: FAILED a state did not select its own"
                        " seq= art\n");
        free(sc.mov);
        free(m);
        return 1;
    }
    if (!obj_visible(&o, 640, 400, 0, 0)) {
        fprintf(stderr, "mov selftest: FAILED a selected patch-art table was"
                        " not drawable\n");
        free(sc.mov);
        free(m);
        return 1;
    }
    sc.art[1].found = 0;                  /* authored name, no record here */
    if (obj_art_of(&o) != NULL) {
        fprintf(stderr, "mov selftest: FAILED a data-hole art name was"
                        " drawn anyway\n");
        free(sc.mov);
        free(m);
        return 1;
    }
    sc.art[1].found = 1;                  /* record is there ...            */
    sc.art[1].eidx0 = -1;                 /* ... but shape-only, no patch   */
    if (obj_art_of(&o) != &sc.art[1]) {
        fprintf(stderr, "mov selftest: FAILED a shape-only art table was"
                        " not reported as the state's own\n");
        free(sc.mov);
        free(m);
        return 1;
    }
    if (obj_visible(&o, 640, 400, 0, 0)) {
        fprintf(stderr, "mov selftest: FAILED a shape-only art table was"
                        " drawn anyway\n");
        free(sc.mov);
        free(m);
        return 1;
    }

    /* PR-46: the element SHAPE geometry leg (no window, no game data). */
    if (seq_codec_selftest() != 0 || seq_anchor_selftest() != 0 ||
        seq_shape_selftest() != 0) {
        free(sc.mov);
        free(m);
        return 1;
    }

    free(sc.mov);
    free(m);
    printf("mov selftest: duration clamp + cycle guard + per-state seq= art"
           " selection + patch codecs + anchored draw + shape geometry (ok)\n");
    return 0;
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

/* ---------------- playable-level runtime (M4-B, PR-43) -------------------
 * LEO_PLAY wires the real player: art = every .SEQ in the ANIMACJE leaf
 * named by the .des `lew=<class>,<art leaf>,<mov>,<player des>` token 2,
 * script = the HD leaf's <stem>.MOV (ll_play.c), ground = the ground
 * polylines named by `platformy=` in the HD ETAPY level block - either
 * the block's <stem>.PLT record or, for the levels that say so, a
 * sub-.des record of the same block whose text IS the same polyline
 * grammar (TUNGA2.12/SADES.06 platf.des; JUNCTION none.des = "0" = no
 * ground), spawn = `pozycja=`, bounds
 * (and the fall-out-of-world respawn) = `lew_rect=`. The simulation and
 * the .MOV physics live in ll_play.c; this file only loads the data and
 * draws. If any piece is missing the level stays a preview and the gap
 * is logged (data-hole), never guessed.
 * The CI drives the same runtime headless through LEO_PLAY_SCRIPT
 * ("<tick>:<FWD|BKWD|UP|DOWN|NONE>" entries, whitespace separated; the
 * last entry at or before the current tick is held until the next one),
 * on the fixed 20 ms grid of the LEO_LEVEL_PPM leg - deterministic. */
typedef struct {
    int have;                               /* fully wired = play mode on */
    ll_plt plt;
    ll_mov_t *mov;                          /* heap: ~160 KB              */
    ll_player p;
    struct {
        char stem[16];
        unsigned char *blob;
        ll_seq seq;
    } art[32];
    int nart;
    char leaf[24];                          /* art leaf being claimed     */
    double xmin, xmax, ydead;               /* lew_rect bounds            */
    double sx, sy;                          /* pozycja spawn              */
    ll_play_keys keys;                      /* held arrows (live path)    */
} play_ctx;

static ll_play_keys script_keys(const char *s, int tick)
{
    ll_play_keys k = { 0, 0, 0, 0 };
    if (!s)
        return k;
    int best = -1;
    const char *p = s;
    while (*p) {
        char *end = NULL;
        long t = strtol(p, &end, 10);
        if (end == p || *end != ':') {
            p++;
            continue;
        }
        const char *v = end + 1;
        char tok[12];
        size_t n = 0;
        while (v[n] && v[n] != ' ' && v[n] != '\t' && n + 1 < sizeof tok) {
            tok[n] = v[n];
            n++;
        }
        tok[n] = '\0';
        if (n && (int)t <= tick && (int)t > best) {
            best = (int)t;
            k.k_fwd  = strcasecmp(tok, "FWD")  == 0;
            k.k_bkwd = strcasecmp(tok, "BKWD") == 0;
            k.k_up   = strcasecmp(tok, "UP")   == 0;
            k.k_down = strcasecmp(tok, "DOWN") == 0;
        }
        p = v + n;
    }
    return k;
}

/* feet at the spawn point, snapped down onto the first .plt surface */
static void play_spawn(play_ctx *pl)
{
    ll_xy hit;
    int g = ll_plt_first_below(&pl->plt, (int)pl->sx, (int)pl->sy, &hit);
    ll_player_start(&pl->p, pl->mov, pl->sx, g ? (double)hit.y : pl->sy, g);
}

static const ll_seq *play_art(const play_ctx *pl, const char *stem)
{
    if (!stem || !stem[0])
        return NULL;
    for (int k = 0; k < pl->nart; k++)
        if (ci_eq(pl->art[k].stem, stem))
            return &pl->art[k].seq;
    return NULL;
}

/* one fixed step + the shared logging (transition lines always; per-tick
 * lines only on the headless CI grid where they are grep-pinned) */
static void play_advance(play_ctx *pl, const ll_play_keys *keys,
                         unsigned dt_ms, int verbose, int tick)
{
    int was = pl->p.sti;
    int ev = ll_player_step(&pl->p, &pl->plt, keys, pl->xmin, pl->xmax,
                            dt_ms);
    int xi = (int)(pl->p.x >= 0 ? pl->p.x + 0.5 : pl->p.x - 0.5);
    int yi = (int)(pl->p.y >= 0 ? pl->p.y + 0.5 : pl->p.y - 0.5);
    if (ev & LL_PLAY_EV_STATE)
        printf("ll_level: play: %s -> %s\n", pl->mov->st[was].name,
               pl->mov->st[pl->p.sti].name);
    if (pl->p.y > pl->ydead) {              /* fell out of the authored   */
        printf("ll_level: play: below lew_rect at (%d,%d) -> respawn\n",   /* world */
               xi, yi);
        play_spawn(pl);
        xi = (int)pl->sx;
        yi = (int)pl->p.y;
    }
    if (verbose)
        printf("ll_level: play tick %d: state %s seq %s frame %d pos %d,%d"
               " grounded %d\n", tick, pl->mov->st[pl->p.sti].name,
               pl->mov->st[pl->p.sti].seqkey, ll_player_frame(&pl->p),
               xi, yi, pl->p.grounded);
}

/* the lion LAST, over bg + scene objects: the .SEQ anchor convention is
 * bbox origin = (-f0,-f2) relative to the feet point (ll_seq.h; verified
 * on UJ09.SEQ f0=35 f2=68, the walk cycle whose feet ARE the ground
 * line), so the draw origin is (x - cam - (short)f0, y - (short)f2). */
/* viewer camera policy: the lion's feet sit 65% down the view */
static int follow_camy(const play_ctx *pl, int ch, int cam_ymax)
{
    int y = (int)pl->p.y - (ch * 65) / 100;
    if (y < 0)
        y = 0;
    if (y > cam_ymax)
        y = cam_ymax;
    return y;
}

static void play_draw(const play_ctx *pl, unsigned char *canvas, int cw,
                      int ch, int cam, int camy)
{
    if (!pl->have || pl->p.sti >= pl->mov->nst)
        return;
    const ll_mov_st *st = &pl->mov->st[pl->p.sti];
    const ll_seq *s = play_art(pl, st->seqkey[0] ? st->seqkey
                                                 : pl->mov->seqkey);
    if (!s || !s->count)
        return;
    ll_frame_ref r = ll_mov_frame_ref(ll_player_frame(&pl->p));
    if (r.blank || r.idx >= (int)s->count)   /* same rule as obj_elem     */
        return;
    const ll_seq_elem *e = &s->els[r.idx];
    int x = (int)(pl->p.x >= 0 ? pl->p.x + 0.5 : pl->p.x - 0.5) - cam;
    int y = (int)(pl->p.y >= 0 ? pl->p.y + 0.5 : pl->p.y - 0.5) - camy;
    ll_seq_draw_elem_anchor(s, e, canvas, cw, ch, x, y, r.mirror, NULL);
}

/* Drop the placements that belong to the player's own art leaf: the player
 * draws that one himself, driven by LEW.MOV. PR-45 measured that NO shipped
 * placement names the player's leaf (the player's attached objects - POCISK,
 * harpun, arrow, balon - are listed in the player leaf's own .des, which is
 * the K_FIRE/POCISK feature, M4-D), so this is only the safety net that
 * keeps exactly one lion on screen. Scripts are shared with the rest of the
 * scene, so nothing is freed here: the placement goes, the script stays. */
static void play_claim_leaf(lvl_obj *objs, int *nobj, const play_ctx *pl)
{
    int w = 0;
    for (int k = 0; k < *nobj; k++) {
        if (objs[k].sc && ci_eq(objs[k].sc->leaf, pl->leaf))
            continue;
        if (w != k)
            objs[w] = objs[k];
        w++;
    }
    if (w < *nobj)
        printf("ll_level: play: claimed leaf %s (%d placement(s) now"
               " player-driven)\n", pl->leaf, *nobj - w);
    *nobj = w;
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

/* PCM overlay layers (PR-53). tlo= names the background stack back to
 * front: the first .PCX is the backdrop, every .PCM is a TRANSPARENT overlay
 * painted over it in tlo order (ll_pcm.h). Where a layer is as wide/tall as
 * the world (KADMAR2 2560x1800, LONA.05 2480x600, TUNGA.11 2560x600 - the
 * back_rect itself) it scrolls 1:1 with the camera; a smaller layer scrolls
 * proportionally so that its far edge meets the world's far edge (VIEWER
 * RULE: the engine's per-layer parallax factors, FUN_00057460, are not
 * recovered; strips wrap modulo the layer width as in the engine), and a
 * layer shorter than the view sits on the bottom edge (the engine
 * offsets layers by (height - 200)). The .PCX backdrop itself does not
 * scroll (PR-51). */
#define MAX_PCM_LAYERS 8

static void layers_at(const ll_pcm *lay, int nlay, int cam, int camy, int cw,
                      int ch, int world_w, int world_h, unsigned char *canvas)
{
    for (int k = 0; k < nlay; k++) {
        const ll_pcm *L = &lay[k];
        long long sx = cam, sy = camy;
        int dy = 0;
        if (world_w > cw && (int)L->w > cw && (int)L->w < world_w)
            sx = (long long)cam * ((int)L->w - cw) / (world_w - cw);
        if ((int)L->h < ch) {
            dy = ch - (int)L->h;
            sy = 0;
        } else if (world_h > ch && (int)L->h < world_h)
            sy = (long long)camy * ((int)L->h - ch) / (world_h - ch);
        ll_pcm_blit(L, canvas, cw, ch, (int)sx, (int)sy, 0, dy);
    }
}

/* 4b) the authored scene: the level's own placement data (PR-45) ---------
 * A level says what stands where in sub-.des files that its main .des names
 * through THREE keys - measured over all 13 shipped levels. Every .des record
 * inside an HD level block is named by exactly one key of that level's main
 * .des (0 orphans), and the three keys below are the ones that name SCENE
 * objects; the rest belong to other features (lew= the player's own .des,
 * platformy= the ground, belka= the HUD bar, sample= / sufity= / blokady= /
 * demolist=). So the PR-36 fallback of inventing objects from every leaf .SEQ
 * is gone:
 *   zwierzaki=<up to 3 .des>  "animals": the scripted scene actors (RAN.01
 *                             chmury.des = 184 cloud placements, ptaki.des)
 *   animacje=<up to 3 .des>   "animations": props and effects (TUNGA.11
 *                             ANI3.DES = the 11 objects this viewer showed
 *                             by guessing before)
 *   ani_plat=<up to 3 .des>   platform animations: 10 of those files are
 *                             placement files, 1 holds polylines only and 2
 *                             carry both (moving ground: ENA2 plat.des has
 *                             31 polyline lines and 14 authored object
 *                             entries) - the polylines are ground, owned by
 *                             the platformy / AC_* signal work (M4-D)
 * A bare `none` in such a list is the game's OWN keyword for "nothing here"
 * (JUNCTION says animacje=none, RAN.01 says zwierzaki=chmury.des,ptaki.des,
 * none): it is NOT a missing record and never a data hole. `none.des` on the
 * other hand IS a real record name (JUNCTION's level block carries a
 * NONE.DES holding literally "0"), and the exact-name match below resolves it
 * normally.
 *
 * Inside a placement file (grammar measured over the 68 files the three keys
 * name - 18 zwierzaki + 38 animacje + 12 ani_plat references, none of them
 * naming a missing record - and 2188 entry lines, 2026-11-16): a count line,
 * then one entry per line
 *   "<LEAF>,<class>,<SCRIPT>.<ext>,<x>,<y>"        world x,y, y down
 * Names padded with spaces are the game's own formatting (109 of the lines,
 * e.g. RAN.01 dymy.des writes `, 16500, 360` and ZARIA.02 anipl2.des
 * `JROPE, dummy, jrope.mov, 490, 230`): the name is the same, so they are
 * parsed as the same name. 4 entries carry parameters after the coordinates
 * (KADMAR.07 `KPALMA,xxx,kpal.mov,640,640  -37,-7  -1 100  0`, ZARIA.01
 * `KAMIEN,dummy,kamien1.mov,1360,250,1308`): placed at their authored x,y,
 * their extra parameters printed verbatim and counted, never guessed at. 1
 * entry names a script with no extension at all (KADMAR.07 ANI1.DES
 * `BERLO,dummy,sdfsdf,61,54`) - nothing resolves from it, and it is reported
 * as the data hole in the placement that it is. Alongside the entries these
 * files also carry per-entry property lines ("damage=100", "bounds=40,6550",
 * "scope=0,0", "energy=150" for RAN.01's clouds: combat data this viewer does
 * not model), group header lines, the game's own comment lines starting with
 * ';' or '//' (SADES.06 ANIM2.DES carries the commented-out placement
 * `;ANIBACK,none,chmury.mov,270,90`), and polyline lines of the shape
 * platformy= files use. All of that is counted and named in the report.
 * The count line is NOT trusted: it disagrees with the number of entry lines
 * in 38 of the 68 files (ENA.03 ZWIERZ3.DES declares 88 and holds 134), so
 * every line is read and the real number is what gets reported.
 *
 * The first field names the ANIMACJE leaf table on BOTH installs of the same
 * level: the HD one holds <LEAF>/<SCRIPT>.MOV (the motion script, PLAN 8g),
 * the CD one holds the art - and the art is whatever the script names for the
 * state it is playing: every [STATE] block carries "seq=<name>", naming
 * <NAME>.SEQ in the SAME leaf. Measured over the 708 distinct (level, leaf,
 * script) triples those 2188 entries use: 681 have their .MOV on the HD
 * install (27 are real data holes, listed by exact name below); those 681
 * scripts name 960 distinct (leaf, seq=) art pairs, 953 of which are on this
 * CD (7 are true data holes). What those tables' elements carry (measured
 * 2026-11-16 over exactly those joins, at element level): 8048 carry patch art AND the per-row shape
 * margins (w0=15), 92 carry patch art only (w0=1), and 169 carry the margins
 * with NO patch array at all (w0=2) - invisible solids: the original game's
 * own draw paths are all gated on its patch pointer array, so it never drew
 * those either. The report counts all three, and names the shape-only tables
 * with the real geometry their margins give (ll_seq_shape_*): never a data
 * hole, and no longer called a draw gap. The .mov stem matches one of its
 * OWN seq= names in only 439 of the 681, so joining by stem (what PR-36 did)
 * invented the art of the rest and the 469 "not on these containers (data
 * hole)" notes it printed were false.
 * The <class> token (none / dummy / xxx / balonik.ani ...) is the game's
 * object-class hint; the *.ani names match NO record anywhere in either
 * container (0 .ANI records exist), so they are counted, never used to pick
 * art. */

static const char *const des_place_keys[] = { "zwierzaki", "animacje",
                                              "ani_plat" };

typedef struct {                    /* ANIMACJE of both installs; either may
                                     * be absent on a partial install      */
    llrs *cd;                       /* CD install: the .SEQ art            */
    const llrs_slot *cd_blk;
    llrs *hd;                       /* HD install: the .MOV scripts        */
    const llrs_slot *hd_blk;
} anim_ctx;

typedef struct {                    /* counted while reading, never guessed */
    int files, file_missing, entries, placed, coord_bad;
    int nscript, nscript_missing;
    int nart, nart_missing, art_shapeonly, art_shapecarry, art_over,
        state_noart;
    int prop, group, comment, class_hint, shortline, poly;
    int param_extra, field_space, entry_noext, longline;
    int ntrans, ctrl_other, directive, field_other, drop_st, drop_fr, drop_tr;
    int fr_total, fr_blank, fr_mirror, fr_oob;   /* frame numbers: the game's
                                                  * 1-based/signed grammar */
} obj_stats;

typedef struct {                    /* the scene being built for one level */
    scene_owner *own;               /* owns the script set + placements     */
    anim_ctx ac;
    const char *level;
    int world_w;
    obj_stats st;
} place_ctx;

/* record of an exact name inside a level's ANIMACJE leaf table (leaf tables
 * are nested: block -> id==2 leaf table -> records). Exact name only:
 * nothing here is guessed by stem. */
static int leaf_record(const llrs *c, const llrs_slot *blk, const char *leaf,
                       const char *fname, llrs_slot *out)
{
    if (!c || !blk)
        return 0;
    llrs_slot *recs = NULL;
    unsigned n = 0;
    if (llrs_block(c, blk, &recs, &n))
        return 0;
    int found = 0;
    for (unsigned k = 0; k < n && !found; k++) {
        if (recs[k].id != 2 || !recs[k].name[0] || !ci_eq(recs[k].name, leaf))
            continue;
        llrs_slot *fr = NULL;
        unsigned nf = 0;
        if (llrs_block(c, &recs[k], &fr, &nf))
            continue;
        for (unsigned j = 0; j < nf; j++)
            if (fr[j].id == 1 && fr[j].name[0] && ci_eq(fr[j].name, fname)) {
                *out = fr[j];
                found = 1;
                break;
            }
        llrs_block_free(fr);
    }
    llrs_block_free(recs);
    return found;
}

static void seq_geom(obj_art *a)
{
    a->eidx0 = -1;
    for (unsigned e = 0; e < a->seq.count; e++)
        if (a->seq.els[e].arr_n > 0) {
            a->eidx0 = (int)e;
            break;
        }
    if (a->eidx0 >= 0)
        elem_size(&a->seq, &a->seq.els[a->eidx0], &a->sw, &a->sh);
    a->rx = a->ry = 0;
    for (unsigned e = 0; e < a->seq.count; e++) {
        const ll_seq_elem *el = &a->seq.els[e];
        if (el->arr_n == 0)
            continue;
        int ew, eh, ax = 0, ay = 0;
        elem_size(&a->seq, el, &ew, &eh);
        int tw = (int)(short)el->b19 * 0x50 + (int)(short)el->b15;
        if (tw > ew)
            ew = tw;
        if (el->f0 != 0x7fff) {
            ax = (int)(short)el->f0;
            ay = (int)(short)el->f2;
        }
        if (ax < 0) ax = -ax;
        if (ay < 0) ay = -ay;
        if (ew + ax > a->rx) a->rx = ew + ax;
        if (eh + ay > a->ry) a->ry = eh + ay;
    }

    /* the shape side of the same table: which elements carry the per-row
     * margin lists the game's hit/push geometry reads, and the bbox it
     * computes from them (ll_seq_shape_bbox = FUN_000420b0). Counted and
     * reported; the scene model does not use it to move anything yet. */
    a->sidx0 = -1;
    a->nshape = 0;
    for (unsigned e = 0; e < a->seq.count; e++) {
        int tw, rows, n;
        ll_seq_shape_wh(&a->seq.els[e], &tw, &rows, &n);
        if (!(a->seq.els[e].w0 & 3) || rows <= 0)
            continue;
        a->nshape++;
        if (a->sidx0 < 0) {
            a->sidx0 = (int)e;
            a->stw = tw;
            a->srows = rows;
            ll_seq_shape_bbox(&a->seq, &a->seq.els[e],
                              &a->sbx, &a->sby, &a->sbw, &a->sbh);
            a->nomargins = ll_seq_shape_empty(&a->seq, &a->seq.els[e]);
        }
    }
}

static obj_script *obj_script_new(void)
{
    obj_script *sc = calloc(1, sizeof *sc);
    if (!sc)
        return NULL;
    for (int k = 0; k < LMOV_MAXST; k++)
        sc->ast[k] = -1;
    return sc;
}

static void obj_script_free(obj_script *sc)
{
    if (!sc)
        return;
    for (int k = 0; k < sc->nart; k++) {
        ll_seq_free(&sc->art[k].seq);
        free(sc->art[k].blob);
    }
    free(sc->mov);
    free(sc->movtxt);
    free(sc);
}

static void scene_owner_free(scene_owner *own)
{
    if (!own)
        return;
    for (int k = 0; k < own->nsc; k++)
        obj_script_free(own->set[k]);
    free(own->set);
    free(own->objs);
    memset(own, 0, sizeof *own);
}

static obj_script *obj_script_find(const scene_owner *own, const char *leaf,
                                   const char *movname)
{
    for (int k = 0; k < own->nsc; k++)
        if (ci_eq(own->set[k]->leaf, leaf) &&
            ci_eq(own->set[k]->movname, movname))
            return own->set[k];
    return NULL;
}

/* Add one authored script, or return the one already loaded for it (RAN.01
 * places the same two cloud scripts 184 times: the .MOV parse and the .SEQ
 * art are loaded ONCE per distinct name pair; the placements stay separate
 * because each has its own position and its own place in the cycle).
 * Data holes are recorded on the script with the exact names searched and
 * reported once. */
static obj_script *obj_script_get(place_ctx *pc, const char *leaf,
                                 const char *movname)
{
    scene_owner *own = pc->own;
    obj_stats *st = &pc->st;
    obj_script *sc = obj_script_find(own, leaf, movname);
    if (sc)
        return sc;
    sc = obj_script_new();
    if (!sc)
        return NULL;
    snprintf(sc->leaf, sizeof sc->leaf, "%s", leaf);
    snprintf(sc->movname, sizeof sc->movname, "%s", movname);

    /* the script: HD ANIMACJE <level>/<LEAF>/<SCRIPT>.MOV, exact name */
    llrs_slot sl;
    if (leaf_record(pc->ac.hd, pc->ac.hd_blk, leaf, movname, &sl)) {
        if (llrs_read(pc->ac.hd, &sl, &sc->movtxt) < 0)
            sc->movtxt = NULL;
    }
    if (sc->movtxt) {
        sc->mov = calloc(1, sizeof *sc->mov);
        if (sc->mov)
            ll_mov_parse((const char *)sc->movtxt, sc->mov);
    }
    if (!sc->mov || !sc->mov->nst) {
        st->nscript_missing++;
        printf("ll_level: obj %s/%s: no .MOV script in that HD ANIMACJE leaf"
               " (data hole: the level places this object, the install does"
               " not carry its script)\n", sc->leaf, sc->movname);
    }

    /* the art: each state names its own .SEQ through seq= (measured: up to
     * 10 distinct names in one script), deduped by name. Three honest
     * outcomes, never a guess: the record is there and carries patch art
     * (drawable; counted further: how many of those ALSO carry the shape
     * margins), the record is there but carries shape margins only (kept,
     * counted, drawn as nothing - which is exactly what the original game
     * drew from it too), or the record is not on these containers (a data
     * hole, reported by exact name). */
    if (sc->mov) {
        for (int k = 0; k < sc->mov->nst && k < LMOV_MAXST; k++) {
            const char *key = sc->mov->st[k].seqkey;
            if (!key[0]) {                  /* state names no art at all */
                st->state_noart++;
                continue;
            }
            char artname[24];
            snprintf(artname, sizeof artname, "%s.SEQ", key);
            int idx = -1;
            for (int a = 0; a < sc->nart; a++)
                if (ci_eq(sc->art[a].name, artname)) {
                    idx = a;
                    break;
                }
            if (idx < 0) {
                if (sc->nart >= OBJ_MAXART) {
                    sc->nartover++;
                    st->art_over++;
                    continue;
                }
                obj_art *a = &sc->art[sc->nart];
                snprintf(a->name, sizeof a->name, "%s", artname);
                long asz = -1;
                int rec = leaf_record(pc->ac.cd, pc->ac.cd_blk, leaf,
                                      artname, &sl);
                if (rec)
                    asz = llrs_read(pc->ac.cd, &sl, &a->blob);
                if (asz >= 0 && ll_seq_parse(a->blob, (uint32_t)asz,
                                             &a->seq) == 0) {
                    a->found = 1;
                    snprintf(a->name, sizeof a->name, "%s",
                             sl.name);      /* report the record's own name */
                    seq_geom(a);
                    if (a->eidx0 < 0) {     /* shape margins, no patch art */
                        sc->nartshape++;
                        st->art_shapeonly++;
                    } else if (a->nshape > 0) {   /* art AND shape (w0=15) */
                        sc->nartcarry++;
                        st->art_shapecarry++;
                    }
                } else {
                    free(a->blob);
                    a->blob = NULL;
                    a->seq.count = 0;
                }
                if (!a->found) {
                    sc->nartmiss++;
                    st->nart_missing++;
                    printf("ll_level: obj %s/%s: its seq= names %s in that"
                           " ANIMACJE leaf: no such record (or no parsable"
                           " .SEQ in it) on these containers (data hole) -"
                           " the states that select it draw nothing\n",
                           sc->leaf, sc->movname, artname);
                } else if (a->eidx0 < 0) {
                    printf("ll_level: obj %s/%s: its seq= names %s: the"
                           " record is there, and element %d of it carries the"
                           " SHAPE margins only, no patch art: shape frame"
                           " tw %d, %d rows, bbox %d %d %d %d%s - an invisible"
                           " solid: every draw path in the original game needs"
                           " a patch array, so it drew nothing from this table"
                           " either, and this viewer has no object-collider"
                           " model to use the shape as yet (not a data hole,"
                           " not a draw gap)\n",
                           sc->leaf, sc->movname, a->name, a->sidx0,
                           a->stw, a->srows, a->sbx, a->sby, a->sbw, a->sbh,
                           a->nomargins
                               ? " (every margin byte of BOTH its lists is"
                                 " the empty marker, so the box above is the"
                                 " game's own degenerate result - w = 1 - tw"
                                 " and h = 1 - rows - a fact about its"
                                 " arithmetic, not a gap in the data)" : "");
                }
                idx = sc->nart++;
                st->nart++;
            }
            sc->ast[k] = idx;
        }
        for (int k = 0; k < sc->mov->nst; k++) {
            const ll_mov_st *s = &sc->mov->st[k];
            st->ntrans += s->ntr;
            st->ctrl_other += s->ctrl_other;
            st->directive += s->directive;
            st->field_other += s->field_other;
            int ai = k < LMOV_MAXST ? sc->ast[k] : -1;
            if (ai >= 0 && ai < sc->nart && sc->art[ai].found)
                for (int f = 0; f < s->nfr; f++) {
                    ll_frame_ref r = ll_mov_frame_ref(s->fr[f].frame);
                    st->fr_total++;
                    if (r.blank)
                        st->fr_blank++;
                    else if (r.idx >= (int)sc->art[ai].seq.count)
                        st->fr_oob++;
                    else if (r.mirror)
                        st->fr_mirror++;
                }
        }
        st->drop_st += sc->mov->drop_st;
        st->drop_fr += sc->mov->drop_fr;
        st->drop_tr += sc->mov->drop_tr;
    }

    st->nscript++;
    if (own->nsc == own->capsc) {
        int nc = own->capsc ? own->capsc * 2 : 128;
        obj_script **p = realloc(own->set, (size_t)nc * sizeof *p);
        if (!p) {
            obj_script_free(sc);
            return NULL;
        }
        own->set = p;
        own->capsc = nc;
    }
    own->set[own->nsc++] = sc;
    return sc;
}

static lvl_obj *obj_slot(place_ctx *pc)
{
    scene_owner *own = pc->own;
    if (own->nobj == own->capobj) {
        int nc = own->capobj ? own->capobj * 2 : 64;
        lvl_obj *p = realloc(own->objs, (size_t)nc * sizeof *p);
        if (!p)
            return NULL;
        own->objs = p;
        own->capobj = nc;
    }
    lvl_obj *o = &own->objs[own->nobj++];
    memset(o, 0, sizeof *o);
    return o;
}

/* One comma-separated name field of a placement line. Spaces around the field
 * are the game's OWN formatting (measured: RAN.01 dymy.des writes `, 16500,
 * 360` and ZARIA.02 anipl2.des `JROPE, dummy, jrope.mov, 490, 230`), so they
 * are not part of the name; a name with a space INSIDE it is not a name (that
 * is what distinguishes a polyline line "1 1495,147;..." from an entry).
 * Returns the position AFTER the ',' that must follow the field, or NULL. */
static const char *place_field(const char *p, const char *end, char *out,
                               size_t cap)
{
    while (p < end && (*p == ' ' || *p == '\t'))
        p++;
    const char *e = p;
    while (e < end && *e != ',')
        e++;
    if (e >= end)
        return NULL;                      /* a field list needs a separator */
    const char *t = e;
    while (t > p && (t[-1] == ' ' || t[-1] == '\t'))
        t--;
    size_t n = (size_t)(t - p);
    if (!n || n >= cap || memchr(p, ' ', n) || memchr(p, '\t', n))
        return NULL;
    memcpy(out, p, n);
    out[n] = '\0';
    return e + 1;
}

/* one integer of a placement line: spaces are allowed before it (see above);
 * returns the position AFTER its last digit, or NULL if there are no digits */
static const char *place_int(const char *p, const char *end, int *v)
{
    while (p < end && (*p == ' ' || *p == '\t'))
        p++;
    const char *s = p;
    if (p < end && (*p == '-' || *p == '+'))
        p++;
    const char *d = p;
    while (p < end && *p >= '0' && *p <= '9')
        p++;
    if (p == d)
        return NULL;
    *v = (int)strtol(s, NULL, 10);       /* digits only, no overflow: the
                                          * widest coordinate is 6 characters */
    return p;
}

/* the ',' between the two coordinates, spaces around it allowed */
static const char *place_comma(const char *p, const char *end)
{
    while (p < end && (*p == ' ' || *p == '\t'))
        p++;
    if (p >= end || *p != ',')
        return NULL;
    return p + 1;
}

/* read one placement file: every line is classified, nothing is skipped in
 * silence and nothing is invented */
static void des_place_file(place_ctx *pc, const char *fname,
                           const char *txt, size_t len)
{
    obj_stats *st = &pc->st;
    const char *p = txt, *end = txt + len;
    int declared = -1, entries = 0;
    while (p < end) {
        const char *eol = p;
        while (eol < end && *eol != '\n' && *eol != '\r')
            eol++;
        const char *b = p;
        while (b < eol && (*b == ' ' || *b == '\t'))
            b++;
        const char *e = eol;
        while (e > b && (e[-1] == ' ' || e[-1] == '\t'))
            e--;
        size_t ll = (size_t)(e - b);
        char line[512];
        if (ll && ll < sizeof line) {
            memcpy(line, b, ll);
            line[ll] = '\0';
        } else {
            line[0] = '\0';              /* measured longest line in the 68
                                          * shipped files is 207 bytes */
            if (ll)
                st->longline++;
        }
        int commas = 0;
        for (const char *q = b; q < e; q++)
            if (*q == ',')
                commas++;

        if (!ll)
            ;                                   /* blank: nothing to say */
        else if (line[0] == ';' || (ll > 1 && line[0] == '/' &&
                                    line[1] == '/'))
            st->comment++;                      /* the game's comment marks */
        else if (declared < 0 && strspn(line, "0123456789") == ll)
            declared = atoi(line);      /* header count: kept for the report,
                                         * never used to stop reading */
        else if (commas < 4) {                  /* no entry shape at all */
            if (strchr(line, '='))
                st->prop++;                     /* damage= / energy= / ... */
            else if (strchr(line, ';'))
                st->poly++;                     /* polyline data (ground) */
            else
                st->group++;
        } else {
            char leaf[16], cls[24], mov[24];
            int x = 0, y = 0;
            const char *eos = line + ll;
            const char *c1 = place_field(line, eos, leaf, sizeof leaf);
            const char *c2 = c1 ? place_field(c1, eos, cls, sizeof cls) : NULL;
            const char *c3 = c2 ? place_field(c2, eos, mov, sizeof mov) : NULL;
            const char *rx = c3 ? place_int(c3, eos, &x) : NULL;
            const char *rc = rx ? place_comma(rx, eos) : NULL;
            const char *ry = rc ? place_int(rc, eos, &y) : NULL;
            if (!ry) {
                /* four commas but not an entry: a polyline line, or a line
                 * this reader cannot make sense of (both counted) */
                if (strchr(line, ';'))
                    st->poly++;
                else
                    st->shortline++;
            } else if (!strchr(mov, '.')) {
                st->entry_noext++;         /* the five authored fields parse,
                                            * but the script field names no
                                            * file: KADMAR.07 ANI1.DES carries
                                            * `BERLO,dummy,sdfsdf,61,54`.
                                            * Nothing resolves from it and
                                            * nothing is guessed. */
                if (st->entry_noext <= 4)
                    printf("ll_level: placement %s: an entry names the script"
                           " \"%s\", which is not a file name: nothing can be"
                           " resolved for it (a data hole in the placement"
                           " itself)\n", fname, mov);
            } else {
                entries++;                  /* a real entry line, whatever
                                            * its coordinates say */
                st->entries++;
                for (const char *q = line; q < ry; q++)
                    if (*q == ' ' || *q == '\t') {
                        st->field_space++;   /* fields written with spaces
                                              * around the commas: the name is
                                              * the same, so it is parsed */
                        break;
                    }
                if (strstr(cls, ".ani"))
                    st->class_hint++;  /* class hint, no .ANI record: never
                                        * used to pick art */
                const char *pm = ry;
                while (pm < eos && *pm == ' ')
                    pm++;
                if (pm < eos) {             /* parameters after the authored
                                             * coordinates: real data in 4 of
                                             * the 2188 entries (KADMAR.07
                                             * `kpal.mov,640,640  -37,-7 -1
                                             * 100 0`, ZARIA.01 `kamien1.mov,
                                             * 1360,250,1308`). Kept verbatim,
                                             * never guessed at. */
                    st->param_extra++;
                    if (st->param_extra <= 4)
                        printf("ll_level: placement %s: an entry's parameters"
                               " after the coordinates (%s) are authored data"
                               " this model does not act on; the object is"
                               " still placed at (%d,%d)\n", fname, pm, x, y);
                }
                if (x < 0 || x >= pc->world_w) {
                    st->coord_bad++;   /* outside the level's own back_rect:
                                        * counted, and not placed */
                    goto next;
                }
                obj_script *sc = obj_script_get(pc, leaf, mov);
                if (!sc)
                    goto next;
                sc->ninst++;
                lvl_obj *o = obj_slot(pc);
                if (!o)
                    goto next;
                o->sc = sc;
                o->sti = sc->mov ? sc->mov->start_st : 0;   /* declared start */
                o->bx = o->x = x;      /* authored position, y down: no
                                        * invented bottom-anchor. Only the
                                        * script's own " M dx,dy" frames move
                                        * it (285 of them game-wide). */
                o->by = o->y = y;
                st->placed++;
            }
        }
next:
        p = eol;
        while (p < end && (*p == '\n' || *p == '\r'))
            p++;
    }
    if (declared >= 0 && declared != entries)
        printf("ll_level: placement %s: its own count line says %d but the"
               " file holds %d entry line(s) - count lines are stale in 38"
               " of the 68 shipped files, so every line is read\n",
               fname, declared, entries);
    st->files++;
}

static void des_placement(place_ctx *pc, llrs *hd_ety,
                          const llrs_slot *hd_blk2, const des_kv *kvs, int nk)
{
    obj_stats *st = &pc->st;
    if (!hd_ety || !hd_blk2) {
        printf("ll_level: no HD ETAPY block for %s: the level's own placement"
               " files are unreachable (data hole)\n", pc->level);
        return;
    }
    llrs_slot *hrec2 = NULL;
    unsigned nh2 = 0;
    if (llrs_block(hd_ety, hd_blk2, &hrec2, &nh2))
        return;

    for (unsigned kk = 0;
         kk < sizeof des_place_keys / sizeof des_place_keys[0]; kk++) {
        const char *key = des_place_keys[kk];
        const char *raw = des_get(kvs, nk, key);
        if (!raw[0])
            continue;      /* key absent: the author says nothing about it
                            * (no shipped level omits these three keys) */
        char list[512];
        snprintf(list, sizeof list, "%s", raw);
        char *save = NULL;
        for (char *tok = strtok_r(list, ",", &save); tok;
             tok = strtok_r(NULL, ",", &save)) {
            while (*tok == ' ')
                tok++;
            if (!tok[0])
                continue;
            if (!strchr(tok, '.') && ci_eq(tok, "none")) {
                printf("ll_level: des %s: the %s list carries the bare 'none'"
                       " keyword (the game's own 'nothing here': skipped,"
                       " not a data hole)\n", pc->level, key);
                continue;
            }
            const llrs_slot *dr = NULL;
            for (unsigned k = 0; k < nh2; k++)
                if (hrec2[k].id == 1 && hrec2[k].name[0] &&
                    ci_eq(hrec2[k].name, tok))
                    dr = &hrec2[k];
            if (!dr) {
                st->file_missing++;
                printf("ll_level: des %s: %s (named by %s=) is not in the HD"
                       " level block (data hole)\n", pc->level, tok, key);
                continue;
            }
            unsigned char *txt = NULL;
            if (llrs_read(hd_ety, dr, &txt) < 0)
                continue;
            printf("ll_level: placement %s (%u B, from %s=)\n", dr->name,
                   dr->size, key);
            des_place_file(pc, dr->name, (const char *)txt, (size_t)dr->size);
            free(txt);
        }
    }
    llrs_block_free(hrec2);

    printf("ll_level: authored scene %s: %d placement file(s), %d object"
           " entry line(s) -> %d placed, %d at an x outside the view the"
           " level's own data defines (its back_rect, or its single screen"
           " where the data gives none), %d distinct script(s) / %d art"
           " table(s) (%d carry the patch art this viewer draws, %d of those"
           " also carry the shape margins, %d carry shape geometry only -"
           " invisible solids the original game never drew either); %d"
           " script(s) and %d art name(s) are not on these containers (data"
           " holes listed above)\n",
           pc->level, st->files, st->entries, st->placed, st->coord_bad,
           st->nscript, st->nart, st->nart - st->art_shapeonly,
           st->art_shapecarry, st->art_shapeonly, st->nscript_missing,
           st->nart_missing);
    if (st->prop || st->group || st->comment || st->class_hint ||
        st->poly || st->shortline || st->param_extra || st->field_space ||
        st->entry_noext || st->longline)
        printf("ll_level: placement data the scene model does not act on"
               " (counted, never invented): %d per-entry property line(s)"
               " (damage/energy/bounds/scope), %d group header line(s),"
               " %d comment line(s) (';' or '//'), %d object-class token(s)"
               " (no .ANI record exists in any container), %d polyline"
               " line(s) (moving ground: needs the AC_* signal bus), %d"
               " entry line(s) with parameters beyond the coordinates"
               " (listed above), %d entry line(s) whose fields are written"
               " with spaces around the commas (parsed as the same names), %d"
               " entry line(s) whose script field is not a file name (listed"
               " above), %d line(s) this reader cannot classify\n",
               st->prop, st->group, st->comment, st->class_hint, st->poly,
               st->param_extra, st->field_space, st->entry_noext,
               st->shortline + st->longline);
    if (st->nscript)
        printf("ll_level: those scripts author %d state transition(s), %d"
               " control flag(s) with no model bit, %d frame signal"
               " directive(s) (GEN / ATC_ON / ATC_OFF / WAIT / lock /"
               " smpauto / GOTO), %d sound-or-scale field(s); %d state(s)"
               " name no seq= art, %d state(s) / %d frame(s) / %d"
               " transition(s) over cap - all parsed and counted, acted on"
               " only once the AC_*/K_* signal bus lands (M4-D)\n",
               st->ntrans, st->ctrl_other, st->directive, st->field_other,
               st->state_noart, st->drop_st, st->drop_fr, st->drop_tr);
    if (st->fr_total)
        printf("ll_level: frame numbers of the scripts that have art (the"
               " game's grammar: 1-based, negative = drawn mirrored, 0 = no"
               " element): %d frame(s), %d mirrored, %d blank (drawn as"
               " nothing: the game's placeholder is not modeled), %d name an"
               " element their art table does not have (drawn as nothing,"
               " never clamped to another)\n",
               st->fr_total, st->fr_mirror, st->fr_blank, st->fr_oob);
}
/* paint one 8bpp canvas through the palette LUT (shared present path).
 * The LUT stays the scene's own business (level PCX palettes AND the
 * documented grayscale fallback for blocks without a palette donor): the
 * CPU expands the canvas into a persistent ARGB scratch and hands the
 * result to the platform (plat_present_argb) - pixels are exactly the
 * ones the scene computed; the platform never sees the palette. */
static uint32_t *s_paint_buf;
static size_t s_paint_n;

static void paint(const unsigned char *canvas, const uint32_t lut[256],
                  int w, int h)
{
    size_t need = (size_t)w * (size_t)h;
    if (s_paint_n < need) {
        uint32_t *nb = realloc(s_paint_buf, need * sizeof *nb);
        if (!nb)
            return;                 /* skip the frame; data work continues */
        s_paint_buf = nb;
        s_paint_n = need;
    }
    for (int y = 0; y < h; y++) {
        uint32_t *row = s_paint_buf + (size_t)y * w;
        const unsigned char *sp = canvas + (size_t)y * w;
        for (int x = 0; x < w; x++)
            row[x] = lut[sp[x]];
    }
    plat_present_argb(s_paint_buf, w, h);
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

static void help_pages(const llrs *hd, const llrs_slot *hdrecs, unsigned nhdrec,
                       const llrs *cd, const llrs_slot *cdrecs, unsigned ncdrec,
                       const unsigned char *bg, unsigned char *canvas,
                       const uint32_t lut[256], int w, int h)
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
    uint64_t t_end = plat_now_ns() +
                     (uint64_t)((auto_close ? lim : 0.0) * 1e9);
    int page = 0, quit = 0;
    uint64_t t_tick = plat_now_ns();   /* FIX-07: fixed 20 ms grid here too */
    while (!quit && (!auto_close || plat_now_ns() < t_end)) {
        ll_event e;
        while (plat_poll(&e)) {               /* F11/Alt+Enter are consumed
                                               * by the backend here too */
            if (e.type == LL_EV_QUIT)
                quit = 2;
            else if (e.type == LL_EV_BUTTON_DOWN)
                quit = 1;                     /* click closes, like a key */
            else if (e.type == LL_EV_KEYDOWN) {
                int k = e.key;                /* ASCII verbatim or LLKEY_* */
                if (k == LLKEY_ESC && !e.repeat)
                    /* FIX-03 (X-04): holding ESC must close the help
                     * page ONLY - the repeats must not leak through and
                     * also ESC the level itself. Page keys (RIGHT/
                     * RETURN/SPACE/PGDN) keep auto-repeat on purpose. */
                    quit = 1;
                else if (k == LLKEY_RIGHT || k == LLKEY_RETURN ||
                         k == ' ' || k == LLKEY_PGDN)
                    page = (page + 1) % np;
                else if (k == LLKEY_LEFT || k == LLKEY_PGUP)
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
        paint(canvas, lut, w, h);
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
        t_tick += 20000000ull;               /* FIX-07: fixed 50 Hz grid */
        {
            uint64_t n = plat_now_ns();
            if (n > t_tick + 100000000ull)   /* >100 ms behind: resync, */
                t_tick = n;                  /* never fast-forward */
        }
        plat_sleep_until(t_tick);
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
    int pcm_rec[MAX_PCM_LAYERS];    /* present tlo .PCM records, tlo order */
    int npcm_rec = 0;
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
                    if (ci_eq(dot, ".PCM") && npcm_rec < MAX_PCM_LAYERS) {
                        int dup = 0;
                        for (int q = 0; q < npcm_rec; q++)
                            dup |= pcm_rec[q] == (int)k;
                        if (!dup)
                            pcm_rec[npcm_rec++] = (int)k;
                    }
                    break;
                }
            }
        }
    }
    if (!bg && npcm_rec == 0)   /* PR-53: only when no PCM layer can be the
                                 * scene (KADMAR2's M15.PCX was a stand-in
                                 * for the layer that could not be shown) */
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
    /* tlo= names the whole background stack back to front (.pcx + .pcm).
     * The .PCX is the backdrop; every present .PCM is a TRANSPARENT overlay
     * decoded here and painted over it each frame (layers_at, ll_pcm.h). */
    ll_pcm lay[MAX_PCM_LAYERS];
    int nlay = 0;
    memset(lay, 0, sizeof lay);
    for (int q = 0; q < npcm_rec; q++) {
        const llrs_slot *rs = &recs[pcm_rec[q]];
        unsigned char *raw = NULL;
        if (llrs_read(&etapy, rs, &raw) < 0)
            continue;
        ll_pcm p2;
        int rc2 = ll_pcm_decode(raw, (size_t)rs->size, &p2);
        free(raw);
        if (rc2 != 0) {
            printf("ll_level: PCM %s: decode failed (malformed layer, size"
                   " %d) - not drawn\n", rs->name, rs->size);
            continue;
        }
        size_t painted = 0;
        for (size_t i = 0; i < (size_t)p2.w * p2.h; i++)
            painted += p2.op[i];
        printf("ll_level: PCM %s: transparent overlay layer %u px x %u rows"
               " (%u strips of 80, tiles of %u), %.1f%% painted, fnv=%016llx"
               " -> layer %d over %s\n", rs->name, p2.w, p2.h, p2.nstrips,
               p2.tile, 100.0 * (double)painted / ((double)p2.w * p2.h),
               (unsigned long long)ll_pcm_fnv(&p2), nlay + 1,
               bg ? bg->name : "an empty backdrop");
        lay[nlay++] = p2;
    }

    int cw = (int)img.w, ch = (int)img.h;
    unsigned char *view = NULL;
    unsigned char scenepal[768];
    memset(scenepal, 0, sizeof scenepal);
    if (img.idx) {
        memcpy(scenepal, img.pal, 768);
    } else if (nlay > 0) {
        /* PCM-only level (no backdrop PCX in the tlo stack, KADMAR2): the
         * window is a viewer choice (the engine blits 400x200 at a time),
         * the palette is the block's BACK.PCX - it equals the backdrop
         * palette on 8 of the 11 levels that have both, and it is the one
         * that makes KADMAR2's layer natural masonry instead of noise. */
        cw = world_w > 0 && world_w < 640 ? world_w : 640;
        ch = world_h > 0 && world_h < 400 ? world_h : 400;
        view = calloc((size_t)cw, (size_t)ch);
        int gray = 1;
        for (unsigned k = 0; k < nrec && gray; k++)
            if (recs[k].id == 1 && ci_eq(recs[k].name, "BACK.PCX")) {
                unsigned char *rawb = NULL;
                ll_pcx bk;
                memset(&bk, 0, sizeof bk);
                if (llrs_read(&etapy, &recs[k], &rawb) >= 0) {
                    if (ll_pcx_decode(rawb, (size_t)recs[k].size, &bk) == 0) {
                        memcpy(scenepal, bk.pal, 768);
                        gray = 0;
                    }
                    ll_pcx_free(&bk);
                    free(rawb);
                }
            }
        if (gray)
            for (int i = 0; i < 256; i++)    /* documented fallback: gray */
                scenepal[3 * i] = scenepal[3 * i + 1] =
                    scenepal[3 * i + 2] = (unsigned char)i;
        snprintf(bgname, sizeof bgname, "%s", recs[pcm_rec[0]].name);
    }
    if (!img.idx && !view) {
        printf("ll_level: no background PCX for '%s' "
               "(wiring wants '%s' - missing from these containers)\n",
               level, des_get(kvs, nk, "tlo"));
        for (int q = 0; q < nlay; q++)
            ll_pcm_release(&lay[q]);
        llrs_block_free(recs);
        free(txt);
        llrs_close(&leveldes);
        llrs_close(&etapy);
        return 1;
    }

    /* 4) the authored scene: what the level's OWN placement data places
     *    (PR-45). The three placement keys of the main .des name the
     *    sub-.des files; every entry in them names an ANIMACJE leaf; that
     *    leaf's .MOV on the HD install is the motion script and each of its
     *    states names its own .SEQ art through seq= (on the CD).
     *    A missing HD install means no scripts (the placement files
     *    themselves live in the HD container); a missing CD ANIMACJE means
     *    the art of the scripts that are present is missing. Both are
     *    reported by exact name as data holes, and nothing is invented:
     *    PR-36's fallback scan of every leaf .SEQ and its one-script-per-
     *    leaf collapsing are gone, and so is the 24-object cap (measured:
     *    RAN.01 places 827 objects from 82 scripts, SADES.06 uses 264
     *    scripts and 277 art tables - the arrays below grow instead). */
    play_ctx pl;
    memset(&pl, 0, sizeof pl);             /* LEO_PLAY wiring (4c below)   */
    scene_owner scene;                     /* scripts + placements, owned;
                                           * freed at done: (E-05)         */
    memset(&scene, 0, sizeof scene);

    llrs an, hd_an, hd_ety;
    memset(&an, 0, sizeof an);
    memset(&hd_an, 0, sizeof hd_an);
    memset(&hd_ety, 0, sizeof hd_ety);
    int an_open = 0, have_hd = 0, have_hd_ety = 0;
    snprintf(p, sizeof p, "%s/ANIMACJE", dir);
    const char *hd = getenv("LEO_HD_DIR");
    if (hd) {
        char hp[4096];
        snprintf(hp, sizeof hp, "%s/ANIMACJE", hd);
        have_hd = llrs_open(&hd_an, hp) == 0;
    }
    const llrs_slot *ablk = NULL, *hblk = NULL, *hblk2 = NULL;
    if (llrs_open(&an, p) == 0) {
        an_open = 1;
        ablk = find_or_stem(&an, level);
    }
    if (have_hd)
        hblk = find_or_stem(&hd_an, level);
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

    place_ctx pc;
    memset(&pc, 0, sizeof pc);
    pc.own = &scene;
    pc.level = level;
    pc.world_w = world_w > 0 ? world_w : (int)img.w;
    pc.ac.cd = an_open ? &an : NULL;
    pc.ac.cd_blk = ablk;
    pc.ac.hd = have_hd ? &hd_an : NULL;
    pc.ac.hd_blk = hblk;
    if (!an_open)
        printf("ll_level: no CD ANIMACJE for %s in %s: the art the object"
               " scripts name is unreachable (data hole)\n", level, dir);
    if (!have_hd)
        printf("ll_level: no HD ANIMACJE for %s (LEO_HD_DIR is not set or"
               " that install has no ANIMACJE): the object scripts of this"
               " level are unreachable (data hole)\n", level);
    des_placement(&pc, have_hd_ety ? &hd_ety : NULL, hblk2, kvs, nk);
    lvl_obj *objs = scene.objs;        /* the print/draw loops below */
    int nobj = scene.nobj;
    if (an_open) {
        llrs_close(&an);
        an_open = 0;                   /* done: must not close twice */
    }
    if (have_hd) {
        llrs_close(&hd_an);
        have_hd = 0;
    }
    /* hd_ety stays open: the help overlay (5b) reads the same HD block */
    /* hd_ety stays open: the help overlay (5b) reads the same HD block */

    /* 4c) playable level prototype (M4-B, PR-43): LEO_PLAY upgrades the
     * preview into the real controllable lion - art from the `lew=` leaf's
     * .SEQ scripts, motion from that leaf's HD .MOV (F=/T=/G= physics,
     * K_* transitions), ground from the `platformy=` target (a .PLT or a
     * sub-.des holding the same polylines), spawn from
     * `pozycja=`, bounds from `lew_rect=` (all of it RAN.01/TUNGA.11
     * data; simulation in ll_play.c). A missing piece is a data hole:
     * logged, and the level stays a preview. */
    if (getenv("LEO_PLAY")) {
        char ap[4096];
        char leafname[24] = "", movstem[16] = "";
        const char *lew = des_get(kvs, nk, "lew");
        if (lew[0]) {
            /* lew=<class>,<art leaf>,<mov>,<player des> (RAN.01:
             * "TLewRan,Lew.ran,lew.mov,lewran.des") */
            char tw[32] = "";
            const char *c1 = strchr(lew, ',');
            const char *c2 = c1 ? strchr(c1 + 1, ',') : NULL;
            if (c1) {
                size_t n = c2 ? (size_t)(c2 - c1 - 1) : strlen(c1 + 1);
                if (n >= sizeof tw)
                    n = sizeof tw - 1;
                memcpy(tw, c1 + 1, n);
                tw[n] = '\0';
                stem_of(tw, leafname, sizeof leafname);
                const char *dot = strrchr(tw, '.');
                if (dot && dot[1])
                    snprintf(leafname + strlen(leafname),
                             sizeof leafname - strlen(leafname), ".%s",
                             dot + 1);
            }
            if (c2) {
                const char *c3 = strchr(c2 + 1, ',');
                char mt[32];
                size_t n = c3 ? (size_t)(c3 - c2 - 1) : strlen(c2 + 1);
                if (n >= sizeof mt)
                    n = sizeof mt - 1;
                memcpy(mt, c2 + 1, n);
                mt[n] = '\0';
                stem_of(mt, movstem, sizeof movstem);
            }
        }
        if (!movstem[0])
            stem_of(leafname[0] ? leafname : "LEW", movstem,
                    sizeof movstem);

        /* CD: every .SEQ of the player art leaf (all states share the
         * leaf; each per-state seq= names one of them) */
        snprintf(ap, sizeof ap, "%s/ANIMACJE", dir);
        llrs can;
        memset(&can, 0, sizeof can);
        if (llrs_open(&can, ap) == 0) {
            const llrs_slot *cb = find_or_stem(&can, level);
            llrs_slot *crecs = NULL;
            unsigned ncrec = 0;
            if (cb && llrs_block(&can, cb, &crecs, &ncrec) == 0) {
                const llrs_slot *lf = NULL;
                for (unsigned k = 0; k < ncrec; k++)
                    if (crecs[k].id == 2 && crecs[k].name[0] &&
                        leafname[0] && ci_eq(crecs[k].name, leafname)) {
                        lf = &crecs[k];
                        break;
                    }
                if (!lf)
                    for (unsigned k = 0; k < ncrec; k++) {
                        char st[16];
                        if (crecs[k].id != 2 || !crecs[k].name[0])
                            continue;
                        stem_of(crecs[k].name, st, sizeof st);
                        if (strncasecmp(st, "LEW", 3) == 0) {
                            lf = &crecs[k];
                            printf("ll_level: play: no leaf '%s' in %s:"
                                   " using '%s' (data-hole note)\n",
                                   leafname, level, crecs[k].name);
                            break;
                        }
                    }
                if (lf) {
                    snprintf(pl.leaf, sizeof pl.leaf, "%s", lf->name);
                    llrs_slot *lrec = NULL;
                    unsigned nlrec = 0;
                    if (llrs_block(&can, lf, &lrec, &nlrec) == 0) {
                        for (unsigned j = 0; j < nlrec && pl.nart < 32;
                             j++) {
                            const char *dot = strrchr(lrec[j].name, '.');
                            if (lrec[j].id != 1 || !lrec[j].name[0] ||
                                !dot || !ci_eq(dot, ".SEQ"))
                                continue;
                            int ai = pl.nart;
                            if (llrs_read(&can, &lrec[j],
                                          &pl.art[ai].blob) < 0 ||
                                ll_seq_parse(pl.art[ai].blob,
                                             (uint32_t)lrec[j].size,
                                             &pl.art[ai].seq) != 0) {
                                free(pl.art[ai].blob);
                                pl.art[ai].blob = NULL;
                                continue;
                            }
                            stem_of(lrec[j].name, pl.art[ai].stem,
                                    sizeof pl.art[ai].stem);
                            printf("ll_level: play: art %s/%s: %u"
                                   " element(s)\n", lf->name,
                                   lrec[j].name,
                                   (unsigned)pl.art[ai].seq.count);
                            pl.nart++;
                        }
                        llrs_block_free(lrec);
                    }
                }
            }
            llrs_block_free(crecs);
            llrs_close(&can);
        }

        /* HD: the player .MOV (same leaf name inside HD ANIMACJE) */
        if (hd && *hd) {
            llrs han;
            memset(&han, 0, sizeof han);
            snprintf(ap, sizeof ap, "%s/ANIMACJE", hd);
            if (llrs_open(&han, ap) == 0) {
                const llrs_slot *hb = find_or_stem(&han, level);
                llrs_slot *hrecs2 = NULL;
                unsigned nhrec2 = 0;
                if (hb && llrs_block(&han, hb, &hrecs2, &nhrec2) == 0) {
                    const llrs_slot *hl = NULL;
                    for (unsigned k = 0; k < nhrec2; k++)
                        if (hrecs2[k].id == 2 && hrecs2[k].name[0] &&
                            pl.leaf[0] &&
                            ci_eq(hrecs2[k].name, pl.leaf))
                            hl = &hrecs2[k];
                    if (hl) {
                        llrs_slot *mrecs = NULL;
                        unsigned nmrec = 0;
                        if (llrs_block(&han, hl, &mrecs, &nmrec) == 0) {
                            const llrs_slot *mv = rec_find(mrecs, nmrec,
                                                           movstem,
                                                           ".MOV");
                            if (!mv) {
                                /* Safety net: the .des names the script
                                 * and all 13 of them name one that
                                 * exists (measured 2026-11-16), so this
                                 * never fires today - but we NEVER guess
                                 * a .MOV (ZARIA's leaves carry an extra
                                 * LEWPL.MOV that its .des does not
                                 * name), so if a future level's name
                                 * does not resolve, print what IS there
                                 * and make the gap diagnosable. */
                                for (unsigned k = 0; k < nmrec; k++) {
                                    const char *dot
                                        = strrchr(mrecs[k].name, '.');
                                    if (mrecs[k].id == 1 &&
                                        mrecs[k].name[0] && dot &&
                                        ci_eq(dot, ".MOV"))
                                        printf("ll_level: play: leaf %s"
                                               " has no %s.MOV, it"
                                               " carries %s (unused: the"
                                               " .des names the"
                                               " script)\n", pl.leaf,
                                               movstem, mrecs[k].name);
                                }
                            }
                            unsigned char *mtxt = NULL;
                            if (mv && llrs_read(&han, mv, &mtxt) > 0) {
                                pl.mov = calloc(1, sizeof *pl.mov);
                                if (!pl.mov ||
                                    ll_mov_parse((char *)mtxt,
                                                 pl.mov) <= 0) {
                                    free(pl.mov);
                                    pl.mov = NULL;
                                } else
                                    printf("ll_level: play: %s: %d"
                                           " state(s), fallback art"
                                           " '%s'\n", mv->name,
                                           pl.mov->nst, pl.mov->seqkey);
                            }
                            free(mtxt);
                            llrs_block_free(mrecs);
                        }
                    }
                }
                llrs_block_free(hrecs2);
                llrs_close(&han);
            }
        }

        /* HD ETAPY: the ground polylines named by platformy=. TWO forms
         * exist in the real data (measured 2026-11-16 over all 13 level
         * blocks):
         *   platformy=<stem>.plt  -> the level block's <stem>.PLT record
         *                           (11 levels: ran.plt, tunga.plt, ...)
         *   platformy=<name>.des  -> a SUB-.DES record of the same level
         *                           block whose TEXT carries the very
         *                           same polyline grammar: TUNGA2.12 and
         *                           SADES.06 say platf.des and their
         *                           PLATF.DES record starts
         *                           "66\r\n1 1495,147;1529,147;" /
         *                           "8\r\n1 863,166;959,166;" - count
         *                           header and all. JUNCTION says
         *                           none.des and its NONE.DES record is
         *                           literally "0": that level has NO
         *                           ground, which is data, not a gap.
         * Same parser either way (ll_plt_parse). */
        const char *pltspec = des_get(kvs, nk, "platformy");
        char pltstem[16];
        stem_of(pltspec, pltstem, sizeof pltstem);
        const char *pltdot = pltspec ? strrchr(pltspec, '.') : NULL;
        int plt_is_des = pltdot && ci_eq(pltdot, ".des");
        int ground_authored_empty = 0;
        if (have_hd_ety && hblk2) {
            llrs_slot *erecs = NULL;
            unsigned nerec = 0;
            if (llrs_block(&hd_ety, hblk2, &erecs, &nerec) == 0) {
                const llrs_slot *pr =
                    pltstem[0]
                        ? rec_find(erecs, nerec, pltstem,
                                   plt_is_des ? ".DES" : ".PLT")
                        : NULL;
                if (!pr && !plt_is_des)
                    for (unsigned k = 0; k < nerec; k++) {
                        const char *dot = strrchr(erecs[k].name, '.');
                        if (erecs[k].id == 1 && erecs[k].name[0] && dot &&
                            ci_eq(dot, ".PLT")) {
                            pr = &erecs[k];
                            printf("ll_level: play: no '%s.PLT' in %s:"
                                   " using '%s' (data-hole note)\n",
                                   pltstem, level, pr->name);
                            break;
                        }
                    }
                if (pr) {
                    unsigned char *ptxt = NULL;
                    if (llrs_read(&hd_ety, pr, &ptxt) > 0 &&
                        ll_plt_parse((char *)ptxt, &pl.plt) == 0) {
                        ground_authored_empty = pl.plt.n == 0;
                        printf("ll_level: play: %s: %d polyline(s)",
                               pr->name, pl.plt.n);
                        if (plt_is_des)
                            printf(" (ground inside the sub-.des that"
                                   " platformy= names)");
                        if (ground_authored_empty)
                            printf(" - the data says this level has NO"
                                   " ground");
                        printf("\n");
                    }
                    free(ptxt);
                }
            }
            llrs_block_free(erecs);
        }

        /* spawn: pozycja=x,y feet, snapped down onto the first surface */
        const char *pos = des_get(kvs, nk, "pozycja");
        int px = 0, py = 0;
        if (pos && sscanf(pos, "%d,%d", &px, &py) == 2) {
            pl.sx = px;
            pl.sy = py;
        } else {
            printf("ll_level: play: no pozycja= in %s: spawning at world"
                   " left edge\n", level);
            pl.sx = 40;
            pl.sy = 0;
        }

        /* bounds: lew_rect=x0,y0;x1,y1 (feet clamp + world-bottom death);
         * fall back to back_rect, else no clamp */
        const char *lr = des_get(kvs, nk, "lew_rect");
        int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
        if (lr && sscanf(lr, "%d,%d;%d,%d", &x0, &y0, &x1, &y1) == 4 &&
            x1 > x0) {
            pl.xmin = x0;
            pl.xmax = x1;
            pl.ydead = y1;
        } else if (world_w > 0) {
            printf("ll_level: play: no lew_rect= in %s: using back_rect"
                   " bounds\n", level);
            pl.xmin = world_x0;
            pl.xmax = world_x0 + world_w;
            pl.ydead = world_h > 0 ? world_h : 1e30;
        } else {
            pl.xmin = -1e30;
            pl.xmax = 1e30;
            pl.ydead = 1e30;
        }

        if (pl.mov && pl.mov->nst > 0 && pl.nart > 0 && pl.plt.n > 0) {
            pl.have = 1;
            play_spawn(&pl);
            printf("ll_level: play: %s %d state(s) on %d polyline(s):"
                   " spawn feet (%d,%d) grounded=%d bounds %d..%d"                   /* lew_rect */
                   " death below %d\n", pl.leaf, pl.mov->nst, pl.plt.n,
                   (int)pl.p.x, (int)pl.p.y, pl.p.grounded,
                   (int)pl.xmin, (int)pl.xmax, (int)pl.ydead);
            play_claim_leaf(objs, &nobj, &pl);
        } else {
            printf("ll_level: play: level %s %s: staying preview\n", level,
                   !pl.mov ? "lacks player .MOV"
                           : !pl.nart ? "lacks player .SEQ art"
                           : ground_authored_empty
                               ? "has NO ground in its own data"
                                 " (platformy= names a file declaring 0"
                                 " polylines)"
                               : "lacks ground (platformy=)");
        }
    }

    printf("ll_level: show %s: bg %s %dx%d, %d animated object(s)\n", level,
           bgname, cw, ch, nobj);
    /* Proof of the join, bounded: 24 placements in file order, then the
     * count of the rest (RAN.01 places 827). LEO_OBJ_ALL=1 prints them all
     * for probes. The line names leaf / script / the art the script's own
     * seq= selected / the state count / the authored position. */
    int obj_all = getenv("LEO_OBJ_ALL") != NULL;
    int obj_shown = 0;
    for (int k = 0; k < nobj; k++) {
        if (!obj_all && obj_shown >= 24)
            break;
        const obj_script *sc = objs[k].sc;
        int ai = objs[k].sti >= 0 && objs[k].sti < LMOV_MAXST
                     ? sc->ast[objs[k].sti] : -1;
        const obj_art *a = ai >= 0 && ai < sc->nart ? &sc->art[ai] : NULL;
        const char *why =
            !a ? " (this state names no seq= art)"
               : !a->found ? " (no such record on these containers: data hole)"
               : a->eidx0 < 0 ? " (shape margins only, no patch art: an"
                              " invisible solid, undrawn by the original game"
                              " too)"
               : "";
        printf("ll_level: obj %s/%s%s: art %s%s, %d state(s), bbox %dx%d at"
               " (%d,%d)\n", sc->leaf, sc->movname,
               sc->mov && sc->mov->nst ? "[mov]" : "[no script: data hole]",
               a && a->name[0] ? a->name : "(none)", why,
               sc->mov ? sc->mov->nst : 0, a ? a->sw : 0, a ? a->sh : 0,
               objs[k].x, objs[k].y);
        obj_shown++;
    }
    if (obj_shown < nobj)
        printf("ll_level: ... %d more placement(s) of those scripts"
               " (LEO_OBJ_ALL=1 prints them)\n", nobj - obj_shown);
    if (getenv("LEO_LEVEL_NO_OBJ")) {
        /* CI-only hook (PR-36): paint the bare background (no object
         * compositing, the logs and animation state above stay as-is). Kept
         * for the help-overlay oracle, which asserts "help frame = bare bg +
         * exactly the page window". */
        printf("ll_level: LEO_LEVEL_NO_OBJ -> %d objects not composited\n",
               nobj);
        nobj = 0;
    }

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
    /* Vertical camera (PR-51, VIEWER POLICY - the game's own camera is not
     * recovered): the backdrop image is exactly the view size, so it cannot
     * scroll; objects and the lion scroll over it by camy. In play mode it
     * follows the lion's feet (65% down the view), LEO_LEVEL_CAMY sets it
     * for CI, Up/Down pan it in the viewer. Without this the lion spawned
     * below the view on 7 of the 12 playable levels (feet y 320..1504 vs
     * views 200..600 px tall) and was never on screen. */
    int cam_ymax = world_h > ch ? world_h - ch : 0;
    int camy = getenv("LEO_LEVEL_CAMY") ? atoi(getenv("LEO_LEVEL_CAMY")) : 0;
    if (camy < 0)
        camy = 0;
    if (camy > cam_ymax)
        camy = cam_ymax;
    int autoscroll = 0;
    if (world_w > 0)
        printf("ll_level: world %dx%d (back_rect %d,%d), view %dx%d, cam"
               " %d/%d [Left/Right pan, Home/End, p autoscroll]\n",
               world_w, world_h, world_x0, world_y0, cw, ch, cam, cam_max);
    else
        printf("ll_level: no back_rect (data hole): static one-screen view"
               " %dx%d\n", cw, ch);

    /* 5) composite canvas + present loop.  bg source: PCX plane, or the
     * PCM 400x200 rest view when the level is PCM-driven. */
    size_t csz = (size_t)cw * (size_t)ch;
    unsigned char *canvas = malloc(csz);
    if (!canvas) {
        rc = 1;                 /* E-05: OOM must still release everything
                                 * else allocated (done: below) */
        goto done;
    }
    const unsigned char *base = img.idx ? img.idx : view;
    memcpy(canvas, base, csz);
    layers_at(lay, nlay, cam, camy, cw, ch, world_w, world_h, canvas);
    for (int k = 0; k < nobj; k++)
        obj_draw(&objs[k], canvas, cw, ch, cam, camy);

    const char *ppm = getenv("LEO_LEVEL_PPM");   /* CI: headless dump */
    if (ppm) {
        int ticks = getenv("LEO_LEVEL_PPM_TICK")
                        ? atoi(getenv("LEO_LEVEL_PPM_TICK"))
                        : 0;
        if (pl.have) {
            /* play leg (PR-43): fixed 20 ms grid, LEO_PLAY_SCRIPT keys
             * (default "1:FWD": walk-right demo); camera stays 0 so the
             * frames are byte-deterministic for the PPM comparison legs.
             * Scene objects advance on the same grid. */
            const char *scr = getenv("LEO_PLAY_SCRIPT");
            if (!scr)
                scr = "1:FWD";
            for (int tk = 1; tk <= ticks; tk++) {
                ll_play_keys keys = script_keys(scr, tk);
                play_advance(&pl, &keys, 20, 1, tk);
                for (int k = 0; k < nobj; k++)
                    obj_advance(&objs[k], 20);
            }
            camy = follow_camy(&pl, ch, cam_ymax);
        } else {
            int shown = 0;
            for (int k = 0; k < nobj; k++) { /* advance fixed-step then redraw */
                for (int m = 0; m < ticks; m++)
                    obj_advance(&objs[k], 40);
                if (shown < 12) {             /* proof of the cycle, bounded */
                    printf("ll_level: tick %d: %s/%s state %d frame %d at"
                           " (%d,%d)\n", ticks, objs[k].sc->leaf,
                           objs[k].sc->movname, objs[k].sti, objs[k].fi,
                           objs[k].x, objs[k].y);
                    shown++;
                }
            }
            if (shown < nobj)
                printf("ll_level: tick %d: %d more placement(s)"
                       " advanced\n", ticks, nobj - shown);
        }
        memcpy(canvas, base, csz);         /* same order as the live loop */
        layers_at(lay, nlay, cam, camy, cw, ch, world_w, world_h, canvas);
        for (int k = 0; k < nobj; k++)
            obj_draw(&objs[k], canvas, cw, ch, cam, camy);
        play_draw(&pl, canvas, cw, ch, cam, camy);
        scene_ppm(ppm, cw, ch, canvas, scenepal);
        goto done;                          /* canvas freed at done: (E-05) */
    }

    {
        /* T-01/T-12: the platform owns the window (logical viewport = the
         * scene view size cw x ch; resizable, HiDPI-ready, F11/Alt+Enter
         * desktop-fullscreen handled by the backend). */
        if (plat_init("Lew Leon - level scene", cw, ch) < 0) {
            fprintf(stderr, "ll_level: no window: %s\n", plat_last_error());
            goto done;
        }
        plat_raise(); /* FIX-02: now a documented no-op (window raised
                       * once at creation); call kept for API parity */
        uint32_t lut[256];
        for (int i = 0; i < 256; i++)
            lut[i] = 0xFF000000u | ((uint32_t)scenepal[3 * i] << 16) |
                     ((uint32_t)scenepal[3 * i + 1] << 8) | scenepal[3 * i + 2];

        /* 5b) help overlay first, like the original POMOC flow: the HD
         *     level block's POMOC1.MNU names page art (.SEQ) that ships in
         *     the CD level block; ESC closes, Left/Right page through. */
        if (have_hd_ety && hblk2) {
            llrs_slot *hrecs2 = NULL;
            unsigned nhrec2 = 0;
            if (llrs_block(&hd_ety, hblk2, &hrecs2, &nhrec2) == 0 && nhrec2)
                help_pages(&hd_ety, hrecs2, nhrec2,
                           &etapy, recs, nrec, base, canvas, lut,
                           cw, ch);
            llrs_block_free(hrecs2);
        }

        /* 5c) level music: the .des `muzyka=<stem>.mod` names a SOUND
         *     container '<STEM>.MOD' payload (standard MOD layout, see
         *     ll_mod.h).  Same singleton as the MAPKA screen; the scene
         *     loop below pumps it (platform queue path, see the note at
         *     the top of this file - callback delivery is unreliable on
         *     SDL2+PipeWire, so the backend opens devices queue-only). */
        if (audio && !getenv("LEO_NO_MUSIC")) {
            const char *muz = des_get(kvs, nk, "muzyka");
            if (muz && *muz) {
                char stem[16], mname[24];
                stem_of(muz, stem, sizeof stem);
                snprintf(mname, sizeof mname, "%s.MOD", stem);
                ll_music_play(dir, mname);
            }
        }

        /* secs <= 0 = no time limit: the scene runs until ESC/q (desktop
         * behaviour; CI pins the window with LEO_LEVEL_SECS>0). All
         * timing on the single platform clock (ns). */
        uint64_t t_end = secs > 0 ? plat_now_ns() + (uint64_t)(secs * 1e9)
                                  : 0;
        uint64_t t_prev_ms = plat_now_ns() / 1000000ull; /* FIX-07 (X-08): ms clock: */
        uint64_t t_tick = plat_now_ns();   /* dt from absolute time, no loss */
        int scroll_acc = 0;                /* per-frame; sub-px scroll acc */
        while (!esc && (!t_end || plat_now_ns() < t_end)) {
            /* Focus: raised once at window creation (FIX-02); no
             * per-tick poll, no raise on pointer-enter. P-10: F11/
             * Alt+Enter are consumed by the backend and never
             * surface here. */
            ll_event e;
            while (plat_poll(&e)) {
                if (e.type == LL_EV_QUIT ||
                    (e.type == LL_EV_KEYDOWN && !e.repeat &&
                     (e.key == LLKEY_ESC || e.key == 'q')))
                    /* FIX-03 (X-04): exit keys act on first press */
                    esc = 1;
                else if (pl.have &&
                         (e.type == LL_EV_KEYDOWN ||
                          e.type == LL_EV_KEYUP) &&
                         (e.key == LLKEY_LEFT || e.key == LLKEY_RIGHT ||
                          e.key == LLKEY_UP   || e.key == LLKEY_DOWN)) {
                    /* M4-B: in play mode the arrows drive the LION
                     * (held-key tracking, KEYUP releases); the camera
                     * follows the player instead (below the loop). */
                    int on = e.type == LL_EV_KEYDOWN;
                    if (e.key == LLKEY_LEFT)
                        pl.keys.k_bkwd = on;
                    else if (e.key == LLKEY_RIGHT)
                        pl.keys.k_fwd = on;
                    else if (e.key == LLKEY_UP)
                        pl.keys.k_up = on;
                    else
                        pl.keys.k_down = on;
                }
                else if (e.type == LL_EV_KEYDOWN &&
                         (cam_max > 0 || cam_ymax > 0)) {
                    int k = e.key;      /* ASCII verbatim or LLKEY_* */
                    if (k == LLKEY_UP)
                        camy = camy > 20 ? camy - 20 : 0;
                    else if (k == LLKEY_DOWN)
                        camy = camy + 20 <= cam_ymax ? camy + 20 : cam_ymax;
                    else if (k == LLKEY_LEFT)
                        cam = cam > 20 ? cam - 20 : 0;
                    else if (k == LLKEY_RIGHT)
                        cam = cam + 20 <= cam_max ? cam + 20 : cam_max;
                    else if (k == LLKEY_HOME)
                        cam = 0;
                    else if (k == LLKEY_END)
                        cam = cam_max;
                    else if (k == 'p' && !e.repeat) {
                        /* FIX-03: camera motion (LEFT/RIGHT) keeps
                         * auto-repeat - holding an arrow scrolls. p is a
                         * TOGGLE: first press only, holding it must not
                         * flicker autoscroll on/off. */
                        autoscroll = !autoscroll;
                        printf("ll_level: autoscroll %s\n",
                               autoscroll ? "on (50 px/s)" : "off");
                    }
                }
            }
            uint64_t now_ms = plat_now_ns() / 1000000ull;
            unsigned dt = (unsigned)(now_ms - t_prev_ms);
            t_prev_ms = now_ms;
            if (dt > 200)
                dt = 200;
            if (autoscroll) {
                scroll_acc += 50 * (int)dt;  /* FIX-07: 50 px/s exact over */
                cam += scroll_acc / 1000;    /* time (old 50*dt/1000 lost  */
                scroll_acc %= 1000;          /* <1 px to truncation per tick) */
                if (cam > cam_max)
                    cam = 0;
            }
            for (int k = 0; k < nobj; k++)
                obj_advance(&objs[k], dt);
            if (pl.have) {
                unsigned used = 0;
                while (used < dt) {          /* same fixed 20 ms grid the
                                                CI legs run on */
                    unsigned st2 = dt - used > 20u ? 20u : dt - used;
                    play_advance(&pl, &pl.keys, st2, 0, 0);
                    used += st2;
                }
                cam = (int)pl.p.x - cw / 2;  /* camera follows the lion */
                if (cam < 0)
                    cam = 0;
                if (cam > cam_max)
                    cam = cam_max;
                camy = follow_camy(&pl, ch, cam_ymax);
            }
            memcpy(canvas, base, csz);
            layers_at(lay, nlay, cam, camy, cw, ch, world_w, world_h, canvas);
            for (int k = 0; k < nobj; k++)
                obj_draw(&objs[k], canvas, cw, ch, cam, camy);
            play_draw(&pl, canvas, cw, ch, cam, camy);
            paint(canvas, lut, cw, ch);
            ll_music_pump();
            t_tick += 20000000ull;               /* FIX-07: fixed 50 Hz grid */
            {
                uint64_t n = plat_now_ns();
                if (n > t_tick + 100000000ull)   /* >100 ms behind: resync, */
                    t_tick = n;                  /* never fast-forward */
            }
            plat_sleep_until(t_tick);
        }
        ll_music_stop();
        /* no window teardown: P-03 keeps it up for the next screen
         * (menu, next level, ...); the platform reuses it on the next
         * plat_init. */
    }
    if (esc)
        printf("ll_level: ESC -> advance\n");

done:                                 /* single cleanup path (E-05): every
                                       * way out of the scene runs this */
    free(canvas);
    scene_owner_free(&scene);            /* scripts (their art + parsed
                                          * .MOV) and placements: the only
                                          * owners of scene memory (PR-45) */
    if (an_open)
        llrs_close(&an);
    if (have_hd)
        llrs_close(&hd_an);
    for (int k = 0; k < pl.nart; k++) {
        ll_seq_free(&pl.art[k].seq);
        free(pl.art[k].blob);
    }
    ll_plt_free(&pl.plt);
    free(pl.mov);
    ll_pcx_free(&img);
    free(view);
    for (int q = 0; q < nlay; q++)
        ll_pcm_release(&lay[q]);
    llrs_block_free(recs);
    free(txt);
    if (have_hd_ety)
        llrs_close(&hd_ety);
    llrs_close(&leveldes);
    llrs_close(&etapy);
    return rc;
}
