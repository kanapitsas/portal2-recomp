// Attributs de terminal : struct termios glibc i386 <-> Darwin.
//
// Disposition glibc i386 (60 octets) : c_iflag, c_oflag, c_cflag, c_lflag
// (u32), c_line (u8), c_cc[32] (u8), 3 octets de bourrage, c_ispeed et
// c_ospeed (u32, codes Bxxx de Linux, pas des bauds). Les bits des drapeaux
// et les indices de c_cc diffèrent de Darwin : tout passe par les tables
// ci-dessous. tcgetattr ne rapporte que les bits que Linux sait nommer ;
// tcsetattr part de l'état hôte courant, ne touche que les bits traduits et
// refuse (trap) un bit Linux sans équivalent Darwin plutôt que de l'ignorer.
// engine.so s'en sert pour la console texte quand stdin est un terminal.
//
// SHIMS: tcgetattr tcsetattr
#include "rt.h"
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

enum { LINUX_NCCS = 32, LINUX_TERMIOS = 60, LINUX_CC = 17, LINUX_ISPEED = 52, LINUX_OSPEED = 56 };

typedef struct { uint32_t linux_bit; tcflag_t host_bit; } Bit;

static const Bit iflags[] = {
    {01, IGNBRK}, {02, BRKINT}, {04, IGNPAR}, {010, PARMRK}, {020, INPCK}, {040, ISTRIP},
    {0100, INLCR}, {0200, IGNCR}, {0400, ICRNL}, {02000, IXON}, {04000, IXANY},
    {010000, IXOFF}, {020000, IMAXBEL}, {040000, IUTF8},
};   // IUCLC (01000) : pas d'équivalent
static const Bit oflags[] = {
    {01, OPOST}, {04, ONLCR}, {010, OCRNL}, {020, ONOCR}, {040, ONLRET},
};   // OLCUC, OFILL, OFDEL et délais : pas traduits
static const Bit cflags[] = {
    {0100, CSTOPB}, {0200, CREAD}, {0400, PARENB}, {01000, PARODD}, {02000, HUPCL},
    {04000, CLOCAL}, {020000000000u, CRTSCTS},
};   // CSIZE et CBAUD : traités à part
static const Bit lflags[] = {
    {01, ISIG}, {02, ICANON}, {010, ECHO}, {020, ECHOE}, {040, ECHOK}, {0100, ECHONL},
    {0200, NOFLSH}, {0400, TOSTOP}, {01000, ECHOCTL}, {02000, ECHOPRT}, {04000, ECHOKE},
    {010000, FLUSHO}, {040000, PENDIN}, {0100000, IEXTEN}, {0200000, EXTPROC},
};   // XCASE (04) : pas d'équivalent

// Indices c_cc Linux -> Darwin (-1 : VSWTC, absent de Darwin).
static const int cc_index[] = {
    VINTR, VQUIT, VERASE, VKILL, VEOF, VTIME, VMIN, -1, VSTART, VSTOP, VSUSP, VEOL,
    VREPRINT, VDISCARD, VWERASE, VLNEXT, VEOL2,
};
enum { LINUX_CC_USED = sizeof(cc_index) / sizeof(cc_index[0]) };

#define LINUX_CSIZE 060u
#define LINUX_CBAUD 010017u
static const struct { uint32_t code; speed_t baud; } speeds[] = {
    {0, 0}, {1, 50}, {2, 75}, {3, 110}, {4, 134}, {5, 150}, {6, 200}, {7, 300}, {010, 600},
    {011, 1200}, {012, 1800}, {013, 2400}, {014, 4800}, {015, 9600}, {016, 19200},
    {017, 38400}, {010001, 57600}, {010002, 115200}, {010003, 230400},
};

static uint32_t to_linux(tcflag_t host, const Bit *table, size_t count) {
    uint32_t out = 0;
    for (size_t i = 0; i < count; ++i) if (host & table[i].host_bit) out |= table[i].linux_bit;
    return out;
}
// Renvoie 0 si un bit Linux n'a pas d'équivalent.
static int to_host(uint32_t guest, tcflag_t *host, const Bit *table, size_t count) {
    uint32_t known = 0;
    for (size_t i = 0; i < count; ++i) {
        known |= table[i].linux_bit;
        if (guest & table[i].linux_bit) *host |= table[i].host_bit; else *host &= ~table[i].host_bit;
    }
    return !(guest & ~known);
}
static uint32_t speed_code(speed_t baud) {
    for (size_t i = 0; i < sizeof(speeds) / sizeof(speeds[0]); ++i) if (speeds[i].baud == baud) return speeds[i].code;
    return 017;   // vitesse hors table (pseudo-terminal) : B38400, la valeur courante sous Linux
}
static int speed_baud(uint32_t code, speed_t *baud) {
    for (size_t i = 0; i < sizeof(speeds) / sizeof(speeds[0]); ++i) if (speeds[i].code == code) { *baud = speeds[i].baud; return 1; }
    return 0;
}

#define COUNT(t) (sizeof(t) / sizeof((t)[0]))

static void termios_to_linux(const struct termios *t, uint8_t out[LINUX_TERMIOS]) {
    memset(out, 0, LINUX_TERMIOS);
    uint32_t speed = speed_code(cfgetospeed(t));
    static const uint32_t sizes[] = {0, 020, 040, 060};
    uint32_t cflag = to_linux(t->c_cflag, cflags, COUNT(cflags)) | speed;
    switch (t->c_cflag & CSIZE) {
    case CS6: cflag |= sizes[1]; break;
    case CS7: cflag |= sizes[2]; break;
    case CS8: cflag |= sizes[3]; break;
    default: break;
    }
    uint32_t words[4] = { to_linux(t->c_iflag, iflags, COUNT(iflags)), to_linux(t->c_oflag, oflags, COUNT(oflags)),
                          cflag, to_linux(t->c_lflag, lflags, COUNT(lflags)) };
    memcpy(out, words, sizeof(words));
    for (int i = 0; i < LINUX_CC_USED; ++i) {
        if (cc_index[i] < 0) continue;
        cc_t value = t->c_cc[cc_index[i]];
        // _POSIX_VDISABLE : 0xff sous Darwin, 0 sous Linux (VMIN/VTIME sont des nombres).
        if (value == _POSIX_VDISABLE && cc_index[i] != VMIN && cc_index[i] != VTIME) value = 0;
        out[LINUX_CC + i] = value;
    }
    uint32_t ispeed = speed_code(cfgetispeed(t)), ospeed = speed;
    memcpy(out + LINUX_ISPEED, &ispeed, 4);
    memcpy(out + LINUX_OSPEED, &ospeed, 4);
}

// 0 : traduit ; sinon, message du bit qui n'a pas d'équivalent Darwin.
static const char *termios_from_linux(const uint8_t in[LINUX_TERMIOS], struct termios *t) {
    uint32_t words[4]; memcpy(words, in, sizeof(words));
    if (!to_host(words[0], &t->c_iflag, iflags, COUNT(iflags))) return "termios c_iflag bit without a Darwin equivalent";
    if (!to_host(words[1], &t->c_oflag, oflags, COUNT(oflags))) return "termios c_oflag bit without a Darwin equivalent";
    if (!to_host(words[2] & ~(LINUX_CSIZE | LINUX_CBAUD), &t->c_cflag, cflags, COUNT(cflags)))
        return "termios c_cflag bit without a Darwin equivalent";
    static const tcflag_t sizes[] = {CS5, CS6, CS7, CS8};
    t->c_cflag = (t->c_cflag & ~CSIZE) | sizes[(words[2] & LINUX_CSIZE) >> 4];
    if (!to_host(words[3], &t->c_lflag, lflags, COUNT(lflags))) return "termios c_lflag bit without a Darwin equivalent";
    for (int i = 0; i < LINUX_CC_USED; ++i) {
        if (cc_index[i] < 0) continue;
        cc_t value = in[LINUX_CC + i];
        if (value == 0 && cc_index[i] != VMIN && cc_index[i] != VTIME) value = _POSIX_VDISABLE;
        t->c_cc[cc_index[i]] = value;
    }
    // glibc garde la vitesse dans c_ispeed/c_ospeed ; CBAUD de c_cflag la double.
    uint32_t ispeed, ospeed; memcpy(&ispeed, in + LINUX_ISPEED, 4); memcpy(&ospeed, in + LINUX_OSPEED, 4);
    speed_t in_baud, out_baud;
    if (!speed_baud(ispeed, &in_baud) || !speed_baud(ospeed, &out_baud)) return "termios speed code not translated";
    // Un pseudo-terminal ignore la vitesse : on ne la change que si elle diffère du code rapporté.
    if (ispeed != speed_code(cfgetispeed(t))) cfsetispeed(t, in_baud);
    if (ospeed != speed_code(cfgetospeed(t))) cfsetospeed(t, out_baud);
    return NULL;
}

static int termios_shim(CPU *c, const char *name) {
    int get = !strcmp(name, "tcgetattr");
    if (!get && strcmp(name, "tcsetattr")) return 0;
    int fd = (int32_t)rt_arg32(c, 0);
    uint32_t guest = rt_arg32(c, get ? 1 : 2);
    struct termios host;
    if (tcgetattr(fd, &host)) { rt_save_errno(c); c->eax = UINT32_MAX; return 1; }
    if (get) {
        uint8_t image[LINUX_TERMIOS];
        termios_to_linux(&host, image);
        memcpy(rt_write_span(c, guest, LINUX_TERMIOS), image, LINUX_TERMIOS);
        c->eax = 0; return 1;
    }
    int action = (int32_t)rt_arg32(c, 1);   // TCSANOW 0, TCSADRAIN 1, TCSAFLUSH 2 : mêmes valeurs
    if (action < 0 || action > 2) { rt_set_errno(c, RT_EINVAL); c->eax = UINT32_MAX; return 1; }
    const char *problem = termios_from_linux(rt_span(c, guest, LINUX_TERMIOS), &host);
    if (problem) rt_trap(c, c->eip, problem);
    if (tcsetattr(fd, action == 0 ? TCSANOW : action == 1 ? TCSADRAIN : TCSAFLUSH, &host)) {
        rt_save_errno(c); c->eax = UINT32_MAX; return 1;
    }
    c->eax = 0; return 1;
}
RT_SHIM_PROVIDER(termios_shim)
