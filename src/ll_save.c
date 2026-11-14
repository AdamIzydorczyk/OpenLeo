/* ll_save.c - STATUS.LEO / LONGSOFT.CFG codec (see ll_save.h). Mirrors
 * FUN_0005dae0 (decode: value = (stored ^ j) & 0xff per 21-word block,
 * j = index WITHIN block) and FUN_0005dcc0 (encode: stored =
 * (j | 0x77072400) ^ (var | (j & 0xffffff00))); save() echoes the raw
 * words back verbatim so a load/save round-trip of a shipped file is
 * byte-identical. */
#include "ll_save.h"

#include <stdio.h>
#include <string.h>

#define STATUS_BYTES  (LL_STATUS_WORDS * 4)
#define OBF_USD       0x77072400u         /* writer constant           */

static unsigned int rd32(const unsigned char *p)
{
    return (unsigned)p[0] | ((unsigned)p[1] << 8) | ((unsigned)p[2] << 16)
           | ((unsigned)p[3] << 24);
}

static void wr32(unsigned char *p, unsigned int v)
{
    p[0] = (unsigned char)v;
    p[1] = (unsigned char)(v >> 8);
    p[2] = (unsigned char)(v >> 16);
    p[3] = (unsigned char)(v >> 24);
}

int ll_status_load(const char *path, ll_status *st)
{
    FILE *f = fopen(path, "rb");
    if (!f)
        return -1;
    unsigned char buf[STATUS_BYTES + 1];
    size_t n = fread(buf, 1, sizeof buf, f);
    fclose(f);
    if (n != STATUS_BYTES)                /* exact size or reject      */
        return -1;
    for (unsigned i = 0; i < LL_STATUS_WORDS; i++)
        st->w[i] = rd32(buf + 4 * i);
    st->loaded = 1;
    return 0;
}

int ll_status_save(const char *path, const ll_status *st)
{
    FILE *f = fopen(path, "wb");
    if (!f)
        return -1;
    unsigned char buf[STATUS_BYTES];
    for (unsigned i = 0; i < LL_STATUS_WORDS; i++)
        wr32(buf + 4 * i, st->w[i]);
    size_t n = fwrite(buf, 1, sizeof buf, f);
    if (fclose(f) != 0 || n != sizeof buf)
        return -1;
    return 0;
}

int ll_status_byte(const ll_status *st, int block, int j)
{
    /* FUN_0005dae0 loops: uVar4 = j ^ *puVar5; keep & 0xff            */
    if (block < 0 || block > 2 || j < 0 || j >= 21)
        return -1;
    return (int)((st->w[block * 21 + j] ^ (unsigned)j) & 0xff);
}

int ll_status_word63(const ll_status *st)
{
    return (int)(((st->w[63] ^ OBF_USD) & 0xff00) >> 8);
}

int ll_status_word64(const ll_status *st)
{
    return (int)(((st->w[64] ^ OBF_USD) & 0xff00) >> 8);
}

unsigned int ll_status_plain(const ll_status *st, int k)
{
    if (k < 0 || k > 9)
        return 0;
    return st->w[65 + k];
}

void ll_status_dump(const ll_status *st, const char *path)
{
    static const char *names[3] = { "A", "B", "C" };
    printf("ll_save: %s: %d words (STATUS.LEO, FUN_0005dae0 decode)\n",
           path, LL_STATUS_WORDS);
    for (int b = 0; b < 3; b++) {
        printf("ll_save: block %s:", names[b]);
        for (int j = 0; j < 21; j++)
            printf(" %d", ll_status_byte(st, b, j));
        printf("\n");
    }
    printf("ll_save: word63(DAT_000a94f0)=%d word64(DAT_000cf464)=%d\n",
           ll_status_word63(st), ll_status_word64(st));
    printf("ll_save: plain[65..74]:");
    for (int k = 0; k < 10; k++)
        printf(" %u", ll_status_plain(st, k));
    printf("\n");
}

int ll_cfg_load(const char *path, ll_cfg *c)
{
    FILE *f = fopen(path, "rb");
    if (!f)
        return -1;
    size_t n = fread(c->b, 1, sizeof c->b, f);
    fclose(f);
    if (n != LL_CFG_SIZE)
        return -1;
    c->loaded = 1;
    return 0;
}

int ll_cfg_save(const char *path, const ll_cfg *c)
{
    FILE *f = fopen(path, "wb");
    if (!f)
        return -1;
    size_t n = fwrite(c->b, 1, LL_CFG_SIZE, f);
    if (fclose(f) != 0 || n != LL_CFG_SIZE)
        return -1;
    return 0;
}

int ll_cfg_drive(const ll_cfg *c)
{
    unsigned char b = c->b[0];            /* FUN_0005bf10: DAT_000cf254
                                           * = byte0 - 0x40            */
    return (b >= 'A' && b <= 'Z') ? b : 0;
}

void ll_cfg_set_drive(ll_cfg *c, char drive)
{
    if (drive >= 'a' && drive <= 'z')
        drive = (char)(drive - 'a' + 'A');
    c->b[0] = (unsigned char)drive;
}
