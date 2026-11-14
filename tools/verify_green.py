#!/usr/bin/env python3
"""verify_green.py - prove the restart-dialog selection tint (round 6).

User request: "TAK (YES) should be bright green on select, as NIE (NO) by
default". Measured ground truth (green_probe, 2026-11-15): in the
OPTIONS.PCX palette indices 2..22 are one monotonic green ramp; CNIE.SEQ
paints up to idx22 (#10f91b, L154) while TAK.SEQ tops out at idx15
(#0aa712, L103), so NIE reads bright green and TAK mid-green. The fix
re-blits the SELECTED word's ink shifted up that ramp (TAK +7, CNIE/X 0).

  usage: verify_green.py BASELINE DEF SEL_TAK SEL_CNIE \
             TAK_X0 TAK_Y0 TAK_X1 TAK_Y1 CNIE_X0 CNIE_Y0 CNIE_X1 CNIE_Y1

BASELINE   = frame from the previous build (no selection on open), or "-"
             to skip that comparison (CI has no old build to compare with)
DEF        = frame as the dialog now opens (default answer selected)
SEL_TAK    = frame with LEO_MENU_SEL=0 (TAK highlighted)
SEL_CNIE   = frame with LEO_MENU_SEL=1 (CNIE highlighted)
TAK/CNIE   = the two zone rects as printed by the menu (canvas px)

Checks (all numeric, no eyeballing):
  1 DEF == SEL_CNIE byte-for-byte          -> NIE is the default selection
  2 DEF vs BASELINE: only the CNIE underline strip + the '>' row-marker
    removal changed                                          -> no regression
  3 SEL_TAK vs DEF: changed pixels stay inside the TAK rect, TAK's underline
    strip (new) and CNIE's underline strip (removed); the word's ink moved
    UP the green ramp (more bright-green, less mid-green, mean lum up)
"""
import sys

GREEN_TOP = (0x10, 0xF9, 0x1B)          # OPTIONS.PCX idx22, brightest green
GREEN_MID = (0x0A, 0xA7, 0x12)         # idx15, TAK's authored brightest


def read_ppm(p):
    with open(p, "rb") as f:
        if f.readline().strip() != b"P6":
            raise ValueError(f"{p}: not a P6 PPM")
        w, h = (int(v) for v in f.readline().split())
        if f.readline().strip() != b"255":
            raise ValueError(f"{p}: bad maxval")
        raw = f.read()
    if len(raw) != w * h * 3:
        raise ValueError(f"{p}: short pixel data {len(raw)}")
    return w, h, raw


def px(raw, w, i):
    return (raw[3 * i], raw[3 * i + 1], raw[3 * i + 2])


def lum(p):
    return 0.299 * p[0] + 0.587 * p[1] + 0.114 * p[2]


def diff(a, b):
    """changed pixel indices"""
    w, h, ra = a
    _, _, rb = b
    assert (w, h) == (b[0], b[1]), "PPM size mismatch"
    return [i for i in range(w * h) if ra[3 * i:3 * i + 3] != rb[3 * i:3 * i + 3]]


def main():
    base = None if sys.argv[1] == "-" else read_ppm(sys.argv[1])
    df, tak, cnie = (read_ppm(p) for p in sys.argv[2:5])
    tx0, ty0, tx1, ty1 = (int(v) for v in sys.argv[5:9])
    cx0, cy0, cx1, cy1 = (int(v) for v in sys.argv[9:13])
    w, h, _ = df
    fails = []
    strip_tak = {(x, y) for x in range(tx0, tx1) for y in (ty1 + 1, ty1 + 2)}
    strip_cnie = {(x, y) for x in range(cx0, cx1) for y in (cy1 + 1, cy1 + 2)}

    # 1) NIE is the answer the dialog shows as selected on open
    if diff(df, cnie):
        fails.append("DEF != SEL_CNIE: the default selection is not CNIE")
    else:
        print("default answer: DEF frame == SEL_CNIE frame (byte-identical, ok)")

    # 2) the previous neutral frame only gained CNIE's underline (2 px tall
    #    under its rect) and lost the '>' row marker (the row is not the
    #    selection any more)
    marker = {(x, y) for x in range(12, 20) for y in range(176, 184)}
    if base is None:
        print("no-regression: SKIP (no pre-fix baseline frame given)")
    else:
        d2 = diff(base, df)
        bad = [xy for xy in ((i % w, i // w) for i in d2)
               if xy not in strip_cnie and xy not in marker]
        nstrip = sum(1 for i in d2 if (i % w, i // w) in strip_cnie)
        if bad:
            fails.append(f"DEF vs BASELINE: {len(bad)} unexpected px, "
                         f"e.g. {bad[:5]}")
        elif nstrip != len(strip_cnie):
            fails.append(f"DEF vs BASELINE: CNIE underline px = {nstrip}, "
                         f"want {len(strip_cnie)}")
        else:
            print(f"no-regression: DEF vs pre-fix frame = CNIE underline "
                  f"({nstrip} px) + '>' marker only (ok)")

    # 3) TAK selected: its ink shifts up the green ramp, nothing else moves
    ch = diff(df, tak)
    inset = {i for i in ch if tx0 <= i % w < tx1 and ty0 <= i // w < ty1}
    inner = list(inset)
    out = [(i % w, i // w) for i in ch if i not in inset
           and (i % w, i // w) not in strip_tak and (i % w, i // w) not in strip_cnie]
    if out:
        fails.append(f"SEL_TAK vs DEF: {len(out)} px outside TAK rect + the two "
                     f"underline strips, e.g. {out[:5]}")
    if len(inner) < 300:
        fails.append(f"SEL_TAK vs DEF: only {len(inner)} changed px inside the "
                     f"TAK rect {(tx0, ty0, tx1, ty1)} - word not re-tinted")

    def ink(p):
        r, g, b = p
        return g > r + 0x20 and g > b + 0x20          # green-ish ink only

    mid = sum(1 for i in inner if px(df[2], df[0], i) == GREEN_MID)
    bright = sum(1 for i in inner if px(tak[2], tak[0], i) == GREEN_TOP)
    l0 = sum(lum(px(df[2], df[0], i)) for i in inner) / max(1, len(inner))
    l1 = sum(lum(px(tak[2], tak[0], i)) for i in inner) / max(1, len(inner))
    if mid == 0 or bright == 0:
        fails.append(f"TAK ink: mid-green(idx15) px {mid}, bright-green(idx22) "
                     f"px {bright} - no ramp shift seen")
    if l1 <= l0:
        fails.append(f"TAK word not brighter: mean lum {l0:.1f} -> {l1:.1f}")
    nongreen = [i for i in inner if not ink(px(tak[2], tak[0], i))
                and not ink(px(df[2], df[0], i))]
    if len(nongreen) > len(inner) // 4:
        fails.append(f"SEL_TAK: {len(nongreen)} px became non-green "
                     f"(ramp shift must stay inside the green ramp)")
    if not fails:
        print(f"TAK on select: {len(ch)} px changed ({len(inner)} inside "
              f"{(tx0, ty0, tx1, ty1)} + underline strips), idx15 ink {mid} -> "
              f"idx22 ink {bright}, mean lum {l0:.1f} -> {l1:.1f} (bright green, ok)")

    for f in fails:
        print("FAILED:", f)
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
