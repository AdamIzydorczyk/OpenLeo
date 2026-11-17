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
 * Scene objects read that too: control=BY_X states WALK at F/4 px/s and
 * their frame lists' second field counts movement units - that slot is
 * authored BOTH ways across the shipped scripts, and only BY_X states
 * read it as units (PR-79; basis re-labelled 2026-11-16, the model
 * stands: see obj_walk below). Everything else keeps the frame-list
 * time semantics through obj_advance/obj_elem. */

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
    unsigned char src;               /* which .des key placed it: 0 zwierzaki,
                                      * 1 animacje, 2 ani_plat (PR-60)    */
    unsigned char layer;             /* entry index inside that key's comma
                                      * list = the tlo/parallax layer     */
    /* PR-68: the key=value lines that follow an entry in the placement
     * file (zwierzaki .des: damage= contact damage, bounds= the patrol x
     * range, scope= detection range, energy= hit points). -1 = absent. */
    int damage, b0, b1, scope, energy;
    /* PR-89: the object's own combat state, mirroring the engine's level
     * object fields (NEXT_STEPS "T2 RE lead" for the whole pipeline):
     * hp = the energy counter at [obj+0xad], initialised from the
     * placement's `energy=`; hp < 0 = not hit-able (the engine writes
     * [obj+0xad] = -1 for its never-dying types at 0x340f5/0x54478 and
     * tests death as `energy < 0` at 0x544a5). hit_cd = the hit
     * countdown at [obj+0x102] (FUN_000502f0 asks "invulnerable?" by
     * testing it non-zero; 0x27264 arms the generic default 0xe = 14).
     * atc = the object's own attack-connect window (PR-90: the ENEMY
     * hits the LION through it, the mirror of the lion's own). */
    int hp, hit_cd;
    unsigned char atc;
    unsigned char atc_hit;          /* PR-90: this object's window has    *
                                     * already landed its one hit (mirror  *
                                     * of ll_player.atc_hit, reset exactly *
                                     * like atc on every state change).    */
    unsigned char dead;             /* the engine-forced death (A_DEAD) */
    unsigned long long sig;         /* PR-88: pending signal pulses at the
                                     * LL_SIG_* bit indexes of the engine's
                                     * signal table (was: K_GEN1..4 in bits
                                     * 0..3). The engine's own set at
                                     * [obj+0x60] is a latching TEventSet;
                                     * the raise-a-pulse-consume-it-same-
                                     * tick model (raised as the frame ends,
                                     * tested by the transition checks of
                                     * the same advance) is from PR-69 and
                                     * stays - the engine's clearing rule
                                     * is unresearched and no data depends
                                     * on it yet. */
    /* PR-79: BY_X walking (see obj_walk): fractional-pixel carry, units
     * accumulated toward the next frame, and the patrol direction
     * (0 = never flipped: walks right - PR-81 shows that is what the
     * authored frame sign draws). PR-81: face is the instance FACING set
     * by walking in control=CH_DIR states (the census pairs bounds= with
     * CH_DIR, and matching the art in both polarities the winner flips
     * with the travel direction). frame numbers already carry the
     * authored base facing in their sign (LICHA's art faces left, its
     * walk list is all-negative and the game draws it facing RIGHT);
     * face=-1 adds a second mirror on top. Initialized +1 at placement =
     * no extra mirror, so non-walkers are unaffected. */
    double xa, facc;
    signed char dir, face;
    /* PR-93: 1 = a static image placement (the line named its .SEQ
     * directly; no .MOV, no states, no interaction - and drawn BEFORE the
     * animated objects, which is what the video shows: actors in front of
     * the stone shelves). Every runtime path already treats a placement
     * whose script has no .mov as inert (obj_hit/arm/advance/settle/
     * shape/touch all gate on sc->mov), so the flag drives only the
     * counts, the draw order and the report labels. */
    unsigned char static_img;
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

/* The draw mirror of a placement: the authored frame sign (obj_elem) plus
 * the facing a CH_DIR walker turned into (PR-81). face=+1 or 0 (never
 * turned, or a non-walker: the placement builder sets +1) draws exactly
 * what the frame line asks for; face=-1 adds one mirror, which is what
 * the video shows - matching the worm art in BOTH polarities, the winner
 * flips with the travel direction (head-right at t=204.4/211.2/212.4,
 * head-left at t=206.9/208.0/213.2) and both polarities appear at
 * x~330 one second apart (t=209.2 vs 210.0): one instance, turned. */
static int obj_mirror_of(const lvl_obj *o, int authored)
{
    return authored ^ (o->face < 0 ? 1 : 0);
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
                            o->y - camy, obj_mirror_of(o, mirror), NULL);
    /* PR-81: the facing a CH_DIR walker turned into is drawn on top of the
     * authored frame sign; every other object keeps face=+1 and the plain
     * authored mirror. Evidence in obj_walk's comment. */
}

/* PR-69: the object scripts are state machines, not a ring of states: a
 * state's frame list loops until one of its transitions fires. Conditions
 * the scene model can decide: AC_BREAK_NEXT (the frame list just ended),
 * K_GEN1..5 and any other signal NAME of the LL_SIG_* table (PR-88: tested
 * against this object's own pulse set - raised by frame `GEN <signal>`
 * tails or by gameplay code like the PR-70 balloon pop); AC_TIME
 * (a timed wait, `time=min,max`, unit unresolved) is APPROXIMATED as "at the
 * end of the frame list" so a waiting state never freezes - OR a raised
 * AC_TIME bit; what still stays false (keys, AC_ONAIR, AC_ENEMY_ATC ...)
 * needs the lion/world signals feeding this set (open). First-match order
 * as the player's transitions; PR-88 aligned the grammar with the engine
 * too: the clause parser at 0x47d60 folds a ',' run into ONE node bit
 * mask (AND), '|' starts another node (OR) - the same OR-of-AND-groups
 * the player evaluator uses since PR-87. */
static int obj_cond_true(const lvl_obj *o, int brk, const ll_mov_tr *tr)
{
    for (int i = 0; i < tr->nc; ) {                 /* PR-87/88: same */
        int g = tr->c[i].grp, all = 1;              /* OR-of-AND-groups */
        for (; i < tr->nc && tr->c[i].grp == g; i++) { /* as the player: the */
            int v;                                  /* engine folds a  */
            switch (tr->c[i].cond) {                /* ',' run into ONE node */
            case LL_COND_AC_BREAK_NEXT:             /* bit mask (clause */
            case LL_COND_AC_TIME:                   /* parser 0x47d60) */
                v = brk || ((o->sig >> LL_SIG_AC_TIME) & 1); break;
            case LL_COND_K_GEN1:
                v = (o->sig >> LL_SIG_K_GEN1) & 1; break;
            case LL_COND_K_GEN2:
                v = (o->sig >> LL_SIG_K_GEN2) & 1; break;
            case LL_COND_K_GEN3:
                v = (o->sig >> LL_SIG_K_GEN3) & 1; break;
            case LL_COND_K_GEN4:
                v = (o->sig >> LL_SIG_K_GEN4) & 1; break;
            case LL_COND_K_GEN5:
                v = (o->sig >> LL_SIG_K_GEN5) & 1; break;
            case LL_COND_OTHER:
                v = tr->c[i].sig >= 0
                    ? (int)((o->sig >> (unsigned)tr->c[i].sig) & 1)
                    : 0;                            /* PR-88: a bare signal */
                break;                              /* NAME tests the  */
            default:                                /* object's own set - */
                v = 0; break;                       /* still false until */
            }                                       /* something raises */
            if (tr->c[i].neg)                       /* it (keys/AC_* of  */
                v = !v;                             /* the lion need the */
            if (!v)                                 /* lion's own set:   */
                all = 0;                            /* OPEN) */
        }
        if (all)
            return 1;
    }
    return 0;
}

/* A state change. PR-101 read the engine's own: it is a TABLE, not a
 * search. 0x52000 (the state record's sub-table slot +0x04, called from
 * 0x53520 inside the key compositor 0x536f0) walks 21-byte entries at
 * [rec+0x1a] - a 13-byte key, a record index at +0xd, a row index at
 * +0x11 - calling 0x47e40 with the object's composed key for each, and
 * 0x535a2 applies the first match as "go to record A, row B" (its
 * same-index path is just `cmp esi,[rec+4]; mov [rec+4],edx`). So the
 * engine's transition is a keyed lookup that also picks the starting
 * row, and the key it looks up is the OR of the attribute-node chains
 * of PR-101, not an authored name. What the port matches is the
 * CONDITION half (a key/flag test gating a state change) and the
 * per-state reset of the motion accumulators. PR-102 corrects what
 * this comment first said about the row-index half: the engine's row
 * number IS authored and the engine APPLIES it. The transition line in
 * a .MOV state section is `TARGET(N)=<key expression>` (the base
 * record parse method 0x51910 matches `" ( %[^) ] %*[)] = "` at
 * 0x9225c - the same function that parses `control=`; the derived
 * record class's parser 0x52590 handles `F=`/`T=`/`G=` and delegates
 * everything else to it), N is stored at entry+0x11 and applied as the
 * new row, and the shipped data asks for non-zero N thousands of times
 * (37 states are entered at several different rows; the numbers reach
 * 97, the documented maximum frames in one state). This port parses
 * that number into ll_mov_tr.entry (PR-86) and then ignores it:
 * obj_transition sets o->fi = 0. So the difference is one unused
 * field, not a missing feature. Nothing is pinned from that: it is an
 * observation of the code and the data, and what it changes on screen
 * is unmeasured (NEXT_STEPS PR-102, open item 4). */
static int obj_transition(lvl_obj *o, ll_mov_t *mov, int brk)
{
    const ll_mov_st *s = &mov->st[o->sti];

    for (int t = 0; t < s->ntr; t++) {
        if (!obj_cond_true(o, brk, &s->tr[t]))
            continue;
        if (s->tr[t].tidx < 0 || s->tr[t].tidx == o->sti)
            continue;                         /* unresolved / keep this state */
        int byx = (s->ctrl & LL_CTRL_BY_X) ||
                  (mov->st[s->tr[t].tidx].ctrl & LL_CTRL_BY_X);
        o->sti = s->tr[t].tidx;
        o->fi = 0;
        if (!byx) {
            o->x = o->bx;
            o->y = o->by;       /* motion deltas are per state; a BY_X
                                 * position is instance state: walking
                                 * creatures carry it across states */
        }
        /* face is instance state too (PR-81): a creature that turned
         * around stays turned around when it changes state */
        o->facc = 0.0;
        o->xa = 0.0;
        o->sig = 0;
        o->atc = 0;                  /* PR-89: an attack window belongs to
                                      * the state that opened it           */
        o->atc_hit = 0;              /* PR-90: and so does its one-hit flag */
        return 1;
    }
    return 0;
}

/* PR-89: the lion's attack lands on a scene object. The engine's damage
 * pipeline (fully mapped in NEXT_STEPS "T2 RE lead"): a touch runs the
 * push-out first (FUN_000552e0 -> 0x53ae0/0x55130) and only then the hit
 * effects (0x55230); the victim's energy counter at [obj+0xad] is
 * decremented by FUN_00054550 `sub [eax+0xad], ebx`, the deliverer's
 * damage is its own [obj+0xb1] (the placement `damage=`, already parsed),
 * and the death gate is `energy < 0` (0x544a5 `cmp [eax+0xad],0; jge`).
 * The ATTACK dispatcher (0x34020) answers on the ARGUMENT object's .des
 * template TYPE (it compares [arg.vt+0xc], the class id, against the
 * factory-instance id array ds:0xcecf0..0xced04 - one startup routine calls a
 * factory, 0x44350, and stores each returned id there; the id->TYPE map is
 * still an open question). Two entries (0xced00, 0xcecf0) make the receiver
 * take `energy*30/100+10` damage (0x34037/0x3406b: shl 4, sub, `lea [eax+
 * eax*1]` = *30, idiv 100, +10 - NOT *45; the earlier *45 note was a lea
 * scale misread, see PR-96), one (0xcecf8) sets the receiver's `[obj+0xad] =
 * -1`, and 0xced04 is the lion-stomp path PR-89 ported (0x340b5 is 0x34020's
 * fall-through second half, NOT a separate method). The fall-through default
 * is 30 - `mov ebx,0x1e` at 0x34103, `call 0x50ed0` at 0x34108. No shipped
 * .des authors any lion damage number, so 30 is the only value the shipped
 * data points to; the proportional types stay unreachable here.
 *
 * Rejection: FUN_00050ed0 is gated by FUN_000502f0 "invulnerable?", which
 * tests the countdown at [obj+0x102]; 0x27264 is the generic setter with
 * the DEFAULT 14 (the lion's own is 30 at 0x50304, already ported as
 * pl->lock). Objects get the same countdown here: 14.
 *
 * The hit reaction is an INFERENCE, flagged as such: every animal script
 * and every lion state carries "A_DOSTAL(1)=AC_ENEMY_ATC" ("got hit" on
 * "enemy attacked"), and no shipped .MOV line ever GENs AC_ENEMY_ATC (0
 * of 279 uses) - the combat code must be its raiser, and the object that
 * was hit is the one that knows. So the hit raises the bit ON THE HIT
 * OBJECT's own set and lets its own transition table pick A_DOSTAL: no
 * new mechanism, exactly the PR-88 one. PR-91 mirrors the raise onto the
 * LION for the enemy's ACTIVE attack (the ATC-window hit): every shipped
 * lion .MOV authors the same lines - measured on ZARIA.01 LEW.ZAR/LEWPL,
 * A_WALK/A_DRAP/A_FIGHT... carry "A_DOSTAL(1)=AC_ENEMY_ATC", and ZARIA's
 * lion A_DOSTAL is a full authored reaction (BY_TIME 9x200 ms, seq=hit,
 * frame 1 `I -320,-250` pullback, exit A_DRAP(1)=AC_BREAK_NEXT at the
 * list end). Contact hits do NOT raise it (model choice + open question:
 * the token names the enemy's ATtack, and a passive touch has no window;
 * raising on every touch would make every bump a reaction - the engine's
 * raiser is still unpinned, so the narrower reading stays).
 *
 * Death enters the script's own state BY NAME because the engine forces
 * it: 340 of the 413 scripts that have an A_DEAD state have NO
 * transition line into it (measured over the extracted .MOV texts), and
 * the engine hardcodes the string "A_DEAD" four times (0x9068c/0x907ab/
 * 0x90e79/0x91008, inside per-level lion classes next to their
 * `energy = %d` parsers). A script with no A_DEAD state keeps its state
 * here - what the engine then does is open, so nothing is invented.
 * The dead object stops hurting the lion (model choice: its A_DEAD art
 * stays in place; whether the engine's corpse still deals contact
 * damage is not pinned). */
#define LL_LION_HIT_DAMAGE 30   /* 0x34103: the dispatcher's default hit */
#define LL_OBJ_HIT_CD      14   /* [obj+0x102] default countdown, 0x27264 */

static int obj_hit(lvl_obj *o, int damage)   /* 0 = no hit, 1 = hurt,
                                              * 2 = hurt AND killed      */
{
    ll_mov_t *mov = (o->sc && o->sc->mov) ? o->sc->mov : NULL;

    if (!mov || o->hp < 0 || o->hit_cd > 0)
        return 0;              /* no script / energy = -1 is not hit-able
                                * (the engine's own never-dying marker,
                                * 0x340f5) / inside the hit countdown    */
    o->hp -= damage;
    o->hit_cd = LL_OBJ_HIT_CD;
    if (o->hp < 0) {                        /* the engine's death gate:
                                             * energy < 0 (0x544a5)      */
        int d = ll_mov_find(mov, "A_DEAD");

        o->dead = 1;
        if (d >= 0 && d != o->sti) {
            int byx = (mov->st[o->sti].ctrl & LL_CTRL_BY_X) ||
                      (mov->st[d].ctrl & LL_CTRL_BY_X);
            o->sti = d;
            o->fi = 0;
            if (!byx) {
                o->x = o->bx;
                o->y = o->by;
            }
            o->facc = 0.0;
            o->xa = 0.0;
            o->sig = 0;
            o->atc = 0;             /* PR-90: same window rule as the     */
            o->atc_hit = 0;         /* transition table's own             */
        }
        return 2;
    }
    o->sig |= 1ull << LL_SIG_AC_ENEMY_ATC;
    obj_transition(o, mov, 0);
    o->sig = 0;
    return 1;
}

/* PR-90 (T3): what ARMS an enemy's attack state. The .MOV data never
 * transitions into A_ATAK - ZARIA's LICHA3 has no line into it - because
 * the engine's enemy AI is COMPILED PER CLASS, and the class comes from
 * the placement's OWN class token (measured in the image, NEXT_STEPS
 * PR-90 record): 0x616d0 string-compares the token against "SEP",
 * "PARROT", "SNAKE", "LICHA" (0x9320b..0x93234) and builds that class's
 * AI object (LICHA: 0x120 bytes, constructor 0x61890); the AI attach
 * (0x618b0) creates four behavior records named "MAIN", "WALK", "ATACK",
 * "TURN" (0x93238..0x93248, list [x+0x106]) with method tables 0xa30a8
 * / 0xa308c / 0xa3070 / 0xa3054, and those classes' state-NAME tables
 * hardcode "A_RUN" and "A_ATAK" (0x93262/0x9326d) - the exact state
 * names of this script. So the attack state is ENTERED BY CODE. The gate
 * for the class LICHA is pinned: its MAIN behavior's check (table
 * 0xa30a8 slot +0x18 -> 0x61b50, jump table 0x61b40 case 0 -> 0x61b6e)
 * tests |obj.x - lion.x| < 0x28 (40) AND |obj.y - lion.y| < 0x32 (50)
 * game px, both against CURRENT positions (position objects: the
 * object's own [x+0x26], the lion's through getter 0x3ff60 = [x+0x394];
 * x at +4, y at +0 of those). The same class's ATACK::check (0x61cd2)
 * then tests the lion's side against a facing getter (vtable +0x70);
 * that second stage, the other AI classes' boxes (they are per class)
 * and the engine's consumer of the placement's scope= pair
 * ([obj+0xfa]/+0xfe: a pair argument to a vtable+0x4 method at
 * 0x3bc1d/0x3bcb9, compared against a vtable+0x70 result at 0x3d70c)
 * are open question 10. scope= is NOT the box: measured with this
 * port's own shape margins the centers hold >= 21 px apart in ZARIA.01,
 * so an authored scope=20 box could never open at all. MODEL CHOICE
 * shipped (flagged everywhere it is used): an object whose CURRENT state
 * carries the authored DANGER control flag arms the script's OWN A_ATAK
 * state by name (the PR-89 A_DEAD pattern) when the lion enters the
 * pinned LICHA MAIN box; only that pinned stage, no invented extras.
 * The engine's per-class AI stays open; this gate is the port's
 * stand-in, off unless LEO_PLAY_DANGER=1, exactly like the PR-59 solid
 * gate. What the armed attack drives (charge motion, TURN behaviors) is
 * T3 stage 2. */
static void obj_arm_atak(lvl_obj *o, int lx, int ly, int verbose, int tick)
{
    ll_mov_t *mov = (o->sc && o->sc->mov) ? o->sc->mov : NULL;
    if (!mov || o->dead || o->sti < 0 || o->sti >= mov->nst)
        return;
    ll_mov_st *s = &mov->st[o->sti];
    if (!(s->ctrl & LL_CTRL_DANGER))     /* the authored DANGER flag is  */
        return;                          /* the port's own precondition  */
    int dx = o->x - lx, dy = o->y - ly; /* the pinned LICHA MAIN box    */
    if (dx <= -40 || dx >= 40 || dy <= -50 || dy >= 50)
        return;                          /* |dx| < 40, |dy| < 50 (0x61b6e) */
    int a = ll_mov_find(mov, "A_ATAK");
    if (a < 0 || a == o->sti)            /* nothing to arm               */
        return;
    int byx = (s->ctrl & LL_CTRL_BY_X) ||
              (mov->st[a].ctrl & LL_CTRL_BY_X);
    if (verbose)
        printf("ll_level: play tick %d: %s/%s DANGER: lion within the class"
               " MAIN box (dx %d dy %d): %s arms A_ATAK (model)\n",
               tick, o->sc->leaf, o->sc->movname, dx, dy, s->name);
    o->sti = a;                          /* the PR-89 force-by-name      */
    o->fi = 0;                           /* pattern (A_DEAD): same       */
    if (!byx) {                          /* per-state rules the transi-  */
        o->x = o->bx;                    /* tion table itself applies    */
        o->y = o->by;
    }
    o->facc = 0.0;
    o->xa = 0.0;
    o->sig = 0;
    o->atc = 0;
    o->atc_hit = 0;
}

/* PR-79: BY_X states WALK. The frame-list slot semantics are data-
 * internal: the same .MOV authors the second frame-list field as ms for
 * BY_TIME states (400/200) and as 4 for its BY_X state - ZARIA.01's
 * LICHA3 A_RUN - so a BY_X frame list counts MOVEMENT UNITS per
 * animation frame, not ms (no video needed; the PR-79 "measured
 * 3.0-3.75 px/frame" per-frame claim was withdrawn in the 2026-11-16
 * re-check: the blob centroid tracked pose deformation, the worm's walk
 * cycle itself rears vertical S-coil poses, and the camera-band shift
 * method tracked parallax layers, not the ground). The scale claim
 * stands as CORROBORATED: on the video (geometry 2.0x of a 320x240
 * screen, pinned) the ZARIA.01 worm's walk bursts average ~20-30 px/s
 * with its authored F=100 -> |fx|/4 = 25 px/s, and its 9-frame x 4-unit
 * stride cycle = 1.44 s, matching the observed stride rhythm; T's
 * semantics stay unresolved and nothing is built on it. NOTE (PR-99,
 * and it limits the /4 above): every object measured on video has
 * T=40 (worm F=100,T=40; rhino F=180,T=40), so /4 is only measured at
 * one T, and at a fixed T it is indistinguishable from the engine's
 * own law, whose steady state is proportional to F/T (the velocity
 * step at 0x54250: `v -= T*v/100` then `v += F*s/20`; s is the
 * terrain scalar at obj+0xba). PR-98 called that law refuted by these
 * samples; PR-99 corrects that - the samples never vary T (the one
 * T=20 walk is the lion, whose path through that code is unverified).
 * So this walk law is unvalidated for objects at T != 40, and the
 * measurement that would settle it is a BY_X object there (the data
 * has F=100 against 11 different T pairs; only ZARIA is on video).
 * bounds= is the authored patrol key; the flip is modelled at its
 * edges. CH_DIR (PR-81):
 * the census pairs bounds= with CH_DIR (8/8 sampled placements; 0/14
 * without; 93 of the 105 BY_X states carry it). The frame-number sign is
 * the authored BASE facing - ZARIA.01's worm art faces left (the eye
 * sits 11-20% from the left edge in all 9 poses) and its walk list is
 * authored all-NEGATIVE, so the game draws it head-RIGHT - and matching
 * the art in BOTH polarities on video, the polarity that wins flips
 * with the travel direction: head-right while it travels right
 * (t=204.4, 211.2, 212.4), head-left while it travels left (t=206.9,
 * 208.0, 213.2), and both polarities at x~330 one second apart (209.2
 * vs 210.0) = one instance turning around. So a CH_DIR state that turns
 * at the bounds turns its facing too, mirrored on top of the frame
 * sign. It also corroborates the initial direction +1 (right): that is
 * what the authored sign draws. The video shows pause-in-place-then-
 * walk bursts: state-gating (A_ATAK/DANGER vs A_RUN), not modelled.
 * With F=0 nothing accumulates, so the state keeps the old time-driven
 * frame advance without moving: kept as the safe reading (a frozen list
 * could deadlock; no shipped game shows a stationary BY_X walk).
 * PR-100 read the engine's own gate for this and it is the SAME shape
 * as below: the state record's step (0x521f0) calls the mode
 * dispatcher (0x543a0, slot +0xa0 of the owner object's method table),
 * whose BY_X branch adds the truncated |v_x|/100 to [obj+0xa1] once per
 * record step, and the step then compares that accumulator against the
 * current row's authored threshold field, admits at most one row per
 * step, and hands the remainder to the row handler - slot +0x18 of a
 * second method table inside the record object ([rec+0x44], 0x520c0),
 * which writes that remainder straight back into [obj+0xa1] at 0x52118:
 * the carry is kept in the object, exactly as `facc` is here. PR-101
 * corrects one half of that reading and it is worth knowing here: that
 * gate is the EMPTY-KEY branch of the step (0x52241 `sete`/`je` after
 * 0x495b0's all-zero test), and a row whose node DOES carry a key
 * takes the other branch - OR the object's key, look it up with
 * 0x47e40, and if it is absent the step does nothing at all, no feed,
 * no threshold. The port has no per-row key, so every row here is the
 * engine's un-keyed kind; that is the assumption to revisit if a
 * per-row condition is ever parsed. So the
 * pair obj_walk's `facc` + obj_advance's `facc < f->ms` gate is
 * structurally what the engine does; what differs is the per-step feed
 * (the engine truncates to whole px - NEXT_STEPS PR-99 item 9 - while
 * the port carries a fraction) and the unit of the comparison value,
 * which in the engine is that feed's unit: counted px for BY_X/BY_Y,
 * not ms. */
static void obj_walk(lvl_obj *o, const ll_mov_st *s, unsigned dt_ms)
{
    double dist = (double)(s->fx >= 0 ? s->fx : -s->fx) * dt_ms / 4000.0;

    o->facc += dist;                          /* units toward the next frame */
    o->xa += dist;
    int px = (int)o->xa;
    o->xa -= (double)px;                      /* carry the fraction to dt+1  */
    if (px <= 0)
        return;
    int dir = o->dir < 0 ? -1 : 1;
    o->x += dir * px;
    if (o->b1 > o->b0) {                      /* bounds= patrol range: flip  */
        if (dir > 0 && o->x >= o->b1)
            o->dir = -1;
        else if (dir < 0 && o->x <= o->b0)
            o->dir = 1;
        dir = o->dir < 0 ? -1 : 1;
    }
    /* PR-81/82: CH_DIR states FACE the direction they travel (PR-81
     * record: the art-vs-authored-sign census and the video, where the
     * worm's winning draw polarity flips with its travel direction at the
     * bounds). PR-82 sets it from the travel direction on every step
     * instead of only at the bounds flip: identical result today (this
     * walk is the port's only object movement source, and dir only
     * changes at a flip), but it stays right the moment another source
     * exists (the DANGER charge-then-walk of T3, `I` lunges). States
     * without CH_DIR never touch facing - it is instance state and sticks
     * (same model as ll_player_step). Applied by obj_mirror_of. */
    if (s->ctrl & LL_CTRL_CH_DIR)
        o->face = dir;                        /* PR-81: CH_DIR = facing follows
                                               * the movement direction */
}

static void obj_advance(lvl_obj *o, unsigned dt_ms)
{
    ll_mov_t *mov = (o->sc && o->sc->mov) ? o->sc->mov : NULL;

    if (!mov || !mov->nst)          /* no script on these containers: stop */
    {
        o->t += dt_ms;
        return;
    }
    if (o->hit_cd > 0)              /* PR-89: the [obj+0x102] countdown runs
                                     * on the same fixed step as the lion's
                                     * pl->lock (obj_advance is called once
                                     * per play tick) */
        o->hit_cd--;
    ll_mov_st *start = &mov->st[o->sti];
    if ((start->ctrl & LL_CTRL_BY_X) && start->fx != 0)
        obj_walk(o, start, dt_ms);
    else {
        o->t += dt_ms;
        o->facc = 0.0;
        o->xa = 0.0;
    }
    unsigned guard = 0;
    for (;;) {
        /* FIX-13 (X-11): a corrupt .MOV with all-zero frame durations would
         * cycle forever; one full pass without consuming time = stop
         * (ll_mov_parse clamps non-positive ms to 100; 4096 is far beyond
         * any legitimate pass). */
        if (++guard > 4096)
            break;
        ll_mov_st *s = &mov->st[o->sti];
        if (s->nfr == 0 || o->fi >= s->nfr) {   /* end of the frame list     */
            /* PR-101 read the engine's end of the row list: 0x520c0's
             * sequential path wraps it - at the last row (`n =
             * [[rec+0x2e]+8]`, the record's row count at rec+0x36) it
             * goes back to row 0 and ORs bit 2 into a byte the caller
             * passed in, and the reversed direction does the mirror at
             * row 0 with bit 3 (0x5219a-0x521d7). That byte is the
             * caller's key buffer, so "the list ended" arrives as KEY
             * BITS, which is what the keyed rows and the transition
             * table of PR-101 can then match. Reaching this branch
             * without a matching transition is the same shape as the
             * engine wrapping and re-running the row list; the port
             * just has no key to raise (PR-95 item 1: the engine
             * reverses rows with the SEQ_REV_X/Y control bits XOR-ed
             * against two owner-side guards, not with any frame
             * reverse - see NEXT_STEPS PR-99/100/101). */
            if (obj_transition(o, mov, 1))
                continue;
            if (s->nfr == 0)
                break;
            o->fi = 0;                          /* no way out: loop the state */
            if (!(s->ctrl & LL_CTRL_BY_X)) {    /* walking keeps its position */
                o->x = o->bx;
                o->y = o->by;
            }
            o->sig = 0;
        }
        ll_mov_fr *f = &s->fr[o->fi];
        int walk = (s->ctrl & LL_CTRL_BY_X) && s->fx != 0;
        if (walk ? o->facc < (double)f->ms      /* units, not ms: the same    */
                 : o->t < f->ms)                /* slot read both ways        */
            break;
        if (walk)
            o->facc -= (double)f->ms;
        else
            o->t -= f->ms;
        o->x += f->dx;             /* the authored "N,ms, M dx,dy" motion  */
        o->y += f->dy;             /* directive of the frame being held    */
        o->sig |= f->gsig;          /* `GEN <signal>`: raised as the frame ends */
        if (f->atc & LMOV_ATC_ON)     /* PR-89: the object's own attack- */
            o->atc = 1;               /* connect window (an A_ATAK opens  */
        if (f->atc & LMOV_ATC_OFF)    /* it, an ATC_OFF frame closes it). */
            o->atc = 0;               /* The ENEMY hitting the LION with  */
                                      /* it is T3, not this PR.           */
        o->fi++;
        if (o->sig && obj_transition(o, mov, 0))
            continue;
        o->sig = 0;
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

/* PR-58: the push-out (ll_seq_shape_push, FUN_00042de0 + FUN_00042cf0)
 * pinned from a 2-element hand-built .SEQ: E0 = a 20x4 full rectangle (all
 * margins 0), E1 = a slanted shape (left margins 0,2,4,6). Every expected
 * number was derived by hand from the machine code and reproduced by the
 * independent python twin (tools/shape_geom.py push), never from this port:
 *   P1 E0@0 vs E0@10  - depth 10 on every row, but the vertical candidate
 *      (4) is smaller than the horizontal push, so (dx,dy) = (0,4);
 *   P2/P3 E0@0 vs E0@18 and the reverse - depth 2 (horizontal wins), the
 *      reverse branch negates the push AND the result;
 *   P4/P5 E0 vs the slanted E1 at x=14 - rows 0..2 overlap by 6,4,2, so
 *      best 6, first row 0, last row 2, d = 0-4 = -4, dr = 2:
 *      dx = 6*2/6 = 2, dy = 2*(-4)/6 = -1 (C truncation); P5 is the mirror;
 *   P6/P7 the mirrored facing: E0 facing -1 sits at x - 20; touching gives
 *      nothing (P6), one pixel of overlap gives (1,0) (P7). */
static int seq_push_selftest(void)
{
    static const unsigned char blob[] = {
        0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x14, 0x00, 0x04, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x14, 0x00, 0x04, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x04, 0x06, 0x00, 0x00,
        0x00, 0x00,
    };
    static const struct {
        int a, ax, fa, b, bx, fb;
        int ret, dx, dy;
    } c[] = {
        { 0,  0,  1, 0, 10,  1,  10,  0,  4 },   /* P1 */
        { 0,  0,  1, 0, 18,  1,   2,  2,  0 },   /* P2 */
        { 0, 18,  1, 0,  0,  1,  -2, -2,  0 },   /* P3 */
        { 0,  0,  1, 1, 14,  1,   6,  2, -1 },   /* P4 */
        { 1, 14,  1, 0,  0,  1,  -6, -2,  1 },   /* P5 */
        { 0,  0, -1, 0,  0,  1,   0,  0,  0 },   /* P6 */
        { 0,  0, -1, 0, -1,  1,   1,  1,  0 },   /* P7 */
    };
    ll_seq sq;
    unsigned i;

    if (ll_seq_parse(blob, (uint32_t)sizeof blob, &sq) != 0 || sq.count != 2) {
        fprintf(stderr, "push selftest: FAILED the synthetic .SEQ did not"
                        " parse as built\n");
        return 1;
    }
    for (i = 0; i < sizeof c / sizeof c[0]; i++) {
        int o1, o2;
        int r = ll_seq_shape_push(&sq, &sq.els[c[i].a], c[i].ax, 0, c[i].fa,
                                  &sq.els[c[i].b], c[i].bx, 0, c[i].fb,
                                  &o1, &o2);
        if (r != c[i].ret || o1 != c[i].dx || o2 != c[i].dy) {
            fprintf(stderr, "push selftest: FAILED P%u: ret %d dx %d dy %d"
                            " (want %d %d %d)\n", i + 1, r, o1, o2,
                    c[i].ret, c[i].dx, c[i].dy);
            ll_seq_free(&sq);
            return 1;
        }
    }
    /* PR-89 follow-up: the PAIRED payload form. The port parses one blob
     * per .SEQ record, so a lion-art vs object-art pairing must read each
     * element's margin lists from its OWN blob; the engine-shaped single
     * form reads the OTHER record's bytes at those offsets (the element
     * offsets are payload-relative). Fixture: a shape element of the SAME
     * frame (w0=2, total_w 20, rows 4) in a second blob whose lists are
     * [0,2,4,6] left and [4,6,1,3] right (the element's own lists are
     * left [0,2,4,6], right all 0; a 4-row element's v-candidate is at
     * most 4, so the horizontal results are not masked). Values measured
     * with the python oracle (tools/shape_geom.py) on the two payloads:
     *   A@8  vs B@0 read correctly -> hit, ret -8, push (-2, 2)
     *   A@12 vs B@0 read correctly -> hit, ret -4, push (-1, 1)
     *   the SAME calls single-form -> ret -12, push (-4, 2)
     *                                    (it read A's own right list)
     * and a same-payload pairing must be byte-identical through both. */
    {
        static const unsigned char blob_b[] = {
            0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x14, 0x00, 0x04, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x02, 0x04, 0x06, 0x01, 0x03, 0x05, 0x07, 0x00, 0x00,
        };
        ll_seq sb;
        int r1, r2, s1, s2, t1, t2, bad = 0;
        if (ll_seq_parse(blob_b, (uint32_t)sizeof blob_b, &sb) != 0 ||
            sb.count != 1 || sb.els[0].n != 4) {
            fprintf(stderr, "push selftest: FAILED the paired synthetic"
                            " .SEQ did not parse as built\n");
            ll_seq_free(&sq);
            return 1;
        }
        if (!ll_seq_shape_hit(&sq, &sq.els[1], 8, 0, 1,
                              &sb.els[0], 0, 0, 1))
            bad = 1;
        if (ll_seq_shape_push2(&sq, &sq.els[1], 8, 0, 1,
                               &sb, &sb.els[0], 0, 0, 1, &r1, &r2) != -8 ||
            r1 != -2 || r2 != 2)
            bad = 2;
        if (!ll_seq_shape_hit(&sq, &sq.els[1], 12, 0, 1,
                              &sb.els[0], 0, 0, 1))
            bad = 3;
        if (ll_seq_shape_push2(&sq, &sq.els[1], 12, 0, 1,
                               &sb, &sb.els[0], 0, 0, 1, &r1, &r2) != -4 ||
            r1 != -1 || r2 != 1)
            bad = 4;
        r1 = ll_seq_shape_push(&sq, &sq.els[1], 8, 0, 1,
                               &sb.els[0], 0, 0, 1, &s1, &s2);
        if (r1 != -12 || s1 != -4 || s2 != 2)
            bad = 5;    /* the wrong-bytes single form: different answer */
        r1 = ll_seq_shape_push(&sq, &sq.els[1], 8, 0, 1,
                               &sq.els[1], 0, 0, 1, &s1, &s2);
        r2 = ll_seq_shape_push2(&sq, &sq.els[1], 8, 0, 1,
                                &sq, &sq.els[1], 0, 0, 1, &t1, &t2);
        if (r1 != r2 || s1 != t1 || s2 != t2)
            bad = 6;
        if (bad) {
            fprintf(stderr, "push selftest: FAILED the paired-payload leg"
                            " (case %d)\n", bad);
            ll_seq_free(&sb);
            ll_seq_free(&sq);
            return 1;
        }
        ll_seq_free(&sb);
    }
    ll_seq_free(&sq);
    return 0;
}

/* PR-69: object scripts are state machines (hand-derived, data-free).
 * Script text: A_RUN loops frames 1,2 (100 ms each) with A_DEAD(1)=K_GEN1;
 * A_DEAD holds frame 3 and goes back with A_RUN(1)=AC_BREAK_NEXT.
 *   no signal: 250 ms later the state is still A_RUN (the old ring would
 *   have walked into A_DEAD at 200 ms);
 *   frame 2 carrying `GEN K_GEN1`: at 200 ms A_RUN -> A_DEAD (fi 0), and
 *   100 ms later the frame list of A_DEAD ends -> A_RUN again. */
static int balloon_hit(int bx, int by, int rx, int ry, int rw, int rh);
static void obj_touch_enemy(lvl_obj *o, int verbose, int tick);
/* PR-93: the two placement grammars proved on synthetic lines (the two
 * real shapes from ZARIA.01 ZWIERZ1.DES + ANI1.DES: one SEP entry with its
 * carried jajo child at 0,0, and PICT/plan4.seq + plan6.seq static images
 * whose art is NOT on these synthetic containers - only the classification,
 * counts and placement are exercised). */
static int place_grammar_selftest(void);

static int obj_contact(const lvl_obj *o);   /* defined below, with the shape
                                             * helpers it reads */

/* PR-78: a placement whose START state has CHECK_LN is a ground-bound actor
 * (the lion's own spawn works the same way: authored pozycja 50,640 falls to
 * the 680 line). Its authored y is where it is DROPPED from, so it settles on
 * the first ground line at or below it. Evidence: licha3.mov A_RUN is
 * `BY_X|CHECK_LN`, `G=0,100`, and ZARIA.01 places the worms at y=650 over a
 * 680 floor (they floated 30 units above the lion's ground line). The fall is
 * instant here (the G= fall itself is not simulated for scene objects).
 * PR-97: the line it comes to rest on is its CONTACT line - the deepest
 * shape-box bottom of its START state's animation (obj_contact ->
 * mov_state_contact, ll_seq_shape_contact), so the actor stands with that row
 * on the floor instead of with its position point on the floor. Same rule and
 * same proof as ll_player.contact. Measured on ZARIA.01: the LICHA worms' own
 * art (MOVE.SEQ, the state their start frame list shows) carries 12 there -
 * they settle at (120,669) on the 681 line instead of (120,681) - and
 * ANIBACK/WAZ.SEQ, the big background worm, carries 46..49.
 * Returns 1 when the object moved. */
static int obj_settle(lvl_obj *o, const ll_plt *plt)
{
    if (!o->sc || !o->sc->mov || o->sti >= o->sc->mov->nst)
        return 0;
    if (!(o->sc->mov->st[o->sti].ctrl & LL_CTRL_CHECK_LN))
        return 0;
    ll_xy hit;
    if (!ll_plt_first_below(plt, o->bx, o->by, &hit))
        return 0;
    int c = obj_contact(o);
    if (hit.y - c == o->by)
        return 0;                       /* already resting on this line */
    o->by = o->y = hit.y - c;
    return 1;
}

/* PR-69: object scripts are state machines (hand-derived, data-free).
 * Script text: A_RUN loops frames 1,2 (100 ms each) with A_DEAD(1)=K_GEN1;
 * A_DEAD holds frame 3 and goes back with A_RUN(1)=AC_BREAK_NEXT.
 *   no signal: 250 ms later the state is still A_RUN (the old ring would
 *   have walked into A_DEAD at 200 ms);
 *   frame 2 carrying `GEN K_GEN1`: at 200 ms A_RUN -> A_DEAD (fi 0), and
 *   100 ms later the frame list of A_DEAD ends -> A_RUN again. */
static int obj_state_selftest(void)
{
    static const char plain[] =
        "A_RUN\nA_DEAD\n\n[A_RUN]\ncontrol=BY_TIME\nA_DEAD(1)=K_GEN1\n"
        "1,100\n2,100\n\n[A_DEAD]\ncontrol=BY_TIME\n"
        "A_RUN(1)=AC_BREAK_NEXT\n3,100\n";
    static const char gen[] =
        "A_RUN\nA_DEAD\n\n[A_RUN]\ncontrol=BY_TIME\nA_DEAD(1)=K_GEN1\n"
        "1,100\n2,100,GEN K_GEN1\n\n[A_DEAD]\ncontrol=BY_TIME\n"
        "A_RUN(1)=AC_BREAK_NEXT\n3,100\n";
    int rc = 1;
    ll_mov_t *m1 = calloc(1, sizeof *m1), *m2 = calloc(1, sizeof *m2);
    obj_script s1, s2;
    lvl_obj a, b;

    if (!m1 || !m2)
        goto out;
    memset(&s1, 0, sizeof s1);
    memset(&s2, 0, sizeof s2);
    memset(&a, 0, sizeof a);
    memset(&b, 0, sizeof b);
    ll_mov_parse(plain, m1);
    ll_mov_parse(gen, m2);
    s1.mov = m1;
    s2.mov = m2;
    a.sc = &s1;
    b.sc = &s2;
    obj_advance(&a, 250);
    if (a.sti != 0) {
        fprintf(stderr, "object state selftest: FAILED plain script left A_RUN"
                        " (state %d)\n", a.sti);
        goto out;
    }
    obj_advance(&b, 200);
    if (b.sti != 1 || b.fi != 0) {
        fprintf(stderr, "object state selftest: FAILED K_GEN1 did not enter"
                        " A_DEAD (state %d frame %d)\n", b.sti, b.fi);
        goto out;
    }
    obj_advance(&b, 100);
    if (b.sti != 0) {
        fprintf(stderr, "object state selftest: FAILED AC_BREAK_NEXT did not"
                        " return to A_RUN (state %d)\n", b.sti);
        goto out;
    }
    /* PR-88: the named-signal bus on objects. A frame raising a table
     * signal by NAME (`GEN AC_PLAT_OFF` - the real platform scripts do
     * exactly this, 181 lines shipped) satisfies a condition naming that
     * signal; a condition naming a DIFFERENT raised-or-not name stays
     * false (the wiring is per name, not "any GEN"); and ',' really
     * ANDs here too (the engine folds a comma run into ONE node mask -
     * clause parser 0x47d60): both raised fires, one raised does not. */
    {
        static const char named[] =
            "A_RUN\nA_DEAD\n\n[A_RUN]\ncontrol=BY_TIME\n"
            "A_DEAD(1)=K_GEN1,AC_PLAT_OFF\n"
            "1,100\n2,100,GEN K_GEN1,GEN AC_PLAT_OFF\n\n[A_DEAD]\n"
            "control=BY_TIME\n3,100\n";
        static const char named_neg[] =
            "A_RUN\nA_DEAD\n\n[A_RUN]\ncontrol=BY_TIME\n"
            "A_DEAD(1)=K_GEN1,AC_PLAT_OUT\n"
            "1,100\n2,100,GEN K_GEN1,GEN AC_PLAT_OFF\n\n[A_DEAD]\n"
            "control=BY_TIME\n3,100\n";
        ll_mov_t *mn = calloc(1, sizeof *mn), *mp = calloc(1, sizeof *mp);
        obj_script sn, sp;
        lvl_obj c, d;
        int ok = mn && mp;

        memset(&sn, 0, sizeof sn);
        memset(&sp, 0, sizeof sp);
        memset(&c, 0, sizeof c);
        memset(&d, 0, sizeof d);
        if (ok) {
            ll_mov_parse(named, mn);
            ll_mov_parse(named_neg, mp);
            sn.mov = mn;
            sp.mov = mp;
            c.sc = &sn;
            d.sc = &sp;
            obj_advance(&c, 200);
            obj_advance(&d, 250);
            if (c.sti != 1)
                fprintf(stderr, "object state selftest: FAILED named-signal"
                                " AND did not enter A_DEAD (state %d)\n",
                        c.sti);
            else if (d.sti != 0)
                fprintf(stderr, "object state selftest: FAILED unraised name"
                                " in an AND group let it leave A_RUN\n");
            else
                ok = 0;                            /* both cases passed */
        }
        free(mn);
        free(mp);
        if (ok)
            goto out;
    }
    /* PR-89: the hit pipeline on objects. A non-lethal hit raises
     * AC_ENEMY_ATC on the HIT object and its own transition table picks
     * A_DOSTAL (exactly the LICHA3 wiring); the engine's death gate is
     * energy < 0, and since no shipped script has a transition INTO
     * A_DEAD (340 of 413), the dead object enters it BY NAME, forced;
     * the [obj+0x102] countdown (14, 0x27264) rejects a second hit
     * inside it and expires tick by tick; `energy = -1` is the
     * never-hit-able marker (0x340f5). */
    {
        static const char hitt[] =
            "A_RUN\nA_DOSTAL\nA_DEAD\n\n[A_RUN]\ncontrol=BY_TIME\n"
            "A_DOSTAL(1)=AC_ENEMY_ATC\n1,100\n2,100\n\n[A_DOSTAL]\n"
            "control=BY_TIME\nA_RUN(1)=AC_BREAK_NEXT\n3,100\n\n[A_DEAD]\n"
            "control=BY_TIME\n4,100\n";
        ll_mov_t *mh = calloc(1, sizeof *mh);
        obj_script sh;
        lvl_obj h, u;
        int ok = mh != NULL, r1 = 0, r2 = 0;

        memset(&sh, 0, sizeof sh);
        memset(&h, 0, sizeof h);
        if (ok) {
            ll_mov_parse(hitt, mh);
            sh.mov = mh;
            h.sc = &sh;
            h.bx = h.x = 200;
            h.by = h.y = 100;
            h.hp = 20;                       /* .des energy=20, like ZARIA */
            r1 = obj_hit(&h, 15);
            if (r1 != 1 || h.hp != 5 || h.hit_cd != LL_OBJ_HIT_CD ||
                h.sti != 1)                    /* A_DOSTAL: 2nd state BLOCK */
                fprintf(stderr, "object state selftest: FAILED the first"
                                " hit did not hurt and enter A_DOSTAL"
                                " (r %d hp %d cd %d state %d)\n",
                        r1, h.hp, h.hit_cd, h.sti);
            else if (obj_hit(&h, 5) != 0 || h.hp != 5)
                fprintf(stderr, "object state selftest: FAILED a hit inside"
                                " the [obj+0x102] countdown was taken\n");
            else {
                for (int q = 0; q < LL_OBJ_HIT_CD; q++)
                    obj_advance(&h, 20);     /* 14 ticks: countdown runs  */
                if (h.hit_cd != 0 || h.sti != 0)
                    fprintf(stderr, "object state selftest: FAILED the hit"
                                    " countdown did not run out (cd %d"
                                    " state %d)\n", h.hit_cd, h.sti);
                else {
                    r2 = obj_hit(&h, 10);
                    if (r2 != 2 || h.hp != -5 || !h.dead || h.sti != 2)
                        fprintf(stderr, "object state selftest: FAILED a"
                                        " lethal hit did not force A_DEAD"
                                        " (r %d hp %d state %d)\n",
                                r2, h.hp, h.sti);
                    else {
                        obj_advance(&h, 500);        /* A_DEAD has no way
                                                      * out: it loops */
                        if (h.sti != 2)
                            fprintf(stderr, "object state selftest: FAILED"
                                            " the A_DEAD state did not"
                                            " hold (state %d)\n", h.sti);
                        else {
                            u = h;
                            u.hp = -1;       /* the never-dying marker */
                            u.dead = 0;
                            u.sti = 0;
                            if (obj_hit(&u, 30) != 0 || u.hp != -1)
                                fprintf(stderr, "object state selftest:"
                                                " FAILED an energy=-1"
                                                " object was hit\n");
                            else
                                ok = 0;
                        }
                    }
                }
            }
        }
        free(mh);
        if (ok)
            goto out;
    }
    {   /* PR-90: the DANGER + class MAIN box arming stand-in. A
         * DANGER-flagged state arms the script's OWN A_ATAK by name when
         * the lion enters the pinned LICHA MAIN box (|dx| < 40,
         * |dy| < 50 - strict, so dx = -40 and dy = -50 are OUT) - and
         * only then: without the DANGER flag, out of range, with no
         * A_ATAK state in the script, or dead, nothing moves. The
         * walking (BY_X) position carries across the forced entry
         * (same rule as the transition table's), the armed state loops
         * (its own frames open/close the attack window), and the one-
         * hit flag resets only on the next state change. */
        static const char dg[] =
            "A_RUN\nA_ATAK\n\n[A_RUN]\ncontrol=BY_X|DANGER\nF=100,0\n"
            "T=40,0\n-1,4\n-2,4\n\n[A_ATAK]\ncontrol=BY_TIME\n"
            "-1,200,ATC_ON\n-2,200\n-3,200,ATC_OFF\n";
        static const char ndg[] =
            "A_RUN\nA_ATAK\n\n[A_RUN]\ncontrol=BY_X\nF=100,0\n"
            "T=40,0\n-1,4\n-2,4\n\n[A_ATAK]\ncontrol=BY_TIME\n"
            "-1,200,ATC_ON\n-2,200\n-3,200,ATC_OFF\n";
        static const char na[] =
            "A_RUN\n\n[A_RUN]\ncontrol=BY_X|DANGER\nF=100,0\n"
            "T=40,0\n-1,4\n-2,4\n";
        ll_mov_t *md = calloc(1, sizeof *md), *mn = calloc(1, sizeof *mn),
                 *ma = calloc(1, sizeof *ma);
        obj_script sd, sn, sa;
        lvl_obj d, n, a;
        int ok = md && mn && ma;

        if (ok) {
            ll_mov_parse(dg, md);
            ll_mov_parse(ndg, mn);
            ll_mov_parse(na, ma);
            if (md->nst < 2 || mn->nst < 2 || ma->nst < 1)
                ok = 0;                           /* parse guard           */
            memset(&sd, 0, sizeof sd);
            memset(&sn, 0, sizeof sn);
            memset(&sa, 0, sizeof sa);
            sd.mov = md;
            sn.mov = mn;
            sa.mov = ma;
            memset(&d, 0, sizeof d);
            d.sc = &sd; d.x = d.bx = 100; d.y = d.by = 50;
            d.hp = 20;
            n = d; n.sc = &sn;
            a = d; a.sc = &sa;                   /* script has no A_ATAK */
            obj_arm_atak(&d, 140, 50, 0, 0);     /* dx -40: out (< test) */
            obj_arm_atak(&d, 100, 100, 0, 0);    /* dy -50: out (< test) */
            obj_arm_atak(&n, 115, 50, 0, 0);     /* no DANGER flag       */
            obj_arm_atak(&a, 115, 50, 0, 0);     /* no A_ATAK to arm     */
            if (d.sti != 0 || n.sti != 0 || a.sti != 0) {
                fprintf(stderr, "object state selftest: FAILED a negative"
                                " DANGER case armed (%d %d %d)\n",
                        d.sti, n.sti, a.sti);
                ok = 0;
            } else {
                d.dead = 1;                       /* dead: never arms */
                obj_arm_atak(&d, 135, 50, 0, 0);
                d.dead = 0;
                obj_arm_atak(&d, 135, 50, 0, 0);  /* dx -35, dy 0: arms  */
                if (d.sti != 1 || d.fi != 0 || d.x != 100 || d.atc ||
                    d.atc_hit) {
                    fprintf(stderr, "object state selftest: FAILED the"
                                    " DANGER arming did not force A_ATAK"
                                    " (state %d frame %d x %d)\n",
                            d.sti, d.fi, d.x);
                    ok = 0;
                } else {
                    obj_advance(&d, 400);         /* its own frames open */
                    if (!d.atc) {                 /* the attack window    */
                        fprintf(stderr, "object state selftest: FAILED"
                                        " A_ATAK did not open the attack"
                                        " window (state %d frame %d)\n",
                                d.sti, d.fi);
                        ok = 0;
                    } else {
                        obj_advance(&d, 200);     /* its ATC_OFF closes  */
                        if (d.atc) {
                            fprintf(stderr, "object state selftest: FAILED"
                                            " A_ATAK did not close the"
                                            " window\n");
                            ok = 0;
                        }
                    }
                }
            }
        }
        free(md);
        free(mn);
        free(ma);
        if (!ok) {
            fprintf(stderr, "object state selftest: FAILED DANGER/MAIN-box"
                            " arming leg\n");
            goto out;
        }
    }
    {   /* PR-92: the touch raise AC_ENEMY, consumed by the object's OWN
         * lines. The raiser is the shipped obj_touch_enemy (raise bit 7,
         * run the transition table, leave the bus empty); the chains are
         * the two shipped .MOV texts verbatim: RBLOK (ENA.03) carries
         * A_ATAK(1)=AC_ENEMY in A_RUN (a touch ARMS the attack), GPTAK
         * (RAN.01) carries A_DEAD(1)=AC_ENEMY in A_ATAK. Negatives: the
         * [obj+0x102] countdown gate (hit_cd) and death block the raise,
         * a script without any AC_ENEMY line (LICHA) stays put, and the
         * signal bus is empty after every call (one-beat pulse). */
        static const char tch[] =
            "A_RUN\nA_ATAK\nA_DEAD\n\n[A_RUN]\ncontrol=BY_TIME\n"
            "A_ATAK(1)=AC_ENEMY\n1,100\n\n[A_ATAK]\ncontrol=BY_TIME\n"
            "A_DEAD(1)=AC_ENEMY\n2,100\n\n[A_DEAD]\ncontrol=BY_TIME\n"
            "3,100\n";
        static const char tno[] =
            "A_RUN\nA_ATAK\n\n[A_RUN]\ncontrol=BY_TIME\n"
            "A_ATAK(1)=K_GEN1\n1,100\n\n[A_ATAK]\ncontrol=BY_TIME\n"
            "2,100\n";
        ll_mov_t *mt = calloc(1, sizeof *mt), *mo = calloc(1, sizeof *mo);
        obj_script st, so;
        lvl_obj t, u;
        int ok = mt && mo;

        if (ok) {
            ll_mov_parse(tch, mt);
            ll_mov_parse(tno, mo);
            if (mt->nst < 3 || mo->nst < 2)
                ok = 0;                           /* parse guard          */
            memset(&st, 0, sizeof st);
            memset(&so, 0, sizeof so);
            st.mov = mt;
            so.mov = mo;
            memset(&t, 0, sizeof t);
            t.sc = &st; t.x = t.bx = 300; t.y = t.by = 100; t.hp = 20;
            memset(&u, 0, sizeof u);
            u.sc = &so; u.x = u.bx = 120; u.y = u.by = 50; u.hp = 20;
            u.hit_cd = LL_OBJ_HIT_CD;              /* countdown gate     */
            obj_touch_enemy(&u, 0, 0);
            if (u.sti != 0 || u.sig) {
                fprintf(stderr, "object state selftest: FAILED a raised"
                                " AC_ENEMY passed the hit_cd gate\n");
                ok = 0;
            }
            u.hit_cd = 0;
            u.dead = 1;                           /* dead: never answers */
            obj_touch_enemy(&u, 0, 0);
            u.dead = 0;
            if (u.sti != 0 || u.sig) {
                fprintf(stderr, "object state selftest: FAILED a dead"
                                " object answered the touch\n");
                ok = 0;
            }
            obj_touch_enemy(&u, 0, 0);            /* no AC_ENEMY line:   */
            if (u.sti != 0 || u.sig) {            /* bit 7 is not K_GEN1 */
                fprintf(stderr, "object state selftest: FAILED a script"
                                " without an AC_ENEMY line answered"
                                " (%d)\n", u.sti);
                ok = 0;
            } else {
                obj_touch_enemy(&t, 0, 0);        /* RBLOK: the touch    */
                if (t.sti != 1 || t.fi != 0 || t.sig) {   /* arms A_ATAK */
                    fprintf(stderr, "object state selftest: FAILED the"
                                    " touch did not arm A_ATAK (%d)\n",
                            t.sti);
                    ok = 0;
                } else {
                    obj_touch_enemy(&t, 0, 0);    /* GPTAK: A_ATAK meets */
                    if (t.sti != 2 || t.fi != 0 || t.sig) { /* A_DEAD    */
                        fprintf(stderr, "object state selftest: FAILED"
                                        " AC_ENEMY in A_ATAK did not go"
                                        " to A_DEAD (%d)\n", t.sti);
                        ok = 0;
                    }
                }
            }
        }
        free(mt);
        free(mo);
        if (!ok) {
            fprintf(stderr, "object state selftest: FAILED AC_ENEMY touch"
                            " leg\n");
            goto out;
        }
    }
    /* balloon_hit: rect 100,50 20x30; the point is (x+10, y+10), inclusive */
    if (!balloon_hit(90, 40, 100, 50, 20, 30) ||     /* corner 100,50      */
        !balloon_hit(110, 70, 100, 50, 20, 30) ||    /* far corner 120,80  */
        balloon_hit(89, 40, 100, 50, 20, 30) ||      /* 99: left of it     */
        balloon_hit(90, 71, 100, 50, 20, 30)) {      /* 81: below it       */
        fprintf(stderr, "object state selftest: FAILED balloon rect\n");
        goto out;
    }
    {   /* PR-78: a CHECK_LN start state drops onto the first line at or
         * below it (150 -> 180 here); a BY_TIME-only one stays; a line
         * ABOVE the object never pulls it up. */
        static const char gnd[] = "A\n\n[A]\ncontrol=BY_X|CHECK_LN\n1,100\n";
        static const char air[] = "A\n\n[A]\ncontrol=BY_TIME\n1,100\n";
        static const char PLTT[] = "2\n1 0,100;500,100;\n1 0,180;500,180;\n";
        ll_mov_t *mg = calloc(1, sizeof *mg), *ma = calloc(1, sizeof *ma);
        obj_script sg, sa;
        lvl_obj g, f, u;
        ll_plt pt;
        int ok = mg && ma && ll_plt_parse(PLTT, &pt) == 0;

        if (ok) {
            ll_mov_parse(gnd, mg);
            ll_mov_parse(air, ma);
            memset(&sg, 0, sizeof sg);
            memset(&sa, 0, sizeof sa);
            sg.mov = mg;
            sa.mov = ma;
            memset(&g, 0, sizeof g);
            g.sc = &sg; g.bx = g.x = 200; g.by = g.y = 150;
            f = g; f.sc = &sa;
            u = g; u.by = u.y = 190;                  /* below every line */
            ok = obj_settle(&g, &pt) == 1 && g.by == 180 && g.y == 180 &&
                 obj_settle(&f, &pt) == 0 && f.by == 150 &&
                 obj_settle(&u, &pt) == 0 && u.by == 190;
            ll_plt_free(&pt);
        }
        free(mg);
        free(ma);
        if (!ok) {
            fprintf(stderr, "object state selftest: FAILED CHECK_LN settle\n");
            goto out;
        }
    }
    rc = 0;
out:
    free(m1);
    free(m2);
    return rc;
}

/* PR-79 selftest, data-free: BY_X states walk at F/4 px/s, advance their
 * frames by MOVEMENT UNITS (the frame list's second field, not ms), keep
 * their position across state-list loops, and flip at the bounds= patrol
 * range. A time-driven or wrong-scale mutation fails here: time-driven
 * never moves x and raises t; F px/s moves 4x too far; a position reset
 * at the state loop lands on the anchor. */
static int obj_motion_selftest(void)
{
    static const char run[] =
        "A_RUN\n\n[A_RUN]\ncontrol=BY_X\nF=100,0\nT=40,0\n1,4\n2,4\n";
    static const char halt[] =
        "A_RUN\n\n[A_RUN]\ncontrol=BY_X\nF=0,0\nT=30,0\n1,4\n2,4\n";
    static const char hit[] =
        "A_RUN\nA_HIT\n\n[A_RUN]\ncontrol=BY_X\nF=100,0\nT=40,0\n"
        "A_HIT(1)=K_GEN1\n1,4,GEN K_GEN1\n2,4\n\n[A_HIT]\ncontrol=BY_TIME\n"
        "A_RUN(1)=AC_BREAK_NEXT\n1,100\n";
    static const char chd[] =
        "A_RUN\n\n[A_RUN]\ncontrol=BY_X|CH_DIR\nF=100,0\nT=40,0\n1,4\n2,4\n";
    int rc = 1;
    ll_mov_t *m1 = calloc(1, sizeof *m1), *m2 = calloc(1, sizeof *m2);
    obj_script s1, s2;
    lvl_obj a, b;

    if (!m1 || !m2)
        goto out;
    memset(&s1, 0, sizeof s1);
    memset(&s2, 0, sizeof s2);
    memset(&a, 0, sizeof a);
    memset(&b, 0, sizeof b);
    ll_mov_parse(run, m1);
    ll_mov_parse(halt, m2);
    s1.mov = m1;
    s2.mov = m2;
    a.sc = &s1;
    b.sc = &s2;
    if (m1->nst != 1 || m1->st[0].fx != 100 || m1->st[0].fr[0].ms != 4) {
        fprintf(stderr, "motion selftest: FAILED F=/units did not parse "
                        "(nst %d fx %d ms %u)\n", m1->nst, m1->st[0].fx,
                        m1->st[0].fr[0].ms);
        goto out;
    }
    /* 1. speed: 4 ticks x 100 ms at F=100 = 25 px/s = exactly 10 px
     * (an F-as-px/s mutation moves 40; a time-driven model moves 0 and
     * raises t); time is never consumed by walking states */
    a.bx = a.x = 0;
    a.b0 = a.b1 = -1;
    for (int i = 0; i < 4; i++)
        obj_advance(&a, 100);
    if (a.x != 10 || a.t != 0) {
        fprintf(stderr, "motion selftest: FAILED BY_X walk: x %d t %u"
                        " (want 10 px at F/4 px/s, time untouched)\n",
                a.x, a.t);
        goto out;
    }
    /* 4 ticks x 50 ms = 5 px at 25 px/s (an F-as-px/s mutation moves
     * 20). Frames advance on UNITS: the 4-unit frame completes once
     * (fi=1) at 5 px. Then 4 more ticks: the second frame completes at
     * 8 px, the state list ends and loops - a state loop that snapped to
     * the anchor lands back on 100 instead of 110 */
    memset(&a, 0, sizeof a);
    a.sc = &s1;
    a.bx = 100;
    a.x = 100;
    a.b0 = a.b1 = -1;
    for (int i = 0; i < 4; i++)
        obj_advance(&a, 50);
    if (a.x != 105 || a.fi != 1 || a.t != 0) {
        fprintf(stderr, "motion selftest: FAILED unit-driven frames: x %d"
                        " fi %d t %u (want 105 px, frame index 1, no"
                        " time consumed)\n", a.x, a.fi, a.t);
        goto out;
    }
    for (int i = 0; i < 4; i++)
        obj_advance(&a, 50);
    if (a.x != 110 || a.sti != 0) {
        fprintf(stderr, "motion selftest: FAILED state loop: x %d sti %d"
                        " (want 110: a walker keeps its position)\n",
                a.x, a.sti);
        goto out;
    }
    /* 2. bounds= flip: patrol 10..26 starting at x=20 walking right
     * (dir 0 = the initial estimate); 50 steps x 40 ms = 50 px of travel
     * hits the right edge at step 6, the left edge at 22, the right edge
     * again at 38, and stops at x=14 walking left. The anchor (20) is
     * never re-occupied: a state loop must not snap a walker back. */
    memset(&a, 0, sizeof a);
    a.sc = &s1;
    a.bx = 20;
    a.x = 20;
    a.b0 = 10;
    a.b1 = 26;
    for (int i = 0; i < 50; i++)
        obj_advance(&a, 40);
    if (a.x != 14 || a.dir != -1 || a.face != 0) {
        fprintf(stderr, "motion selftest: FAILED bounds flip: x %d dir %d"
                        " face %d (want 14 walking left inside 10..26,"
                        " facing untouched without CH_DIR)\n",
                a.x, a.dir, a.face);
        goto out;
    }
    /* 3. F=0: no movement at all (the walk model has nothing to
     * accumulate; the frame list falls back to time - estimate) */
    b.bx = b.x = 5;
    b.b0 = b.b1 = -1;
    for (int i = 0; i < 100; i++)
        obj_advance(&b, 40);
    if (b.x != 5) {
        fprintf(stderr, "motion selftest: FAILED F=0 walk state moved to"
                        " x %d (want: stays at 5)\n", b.x);
        goto out;
    }
    /* 4. a walker keeps its position ACROSS states: it GENs into a
     * BY_TIME state and AC_BREAK_NEXTs back. Walking 5 px at tick 4
     * enters A_HIT at x=105 (no snap to the 100 anchor), the hit frame
     * ends at tick 6 back in A_RUN at x=105, and ticks 7-8 walk on:
     * x=107. An always-snap mutation lands at 102 */
    ll_mov_parse(hit, m2);
    memset(&b, 0, sizeof b);
    b.sc = &s2;
    b.bx = 100;
    b.x = 100;
    b.b0 = b.b1 = -1;
    for (int i = 0; i < 8; i++)
        obj_advance(&b, 50);
    if (b.x != 107 || b.sti != 0) {
        fprintf(stderr, "motion selftest: FAILED cross-state carry: x %d"
                        " sti %d (want 107 back in A_RUN: transitions keep"
                        " a walker's position)\n", b.x, b.sti);
        goto out;
    }
    /* 5. PR-81 facing: the frame-number sign is the authored BASE facing
     * (ZARIA.01's worm art faces left - the eye sits 11-20% from the
     * left edge of all 9 poses - and its walk list is authored all
     * NEGATIVE, so the game draws it head-RIGHT), and control=CH_DIR is
     * what turns that facing when the walk turns at bounds=: the census
     * pairs bounds= with CH_DIR (8/8 sampled placements, 0/14 without),
     * and matching the art in both polarities on the video the winning
     * polarity flips with the travel direction (head-right at
     * t=204.4/211.2/212.4 while it moves right, head-left at
     * t=206.9/208.0/213.2 while it moves left, both at x~330 one second
     * apart = one instance turned around).
     * 1 px per 40 ms tick at F=100: from x=20 in 10..26, 40 ticks reach
     * the right edge (x=26) at tick 6, the left edge (x=10) at tick 22,
     * the right edge again at tick 38, ending at x=24 facing left; 20
     * more ticks: the left edge at tick 14, ending at x=16 facing right.
     * Mutations: no facing update on the flip (face stays 1) or one that
     * ignores the CH_DIR gate both fail here. */
    ll_mov_parse(chd, m2);
    if (!(m2->nst == 1 && (m2->st[0].ctrl & LL_CTRL_CH_DIR))) {
        fprintf(stderr, "motion selftest: FAILED control=BY_X|CH_DIR did not"
                        " parse (nst %d ctrl 0x%x)\n", m2->nst,
                        m2->st[0].ctrl);
        goto out;
    }
    memset(&b, 0, sizeof b);
    b.sc = &s2;
    b.face = 1;                          /* what the placement builder sets */
    b.bx = 20;
    b.x = 20;
    b.b0 = 10;
    b.b1 = 26;
    for (int i = 0; i < 40; i++)
        obj_advance(&b, 40);
    if (b.x != 24 || b.face != -1) {
        fprintf(stderr, "motion selftest: FAILED CH_DIR facing: x %d face %d"
                        " (want 24 facing left after the bounds flip)\n",
                b.x, b.face);
        goto out;
    }
    for (int i = 0; i < 20; i++)
        obj_advance(&b, 40);
    if (b.x != 16 || b.face != 1) {
        fprintf(stderr, "motion selftest: FAILED CH_DIR facing back: x %d"
                        " face %d (want 16 facing right)\n", b.x, b.face);
        goto out;
    }
    /* the draw mirror: authored sign preserved while facing right, XORed
     * once turned (obj_mirror_of is what obj_draw passes to the draw) */
    if (obj_mirror_of(&b, 0) != 0 || obj_mirror_of(&b, 1) != 1 ||
        obj_mirror_of(&a, 1) != 1 || obj_mirror_of(&a, 0) != 0) {
        fprintf(stderr, "motion selftest: FAILED draw mirror: facing right"
                        " must keep the authored frame mirror, facing left"
                        " must add one\n");
        goto out;
    }
    b.face = -1;
    if (obj_mirror_of(&b, 1) != 0 || obj_mirror_of(&b, 0) != 1) {
        fprintf(stderr, "motion selftest: FAILED draw mirror: a turned"
                        " CH_DIR walker must draw the other polarity\n");
        goto out;
    }
    rc = 0;
out:
    free(m1);
    free(m2);
    return rc;
}

static int obj_layer_selftest(void);

/* FIX-13 (X-11) selftest, test-only entry point (ll_level.h): proves the
 * two .MOV duration invariants without any window or game data:
 *   1. ll_mov_parse clamps non-positive durations (the M4-C rule,
 *      pinned);
 *   2. obj_advance terminates on an all-zero-ms cycle (the guard). The
 *      proof is that this RETURNS - the CI legs run it under a timeout,
 *      so a regression HANGS (caught) instead of passing silently.
 * The chain this entry point also runs (PR-49..79) covers the frame
 * grammar, anchored draw, shape geometry, push-out, object layers, the
 * object state machine and the PR-79 walking model (obj_motion_selftest:
 * speed F/4 px/s, unit-driven frame advance, bounds= flip, and no anchor
 * snap at state loops or state transitions). */
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
        seq_shape_selftest() != 0 || seq_push_selftest() != 0 ||
        obj_layer_selftest() != 0 || obj_state_selftest() != 0 ||
        obj_motion_selftest() != 0 ||
        place_grammar_selftest() != 0) {
        free(sc.mov);
        free(m);
        return 1;
    }

    free(sc.mov);
    free(m);
    printf("mov selftest: duration clamp + cycle guard + per-state seq= art"
           " selection + patch codecs + anchored draw + shape geometry + push-out + object layers + object state machine + object walking motion"
           " + PR-92 AC_ENEMY touch raise (touch arms A_ATAK, A_ATAK meets"
           " A_DEAD, hit_cd/dead gate it, bus empty) + PR-93 placement"
           " grammars (0,0 child joined not instantiated, static .SEQ lines"
           " placed at their authored x,y, x-outside-world and non-.SEQ"
           " 3-comma lines stay unplaced) + PR-95 attached static image"
           " (a .SEQ line inside a bounded actor's property block is drawn"
           " at parent + offset, not the world corner; one after a"
           " non-bounded entry keeps its absolute x,y) (ok)\n");
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
 * ("<tick>:<FWD|BKWD|UP|DOWN|JUMP|FIRE|F1..F4|ALT|NONE> entries, comma-
 * combinable per entry e.g. "100:FIRE,F1" (PR-87, mirrors the .MOV's
 * AND-groups); whitespace separated; the
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
    int layer;                              /* the lion's layer = main_back */
    int energy, lives, lock;                /* PR-68: 100 / 6 / hurt-lock ticks */
    int solid;                              /* PR-71: contact pass on/off      */
    int danger;                             /* PR-90: the DANGER + class   *
                                             * MAIN box arming stand-in    *
                                             * (off by default)            */
    int cam_ox, camy_v, camy_set;           /* PR-72: camera follow state      */
} play_ctx;

static ll_play_keys script_keys(const char *s, int tick)
{
    ll_play_keys k = { 0 };
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
            /* PR-87: comma-combinable parts, mirroring the .MOV's own
             * AND-groups: "100:FIRE,F1" = fire while weapon 1 selected
             * (the attack entry). Legacy combined token kept. */
            memset(&k, 0, sizeof k);
            for (char *sub = tok; sub && *sub; ) {
                char *cm = strchr(sub, ',');
                if (cm)
                    *cm++ = '\0';
                k.k_fwd  |= strcasecmp(sub, "FWD")  == 0;
                k.k_bkwd |= strcasecmp(sub, "BKWD") == 0;
                k.k_up   |= strcasecmp(sub, "UP")   == 0;
                k.k_down |= strcasecmp(sub, "DOWN") == 0;
                k.k_jump |= strcasecmp(sub, "JUMP") == 0 ||
                            strcasecmp(sub, "FWDJUMP") == 0;
                k.k_fire |= strcasecmp(sub, "FIRE") == 0;
                k.k_f1   |= strcasecmp(sub, "F1")   == 0;
                k.k_f2   |= strcasecmp(sub, "F2")   == 0;
                k.k_f3   |= strcasecmp(sub, "F3")   == 0;
                k.k_f4   |= strcasecmp(sub, "F4")   == 0;
                k.k_alt  |= strcasecmp(sub, "ALT")  == 0;
                if (strcasecmp(sub, "FWDJUMP") == 0)
                    k.k_fwd = 1;
                sub = cm;
            }
        }
        p = v + n;
    }
    return k;
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

/* PR-97: the contact offset of a STATE's animation - the deepest shape-box
 * bottom (ll_seq_shape_contact) among the elements its frame list can show.
 * The per-frame value wobbles by a few px over a cycle (a walk cycle's
 * lowest solid row is the planted paw, not the lifted one: ZARIA LEW.ZAR
 * WALK 30..32, RAN LEW.RAN UJ09 10..14), and the frame the actor is on at
 * one instant is not a property of where it stands: two runs that differ
 * only in which state the actor came from would park its position point 2
 * px apart with the SAME state and frame on screen. So the offset is taken
 * once per animation: the position point then never depends on which frame
 * happens to be showing, and no frame's art can dip below the line.
 * MODEL CHOICE, not read from the machine code: the shape bottom as the
 * contact geometry is the data-derived part (see the ll_seq.h note), the
 * "max over the animation" part is this port's decision - the alternative
 * (per-frame) makes the sprite bob with the walk cycle. */
static int mov_state_contact(const ll_mov_t *mov, int sti, const ll_seq *s)
{
    int best = 0;

    if (!mov || sti < 0 || sti >= mov->nst || !s || !s->count)
        return 0;
    for (int i = 0; i < mov->st[sti].nfr; i++) {
        ll_frame_ref r = ll_mov_frame_ref(mov->st[sti].fr[i].frame);
        int d;

        if (r.blank || r.idx >= (int)s->count)
            continue;
        d = ll_seq_shape_contact(s, &s->els[r.idx]);
        if (d > best)
            best = d;
    }
    return best;
}

/* PR-97: the contact offset of the state the player is in (his current
 * animation's art, resolved exactly as play_draw resolves it). */
static int play_contact(const play_ctx *pl)
{
    if (!pl->have || pl->p.sti >= pl->mov->nst)
        return 0;
    const ll_mov_st *st = &pl->mov->st[pl->p.sti];
    const ll_seq *s = play_art(pl, st->seqkey[0] ? st->seqkey
                                                 : pl->mov->seqkey);

    return mov_state_contact(pl->mov, pl->p.sti, s);
}

/* the surface under the spawn point is the CONTACT line: the position point
 * is placed contact px above it (PR-97; play_contact needs the state the
 * player starts in, which ll_player_start picks, so this runs after it) */
static void play_spawn(play_ctx *pl)
{
    ll_xy hit;
    int g = ll_plt_first_below(&pl->plt, (int)pl->sx, (int)pl->sy, &hit);
    ll_player_start(&pl->p, pl->mov, pl->sx, g ? (double)hit.y : pl->sy, g);
    ll_player_set_contact(&pl->p, play_contact(pl));
    if (g && pl->p.grounded)
        pl->p.y = (double)hit.y - (double)pl->p.contact;
}

/* PR-59: the element an object's CURRENT frame shows, shape-only tables
 * included (obj_elem returns NULL for those because it gates on drawable
 * art; the collision test does not need art). */
static const ll_seq_elem *obj_shape_elem(const lvl_obj *o, int *mirror,
                                         const ll_seq **seq)
{
    const obj_art *a = obj_art_of(o);
    *mirror = 0;
    if (!a || a->seq.count == 0 || !o->sc->mov || o->sti < 0 ||
        o->sti >= o->sc->mov->nst)
        return NULL;
    const ll_mov_st *s = &o->sc->mov->st[o->sti];
    if (s->nfr <= 0)
        return NULL;
    int fi = o->fi < s->nfr ? o->fi : s->nfr - 1;
    ll_frame_ref r = ll_mov_frame_ref(s->fr[fi].frame);
    if (r.blank || r.idx >= (int)a->seq.count)
        return NULL;
    /* PR-81: the shape belongs to the sprite as it is DRAWN, so a turned
     * CH_DIR walker (obj_walk keeps face) uses the same polarity as
     * obj_draw - otherwise the push box would sit mirrored relative to
     * the art (ll_seq_shape_push takes the object polarity directly). */
    *mirror = obj_mirror_of(o, r.mirror);
    *seq = &a->seq;
    return &a->seq.els[r.idx];
}

/* PR-97: where this object's animation is solid below its position point:
 * the same ll_seq_shape_contact geometry and the same per-animation rule as
 * the player (mov_state_contact). The art is the table the object's CURRENT
 * state selected (obj_art_of), the same one obj_draw draws and play_collide
 * tests, so the ground line, the picture and the hit box agree. 0 when the
 * script has no frames or its art carries no shape. */
static int obj_contact(const lvl_obj *o)
{
    const obj_art *a;

    if (!o->sc || !o->sc->mov)
        return 0;
    a = obj_art_of(o);
    if (!a)
        return 0;
    return mov_state_contact(o->sc->mov, o->sti, &a->seq);
}

/* PR-70: the balloon pickup (class BAL, touch method FUN_0003d370): when
 * the mover is Lew and the balloon is in its first state, the point
 * (balloon.x + 10, balloon.y + 10) is tested against Lew's bounding rect
 * (inclusive on all four sides: jl/jg at 0x3d3d7..0x3d3e9); a hit sends Lew
 * the kind-9 message (FUN_00050df4): energy < 100 -> +32 (clamped to 100)
 * and a signal bit is set on the balloon (its K_GEN1 transition: A_RUN ->
 * A_DEAD, the pop). At full energy the message is ignored - the balloon
 * stays. */
static int balloon_hit(int bx, int by, int rx, int ry, int rw, int rh)
{
    int px = bx + 10, py = by + 10;

    return px >= rx && px <= rx + rw && py >= ry && py <= ry + rh;
}

/* PR-92: the lion's touch raises AC_ENEMY (bit 7 of the PR-88 table) on
 * the touched object. The engine site is the tail of the generic touch
 * handler (0x50ed0, 0x50fd4..0x51083): it builds a 12-byte named-set
 * answer packet, sets BIT 7 into it at 0x51050 (`or dh,0x80` = bit 7 of
 * byte 0), merges it through the TEventSet OR (0x49580) together with
 * whatever 0x55130 returned, and writes the packet into the head of the
 * OTHER object it touched ([obj+0x0..0xb], `rep movsd` 0x51074..0x51081).
 * The whole handler is gated by the invulnerability countdown [obj+0x102]
 * (0x502f0; setter default 14, 0x27264) - the port mirrors that gate
 * through o->hit_cd, which PR-89 already runs on the same fixed step.
 * The bit means "an enemy touched me" exactly as the data reads: all 14
 * active AC_ENEMY consumers in the shipped .MOV texts are object-side
 * lines - "A_DEAD(1)=AC_ENEMY" x6 (the vultures of RAN.01, the barrels
 * of KADMAR.07), "A_AWAY(1)=AC_ENEMY" x4, "A_SHOW(1)=AC_ENEMY" x2 (the
 * seal of LONA.05), "A_ATAK(1)=AC_ENEMY" x1 (the RBLOK of ENA.03) - so
 * the port raises it on the touched object's OWN set (the PR-88
 * mechanism, exactly what PR-89/91 shipped) and the object's own
 * transition table decides the reaction: only the raise is new.
 * MODEL CHOICES (recorded NEXT_STEPS PR-92 + HANDOFF item 10): the
 * packet is a mailbox at the object's HEAD, the port feeds the object's
 * own set - nothing else could read the bit (0x51062 wrote [obj+0x0]
 * fields no shipped model consumes); and the countdown gates but a pure
 * touch does not SET it - the engine's handler does set 14 there, and
 * shipping that would re-pin the PR-89 kill fixtures (an object touched
 * would reject the lion's next hit for 14 ticks); open question.
 * Balloons are excluded: they have their own touch method (the block
 * above). Raise and consume happen at once, exactly like the balloon
 * path: the object's table reacts on this beat or the bus goes empty
 * again (one-beat pulses are the PR-88 bus semantics). */
static void obj_touch_enemy(lvl_obj *o, int verbose, int tick)
{
    if (!o->sc || !o->sc->mov || o->dead || o->hit_cd > 0 || o->sti < 0 ||
        o->sti >= o->sc->mov->nst)
        return;
    o->sig |= 1ull << LL_SIG_AC_ENEMY;
    static int told;
    if (verbose && !told++)
        printf("ll_level: play tick %d: first touch raises AC_ENEMY on"
               " %s/%s: the object's own A_* lines decide the reaction"
               " (PR-92)\n", tick, o->sc->leaf, o->sc->movname);
    if (obj_transition(o, o->sc->mov, 0)) {
        if (verbose)
            printf("ll_level: play tick %d: %s/%s ANSWERS the touch:"
                   " AC_ENEMY -> state %s\n", tick, o->sc->leaf,
                   o->sc->movname, o->sc->mov->st[o->sti].name);
    }
    o->sig = 0;
}

/* PR-59: FUN_00053ae0 - what a solid object does to the lion. The lion is
 * B (pushed); an object is A when its CURRENT state carries SOLID (flag word
 * 0x400) and the lion's state is not ANTI_SOLID (0x10000). The push is
 * ll_seq_shape_push; SOLID alone displaces B by the penetration r
 * horizontally (message {1, r, 0}, 0x53c4d), SOLID2 by the full vector
 * (dx, dy) (0x53c0e). SEMI_SOLID: kind-1 r/5 applied, the kind-13 velocity
 * impulse counted (below).
 * A push UP (dy < 0) is standing on the object: grounded, support set.
 * The shape test uses the PAIRED form (ll_seq_shape_push2): the lion's
 * element and the object's element live in different .SEQ payloads, and
 * reading one from the other's bytes silently broke lion-vs-object
 * contacts (measured 2026-11-17, ZARIA.01: no lion/worm shape contact
 * at any distance; with the paired read the worms answer at their real
 * margin ranges). The earlier PR-66/PR-68 pins of this path were
 * therefore computed through wrong bytes and are re-pinned.
 * OFF unless LEO_PLAY_SOLID=1: WHICH pairs the game tests (the per-class
 * collision loops behind FUN_00053c90 and the CHECK_LEW/CHECK_ALL gating)
 * is not resolved yet, and with it on RAN.01's invisible EDGE0046 stops the
 * lion at x~135 - unverified against the original, so not the default. */
static void play_collide(play_ctx *pl, const lvl_obj *objs, int nobj,
                         int verbose, int tick)
{

    pl->p.support = 0;
    if (pl->lock > 0)
        pl->lock--;
    if (!pl->solid || !pl->have || pl->p.sti >= pl->mov->nst)
        return;
    const ll_mov_st *ls = &pl->mov->st[pl->p.sti];
    if (ls->ctrl & LL_CTRL_ANTI_SOLID)
        return;
    const ll_seq *lseq = play_art(pl, ls->seqkey[0] ? ls->seqkey
                                                    : pl->mov->seqkey);
    if (!lseq || !lseq->count)
        return;
    ll_frame_ref lr = ll_mov_frame_ref(ll_player_frame(&pl->p));
    if (lr.blank || lr.idx >= (int)lseq->count)
        return;
    const ll_seq_elem *le = &lseq->els[lr.idx];
    {   /* balloons: not solids, their own touch method */
        int bx0, by0, bw0, bh0;
        int lx0 = (int)(pl->p.x >= 0 ? pl->p.x + 0.5 : pl->p.x - 0.5);
        int ly0 = (int)(pl->p.y >= 0 ? pl->p.y + 0.5 : pl->p.y - 0.5);

        ll_seq_shape_bbox(lseq, le, &bx0, &by0, &bw0, &bh0);
        for (int k = 0; k < nobj; k++) {
            lvl_obj *o = (lvl_obj *)&objs[k];
            if (!o->sc || !ci_eq(o->sc->leaf, "BAL") || !o->sc->mov ||
                o->sti != o->sc->mov->start_st || o->layer != pl->layer)
                continue;
            if (!balloon_hit(o->x, o->y, lx0 + bx0, ly0 + by0, bw0, bh0))
                continue;
            if (pl->energy >= 100)
                continue;                       /* the message is ignored */
            pl->energy += 32;
            if (pl->energy > 100)
                pl->energy = 100;
            o->sig |= 1ull << LL_SIG_K_GEN1;        /* K_GEN1: the pop  */
            (void)obj_transition(o, o->sc->mov, 0);
            o->sig = 0;
            if (verbose)
                printf("ll_level: play tick %d: balloon at %d,%d picked:"
                       " energy %d\n", tick, o->x, o->y, pl->energy);
        }
    }
    /* PR-82: the lion's facing (ll_player_step keeps it from the CH_DIR
     * states, same flag and same model as obj_walk) decides which way his
     * collision shape and his drawn sprite point, mirrored on top of the
     * authored frame sign. The collision shape uses the DRAWN mirror,
     * exactly like obj_shape_elem does for scene objects (PR-81 rule:
     * the shape keeps the drawn polarity). */
    int lm = ll_player_mirror_of(&pl->p, lr.mirror);
    for (int k = 0; k < nobj; k++) {
        lvl_obj *o = (lvl_obj *)&objs[k];
        if (!o->sc || !o->sc->mov || o->sti < 0 || o->sti >= o->sc->mov->nst)
            continue;
        unsigned c = o->sc->mov->st[o->sti].ctrl;
        if (!(c & LL_CTRL_SOLID))
            continue;
        /* FUN_00052780: the mover's own CHECK_ZW / CHECK_ANI pick the list
         * it is tested against (ani_plat objects are reached through
         * another path, not ported) */
        if (o->src == 0 ? !(ls->ctrl & LL_CTRL_CHECK_ZW)
                        : o->src == 1 ? !(ls->ctrl & LL_CTRL_CHECK_ANI) : 1)
            continue;
        /* the plane gate (FUN_0004b500): obj.plane == mover.plane, plane =
         * the parallax layer [+0x24] (entry index of the placing list);
         * CHECK_ALL_PLANE lifts it */
        if (o->layer != pl->layer && !(ls->ctrl & LL_CTRL_CHECK_ALL_PLANE))
            continue;
        int om;
        const ll_seq *oseq = NULL;
        const ll_seq_elem *oe = obj_shape_elem(o, &om, &oseq);
        if (!oe)
            continue;
        int lx = (int)(pl->p.x >= 0 ? pl->p.x + 0.5 : pl->p.x - 0.5);
        int ly = (int)(pl->p.y >= 0 ? pl->p.y + 0.5 : pl->p.y - 0.5);
        int d1, d2, r;
        r = ll_seq_shape_push2(oseq, oe, o->x, o->y, om ? -1 : 1,
                               lseq, le, lx, ly, lm ? -1 : 1, &d1, &d2);
        if (d1 == 0 && d2 == 0)
            continue;
        int dx = (c & LL_CTRL_SOLID2) ? d1 : r;
        int dy = (c & LL_CTRL_SOLID2) ? d2 : 0;
        if (c & LL_CTRL_SEMI_SOLID) {
            /* 0x53b47..0x53ba9: SEMI_SOLID answers with the horizontal
             * penetration r only: |r| > 5 moves B by r/5 (kind 1), and a
             * kind-13 message adds 30*clamp(r, -4..4) to B's velocity x
             * (FUN_00054560: [+0xc2] += a). The velocity's unit scale is
             * not pinned (integrated at 0x5427c as dt*v/100), so only the
             * position part is applied and the impulse is counted. */
            static int imp;
            dx = (r > 5 || r < -5) ? r / 5 : 0;
            dy = 0;
            if (!imp++)
                printf("ll_level: play: SEMI_SOLID velocity impulse"
                       " (kind 13) counted, not applied (unit scale"
                       " unresolved)\n");
        }
        pl->p.x += dx;
        pl->p.y += dy;
        if (dy < 0 && !(c & LL_CTRL_SEMI_SOLID)) {
            pl->p.support = 1;
            pl->p.vy = 0;
            if (!pl->p.grounded) {
                pl->p.grounded = 1;
                pl->p.sig |= 1ull << LL_SIG_K_GEN2;
            }
        }
        if (pl->p.atc && !pl->p.atc_hit && o->hp >= 0 && !o->dead) {
            /* PR-89: the lion's attack window is open and the shapes
             * just met (same contact as the push-out above; the engine's
             * touch pipeline runs the push-out and then the hit effects
             * - 0x552e0 -> 0x55230). One hit per opened window. */
            int hh = obj_hit(o, LL_LION_HIT_DAMAGE);
            if (hh) {
                pl->p.atc_hit = 1;
                if (verbose)
                    printf("ll_level: play tick %d: lion %s %s/%s:"
                           " damage %d -> hp %d%s\n", tick,
                           hh == 2 ? "KILLS" : "hits", o->sc->leaf,
                           o->sc->movname, LL_LION_HIT_DAMAGE, o->hp,
                           hh == 2 ? " (dead)" : "");
            }
        }
        if (o->atc && !o->atc_hit && o->damage > 0 && pl->lock == 0) {
            /* PR-90: an enemy whose OWN attack window is open (its
             * A_ATAK frames carry the ATC_ON/ATC_OFF pair: ZARIA's
             * LICHA3 opens its window at frame -4 and closes it at
             * -8) lands through it on the lion. One hit per armed
             * state episode, exactly like the lion's own (o->atc_hit
             * resets only on a state change). The numbers are the
             * ones the contact path already uses: the authored
             * damage= (the engine delivers [obj+0xb1], 0x55130) and
             * the lion's 30-tick hurt lock (0x50304). What the
             * window adds over the contact model: the hit is the
             * object's ACTIVE attack (and it runs BEFORE the contact
             * block, so while a window is open the touch answers as
             * the attack, not as an extra contact hit). MODEL: the
             * engine's consumer of the atc_on window is still not
             * pinned (NEXT_STEPS PR-90 record). */
            pl->energy -= o->damage;
            pl->lock = 30;
            o->atc_hit = 1;
            /* PR-91: and the hit raises AC_ENEMY_ATC on the LION's own
             * set (the mirror of what the lion's attack does to the
             * object). The reaction is then pure authored data: the
             * .MOV's "A_DOSTAL(1)=AC_ENEMY_ATC" lines, the seq=hit art
             * and the frame-1 pullback impulse all landed with PR-74/
             * PR-86/PR-88; nothing here is new mechanism. */
            pl->p.sig |= 1ull << LL_SIG_AC_ENEMY_ATC;
            if (verbose)
                printf("ll_level: play tick %d: %s/%s ATTACKS through"
                       " its window: damage %d -> lion energy %d\n",
                       tick, o->sc->leaf, o->sc->movname, o->damage,
                       pl->energy);
        }
        if (o->damage > 0 && pl->lock == 0 && !o->dead) {
            /* FUN_00050ed0 / 00054550: a touched enemy hits with ITS OWN
             * damage (the .des `damage=` of the placement); the lion's hurt
             * lock is 30 ticks (0x50304: [+0x102] = 0x1e) */
            pl->energy -= o->damage;
            pl->lock = 30;
            if (verbose)
                printf("ll_level: play tick %d: hit by %s/%s: damage %d ->"
                       " energy %d\n", tick, o->sc->leaf, o->sc->movname,
                       o->damage, pl->energy);
        }
        /* PR-92: the same touch answers with the AC_ENEMY raise too (the
         * engine's 0x50ed0 tail builds its packet after the damage and
         * DANGER branches, and this loop's shapes have just met). */
        obj_touch_enemy(o, verbose, tick);
        if (verbose)
            printf("ll_level: play tick %d: solid %s/%s %s at %d,%d push"
                   " %d,%d (ret %d) lion %d,%d\n", tick, o->sc->leaf,
                   o->sc->movname, (c & LL_CTRL_SOLID2) ? "SOLID2"
                   : (c & LL_CTRL_SEMI_SOLID) ? "SEMI" : "SOLID",
                   o->x, o->y, dx, dy, r, lx, ly);
        lx = (int)pl->p.x;            /* later objects see the moved lion */
    }
    /* PR-92 (2/2): the touch raise for objects that are NOT solids. The
     * engine's touch handler is not the solid-push pipeline - 0x50ed0
     * runs for any pair that meets (the push-out above is only its
     * FUN_00053ae0 part) - and EVERY shipped AC_ENEMY consumer lives in
     * a state without a SOLID flag (GPTAK: BY_TIME|FRAME_REV|CHECK_LEW|
     * CH_DIR|DANGER, FOKA: BY_TIME|CHECK_LEW|CHECK_LN, RBLOK: BY_TIME|
     * FRAME_REV|CHECK_LN|CHECK_LEW|CHECK_ANI|CH_DIR|DANGER): a raise
     * only inside the solid loop would be data-dead. Same paired-shape
     * test as the loop, evaluated at the lion's POST-push position (the
     * engine answers after the push-out: 0x552e0 -> 0x55230), same
     * list/plane gates, but nobody is moved by it. Objects whose CURRENT
     * state IS solid were already raised inside the loop. */
    {
        int lx2 = (int)(pl->p.x >= 0 ? pl->p.x + 0.5 : pl->p.x - 0.5);
        int ly2 = (int)(pl->p.y >= 0 ? pl->p.y + 0.5 : pl->p.y - 0.5);
        for (int k = 0; k < nobj; k++) {
            lvl_obj *o = (lvl_obj *)&objs[k];
            if (!o->sc || !o->sc->mov || o->sti < 0 ||
                o->sti >= o->sc->mov->nst)
                continue;
            if (ci_eq(o->sc->leaf, "BAL"))      /* their own touch method */
                continue;
            unsigned c = o->sc->mov->st[o->sti].ctrl;
            if (c & LL_CTRL_SOLID)              /* raised inside the loop  */
                continue;
            if (o->src == 0 ? !(ls->ctrl & LL_CTRL_CHECK_ZW)
                            : o->src == 1 ? !(ls->ctrl & LL_CTRL_CHECK_ANI) : 1)
                continue;
            if (o->layer != pl->layer && !(ls->ctrl & LL_CTRL_CHECK_ALL_PLANE))
                continue;
            int om;
            const ll_seq *oseq = NULL;
            const ll_seq_elem *oe = obj_shape_elem(o, &om, &oseq);
            if (!oe)
                continue;
            int d1, d2;
            ll_seq_shape_push2(oseq, oe, o->x, o->y, om ? -1 : 1,
                               lseq, le, lx2, ly2, lm ? -1 : 1, &d1, &d2);
            if (d1 == 0 && d2 == 0)
                continue;                       /* no shape meet: no touch  */
            obj_touch_enemy(o, verbose, tick);
        }
    }
}

/* one fixed step + the shared logging (transition lines always; per-tick
 * lines only on the headless CI grid where they are grep-pinned) */
static void play_advance(play_ctx *pl, const ll_play_keys *keys,
                         unsigned dt_ms, int verbose, int tick,
                         const lvl_obj *objs, int nobj)
{
    int was = pl->p.sti;
    /* The terrain tests of THIS step use the contact of the state the player
     * was in when it started (that is the animation being shown at this
     * instant). */
    int ev = ll_player_step(&pl->p, &pl->plt, keys, pl->xmin, pl->xmax,
                            dt_ms);
    /* PR-97: re-read the contact AFTER the step, because ll_player_step is
     * where the state transitions happen, and keep the actor's contact LINE
     * invariant across a state change: a grounded actor stands on the same
     * surface, so when the new animation's shape bottom sits a different
     * distance below the position point, the position point moves by exactly
     * that difference. Without this the standing height would depend on
     * which state the actor came from (two runs differing only by an
     * A_TURN detour parked the same state and frame 2 px apart). MODEL
     * CHOICE (the engine surely re-tests the ground every tick, the exact
     * instruction is unread, HANDOFF 7 open item): the shape-bottom geometry
     * itself is the data-derived part. */
    int nc = play_contact(pl);
    if (pl->p.grounded && nc != pl->p.contact)
        pl->p.y += (double)(pl->p.contact - nc);
    ll_player_set_contact(&pl->p, nc);
    play_collide(pl, objs, nobj, verbose, tick);
    if (pl->danger && pl->have) {   /* PR-90 stand-in for the engine's
                                     * per-class attack AI: see
                                     * obj_arm_atak's model note. */
        int ax = (int)(pl->p.x >= 0 ? pl->p.x + 0.5 : pl->p.x - 0.5);
        int ay = (int)(pl->p.y >= 0 ? pl->p.y + 0.5 : pl->p.y - 0.5);
        for (int k = 0; k < nobj; k++)
            obj_arm_atak((lvl_obj *)&objs[k], ax, ay, verbose, tick);
    }
    if (pl->energy <= 0) {                  /* 0x50400: lives--, energy 100 */
        pl->lives--;
        pl->energy = 100;
        pl->lock = 0;
        printf("ll_level: play: energy gone -> respawn, %d live(s) left\n",
               pl->lives);
        play_spawn(pl);
    }
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
/* Camera follow, measured on a playthrough video (5150 frames tracked by
 * the lion's colour, outside git): the lion's x on the 320-wide view sits at
 * ~95 while he walks right and ~215 while he faces left (so the view leads in
 * his facing direction); vertically the lion rests with his feet near 64% of
 * the view and the camera follows him (jumps included), see follow_camy.
 * The rates of the camera's catch-up are NOT measured: the x offset eases
 * toward its target at 6 units per 20 ms tick (a viewer choice).
 * Walking speed from the same video: lion-locked camera shifts have a
 * median 75 units/s (3..5 per video frame at 25 fps) - consistent with the
 * .MOV's T=90 cap. */
#define CAM_LEAD_R 100
#define CAM_LEAD_L 220

static int follow_camx(play_ctx *pl, int cw, int cam_max)
{
    int target = pl->p.walkdir < 0 ? CAM_LEAD_L : CAM_LEAD_R;
    int d = target - pl->cam_ox;

    (void)cw;
    pl->cam_ox += d > 6 ? 6 : d < -6 ? -6 : d;
    int c = (int)pl->p.x - pl->cam_ox;
    if (c < 0)
        c = 0;
    if (c > cam_max)
        c = cam_max;
    return c;
}

/* PR-84 (owner compared our ZARIA.01 frame with the original): the lion
 * sat ~19 px too LOW in the view - on the build they tested (before
 * PR-65's reference-layer world rule: max-of-planes 600 -> cam_ymax
 * 400 -> feet at the view bottom). Measured the anchor directly on the
 * original (gp.mp4 t=205.0s frame, 320x200 game space, colour-split of
 * the lion: paws' yellow n=45 at y 133..180): a grounded lion's feet
 * sit at ~180 of 200; PR-72's 5150-frame mode was 175 (its mask
 * included the shadow ellipse, a couple of px below the paws). The
 * 175..180 band is NOT the world clamp: ZARIA.01's reference layer is
 * 700 tall -> cam_ymax 500, and the original's camy for ground y=680
 * is exactly 500 (feet 180) - today's HEAD already lands there, but
 * only by OVER-CLAMPING the 65% anchor (680-130=550 > 500): anywhere
 * the clamp does not bind, the 65% rule sat 40+ px too high. PR-73's
 * single mid-level observation "feet at ~128 of 200" was taken during
 * the camera's catch-up (it measured its own ~0.5 u/frame standing
 * drift), not an anchor.
 * Model: a GROUNDED lion eases the view to feet-at-CAM_FEET_Y; while
 * AIRBORNE the view HOLDS (PR-73 measured the rise/fall of the lion on
 * the view through the whole jump arc; anchoring through the arc would
 * cancel it). The ease rate is not measured (2 u/tick, viewer choice,
 * same honesty flag as the x ease). */
#define CAM_FEET_Y 178

static int follow_camy(play_ctx *pl, int ch, int cam_ymax)
{
    (void)ch;
    /* PR-97: p->y is the position point, and CAM_FEET_Y was measured on the
     * ORIGINAL'S PAWS (view y 175..180 of the 200-row view), which is the
     * contact line: y + contact. While grounded the contact line IS the .plt
     * surface the lion stands on, whatever frame he is showing, so the view
     * does not step when the animation changes. */
    double fy = pl->p.y + (double)pl->p.contact;
    int feet = (int)(fy >= 0 ? fy + 0.5 : fy - 0.5);
    if (!pl->camy_set) {                   /* spawn: snap, no crawl-in */
        pl->camy_set = 1;
        pl->camy_v = feet - CAM_FEET_Y;
    }
    if (pl->p.grounded) {
        int target = feet - CAM_FEET_Y;
        int d = target - pl->camy_v;
        pl->camy_v += d > 2 ? 2 : d < -2 ? -2 : d;
    }
    if (pl->camy_v < 0)
        pl->camy_v = 0;
    if (pl->camy_v > cam_ymax)
        pl->camy_v = cam_ymax;
    return pl->camy_v;
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
    /* PR-82: the lion turns with his facing, same rule as obj_draw's
     * obj_mirror_of (CH_DIR states set it from walkdir; direction-baked
     * states like A_GO_TO_BALON_L draw with their authored sign and keep
     * the facing as it was). */
    ll_seq_draw_elem_anchor(s, e, canvas, cw, ch, x, y,
                            ll_player_mirror_of(&pl->p, r.mirror), NULL);
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
 * painted over it in tlo order (ll_pcm.h). Strips wrap modulo the layer
 * width as in the engine; the .PCX backdrop itself does not scroll (PR-51,
 * viewer policy).
 *
 * PR-57: the per-layer parallax is the engine's own (FUN_00057460/57500/
 * 57530). The level .des carries `speed=x,y;x,y;...` - one percentage pair
 * per tlo layer (default 100*(i+1)/n) - and `main_back=` names the
 * reference layer R (the one that is the world). With camera (cx, cy):
 *     x_i = cx * X[i] / X[R]
 *     y_i = cy * Y[i] / Y[R] + (H_i - 200) - (H_R - 200) * Y[i] / 100
 * (C integer division; 200 = the view height, here ch). */
#define MAX_PCM_LAYERS 8
#define LVL_VIEW_W 320          /* the engine's view, world units */
#define LVL_VIEW_H 200

typedef struct {
    int n;                      /* tlo layers, present or not */
    int ref;                    /* main_back */
    int sx[16], sy[16];         /* percent */
} par_t;

static void par_parse(par_t *pp, const char *speed, const char *mainback,
                      int ntlo)
{
    memset(pp, 0, sizeof *pp);
    pp->n = ntlo > 16 ? 16 : ntlo;
    pp->ref = mainback && *mainback ? atoi(mainback) : 0;
    for (int i = 0; i < pp->n; i++)
        pp->sx[i] = pp->sy[i] = pp->n ? 100 * (i + 1) / pp->n : 100;
    int i = 0;
    const char *c = speed;
    while (c && *c && i < pp->n) {
        int a, b, used = 0;
        if (sscanf(c, " %d , %d ;%n", &a, &b, &used) < 2 &&
            sscanf(c, " %d , %d%n", &a, &b, &used) < 2)
            break;
        pp->sx[i] = a;
        pp->sy[i] = b;
        i++;
        c += used;
    }
    if (pp->ref < 0 || pp->ref >= pp->n)
        pp->ref = 0;
}

static void layers_at(const ll_pcm *lay, const int *tix, int nlay,
                      const par_t *pp, int cam, int camy, int cw, int ch,
                      int world_w, int world_h, unsigned char *canvas)
{
    (void)world_w;
    for (int k = 0; k < nlay; k++) {
        const ll_pcm *L = &lay[k];
        int i = tix[k];
        long long xr = pp->sx[pp->ref] ? pp->sx[pp->ref] : 100;
        long long yr = pp->sy[pp->ref] ? pp->sy[pp->ref] : 100;
        long long sx = (long long)cam * pp->sx[i] / xr;
        long long sy = (long long)camy * pp->sy[i] / yr +
                       ((long long)L->h - ch) -
                       ((long long)world_h - ch) * pp->sy[i] / 100;
        ll_pcm_blit(L, canvas, cw, ch, (int)sx, (int)sy, 0, 0);
    }
}

/* PR-61: an object is NOT drawn at x - cam: FUN_00052d30/52dc0 take the
 * object's authored position in ITS layer and subtract that layer's own
 * scroll (57500/57530, the same x_i/y_i layers_at uses): screen x =
 * ox - cam*X[i]/X[R], screen y = oy - (cam_y*Y[i]/Y[R] + (H_i - ch) -
 * (H_R - ch)*Y[i]/100). An object's layer [obj+0x24] is the entry index of
 * the .des list that placed it (zwierzaki=none,wulkany.des,dymy.des -> the
 * volcanoes sit on layer 1, with PLAN2; the clouds of RAN on layer 0). H_i is
 * the height of that tlo layer when its art is on the CD, else the world
 * height (offset 0). */
static void obj_layer_cam(const lvl_obj *o, const par_t *pp, const ll_pcm *lay,
                          const int *lay_tix, int nlay, int cam, int camy,
                          int ch, int world_h, int *ocam, int *ocamy)
{
    *ocam = cam;
    *ocamy = camy;
    if (pp->n <= 0)
        return;
    int i = o->layer < pp->n ? o->layer : pp->n - 1;
    long long xr = pp->sx[pp->ref] ? pp->sx[pp->ref] : 100;
    long long yr = pp->sy[pp->ref] ? pp->sy[pp->ref] : 100;
    int hi = world_h;
    for (int k = 0; k < nlay; k++)
        if (lay_tix[k] == i)
            hi = (int)lay[k].h;
    *ocam = (int)((long long)cam * pp->sx[i] / xr);
    *ocamy = (int)((long long)camy * pp->sy[i] / yr + (hi - ch) -
                   ((long long)world_h - ch) * pp->sy[i] / 100);
}

static void paint_base(unsigned char *canvas, const unsigned char *base,
                       int bw, int bh, int cw, int ch, int scrolls,
                       const par_t *pp, int bg_tix, int cam, int camy,
                       int world_h);

/* PR-61 selftest: per-layer object scroll, hand-derived from 57500/57530.
 * speeds 40,80;70,70;100,100 (RAN.01), main_back 2, cam 1000/camy 100, view
 * 400, world 400, layers all 400 high: x_i = 1000*X/100 = 400/700/1000;
 * y_i = 100*Y/100 + 0 - 0 = 80/70/100. A layer 300 high shifts by H-ch =
 * -100, and the layer-2 reference keeps cam exactly. */
static int obj_layer_selftest(void)
{
    par_t pp;
    ll_pcm lay[1];
    int tix[1] = { 0 };
    lvl_obj o;
    int c, cy;
    static const int wantx[3] = { 400, 700, 1000 };
    static const int wanty[3] = { 80, 70, 100 };

    par_parse(&pp, "40,80;70,70;100,100", "2", 3);
    memset(lay, 0, sizeof lay);
    memset(&o, 0, sizeof o);
    for (int i = 0; i < 3; i++) {
        o.layer = (unsigned char)i;
        obj_layer_cam(&o, &pp, lay, tix, 0, 1000, 100, 400, 400, &c, &cy);
        if (c != wantx[i] || cy != wanty[i]) {
            fprintf(stderr, "layer selftest: FAILED layer %d: cam %d,%d (want"
                            " %d,%d)\n", i, c, cy, wantx[i], wanty[i]);
            return 1;
        }
    }
    lay[0].h = 300;
    o.layer = 0;
    obj_layer_cam(&o, &pp, lay, tix, 1, 1000, 100, 400, 400, &c, &cy);
    if (c != 400 || cy != 80 - 100) {
        fprintf(stderr, "layer selftest: FAILED short layer: cam %d,%d (want"
                        " 400,-20)\n", c, cy);
        return 1;
    }
    {   /* the backdrop: base 10x4, view 6x2, speeds 50,50;100,100 ref 1, cam
         * 14 -> x shift 7; camy 2 -> y = 2*50/100 + (4-2) - (4-2)*50/100 = 2 */
        unsigned char base[40], out[12];
        par_t p2;

        par_parse(&p2, "50,50;100,100", "1", 2);
        for (int i = 0; i < 40; i++)
            base[i] = (unsigned char)i;
        paint_base(out, base, 10, 4, 6, 2, 1, &p2, 0, 14, 2, 4);
        for (int y = 0; y < 2; y++)
            for (int x = 0; x < 6; x++)
                if (out[y * 6 + x] != (y + 2) * 10 + (x + 7) % 10) {
                    fprintf(stderr, "layer selftest: FAILED backdrop at %d,%d:"
                                    " %d\n", x, y, out[y * 6 + x]);
                    return 1;
                }
        paint_base(out, base, 6, 2, 6, 2, 0, &p2, 0, 14, 2, 4); /* PCM-only */
        if (memcmp(out, base, 12) != 0) {
            fprintf(stderr, "layer selftest: FAILED unscrolled base\n");
            return 1;
        }
    }
    return 0;
}

/* PR-64/66: the .PCX backdrop is the first tlo layer and scrolls like every
 * other (FUN_00057500/57530): x = cam * X[i] / X[R] wrapping modulo the image
 * width (RAN.01: 40% of a 28800 px world - a 960 px image necessarily wraps,
 * so the engine tiles it too); y = cam_y * Y[i] / Y[R] + (H_i - 200) -
 * (H_R - 200) * Y[i] / 100 with the engine's 200-row view, clamped to the
 * image. base is bw x bh; a view-sized base (PCM-only levels, bw == cw) is
 * copied as is. */
static void paint_base(unsigned char *canvas, const unsigned char *base,
                       int bw, int bh, int cw, int ch, int scrolls,
                       const par_t *pp, int bg_tix, int cam, int camy,
                       int world_h)
{
    long long sx = 0, sy = 0;

    if (scrolls && pp->n > 0 && bg_tix < pp->n) {
        long long xr = pp->sx[pp->ref] ? pp->sx[pp->ref] : 100;
        long long yr = pp->sy[pp->ref] ? pp->sy[pp->ref] : 100;
        sx = (long long)cam * pp->sx[bg_tix] / xr;
        sy = (long long)camy * pp->sy[bg_tix] / yr + (bh - ch) -
             ((long long)world_h - ch) * pp->sy[bg_tix] / 100;
    }
    sx %= bw;
    if (sx < 0)
        sx += bw;
    if (sy > bh - ch)
        sy = bh - ch;
    if (sy < 0)
        sy = 0;
    memset(canvas, 0, (size_t)cw * (size_t)ch);
    for (int y = 0; y < ch && sy + y < bh; y++) {
        const unsigned char *r = base + (size_t)(sy + y) * (size_t)bw;
        unsigned char *d = canvas + (size_t)y * (size_t)cw;
        for (int x = 0; x < cw; x++)
            d[x] = r[(sx + x) % bw];
    }
}

/* PR-67: the HUD strip (belka=<level .des>, HD ETAPY block). Grammar read
 * from all 12 shipped BELKA files: optional `key=on|off` lines (tools, crown),
 * then items, each an `x,y` line followed by an `[N ]<name>.seq` line - the
 * first item carries N = 10 (its ten frames). By position: 0 the energy head
 * (ten frames, front-facing at full energy turning away), 1 the lives digits,
 * 2 the tools icons, 3 the item-count digits, 4 the item frame, 5 the crown.
 * Coordinates are in the 320x200 view (285,165 -> the bottom-right box of the
 * playthrough video). The .SEQ art is the level's CD ETAPY record.
 * Not modeled: inventory (the item box shows its empty frame), the crown, and
 * the real lives/energy: the HUD shows full energy and the 6 lives the
 * playthrough video starts with (STATUS.LEO's fields are not mapped). */
#define HUD_MAXIT 8
typedef struct {
    int n;
    int tools_off;
    struct {
        int x, y;
        unsigned char *blob;
        ll_seq seq;
        int ok;
    } it[HUD_MAXIT];
} hud_t;

static void hud_free(hud_t *h)
{
    for (int i = 0; i < h->n; i++)
        if (h->it[i].ok) {
            ll_seq_free(&h->it[i].seq);
            free(h->it[i].blob);
        }
    memset(h, 0, sizeof *h);
}

static void hud_load(hud_t *h, llrs *hd_ety, const llrs_slot *hblk,
                     llrs *cd_ety, const llrs_slot *recs, unsigned nrec,
                     const char *belka)
{
    memset(h, 0, sizeof *h);
    if (!hd_ety || !hblk || !belka || !belka[0])
        return;
    llrs_slot *hr = NULL;
    unsigned nh = 0;
    if (llrs_block(hd_ety, hblk, &hr, &nh))
        return;
    char *txt = NULL;
    for (unsigned k = 0; k < nh && !txt; k++)
        if (hr[k].id == 1 && ci_eq(hr[k].name, belka)) {
            unsigned char *t = NULL;
            long n = llrs_read(hd_ety, &hr[k], &t);
            if (n > 0) {
                txt = malloc((size_t)n + 1);
                if (txt) {
                    memcpy(txt, t, (size_t)n);
                    txt[n] = '\0';
                }
            }
            free(t);
        }
    llrs_block_free(hr);
    if (!txt)
        return;
    int px = -1, py = -1;
    for (char *save = NULL, *ln = strtok_r(txt, "\r\n", &save); ln;
         ln = strtok_r(NULL, "\r\n", &save)) {
        int a, b;
        char nm[32];
        if (strncmp(ln, "tools=off", 9) == 0) {
            h->tools_off = 1;
        } else if (strchr(ln, '=')) {
            continue;
        } else if (sscanf(ln, "%d,%d", &a, &b) == 2 && !strstr(ln, ".")) {
            px = a;
            py = b;
        } else if (px >= 0 && h->n < HUD_MAXIT &&
                   (sscanf(ln, "%d %31s", &a, nm) == 2 ||
                    sscanf(ln, "%31s", nm) == 1)) {
            int i = h->n++;
            h->it[i].x = px;
            h->it[i].y = py;
            px = -1;
            for (unsigned k = 0; k < nrec; k++)
                if (recs[k].id == 1 && ci_eq(recs[k].name, nm)) {
                    unsigned char *raw = NULL;
                    long n = llrs_read(cd_ety, &recs[k], &raw);
                    if (n > 0 && ll_seq_parse(raw, (uint32_t)n,
                                              &h->it[i].seq) == 0) {
                        h->it[i].blob = raw;
                        h->it[i].ok = 1;
                    } else {
                        free(raw);
                    }
                }
        }
    }
    free(txt);
    printf("ll_level: hud: %s: %d item(s) (energy head, lives digits, item"
           " box...), tools %s\n", belka, h->n, h->tools_off ? "off" : "on");
}

static void hud_elem(const hud_t *h, int i, unsigned el, unsigned char *cv,
                     int cw, int ch, int x, int y)
{
    if (i >= h->n || !h->it[i].ok || el >= h->it[i].seq.count)
        return;
    ll_seq_draw_elem_anchor(&h->it[i].seq, &h->it[i].seq.els[el], cv, cw, ch,
                            x, y, 0, NULL);
}

static void hud_draw(const hud_t *h, unsigned char *cv, int cw, int ch,
                     int energy, int lives)
{
    int e = (100 - energy) / 10;

    if (h->n < 2)
        return;
    hud_elem(h, 0, (unsigned)(e < 0 ? 0 : e > 9 ? 9 : e), cv, cw, ch,
             h->it[0].x, h->it[0].y);
    if (h->it[1].ok && h->it[1].seq.count >= 10) {
        char dg[12];
        int x = h->it[1].x;
        snprintf(dg, sizeof dg, "%d", lives < 0 ? 0 : lives);
        for (const char *c = dg; *c; c++) {
            unsigned d = (unsigned)(*c - '0');
            hud_elem(h, 1, d, cv, cw, ch, x, h->it[1].y);
            x += (int)(short)h->it[1].seq.els[d].b15 + 2;
        }
    }
    if (!h->tools_off && h->n > 4)
        hud_elem(h, 4, 0, cv, cw, ch, h->it[4].x, h->it[4].y);
}

static void objs_draw(const lvl_obj *objs, int nobj, unsigned char *canvas,
                      int cw, int ch, int cam, int camy, const par_t *pp,
                      const ll_pcm *lay, const int *lay_tix, int nlay,
                      int world_h)
{
    /* PR-93: static images first - they are the scene the actors stand
     * on (the video: the lion in front of the stone shelves, the balloons
     * in front of them too), and the placement files order does not give
     * that: ANI1.DES comes after the ZWIERZ files in every level. */
    for (int k = 0; k < nobj; k++)
        if (objs[k].static_img) {
            int c, cy;
            obj_layer_cam(&objs[k], pp, lay, lay_tix, nlay, cam, camy, ch,
                          world_h, &c, &cy);
            obj_draw(&objs[k], canvas, cw, ch, c, cy);
        }
    for (int k = 0; k < nobj; k++)
        if (!objs[k].static_img) {
            int c, cy;
            obj_layer_cam(&objs[k], pp, lay, lay_tix, nlay, cam, camy, ch,
                          world_h, &c, &cy);
            obj_draw(&objs[k], canvas, cw, ch, c, cy);
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
    /* PR-93: the two placement grammars the census proved. Static image
     * lines ("<LEAF>,<name>.seq,<x>,<y>", 3 commas) place one image whose
     * .SEQ is named by the line itself; attached children are the five-
     * field entries authored at exactly 0,0 - every one of the 130 in the
     * shipped data follows another entry or its property lines (a vulture's
     * egg, a saw-fish on a log, a drop on a leaf, demo text), so none of
     * them is a placement at the world origin. All counted here; the
     * statics are also placed and drawn, the children are not. */
    int statics, statics_placed, static_out, static_missing,
        static_shapeonly, attached, static_attached;
    int ntrans, ctrl_other, directive, field_other, drop_st, drop_fr, drop_tr;
    int fr_total, fr_blank, fr_mirror, fr_oob;   /* frame numbers: the game's
                                                  * 1-based/signed grammar */
} obj_stats;

typedef struct {                    /* the scene being built for one level */
    int cur_src;                    /* des_place_keys index being read      */
    int cur_layer;                  /* entry index inside that key's list   */
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

/* PR-93: one static image entry. The census over all 13 level blocks found
 * this grammar in the placement files themselves: exactly three commas,
 * "<LEAF>,<name>.seq,<x>,<y>" - the SAME first field as a five-field entry
 * (the ANIMACJE leaf table), but instead of naming a .MOV script the line
 * names the single .SEQ to draw there. Measured joins: 69 of the 84 shipped
 * lines resolve to <level>/<LEAF>/<name>.SEQ on the CD install (the 15
 * misses are all LONA.05 WODOSPAD2, named in the report); ZARIA.01 resolves
 * 14 of 14, and those 14 are the visible scene: the stone shelves the
 * objects stand on (plan4.seq at 70,480 under the balloon at 78,460,
 * plan3.seq at 180,515 under the ladder at 180,510, plan6.seq at 287,430
 * under the vulture at 300,450), the leaves and the nest. The lines carry
 * no class hint, no properties of their own, no motion: the element to draw
 * is the first one of that table that carries patch art (obj_elem's no-
 * script fallback), at the authored x,y through the element's own anchor -
 * the same draw rule every other placement uses (PR-49). Whether the
 * original animated some of these tables stays an open question; drawing
 * frame one is the absence of motion data, not a guess at motion. */
static obj_script *obj_static_get(place_ctx *pc, const char *leaf,
                                  const char *seqname)
{
    scene_owner *own = pc->own;
    obj_stats *st = &pc->st;
    obj_script *sc = obj_script_find(own, leaf, seqname);
    if (sc)
        return sc;
    sc = obj_script_new();
    if (!sc)
        return NULL;
    snprintf(sc->leaf, sizeof sc->leaf, "%s", leaf);
    snprintf(sc->movname, sizeof sc->movname, "%s", seqname);
    /* no .MOV: the line IS the whole script */
    sc->mov = NULL;
    sc->movtxt = NULL;
    obj_art *a = &sc->art[0];
    snprintf(a->name, sizeof a->name, "%s", seqname);
    llrs_slot sl;
    long asz = -1;
    if (leaf_record(pc->ac.cd, pc->ac.cd_blk, leaf, seqname, &sl))
        asz = llrs_read(pc->ac.cd, &sl, &a->blob);
    if (asz >= 0 && ll_seq_parse(a->blob, (uint32_t)asz, &a->seq) == 0) {
        a->found = 1;
        snprintf(a->name, sizeof a->name, "%s", sl.name);
        seq_geom(a);
        if (a->eidx0 < 0) {
            st->static_shapeonly++;
            printf("ll_level: static %s/%s: the record is there but carries"
                   " the SHAPE margins only, no patch art: an invisible"
                   " solid, drawn as nothing by the original game too (not"
                   " a data hole)\n", leaf, seqname);
        }
    } else {
        free(a->blob);
        a->blob = NULL;
        a->seq.count = 0;
        st->static_missing++;
        printf("ll_level: static %s/%s: no such record (or no parsable .SEQ"
               " in it) in that CD ANIMACJE leaf (data hole: the placement"
               " asks for this image, the install does not carry it) - it"
               " draws nothing\n", leaf, seqname);
    }
    sc->nart = 1;
    sc->ast[0] = 0;
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
    o->damage = o->b0 = o->b1 = o->scope = o->energy = o->hp = -1;
    o->hit_cd = 0;
    o->atc = 0;
    o->atc_hit = 0;
    o->dead = 0;
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
    int last_obj = -1;                  /* the placed object the following
                                         * key=value lines belong to */
    int after_prop = 0;                 /* PR-95: 1 while inside last_obj's
                                          * property block (the damage=/bounds=
                                          * lines that bind to it). A placement
                                          * line (five-field OR three-comma
                                          * static) that appears THERE is that
                                          * object's CHILD: its coordinates are
                                          * an offset from the parent. The vulture's
                                          * POCISK/jajo.mov,0,0 egg and the rhino's
                                          * PICT,panc.seq,20,29 mat both sit in the
                                          * property block of the object before
                                          * them. A top-level static (the ANI*.DES
                                          * shelves, or a WODOSPAD waterfall listed
                                          * as a SIBLING right after an ANIBACK
                                          * entry) has no property block before it:
                                          * after_prop is 0 there, so it keeps its
                                          * absolute world coordinates. */
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
            if (strchr(line, '=')) {
                st->prop++;                     /* damage= / energy= / ... */
                if (last_obj >= 0 && last_obj < pc->own->nobj) {
                    lvl_obj *lo = &pc->own->objs[last_obj];
                    after_prop = 1;             /* PR-95: we are inside this
                                                  * object's property block; a
                                                  * placement line that follows
                                                  * is its child (see above) */
                    int v0, v1;
                    if (sscanf(line, " damage = %d", &v0) == 1)
                        lo->damage = v0;
                    else if (sscanf(line, " bounds = %d , %d", &v0, &v1) == 2) {
                        lo->b0 = v0;
                        lo->b1 = v1;
                    } else if (sscanf(line, " scope = %d", &v0) == 1)
                        lo->scope = v0;
                    else if (sscanf(line, " energy = %d", &v0) == 1) {
                        lo->energy = v0;
                        /* PR-89: the spawn energy of the combat counter
                         * (engine field [obj+0xad]). The two-value
                         * authoring ("energy=20,50" once in ZARIA.01,
                         * "1000000,350" once in the census) is read as
                         * the FIRST value by the same sscanf the engine
                         * uses (" energy = %d" - its format string is at
                         * 0x90d9b); which value the engine really keeps
                         * at spawn stays an open question. */
                        lo->hp = v0;
                    }
                }
            }
            else {
                char sleaf[16], sfname[24];
                int sx = 0, sy = 0;
                const char *eos2 = line + ll;
                const char *q1 = place_field(line, eos2, sleaf,
                                             sizeof sleaf);
                const char *q2 = q1 ? place_field(q1, eos2, sfname,
                                                  sizeof sfname) : NULL;
                const char *qx = q2 ? place_int(q2, eos2, &sx) : NULL;
                const char *qc = qx ? place_comma(qx, eos2) : NULL;
                const char *qy = qc ? place_int(qc, eos2, &sy) : NULL;
                size_t fnl = strlen(sfname);
                if (qy && qy == eos2 && fnl > 4 &&
                    ci_eq(sfname + fnl - 4, ".SEQ")) {
                    /* the PR-93 static image line (grammar and joins in
                     * obj_static_get). Counted apart from the entry lines:
                     * this grammar has no script and no properties. */
                    st->statics++;
                    /* PR-95: a static line that directly follows a placed
                     * entry (or one of that entry's property lines, which do
                     * not clear last_obj) is that object's CHILD: its two
                     * numbers are an OFFSET from the parent, not a world
                     * position. This is the SAME child position in which the
                     * five-field 0,0 lines are authored (ZARIA.01 ZWIERZ1:
                     * the vulture's `POCISK,jajo.ani,jajo.mov,0,0` egg one
                     * entry up); here it is the rhino's pancake mat
                     * `NOSOROG,...,1700,600 ... energy=60` then
                     * `PICT,panc.seq,20,29` - the mat belongs under the
                     * rhino, not at the world corner (20,29), which is why
                     * the rhino "stood on nothing" (the exact case PR-93
                     * left open in its own record). A top-level static (last
                     * obj cleared: the ANI*.DES shelf lines, which follow the
                     * file's count line or another static, never an entry)
                     * keeps its absolute authored x,y. */
                    int px = sx, py = sy, child = 0;
                    if (last_obj >= 0 && after_prop &&
                        last_obj < pc->own->nobj &&
                        (pc->own->objs[last_obj].b0 >= 0 ||
                         pc->own->objs[last_obj].scope >= 0)) {
                        /* PR-95: the image is authored inside a bounded
                                          * patrol actor's property block (its
                                          * parent has bounds= / scope= - the
                                          * enemy-placement grammar, HANDOFF §
                                          * "Placement files"): it is the
                                          * actor's own attached prop, drawn
                                          * at the parent + this offset. The
                                          * rhino's pancake mat and the
                                          * vulture's egg are these. A static
                                          * that merely follows some OTHER
                                          * object's property line with that
                                          * object not a bounded actor (TUNGA
                                          * `XCHG ... xchg=5,2` then a
                                          * standalone `TABLICA` sign) is NOT
                                          * an attached child: it keeps its
                                          * absolute authored position. */
                        px = pc->own->objs[last_obj].x + sx;
                        py = pc->own->objs[last_obj].y + sy;
                        child = 1;
                    }
                    if (px < 0 || px >= pc->world_w) {
                        st->static_out++;   /* same rule as an entry's
                                             * coord_bad: outside the level's
                                             * own world, not placed */
                        goto next;
                    }
                    obj_script *sc2 = obj_static_get(pc, sleaf, sfname);
                    if (!sc2)
                        goto next;
                    lvl_obj *so = obj_slot(pc);
                    if (!so)
                        goto next;
                    so->sc = sc2;
                    so->static_img = 1;
                    so->src = (unsigned char)pc->cur_src;
                    so->layer = child
                                    ? pc->own->objs[last_obj].layer
                                    : (unsigned char)pc->cur_layer;
                    so->sti = 0;
                    so->face = 1;
                    so->bx = so->x = px;
                    so->by = so->y = py;
                    if (child) {
                        st->static_attached++;
                        if (st->static_attached <= 4)
                            printf("ll_level: placement %s: static %s/%s"
                                   " follows %s right after it: an image the"
                                   " engine draws attached to that object at"
                                   " the offset (%d,%d), placed at the parent"
                                   " (%d,%d) + offset = (%d,%d), not at the"
                                   " world corner\n", fname, sleaf, sfname,
                                   pc->own->objs[last_obj].sc ?
                                       pc->own->objs[last_obj].sc->leaf
                                       : "?", sx, sy,
                                   pc->own->objs[last_obj].x,
                                   pc->own->objs[last_obj].y, px, py);
                    }
                    last_obj = -1;    /* neither a top-level static nor an
                                       * attached one carries properties; a
                                       * following property line binds to
                                       * nothing and is only counted */
                    after_prop = 0;
                    st->statics_placed++;
                }
                else if (strchr(line, ';'))
                    st->poly++;             /* polyline data (ground) */
                else
                    st->group++;
            }
        } else {
            char leaf[16], cls[24], mov[24];
            int x = 0, y = 0;
            last_obj = -1;
            after_prop = 0;             /* a new entry line is not inside any
                                          * object's property block */
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
                if (x == 0 && y == 0) {
                    /* PR-93: an entry authored at exactly 0,0. Measured
                     * over all 13 level blocks: 130 such lines, and every
                     * one of them directly follows another entry or one of
                     * its property lines - the vulture's carried egg
                     * (ZARIA.01 `SEP ... POCISK,jajo.ani,jajo.mov,0,0`),
                     * the saw-fish on each authored RBLOK log (ENA.03), a
                     * drop on each leaf (KROPLA->KROPLAS), the seaweed
                     * segments, the volcano's smoke from each crater, the
                     * demo texts. Not one of them is a placement at the
                     * world origin; they are the children the engine
                     * spawns FROM their parent when some event fires, and
                     * which event is unresolved. The script and its art
                     * stay joined and counted above (the data authors
                     * them); what is not done is instantiating a world
                     * object at (0,0), which is what drew those fruits and
                     * texts in the top-left corner. */
                    st->attached++;
                    if (st->attached <= 4)
                        printf("ll_level: placement %s: %s/%s is authored at"
                               " 0,0 right after another entry: a child the"
                               " engine spawns from its parent, not a"
                               " placement at the world origin (the spawn"
                               " trigger is unresolved) - joined and"
                               " counted, not instantiated\n",
                               fname, leaf, mov);
                    goto next;
                }
                sc->ninst++;
                lvl_obj *o = obj_slot(pc);
                if (!o)
                    goto next;
                o->sc = sc;
                o->src = (unsigned char)pc->cur_src;
                o->layer = (unsigned char)pc->cur_layer;
                o->sti = sc->mov ? sc->mov->start_st : 0;   /* declared start */
                o->face = 1;     /* PR-81: base facing = the authored draw
                                  * (the frame sign already encodes it) */
                o->bx = o->x = x;      /* authored position, y down: no
                                        * invented bottom-anchor. Only the
                                        * script's own " M dx,dy" frames move
                                        * it (285 of them game-wide). */
                o->by = o->y = y;
                last_obj = pc->own->nobj - 1;
                after_prop = 0;         /* this entry is a placement, not a
                                          * property block; a property line
                                          * that follows re-arms after_prop */
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

/* PR-93: the placement grammar legs. Lines are the shipped shapes, verbatim
 * in structure (ZARIA.01: `SEP,dummy,sep.mov,300,450` + its
 * `POCISK,jajo.ani,jajo.mov,0,0` child + `PICT,plan4.seq,70,480` /
 * `PICT,plan6.seq,287,430`; ZARIA.02 ZWIERZ2: the KAMIEN extra-parameter
 * line; the GRABG 3-comma non-entry): with no containers attached every
 * .MOV and .SEQ is a data hole, so what is asserted is exactly the
 * classification and the placement: entries, the 0,0 child joined but NOT
 * instantiated (no world object at 0,0 - that was the fruit in the
 * corner), static images placed at their authored positions, an x outside
 * the level's own world not placed, the 6-parameter KAMIEN line still an
 * ordinary entry, the GRABG line still only a group header. */
static int place_grammar_selftest(void)
{
    static const char txt[] =
        "5\n"
        "SEP,dummy,sep.mov,300,450\n"
        "damage=10\n"
        "energy=40\n"
        "POCISK,jajo.ani,jajo.mov,0,0\n"
        "PICT,plan4.seq,70,480\n"
        "PICT,nothere.seq,10,20\n"
        "PICT,outsidex.seq,9000,10\n"
        "GRABG,2113,350,kilof.grp\n"
        "KAMIEN,dummy,kamien1.mov,160,250,1308\n"
        /* PR-95: the rhino's mat. NOSOROG is a bounded patrol actor (its
         * bounds= gives b0>=0), so the 3-comma static inside its property
         * block is its child: the image is placed at the parent (700,600)
         * plus the authored offset (20,29) = (720,629), NOT at the literal
         * (20,29) world corner. This is the ZARIA.01 ZWIERZ1 shape. */
        "NOSOROG,dummy,nos.mov,700,600\n"
        "bounds=680,750\n"
        "scope=40\n"
        "PICT,panc.seq,20,29\n"
        /* PR-95 negative: a static that only follows a NON-bounded entry's
         * property line (XCHG has a switch property, no bounds/scope) is
         * NOT a child - it keeps its absolute authored (400,300), not the
         * parent+offset (150+400,535+300). The TUNGA ANI2 TABLICA/jajotab
         * after XCHG shape, scaled into this 1000-px fixture world. */
        "XCHG,dummy,malpa1.mov,150,535\n"
        "xchg=5,2\n"
        "TABLICA,jajotab.seq,400,300\n";
    place_ctx pc;
    scene_owner own;
    memset(&pc, 0, sizeof pc);
    memset(&own, 0, sizeof own);
    pc.own = &own;
    pc.level = "PR93.TEST";
    pc.world_w = 1000;
    des_place_file(&pc, "PR93.DES", txt, sizeof txt - 1);
    const obj_stats *st = &pc.st;
    int nstatic = 0, natzero = 0;
    for (int k = 0; k < own.nobj; k++) {
        if (own.objs[k].static_img)
            nstatic++;
        if (own.objs[k].x == 0 && own.objs[k].y == 0)
            natzero++;
    }
    int fail = 0;
    struct { const char *what; int got, want; } chk[] = {
        { "entry lines", st->entries, 5 },        /* SEP, its child, KAMIEN,
                                                   * the rhino, XCHG */
        { "entries placed", st->placed, 4 },      /* the 0,0 child is NOT one */
        { "attached children", st->attached, 1 },
        { "class hints", st->class_hint, 1 },
        { "property lines", st->prop, 5 },        /* damage/energy + the
                                                   * rhino's bounds/scope +
                                                   * XCHG's switch property */
        { "static lines", st->statics, 5 },
        { "statics placed", st->statics_placed, 4 },
        { "statics outside the world", st->static_out, 1 },
        { "static art holes", st->static_missing, 4 },
        { "attached statics", st->static_attached, 1 }, /* only the rhino's mat */
        { "group lines", st->group, 1 },          /* GRABG: not a static */
        { "objects total", own.nobj, 8 },         /* 4 entries + 4 statics */
        { "static instances", nstatic, 4 },
        { "instances at 0,0", natzero, 0 },
        { "param-extra entries", st->param_extra, 1 },
    };
    for (unsigned k = 0; k < sizeof chk / sizeof chk[0]; k++)
        if (chk[k].got != chk[k].want) {
            fprintf(stderr, "mov selftest: FAILED placement grammar: %s"
                    " counted %d, expected %d\n",
                    chk[k].what, chk[k].got, chk[k].want);
            fail = 1;
        }
    /* the static at 70,480 stands there with its art unresolved; the
     * child of SEP does not stand anywhere of its own, and its parent's
     * energy= property still bound the PARENT (an attached child never
     * becomes the property target either: last_obj was reset) */
    if (own.nobj >= 1 && own.objs[0].hp != 40) {
        fprintf(stderr, "mov selftest: FAILED placement grammar: the SEP"
                " entry's energy= property did not reach it (hp %d)\n",
                own.objs[0].hp);
        fail = 1;
    }
    if (own.objs[0].x != 300 || own.objs[0].y != 450) {
        fprintf(stderr, "mov selftest: FAILED placement grammar: the SEP"
                " entry moved (%d,%d)\n", own.objs[0].x, own.objs[0].y);
        fail = 1;
    }
    int found70 = 0;
    for (int k = 0; k < own.nobj; k++)
        if (own.objs[k].static_img && own.objs[k].x == 70 &&
            own.objs[k].y == 480 && own.objs[k].static_img)
            found70 = 1;
    if (!found70) {
        fprintf(stderr, "mov selftest: FAILED placement grammar: no static"
                " image stands at the authored (70,480)\n");
        fail = 1;
    }
    /* PR-95: the rhino's mat is attached - placed at its parent (700,600)
     * plus the authored offset (20,29), and never at the literal corner.
     * The XCHG case is the negative: its following static keeps its
     * absolute authored position, it is not pulled to parent+offset. */
    int found_panc = 0, found_corner = 0, found_abs = 0;
    for (int k = 0; k < own.nobj; k++) {
        if (!own.objs[k].static_img)
            continue;
        if (own.objs[k].x == 720 && own.objs[k].y == 629)
            found_panc = 1;
        if (own.objs[k].x == 20 && own.objs[k].y == 29)
            found_corner = 1;
        if (own.objs[k].x == 400 && own.objs[k].y == 300)
            found_abs = 1;
    }
    if (!found_panc || found_corner) {
        fprintf(stderr, "mov selftest: FAILED placement grammar: the rhino's"
                " attached static is not at parent+offset (720,629)%s\n",
                found_corner ? " or it also stands at the (20,29) corner" : "");
        fail = 1;
    }
    if (!found_abs) {
        fprintf(stderr, "mov selftest: FAILED placement grammar: the static"
                " after a non-bounded entry moved off its absolute (400,300)"
                " (it is not that entry's child)\n");
        fail = 1;
    }
    scene_owner_free(&own);
    return fail;
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
        pc->cur_src = (int)kk;
        if (!raw[0])
            continue;      /* key absent: the author says nothing about it
                            * (no shipped level omits these three keys) */
        char list[512];
        snprintf(list, sizeof list, "%s", raw);
        char *save = NULL;
        int tix = -1;
        for (char *tok = strtok_r(list, ",", &save); tok;
             tok = strtok_r(NULL, ",", &save)) {
            while (*tok == ' ')
                tok++;
            if (!tok[0])
                continue;
            pc->cur_layer = ++tix;
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
    if (st->statics)
        printf("ll_level: authored scene %s: %d static image line(s) (the"
               " 3-comma \"<LEAF>,<name>.seq,<x>,<y>\" grammar: the line"
               " names its own .SEQ in that CD ANIMACJE leaf, no script)"
               " -> %d placed, %d at an x outside the level's own world,"
               " %d whose .SEQ is not on these containers (data holes"
               " listed above), %d carry shape only (drawn as nothing), %d"
               " authored right after an object and drawn attached to it at"
               " the offset (a mat under the object the line follows: an"
               " offset from that object, not a world position)\n",
               pc->level, st->statics, st->statics_placed, st->static_out,
               st->static_missing, st->static_shapeonly,
               st->static_attached);
    if (st->prop || st->group || st->comment || st->class_hint ||
        st->poly || st->shortline || st->param_extra || st->field_space ||
        st->entry_noext || st->attached || st->longline)
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
               " above), %d entry line(s) authored at 0,0: all of them"
               " follow another entry or its properties - children the"
               " engine spawns from their parent (its trigger is"
               " unresolved), never a placement at the world origin (listed"
               " above), %d line(s) this reader cannot classify\n",
               st->prop, st->group, st->comment, st->class_hint, st->poly,
               st->param_extra, st->field_space, st->entry_noext,
               st->attached, st->shortline + st->longline);
    if (st->nscript)
        printf("ll_level: those scripts author %d state transition(s), %d"
               " control flag(s) with no model bit, %d frame"
               " directive(s) the model does not act on (the engine's own"
               " table 0x917c0 minus the modeled GEN (PR-88) and ATC_ON/"
               " ATC_OFF (PR-89): WAIT / lock / ground_on / chdir / pos /"
               " goto / F / FC / M / MOVE / Wait / end_level / smp*), %d"
               " sound-or-scale field(s); %d state(s)"
               " name no seq= art, %d state(s) / %d frame(s) / %d"
               " transition(s) over cap - all parsed and counted, acted on"
               " only once the rest of the AC_*/K_* signal bus lands"
               " (named-signal bits landed in PR-88; the cross-object"
               " routing and the engine-internal raisers have not)\n",
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

/* the scene: the 320x200 world window shown 2x (nearest neighbour) */
static void paint2x(const unsigned char *canvas, const uint32_t lut[256],
                    int w, int h)
{
    size_t need = (size_t)w * 2 * (size_t)h * 2;
    if (s_paint_n < need) {
        uint32_t *nb = realloc(s_paint_buf, need * sizeof *nb);
        if (!nb)
            return;
        s_paint_buf = nb;
        s_paint_n = need;
    }
    for (int y = 0; y < h; y++) {
        uint32_t *r0 = s_paint_buf + (size_t)y * 2 * (size_t)w * 2;
        const unsigned char *sp = canvas + (size_t)y * w;
        for (int x = 0; x < w; x++)
            r0[2 * x] = r0[2 * x + 1] = lut[sp[x]];
        memcpy(r0 + (size_t)w * 2, r0, (size_t)w * 2 * sizeof *r0);
    }
    plat_present_argb(s_paint_buf, w * 2, h * 2);
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
    int pcm_tix[MAX_PCM_LAYERS];    /* each one's index in the FULL tlo list */
    int ntlo = 0;                   /* tlo tokens, present or not */
    int npcm_rec = 0;
    const llrs_slot *bg = NULL;
    int bgpref = -1;                /* block index of first present tlo layer */
    int bg_tix = 0;                 /* tlo index of the backdrop .PCX         */
    {
        char tlo[512];
        snprintf(tlo, sizeof tlo, "%s", des_get(kvs, nk, "tlo"));
        char *save = NULL;
        for (char *tok = strtok_r(tlo, ",", &save); tok;
             tok = strtok_r(NULL, ",", &save)) {
            while (*tok == ' ')
                tok++;
            const int tix = ntlo++;
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
                    if (!bg && ci_eq(dot, ".PCX")) {
                        bg = &recs[k];
                        bg_tix = tix;
                    }
                    if (ci_eq(dot, ".PCM") && npcm_rec < MAX_PCM_LAYERS) {
                        int dup = 0;
                        for (int q = 0; q < npcm_rec; q++)
                            dup |= pcm_rec[q] == (int)k;
                        if (!dup) {
                            pcm_tix[npcm_rec] = tix;
                            pcm_rec[npcm_rec++] = (int)k;
                        }
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
    int lay_tix[MAX_PCM_LAYERS];
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
        lay_tix[nlay] = pcm_tix[q];
        lay[nlay++] = p2;
    }

    par_t par;
    par_parse(&par, des_get(kvs, nk, "speed"), des_get(kvs, nk, "main_back"),
              ntlo);
    printf("ll_level: danger_power=%s (the damage a DANGER object deals, the\n"
           "ll_level:   game keeps it in level+0x54)\n",
           des_get(kvs, nk, "danger_power")[0] ? des_get(kvs, nk, "danger_power")
                                              : "(not authored)");
    printf("ll_level: parallax: %d tlo layers, reference %d, speed%%", par.n,
           par.ref);
    for (int q = 0; q < par.n; q++)
        printf(" %d,%d", par.sx[q], par.sy[q]);
    printf("\n");
    if (world_w == 0)
        for (int k = 0; k < nlay; k++)
            if (lay_tix[k] == par.ref) {
                /* PR-65: no back_rect (SADES, TUNGA2, ZARIA.01/.02: the CD
                 * hole). The engine's camera is clamped to the reference
                 * layer main_back (FUN_00057590 reads its extent), and the
                 * reference layer's own art is the world: ZARIA.01's PLAN1
                 * is 2240x700 - the same size KADMAR.07 authors as back_rect
                 * - and a playthrough video scrolls over such a world. */
                world_w = (int)lay[k].w;
                world_h = (int)lay[k].h;
                printf("ll_level: no back_rect: world %dx%d taken from the"
                       " reference layer (main_back %d)\n", world_w, world_h,
                       par.ref);
            }
    /* PR-66: the game's view is 320x200 world units (0x140/0xc8 in the camera
     * clamp FUN_00057590 and the layer offsets), shown 2x as 640x400 - a
     * playthrough video shows exactly that (art ~2x the size of the native
     * sprites, a 640x400 picture inside the 640x480 mode). The scene canvas
     * is the 320x200 world window; the window magnifies it. */
    int cw = LVL_VIEW_W, ch = LVL_VIEW_H;
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
    pl.layer = par.ref;                    /* the lion lives on main_back  */
    {   /* PR-71: the contact pass is ON for every level but RAN.01 (the
         * balloon-flight level: its walking start is not what the game
         * plays there); LEO_PLAY_SOLID=0/1 forces it either way */
        const char *sv = getenv("LEO_PLAY_SOLID");
        pl.solid = (sv && *sv) ? *sv != '0' : !ci_eq(level, "RAN.01");
        /* PR-90: the DANGER + class MAIN box arming stand-in is a MODEL
         * (the pinned box applied to DANGER-flagged states), not a
         * recovered driver (the engine's real per-class AI is open, see
         * obj_arm_atak) - OFF unless LEO_PLAY_DANGER=1 asks for it. */
        pl.danger = getenv("LEO_PLAY_DANGER")
                        ? atoi(getenv("LEO_PLAY_DANGER")) != 0 : 0;
    }
    pl.cam_ox = CAM_LEAD_R;
    /* PR-84: the follow-cam-y state (play_ctx camy_v/camy_set) starts
     * empty: follow_camy snaps the view to the measured feet anchor at
     * its first call. LEO_LEVEL_CAMY still seeds the PREVIEW camera and
     * is overridden in play mode, where the game's own camera rule runs. */
    pl.energy = 100;                       /* 0x503ac: energy 100 at (re)start */
    pl.lives = 6;                          /* the playthrough video's start    */
    hud_t hud;
    memset(&hud, 0, sizeof hud);
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
    hud_load(&hud, have_hd_ety ? &hd_ety : NULL, hblk2, &etapy, recs, nrec,
             des_get(kvs, nk, "belka"));
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
            {
                int moved = 0;
                for (int k = 0; k < nobj; k++)
                    moved += obj_settle(&objs[k], &pl.plt);
                printf("ll_level: play: %d ground-bound placement(s) settled"
                       " onto the .PLT (CHECK_LN start state)\n", moved);
            }
            printf("ll_level: play: %s %d state(s) on %d polyline(s):"
                   " spawn feet (%d,%d) grounded=%d bounds %d..%d"                   /* lew_rect */
                   " death below %d\n", pl.leaf, pl.mov->nst, pl.plt.n,
                   (int)pl.p.x, (int)pl.p.y, pl.p.grounded,
                   (int)pl.xmin, (int)pl.xmax, (int)pl.ydead);
            {   /* PR-60 census: placements whose script has a SOLID state,
                 * by the key that placed them (zwierzaki/animacje/ani_plat) */
                int cnt[3][3] = { { 0 } };
                for (int k = 0; k < nobj; k++) {
                    const obj_script *sc = objs[k].sc;
                    int has = 0;
                    if (!sc || !sc->mov)
                        continue;
                    for (int q = 0; q < sc->mov->nst; q++) {
                        unsigned cc = sc->mov->st[q].ctrl;
                        if (cc & LL_CTRL_SOLID2)
                            has = has < 2 ? 2 : has;
                        else if (cc & LL_CTRL_SEMI_SOLID)
                            has = has < 3 ? 3 : has;
                        else if (cc & LL_CTRL_SOLID)
                            has = has < 1 ? 1 : has;
                    }
                    if (has)
                        cnt[objs[k].src < 3 ? objs[k].src : 0][has - 1]++;
                }
                printf("ll_level: play: placements with a SOLID state"
                       " (SOLID/SOLID2/SEMI): zwierzaki %d/%d/%d animacje"
                       " %d/%d/%d ani_plat %d/%d/%d\n", cnt[0][0], cnt[0][1],
                       cnt[0][2], cnt[1][0], cnt[1][1], cnt[1][2],
                       cnt[2][0], cnt[2][1], cnt[2][2]);
            }
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

    int nstatic = 0;
    for (int k = 0; k < nobj; k++)
        if (objs[k].static_img)
            nstatic++;
    if (nstatic)
        printf("ll_level: show %s: bg %s %dx%d, %d animated object(s), %d"
               " static image(s)\n", level,
               bgname, img.idx ? (int)img.w : cw, img.idx ? (int)img.h : ch,
               nobj - nstatic, nstatic);
    else
        printf("ll_level: show %s: bg %s %dx%d, %d animated object(s)\n",
               level,
               bgname, img.idx ? (int)img.w : cw, img.idx ? (int)img.h : ch,
               nobj);
    printf("ll_level: view %dx%d world units, shown 2x as %dx%d\n", cw, ch,
           cw * 2, ch * 2);
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
               objs[k].static_img ? "[static image]"
               : sc->mov && sc->mov->nst ? "[mov]"
               : "[no script: data hole]",
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
     * views 200..600 px tall) and was never on screen. Since PR-84 the
     * play-mode camera runs the measured game rule (feet-at-178 grounded
     * anchor, hold while airborne); LEO_LEVEL_CAMY seeds the PREVIEW only. */
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
    paint_base(canvas, base, img.idx ? (int)img.w : cw,
               img.idx ? (int)img.h : ch, cw, ch, img.idx != NULL, &par,
               bg_tix, cam, camy, world_h);
    layers_at(lay, lay_tix, nlay, &par, cam, camy, cw, ch, world_w,
                  world_h, canvas);
    objs_draw(objs, nobj, canvas, cw, ch, cam, camy, &par, lay, lay_tix,
              nlay, world_h);

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
                play_advance(&pl, &keys, 20, 1, tk, objs, nobj);
                for (int k = 0; k < nobj; k++)
                    obj_advance(&objs[k], 20);
            }
            camy = follow_camy(&pl, ch, cam_ymax);
        } else {
            int shown = 0;
            for (int k = 0; k < nobj; k++) { /* advance fixed-step then redraw */
                for (int m = 0; m < ticks; m++)
                    obj_advance(&objs[k], 40);
                /* PR-93: static images are advanced (inert: no script) but
                 * never take one of the 12 proof slots: a static has no
                 * state or frame to prove; the proof belongs to the
                 * animated cycle, exactly as before PR-93. */
                if (shown < 12 && !objs[k].static_img) {  /* proof of the cycle, bounded */
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
        paint_base(canvas, base, img.idx ? (int)img.w : cw,
                   img.idx ? (int)img.h : ch, cw, ch, img.idx != NULL, &par,
                   bg_tix, cam, camy, world_h);               /* same order as the live loop */
        layers_at(lay, lay_tix, nlay, &par, cam, camy, cw, ch, world_w,
                  world_h, canvas);
        objs_draw(objs, nobj, canvas, cw, ch, cam, camy, &par, lay, lay_tix,
                  nlay, world_h);
        play_draw(&pl, canvas, cw, ch, cam, camy);
        if (pl.have)
            hud_draw(&hud, canvas, cw, ch, pl.energy, pl.lives);
        scene_ppm(ppm, cw, ch, canvas, scenepal);
        goto done;                          /* canvas freed at done: (E-05) */
    }

    {
        /* T-01/T-12: the platform owns the window (logical viewport = the
         * scene view size cw x ch; resizable, HiDPI-ready, F11/Alt+Enter
         * desktop-fullscreen handled by the backend). */
        if (plat_init("Lew Leon - level scene", cw * 2, ch * 2) < 0) {
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
                          e.key == LLKEY_UP   || e.key == LLKEY_DOWN ||
                          e.key == ' '        || e.key == LLKEY_LCTRL ||
                          e.key == LLKEY_LALT ||
                          (e.key >= LLKEY_F1 && e.key <= LLKEY_F4))) {
                    /* M4-B: in play mode the arrows drive the LION
                     * (held-key tracking, KEYUP releases); the camera
                     * follows the player instead (below the loop).
                     * PR-87 (viewer choice, names from the walkthrough):
                     * Ctrl = fire (attack), F1..F4 = the weapon-selector
                     * keys the comma groups gate with, Alt = K_ALT. */
                    int on = e.type == LL_EV_KEYDOWN;
                    if (e.key == LLKEY_LEFT)
                        pl.keys.k_bkwd = on;
                    else if (e.key == LLKEY_RIGHT)
                        pl.keys.k_fwd = on;
                    else if (e.key == LLKEY_UP)
                        pl.keys.k_up = on;
                    else if (e.key == ' ')            /* viewer choice: Space */
                        pl.keys.k_jump = on;
                    else if (e.key == LLKEY_LCTRL)    /* PR-87: attack */
                        pl.keys.k_fire = on;
                    else if (e.key == LLKEY_LALT)
                        pl.keys.k_alt = on;
                    else if (e.key >= LLKEY_F1 && e.key <= LLKEY_F4) {
                        int *kf = e.key == LLKEY_F1 ? &pl.keys.k_f1 :
                                  e.key == LLKEY_F2 ? &pl.keys.k_f2 :
                                  e.key == LLKEY_F3 ? &pl.keys.k_f3 :
                                                      &pl.keys.k_f4;
                        *kf = on;
                    }
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
                    play_advance(&pl, &pl.keys, st2, 0, 0, objs, nobj);
                    used += st2;
                }
                cam = follow_camx(&pl, cw, cam_max);  /* camera follows */
                camy = follow_camy(&pl, ch, cam_ymax);
            }
            paint_base(canvas, base, img.idx ? (int)img.w : cw,
                       img.idx ? (int)img.h : ch, cw, ch, img.idx != NULL,
                       &par, bg_tix, cam, camy, world_h);
            layers_at(lay, lay_tix, nlay, &par, cam, camy, cw, ch, world_w,
                  world_h, canvas);
            objs_draw(objs, nobj, canvas, cw, ch, cam, camy, &par, lay, lay_tix,
                      nlay, world_h);
            play_draw(&pl, canvas, cw, ch, cam, camy);
            if (pl.have)
                hud_draw(&hud, canvas, cw, ch, pl.energy, pl.lives);
            paint2x(canvas, lut, cw, ch);
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
    hud_free(&hud);
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
