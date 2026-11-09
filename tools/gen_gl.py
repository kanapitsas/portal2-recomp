#!/usr/bin/env python3
"""Generate runtime/gl_generated.c: i386 cdecl -> host OpenGL thunks.

NAMES is the list libtogl.so asks SDL_GL_GetProcAddress for (see
docs/gl-audit.md). Prototypes come from the SDK headers (gl.h, glext.h, gl3.h
of OpenGL.framework). Every pointer parameter needs an explicit rule in RULES
(or the function must be in MANUAL, hand-written in runtime/gl.c): the
generator refuses to guess.

    python3 tools/gen_gl.py            # writes runtime/gl_generated.c
"""
import re
import subprocess
import sys
from pathlib import Path

NAMES = """
glGetError glActiveTexture glAlphaFunc glAttachObjectARB glBegin glBindAttribLocationARB
glBindBufferARB glBindProgramARB glBindTexture glBlendColor glBlendEquation glBlendFunc
glBufferDataARB glClear glClearColor glClearDepth glClearStencil glClipPlane glColorMask
glCompileShaderARB glCompressedTexImage2D glCompressedTexImage3D glCreateProgramObjectARB
glCreateShaderObjectARB glDeleteBuffersARB glDeleteObjectARB glDeleteProgramsARB
glDeleteQueriesARB glDeleteShader glDeleteTextures glDepthFunc glDepthMask glDepthRange
glDetachObjectARB glDisable glDisableVertexAttribArray glDrawArrays glDrawBuffer glDrawBuffers
glDrawRangeElements glDrawRangeElementsBaseVertex glEnable glEnableVertexAttribArray glEnd
glFinish glFlush glFrontFace glGenBuffersARB glGenProgramsARB glGenQueriesARB glGenTextures
glGetBooleanv glGetCompressedTexImage glGetDoublev glGetFloatv glGetInfoLogARB glGetIntegerv
glGetObjectParameterivARB glGetProgramivARB glGetString glGetTexImage glGetUniformLocationARB
glIsEnabled glIsTexture glLinkProgramARB glMapBufferARB glOrtho glPixelStorei glPolygonMode
glPolygonOffset glPopAttrib glProgramStringARB glPushAttrib glReadBuffer glScissor
glShaderSourceARB glStencilFunc glStencilMask glStencilOp glTexCoord2f glTexImage2D glTexImage3D
glTexParameterfv glTexParameteri glTexSubImage2D glUniform1f glUniform1i glUniform1iARB
glUniform4fv glUnmapBuffer glUseProgram glVertex3f glVertexAttribPointer glViewport
glEnableClientState glDisableClientState glClientActiveTexture glVertexPointer glTexCoordPointer
glProgramEnvParameters4fvEXT glColor4sv glStencilOpSeparate glStencilFuncSeparate
glGetTexLevelParameteriv glColor4f
glBindFramebufferEXT glBindRenderbufferEXT glCheckFramebufferStatusEXT glDeleteRenderbuffersEXT
glFramebufferRenderbufferEXT glFramebufferTexture2DEXT glFramebufferTexture3DEXT
glGenFramebuffersEXT glGenRenderbuffersEXT glDeleteFramebuffersEXT glBlitFramebufferEXT
glRenderbufferStorageMultisampleEXT
glTestFenceAPPLE glSetFenceAPPLE glFinishFenceAPPLE glDeleteFencesAPPLE glGenFencesAPPLE
glTestFenceNV glSetFenceNV glFinishFenceNV glDeleteFencesNV glGenFencesNV
glGetSynciv glClientWaitSync glWaitSync glDeleteSync glFenceSync
glColorMaskIndexedEXT glEnableIndexedEXT glDisableIndexedEXT glGetBooleanIndexedvEXT
glUniformBufferEXT glGetUniformBufferSizeEXT glGetUniformOffsetEXT
glBufferParameteriAPPLE glFlushMappedBufferRangeAPPLE glMapBufferRange glFlushMappedBufferRange
glBufferSubData glBeginQueryARB glEndQueryARB glGetQueryObjectivARB glGetQueryObjectuivARB
glTextureRangeAPPLE glGetTexParameterPointervAPPLE
glBindFramebuffer glBindRenderbuffer glCheckFramebufferStatus glDeleteRenderbuffers
glFramebufferRenderbuffer glFramebufferTexture2D glFramebufferTexture3D glGenFramebuffers
glGenRenderbuffers glDeleteFramebuffers glBlitFramebuffer glRenderbufferStorageMultisample
glStringMarkerGREMEDY glDebugMessageCallbackARB glDebugMessageControlARB glBindMultiTextureEXT
glGenSamplers glDeleteSamplers glBindSampler glSamplerParameteri glSamplerParameterf
glSamplerParameterfv
glGetTextureHandleNV glGetTextureSamplerHandleNV glMakeTextureHandleResidentNV
glMakeTextureHandleNonResidentNV glUniformHandleui64NV glUniformHandleui64vNV
glProgramUniformHandleui64NV glProgramUniformHandleui64vNV glIsTextureHandleResidentNV
glGenQueries glDeleteQueries glBeginQuery glEndQuery glQueryCounter glGetQueryObjectiv
glGetQueryObjectui64v glCopyBufferSubData glGenVertexArrays glDeleteVertexArrays
glBindVertexArray glPushClientAttrib glPopClientAttrib
""".split()

# Écrites à la main dans runtime/gl.c (rt_gl_manual_<nom>).
MANUAL = set("""
glGetString glBufferParameteriAPPLE glMapBufferARB glUnmapBuffer glMapBufferRange glFlushMappedBufferRange
glFlushMappedBufferRangeAPPLE glDeleteBuffersARB glShaderSourceARB glFenceSync
glClientWaitSync glWaitSync glDeleteSync glGetSynciv glGetTexParameterPointervAPPLE
""".split())

# Règles des paramètres pointeurs. Expressions C sur les paramètres déjà
# convertis (préfixe a_). in/out:<taille>[:null] ; str ; array (pointeur de
# sommets : décalage si un VBO est lié) ; index:<count>:<type> ;
# unpack:<w>:<h>:<d>:<format>:<type>[:null] (d=0 : image 2D) ; unpackc:<taille> ;
# packtex:<target>:<level>:<format>:<type> ; packc:<target>:<level>.
N4 = 'in:rt_gl_n(a_n)*4'
RULES = {
    'glBindAttribLocationARB': {'name': 'str'},
    'glGetUniformLocationARB': {'name': 'str'},
    'glBufferDataARB': {'data': 'in:rt_gl_n(a_size):null'},
    'glBufferSubData': {'data': 'in:rt_gl_n(a_size)'},
    'glClipPlane': {'equation': 'in:32'},
    'glColor4sv': {'v': 'in:8'},
    'glCompressedTexImage2D': {'data': 'unpackc:a_imageSize'},
    'glCompressedTexImage3D': {'data': 'unpackc:a_imageSize'},
    'glDeleteObjectARB': {},
    'glDeleteProgramsARB': {'programs': N4},
    'glDeleteQueriesARB': {'ids': N4},
    'glDeleteQueries': {'ids': N4},
    'glDeleteTextures': {'textures': N4},
    'glDeleteRenderbuffersEXT': {'renderbuffers': N4},
    'glDeleteFramebuffersEXT': {'framebuffers': N4},
    'glDeleteRenderbuffers': {'renderbuffers': N4},
    'glDeleteFramebuffers': {'framebuffers': N4},
    'glDeleteFencesAPPLE': {'fences': N4},
    'glDeleteSamplers': {'samplers': 'in:rt_gl_n(a_count)*4'},
    'glDeleteVertexArrays': {'arrays': N4},
    'glGenBuffersARB': {'buffers': 'out' + N4[2:]},
    'glGenProgramsARB': {'programs': 'out' + N4[2:]},
    'glGenQueriesARB': {'ids': 'out' + N4[2:]},
    'glGenQueries': {'ids': 'out' + N4[2:]},
    'glGenTextures': {'textures': 'out' + N4[2:]},
    'glGenFramebuffersEXT': {'framebuffers': 'out' + N4[2:]},
    'glGenRenderbuffersEXT': {'renderbuffers': 'out' + N4[2:]},
    'glGenFramebuffers': {'framebuffers': 'out' + N4[2:]},
    'glGenRenderbuffers': {'renderbuffers': 'out' + N4[2:]},
    'glGenFencesAPPLE': {'fences': 'out' + N4[2:]},
    'glGenSamplers': {'samplers': 'out:rt_gl_n(a_count)*4'},
    'glGenVertexArrays': {'arrays': 'out' + N4[2:]},
    'glDrawBuffers': {'bufs': N4},
    'glDrawRangeElements': {'indices': 'index:a_count:a_type'},
    'glDrawRangeElementsBaseVertex': {'indices': 'index:a_count:a_type'},
    'glGetBooleanv': {'params': 'out:rt_gl_get_count(a_pname)'},
    'glGetDoublev': {'params': 'out:rt_gl_get_count(a_pname)*8'},
    'glGetFloatv': {'params': 'out:rt_gl_get_count(a_pname)*4'},
    'glGetIntegerv': {'params': 'out:rt_gl_get_count(a_pname)*4'},
    'glGetBooleanIndexedvEXT': {'values': 'out:rt_gl_get_count(a_param)'},
    'glGetCompressedTexImage': {'img': 'packc:a_target:a_lod'},
    'glGetTexImage': {'pixels': 'packtex:a_target:a_level:a_format:a_type'},
    'glGetInfoLogARB': {'length': 'out:4:null', 'infoLog': 'out:rt_gl_n(a_maxLength)'},
    'glGetObjectParameterivARB': {'params': 'out:4'},
    'glGetProgramivARB': {'params': 'out:4'},
    'glGetTexLevelParameteriv': {'params': 'out:4'},
    'glGetQueryObjectivARB': {'params': 'out:4'},
    'glGetQueryObjectuivARB': {'params': 'out:4'},
    'glGetQueryObjectiv': {'params': 'out:4'},
    'glGetQueryObjectui64v': {'params': 'out:8'},
    'glProgramStringARB': {'string': 'in:rt_gl_n(a_len)'},
    'glProgramEnvParameters4fvEXT': {'params': 'in:rt_gl_n(a_count)*16'},
    'glTexImage2D': {'pixels': 'unpack:a_width:a_height:0:a_format:a_type:null'},
    'glTexImage3D': {'pixels': 'unpack:a_width:a_height:a_depth:a_format:a_type:null'},
    'glTexSubImage2D': {'pixels': 'unpack:a_width:a_height:0:a_format:a_type'},
    'glTexParameterfv': {'params': 'in:rt_gl_texparam_count(a_pname)*4'},
    'glSamplerParameterfv': {'param': 'in:rt_gl_texparam_count(a_pname)*4'},
    'glUniform4fv': {'value': 'in:rt_gl_n(a_count)*16'},
    'glVertexAttribPointer': {'pointer': 'array'},
    'glVertexPointer': {'pointer': 'array'},
    'glTexCoordPointer': {'pointer': 'array'},
    'glTextureRangeAPPLE': {'pointer': 'in:rt_gl_n(a_length):null'},
}

KINDS = {
    'GLenum': 'u32', 'GLbitfield': 'u32', 'GLuint': 'u32',
    'GLint': 'i32', 'GLsizei': 'i32',
    'GLshort': 'i16', 'GLushort': 'u16', 'GLbyte': 'i8',
    'GLubyte': 'u8', 'GLboolean': 'u8',
    'GLfloat': 'f32', 'GLclampf': 'f32', 'GLdouble': 'f64', 'GLclampd': 'f64',
    'GLintptr': 'iptr', 'GLsizeiptr': 'iptr', 'GLintptrARB': 'iptr', 'GLsizeiptrARB': 'iptr',
    'GLuint64': 'u64', 'GLint64': 'u64', 'GLuint64EXT': 'u64',
    'GLhandleARB': 'handle', 'GLsync': 'sync',
}
CTYPE = {'u32': 'uint32_t', 'i32': 'int32_t', 'i16': 'int16_t', 'u16': 'uint16_t',
         'i8': 'int8_t', 'u8': 'uint8_t', 'f32': 'float', 'f64': 'double',
         'iptr': 'intptr_t', 'u64': 'uint64_t', 'handle': 'void *', 'sync': 'void *',
         'ptr': 'void *'}
SLOTS = {'f64': 2, 'u64': 2}


def headers():
    sdk = subprocess.run(['xcrun', '--show-sdk-path'], check=True, capture_output=True,
                         text=True).stdout.strip()
    base = Path(sdk) / 'System/Library/Frameworks/OpenGL.framework/Headers'
    protos = {}
    for name in ('gl.h', 'glext.h', 'gl3.h'):
        text = (base / name).read_text()
        for m in re.finditer(r'(?:extern|GLAPI)\s+([\w\s\*]+?)\s*(?:APIENTRY\s+)?\b(gl\w+)\s*\(([^)]*)\)', text):
            protos.setdefault(m.group(2), (m.group(1).strip(), m.group(3).strip()))
    return protos


def kind_of(decl):
    if '*' in decl:
        return 'ptr'
    words = [w for w in decl.replace('const', ' ').split()]
    if words[0] not in KINDS:
        raise SystemExit(f'type inconnu : {decl}')
    return KINDS[words[0]]


def params(text):
    if text in ('', 'void'):
        return []
    out = []
    for p in text.split(','):
        p = p.strip()
        name = re.search(r'(\w+)\s*$', p).group(1)
        decl = p[:p.rfind(name)].strip()
        out.append((kind_of(decl), name, decl))
    return out


def reader(kind, slot):
    """Scalar argument at stack slot `slot` (rt.h: rt_arg32/rt_arg64 by slot, floats by byte offset)."""
    if kind == 'f32':
        return f'rt_arg_f32(c, {4 * slot})'
    if kind == 'f64':
        return f'rt_arg_f64(c, {4 * slot})'
    if kind == 'u64':
        return f'rt_arg64(c, {slot})'
    if kind == 'iptr':
        return f'(intptr_t)(int32_t)rt_arg32(c, {slot})'
    if kind == 'handle':
        return f'rt_gl_handle_arg(rt_arg32(c, {slot}))'
    if kind == 'u32':
        return f'rt_arg32(c, {slot})'
    return f'({CTYPE[kind]})rt_arg32(c, {slot})'


def pointer(fn, name, slot):
    rule = RULES.get(fn, {}).get(name)
    if rule is None:
        raise SystemExit(f'{fn}: paramètre pointeur {name} sans règle')
    raw = f'rt_arg32(c, {slot})'
    parts = rule.split(':')
    if parts[0] in ('in', 'out'):
        null = int(len(parts) > 2 and parts[2] == 'null')
        return f'rt_gl_{parts[0]}(c, {raw}, (uint64_t)({parts[1]}), {null})'
    if parts[0] == 'str':
        return f'rt_guest_cstr(c, {raw})'
    if parts[0] == 'array':
        return f'rt_gl_array(c, {raw})'
    if parts[0] == 'index':
        return f'rt_gl_indices(c, {raw}, {parts[1]}, {parts[2]})'
    if parts[0] == 'unpack':
        null = int(len(parts) > 6 and parts[6] == 'null')
        return f'rt_gl_unpack(c, {raw}, {parts[1]}, {parts[2]}, {parts[3]}, {parts[4]}, {parts[5]}, {null})'
    if parts[0] == 'unpackc':
        return f'rt_gl_unpack_compressed(c, {raw}, {parts[1]})'
    if parts[0] == 'packtex':
        return f'rt_gl_pack_tex(c, {raw}, {parts[1]}, {parts[2]}, {parts[3]}, {parts[4]})'
    if parts[0] == 'packc':
        return f'rt_gl_pack_compressed(c, {raw}, {parts[1]}, {parts[2]})'
    raise SystemExit(f'{fn}: règle inconnue {rule}')


def thunk(fn, ret, args):
    rkind = 'void' if ret in ('void', 'GLvoid') else kind_of(ret)
    if rkind in ('ptr', 'sync', 'f32', 'f64', 'u64'):
        raise SystemExit(f'{fn}: retour {ret} à écrire à la main')
    slot, decls, names, types = 0, [], [], []
    # Les règles peuvent citer n'importe quel paramètre : lire d'abord les scalaires.
    for kind, name, _ in args:
        if kind != 'ptr':
            decls.append(f'    {CTYPE[kind]} a_{name} = {reader(kind, slot)};')
        slot += SLOTS.get(kind, 1)
    slot = 0
    for kind, name, _ in args:
        if kind == 'ptr':
            decls.append(f'    void *a_{name} = (void *){pointer(fn, name, slot)};')
        slot += SLOTS.get(kind, 1)
        names.append(f'a_{name}')
        types.append(CTYPE[kind])
    rtype = 'void' if rkind == 'void' else CTYPE[rkind]
    sig = f'{rtype} (*)({", ".join(types) or "void"})'
    call = f'(({sig})fn)({", ".join(names)})'
    lines = [f'static int g_{fn}(CPU *c, void *fn) {{'] + decls
    if not args and rkind == 'void':
        lines.append('    (void)c;')
    if rkind == 'void':
        lines.append(f'    {call};')
    elif rkind == 'u8':
        lines.append(f'    c->eax = (uint8_t){call};')
    elif rkind == 'handle':
        lines.append(f'    c->eax = rt_gl_handle_ret(c, {call});')
    elif rkind == 'iptr':
        lines.append(f'    c->eax = rt_gl_iptr_ret(c, {call});')
    else:
        lines.append(f'    c->eax = (uint32_t){call};')
    lines += ['    return 1;', '}']
    return '\n'.join(lines)


PREAMBLE = '''// GÉNÉRÉ par tools/gen_gl.py — ne pas éditer à la main.
// Thunks i386 cdecl → OpenGL hôte pour les fonctions demandées par libtogl.
// Les aides rt_gl_* (validation et traduction des pointeurs invités) sont
// déclarées dans runtime/gl_bridge.h et définies dans runtime/gl.c.
#include "gl_bridge.h"
'''


def generate():
    """(source text, generated function count, names absent from the headers)."""
    protos = headers()
    out, table, absent = [PREAMBLE], [], []
    for fn in sorted(MANUAL):
        out.append(f'int rt_gl_manual_{fn}(CPU *c, void *fn);')
    out.append('')
    for fn in sorted(set(NAMES)):
        if fn in MANUAL:
            table.append(f'    {{"{fn}", rt_gl_manual_{fn}}},')
            continue
        if fn not in protos:
            absent.append(fn)
            continue
        ret, text = protos[fn]
        out.append(thunk(fn, ret, params(text)))
        out.append('')
        table.append(f'    {{"{fn}", g_{fn}}},')
    out.append('// Absentes des en-têtes et de OpenGL.framework (rt_gl_proc_address renvoie 0) :')
    for i in range(0, len(absent), 4):
        out.append('//   ' + ' '.join(absent[i:i + 4]))
    out.append('const RtGlEntry rt_gl_entries[] = {')
    out += table
    out.append('};')
    out.append('const unsigned rt_gl_entry_count = sizeof(rt_gl_entries) / sizeof(*rt_gl_entries);')
    return '\n'.join(out) + '\n', len(table), absent


def main():
    text, count, absent = generate()
    target = Path(__file__).resolve().parent.parent / 'runtime/gl_generated.c'
    target.write_text(text)
    print(f'{target}: {count} functions, {len(absent)} absent from the headers')


if __name__ == '__main__':
    sys.exit(main())
