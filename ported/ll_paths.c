/* ll_paths.c - user-writable state location (see ll_paths.h, T-17).
 * Deliberately small: it only locates the per-user dir and imports a
 * legacy file once. All byte-level save/config parsing stays in
 * ll_save.c. The per-user dir itself is the platform's to know
 * (plat_pref_dir) - this file contains no SDL. */
#include "ll_paths.h"
#include "platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static void strip_sep(char *p)
{
    size_t l = strlen(p);
    while (l && (p[l - 1] == '/' || p[l - 1] == '\\'))
        p[--l] = '\0';
}

const char *ll_user_dir(void)
{
    static char cached[4096];
    if (cached[0])
        return cached;
    const char *o = getenv("LEO_USER_DIR");
    if (o && *o) {                       /* test/override: use verbatim */
        snprintf(cached, sizeof cached, "%s", o);
        strip_sep(cached);
        return cached;
    }
    char dirbuf[4096];
    if (plat_pref_dir(dirbuf, sizeof dirbuf) != 0)
        return NULL;
    snprintf(cached, sizeof cached, "%s", dirbuf);
    strip_sep(cached);
    return cached;
}

void ll_path_join(char *out, size_t n, const char *dir, const char *name)
{
    snprintf(out, n, "%s/%s", dir, name);
}

int ll_copy_file(const char *src, const char *dst)
{
    FILE *in = fopen(src, "rb");
    if (!in)
        return -1;
    FILE *out = fopen(dst, "wb");
    if (!out) {
        fclose(in);
        return -1;
    }
    int rc = 0;
    unsigned char buf[8192];
    size_t got;
    while ((got = fread(buf, 1, sizeof buf, in)) > 0)
        if (fwrite(buf, 1, got, out) != got) {
            rc = -1;
            break;
        }
    if (ferror(in))
        rc = -1;
    fclose(in);
    fclose(out);
    return rc;
}

static int exists(const char *path)
{
    struct stat st;
    return path && *path && stat(path, &st) == 0;
}

int ll_resolve_user_file(char *out, size_t n, const char *env_override,
                         const char *data_dir, const char *leaf,
                         int *imported)
{
    if (imported)
        *imported = 0;

    /* 1. explicit override wins, verbatim (preserves LEO_SAVE). */
    if (env_override && *env_override) {
        snprintf(out, n, "%s", env_override);
        return 1;
    }

    char data_path[4096];
    ll_path_join(data_path, sizeof data_path, data_dir ? data_dir : "", leaf);
    int legacy = data_dir && *data_dir && exists(data_path);

    /* Only resolve the user dir when $LEO_USER_DIR was given (no
     * plat_pref_dir side effect on ordinary runs) or a legacy import
     * is actually possible. */
    const char *ud = (getenv("LEO_USER_DIR") || legacy) ? ll_user_dir()
                                                        : NULL;
    if (!ud)
        return 0;

    char user_path[4096];
    ll_path_join(user_path, sizeof user_path, ud, leaf);
    if (exists(user_path)) {
        snprintf(out, n, "%s", user_path);
        return 1;
    }
    if (legacy) {
        if (ll_copy_file(data_path, user_path) == 0) {
            if (imported)
                *imported = 1;
            snprintf(out, n, "%s", user_path);
        } else {
            snprintf(out, n, "%s", data_path);   /* copy failed: read old */
        }
        return 1;
    }
    return 0;
}
