#!/usr/bin/env python3
"""Oracle for the level help overlay render (make verify-help).

Checks that the C ll_level help dump (LEO_HELP_PPM) equals the bare
background plus exactly one POMOC page sprite:
  * painted-pixel count == tools/seq_render.sprite() touched count,
  * all differing pixels inside the page-0 window derived from the
    element header (screen pos = (-f0,-f2), like the menu leaves).

argv: help.ppm bare.ppm ETAPY-container level-block-key page-seq x0 x1 y0 y1
"""
import struct
import sys

sys.path.insert(0, 'tools')
from seq_parse import seq_parse
from seq_render import sprite

SLOT = struct.Struct('<14sIII')


def ppm_px(path):
    d = open(path, 'rb').read()
    return d[d.index(b'255\n') + 4:]


def ppm_w(path):
    d = open(path, 'rb').read()
    return int(d.split(b'\n', 2)[1].split()[0])


def find_payload(container, block_key, rec_name):
    d = open(container, 'rb').read()
    cap, = struct.unpack_from('<I', d, 8)
    for k in range(cap):
        key, rid, offk, sizek = SLOT.unpack_from(d, 0x14 + 26 * k)
        if key.split(b'\0')[0].upper() != block_key.encode().upper() \
                or rid != 2:
            continue
        cnt, = struct.unpack_from('<I', d, offk)
        for t in range(cnt):
            k2, r2, o2, s2 = SLOT.unpack_from(d, offk + 4 + 26 * t)
            if r2 == 1 and k2.split(b'\0')[0].upper() \
                    == rec_name.encode().upper():
                return d[o2:o2 + s2]
    raise SystemExit('%s/%s not found in %s' % (block_key, rec_name,
                                                container))


def main():
    help_ppm, bare_ppm, etapy, blk, rec = sys.argv[1:6]
    x0, x1, y0, y1 = (int(v) for v in sys.argv[6:10])
    a, b = ppm_px(help_ppm), ppm_px(bare_ppm)
    W = ppm_w(help_ppm)
    px = sorted({k // 3 for k in range(min(len(a), len(b))) if a[k] != b[k]})
    xs = [p % W for p in px]
    ys = [p // W for p in px]
    raw = find_payload(etapy, blk, rec)
    e = seq_parse(raw)['els'][0]
    w, h, canv, touched = sprite(e)
    exp = sum(1 for v in touched if v)
    ok = (len(px) == exp and px
          and min(xs) >= x0 and max(xs) <= x1
          and min(ys) >= y0 and max(ys) <= y1)
    bbox = (min(xs), max(xs), min(ys), max(ys)) if px else None
    if ok:
        print('help render: overlay in page-0 window, C-painted px %d '
              '== oracle %d' % (len(px), exp))
    else:
        print('help render FAILED: px %d oracle %d bbox %s expected [%d..%d,'
              '%d..%d]' % (len(px), exp, bbox, x0, x1, y0, y1))
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main())
