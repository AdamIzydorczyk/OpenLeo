#!/usr/bin/env bash
# golden_build_rig.sh - assemble the T-24 golden-master rig images from the
# user's originals (nothing here is in git; see PLAN S6 + NEXT_STEPS).
#
#   boot floppy : MS-DOS 6.22 startup disk (dos-622-disk1.img copy) with
#                 AUTOEXEC.BAT replaced by:  C: / LEO   (the game runs from
#                 its own directory, like RATUNKU/LEO.BAT does on retail)
#   game HDD    : tools/build_data_disk.sh layout - \LEO\ = the CD's LEO\
#                 flat files (the "CD" the CFG points at); partition ROOT =
#                 the LEO_HDD files (patched LEO.EXE, DOS4GW.EXE,
#                 LONGSOFT.CFG byte0='C' via cfg_ll.py, STATUS.LEO,
#                 ANIMACJE, ETAPY). No CD/ASPI needed: the CD-presence gate
#                 is bypassed by the LEO.EXE patch documented in step 2.
#
# Usage: golden_build_rig.sh                (defaults below)
#   env: ISO=LL.iso MSDOS622=/path/dos-622-disk1.img \
#        STAGE=/tmp/opencode GOLDEN=/tmp/opencode/golden
set -euo pipefail
here=$(cd "$(dirname "$0")" && pwd)
root=$(dirname "$here")
ISO=${ISO:-$root/LL.iso}
MSDOS622=${MSDOS622:-/tmp/opencode/msdos622_x/dos-622-disk1.img}
STAGE=${STAGE:-/tmp/opencode}
GOLDEN=${GOLDEN:-$STAGE/golden}

[ -f "$ISO" ] || { echo "need the game ISO at $ISO (user-supplied)" >&2; exit 1; }
[ -f "$MSDOS622" ] || { echo "need an MS-DOS 6.22 bootable disk image: MSDOS622=$MSDOS622" >&2; exit 1; }
mkdir -p "$GOLDEN/hd_root"

# 1. stage CD data + the retail HD files from the ISO (restaging recipe,
#    kept identical to the one documented in NEXT_STEPS.md).
if [ ! -d "$STAGE/iso_stage/LEO" ]; then
    mkdir -p "$STAGE/iso_stage"
    bsdtar -xf "$ISO" -C "$STAGE/iso_stage" LEO
fi
if [ ! -f "$STAGE/hd_files/LEO.EXE" ]; then
    bsdtar -xf "$ISO" -C "$STAGE" LEO_HDD
    [ -d "$STAGE/LEO_HDD" ] && mv "$STAGE/LEO_HDD" "$STAGE/hd_files"
fi

# 2. the game's own directory (its cwd): the LEO_HDD files. CONFIG.EXE-style
#    install layout (RATUNKU/INSTALL.BAT on the CD) keeps LEO.EXE, DOS4GW.EXE,
#    LONGSOFT.CFG, STATUS.LEO and the ANIMACJE/ETAPY resource containers in
#    ONE directory and the game opens those by bare name, relative to its cwd.
#    (The CD's \LEO\ copies of ANIMACJE/ETAPY are DIFFERENT files - the two
#    trees stay separate here too.)
#    CD-gate bypass on a COPY of LEO.EXE (see build_data_disk.sh header):
#    0x5bd51: b8 01 00 00 00 (mov eax,1) + eb 32 (jmp to the pass-counter
#    add at 0x5bd8a). The function's own `push edx` / `pop edx:ret` pair is
#    left intact - patching at 0x5bd50 instead drops the push and the game
#    dies with "DOS/4GW error (2001): exception 06h" on the bad return.
python3 "$here/le_patch.py" "$STAGE/hd_files/LEO.EXE" \
        "$GOLDEN/hd_root/LEO.EXE" 0x5bd51:b801000000eb32
python3 "$here/cfg_ll.py" "$STAGE/hd_files/LONGSOFT.CFG" \
        "$GOLDEN/hd_root/LONGSOFT.CFG" --drive C >/dev/null
cp -f "$STAGE/hd_files/DOS4GW.EXE" "$STAGE/hd_files/STATUS.LEO" \
      "$STAGE/hd_files/ANIMACJE" "$STAGE/hd_files/ETAPY" "$GOLDEN/hd_root/"

# 3. game HDD (C:\LEO.\ + root files)
if [ ! -f "$GOLDEN/leo_hd.qcow2" ]; then
    bash "$here/build_data_disk.sh" "$STAGE/iso_stage/LEO" \
         "$GOLDEN/leo_hd.qcow2" "$GOLDEN/hd_root" >/dev/null
fi

# 4. boot floppy (MS-DOS 6.22 + AUTOEXEC that starts the game from C:\
#    exactly like the retail machine: EXE/CFG at the root, data in \LEO\)
#    DIAG=1 additionally redirects the game's stdout to C:\GAMEOUT.TXT: DOS
#    error text ("Could not open resource file: ...", DOS/4GW crashes) then
#    survives the session - read it with mtype after the run.
cp -f "$MSDOS622" "$GOLDEN/boot_floppy.img"
mdel -i "$GOLDEN/boot_floppy.img" ::AUTOEXEC.BAT 2>/dev/null || true
# DOS 6.22 defaults FILES=8 when CONFIG.SYS exists; the game opens more
# handles and then reports "Could not open resource file: C:\leo\sound".
mcopy -i "$GOLDEN/boot_floppy.img" ::CONFIG.SYS "$GOLDEN/cfg_orig.sys" >/dev/null
{ cat "$GOLDEN/cfg_orig.sys"; printf 'FILES=40\r\nBUFFERS=40\r\n'; } \
    > "$GOLDEN/newcfg.sys"
mdel -i "$GOLDEN/boot_floppy.img" ::CONFIG.SYS
mcopy -m -D s -i "$GOLDEN/boot_floppy.img" "$GOLDEN/newcfg.sys" ::CONFIG.SYS
rm -f "$GOLDEN/cfg_orig.sys" "$GOLDEN/newcfg.sys"
if [ "${DIAG:-0}" = 1 ]; then
    printf '@ECHO OFF\r\nC:\r\nLEO > GAMEOUT.TXT\r\n' > "$GOLDEN/newauto.bat"
else
    printf '@ECHO OFF\r\nC:\r\nLEO\r\n' > "$GOLDEN/newauto.bat"
fi
mcopy -m -D s -i "$GOLDEN/boot_floppy.img" "$GOLDEN/newauto.bat" ::AUTOEXEC.BAT
rm -f "$GOLDEN/newauto.bat"

echo "OK: $GOLDEN/{boot_floppy.img,leo_hd.qcow2} ready"
echo "next: python3 $here/golden_qemu_run.py --workdir $GOLDEN"
