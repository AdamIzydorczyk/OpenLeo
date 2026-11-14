/* is_player.h - shared cutscene player for both native targets. Plays
 * through the platform interface (platform.h, T-01); no SDL here.
 *
 * Fixed-timestep pacing (PLAN S6 "NATIVE PORTS"): the original is
 * execution-rate bound and would run thousands of times too fast on
 * modern CPUs. Here every presented chunk is one clock tick on ONE
 * monotonic ns timeline (plat_now_ns, anchored once in
 * is_player.c as t0_ns - P-02/T-07):
 *   audio regions: hold = region bytes / movie bitrate (the real engine
 *                  model: one SB DMA block per frame, 22050 s16)
 *   silent chunks: repeat * 1/70 s (the DOS PIT reprogram rate)
 *   LEO_IS_PERIOD_MS overrides; deadlines are absolute, drift-clamped,
 *   and Space-pause offsets them instead of rewinding the timeline.
 */
#ifndef IS_PLAYER_H
#define IS_PLAYER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Plays path (a .IS file) in a window with ESC-region audio (both from
 * the platform backend).
 * Returns 0 on clean finish/quit, 2 when the user ESC-skipped mid-stream
 * (scene-chaining signal: caller advances to the next scene/menu),
 * 1 on error.
 * audio=0 disables sound; start_frame skips playback forward.
 * skip_at>0: auto-trigger the ESC-skip once skip_at frames are presented
 * (deterministic test hook; never reports 2). Env LEO_DUMP_SKIP=path dumps
 * the final framebuffer as PPM after a skip (headless bug-hunting).
 * Window keys: q=quit, ESC=skip (returns 2), Space=pause/resume - the
 * pause holds the single presentation clock and works with or without an
 * audio device. */
int is_player_run(const char *path, int audio, unsigned start_frame,
                  unsigned skip_at);

#ifdef __cplusplus
}
#endif
#endif /* IS_PLAYER_H */
