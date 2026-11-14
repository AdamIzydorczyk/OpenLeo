#!/usr/bin/env python3
"""seq_patch.py - decoder for the .SEQ entry frame-patch blobs (the L-byte
payloads after each element's array counts).

Grammar (from raw disasm of FUN_0001dc80 draw blitter / FUN_0001d9b8 erase
blitter, build/seq_blitters.txt; validated on all 750 entry blobs of the
172 container .SEQ payloads):

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
  Erase variant (1d9b8): same rows but copy runs only (no fills); row
  advance is 0x190 - w.
"""
import sys


class PatchError(Exception):
    pass


def decode(blob):
    """Decode a patch blob -> list of rows; row = (gap, ops) or
    (None, []) for an empty row; op = ('copy', bytes) / ('fill', val, cnt).
    Raises PatchError on any inconsistency."""
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
