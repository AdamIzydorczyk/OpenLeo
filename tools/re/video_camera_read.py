#!/usr/bin/env python3
"""Read a level's camera position (x, y) out of a reference video, one value
per frame, by anchoring on the level's OWN static image placements.

The templates come from level_landmarks.py: each is one authored static image
of the level, cut from the port's own renderer, so its pixels AND its world
anchor are the level's data.  A template matched at screen offset (u, v) says
its anchor is on screen there, so camera_x = anchor_x - u and camera_y =
anchor_y - v: the level places every one of these on its reference PCM plane,
the 100/100 layer its own wiring `speed` line names, and the landmark builder
probes prove that 1:1 coupling.  The band of the video is cropped and
downscaled to exactly the view, so 1 px of the frame = 1 world unit.

Matching is a masked normalised cross-correlation over every offset the game
could show a placement at, done per landmark on the FFT of the frame.  That
alone is not enough, and this tool does not stop there:

  * Several placements share one .SEQ art (ZARIA.01 places LISC0, LISC1 and
    PLAN3 twice each), so ONE match cannot say where the camera is - it is
    ambiguous by the distance between the copies.
  * A patch of the level's own background art can match a template and say a
    camera position that is plain wrong.

So each match only PROPOSES a camera, and a proposal is judged by what the
other landmarks say at that camera: every landmark whose box lies wholly
inside the view at the proposed camera is read at exactly that offset from
the same correlation surface (a table lookup, no extra cost), and the camera
whose visible landmarks agree most is the one reported.  A frame where only
one landmark is visible at all, and it matches, is an UNCONTESTED reading
(tier 2); a frame where several are visible and several agree is VERIFIED
(tier 1); a frame where the visible landmarks contradict each other is
reported with its scores and not used as a camera value.

    ./video_camera_read.py [--level NAME] [--atlas DIR] [--video FILE]
                           [--t0 S] [--t1 S] [--stride N]
No seek is used: the clip is constant-rate 60 fps, the frame counter is the
clock, and an input seek would land on a keyframe and move every timestamp.
"""
import argparse
import json
import subprocess

import numpy as np

FPS = 60.0
THRESH = 0.55            # masked NCC to call a template present
AGREE = 4                # px of camera agreement two matches must show
WIN = 6                  # px: the prior camera is re-checked in a small window

ap = argparse.ArgumentParser(add_help=True)
ap.add_argument('--level', default='ZARIA.01')
ap.add_argument('--atlas', default=None)
ap.add_argument('--video', required=True)
ap.add_argument('--t0', type=float, default=0.0)
ap.add_argument('--t1', type=float, default=1e9)
ap.add_argument('--stride', type=int, default=1)
ap.add_argument('--raw', action='store_true',
                help='the input is raw rgb24 frames at the view size (no '
                     'crop/scale): what the self-check feeds, so the read is '
                     'tested against cameras the port itself set')
ap.add_argument('--native', action='store_true',
                help='the input is a NATIVE 1280x720 level clip (PR-207: '
                     'exactly 4x the 320x200 view, the game picture fills '
                     'the frame): crop the 1280x800 band (2 rows of 40 px '
                     'are letterboxed black at the bottom) and scale /4, so '
                     '1 frame px = 1 world unit. Auto-detect is NOT used: '
                     'say it explicitly and the numbers are unambiguous.')
A = ap.parse_args()
ADIR = A.atlas or '/tmp/opencode/landmarks/' + A.level

atlas = json.load(open(ADIR + '/atlas.json'))
CW, CH = atlas['cw'], atlas['ch']
XMAX, YMAX = atlas['cam_xmax'], atlas['cam_ymax']
items = atlas['items']


def nsize(n):                        # next FFT-friendly size
    while True:
        m = n
        for p in (2, 3, 5, 7):
            while m % p == 0:
                m //= p
        if m == 1:
            return n
        n += 1


MH = max(v['h'] for v in items.values())
MW = max(v['w'] for v in items.values())
N1, N2 = nsize(CH + MH - 1), nsize(CW + MW - 1)


def padfft(arr):
    p = np.zeros((N1, N2), dtype=np.float32)
    p[:arr.shape[0], :arr.shape[1]] = arr
    return np.fft.rfft2(p, s=(N1, N2))


lands = []
for key, a in items.items():
    z = np.load('%s/%s' % (ADIR, a['file']))
    m = z['mask'].astype(np.float32)
    lum = (z['art'].astype(np.float32)[..., 0] * 0.299
           + z['art'][..., 1] * 0.587 + z['art'][..., 2] * 0.114) * m
    n = float(m.sum())
    mt = float(lum.sum() / n)
    sd = float(np.sqrt(((lum - mt) ** 2 * m).sum() / n))
    lands.append(dict(key=key, art=a['art_name'], ax=a['ax'], ay=a['ay'],
                      w=a['w'], h=a['h'], n=n, mt=mt, sd=sd,
                      Fm=padfft(m), Ftm=padfft(lum)))
print('%s: %d landmark template(s), %s px, view %dx%d, camera x 0/%d y 0/%d'
      % (A.level, len(lands), ' '.join(str(int(l['n'])) for l in lands),
         CW, CH, XMAX, YMAX), flush=True)

cmd = ['ffmpeg', '-v', 'error']
if A.raw:
    cmd += ['-f', 'rawvideo', '-pixel_format', 'rgb24',
            '-video_size', '%dx%d' % (CW, CH), '-i', A.video]
else:
    if A.native:
        # a native 1280x720 clip = the top 180 rows of the 320x200 view at
        # 4x: scale /4, then pad the bottom 20 rows black to full view.
        cmd += ['-i', A.video, '-vf',
                'scale=%d:%d:flags=area,pad=%d:%d:0:0:black'
                % (CW, CH * 720 // 800, CW, CH)]
    else:
        cmd += ['-i', A.video, '-vf',
                'crop=1152:720:0:0,scale=%d:%d:flags=area' % (CW, CH)]
cmd += ['-f', 'rawvideo', '-pix_fmt', 'rgb24', '-']
proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, bufsize=CW * CH * 12)

RY, RX = np.arange(CH).reshape(CH, 1), np.arange(CW).reshape(1, CW)
i, accepted, lion = 0, None, None
while True:
    b = proc.stdout.read(CW * CH * 3)
    if len(b) < CW * CH * 3:
        break
    t = i / FPS
    if not (i % A.stride or t > A.t1 or t < A.t0):
        rgb = np.frombuffer(b, dtype=np.uint8).reshape(CH, CW, 3)
        g = (rgb[..., 0].astype(np.float32) * 0.299
             + rgb[..., 1].astype(np.float32) * 0.587
             + rgb[..., 2].astype(np.float32) * 0.114)
        # The lion, same pass, same frame.  Body: r>225, 95<g<165, b<70.
        # Paws ("shoes"): r>200, g>170, b<70 - the colour PR-84 measured the
        # feet anchor against, kept here because that is the feature the
        # port's CAM_FEET_Y is defined on, and the body mask's bottom row is
        # NOT that feature (the yellow pixels sit below the orange ones).
        # hudbox.py finds the orange the game paints in a place that never
        # moves - rows 166..196, cols 6..29 of the 320x200 view, the lives
        # head of belka.des - so that box is masked out with a margin.  Each
        # blob is kept near the previous frame's, so other art of the same
        # colour cannot pass for the lion.  Reported as a centroid and a box,
        # never as a position the game names.
        m = ((rgb[..., 0] > 225) & (rgb[..., 1] > 95) & (rgb[..., 1] < 165)
             & (rgb[..., 2] < 70))
        m[160:, :36] = False
        raw = int(m.sum())
        line = dict(i=i, t=round(t, 3), orange=raw)
        if raw >= 30:
            ys, xs = np.nonzero(m)
            cy, cx = lion if lion else (float(ys.mean()), float(xs.mean()))
            for _ in range(3):
                keep = m & (np.abs(RY - cy) <= 25) & (np.abs(RX - cx) <= 25)
                if int(keep.sum()) < 30:
                    break
                m = keep
                ky, kx = np.nonzero(m)
                cy, cx = float(ky.mean()), float(kx.mean())
            if int(m.sum()) >= 30:
                ky, kx = np.nonzero(m)
                lion = (cy, cx)
                line['lion'] = [round(cx, 1), round(cy, 1), int(m.sum()),
                                int(kx.min()), int(kx.max()), int(ky.min()),
                                int(ky.max())]
                yel = ((rgb[..., 0] > 200) & (rgb[..., 1] > 170)
                       & (rgb[..., 2] < 70)) & ~m
                yel[160:, :36] = False
                near = yel & (np.abs(RY - cy) <= 40) & (np.abs(RX - cx) <= 30)
                if int(near.sum()) >= 6:
                    yy, yx = np.nonzero(near)
                    line['paws'] = [round(float(yx.mean()), 1),
                                    round(float(yy.mean()), 1),
                                    int(near.sum()),
                                    int(yx.min()), int(yx.max()),
                                    int(yy.min()), int(yy.max())]
                else:
                    line['paws'] = []
        Fg = padfft(g)
        Fg2 = padfft(g * g)
        surf = {}
        for l in lands:
            s1 = np.fft.irfft2(Fg * np.conj(l['Ftm']), s=(N1, N2))
            s0 = np.fft.irfft2(Fg * np.conj(l['Fm']), s=(N1, N2))
            s2 = np.fft.irfft2(Fg2 * np.conj(l['Fm']), s=(N1, N2))
            num = s1 - l['mt'] * s0
            den = np.maximum(s2 - s0 * s0 / l['n'], 0.0) * (l['sd'] ** 2 *
                                                             l['n'])
            with np.errstate(invalid='ignore', divide='ignore'):
                surf[l['key']] = num / np.sqrt(den)

        def score(cam, l):
            """the correlation of one landmark AT a proposed camera"""
            u, v = l['ax'] - cam[0], l['ay'] - cam[1]
            if not (0 <= u <= CW - l['w'] and 0 <= v <= CH - l['h']):
                return None                    # not wholly in the view here
            s = surf[l['key']][v, u]
            return None if s != s else float(s)

        def margin(cam, l, s):
            """how much better this exact camera is than a +-1 unit shift of
            it, for one landmark: the reading's own precision bound"""
            r = surf[l['key']]
            u, v = l['ax'] - cam[0], l['ay'] - cam[1]
            n = max(r[v, u - 1], r[v, u + 1], r[v - 1, u], r[v + 1, u])
            return s - float(n)

        def judge(cam):
            vis = [(l, score(cam, l)) for l in lands]
            vis = [(l, s) for l, s in vis if s is not None]
            if not vis:
                return None
            hits = [(l, s) for l, s in vis if s >= THRESH]
            if not hits:
                return None                    # a camera nothing supports
            arts = set(l['art'] for l, _ in hits)
            key = (len(arts), round(sum(s for _, s in hits), 3),
                   round(min(s for _, s in vis), 3))
            return key, vis, hits

        cands = []
        for l in lands:                        # each match proposes a camera
            r = surf[l['key']]
            j = int(np.argmax(r[:CH - l['h'] + 1, :CW - l['w'] + 1]))
            v, u = divmod(j, CW - l['w'] + 1)
            if r[v, u] >= THRESH:
                cands.append((l['ax'] - u, l['ay'] - v))
        if accepted:                           # keep temporal continuity in
            cands.append(accepted)             # the candidate set
        scored = []
        for c in set(cands):
            k = judge(c)
            if k:
                scored.append((k[0], c, k[1], k[2]))
        if scored:
            prior = accepted or (0, 0)

            def rank(sc):                 # support first, then continuity: an
                cam = sc[1]               # exact tie means the level's
                return (sc[0], -(abs(cam[0] - prior[0])
                                 + abs(cam[1] - prior[1])))
                                        # duplicated .SEQ art supports two
            key, cam, vis, hits = max(scored, key=rank)
                                        # cameras equally, and only the
            amb = any(sc[0] == key      # video's own continuity can say
                      and sc[1] != cam for sc in scored)  # which one this is
            nk = len(set(l['art'] for l, _ in hits))
            mn = min(s for _, s in vis)
            line.update(camx=cam[0], camy=cam[1], tier=1 if nk >= 2 else 2,
                        ambiguous=amb,
                        visible=len(vis), matched=len(hits),
                        margin=round(min([margin(cam, l, s) for l, s in
                                         hits] + [1.0]), 3),
                        mean=round(sum(s for _, s in hits) / len(hits), 3),
                        worst=round(mn, 3),
                        who=[[l['key'], round(s, 3)] for l, s in
                             sorted(hits, key=lambda x: -x[1])],
                        absent=[[l['key'], round(s, 3)] for l, s in vis
                                if s < THRESH])
            accepted = cam
        print(json.dumps(line), flush=True)
    i += 1
proc.stdout.close()
