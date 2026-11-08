// tcgetattr/tcsetattr on a pseudo-terminal, in the glibc i386 layout.
// Expected Linux values come from <asm-generic/termbits.h> (i386), not from the
// shim's tables.
#include "checks.h"
#include <stdarg.h>
#include <termios.h>
#include <unistd.h>
#include <util.h>

#define STACK 0x20010000u
#define BUFFER 0x20014000u

static uint32_t call(const char *name, unsigned n, ...) {
    va_list list; va_start(list, n);
    for (unsigned i = 0; i < n; ++i) W32(STACK + 4 * i, va_arg(list, uint32_t));
    va_end(list);
    CPU c; rt_cpu_reset(&c); c.esp = STACK;
    rt_call(&c, rt_dynamic_import(name), RT_RETURN_SENTINEL);
    assert(c.esp == STACK);
    return c.eax;
}

RT_CHECK(termios_pty_round_trip) {
    int master, slave;
    assert(!openpty(&master, &slave, NULL, NULL, NULL));
    struct termios host;
    assert(!tcgetattr(slave, &host));
    host.c_lflag |= ICANON | ECHO | ISIG; host.c_iflag |= ICRNL; host.c_oflag |= OPOST | ONLCR;
    host.c_cflag = (host.c_cflag & ~CSIZE) | CS8 | CREAD;
    host.c_cc[VINTR] = 3; host.c_cc[VEOF] = 4; host.c_cc[VMIN] = 1; host.c_cc[VTIME] = 0;
    cfsetspeed(&host, B9600);
    assert(!tcsetattr(slave, TCSANOW, &host));

    memset(GPTR(BUFFER), 0xcc, 64);
    assert(call("tcgetattr", 2, slave, BUFFER) == 0);
    uint32_t iflag = R32(BUFFER), oflag = R32(BUFFER + 4), cflag = R32(BUFFER + 8), lflag = R32(BUFFER + 12);
    assert(iflag & 0400);                                   // ICRNL
    assert((oflag & 05) == 05);                             // OPOST | ONLCR
    assert((cflag & 060) == 060 && (cflag & 0200));         // CS8, CREAD
    assert((cflag & 010017) == 015);                        // B9600 in CBAUD
    assert((lflag & 013) == 013);                           // ISIG | ICANON | ECHO
    assert(R8(BUFFER + 16) == 0);                           // c_line
    assert(R8(BUFFER + 17 + 0) == 3 && R8(BUFFER + 17 + 4) == 4);   // VINTR, VEOF
    assert(R8(BUFFER + 17 + 6) == 1 && R8(BUFFER + 17 + 5) == 0);   // VMIN, VTIME
    assert(R32(BUFFER + 52) == 015 && R32(BUFFER + 56) == 015);     // c_ispeed, c_ospeed
    assert(R8(BUFFER + 60) == 0xcc);                        // nothing past the 60 bytes

    // Typical raw-console setup: ~(ICANON | ECHO), VMIN = 0, VTIME = 0.
    W32(BUFFER + 12, lflag & ~012u);
    W8(BUFFER + 17 + 6, 0);
    assert(call("tcsetattr", 3, slave, 0, BUFFER) == 0);   // TCSANOW
    assert(!tcgetattr(slave, &host));
    assert(!(host.c_lflag & (ICANON | ECHO)) && (host.c_lflag & ISIG));
    assert(host.c_cc[VMIN] == 0 && host.c_cc[VINTR] == 3);
    assert(call("tcsetattr", 3, slave, 7, BUFFER) == UINT32_MAX);   // bad action: EINVAL

    int fds[2]; assert(!pipe(fds));
    assert(call("tcgetattr", 2, fds[0], BUFFER) == UINT32_MAX);    // ENOTTY on a pipe
    close(fds[0]); close(fds[1]); close(slave); close(master);
}
