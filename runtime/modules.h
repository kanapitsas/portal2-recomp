#pragma once
#include "rt.h"
typedef struct { const char *name; uint32_t address; } GuestSymbol;
typedef struct {
    const char *name;
    uint32_t id, base, entry, init, fini;
    const uint32_t *constructors;
    size_t constructor_count;
    const GuestSymbol *symbols;
    size_t symbol_count;
    const char *const *needed;
    size_t needed_count;
} GuestModule;
extern const GuestModule rt_modules[];
extern const size_t rt_module_count;
const GuestModule *rt_find_module(const char *name);
uint32_t rt_module_symbol(const GuestModule *module, const char *name);
uint32_t rt_invoke(CPU *c, uint32_t address, const uint32_t *args, size_t count);
int rt_initialize_module(CPU *c, const GuestModule *module);
void rt_loader_set_arguments(uint32_t argc, uint32_t argv, uint32_t envp);
// Module destructors (DT_FINI_ARRAY reversed, then DT_FINI) of every
// initialised module, newest first, as ld.so does at exit. For exit().
void rt_finalize_modules(CPU *c);
