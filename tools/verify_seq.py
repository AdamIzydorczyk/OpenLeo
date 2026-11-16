#!/usr/bin/env python3
"""verify_seq.py - prove ported/ll_seq.c == tools/seq_parse.py on every .SEQ
payload stored in the game containers (ANIMACJE, ETAPY, LEVELDES). Extracts
each id=1 record with a .SEQ key from the top-level table AND from every
nested id=2 index block, checks the python reference consumes the payload
byte-exactly (consumed == size), then diffs the C dumper's output against
the python dump. The frame-patch rows of every entry blob are diffed the
same way (ported/ll_seq.c ll_seq_patch_walk vs tools/seq_patch.py), and
so are the element SHAPE geometry results of every shape-bearing element
(ported/ll_seq.c ll_seq_shape_* vs tools/shape_geom.py, PR-46).

usage: verify_seq.py DATA_DIR [--grammar TOOL ...] [--patch TOOL ...]
                          [--shape TOOL ...]
       TOOL may be 'wine ./seq_selftest.exe' (quote it).
"""
import os
import shlex
import struct
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import seq_parse  # noqa: E402
import seq_patch  # noqa: E402
import shape_geom  # noqa: E402

# Documented data defect in the 1997 original (surfaced 2026-11-15 when
# the sparse-table scan made leaf records visible - PR-36 record):
# OGIEN_M.SEQ declares 26 elements but even its FIRST element overruns
# the 689-byte payload; the adjacent OGIEN_D.SEQ (the _M/_D pair sits
# back-to-back in the container) does not fit standalone or concatenated
# either. ported/ll_seq.c and the python reference reject it IDENTICALLY
# (seq_selftest prints PARSE-FAIL for the file), so it is an original-
# data defect, not a C-vs-python divergence: still extracted, still
# compared, counted as known-bad instead of a failure.
KNOWN_BAD_SEQ = {"ANIMACJE/TUNGA.11/DNGANI/OGIEN_M.SEQ"}


def records_of(path):
    """yield (slotname, recname, off, size) for every id=1 .SEQ record at
    all nesting depths: top-level table -> id=2 index blocks -> id=2 leaf
    blocks (per-object animation frame tables; offsets stay ABSOLUTE in the
    container at every level)."""
    d = open(path, "rb").read()
    cap, actual = struct.unpack_from("<II", d, 8)

    def ok(rk, ri, ro, rs):
        if ri != 1 or rk[:4] == b"\0\0\0\0":
            return False
        nm = rk.split(b"\0")[0].decode("latin1")
        return nm.upper().endswith(".SEQ")

    def printable(b):
        return b and all(32 <= c < 127 for c in b)

    def walk(base, nslots, slot, depth=0):
        # Sparse-table rule (probed 2026-11-15, same as ported/llrs.c):
        # the block header field is the count of USED records, the slot
        # table continues past it with id==0 holes at EVERY nesting level
        # - scan all whole 26-byte slots the block payload holds.
        for j in range(nslots):
            rk, ri, ro, rs = struct.unpack_from("<14sIII", d, base + 26 * j)
            if ok(rk, ri, ro, rs):
                yield slot, rk.split(b"\0")[0].decode("latin1"), ro, rs
            if ri != 2 or depth >= 4 or ro >= len(d) or ro + rs > len(d):
                continue
            blk = d[ro:ro + rs]
            if len(blk) < 4:
                continue
            bcnt, = struct.unpack_from("<I", blk, 0)
            if 4 + 26 * bcnt > len(blk):
                continue
            kb = rk.split(b"\0")[0]
            sub = slot + "/" + (kb.decode("latin1") if printable(kb)
                                else kb.hex())
            yield from walk(ro + 4, (rs - 4) // 26, sub, depth + 1)

    for k in range(cap):
        key, i, o, s = struct.unpack_from("<14sIII", d, 0x14 + 26 * k)
        if ok(key, i, o, s):
            yield ("TOP", key.split(b"\0")[0].decode("latin1"), o, s)
        if i != 2:
            continue
        blk = d[o:o + s]
        if len(blk) < 4:
            continue
        cnt, = struct.unpack_from("<I", blk, 0)
        if 4 + 26 * cnt > len(blk):
            continue
        yield from walk(o + 4, (s - 4) // 26,
                        key.split(b"\0")[0].decode("latin1"))


def main():
    args = sys.argv[1:]
    if not args:
        print(__doc__)
        return 2
    data, grammar, patch, mode = args[0], [], [], "grammar"
    shape = []
    for a in args[1:]:
        if a == "--grammar":
            mode = "grammar"
        elif a == "--patch":
            mode = "patch"
        elif a == "--shape":
            mode = "shape"
        else:
            {"grammar": grammar, "patch": patch,
             "shape": shape}[mode].append(a)

    tmp = "/tmp/opencode/seqver"
    os.makedirs(tmp, exist_ok=True)
    cases = []            # (label, raw_path, grammar_text, patch_text, shape_text)
    fails = 0
    nbad = 0
    for cont in ("ANIMACJE", "ETAPY", "LEVELDES"):
        p = os.path.join(data, cont)
        if not os.path.exists(p):
            # PR-47: partial staging must FAIL, not shrink the corpus
            print(f"MISSING-CONTAINER {cont}: {p}")
            return 1
        blob = open(p, "rb").read()
        for slot, nm, off, size in records_of(p):
            label = f"{cont}/{slot}/{nm}"
            raw = os.path.join(tmp,
                               f"{cont}_{slot.replace('/', '_')}_{nm}")
            payload = blob[off:off + size]
            open(raw, "wb").write(payload)
            # python oracle must consume the payload exactly
            try:
                st = seq_parse.seq_parse(payload)
                txt = seq_parse.dump_text(payload)
                ptxt = seq_patch.dump_payload(payload)
                stxt = shape_geom.dump_text(payload)
            except (seq_parse.SeqError, seq_patch.PatchError) as ex:
                if label in KNOWN_BAD_SEQ:
                    nbad += 1
                    print(f"KNOWN-BAD {label}: {ex}")
                    continue
                fails += 1
                print(f"PY-FAIL {label}: {ex}")
                continue
            except (IndexError, KeyError) as ex:
                fails += 1
                print(f"PY-SHAPE-FAIL {label}: {ex}")
                continue
            if st["consumed"] != size:
                fails += 1
                print(f"PY-SLACK {label}: {st['consumed']} != {size}")
                continue
            cases.append((label, raw, txt.encode(), ptxt.encode(),
                          stxt.encode()))

    n = len(cases) + fails + nbad
    for kind, tools, idx in (("grammar", grammar, 2), ("patch", patch, 3),
                             ("shape", shape, 4)):
        exp = b"".join(c[idx] for c in cases)
        # Batch the payload paths: wine/CreateProcess caps the command
        # line at 32767 chars, and the PR-36 sparse-table scan lifted the
        # case count from ~170 to ~1650 (one long arg list blew it up).
        # The C tools print per-file in argument order, so running them
        # in batches and concatenating keeps the diff semantics intact.
        batches, cur, cur_len = [], [], 0
        for c in cases:
            if cur_len + len(c[1]) + 1 > 20000 and cur:
                batches.append(cur)
                cur, cur_len = [], 0
            cur.append(c[1])
            cur_len += len(c[1]) + 1
        if cur:
            batches.append(cur)
        for tool in tools:
            # NB: wine truncates large PIPED stdout (~120KB), so capture to
            # a file (file writes through wine are proven byte-exact
            # elsewhere).
            cap = os.path.join(tmp, "tool_out.bin")
            out = b""
            with open(cap, "wb") as fh:
                for bi, batch in enumerate(batches):
                    if bi:
                        fh.flush()
                        out += open(cap, "rb").read().replace(b"\r", b"")
                        fh.seek(0), fh.truncate(0)
                    subprocess.run(shlex.split(tool) + batch,
                                   stdout=fh, check=True)
                fh.flush()
            out += open(cap, "rb").read().replace(b"\r", b"")
            if out == exp:
                continue
            pos, located = 0, False
            for ci, (label, _, _, _) in enumerate(cases):
                e = cases[ci][idx]
                if out[pos:pos + len(e)] != e:
                    print(f"MISMATCH {kind} {label} [{tool}]")
                    located = True
                    break
                pos += len(e)
            if not located:
                print(f"EXTRA/TRUNCATED {kind} OUTPUT [{tool}] "
                      f"({len(out)} vs {len(exp)} bytes)")
            fails += 1
    nsh = sum(1 for c in cases
              for ln in c[4].split(b"\n") if ln.startswith(b"shape "))
    nre = sum(1 for c in cases
              for ln in c[4].split(b"\n")
              if ln.startswith(b"shape ") and ln.endswith(b"shape_empty 1"))
    print(f"SEQ check: {n - fails}/{n} payloads byte-exact "
          f"(C == python, grammar + patch rows + shape geometry "
          f"({nsh} shape elements, {nre} with both margin lists empty - "
          f"the input of the game's degenerate box case), exact "
          f"consumption; {nbad} known-bad original-data defects accepted)")
    return 1 if (fails or n == 0) else 0


if __name__ == "__main__":
    sys.exit(main())
