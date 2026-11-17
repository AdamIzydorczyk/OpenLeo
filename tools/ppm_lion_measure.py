#!/usr/bin/env python3
"""ppm_lion_measure.py PATH XMIN XMAX YMIN YMAX MASK [KEY=VAL ...]

PR-84 camera regression probe. Measures the LION's colour mask inside a
window of a --play PPM dump and prints its bounding box; with KEY=VAL
arguments it instead verifies them and exits 1 on the first mismatch.

The mask is a named colour class of the lion art (orange: r>150,
r-g>55, r-b>90, b<80 - the same predicate measured against the ORIGINAL
frame in NEXT_STEPS PR-84). Keys: top, bottom, left, right (mask bbox
edges). A bare value must match exactly; KEY=175+-3 allows a tolerance.

Why "top" and not feet-bottom for the anchor pin: our A_TURN art includes
a tail that paints ~20 px below the feet point, so the mask bottom is
pose-dependent; the mane's TOP edge is a stable landmark of the drawn
sprite, and the feet-at-178 rule is what places it (feet = top + the
art's fixed head-to-feet height). The window must be tight around the
lion's known position (read from the leg's own position log): ZARIA's
scene carries red props (balloons, the HUD number) that share the hue.

No third-party deps (pure stdlib, same contract as ppm_flip_check.py).
Exit 0 with a summary line; on failure prints the reason + measured box.
"""
import sys

MASKS = {
    # lion body orange: bright AND warm-yellow (g 70..170). The scene's
    # red props (ZARIA's balloon, the HUD life digit) are g<70 reds and
    # fall out of this band; they are NOT the lion.
    "orange": lambda r, g, b: (r > 150 and r - g > 55 and r - b > 90 and
                               b < 80 and 70 <= g < 170),
}


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
        print(__doc__.strip())
        return 2
    path = sys.argv[1]
    x0, x1, y0, y1 = (int(v) for v in sys.argv[2:6])
    mask = MASKS[sys.argv[6]]
    w, h, px = load(path)
    ys, xs = [], []
    for y in range(max(0, y0), min(h, y1 + 1)):
        row = y * w * 3
        for x in range(max(0, x0), min(w, x1 + 1)):
            i = row + x * 3
            if mask(px[i], px[i + 1], px[i + 2]):
                xs.append(x)
                ys.append(y)
    if not xs:
        print("ppm_lion_measure: NO %s pixels in the window x%d..%d y%d..%d"
              % (sys.argv[6], x0, x1, y0, y1))
        return 1
    box = {"left": min(xs), "right": max(xs), "top": min(ys),
           "bottom": max(ys)}
    got = "n=%d x %d..%d y %d..%d" % (len(xs), box["left"], box["right"],
                                       box["top"], box["bottom"])
    for arg in sys.argv[7:]:
        key, _, want = arg.partition("=")
        if key not in box:
            print("ppm_lion_measure: unknown key %r" % key)
            return 1
        tol = 0
        if "+-" in want:
            want, _, ts = want.partition("+-")
            tol = int(ts)
        if abs(box[key] - int(want)) > tol:
            print("ppm_lion_measure: %s = %d, expected %s (measured %s)"
                  % (key, box[key], arg.split("=", 1)[1], got))
            return 1
    print("ppm_lion_measure: %s mask %s" % (got, sys.argv[6]))
    return 0


if __name__ == "__main__":
    sys.exit(main())
