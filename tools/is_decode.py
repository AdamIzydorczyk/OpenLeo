#!/usr/bin/env python3
"""Offline decoder for LongSoft iNFOSTREAM .IS cutscene video (codec v2.01).

Semantics from the live codec in LEO.EXE (Watcom LE), pinned against a
230k-event gdb hbreak trace of the running game (INTRO_P.IS attract replay)
and full 32-bit disassembly of (image vaddr, r2 off = vaddr-0x10000):

  0x1b0d9 frame/chunk control interpreter
  0x1af71 640x480 frame driver   (4 consecutive rows per band, bands=h/4)
  0x1ae78 320x200 Mode X driver  (4 planar segments per band, bands=h/4)
  0x1adb8 / 0x1acbb band opcode dispatchers (identical opcode grammar,
          budget per band = width pixels; every opcode subtracts its len
          from the budget except end-of-band; dispatcher loops to budget 0)
  0x1a998 SOLID  0x1a9fe LUT1  0x1ab9b LUT2  0x1ac29 LUT4  0x1ab32 RAW

Band opcode grammar (one opcode stream per frame, bands back-to-back):
  B: len = B&7; if B&0x40: len = (len<<8)|next byte (2-byte opcode)
  mode = B&0xb8:
    0x00 SOLID: +1 byte val; fill len px in each of 4 segments
    0x08 LUT1:  inline 2-byte table T; ceil(len/2) data bytes; per byte c:
                segment i writes px = T[bit(7-i)] and T[bit(3-i)]
                (odd tail len: one px per segment = T[bit(7-i)])
    0x10 LUT2:  inline 4-byte table; len data bytes; per byte c:
                seg0=T[c>>6] seg1=T[(c>>4)&3] seg2=T[(c>>2)&3] seg3=T[c&3]
    0x18 LUT4:  inline 16-byte table; len 2-byte pairs; per pair (u,v):
                seg0=T[u>>4] seg1=T[u&15] seg2=T[v>>4] seg3=T[v&15]
    0x20 RAW:   literal 4*len bytes: len px per segment, seg0 first
    0x90 SKIP:  move write cursors +len px (budget still reduced)
    0x88 EOB:   end band early; frame band-counter advances by len
    other:      no-op but budget still reduced
  Segment write cursors advance by len per segment for all draw ops;
  they persist across bands in Mode X (plane row advance = width) and
  are re-based per band in the 640 path (row bases).

Frame-level control opcodes (same B&0xb8 mask), polled between frames:
  0xb0 ESC:   u32 follow; stream resumes at payload_pos + 5 + u32
              (live-verified: never crosses the payload end - 0 crossings
              in all 2388 INTRO_P payloads)
  0x98 RPT8:  next byte n  -> [c410c]=n-1, [cde2c]=1 handshake with the
              player loop; NOT read by the band drivers, so it does not
              duplicate frames in the decode (verified by disassembly of
              all ref sites: only writes inside codec, reads at 0xbd11/
              0xbde6/0xbe50 in the player)
  0xa8 RPT16: next word n  -> same, 16-bit
  0xa0 PAL:   768-byte raw VGA palette inline (DAC order RGB triples),
              written straight to 0x3c8/0x3c9 (all 256 entries)
  0xb8 END:   end of stream (sits ~1 byte before the payload end; the
              tail bytes are not opcodes)
Any other opcode starts a full frame decode (h/4 bands).

Framebuffer model:
  640x480:  frame-linear, row r at fb[r*w:...]
  320x200:  planar like VGA A0000: plane p at fb[p*w*h/4 ...]; frame row =
            4*plane_row + p

Usage:
  is_decode.py check FILE.IS [--chunks N]
  is_decode.py frames FILE.IS OUTDIR [--ppm] [--only a,b,c] [--every K]
"""
import argparse
import os
import struct
import sys

ESC, RPT8, RPT16, PAL, END, EOB, SKIP = 0xB0, 0x98, 0xA8, 0xA0, 0xB8, 0x88, 0x90


class Desync(Exception):
    pass


class IS:
    def __init__(self, path):
        self.d = open(path, 'rb').read()
        self.w = struct.unpack_from('<H', self.d, 0x59)[0]
        self.h = struct.unpack_from('<H', self.d, 0x5B)[0]
        self.fps = self.d[0x5D]
        self.modex = self.w == 320
        self.payloads = []
        p = 0x5F
        n = len(self.d)
        while p + 7 <= n:
            typ, ln, tail = struct.unpack_from('<BIH', self.d, p)
            if ln < 7 or p + ln > n:
                break
            self.payloads.append((p + 7, ln - 7, tail))
            p += ln

    def seg_bases(self, band):
        q = self.w * self.h // 4
        if self.modex:
            return [i * q + band * self.w for i in range(4)]
        return [(band * 4 + i) * self.w for i in range(4)]

    def decode_frame(self, pos, fb, stat):
        """Decode one full frame (h/4 bands) starting at stream pos.
        640 path (af71): segments re-based from band index each band
        (EOB advances band index by len).  Mode X (ae78): segment cursors
        persist across bands; EOB does not advance them."""
        w, h = self.w, self.h
        band = 0
        nbands = h // 4
        seg = self.seg_bases(0)
        while band < nbands:
            if not self.modex:
                seg = self.seg_bases(band)
            budget = w
            eob_adv = 0
            while budget > 0:
                b = self.d[pos]
                ln = b & 7
                pos += 1
                if b & 0x40:
                    ln = (ln << 8) | self.d[pos]
                    pos += 1
                mode = b & 0xB8
                if mode == 0x00:                     # SOLID
                    val = self.d[pos]
                    pos += 1
                    blk = bytes([val]) * ln
                    for i in range(4):
                        fb[seg[i]:seg[i] + ln] = blk
                        seg[i] += ln
                    budget -= ln
                elif mode == 0x08:                   # LUT1
                    t0, t1 = self.d[pos], self.d[pos + 1]
                    pos += 2
                    for _ in range(ln >> 1):
                        c = self.d[pos]
                        pos += 1
                        for i in range(4):
                            s = seg[i]
                            fb[s] = t0 if (c >> (7 - i)) & 1 else t1
                            fb[s + 1] = t0 if (c >> (3 - i)) & 1 else t1
                            seg[i] = s + 2
                    if ln & 1:
                        c = self.d[pos]
                        pos += 1
                        for i in range(4):
                            s = seg[i]
                            fb[s] = t0 if (c >> (7 - i)) & 1 else t1
                            seg[i] = s + 1
                    budget -= ln
                    stat['lut1'] = stat.get('lut1', 0) + 1
                elif mode == 0x10:                   # LUT2
                    t = self.d[pos:pos + 4]
                    pos += 4
                    for _ in range(ln):
                        c = self.d[pos]
                        pos += 1
                        fb[seg[0]] = t[c >> 6]
                        fb[seg[1]] = t[(c >> 4) & 3]
                        fb[seg[2]] = t[(c >> 2) & 3]
                        fb[seg[3]] = t[c & 3]
                        seg[0] += 1
                        seg[1] += 1
                        seg[2] += 1
                        seg[3] += 1
                    budget -= ln
                    stat['lut2'] = stat.get('lut2', 0) + 1
                elif mode == 0x18:                   # LUT4
                    t = self.d[pos:pos + 16]
                    pos += 16
                    for k in range(ln):
                        u = self.d[pos + 2 * k]
                        v = self.d[pos + 2 * k + 1]
                        fb[seg[0]] = t[u >> 4]
                        fb[seg[1]] = t[u & 15]
                        fb[seg[2]] = t[v >> 4]
                        fb[seg[3]] = t[v & 15]
                        seg[0] += 1
                        seg[1] += 1
                        seg[2] += 1
                        seg[3] += 1
                    pos += 2 * ln
                    budget -= ln
                    stat['lut4'] = stat.get('lut4', 0) + 1
                elif mode == 0x20:                   # RAW
                    for i in range(4):
                        fb[seg[i]:seg[i] + ln] = self.d[pos:pos + ln]
                        pos += ln
                        seg[i] += ln
                    budget -= ln
                    stat['raw'] = stat.get('raw', 0) + 1
                elif mode == SKIP:
                    stat['skip'] = stat.get('skip', 0) + 1
                    for i in range(4):
                        seg[i] += ln
                    budget -= ln
                elif mode == EOB:
                    stat['eob'] = stat.get('eob', 0) + 1
                    eob_adv = ln
                    break
                else:
                    stat['unknown'] = stat.get('unknown', 0) + 1
                    budget -= ln
            else:
                band += 1
                continue
            band += eob_adv if eob_adv else 1   # a8970=len (defensive >=1)
        return pos

    def decode_all(self, stat, on_frame=None, max_frames=1 << 30):
        """Chunk-at-a-time stream model: each chunk payload carries a
        slice of the opcode stream; control opcodes (incl. ESC skips of
        audio tails) and whole frames interleave.  Framebuffer/palette
        persist across chunks (1b1a0 feeds one chunk per call)."""
        fb = bytearray(self.w * self.h)
        pal = bytes(768)
        frames = 0
        for ps, pl, tail in self.payloads:
            pos = ps
            stop = ps + pl
            stat['chunk'] = stat.get('chunk', 0) + 1
            while pos < stop and frames < max_frames:
                b = self.d[pos]
                mode = b & 0xB8
                if mode == ESC:
                    u = struct.unpack_from('<I', self.d, pos + 1)[0]
                    stat['esc'] = stat.get('esc', 0) + 1
                    stat['esc_bytes'] = stat.get('esc_bytes', 0) + u
                    pos += 5 + u
                    continue
                if mode == END:
                    break
                if mode == RPT8:
                    stat['repeat'] = self.d[pos + 1]
                    pos += 2
                    continue
                if mode == RPT16:
                    stat['repeat'] = struct.unpack_from('<H',
                                                        self.d, pos + 1)[0]
                    pos += 3
                    continue
                if mode == PAL:
                    pal = self.d[pos + 1:pos + 769]
                    stat['palette'] = stat.get('palette', 0) + 1
                    pos += 769
                    continue
                pos = self.decode_frame(pos, fb, stat)
                frames += 1
                stat['frames'] = frames
                if on_frame:
                    on_frame(frames - 1, fb, pal, stat)
        return fb, pal


def write_ppm(path, fb, pal, w, h):
    with open(path, 'wb') as f:
        f.write(f'P6\n{w} {h}\n255\n'.encode())
        if w == 320:            # uninterleave Mode X planes
            q = w * h // 4
            rows = []
            for p in range(4):
                plane = fb[p * q:(p + 1) * q]
                for r in range(h // 4):
                    rows.append((4 * r + p, plane[r * w:(r + 1) * w]))
            rows.sort()
            data = b''.join(d for _, d in rows)
        else:
            data = bytes(fb)
        rgb = bytearray(3 * 256)
        for i in range(256):
            # stream palette is 6-bit DAC; scale to 8-bit (live DAC was
            # v<<2 with occasional +3, verified vs QEMU screendumps)
            for j in range(3):
                rgb[3 * i + j] = pal[3 * i + j] << 2
        out = bytearray(3 * w * h)
        for i, v in enumerate(data):
            out[3 * i:3 * i + 3] = rgb[3 * v:3 * v + 3]
        f.write(bytes(out))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('cmd', choices=['check', 'frames'])
    ap.add_argument('file')
    ap.add_argument('outdir', nargs='?', default='frames_out')
    ap.add_argument('--only', type=lambda s: [int(x) for x in s.split(',')])
    ap.add_argument('--every', type=int, default=0)
    ap.add_argument('--ppm', action='store_true')
    a = ap.parse_args()

    is_ = IS(a.file)
    print(f'# {a.file} {is_.w}x{is_.h} fps={is_.fps} modex={is_.modex} '
          f'chunks={len(is_.payloads)}', file=sys.stderr)
    stat = {}
    want = set(a.only or [])

    def cb(i, fb, pal, st):
        if a.ppm and (i in want or (a.every and i % a.every == 0)):
            os.makedirs(a.outdir, exist_ok=True)
            write_ppm(os.path.join(a.outdir, f'f{i:05d}.ppm'),
                      fb, pal, is_.w, is_.h)

    try:
        fb, pal = is_.decode_all(stat, cb)
        print(f'OK frames={stat} ', file=sys.stderr)
    except Desync as e:
        print(f'DESYNC: {e} stat={stat}', file=sys.stderr)
        sys.exit(1)


if __name__ == '__main__':
    main()
