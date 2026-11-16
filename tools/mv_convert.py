#!/usr/bin/env python3
"""mv_convert.py - offline .IS -> .LVM re-conversion (PR-41, "option 1").

STATUS 2026-11-16: PR-41 was SHELFED by the owner (player stays .IS-
only; see NEXT_STEPS.md PR-41 (SHELVED) record). Kept as a standalone
offline tool - NOT run by CI; needs the .IS stage (DATA) to operate.

Two passes over the chunks, mirroring the player exactly:
  pass 1 (audio): the is_walk_audio_range grammar - collect every ESC
    region (bytes + sizes), the last-RPT state per chunk, and the
    region gcd (stereo = g && g%1260==0, bps 88200/44100). The stored
    audio blob is byte-for-byte what plat_audio_queue receives today:
    region bytes + enqueue_audio's alignment pads + queue_ahead's
    silent-chunk silence fills (1260 B blocks + rest, rep = st.repeat
    else fps else 2).
  pass 2 (video): the is_decode.decode_all model (the reference
    decoder itself) - frames per chunk + PAL ops IN DECODE ORDER as an
    event list; palette changes are attached to the NEXT present,
    exactly as the C codec's persistent-pal visibility rules; frames
    are linearized (Mode X un-interleave as in write_ppm) and
    exact-duplicate frames become hold-only records.

Per-chunk hold = region/bps (audio-block clock, 3bfd8a8 model) or,
for silent chunks, repeat*1/70 s - numerically identical to
is_player_run, so the .LVM plays at the same A/V positions as the
verified .IS path.

Optional temporal de-ghost (default ON, tools/mv_clean.py): a pure
function over the linear index-frame sequence; --no-clean writes the
raw decoded frames. The python side is the oracle: ported/lvm.c must
reconstruct identical content. CI: verify-movies.

LVM1 container (all little-endian):
  "LVM1" | u32 w, u32 h | u32 rate(22050), u8 ch(1|2), u16 bits(16)
  | u64 audio_len, u32 nrec
  | audio bytes (s16le PCM)
  | nrec x { u64 hold_ns, u8 flags(bit0=frame, bit1=pal768),
             [pal], [u8 codec(0 raw | 1 RLE) + frame] }
  RLE: u8 ctl: ctl>0 -> next ctl bytes literal; ctl==0 -> u16 run + u8.
       Literals never contain 0x00 (the encoder stores every zero byte as
       a run, even isolated), so the stream is unambiguous and
       self-delimiting: a frame ends exactly where its output reaches w*h.

Usage: mv_convert.py IN.IS OUT.LVM [--no-clean] [--wav-out F] [--report]
"""
import argparse
import math
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from is_decode import IS  # the pinned python reference decoder

MAGIC = b'LVM1'


# ---- pass 1: audio walk (grammar = is_walk_audio_range) ------------------

def skip_frame(iso, pos):
    """Draw-free frame skip, byte-exact port of the C audio walk band
    walk (same consumption as decode_frame on well-formed data; CI
    pins equality by comparing extracted audio vs LEO_IS_WAV)."""
    d = iso.d
    band = 0
    nbands = iso.h // 4
    while band < nbands:
        budget = iso.w
        eob_adv = 0
        while budget > 0:
            bb = d[pos]
            pos += 1
            ln = bb & 7
            if bb & 0x40:
                ln = (ln << 8) | d[pos]
                pos += 1
            mode = bb & 0xB8
            data = 0
            if mode == 0x00:               # SOLID
                data = 1
            elif mode == 0x08:             # LUT1
                data = 2 + ((ln + 1) >> 1)
            elif mode == 0x10:             # LUT2
                data = 4 + ln
            elif mode == 0x18:             # LUT4
                data = 16 + 2 * ln
            elif mode == 0x20:             # RAW
                data = 4 * ln
            if mode != 0x88:               # everything but EOB spends
                budget -= ln               # band budget (incl. SKIP)
            pos += data
            if mode == 0x88:               # EOB
                eob_adv = ln
                break
        band += eob_adv if eob_adv else 1
    return pos


def audio_pass(iso):
    """Per-chunk audio facts + global region sizes + last-RPT state."""
    d = iso.d
    chunks = []   # {'regions': [(pos,len)], 'region': int, 'rep_after': int}
    repeat = 0
    g = 0
    for ps, pl, tail in iso.payloads:
        pos, stop = ps, ps + pl
        regions = []
        region_total = 0
        while pos < stop:
            b = d[pos]
            mode = b & 0xB8
            if mode == 0xB0:                        # ESC
                u = struct.unpack_from('<I', d, pos + 1)[0]
                a, e = pos + 5, min(pos + 5 + u, stop)
                if e > a:
                    regions.append((a, e - a))
                    region_total += e - a
                    g = math.gcd(g, e - a)
                pos += 5 + u
                continue
            if mode == 0xA8:                        # RPT16
                repeat = struct.unpack_from('<H', d, pos + 1)[0]
                pos += 3
                continue
            if mode == 0x98:                        # RPT8
                repeat = d[pos + 1]
                pos += 2
                continue
            if mode == 0xA0:                        # PAL
                pos += 769
                continue
            if mode == 0xB8:                        # END
                break
            pos = skip_frame(iso, pos)
        chunks.append({'regions': regions, 'region': region_total,
                       'rep_after': repeat})
    stereo = bool(g and g % 1260 == 0)
    bps = 88200 if stereo else 44100
    return chunks, stereo, bps, g


def build_audio(iso, chunks, reps, stereo, bps):
    """The exact queued stream (with pads + silence fills) and the
    LEO_IS_WAV-equivalent stream (raw regions + silence, no pads).
    Silent-chunk silence uses the DECODE-grammar repeat state (reps,
    from video_pass) exactly as the C player's st.repeat handshake."""
    d = iso.d
    ticks = iso.fps if iso.fps > 0 else 2
    aq = bytearray()
    aw = bytearray()
    for ci, c in enumerate(chunks):
        for pos, ln in c['regions']:
            aq += d[pos:pos + ln]
            aw += d[pos:pos + ln]
            pad = ((4 - (ln & 3)) & 3) if stereo else (ln & 1)
            aq += b'\0' * pad
        if c['region'] == 0:
            rep = reps[ci] if reps[ci] else ticks
            sil = rep * bps // 70
            while sil >= 1260:
                aq += b'\0' * 1260
                aw += b'\0' * 1260
                sil -= 1260
            if sil:
                aq += b'\0' * sil
                aw += b'\0' * sil
    return bytes(aq), bytes(aw)


# ---- pass 2: video (decode_all model, events in decode order) ------------

def video_pass(iso):
    """Per-chunk ordered event lists: ('pal', bytes) / ('frame', bytes fb
    snapshot) - the decode_all walk using the reference decoder itself.
    ALSO tracks the is_stream st.repeat handshake (RPT8/RPT16 operands,
    decode-grammar positions only, persists across chunks): the C
    player reads exactly this state for silent-chunk holds and audio
    silence fills (the audio walk itself never sets it - pinned on
    PRAWA 2026-11-16 where the audio-walk grammar sees RPT bytes the
    decode grammar consumes as frame data)."""
    d = iso.d
    w, h = iso.w, iso.h
    fb = bytearray(w * h)
    stat = {}
    out = []
    reps = []
    repeat = 0
    for ps, pl, tail in iso.payloads:
        pos, stop = ps, ps + pl
        ev = []
        while pos < stop:
            b = d[pos]
            mode = b & 0xB8
            if mode == 0xB0:                        # ESC
                u = struct.unpack_from('<I', d, pos + 1)[0]
                stat['esc'] = stat.get('esc', 0) + 1
                stat['esc_bytes'] = stat.get('esc_bytes', 0) + u
                pos += 5 + u
                continue
            if mode == 0xA8:
                repeat = struct.unpack_from('<H', d, pos + 1)[0]
                stat['repeat'] = repeat
                pos += 3
                continue
            if mode == 0x98:
                repeat = d[pos + 1]
                stat['repeat'] = repeat
                pos += 2
                continue
            if mode == 0xA0:                        # PAL
                pal = bytes(d[pos + 1:pos + 769])
                ev.append(('pal', pal))
                stat['palette'] = stat.get('palette', 0) + 1
                pos += 769
                continue
            if mode == 0xB8:                        # END
                break
            pos = iso.decode_frame(pos, fb, stat)
            ev.append(('frame', bytes(fb)))
            stat['frames'] = stat.get('frames', 0) + 1
        out.append(ev)
        reps.append(repeat)
    return out, reps, stat


def linearize(fb, w, h, modex):
    if not modex:
        return bytes(fb)
    q = w * h // 4
    rows = []
    for p in range(4):
        plane = fb[p * q:(p + 1) * q]
        for r in range(h // 4):
            rows.append((4 * r + p, plane[r * w:(r + 1) * w]))
    rows.sort()
    return b''.join(d for _, d in rows)


# ---- RLE (trivial, deterministic; the C lvm.c decodes the mirror) --------

def rle_encode(data):
    out = bytearray()
    n = len(data)
    i = 0
    lit = bytearray()

    def flush_lit():
        nonlocal out, lit
        while lit:
            take = min(len(lit), 255)
            out.append(take)
            out += lit[:take]
            del lit[:take]

    while i < n:
        v = data[i]
        j = i + 1
        while j < n and data[j] == v and j - i < 65535:
            j += 1
        # 0x00 control is the run marker, so a literal may never contain
        # 0x00: every zero byte (even isolated) is stored as a run.
        if j - i >= 4 or v == 0:
            flush_lit()
            out.append(0)
            out += struct.pack('<H', j - i)
            out.append(v)
            i = j
        else:
            lit.append(v)
            i += 1
    flush_lit()
    return bytes(out)


# ---- record assembly ------------------------------------------------------

def assemble(iso, chunks, vevid, reps, stereo, bps):
    """[(hold_ns, pal|None, linear_frame|None)] with C-exact palette
    visibility (pending PAL applies at the next present; a PAL never
    reaching a present is invisible in C and never stored) and C-exact
    silent-chunk holds (decode-grammar repeat state, as in is_player)."""
    modex = (iso.w == 320)          # same rule as is_decode.write_ppm
    ticks = iso.fps if iso.fps > 0 else 2
    records = []
    last_pal = bytes(768)
    pending = None
    prev_lin = None
    for ci, (c, ev) in enumerate(zip(chunks, vevid)):
        hold = (c['region'] * 1000000000 // bps) if c['region'] \
            else ((reps[ci] if reps[ci] else ticks)
                  * 1000000000 // 70)
        # present events of this chunk, in decode order
        presents = [e for e in ev if e[0] == 'frame']
        idx = 0
        for kind, val in ev:
            if kind == 'pal':
                if val != last_pal:
                    pending = val
                continue
            lin = linearize(val, iso.w, iso.h, modex)
            pal_out = pending
            pending = None
            if pal_out is not None:
                last_pal = pal_out
            fr = lin if lin != prev_lin else None
            prev_lin = lin if lin is not None else prev_lin
            last = idx == len(presents) - 1
            records.append((hold if last else 0, pal_out, fr))
            idx += 1
        if not presents:
            records.append((hold, None, None))
    return records


def apply_clean(records, w, h, report):
    """Batch de-ghost: plan() decides which frame records become
    hold-only; palette changes displaced by suppression are rehomed
    to the next present (C applies a PAL at the next present too)."""
    import numpy as np
    from mv_clean import plan
    idxs = [i for i, r in enumerate(records) if r[2] is not None]
    frames = [np.frombuffer(records[i][2], np.uint8).reshape(h, w)
              for i in idxs]
    sup, st = plan(frames)
    # merged records KEEP their own PAL: the palette timeline (fades) is
    # real visual content and stays at its original time - the C reader
    # presents such records for that reason, so the replay contract
    # counts them (clean_merged_pal below).
    st['frames_merged_pal'] = sum(1 for j in sup
                                  if records[idxs[j]][1] is not None)
    for j in sorted(sup):
        i = idxs[j]
        records[i] = (records[i][0], records[i][1], None)
    # forward PAL re-home: a PAL rides to the next present (exact C
    # visibility semantics; a trailing never-presented PAL drops)
    pending = None
    for i, (hold, pal, fr) in enumerate(records):
        if pal is not None:
            pending = pal
        if fr is not None and pending is not None:
            records[i] = (hold, pending, fr)
            pending = None
    report.update({'clean_runs': st['runs'],
                   'clean_merged': st['frames_merged'],
                   'clean_merged_pal': st['frames_merged_pal'],
                   'clean_rejected_runs': st['runs_rejected'],
                   'part_steps': st['part_steps'],
                   'full_steps': st['full_steps'],
                   'noise_steps': st['noise_steps']})


# ---- container writer ------------------------------------------------------

def write_lvm(path, w, h, stereo, audio, records, report):
    body = bytearray()
    st = {'nframe_recs': 0, 'nhold_recs': 0, 'npals': 0,
          'rle_bytes': 0, 'raw_bytes': 0}
    for hold_ns, pal, fr in records:
        flags = (1 if fr is not None else 0) | (2 if pal is not None else 0)
        body += struct.pack('<QB', hold_ns, flags)
        if pal is not None:
            body += pal
            st['npals'] += 1
        if fr is None:
            st['nhold_recs'] += 1
            continue
        st['nframe_recs'] += 1
        enc = rle_encode(fr)
        if len(enc) + 1 < len(fr):
            body += b'\x01' + enc
            st['rle_bytes'] += len(enc)
        else:
            body += b'\x00' + fr
            st['raw_bytes'] += len(fr)
    hdr = MAGIC + struct.pack('<IIIBH', w, h, 22050, 2 if stereo else 1, 16)
    hdr += struct.pack('<QI', len(audio), len(records))
    with open(path, 'wb') as f:
        f.write(hdr + audio + bytes(body))
    report.update(st)
    report['audio_len'] = len(audio)
    report['file'] = os.path.getsize(path)


def convert(inp, outp, clean=True, wav_out=None, report=None):
    iso = IS(inp)
    report = report if report is not None else {}
    chunks, stereo, bps, g = audio_pass(iso)
    vevid, reps, stat = video_pass(iso)
    audio, awav = build_audio(iso, chunks, reps, stereo, bps)
    records = assemble(iso, chunks, vevid, reps, stereo, bps)
    if clean:
        apply_clean(records, iso.w, iso.h, report)
    report.update({'frames': stat.get('frames', 0), 'chunks': len(chunks),
                   'stereo': stereo, 'bps': bps, 'gcd': g,
                   'silent_chunks': sum(1 for c in chunks
                                        if not c['region'])})
    write_lvm(outp, iso.w, iso.h, stereo, audio, records, report)
    if wav_out:
        with open(wav_out, 'wb') as f:
            f.write(awav)
        report['wav_len'] = len(awav)
    return records


def main():
    ap = argparse.ArgumentParser(prog='mv_convert.py')
    ap.add_argument('inp')
    ap.add_argument('outp')
    ap.add_argument('--no-clean', action='store_true')
    ap.add_argument('--wav-out')
    ap.add_argument('--report', action='store_true')
    a = ap.parse_args()
    rep = {}
    convert(a.inp, a.outp, clean=not a.no_clean, wav_out=a.wav_out,
            report=rep)
    if a.report:
        import json
        print(json.dumps(rep, sort_keys=True))


if __name__ == '__main__':
    main()
