#!/usr/bin/env python3
"""seq_patch.py - decoder for the .SEQ entry frame-patch blobs (the L-byte
payloads after each element's array counts).

Grammar (from raw disasm of the two draw blitters FUN_0001dc80 and
FUN_0001d9b8, build/seq_blitters.txt; validated on every entry blob of
the 1656 container .SEQ payloads). TWO codecs exist and the game picks one
PER ELEMENT from w0&8 (loader FUN_00041f60 stores obj+0x14 = w0&8; the
draw entry FUN_000426a0 branches on it): w0&8 set -> FUN_0001dc80 (below),
w0&8 clear -> FUN_0001d9b8. PR-48 corrected the earlier reading of
FUN_0001d9b8 as an "erase variant": it is the draw codec of the 106 w0=1
elements (the player's LEW*.ZAR/I.SEQ, the BERLO map cursor).

  blob := row*
  row  := (0x80|len) content[len]   length-prefixed rows; the >=0x80 header
                                     doubles as the row-END marker for the
                                     byte-driven blitter parser (end-of-row
                                     = "next byte >= 0x80"), and lets the
                                     blitter SKIP whole rows (its clip
                                     prologue walks (len,payload) entries).
  content := gap run*
    gap  := byte added to dst x-offset
    run  := at op position (row start, or after a gap marker):
              C < 0x80   copy C literal bytes
              F >= 0x80  fill F&0x7f bytes with next byte
            at marker position (directly after any run):
              b < 0x40                -> gap byte b, op follows
              0x40..0x7f after COPY   -> fill (b&0x3f) bytes of next byte
              0x40..0x7f after FILL   -> copy (b&0x3f) literal bytes
              b >= 0x80               -> end of row (that's the next row hdr)
  dst advances 0x190 (400-byte pitch) per row; view base = buf + 0x190.
  FUN_0001d9b8 codec: content := gap (cnt bytes[cnt] gap)*; EVERY byte
  below 0x80 in a gap position is a plain advance (0x40..0x7f are LONG
  gaps, not the short fill/copy markers), counts are plain copy lengths,
  no fill runs; a byte >= 0x80 in either position is the next row header.
  (Verified: walked without the length byte it lands on the declared row
  boundaries in 106/106 such blobs - tools/seq_patch.py d9b8 branch.)
"""
import sys


class PatchError(Exception):
    pass


CODEC_DC80, CODEC_D9B8 = 0, 1


def elem_codec(w0):
    """the game's per-element codec choice (obj+0x14 = w0&8)"""
    return CODEC_DC80 if (w0 & 8) else CODEC_D9B8


def decode(blob, codec=CODEC_DC80):
    """Decode a patch blob -> list of rows; row = (gap, ops) or
    (None, []) for an empty row; op = ('copy', bytes) / ('fill', val, cnt).
    Raises PatchError on any inconsistency."""
    if codec == CODEC_D9B8:
        return _decode_d9b8(blob)
    i, rows = 0, []
    while i < len(blob):
        h = blob[i]
        if not (h & 0x80):
            raise PatchError("row hdr high bit clear at %d (0x%02x)"
                             % (i, h))
        n = h & 0x7F
        i += 1
        end = i + n
        if end > len(blob):
            raise PatchError("row overrun at %d len %d" % (i, n))
        if n == 0:
            rows.append((None, []))            # empty row
            i = end
            continue
        gap = blob[i]
        i += 1
        ops = []
        last_fill = False
        at_op = True
        while i < end:
            if at_op:
                op = blob[i]                       # op position
                i += 1
                if op >= 0x80:                     # fill (legal in content)
                    if i >= end:
                        raise PatchError("fill value past end at %d" % i)
                    ops.append(("fill", blob[i], op & 0x7F))
                    i += 1
                    last_fill = True
                else:                              # copy run
                    if i + op > end:
                        raise PatchError("copy overrun at %d" % i)
                    ops.append(("copy", blob[i:i + op]))
                    i += op
                    last_fill = False
                at_op = False
            else:                                  # marker position
                b = blob[i]
                if b >= 0x80:
                    raise PatchError("marker 0x%02x mid-content at %d"
                                     % (b, i))
                if b < 0x40:
                    i += 1                         # gap byte; op follows
                    ops.append(("gap", b))         # the painter advances px
                    at_op = True
                elif last_fill:                    # short copy
                    cnt = b & 0x3F
                    if i + 1 + cnt > end:
                        raise PatchError("copy-marker overrun at %d"
                                         % (i + 1))
                    ops.append(("copy", blob[i + 1:i + 1 + cnt]))
                    i += 1 + cnt
                    last_fill = False
                else:                              # short fill
                    if i + 2 > end:
                        raise PatchError("fill value past end at %d" % i)
                    ops.append(("fill", blob[i + 1], b & 0x3F))
                    i += 2
                    last_fill = True
        rows.append((gap, ops))
        i = end
    return rows


def _decode_d9b8(blob):
    """FUN_0001d9b8 rows: gap (cnt bytes[cnt] gap)*, no fills."""
    i, rows = 0, []
    while i < len(blob):
        h = blob[i]
        if not (h & 0x80):
            raise PatchError("row hdr high bit clear at %d (0x%02x)"
                             % (i, h))
        n = h & 0x7f
        i += 1
        end = i + n
        if end > len(blob):
            raise PatchError("row overrun at %d len %d" % (i, n))
        if n == 0:
            rows.append((None, []))
            i = end
            continue
        gap = blob[i]
        if gap >= 0x80:
            raise PatchError("gap >= 0x80 at %d" % i)
        i += 1
        ops = []
        while i < end:
            cnt = blob[i]
            if cnt >= 0x80:
                raise PatchError("terminator inside content at %d" % i)
            if i + 1 + cnt > end:
                raise PatchError("copy overrun at %d" % (i + 1))
            ops.append(("copy", blob[i + 1:i + 1 + cnt]))
            i += 1 + cnt
            if i < end:
                g = blob[i]
                if g >= 0x80:
                    raise PatchError("gap >= 0x80 at %d" % i)
                ops.append(("gap", g))
                i += 1
        rows.append((gap, ops))
        i = end
    return rows


def dump_op(op):
    """Render one decoded op in the seq_patchdump text format."""
    if op[0] == "copy":
        return "c" + op[1].hex()
    if op[0] == "gap":
        return "g%d" % op[1]
    return "f%02x.%d" % (op[1], op[2])


def dump_rows(rows):
    """prow lines for one decoded blob (list of (gap, ops))."""
    out = []
    for gap, ops in rows:
        toks = " ".join(dump_op(o) for o in ops) or "-"
        out.append("prow %s %s" % ("-1" if gap is None else gap, toks))
    return out


def dump_payload(payload):
    """Full-file dump matching tools/seq_patchdump.c: for every element
    entry blob 'patch len L rows R' + prows + 'pend' (or FAIL)."""
    import seq_parse
    st = seq_parse.seq_parse(payload)
    out = []
    for e in st["els"]:
        for a in e.get("arr", []):
            try:
                rows = decode(a, elem_codec(e["w0"]))
            except PatchError:
                out.append("patch len %d pend FAIL" % len(a))
                continue
            out.append("patch len %d rows %d" % (len(a), len(rows)))
            out.extend(dump_rows(rows))
            out.append("pend")
    return "".join(s + "\n" for s in out)


def main():
    total = bad = 0
    maxrows = 0
    for path in sys.argv[1:]:
        b = open(path, "rb").read()
        total += 1
        try:
            rows = decode(b)
            maxrows = max(maxrows, len(rows))
        except PatchError as ex:
            bad += 1
            print("BAD %s: %s" % (path, ex))
    print("patch blobs: %d, bad %d, max rows %d" % (total, bad, maxrows))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
