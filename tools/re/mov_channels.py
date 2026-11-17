#!/usr/bin/env python3
"""mov_channels.py [DIR ...] - census of the authored MOTION channels.

PR-98. The engine reads a state's motion from TWO channels (NEXT_STEPS
PR-98: the merge at 0x526df..0x5273d and the step at 0x54250):

  F_eff = the state header `F=` pair + the `FC` pairs of its rows
          (+ `IC` pairs, gated by an object flag)
  target = the `F` and `I` pairs of its rows

so a state's authored motion cannot be read from the `F=`/`T=`/`G=` header
alone. This script prints both channels per state (a state whose rows carry
no pairs is skipped - its header is plain in the `.MOV` itself), for every
`.MOV` under DIR (default /tmp/opencode/mov), which is how the PR-98
record's numbers were measured. It also counts the authored `T=` components
that the engine's parser clamps to 130, since that is a data-vs-engine
difference the port does not model (NEXT_STEPS PR-98):

    ZARIA A_FIGHT  F=0,0  +  FC rows summing to +500 x   <- the lunge
    ZARIA A_JUMP   F=180,0 FC 30,0 + F 0,350 (a fall push) + I 150,-1200
    ZARIA A_WALK   F=100,0 T=20,5 with NO row pairs at all
    across the whole set: 272 `F`, 65 `FC`, 60 `I`, 3 `IC` pairs
    and 172 authored `T=` components above the 130 cap that the engine's
    own parser puts on it (0x525f8/0x52610), in 64 of those files

Row syntax is `<frame>,<threshold>,<directives>` (`9,300,FC 100,0`); a
leading `;` comments a row out and is skipped, and directive tokens are
case-insensitive in the data (`F`, `f`). Nothing is written; output only.
Pure stdlib, no game data in the repository.
"""
import os
import re
import sys

ST = re.compile(r"^\s*\[([A-Za-z0-9_]+)\]")
HDR = re.compile(r"^\s*([FTG])=\s*(-?\d+)\s*,\s*(-?\d+)")
ROW = re.compile(r"^\s*(\d+)\s*,\s*(-?\d+)\s*(?:,(.*))?$")
DIR = re.compile(r"\b(FC|IC|F|I)\s+(-?\d+)\s*,\s*(-?\d+)")
KEYW = re.compile(r"^\s*[A-Za-z_][A-Za-z0-9_]*\s*=")   # `seq=`, `sample=`
TRANS = re.compile(r"^\s*[A-Za-z0-9_.]+\s*\(")         # `A_X(3)=...`


def scan(path):
    """Yield (state_name, data) for one .MOV.

    data["hdr"] maps F/T/G -> the header pair; data["rows"] maps
    F/FC/I/IC -> (sum_x, sum_y) over that state's rows, plus "_n" (rows)
    and "<kind>_n" (how many pairs of that kind the rows carry).
    """
    st = None
    with open(path, "r", encoding="latin-1") as f:
        for line in f:
            m = ST.match(line)
            if m:
                if st:
                    yield st
                st = (m.group(1), {})
                continue
            if not st:
                continue
            m = HDR.match(line)
            if m:
                st[1].setdefault("hdr", {})[m.group(1)] = (
                    int(m.group(2)), int(m.group(3)))
                continue
            if KEYW.match(line) or TRANS.match(line):
                continue
            m = ROW.match(line.rstrip())
            if not m:
                continue
            sums = st[1].setdefault("rows", {})
            sums["_n"] = sums.get("_n", 0) + 1
            if not m.group(3):
                continue
            for d in DIR.finditer(m.group(3)):
                k = d.group(1).upper()
                cur = sums.get(k, (0, 0))
                sums[k] = (cur[0] + int(d.group(2)),
                           cur[1] + int(d.group(3)))
                sums[k + "_n"] = sums.get(k + "_n", 0) + 1
                sums["_all"] = sums.get("_all", 0) + 1
    if st:
        yield st


def main(argv):
    dirs = argv[1:] or ["/tmp/opencode/mov"]
    tot = {}
    capped = [0, set()]     # authored T= values the engine clamps to 130
    for root in dirs:
        if os.path.isfile(root):
            files = [root]
        else:
            files = [os.path.join(root, n) for n in sorted(os.listdir(root))
                     if n.upper().endswith(".MOV")]
        for fn in files:
            base = os.path.basename(fn)
            for name, data in scan(fn):
                hdr = data.get("hdr", {})
                rows = data.get("rows", {})
                f = hdr.get("F", (0, 0))
                fc = rows.get("FC", (0, 0))
                ic = rows.get("IC", (0, 0))
                fr = rows.get("F", (0, 0))
                iv = rows.get("I", (0, 0))
                for k in ("F", "FC", "I", "IC"):
                    t = tot.setdefault(k, [0, 0])
                    t[0] += rows.get(k + "_n", 0)
                    t[1] += 0 if rows.get(k, (0, 0)) == (0, 0) else 1
                tv = hdr.get("T", (0, 0))
                for v in tv:
                    if v > 130:      # the engine's parser caps T at 0x82
                        capped[0] += 1
                        capped[1].add(base)
                feff = (f[0] + fc[0] + ic[0], f[1] + fc[1] + ic[1])
                tgt = (fr[0] + iv[0], fr[1] + iv[1])
                if not any(tgt) and not (fc[0] or fc[1] or ic[0] or ic[1]):
                    continue      # header-only state: nothing to add
                print("%-34s [%-13s] hdr F=%-7s T=%-7s G=%-7s | "
                      "FC=%-7s IC=%-7s -> F_eff=%-7s | rowF=%-7s I=%-7s "
                      "-> target=%-8s (%d rows)"
                      % (base[:34], name[:13], f, hdr.get("T", (0, 0)),
                         hdr.get("G", (0, 0)), fc, ic, feff, fr, iv, tgt,
                         rows.get("_n", 0)))
    print("-- pairs by kind (pairs found, states with a non-zero sum):")
    for k in ("F", "FC", "I", "IC"):
        t = tot.get(k, [0, 0])
        print("   %-3s %5d %5d" % (k, t[0], t[1]))
    print("-- authored `T=` components above 130, which the engine's parser"
          " caps\n   to 130 (0x525f8/0x52610): %d values in %d files"
          % (capped[0], len(capped[1])))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
