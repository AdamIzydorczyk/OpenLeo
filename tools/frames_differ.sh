#!/bin/sh
# frames_differ.sh A B - exit 0 only when BOTH frame dumps exist, are
# non-empty, and differ. A missing frame (the producer failed or never
# ran) must FAIL an "animation changes the frame" leg, not satisfy it
# (cmp -s alone treats a missing file as "differs").
[ -s "$1" ] && [ -s "$2" ] && ! cmp -s "$1" "$2"
