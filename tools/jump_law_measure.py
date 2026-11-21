#!/usr/bin/env python3
# PR-253 (arithmetic re-derived by PR-256): measure the authored ZARIA.01
# A_JUMP arc against the LAW.  The expected rise is not a pinned number: it
# is computed here from the authored words alone - the row-2 `I 150,-1200`
# pull pair, the header `T=20,1` y-drag and the header `G=0,55` - through
# the arithmetic 0x54250 actually performs, in ITS OWN units and with its
# own integer steps:
#   the velocity register [obj+0xc2]/[obj+0xc6] is an INTEGER in
#     hundredths of a px per iteration (0x5427f/0x5429b imul + idiv 100,
#     0x54358-0x54372 idiv by 100 again to make the step);
#   the drag term is idiv(T * v, 100) from the OLD v, the `F*s/20` and G
#     terms are added first, the drag is subtracted after (0x542c7,
#     0x542fa, 0x54304, 0x54311), and the sign-cross zero compares the
#     POST-ADD value against the POST-DRAG one (0x5430a, 0x54325);
#   the position step is [obj+0xca] = idiv(v_x,100), [obj+0xce] =
#     idiv(v_y,100), added whole to the position (0x54827-0x54829): a
#     part of a px per iteration is DROPPED, not carried.
# idiv truncates toward zero, so every division here is tdiv, not //.
# 0x54170 from rest SETS the axis, so the two `F 0,350` heads are absorbed
# by the row-2 clamp: v starts at the authored -1200.  The port's own
# ground snap absorbs the last unit on the way down, so the apex is
# compared within 3 px.
import re
import sys


def tdiv(a, b):
    """C's / and x86 idiv: truncate toward zero, not floor."""
    q = abs(a) // abs(b)
    return q if (a < 0) == (b < 0) else -q


log, ground = sys.argv[1], int(sys.argv[2])
raw_v, raw_g, ty = int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5])
v = -raw_v                      # 0x54170 from rest: SETS the axis
y, climb = 0, 0
while v < 0 and climb < 1000:
    drag = tdiv(ty * v, 100)    # the OLD v, idiv
    post = v + raw_g            # G per iteration, in the same units
    v = post - drag
    if post * v < 0:            # the sign-cross zero of 0x5430a/0x54325
        v = 0
    y += tdiv(v, 100)           # [obj+0xce], whole px, no carry
    climb += 1
want = -y                       # px above the launch line

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
