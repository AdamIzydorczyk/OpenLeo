#!/usr/bin/env python3
"""What surface is the clip's lion really standing on, in WORLD units.

PR-138's G-13 pass.  This tool re-derives the standing-surface
clusters of a video_camera_read.py read under the two rules the
first pass (PR-136) broke:

  1. BOTH coordinates are world: x = paws[0] + camx,
     y = paws[6] + camy.  The first pass mixed screen paws-x with
     world paws-y, which turned camera false-matches into phantom
     surfaces "7-51 units above the .PLT".
  2. clusters use tier-1 frames ONLY - frames where two DISTINCT
     placed static images agree on one camera (video_camera_read.py
     already grades this).  A tier-2 (single uncontested landmark)
     read can be a false match, and a false camx turns any paws
     into a world-x claim that is really a screen-x: the tell is a
     pile of different "surfaces" in one narrow x band.

Windows: "the lion's world y is constant over +-1 sampled frame
(+-0.1 s at 60 fps)".  LIMIT (named, not fixed): such a window
also matches a JUMP APEX - the walk split does not separate them
because a jump travels horizontally.  Confirmed on ZARIA.01
frames 466..475 (PR-138 record).  Horizontal travel over the
window is reported per cluster so the reader can judge.

Each cluster is cross-checked against every .PLT segment evaluated
at the cluster centre x, and against every static-image top edge
of the landmark atlas within 14 units of the row.

Usage: ground_surfaces.py READ.jsonl ATLAS.json PLT.txt
  READ.jsonl  video_camera_read.py output (any stride; the +-0.1 s
              window is defined in FRAME INDICES, step 6).
  ATLAS.json  level_landmarks.py atlas (for the top-edge check).
  PLT.txt     the level's .plt text, dumped with tools/re/llrs_cat1
              (lines "<extra-vertex-count> x,y;x,y[;...]", '!' as
              an alternative separator, exactly as ll_plt_parse).
Stdlib only; prints a frame census then one line per cluster of
>= 8 samples.  Compares authored data with watched video; it
changes nothing in the port.
"""
import json
import statistics
import sys

CAM, ATLAS, PLT = sys.argv[1], sys.argv[2], sys.argv[3]

rows = [json.loads(l) for l in open(CAM) if l.startswith('{')]
rows = [r for r in rows if 'camx' in r and r.get('paws') and
        len(r['paws']) == 7 and -2 <= r['camx'] <= 1922 and
        -2 <= r['camy'] <= 502]
t1 = [r for r in rows if r.get('tier') == 1 and not r.get('ambiguous')
      and r.get('lion')]
t2 = [r for r in rows if r.get('tier') != 1 and not r.get('ambiguous')
      and r.get('lion')]
print('read frames: total %d, tier-1 unambiguous %d, tier-2 %d' %
      (len(rows), len(t1), len(t2)))

half = 1  # window = +-1 sampled frame at 6-frame index steps
idx = {r['i']: r for r in t1}
pts = []
for r in t1:
    ks = [r['i'] + k * 6 for k in range(-half, half + 1)]
    if not all(k in idx for k in ks):
        continue
    w = [idx[k] for k in ks]
    wy = [x['lion'][1] + x['camy'] for x in w]
    if max(wy) - min(wy) > 3:
        continue                       # not a constant-y window
    wx = [x['lion'][0] + x['camx'] for x in w]
    pts.append((round(r['paws'][0] + r['camx']),
                round(r['paws'][6] + r['camy']),
                abs(wx[-1] - wx[0]), r['i']))
pts.sort()
clust = []
for sx, sy, dx, i in pts:
    hit = None
    for c in clust:
        if abs(c['y'] - sy) <= 4 and c['x0'] - 12 <= sx <= c['x1'] + 12:
            hit = c
            break
    if hit:
        hit['n'] += 1
        hit['x0'] = min(hit['x0'], sx)
        hit['x1'] = max(hit['x1'], sx)
        hit['ys'].append(sy)
        hit['dx'].append(dx)
        hit['frames'].append(i)
    else:
        clust.append({'y': sy, 'x0': sx, 'x1': sx, 'n': 1, 'ys': [sy],
                      'dx': [dx], 'frames': [i]})
clust = [c for c in clust if c['n'] >= 8]

segs = []
for ln in open(PLT):
    ln = ln.strip()
    if not ln or ln.isdigit():
        continue
    cnt, rest = ln.split(' ', 1)
    P = []
    for tok in rest.replace('!', ';').split(';'):
        if tok:
            x, y = tok.split(',')
            P.append((int(x), int(y)))
    for a, b in zip(P, P[1:]):
        segs.append((int(cnt), a, b))


def segs_at(x):
    out = []
    for k, a, b in segs:
        if min(a[0], b[0]) <= x <= max(a[0], b[0]):
            out.append((a[1] + (b[1] - a[1]) * (x - a[0]) / (b[0] - a[0]),
                        k))
    return sorted(out)


items = json.load(open(ATLAS))['items']

clust.sort(key=lambda c: c['x0'])
print('cluster (WORLD coords, tier-1 only; see LIMITS in the header)')
for c in clust:
    y = int(statistics.median(c['ys']))
    xm = (c['x0'] + c['x1']) // 2
    S = segs_at(xm)
    best = min(S, key=lambda t: abs(y - t[0])) if S else None
    btxt = ('nearest plt seg y%4d d%+3d (n=%s pts)' %
            (best[0], y - best[0], best[1])) if best else 'no seg at x'
    I = []
    for it in items.values():
        x0, y0, sw, sh = it['wx'], it['wy'], it['sw'], it['sh']
        if x0 - 10 <= xm <= x0 + sw + 10 and abs(y - y0) <= 14:
            I.append('%s top=%d d%+d' % (it['art_name'], y0, y - y0))
    walk = sum(1 for v in c['dx'] if v >= 15)
    print('WX %4d..%-4d y %3d n=%4d walk=%3d fr[%d..%d] | %s | %s' %
          (c['x0'], c['x1'], y, c['n'], walk,
           min(c['frames']), max(c['frames']), btxt,
           '; '.join(I) or '-'))
