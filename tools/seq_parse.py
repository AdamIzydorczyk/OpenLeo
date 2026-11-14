#!/usr/bin/env python3
"""seq_parse.py - python reference decoder for the game's .SEQ blobs.

The .SEQ record payloads inside the llrs containers (ANIMACJE, ETAPY,
LEVELDES) are animation-script blobs. Grammar recovered 2026-11-14 from
LEO.EXE raw disassembly (loader FUN_00043e70: buffered read of the whole
payload then u16 element count; element chain FUN_00041ef0 -> FUN_00041f60
with leaf readers FUN_00041d40 / FUN_00041da0 / FUN_00041e00; reads go
through stream vtable slot +0x1c = read(dest, count-BYTES)):

  blob    := u16 count; count x element
  element := u16 w0                       (flag word; only low nibble used)
             u16 b19, u16 b15, u16 b17    (sub-block obj+0x19/+0x15/+0x17)
             u16 f0, u16 f2, u16 f4, u16 f6   (obj+0x00/+0x02/+0x04/+0x06;
                                              f0 = id, 0x7fff = sentinel)
             if w0&4:   u16 f8, u16 fA        (obj+0x08/+0x0a)
             if w0&3:   n = int16(b17);
                        if n>0: n bytes "palette", n bytes "sprite"
             if !(w0&2 && !(w0&1)):          (array reader)
                        m = int16(b19); for i in 0..m (m>=0):
                        u16 L; L bytes

All 49 extracted .SEQ payloads are consumed EXACTLY by this grammar
(consumed == file size, zero leftover). src/ll_seq.c is the C port;
make verify-seq proves C == this reference on every container payload.
Object fields obj+0x0c..0x12/0x14 are DERIVED by the game (FUN_000420b0
bounding-box math), not stored in the blob.

usage: seq_parse.py FILE.SEQ [more.SEQ ...]   -> deterministic dump to stdout
"""
import struct
import sys


class SeqError(Exception):
    pass


def sx(w):
    return w - 0x10000 if w >= 0x8000 else w


def seq_parse(data):
    """Return dict describing the blob, or raise SeqError on truncation."""
    p, n = 0, len(data)

    def u16():
        nonlocal p
        if p + 2 > n:
            raise SeqError("eof u16 @%d" % p)
        v = struct.unpack_from("<H", data, p)[0]
        p += 2
        return v

    def take(k):
        nonlocal p
        if p + k > n:
            raise SeqError("eof bytes @%d len %d" % (p, k))
        v = data[p:p + k]
        p += k
        return v

    count = u16()
    els = []
    for _ in range(count):
        e = {"off": p}
        e["w0"] = u16()
        e["b19"], e["b15"], e["b17"] = u16(), u16(), u16()
        e["f0"], e["f2"], e["f4"], e["f6"] = u16(), u16(), u16(), u16()
        w0 = e["w0"]
        if w0 & 4:
            e["f8"], e["fA"] = u16(), u16()
        if w0 & 3:
            nn = sx(e["b17"])
            e["n"] = nn
            if nn > 0:
                e["pal"] = take(nn)
                e["spr"] = take(nn)
        if not (w0 & 2 and not (w0 & 1)):
            m = sx(e["b19"])
            e["arr"] = []
            for _j in range(m + 1 if m >= 0 else 0):
                L = u16()
                e["arr"].append(take(L))
        els.append(e)
    return {"count": count, "els": els, "consumed": p, "len": n}


def dump_text(data):
    """Deterministic dump of one blob as text (same format as the C
    dumper tools/seq_selftest.c); raises SeqError on truncation."""
    s = seq_parse(data)
    out = ["blob %d count %d" % (s["len"], s["count"])]
    for i, e in enumerate(s["els"]):
        w0 = e["w0"]
        line = ("elem %d off %d w0 %04x b19 %d b15 %d b17 %d f0 %d f2 %d "
                "f4 %d f6 %d" % (i, e["off"], w0, sx(e["b19"]), sx(e["b15"]),
                                 sx(e["b17"]), sx(e["f0"]), sx(e["f2"]),
                                 sx(e["f4"]), sx(e["f6"])))
        if w0 & 4:
            line += " f8 %d fA %d" % (sx(e["f8"]), sx(e["fA"]))
        if w0 & 3:
            line += " n %d" % e["n"]
            if e["n"] > 0:
                line += " pal %s spr %s" % (e["pal"].hex(), e["spr"].hex())
        if "arr" in e:
            line += " arr %d" % len(e["arr"])
        out.append(line)
        for j, a in enumerate(e.get("arr", [])):
            out.append("ent %d len %d data %s" % (j, len(a), a.hex()))
    out.append("end %d" % s["consumed"])
    return "\n".join(out) + "\n"


def dump(data, name):
    try:
        sys.stdout.write(dump_text(data))
    except SeqError as ex:
        print("%s: PARSE-FAIL %s" % (name, ex))
        return 1
    return 0


def main():
    rc = 0
    for path in sys.argv[1:]:
        rc |= dump(open(path, "rb").read(), path)
    return rc


if __name__ == "__main__":
    sys.exit(main())
