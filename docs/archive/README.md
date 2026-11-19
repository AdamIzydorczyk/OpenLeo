# docs/archive — frozen project history

What lives here: superseded work orders, audits and reports — the
documents that MOVED the project at their moment and are now cited as
history. They got there the honest way: each carries (and keeps) its
own banner saying what superseded it, and the journal records cover
the work itself. They were moved out of the repository root by the
PR-200 docs-inventory pass so the root holds only the four living
documents; `make verify-docs` P5 now rejects any tracked `.md` that
is not part of the declared standard, so scattered history cannot
come back by accident.

THE RULES:

1. ARCHIVED FILES ARE NEVER EDITED. Their text, counts and line
   references are frozen at the snapshot each one names. Corrections
   and supersession notes belong in THIS file, in the table below —
   never inside the frozen document.
2. A file moves here only with a record that says why (a `## PR-nn`
   record in NEXT_STEPS.md), and its row is added below in the same
   commit.
3. `git mv` only: history stays followable with `git log --follow`.

CITATIONS INSIDE THESE FILES PREDATE THE 2026-11-19 HISTORY OVERRIDE
(commit rewrites that added the [READ]/[PORT]/[GATE]/[DOCS] tags):
the shas they name are the OLD values. Resolve any of them through
`tools/history_override_2026-11-19.txt`. Measured for this table:
`c5be46c`→`4356d86`, `dfb2bf2`→`c6ca322`, `4c48dd0`→`2a29f0a`;
`a428cef` is unchanged by the rewrite.

THE SET:

| file | what it was | snapshot it names | where the work went |
|---|---|---|---|
| `open-leo_native_port_review.md` | external AI work order, P-/E-/M- ids | `a428cef` | the ids are still cited by the journal's deferred list (P-03, T-14...); each landed or stayed open through PRs recorded in NEXT_STEPS |
| `open-leo_porting_tips.md` | external AI porting guide, T- ids | `a428cef` | same: T- ids resolve here |
| `IMPROVEMENTS.md` | ranked proposals, pre-PR-7 era | 2026-11-14 | worked off through PR-7..PR-35 |
| `PORT_DEEP_DIVE_2026-11.md` | platform audit, X-/W-/L-/B- ids | `4c48dd0` | executed as the PR-25+ fix series |
| `IMPLEMENTATION_PLAN.md` | the PR-25+ work order derived from the deep dive | `4c48dd0` | carried out in PR-25..PR-35 |
| `TEST_REPORT.md` | first cold-run suite report | `c5be46c` (PR-19) | counts long superseded; current evidence is each record's GATE lines |
| `VERIFICATION_2026-11-16.md` | full-suite audit + 91-run frame sweep | `dfb2bf2` (PR-46) | its findings fed PR-47..PR-51 |

Not archived, on purpose (and why): `PLAN.md` STAYS at the root — it
is the RE-facts reference that the code itself cites ("PLAN 8b",
"PLAN 8g" in ported/ comments), so it is living, not history.
`original/README.md`, `ported/README.md`, `tools/*/README.md` are the
directory/tool descriptions and stay where they describe. The full
allowed set is `tools/doc_census.py`'s P5 constants.
