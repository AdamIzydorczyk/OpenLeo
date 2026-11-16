#!/usr/bin/env python3
"""shape_geom.py - python reference for the .SEQ element SHAPE geometry,
the twin of ported/ll_seq.c's ll_seq_shape_* functions.

RE source (2026-11-16, full decompilation, build/decompiled/all):
  FUN_00041f60  loader: with w0&3 == 0 the element's rect/anchor come
                straight from the words; with w0&3 != 0 the two margin
                lists are read and FUN_000420b0 derives the rect from
                them instead.
  FUN_000420b0  bbox over the two lists (0xFF = empty row at the edges;
                0xFF INSIDE the rows participates as the value 255).
  FUN_00042a70  span-overlap hit test between two positioned elements
                (its row window re-read from the machine code of
                build/leo/leo.elf, 0x42b85..0x42be6: the element with the
                LOWER adjusted y is indexed from row 0).
  FUN_00042de0  push-out: read in the machine code but NOT modeled here or
                in the port - the decompiled text loses its register
                arguments and some operand identities are unresolved (the
                note is in ll_seq.h; tracked in NEXT_STEPS, M4-C).
An element's shape: row i (0..int16(b17)-1) spans
  x = [left[i], total_w - right[i]] inside the element rect,
total_w = int16(b19)*0x50 + int16(b15); byte 0xFF = empty row. The
lists are the game's runtime fields +0x27/+0x2f. They are NOT pixels:
this replaces the "inline palette+sprite" reading (wrong, PR-45).

The degenerate case: the game's box math scans the LEFT list from its
first non-marker row and the RIGHT list from its last non-marker row. When
BOTH lists are entirely the marker (34 elements on these containers, and no
element with only one list empty) both scans end having found nothing, and
what gets stored is w = 1 - total_w, h = 1 - rows - the game's own
arithmetic, not a data gap. Both sides reproduce it and flag those elements
"shape_empty 1" so the cross-check is known to cover them.

dump format (compared byte-exact by verify_seq.py --shape against the
C tool tools/seq_shapedump.c): one line per blob, per shape element and
per deterministic self-test pair:

  shapeblob <len> count <count> shapes <n_shape_elems>
  shape <elem idx> tw <total_w> rows <int16(b17)> bbox <x> <y> <w> <h> shape_empty <0|1>
  shapehit <elem idx> xoff <xoff> yoff <yoff> fa <fa> fb <fb> hit <0|1>
  shapeend

usage: shape_geom.py FILE.SEQ [more.SEQ ...]      (payload files)
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import seq_parse  # noqa: E402

sx = seq_parse.sx
EMPTY = 0xFF
SENT = 0x7FFF


class Geo:
    """geometry over one parsed payload; margins read exactly like the C
    port (unsigned, one-past-payload = EMPTY)."""

    def __init__(self, data, st):
        self.d, self.st, self.n = data, st, len(data)

    def marg(self, e, which, i):
        raw = e["ml"] if which == 0 else e["mr"]
        if i < 0:
            return EMPTY
        p = e["_o"] + 16 + (4 if e["w0"] & 4 else 0) + (0 if which == 0
                                                         else len(raw)) + i
        return self.d[p] if p < self.n else EMPTY

    def shape_empty(self, e):
        """declares rows but every margin byte of BOTH lists is the empty
        marker: the game's scans then find nothing and its box math stores
        w = 1 - total_w, h = 1 - rows (34 elements on these containers,
        and no element with only one list empty). Both sides reproduce
        that arithmetic; the flag names the lines that prove the
        cross-check covers it."""
        rows = sx(e["b17"])
        return 1 if (rows > 0 and e["w0"] & 3
                     and all(v == EMPTY for v in e["ml"])
                     and all(v == EMPTY for v in e["mr"])) else 0


def anch_x(e):
    return 0 if e["f0"] == SENT else sx(e["f0"])


def anch_y(e):
    return 0 if e["f2"] == SENT else sx(e["f2"])


def shape_wh(e):
    tw = sx(e["b19"]) * 0x50 + sx(e["b15"])
    rows = sx(e["b17"])
    return tw, rows, (rows if e["w0"] & 3 else 0)


def short(v):
    v &= 0xFFFF
    return v - 0x10000 if v >= 0x8000 else v


def bbox(g, e):
    """FUN_000420b0, statement for statement."""
    tw, rows, _ = shape_wh(e)
    t = 0
    while t < rows and g.marg(e, 0, t) == EMPTY:
        t += 1
    l = tw
    for i in range(t, rows):
        v = g.marg(e, 0, i)
        if v < l:
            l = v
    b = rows - 1
    while b > -1:
        if g.marg(e, 1, b) != EMPTY:
            break
        b -= 1
    r = tw
    for i in range(b, -1, -1):
        v = g.marg(e, 1, i)
        if v < r:
            r = v
    return (short(l - anch_x(e)), short(t - anch_y(e)),
            short(((tw - r) - l) + 1), short((b - t) + 2))


def pos(g, e, x, y, facing, tw):
    """the game's position adjustments at the top of 42a70/42de0."""
    if facing < 0:
        if e["f0"] != SENT:
            return x - (tw - anch_x(e)), y - anch_y(e), 1
        return x, y, 1
    return x - anch_x(e), y - anch_y(e), 0


def window(a, ay2, b, by2):
    """the row window, machine-code verified 2026-11-16 (see shape_window
    in ported/ll_seq.c): the element with the LOWER adjusted y is read
    from row 0, the other one from the distance between them."""
    ah, bh = sx(a["b17"]), sx(b["b17"])
    dy = ay2 - by2
    if dy < 0:
        return -dy, 0, min(ah + dy, bh)
    return 0, dy, min(bh - dy, ah)


def hit(g, a, ax, ay, fa, b, bx, by, fb):
    """FUN_00042a70."""
    aw, _, an = shape_wh(a)
    bw, _, bn = shape_wh(b)
    if an <= 0 or bn <= 0:
        return 0
    ax2, ay2, al = pos(g, a, ax, ay, fa, aw)
    bx2, by2, bl = pos(g, b, bx, by, fb, bw)
    oa, ob, m = window(a, ay2, b, by2)
    if m < 1:
        return 0
    for i in range(m):
        aml = g.marg(a, al, oa + i)
        amr = g.marg(a, 1 - al, oa + i)
        bml = g.marg(b, bl, ob + i)
        bmr = g.marg(b, 1 - bl, ob + i)
        if (aml != EMPTY and bml != EMPTY
                and ax2 + aml < bx2 + bw - bmr
                and bx2 + bml < ax2 + aw - amr):
            return 1
    return 0


# the deterministic self-test grid: every shape element tested against
# ITSELF at fixed offsets and all four facing combinations (the same
# grid on both sides of the oracle, derived only from the element's own
# row count, so batch order cannot change it). Offsets chosen to land
# on and past both ends of the row window and inside it; fa/fb cover
# both facing (margin-swap) branches and both anchor-adjustment sides.
def test_offsets(rows):
    return [
        (0, 0, 1, 1),
        (0, 0, 1, -1),
        (0, 0, -1, 1),
        (0, 0, -1, -1),
        (1, 1, 1, 1),
        (1, 1, -1, -1),
        (rows, 1, 1, 1),
        (rows, -1, -1, 1),
        (rows + 1, 1, 1, 1),
        (rows + 1, 1, -1, -1),
        (-rows - 1, 1, 1, 1),
        (-rows - 1, -1, 1, -1),
        (-rows, 1, 1, 1),
        (-rows, 1, -1, 1),
        (rows // 2, 0, 1, 1),
        (rows // 2, 0, -1, -1),
        (-(rows // 2) - 1, -1, -1, 1),
    ]


def dump_text(data):
    """geometry dump of one payload (bytes), same text as dump_file."""
    try:
        st = seq_parse.seq_parse(data)
    except seq_parse.SeqError:
        return "PARSE-FAIL\n"
    for e in st["els"]:
        e["_o"] = e["off"]
    g = Geo(data, st)
    shapes = [e for e in st["els"] if e["w0"] & 3 and e.get("n", 0) > 0]
    out = ["shapeblob %d count %d shapes %d"
           % (len(data), st["count"], len(shapes))]
    for k, e in enumerate(st["els"]):
        if not (e["w0"] & 3) or e.get("n", 0) <= 0:
            continue
        tw, rows, _ = shape_wh(e)
        bx, by, bw, bh = bbox(g, e)
        out.append("shape %d tw %d rows %d bbox %d %d %d %d shape_empty %d"
                   % (k, tw, rows, bx, by, bw, bh, g.shape_empty(e)))
        for xoff, yoff, fa, fb in test_offsets(rows):
            h = hit(g, e, xoff, yoff, fa, e, 0, 0, fb)
            out.append("shapehit %d xoff %d yoff %d fa %d fb %d hit %d"
                       % (k, xoff, yoff, fa, fb, h))
    out.append("shapeend")
    return "\n".join(out) + "\n"


def dump_file(path):
    return dump_text(open(path, "rb").read())


def census(paths):
    """aggregate shape facts (used by --census; never by the diff)."""
    tot = shapes = rempty = 0
    for p in paths:
        t = dump_file(p)
        for ln in t.splitlines():
            if ln.startswith("shapeblob "):
                tot += int(ln.split()[3])
            elif ln.startswith("shape "):
                shapes += 1
                if ln.split()[-1] == "1":
                    rempty += 1
    return {"blobs": len(paths), "shape_elems": shapes,
            "total_elems": tot, "empty_margin_lists": rempty}


def main():
    args = sys.argv[1:]
    if not args:
        print(__doc__)
        return 2
    if args[0] == "--census":
        c = census(args[1:])
        for k in sorted(c):
            print("%s %d" % (k, c[k]))
        return 0
    for p in args:
        sys.stdout.write(dump_file(p))
    return 0


if __name__ == "__main__":
    sys.exit(main())
