#!/usr/bin/env python3
"""verify_pcm.py - oracle for ported/ll_pcm.c (PCM transparent parallax
layers), written from the machine code of the game's loader FUN_00056d40
and tile interpreter FUN_0001d58c (opcode table in ll_pcm.h), NOT from
ll_pcm.c.

For EVERY .PCM record of the ETAPY container (sparse tables scanned in
full) it decodes the layer with this python reference and compares
geometry, painted-pixel count and the plane FNV-1a-64 with `pcm_selftest`:
  decode <name> strips=<N> h=<H> tile=<T> groups=<G> contiguous=<0|1>
         painted=<n> fnv=<hex>

It ALSO asserts two properties that need no ground truth and so prove the
reading itself: every tile row covers exactly 20 dwords (80 px) and every
tile ends exactly where the next one starts. A wrong opcode table or a wrong
header reading fails them on the first tile.

usage: verify_pcm.py DATA_DIR [selftest_cmd]
"""
import os
import struct
import subprocess
import sys
import tempfile

SLOT = struct.Struct('<14sIII')
M64 = (1 << 64) - 1


def pcm_payloads(etapy_path):
    d = open(etapy_path, 'rb').read()
    cap, = struct.unpack_from('<I', d, 8)
    out = []
    for k in range(cap):
        key, rid, off, size = SLOT.unpack_from(d, 0x14 + 26 * k)
        blk = key.split(b'\0')[0]
        if rid != 2 or not blk:
            continue
        # sparse nested table: scan every slot the block holds
        for t in range((size - 4) // 26):
            k2, r2, o2, s2 = SLOT.unpack_from(d, off + 4 + 26 * t)
            nm = k2.split(b'\0')[0]
            if r2 == 1 and nm.upper().endswith(b'.PCM'):
                out.append((blk.decode() + '/' + nm.decode(), d[o2:o2 + s2]))
    return out


# opcode -> {pixel: byte index (1..3)} for the single-dword opcodes
SINGLE = {
    1: {3: 1}, 2: {2: 1}, 4: {1: 1}, 8: {0: 1},
    3: {2: 2, 3: 3}, 5: {1: 1, 3: 3}, 6: {1: 1, 2: 2}, 7: {1: 1, 2: 2, 3: 3},
    9: {0: 1, 3: 2}, 0xa: {0: 1, 2: 3}, 0xb: {0: 1, 2: 2, 3: 3},
    0xc: {0: 2, 1: 3}, 0xd: {0: 2, 1: 3, 3: 1}, 0xe: {0: 1, 1: 2, 2: 3},
}
SINGLE_LEN = {1: 2, 2: 2, 4: 2, 8: 2}


def run_row(d, i):
    """-> (new_i, row[80] of None|byte, dwords); raises ValueError"""
    row = [None] * 80
    dw = 0
    while True:
        if i >= len(d):
            raise ValueError('opcode past the data')
        op = d[i]
        end = op >= 0x11
        if end:
            op -= 0x11
        if op > 0x10:
            raise ValueError('bad opcode')
        b = [0, d[i + 1] if i + 1 < len(d) else 0,
             d[i + 2] if i + 2 < len(d) else 0,
             d[i + 3] if i + 3 < len(d) else 0]
        if op == 0:
            dw += b[1]
            n = 2
        elif op == 0xf:
            n = 2 + 4 * b[1]
            if i + n > len(d) or dw + b[1] > 20:
                raise ValueError('raw run overflow')
            for k in range(4 * b[1]):
                row[dw * 4 + k] = d[i + 2 + k]
            dw += b[1]
        elif op == 0x10:
            n = 4
            if dw + b[1] > 20 or i + n > len(d):
                raise ValueError('fill overflow')
            for k in range(b[1]):
                for p, v in enumerate((b[2], b[3], b[2], b[3])):
                    row[(dw + k) * 4 + p] = v
            dw += b[1]
        else:
            n = SINGLE_LEN.get(op, 4)
            if dw >= 20 or i + n > len(d):
                raise ValueError('dword overflow')
            for p, bi in SINGLE[op].items():
                row[dw * 4 + p] = b[bi]
            dw += 1
        if dw > 20:
            raise ValueError('row past 20 dwords')
        i += n
        if end:
            if dw != 20:
                raise ValueError('row covers %d dwords, not 20' % dw)
            return i, row


def decode(p):
    """-> (geometry dict, plane bytearray px, op bytearray, contiguous)"""
    H, N, T = struct.unpack_from('<HHH', p, 0)
    G = (H + T - 1) // T
    off = 6
    base = struct.unpack_from('<%dI' % N, p, off)
    off += 4 * N
    lists = []
    for s in range(N):
        lists.append([0] + list(struct.unpack_from('<%dI' % (G - 1), p, off)))
        off += 4 * (G - 1)
    data = p[off:]
    w = N * 80
    px = bytearray(w * H)
    opq = bytearray(w * H)
    contiguous = 1
    for s in range(N):
        for g in range(G):
            pos = base[s] + lists[s][g]
            for r in range(min(T, H - g * T)):
                pos, row = run_row(data, pos)
                y = g * T + r
                for x, v in enumerate(row):
                    if v is not None:
                        px[y * w + s * 80 + x] = v
                        opq[y * w + s * 80 + x] = 1
            if g + 1 < G and pos != base[s] + lists[s][g + 1]:
                contiguous = 0
    return dict(H=H, N=N, T=T, G=G), px, opq, contiguous


def fnv(px, opq):
    h = 0xCBF29CE484222325
    for v, o in zip(px, opq):
        h = ((h ^ (v if o else 0)) * 0x100000001B3) & M64
    for o in opq:
        h = ((h ^ o) * 0x100000001B3) & M64
    return h


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return 2
    data = sys.argv[1]
    tool = sys.argv[2].split() if len(sys.argv) > 2 else ['./pcm_selftest']
    total = bad = 0
    with tempfile.TemporaryDirectory() as tmp:
        for name, raw in pcm_payloads(os.path.join(data, 'ETAPY')):
            total += 1
            try:
                g, px, opq, contig = decode(raw)
            except (ValueError, struct.error) as ex:
                bad += 1
                print('PY-FAIL %s: %s' % (name, ex))
                continue
            f = os.path.join(tmp, name.replace('/', '_'))
            open(f, 'wb').write(raw)
            got = subprocess.run(tool + [f, name], capture_output=True)
            line = got.stdout.decode().replace('\r', '').strip()
            want = ('decode %s strips=%d h=%d tile=%d groups=%d contiguous=%d '
                    'painted=%d fnv=%016x'
                    % (name, g['N'], g['H'], g['T'], g['G'], contig,
                       sum(opq), fnv(px, opq)))
            if not contig:
                bad += 1
                print('NOT-CONTIGUOUS %s (tile ends are not the next starts:'
                      ' the opcode reading is wrong)' % name)
            elif line != want:
                bad += 1
                print('MISMATCH %s\n  want %s\n  got  %s' % (name, want, line))
            else:
                print('%s: %dx%d, %d strips (C == python, rows 20 dwords, '
                      'tiles contiguous) fnv=%016x'
                      % (name, g['N'] * 80, g['H'], g['N'], fnv(px, opq)))
    print('PCM check: %d/%d layers byte-exact (C == python; every row 20 '
          'dwords, every tile contiguous)' % (total - bad, total))
    return 1 if bad or total < 23 else 0


if __name__ == '__main__':
    sys.exit(main())
