#!/usr/bin/env python3
"""llrs_probe.py - structural probe of the Longsoft 'llrs' archive container."""
import sys, struct

def probe(fn):
    d = open(fn, 'rb').read()
    print(f'== {fn} size={len(d):#x}')
    magic, ver, fld2 = struct.unpack_from('<4sII', d, 0)
    print(f'  magic={magic!r} ver={ver} f2={fld2:#x} ({fld2})')
    # region after 12-byte header
    print('  hex @0x0c..0x90:')
    for off in range(0x0c, 0x94, 16):
        row = d[off:off+16]
        print(f'   {off:04x}: ' + ' '.join(f'{b:02x}' for b in row))
    # find where first ASCII name appears
    import re
    m = re.search(rb'[A-Z0-9_]{3,8}\.[A-Z0-9]{2,3}\x00', d)
    if m: print(f'  first name {m.group()[:-1]!r} at {m.start():#x}')
    # constant-region detection: diff this file vs another at same offsets
    return d

a = probe('LL_CD/LEO/SOUND')
b = probe('ANIMACJE')
# byte-diff common prefix to find file-independent structural runs
n = min(len(a), len(b), 0x200)
same = [i for i in range(n) if a[i] == b[i]]
runs, s = [], None
for i in range(n):
    if a[i] == b[i]:
        s = i if s is None else s
    else:
        if s is not None and i - s >= 8: runs.append((s, i))
        s = None
if s is not None and n - s >= 8: runs.append((s, n))
print('identical runs (>=8B) SOUND vs ANIMACJE:', [(hex(x), hex(y)) for x, y in runs])
