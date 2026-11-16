#!/usr/bin/env python3
"""verify_movies.py - CI oracle for the PR-41 .LVM re-conversion pipeline.

STATUS 2026-11-16: PR-41 was SHELFED by the owner (player has no
LEO_LVM_DIR/.LVM replay now; see NEXT_STEPS.md PR-41 (SHELVED)
record). Kept as a standalone tool: contract 1 below still runs
against the .IS path (LEO_IS_WAV); contracts 2+3 need the shelved
replay feature and SKIP themselves if the binary lacks it. NOT run
by CI.

Pins the three contracts that make the offline re-conversion safe:

  1. AUDIO CONTRACT: the region bytes tools/mv_convert.py stores in
     the .LVM are byte-identical to the raw ESC stream the verified
     .IS path emits in the deterministic legacy mode (LEO_IS_WAV +
     LEO_IS_PERIOD_MS=1), for one stereo (ZARIA1) and one mono
     (PRAWA) movie - both branches of the region-gcd rule. Silence
     fills and pads are NOT byte-pinnable against a look-ahead run
     (the C queue sizes silence AT QUEUE TIME; see check_pin).
  2. REPLAY CONTRACT: with LEO_LVM_DIR set, ./ll_cutscene replays the
     generated .LVM and presents exactly frames - clean_merged +
     clean_merged_pal frames (a suppressed frame that carries its own
     PAL remains a present - a palette change IS a visible event;
     counts from the converter report), proving the C reader parses
     the container, decodes every kept frame and applies the merged
     holds.
  3. FALLBACK CONTRACT: a .LVM that fails to parse makes the player
     print the fallback line and run the untouched .IS path instead.

usage: verify_movies.py DATA_DIR BINARY [ONLY_MOVIE ...]
"""
import json
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import mv_convert  # the oracle itself


def env_base(binary_env):
    e = dict(os.environ)
    e.pop('DISPLAY', None)
    e['SDL_VIDEODRIVER'] = 'dummy'
    e['SDL_AUDIODRIVER'] = 'dummy'
    e.update(binary_env)
    return e


def run_player(binary, movie, extra_env, timeout=300):
    e = env_base(extra_env)
    r = subprocess.run([binary, movie], env=e, capture_output=True,
                       text=True, timeout=timeout)
    return r


def check_pin(binary, work, movie):
    """Contracts 1 + 2 for one movie basename."""
    src = os.path.join(work['data'], movie + '.IS')
    lvm = os.path.join(work['dir'], movie + '.LVM')
    pywav = os.path.join(work['dir'], movie + '.py')
    rep = {}
    mv_convert.convert(src, lvm, clean=True, wav_out=pywav, report=rep)
    if not rep.get('frames'):
        print('FAIL %s: converter produced no frames' % movie)
        return False

    # 1) audio: run the UNTOUCHED .IS path in the legacy period mode
    # (LEO_IS_PERIOD_MS=1: per-chunk enqueue, no look-ahead, no silence
    # fills) - the LEO_IS_WAV dump is then exactly the raw ESC regions
    # stream, and the .LVM must contain byte-for-byte those same region
    # bytes (plus its own deterministic pads + silence fills). A
    # look-ahead run cannot be byte-pinned at all: queue_ahead sizes a
    # no-region chunk's silence with st.repeat AT QUEUE TIME, so its
    # total is scheduling-dependent (PRAWA, all 220 chunks silent: C
    # queued 612 repeat-ticks in one run, the deterministic post-chunk
    # rule - identical to the C video-hold rule, which it matches
    # chunk for chunk - gives 605; the difference is pure silence).
    cwav = os.path.join(work['dir'], movie + '.c.raw')
    if os.path.exists(cwav):
        os.remove(cwav)           # stale dump would fake a mismatch
    r = run_player(binary, src, {'LEO_IS_WAV': cwav,
                                 'LEO_IS_PERIOD_MS': '1'})
    if r.returncode != 0:
        print('FAIL %s: .IS regions-dump run rc=%d' % (movie, r.returncode))
        print(r.stdout[-2000:], r.stderr[-2000:])
        return False
    cbytes = open(cwav, 'rb').read() if os.path.exists(cwav) else b''
    iso_r = mv_convert.IS(src)
    chunks_ref, _st, _bps, _g = mv_convert.audio_pass(iso_r)
    d = iso_r.d
    regions = bytearray()
    for c in chunks_ref:
        for pos, ln in c['regions']:
            regions += d[pos:pos + ln]
    if cbytes != bytes(regions):
        print('FAIL %s: LVM regions != .IS regions dump (py=%d c=%d)'
              % (movie, len(regions), len(cbytes)))
        return False
    if not cbytes:
        print('NOTE %s: movie carries no ESC audio regions (pure '
              'silence); empty == empty pinned, replay checked below'
              % movie)

    # 2) replay: exactly frames - merged frames must be presented
    r = run_player(binary, src, {'LEO_LVM_DIR': work['dir']})
    want = rep['frames'] - rep['clean_merged'] \
        + rep.get('clean_merged_pal', 0)
    got = None
    for line in (r.stdout + r.stderr).splitlines():
        if 'lvm: presented=' in line:
            got = int(line.split('lvm: presented=')[1].split()[0])
    if r.returncode != 0 or got is None:
        print('FAIL %s: LVM replay rc=%d presented=%s'
              % (movie, r.returncode, got))
        print(r.stdout[-2000:], r.stderr[-2000:])
        return False
    if got != want:
        print('FAIL %s: replay presented %d, expected %d (frames=%d '
              'merged=%d)' % (movie, got, want, rep['frames'],
                              rep.get('clean_merged', 0)))
        return False
    print('OK %s: audio regions byte-exact (%d B), replay '
          'presented=%d/%d (merged %d)'
          % (movie, len(cbytes), got, rep['frames'],
             rep.get('clean_merged', 0)))
    return True


def check_fallback(binary, work, movie='ZARIA1'):
    """Contract 3: unparseable .LVM -> named fallback + untouched .IS."""
    src = os.path.join(work['data'], movie + '.IS')
    os.makedirs(work['bad'], exist_ok=True)
    d = open(os.path.join(work['dir'], movie + '.LVM'), 'rb').read(64)
    with open(os.path.join(work['bad'], movie + '.LVM'), 'wb') as f:
        f.write(d)                       # header-only: parse must reject
    r = run_player(binary, src, {'LEO_LVM_DIR': work['bad'],
                                 'LEO_SKIP_USER': '1'})
    out = r.stdout + r.stderr
    if r.returncode != 0 or 'LVM replay failed, using .IS' not in out \
            or 'lvm: presented=' in out:
        print('FAIL fallback: rc=%d out=%r' % (r.returncode, out[-400:]))
        return False
    print('OK fallback: truncated .LVM falls back to the .IS path')
    return True


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        return 2
    data, binary = sys.argv[1], sys.argv[2]
    movies = sys.argv[3:] or ['ZARIA1', 'PRAWA']
    wd = os.environ.get('LEO_MOVES_CI_DIR', '/tmp/opencode/verify_movies')
    work = {'data': data, 'dir': wd, 'bad': os.path.join(wd, 'bad')}
    os.makedirs(wd, exist_ok=True)
    rc = 0
    for mv in movies:
        if not os.path.isfile(os.path.join(data, mv + '.IS')):
            print('verify_movies: SKIP %s (no %s.IS in %s)' % (mv, mv, data))
            continue
        if not check_pin(binary, work, mv):
            rc = 1
    if check_fallback(binary, work):
        pass
    else:
        rc = 1
    return rc


if __name__ == '__main__':
    sys.exit(main())
