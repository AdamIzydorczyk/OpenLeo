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
 *      (C:\Users\Łukasz\...) work despite the msvcrt ANSI code page.
 *
 * Write modes ("w"/"a") only get steps 1+3: creating a NEW file must not
 * silently reuse an existing different-cased one.  Returns NULL + errno
 * when nothing resolves.
 *
 * P-07 (second half): plain ftell/fseek use 32-bit `long` on Win64.
 * Every size/seek on a file goes through ll_ftell64 / ll_fseek64 below
 * (fseeko/ftello on POSIX, _ftelli64/_fseeki64 on Windows - the latter
 * also compiles under MSVC, unlike fseeko). */
#ifndef FS_CI_H
#define FS_CI_H

#include <stdio.h>
#ifndef _WIN32
#include <sys/types.h>   /* off_t for fseeko/ftello */
#endif

/* fopen with the case-insensitive/UTF-8 fallbacks described above. */
FILE *ll_fopen_ci(const char *path, const char *mode);

/* malloc'd variant of path that really exists on disk (case-corrected,
 * slash-normalized), or NULL.  Same rules as ll_fopen_ci step 2. */
char *ll_resolve_ci(const char *path);

/* 64-bit file positioning (P-07): removes the 2 GB cliff on Win64. */
static inline int ll_fseek64(FILE *f, long long off, int whence)
{
#ifdef _WIN32
    return _fseeki64(f, off, whence);
#else
    return fseeko(f, (off_t)off, whence);
#endif
}

static inline long long ll_ftell64(FILE *f)
{
#ifdef _WIN32
    return _ftelli64(f);
#else
    return (long long)ftello(f);
#endif
}

#endif /* FS_CI_H */
