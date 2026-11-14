/* ll_paths.h - read-only data vs user-writable state (tips T-17).
 *
 * The original LEO kept STATUS.LEO / LONGSOFT.CFG next to the game
 * data. On modern installs that location is read-only (Windows
 * Program Files + UAC VirtualStore, Linux /usr/share), so saves land in
 * a confusing redirected place or fail. This module gives the ONE place
 * that computes the per-user writable directory and resolves a user-
 * owned file (a save or config) there, importing a legacy copy from the
 * data dir exactly once.
 *
 *   - ll_user_dir(): $LEO_USER_DIR if set (a test/override hook, used
 *     verbatim), else the platform's per-user dir (plat_pref_dir =
 *     SDL_GetPrefPath("LongSoft","LewLeon") on the SDL backend) - on
 *     Linux ~/.local/share/LongSoft/LewLeon/, on Windows %APPDATA%\
 *     LongSoft\LewLeon\.
 *   - ll_resolve_user_file(): the LEO_SAVE-style override still wins
 *     (API preserved); otherwise the file is found/imported under the
 *     user dir. A read-only DATA copy is COPIED (never moved), so the
 *     shipped/original files stay untouched - the same rule the rest of
 *     the port follows for retail data.
 *
 * The DATA (read-only) search order itself lives in main.c
 * (pick_data_dir); this header is only about the writable side.
 */
#ifndef LL_PATHS_H
#define LL_PATHS_H

#include <stddef.h>

/* Per-user writable dir, computed + cached (created by the platform's
 * plat_pref_dir on the default path; $LEO_USER_DIR is used as-is and
 * must exist). Returns NULL only if neither is available. No trailing
 * separator. */
const char *ll_user_dir(void);

/* Join dir + name with a single '/' into out (both platforms accept
 * '/' in native paths). */
void ll_path_join(char *out, size_t n, const char *dir, const char *name);

/* Binary copy src -> dst (creates/overwrites dst). 0 = ok, -1 failed.
 * Used for the one-time legacy import; the source is never modified. */
int ll_copy_file(const char *src, const char *dst);

/* Resolve one user-owned file named `leaf` (e.g. "STATUS.LEO"):
 *   1. env_override (e.g. $LEO_SAVE) set + non-empty -> returned
 *      verbatim, *imported = 0, return 1 (this is the existing CLI/
 *      env hook; treating it as absolute preserves the current API).
 *   2. a user dir is determinable ($LEO_USER_DIR set) OR a legacy copy
 *      exists under data_dir (may be NULL):
 *        - user_dir/leaf already present -> use it, *imported = 0;
 *        - else data_dir/leaf exists -> COPY it to user_dir/leaf and
 *          use that, *imported = 1; if the copy fails, fall back to
 *          data_dir/leaf with *imported = 0.
 *   3. nothing found -> return 0, out untouched.
 * The user dir is only created (via plat_pref_dir) when there is a real
 * chance of writing, so ordinary runs with no save anywhere do nothing.
 */
int ll_resolve_user_file(char *out, size_t n, const char *env_override,
                         const char *data_dir, const char *leaf,
                         int *imported);

#endif /* LL_PATHS_H */
