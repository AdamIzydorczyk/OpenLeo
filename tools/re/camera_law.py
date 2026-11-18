#!/usr/bin/env python3
"""Turn a landmark camera read (video_camera_read.py) into shots and law
numbers for the camera itself.

A shot is a run of frames with no reading gap and no camera jump larger than
JUMP units between consecutive frames; the engine's camera cannot move that
fast, so such a jump is a boundary (a cut in the clip, or a lost match) and is
reported rather than integrated as motion.

The lead.  The port's follow_camx keeps the lion at a fixed screen x:
camx = p.x - cam_ox, so cam_ox IS the lion's x on the 320-unit view.  Both
quantities are read here directly - the camera from the landmarks, the lion
from his colour mask - so the lead is measured, not inferred.  Because the
camera value is an integer and the lion's centroid is not, a lead HELD for a
whole window (the lion's screen step equals the camera's step) is the evidence
of the law; the plain scatter of the lead mixes turnarounds, clamps at the
world edges and shots where the camera is standing still.

Rates are measured over a contiguous WIN-frame window, not frame to frame: at
60 fps a +-1 unit quantisation of the camera read would otherwise put the
rate floor at 30 units/s and hide everything slower.

The vertical anchor.  The port's follow_camy holds a grounded lion's contact
line at CAM_FEET_Y on the view; the same quantity read here is
(contact world y) - camy, and the paws colour class is the feature that
constant was measured against.  Frames are separated into a level run (his
world y constant), an arc (it rises and comes back), and the rest.

    ./camera_law.py READ.jsonl [T0] [T1]

Tier filter: frames whose camera read is contested by the level's duplicated
art are excluded, as are frames below T0 or after T1.  Environment:
LEVEL/ATLAS name the level and its landmark atlas (for the level's own camera
range and coverage), and PLT=<file> (the level's .PLT record, dumped with
tools/re/llrs_cat1) adds the ground cross-check at the end.
"""
import json
import os
import re
import sys

F = 60.0                 # the reference clips are constant-rate 60 fps
JUMP = 20                # units of camera motion between frames = a boundary
GAP = 30                 # frames without a reading = a shot boundary
WALK = 25.0              # world units/s to call it a walk (ZARIA's cap is 100)
WIN = 13                 # frames, contiguous, for any rate measurement
TIER = 1                 # use tier >= this for the law numbers


def pct(v, p):
    if not v:
        return float('nan')
    v = sorted(v)
    return v[min(len(v) - 1, int(p * len(v)))]


def modes(vals, n=8):
    c = {}
    for v in vals:
        c[int(round(v))] = c.get(int(round(v)), 0) + 1
    return ' '.join('%d:%d' % kv for kv in
                    sorted(c.items(), key=lambda kv: -kv[1])[:n])


rows = [json.loads(l) for l in open(sys.argv[1]) if l.startswith('{')]
t0 = float(sys.argv[2]) if len(sys.argv) > 2 else 0.0
t1 = float(sys.argv[3]) if len(sys.argv) > 3 else 1e9
alln = len(rows)
# The level's own geometry: the camera cannot be outside 0..world-view, so a
# read that says so is a template matching something that is not that
# placement.  Counted, and not used for any number below.
adir = os.environ.get('ATLAS', '/tmp/opencode/landmarks')
lvl = os.environ.get('LEVEL', 'ZARIA.01')
atl = json.load(open('%s/%s/atlas.json' % (adir, lvl)))
CW, CH = atl['cw'], atl['ch']
YMAX = atl['cam_ymax']
XMAX = atl['cam_xmax']


TOL = 2                        # units: the read's own +-1 plus one spare


def impossible(r):
    return not -TOL <= r['camx'] <= XMAX + TOL or not -TOL <= r['camy'] \
        <= YMAX + TOL


read = [r for r in rows if 'camx' in r]
bad = [r for r in read if impossible(r)]
far = sum(1 for r in read if not -8 <= r['camx'] <= XMAX + 8
          or not -8 <= r['camy'] <= YMAX + 8)
read = [r for r in read if not impossible(r)]
good = [r for r in read if r['tier'] >= TIER and not r['ambiguous']
        and t0 <= r['t'] <= t1]
print('frames sampled %d; no landmark in view %d; a camera read %d (%d backed'
      ' by two different art names agreeing on one camera, %d by a single'
      ' landmark); ambiguous '
      '(duplicated art supports two cameras equally) %d; outside the '
      'camera range the level\'s own geometry allows %d (not used)'
      % (alln, alln - len(read) - len(bad), len(read),
         sum(1 for r in read if r['tier'] == 1),
         sum(1 for r in read if r['tier'] == 2),
         sum(1 for r in read if r['ambiguous']), len(bad)))
print('used below: %d frames' % len(good))
print('%d of the excluded reads are off the level\'s own camera range by more '
      'than 8 units: the level places animated objects with the SAME art as '
      'its static images, and its backdrop can match a template too, so the '
      'match alone is not proof - the level\'s geometry is' % far)
print('camera_x read: p05=%d p50=%d p95=%d  camera_y: p05=%d p50=%d p95=%d '
      'max=%d' % (pct([r['camx'] for r in good], .05),
                  pct([r['camx'] for r in good], .5),
                  pct([r['camx'] for r in good], .95),
                  pct([r['camy'] for r in good], .05),
                  pct([r['camy'] for r in good], .5),
                  pct([r['camy'] for r in good], .95),
                  max(r['camy'] for r in good)))

# Coverage, honestly split: a bucket with no reading is either the level giving
# no landmark there (a method hole, to be filled with more anchors) or the
# clip's camera never being there (a fact about the clip).  Which landmark the
# level itself would show at a camera is pure geometry from the atlas.
print('coverage by camera_x bucket, READ frames / WOULD-BE-VISIBLE placements '
      '(the level\'s own %d anchors at that camera_x, any camera_y; 0 in the '
      'second number = a method hole this read cannot fill):'
      % len(atl['items']))
have = {}
for r in good:
    have[r['camx'] // 100 * 100] = have.get(r['camx'] // 100 * 100, 0) + 1
line = []
for b in range(0, (atl['cam_xmax'] // 100 + 1) * 100, 100):
    # an anchor at ax is inside the view for any cam in [ax-(CW-w), ax]; on
    # camera_y the same question has an answer when [ay-(CH-h), ay] meets
    # [0, cam_ymax]
    vis = sum(1 for it in atl['items'].values()
              if max(b, it['ax'] - (CW - it['w'])) <= min(b + 99, it['ax'])
              and max(0, it['ay'] - (CH - it['h'])) <= min(YMAX, it['ay']))
    line.append('%d:%d/%d' % (b, have.get(b, 0), vis))
print('  ' + ' '.join(line))

shots, cur = [], []
for r in good:
    if cur and (r['i'] - cur[-1]['i'] > GAP
                or abs(r['camx'] - cur[-1]['camx']) > JUMP
                or abs(r['camy'] - cur[-1]['camy']) > JUMP):
        shots.append(cur)
        cur = []
    cur.append(r)
if cur:
    shots.append(cur)
long = [s for s in shots if len(s) >= 60]
print('shots: %d boundaries, %d runs of >=60 readings' % (len(shots) - 1,
                                                          len(long)))
idx = {r['i']: r for r in good}          # frame index -> row, for windows
shotof = {}
for n, s in enumerate(shots):
    for r in s:
        shotof[r['i']] = n


def window(r, half=WIN // 2):
    """r plus the frames half on each side: all consecutive, all inside the
    same shot (a window that crossed a boundary would measure a cut as
    motion), or None"""
    a, b = r['i'] - half, r['i'] + half
    if all(k in idx and shotof[k] == shotof[r['i']]
           for k in range(a, b + 1)):
        return [idx[k] for k in range(a, b + 1)]
    return None


print('\n%8s %8s %6s  %-14s %-11s %s'
      % ('t0', 't1', 'fr', 'camera x', 'camera y', 'lead of the lion while '
         'he walks right / left (n, median, p95)'))
lead = {'right': [], 'left': [], 'idle': []}
hold = {'right': [], 'left': []}
lionv, camrate, lag, catchup = [], [], [], []
for s in long:
    cx = [r['camx'] for r in s]
    cy = [r['camy'] for r in s]
    loc = {'right': [], 'left': [], 'idle': []}
    for i in range(1, len(s) - 1):
        a, b, c = s[i - 1], s[i], s[i + 1]
        if not (a.get('lion') and b.get('lion') and c.get('lion')):
            continue
        dt = c['i'] - a['i']
        if dt > 6:
            continue
        vx = ((c['lion'][0] + c['camx']) - (a['lion'][0] + a['camx'])) / dt * F
        kind = 'right' if vx > WALK else 'left' if vx < -WALK else 'idle'
        loc[kind].append(b['lion'][0])
        lead[kind].append(b['lion'][0])
    for r in s:
        w = window(r)
        if not w or not all(x.get('lion') for x in w):
            continue
        dt = w[-1]['i'] - w[0]['i']
        vx = ((w[-1]['lion'][0] + w[-1]['camx'])
              - (w[0]['lion'][0] + w[0]['camx'])) / dt * F
        dv = (w[-1]['camx'] - w[0]['camx']) / dt * F
        if abs(vx) > WALK:
            lionv.append(vx)
            camrate.append(dv)
            lag.append(dv - vx)
            if abs((w[-1]['lion'][0] - w[0]['lion'][0])
                   - (w[-1]['camx'] - w[0]['camx'])) <= 1.5:
                hold['right' if vx > 0 else 'left'].append(r['lion'][0])
        elif abs(dv) > 3:
            catchup.append(dv)
    print('%8.2f %8.2f %6d  %5d..%-8d %5d..%-5d n=%4d p50=%5.0f p95=%5.0f'
          '      n=%4d p50=%5.0f p95=%5.0f'
          % (s[0]['t'], s[-1]['t'], len(s), min(cx), max(cx), min(cy),
             max(cy), len(loc['right']), pct(loc['right'], .5),
             pct(loc['right'], .95), len(loc['left']), pct(loc['left'], .5),
             pct(loc['left'], .95)))

print('\nlead = the lion\'s screen x, in world units of the view (the port\'s'
      ' cam_ox is exactly this quantity):')
for k, lbl in (('right', 'lion walks right'), ('left', 'lion walks left'),
               ('idle', 'lion stands still')):
    print('  %-18s n=%5d  lead p05=%5.0f p25=%5.0f p50=%5.0f p75=%5.0f '
          'p95=%5.0f' % (lbl, len(lead[k]), pct(lead[k], .05),
                         pct(lead[k], .25), pct(lead[k], .5),
                         pct(lead[k], .75), pct(lead[k], .95)))
print('the lead HELD over a whole %d-frame window (the lion\'s screen step ='
      ' the camera\'s step): that is the value the game keeps, and it is one'
      ' number per frame, not a scatter' % WIN)
for k, lbl in (('right', 'lion walks right'), ('left', 'lion walks left')):
    print('  %-18s n=%5d  held lead values: %s'
          % (lbl, len(hold[k]), modes(hold[k], 12)))
print('\nspeeds over the %d-frame window, world units/s:' % WIN)
print('  lion, walking        n=%5d  p05=%6.1f p25=%6.1f p50=%6.1f '
      'p75=%6.1f p95=%6.1f' % (len(lionv), pct(lionv, .05), pct(lionv, .25),
                               pct(lionv, .5), pct(lionv, .75),
                               pct(lionv, .95)))
print('  camera, tracking     n=%5d  p05=%6.1f p25=%6.1f p50=%6.1f '
      'p75=%6.1f p95=%6.1f max=%6.1f' % (len(camrate), pct(camrate, .05),
                                         pct(camrate, .25), pct(camrate, .5),
                                         pct(camrate, .75), pct(camrate, .95),
                                         max(map(abs, camrate)) if camrate
                                         else float('nan')))
print('  camera minus lion    n=%5d  p05=%6.1f p50=%6.1f p95=%6.1f  (0 = the'
      ' lead is held; >0 = the camera overtakes)'
      % (len(lag), pct(lag, .05), pct(lag, .5), pct(lag, .95)))
print('  camera while the lion stands still (the catch-up ease the port '
      'models at 6 units per iteration) n=%5d p50=%6.1f p95=%6.1f max=%6.1f'
      % (len(catchup), pct(catchup, .5), pct(catchup, .95),
         max(map(abs, catchup)) if catchup else float('nan')))

# ---- the vertical law ------------------------------------------------------
# His world y = paws row + camy, both read.  A run of constant world y is a
# level surface; in those frames (world y) - camy IS the port's CAM_FEET_Y
# anchor, and camy's own value says whether the world clamp hides it.
seen = [r for r in good if r.get('lion')]
print('\nlion in the view: %d frames with a body blob, %d with the paws '
      'colour near him' % (len(seen), sum(1 for r in seen if r.get('paws'))))
onplane, arcs, camy_during_arc = [], [], []
ARC = 31                       # a jump is ~20-30 frames at 60 fps; 13 is not


def arcwindow(r, half=ARC // 2):
    a, b = r['i'] - half, r['i'] + half
    if all(k in idx and shotof[k] == shotof[r['i']]
           for k in range(a, b + 1)):
        return [idx[k] for k in range(a, b + 1)]
    return None


for r in seen:
    w = window(r)
    if not w or not all(x.get('paws') for x in w):
        continue
    wy = [x['paws'][6] + x['camy'] for x in w]           # his paws, world
    if max(wy) - min(wy) <= 3:
        onplane.append((wy[0], r['paws'][6], r['camy'],
                        r['camy'] >= atl['cam_ymax'] - 1,
                        r['paws'][0] + r['camx'], r['lion'][0] + r['camx']))
        continue
    # an arc: he starts and ends on the same line and is >=8 units above it
    # somewhere in the window
    if (max(wy) - min(wy) >= 8 and abs(wy[0] - wy[-1]) <= 3
            and wy[len(wy) // 2] <= min(wy) + 3):
        arcs.append((max(wy) - min(wy), w[-1]['camy'] - w[0]['camy'],
                     w[-1]['camx'] - w[0]['camx']))
        continue
    w = arcwindow(r)
    if not w or not all(x.get('paws') for x in w):
        continue
    wy = [x['paws'][6] + x['camy'] for x in w]
    if (max(wy) - min(wy) >= 8 and abs(wy[0] - wy[-1]) <= 3
            and wy[len(wy) // 2] <= min(wy) + 3):
        arcs.append((max(wy) - min(wy), w[-1]['camy'] - w[0]['camy'],
                     w[-1]['camx'] - w[0]['camx']))
print('level surfaces (his world y constant across the window): %d frames'
      % len(onplane))
if onplane:
    print('  surface world y the clip shows him standing on: %s'
          % modes([x[0] for x in onplane], 10))
    print('  CAM_FEET_Y read on those frames (his paws row on the view): '
          'p05=%.0f p25=%.0f p50=%.0f p75=%.0f p95=%.0f  modes: %s'
          % (pct([x[1] for x in onplane], .05), pct([x[1] for x in onplane],
                                                    .25),
             pct([x[1] for x in onplane], .5), pct([x[1] for x in onplane],
                                                   .75),
             pct([x[1] for x in onplane], .95), modes([x[1] for x in onplane],
                                                      10)))
    clamped = sum(1 for x in onplane if x[3])
    print('  of those, %d frames have the camera at its world clamp (%d), '
          'where the anchor is not what sets camy'
          % (clamped, atl['cam_ymax']))
    print('  unclamped frames: anchor p50=%.0f modes: %s'
          % (pct([x[1] for x in onplane if not x[3]], .5),
             modes([x[1] for x in onplane if not x[3]], 10)))
print('jump arcs, %d-frame window (his world y goes up by >=8 and comes back '
      'to it): %d' % (ARC, len(arcs)))
if arcs:
    print('  apex height, world units: p05=%.0f p50=%.0f p95=%.0f'
          % (pct([a[0] for a in arcs], .05), pct([a[0] for a in arcs], .5),
             pct([a[0] for a in arcs], .95)))
    print('  camera_y change over the arc: p05=%.0f p50=%.0f p95=%.0f  '
          '(0 = the view HOLDS through the jump, as the port models it)'
          % (pct([a[1] for a in arcs], .05), pct([a[1] for a in arcs], .5),
             pct([a[1] for a in arcs], .95)))
    print('  camera_x change over the arc: p05=%.0f p50=%.0f p95=%.0f'
          % (pct([a[2] for a in arcs], .05), pct([a[2] for a in arcs], .5),
             pct([a[2] for a in arcs], .95)))

# Independent check, from the level's OWN data rather than from the video:
# PLT=zaria_a.plt.txt (the level block's ZARIA_A.PLT record, dumped with
# tools/re/llrs_cat1) is the ground the game itself places the lion on.  If
# the surfaces the clip shows him standing on are those polylines, the whole
# vertical chain - the world anchoring, the camera_y read, the feet row - is
# confirmed by data that has nothing to do with the read.
plt = os.environ.get('PLT')
if plt and onplane:
    segs = []
    for ln in open(plt):
        # the same grammar the port's own ll_plt_parse uses (ll_play.c:34):
        # every "x,y" pair on the line, in order, counts and headers ignored;
        # ';' and '!' are the same separator
        pts = [(int(a), int(b)) for a, b in re.findall(r'(-?\d+),(-?\d+)', ln)]
        for a, b in zip(pts, pts[1:]):
            if a[0] != b[0]:
                segs.append((min(a[0], b[0]), max(a[0], b[0]), a, b))
    ok = near = miss = 0
    d = []
    for sy, row, camy, cl, px, lx in onplane:
        x = px
        cand = [a[1] + (b[1] - a[1]) * (x - a[0]) / (b[0] - a[0])
                for a0, a1, a, b in segs if a0 <= x <= a1]
        if not cand:
            miss += 1
            continue
        e = min((abs(sy - c), sy - c) for c in cand)[1]
        d.append(e)
        if abs(e) <= 2:
            ok += 1
        elif abs(e) <= 5:
            near += 1
    print('%d of these frames are on a %d-segment authored .PLT ground at '
          'their world x: the measured surface equals it within 2 units in %d '
          'frames, within 5 in %d more, and at this x the level has no '
          'ground polyline at all in %d'
          % (len(onplane), len(segs), ok, near, miss))
    if d:
        print('  measured surface minus authored ground: p05=%.1f p50=%.1f '
              'p95=%.1f' % (pct(d, .05), pct(d, .5), pct(d, .95)))
