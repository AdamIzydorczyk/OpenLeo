#!/usr/bin/env python3
"""pcx_dump.py - reference PCX decoder oracle (spec for ported/ll_pcx.c).

Extracts and decodes 8bpp single-plane RLE PCX payloads exactly as pinned
on the real game data: rows = (y1+1), line width = x1+1, RLE counter
>=0xC0, data region = payload[64:len-768], palette = last 768 bytes of the
payload (these game files omit the standard 0x0C marker), trailing junk
after the last row is ignored.

usage: pcx_dump.py PAYLOAD.pcx OUT.ppm
"""
import sys


def decode(src: bytes):
    assert src[0] == 0x0A and src[2] == 1, "not RLE PCX"
    # Non-standard game headers: bytes 12..15 deviate from spec; the reliable
    # fields are the window (4..12), bpp@3 and the RLE stream itself.
    x1 = int.from_bytes(src[8:10], "little")
    y1 = int.from_bytes(src[10:12], "little")
    bpp = src[3]
    assert bpp == 8, "only 8bpp"
    w, h = x1 + 1, y1 + 1
    data, end = src[64:len(src) - 768], len(src) - 768
    px = bytearray()
    i = 64
    while i < end and len(px) < w * h:
        b = src[i]
        i += 1
        if b >= 0xC0:
            n, v = b & 0x3F, src[i]
            i += 1
            px += bytes([v]) * n
        else:
            px.append(b)
    px = px[: w * h]
    assert len(px) == w * h, "truncated"
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
