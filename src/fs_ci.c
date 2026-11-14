/* fs_ci.c - see fs_ci.h. A single implementation of case-insensitive /
 * UTF-8 game-data opening, replacing the per-site resolve_ci() that lived
 * only in is_player.c (review P-01). The old resolve_ci seeded its probe
 * with "", so the first probe became "/data" - an ABSOLUTE path - and every
 * relative path resolved against the filesystem root. Here a relative base
 * stays relative: the probe for the first component is the bare component
 * when the accumulated path is empty (never "/comp"). Absolute "/", "C:\"
 * and "C:/" prefixes are preserved. */
#include "fs_ci.h"
#include "ll_config.h"

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#if FS_CASE_INSENSITIVE
/* Windows (and DOS): the filesystem is already case-insensitive, so the
 * only real hazard is a non-ANSI path from SDL_main argv / SDL_GetBasePath.
 * msvcrt fopen takes the ANSI code page, so a UTF-8 path through a folder
 * like C:\Users\Åukasz\ never opens. Convert UTF-8 -> UTF-16 and _wfopen.
 * (review P-07; a UCRT + activeCodePage=UTF-8 manifest would also fix this
 * but the SDK we cross-link here is msvcrt.) */
#include <windows.h>

static FILE *wopen_utf8(const char *path, const char *mode)
{
    wchar_t pbuf[4096], mbuf[16];
    wchar_t *pw = pbuf, *mw = mbuf;
    if (MultiByteToWideChar(CP_UTF8, 0, path, -1, pbuf, 4096) == 0)
        return NULL;
    if (MultiByteToWideChar(CP_UTF8, 0, mode, -1, mbuf, 16) == 0)
        return NULL;
    return _wfopen(pw, mw);
}

FILE *ll_fopen_ci(const char *path, const char *mode)
{
    FILE *f;
    if (!path || !mode)
        return NULL;
    f = fopen(path, mode);          /* fast path / ASCII stays byte-exact */
    if (f)
        return f;
    return wopen_utf8(path, mode);
}

char *ll_resolve_ci(const char *path)
{
    /* Windows needs no case walk (already CI) and this build never probes
     * existence the way the POSIX path does; callers wanting an existence
     * test use ll_fopen_ci. */
    return path ? strdup(path) : NULL;
}

#else
/* POSIX: exact open first, then a component-by-component case-folded scan.
 * Splitting is on '/' only (a '\' is a normal filename char on unix). */
#include <sys/stat.h>
#include <dirent.h>

static int name_ci_eq(const char *a, const char *b)
{
    for (; *a && *b; a++, b++) {
        char x = *a, y = *b;
        if (x >= 'A' && x <= 'Z') x = (char)(x - 'A' + 'a');
        if (y >= 'A' && y <= 'Z') y = (char)(y - 'A' + 'a');
        if (x != y)
            return 0;
    }
    return *a == *b;
}

/* find the entry in `dir` matching `comp` case-insensitively; copy it into
 * found[n]. Returns 1 on a hit. Exact match short-circuits. */
static int dir_find_ci(const char *dir, const char *comp, char *found,
                       size_t n)
{
    DIR *d = opendir(dir);
    if (!d)
        return 0;
    int hit = 0;
    struct dirent *de;
    while ((de = readdir(d)) != NULL) {
        if (strcmp(de->d_name, comp) == 0) {   /* exact wins immediately */
            snprintf(found, n, "%s", de->d_name);
            hit = 1;
            break;
        }
    }
    if (!hit)
        for (rewinddir(d), de = readdir(d); de; de = readdir(d))
            if (name_ci_eq(de->d_name, comp)) {
                snprintf(found, n, "%s", de->d_name);
                hit = 1;
                break;
            }
    closedir(d);
    return hit;
}

char *ll_resolve_ci(const char *path)
{
    struct stat st;
    if (!path || !*path)
        return NULL;
    if (stat(path, &st) == 0)
        return strdup(path);

    char out[4096];
    out[0] = '\0';
    const char *p = path;

    /* Preserve a rooted/drive prefix so the walk never escapes it. A bare
     * relative path keeps out empty -> first probe is the bare component. */
    if (*p == '/') {
        snprintf(out, sizeof out, "/");
        p++;
    } else if (isalpha((unsigned char)p[0]) && p[1] == ':' &&
               (p[2] == '/' || p[2] == '\\')) {
        snprintf(out, sizeof out, "%c:/", p[0]);   /* "C:/" (unix-side) */
        p += 3;
    }

    while (*p) {
        while (*p == '/' || *p == '\\')
            p++;                       /* collapse duplicate separators */
        if (!*p)
            break;
        const char *sep = p;
        while (*sep && *sep != '/' && *sep != '\\')
            sep++;
        size_t len = (size_t)(sep - p);
        if (len == 0 || len >= 512)
            return NULL;
        char comp[512];
        memcpy(comp, p, len);
        comp[len] = '\0';

        /* probe without ever prepending a stray "/" to a relative base */
        char probe[4096];
        if (out[0]) {
            if (strlen(out) + 2 >= sizeof probe)
                return NULL;
            snprintf(probe, sizeof probe, "%s%s", out,
                     out[strlen(out) - 1] == '/' ? "" : "/");
        } else
            probe[0] = '\0';
        strncat(probe, comp, sizeof probe - strlen(probe) - 1);

        char chosen[512];
        if (stat(probe, &st) == 0) {
            snprintf(chosen, sizeof chosen, "%s", comp);
        } else {
            const char *dir = out[0] ? out : ".";
            if (!dir_find_ci(dir, comp, chosen, sizeof chosen))
                return NULL;
        }
        if (out[0] && out[strlen(out) - 1] != '/') {
            if (strlen(out) + 1 >= sizeof out)
                return NULL;
            strcat(out, "/");
        }
        if (strlen(out) + strlen(chosen) >= sizeof out)
            return NULL;
        strcat(out, chosen);
        p = sep;
    }
    if (!out[0] || stat(out, &st) != 0)
        return NULL;
    return strdup(out);
}

FILE *ll_fopen_ci(const char *path, const char *mode)
{
    FILE *f;
    if (!path || !mode)
        return NULL;
    f = fopen(path, mode);
    if (f)
        return f;
    if (mode[0] != 'r')          /* only read opens do a case-walk */
        return NULL;
    char *rp = ll_resolve_ci(path);
    if (!rp)
        return NULL;
    f = fopen(rp, mode);
    free(rp);
    return f;
}
#endif /* FS_CASE_INSENSITIVE */
