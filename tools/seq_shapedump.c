/* seq_shapedump.c - dump the SHAPE geometry carried by the elements of
 * every .SEQ payload, computed by ported/ll_seq.c's ll_seq_shape_*
 * functions, in the exact text format of tools/shape_geom.py so
 * make verify-seq (--shape) can diff them on every container payload.
 * The self-test grid is the same deterministic one on both sides (each
 * shape element tested against ITSELF at fixed offsets and all four
 * facing combinations, derived only from its own row count, so batch
 * order cannot change it). */
#include <stdio.h>
#include <stdlib.h>

#include "ll_seq.h"

/* identical to shape_geom.py test_offsets() */
static int test_rows(int rows, int xoff[17], int yoff[17])
{
    xoff[0] = 0;         yoff[0] = 0;
    xoff[1] = 0;         yoff[1] = 0;
    xoff[2] = 0;         yoff[2] = 0;
    xoff[3] = 0;         yoff[3] = 0;
    xoff[4] = 1;         yoff[4] = 1;
    xoff[5] = 1;         yoff[5] = 1;
    xoff[6] = rows;      yoff[6] = 1;
    xoff[7] = rows;      yoff[7] = -1;
    xoff[8] = rows + 1;  yoff[8] = 1;
    xoff[9] = rows + 1;  yoff[9] = 1;
    xoff[10] = -rows - 1; yoff[10] = 1;
    xoff[11] = -rows - 1; yoff[11] = -1;
    xoff[12] = -rows;    yoff[12] = 1;
    xoff[13] = -rows;    yoff[13] = 1;
    xoff[14] = rows / 2; yoff[14] = 0;
    xoff[15] = rows / 2; yoff[15] = 0;
    xoff[16] = -(rows / 2) - 1; yoff[16] = -1;
    return 17;
}

/* fa/fb per grid row, identical to the tuples in shape_geom.py
 * test_offsets() (field order there is xoff, yoff, fa, fb) */
static const int t_fa[17] = {
    1,   /* 0 */
    1,   /* 1 */
    -1,  /* 2 */
    -1,  /* 3 */
    1,   /* 4 */
    -1,  /* 5 */
    1,   /* 6 */
    -1,  /* 7 */
    1,   /* 8 */
    -1,  /* 9 */
    1,   /* 10 */
    1,   /* 11 */
    1,   /* 12 */
    -1,  /* 13 */
    1,   /* 14 */
    -1,  /* 15 */
    -1   /* 16 */
};
static const int t_fb[17] = {
    1,   /* 0 */
    -1,  /* 1 */
    1,   /* 2 */
    -1,  /* 3 */
    1,   /* 4 */
    -1,  /* 5 */
    1,   /* 6 */
    1,   /* 7 */
    1,   /* 8 */
    -1,  /* 9 */
    1,   /* 10 */
    -1,  /* 11 */
    1,   /* 12 */
    1,   /* 13 */
    1,   /* 14 */
    -1,  /* 15 */
    1    /* 16 */
};

/* the degenerate case: when BOTH margin lists of an element are entirely
 * the empty marker, the game's two scans end having found nothing and its
 * box math stores w = 1 - total_w, h = 1 - rows (measured 2026-11-16: 34
 * elements on these containers). Both sides of the cross-check flag those
 * elements with the shared ll_seq_shape_empty test - the port reproduces
 * the game's arithmetic, and the flag says which lines prove it. */

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: seq_shapedump FILE.SEQ [more.SEQ ...]\n");
        return 2;
    }
    int rc = 0;
    for (int a = 1; a < argc; a++) {
        FILE *f = fopen(argv[a], "rb");
        if (!f) { fprintf(stderr, "cannot open %s\n", argv[a]); return 2; }
        fseek(f, 0, SEEK_END);
        long flen = ftell(f);
        fseek(f, 0, SEEK_SET);
        unsigned char *data = malloc((size_t)flen ? (size_t)flen : 1);
        if (!data || fread(data, 1, (size_t)flen, f) != (size_t)flen) {
            fprintf(stderr, "read error %s\n", argv[a]);
            return 2;
        }
        fclose(f);

        ll_seq s;
        if (ll_seq_parse(data, (unsigned)flen, &s) != 0) {
            printf("PARSE-FAIL\n");
            free(data);
            rc = 1;
            continue;
        }

        int nshape = 0;
        for (uint32_t i = 0; i < s.count; i++)
            if ((s.els[i].w0 & 3) && (int)(short)s.els[i].b17 > 0)
                nshape++;
        printf("shapeblob %u count %u shapes %d\n", s.len, s.count, nshape);
        for (uint32_t i = 0; i < s.count; i++) {
            const ll_seq_elem *e = &s.els[i];
            int tw, rows, n;
            ll_seq_shape_wh(e, &tw, &rows, &n);
            if (!(e->w0 & 3) || rows <= 0)
                continue;
            int bx, by, bw, bh;
            ll_seq_shape_bbox(&s, e, &bx, &by, &bw, &bh);
            printf("shape %u tw %d rows %d bbox %d %d %d %d shape_empty %d\n",
                   i, tw, rows, bx, by, bw, bh, ll_seq_shape_empty(&s, e));
            int xoff[17], yoff[17];
            test_rows(rows, xoff, yoff);
            for (int k = 0; k < 17; k++) {
                int h = ll_seq_shape_hit(&s, e, xoff[k], yoff[k], t_fa[k],
                                         e, 0, 0, t_fb[k]);
                printf("shapehit %u xoff %d yoff %d fa %d fb %d hit %d\n",
                       i, xoff[k], yoff[k], t_fa[k], t_fb[k], h);
            }
        }
        printf("shapeend\n");
        ll_seq_free(&s);
        free(data);
    }
    return rc;
}
