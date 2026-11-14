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

State detector: the BIOS tick counter at linear 0x46C. It runs ~18.2 Hz
under DOS, ~70 Hz while an .IS movie plays (the game reprograms PIT ch0,
see ll_config.h), and FREEZES outright at the chapter-map menu (the only
known state that stops it, PLAN.md S6: frozen for ~7-8 s at a time).
That window is short, so the counter is sampled at SAMPLE_S intervals; a
freeze of >= FREEZE_S after the movie clock was seen identifies the map
menu deterministically, with no wall-clock alignment needed (the frame
in that state is static).

Artifacts written to --workdir (gitignored, /tmp):
  vram.bin      256 KiB VRAM BAR window (HMP xp /65536b 0xfd000000 x4;
                first 64000 B = the linear 8bpp 320x200 index plane)
  menu.ppm      screendump at the same instant (640x400, 2x2-doubled)
  static2.ppm   second screendump ~2 s later (proves the frame is static)
  capture_manifest.json  hashes + tick-rate log + exact QEMU cmdline

Usage: golden_qemu_run.py [--workdir DIR] [--timeout SEC]
                          [--floppy IMG] [--hd QCOW2] [--keep-running]
"""
import argparse
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
FREEZE_S = 3.0        # consecutive equal 40:6C samples -> menu state
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
    """Tiny HMP client over the QEMU monitor unix socket."""

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
        self.f = self.s.makefile('rw', newline='')

    def _read_until_prompt(self):
        out, line = [], ''
        while True:
            line = self.f.readline()
            if line == '':
                raise SystemExit('monitor EOF')
            line = line.rstrip('\r\n')
            if line.startswith('(qemu)'):
                out.append(line[len('(qemu)'):].strip())
                return '\n'.join(x for x in out if x)
            out.append(line)

    def cmd(self, c):
        self.f.write(c + '\r')
        self.f.flush()
        return self._read_until_prompt()


def read_tick(h):
    r = h.cmd('x /wx 0x46c')
    m = re.search(r':\s+(0x[0-9a-f]+)', r)
    return int(m.group(1), 16) if m else None


def read_vram(h):
    """xp /65536b 0xfd000000, 4 chunks of 16 KiB -> 256 KiB."""
    blob = bytearray()
    for i in range(4):
        r = h.cmd('xp /%db 0x%x' % (16384, 0xfd000000 + i * 16384))
        n = 0
        for line in r.splitlines():
            _, _, rest = line.partition(':')
            for tok in rest.split():
                tok = tok[2:] if tok.startswith('0x') else tok
                if re.fullmatch(r'[0-9a-fA-F]{2}', tok):
                    blob.append(int(tok, 16))
                    n += 1
        if n != 16384:
            raise SystemExit('xp chunk %d parsed %d bytes' % (i, n))
    return bytes(blob)


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


def capture_menu_state(h, wd):
    """At a frozen-tick window: stop the VM and grab the index plane.
    The menu cycles art<->black (one art frame ~0.9 s, PLAN S6), so retry
    a couple of times if the plane happens to be in a black phase.
    Returns (vram_blob, status): 'ok' = two consecutive stopped reads of
    the index plane are IDENTICAL and non-trivial (the static menu frame);
    'black-or-moving' = never got a stable non-black plane in 4 tries.
    """
    for attempt in range(4):
        h.cmd('stop')
        blob = read_vram(h)
        if plane_ok(blob):
            h.cmd('screendump ' + os.path.join(wd, 'menu.ppm'))
            h.cmd('cont')
            time.sleep(0.5)
            h.cmd('stop')
            blob2 = read_vram(h)
            if blob == blob2:
                h.cmd('screendump ' + os.path.join(wd, 'static2.ppm'))
                return blob, 'ok'
            blob = blob2
        h.cmd('cont')
        time.sleep(1.0)
    return blob, 'black-or-moving'


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--workdir', default=WORK)
    ap.add_argument('--timeout', type=int, default=420)
    ap.add_argument('--floppy')
    ap.add_argument('--hd')
    ap.add_argument('--ram', type=int, default=32)
    ap.add_argument('--keep-running', action='store_true')
    a = ap.parse_args()

    wd = a.workdir
    floppy = a.floppy or os.path.join(wd, 'boot_floppy.img')
    hd = a.hd or os.path.join(wd, 'leo_hd.qcow2')
    for p in (floppy, hd):
        if not os.path.exists(p):
            raise SystemExit('missing rig image: ' + p)
    mon = os.path.join(wd, 'mon.sock')
    if os.path.exists(mon):
        os.unlink(mon)

    qemu = ['qemu-system-i386', '-m', str(a.ram), '-boot', 'a',
            '-fda', floppy, '-hda', hd,
            '-machine', 'pc', '-vga', 'std', '-display', 'none',
            '-audiodev', 'none,id=a0', '-device', 'sb16,audiodev=a0',
            '-monitor', 'unix:' + mon + ',server,nowait']
    ver = subprocess.run(['qemu-system-i386', '--version'],
                         capture_output=True, text=True).stdout.splitlines()[0]
    log = open(os.path.join(wd, 'qemu.log'), 'wb')
    proc = subprocess.Popen(qemu, stdout=log, stderr=subprocess.STDOUT)
    manifest = {
        'qemu_version': ver,
        'qemu_argv': qemu,
        'inputs': {'floppy': {'path': floppy, 'sha256': sha256(floppy)},
                   'hd': {'path': hd, 'sha256': sha256(hd)}},
        'tick_log': [], 'phases': [], 'artifacts': {},
    }
    print('qemu pid', proc.pid)

    try:
        h = HMP(mon)
        h.cmd('')                                  # eat banner
        t0 = time.time()
        prev, prev_t, stable = None, None, 0
        max_rate, movie_seen, menu_t = 0.0, False, None
        while True:
            t = time.time() - t0
            if proc.poll() is not None:
                raise SystemExit('qemu exited early (rc %s), see qemu.log'
                                 % proc.returncode)
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
                if (stable >= FREEZE_S and movie_seen and
                        menu_t is None):
                    blob, status = capture_menu_state(h, wd)
                    if status == 'ok':
                        menu_t = t
                        manifest['static_vram_equal'] = True
                        break
                    # frozen window was a fade/stall, not the static art
                    # frame: keep watching for the next one
                    print('[t=%.0fs] frozen window but plane status=%r, '
                          'continuing' % (t, status), flush=True)
                    manifest['phases'].append(
                        {'t': round(t, 1), 'event': 'frozen window, plane '
                         '%s - retrying' % status})
                    stable = 0.0
            prev, prev_t = tick, t
            nsamp = getattr(main, '_ns', 0) + 1
            main._ns = nsamp
            if nsamp % 120 == 0:
                print('[t=%.0fs] tick=%d max_rate=%.1f frozen=%.1fs'
                      % (t, tick or -1, max_rate, stable), flush=True)
            time.sleep(SAMPLE_S)

        size = ppm_size(os.path.join(wd, 'menu.ppm'))
        with open(os.path.join(wd, 'vram.bin'), 'wb') as f:
            f.write(blob)
        manifest['phases'].append(
            {'t': round(menu_t, 1), 'event': '40:6C frozen %.0fs (chapter '
             'map menu): static index plane captured, CRTC=%s'
             % (stable, size)})
        print('menu captured at t=%.1fs (frozen %.1fs, CRTC=%s)'
              % (menu_t, stable, size), flush=True)

        for name in ('vram.bin', 'menu.ppm', 'static2.ppm'):
            p = os.path.join(wd, name)
            manifest['artifacts'][name] = {
                'path': p, 'bytes': os.path.getsize(p), 'sha256': sha256(p)}
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
