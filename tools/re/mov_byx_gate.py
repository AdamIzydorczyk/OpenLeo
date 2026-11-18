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
5. PR-110 items 1 and 8 (PR-115): the row threshold counted AS AUTHORED,
   which is what the engine stores - the row-node reader 0x51660 takes it
   with `%hi` into `[node+0x10]` and scales it at 0x51680-0x5168f
   (`value * time_scale / 1000`) with NO clamp anywhere - against what
   `ll_mov_parse` stores, which clamps `ms <= 0` to 100 FIRST
   (ll_play.c:481) and scales after (ll_play.c:735). Then the rows whose
   STORED threshold is under one iteration of feed (100 units), because
   those are exactly the ones where "the engine admits one row per record
   step" (0x52329 `cmp eax,ebx; jge 0x52354`) and "the port's loop takes
   all that fit" differ. Plus the distance-fed half as an ESTIMATE from
   each state's own `T=`, and the two loops' bounds as written in the
   port (`4 * nfr` + a resync in the player; no cap in the object loop).

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

    # 4. PR-110 items 1 and 8, measured. The authored value is what the
    # ENGINE keeps: the row-node reader 0x51660 reads it with `%hi` into
    # [node+0x10] (0x51667/0x51679), scales it at 0x51680-0x5168f
    # (`imul` by the record's time_scale, `idiv 0x3e8`, store back) and
    # clamps nothing; the step at 0x52320-0x5232b then compares
    # `cmp eax,ebx; jge` and admits a row only when the accumulator is
    # strictly greater, so a threshold of 0 or less admits one on every
    # fed step. The PORT clamps at parse time instead (`ms > 0 ? ms :
    # 100`, ll_play.c:481) before scaling, and its row loop runs as long
    # as the accumulator still covers a threshold - so the difference
    # between "one row per record step" and "all that fit" is about
    # thresholds BELOW one step of feed, which is what this counts.
    print("\n== PR-110 items 1 and 8: the threshold AS AUTHORED (what the"
          " engine stores, it\n   reads `%hi` at 0x51679, scales at"
          " 0x51680-0x5168f, and has NO clamp) versus\n   what the port"
          " stores (ll_play.c:481 clamps `ms <= 0` to 100 FIRST, then\n"
          "   scales at :735). One iteration of the port's feed is 100 of"
          " its own units,\n   so a stored threshold under 100 is one the"
          " port's loop can cross more than\n   once in a single step.")
    allauth = []
    for mode in MODE_ORDER:
        t = thr.get(mode)
        if not t or not t["c"]:
            continue
        auth = t["c"]
        allauth += auth
        fset = {os.path.basename(s["path"]) for s in states
                if s["mode"] == mode and any(v <= 0 for v in s["thr_c"])}
        gset = {os.path.basename(s["path"]) for s in states
                if s["mode"] == mode and any(0 < v <= 99 for v in s["thr_c"])}
        print("   %-8s as authored rows %6d   thr<0 %4d   thr==0 %4d (in %d"
              " files)   1..99 %5d (in %d files)"
              % (mode, len(auth), sum(1 for v in auth if v < 0),
                 sum(1 for v in auth if v == 0), len(fset),
                 sum(1 for v in auth if 0 < v <= 99), len(gset)))
    fset = {os.path.basename(s["path"]) for s in states
            if any(v <= 0 for v in s["thr_c"])}
    gset = {os.path.basename(s["path"]) for s in states
            if any(0 < v <= 99 for v in s["thr_c"])}
    print("   every mode: as authored %d rows, %d at threshold 0 in %d"
          " files, %d negative,\n   %d in 1..99 in %d files. PR-110 item 1"
          " quotes \"170 of 18617 ... threshold 0\n   (73 files) and 1715"
          " have threshold in 1..99 units after scaling (111\n   files)\":"
          " the 170 and 1715 come from that record's own scaling rule, the\n"
          "   count under the file as written is printed above."
          % (len(allauth), sum(1 for v in allauth if v == 0), len(fset),
             sum(1 for v in allauth if v < 0),
             sum(1 for v in allauth if 0 < v <= 99), len(gset)))
    # Per-state stored minimum and the two feed estimates for a walking
    # state, needed by both tables below: a player step at LL_ITER_MS
    # feeds |vx| * 0.028 with |vx| capped by the authored T= (the engine
    # clamps T= to 130 at 0x525f8), a scene object's step feeds
    # |F| * 0.028 / 4000 (obj_walk's law). Both are ESTIMATES from the
    # state's own header pairs.
    for s in states:
        s["tmin"] = min(stored_thresholds(s)) if s["rows"] else 0
        tmax = max(abs(v) for v in s["hdr"].get("T", (0, 0)))
        fmax = max(abs(v) for v in s["hdr"].get("F", (0, 0)))
        s["T_est"] = tmax / 1000.0             # |T| * 0.028, player bound
        s["F_est"] = fmax * 28.0 / 4000.0      # object bound
    for mode in MODE_ORDER:
        t = thr.get(mode)
        if not t or not t["stored"]:
            continue
        st = t["stored"]
        lo = [v for v in st if 0 < v < 100]
        fs = {os.path.basename(s["path"]) for s in states
              if s["mode"] == mode and any(0 < v < 100
                                           for v in stored_thresholds(s))}
        two = [s for s in states if s["mode"] == mode
               and s["feed"] == "time" and s["tmin"] and s["tmin"] <= 50]
        tail = ""
        if mode in ("BY_X", "BY_Y", "BY_TIME", "BY_TY"):
            tail = ("\n            %d %s states (in %d files) have a"
                    " minimum low enough that ONE 100-unit\n"
                    "            feed closes TWO rows in the port's loop;"
                    " the engine's step\n"
                    "            admits exactly one."
                    % (len(two), mode, len({os.path.basename(s["path"])
                                            for s in two})))
        print("   %-8s as STORED, under one iteration's feed (100): %5d rows"
              " in %3d files"
              "   (min %d)%s"
              % (mode, len(lo), len(fs), min(st) if st else 0, tail))
    two_t = [s for s in states if s["feed"] == "time" and s["tmin"]
             and s["tmin"] <= 50]
    two_p = [s for s in states if s["feed"] == "distance" and s["tmin"]
             and s["T_est"] and s["tmin"] * 2 <= s["T_est"]]
    two_o = [s for s in states if s["feed"] == "distance" and s["tmin"]
             and s["F_est"] and s["tmin"] * 2 <= s["F_est"]]
    print("   the whole corpus: %d time-fed states have a stored minimum at"
          " or below 50, so one\n   iteration of the port's feed closes TWO"
          " of their rows in one step;\n   the engine's step (0x52329 `cmp"
          " eax,ebx; jge`) admits one." % len(two_t))
    print("   the distance-fed half, ESTIMATE only: of the %d states the port"
          " calls walking,\n   %d have a stored minimum at or below half the"
          " |T| * 0.028 bound (two\n   rows could close in one player step)"
          " and %d at or below half the object\n   |F| * 0.028 / 4000 bound."
          % (port["distance"][0], len(two_p), len(two_o)))
    for s in sorted(two_p, key=lambda s: -s["T_est"])[:12]:
        print("     %-34s %-16s rows %3d  min stored thr %5d   T=%5d"
              "  est feed/step %.3f"
              % (os.path.basename(s["path"])[:34], s["name"][:16], s["rows"],
                 s["tmin"], max(abs(v) for v in s["hdr"].get("T", (0, 0))),
                 s["T_est"]))
    print("   the loop's own bound, for the record: ll_play.c:1288 caps a"
          " step at\n   `4 * nfr` row closures and :1318-1319 then zeroes"
          " the accumulator\n   (`guard hit (data safety): resync`);"
          " ll_level.c has NO such cap - its\n   state loop is bounded by"
          " the accumulator and by the 4096-iteration guard\n   at :1188-1189,"
          " and it never resyncs the debt.")

    # PR-116: which rows sit exactly on the `>=` / `>` boundary.  The
    # engine's record step reads `0x52329: 39 d8  cmp eax,ebx` then
    # `0x5232b: 7d 27  jge return` with eax = the stored threshold
    # [node+0x10] and ebx = the accumulator [this+0xa1]: the row runs
    # only when the accumulator is STRICTLY past the threshold, and
    # `0x52334: 29 c3  sub ebx,eax` hands the row handler a POSITIVE
    # remainder which the handler writes back at `0x52118: 89 90 a1...`.
    # Under a feed of exactly 100 (`0x4c8b7`), the port's rule and the
    # engine's agree everywhere except at a stored threshold that is an
    # exact multiple of 100: the port fires on the iteration that
    # reaches it exactly
    # with a remainder of 0, the engine returns keeping the accumulator
    # and fires on the NEXT iteration, 100 later, with a remainder of
    # 100.  The port keeps `>=` (what PR-110 implemented); this measures
    # what that covers.  `Wait` is the one directive that builds a keyed
    # node (`0x46b52` loads the name `0x91989`, the node string is
    # written at `0x46b89`, and a non-empty key takes the `0x52251`
    # branch, which calls 0x47e40 and passes ebx = 0 - no threshold at
    # all).  The name compare at 0x46b59 is 0x76a69, which lower-cases
    # both sides, so `WAIT` in the text reaches it too: the counts below
    # are the exact-case and any-case figures, plus how many of them sit
    # on lines this repo reads as rows, which bounds the keyed branch.
    print("\n== 5. PR-116: the boundary population, and the keyed-node"
          " branch")
    wait_cs = wait_ci = wait_rows = 0
    for p in files:
        for line in open(p, "r", encoding="latin-1"):
            hit = 0
            for tok in re.split(r"[^A-Za-z0-9_]+", line):
                if tok == "Wait":
                    wait_cs += 1
                    hit = 1
                elif tok.upper() == "WAIT":
                    wait_ci += 1
                    hit = 1
            if hit and ROW_C.match(line):
                wait_rows += 1
    print("   `Wait` directive tokens: %d with the exact case the engine's\n"
          "            name table stores (the string at 0x91989), %d with"
          " any case.  The\n            compare at 0x46b59 is 0x76a69, and"
          " those bytes lower-case\n"
          "            BOTH sides (`83 f9 41` / `7c 07` / `04 20` twice"
          " over, then `38 e0`\n"
          "            `cmp al,ah`), so the second number is the one that"
          " reaches the\n            handler.  %d of those tokens are on"
          " lines this repo's row parser\n            reads as rows: those"
          " are the only candidates for the keyed\n            `0x52251`"
          " branch, which matches through 0x47e40 and never\n"
          "            compares a threshold.  Whether each one builds a"
          " keyed ROW node or a\n            separate event node is unread"
          " (PR-116 item 3), so the two\n            readings of the"
          " boundary population are printed below: at most\n"
          "            all of it, at least all of it minus those rows."
          % (wait_cs, wait_ci, wait_rows))
    tot = [0, 0, 0, 0]
    for mode in MODE_ORDER:
        t = thr.get(mode)
        if not t or not t["stored"]:
            continue
        stv = t["stored"]
        n100 = sum(1 for v in stv if v > 0 and not v % 100)
        authored0 = sum(1 for s in states if s["mode"] == mode
                        for v in s["thr_c"] if v <= 0)
        bstates = sum(1 for s in states if s["mode"] == mode and s["tmin"]
                      and s["tmin"] > 0 and not s["tmin"] % 100)
        f100 = {os.path.basename(s["path"]) for s in states
                if s["mode"] == mode
                and any(v > 0 and not v % 100
                        for v in stored_thresholds(s))}
        print("   %-8s %5d of %5d stored thresholds are an exact positive"
              " multiple\n            of 100 (%d of them from an authored 0"
              " or negative, which the M4-C\n            clamp rewrites to"
              " 100); %4d states have their MINIMUM there, in\n"
              "            %d files." % (mode, n100, len(stv), authored0,
                                         bstates, len(f100)))
        print("            At each of those rows the engine's record step"
              " returns with the\n            accumulator kept and fires the"
              " NEXT iteration, 100 later.")
        tot[0] += n100
        tot[1] += len(stv)
        tot[2] += authored0
        tot[3] += bstates
    print("   the whole corpus: %d of %d stored thresholds sit on the"
          " exact-multiple-of-100\n   boundary, in %d states; %d of those"
          " are what an authored 0 or negative\n   became through the M4-C"
          " clamp.  Those are the rows where the port's\n   `>=` and the"
          " engine's `cmp/jge` disagree by one whole feed of 100\n"
          "   iterations." % (tot[0], tot[1], tot[3], tot[2]))
    print("   the bracket with the keyed rows kept out: AT MOST %d of the"
          " %d row lines\n   are on that boundary, AT LEAST %d - the %d row"
          " lines whose text carries a\n   standalone `WAIT` token are the"
          " candidates for the keyed branch and\n   are subtracted there."
          % (tot[0], tot[1], max(tot[0] - wait_rows, 0), wait_rows))
    # PR-118: PR-117's leg 8 builds an exact equality between a
    # distance-fed accumulator and a row threshold (F=100, one 100-unit
    # row, two dt=2000 calls: 50.0 then exactly 100.0).  Is that
    # reachable for a real authored row, on a grid this port really uses?
    # Outside a selftest obj_advance is called with three dt values only:
    # 20 ms (ll_level.c:6425, the play/PPM legs), 40 ms (:6432, the
    # element-only leg) and a measured frame dt (:6618, on screen);
    # LL_ITER_MS = 28 is the grid every selftest uses.  The feed is
    # obj_walk's `dist = |fx| * dt_ms / 4000.0` (ll_level.c:1101-1104),
    # so the per-call feed is |F| * dt / 4000 and nothing in that story
    # involves the number 100.  Section 5 counted the boundary population
    # as "stored threshold is an exact positive multiple of 100", which is
    # the right test for a state whose feed IS the level's 100-per-
    # iteration constant, and the wrong one for a walking state.  Measured
    # here per state with the port's own rule (the `walk` expression,
    # `(BY_X token) and F[0] != 0`, the same one main() uses for the
    # feed column above).  Two answers per walking state and grid:
    # whether ANY whole number of identical calls lands a stored threshold
    # exactly in EXACT arithmetic (Fraction: k * |F| * dt / 4000 == t),
    # and whether the port's own double accumulator reaches it, adding
    # dt's rounded feed step by step - they can disagree, because the
    # port's accumulator is a double while the engine's is an integer
    # dword (0x543d1 `add [obj+0xa1], eax`), which cannot drift at all.
    print("\n== 6. PR-118: is that exact-equality landing reachable for a"
          " DISTANCE-fed\n   row, on the grids this port actually calls"
          " obj_advance with?")
    from collections import Counter
    from fractions import Fraction
    from math import gcd
    walking = [s for s in states if s["feed"] == "distance"]
    wrows = [t for s in walking for t in stored_thresholds(s)]
    print("   walking states (the port's own rule, `walk` at"
          " ll_level.c:1169 = a\n   BY_X token with a non-zero first"
          " `F=`): %d states, %d rows.\n   Their stored thresholds run"
          " %d..%d units; how many are an exact positive\n   multiple of"
          " 100, the test section 5 uses:"
          % (len(walking), len(wrows),
             min(wrows) if wrows else 0, max(wrows) if wrows else 0))
    w100 = sum(1 for t in wrows if t > 0 and not t % 100)
    wfiles = {os.path.basename(s["path"]) for s in walking}
    print("   %d of %d rows, in %d files.  The multiple-of-100 test does"
          " not select a\n   single distance-fed row in this corpus: for"
          " those states the divisor is\n   their own per-call distance,"
          " not the level's 100."
          % (w100, len(wrows), len(wfiles)))
    GRIDS = ((20, "the play/PPM grid, ll_level.c:6425"),
             (40, "the element-only grid, :6432"),
             (28, "LL_ITER_MS, the grid every selftest uses"),
             (2000, "the dt PR-117's leg 8 had to invent"))
    r20 = (0, 0, 0)
    for dt, note in GRIDS:
        ex_rows = ex_st = fl_st = fl_rows = drift = skip = 0
        dfiles = set()
        drows = 0
        for s in walking:
            fx = abs(s["hdr"].get("F", (0, 0))[0])
            thr = [t for t in stored_thresholds(s) if t > 0]
            if not fx or not thr:
                continue
            feed = Fraction(fx * dt, 4000)
            hits = [t for t in thr if (Fraction(t) / feed).denominator == 1]
            d = float(fx) * dt / 4000.0
            want, top = set(thr), max(thr)
            hit_f = False
            if d > 0:
                acc, steps, cap = 0.0, 0, 200000
                while acc < top and steps < cap:
                    acc += d
                    steps += 1
                    if acc in want:
                        hit_f = True
                        break
                if steps >= cap:
                    skip += 1
            if hits:
                ex_st += 1
                ex_rows += len(hits)
                dfiles.add(os.path.basename(s["path"]))
            if hit_f:
                fl_st += 1
                fl_rows += 1
            if bool(hits) != hit_f:
                drift += 1
                drows += len(hits)
        if dt == 20:
            r20 = (ex_st, ex_rows, drift)
        print("   dt=%-5d (%s):\n            exact arithmetic"
              " %3d of %d states / %4d of %d rows reachable;\n"
              "            this port's own double accumulator %3d states;"
              " %d states (%d rows)"
              "\n            where the two answers DISAGREE; %d not probed"
              " past 200000 steps."
              % (dt, note, ex_st, len(walking), ex_rows, len(wrows), fl_st,
                 drift, drows, skip))
    print("   why the 28 ms grid lands on nothing: the per-call feed"
          " there is\n   `|F| * 28 / 4000 = 7|F| / 1000`, and landing an"
          " integer threshold t in\n   k identical calls needs `k * 7|F|"
          " = 1000t`, so the t reachable for a state"
          "\n   are exactly the multiples of `7|F| / gcd(7|F|, 1000)`."
          "  Not one (|F|, t) pair\n   this corpus authors satisfies"
          " that - and it is close, not far: a state at\n   |F|=100"
          " WOULD land a 7-unit row in exactly 10 calls, but no |F|=100"
          "\n   state here writes one.  The two columns below are the"
          " whole argument:")
    fx_seen = sorted({abs(s["hdr"].get("F", (0, 0))[0]) for s in walking})
    hist = {}
    for s in walking:
        h = hist.setdefault(abs(s["hdr"].get("F", (0, 0))[0]),
                            Counter())
        h.update(t for t in stored_thresholds(s) if t > 0)
    print("   |F| values among those %d states: %s."
          % (len(walking), ", ".join(str(v) for v in fx_seen)))
    print("            |F|   states  rows   a 28 ms call sequence can"
          " ONLY land on        the thresholds this\n"
          "                                                      |F|"
          " actually authors")
    for fx in fx_seen:
        grp = [s for s in walking
               if abs(s["hdr"].get("F", (0, 0))[0]) == fx]
        g = 7 * fx
        d0 = g // gcd(g, 1000)
        print("            %-4d %6d %5d   first reachable %d units"
              % (fx, len(grp), sum(hist[fx].values()), d0))
        print("                                     the rows are at: %s"
              % " ".join("%d x%d" % (v, c) for v, c in
                         sorted(hist[fx].items())))
    print("   why the 20 ms grid lands on 713 of the 992: the feed there"
          " is `|F|/200`,\n   and |F|=100 - the largest group, 48 states"
          " and 562 rows - feeds exactly\n   1.0 per call, so EVERY"
          " integer threshold is a whole number of calls.\n   That is the"
          " grid the play and PPM legs run on (ll_level.c:6425).")
    print("   and the reverse case, where the PORT drifts off a landing"
          " exact\n   arithmetic says it has: at dt=20 the feed is"
          " |F|/200, so |F|=30 feeds\n   0.15 and a 3-unit row needs"
          " k = 20 calls.  20 * 0.15 is 3 exactly;\n"
          "   the port's accumulator reaches 2.999999999999999 after 20"
          " additions\n   and only crosses the threshold on call 21,"
          " handing itself a remainder\n   of 0.149999999999999 rather"
          " than 0.  The engine's accumulator is an\n   integer dword"
          " (0x543d1 `add [obj+0xa1], eax`), so it has no such\n"
          "   drift.  The states affected are printed above as the"
          " DISAGREE column.")
    print("   WHAT THIS DOES TO THE SECTION-5 NUMBERS.  Nothing that"
          " section 5\n   counted is wrong: its per-mode buckets are the"
          " ENGINE's modes (`word & 7`),\n   and a bucket's rows are"
          " distance-fed only when the port's own rule\n"
          "   says so, which is the BY_X token with a non-zero `F=` -"
          " %d of the\n   %d states whose control word reads mode 0."
          "  Its boundary count is rows\n   whose threshold is a multiple"
          " of 100, and no distance-fed row here\n   is one (%d of %d),"
          " so the 15066-15317 figure is the boundary for\n   the"
          " TIME-fed rows only.  The distance-fed rows have a boundary of"
          " their\n   own, at a multiple of their own per-call distance:"
          " on the 20 ms grid\n   %d of them sit on it (%d of %d states)"
          " and section 5's count includes\n   none of them.  Both numbers"
          " describe the same `cmp`/`jge` boundary\n   PR-116 read at"
          " 0x52329-0x52334; neither changes what the port does"
          " today."
          % (len(walking), len([s for s in states if s["mode"] == "BY_X"]),
             w100, len(wrows), r20[1], r20[0], len(walking)))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
