#!/usr/bin/env python3
"""is_synth_check.py - data-free regression test for the .IS movie codec
(PR-55). It BUILDS a tiny 640x4 movie and checks three things against
values derived BY HAND from the game's machine code, so it cannot be fooled
by the python and C decoders agreeing on the same mistake (they did):

  1. LUT1 polarity (FUN_0001a9fe, 0x1aa1f..0x1aa30): each pixel is
     al = TABLE[bit] - a SET bit selects the SECOND table byte. For table
     (0x11, 0x22) and data byte 0x96 (bits 1001 0110) the 4 rows of the
     band get   22 11 / 11 22 / 11 22 / 22 11   (px0 = bit 7-i, px1 = bit
     3-i of row i). The old decoders produced the colour-swapped picture.
  2. both decoders: tools/is_decode.py and `ll_cutscene --ppm 0`.
  3. palette-only chunks redraw: the movie has one frame chunk and two
     chunks that carry only a PAL block (a fade); the player must present
     3 times (1 frame + 2 palette redraws), not once.

usage: is_synth_check.py PATH/TO/ll_cutscene
"""
import os
import struct
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import is_decode  # noqa: E402

W, H = 640, 4


def chunk(payload):
    return struct.pack('<BIH', 1, 7 + len(payload), 0) + payload


def pal(entries):
    p = bytearray(768)
    for idx, (r, g, b) in entries.items():
        p[3 * idx:3 * idx + 3] = bytes((r, g, b))
    return bytes([0xA0]) + bytes(p)


def build():
    hdr = bytearray(0x5F)
    struct.pack_into('<H', hdr, 0x59, W)
    struct.pack_into('<H', hdr, 0x5B, H)
    hdr[0x5D] = 5
    # frame: LUT1 len 2 (opcode 0x08|2), table 0x11 0x22, data 0x96, then a
    # SOLID of 638 px (2-byte opcode 0x40|2, 0x7E) of colour 0
    frame = bytes([0x0A, 0x11, 0x22, 0x96, 0x42, 0x7E, 0x00])
    c1 = pal({0x11: (10, 20, 30), 0x22: (50, 40, 30)}) + frame + b'\xB8'
    c2 = pal({0x11: (20, 40, 60), 0x22: (50, 40, 30)}) + b'\xB8'
    c3 = pal({0x11: (30, 60, 63), 0x22: (50, 40, 30)}) + b'\xB8'
    return bytes(hdr) + chunk(c1) + chunk(c2) + chunk(c3)


EXPECT = [[0x22, 0x11], [0x11, 0x22], [0x11, 0x22], [0x22, 0x11]]


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return 2
    exe = sys.argv[1]
    bad = 0
    with tempfile.TemporaryDirectory() as tmp:
        f = os.path.join(tmp, 'synth.IS')
        open(f, 'wb').write(build())

        # 2a. python decoder
        got = {}

        def cb(i, fb, palette, st):
            if i == 0:
                got['fb'] = bytes(fb)
        is_decode.IS(f).decode_all({}, cb)
        rows = [[got['fb'][r * W + x] for x in range(2)] for r in range(4)]
        if rows != EXPECT or got['fb'][2] != 0 or got['fb'][W * 3 + 639] != 0:
            bad += 1
            print('FAIL python LUT1 polarity: got %r want %r' % (rows, EXPECT))
        else:
            print('python decoder: LUT1 set bit -> second table byte (ok)')

        # 2b. C decoder through the PPM writer: colour of the two indices
        out = os.path.join(tmp, 'f0.ppm')
        r = subprocess.run([exe, f, '--ppm', '0', out], capture_output=True)
        if r.returncode != 0 or not os.path.exists(out):
            bad += 1
            print('FAIL C --ppm: rc=%d %s' % (r.returncode, r.stderr[-200:]))
        else:
            d = open(out, 'rb').read()
            hdr_end = d.index(b'255\n') + 4
            px = d[hdr_end:]
            cols = {0x11: (10 * 4, 20 * 4, 30 * 4), 0x22: (50 * 4, 40 * 4, 30 * 4)}
            ok = True
            for rr in range(4):
                for x in range(2):
                    o = 3 * (rr * W + x)
                    ok &= tuple(px[o:o + 3]) == cols[EXPECT[rr][x]]
            if ok:
                print('C decoder: LUT1 set bit -> second table byte (ok)')
            else:
                bad += 1
                print('FAIL C LUT1 polarity (PPM colours do not match)')

        # 3. the player redraws on palette-only chunks
        env = dict(os.environ, SDL_VIDEODRIVER='dummy', LEO_IS_PERIOD_MS='1')
        env.pop('DISPLAY', None)
        r = subprocess.run([exe, f, '--no-audio'], capture_output=True, env=env,
                           timeout=60)
        txt = r.stdout.decode('latin1')
        want = 'presented=3 (of which 2 palette-only redraws) decoded=1'
        if want in txt:
            print('player: 1 frame + 2 palette-only chunks = 3 presents (ok)')
        else:
            bad += 1
            line = [l for l in txt.split('\n') if 'is_player: presented' in l]
            print('FAIL player palette redraw: want %r got %r' % (want, line))
    print('IS synth check: %s' % ('FAILED' if bad else 'all ok'))
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
