#!/usr/bin/env bash
# build_data_disk.sh - create the FAT16 "data HDD" image for the no-CD install.
#
# Layout produced (verified working with QEMU rig):
#   - 1 GiB qcow2, DOS MBR (sfdisk: start=63, type=6, bootable)
#   - partition = FAT16 (mtools mformat -F, 32KB clusters)
#   - \LEO\ directory holding the 22 flat game files copied off the CD
#     (ANIMACJE, ETAPY, SOUND, LEVELDES + 15 *.IS) - llrs archives are
#     single files, no subdirs needed.
# Game side: LONGSOFT.CFG byte0 = drive letter of this disk (tools/cfg_ll.py).
#
# Usage: build_data_disk.sh SRC_DIR OUT.qcow2        (SRC_DIR = flat game files)
# Requires: qemu-img, sfdisk, mtools (mformat/mmd/mcopy/mdir).
set -euo pipefail
src=${1:?usage: $0 SRC_DIR OUT.qcow2}
out=${2:?usage: $0 SRC_DIR OUT.qcow2}
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

mformat -i "$part" -F -s 64 -h 16 -t 1024 ::   # FAT16, 32KB clusters
mmd    -i "$part" ::LEO
mcopy  -s -n -m -D s -i "$part" -i "$part" "$src"/ ::/   # flat files -> \LEO\
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
