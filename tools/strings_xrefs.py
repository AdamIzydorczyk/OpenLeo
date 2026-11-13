#!/usr/bin/env python3
"""strings_xrefs.py - find string anchors + data pointer xrefs in a le_dump .img.

Because le_dump.py already applied all LE fixups, pointers stored in the data
segment are plain little-endian dwords = vaddrs. This gives Watcom-vtable /
sscanf-format / error-message anchors without any loader support.

Output TSV: string_vaddr \t first_ref_vaddr(s) \t string
Usage: strings_xrefs.py IMG [lo hi]   (defaults: LEO image)
"""
import re
import struct
import sys

img_path = sys.argv[1]
lo = int(sys.argv[2], 16) if len(sys.argv) > 2 else 0x10000
hi = int(sys.argv[3], 16) if len(sys.argv) > 3 else 0xd76d0
img = open(img_path, 'rb').read()

# 1) printable strings in image (ISO-8859-2 friendly runs)
strings = []
for m in re.finditer(rb'[\x20-\x7e\x80-\xff]{5,}', img):
    va = lo + m.start()
    if va >= hi:
        break
    s = m.group().rstrip(b'\x00')
    strings.append((va, s))

# 2) dword scan for pointers into image
refs = {}
for off in range(0, len(img) - 3, 4):
    v = struct.unpack_from('<I', img, off)[0]
    if lo + 0x10 <= v < hi:
        refs.setdefault(v, []).append(lo + off)

# 3) join
out = []
for va, s in strings:
    # pointers may point at string start
    r = refs.get(va, [])
    if r:
        out.append((va, r, s))
for va, r, s in sorted(out):
    print(f'{va:#x}\t{",".join(hex(x) for x in r[:6])}\t{s[:70].decode("latin1")}')
print(f'# strings={len(strings)} with_xref={len(out)}', file=sys.stderr)
