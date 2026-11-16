#!/usr/bin/env python3
"""Reader/extractor for LongSoft "llrs" resource containers.

Containers in the game data (verified against originals):
  SOUND      cap=500    291 entries: *.WAV (id=1) + *.MOD (id=1)
  ANIMACJE   cap=200    14  entries: level animation blocks (id=2), all size 0x1450
  ETAPY      cap=200    14  entries: level stage blocks (id=2), all size 0x1450
  LEVELDES   cap=16.6M  17  entries: *.DES level descriptors (id=1) + ENG/GER/POL/POL.ORG text (id=2)

Layout (all little-endian):
  header:
    0x00  magic 'llrs'
    0x04  u32 version (== 1)
    0x08  u32 capacity  (number of 26-byte slots in the table)
    0x0c  u32 zero
    0x10  u32 actual    (number of used entries)
  table: capacity slots of 26 bytes, starting at 0x14:
    0x00  char name[14]   (NUL-padded; unused slots contain zeros OR
                           fixed-seed archiver padding junk - same bytes at
                           the same slot indices across archives, no crypto)
    0x0e  u32 id          (resource class: 1=data file, 2=block/text; 0=free)
    0x12  u32 dataOff
    0x16  u32 size
  data: begins exactly at 0x14 + 26*capacity, entries appended in table order.

Slot validity filter used here (empirical): id != 0, printable name,
size > 0 and offset/size inside the file. Random data bytes occasionally
form name-like patterns, so `id != 0` is the primary discriminator;
`actual` from the header is the authoritative count.

Usage:
  llrs_unpack.py list FILE
  llrs_unpack.py extract FILE OUTDIR [--sha256]
"""
import argparse
import hashlib
import os
import struct
import sys

HDR = struct.Struct('<4sIII I')  # magic, ver, cap, zero, actual (0x14 bytes)
SLOT = struct.Struct('<14sIII')  # name, id, off, size  (26 bytes)


class Container:
    def __init__(self, path):
        self.path = path
        self.f = open(path, 'rb')
        magic, ver, cap, zero, actual = HDR.unpack(self.f.read(HDR.size))
        if magic != b'llrs':
            raise ValueError(f'{path}: not an llrs container (magic {magic!r})')
        if ver != 1:
            raise ValueError(f'{path}: unsupported version {ver}')
        self.ver, self.cap, self.actual = ver, cap, actual
        self.data_start = HDR.size + SLOT.size * cap
        self.entries = self._read_table()

    def _read_table(self):
        self.f.seek(HDR.size)
        raw = self.f.read(SLOT.size * self.cap)
        size = os.fstat(self.f.fileno()).st_size
        ents = []
        for k, (name, eid, off, sz) in enumerate(SLOT.iter_unpack(raw)):
            if eid == 0 or sz == 0:
                continue  # free/padding slot
            nm = name.partition(b'\x00')[0]
            # same key rule as ported/llrs.c key_printable (PR-47): a
            # non-empty printable run, everything after it NUL padding
            if not nm or not all(32 <= c < 127 for c in nm) \
                    or any(name[len(nm):]):
                continue
            if off < self.data_start or off + sz > size:
                continue
            ents.append((k, nm.decode('latin1'), eid, off, sz))
        return ents

    def read(self, ent):
        _, _, _, off, sz = ent
        self.f.seek(off)
        return self.f.read(sz)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('cmd', choices=['list', 'extract'])
    ap.add_argument('file')
    ap.add_argument('outdir', nargs='?', default='-')
    ap.add_argument('--sha256', action='store_true')
    a = ap.parse_args()

    c = Container(a.file)
    print(f'# {a.file}: ver={c.ver} cap={c.cap} actual={c.actual} '
          f'parsed={len(c.entries)} data@{hex(c.data_start)}', file=sys.stderr)
    if a.cmd == 'list':
        for k, nm, eid, off, sz in c.entries:
            print(f'{k:8d}  id={eid}  off=0x{off:x}  size=0x{sz:x}  {nm}')
        return
    if a.outdir in (None, '-'):
        a.outdir = os.path.splitext(os.path.basename(a.file))[0] + '_out'
    os.makedirs(a.outdir, exist_ok=True)
    for k, nm, eid, off, sz in c.entries:
        data = c.read((k, nm, eid, off, sz))
        assert len(data) == sz
        out = os.path.join(a.outdir, nm)
        with open(out, 'wb') as f:
            f.write(data)
        if a.sha256:
            print(f'{hashlib.sha256(data).hexdigest()}  {nm}', file=sys.stderr)
    print(f'# extracted {len(c.entries)} entries to {a.outdir}', file=sys.stderr)


if __name__ == '__main__':
    main()
