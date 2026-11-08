// Processus et environnement avec la sémantique glibc i386 / Linux.
// Les processus fils sont de vrais processus hôtes : fork, execvp, popen et
// system exécutent réellement la commande (un exécutable ELF Linux échoue donc
// avec ENOEXEC, comme sur un Linux sans binfmt adapté). Les numéros de signaux
// et les statuts wait sont traduits entre Darwin et Linux.
// SHIMS: setenv getpid getuid getpwuid gethostname getrusage getloadavg kill wait waitpid
// SHIMS: geteuid getgid getegid getlogin_r
// SHIMS: fork execvp popen pclose system _exit exit alarm uuid_generate mallinfo mount
#include "posix.h"
#include "modules.h"
#include <TargetConditionals.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

static uint32_t arg(CPU *c, unsigned i) { return R32(c->esp + 4 + 4 * i); }
static const char *string(CPU *c, uint32_t address) { rt_string_length(c, address); return GPTR(address); }
static void finish(CPU *c, int result) { if (result < 0) rt_save_errno(c); c->eax = (uint32_t)result; }
static void fail(CPU *c, int linux_errno) { rt_set_errno(c, linux_errno); c->eax = UINT32_MAX; }

// Signaux Linux i386 (asm/signal.h) et leurs équivalents Darwin.
static const struct { int linux_value, darwin; } signal_table[] = {
    {1, SIGHUP}, {2, SIGINT}, {3, SIGQUIT}, {4, SIGILL}, {5, SIGTRAP}, {6, SIGABRT},
    {7, SIGBUS}, {8, SIGFPE}, {9, SIGKILL}, {10, SIGUSR1}, {11, SIGSEGV}, {12, SIGUSR2},
    {13, SIGPIPE}, {14, SIGALRM}, {15, SIGTERM}, {17, SIGCHLD}, {18, SIGCONT}, {19, SIGSTOP},
    {20, SIGTSTP}, {21, SIGTTIN}, {22, SIGTTOU}, {23, SIGURG}, {24, SIGXCPU}, {25, SIGXFSZ},
    {26, SIGVTALRM}, {27, SIGPROF}, {28, SIGWINCH}, {29, SIGIO}, {31, SIGSYS},
};
int rt_linux_signal(int darwin) {
    for (size_t i = 0; i < sizeof(signal_table) / sizeof(*signal_table); ++i)
        if (signal_table[i].darwin == darwin) return signal_table[i].linux_value;
    return 0;
}
int rt_darwin_signal(int linux_value) {
    for (size_t i = 0; i < sizeof(signal_table) / sizeof(*signal_table); ++i)
        if (signal_table[i].linux_value == linux_value) return signal_table[i].darwin;
    return 0;
}
uint32_t rt_linux_wait_status(int status) {
    if (WIFEXITED(status)) return (uint32_t)(WEXITSTATUS(status) & 0xff) << 8;
    if (WIFSIGNALED(status)) {
        int sig = rt_linux_signal(WTERMSIG(status));
        return (uint32_t)(sig ? sig : 0x7e) | (WCOREDUMP(status) ? 0x80u : 0);
    }
    if (WIFCONTINUED(status)) return 0xffff;
    if (WIFSTOPPED(status)) return (uint32_t)rt_linux_signal(WSTOPSIG(status)) << 8 | 0x7f;
    return (uint32_t)status;
}

// Les handlers __cxa_atexit invités s'exécutent du plus récent au plus ancien,
// y compris ceux enregistrés pendant l'exécution des autres (comme glibc).
// Chacun est effacé avant son appel : un handler déjà exécuté par
// __cxa_finalize (dlclose) n'est pas rappelé. Puis les destructeurs des
// modules (DT_FINI_ARRAY/DT_FINI), comme _dl_fini.
static void run_guest_atexit(CPU *c) {
    for (;;) {
        size_t i = rt_atexit_count();
        uint32_t function = 0, argument = 0, dso = 0;
        while (i--) {
            if (rt_atexit_get(i, &function, &argument, &dso)) rt_trap(c, c->eip, "atexit table changed");
            if (function) break;
        }
        if (i == (size_t)-1) break;
        rt_atexit_clear(i);
        rt_invoke(c, function, &argument, 1);
    }
    rt_finalize_modules(c);
}

// Copie une liste argv invitée terminée par NULL (au plus 65536 entrées).
static char **host_argv(CPU *c, uint32_t vector) {
    size_t count = 0;
    while (R32(vector + 4 * (uint32_t)count)) if (++count > 65536) return NULL;
    char **result = calloc(count + 1, sizeof(*result));
    if (!result) return NULL;
    for (size_t i = 0; i < count; ++i) result[i] = (char *)string(c, R32(vector + 4 * (uint32_t)i));
    return result;
}

static uint32_t passwd_buffer, passwd_size;
static int write_passwd(CPU *c, const struct passwd *pw) {
    const char *fields[] = {pw->pw_name, pw->pw_passwd, pw->pw_gecos, pw->pw_dir, pw->pw_shell};
    size_t total = 28;
    for (int i = 0; i < 5; ++i) total += strlen(fields[i] ? fields[i] : "") + 1;
    if (total > UINT32_MAX) { errno = ENOMEM; return -1; }
    if (total > passwd_size) {
        uint32_t grown = rt_realloc(passwd_buffer, (uint32_t)total);
        if (!grown) { errno = ENOMEM; return -1; }
        passwd_buffer = grown; passwd_size = (uint32_t)total;
    }
    // struct passwd i386 : name, passwd, uid, gid, gecos, dir, shell (28 octets).
    uint32_t base = passwd_buffer, text = base + 28, pointers[5];
    for (int i = 0; i < 5; ++i) {
        const char *value = fields[i] ? fields[i] : "";
        size_t length = strlen(value) + 1;
        memcpy(GPTR(text), value, length); pointers[i] = text; text += (uint32_t)length;
    }
    (void)c;
    W32(base, pointers[0]); W32(base + 4, pointers[1]); W32(base + 8, pw->pw_uid); W32(base + 12, pw->pw_gid);
    W32(base + 16, pointers[2]); W32(base + 20, pointers[3]); W32(base + 24, pointers[4]);
    return 0;
}

static int process_shim(CPU *c, const char *name) {
    if (!strcmp(name, "setenv")) {
        uint32_t key = arg(c, 0), value = arg(c, 1); int overwrite = (int32_t)arg(c, 2);
        if (!key) { fail(c, 22); return 1; }
        const char *k = string(c, key);
        if (!*k || strchr(k, '=')) { fail(c, 22); return 1; }
        const char *v = string(c, value);
        if (!overwrite && getenv(k)) { c->eax = 0; return 1; }
        // L'environnement hôte est partagé : les bibliothèques natives (SDL...)
        // voient les mêmes variables que le code invité.
        if (setenv(k, v, 1)) { finish(c, -1); return 1; }
        if (rt_environ_sync()) { fail(c, 12); return 1; }
        c->eax = 0;
    } else if (!strcmp(name, "getpid")) {
        c->eax = (uint32_t)getpid();
    } else if (!strcmp(name, "getuid")) {
        c->eax = (uint32_t)getuid();
    } else if (!strcmp(name, "geteuid")) {
        c->eax = (uint32_t)geteuid();
    } else if (!strcmp(name, "getgid")) {
        c->eax = (uint32_t)getgid();
    } else if (!strcmp(name, "getegid")) {
        c->eax = (uint32_t)getegid();
    } else if (!strcmp(name, "getlogin_r")) {
        // Renvoie un numéro d'erreur Linux (0 si réussi), pas -1 + errno.
        uint32_t buffer = arg(c, 0), size = arg(c, 1);
        char login[256];
        if (getlogin_r(login, sizeof(login))) { c->eax = (uint32_t)rt_linux_errno(errno); return 1; }
        size_t length = strlen(login) + 1;
        if (length > size) { c->eax = 34; return 1; }   // ERANGE
        memcpy(rt_write_span(c, buffer, (uint32_t)length), login, length);
        c->eax = 0;
    } else if (!strcmp(name, "getpwuid")) {
        struct passwd entry, *found = NULL; char buffer[16384];
        errno = 0;
        int error = getpwuid_r((uid_t)arg(c, 0), &entry, buffer, sizeof(buffer), &found);
        c->eax = 0;
        if (error) { errno = error; rt_save_errno(c); }
        else if (found) { if (write_passwd(c, found)) rt_save_errno(c); else c->eax = passwd_buffer; }
    } else if (!strcmp(name, "gethostname")) {
        uint32_t out = arg(c, 0), length = arg(c, 1);
        char host[256];
        if (gethostname(host, sizeof(host))) { finish(c, -1); return 1; }
        host[sizeof(host) - 1] = 0;
        size_t node = strlen(host) + 1;
        size_t copy = length < node ? length : node;
        memcpy(rt_write_span(c, out, copy), host, copy);
        if (node > length) { fail(c, 36); return 1; }
        c->eax = 0;
    } else if (!strcmp(name, "getrusage")) {
        int who = (int32_t)arg(c, 0); uint32_t out = arg(c, 1);
        if (who == 1) rt_trap(c, c->eip, "getrusage(RUSAGE_THREAD) is not implemented");
        if (who != 0 && who != -1) { fail(c, 22); return 1; }
        struct rusage usage;
        if (getrusage(who ? RUSAGE_CHILDREN : RUSAGE_SELF, &usage)) { finish(c, -1); return 1; }
        // struct rusage i386 : 2 timeval {long,long} puis 14 long (72 octets).
        long fields[14] = {usage.ru_maxrss / 1024, usage.ru_ixrss, usage.ru_idrss, usage.ru_isrss,
            usage.ru_minflt, usage.ru_majflt, usage.ru_nswap, usage.ru_inblock, usage.ru_oublock,
            usage.ru_msgsnd, usage.ru_msgrcv, usage.ru_nsignals, usage.ru_nvcsw, usage.ru_nivcsw};
        rt_write_span(c, out, 72);
        W32(out, (uint32_t)usage.ru_utime.tv_sec); W32(out + 4, (uint32_t)usage.ru_utime.tv_usec);
        W32(out + 8, (uint32_t)usage.ru_stime.tv_sec); W32(out + 12, (uint32_t)usage.ru_stime.tv_usec);
        for (int i = 0; i < 14; ++i) W32(out + 16 + 4 * (uint32_t)i, (uint32_t)(fields[i] > INT32_MAX ? INT32_MAX : fields[i]));
        c->eax = 0;
    } else if (!strcmp(name, "getloadavg")) {
        uint32_t out = arg(c, 0); int count = (int32_t)arg(c, 1);
        if (count < 0) { c->eax = UINT32_MAX; return 1; }
        double values[3]; int result = getloadavg(values, count > 3 ? 3 : count);
        if (result > 0) { rt_write_span(c, out, (uint64_t)result * 8); for (int i = 0; i < result; ++i) WD(out + 8 * (uint32_t)i, values[i]); }
        c->eax = (uint32_t)result;
    } else if (!strcmp(name, "kill")) {
        int sig = (int32_t)arg(c, 1), native = sig ? rt_darwin_signal(sig) : 0;
        if (sig && !native) { fail(c, 22); return 1; }
        finish(c, kill((pid_t)(int32_t)arg(c, 0), native));
    } else if (!strcmp(name, "wait") || !strcmp(name, "waitpid")) {
        int wide = !strcmp(name, "waitpid");
        pid_t pid = wide ? (pid_t)(int32_t)arg(c, 0) : -1;
        uint32_t out = arg(c, wide ? 1 : 0), options = wide ? arg(c, 2) : 0;
        if (options & ~(1u | 2u | 8u)) {
            if (options & 0xe0000000u) rt_trap(c, c->eip, "waitpid __WALL/__WCLONE/__WNOTHREAD not supported");
            fail(c, 22); return 1;
        }
        int native = (options & 1 ? WNOHANG : 0) | (options & 2 ? WUNTRACED : 0) | (options & 8 ? WCONTINUED : 0);
        if (out) rt_write_span(c, out, 4);
        int status = 0;
        unsigned saved = options & 1 ? 0 : rt_guest_suspend();
        pid_t result = waitpid(pid, &status, native); int error = errno;
        if (!(options & 1)) rt_guest_resume(saved);
        errno = error;
        if (result > 0 && out) W32(out, rt_linux_wait_status(status));
        finish(c, result);
    } else if (!strcmp(name, "fork")) {
        pid_t result = fork();
        finish(c, result);
    } else if (!strcmp(name, "execvp")) {
        const char *file = string(c, arg(c, 0));
        char **argv = host_argv(c, arg(c, 1));
        if (!argv) { fail(c, 7); return 1; } // E2BIG
        const char *resolved = strchr(file, '/') ? rt_read_path(file) : file;
        if (resolved) execvp(resolved, argv);
        int error = errno; free(argv); errno = error;
        finish(c, -1);
    } else if (!strcmp(name, "popen")) {
        const char *command = string(c, arg(c, 0)), *mode = string(c, arg(c, 1));
        // glibc : « r » ou « w », suivi éventuellement de « e » (O_CLOEXEC).
        if ((mode[0] != 'r' && mode[0] != 'w') || (mode[1] && (mode[1] != 'e' || mode[2]))) { c->eax = 0; rt_set_errno(c, 22); return 1; }
        char native[2] = {mode[0], 0};
        FILE *file = popen(command, native);
        if (!file) { rt_save_errno(c); c->eax = 0; return 1; }
        if (mode[1] == 'e') fcntl(fileno(file), F_SETFD, FD_CLOEXEC);
        c->eax = rt_stream_new(file);
        if (!c->eax) { pclose(file); rt_set_errno(c, 12); }
    } else if (!strcmp(name, "pclose")) {
        FILE *file = rt_stream_take(c, arg(c, 0));
        unsigned saved = rt_guest_suspend();
        int status = pclose(file), error = errno;
        rt_guest_resume(saved);
        errno = error;
        if (status == -1) finish(c, -1); else c->eax = rt_linux_wait_status(status);
    } else if (!strcmp(name, "system")) {
        uint32_t command = arg(c, 0);
        if (!command) { c->eax = !access("/bin/sh", X_OK); return 1; }
        const char *text = string(c, command);
        unsigned saved = rt_guest_suspend();
#if TARGET_OS_IPHONE
        int status = -1, error = ENOSYS; (void)text;   // pas de shell ni de processus fils sur iOS
#else
        int status = system(text), error = errno;
#endif
        rt_guest_resume(saved);
        errno = error;
        if (status == -1) finish(c, -1); else c->eax = rt_linux_wait_status(status);
    } else if (!strcmp(name, "_exit")) {
        _exit((int)(arg(c, 0) & 0xff));
    } else if (!strcmp(name, "exit")) {
        int status = (int)arg(c, 0);
        run_guest_atexit(c);
        exit(status);
    } else if (!strcmp(name, "alarm")) {
        c->eax = alarm(arg(c, 0));
    } else if (!strcmp(name, "uuid_generate")) {
        // libuuid : UUID aléatoire version 4 quand une source aléatoire existe.
        uint8_t *out = rt_write_span(c, arg(c, 0), 16);
        arc4random_buf(out, 16);
        out[6] = (uint8_t)((out[6] & 0x0f) | 0x40);
        out[8] = (uint8_t)((out[8] & 0x3f) | 0x80);
    } else if (!strcmp(name, "mallinfo")) {
        // Retour de structure i386 : pointeur caché en premier argument, que
        // l'appelé retire de la pile (ret $4) et renvoie dans EAX.
        uint32_t out = arg(c, 0), arena, used, free_bytes, free_blocks;
        rt_heap_info(&arena, &used, &free_bytes, &free_blocks);
        memset(rt_write_span(c, out, 40), 0, 40);
        W32(out, arena); W32(out + 4, free_blocks); W32(out + 28, used); W32(out + 32, free_bytes);
        uint32_t return_address = R32(c->esp);
        c->esp += 4; W32(c->esp, return_address);
        c->eax = out;
    } else if (!strcmp(name, "mount")) {
        if (geteuid()) { fail(c, 1); return 1; } // EPERM sans CAP_SYS_ADMIN
        rt_trap(c, c->eip, "mount as root is not supported");
    } else return 0;
    return 1;
}
RT_SHIM_PROVIDER(process_shim)
