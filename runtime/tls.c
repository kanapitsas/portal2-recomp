#include "rt.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct TLSModule {
    uint32_t id, size, alignment;
    uint8_t *initial;
    struct TLSModule *next;
} TLSModule;
typedef struct TLSBlock {
    uint32_t thread, module, address, allocation;
    struct TLSBlock *next;
} TLSBlock;
typedef struct ThreadContext { uint32_t thread, pointer; struct ThreadContext *next; } ThreadContext;
static ThreadContext *contexts;
static TLSModule *modules;
static TLSBlock *blocks;

int rt_register_tls(const char *directory, const char *name, uint32_t id,
                    uint32_t offset, uint32_t filesz, uint32_t memsz, uint32_t alignment) {
    if (!id || !memsz || filesz > memsz || !alignment || (alignment & (alignment - 1))) return -1;
    for (TLSModule *m = modules; m; m = m->next) if (m->id == id) return -1;
    TLSModule *m = calloc(1, sizeof(*m));
    if (!m) return -1;
    m->initial = calloc(1, memsz);
    if (!m->initial) { free(m); return -1; }
    if (filesz) {
        char path[4096];
        int n = snprintf(path, sizeof(path), "%s/%s", directory, name);
        if (n < 0 || (size_t)n >= sizeof(path)) { free(m->initial); free(m); return -1; }
        FILE *file = fopen(path, "rb");
        int failed = !file;
        if (file) {
            failed = fseek(file, (long)offset, SEEK_SET) || fread(m->initial, 1, filesz, file) != filesz;
            fclose(file);
        }
        if (failed) { free(m->initial); free(m); return -1; }
    }
    m->id = id; m->size = memsz; m->alignment = alignment;
    m->next = modules; modules = m;
    return 0;
}

uint32_t rt_thread_pointer(CPU *c) {
    if (c->gs_base) return c->gs_base;
    if (!c->tid) c->tid = 1;
    for (ThreadContext *ctx = contexts; ctx; ctx = ctx->next)
        if (ctx->thread == c->tid) { c->gs_base = ctx->pointer; return c->gs_base; }
    uint32_t allocation = rt_alloc(0x11000);
    ThreadContext *ctx = calloc(1, sizeof(*ctx));
    if (!allocation || !ctx) rt_trap(c, c->eip, "cannot allocate guest thread control block");
    memset(GPTR(allocation), 0, 0x11000);
    ctx->thread = c->tid; ctx->pointer = allocation + 0x10000;
    W32(ctx->pointer, ctx->pointer); W32(ctx->pointer + 8, ctx->pointer);
    W32(ctx->pointer + 20, arc4random() & 0xffffff00u);
    W32(ctx->pointer + 24, arc4random());
    ctx->next = contexts; contexts = ctx;
    c->gs_base = ctx->pointer;
    return c->gs_base;
}

uint32_t rt_tls_address(CPU *c, uint32_t module, uint32_t offset) {
    if (!c->tid) c->tid = 1;
    TLSModule *m = modules;
    while (m && m->id != module) m = m->next;
    if (!m || offset >= m->size) rt_trap(c, c->eip, "unknown module or offset in guest TLS descriptor");
    for (TLSBlock *b = blocks; b; b = b->next)
        if (b->thread == c->tid && b->module == module) return b->address + offset;
    uint64_t size = (uint64_t)m->size + m->alignment - 1;
    if (size > UINT32_MAX) rt_trap(c, c->eip, "guest TLS allocation overflow");
    uint32_t allocation = rt_alloc((uint32_t)size);
    TLSBlock *b = calloc(1, sizeof(*b));
    if (!allocation || !b) rt_trap(c, c->eip, "cannot allocate guest TLS block");
    b->address = (allocation + m->alignment - 1) & ~(m->alignment - 1);
    memcpy(GPTR(b->address), m->initial, m->size);
    b->allocation = allocation; b->module = module; b->thread = c->tid; b->next = blocks; blocks = b;
    return b->address + offset;
}

// Frees the TCB and TLS blocks of a guest thread whose guest code is over.
void rt_tls_release(uint32_t thread) {
    for (ThreadContext **link = &contexts; *link;) {
        ThreadContext *ctx = *link;
        if (ctx->thread != thread) { link = &ctx->next; continue; }
        *link = ctx->next; rt_free(ctx->pointer - 0x10000); free(ctx);
    }
    for (TLSBlock **link = &blocks; *link;) {
        TLSBlock *b = *link;
        if (b->thread != thread) { link = &b->next; continue; }
        *link = b->next; rt_free(b->allocation); free(b);
    }
}

void rt_tls_destroy(void) {
    while (contexts) { ThreadContext *next = contexts->next; free(contexts); contexts = next; }
    while (blocks) { TLSBlock *next = blocks->next; free(blocks); blocks = next; }
    while (modules) { TLSModule *next = modules->next; free(modules->initial); free(modules); modules = next; }
}
