#!/usr/bin/env python3
"""re_index.py - the generated engine index (PR-197, doc-audit step 2).

WHAT THIS IS. The owner's documentation audit (2026-11-19, carried into
the PR-195/PR-196 records) measured that the ledger has no index: an
address like 0xcef40 appears on 115 lines spread over dozens of records and
nothing in the file says which claim is still current, and 3,022 of 3,371
PR-nn citations
carry no address/field/name token. This tool builds the missing landing
page: it reads NEXT_STEPS.md and writes a generated markdown index,
docs/RE_INDEX.md, one entry per engine key giving the records that
mention it and, where the ledger writes one, a pointer to a correction.

IT IS GENERATED, SO IT IS PINNED, NOT EDITED. `make verify-index` runs
this tool and `cmp`s its output against the tracked file; any different
byte fails the build (the index cannot drift from the ledger). That is
only possible because the output is DETERMINISTIC: every list is sorted
by PR number, every entry is sorted by (record-count desc, key), and no
timestamp or set-iteration order is emitted. It reads only tracked
markdown (no fixtures, no window), so it runs in both fixture modes.

WHAT IT INDEXES, AND WHY THESE THREE VOCABULARIES. They are exactly the
things a reader greps for, measured by the audit:
  - engine ADDRESSES (`0x...`) and FIELD tokens (`[reg+0xNN]`): listed
    when they appear in at least MIN_RECORDS distinct records. The long
    tail (an address in one or two records) is a read-once detail; the
    >=3 set is what the audit showed carries the lookups.
  - authored NAMES (`A_*`, `AC_*`, `K_*`, `.des` keys like `control=`):
    listed in FULL, with no count floor. A name is a word the engine
    author chose; even a single-record name is something someone will
    search for, and the whole vocabulary is only a few hundred rows.
    The `word=` branch is a PATTERN, not a verified dictionary: any
    `word=` token can appear (measured: `access=basic` in the ledger
    is git-credential config, not a `.des` key). The entry answers
    "which records name this token", never "the engine author wrote
"
    "    it"; the record list is what the reader judges from.

THE COUNTS ARE HONEST AND RECORDS-SCOPED. The audit's headline numbers
(729 addresses in >=3 records, etc.) came from its own census, which
cut records at ANY line starting `## PR-`, including the wrapped-title
CONTINUATION lines of multi-line titles - phantom records that steal
text from their real parent and split one record's addresses across two
fake ones. This tool imports doc_census and reuses its regexes and its STRICT
boundary (a `## PR-<n>` heading preceded by a blank line, ending at
the next real heading) - exactly the record-slice P4 uses. The
honest number difference is measured, not hidden: P1 REPORTS
heading-SHAPED lines (187 at PR-197) because its job is presence
and gaps by PR number, while this tool counts only REAL headings
(182) - the 5-line gap is five old titles that repeat their own
number on a continuation line (PR-29/30/31/32/116), listed in the
PR-197 record; on the SET of PR numbers the two tools agree
exactly. The audit's 189 segments are the same 182 real records
plus 8 such title wraps (the 3 beyond P1's 5 use `## PR-138..149`
and `## PR-159/160` forms its anchor rejects). Every number here
is in the PR-197 record, so the index and the audit are
reconciled, not silently replaced.

RETRACTION: A POINTER TO WHERE A CORRECTION IS WRITTEN, NOT A VERDICT.
The audit's sharpest complaint was that `grep 0x4be20` finds the
mentions but never the sentence that KILLED the claim. The ledger does
write such sentences, in one detectable form: a physical line carrying
the address token AND a correction word, e.g. `[PR-193 RETRACTION:
0x4be20 ...]`. So an entry can get a `corrected in:` line. Measured,
that signal is PRECISE (every flagged line is a genuine correction) but
SPARSE and DIRECTION-AGNOSTIC: only a few percent of address entries
have such a line, it cannot say whether the key is the wrong claim or
its replacement, and a `[PR-193 ...]` bracket pasted into PR-192 names
PR-192 (where the text sits). The index therefore does NOT invent a
current/obsolete verdict for the rest: it gives the record list and the
highest-numbered mention as a reading pointer, and its own header says
what `corrected in:` is NOT. Over-claiming a status the text does not
state would be the drift this whole line of work is trying to remove.

usage: python3 tools/re_index.py [OUT.md]   (default: stdout). Exit 0;
prints the measured counts on stderr. Exits 1 if the generated text has
any line over 79 columns - the index is held to the width rule.
"""
import collections
import io
import os
import re
import sys

# Reuse doc_census's vocabulary regexes and STRICT record boundary, so
# the index and `make verify-docs` P1 cannot drift apart - that shared
# definition of "a record" is why the two tools count the same records.
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import doc_census as dc  # noqa: E402

MIN_RECORDS = 3

# The width the generated index is held to. One constant, used by
# the line-wrap budget, the guard, and the guard's own message, so a
# test that lowers it changes all three together (the earlier control
# lowered the budget but not the message and so printed a confusing
# 'over 79' for a 74-column line).
WIDTH_MAX = 79

# The correction vocabulary the ledger actually uses, MEASURED. Plain
# negation (NOT on 412 lines) is EXCLUDED: it would flag almost any
# address as "corrected", noise dressed as signal.
CORRECT_RE = re.compile(
    r'\b(CORRECT(?:ED|S|ION|ING)?|SUPERSEDE\w*|RETRACT\w*|OVERRULE\w*|'
    r'FALSIF\w*|MISREAD|MISLABEL\w*|WRONG)\b')


def records(lines):
    """STRICT records: (pr_number, body_text, body_lines) in file order,
    using doc_census's heading rules (a `## PR-<n>` anchor preceded by a
    blank line; the body ends at the next such real heading)."""
    heads = {i for i in range(len(lines))
             if dc.HEAD_RE.match(lines[i])
             and (i == 0 or lines[i - 1].strip() == '')}
    starts = []
    for i in sorted(heads):
        m = dc.RECORD_RE.match(lines[i])
        if m:
            starts.append((i, int(m.group(1))))
    out = []
    for i, pr in starts:
        end = len(lines)
        for j in range(i + 1, len(lines)):
            if j in heads:
                end = j
                break
        out.append((pr, '\n'.join(lines[i:end]), lines[i:end]))
    return out


def build_index(recs):
    """key -> [mention_pr_set, corrected_pr_set], from a parsed record
    list. Address keys are lower-cased so 0x4BE20 and 0x4be20 merge
    (the ledger writes the same address in both cases)."""
    addr = collections.defaultdict(lambda: [set(), set()])
    field = collections.defaultdict(lambda: [set(), set()])
    name = collections.defaultdict(lambda: [set(), set()])
    for pr, body, block in recs:
        for key in set(dc.ADDR_RE.findall(body)):
            addr[key.lower()][0].add(pr)
        for key in set(dc.FIELD_RE.findall(body)):
            field[key][0].add(pr)
        keys = set(dc.NAME_RE.findall(body))
        keys |= set(dc.DES_RE.findall(body))
        for key in keys:
            name[key][0].add(pr)
        # correction: an address token sharing one physical line with a
        # correction word is a correction the ledger states in place.
        for line in block:
            if CORRECT_RE.search(line):
                for key in dc.ADDR_RE.findall(line):
                    addr[key.lower()][1].add(pr)
    return addr, field, name


def wrap(tokens, width):
    """Pack 'PR-n' tokens into content runs no wider than `width`; the
    caller passes the width that fits after its own prefix so the
    emitted line stays within 79 columns."""
    out, cur = [], ''
    for t in tokens:
        if cur and len(cur) + 1 + len(t) > width:
            out.append(cur)
            cur = t
        else:
            cur = (cur + ' ' + t) if cur else t
    if cur:
        out.append(cur)
    return out


def prlist(prs):
    return ['PR-%d' % p for p in sorted(prs)]


def emit_wrapped(out, label, toks):
    """Emit `<indent><label>: <toks...>`, continuing on lines aligned to
    the label so every emitted line stays within 79 columns."""
    pre = '        %s: ' % label
    body = wrap(toks, WIDTH_MAX - len(pre))
    out.append(pre + body[0])
    for ln in body[1:]:
        out.append(' ' * len(pre) + ln)


def entry(out, key, recs):
    """One entry: a `### key - N records` heading, a `mentions:` line,
    an optional `corrected in:` pointer, and a `latest mention:` reading
    pointer (not a currency verdict)."""
    mset, cset = recs
    cnt = len(mset)
    head = '### %s - %d record%s' % (key, cnt, '' if cnt == 1 else 's')
    if len(head) > 79:                      # an over-long name key
        head = '### %s' % key
    out.append(head)
    emit_wrapped(out, 'mentions', prlist(mset))
    if cset:
        emit_wrapped(out, 'corrected in', prlist(cset))
    out.append('        latest mention: PR-%d' % max(mset))
    out.append('')


HEADER = """# Engine index - GENERATED by tools/re_index.py; do not edit.

This file is generated from NEXT_STEPS.md. `make verify-index`
regenerates and `cmp`s it; `python3 tools/re_index.py docs/RE_INDEX.md`
writes it. Editing this file by hand makes the cmp fail, which is the
point: the index must track the ledger, not diverge from it.

WHAT EACH ENTRY IS. A key a reader greps for, the records that mention
it, and - only where the LEDGER writes one - a pointer to a correction
of it. `mentions:` lists every record containing the key (the largest:
{exkey}, on {exlines} raw lines, in {exrecs} records - its entry is the
first one below). `corrected
in:` lists records where that key shares a PHYSICAL LINE with a
correction word (the ledger's own correction form, e.g. `[PR-193
RETRACTION: 0x4be20 ...]`). `latest mention:` is the highest-numbered
record, a starting point for reading, not a verdict.

WHAT `corrected in:` IS NOT. It points to where a correction is
WRITTEN; it is never a census of corrections nor a currency verdict:
  - it names the HOST record holding the text. A `[PR-193 RETRACTION:
    ...]` bracket pasted into PR-192 names PR-192, because that is
    where the sentence sits - follow the PR-nn written inside it.
  - it fires only when key and correction word share one line, so it
    MISSES retractions not phrased that way. Absence is NOT evidence a
    claim stands: 0x4be20 IS retracted, yet only its bracket line is
    flagged, not the retraction body that killed it.
  - it is DIRECTION-AGNOSTIC: it will not say whether the key is the
    wrong claim or its replacement; read the line.
Measured, this is precise (every flagged line is a real correction) but
sparse: {ncorrect} of {naddr} address entries carry one.

HONEST LIMITS. Addresses and field tokens appear only if they are in at
least {minrec} records; the one-or-two-record tail is a read-once
detail, not something to index. Names (A_*, AC_*, K_*, `word=` keys)
are indexed in full, and the `word=` list is a PATTERN'S WORK, not a
verified `.des` dictionary: `access=basic` (a git-credential setting
in the ledger) matches it too. An entry says which records NAME a
token, never that the engine author WROTE it. A key with NO
`corrected in:` line is NOT asserted
current - the ledger simply never wrote a correction for it; deciding
whether an uncorrected claim still stands means reading its records.
"""


def section(out, title, dic, floor):
    out.append('\n## %s\n' % title)
    for k, v in sorted(dic.items(), key=lambda kv: (-len(kv[1][0]), kv[0])):
        if len(v[0]) >= floor:
            entry(out, k, v)


def _count_ge(dic, floor):
    return len([k for k in dic if len(dic[k][0]) >= floor])


def _coverage(dic, floor):
    tot = sum(len(v[0]) for v in dic.values())
    cov = sum(len(v[0]) for v in dic.values() if len(v[0]) >= floor)
    return cov, tot


def generate(lines):
    recs = records(lines)
    addr, field, name = build_index(recs)
    naddr = _count_ge(addr, MIN_RECORDS)
    nfield = _count_ge(field, MIN_RECORDS)
    ncorrect = len([k for k in addr if addr[k][1]
                    and len(addr[k][0]) >= MIN_RECORDS])
    cov_a, tot_a = _coverage(addr, MIN_RECORDS)
    cov_f, tot_f = _coverage(field, MIN_RECORDS)
    ex = max(addr.items(), key=lambda kv: (len(kv[1][0]), kv[0]))
    exlines = sum(1 for l in lines if ex[0] in l.lower())
    out = [HEADER.format(minrec=MIN_RECORDS, ncorrect=ncorrect,
                         naddr=naddr, exkey=ex[0], exlines=exlines,
                         exrecs=len(ex[1][0]))]
    out.append('# entries: %d address keys (>=%d records) = %d/%d (%d%%)'
               % (naddr, MIN_RECORDS, cov_a, tot_a,
                  100 * cov_a // max(1, tot_a)))
    out.append('# of all address mentions; %d field tokens (>=%d) = %d%%'
               % (nfield, MIN_RECORDS, 100 * cov_f // max(1, tot_f)))
    out.append('# of field mentions; %d authored names (all of them).'
               ' Built from %d records.\n' % (len(name), len(recs)))
    section(out, 'Addresses', addr, MIN_RECORDS)
    section(out, 'Field tokens', field, MIN_RECORDS)
    section(out, 'Authored names', name, 0)
    stats = {'records': len(recs), 'naddr': naddr, 'nfield': nfield,
             'names': len(name), 'ncorrect': ncorrect,
             'cov_a': (cov_a, tot_a), 'cov_f': (cov_f, tot_f)}
    return '\n'.join(out).rstrip('\n') + '\n', stats


def main(argv):
    lines = io.open('NEXT_STEPS.md', encoding='utf-8',
                    errors='replace').read().split('\n')
    text, st = generate(lines)

    over = [l for l in text.split('\n') if len(l) > WIDTH_MAX]
    if over:
        print('re_index: FAILED the generated index has %d lines over '
              '%d columns (longest %d): %r'
              % (len(over), WIDTH_MAX, len(max(over, key=len)),
                 max(over, key=len)[:60]), file=sys.stderr)
        return 1

    print('re_index: %d records; %d addresses (>=%d recs, %d%% of addr '
          'mentions), %d field tokens (>=%d, %d%%), %d names (all); %d '
          'address entries carry a correction pointer'
          % (st['records'], st['naddr'], MIN_RECORDS,
             100 * st['cov_a'][0] // max(1, st['cov_a'][1]),
             st['nfield'], MIN_RECORDS,
             100 * st['cov_f'][0] // max(1, st['cov_f'][1]),
             st['names'], st['ncorrect']), file=sys.stderr)

    if len(argv) > 1:
        d = os.path.dirname(argv[1])
        if d:
            os.makedirs(d, exist_ok=True)
        io.open(argv[1], 'w', encoding='utf-8', newline='').write(text)
    else:
        sys.stdout.write(text)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
