# nocd — DOS build with CD requirement removed in source

Same shared engine as `../original`, compiled with `WANT_CD_GATE=0`:
the CD-presence chain (`ll_cdcheck_chain` @0x5bd50 + its four subchecks
`0x5bbd0/0x5bc60/0x5bd10/0x5bca0`) is **not compiled in** — no binary NOP,
no self-checksum conflict, nothing to "detect".

Data paths: game data (`*.IS`, `ANIMACJE`, `ETAPY`, `SOUND`, `LEVELDES`)
read from a plain HDD directory (letter from `LONGSOFT.CFG` byte 0 or env
override) instead of `\LEO\` on CD. Verified install example: 2nd IDE HDD,
FAT16, `D:\LEO\` with the 22 flat files from the disc.

Goal: boots to title/menu + gameplay in QEMU/DOSBox with **no CD drive**.
