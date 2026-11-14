/* is_player.h - shared SDL2 cutscene player for linux/ + win11/.
 *
 * Fixed-timestep pacing (PLAN S6 "NATIVE PORTS"): the original is
 * execution-rate bound and would run thousands of times too fast on
 * modern CPUs. Here every presented frame is one clock tick:
 *   cutscenes: period = 1000/.IS header fps byte (authoritative per file)
 *   fallback:  TICK_HZ from ll_config.h
 * Clock = SDL_GetTicks64 + SDL_Delay, with drift clamp (never spiral).
 */
#ifndef IS_PLAYER_H
#define IS_PLAYER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Plays path (a .IS file) in an SDL window with ESC-region audio.
 * Returns 0 on clean finish/quit, non-zero on error.
 * audio=0 disables sound; start_frame skips playback forward.
 * skip_at>0: auto-trigger the ESC-skip once skip_at frames are presented
 * (deterministic test hook). Env LEO_DUMP_SKIP=path dumps the final
 * framebuffer as PPM after a skip (headless bug-hunting). */
int is_player_run(const char *path, int audio, unsigned start_frame,
                  unsigned skip_at);

#ifdef __cplusplus
}
#endif
#endif /* IS_PLAYER_H */
