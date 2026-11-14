/* fs_ci.h - robust opening of game data files (review P-01/P-07).
 *
 * The engine's internal names are lowercase (status.leo, glowne.mnu) while
 * users' CD extracts are usually UPPERCASE (ANIMACJE, LEVELDES, LOGO.IS),
 * and paths arrive from argv/env/SDL_GetBasePath in mixed case.  Every
 * GAME DATA open (containers, .IS, saves/config) goes through ll_fopen_ci:
 *   1. try the exact path (fast path, byte-identical to plain fopen);
 *   2. POSIX: retry with a component-by-component case-insensitive
 *      directory scan (relative paths stay relative; absolute paths keep
 *      their root; duplicate slashes collapse - no more "/comp" probe);
 *   3. Windows: retry as UTF-8 -> wide via _wfopen, so non-ASCII paths
 *      (C:\Users\Åukasz\...) work despite the msvcrt ANSI code page.
 *
 * Write modes ("w"/"a") only get steps 1+3: creating a NEW file must not
 * silently reuse an existing different-cased one.  Returns NULL + errno
 * when nothing resolves. */
#ifndef FS_CI_H
#define FS_CI_H

#include <stdio.h>

/* fopen with the case-insensitive/UTF-8 fallbacks described above. */
FILE *ll_fopen_ci(const char *path, const char *mode);

/* malloc'd variant of path that really exists on disk (case-corrected,
 * slash-normalized), or NULL.  Same rules as ll_fopen_ci step 2. */
char *ll_resolve_ci(const char *path);

#endif /* FS_CI_H */
