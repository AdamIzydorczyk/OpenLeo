/* ll_config.h - variant knobs + naming convention for Lew Leon recompile.
 *
 * Naming convention (applies across src/):
 *   is_*   - .IS cutscene stream format (codec, stream walking): the
 *            spec/oracle is tools/is_decode.py.
 *   llrs_* - LongSoft "llrs" resource container format; spec/oracle is
 *            tools/llrs_unpack.py.
 *   ll_*   - project-level glue that is neither of the two wire formats
 *            (ll_config.h, the ll_cutscene binaries).
 *   platform_sdl/ - the ONE SDL2 platform layer shared by the linux and
 *            windows targets; target differences live only in the root
 *            Makefile + the PLATFORM_* knobs below.
 * Every public symbol follows its prefix; file name == primary prefix.
 *
 * Knobs are set per target with -D on the compiler line (see Makefile) or
 * before including any engine header. Defaults here describe native builds.
 */
#ifndef LL_CONFIG_H
#define LL_CONFIG_H

/* Platform selector: exactly one of 0/0/1 style; set by variant Makefile. */
#ifndef PLATFORM_DOS
#define PLATFORM_DOS 0
#endif
#ifndef PLATFORM_LINUX
#define PLATFORM_LINUX 0
#endif
#ifndef PLATFORM_WIN
#define PLATFORM_WIN 0
#endif

/* CD-presence gate chain (PLAN S6: ll_cdcheck_chain vaddr 0x5bd50 and its
 * four subchecks). 1 = faithful retail behavior (original/); 0 = not
 * compiled at all (native linux/windows targets - data comes from a
 * directory). */
#ifndef WANT_CD_GATE
#define WANT_CD_GATE 0
#endif

/* Derive the data drive/dir from LONGSOFT.CFG byte0 (retail layout,
 * "X:\LEO\") instead of argv/env. Native builds: 0. */
#ifndef DATA_DIR_FROM_CFG
#define DATA_DIR_FROM_CFG 0
#endif

/* Fixed-timestep pacing for the native ports (PLAN S6 "NATIVE PORTS").
 * REV 2026-11-14, measured against plain-TCG QEMU as real-time oracle:
 * LEO reprograms PIT ch0 to 70.00 Hz (count 0x4295, FUN_00018870) for the
 * whole session after boot - BIOS ticks 40:6C were measured at 69.8/s in
 * EVERY game state (three full INTRO_P replays + attract + menu), and the
 * counter FREEZEs outright while the 640x400 corner-word menu polls the
 * keyboard.  The .IS movie clock is the same 70 Hz (22050/315 audio
 * blocks; is_player.c derives it from the block size directly).  Non-movie
 * one-shot waits use the 55 ms timer class (FUN_0005b860 +0xe=0x37).
 * So the engine beat for menu/gameplay logic = 70 Hz; old "22 Hz" came
 * from a throttled-icount artifact.  original/ (DOS) ignores this knob. */
#ifndef TICK_HZ
#define TICK_HZ 70
#endif

/* Case-insensitive filesystem lookup (Windows/DOS FAT). Native Linux
 * builds may set 0; a small dir-scan fallback is cheap either way. */
#ifndef FS_CASE_INSENSITIVE
#if PLATFORM_WIN || PLATFORM_DOS
#define FS_CASE_INSENSITIVE 1
#else
#define FS_CASE_INSENSITIVE 0
#endif
#endif

#endif /* LL_CONFIG_H */
