#!/usr/bin/env python3
"""Predict .IS movie wall duration from the verified audio-block clock.

Model (PLAN.md "cutscenes: pacing is the .IS codec's audio DMA clock"):
ISplay plays the ESC audio at 22050 Hz s16 in DMA blocks of 315 source
frames = 14.286 ms (FUN_00019a60: rate 0x5622, block 0x13b).  Its DMA IRQ
handler (FUN_000185c0) advances video every `repeat` blocks, and every
payload's ESC region == repeat x block bytes (1260 B stereo / 630 B mono).
So a chunk's hold = region_bytes / bps, and a region-less chunk holds
repeat x 1/70 s (the engine silence-pads, cf. the 0x80 fill).
Channels: gcd of the region sizes - an odd multiple of 630 proves mono.

This is the python twin of ported/platform_sdl/is_player.c's pacing loop;
use it to sanity-check what the C player should show on screen.

Usage: is_duration.py [DIR | FILE.IS ...]   (default: the staged LEO dir)
"""
import glob
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import is_decode as I

ESC, END, RPT8, RPT16, PAL = 0xB0, 0xB8, 0x98, 0xA8, 0xA0
BLK_ST, BLK_MO = 1260, 630


def igcd(a, b):
    while b:
        a, b = b, a % b
    return a


def analyse(path):
    is_ = I.IS(path)
    try:
        rep0 = is_.d[0x5d]
        g = tot = regions = nch = nfr = silent = 0
        sil_s = 0.0
        fb = bytearray(is_.w * is_.h)
        for ps, pl, tail in is_.payloads:
            pos, stop = ps, ps + pl
            rep, esc, frames = None, 0, 0
            while pos < stop:
                b = is_.d[pos]
                m = b & 0xB8
                if m == ESC:
                    u = struct.unpack_from('<I', is_.d, pos + 1)[0]
                    esc += u
                    g = igcd(g, u)
                    regions += 1
                    pos += 5 + u
                    continue
                if m == END:
                    break
                if m == RPT8:
                    rep = is_.d[pos + 1]
                    pos += 2
                    continue
                if m == RPT16:
                    rep = struct.unpack_from('<H', is_.d, pos + 1)[0]
                    pos += 3
                    continue
                if m == PAL:
                    pos += 769
                    continue
                pos = is_.decode_frame(pos, fb, {})
                frames += 1
            nch += 1
            nfr += frames
            tot += esc
            if not esc:                 # silent chunk: engine keeps the
                silent += 1             # DMA ticking, video still advances
                sil_s += (rep if rep is not None else rep0) / 70.0
        stereo = (g % BLK_ST == 0) if g else True
        bps = 88200.0 if stereo else 44100.0     # 22050 Hz s16 x 2 or x 1
        dur = tot / bps + sil_s
        return dict(w=is_.w, h=is_.h, rep0=rep0, chunks=nch, frames=nfr,
                    regions=regions, silent=silent, tot=tot, gcd=g,
                    stereo=stereo, dur=dur)
    finally:
        if hasattr(is_, 'close'):
            is_.close()


def main():
    args = sys.argv[1:] or ['/tmp/opencode/iso_stage/LEO']
    files = []
    for a in args:
        if a.endswith('.IS'):
            files.append(a)
        else:
            files.extend(sorted(glob.glob(os.path.join(a, '*.IS'))))
    print('%-11s %-9s %6s %6s %6s %9s %7s %9s' %
          ('movie', 'size', 'chunks', 'frames', 'chan', 'audio B',
           'gcd', 'duration'))
    for f in files:
        d = analyse(f)
        print('%-11s %-9s %6d %6d %6s %9d %7d %8.2f s' %
              (os.path.basename(f)[:-3], '%dx%d' % (d['w'], d['h']),
               d['chunks'], d['frames'], 'mono' if not d['stereo'] else 'st',
               d['tot'], d['gcd'], d['dur']))


if __name__ == '__main__':
    main()
