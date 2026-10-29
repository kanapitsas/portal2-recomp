// P2_FRAME_TIMES=<path>: one line per present (format in frame_times.h).
// Presents are serialized by the guest gate in practice; the mutex only
// guards against the exit-time flush and tests.
#include "frame_times.h"
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static int state = -1;   // -1: variable not read yet, 0: off, 1: on
static int fd = -1;
static char buffer[1 << 16];
static size_t used;
static uint64_t last_flush, recorded;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_once_t once = PTHREAD_ONCE_INIT;

uint64_t rt_frame_times_now(void) { return clock_gettime_nsec_np(CLOCK_UPTIME_RAW); }

static void flush_locked(void) {
    size_t done = 0;
    while (fd >= 0 && done < used) {
        ssize_t n = write(fd, buffer + done, used - done);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) break;
        done += (size_t)n;
    }
    used = 0;
}
void rt_frame_times_flush(void) {
    pthread_mutex_lock(&lock);
    flush_locked();
    pthread_mutex_unlock(&lock);
}

int rt_frame_times_open(const char *path) {
    static int registered;
    pthread_mutex_lock(&lock);
    flush_locked();
    if (fd >= 0) close(fd);
    fd = -1;
    if (path && *path) {
        fd = open(path, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0644);
        if (fd < 0) fprintf(stderr, "[frame-times] cannot open %s: %s\n", path, strerror(errno));
        else {
            used = (size_t)snprintf(buffer, sizeof(buffer), "# p2-frame-times v1 clock=uptime_raw_ns pid=%d\n", (int)getpid());
            last_flush = rt_frame_times_now();
            if (!registered) { registered = 1; atexit(rt_frame_times_flush); }
        }
    }
    state = fd >= 0;
    pthread_mutex_unlock(&lock);
    return state;
}

static void open_from_environment(void) { if (state < 0) rt_frame_times_open(getenv("P2_FRAME_TIMES")); }
int rt_frame_times_enabled(void) {
    if (__builtin_expect(state < 0, 0)) pthread_once(&once, open_from_environment);
    return state > 0;
}

void rt_frame_times_record(char renderer, uint64_t start) {
    uint64_t end = rt_frame_times_now();
    uint64_t cpu = clock_gettime_nsec_np(CLOCK_THREAD_CPUTIME_ID);
    pthread_mutex_lock(&lock);
    if (fd >= 0) {
        if (sizeof(buffer) - used < 96) flush_locked();
        used += (size_t)snprintf(buffer + used, sizeof(buffer) - used, "%c %llu %llu %llu\n", renderer,
                                 (unsigned long long)start, (unsigned long long)(end - start), (unsigned long long)cpu);
        ++recorded;
        if (end - last_flush >= 1000000000ull) { flush_locked(); last_flush = end; }
    }
    pthread_mutex_unlock(&lock);
}

uint64_t rt_frame_times_count(void) {
    pthread_mutex_lock(&lock);
    uint64_t n = recorded;
    pthread_mutex_unlock(&lock);
    return n;
}
