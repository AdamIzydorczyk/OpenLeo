#!/usr/bin/env python3
"""Extract ISplay .IS embedded audio.

The .IS opcode stream interleaves ESC(0xb0) ops whose u32-length byte blocks
are RAW 8-bit unsigned PCM (mono).  Regions are contiguous in play order:
the stream's ESC blocks concatenate into the movie's soundtrack
(INTRO_P: 2388 chunks, 8,497,810 bytes).  Sample rate is not stored in the
header; measured ~8000-11025 Hz (chunk cadence x 3150 B/chunk ~ 28 kB/s but
the attract loop stretches to audio playback).  Use --rate to pick.

Usage: is_audio.py IN.IS OUT.WAV [--rate N] [--regions]
"""
import argparse
import os
import struct
import sys
import wave

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import is_decode as I

ESC, END, RPT8, RPT16, PAL = 0xB0, 0xB8, 0x98, 0xA8, 0xA0


def audio_regions(is_):
    """Yield (chunk_no, offset, length) for every ESC PCM block."""
    stat = {}
    fb = bytearray(is_.w * is_.h)
    for k, (ps, pl, tail) in enumerate(is_.payloads, 1):
        pos, stop = ps, ps + pl
        while pos < stop:
            b = is_.d[pos]
            mode = b & 0xB8
            if mode == ESC:
                u = struct.unpack_from('<I', is_.d, pos + 1)[0]
                yield (k, pos + 5, u)
                pos += 5 + u
                continue
            if mode == END:
                break
            if mode == RPT8:
                pos += 2
                continue
            if mode == RPT16:
                pos += 3
                continue
            if mode == PAL:
                pos += 769
                continue
            pos = is_.decode_frame(pos, fb, stat)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('input')
    ap.add_argument('output')
    ap.add_argument('--rate', type=int, default=8000)
    ap.add_argument('--regions', action='store_true',
                    help='print a region map instead of writing audio')
    a = ap.parse_args()

    is_ = I.IS(a.input)
    parts = []
    for k, off, ln in audio_regions(is_):
        if a.regions:
            print(f'chunk {k:5} off {off:#x} len {ln}')
        parts.append(is_.d[off:off + ln])
    if a.regions:
        return
    pcm = b''.join(parts)
    with wave.open(a.output, 'wb') as w:
        w.setnchannels(1)
        w.setsampwidth(1)
        w.setframerate(a.rate)
        w.writeframes(pcm)
    print(f'{a.output}: {len(pcm)} bytes @ {a.rate} Hz '
          f'({len(pcm)/a.rate:.1f}s)')


if __name__ == '__main__':
    main()
