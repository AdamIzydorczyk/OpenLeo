#!/usr/bin/env python3
"""package.py - release packaging (tips T-21 windows zip, T-22 linux tar.gz).

Assembles dist/ trees + archives from an already-BUILT repo:

    make package-win      -> dist/LewLeon-windows.zip
    make package-linux    -> dist/LewLeon-linux.tar.gz

Layout inside the archive (T-21):
    ll_cutscene.exe|ll_cutscene   the game binary (built static on
                                 windows by `make win`; the linux build
                                 needs the system SDL2 runtime)
    README.txt                    where to put the original data, how
                                 to run, controls
    data/README.txt               placeholder directory: dropping the
                                 original game directories (ANIMACJE,
                                 ETAPY, SOUND, LEVELDES) next to the
                                 exe just works (pick_data_dir chain)

Original game data is never packaged: it is copyright Adventure
International / Longsoft and users supply their own copy.
"""
import argparse
import os
import shutil
import sys
import time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DIST = os.path.join(ROOT, "dist")

README_SHARED = """\
Lew Leon - OpenLeo native port {version} (built {date})
========================================================

This is a clean-room native recompile of the engine parts of Lew Leon
(1998, Adventure International / Longsoft). The GAME ITSELF needs the
original data - it is NOT included (copyright).


WHERE TO PUT THE GAME DATA
--------------------------
Copy these directories from the original CD/extract into the "data"
folder next to the program:

    ANIMACJE/   ETAPY/   SOUND/   LEVELDES/

(or point the LEO_DATA environment variable at an existing tree, or
pass the directory on the command line). With no arguments the program
starts the original boot demo chain if it can find that data, and
reports exactly what is missing if it cannot.


HOW TO RUN
----------
    {runline}

    no arguments        boot demo -> title menu (the original behaviour)
    FILE.IS [...]       play specific scenes; --then FILE.IS chains more
    --demo [DIR]        force the boot chain from data directory DIR
    --menu [DIR]        open the title menu directly

Controls: ESC skip (cutscenes; the game continues), SPACE pause,
Q quit a scene, arrows/Enter menu navigation, F11 or Alt+Enter
toggles fullscreen. Options: --scale N (window enlargement),
--fullscreen, --no-audio, --ppm N OUT.ppm (frame dump), {console_flag}


NOTES
-----
- The windows binary is a single self-contained .exe (statically
  linked SDL2); a binary built with the repo's CMake instead links
  SDL2.dll and needs it next to the exe.
- Windows SmartScreen may warn "unknown publisher": the builds here
  are unsigned.
- Game data remains copyright Adventure International / Longsoft
  (1998); engine code here is the OpenLeo project source.
"""


def readme(platform: str) -> str:
    if platform == "win":
        runline = "ll_cutscene.exe"
        console = "--console (open a console window for diagnostics)"
    else:
        runline = "./ll_cutscene        (needs libSDL2: apt install libsdl2-2.0-0)"
        console = ""
    return README_SHARED.format(version="0.1.0", date=time.strftime("%Y-%m-%d"),
                               runline=runline, console_flag=console)


DATA_PLACEHOLDER = """\
Put the original game directories here (ANIMACJE/, ETAPY/, SOUND/,
LEVELDES/). They are not part of this archive: Lew Leon (1998,
Adventure International / Longsoft) data must come from your own copy.
"""


def stage(name: str, binary: str, platform: str) -> str:
    out = os.path.join(DIST, name)
    if os.path.isdir(out):
        shutil.rmtree(out)
    os.makedirs(out)
    shutil.copy2(os.path.join(ROOT, binary), os.path.join(out, binary))
    with open(os.path.join(out, "README.txt"), "w") as f:
        f.write(readme(platform))
    os.mkdir(os.path.join(out, "data"))
    with open(os.path.join(out, "data", "README.txt"), "w") as f:
        f.write(DATA_PLACEHOLDER)
    return out


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--win", action="store_true", help="zip the windows build")
    ap.add_argument("--linux", action="store_true", help="tar.gz the linux build")
    args = ap.parse_args()
    if not (args.win or args.linux):
        ap.print_usage(sys.stderr)
        return 2

    os.makedirs(DIST, exist_ok=True)
    made = []
    if args.win:
        src = os.path.join(ROOT, "ll_cutscene.exe")
        if not os.path.exists(src):
            print("build first: make ll_cutscene.exe", file=sys.stderr)
            return 1
        stage_dir = stage("LewLeon-windows", "ll_cutscene.exe", "win")
        made.append(shutil.make_archive(os.path.join(DIST, "LewLeon-windows"),
                                        "zip", stage_dir))
    if args.linux:
        src = os.path.join(ROOT, "ll_cutscene")
        if not os.path.exists(src):
            print("build first: make ll_cutscene", file=sys.stderr)
            return 1
        stage_dir = stage("LewLeon-linux", "ll_cutscene", "linux")
        made.append(shutil.make_archive(os.path.join(DIST, "LewLeon-linux"),
                                        "gztar", stage_dir))
    for m in made:
        print(f"packaged {m}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
