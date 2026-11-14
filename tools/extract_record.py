#!/usr/bin/env python3
"""extract_record.py - pull one payload out of an llrs container.

usage: extract_record.py CONTAINER BLOCK KEY OUTFILE
Finds the id=2 index block named BLOCK (case-insensitive), then the first
id=1 record whose name starts with KEY (case-insensitive), writes its raw
payload to OUTFILE.  Used by the PCM level CI (RAN.01 / PLAN1.PCM).
"""
import struct
import sys

SLOT = struct.Struct('<14sIII')


def main():
    cont, block, key, out = sys.argv[1:5]
    d = open(cont, 'rb').read()
    cap, = struct.unpack_from('<I', d, 8)
    blkub, keyub = block.upper().encode(), key.upper().encode()
    for k in range(cap):
        name, rid, off, size = SLOT.unpack_from(d, 0x14 + 26 * k)
        if rid != 2 or name.split(b'\0')[0].upper() != blkub:
            continue
        cnt, = struct.unpack_from('<I', d, off)
        for t in range(cnt):
            k2, r2, o2, s2 = SLOT.unpack_from(d, off + 4 + 26 * t)
            nm = k2.split(b'\0')[0]
            if r2 == 1 and nm.upper().startswith(keyub):
                open(out, 'wb').write(d[o2:o2 + s2])
                return
    sys.exit(f'{block}/{key} not found in {cont}')


if __name__ == '__main__':
    main()
