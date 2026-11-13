#!/usr/bin/env python3
"""
le_dump.py - Watcom LE (Linear Executable) loader for RE work.

Reads a DOS4GW/LE binary (LEO.EXE, SETUP.EXE...), maps all demand-load pages
into a flat linear image at their runtime addresses, applies every LE fixup
(internal, chains, source lists, additive; semantics per rizin le.c), and emits:

  <out>.img    flat linear image covering all objects (0x10000...)
  <out>.elf    ELF32 i386, one PT_LOAD per LE object + entry point,
               relocation-resolved, ready for Ghidra / radare2 / retdec
  <out>.json   header info, segments, entry, resident names, fixup stats

Usage: le_dump.py IN.EXE [OUTPREFIX]
"""
import json
import struct
import sys

FIELDS = ['magic','border','worder','level','cpu','os','ver','mflags','mpages',
          'startobj','eip','stackobj','esp','pagesize','le_last_page_size',
          'fixupsize','fixupsum','ldrsize','ldrsum','objtab','objcnt','objmap',
          'itermap','rsrctab','rsrccnt','restab','enttab','dirtab','dircnt',
          'fpagetab','frectab','impmod','impmodcnt','impproc','pagesum',
          'datapage','preload','nrestab','cbnrestab','nressum','autodata',
          'debuginfo','debuglen','instpreload','instdemand','heapsize','stacksize']
FMT = '<2sBB' + 'H' * 2 + 'I' * (len(FIELDS) - 4)   # magic,2xbyte,level..stacksize

# fixup record flags (rz le_specs.h)
F_SOURCE_TYPE_MASK, F_SOURCE_LIST = 0xF, 0x20
F_TARGET_TYPE_MASK = 0x3
F_TARGET_ADDITIVE, F_TARGET_CHAIN = 0x4, 0x8
F_TARGET_OFF32, F_TARGET_ADD32, F_TARGET_ORD16, F_TARGET_ORD8 = 0x10, 0x20, 0x40, 0x80
TGT_INTERNAL, TGT_IMP_ORD, TGT_IMP_NAME, TGT_INT_ENTRY = 0, 1, 2, 3


class LEFile:
    def __init__(self, path):
        self.data = bytearray(open(path, 'rb').read())
        le_off = struct.unpack_from('<I', self.data, 0x3c)[0]
        h = dict(zip(FIELDS, struct.unpack_from(FMT, self.data, le_off)))
        assert h['magic'] == b'LE', f'{path}: not an LE file ({h["magic"]!r})'
        assert not (h['border'] or h['worder']), 'non-native byte order'
        self.le_off, self.h = le_off, h
        self.pagesize = h['pagesize']

        self.objects = []
        for i in range(h['objcnt']):
            vs, base, flags, pti, pte, _r = struct.unpack_from(
                '<6I', self.data, le_off + h['objtab'] + i * 24)
            self.objects.append(dict(vsize=vs, base=base, flags=flags,
                                     pti=pti, pte=pte))

        # classic LE page table at objmap: BE32 [3B page number][1B info] (rizin le.c:755)
        rec0 = le_off + h['objmap']
        self.pages = []
        for pi in range(h['mpages']):
            rec = struct.unpack_from('>I', self.data, rec0 + pi * 4)[0]
            info, pageno = rec & 0xFF, rec >> 8
            assert info == 0, f'page {pi+1} info {info:#02x}: iterated/compressed unsupported'
            self.pages.append(dict(info=info, pageno=pageno or pi + 1))
        for oi, o in enumerate(self.objects):
            for i in range(o['pte']):
                p = self.pages[o['pti'] - 1 + i]
                p['obj'], p['vaddr'] = oi + 1, o['base'] + i * self.pagesize
        self.lo = min(o['base'] for o in self.objects)
        self.hi = max(o['base'] + o['vsize'] for o in self.objects)

        fbase = le_off + h['frectab']
        fmap = struct.unpack_from(f"<{h['mpages'] + 1}I", self.data,
                                  le_off + h['fpagetab'])
        for pi, p in enumerate(self.pages):
            p['fx_start'] = fbase + fmap[pi]
            p['fx_end'] = fbase + fmap[pi + 1]

    def _page_bytes(self, p):
        d = self.h['datapage'] + (p['pageno'] - 1) * self.pagesize
        return self.data[d:d + self.pagesize]

    def load_image(self):
        img = bytearray(self.hi - self.lo)
        for p in self.pages:
            b = self._page_bytes(p)
            off = p['vaddr'] - self.lo
            img[off:off + len(b)] = b
        return img

    # ---- fixup record stream ---------------------------------------------------------
    def _records(self, p):
        off, end, D = p['fx_start'], p['fx_end'], self.data

        def rd(fmt):
            nonlocal off
            v = struct.unpack_from(fmt, D, off)[0]
            off += struct.calcsize(fmt)
            return v

        while off < end:
            src_flags, trg_flags = rd('<B'), rd('<B')
            if src_flags & F_SOURCE_LIST:
                cnt = rd('<B')
                srcs = [rd('<h') for _ in range(cnt)]
            else:
                src_off = rd('<h')
                srcs = None
            ordinal = rd('<H' if trg_flags & F_TARGET_ORD16 else '<B')
            ttype = trg_flags & F_TARGET_TYPE_MASK
            extra = None
            if ttype == TGT_INTERNAL:
                extra = rd('<I' if trg_flags & F_TARGET_OFF32 else '<H')
            elif ttype in (TGT_IMP_ORD, TGT_IMP_NAME):
                extra = (rd('<B') if trg_flags & F_TARGET_ORD8
                         else rd('<I') if trg_flags & F_TARGET_OFF32 else rd('<H'))
            addend = (rd('<I' if trg_flags & F_TARGET_ADD32 else '<H')
                      if trg_flags & F_TARGET_ADDITIVE else 0)
            if srcs is None and (trg_flags & F_TARGET_CHAIN):
                yield ('chain', src_flags, trg_flags, ordinal, extra, addend, p, src_off)
            elif srcs is not None:
                for s in srcs:
                    yield ('one', src_flags, trg_flags, ordinal, extra, addend, p, s)
            else:
                yield ('one', src_flags, trg_flags, ordinal, extra, addend, p, src_off)

    def apply_fixups(self, img):
        cnt = dict(off32=0, rel32=0, sel16off32=0, off16=0, sel16=0, byte=0,
                   chain=0, chain_bad=0, imports=0, skipped=0)
        sentinel = self.pagesize - 1
        for p in self.pages:
            if p['fx_start'] >= p['fx_end']:
                continue
            for kind, sf, tf, ordn, extra, add, pg, src in self._records(p):
                ftype, ttype = sf & F_SOURCE_TYPE_MASK, tf & F_TARGET_TYPE_MASK
                obj = self.objects[ordn - 1] if ordn - 1 < len(self.objects) else None
                if kind == 'chain':
                    if obj is None:
                        cnt['skipped'] += 1  # chain targets entry table
                        continue
                    so, guard = src, 0
                    while so not in (sentinel,) and so >= 0 and guard < self.pagesize:
                        guard += 1
                        va = pg['vaddr'] + so - self.lo
                        info = struct.unpack_from('<I', img, va)[0]
                        tv = obj['base'] + (info & 0xFFFFF)
                        struct.pack_into('<I', img, va, tv + add)
                        cnt['chain'] += 1
                        so = info >> 20
                        if so and guard >= self.pagesize:
                            cnt['chain_bad'] += 1
                    continue
                if ttype in (TGT_IMP_ORD, TGT_IMP_NAME):
                    cnt['imports'] += 1        # statically linked: none expected
                    continue
                if obj is None or (ttype not in (TGT_INTERNAL, TGT_INT_ENTRY)):
                    cnt['skipped'] += 1
                    continue
                tv = obj['base'] + (extra or 0)
                if src < 0:
                    cnt['skipped'] += 1        # cross-page dup already handled
                    continue
                va = pg['vaddr'] + src - self.lo
                if ftype == 7:                 # FIXUP_OFF32
                    struct.pack_into('<I', img, va, tv + add); cnt['off32'] += 1
                elif ftype == 8:               # FIXUP_REL32
                    struct.pack_into('<I', img, va, tv + add - (pg['vaddr'] + src + 4))
                    cnt['rel32'] += 1
                elif ftype == 6:               # FIXUP_SEL16_OFF32
                    struct.pack_into('<I', img, va, tv + add - obj['base'])
                    struct.pack_into('<H', img, va + 4, ordn); cnt['sel16off32'] += 1
                elif ftype == 3:               # FIXUP_SEL16_OFF16
                    struct.pack_into('<H', img, va, tv + add - obj['base'])
                    struct.pack_into('<H', img, va + 2, ordn); cnt['off16'] += 1
                elif ftype == 5:               # FIXUP_OFF16
                    struct.pack_into('<H', img, va, tv + add - obj['base'])
                    cnt['off16'] += 1
                elif ftype == 2:               # FIXUP_SEL16
                    struct.pack_into('<H', img, va, ordn); cnt['sel16'] += 1
                elif ftype == 0:
                    cnt['byte'] += 1           # rare, leave as-is
                else:
                    cnt['skipped'] += 1
        return cnt

    def names(self):
        out, p, D = [], self.le_off + self.h['restab'], self.data
        while p < len(D) and D[p]:
            ln = D[p]
            out.append(D[p + 1:p + 1 + ln].decode('latin1'))
            p += ln + 3
        return out


def write_elf32(img, le, path):
    lo, h = le.lo, le.h
    ehsz, phsz, phnum = 52, 32, len(le.objects)
    data_off = 0x1000  # keep p_offset == p_vaddr (mod pagesize): valid ELF
    ph = b''
    for o in le.objects:
        flags = 5 if o['flags'] & 4 else 6  # LE O_CODE -> r-x, else rw-
        ph += struct.pack('<IIIIIIII', 1, data_off + (o['base'] - lo),
                          o['base'], o['base'], o['vsize'], o['vsize'], flags, 0x1000)
    entry = le.objects[h['startobj'] - 1]['base'] + h['eip']
    ident = b'\x7fELF\x01\x01\x01\x00' + b'\0' * 8
    eh = ident + struct.pack('<HHIIIIIHHHHHH', 2, 3, 1, entry, ehsz, 0, 0,
                             ehsz, phsz, phnum, 0, 0, 0)
    open(path, 'wb').write(eh + ph + b'\0' * (data_off - ehsz - phsz * phnum) + bytes(img))


def main():
    src = sys.argv[1]
    pre = sys.argv[2] if len(sys.argv) > 2 else src.rsplit('.', 1)[0]
    le = LEFile(src)
    img = le.load_image()
    stats = le.apply_fixups(img)
    open(pre + '.img', 'wb').write(bytes(img))
    write_elf32(img, le, pre + '.elf')
    h = le.h
    info = dict(file=src, cpu=h['cpu'],
                entry=hex(le.objects[h['startobj'] - 1]['base'] + h['eip']),
                stack=hex(le.objects[h['stackobj'] - 1]['base'] + h['esp']),
                image=[hex(le.lo), hex(le.hi)], pages=len(le.pages),
                segments=[dict(base=hex(o['base']), vsize=hex(o['vsize']),
                               flags=hex(o['flags'])) for o in le.objects],
                resident_names=le.names(), fixups=stats)
    open(pre + '.json', 'w').write(json.dumps(info, indent=1))
    print(json.dumps(info, indent=1))


if __name__ == '__main__':
    main()
