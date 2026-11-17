#!/usr/bin/env python3
"""Indexed SQLite index over the original image, so RE searches stop being
grep-and-scan work: `python3 tools/re/leodb.py build` once, then indexed
lookups. Built from build/leo/leo.img (+ the Ghidra function list in
build/decompiled/all/index.csv) and stamped with its sha256; the DB
itself is a derived artifact and lives only under /tmp/opencode.

Tables
  meta(k,v)                     source sha256, ranges, row counts
  insn(va,len,mnem,ops,target)  one row per disassembled instruction
        .prov = 'code' inside .text, 'data' elsewhere (best-effort decode)
        .suspect = 1 when a linear sweep may have drifted across this row
                   (another instruction overlaps it) -> verify with `dis`
  xref(from,to,kind)            direct call/jmp/branch; indexed by `to`
  dword(va,value,aligned,seg)   EVERY byte offset of the image, indexed by
                   value: "who points at X", aligned or not, in one lookup
  imm(site,value,mnem,ops)      every trailing hex immediate of an
                   instruction (mov reg,0x...; push 0x...; cmp ...,0x...),
                   indexed by value: "which code loads this address" in one
                   lookup. This is what finds the `mov eax,0xcef30` form
                   that a dword scan cannot see.
  func(va,name)                 Ghidra entry points + names
  loadimm(site,disp,value,reg)  every `mov DWORD PTR [reg+DISP],imm` site,
                   indexed by value: "what code loads this address, into
                   which field" - that is how the ctor pattern
                   [obj+8]=T / [obj+0xc]=T-8 (the two views) is found
  mtable(va,n,src)              class tables: 'ctor' = proven by the ctor
                   pattern above, 'run' = maximal run of >=8 code pointers
                   in the data segment (skips the other view of a ctor pair)
  str(va,len,text)              printable runs >= 4 bytes in the image

Commands
  build                     (re)build the database
  callers 0xADDR [depth]    indexed reverse-call BFS
  refs 0xADDR               every pointer to it, aligned or not, + code refs
  callees 0xADDR
  dis 0xADDR [len]          EXACT disassembly at a VA (objdump on the
                            exact bytes - never trusts the index)
  func 0xADDR               enclosing function
  slots +0x60 [+0x64 ...]   what sits at those slots in all method tables
  tables 0xADDR             who builds/uses one class table (ctor sites,
                            code that mentions it, pointers to it, length)
  grep 'pattern'            instruction/operand search (indexed by mnem)
  sql 'select ...'          run any read-only-ish query against the schema
  schema                    dump the schema + indexes
  find 'name'               string search

CAVEATS (why this is a SEARCH index and not the answer):
 - the instruction rows come from ONE linear sweep of the image, which
   drifts wherever an entry point is mid-instruction. Overlapping rows
   are flagged suspect, and every listing command prints the flag, but
   the only authoritative disassembly is `dis` (objdump over the exact
   bytes at the exact VA). Same for `func`: body ranges are hints from
   the Ghidra entry list.
 - the class-table set is mechanical: `ctor` = proven by the two-view
   pattern found in code, `run` = maximal runs of >=8 code pointers.
   This build finds 158 ctor tables; the PR series' call-site-derived
   census measures 162. Slot counts are therefore counts OVER DETECTED
   TABLES (129/158 for +0x60=0x23b80 vs the record's 132/162) - close,
   not identical, and never quoted as a fact about the game.
 - raw dword rows include every byte offset, so a value can match inside
   an instruction (`aligned=0`): that was the 0x587d9 false lead. The
   alignment column exists so one lookup shows both kinds.
"""
import hashlib
import os
import re
import shutil
import sqlite3
import struct
import subprocess
import sys

# paths follow the tools/re convention (HANDOFF.md section 3): the repo is
# where this file is checked out, the generated DB lives in /tmp/opencode.
# Paths follow the tools/re convention (HANDOFF.md section 3): the image is
# read from the build/leo output of the checkout, the generated DB is a
# derived artifact and lives only under /tmp/opencode - never in git.
REPO = os.environ.get('LEO_REPO', '/home/aizy/Projects/LL_RECOMPILE')
IMG = os.path.join(REPO, 'build', 'leo', 'leo.img')
GHIDRA_IDX = os.path.join(REPO, 'build', 'decompiled', 'all', 'index.csv')
DB = os.environ.get('LEO_DB', '/tmp/opencode/leo.sqlite')
TEXT_LO, TEXT_HI = 0x10000, 0x82B51
SEG_LO, SEG_HI = 0x10000, 0xD76D0


def sha(path):
    h = hashlib.sha256()
    with open(path, 'rb') as f:
        for blk in iter(lambda: f.read(1 << 20), b''):
            h.update(blk)
    return h.hexdigest()


def run(cmd):
    return subprocess.run(cmd, capture_output=True, text=True).stdout


def build():
    data = open(IMG, 'rb').read()
    stamp = sha(IMG)
    if os.path.exists(DB):
        os.remove(DB)
    db = sqlite3.connect(DB)
    q = db
    q.execute('create table meta(k text primary key, v text)')
    q.execute('create table insn(va integer primary key, len integer,'
              ' mnem text, ops text, target integer, prov text,'
              ' suspect integer)')
    q.execute('create table xref(src integer, dst integer, kind text)')
    q.execute('create table dword(va integer, value integer, aligned integer,'
      ' seg text)')
    q.execute('create table func(va integer, name text)')
    q.execute('create table mtable(va integer, n integer, src text)')
    q.execute('create table str(va integer, len integer, text text)')

    q.execute("insert into meta values('img_sha256',?)", (stamp,))
    q.execute("insert into meta values('built',datetime('now'))")
    q.execute("insert into meta values('text','0x%x 0x%x')"
              % (TEXT_LO, TEXT_HI))

    # ---- instructions: one linear sweep of the whole image (the .img is
    # the flat image based at its own VA, so --adjust-vma is not needed).
    asm = run(['objdump', '-D', '-b', 'binary', '-m', 'i386',
               '--adjust-vma=0x10000', '-M', 'intel', IMG])
    ln = re.compile(r'^\s*([0-9a-f]+):\s*((?:[0-9a-f]{2} )+)\s*(.*)$')
    rows, irows, ncode, ndata = [], [], 0, 0
    branch = {'call', 'jmp', 'je', 'jne', 'jz', 'jnz', 'ja', 'jae', 'jb',
              'jbe', 'jg', 'jge', 'jl', 'jle', 'jc', 'jnc', 'js', 'jns',
              'jo', 'jno', 'jp', 'jnp', 'loop', 'loope', 'loopne',
              'jmp', 'bx'}
    trail = re.compile(r'(?:,| )((?:0x[0-9a-f]+)|(?:[0-9]{3,}))\s*$')
    for line in asm.split('\n'):
        m = ln.match(line)
        if not m:
            continue
        va = int(m.group(1), 16)
        by = m.group(2).split()
        body = m.group(3).strip()
        if '.' in body and not body[0].isalpha():
            mnem, ops = '.byte', body
        else:
            parts = body.split(None, 1)
            mnem = parts[0] if parts else ''
            ops = parts[1] if len(parts) > 1 else ''
        tgt = None
        if mnem in branch:
            t = re.search(r'0x[0-9a-f]+\s*$', ops)
            if t:
                tgt = int(t.group(0), 16)
        prov = 'code' if TEXT_LO <= va < TEXT_HI else 'data'
        if prov == 'code':
            ncode += 1
        else:
            ndata += 1
        rows.append((va, len(by), mnem, ops, tgt, prov, 0))
        t = trail.search(ops)
        if t and mnem not in branch:
            v = int(t.group(1), 16) if 'x' in t.group(1) else int(t.group(1))
            if SEG_LO <= v < SEG_HI:
                irows.append((va, v, mnem, ops))
    q.executemany('insert into insn values(?,?,?,?,?,?,?)', rows)
    q.execute('create table imm(site integer, value integer, mnem text,'
              ' ops text)')
    q.executemany('insert into imm values(?,?,?,?)', irows)

    # ---- drift marking: an instruction overlapped by another instruction
    # is not an entry point; anything reached only through such a row is a
    # best-effort decode and must be verified with `dis`.
    starts = {r[0] for r in rows}
    for va, ln_ in ((r[0], r[1]) for r in rows):
        for k in range(1, ln_):
            if va + k in starts:
                q.execute('update insn set suspect=1 where va=?', (va,))

    # ---- xrefs from direct branches
    q.execute('insert into xref select va, target, mnem from insn '
              'where target is not null')

    # ---- every dword at every byte offset, indexed by value
    n = len(data)
    drows = []
    for off in range(n - 3):
        v = struct.unpack_from('<I', data, off)[0]
        drows.append((0x10000 + off, v, 1 if off % 4 == 0 else 0,
                      'code' if TEXT_LO <= 0x10000 + off < TEXT_HI else
                      'data'))
    q.executemany('insert into dword values(?,?,?,?)', drows)

    # ---- Ghidra function entries (names match our VAs: FUN_0004ba90)
    if os.path.exists(GHIDRA_IDX):
        frows = []
        with open(GHIDRA_IDX) as fh:
            next(fh)
            for line in fh:
                p = line.rstrip('\n').split(',')
                if len(p) >= 2 and re.fullmatch(r'[0-9a-f]{8}', p[1]):
                    frows.append((int(p[1], 16), p[0]))
        q.executemany('insert into func values(?,?)', frows)

    # ---- the class-table detector the PR series actually uses, straight out
    # of the index: a ctor writes TWO views, [obj+8]=T and [obj+0xc]=T-8
    # (the second view's slot N is the first view's slot N+8), so any value
    # that appears as the immediate of a `mov DWORD PTR [reg+0x8],T` and 8
    # lower for +0xc IS a class table. VERIFIED at 0x1ed08:
    #   mov DWORD PTR [eax+0x8],0x96874 / mov DWORD PTR [eax+0xc],0x9686c
    q.execute('create table loadimm(site integer, disp integer,'
              ' value integer, reg text)')
    imm = re.compile(r'^DWORDPTR\[((?:e[a-z]{2}|b[a-z][a-z]|s[ip]|d[ip])'
                     r'(?:[-+]0x[0-9a-f]+)?)\],(0x[0-9a-f]+)$')
    lrows = []
    for va, _ln, mnem, ops, _t, _p, _s in rows:
        if mnem != 'mov':
            continue
        m = imm.match(ops.replace(' ', ''))
        if not m:
            continue
        reg = re.match(r'e[a-z]{2}|b[a-z][a-z]|s[ip]|d[ip]', m.group(1))
        dp = re.search(r'([-+])0x([0-9a-f]+)$', m.group(1))
        d_ = 0
        if dp:
            d_ = int(dp.group(2), 16) * (-1 if dp.group(1) == '-' else 1)
        lrows.append((va, d_, int(m.group(2), 16), reg.group(0)))
    q.executemany('insert into loadimm values(?,?,?,?)', lrows)
    cand = {r[2] for r in lrows if r[1] == 8} & \
           {r[2] + 8 for r in lrows if r[1] == 12}
    # One pattern site names BOTH views (T at +0x8 and T-8 at +0xc), so T-8
    # is usually in that set too. Keep the view the records number slots by:
    # a base is the SECOND view when base+8 is also ctor-named (verified at
    # 0x1ed08: 0x96874 is the first view, 0x9686c the second), because the
    # second view's slot N is the first view's slot N+8.
    cand = {va for va in cand if va + 8 not in cand}
    # (b) maximal runs of code pointers in the data segment (a fallback that
    # also finds tables no ctor names, but it cannot tell a table's two
    # views apart, so it is labelled separately).
    runs = {}
    for va in range(0x90000, SEG_HI - 4, 4):
        k = 0
        while k < 512 and va + k * 4 + 4 <= SEG_HI:
            v = struct.unpack_from('<I', data, va - 0x10000 + k * 4)[0]
            if not (TEXT_LO <= v < TEXT_HI):
                break
            k += 1
        if k < 8:
            continue
        prev = struct.unpack_from('<I', data, va - 0x10000 - 4)[0]
        if TEXT_LO <= prev < TEXT_HI:
            continue                    # not a maximal run: not a start
        runs[va] = k
    mrows = []
    for va in sorted(cand):
        k = runs.get(va)
        if k is None:
            k = 0
            while k < 512 and va + k * 4 + 4 <= SEG_HI:
                v = struct.unpack_from('<I', data, va - 0x10000 + k * 4)[0]
                if not (TEXT_LO <= v < TEXT_HI):
                    break
                k += 1
        mrows.append((va, k, 'ctor'))
    for va, k in sorted(runs.items()):
        if va - 8 in cand or va + 8 in cand:
            continue                    # the other view of a known table
        mrows.append((va, k, 'run'))
    q.executemany('insert into mtable values(?,?,?)', mrows)

    # ---- strings
    srows = []
    for m in re.finditer(rb'[\x20-\x7e]{4,}', data):
        srows.append((0x10000 + m.start(), m.end() - m.start(),
                      m.group().decode('latin1')))
    q.executemany('insert into str values(?,?,?)', srows)

    for stmt in ('create index ix_xref_dst on xref(dst,src)',
                 'create index ix_dw_val on dword(value,aligned,va)',
                 'create index ix_dw_va on dword(va)',
                 'create index ix_func_va on func(va)',
                 'create index ix_str on str(text)',
                 'create index ix_loadimm on loadimm(value,disp,site)',
                 'create index ix_imm on imm(value,site)',
                 'create index ix_mtable on mtable(va)',
                 'create index ix_insn_mnem on insn(mnem,va)'):
        q.execute(stmt)
    for t, c in (('insn', ncode + ndata), ('xref', None),
                 ('dword', n - 3), ('func', None), ('imm', len(irows)),
                 ('loadimm', len(lrows)), ('mtable', len(mrows)),
                 ('str', len(srows))):
        cnt = c if c is not None else q.execute(
            'select count(*) from ' + t).fetchone()[0]
        q.execute("insert into meta values(?,?)", ('rows_' + t, str(cnt)))
    db.commit()
    print('built %s from %s sha256=%s' % (DB, IMG, stamp[:16]))
    for k, v in q.execute("select k,v from meta where k like 'rows_%'"):
        print('  %-8s %s' % (k[5:], v))
    print('  method tables: %d (%s)'
          % (len(mrows), ', '.join('%s=%d' % r for r in
                                   q.execute('select src,count(*) from '
                                             'mtable group by src'))))


def db():
    d = sqlite3.connect(DB)
    d.row_factory = sqlite3.Row
    return d


def va_of(a):
    return int(a, 16) if a.lower().startswith('0x') else int(a, 0)


def dis(va, n=0x40):
    """Exact disassembly: objdump over the requested bytes only."""
    if not shutil.which('objdump'):
        print('objdump missing')
        return
    raw = open(IMG, 'rb').read()[va - 0x10000:va - 0x10000 + n]
    tmp = '/tmp/opencode/.leodb_slice.bin'
    open(tmp, 'wb').write(raw)
    out = run(['objdump', '-D', '-b', 'binary', '-m', 'i386',
               '--adjust-vma=0x%x' % va, '-M', 'intel', tmp])
    print('\n'.join(l for l in out.split('\n')
                    if re.match(r'^\s*[0-9a-f]+:', l)))


def callers(d, target, depth=4):
    seen = {target}
    level = [target]
    for dep in range(depth + 1):
        if not level:
            return
        q = ','.join('?' * len(level))
        rows = d.execute(
            'select distinct src, kind from xref where dst in (%s)' % q,
            level).fetchall()
        nxt = []
        for r in rows:
            site = d.execute('select mnem from insn where va=?',
                             (r['src'],)).fetchone()
            print('  d=%d 0x%05x %-6s in %s'
                  % (dep, r['src'], site['mnem'] if site else '?',
                     enclosing(d, r['src'])))
            if r['src'] not in seen:
                seen.add(r['src'])
                nxt.append(r['src'])
        level = nxt


def refs(d, target):
    print('-- pointers whose value is 0x%x (aligned first)' % target)
    for r in d.execute('select va,aligned,seg from dword where value=? '
                       'order by aligned desc, va limit 60', (target,)):
        print('  0x%05x aligned=%d seg=%s %s'
              % (r['va'], r['aligned'], r['seg'], enclosing(d, r['va'])))
    print('-- code that refers to it (direct call/jmp/branch targets)')
    for r in d.execute('select va,mnem,suspect from insn '
                       'where target=? order by va', (target,)):
        print('  0x%05x %s%s' % (r['va'], r['mnem'],
                                 '  [SUSPECT]' if r['suspect'] else ''))
    print('-- code whose immediate is it (mov reg,X / push X / lea ...)')
    for r in d.execute('select site,mnem,ops from imm where value=? '
                       'order by site limit 60', (target,)):
        print('  0x%05x %-8s %s' % (r['site'], r['mnem'], r['ops']))


def enclosing(d, va):
    """Best name for an address from the Ghidra entry list: the nearest
    entry at or below it, with the offset. The index never claims this is
    authoritative - `dis` is what decides what code is really there."""
    r = d.execute('select va,name from func where va<=? order by va desc '
                  'limit 1', (va,)).fetchone()
    if not r:
        return '(no entry at or below)'
    if r['va'] == va:
        return r['name']
    return '%s+0x%x' % (r['name'], va - r['va'])


def slots(d, offs, want='ctor'):
    """Slot census. Default `want='ctor'` = only tables PROVEN by the ctor
    pattern, which is the same set the PR series measures (the +0xc view is
    skipped, so slot numbering matches the records). Pass --all to include
    the run-detected extras; every number is a count over detected tables,
    not a fact about the game."""
    tab = d.execute('select va, n from mtable where src=? '
                    'order by n desc', (want,)).fetchall()
    print('class tables used: %d (src=%s)' % (len(tab), want))
    for o in offs:
        hist = {}
        for t in tab:
            v = d.execute('select value from dword where va=? and aligned=1',
                          (t['va'] + o,)).fetchone()
            if v:
                hist.setdefault(v['value'], []).append(t['va'])
        code = {v: ts for v, ts in hist.items() if TEXT_LO <= v < TEXT_HI}
        other = len(hist) - len(code)
        print('slot +0x%02x: %d tables hold a .text target, %d values '
              'outside .text (%d tables total)'
              % (o, sum(len(ts) for ts in code.values()), other, len(tab)))
        for v, ts in sorted(code.items(), key=lambda kv: -len(kv[1]))[:14]:
            print('   0x%05x  %-28s in %d tables   e.g. %s'
                  % (v, enclosing(d, v), len(ts),
                     ', '.join('0x%x' % t for t in ts[:3])))


def tables(d, arg):
    """Provenance of one class table: which code loads it, and where."""
    v = va_of(arg)
    print('-- code that stores this address into an object field')
    for r in d.execute('select site,disp,reg from loadimm where value=? '
                       'order by disp,site', (v,)):
        f = d.execute('select name from func where va<=? order by va desc '
                      'limit 1', (r['site'],)).fetchone()
        print('  0x%05x mov [%s+0x%x],0x%x   in %s'
              % (r['site'], r['reg'], r['disp'], v,
                 f['name'] if f else '?'))
    print('-- code that loads this address as an immediate')
    for r in d.execute('select site,mnem,ops from imm where value=? '
                       'order by site limit 40', (v,)):
        print('  0x%05x %-8s %s' % (r['site'], r['mnem'], r['ops']))
    print('-- pointers to it (aligned first)')
    for r in d.execute('select va,aligned from dword where value=? '
                       'order by aligned desc,va limit 20', (v,)):
        print('  0x%05x aligned=%d' % (r['va'], r['aligned']))
    n = d.execute('select n,src from mtable where va=?', (v,)).fetchone()
    if n:
        print('-- measured length: %d slots (detected by: %s)'
              % (n['n'], n['src']))
    else:
        print('-- not in the detected table set')
    print('-- counts by detector: %s'
          % dict(d.execute('select src,count(*) from mtable group by src')))


def frange(d, va):
    """Best body range for a function: from its entry (or the nearest one
    below) to the next entry, capped at the first ret. A HINT from the
    Ghidra entry list, not a fact - use `dis` for the bytes."""
    e = d.execute('select va from func where va<=? order by va desc '
                  'limit 1', (va,)).fetchone()
    if not e:
        return None, None
    start = e['va']
    nxt = d.execute('select va from func where va>? order by va limit 1',
                    (start,)).fetchone()
    end = nxt['va'] if nxt else TEXT_HI
    ret = d.execute("select va from insn where va>? and va<? and "
                    "mnem in ('ret','iret') order by va limit 1",
                    (start, end)).fetchone()
    if ret:
        end = min(end, ret['va'] + 1)
    return start, end


def main():
    if len(sys.argv) < 2 or sys.argv[1] == 'build':
        build()
        return
    d = db()
    cmd = sys.argv[1]
    if cmd == 'callers':
        callers(d, va_of(sys.argv[2]), int(sys.argv[3]) if len(sys.argv) > 3
                else 4)
    elif cmd == 'refs':
        refs(d, va_of(sys.argv[2]))
    elif cmd == 'callees':
        v = va_of(sys.argv[2])
        lo, hi = frange(d, v)
        if lo is None:
            print('no function entry at or below 0x%x' % v)
        else:
            print('-- body 0x%05x..0x%05x (hint from the entry list)'
                  % (lo, hi))
            for r in d.execute('select va,target,mnem from insn where '
                               'va>=? and va<? and target is not null '
                               'order by va', (lo, hi)):
                print('  0x%05x %s -> 0x%05x %s'
                      % (r['va'], r['mnem'], r['target'],
                         enclosing(d, r['target'])))
    elif cmd == 'dis':
        dis(va_of(sys.argv[2]),
            int(sys.argv[3], 0) if len(sys.argv) > 3 else 0x40)
    elif cmd == 'func':
        v = va_of(sys.argv[2])
        lo, hi = frange(d, v)
        if lo is None:
            print('no function entry at or below 0x%x' % v)
        else:
            print('0x%05x %s   body ends by 0x%05x '
                  '(entry-list hint; use `dis` for the bytes)'
                  % (lo, enclosing(d, lo), hi))
            for i in d.execute('select va,mnem,ops,suspect from insn '
                               'where va>=? and va<? order by va', (lo, hi)):
                print('   0x%05x %-8s %s%s' % (i['va'], i['mnem'], i['ops'],
                                               '  [SUSPECT]' if i['suspect']
                                               else ''))
    elif cmd == 'slots':
        args = [a for a in sys.argv[2:] if not a.startswith('--')]
        want = 'ctor'
        for a in sys.argv[2:]:
            if a == '--all':
                want = 'run'      # extras only; see --tables for the totals
            if a == '--any':
                want = 'run'
        slots(d, [int(a, 16) for a in args], want)
    elif cmd == 'tables':
        tables(d, sys.argv[2])
    elif cmd == 'find':
        pat = sys.argv[2]
        for r in d.execute('select va,len,text from str where text like ? '
                           'limit 40', ('%' + pat + '%',)):
            print('0x%05x %r' % (r['va'], r['text']))
    elif cmd == 'grep':
        pat = sys.argv[2]
        # `mnem<space>rest` uses the (mnem,va) index; anything else is a
        # full scan of 201k instruction rows, which is fine but slow.
        head = pat.split(None, 1)
        if len(head) == 2 and re.fullmatch(r'[a-z.$]+', head[0]):
            cur = d.execute('select va,mnem,ops,suspect from insn where '
                            'mnem=? and ops like ? limit 60',
                            (head[0], '%' + head[1] + '%'))
        else:
            print('   (no mnemonic prefix: full scan)')
            cur = d.execute("select va,mnem,ops,suspect from insn where "
                            "mnem||' '||ops like ? limit 60",
                            ('%' + pat + '%',))
        for r in cur:
            print('0x%05x %-8s %s%s' % (r['va'], r['mnem'], r['ops'],
                                        '  [SUSPECT]' if r['suspect'] else ''))
    elif cmd == 'sql':
        cur = d.execute(sys.argv[2])
        if cur.description:
            print(' | '.join(c[0] for c in cur.description))
        for r in cur:
            print(' | '.join(
                '%x' % v if isinstance(v, int) and v > 0xFFFF else str(v)
                for v in r))
    elif cmd == 'schema':
        for r in d.execute("select sql from sqlite_master where sql is not "
                           "null order by type desc, name"):
            print(r[0] + ';\n')
    else:
        print(__doc__)


main()
