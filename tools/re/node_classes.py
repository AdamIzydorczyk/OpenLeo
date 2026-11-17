#!/usr/bin/env python3
"""Census of the engine's "attribute node" class tables (NEXT_STEPS PR-101).

    python3 tools/re/node_classes.py [LEO.ELF]      # default build/leo/leo.elf

What the scan found, read out of the image (see the PR-101 record for the
working model):

* 31 node-class tables: a contiguous run of 30 at stride 0x18 from
  0xa0f28 to 0xa11e0, plus one at 0xa2638 - which is also the tail of
  the record-class sub-table 0xa260c (PR-77's table), the only one of
  the 16 that holds this method set where the others keep their own.
* Every table is 6 slots. Slots +0x08/+0x0c are the two chain getters
  (30x 0x45800 / 0x45870; the exceptions are 0x45970 at +0x08 and
  0x45920 at +0x0c, the two classes that OR their own three dwords into
  the value the chain handed back). Slot +0x10 is the channel query: 28x
  the pass-through walk 0x458e0 and 3 classes that answer - 0x45d50
  (matches the node's own id byte), 0x45d90/0x45dc0 (answer "id 6").
* Slots +0x00 and +0x14 are unique per class: 31 different functions
  each, which is the per-parameter part of the family.

The scan does NOT know anything else about these classes: which authored
property each of the 30 carries is still open.
"""
import struct
import sys

WALK = (0x458E0, 0x45D50, 0x45D90, 0x45DC0)   # slot +0x10: walk / answer
GET = (0x45800, 0x45870, 0x45970, 0x45920)    # slots +0x08 / +0x0c: chain getters
FROM = 0x90000                                 # .rdata begins here in the image


def main():
    path = sys.argv[1] if len(sys.argv) > 1 else "build/leo/leo.elf"
    blob = open(path, "rb").read()

    def rd(va):
        return struct.unpack_from("<I", blob, va - 0xF000)[0]

    hits = []
    for va in range(FROM, len(blob) + 0xF000 - 0x18, 4):
        if rd(va + 0x10) in WALK and (rd(va + 8) in GET or rd(va + 0xC) in GET):
            hits.append(va)

    groups = []
    for va in hits:
        if groups and va == groups[-1][-1] + 0x18:
            groups[-1].append(va)
        else:
            groups.append([va])
    print("node-class tables:", len(hits), "in", len(groups), "groups")
    for g in groups:
        print("  %s .. %s  (%d tables, stride 0x18)"
              % (hex(g[0]), hex(g[-1] + 0x18), len(g)))

    for off in (0x00, 0x04, 0x08, 0x0C, 0x10, 0x14):
        seen = {}
        for va in hits:
            seen[rd(va + off)] = seen.get(rd(va + off), 0) + 1
        print("slot +0x%02x: %d distinct" % (off, len(seen)))
        for fn, n in sorted(seen.items(), key=lambda kv: (-kv[1], kv[0])):
            print("    %s x%d" % (hex(fn), n))


if __name__ == "__main__":
    main()
