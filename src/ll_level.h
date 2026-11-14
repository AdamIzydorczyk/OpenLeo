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

#ifdef __cplusplus
}
#endif
#endif /* LL_LEVEL_H */
