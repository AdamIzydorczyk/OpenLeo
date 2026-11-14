/* ll_level.h - level intro/preview viewer (M3): resolves the plain-text
 * .DES wiring for a level, then shows the PCX art and plays the PCM
 * parallax track stored in the level's ETAPY block. Used by the ll_level
 * tool and by the demo flow (level start preview). Assumes SDL already
 * initialised; never calls SDL_Quit.
 */
#ifndef LL_LEVEL_H
#define LL_LEVEL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Shows the level for secs seconds (or until ESC), plays its PCM if
 * play_audio. Prints the .DES wiring. 0 = ok/shown, 1 = level not found. */
int ll_level_show(const char *data_dir, const char *level, double secs,
                  int play_audio);

/* Prints the level's ANIMACJE animation inventory (cutscene .SEQ scripts
 * + per-object leaf tables of animation-frame .SEQ scripts). No SDL.
 * 0 = block found, 1 = no container/block. */
int ll_level_anims(const char *data_dir, const char *level);

/* Standalone music (singleton), used by the MAPKA level-select screen:
 * the engine plays "mapka.mod" from the SOUND container on entering the
 * map (FUN_0005dec0) and stops it on every exit (FUN_0005df20).
 * ll_music_play: 0 = playing, 1 = missing/disabled (LEO_NO_MUSIC).
 * ll_music_stop: idempotent. Requires SDL (audio subsystem self-init). */
int ll_music_play(const char *data_dir, const char *modname);
void ll_music_stop(void);

/* Level names = top-level id==2 block keys of the ETAPY container, in
 * container order (verified: the CD ETAPY carries 14 such blocks).
 * Returns the count filled (max = capacity of names[][32]). */
int ll_level_names(const char *data_dir, char names[][32], int max);

#ifdef __cplusplus
}
#endif
#endif /* LL_LEVEL_H */
