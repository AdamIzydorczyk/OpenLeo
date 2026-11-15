#!/usr/bin/env python3
"""golden_probe_inspect.py - classify QEMU screendumps from the golden rig
(timeout.ppm, probe_NNN.ppm) without opening any image: PPM header dims are
already the mode fingerprint -

  height 480  -> the .IS movie chain (480-line FMV mode, PLAN S6)
  height 400  -> the chapter-map menu mode (640x400 CRTC, hunt13)
  + text-only low-color frame -> DOS text screen

and mean/max RGB + distinct-color counts separate black fades, held final
frames and live art. Companion of golden_qemu_run.py --probe-shots.

Usage: golden_probe_inspect.py DIR_OR_FILE [more...]
"""
import os
import sys


def classify(path):
    with open(path, 'rb') as f:
        if f.readline().strip() != b'P6':
            return 'not-a-PPM'
        w, h = (int(x) for x in f.readline().split())
        f.readline()
        px = f.read()
    step = max(1, (len(px) // 3) // 4000) * 3
    cols, mean, n, dark = set(), 0, 0, 0
    for i in range(0, len(px) - 2, step):
        cols.add((px[i] >> 3, px[i + 1] >> 3, px[i + 2] >> 3))
        mean += px[i] + px[i + 1] + px[i + 2]
        n += 3
        if px[i] + px[i + 1] + px[i + 2] < 24:
            dark += 1
    mode = {480: 'movie(480-line FMV)',
            400: 'MENU(%dx400 canvas)' % w}.get(h, 'other(%dx%d)' % (w, h))
    return '%s colors=%d mean=%d dark=%d%%' % (
        mode, len(cols), mean // max(n, 1), 100 * dark // max(n // 3, 1))


def main():
    for arg in sys.argv[1:]:
        for d in [arg] if os.path.isfile(arg) else sorted(
                os.path.join(arg, f) for f in os.listdir(arg)
                if f.endswith('.ppm')):
            print('%-28s %s' % (os.path.basename(d), classify(d)))


if __name__ == '__main__':
    main()
