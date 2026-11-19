#!/usr/bin/env python3
"""doc_census.py - THE ENGINE-DOCUMENTATION STRUCTURE PIN (PR-195).

WHY. The owner's documentation audit (2026-11-19) measured the doc set
and found that the only structural checks in this repo were two greps
run by hand: a heading census that is a PREFIX PROXY (`grep -c
'^## PR-1'` sees 107 of the 181 records in NEXT_STEPS.md) and a frozen
set of over-79-column lines. Neither one can see a record heading
disappear, a title stop matching the record anchor, or the plan section
be duplicated. This tool turns those hand-checks into a build leg. It
reads nothing but the tracked documents: no fixtures, no data, no
window, so it runs in both fixture modes like stage_selftest.

WHAT IT PINS - any of these fails the build:
  P1 record headings. A record starts with a `## PR-<n>` heading
     followed by a space or `(` (PR-41/42/77 do not use a plain dash
     title, so no title form may be demanded). The count must not FALL
     below RECORD_MIN (records are appended, never rewritten away), and
     every PR number up to the highest one must either have a record or
     be in MISSING_OK - the gaps that history already accepted, frozen
     here so a NEW gap (a heading that went missing or stopped matching
     the anchor) is loud.
  P2 the plan/rule sections exist exactly once: `## Next`,
     `## Deferred big items`, `## Standing rules` in NEXT_STEPS.md.
  P3 the width ratchet. Over-79-column lines per tracked document may
     not exceed WIDTH_MAX. Lowering one is always welcome; RAISING one
     needs a commit that says why. The current numbers are what the
     2026-11-19 audit measured (41 of HANDOFF's are section 6 table
     rows, 22 of NEXT_STEPS' are wrapped record titles).

WHAT IT ONLY REPORTS (no rule attached yet, they are the audit's
measurement points so a future PR can show a change): wrapped-title
`##` continuation lines, distinct addresses and mentions, `PR-nn`
citations, `[field+0xNN]` tokens, authored-name tokens.

usage: python3 tools/doc_census.py [FILE ...]   (default: the tracked
docs). Exit 0 + one "(ok)" line, or exit 1 with every FAILED reason.
"""
import collections
import io
import re
import sys

DOCS = ('NEXT_STEPS.md', 'HANDOFF.md', 'README.md')

# P1: measured 2026-11-19 by this tool. 184 record headings = 183
# distinct PR numbers (PR-73 has two records, which is correct history:
# two separate steps were shipped that day). The anchor is a `## PR-<n>`
# heading followed by a space or `(`: PR-41/PR-42 are `## PR-41
# (SHELVED 2026-11-16) ...` and PR-77 is `## PR-77 investigation ...`,
# so neither the dash nor a fixed title form may be required. Lines
# like `## PR-138/141/142 precedent)` are wrapped-title continuations,
# and they are rejected because a `/` follows the number.
RECORD_MIN = 184
# PR numbers up to the highest one that have NO record heading. They are
# frozen as accepted history: PR-1..PR-6 predate the record convention,
# PR-22/23/24 are described inside the old status block, PR-80 is
# written inside PR-79's record, PR-113 has no record at all.
MISSING_OK = frozenset(list(range(1, 7)) + [22, 23, 24, 80, 113])

# P3: the width ratchet, exactly as the 2026-11-19 audit measured it.
WIDTH_MAX = {'NEXT_STEPS.md': 71, 'HANDOFF.md': 71, 'README.md': 12}

RECORD_RE = re.compile(r'^## PR-(\d+)[\s(]')
OWNER_RE = re.compile(r'^## OWNER')
HEAD_RE = re.compile(r'^## ')
SECTION_RE = {
    '## Next': re.compile(r'^## Next'),
    '## Deferred big items': re.compile(r'^## Deferred big items'),
    '## Standing rules': re.compile(r'^## Standing rules'),
}
ADDR_RE = re.compile(r'0x[0-9a-fA-F]{4,6}')
PRCITE_RE = re.compile(r'\bPR-\d+\b')
FIELD_RE = re.compile(r'\[[a-z]+(?:_[a-z]+)?\+0x[0-9a-f]{1,3}\]')
NAME_RE = re.compile(r'\b(?:A_[A-Z0-9_]{3,}|AC_[A-Z0-9_]{2,}|'
                     r'K_[A-Z0-9_]{2,}|[a-z_]{4,20}=)\b')


def read(path):
    try:
        return io.open(path, encoding='utf-8', errors='replace').read()
    except OSError as e:
        print('docs structure: FAILED cannot read %s (%s)' % (path, e))
        return None


def main(argv):
    files = argv[1:] or list(DOCS)
    bad = []
    ns_path = 'NEXT_STEPS.md'
    text = {}
    for f in files:
        t = read(f)
        if t is None:
            bad.append('unreadable ' + f)
        else:
            text[f] = t

    # ---- P1 record headings -----------------------------------------
    ids = []
    owner = 0
    heads = 0
    for f, t in text.items():
        for line in t.split('\n'):
            if HEAD_RE.match(line):
                heads += 1
            m = RECORD_RE.match(line)
            if m:
                ids.append(int(m.group(1)))
            elif OWNER_RE.match(line):
                owner += 1
    uniq = set(ids)
    top = max(uniq) if uniq else 0
    print('docs structure: records=%d (%d distinct PR numbers, up to '
          'PR-%d) + %d OWNER blocks; %d "## " lines across the files '
          'read (wrapped-title continuations are reported below)'
          % (len(ids), len(uniq), top, owner, heads))
    if len(ids) < RECORD_MIN and ns_path in text:
        bad.append('only %d record headings, expected at least %d: a '
                   'record went missing or stopped matching '
                   '"## PR-<n> -"' % (len(ids), RECORD_MIN))
    if top:
        gaps = {n for n in range(1, top + 1) if n not in uniq}
        new = sorted(gaps - set(MISSING_OK))
        if new:
            bad.append('PR numbers with no record heading, NOT in the '
                       'accepted-gap list: %s'
                       % ', '.join('PR-%d' % n for n in new))
        dups = [n for n, c in collections.Counter(ids).items()
                if c > 1 and n != 73]
        if dups:
            print('docs structure: NOTE repeated record headings: %s'
                  % ', '.join('PR-%d x%d' % (n, collections.Counter(
                      ids)[n]) for n in sorted(dups)))

    # ---- P2 the plan and rule sections exist exactly once ----------
    if ns_path in text:
        for label, rx in SECTION_RE.items():
            n = sum(1 for l in text[ns_path].split('\n') if rx.match(l))
            if n != 1:
                bad.append('%s appears %d times in %s, expected exactly '
                           '1' % (label, n, ns_path))
        print('docs structure: plan/rule sections present once (%s)'
              % ', '.join(sorted(SECTION_RE)))

    # ---- P3 the width ratchet --------------------------------------
    parts = []
    for f in sorted(text):
        over = [l for l in text[f].split('\n') if len(l) > 79]
        limit = WIDTH_MAX.get(f, 0)
        parts.append('%s %d/%d' % (f, len(over), limit))
        if len(over) > limit:
            worst = max(over, key=len)
            bad.append('%s has %d lines over 79 columns, the ratchet is '
                       '%d (longest now %d columns: %r)'
                       % (f, len(over), limit, len(worst), worst[:60]))
    print('docs structure: over-79-column lines (count/ratchet): %s'
          % ', '.join(parts))

    # ---- report only ----------------------------------------------
    ns = text.get(ns_path, '')
    addrs = ADDR_RE.findall(ns)
    fields = FIELD_RE.findall(ns)
    names = NAME_RE.findall(ns)
    cites = PRCITE_RE.findall(ns)
    cont = 0
    if ns:
        prev = False
        for l in ns.split('\n'):
            if RECORD_RE.match(l) or OWNER_RE.match(l):
                prev = True
            elif HEAD_RE.match(l) and prev:
                cont += 1
            else:
                prev = False
    print('docs structure: report only (the audit\'s measures, no rule): '
          'addresses %d distinct/%d mentions, PR-nn citations %d, '
          'field tokens %d distinct/%d mentions, name tokens %d '
          'distinct, wrapped-title "##" lines %d'
          % (len(set(addrs)), len(addrs), len(cites), len(set(fields)),
             len(fields), len(set(names)), cont))

    if bad:
        for b in bad:
            print('docs structure: FAILED %s' % b)
        return 1
    print('docs structure: record headings, plan sections and the '
          'width ratchet hold (ok)')
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
