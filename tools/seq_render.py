#!/usr/bin/env python3
"""seq_render.py - reconstruct sprite bitmaps from .SEQ element patch
blobs and print them as ASCII art (or save PPM).

An element's entries = sprite chunks: chunks 0..b19-1 are 80 px wide,
the last chunk is b15 px wide; all chunks share the same row count
(height). Row advance = 400 (dst pitch) but chunks are contiguous
within their own row set, so we composite per chunk with x offset
80*i and each chunk's own decoded rows (row index = vertical pos).
"""
import sys

from seq_parse import seq_parse, sx
from seq_patch import decode, elem_codec


RAMP = " .:-=+*#%@"


def sprite(elem):
    """-> (width, height, canvas dict) reconstructing the element image."""
    ents = elem.get("arr", [])
    nchunk = sx(elem["b19"])          # loop chunks = b19 count, tail = b15
    w = 80 * nchunk + sx(elem["b15"])
    rows = [decode(a, elem_codec(elem['w0'])) for a in ents]
    h = max((len(r) for r in rows), default=0)
    canv = bytearray(h * w)           # 0 = untouched
    touched = bytearray(h * w)
    for ci, rws in enumerate(rows):
        cw = 80 if ci < nchunk else sx(elem["b15"])
        x0 = 80 * ci
        for ri, (gap, ops) in enumerate(rws):
            if gap is None:
                continue
            px = x0 + gap
            for op in ops:
                kind = op[0]
                if kind == "gap":              # inner gap: advance only
                    px += op[1]
                    continue
                if kind == "copy":
                    val, cnt = op[1], len(op[1])
                    get = lambda k: val[k]
                else:
                    val, cnt = op[1], op[2]
                    get = lambda k: val
                for k in range(cnt):
                    xx = px + k
                    if 0 <= xx < w and ri < h:
                        canv[ri * w + xx] = get(k)
                        touched[ri * w + xx] = 1
                px += cnt
    return w, h, canv, touched


def ascii_art(w, h, canv, touched, cols=112):
    step = max(1, w // cols)
    rows = []
    for y in range(h):
        line = ""
        for x in range(0, w, step):
            i = y * w + x
            line += RAMP[min(9, canv[i] % 10 + (canv[i] // 32))] \
                if touched[i] else " "
        rows.append(line.rstrip())
    return "\n".join(rows)


def main():
    path = sys.argv[1]
    blob = open(path, "rb").read()
    st = seq_parse(blob)
    for i, e in enumerate(st["els"]):
        w, h, canv, touched = sprite(e)
        nt = sum(touched)
        print(f"--- elem {i}: w0=0x{e['w0']:04x} b19={sx(e['b19'])} "
              f"b15={sx(e['b15'])} b17={sx(e['b17'])} "
              f"f0={sx(e['f0'])} f2={sx(e['f2'])} f4={sx(e['f4'])} "
              f"f6={sx(e['f6'])} chunks={len(e.get('arr', []))} "
              f"size {w}x{h} painted {nt}")
        if "--art" in sys.argv and w and h:
            print(ascii_art(w, h, canv, touched))


if __name__ == "__main__":
    main()
