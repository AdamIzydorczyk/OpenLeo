# original — faithful DOS build (CD checks intact)

Recompile of the 1997 Longsoft engine as C/C++ source, built with Open Watcom
(wpp386 -> DOS4GW `.EXE`). Behavior matches retail, **including** the CD gate
chain and the "Blad Programu" tamper/CD-presence message, so it can be
A/B-tested against the pristine `LEO.EXE` in the QEMU baseline.

Contents (will be added as the recompile lands):
- `Makefile.wc`  watcom make build of shared `../src` with `WANT_CD_GATE=1`
- `platform_dos.c`  DOS/DPMI entry, INT21 file I/O, VESA mode-X / 320x200,
  SB DMA audio glue
- run notes: install layout mirroring retail CD (`\LEO\`), `LONGSOFT.CFG`

Ground truth for this variant: `../build/decompiled/all/*.c` (frozen Ghidra
reference) + live traces from the QEMU rig.
