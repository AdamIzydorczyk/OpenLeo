#!/usr/bin/env python3
# PR-253: measure the authored ZARIA.01 A_JUMP arc against the LAW.
# The expected rise is not a pinned number: it is computed here from
# the authored words alone - the row-2 `I 150,-1200` pull pair, the
# header `T=20,1` y-drag and the header `G=0,55` - through the
# measured units (raw/LL_PLAY_RAW_VDIV = /5 for a velocity pair,
# raw*LL_PLAY_RAW_GMUL = *4 for G, one iteration = LL_ITER_MS = 50 ms)
# and the measured order of 0x54250 (drag of the OLD v, then G, then
# the sign-cross zero, PR-252). 0x54170 from rest SETS the axis, so
# the two `F 0,350` heads are absorbed by the row-2 clamp: v starts at
# -1200/5. The port's own ground snap absorbs the last unit on the way
# down, so the apex is compared within 3 px.
import re
import sys

log, ground = sys.argv[1], int(sys.argv[2])
raw_v, raw_g, ty = int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5])
DT = 0.05                       # LL_ITER_MS
v = -float(raw_v) / 5.0         # 0x54170 from rest: SETS the axis
g = float(raw_g) * 4.0 * DT     # per-iteration velocity step
climb, y = 0, 0.0
while v < 0.0 and climb < 1000:
    v = v * (1.0 - ty / 100.0) + g   # drag of the OLD v, then G
    if v * (v - g) < 0.0:            # the sign-cross zero of 0x54325
        v = 0.0
    y += v * DT
    climb += 1
want = -y                          # px above the launch line

mn, air = None, 0
pat = re.compile(r'play tick (\d+): state A_JUMP .* pos (\d+),(\d+) '
                 r'grounded (\d)')
for line in open(log):
    m = pat.search(line)
    if m and m.group(4) == '0':
        air += 1
        yy = int(m.group(3))
        if mn is None or yy < mn:
            mn = yy
if mn is None:
    sys.exit("jump law: no airborne A_JUMP rows in the log")
rise = ground - mn
if abs(rise - want) > 3.0:
    sys.exit("jump law: apex %d px, the authored words give %.1f"
             % (rise, want))
if not (climb <= air <= 4 * climb):
    sys.exit("jump law: air %r iterations outside the law's climb %r"
             % (air, climb))
print("pull law: apex %d px = the closed form of the authored words"
      " (%.1f), air %d iterations, %d of them climbing"
      % (rise, want, air, climb))
