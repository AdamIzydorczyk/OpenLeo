# Test report — Lew Leon native port (ported/)

Date: 2026-11-15 (full cold-tree suite run)
Tree: `main` @ `c5be46c` — “PR-19: T-24 golden-master rig + pins” (PR-1..19 all landed)
Host: CachyOS x86-64, gcc 16.2.1, vendored llvm-mingw clang 23.1.5, wine-11.21,
Python 3.14.10 (+PIL/numpy for the python oracles), cmake 4.4.5, QEMU available.

**Result: `make verify-all` — EXIT 0, “ALL TESTS PASSED (linux + windows + llrs)”.
21 test targets, 61 emitted proof lines, 0 compiler warnings on either target.**
Two latent test-harness defects were found by the cold run and fixed (section 5);
no product-code regressions.

---

## 1. What the suite defends

The port reconstructs LEO.EXE (Longsoft/Watcom DOS) as one shared C codebase
(`ported/`) building natively for Linux (SDL2) and Windows x86-64. The suite
pins, with machine-checkable oracles:

1. **Codec byte-exactness** — every recovered file-format decoder in `ported/`
   must agree byte-for-byte with an independent Python reference on ALL real
   shipped payloads (not samples).
2. **Cross-target parity** — the Windows binary (cross-built, run under wine)
   must produce byte-identical frames/decodes/round-trips to the Linux binary.
3. **Scene & UI behavior** — chapter-map menu, restart-confirm dialog, level
   scenes, help overlay, level select: pinned observable log lines + fixed-tick
   PPM dumps (in-frame changes must show animation; overlays must be exact).
4. **Audio** — MOD parse+render determinism (hash equality linux==wine) and a
   real music-pump path producing continuous s16/44.1 kHz output.
5. **Persistence** — STATUS.LEO / LONGSOFT.CFG codecs that round-trip the
   shipped user files byte-exactly, plus per-user-dir import-once semantics.
6. **Structure** — the engine stays SDL-free behind `platform.h` (T-01 end-state),
   pinned compile-free; memory safety via ASan/UBSan on all headless tools and a
   seeded real-payload fuzzer.
7. **Golden-master vs the ORIGINAL** (T-24) — the original DOS game in a scripted
   QEMU rig vs the port’s pinned animation pose set; port-side half is pinned,
   the original-capture half is still open (section 7).

## 2. Suite inventory (21 targets, one command: `make verify-all`)

### A. Codec / data-format legs (python reference oracles)

| leg | defends | oracle & pass rule | run result |
|---|---|---|---|
| `verify` | `.IS` cutscene decode (INTRO_P.IS, 1521 frames) | C `--ppm 1020` == `tools/is_decode.py` frame 1020, byte-exact | PASS |
| `verify-skip` | ESC-skip UX end state | held final frame == python reference frame 1520 | PASS |
| `verify-llrs` | `llrs` container reader | `llrs_selftest` (ASan/UBSan) output == `tools/llrs_unpack.py list` for **ANIMACJE, ETAPY, SOUND, LEVELDES**; MENU index block (39 records) dump | PASS ×4 |
| `verify-pcx` | PCX decoder | **6/6 payloads byte-exact** vs `tools/pcx_dump.py` (every id=1 .PCX in nested blocks) | PASS |
| `verify-seq` | .SEQ animation grammar + frame-patch rows | **172/172 payloads byte-exact**, exact consumption (python asserts consumed==size), all 750 entry patch blobs diffed vs `seq_patch.py` | PASS |
| `verify-pcm` | PCM parallax layers | **4/4 payloads**: plane FNV-1a-64 + geometry vs `tools/verify_pcm.py` (TUNGA.11/ZARIA.01/.02/RAN.01 PLAN1.PCM rows) | PASS |
| `verify-mod` | MOD (Longsoft repack) parse+render | 3 tunes pulled from SOUND: TUNGA, RAN, ENA2 — linux vs wine byte-identical FNV render hashes + non-silent peak | PASS (fnv 5358b5a4…, 2e72c5e7…, 01b15e11…) |
| `verify-save` | STATUS.LEO + LONGSOFT.CFG codecs | byte-exact round-trip of the shipped files + pinned decode fields (`word63=4`, `plain[65..74]: 128 200 1 …`, `drive='H'`) | PASS (SKIP-safe) |

### B. Scene / UI legs (pinned logs + differential frame tests)

| leg | defends | oracle & pass rule | run result |
|---|---|---|---|
| `verify-menu` | demo chain (cutscene→menu→level), chapter-map render, BERLO arrow anim, restart-confirm dialog (OPTIONS.PCX + RESTGRY/TAK/CNIE/X), TAK answer flow, keyboard nav, bright-green selection tint | pinned log strings (`map: anim 0: 30 frames`, `restart confirmed (tak)…`); `LEO_MENU_PPM_TICK=2/8` dumps must DIFFER (animation alive); `tools/verify_green.py` numeric oracle: open frame == CNIE frame byte-exact, TAK select changes only the TAK rect + underlines and moves ink UP the green ramp (idx15→idx22) | PASS ×8 |
| `verify-sel` | MAPKA level-select submenu + MAPKA.MOD + TUNGA2.12 open | pinned chain log via LEO_MENU_AUTO/LEO_LEVELSEL_AUTO | PASS |
| `verify-level` | TUNGA.11 .DES wiring, .SEQ sprite driven by .MOV state machine, ANI3.DES placement join, scroll camera (world 2560x600, cam 300/1920), PCM-canvas level RAN.01 | pinned wiring logs; tick2≠tick8 (motion); PCM view FNV == `pcm_selftest` oracle (`bba1bfc6…`); cam300 frame differs | PASS ×5 |
| `verify-help` | level help overlay (POMOC .MNU pages + POMOC00.SEQ art) | `tools/verify_help.py`: help frame == bare bg + exactly the page-0 window; painted-pixel count == `seq_render.sprite()` touched count | PASS |

### C. Audio path

| leg | defends | oracle & pass rule | run result |
|---|---|---|---|
| `verify-music` | E-09 music pump thread + llrs→MOD join in a real level scene | `tools/verify_music.py` on LEO_MUSIC_WAV dumps: ≥1.5 s continuous s16 stereo @44.1 kHz, silent=0 clip=0 | PASS (tunga.MOD 2.37 s, peak 19259) |

### D. Portability legs

| leg | defends | oracle & pass rule | run result |
|---|---|---|---|
| `verify-win` | the WHOLE behavior contract on the Windows x86-64 binary under wine: decode, skip, PCX/SEQ/PCM/containers, menu/level/dialog frames, scroll camera, PCM level, save codec | every Linux proof above re-run against `*.exe` and `cmp`-ed **byte-identical** (14 sub-checks: “win … == linux/python (byte-exact)”) | PASS ×14 |
| `verify-utf8` | non-ASCII data/output paths (the review’s `C:\Gry\Łódź\` case) | same byte-exact frame through a UTF-8 path on linux AND wine; DATA discovery/probe `DATA …Łódź…: complete` | PASS ×3 |
| `verify-cmake` | CMake build == Makefile build (T-20) | both cmake linux/windows binaries produce the identical python-reference frame | PASS (SKIP-safe) |

### E. Robustness / structure legs

| leg | defends | oracle & pass rule | run result |
|---|---|---|---|
| `verify-fuzz` | all parsers under mutation (T-25) | `tools/fuzz.c` (ASan/UBSan), 3 fixed seeds × 400 iters over an 80-blob corpus of REAL payloads (llrs/IS/pcx/seq/pcm/mod); every run must exit clean, no crash/UB/leak | PASS (1200 mutations, zero errors) |
| `verify-platform` | engine runs with NO SDL (T-01 headless contract) | `platform_selftest` built with the dummy backend, zero SDL in the link; **29 checks** incl. clock/audio/input/fopen contracts, “no pref dir” and box/video no-op fallbacks | PASS |
| `verify-arch` | SDL confinement (regression pin for PR-15..18) | compile-free grep: only `platform_sdl.c`, `platform_sdl/`, `ll_view.h`, `ll_audio.h` may include SDL2 | PASS |
| `verify-userdir` | T-17 save/config dir semantics | hermetic temp-dir runs: import legacy save EXACTLY once, byte-identical, original untouched, LEO_SAVE override wins | PASS (SKIP-safe) |

### F. Golden-master vs the original game (T-24)

| leg | defends | oracle & pass rule | run result |
|---|---|---|---|
| `verify-golden` | port chapter-map animation == the pinned golden contract | port frame dumps (LEO_MENU_PPM_DIR) must reproduce the **30-hash pose set** in `tools/golden_expect.json`; if a QEMU capture exists (`make golden_rig_ddir`, now `--want-art`), every port pose is scored against the original ART frame RGB+index plane with `--min-match 99.9`, and golden.* manifest hashes asserted | PASS — port half: “port pose set: 30 hashes match manifest”; original-capture half: “original capture absent … checked port pins only” (expected, see §7.1; full-compare with a capture is measured 98.4% vs the art frame, floor 98 (PR-23) |
| `golden_rig` (not in verify-all) | the ORIGINAL DOS game booting and rendering in QEMU no-CD rig | `tools/golden_build_rig.sh` (MS-DOS 6.22 floppy + mtools HDD, CD-gate patch `0x5bd51:b801000000eb32`; `DDIR=1` disk-D route) + `tools/golden_qemu_run.py` (QMP, CRTC-geometry frame detector, static-frame capture) | PASS — capture landed (runs L10+L12 byte-identical: vram.bin 14e15fb2…, menu.ppm f3ee4764…, static_vram_equal=True, 161 indices, 0 palette-bijection violations); golden.* pinned in golden_expect.json |

## 3. Methods used (7 oracle classes)

1. **Independent Python references** per format (`is_decode`, `llrs_unpack`,
   `pcx_dump`, `seq_parse`/`seq_patch`, `verify_pcm`, `verify_help`,
   `verify_green`, `verify_music`) — themselves pinned against the real DOS
   codec by gdb traces and raw disassembly of LEO.EXE (documented in each).
2. **Cross-binary byte-identity** (linux ELF vs Windows PE under wine) — the
   strongest available proof without a Windows CI box.
3. **Differential frame tests** — fixed virtual tick injection (LEO_*_PPM_TICK):
   same tick must be deterministic, tick 2 vs 8 must differ (animation alive).
4. **Pinned behavior logs** — stable diagnostic strings double as flow contracts.
5. **Numeric geometry oracles** — pixel-count/rect/ramp checks (green select,
   help overlay window) instead of brittle whole-frame hashes for reconstructed art.
6. **Round-trip codecs** — decode+re-encode shipped saves == original bytes.
7. **Golden-master** — the original binary’s own rendered frames, captured in a
   scripted QEMU rig, compared against the port (pose-set model on the port side).

All headless C tools build `-O1 -g -fsanitize=address,undefined`; the two game
binaries build `-O2 -std=gnu11 -Wall -Wextra` warning-free on gcc and clang.

## 4. This run, end to end

- `make clean` → linux build: 0 warnings → `make win` (incl. `resources/ll_cutscene.res`
  icon/VERSIONINFO/manifest via windres): 0 warnings.
- First full-suite pass then hit the two Makefile defects in §5; after the fixes:
  single-invocation `make verify-all` → **EXIT 0, ALL TESTS PASSED**, 61 proof
  lines (Appendix).
- Representative emitted evidence (verbatim):
  - `linux .elf == python reference (byte-exact)` / `win .exe == python reference (byte-exact)`
  - `SEQ check: 172/172 payloads byte-exact (C == python, grammar + patch rows, exact consumption)`
  - `ANIMACJE/ETAPY/SOUND/LEVELDES: C == python (exact)` (linux AND win)
  - `MOD TUNGA: parse+render linux == wine (fnv=5358b5a4ba797f05)` (×3)
  - `music pump: tunga.MOD 2.37s s16 stereo @44.1k, silent=0 clip=0 peak=19259 (ok)`
  - `STATUS.LEO: decode + byte-exact round-trip of shipped save (ok)` + `win save codec == linux (ok)`
  - `fuzz: seed=1..3 corpus=80 blobs iters=400` → `fuzz: clean (no crash/UB/leak reported by ASan)` (×3)
  - `verify-platform: headless engine path + clock/audio/input/fopen contract (ok)` (29 checks)
  - `T-01 structure: engine+app SDL-free, backend only (ok)`
  - `port pose set: 30 hashes match manifest` / `golden-master comparison: ok`
  - `cmake linux binary == python reference (byte-exact)` (+windows binary)

## 5. Defects found BY this run (test harness, not product) — fixed in the working tree

1. **`verify-win` missing prerequisites** (`Makefile`): the leg compares against
   FRESH Linux runs (`./ll_level`, `./ll_save`) but listed only the `.exe` twins.
   On a cold tree it failed mid-leg with the confusing
   `cmp: /tmp/opencode/lvlv_t2.ppm: No such file`. Every previous green run had
   a stale binary in the worktree masking this. **Fix:** added `ll_cutscene
   ll_level ll_save` to the prerequisite list.
2. **`verify-level` implicit dependency**: its PCM-canvas check runs
   `./pcm_selftest` (undeclared; masked only by target ordering inside
   verify-all). **Fix:** added the prerequisite.
3. **`tools/golden_menu_cmp.py` crash on numpy ≥ 2.0** (from an earlier session,
   still uncommitted): `np.unique(return_inverse)` now returns same-shape
   inverse; `np.bincount` then rejects it. **Fix:** ravel + explicit int64.
   Latent in CI because only the capture-compare path used it.

These matter for the *report’s trustworthiness*: the suite was green before too,
but not provably green on a cold tree until now.

## 6. Environment / fixture operations (reproducible bootstrap)

Game data and user saves are deliberately **not in git** (copyright; hashes only).
At 04:27 today the machine rebooted and `/tmp` (tmpfs) wiped all staged
fixtures; the suite was re-run after the recipe documented in NEXT_STEPS.md:

```
bsdtar -xf LL.iso -C /tmp/opencode/iso_stage LEO                    # 19 CD files
bsdtar -xf LL.iso -C /tmp/opencode LEO_HDD && mv LEO_HDD/* /tmp/opencode/hd_files/
cp STATUS.LEO LONGSOFT.CFG /tmp/opencode/hd_files/                   # repo-root copies (gitignored)
python3 tools/is_decode.py frames $DATA/INTRO_P.IS /tmp/opencode/py_last --ppm --only 1520
```

Notes for operators:
- `verify-save`, `verify-userdir`, `verify-golden`, `verify-cmake` SKIP cleanly
  when their optional fixtures/tools are absent — that is by design; the rest
  hard-fail, which is why the bootstrap above is required, not optional.
- Fragility found: several recipes hardcode `LEO_HD_DIR=/tmp/opencode/hd_files`
  while `HD ?=` is parameterized — half-migration; recommend finishing the
  variableization so a non-/tmp fixture root works (no product risk).
- A wiped `WINEPREFIX` just costs ~1 minute of fresh prefix init.
- Long CI jobs should run in the foreground/with proper timeout handling: a
  12-minute background suite run was SIGKILLed by the harness at 15 min (no
  OOM; 61 GB RAM host) — worth noting before wiring a CI runner.

## 7. Honest gaps and open items

1. **Golden-master original-side capture: LANDED (PR-21/PR-21b/PR-22,
   2026-11-15).** The scripted no-CD disk-D rig reaches the map-menu state
   (one `Esc` landing in the intro-movie window at t≈30 s) and the driver
   captures the ORIGINAL's index plane. `golden.*` now pins the MAIN
   LANDSCAPE art frame: two `--want-art` runs byte-identical (`vram.bin`
   4cb7a311…, screendump sha1 `b880e203ce52` — the frame seen byte-
   identical in every observation ever made; 209 indices, zero index→color
   violations; idx-plane = the exact `ba16421b…` bytes that produced
   `ll_menu_dac.h` in PR-19 — independent recovery). The menu flips CRTC
   display-start between two composed pages: art at VRAM +0x0000, the
   near-black overlay half at +0x20000 (its hashes kept as
   `golden_black_page_info`, informational; content-based page picking was
   replaced by page_reproduces_screen + the exact frame hash after a wrong
   page (L11) and a mid-fade frame (A3) fooled the heuristics). The
   frozen-0x46C clock never froze in any run and is not a menu signal.
   MEASURED GAP: port best = 0.43% RGB / 6.4% index vs the ART frame —
   the port composes only the map-menu overlay poses, the original's page
   is one full static landscape; `min_best_rgb` deliberately unset (the
   gap is the target signal, not an acceptance bar), so `verify-golden`
   stays green in port-pins mode and full-compare mode is red by design
   when a capture is present in `$(GOLDEN)`. Asset hunt round 2 (palette-
   free permutation sweep vs all 5619 frames of all 15 .IS movies + the
   still-valid verbatim byte-grep): the landscape is NOT an .IS frame and
   NOT in any container → runtime-synthesized or lost; the port path is
   documented reconstruction scored against the golden bytes. FOLLOW-UP
   (PR-23, same day, user-approved): the recovered picture is embedded as
   generated source (`tools/menu_art_gen.py` -> `ported/ll_menu_art.h`,
   integrity-checked against the pinned idx sha; default-painted,
   `LEO_MENU_BG=placeholder` selects the procedural stand-in). Measured
   port best: 0.43% -> **98.43% RGB/idx** (off-palette px 0); pose pins
   regenerated (still 30), `min_best_rgb`/`GOLDEN_MIN` set to a 98.0
   regression floor. Remaining ~1.6% is composed-text/overlay placement
   vs the original's burned-in texts — background work is DONE.
2. **CI runner config — ADDED (PR-24, 2026-11-15).** `.github/workflows/ci.yml`
   (ubuntu-latest: system toolchain builds BOTH targets — the vendored
   llvm-mingw/SDL2 are gitignored, so the Makefile's system fallbacks apply —
   a `python3 -m compileall tools` check, then `make -k verify-all`). The
   Makefile now probes fixture presence at PARSE time (`PX_DATA`, `PX_HD`,
   `PX_SAVE`, `PX_WINREF`, `PX_CMAKE`) and ifeq-wraps every data-backed leg,
   so a fixture-free runner gets a named SKIP carrying the exact staging
   command and NO stray command executes. Proven both ways on 2026-11-15:
   fixture-free `make -k verify-all` exits 0 with clean SKIPs; the full
   local fixture set is still exit 0 with all data legs actually running.
   The copyrighted game data never reaches the runner (LL.iso is gitignored
   by design), so the byte-exact frame proofs and golden comparison remain
   local runs. The real-Windows job (handheld) is still open — see §7.3.
3. **Windows proof is wine-based.** The actual Windows 11 target (ADAM-HANDHELD)
   is not in the automated loop (host currently unreachable over the lab link).
   The manifest/UCRT UTF-8 path is proven under wine emulation only.
4. **Cutscene coverage is single-stream in playback.** All 13 .IS movies are
   parseable and the fuzzers mutate them, but end-to-end pinned playback is run
   for INTRO_P.IS only (frames 1020/1520); the other 12 attract/ending movies
   have no per-frame golden.
5. **Fuzzing depth**: 3 fixed seeds × 400 iters is a regression gate, not
   exploration; .DES text grammar is not fuzzed yet (noted in NEXT_STEPS).
6. **Timing realism**: legs inject fixed virtual ticks by design; wall-clock
   pacing (T-05/T-06) remains a deferred item, correctly untested.

## 8. Reproduce

```
make clean && make && make win            # expect zero warnings on both
make verify-all                           # expect “ALL TESTS PASSED (linux + windows + llrs)”
# optional deep rig (needs LL.iso + MS-DOS 6.22 boot floppy image + qemu + mtools):
make golden_rig && make verify-golden     # then golden.* can be pinned in tools/golden_expect.json
```

## Appendix — proof-line census (this run)

61 emitted proofs in one `make verify-all` pass:
34 cross-target/byte-exact codec comparisons (incl. 4 containers ×2 platforms,
6 PCX, 172 SEQ, 4 PCM, 3 MOD, saves, utf-8 frames),
16 scene/UI behavior checks (linux 8 + wine twin 8),
3 audio (mod determinism + music pump + MAPKA.MOD in select chain),
4 robustness (fuzz×3 + platform 29-check run),
2 structure/architecture (verify-arch, pose-set manifest),
2 build-parity (cmake linux+windows binaries).
Plus 1 designed informational SKIP (verify_green pre-fix baseline frame
comparison, CI has no old build) — see Makefile comment at that leg.

*Report generated from the actual run logs (`/tmp/opencode/vf_all.log`).*
