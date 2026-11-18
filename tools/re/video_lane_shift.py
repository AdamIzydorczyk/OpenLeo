#!/usr/bin/env python3
"""video_lane_shift.py - horizontal object speed inside a camera-still window.

PR-125. Once video_stills.py has proved a window is camera-locked, any
content shift of a fixed screen band between frame t and t+LAG frames is
object motion in world coordinates. This accumulates mean |delta| per
integer x-shift over the band and reports the best shifts, in game px/s
at the R3 60 fps timeline.

Band coordinates are in the 640x400 game frame (crop=1152:720:0:0,
scale=640:400, 1 game px = 1 px). Average over many pairs (seconds of
footage) so in-place oscillation (a waving penguin, a swimming fish
animation) cancels and only net translation survives: a band that sits
still scores lowest at shift 0.

  ./video_lane_shift.py reference/video/levels/ENA.03.mkv 30 48 130 180 0 250
  -> PR-125, fish school band during the 27.7-49.7 s camera lock:
     best shift 0 px at lag 40 over 316 pairs: the RYBKA schools do not
     translate; A_START idles.
"""
import argparse
import subprocess
from collections import deque

import numpy as np


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('file')
    ap.add_argument('--t0', type=float, required=True)
    ap.add_argument('--t1', type=float, required=True)
    ap.add_argument('--band', type=int, nargs=4, required=True,
                    metavar=('Y0', 'Y1', 'X0', 'X1'),
                    help='screen band in 640x400 game pixels')
    ap.add_argument('--lag', type=int, default=40, help='frames per pair')
    ap.add_argument('--maxshift', type=int, default=60)
    ap.add_argument('--every', type=int, default=4,
                    help='use one of every N frames as the later side')
    args = ap.parse_args()
    y0, y1, x0, x1 = args.band
    ms, lag = args.maxshift, args.lag

    cmd = ['ffmpeg', '-v', 'error', '-ss', str(args.t0), '-to', str(args.t1),
           '-i', args.file, '-vf', 'crop=1152:720:0:0,scale=640:400',
           '-f', 'rawvideo', '-pix_fmt', 'gray', '-']
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, bufsize=10 ** 8)
    w, h = 640, 400
    ring = deque(maxlen=lag + 1)
    res = np.zeros(2 * ms + 1)
    n = fi = 0
    while True:
        buf = proc.stdout.read(w * h)
        if len(buf) < w * h:
            break
        frame = np.frombuffer(buf, np.uint8).reshape(h, w)
        if len(ring) == lag + 1 and fi % args.every == 0:
            a = ring[0][y0:y1, x0:x1].astype(np.int16)
            b = frame[y0:y1, x0:x1].astype(np.int16)
            padded = np.pad(b, ((0, 0), (ms, ms)))
            for k in range(-ms, ms + 1):
                res[k + ms] += np.abs(
                    padded[:, ms + k:ms + k + (x1 - x0)] - a).mean()
            n += 1
        ring.append(frame)
        fi += 1
    proc.wait()
    res /= max(n, 1)
    print('frames=%d pairs=%d' % (fi, n))
    for o in np.argsort(res)[:12]:
        k = int(o) - ms
        print('  shift %4d  sad=%8.2f  v=%7.1f px/s' %
              (k, res[o], k * 60.0 / lag))


if __name__ == '__main__':
    main()
