// Fichiers et répertoires POSIX : vue fusionnée jeu / dépôt Linux / données
// utilisateur, suppressions logiques, structures glibc i386 et rappels invités.
#include "checks.h"
#include <ctype.h>
#include "posix.h"
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define STACK 0x20010000u
#define STR 0x20011000u
static uint32_t strings_top;
static uint32_t gstr(const char *value) {
    size_t size = strlen(value) + 1;
    uint32_t at = strings_top; strings_top += (uint32_t)((size + 15) & ~(size_t)15);
    assert(strings_top < 0x2001f000u);
    memcpy(GPTR(at), value, size); return at;
}
static CPU cpu;
static uint32_t call(const char *name, unsigned count, ...) {
    rt_cpu_reset(&cpu); cpu.esp = STACK;
    va_list list; va_start(list, count);
    for (unsigned i = 0; i < count; ++i) W32(STACK + 4 + 4 * i, va_arg(list, uint32_t));
    va_end(list);
    assert(rt_shim(&cpu, name));
    return cpu.eax;
}
static uint32_t guest_errno(void) { CPU c; rt_cpu_reset(&c); c.esp = STACK; assert(rt_shim(&c, "__errno_location")); return R32(c.eax); }
static char root[PATH_MAX], game[PATH_MAX], depot[PATH_MAX], user[PATH_MAX];
static void put(const char *base, const char *relative, const char *contents) {
    char path[PATH_MAX]; snprintf(path, sizeof(path), "%s/%s", base, relative);
    FILE *file = fopen(path, "wb"); assert(file); fputs(contents, file); assert(!fclose(file));
}
static void slurp(const char *path, char *out, size_t size) {
    FILE *file = fopen(path, "rb"); assert(file);
    size_t n = fread(out, 1, size - 1, file); out[n] = 0; fclose(file);
}
static const char *game_path(const char *relative) {
    static char buffers[8][PATH_MAX]; static unsigned next;
    char *out = buffers[next++ % 8]; snprintf(out, PATH_MAX, "%s/%s", game, relative); return out;
}
// Liste d'un répertoire invité par opendir/readdir, triée et jointe par « , ».
static void listing(const char *relative, int wide, char *out, size_t size) {
    uint32_t dir = call("opendir", 1, gstr(game_path(relative)));
    assert(dir);
    char *names[64]; size_t count = 0, index = 0;
    for (;;) {
        uint32_t entry = call(wide ? "readdir64" : "readdir", 1, dir);
        if (!entry) break;
        unsigned header = wide ? 19 : 11;
        uint16_t reclen = R16(entry + header - 3);
        assert(reclen % (wide ? 8 : 4) == 0 && reclen >= header + strlen(GPTR(entry + header)) + 1);
        ++index;
        assert(wide ? R64(entry + 8) == index : R32(entry + 4) == index);
        const char *name = GPTR(entry + header);
        if (!strcmp(name, ".") || !strcmp(name, "..")) { assert(R8(entry + header - 1) == 4); continue; }
        assert(count < 64); names[count++] = strdup(name);
    }
    assert(call("closedir", 1, dir) == 0);
    for (size_t i = 0; i < count; ++i) for (size_t j = i + 1; j < count; ++j)
        if (strcmp(names[i], names[j]) > 0) { char *t = names[i]; names[i] = names[j]; names[j] = t; }
    out[0] = 0;
    for (size_t i = 0; i < count; ++i) { if (i) strlcat(out, ",", size); strlcat(out, names[i], size); free(names[i]); }
}

static int filter_calls;
static int test_guest_functions(CPU *c, const char *name) {
    if (!strcmp(name, "test_skip_dots")) {  // « fonction invitée » filtre de scandir
        uint32_t entry = R32(c->esp + 4); ++filter_calls;
        c->eax = R8(entry + 11) != '.';
    } else if (!strcmp(name, "test_skip_dots64")) {
        uint32_t entry = R32(c->esp + 4); ++filter_calls;
        c->eax = R8(entry + 19) != '.';
    } else return 0;
    return 1;
}
RT_SHIM_PROVIDER(test_guest_functions)

static void setup(void) {
    strings_top = STR;
    snprintf(root, sizeof(root), "/tmp/portal2-posix-XXXXXX"); assert(mkdtemp(root));
    snprintf(game, sizeof(game), "%s/game", root); assert(!mkdir(game, 0755));
    snprintf(depot, sizeof(depot), "%s/linux", root); assert(!mkdir(depot, 0755));
    snprintf(user, sizeof(user), "%s/user", root);
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/dir", game); assert(!mkdir(path, 0755));
    snprintf(path, sizeof(path), "%s/dir", depot); assert(!mkdir(path, 0755));
    put(game, "a.txt", "game-a"); put(game, "dir/x", "game-x"); put(game, "dir/y", "game-y");
    put(depot, "dir/y", "linux-y"); put(depot, "dir/z", "linux-z");
    assert(!rt_paths_init(game, depot, user));
    // Le préfixe /tmp devient /private/tmp : travailler sur les chemins canoniques.
    assert(realpath(game, path)); strcpy(game, path);
    assert(realpath(user, path)); strcpy(user, path);
}
static void cleanup(void) {
    char command[PATH_MAX + 16]; snprintf(command, sizeof(command), "rm -rf '%s'", root);
    assert(!system(command));
}

RT_CHECK(posix_errno_table_is_linux) {
    assert(rt_linux_errno(EAGAIN) == 11 && rt_linux_errno(EDEADLK) == 35);
    assert(rt_linux_errno(EINPROGRESS) == 115 && rt_linux_errno(ENOTEMPTY) == 39);
    assert(rt_linux_errno(EADDRINUSE) == 98 && rt_linux_errno(ECONNRESET) == 104);
    assert(rt_linux_errno(ENOENT) == 2 && rt_linux_errno(EXDEV) == 18);
    assert(rt_darwin_errno(115) == EINPROGRESS && rt_darwin_errno(11) == EAGAIN && rt_darwin_errno(4095) == 0);
}

RT_CHECK(posix_directory_listing_merges_sources) {
    setup();
    char names[512];
    listing("dir", 0, names, sizeof(names)); assert(!strcmp(names, "x,y,z"));
    // Une copie utilisateur masque l'original sans doublon.
    uint32_t mode = gstr("wb");
    uint32_t file = call("fopen", 2, gstr(game_path("dir/x")), mode); assert(file);
    assert(call("fputs", 2, gstr("user-x"), file) != UINT32_MAX && call("fclose", 1, file) == 0);
    listing("dir", 1, names, sizeof(names)); assert(!strcmp(names, "x,y,z"));
    char contents[64]; slurp(game_path("dir/x"), contents, sizeof(contents)); assert(!strcmp(contents, "game-x"));
    // Racine du jeu : le répertoire interne des suppressions reste invisible.
    assert(call("unlink", 1, gstr(game_path("dir/z"))) == 0);
    listing("", 0, names, sizeof(names)); assert(!strcmp(names, "a.txt,dir"));
    // Erreurs Linux : ENOENT, ENOTDIR.
    assert(call("opendir", 1, gstr(game_path("missing"))) == 0 && guest_errno() == 2);
    assert(call("opendir", 1, gstr(game_path("a.txt"))) == 0 && guest_errno() == 20);
    cleanup();
}

RT_CHECK(posix_unlink_rename_keep_original_copy) {
    setup();
    char names[512], contents[64], path[PATH_MAX];
    // Fichier présent seulement dans l'original : supprimé de la vue, intact sur disque.
    assert(call("unlink", 1, gstr(game_path("dir/y"))) == 0);
    listing("dir", 0, names, sizeof(names)); assert(!strcmp(names, "x,z"));
    assert(call("__xstat", 3, 3, gstr(game_path("dir/y")), 0x2001f000u) == UINT32_MAX && guest_errno() == 2);
    assert(call("unlink", 1, gstr(game_path("dir/y"))) == UINT32_MAX && guest_errno() == 2);
    slurp(game_path("dir/y"), contents, sizeof(contents)); assert(!strcmp(contents, "game-y"));
    snprintf(path, sizeof(path), "%s/dir/y", depot); slurp(path, contents, sizeof(contents)); assert(!strcmp(contents, "linux-y"));
    // Recréer le fichier le rend de nouveau visible avec le nouveau contenu.
    uint32_t file = call("fopen", 2, gstr(game_path("dir/y")), gstr("w")); assert(file);
    assert(call("fputs", 2, gstr("new-y"), file) != UINT32_MAX && call("fclose", 1, file) == 0);
    listing("dir", 0, names, sizeof(names)); assert(!strcmp(names, "x,y,z"));
    // rename d'un original : copie dans les données utilisateur, source masquée.
    assert(call("rename", 2, gstr(game_path("a.txt")), gstr(game_path("dir/b.txt"))) == 0);
    listing("", 0, names, sizeof(names)); assert(!strcmp(names, "dir"));
    snprintf(path, sizeof(path), "%s/dir/b.txt", user); slurp(path, contents, sizeof(contents)); assert(!strcmp(contents, "game-a"));
    slurp(game_path("a.txt"), contents, sizeof(contents)); assert(!strcmp(contents, "game-a"));
    // rename d'une copie utilisateur sur un fichier original existant.
    assert(call("rename", 2, gstr(game_path("dir/b.txt")), gstr(game_path("dir/x"))) == 0);
    listing("dir", 0, names, sizeof(names)); assert(!strcmp(names, "x,y,z"));
    snprintf(path, sizeof(path), "%s/dir/x", user); slurp(path, contents, sizeof(contents)); assert(!strcmp(contents, "game-a"));
    // Répertoire original : EXDEV comme overlayfs ; source absente : ENOENT.
    assert(call("rename", 2, gstr(game_path("dir")), gstr(game_path("dir2"))) == UINT32_MAX && guest_errno() == 18);
    assert(call("rename", 2, gstr(game_path("nope")), gstr(game_path("dir2"))) == UINT32_MAX && guest_errno() == 2);
    // remove sur un répertoire non vide : ENOTEMPTY (39).
    assert(call("remove", 1, gstr(game_path("dir"))) == UINT32_MAX && guest_errno() == 39);
    cleanup();
}

RT_CHECK(posix_mkdir_rmdir_follow_merged_view) {
    setup();
    char names[512], path[PATH_MAX]; struct stat info;
    assert(call("mkdir", 2, gstr(game_path("dir")), 0755) == UINT32_MAX && guest_errno() == 17);
    assert(call("mkdir", 2, gstr(game_path("none/sub")), 0755) == UINT32_MAX && guest_errno() == 2);
    snprintf(path, sizeof(path), "%s/none", user); assert(lstat(path, &info));
    assert(call("mkdir", 2, gstr(game_path("dir/new")), 0700) == 0);
    snprintf(path, sizeof(path), "%s/dir/new", user); assert(!stat(path, &info) && S_ISDIR(info.st_mode));
    snprintf(path, sizeof(path), "%s/dir/new", game); assert(lstat(path, &info));
    listing("dir", 0, names, sizeof(names)); assert(!strcmp(names, "new,x,y,z"));
    assert(call("rmdir", 1, gstr(game_path("dir"))) == UINT32_MAX && guest_errno() == 39);
    assert(call("rmdir", 1, gstr(game_path("dir/x"))) == UINT32_MAX && guest_errno() == 20);
    assert(call("unlink", 1, gstr(game_path("dir/new"))) == UINT32_MAX && guest_errno() == 21);
    assert(call("rmdir", 1, gstr(game_path("dir/new"))) == 0);
    // Vider puis supprimer un répertoire original.
    assert(call("unlink", 1, gstr(game_path("dir/x"))) == 0 && call("unlink", 1, gstr(game_path("dir/y"))) == 0);
    assert(call("unlink", 1, gstr(game_path("dir/z"))) == 0);
    assert(call("rmdir", 1, gstr(game_path("dir"))) == 0);
    assert(call("opendir", 1, gstr(game_path("dir"))) == 0 && guest_errno() == 2);
    snprintf(path, sizeof(path), "%s/dir/x", game); assert(!stat(path, &info));
    // mkdir le recrée vide : les anciens fichiers restent supprimés.
    assert(call("mkdir", 2, gstr(game_path("dir")), 0755) == 0);
    listing("dir", 0, names, sizeof(names)); assert(!strcmp(names, ""));
    cleanup();
}

RT_CHECK(posix_scandir_calls_guest_filter_and_alphasort) {
    setup();
    put(game, "dir/b", "1"); put(depot, "dir/a", "2");
    uint32_t list = 0x2001f800u;
    for (int wide = 0; wide < 2; ++wide) {
        filter_calls = 0;
        uint32_t filter = rt_dynamic_import(wide ? "test_skip_dots64" : "test_skip_dots");
        uint32_t sort = rt_dynamic_import(wide ? "alphasort64" : "alphasort");
        uint32_t count = call(wide ? "scandir64" : "scandir", 4, gstr(game_path("dir")), list, filter, sort);
        assert(count == 5 && filter_calls == 7);
        uint32_t array = R32(list); const char *expected[] = {"a", "b", "x", "y", "z"};
        unsigned header = wide ? 19 : 11;
        for (unsigned i = 0; i < 5; ++i) {
            uint32_t entry = R32(array + 4 * i);
            assert(!strcmp(GPTR(entry + header), expected[i]) && R8(entry + header - 1) == 8);
            assert(R16(entry + header - 3) == (wide ? 24 : 16));
            call("free", 1, entry);
        }
        call("free", 1, array);
    }
    // Sans filtre ni tri : toutes les entrées, y compris « . » et « .. ».
    assert(call("scandir", 4, gstr(game_path("dir")), list, 0, 0) == 7);
    uint32_t count = call("scandir", 4, gstr(game_path("missing")), list, 0, 0);
    assert(count == UINT32_MAX && guest_errno() == 2);
    cleanup();
}

RT_CHECK(posix_copy_up_metadata_and_realpath) {
    setup();
    char path[PATH_MAX]; struct stat info;
    assert(call("chmod", 2, gstr(game_path("dir/z")), 0600) == 0);
    snprintf(path, sizeof(path), "%s/dir/z", user); assert(!stat(path, &info) && (info.st_mode & 0777) == 0600);
    snprintf(path, sizeof(path), "%s/dir/z", depot); assert(!stat(path, &info) && (info.st_mode & 0777) != 0600);
    assert(call("chmod", 2, gstr(game_path("dir/missing")), 0600) == UINT32_MAX && guest_errno() == 2);
    uint32_t times = 0x2001f000u; W32(times, 1000); W32(times + 4, 0); W32(times + 8, 2000); W32(times + 12, 0);
    assert(call("utimes", 2, gstr(game_path("a.txt")), times) == 0);
    snprintf(path, sizeof(path), "%s/a.txt", user); assert(!stat(path, &info) && info.st_mtime == 2000);
    snprintf(path, sizeof(path), "%s/a.txt", game); assert(!stat(path, &info) && info.st_mtime != 2000);
    // realpath donne le chemin vu par l'invité, jamais celui des données utilisateur.
    uint32_t out = call("realpath", 2, gstr(game_path("dir/../dir/./z")), 0);
    snprintf(path, sizeof(path), "%s/dir/z", game); assert(out && !strcmp(GPTR(out), path));
    assert(call("realpath", 2, gstr(game_path("dir/nothing")), 0x2001f400u) == 0 && guest_errno() == 2);
    assert(call("symlink", 2, gstr("z"), gstr(game_path("dir/z"))) == UINT32_MAX && guest_errno() == 17);
    assert(call("symlink", 2, gstr("z"), gstr(game_path("dir/link"))) == 0);
    snprintf(path, sizeof(path), "%s/dir/link", user); assert(!lstat(path, &info) && S_ISLNK(info.st_mode));
    assert(call("link", 2, gstr(game_path("dir/x")), gstr(game_path("dir/hard"))) == 0);
    snprintf(path, sizeof(path), "%s/dir/hard", user); assert(!stat(path, &info) && info.st_nlink == 2);
    cleanup();
}

RT_CHECK(posix_temporary_files_and_fifos) {
    setup();
    char path[PATH_MAX], names[512]; struct stat info;
    uint32_t template = gstr(game_path("dir/tmpXXXXXX"));
    uint32_t fd = call("mkstemp", 1, template);
    assert((int32_t)fd >= 0 && memcmp(GPTR(template), game_path("dir/tmp"), strlen(game_path("dir/tmp"))) == 0);
    assert(!strstr(GPTR(template), "XXXXXX"));
    snprintf(path, sizeof(path), "%s/dir/%s", user, strrchr(GPTR(template), '/') + 1);
    assert(!stat(path, &info) && (info.st_mode & 0777) == 0600);
    assert(lstat(GPTR(template), &info)); // rien d'écrit dans la copie du jeu
    close((int)fd);
    assert(call("mkstemp", 1, gstr("/tmp/bad")) == UINT32_MAX && guest_errno() == 22);
    uint32_t name = call("tmpnam", 1, 0x2001f400u);
    assert(name == 0x2001f400u && strlen(GPTR(name)) == 15 && !strncmp(GPTR(name), "/tmp/file", 9));
    assert(call("mkfifo", 2, gstr(game_path("dir/fifo")), 0600) == 0);
    snprintf(path, sizeof(path), "%s/dir/fifo", user); assert(!lstat(path, &info) && S_ISFIFO(info.st_mode));
    uint32_t dev = 0x2001f600u; W64(dev, 0);
    assert(call("__xmknod", 4, 1, gstr(game_path("dir/node")), 0100644, dev) == 0);
    assert(call("__xmknod", 4, 1, gstr(game_path("dir/node")), 0100644, dev) == UINT32_MAX && guest_errno() == 17);
    assert(call("__xmknod", 4, 0, gstr(game_path("dir/n2")), 0100644, dev) == UINT32_MAX && guest_errno() == 22);
    if (geteuid()) assert(call("__xmknod", 4, 1, gstr(game_path("dir/dev")), 0020644, dev) == UINT32_MAX && guest_errno() == 1);
    listing("dir", 0, names, sizeof(names));
    assert(strstr(names, "fifo") && strstr(names, "node") && strstr(names, "tmp"));
    uint32_t creat_fd = call("creat", 2, gstr(game_path("a.txt")), 0644);
    assert((int32_t)creat_fd >= 0); close((int)creat_fd);
    snprintf(path, sizeof(path), "%s/a.txt", user); assert(!stat(path, &info) && info.st_size == 0);
    snprintf(path, sizeof(path), "%s/a.txt", game); assert(!stat(path, &info) && info.st_size == 6);
    cleanup();
}

RT_CHECK(posix_descriptors_vectors_and_ioctl) {
    uint32_t fds = 0x2001f000u;
    assert(call("pipe", 1, fds) == 0);
    int in = (int)R32(fds), out = (int)R32(fds + 4);
    uint32_t iov = 0x2001f010u;
    W32(iov, gstr("hello ")); W32(iov + 4, 6); W32(iov + 8, gstr("world")); W32(iov + 12, 5);
    assert(call("writev", 3, (uint32_t)out, iov, 2) == 11);
    uint32_t available = 0x2001f020u;
    assert(call("ioctl", 3, (uint32_t)in, 0x541bu, available) == 0 && R32(available) == 11);
    W32(available, 1);
    assert(call("ioctl", 3, (uint32_t)in, 0x5421u, available) == 0 && (fcntl(in, F_GETFL) & O_NONBLOCK));
    assert(call("ioctl", 3, (uint32_t)in, 0x5401u, 0x2001f100u) == UINT32_MAX && guest_errno() == 25);
    uint32_t copy = call("dup", 1, (uint32_t)in);
    assert((int32_t)copy > 2);
    char buffer[16] = {0}; assert(read((int)copy, buffer, sizeof(buffer)) == 11 && !strcmp(buffer, "hello world"));
    assert(call("dup2", 2, (uint32_t)out, copy) == copy);
    assert(call("writev", 3, (uint32_t)copy, iov, 1) == 6);
    // fdopen + ungetc + flux invités.
    uint32_t stream = call("fdopen", 2, (uint32_t)in, gstr("r")); assert(stream);
    assert(call("fgetc", 1, stream) == 'h' && call("ungetc", 2, 'H', stream) == 'H' && call("fgetc", 1, stream) == 'H');
    assert(call("fclose", 1, stream) == 0);
    close(out); close((int)copy);
    assert(call("dup", 1, 999) == UINT32_MAX && guest_errno() == 9);
    // flock et ftruncate sur un fichier temporaire.
    char path[] = "/tmp/portal2-flock-XXXXXX"; int fd = mkstemp(path); assert(fd >= 0);
    assert(call("flock", 2, (uint32_t)fd, 2 | 4) == 0);
    int other = open(path, O_RDONLY); assert(other >= 0);
    assert(call("flock", 2, (uint32_t)other, 2 | 4) == UINT32_MAX && guest_errno() == 11);
    assert(call("flock", 2, (uint32_t)fd, 8) == 0);
    assert(call("ftruncate", 2, (uint32_t)fd, 1234) == 0);
    struct stat info; assert(!fstat(fd, &info) && info.st_size == 1234);
    close(other); close(fd); unlink(path);
}

RT_CHECK(posix_statfs_layouts_are_i386) {
    uint32_t buffer = 0x2001e000u, path = gstr("/");
    memset(GPTR(buffer), 0xa5, 128);
    assert(call("statfs64", 2, path, buffer) == 0);
    assert(R32(buffer + 4) >= 512 && R64(buffer + 8) >= R64(buffer + 16) && R64(buffer + 16) >= R64(buffer + 24));
    assert(R32(buffer + 56) == 255 && R32(buffer + 60) == R32(buffer + 4) && (R32(buffer + 64) & 0x20));
    assert(R32(buffer + 68) == 0 && R32(buffer + 80) == 0 && R8(buffer + 84) == 0xa5); // 84 octets
    memset(GPTR(buffer), 0xa5, 128);
    assert(call("statvfs64", 2, path, buffer) == 0);
    assert(R32(buffer) >= 512 && R32(buffer + 4) == R32(buffer) && R64(buffer + 8) >= R64(buffer + 16));
    assert(R64(buffer + 48) == R64(buffer + 40) && R32(buffer + 68) == 255 && R8(buffer + 96) == 0xa5);
    memset(GPTR(buffer), 0xa5, 128);
    uint32_t result = call("statvfs", 2, path, buffer);
    if (result == 0) assert(R32(buffer + 44) == 255 && R8(buffer + 72) == 0xa5);
    else assert(guest_errno() == 75); // compteurs > 32 bits : EOVERFLOW comme le noyau i386
    result = call("statfs", 2, gstr("/nonexistent-portal2"), buffer);
    assert(result == UINT32_MAX && guest_errno() == 2);
    assert(call("pathconf", 2, path, 3) == 255);
    assert(call("pathconf", 2, path, 99) == UINT32_MAX && guest_errno() == 22);
    assert(call("faccessat", 4, (uint32_t)-100, path, 4, 0) == 0);
    assert(call("faccessat", 4, (uint32_t)-100, gstr("/nonexistent-portal2"), 4, 0) == UINT32_MAX && guest_errno() == 2);
}

// ---- Processus et environnement ----
static int exit_pipe = -1;
static int test_exit_handlers(CPU *c, const char *name) {
    if (strncmp(name, "test_exit_handler", 17)) return 0;
    char tag = (char)R32(c->esp + 4);
    assert(write(exit_pipe, &tag, 1) == 1);
    if (tag == '2') {  // un handler qui en enregistre un autre, exécuté juste après
        uint32_t saved = c->esp;
        c->esp -= 16; W32(c->esp + 4, rt_dynamic_import("test_exit_handler3")); W32(c->esp + 8, '3'); W32(c->esp + 12, 0);
        assert(rt_shim(c, "__cxa_atexit") && c->eax == 0);
        c->esp = saved;
    }
    c->eax = 0; return 1;
}
RT_SHIM_PROVIDER(test_exit_handlers)

RT_CHECK(process_environment_and_identity) {
    strings_top = STR;
    uint32_t environ_object = 0x2001fffcu;
    assert(!rt_init_import_data("environ", environ_object, 4));
    assert(call("setenv", 3, gstr("P2_POSIX_TEST"), gstr("one"), 1) == 0 && !strcmp(getenv("P2_POSIX_TEST"), "one"));
    assert(call("setenv", 3, gstr("P2_POSIX_TEST"), gstr("two"), 0) == 0 && !strcmp(getenv("P2_POSIX_TEST"), "one"));
    uint32_t value = call("getenv", 1, gstr("P2_POSIX_TEST")); assert(value && !strcmp(GPTR(value), "one"));
    int seen = 0;
    for (uint32_t p = R32(environ_object); R32(p); p += 4) seen |= !strcmp(GPTR(R32(p)), "P2_POSIX_TEST=one");
    assert(seen);
    assert(call("setenv", 3, gstr("BAD=NAME"), gstr("x"), 1) == UINT32_MAX && guest_errno() == 22);
    assert(call("setenv", 3, gstr(""), gstr("x"), 1) == UINT32_MAX && guest_errno() == 22);
    unsetenv("P2_POSIX_TEST");
    assert(call("getpid", 0) == (uint32_t)getpid() && call("getuid", 0) == (uint32_t)getuid());
    uint32_t pw = call("getpwuid", 1, (uint32_t)getuid());
    struct passwd *host = getpwuid(getuid());
    assert(pw && host && !strcmp(GPTR(R32(pw)), host->pw_name) && R32(pw + 8) == (uint32_t)getuid());
    assert(!strcmp(GPTR(R32(pw + 20)), host->pw_dir) && !strcmp(GPTR(R32(pw + 24)), host->pw_shell));
    uint32_t name = 0x2001f000u; char hostname[256]; assert(!gethostname(hostname, sizeof(hostname)));
    assert(call("gethostname", 2, name, 256) == 0 && !strcmp(GPTR(name), hostname));
    if (hostname[0]) assert(call("gethostname", 2, name, 1) == UINT32_MAX && guest_errno() == 36);
    uint32_t usage = 0x2001f100u; memset(GPTR(usage), 0xa5, 80);
    assert(call("getrusage", 2, 0, usage) == 0 && R32(usage + 16) > 0 && R32(usage + 16) < 64 * 1024 * 1024);
    assert(R32(usage + 4) < 1000000 && R8(usage + 72) == 0xa5);
    assert(call("getrusage", 2, 7, usage) == UINT32_MAX && guest_errno() == 22);
    assert(call("getloadavg", 2, 0x2001f200u, 3) == 3 && RD(0x2001f200u) >= 0);
    uint32_t uuid = 0x2001f300u;
    call("uuid_generate", 1, uuid);
    assert((R8(uuid + 6) >> 4) == 4 && (R8(uuid + 8) >> 6) == 2);
    assert(call("mount", 5, gstr("a"), gstr("b"), gstr("c"), 0, 0) == UINT32_MAX && guest_errno() == (geteuid() ? 1u : 0u));
    assert(call("alarm", 1, 0) == 0);
}

RT_CHECK(process_mallinfo_pops_hidden_pointer) {
    uint32_t keep = rt_alloc(4096), out = 0x2001f000u;
    rt_cpu_reset(&cpu); cpu.esp = STACK; W32(STACK, 0x1234); W32(STACK + 4, out);
    assert(rt_shim(&cpu, "mallinfo"));
    assert(cpu.eax == out && cpu.esp == STACK + 4 && R32(cpu.esp) == 0x1234); // ret $4 après le pop
    assert(R32(out) > 0 && R32(out + 28) >= 4096 && R32(out + 8) == 0);
    rt_free(keep);
}

RT_CHECK(process_children_and_linux_wait_status) {
    strings_top = STR;
    uint32_t status = 0x2001f000u;
    // fork réel + _exit dans le fils ; statut Linux.
    uint32_t pid = call("fork", 0);
    if (pid == 0) { CPU child; rt_cpu_reset(&child); child.esp = STACK; W32(STACK + 4, 42); rt_shim(&child, "_exit"); }
    assert((int32_t)pid > 0);
    assert(call("waitpid", 3, pid, status, 0) == pid && R32(status) == 42u << 8);
    // Signal : SIGUSR1 Linux (10) livré au fils et rapporté avec le numéro Linux.
    pid_t child = fork();
    if (!child) { for (;;) pause(); }
    assert(call("kill", 2, (uint32_t)child, 10) == 0);
    assert(call("wait", 1, status) == (uint32_t)child && (R32(status) & 0x7f) == 10);
    assert(call("kill", 2, (uint32_t)getpid(), 99) == UINT32_MAX && guest_errno() == 22);
    assert(call("waitpid", 3, (uint32_t)-1, status, 1) == UINT32_MAX && guest_errno() == 10); // ECHILD
    // execvp : échec réel dans le fils (ENOENT), rapporté par le code de sortie.
    pid = call("fork", 0);
    if (pid == 0) {
        uint32_t argv = 0x2001f100u; W32(argv, gstr("p2-no-such-program")); W32(argv + 4, 0);
        uint32_t result = call("execvp", 2, R32(argv), argv);
        _exit(result == UINT32_MAX ? (int)guest_errno() : 100);
    }
    assert(call("waitpid", 3, pid, status, 0) == pid && R32(status) == 2u << 8);
    // system et popen exécutent réellement la commande hôte.
    assert(call("system", 1, gstr("exit 5")) == 5u << 8 && call("system", 1, 0) != 0);
    uint32_t stream = call("popen", 2, gstr("echo hi; exit 3"), gstr("re")); assert(stream);
    uint32_t line = 0x2001f200u;
    assert(call("fgets", 3, line, 16, stream) == line && !strcmp(GPTR(line), "hi\n"));
    assert(call("pclose", 1, stream) == 3u << 8);
    assert(call("popen", 2, gstr("true"), gstr("rw")) == 0 && guest_errno() == 22);
    // exit : handlers invités du plus récent au plus ancien, puis sortie hôte.
    int fds[2]; assert(!pipe(fds));
    pid_t exiting = fork();
    if (!exiting) {
        close(fds[0]); exit_pipe = fds[1];
        // D'autres checks du même processus ont pu enregistrer des handlers.
        for (size_t i = rt_atexit_count(); i--;) rt_atexit_clear(i);
        call("__cxa_atexit", 3, rt_dynamic_import("test_exit_handler1"), '1', 0);
        call("__cxa_atexit", 3, rt_dynamic_import("test_exit_handler2"), '2', 0);
        call("exit", 1, 9);
        _exit(98);
    }
    close(fds[1]);
    char order[8] = {0}; ssize_t got = 0, n;
    while ((n = read(fds[0], order + got, sizeof(order) - 1 - (size_t)got)) > 0) got += n;
    close(fds[0]);
    int host_status; assert(waitpid(exiting, &host_status, 0) == exiting);
    assert(WIFEXITED(host_status) && WEXITSTATUS(host_status) == 9 && !strcmp(order, "231"));
    // Un handler déjà exécuté par __cxa_finalize (dlclose) n'est pas rappelé par exit.
    assert(!pipe(fds));
    exiting = fork();
    if (!exiting) {
        close(fds[0]); exit_pipe = fds[1];
        for (size_t i = rt_atexit_count(); i--;) rt_atexit_clear(i);
        call("__cxa_atexit", 3, rt_dynamic_import("test_exit_handler1"), '1', 0);
        call("__cxa_atexit", 3, rt_dynamic_import("test_exit_handler4"), '4', 0x1234);
        call("__cxa_finalize", 1, 0x1234);
        call("exit", 1, 7);
        _exit(98);
    }
    close(fds[1]);
    memset(order, 0, sizeof(order)); got = 0;
    while ((n = read(fds[0], order + got, sizeof(order) - 1 - (size_t)got)) > 0) got += n;
    close(fds[0]);
    assert(waitpid(exiting, &host_status, 0) == exiting);
    assert(WIFEXITED(host_status) && WEXITSTATUS(host_status) == 7 && !strcmp(order, "41"));
}

RT_CHECK(posix_guest_symlinks_never_reach_original_copy) {
    setup();
    char path[PATH_MAX], contents[64]; struct stat info;
    // Lien absolu vers un fichier original, puis écriture à travers le lien.
    assert(call("symlink", 2, gstr(game_path("a.txt")), gstr(game_path("dir/abs"))) == 0);
    uint32_t file = call("fopen", 2, gstr(game_path("dir/abs")), gstr("w")); assert(file);
    assert(call("fputs", 2, gstr("through-link"), file) != UINT32_MAX && call("fclose", 1, file) == 0);
    slurp(game_path("a.txt"), contents, sizeof(contents)); assert(!strcmp(contents, "game-a"));
    snprintf(path, sizeof(path), "%s/a.txt", user); slurp(path, contents, sizeof(contents)); assert(!strcmp(contents, "through-link"));
    // Lien relatif vers un répertoire original utilisé comme composant.
    assert(call("symlink", 2, gstr("dir"), gstr(game_path("alias"))) == 0);
    assert(call("chmod", 2, gstr(game_path("alias/x")), 0600) == 0);
    snprintf(path, sizeof(path), "%s/dir/x", game); assert(!stat(path, &info) && (info.st_mode & 0777) != 0600);
    snprintf(path, sizeof(path), "%s/dir/x", user); assert(!stat(path, &info) && (info.st_mode & 0777) == 0600);
    // Boucle de liens : ELOOP Linux (40).
    assert(call("symlink", 2, gstr("loop2"), gstr(game_path("loop1"))) == 0);
    assert(call("symlink", 2, gstr("loop1"), gstr(game_path("loop2"))) == 0);
    assert(call("fopen", 2, gstr(game_path("loop1")), gstr("w")) == 0 && guest_errno() == 40);
    // rename sur un lien existant remplace le lien, pas sa cible.
    assert(call("rename", 2, gstr(game_path("dir/y")), gstr(game_path("dir/abs"))) == 0);
    snprintf(path, sizeof(path), "%s/dir/abs", user); assert(!lstat(path, &info) && S_ISREG(info.st_mode));
    snprintf(path, sizeof(path), "%s/a.txt", user); slurp(path, contents, sizeof(contents)); assert(!strcmp(contents, "through-link"));
    // Les marques de suppression ne sont pas accessibles à l'invité.
    assert(call("unlink", 1, gstr(game_path("dir/z"))) == 0);
    assert(call("unlink", 1, gstr(game_path(RT_WHITEOUTS "/dir/z/.wh"))) == UINT32_MAX && guest_errno() == 2);
    assert(call("opendir", 1, gstr(game_path(RT_WHITEOUTS))) == 0 && guest_errno() == 2);
    assert(call("fopen", 2, gstr(game_path(RT_WHITEOUTS "/x")), gstr("w")) == 0 && guest_errno() == 2);
    // Socket AF_UNIX nommée sous la racine du jeu : créée dans les données utilisateur.
    int32_t fd = (int32_t)call("socket", 3, 1, 1, 0); assert(fd >= 0);
    uint32_t address = 0x2001e000u; memset(GPTR(address), 0, 110); W16(address, 1);
    strcpy(GPTR(address + 2), "dir/sock"); // relatif : passer par le répertoire courant
    char cwd[PATH_MAX]; assert(getcwd(cwd, sizeof(cwd)) && !chdir(game));
    assert(call("bind", 3, (uint32_t)fd, address, 110) == 0);
    assert(!chdir(cwd));
    snprintf(path, sizeof(path), "%s/dir/sock", user); assert(!lstat(path, &info) && S_ISSOCK(info.st_mode));
    snprintf(path, sizeof(path), "%s/dir/sock", game); assert(lstat(path, &info));
    close(fd);
    assert(call("unlink", 1, gstr(game_path("alias"))) == 0); // le lien, pas le répertoire
    assert(call("opendir", 1, gstr(game_path("alias"))) == 0 && guest_errno() == 2);
    assert(call("rmdir", 1, gstr(game_path("loop1"))) == UINT32_MAX && guest_errno() == 20);
    cleanup();
}

RT_CHECK(posix_windows_binaries_of_game_copy_are_invisible) {
    setup();
    put(game, "dir/mssmp3.asi", "PE");
    put(game, "dir/Engine.DLL", "PE");
    put(depot, "dir/tool.exe", "linux-depot-file");   // une copie fournie par le dépôt Linux reste visible
    { char bin[PATH_MAX]; snprintf(bin, sizeof(bin), "%s/dir/bin", game); mkdir(bin, 0755); }
    put(game, "dir/bin/client.dll", "PE");               // lu pour CL_CheckCRCs (coop en réseau)
    put(game, "dir/client.dll", "PE");                   // hors de bin/ : caché
    assert(rt_read_path(game_path("dir/bin/client.dll")));
    assert(!rt_read_path(game_path("dir/client.dll")));
    errno = 0;
    assert(!rt_read_path(game_path("dir/mssmp3.asi")) && errno == ENOENT);
    assert(!rt_read_path(game_path("dir/Engine.DLL")));
    assert(rt_read_path(game_path("dir/tool.exe")));
    assert(rt_read_path(game_path("dir/x")));         // données ordinaires inchangées
    RtDirEntry *entries; size_t count;
    assert(!rt_dir_entries(game_path("dir"), &entries, &count));
    int asi = 0, dll = 0, exe = 0, x = 0;
    for (size_t i = 0; i < count; ++i) {
        asi |= !strcmp(entries[i].name, "mssmp3.asi"); dll |= !strcmp(entries[i].name, "Engine.DLL");
        exe |= !strcmp(entries[i].name, "tool.exe"); x |= !strcmp(entries[i].name, "x");
    }
    rt_dir_entries_free(entries, count);
    assert(!asi && !dll && exe && x);
}

extern const uint8_t testpe_file[];
extern const uint32_t testpe_file_size;
static void put_bytes(const char *relative, const uint8_t *data, size_t size) {
    char path[PATH_MAX]; snprintf(path, sizeof(path), "%s/%s", game, relative);
    unlink(path);   // nouveau fichier : nouvelle identité pour le cache de pe.c
    FILE *file = fopen(path, "wb"); assert(file);
    assert(fwrite(data, 1, size, file) == size && !fclose(file));
}

// Seuls les greffons Miles traduits (build/pe-fixture.c enregistre testpe.asi),
// à leur place bin/linux32 et identiques au fichier traduit, sont visibles.
RT_CHECK(posix_translated_miles_plugins_are_visible) {
    setup();
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/bin", game); assert(!mkdir(path, 0755));
    snprintf(path, sizeof(path), "%s/bin/linux32", game); assert(!mkdir(path, 0755));
    put_bytes("bin/linux32/testpe.asi", testpe_file, testpe_file_size);
    put_bytes("bin/testpe.asi", testpe_file, testpe_file_size);          // dossier Windows de Miles
    put_bytes("bin/linux32/mssmp3.asi", testpe_file, testpe_file_size);  // nom non traduit ici
    put(game, "bin/linux32/engine.dll", "PE");
    assert(rt_read_path(game_path("bin/linux32/testpe.asi")));
    assert(rt_read_path(game_path("BIN/Linux32/TESTPE.ASI")));          // volume insensible à la casse
    errno = 0;
    assert(!rt_read_path(game_path("bin/testpe.asi")) && errno == ENOENT);
    assert(!rt_read_path(game_path("bin/linux32/mssmp3.asi")));
    assert(!rt_read_path(game_path("bin/linux32/engine.dll")));
    char names[256];
    listing("bin/linux32", 0, names, sizeof(names));
    assert(!strcmp(names, "testpe.asi"));
    listing("bin", 0, names, sizeof(names));
    assert(!strcmp(names, "linux32"));
    // Une autre version du greffon (un octet changé) reste invisible. Nouvelle
    // racine : les listes de répertoires sont mémorisées entre écritures invitées.
    cleanup();
    setup();
    snprintf(path, sizeof(path), "%s/bin", game); assert(!mkdir(path, 0755));
    snprintf(path, sizeof(path), "%s/bin/linux32", game); assert(!mkdir(path, 0755));
    uint8_t *copy = malloc(testpe_file_size);
    memcpy(copy, testpe_file, testpe_file_size); copy[0x400] ^= 1;
    put_bytes("bin/linux32/testpe.asi", copy, testpe_file_size);
    free(copy);
    assert(!rt_read_path(game_path("bin/linux32/testpe.asi")));
    listing("bin/linux32", 0, names, sizeof(names));
    assert(!strcmp(names, ""));    cleanup();
}

RT_CHECK(posix_case_variants_of_root_and_listing_cache) {
    setup();
    if (pathconf(game, _PC_CASE_SENSITIVE) == 0) {
        // Variante de casse de la racine (correcteur de casse de Source) : même fichier.
        char variant[PATH_MAX]; snprintf(variant, sizeof(variant), "%s/a.txt", game);
        for (char *p = variant; *p; ++p) *p = (char)tolower((unsigned char)*p);
        char relative[PATH_MAX];
        assert(rt_path_relative(variant, relative) == 1 && !strcmp(relative, "a.txt"));
    }
    char names[256];
    listing("dir", 0, names, sizeof(names)); assert(!strcmp(names, "x,y,z"));
    // Création par le jeu : la liste mise en cache est invalidée.
    uint32_t file = call("fopen", 2, gstr(game_path("dir/w")), gstr("w")); assert(file && call("fclose", 1, file) == 0);
    listing("dir", 0, names, sizeof(names)); assert(!strcmp(names, "w,x,y,z"));
    cleanup();
}

RT_CHECK(posix_read_cache_sees_guest_creations) {
    setup();
    char host[PATH_MAX]; snprintf(host, sizeof(host), "%s/later.cfg", user);
    const char *before = rt_read_path(game_path("later.cfg"));
    assert(before && access(before, F_OK));                        // absent (résolution mise en cache)
    assert(rt_read_path(game_path("later.cfg")));                  // deuxième sondage servi par le cache
    uint32_t file = call("fopen", 2, gstr(game_path("later.cfg")), gstr("w"));
    assert(file && call("fclose", 1, file) == 0);
    const char *after = rt_read_path(game_path("later.cfg"));
    char resolved[PATH_MAX];
    assert(after && !access(after, F_OK) && realpath(after, resolved) && !strcmp(resolved, host)); // créé par le jeu : visible aussitôt
    cleanup();
}

RT_CHECK(posix_access_cache_follows_guest_writes) {
    setup();
    assert(call("access", 2, gstr(game_path("a.txt")), 0) == 0);
    char parent[PATH_MAX]; snprintf(parent, sizeof(parent), "%s", game); *strrchr(parent, '/') = 0;
    assert(rt_access_cached(parent, F_OK) == 0 && rt_access_cached("/", R_OK) == 0);   // parents de la racine
    assert(call("access", 2, gstr(game_path("absent.cfg")), 0) == UINT32_MAX && guest_errno() == 2);
    assert(call("access", 2, gstr(game_path("absent.cfg")), 4) == UINT32_MAX);   // servi par le cache
    assert(call("fopen", 2, gstr(game_path("absent.cfg")), gstr("rb")) == 0 && guest_errno() == 2);
    assert(call("open", 2, gstr(game_path("absent.cfg")), 0) == UINT32_MAX && guest_errno() == 2);
    uint32_t file = call("fopen", 2, gstr(game_path("absent.cfg")), gstr("w"));
    assert(file && call("fclose", 1, file) == 0);
    assert(call("access", 2, gstr(game_path("absent.cfg")), 0) == 0);           // créé : visible aussitôt
    assert(call("unlink", 1, gstr(game_path("a.txt"))) == 0);
    assert(call("access", 2, gstr(game_path("a.txt")), 0) == UINT32_MAX && guest_errno() == 2);
    cleanup();
}

RT_CHECK(posix_closed_streams_are_released) {
    setup();
    uint32_t first = call("fopen", 2, gstr(game_path("a.txt")), gstr("rb"));
    assert(first && call("fclose", 1, first) == 0);
    uint32_t path = gstr(game_path("a.txt")), mode = gstr("rb");
    for (int i = 0; i < 2000; ++i) {
        uint32_t file = call("fopen", 2, path, mode);
        assert(file && call("fclose", 1, file) == 0);
    }
    uint32_t dirpath = gstr(game_path("dir"));
    for (int i = 0; i < 70000; ++i) {                            // > 65 535 handles : réutilisés
        uint32_t dir = call("opendir", 1, dirpath);
        assert(dir && dir < 0xd1000000u && call("closedir", 1, dir) == 0);
    }
    assert(rt_standard_stream(0) == 0xd1000000u);                // stdin garde son handle propre
    // Un flux fermé (handle non réutilisé depuis) reste inconnu : l'utiliser piège.
    pid_t pid = fork();
    if (!pid) { freopen("/dev/null", "w", stderr); call("fgetc", 1, first); _exit(0); }
    int status; assert(waitpid(pid, &status, 0) == pid && WIFSIGNALED(status));
    cleanup();
}

RT_CHECK(posix_interleaved_stream_handles_stay_valid) {
    setup();
    uint32_t path = gstr(game_path("a.txt")), mode = gstr("rb"), dirpath = gstr(game_path("dir"));
    uint32_t open_files[64] = {0}, open_dirs[64] = {0};
    unsigned seed = 7;
    for (int i = 0; i < 4000; ++i) {
        seed = seed * 1103515245u + 12345u;
        unsigned k = (seed >> 8) & 63;
        if ((seed >> 20) & 1) {
            if (open_files[k]) { assert(call("fgetc", 1, open_files[k]) != 0xdeadbeefu); assert(call("fclose", 1, open_files[k]) == 0); open_files[k] = 0; }
            else assert((open_files[k] = call("fopen", 2, path, mode)));
        } else {
            if (open_dirs[k]) { call("readdir", 1, open_dirs[k]); assert(call("closedir", 1, open_dirs[k]) == 0); open_dirs[k] = 0; }
            else assert((open_dirs[k] = call("opendir", 1, dirpath)));
        }
    }
    for (int k = 0; k < 64; ++k) {
        if (open_files[k]) assert(call("fclose", 1, open_files[k]) == 0);
        if (open_dirs[k]) assert(call("closedir", 1, open_dirs[k]) == 0);
    }
    cleanup();
}
