# OpenLeo native port — next steps

Working plan derived from `OpenLeo_native_port_review.md` (P/E/M) and
`OpenLeo_porting_tips.md` (T). Update the status lines here as items
land; the review/tips files stay as the detailed work orders.

## Status as of 2026-11-14 (through PR-19, all pushed)

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

## PR-21 — golden rig: input + dual state detectors (the menu is NOT CD-gated) (IN PROGRESS 2026-11-15)

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

## Next — pick from the Deferred list below

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
golden.* capture half stays open until the no-CD rig is shown to
reach the map-menu state - next step there would be scripting the
CD-attached route or probing the attract chain further). Open
candidates: the Deferred big architecture items, or the user's call
(T-11 4:3 aspect default, T-22 AppImage).

## Deferred big items (deliberately out of scope so far)

- P-03 single window + single SDL lifecycle - DONE for both: PR-10
  (ll_view session) + PR-12 (ll_audio session). The device itself is
  per-playback by design; see the PR-12 note.
- P-04 focus stealing - DONE in PR-10 (raise on WINDOWEVENT_ENTER).
- P-05 Wayland/X11 (current pipewire->alsa swap works; revisit if
  users report issues).
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
  DONE in PR-19; the original-side capture half open), T-26 engine state
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
