#!/usr/bin/env python3
"""video_stills.py - list camera-still windows of an R3 capture.

PR-125. A world-frame speed measurement needs intervals where the camera
does not move, so screen dx = world dx. This finds them from the static
decor only: every second, difference the 640x400 game-frame against the
previous sample; pixels whose luma changed are moving objects (lion,
enemies, fish, bubbles, water shimmer), pixels that did not are decor.
The median absolute frame-to-frame shift of the UNCHANGED pixel band, by
circular shift of 4x-downsampled column sums, is the camera drift; the
tool converts it to GAME PX/S (1 game px = 1 px of the 640x400 frame).
Runs of samples with |drift| <= --max are printed, in SECONDS of the
file (the -ss argument passed to ffmpeg, plus t0).

  ./video_stills.py --t0 50 --t1 170 reference/video/levels/ENA.03.mkv
  ./video_stills.py --max 6 --min 4 reference/video/levels/LONA.05_b.mkv

Output columns: start_s end_s s min|drift| samples mean|drift|

Estimator checked on ENA.03's hand-verified lock, 27.7-49.7 s: the
independent per-frame masked-SAD estimator (tools/re/joint-style,
/tmp/opencode/pr125/joint_val27.npz, 1318 consecutive-frame samples)
reports total drift -10.7 half-resolution px over the 22 s, about
-0.5 game px/s - the lock holds to well under 1 px/s. Like
video_track_lion.py this is an ESTIMATOR, not ground truth: a scene
with little vertical decor contrast (sand banks) can report drift it
does not have - hand-pick the window and eyeball it before trusting
a number.
"""
import argparse
import subprocess

import numpy as np

SCALE = 'crop=1152:720:0:0,'            # R3: 16:9 capture, bar right
GAME = 'scale=640:400:flags=neighbor,'  # 1 game px = 1 frame px
GRAY = 'format=gray,fps=%d,scale=160:100:flags=area,format=gray'


def frames(path, t0, span, fps):
    cmd = ['ffmpeg', '-v', 'error', '-ss', str(t0), '-t', str(span),
           '-i', path, '-vf', SCALE + GAME + GRAY % fps,
           '-f', 'rawvideo', '-pix_fmt', 'gray', '-']
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    w, h = 160, 100
    out = []
    while True:
        buf = proc.stdout.read(w * h)
        if len(buf) < w * h:
            break
        out.append(np.frombuffer(buf, dtype=np.uint8).reshape(h, w))
    proc.wait()
    return out


def columns(img, changed=None):
    """column sums of pixels that did NOT change since the sample 1 s ago"""
    cols = img.astype(np.float64)
    if changed is not None:
        cols[changed] = 0.0
    return cols.sum(axis=0)


def circular_shift(a, b, rng=12):
    best, shift = None, 0
    sa = np.roll(a, 16) - a.mean()
    for s in range(-rng, rng + 1):
        cost = float(np.abs(sa - (np.roll(b, 16 + s) - b.mean())).sum())
        if best is None or cost < best:
            best, shift = cost, s
    return shift


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('file')
    ap.add_argument('--t0', type=float, default=0.0,
                    help='first file second to sample (add this to output)')
    ap.add_argument('--t1', type=float, default=None,
                    help='last file second to sample (default: whole file)')
    ap.add_argument('--max', type=float, default=1.0,
                    help='still threshold, px/s of camera drift')
    ap.add_argument('--min', type=float, default=4.0,
                    help='shortest run to report, seconds')
    args = ap.parse_args()
    span = (args.t1 - args.t0) if args.t1 is not None else None

    imgs = frames(args.file, args.t0, span, 2)
    prev = changed = None
    runs = []
    for i, img in enumerate(imgs):
        drift = None
        if prev is not None:
            diff = np.abs(img.astype(np.int16) - prev.astype(np.int16))
            changed = diff >= 6
            moved = float(changed.mean())
            if moved < 0.85:  # else the whole frame churned: no signal
                drift = abs(circular_shift(columns(img, changed),
                                          columns(prev, None)))
        prev = img
        if drift is not None and drift <= args.max:
            if runs and runs[-1][1] == i - 1:
                runs[-1][1] = i
                runs[-1][2].append(drift)
            else:
                runs.append([i, i, [drift]])
    for st, en, ds in runs:
        if en - st + 1 >= args.min:
            print('%8.1f %8.1f %5.2f %8.3f %5d %8.2f' %
                  (args.t0 + st / 2, args.t0 + (en + 1) / 2,
                   (en - st + 1) / 2, min(ds), len(ds),
                   float(np.mean(ds))))


if __name__ == '__main__':
    main()
