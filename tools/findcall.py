#!/usr/bin/env python3
"""findcall.py - flat-model xref finder for le_dump images.

Watcom 32-bit flat code calls/jumps with rel32 (E8/Jmp E9), and pushes/loads
absolute vaddrs as imm32. le_dump already applied LE fixups, so dwords in
data are absolute vaddrs. This script finds:
  callers : E8 rel32 / E9 rel32 that land on TARGET (also push/imm refs)
  callees : targets referenced FROM a function range (informational)
Usage: findcall.py IMG TARGET [--img-lo 0x10000] [--code-hi 0x82b51]
"""
import struct
import sys

img_path, target = sys.argv[1], int(sys.argv[2], 0)
lo = 0x10000
code_hi = 0x82b51
for i, a in enumerate(sys.argv):
    if a == '--img-lo': lo = int(sys.argv[i+1], 0)
    if a == '--code-hi': code_hi = int(sys.argv[i+1], 0)
img = open(img_path, 'rb').read()

callers = []
for off in range(0, code_hi - lo - 5):
    b = img[off]
    if b in (0xE8, 0xE9):
        rel = struct.unpack_from('<i', img, off + 1)[0]
        va = lo + off
        if va + 5 + rel == target:
            callers.append((va, 'call' if b == 0xE8 else 'jmp'))
    elif b in (0x68, 0x6A):         # push imm (checked below for 68 only)
        if b == 0x68 and struct.unpack_from('<I', img, off + 1)[0] == target:
            callers.append((lo + off, 'push'))
# mov r32, imm32  (B8+r)
for off in range(0, code_hi - lo - 5):
    if 0xB8 <= img[off] <= 0xBF and struct.unpack_from('<I', img, off + 1)[0] == target:
        callers.append((lo + off, 'mov-imm'))
# data pointers
for off in range(code_hi - lo, len(img) - 3, 4):
    if struct.unpack_from('<I', img, off)[0] == target:
        callers.append((lo + off, 'dword-ptr'))

print(f'# callers of {target:#x}: {len(callers)}')
for va, kind in sorted(callers):
    print(f'{va:#x}\t{kind}')
