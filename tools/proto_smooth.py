#!/usr/bin/env python3
"""PR-42 SMOOTHING PROTOTYPE - SHELFED 2026-11-16 (kept for a future
attempt; no C code landed, nothing in the product uses this file).

Owner review of rendered comparisons (ZARIA1 full, LONA motion burst,
normal + 2x slow, columns CURRENT|BLEND|MOTION): "all have same
artifacts" - the artifact is already in the 1997 source frames, so
playback-side smoothing was parked. Measured meanwhile: frames are
NOT baked crossfades (0/200 triplets fit a blend-of-neighbours model).
Findings + full record: NEXT_STEPS.md, section "PR-42 (SHELFED)".

Original design notes below.

Reads a de-ghosted .LVM (keyframes + holds = the PR-41 shipped stream)
and renders 60 fps mp4s for a chosen movie window:
  CURRENT/BLEND/MOTION side-by-side comparison.mp4 + a 2x slow version.
  column 1 CURRENT : faithful presentation (what ll_cutscene shows today)
  column 2 BLEND   : whole-hold dissolve of each swap to 60 fps
  column 3 MOTION  : block-motion synthesis (16x16 blocks, 4-px search
                     steps, translation; unmatched blocks static)
No timeline changes anywhere: everything happens inside each record's OWN
hold, so audio sync is untouched by construction (the contract a C port
would have to keep). Needs numpy + ffmpeg.

Usage: proto_smooth.py MOVIE.LVM START_S DUR_S OUTDIR
"""
import struct, sys, os, subprocess
import numpy as np

FPS = 60.0
STEP = 1.0 / FPS


def decode_frame(f, off, codec, n):
    if codec == 0:
        return np.frombuffer(f[off:off + n], np.uint8).copy(), off + n
    out = np.empty(n, np.uint8)
    i, k = off, 0
    while k < n:
        c = f[i]; i += 1
        if c:
            out[k:k + c] = np.frombuffer(f[i:i + c], np.uint8); i += c; k += c
        else:
            ln = struct.unpack_from('<H', f, i)[0]; i += 2
            out[k:k + ln] = f[i]; i += 1; k += ln
    return out, i


def to_rgb(plane, pal, w, h):
    return pal.reshape(256, 3)[plane.reshape(h, w)]


def read_lvm(path, tmin=-1.0, tmax=1e18):
    """(w,h,recs): recs = [(t_s, hold_s, rgb|None, pal768)]. RGB decoded
    only for frames starting in [tmin,tmax]; the last frame before tmin
    is decoded too (it is the hold covering the window start)."""
    f = open(path, 'rb').read()
    assert f[:4] == b'LVM1'
    w, h = struct.unpack_from('<II', f, 4)
    alen, = struct.unpack_from('<Q', f, 19)
    nrec, = struct.unpack_from('<I', f, 27)
    off = 31 + alen
    pal = np.zeros(768, np.uint8)
    n = w * h
    recs = []
    anchor = None       # (t, hold, rgb) of last frame before tmin
    t = 0.0
    for _ in range(nrec):
        hold = struct.unpack_from('<Q', f, off)[0] / 1e9
        flags = f[off + 8]
        off += 9
        if flags & 2:
            pal = np.frombuffer(f[off:off + 768], np.uint8).copy(); off += 768
        rgb = None
        if flags & 1:
            codec = f[off]; off += 1
            plane, off = decode_frame(f, off, codec, n)
            if tmin <= t <= tmax:
                rgb = to_rgb(plane, pal, w, h)
            elif t < tmin:
                anchor = (t, hold, to_rgb(plane, pal, w, h))
        recs.append((t, hold, rgb, pal.copy()))
        t += hold
    if anchor is not None:
        for i, r in enumerate(recs):
            if r[0] == anchor[0] and r[2] is None:
                recs[i] = (r[0], r[1], anchor[2], r[3])
                break
    return w, h, recs


def block_mv(A, B, r=3):
    """Per-16x16-block displacement D (full-res px) such that
    B[y] ~= A[y - D] (content moved by +D from A to B).
    Search +/-4*r px in 4-px steps, SAD on stride-4 subsamples."""
    h, w, _ = A.shape
    bh, bw = h // 16, w // 16
    gA = A[:bh * 16, :bw * 16][::4, ::4]
    gB = B[:bh * 16, :bw * 16][::4, ::4]
    gAp = np.pad(gA, ((r, r), (r, r), (0, 0)))
    nh, nw = gA.shape[0], gA.shape[1]
    base = np.full((bh, bw), 1 << 30, np.int64)
    vecs = np.zeros((bh, bw, 2), np.int16)
    for dy in range(-r, r + 1):
        for dx in range(-r, r + 1):
            shifted = gAp[r + dy:r + dy + nh, r + dx:r + dx + nw]
            cost = (np.abs(shifted.astype(np.int16) - gB.astype(np.int16))
                    .sum(axis=2).reshape(bh, 4, bw, 4).sum(axis=(1, 3)))
            imp = cost < base
            base[imp] = cost[imp]
            vecs[imp, 0] = dy * 4
            vecs[imp, 1] = dx * 4
    ok = base <= (16 * 3 * 6)          # mean abs err <= 6/channel-value
    return vecs, ok


def synth_motion(A, alpha, vecs, ok):
    """synthetic frame at phase alpha of A->B: matched blocks show A
    content at y - (1-alpha)*D (slide from A position toward B position)."""
    h, w, _ = A.shape
    bh, bw = h // 16, w // 16
    out = A.copy()
    for by in range(bh):
        for bx in range(bw):
            if not ok[by, bx]:
                continue
            vy, vx = int(vecs[by, bx, 0]), int(vecs[by, bx, 1])
            if vy == 0 and vx == 0:
                continue
            ay = max(0, min(h - 16, int(round(by * 16 - vy * (1 - alpha)))))
            ax = max(0, min(w - 16, int(round(bx * 16 - vx * (1 - alpha)))))
            out[by * 16:(by + 1) * 16, bx * 16:(bx + 1) * 16] = A[ay:ay + 16, ax:ax + 16]
    return out


def frame_events(recs):
    """real frame events only: (start, end, rgb)."""
    for (t, hold, rgb, pal) in recs:
        if rgb is not None:
            yield (t, t + hold, rgb)


def interp_slots(H):
    """number of synthetic frames that fit in one hold of length H at
    60 fps output (capped at 4): an H=66.7ms (15 fps native) hold gets
    3 -> the 15 fps stream becomes a true 60 fps presentation."""
    return min(4, max(0, int(H / STEP + 0.05) - 1))


def blend_events(recs, tmin=-1e9):
    """dissolve A->B evenly across A's whole hold; total time unchanged,
    B still starts at its exact time."""
    prev = None
    for (t, hold, rgb, pal) in recs:
        if rgb is None:
            continue
        if prev is not None:
            tA, A = prev
            H = t - tA
            if (np.abs(A.astype(np.int16) - rgb.astype(np.int16)).mean() > 3
                    and t >= tmin):
                M = interp_slots(H)
                if M >= 1:
                    for k in range(1, M + 1):
                        al = k / (M + 1.0)
                        yield (tA + H * al, tA + H * al + STEP,
                               ((1 - al) * A + al * rgb).astype(np.uint8))
                    yield (t, t + hold, rgb)
                    prev = (t, rgb)
                    continue
            yield (t, t + hold, rgb)
        else:
            yield (t, t + hold, rgb)
        prev = (t, rgb)


def motion_events(recs, tmin=-1e9):
    """same slot plan as blend_events, but each synthetic frame SLIDES
    matched 16x16 blocks (true 60 fps motion-compensated presentation)."""
    prev = None
    for (t, hold, rgb, pal) in recs:
        if rgb is None:
            continue
        if prev is not None:
            tA, A = prev
            H = t - tA
            if (np.abs(A.astype(np.int16) - rgb.astype(np.int16)).mean() > 3
                    and t >= tmin):
                vecs, ok = block_mv(A, rgb)
                moving = bool(ok.any() and (vecs[ok] != 0).any())
                M = interp_slots(H) if moving else 0
                if M >= 1:
                    for k in range(1, M + 1):
                        al = k / (M + 1.0)
                        yield (tA + H * al, tA + H * al + STEP,
                               synth_motion(A, al, vecs, ok))
                    yield (t, t + hold, rgb)
                    prev = (t, rgb)
                    continue
            yield (t, t + hold, rgb)
        else:
            yield (t, t + hold, rgb)
        prev = (t, rgb)


def rasterize(evgen, t0, dur, w, h):
    ev = sorted(evgen, key=lambda e: e[0])
    ns = int(dur * FPS) + 1
    frames = []
    cur = np.zeros((h, w, 3), np.uint8)
    i = 0
    for s in range(ns):
        ts = t0 + s * STEP
        while i < len(ev) and ev[i][0] <= ts:
            cur = ev[i][2]
            i += 1
        frames.append(cur)   # player holds the last frame
    return np.stack(frames)


def encode(vid, w, h, mp):
    p = subprocess.run(['ffmpeg', '-y', '-loglevel', 'error',
                        '-f', 'rawvideo', '-pix_fmt', 'rgb24',
                        '-s', '%dx%d' % (w, h), '-r', str(int(FPS)),
                        '-i', '-', '-c:v', 'libx264', '-preset', 'veryfast',
                        '-crf', '16', '-pix_fmt', 'yuv420p', mp],
                       input=vid.tobytes())
    assert p.returncode == 0, 'ffmpeg failed for ' + mp


def main():
    lvm, t0, dur, outdir = sys.argv[1], float(sys.argv[2]), float(sys.argv[3]), sys.argv[4]
    os.makedirs(outdir, exist_ok=True)
    w, h, recs = read_lvm(lvm, t0 - 0.5, t0 + dur + 0.5)
    print('lvm %s %dx%d recs=%d window=%.1f..%.1f' %
          (os.path.basename(lvm), w, h, len(recs), t0, t0 + dur), flush=True)
    variants = [('CURRENT', frame_events(recs)),
                ('BLEND', blend_events(recs)),
                ('MOTION', motion_events(recs, t0 - 0.2))]
    mps = []
    for name, gen in variants:
        vid = rasterize(gen, t0, dur, w, h)
        mp = os.path.join(outdir, name + '.mp4')
        encode(vid, w, h, mp)
        print('encoded', mp, flush=True)
        mps.append(mp)
    cmp_mp4 = os.path.join(outdir, 'compare.mp4')
    fs = max(10, h // 16)
    lbl = lambda i, tag, ch: ("[%d:v]drawtext=text='%s':x=6:y=6:fontsize=%d:"
                              "fontcolor=white:box=1:boxcolor=black@0.6[%s]" % (i, tag, fs, ch))
    fc = ";".join([lbl(0, 'CURRENT', 'a'), lbl(1, 'BLEND', 'b'), lbl(2, 'MOTION', 'c'),
                   "[a][b][c]hstack=3,scale=iw*2:ih*2:flags=neighbor"])
    subprocess.run(['ffmpeg', '-y', '-loglevel', 'error'] +
                   sum([['-i', m] for m in mps], []) +
                   ['-filter_complex', fc, '-c:v', 'libx264', '-crf', '17',
                    '-pix_fmt', 'yuv420p', cmp_mp4], check=True)
    slow = os.path.join(outdir, 'compare_slow.mp4')
    subprocess.run(['ffmpeg', '-y', '-loglevel', 'error', '-i', cmp_mp4,
                    '-filter:v', 'setpts=2.0*PTS', '-r', '30', '-c:v', 'libx264',
                    '-crf', '18', '-pix_fmt', 'yuv420p', slow], check=True)
    print('WROTE', cmp_mp4, 'and', slow)


main()
