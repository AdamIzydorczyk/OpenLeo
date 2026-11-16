/* is_stream.c - portable C port of the verified .IS codec (tools/is_decode.py).
 * Opcode semantics: see PLAN.md S3 and the python docstring; DO NOT "fix"
 * deviations from the python reference without re-verifying against the
 * live trace, this file's byte-exactness is the M2 deliverable.
 */
#include "is_stream.h"
#include "fs_ci.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------ load */

static uint16_t rd16(const uint8_t *p) { return (uint16_t)(p[0] | p[1] << 8); }
static uint32_t rd32(const uint8_t *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 |
           (uint32_t)p[3] << 24;
}

int is_open(is_stream *s, const char *path)
{
    FILE *f = ll_fopen_ci(path, "rb");   /* case-insensitive + UTF-8 path
                                          * fallback (fs_ci.h, P-01/P-07) */
    if (!f)
        return -1;
    if (ll_fseek64(f, 0, SEEK_END) != 0) {
        fclose(f);
        return -1;
    }
    long long n = ll_ftell64(f);
    ll_fseek64(f, 0, SEEK_SET);
    if (n < (long long)IS_CHUNK_BASE + 7
        || (unsigned long long)n > (unsigned long long)SIZE_MAX) {
        fclose(f);
        return -1;
    }
    uint8_t *img = (uint8_t *)malloc((size_t)n);
    if (!img || fread(img, 1, (size_t)n, f) != (size_t)n) {
        free(img);
        fclose(f);
        return -1;
    }
    fclose(f);

    memset(s, 0, sizeof(*s));
    s->buf.data = img;
    s->buf.size = (size_t)n;
    s->w = rd16(img + IS_HDR_W);
    s->h = rd16(img + IS_HDR_H);
    s->fps = img[IS_HDR_FPS];
    if (s->w <= 0 || s->h <= 0 || (s->h % 4)) {
        free(img);
        return -1;
    }
    s->modex = s->w == 320;

    /* chunk walk identical to python: [BIH] type/len/tail at p */
    unsigned cap = 64, cnt = 0;
    struct chunk *v = (struct chunk *)malloc(cap * sizeof *v);
    if (!v) {
        free(img);
        return -1;
    }
    size_t p = IS_CHUNK_BASE;
    while (p + 7 <= s->buf.size) {
        uint32_t ln = rd32(img + p + 1);
        uint16_t tail = rd16(img + p + 5);
        if (ln < 7 || p + ln > s->buf.size)
            break;
        if (cnt == cap) {
            cap *= 2;
            struct chunk *nv = (struct chunk *)realloc(v, cap * sizeof *v);
            if (!nv) {
                free(v);
                free(img);
                return -1;
            }
            v = nv;
        }
        v[cnt].pos = p + 7;
        v[cnt].len = ln - 7;
        v[cnt].tail = tail;
        cnt++;
        p += ln;
    }
    s->nchunks = cnt;
    s->chunks = v;
    s->fb = (uint8_t *)calloc((size_t)s->w * (size_t)s->h, 1);
    if (!s->fb) {
        free(v);
        free(img);
        return -1;
    }
    return 0;
}

void is_close(is_stream *s)
{
    free(s->chunks);
    s->chunks = NULL;
    free(s->fb);
    s->fb = NULL;
    free(s->buf.data);
    s->buf.data = NULL;
    s->nchunks = 0;
}

/* ------------------------------------------------------- band/frame decode */

static inline size_t seg_base(const is_stream *s, int band, int i)
{
    if (s->modex) {
        size_t q = (size_t)s->w * (size_t)s->h / 4;
        return (size_t)i * q + (size_t)band * (size_t)s->w;
    }
    return ((size_t)band * 4 + (size_t)i) * (size_t)s->w;
}

#define BOUNDS()                                                               \
    do {                                                                       \
        s->st.ok = -1;                                                         \
        return pos;                                                            \
    } while (0)

/* decode one full frame (h/4 bands) starting at stream pos; mirrors
 * decode_frame(): 640 path re-bases segment cursors per band, Mode X
 * keeps them persistent across bands. */
static size_t decode_frame(is_stream *s, size_t pos, size_t stop)
{
    const uint8_t *d = s->buf.data;
    const size_t fsz = (size_t)s->w * (size_t)s->h;
    uint8_t *fb = s->fb;
    size_t seg[4];
    int band = 0;
    const int nbands = s->h / 4;

    seg[0] = seg_base(s, 0, 0);
    seg[1] = seg_base(s, 0, 1);
    seg[2] = seg_base(s, 0, 2);
    seg[3] = seg_base(s, 0, 3);

    while (band < nbands) {
        if (!s->modex) {
            seg[0] = seg_base(s, band, 0);
            seg[1] = seg_base(s, band, 1);
            seg[2] = seg_base(s, band, 2);
            seg[3] = seg_base(s, band, 3);
        }
        int budget = s->w, eob_adv = 0;
        int saw_eob = 0;
        while (budget > 0) {
            if (pos >= stop)
                BOUNDS();
            uint8_t b = d[pos++];
            int ln = b & 7;
            if (b & 0x40) {
                if (pos >= stop)
                    BOUNDS();
                ln = (ln << 8) | d[pos++];
            }
            switch (b & 0xB8) {
            case IS_OP_SOLID: {
                if (pos >= stop)
                    BOUNDS();
                uint8_t val = d[pos++];
                for (int i = 0; i < 4; i++) {
                    if (seg[i] + (size_t)ln > fsz)
                        BOUNDS();
                    memset(fb + seg[i], val, (size_t)ln);
                    seg[i] += (size_t)ln;
                }
                budget -= ln;
                s->st.solid++;
                break;
            }
            case IS_OP_LUT1: {
                if (pos + 2 > stop)
                    BOUNDS();
                /* PR-55: the game indexes the 2-byte table BY THE BIT
                 * (FUN_0001a9fe: al = [table + bit]), so a SET bit selects
                 * the SECOND table byte. This port (and its python twin)
                 * had the two swapped, which colour-swapped every two-
                 * colour run: thin lines became thick black blocks. */
                uint8_t t0 = d[pos], t1 = d[pos + 1];
                pos += 2;
                int ndata = (ln + 1) >> 1;
                if (pos + (size_t)ndata > stop)
                    BOUNDS();
                for (int k = 0; k < (ln >> 1); k++) {
                    uint8_t c = d[pos++];
                    for (int i = 0; i < 4; i++) {
                        size_t p2 = seg[i];
                        if (p2 + 2 > fsz)
                            BOUNDS();
                        fb[p2] = (c >> (7 - i)) & 1 ? t1 : t0;
                        fb[p2 + 1] = (c >> (3 - i)) & 1 ? t1 : t0;
                        seg[i] = p2 + 2;
                    }
                }
                if (ln & 1) {
                    uint8_t c = d[pos++];
                    for (int i = 0; i < 4; i++) {
                        size_t p2 = seg[i];
                        if (p2 + 1 > fsz)
                            BOUNDS();
                        fb[p2] = (c >> (7 - i)) & 1 ? t1 : t0;
                        seg[i] = p2 + 1;
                    }
                }
                budget -= ln;
                s->st.lut1++;
                break;
            }
            case IS_OP_LUT2: {
                if (pos + 4 > stop)
                    BOUNDS();
                const uint8_t *t = d + pos;
                pos += 4;
                if (pos + (size_t)ln > stop)
                    BOUNDS();
                for (int k = 0; k < ln; k++) {
                    uint8_t c = d[pos++];
                    for (int i = 0; i < 4; i++) {
                        if (seg[i] >= fsz)
                            BOUNDS();
                        fb[seg[i]++] = t[(c >> (6 - 2 * i)) & 3];
                    }
                }
                budget -= ln;
                s->st.lut2++;
                break;
            }
            case IS_OP_LUT4: {
                if (pos + 16 > stop)
                    BOUNDS();
                const uint8_t *t = d + pos;
                pos += 16;
                if (pos + 2 * (size_t)ln > stop)
                    BOUNDS();
                for (int k = 0; k < ln; k++) {
                    uint8_t u = d[pos], v = d[pos + 1];
                    pos += 2;
                    const uint8_t px[4] = { t[u >> 4], t[u & 15], t[v >> 4],
                                           t[v & 15] };
                    for (int i = 0; i < 4; i++) {
                        if (seg[i] >= fsz)
                            BOUNDS();
                        fb[seg[i]++] = px[i];
                    }
                }
                budget -= ln;
                s->st.lut4++;
                break;
            }
            case IS_OP_RAW: {
                for (int i = 0; i < 4; i++) {
                    if (pos + (size_t)ln > stop || seg[i] + (size_t)ln > fsz)
                        BOUNDS();
                    memcpy(fb + seg[i], d + pos, (size_t)ln);
                    pos += (size_t)ln;
                    seg[i] += (size_t)ln;
                }
                budget -= ln;
                s->st.raw++;
                break;
            }
            case IS_OP_SKIP: {
                for (int i = 0; i < 4; i++) {
                    if (seg[i] + (size_t)ln > fsz)
                        BOUNDS();
                    seg[i] += (size_t)ln;
                }
                budget -= ln;
                s->st.skip++;
                break;
            }
            case IS_OP_EOB:
                eob_adv = ln;
                saw_eob = 1;
                s->st.eob++;
                /* PR-56: FUN_0001adb8/1acbb @0x1ad43: EOB sets the band
                 * advance to len AND adds len * band-stride (a8958) to all
                 * four write cursors. The 640 path re-bases its cursors
                 * from the band number so it never noticed; the planar
                 * (320x200) path keeps them across bands, and without this
                 * everything drawn after a mid-frame skip landed len bands
                 * too high (hundreds of EOBs per movie): scattered
                 * fragments in ENA2, KARA, RAN, SADES, ZARIA... */
                if (s->modex) {
                    size_t adv = (size_t)(ln ? ln : 1) * (size_t)s->w;
                    seg[0] += adv;
                    seg[1] += adv;
                    seg[2] += adv;
                    seg[3] += adv;
                }
                goto band_end;
            default:
                budget -= ln;
                s->st.unknown++;
                break;
            }
        }
    band_end:
        if (!saw_eob)
            band += 1;
        else
            band += eob_adv ? eob_adv : 1; /* defensive: EOB len 0 -> +1 */
    }
    return pos;
}

/* --------------------------------------------------------- stream driver */

/* decode one chunk payload; returns next pos, (size_t)-1 on END/desync */
static size_t decode_payload(is_stream *s, size_t pos, size_t stop,
                             is_frame_cb cb, void *user, unsigned max_frames)
{
    const uint8_t *d = s->buf.data;
    while (pos < stop && s->st.frames < max_frames) {
        uint8_t b = d[pos];
        switch (b & 0xB8) {
        case IS_OP_ESC: {
            if (pos + 5 > stop)
                BOUNDS();
            uint32_t u = rd32(d + pos + 1);
            s->st.esc++;
            s->st.esc_bytes += u;
            pos += 5 + (size_t)u;
            continue;
        }
        case IS_OP_END:
            return (size_t)-1;
        case IS_OP_RPT8:
            if (pos + 2 > stop)
                BOUNDS();
            s->st.repeat = d[pos + 1];
            pos += 2;
            continue;
        case IS_OP_RPT16:
            if (pos + 3 > stop)
                BOUNDS();
            s->st.repeat = rd16(d + pos + 1);
            pos += 3;
            continue;
        case IS_OP_PAL:
            if (pos + 769 > stop)
                BOUNDS();
            memcpy(s->pal, d + pos + 1, 768);
            s->st.palette++;
            pos += 769;
            continue;
        default:
            pos = decode_frame(s, pos, stop);
            if (s->st.ok)
                return (size_t)-1;
            s->st.frames++;
            if (cb)
                cb(user, s->st.frames - 1, s);
            continue;
        }
    }
    return pos;
}

size_t is_decode_chunk(is_stream *s, size_t chunk_i, is_frame_cb cb,
                       void *user, unsigned *frames_in_out)
{
    if (chunk_i >= s->nchunks || s->st.ok)
        return (size_t)-1;
    size_t ps = s->chunks[chunk_i].pos;
    size_t stop = ps + s->chunks[chunk_i].len;
    unsigned before = s->st.frames;
    s->st.chunks++;
    size_t r = decode_payload(s, ps, stop, cb, user, 1u << 30);
    if (frames_in_out)
        *frames_in_out = s->st.frames - before;
    return r;
}

int is_decode_all(is_stream *s, is_frame_cb cb, void *user,
                  unsigned max_frames)
{
    for (unsigned i = 0; i < s->nchunks; i++) {
        if (s->st.frames >= max_frames)
            break;
        size_t ps = s->chunks[i].pos;
        size_t stop = ps + s->chunks[i].len;
        s->st.chunks++;
        size_t r = decode_payload(s, ps, stop, cb, user, max_frames);
        if (r == (size_t)-1 && s->st.ok) /* desync, not plain END */
            return -1;
    }
    return s->st.ok ? -1 : 0;
}

void is_walk_audio(is_stream *s, is_esc_cb cb, void *user)
{
    is_walk_audio_range(s, 0, s->nchunks, cb, user);
}

void is_walk_audio_range(is_stream *s, unsigned first, unsigned count,
                         is_esc_cb cb, void *user)
{
    const uint8_t *d = s->buf.data;
    unsigned last = first + count;
    if (last > s->nchunks)
        last = s->nchunks;
    for (unsigned i = first; i < last; i++) {
        size_t pos = s->chunks[i].pos;
        size_t stop = pos + s->chunks[i].len;
        while (pos < stop) {
            uint8_t b = d[pos];
            switch (b & 0xB8) {
            case IS_OP_ESC: {
                if (pos + 5 > stop)
                    return;
                uint32_t u = rd32(d + pos + 1);
                size_t a = pos + 5, e = a + u;
                if (e > stop)
                    e = stop;
                if (cb && e > a)
                    cb(user, d + a, (unsigned)(e - a));
                pos += 5 + (size_t)u;
                continue;
            }
            case IS_OP_END:
                pos = stop;
                break;
            case IS_OP_RPT8:
                pos += 2;
                continue;
            case IS_OP_RPT16:
                pos += 3;
                continue;
            case IS_OP_PAL:
                pos += 769;
                continue;
            default: {
                /* skip a frame without drawing: walk bands/opcodes like
                 * decode_frame but touch only fb cursor math via a
                 * scratch copy is wasteful - audio walk only needs ESC
                 * regions, and ESC never appears mid-frame (live-verified
                 * 0 crossings), so a full draw-free band walk: */
                int band = 0;
                const int nbands = s->h / 4;
                int ln;
                while (band < nbands && pos < stop) {
                    int budget = s->w, eob_adv = 0, saw_eob = 0;
                    while (budget > 0 && pos < stop) {
                        uint8_t bb = d[pos++];
                        ln = bb & 7;
                        if (bb & 0x40) {
                            if (pos >= stop)
                                return;
                            ln = (ln << 8) | d[pos++];
                        }
                        int data = 0;
                        switch (bb & 0xB8) {
                        case IS_OP_SOLID:
                            data = 1;
                            budget -= ln;
                            break;
                        case IS_OP_LUT1:
                            data = 2 + ((ln + 1) >> 1);
                            budget -= ln;
                            break;
                        case IS_OP_LUT2:
                            data = 4 + ln;
                            budget -= ln;
                            break;
                        case IS_OP_LUT4:
                            data = 16 + 2 * ln;
                            budget -= ln;
                            break;
                        case IS_OP_RAW:
                            data = 4 * ln;
                            budget -= ln;
                            break;
                        case IS_OP_SKIP:
                            budget -= ln;
                            break;
                        case IS_OP_EOB:
                            eob_adv = ln;
                            saw_eob = 1;
                            goto abw_eob;
                        default:
                            budget -= ln;
                            break;
                        }
                        if (pos + (size_t)data > stop)
                            return;
                        pos += (size_t)data;
                    }
                abw_eob:
                    if (!saw_eob)
                        band++;
                    else
                        band += eob_adv ? eob_adv : 1;
                }
                continue;
            }
            }
        }
    }
}

/* ------------------------------------------------------------ output */

int is_linearize_into(const is_stream *s, uint8_t *out)
{
    size_t q = (size_t)s->w * (size_t)s->h / 4;
    if (!s->modex) {
        memcpy(out, s->fb, (size_t)s->w * (size_t)s->h);
        return 0;
    }
    /* plane p row r -> display row 4r+p (Mode X uninterleave) */
    for (int p = 0; p < 4; p++) {
        const uint8_t *plane = s->fb + (size_t)p * q;
        uint8_t *dst = out + (size_t)p * (size_t)s->w;
        for (int r = 0; r < s->h / 4; r++)
            memcpy(dst + (size_t)4 * r * (size_t)s->w,
                   plane + (size_t)r * (size_t)s->w, (size_t)s->w);
    }
    return 0;
}

uint8_t *is_linearize(const is_stream *s)
{
    uint8_t *out = (uint8_t *)malloc((size_t)s->w * (size_t)s->h);
    if (!out)
        return NULL;
    is_linearize_into(s, out);
    return out;
}

void is_palette_rgb8(const is_stream *s, uint8_t rgb[768])
{
    for (int i = 0; i < 768; i++)
        rgb[i] = (uint8_t)(s->pal[i] << 2); /* live DAC = v<<2 */
}

int is_write_ppm(const is_stream *s, const char *path)
{
    uint8_t *lin = is_linearize(s);
    if (!lin)
        return -1;
    uint8_t rgb[768];
    is_palette_rgb8(s, rgb);
    FILE *f = ll_fopen_ci(path, "wb");
    if (!f) {
        free(lin);
        return -1;
    }
    fprintf(f, "P6\n%d %d\n255\n", s->w, s->h);
    for (size_t i = 0; i < (size_t)s->w * (size_t)s->h; i++) {
        uint8_t c = lin[i];
        fputc(rgb[3 * c], f);
        fputc(rgb[3 * c + 1], f);
        fputc(rgb[3 * c + 2], f);
    }
    fclose(f);
    free(lin);
    return 0;
}
