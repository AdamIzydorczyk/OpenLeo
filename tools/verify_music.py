#!/usr/bin/env python3
"""verify_music.py - CI gate for the music pump path (review E-09).

Reads the LEO_MUSIC_WAV dumps written by a real level/menu run and
proves the queue pump produced real, complete audio:

    usage: verify_music.py /tmp/opencode/mus_ci
      (the run wrote /tmp/opencode/mus_ci.<NNN>.<NAME>.raw)

Checks: exactly one dump; whole s16 stereo frames at 44100 Hz;
>= 1.5 s of content (2 s scene - queue slack, still far above the
pre-E-09 ~68 ms low-water); not silent; never clamped (the PR-6
mixer leaves headroom).
"""
import array
import glob
import sys


def main(prefix: str) -> int:
    files = sorted(glob.glob(prefix + ".*.raw"))
    if len(files) != 1:
        print(f"FAIL: expected exactly 1 music dump for {prefix}, got {files}")
        return 1
    data = open(files[0], "rb").read()
    if len(data) % 4:
        print(f"FAIL: {files[0]}: {len(data)} B is not whole s16 stereo frames")
        return 1
    a = array.array("h")
    a.frombytes(data)
    frames = len(a) // 2
    need = int(1.5 * 44100)
    if frames < need:
        print(f"FAIL: only {frames} frames dumped, need >= {need} (1.5 s)")
        return 1
    if not any(a):
        print(f"FAIL: {files[0]} is silent")
        return 1
    peak = max(abs(x) for x in a)
    if peak >= 32768:
        print(f"FAIL: sample hit the int16 clamp (peak {peak})")
        return 1
    name = files[0].split(".", 2)[2].removesuffix(".raw")
    print(f"music pump: {name} {frames / 44100:.2f}s s16 stereo @44.1k, "
          f"silent=0 clip=0 peak={peak} (ok)")
    return 0


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(__doc__)
        sys.exit(2)
    sys.exit(main(sys.argv[1]))
