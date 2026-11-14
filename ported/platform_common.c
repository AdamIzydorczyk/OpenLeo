/* platform_common.c - the backend-independent part of platform.h.
 *
 * plat_fopen is identical on every backend (the engine's data files are
 * opened the same way regardless of window/audio driver), so it lives
 * here once and links into whichever backend a target selects. It
 * delegates to the fs_ci opener (tips T-18, review P-01/P-07): exact
 * path first (byte-identical to plain fopen), then case-insensitive +
 * UTF-8/wide fallbacks. */
#include "platform.h"
#include "fs_ci.h"

FILE *plat_fopen(const char *path, const char *mode)
{
    return ll_fopen_ci(path, mode);
}
