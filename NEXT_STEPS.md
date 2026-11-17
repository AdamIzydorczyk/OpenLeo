# OpenLeo native port — next steps

> **Start with HANDOFF.md** (the current map, rules, estimates, open
> questions and the ordered task list). This file is the per-PR history:
> each `## PR-nn` heading is one step with its evidence; the newest are at
> the end, just before `## Next`. Numbering continues at PR-75.

Working plan derived from `OpenLeo_native_port_review.md` (P/E/M) and
`OpenLeo_porting_tips.md` (T). Update the status lines here as items
land; the review/tips files stay as the detailed work orders.

## Status as of 2026-11-14 (through PR-19, all pushed; later PRs have their own
## records below - the newest is the last `## PR-nn` heading before `## Next`)

Done and verified green (full `make verify-all` incl. wine parity):

- P-01, T-18  fs_ci case-insensitive/UTF-8-aware opener            (da96f3a)
- E-07        llrs range guards                                     (4f0fcad)
- P-08, P-09, T-17(partial) DATA discovery chain, demo-chain ESC
  semantics, clear missing-data report                              (7491583, fb02a98)
- layout      src/ -> ported/, merged upstream snapshot             (282b616, 8cf8636)
- P-02, P-12  player: single ns clock (p->t0_ns), unified pause,
  reused lbuf, LEO_IS_WAV fclose + header patch                    (cc1b5de)
- E-02..E-06  help overlay lifetime, obj_elem clamp, count==0
  anims, ll_level_show cleanup, ESC = cancel                        (ef734ee)
- M-01..M-12, T-10  MOD player full ProTracker semantics + int32
  mixer, L,R,R,L pan, ~-6 dB master (0% clip, 22/22 ASan-clean)    (e1f89db)
- E-09, P-14(strip) music pump thread (queue >=250 ms, first chunk
  sync), -Wl,-s -> ll_cutscene.exe 13.4 MB -> 1.8 MB               (857de73)
- PR-7 quick wins: P-13 accepted-audio-spec logging (player + music);
  P-11 header docs (done in cc1b5de, verified); P-06 fatal_msg =
  stderr + Windows MessageBox + --console attach; P-14b .rc + icon +
  VERSIONINFO + app.manifest (PerMonitorV2, UTF-8 ACP) linked into
  ll_cutscene.exe via vendored windres; NEW verify-music CI target
  (tools/verify_music.py: LEO_MUSIC_WAV dump >=1.5s, not silent,
  unclamped) wired into verify-all.
- PR-8 display UX: shared ported/ll_view.[ch] (logical viewport at
  content size, resizable + ALLOW_HIGHDPI, nearest scaling, F11 /
  Alt+Enter desktop-fullscreen in all four pumps), --scale/--fullscreen
  flags, menu /sc math removed. Default geometry unchanged.          (ca2ff24)
- PR-9 windows polish: P-07 completed - ll_ftell64/ll_fseek64 helpers
  (no 2 GB cliff on Win64), all write opens via ll_fopen_ci (wide-path
  writes), manifest longPathAware; NEW verify-utf8 (game + PPM I/O +
  DATA probe through /tmp/opencode/Gry/Łódź/, linux + wine).
  T-20: CMakeLists.txt + cmake/toolchain-mingw-x86_64.cmake, both
  byte-exact under CI (verify-cmake, skips when cmake absent; windows
  cmake build links SDL2.dll dynamically - Makefile stays static).
  T-21/T-22: tools/package.py + make package{,-win,-linux} -> dist/
  zip|tar.gz (exe + README + data/ placeholder; unpack-and-play proven
  under wine).                                                        (aa4e97a)
- PR-10 P-03+P-04: one session window for the whole program. ll_view
  is now a process-wide session: first ll_view_create builds the
  window, later screens RECONFIGURE it (title/logical size/geometry);
  screens never destroy win/renderer (ll_view_release keeps it).
  is_player dropped SDL_Init/SDL_Quit. Focus: per-tick
  SDL_RaiseWindow polls deleted; raise once at reconfigure + on
  WINDOWEVENT_ENTER via ll_view_handle_event (was
  ll_view_fullscreen_key). External SDL_Quit() healed via
  SDL_WasInit re-create.                                                (accfd93)
- PR-11 T-25: seeded in-process parser fuzzer (tools/fuzz.c, Makefile
  fuzz/verify-fuzz, in verify-all). Corpus = 80 REAL payloads from
  DATA: container heads (ANIMACJE/ETAPY/SOUND/LEVELDES, 2 MB cap),
  .IS streams (<=8 MB, 3 files), and PCX/SEQ/PCM/MOD records pulled
  via llrs_block + llrs_read (the payloads live INSIDE id=2 index
  blocks - same trick as verify_pcx.py). 1..6 mutations per blob
  (xor-bit / 0x00 / 0xff / truncate), 3 fixed seeds x 400 iters,
  ASan+UBSan build; accepted/rejected counters per kind prove the
  corpus reaches every decoder (a kind that never parses = fail 2).
  All six kinds green, zero crashes/UB/leaks, ~3 s per seed.            (71ece54)
- PR-12 P-03(audio): one audio session. New ported/ll_audio.[ch] owns
  the SDL audio subsystem lifecycle (ll_audio_session_init = the old
  ll_audio_init/pipewire-pin, moved out of ll_level.c; ll_audio_session
  _close). Device acquisition centralised in ll_audio_open (enforces
  the samples=0 + queue-only contract, logs accepted spec once = P-13)
  and ll_audio_close (closes + SDL_ClearError). is_player + ll_music
  both go through it; the per-caller spec/P-13 code is deleted. Device
  is per-playback on purpose (spec differs 22k/44k, no resampling,
  queue is a stateful FIFO) but scenes never overlap, so exactly one
  live device at a time. ll_level.h drops the ll_audio_init decl.        (691b545)
- PR-13 T-17 save/config dir: new ported/ll_paths.[ch] computes the
  per-user WRITABLE dir ($LEO_USER_DIR override, else
  SDL_GetPrefPath("LongSoft","LewLeon") = ~/.local/share/... or
  %APPDATA%\...), ll_resolve_user_file() resolves STATUS.LEO there and
  imports a DATA-adjacent legacy copy EXACTLY once (COPIED byte-ident,
  original untouched); main.c LEO_SAVE stays an absolute override (API
  preserved). pick_data_dir search order extended with
  $XDG_DATA_HOME/openleo / ~/.local/share/openleo / /usr/share/openleo,
  factored into find_data_candidate + a silent detect_data_dir() (so
  single-file runs keep byte-ident output). NEW verify-userdir CI leg.   (f34543e)
- PR-14 T-01 phase 1: engine-facing platform interface. ported/platform.h
  declares the T-01 surface (plat_init/present/poll/now_ns/sleep_until/
  audio_queue/queued_ms/fopen + engine-level ll_event). Backends chosen
  at COMPILE time: PLAT_BACKEND_DUMMY (ported/platform_dummy.c, headless
  - implemented now) vs PLAT_BACKEND_SDL (over ll_view/ll_audio - the
  game keeps using those directly for now). plat_fopen delegates to
  fs_ci in a shared platform_common.c. tools/platform_selftest.c runs
  decode(real .IS)->present->clock->audio->input THROUGH the interface
  and links ZERO SDL (ldd/nm prove it) - the "test the engine without
  SDL" capability T-01 exists for. NEW verify-platform CI leg (headless,
  ASan/UBSan). Screens not yet migrated to plat_*: that is PR-15.        (c8b68f1)
- PR-15 T-01 phase 2 (player): ported/platform_sdl.c implements the
  SDL backend as a thin delegate over ll_view/ll_audio (streaming
  texture + LUT blit moved out of the player, input -> ll_event with
  F11/Alt+Enter consumed, ns clock, byte-based audio open/queue/
  pause/clear/queued). is_player.c is now SDL-FREE (0 SDL headers in
  its TU): present() linearizes then hands the INDEXED frame over,
  pump_input reads plat_poll (q/' ' verbatim ASCII, LLKEY_ESC), audio
  + drain + end-hold via plat_*. platform.h audio API extended to
  the real contract (open/queue bytes/pause/clear/queued + -1/-2
  error split preserves both old diagnostics verbatim). Backends now
  switch on PLAT_BACKEND_DUMMY on BOTH sides (glob-safe). Verified:
  full verify-all 36 (incl. win/wine PPM byte-exact, demo chains),
  ASan/UBSan chain zero app leaks, REAL X11+audio smoke with XTEST
  keys: q->quit(q), ESC->skip@35, space->pause froze the LEO_IS_WAV
  feed exactly for the hold then resumed 125/125. menu/map/level
  still drive ll_view/ll_audio directly (their migration = later PR).  (5634649)
- PR-16 T-01 phase 3 (screens): title menu, chapter map and level
  scene migrated off direct ll_view/SDL onto plat_* — ll_menu.c is
  now SDL-FREE (cpp proves 0 SDL headers in its TU), ll_level.c keeps
  SDL ONLY for the music service (documented at its include; moving
  that = PR-17). Interface grew: LL_EV_BUTTON_DOWN/MOTION with x,y in
  LOGICAL viewport coords (backend un-scales, P-10), LLBTN_LEFT,
  LLKEY_UP/DOWN/LEFT/RIGHT/RETURN(KP_ENTER folded)/HOME/END/PGUP/
  PGDN (printables incl. space stay ASCII); plat_present_argb(fb,w,h)
  = the native form of these compositing screens (upload as-is, zero
  conversion); plat_raise() replaces every win_force_focus /
  RaiseWindow+SetWindowInputFocus pair. ll_level's paint() keeps the
  scene's own LUT (PCX palette AND the documented gray fallback) and
  expands into a persistent ARGB scratch — the platform never sees a
  palette, so the PPM/help/scene pins are untouched by construction.
  Menu timing derives from the ONE plat clock (local now_ms helper).
  ll_view/ll_audio are now backend-private except for that music
  exception. Verified: full verify-all 36 after make clean (incl.
  help-render oracle px-count, scene-canvas fnv, linux==wine PPM
  byte-exact), platform_selftest 24 (added mouse round-trip, argb
  accounting, raise safety), ASan/UBSan clean on the glob build +
  demo chain + level tool, and a REAL-session X11 smoke: island
  CLICK launches chapter -> IS -> ESC skip -> level p/ESC -> map q,
  rc 0 (synthetic core Button events under XI2-off; kwin owns the
  Wayland-side pointer so XTEST hit-testing is unavailable there).    (d58f33c)
- PR-17 T-01 phase 4 (music service): ll_level.c is now COMPLETELY
  SDL-free - the music singleton uses plat_audio_open/queue/close for
  its device (same queue-only device the player uses; samples=0
  contract now enforced backend-side), and the E-09 pump thread goes
  through NEW plat_thread_start/plat_thread_join (C11 <threads.h> is
  NOT available on the vendored llvm-mingw - probed; the platform
  keeps the thread: SDL backend = the exact SDL_CreateThread call,
  diagnostics byte-identical; headless dummy = -1, which runs the
  documented UI-loop pump fallback = pre-E-09 path, both paths byte-
  same). Quit flag moved to C11 stdatomic (probed available on both
  toolchains); low_water rewritten as (size_t)LL_MOD_RATE = the same
  250 ms; error split preserved (-1 subsystem -> silent play like the
  old silent session-init path, -2 device -> ll_music: device: %s
  with plat_last_error). Remaining direct SDL after this PR = app
  glue ONLY (main.c GetBasePath/MessageBox/Delay/video-pin, ll_paths.c
  GetPrefPath) - PR-18 candidate. Verified: verify-all 36/36 after
  clean, selftest 26 (+2 thread-contract checks), ASan/UBSan glob
  build + real X11+ALSA session chain: MAPKA.MOD played through the
  platform device + REAL platform pump thread (no fallback message),
  joined clean, rc 0; LSan in real-audio sessions shows only third-
  party libasound/sdl2-compat one-time allocs (zero app frames) -
  documented below with the timeout/SIGTERM gotcha.                  (9e52313)
- PR-18 T-01 phase 5 (app glue): main.c and ll_paths.c are now
  SDL-FREE too (cpp proof: 0 SDL headers AND 0 SDL_ tokens in their
  TUs) - only the backend files (platform_sdl.c, platform_sdl/
  is_player.c, ll_view/ll_audio) reference SDL anymore, and that is
  now a CI STRUCTURAL pin: new verify-arch greps ported/ for SDL2
  includes outside the backend set and names any offender. Interface
  grew: plat_base_path (malloc'd copy of SDL_GetBasePath),
  plat_message_box (box policy backend-side: Windows only, P-06;
  headless no-op), plat_pref_dir (per-user dir, GetPrefPath
  semantics, creates it; headless -1 -> LEO_USER_DIR-only fallback
  is exactly what headless tests want), plat_prepare_video (the
  Wayland->X11 focus pin moved VERBATIM incl. both byte-identical
  "main: ..." diagnostics), plat_opt_scale/plat_opt_fullscreen
  (renamed ll_opt_* display overrides now declared on platform.h,
  still defined + consumed by ll_view.c); demo title hold uses the
  one plat clock (plat_sleep_until); main.c's vestigial ll_view.h
  include dropped. Headless host answers pinned in selftest (+3 =
  29). Verified: verify-all 37/37 (incl. new verify-arch),
  verify-userdir unchanged, both toolchains warning-free.        (this commit)
- PR-19 T-24 golden-master: scripted no-CD QEMU rig
  (tools/golden_build_rig.sh + build_data_disk.sh), ORIGINAL-game
  capture driver (tools/golden_qemu_run.py: 0.25 s tick sampling,
  sustained 70 Hz movie-clock gate, frozen-counter map-menu detector
  with art<->black retry), and golden-vs-port comparator
  (tools/golden_menu_cmp.py: 2x2-doubling + index/color bijection
  validation, port-frame scoring). Port frames captured via new
  LEO_MENU_PPM_DIR/MAX hook. Repo stores HASHES ONLY (never frames,
  per T-24): tools/golden_expect.json pins the 30-hash chapter-map
  animation POSE SET (timing-independent; per-window counts are
  not). CD-gate patch FINALISED at 0x5bd51:b801000000eb32 (the
  0x5bd5c variants are non-deterministic without ASPI). Makefile
  verify-golden (SKIP-safe: data-missing=SKIP, capture-missing =
  port-pins-only) + golden_rig; verify-all now 38 legs. The
  golden.* half stays open: the no-CD route has not yet been shown
  to reach the frozen-counter map-menu state PLAN S6 measured in
  the CD rig (still in its 480-line movie chain at ~900 s attract).

Known-clean baselines: ASan/UBSan on demo chain + all 22 MODs = app
leaks 0 (SDL backend internals only); CI pins are format-only for MOD
output, byte-exact for llrs/seq/pcm/pcx, grep+PPM pinned elsewhere.

## PR-7 — quick wins (DONE, see status above)

Kept for the record: P-13 (audio spec log), P-11 (docs), P-06
(diagnostics), P-14b (rc/manifest), verify-music. llvm-rc quirk found:
the .rc must use numeric resource statements (`1 VERSIONINFO`, `1 24`),
not winresrc.h macros.

## PR-8 — display & input UX  (DONE, see status above)

Landed: P-10 (logical-size presentation via ported/ll_view.c shared by
player/title menu/chapter map/level scene; resizable + ALLOW_HIGHDPI
windows; --scale N / --fullscreen; F11 + Alt+Enter desktop-fullscreen
toggle), T-12 (nearest scaling hint + manifest DPI already in PR-7),
T-13 (FULLSCREEN_DESKTOP only, never exclusive). Menu click/motion
math now uses SDL logical coords (the /sc division is gone). Default
geometry and every pixel pinned by CI are unchanged (verify-all green,
25 checks incl. wine parity).

Deliberately left open from P-10/T-1x:
- vsync: intentionally NOT enabled (the cutscene clock is audio-driven;
  the review agrees).
- T-11 aspect 4:3: still a decision to make (see deferred list).
- T-15 scancodes: menu zone keys are letter MNEMONICS (A/O/R match the
  drawn words), so keycodes stay correct there; arrows/Enter are
  already layout-independent. Revisit only for text entry (T-16).
- remember fullscreen choice in config (belongs with T-17 save/config).

## PR-9 — Windows polish (DONE, see status above)

Landed: P-07 (UTF-8 paths already via ll_fopen_ci since PR-1; added the
64-bit half: ll_ftell64/ll_fseek64 + wide-path write opens +
longPathAware manifest + verify-utf8 CI), T-20 (CMakeLists + mingw
toolchain, verify-cmake CI), T-21/T-22 (tools/package.py, make
package). Notes: wine mangles non-ASCII argv under LC_ALL=C (don't set
it in CI); SDL2 mingw config exports .../include/SDL2 — CMakeLists
normalizes to the <SDL2/SDL.h> parent convention. MSVC compat shims
(T-20 compat.h: strcasecmp/strtok_r/dirent) intentionally deferred.

## PR-10 — one window + focus policy (DONE, see status above)

P-03 + P-04 landed together through the ll_view session (the seam
PR-8 created). Notes for later: screens must free their OWN textures
(is_player now does); an external SDL_Quit (tools harness) is healed
by the WasInit guard; P-05 (Wayland native) stays open - the focus
policy change may make the forced-X11 probe unnecessary, test on
hardware before touching main.c's wayland branch.

## PR-11 — parser fuzzing (DONE, see status above)

T-25 landed as a deterministic in-process fuzzer rather than libFuzzer:
same binary, no toolchain dependency, corpus from the real formats.
Deliberate limits, for the record: ll_save is not fuzzed (fixed-size
raw-word copy, no grammar) and the .des/.MNU text scanners are not
either (a text-grammar fuzz is its own project). If a fuzz failure
ever appears, reproduce with the printed seed and shrink the blob.

## PR-12 — one audio session (DONE, see status above)

Completes P-03 on the audio side, mirroring how PR-10 did video.
What is deliberately NOT a persistent singleton: the audio DEVICE.
Unlike a window, an SDL audio device is spec-specific (cutscene 22050
mono/stereo, MOD music 44100 stereo; SDL applies no resampling to
queued bytes and both streams are byte-pinned by CI) and a stateful
FIFO (a kept-open device would bleed one scene's queue tail into the
next). So the SUBSYSTEM + driver policy + spec contract are session-
owned (ll_audio.c, exactly once per process); the DEVICE stays
acquire-per-playback (ll_audio_open/close). Scenes never play
simultaneously, so there is still exactly one live device at a time -
which is what P-03 was actually about. If a future screen needs
concurrent SFX, add a mixer in ll_audio.c, do not reopen per call.

## PR-13 — per-user save/config dir (DONE, see status above)

T-17 landed on its writable-state half: ll_paths is the single seam
for the user dir + save resolution, and main's LEO_SAVE hook became a
verbatim override over it (so the existing save-diagnostic API and the
"not loadable" text are unchanged; repo data/ has no STATUS.LEO so no
CI leg triggers an import, and detect_data_dir is silent so single-file
runs are byte-identical). Not done here, by design: a CONFIG file
(openleo.ini) has no runtime consumer yet - settings are still CLI/env -
so the helper (ll_resolve_user_file with leaf="LONGSOFT.CFG"/"openleo.ini")
is ready but unwired. When a settings subsystem lands, it uses the same
resolution + one-time-import path, no new plumbing. Import leaves the
original in place (mirrors the "retail data untouched" rule); the
resolution then re-reads the user copy on later runs.

## PR-14 — platform interface + headless backend (DONE, phase 1 of T-01)

The concrete pain T-01 named (window per scene, SDL_Init per cutscene,
quirks in 4 screens) was already removed by PR-8/10/12's ll_view +
ll_audio sessions. PR-14 builds the engine-facing seam ON TOP of that:
one platform.h contract + a second, headless backend, and a selftest
that runs the whole decode->present->audio->clock->input surface with
zero SDL (proven by ldd/nm). That is the "engine testable without SDL"
gate T-01 asks for, and it is where future headless unit tests
(golden-master, libretro/Emscripten backends) attach.

Remaining for the full T-01 (the risky part deliberately deferred so
each step stays verifiable): implement PLAT_BACKEND_SDL as a thin
delegate over the existing ll_view/ll_audio (no new internals) and
MIGRATE THE SCREENS (cutscene player first, then title/map/level)
to plat_poll + plat_present(indexed fb) + plat_now_ns/sleep_until,
retiring direct SDL in engine code. That migration touches the byte-
exact PPM/menu/sel pins, so it lands behind those same oracle checks;
the dummy backend already gives a second consumer to catch divergence.
DONE for the player in PR-15, DONE for title/map/level in PR-16;
ll_view/ll_audio are backend-private now, with the music service the
single remaining direct user (PR-17 candidate).

## PR-15 — SDL backend + player migration (DONE, phase 2 of T-01)

platform_sdl.c contains NO new session semantics: ll_view_create /
ll_audio_open keep owning the window, focus, viewport, spec contract
and logs; what moved out of is_player.c is only what a screen used to
own - the streaming texture + palette-LUT blit (byte-identical code
moved), the SDL->ll_event mapping (F11/Alt+Enter consumed inside
plat_poll, printables verbatim, ESC = LLKEY_ESC), and the ns clock.
The audio API grew from the T-01 sketch to the player's REAL needs:
open/close (one device at a time), queue in BYTES (no resampling,
P-13 contract intact), queued_bytes (drain-to-end), pause (A/V-locked
pause, P-02), clear (ESC skip); -1/-2 split keeps both legacy
diagnostics byte-identical. plat_present takes the LINEARISED indexed
frame + raw 6-bit palette; the backend's v<<2 expansion is the exact
is_palette_rgb8 formula, so presented pixels are unchanged (and the
PPM oracle pins, which read CPU-side buffers, are untouched by design
- they passed byte-exact on linux AND wine anyway).

Backends both switch on PLAT_BACKEND_DUMMY, so any target that globs
sources links exactly one (the ASan smoke build is that glob).

Verified beyond the standard suite: 36/36 verify-all after make clean
(incl. win .exe PPM byte-exact under wine + demo-chain legs that run
the migrated player through skip->menu->level), zero app leaks/UB
under ASan in single-file AND demo-chain runs, and a REAL-session
smoke (Xwayland :0 + real audio) with XTEST-injected keys proving
the whole input path: q -> "quit=1 (q)", Escape -> "ESC at frame 35
-> skipping 2351 chunks" (rc semantics untouched), and Space-pause
proven by sampling the LEO_IS_WAV dump: the audio feed froze at
282624 B for exactly the pause window, then resumed and the run
finished 125/125 (pause shifts the clock, never rewinds - P-02).

Next T-01 step (unchanged plan): migrate title menu / chapter map /
level scene the same way; they are the last direct ll_view/ll_audio
users in engine code. DONE in PR-16 (next section).

## PR-16 — remaining screens (DONE, phase 3 of T-01)

The three compositing screens (title dialog, chapter map, level
scene+help overlay) draw by building their frame on the CPU, so the
right interface call for them is plat_present_argb: the buffer they
just composited is uploaded verbatim (no palette, no conversion) —
which is exactly why their PPM pins could not shift. The menu's
mouse zones work through new LL_EV_BUTTON_DOWN/LL_EV_MOTION events;
x,y are LOGICAL viewport coordinates (the backend maps them, P-10 —
the /sc math stays deleted). Key handling went from SDL keysyms to
engine ids: printables (incl. space) pass through as ASCII — so the
ll_menu_zone.key contract (ASCII chars) is untouched — plus
LLKEY_UP/DOWN/LEFT/RIGHT/RETURN/HOME/END/PGUP/PGDN for navigation.
ll_level's paint() deliberately KEPT its LUT indirection (level
palettes come from the tlo= PCX, and some blocks have NO palette
donor: the documented gray-ramp fallback lives there); it expands
canvas+LUT into a persistent ARGB scratch and hands that over. The
music service (ll_mod + ll_audio pump thread) is NOT a screen and
was left on the SDL audio queue on purpose — one subsystem per PR;
that split is documented at ll_level.c's include (PR-17 would move
it behind plat_audio_* too, at which point ll_view/ll_audio go
fully backend-private).

Real-session proof (X11/Xwayland, :0): XTEST keys work as before;
the pointer is NOT testable there (kwin owns the Wayland-side
pointer, XTEST warps get overridden — measured), so xtest_click.c
sends synthetic core ButtonPress/Release with client coords with
SDL_VIDEO_X11_XINPUT2=0 (SDL's classic path processes them; past
SDL_PumpEvents the code path is identical to a device click). The
chain that passed: island click at (188,132) -> "map: island 0
launch (click)" -> chapter .IS -> ESC -> "skipping 385 chunks" ->
level "KADMAR2" -> p -> "autoscroll on" -> ESC -> "ESC -> advance"
-> back to map -> q -> "map: wyjście" -> exit rc 0. All five
migrated input surfaces in one run.

## PR-17 — music service (DONE, phase 4 of T-01)

The music singleton (MAPKA level-select + per-level tracks) was the
last engine code talking to SDL directly. It now uses the SAME audio
API the cutscene player already used (plat_audio_open/queue/
queued_bytes/close - queue-only devices, the samples=0 contract and
the P-13 accepted-spec log live backend-side), plus a NEW tiny
thread API: plat_thread_start(fn,ctx,name)/plat_thread_join. C11
<threads.h> was the obvious portable choice and is NOT available on
the vendored llvm-mingw (probe: fatal error, gcc has it); the
portable atomic flag IS (stdatomic on both toolchains, probed), so:
thread = platform's job, stop-protocol = engine's job (atomic_int
s_mus_quit). The SDL backend is the exact SDL_CreateThread call the
music code used to make (same ABI cast, same thread name, the
"pump thread failed (%s)" diagnostic verbatim via plat_last_error);
the headless dummy returns -1, which makes CI exercise the
documented UI-loop pump fallback (the pre-E-09 path) as the normal
path. low_water was rewritten as (size_t)LL_MOD_RATE = the same
250 ms value. Error mapping preserved: plat_audio_open -1 (subsystem
unavailable) -> silent play, exactly like the old failed
ll_audio_session_init path; -2 -> ll_music: device: %s. ll_level.c
is now SDL-FREE end to end (its SDL include had also been smuggling
in the std* headers - the std includes moved to the top of the file,
the stray mid-file include block deleted; behaviour unchanged).

Verified: verify-all 36/36 from clean, verify-music byte-pin
(tunga.MOD 2.25 s, silent=0 clip=0 peak=19259), platform_selftest
26/26 (+2 thread-contract checks: headless start must fail cleanly,
join safe when none ran), ASan/UBSan glob build links, and a REAL
X11+ALSA session run of the CI auto-chain under ASan: MAPKA.MOD
playing through the platform device with a REAL platform pump
thread (no fallback line), ll_music_stop joined it, chain rc 0,
zero UBSan errors, and zero app frames in the leak report.

Two environment lessons worth keeping:
1. timeout(1) + SDL2: SDL installs SIGINT/SIGTERM handlers, so
   `timeout N ./game` does NOT kill it - SDL turns TERM into an
   app-quit event, the game exits the current scene and keeps
   running the next screen; timeout without -k then waits forever.
   Real session runs must either pass the CI auto-hooks (so the
   chain ends on its own) or use `timeout -k`. A LSan-at-exit run
   also prints the leak report BEFORE stdio is flushed, so a
   killed-by-timeout ASan log can show ONLY stderr lines - do not
   mistake that for "the app hung at startup".
2. The ASan-in-real-audio leak report (3.4 KB/47 allocs) is ALL
   third-party one-time allocations in libasound + the pipewire
   ALSA plugin + sdl2-compat internals (zero app frames) - it
   appears only when a real device is touched; the dummy-driver
   legs remain leak-clean.

Workspace note: /tmp/opencode was swept mid-PR (iso_stage,
py_last, wineprefix, harness scripts gone). Restored: full CD from
the repo's LL.iso -> bsdtar -xf LL.iso LEO (=> /tmp/opencode/
iso_stage/LEO), HD overlay -> bsdtar -xf LL.iso LEO_HDD && mv
LEO_HDD hd_files, save fixtures cp STATUS.LEO LONGSOFT.CFG from
the repo root (they live there, gitignored on purpose - verify-
save SKIPs cleanly without them), last-frame oracle -> python3
tools/is_decode.py frames $(DATA)/INTRO_P.IS /tmp/opencode/py_last
--ppm --only 1520. Note: the hd_files save fixtures that existed
before were from another install (word63=5, drive='D'); the restored
repo-root ones decode with the SAME grammar (word63=4, drive='H'),
verify-save pins updated to the present files + provenance comment.
Side benefit: hd_files/LEO.EXE is the original DOS executable -
useful for the T-24 golden-master idea.

## PR-18 — app glue (DONE, phase 5 of T-01)

The last two non-backend SDL users were entry-code concerns: main.c
(base path for the DATA search, the P-06 error box, the demo title
hold, the Wayland video-driver pin) and ll_paths.c (GetPrefPath for
the per-user dir). Four new platform.h entry points cover exactly
those, nothing more: plat_base_path, plat_message_box, plat_pref_
dir, plat_prepare_video. Design notes:

- The P-06 "box only on Windows" policy moved INTO the SDL backend
  (that is a property of the host, not of the app code), so main.c
  calls plat_message_box unconditionally and the dummy is a no-op -
  the caller's stderr line stays the POSIX report.
- plat_prepare_video is the main.c block VERBATIM, including both
  byte-identical "main: Wayland session..." diagnostics and the
  "explicit SDL_VIDEODRIVER always wins" rule; called once at the
  top of main, same position as before.
- plat_pref_dir keeps GetPrefPath semantics (creates the directory;
  NULL/-1 -> ll_user_dir's documented LEO_USER_DIR-only fallback).
  Headless answering -1 is the honest answer and matches what the
  test harness wants.
- The --scale/--fullscreen display overrides were always an app ->
  window-backend channel; their ll_opt_* globals were declared in
  the backend-private ll_view.h. Renamed plat_opt_scale /
  plat_opt_fullscreen, declared on platform.h (next to plat_init),
  still DEFINED and consumed in ll_view.c. CLI surface unchanged.
- The demo auto-pick hold was SDL_Delay; it now derives from the
  one platform clock (plat_sleep_until) like everything else
  (T-05/T-07 clock ownership falls out for free).
- main.c's ll_view.h include was vestigial after PR-15 (it used no
  ll_view_* call); removing it + the SDL include makes the TU SDL-
  free, proven with cpp (0 SDL headers, 0 SDL_ tokens) for both
  files. The windows.h include for --console stays: that is the
  real Win32 API, not SDL.
- New CI leg verify-arch (compile-free structural pin): greps
  ported/ for SDL2 includes outside the allowed backend set
  (platform_sdl.c, platform_sdl/, ll_view.h, ll_audio.h) and names
  the offender if the invariant ever regresses. verify-all is now
  37 legs.
- platform_selftest +3: headless must answer "no executable dir",
  "no pref dir", and message-box/video-prep must be safe no-ops -
  the app-glue fallbacks then hold by construction (29 checks).

With this PR T-01 is COMPLETE: no screen, service, or entry file in
the tree talks to SDL; the only SDL is behind platform.h (plus
ll_view/ll_audio as backend-private helpers).

## PR-19 — T-24 golden-master rig + pins (DONE 2026-11-14)

- QEMU baseline rig, no-CD route, fully scripted:
  `tools/golden_build_rig.sh` restages from LL.iso + an MS-DOS 6.22
  boot floppy: C:\ root = the LEO_HDD files (patched LEO.EXE copy,
  DOS4GW.EXE, LONGSOFT.CFG byte0='C', STATUS.LEO, ANIMACJE, ETAPY -
  the HD-install copies, which DIFFER from the CD's), C:\LEO = the
  CD's 19 flat files (the game opens them as `C:\leo\...` via the
  CFG drive byte); data disk via `tools/build_data_disk.sh`
  (mkfs.fat -F16 + BPB geometry pin). Boot floppy CONFIG.SYS gets
  `FILES=40 BUFFERS=40` appended - DOS 6.22 defaults FILES=8 and the
  game then reports "Could not open resource file: C:\leo\sound".
  AUTOEXEC: `C:` then `LEO` (the retail LEO.BAT pattern). Verified
  live: gate passed, resources open, the .IS movie chain runs at the
  real 70 Hz audio-DMA clock (PIT ch0 0x4295).
- CD-gate patch FINAL: `0x5bd51:b801000000eb32` (see PLAN S6 - the
  0x5bd5c variants are NOT deterministic without ASPI; patching one
  byte earlier than the `push edx` breaks the `pop edx:ret` pair ->
  DOS/4GW error 06h).
- `tools/golden_qemu_run.py` drives the rig over the QEMU monitor:
  samples the BIOS tick at 0x46C at 0.25 s, waits for the 70 Hz
  movie clock (2 sustained high samples - a lone early sample is
  boot noise), then for the map-menu state (counter frozen >= 3 s,
  PLAN S6; the window is only ~7-8 s, so coarse sampling misses it)
  and captures the 320x200 linear index plane + doubled screendump,
  retrying the art<->black cycle until the frame is static
  (vram.bin/menu.ppm/static2.ppm + capture_manifest.json, hashes
  only in the repo - never frames, per T-24).
   NOTE: the map menu is CD-GATED - two scripted no-CD watches (871/931 s
   in PR-19, then 1500 s at 0.25 s sampling on 2026-11-15) never produced
   a frozen-40:6C window; the game stays in its 480-line movie loop. The
   frozen-counter state from PLAN S6 was measured with a CD attached, and
   the CD-attached route is now scripted too (golden_build_rig.sh CD=1 +
   golden_qemu_run.py --cdrom + `make golden_rig_cd`, see PR-21 below).
   The golden.* half of the manifest stays open until a real capture
   lands.
- `tools/golden_menu_cmp.py`: validates the capture against the
  verified layout model (pure 2x2 doubling, index->color bijection
  like menu_dac_gen), scores every port map frame against the
  original RGB + index plane, and asserts the committed manifest
  pins. Port-side pinning is a POSE SET, not per-file hashes: the
  chapter-map animation is a 30-frame cycle, so the set of distinct
  composed frames is deterministic while per-window counts are not
  (pinned in tools/golden_expect.json, 30 hashes, reproduced
  identically across runs).
- Port hook: `LEO_MENU_PPM_DIR`/`LEO_MENU_PPM_MAX` (ll_menu.c)
  dump the composed frames silently (verify-menu log pins stay
  clean).
- Makefile: `golden_rig` + `verify-golden` (SKIP-safe: no game
  data -> SKIP; no QEMU capture -> checks the port pose pins only,
  so the leg runs in ordinary CI). verify-all is now 38 legs.

## PR-20 — test-suite cold-run fixes + TEST_REPORT.md (DONE 2026-11-15)

- Cold-tree end-to-end run of verify-all (make clean first) for the report
  exposed two latent Makefile dependency bugs that stale worktree binaries
  had always masked: verify-win RUNS the linux twins ./ll_level and ./ll_save
  (frames it cmps against) but only listed the .exe twins as prerequisites,
  and verify-level's PCM-canvas check runs an undeclared ./pcm_selftest
  (masked by sibling ordering). Both now declared; the single-invocation
  verify-all is green from a cold tree (61 emitted proofs, 0 warnings on
  gcc + vendored clang). No product-code changes were needed.
- tools/golden_menu_cmp.py: numpy>=2.0 compatibility (np.unique's inverse
  is now same-shape; ravel + explicit int64 before bincount). Latent
  because the capture-compare path never ran in port-pins-only CI mode.
- TEST_REPORT.md: full-suite report (21-leg inventory, oracle methods, this
  run's evidence census, defects found+fixed, fixture bootstrap, gap list).
  Environment note: a 04:27 reboot wiped the tmpfs /tmp fixtures; the
  restage recipe above (LL.iso -> iso_stage + LEO_HDD -> hd_files + repo-
  root save fixtures + py_last frame) restored the suite unchanged.

## PR-21 — golden rig: input + dual state detectors (the menu is NOT CD-gated) (DONE: the capture landed, see its own record and PR-37/PR-40)

- The no-CD route is DEFINITIVELY negative for the map menu under attract:
  a second scripted watch (1500 s, 0.25 s sampling = 6000 samples) again
  showed zero frozen-0x46C windows and the game stayed in its 480-line
  movie loop (movie clock 72 -> 88 -> 107.9 Hz across phases; same
  signature as PR-19's 871 s/931 s runs). The timeout.ppm screendump is
  a 480-line .IS movie frame and timeout_text.txt is EMPTY -> the game
  is in graphics mode, not waiting at a prompt.
- The CD-attached route per PLAN S6 line 287 ("the -cdrom+UDVD2/MSCDEX
  rig from S6 with the 0x5bd51 patch instead of the ASPI-dependent NOP")
  is now scripted:
  - tools/golden_build_rig.sh CD=1 CDDRV=... mode: CFG drive byte stays
    'D' (retail semantics - the game reads D:\LEO\... from the emulated
    CD), the qcow2 is rebuilt on a drive-mode switch (hd_mode stamp),
    and the boot floppy gains a real-mode ATAPI CD driver + MSCDEX line
    (MSCDEX.EXE is on the MS-DOS 6.22 startup disk itself; the driver is
    OAKCDROM.SYS - UDVD2 mirrors are unreachable, OAK is freely
    redistributable and is what DOSBox-Staging ships; same role).
    The 0x5bd51 patch STAYS: OAK brings no ASPI manager, and the
    pristine gate would otherwise fail its AH=4408h/4409h probe chain.
  - tools/golden_qemu_run.py --cdrom IMG: attaches the ISO to ide cd0
    and records its sha256 in capture_manifest.json inputs.
  - Makefile: `make golden_rig_cd` (CD=1 rig + --cdrom capture).
- The CD-attached rig WITHOUT input ALSO stays in the movie loop
  (1500 s, 2026-11-15 06:15): CD presence alone is NOT sufficient. The
  S6 baseline where the menu chain was live describes HUMAN INPUT
  ("intro FMV plays, Esc opens the menu") - so the scripted detector now
  injects keys through the QEMU monitor too: golden_qemu_run.py
  --sendkey-at "T=KEY,..." (sendkey esc/spc/ret/down at wall-clock T,
  recorded in the manifest phases). First Esc visibly changes the run's
  phase progression, so the input path reaches the game.
- DISK-D ROUTE per the user's rule "data loads from a directory, without
  mounting": the rig's D: is a plain second FAT disk (build_data_disk.sh
  without ROOT_DIR: \LEO\ at the volume root, attached -hdb) - no CD
  medium, no MSCDEX, no device driver anywhere. golden_build_rig.sh
  DDIR=1 + golden_qemu_run.py --ddisk + `make golden_rig_ddir`. This is
  the control that separates "the original needs the CD medium" from
  "the original only needs its D:\ paths to exist as an ordinary
  directory". If the menu appears here, the golden capture runs on a
  fully CD-free rig - matching the port's own no-CD design.
- Three full runs completed 2026-11-15 (sparse Esc/Ret/Down/Space
  schedule at t=75..1306 s, identical for CD and disk-D):
  - CD + keys, 1500 s: stays movie-loop, stuck at 72.0 Hz phase.
  - disk-D + keys, 1500 s: same, re-phases to 87.9 Hz after Esc+spc.
  - disk-D + dense (Esc every 25 s + Ret/Down, 900 s): re-phases to
    84.0 Hz, then 100.0 Hz after t≈560. So keys DO reach the game and
    change the movie chain's phase, but NO run produced a frozen-0x46C
    window; the timeout frames are all live 480-line movie art.
  - PROBE-SHOT FINDING: every screendump taken +0.6 s after a key shows
    a pure-black frame - a transition/fade, and black is NOT menu proof
    (PLAN: menu entry/exit fades are black too). A single delayed probe
    per key measures nothing; the a/b/c (+0.6/+3/+9 s) multi-shot mode
    plus an observe-only "noop" event were added for the next round.
- TWO DETECTOR BLIND SPOTS found and FIXED in golden_qemu_run.py:
  1. the a/b/c probe series used to BLOCK the sampling loop ~10 s per
     event - a 7-8 s menu freeze fit exactly inside that hole. Shots are
     now scheduled as due-events inside the loop (the QEMU HMP serves
     one client - a second monitor connection hangs, tested QEMU 11.1.3,
     so threading was not an option either).
  2. the ONLY menu trigger was the frozen-0x46C counter. Per the PLAN's
     own hunt12 measurement the menu is ONE STATIC ART FRAME ~0.9 s on a
     400-line canvas, and the .IS chain is always 480-line: a frame
     sensor is independent and immune to counter doubts. --frame-interval
     (default 0.5 s) now screendumps+classifies every sample:
     frame_journal.jsonl logs every frame CHANGE (crtc height, colors,
     mean, hash); non-480 frames are kept as frame_T.ppm; sustained
     400-line ART (>= 1 s, colors > 8, mean > 16) triggers the same
     capture path as the frozen counter (capture_menu_state validates
     the static index plane before anything is written, so a fade never
     becomes a false capture).
- STATUS AFTER 7 SCRIPTED RUNS (L1-L7, 2026-11-15): the driver now
  speaks QMP (fix 2 above) and classifies every frame. Findings:
  * EVERY early Esc (~8-70 s after game start on the disk-D rig) flips
    the CRTC 640x480 -> 640x400 for ~1 s and back (blips at t=9/15/
    21/31/46/71 in L7, same pattern in L1/L2): the game DOES enter its
    400-line canvas on input; its own attract logic drops it back into
    the movie chain ~1 s later.
  * In L1 one episode STUCK: 640x400 held t=46.2 -> 70.8 (~24 s, the
    black half of the menu's art<->black cycle), and the next Esc
    produced a 640x400 ART frame (colors=187, mean=52) at t=71. That
    matches the live-verified menu (PLAN line 255): in the frame
    domain this canvas is only ever used by the menu state.
  * No vram.bin capture has landed yet: L1's stuck episode happened
    under the older broken-HMP driver whose trigger also required ART
    while the menu's longest phase is black. L7 ran the final detector
    code and logged all six blips correctly, but none held 2 s.
  * 0x46C never froze in ANY run, menu-like states included: the clock
    detector is NOT reliable on this rig; the frame detector is the
    primary and stands on its own.
- CAPTURE LANDED (L10+L12, 2026-11-15, scripted disk-D rig, single
  Esc@30): the ART frame ITSELF is the trigger (the menu shows it
  ~0.6 s per appearance - any 'hold >= 2 s' gate can never fire;
  screendump content-hash b880e203 byte-identical across 5
  observations, twice spontaneously). The 320x200 index page is at
  VRAM offset 0x20000 (read_vram scans LFB pages + A0000 and takes
  the page with real 8bpp content; NOT vram+0 as the hunt13-era
  header implied - display start is set). Stopped-pair reads are
  IDENTICAL, the comparator's index->color bijection holds with ZERO
  violations, and both runs' artifacts are byte-identical: golden.*
  is now PINNED in tools/golden_expect.json (idx-plane sha
  046ee42c..., 161 indices; orig-rgb reconstruct ea0787f6...; full
  window 14e15fb2...). The captured instant is the menu's near-black
  composed half - the menu FLIPS COMPOSED PAGES (art / near-black)
  via display-start, not a palette fade; 26/26 palette entries
  visible at that instant match ll_menu_dac.h exactly, independently
  confirming the PR-19 DAC recovery.
- PR-23 CLOSED THE BACKGROUND (2026-11-15): tools/menu_art_gen.py
  (refuses non-golden pages) -> ported/ll_menu_art.h (authentic index
  plane, 4020 lines); map_bg paints it through ll_menu_dac.h by
  DEFAULT, LEO_MENU_BG=placeholder = old stand-in; pose pins
  regenerated (still 30); min_best_rgb SET 98.0, Makefile GOLDEN_MIN
  default 98 (measured 98.43). Residual is composed-text/overlay
  placement vs the original's burned-in texts - that is the remaining
  (small) path up the golden scale; background is DONE. Capture
  re-proved deterministic after a full /tmp wipe (fresh rig build ->
  bit-identical 4cb7a311).
- PR-24 CI (2026-11-15): .github/workflows/ci.yml runs on push/PR to main:
  ubuntu-latest installs build-essential/libsdl2-dev/mingw-w64 (gcc+binutils
  for windres)/cmake/python3, builds BOTH targets (vendored toolchains are
  gitignored, system fallbacks used), compileall's tools/, then
  'make -k verify-all'. Makefile now defines parse-time fixture probes
  (PX_DATA = $(DATA)/INTRO_P.IS present; PX_HD/PX_SAVE = $(HD) save
  fixtures; PX_WINREF = data + c_py + py_last oracle frames; PX_CMAKE)
  and ifeq-wraps every fixture-backed leg - a bare runner prints a named
  SKIP with the exact staging command and runs nothing stray (fixture-free
  verify-all: exit 0). Recipe-level '@if ... exit 0' guards DON'T work for
  this (make runs each recipe line in its own shell - the rest of the
  target still executes); the parse-time wrap is what makes skips clean.
  HD was moved next to DATA (probes need it defined early). verify-mod and
  verify-platform gained PX_DATA wraps (they take $(DATA) too). The
  full-fixture local run is unchanged (exit 0, all legs real).
- MEASURED GAP (the point of T-24): port best = 0.000% RGB/idx
  against the captured original frame. The port composes map-menu
  OVERLAYS (arrow/words/titles, the pinned 30-pose animation stays
  green); the original's page is one static full-landscape art
  frame. Composing that background is the next port work item - and
  it is now measurable to the byte.
- ART-PAGE CAPTURE LANDED (A4+A5, 2026-11-15, --want-art mode,
  byte-identical): golden.* is now the MAIN LANDSCAPE frame itself -
  screendump sha1 b880e203ce52, idx-plane ba16421b... (= EXACTLY the
  hunt13 bytes that produced ll_menu_dac.h - independent recovery),
  209 indices, zero bijection violations, full window 4cb7a311...,
  orig-rgb c53d04ab.... The menu flips display start between two
  pages: art at VRAM +0x0000, near-black overlay half at +0x20000
  (its hashes kept as golden_black_page_info, informational). Recipe:
  same run + --want-art (try-loop: sendkey esc keepalive -> stop ->
  screendump -> frame must hash b880e203/2c4c0583/4b905f7e -> page
  must reproduce it exactly). The 2c4c0583 second-landscape variant
  alternates in the cycle; its page was never requested (qualify it
  there if ever needed). Port best vs the ART frame: 0.43% RGB /
  6.4% index (was 0% vs the near-black half).
- VERIFY-GOLDEN BEHAVIOUR: CI stays green (no capture in $(GOLDEN)
  -> port-pins-only mode). With a capture present it asserts
  golden.* and then --min-match $(GOLDEN_MIN) vs port poses ->
  currently RED by design (best 0%): that is the target signal, not
  a regression. Caveat recorded in the manifest note: one re-run
  produced a different page variant (L11 62d58767...) - the VM stop
  can land after the game re-composed - so a differing
  vram_full_sha256 on a manual re-capture is not automatically a
  regression.

## PR-25 — FIX-01: quit propagation (DONE 2026-11-15)

- Deep dive Part B FIX-01 / X-01: closing the window only ended the
  CURRENT .IS/level; main happily opened the next one (menu reopened,
  several closes needed to quit; the P-04 raise-on-close symptom).
- Design (per Part B ruling): FLAG ONLY - no new player return code, all
  existing 0/1/2 + menu codes unchanged. platform.h gains
  plat_quit_requested()/plat_request_quit(): sticky, never cleared.
  SDL backend latches on SDL_QUIT and on WINDOWEVENT_CLOSE (belt and
  braces; not surfaced - LL_EV_QUIT contract untouched). Dummy backend
  latches when LL_EV_QUIT dequeues - same contract, scriptable via
  plat_dummy_inject.
- main.c glue unwinds on the flag: demo chain break, menu entry skipped,
  menu loop top-of-loop break, chapter .IS -> level guard, CLI --then
  chain + --menu guard. Screens still END through their own paths; only
  the NEXT screen is skipped.
- New test hook: LEO_QUIT_AFTER_MS=N (platform_sdl) pushes a real
  SDL_QUIT N ms after the FIRST poll. verify-quit (PX_DATA-wrapped):
  headless --demo run must exit 0 in <10 s with NO 'menu:' line;
  measured: LOGO.IS ends at frame 13 (quit=1), chain unwinds, clean 0.
  platform_selftest +3 checks (no flag before QUIT; QUIT latches sticky;
  survives polls / plat_request_quit idempotent) - 33 checks total.
- Same-shape CI behaviour verified fixture-free (named SKIP). Full local
  suite stays green; .exe cross-build clean under vendored clang.

## PR-26 — FIX-02/03/04/05: window hygiene bundle (DONE 2026-11-15)

- FIX-02 (X-03, P-04 regression fix per ruling C8): the window is now
  raised EXACTLY ONCE at creation (ll_view_create). Removed: the
  WINDOWEVENT_ENTER raise in ll_view_handle_event, the raise on every
  scene-switch in ll_view_create's else branch, and the
  SDL_RaiseWindow+SDL_SetWindowInputFocus body of plat_raise (the
  hard X11 grab was the taskbar-flash offender). plat_raise stays in
  the API as a documented no-op (callers untouched, selftest still
  calls it); platform.h + the four stale "re-raises on pointer-enter"
  comments (ll_menu x2, ll_level x2) updated to the raised-once rule.
- FIX-03 (X-04, no scancodes per ruling C4): ll_event gains a tail
  field `repeat` (1 = OS auto-repeat). SDL backend fills it from
  e.key.repeat; dummy needs no change (structs pass through - proven
  by new selftest check, 34 checks total). All 5 existing ll_event
  initializers got explicit ', 0' (CFLAGS carry -Wextra). Gates:
  ll_view consumes F11/Alt+Enter repeats (toggle ONCE, must return 1
  so a repeated Alt+Enter cannot leak into the engine as RETURN);
  is_player ignores repeats outright (q/ESC/space are all toggles);
  ll_menu dialogs filter repeats except arrow NAV cycles; map screen
  filters after the arrow chain; ll_level filters ESC/q and the 'p'
  autoscroll toggle but keeps arrow scrolling; help page ESC is
  first-press-only (was: holding ESC closed help AND escaped the
  level). Arrows keep auto-repeat everywhere.
- FIX-04 (X-05): platform_sdl present_last() helper = the former
  identical Clear/Copy/Present tails of plat_present+plat_present_argb
  (single implementation now, pixels untouched); called after
  ll_view_handle_event hits (fullscreen toggle) and on EXPOSED/
  SIZE_CHANGED/RESTORED - the drain's window handling merged into one
  switch with the FIX-01 CLOSE latch. Pause + F11/resize now keeps
  the frame visible instead of black.
- FIX-05 (X-07/W-02): ll_audio logs the REAL accepted spec - fmt_name()
  switch over AUDIO_* values (no *SYS alias cases: duplicate-value
  error) + SDL_GetCurrentAudioDriver(). Measured: 'device S16LSB
  22050Hz 2ch 1024 samples (driver dummy)' / '(driver alsa)' - the
  old always-'?' line is gone. Comment fixed: SDL 2.32 Windows
  default is wasapi, not directsound. fmt_name lives at file scope
  (C: no nested functions - the deep dive's snippet was illustrative).
- verify-quit/verify-sel/verify-platform/verify-arch green locally;
  full suite + fixture-free sim run next as always.

- PR-26 pushed 656dbc5; CI green.

## PR-27 — FIX-12: CI hardening (DONE 2026-11-15)

- ci.yml only + one small Makefile variable (WINDRES ?=, same fallback
  chain as XC; the .res rule now uses it so a native MSYS2 build can
  point it at plain 'windres').
- linux job: the SDL2 mingw tarball download is now sha256-PINNED
  (83a5d740...38ff2, computed from the exact official libsdl.org URL;
  a deliberately broken pin fails the job - acceptance criterion).
  Both target sets are uploaded via actions/upload-artifact@v4 (name
  openleo-<sha>, if-no-files-found: error). All 19 artifact names are
  built by `make`/`make win`/verify-leg dependencies even when legs
  SKIP - verified against the PR-26 CI log's target list.
- NEW windows-smoke job (windows-latest, msys2/setup-msys2@v2 MINGW64):
  first-ever NATIVE Windows build + RUN of the headless selftests
  (platform_selftest is SDL-free by the T-01 design, so make
  platform_selftest llrs_selftest pcx_selftest seq_selftest
  seq_patchdump pcm_selftest mod_selftest compile natively; the rules
  hardcode their flags - no $(CFLAGS) dependence - and use
  -DPLAT_BACKEND_DUMMY=1), then RUN them for real against the
  repo-committed fixtures: platform_selftest defaults to data/INTRO_P.IS
  (in git), llrs_selftest over data/{ANIMACJE,ETAPY,SOUND} - all exit
  0 locally first. platform_selftest is an ASan build, so this job is
  also native-Windows sanitizer coverage. Then the REAL ll_cutscene.exe
  builds natively (XC=gcc WINDRES=windres SDL_WIN=/mingw64 reuses the
  cross rules) and smoke-runs from an EMPTY dir with dummy drivers:
  expects exit 2 + 'usage:' on output (proven locally against the
  cross-built .exe: '[leo] no DATA found...' + usage + rc=2). NOT
  --help until FIX-13 lands (X-12: treated as FILE.IS today, and
  fatal_msg would open a blocking message box); the YAML says to flip
  it to --help/exit-0 in PR-32.
- NEW asan job (ubuntu): CFLAGS-override ASan+UBSan build of
  ll_cutscene ll_level ll_save + all tool selftests (O1 -g -fsanitize
  as the fuzz leg composes them), then the REAL runs over committed
  fixtures with ASAN_OPTIONS=detect_leaks=1:abort_on_error=1
  UBSAN_OPTIONS=halt_on_error=1 + SDL dummy drivers (ruling C7's
  documented leak-clean config). Replicated locally: zero warnings,
  zero leaks/UB.
- Deviations from the deep-dive spec (recorded for the owner): the
  spec's 'make verify-platform verify-fuzz verify-llrs/pcx/seq/pcm/
  mod' under asan/windows jobs would print all-SKIP (those legs are
  PX_DATA-wrapped on $(DATA) staging); running the built binaries
  against the COMMITTED data/ fixtures gives the real assertions
  instead. Extending the data-backed legs to the committed containers
  needs nested-container restaging machinery (fuzz over data/ exits 2:
  'pcx corpus' needs extracted trees) - deliberately NOT built now.
  Optional macos-latest build-only job: not added (spec marks it
  optional; one more runner minute + brew volatility for build-only
  signal). Revisit at series end if wanted.
- Local gates: full verify-all exit 0 + fixture-free sim exit 0 after
  the WINDRES change; the two new jobs run their exact steps locally
  first (asan fully; windows partially - the .exe smoke executed via
  the local binfmt path, real-MSYS2 proof lands with this CI run).
- CI green run 43717359504 (PR-27b 4618d61): linux, windows-smoke
  (first native Windows build+run in repo history) and asan all ok.
  PR-27 CLOSED.
- CI iteration (first run 43717009882: linux ok, both new jobs failed,
  exactly the expected first-run surface):
  1. windows-smoke: MSYS2 mingw-w64 gcc (16.2) ships NO libasan/
     libubsan ('cannot find -lasan' on every selftest link). Fix: the
     Makefile's 8 hardcoded -fsanitize flags became SAN ?= -fsanitize
     =address,undefined (same override pattern as WINDRES ?=), and
     windows-smoke builds with SAN= (plain native builds; sanitizer
     coverage stays on the asan job - honest trade-off, not a loss of
     the job's actual point: native toolchain build + real runs).
  2. asan: exit 127 - the build list omitted platform_selftest (it
     existed locally from earlier builds, not on a fresh checkout).
     Fix: added to the make target list (its own rule always keeps
     $(SAN)).
  3. Hardened proactively: the .exe smoke now copies libwinpthread/
     libgcc_s_seh/libstdc++ DLLs alongside libSDL2 (the official
     libsdl.org SDL2.dll imports only system DLLs - verified by
     llvm-objdump -p - but MSYS2's own build may differ).
  Locally after the change: plain path (make -B SAN= <7 tools>,
  nm asan count = 0, all real runs green) AND default path (nm asan
  counts 24/19/27 on platform_selftest/llrs/fuzz, runs green) both
  replicated; YAML parses.

## PR-28 — FIX-06+07: timing core (DONE 2026-11-15)

- FIX-06 (platform_sdl.c only; platform_dummy untouched per spec):
  plat_now_ns is now exact integer math ((c/f)*1e9 + (c%f)*1e9/f; safe
  for f <= ~1.8e10, comment proves the bound) - the old double path
  lost precision. plat_sleep_until rewritten: OS-sleep until 300 us
  before the deadline, yield-spin the rest (old: SDL_Delay(2) coarse
  steps + SDL_Delay(0) spin for 2 ms of every tick). os_sleep_ns:
  Win32 high-res waitable timer (CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
  #ifndef-guarded; per-call handle = THREAD-SAFE - the ll_level music
  thread sleeps through this), falling back to SDL_Delay on pre-1803
  Windows; POSIX nanosleep (EINTR: caller loop re-checks). windows.h/
  time.h included locally in the #if _WIN32 block - verify-arch still
  ok (it only pins SDL includes; windows.h allowed by the spec).
  Cross-builds zero-warning with vendored llvm-mingw (headers fine;
  CI's Ubuntu mingw gets the #ifndef guard for the newer constant).
- FIX-07 (ll_level x2 loops, ll_menu x2 loops): level + help loops
  moved to a fixed 20 ms t_tick grid (resync if >100 ms behind, never
  fast-forward); dt now derived from an absolute ms clock (t_prev_ms)
  - no per-frame truncation loss; autoscroll uses an integer sub-px
  accumulator (scroll_acc += 50*dt; cam += /1000; %=1000) instead of
  the lossy cam += 50*dt/1000. Menu + map loops: outer tick loop gets
  the 30 ms t_sched grid (t_next = t_sched; inner wait loop and all
  hooks untouched); TICK_HZ in ll_config.h documented INTENTIONALLY
  UNUSED (option 2 of the spec; loops stay on measured 20/30 ms).
- Measured acceptance (all hit):
  * level: 500 ticks/10 s = 50.0 Hz, autoscroll px=499 (needs 500+-5)
    - IDENTICAL under full-CPU load (16 yes-spinners).
  * map: 334 ticks/10 s = 33.4 Hz (needs 33.3+-0.3).
  * idle map-screen CPU: 370 ms -> 100 ms per 10 s = 1.0% of a core
    (needs <=3%). Cutscene-run CPU also dropped 7.1 s -> 0.9 s per
    LOGO play (music-thread sleeps were the hog).
  * LOGO wall-clock byte-identical to before (10532 ms both runs,
    same probe script); LEO_IS_WAV sha identical (cmp ok) - audio
    master clock untouched.
  * Instruments were temporary (guarded LEO_TIMING_LOG block + px/
    tick counters) and are REMOVED - git grep shows no residue.
- Tick-keyed hooks re-checked directly: verify-level passes incl. the
  drift-sensitive 'tick 8: ED000002.SEQ state 1' grep and the
  tick2!=tick8 animation proof; verify-help, verify-sel, verify-quit,
  verify-platform, verify-arch all green before the full suite.
- PR-28 pushed 42f3703; CI green run 43718104724 (all 3 jobs). CLOSED.
## PR-29 — FIX-08 cutscene A/V sync (DONE 2026-11-15)

- Step A (is_player.c ONLY; no new plat_* - plat_audio_queued_bytes already
  existed in both backends + selftest; platform_dummy untouched):
  enqueue_audio split into count_region (hold-time accounting, runs at
  present time) + byte-feeding (runs on the look-ahead). queue_ahead()
  keeps >= 150 ms queued (AUDIO_LEAD_NS) with a monotonic ci_audio
  pointer = every live chunk queued exactly once, in order; the CURRENT
  chunk is always topped up first; topped up again from wait_until's
  loop (never while paused - device held, nothing drains). ESC/skip
  sets ci_audio = nchunks (plus the existing plat_audio_clear). Legacy
  period modes + no-audio keep the old exact-one-chunk branch verbatim.
- Silent-chunk fill (found by review, needed by the data): chunks with
  no ESC regions hold video for rep ticks at 70 Hz; the stream now
  queues exactly rep*bps/70 zero bytes there (rep = st.repeat ?: fps
  fallback - same expression as the video hold; 44100/70=630,
  88200/70=1260 exact). This fixes a LATENT audible bug: in the old
  model silent holds were absent from the stream, so ALL audio after a
  silent chunk played early vs video (INTRO_P has a designed 1.786 s
  pause at 192.694 s; LOGO 171 ms and AUTORZY 71 ms silent intros).
  Without the fill, Step B would also have misread intentional pauses
  as video being ahead and compressed the playback after them.
- Step B: after sched_ns += hold, audio position = q_total - queued;
  err = v_ns - a_ns - av_latency (46 ms default = one period at 22050
  Hz, LEO_AV_LATENCY_MS override); if |err| > 20 ms correct half the
  err capped +/-10 ms via explicit signed wraparound write to t0_ns;
  gated off while paused (pause_shift keeps v/a locked across resume -
  no error spike by construction).
- Diagnostics (additive): LEO_AV_DEBUG=1 -> 'is_player: av
  min_queued=%ums err=%+dms' once per second.
- Measured acceptance (real ALSA device, dummy video):
  * Before/after LEO_IS_WAV vs pristine 42f3703 binary (kept as
    /tmp/opencode/ll_cutscene_base): LOGO/INTRO_P/AUTORZY differ ONLY
    by inserted zero runs at exactly the silent-chunk positions
    (zero_diff.py: every content sample identical; insertions
    3780/39375/1575 samples, ZERO deletions). Pre-existing wavs
    byte-identical for content.
  * Full INTRO_P with LEO_AV_DEBUG: 185 per-second samples,
    min_queued min 152 ms avg 200 (accept >=60); err range [-42,+26]
    mean -1.1 ms; first sample (-42 at the 1 s boundary) is the
    device start-up transient, inside the +/-20 band by t~2 s; no
    spike around the 1.8 s beat (max +21); 1521/1521 presented, no
    desync. 46 ms default latency confirmed correct in steady state.
  * Skip path WITH audio (--skip-at 10): identical skip decision both
    binaries, skip-target frame PPM byte-identical; new wav is a
    strict prefix of base + 3150 B extra lead, all of it discarded by
    the existing audio clear -> no stale audio beyond the bounded lead.
  * Pause/resume: correct by construction (device holds queue -> lead
    intact; pause_shift offsets v/a together; correction gated off).
    Interactive key-press check deferred to the handheld/C1 session.
- Regression: full verify-all exit 0 (only by-design no-regression
  SKIP; golden 98.4% unchanged); fixture-free sim exit 0, 22 named
  SKIPs; verify-skip byte-exact frame unchanged (uses --no-audio =
  legacy branch); spot legs verify/verify-sel/verify-menu green;
  zero-warning native + .exe builds.
- Method notes: (1) rebuilding while a background capture loop execs
  ./ll_cutscene per movie MIXES binary versions mid-capture - capture
  from a pinned binary copy (ll_cutscene_base pattern) or rebuild
  before starting; (2) LEO_IS_WAV files are RAW s16 LE despite the
  extension (ffplay -f s16le -ar 22050).
- PR-29 pushed 79961aa; CI green run 43720858381 (all 3 jobs). CLOSED.
## PR-30 — FIX-09 Part 1 + FIX-10 Part 1 + W-04 tail (DONE 2026-11-15)

- FIX-09 Part 1 (ll_audio.c only): the pipewire->alsa pin uses
  SDL_SetHint(SDL_HINT_AUDIODRIVER) instead of SDL_setenv (no leak into
  child processes; the old "restore" path even left drv=pipewire in the
  ENV - gone), skippable with new LEO_KEEP_PIPEWIRE=1, and a static
  s_forced_alsa lets ll_audio_open retry a failed device open ONCE on
  SDL's own choice (spec-shape block verbatim; safe under the
  one-device contract). Spec-mandated source check done against
  libsdl-org/SDL release-2.32.10 (tag name for 2.32.10 is
  release-2.32.x, not SDL2_2_32_x): SDL_AudioInit reads the HINT
  (src/audio/SDL_audio.c:969) and SDL_GetHint returns the ENV VAR value
  unless the hint is SDL_HINT_OVERRIDE (src/SDL_hints.c:159-177) while
  SDL_SetHint is non-override, and SDL_HINT_AUDIODRIVER ==
  "SDL_AUDIODRIVER" (include/SDL_hints.h) - user pins still win.
  Comment corrected: the '2.32.74' seen in the original investigation is
  sdl2-compat over SDL3 (squeak likely the compat resampler); FIX-09
  Part 2 (device at native rate, C1) is expected to make the pin
  unnecessary - re-test before deleting.
- Measured on the real desktop: default -> pin + alsa (as before);
  LEO_KEEP_PIPEWIRE=1 -> driver pipewire, movie plays; user
  SDL_AUDIODRIVER=dummy -> honoured verbatim; fallback forced with a
  temporary env-hook at the first open (instrument removed, git grep
  clean): 'alsa open failed (...), retrying with default driver' ->
  device reopens on the default driver -> 125/125 presented. ALSA here
  is shared-mix so the natural failure (busy device) can't be forced
  locally - instrument stands in for it.
- FIX-10 Part 1 (W-01): fs_ci.c wopen_utf8 = heap utf8_to_wide +
  _wfopen per spec (the fixed wchar_t[4096] silently truncated long
  paths and defeated the longPathAware manifest; MB_ERR_INVALID_CHARS
  now rejects invalid UTF-8). main.c: GetACP()!=CP_UTF8 warning right
  after the P-06 console block (fires only pre-1903 or manifest-less:
  resources/app.manifest already carries activeCodePage=UTF-8 +
  longPathAware, verified). README states the baseline: Win10 1903+,
  ASCII-only paths older. Part 2 (wide stat/dir wrappers) stays
  optional per spec - not a pre-1903 support goal.
- W-04 tail (CMakeLists.txt): post-build copy of SDL2.dll next to
  ll_cutscene.exe - for imported-target configs only when TYPE is
  SHARED_LIBRARY (vcpkg-static unaffected), for the pkg-config path
  (vendored mingw-xc SDL) by probing ../bin/SDL2.dll beside the lib
  dir; WARNING when no DLL is found instead of silently shipping a
  broken exe. verify-cmake windows cross-build now produces
  ll_cutscene.exe + SDL2.dll together and the wine proof is
  byte-exact; the Makefile's manual cp becomes a no-op fallback.
  Manifest half of W-04 was already done (P-14b).
- Regression: verify-utf8 (Gry/Lódź dir incl. win .exe byte-exact),
  verify-win all wine legs, verify-platform, verify-music green; full
  verify-all exit 0 (only by-design no-regression SKIP, golden 98.4%
  unchanged); fixture-free sim exit 0 with 22 named SKIPs; zero-warning
  native + llvm-mingw builds.
- PR-30 pushed f2bfbc4; CI green run 43722275109 (all 3 jobs). CLOSED.
## PR-31 — FIX-11 Wayland policy: LEO_FORCE_X11 hook, default = SDL's choice (DONE 2026-11-15)

- plat_prepare_video: the X11/XWayland pin now runs ONLY with
  LEO_FORCE_X11=1 (strict strcmp "1"); conditions + both log lines
  byte-identical. Default: no forcing, SDL chooses. Explicit
  SDL_VIDEODRIVER wins first (early return). Comment records the
  verified reason: the old "never get keyboard focus" claim was about
  PROGRAMMATIC raising, removed by FIX-02; classic SDL2 2.32 orders
  X11 BEFORE Wayland in the src/video/SDL_video.c:74-83 bootstrap
  table (verified against release-2.32.10),
  so on classic SDL2 the default still lands X11 when DISPLAY is set;
  the flip matters where SDL3/sdl2-compat prefers Wayland.
  main.c + platform.h comments updated; README documents the hook (plus
  gathered the other runtime policy hooks: LEO_KEEP_PIPEWIRE, LEO_AV_*).
- New diagnostic (backend-private, verify-arch-clean): ll_view_create
  logs once per process 'll_view: window WxH (driver %s)' - the
  FIX-11 acceptance needs SDL_GetCurrentVideoDriver reported, and there
  was no video startup line before (mirrors the 'll_audio: device'
  pattern). ll_view.c gains its own stdio.h include for it.
- This dev desktop IS a live KDE/Wayland session (WAYLAND_DISPLAY=
  wayland-1 + XWayland :0) and links system SDL 2.32.74 = sdl2-compat
  (same version as the FIX-09 finding) - so the flip is a REAL change
  of behaviour here, and acceptance was run for real:
  * default: 'll_view: window 640x480 (driver wayland)', no pin line,
    LOGO plays, window-close quit clean.
  * KEYBOARD WITHOUT CLICK (the core claim): a /dev/uinput synthetic
    key (~3 s after launch, /tmp/opencode/uinput_key.c) reached the
    native Wayland window with no user interaction: quit=1 (q). The
    pre-FIX-11 rationale for the pin is refuted on this compositor.
  * LEO_FORCE_X11=1: byte-identical old 'main: Wayland session
    detected...' line + driver x11. SDL_VIDEODRIVER=x11 honoured too.
  * 150% sharpness eyeball: PERFORMED by the owner 2026-11-15 - full
    LOGO run on the native path at the owner's 150% scaling
    ('window 640x480 (driver wayland)', presented=125/125), verdict
    SHARP. PR-31's acceptance list is now fully closed.
- Regression: full verify-all exit 0 (only by-design no-regression
  SKIP; golden 98.4% unchanged; the suite's own real-window leg now
  shows 'driver wayland' and passes unchanged); fixture-free sim exit
  0, 22 named SKIPs; verify/quit/menu/arch/platform spot legs green;
  zero-warning native + llvm-mingw builds.
- PR-31 pushed 45fbd26; CI green run 43723272576 (all 3 jobs). CLOSED.
## PR-32 — FIX-13 robustness + CLI polish (X-11, X-12, B-02) + smoke flip (DONE 2026-11-15)

- Item 1 (.MOV hang, X-11): obj_advance for(;;) got the spec's
  unsigned guard/4096 break. Investigation note: the deep dive's
  "all-zero ms cycles forever" shape is ALREADY clamped at load by
  mov_parse's ms>0?ms:100u rule (M4-C, git-log-verified) - the guard
  is defense-in-depth for any future loader/patch path, exactly what
  the spec's alternative ("clamp OR bound") asks to guarantee.
  Selftest per spec: ll_mov_selftest() exported via ll_level.h
  (test-only, same tool-facing precedent as ll_level_anims), called
  by 'll_level --mov-selftest': (a) pins the parser clamp (zero and
  negative -> 100, positive kept), (b) builds an all-zero-ms mov_t
  DIRECTLY (bypasses the clamp) and asserts obj_advance RETURNS.
  Wired: verify-level line runs BEFORE the PX_DATA condition (data-
  free, runs in the fixture-free sim too, wrapped in timeout 30) and
  the asan CI job (ll_level already built there). META-PROVED not
  vacuous: recompiling ll_level.c with the guard compiled out makes
  the selftest hang (killed by timeout, rc=124). Local ASan/UBSan
  build of the exact asan-job config: clean, exit 0.
- Items 2-3 (buffering order): --console block (now + setvbuf _IONBF
  after the freopens) + the FIX-10 GetACP warning moved as a unit to
  directly after the arg loop, before the T-17 save block; main()
  starts with setvbuf(stdout, _IOLBF, BUFSIZ). Windows A/B under
  wine with bad LEO_SAVE: OLD binary (HEAD build) leaks 'll_save:
  ... not loadable' into the pipe despite --console (the exact X-12
  bug: printed before attach); NEW binary leaks NOTHING app-wise
  (everything, save lines included, goes to the attached console).
  The literal acceptance ('shown from cmd') needs an interactive
  console; the A/B proves the ordering half deterministically.
- Item 4 (CLI surface): ONE usage_text at file scope (extended with
  --skip-at/--help/--version; 'usage:' prefix kept - the smoke
  greps it). --help/-h -> stdout exit 0; --version -> 'OpenLeo
  ll_cutscene 0.1.0' via new LEO_VERSION in ll_config.h (synced to
  the .rc's FILEVERSION 0,1,0,0, not the spec's 'dev' - an existing
  version string DOES exist there); unknown '-' args -> 'main:
  unknown option X' + usage, exit 2; a lone '-' still means the
  file. All proven native AND on the .exe under wine. Trailing-
  value-less flags (--ppm with missing args etc.) now exit 2 too
  (previously became FILE.IS).
- Item 5: --then overflow warns 'main: --then limit (16) reached,
  ignoring X' and the value is STILL consumed (pre-overflow the 17th
  value leaked into file=). Proven with 18 --then args: exactly 2
  warnings, T17/T18 named, LOGO + chain semantics untouched (chain
  stops at first unopenable scene - pre-existing behavior).
- Item 6: bare launch prints usage ONLY when pick_data_dir() fails;
  with data only the [leo] notice line (verified: data found -> no
  usage, demo runs; empty isolated dir -> usage+exit 2).
- Item 7: missing_movie_box() helper + the three run_menu .IS call
  sites (AUTORZY x2, chapter .IS table) show plat_message_box on
  is_player_run==1, gated by !auto_i && !secs_hook (dummy backend's
  box is a no-op anyway; the hooks mark automated legs).
- Item 8 (B-02): WANT_CD_GATE + DATA_DIR_FROM_CFG removed from
  ll_config.h (git grep: no use anywhere outside the file; PLAN.md's
  historical mention left as history, same pattern as TICK_HZ).
- Item 9: llrs.c index fseek -> ll_fseek64 (fs_ci.h already incl.).
- X-12 follow-up beyond the table (diagnosed this session):
  '--demo /nonexistent' "hang" = empty interactive menu after the
  missing-data notice. Now: 'main: no game data in D (need an
  ANIMACJE/ or ETAPY/ subdirectory)' + usage, exit 2 - same
  predicate as find_data_candidate, same contract as the bare
  launch. Every --demo CI leg uses real data dirs (audited).
- CI: windows-smoke flipped to '--help' expecting exit 0 (the
  FIX-12-anticipated flip, comment updated); asan job runs the
  mov selftest. README: CLI contract paragraph added to ## Run.
- Regression: full verify-all exit 0 (only by-design no-regression
  SKIP; golden 98.4% unchanged; mov selftest green inside the suite);
  fixture-free sim exit 0, still 22 named SKIPs (the selftest runs
  there as real coverage); spot verify/verify-skip/verify-level/
  verify-menu/verify-arch green; zero-warning native + llvm-mingw +
  cmake-mingw builds (the latter re-exercised the PR-30 DLL copy).

- PR-32 pushed 8b4f908; CI green run 43724602260 (all 3 jobs, incl. the
  flipped windows --help smoke + asan mov selftest). CLOSED.
- SERIES PR-25..32 COMPLETE (c0d959a, 656dbc5, 816a9a0, 4618d61, 42f3703,
  79961aa, f2bfbc4, 45fbd26, 8b4f908 - all CI green). Owner decisions
  C1/C2/C3 presented below (## Presentations section); C2 default flip
  already shipped per its ruling ("implement behind the flag" - done),
  awaiting confirmation/rollback call.
## Presentations — C1 / C2 / C3 (owner decisions, end of series 2026-11-15)

### C1 — FIX-09 Part 2: one audio device for the session (X-10 half) - DONE end-to-end: implemented PR-34, owner ear check "identical" -> alsa pin retired PR-35, handheld measurement CONFIRMED 2->1 on wasapi (native 48k answer)
- Question: keep the PR-12/P-03 design (device opened per playback, closed
  at scene end) or open ONE device for the process (spec Part 2; opens at
  the device's native rate, engine resamples).
- Evidence gathered: Part 1 shipped in PR-30 (hint not env, one-shot
  open-failure fallback proven end-to-end, LEO_KEEP_PIPEWIRE=1). On this
  desktop the UNPINNED path plays fine (native pipewire, LOGO 125/125) -
  the original 'pipewire squeaks' predates the discovery that the system
  SDL 2.32.74 is sdl2-compat; squeak was likely the compat resampler,
  not PipeWire. PR-29's A/V sync now measures DEVICE-CONSUMED bytes;
  a mid-chain device reopen creates a gap that clock cannot see (the
  look-ahead bounds it, but continuous play is what the design wants).
- Not measurable here: ALSA shared-mix cannot show a reopen pop. The
  decisive 2-minute test on the exact risk hardware (handheld, WASAPI):
  push ll_cutscene.exe+SDL2.dll+two .IS, run A.IS --then B.IS, count
  'll_audio: device' lines = one line per real WASAPI open/close cycle
  = one pop candidate. Handheld asleep (physical wake needed).
- Options: (a) measure first (recommended: wake handheld), then likely
  implement Part 2 (PR-30's own comment predicts the alsa pin becomes
  unnecessary); (b) keep per-playback (worst case now bounded by PR-30
  fallback); (c) implement Part 2 without measuring.

### C2 — Wayland default flip (FIX-11) - CLOSED: shipped PR-31, owner 150% eyeball verdict "SHARP" 2026-11-15
- PR-31 shipped default = SDL's own choice (spec steps 1-2), pin behind
  LEO_FORCE_X11=1 (byte-identical old lines), escape hatch proven live;
  classic SDL2 still picks X11-first (source-verified SDL_video.c:74-83)
  so the flip only takes effect on the SDL3/sdl2-compat stack.
- Live proof: native Wayland window + keyboard WITHOUT any click
  (/dev/uinput synthetic key -> 'quit=1 (q)') = the pin's original
  rationale (focus never works) refuted on this compositor.
- Owner eyeball DONE 2026-11-15: native-Wayland LOGO run at 150%
  fractional scaling -> 'sharp'. No rollback needed; LEO_FORCE_X11=1
  stays as the escape hatch. All three owner rulings (C1/C2/C3) are
  now fully closed.

### C3 — q = quit the app (FIX-01 step 6) - RULED: adopt -> shipped PR-33
- Verified current semantics: q in a cutscene ENDS THAT SCENE (player
  returns 0; the chain advances to the next scene / the title menu);
  ONLY a window close (or LEO_QUIT_AFTER_MS) sets the sticky
  plat_quit_requested that unwinds every screen (verify-quit).
- Adopt = one line: plat_request_quit() in the q handler -> q anywhere
  quits the app; clean pair with existing ESC=skip; needs the player
  selftest + a verify-quit variant updated.
- Risk note: q is our own affordance (no original binding); accidental
  quits mid-boot-chain are the argument for keeping status quo.

## PR-33 — C3 adoption: q quits the app (DONE 2026-11-15)

- Ruling executed ("all 3 changes"): C3 first, exactly as presented -
  one line in the q handler, riding the machinery PR-25 built.
- ported/platform_sdl/is_player.c pump_input: `q` now also calls
  plat_request_quit() (the SAME sticky flag as a window close). ESC
  keeps the original two-step (skip this scene, chain advances); the
  is_player.h header comment documents the split.
- New test hook (platform_sdl.c, additive, default off):
  LEO_TEST_KEY_AFTER_MS=N + LEO_TEST_KEY=<key> pushes ONE synthetic
  SDL_KEYDOWN through SDL's own queue N ms after the first poll, so it
  travels the exact map_key/repeat-filter path of a physical press.
  Shares the LEO_QUIT_AFTER_MS first-poll clock; accepts printable
  chars (case-folded) or "esc"/"space".
- verify-quit: second PX_DATA leg - synthetic q 300 ms into a
  --demo boot chain must print 'quit=1 (q)', exit 0 and NEVER open
  the menu. Proven non-vacuous: with the plat_request_quit() call
  compiled out the leg fails exactly as the old behaviour predicts
  (rc=124 - the menu opens and waits forever headless).
- Live desktop proof (owner's Wayland session, no pins): real window,
  seat-level synthetic key via /dev/uinput -> 'is_player: presented=99
  decoded=99 quit=1 (q)', app exit rc 0, menu never opened.
- README: key semantics + the new hook documented.
- Full suites: clean verify-all (with fixtures) ALL TESTS PASSED; fixture-free
  sim exit 0 (22 named SKIPs); zero-warning on the vendored llvm-mingw
  build; first verify-all run had one self-inflicted failure (my own wine
  A/B run shared WINEPREFIX concurrently with the suite - lesson: never
  run wine while verify legs are using the prefix).
- PR-33 pushed 61a00ee; CI green run 43741448274 (all 3 jobs). CLOSED.

## PR-34 — C1: FIX-09 Part 2, one audio device for the session (DONE 2026-11-15)

- Owner ruling "all 3 changes" executed. Measure-first was blocked (the
  handheld never woke today; WOL does not work), so the implementation
  went first; the before/after measurement is fully staged for whenever
  the device is on: /tmp/opencode/cmwin_before (a7a3388 build) vs
  /tmp/opencode/cmwin (PR-34 build), instrument = count of
  'll_audio: device' lines over a two-movie chain (expect 2->1, the
  device line is now once per process).
- ported/platform_sdl.c: ONE session device, opened at first use with a
  48000 request + SDL_AUDIO_ALLOW_FREQUENCY_CHANGE (the backend answers
  its NATIVE rate; here: S16LSB 48000). Every plat_audio_open is a
  logical stream (SDL_NewAudioStream source->device), plat_audio_queue
  converts on the feeding thread and lands the bytes in the device
  queue immediately, plat_audio_close frees the stream, drops every
  queued byte and pauses the device silent but does NOT close it,
  plat_audio_queued_bytes scales device+stream bytes back to the
  CALLER'S SOURCE byte format (uint64 math) so the FIX-08 clock and the
  music low-water keep reading their own units.
- ported/ll_audio.c: ll_audio_open takes allowed_changes + have_out
  (accepted-spec log P-13 stays in one place, fallback retry passes the
  same changes). Docs rewritten: the old "device is deliberately per-
  playback / no resampling" contract is replaced by the session-device
  model (engine WAV dumps stay UNresampled - only hardware bytes are).
- platform.h + is_player.* comments updated to the new contract; API
  and return codes byte-for-byte unchanged (callers see the old model).
- Proven: exactly ONE device line across a --then chain + menu + menu
  intros (4 plays, one process); A/V margins on the new clock far
  inside spec (min_queued ~150 ms >= 60 required, |err| <= 16 ms <= 40
  required, LEO_AV_DEBUG over real alsa); verify-music byte-exact
  (verify_music.py unchanged); LEO_IS_WAV for LOGO byte-identical
  710,640 B vs the a7a3388 build (acceptance 4); verify-platform,
  verify-quit both halves green; zero-warning gcc + vendored
  llvm-mingw (SDL2 API note: free is SDL_FreeAudioStream, not the
  SDL3-style Delete name).
- LEO_KEEP_PIPEWIRE=1 A/B (the pin-removal pre-test): native pipewire
  now answers 48000 S16 with the same A/V margins and a clean run -
  mechanically healthy. The 'squeak' was an AUDIO artifact the numbers
  cannot see: pin STAYS until an ear check says otherwise.
- PR-34 pushed f7f716a; CI green run 43742345929 (all 3 jobs). CLOSED.
  Open only as follow-ups: the STAGED handheld measurement (device lines
  2->1: /tmp/opencode/cmwin_before vs /tmp/opencode/cmwin, one
  power-button press away) and the optional LEO_KEEP_PIPEWIRE ear check
  for pin removal. SERIES PR-25..34 COMPLETE.
- HANDHELD MEASUREMENT DONE 2026-11-15 (ADAM-HANDHELD, wasapi, real
  device, two-movie chain LOGO+PRAWA, same staged binaries):
  BEFORE build = 2 device opens ('S16LSB 22050Hz 2ch' then '...1ch',
  one per scene - the pop source confirmed on real hardware);
  AFTER build = 1 device open ('S16LSB 48000Hz 2ch 4096' - WASAPI
  answered its NATIVE rate exactly as Part 2 predicted), both scenes
  played clean (125+142 frames). A/V on WASAPI: after worst
  min_queued=72ms (floor 60 OK), worst |err|=40ms (at the acceptance
  bound; first real-device note about the 46ms latency estimate under
  a 4096-sample device - watch if anyone ever reports drift). C1 FULLY
  CONFIRMED ON THE ORIGIN'S HARDWARE.

## PR-35 — FIX-09 tail: retire the automatic alsa pin (DONE 2026-11-15)

- C1's last open thread closed by the owner's A/B ear test on this
  desktop: LOGO played back-to-back pinned (alsa) vs unpinned
  (pipewire) on the new native-rate session device - verdict "seems
  identical". The automatic pin is therefore RETIRED.
- ll_audio.c session_init: default = SDL's own choice (pipewire on
  this stack). The pin survives as opt-in LEO_PIN_ALSA=1 (same
    mechanism: hint not env, user SDL_AUDIODRIVER still wins, same
    one-shot open-failure fallback via s_forced_alsa). The pin's log
    line changed honestly to 'll_audio: alsa driver pinned
    (LEO_PIN_ALSA=1)' (nothing keys on the old text; prefix intact).
    LEO_KEEP_PIPEWIRE=1 stays accepted as a no-op (it is the default
    now) so no existing setup or script breaks.
- Live proofs (this machine, real audio): default run -> 'driver
  pipewire' device line, no pin line; LEO_PIN_ALSA=1 -> 'alsa driver
  pinned' + 'driver alsa'. verify-platform/verify-music/verify-quit
  green; zero-warning mingw. Full verify-all (fixtures) ALL TESTS
  PASSED exit 0; fixture-free sim exit 0 (22 named SKIPs).
- The FIX-09 story is now fully closed: Part 1 (PR-30) + Part 2
  (PR-34) + pin retirement (PR-35). Only the staged handheld
  device-count measurement (2->1) remains as confirmation, not code.
- PR-35 pushed 6177637; CI run 43743655530: windows-smoke failed on
  the known transient dummy-backend clock flake ('sleep_until honors
  the deadline (>=20ms) FAILED' on a loaded runner - code untouched by
  this PR, passed on identical earlier runs; asan+linux green). Rerun
  of the failed job: success. CLOSED.

## PR-36 — chapter-map authenticity: sparse table + BACK.PCX base + PCX header truth (DONE 2026-11-15)

Owner model for the title menu (confirmed 2026-11-15): the map base is the
engine's own clean landscape, the corner words are engine-DRAWN text (no
button plates - "never on the map"), and the double text the user saw in
the port had to go. This PR delivers all three and cracks two container
secrets on the way.

- llrs nested-BLOCK FORMAT CRACKED (was: truncated read). The u32 at a
  block payload's head is the count of USED records, NOT a table capacity:
  the 26-byte slots continue past it as a sparse free-table with id==0
  holes. Probed on all 29 real index blocks (ANIMACJE/MENU declares 39;
  its 39 records sit at slot indices 0..197 of a 199-slot payload).
  ported/llrs.c llrs_block now parses every whole slot; llrs.h documents
  the contract; llrs_selftest prints "%u table slots, %u used records"
  and verify-llrs pins 'MENU: 199 table slots, 39 used records' +
  WYJSCIE.SEQ present (slot 81, past the old cutoff). Same rule applied
  to the python verifiers (tools/verify_pcx.py, tools/verify_seq.py;
  other helpers are top-level-only, unaffected).
- What the hidden records turned out to fix (all re-pinned, all real):
  verify-seq 172 -> 1656 blobs byte-exact C==python; TUNGA.11 shows
  "11 animated object(s)" (STRUM1/STRUM2 were a FALSE "data hole");
  RAN.01's true canvas = full tlo stack plan3.pcx,plan2.pcm,plan1.pcm
  -> REAL bg PLAN3.PCX 960x400 (the old wrong bg was PLAN1.PCM 400x200
  because only slot 0 was visible); help pages 1-5 join real
  pomoc02/04/06/08/10.seq art (page 6 POMOC32.SEQ stays [CD]);
  verify-help pixel oracle stays exact (1768==1768 px).
- OGIEN_M.SEQ (ANIMACJE/TUNGA.11/DNGANI): genuine 1997 data defect -
  even its FIRST element overruns the 689-byte payload; C and python
  reject it identically (PARSE-FAIL both). tools/verify_seq.py keeps
  extracting + comparing it under a documented KNOWN_BAD_SEQ allowlist
  ("1 known-bad original-data defects accepted"), not a silent drop.
- PCX HEADER TRUTH-PIN (the big one, latent since the first PR): the
  decoder rule "non-standard header, RLE at offset 64" had only ever been
  C==python-verified, never truth-checked. BACK.PCX (see below) proved
  the game PCXs carry the STANDARD 128-byte header (0a050108, window
  @4..12, byte-per-line u16 @66, zeros @120..127, RLE @128, palette =
  raw last 768B, no 0x0C): decoded from 128 its index plane is
  100.00% byte-identical to the captured menu VRAM outside the text
  pixels; from 64 every pixel was shifted 64 bytes (0.2 row) and matched
  3.8%. ported/ll_pcx.c + tools/pcx_dump.py now read the stride field
  and count rows at it (standard even-byte scanline padding), keeping
  the first w bytes - MFAKE.PCX (ETAPY/KADMAR2) turned out to be a
  REAL 1997 13x13 solid-color test asset (stride 14 + one stray 0x0C
  after the stream) handled natively, not a fixture bug. 65/65 payloads
  stay byte-exact; BACK.PCX vs the proven capture plane: 100.00%.
- The map now uses its REAL base: main.c gains menu_record()/menu_back()
  (reads ANIMACJE/MENU/BACK.PCX - found only via the fixed block scan,
  slot 80); ll_menu_map_open takes (back_pcx, back_len); ll_menu.c
  map_bg() is a 3-way base: BACK.PCX (colored through ll_menu_dac.h -
  its own tail palette is a standard-VGA leftover, 0/256 entries match
  the captured DAC) -> captured-art fallback (ll_menu_art.h, text BAKED
  IN) -> LEO_MENU_BG=placeholder procedural. map_bg returns baked_text
  and the font corner words are drawn IFF !baked_text: that was the
  double-text defect (capture fallback + font layer = words twice).
  AUTORZY is font-drawn now at its measured anchor (the word .SEQ files
  decode as filled button PLATES ~50x16 bands - not letters - and the
  owner confirms no plate ever shows on the map; they stay excluded from
  the map, still used by the restart dialog).
  Measured engine-text bboxes (capture diff vs BACK, logical 320x200):
  WYJSCIE x4..110 y4..39, RESTART x254..313 y2..17, OPCJE x259..313
  y183..199, AUTORZY x2..66 y179..196, title x23..174 y45..81 - the
  port draws at these anchors.
- verify-menu re-ran with ZERO re-pinned expectations (all log pins and
  the BERLO cycle still hold on the real base). Trap re-armed: the
  wine frame legs compare .exe output against fresh linux frames, so
  verify-menu now lists ll_cutscene.exe as a prerequisite (a stale
  pre-PCX-fix .exe gave a false "win frame != linux").
- verify-golden: port pose set re-pinned (30 hashes, BACK base); full
  RGB vs the ORIGINAL capture frame: 3.773% (shifted) -> 92.088%; the
  whole 3308-px residual is exactly the text areas (our 8x8 bitmap font
  vs the original's cursive text engine). min_best_rgb 98.0 -> 92.0 and
  GOLDEN_MIN 98 -> 92 are the honest floor: PR-23's 98.43% had been
  measured with the captured SCREENSHOT as base - a screenshot is not
  game data. PR-37 candidate that closes the rest: recover the original
  cursive font from the capture glyphs (the QEMU rig can capture more
  title pages for glyph coverage).
- verify-win regression found + fixed in CI tooling: the sparse scan
  lifted the .SEQ case count ~10x and wine's 32767-char CreateProcess
  argv limit killed the batch (Traceback, "enough memory"? no - long
  cmd line). tools/verify_seq.py now batches tool invocations (~20 KB
  argv, concatenating outputs - C tools print per-file in argument
  order, diff semantics unchanged).
- Visual truth (VL review of port best frame vs the original capture):
  landscape/ocean/islands identical positions, no shift/banding; every
  corner word exactly ONCE, no ghost; title once; red arrow on the
  green island. Verdict: "same screen, cleaner data, different font".
- CI hygiene (additive, preserved all LEO_* hooks/returns): NEW hook
  LEO_LEVEL_NO_OBJ=1 (ll_level CI-only: bare bg without .des object
  compositing; needed because JUNCTION now HAS live scene sprites and
  the help-overlay oracle asserts "help frame == bare bg + page").
- Proofs: full verify-all (fixtures) green (re-run after the batching
  fix, exit 0); fixture-free sim exit 0 (22 named SKIPs); zero-warning
  llvm-mingw build; verify-arch ok in both modes; verify-pcx 65/65,
  verify-seq 1656/1656 (1 known-bad), verify-golden pose+full both ok.
- PR-36 pushed 82727cd; CI run 43749480757: linux + asan green first
  pass; windows-smoke failed on the SAME transient clock flake as
  PR-35 ('sleep_until honors the deadline (>=20ms)', code untouched by
  this PR). Rerun --failed: all three jobs success. CLOSED.

## PR-37 — keyboard-only menus + real cursive text: corner words, title
sprites, corner cycle (DONE 2026-11-15, owner directives)

Owner asked, in order: "berlo should be controled only by keyboard,
clicking not necessary"; "Sprites instead of font text"; "in menu i
cant choose in corners titles other than 'autors' and still there are
simply text not stylised". All three shipped; every step is DATA
(owner-supplied mapping + bytes recovered from the golden capture).

1. KEYBOARD-ONLY map + restart dialog: BUTTON_DOWN corner/island click
   handling and MOTION hover feedback removed (draw_text1_r/draw_rect
   helpers dropped with them); help text, README and Makefile echoes
   updated. LEO_* hooks and return codes unchanged (additive only).
2. CURSIVE CHAPTER-TITLE SPRITES: ll_menu_ch gains seq/seq_len (raw
   ANIMACJE/MENU/LEVELxx.SEQ payload); the map compositor parses on
   selection change and draws ALL elements simultaneously at their
   authored (-f0,-f2) anchors, skipping the font title. main.c carries
   the OWNER-READ mapping (2026-11-15): LEVEL01..11 -> ZARIA.01,
   ZARIA.02, SADES.06, KADMAR.07, KADMAR2, ENA.03, ENA2, LONA.05,
   KARA.07, TUNGA.11, TUNGA2.12; RAN.01/JUNCTION (no image exists -
   11 titles for 13 chapters) and dataless mode keep the font-text
   fallback. Data-side binding routes were exhausted first (ETAPY
   slots are hash order, no chapter metadata anywhere, original
   LEO.EXE has zero LEVELxx refs - the binding lived in the missing
   MENU.mnu, CD data hole, PLAN 8b).
3. ORIGINAL CURSIVE CORNER WORDS: the captured-VRAM vs BACK_idx.bin
   diff IS the engine text layer: corner words 2406 px + title band
   2370 px = 100% of the capture's text ink (±26). The four corner
   words are embedded in ported/ll_menu_text.h as recovered (x,y,
   palette-idx) entries + per-word bbox macros and blitted through
   the menu DAC - pixel-exact: all 1886 corner-ink px of the captured
   frame now match the port frame EXACTLY (verify script method:
   sample even rows/cols of the PPM vs ll_menu_dac[cap_idx], title
   band excluded). Skipped on the baked-art fallback (same no-double-
   text rule as PR-36). The title band of that diff is NOT embedded:
   full template match proved the captured map title is engine cursive
   TEXT, not any LEVELxx sprite (best IoU 0.23), and the engine text
   font has no asset in any container.
4. CORNER WORDS BY KEYBOARD: the map KEYDOWN handler extracted as
   map_key(); arrows now cycle 13 chapters then the 4 corners
   (clockwise WYJSCIE/RESTART/OPCJE/AUTORZY), Enter/Space activates
   (-2/-6/-5/-4, log 'map: corner <tag>'), a/o/r shortcuts kept.
   Selection marker = solid bright 2px bar at the word's recovered
   bbox edge (dialog answer-zone language; clamped for bottom words).
5. CI PIN: LEO_MENU_KEY extended with 'corner' - feeds Down x(nchs+1)
   + Enter through map_key, ONE key per grid tick (guard added: the
   FIX-07 inner poll runs several times per tick, the hook must not
   fire at poll rate - first attempt walked 2 poses/tick and
   activated a chapter by accident). New verify-menu leg pins the
   marker line, exactly 11 'map: title sprite' poses (every mapped
   chapter rendered once), 'map: corner restart' and the proven
   restart-confirm downstream.
6. GOLDEN RE-PIN (honest, by-design frame change): 30 pose hashes in
   tools/golden_expect.json regenerated from the new deterministic
   30 ms-grid frames; measured best vs the ART frame 92.4516% RGB
   (UP from PR-36's 92.088% - the corner words are now exact),
   worst pose in the cycle 92.348%; min_best_rgb KEPT at 92.0 (floor
   unchanged, safety margin widened: worst pose +0.348 pt over the
   floor, previously best pose only +0.088 over it). Remaining residual
   is exactly: the captured title band (engine text of the
   then-selected chapter - unrecoverable as data), the two font-
   fallback titles and 43 off-palette px = the BERLO arrow (the
   capture shows NO arrow ink at all - consistent with the owner's
   "sprite disapeares" memory of the original).
Verification: native + llvm-mingw builds warning-free; full
make -k verify-all rc=0 INCLUDING the new corner-cycle leg;
fixture-free sim rc=0 (22 named SKIPs); verify-arch ok; pixel checks
above from LEO_MENU_PPM frames. Not pinned headless: interactive
arrow-walk UI feel (owner to try - 'corner' covers the code path).
 - PR-37 pushed 81a4711; CI run 43753633583: ALL THREE JOBS (linux,
   asan, windows-smoke) green on the FIRST pass - the windows clock
   flake did not recur. CLOSED.

## PR-38 — corner selection made VISIBLE: word .SEQ records are the
cursive words themselves; bulawa moves onto the word (DONE 2026-11-15)

Owner re-tested PR-37: "i still cant choose wyjscie, restart i opcje"
and "when bulawa is on autorzy text isnt animated" + "and bulawa is
under text". Diagnosis: the corner cycle WORKED (CI-proven) but was
INVISIBLE - the BERLO cursor stayed parked on the last chapter island
(which sits right under the bottom-left corner text - that is the
"bulawa under text" the owner saw) and only a thin bar marked the
selection. The feedback layer was wrong, not the input.

DATA DISCOVERY (re-examined the word records, tools/seq_render.py):
ANIMACJE/MENU/WYJSCIE/RESTART/OPCJE/AUTORZY.SEQ are NOT button plates
(the PR-36-era "filled plates" reading was WRONG and is corrected in
the code comments) - they ARE the cursive words: 2 nearly-identical
variants each, and the bigger variant's authored (-f0,-f2) anchor
lands EXACTLY on the recovered captured-text bbox (AUTORZY 64x18 at
(4,180) vs text bbox x2..66 y179..196; IoU vs captured ink: emphasis
variant 0.70-0.77 vs plain 0.49-0.57). The two variants are the
engine's own normal/bold pair - the only record of the original's
corner-word SELECTION animation.

Fix (ll_menu.c map compositor + ll_menu_word plumbing in main.c):
- selected corner -> the word ALTERNATES its two .SEQ variants at
  their authored anchors (600 ms bold pulse - the only animation the
  records hold); plain captured-text layer pixels inside the selected
  bbox are skipped (replace, never double).
- the BERLO selection cursor (the original menu's pointer) MOVES from
  the island onto the selected corner word (centered, clamped inside
  the canvas) - exactly the "bulawa over the text" the owner
  described; it keeps cycling its 30 frames there (animated).
- bright-bar marker stays ONLY as fallback (dataless/baked base/parse
  fail). Data flows as ll_menu_word words[4] (WYJSCIE/RESTART/OPCJE/
  AUTORZY order = corner cycle order), loaded via menu_record.
- CI: the LEO_MENU_KEY=corner leg now also pins 'map: corner art
  wyjscie/restart' (both word records parsed during the walk).
Pixel-verified from map_0013..15 pose frames: with RESTART selected -
emphasis variant paints 573 px at its authored anchor, the three
UNselected words stay pixel-exact vs the capture (0 wrong of
1899 ink px), bulawa pixels appear at the word center and are GONE
from the island; no corner selected = no bulawa at any word. Frames
with no corner selected are UNCHANGED -> golden pose hashes and the
92.0 floor hold untouched (verify-golden ok, best still 92.4516%).
Verification: full make -k verify-all rc=0 (new corner-art pins green
incl. the .exe legs); fixture-free sim rc=0 (22 named SKIPs); native
+ llvm-mingw warning-free.
 - PR-38 pushed fe2cf12; CI run 43758144143: all three jobs green on
   the first pass. CLOSED. Awaiting owner re-test of the corner
   cycle + bulawa-on-word feel (not pinnable headless).

## PR-39 — map navigation restructured to TWO AXES per owner directive
(DONE 2026-11-15)

Owner: "mayby better do it that way, up, down arrow switch between
choosing corner options and maps, left right switch between specific
map or specyfic corner option". Replaced PR-37's single flat cycle
(chapters then corners) in map_key():
- Up/Down TOGGLE the selection MODE: chapter map <-> corner options.
  Entering corner mode lands on WYJSCIE (index 0 = the corner-cycle
  order already used by the return codes); leaving clears it (cor=-1,
  bulawa back to the island). First press only - holding Up/Down
  must not flicker the mode.
- Left/Right move WITHIN the active group: chapters wrap among
  chapters, corner words among corner words (auto-repeat stays on,
  FIX-03).
No new state: the mode IS (cor >= 0), so all PR-38 selection visuals
(bulawa-on-word, variant pulse) carry over untouched; Enter/Space,
a/o/r shortcuts and return codes unchanged.
CI: the LEO_MENU_KEY=corner leg now walks the two axes: Right
nchs-1x (every chapter pose - still 11 title-sprite logs), Down
(corner mode -> 'map: corner art wyjscie'), Right ('corner art
restart'), Enter ('map: corner restart'). All grep pins + -eq 11
unchanged.
Golden untouched: map frames never select corners, so the 30 pose
hashes + 92.0 floor hold (verify-golden ok, best still 92.4516%).
Verification: full make -k verify-all rc=0; fixture-free sim rc=0
(22 named SKIPs); native + vendored llvm-mingw warning-free (the
only 'warning:' lines in the logs are MESA-EGL runtime noise).
 - PR-39 pushed 0b631fa; CI run 43759690580: all three jobs green on
   the first pass. CLOSED. Awaiting owner re-test of the two-axis
   feel (not pinnable headless).

## PR-40 — BERLO selection cursor reconstructed (owner defect report)

Owner (2026-11-15, after PR-39 build): "after switching left some
artifacts from previous state... on screen we see curently selected
ZARIA 2, but for some reason is still wisible arrow form ZARIA 1,
btw there is also some issue with color, arrow is grey not red".

ROOT CAUSE (proven from data, no repro needed):
- The red mace at logical x147..205 y94..136 is BAKED into
  ANIMACJE/MENU/BACK.PCX. Both original captures (golden/menu_art.ppm,
  menu_art2.ppm; vram.bin == menu_art's index plane) show ZERO
  engine-drawn pixels over it: the baked mace never moves and the
  engine never touches it.
- BERLO.SEQ's 30 elements are a DELTA CHAIN over that art: their
  union (each element drawn at el0-anchor-normalized authored
  position) is a 37x44 px mace envelope, 986 px; slide-aligned onto
  BACK's warm-color core it overlaps 82% (813/986) and the el0 canvas
  origin lands on the art at (164,95) = BERLO_HOME. The chain's own
  colors are dark olive/browns (31,23,0)/(32,39,7)/(104,96,40) -
  the shading patches ADD, not the mace body (body = BACK's golds
  (247,208,7)/(239,175,0) + reds).
- The map path drew ONE chain element per 100 ms tick at the park =
  a single delta fragment: a floating olive-grey smudge (the "grey
  arrow") that also changed shape between ticks like leftover
  artifacts. Measured in the old map_*.ppm walk frames (a 308px
  smudge blob at the island0 park).

FIX (ported/ll_menu.c): bake the FULL cursor once at map open -
cchain = the 30-element accumulation (later elements overwrite) in
el0-relative coords; cbase = BACK.PCX pixels of the warm class
(r>g && r>=b: the mace art's reds/golds/sands) sampled at BERLO_HOME.
At runtime composite the whole 44x52 sprite el0-anchored at the park
(chain first, art pixels WIN where they exist - the captures show the
original is the bright art; chain alone reads grey) then re-paint the
current element's patch at its authored relative anchor = the chain's
shimmer over the full sprite. Park/word anchors, corner clamps and
the LEO_MENU_KEY t2!=t8 BERLO-cycle leg are untouched (still 2824 px
differ between t2 and t8 frames). Without a decoded BACK (no-data /
LEO_MENU_BG=placeholder) cbase is empty and only the chain layer
exists - documented fallback.
Proof the reconstruction lands right: frame diffs old-vs-new are
confined to the 44x52 mace rect at the park (1596 px, bbox
x63..106 y56..104 for island0), the park sprite's top colors are now
the HOME art's colors ((247,208,7)x241, (200,135,0), (248,120,7),
(239,175,0) - identical palette), and the baked home mace region is
byte-unchanged in every frame.

GOLDEN RE-PIN (honest, by-design): all 30 pose hashes regenerated;
best vs the ART frame 90.7281% RGB (was 92.4516%), worst pose
90.7188%. The -1.7pt IS the ~1600 px mace drawn at the park - the
single fixed capture can never show a moving cursor (the original's
never moves; selection feedback there is island art + engine cursive
titles). min_best_rgb 92.0 -> 90.0 (margin +0.719pt over the worst
pose), Makefile GOLDEN_MIN comment updated to match. The moving red
cursor is an explicit owner directive, so this is the accepted look.
Open question parked for the owner: with a moving cursor the BAKED
center object stays visible (faithful to the original) - hide/inpaint
it when the cursor is elsewhere? Needs the owner's call. (RESOLVED by
the v2 follow-up below: the vision read showed that object is island
decoration, not a mace - it stays, authentic.)
CI additions: none (the existing 30-frame/BERLO-cycle/corner pins all
still pass unchanged).
Verification: full make -k verify-all with data rc=0 (all legs ok,
verify-menu + verify-golden green incl. the mingw/wine halves);
fixture-free make -k verify-all rc=0 with the named SKIPs; gcc +
vendored llvm-mingw builds warning-free.
 - PR-40 pushed f6fec38; CI run 43763544035: linux + asan green on
   the first pass, windows-smoke hit the known flake (fixture-only
   leg, passed unchanged on rerun --failed). CLOSED.
FOLLOW-UP v2 (same owner thread, same day): after f6fec38 the owner
reported "now is even worst, mace is rendered with part of background
and moving with it around" + "and still there is same previous issue".
Diagnosis needed eyes this tool does not have - the owner supplied a
Gemini API key; wiring the REST endpoint (generativelanguage v1beta,
model gemini-3.8-flash - 2.5-flash 404s for new users - inline_data
base64 PNG parts) and sending crops DISPROVED the v1 premise: the
BACK.PCX "home" region is island DECORATION (sand + two terracotta
stepped-pyramid shapes + palms), NOT a mace. The (164,95) home
calibration had been an accidental fit against that island coverage,
and filling the cursor outline from BACK art painted pyramid/sand
patches inside it - literally "part of background rendered with the
mace, moving with it". PR-40 v2: the cursor is the FULL accumulated
BERLO chain alone, in the file's own authored palette (dark olive
outline, golds idx223/224/230, reds idx153/154/239/240) + the current
element re-painted at its authored anchor (shimmer). Removed: the
cbase BACK sampling, the BERLO_HOME_* calibration, and the v1.5
envelope-mask variant + home-region inpaint (implemented and verified
algorithmically correct in /tmp/opencode/p40c - 2464-px flood blob,
frame == independent python simulation byte-for-byte - but the wrong
design: the central object is AUTHENTIC original art, present in the
original's own captures, so we do not erase it; OPEN QUESTION for the
owner whether they want it removed anyway. (CLOSED 2026-11-16: owner
ruled KEEP - the island decoration stays, no change was ever needed.)
v2 verification (p40d): home window byte-identical to BACK (the 4
diff px are the park-3 chain cursor overlapping it, as in the
original); island-0 park = one envelope-confined cluster 981px (of
986 chain px, 5 cancelled by same-color overlap) at x64..100 y56..99;
shimmer t2 vs t8 2821px; Gemini read of the new crop: "(1) clean
mace-only cursor, no background pieces... olive-gold to golden-yellow
with dark olive shadow outlines, ruby-red studs".
Gates: 30 pose hashes re-pinned; best vs ART frame 91.7375% (v1
measured 90.73; the +1.0pt is the authentic center art restored) -
min_best_rgb stays 90.0, margin +1.74pt. full make -k verify-all
rc=0 (verify-menu/verify-golden green), fixture-free rc=0 with named
SKIPs, gcc + vendored llvm-mingw warning-free.
FOLLOW-UP v3 (same thread, minutes later): owner: "now mace copying
itself during rotating". Re-measured BERLO.SEQ per ELEMENT
(seq_parse + seq_patch, the C-verified pair): every element is a
COMPLETE pose of the mace (~200-310 painted px, 15..27 x 35-37,
per-pose width varies = the authored rotation; anchors sway ~1px;
top colors idx223/224/218/230 golds + idx90..95 reds/browns). The
"30-element delta accumulation = one complete cursor" premise was
WRONG - it is a normal one-frame-per-tick animation script, and the
v2 union bake fused 30 poses into one 986px sprite = literally
"copying itself". (The old "delta chain" reading came from measuring
the UNION size, not per-element completeness.) PR-40 v3: no baking
at all - each tick draws ONLY the current element, anchored el0-at-
the-park so the ~1px authored anchor sway reads as one mace moving;
the canvas rebuilds from the base every tick so poses never overlap.
Also checked: each element carries an embedded 18-entry palette
override (w0&3, 36 bytes) - NOT applied (the raw indices hit the
menu DAC's golds/reds directly and the vision read accepts the
colors; recorded for future reference).
v3 verification (p40e): EVERY dumped frame shows exactly ONE
mace-colored blob, ~150-225 px, bbox 15..25 x 35-37 (the pose's own
shape), parked at the correct island/corner as the walk advances;
home region untouched; Gemini: "single clean mace... no ghost copies"
at island AND at the word-adjacent crop. Gates: pose hashes
re-pinned; best vs ART 92.4594% - ABOVE the pre-cursor baseline
(92.4516): a ~200px cursor overlaps the original's static frame
better than the 986px v2 blob; floor 90 unchanged. verify-menu green
unchanged; full verify-all rc=0 with data and fixture-free; warning-
free gcc + llvm-mingw.
Open from the same report, NOW REPRODUCED + DIAGNOSED (2026-11-15):
owner gave the concrete repro: "when start zaria 1 intro character on
it moves right and behind him left fragments of his model that
disappear after while". Method + findings:
- C == oracle on the actual movie: ZARIA1.IS sampled every 5th frame
  0..240 (49 frames) - ll_cutscene --ppm N vs tools/is_decode.py
  frames --ppm: all byte-identical (/tmp/opencode/zaria1). So the
  port shows exactly the data's accumulation states.
- Presentation adds no state: is_player present() re-linearizes +
  re-uploads the FULL persistent fb every frame (is_player.c:211-225,
  platform_sdl.c:86-116); no dirty rects. IS_OP_SKIP (is_stream.c:
  264-273) advances write cursors without writing - old pixels
  persist; that semantic is pinned against a live 230k-event trace of
  the original engine (is_stream.h:3-6).
- The data itself: 184,855 SKIP ops in 243 frames; per-frame change
  windows move right (f13 diff-bbox x22..115 -> f19 x60..160); stale
  px from the object's earlier positions survive ~10-20 frames before
  later frames repaint them.
- Gemini read of decoded frames f00..f40: a pterosaur-like creature
  flies left-to-right over a jungle dusk scene; "textbook inter-frame
  (delta) compression ghosting... detached fragments matching its
  previous body/wing coordinates... linger ~10-20 frames then clear".
  Exactly the owner's description.
Conclusion: NOT a port defect - it is the original 1997 video data's
own encoding (aggressive skip-coding at 8 fps); the original game
showed the same trails, same durations (pacing is the pinned
audio-block clock). Option IF the owner wants it cleaned: a port-side
temporal filter (e.g. blank a px when N consecutive frames leave it
unchanged while its neighbourhood moves) - explicitly non-faithful,
changes the golden-pinned frame contract; needs the owner's call.
(The Gemini vision-model wiring used for this diagnosis - owner's API
key, gemini-3.8-flash via the generativelanguage REST API - is now a
standing tool for visual defect triage; this model itself still has
no image input.)
Owner follow-up: "some time before it worked correctly, hard to say
which change broke it". Checked: the .IS codec is UNTOUCHED since the
first snapshot (git log --follow on is_stream.*: only moves/renames),
frame content is therefore identical in every past build; ZARIA1
pacing is also identical across the 2026-11-14 audio-block re-pin
(3bfd8a8): every ZARIA1 chunk's ESC region is exactly 176 B = one
DMA block = 14.29 ms in the new model, and the old uniform model was
1/8 s per chunk - both 8 fps, ~30 s. What DID change on Nov 14-15:
LOGO/INTRO_P-type pacing (they ran 2x slow before: their region math
was wrong) and the A/V sync machinery (PR-28/29 exact clock +
look-ahead queue, PR-34 native-rate device). Asked the owner which
build they compared and what "correctly" looked like (no fragments vs
smooth motion vs A/V). Note for future: if ghost-trails ever need
smoothing it must be an explicit non-faithful option (see above).

## PR-41 (SHELVED 2026-11-16) — offline movie re-conversion (.LVM1) + opt-in de-ghost replay

RESOLVED - SHELVED (owner decision, 2026-11-16): after the PR-42
review below ("all [smoothing variants] have same artifacts" - the
artifact lives in the 1997 source frames, not in presentation), the
owner's final call for now was "back to the version before the LVM
format". The product feature of this commit is REVERTED: no
ported/lvm.c, no LEO_LVM_DIR hook, no `make movies`/`make
verify-movies` wiring - the player is .IS-only again, exactly the
PR-40 state. The offline scripts STAY in tools/ (mv_convert.py,
mv_clean.py, verify_movies.py, plus the PR-42 proto_smooth.py) as
shelved tooling, NOT wired into CI; a local movies/ .LVM set is
gitignored scratch, regenerable with tools/mv_convert.py.
The full record below is kept because every measurement and grammar
finding in it stands regardless of whether the feature ever returns.


Owner (2026-11-15, after the ZARIA1 ghost-trail diagnosis in the PR-40
thread): "convert all movies to a less problematic format". Chosen
option 1: PRE-CONVERTED files + offline converter + a small in-tree
replay decoder (no ffmpeg runtime); scope: ALL 15 .IS movies. The
cleanup is explicitly NON-faithful (deviates from the exact 1997
picture); the default .IS path stays byte-identical, the new path is
opt-in behind LEO_LVM_DIR.

Pipeline:
- tools/mv_convert.py - the oracle, runs in CI. Two passes over the
  chunks mirroring the player exactly: audio_pass (ESC region grammar,
  stereo rule = gcd of region sizes) + video_pass (the reference
  decoder walk producing LINEAR index frames + the decode-grammar
  st.repeat timeline). Writes .LVM1: "LVM1" | u32 w,h | u32 rate
  =22050, u8 ch, u16 bits=16 | u64 audio_len, u32 nrec | PCM | nrec x
  {u64 hold_ns, u8 flags(bit0=frame, bit1=pal768), [pal], [u8 codec
  0=raw|1=RLE + frame]}. RLE is self-delimiting: ctl>0 = that many
  literal bytes, ctl==0 = u16 len + 1 value byte; literals NEVER
  contain 0x00 (every zero byte is stored as a run) - without that
  escape the first ZARIA1 frame desynced the decode at o=63804.
- tools/mv_clean.py - temporal de-ghost (design + measured thresholds
  in its docstring). Classification: FULL >=0.30 / PART 0.005..0.30 /
  NOISE <0.005 (kept: AUTORZY's 1-px text scroll). A maximal PART run
  (len >=3) merges (intermediates dropped, final kept, holds stay ->
  total duration byte-identical) only if ADDITIVE: net persistence
  >=0.70 AND <=40% of changed px ever return to base mid-run.
  Geometry tests were tried and RETIRED: live-action movies deliver
  new scenes as non-geometric CONVERGING updates (KADMAR newpx decays
  17k->8k->7k...), and the earlier monotone-sweep test was vacuous
  (np.sort makes monotonicity trivially true). Measured separation:
  motion runs persist 0.03-0.63 / restore 0.50-0.99; paint runs
  persist 0.70-1.00 / restore 0.00-0.36.
- ported/lvm.c/.h - parse-before-platform reader (any parse failure
  prints lvm: ... and falls back to .IS) + replay loop mirroring
  is_player's ONE clock + 150 ms lead + drift clamp + q/ESC/space +
  2 s end hold. Hook at the top of is_player_run (lvm_candidate():
  LEO_LVM_DIR + <basename>.LVM). Frame-less records advance the clock
  but do not count as presented (verify contract below).
- tools/verify_movies.py + `make verify-movies` (in verify-all).

Grammar correction pinned on PRAWA (all 220 chunks carry NO ESC
region - its audio is pure silence): st.repeat is set ONLY by the
DECODE walk; the audio-walk model read RPT bytes the decode grammar
consumes as frame data (reps 7/4 vs C's 0). video_pass now tracks the
decode-grammar repeat state per chunk and both silence fills and
silent-chunk holds use it. Proven identical to C's post-decode state
chunk-for-chunk (PRAWA transitions c=0->2, 59->4, 79->2, 184->7,
209->2; zero desync).

CI audio contract: C's look-ahead sizes silence at QUEUE TIME, so a
look-ahead run is NOT byte-pinnable (PRAWA: one run queued 612
repeat-ticks, the deterministic post-chunk rule gives 605 - the
difference is pure silence). The deterministic pin: .LVM region bytes
== the LEO_IS_WAV dump of a LEO_IS_PERIOD_MS=1 run (legacy per-chunk
enqueue = pure regions stream; the dump never contains alignment
pads). Also pinned: LEO_LVM_DIR replay presents == frames -
clean_merged + clean_merged_pal (a merged frame KEEPS its own PAL:
the palette/fade timeline is real content and stays at its original
time - such records ARE presents; found on INTRO_P: 3 of 52 merged
records carry PAL, presented 1472 == 1521-52+3); a truncated .LVM
prints the fallback line and runs .IS.
Green locally: ZARIA1 regions 1,219,680 B byte-exact + replay
204/243, PRAWA 0 B empty == empty + replay 142/142, fallback OK.

SHIP POLICY (owner, final 2026-11-16): converter ONLY - the .LVM set
is NOT committed. Sequence: "ship them too" -> pushed 14 plain + LFS
for the 195 MB INTRO_P.LVM (GitHub refuses blobs >100 MB; git-lfs
3.7.0 under ~/.local/bin, push rejected a second time for a token
without `workflow` scope on the ci.yml tweak) -> "push only
converter, override history to not waste space": nothing ever reached
the remote, so the commit was simply rewritten; no GC needed.
movies/ is gitignored LOCAL scratch: `make movies` generates it from
$(DATA) (deterministic: byte-identical regeneration spot-proven on
PRAWA), then LEO_LVM_DIR=movies plays it; single movies via the
converter CLI.

Batch results (final code, all 15, /tmp/opencode/lvm/clean_report.txt;
merged counts are deterministic across runs):
```
AUTORZY  frames= 494 merged=   0   ENA1  merged= 103   ENA2  merged=  87
INTRO_P  frames=1521 merged=  52   KADMAR1 merged=  94 KADMAR2 merged=  73
KARA     frames= 820 merged= 301   KONIEC  merged= 157 LONA  merged=  26
PRAWA    frames= 142 merged=   0   RAN     merged= 212 SADES merged= 131
ZARIA1   frames= 243 merged=  39   ZARIA2  merged= 108
```
1383 merged frames total; PRAWA/LOGO/AUTORZY untouched (PRAWA has zero
regions: pure silence; LOGO/INTRO animation runs REJECTED by the
additive test as motion). Full replay smoke (every .LVM through the C
reader, expectations audited independently by walking the container's
record flags): ALL 15 MATCH -
```
AUTORZY 494/494  ENA1 260/260  ENA2 171/171  INTRO_P 1472/1472*
KADMAR1 234/234  KADMAR2 296/296  KARA 519/519  KONIEC 417/417*
LOGO 125/125     LONA 391/391    PRAWA 142/142  RAN 187/187
SADES 343/343    ZARIA1 204/204  ZARIA2 210/210
```
(* = the clean_merged_pal cases above; a naive frames-minus-merged
expectation is wrong only for these two). The record-walk of every
container also ends exactly at EOF (RLE self-delimiting verified).
Visual check via Gemini on matched
timecodes (/tmp/opencode/lvm/vis): mid-run ghost smears "completely
eliminated" (ZARIA1 bird trail, KADMAR1 double-exposure head); the
cost is exactly the designed one - during a paint run the pre-run
scene holds up to ~1.5 s longer (pair verdicts "cleaner" vs the
accepted "holds the previous pose" trade-off).

## PR-42 (SHELVED 2026-11-16) — playback smoothing, made to look better than the original

Owner picked option 2 earlier ("make the movies look *better* than the
original", not accept the authentic low-fps look, not just add fades).
Prototype only - no C code landed; at review time the shipped state
was PR-41, itself SHELVED the same day (record above). The prototype
lives at tools/proto_smooth.py (was /tmp/opencode/smooth/proto.py).

What was measured first (root-cause, QEMU ground-truth rig):
- Pixels AND frame sequence/timing are byte-identical to the original
  (every static QEMU screendump matched a decoded frame at diff 0.0);
  the menu is equally soft on both paths. Our port is faithful.
- The "ghosty/oversharpen" look is the .IS data itself: natively
  low-framerate progressive painting (7-14 fps for ZARIA/LONA/etc,
  190 s of black+narration before any picture in INTRO_P). The
  original player additionally inserts its own black segment fades,
  which our standalone player lacks - those were the only QEMU screens
  that matched no single frame.

What the prototype tested (60 fps, side-by-side CURRENT|BLEND|MOTION,
normal + 2x slow, on ZARIA1 full + LONA motion burst):
- BLEND: dissolve each swap across its own hold to fill 60 fps.
- MOTION: 16x16 block motion (SAD, 4 px steps) + block translation to
  synthesize in-between frames.
Both are strictly inside each record's existing hold, so they never
move the audio timeline (that was the design contract, not reached).

Owner verdict after viewing: "all have same artifacts" - the artifact
lives in the 1997 source frames, so playback-side interpolation does
not remove it. Follow-up test confirmed the frames are keyframes, not
baked crossfades: 0/200 consecutive triplets fit a blend-of-neighbours
model (best alpha residual was never <25% of the direct frame diff).
=> smoothing does not address the real artifact; SHELVED. Revisit only
if a source-level fix (the de-ghost filter exists in tools/mv_clean.py) is found to help.

## PR-43 — playable level prototype M4-B: ll_play + real LEW.MOV physics (DONE 2026-11-16)

`ll_level <DATA> RAN.01 --play` runs the LION on the level's own data
(walk FWD/BKWD, edge fall, landing, world-bottom respawn). New SDL-free
module ported/ll_play.c/.h: .plt ground parser (ll_plt_*), the EXTENDED
.MOV parser (ll_mov_t = viewer mov_t + F=/T=/G= per-state physics +
TARGET(p)=COND transitions + control= flags + per-state seq= art, see
PLAN 8g for the grammar), and the player runtime (ll_player_step on a
fixed 20 ms grid; physics constants come FROM the data: walk F=400
T=90, STAY brake F=600). ll_level.c keeps a single .MOV parser (its
mov_t moved to ll_play.c; scene objects switched to heap ll_mov_t -
24 x ~100 KB embedded would blow the stack), loads the player wiring
from the .des (lew= leaf art + .MOV, platformy= .PLT, pozycja= spawn
snapped down onto the ground, lew_rect= bounds + death) and draws the
lion LAST at the .SEQ anchor (-f0,-f2) - anchor semantics confirmed
from UJ09.SEQ f0=35/f2=68 (feet at the ground line). Live window path:
held arrows (LL_EV_KEYDOWN/KEYUP) drive the lion, camera follows.

Correction of a PR-42 era 'data hole' (important): the plan to borrow
ZARIA.02 lion art for the playable prototype was WRONG - my ad-hoc
inventory probes re-introduced the PR-36 sparse-table bug (scanning
only `count` index slots), so LEW.RAN looked empty. With the correct
scan (port/llrs.c llrs_block already right; tools/extract_record.py
now FIXED the same way) BOTH test levels carry complete player data:
RAN.01 (LEW.RAN: 8 .SEQ incl. UJ09 walk cycle; LEW.MOV 30 states;
RAN.PLT 153 ground lines) and TUNGA.11 (LEW.TUN art; own 36-state
LEW.MOV that OPENS with the scripted A_GIVE_EGG egg intro - the
runtime plays it untouched; TUNGA.PLT; lew_rect -100..1570). No
substitution, no level special-casing.

Documented simplifications (first honest ones, see PLAN 8g): one
project constant LL_PLAY_GRAVITY=200 px/s^2 (same magnitude as the
real A_START_LAD1 G=0,200) only when a CHECK_LN state loses its
ground with no authored fall route - RAN.01's authored fall chain
(A_START_LAD1 -> K_GEN2 -> A_LADOW) is an 18.4 s scripted waterfall,
not a curb step; landing still fires the K_GEN2 pulse so those states
route authentically. Ground following = +-16 px snap window
(LL_PLAY_STEP) - RAN.PLT's 20 px same-height gaps are walked, deep
falls fall. `speed=` keys ignored (the .MOV T= caps are the authored
speeds). No jump/crouch yet; K_UP/K_DOWN reach the transition layer
but no states consume them in v1.

Two fixes surfaced by the PR's own checks: `--play` uses `SDL_setenv`
(the first `make win` run of this PR failed - mingw has no `setenv`;
tools/ll_level.c already links SDL) and a PRE-EXISTING memory leak in
the .des placement path (ll_level.c `des_place_lines`: the "no
drawable element" branch freed the blob but not the parsed `els` table
the main object loop frees). Repro recipe (also what the ci.yml asan
job now covers for the hermetic part - `--play-selftest` added there):

    gcc -O1 -g -std=gnu11 -Wall -Wextra -fsanitize=address,undefined \
        -Iported $(pkg-config --cflags sdl2) -o /tmp/opencode/ll_level_asan \
        tools/ll_level.c ported/ll_level.c ported/ll_play.c ported/llrs.c \
        ported/ll_pcx.c ported/ll_pcm.c ported/ll_seq.c ported/ll_mod.c \
        ported/fs_ci.c ported/ll_view.c ported/ll_audio.c \
        ported/platform_sdl.c ported/platform_common.c \
        $(pkg-config --libs sdl2) -lm
    LEO_HD_DIR=/tmp/opencode/hd_files LEO_HELP_SECS=0 LEO_LEVEL_SECS=2 \
    LEO_PLAY_SCRIPT="1:FWD 100:BKWD" LEO_LEVEL_PPM_TICK=20 \
    LEO_LEVEL_PPM=/tmp/x.ppm /tmp/opencode/ll_level_asan <DATA> RAN.01 --play

Before the fix: "Direct leak of 20736 byte(s) in 432 object(s)" (also
in the PREVIEW path - not a PR-43 regression). After: ASan+LSan+UBSan
clean, rc=0, zero errors, for both the preview and the play run.

Usage: `ll_level --play-selftest` (hermetic, always runs), `--play`
or LEO_PLAY=1 on any level; headless CI driving: LEO_PLAY_SCRIPT=
"<tick>:<FWD|BKWD|UP|DOWN|NONE>" entries held until the next entry,
LEO_LEVEL_PPM(_TICK) dumps deterministic frames on the fixed grid.
CI: new verify-play in verify-all (selftest leg in EVERY config;
data legs = RAN.01 scripted walk/reverse/stop + the same demo run
further pinning the .plt-gap fall -> lew_rect death -> pozycja respawn
+ PPM differential NONE-vs-FWD + TUNGA.11 per-level proof);
fixture-free config prints its SKIP. verify-level pins unchanged (preview path untouched, and the
new claim step is LEO_PLAY-gated; LEW leaves never appear in the
scene lists on these two levels anyway - the 24-slot cap cuts them).

Verification (2026-11-16): `make -k verify-all` with the CD+HD
fixtures = EXIT=0 / "ALL TESTS PASSED (linux + windows + llrs)", the
six verify-play legs ok; fixture-free config runs the selftest and
prints the named SKIP; `make` and `make win` warning-free; verify-arch
green (ll_play.c/.h are SDL-free - the 4 allowed files unchanged);
the ASan/LSan/UBSan recipe above rc=0 with zero output.

PR-43 follow-up (same day): the LIVE window path was the one part of
M4-B still unproven headless (the scripted legs drive `LEO_PLAY_SCRIPT`
and bypass the session loop). Fixed by extending the existing key
injector (platform_sdl.c `LEO_TEST_KEY`) with the arrow names
`left`/`right`/`up`/`down` - it pushes a real SDL_KEYDOWN, so the
event travels map_key -> LL_EV_KEYDOWN -> the held-key tracking ->
fixed-substep physics -> camera follow; the injected RIGHT press makes
the lion take the A_STAY -> A_WALK_FWD transition in a 2 s real
session, which the new verify-play "live input" leg pins. Same
injector also gained nothing else: no behaviour change for printable
keys/esc/space (verify-quit/verify-menu legs unchanged and green).

Then the same probe approach found two REAL-DATA grammar gaps (both
fixed, both pinned):
- `.plt` point separator `!` is an exact equivalent of `;` INSIDE one
  polyline (real bytes: ENA.PLT `1 1,560!7500,570;`, ZARIA_A.PLT uses
  it 10 times; the leading segment count still counts segments).
  Before: those grounds parsed to 0 lines and ENA.03/ZARIA.01 reported
  "lacks ground .PLT" - a false data hole. Now ENA.03 wires (1 line,
  spawn snapped to 100,560, state A_SWIM) and ZARIA.01 gains 45 lines.
- A transition that targets its OWN state (KADMAR2's real
  `A_MASZERUJ(x)=K_FWD`) means KEEP running. Re-entering it reset the
  frame list every tick - measured: 50 `A_MASZERUJ -> A_MASZERUJ`
  events in 50 ticks, animation frozen on frame 1 while the lion slid
  at the state's speed. Self-targets now change nothing (facing still
  updates). Side fact from the same line: the priority token may be
  NON-NUMERIC (`(x)`, `(X)`) - the parser never required a number.
- `ll_level.c` also prints a leaf/.MOV name-mismatch diagnostic
  instead of staying silent. Measured: it never fires - all 13 .des
  name a leaf AND a .MOV that really exist; the code still never
  guesses a script (ZARIA's leaves carry an extra LEWPL.MOV its .des
  does not name).

GAME-WIDE measurement (full CD + HD-install trees, sparse-correct
scan; script /tmp/opencode/play_matrix.sh runs one headless --play
per level block): 10 of the 13 level blocks run their OWN lion data
unchanged (SUPERSEDED by the PR-44 record below: 12 of 13). - RAN.01 30, TUNGA.11 36, ENA.03 23, ENA2 17, KARA.07 23,
KADMAR.07 37 (its .des names a leaf literally called "0", which
really exists with TURN/LOOKUP/JUMPS/LAZY2 .SEQ), KADMAR2 14, LONA.05
40, ZARIA.01 37, ZARIA.02 31. The 3 that stay preview-only are NOT
parser gaps: TUNGA2.12/SADES.06 say `platformy=platf.des` and
JUNCTION says `platformy=none.des` - ground defined inside a sub-.des
is M4-C work. Also recorded in PLAN 8g: LEVELDES records are named
after the CHAPTER (RAN.DES, ENA1.DES, KADMAR1.DES) while
ANIMACJE/ETAPY blocks are named after the STAGE (RAN.01, ENA.03,
KADMAR.07) - only the stage names work on the CLI, and .PLT exists
only in the HD install.

verify-play is now EIGHT legs (selftest, wiring, transitions,
fall/respawn, render differential, live input, NEW multi-level matrix
over ENA.03/ENA2/KARA.07/KADMAR.07/KADMAR2/LONA.05/ZARIA.01/
ZARIA.02 with ENA.PLT `!` + A_SWIM pins, TUNGA.11); `--play-selftest`
grew the matching coverage (`!` separator, self-transition keeps its
state and lets frames keep running, non-numeric priority token).
Unknown .MOV condition names in the real data (inventory measured:
K_FIRE 253 uses, K_JUMP 76, AC_ONAIR 199, AC_ENEMY_ATC 144, K_F1..
F12, K_ALT, ...) all evaluate FALSE - nothing is guessed; K_JUMP +
the AC_* signals are the M4-C list.
 - PR-43 pushed 73dec71; CI run 43783591185: linux + asan +
   windows-smoke ALL green on the first pass. CLOSED. Ready-to-apply
   CI follow-up (NOT pushed: our gh token has no `workflow` scope, so
   this repo cannot change .github/workflows - owner applies it): in
   the asan job, after the existing `./ll_level --mov-selftest` line
   add `timeout 30 ./ll_level --play-selftest` - hermetic, so the
   player physics gets sanitizer coverage without fixtures (verified
   locally under the asan job's exact ASAN/UBSAN_OPTIONS: rc=0).
 - PR-43 follow-up pushed 47dea87; CI run 43821851544: linux + asan +
   windows-smoke ALL green on the first pass (the 8 verify-play legs
   ran on the real CD+HD fixtures there). Verification before pushing:
   local `make -k verify-all` EXIT=0 / "ALL TESTS PASSED (linux +
   windows + llrs)"; fixture-free config (`make -k verify-all
   DATA=<absent> HD=<absent>`) EXIT=0 with 23 named SKIPs and the
   selftest still running; `make` + `make win` warning-free;
   verify-arch green; the ASan/LSan/UBSan ll_level build clean
   (rc=0, empty stderr) on the selftest, the scripted RAN.01 play,
   the live-key session and ENA.03. CLOSED.

## PR-44 — M4-C step 1: the SECOND real form of `platformy=` (ground
inside a sub-.des) + the authored-EMPTY ground + the bare `none`
keyword (DONE 2026-11-16)

The probe (`/tmp/opencode/play_matrix.sh`, one headless `--play` per
level block) left 3 of 13 levels printing "lacks ground (platformy=)".
Reading their own level blocks in the HD `ETAPY` container closed all
three - and none of them was a missing file:
- `platformy=` has TWO real forms and the ground grammar is IDENTICAL
  in both. 11 levels name `<stem>.plt` (the block's <stem>.PLT
  record). TUNGA2.12 and SADES.06 name `platf.des` - and that block's
  PLATF.DES record's TEXT is the very same polyline file, count header
  and all: "66\r\n1 1495,147;1529,147;\r\n..." and
  "8\r\n1 863,166;...". So `ll_plt_parse` already parsed it; only the
  RECORD RESOLUTION was missing (`stem_of` + `.DES` instead of
  `.PLT`, one `plt_is_des` flag, no second parser). Both levels now
  run their own lion on their own ground:
  TUNGA2.12 `PLATF.DES: 66 polyline(s)` + LEW.TUN 33 states, spawn
  snapped (50,320) grounded; SADES.06 8 polylines + LEW.SAD 37
  states, spawn (20,166) grounded.
- the THIRD answer is an AUTHORED EMPTY ground: JUNCTION says
  `platformy=none.des` and its NONE.DES record is literally the byte
  "0". That is data ("this level has no floor"), not a gap, so
  `ll_plt_parse` now separates "declared 0" (a complete file: rc 0,
  n 0) from "nothing usable" (no count, or a count with no geometry:
  rc 1) - the `declared` value is kept, the selftest pins
  `"0" -> 0`, `"3\r\n" -> 1`, and the log says
  `play: NONE.DES: 0 polyline(s) ... the data says this level has NO
  ground` followed by "has NO ground in its own data" instead of the
  "lacks ground (platformy=)" data-hole note. Such a level stays a
  preview: nothing in the data asks for walking on air.
- a bare `none` in a .des list (JUNCTION `animacje=none`, RAN.01
  `zwierzaki=chmury.des,ptaki.des,none`, `sample=none`,
  `ani_plat=none,...`) is the game's OWN keyword for "nothing here" -
  not a missing record. Before it produced 6 false
  "not in the HD level block (data hole)" notes (RAN.01 x2,
  JUNCTION x2, KARA.07, KADMAR.07). `des_placement` now skips it with
  an honest keyword note and counts it as an ANSWER, so the
  "no animacje= list: try any .DES" fallback no longer invents
  placements the level never asked for (JUNCTION's preview stays at
  its real 1 animated object, so the verify-help JUNCTION pixel pins
  are untouched). `none.des` WITH the extension is a different thing -
  a real record name, and it resolves to that NONE.DES normally.

Result: 12 of 13 level blocks run their own lion data; the one that
does not (JUNCTION) is that way BY ITS OWN DATA. PLAN 8g carries the
grammar and the full per-level inventory.

verify-play is now TEN legs: the eight from PR-43 + ground-from-.des
(TUNGA2.12 / SADES.06: must read PLATF.DES 66 / 8 polylines, snap the
spawn onto it (50,320)/(20,166) and run a state by tick 30) +
authored-empty ground (JUNCTION: must print `NONE.DES: 0 polyline(s)`
and "has NO ground in its own data", and must NOT contain the words
"lacks ground", "not in the HD level block", or any invented
"placement" line - the negatives are what pin the honesty).
`--play-selftest`'s own summary line now names the new case ("an
authored-empty '0' ground is data not an error"), which is the text
CI prints.

Verification (2026-11-16, script /tmp/opencode/verify_pr44_final.sh,
summary /tmp/opencode/v44_summary.txt): `make` and `make win`
warning-free; the ASan/LSan/UBSan `ll_level` build clean and every run
rc=0 with ZERO sanitizer output - `--play-selftest`, and BOTH the
`--play` and the plain-preview path of TUNGA2.12, SADES.06, JUNCTION
and RAN.01 (the new code paths: sub-.des ground, authored-empty
ground, the `none` keyword skip); `make -k verify-all` on the CD+HD
fixtures EXIT=0 with all TEN verify-play legs reported ok and no
target failures; the fixture-free config (`DATA=<absent> HD=<absent>`)
EXIT=0 with 23 named SKIPs, zero failures, and the selftest still
running; verify-arch green (no new SDL include - ll_play/ll_level stay
engine-side).
  - PR-44 pushed 634cb82; CI run 43824561094: linux + asan +
    windows-smoke ALL green on the first pass. On the runner the
    hermetic leg printed the new summary line ("an authored-empty '0'
    ground is data not an error") and the ten data legs printed their
    named SKIP, exactly as the fixture-free contract requires. CLOSED.

## PR-45 — M4-C step 2: the authored scene — placed objects run their own .MOV, art by each state's seq= (DONE 2026-11-16)

  - What: replaced the PR-36 invented scene (one object per ANIMACJE leaf
    .SEQ, joined by .mov stem) with the level's OWN scene. ll_level now
    reads every file named by the three placement keys of the level's
    main .des (`zwierzaki=`/`animacje=`/`ani_plat=` - 68 files game-wide,
    0 references to missing records), parses all 2188 entry lines, and
    joins each to the HD <LEAF>/<SCRIPT>.MOV script and, PER STATE, to
    the .SEQ art that script's own `seq=` key names in the same leaf.
    No fixed object cap (grown per level; measured maxima 246 art tables
    / 164 scripts SADES.06, 827 placements RAN.01). The PR-36 fallback
    scan is gone.
  - Entry grammar (ll_level.c place_field/place_int/place_comma): the
    game's own formatting is parsed as the game wrote it - spaces around
    fields (109 lines, incl. RAN.01 DYMY.DES's 107 ", 16500, 360") are
    the same names; a space INSIDE a field is not a name (this is what
    keeps polyline lines out, no %n trick needed); 4 entries carry
    parameters after the coordinates and 1 names a script with no
    extension (KADMAR.07 `BERLO,dummy,sdfsdf,61,54`) - placed/reported
    as authored, counted verbatim (`param_extra`/`entry_noext`), never
    guessed at; count lines are never trusted (stale in 38 of 68 files,
    reported with both numbers). The totals the binary prints (2188
    entries, 1857 placed, 331 outside the level's own view) match the
    independent python census (/tmp/opencode/pr45_census.py) exactly -
    the earlier 2075/2188 gap was MY strict sscanf replica, fixed by
    reading the formatting the data actually uses.
  - Art-form distinction (a real data/art finding): .SEQ elements come
    in two forms (PLAN 8 grammar). obj_art.found separates "no such
    record" (data hole, named exactly) from "record is there, element
    carries the inline palette+sprite form" (VIEWER gap, reported as
    such, never counted as data) [RETRACTED in PR-46: those byte lists
    are per-row collision margins, the 169 tables are invisible solids
    the original never drew - not a viewer gap]: 953 of 960 authored art names are on
    this CD - 784 patch-form (drawn), 169 inline-form (draw nothing,
    incl. all 48 RAN.01 ANIBACK EDGE*.SEQ), 7 true holes. The old
    build's 469 stem-join "data hole" notes were false; the new
    report's holes are 27 real missing .MOV + 7 real missing art.
  - Proof: --mov-selftest extended - data-free leg now pins per-state
    seq= art selection (a state gets exactly its own table; names it
    does not carry, and inline-form tables, draw NOTHING). verify-level
    rewritten: TUNGA.11 authored-scene pins (7 files, 77->69+8, stale
    count lines, the one ogien_m.SEQ hole named, bare 'none' never a
    fake hole), fixed-grid advance naming the SCRIPT ("tick 8:
    ANIBACK/strum1.mov state 0 frame 0", "57 more placement(s)
    advanced"), tick2!=tick8 and cam300 PPM differentials, new
    LEO_OBJ_ALL=1 leg (all 69 placements named; MOVEPLAT/ed000002.mov
    art ED000002.SEQ at its authored 599,164 - now from the level's
    own ani_plat=), RAN.01 canvas pins (827 objects, CHMURY.DES named,
    48 inline tables distinguished by name from data). JUNCTION
    authored-EMPTY scene positive pin added to the play leg
    ("authored scene JUNCTION: 0 placement file(s), 0 object").
  - Bounded prints (24 placements / 12 tick lines + counts of the rest);
    LEO_OBJ_ALL=1 prints all (documented in README).
  - Docs: PLAN 8h carries the full grammar/census facts (three keys,
    68 files, 2188 entries, stale-count 38/68, spaces/extra-params/
    no-ext entry, the seq= join vs the 439/681 stem coincidence, the
    784/169/7 art-form split, per-level totals).
  - Verification: make + make win clean; full verify-all incl. the
    rewritten verify-level legs green on the real fixtures; the
    fixture-free config EXIT=0 with named SKIPs and the mov selftest
    still running; verify-arch unchanged (no new SDL include). ASan/
    LSan/UBSan run of ll_level (selftest + full-data level + preview)
    clean. Peak child RSS unchanged (1114 MB; a one-off measurement, no pin exists in the Makefile or CI).
  - PR-45 pushed 3cb067d; CI run 43836226298: linux + asan +
    windows-smoke ALL green on the first pass. On the runner the
    hermetic mov selftest printed its extended summary ("per-state seq=
    art selection (ok)") in both the asan job and the linux full suite,
    while the rewritten verify-level leg and the verify-play legs
    printed their named SKIPs - exactly as the fixture-free contract
    requires. CLOSED.

## PR-46 — M4-C step 3: the element SHAPE geometry — the w0&3 lists are
          per-row collision margins, not art; the shipped wording
          corrected (DONE 2026-11-16)

  - What: the shape model ported (ported/ll_seq.c: ll_seq_shape_wh,
    _bbox = FUN_000420b0 verbatim, _hit = FUN_00042a70, _empty) over
    real data, a C-vs-python oracle for it (tools/shape_geom.py twin +
    new tools/seq_shapedump.c: verify-seq --shape now dumps, per
    payload, every element's shape frame + bbox + empty flag and a
    17-position/facing self-test grid per shape element, compared
    byte-exact on all 1656 container payloads), and the honest
    reclassification of what PR-45 printed.
  - RE (closes the reading; PLAN 8/8h carries the full facts):
    * the w0&3 byte lists are per-row LEFT/RIGHT margins: row i is
      solid on x = [left[i], tw - right[i]] with tw = int16(b19)*0x50 +
      int16(b15), rows = int16(b17); byte 0xFF = empty row (skipped by
      the edge scans, and a LEFT 0xFF takes that row out of the span
      test; an INTERIOR 0xFF participates as 255). They are NOT
      "palette + sprite": w0&1 = patch art, w0&2 = shape, and no real
      element is neither (w0 = 0/4 never occurs).
    * the 169 "inline" tables PR-45 named are w0=2 shape-only INVISIBLE
      SOLIDS - every draw path in the game gates on the element's patch
      pointer array (+0x1f), so the ORIGINAL game never drew them
      either. "Viewer gap / drawing them is the next step" was wrong
      twice; the reports now name them as solids with their measured
      geometry (EDGE0000.SEQ: tw 28, 19 rows, bbox 1 0 28 20) and say
      the viewer has no object-collider model to use them with YET.
    * loader FUN_00041f60: no lists -> rect straight from the words;
      lists -> FUN_000420b0 derives the bbox. Positioning (identical in
      every consumer): x -= f0, y -= f2, both skipped when f0 is the 0x7fff ctor
      sentinel (the game gates on f0 alone; corrected in PR-47's doc
      pass); facing < 0 uses (tw - f0) and swaps the margin lists.
      The game's box h = (last_row - first_row) + 2 (one row MORE than
      the scanned rows - its own arithmetic, reproduced); no read ever
      leaves the declared rows.
    * measured degenerate case (replaces PR-45's WRONG "one byte past
      the buffer" note, which is retracted): 34 shape elements declare
      rows whose margin lists are BOTH wholly 0xFF - both scans end
      having found nothing and the game stores w = 1 - tw, h = 1 - rows.
      Flagged ll_seq_shape_empty / dump token "shape_empty 1", counted
      on both sides of the oracle (no one-sided case exists in data).
    * machine-code RE (build/leo/leo.elf, objdump of sliced chunks,
      VA = file offset + 0xf000): the decompiled text of FUN_00042a70/
      FUN_00042de0 loses their register arguments ("unaff_EBX"), so the
      row window was read from the code at 0x42b85..0x42be6 /
      0x42f21..0x42f7c: the element lower on screen (larger adjusted y) indexes
      from row 0, the other from the distance between them, over the
      shorter remaining span (min(rows_a+dy, rows_b) / min(rows_b-dy,
      rows_a)). The shape_window the tree shipped was TRANSPOSED -
      fixed in ported/ll_seq.c (and in the python twin), pinned by two
      direction cases in the new selftest leg.
  - The push-out (FUN_00042de0 + helper FUN_00042cf0) is NOT modeled,
    and ll_seq_shape_push + the oracle's push legs were REMOVED rather
    than shipped half-resolved: the machine code shows the structure
    (which side gets pushed decided by signed "edge position" sums
    6*x + 3*tw + margin deltas at first/middle/last window row; the
    helper scans w - list1[i] - (list2[i] + EBX) with EBX a spilled
    register input (0x42cfe), keeps max penetration + first/last rows
    reaching it and interpolates (0x42d55..0x42dcf); a final clamp
    against |position.x + tw|), but the operand identities at both call
    sites and the meaning of the two output pointers are unresolved -
    the callers (0x530d1/0x53184) go through element-vtable slots
    +0x48/+0x50 that are not mapped. A value computed from that would
    be a guess: documented in ll_seq.h ("NOT MODELED") and in the
    M4-C list instead.
  - Proof: --mov-selftest gained a data-free shape-geometry leg (runs
    in EVERY config incl. the CI asan job): a synthetic 9-element .SEQ
    (186 B, grammar-exact: shape-only elements skip the patch
    sub-blocks; the art-only element carries its one empty array)
    parsed by ll_seq_parse, then pinned element-by-element against
    hand-derived constants - wh incl. the b19*0x50 multiplier, edge
    0xFF runs (h = 0 case), the both-empty degenerate box + the empty
    flag, anchor shift and the 0x7fff sentinel, per-row right margin,
    the no-lists participation gate, x/y adjacency bounds (x=19 hits /
    x=20 not, y=3/4), the two window-direction pins, and the facing
    list swap. The same payload run through the python twin produces
    the identical table (two independent implementations; constants
    never taken from the port itself).
  - Wording pass: ll_level per-table reports + summary ("44 carry the
    patch art this viewer draws, 44 of those also carry the shape
    margins, 48 carry shape geometry only - invisible solids the
    original game never drew either"), obj-listing why-strings, the
    doc-comment census rewritten to element-level facts (8048 art+shape
    elements w0=15, 92 art-only w0=1, 169 shape-only w0=2 across the
    953 named tables), Makefile RAN.01 pins re-measured and re-pinned
    (the new summary string + the EDGE0000 geometry line), README's
    verify-seq line corrected (it still said "172 payloads"; the walk
    recurses every container = 1656), PLAN 8 grammar annotation + 8h
    rewritten.
   - A new measured gap, named rather than guessed (probe over the HD
     ANIMACJE block tree; facts in PLAN 8g): of the 777 HD .MOV
     records, 775 carry control= flag lists (2256 lines) and 415 of
     them carry at least one token our ctrl_flag table does not model -
     13 distinct tokens: CHECK_LEW 535 lines/205 scripts, DANGER
     385/195, CHECK_ANI 168/59, SOLID2 152/132, ANTI_POCISK 121/121,
     SEMI_SOLID 92/25, ANTI_SOLID 87/30, CHECK_ZW 47/22, SEQ_REV_Y
     22/10, BY_TY 12/7, CHECK_BASE_LN 8/8, SEQ_REV_X 7/7, INV_V 3/3.
     Note the SOLID-family (SOLID is modelled as a bit; SOLID2/
     SEMI_SOLID/ANTI_SOLID are not) - they are exactly the object-
     collision behaviour the (unmodeled) push-out feeds, so this and
     the push-out gap are one and the same line of work.
   - Verification: make + make win warning-free; FULL verify-all on the
     real fixtures EXIT=0 (no SKIPs) - the extended verify-seq printed
     "1656/1656 payloads byte-exact (grammar + patch rows + shape
     geometry, 14306 shape elements, 34 with both margin lists
     empty)", verify-level green with the re-pinned RAN.01 strings and
     the unchanged TUNGA.11/ED000002 pins; the standalone C-vs-python
     shape cross-check over every payload diffed 0 lines (243202
     shapehit lines, 146500 hits, 34 shape_empty - both sides).
     Fixture-free config (DATA/HD pointed at an absent dir) EXIT=0:
     21 named data SKIPs and the data-free legs still printed "(ok)"
     (the extended mov selftest incl. shape geometry, the play
     selftest, verify-arch). Local asan/ubsan/lsan build of
     ll_level --mov-selftest rc=0 with zero sanitizer output (same leg
     the CI asan job runs). verify-arch unchanged. Peak child RSS
     1113 MB on RAN.01 (pinned 1114, unchanged).
   - PR-46 pushed c239ae0; CI run 43845936475: linux + windows-smoke +
     asan ALL green on the first pass. On the runner the extended mov
     selftest printed its new summary ("... per-state seq= art
     selection + shape geometry (ok)") in BOTH the asan job and the
     linux full suite, while verify-seq and verify-level printed their
     named SKIPs (the runner stages no game data) - exactly as the
     fixture-free contract requires. CLOSED.

## PR-47 — verification harness: the suite now fails when it should
          (from VERIFICATION_2026-11-16.md, DONE 2026-11-16)

  - Why: the fresh-clone audit found legs that passed with nothing
    checked. All fixed here, none of them touched the port itself.
  - verify-cmake: the data branch reused /tmp/opencode/cmbuild, so any
    other checkout failed ("source does not match cache"); both
    branches now wipe cmbuild/cmwin first.
  - verify-llrs / verify-win llrs leg: a missing container made both
    sides print nothing and `diff` called that exact. llrs_selftest now
    exits 1 on an unopenable container, the python side's status is
    checked, an empty listing is refused and the compare is a cmp of
    captured files. (Reproduced with /nonexistent before the fix.)
  - "frames differ = animation works" legs (menu/level/win/play): `cmp
    -s` treats a missing frame as "differs". New tools/frames_differ.sh
    (both exist, non-empty, differ) replaces all 6 uses and every
    producing command is preceded by an rm of its own frame, so a stale
    PPM can no longer satisfy a leg.
  - python reference frames: verify-skip/verify-win trusted
    py_last/f01520.ppm, which no target created (a clean machine
    skipped verify-win with the data present). New `py-refs` target
    regenerates c_py/f01020 + py_last/f01520 and is a prerequisite of
    verify, verify-skip, verify-win, verify-golden, verify-cmake;
    PX_WINREF now only needs the data.
  - verify_pcm.py / verify_help.py scanned `range(cnt)` of nested index
    tables (the PR-36 sparse-table bug): 4 of 23 PCM payloads were
    compared. Both scan every slot the block holds; the PCM pin is now
    23/23 (C == python on all 23, incl. RAN.01/PLAN2.PCM and the
    MASKA* layers that verify-level renders).
  - verify-arch also catches `<SDL.h>` / quoted SDL includes and pins
    that ll_view.h / ll_audio.h (which pull SDL in) are included only by
    the backend; negative-tested with a temporary offender.
  - tools: llrs_unpack.py key validity aligned with ported/llrs.c
    (non-empty printable run, rest NUL); verify_seq.py fails on a
    missing container instead of shrinking the corpus; checkfix.py and
    verify_help.py no longer hard-code paths; the ~20 Makefile legs that
    hard-coded /tmp/opencode/hd_files now use $(HD); `clean` removes
    seq_shapedump*; stale "172/49 payloads" comments corrected.
  - Still open (recorded, not fixed here): ll_mod.c has no independent
    python mixer oracle (verify-mod compares linux with wine, i.e. the
    same C twice); most python oracles are written from the same
    reading as the C. PR-48 closes the inner-gap blind spot of the SEQ
    patch oracle.
  - Verification: make -k of py-refs verify verify-skip verify-win
    verify-llrs verify-pcx verify-seq verify-pcm verify-help
    verify-menu verify-level verify-play verify-arch verify-cmake on the
    fixtures EXIT=0.

## PR-48 — TWO .SEQ row codecs: elements with w0&8 clear draw through
          FUN_0001d9b8, not FUN_0001dc80 (G-1 of the audit, DONE 2026-11-16)

  - Bug: ll_seq_patch_walk applied the FUN_0001dc80 row rule to EVERY
    element. The game picks the blitter per element: the loader stores
    obj+0x14 = w0&8 and the draw entry FUN_000426a0 calls FUN_0001d9b8
    when it is 0, FUN_0001dc80 otherwise. FUN_0001d9b8 (earlier read as an
    "erase variant", wrongly) has NO fill runs and treats every byte
    < 0x80 in a gap position as a plain advance - 0x40..0x7f are LONG
    gaps there, not dc80's short fill/copy markers. 106 elements use it
    (all w0 = 1); 48 of their 106 blobs paint different pixels: the
    player's LEW.ZAR/I.SEQ and LEW2.ZAR/I.SEQ (ZARIA.01/.02) and BERLO.SEQ
    in KADMAR.07/KADMAR2/KARA.07.
  - Independent evidence (not the port's own reading): the machine's
    stream walk of FUN_0001d9b8 WITHOUT the row length byte lands exactly
    on the declared row boundaries in 106/106 blobs (probe
    /tmp/opencode/vis/d9b8_exact.py); each blob ends in exactly one
    length-0 row, the end marker, in both codecs.
  - Fix: ll_seq_patch_walk_codec / ll_seq_patch_draw_codec /
    ll_seq_elem_codec (ll_seq.h); ll_seq_draw_elem_map and the element
    walkers in ll_menu.c/ll_level.c use the element's codec;
    ll_seq_patch_walk stays the dc80 default (row counting only).
  - Oracle blind spot closed (audit F-2/F-6): seq_patch.py now emits the
    inner gap as an op and both dumpers print it (g<cnt>), so the 630k
    inner gaps are compared C == python (891 in LEW.ZAR/I.SEQ alone);
    seq_patch.decode takes the codec; seq_render.sprite advances on gaps
    and uses the element's codec (its pixel positions were off by up to
    970 of 1768 on POMOC00 element 0). verify-seq: 1656/1656 byte-exact.
  - Proof: --mov-selftest gained a data-free codec leg (a row where the
    same bytes mean different pictures; w0 1/15/12 pick d9b8/dc80/dc80);
    mutation-tested (forcing dc80 makes it fail). fuzz.c exercises the
    d9b8 walker too.
  - Not moved: the golden chapter-map comparison (30 pose hashes, best
    92.4594%) is unchanged - the map cursor is not one of the 48 blobs,
    so the original-capture comparison cannot corroborate this fix; the
    machine-code walk above is the evidence.

## PR-49 — .MOV frame numbers are 1-based with the sign = mirror, and the
          element anchor belongs to the draw (G-2/G-3 of the audit, DONE
          2026-11-16)

  - Bug (visible on every level): the port used a .MOV frame number as a
    0-BASED element index, clamped out-of-range numbers to the last element,
    never mirrored, and drew scene objects at the raw .des position with no
    anchor. 4266 of 18432 authored frame numbers (23%) were out of range
    under that reading; negative numbers (pervasive: -N..-1) meant nothing.
  - RE (machine code of build/leo/leo.elf, VA = file offset + 0xf000):
    * the game's frame-line parser FUN_00051660 (0x51692..0x516d5): n > 0
      -> element n-1, frame+0x14 = 0; n < 0 -> element -(n+1) = |n|-1 with
      frame+0x14 = 1 (MIRRORED); n == 0 -> element index 0 and an object
      from FUN_00044110() (a placeholder pulled from a global resource at
      game+0x398: NOT modeled - frame 0 draws nothing).
    * the four element draw methods (element vtable +0x4c/+0x50/+0x54/
      +0x58 = 0x42230/0x422f0/0x423c0/0x42480) are the ONLY code that tests
      the 0x7fff sentinel: the anchor is subtracted INSIDE the draw, the
      actor adds nothing back. plain: origin (x-f0, y-f2), sentinel -> (x,y);
      mirrored: (x - tw + f0, y - f2), sentinel -> (x - tw, y). +0x54/+0x58
      use the second anchor pair (f4,f6): unused by this port. The mirrored
      blitters FUN_0001daa4/dda8 write each row right to left from x+w-1,
      and FUN_000426a0's flip branch lays the chunks out right to left, so
      the mirrored element is the plain image flipped across tw.
    * the state parser FUN_00051910 shows what the unmodeled control= flags
      are: SEQ_REV_X / SEQ_REV_Y (reversed sequence), CHECK_ZW / CHECK_ANI /
      CHECK_LEW ... (still counted as ctrl_other, not acted on).
  - Fix: ll_mov_frame_ref (ll_play.c), ll_seq_anchor_origin /
    ll_seq_draw_elem_anchor (ll_seq.c), obj_elem/obj_draw/play_draw use
    them; a number that names no element (0, or past the table) draws
    NOTHING instead of the last element; the cull rectangle covers the
    anchor and both facings (obj_art rx/ry over all elements).
  - Independent confirmation, not the port's own output: under the game's
    grammar ZERO frame numbers are out of range on every level (RAN.01 747
    frames, 91 mirrored, 7 blank, 0 out of range - pinned in verify-level;
    the old reading had 23% out of range), and the renders assemble into
    pictures they did not before (ENA.03: the wreck now carries its mast and
    pirate flag, seaweed appears, fish have bodies and fins; TUNGA.11: the
    rope bridge hangs from its ropes instead of a scatter of vertical
    lines). The three black 40x80 boxes on KADMAR.07 (SOLID box40 objects
    drawn through the old last-element clamp) are gone.
  - Proof: --mov-selftest anchored-draw leg (frame grammar table + a
    synthetic element drawn plain/mirrored/sentinel-plain/sentinel-mirrored,
    hand-derived pixels); mutation-tested four ways (mirror origin, anchor
    y, 0-based grammar, mirror direction - all caught). The per-level report
    prints the frame counts.
  - Harness note: the new report line moved a stdout buffer boundary onto
    the 'play: LEW.RAN' line, exposing a latent race (block-buffered stdout
    vs unbuffered stderr split a line in the log); tools/ll_level.c now
    line-buffers stdout.
  - Open (recorded, not guessed): SEQ_REV_X/Y and FRAME_REV flags change
    how a state is drawn (counted under ctrl_other); the second anchor pair;
    frame 0's placeholder; objects authored at (0,0) (G-7) are drawn there.

## PR-50 — PCM parallax layers: investigated, NOT resolved (G-4 of the audit,
          2026-11-16) [RESOLVED in PR-53 below - the 'no layout gives a picture'
          conclusion came from reading the WRONG layer class]

  - Symptom: KADMAR2 (tlo = plan_.pcm only) shows a 32-row strip of noise
    over a never-written canvas; LONA.05's PLAN3_.PCX rows 200..600 are
    100% key colour (0,0,255) with PLAN12.PCM reported as "0x0 rows".
  - New facts (machine code + data, probes /tmp/opencode/vis/pcm_*.py):
    * the layer constructor FUN_00056b20 decodes exactly `rows` (header
      field 2) rows of `w` bytes from the RLE stream (FUN_00044b80: one call
      = one row, `(b & 0xc0) == 0xc0` = repeat) and copies each row's first
      80 bytes into the 80-byte gutter after it: a layer is a thin w x rows
      BAND (RAN PLAN1 400x40, KADMAR2 1800x32 ...). The multi-MB remainder
      of every payload (KADMAR2 2.76 MB for a 57 KB band) has no known
      consumer in the constructor.
    * the 40-entry offset table has exactly `rows` valid entries in every
      payload (RAN 40, TUNGA.11 32, KADMAR2 32, LONA 31): one entry per
      band row, but the segments between consecutive entries decode to
      irregular lengths (17503, 30655, 9674 ... never a multiple of w or
      80), so they are not row/strip units.
    * the decoded band is NOT a picture under either framing (pitch w and
      pitch w+80 both give streaky noise); byte autocorrelation finds no
      row pitch >= 40 in 3 of 4 payloads (KADMAR2 peaks at 80 = the strip
      width, weakly). The RLE rule and the 0xa6 start are the game's own;
      what is unknown is how the stream's samples become pixels.
  - Decision: no code change. A guessed layout would paint invented art
    (the project rule). verify_pcm now covers all 23 payloads (PR-47), so a
    future fix has the oracle. Next step for whoever resumes: decompile the
    FUN_00057250/57460 strip manager and FUN_0001d528 copy to see what
    consumes the table and the stream beyond the first `rows` rows.

## PR-51 — viewer camera, backdrop, declared start state (G-5/G-6/G-7 of
          the audit, DONE 2026-11-16)

  - G-5 (VIEWER POLICY, not RE'd): the camera was x-only, so the lion
    spawned below the view on 7 of 12 levels (feet y 320..1504 vs views
    200..600 px). A vertical camera now follows the lion's feet (65% down
    the view) in play mode and in the CI play leg; LEO_LEVEL_CAMY sets it
    for CI, Up/Down pan it in the viewer. Verified on ZARIA.01, ENA2,
    KADMAR.07, TUNGA.11: the lion is on screen.
  - G-6 (VIEWER POLICY): the .PCX backdrop is now fixed instead of tiled.
    The view equals the image size, so tiling could only repeat a
    non-seamless image with a hard cut mid-screen (KADMAR.07 cam 1280,
    KARA.07). The original scrolls the backdrop by a per-plane parallax
    factor (FUN_00057530/57500 scale the camera by obj+0x24) that is not
    recovered; a fixed backdrop invents no rate. PCM layers keep their
    engine strip wrap.
  - G-7 (real RE finding): every .MOV declares its state list before the
    first [STATE] (777 of 777) and the FIRST declared name is the start
    state. In 68 scripts the first written block is a terminal [A_DEAD]
    (KADMAR.07's balloons, box40, camel ...), so the port started them
    dead: bursts and blanks where intact objects belong. ll_mov_t now
    carries first_decl/start_st; objects start there. Before/after:
    KADMAR.07 shows balloons instead of burst sparkles; the stray
    explosion rings in the top-left corners of RAN.01/ZARIA.01 are gone
    (those were (0,0) pool objects drawn in their dead state). --mov-
    selftest pins the declared start.
  - G-8 is not a bug: LONA.05's PLAN3_.PCX is itself 2080 px wide.
  - Left as authored: objects at (0,0) are real placements (PILARYB,
    POCISK, TGA_OBJ, KROPLAS, DEMOTEXT ...); the 'ALPHA VERSION' label
    (DEMOTEXT, anchor -86,-91, f4/f6 identical) is drawn at (86,91) from
    its object position, exactly as the data says.

## PR-52 — documentation pass, the `A` glyph, an independent MOD parse oracle
          (rest of VERIFICATION_2026-11-16.md, DONE 2026-11-16)

  - README: the 18 undocumented LEO_* hooks the audit found (LEO_DATA,
    LEO_HD_DIR - required by its own --play example -, LEO_USER_DIR,
    LEO_SAVE, LEO_NO_MUSIC, LEO_DEMO*, LEO_LEVELSEL_AUTO, LEO_MENU_SECS,
    LEO_MENU_BG, LEO_HELP_SECS/PPM, LEO_LEVEL_CAM/NO_OBJ, LEO_DUMP_SKIP,
    LEO_IS_*) are now listed; every hook the code reads is documented
    (checked by grep). verify list corrected (65 PCX, 23 PCM, the 7 targets
    that were missing, py-refs), the `make ll_level` build step, a
    Documents map. ported/README.md rewritten (it claimed the Makefile
    builds original/).
  - Historical documents carry banners (TEST_REPORT, IMPROVEMENTS,
    PORT_DEEP_DIVE, IMPLEMENTATION_PLAN); stale statements fixed in place:
    NEXT_STEPS header/PR-21 status, "golden capture half open" (landed in
    PR-37/PR-40), the P-05 pipewire note (removed in PR-35), the unpinned
    "RSS pinned 1114", the PR-45 record's "inline palette+sprite viewer
    gap" (now carries an inline RETRACTED pointer to PR-46), PLAN's
    `src/` and 172-payload counts (reading note).
  - Anchor wording corrected everywhere (PLAN 8h, NEXT_STEPS PR-46,
    ll_seq.h): both shifts are skipped when f0 is the 0x7fff sentinel (the
    game gates on f0 alone). The shape code and its python twin now do
    exactly that (identical on all shipped data: 14126 both real, 180 both
    sentinel, none mixed - verify-seq still 1656/1656). The "LOWER adjusted
    y" window wording now says lower ON SCREEN = numerically larger y.
  - G-9 was a real font bug, not a missing glyph: the built-in 8x8 `A`
    was a diamond, so "(BACK)" drew as "(B<>CK)" on the restart screen
    and in every list menu. Fixed; no pin moved (30 golden pose hashes and
    the 92.4594% best match are unchanged).
  - New oracle tools/verify_mod_parse.py, written from the public
    ProTracker layout (not from ll_mod.c): title, signature, channels,
    song length, pattern-slot count, and every sample's length / present
    bytes / loop / volume / finetune for all 22 SOUND tracks, compared with
    mod_selftest's parse summary: 22/22, mutation-checked (a wrong pattern
    count fails 0/22). Wired into verify-mod. Honest limit: the MIXER still
    has no independent oracle (verify-mod's render hashes prove linux ==
    wine, i.e. determinism).
  - Still open from the audit: the PCM layer format (PR-50), second anchor
    pair f4/f6 (identical to f0/f2 on the data), SEQ_REV_X/Y and the other
    unmodeled control= flags, frame 0's placeholder, the real camera and
    backdrop parallax (PR-51 viewer policies), a mixer oracle.

## PR-53 — the PCM layer format, decoded: transparent tile-opcode overlays
          (G-4 of the audit, DONE 2026-11-16)

  - What was wrong: every earlier PCM reading (PR-20s through PR-52) decoded
    the stream as byte-RLE pixels and got noise, because the loader they
    read, FUN_00056b20, is the PCX layer class. The PCM class is the other
    one the strip manager FUN_00057250 can create (vtable 0xa277c): loader
    FUN_00056d40, blitter FUN_00056ef0, tile interpreter FUN_0001d58c.
  - Found by (a) reading FUN_00044ad0 (a PCX-header parser: the class I had
    mistaken), (b) the two vtables of the strip manager, (c) the machine
    code of FUN_00056d40: header u16 H, u16 N strips, u16 T tile rows (the
    old reading had H and N swapped), N x u32 strip offsets, per strip
    (ceil(H/T)-1) x u32 tile offsets, then the raw data; (d) the 14
    handlers of FUN_0001d58c: a per-dword opcode stream with masks, skips,
    raw runs and 2-pixel fills, an opcode >= 0x11 ending the row.
  - Proof without ground truth: on all 23 shipped layers every row covers
    exactly 20 dwords and every tile ends exactly where the next one
    starts (1.2 M rows). Layer sizes equal the levels' own worlds (KADMAR2
    2560x1800 = its back_rect, LONA.05 2480x600, TUNGA.11 2560x600).
    The pictures are recognisable: KADMAR2 a pyramid interior with a
    pharaoh and hieroglyph blocks, LONA.05 ice mountains with igloos.
  - Port: ported/ll_pcm.{h,c} rewritten (ll_pcm_decode with a transparency
    mask, ll_pcm_blit with horizontal wrap, ll_pcm_fnv; memory-safe on any
    input: fuzzed under ASan); ll_level composites every present tlo .PCM
    over the .PCX backdrop in tlo order (layers_at), a PCM-only level
    (KADMAR2) uses a 640x400 window and the block's BACK.PCX palette, and
    the any-.PCX fallback now applies only without PCM. Scroll mapping is a
    viewer rule where a layer is smaller than the world (the engine's
    per-layer factors, FUN_00057460, are not recovered): 1:1 when equal.
  - Oracle: tools/verify_pcm.py rewritten from the machine code (not from
    ll_pcm.c) - 23/23 layers C == python on geometry, painted count and
    hash, plus the two intrinsic assertions; mutation-checked (one opcode
    mapping broken -> 22/23 mismatch). verify-level pins the RAN.01
    overlays, KADMAR2 and LONA.05 and cross-checks each level's printed
    layer hash against the python oracle; verify-win compares the KADMAR2
    frame linux == wine byte for byte.
  - fuzz.c: runs every corpus blob unmutated first (the strict decoder
    rejects almost every mutant of a multi-MB layer, tripping the "corpus
    NEVER parsed" guard on a short run).
  - Before/after (all 13 levels, vis/all13.png): backdrops that were flat
    colour or a noise strip now carry their scenery - RAN.01 spires and
    coral over PLAN3, ENA.03 foreground seaweed and wreck, ENA2/TUNGA.11/
    TUNGA2.12/KADMAR.07/KARA.07/ZARIA vegetation and architecture, LONA.05
    and KADMAR2 complete.
  - Retracted claims: PLAN 8c "PCM DECODE + DISPLAY SOLVED 2026-11-14",
    "RAN.01: plan3.pcx/plan2.pcm are CD holes", the win_rows window model,
    ll_pcm_view/ll_pcm_view_at (removed).
  - Still open: the engine's per-layer parallax factors (FUN_00057460 +
    the tables at layer+0x20/+0x24), PCM-only window size (640x400 is a
    viewer choice), the second layer class's blitter modes.

## PR-54 — the chapter map, read from the data and checked against the
          original (DONE 2026-11-16)

  - Ground truth: three screenshots of the ORIGINAL game supplied by the
    owner (start, first island selected, game finished; kept OUTSIDE git in
    /tmp/opencode/golden/user_refs/, game frames are never committed).
  - Findings from the data (ANIMACJE/MENU):
    * LEVELnn.SEQ = one level each: elements 0 = the level's ARROW, 1 = the
      arrow's second state (LEVEL01's carries the red "start" label), 2 =
      the dimmed grey arrow, 3 = the cursive TITLE. Anchors are ABSOLUTE
      screen positions in 320x200 space (LEVEL01 title at (23,52) = where
      "ZARIA I" stands in the screenshot, its arrow at (79,17)).
    * The titles (decoded from element 3) in file order are 01 ZARIA I, 02
      ZARIA II, 03 ENA I, 04 ENA II, 05 LONA, 06 SADES, 07 KADMAR I, 08
      KADMAR II, 09 RAN, 10 KARA, 11 TUNGA I: this is the GAME ORDER; level
      12 (TUNGA II) has no title image. The 2026-11-15 "owner-given" table
      in main.c was wrong for 7 of the 11 files (03, 05, 06, 08, 09, 10,
      11).
    * Arrow colours are in the data: levels 1-4 red (palette 243/154),
      5-8 green (23/24), 9-11 blue (121/118) - exactly the finished-game
      screenshot (blue RAN/KARA/TUNGA I top and right, green LONA/SADES/
      KADMAR bottom, red ZARIA/ENA around island 1).
    * The thick red straight arrow that moves between the screenshots is
      the MOUSE POINTER (MOUSE.SEQ); BERLO.SEQ's 30 frames are the yellow
      mace (keyboard cursor, owner-directed, kept). MASK.PCM is a 320x200
      dissolve dither mask (the transition), not art.
    * No level name is hard-coded in the executable: the first level is
      data/progress driven. The screenshot puts "start" on ZARIA I, so NEW
      GAME starts ZARIA.01 (the old RAN.01 default was a guess; RAN is the
      9th level).
  - Port: chapters are cycled in game order; titles map by the data; the
    selected chapter draws its arrow (element 0) and title (element 3)
    instead of all four stacked; NEW GAME / restart / the demo chain start
    ZARIA.01 (LEO_DEMO_LEVEL still overrides).
  - Independent result: the golden comparison against the original's
    captured screen rose from 92.46% to 99.57% pixel-exact (best pose);
    the floor is now 99, the 30-pose manifest re-measured. The port frame
    and the owner's second screenshot agree on title and arrow position.
  - Not done (needs the game's progress rules): which arrows are VISIBLE
    (one at game start, all after finishing - STATUS.LEO progress), the
    "start" variant (element 1) before a level is touched, the grey element
    2, the red mouse pointer, arrow animation. Recorded for the next step.

## PR-55 — movie image quality: LUT1 polarity and palette-only redraws
          (owner report with screenshots, DONE 2026-11-16)

  - Report: the intro movie looked "really bad" (owner screenshots of the
    port vs the original: the crocodile's belly stripes were thick black
    blocks, dark checkerboards, no eye detail) and a black screen appeared
    at some moment. Screenshots kept outside git
    (/tmp/opencode/golden/user_refs/movies/).
  - Bug 1 - LUT1 colour swap (every .IS movie): the two-colour opcode
    (FUN_0001a9fe) loads `al = TABLE[bit]`, so a SET bit selects the SECOND
    table byte. tools/is_decode.py and ported/is_stream.c (twins, written
    together) did `bit ? T[0] : T[1]`, swapping both colours of every
    two-colour run: thin outlines and stripes turned into solid blocks
    while broad fills (SOLID/RAW/LUT2/LUT4, all checked against the
    machine code and correct) looked right. Found by decoding the exact
    frame of the owner's screenshot (592, by pose/background match),
    tracking the last-writing opcode per pixel, then re-reading the
    handler's instructions. The old "verified byte-exact" claims compared
    C with python - both wrong the same way.
  - Bug 2 - the black screen: chunks 551.. of INTRO_P carry ONLY a palette
    (PAL) and audio, no frame: the movie fades the title card out and a
    jungle-temple scene IN by stepping the palette for ~28 s (t = 40..68 s,
    brightness 1 -> 75 over 80+ chunks; a fade-out closes the movie). The
    player presented only on frames, so the black palette stayed on screen
    for the whole fade. It now redraws on a palette-only chunk (162 such
    chunks in INTRO_P: presents = 1521 frames + 162 = 1683, counted).
  - Not a bug: the first ~490 frames are the title cards (red text on black,
    walking characters) - dark by design; 0.2-1.8 s black gaps separate the
    cards; single black frames elsewhere are cuts (0.07 s).
  - Tests that cannot be fooled by the twins agreeing: new data-free
    verify-is-synth (tools/is_synth_check.py) builds a 640x4 movie and
    checks the python decoder, the C decoder (--ppm) and the player's
    presents (1 frame + 2 palette-only chunks = 3) against values derived
    by hand from the machine code; mutation-tested three ways (each fix
    reverted -> FAILED). The existing byte-exact legs (verify, verify-skip,
    verify-win, verify-cmake) now compare the corrected decoders.
  - Open: the live player also ignores the RPT (repeat) operand for hold
    times (it paces by the audio clock, as documented); the vertical wipe
    seen mid-movie was left as authored.

## PR-56 — Mode X movies: EOB advances the plane cursors (DONE 2026-11-16)

  - Sweep of all 15 .IS: no desync, no unknown opcodes; palette-only chunks
    INTRO_P 162, KONIEC 20, LOGO 19; the four 640x480 movies were clean.
  - Bug: in the 320x200 movies (ENA1/2, KADMAR1/2, KARA, KONIEC, LONA, RAN,
    SADES, ZARIA1/2) the EOB handler (0x1ad43) adds len * band-stride
    ([a8958] = width) to all four write cursors. The decoders only advanced
    the band counter, so everything after a mid-frame EOB (hundreds per
    movie: ENA2 779, KARA 648) landed len bands too high -> scattered
    fragments. Fixed in tools/is_decode.py and ported/is_stream.c.
  - Test: tools/is_synth_check.py gained a synthesised 320x200 movie with a
    mid-frame EOB (hand-derived expectation); mutation-tested (C fix reverted
    -> FAILED).
  - Caveat: RPT operands still do not set hold times (audio-clock paced).

## PR-57 — PCM layer parallax read from the engine (DONE 2026-11-16)

  - FUN_00057460/57500/57530: the level .des `speed=x,y;...` (one percent
    pair per tlo layer, default 100*(i+1)/n) and `main_back=` (reference
    layer R) give x_i = cx*X[i]/X[R], y_i = cy*Y[i]/Y[R] + (H_i-200)
    - (H_R-200)*Y[i]/100. Replaces the viewer rule (proportional to the
    layer/world size). tlo indices count absent (CD-hole) layers too.
  - Not changed: the .PCX backdrop stays fixed (PR-51 viewer policy; the
    engine would scroll it at its own speed, unverified against captures).
  - Prints `ll_level: parallax: ...` per level. verify-all green (data and
    fixture-free). No new hooks.

## PR-58 — the push-out is modeled (FUN_00042de0 + FUN_00042cf0)
          (DONE 2026-11-16; first step of the collision line)

  - What blocked it (PR-46): the callers' operands. Resolved now from the
    caller FUN_00053030 (0x5309x/0x5313x) and the object vtable slots, read
    out of the LE image: the common object vtable has slot +0x48 =
    FUN_00023aa0 (the current frame's shape element), +0x50 = FUN_00053e40
    (facing -1/+1 from the element's mirror flag), and FUN_00052d30 /
    FUN_00052dc0 give the screen-space position (object position minus
    the camera offset of its layer, 0x57500/0x57530). So the call is
    push(elemA, posA, elemB, facA, posB, facB, &dx, &dy).
  - The helper's "w" is A's right edge (ax + tw) and its EBX is B's x
    (branch 1), or B's right edge and A's x (branch 2, outputs and result
    negated). The earlier note's "clamp against |position.x + total_w|" was
    wrong: the clamp value is the VERTICAL overlap candidate
    v = (by+bh-ay < ay+ah-by) ? -(by+bh-ay) : ay+ah-by, and it replaces
    the push (0, v) when it is smaller than both |dx| and |dy|.
  - FUN_00053ae0 (the caller of 53030) packs dx/dy into the {0,1,dx,dy}
    message it sends to B; its own use of the return value limits the
    sliding to +-4 (0x53b4c..0x53b8e). Not ported yet.
  - Proof: ll_seq_shape_push (ported/ll_seq.c) + python twin
    (tools/shape_geom.py push): verify-seq now diffs a `shapepush` line for
    every shape element x 17 offsets (14306 elements, byte-exact), and
    --mov-selftest has a hand-derived 7-case leg (rectangle, slope with
    first/last interpolation, reverse branch, mirrored facing);
    mutation-tested twice (branch flip, interpolation sign).
  - Next in this line: the caller FUN_00053ae0 / slot +0x10 message, which
    objects collide with which (the 13 control= flags: SOLID, SOLID2,
    SEMI_SOLID, ANTI_SOLID ...), then gravity/ground for the lion.

## PR-59 — solid contact: the game's flag word and FUN_00053ae0 (EXPERIMENTAL)
          (DONE 2026-11-16)

  - control= flag bits read from the token parser (0x51b30..0x51dc3, flag
    word at state+0x12): SOLID 0x400, SOLID2 0x20400, SEMI_SOLID 0x40400,
    ANTI_SOLID 0x10000, ANTI_POCISK 0x80000, DANGER 0x2000, CHECK_LEW 0x200,
    CHECK_LN 0x100, CHECK_ALL 0x1c0, CHECK_BASE_LN 0x4000, CH_DIR 0x800,
    INV_V 0x1000, CHECK_ALL_PLANE 0x8000. ll_play now models SOLID, SOLID2,
    SEMI_SOLID, ANTI_SOLID, ANTI_POCISK (they leave ctrl_other).
  - FUN_00053ae0 (A = the solid, B = what is pushed; the response goes to
    B's message slot +0x10 as {kind 1, a, b}): A needs SOLID, B must not be
    ANTI_SOLID; SEMI_SOLID takes a separate path (kind-1 r/5 plus a kind-13
    message, unresolved); SOLID2 sends the full (dx, dy); plain SOLID sends
    (r, 0) only. FUN_00052b10 = enqueue a deferred message node.
  - ll_level play mode gets play_collide (LEO_PLAY_SOLID=1, OFF by default):
    solid objects of the scene push the lion; a push up is standing
    (ll_player.support keeps CHECK_LN from dropping the lion). Measured on
    RAN.01: the invisible EDGE0046 solid stops the walk at x~135 (tick 54
    push -3,0). SEMI_SOLID contacts are ignored and announced once.
  - WHY OFF: which object pairs the game tests is not resolved - the per-
    class collision loops behind FUN_00053c90 (callers 0x2c941, 0x2f8d1,
    0x2f9f1, 0x38246) and the CHECK_LEW/CHECK_ALL gating decide it. With it
    on, RAN.01's verify-play fall pin (walk to x=314 and fall) changes.
  - Pinned by the new verify-play leg (contact line, default run clean).
  - Next: the plane gating below, the kind-13 message, then switch the pass
    on by default and fix the pins.
  - PR-59 follow-up RE (2026-11-16, no code change): WHO collides with whom.
    FUN_0004b500(level, out, mover, list, flag) is the touch dispatcher: for
    every non-null object of the level's object list `list` it calls the
    object's slot +0x60 (is-active) and requires (obj.plane == mover.plane
    [+0x24]) OR `flag`, obj != mover, then calls obj slot +0x78 (touch) with
    the mover. Wrappers FUN_0004b6a0 / 0004b870 / 0004b890 pass the list ids
    held in 0xa94e8 (=1), 0xcef44 (=0) and 0xa94ec (=2) with flag 0 or the
    caller's. Slot +0x78 of the 62 common classes is FUN_000552e0 = {if
    53ae0(this, mover) and this.slot+0x9c(mover) then 55230(this, mover)}
    (55230 = the hit effects: DANGER/ANTI_POCISK events through slot +0xd0);
    10 classes use FUN_00053c90, ~20 have their own. So the response is only
    reached for objects in the SAME plane slot as the mover (the plany=
    letters, e.g. TALAB: T tlo, A animacje, L Lew, A, B belka) unless the
    caller sets the all-planes flag. The experiment's RAN.01 EDGE0046 stop is
    therefore probably NOT what the game does (EDGE objects are in an A
    plane, the lion in the L plane). Next: parse plany= and map placements to
    plane slots, then gate play_collide on it and switch it on.


## PR-60 — which list a mover collides with (experimental contact pass, gated)
          (DONE 2026-11-16)

  - FUN_00052780 (0x527b3..0x5282f): the MOVER's own state flags select the
    object lists it is tested against: CHECK_LEW (0x200) -> Lew (FUN_0004bd30),
    CHECK_ANI (0x40) -> the animacje= list (FUN_0004b890), CHECK_ZW (0x80)
    -> the zwierzaki= list (FUN_0004b870); CHECK_ALL_PLANE (0x8000) is passed
    as the dispatcher's all-planes flag. CHECK_ALL = ANI|ZW|LN (0x1c0).
    List ids come from the level loader (0x4cd1d..0x4ceb5): each of the
    keys zwierzaki=/animacje=/ani_plat= creates the next list ([level+0x40]++)
    and stores its id in 0xcef44 / 0xa94ec / 0xa94e8; entry i of the key's
    comma list becomes the placement file loaded into that list (0x4dd50 ->
    0x4aaa0). The ani_plat list is reached by another path (not ported).
  - Ported: lvl_obj.src (the key that placed it); ll_play bits CHECK_ANI,
    CHECK_ZW, CHECK_LEW (CHECK_ALL = its own bit + ANI + ZW; the LN physics
    is unchanged); play_collide tests a placement only when the lion's
    CURRENT state carries the matching flag. Census line per level
    ("placements with a SOLID state SOLID/SOLID2/SEMI by key"), measured:
    RAN.01 animacje 432/0/0 (the EDGE solids), SADES.06 zwierzaki 19/0/0 +
    animacje 4/64/0, KADMAR.07 animacje 7 + zwierzaki 6 semi, KARA.07 11 +
    10 semi, KADMAR2 5 + 1 semi, TUNGA.11 4 + 2 semi, ENA.03 animacje 4
    SOLID2, ZARIA.01/.02 only SEMI_SOLID (2 and 4), LONA/ENA2 none.
  - STILL OPEN: the plane gate (obj.plane == mover.plane [+0x24]): where
    [+0x24] is assigned (probably the entry index i of the key's list, the
    lion's = main_back layer) was not confirmed, so the pass stays opt-in
    (LEO_PLAY_SOLID=1); RAN.01 still shows the EDGE0046 contact because the
    lion's walk state carries CHECK_ANI. SEMI_SOLID (the platforms of ZARIA,
    KADMAR, KARA, TUNGA) needs the unresolved kind-13 message, so most of
    the platform-like behaviour is still missing.

## PR-61 — objects scroll with THEIR layer; the plane gate resolved
          (DONE 2026-11-16)

  - The plane [obj+0x24] is the PARALLAX LAYER index, not a plany= slot:
    FUN_00052d30/52dc0 hand it to FUN_00057500/57530 (the per-layer scroll of
    PR-57) to turn the object's authored position into its screen position
    (screen x = ox - cam*X[i]/X[R], y = oy - (cam_y*Y[i]/Y[R] + (H_i-200) -
    (H_R-200)*Y[i]/100)). An object's layer = the ENTRY INDEX of the .des
    list that placed it (the loader hands entry i to cell i of the key's
    list, 0x4cdf5..0x4ceb5 / 0x4aaa0): RAN.01 zwierzaki=chmury,ptaki,none ->
    the clouds sit on layer 0 (40%/80%), animacje=none,wulkany,dymy -> the
    volcanoes on layer 1 (70%) and the smoke puffs on layer 2 (100%, with
    PLAN1, the reference). Before this PR every object scrolled 1:1 with the
    camera, which is wrong for every object off the reference layer.
  - ll_level: lvl_obj.layer, obj_layer_cam/objs_draw (all three draw paths);
    hand-derived data-free leg obj_layer_selftest (part of --mov-selftest).
    Checked by eye on RAN.01: clouds drift slowly, the smoke puffs stay on
    the foreground chimneys at cam 0 and cam 1500.
  - Plane gate of the contact pass: obj.layer == the lion's layer (main_back)
    unless the lion's state has CHECK_ALL_PLANE. RAN.01's EDGE0046 is on the
    lion's layer and the lion's walk state has CHECK_ANI, so the contact stays
    - consistent with every gate now read from the code. The pass is still
    opt-in (LEO_PLAY_SOLID=1): it is unverified against the original, and
    the SEMI_SOLID platforms (kind-13 message) are still missing.

## PR-62 — SEMI_SOLID: what the kind-13 message is (DONE 2026-11-16)

  - Message nodes are {next, kind, a, b}. The common slot +0x10 handler is
    FUN_00054560: kind 2 sets the mover's velocity (+0xc2, +0xc6) = (a, b),
    kind 13 ADDS (a, b) to it, kind 8 zeroes it, kind 3/4 set flag bits,
    kind 6 sets the push vector (+0x6d, +0x71); everything else goes to
    FUN_00053770 (kind 1 among them). The velocity is integrated at
    0x5427c as dt * v / 100 per axis with friction (+0x6d etc.), so the unit
    scale is not pinned.
  - So SEMI_SOLID (FUN_00053ae0 0x53b47..0x53ba9) = a soft contact: only the
    horizontal penetration r matters; |r| > 5 moves the mover by r/5
    (kind 1) and a velocity impulse of 30*clamp(r, -4, 4) is added (kind
    13). ZARIA.01's SEMI objects are the LICHA creatures: touching one
    pushes the lion away (LICHA/licha3.mov at 120,650: the lion is pushed
    10..18 px per contact at ret 50..90).
  - Ported: the kind-1 part only (play_collide); the velocity impulse is
    announced and counted, not applied (its unit scale is unresolved). The
    pass is still opt-in (LEO_PLAY_SOLID=1).

## PR-63 — hit effects: what the code says, and the one number it does not (DONE 2026-11-16, RE only + a data fact)

  - FUN_00055230 (the effect of a touch, called by the common touch handler
    552e0 after the push-out): A = the object touched, B = the mover.
    A with ANTI_POCISK (0x80000) calls B's slot +0xd0 (the hit method) with
    (source A, 0, 0); if A.slot+0x40() it goes through FUN_00055130, else A
    with DANGER (0x2000) calls B.hit(A, 0, [level+0x54]) - the damage is the
    LEVEL's danger_power. The .des key danger_power (scanned at 0x4cf2e into
    level+0x54) is authored in NONE of the 12 levels (checked in the CD
    LEVELDES and the HD ETAPY/ANIMACJE; ll_level now prints it per level), so
    the real value is whatever the level object holds un-initialised (its
    ctor 0x4a5d4 never writes +0x54) - not recoverable from the files.
  - The hit method of the common class (38 vtables, 0x41180): ignore while
    the invulnerability timer [+0xee] is set, else energy [+0xad] -= damage
    and [+0xee] = 75. Lew's own (11 vtables, FUN_00050ed0): ignored in
    states [+0x11a] 6 and 7, with damage 0, or while [+0x126] is set; only
    when the lion is flagged friendly-fire-safe it needs the source to be an
    enemy (slot +0x40); it then records the source's slot 0xc / 0x70 results,
    drains its pending-message list, and unless the hurt lock [+0x102] is
    active enters the hurt state (slot 0xe4(1)) and subtracts the damage
    (FUN_00054550: [+0xad] -= damage). Energy starts at 100 (0x503ac: [+0xad]
    = 0x64 on respawn), lives are the global 0xa94f0 (decremented at
    0x50406 on respawn; STATUS.LEO word 63).
  - So the mechanism is known but the damage number is not: nothing was
    ported (a number would be a guess). Open: the allocator's initial value
    (5c320) or a default set by a path not yet found; the invulnerability
    constants (75 ticks, hurt lock 30).

## PR-64 — the .PCX backdrop scrolls (DONE 2026-11-16)

  - PR-51 had fixed the backdrop only because the engine's per-layer factor
    was unknown; PR-57 recovered it (speed= / main_back=, FUN_00057500).
    The backdrop is the first tlo layer, so x = cam * X[i] / X[R], wrapping
    modulo the image width like the PCM strips (RAN.01: 40% of a 28800 px
    world - a 960 px image necessarily wraps, so the engine tiles it too;
    the hard cut seen on KADMAR.07/KARA.07 is then authored). ll_level
    paint_base does it in all three frame paths; PCM-only levels are
    untouched.
  - NOT applied: the vertical factor. The engine's view is 200 rows (0xc8
    in 57460) while this viewer shows the image's full height, so the
    offset would only open blank rows; the backdrop keeps its y.
  - Proof: backdrop shift hand-derived in the data-free layer selftest
    (cw 10, speeds 50/100, cam 14 -> shift 7; PCM-only = no shift);
    RAN.01 checked by eye at cam 0 and 3000 (mountains shift 240 px, no
    seam). README/Makefile wording updated.

## PR-65 — world extent of the "back_rect hole" levels, and what a playthrough video shows
          (DONE 2026-11-16)

  - Reference material (kept OUTSIDE git, /tmp/opencode/yt/, never committed):
    a 7-minute playthrough of ZARIA.01 ("Let's play ... Lew Leon PL - Pierwsze
    Kroki", a YouTube video the owner suggested), frames at 6 s steps.
    What it shows: the intro chain (LERYX LONGSOFT / CD PROJEKT credits, the
    title, the story cutscenes), the island map with "start" on the first
    (jungle) island, then ZARIA.01 itself: the game view is 640x400 inside
    the 640x480 mode (black bars 40 px top and bottom), HUD = a lion head
    with a red number at the bottom-left (6 on screen) and an item box at the
    bottom-right; red balloons, banana bunches, rhino, a ladder that unrolls,
    sign boards, the vulture that grabs the lion and carries it over the
    treetops (the walkthrough's end of the level).
  - Data fact checked against it: levels without back_rect (SADES.06,
    TUNGA2.12, ZARIA.01/.02) took "one screen" as the world. The engine
    clamps the camera to the reference layer (main_back), so that layer's own
    art is the world: ZARIA.01/.02 2240x700, TUNGA2.12 2560x800, SADES.06
    960x3000 (a vertical shaft, as the walkthrough's "swim down" says).
    Measured proof: placements inside the world rose from 29 -> 53 of 53
    (ZARIA.01), 63/63 (ZARIA.02), 118/118 (TUNGA2.12), 405/419 (SADES.06).
  - Not done (evidence now available): the viewer window is the .PCX size
    (960x475 on ZARIA) while the game's view is 640x400 with the HUD; adopting
    it means cropping the layers to 640x400, adding the HUD (lives/energy
    number, item box) and re-pinning the frame legs.

## PR-66 — the level view is 320x200 world units, shown 2x (DONE 2026-11-16)

  - Evidence: the playthrough video (outside git) shows the art at ~2x the
    size of the native sprites inside a 640x400 picture (chunky, pixel-
    doubled leaves; the red fruits 2.5x ours), and the engine's constants say
    the same: the camera clamp (FUN_00057590) and the layer offsets use
    0x140 x 0xc8 = 320x200. So the scene canvas is the 320x200 window onto
    the world and the window shows it magnified 2x (640x400). Before this the
    viewer drew a window the size of the backdrop image (960x475 ...), i.e.
    unscaled and far too wide.
  - ll_level: LVL_VIEW_W/H; canvas 320x200 for every level (PCM-only levels
    too); paint2x for the window; the CI PPM dumps stay at the logical
    320x200. cam_max/camy follow use the new size; the engine's own
    formulas are now literal (200-row view): paint_base applies the full
    backdrop parallax, y included (clamped to the image) - the PR-64
    restriction is lifted.
  - Checked by eye against the video frame at 205 s: ZARIA.01's spawn now
    shows the same tree, backdrop and scale as the original. Pins updated
    (view 320x200; the "show" line keeps the image size, a new "view" line
    names the logical and the displayed size). Not done: the HUD (lion head +
    number bottom-left, item box bottom-right) and the exact camera follow
    (the video keeps the lion lower than 65% of the view).

## PR-67 — the HUD strip, read from the level's BELKA file (DONE 2026-11-16)

  - `belka=<name>.des` in each level's .des names a text file in the HD ETAPY
    block (BELKA.DES; RAN.01 BELKA1.DES) that lays out the HUD in the 320x200
    view. Grammar from all 12 shipped files: optional key=value lines
    (tools=off, crown=on), then items, each an `x,y` line + `[N ]name.seq`
    (the first carries N = 10). By position: energy head (ENERGIA.SEQ, ten
    frames: front-facing roaring at full energy, turning away as it drops),
    lives digits (CYFRYD/CYFRY_D/LITT ..., ten digit elements), the tools
    icons (TOOLS.SEQ, 4 icons), item-count digits, the item frame (IKOKO.SEQ),
    the crown. ZARIA.01: head (3,158), digits (40,173), box (285,165).
  - ll_level hud_load/hud_draw: drawn in play mode over everything, from the
    level's CD ETAPY records. Checked against the playthrough video: the head
    at the bottom-left, the red number next to it and the item box at the
    bottom-right land where the original has them.
  - Placeholders (named, not invented as data): full energy (frame 0), 6
    lives (what the video starts with - STATUS.LEO's fields are unmapped),
    the empty item frame (no inventory yet: the video shows the TOOLS icon
    of the held item, the cyan curved blade first), no crown. Pinned by the
    "hud: belka.des: 5 item(s)" line of the TUNGA.11 scene leg.

## PR-68 — enemy stats are in the placement files; contact damage (opt-in) (DONE 2026-11-16)

  - The key=value lines under an entry of the zwierzaki .des files were
    only counted (st->prop); they ARE the enemy definitions: `damage=`
    (contact damage), `bounds=x0,x1` (patrol range), `scope=` (detection
    range), `energy=` (hit points; some carry two values). Examples
    (ZARIA.01): RYBA damage 20, LICHA 15/15/10 with energy 20, PAJAK 10 /
    energy 50, SEP 10 / 40, NOSOROG 10 / 60 (scope 40,150), SEP2 10 / 80.
    lvl_obj now keeps damage/b0/b1/scope/energy (-1 = authored nowhere).
  - The class touch handler (FUN_0003d990 and its siblings, slot +0x78)
    runs the push-out and then calls the mover's hit method with the
    ENEMY's own damage field ([+0xb1] <- damage=); DANGER objects use the
    level's danger_power (unauthored, see PR-63).
  - ll_level (opt-in LEO_PLAY_SOLID=1): a contact that pushes the lion hurts
    him by the object's damage= (energy -= damage, hurt lock 30 ticks =
    0x50304's constant); energy <= 0 -> lives-- and respawn at the spawn;
    the HUD head/digits follow (energy frame (100-e)/10). Assumptions named:
    the lock is counted in 20 ms ticks, the lion's hurt state and knockback
    are not played, enemies do not patrol (bounds=) or detect (scope=).
    Pinned: ZARIA.01 tick 84, LICHA damage 15 -> energy 85.
  - Next: patrol/detect from bounds=/scope= (the enemy scripts are .MOV
    state machines driven by signals - the unported signal bus), the lion's
    attack (damage to enemies, energy=), pickups.

## PR-69 — scene objects are state machines (the first slice of the signal bus)
          (DONE 2026-11-16)

  - Survey of all 777 HD .MOV scripts: frame-line tails are 595 `GEN <sig>`
    (K_GEN1 295, AC_PLAT_OFF 181, AC_TIME 60, K_GEN3 ~140, K_GEN4, K_GEN2,
    AC_XCHG_LEVEL, AC_LEAVE, AC_SHOW_CROWN, AC_MSG_PUSH ...), ATC_ON/OFF,
    WAIT, F/M/I/FC (velocity/move/impulse), lock/unlock, GOTO_REL, RDIR,
    sound (smpauto), end_level, VOL_MUSIC. Transition conditions: AC_BREAK_NEXT
    674 (the frame list ended), K_GEN1 231, AC_ENEMY_ATC 290, AC_TIME 179,
    AC_ONAIR 139, K_GEN2 126, AC_DUP 76, keys (K_JUMP, K_FIRE, K_F1/F2 ...).
  - Old behaviour: a scene object walked its states as a RING (state end ->
    the next state in file order), which kills e.g. ZARIA's balloons after
    one cycle (A_RUN -> A_DEAD). Now (obj_advance): a state's frame list
    LOOPS until a transition fires; conditions decided here: AC_BREAK_NEXT,
    K_GEN1..4 (raised by a frame's `GEN K_GENn` tail, now parsed into
    ll_mov_fr.gen), AC_TIME (approximated as the end of the frame list: the
    `time=min,max` unit is unresolved, a waiting state must not freeze);
    AC_ENEMY_ATC, AC_ONAIR, K_LEW, keys etc. are false until the lion/world
    signals exist. First-match, OR semantics, self-targets keep the state:
    the player's rules.
  - Effects seen: ZARIA.01's LICHA now first hurts the lion at tick 163 (was
    84, pin moved); TUNGA.11's scene animation shows from ~tick 20 (was 8:
    looping states hold a frame longer than the old ring) - the level-anim
    leg compares tick 2 vs 20.
  - Proof: hand-derived data-free obj_state_selftest (plain script stays in
    A_RUN at 250 ms; `GEN K_GEN1` enters A_DEAD at 200 ms and AC_BREAK_NEXT
    returns at 300 ms), mutation-checked twice (ring wrap, GEN dropped).
  - Open: who raises K_GEN* from OUTSIDE (the balloon's A_RUN waits for a
    K_GEN1 that the touch handler sends - the class touch methods), the AC_*
    actions (PLAT_OFF/OUT, XCHG_LEVEL, SHOW_CROWN, MSG_PUSH, LEAVE),
    ATC_ON/OFF and the F/I/FC velocity directives.

## PR-70 — the balloon pickup, and how an EXTERNAL signal reaches an object (opt-in)
          (DONE 2026-11-16)

  - Factory 0x4df10: the first field of a placement line is a CLASS name;
    BAL builds the object with vtable 0xa17dc. Its touch method FUN_0003d370
    (slot +0x78): when the mover is Lew (slot +0x3c) and the balloon is in
    its first state ([+0x5c] == 1), the point (balloon.x + 10, balloon.y +
    10) is tested against Lew's bounding rect (slot +0x4c, inclusive); a hit
    sends Lew the kind-9 message {0, 9, balloon, 0}.
  - Lew's message dispatcher (0x50d60, jump table 0x50d20 for kinds 9..21):
    kind 9 (0x50df4) = if energy < 100: set a bit (byte 6 bit 6 = bit 54) in
    the SENDER's signal bitset ([obj+0x60]; this is how an outside event
    becomes the balloon's K_GEN1) and energy += 0x20 (32), clamped to 100;
    at full energy the message is ignored and the balloon stays. Kind 10
    sets [+0x14/+0x18], kind 11 -> slot 0xd8, kind 15 -> slot 0xfc, kind 21
    sets [+0x116] (the same flag the death code uses).
  - Ported in the opt-in contact pass (LEO_PLAY_SOLID=1): balloon_hit rect
    test (hand-derived data-free selftest: corners inclusive, one pixel off
    misses), energy +32, K_GEN1 raised and the transition evaluated at once
    (A_RUN -> A_DEAD, the pop). Gated on the placement's leaf "BAL", its
    start state, and the lion's layer. The ZARIA.01 balloons sit at
    (78,460) and above - not reachable by the scripted ground walk, so the
    effect chain is covered by the read code + the rect selftest, not by a
    run pin.
  - Open: the signal-bit numbering ([obj+0x60] bit n <-> K_GENn/AC_*: bit 54
    is assumed K_GEN1), the other pickups (GRABOBJ 0x3db90 handler, KORONA,
    SURPRISE), score/points, the pop sound.

## PR-71 — the contact pass is ON by default (not on RAN.01) (DONE 2026-11-16)

  - Every gate of the pass now comes from the game's code (list/flag/plane
    gates PR-60/61, solid response PR-58/59, SEMI_SOLID PR-62, enemy damage
    PR-68, balloon PR-70), so it is the default: LEO_PLAY_SOLID=0 turns it
    off, =1 forces it on. RAN.01 is excluded by default: it is the balloon-
    flight level (walkthrough: fly right, defend the canopy), its lion
    walks only because the port has no balloon control mode, and the
    invisible EDGE solids that stop that walk at x~135 cannot be judged.
  - Measured with the default (600 ticks of held FWD/BKWD on the 11 other
    levels): only ZARIA.01 FWD touches anything - the first LICHA at
    x~100 (49 contacts, 10 hits, one respawn: the lion cannot jump or fight
    yet, so it stays pushed against it); the rest of the walks are free.
  - The pins that need the pass set LEO_PLAY_SOLID=1 explicitly (RAN.01
    contact, ZARIA.01 LICHA hit); verify-all unchanged otherwise.
  - Next: the lion's moves (jump, attack: the K_JUMP/K_FIRE/K_F1.. keys and
    the lion's attack damage vs enemy energy=), the balloon control mode of
    RAN, the unported signal bus (pickups, platforms, AC_* actions).

## PR-72 — camera follow and walking speed measured on the playthrough video
          (DONE 2026-11-16)

  - Method (video kept outside git): the lion tracked by colour in all 5150
    frames of the 7-minute ZARIA.01 playthrough (view cropped to the 640x400
    picture, halved to 320x200 world units), plus a per-frame global shift of
    the background by phase correlation. The accumulated camera path drifts
    (several layers scroll at different rates), so only LOCAL facts were
    used.
  - Measured: the lion's x on the view clusters at ~95 (mode, 27-37% of the
    width) while he walks right and ~200-230 while he faces left; his feet
    sit at y ~175 of 200 whenever he stands (mode of 4829 tracked frames),
    other values only during climbs/jumps/the vulture ride; in windows where
    the lion is locked on screen the camera shift per video frame is 3..5
    units (median 3 = 75 units/s at 25 fps) - consistent with the lion's
    T=90 cap, which supports the units of the .MOV's F/T fields as px/s.
  - Ported (viewer policy built on those facts): camera x leads in the facing
    direction (lion at 100 facing right, 220 facing left, eased at 6 units
    per 20 ms tick - the ease rate is a viewer choice), camera y follows the
    FLOOR (feet at 175 while grounded, held while airborne). Replaces the
    65%-down / centred follow.
  - NOT obtained: jump units. The jump impulse (`I 250,-1100` in A_PRE_JUMP
    with G=300,55) cannot be read as px/s (the apex would be thousands of
    px); the tracked jumps are mixed with camera scrolling, so no clean
    trajectory came out. A pixel-accurate camera model (or a captured jump
    with a static camera) is still needed - until then no jump is ported.

## PR-73 — first jump measurement from the video (5:16-5:19; notes only)
          (2026-11-16, owner pointed at the segment)

  - Segment (episode 1, 318.0-319.1 s, 25 fps, per-frame lion shorts colour
    (158,0,0) + the lowest yellow shoe pixel): the lion stands on a ledge
    (318.0-318.56), crouches (A_PRE_JUMP, two frames), jumps in place and
    grabs a vine at the apex.
  - Camera: it is NOT static for the whole segment - while the lion stands
    still the camera keeps easing: his screen x drifts 215 -> 145 and his
    feet y 146 -> 128 world units in ~0.55 s (decelerating), i.e. a standing
    lion converges to ~x 145 (roughly the view centre), not to the walking
    lead of 95/215 (PR-72). The easing is a visible rate to copy later.
  - Jump (feet y, world units, takeoff at 318.60): 127.5, 119.5, 116.5,
    114.5, 109.5 (apex, 318.76), 110.5, 114.0, 116.5, 119.0 (318.92). Net of
    the camera drift (~0.5 u/frame) the rise is ~21 u in 0.16 s, the fall as
    long: air time ~0.35 s. A parabola through that: v0 ~ 262 u/s upwards,
    g ~ 1640 u/s^2.
  - Relations that FIT (not proven): the impulse in A_PRE_JUMP's last frame
    `I 250,-1100` divided by 4 gives vy -275 u/s (measured ~262); the state's
    `G=300,55` second value x 30 = 1650 u/s^2 (measured ~1640). Two numeric
    coincidences with different factors - treat as hypotheses until a second
    jump (episodes 2 and 9 are on the playlist) confirms them.
  - Not ported: still no jump in the player sim.

## PR-73 — episode 2 segments (0:53-0:55, 0:56-0:58, 7:44-7:47): a correction
          (DONE 2026-11-16, owner supplied the timestamps)

  - What they show: ZARIA II (episode 2, 480p): a drop from a branch, a
    ledge climb with a jump (54.2-54.9 s), a stone-to-stone run past the
    crocodile (464.8-466.6 s). All three scroll while the lion jumps, so no
    clean static-camera jump; per-frame tracking (shorts colour + phase-
    correlated camera, tools in /tmp/opencode/yt) is too noisy at 480p to
    confirm the I/4 and G*30 hypotheses from them.
  - They DO refute one PR-72 rule: the camera y is not held while airborne
    (during the 54.2-54.5 s ledge jump the view rose ~45 units with the
    lion), and the "feet at y ~175" mode of the first video is the world's
    bottom clamp (ZARIA's ground is the last rows), not a rule: a lion
    resting mid-level sat with his feet at ~128 of 200 (64% - the original
    65% rule). follow_camy goes back to 65% (clamped to the world).
    Kept from PR-72: the horizontal lead (100 facing right / 220 facing
    left; a resting lion drifts toward ~145).
  - Jump impulses in LEW.MOV (ZARIA): A_JUMP_S `I 120,-1200`, A_JUMP `I 150,
    -1200`, A_PRE_JUMP `I 250,-1100`; with the two hypotheses (impulse/4,
    gravity = G.y*30 = 1650) a standing jump rises 27 units in 0.18 s and
    stays up 0.36 s. Untested on a second clean jump.

  - Addendum (owner's 7:13-7:16 of episode 2, i.e. 433-436 s): the balloon
    pickup is confirmed - the red balloon bursts into a ring of red sparks
    (ZARIA's BAL A_DEAD plays `dym.seq`, PR-70), the lion then jumps up to
    the crown ledge. It is not a plain jump in place: the lion rises ~65
    units while the camera follows him vertically at up to ~6 units per video
    frame (150 units/s) and scrolls back after the landing, i.e. the camera y
    tracks the lion's y quickly (PR-73 correction stays 65%-follow). No clean
    jump height could be read from it.

## PR-74 — the lion can jump (ESTIMATED units) (DONE 2026-11-16)

  - Parsed: a frame tail `I ix,iy` (impulse) into ll_mov_fr; the K_JUMP
    condition; ll_play_keys.k_jump (Space in the viewer, `JUMP`/`FWDJUMP` in
    LEO_PLAY_SCRIPT).
  - Model: entering a frame with an impulse sets vx = ix/4 * facing and vy =
    iy/4 (world units/s), leaves the ground and falls at 1650 u/s^2 until the
    .plt (or a solid) catches him; landing needs vy >= 0 (before this the
    landing search re-grounded him on the first tick of a jump); a landing
    ends the jump state and goes to the idle state (A_STAY, else A_DRAP) -
    the authored `WAIT AC_DUP` hold is not decoded (DUP is unidentified; the
    jump frame list is ~2 s long against a 0.36 s flight). A bug found by
    the selftest: the frame advance of the same tick used the OLD state after
    the landing switch and threw the lion a second time.
  - Numbers: ZARIA I's A_JUMP_S (`I 120,-1200`) gives v0 -300 u/s, apex 24.3
    units, 17-18 ticks (0.36 s) in the air - the vine jump measured on video
    (PR-73: ~21 units, 0.35 s). The units (/4 and g = G.y*30) come from ONE
    measured jump; the first jump of a run prints a log line saying so.
  - Not right yet (named): the running jump is far too short (+5 units in
    the air): the state's T=20 caps vx and the mid-state `F x,y` / `FC x,y`
    directives that probably change the acceleration/limit are not parsed;
    airborne conditions (AC_ONAIR/AC_DUP/AC_DRECT) stay false; the other
    moves (attack, K_FIRE/K_F1.., K_ALT, ladders) are not ported.
  - Proof: data-free selftest in ll_play (hand-derived Euler numbers: apex
    24.3, 16-20 airborne ticks, return to the idle state), mutation-checked
    (impulse divisor 2 fails it). ZARIA.01 run: the jump arc matches.

## PR-75 — handoff documentation, RE helper tools, Makefile header deps (DONE 2026-11-16)

  - HANDOFF.md written (state, rules, machine layout, architecture, facts with
    sources, every estimate in the code, open questions, ordered tasks T1-T11
    with acceptance criteria, method notes, PR checklist); README got a status
    section and its stale "NEW GAME = RAN.01" line is fixed; PLAN.md and this
    file point to HANDOFF.md.
  - tools/re/: the throw-away RE and video helpers used since PR-58
    (container tree/cat/mov-extract, .SEQ contact sheet, disassembly recipe,
    lion/camera tracking on the reference video) with a README; not part of
    verify-all, paths hard-coded to /tmp/opencode.
  - Makefile: ll_level / ll_level.exe now depend on ll_play.h, llrs.h and
    ll_pcx.h (an edit of ll_play.h used to leave a stale binary - seen when a
    mutation test appeared not to fail).
  - Open from the F/FC investigation (not ported): `F x,y` and `FC x,y` frame
    tails stay unparsed; the usage statistics are in HANDOFF section 7.

## PR-76 — music played 2.47x too fast: 32-bit overflow in the MOD tick length (DONE 2026-11-16)

  - Found by the owner listening to a live run ("music too fast"). The tick
    length `((uint32_t)110250u << 16) / tempo` overflows 32 bits
    (110250 * 65536 = 7.2e9): at tempo 125 it gave 357.7 frames per tick
    instead of 882, i.e. every song ran 2.47x fast. The comment above it
    even claimed "125 -> 882". The old render hashes agreed between linux and
    wine because both ran the same wrong code - the same lesson as PR-55.
  - Fix: 64-bit intermediate. Proof: `mod_selftest --synth` (data-free, wired
    into verify-mod): an empty 4-channel MOD must complete 25 ticks in 0.5 s
    at tempo 125 and 50 at tempo 250; hand-derived, mutation-checked (old
    code gives 62 / 124 ticks).
  - Still open from the same report: animations and objects look 5-10x too
    fast. The port's clock is correct (wall 5.22 s for a 5 s run on the
    owner's PipeWire device), so the suspect is the frame duration unit:
    LEW walk frames are authored `6` ms x 12 frames and played as 6 ms. Needs
    the real unit from the machine code (the `time_scale=` state field and the
    frame-time consumer) or a DOSBox timing of a walk cycle.

## PR-78 — ground-bound scene objects settle onto the .PLT (DONE 2026-11-16)

  - Owner report from a live run: the worms (LICHA) floated ~30 units above
    the lion's ground. Cause: ZARIA.01 places them at y=650 over a floor at
    680, and the port drew every placement exactly where it was authored. The
    lion's own authored spawn (pozycja 50,640) is also "in the air" and falls
    onto the 680 line - the same rule applies to actors whose start state has
    `CHECK_LN` (licha3.mov A_RUN: BY_X|CHECK_LN, G=0,100).
  - Change: `obj_settle` drops such a placement onto the first ground line at
    or below it (never up); play mode only; logs the count (ZARIA.01: 10).
    The G= fall itself is instant, not simulated. Data-free test in the
    object state selftest (settle 150 -> 180; non-CHECK_LN stays; an object
    below every line stays), mutation-checked. Pin moved: LICHA contact is
    now tick 138 (was 163: it hits earlier because it now stands at the
    lion's height).
  - NOT changed, still open: worm motion (BY_X F/T needs the patrol AI) and
    the frame-duration unit (worms animate at 4 ms frames; the lion's 6 ms
    walk frames use the same unit and looked fine to the owner, so do not
    change the unit without a DOSBox timing).
    [RESOLVED in PR-79: the BY_X unit is movement units per frame, the
    speed is F/4 px/s, measured on the video.]

## PR-77 investigation — F/FC directive parser and accumulator (IN PROGRESS 2026-11-16; no gameplay change)

  - Executable evidence (`build/leo/leo.elf`, VA = file offset + 0xf000):
    strings `F`=0x9196f and `FC`=0x91974; their parser branches are in
    0x468dc..0x46a14. Both create a 17-byte directive record and parse an
    integer pair with the format at 0x91968 (`%i,%i`). The record's handler
    vtable is 0xa1090; its slot +0x10 is FUN_00045d50.
  - FUN_00045d50 (0x45d50): if the record's selector byte equals the caller's
    mode byte, add record x/y (offsets +8/+0xc) to two caller-provided integer
    accumulators. FUN_00045900 (0x45900) zeros both accumulators before
    dispatching the handler. This establishes additive vector accumulation,
    NOT yet what the vectors mean physically.
  - Parser selector values from the immediate writes: `F`=0, `FC`=1, `I`=2,
    `IC`=3. The `F` and `FC` labels therefore identify separate selector
    streams even though they share the same pair parser and accumulation
    method. Do not treat `FC` as a typo for `F`.
  - Distinguish state-header `F=x,y` from frame-tail `F x,y`. The header
    parser at 0x52590 matches `F` at 0x92281 and writes the pair to state
    offsets +0x50/+0x54; `T=` at 0x9228a writes +0x58/+0x5c; `G=` at 0x92293
    writes +0x48/+0x4c. FUN_00052680 copies these as three pairs into callback
    context offsets 0, 8 and 0x10, with the frame-handler output pair separate.
    This identifies the state fields, but still does not identify the units or
    how the output is integrated into world position/velocity.
  - FUN_00052680's code at 0x526ec..0x52700 passes modes/pointers into that
    handler system: the initial call uses mode 0 and output locations at the
    caller context +0x18/+0x1c; conditional calls later use modes 3 and 2.
    These mode values and fields are not yet tied to world position/velocity.
  - More dataflow from FUN_00052680 (0x52680): before invoking the frame
    callbacks, it copies three stored pairs from the motion object offsets
    (+0x50,+0x54), (+0x58,+0x5c), (+0x48,+0x4c) into context offsets
    (0,+4), (8,+0xc), (0x10,+0x14); callback outputs use another pair at
    (+0x18,+0x1c). FUN_000524a0 initializes all six stored values to zero and
    FUN_00052500 copies them. This establishes four vector-pair slots in the
    callback context, but not their physical units or which pair drives x/y.
  - Next: trace the movement-state vtable/caller around 0x52680 and the
    consumer of those context fields. Only then implement F/FC. No code change
    has been made; T1 remains in progress and jump units remain estimates.

## PR-79 — BY_X scene objects WALK: units measured on the gameplay video (DONE 2026-11-16)

  - Video geometry pinned first (this invalidated an earlier 2.0x/2.4x
    reading - that test searched the wrong region): the 640x480 gameplay
    video is a UNIFORM 2.0x of a 320x240 game screen. Proof: the LICHA
    worm template scores 0.644 at 2.0x2.0 vs 0.593 at 2.0x2.4 (and 0.612
    at 1.9x1.9); the on-screen lion is ~125 px tall = its 64 px art x
    2.0; the f269 "unmatchable" lion is an IDLE el0 frame, right-facing,
    occluded by foreground plants (confirmed by VL look at a labelled
    candidate sheet). The video darkens rows ~x0.81 (scanline); matching
    through it works; de-interlacing changes nothing.
  - MEASURED units (ZARIA.01 = video frames 5102-9649):
      * A BY_X frame list's 2nd field is MOVEMENT UNITS per animation
        frame, not ms: the worm (authored 4) crawls 3.0-3.75 game px per
        displayed frame; the rhino (authored 7) moves 7-10 px between
        walk elements 5 and 6. BY_TIME frames keep the ms unit.
      * Speed = F/4 game px/s: the worm (F=100,T=40) and the
        camera-following lion A_WALK (F=100,T=20) both move 25 px/s
        (camera band-shifts: median 2 px/video-frame = 25 px/s); the
        NOSOROG rhino (F=180,T=40) tracks ~40-50 px/s = the 1.8x F
        ratio. Equal F + different T -> same speed, so T is NOT speed;
        T=(t1,t2) semantics stay unresolved and nothing was built on it.
      * bounds= IS the patrol range: measured LICHA crawl 118->128 inside
        its authored bounds=100,180; direction flips at the edges. All
        levels' lion A_WALK is F=100 (RAN.01 400, KARA 150 = per-level
        design), confirming F is the authored speed.
  - Code: obj_walk() + the BY_X branch in obj_advance (ll_level.c).
    v = |F|/4 px/s, position carried in fixed steps with a fractional
    double carry (xa), frame advance when facc >= the frame's unit count,
    bounds flip. The frame's 2nd slot (ll_mov_fr.ms) is read as units
    only in BY_X states; the BY_TIME path is untouched. obj_transition
    and the state-loop wrap keep the position when either end is BY_X
    (walking is instance state, not per-state authored deltas).
    Estimates (labelled in code): initial direction +1 (no .des key
    names it; the video shows both directions for one script), and
    BY_X with F=0 keeps the old time-driven frame advance without
    moving (no measured F=0 walk state on video; a frozen list could
    deadlock). Ground slope-follow (ll_plt_surface_at) deliberately NOT
    applied to walkers yet - the measured worms stay on one line.
  - Tests: obj_motion_selftest (data-free, wired into the .MOV chain):
    F=100 walks exactly 10 px in 400 ms = F/4 px/s (mutations: F-as-px/s
    -> 4x too far; time-driven -> never moves x and raises t);
    unit-driven frames (fi=1 after 5 px on 4-unit frames; mutation:
    never advancing); a state-loop wrap keeps the position (110, not
    the 100 anchor; mutation: always-snap -> 0); bounds flip: 50x40 ms
    steps inside 10..26 land at x=14 dir=-1 (mutation: no flip ->
    x=70); GEN into a BY_TIME state and AC_BREAK_NEXT back keeps the
    walked position (x=107; mutation: always-snap transitions -> 102).
  - Pins moved: verify-play LICHA contact tick 138 -> 199 (the worm
    patrols instead of sitting at x=120; the lion meets it head-on).
    All other verify-play legs pass unchanged, including the RAN.01
    EDGE0046 solid pin at tick 54 (that object is not a walker).
  - Known tension, NOT touched here: ll_play.c reads the player's F/T
    as "walk at +/-T cap, F=accel", contradicting these object
    measurements (the lion's real A_WALK F=100,T=20 moves at 25 px/s
    = F/4). Re-resolve player physics in T1 with the video as referee.
  - Tools left in /tmp/opencode/yt: tms2.py (scale-sweep template match),
    findseq.py (whole-segment .SEQ template tracker), plus w205.png,
    lionmask.png, lionid.png. The .MOV inventory of all 105 BY_X states
    (F/T/frame-unit matrix) supports the F model; see /tmp/opencode/mov.

### PR-79 verification re-check (same day, direct vision; PR-80, docs+labels only)

  - CONFIRMED by eye: the 2.0x geometry (worm-sheet + 1-game-px grid:
    on-screen worm 38-40 game px = MOVE.SEQ el6 38 px x2.0; 2.4x
    visibly too long); the sprite reconstructions (strip_MOVE.png = the
    green worm with blue eyes, strip_WALK.png = the lion cycle); the
    vertical "rearing S-coil" poses in the video ARE the MOVE.SEQ art
    (el0 18x27, el1 24x25) - an animation pose at fixed feet, so no
    y-motion model is needed for walkers.
  - DATA-INTERNAL (stronger than any video claim): LICHA3.MOV's A_RUN
    is `control=BY_X` with every frame authored `N,4` while the same
    file's BY_TIME states author `N,400`/`N,200` in the same slot -
    the unit-vs-ms slot conflict is proven by the shipped data.
  - CORROBORATED: walk-burst speeds ~20-30 px/s vs F=100 -> F/4 = 25
    px/s; pause-in-place (A_ATAK/BY_TIME-style, and gated runs) between
    walk bursts = the state machine, which the port already models
    (walk only happens in BY_X states); observed range consistent with
    authored bounds=100,180 (80 game px; the worm leaving the crop
    window matches crossing its lower edge).
  - WITHDRAWN from the record above (keep the text, distrust these
    numbers): "worm 3.0-3.75 px per displayed frame" and "rhino 7-10
    px between elements" (measured pose deformation, not locomotion);
    "camera band-shifts 2 px/video-frame" as the speed method (the
    y350-470 band mixes parallax layers and the worm's own animation;
    whole-band and tree-template re-attempts stayed contradictory);
    "lion at 25 px/s via camera follow" as an independent anchor. The
    rhino 1.8x ratio note leans on the same suspect method.
  - NEW CENSUS (lead for T3): across the sampled placement .des files,
    bounds= appears ONLY on placements whose script has a CH_DIR state
    (8/8 with, 0/14 without); across 777 extracted .MOV scripts / 2317
    states: CH_DIR 556 states, BY_X 105 states, 93 BY_X states also
    CH_DIR. CH_DIR is likely the direction-flip flag that pairs with
    bounds=; resolve before wiring the flip to the flag itself.
  - Methods that failed (do not retry naively): color-centroid blob
    tracking on the ground band (foreground plants share the worm's
    greens; 12-22k px plant blobs), blue-eye-color masks (the scene
    has ~30-50 blue-cyan background blobs), band/region cross-
    correlation for camera shift (parallax + animated scenery).
  - No behavior change in PR-80: obj_walk/obj_advance/obj_transition
    and obj_motion_selftest are unchanged; only the measurement-basis
    wording in ll_level.c, HANDOFF.md and the Makefile pin comment is
    relabelled. The verify-play tick-199 pin is a model pin, not a
    video measurement.

## PR-81 — CH_DIR: walkers TURN TO FACE their direction (DONE 2026-11-16)

  - Question: the PR-79 walk never changed which way the sprite faced.
    Answer from the shipped data + video: yes, they rotate.
  - Authored base facing (data): ZARIA.01's worm art is MOVE.SEQ: the
    eye pixels sit 11-20% from the LEFT edge in all 9 poses (el0..el8),
    so the unmirrored art is head-LEFT; the walk list is authored all-
    NEGATIVE (-1,4 .. -9,4 - the sign is the mirror flag, PR-49), so
    the game draws that worm head-RIGHT. 379 of the 1282 BY_X frame
    entries game-wide are negative; e.g. KADMAR.07_0_0 A_WALK mixes
    both signs, so the sign cannot be a per-frame animation trick.
  - MEASURED polarity on video (the method that worked: cv2
    matchTemplate of every MOVE.SEQ pose >=30 px wide, x2 scale,
    masked TM_CCOEFF_NORMED, template colors x0.81 for the scanline
    darkening, both polarities of every pose as separate templates,
    ground band y350..445 only). A score >=0.55 window is a worm;
    WHICH polarity wins is the facing. ZARIA.01 t=203.5-216.5:

      | window      | winning polarity | on-screen x   | travel |
      |-------------|----------------|---------------|--------|
      | 204.4-204.9 | head-RIGHT     | 216 -> 254    | right  |
      | 206.9-207.6 | head-LEFT      | 498 -> 472    | left   |
      | 208.0-208.9 | head-LEFT      | 440 -> 384    | left   |
      | 209.2-209.6 | head-LEFT      | 378 -> 346    | left   |
      | 210.0-210.8 | head-RIGHT     | 332 -> 278    | right  |
      | 211.2-212.0 | head-RIGHT     | 244 -> 314    | right  |
      | 212.4-212.8 | head-RIGHT     | 336 -> 364    | right  |
      | 213.2-214.0 | head-LEFT      | 360 -> 298    | left   |
      | 214.4-214.5 | head-LEFT      | 260 -> 230    | left   |

    Polarity tally 44 head-LEFT frames / 38 head-RIGHT. Every segment
    faces its on-screen travel direction, and the polarity flips at the
    same place within one second (x~330-350 at 209.2 vs 210.0): one
    instance turned around. The on-screen dx is camera-contaminated,
    the polarity flip is not.
  - CENSUS (extends the PR-80 lead, /tmp/opencode/chdir_census.txt):
    bounds= appears ONLY on placements whose script has a CH_DIR state:
    8 of 8 sampled placements with CH_DIR carry bounds=, 0 of 14
    without (sampled .des in /tmp/opencode/mus/zdes). Over the 777
    extracted .MOV scripts / 2317 states: CH_DIR 556, BY_X 105, 93
    states carry both. So bounds= pairs with CH_DIR, not with BY_X.
  - Model: `face` is instance state on lvl_obj (+1 at placement = "draw
    exactly what the frame line asks"), obj_walk sets it ONLY at the
    bounds flip and ONLY when that state carries LL_CTRL_CH_DIR;
    obj_mirror_of(o, authored) XORs it onto the authored frame sign,
    used by obj_draw AND obj_shape_elem (a shape belongs to the sprite
    as drawn - the SOLID push at play_collide now uses the same
    polarity as the picture; the game's own polarity source for that
    call is not decompile-pinned, so this is labelled as consistency,
    not as a RE fact). face survives state changes (obj_transition
    note). Non-walkers and non-CH_DIR states are untouched.
  - The initial direction +1 (right) is no longer a bare estimate: the
    authored sign (all-negative walk list over head-left art) draws
    head-right, which is what the worm does at its start, and the
    video agrees.
  - Tests: obj_motion_selftest test 2 now also asserts face is
    UNTOUCHED when the state has no CH_DIR; new test 5 parses
    `control=BY_X|CH_DIR` (the separator handling is ll_mov_parse's),
    walks 20..26->10 in 40 ticks and requires x=24 face=-1, then 20
    more ticks: x=16 face=+1, and pins obj_mirror_of both ways
    (facing right keeps the authored mirror, facing left adds one).
    Mutations caught: M1 facing updated regardless of CH_DIR (fails
    test 2), M2 facing never turned (fails test 5), M3 obj_mirror_of
    ignores face (fails the mirror check). Baseline green.
  - Pins: NO verify-play pin moved. The ZARIA.01 LICHA contact at tick
    199 is unchanged - the worm is on its first outbound leg there,
    where face is still +1, and the hit test shares the same push call
    so the polarity is identical either way. verify-all EXIT=0.
  - Still open in this area (NOT modelled): the pause-in-place -> walk
    burst gating (A_ATAK/DANGER vs A_RUN), and whether CH_DIR also
    means anything beyond the facing flip (its name = "change
    direction"; the bounds flip is the only direction change the data
    shows for these placements).
  - Tools/artifacts in /tmp/opencode/yt: the polarity matcher (inline,
    re-run as needed), head_dir.png, head_dir2.png, worm_heads.png
    (both hits were the LION's head - the eye-color blob method is
    dead, the template method is the one that works), worm_sheet.png,
    gp.mp4.

## PR-82 — facing follows movement for EVERY mover, and for the lion (DONE 2026-11-16)

  - Owner request: PR-81's turning must cover all moving enemies and the
    player too, not just bounds-patrollers.
  - CENSUS for that (all 777 extracted .MOV scripts, /tmp/opencode/mov):
    CH_DIR is a per-STATE facing flag, not a walk flag: 556 CH_DIR
    states vs 105 BY_X, only 93 overlap - most CH_DIR states are
    BY_TIME/BY_Y. For the LION every LEW state carries CH_DIR EXCEPT
    the direction-baked ones, whose direction lives in the frame sign
    instead: RAN LEW A_GO_TO_BALON_R is authored `1,1...`, A_GO_TO_
    BALON_L the SAME list authored `-1,1...` with NO CH_DIR; also
    A_RZUT_W, A_JUMP_WLK, A_WALK_LEFT, KAMIEN A_GO carry no CH_DIR.
    A_WALK_FWD vs A_WALK_BKWD carry identical ALL-POSITIVE frame lists
    and both CH_DIR - the lion's art is one polarity (faces right) and
    walking left is done by the facing flip. All 29 states in the whole
    corpus that MIX frame polarities are turn/exit/death animations
    (A_TURN, A_OBROT, A_TURNBACK, A_GOBACK, A_WALKOUTIGLO, A_ESCAPE,
    A_WYJSCIE); SEQ_REV_X (a control name the port does not handle)
    appears ONLY on the 7 A_TURN states, whose lists go `-,-,+,+...` =
    the draw flips mid-sequence, i.e. a turn-around animation.
    M-delta check: only 5 CH_DIR states carry `M dx,dy` frame motion at
    all, and BY_TIME M motion is undone by the state-loop position
    reset (obj_advance), so authored M motion is NOT a facing source.
  - Model (implemented):
      * objects: obj_walk sets face from the TRAVEL DIRECTION on every
        step (not only at the bounds flip as in PR-81): identical
        result today (this walk is the only object movement source and
        dir only changes at a flip), correct the moment another source
        exists (the DANGER charge of T3, `I` lunges).
      * player: ll_player gains a `face` field (instance state, same
        role as lvl_obj.face). ll_player_step sets face=walkdir ONLY
        while the current state carries CH_DIR; direction-baked states
        never touch it, and facing sticks across states without the
        flag (the .MOV has no turn-back animation for standing still).
        ll_player_mirror_of(authored) = authored XOR face - the exact
        obj_mirror_of rule - used by play_draw AND the lion's shape in
        play_collide (PR-81's "the shape keeps the drawn polarity").
      * the player-side gate is inert in practice (face can only differ
        from walkdir if a key-transition entered a non-CH_DIR state,
        and every real K_FWD/K_BKWD target is a CH_DIR walk); it is
        kept so both sides run ONE model.
  - Video scan (new fact, negative): ZARIA.01 gameplay is 204-386 s of
    gp.mp4; montages every 5 s over 250-386 s (yt/scan2-*, scan3.png)
    show the lion RIGHT-facing in every sampled frame - this recording
    is a rightward walkthrough end to end, so it cannot settle the
    standing/left-facing question. The 115-160 s segment is a DIFFERENT
    demo, not Lew Leon - do not use it.
  - Tests:
      * --play-selftest: face +1 at walk-FWD entry, -1 at walk-BKWD
        entry (fixture flags are the real RAN LEW ones), STICKS at -1
        in A_STAY (no CH_DIR), ll_player_mirror_of both ways; new GL
        block reproduces A_GO_TO_BALON_L's shape (BY_X, no CH_DIR,
        frames authored -1..-3, entered via K_GEN1 after a left walk):
        walkdir set by the transition, face untouched, frame mirror 1
        from the authored sign, and the combined draw mirror 0 - that
        last value pins the CHOSEN always-XOR model, labelled as such
        in the test comment (the alternative "no-CH_DIR means the
        authored sign is absolute" is NOT excluded, see HANDOFF Q0).
      * verify-play NEW leg "play facing": script `1:BKWD 2:NONE`
        sampled at tick 2 - the lion taps BKWD one tick and returns to
        A_STAY, so both runs' logs show state A_STAY, seq uj09, frame
        1, pos 40,298, and the ONLY allowed canvas difference is the
        sprite redrawn flipped in place. tools/ppm_flip_check.py
        requires a real redraw (>=300 px; measured 1425 px over
        x=17..62 y=85..143) strictly inside the lion's area. It
        deliberately does NOT require a bitmap-perfect mirror: the
        game's element draw (ll_seq.c, PR-49/65) mirrors EACH PATCH and
        re-anchors it, and the flipped frame eyeballed by direct vision
        (yt + magick crops, /tmp/opencode/lion_cmp.png) is unambiguously
        the lion facing left with head/tail correctly swapped.
  - Mutations caught: A play_draw ignores face (caught by the
    ppm_flip leg: frames identical); B obj_walk never sets face (caught
    by --mov-selftest test 5); C ll_player_step never sets face (caught
    by --play-selftest "FAILED (line 910): p.face == -1"). LESSON:
    `make` default target builds ll_cutscene ONLY - after editing a
    source, rebuild the ll_level target (or run verify legs) before
    mutating checks, or a stale binary fakes a pass.
  - Pins: no existing pin moved (the FWD/NONE tick-40 legs run facing
    +1, the ZARIA.01 tick-199 contact leg is on a face=+1 outbound
    leg). verify-all EXIT=0, make/make win warning-free, both new
    legs green in-suite.

## PR-83 — the two --play bug reports: spawn state + sticky keys (DONE 2026-11-16)

Owner tested the HD build (ZARIA.01 --play) and reported (1) the lion
"spawns too low, crouched" and (2) input "sticks - he keeps pressing
in a loop continuously". Both root-caused, both fixed, both pinned.

- (2) ROOT CAUSE (backend, not gameplay): no backend ever translated
  SDL_KEYUP - platform_sdl.c mapped KEYDOWN/MOTION/BUTTON/QUIT but not
  releases; LL_EV_KEYUP existed in the contract, never produced. Every
  held key was forever-down: A_TURN self-transitions, endless walking.
  WHY CI MISSED IT: every scripted leg drives ll_play_keys directly,
  bypassing platform_poll. FIX: translate SDL_KEYUP -> LL_EV_KEYUP;
  extend the injector hook with LEO_TEST_KEY_UP_AFTER_MS=M (pushes the
  matching SDL_KEYUP M ms after the first poll; README documents it).
  Pinned: platform_selftest KEYUP pass-through + a NEW verify-play leg
  on RAN.01 through the REAL live loop (no LEO_LEVEL_PPM - the grid
  path bypasses platform_poll, that is the whole point): synthetic
  right-key down at 50 ms, up at 1300 ms, pins `A_STAY -> A_WALK_FWD`
  AND `A_WALK_FWD -> A_STAY` and counts exactly one WALK_FWD entry.
- (1) ROOT CAUSE (not geometry): spawn POS matches the game exactly
  (pozycja 50,640 -> feet 50,680 on both model and original); the CROUCH
  was the spawn STATE: block index 0 of ZARIA's LEW.MOV is A_JUMP_DOWN
  (crouch art). New rule in ll_player_start: prefer A_STAY where it
  exists, else the .MOV's DECLARED first name (ll_mov_parse.start_st -
  the header list, which block order can disagree with), else 0.
  EVIDENCE: all 13 player .MOV declare their level's start pose first
  (ENA A_SWIM - the lion spawns SWIMMING there; KADMAR2 A_MASZERUJ;
  JUNCTION A_IDLE; SADES A_STAND; ZARIA/TUNGA A_WALK). TUNGA.11's old
  A_GIVE_EGG-at-spawn pin was a block-0 artifact: A_GIVE_EGG is reached
  only via A_DRAP's K_FIRE,K_F6 line = an action state. RAN keeps
  A_STAY (its declared first, A_FLY_PTAKI1, is a glider) - the
  forced-standing-vs-declared question stays OPEN (HANDOFF 7.8).
- Player AC_TIME mechanism (needed for the idle half of the loop): the
  REAL source is frame tails `GEN AC_TIME` ("10,6,GEN AC_TIME" - 11
  such rows in ZARIA's A_WALK), raised as the frame completes, same
  rule as objects' o->gen |= f->gen. PR-83 first modeled the state
  time= timer as ms and the list wrap as an arm - BOTH WRONG on real
  bytes (time= as ms pulled ENA.03 out of A_SWIM at tick 2; wrap-arming
  has no data). time= stays parsed-and-counted, unit OPEN (32,64/90,120
  read as ranges); GEN K_GEN1..3 now reach the player bus the same way
  (fixture asserts the chain). Earlier "frame 2nd field = movement
  units for BY_X" also formally RETRACTED (always ms) - HANDOFF 7.0.
- BY_X key-gate: tv=+/-T only while the matching key is held OR
  p->auto_walk (auto_walk = entered/kept through a transition WITHOUT a
  K_FWD/K_BKWD condition - preserves the K_GEN1 cutscene walkers).
  ZARIA's A_WALK is the combined idle+walk (no !K_FWD exit exists):
  stopping = key-gate + the authored F=100 brake, then ITS OWN GEN
  AC_TIME drops to the A_DRAP hub - observed exactly on video-shaped
  data. apply_impulse: only iy<0 lifts (ZARIA A_TURN's I 100,0 is a
  horizontal push; forcing airborne re-created the flicker-loop).
- Pins MOVED (all justified at their Makefile comments): PR-68 LICH
  contact 199 -> 181 (walking starts at tick 1 in the correct spawn
  state; walk meets the authored x=120 at exactly T=20 px/s; pin
  history 163 -> 138 -> 199 -> 181); TUNGA.11 tick-30 A_GIVE_EGG ->
  A_WALK walking pos 60. NEW legs: ZARIA.01 zero-input (spawn A_WALK
  standing, GEN AC_TIME -> A_DRAP, x NEVER creeps - the earlier
  "mystery creep" was just the leg default "1:FWD"), and ZARIA.01
  press/release/BKWD (walks 39 px to tick 100, release -> A_DRAP,
  BKWD -> A_TURN ONCE, count==1, stays held). --play-selftest gained
  two data-free fixtures: ZARIA-shaped start/key-gate/impulse/wrap
  guards, and the GEN K_GEN1 -> auto-walker chain with time= inert.
  Also NEW selftest gotcha learned: LEO_PLAY_SCRIPT entries are
  SPACE-separated ("1:FWD 100:NONE"), semicolons silently match
  nothing, and the fixed grid advances ONLY to LEO_LEVEL_PPM_TICK -
  later transitions must be proven by the transition log lines.
- Mutation lessons: a fixture that reuses real transition ORDER caught
  the walkdir-mirrored impulse (selftest FAILED line: vx -23 vs +23);
  the PK fixture's first thresholds were wrong model math (F=100 =
  2 u/s per 20 ms tick) - pins state the arithmetic, not vibes.

## PR-84 — the grounded camera anchor made explicit: feet-at-178, view
          holds while airborne (DONE 2026-11-17)

  - Owner report: "still an issue with lion position" - our ZARIA.01
    frame vs the original's. MEASURED, not eyeballed: the original's
    640x480 frame is the 320x200 view at y-offset 40 (letterbox rows
    0..39/440..479); backgrounds align 1:1 (MSE 1211 on the halved
    frame); the original's grounded lion has his paw-yellow at screen
    y 133..180 (colour split of the t=205.0s frame of gp.mp4, the same
    scene - n=45 px of the paws class) -> feet anchor ~178-180 of 200.
    PR-72's 5150-frame feet mode 175 (its mask included the shadow
    ellipse, a couple of px lower) CONFIRMS the band; the two agree
    within 5.
  - HONEST FINDING on today's HEAD: ZARIA.01 at the spawn ground was
    ALREADY drawn at that anchor - the PR-65 reference-layer rule
    (PLAN1 2240x700 -> cam_ymax 500) + the old 65% rule over-clamp to
    the same camy 500 here (680-130=550 > 500), and 680-500 = 180 =
    the original. The owner's complaint matches the PRE-PR-65 build
    (max of plane layer heights: PLAN2 600, PLAN3 475 -> cam_ymax 400
    -> feet at ~199-200, the view bottom) - fixed in-repo by PR-65;
    their tested binary predates it. Also corrected: this session's
    "feet 199" of /tmp/zk.ppm was the mask bottom including the red
    ring/tail the A_TURN art paints ~20 px BELOW the feet point - the
    pose's art, not the camera. (The /tmp/zk.ppm the summary measured
    had already been regenerated by a probe at HEAD - the owner's
    original file was overwritten before measurement. Lesson: give the
    owner's artifacts protected paths, probe into new ones.)
  - So what did need fixing: the ANCHOR RULE itself. It only coincided
    with the truth where the bottom clamp dominated; anywhere else the
    65% rule sat 40+ px too high, and PR-73's single "feet ~128 of
    200" reading (taken during the camera's catch-up: its own notes
    show a ~0.5 u/frame standing drift) had invented that. Ported
    (follow_camy): grounded target camy = feet - CAM_FEET_Y(178) (the
    measured 175..180 band midpoint), eased at 2 u/tick (rate NOT
    measured, same honesty flag as the x ease); while AIRBORNE the
    view HOLDS - PR-73 measured the whole jump arc ON the view, an
    always-on anchor would cancel it. The contradiction is recorded in
    code + HANDOFF 7.6/7.7: PR-73's "the view rose ~45 u with a ledge
    jump" reads as a slow follow instead; the airborne rule stays an
    OPEN model choice, grounded is settled. Spawn snaps (camy_set on
    the first follow call). LEO_LEVEL_CAMY keeps seeding the PREVIEW
    camera only; play mode runs the game's own rule.
  - NEW tool + legs: tools/ppm_lion_measure.py (stdlib PPM colour-mask
    bbox with tolerance checks; the "orange" mask must EXCLUDE the
    scene's red props - g<70 reds like ZARIA's balloon and the HUD
    digit - a real mutation: the first mask caught the balloon,
    top=59 vs the lion's 137). verify-play gained 2 cam-y pins on
    ZARIA.01 (A_TURN-pose window and FWD-walk window built around the
    LOGGED position): drawn art top/bottom pinned +-6 (pose-dependent
    edges documented; the fixed window clipped the A_TURN mane -
    measure around the log, not a guess). No existing pin moved (all
    other play legs measure logs or cam-y-free windows; the
    scroll-camera legs are preview mode).
  - Method lesson: measure the ORIGINAL from its own pixels (the yt/
    materials: gp.mp4 + cs_*.png + e2a-e2d/f frame segments), not from
    memory of earlier notes - both earlier camera rules were eyeball
    level, and one full PR of back-and-forth (72 -> 73 -> 84) was a
    +/-50 px anchor argument.

## PR-85 — the left key now WALKS left: K_FWD/K_BKWD are facing-relative
          (Model M) (DONE 2026-11-17)

  - Owner report: "left key turns but he never walks left - slides more
    than it walks, and slow". ROOT CAUSE in the real data, not physics:
    ZARIA's LEW.ZAR authors exactly ONE walk state (A_WALK, entered only
    through K_FWD) and NO backward-walk state anywhere; A_TURN's exits
    are ONLY `A_DRAP(1)=AC_BREAK_NEXT / AC_BREAK_PREV`. Under
    literal-left/right key semantics its lion could not walk left AT
    ALL: BKWD leads only into A_TURN, whose F=0 slide just froze forever
    (the exact reported symptom). On video the player obviously walks
    left (PR-72 measured the facing-left screen band 200-230).
  - MODEL CHOICE (recorded in ll_play.c cond_true + ll_play.h grammar):
    K_FWD/K_BKWD are FACING-RELATIVE - K_FWD = the arrow along the
    CURRENT facing, K_BKWD the opposite arrow; walkdir stays the LITERAL
    arrow (physics/impulse mirroring unchanged), CH_DIR re-aligns the
    draw. K_UP/K_DOWN stay literal (ladders/vines don't mirror). RAN's
    A_WALK_FWD/A_WALK_BKWD pair reads consistently too (entered
    facing-relative; its cross-lines make the pair settle facing the
    movement - one-tick flicker is how RAN data turns, it authors no
    turn state). AC_BREAK_NEXT for the player = armed by the frame-list
    WRAP (p->broke, PR-83 mechanism); AC_BREAK_PREV unmodeled (false).
    STILL UNPROVEN on video (kept in HANDOFF 7.8): a facing-left lion
    with the RIGHT key pressed on a capture would CONFIRM the model.
  - Flow the user was missing (real ZARIA data, pinned by a new verify
    leg): LEFT @ face=+1 -> A_DRAP's A_TURN(1)=K_BKWD matches (the real
    A_TURN declares CH_DIR - verified in the .MOV) -> face flips, the
    F=0 slide carries him left -> at the 5x200ms list WRAP AC_BREAK_NEXT
    -> A_DRAP hub -> the STILL-HELD LEFT key is now K_FWD at face=-1 ->
    A_WALK walks LEFT at the authored T=20 cap. Measured on the fixed
    grid: A_TURN once (count==1), hand-off at 180/181, x 74 -> 64 by
    205 (~20 u/s). The selftest ZM fixture was rebuilt to this real
    shape (A_TURN with CH_DIR + AC_BREAK exits + 50/100/100 ms frames).
  - Fixture arithmetic LESSON (again): my first Model-M fixture
    expectations were model math done wrong (I predicted "RIGHT walks a
    left-facing lion LEFT" and walk distances ignoring the T cap) - the
    CODE was consistent from the first build; the FIXTURES got rewritten
    to the tick-by-tick model (one A_WALK_BKWD tick -> A_STAY -> settled
    A_WALK_FWD), and windows widened where a flicker tick applies the
    F=600 brake. Pins state the arithmetic, not the guess.
  - Pins: RAN state-machine leg re-pinned to the true real-data flow
    (its A_WALK_FWD -> A_WALK_BKWD and cross-lines are REAL LEW.MOV
    text - the flow only changes SHAPE: BKWD at 200 enters A_WALK_BKWD
    one tick, settles walking left, x 39x -> 33x proved); NEW zwalk leg
    (turn-once + hub hand-off + x decreasing + mirrored pose at the
    PR-84 cam anchor, top=128/bottom=199 - measured, not copied from
    the right-walk pose's 137). No other leg moved: RAN walk/stop,
    KEYUP live, LICH, PR-82 flip (single-tap BKWD at face=+1 behaves
    identically), multi-level - all press at face=+1. verify-play 12/12.

## PR-86 — jump: height verified faithful (negative result), `(N)` =
          target START ROW, authored AC_BREAK_NEXT ends jump states
          (DONE 2026-11-17)

  - Owner complaint "jumps are very low": measured honestly, NOT
    reproduced on the current binary - our model already matches:
    live probes on ZARIA.01 give apex ~24 u / air 0.36 s for BOTH
    A_JUMP (walk-jump) and A_JUMP_S (standing hop); the authored
    arithmetic (I 150,-1200 / LL_PLAY_JUMP_DIV=4 -> -300 u/s,
    g=1650) says ~27 u; and the full gp.mp4 feet-track scan
    (10367 frames) shows clean jump candidates 23-30 u over 8-13
    frames - all consistent. The game's jump IS a ~12%-of-screen hop
    (the PR-73 vine measurement was the same scale). Likely an older
    binary again (same pattern as the sitting-low report) - recorded
    as an open question in HANDOFF 7.8; PR-86 PINS the height rather
    than inflating constants.
  - GRAMMAR DECODED (the big one): the `(N)` in `TARGET(N)=COND` is
    the START ROW (1-based) in the target state's frame list - not a
    "priority" (stale comments gone; ll_play.h grammar updated).
    Evidence chain (recorded in the ll_mov_tr.entry comment):
    * A_PRE_JUMP's OWN impulse sits at its row 3, and it exits via
      `A_JUMP(4)` - A_JUMP's row 4 is its FIRST row after ITS impulse
      row 3: entering at (4) skips re-applying an impulse A_PRE_JUMP
      already gave. No double jump.
    * `A_JUMP(9)` from A_FIGHT_UP etc re-joins mid-air.
    * A_JUMP's `A_WALK(7)` re-joins ZARIA's 12-row walk cycle at row
      7 (mid-phase) - a seamless landing, not a restart at row 1.
    Non-numeric `(x)` (KADMAR2) keeps entry 0 = from the top (the
    A_MASZERUJ KEEP-semantics measurement stays valid). The parser
    stores the number; transitions set p->fi = entry-1 (clamped;
    overlong row -> top) and apply_impulse at THAT row (row without
    I = nothing; the walk-jump `(1)` entry behaves exactly as
    before: the impulse fires when the cursor reaches the I row).
  - LANDING RULE FIXED (data contradiction): the old viewer rule
    "landing ends the jump state -> A_STAY/A_DRAP" contradicts the
    data - no authored jump state exits to A_STAY; ZARIA/TUNGA
    A_JUMP ends with `A_WALK(7)=AC_BREAK_NEXT`, A_JUMP_S with
    `A_DRAP(1)=AC_BREAK_NEXT`. Now: a state that DECLARES an
    AC_BREAK_NEXT exit (helper st_has_break_next_exit) ends at its
    frame-list WRAP (PR-83's p->broke mechanism) - recovery rows
    (A_JUMP row 6 = the 900 ms crouch) play out INSIDE the state,
    grounded. The forced-idle stays ONLY as a fallback for states
    with no such line (the JM selftest fixture keeps covering that
    branch). p->jumping=0 still clears on landing (the flag only
    selects LL_PLAY_JUMP_G while airborne - verified safe).
  - Real-data flow pinned (new zjump verify leg, script
    `1:FWD 100:JUMP 130:NONE`): A_WALK -> A_JUMP at 100; apex ticks
    114-115 pos 90,656 grounded 0; landing ~125, state STILL A_JUMP
    grounded 1 at tick 135, `A_JUMP -> A_DRAP` count 0 (negated
    grep pin); wrap ~211 -> `play tick 212: state A_WALK seq walk
    frame 7` - the entry-row re-join (frame cursor 7, NOT 1; the
    tick log prints the row cursor: A_WALK's row LABELS run
    10,11,12,1..9, the cursor is the 7th row); drawn art at the
    PR-84 anchor (measured top=128 bottom=199, window from the
    logged pos). Chained hops while JUMP stays held also work (the
    re-entered A_WALK(1)=K_JUMP fires the next hop immediately).
  - selftest: JE fixture (ZARIA-shape): entry (1) at start, landing
    INSIDE A_JUMP (held window), wrap -> own exit to A_WALK(3) with
    fi>=2 (mid-row re-join); entry-parsed pins added to the ST
    parser test. Fixture arithmetic held at the 2nd build (one
    lesson kept: the entry tick CONSUMES its own first row - fi=1
    right after entry when the row is one tick long).
  - RAN has NO jump states at all (it's the balloon level - zero
    `I` impulses in its LEW.MOV), so no real-data RAN jump leg
    exists or can exist; TUNGA's player .MOV matches ZARIA's shape
    (its I values are -960 variants) - not separately legged.
    verify-play 13/13 (new zjump leg).

## PR-87 — fire-family keys (K_FIRE/K_F1..K_F4/K_ALT) + comma groups
          decoded as AND: A_FIGHT is reachable (DONE 2026-11-17)

  - What the data asked for: EVERY player .MOV gates its attack states
    with comma groups (`A_FIGHT(1)=K_FIRE,K_F1`, `A_FG_PACKA(1)=
    K_FIRE,K_F2`; census: K_FIRE 196, K_F1 81, K_F2 38, K_F3 11,
    K_F4 6, K_ALT 28 uses) - but ll_play_keys carried only the
    arrows+jump, so those lines could never fire (HANDOFF 7.8 item).
  - GRAMMAR DECODED: ',' inside a transition COND is AND, '|' stays
    OR: conditions parse into groups (ll_mov_tr.c gained grp ids;
    parse_cond splits bar segments at commas). MODEL CHOICE argued
    from data (ll_play.h): `A_WPAD(1)=AC_ONAIR,AC_DRECT` /
    `A_WSP(3)=AC_DRAB_CATCH,AC_ONAIR` pair a catch with being
    airborne (OR would be meaningless), and `K_FIRE,K_Fn` is "fire
    WHILE weapon n selected" (the game has Ctrl-attack + F1..F4
    weapon keys per the walkthrough). A ';' inside a group (3 lines)
    stays one OTHER token and poisons only ITS group. Old comma lines
    were one never-true OTHER token; nothing that matched before
    changes behavior (no existing leg presses fire keys).
  - Keys ported through ALL THREE input surfaces: ll_play_keys
    (k_fire, k_f1..k_f4, k_alt), script_keys tokens FIRE/F1..F4/ALT
    comma-combinable ("100:FIRE,F1" - the AND-group mirror), live
    keyboard Ctrl/F1..F4/Left-Alt (viewer mapping from the
    walkthrough; platform.h gained LLKEY_LCTRL/LALT/F1..F4, map_key
    maps both ctrls/both alts), LEO_TEST_KEY names ctrl/alt/f1..f4.
    K_ANY now includes the fire family.
  - Real-data proof (new zfight pair of legs): `50:FIRE,F1` on
    ZARIA.01 -> A_WALK -> A_DRAP (idle hub) -> A_FIGHT at 50, the
    punch frame rows play (300 ms rows: frame 5 by tick 100), drawn
    punch at the PR-84 anchor (measured top=128 bottom=199; the x
    spread wider right = the FC= rows aim the punch, FC accumulators
    themselves stay T1); NEGATIVE leg: `50:FIRE` alone -> A_FIGHT
    appears ZERO times (AND gate; impossible to pin under either old
    reading). selftest: parse pins (group ids, OTHER poisoning one
    group, leading-'|' empty group) + FK runtime fixture (FIRE alone
    stays idle, +F1 enters the fight).
  - Kept open (recorded HANDOFF 7.8): A_FIGHT's own ending mechanics
    (its wrap AC_BREAK_NEXT -> A_FIGHT_END works now; AC_ENEMY_ATC
    and the ATC_ON/ATC_OFF "attack connect" flags = T2 Lion-vs-LICHA
    work), K_F6..F12 stay OTHER (7 uses), F-keys modeled as HELD (a
    TOGGLED weapon-state reading behaves the same for the gates -
    unproven which the original does).
  - verify-play 15 lines (13 + zfight + fire-alone negative);
    selftest prints updated.

## PR-88 — the signal bus, part 1: the engine's signal-bit TABLE decoded
          (open question 2 CLOSED, PR-70's bit-54 assumption confirmed);
          named `GEN <signal>` tails + name conditions wired on both the
          lion and scene objects; obj conditions grouped AND like the
          player's (DONE 2026-11-17)

  - RE (the big one): the engine's signal names and bit numbers came
    straight off the code. The name block is 0x919ec..0x91c40 (NOT the
    0x92180 range open question 2 guessed - that one is the control=
    flags); the name->bit function is 0x48000 (not 0x51b30) and ORs an
    EXPLICIT constant per name into the set at [obj+0x60] (TEventSet,
    error string at 0x919cd): AC_BLOK 0 ... AC_BREAK_NEXT 2 ... AC_ENEMY
    7, AC_TIME 9, AC_ENEMY_ATC 16, AC_ONAIR 18 ... K_FWD 36, K_FIRE 39,
    K_JUMP 53, K_GEN1 54 ... K_GEN5 58, AC_PLAT_OFF 59 ... AC_EV_MAX 68
    -> 3-dword masks. VERIFIED constants in the disassembly (mov al,0x0
    / 0x1 / 0x2, mov dl,0x7 / 0x12, mov al,0x36 / 0x37); PR-70's
    "bit 54 = K_GEN1" assumption CONFIRMED, no behavior change from it.
  - Bonus grammar RE: the clause parser at 0x47d60 scans tokens with the
    format string ' %[^, \r\n\t]' (exactly our token grammar), keeps
    folding ','-separated names into ONE node bit-mask (AND), starts a
    new node for '|' (OR), tags '!' (negate) and '*' (and-with-set,
    unused in shipped texts), and special-cases K_ANY/K_NEVER as node
    ops, not bits. That is the engine's own design behind PR-87's
    OR-of-AND model choice - the reading is no longer a guess.
  - Ported: LL_SIG_* enum + ll_sig_id() (ll_play.h/ll_play.c); ll_mov_fr
    carries a 64-bit named-signal mask gsig (the K_GENn bit n-1 +
    AC_TIME bit 4 encodings are gone - same bits now at table indexes;
    names at index >= 64 never appear in a GEN tail in shipped data, a
    hypothetical one stays a counted unmodeled directive); the PLAYER
    keeps one signal word `p->sig` (was gen[3], same one-tick pulses at
    bits 54..56 - landing K_GEN2, frame GEN tails raise ANY name:
    `GEN AC_TIME` still arms ac_time, `GEN AC_BREAK_NEXT` now arms the
    break directly - the real TUNGA A_FIGHT rows do exactly that);
    obj_cond_true now tests any table NAME against the object's own
    pulse set, maps K_GEN1..5 to bits 54..58, keeps AC_TIME = frame-list
    end OR a raised bit, and evaluates PR-87's OR-of-AND-groups (the
    engine folds commas into one node mask class-wide, so objects AND
    too now - objects' keys/AC_* stay false pending the lion's set and
    the internal raisers). obj state transitions do NOT apply the (N)
    entry rows yet (the player's do since PR-86) - no legged data
    needs it; recorded.
  - Proof: obj_state_selftest gained a named-signal fixture pair -
    `GEN K_GEN1,GEN AC_PLAT_OFF` + `A_DEAD(1)=K_GEN1,AC_PLAT_OFF`
    enters A_DEAD at 200 ms, while the same script testing the
    UNRAISED name (AC_PLAT_OUT) stays in A_RUN (wiring is per-name and
    the AND really ANDs). Player-side real data: the ZARIA idle chain
    (GEN AC_TIME x11) now flows through the table path - all 14
    verify-play lines and all 7 verify-level lines unchanged and green
    (no shipped self-signal pair exists outside AC_TIME/K_GENn, so no
    behavior moved: honest null on real data, pinned fixtures cover the
    new semantics). The scene stats line now says precisely what is
    missing (cross-object routing, engine-internal raisers).
  - Still open (recorded in HANDOFF 7.2): WHO ROUTES signals across
    objects (ENA2's raft/barrels raise AC_PLAT_OUT on their own sets
    while the LION's A_JUMP_PLAT(1)=AC_PLAT_OUT tests the lion's - a
    message/broadcast connects them), WHO raises the engine-internal
    AC_* bits (AC_ENEMY_ATC 279, AC_ONAIR 211 condition uses, zero GEN
    lines - physics/combat must), and whether sets are ever CLEARED
    (ours: one-tick pulses; engine's OR: latching; no data pin yet
    discriminates - do NOT "fix" without evidence).

## T2 RE lead (gathered 2026-11-17, before any T2 code) — how the engine
          delivers damage (nothing ported yet from this)

  - `energy` is the level-object field at **[obj+0xad]** (the earlier
    "+0x54 danger_power" note is a DIFFERENT object: the LEVEL object at
    global 0xcef40, +0x54). `FUN_00054550` = `sub [eax+0xad], ebx` —
    "take damage ebx" — called from:
    (a) `0x50ed0(eax=hit obj, edx=edi)`: skips types `[obj+0x11a]` 6/7,
        requires ebx != 0; if `[obj+0x126] != 0` bails; method slot 0xc
        (x10) and 0x40/0x44 (x10) = rects of both; walks the touch list
        [obj+0xd2] calling slot 0x44 (x4) per entry (FUN_000551f0);
        then `FUN_000502f0` = "invulnerable?" tests `[obj+0x102] != 0`
        (a tick countdown: `0x50300` sets it to 30 when edx==0, else to
        edx); when not invulnerable: `call [class+0xe4](edx=edi)` (the
        per-class HOOK, e.g. 0x54250 / 0x55020 / 0x23a30 per vtable —
        unresearched what it returns/does) and THEN 0x54550 takes edi
        as the damage. Where edi comes from at each caller is NOT yet
        traced — the OPEN question for T2.
    (b) `0x30800`: after slot-0x80 interaction, subtracts `[ecx+4]`.
  - Hit entry on TOUCH (`FUN_000552e0`): push-out 0x55130/0x53ae0, gate
    slot 0x9c, then `FUN_00055230`: if the MOVER's flags ([x+0x44]) have
    ANTI_POCISK 0x80000 -> `hit_method edx=0` (no damage); else if
    flags & DANGER (test ah,0x20 of the dword = bit 0x2000) ->
    `hit_method edx=level.danger_power` = `[[0xcef40]+0x54]` (so the
    open question 4 default is this global's field, still uninitialized
    in the ctor); the hit method = **class vtable slot 0xd0** (generic
    actors 0x238b0, variant 0x23870/0x23880, bear-type class 0x544f0,
    some classes stub it to 0x52a00/0x52a50 = never damaged).
  - `FUN_0x3406b` (caller of 0x50ed0): for one enemy type (template
    globals 0xcecf0/0xcecf8/0xced04 = five factory instances made at
    0x33bd0 by 0x44350, identity NOT decoded yet) the "damage" passed is
    `[other+0xad]*45/100 + 10` (45% of its current energy + 10); for
    the 0xcecf8 type: `[obj+0xad] = -1` (indefinitely alive).
  - Next when T2 starts: trace what `edi` is at the 0x50ed0 call sites
    (the lion's attack damage source), identify the factory types, read
    one hit method (0x238b0 / 0x544f0) for the energy<=0 -> death-state
    transition, then port: attack hit test (the A_FIGHT art's own hit
    box), damage to [obj+0xad], death, invulnerability countdown +0x102.
  - SAME-DAY FOLLOW-UP (later the same night, still RE only):
    (1) THE damage source for normal touch combat: the touch hit path
        calls `FUN_00055130(eax=objA touched, edx=?, ebx=objB toucher)`:
        reads `EBP = [B+0xad]` (B's energy), calls 0x550c0 (a gate), and
        when it fails, `call [B.class+0xd0](eax=A, edx=B, ebx=[A+0xb1])`
        - so **[obj+0xb1] is the object's "damage" field** (the .des
        `damage=`, already parsed by ll_level) delivered through the hit
        method, and `[obj+0xad]` its energy. A's own energy is then
        compared at 0x5516a (`cmp ebp,[esi+0xad]; jle`) - energy bookkeeping.
    (2) A template object (one of the 5 factory instances) has a hit
        override at 0x3d0e0: `if ([obj+0x15a] != 0) call 0x50ed0` (that
        45%+10 energy-proportional attack); which template (lion? bear?)
        and what gates on [obj+0x15a] stays OPEN.
    (3) The complete .MOV frame-directive token table decoded
        (0x917c0..0x919c7, matched case-insensitively via the
        lowercasing strcmp at 0x76a69): atc_on/atc_off, lock/lock2/
        unlock/unlock2, ground_on/ground_off, chdir, ldir/rdir, pos,
        goto/gotorel/goto_rel, I, F, IC, FC, M, MOVE, GEN, Wait,
        end_level, smp/smp2/smp3/smpauto/smpauto2/smpprior/vol/
        smp_loop/smp_unloop/smp_stop, rev, zero_v/zero/zero_t,
        vol_music; unknown -> "Bledna komenda %s w pliku %s". The
        A_FIGHT attack = frames 8 `ATC_ON` then 9 `FC 100,0`: the
        attack-connect window is a FRAME FLAG (atc_on), not a signal -
        PR-89's hit test should gate on it (its consumer in the code
        not yet pinned; [obj+0xa5/0xa6] bit tests exist near 0x504e0/
        0x55230 but tying them to atc_on is unconfirmed).
    (4) ZARIA.01 authored combat numbers (zwierz1.des): LICHA x3
        damage=15/15/10 energy=20/20/"20,50" bounds= scope=20; PAJAK
        damage=10 energy=50; SEP energy=40/80; NOSOROG damage=10
        energy=60 scope=40,150; RYBA damage=20. (The energy "20,50" and
        "1000000,350" two-value forms = min,max - which one applies at
        spawn is OPEN.)

## Next — pick from the Deferred list below

PR-43 (M4-B playable prototype) opened the M4-C line, PR-44 closed its
first item and PR-45 its second (the authored scene), PR-46 its third
(the element SHAPE model, which also corrected PR-45's reading of the
169 "inline" tables); the game-wide measurement (see the PR-43
follow-up, PR-44, PR-45 and PR-46 records) says
exactly where the remaining gaps are:
- DONE in PR-44: ground from a SUB-.des (TUNGA2.12/SADES.06) and the
  authored-empty ground (JUNCTION). 12 of 13 level blocks now run
  their own lion; JUNCTION does not because its own data declares 0
  polylines (no floor) - that is the honest end of the level matrix,
  not a gap to close.
- DONE in PR-45: the authored scene - all 68 placement files parsed,
  2188 entries, real .MOV scripts per object, art per state via the
  script's own seq=. All 13 levels report their own authored scene
  (JUNCTION's is honestly 0).
- DONE in PR-46: the SHAPE model. The census item "draw the inline
  .SEQ form (169 tables)" turns out to be the MISREADING itself:
  those tables are w0=2 shape-only invisible solids - the original
  game never drew them either (every draw path needs a patch array).
  Nothing to draw; what the shapes actually feed is collision:
  ll_seq_shape_hit is ported and oracle-covered, and the real item
  is USING it - player-vs-object blocking against authored objects
  (the 169 solid tables incl. RAN.01's 48 EDGE*/DYMY solids, and
  the art+shape tables), with the push-out (FUN_00042de0/42cf0)
  still unmodeled: its operands and outputs are unresolved through
  unmapped vtable slots - see the ll_seq.h "NOT MODELED" note;
  resolving them (or a caller-side RE of the +0x48/+0x50 slots)
  comes before any push behavior ships.
- the condition names the data uses but our runtime scores false:
  K_JUMP (76 uses game-wide - the up-arrow route into the A_UP_*
  states is the single most valuable one), K_FIRE (253), K_ALT,
  K_F1..F12 and the engine-internal AC_* signals (AC_ONAIR 199,
  AC_ENEMY_ATC 144, AC_LINA/AC_LINA_CATCH, AC_DRAB, AC_TIME...).
  K_GEN1..3 stay the hook point for hand/foot interactions
  (A_MA_KORONE crown climb etc.), and the authored A_LADOW landing
  chain is the real replacement for the LL_PLAY_GRAVITY path.
- the control= flag names the data uses but our ctrl_flag table has no
  bit for (measured 2026-11-16, counts in the PR-46 record): 13
  tokens in 415 of the 775 scripts that carry control= lines at all -
  head of the list CHECK_LEW / DANGER / CHECK_ANI, and the collision
  family SOLID2 / SEMI_SOLID / ANTI_SOLID (SOLID itself has a bit) -
  counted as ctrl_other in every report, never silently dropped. The
  SOLID family is the authored switch for the shape collision the
  PR-46 model feeds, so resolving the push-out (ll_seq.h "NOT
  MODELED") and these flags land together.
- objects that react to the player (move platforms via their own .MOV +
  player riding; PR-45 already PLACES every authored object incl. the
  ani_plat= platform scripts - what is left is the AC_* signal bus
  that starts them and the player riding one);
- the belka HUD (`BELKA.DES`/`BELKA1.DES` - 12 of 13 level blocks
  name it through `belka=`, 0 orphan .des records game-wide says these
  are all there is); `RAN.BLK`/`RAN.SUF` (blokady=/sufity=) are the
  same pattern for their own keys.
None of this is blocked on data - 12 of 13 levels already run their
complete own player sets (see PLAN 8g). Small latent cleanup noticed on
the way: tools/verify_help.py and tools/verify_pcm.py (FIXED in PR-47) scanned
nested index tables only `count` slots (the PR-36 sparse-table bug,
same shape as the extract_record.py fix in this PR); their pins are
dense on today's fixtures so they pass - fix them before using those
probes on other levels. Also waiting for an owner-side one-liner
(workflow files are outside our token's scope): the asan job +
`ll_level --play-selftest` (see the PR-43 record).

PR-37 leaves one named golden follow-up: recover the ORIGINAL engine
text FONT (the cursive used for corner words AND chapter titles).
The capture's title band (2370 px) + corner words (2406 px) give
glyph coverage for A,C,E,I,J,O,P,R,S,T,U,W,Y,Z only - closing the
last ~5.8% needs more captured words (golden_rig can script more
menu states) or a font reconstruction. PCM parallax layer row-windows
read "0x0 rows" on RAN.01's plan2/plan1.pcm stack - the tlo ORDER is
resolved (PR-36), the window semantics are the open question
(verify-pcm owns the decoder oracle).

Nothing queued from the review's "worth fixing" set: all of it has
landed, and T-01 is finished (T-24 tooling landed in PR-19: the
the original-side capture half LANDED in PR-37/PR-40: verify-golden
compares the port's chapter map against a real capture of the original
when the fixture is present). Open
candidates: the Deferred big architecture items, or the user's call
(T-11 4:3 aspect default, T-22 AppImage).

## Deferred big items (deliberately out of scope so far)

- P-03 single window + single SDL lifecycle - DONE for both: PR-10
  (ll_view session) + PR-12 (ll_audio session). The device itself is
  per-playback by design; see the PR-12 note.
- P-04 focus stealing - DONE in PR-10 (raise on WINDOWEVENT_ENTER).
- P-05 Wayland/X11 - the automatic pipewire->alsa pin was REMOVED in
  PR-35 (SDL's own choice is the default; LEO_PIN_ALSA=1 opts in).
- E-08/T-14 palette codec fidelity (frozen: pixel output must not
  change).
- T-01 platform abstraction - DONE (PR-14 interface + headless
  backend; PR-15 SDL backend + player; PR-16 title/menu/level
  screens; PR-17 music service + platform thread; PR-18 app glue).
  Only platform_sdl.c, platform_sdl/is_player.c and the private
  ll_view/ll_audio helpers reference SDL now - pinned structurally
  by the verify-arch CI leg so it cannot silently regress.
- T-03 config knobs cleanup, T-04 globals,
  T-05/T-06 fixed-step timing + precise sleep, T-07 clock ownership,
  T-16 Polish text, T-17 save/config separation (STATUS.LEO codec
  itself is done), T-19 startup data validation, T-23 structured RE
  traces, T-24 golden-master vs original EXE (tooling + port pins
  DONE in PR-19; the original-side capture landed in PR-37/PR-40), T-26 engine state
  machine. T-25 parser fuzzing - DONE in PR-11 (tools/fuzz.c);
  extending it later = more kinds (.des text) + a longer seed list.
  T-17 writable-state (user dir + save resolution + XDG data search +
  one-time import) - DONE in PR-13; what's left of T-17 is wiring a
  config file to that same helper once a settings subsystem exists.

## Standing rules

- verify-llrs/seq/pcm/pcx byte-exact vs tools/*.py; never change
  parser acceptance; verify-menu/level/sel keep their pinned log
  strings + PPM frames (rebuild ll_cutscene.exe before verify-menu!).
- LEO_* hooks and return codes are API (is_player_run 0/1/2, menu
  -2..-6, dialog -10..-12, usage exit 2).
- Builds stay -std=gnu11 -Wall -Wextra warning-free on gcc and the
  vendored mingw clang.
