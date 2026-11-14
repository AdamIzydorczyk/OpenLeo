/* llrs.c - see llrs.h for the verified container format description.
 * Reference oracle: tools/llrs_unpack.py (byte-for-byte agreement on all
 * four shipped containers, checked by linux `make verify-llrs`). */
#include "llrs.h"
#include "fs_ci.h"

#include <stdlib.h>
#include <string.h>

static unsigned rd_le32(const unsigned char *p)
{
    return (unsigned)p[0] | ((unsigned)p[1] << 8) | ((unsigned)p[2] << 16)
           | ((unsigned)p[3] << 24);
}

/* ASCII-case-insensitive compare of NUL-padded fixed keys */
static int key_ci_eq(const unsigned char *k, const char *name)
{
    unsigned i = 0;
    for (; i < LLRS_KEYLEN && name[i]; i++) {
        char a = (char)k[i], b = name[i];
        if (a >= 'A' && a <= 'Z') a = (char)(a - 'A' + 'a');
        if (b >= 'A' && b <= 'Z') b = (char)(b - 'A' + 'a');
        if (a != b)
            return 0;
    }
    /* i is the name's terminator index (< LLRS_KEYLEN) or == LLRS_KEYLEN
     * when the name is at least 14 chars long. Remaining key bytes must be
     * NUL padding, and the name must end within the key. */
    for (unsigned j = i; j < LLRS_KEYLEN; j++)
        if (k[j])
            return 0;
    return name[i] == '\0';
}

static int key_printable(const unsigned char *k, unsigned *len)
{
    unsigned n = 0;
    while (n < LLRS_KEYLEN && k[n] >= 0x20 && k[n] < 0x7f)
        n++;
    *len = n;
    /* the rest must be NUL padding (or it is a binary key) */
    for (unsigned i = n; i < LLRS_KEYLEN; i++)
        if (k[i])
            return 0;
    return n > 0;
}

static void slot_fill(llrs_slot *s, const unsigned char *raw)
{
    memcpy(s->key, raw, LLRS_KEYLEN);
    unsigned len;
    if (key_printable(s->key, &len)) {
        memcpy(s->name, s->key, len);
        s->name[len] = '\0';
    } else {
        s->name[0] = '\0';
    }
    s->id    = rd_le32(raw + 14);
    s->off   = rd_le32(raw + 18);
    s->size  = rd_le32(raw + 22);
}

int llrs_open(llrs *c, const char *path)
{
    memset(c, 0, sizeof *c);
    c->f = ll_fopen_ci(path, "rb");   /* container names ship UPPERCASE but
                                       * users extract lowercase trees */
    if (!c->f)
        return 1;

    unsigned char hdr[0x14];
    if (fread(hdr, 1, sizeof hdr, c->f) != sizeof hdr
        || memcmp(hdr, "llrs", 4) != 0 || rd_le32(hdr + 4) != 1) {
        llrs_close(c);
        return 2;
    }
    c->cap        = rd_le32(hdr + 8);
    c->actual     = rd_le32(hdr + 0x10);
    c->data_start = 0x14u + 26u * (size_t)c->cap;

    long long fsize;
    if (ll_fseek64(c->f, 0, SEEK_END) != 0
        || (fsize = ll_ftell64(c->f)) < 0) {   /* P-07: 64-bit clean */
        llrs_close(c);
        return 3;
    }
    c->fsize = fsize;

    c->slots = (llrs_slot *)calloc(c->cap, sizeof *c->slots);
    if (!c->slots) {
        llrs_close(c);
        return 4;
    }
    unsigned char *raw =
        (unsigned char *)malloc(26u * (size_t)c->cap);
    if (!raw || fseek(c->f, 0x14, SEEK_SET) != 0
        || fread(raw, 26, c->cap, c->f) != c->cap) {
        free(raw);
        llrs_close(c);
        return 5;
    }
    for (unsigned k = 0; k < c->cap; k++) {
        llrs_slot *s = &c->slots[k];
        slot_fill(s, raw + 26 * k);
        /* mirror the python reference filter: id, size, range in data */
        if (s->id == 0 || s->size == 0 || s->name[0] == '\0'
            || (size_t)s->off < c->data_start
            || (size_t)s->off + s->size > (size_t)fsize) {
            s->id = 0; /* keep key/coords for forensics but mark unused */
        } else {
            c->used++;
        }
    }
    free(raw);
    return 0;
}

void llrs_close(llrs *c)
{
    if (c->f)
        fclose(c->f);
    free(c->slots);
    memset(c, 0, sizeof *c);
}

const llrs_slot *llrs_find(const llrs *c, const char *name)
{
    for (unsigned k = 0; k < c->cap; k++) {
        const llrs_slot *s = &c->slots[k];
        if (s->id && key_ci_eq(s->key, name))
            return s;
    }
    return NULL;
}

int llrs_block(const llrs *c, const llrs_slot *slot, llrs_slot **out,
               unsigned *nrecs)
{
    unsigned char *b = NULL;
    long n = llrs_read(c, slot, &b);
    if (n < 4) {
        free(b);
        return 1;
    }
    unsigned cnt = rd_le32(b);
    if (4u + 26u * (size_t)cnt > (size_t)n) {
        free(b);
        return 2;
    }
    if (cnt == 0) {                /* a legal empty block: calloc(0) may
                                    * return NULL, which is NOT an error */
        free(b);
        *out   = NULL;
        *nrecs = 0;
        return 0;
    }
    llrs_slot *r = (llrs_slot *)calloc(cnt, sizeof *r);
    if (!r) {
        free(b);
        return 3;
    }
    for (unsigned k = 0; k < cnt; k++) {
        slot_fill(&r[k], b + 4 + 26 * k);
        /* sub-records carry ABSOLUTE offsets too: range-validate them
         * against the container, mirroring the top-level filter (E-07);
         * out-of-range records become id==0 (padding) */
        if (r[k].id &&
            (r[k].size == 0 ||
             (unsigned long long)r[k].off + r[k].size >
                 (unsigned long long)c->fsize))
            r[k].id = 0;
    }
    free(b);
    *out   = r;
    *nrecs = cnt;
    return 0;
}

void llrs_block_free(llrs_slot *recs) { free(recs); }

long llrs_read(const llrs *c, const llrs_slot *s, unsigned char **out)
{
    /* full range check first (E-07): never malloc/seek past the file */
    if ((unsigned long long)s->off + s->size >
            (unsigned long long)c->fsize || s->off < c->data_start
        || s->size > 0x7fffffffu)   /* return type is long (P-07 guard) */
        return -1;
    /* one extra NUL: several payloads are text (.DES/.MNU/.MOV) and the
     * scanners walk them as C strings; the NUL is past the payload bytes */
    unsigned char *b = (unsigned char *)malloc((size_t)s->size + 1);
    if (!b)
        return -1;
    if (ll_fseek64(c->f, (long long)s->off, SEEK_SET) != 0
        || fread(b, 1, s->size, c->f) != s->size) {
        free(b);
        return -1;
    }
    b[s->size] = '\0';
    *out = b;
    return (long)s->size;
}
