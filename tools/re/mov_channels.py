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

It also decodes every state's `control=` word with the engine's OWN bit
layout (the parser at 0x51b0d: bits 0..2 are a mutually exclusive BY_*
mode, the rest are the named flags), and prints the control-token census
- any token outside ENGINE_CTRL would make the engine print "Unknown
control mode" (0x9223b) - plus the F/T pairs of every BY_X state and
any force that appears with two different durations, which is the one
observable that would separate "the force sets the speed" from "the
force and the duration together do" (NEXT_STEPS PR-99).

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
CTRL = re.compile(r"^\s*control\s*=\s*(.*)$")   # tokens split on | or ,
ROW = re.compile(r"^\s*(\d+)\s*,\s*(-?\d+)\s*(?:,(.*))?$")
DIR = re.compile(r"\b(FC|IC|F|I)\s+(-?\d+)\s*,\s*(-?\d+)")
KEYW = re.compile(r"^\s*[A-Za-z_][A-Za-z0-9_]*\s*=")   # `seq=`, `sample=`
TRANS = re.compile(r"^\s*[A-Za-z0-9_.]+\s*\(")         # `A_X(3)=...`

# The engine's own control word: `mov DWORD [rec+0x12],0` then a chain of
# `or` per token (0x51b0d..0x51e07), read back as `and [rec+0x12],7` for
# the mode and by bit tests for the flags (NEXT_STEPS PR-99). Values are
# the bits of the 4-byte word at rec+0x12, exactly as the engine ORs them.
ENGINE_CTRL = (
    ("BY_X", 0x0), ("BY_Y", 0x1), ("BY_TIME", 0x2), ("BY_TX", 0x3),
    ("BY_TY", 0x4), ("FRAME_REV", 0x8), ("SEQ_REV_X", 0x10),
    ("SEQ_REV_Y", 0x20), ("CHECK_ANI", 0x40), ("CHECK_ZW", 0x80),
    ("CHECK_LN", 0x100), ("CHECK_LEW", 0x200), ("SOLID", 0x400),
    ("CHECK_BASE_LN", 0x4000), ("CH_DIR", 0x800), ("INV_V", 0x1000),
    ("DANGER", 0x2000), ("CHECK_ALL_PLANE", 0x8000),
    ("ANTI_SOLID", 0x10000), ("SOLID2", 0x20400),
    ("SEMI_SOLID", 0x40400), ("ANTI_POCISK", 0x80000),
    ("CHECK_ALL", 0x1C0),
)
ENGINE_NAMES = {n: v for n, v in ENGINE_CTRL}
MODES = {0: "BY_X", 1: "BY_Y", 2: "BY_TIME", 3: "BY_TX", 4: "BY_TY"}


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
            m = CTRL.match(line)
            if m:
                # the engine's loop (0x51dc3..0x51de9) accepts ',' or '|'.
                toks = [t.strip() for t in re.split(r"[|,]", m.group(1))]
                st[1]["ctrl"] = [t for t in toks if t]
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
    tok_ct = {}             # control token -> states, files
    tok_files = {}
    mode_ct = {}            # movement mode -> states
    no_ctrl = [0, 0]        # states without a control= line, all states
    pair_ct = {}            # (mode, F pair, T pair) -> states
    byx_f = {}              # BY_X states: F pair -> the T pairs used with it
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
                no_ctrl[1] += 1
                ctrl = data.get("ctrl")
                if ctrl is None:
                    no_ctrl[0] += 1
                word = 0
                for t in ctrl or []:
                    tok_ct[t] = tok_ct.get(t, 0) + 1
                    tok_files.setdefault(t, set()).add(base)
                    word |= ENGINE_NAMES.get(t, 0)
                mode = MODES.get(word & 7, "?%d" % (word & 7))
                mode_ct[mode] = mode_ct.get(mode, 0) + 1
                if f != (0, 0) or tv != (0, 0):
                    key = (mode, f, tv)
                    pair_ct[key] = pair_ct.get(key, 0) + 1
                    if not (word & 7):
                        byx_f.setdefault(f, set()).add(tv)
                feff = (f[0] + fc[0] + ic[0], f[1] + fc[1] + ic[1])
                tgt = (fr[0] + iv[0], fr[1] + iv[1])
                if not any(tgt) and not (fc[0] or fc[1] or ic[0] or ic[1]):
                    continue      # header-only state: nothing to add
                print("%-34s [%-13s] hdr F=%-7s T=%-7s G=%-7s | "
                      "FC=%-7s IC=%-7s -> F_eff=%-7s | rowF=%-7s I=%-7s "
                      "-> target=%-8s (%d rows)"
                      % (base[:34], name[:13], f, tv,
                         hdr.get("G", (0, 0)), fc, ic, feff, fr, iv, tgt,
                         rows.get("_n", 0)))
    print("-- pairs by kind (pairs found, states with a non-zero sum):")
    for k in ("F", "FC", "I", "IC"):
        t = tot.get(k, [0, 0])
        print("   %-3s %5d %5d" % (k, t[0], t[1]))
    print("-- authored `T=` components above 130, which the engine's parser"
          " caps\n   to 130 (0x525f8/0x52610): %d values in %d files"
          % (capped[0], len(capped[1])))
    print("-- states: %d total, %d of them with no `control=` line at all"
          " (the parser\n   never runs, so the word stays 0 = the BY_X mode)"
          % (no_ctrl[1], no_ctrl[0]))
    print("-- control tokens the files use (states, files) and the bit the"
          " engine ORs for\n   them; UNKNOWN = outside 0x51b0d's list, where"
          " the engine prints \"Unknown\n   control mode\":")
    for t in sorted(tok_ct, key=lambda k: (-tok_ct[k], k)):
        bit = ENGINE_NAMES.get(t)
        print("   %-16s %5d %5d  %s"
              % (t, tok_ct[t], len(tok_files[t]),
                 hex(bit) if bit is not None else "UNKNOWN"))
    print("-- states per movement mode (control bits 0..2):")
    for m, c in sorted(mode_ct.items(), key=lambda kv: (-kv[1], kv[0])):
        print("   %-8s %5d" % (m, c))
    print("-- distinct header (mode, F=, T=) combinations, most common first:")
    for k in sorted(pair_ct, key=lambda k: (-pair_ct[k], k[0], str(k[1]))):
        print("   %-8s F=%-14s T=%-12s %4d states"
              % (k[0], str(k[1]), str(k[2]), pair_ct[k]))
    print("-- the test PR-79 wanted: any BY_X state using one F with two"
          " different\n   T (that would separate the force from the"
          " duration)?")
    multi = {f: ts for f, ts in byx_f.items() if len(ts) > 1}
    if not multi:
        print("   no: each of the %d F values used by a BY_X state pairs"
              " with exactly one T" % len(byx_f))
    for f, ts in sorted(multi.items()):
        print("   F=%s -> T=%s" % (f, sorted(ts)))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
