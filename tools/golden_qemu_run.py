#!/usr/bin/env python3
"""golden_qemu_run.py - drive the QEMU baseline rig and capture a golden
frame of the ORIGINAL game at a known state (T-24 golden-master).

Rig (same shape as PLAN.md S6, verified 2026-11-13): MS-DOS 6.22 boot
floppy (-fda), game HDD (-hda, C:\\LEO\\ with build_data_disk.sh layout),
SB16 at 0x220/dma1 (defaults; matches the original rig's audio clock),
-vga std, headless, monitor on a unix socket. The game auto-starts from
AUTOEXEC.BAT on the floppy; the CD-check gate is bypassed by the known
0x5bd51 patch on a COPY (le_patch.py, see tools/build_data_disk.sh
header), so no CD/ASPI is needed.

Two INDEPENDENT menu detectors (2026-11-15, after 5 scripted runs showed
the tick counter is NOT reliable evidence here - it kept running even in
menu-like states, and 40:6C-freeze never appeared; see PLAN S6 notes):
 1. FRAME DOMAIN (primary, --frame-interval, default 0.5 s): every frame
    is screendumped and classified by CRTC geometry - the .IS movie chain
    ALWAYS runs 640x480, DOS text is 720 wide, fades/menu-transition are
    black; the chapter map menu is the ONLY state on a 640x400 canvas,
    and it cycles one static ART frame with black (PLAN hunt12). A
    640x400 episode >= 2 s that showed art frames is the capture
    trigger. Every frame CHANGE is journalled (frame_journal.jsonl:
    time, geometry, color counts, hash) and every non-movie frame is
    kept (frame_T.ppm) - the full state trail of the run.
 2. CLOCK DOMAIN (kept as secondary): the 0x46C tick counter, ~18.2 Hz
    under DOS / ~70 Hz while an .IS movie plays (PIT reprogrammed,
    ll_config.h). A >= FREEZE_S freeze with the movie clock seen also
    triggers. Proven NOT to be a sufficient detector on this rig; a
    monitor-desync bug (HMP echo) broke it entirely in the L1-L4 runs,
    fixed by speaking QMP to the monitor instead of HMP.

Artifacts written to --workdir (gitignored, /tmp):
  vram.bin      64 KiB VRAM window (QMP xp /65536b 0xfd000000 x4;
                first 64000 B = the linear 8bpp 320x200 index plane)
  menu.ppm / menu_black.ppm  screendump at the capture instant
  static2.ppm   second screendump (proves the frame is static)
  frame_journal.jsonl, frame_*.ppm, probe_*.ppm  observation trail
  capture_manifest.json  hashes + tick log + phases + exact QEMU cmdline

Usage: golden_qemu_run.py [--workdir DIR] [--timeout SEC]
                          [--floppy IMG] [--hd QCOW2] [--keep-running]
"""
import argparse
import bisect
import hashlib
import json
import os
import re
import shutil
import socket
import subprocess
import sys
import time

WORK = "/tmp/opencode/golden"
FREEZE_S = 3.0        # consecutive equal 0x46C samples -> menu state
SAMPLE_S = 0.25       # tick polling period; the menu freeze window is only
                      # ~7-8 s, so minute-scale sampling can miss it entirely
PROVE_MOVIE = 40      # max rate needed before a freeze counts (audio clock seen)
MAX_RATE = 110


def sha256(path):
    h = hashlib.sha256()
    with open(path, 'rb') as f:
        for b in iter(lambda: f.read(1 << 20), b''):
            h.update(b)
    return h.hexdigest()


class HMP:
    """Monitor client over the QEMU QMP unix socket. QMP (JSON lines) is
    used instead of human-readable HMP on purpose: QEMU 11's HMP readline
    ECHOES every typed command with ANSI edit sequences, and that echo
    framing broke reply parsing in the L1-L4 runs (all tick reads failed,
    found 2026-11-15). .cmd(c) keeps the old call shape by wrapping c
    in human-monitor-command, so x/sendkey/screendump/stop/cont all work
    exactly as before - just with reliable framing."""

    def __init__(self, path):
        self.s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        for _ in range(200):
            try:
                self.s.connect(path)
                break
            except OSError:
                time.sleep(0.1)
        else:
            raise SystemExit('cannot connect to monitor ' + path)
        self.f = self.s.makefile('rw', newline='\n')
        greeting = self._read()
        if 'QMP' not in greeting:
            raise SystemExit('not a QMP socket: ' + str(greeting)[:120])
        self._send({'execute': 'qmp_capabilities'})
        r = self._read()
        if 'error' in r:
            raise SystemExit('qmp_capabilities: ' + str(r)[:200])

    def _send(self, obj):
        self.f.write(json.dumps(obj) + '\n')
        self.f.flush()

    def _read(self):
        while True:
            line = self.f.readline()
            if line == '':
                raise SystemExit('monitor EOF')
            try:
                obj = json.loads(line)
            except ValueError:
                continue                 # tolerate any stray framing
            if 'event' in obj:           # async events: skip, not replies
                continue
            return obj

    def cmd(self, c):
        r = None
        for _ in range(4):              # self-healing against desync
            self._send({'execute': 'human-monitor-command',
                        'arguments': {'command-line': c}})
            r = self._read()
            if 'return' in r:
                return r['return'] or ''
        raise SystemExit('qmp hmc failed: %s -> %s' % (c, str(r)[:200]))


def read_tick(h):
    r = h.cmd('x /wx 0x46c')
    m = re.search(r':\s+(0x[0-9a-f]+)', r)
    if not m and r:
        read_tick.last_raw = r
    return int(m.group(1), 16) if m else None


def _xp_bytes(h, base, total=65536, chunk=16384):
    blob = bytearray()
    for i in range(total // chunk):
        r = h.cmd('xp /%db 0x%x' % (chunk, base + i * chunk))
        n = 0
        for line in r.splitlines():
            _, _, rest = line.partition(':')
            for tok in rest.split():
                tok = tok[2:] if tok.startswith('0x') else tok
                if re.fullmatch(r'[0-9a-fA-F]{2}', tok):
                    blob.append(int(tok, 16))
                    n += 1
        if n != chunk:
            raise SystemExit('xp chunk %d parsed %d bytes' % (i, n))
    return bytes(blob)


def read_vram(h):
    """64 KiB index-plane window, auto-located. The ORIGINAL puts its
    visible page at a NON-ZERO display start: measured 2026-11-15
    (vram_probe.py, frozen menu frame) the menu's 320x200 8bpp index
    page lives at VRAM offset 0x20000 (read via LFB BAR 0xfd020000:
    161 distinct indices, index->color bijection vs the undoubled
    screendump = ZERO violations). Scan all four 64 KiB LFB pages and
    the A0000 aperture; the first page with real 8bpp content wins.
    Returns (bytes, base); manifest records it as vram_window."""
    last, last_nd = None, 0
    for base in (0xfd020000, 0xfd000000, 0xfd010000, 0xfd030000,
                 0xa0000):
        blob = _xp_bytes(h, base)
        nd = len(set(blob[:64000]))
        if nd > last_nd:
            last, last_nd = blob, nd
        if nd >= 40:
            return blob, base
    return last, 0


def screen_mean(path):
    """Mean RGB of a P6 PPM (QEMU screendump writes P6)."""
    with open(path, 'rb') as f:
        magic = f.readline().strip()
        if magic != b'P6':
            return -1.0
        w, h = map(int, f.readline().split())
        f.readline()
        px, tot, n = f.read(), 0, 0
        for i in range(0, min(len(px), w * h * 3), 997 * 3):   # sample ~1/1000
            tot += px[i] + px[i + 1] + px[i + 2]
            n += 3
    return tot / max(n, 1)


def ppm_size(path):
    with open(path, 'rb') as f:
        if f.readline().strip() != b'P6':
            return None
        return tuple(int(x) for x in f.readline().split())


def text_grid(h):
    """The VGA text buffer (0xB8000, 80x25) as lines - the fastest way to
    see where a DOS session actually is (boot loader, COMMAND.COM prompt,
    a DOS/4GW error). Characters are at the even byte of each cell."""
    chars = []
    for seg in range(0, 4000, 250):
        r = h.cmd('xp /%db 0x%x' % (250, 0xb8000 + seg))
        vals = [t for t in re.findall(r'0x([0-9a-fA-F]{2})', r)]
        chars += [int(v, 16) for i, v in enumerate(vals) if i % 2 == 0]
    lines = []
    for i in range(0, len(chars), 80):
        ln = ''.join(chr(c) if 32 <= c < 127 else ' '
                     for c in chars[i:i + 80]).rstrip()
        if ln:
            lines.append(ln)
    return '\n'.join(lines)


def plane_ok(blob):
    """True if the 320x200 index plane holds real art, not a black fade
    (the map menu's own index space used 209 distinct values in the
    original capture, PLAN S6; a black/blank plane has < 5)."""
    return len(set(blob[:64000])) >= 40


def page_reproduces_screen(blob, px):
    """True iff blob's 320x200 8bpp page, mapped through the screendump's
    own index->color table, reproduces that screendump EXACTLY (after
    undoing the 2x2 doubling). This is the only honest page selector: the
    game rewrites the displayed page per menu half-cycle (artpage_probe,
    2026-11-15: at a dark stop NO page anywhere matches the earlier art
    frame, and content heuristics picked a wrong page once in L11), so
    'does it explain the frozen screen' decides - not distinct-byte
    counts or brightness."""
    st = ppm_stats(px)
    if not st or st[0] != 640 or st[1] != 400:
        return False
    body = px.split(b'\n', 3)
    if len(body) < 4 or body[0].strip() != b'P6':
        return False
    raw = body[3]
    if len(raw) < 640 * 400 * 3:
        return False
    page = memoryview(blob)[:64000]
    colmap = {}
    for y in range(400):
        base = y * 1920
        py = (y // 2) * 320
        for x in range(640):
            i = page[py + x // 2]
            c = raw[base + 3 * x:base + 3 * x + 3]
            prev = colmap.get(i)
            if prev is None:
                colmap[i] = c
            elif prev != c:
                return False            # one index, two colors: wrong page
    for y in range(400):
        base = y * 1920
        py = (y // 2) * 320
        for x in range(640):
            if colmap[page[py + x // 2]] != raw[base + 3 * x:
                                                base + 3 * x + 3]:
                return False
    return True


def capture_menu_state(h, wd, want_art=False, max_tries=14):
    """Inside a menu-candidate window: stop the VM, screendump FIRST (the
    frozen screen is ground truth), then read the index pages and keep the
    page that reproduces that screen. The menu FLIPS COMPOSED PAGES
    (full-landscape art / near-black) every ~1.2 s, so one stop catches
    whichever half is live. want_art=True keeps trying across cycles
    until the ART half is caught with a verified page ('ok-art'); the
    near-black half ('ok-dark') is kept as the documented fallback.
    Frozen-clock mode (want_art=False) keeps the L10-proven static-pair
    path: two identical stopped reads, never an all-zero pair (L8)."""
    if want_art:
        dark_blob, dark_window, tries = None, None, 0
        while tries < max_tries:
            tries += 1
            h.cmd('sendkey esc')   # keepalive: repeated Esc keeps the
            h.cmd('stop')          # menu's 400-line window open (L7/L8)
            h.cmd('screendump ' + os.path.join(wd, 'stop_screen.ppm'))
            px = ppm_read(os.path.join(wd, 'stop_screen.ppm'))
            st = ppm_stats(px)
            blob, window, shot = None, 0, None
            # The golden TARGET is the exact landscape frame the sensor
            # proved byte-stable across cycles (sha1 b880e203: seen in
            # L7 t=30/t=266, A2/A3 cycles). Content thresholds alone
            # qualified a mid-FADE frame once (A3 try 11: colors=109
            # mean=17 = a blend, not the art), so the hash IS the gate.
            fsh = hashlib.sha1(px).hexdigest() if px else ''
            if st and st[0] == 640 and st[1] == 400:
                if fsh.startswith('b880e203'):
                    shot = 'menu_art.ppm'
                elif fsh.startswith('2c4c0583'):
                    shot = 'menu_art2.ppm'   # 2nd landscape variant the
                elif fsh.startswith('4b905f7e'):
                    shot = 'menu_black.ppm'  # menu cycles (A2 journal)
                print('[cap try %d] screen %dx%d colors=%d mean=%d '
                      'frame=%s%s' % (tries, st[0], st[1], st[2], st[3],
                                      fsh[:8], ' TARGET' if shot ==
                                      'menu_art.ppm' else ''), flush=True)
                if shot:
                    # latency matters: the 400-line window is ~10 s per
                    # Esc. The page lives at display start (0x20000 ==
                    # LFB BAR +0x20000, measured); only scan the rest
                    # if that fails.
                    for base in (0xfd020000, 0xfd000000, 0xfd010000,
                                 0xfd030000):
                        b = _xp_bytes(h, base)
                        if page_reproduces_screen(b, px):
                            blob, window = b, base
                            break
                    if blob is not None:
                        with open(os.path.join(wd, shot), 'wb') as f:
                            f.write(px)
                        if shot == 'menu_art.ppm':
                            read_vram.window = window
                            h.cmd('cont')
                            return blob, 'ok-art'
                        if dark_blob is None:
                            dark_blob, dark_window = blob, window
            h.cmd('cont')
            time.sleep(0.2)         # ~1/6 of the 1.2 s page-flip cycle
        if dark_blob is not None:
            read_vram.window = dark_window
            return dark_blob, 'ok-dark'
        return b'', 'moving'
    best_black = None
    for attempt in range(4):
        h.cmd('stop')
        blob, window = read_vram(h)
        read_vram.window = window
        art = plane_ok(blob)
        if art:
            h.cmd('screendump ' + os.path.join(wd, 'menu.ppm'))
        else:
            h.cmd('screendump ' + os.path.join(wd, 'menu_black.ppm'))
        h.cmd('cont')
        time.sleep(0.5)
        h.cmd('stop')
        blob2, window2 = read_vram(h)
        if window == 0 and window2 != 0:
            blob, blob2, window = blob2, blob2, window2
            art = plane_ok(blob)
        if blob == blob2 and (art or set(blob[:64000]) != {0}):
            # identical stopped pair with real content = a genuine static
            # frame; an ALL-ZERO pair is a wrong-window artifact, never
            # a capture (L8 lesson, 2026-11-15)
            if art:
                h.cmd('screendump ' + os.path.join(wd, 'static2.ppm'))
                return blob, 'ok'
            best_black = blob
        else:
            best_black = best_black or (blob2 if not plane_ok(blob2)
                                        and set(blob2[:64000]) != {0}
                                        else None)
        h.cmd('cont')
        time.sleep(1.0)
    if best_black is not None:
        return best_black, 'ok'
    return blob, 'moving'


def ppm_read(path):
    """Complete PPM bytes from a QEMU screendump; retries while the file
    is still being written (a torn read would corrupt BOTH the stats and
    the frame hash - and stats/hash MUST come from ONE byte string, the
    two-read version mislabelled frames, found 2026-11-15)."""
    prev = b''
    for _ in range(6):
        try:
            with open(path, 'rb') as f:
                px = f.read()
        except OSError:
            px = b''
        if px and px == prev:
            return px
        prev = px
        time.sleep(0.04)
    return prev


def ppm_stats(px):
    """(w, h, ncolors, mean) of PPM bytes; small samples only. CRTC size
    is the state fingerprint: 640x480 = .IS movie chain, 640x400 = the
    game's menu canvas (black 400 ALSO = entry/exit fade - a height hit
    needs art content or persistence to count, PLAN ~312); 720x400 =
    plain DOS text mode."""
    try:
        hdr = px.split(b'\n', 3)
        if hdr[0].strip() != b'P6':
            return None
        w, h = (int(x) for x in hdr[1].split())
        px = px[len(hdr[0]) + 1 + len(hdr[1]) + 1 + len(hdr[2]) + 1:]
    except (ValueError, IndexError):
        return None
    step = max(1, (len(px) // 3) // 3000) * 3
    cols, total, n = set(), 0, 0
    for i in range(0, len(px) - 2, step):
        cols.add((px[i] >> 3, px[i + 1] >> 3, px[i + 2] >> 3))
        total += px[i] + px[i + 1] + px[i + 2]
        n += 3
    return w, h, len(cols), total // max(n, 1)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--workdir', default=WORK)
    ap.add_argument('--timeout', type=int, default=420)
    ap.add_argument('--floppy')
    ap.add_argument('--hd')
    ap.add_argument('--cdrom', help='attach this ISO as IDE cd0 (CD route; '
                                   'the boot floppy must load the matching '
                                   'real-mode CD driver + MSCDEX)')
    ap.add_argument('--ddisk', help='attach this disk image as IDE hdb: the '
                                    'disk-D route - D: is a plain FAT disk '
                                    'with \\\\LEO\\\\ at its root (no CD, no '
                                    'mounting, no driver)')
    ap.add_argument('--sendkey-at', default='', help='scripted user input: '
                                   'comma list of T=KEY (monitor sendkey at '
                                   't=T s). The ORIGINAL reaches the chapter '
                                   'map menu only after input - in the S6 '
                                   'GUI baseline "Esc opens menu" during the '
                                   'intro FMV; no attract chain reaches it '
                                   'on its own (no-CD AND CD rigs, both '
                                   '1500 s, 2026-11-15).')
    ap.add_argument('--probe-shots', action='store_true',
                    help='after every scheduled event, screendump at '
                         '+0.6/+3/+9 s (probe_NNNa/b/c.ppm): what each key '
                         'landed on vs. its fade. KEY "noop" = observe '
                         'without input.')
    ap.add_argument('--frame-interval', type=float, default=0.5,
                    help='screendump+classify every N s (0=off). Second, '
                         'INDEPENDENT menu detector: the chapter map menu '
                         'is the only state on a 400-line CRTC with art '
                         'content (the .IS movie chain is always 480); '
                         'frame changes are journalled to frame_journal '
                         '.jsonl, art frames kept as frame_T.ppm.')
    ap.add_argument('--ram', type=int, default=32)
    ap.add_argument('--keep-running', action='store_true')
    ap.add_argument('--want-art', action='store_true',
                    help='catch the ART half of the menu cycle: retry the '
                    'stop across art frames until the frozen screen IS the '
                    'art frame and a VRAM page reproduces it exactly '
                    '(page_reproduces_screen); the verified near-black '
                    'page is kept as fallback')
    a = ap.parse_args()

    wd = a.workdir
    os.makedirs(wd, exist_ok=True)
    floppy = a.floppy or os.path.join(wd, 'boot_floppy.img')
    hd = a.hd or os.path.join(wd, 'leo_hd.qcow2')
    for p in (floppy, hd) + tuple(x for x in (a.cdrom, a.ddisk) if x):
        if not os.path.exists(p):
            raise SystemExit('missing rig image: ' + p)
    mon = os.path.join(wd, 'mon.sock')
    if os.path.exists(mon):
        os.unlink(mon)

    qemu = ['qemu-system-i386', '-m', str(a.ram), '-boot', 'a',
            '-fda', floppy, '-hda', hd,
            '-machine', 'pc', '-vga', 'std', '-display', 'none',
            '-audiodev', 'none,id=a0', '-device', 'sb16,audiodev=a0',
            '-qmp', 'unix:' + mon + ',server,nowait']
    if a.cdrom:
        qemu[5:5] = ['-cdrom', a.cdrom]
    if a.ddisk:
        i = qemu.index('-hda')
        qemu[i + 2:i + 2] = ['-hdb', a.ddisk]
    ver = subprocess.run(['qemu-system-i386', '--version'],
                         capture_output=True, text=True).stdout.splitlines()[0]
    log = open(os.path.join(wd, 'qemu.log'), 'wb')
    # stdin=/dev/null is REQUIRED: with -display none the default monitor
    # muxes onto stdio, and when this process's stdin is a PTY (agent
    # harness) the mux turns readline ECHO on - the echo of every command
    # then desyncs the HMP reply framing (found 2026-11-15: all tick
    # reads failed in the L1/L2 runs; earlier runs from plain shells
    # worked). /dev/null stdin -> not a tty -> no echo.
    proc = subprocess.Popen(qemu, stdin=subprocess.DEVNULL,
                            stdout=log, stderr=subprocess.STDOUT)
    manifest = {
        'qemu_version': ver,
        'qemu_argv': qemu,
        'inputs': {'floppy': {'path': floppy, 'sha256': sha256(floppy)},
                   'hd': {'path': hd, 'sha256': sha256(hd)}},
        'tick_log': [], 'phases': [], 'artifacts': {},
    }
    if a.cdrom:
        manifest['inputs']['cdrom'] = {'path': a.cdrom, 'sha256': sha256(a.cdrom)}
    if a.ddisk:
        manifest['inputs']['ddisk'] = {'path': a.ddisk, 'sha256': sha256(a.ddisk)}
    print('qemu pid', proc.pid)

    sched = []
    for item in a.sendkey_at.split(','):
        item = item.strip()
        if item:
            tstr, _, key = item.partition('=')
            sched.append((float(tstr), key))
    sched.sort()
    si = 0
    shotq = []                              # (due_t, si, tag, phase_entry)

    try:
        h = HMP(mon)
        t0 = time.time()
        prev, prev_t, stable = None, None, 0
        max_rate, movie_seen, menu_t = 0.0, False, None
        want_art = a.want_art
        fnext = a.frame_interval if a.frame_interval > 0 else 9e9
        fhash = None
        m_w = m_h = 0
        mode_t, art_seen = 0.0, False      # current CRTC episode
        dark_blob, dark_shot = None, 'menu_black.ppm'
        journal = open(os.path.join(wd, 'frame_journal.jsonl'), 'w')
        while True:
            t = time.time() - t0
            if proc.poll() is not None:
                raise SystemExit('qemu exited early (rc %s), see qemu.log'
                                 % proc.returncode)
            while si < len(sched) and t >= sched[si][0]:
                key = sched[si][1]
                print('[t=%.0fs] sendkey %s' % (t, key), flush=True)
                manifest['phases'].append(
                    {'t': round(t, 1), 'event': 'sendkey ' + key})
                if key != 'noop':
                    h.cmd('sendkey ' + key)
                if a.probe_shots:
                    # ONE shot is not evidence: the game fades black for
                    # ~1-2 s after any input (measured 2026-11-15: every
                    # +0.6 s shot is black = a mode-switch/fade, and a
                    # black frame is NOT menu proof - menu entry/exit
                    # fades are black too, PLAN ~line 312). Shoot at
                    # increasing delays so one frame shows the landing
                    # state. The shots are DELAYED EVENTS IN THIS LOOP
                    # (never a sleep): a 7-8 s menu-freeze window must
                    # never hide inside a blocking shot-pause (bug found
                    # 2026-11-15). A second monitor connection is not
                    # possible: QEMU's HMP serves one client (tested).
                    for dl, tag in ((0.6, 'a'), (3.0, 'b'), (9.0, 'c')):
                        bisect.insort(
                            shotq,
                            (t + dl, si, tag, manifest['phases'][-1]))
                si += 1
            while shotq and t >= shotq[0][0]:
                _, sidx, tag, ph = shotq.pop(0)
                pth = os.path.join(wd, 'probe_%03d%s.ppm' % (sidx, tag))
                h.cmd('screendump ' + pth)
                ph.setdefault('shots', {})[tag] = pth
            if t > a.timeout:
                h.cmd('screendump ' + os.path.join(wd, 'timeout.ppm'))
                grid = text_grid(h)
                with open(os.path.join(wd, 'timeout_text.txt'), 'w') as f:
                    f.write(grid + '\n')
                manifest['phases'].append(
                    {'t': round(t, 1), 'event': 'timeout; text screen '
                     'written to timeout_text.txt'})
                manifest['max_tick_rate'] = round(max_rate, 2)
                with open(os.path.join(wd, 'capture_manifest.json'),
                          'w') as f:
                    json.dump(manifest, f, indent=1)
                raise SystemExit('timeout %ds waiting for map menu '
                                 '(max_rate=%.1f) - see timeout.ppm / '
                                 'timeout_text.txt' % (a.timeout, max_rate))
            tick = read_tick(h)
            if tick is None:                       # monitor reply desync
                bad = getattr(main, '_badtick', 0) + 1
                main._badtick = bad
                if bad <= 3 or bad % 40 == 0:
                    print('[t=%.0fs] WARNING tick read failed (#%d) raw=%r'
                          % (t, bad, getattr(read_tick, 'last_raw',
                                             '')[:100]), flush=True)
            if a.frame_interval > 0 and t >= fnext:
                fnext = t + a.frame_interval
                cur = os.path.join(wd, 'cur_frame.ppm')
                h.cmd('screendump ' + cur)
                px = ppm_read(cur)
                st = ppm_stats(px)
                if st:
                    fw, fh, fc, fm = st
                    hsh = hashlib.sha1(px).hexdigest()
                    if (fw, fh) != (m_w, m_h):       # CRTC mode episode
                        m_w, m_h, mode_t = fw, fh, t
                        art_seen = False
                        print('[t=%.0fs] CRTC mode %dx%d' % (t, fw, fh),
                              flush=True)
                        manifest['phases'].append(
                            {'t': round(t, 1), 'event': 'CRTC %dx%d'
                             % (fw, fh)})
                    if hsh != fhash:
                        main._fcap = 0    # new content: fresh cap budget
                        journal.write(json.dumps(
                            {'t': round(t, 1), 'w': fw, 'crtc_h': fh,
                             'colors': fc, 'mean': fm,
                             'hash': hsh[:12]}) + '\n')
                        journal.flush()
                        if fh != 480:      # movie frames journal-only
                            shutil.copyfile(
                                cur, os.path.join(
                                    wd, 'frame_%05.0f_%s.ppm' % (t, hsh[:8])))
                        fhash = hsh
                    if fh == 480 and fc > 8 and fm > 16:
                        movie_seen = True     # .IS chain visible: running
                    if (fw, fh) == (640, 400) and fc > 8 and fm > 16:
                        # ART on the 640x400 canvas: only the chapter map
                        # menu uses this mode (movie chain = 480, DOS
                        # text = 720 wide, fades = black)
                        if not art_seen:
                            art_seen = True
                            print('[t=%.0fs] 400-line ART colors=%d '
                                  'mean=%d (episode %.1fs) - menu'
                                  % (t, fc, fm, t - mode_t), flush=True)
            if prev is not None and tick is not None:
                dt = t - prev_t
                rate = (tick - prev) / dt if dt > 0 else 0.0
                if 0 < rate < MAX_RATE:
                    max_rate = max(max_rate, rate)
                    hi = getattr(main, '_hi', 0) + 1 if rate > PROVE_MOVIE else 0
                    main._hi = hi
                    if hi >= 2 and not movie_seen:
                        movie_seen = True
                        print('[t=%.0fs] movie clock ~%.0fHz sustained: '
                              'game launched, IS audio DMA paced'
                              % (t, rate), flush=True)
                        manifest['phases'].append(
                            {'t': round(t, 1), 'event': 'movie clock '
                             '~70Hz sustained (game launched, audio DMA '
                             'paced)'})
                if tick == prev:
                    stable += dt
                else:
                    stable = 0.0
                if len(manifest['tick_log']) < 4000:
                    manifest['tick_log'].append(
                        {'t': round(t, 1), 'tick': tick})
                game400 = (m_w, m_h) == (640, 400)
                # THE trigger is the ART frame itself: measured L7
                # 2026-11-15, the menu's static art frame appears ~0.6 s
                # per cycle and is BYTE-IDENTICAL across cycles (journal
                # hash b880e203 at t=30 AND t=266). A '>= 2 s episode'
                # gate can never fire; the frame IS the event.
                if (menu_t is None and
                        ((game400 and art_seen
                          and getattr(main, '_fcap', 0) < 6)
                         or (stable >= FREEZE_S and movie_seen))):
                    if game400:
                        print('[t=%.0fs] menu ART frame seen (episode '
                              '%.1fs): capturing' % (t, t - mode_t),
                              flush=True)
                    blob, status = capture_menu_state(
                        h, wd, want_art=want_art)
                    if want_art and status == 'ok-art':
                        manifest['menu_page_kind'] = 'art'
                        manifest['static_vram_equal'] = True
                        menu_t = t
                        break
                    if want_art and status in ('ok-dark', 'moving'):
                        # the stop landed in the near-black half of the
                        # cycle (the usual case: art shows ~0.6 s per
                        # 1.2 s); keep watching for the next ART frame
                        if status == 'ok-dark' and dark_blob is None:
                            dark_blob, dark_shot = blob, \
                                'menu_black.ppm'
                        print('[t=%.0fs] stop landed on %s half, '
                              'retrying on next art frame'
                              % (t, status), flush=True)
                        stable = 0.0
                        main._fcap = getattr(main, '_fcap', 0) + 1
                        art_seen = False
                        continue
                    if (not want_art) and (
                            status == 'ok'
                            or (game400 and t - mode_t >= 6.0
                                and art_seen)):
                        if status != 'ok':
                            # strict static-pair never landed, but the
                            # mode episode itself is the menu signature
                            manifest['static_vram_equal'] = False
                            manifest['capture_note'] = (
                                'accepted on 640x400 episode >= 6s with '
                                'art frames; vram single-read')
                        else:
                            manifest['static_vram_equal'] = True
                        menu_t = t
                        break
                    # frozen window was a fade/stall, not the static art
                    # frame: keep watching for the next one
                    print('[t=%.0fs] frozen window but plane status=%r, '
                          'continuing' % (t, status), flush=True)
                    manifest['phases'].append(
                        {'t': round(t, 1), 'event': 'frozen window, plane '
                         '%s - retrying' % status})
                    stable = 0.0
                    main._fcap = getattr(main, '_fcap', 0) + 1
                    # a black episode gets at most 6 capture attempts;
                    # re-arm needs new art content in the same episode
                    art_seen = False
            prev, prev_t = tick, t
            nsamp = getattr(main, '_ns', 0) + 1
            main._ns = nsamp
            if nsamp % 120 == 0:
                print('[t=%.0fs] tick=%d max_rate=%.1f frozen=%.1fs'
                      % (t, tick or -1, max_rate, stable), flush=True)
            time.sleep(SAMPLE_S)

        if menu_t is None and want_art and dark_blob is not None:
            # never caught the art half verified - fall back to the
            # verified near-black page (still the menu, still a pin)
            blob = dark_blob
            manifest['menu_page_kind'] = 'dark'
            manifest['static_vram_equal'] = True
            menu_t = t
        shot = 'menu_art.ppm' if manifest.get('menu_page_kind') == 'art' \
            else 'menu.ppm'
        if not os.path.exists(os.path.join(wd, shot)):
            shot = 'menu.ppm'
        if not os.path.exists(os.path.join(wd, shot)):
            shot = 'menu_black.ppm'      # black-half capture
        size = ppm_size(os.path.join(wd, shot))
        manifest['menu_screenshot'] = shot
        manifest['vram_window'] = ('0x%x' % getattr(read_vram, 'window',
                                                    0))
        with open(os.path.join(wd, 'vram.bin'), 'wb') as f:
            f.write(blob)
        manifest['phases'].append(
            {'t': round(menu_t, 1), 'event': 'menu state (400-line art '
             'and/or 0x46C frozen %.0fs): static index plane captured, '
             'CRTC=%s' % (stable, size)})
        print('menu captured at t=%.1fs (frozen %.1fs, CRTC=%s)'
              % (menu_t, stable, size), flush=True)

        for name in ('vram.bin', shot, 'menu_black.ppm',
                     'menu_art.ppm', 'static2.ppm'):
            p = os.path.join(wd, name)
            if os.path.exists(p):
                manifest['artifacts'][name] = {
                    'path': p, 'bytes': os.path.getsize(p),
                    'sha256': sha256(p)}
        manifest['menu_t'] = round(menu_t, 1)
        manifest['max_tick_rate'] = round(max_rate, 2)
        with open(os.path.join(wd, 'capture_manifest.json'), 'w') as f:
            json.dump(manifest, f, indent=1)
        print('captured: vram.bin (%s), menu.ppm static_vram_equal=%s'
              % (sha256(os.path.join(wd, 'vram.bin'))[:16],
                 manifest['static_vram_equal']))
        if not a.keep_running:
            h.cmd('quit')
    finally:
        if a.keep_running:
            print('qemu left running on monitor', mon)
        elif proc.poll() is None:
            try:
                proc.wait(timeout=10)
            except subprocess.TimeoutExpired:
                proc.terminate()
                proc.wait(timeout=10)
        log.close()


if __name__ == '__main__':
    main()
