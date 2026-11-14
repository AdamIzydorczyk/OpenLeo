#!/usr/bin/env bash
# build_data_disk.sh - create the FAT16 "data HDD" image for the no-CD install.
#
# Layout produced (verified working with QEMU rig):
#   - 1 GiB qcow2, DOS MBR (sfdisk: start=63, type=6, bootable)
#   - partition = FAT16 (mtools mformat -F, 32KB clusters)
#   - \LEO\ directory holding the 19 flat game files copied off the CD
#     (ANIMACJE, ETAPY, SOUND, LEVELDES + 15 *.IS) - llrs archives are
#     single files, no subdirs needed.
# Game side: LONGSOFT.CFG byte0 = drive letter of this disk (tools/cfg_ll.py).
# The CD-presence gate: a pristine LEO.EXE prints "Blad Programu..." without
# an ASPI manager, and NO subset of the chain passes without ASPI (the
# 0x5bd5c:9090 NOP leaves the je reading the call's stale ZF; the
# 0x5bd5c:eb029090 jmp still reaches the INT21 4408h/4409h probes, which
# fail without a CD driver there). The deterministic no-CD bypass forces
# the pass branch at the chain ENTRY - one byte in, so the function's own
# push edx / pop edx:ret pair stays balanced (patching the push itself
# gives "DOS/4GW error (2001): exception 06h"). Apply to a COPY first:
#   tools/le_patch.py LEO.EXE OUT.EXE 0x5bd51:b801000000eb32
# (mov eax,1; jmp 0x5bd8a - the pass-counter add; full reasoning in
# PLAN.md S6 and tools/golden_build_rig.sh step 2.)
#
# Usage: build_data_disk.sh SRC_DIR OUT.qcow2 [ROOT_DIR]
#   SRC_DIR  = flat game files (go under \LEO\)
#   ROOT_DIR = optional; its files go to the partition ROOT (retail C:\
#              layout: LEO.EXE, DOS4GW.EXE, LONGSOFT.CFG next to \LEO\)
# Requires: qemu-img, sfdisk, mtools (mformat/mmd/mcopy/mdir).
set -euo pipefail
src=${1:?usage: $0 SRC_DIR OUT.qcow2 [ROOT_DIR]}
out=${2:?usage: $0 SRC_DIR OUT.qcow2 [ROOT_DIR]}
rootdir=${3:-}
work=$(dirname "$out")

qemu-img create -f qcow2 "$out" 1G
raw="$out.raw.tmp"
qemu-img convert -f qcow2 -O raw "$out" "$raw"
printf 'label: dos\nstart=63, size=2097089, type=6, bootable\n' | sfdisk --force "$raw"

# carve partition (offset 63*512) with mtools' raw access via dd-equivalent
part="$out.part.tmp"
python3 - "$raw" "$part" <<'EOF'
import sys
src, dst = sys.argv[1], sys.argv[2]
with open(src, 'rb') as f, open(dst, 'wb') as o:
    f.seek(63 * 512)
    while True:
        b = f.read(1 << 20)
        if not b:
            break
        o.write(b)
EOF

# The FAT16 must carry a REAL BIOS disk geometry in its BPB (63 sec/trk,
# 16 heads, 63 hidden) or MS-DOS 6.22 answers "Not reading drive C".
# (mformat's own -F geometry for raw images is mtools-internal 64/4096:
# fine for mtools, unreadable by DOS.) mkfs.fat writes a clean FAT16;
# the three geometry words are then pinned explicitly.
mkfs.fat -F 16 -f 2 -r 512 "$part" >/dev/null
python3 - "$part" <<'EOF'
import sys
p = sys.argv[1]
with open(p, 'r+b') as f:
    b = bytearray(f.read(512))
    assert b[510:512] == b'\x55\xAA', 'not a boot sector'
    b[11:13] = (512).to_bytes(2, 'little')      # bytes/sector
    b[24:26] = (63).to_bytes(2, 'little')       # sectors/track
    b[26:28] = (16).to_bytes(2, 'little')       # heads
    b[28:32] = (63).to_bytes(4, 'little')       # hidden sectors (= MBR->part)
    f.seek(0); f.write(b)
EOF

mmd    -i "$part" ::LEO
mcopy  -s -n -m -D s -i "$part" "$src"/* ::LEO/   # flat files -> \LEO\
if [ -n "$rootdir" ]; then
    mcopy -n -m -D s -i "$part" "$rootdir"/* ::/  # C:\ files (EXE/CFG)
fi
mdir   -i "$part" ::LEO | tail -3

# reassemble + recompress into the final qcow2
python3 - "$raw" "$part" "$out.raw2.tmp" <<'EOF'
import sys
head = open(sys.argv[1], 'rb').read(63 * 512)
with open(sys.argv[3], 'wb') as o:
    o.write(head)
    with open(sys.argv[2], 'rb') as p:
        while True:
            b = p.read(1 << 20)
            if not b:
                break
            o.write(b)
EOF
qemu-img convert -f raw -O qcow2 "$out.raw2.tmp" "$out"
rm -f "$raw" "$part" "$out.raw2.tmp"
qemu-img info "$out" | grep -E 'virtual|disk size'
echo "OK: $out  (attach as 2nd IDE drive; CFG byte0 -> its drive letter)"
