#!/usr/bin/env python3
"""ppm_flip_check.py BASE FLIPPED XMIN XMAX YMIN YMAX [MINPIX]

PR-82 render check. Two PPM dumps of the SAME tick of the deterministic
--play grid, from two runs whose only difference is that one of them
touched a CH_DIR state and turned its instance facing (the Makefile leg:
a one-tap BKWD and back to A_STAY, sampled at a tick where BOTH logs show
the same state, the same frame number and the same position). The frames
must then differ exactly by the lion's sprite being REDRAWN mirrored in
place: at least MINPIX pixels (default 300) differ, and every differing
pixel lies inside the box XMIN,XMAX,YMIN,YMAX - the lion's own area.
A change outside that box (camera shift, another object, a moved sprite)
fails the check, as does "no change" (the facing flip drew nothing).

Note the check deliberately does NOT demand an image-perfect horizontal
mirror: the game's element draw (and ll_seq.c's, PR-49/65) mirrors EACH
PATCH and re-anchors it, it is not a bitmap reflection, so the mirrored
lion is not a pixel-for-pixel reflection of the plain one.

Exit 0 with a summary line, 1 with the reason.
"""
import sys


def load(path):
    with open(path, "rb") as f:
        if f.readline().strip() != b"P6":
            raise ValueError("%s: not a P6 PPM" % path)
        line = f.readline()
        while line.startswith(b"#"):
            line = f.readline()
        w, h = (int(v) for v in line.split())
        if f.readline().strip() != b"255":
            raise ValueError("%s: not 8-bit" % path)
        data = f.read()
    if len(data) != w * h * 3:
        raise ValueError("%s: truncated pixel data" % path)
    return w, h, data


def main():
    if len(sys.argv) < 7:
        print(__doc__)
        return 2
    ax, bx, ay, by = (int(v) for v in sys.argv[3:7])
    minpix = int(sys.argv[7]) if len(sys.argv) > 7 else 300
    try:
        wa, ha, A = load(sys.argv[1])
        wb, hb, B = load(sys.argv[2])
    except ValueError as e:
        print("ppm_flip_check: FAILED: %s" % e)
        return 1
    if (wa, ha) != (wb, hb):
        print("ppm_flip_check: FAILED: size %dx%d vs %dx%d" % (wa, ha, wb, hb))
        return 1

    x0, x1 = wa, -1
    y0, y1 = ha, -1
    ndiff = 0
    for y in range(ha):
        for x in range(wa):
            i = (y * wa + x) * 3
            if A[i:i + 3] != B[i:i + 3]:
                ndiff += 1
                x0 = min(x0, x); x1 = max(x1, x)
                y0 = min(y0, y); y1 = max(y1, y)
    if ndiff == 0:
        print("ppm_flip_check: FAILED: the frames are identical - the"
              " facing flip drew nothing")
        return 1
    if ndiff < minpix:
        print("ppm_flip_check: FAILED: only %d pixel(s) differ (want >=%d):"
              " too small to be a sprite redraw" % (ndiff, minpix))
        return 1
    if not (ax <= x0 and x1 <= bx and ay <= y0 and y1 <= by):
        print("ppm_flip_check: FAILED: %d pixel(s) differ over x=%d..%d"
              " y=%d..%d, outside the allowed box x=%d..%d y=%d..%d: the"
              " change is not a redraw inside the lion's own area"
              % (ndiff, x0, x1, y0, y1, ax, bx, ay, by))
        return 1
    print("ppm_flip_check: %d pixel(s) redrawn over x=%d..%d y=%d..%d, all"
          " inside the lion's area, same state/frame/position in both logs:"
          " the facing flip redrew his sprite in place (ok)"
          % (ndiff, x0, x1, y0, y1))
    return 0


if __name__ == "__main__":
    sys.exit(main())
