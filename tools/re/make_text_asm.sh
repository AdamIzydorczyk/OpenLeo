#!/bin/sh
# Disassemble the .text of the game's LE image so a function can be read by
# address. build/leo/leo.elf comes from tools/le_dump.py (all LE fixups
# applied); virtual address = file offset + 0xf000. Output: one line per
# instruction "<va>:\t<bytes>\t<mnemonic operands>" in $OUT (default
# /tmp/opencode/vis/text.asm). Needs binutils (objdump).
#
#   tools/re/make_text_asm.sh [build/leo/leo.elf] [out.asm]
#   awk -F'\t' '{x=$1; gsub(/[ :]/,"",x)} x>="42de0" && x<="43240" {print x" "$3}' out.asm
set -e
ELF=${1:-build/leo/leo.elf}
OUT=${2:-/tmp/opencode/vis/text.asm}
mkdir -p "$(dirname "$OUT")"
python3 - "$ELF" /tmp/opencode/text.bin <<'PY'
import sys
d = open(sys.argv[1], 'rb').read()
open(sys.argv[2], 'wb').write(d[0x10000 - 0xf000:])
PY
objdump -D -b binary -m i386 --adjust-vma=0x10000 -M intel /tmp/opencode/text.bin > "$OUT"
echo "wrote $OUT ($(wc -l < "$OUT") lines)"
