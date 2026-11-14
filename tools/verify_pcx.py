#!/usr/bin/env python3
"""verify_pcx.py - prove ported/ll_pcx.c == tools/pcx_dump.py on every PCX
payload stored in the game containers (ANIMACJE, ETAPY ...). Extracts each
id=1 record with a .PCX key from every nested index block, decodes it with
the python reference and with the compiled C tool, and cmps the PPMs.

usage: verify_pcx.py DATA_DIR ./llrs_selftest ./pcx_selftest
"""
import os
import shlex
import struct
import subprocess
import sys


def blocks_of(path):
    """yield (slotname, [(recname, off, size)]) for each id=2 index block."""
    d = open(path, "rb").read()
    cap, actual = struct.unpack_from("<II", d, 8)
    for k in range(cap):
        key, i, o, s = struct.unpack_from("<14sIII", d, 0x14 + 26 * k)
        if i != 2:
            continue
        blk = d[o:o + s]
        if len(blk) < 4:
            continue
        cnt, = struct.unpack_from("<I", blk, 0)
        if 4 + 26 * cnt > len(blk):
            continue
        recs = []
        for j in range(cnt):
            rk, ri, ro, rs = struct.unpack_from("<14sIII", blk, 4 + 26 * j)
            if ri == 1 and rk[:4].upper() != b"\0\0\0\0":
                nm = rk.split(b"\0")[0].decode("latin1")
                if nm.upper().endswith(".PCX"):
                    recs.append((nm, ro, rs))
        if recs:
            yield key.split(b"\0")[0].decode("latin1"), recs


def main():
    data, llrs_t, pcx_t = sys.argv[1], sys.argv[2], sys.argv[3]
    tmp = "/tmp/opencode/pcxver"
    os.makedirs(tmp, exist_ok=True)
    n = fails = 0
    for cont in ("ANIMACJE", "ETAPY"):
        p = os.path.join(data, cont)
        for slot, recs in blocks_of(p):
            d = open(p, "rb").read()
            for nm, off, size in recs:
                raw = os.path.join(tmp, f"{cont}_{slot}_{nm}")
                open(raw, "wb").write(d[off:off + size])
                py = raw + ".py.ppm"
                c = raw + ".c.ppm"
                subprocess.run([sys.executable,
                                os.path.join(os.path.dirname(__file__),
                                             "pcx_dump.py"), raw, py],
                               check=True)
                subprocess.run(shlex.split(pcx_t) + [raw, c], check=True)
                same = open(py, "rb").read() == open(c, "rb").read()
                n += 1
                if not same:
                    fails += 1
                    print(f"MISMATCH {cont}/{slot}/{nm}")
    print(f"PCX check: {n - fails}/{n} payloads byte-exact (C == python)")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
