#!/usr/bin/env python3
"""mov_byx_gate.py [DIR ...] - census of the row-accumulator FEED GATE.

PR-114. The port decides which clock feeds a state's row accumulator with
one expression, written twice per engine (NEXT_STEPS PR-110):

    ll_play.c  section 4  and section 7:
        (st->ctrl & LL_CTRL_BY_X) && st->fx != 0
    ll_level.c obj_advance, `walk0` and the loop's `walk`:
        (s->ctrl & LL_CTRL_BY_X) && s->fx != 0

Its FIRST half is what the machine code says: the dispatcher 0x543a0
feeds ONE accumulator per moving thing, and its branch is chosen by the
`control=` MODE alone - mode 0 (BY_X) adds `|[obj+0xca]|`, mode 1 adds
`|[obj+0xce]|`, modes 2/3/4 add the level constant 100 (PR-99/PR-100).
Its SECOND half, `fx != 0`, is the port's own: a BY_X state whose header
`F=` is 0 falls back to the time feed. PR-110's record carries that as
open item 4 ("has shipped data behind it and no test in front of it").
This script measures the surface that gate covers, so the count is a tool
output and not a claim. Default DIR is /tmp/opencode/mov, the `.MOV` text
extracted from ANIMACJE by tools/re/llrs_extract_mov.c, as in
mov_channels.py.

WHAT IT PRINTS
1. The two row-line patterns, side by side. The C parser takes a row with
   `sscanf(line, "%d,%d")`, which accepts a NEGATIVE first field (the
   authored mirrored frame, `-3,6`); mov_channels.py's row regex
   (`^\\s*(\\d+)\\s*,`) does not. Every row count here uses the C
   pattern, because that is the count the port acts on; the other one is
   printed too, and the difference is the count of rows with a negative
   frame number. This is why the PR-110 record's "290 rows" is larger
   than what `mov_channels.py` reports for the same states.
2. The mode census as the ENGINE reads it (control word bits 0..2, with
   the value a state gets by DEFAULT when it names no mode), and the same
   states as the PORT reads them (`BY_X` token). Named vs defaulted is
   split out: 62 states carry no `control=` line at all, so the engine's
   word stays 0 = mode 0, while the port's bit test is false.
3. The 2x2 of the two readings, in states / rows / files.
4. The PR-110 open item 4 numbers exactly - the state blocks that NAME
   `BY_X` (105 in 69 files) and of those the ones with header `F=0,0`
   (30, 290 rows, 21 files) - and then, per such state: its rows, its
   `T=`/`G=` pair, whether ANY motion source exists in its own text
   (header `F=` or row `F`/`FC`/`IC` pairs), and whether it has an exit
   that does not need its own row list to finish. That last column is
   what the literal mode-only reading would decide: a mode-0 state with
   no motion source adds `|v_x|/100 = 0` to its accumulator every step,
   so its rows never advance, its list never wraps and an exit that waits
   for the wrap can never fire. INFERRED from 0x543a0 + 0x521f0, not
   measured on a capture.

Nothing is written; output only. Pure stdlib, no game data in git.
"""
import os
import re
import sys

ST = re.compile(r"^\s*\[([A-Za-z0-9_]+)\]")
HDR = re.compile(r"^\s*([FTG])=\s*(-?\d+)\s*(?:,\s*(-?\d+))?")
CTRL = re.compile(r"^\s*control\s*=\s*(.*)$")
TRANS = re.compile(r"^\s*([A-Za-z0-9_.]+)\s*\(([^)]*)\)\s*=\s*(.*)$")
KEYW = re.compile(r"^\s*[A-Za-z_][A-Za-z0-9_.]*\s*=")
PAIR = re.compile(r"\b(FC|IC|F)\s+(-?\d+)\s*,\s*(-?\d+)", re.I)
TS = re.compile(r"^\s*;?\s*time_scale\s*=\s*(-?\d+)")

# Row line, as ll_mov_parse reads it: `sscanf(line, "%d,%d")` accepts a
# leading '-' (mirrored frame) and leading blanks, and a line whose first
# character is ';' is the game's own comment and never reaches it.
ROW_C = re.compile(r"^\s*-?\d+\s*,\s*-?\d+")
# The pattern mov_channels.py uses, kept here only to print the gap.
ROW_MC = re.compile(r"^\s*\d+\s*,\s*-?\d+\s*(?:,.*)?$")

# The engine's control word (parser 0x51b0d..0x51e07, mode = word & 7):
# see mov_channels.py for the same table and NEXT_STEPS PR-99 for how it
# was read. The port's LL_CTRL_* bits are an INTERNAL numbering and do not
# match these values; its "BY_X" is the token, so a state that names no
# mode is mode 0 to the engine and "not BY_X" to the port.
ENGINE_NAMES = {
    "BY_X": 0x0, "BY_Y": 0x1, "BY_TIME": 0x2, "BY_TX": 0x3, "BY_TY": 0x4,
    "FRAME_REV": 0x8, "SEQ_REV_X": 0x10, "SEQ_REV_Y": 0x20,
    "CHECK_ANI": 0x40, "CHECK_ZW": 0x80, "CHECK_LN": 0x100,
    "CHECK_LEW": 0x200, "SOLID": 0x400, "CH_DIR": 0x800,
    "CHECK_BASE_LN": 0x4000, "INV_V": 0x1000, "DANGER": 0x2000,
    "CHECK_ALL_PLANE": 0x8000, "ANTI_SOLID": 0x10000, "SOLID2": 0x20400,
    "SEMI_SOLID": 0x40400, "ANTI_POCISK": 0x80000, "CHECK_ALL": 0x1C0,
}
MODES = {0: "BY_X", 1: "BY_Y", 2: "BY_TIME", 3: "BY_TX", 4: "BY_TY"}
MODE_ORDER = ["BY_X", "BY_Y", "BY_TIME", "BY_TX", "BY_TY"]

# Exits that can only fire when the state's own row list finishes: the
# wrap arms AC_BREAK_NEXT/AC_BREAK_PREV (ll_play.c sets `broke` there).
WRAP_CONDS = ("AC_BREAK_NEXT", "AC_BREAK_PREV", "AC_DUP")


def scan(path, stats=None):
    """Yield one dict per `[STATE]` block: rows (C pattern), header pairs,
    control tokens, transitions and the row pairs that could move it.
    `stats`, if given, is a dict of corpus-wide counters this updates."""
    st = None

    def flush():
        if st is None:
            return None
        return st

    for line in open(path, "r", encoding="latin-1"):
        m = ST.match(line)
        if m:
            got = flush()
            if got:
                yield got
            st = {"name": m.group(1), "path": path, "rows": 0, "rows_mc": 0,
                  "hdr": {}, "ctrl": None, "tr": [], "pairs": 0,
                  "thr_c": [], "thr_mc": [], "scale": 1000, "scale_at": 0}
            continue
        if st is None:
            continue
        line = line.rstrip("\r\n")
        m = CTRL.match(line)
        if m:
            st["ctrl"] = [t.strip() for t in re.split(r"[|,]", m.group(1))
                          if t.strip()]
            continue
        m = HDR.match(line)
        if m:
            st["hdr"][m.group(1)] = (int(m.group(2)),
                                     int(m.group(3) or m.group(2)))
            continue
        m = TRANS.match(line)
        if m:
            st["tr"].append((m.group(1), m.group(3).strip()))
            continue
        m = TS.match(line)
        if m:
            # ll_mov_parse reads it only as `strncmp(line, "time_scale", 10)`
            # - column 0 - and rows already built stay unscaled (`scale_at`).
            if stats is not None:
                if line.startswith(";time_scale"):
                    stats["ts_comment"] += 1
                    stats["ts_cfiles"].add(os.path.basename(path))
                else:
                    stats["ts_lines"] += 1
                    stats["ts_files"].add(os.path.basename(path))
                    stats["ts_vals"].add(int(m.group(1)))
            if line.startswith("time_scale"):
                st["scale"] = int(m.group(1))
                st["scale_at"] = len(st["thr_c"])
            continue
        if KEYW.match(line):
            continue
        if ROW_C.match(line):
            st["rows"] += 1
            thr = int(re.match(r"^\s*(-?\d+)\s*,\s*(-?\d+)", line).group(2))
            st["thr_c"].append(thr)
            if ROW_MC.match(line):
                st["rows_mc"] += 1
                st["thr_mc"].append(thr)
            for d in PAIR.finditer(line):
                if d.group(1).upper() != "G":
                    st["pairs"] += 1
    got = flush()
    if got:
        yield got


def word_of(tokens):
    w = 0
    for t in tokens or []:
        w |= ENGINE_NAMES.get(t, 0)
    return w


def conds_of(s):
    """Condition names this state's transitions wait for. The `!` and `|`
    markers (NOT, OR-group) are dropped: this is only about WHICH kinds of
    condition could ever let the state leave."""
    out = []
    for _, expr in s["tr"]:
        for c in re.split(r"[|,]", expr):
            c = c.strip().lstrip("!")
            if c:
                out.append(c)
    return out


def non_wrap_exits(s):
    return [c for c in conds_of(s) if c not in WRAP_CONDS]


def stored_thresholds(s):
    """The thresholds ll_mov_parse ends up storing for this block: the M4-C
    clamp (`ms > 0 ? ms : 100`) and then the block's `time_scale`, applied
    only to the rows at or after the `time_scale=` line (0x51679 `imul`,
    0x5168d `idiv 0x3e8`)."""
    out = []
    for i, v in enumerate(s["thr_c"]):
        t = v if v > 0 else 100
        if i >= s["scale_at"]:
            t = int(t * s["scale"] / 1000)
        out.append(t)
    return out


def thr_counts(vals):
    n = len(vals)
    return (n,
            sum(1 for v in vals if v == 0),
            sum(1 for v in vals if 0 < v <= 99),
            sum(1 for v in vals if v and not v % 100),
            sum(1 for v in vals if v > 0),
            sum(1 for v in vals if v > 20))


def pr110_thresholds(s):
    """The reading PR-110's record used: every row of a block multiplied by
    that block's `time_scale` (integer division), with NO M4-C clamp and no
    rows-before-the-line rule. Kept here because those numbers
    (170 of 18617 at threshold 0, 1715 in 1..99 in 111 files) are quoted in
    a shipped record and a later reader needs the definition that gives them
    - it is NOT what ll_mov_parse stores."""
    return [v * s["scale"] // 1000 for v in s["thr_c"]]


def main(argv):
    dirs = argv[1:] or ["/tmp/opencode/mov"]
    files = []
    for root in dirs:
        if os.path.isfile(root):
            files.append(root)
            continue
        if not os.path.isdir(root):
            print("no corpus at %s - extract the .MOV text first:\n"
                  "  cc -O1 -Iported -o /tmp/opencode/llrs_extract_mov "
                  "tools/re/llrs_extract_mov.c ported/llrs.c ported/fs_ci.c"
                  " && /tmp/opencode/llrs_extract_mov <ANIMACJE>" % root)
            return 1
        for n in sorted(os.listdir(root)):
            if n.upper().endswith(".MOV"):
                files.append(os.path.join(root, n))

    states = []
    stats = {"ts_lines": 0, "ts_comment": 0, "ts_files": set(),
             "ts_cfiles": set(), "ts_vals": set()}
    for fn in files:
        for st in scan(fn, stats):
            states.append(st)

    n_c = sum(s["rows"] for s in states)
    n_mc = sum(s["rows_mc"] for s in states)
    neg_files = {os.path.basename(s["path"]) for s in states
                 if s["rows"] != s["rows_mc"]}
    print("== corpus: %d .MOV files, %d state blocks, %d row lines read the"
          " way\n   ll_mov_parse reads them (`sscanf(\"%%d,%%d\")`, a negative"
          " frame number\n   counts); %d of those %d rows carry one, in %d"
          " files. mov_channels.py's\n   row regex sees %d - the gap is %d"
          " rows it misses."
          % (len(files), len(states), n_c, n_c - n_mc, n_c, len(neg_files),
             n_mc, n_c - n_mc))

    # 1. how the ENGINE reads each state's mode, and how the PORT does.
    eng = {}       # mode name -> [states, rows, files]
    eng_default = {}
    port = {}      # "distance-feed" / "time-feed" -> [states, rows, files]
    cross = {}     # (engine mode, port feed) -> [states, rows, files]
    thr = {}       # mode name -> {"c": [], "mc": [], "stored": []}
    for s in states:
        toks = s["ctrl"]
        mode = MODES[word_of(toks) & 7]
        named_modes = {t.upper() for t in toks or []} & set(MODES.values())
        if toks is None:
            default = "no `control=` line at all"
        elif not named_modes:
            default = "a `control=` list with no mode token in it"
        else:
            default = None
        byx = any(t.upper() == "BY_X" for t in toks or [])
        feed = "distance" if (byx and s["hdr"].get("F", (0, 0))[0] != 0) \
            else "time"
        s["mode"], s["feed"], s["default"] = mode, feed, default
        t = thr.setdefault(mode, {"c": [], "mc": [], "stored": [],
                                  "pr110": []})
        t["c"] += s["thr_c"]
        t["mc"] += s["thr_mc"]
        t["stored"] += stored_thresholds(s)
        t["pr110"] += pr110_thresholds(s)
        for tbl, key in ((eng, mode), (port, feed), (cross, (mode, feed))):
            c = tbl.setdefault(key, [0, 0, set()])
            c[0] += 1
            c[1] += s["rows"]
            c[2].add(os.path.basename(s["path"]))
        if default:
            c = eng_default.setdefault((mode, default), [0, 0, set()])
            c[0] += 1
            c[1] += s["rows"]
            c[2].add(os.path.basename(s["path"]))

    print("\n== the engine's mode (control word bits 0..2, the branch of"
          " 0x543a0 that\n   feeds the accumulator) and how many of those"
          " states name the mode in\n   their `control=` list versus get it"
          " by default:")
    for mode in MODE_ORDER:
        e = eng.get(mode, [0, 0, set()])
        print("   %-9s %6d states %7d rows %6d files"
              % (mode, e[0], e[1], len(e[2])))
        for k in sorted(eng_default, key=lambda k: (-eng_default[k][0], k)):
            if k[0] != mode:
                continue
            c = eng_default[k]
            print("   %-9s %6d states %7d rows %6d files   of which: %s"
                  % ("", c[0], c[1], len(c[2]), k[1]))
    print("\n   the port's gate, the same states read by `(ctrl & BY_X) &&"
          " fx != 0`:")
    for feed in ("distance", "time"):
        c = port[feed]
        print("   %-9s %6d %7d %6d   %s" % (feed, c[0], c[1], len(c[2]),
                                            "walk gate ON" if feed ==
                                            "distance" else
                                            "time feed (incl. every mode-0"
                                            " state the port does not call"
                                            " BY_X)"))
    print("   where the two readings disagree:")
    for k in sorted(cross, key=lambda k: (-cross[k][0], k)):
        c = cross[k]
        if (k[0] == "BY_X") == (k[1] == "distance"):
            continue
        print("   engine mode %-6s / port %-9s %6d states %7d rows"
              " %6d files" % (k[0], k[1], c[0], c[1], len(c[2])))
        named0 = [s for s in states if s["mode"] == k[0] and s["default"]
                  is None and k[1] == "time"
                  and s["hdr"].get("F", (0, 0))[0] == 0]
        parts = [("the token `BY_X` with `F=0,0`", named0)]
        for why in ("no `control=` line at all",
                    "a `control=` list with no mode token in it"):
            parts.append((why, [s for s in states if s["mode"] == k[0]
                                and s["default"] == why]))
        for why, g in parts:
            if not g:
                continue
            print("        %-38s %6d states %7d rows %6d files"
                  % (why, len(g), sum(s["rows"] for s in g),
                     len({os.path.basename(s["path"]) for s in g})))

    # 2. the PR-110 open item 4 numbers.
    named = [s for s in states
             if any(t.upper() == "BY_X" for t in s["ctrl"] or [])]
    zero = [s for s in named if s["hdr"].get("F", (0, 0)) == (0, 0)]
    def agg(lst):
        return (len(lst), sum(s["rows"] for s in lst),
                len({os.path.basename(s["path"]) for s in lst}))
    a, b = agg(named), agg(zero)
    half = [s for s in named
            if s["hdr"].get("F", (0, 0)) != (0, 0)
            and s["hdr"].get("F", (0, 0))[0] == 0]
    print("\n== PR-110 open item 4, reproduced: state blocks that NAME"
          " `BY_X`: %d in %d\n   files. Of those, %d have a non-zero `F=`"
          " (the gate's first half, so both\n   readings feed them the same"
          " way) and %d have `F=0,0`. Blocks naming\n   `BY_X` with"
          " `F=0,<non-zero>`: %d - so the gate's second half, `fx == 0`,"
          "\n   is the same test as the data's `F=0,0` here."
          % (a[0], a[2], a[0] - b[0], b[0], len(half)))
    print("\n   %-34s %-16s %5s %-11s %-12s %-7s %s"
          % ("file", "state", "rows", "T=", "G=", "motion", "exits"))
    for s in sorted(zero, key=lambda s: (os.path.basename(s["path"]),
                                         s["name"])):
        conds = conds_of(s)
        other = [c for c in conds if c not in WRAP_CONDS]
        print("   %-34s %-16s %5d %-11s %-12s %-7s %s"
              % (os.path.basename(s["path"])[:34], s["name"][:16], s["rows"],
                 s["hdr"].get("T", (0, 0)), s["hdr"].get("G", (0, 0)),
                 "in rows" if s["pairs"] else "NONE",
                 "-" if not conds else
                 ("wrap-only: " + ",".join(sorted(set(conds)))
                  if not other else ",".join(sorted(set(other))))))

    nomot = [s for s in zero if not s["pairs"]]
    dead = [s for s in nomot if not non_wrap_exits(s)]
    print("\n   of the %d: %d declare no motion source of their own (no"
          " header `F=` and no\n   row `F`/`FC`/`IC` pair), so a"
          " mode-only feed adds `|v_x|/100 = 0` forever;\n   of those, %d"
          " also have no exit except one the row-list wrap arms (or none at"
          "\n   all), so under the literal mode-only reading they would"
          " never advance a\n   row AND never leave the state:" % (len(zero),
                                                                  len(nomot),
                                                                  len(dead)))
    for s in sorted(dead, key=lambda s: (os.path.basename(s["path"]),
                                        s["name"])):
        print("     %-34s %-16s %5d rows  exits: %s"
              % (os.path.basename(s["path"])[:34], s["name"][:16], s["rows"],
                 ",".join(sorted(set(conds_of(s)))) or "none"))

    # 3. the row threshold, counted under BOTH row patterns: the shipped
    #    records quote the C-pattern total (18617 rows) and the per-mode
    #    threshold table printed by mov_channels.py (15996).
    print("\n== row field 2 (the threshold the engine compares at 0x52320),"
          " per engine\n   mode, under both row patterns. `as written` = the"
          " value in the file;\n   `as stored` = what ll_mov_parse keeps"
          " (M4-C: a non-positive value\n   becomes 100) and then the block's"
          " `time_scale` on the rows at or after\n   its line. This is the"
          " table PR-110 items 1, 2 and 8 are measured with.")
    for mode in MODE_ORDER:
        t = thr.get(mode)
        if not t or not t["c"]:
            continue
        for label, vals in (("as written, C pattern", t["c"]),
                            ("as written, mov_channels", t["mc"]),
                            ("as ll_mov_parse stores", t["stored"]),
                            ("as PR-110 measured", t["pr110"])):
            n, z, lo, x100, _, gt20 = thr_counts(vals)
            print("   %-8s %-26s rows %6d  thr==0 %4d  1..99 %5d  x100 %5d"
                  "  thr>20 %5d" % (mode, label, n, z, lo, x100, gt20))
    tot_pr = [v for t in thr.values() for v in t["pr110"]]
    print("   across every mode: `as written, C pattern` %d rows / %d at"
          " threshold 0;\n   `as PR-110 measured` %d rows / %d in 1..99 in"
          " %d files - the two counts that\n   record's items 1 and 8 quote."
          % (sum(len(t["c"]) for t in thr.values()),
             sum(1 for t in thr.values() for v in t["c"] if v == 0),
             len(tot_pr), sum(1 for v in tot_pr if 0 < v <= 99),
             len({os.path.basename(s["path"]) for s in states
                  if any(0 < v <= 99 for v in pr110_thresholds(s))})))
    vals = sorted(stats["ts_vals"])
    print("   `time_scale=` lines the port reads (column 0, as its\n"
          "   `strncmp(line, \"time_scale\", 10)` requires): %d lines in %d"
          " files; commented\n   with the game's own `;`: %d lines in %d"
          " files. The %d active values run\n   %d..%d, and the last line"
          " in a block is the one that block keeps."
          % (stats["ts_lines"], len(stats["ts_files"]), stats["ts_comment"],
             len(stats["ts_cfiles"]), stats["ts_lines"], vals[0], vals[-1]))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
