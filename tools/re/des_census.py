#!/usr/bin/env python3
"""des_census.py [DESDIR] [MOVDIR] - join authored PLACEMENTS to .MOV states.

PR-126. The F-vs-T law probes need a BY_X object with T other than 40
that the game ACTUALLY spawns (PR-99: at T=40 the two laws give the
same speed, so only T!=40 states can ever decide). mov_channels.py
censused the .MOV side; nothing censused the other half - the level
placement files. A class can ship complete ANIMACJE data and still
never be instantiated: a `.des` line is the only thing that puts a
class on screen. This script joins the two sides and prints measured
counts, so "which T!=40 BY_X object can ever be filmed" is a tool
output and not an inference.

DESDIR holds the `.des` text of EVERY ETAPY level: one file per
container member, named <LEVEL>_<NAME>.DES, dumped with
tools/re/llrs_cat1 <ETAPY> <LEVEL> <NAME>.DES for every member that
tools/re/llrs_tree lists (103 files as measured; PR-126 dumped them to
/tmp/opencode/pr126/des). MOVDIR defaults to /tmp/opencode/mov, the
`.MOV` text extracted from ANIMACJE (same input as mov_byx_gate.py);
its file names carry the same <LEVEL>_<CLASS>_<INST> triple, which is
the join key.

A `.des` placement line looks like `CLASS,grp,name.mov,X,Y`; indented
lines attach keys (damage, bounds, scope, energy) to the line above.
The join keeps (level, class) pairs only. WHAT IT PRINTS:
1. every (level, class, state) that is BY_X with F!=0 AND whose class
   a `.des` of that level places, with both laws' speeds:
   A = |F|/4 px/s (the port's walk law, PR-79) and B = 10*F/T px/s
   (PR-109's iteration model) - they coincide exactly at T=40;
2. the same rows with T=40 folded away: the complete discriminating
   surface, plus the verbatim `.des` line that places each of them;
3. BY_X F!=0 classes that NO `.des` of that level places, split into
   the lion variants (LEW.* - the engine spawns the lion itself, so
   their absence from `.des` is expected) and everything else, which
   is a real orphan: complete data, no reference anywhere.
"""
import glob
import os
import re
import sys

DES = re.compile(r'^\s*([A-Z][A-Z0-9_.]*),[^,]*,[^,]*,(-?\d+),(-?\d+)')


def placements(desdir):
    """{level: {class: [verbatim placement lines]}} from *.DES text"""
    out = {}
    for f in sorted(glob.glob(os.path.join(desdir, '*.DES'))):
        lvl = os.path.basename(f).split('_')[0]
        last = None
        for line in open(f, errors='ignore'):
            m = DES.match(line.rstrip())
            if m:
                last = m.group(1)
                out.setdefault(lvl, {}).setdefault(
                    last, []).append(line.rstrip())
    return out


def states(movdir):
    """(level, class, inst, state, control, F, T) for every F!=0 BY_X"""
    rows = []
    for f in sorted(glob.glob(os.path.join(movdir, '*.MOV'))):
        lvl, cls, inst = os.path.basename(f)[:-4].split('_', 2)
        cur = [None]

        def flush():
            if cur[0] and cur[0][1] and 'BY_X' in cur[0][1] and cur[0][2]:
                rows.append((lvl, cls, inst) + tuple(cur[0]))
            cur[0] = None

        for line in open(f, errors='ignore'):
            line = line.strip()
            if line.startswith('[') and line.endswith(']'):
                flush()
                cur[0] = [line[1:-1], None, None, None]
            elif cur[0] is None:
                continue
            elif line.startswith('control=') and cur[0][1] is None:
                cur[0][1] = line[8:]
            elif line.startswith('F=') and cur[0][2] is None:
                cur[0][2] = first_int(line[2:])
            elif line.startswith('T=') and cur[0][3] is None:
                cur[0][3] = first_int(line[2:])
        flush()
    return rows


def first_int(s):
    try:
        return int(s.split(',')[0])
    except ValueError:
        return 0


def main():
    desdir = sys.argv[1] if len(sys.argv) > 1 else '/tmp/opencode/pr126/des'
    movdir = sys.argv[2] if len(sys.argv) > 2 else '/tmp/opencode/mov'
    des = placements(desdir)
    rows = states(movdir)
    print('des files: %d   .MOV files: %d   BY_X F!=0 states: %d' %
          (len(glob.glob(os.path.join(desdir, '*.DES'))),
           len(glob.glob(os.path.join(movdir, '*.MOV'))), len(rows)))

    uniq = sorted(set((l, c, s, f, t)
                      for l, c, _, s, _, f, t in rows if l in des
                      and c in des[l]))
    print('\n=== placed (level, class, state) with BY_X and F!=0')
    for l, c, s, f, t in uniq:
        print('%-10s %-10s %-12s F=%4d T=%3d  A=%6.1f  B=%6.1f' %
              (l, c, s, f, t, abs(f) / 4.0, 10.0 * f / t if t else 0.0))
    print('placed rows: %d   with T!=40 (law-discriminating): %d' %
          (len(uniq), sum(1 for r in uniq if r[4] != 40)))

    print('\n=== the T!=40 rows, with the verbatim .des line that '
          'places each class')
    for l, c, s, f, t in uniq:
        if t == 40:
            continue
        print('%-10s %-10s %-12s F=%4d T=%3d  A=%6.1f  B=%6.1f' %
              (l, c, s, f, t, abs(f) / 4.0, 10.0 * f / t if t else 0.0))
        for line in des[l][c]:
            print('    %s' % line)

    print('\n=== BY_X F!=0 classes never placed by any .des of their level')
    orphan = sorted(set((l, c) for l, c, _, s, _, f, t in rows
                        if l not in des or c not in des[l]))
    lion = [(l, c) for l, c in orphan if c.startswith('LEW')]
    rest = [(l, c) for l, c in orphan if not c.startswith('LEW')]
    print('lion variants (engine-spawned, expected): %d' % len(lion))
    for l, c in lion:
        print('    %-10s %s' % (l, c))
    print('true orphans: %d' % len(rest))
    for l, c in rest:
        insts = sorted(set(r[2] for r in rows
                           if r[0] == l and r[1] == c))
        print('    %-10s %-10s instances: %s' % (l, c, ','.join(insts)))


if __name__ == '__main__':
    main()
