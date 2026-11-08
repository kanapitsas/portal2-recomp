// Pont OpenGL : aides partagées par runtime/gl.c (écrit à la main) et
// runtime/gl_generated.c (tools/gen_gl.py) : validation et traduction des
// pointeurs invités, table des fonctions pontées. Les arguments scalaires sont
// lus avec rt_arg32/rt_arg64/rt_arg_f32/rt_arg_f64 (rt.h).
#pragma once
#include "rt.h"
#include <stdint.h>

typedef int (*rt_gl_thunk)(CPU *c, void *fn);
typedef struct { const char *name; rt_gl_thunk thunk; } RtGlEntry;
extern const RtGlEntry rt_gl_entries[];   // triées par nom (bsearch)
extern const unsigned rt_gl_entry_count;

uint64_t rt_gl_n(int64_t n);                          // GLsizei négatif → 0
uint32_t rt_gl_get_count(uint32_t pname);             // valeurs écrites par glGet*v
uint32_t rt_gl_texparam_count(uint32_t pname);
void *rt_gl_in(CPU *c, uint32_t ptr, uint64_t size, int nullable);
void *rt_gl_out(CPU *c, uint32_t ptr, uint64_t size, int nullable);
const void *rt_gl_array(CPU *c, uint32_t ptr);        // tableau de sommets : décalage si VBO lié
const void *rt_gl_indices(CPU *c, uint32_t ptr, int32_t count, uint32_t type);
const void *rt_gl_unpack(CPU *c, uint32_t ptr, int32_t w, int32_t h, int32_t d, uint32_t format, uint32_t type, int nullable);
const void *rt_gl_unpack_compressed(CPU *c, uint32_t ptr, int32_t size);
void *rt_gl_pack_tex(CPU *c, uint32_t ptr, uint32_t target, int32_t level, uint32_t format, uint32_t type);
void *rt_gl_pack_compressed(CPU *c, uint32_t ptr, uint32_t target, int32_t level);
void *rt_gl_handle_arg(uint32_t handle);              // GLhandleARB (void * sur macOS)
uint32_t rt_gl_handle_ret(CPU *c, void *handle);
uint32_t rt_gl_iptr_ret(CPU *c, intptr_t value);
