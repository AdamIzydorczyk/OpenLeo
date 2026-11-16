#!/usr/bin/env python3
"""verify_pcm.py - oracle for ported/ll_pcm.c (PCM parallax image layers).

Decodes each PCM payload in the ETAPY containers (level-block records
named *.PCM) with the python reference decode and compares the plane
FNV-1a-64 hash + geometry against `pcm_selftest <file>` output lines:
  decode <name> w=<w> win=<win> stride=<s> rows=<r> partial=<0|1> fnv=<hex>

usage: verify_pcm.py DATA_DIR [selftest_cmd]
"""
import subprocess
import struct
import sys

SLOT = struct.Struct('<14sIII')


def pcm_payloads(etapy_path):
    d = open(etapy_path, 'rb').read()
    cap, = struct.unpack_from('<I', d, 8)
    out = []
    for k in range(cap):
        key, rid, off, size = SLOT.unpack_from(d, 0x14 + 26 * k)
        blk = key.split(b'\0')[0]
        if rid != 2 or not blk:
            continue
        # sparse-table rule of ported/llrs.c: the header counts USED
        # records, the slot table continues past it with id==0 holes
        # (PR-36); scan every slot the block's size holds
        for t in range((size - 4) // 26):
            k2, r2, o2, s2 = SLOT.unpack_from(d, off + 4 + 26 * t)
            nm = k2.split(b'\0')[0]
            if r2 == 1 and nm.upper().endswith(b'.PCM'):
                out.append((blk.decode() + '/' + nm.decode(), d[o2:o2 + s2]))
    return out


def decode(raw):
    """-> (w, win, stride, plane bytes, rows, partial) or None."""
    if len(raw) < 0x1c4:
        return None
    w, win = struct.unpack_from('<HH', raw, 0)
    stride = w + 80                   # ctor alloc pitch (w + 0x50)
    i, dec = 0xa6, bytearray()
    n = len(raw)
    while i < n:
        c = raw[i]
        i += 1
        if (c & 0xC0) == 0xC0:
            if i >= n:
                break
            dec += bytes([raw[i]]) * (c & 0x3F)
            i += 1
        else:
            dec.append(c)
    rows = -(-len(dec) // stride)
    plane = bytes(dec) + bytes(rows * stride - len(dec))
    return w, win, stride, plane, rows, 1 if len(dec) % stride else 0


def view(plane, stride, strips, rows, win):
    """400-px rest window of FUN_00056c72 at world_x=0, clamped to the
    load window (hdr u16@2): screen strip k shows plane strip k mod
    strips (identity for k < 400)."""
    nh = min(rows, win if win else rows, 200)
    out = bytearray()
    for r in range(nh):
        row = plane[r * stride:(r + 1) * stride]
        for k in range(400):
            col = 80 * ((k // 80) % strips) + k % 80
            out.append(row[col])
    return bytes(out)


def fnv64(b):
    h = 0xCBF29CE484222325
    for v in b:
        h = ((h ^ v) * 0x100000001B3) & 0xFFFFFFFFFFFFFFFF
    return h


def main():
    data = sys.argv[1]
    cmd = sys.argv[2] if len(sys.argv) > 2 else './pcm_selftest'
    total = bad = 0
    tmp = '/tmp/opencode/pcm_verify.tmp'
    arg = tmp
    if len(sys.argv) > 3:                  # how the selftest should see it
        arg = sys.argv[3]                  # (wine needs a Z:/... path)
    for etapy in ('ETAPY',):
        for name, raw in pcm_payloads(data + '/' + etapy):
            total += 1
            ref = decode(raw)
            with open(tmp, 'wb') as fh:           # wine mangles big stdin
                fh.write(raw)
            got = subprocess.run(cmd.split() + [arg, name],
                                 capture_output=True)
            lines = got.stdout.decode().splitlines()
            line = '\n'.join(l.strip() for l in lines[:2])
            nh = min(ref[4], ref[1] if ref[1] else ref[4], 200)
            want = ('decode %s w=%d win=%d stride=%d rows=%d partial=%d '
                    'fnv=%016x\nview %s h=%d fnv=%016x'
                    % (name, ref[0], ref[1], ref[2], ref[4],
                       ref[5], fnv64(ref[3]), name, nh,
                       fnv64(view(ref[3], ref[2], ref[0] // 80, ref[4],
                                  ref[1]))))
            if line != want:
                bad += 1
                print('MISMATCH %s\n  want %s\n  got  %s' % (name, want, line))
            else:
                print('%s: %dx rows=%d (C == python, exact)'
                      % (name, ref[0], ref[4]))
    print('PCM check: %d/%d payloads byte-exact (C == python)'
          % (total - bad, total))
    return 1 if bad or total < 23 else 0   # 23 = every PCM payload (PR-47: sparse scan)


if __name__ == '__main__':
    sys.exit(main())
