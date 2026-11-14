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

/* Fixed-timestep pacing for the native ports (PLAN S6 "NATIVE PORTS"):
 * the original paces by execution rate (busy loop), thousands of times
 * too fast on modern CPUs. Engine logic must advance only on tick
 * boundaries at this rate. 22 Hz = observed counter cadence of the
 * slowed QEMU baseline. original/ (DOS) ignores this knob. */
#ifndef TICK_HZ
#define TICK_HZ 22
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
