# OpenLeo native port — next steps

Working plan derived from `OpenLeo_native_port_review.md` (P/E/M) and
`OpenLeo_porting_tips.md` (T). Update the status lines here as items
land; the review/tips files stay as the detailed work orders.

## Status as of 2026-11-14 (through PR-15, all pushed)

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
  still drive ll_view/ll_audio directly (their migration = later PR).  (this commit)

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
DONE for the player in PR-15 (next section); title/map/level remain.

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
users in engine code.

## Next — pick from the Deferred list below

Nothing queued: every review item that was scoped as "worth fixing
soon" has landed. Next round = Deferred (big architecture) or the
user's call (T-11 4:3 aspect default, T-22 AppImage).

## Deferred big items (deliberately out of scope so far)

- P-03 single window + single SDL lifecycle - DONE for both: PR-10
  (ll_view session) + PR-12 (ll_audio session). The device itself is
  per-playback by design; see the PR-12 note.
- P-04 focus stealing - DONE in PR-10 (raise on WINDOWEVENT_ENTER).
- P-05 Wayland/X11 (current pipewire->alsa swap works; revisit if
  users report issues).
- E-08/T-14 palette codec fidelity (frozen: pixel output must not
  change).
- T-01 platform abstraction - phases 1+2 DONE (PR-14 interface +
  headless backend; PR-15 SDL backend + cutscene-player migration).
  Remaining: migrate the title menu / chapter map / level scene to
  plat_poll/plat_present (same pattern, behind the PPM/grep oracle
  pins), then ll_view/ll_audio become backend-private.
- T-03 config knobs cleanup, T-04 globals,
  T-05/T-06 fixed-step timing + precise sleep, T-07 clock ownership,
  T-16 Polish text, T-17 save/config separation (STATUS.LEO codec
  itself is done), T-19 startup data validation, T-23 structured RE
  traces, T-24 golden-master vs original EXE, T-26 engine state
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
