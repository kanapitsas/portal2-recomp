// GÉNÉRÉ par tools/gen_gl.py — ne pas éditer à la main.
// Thunks i386 cdecl → OpenGL hôte pour les fonctions demandées par libtogl.
// Les aides rt_gl_* (validation et traduction des pointeurs invités) sont
// déclarées dans runtime/gl_bridge.h et définies dans runtime/gl.c.
#include "gl_bridge.h"

int rt_gl_manual_glBufferParameteriAPPLE(CPU *c, void *fn);
int rt_gl_manual_glClientWaitSync(CPU *c, void *fn);
int rt_gl_manual_glDeleteBuffersARB(CPU *c, void *fn);
int rt_gl_manual_glDeleteSync(CPU *c, void *fn);
int rt_gl_manual_glFenceSync(CPU *c, void *fn);
int rt_gl_manual_glFlushMappedBufferRange(CPU *c, void *fn);
int rt_gl_manual_glFlushMappedBufferRangeAPPLE(CPU *c, void *fn);
int rt_gl_manual_glGetString(CPU *c, void *fn);
int rt_gl_manual_glGetSynciv(CPU *c, void *fn);
int rt_gl_manual_glGetTexParameterPointervAPPLE(CPU *c, void *fn);
int rt_gl_manual_glMapBufferARB(CPU *c, void *fn);
int rt_gl_manual_glMapBufferRange(CPU *c, void *fn);
int rt_gl_manual_glShaderSourceARB(CPU *c, void *fn);
int rt_gl_manual_glUnmapBuffer(CPU *c, void *fn);
int rt_gl_manual_glWaitSync(CPU *c, void *fn);

static int g_glActiveTexture(CPU *c, void *fn) {
    uint32_t a_texture = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_texture);
    return 1;
}

static int g_glAlphaFunc(CPU *c, void *fn) {
    uint32_t a_func = rt_arg32(c, 0);
    float a_ref = rt_arg_f32(c, 4);
    ((void (*)(uint32_t, float))fn)(a_func, a_ref);
    return 1;
}

static int g_glAttachObjectARB(CPU *c, void *fn) {
    void * a_containerObj = rt_gl_handle_arg(rt_arg32(c, 0));
    void * a_obj = rt_gl_handle_arg(rt_arg32(c, 1));
    ((void (*)(void *, void *))fn)(a_containerObj, a_obj);
    return 1;
}

static int g_glBegin(CPU *c, void *fn) {
    uint32_t a_mode = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_mode);
    return 1;
}

static int g_glBeginQuery(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    uint32_t a_id = rt_arg32(c, 1);
    ((void (*)(uint32_t, uint32_t))fn)(a_target, a_id);
    return 1;
}

static int g_glBeginQueryARB(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    uint32_t a_id = rt_arg32(c, 1);
    ((void (*)(uint32_t, uint32_t))fn)(a_target, a_id);
    return 1;
}

static int g_glBindAttribLocationARB(CPU *c, void *fn) {
    void * a_programObj = rt_gl_handle_arg(rt_arg32(c, 0));
    uint32_t a_index = rt_arg32(c, 1);
    void *a_name = (void *)rt_guest_cstr(c, rt_arg32(c, 2));
    ((void (*)(void *, uint32_t, void *))fn)(a_programObj, a_index, a_name);
    return 1;
}

static int g_glBindBufferARB(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    uint32_t a_buffer = rt_arg32(c, 1);
    ((void (*)(uint32_t, uint32_t))fn)(a_target, a_buffer);
    return 1;
}

static int g_glBindFramebuffer(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    uint32_t a_framebuffer = rt_arg32(c, 1);
    ((void (*)(uint32_t, uint32_t))fn)(a_target, a_framebuffer);
    return 1;
}

static int g_glBindFramebufferEXT(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    uint32_t a_framebuffer = rt_arg32(c, 1);
    ((void (*)(uint32_t, uint32_t))fn)(a_target, a_framebuffer);
    return 1;
}

static int g_glBindProgramARB(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    uint32_t a_program = rt_arg32(c, 1);
    ((void (*)(uint32_t, uint32_t))fn)(a_target, a_program);
    return 1;
}

static int g_glBindRenderbuffer(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    uint32_t a_renderbuffer = rt_arg32(c, 1);
    ((void (*)(uint32_t, uint32_t))fn)(a_target, a_renderbuffer);
    return 1;
}

static int g_glBindRenderbufferEXT(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    uint32_t a_renderbuffer = rt_arg32(c, 1);
    ((void (*)(uint32_t, uint32_t))fn)(a_target, a_renderbuffer);
    return 1;
}

static int g_glBindSampler(CPU *c, void *fn) {
    uint32_t a_unit = rt_arg32(c, 0);
    uint32_t a_sampler = rt_arg32(c, 1);
    ((void (*)(uint32_t, uint32_t))fn)(a_unit, a_sampler);
    return 1;
}

static int g_glBindTexture(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    uint32_t a_texture = rt_arg32(c, 1);
    ((void (*)(uint32_t, uint32_t))fn)(a_target, a_texture);
    return 1;
}

static int g_glBindVertexArray(CPU *c, void *fn) {
    uint32_t a_array = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_array);
    return 1;
}

static int g_glBlendColor(CPU *c, void *fn) {
    float a_red = rt_arg_f32(c, 0);
    float a_green = rt_arg_f32(c, 4);
    float a_blue = rt_arg_f32(c, 8);
    float a_alpha = rt_arg_f32(c, 12);
    ((void (*)(float, float, float, float))fn)(a_red, a_green, a_blue, a_alpha);
    return 1;
}

static int g_glBlendEquation(CPU *c, void *fn) {
    uint32_t a_mode = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_mode);
    return 1;
}

static int g_glBlendFunc(CPU *c, void *fn) {
    uint32_t a_sfactor = rt_arg32(c, 0);
    uint32_t a_dfactor = rt_arg32(c, 1);
    ((void (*)(uint32_t, uint32_t))fn)(a_sfactor, a_dfactor);
    return 1;
}

static int g_glBlitFramebuffer(CPU *c, void *fn) {
    int32_t a_srcX0 = (int32_t)rt_arg32(c, 0);
    int32_t a_srcY0 = (int32_t)rt_arg32(c, 1);
    int32_t a_srcX1 = (int32_t)rt_arg32(c, 2);
    int32_t a_srcY1 = (int32_t)rt_arg32(c, 3);
    int32_t a_dstX0 = (int32_t)rt_arg32(c, 4);
    int32_t a_dstY0 = (int32_t)rt_arg32(c, 5);
    int32_t a_dstX1 = (int32_t)rt_arg32(c, 6);
    int32_t a_dstY1 = (int32_t)rt_arg32(c, 7);
    uint32_t a_mask = rt_arg32(c, 8);
    uint32_t a_filter = rt_arg32(c, 9);
    ((void (*)(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t))fn)(a_srcX0, a_srcY0, a_srcX1, a_srcY1, a_dstX0, a_dstY0, a_dstX1, a_dstY1, a_mask, a_filter);
    return 1;
}

static int g_glBlitFramebufferEXT(CPU *c, void *fn) {
    int32_t a_srcX0 = (int32_t)rt_arg32(c, 0);
    int32_t a_srcY0 = (int32_t)rt_arg32(c, 1);
    int32_t a_srcX1 = (int32_t)rt_arg32(c, 2);
    int32_t a_srcY1 = (int32_t)rt_arg32(c, 3);
    int32_t a_dstX0 = (int32_t)rt_arg32(c, 4);
    int32_t a_dstY0 = (int32_t)rt_arg32(c, 5);
    int32_t a_dstX1 = (int32_t)rt_arg32(c, 6);
    int32_t a_dstY1 = (int32_t)rt_arg32(c, 7);
    uint32_t a_mask = rt_arg32(c, 8);
    uint32_t a_filter = rt_arg32(c, 9);
    ((void (*)(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t))fn)(a_srcX0, a_srcY0, a_srcX1, a_srcY1, a_dstX0, a_dstY0, a_dstX1, a_dstY1, a_mask, a_filter);
    return 1;
}

static int g_glBufferDataARB(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    intptr_t a_size = (intptr_t)(int32_t)rt_arg32(c, 1);
    uint32_t a_usage = rt_arg32(c, 3);
    void *a_data = (void *)rt_gl_in(c, rt_arg32(c, 2), (uint64_t)(rt_gl_n(a_size)), 1);
    ((void (*)(uint32_t, intptr_t, void *, uint32_t))fn)(a_target, a_size, a_data, a_usage);
    return 1;
}

static int g_glBufferSubData(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    intptr_t a_offset = (intptr_t)(int32_t)rt_arg32(c, 1);
    intptr_t a_size = (intptr_t)(int32_t)rt_arg32(c, 2);
    void *a_data = (void *)rt_gl_in(c, rt_arg32(c, 3), (uint64_t)(rt_gl_n(a_size)), 0);
    ((void (*)(uint32_t, intptr_t, intptr_t, void *))fn)(a_target, a_offset, a_size, a_data);
    return 1;
}

static int g_glCheckFramebufferStatus(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    c->eax = (uint32_t)((uint32_t (*)(uint32_t))fn)(a_target);
    return 1;
}

static int g_glCheckFramebufferStatusEXT(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    c->eax = (uint32_t)((uint32_t (*)(uint32_t))fn)(a_target);
    return 1;
}

static int g_glClear(CPU *c, void *fn) {
    uint32_t a_mask = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_mask);
    return 1;
}

static int g_glClearColor(CPU *c, void *fn) {
    float a_red = rt_arg_f32(c, 0);
    float a_green = rt_arg_f32(c, 4);
    float a_blue = rt_arg_f32(c, 8);
    float a_alpha = rt_arg_f32(c, 12);
    ((void (*)(float, float, float, float))fn)(a_red, a_green, a_blue, a_alpha);
    return 1;
}

static int g_glClearDepth(CPU *c, void *fn) {
    double a_depth = rt_arg_f64(c, 0);
    ((void (*)(double))fn)(a_depth);
    return 1;
}

static int g_glClearStencil(CPU *c, void *fn) {
    int32_t a_s = (int32_t)rt_arg32(c, 0);
    ((void (*)(int32_t))fn)(a_s);
    return 1;
}

static int g_glClientActiveTexture(CPU *c, void *fn) {
    uint32_t a_texture = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_texture);
    return 1;
}

static int g_glClipPlane(CPU *c, void *fn) {
    uint32_t a_plane = rt_arg32(c, 0);
    void *a_equation = (void *)rt_gl_in(c, rt_arg32(c, 1), (uint64_t)(32), 0);
    ((void (*)(uint32_t, void *))fn)(a_plane, a_equation);
    return 1;
}

static int g_glColor4f(CPU *c, void *fn) {
    float a_red = rt_arg_f32(c, 0);
    float a_green = rt_arg_f32(c, 4);
    float a_blue = rt_arg_f32(c, 8);
    float a_alpha = rt_arg_f32(c, 12);
    ((void (*)(float, float, float, float))fn)(a_red, a_green, a_blue, a_alpha);
    return 1;
}

static int g_glColor4sv(CPU *c, void *fn) {
    void *a_v = (void *)rt_gl_in(c, rt_arg32(c, 0), (uint64_t)(8), 0);
    ((void (*)(void *))fn)(a_v);
    return 1;
}

static int g_glColorMask(CPU *c, void *fn) {
    uint8_t a_red = (uint8_t)rt_arg32(c, 0);
    uint8_t a_green = (uint8_t)rt_arg32(c, 1);
    uint8_t a_blue = (uint8_t)rt_arg32(c, 2);
    uint8_t a_alpha = (uint8_t)rt_arg32(c, 3);
    ((void (*)(uint8_t, uint8_t, uint8_t, uint8_t))fn)(a_red, a_green, a_blue, a_alpha);
    return 1;
}

static int g_glColorMaskIndexedEXT(CPU *c, void *fn) {
    uint32_t a_index = rt_arg32(c, 0);
    uint8_t a_r = (uint8_t)rt_arg32(c, 1);
    uint8_t a_g = (uint8_t)rt_arg32(c, 2);
    uint8_t a_b = (uint8_t)rt_arg32(c, 3);
    uint8_t a_a = (uint8_t)rt_arg32(c, 4);
    ((void (*)(uint32_t, uint8_t, uint8_t, uint8_t, uint8_t))fn)(a_index, a_r, a_g, a_b, a_a);
    return 1;
}

static int g_glCompileShaderARB(CPU *c, void *fn) {
    void * a_shaderObj = rt_gl_handle_arg(rt_arg32(c, 0));
    ((void (*)(void *))fn)(a_shaderObj);
    return 1;
}

static int g_glCompressedTexImage2D(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    int32_t a_level = (int32_t)rt_arg32(c, 1);
    uint32_t a_internalformat = rt_arg32(c, 2);
    int32_t a_width = (int32_t)rt_arg32(c, 3);
    int32_t a_height = (int32_t)rt_arg32(c, 4);
    int32_t a_border = (int32_t)rt_arg32(c, 5);
    int32_t a_imageSize = (int32_t)rt_arg32(c, 6);
    void *a_data = (void *)rt_gl_unpack_compressed(c, rt_arg32(c, 7), a_imageSize);
    ((void (*)(uint32_t, int32_t, uint32_t, int32_t, int32_t, int32_t, int32_t, void *))fn)(a_target, a_level, a_internalformat, a_width, a_height, a_border, a_imageSize, a_data);
    return 1;
}

static int g_glCompressedTexImage3D(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    int32_t a_level = (int32_t)rt_arg32(c, 1);
    uint32_t a_internalformat = rt_arg32(c, 2);
    int32_t a_width = (int32_t)rt_arg32(c, 3);
    int32_t a_height = (int32_t)rt_arg32(c, 4);
    int32_t a_depth = (int32_t)rt_arg32(c, 5);
    int32_t a_border = (int32_t)rt_arg32(c, 6);
    int32_t a_imageSize = (int32_t)rt_arg32(c, 7);
    void *a_data = (void *)rt_gl_unpack_compressed(c, rt_arg32(c, 8), a_imageSize);
    ((void (*)(uint32_t, int32_t, uint32_t, int32_t, int32_t, int32_t, int32_t, int32_t, void *))fn)(a_target, a_level, a_internalformat, a_width, a_height, a_depth, a_border, a_imageSize, a_data);
    return 1;
}

static int g_glCopyBufferSubData(CPU *c, void *fn) {
    uint32_t a_readTarget = rt_arg32(c, 0);
    uint32_t a_writeTarget = rt_arg32(c, 1);
    intptr_t a_readOffset = (intptr_t)(int32_t)rt_arg32(c, 2);
    intptr_t a_writeOffset = (intptr_t)(int32_t)rt_arg32(c, 3);
    intptr_t a_size = (intptr_t)(int32_t)rt_arg32(c, 4);
    ((void (*)(uint32_t, uint32_t, intptr_t, intptr_t, intptr_t))fn)(a_readTarget, a_writeTarget, a_readOffset, a_writeOffset, a_size);
    return 1;
}

static int g_glCreateProgramObjectARB(CPU *c, void *fn) {
    c->eax = rt_gl_handle_ret(c, ((void * (*)(void))fn)());
    return 1;
}

static int g_glCreateShaderObjectARB(CPU *c, void *fn) {
    uint32_t a_shaderType = rt_arg32(c, 0);
    c->eax = rt_gl_handle_ret(c, ((void * (*)(uint32_t))fn)(a_shaderType));
    return 1;
}

static int g_glDeleteFencesAPPLE(CPU *c, void *fn) {
    int32_t a_n = (int32_t)rt_arg32(c, 0);
    void *a_fences = (void *)rt_gl_in(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_n)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_n, a_fences);
    return 1;
}

static int g_glDeleteFramebuffers(CPU *c, void *fn) {
    int32_t a_n = (int32_t)rt_arg32(c, 0);
    void *a_framebuffers = (void *)rt_gl_in(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_n)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_n, a_framebuffers);
    return 1;
}

static int g_glDeleteFramebuffersEXT(CPU *c, void *fn) {
    int32_t a_n = (int32_t)rt_arg32(c, 0);
    void *a_framebuffers = (void *)rt_gl_in(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_n)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_n, a_framebuffers);
    return 1;
}

static int g_glDeleteObjectARB(CPU *c, void *fn) {
    void * a_obj = rt_gl_handle_arg(rt_arg32(c, 0));
    ((void (*)(void *))fn)(a_obj);
    return 1;
}

static int g_glDeleteProgramsARB(CPU *c, void *fn) {
    int32_t a_n = (int32_t)rt_arg32(c, 0);
    void *a_programs = (void *)rt_gl_in(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_n)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_n, a_programs);
    return 1;
}

static int g_glDeleteQueries(CPU *c, void *fn) {
    int32_t a_n = (int32_t)rt_arg32(c, 0);
    void *a_ids = (void *)rt_gl_in(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_n)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_n, a_ids);
    return 1;
}

static int g_glDeleteQueriesARB(CPU *c, void *fn) {
    int32_t a_n = (int32_t)rt_arg32(c, 0);
    void *a_ids = (void *)rt_gl_in(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_n)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_n, a_ids);
    return 1;
}

static int g_glDeleteRenderbuffers(CPU *c, void *fn) {
    int32_t a_n = (int32_t)rt_arg32(c, 0);
    void *a_renderbuffers = (void *)rt_gl_in(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_n)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_n, a_renderbuffers);
    return 1;
}

static int g_glDeleteRenderbuffersEXT(CPU *c, void *fn) {
    int32_t a_n = (int32_t)rt_arg32(c, 0);
    void *a_renderbuffers = (void *)rt_gl_in(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_n)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_n, a_renderbuffers);
    return 1;
}

static int g_glDeleteSamplers(CPU *c, void *fn) {
    int32_t a_count = (int32_t)rt_arg32(c, 0);
    void *a_samplers = (void *)rt_gl_in(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_count)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_count, a_samplers);
    return 1;
}

static int g_glDeleteShader(CPU *c, void *fn) {
    uint32_t a_shader = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_shader);
    return 1;
}

static int g_glDeleteTextures(CPU *c, void *fn) {
    int32_t a_n = (int32_t)rt_arg32(c, 0);
    void *a_textures = (void *)rt_gl_in(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_n)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_n, a_textures);
    return 1;
}

static int g_glDeleteVertexArrays(CPU *c, void *fn) {
    int32_t a_n = (int32_t)rt_arg32(c, 0);
    void *a_arrays = (void *)rt_gl_in(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_n)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_n, a_arrays);
    return 1;
}

static int g_glDepthFunc(CPU *c, void *fn) {
    uint32_t a_func = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_func);
    return 1;
}

static int g_glDepthMask(CPU *c, void *fn) {
    uint8_t a_flag = (uint8_t)rt_arg32(c, 0);
    ((void (*)(uint8_t))fn)(a_flag);
    return 1;
}

static int g_glDepthRange(CPU *c, void *fn) {
    double a_zNear = rt_arg_f64(c, 0);
    double a_zFar = rt_arg_f64(c, 8);
    ((void (*)(double, double))fn)(a_zNear, a_zFar);
    return 1;
}

static int g_glDetachObjectARB(CPU *c, void *fn) {
    void * a_containerObj = rt_gl_handle_arg(rt_arg32(c, 0));
    void * a_attachedObj = rt_gl_handle_arg(rt_arg32(c, 1));
    ((void (*)(void *, void *))fn)(a_containerObj, a_attachedObj);
    return 1;
}

static int g_glDisable(CPU *c, void *fn) {
    uint32_t a_cap = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_cap);
    return 1;
}

static int g_glDisableClientState(CPU *c, void *fn) {
    uint32_t a_array = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_array);
    return 1;
}

static int g_glDisableIndexedEXT(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    uint32_t a_index = rt_arg32(c, 1);
    ((void (*)(uint32_t, uint32_t))fn)(a_target, a_index);
    return 1;
}

static int g_glDisableVertexAttribArray(CPU *c, void *fn) {
    uint32_t a_index = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_index);
    return 1;
}

static int g_glDrawArrays(CPU *c, void *fn) {
    uint32_t a_mode = rt_arg32(c, 0);
    int32_t a_first = (int32_t)rt_arg32(c, 1);
    int32_t a_count = (int32_t)rt_arg32(c, 2);
    ((void (*)(uint32_t, int32_t, int32_t))fn)(a_mode, a_first, a_count);
    return 1;
}

static int g_glDrawBuffer(CPU *c, void *fn) {
    uint32_t a_mode = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_mode);
    return 1;
}

static int g_glDrawBuffers(CPU *c, void *fn) {
    int32_t a_n = (int32_t)rt_arg32(c, 0);
    void *a_bufs = (void *)rt_gl_in(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_n)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_n, a_bufs);
    return 1;
}

static int g_glDrawRangeElements(CPU *c, void *fn) {
    uint32_t a_mode = rt_arg32(c, 0);
    uint32_t a_start = rt_arg32(c, 1);
    uint32_t a_end = rt_arg32(c, 2);
    int32_t a_count = (int32_t)rt_arg32(c, 3);
    uint32_t a_type = rt_arg32(c, 4);
    void *a_indices = (void *)rt_gl_indices(c, rt_arg32(c, 5), a_count, a_type);
    ((void (*)(uint32_t, uint32_t, uint32_t, int32_t, uint32_t, void *))fn)(a_mode, a_start, a_end, a_count, a_type, a_indices);
    return 1;
}

static int g_glDrawRangeElementsBaseVertex(CPU *c, void *fn) {
    uint32_t a_mode = rt_arg32(c, 0);
    uint32_t a_start = rt_arg32(c, 1);
    uint32_t a_end = rt_arg32(c, 2);
    int32_t a_count = (int32_t)rt_arg32(c, 3);
    uint32_t a_type = rt_arg32(c, 4);
    int32_t a_base_vertex = (int32_t)rt_arg32(c, 6);
    void *a_indices = (void *)rt_gl_indices(c, rt_arg32(c, 5), a_count, a_type);
    ((void (*)(uint32_t, uint32_t, uint32_t, int32_t, uint32_t, void *, int32_t))fn)(a_mode, a_start, a_end, a_count, a_type, a_indices, a_base_vertex);
    return 1;
}

static int g_glEnable(CPU *c, void *fn) {
    uint32_t a_cap = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_cap);
    return 1;
}

static int g_glEnableClientState(CPU *c, void *fn) {
    uint32_t a_array = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_array);
    return 1;
}

static int g_glEnableIndexedEXT(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    uint32_t a_index = rt_arg32(c, 1);
    ((void (*)(uint32_t, uint32_t))fn)(a_target, a_index);
    return 1;
}

static int g_glEnableVertexAttribArray(CPU *c, void *fn) {
    uint32_t a_index = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_index);
    return 1;
}

static int g_glEnd(CPU *c, void *fn) {
    (void)c;
    ((void (*)(void))fn)();
    return 1;
}

static int g_glEndQuery(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_target);
    return 1;
}

static int g_glEndQueryARB(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_target);
    return 1;
}

static int g_glFinish(CPU *c, void *fn) {
    (void)c;
    ((void (*)(void))fn)();
    return 1;
}

static int g_glFinishFenceAPPLE(CPU *c, void *fn) {
    uint32_t a_fence = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_fence);
    return 1;
}

static int g_glFlush(CPU *c, void *fn) {
    (void)c;
    ((void (*)(void))fn)();
    return 1;
}

static int g_glFramebufferRenderbuffer(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    uint32_t a_attachment = rt_arg32(c, 1);
    uint32_t a_renderbuffertarget = rt_arg32(c, 2);
    uint32_t a_renderbuffer = rt_arg32(c, 3);
    ((void (*)(uint32_t, uint32_t, uint32_t, uint32_t))fn)(a_target, a_attachment, a_renderbuffertarget, a_renderbuffer);
    return 1;
}

static int g_glFramebufferRenderbufferEXT(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    uint32_t a_attachment = rt_arg32(c, 1);
    uint32_t a_renderbuffertarget = rt_arg32(c, 2);
    uint32_t a_renderbuffer = rt_arg32(c, 3);
    ((void (*)(uint32_t, uint32_t, uint32_t, uint32_t))fn)(a_target, a_attachment, a_renderbuffertarget, a_renderbuffer);
    return 1;
}

static int g_glFramebufferTexture2D(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    uint32_t a_attachment = rt_arg32(c, 1);
    uint32_t a_textarget = rt_arg32(c, 2);
    uint32_t a_texture = rt_arg32(c, 3);
    int32_t a_level = (int32_t)rt_arg32(c, 4);
    ((void (*)(uint32_t, uint32_t, uint32_t, uint32_t, int32_t))fn)(a_target, a_attachment, a_textarget, a_texture, a_level);
    return 1;
}

static int g_glFramebufferTexture2DEXT(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    uint32_t a_attachment = rt_arg32(c, 1);
    uint32_t a_textarget = rt_arg32(c, 2);
    uint32_t a_texture = rt_arg32(c, 3);
    int32_t a_level = (int32_t)rt_arg32(c, 4);
    ((void (*)(uint32_t, uint32_t, uint32_t, uint32_t, int32_t))fn)(a_target, a_attachment, a_textarget, a_texture, a_level);
    return 1;
}

static int g_glFramebufferTexture3D(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    uint32_t a_attachment = rt_arg32(c, 1);
    uint32_t a_textarget = rt_arg32(c, 2);
    uint32_t a_texture = rt_arg32(c, 3);
    int32_t a_level = (int32_t)rt_arg32(c, 4);
    int32_t a_zoffset = (int32_t)rt_arg32(c, 5);
    ((void (*)(uint32_t, uint32_t, uint32_t, uint32_t, int32_t, int32_t))fn)(a_target, a_attachment, a_textarget, a_texture, a_level, a_zoffset);
    return 1;
}

static int g_glFramebufferTexture3DEXT(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    uint32_t a_attachment = rt_arg32(c, 1);
    uint32_t a_textarget = rt_arg32(c, 2);
    uint32_t a_texture = rt_arg32(c, 3);
    int32_t a_level = (int32_t)rt_arg32(c, 4);
    int32_t a_zoffset = (int32_t)rt_arg32(c, 5);
    ((void (*)(uint32_t, uint32_t, uint32_t, uint32_t, int32_t, int32_t))fn)(a_target, a_attachment, a_textarget, a_texture, a_level, a_zoffset);
    return 1;
}

static int g_glFrontFace(CPU *c, void *fn) {
    uint32_t a_mode = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_mode);
    return 1;
}

static int g_glGenBuffersARB(CPU *c, void *fn) {
    int32_t a_n = (int32_t)rt_arg32(c, 0);
    void *a_buffers = (void *)rt_gl_out(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_n)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_n, a_buffers);
    return 1;
}

static int g_glGenFencesAPPLE(CPU *c, void *fn) {
    int32_t a_n = (int32_t)rt_arg32(c, 0);
    void *a_fences = (void *)rt_gl_out(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_n)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_n, a_fences);
    return 1;
}

static int g_glGenFramebuffers(CPU *c, void *fn) {
    int32_t a_n = (int32_t)rt_arg32(c, 0);
    void *a_framebuffers = (void *)rt_gl_out(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_n)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_n, a_framebuffers);
    return 1;
}

static int g_glGenFramebuffersEXT(CPU *c, void *fn) {
    int32_t a_n = (int32_t)rt_arg32(c, 0);
    void *a_framebuffers = (void *)rt_gl_out(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_n)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_n, a_framebuffers);
    return 1;
}

static int g_glGenProgramsARB(CPU *c, void *fn) {
    int32_t a_n = (int32_t)rt_arg32(c, 0);
    void *a_programs = (void *)rt_gl_out(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_n)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_n, a_programs);
    return 1;
}

static int g_glGenQueries(CPU *c, void *fn) {
    int32_t a_n = (int32_t)rt_arg32(c, 0);
    void *a_ids = (void *)rt_gl_out(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_n)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_n, a_ids);
    return 1;
}

static int g_glGenQueriesARB(CPU *c, void *fn) {
    int32_t a_n = (int32_t)rt_arg32(c, 0);
    void *a_ids = (void *)rt_gl_out(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_n)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_n, a_ids);
    return 1;
}

static int g_glGenRenderbuffers(CPU *c, void *fn) {
    int32_t a_n = (int32_t)rt_arg32(c, 0);
    void *a_renderbuffers = (void *)rt_gl_out(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_n)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_n, a_renderbuffers);
    return 1;
}

static int g_glGenRenderbuffersEXT(CPU *c, void *fn) {
    int32_t a_n = (int32_t)rt_arg32(c, 0);
    void *a_renderbuffers = (void *)rt_gl_out(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_n)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_n, a_renderbuffers);
    return 1;
}

static int g_glGenSamplers(CPU *c, void *fn) {
    int32_t a_count = (int32_t)rt_arg32(c, 0);
    void *a_samplers = (void *)rt_gl_out(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_count)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_count, a_samplers);
    return 1;
}

static int g_glGenTextures(CPU *c, void *fn) {
    int32_t a_n = (int32_t)rt_arg32(c, 0);
    void *a_textures = (void *)rt_gl_out(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_n)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_n, a_textures);
    return 1;
}

static int g_glGenVertexArrays(CPU *c, void *fn) {
    int32_t a_n = (int32_t)rt_arg32(c, 0);
    void *a_arrays = (void *)rt_gl_out(c, rt_arg32(c, 1), (uint64_t)(rt_gl_n(a_n)*4), 0);
    ((void (*)(int32_t, void *))fn)(a_n, a_arrays);
    return 1;
}

static int g_glGetBooleanIndexedvEXT(CPU *c, void *fn) {
    uint32_t a_param = rt_arg32(c, 0);
    uint32_t a_index = rt_arg32(c, 1);
    void *a_values = (void *)rt_gl_out(c, rt_arg32(c, 2), (uint64_t)(rt_gl_get_count(a_param)), 0);
    ((void (*)(uint32_t, uint32_t, void *))fn)(a_param, a_index, a_values);
    return 1;
}

static int g_glGetBooleanv(CPU *c, void *fn) {
    uint32_t a_pname = rt_arg32(c, 0);
    void *a_params = (void *)rt_gl_out(c, rt_arg32(c, 1), (uint64_t)(rt_gl_get_count(a_pname)), 0);
    ((void (*)(uint32_t, void *))fn)(a_pname, a_params);
    return 1;
}

static int g_glGetCompressedTexImage(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    int32_t a_lod = (int32_t)rt_arg32(c, 1);
    void *a_img = (void *)rt_gl_pack_compressed(c, rt_arg32(c, 2), a_target, a_lod);
    ((void (*)(uint32_t, int32_t, void *))fn)(a_target, a_lod, a_img);
    return 1;
}

static int g_glGetDoublev(CPU *c, void *fn) {
    uint32_t a_pname = rt_arg32(c, 0);
    void *a_params = (void *)rt_gl_out(c, rt_arg32(c, 1), (uint64_t)(rt_gl_get_count(a_pname)*8), 0);
    ((void (*)(uint32_t, void *))fn)(a_pname, a_params);
    return 1;
}

static int g_glGetError(CPU *c, void *fn) {
    c->eax = (uint32_t)((uint32_t (*)(void))fn)();
    return 1;
}

static int g_glGetFloatv(CPU *c, void *fn) {
    uint32_t a_pname = rt_arg32(c, 0);
    void *a_params = (void *)rt_gl_out(c, rt_arg32(c, 1), (uint64_t)(rt_gl_get_count(a_pname)*4), 0);
    ((void (*)(uint32_t, void *))fn)(a_pname, a_params);
    return 1;
}

static int g_glGetInfoLogARB(CPU *c, void *fn) {
    void * a_obj = rt_gl_handle_arg(rt_arg32(c, 0));
    int32_t a_maxLength = (int32_t)rt_arg32(c, 1);
    void *a_length = (void *)rt_gl_out(c, rt_arg32(c, 2), (uint64_t)(4), 1);
    void *a_infoLog = (void *)rt_gl_out(c, rt_arg32(c, 3), (uint64_t)(rt_gl_n(a_maxLength)), 0);
    ((void (*)(void *, int32_t, void *, void *))fn)(a_obj, a_maxLength, a_length, a_infoLog);
    return 1;
}

static int g_glGetIntegerv(CPU *c, void *fn) {
    uint32_t a_pname = rt_arg32(c, 0);
    void *a_params = (void *)rt_gl_out(c, rt_arg32(c, 1), (uint64_t)(rt_gl_get_count(a_pname)*4), 0);
    ((void (*)(uint32_t, void *))fn)(a_pname, a_params);
    return 1;
}

static int g_glGetObjectParameterivARB(CPU *c, void *fn) {
    void * a_obj = rt_gl_handle_arg(rt_arg32(c, 0));
    uint32_t a_pname = rt_arg32(c, 1);
    void *a_params = (void *)rt_gl_out(c, rt_arg32(c, 2), (uint64_t)(4), 0);
    ((void (*)(void *, uint32_t, void *))fn)(a_obj, a_pname, a_params);
    return 1;
}

static int g_glGetProgramivARB(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    uint32_t a_pname = rt_arg32(c, 1);
    void *a_params = (void *)rt_gl_out(c, rt_arg32(c, 2), (uint64_t)(4), 0);
    ((void (*)(uint32_t, uint32_t, void *))fn)(a_target, a_pname, a_params);
    return 1;
}

static int g_glGetQueryObjectiv(CPU *c, void *fn) {
    uint32_t a_id = rt_arg32(c, 0);
    uint32_t a_pname = rt_arg32(c, 1);
    void *a_params = (void *)rt_gl_out(c, rt_arg32(c, 2), (uint64_t)(4), 0);
    ((void (*)(uint32_t, uint32_t, void *))fn)(a_id, a_pname, a_params);
    return 1;
}

static int g_glGetQueryObjectivARB(CPU *c, void *fn) {
    uint32_t a_id = rt_arg32(c, 0);
    uint32_t a_pname = rt_arg32(c, 1);
    void *a_params = (void *)rt_gl_out(c, rt_arg32(c, 2), (uint64_t)(4), 0);
    ((void (*)(uint32_t, uint32_t, void *))fn)(a_id, a_pname, a_params);
    return 1;
}

static int g_glGetQueryObjectui64v(CPU *c, void *fn) {
    uint32_t a_id = rt_arg32(c, 0);
    uint32_t a_pname = rt_arg32(c, 1);
    void *a_params = (void *)rt_gl_out(c, rt_arg32(c, 2), (uint64_t)(8), 0);
    ((void (*)(uint32_t, uint32_t, void *))fn)(a_id, a_pname, a_params);
    return 1;
}

static int g_glGetQueryObjectuivARB(CPU *c, void *fn) {
    uint32_t a_id = rt_arg32(c, 0);
    uint32_t a_pname = rt_arg32(c, 1);
    void *a_params = (void *)rt_gl_out(c, rt_arg32(c, 2), (uint64_t)(4), 0);
    ((void (*)(uint32_t, uint32_t, void *))fn)(a_id, a_pname, a_params);
    return 1;
}

static int g_glGetTexImage(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    int32_t a_level = (int32_t)rt_arg32(c, 1);
    uint32_t a_format = rt_arg32(c, 2);
    uint32_t a_type = rt_arg32(c, 3);
    void *a_pixels = (void *)rt_gl_pack_tex(c, rt_arg32(c, 4), a_target, a_level, a_format, a_type);
    ((void (*)(uint32_t, int32_t, uint32_t, uint32_t, void *))fn)(a_target, a_level, a_format, a_type, a_pixels);
    return 1;
}

static int g_glGetTexLevelParameteriv(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    int32_t a_level = (int32_t)rt_arg32(c, 1);
    uint32_t a_pname = rt_arg32(c, 2);
    void *a_params = (void *)rt_gl_out(c, rt_arg32(c, 3), (uint64_t)(4), 0);
    ((void (*)(uint32_t, int32_t, uint32_t, void *))fn)(a_target, a_level, a_pname, a_params);
    return 1;
}

static int g_glGetUniformBufferSizeEXT(CPU *c, void *fn) {
    uint32_t a_program = rt_arg32(c, 0);
    int32_t a_location = (int32_t)rt_arg32(c, 1);
    c->eax = (uint32_t)((int32_t (*)(uint32_t, int32_t))fn)(a_program, a_location);
    return 1;
}

static int g_glGetUniformLocationARB(CPU *c, void *fn) {
    void * a_programObj = rt_gl_handle_arg(rt_arg32(c, 0));
    void *a_name = (void *)rt_guest_cstr(c, rt_arg32(c, 1));
    c->eax = (uint32_t)((int32_t (*)(void *, void *))fn)(a_programObj, a_name);
    return 1;
}

static int g_glGetUniformOffsetEXT(CPU *c, void *fn) {
    uint32_t a_program = rt_arg32(c, 0);
    int32_t a_location = (int32_t)rt_arg32(c, 1);
    c->eax = rt_gl_iptr_ret(c, ((intptr_t (*)(uint32_t, int32_t))fn)(a_program, a_location));
    return 1;
}

static int g_glIsEnabled(CPU *c, void *fn) {
    uint32_t a_cap = rt_arg32(c, 0);
    c->eax = (uint8_t)((uint8_t (*)(uint32_t))fn)(a_cap);
    return 1;
}

static int g_glIsTexture(CPU *c, void *fn) {
    uint32_t a_texture = rt_arg32(c, 0);
    c->eax = (uint8_t)((uint8_t (*)(uint32_t))fn)(a_texture);
    return 1;
}

static int g_glLinkProgramARB(CPU *c, void *fn) {
    void * a_programObj = rt_gl_handle_arg(rt_arg32(c, 0));
    ((void (*)(void *))fn)(a_programObj);
    return 1;
}

static int g_glOrtho(CPU *c, void *fn) {
    double a_left = rt_arg_f64(c, 0);
    double a_right = rt_arg_f64(c, 8);
    double a_bottom = rt_arg_f64(c, 16);
    double a_top = rt_arg_f64(c, 24);
    double a_zNear = rt_arg_f64(c, 32);
    double a_zFar = rt_arg_f64(c, 40);
    ((void (*)(double, double, double, double, double, double))fn)(a_left, a_right, a_bottom, a_top, a_zNear, a_zFar);
    return 1;
}

static int g_glPixelStorei(CPU *c, void *fn) {
    uint32_t a_pname = rt_arg32(c, 0);
    int32_t a_param = (int32_t)rt_arg32(c, 1);
    ((void (*)(uint32_t, int32_t))fn)(a_pname, a_param);
    return 1;
}

static int g_glPolygonMode(CPU *c, void *fn) {
    uint32_t a_face = rt_arg32(c, 0);
    uint32_t a_mode = rt_arg32(c, 1);
    ((void (*)(uint32_t, uint32_t))fn)(a_face, a_mode);
    return 1;
}

static int g_glPolygonOffset(CPU *c, void *fn) {
    float a_factor = rt_arg_f32(c, 0);
    float a_units = rt_arg_f32(c, 4);
    ((void (*)(float, float))fn)(a_factor, a_units);
    return 1;
}

static int g_glPopAttrib(CPU *c, void *fn) {
    (void)c;
    ((void (*)(void))fn)();
    return 1;
}

static int g_glPopClientAttrib(CPU *c, void *fn) {
    (void)c;
    ((void (*)(void))fn)();
    return 1;
}

static int g_glProgramEnvParameters4fvEXT(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    uint32_t a_index = rt_arg32(c, 1);
    int32_t a_count = (int32_t)rt_arg32(c, 2);
    void *a_params = (void *)rt_gl_in(c, rt_arg32(c, 3), (uint64_t)(rt_gl_n(a_count)*16), 0);
    ((void (*)(uint32_t, uint32_t, int32_t, void *))fn)(a_target, a_index, a_count, a_params);
    return 1;
}

static int g_glProgramStringARB(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    uint32_t a_format = rt_arg32(c, 1);
    int32_t a_len = (int32_t)rt_arg32(c, 2);
    void *a_string = (void *)rt_gl_in(c, rt_arg32(c, 3), (uint64_t)(rt_gl_n(a_len)), 0);
    ((void (*)(uint32_t, uint32_t, int32_t, void *))fn)(a_target, a_format, a_len, a_string);
    return 1;
}

static int g_glPushAttrib(CPU *c, void *fn) {
    uint32_t a_mask = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_mask);
    return 1;
}

static int g_glPushClientAttrib(CPU *c, void *fn) {
    uint32_t a_mask = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_mask);
    return 1;
}

static int g_glQueryCounter(CPU *c, void *fn) {
    uint32_t a_id = rt_arg32(c, 0);
    uint32_t a_target = rt_arg32(c, 1);
    ((void (*)(uint32_t, uint32_t))fn)(a_id, a_target);
    return 1;
}

static int g_glReadBuffer(CPU *c, void *fn) {
    uint32_t a_mode = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_mode);
    return 1;
}

static int g_glRenderbufferStorageMultisample(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    int32_t a_samples = (int32_t)rt_arg32(c, 1);
    uint32_t a_internalformat = rt_arg32(c, 2);
    int32_t a_width = (int32_t)rt_arg32(c, 3);
    int32_t a_height = (int32_t)rt_arg32(c, 4);
    ((void (*)(uint32_t, int32_t, uint32_t, int32_t, int32_t))fn)(a_target, a_samples, a_internalformat, a_width, a_height);
    return 1;
}

static int g_glRenderbufferStorageMultisampleEXT(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    int32_t a_samples = (int32_t)rt_arg32(c, 1);
    uint32_t a_internalformat = rt_arg32(c, 2);
    int32_t a_width = (int32_t)rt_arg32(c, 3);
    int32_t a_height = (int32_t)rt_arg32(c, 4);
    ((void (*)(uint32_t, int32_t, uint32_t, int32_t, int32_t))fn)(a_target, a_samples, a_internalformat, a_width, a_height);
    return 1;
}

static int g_glSamplerParameterf(CPU *c, void *fn) {
    uint32_t a_sampler = rt_arg32(c, 0);
    uint32_t a_pname = rt_arg32(c, 1);
    float a_param = rt_arg_f32(c, 8);
    ((void (*)(uint32_t, uint32_t, float))fn)(a_sampler, a_pname, a_param);
    return 1;
}

static int g_glSamplerParameterfv(CPU *c, void *fn) {
    uint32_t a_sampler = rt_arg32(c, 0);
    uint32_t a_pname = rt_arg32(c, 1);
    void *a_param = (void *)rt_gl_in(c, rt_arg32(c, 2), (uint64_t)(rt_gl_texparam_count(a_pname)*4), 0);
    ((void (*)(uint32_t, uint32_t, void *))fn)(a_sampler, a_pname, a_param);
    return 1;
}

static int g_glSamplerParameteri(CPU *c, void *fn) {
    uint32_t a_sampler = rt_arg32(c, 0);
    uint32_t a_pname = rt_arg32(c, 1);
    int32_t a_param = (int32_t)rt_arg32(c, 2);
    ((void (*)(uint32_t, uint32_t, int32_t))fn)(a_sampler, a_pname, a_param);
    return 1;
}

static int g_glScissor(CPU *c, void *fn) {
    int32_t a_x = (int32_t)rt_arg32(c, 0);
    int32_t a_y = (int32_t)rt_arg32(c, 1);
    int32_t a_width = (int32_t)rt_arg32(c, 2);
    int32_t a_height = (int32_t)rt_arg32(c, 3);
    ((void (*)(int32_t, int32_t, int32_t, int32_t))fn)(a_x, a_y, a_width, a_height);
    return 1;
}

static int g_glSetFenceAPPLE(CPU *c, void *fn) {
    uint32_t a_fence = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_fence);
    return 1;
}

static int g_glStencilFunc(CPU *c, void *fn) {
    uint32_t a_func = rt_arg32(c, 0);
    int32_t a_ref = (int32_t)rt_arg32(c, 1);
    uint32_t a_mask = rt_arg32(c, 2);
    ((void (*)(uint32_t, int32_t, uint32_t))fn)(a_func, a_ref, a_mask);
    return 1;
}

static int g_glStencilFuncSeparate(CPU *c, void *fn) {
    uint32_t a_face = rt_arg32(c, 0);
    uint32_t a_func = rt_arg32(c, 1);
    int32_t a_ref = (int32_t)rt_arg32(c, 2);
    uint32_t a_mask = rt_arg32(c, 3);
    ((void (*)(uint32_t, uint32_t, int32_t, uint32_t))fn)(a_face, a_func, a_ref, a_mask);
    return 1;
}

static int g_glStencilMask(CPU *c, void *fn) {
    uint32_t a_mask = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_mask);
    return 1;
}

static int g_glStencilOp(CPU *c, void *fn) {
    uint32_t a_fail = rt_arg32(c, 0);
    uint32_t a_zfail = rt_arg32(c, 1);
    uint32_t a_zpass = rt_arg32(c, 2);
    ((void (*)(uint32_t, uint32_t, uint32_t))fn)(a_fail, a_zfail, a_zpass);
    return 1;
}

static int g_glStencilOpSeparate(CPU *c, void *fn) {
    uint32_t a_face = rt_arg32(c, 0);
    uint32_t a_fail = rt_arg32(c, 1);
    uint32_t a_zfail = rt_arg32(c, 2);
    uint32_t a_zpass = rt_arg32(c, 3);
    ((void (*)(uint32_t, uint32_t, uint32_t, uint32_t))fn)(a_face, a_fail, a_zfail, a_zpass);
    return 1;
}

static int g_glTestFenceAPPLE(CPU *c, void *fn) {
    uint32_t a_fence = rt_arg32(c, 0);
    c->eax = (uint8_t)((uint8_t (*)(uint32_t))fn)(a_fence);
    return 1;
}

static int g_glTexCoord2f(CPU *c, void *fn) {
    float a_s = rt_arg_f32(c, 0);
    float a_t = rt_arg_f32(c, 4);
    ((void (*)(float, float))fn)(a_s, a_t);
    return 1;
}

static int g_glTexCoordPointer(CPU *c, void *fn) {
    int32_t a_size = (int32_t)rt_arg32(c, 0);
    uint32_t a_type = rt_arg32(c, 1);
    int32_t a_stride = (int32_t)rt_arg32(c, 2);
    void *a_pointer = (void *)rt_gl_array(c, rt_arg32(c, 3));
    ((void (*)(int32_t, uint32_t, int32_t, void *))fn)(a_size, a_type, a_stride, a_pointer);
    return 1;
}

static int g_glTexImage2D(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    int32_t a_level = (int32_t)rt_arg32(c, 1);
    int32_t a_internalformat = (int32_t)rt_arg32(c, 2);
    int32_t a_width = (int32_t)rt_arg32(c, 3);
    int32_t a_height = (int32_t)rt_arg32(c, 4);
    int32_t a_border = (int32_t)rt_arg32(c, 5);
    uint32_t a_format = rt_arg32(c, 6);
    uint32_t a_type = rt_arg32(c, 7);
    void *a_pixels = (void *)rt_gl_unpack(c, rt_arg32(c, 8), a_width, a_height, 0, a_format, a_type, 1);
    ((void (*)(uint32_t, int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t, void *))fn)(a_target, a_level, a_internalformat, a_width, a_height, a_border, a_format, a_type, a_pixels);
    return 1;
}

static int g_glTexImage3D(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    int32_t a_level = (int32_t)rt_arg32(c, 1);
    int32_t a_internalformat = (int32_t)rt_arg32(c, 2);
    int32_t a_width = (int32_t)rt_arg32(c, 3);
    int32_t a_height = (int32_t)rt_arg32(c, 4);
    int32_t a_depth = (int32_t)rt_arg32(c, 5);
    int32_t a_border = (int32_t)rt_arg32(c, 6);
    uint32_t a_format = rt_arg32(c, 7);
    uint32_t a_type = rt_arg32(c, 8);
    void *a_pixels = (void *)rt_gl_unpack(c, rt_arg32(c, 9), a_width, a_height, a_depth, a_format, a_type, 1);
    ((void (*)(uint32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t, void *))fn)(a_target, a_level, a_internalformat, a_width, a_height, a_depth, a_border, a_format, a_type, a_pixels);
    return 1;
}

static int g_glTexParameterfv(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    uint32_t a_pname = rt_arg32(c, 1);
    void *a_params = (void *)rt_gl_in(c, rt_arg32(c, 2), (uint64_t)(rt_gl_texparam_count(a_pname)*4), 0);
    ((void (*)(uint32_t, uint32_t, void *))fn)(a_target, a_pname, a_params);
    return 1;
}

static int g_glTexParameteri(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    uint32_t a_pname = rt_arg32(c, 1);
    int32_t a_param = (int32_t)rt_arg32(c, 2);
    ((void (*)(uint32_t, uint32_t, int32_t))fn)(a_target, a_pname, a_param);
    return 1;
}

static int g_glTexSubImage2D(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    int32_t a_level = (int32_t)rt_arg32(c, 1);
    int32_t a_xoffset = (int32_t)rt_arg32(c, 2);
    int32_t a_yoffset = (int32_t)rt_arg32(c, 3);
    int32_t a_width = (int32_t)rt_arg32(c, 4);
    int32_t a_height = (int32_t)rt_arg32(c, 5);
    uint32_t a_format = rt_arg32(c, 6);
    uint32_t a_type = rt_arg32(c, 7);
    void *a_pixels = (void *)rt_gl_unpack(c, rt_arg32(c, 8), a_width, a_height, 0, a_format, a_type, 0);
    ((void (*)(uint32_t, int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t, void *))fn)(a_target, a_level, a_xoffset, a_yoffset, a_width, a_height, a_format, a_type, a_pixels);
    return 1;
}

static int g_glTextureRangeAPPLE(CPU *c, void *fn) {
    uint32_t a_target = rt_arg32(c, 0);
    int32_t a_length = (int32_t)rt_arg32(c, 1);
    void *a_pointer = (void *)rt_gl_in(c, rt_arg32(c, 2), (uint64_t)(rt_gl_n(a_length)), 1);
    ((void (*)(uint32_t, int32_t, void *))fn)(a_target, a_length, a_pointer);
    return 1;
}

static int g_glUniform1f(CPU *c, void *fn) {
    int32_t a_location = (int32_t)rt_arg32(c, 0);
    float a_v0 = rt_arg_f32(c, 4);
    ((void (*)(int32_t, float))fn)(a_location, a_v0);
    return 1;
}

static int g_glUniform1i(CPU *c, void *fn) {
    int32_t a_location = (int32_t)rt_arg32(c, 0);
    int32_t a_v0 = (int32_t)rt_arg32(c, 1);
    ((void (*)(int32_t, int32_t))fn)(a_location, a_v0);
    return 1;
}

static int g_glUniform1iARB(CPU *c, void *fn) {
    int32_t a_location = (int32_t)rt_arg32(c, 0);
    int32_t a_v0 = (int32_t)rt_arg32(c, 1);
    ((void (*)(int32_t, int32_t))fn)(a_location, a_v0);
    return 1;
}

static int g_glUniform4fv(CPU *c, void *fn) {
    int32_t a_location = (int32_t)rt_arg32(c, 0);
    int32_t a_count = (int32_t)rt_arg32(c, 1);
    void *a_value = (void *)rt_gl_in(c, rt_arg32(c, 2), (uint64_t)(rt_gl_n(a_count)*16), 0);
    ((void (*)(int32_t, int32_t, void *))fn)(a_location, a_count, a_value);
    return 1;
}

static int g_glUniformBufferEXT(CPU *c, void *fn) {
    uint32_t a_program = rt_arg32(c, 0);
    int32_t a_location = (int32_t)rt_arg32(c, 1);
    uint32_t a_buffer = rt_arg32(c, 2);
    ((void (*)(uint32_t, int32_t, uint32_t))fn)(a_program, a_location, a_buffer);
    return 1;
}

static int g_glUseProgram(CPU *c, void *fn) {
    uint32_t a_program = rt_arg32(c, 0);
    ((void (*)(uint32_t))fn)(a_program);
    return 1;
}

static int g_glVertex3f(CPU *c, void *fn) {
    float a_x = rt_arg_f32(c, 0);
    float a_y = rt_arg_f32(c, 4);
    float a_z = rt_arg_f32(c, 8);
    ((void (*)(float, float, float))fn)(a_x, a_y, a_z);
    return 1;
}

static int g_glVertexAttribPointer(CPU *c, void *fn) {
    uint32_t a_index = rt_arg32(c, 0);
    int32_t a_size = (int32_t)rt_arg32(c, 1);
    uint32_t a_type = rt_arg32(c, 2);
    uint8_t a_normalized = (uint8_t)rt_arg32(c, 3);
    int32_t a_stride = (int32_t)rt_arg32(c, 4);
    void *a_pointer = (void *)rt_gl_array(c, rt_arg32(c, 5));
    ((void (*)(uint32_t, int32_t, uint32_t, uint8_t, int32_t, void *))fn)(a_index, a_size, a_type, a_normalized, a_stride, a_pointer);
    return 1;
}

static int g_glVertexPointer(CPU *c, void *fn) {
    int32_t a_size = (int32_t)rt_arg32(c, 0);
    uint32_t a_type = rt_arg32(c, 1);
    int32_t a_stride = (int32_t)rt_arg32(c, 2);
    void *a_pointer = (void *)rt_gl_array(c, rt_arg32(c, 3));
    ((void (*)(int32_t, uint32_t, int32_t, void *))fn)(a_size, a_type, a_stride, a_pointer);
    return 1;
}

static int g_glViewport(CPU *c, void *fn) {
    int32_t a_x = (int32_t)rt_arg32(c, 0);
    int32_t a_y = (int32_t)rt_arg32(c, 1);
    int32_t a_width = (int32_t)rt_arg32(c, 2);
    int32_t a_height = (int32_t)rt_arg32(c, 3);
    ((void (*)(int32_t, int32_t, int32_t, int32_t))fn)(a_x, a_y, a_width, a_height);
    return 1;
}

// Absentes des en-têtes et de OpenGL.framework (rt_gl_proc_address renvoie 0) :
//   glBindMultiTextureEXT glDebugMessageCallbackARB glDebugMessageControlARB glDeleteFencesNV
//   glFinishFenceNV glGenFencesNV glGetTextureHandleNV glGetTextureSamplerHandleNV
//   glIsTextureHandleResidentNV glMakeTextureHandleNonResidentNV glMakeTextureHandleResidentNV glProgramUniformHandleui64NV
//   glProgramUniformHandleui64vNV glSetFenceNV glStringMarkerGREMEDY glTestFenceNV
//   glUniformHandleui64NV glUniformHandleui64vNV
const RtGlEntry rt_gl_entries[] = {
    {"glActiveTexture", g_glActiveTexture},
    {"glAlphaFunc", g_glAlphaFunc},
    {"glAttachObjectARB", g_glAttachObjectARB},
    {"glBegin", g_glBegin},
    {"glBeginQuery", g_glBeginQuery},
    {"glBeginQueryARB", g_glBeginQueryARB},
    {"glBindAttribLocationARB", g_glBindAttribLocationARB},
    {"glBindBufferARB", g_glBindBufferARB},
    {"glBindFramebuffer", g_glBindFramebuffer},
    {"glBindFramebufferEXT", g_glBindFramebufferEXT},
    {"glBindProgramARB", g_glBindProgramARB},
    {"glBindRenderbuffer", g_glBindRenderbuffer},
    {"glBindRenderbufferEXT", g_glBindRenderbufferEXT},
    {"glBindSampler", g_glBindSampler},
    {"glBindTexture", g_glBindTexture},
    {"glBindVertexArray", g_glBindVertexArray},
    {"glBlendColor", g_glBlendColor},
    {"glBlendEquation", g_glBlendEquation},
    {"glBlendFunc", g_glBlendFunc},
    {"glBlitFramebuffer", g_glBlitFramebuffer},
    {"glBlitFramebufferEXT", g_glBlitFramebufferEXT},
    {"glBufferDataARB", g_glBufferDataARB},
    {"glBufferParameteriAPPLE", rt_gl_manual_glBufferParameteriAPPLE},
    {"glBufferSubData", g_glBufferSubData},
    {"glCheckFramebufferStatus", g_glCheckFramebufferStatus},
    {"glCheckFramebufferStatusEXT", g_glCheckFramebufferStatusEXT},
    {"glClear", g_glClear},
    {"glClearColor", g_glClearColor},
    {"glClearDepth", g_glClearDepth},
    {"glClearStencil", g_glClearStencil},
    {"glClientActiveTexture", g_glClientActiveTexture},
    {"glClientWaitSync", rt_gl_manual_glClientWaitSync},
    {"glClipPlane", g_glClipPlane},
    {"glColor4f", g_glColor4f},
    {"glColor4sv", g_glColor4sv},
    {"glColorMask", g_glColorMask},
    {"glColorMaskIndexedEXT", g_glColorMaskIndexedEXT},
    {"glCompileShaderARB", g_glCompileShaderARB},
    {"glCompressedTexImage2D", g_glCompressedTexImage2D},
    {"glCompressedTexImage3D", g_glCompressedTexImage3D},
    {"glCopyBufferSubData", g_glCopyBufferSubData},
    {"glCreateProgramObjectARB", g_glCreateProgramObjectARB},
    {"glCreateShaderObjectARB", g_glCreateShaderObjectARB},
    {"glDeleteBuffersARB", rt_gl_manual_glDeleteBuffersARB},
    {"glDeleteFencesAPPLE", g_glDeleteFencesAPPLE},
    {"glDeleteFramebuffers", g_glDeleteFramebuffers},
    {"glDeleteFramebuffersEXT", g_glDeleteFramebuffersEXT},
    {"glDeleteObjectARB", g_glDeleteObjectARB},
    {"glDeleteProgramsARB", g_glDeleteProgramsARB},
    {"glDeleteQueries", g_glDeleteQueries},
    {"glDeleteQueriesARB", g_glDeleteQueriesARB},
    {"glDeleteRenderbuffers", g_glDeleteRenderbuffers},
    {"glDeleteRenderbuffersEXT", g_glDeleteRenderbuffersEXT},
    {"glDeleteSamplers", g_glDeleteSamplers},
    {"glDeleteShader", g_glDeleteShader},
    {"glDeleteSync", rt_gl_manual_glDeleteSync},
    {"glDeleteTextures", g_glDeleteTextures},
    {"glDeleteVertexArrays", g_glDeleteVertexArrays},
    {"glDepthFunc", g_glDepthFunc},
    {"glDepthMask", g_glDepthMask},
    {"glDepthRange", g_glDepthRange},
    {"glDetachObjectARB", g_glDetachObjectARB},
    {"glDisable", g_glDisable},
    {"glDisableClientState", g_glDisableClientState},
    {"glDisableIndexedEXT", g_glDisableIndexedEXT},
    {"glDisableVertexAttribArray", g_glDisableVertexAttribArray},
    {"glDrawArrays", g_glDrawArrays},
    {"glDrawBuffer", g_glDrawBuffer},
    {"glDrawBuffers", g_glDrawBuffers},
    {"glDrawRangeElements", g_glDrawRangeElements},
    {"glDrawRangeElementsBaseVertex", g_glDrawRangeElementsBaseVertex},
    {"glEnable", g_glEnable},
    {"glEnableClientState", g_glEnableClientState},
    {"glEnableIndexedEXT", g_glEnableIndexedEXT},
    {"glEnableVertexAttribArray", g_glEnableVertexAttribArray},
    {"glEnd", g_glEnd},
    {"glEndQuery", g_glEndQuery},
    {"glEndQueryARB", g_glEndQueryARB},
    {"glFenceSync", rt_gl_manual_glFenceSync},
    {"glFinish", g_glFinish},
    {"glFinishFenceAPPLE", g_glFinishFenceAPPLE},
    {"glFlush", g_glFlush},
    {"glFlushMappedBufferRange", rt_gl_manual_glFlushMappedBufferRange},
    {"glFlushMappedBufferRangeAPPLE", rt_gl_manual_glFlushMappedBufferRangeAPPLE},
    {"glFramebufferRenderbuffer", g_glFramebufferRenderbuffer},
    {"glFramebufferRenderbufferEXT", g_glFramebufferRenderbufferEXT},
    {"glFramebufferTexture2D", g_glFramebufferTexture2D},
    {"glFramebufferTexture2DEXT", g_glFramebufferTexture2DEXT},
    {"glFramebufferTexture3D", g_glFramebufferTexture3D},
    {"glFramebufferTexture3DEXT", g_glFramebufferTexture3DEXT},
    {"glFrontFace", g_glFrontFace},
    {"glGenBuffersARB", g_glGenBuffersARB},
    {"glGenFencesAPPLE", g_glGenFencesAPPLE},
    {"glGenFramebuffers", g_glGenFramebuffers},
    {"glGenFramebuffersEXT", g_glGenFramebuffersEXT},
    {"glGenProgramsARB", g_glGenProgramsARB},
    {"glGenQueries", g_glGenQueries},
    {"glGenQueriesARB", g_glGenQueriesARB},
    {"glGenRenderbuffers", g_glGenRenderbuffers},
    {"glGenRenderbuffersEXT", g_glGenRenderbuffersEXT},
    {"glGenSamplers", g_glGenSamplers},
    {"glGenTextures", g_glGenTextures},
    {"glGenVertexArrays", g_glGenVertexArrays},
    {"glGetBooleanIndexedvEXT", g_glGetBooleanIndexedvEXT},
    {"glGetBooleanv", g_glGetBooleanv},
    {"glGetCompressedTexImage", g_glGetCompressedTexImage},
    {"glGetDoublev", g_glGetDoublev},
    {"glGetError", g_glGetError},
    {"glGetFloatv", g_glGetFloatv},
    {"glGetInfoLogARB", g_glGetInfoLogARB},
    {"glGetIntegerv", g_glGetIntegerv},
    {"glGetObjectParameterivARB", g_glGetObjectParameterivARB},
    {"glGetProgramivARB", g_glGetProgramivARB},
    {"glGetQueryObjectiv", g_glGetQueryObjectiv},
    {"glGetQueryObjectivARB", g_glGetQueryObjectivARB},
    {"glGetQueryObjectui64v", g_glGetQueryObjectui64v},
    {"glGetQueryObjectuivARB", g_glGetQueryObjectuivARB},
    {"glGetString", rt_gl_manual_glGetString},
    {"glGetSynciv", rt_gl_manual_glGetSynciv},
    {"glGetTexImage", g_glGetTexImage},
    {"glGetTexLevelParameteriv", g_glGetTexLevelParameteriv},
    {"glGetTexParameterPointervAPPLE", rt_gl_manual_glGetTexParameterPointervAPPLE},
    {"glGetUniformBufferSizeEXT", g_glGetUniformBufferSizeEXT},
    {"glGetUniformLocationARB", g_glGetUniformLocationARB},
    {"glGetUniformOffsetEXT", g_glGetUniformOffsetEXT},
    {"glIsEnabled", g_glIsEnabled},
    {"glIsTexture", g_glIsTexture},
    {"glLinkProgramARB", g_glLinkProgramARB},
    {"glMapBufferARB", rt_gl_manual_glMapBufferARB},
    {"glMapBufferRange", rt_gl_manual_glMapBufferRange},
    {"glOrtho", g_glOrtho},
    {"glPixelStorei", g_glPixelStorei},
    {"glPolygonMode", g_glPolygonMode},
    {"glPolygonOffset", g_glPolygonOffset},
    {"glPopAttrib", g_glPopAttrib},
    {"glPopClientAttrib", g_glPopClientAttrib},
    {"glProgramEnvParameters4fvEXT", g_glProgramEnvParameters4fvEXT},
    {"glProgramStringARB", g_glProgramStringARB},
    {"glPushAttrib", g_glPushAttrib},
    {"glPushClientAttrib", g_glPushClientAttrib},
    {"glQueryCounter", g_glQueryCounter},
    {"glReadBuffer", g_glReadBuffer},
    {"glRenderbufferStorageMultisample", g_glRenderbufferStorageMultisample},
    {"glRenderbufferStorageMultisampleEXT", g_glRenderbufferStorageMultisampleEXT},
    {"glSamplerParameterf", g_glSamplerParameterf},
    {"glSamplerParameterfv", g_glSamplerParameterfv},
    {"glSamplerParameteri", g_glSamplerParameteri},
    {"glScissor", g_glScissor},
    {"glSetFenceAPPLE", g_glSetFenceAPPLE},
    {"glShaderSourceARB", rt_gl_manual_glShaderSourceARB},
    {"glStencilFunc", g_glStencilFunc},
    {"glStencilFuncSeparate", g_glStencilFuncSeparate},
    {"glStencilMask", g_glStencilMask},
    {"glStencilOp", g_glStencilOp},
    {"glStencilOpSeparate", g_glStencilOpSeparate},
    {"glTestFenceAPPLE", g_glTestFenceAPPLE},
    {"glTexCoord2f", g_glTexCoord2f},
    {"glTexCoordPointer", g_glTexCoordPointer},
    {"glTexImage2D", g_glTexImage2D},
    {"glTexImage3D", g_glTexImage3D},
    {"glTexParameterfv", g_glTexParameterfv},
    {"glTexParameteri", g_glTexParameteri},
    {"glTexSubImage2D", g_glTexSubImage2D},
    {"glTextureRangeAPPLE", g_glTextureRangeAPPLE},
    {"glUniform1f", g_glUniform1f},
    {"glUniform1i", g_glUniform1i},
    {"glUniform1iARB", g_glUniform1iARB},
    {"glUniform4fv", g_glUniform4fv},
    {"glUniformBufferEXT", g_glUniformBufferEXT},
    {"glUnmapBuffer", rt_gl_manual_glUnmapBuffer},
    {"glUseProgram", g_glUseProgram},
    {"glVertex3f", g_glVertex3f},
    {"glVertexAttribPointer", g_glVertexAttribPointer},
    {"glVertexPointer", g_glVertexPointer},
    {"glViewport", g_glViewport},
    {"glWaitSync", rt_gl_manual_glWaitSync},
};
const unsigned rt_gl_entry_count = sizeof(rt_gl_entries) / sizeof(*rt_gl_entries);
