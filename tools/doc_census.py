#!/usr/bin/env python3
"""doc_census.py - THE ENGINE-DOCUMENTATION STRUCTURE PIN (PR-195;
   P4 added by PR-196, the record template).

WHY. The owner's documentation audit (2026-11-19) measured the doc set
and found that the only structural checks in this repo were two greps
run by hand: a heading census that is a PREFIX PROXY (`grep -c
'^## PR-1'` sees 107 of the 184 records in NEXT_STEPS.md) and a frozen
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
  P4 the record template (PR-196, HANDOFF section 10a). A record whose
     number is >= TEMPLATE_FROM must carry the field block - the eleven
     lines (ten fields, the gate in two lines) each present once, at
     line start, in the agreed order. Only FIELDS_EXEMPT is exempt of
     the requirement (it may still use the format - PR-196 carries its
     two gate lines - and is reported as a NOTE); no other record may
     carry a partial block. From the next record on,
     the template cannot decay back into prose: this is what makes it
     a contract. Position is NOT tested ("immediately after the title"
     is section 10a's style, not a rule); presence, order and
     exactly-one-each are.

   P5 the .md INVENTORY (PR-200, doc-work step 6). The set of TRACKED
      .md files must equal the declared document standard, exactly:
      the four living root docs, the generated index, the directory
      and tool READMEs, and files under docs/archive/ (frozen
      history, see docs/archive/README.md). A new root-level .md -
      the exact failure mode this repo showed on 2026-11-19: eight
      superseded work orders and reports sitting in the root, four
      thousand lines no tool could see - fails the build HERE, with
      the message saying where content belongs instead. A REQUIRED
      file going missing fails it too. The rule deliberately covers
      tracked files only: vendored material (vendor/SDL2's docs) is
      not ours to standardize and is not tracked.
   report-only: the archive count, so drift in the frozen set shows.

WHAT IT ONLY REPORTS (no rule attached yet, they are the audit's
measurement points so a future PR can show a change): wrapped-title
`##` continuation lines, distinct addresses and mentions, `PR-nn`
citations, `[field+0xNN]` tokens, authored-name tokens.

usage: python3 tools/doc_census.py [FILE ...]   (default: the tracked
docs). Exit 0 + one "(ok)" line, or exit 1 with every FAILED reason.
P5 runs only in default mode (it measures the repo, not the files
handed to it).
"""
import collections
import io
import re
import sys

DOCS = ('NEXT_STEPS.md', 'HANDOFF.md', 'README.md')

# P5: the .md standard (PR-200). The four root docs: README is the
# entry, NEXT_STEPS the journal, HANDOFF the current map, PLAN the
# RE-facts reference the CODE cites ("PLAN 8b" / "PLAN 8g" appear in
# ported/ comments, so it is living, not archive). The archive is a
# prefix rule because adding frozen history is a normal act - listing
# every file by hand would make the check nag about appends while
# staying blind about real additions elsewhere.
ALLOW_ROOT = {'README.md', 'NEXT_STEPS.md', 'HANDOFF.md', 'PLAN.md'}
ALLOW_GEN = {'docs/RE_INDEX.md'}
ALLOW_ARCHIVE = 'docs/archive/'
ALLOW_DIR = {'original/README.md', 'ported/README.md'}
ALLOW_TOOL_RE = re.compile(r'^tools/[^/]+/README\.md$')
P5_REQUIRED = sorted(ALLOW_ROOT | ALLOW_GEN | ALLOW_DIR |
                    {'docs/archive/README.md'})

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

# P4: the record template (PR-196), HANDOFF section 10a. It applies to
# every record whose number is TEMPLATE_FROM or higher. The one
# exemption is PR-196 itself: the record that WRITES this contract.
# The exemption skips the BLOCK requirement - that record does carry
# its two gate lines in the new format, which is legal for an exempt
# record and reported as a NOTE - and no other record may carry a
# partial block. The number is a constant on purpose - bumping
# TEMPLATE_FROM would silently let records skip the block again.
TEMPLATE_FROM = 196
FIELDS_EXEMPT = frozenset([196])
# The field lines, in order (ten fields, the gate in two lines =
# eleven lines), matched at line start only (a wrapped
# record title cannot produce a false hit: its lines never start with
# these names at column 0). The names are exact; prose variants are
# what the audit measured (86 gate-marker lines in 4 forms: `- GATES.`
# / `- GATES:` / `- GATES (` / `- TESTS AND GATES`, 12 distinct count
# claims) - so the field name is checked as a literal, not a stem. The
# gate line's wording is short on purpose and split in two: measured,
# one line carrying BOTH gate halves with their counts is 107 columns
# (control N7: merged, it fails P3 AND stops being the GATE-FREE
# field), so `GATE:` is the with-fixtures half and
# `GATE-FIXTURE-FREE:` the fixture-free half (section 2 rule 4
# requires both, so both are fields). A record may continue either
# line with wrapped prose. The counts are the point of
# the format - the audit measured 12 distinct count claims floating
# around 86 prose gate markers, which is where the 18/19/20 drift
# came from.
FIELDS = (
    ('KIND', re.compile(r'^KIND: (read|port|gate|docs) *$')),
    ('SUBSYSTEM', re.compile(r'^SUBSYSTEM: \S')),
    ('ADDRESSES', re.compile(r'^ADDRESSES: [a-z0-9.,;+ /\[\]x_-]+')),
    ('NAMES', re.compile(r'^NAMES: [A-Za-z0-9.,;+ /()\[\]_-]+')),
    ('CODE', re.compile(r'^CODE: (no|yes): \S')),
    ('HOOKS', re.compile(r'^HOOKS: \S')),
    ('OPENS', re.compile(r'^OPENS: [A-Za-z0-9,;+ ._-]+')),
    ('CLOSES', re.compile(r'^CLOSES: [A-Za-z0-9,;+ ._-]+')),
    ('CORRECTS', re.compile(r'^CORRECTS: (none|PR-\d+)')),
    ('GATE', re.compile(r'^GATE: make 0 warnings; verify-all fixtures '
                        r'rc=0 \d+ \(ok\);')),
    ('GATE-FREE', re.compile(r'^GATE-FIXTURE-FREE: rc=0 \d+ \(ok\) \+ '
                             r'\d+ named')),
)

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
# The `.des`-key half of NAME_RE (`\b[a-z_]{4,20}=\b`) is DEAD: there
# is no \b between a word char and `=`, so it never matches, and the
# authored key names (control=, access=, ...) were invisible to the
# audit's name count and to the report-only measure below. Caught while
# building the index (PR-197), which needs those keys. DES_RE is the
# working pattern, guarded so operators (>=, !=, <=) are not mistaken
# for keys. NAME_RE is kept so the historical report number stays
# comparable to the audit; the name measure counts both together.
DES_RE = re.compile(r'\b[a-z][a-z0-9_]{3,20}=(?![=<>!])')


def read(path):
    try:
        return io.open(path, encoding='utf-8', errors='replace').read()
    except OSError as e:
        print('docs structure: FAILED cannot read %s (%s)' % (path, e))
        return None


def check_template(lines, starts, heads, bad):
    """P4: every record at or above TEMPLATE_FROM carries the field
    block - ten fields, the gate in two lines, per HANDOFF section
    10a. A record's text runs from its heading to the NEXT REAL
    HEADING (the `## PR-<n>` anchor matches wrapped-title
    continuation lines too, and stopping at one of those cut this
    check's own first run of PR-196 down to a single line - the bug
    the real record caught, where the controls could not see it);
    fields are matched at column 0 only, so a quoted example
    of the template has to be indented to stay an example."""
    have, exempt = [], []
    for i, n in starts:
        if n < TEMPLATE_FROM or i not in heads:
            continue
        end = len(lines)
        for j in range(i + 1, len(lines)):
            if j in heads:
                end = j
                break
        block = lines[i:end]
        fields_here = [l for l in block
                       if any(rx.match(l) for _, rx in FIELDS)]
        if n in FIELDS_EXEMPT:
            exempt.append(n)
            if fields_here and len(fields_here) < len(FIELDS):
                print('docs structure: NOTE the exempt record PR-%d '
                      'carries %d field line(s) anyway (it is exempt '
                      'from the block, not forbidden from using the '
                      'format)' % (n, len(fields_here)))
            continue
        # found is built in LINE order, so a swap or a duplicated
        # field is visible: wanted == found only for exactly-one-each,
        # in the agreed order.
        want = [f for f, _ in FIELDS]
        found = []
        for l in block:
            for f, rx in FIELDS:
                if rx.match(l):
                    found.append(f)
        if found == want:
            have.append(n)
            continue
        cnt = collections.Counter(found)
        miss = [f for f in want if not cnt[f]]
        dup = [f for f in want if cnt[f] > 1]
        msg = []
        if miss:
            msg.append('missing %s' % ','.join(miss))
        if dup:
            msg.append('duplicated %s' % ','.join(dup))
        if not msg:
            at = next(k for k in range(len(want)) if found[k] != want[k])
            msg.append('%s before %s (the template order is %s before '
                       '%s)' % (found[at], want[at], want[at],
                                found[at]))
        bad.append('PR-%d does not carry the record template: %s'
                   % (n, '; '.join(msg)))
    print('docs structure: template check (P4, from PR-%d): %d records '
          'carry the field block, %d without (accepted)'
          % (TEMPLATE_FROM, len(have), len(exempt)))
    if exempt:
        print('docs structure: NOTE exempt from the template: %s'
              % ', '.join('PR-%d' % n for n in exempt))
    return have


def check_inventory(bad):
    """P5 (PR-200): the tracked .md set must equal the standard. Uses
    git ls-files (the repo, not the argv), so vendored/untracked
    material is out of scope by definition."""
    import subprocess
    try:
        out = subprocess.run(['git', 'ls-files', '-z', '--', '*.md'],
                             capture_output=True, text=True, timeout=20)
    except (OSError, subprocess.SubprocessError) as e:
        bad.append('P5 cannot list tracked .md files (%s)' % e)
        return
    if out.returncode != 0:
        bad.append('P5: git ls-files failed: %s' % out.stderr.strip())
        return
    tracked = sorted(p for p in out.stdout.split('\0') if p)

    def allowed(p):
        return (p in ALLOW_ROOT or p in ALLOW_GEN or p in ALLOW_DIR
                or p.startswith(ALLOW_ARCHIVE)
                or ALLOW_TOOL_RE.match(p))

    extra = [p for p in tracked if not allowed(p)]
    have = set(tracked)
    missing = [p for p in P5_REQUIRED if p not in have]
    n_arch = sum(1 for p in tracked if p.startswith(ALLOW_ARCHIVE))
    print('docs structure: tracked .md inventory (P5): %d file(s), '
          '%d in the standard, %d archived (frozen), %d outside it%s'
          % (len(tracked), len(tracked) - len(extra), n_arch,
             len(extra),
             ': ' + ', '.join(extra) if extra else ''))
    if extra:
        bad.append('%d tracked .md file(s) outside the document '
                   'standard: %s - living content belongs in '
                   'NEXT_STEPS/HANDOFF/README/PLAN, superseded work '
                   'orders and reports go to docs/archive/ (listed in '
                   'its README), tool docs to tools/*/README.md; the '
                   'set itself lives in tools/doc_census.py, change '
                   'it there on purpose, never by accident'
                   % (len(extra), ', '.join(extra)))
    if missing:
        bad.append('document standard incomplete: required file(s) '
                   'missing: %s' % ', '.join(missing))


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
    starts = {}
    heads_at = {}
    for f, t in text.items():
        starts[f] = []
        fl = t.split('\n')
        heads_at[f] = {ln for ln in range(len(fl))
                       if HEAD_RE.match(fl[ln])
                       and (ln == 0 or fl[ln - 1].strip() == '')}
        for ln, line in enumerate(fl):
            if HEAD_RE.match(line):
                heads += 1
            m = RECORD_RE.match(line)
            if m:
                ids.append(int(m.group(1)))
                starts[f].append((ln, int(m.group(1))))
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
                   'record went missing or stopped matching the '
                   '"## PR-<n>" anchor' % (len(ids), RECORD_MIN))
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

    # ---- P4 the record template (PR-196) ----------------------------
    if ns_path in text:
        check_template(text[ns_path].split('\n'), starts[ns_path],
                       heads_at[ns_path], bad)

    # ---- P5 the .md inventory (PR-200), default mode only ----------
    if not argv[1:]:
        check_inventory(bad)

    # ---- report only ----------------------------------------------
    ns = text.get(ns_path, '')
    addrs = ADDR_RE.findall(ns)
    fields = FIELD_RE.findall(ns)
    names = NAME_RE.findall(ns) + DES_RE.findall(ns)
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
          'distinct (NAME_RE + the fixed DES_RE; NAME_RE alone saw '
             '%d), wrapped-title "##" lines %d'
          % (len(set(addrs)), len(addrs), len(cites), len(set(fields)),
             len(fields), len(set(names)), len(set(NAME_RE.findall(ns))),
             cont))

    if bad:
        for b in bad:
            print('docs structure: FAILED %s' % b)
        return 1
    print('docs structure: record headings, plan sections, the width '
          'ratchet, the record template and the .md inventory hold '
          '(ok)')
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
