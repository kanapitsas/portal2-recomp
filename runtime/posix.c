// Fichiers et répertoires POSIX avec la sémantique glibc i386 / Linux.
// Toute modification d'un chemin du jeu passe par rt_write_path : la copie
// originale et le dépôt Linux ne sont jamais modifiés. Une suppression ou un
// renommage d'un fichier original laisse une marque de suppression dans les
// données utilisateur (même principe que les « whiteouts » d'overlayfs) ;
// renommer un répertoire original renvoie EXDEV, comme overlayfs.
// SHIMS: mkdir rmdir unlink remove rename chmod chown lchown link symlink realpath
// SHIMS: scandir scandir64 alphasort alphasort64 readdir64 statfs statfs64 statvfs statvfs64
// SHIMS: ftruncate fdopen freopen tmpfile tmpnam mkstemp mkfifo creat dup dup2 pipe
// SHIMS: pathconf faccessat futimes utimes flock __xmknod setvbuf ungetc getwc ungetwc
// SHIMS: putwc putchar writev ioctl fcntl fcntl64
#include "posix.h"
int rt_trace_files(const char *path);   // libc.c
#include "modules.h"
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <unistd.h>
#include <wchar.h>

static uint32_t arg(CPU *c, unsigned i) { return R32(c->esp + 4 + 4 * i); }
static const char *string(CPU *c, uint32_t address) { rt_string_length(c, address); return GPTR(address); }
static void finish(CPU *c, int result) { if (result < 0) rt_save_errno(c); c->eax = (uint32_t)result; }
static void fail(CPU *c, int linux_errno) { rt_set_errno(c, linux_errno); c->eax = UINT32_MAX; }

// Copie un résultat de rt_read_path/rt_write_path (tampons tournants).
static int keep(char out[PATH_MAX], const char *path) {
    if (!path) return -1;
    if (strlen(path) >= PATH_MAX) { errno = ENAMETOOLONG; return -1; }
    strcpy(out, path); return 0;
}
// Chemin visible dans la vue fusionnée, et son lstat.
static int visible(const char *path, char out[PATH_MAX], struct stat *info) {
    if (keep(out, rt_read_path(path))) return -1;
    return lstat(out, info);
}
static int create_target(const char *path, char out[PATH_MAX]) {
    char seen[PATH_MAX]; struct stat info;
    if (!visible(path, seen, &info)) { errno = EEXIST; return -1; }
    if (errno != ENOENT) return -1;
    return keep(out, rt_write_path(path, 0));
}
// Copie utilisateur d'un fichier existant (copie à l'écriture).
static int copy_up(const char *path, char out[PATH_MAX]) {
    char relative[PATH_MAX]; int inside = rt_path_relative(path, relative);
    if (inside < 0) return -1;
    if (!inside) return keep(out, path);
    char seen[PATH_MAX]; struct stat info;
    if (visible(path, seen, &info)) return -1;
    if (S_ISDIR(info.st_mode)) {
        if (keep(out, rt_write_path(path, 0))) return -1;
        return mkdir(out, info.st_mode & 07777) && errno != EEXIST ? -1 : 0;
    }
    if (keep(out, rt_write_path(path, 1))) return -1;
    struct stat copied;
    if (!lstat(out, &copied) && !S_ISDIR(copied.st_mode) && copied.st_mode != info.st_mode)
        chmod(out, info.st_mode & 07777);
    return 0;
}
static int path_lower_exists(const char *relative) {
    char lower[2][PATH_MAX];
    return rt_path_lower(relative, lower) > 0;
}

static int do_unlink(const char *path, int directory) {
    char relative[PATH_MAX]; int inside = rt_path_relative(path, relative);
    if (inside < 0) return -1;
    if (!inside) return directory ? rmdir(path) : unlink(path);
    if (!*relative) { errno = directory ? EBUSY : EISDIR; return -1; }
    char seen[PATH_MAX], overlay[PATH_MAX]; struct stat info;
    // Un lien créé par l'invité se supprime lui-même, sans regarder sa cible.
    if (rt_path_overlay(relative, overlay)) return -1;
    if (!lstat(overlay, &info) && S_ISLNK(info.st_mode)) {
        if (directory) { errno = ENOTDIR; return -1; }
        if (unlink(overlay)) return -1;
        return path_lower_exists(relative) ? rt_path_whiteout(relative) : 0;
    }
    if (visible(path, seen, &info)) return -1;
    if (directory && !S_ISDIR(info.st_mode)) { errno = ENOTDIR; return -1; }
    if (!directory && S_ISDIR(info.st_mode)) { errno = EISDIR; return -1; }
    if (directory) {
        RtDirEntry *entries; size_t count, names = 0;
        if (rt_dir_entries(path, &entries, &count)) return -1;
        for (size_t i = 0; i < count; ++i)
            if (strcmp(entries[i].name, ".") && strcmp(entries[i].name, "..")) ++names;
        rt_dir_entries_free(entries, count);
        if (names) { errno = ENOTEMPTY; return -1; }
    }
    if (rt_path_overlay(relative, overlay)) return -1;
    struct stat own;
    if (!lstat(overlay, &own) && (directory ? rmdir(overlay) : unlink(overlay))) return -1;
    if (path_lower_exists(relative) && rt_path_whiteout(relative)) return -1;
    return 0;
}

static int copy_file(const char *from, const char *to, mode_t mode) {
    char temporary[PATH_MAX];
    if (snprintf(temporary, sizeof(temporary), "%s.rt-rename-XXXXXX", to) >= (int)sizeof(temporary)) { errno = ENAMETOOLONG; return -1; }
    int output = mkstemp(temporary);
    if (output < 0) return -1;
    int input = open(from, O_RDONLY), failed = input < 0;
    char block[65536];
    while (!failed) {
        ssize_t size = read(input, block, sizeof(block));
        if (size < 0) { failed = 1; break; }
        if (!size) break;
        for (ssize_t done = 0; done < size;) {
            ssize_t wrote = write(output, block + done, (size_t)(size - done));
            if (wrote < 0) { failed = 1; break; }
            done += wrote;
        }
    }
    int error = errno;
    if (input >= 0) close(input);
    if (!failed && fchmod(output, mode & 07777)) { failed = 1; error = errno; }
    if (close(output) && !failed) { failed = 1; error = errno; }
    if (!failed && rename(temporary, to)) { failed = 1; error = errno; }
    if (failed) { unlink(temporary); errno = error; return -1; }
    return 0;
}

static int do_rename(const char *from, const char *to) {
    char from_relative[PATH_MAX], to_relative[PATH_MAX], seen[PATH_MAX], target[PATH_MAX];
    int from_inside = rt_path_relative(from, from_relative), to_inside = rt_path_relative(to, to_relative);
    if (from_inside < 0 || to_inside < 0) return -1;
    struct stat info, existing;
    if (visible(from, seen, &info)) return -1;
    char to_seen[PATH_MAX];
    int to_exists = !visible(to, to_seen, &existing);
    if (!to_exists && errno != ENOENT) return -1;
    if (to_exists && existing.st_dev == info.st_dev && existing.st_ino == info.st_ino) return 0;
    if (to_exists && S_ISDIR(existing.st_mode) && !S_ISDIR(info.st_mode)) { errno = EISDIR; return -1; }
    if (to_exists && !S_ISDIR(existing.st_mode) && S_ISDIR(info.st_mode)) { errno = ENOTDIR; return -1; }
    char to_overlay[PATH_MAX]; struct stat own;
    // Un répertoire cible présent dans l'original ne peut pas être remplacé.
    if (to_exists && S_ISDIR(existing.st_mode) && to_inside &&
        (rt_path_overlay(to_relative, to_overlay) || lstat(to_overlay, &own))) { errno = EXDEV; return -1; }
    if (keep(target, to_inside ? rt_write_path_nofollow(to) : to)) return -1;
    if (!from_inside) {
        if (rename(from, target)) return -1;
    } else {
        if (!*from_relative) { errno = EBUSY; return -1; }
        char overlay[PATH_MAX];
        if (rt_path_overlay(from_relative, overlay)) return -1;
        int lower = path_lower_exists(from_relative) && rt_path_is_whiteout(from_relative) != 1;
        if (!lstat(overlay, &own)) {
            if (S_ISDIR(own.st_mode) && lower) { errno = EXDEV; return -1; }
            if (rename(overlay, target)) return -1;
        } else if (S_ISDIR(info.st_mode)) {
            errno = EXDEV; return -1;
        } else if (S_ISLNK(info.st_mode)) {
            char link_target[PATH_MAX]; ssize_t length = readlink(seen, link_target, sizeof(link_target) - 1);
            if (length < 0) return -1;
            link_target[length] = 0;
            if (unlink(target) && errno != ENOENT) return -1;
            if (symlink(link_target, target)) return -1;
        } else if (copy_file(seen, target, info.st_mode)) return -1;
        if (lower && rt_path_whiteout(from_relative)) return -1;
    }
    if (to_inside && *to_relative) rt_path_clear_whiteout(to_relative);
    return 0;
}

// Tri stable par fusion avec un comparateur invité ; la paire comparée est
// placée dans une zone invitée, comme les éléments que qsort transmet.
static void merge_sort(CPU *c, uint32_t *items, uint32_t *scratch, size_t count, uint32_t compar, uint32_t pair) {
    if (count < 2) return;
    size_t half = count / 2;
    merge_sort(c, items, scratch, half, compar, pair);
    merge_sort(c, items + half, scratch, count - half, compar, pair);
    size_t i = 0, j = half, k = 0;
    while (i < half && j < count) {
        W32(pair, items[i]); W32(pair + 4, items[j]);
        uint32_t args[2] = {pair, pair + 4};
        if ((int32_t)rt_invoke(c, compar, args, 2) <= 0) scratch[k++] = items[i++];
        else scratch[k++] = items[j++];
    }
    while (i < half) scratch[k++] = items[i++];
    while (j < count) scratch[k++] = items[j++];
    memcpy(items, scratch, count * sizeof(*items));
}

static void scandir_shim(CPU *c, int wide) {
    const char *path = string(c, arg(c, 0));
    uint32_t list = arg(c, 1), filter = arg(c, 2), compar = arg(c, 3);
    RtDirEntry *entries; size_t count;
    if (rt_dir_entries(path, &entries, &count)) { rt_save_errno(c); c->eax = UINT32_MAX; return; }
    uint32_t *items = calloc(count ? count : 1, sizeof(*items)), *scratch = calloc(count ? count : 1, sizeof(*items));
    size_t kept = 0; int error = 0;
    if (!items || !scratch) error = ENOMEM;
    for (size_t i = 0; i < count && !error; ++i) {
        if (strlen(entries[i].name) > 255) { error = 36; break; }
        if (!wide && entries[i].ino > UINT32_MAX) { error = 75; break; }
        uint32_t entry = rt_alloc(rt_dirent_size(wide, strlen(entries[i].name)));
        if (!entry) { error = 12; break; }
        rt_write_dirent(entry, wide, &entries[i], i + 1);
        if (filter && !rt_invoke(c, filter, &entry, 1)) { rt_free(entry); continue; }
        items[kept++] = entry;
    }
    rt_dir_entries_free(entries, count);
    uint32_t array = 0;
    if (!error && kept) {
        if (compar) {
            uint32_t pair = rt_alloc(8);
            if (!pair) error = 12;
            else { merge_sort(c, items, scratch, kept, compar, pair); rt_free(pair); }
        }
        if (!error && !(array = rt_alloc((uint32_t)kept * 4))) error = 12;
    }
    if (error) {
        for (size_t i = 0; i < kept; ++i) rt_free(items[i]);
        free(items); free(scratch);
        rt_set_errno(c, error == ENOMEM ? 12 : error); c->eax = UINT32_MAX; return;
    }
    for (size_t i = 0; i < kept; ++i) W32(array + (uint32_t)i * 4, items[i]);
    free(items); free(scratch);
    rt_write_span(c, list, 4); W32(list, array);
    c->eax = (uint32_t)kept;
}

// Magic Linux du système de fichiers hôte (statfs.f_type). 0 si inconnu.
static uint32_t linux_fs_magic(const char *type) {
    static const struct { const char *name; uint32_t magic; } table[] = {
        {"hfs", 0x482b},            // HFSPLUS_SUPER_MAGIC
        {"apfs", 0x4253584e},       // pilote Linux apfs : magic du conteneur « NXSB »
        {"msdos", 0x4d44}, {"exfat", 0x2011bab0}, {"ntfs", 0x5346544e},
        {"nfs", 0x6969}, {"smbfs", 0xff534d42}, {"devfs", 0x1373}, {"autofs", 0x0187},
        {"webdav", 0}, {"tmpfs", 0x01021994},
    };
    for (size_t i = 0; i < sizeof(table) / sizeof(*table); ++i)
        if (!strcmp(type, table[i].name)) return table[i].magic;
    return 0;
}
static uint32_t linux_mount_flags(uint32_t flags) {
    uint32_t result = 0;
    if (flags & MNT_RDONLY) result |= 1;
    if (flags & MNT_NOSUID) result |= 2;
    if (flags & MNT_NODEV) result |= 4;
    if (flags & MNT_NOEXEC) result |= 8;
    if (flags & MNT_SYNCHRONOUS) result |= 16;
    if (flags & MNT_NOATIME) result |= 1024;
    return result;
}
// Disposition glibc i386 : bits/statfs.h et bits/statvfs.h.
static int write_statfs(CPU *c, uint32_t address, const struct statfs *s, int vfs, int wide) {
    uint64_t counts[] = {s->f_blocks, s->f_bfree, s->f_bavail, s->f_files, s->f_ffree, s->f_ffree};
    if (!wide) for (unsigned i = 0; i < 6; ++i) if (counts[i] > UINT32_MAX) { errno = EOVERFLOW; return -1; }
    uint32_t size = vfs ? (wide ? 96 : 72) : (wide ? 84 : 64), at = address;
    memset(rt_write_span(c, address, size), 0, size);
    uint32_t bsize = s->f_bsize;
    if (vfs) { W32(at, bsize); W32(at + 4, bsize); at += 8; }
    else { W32(at, linux_fs_magic(s->f_fstypename)); W32(at + 4, bsize); at += 8; }
    for (unsigned i = 0; i < (vfs ? 6u : 5u); ++i) {
        if (wide) { W64(at, counts[i]); at += 8; } else { W32(at, (uint32_t)counts[i]); at += 4; }
    }
    if (vfs) {
        W32(at, (uint32_t)s->f_fsid.val[0]); W32(at + 4, 0);
        W32(at + 8, linux_mount_flags(s->f_flags)); W32(at + 12, 255);
    } else {
        W32(at, (uint32_t)s->f_fsid.val[0]); W32(at + 4, (uint32_t)s->f_fsid.val[1]);
        W32(at + 8, 255); W32(at + 12, bsize); W32(at + 16, linux_mount_flags(s->f_flags) | 0x20); // ST_VALID
    }
    return 0;
}

static int linux_ioctl(CPU *c, int fd, uint32_t request, uint32_t value) {
    switch (request) {
    case 0x5421: { // FIONBIO
        rt_span(c, value, 4);
        int enable = (int32_t)R32(value);
        return ioctl(fd, FIONBIO, &enable);
    }
    case 0x541b: { // FIONREAD
        int available = 0;
        rt_write_span(c, value, 4);
        if (ioctl(fd, FIONREAD, &available)) return -1;
        W32(value, (uint32_t)available); return 0;
    }
    case 0x5413: { // TIOCGWINSZ : struct winsize identique (4 × u16)
        struct winsize size;
        rt_write_span(c, value, 8);
        if (ioctl(fd, TIOCGWINSZ, &size)) return -1;
        W16(value, size.ws_row); W16(value + 2, size.ws_col); W16(value + 4, size.ws_xpixel); W16(value + 6, size.ws_ypixel);
        return 0;
    }
    case 0x5451: return fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC) < 0 ? -1 : 0;  // FIOCLEX
    case 0x5450: return fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) & ~FD_CLOEXEC) < 0 ? -1 : 0; // FIONCLEX
    case 0x5401: // TCGETS (isatty) : seul le cas « pas un terminal » est traduit
        if (fcntl(fd, F_GETFD) < 0) return -1;
        if (!isatty(fd)) { errno = ENOTTY; return -1; }
        rt_trap(c, c->eip, "ioctl TCGETS on a terminal: termios translation not implemented");
    default:
        fprintf(stderr, "unimplemented Linux ioctl %#x\n", request);
        rt_trap(c, c->eip, "unimplemented Linux ioctl request");
    }
}

// Suffixe aléatoire de tmpnam/mkstemp : 62^6 = 56 800 235 584 noms, glibc
// tente TMP_MAX (238 328) tirages avant EEXIST.
#define TMP_ATTEMPTS 238328
static void random_suffix(char *out) {
    static const char letters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < 6; ++i) out[i] = letters[arc4random_uniform(62)];
}

static int posix_shim(CPU *c, const char *name) {
    char a[PATH_MAX], b[PATH_MAX];
    struct stat info;
    if (!strcmp(name, "mkdir")) {
        const char *path = string(c, arg(c, 0));
        char relative[PATH_MAX]; int inside = rt_path_relative(path, relative);
        int result = inside < 0 || create_target(path, a) ? -1 : mkdir(a, (mode_t)arg(c, 1));
        if (!result && inside > 0 && *relative) rt_path_clear_whiteout(relative);
        finish(c, result);
    } else if (!strcmp(name, "rmdir")) {
        finish(c, do_unlink(string(c, arg(c, 0)), 1));
    } else if (!strcmp(name, "unlink")) {
        int result = do_unlink(string(c, arg(c, 0)), 0);
        if (rt_trace_files(string(c, arg(c, 0)))) { int kept = errno; fprintf(stderr, "[fichiers] unlink %s : %s\n", string(c, arg(c, 0)), result ? strerror(errno) : "ok"); errno = kept; }
        finish(c, result);
    } else if (!strcmp(name, "remove")) {
        const char *path = string(c, arg(c, 0));
        int result = do_unlink(path, 0);
        if (result && errno == EISDIR) result = do_unlink(path, 1);
        finish(c, result);
    } else if (!strcmp(name, "rename")) {
        int result = do_rename(string(c, arg(c, 0)), string(c, arg(c, 1)));
        if (rt_trace_files(string(c, arg(c, 0))) || rt_trace_files(string(c, arg(c, 1)))) { int kept = errno; fprintf(stderr, "[fichiers] rename %s -> %s : %s\n", string(c, arg(c, 0)), string(c, arg(c, 1)), result ? strerror(errno) : "ok"); errno = kept; }
        finish(c, result);
    } else if (!strcmp(name, "chmod")) {
        finish(c, copy_up(string(c, arg(c, 0)), a) ? -1 : chmod(a, (mode_t)arg(c, 1)));
    } else if (!strcmp(name, "chown") || !strcmp(name, "lchown")) {
        int result = copy_up(string(c, arg(c, 0)), a) ? -1 :
            (!strcmp(name, "chown") ? chown : lchown)(a, (uid_t)arg(c, 1), (gid_t)arg(c, 2));
        finish(c, result);
    } else if (!strcmp(name, "link")) {
        const char *from = string(c, arg(c, 0)), *to = string(c, arg(c, 1));
        finish(c, copy_up(from, a) || create_target(to, b) ? -1 : link(a, b));
    } else if (!strcmp(name, "symlink")) {
        const char *target = string(c, arg(c, 0)), *path = string(c, arg(c, 1));
        finish(c, create_target(path, b) ? -1 : symlink(target, b));
    } else if (!strcmp(name, "realpath")) {
        const char *path = string(c, arg(c, 0)); uint32_t out = arg(c, 1);
        c->eax = 0;
        if (rt_guest_realpath(path, a)) { rt_save_errno(c); return 1; }
        size_t size = strlen(a) + 1;
        if (size > 4096) { rt_set_errno(c, 36); return 1; }
        if (!out && !(out = rt_alloc((uint32_t)size))) { rt_set_errno(c, 12); return 1; }
        memcpy(rt_write_span(c, out, size), a, size);
        c->eax = out;
    } else if (!strcmp(name, "scandir") || !strcmp(name, "scandir64")) {
        scandir_shim(c, !strcmp(name, "scandir64"));
    } else if (!strcmp(name, "alphasort") || !strcmp(name, "alphasort64")) {
        // glibc : strcoll sur d_name ; le runtime fonctionne en locale « C ».
        unsigned offset = !strcmp(name, "alphasort64") ? 19 : 11;
        uint32_t lhs = R32(arg(c, 0)) + offset, rhs = R32(arg(c, 1)) + offset;
        c->eax = (uint32_t)strcmp(string(c, lhs), string(c, rhs));
    } else if (!strcmp(name, "readdir64")) {
        return rt_libc_shim(c, name);
    } else if (!strncmp(name, "statfs", 6) || !strncmp(name, "statvfs", 7)) {
        int vfs = name[4] == 'v', wide = strstr(name, "64") != NULL;
        struct statfs s;
        if (keep(a, rt_read_path(string(c, arg(c, 0))))) { finish(c, -1); return 1; }
        int result = statfs(a, &s);
        if (!result) result = write_statfs(c, arg(c, 1), &s, vfs, wide);
        finish(c, result);
    } else if (!strcmp(name, "ftruncate")) {
        finish(c, ftruncate((int32_t)arg(c, 0), (off_t)(int32_t)arg(c, 1)));
    } else if (!strcmp(name, "dup")) {
        finish(c, dup((int32_t)arg(c, 0)));
    } else if (!strcmp(name, "dup2")) {
        finish(c, dup2((int32_t)arg(c, 0), (int32_t)arg(c, 1)));
    } else if (!strcmp(name, "pipe")) {
        uint32_t out = arg(c, 0); int fds[2];
        rt_write_span(c, out, 8);
        int result = pipe(fds);
        if (!result) { W32(out, (uint32_t)fds[0]); W32(out + 4, (uint32_t)fds[1]); }
        finish(c, result);
    } else if (!strcmp(name, "creat")) {
        const char *path = string(c, arg(c, 0));
        finish(c, keep(a, rt_write_path(path, 0)) ? -1 : open(a, O_CREAT | O_WRONLY | O_TRUNC, (mode_t)arg(c, 1)));
    } else if (!strcmp(name, "mkfifo")) {
        finish(c, create_target(string(c, arg(c, 0)), a) ? -1 : mkfifo(a, (mode_t)arg(c, 1)));
    } else if (!strcmp(name, "__xmknod")) {
        if (arg(c, 0) != 1) { fail(c, 22); return 1; } // _MKNOD_VER_LINUX
        const char *path = string(c, arg(c, 1)); uint32_t mode = arg(c, 2);
        rt_span(c, arg(c, 3), 8);
        switch (mode & 0170000) {
        case 0010000: finish(c, create_target(path, a) ? -1 : mkfifo(a, mode & 07777)); break;
        case 0: case 0100000: {
            int fd = create_target(path, a) ? -1 : open(a, O_CREAT | O_EXCL | O_WRONLY, mode & 07777);
            finish(c, fd < 0 ? -1 : close(fd));
            break;
        }
        case 0020000: case 0060000:
            if (geteuid()) { fail(c, 1); break; } // EPERM sans CAP_MKNOD
            rt_trap(c, c->eip, "mknod device node as root is not supported");
        default: rt_trap(c, c->eip, "mknod file type not supported");
        }
    } else if (!strcmp(name, "pathconf")) {
        int key = (int32_t)arg(c, 1), native;
        switch (key) {
        case 0: native = _PC_LINK_MAX; break; case 1: native = _PC_MAX_CANON; break;
        case 2: native = _PC_MAX_INPUT; break; case 3: native = _PC_NAME_MAX; break;
        case 4: native = _PC_PATH_MAX; break; case 5: native = _PC_PIPE_BUF; break;
        case 6: native = _PC_CHOWN_RESTRICTED; break; case 7: native = _PC_NO_TRUNC; break;
        case 8: native = _PC_VDISABLE; break; case 13: native = _PC_FILESIZEBITS; break;
        default:
            if (key < 0 || key > 20) { fail(c, 22); return 1; }
            rt_trap(c, c->eip, "pathconf name not translated");
        }
        if (keep(a, rt_read_path(string(c, arg(c, 0))))) { finish(c, -1); return 1; }
        errno = 0; long result = pathconf(a, native);
        if (result == -1 && errno) rt_save_errno(c);
        c->eax = (uint32_t)result;
    } else if (!strcmp(name, "faccessat")) {
        int dirfd = (int32_t)arg(c, 0); const char *path = string(c, arg(c, 1));
        uint32_t mode = arg(c, 2), flags = arg(c, 3);
        if (flags & ~0x300u || mode & ~7u) { fail(c, 22); return 1; }
        int native = (flags & 0x200 ? AT_EACCESS : 0) | (flags & 0x100 ? AT_SYMLINK_NOFOLLOW : 0);
        int result;
        if (dirfd == -100 || path[0] == '/') result = keep(a, rt_read_path(path)) ? -1 : faccessat(AT_FDCWD, a, (int)mode, native);
        else result = faccessat(dirfd, path, (int)mode, native);
        finish(c, result);
    } else if (!strcmp(name, "futimes") || !strcmp(name, "utimes")) {
        uint32_t times = arg(c, 1); struct timeval tv[2], *pointer = NULL;
        if (times) {
            rt_span(c, times, 16);
            for (int i = 0; i < 2; ++i) { tv[i].tv_sec = (int32_t)R32(times + 8 * i); tv[i].tv_usec = (int32_t)R32(times + 8 * i + 4); }
            pointer = tv;
        }
        int result;
        if (!strcmp(name, "futimes")) result = futimes((int32_t)arg(c, 0), pointer);
        else result = copy_up(string(c, arg(c, 0)), a) ? -1 : utimes(a, pointer);
        finish(c, result);
    } else if (!strcmp(name, "flock")) {
        int fd = (int32_t)arg(c, 0), operation = (int32_t)arg(c, 1);
        unsigned saved = operation & LOCK_NB ? 0 : rt_guest_suspend();
        int result = flock(fd, operation), error = errno;
        if (!(operation & LOCK_NB)) rt_guest_resume(saved);
        errno = error; finish(c, result);
    } else if (!strcmp(name, "writev")) {
        int fd = (int32_t)arg(c, 0); uint32_t vector = arg(c, 1); int32_t count = (int32_t)arg(c, 2);
        if (count < 0 || count > 1024) { fail(c, 22); return 1; }
        struct iovec iov[1024];
        rt_span(c, vector, (uint64_t)count * 8);
        uint64_t total = 0;
        for (int32_t i = 0; i < count; ++i) {
            uint32_t base = R32(vector + 8 * (uint32_t)i), length = R32(vector + 8 * (uint32_t)i + 4);
            total += length;
            if (total > INT32_MAX) { fail(c, 22); return 1; }
            iov[i].iov_base = length ? rt_span(c, base, length) : NULL; iov[i].iov_len = length;
        }
        finish(c, (int)writev(fd, iov, count));
    } else if (!strcmp(name, "fcntl") || !strcmp(name, "fcntl64")) {
        // Commandes Linux i386 : F_DUPFD 0, F_GETFD 1, F_SETFD 2, F_GETFL 3, F_SETFL 4,
        // F_DUPFD_CLOEXEC 1030. FD_CLOEXEC vaut 1 des deux côtés. Les verrous (F_*LK) et
        // les autres commandes ne sont pas traduits : EINVAL plutôt qu'un faux succès.
        int fd = (int32_t)arg(c, 0); uint32_t command = arg(c, 1), value = arg(c, 2);
        if (command == 0 || command == 1030) {
            finish(c, fcntl(fd, command == 0 ? F_DUPFD : F_DUPFD_CLOEXEC, (int32_t)value));
        } else if (command == 1) {
            finish(c, fcntl(fd, F_GETFD));
        } else if (command == 2) {
            finish(c, fcntl(fd, F_SETFD, value & 1 ? FD_CLOEXEC : 0));
        } else if (command == 3) {
            int native = fcntl(fd, F_GETFL);
            if (native < 0) { finish(c, native); return 1; }
            uint32_t flags = (uint32_t)(native & O_ACCMODE);
            if (native & O_APPEND) flags |= 0x400;
            if (native & O_NONBLOCK) flags |= 0x800;
            if (native & O_SYNC) flags |= 0x101000;
            c->eax = flags;
        } else if (command == 4) {
            if (value & ~(3u | 0x400u | 0x800u | 0x8000u)) { fail(c, 22); return 1; }  // O_ASYNC, O_DIRECT...
            int native = fcntl(fd, F_GETFL);
            if (native < 0) { finish(c, native); return 1; }
            native &= ~(O_APPEND | O_NONBLOCK);
            if (value & 0x400) native |= O_APPEND;
            if (value & 0x800) native |= O_NONBLOCK;
            finish(c, fcntl(fd, F_SETFL, native));
        } else {
            fail(c, 22);
        }
    } else if (!strcmp(name, "ioctl")) {
        finish(c, linux_ioctl(c, (int32_t)arg(c, 0), arg(c, 1), arg(c, 2)));
    } else if (!strcmp(name, "fdopen")) {
        const char *mode = string(c, arg(c, 1));
        FILE *file = fdopen((int32_t)arg(c, 0), mode);
        c->eax = file ? rt_stream_new(file) : 0;
        if (!file) rt_save_errno(c);
    } else if (!strcmp(name, "freopen")) {
        uint32_t path_address = arg(c, 0), handle = arg(c, 2);
        const char *mode = string(c, arg(c, 1));
        FILE *file = rt_stream_file(c, handle);
        const char *path = path_address ? rt_stream_path(string(c, path_address), mode) : NULL;
        FILE *result = path_address && !path ? NULL : freopen(path, mode, file);
        if (!result) {
            int error = errno;
            if (path_address && !path) fclose(file);
            if (handle == rt_standard_stream(0) || handle == rt_standard_stream(1) || handle == rt_standard_stream(2))
                rt_trap(c, c->eip, "freopen failed on a standard stream");
            rt_stream_take(c, handle);
            errno = error; rt_save_errno(c); c->eax = 0;
        } else if (result != file) rt_trap(c, c->eip, "host freopen moved the stream");
        else c->eax = handle;
    } else if (!strcmp(name, "tmpfile")) {
        FILE *file = tmpfile();
        c->eax = file ? rt_stream_new(file) : 0;
        if (!file) rt_save_errno(c);
        else if (!c->eax) { fclose(file); rt_set_errno(c, 12); }
    } else if (!strcmp(name, "tmpnam")) {
        // glibc : P_tmpdir "/tmp", préfixe « file », L_tmpnam = 20.
        static uint32_t internal;
        uint32_t out = arg(c, 0);
        c->eax = 0;
        for (int attempt = 0; attempt < TMP_ATTEMPTS; ++attempt) {
            strcpy(a, "/tmp/file");
            random_suffix(a + 9);
            a[15] = 0;
            if (!lstat(a, &info)) continue;
            if (errno != ENOENT) { rt_save_errno(c); return 1; }
            if (!out) {
                if (!internal) internal = rt_alloc(20);
                if (!internal) { rt_set_errno(c, 12); return 1; }
                out = internal;
            }
            memcpy(rt_write_span(c, out, 16), a, 16);
            c->eax = out; return 1;
        }
        rt_set_errno(c, 17);
    } else if (!strcmp(name, "mkstemp")) {
        uint32_t template = arg(c, 0), length = rt_string_length(c, template);
        if (length < 6 || memcmp(GPTR(template + length - 6), "XXXXXX", 6) || length >= PATH_MAX) { fail(c, 22); return 1; }
        memcpy(b, GPTR(template), length + 1);
        for (int attempt = 0; attempt < TMP_ATTEMPTS; ++attempt) {
            random_suffix(b + length - 6);
            if (create_target(b, a)) { if (errno == EEXIST) continue; finish(c, -1); return 1; }
            int fd = open(a, O_RDWR | O_CREAT | O_EXCL, 0600);
            if (fd < 0) { if (errno == EEXIST) continue; finish(c, -1); return 1; }
            memcpy(rt_write_span(c, template, length), b, length);
            c->eax = (uint32_t)fd; return 1;
        }
        fail(c, 17);
    } else if (!strcmp(name, "setvbuf")) {
        FILE *file = rt_stream_file(c, arg(c, 0));
        uint32_t buffer = arg(c, 1), mode = arg(c, 2), size = arg(c, 3);
        if (mode > 2) { fail(c, 22); return 1; } // _IOFBF/_IOLBF/_IONBF identiques
        // Le tampon invité est en mémoire invitée projetée, donc valide pour l'hôte.
        char *native = buffer && size ? rt_write_span(c, buffer, size) : NULL;
        finish(c, setvbuf(file, native, (int)mode, size));
    } else if (!strcmp(name, "ungetc")) {
        FILE *file = rt_stream_file(c, arg(c, 1));
        c->eax = (uint32_t)ungetc((int32_t)arg(c, 0), file);
    } else if (!strcmp(name, "getwc")) {
        FILE *file = rt_stream_file(c, arg(c, 0));
        wint_t result = getwc(file);
        if (result == WEOF && ferror(file)) rt_save_errno(c);
        c->eax = (uint32_t)result;
    } else if (!strcmp(name, "ungetwc")) {
        c->eax = (uint32_t)ungetwc((wint_t)arg(c, 0), rt_stream_file(c, arg(c, 1)));
    } else if (!strcmp(name, "putwc")) {
        FILE *file = rt_stream_file(c, arg(c, 1));
        wint_t result = putwc((wchar_t)arg(c, 0), file);
        if (result == WEOF) rt_save_errno(c);
        c->eax = (uint32_t)result;
    } else if (!strcmp(name, "putchar")) {
        int result = fputc((int32_t)arg(c, 0), rt_stream_file(c, rt_standard_stream(1)));
        if (result == EOF) rt_save_errno(c);
        c->eax = (uint32_t)result;
    } else return 0;
    return 1;
}
RT_SHIM_PROVIDER(posix_shim)
