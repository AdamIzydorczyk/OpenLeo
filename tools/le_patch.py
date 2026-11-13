#!/usr/bin/env python3
"""le_patch.py - apply small byte patches to a copy of an LE/DOS4GW exe.

Usage: le_patch.py IN.EXE OUT.EXE vaddr:hexbytes ...  (vaddrs are flat image
addresses, as used by le_dump.py / Ghidra)
Each patch: VADDR:HEXNEW   (verified against file; original pages untouched
unless inside the patched range)
"""
import struct
import sys

sys.path.insert(0, __file__.rsplit('/', 1)[0])
import le_dump

def vaddr_to_foff(le, va):
    for p in le.pages:
        if p.get('vaddr') and p['vaddr'] <= va < p['vaddr'] + le.pagesize:
            d = le.h['datapage'] + (p['pageno'] - 1) * le.pagesize
            return d + (va - p['vaddr'])
    raise KeyError(hex(va))

def main():
    src, dst = sys.argv[1], sys.argv[2]
    le = le_dump.LEFile(src)
    data = bytearray(le.data)
    for spec in sys.argv[3:]:
        va_s, hexb = spec.split(':')
        va = int(va_s, 0)
        new = bytes.fromhex(hexb)
        fo = vaddr_to_foff(le, va)
        old = bytes(data[fo:fo + len(new)])
        print(f'{va:#x} -> file {fo:#x}: {old.hex()} -> {new.hex()}')
        data[fo:fo + len(new)] = new
    open(dst, 'wb').write(data)
    print('wrote', dst)

if __name__ == '__main__':
    main()
