#!/usr/bin/env python3
"""checkfix.py -- report ALL LE fixup source addresses overlapping patch windows.
usage: checkfix.py LEO.EXE 0x5bb20-0x5bb26 ...
Covers single records, source lists, AND chain walks (chain members are the
dwords DOS4GW patches at load -> any patch window touching them is DEAD).
"""
import os, sys, struct
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import le_dump

def main():
    le = le_dump.LEFile(sys.argv[1])
    ranges = []
    for a in sys.argv[2:]:
        if '-' in a:
            s, e = a.split('-')
        else:
            s, e = a, hex(int(a, 16) + 4)
        ranges.append((int(s, 16), int(e, 16)))
    img = le.load_image()
    sentinel = le.pagesize - 1
    bad = 0
    def hit(va):
        nonlocal bad
        for s, e in ranges:
            if s <= va < e:
                print(f"FIXUP-SRC {hex(va)} in window {hex(s)}..{hex(e)}")
                bad += 1
    for p in le.pages:
        if p['fx_start'] >= p['fx_end'] or not p.get('vaddr'):
            continue
        for kind, sf, tf, ordn, extra, add, pg, src in le._records(p):
            if src < 0:
                continue
            if kind == 'chain':
                so, guard = src, 0
                while so != sentinel and so >= 0 and guard < le.pagesize:
                    guard += 1
                    va = p['vaddr'] + so
                    hit(va)
                    info = struct.unpack_from('<I', img, va - le.lo)[0]
                    so = info >> 20
            else:
                hit(p['vaddr'] + src)
    print('OVERLAPS:', bad, '(0 = all windows safe)')
    return 1 if bad else 0

if __name__ == '__main__':
    sys.exit(main())
