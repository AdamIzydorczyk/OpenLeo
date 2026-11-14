/* ll_level.c - command-line front-end for ported/ll_level.c (shared viewer).
 * usage: ll_level DATA_DIR LEVEL [-t SECONDS] [--no-audio]
 *   e.g. ll_level /tmp/opencode/iso_stage/LEO RAN.01
 */
#include "ll_level.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr,
                "usage: %s DATA_DIR LEVEL [-t SECONDS] [--no-audio] "
                "[--list]\n",
                argv[0]);
        return 2;
    }
    const char *dir = argv[1], *level = argv[2];
    double secs = 5.0;
    int audio = 1;
    int list_only = 0;
    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "-t") == 0 && i + 1 < argc)
            secs = atof(argv[++i]);
        else if (strcmp(argv[i], "--no-audio") == 0)
            audio = 0;
        else if (strcmp(argv[i], "--list") == 0)
            list_only = 1;
    }
    if (list_only) /* headless: wiring + animation inventory, no SDL */
        return ll_level_anims(dir, level) ? 1 : 0;
    if (SDL_Init(SDL_INIT_VIDEO | (audio ? SDL_INIT_AUDIO : 0)) != 0) {
        fprintf(stderr, "SDL: %s\n", SDL_GetError());
        return 1;
    }
    int rc = ll_level_show(dir, level, secs, audio);
    SDL_Quit();
    return rc;
}
