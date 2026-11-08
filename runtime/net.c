// Sockets BSD avec les constantes et structures Linux i386. Les descripteurs
// sont ceux de l'hôte (comme open) ; sockaddr Linux {u16 famille, ...} est
// traduit vers sockaddr Darwin {u8 len, u8 famille, ...}. fd_set Linux i386 et
// Darwin ont la même disposition (32 mots de 32 bits, bit fd%32 du mot fd/32).
// Les appels bloquants libèrent le verrou d'exécution invité.
// SHIMS: socket bind listen accept connect send recv sendto recvfrom select poll
// SHIMS: setsockopt getsockname gethostbyname inet_addr
// SHIMS: getpeername getsockopt shutdown inet_ntop inet_pton inet_ntoa
// SHIMS: getaddrinfo freeaddrinfo getifaddrs freeifaddrs
#include "posix.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

static void finish(CPU *c, long result) { if (result < 0) rt_save_errno(c); c->eax = (uint32_t)result; }
static void fail(CPU *c, int linux_errno) { rt_set_errno(c, linux_errno); c->eax = UINT32_MAX; }

enum { L_AF_UNSPEC = 0, L_AF_UNIX = 1, L_AF_INET = 2, L_AF_INET6 = 10 };

// sockaddr invité Linux -> sockaddr hôte. Renvoie un errno Linux, 0 si succès.
static int host_address(CPU *c, uint32_t address, uint32_t length, struct sockaddr_storage *out, socklen_t *out_length, int creating) {
    if (!address || length < 2 || length > 128) return 22;
    const uint8_t *in = rt_span(c, address, length);
    uint16_t family; memcpy(&family, in, 2);
    memset(out, 0, sizeof(*out));
    switch (family) {
    case L_AF_INET: {
        if (length < 16) return 22;
        struct sockaddr_in *v4 = (struct sockaddr_in *)out;
        v4->sin_len = sizeof(*v4); v4->sin_family = AF_INET;
        memcpy(&v4->sin_port, in + 2, 2); memcpy(&v4->sin_addr, in + 4, 4);
        *out_length = sizeof(*v4); return 0;
    }
    case L_AF_INET6: {
        if (length < 24) return 22;
        struct sockaddr_in6 *v6 = (struct sockaddr_in6 *)out;
        v6->sin6_len = sizeof(*v6); v6->sin6_family = AF_INET6;
        memcpy(&v6->sin6_port, in + 2, 2); memcpy(&v6->sin6_flowinfo, in + 4, 4);
        memcpy(&v6->sin6_addr, in + 8, 16);
        if (length >= 28) memcpy(&v6->sin6_scope_id, in + 24, 4);
        *out_length = sizeof(*v6); return 0;
    }
    case L_AF_UNIX: {
        struct sockaddr_un *un = (struct sockaddr_un *)out;
        size_t path = length - 2;
        if (path && in[2] == 0) rt_trap(c, c->eip, "Linux abstract AF_UNIX socket names are not supported");
        size_t used = strnlen((const char *)in + 2, path);
        if (used >= PATH_MAX) return 36;
        // Les sockets nommées sont des fichiers : même vue fusionnée que open.
        char guest_path[PATH_MAX]; memcpy(guest_path, in + 2, used); guest_path[used] = 0;
        const char *host_path = creating ? rt_write_path(guest_path, 0) : rt_read_path(guest_path);
        if (!host_path) return rt_linux_errno(errno);
        used = strlen(host_path);
        if (used >= sizeof(un->sun_path)) return 36;
        un->sun_family = AF_UNIX; memcpy(un->sun_path, host_path, used);
        un->sun_len = (uint8_t)(offsetof(struct sockaddr_un, sun_path) + used + 1);
        *out_length = un->sun_len; return 0;
    }
    case L_AF_UNSPEC:
        out->ss_len = 2; out->ss_family = AF_UNSPEC; *out_length = 2; return 0;
    default: return 97; // EAFNOSUPPORT
    }
}

// sockaddr hôte -> octets Linux (out[128]) ; renvoie la taille Linux.
static uint32_t linux_address(CPU *c, const struct sockaddr_storage *in, socklen_t in_length, uint8_t out[128]) {
    uint32_t size; memset(out, 0, 128);
    switch (in->ss_family) {
    case AF_INET: {
        const struct sockaddr_in *v4 = (const struct sockaddr_in *)in;
        uint16_t family = L_AF_INET; memcpy(out, &family, 2);
        memcpy(out + 2, &v4->sin_port, 2); memcpy(out + 4, &v4->sin_addr, 4); size = 16; break;
    }
    case AF_INET6: {
        const struct sockaddr_in6 *v6 = (const struct sockaddr_in6 *)in;
        uint16_t family = L_AF_INET6; memcpy(out, &family, 2);
        memcpy(out + 2, &v6->sin6_port, 2); memcpy(out + 4, &v6->sin6_flowinfo, 4);
        memcpy(out + 8, &v6->sin6_addr, 16); memcpy(out + 24, &v6->sin6_scope_id, 4); size = 28; break;
    }
    case AF_UNIX: {
        const struct sockaddr_un *un = (const struct sockaddr_un *)in;
        uint16_t family = L_AF_UNIX; memcpy(out, &family, 2);
        size_t path = in_length > offsetof(struct sockaddr_un, sun_path) ? strnlen(un->sun_path, in_length - offsetof(struct sockaddr_un, sun_path)) : 0;
        memcpy(out + 2, un->sun_path, path); size = (uint32_t)(2 + path + (path ? 1 : 0)); break;
    }
    case AF_UNSPEC: size = 2; break;
    default: rt_trap(c, c->eip, "host returned an untranslated socket address family");
    }
    return size;
}

// sockaddr hôte -> invité, avec la sémantique Linux de troncature : la longueur
// réelle est rendue même si le tampon invité est plus petit.
static void guest_address(CPU *c, const struct sockaddr_storage *in, socklen_t in_length, uint32_t address, uint32_t length_address) {
    if (!address) return;
    uint8_t out[128]; uint32_t size = linux_address(c, in, in_length, out);
    rt_write_span(c, length_address, 4);
    uint32_t capacity = R32(length_address);
    if ((int32_t)capacity < 0) capacity = 0;
    uint32_t copy = capacity < size ? capacity : size;
    if (copy) memcpy(rt_write_span(c, address, copy), out, copy);
    W32(length_address, size);
}

static int host_message_flags(CPU *c, uint32_t flags, int receiving) {
    int native = 0;
    if (flags & 0x1) native |= MSG_OOB;
    if (flags & 0x2) native |= MSG_PEEK;
    if (flags & 0x4) native |= MSG_DONTROUTE;
    if (flags & 0x40) native |= MSG_DONTWAIT;
    if (flags & 0x80) native |= MSG_EOR;
    if (flags & 0x100) native |= MSG_WAITALL;
    if (flags & 0x4000) native |= MSG_NOSIGNAL;
    // MSG_CONFIRM n'est qu'une indication ARP sans effet observable.
    uint32_t known = 0x1 | 0x2 | 0x4 | 0x40 | 0x80 | 0x100 | 0x4000 | (receiving ? 0 : 0x800);
    if (flags & ~known) rt_trap(c, c->eip, "socket message flags not translated (MSG_TRUNC/MSG_MORE/...)");
    return native;
}

// Les opérations réseau bloquantes libèrent le verrou invité (RT_BLOCKING, rt.h).

// Options entières de même forme sous Linux et Darwin : niveau et nom Linux ->
// hôte. Renvoie 0 si l'option n'est pas de cette forme (cas particuliers).
static int integer_option(int level, int name, int *native_level, int *native_name) {
    static const struct { int level, name, native_level, native_name; } table[] = {
        {1, 2, SOL_SOCKET, SO_REUSEADDR}, {1, 5, SOL_SOCKET, SO_DONTROUTE}, {1, 6, SOL_SOCKET, SO_BROADCAST},
        {1, 7, SOL_SOCKET, SO_SNDBUF}, {1, 8, SOL_SOCKET, SO_RCVBUF}, {1, 9, SOL_SOCKET, SO_KEEPALIVE},
        {1, 10, SOL_SOCKET, SO_OOBINLINE}, {1, 15, SOL_SOCKET, SO_REUSEPORT}, {1, 18, SOL_SOCKET, SO_RCVLOWAT},
        {1, 19, SOL_SOCKET, SO_SNDLOWAT},
        {0, 1, IPPROTO_IP, IP_TOS}, {0, 2, IPPROTO_IP, IP_TTL},
        {6, 1, IPPROTO_TCP, TCP_NODELAY}, {6, 4, IPPROTO_TCP, TCP_KEEPALIVE},
        {6, 5, IPPROTO_TCP, TCP_KEEPINTVL}, {6, 6, IPPROTO_TCP, TCP_KEEPCNT},
        {41, 26, IPPROTO_IPV6, IPV6_V6ONLY},
    };
    for (size_t i = 0; i < sizeof(table) / sizeof(*table); ++i)
        if (table[i].level == level && table[i].name == name) {
            *native_level = table[i].native_level; *native_name = table[i].native_name; return 1;
        }
    return 0;
}

static _Noreturn void untranslated_option(CPU *c, int level) {
    rt_trap(c, c->eip, level == 1 ? "SOL_SOCKET option not translated" : level == 0 ? "IPPROTO_IP option not translated"
                     : level == 6 ? "IPPROTO_TCP option not translated" : level == 41 ? "IPPROTO_IPV6 option not translated"
                     : "socket option level not translated");
}

static int socket_option(CPU *c, int fd, int level, int name, uint32_t value, uint32_t length) {
    if (length) rt_span(c, value, length);
    int integer = length >= 4 ? (int32_t)R32(value) : 0, native_level, native_name;
    if (integer_option(level, name, &native_level, &native_name)) {
        if (length < 4) { errno = EINVAL; return -1; }
        return setsockopt(fd, native_level, native_name, &integer, sizeof(integer));
    }
    if (level == 1) switch (name) {
    case 13: { // SO_LINGER : struct linger {int,int} identique
        if (length < 8) { errno = EINVAL; return -1; }
        struct linger linger = {(int32_t)R32(value), (int32_t)R32(value + 4)};
        return setsockopt(fd, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger));
    }
    case 20: case 21: { // SO_RCVTIMEO/SO_SNDTIMEO : timeval i386 {long,long}
        if (length < 8) { errno = EINVAL; return -1; }
        struct timeval tv = {(int32_t)R32(value), (int32_t)R32(value + 4)};
        return setsockopt(fd, SOL_SOCKET, name == 20 ? SO_RCVTIMEO : SO_SNDTIMEO, &tv, sizeof(tv));
    }
    case 3: case 4: errno = ENOPROTOOPT; return -1; // SO_TYPE/SO_ERROR : lecture seule
    }
    if (level == 0) switch (name) {
    case 33: case 34: { // IP_MULTICAST_TTL/LOOP : int ou octet sous Linux, u_char sous Darwin
        if (!length) { errno = EINVAL; return -1; }
        unsigned char byte = (unsigned char)(length >= 4 ? integer : R8(value));
        return setsockopt(fd, IPPROTO_IP, name == 33 ? IP_MULTICAST_TTL : IP_MULTICAST_LOOP, &byte, 1);
    }
    case 32: { // IP_MULTICAST_IF : in_addr, ip_mreq ou ip_mreqn
        if (length < 4) { errno = EINVAL; return -1; }
        struct in_addr address; memcpy(&address, rt_span(c, length >= 8 ? value + 4 : value, 4), 4);
        if (length >= 12 && R32(value + 8) && !address.s_addr) rt_trap(c, c->eip, "ip_mreqn interface index not translated");
        return setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, &address, sizeof(address));
    }
    case 35: case 36: { // IP_ADD/DROP_MEMBERSHIP : ip_mreq (8) ou ip_mreqn (12)
        if (length < 8) { errno = EINVAL; return -1; }
        struct ip_mreq request; memcpy(&request, rt_span(c, value, 8), 8);
        if (length >= 12 && R32(value + 8) && !request.imr_interface.s_addr) rt_trap(c, c->eip, "ip_mreqn interface index not translated");
        return setsockopt(fd, IPPROTO_IP, name == 35 ? IP_ADD_MEMBERSHIP : IP_DROP_MEMBERSHIP, &request, sizeof(request));
    }
    }
    untranslated_option(c, level);
}

// getsockopt Linux : copie min(*optlen, taille) octets et écrit la taille réelle.
static int read_socket_option(CPU *c, int fd, int level, int name, uint32_t value, uint32_t length_address) {
    rt_write_span(c, length_address, 4);
    int32_t capacity = (int32_t)R32(length_address);
    if (capacity < 0) { errno = EINVAL; return -1; }
    uint8_t out[8] = {0}; uint32_t size = 0; int native_level, native_name, integer = 0; socklen_t got = sizeof(integer);
    if (integer_option(level, name, &native_level, &native_name)) {
        if (getsockopt(fd, native_level, native_name, &integer, &got)) return -1;
    } else if (level == 1 && (name == 3 || name == 4)) { // SO_TYPE (mêmes valeurs SOCK_*), SO_ERROR (errno traduit)
        if (getsockopt(fd, SOL_SOCKET, name == 3 ? SO_TYPE : SO_ERROR, &integer, &got)) return -1;
        if (name == 4 && integer) integer = rt_linux_errno(integer);
    } else if (level == 1 && name == 13) { // SO_LINGER
        struct linger linger; got = sizeof(linger);
        if (getsockopt(fd, SOL_SOCKET, SO_LINGER, &linger, &got)) return -1;
        int32_t pair[2] = {linger.l_onoff, linger.l_linger}; memcpy(out, pair, 8); size = 8;
    } else if (level == 1 && (name == 20 || name == 21)) { // timeval Darwin -> i386 {long,long}
        struct timeval tv; got = sizeof(tv);
        if (getsockopt(fd, SOL_SOCKET, name == 20 ? SO_RCVTIMEO : SO_SNDTIMEO, &tv, &got)) return -1;
        int32_t pair[2] = {(int32_t)tv.tv_sec, (int32_t)tv.tv_usec}; memcpy(out, pair, 8); size = 8;
    } else if (level == 0 && (name == 33 || name == 34)) { // u_char Darwin ; glibc rend un int (octet si optlen < 4)
        unsigned char byte = 0; got = 1;
        if (getsockopt(fd, IPPROTO_IP, name == 33 ? IP_MULTICAST_TTL : IP_MULTICAST_LOOP, &byte, &got)) return -1;
        integer = byte; if (capacity < 4) { out[0] = byte; size = 1; }
    } else untranslated_option(c, level);
    if (!size) { memcpy(out, &integer, 4); size = 4; }
    uint32_t copy = (uint32_t)capacity < size ? (uint32_t)capacity : size;
    if (copy) memcpy(rt_write_span(c, value, copy), out, copy);
    W32(length_address, copy);
    return 0;
}

static uint32_t hostent_buffer, hostent_size;
static int lookup_host(CPU *c, const char *name) {
    struct addrinfo hints = {0}, *list = NULL;
    hints.ai_family = AF_INET; hints.ai_socktype = SOCK_STREAM; hints.ai_flags = AI_CANONNAME;
    int error = RT_BLOCKING(getaddrinfo(name, NULL, &hints, &list));
    if (error) return -1; // h_errno n'est pas exposé (pas d'import __h_errno_location)
    size_t count = 0;
    for (struct addrinfo *i = list; i; i = i->ai_next) if (i->ai_family == AF_INET) ++count;
    const char *canonical = list && list->ai_canonname ? list->ai_canonname : name;
    size_t name_size = strlen(canonical) + 1;
    // struct hostent i386 : h_name, h_aliases, h_addrtype, h_length, h_addr_list (20 octets),
    // puis aliases[1], addr_list[count+1], adresses et nom.
    uint64_t total = 20 + 4 + 4 * ((uint64_t)count + 1) + 4 * (uint64_t)count + name_size;
    if (total > hostent_size) {
        uint32_t grown = total > UINT32_MAX ? 0 : rt_realloc(hostent_buffer, (uint32_t)total);
        if (!grown) { freeaddrinfo(list); rt_set_errno(c, 12); return -1; }
        hostent_buffer = grown; hostent_size = (uint32_t)total;
    }
    uint32_t base = hostent_buffer, aliases = base + 20, addresses = aliases + 4;
    uint32_t data = addresses + 4 * ((uint32_t)count + 1), text = data + 4 * (uint32_t)count;
    W32(aliases, 0);
    size_t index = 0;
    for (struct addrinfo *i = list; i; i = i->ai_next) {
        if (i->ai_family != AF_INET) continue;
        memcpy(GPTR(data + 4 * (uint32_t)index), &((struct sockaddr_in *)i->ai_addr)->sin_addr, 4);
        W32(addresses + 4 * (uint32_t)index, data + 4 * (uint32_t)index); ++index;
    }
    W32(addresses + 4 * (uint32_t)count, 0);
    memcpy(GPTR(text), canonical, name_size);
    freeaddrinfo(list);
    W32(base, text); W32(base + 4, aliases); W32(base + 8, L_AF_INET); W32(base + 12, 4); W32(base + 16, addresses);
    return 0;
}

static short host_poll_events(uint32_t events) {
    short native = 0;
    if (events & 0x1) native |= POLLIN;
    if (events & 0x2) native |= POLLPRI;
    if (events & 0x4) native |= POLLOUT;
    if (events & 0x40) native |= POLLRDNORM;
    if (events & 0x80) native |= POLLRDBAND;
    if (events & 0x100) native |= POLLWRNORM;
    if (events & 0x200) native |= POLLWRBAND;
    return native;
}
static uint16_t linux_poll_events(short native) {
    uint16_t events = 0;
    if (native & POLLIN) events |= 0x1;
    if (native & POLLPRI) events |= 0x2;
    if (native & POLLOUT) events |= 0x4;
    if (native & POLLERR) events |= 0x8;
    if (native & POLLHUP) events |= 0x10;
    if (native & POLLNVAL) events |= 0x20;
    if (native & POLLRDNORM) events |= 0x40;
    if (native & POLLRDBAND) events |= 0x80;
    if (native & POLLWRNORM) events |= 0x100;
    if (native & POLLWRBAND) events |= 0x200;
    return events;
}

static double now(void) { struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t); return (double)t.tv_sec + t.tv_nsec * 1e-9; }

// getaddrinfo : drapeaux AI_* Linux -> Darwin. -1 si un drapeau Linux inconnu
// (EAI_BADFLAGS comme glibc) ; AI_IDN et compagnie n'ont pas d'équivalent.
static int host_addrinfo_flags(CPU *c, uint32_t flags) {
    if (flags & ~0x7ffu) return -1;
    if (flags & 0x3c0) rt_trap(c, c->eip, "getaddrinfo AI_IDN/AI_CANONIDN flags not translated");
    int native = 0;
    if (flags & 0x1) native |= AI_PASSIVE;
    if (flags & 0x2) native |= AI_CANONNAME;
    if (flags & 0x4) native |= AI_NUMERICHOST;
    if (flags & 0x8) native |= AI_V4MAPPED;
    if (flags & 0x10) native |= AI_ALL;
    if (flags & 0x20) native |= AI_ADDRCONFIG;
    if (flags & 0x400) native |= AI_NUMERICSERV;
    return native;
}
// Codes EAI_* Darwin (positifs) -> Linux (négatifs).
static int32_t linux_addrinfo_error(int error) {
    switch (error) {
    case 0: return 0;
    case EAI_ADDRFAMILY: return -9; case EAI_AGAIN: return -3; case EAI_BADFLAGS: return -1;
    case EAI_FAIL: return -4; case EAI_FAMILY: return -6; case EAI_MEMORY: return -10;
    case EAI_NODATA: return -5; case EAI_NONAME: return -2; case EAI_SERVICE: return -8;
    case EAI_SOCKTYPE: return -7; case EAI_SYSTEM: return -11; case EAI_OVERFLOW: return -12;
    case EAI_BADHINTS: return -1; case EAI_PROTOCOL: return -7;
    default: return -4;
    }
}

// Libère une liste chaînée invitée dont le lien suivant est à l'offset 0 ou 28.
static void free_guest_list(uint32_t node, uint32_t next_offset) {
    while (node) { uint32_t next = R32(node + next_offset); rt_free(node); node = next; }
}

// struct addrinfo i386 (32 octets) : flags, family, socktype, protocol, addrlen,
// addr, canonname, next. Un bloc invité par nœud : addrinfo, sockaddr Linux (28), nom.
static int32_t guest_addrinfo(CPU *c, uint32_t node_text, uint32_t service_text, uint32_t hints, uint32_t out) {
    struct addrinfo native_hints = {0}, *list = NULL;
    uint32_t linux_flags = 0;
    if (hints) {
        rt_span(c, hints, 32);
        linux_flags = R32(hints);
        int flags = host_addrinfo_flags(c, linux_flags), family = (int32_t)R32(hints + 4);
        if (flags < 0) return -1;
        native_hints.ai_flags = flags;
        native_hints.ai_family = family == L_AF_UNSPEC ? AF_UNSPEC : family == L_AF_INET ? AF_INET : family == L_AF_INET6 ? AF_INET6 : -1;
        if (native_hints.ai_family < 0) return -6;
        native_hints.ai_socktype = (int32_t)R32(hints + 8); // SOCK_* et IPPROTO_* identiques
        native_hints.ai_protocol = (int32_t)R32(hints + 12);
    } else { native_hints.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG; linux_flags = 0x8 | 0x20; } // défaut glibc
    if (node_text) rt_string_length(c, node_text);
    if (service_text) rt_string_length(c, service_text);
    rt_write_span(c, out, 4);
    int error = RT_BLOCKING(getaddrinfo(node_text ? GPTR(node_text) : NULL, service_text ? GPTR(service_text) : NULL,
                                     &native_hints, &list));
    if (error) { int32_t code = linux_addrinfo_error(error); if (code == -11) rt_save_errno(c); return code; }
    uint32_t head = 0, tail = 0;
    for (struct addrinfo *i = list; i; i = i->ai_next) {
        if (i->ai_family != AF_INET && i->ai_family != AF_INET6) continue;
        size_t name = i->ai_canonname ? strlen(i->ai_canonname) + 1 : 0;
        uint32_t node = rt_alloc((uint32_t)(32 + 28 + name));
        if (!node) { freeaddrinfo(list); free_guest_list(head, 28); return -10; }
        memset(GPTR(node), 0, 32 + 28 + name);
        struct sockaddr_storage address = {0}; memcpy(&address, i->ai_addr, i->ai_addrlen);
        uint8_t bytes[128]; uint32_t size = linux_address(c, &address, i->ai_addrlen, bytes);
        memcpy(GPTR(node + 32), bytes, size);
        // glibc recopie les drapeaux de la demande dans chaque résultat.
        W32(node, linux_flags); W32(node + 4, i->ai_family == AF_INET ? L_AF_INET : L_AF_INET6);
        W32(node + 8, (uint32_t)i->ai_socktype); W32(node + 12, (uint32_t)i->ai_protocol);
        W32(node + 16, size); W32(node + 20, node + 32);
        if (name) { memcpy(GPTR(node + 60), i->ai_canonname, name); W32(node + 24, node + 60); }
        if (tail) W32(tail + 28, node); else head = node;
        tail = node;
    }
    freeaddrinfo(list);
    if (!head) return -2; // que des familles non IP : EAI_NONAME
    W32(out, head);
    return 0;
}

// Drapeaux IFF_* Darwin -> Linux : bits 0..9 identiques, IFF_MULTICAST déplacé ;
// OACTIVE/SIMPLEX/LINK* n'ont pas d'équivalent (et collisionnent avec MASTER/SLAVE).
static uint32_t linux_interface_flags(unsigned flags) {
    return (flags & 0x3ffu) | (flags & IFF_MULTICAST ? 0x1000u : 0);
}

// sockaddr d'interface Darwin -> bloc Linux à `at` ; les masques Darwin ont
// parfois sa_len tronqué et une famille nulle : on prend celle de l'adresse.
static uint32_t interface_address(CPU *c, const struct sockaddr *in, int family, uint32_t at) {
    if (!in) return 0;
    struct sockaddr_storage address = {0};
    size_t length = in->sa_len ? in->sa_len : (family == AF_INET ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6));
    if (length > sizeof(address)) length = sizeof(address);
    memcpy(&address, in, length);
    address.ss_family = (sa_family_t)family;
    address.ss_len = family == AF_INET ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
    uint8_t bytes[128]; uint32_t size = linux_address(c, &address, address.ss_len, bytes);
    memcpy(GPTR(at), bytes, size);
    return at;
}

// struct ifaddrs i386 (28 octets) : next, name, flags, addr, netmask, ifu, data.
// Seules les entrées AF_INET/AF_INET6 sont rendues (pas AF_LINK).
static int guest_ifaddrs(CPU *c, uint32_t out) {
    rt_write_span(c, out, 4);
    struct ifaddrs *list = NULL;
    if (getifaddrs(&list)) return -1;
    uint32_t head = 0, tail = 0;
    for (struct ifaddrs *i = list; i; i = i->ifa_next) {
        if (!i->ifa_addr || (i->ifa_addr->sa_family != AF_INET && i->ifa_addr->sa_family != AF_INET6)) continue;
        int family = i->ifa_addr->sa_family;
        size_t name = strlen(i->ifa_name) + 1;
        uint32_t node = rt_alloc((uint32_t)(28 + 3 * 28 + name));
        if (!node) { freeifaddrs(list); free_guest_list(head, 0); errno = ENOMEM; return -1; }
        memset(GPTR(node), 0, 28 + 3 * 28 + name);
        memcpy(GPTR(node + 112), i->ifa_name, name);
        W32(node + 4, node + 112); W32(node + 8, linux_interface_flags(i->ifa_flags));
        W32(node + 12, interface_address(c, i->ifa_addr, family, node + 28));
        W32(node + 16, interface_address(c, i->ifa_netmask, family, node + 56));
        if (i->ifa_flags & (IFF_BROADCAST | IFF_POINTOPOINT)) W32(node + 20, interface_address(c, i->ifa_dstaddr, family, node + 84));
        if (tail) W32(tail, node); else head = node;
        tail = node;
    }
    freeifaddrs(list);
    W32(out, head);
    return 0;
}

// Tampon de inet_ntoa : un seul par processus (glibc en a un par thread) ;
// alloué au premier appel dans le tas invité.
static uint32_t ntoa_buffer;

static int net_shim(CPU *c, const char *name) {
    struct sockaddr_storage address; socklen_t length; int error;
    if (!strcmp(name, "socket")) {
        int domain = (int32_t)rt_arg32(c, 0), type = (int32_t)rt_arg32(c, 1), protocol = (int32_t)rt_arg32(c, 2);
        int native_domain = domain == L_AF_INET ? AF_INET : domain == L_AF_INET6 ? AF_INET6 : domain == L_AF_UNIX ? AF_UNIX : -1;
        if (native_domain < 0) { fail(c, 97); return 1; }
        int kind = type & 0xf, flags = type & ~0xf;
        if (flags & ~(0x800 | 0x80000) || kind < 1 || kind > 5 || kind == 4) { fail(c, 22); return 1; }
        int fd = socket(native_domain, kind, protocol);
        if (fd >= 0 && (flags & 0x800) && fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK)) { error = errno; close(fd); errno = error; fd = -1; }
        if (fd >= 0 && (flags & 0x80000) && fcntl(fd, F_SETFD, FD_CLOEXEC)) { error = errno; close(fd); errno = error; fd = -1; }
        finish(c, fd);
    } else if (!strcmp(name, "bind") || !strcmp(name, "connect")) {
        int fd = (int32_t)rt_arg32(c, 0);
        if ((error = host_address(c, rt_arg32(c, 1), rt_arg32(c, 2), &address, &length, !strcmp(name, "bind")))) { fail(c, error); return 1; }
        int result = !strcmp(name, "bind") ? bind(fd, (struct sockaddr *)&address, length)
                                           : RT_BLOCKING(connect(fd, (struct sockaddr *)&address, length));
        finish(c, result);
    } else if (!strcmp(name, "listen")) {
        finish(c, listen((int32_t)rt_arg32(c, 0), (int32_t)rt_arg32(c, 1)));
    } else if (!strcmp(name, "accept")) {
        int fd = (int32_t)rt_arg32(c, 0); uint32_t out = rt_arg32(c, 1), out_length = rt_arg32(c, 2);
        length = sizeof(address);
        int result = RT_BLOCKING(accept(fd, (struct sockaddr *)&address, &length));
        if (result >= 0 && out) guest_address(c, &address, length, out, out_length);
        finish(c, result);
    } else if (!strcmp(name, "getsockname")) {
        length = sizeof(address);
        int result = getsockname((int32_t)rt_arg32(c, 0), (struct sockaddr *)&address, &length);
        if (!result) guest_address(c, &address, length, rt_arg32(c, 1), rt_arg32(c, 2));
        finish(c, result);
    } else if (!strcmp(name, "send") || !strcmp(name, "sendto")) {
        int fd = (int32_t)rt_arg32(c, 0); uint32_t buffer = rt_arg32(c, 1), size = rt_arg32(c, 2);
        int flags = host_message_flags(c, rt_arg32(c, 3), 0);
        const void *data = rt_span(c, buffer, size);
        uint32_t target = !strcmp(name, "sendto") ? rt_arg32(c, 4) : 0;
        if (target && (error = host_address(c, target, rt_arg32(c, 5), &address, &length, 0))) { fail(c, error); return 1; }
        ssize_t result = RT_BLOCKING(target ? sendto(fd, data, size, flags, (struct sockaddr *)&address, length)
                                         : send(fd, data, size, flags));
        finish(c, result);
    } else if (!strcmp(name, "recv") || !strcmp(name, "recvfrom")) {
        int fd = (int32_t)rt_arg32(c, 0); uint32_t buffer = rt_arg32(c, 1), size = rt_arg32(c, 2);
        int flags = host_message_flags(c, rt_arg32(c, 3), 1);
        void *data = rt_write_span(c, buffer, size);
        uint32_t source = !strcmp(name, "recvfrom") ? rt_arg32(c, 4) : 0, source_length = source ? rt_arg32(c, 5) : 0;
        length = sizeof(address); memset(&address, 0, sizeof(address));
        ssize_t result = RT_BLOCKING(recvfrom(fd, data, size, flags, source ? (struct sockaddr *)&address : NULL, source ? &length : NULL));
        if (result >= 0 && source) {
            if (length) guest_address(c, &address, length, source, source_length);
            else { rt_write_span(c, source_length, 4); W32(source_length, 0); }
        }
        finish(c, result);
    } else if (!strcmp(name, "setsockopt")) {
        finish(c, socket_option(c, (int32_t)rt_arg32(c, 0), (int32_t)rt_arg32(c, 1), (int32_t)rt_arg32(c, 2), rt_arg32(c, 3), rt_arg32(c, 4)));
    } else if (!strcmp(name, "select")) {
        int count = (int32_t)rt_arg32(c, 0); uint32_t sets[3] = {rt_arg32(c, 1), rt_arg32(c, 2), rt_arg32(c, 3)}, timeout = rt_arg32(c, 4);
        if (count < 0 || count > FD_SETSIZE) { fail(c, 22); return 1; }
        fd_set native[3]; fd_set *pointers[3]; size_t bytes = (size_t)((count + 31) / 32) * 4;
        for (int i = 0; i < 3; ++i) {
            pointers[i] = sets[i] ? &native[i] : NULL;
            if (!sets[i]) continue;
            FD_ZERO(&native[i]);
            memcpy(&native[i], rt_write_span(c, sets[i], bytes), bytes);
        }
        struct timeval tv, *wait = NULL; double start = now(), limit = 0;
        if (timeout) {
            rt_write_span(c, timeout, 8);
            int32_t seconds = (int32_t)R32(timeout), micro = (int32_t)R32(timeout + 4);
            if (seconds < 0 || micro < 0 || micro >= 1000000) { fail(c, 22); return 1; }
            tv.tv_sec = seconds; tv.tv_usec = micro; wait = &tv; limit = seconds + micro * 1e-6;
        }
        int result = RT_BLOCKING(select(count, pointers[0], pointers[1], pointers[2], wait));
        error = errno;
        if (timeout) { // Linux écrit le temps restant
            double left = limit - (now() - start);
            if (left < 0 || result == 0) left = 0;
            uint32_t seconds = (uint32_t)left, micro = (uint32_t)((left - seconds) * 1e6);
            if (micro > 999999) micro = 999999;
            W32(timeout, seconds); W32(timeout + 4, micro);
        }
        if (result >= 0) for (int i = 0; i < 3; ++i) if (sets[i]) memcpy(GPTR(sets[i]), &native[i], bytes);
        errno = error; finish(c, result);
    } else if (!strcmp(name, "poll")) {
        uint32_t fds = rt_arg32(c, 0), count = rt_arg32(c, 1); int timeout = (int32_t)rt_arg32(c, 2);
        if (count > 65536) { fail(c, 22); return 1; }
        struct pollfd *native = calloc(count ? count : 1, sizeof(*native));
        if (!native) { fail(c, 12); return 1; }
        rt_write_span(c, fds, (uint64_t)count * 8);
        for (uint32_t i = 0; i < count; ++i) {
            native[i].fd = (int32_t)R32(fds + 8 * i);
            native[i].events = host_poll_events(R16(fds + 8 * i + 4));
        }
        int result = RT_BLOCKING(poll(native, count, timeout));
        if (result >= 0) for (uint32_t i = 0; i < count; ++i) W16(fds + 8 * i + 6, linux_poll_events(native[i].revents));
        error = errno; free(native); errno = error;
        finish(c, result);
    } else if (!strcmp(name, "gethostbyname")) {
        uint32_t text = rt_arg32(c, 0); rt_string_length(c, text);
        c->eax = lookup_host(c, GPTR(text)) ? 0 : hostent_buffer;
    } else if (!strcmp(name, "getpeername")) {
        length = sizeof(address);
        int result = getpeername((int32_t)rt_arg32(c, 0), (struct sockaddr *)&address, &length);
        if (!result) guest_address(c, &address, length, rt_arg32(c, 1), rt_arg32(c, 2));
        finish(c, result);
    } else if (!strcmp(name, "getsockopt")) {
        finish(c, read_socket_option(c, (int32_t)rt_arg32(c, 0), (int32_t)rt_arg32(c, 1), (int32_t)rt_arg32(c, 2), rt_arg32(c, 3), rt_arg32(c, 4)));
    } else if (!strcmp(name, "shutdown")) { // SHUT_RD/WR/RDWR identiques
        finish(c, shutdown((int32_t)rt_arg32(c, 0), (int32_t)rt_arg32(c, 1)));
    } else if (!strcmp(name, "inet_pton")) {
        int family = (int32_t)rt_arg32(c, 0); uint32_t text = rt_arg32(c, 1), out = rt_arg32(c, 2);
        if (family != L_AF_INET && family != L_AF_INET6) { fail(c, 97); return 1; }
        rt_string_length(c, text);
        uint8_t bytes[16];
        int result = inet_pton(family == L_AF_INET ? AF_INET : AF_INET6, GPTR(text), bytes);
        if (result == 1) memcpy(rt_write_span(c, out, family == L_AF_INET ? 4 : 16), bytes, family == L_AF_INET ? 4 : 16);
        c->eax = (uint32_t)result;
    } else if (!strcmp(name, "inet_ntop")) {
        int family = (int32_t)rt_arg32(c, 0); uint32_t source = rt_arg32(c, 1), out = rt_arg32(c, 2), size = rt_arg32(c, 3);
        if (family != L_AF_INET && family != L_AF_INET6) { rt_set_errno(c, 97); c->eax = 0; return 1; }
        char text[INET6_ADDRSTRLEN];
        const void *bytes = rt_span(c, source, family == L_AF_INET ? 4 : 16);
        if (!inet_ntop(family == L_AF_INET ? AF_INET : AF_INET6, bytes, text, sizeof(text))) { rt_save_errno(c); c->eax = 0; return 1; }
        size_t used = strlen(text) + 1;
        if (used > size) { rt_set_errno(c, 28); c->eax = 0; return 1; } // ENOSPC
        memcpy(rt_write_span(c, out, used), text, used);
        c->eax = out;
    } else if (!strcmp(name, "inet_ntoa")) { // struct in_addr passée par valeur
        struct in_addr address = {rt_arg32(c, 0)};
        if (!ntoa_buffer && !(ntoa_buffer = rt_alloc(16))) { rt_set_errno(c, 12); c->eax = 0; return 1; }
        inet_ntop(AF_INET, &address, GPTR(ntoa_buffer), 16);
        c->eax = ntoa_buffer;
    } else if (!strcmp(name, "getaddrinfo")) {
        c->eax = (uint32_t)guest_addrinfo(c, rt_arg32(c, 0), rt_arg32(c, 1), rt_arg32(c, 2), rt_arg32(c, 3));
    } else if (!strcmp(name, "freeaddrinfo")) {
        free_guest_list(rt_arg32(c, 0), 28);
    } else if (!strcmp(name, "getifaddrs")) {
        finish(c, guest_ifaddrs(c, rt_arg32(c, 0)));
    } else if (!strcmp(name, "freeifaddrs")) {
        free_guest_list(rt_arg32(c, 0), 0);
    } else if (!strcmp(name, "inet_addr")) {
        uint32_t text = rt_arg32(c, 0); rt_string_length(c, text);
        c->eax = inet_addr(GPTR(text));
    } else return 0;
    return 1;
}
RT_SHIM_PROVIDER(net_shim)
