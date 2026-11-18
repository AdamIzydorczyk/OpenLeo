#!/usr/bin/env python3
"""video_blob_scan.py - walk a whole R3 capture and report, per sampled
frame, whether the game is PLAYING (the HUD is drawn) or showing a
picture, plus where a colour-defined blob of a chosen RGB is on screen.

PR-128. A law probe needs a run of a named class inside a camera-still
window, and PR-126's lesson was that the first question is whether the
class is on screen at all: the KADMAR.07 windows held a ZOLW march and a
scripted scene, not the boss. Answering that one window at a time by eye
does not scale to a 24-minute level cut, so this scans every frame: the
game window is cropped (1152x720 of R3, then down-scaled to 640x400, so
1 measurement px = 1 game px), and for each sample it prints two things.

  1. HUD CLASS. The energy/lives portrait in the bottom-left corner and
     the spiral in the bottom-right are drawn only while the level is
     playing. Counting non-black pixels in those two fixed boxes says
     'G' (playing) or 'C' (a cutscene, a title card, an in-level
     scripted scene, a black room) without anyone looking at the frame.
     This part is a MEASUREMENT: the boxes are fixed, and across the
     whole KADMAR.07 cut the two states separate with a 34x margin:
     the WEAKER of the two boxes never rises above 126 non-black px
     in a scripted frame and never falls below 4 337 px in a gameplay
     frame (2 903 samples at 2 fps, and no ambiguous frame in the
     file), so the default threshold of 1 500 sits inside the gap.
  2. COLOUR BLOB. Pixels whose RGB is within --tol (sum of absolute
     component differences) of --rgb, with the --exclude rectangles
     cleared (default: the two HUD corners, so the game's own violet
     lion portrait cannot match forever), are masked and the LARGEST
     4-connected component is reported with its x span; runs of samples
     holding a component of at least --min pixels are listed.
     This part is a CANDIDATE FINDER, not an identification: one
     palette value is shared by unrelated art. The KADMAR.07 jungle
     matches the boss robe at 702-712 s, and so does the player's own
     purple-robe costume (PR-128) - the frames say which is which,
     the mask cannot.
  ./video_blob_scan.py --rgb 86,30,152 --min 800 \
      reference/video/levels/KADMAR.07.mkv

--rgb must be sampled from the capture itself: the boss robe at 1339 s
is 6025 pixels of one exact value, (86,30,152), after the uploader's
x264 pass; --tol is there for the codec noise around it. An object
drawn with a different palette entry, or one hidden for most of a
window, is simply not found.
"""
import argparse
import subprocess

import numpy as np

GAME = 640, 400                     # 1 measurement px = 1 game px


def frames(path, t0, span, fps):
    vf = ('crop=1152:720:0:0,scale=640:400:flags=neighbor,'
          'fps=%d,format=rgb24' % fps)
    cmd = ['ffmpeg', '-v', 'error', '-ss', str(t0)]
    if span is not None:                # -t 'None' is not a duration
        cmd += ['-t', str(span)]
    cmd += ['-i', path, '-vf', vf, '-f', 'rawvideo', '-pix_fmt', 'rgb24',
            '-']
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    w, h = GAME
    n = 0
    while True:
        buf = proc.stdout.read(w * h * 3)
        if len(buf) < w * h * 3:
            break
        n += 1
        yield np.frombuffer(buf, dtype=np.uint8).reshape(h, w, 3)
    proc.wait()


def runs_of(mask_row):
    """[start,end) intervals of True in a 1-D boolean row"""
    idx = np.flatnonzero(np.diff(np.r_[0, mask_row.view(np.int8), 0]))
    return list(zip(idx[0::2], idx[1::2]))


def largest_blob(mask):
    """largest 4-connected component: (pixels, x0, x1, y0, y1) or None.
    Row-run union-find - the masks here are a few thousand pixels wide,
    so a full two-pass labeller would be waste."""
    parent = []                       # (root, y, x0, x1) per row run
    start = []                        # index of each row's first run
    rows = []
    for y, row in enumerate(mask):
        rs = runs_of(row)
        start.append(len(parent))
        rows.append(rs)
        for a, b in rs:
            parent.append((len(parent), y, a, b))

    def find(i):
        while parent[i][0] != i:
            parent[i] = (parent[i][0],) + parent[i][1:]
            i = parent[i][0]
        return i

    def union(i, j):
        ri, rj = find(i), find(j)
        if ri != rj:
            parent[rj] = (ri,) + parent[rj][1:]

    for y in range(1, len(rows)):
        for k, (a, b) in enumerate(rows[y]):
            for h, (c, d) in enumerate(rows[y - 1]):
                if d <= a:
                    continue
                if c >= b:
                    break
                union(start[y] + k, start[y - 1] + h)
    if not parent:
        return None
    counts = {}
    for i in range(len(parent)):
        root = find(i)
        counts[root] = counts.get(root, 0) + (parent[i][3] - parent[i][1])
    root = max(counts, key=lambda k: counts[k])
    px = counts[root]
    xs0 = xs1 = ys0 = ys1 = None
    for i in range(len(parent)):
        if find(i) == root:
            _, y, a, b = parent[i]
            xs0 = a if xs0 is None else min(xs0, a)
            xs1 = b if xs1 is None else max(xs1, b)
            ys0 = y if ys0 is None else min(ys0, y)
            ys1 = y if ys1 is None else max(ys1, y)
    return px, xs0, xs1 - 1, ys0, ys1 - 1


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('file')
    ap.add_argument('--rgb', default='86,30,152',
                    help='the colour to look for, R,G,B as seen in the '
                         'capture itself (default: the KADMAR.07 boss robe)')
    ap.add_argument('--tol', type=int, default=24,
                    help='sum of absolute component differences')
    ap.add_argument('--min', type=int, default=800,
                    help='smallest blob to report, pixels')
    ap.add_argument('--t0', type=float, default=0.0)
    ap.add_argument('--t1', type=float, default=None)
    ap.add_argument('--fps', type=int, default=2,
                    help='samples per second of the file (default 2)')
    ap.add_argument('--hud', type=int, default=1500,
                    help='non-black pixels in a HUD corner that counts as '
                         'drawn (default 1500)')
    ap.add_argument('--exclude', action='append', default=[],
                    metavar='X0,Y0,X1,Y1', help='rectangle to clear before '
                    'searching (repeatable; default: the two HUD corners)')
    ap.add_argument('--every', action='store_true',
                    help='also print one line per sample')
    args = ap.parse_args()

    rgb = np.array([int(v) for v in args.rgb.split(',')], dtype=np.int16)
    excl = ([tuple(int(v) for v in e.split(',')) for e in args.exclude] or
            [(0, 250, 110, 400), (540, 250, 640, 400)])
    span = (args.t1 - args.t0) if args.t1 is not None else None

    runs = []
    cls_runs = []
    nG = nC = 0
    for i, img in enumerate(frames(args.file, args.t0, span, args.fps)):
        img = img.astype(np.int16)
        mask = np.abs(img - rgb).sum(axis=2) <= args.tol
        for x0, y0, x1, y1 in excl:
            mask[y0:y1, x0:x1] = False
        hud = []
        for x0, y0, x1, y1 in ((0, 250, 110, 400), (540, 250, 640, 400)):
            hud.append(int((img[y0:y1, x0:x1].max(axis=2) > 40).sum()))
        cls = 'G' if min(hud) >= args.hud else 'C'
        if cls == 'G':
            nG += 1
        else:
            nC += 1
            if cls_runs and cls_runs[-1][1] == i - 1:
                cls_runs[-1][1] = i
            else:
                cls_runs.append([i, i])
        blob = largest_blob(mask)
        px, x0, x1 = 0, 0, 0
        if blob and blob[0] >= args.min:
            px, x0, x1 = blob[0], blob[1], blob[2]
            if runs and runs[-1][1] == i - 1:
                runs[-1][1] = i
                runs[-1][2].append((px, x0, x1, cls))
            else:
                runs.append([i, i, [(px, x0, x1, cls)]])
        if args.every:
            print('%8.1f %s hud %5d/%5d blob %6s x %4s..%4s' %
                  (args.t0 + i / args.fps, cls, hud[0], hud[1],
                   px or '-', x0 if px else '-', x1 if px else '-'))
    print('# %d samples with the HUD drawn, %d without' % (nG, nC))
    print('# segments with NO HUD (a cutscene, a card, a scripted scene,'
          ' a black room)')
    for st, en in cls_runs:
        print('%8.1f %8.1f %5.2f' % (args.t0 + st / args.fps,
                                     args.t0 + (en + 1) / args.fps,
                                     (en - st + 1) / args.fps))
    print('# runs of at least one sample with a blob >= %d px '
          '(start end s maxpx xmin xmax class)' % args.min)
    for st, en, samples in runs:
        cls = ''.join(sorted(set(s[3] for s in samples)))
        print('%8.1f %8.1f %5.2f %6d %5d %5d %s' %
              (args.t0 + st / args.fps, args.t0 + (en + 1) / args.fps,
               (en - st + 1) / args.fps, max(s[0] for s in samples),
               min(s[1] for s in samples), max(s[2] for s in samples), cls))


if __name__ == '__main__':
    main()
