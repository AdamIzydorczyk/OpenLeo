#!/usr/bin/env python3
"""golden_menu_cmp.py - compare the port's chapter-map frames against the
ORIGINAL game captured in QEMU (T-24 golden-master).

Original-side inputs (from tools/golden_qemu_run.py; optional in --expect
manifest-only mode):
  --vram BIN     256 KiB VRAM window; first 64000 B = linear 8bpp
                 320x200 index plane, shown 2x2-doubled on the 640x400
                 CRTC (layout verified by menu_dac_gen.py)
  --screen PPM   the matching QEMU screendump (640x400, RGB)
Port-side input:
  --port-dir DIR port frames map_NNNN.ppm (640x400 RGB, hover-free;
                 from LEO_MENU_PPM_DIR; contents deterministic per tick)
  --expect MAN   committed facts to assert (hashes only, never frames):
                 golden.{vram_idx_sha256,vram_full_sha256,orig_rgb_
                 sha256,indices_used} + port.pose_hashes (the SET of
                 distinct map-animation frame hashes - timing-independent,
                 unlike per-file pins) + min_best_rgb. Works WITHOUT
                 --vram/--screen: then only the port pins are checked
                 (SKIP-safe CI mode).

Method (same layout rules as menu_dac_gen.py):
  1. the screendump must be 2x2-doubled -> undouble to 320x200 colors;
  2. every VRAM index byte must map to exactly ONE color, else abort
     (the layout assumption broke - same rule as menu_dac_gen);
  3. port frames must be 2x2-doubled too (the port doubles its own
     composition the same way); undouble both;
  4. per frame: exact-RGB match %, plus index-plane match via the
     original DAC (the strongest claim: identical framebuffer).

Exit 0 iff the golden half passes AND every --expect pin holds AND
some frame reaches --min-match exact RGB. Only hashes/facts go into
the repo; frames stay in /tmp (copyright rule, tips T-24).
"""
import argparse
import hashlib
import json
import os
import sys

import numpy as np
from PIL import Image


def load_ppm_rgb(path):
    return np.asarray(Image.open(path).convert('RGB'), np.uint8)


def undouble2(img):
    """Return (half_rgb, is_pure_doubling)."""
    half = img[0::2, 0::2]
    pure = bool((img[0::2, 0::2] == half).all()
                and (img[1::2, 0::2] == half).all()
                and (img[0::2, 1::2] == half).all()
                and (img[1::2, 1::2] == half).all())
    return half, pure


def sha(b):
    return hashlib.sha256(b).hexdigest()


def file_sha(path):
    with open(path, 'rb') as f:
        return sha(f.read())


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--vram')
    ap.add_argument('--screen')
    ap.add_argument('--port-dir', required=True)
    ap.add_argument('--expect', help='committed manifest to assert')
    ap.add_argument('--json', help='write machine summary here')
    ap.add_argument('--min-match', type=float, default=100.0)
    a = ap.parse_args()

    fails = []
    summary = {'frames': [], 'best': None, 'golden': None}

    # ---------- golden half (needs the QEMU capture) ----------
    orig = idx = None
    if a.vram or a.screen:
        if not (a.vram and a.screen):
            sys.exit('--vram and --screen come together')
        blob = open(a.vram, 'rb').read()
        if len(blob) < 64000:
            sys.exit('vram.bin too short (%d)' % len(blob))
        idx = np.frombuffer(blob[:64000], np.uint8).reshape(200, 320)

        g = load_ppm_rgb(a.screen)
        if g.shape[:2] != (400, 640):
            sys.exit('screendump must be 640x400, got %r'
                     % (g.shape[:2],))
        _, pure = undouble2(g)
        if not pure:
            sys.exit('screendump is not 2x2-doubled: wrong capture state '
                     '(the map menu shows a CRTC-doubled 320x200 FB)')

        # index -> color, consistency-checked exactly like menu_dac_gen
        flat_c = g.reshape(-1, 3)
        cols, inv = np.unique(flat_c, axis=0, return_inverse=True)
        # numpy>=2.0 returns inverse with the input's shape; bincount needs
        # a flat index array, and numpy<2 returns it flat already.
        inv = np.ravel(inv).astype(np.int64)
        cid = inv.reshape(400, 640)[0::2, 0::2]
        M = np.bincount(idx.ravel().astype(np.int64) * len(cols)
                       + cid.ravel(), minlength=256 * len(cols))
        per = M.reshape(256, len(cols))
        bad = [i for i in range(256) if (per[i] > 0).sum() > 1]
        if bad:
            sys.exit('layout assumption broken: %d indices map to '
                     'multiple colors' % len(bad))
        used = [i for i in range(256) if (per[i] > 0).sum() == 1]
        missing = set(np.unique(idx)) - set(used)
        if missing:
            sys.exit('indices without a color: %r' % sorted(missing))
        idx2col = {i: cols[int(np.argmax(per[i]))] for i in used}
        orig = np.zeros((200, 320, 3), np.uint8)
        for i, c in idx2col.items():
            orig[idx == i] = c

        summary['golden'] = {
            'vram_idx_sha256': sha(blob[:64000]),
            'vram_full_sha256': sha(blob),
            'orig_rgb_sha256': sha(orig.tobytes()),
            'indices_used': len(used),
        }
        print('golden: %d distinct indices, palette consistent, screen '
              'is pure 2x2' % len(used))
        print('  idx-plane sha256: %s' % summary['golden']
              ['vram_idx_sha256'])
        print('  orig-rgb sha256:  %s' % summary['golden']
              ['orig_rgb_sha256'])
        if a.expect:
            exp = json.load(open(a.expect)).get('golden', {})
            for k, v in exp.items():
                if summary['golden'].get(k) != v:
                    fails.append('golden %s changed: manifest %r vs '
                                 'capture %r' % (k, v,
                                                 summary['golden'].get(k)))

    # ---------- port half ----------
    port_files = sorted(f for f in os.listdir(a.port_dir)
                        if f.startswith('map_') and f.endswith('.ppm'))
    if not port_files:
        sys.exit('no port frames in %s (demo chain did not reach the '
                 'map menu?)' % a.port_dir)

    col2idx = {}
    dup = False
    if idx is not None:
        for i, c in sorted(idx2col.items()):
            k = bytes(int(x) for x in c)
            if k in col2idx:
                dup = True
            col2idx[k] = i
        if dup:
            print('note: original palette has duplicate RGB entries; '
                  'index-plane comparison uses the lowest index')

    for pf in port_files:
        path = os.path.join(a.port_dir, pf)
        rec = {'file': pf, 'sha256': file_sha(path)}
        port = load_ppm_rgb(path)
        if port.shape[:2] == (400, 640):
            ph, ppure = undouble2(port)
            if not ppure:
                rec['error'] = 'not 2x2-doubled'
                summary['frames'].append(rec)
                print('  %-16s NOT 2x2-doubled (unexpected)' % pf)
                continue
        elif port.shape[:2] == (200, 320):
            ph = port
        else:
            rec['error'] = 'size %r' % (port.shape[:2],)
            summary['frames'].append(rec)
            continue
        if orig is not None:
            same = (ph == orig).all(axis=2)
            rec['rgb_match_pct'] = round(100.0 * same.sum() / same.size,
                                         4)
            key = [bytes(int(x) for x in c)
                   for c in ph.reshape(-1, 3)]
            pidx = np.full(len(key), -1, np.int64)
            for n, k in enumerate(key):
                if k in col2idx:
                    pidx[n] = col2idx[k]
            pidx = pidx.reshape(200, 320)
            known = pidx >= 0
            rec['index_match_pct'] = round(
                100.0 * (pidx[known] == idx[known]).sum() / known.sum()
                if known.sum() else 0.0, 4)
            rec['off_palette_px'] = int((~known).sum())
            print('  %-16s rgb=%7.3f%%  idx=%7.3f%%  offpal_px=%d  %s'
                  % (pf, rec['rgb_match_pct'], rec['index_match_pct'],
                     rec['off_palette_px'], rec['sha256'][:12]))
        else:
            print('  %-16s sha=%s (no capture: pin-mode only)'
                  % (pf, rec['sha256'][:16]))
        summary['frames'].append(rec)

    # ---- port pose-set pin (timing-independent) ----
    # The map animation is a fixed cycle; how many frames land in a wall
    # clock capture window varies, but the SET of distinct composed frames
    # for one full cycle does not. That set is what we pin.
    if a.expect:
        want = json.load(open(a.expect)).get('port', {}).get('pose_hashes')
        if want is not None:
            got = sorted({r['sha256'] for r in summary['frames']
                          if 'sha256' in r and 'error' not in r})
            want = sorted(want)
            if got != want:
                only_got = [h[:12] for h in set(got) - set(want)]
                only_want = [h[:12] for h in set(want) - set(got)]
                fails.append('port pose set changed: %d vs %d hashes '
                             '(extra %s missing %s)'
                             % (len(got), len(want), only_got, only_want))
            else:
                print('port pose set: %d hashes match manifest' % len(got))
            summary['port_poses'] = got

    scored = [r for r in summary['frames'] if 'rgb_match_pct' in r]
    best = max(scored, key=lambda r: r['rgb_match_pct'], default=None)
    if best:
        print('best: %s rgb=%.4f%% idx=%.4f%%'
              % (best['file'], best['rgb_match_pct'],
                 best['index_match_pct']))
        summary['best'] = best
        if best['rgb_match_pct'] < a.min_match:
            # localize the mismatch for the best frame
            port = load_ppm_rgb(os.path.join(a.port_dir, best['file']))
            ph, _ = undouble2(port)
            d = (ph != orig).all(axis=2)
            ys, xs = np.nonzero(d)
            if len(ys):
                print('diff bbox: y %d..%d x %d..%d (%d px, %.3f%%)'
                      % (ys.min(), ys.max(), xs.min(), xs.max(), d.sum(),
                         100.0 * d.sum() / d.size))
            fails.append('best RGB match %.3f%% < min-match %.3f%%'
                         % (best['rgb_match_pct'], a.min_match))
    elif a.vram:
        fails.append('no comparable port frame')

    if a.expect:
        exp = json.load(open(a.expect))
        mb = exp.get('min_best_rgb')
        if mb is not None and a.vram and (not best or
                                          best['rgb_match_pct'] < mb):
            fails.append('best %.3f%% < manifest min_best_rgb %.3f%%'
                         % (best['rgb_match_pct'] if best else 0.0, mb))

    if a.json:
        with open(a.json, 'w') as f:
            json.dump(summary, f, indent=1)
    if fails:
        for x in fails:
            print('FAIL: %s' % x, file=sys.stderr)
        sys.exit(1)
    print('golden-master comparison: ok')


if __name__ == '__main__':
    main()
