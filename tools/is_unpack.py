#!/usr/bin/env python3
"""Reader/extractor for LongSoft iNFOSTREAM .IS cutscene containers.
Spec verified 2026-11-14 against all 15 shipped .IS files (chunk chains
walk exactly to EOF) and against the live ISplay runtime chunk table
(hardware-breakpoint trace of the record function at vaddr 0x1b7b0).

Layout (little-endian):
  0x00  header block, 0x5f bytes:
        0x00..0x50: text block (title/author/date strings, varies by file)
        0x51..0x55: fixed bytes 0a 73 19 00 01
        0x56..0x58: u24 field (size-ish; INTRO_P=611394, PRAWA=56384)
        0x59  u16 width   (320 or 640)
        0x5b  u16 height  (200 or 480)
        0x5d  u8  fps     (2..9)
        0x5e  u8  zero
  0x5f  chunk chain, contiguous until EOF:
        +0x00 u8  type (0 in every file)
        +0x01 u32 length  (of the WHOLE chunk record, incl. this 7-byte header)
        +0x05 u16 tail    (per-chunk parameter: audio size/frame info;
                           last still-frame chunks all have len 777, tail 1)
        +0x07 payload, length-7 bytes
Runtime: ISplay streams chunks into a circular cache and records the
PAYLOAD pointer of each chunk in a 10000-slot ring table (DAT 0xc41d4,
read idx 0xcde18, write idx 0xcde1c); table[i] == file offset
(chunk_start + 7) of consecutive chunks, verified byte-exact.

Chunk payloads start with a 5-byte sub-header (video chunks begin
b0 4e 0c 00 00 followed by i16 delta streams; audio chunk 0 is raw PCM).
The frame codec itself lives in the ISplay module (vaddr ~0x1a000-0x1e000,
"iNFOSTREAM real-time decompression module V 2.01 BETA").

Usage:
  is_unpack.py list FILE.IS
  is_unpack.py extract FILE.IS OUTDIR [--index N]
"""
import argparse
import os
import struct
import sys

CHDR = struct.Struct('<BIH')  # type, len(total incl hdr), tail


def walk(data):
    n = len(data)
    p = 0x5F
    while p + 7 <= n:
        typ, ln, tail = CHDR.unpack_from(data, p)
        if ln < 7 or p + ln > n:
            break
        yield p, typ, ln, tail
        p += ln
    if p != n:
        print(f'warning: chain ended at {hex(p)}, EOF at {hex(n)} '
              f'({n - p} bytes trailing)', file=sys.stderr)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('cmd', choices=['list', 'extract'])
    ap.add_argument('file')
    ap.add_argument('outdir', nargs='?', default='-')
    ap.add_argument('--index', type=int, default=-1,
                    help='extract only this chunk index')
    a = ap.parse_args()

    data = open(a.file, 'rb').read()
    if len(data) < 0x60 or data[0x51:0x56] != b'\x0a\x73\x19\x00\x01':
        print(f'{a.file}: not a recognized .IS (header marker mismatch)',
              file=sys.stderr)
    u24 = int.from_bytes(data[0x56:0x59], 'little')
    w, h = struct.unpack_from('<HH', data, 0x59)
    fps = data[0x5D]
    chunks = list(walk(data))
    print(f'# {a.file}: {w}x{h} fps={fps} u24@56={u24} '
          f'chunks={len(chunks)} size={len(data)}', file=sys.stderr)

    if a.cmd == 'list':
        for i, (off, typ, ln, tail) in enumerate(chunks):
            print(f'{i:6d}  off=0x{off:x}  len={ln:8d}  payload={ln-7:8d}  '
                  f'tail=0x{tail:04x}  type={typ}')
        return

    if a.outdir in (None, '-'):
        a.outdir = os.path.splitext(os.path.basename(a.file))[0] + '_chunks'
    os.makedirs(a.outdir, exist_ok=True)
    sel = (enumerate(chunks) if a.index < 0
           else [(a.index, chunks[a.index])])
    for i, (off, typ, ln, tail) in sel:
        name = f'{i:05d}_{tail:04x}.raw'
        with open(os.path.join(a.outdir, name), 'wb') as f:
            f.write(data[off + 7:off + ln])
    print(f'# wrote {len(chunks) if a.index < 0 else 1} chunks to {a.outdir}',
          file=sys.stderr)


if __name__ == '__main__':
    main()
