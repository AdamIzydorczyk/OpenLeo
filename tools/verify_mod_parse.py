#!/usr/bin/env python3
"""verify_mod_parse.py - independent oracle for the PARSE layer of
ported/ll_mod.c over every .MOD record of the SOUND container.

Written from the public ProTracker module layout and the engine facts in
ll_mod.h (title@0, 31 x 30-byte sample records@20, song length@950, 128
order bytes@952, signature@1080, pattern slots = max(order)+1 stored back to
back at 1084, sample data contiguous after them) - NOT from ll_mod.c - and
compared line for line with `mod_selftest`'s parse summary:

  parse <name>: '<title>' id=<sig> <n>ch nseq=<N> npat=<P> nsmp=<S>
    smpNN len=<bytes> dlen=<present bytes> loop=<start>/<len> vol=<v> fine=<f>

What this covers: the header/sample-table/order-table arithmetic of the
loader for all 22 shipped tracks (the loader-side half of "M-01..M-12").
What it does NOT cover: the mixer (verify-mod compares the render hash of 3
tracks between linux and wine, which is determinism, not correctness).

usage: verify_mod_parse.py SOUND_CONTAINER MOD_SELFTEST_CMD
"""
import os
import struct
import subprocess
import sys

SLOT = struct.Struct('<14sIII')
CHANNELS = {b'M.K.': 4, b'M!K!': 4, b'FLT4': 4, b'4CHN': 4, b'6CHN': 6,
            b'8CHN': 8, b'CD81': 8, b'OKTA': 8, b'16CN': 16, b'32CN': 32}


def mod_records(path):
    d = open(path, 'rb').read()
    cap, = struct.unpack_from('<I', d, 8)
    for k in range(cap):
        key, rid, off, size = SLOT.unpack_from(d, 0x14 + 26 * k)
        name = key.split(b'\0')[0].decode('latin1')
        if rid == 1 and name.upper().endswith('.MOD'):
            yield name, d[off:off + size]


def expected(name, img):
    """the parse summary the spec dictates, as mod_selftest prints it"""
    title = img[0:20].split(b'\0')[0].decode('latin1')
    sig = img[1080:1084]
    nch = CHANNELS.get(sig, 4)
    songlen = img[950]
    order = img[952:952 + 128]
    npat = max(order) + 1
    sd = 1084 + npat * nch * 256            # first sample byte
    lines = ["parse %s: '%s' id=%s %dch nseq=%d npat=%d nsmp=31"
             % (name, title, sig.decode('latin1'), nch, songlen, npat)]
    pos = sd
    for i in range(31):
        r = img[20 + 30 * i:50 + 30 * i]
        ln, = struct.unpack_from('>H', r, 22)
        ln *= 2
        fine = r[24] & 0x0f
        vol = r[25]
        ls, = struct.unpack_from('>H', r, 26)
        ll, = struct.unpack_from('>H', r, 28)
        present = max(0, min(ln, len(img) - pos))
        pos += ln
        if ln:
            lines.append("  smp%02d len=%d dlen=%d loop=%d/%d vol=%d fine=%d"
                         % (i + 1, ln, present, ls * 2, ll * 2, vol, fine))
    return lines


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        return 2
    sound, tool = sys.argv[1], sys.argv[2].split()
    tmp = '/tmp/opencode/modver'
    os.makedirs(tmp, exist_ok=True)
    n = bad = 0
    for name, img in mod_records(sound):
        f = os.path.join(tmp, name.lower())
        open(f, 'wb').write(img)
        out = subprocess.run(tool + [f, '1'], capture_output=True)
        got = out.stdout.decode('latin1').replace('\r', '').split('\n')
        want = expected(os.path.basename(f), img)
        got = [ln for ln in got if ln.startswith('parse ') or ln.startswith('  smp')]
        n += 1
        if got != want:
            bad += 1
            print('MISMATCH %s' % name)
            for i, (a, b) in enumerate(zip(want, got)):
                if a != b:
                    print('  want %s\n  got  %s' % (a, b))
                    break
            else:
                print('  line count: want %d got %d' % (len(want), len(got)))
    print('MOD parse check: %d/%d tracks match the spec-derived summary'
          % (n - bad, n))
    return 1 if bad or n < 22 else 0


if __name__ == '__main__':
    sys.exit(main())
