#!/usr/bin/env python3
"""cfg_ll.py - edit LONGSOFT.CFG (Longsoft Lew Leon settings) safely.

The game derives its data-drive letter from byte 0 of LONGSOFT.CFG:
ASCII drive letter minus 0x40 (0x44='D' -> drive D:).  This tool makes a
patched COPY (original file is never touched).

Usage: cfg_ll.py IN.CFG OUT.CFG --drive D [--show]
"""
import argparse


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('inp')
    ap.add_argument('out', nargs='?')
    ap.add_argument('--drive', choices=[chr(c) for c in range(ord('A'), ord('Z') + 1)])
    ap.add_argument('--show', action='store_true')
    a = ap.parse_args()

    d = bytearray(open(a.inp, 'rb').read())
    print(f'len={len(d)} byte0={d[0]:#04x} ({chr(d[0])!r})')
    if a.show:
        print(d[:16].hex(' '))
    if a.drive:
        if not a.out:
            raise SystemExit('OUT.CFG required with --drive')
        new = ord(a.drive)
        old = d[0]
        d[0] = new
        open(a.out, 'wb').write(d)
        print(f'wrote {a.out}: byte0 {chr(old)!r} -> {chr(new)!r}')
        diffs = sum(1 for x, y in zip(d, open(a.inp, "rb").read()) if x != y)
        print(f'diffs vs input: {diffs}')


if __name__ == '__main__':
    main()
