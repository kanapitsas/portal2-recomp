#include "rt.h"
#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>

uint8_t rt_vm_permissions[(RT_VM_END - RT_VM_BEGIN) / 4096] __attribute__((aligned(64)));
typedef struct Mapping { uint32_t address, size; int anonymous; struct Mapping *next; } Mapping;
static Mapping *mappings;
static uint32_t align_up(uint32_t value, uint32_t alignment) { return (value + alignment - 1) & ~(alignment - 1); }
static unsigned page_flags(int protection) {
    // On i386 a writable/executable page is also readable.
    return 8u | (unsigned)protection | (protection ? 1u : 0u);
}
static int update_host_permissions(uint32_t address, uint32_t size, int discard_empty) {
    uint64_t host_page = (uint64_t)sysconf(_SC_PAGESIZE);
    uint64_t lo = address & ~(host_page - 1);
    uint64_t hi = ((uint64_t)address + size + host_page - 1) & ~(host_page - 1);
    for (uint64_t base = lo; base < hi; base += host_page) {
        unsigned protection = 0;
        for (uint64_t p = base; p < base + host_page; p += 4096)
            protection |= rt_vm_permissions[(p - RT_VM_BEGIN) >> 12] & 3;
        if (!protection && discard_empty) {
            int any_mapping = 0;
            for (uint64_t p = base; p < base + host_page; p += 4096)
                any_mapping |= rt_vm_permissions[(p - RT_VM_BEGIN) >> 12] & 8;
            if (!any_mapping) {
                if (mmap(M + base, (size_t)host_page, PROT_NONE, MAP_FIXED | MAP_PRIVATE | MAP_ANON, -1, 0) == MAP_FAILED) return -1;
                continue;
            }
        }
        if (mprotect(M + base, (size_t)host_page, (int)protection)) return -1;
    }
    return 0;
}
uint32_t rt_mmap(CPU *c, uint32_t hint, uint32_t length, int protection, uint32_t flags, int fd, uint64_t offset) {
    (void)c;
    uint32_t page = (uint32_t)sysconf(_SC_PAGESIZE);
    if (!length || length > RT_VM_END - RT_VM_BEGIN || protection & ~7 || offset % page || !(flags & 3)) { errno = EINVAL; return UINT32_MAX; }
    uint32_t size = align_up(length, page), address = RT_VM_BEGIN;
    int fixed = (flags & 0x10) != 0, no_replace = (flags & 0x100000) != 0;
    if (fixed || no_replace) {
        if (hint < RT_VM_BEGIN || hint % page || (uint64_t)hint + size > RT_VM_END) { errno = EINVAL; return UINT32_MAX; }
        address = hint;
        if (fixed && rt_munmap(address, size)) return UINT32_MAX;
    }
    Mapping **link = &mappings;
    while (*link) {
        Mapping *m = *link;
        if ((uint64_t)address + size <= m->address) break;
        if (address < (uint64_t)m->address + m->size) {
            if (fixed || no_replace) { errno = EEXIST; return UINT32_MAX; }
            address = align_up(m->address + m->size, page);
        }
        link = &m->next;
    }
    if ((uint64_t)address + size > RT_VM_END) { errno = ENOMEM; return UINT32_MAX; }
    Mapping *m = calloc(1, sizeof(*m));
    if (!m) { errno = ENOMEM; return UINT32_MAX; }
    int native_flags = MAP_FIXED | ((flags & 1) ? MAP_SHARED : MAP_PRIVATE);
    if (flags & 0x20) { native_flags |= MAP_ANON; fd = -1; offset = 0; }
    int native_protection = (int)page_flags(protection) & 3;
    if (mmap(M + address, size, native_protection, native_flags, fd, (off_t)offset) == MAP_FAILED) { free(m); return UINT32_MAX; }
    m->address = address; m->size = align_up(length, 4096); m->anonymous = (flags & 0x20) != 0;
    m->next = *link; *link = m;
    memset(rt_vm_permissions + ((address - RT_VM_BEGIN) >> 12), (int)page_flags(protection), align_up(length, 4096) / 4096);
    return address;
}
int rt_munmap(uint32_t address, uint32_t length) {
    if (!length || address < RT_VM_BEGIN || address >= RT_VM_END || address % 4096 || length > RT_VM_END - address) { errno = EINVAL; return -1; }
    uint32_t size = align_up(length, 4096), end = address + size;
    if (end > RT_VM_END) { errno = EINVAL; return -1; }
    rt_pe_forget(address, size);   // greffon PE projeté ici : plus reconnu
    memset(rt_vm_permissions + ((address - RT_VM_BEGIN) >> 12), 0, size / 4096);
    if (update_host_permissions(address, size, 1)) return -1;
    Mapping **link = &mappings;
    while (*link) {
        Mapping *m = *link; uint32_t last = m->address + m->size;
        if (last <= address || m->address >= end) { link = &m->next; continue; }
        if (address <= m->address && end >= last) { *link = m->next; free(m); }
        else if (address <= m->address) { m->address = end; m->size = last - end; link = &m->next; }
        else if (end >= last) { m->size = address - m->address; link = &m->next; }
        else {
            Mapping *tail = calloc(1, sizeof(*tail));
            if (!tail) { errno = ENOMEM; return -1; }
            *tail = *m; tail->address = end; tail->size = last - end;
            m->size = address - m->address; m->next = tail; link = &tail->next;
        }
    }
    return 0;
}
int rt_mprotect(uint32_t address, uint32_t length, int protection) {
    uint64_t end = (uint64_t)address + length;
    if (end > (UINT64_C(1) << 32) || address % 4096 || protection & ~7) { errno = EINVAL; return -1; }
    if (!length) return 0;
    if (address >= RT_VM_BEGIN && end <= RT_VM_END) {
        uint32_t size = align_up(length, 4096);
        for (uint32_t p = address; p < address + size; p += 4096)
            if (!(rt_vm_permissions[(p - RT_VM_BEGIN) >> 12] & 8)) { errno = ENOMEM; return -1; }
        memset(rt_vm_permissions + ((address - RT_VM_BEGIN) >> 12), (int)page_flags(protection), size / 4096);
        return update_host_permissions(address, size, 0);
    }
    size_t page = (size_t)sysconf(_SC_PAGESIZE);
    if (address % page) { errno = EINVAL; return -1; }
    return mprotect(M + address, length, (int)page_flags(protection) & 3);
}
int rt_madvise(uint32_t address, uint32_t length, int advice) {
    if ((uint64_t)address + length > RT_VM_END || address < RT_VM_BEGIN || address % 4096) { errno = EINVAL; return -1; }
    if (!length) return 0;
    if (advice == 4) {
        uint32_t size = align_up(length, 4096);
        for (uint32_t p = address; p < address + size; p += 4096)
            if (!(rt_vm_permissions[(p - RT_VM_BEGIN) >> 12] & 8)) { errno = ENOMEM; return -1; }
        for (Mapping *m = mappings; m; m = m->next)
            if (address < m->address + m->size && (uint64_t)address + size > m->address && !m->anonymous) {
                errno = ENOTSUP; return -1;
            }
        uint64_t host_page = (uint64_t)sysconf(_SC_PAGESIZE);
        uint64_t lo = address & ~(host_page - 1), hi = ((uint64_t)address + size + host_page - 1) & ~(host_page - 1);
        if (mprotect(M + lo, (size_t)(hi - lo), PROT_READ | PROT_WRITE)) return -1;
        memset(M + address, 0, size);
        return update_host_permissions(address, size, 0);
    }
    int native;
    switch (advice) {
    case 0: native = MADV_NORMAL; break;
    case 1: native = MADV_RANDOM; break;
    case 2: native = MADV_SEQUENTIAL; break;
    case 3: native = MADV_WILLNEED; break;
    case 8: native = MADV_FREE; break;
    default: errno = EINVAL; return -1;
    }
    return madvise(GPTR(address), length, native);
}
void rt_vm_destroy(void) {
    while (mappings) { Mapping *next = mappings->next; free(mappings); mappings = next; }
    memset(rt_vm_permissions, 0, sizeof(rt_vm_permissions));
}

// Diagnostic : octets invités actuellement projetés par rt_mmap.
uint32_t rt_vm_mapped_bytes(void) {
    uint64_t total = 0;
    for (Mapping *m = mappings; m; m = m->next) total += m->size;
    return total > UINT32_MAX ? UINT32_MAX : (uint32_t)total;
}

// Diagnostic de faute (gestionnaire de signal) : droits de la page invitée.
// Ne lit que le tableau des droits ; la liste des projections peut être en
// cours de modification par le thread fautif.
int rt_vm_describe(uint32_t address, char *out, size_t size) {
    if (address < RT_VM_BEGIN || address >= RT_VM_END) return snprintf(out, size, "  outside the mmap area\n");
    unsigned flags = rt_vm_permissions[(address - RT_VM_BEGIN) >> 12];
    return snprintf(out, size, "  page flags %#x (%s)\n", flags, flags & 8 ? "mapped" : "not mapped");
}
