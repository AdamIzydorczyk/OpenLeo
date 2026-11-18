#!/usr/bin/env python3
"""Level landmark atlas: match templates for the level's OWN static art.

Run the level scene through ./ll_level twice at a camera that puts each
authored static image placement ("<LEAF>,<name>.seq,<x>,<y>" - the 3-comma
placement grammar, drawn by the port at the authored point with no shape
record) into the 320x200 view: once with the placed objects composited, once
with LEO_LEVEL_NO_OBJ (the bare background: the backdrop PCX plus the PCM
layers).  The difference inside that placement's own box IS the placement's
pixels and its alpha mask; the composited render gives its art.

Three cuts make the patch usable as a match template for a video frame:
pixels of any OTHER placement whose box overlaps are removed (that art is
animated in the game); pixels that change when the camera moves 8 units
sideways are removed (they blend with what is behind the placement, so the
patch is not the placement's own); pixels that change when the camera moves 4
units down are removed the same way.  What is left is the placement's opaque,
background-independent art, anchored to the authored world position.

Only the static-image grammar is anchored that way.  The .MOV placements draw
their art at the shape-margin offset their .SEQ record carries, so their
world anchor is NOT the authored point: they are counted and not used.

    ./level_landmarks.py [LEVEL] [OUTDIR]        # default ZARIA.01

Writes OUTDIR/atlas.json plus tpl_NN.npz (art + mask) and prints the census
it builds the atlas from.  Paths are hard-coded per tools/re/README.md.
"""
import json
import os
import re
import subprocess
import sys

import numpy as np

REPO = '/home/aizy/Projects/LL_RECOMPILE'
DATA = '/tmp/opencode/iso_stage/LEO'
HD = '/tmp/opencode/hd_files'
LEVEL = sys.argv[1] if len(sys.argv) > 1 else 'ZARIA.01'
OUT = sys.argv[2] if len(sys.argv) > 2 else '/tmp/opencode/landmarks/' + LEVEL
INFLATE = 8                     # margin of another placement's cut-out box
MINPX = 150                     # opaque px a patch needs to be matchable art

os.makedirs(OUT, exist_ok=True)

PLACE = re.compile(r'obj ([A-Za-z0-9/._]+)\[([a-z ]+)\].*? art ([A-Z0-9.()]+),'
                   r' (\d+) state\(s\), bbox (\d+)x(\d+)'
                   r' at \((-?\d+),(-?\d+)\)')
STAT = re.compile(r'obj (PICT/[a-z0-9]+\.seq)\[static image\]: art'
                  r' ([A-Z0-9.]+), 0 state\(s\), bbox (\d+)x(\d+)'
                  r' at \((-?\d+),(-?\d+)\)')
GEOM = re.compile(r'world (\d+)x(\d+) .*view (\d+)x(\d+), cam -?\d+/(\d+)')


def render(ppm, cam, camy, noobj):
    """one ./ll_level scene frame at a chosen camera; returns the clamps"""
    env = dict(os.environ, DISPLAY='', SDL_VIDEODRIVER='dummy',
               SDL_AUDIODRIVER='dummy', LEO_HD_DIR=HD, LEO_HELP_SECS='0',
               LEO_LEVEL_SECS='1', LEO_LEVEL_CAM=str(cam),
               LEO_LEVEL_CAMY=str(camy), LEO_LEVEL_PPM=ppm,
               LEO_OBJ_ALL='1')      # else the proof listing is bounded to 24
    if noobj:
        env['LEO_LEVEL_NO_OBJ'] = '1'
    else:
        env.pop('LEO_LEVEL_NO_OBJ', None)
    r = subprocess.run(['./ll_level', DATA, LEVEL, '--no-audio'],
                       cwd=REPO, env=env, capture_output=True, timeout=120)
    if r.returncode:
        raise SystemExit('ll_level rc=%d\n%s'
                         % (r.returncode, r.stderr.decode()[:400]))
    return r.stdout.decode(errors='replace')


def ppm(p):
    b = open(p, 'rb').read()
    m = re.match(rb'P6\s+(\d+)\s+(\d+)\s+255\s', b)
    w, h = int(m.group(1)), int(m.group(2))
    return np.frombuffer(b[m.end():], dtype=np.uint8).reshape(h, w, 3)


def rect_of(sw, sh, x, y, cam, camy):
    """the box the renderer draws at this camera, clipped to the canvas"""
    x0, y0 = max(0, x - cam), max(0, y - camy)
    x1, y1 = min(CW, x - cam + sw), min(CH, y - camy + sh)
    return x0, y0, x1, y1


head = render('%s/ra.ppm' % OUT, 0, 0, False)
g = GEOM.search(head)
if not g:
    raise SystemExit('ll_level did not report this level\'s world/view'
                     ' geometry - not a playable level, or a data hole')
WW, WH, CW, CH, CAM_XMAX = (int(x) for x in g.groups())
CAM_YMAX = max(WH - CH, 0)
print('%s: world %dx%d, view %dx%d, camera x 0/%d y 0/%d'
      % (LEVEL, WW, WH, CW, CH, CAM_XMAX, CAM_YMAX))

allplaces = [(int(m.group(5)), int(m.group(6)), int(m.group(7)),
              int(m.group(8))) for m in PLACE.finditer(head)]
onestate = sum(1 for m in PLACE.finditer(head) if m.group(4) == '1')
items = [(m.group(1), m.group(2), int(m.group(3)), int(m.group(4)),
          int(m.group(5)), int(m.group(6))) for m in STAT.finditer(head)]
print('placements in the scene: %d total, %d of them one-state .MOV; the %d'
      ' static image(s) are the anchored ones' % (len(allplaces), onestate,
                                                 len(items)))
print('\n%-26s %-9s %-8s %-12s %-9s %-9s %7s %10s %8s'
      % ('placement', 'authored', 'bbox', 'cam/camy', 'anchor', 'template',
         'opaque', 'blend cut', 'camy cut'))
atlas, n = {}, 0
for leaf, art, sw, sh, x, y in items:
    key = '%s@%d,%d' % (leaf, x, y)
    if sw > CW:
        print('%-26s %5d,%-4d %4dx%-4d skipped: wider than the view'
              % (key[:26], x, y, sw, sh))
        continue
    cam = min(max(x - CW // 2 + sw // 2, 0), CAM_XMAX)
    camy = min(max(y - CH // 2 + sh // 2, 0), CAM_YMAX)
    rx0, ry0, rx1, ry1 = rect_of(sw, sh, x, y, cam, camy)
    vis = (rx1 - rx0) * (ry1 - ry0)
    if vis < sw * sh // 2:
        print('%-26s %5d,%-4d %4dx%-4d skipped: no camera of this level'
              ' shows half of it (the world or the view cuts it)'
              % (key[:26], x, y, sw, sh))
        continue
    head = render('%s/ra.ppm' % OUT, cam, camy, False)
    render('%s/rb.ppm' % OUT, cam, camy, True)
    A = ppm('%s/ra.ppm' % OUT).astype(np.int16)
    B = ppm('%s/rb.ppm' % OUT).astype(np.int16)
    d = np.abs(A - B).sum(axis=2)
    rx0, ry0, rx1, ry1 = rect_of(sw, sh, x, y, cam, camy)
    if rx1 - rx0 < 6 or ry1 - ry0 < 6:
        print('%-26s %5d,%-4d %4dx%-4d cam %d camy %d: nothing in the view'
              % (key[:26], x, y, sw, sh, cam, camy))
        continue
    mask = d[ry0:ry1, rx0:rx1] > 0
    for (ow, oh, ox, oy) in allplaces:
        if (ox, oy) == (x, y):
            continue
        a, b, c, e = rect_of(ow + INFLATE, oh + INFLATE, ox - INFLATE,
                             oy - INFLATE, cam, camy)
        if c > a and e > b:
            mask[max(0, b - ry0):max(0, e - ry0),
                 max(0, a - rx0):max(0, c - rx0)] = False
    if not mask.any():
        print('%-26s %5d,%-4d %4dx%-4d cam %d camy %d: draws nothing there'
              % (key[:26], x, y, sw, sh, cam, camy))
        continue
    if int(mask.sum()) < MINPX:
        print('%-26s %5d,%-4d %4dx%-4d cam %d camy %d: only %d opaque px (the'
              ' scene paints over it), too small to match'
              % (key[:26], x, y, sw, sh, cam, camy, int(mask.sum())))
        continue

    dc = -8 if cam + 8 > CAM_XMAX else 8
    if rx0 - dc < 0 or rx1 - dc > CW:
        print('%-26s %5d,%-4d %4dx%-4d skip: the view edge blocks the'
              ' background probe' % (key[:26], x, y, sw, sh))
        continue
    render('%s/rc.ppm' % OUT, cam + dc, camy, False)
    C = ppm('%s/rc.ppm' % OUT).astype(np.int16)
    if np.array_equal(A, C):
        print('%-26s %5d,%-4d %4dx%-4d skip: the renderer clamped the probe'
              ' camera' % (key[:26], x, y, sw, sh))
        continue
    same = np.all(A[ry0:ry1, rx0:rx1] == C[ry0:ry1, rx0 - dc:rx1 - dc],
                  axis=2)
    blended = int(np.count_nonzero(mask & ~same))
    mask &= same
    if not mask.any():
        print('%-26s %5d,%-4d %4dx%-4d cam %d camy %d: every pixel blends'
              % (key[:26], x, y, sw, sh, cam, camy))
        continue
    vcut = 0
    render('%s/rd.ppm' % OUT, cam, camy + 4, False)
    D = ppm('%s/rd.ppm' % OUT).astype(np.int16)
    if not np.array_equal(A, D) and ry0 - 4 >= 0:
        vsame = np.all(A[ry0:ry1, rx0:rx1] == D[ry0 - 4:ry1 - 4, rx0:rx1],
                       axis=2)
        vcut = int(np.count_nonzero(mask & ~vsame))
        mask &= vsame
    if not mask.any():
        print('%-26s %5d,%-4d %4dx%-4d cam %d camy %d: every pixel moves'
              % (key[:26], x, y, sw, sh, cam, camy))
        continue
    ys, xs = np.nonzero(mask)
    dx0, dx1, dy0, dy1 = (int(xs.min()), int(xs.max()) + 1, int(ys.min()),
                          int(ys.max()) + 1)
    mask = mask[dy0:dy1, dx0:dx1]
    tpl = A[ry0 + dy0:ry0 + dy1, rx0 + dx0:rx0 + dx1]
    ax, ay = cam + rx0 + dx0, camy + ry0 + dy0     # the patch's world anchor
    n += 1
    fn = 'tpl_%02d.npz' % n
    np.savez('%s/%s' % (OUT, fn), art=tpl.astype(np.uint8),
             mask=mask.astype(np.uint8))
    atlas[key] = dict(leaf=leaf, art_name=art, wx=x, wy=y, sw=sw, sh=sh,
                      ax=ax, ay=ay, w=int(mask.shape[1]), h=int(mask.shape[0]),
                      opaque=int(mask.sum()), blended=blended, vmoved=vcut,
                      render_cam=cam, render_camy=camy, file=fn)
    print('%-26s %5d,%-4d %4dx%-4d %5d/%-6d %5d,%-4d %6dx%-4d %7d %10d %8d'
          % (key[:26], x, y, sw, sh, cam, camy, ax, ay, mask.shape[1],
             mask.shape[0], int(mask.sum()), blended, vcut))
json.dump(dict(world_w=WW, world_h=WH, cw=CW, ch=CH, cam_xmax=CAM_XMAX,
               cam_ymax=CAM_YMAX, items=atlas),
          open(OUT + '/atlas.json', 'w'), indent=1)
print('\ntemplates: %d of %d static image placement(s) -> %s/atlas.json'
      % (n, len(items), OUT))
