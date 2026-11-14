#!/usr/bin/env python3
"""pull_record.py - dump one top-level id=1 record payload from an llrs
container by exact (case-insensitive) key.

usage: pull_record.py CONTAINER KEY OUTFILE
"""
import struct
import sys

SLOT = struct.Struct('<14sIII')


def main():
    cont, key, out = sys.argv[1:4]
    d = open(cont, 'rb').read()
    cap, = struct.unpack_from('<I', d, 8)
    kub = key.upper().encode()
    for k in range(cap):
        name, rid, off, size = SLOT.unpack_from(d, 0x14 + 26 * k)
        if rid and name.split(b'\0')[0].rstrip().upper() == kub:
            open(out, 'wb').write(d[off:off + size])
            return
    sys.exit(f'{key} not in {cont}')


if __name__ == '__main__':
    main()
