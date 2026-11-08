// Sockets avec constantes et structures Linux i386 : aller-retour UDP et TCP
// en boucle locale, select/poll, options, erreurs Linux et gethostbyname.
#include "checks.h"
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>

#define STACK 0x20010000u
static CPU net_cpu;
static uint32_t net_call(const char *name, unsigned count, ...) {
    rt_cpu_reset(&net_cpu); net_cpu.esp = STACK;
    va_list list; va_start(list, count);
    for (unsigned i = 0; i < count; ++i) W32(STACK + 4 + 4 * i, va_arg(list, uint32_t));
    va_end(list);
    assert(rt_shim(&net_cpu, name));
    return net_cpu.eax;
}
static uint32_t net_errno(void) { CPU c; rt_cpu_reset(&c); c.esp = STACK; assert(rt_shim(&c, "__errno_location")); return R32(c.eax); }
// sockaddr_in Linux : {u16 famille, u16 port (réseau), u32 adresse (réseau), 8 zéros}.
static void linux_sockaddr(uint32_t at, uint16_t port_network, uint32_t address_network) {
    memset(GPTR(at), 0, 16); W16(at, 2); W16(at + 2, port_network); W32(at + 4, address_network);
}
static uint32_t loopback(void) { return 0x0100007fu; } // 127.0.0.1 en ordre réseau

RT_CHECK(net_udp_loopback_round_trip) {
    uint32_t a = 0x2001e000u, b = 0x2001e020u, len = 0x2001e040u, text = 0x2001e100u, out = 0x2001e200u;
    int32_t receiver = (int32_t)net_call("socket", 3, 2, 2, 0), sender = (int32_t)net_call("socket", 3, 2, 2, 0);
    assert(receiver >= 0 && sender >= 0);
    W32(text, 1);
    assert(net_call("setsockopt", 5, (uint32_t)receiver, 1, 2, text, 4) == 0); // SOL_SOCKET/SO_REUSEADDR Linux
    linux_sockaddr(a, 0, loopback());
    assert(net_call("bind", 3, (uint32_t)receiver, a, 16) == 0);
    memset(GPTR(b), 0xcc, 32); W32(len, 32);
    assert(net_call("getsockname", 3, (uint32_t)receiver, b, len) == 0);
    assert(R32(len) == 16 && R16(b) == 2 && R16(b + 2) != 0 && R32(b + 4) == loopback() && !R64(b + 8));
    uint16_t port = R16(b + 2);
    // Rien à lire : MSG_DONTWAIT Linux (0x40) donne EAGAIN Linux (11).
    assert(net_call("recv", 4, (uint32_t)receiver, out, 64, 0x40) == UINT32_MAX && net_errno() == 11);
    strcpy(GPTR(text), "portal");
    linux_sockaddr(a, port, loopback());
    assert(net_call("sendto", 6, (uint32_t)sender, text, 6, 0x4000 /* MSG_NOSIGNAL */, a, 16) == 6);
    // select Linux : fd_set i386 et temps restant écrit dans le timeval.
    uint32_t set = 0x2001e300u, timeout = 0x2001e400u;
    memset(GPTR(set), 0, 128); W32(set + 4 * ((uint32_t)receiver / 32), 1u << (receiver % 32));
    W32(timeout, 2); W32(timeout + 4, 0);
    assert(net_call("select", 5, (uint32_t)receiver + 1, set, 0, 0, timeout) == 1);
    assert(R32(set + 4 * ((uint32_t)receiver / 32)) & (1u << (receiver % 32)));
    assert(R32(timeout) <= 2 && (R32(timeout) == 1 || R32(timeout) == 2) && R32(timeout + 4) < 1000000);
    // poll : POLLIN Linux, revents traduit.
    uint32_t fds = 0x2001e500u; W32(fds, (uint32_t)receiver); W16(fds + 4, 0x1 | 0x40); W16(fds + 6, 0xffff);
    assert(net_call("poll", 3, fds, 1, 1000) == 1 && R16(fds + 6) == (0x1 | 0x40));
    memset(GPTR(b), 0, 32); W32(len, 8); // tampon trop petit : longueur réelle rendue
    assert(net_call("recvfrom", 6, (uint32_t)receiver, out, 64, 0, b, len) == 6 && !memcmp(GPTR(out), "portal", 6));
    assert(R32(len) == 16 && R16(b) == 2 && R32(b + 4) == loopback() && !R32(b + 8));
    // select expiré : 0, ensembles vidés, temps restant nul.
    memset(GPTR(set), 0, 128); W32(set + 4 * ((uint32_t)receiver / 32), 1u << (receiver % 32));
    W32(timeout, 0); W32(timeout + 4, 1000);
    assert(net_call("select", 5, (uint32_t)receiver + 1, set, 0, 0, timeout) == 0);
    assert(!R32(set + 4 * ((uint32_t)receiver / 32)) && !R32(timeout) && !R32(timeout + 4));
    // SO_RCVTIMEO Linux (20) avec timeval i386 : recv bloquant expire avec EAGAIN.
    W32(text, 0); W32(text + 4, 50000);
    assert(net_call("setsockopt", 5, (uint32_t)receiver, 1, 20, text, 8) == 0);
    assert(net_call("recv", 4, (uint32_t)receiver, out, 64, 0) == UINT32_MAX && net_errno() == 11);
    // FIONBIO par ioctl Linux.
    W32(text, 1);
    assert(net_call("ioctl", 3, (uint32_t)sender, 0x5421u, text) == 0 && (fcntl(sender, F_GETFL) & O_NONBLOCK));
    close(receiver); close(sender);
}

RT_CHECK(net_tcp_loopback_round_trip) {
    uint32_t a = 0x2001e000u, b = 0x2001e020u, len = 0x2001e040u, text = 0x2001e100u, out = 0x2001e200u;
    int32_t server = (int32_t)net_call("socket", 3, 2, 1 | 0x80000 /* SOCK_CLOEXEC */, 0);
    assert(server >= 0 && (fcntl(server, F_GETFD) & FD_CLOEXEC));
    linux_sockaddr(a, 0, loopback());
    assert(net_call("bind", 3, (uint32_t)server, a, 16) == 0 && net_call("listen", 2, (uint32_t)server, 4) == 0);
    W32(len, 16); assert(net_call("getsockname", 3, (uint32_t)server, b, len) == 0);
    int32_t client = (int32_t)net_call("socket", 3, 2, 1, 0); assert(client >= 0);
    W32(text, 1); assert(net_call("setsockopt", 5, (uint32_t)client, 6, 1, text, 4) == 0); // TCP_NODELAY
    assert(net_call("connect", 3, (uint32_t)client, b, 16) == 0);
    W32(len, 16); memset(GPTR(a), 0, 16);
    int32_t peer = (int32_t)net_call("accept", 3, (uint32_t)server, a, len);
    assert(peer >= 0 && R32(len) == 16 && R16(a) == 2 && R32(a + 4) == loopback());
    strcpy(GPTR(text), "cake");
    assert(net_call("send", 4, (uint32_t)client, text, 4, 0) == 4);
    assert(net_call("recv", 4, (uint32_t)peer, out, 4, 0x100 /* MSG_WAITALL */) == 4 && !memcmp(GPTR(out), "cake", 4));
    close(client);
    assert(net_call("recv", 4, (uint32_t)peer, out, 4, 0) == 0);
    close(peer); close(server);
    // Connexion refusée : ECONNREFUSED Linux (111).
    int32_t lonely = (int32_t)net_call("socket", 3, 2, 1, 0);
    assert(net_call("connect", 3, (uint32_t)lonely, b, 16) == UINT32_MAX && net_errno() == 111);
    close(lonely);
}

RT_CHECK(net_errors_and_name_lookup) {
    uint32_t a = 0x2001e000u, text = 0x2001e100u;
    assert(net_call("socket", 3, 99, 2, 0) == UINT32_MAX && net_errno() == 97);
    assert(net_call("socket", 3, 2, 2 | 0x100, 0) == UINT32_MAX && net_errno() == 22);
    int32_t fd = (int32_t)net_call("socket", 3, 2, 2 | 0x800 /* SOCK_NONBLOCK */, 0);
    assert(fd >= 0 && (fcntl(fd, F_GETFL) & O_NONBLOCK));
    assert(net_call("recv", 4, (uint32_t)fd, text, 16, 0) == UINT32_MAX && net_errno() == 11);
    memset(GPTR(a), 0, 16); W16(a, 99);
    assert(net_call("bind", 3, (uint32_t)fd, a, 16) == UINT32_MAX && net_errno() == 97);
    linux_sockaddr(a, 0, loopback());
    assert(net_call("bind", 3, (uint32_t)fd, a, 8) == UINT32_MAX && net_errno() == 22);
    assert(net_call("bind", 3, 9999, a, 16) == UINT32_MAX && net_errno() == 9);
    assert(net_call("send", 4, (uint32_t)fd, text, 1, 0) == UINT32_MAX && net_errno() == 89); // EDESTADDRREQ Linux
    close(fd);
    strcpy(GPTR(text), "1.2.3.4"); assert(net_call("inet_addr", 1, text) == 0x04030201u);
    strcpy(GPTR(text), "nope"); assert(net_call("inet_addr", 1, text) == UINT32_MAX);
    strcpy(GPTR(text), "127.0.0.1");
    uint32_t host = net_call("gethostbyname", 1, text);
    assert(host && R32(host + 8) == 2 && R32(host + 12) == 4 && !R32(R32(host + 4)));
    uint32_t list = R32(host + 16);
    assert(R32(R32(list)) == loopback() && !R32(list + 4) && !strcmp(GPTR(R32(host)), "127.0.0.1"));
    strcpy(GPTR(text), "localhost");
    host = net_call("gethostbyname", 1, text);
    assert(host && R32(host + 8) == 2 && R32(R32(R32(host + 16))) == loopback());
}

RT_CHECK(net_inet_text_conversions) {
    uint32_t text = 0x2001e100u, bytes = 0x2001e200u, out = 0x2001e300u;
    strcpy(GPTR(text), "192.168.1.20");
    assert(net_call("inet_pton", 3, 2, text, bytes) == 1 && R32(bytes) == 0x1401a8c0u);
    assert(net_call("inet_ntop", 4, 2, bytes, out, 16) == out && !strcmp(GPTR(out), "192.168.1.20"));
    // Tampon trop petit : NULL et ENOSPC Linux (28).
    assert(net_call("inet_ntop", 4, 2, bytes, out, 8) == 0 && net_errno() == 28);
    strcpy(GPTR(text), "fe80::1:2");
    assert(net_call("inet_pton", 3, 10, text, bytes) == 1 && R8(bytes) == 0xfe && R8(bytes + 1) == 0x80 && R8(bytes + 15) == 2);
    assert(net_call("inet_ntop", 4, 10, bytes, out, 46) == out && !strcmp(GPTR(out), "fe80::1:2"));
    strcpy(GPTR(text), "1.2.3"); assert(net_call("inet_pton", 3, 2, text, bytes) == 0);
    assert(net_call("inet_pton", 3, 99, text, bytes) == UINT32_MAX && net_errno() == 97);
    uint32_t first = net_call("inet_ntoa", 1, loopback());
    assert(first && !strcmp(GPTR(first), "127.0.0.1"));
    assert(net_call("inet_ntoa", 1, 0x0403020au) == first && !strcmp(GPTR(first), "10.2.3.4")); // tampon statique
}

RT_CHECK(net_getaddrinfo_linux_layout) {
    uint32_t node = 0x2001e100u, service = 0x2001e140u, hints = 0x2001e200u, out = 0x2001e300u;
    strcpy(GPTR(node), "127.0.0.1"); strcpy(GPTR(service), "27015");
    memset(GPTR(hints), 0, 32);
    W32(hints, 0x4 | 0x400); W32(hints + 4, 2); W32(hints + 8, 2); // AI_NUMERICHOST|AI_NUMERICSERV, AF_INET, SOCK_DGRAM
    W32(out, 0xdeadbeef);
    assert(net_call("getaddrinfo", 4, node, service, hints, out) == 0);
    uint32_t info = R32(out);
    assert(info && R32(info) == (0x4 | 0x400) && R32(info + 4) == 2 && R32(info + 8) == 2 && R32(info + 12) == 17);
    assert(R32(info + 16) == 16 && !R32(info + 24) && !R32(info + 28));
    uint32_t sa = R32(info + 20);
    assert(sa && R16(sa) == 2 && R16(sa + 2) == 0x8769 /* htons(27015) */ && R32(sa + 4) == loopback() && !R64(sa + 8));
    net_call("freeaddrinfo", 1, info);
    // IPv6 numérique : sockaddr_in6 Linux de 28 octets.
    strcpy(GPTR(node), "::1"); W32(hints + 4, 10);
    assert(net_call("getaddrinfo", 4, node, service, hints, out) == 0);
    info = R32(out); sa = R32(info + 20);
    assert(R32(info + 4) == 10 && R32(info + 16) == 28 && R16(sa) == 10 && R16(sa + 2) == 0x8769 && R8(sa + 23) == 1);
    net_call("freeaddrinfo", 1, info);
    // Erreurs EAI_* Linux négatives.
    strcpy(GPTR(node), "not-a-number"); W32(hints + 4, 2);
    assert(net_call("getaddrinfo", 4, node, service, hints, out) == (uint32_t)-2); // EAI_NONAME
    strcpy(GPTR(node), "127.0.0.1"); W32(hints + 4, 99);
    assert(net_call("getaddrinfo", 4, node, service, hints, out) == (uint32_t)-6); // EAI_FAMILY
    W32(hints + 4, 2); W32(hints, 0x10000);
    assert(net_call("getaddrinfo", 4, node, service, hints, out) == (uint32_t)-1); // EAI_BADFLAGS
}

RT_CHECK(net_getifaddrs_loopback) {
    uint32_t out = 0x2001e300u;
    assert(net_call("getifaddrs", 1, out) == 0);
    int found = 0;
    for (uint32_t i = R32(out); i; i = R32(i)) {
        uint32_t addr = R32(i + 12), flags = R32(i + 8);
        assert(addr && (R16(addr) == 2 || R16(addr) == 10) && !R32(i + 24) && R32(i + 4));
        if (R16(addr) == 2 && R32(addr + 4) == loopback()) {
            assert((flags & 0x8) && (flags & 0x1) && !strcmp(GPTR(R32(i + 4)), "lo0"));
            uint32_t mask = R32(i + 16);
            assert(mask && R16(mask) == 2 && R32(mask + 4) == 0x000000ffu); // 255.0.0.0
            found = 1;
        }
    }
    assert(found);
    net_call("freeifaddrs", 1, R32(out));
}

RT_CHECK(net_getsockopt_getpeername_shutdown) {
    uint32_t a = 0x2001e000u, b = 0x2001e020u, len = 0x2001e040u, value = 0x2001e100u, out = 0x2001e200u;
    int32_t udp = (int32_t)net_call("socket", 3, 2, 2, 0);
    W32(value, 0xffffffff); W32(len, 4);
    assert(net_call("getsockopt", 5, (uint32_t)udp, 1, 3, value, len) == 0 && R32(value) == 2 && R32(len) == 4); // SO_TYPE
    assert(net_call("getsockopt", 5, (uint32_t)udp, 1, 4, value, len) == 0 && R32(value) == 0); // SO_ERROR
    W32(value, 1); assert(net_call("setsockopt", 5, (uint32_t)udp, 1, 6, value, 4) == 0); // SO_BROADCAST
    W32(value, 0); W32(len, 8);
    assert(net_call("getsockopt", 5, (uint32_t)udp, 1, 6, value, len) == 0 && R32(value) != 0 && R32(len) == 4);
    W32(value, 0); W32(value + 4, 250000); assert(net_call("setsockopt", 5, (uint32_t)udp, 1, 20, value, 8) == 0);
    memset(GPTR(value), 0, 8); W32(len, 8);
    assert(net_call("getsockopt", 5, (uint32_t)udp, 1, 20, value, len) == 0 && R32(len) == 8 && R32(value) == 0 && R32(value + 4) == 250000);
    // getpeername sur une socket non connectée : ENOTCONN Linux (107).
    W32(len, 16); assert(net_call("getpeername", 3, (uint32_t)udp, b, len) == UINT32_MAX && net_errno() == 107);
    close(udp);
    int32_t server = (int32_t)net_call("socket", 3, 2, 1, 0);
    linux_sockaddr(a, 0, loopback());
    assert(net_call("bind", 3, (uint32_t)server, a, 16) == 0 && net_call("listen", 2, (uint32_t)server, 4) == 0);
    W32(len, 16); assert(net_call("getsockname", 3, (uint32_t)server, b, len) == 0);
    int32_t client = (int32_t)net_call("socket", 3, 2, 1, 0);
    assert(net_call("connect", 3, (uint32_t)client, b, 16) == 0);
    W32(len, 16); int32_t peer = (int32_t)net_call("accept", 3, (uint32_t)server, a, len); assert(peer >= 0);
    memset(GPTR(out), 0, 16); W32(len, 16);
    assert(net_call("getpeername", 3, (uint32_t)client, out, len) == 0 && R32(len) == 16 && !memcmp(GPTR(out), GPTR(b), 16));
    W32(value, 1); assert(net_call("setsockopt", 5, (uint32_t)client, 6, 1, value, 4) == 0);
    W32(len, 4); assert(net_call("getsockopt", 5, (uint32_t)client, 6, 1, value, len) == 0 && R32(value) != 0); // TCP_NODELAY
    assert(net_call("getsockopt", 5, (uint32_t)client, 1, 4, value, len) == 0 && R32(value) == 0);
    // shutdown(SHUT_WR) : le pair lit la fin de flux.
    assert(net_call("shutdown", 2, (uint32_t)client, 1) == 0);
    assert(net_call("recv", 4, (uint32_t)peer, out, 4, 0) == 0);
    assert(net_call("shutdown", 2, 9999, 2) == UINT32_MAX && net_errno() == 9);
    close(peer); close(client); close(server);
    // SO_ERROR traduit : connexion refusée en non bloquant -> ECONNREFUSED Linux (111).
    int32_t lonely = (int32_t)net_call("socket", 3, 2, 1 | 0x800, 0);
    uint32_t r = net_call("connect", 3, (uint32_t)lonely, b, 16);
    if (r == UINT32_MAX && net_errno() == 115) { // EINPROGRESS
        uint32_t fds = 0x2001e500u; W32(fds, (uint32_t)lonely); W16(fds + 4, 0x4); W16(fds + 6, 0);
        net_call("poll", 3, fds, 1, 2000);
        W32(len, 4); assert(net_call("getsockopt", 5, (uint32_t)lonely, 1, 4, value, len) == 0 && R32(value) == 111);
    } else assert(r == UINT32_MAX && net_errno() == 111);
    close(lonely);
}
