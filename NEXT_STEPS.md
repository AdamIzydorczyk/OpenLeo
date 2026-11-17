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

## PR-89 — the lion's attack lands: ATC window, enemy energy, death —
          and a root-cause fix for every lion-vs-object shape contact
          (DONE 2026-11-17; T2's stated goal "kill the first LICHA" met)

  - MEASURED (extracted .MOV texts, all 777): `ATC_ON`/`ATC_OFF` are
    frame-tail directives (engine directive table 0x917c0, decoded in
    adcea5f) and the scripts PAIR them: 267 state blocks open AND close
    the window (ZARIA's A_FIGHT: ATC_ON at frame 8 - just before its
    `FC 100,0` lunge - ATC_OFF at 13, re-open at 17; LICHA3's A_ATAK
    opens at frame -4, closes at -8), 18 blocks open without closing.
    Ported: `ll_mov_fr.atc` flags + `ll_player.atc`/`atc_hit`
    (ll_play.c/h) - the window opens on an ATC_ON frame and STAYS open
    until an ATC_OFF frame closes it; for those 18 it is also cleared
    on every state change (MODEL CHOICE: the engine's own clearing rule
    is unpinned; an attack hitting forever after one swipe is not what
    the video shows). One hit per opened window (`atc_hit`).
  - The engine pipeline (the T2 RE lead above) is what this ports:
    a touch runs the push-out and only then the hit effects
    (0x552e0 -> 0x55230); the port runs the attack hook between the
    push-out and the contact-damage block in play_collide, with the
    engine's own numbers: the dispatcher default `mov ebx,0x1e` =
    **damage 30** (0x34103, the only value shipped data points to),
    the `[obj+0x102]` invulnerability countdown default **14** (0x27264;
    the lion's own 30 at 0x50304 was already ported as pl->lock),
    `energy = -1` = never hit-able (the engine's own marker, 0x340f5),
    the death gate `cmp [eax+0xad],0; jge` = **hp < 0** (0x544a5).
  - Death enters the script's A_DEAD state BY NAME because the engine
    forces it (340 of the 413 scripts with an A_DEAD state have NO
    transition line into it; the string is hardcoded at 0x9068c et al.).
    A script with no A_DEAD state keeps its state (what the engine does
    there is open, so nothing is invented). INFERENCE, flagged in code:
    the hit raises AC_ENEMY_ATC on the HIT object's own set so its own
    `A_DOSTAL(1)=AC_ENEMY_ATC` line reacts - 0 of 279 shipped GEN lines
    raise it, so the combat code must be the raiser (PR-88 mechanism,
    no new machinery). Deliberately NOT raised on the lion (that would
    re-pin the walking fixtures; its own step). Dead objects stop
    dealing contact damage (model choice - whether the engine's corpse
    still hurts is not pinned).
  - ROOT-CAUSE FIX, bigger than the feature: `ll_seq_shape_push(oseq,
    oe, ..., le, ...)` read the LION element's margin lists out of the
    OBJECT's payload - element offsets are payload-relative and the port
    keeps one blob per .SEQ record - so EVERY lion-vs-object shape
    contact before PR-89 used wrong bytes (measured: FIGHT.SEQ el 9 vs
    the worm's MOVE.SEQ el 5 through the worm's bytes said "no contact"
    at every distance where the correct margins overlap from ~26-31 px).
    Added the paired form `ll_seq_shape_push2(sa, a..., sb, b...)`
    (ll_seq.c/h) and play_collide uses it. How the ORIGINAL resolved
    two elements into the single payload its own routine takes (one
    shared address space at load time, or per-record blocks like here)
    is NOT pinned - open question; the per-element read is the model,
    and any same-payload pairing stays byte-identical through it
    (pinned in seq_push_selftest).
  - Pins: new seq_push_selftest paired-payload leg (fixture blob_b left
    [0,2,4,6] right [4,6,1,3]: paired hit@8, push2@8 ret -8 push
    (-2,2), push2@12 ret -4 push (-1,1), the single-form through one
    payload answers ret -12 push (-4,2) - different numbers, the wrong
    bytes - and push/push2 byte-identical on a same-payload pairing;
    values measured with tools/shape_geom.py), the obj_hit selftest
    (countdown, -1 energy, A_DEAD-by-name at hp<0, no double hit), and
    Makefile legs: PR-59's solid leg RE-PINNED (was EDGE0046 at tick
    54 - a wrong-bytes artifact - now edge0041 pushes back 1 px at
    tick 28), PR-68's enemy-contact tick RE-PINNED 181 -> 175 (the
    wrong bytes made the worm's silhouette too wide, so the touch came
    ~6 px = 6 ticks early), and a NEW PR-89 leg: `1:FIRE,F1 75:NONE`
    on ZARIA.01 -> A_WALK -> A_FIGHT, one hit, `lion KILLS LICHA/
    licha3.mov: damage 30 -> hp -10 (dead)` at tick 184 (A_FIGHT at 75,
    its frame-8 ATC_ON open at 160 - model pins, not video numbers),
    no `lion hits` line (exactly one hit per window) and the killed
    worm never deals contact damage again.

## PR-90 — the ENEMY attacks: class AI identified by the .des token,
## the LICHA MAIN gate pinned, DANGER arming + window hits shipped (2026-11-17)

T3 stage 1 ("enemies attack the lion"). The RE groundwork closed the
central question of why NO .MOV anywhere transitions INTO an attack
state (ZARIA's LICHA3 included): the engine's enemy AI is COMPILED PER
CLASS and the class is selected by the placement file's own class token.

RE facts (all measured in build/leo/leo.elf, VA = file offset + 0xf000;
recipe in tools/re/make_text_asm.sh, working disassembly
/tmp/opencode/pr89_text.asm):

    (1) CLASS IDENTIFICATION. 0x616d0 is a factory: it string-compares
        a name against "SEP" (0x9320b), "PARROT" (0x9320f), "SNAKE"
        (0x93216), "LICHA" (0x9321c), "PAJAK" (0x93222), "NOSOROG"
        (0x93228), "PAPUGA" (0x93230) and allocates that class's AI
        object (0x120 bytes for the ground classes, 0x124 for NOSOROG)
        and calls its constructor: LICHA -> 0x61890 (sets [x+8] =
        0xa2f78, [x+0xc] = 0xa2f70), SNAKE -> 0x64830, PAJAK ->
        0x626c0, NOSOROG -> 0x63750, PAPUGA -> 0x53ff0. Those strings
        are exactly the class tokens the zwierzaki .des entries carry
        as field 1 (verified in the ETAPY container bytes:
        "LICHA,dummy,licha3.mov,120,650"). The same 0x931b1 string
        block also holds the LION's state names (A_JUMP, A_JUMP_S,
        A_JUMP_DOWN, A_LOOKUP, A_LOOKDOWN, A_FALL_DOWN at
        0x931cf..0x93207) - the engine knows the player's states as
        code names too.
    (2) BEHAVIOR RECORDS. The LICHA AI attach (0x618b0) builds FOUR
        behavior objects (0x32 bytes each through 0x3fb30) named
        "MAIN", "WALK", "ATACK", "TURN" (0x93238..0x93248), appends
        them to the list at [x+0x106] and gives each a method table:
        MAIN -> 0xa30a8, WALK -> 0xa308c, ATACK -> 0xa3070,
        TURN -> 0xa3054 (written at [behavior+0x2e]). Each table's
        slot +0x0c is a SETUP that pushes records by state NAME onto
        the behavior's own lists ([x+0]/[x+8]/[x+c]): 0x3fe80
        resolves a name against the script's state list at [x+0x106]
        (0x3ffe0 scans entries at +0x1e with 0x76d20) and 0x3fa20
        wraps the state object (record vtable 0xa0a28); 0x3fe10 wraps
        a state-index record. The names the setups hardcode are the
        script's own state names - "TURN","TURN","A_RUN","TURN",
        "A_ATAK" at 0x93258..0x93273, and sibling classes carry
        bigger tables (0x93480.., 0x93588.. with A_RUN/A_ATAK, IDLE
        variants, ...). So A_ATAK is ENTERED BY CODE, never by .MOV
        data - exactly what the data census had implied.
    (3) THE PINNED GATE (class LICHA). MAIN's check method is table
        0xa30a8 slot +0x18 -> 0x61b50: ebx = 2*flag + (edx==0),
        > 3 -> the "Blad zakresu w TExt...MAIN::check BOOL" error
        (0x93274), else jump table 0x61b40. CASE 0 (0x61b6e) is the
        near-lion box: abs([obj+0x26].x - lion.x) < 0x28 (40) AND
        abs([obj+0x26].y - lion.y) < 0x32 (50) - CURRENT positions
        (position objects: the object's own [x+0x26], the lion's via
        getter 0x3ff60 = [x+0x394]; x at +4, y at +0). Cases 1-3
        are [ecx+0x1a] (0x61bb1), always-true (0x61ba6), and a
        cooldown-style call 0x3fdb0 with edx=1 on [ecx+0x16]
        (0x61bb6). ATACK's check (0x61cb0, error 0x932da) case 0
        (0x61cd2) tests the lion's SIDE against a facing getter
        (vtable slot +0x70): (obj.pos0 - lion.pos0) < 0 with side
        == -1, or > 0 with side == +1; another case (0x61d26) pushes
        the double 0.5 (0x3FE999999999999A) into a random helper
        0x3fee0. The boxes differ PER CLASS (each sibling's check
        has its own immediates); only the LICHA numbers are pinned
        and used here.
    (4) scope= IS NOT THAT BOX. The placement scope pair lands at
        [obj+0xfa]/+0xfe (0x3bde0 selects a pair from the table at
        [obj+0x10e] using a counter [obj+0xf6] bounded by [obj+0xf2]
        and copies it there; used as a pair argument to a
        vtable+0x4 method at 0x3bc1d/0x3bcb9 and compared with a
        vtable+0x70 result at 0x3d70c) - consumer NOT pinned. And it
        is measurably not the arming distance: with this port's own
        shape margins the lion/worm centers hold >= 21 px apart in
        ZARIA.01, so an authored scope=20 box could NEVER open.
        scope= stays counted, unmodelled placement data.
    (5) attack-adjacent leads, still open: 0x550c0 is a "toucher is
        MOVING TOWARD the touched" gate (B's vtable+0x40 getter true,
        then sign(xB - xA) * dirB < 0) right before the generic
        contact-damage call; the attack dispatcher 0x34020 appears
        ONLY as a pointer at 0x9e204 inside the big table at
        0x9d6d8.. (called through it, no direct call site found);
        the atc_on/atc_off directive objects (parse 0x4636b..,
        12-byte objects vtable 0xa10f0, +8 = 1/0; runtime clones the
        window templates 0xa0e60/0xa0e74, whose slots are small
        data not code) still have no pinned consumer in the hit
        path.

The port (MODEL parts flagged as such; the box numbers are RE-pinned
for the class LICHA):
  - LL_CTRL_DANGER = 0x10000 internal bit (the engine's own flag word
    carries DANGER as 0x2000 - the touch path tests exactly that bit
    of the state word (`test ah,0x20` at 0x5528b/0x51019, and that
    branch loads [level+0x54], the never-authored danger_power; the
    enum's values are never the wire values). ctrl_flag() now maps
    the authored "DANGER" token (was counted-only).
  - obj_arm_atak(): an object whose CURRENT state carries the authored
    DANGER flag arms the script's OWN A_ATAK by NAME (the PR-89
    force-by-name pattern, per-state rules identical to the transition
    table's, BY_X walking position carried across) when the lion is
    inside the pinned |dx| < 40, |dy| < 50 box. No facing test, no
    probability gate - only the pinned stage. OFF unless
    LEO_PLAY_DANGER=1 (like the PR-59 solid gate); every default run
    is unchanged.
  - The enemy lands through its OWN window: in play_collide, an
    object with atc set, damage > 0, its window unlanded
    (atc_hit == 0) and the lion out of his hurt lock hits -
    energy -= authored damage=, lion lock = 30 (0x50304),
    atc_hit = 1. atc/atc_hit reset ONLY on state change (mirror of
    ll_player.atc / atc_hit). The block runs BEFORE the contact
    block: while a window is open the touch answers as the attack,
    not as an extra contact hit.
  - obj_state_selftest (under --mov-selftest) gained the arming leg:
    negatives at the EXACT boundaries (dx = -40, dy = -50 never arm -
    the engine's jge), no DANGER flag, no A_ATAK in the script,
    dead; the armed state keeps the BY_X position, its own frames
    open the window by 400 ms and close it by 600 ms.
  - verify-play leg: LEO_PLAY_DANGER=1 on ZARIA.01, 300 ticks.
    Pins (model pins, documented): arm at tick 171 (dx 38, dy 1 -
    two ticks before the old contact-only hit at 175); the worm
    STANDS in the BY_TIME attack state so the contact model next
    answers at 195/231; A_ATAK's own frames open the window at 251
    (three completed 400-unit frames, ATC_ON on the fourth) and the
    window hit lands at 265 ("ATTACKS through its window: damage 15
    -> lion energy 55"), right after the 231 contact's 30-tick lock;
    `! grep 'DANGER: lion' /tmp/opencode/ly_hit.log` pins that the
    default run never arms.
  - Tooling note that cost an hour: the Makefile's DEFAULT goal does
    NOT build ll_level - run `make ll_level` explicitly (verify-play
    depends on it, but ad-hoc probes silently used a stale binary).

## PR-91 — the LION answers an enemy attack with its OWN authored hit
reaction (T3 stage 2): the raise only; everything else was already landed.

  - The reaction is authored, complete data (measured over the extracted
    .MOV texts): ZARIA.01's lion LEW.ZAR (and LEWPL, 16 lines) carries
    "A_DOSTAL(1)=AC_ENEMY_ATC" from A_WALK/A_DRAP/A_FIGHT/A_LOOKUP...
    and a full [A_DOSTAL] block: BY_TIME, 9 frames x 200 ms, seq=hit,
    sample=12, frame 1 `I -320,-250` (a pullback: the horizontal part
    follows facing, -250 lifts the feet), frame 5 `WAIT !AC_ENEMY_ATC`
    (WAIT stays counted/unmodelled), and its ONE exit is
    A_DRAP(1)=AC_BREAK_NEXT - the frame-list wrap ends it.
  - Every mechanism that reaction needs already shipped: the player IS
    its parsed .MOV (state indexes + transition targets, PR-84/86), the
    named-signal condition tests the lion's own sig set (PR-88), seq=
    picks the state's art (PLAN 8g, live report: "0 state(s) name no
    seq= art" - the lion's hit group resolves in the real install), the
    frame `I` impulse is applied on transition entry (PR-74) and an
    authored AC_BREAK_NEXT exit is armed by the wrap (PR-83/86). The
    ONLY missing piece was the raise itself.
  - Implementation (one line + docs): the PR-90 window-hit block now
    raises AC_ENEMY_ATC on pl->p.sig (mirror of PR-89 raising it on
    the hit object). play order: ll_player_step then play_collide, so
    the CURRENT state's authored line fires on the NEXT tick - the
    one-tick pulse matches the bus semantics; no new flag needed: the
    raise sits inside the window block, which only exists when an
    attack was armed (LEO_PLAY_DANGER), so every default run stays.
  - MODEL CHOICE (flagged in the code, open question): contact hits do
    NOT raise it. The token names the enemy's ATtack and a passive
    touch has no window; raising on every touch would make every bump
    a reaction. The engine's own raiser is still unpinned (bit 16 =
    AC_ENEMY_ATC has no immediate-OR site in the text disasm).
  - Proof (offline): ll_play_selftest gained the authored chain -
    raised bit -> A_DOSTAL entry at ROW 1, vy<0 + grounded=0 (the -250
    lift), vx<0 (the -320 pullback), x actually decreased, recovery at
    the wrap via AC_BREAK_NEXT into A_DRAP at ENTRY ROW 2 (proves the
    (N) token on a signal-driven transition too), and the line stays
    dark when nobody raises the bit.
  - Proof (live): the PR-90 leg now runs to PPM tick 380 and pins the
    chain on real data: hit at 265 -> play tick 266 "state A_DOSTAL
    seq hit frame 1" (the (1) entry row; the -250 impulse hops the
    lion off the ground for two ticks, grounded again at 268, the
    pullback slides ~45 u left), frames advance at the authored
    200 ms (frame 9 by 345), wrap -> "A_DOSTAL -> A_DRAP" and
    "play tick 356: state A_DRAP seq idle frame 1". Contact hits keep
    their lock rhythm through it (331/362). All earlier pins unchanged.
  - Still open: does the engine ALSO raise on a plain touch (then a
    bump would recoil the lion - the A_WALK line reads that way); the
    A_ATAK state has NO exit except being hit itself (AC_ENEMY_ATC on
    the object), so a worm that is never interrupted loops its attack
    forever - matches "A_ATAK has one exit: A_DOSTAL(1)" in data, but
    whether the engine re-arms/aborts the state from its compiled AI is
    unpinned; the WAIT directive; sample=12/20 sounds (T5).
    (The first question is now HALF answered by the addendum below: a
    touch raises AC_ENEMY (bit 7), not AC_ENEMY_ATC (16).)

## docs addendum (no code) - the engine's signal-MERGE family and what
the CONTACT path really raises (RE after PR-91; answers half of the
"does a touch raise it" question and half of the DANGER-semantics lead):

  - TEventSet operation family (Borland regcall, eax=self, 3 dwords):
    0x49552 = AND-into, 0x49580 = OR-INTO ([eax..eax+8] |= [edx..]),
    0x495b2 = "is ANY bit set", 0x495e2 = clear. Outside the GEN
    raiser (0x48000, the ONLY name-driven raiser, reached only from
    its parser at 0x47de5), named sets are manipulated as WHOLE masks.
    No immediate-OR of 1<<16 exists anywhere in the text disasm -
    AC_ENEMY_ATC's raiser stays unpinned, exactly as PR-91 said.
  - The lion contact path (tail of the 0x50ed0 handler, 0x50fd4..
    0x51083): builds an answer packet whose FIRST 12 bytes are a named
    set - [esp] = the set returned by 0x55130 (lion=eax, object=edx,
    ecx=[lion+0xee]), then 0x51050 `or dh,0x80` = raise BIT 7 =
    AC_ENEMY, merged through 0x49580 and `rep movsd`'d (ecx=0xc) into
    the packet. So a TOUCH raises AC_ENEMY (index 7, PR-88 table),
    NOT AC_ENEMY_ATC (16) - the PR-91 model choice (contact does not
    raise the attack bit) is now SUPPORTED by pinned code, not just by
    the token name. What bit 7 does with the lion's own set is
    unpinned - no shipped LION .MOV consumes AC_ENEMY; the 14 active
    consumers are all object-side (below).
  - 0x55130 itself: gated by 0x550c0 (the toucher-moving-toward test,
    PR-90's note), damages via [touched+0xb1] through the hit method
    vtable+0xd0, and in the branch where the [x+0xad] energy test
    passes (0x5516a jle) it raises BIT 8 = AC_TIME into the returned
    set instead (0x5518c `or bl,0x1` = bit 8 of dword 0... the shift
    arithmetic lands on index 8). Exact guard meaning unpinned (0xad =
    energy per the earlier docs finding); raised-AC_TIME-on-contact is
    a NEW lead (the port models AC_TIME as the player's own wrap/
    time= arm only).
  - The DANGER branch (0x51019 `test ah,0x20` -> vtable+0xd0 with
    ds:0xcef40[+0x54], the never-authored danger_power) raises NO
    signal: on the engine side DANGER is a contact-HAZARD damage
    route, not an AI arming precondition. HANDOFF item 10's DANGER
    question: half-answered; the port's flag-as-arming-precondition
    stays the declared stand-in.
  - Data census (extracted .MOV texts, active lines only): AC_ENEMY
    is consumed 14x, all OBJECT-side - "A_DEAD(1)=AC_ENEMY" x6,
    "A_AWAY(1)=AC_ENEMY" x4, "A_SHOW(1)=AC_ENEMY" x2 (one combined
    with AC_BREAK_NEXT), "A_ATAK(1)=AC_ENEMY" x1, plus 2 commented;
    AC_ENEMY_ATC is consumed 262x - "A_DOSTAL(1)" 226,
    "A_HIT(1)=AC_ENEMY_ATC" 30, "A_ESCAPE(1)=AC_ENEMY_ATC" 6 (the
    attacked parrot). Next feature candidate: exactly those
    object-side touch reactions - touched objects' A_DEAD(1)/A_AWAY
    lines and the parrot's A_ESCAPE(1). The PR-89 raiser (hit object)
    and PR-91's mirror already feed these tables; the engine's own
    contact path feeds them AC_ENEMY (bit 7), so raising THAT bit on
    touched objects (and the AC_TIME lead above) is the data-faithful
    next step, not inventing a new token.

## PR-92 - the lion's touch raises AC_ENEMY (bit 7) on touched objects
(raises + consumes; the object's OWN lines then decide the reaction)

- Engine pin (all of it): the tail of the generic touch handler (0x50ed0,
  0x50fd4..0x51083) builds the 12-byte answer packet and sets BIT 7 into
  it at 0x51050 (`or dh,0x80`), merges 0x55130's set through the OR
  (0x49580) and writes the packet into the touched object's head
  ([obj+0x0..0xb], `rep movsd` 0x51074..0x51081). The handler is gated
  by the invulnerability countdown [obj+0x102] (0x502f0; setter default
  14, 0x27264). The port: obj_touch_enemy (ll_level.c, after
  balloon_hit) raises bit 7 on the touched object's OWN sig set and runs
  obj_transition immediately - the balloon precedent (raise, transition,
  leave the bus empty; one-beat pulses per PR-88). No new flag: the
  raise is default-on exactly like contact damage.
- Why the object's OWN set (MODEL, flagged in code): the packet is a
  mailbox at the object's head; [obj+0x0..0xb] is not modelled and
  nothing else could read the bit. The data reads the same way: all 14
  active AC_ENEMY consumers are object-side lines (census in the
  addendum), so the object's own transition table decides the reaction.
- Why a SECOND pass outside the SOLID loop (data-forced): NONE of the
  14 consumer states carries SOLID (GPTAK BY_TIME|FRAME_REV|CHECK_LEW|
  CH_DIR|DANGER, FOKA BY_TIME|CHECK_LEW|CHECK_LN, RBLOK BY_TIME|FRAME_
  REV|CHECK_LN|CHECK_LEW|CHECK_ANI|CH_DIR|DANGER) - a raise inside the
  existing solid loop alone would be data-dead. The post-loop pass uses
  the same paired-shape test (ll_seq_shape_push2, meet = d1||d2) at the
  lion's POST-push position (the engine answers after the push-out,
  0x552e0 -> 0x55230), same src/plane gates, skips BAL (own touch
  method) and objects whose CURRENT state is solid (raised in-loop
  already), and moves nobody.
- hit_cd gates the raise (mirror of the [obj+0x102] gate; PR-89's
  countdown already runs on the fixed step) but a pure touch does NOT
  SET hit_cd=14 (OPEN: the engine's handler does set 14 there; shipping
  that would make a touched object reject the lion's next hit for 14
  ticks and would re-pin the PR-89 kill fixtures - left as-is).
- Proof shipped: the --mov-selftest fixture carries the TWO CHAIN TEXTS
  VERBATIM (RBLOK: A_RUN carries A_ATAK(1)=AC_ENEMY, a touch ARMS the
  attack; GPTAK: A_ATAK carries A_DEAD(1)=AC_ENEMY) plus negatives -
  hit_cd gate, dead gate, a script without any AC_ENEMY line stays put
  (bit 7 is not K_GEN1), the bus is empty after every call. Live: the
  DANGER leg pins 'play tick 195: first touch raises AC_ENEMY on
  LICHA/licha3.mov' and the contact leg pins the same at 175 - both
  contact answers raise it; licha3.mov consumes nothing, so every
  PR-89/90/91 pin (171/175/231/265/266/356) stayed byte-identical.
- NOT live, future candidate: the only reachable bit-7 consumer chain on
  shipped data is ENA.03's RBLOK x3 (authored (1600..1640,562),
  A_ATAK(1)=AC_ENEMY) - but ENA.03 spawns the lion in A_SWIM (LEW.ENA
  23 states, spawn feet (100,560) grounded=1) and the port has no
  swimming locomotion: a 120 s FWD script never reaches x=1600
  (measured, pr92_ena_play.log: A_SWIM <-> A_SWIM_WB forever). RAN.01's
  GPTAK vultures all sit at (200,200) above ground y=298 (dy 98 > the
  40x50 box; BY_TIME, the port never moves them); LONA.05's FOKA sits
  at (893,480) in a level no play leg runs. Live consumption waits for
  swimming/flight locomotion work, not for more signal plumbing.

## PR-93 - the two missing placement grammars: static image lines and
attached children (the user's ZARIA.01 "objects not in place" report)

- The report first, honestly: the user saw ZARIA.01's scene objects in
  the wrong places (fruits in the top-left corner, the ladder/vulture/
  balloon standing on nothing) and asked whether we read the coordinates
  at all. We DO - every .des placement coordinate in the level block is
  read and used (ZWIERZ1/2.DES, ANI1/2/3/ANIPL2.DES inside the HD
  ETAPY ZARIA.01 id=2 block). What the audit found were TWO GRAMMARS of
  the placement files that the port never acted on, and both were real
  scene content:
- Grammar 1 - static image lines, three commas:
  "<LEAF>,<name>.seq,<x>,<y>" (e.g. `PICT,plan4.seq,70,480`). Measured
  census over all 13 level blocks: 84 such lines game-wide; 69 of them
  join as <level>/<LEAF-as-class-named-leaf-block>/<name>.SEQ on the CD
  ANIMACJE (the first field names the leaf table on BOTH installs; the
  15 misses are ALL LONA.05 WODOSPAD2 asking for elem26/27/29.seq,
  which the install does not carry - reported as named data holes);
  ZARIA.01 joins 14 of 14 including PANC.SEQ, which does exist. The HD
  ANIMACJE carries NO PICT/TABLICA blocks for these levels - this art
  is CD-only, so obj_static_get resolves art through the CD container
  only. These 14 ARE the visible scene the user compared against the
  video: the stone shelves the objects stand on (plan4 at 70,480 under
  the balloon at 78,460, plan3 at 180,515 under the ladder at 180,510,
  plan6 at 287,430 under the vulture at 300,450, the vine from
  plan5.seq at 130,350), the leaves and the nest. The lines carry no
  script, no properties, no motion: the element drawn is the first one
  of that table that carries patch art (obj_elem's existing no-script
  fallback) at the authored x,y through the element's own anchor (the
  PR-49 rule, no new geometry). Runtime inertness is not a policy
  choice - every play path (obj_hit/arm/advance/settle/shape_elem/
  touch_enemy/play_collide/balloon) already gates on o->sc->mov being
  non-NULL, so a placement with no .MOV was always inert; PR-93 only
  makes it DRAWN and counted. One model choice, stated in code: the
  frame-1-only draw. Whether the original animated some of these
  tables stays OPEN (no motion data exists for them; drawing frame one
  is the absence of motion data, not a guess at motion).
- Grammar 2 - attached children: five-field entries authored at exactly
  0,0. Measured: 130 such lines in the shipped data and EVERY one of
  them directly follows another entry or one of its property lines -
  the vulture's carried egg (ZARIA.01 `SEP ... POCISK,jajo.ani,
  jajo.mov,0,0`), the saw-fish on each authored RBLOK log (ENA.03), a
  drop on each leaf, the seaweed segments, the volcano smoke from each
  crater, the demo texts. Not one is a placement at the world origin -
  they are children the engine spawns FROM their parent, and WHICH
  event spawns them is unresolved. The port now still joins and counts
  their scripts and art exactly as before (only instantiation at (0,0)
  is skipped - which is what put those fruits in the corner), resets
  last_obj so a following property line cannot bind the child, and
  reports them in the "does not act on" census ("N entry line(s)
  authored at 0,0 ... never a placement at the world origin") plus the
  first four by name.
- Draw order (from the video, stated as model choice): statics compose
  BEFORE animated objects - the lion and the balloons stand IN FRONT of
  the stone shelves, and the placement-file order does not give that
  (ANI1.DES comes after the ZWIERZ files in every level). objs_draw
  became a two-pass loop over the static_img flag. PLT physics is
  unchanged and stays the only shelf collision (obj_shape_elem returns
  NULL without a .mov) - intended, the lion stands on the authored
  polylines, not on the pictures.
- Measured after PR-93 (all RE): ZARIA.01 "14 static image line(s) ...
  -> 14 placed, 0 ... data holes", "2 entry line(s) authored at 0,0",
  show line "51 animated object(s), 14 static image(s)" (51 unchanged),
  obj dump "obj PICT/plan4.seq[static image]: art PLAN4.SEQ, 0
  state(s), bbox 136x60 at (70,480)". TUNGA.11: 77 entries -> 68
  placed (1 was the 0,0 demo text, 8 stay outside its world) + 18
  statics all placed (the WODOSPAD waterfalls wodos1.seq at their
  authored x,y, the TABLICA sign, DOMEK); "tick 8: 74 more
  placement(s) advanced" (statics advance inertly and are counted; they
  take NONE of the 12 proof slots, so the strum1/strum2 proof lines are
  byte-identical). RAN.01: 827 -> 826 placed (the one 0,0 line is its
  demo text child), 0 statics - the show line keeps its old shape
  exactly when there are none. Frame/script/art census counts did NOT
  move anywhere (children stay joined). The bounded obj listing and
  LEO_OBJ_ALL counts moved with the placed set (pins updated).
- Visual proof (method now recorded for reuse): the probe recipe is
  byte-deterministic (same binary twice -> cmp identical), so a
  before/after PPM diff at a PINNED camera (LEO_LEVEL_CAM=0
  LEO_LEVEL_CAMY=440 LEO_LEVEL_PPM_TICK=10 --show) attributes pixels
  exactly: the added clusters land at the authored shelf positions, and
  they are the stone platforms + leafy branches the original footage
  shows there (yt/gp.mp4 ZARIA segment ~240-320 s). The corner fruits
  in the before frame are authored KOKOS placements at real x,y; the
  0,0 jajo children were never at the corner at this camera.
- Pins: --mov-selftest grew place_grammar_selftest - the shipped line
  shapes verbatim (SEP + its 0,0 jajo child + two PICT statics + an
  outside-world static + the GRABG 3-comma NON-entry + the ZARIA.02
  6-parameter KAMIEN line), asserting classification and placement only
  (no containers in the fixture: every name is a data hole there): 3
  entries/2 placed, 1 attached, 3 statics/2 placed/1 outside/2 art
  holes, group 1 (GRABG is NOT a static), zero instances at (0,0), the
  parent's energy= still binds the PARENT. Makefile: TUNGA.11 show +
  statics + WODOSPAD obj lines + "74 more advanced" pinned with the
  census justification in the comment; RAN.01 826/802 pinned; a NEW
  verify leg pins ZARIA.01's statics report, two static obj lines
  (plan4 shelf + gniazd nest), the split show line, the 0,0 census
  clause, and NEGATIVE-pins that jajo never stands at (0,0). JUNCTION
  and the authored-scene file/entry pins stay as they were.
- OPEN (recorded, not guessed): (a) whether any PICT .SEQ was animated
  by the original; (b) the engine trigger that spawns attached children
  from their parents; (c) the ZARIA.01 `PICT,panc.seq,20,29` line
  following the NOSOROG entry - placed at its authored (20,29) by the
  uniform grammar; a parent-relative reading of those coordinates is
  possible but unproven; (d) ZAR.BLK's fields and the GRP format
  (WODOSPAD.GRP/KIJ.GRP/KILOF.GRP) remain unread; (e) the lion's feet
  sink ~20 units below the drawn grass tops - camera/anchor follow-up,
  unrelated to placement.

## PR-94 — the engine's contact/answer protocol, verbatim RE (documentation
only: no behaviour change, no re-pins; the PR-92 shipped model stands)

- Source: /tmp/opencode/vis/text.asm (regenerate with tools/re/make_text_asm.sh;
  VA = file offset + 0xf000). Everything below is machine-code fact unless
  marked OPEN. This record supersedes the pre-compaction interim notes; two of
  them were WRONG and are corrected here (see "corrections").
- The "hit" call convention, inside ONE class family:
  `call [vtable+0xd0](eax=other, ebx=damage, ecx=0)` = "hit other with this
  damage". Call sites: 0x50fce, 0x51030 (answer-A), 0x55164 (answer-C),
  0x55255, 0x552a2 (answer-B), 0x3da2c, 0x3da7c (lion variant). The final
  generic sink is 0x54550 = `sub [obj+0xad],ebx` (hp -= damage). hp is
  [obj+0xad]; -1 = dead.
- Touch method 0x50ed0 (eax=receiver, edx=other, ebx=damage, non-NULL
  required at 0x50ef): skip if [self+0x11a] is 6 or 7, other NULL, or
  [self+0x126]!=0; if is-enemy(self) ([self.vt+0x40]) it additionally
  requires is-enemy(other). It records the counterpart: [other.vt+0xc] ->
  [self+0x10a], [other.vt+0x70] -> [self+0x10e]. The engine keeps a
  per-object CONTACT LIST [obj+0xd2] (nodes 0x5c370-allocated): 0x551f0
  pops nodes in a loop while [self+0xd2]!=0, and when the list goes empty
  calls [self.vt+0x3c]() and sets [obj+0x106]=1 - a "contact ended" edge
  the port does not model (OPEN: who fills the list). Then the
  invulnerability gate 0x502f0 ([obj+0x102] countdown, PR-92 pinned):
  countdown != 0 -> [obj.vt+0xe4](1) (a blocked-hit notify, not modelled);
  countdown == 0 -> 0x54550: hp -= damage. NOTE: 0x50ed0 ENDS at 0x50f79
  (ret); 0x50f80 starts its own prologue at 0x50f80 - the two ARE
  SEPARATE FUNCTIONS (correction: the interim "one body, two entry points"
  reading was wrong; their fixed +0x58 spacing in 25 vtables means two
  ADJACENT methods, full-implementation vs answer).
- Lion-side contact dispatch 0x340b5..0x34114 (this IS the code PR-89
  ported; it is the SECOND HALF of the 0x34020 function - see the
  vtable survey bullet): inside it, if this object's id ([self.vt+0xc]) equals global
  ds:0xced04 - the player lion, OPEN as identity but strongly implied -
  and the feet land within other.top .. other.top+20 (0x340d5/0x340e5
  getters +0x6c, and [self+0x5c]!=2): `[other+0xad] = -1` instant kill
  (the stomp); otherwise damage 0x1e=30 at 0x34103 and the call at
  0x3410c is 0x50ed0(eax=other, edx=lion, ebx=30) - the OBJECT is the
  receiver and takes the 30. PR-89's shipped numbers stand; the direction
  (receiver = object) is now pinned verbatim.
- The ANSWER packet: 12 bytes (3 dwords of the named signal set) + a
  FLAG byte at +0xc, built by 0x749a0(len 0xc, empty, flag 0), merged
  through 0x49580 (OR), delivered by `rep movsd` 12 bytes + the flag byte
  copied separately (0x51062..0x51083 = the PR-92 pinned tail; 0x551bc..
  0x551d6; 0x3dab5..0x3dac5). Three handlers, ALL vtable-only (no direct
  call sites anywhere in .text; dispatched through class vtables at
  family-specific slots):
  - answer-C 0x55130(eax=A, edx=out-buffer, ebx=C, ecx=[A+0xee]):
    predicate 0x550c0(A,C) at 0x55146 - C.is-enemy, then 0x52d30(C) /
    0x52d30(A) lookups, first-dword difference sign * [C.vt+0x70]
    (OPEN: what this orders). Gate FALSE -> C.hit(A, damage=[A+0xb1])
    through [vtable+0xd0]; compare [C+0xad] before/after: hp REDUCED ->
    raise BIT 16 = AC_ENEMY_ATC (byte2 |= 0x1 at 0x5518c); hp NOT
    reduced -> raise BIT 7 = AC_ENEMY (byte0 |= 0x80 at 0x551b0).
    (Correction: the interim read called the second branch bit 17
    AC_RECT; verbatim it is byte 2 bit 0 = bit 16 = AC_ENEMY_ATC, and
    that gives that bit a RAISER - it was on the unmodelled list.)
  - answer-A 0x50f80(eax=self, edx=buffer, ebx=other): gate 0x53ae0 —
    NOW FULLY READ, this is the SOLID collision responder (see next
    bullet); its own writes go straight into answer-A's local packet.
    [self+0x11a]==7 -> other.hit(self, damage=[self+0xb1]); filter gate
    [self.vt+0x9c](other); is-enemy(self) -> 0x55130(self, local,
    other, [self+0xee]) merged into the local packet; DANGER flag
    ([self.vt+0x44], test ah,0x20 = 0x2000) -> other.hit(level,
    damage=[[ds:0xcef40]+0x54]); then ALWAYS merge {bit 7 AC_ENEMY}
    (0x5104c/0x51050) and copy the 13-byte packet to the buffer arg.
    On a gate-fail the packet stays empty - the copy at 0x51062 still
    runs (an empty answer).
  - The answer-A gate/responder 0x53ae0(eax=self, edx=other,
    ebx=packet): geometry pair via 0x53030 (two outputs + a returned
    value at [esp+0x4c]; port counterpart = the PR-46/PR-69 shape
    push); FAILS only when the pair is (0,0) AND NOT (self has SOLID
    0x400 and other has ANTI_SOLID 0x10000). Body: if self is
    SEMI_SOLID (flag word ==/has 0x40400): |depth|>5 -> push other
    via 0x52b10(kind 1), then always a 0x52b10(kind 13 velocity-add,
    magnitude depth*30) with the depth clamped to +-4; else: raise
    BIT 0 = AC_BLOK into the packet (0x53bc8 byte0 |= 1); if self is
    SOLID2 (0x20400): message {0,1,depth} to other, and if the second
    pair output != 0 raise BIT 1 = AC_BLOK_Y merged into the packet
    (0x53c36); else message {0,1,depth}. So the answer packet a solid
    object hands its toucher is {AC_BLOK?, AC_BLOK_Y?} + the hit bits
    + AC_ENEMY - consistent with the shipped PR-46/69 push-out and
    PR-89/92 combat model. AC_BLOK/AC_BLOK_Y thus get ENGINE raisers
    (the port's own raises via shape contact stand; cross-object
    ROUTING of these bits into the touched object's table remains the
    same OPEN as before).
  - answer-B 0x55232(eax=self, edx=other, ebx=buffer): ANTI_POCISK flag
    0x80000 ([self.vt+0x44] test 0x80000) -> other.hit(self, damage 0);
    is-enemy(self) -> 0x55130(self, local, other) merged into the CALLER
    buffer; ELSE DANGER -> other.hit(level, danger); then ALWAYS merge
    {AC_ENEMY} at 0x52be into the caller buffer.
  - Lion-class variant (body 0x3d9c0..0x3dae0; entry boundary and the
    vtable slot that reaches it: OPEN): [self+0x5c] type branches 1/2;
    after the [self.vt+0x9c] filter it merges {bit 7 AC_ENEMY}
    UNCONDITIONALLY (0x3d9fb - correction: the interim note called this
    AC_RECT too); is-enemy(self) -> other.hit(self,[self+0xb1]); DANGER
    -> other.hit(level, danger); then 0x3da85: if [self+0xb1] !=
    0xf4240 (1,000,000 = "no damage" sentinel) -> [self+0xad] = -1: a
    lion-class object that IS hit dies unless the damage field carries
    the sentinel. The 0x3dae0 data-occurrence (0x95944 etc) is the
    hit-slot entry of its family (see vtable survey).
- danger_power CLOSED: every DANGER branch reads [[ds:0xcef40]+0x54]
  (0x3da7c, 0x5102b, 0x5529f). ds:0xcef40 is the global current-LEVEL
  descriptor: the shipped config parser 0x325b0 (its split patterns point
  INTO the token/keys block: 'belka' 0x90e8f, ' %d', 'ew_rect' 0x90ed3
  = 'lew_rect'+1 - the parser searches substrings) explicitly writes
  [obj+0x54]=0 at 0x3266b and NO config key writes it anywhere. So every
  shipped DANGER-route hit carries damage 0. (The +0x54 writes at
  0x326f3/0x32700 are list-node fields of a DIFFERENT object class -
  reached via [levelobj+0x394].) PR-90's LEO_PLAY_DANGER default-off is
  consistent; the authored enemy damage still comes from placement
  `damage=` (unchanged).
- AC_RECT, the T2 question "where is AC_RECT decided", PARTIAL answer: a
  grep over the shipped HD ANIMACJE .MOV corpus shows AC_RECT appears ONLY
  in lion-class scripts and level flows: ZARIA.01 LEW.ZAR/LEWPL.MOV
  A_KILOF(25)=AC_RECT,K_GEN1; SADES.06 LEW.MOV A_HOLE; LONA.05 LEW.MOV
  A_WALKINTOIGLO (PINGWIN/LONAKULA lines commented out); TUNGA.11 and
  TUNGA2.12 LEW.MOV A_EXIT=AC_RECT,AC_BLOK; KADMAR.07 0.MOV
  A_LISC_FALL(1)=AC_RECT,AC_ONAIR / A_LISC_END(1)=!AC_RECT; ZOLW.MOV
  A_JUMP. None of the contact/answer handlers above raises AC_RECT. So
  AC_RECT is raised on the LION side at stone-push/hole/igloo/exit state
  entry - not by object contact. The raiser code and the object-side
  consumer stay OPEN.
- vtable survey (data scan of /tmp/opencode/text.bin = the elf bytes
  objdump read, VA = file offset + 0x10000; + the class factory sites
  that install `mov DWORD PTR [reg+0x8], 0x9xxxx` in text.asm): 111
  distinct bases. IMPORTANT address-space note: build/leo/leo.img places
  these segments 0x1000 LOWER than the elf/objdump VAs (the same bytes
  sit at 0x959d8 in the image but 0x969d8 in the elf); scanning pointers
  in the IMAGE gave a wrong first census - this version is re-derived
  against the elf bytes and is authoritative.
  Census: answer-A 0x50f80 appears in 70 of the 111 class tables, touch
  0x50ed0 in 48, hp-sub 0x54550 in 41, enemy-AI 0x41180 in 39, the
  lion-class variant 0x3dae0 in 12, the id-keyed handler 0x34020 in 9.
  A constant +0x58 (answer-A -> touch) spacing exists in 56 bases, so
  those two ARE adjacent methods in every family that has both - as
  corrected above. BUT their ABSOLUTE slot numbers are per family: 12
  bases put answer-A at exactly +0x78 and 10 put 0x50ed0 at exactly
  +0xd0 (those are the family the §5 slot list describes); others put
  the same pair at +0x15c/+0x1b4, +0x298/+0x2f0, +0x3fc/+0x454, ...
  Top +0xd0 slot targets across the 111: 0x54550 hp-sub (19), 0x23840
  (19, a different 0x23xxx actor family), 0x50ed0 (10), 0x41180 (8),
  0x23c60 (6), 0x3dae0 (5 - bases incl. 0x96874, 0x9916c, 0x9ad48,
  0x9e740), 0x34020 (the old "0x9e204 pointer" = base 0x9e134's +0xd0
  slot). Conclusion: the HANDOFF slot table (+0x44/+0x48/+0x78/+0x9c/
  +0xd0...) is the layout of the CORE object family only; derived
  classes shift or replace slots - layouts are per family.
  Bonus finding, full reading goes to the NEXT record (T4): 0x34020 is
  a +0xd0 contact-receiver keyed on the toucher's class id compared
  against globals 0xced00/0xcecf0/0xcecf8; its player branch applies
  the hp*30/100+10 damage formula through 0x50ed0 and the third id
  sets hp=-1; its tail falls through INTO 0x340b5 - so 0x340b5 (the
  code PR-89 ported) is the second half of this same function.
- Class name table at 0x90f3d (the .des class tokens VERBATIM): ANITAB,
  PLANT, GRABOBJ, DEMOTEXT, ANIBACK, MOVEPLAT, MOVERECT, RECT, DRECT,
  DRECT2, KATPLAT, DZWIGNIA, WODOSPAD, ZWIERZ, GRUPA, PICT, POCISK,
  IDPOCISK, GRABG, GWIAZDKI, NIESP, ZONE, KORONA, SURPRISE, BAL, DNGANI,
  TRASA, TRASA2, LINA, TABLICA, BERLO. Every token the port's placement
  parser accepts is there; the parse helpers 0x325b0/0x328c0 use split
  patterns pointing INTO this block (0x90f40 'TAB' is the tail of
  'ANITAB', 0x90f4b 'RABOBJ' of 'GRABOBJ'), i.e. the shipped .des
  grammar and this table are the same thing. The tokens the port only
  counts (GRUPA, BERLO, TRASA, ...) get their full reader in future
  work; the follower strings XaXnDdOrCz etc. in the same block are the
  per-entry format strings, meanings OPEN.
- OPEN (recorded, not guessed): (a) the dispatch DIRECTION - which code
  calls the answer handlers through which family slot, and which object's
  head finally receives the answer buffer (the port currently models the
  NET effect: the touched object's own set sees AC_ENEMY); (b) the
  0x550c0 ordering predicate and 0x52d30; (c) the exact geometry the
  0x53030 pair query computes (its consumer 0x53ae0 is fully read, and
  its flag tests are the pinned SOLID/ANTI_SOLID/SEMI_SOLID/SOLID2
  words); (d) [obj+0x5c] and
  [obj+0x11a] type semantics (gates at 0x340ed/0x50ed7/0x50fb6); (e)
  the 0x3d9c0 function's entry boundary; (f) who fills the contact list
  [obj+0xd2] and what consumes [obj+0x106]=1; (g) the XaXnDdOrCz format
  strings.
- No code, no pins: RE-only commit. make/make win untouched; full
  verify-all re-run before commit as always.

## PR-95 — the attached static image: a 3-comma `.SEQ` line inside a bounded
actor's property block is drawn at parent + offset, not its literal world
corner (fixes the user's ZARIA.01 "something stands on nothing")

- Source: the placement files themselves (the LEVELDES / HD ETAPY containers;
  `/tmp/opencode/zdes_ZWIERZ1.des`, `zdes_ANI1.des`, and raw `hd_files/ETAPY`
  reads for TUNGA ANI2/ANI4). Everything here is read from the shipped bytes;
  the one model choice (draw attached to the spawn position, not a live follow)
  is marked OPEN.
- The report the user saw: ZARIA.01's rhino `NOSOROG` is placed at (1700,600)
  but the image authored right under it `PICT,panc.seq,20,29` was placed at the
  literal (20,29) — the top-left world corner — so the rhino "stood on nothing"
  and a pancake mat appeared in the sky. PR-93 had placed every 3-comma static
  at its authored x,y and had itself flagged this exact case: "a PARENT-RELATIVE
  reading is unproven (the 84-line census has no other follower of a class
  entry)" (HANDOFF §7 item 11).
- The grammar proof (this IS the second follower PR-93 lacked): in ZWIERZ1.DES
  the panc line is NOT a sibling of the entries — it sits inside the rhino's
  property block, the same position the 5-field 0,0 children use one entry up:
  `NOSOROG,dummy,nos.mov,1700,600` / `damage=10` / `bounds=1680,1950` /
  `scope=40,150` / `energy=60` / `PICT,panc.seq,20,29`. The vulture one entry
  earlier is `SEP ... energy=40` / `POCISK,jajo.ani,jajo.mov,0,0` — PR-93
  already treats that as an attached child (spawned from the parent, never a
  0,0 placement). The rhino's mat is the same construction with a real offset
  instead of 0,0. `20,29` is a parent-relative offset: the mat belongs under the
  rhino (→ 1720,629).
- The rule (decided in code, `des_place_file`, `ported/ll_level.c`): a
  3-comma static is a CHILD only when it is authored inside a BOUNDED patrol
  actor's property block. Gated by a new `after_prop` flag (set by a
  `damage=`/`bounds=`/`scope=`/`energy=` line that binds to `last_obj`, cleared
  at every entry/static placement and at each new entry-parse) AND the parent
  being bounded (`objs[last_obj].b0>=0` from `bounds=`, or `.scope>=0` from
  `scope=`). The child inherits the parent's layer and is placed at
  `parent.x+dx, parent.y+dy`. A bounded actor is the ONLY parent the data
  supports: the looser `last_obj`-only and `after_prop`-only readings each
  produced false positives that this gate rejects —
  `XCHG,dummy,malpa1.mov,1150,535 / xchg=5,2` then top-level
  `TABLICA,jajotab.seq,1082,520` (a standalone sign, absolute; the switch is
  not a bounded actor), and the ANI4 WODOSPAD waterfalls listed as SIBLINGS
  right after each `ANIBACK` entry (no property block between them). A static
  that merely follows some non-bounded object's property line, or is a sibling
  of entries (the ANI*.DES stone shelves, the waterfalls), keeps its absolute
  authored x,y — PR-93's correct handling of those stands unchanged.
- Blast radius (full census, all 13 level blocks): ONLY ZARIA.01 `PICT/panc.seq`
  → (1720,629) and ZARIA.02 → (520,679) attach. TUNGA.11/TUNGA2.12/KADMAR.07/
  LONA.05/SADES.06/ENA.03/ENA.04/KARA.07/RAN.01/JUNCTION attached=0; every
  level's "N placed, 0 at an x outside" counts are unchanged. The two ZARIA mats
  and nothing else move.
- Visual proof (not committed): /tmp/opencode/rh2_cmp.png — framed on the rhino
  at (1700,600), objects drawn. Top (pre-PR-95): the rhino on bare ground, the
  purple pancake mat at the top-left corner (off-frame). Bottom (PR-95): the
  flat purple pancake mat sits at the rhino's feet at (1720,629).
- Pins: Makefile `verify-level` gained an attached-static leg — positive
  `obj PICT/panc.seq[static image]: art PANC.SEQ, 0 state(s), bbox 92x32 at
  (1720,629)`, the report clause `... 1 authored right after an object and drawn
  attached to it at the offset`, the negative `! grep 'panc.seq.*at (20,29)'`,
  and a cross-level negative (TUNGA.11 report ends `0 authored right after an
  object`). PR-93's existing pins (plan4 70,480, gniazd 2140,270, "14 placed, 0
  at an x outside", POCISK 0,0 not instantiated) all still pass unchanged — NO
  data re-pin. The census report line now also prints the attached count.
- Selftest: `place_grammar_selftest` fixture extended with a bounded-actor case
  (`NOSOROG,...,700,600 / bounds=680,750 / scope=40 / PICT,panc.seq,20,29`)
  asserting the child stands at parent+offset (720,629), never at the corner
  (20,29), plus the negative `XCHG,... / xchg=5,2 / TABLICA,jajotab.seq,400,300`
  asserting it stays absolute. Counts updated to the 8-object fixture; new
  `static_attached==1` assertion.
- OPEN (recorded, not guessed): the mat is placed at the parent's SPAWN position
  and does NOT follow the actor as it walks, and the TRIGGER that spawns the
  5-field 0,0 children (the same question PR-93 left) is still unresolved — both
  belong to the same "attached children follow/spawn" open item (HANDOFF §7
  item 11), which this PR advances but does not close.
- make + make win warning-free; full `make -k verify-all` re-run green before
  commit. Docs: HANDOFF §7 item 11 panc sub-item marked RESOLVED.
- CI: PR-95 (`3c2937b`) green on all jobs. Note: the immediately preceding
  docs-only PR-94 (`c3e0547`) showed a windows-smoke FAILURE at
  `platform_selftest: sleep_until honors the deadline (>=20ms)` - a wall-clock
  timing selftest, unrelated to that commit (it changed no code); the same tree
  in this run passed windows-smoke, so that run was a shared-runner timing flake
  (the selftest itself is worth a later non-wall-clock rework - low priority).

## PR-96 — the contact-receiver and pickup helpers (RE only: no behaviour
change, no pins; the PR-89/92/94 shipped model stands)

- Source: /tmp/opencode/vis/text.bin disassembled to text.asm
  (tools/re/make_text_asm.sh). The dump is a flat binary at VA 0x10000 = file
  offset 0, so a labelled address in text.asm == its byte offset + 0x10000.
  vtables and data-segment globals (ds:0xce..., ds:0xcf...) live OUTSIDE this
  range and are not in the dump - they are named here, never read. Machine-code
  fact unless marked OPEN. This folds the PR-95 bonus findings into the record.
- 0x34020 is ONE function (the +0xd0 contact-receiver): prologue at 0x34020
  (push esi,edi,ebp; edi=this/receiver, esi=the argument object); it RETURNS at
  0x34096, 0x340b4, 0x34102 and 0x34114, and 0x340b5 is reached ONLY by the
  `jne 0x340b5` at 0x340a5 - so the code PR-89 ported (0x340b5..0x34114, the
  lion stomp) is this function's SECOND HALF reached by fall-through, not a
  separate method. PR-94 said 0x340b5 is "the second half of 0x34020"; that is
  now confirmed verbatim, with the exact fall-through edge and the function end
  (the ret at 0x34114, padding to 0x3411d).
- 0x34020 dispatches on the ARGUMENT's class id: it calls [arg.vt+0xc] four
  times (0x3402c / 0x34060 / 0x3409c / 0x340c5) and compares the result against
  four data-segment globals in ds:
    * == 0xced00 (0x3402f): damage ebx = (this.hp*30)/100 + 10, read from
      [this+0xad] (0x34037..0x34056: edx=[edi+0xad]; edx*0x15*2 = *30; idiv
      100; +0xa), then jmp 0x34108 -> 0x50ed0(this, arg, damage).
    * == 0xcecf0 (0x34063): the same (hp*30/100+10) at 0x3406b..0x34087, then
      0x50ed0(this, arg, damage) at 0x3408e.
    * == 0xcecf8 (0x3409f): [this+0xad] = -1 (the RECEIVER dies outright), ret.
    * == 0xced04 (0x340c8) = the player lion (PR-94; identity still OPEN): the
      PR-89 stomp - if [this via +0x6c getter] lands within the lion's
      top..top+0x14 (0x340d5..0x340eb) and [arg+0x5c]!=2, [this+0xad]=-1, else
      fall to 0x34103 damage 0x1e=30 -> 0x50ed0(this, arg, 30).
    * any other id: falls through 0x340a5->0x340b5->0x34103 -> 0x50ed0(this,
      arg, 30) with the default 30 (the receiver takes 30, exactly PR-89/94).
  The receiver is ALWAYS eax=this=edi in the 0x50ed0 call (eax=receiver), so the
  object being contacted is the one that takes the damage - consistent with the
  PR-94 "the OBJECT is the receiver" reading.
  OPEN: what 0xced00 / 0xcecf0 / 0xcecf8 identify (other lion variants? a
  class that damages proportionally to its own hp?), and why three of the four
  ids use a PROPORTIONAL hit (hp*30%+10) while the lion/else paths use the
  fixed 30. This is the strongest lead on open question 9 (the "45%-template"
  identity) - a per-class hit that scales with the receiver's own hp.
- CORRECTION (machine-verified, this session): the proportional hit at 0x34037
  (and its duplicate at 0x3406b) is energy*30/100 + 10, NOT energy*45/100+10 as
  HANDOFF §5/§7 (the "lion attack damage" ledger row and open question 9) have
  said. Byte-exact: `shl eax,4` (energy*16), `sub eax,edx` (energy*15),
  `lea edx,[eax+eax*1]` (energy*30), `idiv 0x64` (/100), `lea ebx,[eax+0xa]`
  (+10). The earlier "45%" is a scale misread of that lea (`[eax+eax*1]`=x2 vs
  `[eax+eax*2]`=x3; x3 of x15 would give 45, but the byte is x2). Hand-checked:
  energy=100 -> 40. This changes no shipped behaviour (the port ships the fixed
  30 dispatcher default either way) but the coefficient in the docs was wrong.
- open question 9 STRUCTURE, now RE-confirmed (what the 0xcecf0..0xced04 globals
  ARE): a startup routine at ~0x33bc..0x33c4a+ calls a FACTORY, 0x44350, over
  and over and stores each returned type-id into a STRIDE-4 ARRAY of globals:
  ds:0xcecdc, 0xcece0, 0xcece4, 0xcece8, 0xcecec, 0xcecf0, 0xcecf4, 0xcecf8,
  0xcecfc, 0xced00, 0xced04, 0xced08, ... (the writes at 0x33bd5..0x33c43). So
  the "factory template instances" are consecutive entries of one built-at-load
  array, not hand-coded ids - and 0x34020 compares only a subset of them
  (0xced00, 0xcecf0, 0xcecf8, 0xced04); 0xcecf4 and 0xcecfc are NOT touched by
  0x34020 (0xcecfc is read elsewhere, e.g. into eax at 0x35e30). The remaining
  OPEN half of question 9 is which 0x44350 call index yields which .des TYPE,
  and what the 0xcecfc-reading consumer does - both need 0x44350 and the 0x35e00
  region traced, deliberately not done here.
- ADDENDUM (same session, closes the 0xcecfc thread the old note left open):
  the 0x35e00 region is NOT a second contact handler. Those are per-class VTABLE
  +0xc type-getter thunks, one per class, laid out back to back in the 0x35d50..
  0x35eb5 range: each is `mov eax, <X>; ret` where X is either the literal 1
  (the ordinary classes - e.g. 0x35d50, 0x35df0, 0x35e10, 0x35e40, 0x35e90,
  0x35eb0) or one specific factory-instance global (0x35e00->0xced04, 0x35e20->
  0xced00, 0x35e30->0xcecfc, 0x35e50->0xcecf8, 0x35e60->0xcecf4, 0x35e70->
  0xcecf0, 0x35e80->0xcecec, 0x35ea0->0xcece8). So [obj.vt+0xc] (the getter
  0x34020 and the touch method call) returns the object's class type-id: 1 for
  normal classes, a global for the "special" ones. That is the whole mechanism -
  CONFIRMED. And 0xcecfc is now shown to have NO comparator anywhere in text.asm
  (it is only constructed at 0x33c25 and returned by 0x35e30; the 0x358bf
  "add BYTE PTR [ecx+0xcecfc]" hit is a mis-aligned data read, not code). So the
  old HANDOFF attribution of the "energy*45/100+10 / never-dies" behaviour to
  0xcecfc was unfounded twice over: the coefficient is 30 not 45 (above), AND the
  proportional/never-dies special cases in 0x34020 are keyed ONLY on 0xced00/
  0xcecf0/0xcecf8/0xced04 - 0xcecfc/0xcecf4/0xcecec/0xcece8 classes fall through
  to the default-30 path. Which class is which id (and why 8 classes get a
  global while the rest return 1) still needs the data-segment vtables to map the
  getter address to a constructor, so it stays OPEN - but it is no longer needed
  to explain the hit: 0x34020 fully explains the lion/object hit on its own.
- 0x3db90 = the GRABOBJ handler (slot +0x78 per the PR-94 vtable survey; the
  SLOT is not re-verified here - vtables are in the data segment, outside this
  dump). Clean function 0x3db90..0x3dca1 (ret at 0x3dca1, padding to the next
  function at 0x3dcb0). Calling convention (verified): eax=ecx=the pickup
  descriptor, ebx=esi=the receiving actor, the edx argument is saved at
  [esp+0xc]. Shape:
    1. gate: call [actor.vt+0x3c] (0x3dba4); if 0, jump to the exit at 0x3dc80.
    2. resolve the item by name: call 0x229c0 at 0x3dbcc with the descriptor's
       name pair ([ecx+0x0], [ecx+0x4]), a data-segment format string 0x98278,
       and ecx=[ecx+0x3c]+8. From the returned item record read WORD [res+0x17]
       (>>1) -> edi = an item/icon index (0x3dbd3..0x3dbe0), and WORD [res+0x19]
       *5<<4 + [res+0x15] (>>1) = a position term added to the descriptor's x,y
       (0x3dbe5..0x3dc1f).
    3. test the point against the actor's bounds: call [actor.vt+0x4c] (a rect:
       [edx]=x0,[edx+8]=x1,[edx+4]=y0,[edx+0xc]=y1) and compare (0x3dc2c..
       0x3dc4f); if outside, exit.
    4. on inside: if [ecx+0x44] != -1 call 0x52cc0(actor, edi) (0x3dc5f); call
       [ecx.vt+0x84] (0x3dc69); set edx=0xf and call 0x52b10(actor, ebx=[ecx+0x30],
       0xf) (0x3dc7b) - raises signal 15 on the descriptor's owner set.
    5. exit 0x3dc80: clear the descriptor (the saved [esp+0xc] arg ->
       [arg+0xc]=0 at 0x3dc8d) then call 0x749a0 (0x3dc91) and ret.
  OPEN: the item record's fields (+0x15/+0x17/+0x19), the 0x98278 format
  string's content, and what signal 0xf drives. (All data-segment, outside the
  dump.)
- The 0x52cc0 / 0x56600 pair (the deferred helper the user named):
    * 0x52cc0..0x52cf9 - clean function (eax=this, edx=arg): call
      [this.vt+0x30] (0x52ccb), [this.vt+0x24](0) (0x52cd5), [this.vt+0x28](0x80)
      (0x52ce2), then 0x56600(this, arg) (0x52ce9), then [this+0x20]=1 (0x52cee).
      Its near-TWIN, the function ending at 0x52cb4, runs the same
      [vt+0x24]/[vt+0x28]/0x56600 sequence and then writes [obj+0x20]=0 (0x52caa)
      - so the pair is a 0/1 switch on [obj+0x20] (a hold/release or disable/
      enable on the object).
    * 0x56600..0x5661d (eax=this, edx=index): push ebx,ecx; if index >=
      [ds:0xcf0b4] skip (0x56602..0x5660a); load the table base [ds:0xcf0ac];
      edx = [table + index*4]; call [this.vt+0x18](edx) (0x56618); pop, ret. A
      bounded index -> pointer-table -> method dispatch.
  OPEN: what [obj+0x20] gates, the meaning of the [vt+0x28] argument 0x80, and
  what the 0xcf0ac/0xcf0b4 table and signal 0xf do (all data-segment addresses).
- The factory <-> class-token table (the 0x90f3d / "LICHA"@0x9321c / 0x616d0
  note carried from PR-90) is NOT re-derivable from this repo's artifacts: those
  are addresses in the ORIGINAL Leo.exe image. build/leo/leo.img:0x90f3d holds a
  pointer table, not the class strings (checked: raw bytes there are dwords, and
  0x9321c / 0x616d0 in leo.img are code/pointers, not the ASCII tokens PR-90
  quoted). PR-90's conclusion STANDS and is unchanged: the engine's enemy AI is
  compiled per class and the class is the PLACEMENT token (the .des field-1
  token), not the field that looks like a class. Verifying the factory table
  itself needs a capture of the original executable - recorded as a deferred
  step under open question 9 / T4, deliberately NOT done here (no guessing).
- No code, no pins: RE-only. make/make win untouched; full verify-all re-run
  before commit as always (docs-only tree, expected green).

## PR-97 — the contact line: an animation's frame anchors keep its SHAPE-BOX
bottom at a fixed offset below the position point, so that row is what stands
on the `.PLT` line (the owner's report: "still same issue, leo is below grass,
gis legs are cutted becouse it is to low", on ZARIA.01)

- Symptom and first measurement: at the ZARIA.01 spawn the `.plt` main ground
  line is y=680-684 and the port settled the lion's POSITION POINT exactly
  there (`pos 50,680 grounded 1`). The camera is `cam_ymax = 500` there (no
  `back_rect`, reference layer PLAN1 is 2240x700), so that line renders at
  view row 180 - the paw line PR-84 measured on the video (paws at 175..180) -
  and PR-84's anchor was therefore already right. What was wrong is the sprite:
  with the position point on the line, the shape the game itself measures
  landed at world 710 = **view 210, below the 200-row view**, which is exactly
  "his legs are cut because he is too low".
- DATA, measured over the `.SEQ` records of ZARIA.01 and RAN.01 (throwaway
  probe /tmp/opencode/diag_d.c linked against ported/ll_seq.c; full dump
  /tmp/opencode/zaria01_D.txt; `./diag_d <ANIMACJE-dir> <LEVEL> <STEM>` prints
  each element's `D` = shape bottom below the position point): for an animated
  element the pair of margin lists - the shape the game's own box math
  `ll_seq_shape_bbox` = FUN_000420b0 computes - keeps its bottom row at a
  nearly CONSTANT offset below the position point across the whole cycle,
  while the frame anchors f0/f2 move freely:
    * `LEW.ZAR/WALK.SEQ`, 12 frames: `f2` = 31 on every frame, D = 30..32;
    * the same lion's `END.SEQ`, 30 frames: `f2` climbs 5 -> 54 while D stays
      at EXACTLY 58 - the box is stable, the anchor is not;
    * ZARIA's worm `ANIBACK/WAZ.SEQ`, 30 frames: `f0` swings -2..-22 and
      `f2` -9..-16, D fixed at 46..49 (WAZ has no `f2` at all, which is itself
      proof the anchors are not the contact);
    * `LEW.RAN/UJ09.SEQ` (RAN.01's walk cycle), 12 frames: `f2` = 45 constant
      while D is only 10..14 - **the cross-check that kills the anchor
      reading**: an anchor-based contact would leave that lion 33 px in the air.
- The engine's terrain-side geometry IS that shape box - it is what the hit
  test FUN_00042a70 and the push-out FUN_00042de0 read (PR-46/PR-58/PR-89),
  and `y + h - 2` is that code's own last scanned row (`obj+0x0e + obj+0x12 -
  2`). INFERENCE, not read as an instruction: the engine parks an actor with
  that row on the ground. What IS read is that the shape box is the only
  rectangle the data keeps pinned to a constant distance from the position
  point, and the only geometry the game's own collision code uses.
- Code:
  * `ported/ll_seq.c/.h` - `ll_seq_shape_contact(s, e)` = `ll_seq_shape_bbox`'s
    `y + h - 2`; 0 for an element with no margin lists (the game gives it no
    shape, its hit test and push-out skip those too) or a negative box (the
    all-empty-rows degenerate case already documented in ll_seq.h). The header
    note carries the tables above and the UJ09 cross-check.
  * `ported/ll_play.h/.c` - `ll_player.contact` + `ll_player_set_contact()`;
    the `x,y` doc now says plainly that it is the POSITION POINT, not the feet.
    `ll_player_step`'s ground block (its "5) ground logic" comment carries the
    proof) tests `y + contact` for the slope follow, the fall-away test and
    the landing search, and parks the position point `contact` px above the
    line it hit. With `contact` 0 (no art, no shape) the behaviour is exactly
    the previous build's.
  * `ported/ll_level.c` - `mov_state_contact(mov, sti, seq)` = the max of
    `ll_seq_shape_contact` over the elements the state's frame list can show;
    `play_contact()` resolves the animation exactly as `play_draw` does (state
    `seq=` else the `.MOV` stem, element index) and feeds the sim through
    `play_advance`; `play_spawn` (which now runs below `play_art`, since the
    starting state is what `ll_player_start` picks) parks the spawn position
    point `contact` above the surface; `follow_camy` anchors `CAM_FEET_Y` on
    `p.y + p.contact` - the line PR-84 measured the original's paws at IS the
    surface for a grounded actor, so the anchor is unchanged in effect and now
    has an exact definition.
  * `obj_contact()` + `obj_settle` apply the same rule to the `CHECK_LN`
    ground-bound placements, using the art the object's current state selects
    (`obj_art_of`) - the one `obj_draw` draws and `play_collide` tests. Not
    decoration: settled actors only touch each other if their shape boxes are
    placed by the same rule, and with only the player raised the lion's box and
    the worm's stopped overlapping, so the PR-58/PR-70 contact legs would have
    stopped firing. Static `PICT` images have no states and no `.MOV`, so they
    are not settled and PR-95's placement of them stands untouched.
- MODEL CHOICE (recorded, not read): taking the offset per ANIMATION (the max
  over the state's frames) instead of per frame. The per-element value wobbles
  a few px over a cycle - the walk cycle's lowest solid row is the planted paw
  (ZARIA WALK 30..32, RAN UJ09 10..14) - and with the per-frame value two runs
  differing only in which state they came from parked the SAME state and frame
  2 px apart (`play facing check` proved it). Max-over-animation also means no
  frame of a showing animation can dip below the line. The shape-bottom
  GEOMETRY is the data-derived part; "max over the animation" is this port's
  decision.
- MODEL CHOICE (recorded): `play_advance` re-reads the contact AFTER
  `ll_player_step` (which is where the state transitions happen) and, for a
  grounded actor whose new animation has a different offset, shifts the
  position point by exactly the difference, keeping the CONTACT LINE
  invariant. So the surface is the invariant and the logged position depends
  only on the state being shown: ZARIA.01's lion is at 648 while walking
  (WALK 32), 652 while idling (IDLE 28), 651 in A_TURN (29), 646 in A_FIGHT
  (34) - all of them standing on the same 680 line.
- Measured after: spawn log `pos 50,648 grounded 1` (648 + 32 = 680), LICHA
  worms settle at (120,669) on the 681 line instead of (120,681) (their start
  state's MOVE.SEQ carries 12 there), the centaur at the top-right of the
  ZARIA.01 render rises onto its ledge. `/tmp/opencode/pr97_final_pair.png`
  puts the same tick side by side with the pre-PR-97 binary: legs and paws on
  the grass instead of under it.
- Pins: every grounded y in `verify-play` moves by its art set's own offset
  (the target's PR-97 RE-PIN NOTE lists them) - the RAN.01 298/313 pair
  becomes 286/299, the ZARIA.01 680 floor becomes 648/651/652/646 per state,
  the ENA.03/TUNGA2.12/SADES.06/TUNGA.11 spawn snaps move 22/30/28/32, the
  ZARIA contact legs shift 175 -> 170 (both actors' boxes now reach each other
  earlier), the DANGER chain 265/266/356 -> 264/265/355, the RAN fall respawn
  305 -> 308 (the fall starts 12 px higher) and the RAN solid leg now meets
  `edge0039` (2 px) instead of `edge0041` (1 px) at the same tick 28, because
  the ANIBACK edge stack is a wall of shape-only records and which one the
  first touch hits depends on the height the actor stands at. The five
  `tools/ppm_lion_measure.py` windows are re-measured; the note in that target
  also records their weakness (they are colour masks in a window around the
  logged position, so they catch same-coloured scenery - the berry cluster at
  x 75..115 - as well as the lion: tripwires, not measurements of the lion).
  PR-78's `obj_settle` selftest is untouched (its synthetic `.MOV` carries no
  art, contact 0, expected 180 stays). One honest nuance in the RAN.01 pair:
  the spawn line prints 286 because the state `ll_player_start` picks has no
  `seq=` of its own, so the animation resolves through the `.MOV`'s fallback
  stem (`uj11`, contact 12); the first sim tick shows `A_WALK_FWD`, whose own
  `seq=uj09` carries 14, and the invariant rule parks him at 284 on the 298
  line. Self-consistent (the offset always belongs to the animation being
  shown), but it is why that pair moved 298 -> 286/284 and not by one clean
  constant.
- CI: PR-97 (`176699b`) linux + asan green on the first pass; the
  windows-smoke job failed at `platform_selftest: sleep_until honors the
  deadline (>=20ms)` - the same wall-clock selftest that flaked on PR-35,
  PR-36 and PR-94 (see the note under the PR-95 record). This PR's own
  platform code is untouched and the full `verify-all` (which runs
  `verify-platform`) passed locally twice before the commit, so the tree was
  re-run rather than changed (`gh run rerun --failed`), and that rerun then
  completed green - the same outcome as PR-35/36/94, which confirms the
  flake reading rather than a code cause. A non-wall-clock rework of that
  selftest stays a low-priority item.
- Open questions this leaves:
  1. Whether the engine's grounded y bookkeeping is really `pos + contact` or
     a per-class constant. The shape bottom is what the data pins to the ground
     line and what the game's hit/push code uses, but the instruction that
     turns it into the grounded y has not been read. OPEN, not claimed.
  2. Elements without margin lists, and the 34 all-empty-margin ones, get
     contact 0 (their position point on the line). No evidence either way for
     those.
  3. Re-grounding after a `M dx,dy` frame delta or a `P oz,px,py` position
     write (`M`/`P` move the position point) is untested - the legs that use
     them (PR-74/76/77) pass unchanged because those states have no `.SEQ` art
     in the fixtures.
  4. Only `obj_settle` follows the terrain, once at spawn; no other object is
     re-grounded continuously (the BY_X patrol states walk flat at their
     settled y). Whether the engine re-tests each object every tick is
     unread (the `G=`/`CHECK_LN` pair suggests it does).
  5. The drawn-art extent and the shape box are different rectangles; the
     anchor windows above conflate the lion and same-coloured scenery. A
     future leg should window the sprite from its own bbox instead of colour.

## PR-98 — the engine's motion model for the `F=`/`FC=`/`I`/`IC` channels,
verbatim RE (documentation + code comments only: no behaviour change, no
re-pins; closes the F=/FC=/I/IC chain half of HANDOFF §8 T1(a) - what is
left of (a) is how a BY_X state consumes `F=`, recorded as open item 2)

- Source: /tmp/opencode/vis/text.asm (tools/re/make_text_asm.sh, from
  build/leo/leo.elf: VA = file offset + 0xf000, and the dump starts at VA
  0x10000 = file offset 0, so a labelled address here is its byte offset +
  0x10000). Strings and vtables are OUTSIDE that dump, but they ARE in the
  image, so this PR reads them directly (file offset = VA - 0xf000):

      python3 -c "d=open('build/leo/leo.elf','rb').read();o=0x92281-0xf000;
      print(d[o:o+32])"          # for a string
      # for a vtable: struct.unpack_from('<12I', d, va-0xf000)

  That covers every string and vtable quoted below (0x91966/0x9196f/
  0x91971/0x91974, 0x92281/0x9228a/0x92293, 0x92270, 0xa1090, 0xa25d4,
  0xa2638); `ds:` globals stay UNREAD. Every quoted address was re-read
  rather than taken on trust.
- Builds on PR-77 (which found the parser, the 17-byte node, the vtable
  0xa1090 +0x10 accumulator handler, and the selector values `F`=0, `FC`=1,
  `I`=2, `IC`=3) and stops exactly where it stopped: "additive vector
  accumulation, NOT yet what the vectors mean physically". That is now
  answered.
- PARSER (FACT): the frame-row dispatcher is the function at 0x45df4 (no
  instruction in the dump calls it - its caller is reached through data,
  UNREAD). Each motion branch allocates a 17-byte node and fills the same
  layout: vtable 0xa1090 at +4, the selector byte at +0x10, and the `%i,%i`
  pair at +8/+0xc. Chain order 0x46878 (`I` 0x91966, `468a5: mov dl,0x2`) ->
  0x468dc (`F` 0x9196f, `46910: xor dl,dl` -> 0) -> 0x46944 (`IC` 0x91971,
  `46971: mov dl,0x3`) -> 0x469ac (`FC` 0x91974, `469d9: mov dl,0x1`). So
  these rows are not fixed struct fields in the engine either - they are a
  per-row list of nodes, exactly as the port counts them.
- THE HEADER-KEY MAPPING, READ FROM THE IMAGE (FACT). Everything below
  names struct fields after PR-77, so that mapping was re-verified rather
  than trusted. The `=` parser is 0x52590: it splits the key with the
  format at 0x92270 (`%[^[=,( ] %*[,=]`, i.e. read up to `=` and skip it),
  string-compares the key, and sscanf's the `%i,%i` pair into the record:
    * `52598: mov edx,0x92281` = `"F"`, format `0x92283` = `" %i,%i"`,
      `525b1/525b5: push ecx+0x54 / ecx+0x50` -> state `F=` = rec+0x50/54
    * `525c7: mov edx,0x9228a` = `"T"`, -> `525df/0x525e3: +0x5c / +0x58`
      -> state `T=` = rec+0x58/+0x5c
    * `5261a: mov edx,0x92293` = `"G"`, -> `52632/0x52636: +0x4c / +0x48`
      -> state `G=` = rec+0x48/+0x4c
    * any other key -> `5264f: call 0x51910` (unknown-key path)
  The bytes at those addresses are `F\0`, `" %i,%i"`, `T\0`, `G\0`, so the
  header keys really are `F=`/`T=`/`G=` and PR-77's naming was right. Same
  recipe for the vtables: 0xa25d4 is THIS record class's method table -
  slot +0x0 = 0x52590 (the `=` parser), +0x2c = 0x52680 (the merge below),
  +0x24 = 0x52660, plus 0x23c90/0x52750/0x521f0/0x23c80/0x52570/0x520c0/
  0x52360/0x1ffd0/0x52470 - and 0xa1090's +0x10 slot is 0x45d50, so the
  `call DWORD PTR [ebp+0x10]` at 0x526e9 IS the accumulator handler PR-77
  read. No unread vtable is left inside the chain merge -> 0x54250 -> /100.
  TWO MORE FACTS from 0x52590: it CLAMPS the `T=` pair to at most
  0x82 = 130 (`525f8: mov DWORD PTR [ecx+0x58],0x82`, mirrored at 0x52610
  for +0x5c), a cap no other key gets. And 0x52540..0x52567 zeroes all six
  fields (it clears +0x5c/+0x54/+0x4c and copies those zeros into
  +0x58/+0x50/+0x48); its callers are unread, so the useful consequence -
  a state with no `F=`/`T=`/`G=` line starting from 0 instead of keeping
  the previous state's values - is INFERENCE from its shape. THE CAP
  BITES ON REAL DATA: 172 authored `T=` components (last-wins per state)
  in 64 files sit above it - the lion's own TUNGA.11 A_WPAD `T=150` and
  A_WSP_LIN `T=160,50` (a BY_TY climb), LONA.05's penguin `T=1600`,
  TUNGA.11 POCISK_ARROW A_DEAD `T=1000`; `python3
  tools/re/mov_channels.py` prints that count. The port parses the
  authored number and applies no cap, which today is harmless because
  those states are BY_TY/other classes whose T= the port never reads - but
  it is a faithfulness gap for any future drag model, and it is not a
  measurement risk: the walks this repo measured are T=20 and T=40, well
  under the cap.
- MERGE, per row (0x52680 = slot +0x2c of the 0xa25d4 class table above). It
  copies the three state-header pairs into a context buffer (`+0x50/+0x54 ->
  ctx+0x0/+0x4` = the state's `F=`, `+0x58/+0x5c -> ctx+0x8/+0xc` = `T=`,
  `+0x48/+0x4c -> ctx+0x10/+0x14` = `G=` - the exact fields the `=` parser
  writes, so this mapping is FACT, not PR-77's naming), and then, through
  the node handler at 0x45d50 (`45d6e: movsx eax,BYTE PTR
  [esi+0x10]` / `45d72: cmp eax,edx` / `jne` - add `[+8]`/`[+0xc]` into two
  caller ints only when the selector equals the requested mode):
    * `526e4: mov edx,0x1` -> the `FC` nodes are added ONTO THE COPY OF THE
      STATE'S `F=` PAIR (ctx+0x0/+0x4), so F_eff = state `F=` + those pairs.
      Not decoration, and the data says so: ZARIA's A_FIGHT header is
      `F=0,0` while its rows carry `FC 100,0`, `FC 90,0`, `FC 90,0`,
      `FC 100,0`, `FC 120,0` (sum +500 x over 20 rows) - without those rows
      the punch has NO authored forward motion at all. A_JUMP is `F=180,0` +
      `FC 30,0`, A_JUMP_S `F=180,0` + `FC 80,0`, TUNGA's A_FIGHT `F=0,0` +
      `FC` sum +500. How often each appears in the 777 extracted .MOV files:
      `F` pairs 272, `FC` 65, `I` 60, `IC` 3 (re-run:
      `python3 tools/re/mov_channels.py`, added with this PR, which prints
      both channels for every state of every .MOV). The row syntax is
      `<index>,<threshold>,<directives>` (`9,300,FC 100,0`) and a leading
      `;` comments a row out, which is why PR-77 could only see the parser
      and not the counts.
    * `526f3/526fc: lea ebp,[esi+0x18]` + `call 0x45900` with dl=0 -> 0x45900
      (`45901/45907: mov DWORD PTR [...],0x0`) zeroes ctx+0x18/0x1c and then
      sums the `F` pairs there (the port's ll_mov_fr carries no F field;
      those rows are parsed and counted in the .MOV parser's
      directive-counting branch, never kept).
    * gate `5270b: cmp DWORD PTR [eax+0x95],0x0`: when that per-object flag
      is set, `mov edx,0x3` adds the `IC` pairs INTO THE F CHANNEL too and
      `mov edx,0x2` adds the `I` pairs into ctx+0x18/0x1c. What sets the flag
      is NOT pinned (set at 0x5234a/0x535d1/0x538b7, cleared at 0x52217 -
      row-transition sites); OPEN.
  So the two channels are: **F_eff = state `F=` + Σ`FC` (+ Σ`IC` when
  gated)** and **target = Σ`F` + Σ`I` (when gated)**. THE SCOPE OF THE SUMS,
  now at instruction level: the merge takes the node chain as
  `5268e: lea edi,[eax+0x2e]` / `52694: mov edi,[edi]` / `52696:
  mov edi,[edi+[A+0x4]*4]` - a table of node-chain heads inside the record,
  indexed by the field at rec+0x4 - and calls that chain's +0x10 handler,
  which adds up the nodes whose selector byte matches (0x45d50). Since the
  parser builds one chain per frame row (0x45df4 is the per-row dispatcher),
  the reading used here and in the code comments is **the sums are of the
  current frame row's directives**. Marked INFERENCE: nothing read here
  writes rec+0x4, and 0x518b6 stores the vtable 0xa2638 at [obj+0x4] of a
  neighbouring class, so that field's own role is unverified. It matters: an
  `FC` folded in for a whole state is a different animation from an `FC`
  that only applies while its own rows run.
  The authored lion data also shows the target channel is SPARSE - most
  states have no `F`/`I` rows at all (A_WALK, A_STAY, A_FIGHT have none),
  the ones that do are the jumps and the knocked-back states: ZARIA
  A_PRE_JUMP `I 250,-1100`, A_JUMP `F 0,350` + `I 150,-1200`, A_JUMP_S
  `F 0,350` + `I 120,-1200`, A_DOSTAL `I -320,-250` (its header carries
  `G=-50,55`), KARA A_PRE_JUMP one row `9,150,F -1000,100`, ENA2
  A_JUMP_LINE rows summing (1500,-1650) over 6 rows, ZARIA A_ODLOT `F` sum
  +1200. In every jump and fall row a NEGATIVE y goes UP (the launch
  pairs) and a POSITIVE y goes DOWN (the `F 0,350` rows on the fall
  states), i.e. the same y convention the port uses.
- CONSUMER, the velocity step at 0x54250 (called at 0x24731/0x321fe/0x33348;
  the wrappers pass `eax`=the object, `edx`=a filled param struct - the same
  field layout as that context, byte for byte, so it is THIS ctx: INFERENCE,
  the connecting call is through the unread vtable). Ranges 0x54170 and up
  need a clean `objdump -D -b binary -m i386 --adjust-vma=0x10000 -M intel
  --start-address=... /tmp/opencode/vis/text.bin`: text.asm's linear sweep
  desynchronizes at the jump table at 0x543be.
    * `54259..0x5427c`: if a [vt+0xbc] test passes and param+0x8 (the state
      `T=` x) is < 0x14, force it to 0x14 unless the object's byte +0xb6 has
      bit 2 - a default drag floor of 20 (OPEN: which objects carry +0xb6&2).
    * `5427c..0x542a7`: drag1 = param+0x8 · v_x /100, drag2 = param+0xc ·
      v_y /100 (the `T=` pair, as a PERCENT per tick) - computed from the
      velocity BEFORE this tick's increments, which is what makes the steady
      state below solvable.
    * `542a9..0x542c7`: `v_x += F_eff_x · [obj+0xba]/20 + G_x · [obj+0x6d]`,
      where [obj+0x6d] is the facing (+-1) and [obj+0xba]/[obj+0xbe] are
      terrain scalars the ground code recomputes each tick (written +-10/0 at
      0x54a57/0x54a82/0x54a93 - NOT .MOV-authored). y-symmetric at
      0x542cd..0x542fa. **`F=` is a per-tick velocity increment (a force),
      not a velocity and not a slew-rate limit.**
    * `0x54300..0x54337`: `v_x -= drag1`, `v_y -= drag2`; if the update
      reversed the sign, the component is set to 0 (0x5431b/0x54330).
    * `0x5433a..0x54348`: `edx = ctx+0x18 · facing`, `ebx = ctx+0x1c`
      (NO facing on y) into **0x54170, which is neither an add nor a force**.
      For a positive target: `5417e: cmp edx,ebp / jle` skips when v is
      already at or past it; otherwise `54182: lea ecx,[edx+ebp]` computes
      v + target and `5418b: cmp edx,ecx` keeps that ONLY if it did not pass
      the target (which can only happen when v opposed it), else `541a8`
      stores the target itself. So v is moved TO the target and never past
      it: from rest or from the same sign it is a ONE-TICK SET, and only a v
      that OPPOSES the target steps toward it, |target| per tick. A target of
      0 changes nothing (`0x54174`/`0x54176` and the mirror at
      `0x54191..0x541a8`), which is why states with no `F`/`I` rows are
      driven by the F=/T= loop alone. The mirror branch handles a negative
      target. This runs AFTER the drag, so while a row carrying a target is
      the current row, the `T=` drag cannot slow that axis: an `I 150,-1200`
      row pins v_x at +150·facing and v_y at -1200, it does not launch a
      burst that then decays.
    * `0x5434d..0x54372`: `v/100` is written to +0xca/+0xce (position
      deltas), so **v is in hundredths of a pixel per tick** - the port's own
      px/s numbers are a different scale, which is why no constant here can
      yet be compared with the video.
- FRAME ADVANCE BY DISTANCE, now seen in the machine code (this is PR-79's
  "movement units per frame" mechanism, which had only been measured on the
  video): the next entry in the same switch at 0x543a0 (`0x543be: jmp DWORD
  PTR cs:[eax*4+0x54380]`, indexed by a state kind `and eax,0x7 / cmp
  eax,0x4`) adds |obj+0xca| / |obj+0xce| - the WHOLE-PIXEL position deltas,
  not the scaled velocity - into [obj+0xa1] (0x543c6..0x543d1, and 0x543e7
  for the other case), and at 0x52317..0x52338 the row's own threshold comes
  from `call [vt+0xa0]` (0x52320: `mov eax,[ebx+0x10]`), is compared with
  `cmp eax,ebx` against [obj+0xa1], and when it is not reached yet the
  threshold is SUBTRACTED from the accumulator (0x52334) before the row is
  re-entered through `[esi+0x18]` - i.e. the row is held and the debt is
  carried, exactly the behaviour PR-79 pinned. The +0xa1 accumulator is set
  at 0x52118 and zeroed at 0x5236b/0x533b0/0x5389e (state/row changes), and
  two switch entries add `ds:0xcef40 -> +0x388` into it (0x543f2..0x5443d)
  when the terrain scalar is non-zero - a global tick/time scalar whose
  meaning is UNREAD (a `ds:` global).
- WHAT THE ROW CHANNEL IS FOR (DATA, and it reads cleanly): the row-level
  `F`/`I` pairs sit on exactly the motions that are SCRIPTED rather than
  continuous - jumps and their launches, the knocked-back states
  (A_DOSTAL), thrown things (ZARIA.02 POCISK_JAJO and POCISK_KOKOS
  `F 1200,-300`, SADES.06 POCISK_ARROW `F 2000,2000`, the sinking
  POCISK_HARPUN `F 1300,2200`) and the scripted rides (ENA2 A_JUMP_LINE) -
  and NEVER on a walk, idle or stand state (no lion A_WALK/A_STAY row
  carries one; A_FIGHT carries `FC` only). Read next to the
  row-held-until-travelled mechanism above (the [obj+0xa1] accumulator
  against the row's own threshold), the design says: **a row sets the
  velocity for its own segment, and the row ends when the travelled
  distance reaches its threshold**. Two consequences worth stating: the
  row-target channel and the walk law are separate questions, and the
  port's ll_mov_fr - which keeps `I` but throws the row-level `F` pairs
  away with the directive counter - is missing half of a mechanism that
  already works in the engine.
- WHAT THIS SAYS ABOUT THE PORT'S MODEL (recorded, NOT changed here). The
  port actually has TWO parameterizations: the object side (ll_level.c
  obj_walk, PR-79) walks BY_X states at F/4 px/s with the per-row distance
  thresholds; the player side (ll_play.c block 2) drives toward +/-T for a
  key-held or auto_walk BY_X state, ramping at |F| per second and clamped
  at T. Taking the engine loop exactly as read above (drag from the pre-add
  velocity, then the increment), its steady state is solvable:
      v_eq = 5·F_eff·s/T            (v units, = hundredths of a px per tick)
      px/tick = v_eq/100 = F_eff·s/(20·T)
  and that says the walking speed is proportional to F/T. THE MEASURED DATA
  SAYS OTHERWISE, and it says so with the samples this repo already has:
  PR-79 measured 25 px/s for the worm's BY_X A_RUN (F=100, T=40), 25 px/s
  for the lion's A_WALK (F=100, T=20) and ~40-50 px/s for the NOSOROG
  rhino (F=180, T=40) - three samples, two actor classes, and the speed
  follows F alone (F/4 px/s), with T=20 and T=40 giving the same result.
  So as read, the chain cannot be the whole story.
  WITHDRAWN BY PR-99: that inference does not follow from those three
  samples. The worm and the rhino share T=40, where an F law and an
  F/T law differ only by a constant (PR-79's /4 IS that constant), and
  the T=20 sample is the player - open item 8 below. PR-99's
  CORRECTION section works this same algebra with s=10 at a 20Hz tick
  and lands on both object samples AND their per-row distances. What
  survives of this paragraph is only what was true: the samples never
  vary T.
- THE OBVIOUS ESCAPE IS NOW CLOSED BY THE MAPPING. The reading that would
  have made that loop agree with the measurements was "the two channels are
  the other way round": the state's `F=` pair reaching 0x54170's target,
      v = F=   ->   px/s = F/100 * ticks/s = F/4 at 25 ticks/s
  with no T in it - literally the law the port already ships on the object
  side. The instruction-level chain rules it out: 0x54170 reads its target
  from ctx+0x18/+0x1c (0x5433a's `mov edx,[ebx+0x18]`, `mov eax,[ebx+
  0x1c]`), and 0x52680 writes those two words ONLY from the row-node sums,
  while the header pairs go to ctx+0x0..+0x14. So the header `F=` is not a
  velocity target; RAN's `A_STAY F=600` is an increment, not a 150 px/s
  target - which is also the sane reading of that data.
- WHAT IS SOLID ABOUT THE UNITS, and it is not nothing: v is divided by 100
  before it moves the object (0x5435d/0x54370), so the engine's velocity
  unit IS hundredths of a px per tick, and
      `I 150,-1200` / 100 px per tick at 25 ticks/s = (37.5, -300) px/s
  is exactly the impulse this port fitted from the video (PR-74's
  LL_PLAY_JUMP_DIV = 4 gives the same two numbers), just as PR-79's walk
  divisor 4 is F/100 px per tick at 25 Hz. Two divisors fitted from video
  landing on 100/25 is what those units at that cadence would produce; the
  cadence is still UNREAD (open item 4), so this stays a strong hint, not
  a pin.
- WHERE THE MEASURED LAW MOST LIKELY LIVES (INFERENCE, and it redirects
  the next RE step). Every F/4 measurement is a BY_X state, and a BY_X walk
  needs no velocity integrator at all: the frame-advance-by-distance
  machinery above IS a position mechanism - a row's own threshold compared
  against a travelled-distance accumulator ([obj+0xa1], with the debt
  carried at 0x52334), which is precisely what PR-79 pinned as "advance the
  frame list by distance, with a fractional carry". So a walking BY_X
  object probably never goes through 0x54250: some BY_X handler turns `F=`
  into the distance added to that accumulator per tick, and F/100 px per
  tick at 25 Hz is F/4 px/s with no T anywhere. The next step is the
  `control=` dispatch (which handler runs a BY_X / BY_TY / BY_TIME state)
  and the writer of [obj+0xa1], not another pass over 0x54250.
  REFUTED BY PR-99, which read that dispatch: it is 0x543a1 (PR-100
  CORRECTS THIS TO 0x543a0 - see the PR-100 record), its
  BY_X branch adds `[obj+0xca]` into that accumulator - a value only
  0x54250 writes (0x54365, as `v_x/100`; 0x54e56 zeroes it) - and
  apart from a set at 0x52118 and the zeroes on state/row changes
  there is no other writer. A BY_X walk does go through 0x54250.
- A SHIPPED DISCREPANCY this brings into the open: the port's player walk
  speed on ZARIA is the authored cap T=20 -> 20 px/s, while the video
  measurement is 25 px/s (PR-79 closed its own record with "re-resolve
  player physics in T1 with the video as referee" for exactly this). The
  object-side law would give 25. On RAN's A_WALK_FWD the two readings
  nearly agree (T=90 vs F/4=100 px/s), which is why it has stayed hidden.
  No pin is changed by this note; it is recorded so the next physics PR
  faces it on purpose.
- OPEN ITEMS this leaves (each flagged in the code too):
  1. CLOSED by this PR ("THE HEADER-KEY MAPPING" above): `F=` = rec+0x50/
     +0x54, `T=` = +0x58/+0x5c (the parser caps it at 130), `G=` = +0x48/
     +0x4c, read from the key strings and sscanf formats in the image, and
     the merge copies exactly those. The increment channel is the header
     `F=` plus the row `FC` pairs; 0x54170's target is only the row `F`/
     `I` sums. No claim above rests on PR-77's naming any more.
  2. THE NEW FIRST TARGET: the `control=` dispatch. Which handler runs a
     BY_X (or BY_TY) state, and what writes [obj+0xa1] - the distance
     accumulator the row thresholds are compared against. That is where the
     measured F/4 law should be, and it is also the caller of the row
     dispatcher 0x45df4 and probably the user of `ds:0xcef40 -> +0x388`.
     CLOSED BY PR-99: the dispatch is 0x543a1 (PR-100 CORRECTS THIS TO
     0x543a0) and its five mode
     branches (the BY_* enum at movrec+0x12 bits 0..2, jump table
     0x54380) are the writers of [obj+0xa1]; BY_X takes `[obj+0xca]`,
     BY_Y `[obj+0xce]`, and BY_TIME/BY_TX/BY_TY the global
     `ds:0xcef40 -> +0x388` - which therefore IS what this item was
     looking for, and its meaning is the new open item 11.
  3. What rec+0x4 is: the index that picks which node chain the merge sums
     (see the scope note above). Per-row is the reading used here; nothing
     read here writes it.
  4. The cadence: how often 0x54250 runs (its callers 0x24731/0x321fe/
     0x33348 and what paces them), and the same question for the BY_X
     handler. The /4 coincidences point at 25 Hz; the port's own play tick
     is 60 ms, and those numbers must be reconciled before any unit is
     re-derived. PR-100 NARROWS THIS: the BY_* feed is advanced by 0x521f0
     once per state-record step, so the question is now "who calls the
     record step", and the 0x54250 calls are a separate velocity path.
  5. `[obj+0xba]/[obj+0xbe]`: read their writer (0x54a00..0x54ab0 writes
     +-10/0) - a terrain constant, or a per-object scalar.
  6. The `+0x95` gate that turns `I`/`IC` rows on at all (set at
     0x5234a/0x535d1/0x538b7, cleared at 0x52217) - in the shipped data it
     decides whether an `I` row is live, so it is on the critical path of
     the jump tests below. CLOSED BY PR-100: PR-100 scanned the whole
     text section for writes of this field (every `mov dword [reg+0x95],
     0/1` form in the image) and there are exactly four: the clear at
     0x52217, which is the top of the state record's step function
     0x521f0, and three sets - 0x5234a, inside 0x521f0 just after the
     threshold compare at 0x5232b passes and the row handler at
     `[[rec+0x44]+0x18]` has been called, and 0x535d1 / 0x538b7 in the
     two sibling paths that also zero the row accumulator [obj+0xa1]
     (0x533b0 / 0x5389e) before setting it. So the flag can only mean
     "the row advanced on this record step", which is exactly what the
     merge needs (0x5270b gates the `I`/`IC` sum on `[obj+0x95]` being
     non-zero). CAVEAT: the ORDER of the record step and the merge
     inside one engine tick is not traced, so PR-98's warning that the
     value may be stale from an earlier tick survives as a caveat, not
     as an open mechanism.
  7. The `[vt+0xbc]` test and the byte +0xb6 bit 2 behind the drag floor
     20 (0x54266) - the engine also EDITS param+0x8, so the effective drag
     need not be the authored T= (which its parser already caps at 130).
  8. Whether the player object goes through 0x54250 at all (the three call
     sites touch lion-adjacent fields but check no class).
- TESTS THAT SETTLE T1 (the fixture is already in scope; no code change is
  needed to run any of them).
  a. T-dependence, ALREADY MEASURED and the sharpest result here: PR-79's
     windows give the same 25 px/s at T=20 (lion) and T=40 (worm), and
     40-50 px/s for F=180 (rhino, T=40) - so speed tracks F/4 and T plays
     no part in it. Any model whose steady state goes like F/T is dead on
     that data, and the number is consistent with the engine's own units
     (F/100 px per tick at 25 Hz = F/4 px/s). What the video does NOT
     settle is the MECHANISM: a distance added per tick to the row's
     distance accumulator (open item 2, the BY_X handler) versus an
     integrated velocity. A pair of walk states at different F AND
     different T in one level (KARA's A_WALK F=150,T=40 vs ZARIA's
     F=100,T=20: the F law predicts 37.5 vs 25 px/s, the port's
     player-side T law 40 vs 20) would pin the law harder - and gp.mp4 is
     ZARIA only, so that needs footage of another level.
     CORRECTED BY PR-99: "T plays no part / any model whose steady
     state goes like F/T is dead on that data" over-reads the samples.
     The worm and the rhino are BOTH T=40, and the "same 25 px/s at
     T=20" half is the player, whose path through the engine's
     velocity step is unverified (item 8). The pairs that would settle
     it are already in the data - F=(100,0) is authored against 11
     different T pairs, F=(150,0) against 4 (`tools/re/mov_channels.py`
     prints them) - so what is missing is only footage of a second
     level.
  b. The row-target channel, on ZARIA's jump: A_PRE_JUMP `I 250,-1100` and
     A_JUMP `I 150,-1200` are, on the reading above, a velocity that is SET
     (not a burst that decays) and re-set every tick of that row, while the
     port sets vx/vy once at the impulse row (apply_impulse: `vx = ix/4`).
     Discriminate on the HORIZONTAL track between takeoff and landing
     (gp.mp4 ep.1 ~316-317 s, ep.2 ~465 s): constant-then-cut-at-row-end
     versus a single fixed value; and with a direction change in the air,
     which the row target can reverse and the port's one-shot set cannot.
  c. The `FC` channel: any A_FIGHT window - ZARIA's A_FIGHT has F=0 and
     FC rows, so the lunge exists only while those rows run, and stops
     with them.
  Those measurements, not this note, decide whether a PR-99 re-parameterizes
  block 2. Until then T1's RE half is closed and its units half is open.
  CORRECTED BY PR-99, which read the `control=` dispatch this note was
  waiting for: (i) "a walking BY_X object probably never goes through
  0x54250" is REFUTED - the BY_X branch of that dispatch feeds the row
  accumulator from 0x54250's own `v/100` and nothing else; (ii) "Any
  model whose steady state goes like F/T is dead on that data" is
  WITHDRAWN - the two object samples share T=40, where the F and F/T
  laws coincide by construction, and the T=20 sample is the player,
  whose path through this code is open item 8; (iii) open item 2 is
  CLOSED. See the PR-99 record below for the arithmetic.

## PR-99 — the `control=` mode enum, the frame-advance dispatcher at
0x543a1 (PR-100 corrects this address to 0x543a0), and a correction of
PR-98's "the F/T law is refuted"
(documentation + the census tool only: no behaviour change, no re-pins)

RECIPE unchanged from PR-98: `objdump -D -b binary -m i386
--adjust-vma=0x10000 -M intel --start-address=... --stop-address=...
/tmp/opencode/vis/text.bin` for code, and `build/leo/leo.elf` read at
file offset = VA - 0xf000 for strings, vtables and jump tables. text.asm
desynchronizes at 0x543be and using it cost this PR one false finding
(see WITHDRAWN below) - the clean sweep is the discipline, not a
shortcut.

- THE CONTROL WORD, NOW COMPLETE (FACT; the parser 0x51b0d..0x51e07).
  PR-59 read 13 of these bits from the same function; the values match
  bit for bit and are not re-derived here. What PR-59 did not have is
  the mode enum and the five names it skipped. Taken from the token
  strings the code itself compares against (0x9215a `control`, 0x92162
  the token format, the names at 0x9216e..0x9222f), the word is 4 bytes
  at movrec+0x12, cleared at 0x51b2e and ORed one token at a time:
    * bits 0..2, a mutually exclusive MODE: `BY_X` sets nothing at all
      (0x51b49 jumps straight to the done path with the word cleared),
      `BY_Y` 0x1 (0x51b6f), `BY_TIME` 0x2 (0x51b89), `BY_TX` 0x3
      (0x51ba3), `BY_TY` 0x4 (0x51bbd);
    * in byte +0x12: `FRAME_REV` 0x8, `SEQ_REV_X` 0x10, `SEQ_REV_Y`
      0x20, `CHECK_ANI` 0x40, `CHECK_ZW` 0x80;
    * in byte +0x13: `CHECK_LN` 0x1 (0x51c59), `CHECK_LEW` 0x2,
      `SOLID` 0x4, `CHECK_BASE_LN` 0x40, `CH_DIR` 0x8, `INV_V` 0x10,
      `DANGER` 0x20, `CHECK_ALL_PLANE` 0x80;
    * in byte +0x14: `ANTI_SOLID` 0x1, `ANTI_POCISK` 0x8;
      (the +0x13/+0x14 values above are the BYTES that function ORs
      into those bytes, i.e. PR-59's word values divided by 0x100:
      CHECK_LN is bit 0x100 of the word, SOLID 0x400, and so on - the
      two tables agree);
    * three names OR a mask that spans bytes: `CHECK_ALL` 0x1c0 as a
      WORD (0x51c73) = CHECK_ANI|CHECK_ZW|CHECK_LN; `SOLID2` 0x20400
      and `SEMI_SOLID` 0x40400 as DWORDs (0x51d5f/0x51d79) - both
      masks include the `SOLID` bit 0x400 plus one further bit;
    * the token list is separated by `,` OR `|` (0x51dd8 tests 0x2c,
      0x51ddd tests 0x7c), and anything unmatched prints "Unknown
      control mode" (the string at 0x9223b, used at 0x51d99).
  The mode is read by masking, never compared against a name:
  `mov eax,[rec+0x12]; and eax,7` (0x543af) - so the low 3 bits really
  are an enum and the other bits really are flags.
- THE DISPATCHER (FACT; 0x543a0). [CORRECTED BY PR-100: this record first
  printed 0x543a1 and "PR-98 called 0x543a0" - PR-98 was right. The 0x543a1
  came from an objdump started at 0x5437e, which is mid-jump-table, so the
  sweep was one byte misaligned where the function begins; the aligned
  prologue is `push ebx; push ecx; push edx; push edi; mov ebx,eax` at
  0x543a0, and every table reference in the image is to 0x543a0. See the
  PR-100 record's CORRECTION.] This is
  the routine that advances a state's frame list, and its shape is the
  missing half of PR-79's "movement units per frame":
      543a6: eax = [obj+0x5c]                  ; state index, x4
      543a9: edx = [obj+0x50]                  ; state-record table
      543ac: eax = [edx + eax*4]               ; the CURRENT state
      543af: eax = [eax+0x12]; and eax,7; cmp eax,4; ja -> nothing
      543be: jmp DWORD [eax*4 + 0x54380]
  and the jump table (read out of the ELF, not disassembled) is exactly
  the enum the parser assigns:
      0x54380 0x543c6 BY_X     -> [obj+0xa1] += |[obj+0xca]|
      0x54384 0x543dc BY_Y     -> [obj+0xa1] += |[obj+0xce]|
      0x54388 0x543fb BY_TIME  -> [obj+0xa1] += [ds:0xcef40 -> +0x388]
      0x5438c 0x543f2 BY_TX    -> the same, only if [obj+0xba] != 0
      0x54390 0x5441b BY_TY    -> the same, only if [obj+0xbe] != 0
  Two independent sites - the bit the parser ORs, and this table's
  order - agree on the numbering, which is what makes the enum pinned
  rather than fitted. `[obj+0xca]`/`[obj+0xce]` are exactly what
  0x54250 stores at 0x54365/0x54372: `v_x/100`, `v_y/100` (0x54e56
  zeroes them), i.e. the TRUNCATED whole-pixel delta of that tick. So
  the BY_X walk path runs through 0x54250 by construction - PR-98's
  inference that it does not is closed. The only other writers of
  [obj+0xa1] are one assignment at 0x52118 (its inputs unread) and the
  zeroes at 0x5236b/0x533b0/0x5389e on state and row changes; there is
  no second per-tick feeder, and the row threshold is compared against
  that accumulator exactly as PR-98 read it (0x52317..0x52338).
- WHAT THE SHIPPED DATA DOES (FACT; `tools/re/mov_channels.py` now
  decodes the mode with the table above, so these numbers are
  reproducible): of 2318 states in 777 files, 2121 are BY_TIME, 172
  BY_X, 13 BY_Y, 12 BY_TY and none BY_TX. Of the 172 BY_X, 105 name
  `BY_X`, and 67 get it by default - 62 states carry no `control=`
  line at all, where the parser never runs and the word is whatever the
  allocation left (zero means BY_X: INFERENCE), 5 name a control list
  with no mode in it. Token frequencies (states): BY_TIME 2121,
  FRAME_REV 818, CH_DIR 556, CHECK_LEW 535, DANGER 385, CHECK_ALL 360,
  CHECK_LN 340, CHECK_ANI 168, SOLID2 152, SOLID 141, ANTI_POCISK 121,
  BY_X 105, SEMI_SOLID 92, ANTI_SOLID 87, CHECK_ALL_PLANE 62,
  CHECK_ZW 47, SEQ_REV_Y 22, BY_Y 13, BY_TY 12, CHECK_BASE_LN 8,
  SEQ_REV_X 7, INV_V 3. Not one token in the whole set falls outside
  the parser's list, and every one of the parser's 23 names is used by
  the data except `BY_TX`, which no state asks for (a check that
  passes, not an assumption - the separator rule matters here, 20
  states separate their list with commas).
- CORRECTION, the point of this PR. PR-98 recorded "Any model whose
  steady state goes like F/T is dead on that data" and HANDOFF §7.13
  still says "THE ENGINE'S WALK LAW IS REFUTED BY THIS REPO'S OWN
  MEASUREMENTS". That claim is stronger than its evidence:
    * The two OBJECT samples (the worm F=100 T=40, the rhino F=180
      T=40) share T, so they show only speed proportional to F at a
      FIXED T. They cannot separate "the force sets the speed" from
      "the force and the duration together do": at T=40 the two laws
      differ by a constant, and PR-79's /4 absorbed that constant.
    * The T=20 sample is the LION, i.e. the player, and PR-98's own
      open item 8 says it is unverified that the player goes through
      0x54250 at all. A sample from a path that may not be this one
      cannot refute a law about this one.
    * PR-98's algebra was right (its `v_eq = 5*F_eff*s/T` in v units,
      `px/tick = F_eff*s/(20*T)`); what was overstated was treating it
      as excluded. ARITHMETIC, labelled as such: with s = 10 (the
      value PR-98 read at 0x54a57) and 20 ticks/s the worm comes out
      at 100*10/(20*40) = 1.25 px/tick = 25 px/s (PR-79 measured 25),
      the rhino at 180*10/800 = 2.25 px/tick = 45 px/s (PR-79
      measured 40-50), and their row thresholds are then reached after
      4/1.25 = 3.2 ticks - 4 px per row, against the 3.0-3.75 px per
      displayed frame PR-79 measured for that worm - and 7/2.25 = 3.1
      ticks, 7 px, against the 7-10 measured for the rhino. Four
      measured numbers from one s and one tick rate is not proof, and
      both numbers are still open items (4 and 5), but it is the
      opposite of "dead on that data".
    * What DOES separate the two laws is the fixture this repo does not
      have: a BY_X OBJECT at T != 40. The authored data is full of
      them - the tool prints, for each force, every duration it is
      paired with: F=(100,0) with 11 different T pairs, F=(150,0) with
      4, F=(30,0), (75,0), (85,0), (200,0) with 2 each - and the
      prediction is sharp (the F law says those 11 states walk
      together, the F/T law says 5x apart). gp.mp4 is ZARIA only, so
      measuring it needs footage of another level, exactly as PR-79
      said.
  WITHDRAWN (this session's first pass, not PR-98): "`0x54170` also
  sets [obj+0xba]/[obj+0xbe] to 30000000/max(T,30)". That came from
  text.asm's desynchronized sweep: the clean sweep of 0x54170..0x541ec
  shows only the target-setter PR-98 described, and nothing else.
  PR-98's reading of [obj+0xba]/[obj+0xbe] (terrain scalars, written
  at 0x54a57/0x54a82/0x54a93) stands.
  NET: nothing about the port's two walk parameterizations changes, and
  no pin moves. T1 stays where PR-98 left it - the channels' RE is
  done, the mechanism is now seen in the machine code, the units half
  is still open - and PR-98's "A SHIPPED DISCREPANCY" note (the port's
  player walk 20 px/s vs the measured 25) is untouched by this PR and
  is still the most visible thing left in this area.
- A MINOR OBSERVATION, flagged not chased: the helper at 0x52540
  zeroes +0x5c/+0x54/+0x4c and then copies the SECOND component of each
  authored pair over the first (+0x5c -> +0x58, +0x54 -> +0x50,
  +0x4c -> +0x48). It sits directly before the `=` parser at 0x52590
  (which fills +0x50/+0x54 from the `F=` string, +0x58/+0x5c from
  `T=`, +0x48/+0x4c from `G=`). Its callers are unread. If it ever
  runs on a state's own record, the x slot ends up holding the
  authored SECOND value - which would matter for which component of
  `F=`/`T=` the port reads as x.
- OPEN ITEMS, PR-98's list with one closed and three added:
  2. CLOSED by this PR: the `control=` dispatch and the writer of
     [obj+0xa1] are 0x543a1 (PR-100 corrects this to 0x543a0) and its
     five branches, above. The reading
     that item was meant to support ("a BY_X handler turns F= into a
     distance per tick with no T anywhere") is not what the code does.
  9. NEW, and it is a real model difference: the BY_X accumulator only
     ever sees the TRUNCATED `|v|/100` (the idiv at 0x5435d/0x54370),
     so a walk under 1 px/tick advances the frame list by nothing, and
     on the s=10/20Hz arithmetic above the worm contributes 1 px per
     tick, not 1.25. The port's obj_walk instead carries a fractional
     accumulator. That quantization is visible at low speeds and is
     worth a test of its own.
  10. NEW: where the full-precision velocity moves the coordinates.
     0x54250 stores only `v/100` into +0xca/+0xce, and nothing read
     here uses the hundredths value for the position. The position
     writer is still unread - probably the same ground/terrain code
     that writes [obj+0xba]/[obj+0xbe].
  11. NEW: what `ds:0xcef40 -> +0x388` is (the increment BY_TIME
     adds, and with the +0xba/+0xbe gates what BY_TX/BY_TY mean). 91%
     of the shipped states are BY_TIME, so the frame pacing of almost
     every state in the game rests on that one field. The four names
     PR-59 could not pair are now paired but still unexplained:
     FRAME_REV (818 states), CHECK_ANI (168), CHECK_ZW (47),
     SEQ_REV_X/Y (7/22). PR-100 PARTLY ANSWERS THE FIRST HALF: the field
     is a level-level global with exactly one write in the whole image,
     `mov [eax+0x388],0x64` at 0x4c8b7 - a constant 100 - and all three
     time-ish modes add that same value (BY_TIME through the unguarded
     table entry at 0x543fb, BY_TX and BY_TY through the reads at 0x54406
     and 0x5442f, behind the [obj+0xba] and [obj+0xbe] gates), so the
     question narrows to "what are 100 units of?" (the BY_TIME threshold
     census, 94% exact multiples of 100 with 200/300/500/400 the common
     values, says those units are what a threshold counts: 200/300 units
     = 2/3 steps of that feed).
  In PR-98's list, 6 is CLOSED BY PR-100 (see its record) and item 4's
  code half is answered there: the dispatcher's caller is 0x521f0, the
  state record's own step method, so the feed is advanced exactly once
  per record step - how often that step runs is still the open number,
  and PR-98's "10 Hz" was a guess about it rather than a reading.
  Unchanged there: 1, 3, 5, 7, 8 - and 4 itself, now narrowed to "where
  is 0x521f0 called from" (PR-100's open item 1).
- TESTS, unchanged in kind, one added:
  d. A BY_X object at T != 40 (see CORRECTION above) - now the
     critical T1 measurement, and the data supplies the fixture as
     soon as there is footage beyond ZARIA.
  a/b/c from PR-98 stand, with (a)'s "T plays no part" reading
     downgraded to "T is not varied by these samples".

## PR-100 — the caller of the dispatcher (0x521f0, the state record's
per-tick step), a correction of this PR series' own 0x543a1, the
threshold unit model, and the level-level feed constant (documentation +
the census tool only: no behaviour change, no re-pins)

RECIPE unchanged (PR-96/97, with the PR-99 addendum). Everything below
is from the shipped `build/leo/leo.elf`, with the disassembly read by
`objdump -D -b binary -m i386 --adjust-vma=0x10000 -M intel` over
ranges of `/tmp/opencode/vis/text.bin` rather than from the vendored
`text.asm`, which desyncs at 0x543be and was not trusted here. The
threshold census is a new section of `tools/re/mov_channels.py` over
`/tmp/opencode/mov/*.MOV` (data).

- CORRECTION TO TAKE, FIRST: the address is **0x543a0, not 0x543a1**.
  PR-98 printed 0x543a0 and PR-99 "corrected" it to 0x543a1; PR-98 was
  right. How I know:
  * the function prologue, read from an ALIGNED sweep, is
    `push ebx; push ecx; push edx; push edi; mov ebx,eax` at **0x543a0**
    (bytes 53 51 52 57 89 c3). At 0x543a1 that byte stream begins
    `push ecx`, which would leave ebx - the object the whole function
    uses - unset. The earlier 0x543a1 came from an objdump started at
    0x5437e, which is in the middle of the jump table, so the sweep was
    one byte out of step exactly where the function begins;
  * the image contains **155 raw occurrences of the word 0x543a0** and
    none of 0x543a1, and every one of them sits at offset +0xa0 inside a
    table in 0x96914..0xa65c0, the class method-table area (the slot
    before it is 0x23a40 = `mov eax,1; ret` in 150 of the 155 and
    0x3d980 in the other five; the slot after is 0x239c0 = a bare `ret`
    in 153 of them). 31 tables instead put 0x239d0 at +0xa0 - the same
    feed with no mode switch at all, `[obj+0xa1] +=
    [0xcef40 -> +0x388]` - and they carry the same two neighbours;
  * the call sites agree: `mov eax,[obj+0x8]; call [eax+0xa0]`. The
    constructors store TWO views of one table - `[obj+0x8]` = the table
    base and `[obj+0xc]` = base+8 (0x1ed08 stores 0x96874 into +0x8 and
    0x1ed0f stores 0x9686c into +0xc, so `call [eax+0x134]` on the
    second view is the same slot as `call [eax+0x13c]` on the first).
    PR-100's own guess about this, drafted from PR-99's notes, was wrong
    and is corrected here: in THAT table 0x54250 sits at +0xc8 and the
    slot at +0x134 is 0x23aa0 (a current-row accessor: it reads
    [obj+0x5c]/[obj+0x50] and then the node's +0x8/+0xc and calls
    0x229c0). PR-99's `[[obj+8]+0x134]` sites are calls on other
    classes' tables, where a different method happens to land at that
    offset - a slot number is only meaningful together with the table,
    which is why open item 5 asks for the table map rather than a slot
    census.
  Everything else PR-99 printed inside this function is CONFIRMED by the
  aligned sweep - the mode read `mov eax,[eax+0x12]; and eax,7; cmp
  eax,4; ja 0x5443f` at 0x543af-0x543b8, the jump `jmp DWORD
  [eax*4+0x54380]` at 0x543be, the table at 0x54380 (an island just
  before the entry, which is why a sweep from 0x5437e looked continuous),
  and the BY_X / BY_Y / BY_TIME / BY_TX bodies at 0x543c6 / 0x543dc /
  0x543f2 / 0x5441b with the +0xba / +0xbe guards. The error was only
  the entry address.
- THE CALLER: **0x521f0**, and it is a state record's step function, not
  an object one. 0x521f8 keeps `eax` as ebp = the record, and
  `[ebp+0x00]` is the owner object - it is loaded at 0x52214 to clear
  [obj+0x95], at 0x52311 to reach the method table, and the merge at
  0x526df reads the same back-pointer at 0x52705-0x52709 before gating
  on `[obj+0x95]` there, which is what ties the two together. Its
  shape, with the dispatcher's caller's half that PR-99 left open:
  * 0x521f8 `mov ebp,eax` (the record); the caller's second argument
    (edx) is kept at [esp+0x68] as a key buffer, and the prologue builds
    an empty 12-byte key locally (0x749a0 with the length 0xc);
  * 0x52214-0x52217: `eax = [ebp+0]` (the owner object), then
    **[obj+0x95] = 0** on entry (open item 6, closed above);
  * 0x52221-0x52229 takes the CURRENT row node: `eax = [[rec+0x2e]]` -
    the node-array pointer at the head of the chain object embedded in the
    record at +0x2e - `ebx = [rec+0x4]` (the row index) and
    `node = [eax + ebx*4]`. Same chain object and index pair PR-98 mapped
    from the merge at 0x526df, so the step and the merge read the same
    current row;
  * 0x5222c-0x5224b: `call [[node+0x4]+0x8]` fills a buffer with the
    NODE's key (12 payload bytes plus a length byte at +0xc), and 0x495b0
    answers "are all three dwords zero?" - an EMPTY key takes the branch
    at 0x52251, a non-empty key takes 0x522fc. What that key is (a
    runtime identity, not an authored field - see open item 4, which
    also corrects the "authored `key=`" wording of PR-100's own draft)
    is open;
  * EMPTY-KEY BRANCH (0x52251..0x522fa): copies the caller's key and the
    object's own key at [obj+0x60] into two buffers, combines them with
    0x49580 (which is an in-place OR of three dwords, NOT a lookup -
    PR-100 corrects the guess PR-98's notes called it a pair-compare),
    looks the result up with 0x47e40 (null = return), and calls the row
    handler `[[rec+0x44]+0x18]` at 0x52338 with ebx = 0 - no threshold,
    no remainder;
  * KEYED BRANCH (0x522fc..0x5234a): `call [[node+0x4]+0xc]` makes the
    next key and 0x49580 ORs it into the caller's key buffer in place;
    then 0x52311-0x52317 `mov eax,[ebp]; mov edx,[eax+8];
    call [edx+0xa0]` - **the dispatcher, slot +0xa0 of the owner's method
    table, once per record step**; then 0x52320-0x5232b `eax = [node+0x10]`
    (the row's threshold), `ebx = [obj+0xa1]` (the accumulator),
    `cmp eax,ebx; jge 0x52354` = **threshold >= accumulator -> return with
    the debt kept**, which is PR-98's `while (acc >= thr)` gate read from
    the other side, and it admits at most one row per step; when the
    accumulator is strictly greater, 0x52334 hands ebx = acc - threshold
    to `[[rec+0x44]+0x18]`, 0x52341 calls `[[obj+0x8]+0xa4]` (0x239c0 =
    a bare `ret` on the movrec tables checked - an empty hook), and
    0x5234a sets [obj+0x95] = 1.
  So the row-advance decision is one row per record step, the remainder is
  handed to the row handler, and the accumulator is not zeroed by a row
  advance. PR-100 scanned the text section for every write of the form
  `mov dword [reg+0xa1],0` and there are exactly three: 0x5236b, inside
  the helper at 0x52360 (open item 2 - PR-100 first called this block
  "unreferenced" because it looked for the address 0x5235f, one byte
  early, and found nothing), and 0x533b0 / 0x5389e in the two sibling
  paths that set [obj+0x95] a
  little later (0x535d1 / 0x538b7). The only non-zero assignment found is
  the one PR-99 noted at 0x52118, which stores the result of an owner
  method. So the carry is real and it stays in [obj+0xa1] until a state
  or row change runs one of those three zeroes.
- THE FEED CONSTANT FOR BY_TIME / BY_TY: `ds:0xcef40 -> +0x388` has
  exactly ONE write in the image - `mov dword [eax+0x388],0x64` at
  **0x4c8b7**, the constant **100** - inside a method of the class whose
  methods read that global through 0xcef40 (0x239de, 0x54406, 0x5442f)
  and whose +0x390 block is what 0x54458 reads through the same global.
  The function that writes it starts at 0x4c8a0 and takes the object in
  eax; its only direct call in the image is 0x4a65f, at the end of a
  small setter-shaped stub (`call 0x4a8c0`, then `mov eax,ebx;
  mov edx,esi; call 0x4c8a0; ret 4`). Who calls THAT stub is unread, so
  WHEN the constant is installed in a level's lifetime is open; what is
  not open is that it is a constant, not a measured thing. The same
  class's fields +0x398/+0x39c/+0x3e6 are
  used by the state parser at 0x5e340/0x5d2a1, and +0x3a0 is the state
  record array the parser indexes (PR-59), so that global IS the level's
  own record object - the context of the whole MOVRECORD area, which
  closes part of what PR-98's item 2 was groping for.
- THE THRESHOLD UNITS (data, new census section in the tool). Reading the
  row threshold field `[node+0x10]` through the engine's own comparison
  against the accumulator says, per mode:
    BY_TIME: 14881 rows, median 300, max 40000, and **13932 of them
      (94%) are exact multiples of 100** - 200 x4217, 300 x2552,
      500 x2309, 400 x2166, 100 x1397, 250 x258.
    BY_TY:   108 rows, min 200, median 200, max 500, **108 of 108
      multiples of 100** - 200 x82, 300 x16, 500 x10.
    BY_X:    970 rows, median 4, **885 of them <= 20** - 4 x400, 6 x239,
      2 x101, 5 x71, 8 x20, 7 x18, and only 53 multiples of 100; the
      long tail (46 rows of exactly 100, max 5000 at KADMAR's falling
      `KOLUMNA` pillars) is states that are not ground walkers.
    BY_Y:    37 rows, min 1, median 6, max 6 - all of them <= 20.
  WHAT THAT DOES AND DOESN'T ESTABLISH:
  * FACT (mechanical): the engine compares once per record step and
    admits at most one row per step, so a threshold at or below the
    per-step feed means "advance every step". The BY_X numbers are of
    exactly that order - a walk at the F/4 law feeds about F/4 = 25 per
    step at F=100 (0x543a0's BY_X branch adds the truncated `|v|/100`
    of the hundredths value 0x54250 stores) while its authored
    thresholds are 2..6 - and the BY_TIME / BY_TY medians (300, 200)
    against a feed of exactly 100 mean 3 and 2 steps.
  * READING, NOT PINNED: that the threshold is in the mode's own feed
    unit - counted px of travel for BY_X/BY_Y, feed-steps for
    BY_TIME/BY_TX/BY_TY. Nothing measured says so; it is what the
    comparison at 0x52320 implies IF the feed is what 0x543a0's
    branches write.
  * NOT ESTABLISHED, and this is the honest part: the samples show no
    clean threshold-vs-F relation. F=100,T=40 states carry 2 (KARA's
    `ZABA`), 4 (`KOLC`) and 6 (the lion's own `A_WALK`), the lion's
    A_WALK is 6 in ENA and 60 in LONA at T=20, and the F/T pairs in
    the shipped set are dominated by F=100 with T from 10 to 50. So
    this does not re-derive the walk law: the F-vs-F/T question stays
    exactly where PR-99 left it, still needing a T != 40 fixture.
    **No pin moves.**
- WHAT THE PORT DOES WITH THIS, UNCHANGED AND NOW BETTER LABELLED:
  `obj_walk` accumulates |vx|·dt/1000 into `o->facc` and `obj_advance`
  gates on `o->facc < f->ms`, which is the engine's shape (distance in,
  threshold compare, carry kept, one row per gate). The differences are
  the per-tick feed (the port adds the continuous F/4-style distance
  through the ground-friction branch; the engine adds truncated `|v|/100`
  per record step) and that the port's walk gate runs inside its own
  60 ms play tick. Nothing to change from this PR; PR-99's item 9 (the
  engine truncating the feed to whole px/tick) is still the visible
  difference at low speeds.
- WHAT `[[rec+0x44]+N]` IS - THE RECORD-CLASS SUB-TABLE. The record is
  96 bytes: the constructor at 0x1e104 is `mov eax,0x60; call 0x5c320`
  (allocate), `mov edx,esi; call 0x524e0` (initialise, owner in edx),
  then `1e119: mov dword [eax+0x44],0x97c9c` - and 0x97c9c is exactly
  where the `=` parser 0x52590 sits. So [rec+0x44] is a SECOND method
  table inside the record object, one per record class, and the calls
  `[[rec+0x44]+0x8]`, `+0xc`, `+0x18]` in 0x521f0 are virtual calls on
  the record itself. Anchoring on the step slot gives 16 such tables
  (0x97c9c, 0x98e7c, 0x99338, 0x9a85c, 0x9a894, 0x9a8cc, 0x9b5f4,
  0x9cedc, 0x9d53c, 0x9e254, 0x9f460, 0x9fbfc, 0xa25d4, 0xa260c,
  0xa2e9c, 0xa2ed4); a slot-by-slot census over those 16 says which
  methods are shared by the whole family and which are per class:
    +0x0c 0x521f0  THE STEP                   16/16
    +0x10 0x23c80                            16/16
    +0x18 0x520c0  the row-change handler    16/16, no direct caller
    +0x28 0x52470                           16/16, no direct caller
    +0x00 0x52590  the `F=`/`T=`/`G=` parser 15/16; the 16th holds
                   0x51910, the unknown-key path the parser itself jumps
                   to for an unrecognised key
    +0x04 0x23c90                           15/16 (once 0x61270)
    +0x14 0x52570                           15/16 (once 0x518f0)
    +0x2c 0x52680  the merge of PR-98        15/16 (once 0x55820)
    +0x34 0x52790                           15/16 (once 0x45800)
    +0x20 0x52660 13/16; once each 0x50060, 0x50020, 0x523e0
    +0x1c 0x4fef0 10/16; 0x52360 4/16; once each 0x50040, 0x50000
    +0x08 the key builder: 12 different functions, 0x52750 5x and
         0x4ffb0/0x23f90/0x26470/0x26fd0/0x2c360/0x2ded0/0x31fa0/
         0x33c60/0x361c0/0x559c0/0x61200 once each
    +0x24 and +0x30 are per class too (0x230c0/0x263d0/... and
         0x4ff80 10x, 0x2b4d0 3x, 0x61250 2x, 0x457e0 1x)
  What the shared ones do, briefly: +0x18 is the row-change handler
  (open item 3); +0x28 = 0x52470 takes the current node and calls ITS
  `+0x10` method with edx = 4 and ebx = the caller's buffer - the same
  node method 0x520c0 calls with edx = 6 - so that +0x10 is a node
  descriptor with a mode code, not a next-row search; +0x14 = 0x52570
  ALLOCATES A NEW RECORD (`mov eax,0x60; call 0x5c320`), copies this
  one into it through 0x52500 and writes the new owner into [new+0],
  which is how an authored level's records get instantiated per object;
  +0x20 = 0x52660 calls 0x54f90 on the owner, clears bit 0x20 of
  [owner+0xb6] and calls 0x523e0 (the two-instruction wrapper
  `mov eax,[eax]; call 0x52d00`), i.e. the record's leave hook, and
  0x535a0 runs it on the OLD record during a switch.
  That settles PR-77's table 0xa25d4 as one member of this family and
  makes the item-2 helper 0x52360 a row-set method that the per-class
  +0x1c methods call: its three direct calls are at 0x4ff6b (inside
  0x4fef0, the +0x1c slot of 10 classes), 0x50003 (inside 0x50000) and
  0x50043 (inside 0x50040) - 0x50000 and 0x50040 are themselves +0x1c
  slots of two other classes, all three are the same shape, and 0x4ff80
  (the +0x30 slot of 10 classes) is the 60/60/7/7 writer next door.
  The class-of-record mapping (which state type uses which of the 16) is
  still open: the constructors that install these tables are reachable
  (0x1e119 installs 0x97c9c, 0x23115 also does, 0x240f9/0x26428 install
  0x98e7c) but tying them to a state id needs the parser side.
- THE CALLER OF THE STEP, AND THE 13-BYTE KEY (closes most of item 1 of
  PR-98 and item 1 below). `call [[rec+0x44]+0xc]` exists exactly once,
  at **0x5240c inside 0x523f0**, and 0x523f0 has exactly one direct
  caller, **0x5371c**. 0x523f0 is a key builder: it puts a 13-byte
  buffer on the stack, then in order calls `[[rec+0x44]+0x8]` (the
  per-class key builder, edx = the buffer), the STEP,
  `[[obj+0x8]+0xac]` and `[[obj+0x8]+0xa8]` (two methods of the OWNER,
  same buffer), then `[[rec+0x44]+0x10]` (0x23c80, ecx = 0xc), and
  finally copies those 13 bytes - three dwords plus a length byte at
  +0xc, the same shape 0x521f0 uses at 0x5222c-0x5224b - to the
  caller's out pointer and returns it. One of those key builders,
  0x4ffb0, reads `[owner+0x50]` indexed by `[owner+0x5c]` (the frame
  array of PR-77) and takes a branch on `[owner+0x126]` and on the
  globals 0xcf068/0xcf074. So the record step runs as part of computing
  the object's identity key, and the key is assembled from the record
  class, the owner and the frame - which is the strongest lead yet for
  the `[obj+0x60]` / 0x47e40 key question of item 4.
- THE FAR SIDE OF THAT CALLER CHAIN, found by following 0x5371c one
  step on. 0x5371c is inside **0x536f0**, a key compositor: it calls
  `[[obj+8]+0x8c]` into a scratch buffer and ORs that into the object's
  key at **[obj+0x60]** with 0x49580, then takes the object's CURRENT
  record `[[obj+0x50] + [obj+0x5c]*4]`, passes it to 0x523f0 - which
  fills a 13-byte key out of the record class, the step, the owner and
  the frame - and ORs that key into [obj+0x60] too, then calls
  `[[obj+8]+0x54]`, 0x53520 (which copies 13 bytes from [obj+0x60] and
  re-reads [obj+0x50]/[obj+0x5c]) and 0x535a0. So
  PR-77's "frame array" at [obj+0x50]/[obj+0x5c] IS the record array
  plus a current index, and each frame carries its own motion record.
  0x536f0 has exactly one direct call, 0x550b4 in a stub that first
  clears [obj+0xe6] and [obj+0xea], and 31 method-table slots across
  the object classes; there is no `call [reg+0x128]` or
  `call [reg+0x130]` site in the image, so the offset it is dispatched
  through is not one of the obvious ones - that offset, and who runs it,
  is the cadence question now.
- **0x535a0 IS THE RECORD/FRAME SWITCH**, and it is the third of the
  three paths that set [obj+0x95] (at 0x535d1). With the new index in
  edx: if it equals [obj+0x5c] it returns; else it calls the CURRENT
  record's `[[rec+0x44]+0x20]` (0x52660 in 13 of the 16) with the
  object's key as the argument, zeroes [obj+0x9d], sets [obj+0x95] = 1,
  calls `[[obj+8]+0xa4]` (the bare `ret` hook, 0x239c0), takes the NEW
  record `[[obj+0x50] + edx*4]`, calls its `[[rec+0x44]+0x1c]` - the
  0x52360 / 0x4fef0 row-set method of item 2 - writes [obj+0x5c] = the
  new index and calls `[[obj+8]+0xac]` with the key. So "the frame
  changed" and "the row advanced" are the same event in the object's
  own bookkeeping, which is a second argument for the row-order reading
  of bits 0x10/0x20. NOTE AN ARGUMENT MYSTAKE TO CHECK, not a fact:
  0x52360 uses its edx as a row INDEX (`test edx,edx; jl` out, then
  [rec+4] = edx or `[rec+0x2e]+8 - edx - 1`), but at the 0x535a0 call
  site edx is not the new index - the index is saved into esi at
  0x535a6 and edx is reloaded from the caller's ebx at 0x535a8 (the
  buffer that same method passes to the +0x20 call), and the hook in
  between is the bare `ret` 0x239c0, so that pointer survives into the
  +0x1c call. 0x50000 and 0x50040 likewise only forward their own edx.
  So either the +0x1c slot has different argument conventions in
  different classes, or one of these two readings is wrong; 0x535a0 is
  one object class's switch and the four record classes that hold
  0x52360 may be driven by another one. Resolving it is PR-101's first
  job.
- OPEN ITEMS AFTER PR-100 (numbering continues; PR-99's 4 and 11 are
  partly closed above, PR-98's 6 is closed):
  1. NEW, STILL THE MOST USEFUL ONE: the real-time RATE of one record
     step. The caller question is answered (see the 0x523f0 block above:
     `call [[rec+0x44]+0xc]` only at 0x5240c, one direct caller 0x5371c)
     - what is open is how often 0x5371c runs. All 16 other references
     to 0x521f0 in the image are the sub-table slots listed above, so the
     step's rate is whatever drives that key-building path, and 0x5371c's
     enclosing function is where to read next. PR-100 read one step
     further - the 0x535a0 / 0x536f0 block below - and that is where
     PR-101 should start.)
     (PR-100's earlier draft of this list, written from grep hits in
     text.asm, gave the wrong addresses; these come from a raw word scan
     of the ELF.)
  2. NEW: a helper at **0x52360..0x523d0**, the other writer of the
     accumulator's zero (0x5236b). PR-100 first wrote "referenced
     nowhere" and that was wrong twice over: the entry is 0x52360 (a
     scan for 0x5235f/0x52361/0x52362 finds nothing, which is what made
     the block look dead - the same one-byte trap as 0x543a0), it sits
     in FOUR method tables at offset +0x1c (0xa25f0, 0xa2628, 0xa2eb8,
     0xa2ef0 - four of the sixteen record-class tables from item 1),
     which is the exact slot PR-77's record already listed in the table
     at 0xa25d4, and there are three direct calls at 0x4ff6b, 0x50003
     and 0x50043. Those three are setter-shaped stubs: `push ebx;
     mov ebx,eax; call 0x52360; mov edx,[ebx]; mov [edx+0xfa],imm32;
     ret` writing the vtable slot +0xfa with 0xffffffc9 / 0x46 / 0 (the
     third one wraps `call 0x52660` instead), and the first of them
     calls 0x56380 and 0x56310 on the global 0xcf068 first. So this is
     a record-class method, i.e. a state-change path, not dead code.
     The entry is confirmed by its own epilogue: the prologue at 0x52360
     is `push ebx; push ecx; mov ebx,eax` and 0x523ce-0x523d0 is
     `pop ecx; pop ebx; ret`. Its argument is edx, an index, and what it
     does is set the current row: if edx < 0 it returns without touching
     anything; else it calls 0x52070 - the setter that computes the word
     at [rec+0xc] from the words at [rec+0x8]/[rec+0xa] through the
     divide by 0x7fff at 0x76bcc, and whose only other caller is
     0x45a5a - zeroes [obj+0xa1], sets [obj+0x99] = 1, and then at
     0x52385/0x523a0 tests bits **0x10 and 0x20 of [rec+0x12]** - the
     control flags, and the only reading of those two bits found so far:
     bit 0x10's condition is `[[obj+8]+0x70]() >= 0` (a method of the
     owner) and bit 0x20's is `[obj+0x71] >= 0` (a signed field of the
     owner), each contributing 1 when negative; if exactly one of the
     two says negative, [rec+4] = [[rec+0x2e]+8] - edx - 1, otherwise
     [rec+4] = edx. That is a REVERSED ROW ORDER keyed on the owner's
     facing-ish sign, and SEQ_REV_X / SEQ_REV_Y (7 / 22 states) are the
     obvious names for those two bits - which would make them row-order
     flags and not movement modes at all. Worth confirming with the
     parser, and note this is also a fourth writer of the [obj+0xa1]
     zero and a writer of [obj+0x99] = 1.
  3. MOSTLY CLOSED: the handler `[[rec+0x44]+0x18]` that receives
     `acc - threshold` (0x52338 with ebx=0, 0x52334 with the remainder)
     IS **0x520c0**, the same function in all 16 record-class sub-tables
     and with no direct call anywhere in the image. What it does with the
     remainder: it saves ebx at [esp+0x14], takes the current node
     `[[rec+0x2e]] + [rec+4]*4`, calls the node's `+0x4` method, then a
     fixed table at 0x98278 slot +0x4, then `[[rec+0x44]+0x28]`
     (0x52470), then `[[obj+0x8]+0x4]`, and at **0x52118 writes edx
     into [owner+0xa1]** - the accumulator is REWRITTEN here, so the
     carry is real and lives in the object, not in the node; that is why
     a row advance does not zero it. After that it walks the node chain
     calling `[[node+0x4]+0x10]` with edx=6 and ebx = &[rec+4] and a
     counter at [esp+0xc], comparing to 6 at 0x52145. What remains open
     is that chain search. The node's `+0x10` method is called with
     edx = 6 here and with edx = 4 in 0x52470, and it gets ebx =
     &[rec+4] and a counter out-parameter at [esp+0xc], which 0x520c0
     then compares with 6 at 0x52145. What that mode code selects, and
     what the method writes through ebx, is open. Note 0x520c0 also
     tests the same pair of control bits as item 2 - `test byte
     [rec+0x12],0x10` at 0x52158 guarded by
     `[[obj+0x8]+0x70]() == -1`, and `[obj+0x71] == -1` at 0x5216c -
     so the SEQ_REV reading of those two bits is now supported by two
     independent methods, not one.
  4. THE KEY, AND WHAT IT IS. The current row node's key is fetched by
     `call [[node+0x4]+0x8]` into a 12-byte buffer plus a length byte at
     +0xc (0x5222c-0x52235); 0x495b0 answers "are all three dwords
     zero"; 0x49580 ORs two keys in place; 0x47e40 maps a key to an
     object and returns null when absent (0x522e6, with the record's
     scratch key and the object's own key at [obj+0x60]). What it is
     NOT, established here: an authored name. The shipped .MOV data has
     no key field at all - 0 of the 777 extracted .MOV files mention
     `key` and the .MOV parser's only token strings are `F`/`T`/`G` at
     0x92281/0x9228a/0x92293, with row directives `F`/`FC`/`I`/`IC` -
     and the only `key=value` grammar in this data set is the .DES
     placement properties of PR-67/PR-68 (`tools=`, `damage=`,
     `bounds=`, `scope=`, `energy=`). PR-100's draft cited that as
     "PR-54's `key=`", which is a wrong citation on two counts;
     corrected. So the 13-byte key is a runtime identity (three dwords
     plus a length byte), and whether it encodes a resource name - the
     `call 0x562c0(0xcf068, "deatd.mod")` at 0x4ffdd is the one thread
     pointing that way - or a (class, id, sub-id) triple is open.
     Reading the two key producers, not guessing, decides what the
     empty-key branch of 0x521f0 is for.
  5. THE TABLE MAP. 155 tables put 0x543a0 at +0xa0 and 31 put 0x239d0
     there - the same feed with no mode switch at all, `[obj+0xa1] +=
     [0xcef40 -> +0x388]` - and all of them carry the same two
     neighbours (0x23a40 = `mov eax,1; ret` at +0x9c, 0x239c0 = a bare
     `ret` at +0xa4). Which CLASSES those two groups are - who animates
     from a row threshold and who ignores the authored `control=` word
     entirely - is the useful map, and it needs walking the table region
     (0x96874..0xa65c0) back to its constructors, not a slot census: in
     one such table 0x54250 is at +0xc8 while +0x134 is a current-row
     accessor, so the same slot number means different methods per class.
  6. Unchanged from PR-98: 1, 3, 7, 8. Unchanged from PR-99: 9, 10.
  7. The ordering of the record step (0x521f0) and the merge (0x526df)
     inside one tick, which decides whether [obj+0x95] can ever be stale.
- TESTS, unchanged in kind: the decisive one is still a BY_X object at
  T != 40 outside ZARIA (PR-98's a, PR-99's d); a second useful one is a
  BY_TIME state whose authored threshold is NOT a multiple of 100 (there
  are 949 such rows in the shipped data, e.g. 120 x53, 240 x49) - that
  would say whether the 100-unit feed is really the unit for those.

## PR-101 — the row node and its class table, the key as an OR-lineage,
and the two methods that move a row, read end to end (RE only: no
behaviour change, no pin moved)

Worked PR-100's open items 2 (the node-chain channel codes), 3 (the
row-set method and its argument) and 5's tooling side, plus the
`+0x1c` argument tension the PR-100 record flagged. Method as before:
objdump of `/tmp/opencode/vis/text.bin` for `.text`, words and tables
from `build/leo/leo.elf` (file offset = VA - 0xf000), always starting
from an objdump-confirmed entry. New tool:
`tools/re/node_classes.py`, the class-table census.

- CORRECTION FIRST: PR-100 FLIPPED THE TWO BRANCHES OF THE STEP.
  0x52241-0x5224b is `test eax,eax; sete al; and eax,0xff; je 0x522fc`
  after `call 0x495b0` (verified: 0x495b2 returns 1 when all three key
  dwords are zero, 0 otherwise, 0x495bc-0x495d8). So `je` is taken
  exactly when the row node's key IS empty, and the branch at 0x522fc -
  the one that ORs the node's second chain value into the caller's key,
  runs the feed dispatcher `[[obj+0x8]+0xa0]` and compares
  `[node+0x10]` with `[obj+0xa1]` - is the EMPTY-KEY branch. A row that
  DOES carry a key takes 0x52251: OR the object's key into it, look the
  pair up with 0x47e40, and if it is not found do nothing at all.
  That was mislabelled the other way in PR-100's text ("an EMPTY key
  takes the branch at 0x52251"). It matters because it puts the feed
  accrual and the threshold test on the un-keyed rows and makes keyed
  rows conditional on the key match, which is the shape the port models
  as a keyed transition. The rest of PR-100's description of both
  branch bodies stands.
- THE ROW NODE. All four users take the current row the same way -
  `[[rec+0x2e]]` is a pointer to an array of node pointers, `[rec+4]`
  is the row index, `node = [array + row*4]` (0x521f0 at
  0x52221-0x52229, 0x52470 at 0x52473-0x5247b, 0x520c0 at
  0x520cf-0x520d7, and 0x523f0/0x52360 through the same pair). The
  node's own fields, from the uses:

      [node+0x00] the previous link of the chain this node hangs on
      [node+0x04] its class table (6 slots, see below)
      [node+0x08] value A
      [node+0x0c] value B
      [node+0x10] on a row node: the threshold, compared against the
                  accumulator at 0x52320-0x5232b (`mov eax,[node+0x10];
                  mov ebx,[obj+0xa1]; cmp eax,ebx`); on a chained
                  node: its id byte (0x45d6e `movsx eax,byte
                  [node+0x10]`, compared with the queried id)
      [node+0x18] an int: 0 in a record's row chain, the owner object
                  in the other chain (PR-98's 0x45270 pairs)

  So one authored row line is one node: its second number is the
  threshold the step compares, its first number is what an id-6 answer
  adds into the row index (0x45dc0 `add [out], [node+8]`), and its
  directives are the chain reached through [node+0x00]. READING, not a
  pin - the field roles come from those four uses, and no node image
  has ever been read back against the shipped `.MOV` text.
- THE CLASS TABLES, CENSUSED (`tools/re/node_classes.py`). 31 tables,
  6 slots each: a run of 30 at stride 0x18 from 0xa0f28 to 0xa11e0,
  plus one at 0xa2638 - which is exactly the tail of the record-class
  sub-table 0xa260c, the only one of PR-100's 16 sub-tables that holds
  this method set where the other 15 hold their own per-class
  functions. Over those 31:
    +0x08  0x45800 x30, 0x45970 x1    the chain's first value
    +0x0c  0x45870 x30, 0x45920 x1    the chain's second value
    +0x10  0x458e0 x28, and 3 classes answer for themselves:
           0x45d50, 0x45d90, 0x45dc0  the channel query
    +0x00  and +0x14  31 DIFFERENT functions each, one per class
  0x45800 and 0x45870 are walks: if [node+0x00] is non-null they ask
  THAT node for the value through its own +0x08/+0x0c, and at the end
  of a chain they return 13 zero bytes (`mov ecx,0xc; ... call 0x749a0`
  at 0x4581d-0x45831) - the 13-byte key shape again, three dwords plus
  a length byte at +0xc. The class at 0xa1000 shows the other slots in
  use: its +0x00 (0x457b0) unlinks the node (`mov [node],0`,
  `mov [node+4],0xa1000`) and calls the previous link's +0x00 with
  edx=2; its +0x04 (0x457e0) only walks; and 0x45750, another class's
  +0x04, writes `[node+8]` into `[ds:0xcef40 + 4]` - the level's own
  record object of PR-100 - and then walks. Reading: +0x08/+0x0c read
  the chain, +0x00/+0x04 write through it, +0x14 is a per-class
  accessor. Which authored property each of the 30 classes carries is
  open; that census is PR-102's job.
- THE KEY IS AN OR-LINEAGE OVER THAT CHAIN. The two exceptions in the
  +0x08/+0x0c census are the composition step: 0x45970 (the class at
  0xa11c8, slot +0x08) and 0x45920 (0xa11e0, slot +0x0c) call the
  chain getter 0x45870 and then do `lea edx,[node+8]; mov ecx,0xc;
  call 0x49580` and hand that back. 0x49580 is three dwords of OR in
  place (0x49585-0x4959a, `or edi,ebx` around a 12-byte loop), so the
  value is "what the chain gave me, with my own [node+8], [node+0x0c]
  and [node+0x10] OR-ed into it". Nothing stores a key: a key is the
  OR of the three dwords of the nodes on a chain, computed on demand,
  and 0x47e40 starts by switching on the length byte at [key+0xc]
  (`cmp al,3; ja far`, jump table at 0x47e30) so 1..3 dwords are
  significant. That closes PR-100's item 4: a key is a composed bit
  mask with a significant-dword count, not a name and not an authored
  field. The `deatd.mod` thread is off this path: 0x4ffb6 reports it
  only when an object's [obj+0x126] is 0 and the current record's row
  is 0 (0x4ffb4-0x4ffe7), which reads like a "nothing has run yet"
  diagnostic, not a key producer.
- THE CHANNEL QUERY, AND WHAT THE "MODE CODES" ARE. Slot +0x10 is
  `f(eax=node, ebx=out A, ecx=out B, edx=id)` and the four bodies are
  one mechanism:
      0x458e0  walk: if [node+0x00] non-null, visit it with the SAME
               id; this node contributes nothing
      0x45d50  walk, then if the node's own id byte [node+0x10]
               equals the queried id: `*A += [node+8]`,
               `*B += [node+0x0c]`
      0x45dc0  walk, then if the queried id is 6: `*A += [node+8]`,
               `*B = 6`
      0x45d90  walk, then if the queried id is 6: `*A = [node+8]`
               (assign, not add), `*B = 6`
  so out B is the id that answered and out A is the summed value: the
  chain is queried BY id and every node tagged with that id adds its
  pair. That unifies the numbers PR-98..PR-100 saw as separate mode
  codes: 0x52470 (the record sub-table's +0x28) asks the CURRENT row's
  node for id 4 into the caller's two words (0x5247e-0x5248b), 0x520c0
  asks for id 6 with out A = `&[rec+4]` - the row index itself
  (0x52124-0x5213d) - and 0x521f0/0x523f0 use the +0x08 getters for
  the key. The id space is one thing; which authored property ids
  1/2/3 (PR-98's merge selects), 4 and 6 name is still open, and the
  `[node+0x10]` overload (a threshold dword on a row node, an id byte
  on a chained node) is RE as two uses, not as one field.
- 0x520c0 END TO END, THE ROW-ADVANCE LAW. `f(eax=rec, ebx=remainder,
  edx=flags byte out)`; the argument registers of this convention are
  (eax=this, ebx, ecx, edx) and the fourth one is an out param here:
  * 0x520cf-0x520e1 takes the current row node and calls ITS +0x04
    with edx = the record (the write-through-the-chain family);
  * 0x520e4-0x52100 is an indirect call through a data table
    (`mov ecx,0x98278; call [ecx+4]`, with the owner's two table views
    in edx/ebx and 0x98278 itself pushed as an argument) - unread;
  * 0x520f9-0x52100 calls `[[rec+0x44]+0x28]` = 0x52470, the id-4
    query, into locals, and 0x52103-0x5210f feeds that pair to
    `[[obj+0x8]+0x04]` - the owner object's own +0x04;
  * 0x52112-0x52118 writes the remainder straight back: `mov
    [obj+0xa1], <ebx from the caller>` (PR-100's carry claim,
    confirmed);
  * 0x52124-0x52145 is the id-6 query with `lea ebx,[esi+4]` - out A
    IS `[rec+4]` - and `cmp [out B],6; je end`: if any node in the
    chain answers 6, the row index has just been moved by the authored
    value and the sequential path is skipped;
  * otherwise the sequential move, with the reversal test PR-99/PR-100
    saw a piece of (0x5214b-0x52193):
        A = ([rec+0x12] & 0x10) && ([[obj+0x8]+0x70]() < 0)
        B = ([rec+0x12] & 0x20) && ([[obj]+0x71] < 0)
        C = ([obj+0x99] == -1)
        if ((A ^ B ^ C) == 0) forward else reversed
  * forward (0x521b6-0x521d7): `n = [[rec+0x2e]+8]` - the row count,
    the field at rec+0x36 - and if `[rec+4] == n-1` then `or
    byte [flags],4` and `[rec+4] = 0`, else `[rec+4] += 1`;
  * reversed (0x5219a-0x521b1): if `[rec+4] == 0` then `or byte
    [flags],8` and `[rec+4] = n-1`, else `[rec+4] -= 1`.
  So the row list WRAPS, forward and back, and the wrap is reported to
  the caller as bit 2 / bit 3 of the byte buffer it passed in - which
  in 0x521f0 is the caller's 13-byte key buffer (0x52338 with edx kept
  from [esp+0x68]), i.e. "the list ended" arrives as key bits. The row
  count at rec+0x36 is now established from two independent places:
  this wrap test and 0x52360's mirror formula. PR-50's SEQ_REV_X /
  SEQ_REV_Y reading of control bits 4 and 5 is CONFIRMED as the
  row-order flags (PR-99's and PR-100's guard pair, plus the third
  term C here) and PR-96's correction that the bound is the record's
  own array length is confirmed too.
- 0x52360 END TO END, THE ROW-SET (the same law's other half).
  `f(eax=rec, edx=row index)`: 0x52364 calls the 0x52070 helper, then
  zeroes `[obj+0xa1]` (0x5236b) and writes 1 to `[obj+0x99]` (0x52377)
  before checking `test edx,edx; jl ret` - a negative index means
  "leave the row alone but still clear the accumulator". Then the same
  A/B pair as above (0x52385-0x523b8): equal -> `[rec+4] = edx`
  (0x523cb); different -> `[rec+4] = [[rec+0x2e]+8] - edx - 1`
  (0x523bc-0x523c5), the mirror index, exactly the inverse of the wrap
  above. The three direct callers are the per-class +0x1c wrappers:
  0x4fef0 (10 classes), 0x50000 and 0x50040 - all three forward edx
  unchanged. 0x50000 then writes `[[rec]+0xfa] = -55` (0x5000a) and
  0x50040 `= 70` (0x5004a) into the owner; the matching leave hooks
  0x50020/0x50060 (the `+0x20` slots of the same two classes) run
  0x52660 and write `[[rec]+0xfa] = 0`.
  0x4fef0 instead pokes the level record's `+0x3a0`/`+0x3a8` flags,
  calls `+0xe4` on the object at `[[0xcef40]+0x394]`,
  `[[owner+0x8]+0x4](edx=200)`, and 0x56380/0x56310 on the global at
  0xcf068. What the -55/70/0 codes mean is open.
- THE `+0x1c` ARGUMENT TENSION, DISSOLVED (the register convention,
  not a contradiction). The calling convention here is (eax = this,
  then ebx, ecx, edx). 0x536f0's body in order: `[[obj+0x8]+0x8c]`
  with a 13-byte out buffer -> 0x49580-OR into `[obj+0x60]`; 0x523f0
  (the current record's key) -> OR again; `[[obj+0x8]+0x54](ebx =
  &[esp+0x24])`; 0x53520 (edx = &[esp+0x20], ebx = &[esp+0x24]) which
  copies the object key and calls **0x52000**; `[[obj+0x8]+0x90]`;
  then `0x535a0(eax=obj, edx=[esp+0x20], ebx=[esp+0x24])`, and 0x535a2
  does `mov esi,edx` (an INDEX) and `mov edx,ebx` - so the value
  handed to the record's `+0x20` and `+0x1c` slots is an INT, not a key
  pointer. The key pointer `[obj+0x60]` is only ever passed to
  `[[obj+8]+0xac]` (0x535f3-0x535f7). Where the ints come from is the
  interesting part: 0x52000 scans a table of 21-byte entries at
  `[rec+0x1a]` (base at [rec+0x1a], count at [rec+0x22], stride 0x15)
  calling 0x47e40 with the object's combined key for each entry, and
  the first match writes `[out1] = [entry+0xd]` and `[out2] =
  [entry+0x11]` (0x5203c-0x5204d) - a (13-byte key, record index, row
  index) table. 0x535a2 then uses out1 as the record index
  (`[obj+0x50][out1*4]`, 0x535e1-0x535e7) and out2 as the row index:
  its same-index path is `cmp esi,[rec+4]; je ret; mov [rec+4],edx`
  (0x5360f-0x53616). So one matched entry means "go to record A, row
  B" - the state-transition table - and 0x52360's edx really is a row
  index. PR-100's flagged mismatch came from assuming ebx carried a key
  pointer at 0x535a2; it does not.
- 0x53520 / 0x535a2 AS A PAIR (the switch, and what it hooks). Leave
  half (0x53520): the record's `+0x20` hook is NOT called here - that
  is 0x535a2's job - and 0x53520 only fetches the key copy, runs the
  transition lookup and calls `[[obj+0x8]+0x90]`, then zeroes its
  local key copy. Switch half (0x535a2): if the index differs it calls
  the OLD record's `[[old_rec+0x44]+0x20]` with the int, zeroes
  `[obj+0x9d]`, sets `[obj+0x95] = 1` (0x535d1, PR-100's third
  setter), calls `[[obj+0x8]+0xa4]`, installs the new record, calls
  the NEW record's `+0x1c` with the same int, writes `[obj+0x5c]`,
  then `[[obj+0x8]+0xac](obj, obj+0x60)`. Both paths end at 0x53626:
  `[obj+0x9d] += 1`, and if the WORD at `[rec+0xc]` is non-zero and
  `<= [obj+0x9d]`, set bit 0x2 of the byte at [obj+0x61] - bit 9 of
  the object key's first dword. So the record carries a per-record
  counter limit at +0xc that publishes a key bit when the switch count
  passes it. Unread otherwise; it belongs with the transition table.
- WHAT THIS DOES NOT SETTLE (PR-101's open items):
   1. Which of the 30 classes in the run is which authored property,
      and what ids 1/2/3/4/6 name. The census tool lists the tables;
      tying a class to a `.MOV` directive needs a write: the `.MOV`
      row directives are `F`/`FC`/`I`/`IC` (0x92281/0x9228a/0x92293
      are the only token strings) and the parse side was never read
      against the node image. That is the next PR's first item.
   2. The transition table at [rec+0x1a]: who fills it, and what
      0x47e40's four length-byte cases actually compare (subset?
      exact?). It is the direct owner of every state change the port
      models, so it is worth its own PR.
   3. The `[obj+0xfa]` state ints (-55 / 70 / 0) and the
      `[[obj+0x8]+0x54]`, `+0x90`, `+0x8c`, `+0xac`, `+0xa8`, `+0x70`,
      `+0x04` owner hooks: all of them are slots of the object class,
      and none of them can be resolved without the class map - PR-100's
      item 5, unchanged and now the biggest one.
   4. The 0x98278-indirect call inside 0x520c0 (0x520e4-0x520f6).
   5. Whether the wrap bits (`or byte [key],4` / `8`) are the same
      bits the port's end-of-list handling means in `obj_transition`,
      and whether the `[rec+0xc]` limit + `[obj+0x9d]` counter is the
      "play this state N times" rule. Unmeasured; no pin.
   6. Unchanged from PR-100: items 1's cadence half (0x536f0's 31
      table slots vs the 0x550a0 handler tails), 7, and from PR-98/99:
      items 1, 3, 7, 8, 9, 10.
- TESTS, unchanged in kind: the decisive one is still a BY_X object at
  T != 40 outside ZARIA (PR-98's a, PR-99's d). New and cheap: any
  level whose `.MOV` has a row with a non-zero key would confirm the
  keyed-row branch of 0x521f0 is a real path in shipped data rather
  than only in engine code - the census tool counts row fields, not
  node images, so it cannot answer this; a node dump would.

## PR-102 — the record class pair: their parsers and constructors, the
transition table they fill, the `F=`/`T=`/`G=` fields, the state-name
table, and a mode-enum correction (RE only: no behaviour change, no
pin moved)

Works PR-101's open items 1 and 2 (the transition table's writer and
what 0x47e40 compares) and half of item 5. Method unchanged: objdump of
`/tmp/opencode/vis/text.bin` for `.text`, words/strings/tables from
`build/leo/leo.elf` (file offset = VA - 0xf000), every chain started
from an objdump-confirmed entry. Cross-checked against the 777 shipped
`.MOV` files in `/tmp/opencode/mov/`.

- THE WRITER OF `[rec+0x1a]` IS THE RECORD CLASS'S OWN PARSE METHOD.
  The class table PR-101 read at 0xa260c (its +0x0c is the row step
  0x521f0, its +0x18 the row handler 0x520c0) has **0x51910 at its
  +0x00 parse slot** - a real function start (`push ecx/esi/edi/ebp;
  mov ebp,esp; sub esp,0x38`), referenced from exactly one table slot.
  Inside it, 0x51e07..0x51e91 resolves a state name to an index
  (0x53440), reads a number (0x57d30/0x57db0/0x57fd0), builds an entry
  with `0x49610(eax=scratch, ebx=expression text, ecx=<that number>)`
  and pushes it into the vector that lives AT `[rec+0x1a]` - `[+0x1a]`
  base, `[+0x1e]` end, `[+0x22]` capacity, grow through 0x22010, then
  `rep movsd` of 5 dwords + `movsb` = exactly the 21-byte entry.
  0x49610 = `0x47d50` (the key-expression parser) followed by
  `mov [entry+0xd],<state index>` and `mov [entry+0x11],<the number>`;
  it is called from exactly one place, 0x51e4d, so the transition table
  is filled ONLY by the `.MOV` parse path. So PR-101's "record index at
  +0xd" is a **state index in the owner's `[obj+0x50]` array** and
  "+0x11" is a **row index into that state's row list** - the same pair
  of fields the engine itself uses for the current state and row.
  WITHDRAWN from the first draft of this record: "the parse slot is
  0x559c0 and it calls 0x51e07". 0x559c0 is a different slot of the
  same table (+0x08) and is a 13-byte key reset (`mov byte [edx+0xc],0`
  + `0x749a0` zeroing the other 12 bytes), not a parser and not a
  caller of 0x51e07.
- TWO RECORD CLASSES, THEIR SIZES, AND WHERE `F=`/`T=`/`G=` LIVE. The
  sub-table's +0x14 slot is 0x518f0: `operator new(0x48)` then
  0x517e0, a copy-ctor that copies the source's +0x00 (a pointer),
  +0x04 (the row index), +0x08/+0x0a (the `time` pair, WORDs), +0x0c
  (WORD), +0x0e (dword), +0x12 (the control dword), +0x16 (dword),
  copy-constructs the vector at +0x1a (0x22190 / 0x21750 reserve 10),
  initialises the vector at +0x2e, and ends with
  `mov DWORD PTR [eax+0x44],0xa260c` - the first hard proof that
  `[rec+0x44]` is that class table, and that the BASE record is 72
  bytes. The table PR-100 called 0xa25d4 "this record class's method
  table" is a DERIVED class: its +0x14 slot 0x52570 does
  `operator new(0x60)` then a ctor 0x52500 that calls the base copy-
  ctor 0x517e0 and writes `mov [eax+0x44],0xa25d4`, and its +0x00
  parse slot 0x52590 parses three one-character keywords with the
  format `' %i,%i'` - `F=` into `[rec+0x50]`/`[rec+0x54]`, `T=` into
  `[rec+0x58]`/`[rec+0x5c]` (each clamped: `cmp 0x82` / `mov 0x82` =
  the 130 cap PR-101 recorded), `G=` into `[rec+0x48]`/`[rec+0x4c]` -
  and then DELEGATES any other line to the base parser: 0x5264b is
  `mov edx,esi; mov eax,ecx; call 0x51910`. So `F=`/`T=`/`G=` are the
  derived record's fields at +0x48..+0x5c, which is what the port's
  `s->fx/fy`, `s->tx/ty`, `s->gx/gy` (ll_play.c) correspond to, and
  the two tables share one slot numbering (+0x0c is the row step
  0x521f0 in both, +0x18 the row handler 0x520c0 in both, the stubs
  0x23c90/0x23c80 at +0x04/+0x10 in both). Reading that region also
  shows why PR-101's node census flagged 0xa2638 and 0xa2650 as
  "stray" node tables: they fall INSIDE the record class table at
  0xa260c (its slots +0x2c..+0x44), and 0x457e0/0x45800/0x45870/
  0x458e0 are PR-101's node chain getters and walk query. Where the
  record class table ends and a node class table begins there is
  unresolved (open item 3).
- THE RECORD'S WHOLE BASE-CLASS VOCABULARY IS IN ONE FUNCTION, AND
  IT MAPS ONTO THE LAYOUT PR-101/PR-100 inferred. 0x51910 dispatches
  on the name and writes: `time` -> `"%hi"` at `[rec+0x8]` then
  `",%hi"` at `[rec+0xa]` (so `time=60,100` is two 16-bit halves);
  `sample` -> `"%d"` at `[rec+0x16]`; `time_scale` -> `"%d"` at
  `[rec+0xe]`; `seq` -> reads `%s`, resolves the name through the
  object at `[[0xcef40]+0x398]` (a vtable at +0x16, lookup slot +0x04
  - a name->index map) and stores the index in the global 0xceff4;
  `control` -> clears `[rec+0x12]` and ORs the tokens (PR-59/PR-99);
  `NAME ( number ) =` -> the transition entry above. There is also a
  branch at 0x51a4e keyed on the one-character name at 0x92146 (`s`)
  that has exactly the shape of a row line: it reads a token with
  `' %[^ ,]%*[ ,]'`, resolves it through the same name->index map,
  reads the next number with `"%d,"`, allocates 0x18 (0x5c320) and
  builds it with `0x515e0(eax=node, ebx=<that name's index>, ecx=<the
  number>, edx=<the stream>)` with 1000 pushed as a fifth argument,
  then pushes the POINTER into the vector at `[rec+0x2e]` - so that
  vector is {+0x2e base, +0x32 capacity, +0x36 count, +0x3a
  capacity-end} with 4-byte elements, grown through 0x21e80: PR-101's
  "[rec+0x36] = the row count" and 0x52360's mirror
  `[[rec+0x2e]+8]-edx-1` are both consistent with that (the count is
  an element count, not a pointer: 0x51ae3 does `*4` then `inc`, not
  `add 4`). What is NOT settled: the token is the single character at
  0x92146 = `s`, and NO shipped `.MOV` has a line starting with `s`
  followed by `=` (0 hits in 777 files), so this is not the branch the
  row lists really take; the reading of the plain `N,T` lines is open
  item 2. And `[rec+0x0c]`, the one WORD in the parsed range that
  neither parser writes, is still unexplained.
- ROW NODES ARE BUILT AT PARSE TIME, AND THAT IS WHERE `time_scale` IS
  APPLIED. 0x51eb0 is a second real function start (`push ecx/esi/edi/
  ebp; sub esp,0x10`) and the derived ctor 0x52500 calls it at 0x52568,
  after the base copy-ctor. It clears `[rec+0x0c]` (`mov WORD
  [eax+0xc],0` - so THIS is the only writer of the field both parsers
  leave alone), copies it to `[rec+0x0a]`/`[rec+0x08]`, computes
  `edi = rec + 0x2e` (the vector's head slot) and loops reading tokens
  with `0x9226f: " %[^[=,( ] %*[,=]"` and numbers with 0x57db0/0x57fd0.
  THE NODE ITSELF: the ctor 0x515e0 (`push esi/edi`, chains to 0x45770,
  `new(0x18)` at its clone 0x51622) writes `mov DWORD [node+0x4],
  0xa2638` - **0xa2638 is a row-node class table**, which is the answer
  to PR-101's "stray node tables at 0xa2638/0xa2650": those words are
  inside the record class table at 0xa260c (its slots +0x2c..+0x44) and
  the same numbers 0x457e0/0x45800/0x45870/0x458e0 are PR-101's node
  dtor, chain getters and walk query. Its fields: `+0x08`, `+0x0c`,
  `+0x14` zeroed and `[node+0x10] = 1` (the PR-101 "threshold or id
  byte" slot - it is THRESHOLD here), then it calls the reader 0x51660,
  which reads a number at `[node+0x10]` with the format at 0x92115 =
  `"%hi"` (a 16-bit integer: shipped T values are all small) and then
  SCALES IT: `imul edx, value` by the caller's `time_scale` argument and
  `idiv 0x3e8` - `node+0x10 = value * time_scale / 1000`. The s-branch
  passes the constant 1000 (a no-op scale); the row path is where a
  real `[rec+0xe]` reaches it. Then a three-way branch on `ecx` (the
  number read after the token): `> 0` -> `[node+0x08] = ecx`,
  `[node+0x0c] = ecx - 1`, `[node+0x14] = 0`; `== 0` -> `[node+0x08] =
  0x44110([[0xcef40]+0x398])` (a count of that name->index map) and
  the id is not stored; `< 0` -> `[node+0x14] = 1` and `[node+0x0c] =
  -(ecx + 1)`. So `[node+0x14]` is a signed-input flag and `[node+
  0x0c]` is an index offset by one, positive or negated.
- `time_scale` IS NOT AN UNMODELLED FIELD - IT IS A PARSE-TIME SCALE ON
  EVERY ROW THRESHOLD (this is the strongest lead yet on the PR-79/98/
  99/100 pacing tension). The shipped data: `KADMAR.07_BAL_KBALONIK.
  MOV` has `time_scale=2000` at line 19 followed by rows `1,200`,
  `2,200`, ... So the engine stores `200 * 2000 / 1000 = 400` as that
  row's threshold at `[node+0x10]`, which PR-101 showed is what
  0x52320 compares `[obj+0xa1]` against, and PR-100 showed that
  accumulator only ever grows by `[[0xcef40]+0x388]` = 100 per step:
  400 = 4 feed steps. With the 1000 default and `T=200` it is 2 steps.
  The census: `time_scale=` appears in 38 of 777 files, 120 lines, and
  EVERY value is a multiple of 100 (700 x20, 800 x16, 600 x16, 1000
  x14, 500, 2000, 1500, 650, 400, 1300, 1100, 900) - exactly the
  shape of a per-mille frame-rate multiplier. It does NOT change the
  cadence of the feed itself (that is still the unread step rate of
  0x521f0), it changes how many feed steps one row needs, i.e. rows
  per second. This port reads `time_scale=` into `s->field_other` (an
  unmodelled counter, ll_play.c line ~624) and treats `f->ms` as a
  duration directly (`o->t < f->ms`, ll_level.c:1136) - a 2.0x
  difference for that balloon's whole state. Nothing pinned: it is a
  reading of the code plus the data, and the test that would settle it
  (a state with a non-1000 `time_scale` measured against the port) is
  new to this PR.
- THE AUTHORED FORM, READ OFF THE PARSER. Before 0x51e07 the record
  parser matches a token and then `0x9225c: " ( %[^) ] %*[)] = "` -
  i.e. `NAME ( number ) =` - and the expression after the `=` is read
  with `0x9226f: " %[^[=,( ] %*[,=]"` and fed to the key parser. In
  the shipped data that is exactly a state line

      A_WAIT(1)=AC_BREAK_NEXT      2572 such lines in 777 .MOV files

  and its meaning is now fully determined: "when the key expression
  after `=` is raised, go to state A_WAIT starting at row 1". Census
  (`/tmp/opencode/trans_census.py`, scratch): the left-hand name is
  DIFFERENT from the enclosing state's name in 2447 of 2572 lines (so
  it is a target, not the owner), and 37 target states are entered at
  several different rows (A_DEAD: 1,26,30,...,97; A_JUMP: 1,4,6,...,14)
  - those numbers reach 97, which is the documented maximum frame count
  of one state, so they index rows and nothing else.
  What the port does here: `ll_mov_parse` already reads the number into
  `ll_mov_tr.entry` (PR-86) - and `obj_transition` then sets
  `o->fi = 0` and never uses it. So the port parses the start row and
  throws it away; the engine applies it thousands of times. RE
  observation plus a data census, not a behaviour claim: nothing here
  says the visible result differs.
- 0x47e40 IS THE KEY MATCH, AND ITS FOUR CASES ARE NOW EXACT (the
  diagnostic string it prints for a too-long key is `TEventSet::match
  :Bledna operacja` at 0x919ca, which names the subsystem). Its switch
  is on `[key+0xc]` (the significant-dword count), jump table at
  0x47e30:
      len 3 (0x47fb2): 13-byte compare - EXACT match
      len 2 (0x47f42): 0x49550 = in-place AND of 3 dwords, then a
                       13-byte compare of the AND result against the
                       requested key - SUBSET ("every bit I asked for
                       is set")
      len 1 (0x47edc): AND, then 0x495b0 (all-zero test) inverted -
                       NON-ZERO INTERSECTION ("any bit I asked for")
      len 0 (0x47e63): AND then the same 13-byte compare - the
                       degenerate case
      len > 3: prints the diagnostic and answers "no match"
  0x49550 (AND) is the sibling of PR-101's 0x49580 (OR); PR-101's
  "key = OR-lineage of the node chain" and this "match = AND test"
  are one bit-mask system. This closes PR-101 item 2's second half.
- THE STATE-NAME TABLE `[obj+0x3c]`, AND WHAT IT IS WIRED TO. 0x53440
  is a case-insensitive name search (`0x76a69`) over `[obj+0x3c]`
  (entry stride 8, count at `[obj+0x44]`), returning a 1-based index,
  0 for "not found" - it is what resolves the left-hand state name of a
  transition line. 0x53480 (a method of the object class) composes the
  object's key from two owner hooks (`[[obj+8]+0x84]`,
  `[[obj+8]+0x88]`), looks its result up by name in that same table,
  and stores a value at `[[obj+0x50] + state_index*4]` - and
  `[obj+0x50]` is the array the engine indexes with `[obj+0x5c]` to
  get the current state record (0x53711-0x53717, 0x543a6-0x543ac). So
  `[obj+0x50]` is indexed by STATE INDEX, confirming the same reading
  of entry+0xd. Who fills `[obj+0x3c]`/`[obj+0x50]` is unread - note
  that state names resolve through the OBJECT's table while `seq` names
  resolve through `[[0xcef40]+0x398]`: two different name spaces.
- CORRECTION TO THE PORT'S CONTROL VOCABULARY NOTE - THE MODE FIELD
  HAS FIVE VALUES, NOT THREE. The parser maps
  `BY_X` -> 0 (sets nothing; 0x51b58 jumps to the end),
  `BY_Y` -> mode 1 (0x51b6f), `BY_TIME` -> mode 2 (0x51b89),
  `BY_TX` -> mode 3 (0x51ba3), `BY_TY` -> mode 4 (0x51bbd); the mode
  is read by `mov eax,[rec+0x12]; and eax,7` with the jump table at
  0x54380: mode 0 -> 0x543c6 feeds `|v_x|` (=[obj+0xca]) into
  [obj+0xa1]; mode 1 -> 0x543dc feeds `|v_y|` (=[obj+0xce]) into
  [obj+0xa1]; mode 2 -> 0x543f2 is guarded by `cmp [obj+0xba],0; je
  out` and then `add [obj+0xa1], [[0xcef40]+0x388]` (PR-100's 100);
  mode 3 -> 0x543fb is the SAME body reached after passing that guard;
  mode 4 -> 0x5441b is a separate body, guarded by `cmp [obj+0xbe],0;
  je out`, adding that same 100. So the shipped data's `BY_TY` is not a
  harmless typo for BY_TIME: the engine gives it its own mode with a
  different guard, and `BY_Y` is a real mode feeding the vertical
  velocity. The data agrees with the engine, not with the port's note
  (ll_play.h "BY_TY is the data's own typo for BY_TIME"): BY_TY occurs
  in 7 files, BY_TX in none, and every BY_TY state is a climb/hang
  state of the lion (`[A_WSP_LIN]`, `[A_WSP]`, `[A_DRAB]` -
  ladder/rope), which fits a vertical feed. RE + a data census;
  nothing pinned.
- THE FEED GATES `[obj+0xba]` AND `[obj+0xbe]` ARE ASSIGNED, NOT
  DERIVED (this replaces the "meaning unread" hedge of the first
  draft). The method at 0x549f0 does the two assignments directly:
  `mov eax,[edx]; mov [obj+0xba],eax` (0x549f9) and
  `mov eax,[edx+4]; mov [obj+0xbe],eax` (0x54a08) - both fields come
  from a 2-dword structure the caller hands in, so their production is
  one level up (open item). The same function then re-decides
  `[obj+0xba]` when `[[obj+0xd2]+4] == 1`: it computes
  `flag = ([[obj+0xd2]+0xc] != [obj+0x6d])`, scales it by 15
  (`shl 4` + `sub`), and compares `[[obj+0xd2]+8] +/- 15*flag` against
  `[[[obj+8]+0x68](obj)]` with a +/- 5 margin, writing **+10**,
  **-10** or **0** into `[obj+0xba]` (0x54a57, 0x54a82, 0x54a93). The
  modes' use of it is only a zero test (see above), so what reaches
  the accumulator is the constant 100 either way - but the gate is a
  signed distance-to-something check with a 5-unit and 15-unit margin,
  which is a much better lead than "an unread scalar". What `[obj+0xd2]`
  is (its +4 kind field, +8 position, +0xc id) and who fills the
  caller's two dwords: open.
- THE CONTROL-TOKEN TABLE IS RE-DERIVED AND MATCHES THE PORT EXACTLY.
  The 22 token strings (0x9216e..0x9222f, listed in PR-99) OR these
  bits: SEQ_REV_X 0x10, SEQ_REV_Y 0x20, CHECK_ZW 0x80, CHECK_ANI
  0x40, CHECK_LN 0x100, CHECK_ALL 0x1c0 (= ANI|ZW|LN), CHECK_LEW
  0x200, CHECK_BASE_LN 0x4000, SOLID 0x400, CH_DIR 0x800, INV_V
  0x1000, DANGER 0x2000, CHECK_ALL_PLANE 0x8000, ANTI_SOLID 0x10000,
  SOLID2 0x20400, SEMI_SOLID 0x40400, ANTI_POCISK 0x80000, FRAME_REV
  0x8. PR-59's/PR-60's/PR-90's word values in ll_play.h are
  reproduced bit for bit by the token table this time read as tokens
  (the earlier reads were of the test sites), and the runtime tests
  agree: 0x527b3 `test byte [rec+0x13],0x2` (CHECK_LEW) / 0x527db
  0x40 (CHECK_ANI) / 0x52805 0x80 (CHECK_ZW), each passing the
  control word masked by 0x8000 to the list provider; 0x53b2e
  `test eax,0x10000` (ANTI_SOLID) and 0x53b3b `and eax,0x40400;
  cmp 0x40400` (SEMI_SOLID); 0x5528b/0x51019 `test ah,0x20` after
  `mov eax,[rec+0x12]` (DANGER). No discrepancy to fix.
- `;` AND `|` IN A CONDITION, CONFIRMED: the key parser 0x47d60 is
  "parse one comma-free token; if its first character is `*` set
  len=3, if `|` set len=2, if `!` set len=1, else len=2; resolve the
  token by name (`!` = the NEGATE node, `*` = AND-with-set, `|` =
  start another node, which is how the OR-of-AND-groups is built) or
  as a decimal bit number; while the next character is `,` loop" -
  so `,` folds into one node mask and `|` starts a new node, exactly
  the port's PR-87/PR-88 reading. The shipped data has 49 lines whose
  expression STARTS with `|` (`A_WALK(1)=|K_FWD,AC_BREAK_NEXT`),
  which the engine reads as an empty first node - an open question,
  not a bug claim.
- WHAT THIS DOES NOT SETTLE (PR-102's open items):
   1. Who fills `[obj+0x3c]` (the state-name table) and `[obj+0x50]`
      (the state array), and who calls 0x53480. That is the object
      class's construction path - PR-100's item 5, the class map,
      still the biggest open item. The record side is now mapped
      (0x518f0 factory, 0x517e0 copy-ctor, 0x51910 parse method,
      `[rec+0x44]` written with the class table); the object side is
      not, and the record's TEMPLATE stage that 0x517e0 copies from is
      part of the same question.
   2. Which token names a row line, and whether 0x51eb0's loop really
      builds the nodes. 0x51eb0 is the only other reader of the
      `[rec+0x2e]` vector and the derived ctor calls it, but its body
      was read only as far as the token loop's start; the branch that
      pushes into `[rec+0x2e]` is keyed on a token `s` that appears in
      no shipped file, and the row lists plainly exist in every file.
      Read the rest of 0x51eb0 (and 0x51a4e's role) in PR-103. Now
      answered at least partly: `[rec+0x0c]` is cleared by 0x51eb0
      itself.
   3. Which of the 30 node-class tables (PR-101's census) is which
      authored row property, and what the query ids 1/2/3/4/6 name.
      PR-102 identifies ONE of them for certain - 0xa2638, the row node
      - and explains where the "stray" ones sit (inside the record
      class table's slots +0x2c..+0x44). Also unresolved there: 0x515e0
      chains to 0x45770 (a base ctor, unread) and to 0x45df0 after the
      reader.
   3b. NEW: the actual step rate of 0x521f0 (how often one record step
      runs) is still the one number between "T * time_scale/1000
      compared against a 100-per-step accumulator" and a frame rate.
      With `time_scale=1000, T=200` the answer is "2 steps per row",
      so the whole pacing question is now exactly this one number.
   4. What the row-start numbers actually change on screen: the port
      parses them (PR-86) and ignores them. A video test would be an
      entry with a large N (A_DEAD(97), A_JUMP(14)) - unmeasured, no
      pin.
   5. Who hands 0x549f0 its 2-dword structure, and what `[obj+0xd2]`
      (+4 kind, +8 position, +0xc id) is; what
      `[[0xcef40]+0x398]+0x16`'s name->index map contains and where
      the global 0xceff4 (the resolved `seq` index) is used.
   6. Unchanged from PR-101: items 3 (the `[[obj+8]+0x54/+0x8c/+0x90/
      +0xac/+0xa8/+0x70/+0x04]` hooks, the `[[rec]+0xfa]` = -55/70/0
      codes), 4 (the 0x98278-indirect call inside 0x520c0), 5, 6; and
      from PR-98/99/100: the cadence question and items 1, 3, 7, 8, 9,
      10.
- TESTS: unchanged in kind (a BY_X object at T != 40 outside ZARIA is
  still the decisive one). Two cheap new ones from this PR: a climb
  state (BY_TY) measured against the port, which would test both the
  mode-4 guard and the vertical-feed reading; and an entry with a
  large row number (A_DEAD(97)), which would test whether the engine
  really starts a state mid-list.

## PR-103 — the object-class map: the engine's name factory, its
per-level class groups including the TLew* lion controllers, the exact
two-table pair every object writes, a per-class slot census, and the
parse-time state-name lookup (RE only: no behaviour change, no pin
moved)

Works PR-100 item 5 / PR-102 open item 1 - "the class map, still the
biggest open item". No behaviour change and no play pin moved. Method
as before: objdump of /tmp/opencode/vis/text.bin for `.text`, words and
strings from build/leo/leo.elf (VA = file offset + 0xf000, and a raw
byte search must start at file 0xf000 so .text-resident immediates are
not missed), cross-checked against the placement data in the ETAPY/
ANIMACJE containers. Tools left in /tmp/opencode: objfactory.py (the
name-to-class table), objfactory2.py (which literal is compared by
which chain), objcensus2.py (the class-table pair census), xref.py
(fixed this PR: a `\b` before a hex literal never matches inside
`0x...` because `x` is a word character - it now uses
`(?<![0-9a-f])...`, and that fix is what opened up this whole line of
work), class_usage.txt (the authored-data cross-check).

- THE FACTORY IS ONE FUNCTION AND ITS KEY IS THE AUTHORED NAME STRING.
  **0x4df10** (ends 0x4e9f1, `ret` at 0x4e9f1) has exactly two direct
  callers: **0x4aac3** (inside 0x4aaa2) and **0x4dcdc** (inside
  0x4dcc0). Both sites are the same shape: read one comma-delimited
  token with the scanf-style format at 0x91d1c / 0x91f2a =
  `' %[^,\r\n] , '` (0x57a40), and pass that token to the factory.
  Those are the readers of the port's placement grammar (ll_level.c:
  `"<LEAF>,<class>,<script>.mov,<x>,<y>"`) - the token is the line's
  first field, the one the port already calls LEAF/class. Inside the
  factory each candidate is one case block: `mov edx,<literal name>`;
  `call 0x76a69` (a per-character comparison that folds A-Z by
  `add 0x20` behind `cmp 0x41`/`cmp 0x5a` - case-INSENSITIVE equality,
  0 = equal); on match `mov eax,<size>; call 0x5c320` (operator new),
  a base ctor, then the pair write `mov [obj+8],T+8; mov [obj+0xc],T`
  and return. Sizes run 0x40..0x1bad, most 0x44..0x132. On no match
  the factory returns 0.
- THE CASCADE. The factory first tries a GENERIC chain of 31 names
  (literal block 0x91f3d..0x92018, in code order): ANITAB PLANT
  GRABOBJ DEMOTEXT ANIBACK MOVEPLAT MOVERECT RECT DRECT DRECT2
  KATPLAT DZWIGNIA WODOSPAD ZWIERZ GRUPA PICT POCISK IDPOCISK GRABG
  GWIAZDKI NIESP ZONE KORONA SURPRISE BAL DNGANI TRASA TRASA2 LINA
  TABLICA BERLO. After it fails, 0x4e971..0x4e9e7 CALL eleven
  per-level sub-factories in sequence, returning the first non-null
  result: 0x1f870 (ZARIA), 0x24f60 (ENA), 0x26860 (ENA2), 0x29090
  (KADMAR), 0x2ba90 (KADMAR2), 0x2ea00 (SADES), 0x33370 (RAN),
  0x34c20 (KARA), 0x3a250 (TUNGA1), 0x3acf0 (TUNGA2), 0x383c0 (LONA)
  - the last call's result is returned without a test. A separate
  stub, 0x4e9fe (no direct caller found; open item 4), chains three
  more second segments: 0x38a60, 0x29b30, 0x3a4a0.
- THE CLASS VOCABULARY IS ORGANISED BY LEVEL, AND EVERY LEVEL GROUP
  ENDS WITH ITS LION. Each sub-factory's compared literals sit in one
  contiguous .data block, and each block is interleaved with that
  level's model file names (looser.mod, ena2ed.mod, kadmdead.mod,
  k2dead.mod, sadesdea.mod, ran1.mod, karade.mod, lonade.mod,
  deatd.mod) and with state names (A_DEAD, A_LOOKUP, A_LOOKDOWN)
  compared in the same code - per-level script logic. Captured names
  per chain (from objfactory2.py; the exact class the name builds,
  with its object size, ctor hub and table pair, is in
  /tmp/opencode/objfactory.out):
  - ZARIA (0x1f86f..): SEP2 DRABINKA RYBA POKAZ STRUMIEN PIANA SPLYW
    ZWODOSPD KAMIEN KATAPULT KILOF_PL PALMA WODA PLANT ZARROW
    CROCODIL BRANCH TREE JBRANCH JROPE ZTABL **TLewZaria TLewZariaB**
  - ENA (0x24f60..): PLANT PERLOPL KOLC PIRANIA RMLOT REKIN RPILA
    RYBKA KRAB ROZGW OLEJ RBLOK KOTWICA SKALA SKALA_A GLAZ KROPLA
    KROPLAS PILAGEN PILARYB KONIK **TLewEna**
  - ENA2 (0x26860..): E2BOSS ECOIN EBECZKA **TLewEna2**
  - KADMAR (0x29090..): LISC PLATF SCHODY SCHODY2 SCHODY3 SCHODY4
    KPALMA CIUCHCIA BRECT BICZRECT WSPRECT PIASEK KOLUMNA NINJA WAZ
    KOBRA PTAK BOSS CAMEL MALPA MALPA2 MALPA4 ZOLW POCISK2 SMIGLO
    **TLewKadmar1**
  - KADMAR2 (0x2ba90..): SWIATLO LEW_SWT OBJ_SWT OBRECZ GWIZADKI
    KRZYZYK WINDA SCHODY FARAON MACH_A MACH_B OK ODBIJACZ PODMUCH
    **TLewKadmar2**
  - SADES (0x2ea00..): TGA_OBJ KALAMARN KONIKI KRABY MEWA MURENA
    OSMIORNI PERLOPLA ROZ_KOLC SLIMAK STULBIA STULB_ST PURCHAWA
    PILA ARMATA WULKAN STONE BUTLA GWIAZDKA KOREK TAB_KOT TAB_KOLO
    BRAMA BRAMA_B BUTLA_BL BUTLA_BK SIATKA WYBUCH ROBALE
    **TLewSades**
  - RAN (0x33370..): GEN_RAN GPTAK_RA PIOR_RAN SEP_RAN POCISK
    BAL_RAN **TLewRan**
  - KARA (0x34c20..): KSOWA KZABA SZCZURK SZCZURS KSMOK KLINA
    KMIOTLA KDRABINA KDYBY KWINDA KBLOK KGARNEK KMAG KPAJECZ KKULA
    KTRASA KBLOKZ KBOMBA KMASKA **TLewKara**
  - LONA (0x383c0..0x38a1f + the second segment at 0x38a60): SOPEL
    FOKA YETI CEBRZYK PINGWIN LONAKULA IGLO GRABOBJL1 GRABOBJL2
    WODOSPAD2 KRA KRA0 KRA1 KRA2 KRY SOUNDSOURCE SKOCZNIE SKJUMP
    FLAGA TABLICZKALONA (+ **TLewLona**, literal at 0x912cf; its
    comparison site was not captured by the scan)
  - TUNGA1 (0x3a250..): TUNTRAS KLODA GUARD TMPLAT XCHG
    **TLewTunga1**
  - TUNGA2/JUNCTION (0x3acf0..): **TLewJunction** TGA_OBJ PTAKPLAT
    GEJZER TWINDA PANTERA GORYL DOMEK DRAB KALUZA ZAPADNIA SPAD
    **TLewTunga2**
  The class names spell the authored content in Polish (ARMATA =
  cannon, BRAMA = gate, BUTLA = bottle, KLODA = log, LINA = rope,
  ZOLW = turtle, STRUMIEN = stream, WODOSPAD = waterfall, PANTERA,
  CROCODIL, ...), and `lew` = lion: the TLew<level> classes - one per
  stage plus TLewJunction - are the lion/script-controller object, and
  the level `.des` grammar has a `lew=` key the port already parses.
  The engine's lion is a C++ class per level, not script data; the
  `.mod` names next to each group are the open question of what model
  data those classes load.
- CENSUS NUMBERS. objcensus2.py (the pair-write census over the whole
  of .text): 394 sites write an ([obj+8], [obj+0xc]) class-table pair,
  163 distinct pairs. objfactory.py matched 122 of those sites to a
  preceding name test - 107 distinct class names - the other sites are
  clone/internal paths (for example 0x1ece0, the one caller of which,
  0x1edb0, runs only when `[obj+0x5c] == 3` and the state
  descriptor's `[+4]` is 0 or 6, guards with a counter at [obj+0x124],
  and builds the 0xf2 object by copy-constructing [obj+0x120] through
  0x54030 - the same method also writes [obj+0x120] from
  0x4dcc0(global, name-string) at 0x1ed90).
- THE AUTHORED-DATA CROSS-CHECK (class_usage.txt): 87 of the 107
  factory names appear in the shipped containers as ANIMACJE leaf
  names (69) and/or as placement first-fields - the same token does
  both jobs, which is consistent with 0x4dcc0 using the captured
  token both as the class key and for an ANIMACJE path. Placement
  counts agree with the game: ANIBACK 761, ZWIERZ 191, POCISK 123,
  DNGANI 78, BAL 52, SURPRISE 50, MOVEPLAT 48... Of the 154 distinct
  placement first-fields in ETAPY, 131 are hit by the compared
  vocabulary (12 of them only after case-folding, e.g. `plant` vs
  the literal `PLANT`, which the case-insensitive comparison accepts).
  Eleven names resolve in neither place: GLON, ROZGWIAA/B/C and
  STULB_D have no literal anywhere in the image, and SEP, PARROT,
  SNAKE, LICHA, PAJAK, NOSOROG sit in an unmapped literal block at
  0x9320b..0x93230 whose only compared site found is 0x616a4 (open
  item 3).
- THE PAIR LAYOUT, EXACTLY. 157 of the 163 pairs satisfy
  `[obj+8] = [obj+0xc] + 8` to the byte - one method list, two entry
  points 2 slots apart (two base-class headers over one method array),
  which is why PR-99's `[[obj+8]+off]` offsets and the base-table
  slot numbering differ by exactly 8 bytes. The exceptions: two pairs
  whose `[obj+0xc]` is not a table but a scalar (0x98278 written with
  `[obj+0xc]` = 0 at 0x4fbdd/0x52885/0x5ec2a and = 1 at 0x5327d -
  PR-101's 0x98278 indirect call; and 0xa26c8 with `[obj+0xc]` = 0xff
  at 0x56641/0x5667c); and three genuine multiple-inheritance cases
  where the two tables are far apart: 0x99828/0x997fc and
  0x9a778/0x9a74c (delta 0x2c) and 0xa0780/0xa076c (delta 0x14).
- SLOT MEANINGS ARE PER CLASS - THAT IS WHAT THE CLASS MAP SAYS.
  Scanning 0x543a0 (PR-99's frame-advance dispatcher) across the 163
  pairs: it sits at `[[obj+8]+0xa0]` in 118 of them and nowhere near
  that in the rest. And PR-100's open question - "0x536f0 has 31
  method-table slots... the offset it is dispatched through is not one
  of the obvious ones" - has its answer for at least 24 classes:
  0x536f0 sits at **[[obj+8]+0x14]** in those tables (and at v8+0xcc,
  +0xf4, +0x130... in others). The 0x9686c family that PR-99/100
  measured through +0xa4/+0xac/+0xa8 is a different family: at
  [obj+8] = 0x96874 its slot +0x14 is 0x3d8f0. The port's
  `[[obj+8]+0x54/+0x8c/+0x90/+0xac/+0xa8/+0x70/+0x04]` hook offsets
  are therefore properties of the measured classes, not of all
  objects - the class map is exactly the table of which class answers
  which method at which slot.
- THE CONSTRUCTOR CHAIN AND 1-BASED STATE INDICES. The ctor hubs are
  chained: 0x3d8c0 -> 0x53ff0 -> 0x531f0 -> 0x52860 (the last is the
  0x44/0x48 GRUPA/GRABOBJ base). Inside 0x531f0: `[obj+0x50] = 0`
  (the record array, PR-77) and **`[obj+0x5c] = 1`** - the current
  state/record index starts at 1, which confirms 0x53440's 1-based
  returns are the same index space. The same ctor chain initialises
  the [obj+0x3c] name vector through 0x21e30, but nothing found so
  far appends entries to it (open item 2).
- THE STATE-NAME LOOKUP HAPPENS AT PARSE TIME, AND TWO PR-102
  DETAILS ARE CORRECTED. Inside the transition-entry builder 0x51e07
  there is `call 0x53440` at 0x51e0b: the authored `TARGET` name is
  resolved to a state index against the record owner's `[obj+0x3c]`
  table while the .MOV is parsed, and the index goes to the entry
  ctor 0x49610 as ebx (entry+0xd). The `( N )` number: 0x51e33..
  0x51e3e parse it, subtract 1, and yield -1 when the `( N )` part is
  absent - so **entry+0x11 = N-1 or -1**, not N as PR-102 recorded.
  0x53440's own body (0x53440..0x53477) walks [obj+0x3c] with stride
  8, compares with 0x76a69 (so state names are matched
  case-insensitively too), returns the 1-based position or 0. Its
  siblings: 0x53410 is the current-state getter (answers `[obj+0x5c]`
  into one out-pointer; when the other argument is -1 it answers
  `[[obj+0x50] + [obj+0x5c]*4] + 4` - the `[rec+4]` row index of the
  current record); 0x53480 starts with `call [[obj+8]+0x84]` and then
  re-enters 0x53440 (partially read, open item 2); and 0x3fe10 shows
  the cross-object form - `[obj+0x26]` reaches another object's
  state machine, looks its state up by name, allocates a node with
  `new(0x18)` and hands `[[other+0x50] + state*4]` to 0x3fa20.
- WHAT THIS DOES NOT SETTLE (PR-103's open items):
  1. Who calls the placement readers 0x4aaa2 and 0x4dcc0 upstream, and
     which authored file each one is reading (the .des section
     keywords, `tlo/zwierzaki/animacje/ani_plat/platformy/...`, all
     sit in the same data block - the per-section mapping is
     unread). 0x4dcc0 also calls `[[class table]+0x7c]` with the
     parsed x,y after construction - that method is unread.
  2. Who APPENDS to [obj+0x3c] (the state-name table) and who fills
     [obj+0x50] (the record array) - the ctor chain only initialises
     them; 0x53480 is a partial read; PR-102 item 1's original
     question is answered only halfway (we now know which object
     classes exist, not how their state tables get built).
  3. The 11 unresolved placement names (GLON, ROZGWIAA/B/C, STULB_D
     have no literal anywhere) and the unmapped 0x9320b literal block
     compared at 0x616a4.
  4. The 0x4e9fe stub with its three second-segment chains and no
     direct caller, and the classes reachable only there.
  5. What any of the per-class methods DO - the map now names 163
     classes but the method semantics are per-class work; TLew* and
     the `.mod` model files are the natural next target (the port's
     per-level lion behaviour is what those classes implement).
  6. The [obj+0xc]-is-a-scalar classes (0x98278 with 0/1, 0xa26c8
     with 0xff) and the three wide-pair multiple-inheritance cases.
- Docs: this record in NEXT_STEPS.md, mirrored into HANDOFF.md
  section 7 item 13 and the section 8 open-item paragraph; the
  engine-comparison comments corrected at the placement-grammar header
  comment in ported/ll_level.c (the first field is the factory's
  class key, case-insensitive) and at ported/ll_level.c
  obj_transition (entry+0x11 is 0-based / -1; the target name is
  resolved at parse time through the case-insensitive 0x53440). No
  behaviour change, no pin moved.

## PR-104 — the TLew* lion classes: all thirteen factory cases and
their class tables, the per-level death-theme methods that play a
hard-coded .MOD through the music singleton (the class map supplies
music data the authored grammar never names), the base-lion ctor and
its default state-entry hook, and a correction to PR-103's pair
census (RE only: no behaviour change, no pin moved)

Works PR-103's open item 5 ("what the per-class methods DO - TLew*
and the `.mod` names are the natural next target"). No behaviour
change and no play pin moved. Method as before: objdump of
/tmp/opencode/vis/text.bin, words and strings from build/leo/leo.elf,
authored data from the ETAPY/ANIMACJE/SOUND containers. Tools: the
PR-103 set (objfactory.py, objfactory2.py, objcensus2.py, xref.py)
plus objslot.py (finds every image dword equal to a code address and
annotates which measured class-table pair and slot owns it).

- THE THIRTEEN LION CLASSES, EVERY FACTORY CASE. All thirteen
  TLew<level> literals exist in .data (0x9078b..0x91391, each in the
  same packed block as that level's `.mod` name, its class name and
  its A_DEAD/A_LOOKUP state names - the literals really are packed
  back to back, which is also why `KARADE.MOD` at 0x90ff0 is the
  tail of the string `TLewKaraDE...` and `RAN1.MOD` at 0x90e70 the
  tail of `TLewRanPOCISK...` read from a different start). The cases
  are at 0x1fef4 ZARIA, 0x1ff26 ZARIA B, 0x25510 ENA, 0x269a1 ENA2,
  0x29b01 KADMAR, 0x2bfa1 KADMAR2, 0x2f0f0 SADES, 0x33501 RAN,
  0x35201 KARA, 0x38a2f LONA, 0x3a434 TUNGA1, 0x3a466 JUNCTION,
  0x3b14f TUNGA2 - five of them (ENA, SADES, LONA, KADMAR's tail and
  TUNGA2) sit at a non-instruction-boundary in the objdump listing
  and were missed by PR-103's line scan - PR-103 NAMED all thirteen
  classes but recorded a construction site and table pair for only
  eight of them. Sizes 0x12a..0x162; ctors
  0x1e0a0 (both ZARIA), 0x24110 ENA, 0x500b0 (the SHARED base ctor:
  ENA2, KADMAR2, KARA, TUNGA1, JUNCTION), 0x27030 KADMAR, 0x2df60
  SADES, 0x31ff0 RAN, 0x36390 LONA, 0x3b180 TUNGA2.
- THE CLASS TABLES (name = the pair the ctor or the case writes):
  TLewZaria 0x96960/0x96958 (over the base 0x97970/0x97968 that its
  ctor 0x1e0a0 sets), TLewZariaB 0x97a98/0x97a90, TLewEna
  0x988ec/0x988e4 (written inside 0x24110 at `[this-0x12e]/-0x12a`,
  size 0x14a), TLewEna2 0x98f34/0x98f2c, TLewKadmar1 0x9a54c/0x9a544
  (inside 0x27030, with `[obj+0xf6]=0x5dc`), TLewKadmar2
  0x9b4d4/0x9b4cc, TLewSades 0x9c958/0x9c950 (inside 0x2df60 at
  `[this-0x1ae]/-0x1aa`, size 0x1ee), TLewRan 0x9d338/0x9d330 (the
  ctor ALSO writes 0x9d338 twice, at 0x31ff5 and 0x3204d), TLewKara
  0x9e134/0x9e12c, TLewLona 0x9f380/0x9f378 (also 0x9f298/0x9f290
  through copy paths 0x39b23/0x39bad/0x73e55), TLewTunga1
  0x9f668/0x9f660, TLewJunction 0x9f540/0x9f538, TLewTunga2
  0xa0624/0xa061c (inside 0x3b180 at `[this-0x13a]/-0x136`, size
  0x162).
- THE SHARED BASE-LION CLASS. All of ENA2/KADMAR2/KARA/TUNGA1/
  JUNCTION (and through their own ctors, ENA/SADES/TUNGA2) chain to
  one ctor 0x500b0 = base chain 0x53ff0 + the lion fields
  `[+0xee]=0 [+0xf2]=1 [+0xf6]=0x7d0 [+0xfa]=0 [+0xfe]=0 [+0x102]=0
  [+0x106]=1 [+0x112]=0 [+0x11a]=0 [+0x11e]=0 [+0x10a]=0` and the
  pair `0xa2218/0xa2210`. The ZARIA pair uses a different base
  chain, 0x1e0a0 = 0x500b0 + `[obj+0x12a]=0` + a `dl` byte-preserving
  store at [obj+0xb6] + `[obj+0xad]=0x1f4` + pair 0x97970/0x97968 -
  so there are exactly two lion bases, and the per-level classes sit
  on one of them. Field defaults note: `[obj+0xf6]` (PR-102 found a
  row-node field set to the same 0x2710/0x5dc/0x7d0 family) is set to
  2000 by the base, 500 (0x1f4 at [obj+0xad]) by the ZARIA base and
  1500 (0x5dc) by TLewKadmar1 - per-level tuning numbers carried IN
  THE CLASS, not in the data.
- THE DEATH-THEME METHODS - THE MAIN FINDING, AND A DATA-SOURCE ONE.
  Ten methods, all the same body: read `[[[obj+0x50]+[obj+0x5c]*4]
  +4]` - the CURRENT STATE RECORD's row index (the same field
  0x53410 answers and PR-102's `TARGET(N)` writes) - and if it is 0,
  and global 0xcf074 is 0, `mov eax,0xcf068; mov edx,<"NAME.MOD">;
  call 0x562e0` (once 0x562c0, which first copies `[obj+0x3c]`);
  then tail-call 0x52750, the base version of the same method.
  0x562e0/0x562c0 are the music object's play methods on the
  singleton at 0xcf068 - the port already anchors that object (its
  ll_level.c comment "Engine anchor: FUN_0005dec0 opens+plays
  mapka.mod for the MAPKA screen"), and 0x56010 appends the
  extension literal `.mod` (0x922e2) and reads the file;
  0x56270/0x56310/0x56330 start/restart playback.
  The method and its .MOD, per level: 0x23f96 LOOSER.MOD (ZARIA,
  owner table unlocated - open item 4), 0x26470 ENA2ED.MOD (at
  [[TLewEna's second base 0x9916c]+0x1d4] and [[0x99258]+0xe8]),
  0x26fd0 KADMDEAD.MOD ([[0x9a674]+0x1f0] and [[0x9a778]+0xec]),
  0x2c360 K2DEAD.MOD ([[TLewKadmar2 0x9b4d4]+0x128]), 0x2defe
  SADESDEA.MOD, 0x31fa0 RAN1.MOD ([[TLewRan's second base
  0x9d460]+0xe4]), 0x33c60 KARADE.MOD ([[TLewKara 0x9e134]+0x128]),
  0x361e4 LONA lonade.mod (in the TLewLona pair's neighbourhood:
  [[0x9f298]+0x1d0]/[[0x9f380]+0xe8]), 0x4ffb0 DEATD.MOD - the
  DEFAULT version, which also requires `[[obj]+0x126] == 0`, at
  [[0x97bc0]+0xe4] - and 0x97bc0 is NOT a lion table (108 of its
  first 144 slots differ from the base-lion table; it is built by
  0x22fc3/0x3ff70/0x53a30/0x1e0a0/0x3fb80/0x36390 and re-typed at
  0x202f5/0x62e59, so its identity stays open). The .MOD names
  themselves all exist as SOUND-container records (LOOSER.MOD,
  ENA2ED.MOD, KADMDEAD.MOD, K2DEAD.MOD, SADESDEA.MOD, RAN1.MOD,
  KARADE.MOD, MAPKA.MOD, LEW.MOD, PIRAMID.MOD, PODWODA.MOD,
  RAN.MOD, KARA.MOD, LONA.MOD, KADMAR2.MOD, ENA2.MOD; the container
  spells them upper-case, and a `strings` read of its index shows
  one-`~`/`n`-prefixed forms like `~RAN1.MOD` and `nLONADE.MOD`).
  WHY THIS MATTERS FOR THE PORT: the level `.des` grammar names only
  the level tune (`muzyka=`), which the port plays; these death
  themes appear NOWHERE in the authored files - they are hard-coded
  in the lion classes, so the class map is the only source for them,
  and no data-only port can find them. (The SOUND container keeps
  them upper-case: `LOOSER.MOD`, `KARADE.MOD`, `RAN1.MOD`...) The
  guard makes the hook a
  "the state machine has arrived at row 0 of the current state"
  event; the default version of the slot is 0x50300, and it sits at
  **[[obj+8]+0xe4] in every lion table measured** (TLewZaria, Ena,
  Ena2, Kadmar2, Sades, Tunga1, Junction, ZariaB, Tunga2 and both
  lion bases 0xa2218 and 0x97970; TLewLona's copy answers it at
  +0x2a4). Per-level overrides at other slots (+0x128 for Kadmar2
  and Kara, +0x1d4/+0x1f0/+0xe4 in the second-base tables, +0x20c
  for ZariaB/Ran/0x97970) - so, exactly as PR-103 said, the slot
  number is a property of the class, and only the +0xe4 default is
  common.
- 0x52750 IS THE BASE VERSION OF THE HOOK: it brackets work with
  three calls through the object's attribute set `[obj+0x44]`
  (`call [[obj+0x44]+0x2c]`, `+0x30`, `+0x34`, all with a stack
  buffer) and between them `call [[[obj]+8]+0xc8]` - the
  [[0xcef40]-style owner/level object's own hook at +0xc8]. The
  KADMAR and LONA versions additionally write
  `[[0xcef40]+0x394]+0x106 = 1` - the same field the base lion ctor
  initialises to 1, reached through a pointer the level global keeps
  at +0x394 (its sibling +0x398 is the ANIMACJE name->index object) -
  consistent with "re-arm the lion object"; and the generic 0x4ffb0
  version is gated by `[[obj]+0x126] == 0` (a field in the 0xee..
  0x12a block the base ctor initialises) - the meaning of both flags
  is open item 1.
- CORRECTION TO PR-103'S CENSUS METHOD (not to its findings): the
  163-pair count came from scanning ONLY the shapes
  `mov [reg+0x8],T; mov [reg+0xc],T`. A wider scan of every write of
  a 0x96000..0xb0000 immediate inside .text finds 1982 sites, and the
  lion classes above show why the narrow shape under-counts: a ctor
  whose `this` is not the object base writes its pair at the object's
  real offset relative to ITS register (`[eax-0x12e]` for TLewEna =
  `[obj+0x1c]`, `[eax-0x1ae]` for TLewSades = `[obj+0x3c]`,
  `[eax-0x13a]` for TLewTunga2 = `[obj+0x0]`). So the class count is
  higher than 163, and PR-103's "[obj+0xc] is a scalar 0/1/0xff"
  exception class deserves the same re-read: the same tool ALSO shows
  0x98278 and 0x98274 written as a MEMBER at many offsets ([eax-0x4],
  [edx+0x8], [esp+0xa8], [obj+0x4c]...), which fits PR-101's
  0x98278-indirect call being an embedded object referenced by
  several classes rather than a class table. (The 1982 number also
  includes PR-101/102's known non-vptr table writes - the record
  class at [rec+0x44], node classes at [rec+0x2e]-range slots, the
  0x961fc/0x96200 sentinel pairs - so it is a census of table-
  immediate writes, not of vptr pairs.)
- PR-103 OPEN ITEM 4 IS CLOSED. The 0x4e9fe stub's three callees -
  0x38a60, 0x29b30, 0x3a4a0 - are not factory chains: 0x3a4a0 is
  `mov [0xcee24],0; ret`, 0x67e60 (the KADMAR case's tail call at
  0x29b2b) is `mov [0xcf4ec],0; ret`, 0x36650 zeros 0xced10/0xced14,
  0x37af0 zeroes a 7-dword array at 0xcedf0; 0x38a60 chains those
  level-state zeroing helpers. No raw dword anywhere in the image
  equals 0x4e9fe or those callees outside their own call sites, so
  the stub is an unreferenced group of per-level global-state
  resetters - dead or reached through an unread pointer; either way
  it names no classes.
- WHAT THIS DOES NOT SETTLE (PR-104's open items):
  1. The flags: `[[obj]+0x126]` (gates the default death hook),
     global 0xcf074 (gates all of them), `[[0xcef40]+0x394]` (the
     lion pointer) and the re-armed `+0x106`.
  2. The exact semantics of the hook slot: "row index == 0" is
     measured, but WHY row 0 (state-entry? a mid-state start-row
     suppressing the theme - PR-102 showed `TARGET(N)` can start a
     state at row N) is inference, and 0x50300 (the default) has not
     been read.
  3. What any OTHER lion method does - only this one slot family has
     been read; the lion classes' move/camera/script logic (the
     `A_LOOKUP`/`A_LOOKDOWN` compares near those literals) is still
     the open work.
  4. The owner table of the ZARIA LOOSER.MOD method 0x23f96 (found
     in no measured pair up to +0x300), and the identity of class
     0x97bc0 (its +0xe4 is the DEATD version; it is constructed by
     six different ctors and re-typed at 0x202f5/0x62e59).
  5. The MI layouts: why TLewEna's pair sits at [obj+0x1c] with the
     base subobject pointer at +0x14, and where TLewKadmar1's and
     TLewKara's second-base tables are.
  6. Whether the port should play these themes: that is a FEATURE
     question (the data does not name them; only this RE does), so
     it is recorded as an option, not pinned.
- Docs: this record in NEXT_STEPS.md; mirrored into HANDOFF.md §7
  item 13 and the §8 class-map paragraph; one engine-comparison
  comment added at the `muzyka=` block in ported/ll_level.c (the
  engine also plays hard-coded per-level death themes through the
  same music object - the class map, not the grammar). No behaviour
  change, no pin moved.

## PR-105 — the state-step chain closed end to end: the unique driver
of the record step, the object virtual slot that IS one state-machine
step, the message pump that steps every object, the lion controller
chain, and five corrections to PR-104's placement of the death-theme
methods (RE only: no behaviour change, no pin moved)

Works PR-100's still-open cadence question ("what rate does the step
at 0x521f0 run at") and PR-104's open items 2 and 4. No behaviour
change and no play pin moved. Method as before: objdump of
/tmp/opencode/vis/text.bin for `.text`, dwords and strings from
build/leo/leo.elf. Three new tools: indcall.py (census of
`call DWORD PTR [reg+off]` sites and call-site function attribution),
vtable_scan.py (resolves an indirect call back to the table VIEW that
loaded the register - PR-100's two-view fact is what makes slot
searches findable), owner_slot.py (call-site -> enclosing function ->
which measured class-table slot owns it). One tool bug found and
fixed: the objfactory.out class-name regex required single spaces
between fields while the file aligns with several, so PR-104's
objslot.py annotations silently carried NO class names and matched
raw table ranges only - part of how PR-104's mis-attribution below
happened.

- THE CHAIN IS UNIQUE, END TO END. The record step 0x521f0 (record
  sub-table slot +0x0c, PR-100's threshold-vs-accumulator gate) has
  exactly ONE call site in the image: 0x5240c. It is inside 0x523f0,
  the record-step driver: `esi=[rec+0x44]` then, with the caller's
  key buffer kept at [esp+0x10]: `call [[rec+0x44]+0x08]` (the
  per-class key builder), `call [[rec+0x44]+0x0c]` (the step, with
  that key), `call [[[rec+0]+8]+0xac]` and `+0xa8]` (TWO OWNER-OBJECT
  SLOTS - the port's hook list at ll_level.c gets their WHEN), then
  `call [[rec+0x44]+0x10]` with ecx=0xc (the key-out method), and
  the driver RETURNS the composed key buffer to its caller. 0x523f0
  itself has exactly ONE call site: 0x5371c, inside 0x536f0.
- 0x536f0 IS THE OBJECT'S STATE-STEP VIRTUAL METHOD (slot +0x14). Its
  body, in order: `call [[obj+8]+0x8c]` (owner hook #3) and 0x49580
  ORs its key into the object's persistent key at [obj+0x60]; load
  the current record `[[obj+0x50]+[obj+0x5c]*4]`; call 0x523f0 on it
  (one key-build + one row step + one key-out); OR the returned key
  into [obj+0x60] too; `call [[obj+8]+0x54]` (owner hook #4);
  0x53520 (PR-102's keyed transition-table walk via 0x52000) and
  0x535a0 (PR-100's apply: switch record + row, running the OLD
  record's +0x20 leave hook). So ONE CALL TO ONE VIRTUAL SLOT = one
  key composition + one threshold/feed update + at most one row
  advance + one transition lookup + one transition apply. PR-98/99/
  100/102's machinery is one method.
- THE SLOT AND ITS TWO VIEWS. Constructors write [obj+8]=T and
  [obj+0xc]=T+8, so slot +0x14 is reached two ways: 6 sites call
  `[[obj+8]+0x14]` and 31 sites call `[[obj+0xc]+0xc]` - 37 call
  sites total, in per-level glue and in generic paths. Implementations
  found: the bare 0x536f0 at +0x14 in 24 class tables (PR-103's
  census, unchanged); the DEFAULT WRAPPER 0x400b0 at +0x14 in 45
  tables (it parses the condition vector `[[rec+0x2e]]+0x4`, calls
  the +0xcc handler, decrements [obj+0xee] if non-zero WITHOUT
  gating anything, then tail-calls 0x550a0); the LION base 0x550a0
  itself at +0x14 in both lion base tables 0xa2218/0x97970 (`zero
  [obj+0xe6],[obj+0xea]; call 0x536f0`); and per-class wrappers at
  +0x14 (e.g. 0x27d60 at the KADMAR second-base table 0x9a778+0x14)
  ending in the same `call 0x550a0`. All ten direct callers of
  0x550a0 are these wrapper bodies. [obj+0xee] is NOT a step gate:
  the wrappers decrement but never branch on it; who sets it
  non-zero stays open.
- FEED EXCLUSIVITY (the cadence core, now a FACT). The mode-
  accumulator dispatcher 0x543a0 (slot +0xa0) has exactly ONE call
  site in the image: 0x52317, inside the keyed branch of 0x521f0
  itself. Slot +0xa4 has two: 0x52341 (bare-ret hook after the row
  advance) and 0x535db (in 0x535a0's apply path). So the [obj+0xa1]
  accumulator grows EXACTLY once per state step, one row per step
  (PR-100), and there is no second scheduler anywhere: engine "time"
  for BY_TIME/BY_TY flows only when slot +0x14 runs. The port's
  facacde `obj_walk` + `o->facc < f->ms` gate is structurally the
  engine's gate; the only open number is the CALL RATE of +0x14.
- THE MESSAGE PUMP. 0x4bb30 (the earlier scan mislabelled it 0x4bb2f,
  one byte into padding) is a generic iterator-driver: for three
  vectors at [X+0x58+0/4/8] with counts at [X+0x44], and a fourth
  linked list at [X+0x64], it walks each item list through the
  [item+0x34] link and per item calls `[[item+8]+0xc]` - that IS the
  item's +0x14 state step, second view - and where the step's
  returned value matches a filter (esi) it then calls
  `[[item+8]+0x10]` with a stack-built message. So the level object
  keeps its placed objects in four lists at +0x58/+0x64 and its pump
  steps every object's state machine, then delivers messages to the
  ones whose step answered. Callers found: 0x4d7ab and 0x4d7c5 INSIDE
  0x4c8a0 - the huge function PR-100 knew as the feed-constant
  installer (it writes [level+0x388]=100 as its first word after the
  prologue; 0x24c locals; allocs; the two pumps sit at the end of
  its SETUP part, the second gated on global 0xa9ab4==0, followed by
  options init 0x57a00/0x4ea10 calls); 0x4a6b7 inside 0x4a4be; and
  0x1e333 inside 0x1e30c. Functions 0x4a4be and 0x1e30c are
  REFERENCED NOWHERE in the image (no call, no jump-table word) -
  dead variants or reached through an unread path. The steady-state
  per-frame call of the pump (or of any +0x14 site) is STILL NOT
  FOUND - this series has never located the frame loop - so "one
  step per video frame" remains the assumption it always was, now
  narrowed to exactly one question: who calls the pump or +0x14
  during play. What IS settled: there is no other driver, no timer,
  and no hidden scheduler between the port's per-tick gate and the
  engine's.
- MESSAGES AND THE +0x10 SLOT. Slot +0x10 is the message handler:
  the base lion's 0x50d60 (at +0x10 in 0xa2218, 0x98f34, 0x9b4d4,
  0x9f540, 0xa0624) switches `[msg+4] - 9` over a 13-case jump table
  at 0x50d20, calling lion slots (+0xd8, +0xfc, ...) with the
  message's payload fields [msg+8]/[msg+0xc], or setting
  [obj+0x116]=1; if [obj+0x126]!=0 and type==1 it takes a separate
  path first. TLewKara overrides +0x10 (0x33e60) and chains the base.
  ZARIA's own message handler 0x1e12f (UNREFERENCED - dead override):
  type < 5 tail-calls `mov eax,ebx; call 0x50d60` (the lion base
  +0x10 on the ARGUMENT object), type 5 runs a static counter
  [0xa94f4]++, `[[0xcef40]+0x38c]` through 0x44900 and `0x52cc0`
  with edx=9, type 0xc sets [obj+0x12a]=1. 0x52cc0 is the
  message-posting helper: 14 direct calls from per-level code, all
  with edx=9.
- THE LION CONTROLLER CHAIN (the one measured runtime driver of
  +0x14). Slot +0xd0 = 0x50ed0, present at +0xd0 in 12 lion tables
  including both bases (0xa2218+0xd0, 0x97970+0xd0, and the ten
  per-level classes): guards `[obj+0x11a] not in {6,7}`, argument
  ebx != 0, `[obj+0x126] == 0`, and BOTH `[[self+8]+0x40]` and
  `[[arg+8]+0x40]` non-zero (a per-class "am I live/enabled"
  virtual, called on controller and target); THEN it runs the
  ARGUMENT's full state step `[[arg+8]+0xc]` and stores the returned
  key at [obj+0x10a], calls `[[arg+8]+0x70]` into [obj+0x10e],
  walks the [obj+0xd2] object chain through 0x551f0, and finishes at
  0x502f0. The controller's own +0x10 message handler (or the +0x10
  slot itself, e.g. TLewKara 0x33e60 -> its +0xd0) reaches it. So
  the LION object synchronously steps the TARGET's state machine and
  caches its key - the state machine of a controlled object is
  driven by whoever holds it. Field [obj+0x11a] is a small enum:
  the +0xcc handler 0x50340 (which every +0x14 wrapper calls first)
  checks `== 5` and NEGATES the first dword of the caller's
  composed key; 6/7 suspend the +0xd0 chain; the base ctor inits 0.
- CORRECTION 1 TO PR-104, THE BIG ONE: THE DEATH THEMES ARE NOT LION
  OBJECT METHODS - they are the +0x08 key-builder slot of the RECORD
  (state) class sub-tables, the very slot 0x523f0 calls before the
  step. PR-100's census line "+0x08 the key builder: 12 different
  functions, 0x52750 5x and 0x4ffb0/0x23f90/0x26470/0x26fd0/
  0x2c360/0x2ded0/0x31fa0/0x33c60/0x361c0/..." IS the death-theme
  set: 0x97c9c+8 = 0x4ffb0 (DEATD, the default sub-table), 0x99338+
  8 = 0x26470 (ENA2ED), 0x9a85c+8 = 0x26fd0, 0x9b5f4+8 = 0x2c360
  (K2DEAD), 0x9d53c+8 = 0x31fa0 (RAN1), 0x9e254+8 = 0x33c60
  (KARADE), 0x9f460+8 = 0x361c0 (lonade), ... Every PR-104
  "override at [[TLew...]+0x128/+0x1d4/+0x1f0/+0xe4/+0x20c]" was a
  range artifact: the record sub-tables sit within +0x120 of their
  level's object table (0x9b5f4 = 0x9b4d4+0x120), and the regex bug
  above had stripped the class names that would have shown it. PR-
  104's DIRECT probe stands: [[obj+8]+0xe4] = 0x50300 really is an
  object-table slot (its body unread - open). The THEME's slot +0x08
  belongs to the record class only.
- CORRECTION 2: 0x52750'S RECEIVER IS THE RECORD, not the object.
  With this=rec it calls `[[rec+0x44]+0x2c]` (0x52680, PR-98's
  merge), `[[rec+0x44]+0x30]`, then `[[rec+0]+8]+0xc8]` - the OWNER
  object's +0xc8 slot (the fifth owner-side hook) - then
  `[[rec+0x44]+0x34]`, and returns the key buffer. It is the default
  key builder (PR-100: 0x52750 5x at +0x08); the death-theme
  builders are per-class OVERRIDES of that same slot which check the
  guard and then tail-call it.
- CORRECTION 3 (answers PR-104 open item 2): the guard reads
  `[[rec+0]+0x50]+[+0x5c]*4]+4]` - [rec+0] IS the owner pointer
  (PR-100), so it is the owner's current state's row index - and it
  is evaluated inside the key builder, i.e. ONCE PER STATE STEP.
  "Row 0" = the state machine is sitting on a state entered at row 0:
  entry happens in 0x535a0, and the next +0x14 call composes with
  row 0. Why row 0: structural answer now; whether the play methods
  restart the module on repeat calls (0x56010 reopens the file every
  call) is unmeasured.
- CORRECTION 4 (partly answers PR-104 open item 1): the flag
  [[obj]+0x126]==0 gates the DEFAULT DEATD builder, AND the +0xd0
  lion-step guard, AND the branch at the head of the +0x10 message
  handler 0x50d60. It is an object enable/active flag of the lion
  family; exact authoring (what sets it) still open. The
  [[0xcef40]+0x394]+0x106 re-arm from PR-104 now reads: slot +0xcc
  (run by every +0x14 wrapper before the step) writes the caller's
  three output dwords only when [obj+0x106]==0 - 1 means "keep".
- CORRECTION 5 (refines PR-100's record-ctor note): the "0x1e104
  record allocator" PR-100 quoted is the BODY OF A CLASS SLOT:
  0x1e0f0 sits at +0x84 in TLewZaria 0x96960, TLewZariaB 0x97a98
  and the ZARIA base 0x97970. Its key is a STATE NAME (edx): if it
  equals 'A_DEAD' case-insensitively it builds the record directly
  (`new(0x60); 0x524e0; [rec+0x44]=0x97c9c`), else delegates to
  0x61530. So per-level A_DEAD state records get the BASE sub-table
  0x97c9c explicitly - the A_DEAD state name is special-cased per
  level class at record-instantiation time.
- WHAT IS SETTLED FOR THE PORT, AND WHAT IS NOT. The port's walk/
  time gate (ll_level.c obj_walk + `facc < f->ms`) matches the
  engine's EXACT gate and the engine's step has no other driver: one
  state step = one feed update = at most one row. The remaining
  pacing unknown is single and concrete: the call rate of +0x14
  during play. Candidates found: a steady-state call of 0x4bb30 or
  per-object +0x14 sites from a frame loop (not yet located), the
  lion controller chain (measured, but only for controlled objects),
  and per-level glue. PR-105 changes no pin.
- OPEN ITEMS (PR-105 1-7): (1) the frame loop / steady-state caller
  of the pump; (2) [obj+0xee]: who sets it, what the wrapper counts;
  (3) the bodies and meanings of the five owner-side hooks now
  located (+0x54, +0x8c, +0xac, +0xa8, +0xc8) and of 0x50300
  (object +0xe4); (4) the message vocabulary: [msg+4] type numbering
  (who builds messages, the 0x52cc0 family, what type 9 and the
  9..0x21 lion range mean); (5) [obj+0x126] authoring, [obj+0x11a]
  enum values, [obj+0x10a]/[obj+0x10e] consumers; (6) the
  unreferenced 0x4a4be/0x1e30c/0x1e12f cluster - dead code or read
  path missing; (7) the four object lists at [level+0x58]/+0x64:
  which placement section feeds which list.
- Docs: this record in NEXT_STEPS.md; mirrored into HANDOFF.md §7
  item 13 and the §8 pacing paragraph; engine-comparison comments
  corrected/added at the row-threshold compare in ported/ll_level.c
  (one step = one +0x14 call, feed once, frame rate still open), at
  the `muzyka=` block (death themes are record-class key builders,
  not lion object methods), and one line added at obj_transition (the
  whole compose-step-lookup-apply sequence is one virtual method).
  No behaviour change, no pin moved.

## PR-106 — the frame loop found: what drives the object state machine
during play, the +0x60 gate and who writes the flag it reads, the
correction of PR-105's pump paragraph, and the frame source the pacing
question was really about (RE only: no behaviour change, no pin moved)

Closes PR-105 open item 1 (the frame loop) and the "who calls +0x14
during play" question that PR-98..PR-105 carried. Method change worth
recording: grepping the linear-sweep listing had to be replaced by a
call graph. Four new scratch tools under /tmp/opencode: cgraph.py
(direct-call graph, reverse callers with depth, dword/pointer scans,
--indcall/--reach), path.py (forward path from a root), dis.py (range
listing, enclosing-function heuristic), dat.py (exact-byte disassembly
at a VA with --adjust-vma, so no linear-sweep drift). Every address
below was read through dat.py at its own VA, not from a sweep. The
method also killed a false lead: the "jump-table entry at 0x587d9
pointing at 0x4bb30" is instruction bytes (`mov esi,[eax]` at
0x587d8, `mov ebx,4` at 0x587da) - an unaligned dword read, not data.

- THE FRAME LOOP IS 0x4abd0 (level object in ecx; the caller keeps it
  in eax/ecx). Exactly one caller: 0x5d251, inside 0x5d170 - the
  level-run wrapper (C++ exception frames; it passes `mov eax,ds:
  0xcef40`, so [0xcef40] is a POINTER to the level object; after the
  loop it reads [level+4] as the return value and zeroes the global).
  0x5d170 is reached from the per-level wrappers 0x5d3a0..0x5d7a0
  (entry table at 0x5d9d0) and from the single-level path 0x5eb11.
  Loop head 0x4ac3b, verbatim order: exit code 3 if
  [[[level+0x394]+0x116]] != 0 (the lion's flag PR-105 knew); exit 1
  if [level+4] != 0; `mov eax,0xcf400; call 0x58690`; if [level+
  0x3a0] != 0 the iteration is skipped (jmp 0x4ae82); if [level+
  0x3b0] != 0, 0x4a8f0(level), `[[[level+0x3b0]+4]+0x8]`, clear the
  field, 0x4a8c0(level); three modal blocks keyed on bits 0x20/0x4/
  0x8 of two stack bytes, each wrapped in 0x4a8f0/0x4a8c0 (0x4a8f0
  suspends, 0x4a8c0 resumes - the pair brackets every modal path).
  Then 0x4ae82: exit 2 if the lives global ds:0xa94f0 == 0; `call
  0x4ab50` (a plain 12-byte copy, src=edx, dst=eax: [esp+0x1f0] ->
  [esp+0x1e4]); `mov eax,edi; call 0x222d0`; **`mov eax,ecx; call
  0x4b970`**; **`mov eax,ecx; call 0x4ba90`**; `mov eax,[ecx+0x394]`
  (the LION, at +0x394 - the layer list is +0x390), `mov ebx,[eax+
  8]`, `lea edx,[esp+0x1e4]`, `call [[ebx+0xcc]]`; then
  `mov eax,[ecx+0x394]; call 0x52b80` at 0x4aed6 - the lion is
  pumped+stepped unconditionally every iteration; then 0x4ab80,
  `mov eax,0xcf068; call 0x563d0` (the music singleton), 0x4af10
  (applies the `plany=` layer-command string at [level+0x30]),
  `call 0x4b0e0` (the pacer, below), `inc DWORD PTR ds:0xcef3c`
  (the frame counter) and `jmp 0x4ac3b`.
- THE STEADY-STATE DRIVER OF +0x14 IS 0x4ba90 (the think pass), read
  verbatim: three groups at [[level+0]/[level+4]/[level+8]], each
  with a count at group+0x44 and a list of heads at group+0x58
  indexed in bytes, plus a fourth list at [level+0x64]; every item
  list is walked through the [item+0x34] link; per item:
  `mov ebx,[edx+8]; call DWORD PTR [ebx+0x60]`, and ONLY if that
  returns non-zero `call 0x52b80(item)` (0x4baed for the three
  groups, 0x4bb12 for the fourth). So PR-105's slot +0x14 is called
  once per frame-loop iteration per object, through a per-object
  gate - and the four lists are the same four PR-105 saw in the
  pump.
- 0x52b80 IS THE PER-OBJECT PUMP: first 0x52b50, which drains the
  object's own message queue at [obj+0x10] by repeatedly calling
  `[[obj+8]+0x10]` with edx = the queue node and freeing it through
  0x5c370 - the node's first dword IS the queue link, which is why
  PR-105 read the type at [msg+4]: the delivery form is edx = &head,
  so [msg+0] is the link and [msg+4] the type, one mechanism for
  both queued and direct messages. Then `call [[obj+8]+0x14]` - the
  state step, PR-105's chain. Then if [obj+0x28] != 0,
  `call [[obj+8]+0x0]` with edx=2.
- THE GATE READS A FLAG, AND THE OTHER PASS WRITES IT. Census over
  the 162 class-table pairs that vtable_scan.py measures (new scratch
  tool slot_census.py, reads the dwords straight out of
  build/leo/leo.img): slot +0x60 = 0x23b80 in **132 of 162** tables,
  and 0x23b80 is two bytes of body: `mov eax,[eax+0x2c]; ret`. So
  the gate is the object's field +0x2c. Who writes it: the pass at
  0x4b970 calls `[[item+8]+0x64]` with edx = [item+0x24]*0x14 + a
  scratch block, and slot +0x64 = 0x238e0 in 111 tables: `[obj+0x2c]
  = ([obj+0x5c] == 2) || ([[obj+8]+0x08] != 0)`, and the +0x08 slot
  = 0x23e00 in 157 tables: a point-in-rect test of [obj]/[obj+4]
  against a rect at [edx+0]/[edx+4]/[edx+8]/[edx+0xc]. The 21-table
  variant 0x53690 gets its rect from a 5-dword request through
  0x534e0 (request buffer {0,0,0,0,0x98274}) and compares it against
  the record block. So: **on-camera objects are stepped, off-camera
  objects are not stepped at all** (they keep no partial progress -
  their accumulator simply does not move), and the author of the
  activation flag is the drawing pass, not the state machine. The
  lion is exempt: 0x4aed6 pumps it every iteration.
- CORRECTION TO PR-105 (quote of the text being corrected: "per item
  calls `[[item+8]+0xc]` - that IS the item's +0x14 state step,
  second view - and where the step's returned value matches a filter
  (esi) it then calls `[[item+8]+0x10]`", and "the steady-state
  per-frame call of the pump (or of any +0x14 site) is STILL NOT
  FOUND"). Both are wrong. 0x4bb30 is not a state-stepping pump: it
  takes four register arguments (eax = the list owner, edx -> esi,
  ebx -> edi, ecx -> ebp) plus a stack argument at [esp+0x44]; it
  walks the same four lists; the `[[item+8]+0xc]` call is made ONLY
  when esi != 0 and its result is compared against esi - it is a
  filter predicate, and +0xc in the [obj+8] view is NOT the state
  step (+0x14 is; the 8-byte view shift PR-105 documented does not
  make +0xc into +0x14). When the predicate passes it builds a
  4-dword stack message {0, edi, ebp, stack arg} and calls
  `[[item+8]+0x10]`: a FILTERED MESSAGE BROADCAST, nothing more. Its
  callers are 0x4d7ab/0x4d7c5 inside 0x4c8a0 - confirmed here as the
  LEVELDES config parser (key literals from 0x91e01, the string
  compare 0x76d20, the loop back to 0x4cbbc), i.e. setup, not play -
  plus 0x4a6b7 (0x4a4be) and 0x1e333 (0x1e30c), still unreferenced.
- THE FRAME SOURCE, in pieces, all read at their own VAs. (a)
  0x4b0e0 = `mov eax,0xcef30; call 0x4a120; call 0x1cfe5; ret`.
  (b) 0x4a120 is the pacer: if `ds:0xcf1b2 - [obj+4] >= [obj+8]` it
  returns at once, else it spins on that clock and then `call
  0x1dba0`; finally [obj+4] = ds:0xcf1b2. (c) 0x1dba0 is a VGA
  retrace wait: `mov dx,0x3da` then `in al,dx; and al,8` waiting for
  bit 3 to go 1, then again waiting for 0 - one retrace, port 3DAh.
  (d) The pacer struct is the global controller object at 0xcef30:
  [0xcef34] is the last stamp, [0xcef38] the setpoint. Its reset
  0x4a0e0 writes 0, 0 and 1000 (a not-yet-measured value); the only
  other writer of those words is 0x4a160 (`mov [eax+4],0`, called
  with eax=0xcef30 from the loop's exit path) and the pacer itself.
  (e) The setpoint is authored at startup by 0x4a190: it starts the
  ms clock, measures 5 retrace pairs (0x1dba0, read the clock,
  0x1dba0, read the clock), keeps the MINIMUM in ecx and the MAXIMUM
  in esi, then if (max-min) > 2 writes **[obj+8] = 0x1b = 27**,
  else writes **[obj+8] = min * edx - 1**; both call sites pass
  edx = 2 (0x5e4a1 in 0x5e410, 0x5eaee in 0x5eae0). So the engine
  paces over TWO display frames, and the fallback 27 is exactly
  2*14-1: the author assumed a 14 ms frame.
- THE CLOCK BEHIND IT, AND THE MODE THAT SETS THE FRAME TIME. The
  clock ds:0xcf1b2 is advanced only at 0x1c65d: `add ds:0xcf1b2,
  ds:0xcf1b6`, inside the dispatcher 0x1c5b0 (reached from the ISR
  0x1c6d0 -> 0x1c680, which switches to the ISR stack at [0xcf450]);
  that dispatcher first walks the registered periodic-handler list at
  ds:0xcf1bc, per handler `call [[h+0x14]+0]` with its period at
  h+0xc and last-fire at h+0x8. ds:0xcf1b6 = 0xcf1a8+0xe and the
  clock ctor 0x5b860 sets `[esi+0xe] = 0x37 = 55` (also 0x5b9a2 for
  another clock object), and 0x5b860 is the only thing that installs
  0x1c6d0: it gets/saves the INT 8 vector through the DOS wrappers
  0x75c5a (INT 21h AH=0x35) and 0x75c8c (AH=0x25; both have a DOS
  7+ extended variant chosen by the version byte ds:0xa9bee) and
  0x5b8f0 restores it. So the game's "ms" clock = one BIOS timer
  tick stamped as 55 ms. 0x5b860 is called from 0x4a1ac (inside the
  calibration), from **0x5d18b inside the level-run wrapper** and
  from 0x5decb - the clock only runs during a level. Separately, the
  video mode: 0x4a230 = INT 10h with AX=0x13 (mode 13h, 320x200)
  through the real-mode INT wrapper 0x75c3d, then a helper 0x76a64
  with eax = 0x13ba0 (80,800 = 400x202) whose result is stored in
  ds:0xcef2c; the frame-address helper 0x4a170 = ds:0xcef2c +
  ds:0xcf0e8 + 0x190 shows the internal buffer's row pitch is 400.
  0x4a270 undoes it (INT 10h AX=3). Both are called by the run
  wrappers 0x5e410 and 0x5eae0 - and the 0x5eae0 path is the single-
  level path: set mode, calibrate the pacer, `mov edx,9; mov ds:
  0xa94f0,edx` (lives = 9, the global the loop tests for exit 2),
  then 0x5d170 (the level run) and 0x4a270. Finally, the sound
  driver init (0x5cfc0 -> 0x1b800 -> ... -> 0x18870, reached from
  the level-run setup) installs the INT 1Ch handler 0x18730 AND
  writes PIT counter 0: control 0x36 then count bytes 0x95,0x42 =
  0x4295 = 17045, i.e. 1193182/17045 = **70.0 Hz**, with 0x188c0
  restoring count 0 (18.2 Hz) on teardown.
- WHAT IS SETTLED FOR THE PORT (and what is arithmetic, labelled).
  For an ON-CAMERA object one frame-loop iteration = exactly one
  +0x14 call = one key composition + one feed update + at most one
  row (PR-105), so the state machine's cadence is the frame loop's
  cadence, and PR-105's "no other scheduler" stands: the loop calls
  it, nothing else does at runtime. The loop's own period is bounded
  by (i) the retrace wait 0x1dba0, (ii) the setpoint [0xcef38] =
  2 measured retrace intervals - 1, or the 27 fallback, and (iii)
  the clock's own tick, since the spin in 0x4a120 cannot exit until
  the clock has advanced by >= the setpoint and the clock only moves
  in 55-unit steps. INFERENCE, not measurement: with mode 13h's
  70.08 Hz retrace and the sound driver's 70.0 Hz PIT, one
  iteration is 2 retraces = 28.6 ms, i.e. ~35 object steps per
  second, and the 27/2-frame design matches that. The port's 20 ms
  tick (ll_level.c:1136, `facc < f->ms`, time_scale compared
  unscaled) is NOT what the engine does; this record does not
  re-pin it - the number to fit is now a small, well-posed set (see
  open items) and any change needs the data-free test + mutation
  check the ground rules require.
- OPEN ITEMS (PR-106 1-6): (1) what 0x58690 does with the 0xcf400
  singleton per iteration: it reads a state dword at [obj+0xb], and
  for state 2 copies 12 bytes from a 16-byte-strided record array
  (index at [obj+0x2b], sub-index at [obj+0x2f]); those 12 bytes are
  then copied by 0x4ab50 into the block handed to the lion's slot
  +0xcc at 0x4aeca. Input-record playback, format and authoring
  unread. (2) The real-hardware frame time: 55 ms clock granularity
  vs a 70 Hz PIT vs the 70.08 Hz mode-13h retrace - the three
  numbers are machine-verified individually, their combination on a
  real machine is not, and the port needs one of them (or a video
  measurement) to convert rows to seconds. (3) What writes
  [obj+0x5c] (the == 2 force-active case) and the other [obj+0x2c]
  writers (0x2b809, 0x2db80, 0x3f7a0, 0x28eb6/0x28ef6/0x2905b, ...).
  (4) PR-105 item 7 unchanged: what feeds the four lists. (5) The
  unreferenced 0x4a4be/0x1e30c/0x1e12f cluster: the aligned-dword
  pointer scan finds no reference, so either dead code or a
  computed/unaligned address. (6) The +0x60 slots that are NOT
  0x23b80 (26 other targets over 30 tables, e.g. 0x23280 in 4) -
  per-class rate gates or flag variants, unread.
- Docs: this record in NEXT_STEPS.md; mirrored into HANDOFF.md §7
  item 13 and the §8 pacing paragraphs (the sentence "what a row's
  duration is in seconds is one unread number: how often +0x14 is
  called" is replaced by the frame loop + gate + pacer chain, and
  PR-105's pump claim is corrected in place); the engine-comparison
  comment at obj_walk in ported/ll_level.c is corrected the same way
  (the 0x4bb30 "pump that steps every level object" sentence
  replaced by the 0x4abd0 -> 0x4b970/+0x64 -> 0x4ba90/+0x60 ->
  0x52b80 -> +0x14 chain and the pacer). No behaviour change, no
  pin moved; §1/§6 estimates untouched.

## Tooling addendum (no code, no pin) - an indexed search over the image,
and what it cross-checks from PR-106 (the method change PR-106 recorded,
turned into a tool: `tools/re/leodb.py`)

The session's other tooling (cgraph.py, path.py, dis.py, dat.py,
slot_census.py) stays scratch under /tmp/opencode; this one is committed
because the "one script per question" cost was the actual bottleneck.
Built with `python3 tools/re/leodb.py build`: 1.5 s,
/tmp/opencode/leo.sqlite (51 MB, derived, never staged), sha256-stamped
against build/leo/leo.img (edf19a129e85efb8...).

- WHAT IS INDEXED. `insn` 201,475 rows (the whole image's linear sweep,
  indexed by (mnem,va), with `prov` for inside/outside .text); `xref`
  27,558 direct branch targets indexed by dst; `dword` 816,845 rows -
  EVERY byte offset of the image, indexed by value, with an `aligned`
  column; `imm` 5,181 trailing immediates of instructions (the
  `mov eax,0xcef30` form a dword scan cannot see); `loadimm` 3,875
  `mov DWORD PTR [reg+disp],imm` sites; `mtable` 189 class tables;
  `str` 4,359 printable runs; `func` 1,845 entries from
  build/decompiled/all/index.csv (names only, as hints).
- WHAT IT ACTUALLY BUYS. Single lookups are NOT much faster than grep
  (measured: `callers 0x5d170` 0.020 s vs cgraph.py 0.223 s; `refs`,
  `slots`, `find` all ~0.02 s, the same as grepping full.asm). The win
  is composite questions in one lookup, which is what PR-106's method
  needed: `refs 0x4bb30` returns the single pointer at 0x587d9 with
  `aligned=0 seg=code FUN_000587d0+0x9` - the false lead PR-106 had to
  chase by hand - next to the four real call sites 0x1e333/0x4a6b7/
  0x4d7ab/0x4d7c5; `loadimm` turns the two-view ctor pattern into one
  set intersection instead of vtable_scan.py + objcensus2.out;
  `tables 0x96874` lists all 16 sites that store a class table into
  `[obj+0x8]`, which is the "which class builds which table" question
  PR-103/PR-104 could only answer through the 0x4df10 factory.
- CROSS-CHECK OF PR-106 (independent method, same facts). The detector
  proves 158 class tables from the code pattern (`ctor`) where the
  call-site census measured 162; over its set: +0x60 = 0x23b80 in 129
  (record: 132), +0x64 = 0x238e0 in 109 (111), +0x08 = 0x23e00 in 153
  (157) - and 26 DISTINCT non-default +0x60 targets, exactly the 26 of
  PR-106 open item 6. So the record's numbers are not an artefact of
  one tool; the small differences are the table SET, and the counts are
  reported as counts over detected tables, never as facts about the
  game.
- WHAT STAYS OUT OF THE INDEX. Any disassembly that is read as evidence:
  `dis` shells out to objdump over the exact bytes at the exact VA and
  is byte-identical to dat.py (diffed at 0x23b80), and the sweep rows
  are marked `[SUSPECT]` where another instruction overlaps them - the
  first such row is visible immediately in `func 0x4ba90`, whose body
  starts one byte off. `func`/`callees` ranges come from the Ghidra
  entry list and are labelled hints.
- NOT DONE, on purpose: importing Ghidra's own xref database (better
  function boundaries than the entry-list hints) and capstone as a
  second disassembler to cross-check objdump. Say the word if either
  starts to matter; until then the two-level rule holds - index for
  FINDING, objdump for READING.
- Docs: this note; tools/re/README.md (usage + caveats); HANDOFF.md §9
  method note 1 gets the indexed step. No ported/ file, no pin, no
  behaviour change; the generated .sqlite is not in git.

## PR-107 — the authored start row applied on the object side too: the
row-set method read verbatim, rec+0x4 shown to be the live row counter,
the correction of PR-101's end of the switch, and the shipped scope of a
start row (behaviour change, one new selftest leg)

Closes the half of PR-102/PR-103 that the port never used: `ll_mov_parse`
has read the `( N )` of a transition line since PR-86 and `obj_transition`
threw it away. Every address below was read with `python3 dat.py <VA>
<len>` (exact bytes, `--adjust-vma`), not from the linear-sweep listing;
the image is 32-bit i386, so the register width of that listing was the
first thing to check and it is `eax/edx/ebx`, not the 64-bit form.

- THE ROW NUMBER IS BUILT AT PARSE TIME, VERBATIM. 0x51e07 is the
  transition-line tail of the record parser: 0x51e0b calls 0x53440 (the
  case-insensitive state-name walk of PR-102/PR-103) and keeps the index
  at `[ebp-0xc]` (0x51e1c); 0x51e12 loads the format at 0x9225c =
  `" ( %[^) ] %*[)] = "` and 0x51e22 matches it (0x57d30) into a local
  buffer; 0x51e2a calls 0x57db0, which is a scan of that token with the
  format at 0x92310 = `"%ld"` through the scanner at 0x74cc8 and answers
  the CONVERSION COUNT, 0 when the token is not a number; on 0 the code
  jumps at 0x51e31 to 0x51e3e, `mov eax,0xffffffff`; on success 0x51e36
  calls 0x57fd0 (`strtol`, the call at 0x76fa3) and 0x51e3b is `dec eax`.
  So the row field is N-1 for a numeric token and -1 for an absent or
  non-numeric one - PR-103's reading, confirmed at the byte level. 0x51e4d
  then builds the entry (0x49610) from (state index, row value, condition)
  and 0x51e8f pushes it with `rep movsd` of 5 dwords plus one `movsb` =
  21 bytes, which is PR-102's stride `add ebx,0x15` exactly.
- THE LOOKUP HANDS THE ROW OUT SEPARATELY, VERBATIM. 0x52000 walks that
  vector (0x52011 `lea esi,[eax+0x1a]`, 0x52020 `add ebx,0x15`, count at
  `[esi+8]`), calls 0x47e40 per entry (0x52033) and on the FIRST match
  writes `*out0 = [entry+0xd]` at 0x5203c-0x52042 and `*out1 =
  [entry+0x11]` at 0x52044-0x5204d: the target state index and the row,
  as two values. 0x535a2 receives them (0x535a4-0x535a8: `mov ecx,eax;
  mov esi,edx; mov edx,ebx`) and on its same-index path writes the row
  into the record's field at rec+0x4 unless it already holds it
  (0x5360f-0x53616).
- rec+0x4 IS THE LIVE ROW COUNTER - PROOF FROM THE ROW STEPPER (new, and
  the fact this whole change rests on). 0x520c0 takes the row structure as
  `[[rec+0x2e] + [rec+0x4]*4]` (0x520cf-0x520d7). Forward: if
  `[rec+0x4] != count-1` it adds one (0x521d4-0x521d7); AT the last row it
  ORs key bit 2 into the caller's key byte (`or byte [ebp+0],4`, 0x521c1)
  and sets `[rec+0x4] = 0` (0x521c5). Reversed: it subtracts one
  (0x521ae-0x521b1) and at row 0 ORs bit 3 (0x521a1) and sets
  `[rec+0x4] = count-1` (0x521a5-0x521a9). The count is
  `[[rec+0x2e]+8]` = `[rec+0x36]` - the expression the port's own
  obj_advance comment already quotes, now seen from the other side. The
  direction test at 0x5214b-0x52193 is PR-95's: record bits 0x10/0x20 at
  `[rec+0x12]`, XOR-ed against the owner-side guards and against
  `[owner+0x99] == -1`.
- THE ROW-SET METHOD 0x52360, VERBATIM. Prologue calls 0x52070 first
  (0x52077 zeroes `[owner+0x9d]`, then interpolates between the record's
  `[+0x8]`/`[+0xa]` words), then `[owner+0xa1] = 0` (0x5236b) and
  `[owner+0x99] = 1` (0x52377) - the second is the field the stepper
  compares against -1 at 0x52184, so a row write also flips the direction
  term; meaning unread. Then `test edx,edx` + `jl 0x523ce` (0x52381): a
  NEGATIVE row changes nothing. Otherwise the SEQ_REV test (0x52385
  `[rec+0x12] & 0x10` against the owner's `+0x70` slot, 0x523a0 `& 0x20`
  against `[owner+0x71]`, XOR-ed at 0x523b8) picks between the mirror
  `[rec+0x4] = [rec+0x36] - n - 1` (0x523bc-0x523c5) and the raw
  `[rec+0x4] = n` (0x523cb). There is NO upper bound anywhere in it, and
  the value is stored as the parser left it - N-1.
  THE SLOT IS RESOLVED, AND IT IS THE STATE RECORD'S OWN METHOD TABLE
  (this is what turns "the effect is proven either way" into the engine's
  actual call graph). Four tables point at 0x52360 (0xa25f0, 0xa2628,
  0xa2eb8, 0xa2ef0) and in each it sits four bytes after 0x520c0, the
  same adjacency as the .MOV state-record family (0x52590 at 0x97c9c with
  0x520c0 at 0x97cb4; 0x9a85c with 0x9a874; the same pairs at
  0x98e7c/0x98e94, 0x99338/0x99350, 0x9b5f4/0x9b60c, 0x9cedc/0x9cef4,
  0x9d53c/0x9d554, 0x9e254/0x9e26c, 0x9f460/0x9f478). Which one a
  record holds is answered by what writes `rec+0x44` at all: the record
  constructors 0x524cf and 0x52508 both do `mov [eax+0x44],0xa25d4`, so
  every state record of this class uses the table at 0xa25d4. Its slots,
  read out of it: +0x00 = 0x52590, the state-record
  parser of PR-102, which is what makes it the record's own vtable and
  not a per-class view; +0x0c = 0x521f0, the step PR-105 traced to one
  call site (0x5240c), now visible as `mov esi,[ebx+0x44]` +
  `call [esi+0xc]` - the record driver reaching this table's slot +0x0c;
  +0x18 = 0x520c0, the row stepper; +0x1c = 0x52360, the row-setter;
  +0x20 = 0x52660, which clears bit 0x20 of `[owner+0xb6]` (the PR-98
  drag-floor byte). Of the two constructors, 0x524cf's first zeroes
  `[eax+0x48]..[eax+0x5c]` (PR-98's `F=`/`T=`/`G=` pair) while
  0x52508's copies them from the record being cloned (0x5250f-0x52530),
  and 0x52503 calls the base constructor 0x517e0 where 0x524a0 calls
  0x51700. The three sibling tables of the family carry the same pair:
  0xa260c (+0x00 = 0x51910, the BASE record parser of PR-102) has
  0x520c0 at +0x18 and 0x52360 at +0x1c, and so do 0xa2e9c and 0xa2ed4
  - those slots are the record family's shape, not one variant's. So the
  transition step's two calls are those slots: the old record's +0x20
  (0x535bc-0x535bf) and the new record's +0x1c (0x535e7-0x535ea) - the
  cross-state path does go through 0x52360. Both methods take the record
  in eax (`mov edx,eax` at 0x52661, `mov ebx,eax` at 0x52362) and leave
  the object in ecx, exactly as the call sites do, and the row argument
  survives the calls in between because those methods carry edx for their
  caller: 0x52660 opens with `push edx` (0x52660) and returns through
  `pop edx` (0x5267a), and 0x52070 does the same (0x52072, 0x520b4) -
  the way this binary keeps its second register argument. Reading the two
  paths apart also settles what a -1 means on each: the same-index path
  does NOT call the setter, it writes `mov [eax+0x4],edx` itself (0x53616)
  with no sign test, so a -1 is really stored and the stepper's wrap
  (0x521c5) puts it on row 0 at the next tick - the same visible result
  as the setter refusing it. That is the support for the port's model
  choice (2): "no number" starts at the top on both of the engine's
  paths, by different routes.
- CORRECTION TO PR-101 (quote: "Both paths end at 0x53626: `[obj+0x9d] +=
  1`, and if the WORD at `[rec+0xc]` is non-zero and `<= [obj+0x9d]`, set
  bit 0x2 of the byte at [obj+0x61] ..."). Only the SAME-INDEX path
  reaches 0x53626. The different-index path returns at 0x53601 - and it
  zeroes `[obj+0x9d]` on the way (0x535c2). So that per-record counter
  limit publishes the key bit for same-state switches only, and
  `[obj+0x9d]` is not the row counter: the row counter is rec+0x4 (the
  stepper above reads and writes only that).
- CORRECTION TO PR-102'S CENSUS FRAME (quotes: "`A_WAIT(1)=AC_BREAK_NEXT`
  2572 such lines in 777 .MOV files", "37 target states are entered at
  several different rows ... the port parses the start row and throws it
  away; the engine applies it thousands of times"). The 2572 count is
  right; the scope is not what that sentence implies. Re-measured THROUGH
  THE PORT'S OWN PARSER (new scratch census `/tmp/opencode/movrows.c`,
  links ported/ll_play.c so the row counts are what ll_mov_parse sees -
  the python mirror `/tmp/opencode/rowstart_census.py` had counted only
  rows whose frame number is non-negative and so got 17 out-of-range cases
  where there are 14, and its row counts for states authored with
  negative frame numbers were wrong): of the 2572 numeric lines **2299 are
  `(1)`**, which IS the engine's row 0, so they cannot change anything;
  195 carry an absent or non-numeric token (the census does not split the
  two). A blind grep is not the same count and is worth having on record:
  `grep -roE '\([a-zA-Z][^)]{0,12}\)='` over those 777 files finds 208
  `(x)` and 3 `(X)` lines in 44 files, so more transition-shaped lines
  exist than the parser records as entries - the census number is the one
  that describes what ll_mov_parse actually stores. Only **273** ask for a
  row other than 0, **101** of those are self-targets, so **172** shipped
  lines are
  cross-state start rows. And "37 states" counts state NAMES; as
  (file, target-state) pairs it is 51.
- THE SCOPE THAT CAN ACTUALLY MOVE ON SCREEN. Of those 172, the ones in a
  SCENE-object script (a .MOV whose class is not the lion's `LEW`/`LEW2`)
  are **25**: KADMAR.07 22, KADMAR2 1, ENA2 1, SADES.06 1 (the other 147
  are in the lion's own scripts, which the port has honoured since PR-86).
  1 of the 25 targets a state whose block carries a `SEQ_REV` name, 7 of
  the 25 target a state that declares an `AC_BREAK_NEXT` exit - the wrap
  of PR-101 is what publishes those transitions, so a start row shortens
  such a state. 14 shipped lines in the whole data set ask for a row at or
  past their target's row count (12 of them self-targets); the two
  cross-state ones are `A_WALK -> T_TELEPORT1 (39)` and `A_BORRING ->
  T_TELEPORT1 (39)` in LONA.05's lion script, where T_TELEPORT1 has 31
  rows - both are already clamped by the lion's side.
- WHAT THE PORT NOW DOES. `obj_transition` sets the target's start row
  instead of 0, and both sides call one helper, `ll_mov_row_start`
  (ported/ll_play.c), so the lion's rule and the object's cannot drift:
  `entry > 0` -> index `entry - 1`; `entry == 0` (the port's form of the
  engine's -1: absent or non-numeric) -> the top; an index at or past the
  target's row count -> the top. Three MODEL CHOICES, stated: (1) the
  clamp is the port's, the engine writes the value raw (0x523cb) and its
  consumer then takes the end-of-list branch; the lion's side has clamped
  since PR-86 and this keeps the two sides identical, and it is what the
  new selftest pins for `(9)`. (2) "leave the row where it was" is what
  the engine does for -1 (0x52381), and it is NOT what the port does: the
  port starts at the top. That is the common case by accident - a row list
  that ran to its end leaves rec+0x4 = 0 (0x521c5) - but the engine's
  per-record counter also survives a state change MID-list, and the port
  has one counter per object, not one per state record. Open item. (3) the
  mirror of 0x523bc-0x523c5 is not modelled at all, as before (PR-95 item
  1); the census says 1 of the 25 would touch it.
- TEST (data-free, in `obj_state_selftest`, run by `./ll_level
  --mov-selftest`): one synthetic script, four cases generated from it by
  substituting the token. A_RUN is two 100 ms rows and its second raises
  K_GEN1, so `A_DEAD(T)=K_GEN1` fires at 200 ms; A_DEAD is three 100 ms
  rows. Hand-derived: `(3)` must enter at frame index 2 (N is 1-based) -
  the value the old code could not produce - `(1)` must enter at 0, `(9)`
  clamps to 0, `(x)` enters at 0; then each case advances by
  (3 - start) x 100 ms and must reach the end of A_DEAD's list, take the
  wrap's key bit and return through `A_RUN(1)=AC_BREAK_NEXT` to A_RUN row
  0. Mutation checks, all three seen to fail: reverting obj_transition to
  `o->fi = 0` fails the `(3)` case; deleting the clamp makes `(9)` land in
  A_RUN instead of A_DEAD; reading N as 0-based fails the `(1)`/`(3)`
  cases. Restored, `--mov-selftest` and `--play-selftest` are green, and
  the selftest's own summary line now names this leg the way it names the
  PR-92/PR-93/PR-95 ones.
- READING NOTES (the method cost of this PR, worth keeping). The
  linear-sweep listing in /tmp/opencode/vis/text.asm (a) misses entry
  points mid-instruction - 0x52360, 0x520c0 and 0x4ff40 have no row at
  their own address - and (b) any reasoning over it must be in 32-bit
  registers: the ELF is `ELF 32-bit LSB executable, Intel i386, no section
  headers`, VA = file offset + 0xf000. `tools/re/leodb.py` answered the
  pointer questions this PR needs in one call each: `refs 0x52360` (the
  four table slots), `refs 0x520c0` (the same tables + the .MOV state
  record family, which is what proved the adjacency), `slots +0x1c` (the
  negative result, which is the wrong question: that census only walks the
  tables object constructors store at `[obj+0x8]`, and the answer is about
  the table a RECORD stores - it came from asking what writes `rec+0x44`
  instead, which is one grep in the disassembly and two `dat.py` reads).
- PINS: none moved, and why, because it says what the suite can and
  cannot see. `make -k verify-all DATA=/tmp/opencode/iso_stage/LEO
  HD=/tmp/opencode/hd_files` is EXIT=0 on this tree with the same outputs
  as HEAD, and the fixture-free run is EXIT=0 too. No pin moved because
  nothing in the suite pins an object's row index over time: the level
  legs pin scene TEXT (`show KADMAR2: bg PLAN_.PCM ...`, the
  `authored scene TUNGA.11: ...` greps, `play tick 30: state A_...`) and
  compare composites C-against-C (linux vs wine), never against a stored
  frame; the lion's own start rows, which the play legs do exercise
  (KADMAR.07, ENA2, LONA.05, ZARIA.01/02, KARA.07, ENA.03), go through
  the same helper but with the rule the port has had since PR-86, so
  their state text is unchanged. Consequence, stated as an open item:
  the 25 cross-state object start rows this change makes live are visible
  to no data leg today - KADMAR.07's 22 are in a level the suite only
  runs as a lion - so the new selftest is the only coverage, which is why
  it is four cases plus the wrap rather than one.
- CI: the first pass on `5238c4f` was green on `linux` and `asan` and
  failed on `windows-smoke` at `platform_selftest: sleep_until honors the
  deadline (>=20ms)` - the same wall-clock selftest that flaked on
  PR-35, PR-36, PR-94 and PR-97 (the note under the PR-95 record, and
  PR-97's own CI note). Nothing in this commit touches platform code, and
  `verify-all`, which runs `verify-platform`, had passed locally three
  times before the commit (twice with fixtures, once fixture-free), so the
  tree was re-run rather than changed (`gh run rerun --failed`) and the
  rerun completed green for all three jobs - the same outcome as those
  four. The non-wall-clock rework of that selftest stays a low-priority
  item.
- Docs touched: this record; HANDOFF.md §7 item 13 (the row-start
  findings and the three model choices) and §8's class-map/pacing
  paragraph (the "the port parses it and ignores it" sentence is now
  closed, and the rec+0x4 fact is added); the code comments in
  ported/ll_play.c (ll_mov_row_start, the evidence above),
  ported/ll_play.h (ll_mov_tr.entry and the new prototype) and
  ported/ll_level.c (the obj_transition header). No file under
  .github/workflows, nothing copyrighted staged.
- OPEN ITEMS: (1) The row counter's OWNER. rec+0x4 belongs to a state
  record, and the transition step reaches those records through the
  object's array at `[obj+0x50]` indexed by state index (0x535b6 and
  0x535e1) - so the engine keeps one counter per (object, state), where
  the port has one `o->fi` per object. Whether those records are built per
  instance or shared, and who writes `[obj+0x50]`, is PR-103's open item
  2, still open. What follows from it is measurable: an entry with no
  number leaves the engine's row where it was (0x52381), so that path
  RESUMES a state's row list mid-way, and only the ran-to-the-end case
  coincides with 0. 195 shipped lines carry an absent or non-numeric
  token; how many of those are object cross-state lines was not measured
  (the same scratch census answers it). (2) The 101 shipped self-target
  lines with a row: the engine's same-index path DOES write them (0x53616);
  the port's object path skips self-target lines entirely and its player
  path deliberately keeps the state running (PR-86). Neither side applies
  those rows today. (3) The owner side of the row-set method. `[rec+0]` is
  the owner: 0x52369 and 0x52663 both read it and 0x5266c then clears bit
  0x20 of `[owner+0xb6]`, the byte PR-98 read the drag floor in - so
  leaving a state clears that bit, and what that costs is unread. The pair
  `[owner+0xa1] = 0` / `[owner+0x99] = 1` (0x5236b/0x52377), the second
  feeding the direction test at 0x52184, and the `[obj+0x9d]` zeroing at
  0x535c2 and the record getter's own `[owner+0x9d] = 0` (0x52077), are
  still unmapped. (4) The sibling record vtables: 0xa260c is the BASE
  record class's (+0x00 = the base record parser 0x51910), 0xa2e9c and
  0xa2ed4 are two more derived ones, and the methods only they carry
  (0x559c0, 0x518f0, 0x55970, 0x55820 at 0xa260c+0x08/+0x14/+0x24/
  +0x2c) are unread; 0x51700, which the record constructor 0x524a0 calls,
  is the base class's constructor. Which .MOV state class uses which of
  the four is open. (5) The
  death-theme hook of PR-104/PR-105 reads the current state's row index
  == 0; a non-zero authored start row would suppress that theme, which is
  unmeasured and is the one visible consequence of this change that the
  video could decide. (6) Whether a row write also resets the row
  accumulator (the method zeroes `[owner+0xa1]` before storing) - the port
  carries its row timer across a state change (`o->t` is not reset by
  obj_transition, and the lion's side resets `p->ft`); unchanged here, and
  the difference is measurable on the lion's side whenever a transition
  fires mid-row. (7) No data leg can see a start row at all today (see
  the PINS note): the 25 shipped object rows live in KADMAR.07, KADMAR2,
  ENA2 and SADES.06 and the suite runs those blocks as a lion scene or a
  PCM scene only. A leg that follows one of KADMAR.07's objects, pinned
  against the video of that scene, is what would close it.

## PR-108 — the BY_TY vertical feed and its `[obj+0xbe]` guard: the
input-apply method and the contact record that write them, the corrected
PR-102 gate, a census of the twelve authored climb states, and a
regression leg for the model the port runs today (RE + test, no
behaviour change)

PR-106's T1 item (3) - "the `[obj+0xba]`/`[obj+0xbe]` gate of the time
feed, meaning unread" - and HANDOFF T5 (ladders, vines, climbing) are the
same open end, and PR-107's pacing paragraph says the test that would
close it is footage of a BY_TY climb. This PR does the RE, the shipped
census and the test that makes the CURRENT model non-vacuous; it does not
choose between the two readings, because the readings cannot be told
apart from the data on hand. Per the standing rule about not guessing, it
stops there and names the exact clip that would decide it (below). The
one line of the port that is corrected is a comment, not a behaviour:
the ll_play.h vocabulary note repeated PR-102's wrong description of the
feed's guards.

- CORRECTION TO PR-102, QUOTED FROM THE PR-102 RECORD: "mode 2 ->
  0x543f2 is guarded by `cmp [obj+0xba],0; je out` and then `add
  [obj+0xa1], [[0xcef40]+0x388]` (PR-100's 100); mode 3 -> 0x543fb is
  the SAME body reached after passing that guard". The first half of that
  has the two addresses swapped, and the consequence is the interesting
  part: **BY_TIME, the mode in 91% of the shipped states (2121 of 2318,
  PR-99), has NO guard at all.** The dispatcher 0x543a0 reads the mode
  with `mov eax,[eax+0x12]; and eax,7; cmp eax,4; ja 0x5443f`
  (0x543af..0x543b8) and jumps through `jmp DWORD PTR
  cs:[eax*4+0x54380]`; the raw little-endian dwords at 0x54380 of
  build/leo/leo.elf are, in mode order 0..4, 0x543c6, 0x543dc, 0x543fb,
  0x543f2, 0x5441b. Entry 4 is the fifth entry of a five-case table,
  which confirms PR-102's five-value mode enum. Mode 2 enters the feed
  body at its first instruction, `mov eax,ds:0xcef40` (0x543fb); mode
  3's entry 0x543f2 is `cmp DWORD PTR [ebx+0xba],0x0; je 0x5443f`
  (0x543f2-0x543f9) and its fall-through IS that same body; mode 4
  (0x5441b) is a separate body with the `[obj+0xbe]` guard, adding the
  same `[[0xcef40]+0x388]`. So the correct sentence is: the guarded
  bodies are 0x543f2 (mode 3 only) and 0x5441b (mode 4), and mode 2 is
  unguarded. Corrected in ported/ll_play.h's vocabulary comment, in
  HANDOFF 7 item 13 and in HANDOFF 8's T1 paragraph. BY_TX is still 0
  occurrences in the shipped .MOV texts.

- THE GUARD'S PRODUCER, AT LAST: THE INPUT-APPLY METHOD 0x549f0. It is
  the frame-loop slot +0xcc: `leodb.py slots +0xcc` reports it as the
  target in 102 of the 158 detected class tables (e.g. 0x96874, 0x96fd8,
  0x970bc), and it is what the level frame loop calls for the lion every
  iteration - PR-106's frame loop 0x4abd0: 0x4ae92/0x4ae99 are the two
  stack buffers, 0x4ab50 the 12-byte copy, 0x4aeca
  `call DWORD PTR [ebx+0xcc]` with `ebx = [[ecx+0x394]+0x8]` (the LION)
  and edx = the block, which comes from `mov eax,0xcf400; call 0x58690`
  (PR-106 open item 1, still unread). That census also found a second
  target in 14 tables: 0x50340, e.g. 0x96960 (TLewZaria, PR-104),
  0x97970, 0x97a98 - the lion-family class tables. 0x50340 is a derived
  override and its only call of 0x549f0 is at 0x50369; before it, it
  zeroes all three block dwords when `[obj+0x106] == 0`
  (0x50345-0x50354) and negates dword[0] when `[obj+0x11a] == 5`
  (0x50357-0x50365). So an object that the drawing pass has not switched
  on (PR-106's slot +0x60 flag) and a lion-family object with that field
  at zero both get a zeroed block - and a zeroed block means no row feed
  and no force, because the block IS the force pair.
- WHAT 0x549f0 WRITES, VERBATIM: `[obj+0xba] = [edx]` (0x549f9),
  `[obj+0xbe] = [edx+4]` (0x54a08), and at its tail `[obj+0xa5] =
  [[edx]+8]`, a copy of the block's own class table (0x54be6-0x54be9,
  ecx still holds edx from 0x549f5). If `[obj+0xd2]` is zero it jumps
  straight to that tail (0x54a0e-0x54a10); otherwise `[obj+0xd2]` is the
  first record of a typed list and the method dispatches on the record's
  kind at `[rec+4]` (0x54a16-0x54a1b):
  - THE LIST ITSELF: those records are exactly what heads `[obj+0xd2]`.
    Two constructors - 0x53f00, called with `lea edx,[obj+0xd2]` pushed
    (e.g. 0x3d60a-0x3d618), and 0x53eb0, called as `add ecx,0xd2; call
    0x53eb0` (0x24672-0x24678) - write `[rec]=0, [rec+4]=1, [rec+8]=edx,
    [rec+0xc]=ebx` and then link the node at `[rec+0x10]`; the link
    writes the head (0x53f48 `mov [ecx],esi`, esi = `rec+0x10` minus the
    node offset) and the unlink at 0x551f4-0x551fc moves the head to the
    node's `next`. The head is also zeroed at 0x5415b and 0x54e88. That
    closes the shape half of PR-102's "what is `[obj+0xd2]`".
  - kind 1 (0x54a1b): writes `[obj+0xba]` only. It reads a facing at
    `[rec+0xc]`, compares it with `[obj+0x6d]` (0x54a24-0x54a2a),
    scales that difference by 15 (`shl 4` minus 1, 0x54a34-0x54a37) and
    compares `[rec+8]` plus that bias against the object's x with a
    5-unit dead band, writing **+10** (0x54a57), **-10** (0x54a82) or
    **0** (0x54a93). This is PR-102's reading, unchanged.
  - kind 4 (0x54aa2): writes BOTH. x from the position getter
    `[[obj+8]+0x68]` (PR-102's) against `[rec+8]` with an 8-unit dead
    band -> `[obj+0xba]` = +10 / -10 / 0 (0x54aab-0x54af3); y from that
    getter's second dword against `[rec+0x1d]` -> `[obj+0xbe]` = +10 /
    -10 / 0 (0x54afd-0x54b4e). If either is non-zero it goes to the
    tail; when BOTH are zero it calls 0x551f0 (0x54b51-0x54b8b), which
    unlinks that record from `[obj+0xd2]` and, when the list has gone
    empty, calls `[[obj+8]+0x3c]` and sets `[obj+0x106] = 1` on a
    non-zero answer (0x55207-0x55223). So a contact record is dropped
    when the object stops moving relative to it, and `[obj+0x106]` is
    set when no contact is left - which is precisely the field 0x50340
    tests before zeroing the block. INFERENCE, flagged: the flag reads
    "no contact, no push", not a proven meaning.
  - kind 2 (0x54b65): `[rec+8]` must be 0 or equal to `[obj+0x5c]`, the
    current STATE index - the same field 0x543a6 indexes the state array
    with (PR-102); then a 12-byte set at `[rec+0x10]` is copied into
    `[obj+0x60]` through 0x49580 and 0x551f0 runs. Purpose unread.
  - WHERE THE AUTHORED VALUES COME FROM in the kind-4 case. This is what
    was missing from PR-98's "written +-10/0 ... NOT .MOV-authored":
    0x245b0 (a method of one class table, 0x9897c) iterates an array of
    12-byte records at `[obj+0x136]` with a count at +0x13e
    (0x24612-0x2465f) and for each entry creates a contact on the
    LION's list - `mov eax,[ds:0xcef40+0x394]` (the lion, 0x24623-
    0x2462e), `add ecx,0xd2; call 0x53eb0` (0x24672-0x24678, which
    writes kind 1) - then patches it: `[rec+4] = 4` (0x24681) and
    `[rec+0x1d] = that entry's third dword` (0x24688), which is the y
    the kind-4 handler compares against; the constructor already put the
    entry's first field in `[rec+8]`. Same shape at 0x3d5a0-0x3d618: it
    sets `[lion+0x106] = sign(lion.x - ([obj+0xf2] + offset))`
    (0x3d5d8-0x3d5f6) and then builds a kind-1 contact on `[lion+0xd2]`
    with that x offset (0x3d601-0x3d618). Whether that `[obj+0x136]`
    array is the authored attach/child data this port joins but never
    instantiates (PR-93's 0,0 entries) is OPEN.
- THE TAIL OF THE SAME METHOD, 0x54b90-0x54be1, settles something
PR-101/PR-107 left open: **the owner-side guard of SEQ_REV_Y is written
by the vertical half of the input-apply.** `if ([obj+0xbe] > 0)
[obj+0x71] = -1; else if ([obj+0xbe] < 0) [obj+0x71] = +1`
(0x54b90-0x54ba5) - and `[obj+0x71] == -1` is the test PR-101 showed
enabling the SEQ_REV_Y row mirror. Then bit 0x10 of `[obj+0xb6]` is set
or cleared EVERY frame by `([obj+0xba] * [obj+0x6d]) < 0`
(0x54bac-0x54bd4) - "the horizontal terrain force points against my
facing" - and while that bit is set the method calls 0x53670
(0x54bd6-0x54be1), which negates `[obj+0x6d]` (flips facing) when the
current state's byte at +0x15 has bit 0x8, and then calls
`[[obj+8]+0x54]`, the same invalidation the SEQ_REV paths use (PR-105).
So the engine can turn the lion around every frame while it walks into a
slope, with the STATE deciding whether that is allowed. Whether
`[obj+0xb6]` & 0x10 is what the climbs' `AC_DUP` tests is NOT
established; it is the strongest candidate found and it goes on the open
list.
- THE CONSUMERS, re-read so this claim is exact: 0x54250's velocity step
(PR-98's) adds `([blk+0] * [obj+0xba])/20 + [obj+0x6d] * [blk+0x10]` to
`v_x` (0x542a9-0x542c7) and `([blk+4] * [obj+0xbe])/20` to `v_y`
(0x542cd-0x542e3), so those two gate registers are ALSO the force pair,
and a signed +-10 times a block dword divided by 20 is a full force, not
a hint. PR-98 recorded them as terrain scalars "written +-10/0 ..., NOT
.MOV-authored"; this record names the writer and the authoring layer
above it. The row-feed bodies only test them for zero, which is why a
BY_TY state advances at the authored row rate while a vertical force is
active and stops dead when it is not.
- CENSUS OF THE SHIPPED BY_TY STATES (script kept out of git at
/tmp/opencode/pr108_census.py, run 2026-11-17): **12 state blocks in 7
files, three state names only** - A_WSP, A_WSP_LIN and TUNGA2's A_DRAB.
KARA.07 LEW.KAR (1, A_WSP_LIN, `seq=0024`), TUNGA.11 LEW.TUN (A_WSP
`seq=climb`, A_WSP_LIN `seq=0007`), TUNGA2.12 LEW.TUN (A_DRAB
`seq=uj31`), and ZARIA.01/ZARIA.02 in BOTH their script variants
LEW.ZAR/LEW2.ZAR and LEW.ZARPL/LEW2.ZARPL (4 each: A_WSP `seq=climb`,
A_WSP_LIN `seq=climb_r`). All of them are the lion on a vine or a rope.
The eleven non-A_DRAB blocks: `control=BY_TY|CHECK_ALL|SEQ_REV_Y` (KARA
adds ANTI_SOLID|FRAME_REV), `F=0,100 T=160,50 G=0,0`, rows `1,300` then
`2,200 ... 9,200 10,300`, or 8 rows at the same ms. A_DRAB:
`BY_TY|FRAME_REV|CHECK_ALL|SEQ_REV_Y`, `F=0,100 T=1000,50 G=0,0`, ten
rows of `*,500`. Entered from A_JUMP / A_JUMP_S / A_FALL_DOWN by
`A_WSP(3)=AC_DRAB_CATCH,AC_ONAIR` and from A_TURN_BACK / A_SCIAG by
`A_WSP(1)=AC_BREAK_NEXT,AC_DRAB`; left by `!AC_DRAB` / `!AC_LINA`, by
`A_TURN_BACK(1)=AC_DUP,K_DOWN`, by `A_PRE_JUMP(1)=K_JUMP`; A_DRAB also
carries the self-line `A_DRAB(x)=AC_DRAB,AC_ONAIR`. **None of the 12
declares `time_scale=`, so every row threshold is the authored ms number
itself** (PR-104's `[node+0x10] = T * time_scale / 1000`, default 1000).
That is exactly why this cannot be resolved from data: the engine's
thresholds (300 for row 1, 200 for the rest) and this port's per-row ms
(300, then 200) are the SAME numbers in different units - the engine's
need PR-106's real step cadence, the port's takes them as milliseconds -
so both models fit the authored file and only footage can say which one
the lion's animation follows. BY_TX: 0 occurrences, re-measured. The
`(x)` row tokens in those scripts are the author shorthand PR-107 handles
(entry 0), not a separate form.
- WHAT THE PORT DOES TODAY, AND WHAT IS NOW PINNED. BY_TY and SEQ_REV_Y
have no LL_CTRL bit, so both land in `st->ctrl_other` and nothing acts on
either; the player's row clock is the subtractive ms accumulator
(ll_play.c ~1179) with no gate and no vertical feed; neither the SEQ_REV
mirror nor the engine's threshold comparison is modelled. No behaviour is
changed here: the suite cannot separate the two readings (no port code
raises AC_DRAB/AC_DRAB_CATCH/AC_LINA/AC_EDGE, so no BY_TY state is
reachable from `--play` at all - which is why the new leg raises its
signals directly, PR-91's way), and the ground rule is that a behaviour
change needs a test that says why. One more difference recorded and NOT
touched: the port advances a row when `p->ft >= ms` on a timer it zeroes,
while the engine compares the POST-ADD accumulator with `jae` and hands
the remainder back (PR-100's 0x52317-0x52338) - so any comparison of row
timings carries a one-step offset.
- TEST (data-free, `ll_play_selftest()`, the always-run `--play-selftest`
leg of verify-play): one synthetic script copied from ZARIA.01's real
A_JUMP -> A_WSP -> A_FALL_DOWN/A_PRE_JUMP/A_TURN_BACK/A_PODCIAG set, with
the authored control words, `F=`/`T=`/`G=` and A_WSP's ten rows verbatim.
Hand-derived expectations, dt = 20 ms: `A_WSP(3)=` enters at list index 2
(drawn frame 3); the cycle from there is 7*200 + 300 = 1700 ms = 85
ticks, so index 3 at tick 10, one row every 10 ticks after that, index 9
(frame 10) at ticks 70 and 84, and the wrap to index 0 at tick 85, which
arms `broke` - the authored AC_BREAK_NEXT arm. x, y, v_x, v_y never move
(500, 500, 0, 0): the port has no vertical feed and `F=0,100`
accelerates nothing. With AC_DRAB held the state's other three exits
stay dark, and on the tick it is not held `A_FALL_DOWN(1)=!AC_DRAB` wins
and the fall starts at the top of its own list. Parse facts are pinned
too: `ctrl_other == 2` (BY_TY and SEQ_REV_Y, both counted, neither
invented), CHECK_ALL present and no BY_X/BY_TIME bit,
`tr[2].c[0].sig == LL_SIG_AC_DRAB` with `neg == 1`, and A_JUMP's entry
row 3 with its AC_DRAB_CATCH condition. Mutation checks, all four seen to
fail the leg: (MUT-A) gating the port's row loop on `p->vy != 0.0` - the
engine's shape transplanted into the port - breaks the schedule and the
`broke` check; (MUT-B) `p->y -= 1.0` per tick - a vertical feed - fails
only the position check; (MUT-C) making `ll_mov_row_start` ignore the
`(N)` token breaks the schedule and `broke`; (MUT-D) giving `!AC_DRAB` a
second term that never fires fails the release-step check and the parse
check. The selftest summary line names the leg, as it names the
PR-83/PR-91 ones.
- CI: not run at the time this record was written; the result for this
commit is reported in the commit message (the workflow is read with
`gh run list --workflow=ci.yml --limit 3 --json
status,conclusion,headSha,displayTitle`, never `-r`).
Local gates on this exact tree:
`make`, `make win` and `ll_level` are warning-free, and `make -k
verify-all DATA=/tmp/opencode/iso_stage/LEO HD=/tmp/opencode/hd_files`
is EXIT=0 ("ALL TESTS PASSED"), as is the fixture-free
`DATA=/tmp/opencode/nope HD=/tmp/opencode/nope` run, with no "fail" line
in either log. No pin moved and none could: this PR changes no
behaviour, and the new leg is the only BY_TY coverage the suite has.
- THE CLIP THAT WOULD DECIDE IT (asking the owner, because the data
cannot): a vine or rope climb - A_WSP on `climb`/`climb_r` art - as
clearly as the footage has it: ZARIA.01 or TUNGA.11, one clip from the
grab to the top or the drop; picture only is enough. The reference
videos are ep.1 xPiXBmPEdXg and ep.2 ti6IYfhh1bI, playlist
PL6B7604DEF5B643F9, and the crop that has worked for measurement is
`crop=640:400:0:40`. TWO questions, easiest first: (1) **Does the
hang/climb animation freeze while the lion hangs without vertical
travel?** The engine's mode-4 guard says yes - no vertical contact
force, so no row feed and no vertical velocity; the port says no, it
runs the authored ms rate continuously. That one observation decides
whether the port needs the gate at all, and it needs no frame-accurate
clock. (2) If it does not freeze, **does a row last the authored ms (the
port) or 2 feed steps at the engine's step period** - PR-106's ~28.6 ms
per frame-loop iteration would make a 200-unit row ~57 ms, about 3.5x
faster than the port's 200 ms. A third check needs no clip: if a steady
climb measures near 10 px per object step (ESTIMATE: 0x54250's
`([blk+4] * [obj+0xbe])/20` with block dword[1] = 10 against a
`T=160`-percent-per-step drag settles at 10 units - PR-98's law), that
is the engine's force model and it fixes the climb speed too.
- OPEN ITEMS (PR-108 1-10): (1) which class defines the authored array at
`[obj+0x136]` (count at +0x13e; 0x2e6a0-0x2e6bd sets a count of 1 and a
value at `[obj+0x13a]`), and whether that is PR-93's attached-child
data. (2) The meaning of `[obj+0x106]` - 1 from 0x551f0 when the contact
list empties, a side-of-object sign from 0x3d5f6, plain 0/1 writes at
0x2700e/0x28991/0x3346f/0x361fe - and what `[obj+0x11a] == 5` is (the
lion-family negation at 0x50357). (3) PR-106 item 1 unchanged: can a
live player's block dword[1] ever be non-zero? 0x58690 (the reader of
the 0xcf400 singleton) and the input-record format are unread, so the
port cannot yet say whether the vertical half of that block is reachable
at all. (4) Who consumes `[obj+0xa5]`, the block's class table. (5) The
`[[obj+8]+0x68]` position getter and the `+0x54` invalidation, per class
- both were assumed here, not read. (6) The default value of
`[obj+0xba]`/`[obj+0xbe]`: no constructor writes them; only 0x549f0 and
the frame loop's zeroing do. (7) Whether `[obj+0xb6]` & 0x10 is the
`AC_DUP` the climbs test, and what the state byte +0x15's 0x8 bit
authorises (the facing flip at 0x53670). (8) ZAR vs ZARPL: two script
variants per ZARIA lion with identical BY_TY blocks - which one the
level loads and why a `.ZARPL` variant exists is unexplored. (9) the
one-step row-threshold offset noted above. (10) A lead, not a finding:
0x3d440 is a method of the lion-family class tables (pointers at
0x96b00 and 0x99edc, in sub-tables reached from 0x1fb1c/0x205b4 =
TLewZaria+0x128) and its one direct call at 0x28682 passes a freshly
zeroed 12-byte block in edx - the same shape as 0x50340's argument - so
it may be the other half of how the lion's block is filled.
- READING NOTES (the method cost): `leodb.py grep` takes its pattern as
the LAST argument, so passing `--limit 40` made it search for the literal
text "0x106] --limit 40" and report nothing - which briefly looked like
an absence of writers. `leodb.py sql "select ... where ops like
'%0x106]%'"` is the reliable form for field-writer searches. A jump table
has to be read as RAW dwords (a `struct.unpack_from` over
build/leo/leo.elf at VA - 0xf000), because disassembling a table's
address range prints instructions, not entries - which is how PR-102 got
the two entry addresses swapped. `python3 dat.py <VA> <len>` stayed the
only authority for code, and every address here came from it or from
that raw read.
- Docs touched: this record; ported/ll_play.c (the new selftest leg and
its summary line); ported/ll_play.h (the vocabulary comment corrected:
five modes, BY_TIME unguarded, BY_TX 0 files, BY_TY 12 blocks in 3 state
names, plus a pointer to this PR's leg); HANDOFF.md (a new item-13
bullet for PR-108, the same wording fix in item 13's PR-102 paragraph and
in 8's "DONE since PR-98" paragraph, the T1 item-(2) paragraph rewritten
to the clip request, a STATUS line under T5, and a 6-table row recording
that the mode-4 feed and its guard are not modelled). Nothing under
.github/workflows, nothing copyrighted staged.

## PR-109 — the row clock's unit, decided by footage: authored row
thresholds count FRAME-LOOP ITERATIONS (100 per iteration, one
iteration = two display frames = 28-33 ms), not milliseconds; the pacer
and the PIT clock read end to end, and two sentences of PR-106's clock
paragraph are wrong (RE + one measurement on the owner's clip: no
behaviour change, no pin moved)

Closes the question PR-108 opened ("only footage decides") and the "what
are 100 units of?" question carried since PR-100, and PR-106's open
item about the clock's rate. No port behaviour changes in this PR: the
row clock still runs on `p->ft += dt_ms`. The behaviour change is
PR-110, named at the end of this record.

- WHAT THE OWNER SUPPLIED, AND WHAT IT IS NOT. `yt-dlp -f
  "bv*[height<=480]+ba/b" -o "ladder.%(ext)s"
  "https://www.youtube.com/watch?v=DomCTOtx2rg"` ->
  /tmp/opencode/yt/ladder.webm; ffprobe: vp9, 576x360, 30 fps, 481.63
  s. Nothing copyrighted is staged in the repo; everything measured here
  stays under /tmp/opencode/yt. In it the lion hangs from a rung ladder
  (rope strops, wooden rungs) from about 46.6 s, climbs steadily, and
  steps off onto a ledge at about 48.3 s. THE LEVEL IS NOT IDENTIFIED
  and I could not identify it from the picture; that does not weaken the
  timing conclusion, because the ladder-grab state block `[A_WSP]`
  (seq=climb) is BYTE-IDENTICAL in ZARIA.01, ZARIA.02 and TUNGA.11, so
  every candidate supplies the same row table. What the picture does
  establish is the object type: a rung ladder, i.e. the DRABINKA family
  (`AC_DRAB`/`AC_DRAB_CATCH`), whose states are BY_TY (mode 4). It is
  NOT ENA2's rope-handover state `[A_CLIMB]`, which is BY_Y (mode 1)
  with rows `1,4 ... 10,4` - a threshold of 4 against a feed of |v_y|
  per step is a velocity-coupled animation, a different mechanism, and
  not what was filmed.
- HOW IT WAS MEASURED (the scripts stay under /tmp/opencode/yt/lad:
  period.py, pose.py, idseq.py, track.py, cam.py, rung.py). Frames at
  the container's own 30 fps (`-ss 43.5 -t 5`, f_0001 = 43.500 s). In
  each frame the lion is masked by colour (r>165, 60<g<185, b<95,
  r-b>110), x is cropped to the ladder column (100..280 of 576) to drop
  the background, and the largest 8-connected component (>=150 px) is
  kept; its pixel count `n` is a sprite fingerprint, because the video
  is a scaled copy of a 320x200 framebuffer, so the same sprite reads
  the same `n` frame after frame (identical sprites agreed within ~3
  px; runs were merged at a tolerance of max(25, 1.5%) to survive the
  lossy codec). A run of k video frames is one animation step displayed
  for k x 33.3 ms. Two independent pose counters were also run: a
  strict IoU change test (pose.py, 0.99) and greedy pose clustering
  (idseq.py).
- THE MEASUREMENT, AND THE 3x. In the continuous climb (46.633-47.900 s)
  the run lengths are 4,2,4,3,2,2,2,3,2,3,2,4,2,2,2: 15 changes in 39
  frames, mean 2.6 frames = 87 ms, MAXIMUM 4 frames = 133 ms. For the
  authored `[A_WSP]` table (`1,300`, eight rows of `200`, `10,300`) the
  two models predict, in video frames at 30 fps:
    - the port's ms model (threshold = ms): 9,6,6,...,6 - a MINIMUM run
      of 6, and a 2200 ms cycle = 66 frames;
    - the iteration model (threshold/100 iterations, one iteration = two
      display frames, so a row of 200 is 2 iterations and one of 300 is
      3): 1.7-2 and 2.6-3 frames depending on the retrace rate (see the
      pacer bullet), and a 22-iteration cycle = 630-730 ms.
  NOT ONE run of 5 or more frames appears anywhere in the 5 s window,
  while the observed runs are 2, 3 and 4. Run-merging cannot rescue the
  ms model: it would need every 6-frame run to be seen as 2. The change
  COUNT taken from the same run data points the same way, though less
  strongly: 15 pose changes in 1.27 s, where the ms model allows at most
  5.8 row advances and the iteration model 17-20 - above the ms ceiling,
  below the iteration one, which is the shape a detector that misses some
  changes produces (it only sees a change when the silhouette changes). A
  third method, the strict-IoU one, over a wider window that also
  contains the grab and the step-off (46.2-48.5 s) reports 8 changes in
  66 frames; it under-counts too much to discriminate and is recorded
  only for completeness. The window before the grab (43.50-44.40 s,
  static camera) gives runs of 3-4 frames for a state whose authored rows
  I did not identify; it is used here ONLY as a control that the method
  can measure 3-4 frame runs at all.
  DECIDED: an authored row threshold T is displayed for T/100 frame-loop
  iterations, NOT for T milliseconds. Measured row durations are
  66-100 ms for T = 200-300, i.e. 0.33-0.50 ms per threshold unit
  against the port's 1.0 ms per unit: THE PORT'S ROW CLOCK RUNS
  3.0x-3.5x TOO SLOW, which is PR-108's own estimate ("about 3.5x
  faster than the port's 200 ms") confirmed.
- STILL OPEN AND NOT CLOSED HERE: the vertical DISTANCE law. Measuring
  his feet against the rungs (rung pitch 12 crop px = 18 source px at
  the 0.667 crop scale) gives about 100 source px/s, ESTIMATE +-20, and
  nothing in the repo turns `F=0,100 T=160,50` into 100 px/s: the port
  has no reading of either number for this state and the row glyph lines
  carry no dy. The timing law is settled, the motion law is not, and no
  future PR may quote the 100 px/s as if it followed from the timing
  law.
- THE PACER, VERBATIM (bytes re-read through leodb `dis` at their own
  VA). The frame loop's tail is 0x4aee0: `call 0x563d0`, `mov eax,ecx;
  call 0x4af10`, `mov eax,ecx; call 0x4b0e0`, `inc ds:0xcef3c` (a frame
  counter), `jmp 0x4ac3b` (the loop head). 0x4b0e0 is six bytes: `mov
  eax,0xcef30; call 0x4a120; call 0x1cfe5; ret`. The only callers of
  0x4b0e0 are 0x4aef5 (that tail) and 0x609e5 (unread, open item 6).
  0x4a120 is the wait method: while `ds:0xcf1b2 - [p+4] < [p+8]` spin
  re-reading both, then ONE `call 0x1dba0`, then restamp `[p+4] =
  ds:0xcf1b2`. 0x1dba0 is exactly `mov dx,0x3da` and two polling loops
  on `in al,dx; and al,8` (first waiting for bit 3 clear, then for it
  set) - the standard VGA 3DA status read, so it ends aligned to the
  start of a vertical retrace: a display-frame boundary really is the
  unit. 0x4a160 is the reset (`mov DWORD [eax+4],0; ret`), called on
  the loop's EXIT path at 0x4ae67 next to `call 0x1dba0`; that exit path
  also re-arms the level when `ds:0xa94f0` (lives) is non-zero (0x4ae82)
  - and the retrace wait at 0x4ae58 belongs to that exit path, not to
  each iteration. THE SETPOINT, confirming PR-106's reading and adding
  the arithmetic: 0x4a190 starts the clock (`mov eax,0xcf1a8; call
  0x5b8f0`, then `call 0x5b860`), takes a factor from the caller (its
  prologue is `mov ebp,eax; mov [esp],edx`), samples FIVE retrace-to-
  retrace intervals, keeps the minimum in ecx and the maximum in esi,
  and writes `[p+8] = min*factor - 1`, or the constant 0x1b = 27 when
  the samples spread by more than 2. The factor is 2 at both call sites:
  0x5e49c-0x5e4b0 and 0x5eae0-0x5eafd each do `mov eax,0xcef30; mov
  edx,2; call <video-mode setter>; call 0x4a190`, and 0x4a230 preserves
  edx across its body (push/pop) even though it uses `mov edx,0x13` for
  its `INT 10h, AX=0x13` mode-13h call; 0x4a270 is the matching
  mode-0 setter. The second calibration site, 0x5eae0, then writes
  `ds:0xa94f0 = 9` (lives) and `call 0x5d170`: the calibration runs
  immediately before the level runs. So ONE FRAME-LOOP ITERATION PER TWO
  DISPLAY FRAMES, by construction, and the 27 fallback is exactly
  2*14-1: the author's assumed 14 ms retrace.
- THE ITERATION PERIOD, AS INFERENCE AND NOT RE. Two display frames per
  iteration means the loop rate is half the retrace rate: 35/s at mode
  13h's 70.08 Hz (PR-106's figure, 28.6 ms), 30/s at a 60 Hz retrace
  (33 ms). The clip brackets it: rows of 200/300 show plateaus of 2-4
  video frames at 30 fps, i.e. 1.7-3 iterations' worth of display, which
  fits 28-33 ms per iteration and excludes both a much faster loop (a
  100/s loop would change the pose every video frame) and the ms model.
  The absolute value cannot be pinned from a 30 fps copy of a 50/60/70 Hz
  original, so it stays ESTIMATE 28-33 ms. Note the consequence, which
  the port has to face: the setpoint is measured at runtime, so the
  game's animation speed is display-rate dependent.
- THE CLOCK, END TO END, AND THE TWO SENTENCES OF PR-106 THAT ARE
  WRONG. The counter ds:0xcf1b2 is advanced at exactly one place, 0x1c65d
  `add DWORD ds:0xcf1b2, ds:0xcf1b6`, in the tail of the timer
  dispatcher 0x1c5b0. Its caller chain is the installed INT 8 handler:
  0x1c6d0 is `pusha; push ds/es/fs/gs; call 0x74fb8` (a DS fixup,
  `data16 mov ds, cs:[...]`), `mov al,0x20; out 0x20` (EOI), then a
  re-entrancy flag at ds:0xcea94 guards one `call 0x1c680`; 0x1c680
  saves ss:esp, switches to the alternate ISR stack at
  ds:0xcf450/0xcf454, `call 0x1c5b0`, and restores - it does NOT chain
  to the interrupted BIOS handler. 0x1c5b0 first walks the registered
  handler list at ds:0xcf1bc - per handler skip if `[h+8] >= now`, skip
  if `[h+0x10] & 1`, fire if `[h+8] + [h+0xc] >= now`, then `call
  [[h+0x14]+0]` - and only then adds the step ds:0xcf1b6. The clock is
  the static struct at 0xcf1a8: +0x00/+0x04 the saved INT 8 vector,
  +0x06 the saved handler pointer, +0x0a the counter (ds:0xcf1b2), +0x0e
  the step per tick. 0x5b860 starts it: bails if already started
  (0x5b840), saves the INT 8 vector through 0x75c5a, installs 0x1c6d0
  through 0x75c8c, programs channel 0 through 0x5b820 (`mov al,0x36; out
  0x43`, then lo/hi `out 0x40`), zeroes `[c+0x0a]` and writes
  `[c+0x0e] = 0x37 = 55`; that 55 is the BIOS tick's 54.93 ms rounded
  up, so the default configuration is also a millisecond clock, one that
  counts BIOS ticks and stamps each as 55 ms. 0x5b8f0 stops it (restore
  the vector, clear it, `call 0x5b820` with edx = 0). CORRECTION 1,
  quoted: "the clock only moves in 55-unit steps" and "the game's 'ms'
  clock = one BIOS timer tick stamped as 55 ms" - true only of a started
  but never-reprogrammed clock, and the level-run path always
  reprograms it. 0x5b9e0 is a SET-PERIOD method in milliseconds, not a
  "N ticks" setter: clamp edx to >=1, `imul edx,edx,0x1d216` (119318)
  and `idiv 100` -> the PIT divisor for a period of edx ms; a divisor of
  0xffff is replaced by 0 (a period request beyond ~549 ms falls back to
  the default rate); then it re-stamps
  the rate the hardware will actually deliver, `[c+0x0e] = (divisor*100
  + 99) / 119318` (1 for any divisor below 1193.18); `cli`, store,
  `call 0x5b820`, `sti`. 0x5b820 has exactly three call sites, all
  inside those three methods (0x5b8c7, 0x5b920, 0x5ba32). It writes
  `al=0x36` (channel 0, latch lo/hi, square-wave mode), so edx = 0 means
  a count of 65536 = the BIOS 18.2 Hz rate - which is what the stop
  method restores and what the 0xffff clamp lands on, and the step it
  computes there, (0xffff*100+99)/119318 = 54, makes that rate a
  millisecond clock too (54 per 18.2 Hz tick = 983/s). The design is
  self-consistent: whatever the tick rate, the counter is milliseconds,
  and set-period only chooses the granularity. THE ORDERING
  THAT SETTLES THE PLAYED RATE: the level-run wrapper 0x5d170 opens with
  `mov eax,0xcf1a8; xor edx,edx; call 0x5b860` (0x5d18b) and `mov
  edx,1; mov eax,0xcf1a8; call 0x5b9e0` (0x5d190-0x5d19a), and 0x5dec0
  is the same start-then-set-1ms pair as a helper, called at 0x5e4b7
  right after the pacer calibration. Every path into 0x4abd0's frame
  loop goes through 0x5d170, so during play channel 0 runs at divisor
  1193 = 1.0006 ms with a step of 1: the played clock is a
  millisecond clock with millisecond resolution, and the pacer's
  `[p+8]` is a count of milliseconds (about 27-33 for two display
  frames). The other channel-0 writer is a BIOS-tick path: 0x18870 saves
  INT 1Ch into ds:0xc4030/0xc4034, installs 0x18730 there, stores 1 at
  ds:0xc3fe4, and writes `al=0x36`, lo `0x95`, hi `0x42` = divisor
  17045 = 7.00 Hz; it is reached from 0x18c40@0x18cb9 <-
  0x1a450@0x1a462 <- 0x1b800@0x1bb90 (conditional on `[ebp+0x9c] == 0`)
  <- 0x5cfc0@0x5d022, and 0x5cfc0 is called by the level setup 0x5d050
  at 0x5d0de/0x5d12e. On the retry path the per-level wrapper 0x5d3a0
  calls 0x5d170 FIRST (0x5d3b0) and reaches the setup 0x5d050 only for
  result code 3 (0x5d3c6-0x5d3d7, via 0x5da60), so the 1 ms write is
  again the last one before the next frame loop. CORRECTION 2, quoted:
  PR-106's "the sound driver's 70.0 Hz PIT" - that PIT write is
  divisor 0x4295 = 17045, i.e. 7.00 Hz, a factor of ten off; and
  because the level-run path reprograms channel 0 to 1193 afterwards,
  that 7 Hz rate is not the played rate. Independently, the clip
  supports the fast clock directly: at a 7 Hz tick the calibration would
  measure 0 between retraces, `[p+8]` would be -1, the spin would never
  wait and the loop would run unthrottled, which the footage is not.
  The 0xcf1bc handler list is a separate mechanism and not part of the
  state machine: it has exactly ONE add site (0x562a1 -> 0x5ba40) and
  its four remove sites (0x55dd0, 0x5631c, 0x56922, 0x56aba) lie inside
  the same 0x55d70-0x56c60 range that owns the per-frame input block -
  an inference from address ranges, not a read of the handler body.
- THE ONE-LINE ANSWER TO PR-100'S QUESTION 11, quoted to be corrected:
  "the field is a level-level global with exactly one write in the whole
  image, `mov [eax+0x388],0x64` at 0x4c8b7 - a constant 100 - and ...
  the question narrows to 'what are 100 units of?'". They are per-
  frame-loop-iteration increments of the object's row accumulator at
  `[obj+0xa1]`: the mode-3 and mode-4 bodies (0x543f2, 0x5441b) add the
  constant once per iteration behind their gates and the stored row
  thresholds are compared against that accumulator, so an authored `200`
  is two iterations and `300` is three. The constant and the readers of
  the global (0x239de, 0x54406, 0x5442f; 0x543fb is the ungated mode-2
  entry) are PR-108's RE facts; the unit is now measured.
- WHY THE CLIP'S EXISTENCE WAS ALREADY EVIDENCE, spelled out because it
  is the shortest argument here and the long ones only corroborate it:
  `[A_WSP]` has `control=BY_TY`, whose feed is that constant 100 once
  per iteration behind `[obj+0xbe] != 0`, and the clip is a climb
  animation that visibly advances. Under the port's model the row clock
  has no gate at all, so the climb animation advances with or without
  vertical force; under the engine's, if `[obj+0xbe]` were 0 during a
  climb the animation would freeze. A climb was filmed advancing, so the
  guarded feed is what runs - and its durations, divided by 100, are
  what the video shows.
- CORRECTIONS TO PR-108, quoted (its record stands untouched). PR-108:
  "the authored rows ARE the engine's thresholds (the model difference
  survives). So the choice is a physics/animation question that only
  FOOTAGE can answer, and this PR asks for it" - answered: the footage
  decides the clock, and it decides the ITERATION model. Its T5
  question 2 predicted "~57 ms (2 feed steps at PR-106's ~28.6 ms)" for
  a 200-unit row: CONFIRMED within the noise (measured 66-100 ms for
  T = 200-300, i.e. 57-100 ms for those two rows at 28-33 ms per
  iteration) - although the 28.6 ms it leaned on came from a clock
  paragraph that is wrong in its wording (above) and right in its
  arithmetic. Question 1 ("does the animation freeze while the lion
  hangs without vertical travel") is NOT answered by this clip: the
  window contains no hang without travel, he climbs continuously from
  the grab to the step-off.
- WHAT THIS DOES NOT CHANGE. `time_scale` stays a parse-time scale on
  the stored threshold (PR-104: 0x51850 -> node+0x10 = value *
  time_scale/1000), so an authored 200 with `time_scale=2000` stores 400
  and is 4 iterations - KADMAR.07's balloon rows are 4 and 6
  iterations, not 2000 ms apart. The port's `ll_mov_fr.ms` field is now
  a MISNOMER (a threshold in 1/100-iteration units); it is
  deliberately NOT renamed here, because the rename belongs with the
  code that changes what it means. Nothing here touches BY_X (mode 0) or
  BY_Y (mode 1) thresholds, keyed on dx and |v_y| and carrying no
  timing claim at all.
- PR-110, THE BEHAVIOUR CHANGE, NAMED SO IT CAN BE REVIEWED AS A
  CHANGE: make the row clock an iteration clock. (1) Law: a row is
  current for `authored * time_scale / 100000` iterations; the engine
  feeds exactly 100 per iteration, and the comparison stays AFTER the
  add with the threshold subtracted on a hit, so the remainder carries.
  (2) The port has no iteration concept except its play tick, and the
  engine's iteration is two display frames. So PR-110 must decide, and
  say so: either declare one play tick to be one iteration (feed 100 per
  tick; on the port's fixed 20 ms grid a row of 200 would then last
  200 ms, still 2-3x slower than the footage), or name an
  `LL_ITER_MS` constant and feed `100 * dt_ms / LL_ITER_MS` per tick,
  which reproduces the measured wall durations. The value has a source
  in the image itself - the engine's own fallback setpoint is 27, which
  at the played 1 ms clock is 27 ms per iteration - and the clip's
  bracket is 28-33 ms, so 28 is the honest choice; the choice and its
  derivation belong in that PR, not here. (3) The test must be
  data-free: hand-derived from the authored A_WSP table that rows 1-10
  span 3,2,2,2,2,2,2,2,2,3 iterations and that the 10-row cycle closes
  after exactly 22 steps and never 23. (4) Mutations, in PR-108's
  style: MUT-A a fixed 2 iterations per row (breaks the 300 rows),
  MUT-B ignore `time_scale` (breaks the KADMAR.07 4x case), MUT-C keep
  the ms feed (breaks the 22-step cycle). (5) Re-derive by hand, do not
  scale, every leg that quotes a row duration: ll_play.c ~1540 and
  ~1571, ll_level.c:1136, and the BY_X walk feed in obj_walk, whose
  `facc` model is a different law again (PR-98's). (6) The climb's
  vertical distance law stays open: PR-110 must not invent 100 px/s.
- OPEN ITEMS (PR-109 1-6). (1) The vertical distance law of a BY_TY
  climb (measured ~100 source px/s; no repo reading explains it). (2)
  T5's question 2 (does the climb animation freeze with no vertical
  force?) is still unanswered - this clip has no hang-without-travel;
  what is needed is footage that includes one, or the hold at the top
  rung before the step-off. (3) The real display rate, hence the real
  iteration rate: the setpoint is measured at runtime, so a 50 Hz or
  70 Hz display changes the animation speed by +-40%, and the port must
  state which it reproduces. (4) Whether anything reprograms PIT
  channel 0 DURING play: 0x18870 installs the INT 1Ch handler 0x18730
  and writes divisor 17045 = 7.00 Hz, and 0x188c0 branches on
  ds:0xc3fe4; if that runs mid-level the clock's rate changes under the
  pacer. (5) The `[obj+0xa1]` accumulator's width, saturation and other
  readers. (6) The second caller of the pacer, 0x609e5, and the loop
  behind it.
- TESTS AND GATES: no behaviour change, so no new pin. Every gate was
  run on the committed tree exactly: `make`, `make win`, `make ll_level`
  warning-free; `make -k verify-all DATA=/tmp/opencode/iso_stage/LEO
  HD=/tmp/opencode/hd_files`, and the data-free pair with DATA/HD
  pointed at an empty directory - both EXIT=0.
- DOCS: this record in NEXT_STEPS.md; HANDOFF.md (the 6-table BY_TY row
  rewritten to the decided clock, a new item-13 bullet for this PR, the
  T1 item-(2) paragraph changed from "the request to the owner" to the
  answer, and the T5 STATUS rewritten to what the clip settled and what
  it still owes). Three code comments that asserted the ms row model are
  corrected in place: ll_play.h's control-word vocabulary note -
  including the wording "the lion hanging on a vine or rope", which is
  wrong: A_WSP/A_WSP_LIN/A_DRAB are the rung-ladder family with
  seq=climb/climb_r, and ENA2's rope state is A_CLIMB, mode 1 - and the
  row-clock comments in ll_play.c section 7 and ll_level.c's obj_advance,
  each now stating that the engine counts iterations (100 per
  iteration, one iteration = two display frames) and that this port
  still counts ms until PR-110. Nothing under .github/workflows,
  nothing copyrighted staged.

## PR-110 — the row clock becomes an ITERATION clock: `LL_ITER_MS` = 28,
one accumulator fed BY MODE the way 0x543a2's dispatcher does (a walking
state's rows are fed by travelled distance ONLY, a time-driven state's
distance accumulator stays 0), `time_scale=` implemented at parse time,
and every row-duration pin in CI re-derived by hand rather than scaled
(the behaviour change PR-109 named; PR-104's time_scale sentence and two
of PR-109's are corrected, and one stale PR-102 code comment with them)

This is the PR-110 that PR-109's record named (NEXT_STEPS.md:8188-8213).
It changes behaviour, so it moves pins, and the interesting part of the
review is not the numbers but where each one comes from: none of them is
the old number multiplied by a ratio.

- WHAT CHANGED, FILE BY FILE. `ll_play.h`: `LL_ITER_MS` (28u, :466),
  `ll_mov_fr.ms` -> `.thresh` (:244), `ll_mov_st.time_scale` +
  `.scale_at` (:381, :400), `ll_player.row_t` / `.row_rem` / `.facc`
  (:499-:529). `ll_play.c`: the `time_scale=` branch and the block's
  scale post-pass (:671-:694, :735-:737); section 4's player-side
  distance feed and its mode condition (:1103-:1119); section 7's
  mode-gated row clock (:1275-:1319); both accumulators zeroed on a
  state change (:960-:963, :1211-:1212). `ll_level.c`: `obj_row_tick`
  (:1145-:1152),
  the same mode gate in `obj_advance` (`walk0`), and the renames.
  `Makefile`: the legs that quote a row duration, each with its
  derivation written out beside it.
- THE LAW IT IMPLEMENTS, ALL OF IT ALREADY ON RECORD. PR-99 read the
  dispatcher (NEXT_STEPS.md:5439-5447): one accumulator `[obj+0xa1]`,
  fed by the state's mode - mode 0 BY_X adds `|[obj+0xca]|`, mode 1
  BY_Y `|[obj+0xce]|`, modes 2/3/4 the global
  `[[0xcef40]+0x388]`, modes 5-7 nothing. PR-100 read the rest
  (:5660-5680): that global has exactly one writer, 0x4c8b7, and it is
  the constant **100**; the row comparison is `[node+0x10]` against the
  accumulator at 0x52320-0x5232b, and on a hit the remainder
  (`sub ebx,eax`, 0x52334) is handed to the row handler, so the
  accumulator is NOT zeroed. So: one accumulator, one feed source chosen
  by mode, 100 per iteration for the time modes, distance for BY_X,
  remainder carried. `walk = (st->ctrl & LL_CTRL_BY_X) && st->fx != 0`
  is that gate; `if (!walk)` feeds the time accumulator and `walk ?
  facc : row_t` is what the comparison reads. `ll_level.c` has modelled
  the split since PR-98 (`obj_walk` / `obj_advance`); PR-110 puts it in
  the player too, which is the scope extension in this PR - a walking
  lion state's row clock now does not run at all, and its frames move
  on travelled distance alone.
- `LL_ITER_MS = 28`. The port has no iteration concept of its own (its
  play tick is a fixed 20 ms), so the constant is the decision PR-109
  said had to be made and stated. Two sources, both already measured:
  the engine's own fallback pacer setpoint is 27 (PR-106, read as 27 ms
  per iteration at the played 1 ms clock), and the owner's ladder clip
  brackets one iteration at 28-33 ms (PR-109). 28 is the low end of the
  clip's bracket and the closest number to the setpoint that is inside
  it, so it is the honest choice - and it is a CONSTANT, not a fit: the
  accumulator counts 1/100-iteration units and `row_rem` carries whole
  milliseconds, so the wall-clock speed is the only thing the constant
  touches, and the row counts do not depend on the step size at all.
- THE FEED AND THE CARRY. `row_rem += dt_ms`, and every whole
  `LL_ITER_MS` adds exactly 100 to `row_t` (Bresenham: nothing is
  multiplied by dt and nothing drifts). The compare stays AFTER the add
  and the threshold is SUBTRACTED on a hit, which is the engine's shape
  (above), and it is what makes a row of 300 exactly 3 feeds and a row
  of 200 exactly 2, at any step size. On the CI's 20 ms grid that means
  the m-th feed lands at step `ceil(28m/20)` and `floor(20k/28)` feeds
  have happened by step k - the identity the RC test below asserts, step
  by step, for both accumulators (`row_t == 100*floor(20t/28)` minus
  the closed thresholds, `row_rem == 20t mod 28`). The port's loop is a
  `while` with a `4 * nfr` guard; the engine admits at most ONE row per
  record step, which is open item 1.
- `time_scale=` AT PARSE TIME (PR-104's reading, now in the code). The
  row node's ctor scales every threshold it builds: 0x51679 `imul`,
  0x51683/0x5168d `idiv 0x3e8`, 0x5168f store - `thresh = value *
  time_scale / 1000`, default 1000, and only rows built AFTER the line
  see it (PR-104's ctor-order reading), hence `scale_at = s->nfr` when
  the line is read and the post-pass `for (r = s->scale_at; ...)`.
  KADMAR.07's balloon rows are 4 and 6 ITERATIONS, not 2000 ms. Two
  honest notes. (a) The first version of the branch read the value at
  `line + 10`, which lands on the `=`; every shipped line joins with
  `=`, so the value now starts at `strchr(line, '=') + 1` and the
  fixture asserts the parsed scale, not just the effect. (b) The census
  (`/tmp/opencode/pr110_census4.py` over the 777 cleaned .MOV texts):
  120 `time_scale=` lines in 38 files, 4 of them commented out
  (`;time_scale=`, which never matches), 116 active; values 60..4000,
  26 distinct, 78 of them BELOW 1000 - so the shipped scale mostly
  makes rows SHORTER, and it is not all round hundreds (150, 550, 650,
  750, 820, 850 all appear). Not one active line sits after a row in
  its own state, so `scale_at` records the position rule without
  changing any shipped result.
- WHAT MOVED ON SCREEN, AND WHERE THE NEW NUMBERS COME FROM. Every leg
  that quotes a row duration was re-derived from the row list and the
  feed law, then confirmed against a fresh log; the Makefile comment
  beside each leg carries its arithmetic. The `--play-selftest`
  fixtures (ZM/JM/JE/FK/GL/PK/HR/CE, the PR-108 BY_TY leg, the
  PR-69/107/90 legs) were re-derived on the 28 ms grid; the object-side
  legs in `obj_state_selftest` and `obj_motion_selftest`
  (`--mov-selftest`) on the object row clock. In
  verify-play: solid contact 28, enemy contact 186 (energy 85), the lion
  attack kills at 180 with its window entry+26..+47, DANGER arms at 171
  with its window entry+23..+45 (LICHA's [A_ATAK] is 8 rows of 400 = 16
  feeds), hit 199, react 200, A_DRAP 226, the two later contacts 238 and
  268; the idle lion holds `frame 10 pos 50,648` at ticks 1 and 100;
  the key leg reaches `frame 4 pos 88` at 99 and A_TURN at 132/135; in
  the left-walk leg A_TURN enters at 130, its five rows of 200 are 10
  feeds and the wrap that arms AC_BREAK_NEXT lands at 143, so the hub
  hand-off is at 144 and A_WALK starts at 145; the jump leg's apex is
  110, its row boundaries are steps 1,2,4,5,6,9,12,25,27,... of the
  authored 75,75,75,100,150,150,225,900,225,0+WAIT,150 list
  (2125 units), frame 6 spans 112-124 and the list wraps at 132
  (behind the `8,0,WAIT AC_DUP` row's own gate); that leg's PPM tick
  is 133, which is where its own `A_WALK(7)=AC_BREAK_NEXT` re-join
  prints authored frame 4 - ZARIA's walk list is ROTATED, so row index
  6 is frame 4, and the `(N)` token is the target's start row, on real
  data; and the next row closes on DISTANCE, at exactly 6 px of travel
  (x 90 -> 96) as `frame 5 pos 96,649` at 152. The fight leg's frames
  5/8/15 land at 62/70/100 on the A_FIGHT row boundaries at ticks
  54,58,62,66,70,75,80,87,94,100; TUNGA's walk
  prints `frame 11 pos 60,370` at 30. In verify-level the strum fixtures
  land at `frame 2` and `frame 3` (`floor(40t/28)` feeds). Two script
  changes came with the re-derivation, both in the direction of testing
  the same claim, not weakening it: the lion-attack leg moved its punch
  from tick 74 to 150 so the kill still lands INSIDE the window the
  .MOV's own ATC_ON/ATC_OFF frames open, and the idle leg no longer
  claims an idle-drop (`A_WALK -> A_DRAP`) that was a property of the ms
  clock - on the iteration clock ZARIA's A_WALK rows do not time out,
  which is the correct reading of a BY_X state, so that `! grep` is a
  stronger pin than the old one. A_DRAP stays pinned through the
  authored attack chain (the DANGER leg, 226).
- TESTS. New in `--play-selftest`: the RC fixture (`BY_TIME`, 5 rows
  300/200/200/200/300 = 1200 units = 12 feeds, data-free, no authored
  data in git) walked on TWO grids - 28 ms (one feed per step: closes
  at 3,5,7,9,12) and 20 ms (closes at 5,7,10,13,17) - with the exact
  carry identity checked after every step, and a wrap check; a
  `time_scale=2000` fixture (rows before the line unscaled, after it
  doubled: 300+400+400 = 11 feeds, closing at 3,7,11) with the same
  list without the line as its control; and the ZM leg, which is the
  mode gate itself under test and the sharpest evidence in this PR. It
  reads the accumulators at both ends of the gate: with no key held in a
  BY_X state `facc`, `row_t` and `row_rem` all stay exactly 0 for 17
  steps, and with the key held the list runs on travelled distance alone
  - `fi` stays 0 until 6 px of travel and then closes exactly one row
  (ZARIA's own authored threshold), where a time feed on that same
  20 ms grid would have cycled the 6-unit list inside the first step.
  Measured with the scratch probe `/tmp/opencode/zm_probe.c`, that same
  fixture at HEAD (f86280c, the ms clock PR-109 shipped) runs 3 rows per
  20 ms step, raises AC_TIME inside tick 1 and enters A_DRAP at tick 2.
  The RC loops also read `facc` at every step - in a BY_TIME state it
  stays exactly 0 - and the ZM walk loop reads the time accumulator the
  same way. Seven mutations, each rebuilt and re-run on this tree, are
  caught. `mut_ms`
  (accumulator in ms) fails the jump leg at `air == 18`; `mut_fixed2`
  (a fixed 200 as the threshold) fails `top > 75.69 && top < 75.71`;
  `mut_noscale` (the post-pass disabled) fails the parse assertion
  `fr[1].thresh == 400u`; `mut_nodrift` (`row_rem = 0`, the remainder
  dropped) fails `land == 19`; `mut_bothfeed` (section 7's time feed
  unconditional) fails the ZM invariant
  `s.facc == 0.0 && s.row_t == 0u && s.row_rem == 0u`; `mutl1` (object
  clock in ms) fails
  `object state selftest: ... (state 0 frame 0 t 28)`; `mutl2` (object
  time feed removed) fails the same check at `t 0`. PR-109's three named
  mutations all shipped, under these names: its MUT-A (a fixed 2
  iterations per row) is `mut_fixed2`, its MUT-B (ignore `time_scale`)
  is `mut_noscale`, its MUT-C (keep the ms feed) is `mut_ms`. An eighth,
  `mut_nogate4` (section 4's own
  `if ((st->ctrl & LL_CTRL_BY_X) && st->fx != 0)` removed, so the
  distance feed runs in every mode), SURVIVES `--play-selftest` on this
  tree, and the reason is worth stating: `facc` is only ever read
  through the walk gate and is zeroed at every state change, so no
  current leg can observe the feed-site condition. The half of the mode
  split with behavioural consequences is the read site, and that is what
  `mut_bothfeed` catches; the feed-site condition stays as
  belt-and-braces for 0x543a2's shape.
- CORRECTIONS. (1) PR-109's PR-110 plan, item (3): "hand-derived from
  the authored A_WSP table that rows 1-10 span 3,2,2,2,2,2,2,2,2,3
  iterations and that the 10-row cycle closes after exactly 22 steps and
  never 23". The spans and the 22 are right arithmetic for that table,
  but a data-free test cannot quote authored data in git, so the test
  that shipped is the RC fixture above: the shipped fixture is 5 rows
  summing to 1200 units, 12 feeds, and no shipped test measures 22.
  (2) PR-104: "This port reads `time_scale=` into `s->field_other` (an
  unmodelled counter, ll_play.c line ~624) and treats `f->ms` as a
  duration directly (`o->t < f->ms`, ll_level.c:1136)". Both halves are
  superseded by this PR: the scale is applied at parse time and the
  field is named `.thresh` because it is a threshold, compared against
  the accumulator, not a duration. (3) The same census sentence in a
  code comment (ll_play.c, PR-102's RE note): "all 120 shipped values
  are multiples of 100, 400..2000" - 120 lines is right, 116 are active
  and they run 60..4000, mostly below 1000. Corrected in place.
- OPEN ITEMS (PR-110 1-9). (1) The engine admits at most ONE row per
  record step (0x5232b jumps to the epilogue); the port's loop takes
  several, bounded by the `4 * nfr` guard. With the feed at exactly 100
  and a threshold below it, "one per step" and "all that fit" differ -
  170 of 18617 authored rows have threshold 0 (73 files) and 1715 have
  threshold in 1..99 units after scaling (111 files), so this is not
  hypothetical. (2) The boundary at exact equality: 13932 of the 14881
  authored BY_TIME rows are exact multiples of 100 and the feed is
  exactly 100, so whether a row fires at `acc == thresh` (the port) or
  only above it (the literal flag direction of `39 d8` / `7d 27` read
  AT&T-style, which also makes the `sub ebx,eax` remainder negative) is
  a systematic +1-feed difference. PR-100's reading - threshold first,
  "return with the debt kept" - is the coherent one and is what the
  port implements; the two readings are not equivalent, and nothing in
  the repo or the clip separates them. (3) The engine's mode-0 feed
  adds `|[obj+0xca]|` = the truncated `v_x/100` of that tick (PR-99's
  record, 0x54365); the port's section-4 feed adds `|vx| * dt`
  untruncated. At the lion's own authored T=20 cap the engine's term
  truncates to 0. That is PR-98/99/100's pacing tension, unchanged by
  this PR. (4) PR-109 wrote "Nothing here touches BY_X (mode 0) or BY_Y
  (mode 1) thresholds" (NEXT_STEPS.md:8185-8187) - PR-110 does touch
  them, and the `fx != 0` half of the gate has shipped data behind it
  and no test in front of it: 105 state blocks in 69 files are BY_X,
  and 30 of them (290 rows, 21 files - ZARIA's own [A_ZABLOK] among
  them) declare `F=0,0`, which the port then treats as time-fed. No leg
  in the suite enters such a state, so which half of that gate the
  engine really keys on is untested. (5) The ordering of the
  record step and the merge inside one iteration (PR-100's open item 7)
  is why the port's entry step already accrues a feed, which is what
  makes every play leg's hit land at `entry + ceil(28m/20)` counting
  the entry step; nothing proves the engine accrues on the entry step.
  (6) Whether an armed AC_TIME outlives its own step is PR-83's open
  item and PR-110 did not settle it; the legs show how sensitive the
  answer is: in the press/release leg, releasing the walk key at step 18
  or 35 drops the lion into A_DRAP (a `GEN AC_TIME` row completes
  during the brake, after the key is up) and releasing at 22, 26 or 30
  does not. (7) ZARIA's
  [A_WALK] rows 10, 11, 3, 4, 5 carry `GEN AC_TIME` while the same
  state declares `A_WALK(x)=K_FWD`; no leg distinguishes the two paths.
  (8) The 170 rows with threshold 0: the port's compare-after-add makes
  them close on every step that feeds, and the guard exists, but no
  sample shows what the engine does with one. (9) PR-109's items 1-6
  (the BY_TY vertical distance law, the real display rate, PIT
  reprogramming, the accumulator's width, the pacer's second caller) are
  untouched.
- TESTS AND GATES. Run on the committed tree, exactly: `make`,
  `make win`, `make ll_level` warning-free; `./ll_level --play-selftest`
  and `./ll_level --mov-selftest` EXIT=0; `make -k verify-all
  DATA=/tmp/opencode/iso_stage/LEO HD=/tmp/opencode/hd_files` EXIT=0;
  the same with DATA and HD pointed at an empty directory EXIT=0 with
  its 23 SKIPs; never two verify runs at once. PR-109's CI run
  44148176755 is now `completed / success` including the windows-smoke
  job after its rerun, so the clock work sits on a green base.
- DOCS: this record in NEXT_STEPS.md; HANDOFF.md (section 6's row-clock
  row of the estimates table rewritten as the mode law, the item-0
  sentence that said "frames advance on ms", a section 7 item-13 bullet
  for this PR, and section 8's T5 status). Nothing under
  .github/workflows, nothing copyrighted staged.

## PR-111 — the reference captures move onto disk and into the docs:
`reference/video/` (gitignored) holds ep.1, the ladder clip and a full
5 h 39 m playthrough; their geometry is re-measured rather than assumed;
the owner's level-by-level map of the playthrough is recorded; and a new
ground rule says this tree is local - the `lin`/`wrt` remote-host skills
are not used here (docs + .gitignore only, no code or Makefile change)

- WHY. The owner supplied two gameplay videos and asked for them to be
  kept and annotated as verification reference, plus the clip PR-109's
  conclusion rests on, and separately asked that the docs say the
  remote-host skills are not to be used in this project. Until now the
  captures lived only under the tmpfs `/tmp/opencode/yt/`, so a reboot
  has been losing them, and the only long-form capture of the game was
  not in the project at all.
- WHAT IS STORED WHERE. `reference/video/` in the working tree:
  `xPiXBmPEdXg.mkv` (playlist ep.1), `ladder-DomCTOtx2rg.webm` (the
  rung-ladder clip of PR-109), `OvbbJTmp79I.mkv` (the playthrough),
  each with the `.info.json` and `.description` yt-dlp wrote beside it.
  `.gitignore` gained a block excluding `reference/video/` (the videos
  and their metadata files are all copyrighted); checked with
  `git check-ignore -v`, which reports `.gitignore:78:reference/video/`
  for `reference/video/OvbbJTmp79I.info.json`. Frames, crops and
  scripts cut from them stay under `/tmp/opencode/` as before, so rule 2
  of HANDOFF section 2 keeps its meaning; only the location of the
  videos themselves changed. Nothing copyrighted is staged: the diff is
  `.gitignore`, `HANDOFF.md` and this file.
- MEASURED, NOT ASSUMED (ffprobe on the files, plus frame probes in
  `/tmp/opencode/refcap/`). ep.1: h264 640x480 at 25 fps + opus,
  414.821 s, 41 034 044 bytes. A 6 s window at t=314-320 gives a
  per-row motion profile that is flat at rows 0..37 and 442..479 and
  steps up at rows 40-41 and down at 439-440, with motion across all
  640 columns - so a GAMEPLAY view is the full width at rows 40..439,
  exactly 2x, and the recipe `crop=640:400:0:40` that section 9 and
  PR-108 quoted is confirmed exact for gameplay. It is NOT the right box
  for the rest of the file: over the window t=118-124 the black bands
  reach to column 36 and row 65, and the frame there is one of the
  game's own animated cutscenes (the `.IS` movies), whose picture is
  pillar- and letterboxed inside the frame - at t=120 a rounded
  cave-interior image with a dog, the lion and a spotted big cat around
  a table, on black. Measured at 4:3 within the game-view band only;
  what its box is in an actual 320x200 Mode X frame is open. The level
  title cards are black-on-black text and sit elsewhere again. A caveat
  no earlier record had. Ladder clip: vp9 576x360 at 30 fps + opus,
  481.633 s, 26 902 029 bytes; the whole frame is the game view at
  exactly 1.8x, no bars, no overlay. Playthrough: h264 1280x720 at
  60 fps + opus, 20389.021 s, 4 774 166 785 bytes; the game view is
  x 0..1151, y 0..719 - exactly 3.6x, no letterbox - and the right
  128 px is the uploader's static cartoon column, measured as a fall in
  per-column temporal std from 19-21 to 2.3-2.8 at x=1152, at t=120,
  3600, 7200, 14400 and 20040 alike. 60 fps (16.7 ms) is finer than
  the 28-33 ms iteration of PR-109/110, which is what the 720p60
  download is for; 1 video px = 0.28 game px, so it is a good clock and
  a poor ruler.
- WHAT THE OLD RECORDS SAID, AND WHAT CHANGES. PR-108's record at
  :7879-7882 said "The reference videos are ep.1 xPiXBmPEdXg and ep.2
  ti6IYfhh1bI, playlist PL6B7604DEF5B643F9, and the crop that has
  worked for measurement is `crop=640:400:0:40`." That stays true and
  is now extended: the playthrough is a fourth, much longer reference,
  and that crop is verified exact for gameplay but wrong for the game's
  own title cards. PR-109's record at :7959-7960 said "Nothing
  copyrighted is staged in the repo; everything measured here stays
  under /tmp/opencode/yt." That is superseded in one part and kept in
  another: still nothing copyrighted is staged, but the captures
  themselves are now in the repo tree (gitignored) instead of the
  tmpfs; what stays under `/tmp/opencode/` is what is cut from them.
  HANDOFF section 3's old row "reference video |
  `/tmp/opencode/yt/gp.mp4` (episode 1), `ep2.mp4` (episode 2); the
  playlist is below" is replaced by a "reference captures" row plus the
  R1/R2/R3 block that documents each file, its yt-dlp line and its
  measured geometry.
- OLD COPIES vs NEW, MEASURED. The ladder file is NOT byte-identical to
  `/tmp/opencode/yt/ladder.webm` (same total size, different sha256),
  but it is the same encode of the same video: the extracted video
  streams are both 17 868 921 bytes with identical codec parameters,
  and 90 consecutive frames decoded from the two files at t=46-49 - the
  climb itself - are md5-identical with mean absolute difference 0.0
  (the audio streams differ). So PR-109's timing numbers transfer to
  this copy unchanged. The same test on ep.1: 75 consecutive frames
  decoded at t=300-303 from `xPiXBmPEdXg.mkv` and from `gp.mp4` are
  md5-identical (mean abs difference 0.0), both 640x480 at 25 fps, so
  every measurement ever made on gp.mp4 is a measurement on R1. The
  containers differ: gp.mp4 reports 414.680 s, the new one 414.821 s,
  the extra ~0.14 s being the audio stream's length in matroska -
  timestamps must come from frame counts, not from the container
  duration. The old `/tmp/opencode/yt/ep2.mp4` was not compared.
- THE OWNER'S MAP OF THE PLAYTHROUGH. YouTube has no chapters and no
  subtitles for it (`chapters` is null in its `.info.json`), so until
  now it could only be scanned. The owner supplied a level-by-level
  time map, recorded verbatim in HANDOFF section 3 (zaria 1 at 0:00,
  zaria 2 at 7:77, ENA 1 at 13:17, ... TUNGA 2 at 4:42:29, MENU at
  5:37:54). Its labels are the game's own words: the 13 `ETAPY` blocks
  listed by the PR-46 game-wide measurement (NEXT_STEPS.md:2024-2029,
  and the "all 13 .des" count at :2016) are ZARIA.01, ZARIA.02, ENA.03,
  ENA2, LONA.05, SADES.06, KADMAR.07, KADMAR2, RAN.01, KARA.07,
  TUNGA.11, TUNGA2.12, JUNCTION, and LEVELDES records are named after
  the chapter (ZARIA1.DES, ENA1.DES, KADMAR1.DES), which is why
  "KADMAR 1" and "TUNGA 2" are names from the data and not a viewer's
  guess. Only JUNCTION has no entry. The map is approximate and has
  defects that are recorded rather than fixed silently: `7:77` is not a
  valid mm:ss, and `43:59 ENA 2` precedes `45:10 ENA 1`. Both frames
  were read: at 45:13 the game shows its own "ENA I / underwater
  journey" card, so `45:10` is right to the second, while the frame at
  43:59 is jungle with the lion and the same HUD art as at 0:00 - the
  43:59 entry does not mark an ENA boundary. JUNCTION's place in the
  route and the 43:59 entry are both left open for the owner. Three of
  the map's marks were checked against the file directly by reading the
  card at each one, and all three agree within about a second; a fourth
  (13:17) is a detector hit whose frame was not read (see the
  title-card bullet below). That is why the map is treated as usable
  with its defects noted.
- THE GAME'S OWN TITLE CARDS ARE A MEASURABLE MARKER. Reading the
  sampled frames showed that level starts carry a full-screen card, and
  three of them were read frame by frame: 2353 s "ZARIA II" (the map's
  39:13), 2713 s "ENA I / underwater journey" (45:10), 7233 s "KADMAR I
  / ancient mysteries" (2:00:33). Three for three, within about a
  second; the detector below also flags a card-shaped frame at 790 s
  (13:10) against the map's 13:17, but that frame was not read.
  The other marks were then probed by reading the brightest frame within
  +-25 s of each: none of those twelve landed on a card, but their
  neighbourhoods agree with the map everywhere except `43:59 ENA 2` - at
  43:59 the lion is in jungle with the same HUD art as at 0:00, while
  ENA I really begins at 45:13 with its own card and a swimming bird in
  place of the lion. Frames: `/tmp/opencode/refcap/sheet_marks.png`,
  `m_2639_big.png`.
  A whole-file detector for the cards - one 320x200 grayscale frame per
  10 s of the 1152 px game column (2039 frames,
  `/tmp/opencode/refcap/cards.sh` and `cards.py` for the per-frame
  black/bright statistics) - flags 104 card-shaped frames in 68 windows,
  many of them plain fade-to-black, and it MISSED the cards at most of
  the marks above. It is a candidate finder, not a verdict: the
  10-second-accurate version of the owner's map still needs a detector
  that keys on the card's colour and persistence, or a human reading.
- GROUND RULE ADDED. HANDOFF section 2 gains rule 7: "This working tree
  is local. Every build, test and measurement of this project runs on
  the machine that holds this repo ... Do NOT use the remote-host
  skills here - `debian-server` (the `lin` wrapper) and
  `openwrt-router` (the `wrt` wrapper): those hosts hold neither this
  repo nor the game data, so a command run there checks nothing about
  the port." The `windows-handheld` (`win`) skill is explicitly kept,
  because the WASAPI device measurement of PR-34/35 was taken on
  ADAM-HANDHELD - the rule excludes hosts that hold no evidence, not
  remote measurement as such. Rule 2 was rewritten in the same spirit:
  the reference captures live in `reference/video/` (gitignored), and
  everything cut from them stays under `/tmp/opencode/`.
- TESTS AND GATES. No code, no Makefile and no test changed, so there
  is no new behaviour to pin and no mutation check applies; the
  self-tests are not the evidence for anything here. The standing gates
  were still run on this exact tree (`/tmp/opencode/gates_pr111.sh`,
  logs `/tmp/opencode/g3_*.log`, one verify run at a time): `make`,
  `make win`, `make ll_level` all EXIT=0 with 0 warning lines;
  `./ll_level --play-selftest` and `--mov-selftest` EXIT=0;
  `make -k verify-all DATA=/tmp/opencode/iso_stage/LEO
  HD=/tmp/opencode/hd_files` EXIT=0; the same with
  `DATA=/tmp/opencode/nope HD=/tmp/opencode/nope` EXIT=0 with its 23
  named SKIPs - identical to the PR-110 baseline, which is the point of
  running it for a docs change: a `.gitignore` pattern that hid a
  fixture the suite needs would move that count. I also checked the
  other half of the possible interaction directly: no leg in the
  Makefile and no tool under `tools/` mentions `reference/video`.
- OPEN ITEMS. (1) The playthrough's level boundaries are dated by the
  owner's map to minute accuracy, and by my own reading to three exact
  cards (39:13, 45:13, 2:00:33). A complete card
  list needs a better detector than the one built here - the grayscale
  black/bright test flags 68 windows and misses real cards, so the next
  attempt should key on the card's green text on black and on the
  several seconds a card stays up. (2) JUNCTION is unmapped - it is the
  only one of the 13 blocks with no entry in the owner's map, and the
  map's four `MENU` entries are not identified with it or with each
  other. (3) The `43:59 ENA 2` entry is unexplained: the frame there is
  jungle with the lion, and ENA I starts 74 s later. (4) ep.2 and the
  other nine playlist episodes are still only on YouTube - only ep.1,
  the ladder clip and the playthrough are kept on disk. (5) The framing
  of the game's own `.IS` cutscenes in the video (ep.1 t=118-124: black
  bands reaching to column 36 and row 65) is not understood: whether
  that rounded picture IS the whole Mode X frame the movie decoder
  produces or a letterboxed copy of it is exactly what PR-55/56's movie
  work could answer, and no record settles it yet.
- FOLLOW-UP, SAME DAY (owner's instruction, quoted rather than
  paraphrased): "its worth reason why verify-all after that changes?"
  and then "yes, not run veryfication for pure doc commits". PR-111
  itself WAS fully gated and its numbers are above (the gate script was
  `/tmp/opencode/gates_pr111.sh`); the rule changes for future docs
  commits only. HANDOFF section 2 rule 4 gained the exception, in the
  narrow scope the owner asked for: `.md`-only commits, and `.md` +
  `.gitignore` is NOT inside it, because a `.gitignore` pattern can hide
  a tree-based fixture and the data-free run's named-SKIP census is
  exactly what catches that. Say the word and I will widen it to
  `.gitignore` and `Makefile`-comment-only changes too. As evidence for
  the change itself: grep over `Makefile`, `tools/checkfix.py` and
  `tools/verify_help.py` shows those files only ever MENTION
  NEXT_STEPS.md inside `#` comments and `@echo` SKIP strings - no leg
  reads a `.md` file - so no build or verify target can be affected by a
  docs edit. PR-111's own CI, for the record: run 44175913075,
  completed / success, jobs linux, asan, windows-smoke.
- FOLLOW-UP 2, SAME DAY - THE BAR ON THE RIGHT (owner's request, quoted):
  "on comment in that 5+ hrs video is some bar on right site, worth to
  note it to keep it in mind". It is in `R3` at x 1152..1279, 128 px wide,
  and it is the uploader's decoration, not game art. PR-111's record
  already cropped it out and measured its boundary; this follow-up
  strengthens the claim and records the caveat. Two measurements now agree:
  the 2 s-burst per-column temporal std (19-21 -> 2.3-2.8 at x=1152), and
  a new one over 40 consecutive frames (0.67 s) at t=120, 7200 and 20040,
  where the mean per-column temporal std drops from 12.3 to 0.1 at x=1152
  and stays 0.0-0.1 to the right edge while in-game columns range 3-24 -
  so those columns carry no motion anywhere in 5 h 39 m. It is also NOT a
  black bar: mean luma 128.9, spatial std 65.0, identical statistics at all
  three times, 718 of 720 rows above luma 8 - which is why a "find the
  black columns" test misses it and a temporal-std test finds it. The
  comment angle was checked and does not hold: this video has no comments
  at all (`yt-dlp --write-comments` extracts 0, `comment_count` 0), and a
  keyword scan (`pas`, `słup`, `bar`, `right`, `stron`, ...) of the 34
  comments on ep.1 `xPiXBmPEdXg` found nothing about a bar - so the note is
  the owner's own observation and is attributed that way. Written into
  HANDOFF section 3 (R3, "THE BAR ON THE RIGHT") and section 9, including
  the recipe for any future upload from this channel: find the boundary by
  per-column temporal std over frames taken at several times in the file,
  and require "static at every sampled time" rather than "low" (a low-but
  moving band does exist inside the game window - at t=7200 x 544..704 sits
  at std 3.2-4.0). Files changed by this follow-up: `HANDOFF.md`,
  `NEXT_STEPS.md` only. No build or verify leg was run for it, per the
  exception recorded in the bullet above: nothing under `.gitignore`,
  `Makefile`, `tools/`, `ported/` or `.github/` is touched, and no Makefile
  target or tool reads a `.md` file (grep evidence in that bullet).
- CI OF THE FOLLOW-UP PUSH (recorded, not fixed here). Run 44176302862 on
  `29da788`: `linux` success, `windows-smoke` FAILURE in the step "Run
  them (committed fixtures only; no game-data staging)" - one check of
  about thirty, `platform_selftest: sleep_until honors the deadline
  (>=20ms) FAILED` (job log `/tmp/opencode/ws_fail.log`, the check is
  `tools/platform_selftest.c:60-63`: sleep 20 ms from `t0`, then require
  `now_ns() >= t0 + 20ms`). `asan` was still running, so a rerun of the
  failed job had to wait for the run to finish. This is NOT a consequence
  of a docs change: `git diff f1afa64..HEAD --stat -- ported/ tools/
  Makefile cmake/` is empty, i.e. that leg's code is byte-identical to
  the CI-green PR-110 and PR-111 trees. It is a timing assertion on the
  msys2 runner, and it is a different thing from the known-flaky Wine
  `win restart-confirm` leg named in HANDOFF section 3's pitfalls. Outcome
  of the rerun, and whether it needs a code change (a tolerance, or
  `plat_sleep_until` undershooting on Windows), goes in the PR-112 record.

## PR-112 — the headless backend re-checks its OWN clock: the suite's
only timing-sensitive assertion, `sleep_until honors the deadline`, has
failed 2 of the 125 CI runs this workflow has ever had, and it is
asserted 20 times at zero tolerance instead of once

- WHY. CI's `windows-smoke` job failed on the PR-111 follow-up push
  (run 124 / 44176302862) in the step "Run them (committed fixtures
  only; no game-data staging)", on ONE of the 35 checks that
  `platform_selftest` runs there (job log `/tmp/opencode/ws_fail.log`:
  34 lines "(ok)" plus this one FAILED, then "1 check(s) FAILED"):
  `platform_selftest: sleep_until honors the deadline (>=20ms) FAILED`.
  Pulling the whole workflow history (125 `ci` runs, `gh api
  actions/runs?per_page=100&page=1..2`) it is a pattern, not a one-off:
  only 4 runs ever failed, and 2 of those 4 are this exact check - run
  101 / 44043011593 at 07:59 UTC on `c3e0547` and run 124 at 21:06 UTC
  on `29da788`. The other three failures (runs 1, 2, 8) are real setup
  breakage: `SDL2/SDL.h: No such file or directory`, `llrs_open failed:
  .../ANIMACJE`, and a link error. Rerunning run 124's failed job passed
  on attempt 2 (`linux`, `asan`, `windows-smoke` all success). Run 101
  was never rerun, its `attempt` is 1 and its conclusion is still
  failure. Neither failing tree was a regression: `git diff
  f1afa64..HEAD --stat -- ported/ tools/ Makefile cmake/` is empty for
  124, i.e. the leg's code was byte-identical to the CI-green PR-110/
  PR-111 trees. What is special about this check is that it is the ONLY
  wall-clock assertion in the suite (`grep -rn 'sleep_until honors'` ->
  one hit, `tools/platform_selftest.c:62`); every other check compares
  hashes, counts or structures and cannot drift.
- WHAT THE CODE DID, AND WHAT IT DOES NOW. `platform.h:137-140` states
  the contract: "plat_sleep_until returns no earlier than the given
  timestamp". The SDL backend has always enforced that by its own loop
  (`platform_sdl.c:434-444`: re-read `plat_now_ns()`, OS-sleep until
  0.3 ms before the deadline, then yield-spin). The DUMMY backend - the
  one CI's windows-smoke and asan legs build, and the one
  `platform_selftest` links - did not: it called
  `clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, ...)` in a loop that
  only retried on a NON-zero return, so whatever that call returns early
  is taken at face value and never re-checked against our own clock. It
  now reads the clock like the SDL backend does (`while (plat_now_ns() <
  ns)`), keeps the EINTR retry, and breaks instead of spinning if the
  call is unsupported or its arguments rejected. That also makes the
  contract survive a backward step of the system clock, which trusting
  one CRT call could not.
- SCOPE, MEASURED. Only one target builds this backend: `grep -n
  PLAT_BACKEND_DUMMY Makefile` has exactly one hit, line 1675, the
  `platform_selftest` rule. No game or engine binary (`ll_level`,
  `ll_cutscene`, `ll_cutscene.exe`) contains this code, so no pin, pose,
  frame or log line in any verify-* leg can move - this is a
  self-test-tool change and nothing else. The behaviour that DOES change
  is inside `platform_selftest` itself (its sleeps can now only be later,
  never earlier), which is what the new checks below guard.
- TESTS (data-free, hand-derived, as ground rule 3 requires). The single
  sample is kept and two checks are added: 20 deadlines of 20 ms with
  ZERO tolerance for an early return, and a 200 ms ceiling. Both
  expectations are read off the contract, not measured: "returns no
  earlier than the given timestamp" means the expected count of early
  returns is 0 on every platform - no constant to calibrate, no game
  data, no video. The 200 ms ceiling is hand-derived the same way (~10x
  the deadline): it asserts "a sleep happened" and deliberately claims
  nothing about OS wake latency, so it cannot drift with runner load the
  way a tight upper bound would.
- MUTATION CHECKS (script `/tmp/opencode/pr112_mut.sh`; mutants are
  whole-function swaps of the dummy `plat_sleep_until`, logs in
  `/tmp/opencode/pr112_mut/`). BASELINE: 3 clean runs, 0 warnings, 0
  failed checks. M1 = the OLD shipped form restored: passes on this box,
  which is the honest and important result - glibc keeps the absolute
  deadline, so this defect is structurally invisible on Linux and can
  only be seen on the Windows legs. M2 = a backend that returns up to
  5 ms EARLY: the new check fails ("sleep_until never returns before its
  deadline, 20x FAILED", exit 1), so it does catch that class. M3 = a
  backend that returns 250 ms LATE: the ceiling check fails, exit 1.
  M4 = M2 with the two new assertions deleted: exit 1 still, because the
  original single-sample check also catches a 5 ms early return. That is
  recorded rather than hidden: the new checks do NOT claim sensitivity to
  a bigger defect than the old one - their value is that they do not
  depend on one sample happening to land early, which is precisely how
  the 2 CI failures behaved (a rare event: 2 of 125 runs). M4's build
  also prints 2 `-Wunused-but-set-variable` warnings, which is the
  compiler confirming those counters exist only for the assertions.
- THE DIAGNOSTIC, because the mechanism is NOT pinned. What is measured
  is the fact (2 failures, native msys2 MINGW64 only, one specific
  check, pass on rerun). Two candidate mechanisms remain open: the CRT's
  `clock_nanosleep` wrapper waking early for a sub-millisecond
  remainder, or a rare backward step of the clock source. To settle it,
  the selftest now PRINTS (never asserts) what the raw CRT call would
  have done on the machine running it: 20 more deadlines, reporting
  `diag raw-CRT clock_nanosleep 20ms x20: early=.. unsupported=..
  worst_undershoot_us=..`. On this Linux box all three baseline runs read
  `early=0 unsupported=0 worst_undershoot_us=0`, i.e. glibc is exact -
  consistent with M1 passing here. The next `windows-smoke` run will
  print the same line on the runner that failed, and that number is the
  answer. It is a print, not a check, because wake latency is not part
  of any contract; the Makefile's `verify-platform` greps for "(ok)" and
  the "NO SDL display/audio" line only, so a diag line changes nothing.
  A local Windows probe was tried and did NOT work, also recorded:
  building the same 20-sample probe with the vendored llvm-mingw
  (`vendor/llvm-mingw/bin/x86_64-w64-mingw32-clang`, clang 23.1.5)
  fails at `ld.lld: error: undefined symbol: clock_nanosleep64` (and
  `clock_gettime64`) - that CRT import library has no such symbol, and
  `grep -rn clock_nanosleep vendor/llvm-mingw/include/` finds nothing,
  so this toolchain cannot even express the call msys2's mingw-w64 gcc
  compiles. A real-Windows measurement therefore needs either the
  handheld (ADAM-HANDHELD, asleep when probed: `win` reports "does not
  answer on TCP within 2s", WOL does not work there) or the CI leg.
- TESTS AND GATES on the exact tree this commit ships (`/tmp/opencode/
  gates_pr112.sh`, logs `/tmp/opencode/g4_*.log`, one verify run at a
  time): `make`, `make win`, `make ll_level` EXIT=0 with 0 warning lines;
  `./ll_level --play-selftest` and `--mov-selftest` EXIT=0;
  `./platform_selftest` EXIT=0 printing 37 "(ok)" lines on three
  consecutive runs (the pre-PR binary prints 35 "(ok)" lines + 1 final
  line without the suffix, and has 34 `check()` calls vs 36 now - the +2
  are the new checks);
  `make verify-platform DATA=/tmp/opencode/iso_stage/LEO` EXIT=0 and
  green; `make -k verify-all DATA=/tmp/opencode/iso_stage/LEO
  HD=/tmp/opencode/hd_files` EXIT=0; the same with
  `DATA=/tmp/opencode/nope HD=/tmp/opencode/nope` EXIT=0 with its 23
  named SKIPs - identical to the PR-110/PR-111 baseline, i.e. nothing
  moved. The added 40 sleep cycles cost real wall clock, measured both
  ways: three timed runs of the pre-PR binary (old backend + old
  selftest, rebuilt from `HEAD`) are 0.03/0.05/0.05 s and print 35
  "(ok)" lines, three runs of the new one 0.89/0.85/0.84 s and print 37
  - so ~0.8 s added, which is the 40 x 20 ms of sleeps themselves. That
  is noise against the suite and the price of not depending on one
  sample. The diagnostic printed here:
  `early=0 unsupported=0 worst_undershoot_us=0`.
- OPEN ITEMS. (1) The mechanism: SEEDED - the next run changed the
  answer. Run 136 / 44180333781 (the follow-up push's CI, 20 s later on
  the same runner class) prints on its `windows-smoke` leg, job
  134382654217, log line 444:
  `diag raw-CRT clock_nanosleep 20ms x20: early=1 unsupported=0
  worst_undershoot_us=40` - one raw `clock_nanosleep(TIMER_ABSTIME)`
  call returned **40 microseconds before its absolute deadline**. Two
  Windows samples in total: run 135 `early=0`, run 136 `early=1
  worst=40us`; the asan (glibc) legs print `early=0` in both runs. So
  the early-wake behaviour is REAL and intermittent, and 40 us is well
  inside the sub-millisecond class the two CI failures needed. That is
  not proof it caused those two failures (they needed >= ~0.1 ms of
  undershoot, and 40 us is what the next sample happened to show), but
  the mechanism is now supported by a direct measurement instead of
  being an open guess, and the "backward clock step" theory is no
  longer needed to explain anything. It also retrospectively validates
  the framing of this PR: the defect was in trusting one CRT call, and
  the fix - re-reading our own clock - is exactly what a 40 us early
  wake needs. The diagnostic keeps counting for free on every future
  run; a growing `early=` count with a larger `worst_undershoot_us`
  would quantify the residual for the record. Not proven causal for the
  two failures (40 us is one sample; they needed >= ~0.1 ms) - the
  evidence is that the behaviour exists, not that it was the whole of
  those runs. (2) Run 101 (`44043011593`) is still red in
  the history - it was never rerun; nothing was done to it, and its tree
  predates PR-110/111. (3) The suite still has exactly one
  wall-clock-dependent assertion; if it ever needs a second, it should
  be written with the same zero-tolerance-on-the-lower-bound + loose
  upper-bound shape, or the flake rate comes back.
- CI FOR THIS COMMIT, and the diagnostic answers back. Run 135 /
  44179955442 on `b273981`: `linux`, `asan`, `windows-smoke` all
  success. The three sleep_until checks pass on the NATIVE msys2 leg
  (job 134381308755, `/tmp/opencode/g5_job_134381308755.log` lines
  439-441: `sleep_until honors the deadline (>=20ms) (ok)`, `... never
  returns before its deadline, 20x (ok)`, `... stays within 200ms ...,
  20x (ok)`), and the diagnostic prints:
  `diag raw-CRT clock_nanosleep 20ms x20: early=0 unsupported=0
  worst_undershoot_us=0` - and the same line on the asan job
  (134381309044, line 665). The `linux` job does not print it: its
  full suite runs `verify-all`, where `verify-platform` SKIPs for want
  of `$(DATA)` (4 mentions of `platform_selftest` in that log, zero
  diag lines).
  So, honestly: **the raw CRT call showed no undershoot on the very
  runner class where the check failed twice.** That does NOT prove the
  wrapper is exact - 20 samples on one run of a 2-in-125 event is not
  evidence of absence, and the failure may need a loaded runner. What it
  does is fail to support the "sub-millisecond early wake" theory on
  demand, which leaves a rare backward step of the clock source as the
  surviving candidate. Neither is pinned; the diagnostic will accumulate
  the number on every future Windows run at no cost, and only a
  non-zero line - or a real Windows box running `/tmp/opencode/cnsl.c`
  with hundreds of samples - can settle it. What PR-112 changes does not
  depend on which it was: the contract is now enforced by re-reading our
  own clock, which covers both.
  One residual to state rather than hide: that loop is safe under a
  backward step (it re-sleeps to the same absolute deadline), but it is
  bounded only by the clock, not by a retry limit - an unbounded
  backward step would hang the selftest rather than fail it. A limit was
  left out deliberately: a bounded loop that gives up would reintroduce
  exactly the silent undershoot this PR removes.
- THE DIAGNOSTIC ANSWERS DIFFERENTLY 20 SECONDS LATER (supersedes the
  sentence in the bullet above, quoted rather than edited there): it said
  the diagnostic printed `early=0 ... worst_undershoot_us=0` on the
  Windows leg and concluded the early-wake theory "did not reproduce".
  Run 136 / 44180333781 (the push of that very sentence) prints, on its
  `windows-smoke` leg, job 134382654217, log line 444:
  `diag raw-CRT clock_nanosleep 20ms x20: early=1 unsupported=0
  worst_undershoot_us=40` - one raw `clock_nanosleep(CLOCK_MONOTONIC,
  TIMER_ABSTIME)` call came back **40 microseconds before its absolute
  deadline**, with its three sleep_until checks still ok. Tally so far:
  windows 135 `early=0`, windows 136 `early=1 worst=40us`, glibc (asan)
  `early=0` in both. That is the first direct evidence of the behaviour,
  and it has exactly the shape the two failures needed: intermittent,
  Windows-only, sub-millisecond. Two honest limits: 40 us is one sample
  and the failures needed >= ~0.1 ms, so this does not prove those two
  runs undershot that far; and 2 windows + 2 asan runs is a tiny census.
  What changed is the status of the explanation - it is no longer a
  guess with no supporting measurement, and the "backward clock step"
  theory is no longer needed to explain anything. Nothing in the code
  changes: re-reading our own clock is exactly what a 40 us early wake
  needs, which is why the fix was written to not depend on the answer.
- THE RUN NUMBERS IN THE TWO BULLETS ABOVE ARE WRONG, quoted rather than
  edited: they read "Run 135 / 44179955442", "Run 136 / 44180333781"
  and "run 135 `early=0`, run 136 `early=1 worst=40us`". The correct
  `ci.yml` numbers are **126** and **127**. Checked against the API:
  `actions/runs/44043011593` answers `wf=ci wf_run#101` (so the run 101
  citation in the WHY bullet is right, and run 124 / 44176302862 is
  right too), and the repository has 128 Actions runs in total, all of
  them `ci`, so no numbering can reach 135. Every run ID, job ID and log
  line number quoted in this record comes from the API and is correct -
  what was wrong is a label I invented, not a measurement.
- THE TALLY, corrected and extended with the third sample, superseding
  the "Tally so far" sentence quoted above. Run 128 / 44181397463 (the
  push of the two bullets above) is green on all three legs, and its
  `windows-smoke` job 134386350669 prints `diag raw-CRT clock_nanosleep
  20ms x20: early=1 unsupported=0 worst_undershoot_us=11` while its asan
  job 134386349975 prints `early=0 ... worst_undershoot_us=0`. Three
  Windows legs now: 126 `early=0`, 127 `early=1 40us`, 128 `early=1
  11us`. Three glibc legs: all `early=0`. The `linux` job prints nothing
  either time (its `verify-platform` SKIPs for want of `$(DATA)`). So
  two of three Windows runs show an early wake and none of three glibc
  runs do, at 11 and 40 us. Still a tiny census, still not evidence that
  those two failures undershot this far, but the shape - intermittent,
  Windows-only, sub-millisecond - is now supported twice, and the
  diagnostic keeps collecting it for free.

## PR-114 — the `fx != 0` half of the walk gate: the count becomes a tool
output, the coverage claim gets corrected, and two legs pin the port's
current choice without changing it

- WHY. PR-110's open item 4 is the only item in the queue whose shipped
  behaviour rests on a term the engine never wrote. Its wording, quoted
  from that record (NEXT_STEPS.md:8454-8459): "the `fx != 0` half of
  the gate has shipped data behind it and no test in front of it: 105
  state blocks in 69 files are BY_X, and 30 of them (290 rows, 21 files
  \- ZARIA's own [A_ZABLOK] among them) declare `F=0,0`, which the port
  then treats as time-fed. No leg in the suite enters such a state, so
  which half of that gate the engine really keys on is untested." Both
  engines of this repo use that gate: `ll_player_step` sections 4 and 7
  (`ll_play.c:1118`, `:1276`) and `obj_advance` / `obj_row_tick`
  (`ll_level.c:1169`, `:1219`) all read `(ctrl & LL_CTRL_BY_X) && fx !=
  0`. PR-99's disassembly of the dispatcher 0x543a0 keys the feed on the
  mode bits 0..2 ALONE, so the second term is the port's own addition
  (PR-100's owner reading, unchanged here). Item 4 also asked for the
  count to stop being a hand claim; it hadn't, and the tool that
  shipped the neighbouring counts (`tools/re/mov_channels.py`) cannot
  reproduce it, because its row regex needs a two-digit frame field.
- WHAT SHIPPED: `tools/re/mov_byx_gate.py` (stdlib only, inventories,
  no game data in git) plus two data-free selftest legs. No behaviour
  changed: both engines keep requiring `fx != 0`. The legs PIN the
  consequences of that choice, so the next PR that changes the gate is
  a visible decision instead of a silent one. The tool is the census
  item 4 asked for; its 102-line output is the evidence this record
  cites (`/tmp/opencode/pr114_census.txt`).
- WHAT THE TOOL MEASURES, and item 4's numbers reproduced. Over the
  777-file .MOV corpus: 18617 row lines read the way `ll_mov_parse`
  reads them (`sscanf("%d,%d")`, a negative frame number counts);
  `mov_channels.py`'s regex sees 15996, and the whole 2621-row gap is
  rows with a one-digit or negative frame field, in 78 files. State
  blocks that NAME `BY_X`: 105 in 69 files. Of those, 75 carry a
  non-zero `F=` (both readings feed them the same way) and 30 carry
  `F=0,0` - 290 rows in 21 files, ZARIA's `[A_ZABLOK]` among them
  (5 files write it). Blocks naming `BY_X` with `F=0,<non-zero>`: 0,
  so the gate's `fx == 0` test and the data's `F=0,0` test are the
  same set here, exactly as item 4 said. Beyond item 4, the tool also
  prints what the disagreement costs: the engine's mode table is 172
  BY_X states / 1349 rows / 128 files, but 62 of those states name no
  `control=` line at all and 5 name a list with no mode token, so they
  get mode 0 by default; the port's gate splits the same states 75
  distance-fed / 2243 time-fed, and the two readings disagree on 97
  states / 357 rows / 80 files. For 29 of the 30 `F=0,0` blocks the
  gate is the whole law, because they declare no motion source of their
  own (no header `F=`, no row `F`/`FC`/`IC` pair) - a mode-only feed
  would add the truncated `|v_x|/100` = 0 forever. Five of those 29
  also have no exit except one a row-list wrap arms, or none at all:
  ENA.03's `A_CHANGE_LEVEL` (9 rows), KADMAR.07 CIUCHCIA's `A_STAND`
  (20), and ZARIA.01 KAMIEN1/2/3's `A_STAND` (8 each). Under the
  literal mode-only reading those five never advance a row and never
  leave the state - which is an argument for the port's gate, not
  against it, and is recorded as evidence, not as a decision. The
  threshold table PR-110 items 1, 2 and 8 need is printed four ways:
  "as written" gives 170 rows at threshold 0 and 1715 rows in 1..99 in
  111 files (the two counts that record quotes), while "as
  `ll_mov_parse` stores" gives threshold 0: 0 rows, because M4-C turns
  a non-positive row value into 100 - so PR-115 must read those 170 as
  authored values, not as stored ones. Also counted: 116 active
  `time_scale=` lines in 38 files (values 60..4000) plus 4 commented
  with the game's own `;` in 2 files.
- CORRECTION TO ITEM 4 (quoted, not edited in place). "No leg in the
  suite enters such a state" is too strong - three fixture states in
  this repo ARE BY_X with `fx == 0`, and none of them says anything
  about which clock feeds its rows. `ll_play.c:1798`'s PR-82 fixture is
  the real RAN LEW `A_GO_TO_BALON_L` (`control=BY_X`, no `F=`, negative
  authored frames); its CHECKs read the drawn frame and facing, not the
  row clock. `ll_level.c:2196`'s PR-78 `gnd` fixture is
  `control=BY_X|CHECK_LN` with one 100-unit row and no `F=` line at
  all; its CHECK only asks that the object drops onto the 180 line.
  `ll_level.c:2248`'s `halt` (`control=BY_X`, `F=0,0`) is leg 3 of
  `obj_motion_selftest`, whose own comment at `ll_level.c:2364` writes
  the open part as an estimate: "F=0: no movement at all (the walk
  model has nothing to accumulate; the frame list falls back to time -
  estimate)". The claim that survives is the narrower one: nothing in
  the suite PINS that fallback, and which half of the gate the engine
  keys on is still untested. That is what the legs below do.
- THE PLAYER LEG (`ll_play_selftest`, fixture `A_PUSH` -> `A_COAST`).
  Hand-derived, no game data, every step at `dt = LL_ITER_MS`, the grid
  where one step is exactly one feed. `A_PUSH` is the control half:
  `control=BY_X|CH_DIR`, `F=1000,0`, `T=40,0`, one 1000-unit row, so
  section 2's accel is `|F| * dt` = 28 per step against a `T=` cap of
  40: `vx` is 28.0 after one held step and 40.0 after the second, the
  travelled distance (28 x 0.028 + 40 x 0.028 = 1.904) lands in `facc`
  only, and `row_t` / `row_rem` stay exactly 0 with the row unclosed -
  the mode-0 branch replaces the wall clock, PR-110's law, re-pinned
  here as the control. The `K_GEN1` pulse (a signal, not a key) moves
  him into `A_COAST`: `control=BY_X`, `F=0,0`, no `T=`, `G=0,0`, six
  100-unit rows - the shape of 29 of the 30 shipped blocks. Pinned: the
  velocity is FROZEN at the entry value, 40.0, because the accel law is
  `|F| * dt` = 0 and both target readings (the state's own 0 with no
  `T=`, and +40 through `auto_walk` from the pulse) clamp to a zero
  step; it does not brake on key release either, for the same law
  (three more steps with `k_fwd` up still read 40.0). The rows run on
  the ITERATION clock: exactly one 100-unit row closes per step, the
  sixth closes index 5 of 5 and wraps (`fi` back to 0, `broke` armed by
  the wrap alone, `ll_play.c:1309`), `row_t` / `row_rem` read 0 at
  every step, and `facc` stays exactly 0.0 - section 4's feed does not
  run at all. x advances 1.12 per step (40 px/s x 0.028 s) and y/vy
  never move (`G=0,0`). One parser/model detail the leg also pins: the
  state-change reset (`ll_play.c:957-962`) zeroes `row_t` and `facc`
  and NOT `row_rem`, so a wall-clock carry would cross the pulse; here
  A_PUSH is a mode-0 state, its accumulator was fed by distance only,
  so `row_rem` is legitimately 0 on entry to `A_COAST` and the
  one-row-per-step identity holds from the first step after the pulse.
- THE OBJECT LEG (`obj_motion_selftest` leg 6, fixture `glide`, with a
  BY_TIME `snaps` control). `control=BY_X`, `F=0,0`, three 100-unit
  rows, entered at (40,30) with its anchor at (5,9) deliberately far
  away. `walk0` (`ll_level.c:1169`) is false, so lines 1179-1181 zero
  `facc`/`xa` every call and `obj_walk` never runs: one row closes per
  `obj_advance(…, LL_ITER_MS)` call (fi 1, 2, then the third closes
  index 2 of 3, finds no transition and wraps to 0), `row_t`/`row_rem`
  read 0, `facc`/`xa` stay exactly 0.0, and x/y stay (40,30). The wrap
  does NOT snap the anchor, because the test at `ll_level.c:1212` is
  `if (!(s->ctrl & LL_CTRL_BY_X))`: a mode-bit test,
  not an `F=`-non-zero test. The BY_TIME one-row fixture right after it
  does snap back to (5,9), so the difference between the two is the
  mode bit and nothing else.
- MUTATIONS (`/tmp/opencode/mut/run.sh`, logs in that directory, tree
  restored and re-verified with `git status`). M1, both `fx != 0` terms
  dropped from the player (the literal mode-only reading of 0x543a0):
  3 failures, all three inside the new leg (`ll_play.c:2569`, `:2570`,
  `:2581`), and no other leg changes - which IS the coverage claim.
  M2, both terms dropped from the object: 1 failure, the new leg's call
  1 (`fi 0` instead of 1); leg 3's "F=0 does not move" check still
  passes, and that gap is exactly what item 4 was about. M3 (control):
  drop the MODE half in the player instead - 10 failures, every one of
  them pre-existing (first at `ll_play.c:1646-1648`, then `:1744`), and
  the new leg is unaffected, so the mode half was already covered and
  the `fx` half was not. M4: delete the time-fed fallback
  (`if (!walk)` -> `if (0)`) - 59 failures including the new leg, so
  the leg also depends on the fallback being there. M5: make the
  `ll_level.c:1212` wrap snap require `fx != 0` too - 1 failure, the
  new leg at call 3, `at (5,9)`.
- WHAT THIS DOES NOT DECIDE. (1) Which half the engine keys on: the
  legs pin the port's current choice, they do not measure the engine,
  and the mode-only reading stays open - the five no-exit `F=0,0`
  states above are the strongest counter-example to it that the corpus
  gives, not a proof. (2) Whether a control word absent from the .MOV
  text really means engine mode 0: that is the 62-state / 5-file half of
  the 97-state disagreement, and this tool counts the absence, it does
  not read the parsed .MOV binary where the control word actually
  lives. (3) The `T=` cap: the port still does not apply the engine's
  130 clamp (0x52590), and `A_COAST` here has no `T=` at all, so this
  leg neither uses nor tests the cap. (4) The vertical-distance law
  (T5), one-row-per-step (`0x5232b`) and the `acc == thresh` boundary
  are untouched - PR-115 and PR-116, measured with the corrected
  threshold table above.
- DOCS: `tools/re/README.md` gains the tool's entry and a correction of
  `mov_channels.py`'s row count ("it sees 15996 of the 18617 row lines
  that `ll_mov_parse` would read", the 2621-row gap named); HANDOFF.md
  §2's row-clock table row and §7 item 13 both get a new bullet quoting
  their own wording rather than being edited, since both still say "no
  leg covers one" / "no test does".
- TESTS AND GATES: `make`, `make win`, `make ll_level` with 0 warnings;
  `./ll_level --play-selftest` and `./ll_level --mov-selftest` pass
  with the new legs and their summary strings now name PR-114;
  `make -k verify-all DATA=/tmp/opencode/iso_stage/LEO
  HD=/tmp/opencode/hd_files` and the fixture-free
  `DATA=/tmp/opencode/nope HD=/tmp/opencode/nope` both EXIT=0, the
  fixture-free run printing the same 23 named SKIPs as the PR-110 /
  PR-111 / PR-112 baseline. The five mutations above are the evidence
  the legs are load-bearing; the tree was restored from
  `/tmp/opencode/mut/{ll_play,ll_level}.orig` after each and the diff
  was checked to contain no deletions.

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
- DONE in PR-93: the two placement grammars PR-45 had not recognised -
  the 3-comma static image lines ("<LEAF>,<name>.seq,x,y": the line
  names its own .SEQ in that CD leaf; 84 game-wide, 69 join, the 15
  LONA.05 WODOSPAD2 misses named) are placed and drawn BEHIND the
  actors, and the 130 five-field entries authored at exactly 0,0 are
  attached children (joined and counted, never instantiated at the
  world origin - that was the user's "fruits not in place" corner).
  See the PR-93 record for the census and the open items (§ statics
  animation, child spawn trigger, panc.seq coordinates, GRP format).
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
