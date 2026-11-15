#!/usr/bin/env python3
"""pcx_dump.py - reference PCX decoder oracle (spec for ported/ll_pcx.c).

Extracts and decodes 8bpp single-plane RLE PCX payloads exactly as
truth-pinned 2026-11-15 (PR-36) on the real game data: STANDARD 128-byte
PCX header, window fields @4..12, bpp@3, byte-per-line (stride) u16 @66,
RLE stream from offset 128 to len-768, palette = last 768 bytes of the
payload (these game files omit the standard 0x0C marker). Proof the
stream starts at 128, not 64 (the old, unverified rule): BACK.PCX
decoded from 128 reproduces the captured menu VRAM index plane
byte-identically over the whole non-text area (92.6% of all pixels;
from 64 it matches only 3.8%). Rows are counted at the header stride
and the first w bytes kept - PCX pads scanlines to an even byte count
(MFAKE.PCX 13x13, stride 14); trailing junk (its stray 0x0C) is never
reached.

usage: pcx_dump.py PAYLOAD.pcx OUT.ppm
"""
import sys


def decode(src: bytes):
    assert src[0] == 0x0A and src[2] == 1, "not RLE PCX"
    x1 = int.from_bytes(src[8:10], "little")
    y1 = int.from_bytes(src[10:12], "little")
    bpp = src[3]
    assert bpp == 8, "only 8bpp"
    w, h = x1 + 1, y1 + 1
    stride = int.from_bytes(src[66:68], "little")
    assert stride >= (w + 1) & ~1, "bad stride field"
    end = len(src) - 768
    px = bytearray()
    i = 128
    for _ in range(h):
        row = bytearray()
        while len(row) < stride and i < end:
            b = src[i]
            i += 1
            if b >= 0xC0:
                n, v = b & 0x3F, src[i]
                i += 1
                row += bytes([v]) * n
            else:
                row.append(b)
        assert len(row) >= stride, "truncated"
        px += row[:w]
    return w, h, bytes(px), src[len(src) - 768:]


def main():
    src = open(sys.argv[1], "rb").read()
    w, h, px, pal = decode(src)
    with open(sys.argv[2], "wb") as f:
        f.write(f"P6\n{w} {h}\n255\n".encode())
        for v in px:
            f.write(pal[3 * v:3 * v + 3])


if __name__ == "__main__":
    main()
