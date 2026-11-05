# OpenGL audit of `libtogl.so` (Linux i386) against Apple's OpenGL (25 October 2026)

This audit predates rendering. Its conclusions held: togl runs on the legacy
2.1 context and is the OpenGL path of the project. The default renderer is
now the Direct3D 9 bridge of [d3d9-bridge.md](d3d9-bridge.md).

Sources: disassembly of `bin/linux32/libtogl.so` (the `COpenGLEntryPoints`
constructor at 0x3b050–0x3d820, called by `GetOpenGLEntryPoints`) and of
`launcher.so`; measurements on this Mac (M3 Pro, macOS 27.0.1) with CGL and SDL
(sdl2-compat from /opt/homebrew). Reproducible method: each call to
`VoidFnPtrLookup(name, &okay, required, fallback)` (0x3abd0) gives the name, the
`okay` flag (= the extension, or the "OpenGL" group) and whether it is required.

## How togl obtains GL

- `GetOpenGLEntryPoints(lookup)` receives the launcher's callback (0x36960 in
  `launcher.so`), which calls `SDL_GL_GetProcAddress(name)`. There are no static
  GL imports. **194 names** are requested (plus `glXGetCurrentDisplay` and
  `glXQueryExtensionsString`, see below).
- Version: `glGetString(GL_VERSION)` is parsed as `major.minor.patch`; it must be
  **≥ 2.0.0**, otherwise it prints "PROBLEM: You appear to have OpenGL…" and
  fails.
- Extensions: `strstr` on `glGetString(GL_EXTENSIONS)` (never
  `glGetStringi`), falling back to GLX if the name is missing. Some also accept
  a version instead (ARB_sync ≥ 3.2, ARB_framebuffer_object ≥ 3.0).
- Fatal: if **even one** of the 124 functions in the "OpenGL" group returns
  NULL, `Error("Missing basic required OpenGL functionality.")`. The 70
  extension functions only clear their extension's flag.
- Context: the launcher only sets RGBA8, double buffering, 24-bit depth,
  8-bit stencil, acceleration (and `CONTEXT_FLAGS=DEBUG` behind an option).
  **No version or profile is requested** → on macOS, SDL creates a **legacy
  2.1** context (measured: "2.1 Metal - 91.7", GLSL 1.20).
- GLSL shaders are generated as `#version 120` + `GL_ARB_shader_texture_lod`.
  `#version 330 compatibility` is only chosen for shaders flagged 0x1000 (the
  bindless path, unreachable without `GL_NV_bindless_texture` — inferred, not
  verified at runtime).

## Apple profiles

| | legacy 2.1 | core 3.2/4.1 |
|---|---|---|
| `glGetString(GL_EXTENSIONS)` | yes | **NULL + GL_INVALID_ENUM** |
| immediate mode/fixed function (`glBegin`, `glAlphaFunc`, `glPushAttrib`, `glClipPlane`, client state) | yes | no |
| ARB_vertex/fragment_program, ARB_shader_objects objects | yes | no |
| GLSL 1.20 | yes | no (≥ 1.40/1.50) |
| sampler objects, ARB timer query, ARB VAO, copy_buffer, map_buffer_range | no | yes |

The core profile is **ruled out**: togl relies heavily on the fixed-function
pipeline, ARB programs, `GL_EXTENSIONS` and GLSL 1.20. It needs the legacy
profile, just like the original Mac port.

## Extensions checked by togl, on legacy 2.1 (Metal, M3 Pro)

Present: EXT/ARB_framebuffer_object, EXT_framebuffer_blit,
EXT_framebuffer_multisample(+_blit_scaled), **APPLE_fence, ARB_sync**,
EXT_draw_buffers2, EXT_bindable_uniform, APPLE_flush_buffer_range,
ARB_vertex_buffer_object, ARB_occlusion_query, APPLE_texture_range,
APPLE_client_storage, ARB/EXT_vertex_array_bgra, **EXT_texture_compression_s3tc
+ dxt1**, **EXT_texture_sRGB_decode**, ARB_draw_elements_base_vertex,
EXT_gpu_program_parameters, EXT_timer_query, ARB_shader_texture_lod.

Missing (all optional): NV_fence, ARB_map_buffer_range,
ARB_debug_output, GREMEDY_string_marker, EXT_direct_state_access,
NV_bindless_texture, AMD_pinned_memory, NVX_gpu_memory_info, ATI_meminfo,
ANGLE_dxt3/5, GLX_EXT_swap_control_tear.

togl's three blocking checks (NV/ARB/APPLE fence; S3TC;
sRGB_decode) **pass** on legacy 2.1.

## Functions: measured presence and behavior

- `dlsym(OpenGL.framework)` (what SDL does) finds **176/194** names. The 18
  missing ones are all optional: 5 `*FenceNV`, `glStringMarkerGREMEDY`,
  `glDebugMessageCallbackARB`/`ControlARB`, `glBindMultiTextureEXT`, and 9
  NV_bindless functions. `glX*`: missing (togl only falls back to them when an
  extension is absent from `GL_EXTENSIONS`; it then prints "Could not find
  required OpenGL entry point 'glXGetCurrentDisplay'" without failing).
- On macOS, `SDL_GL_GetProcAddress` returns a pointer even for 3.x
  functions (measured for `glGenSamplers`). The required group is therefore
  "present", but in a legacy context these functions return
  **GL_INVALID_OPERATION** and do nothing (measured):
  - `glGenSamplers/DeleteSamplers/BindSampler/SamplerParameter*` —
    `glGenSamplers` is called 512 times (a loop from 0x550 to 0x1d50, stride 0xc) during context init (0x4528b); the
    names stay 0. `glBindSampler`/parameters are only used with
    `-gl_enablesamplerobjects` (context flag 0x5c, off by
    default). **Not blocking**; the leftover GL error is only read by
    the single diagnostic `glGetError` (0x4127c, behind a debug flag).
  - `glGenVertexArrays/Delete/Bind`: loaded but never called
    directly (no `call *0x378/0x37c/0x380` sites).
  - `glCopyBufferSubData`: a single call site, targeting `GL_PINNED_MEMORY_AMD`
    (0x9160), so unreachable without AMD_pinned_memory.
  - `glQueryCounter`: only in `GLMBeginPIXEvent`/telemetry (flag
    0x4d5240). `glGetQueryObjectui64v` works on legacy (EXT_timer_query).
  - `glMapBufferRange`/`glFlushMappedBufferRange`: optional; togl
    falls back to `glMapBufferARB` + APPLE_flush_buffer_range.
- The other required functions (ARB programs, `glProgramEnvParameters4fvEXT`,
  `glDrawRangeElementsBaseVertex`, `glColorMaskIndexedEXT`…) are covered
  by the legacy extensions listed above.

Usage method: search for indirect calls `call *OFF(%reg)` at the offset of
each pointer in the `gGL` struct; this is a lower bound (a load followed by
`call *%reg` slips past the search). The "not called" conclusions above were
re-checked by hand.

## Conclusion

**Nothing on Apple's OpenGL side blocks togl's initialization**, provided
there is a legacy 2.1 context (SDL's default behavior) and an
`rt_gl_proc_address` that, like SDL on macOS, returns an address for
every symbol exported by OpenGL.framework (otherwise `glGenSamplers` etc.
are missing ⇒ fatal error). Remaining risks, which cannot be checked before
actual rendering: bugs in the legacy Metal driver (2.1 has been unmaintained
since 10.14), performance (no map_buffer_range, the bridge copying mapped
buffers), and the specific GLSL/ARB paths togl generates.

## Resulting requirements for the bridge

- Client pointers: togl calls `glVertexAttribPointer` **with no VBO bound**
  and a static guest array (0x40f75); the bridge reads the current binding
  and only translates in that case. Same for indices/PBOs.
- `GLhandleARB` is `void *` on macOS and `unsigned int` on Linux; the
  measured values are small integers (1, 3).
- Alternative, not pursued on 25 October: `libdxvk_d3d9.so` +
  `shaderapivk.so` (`-vulkan`) exist in the Linux depot, but DXVK would
  require Vulkan (MoltenVK, a new dependency). Since then `shaderapivk.so`
  is used without Vulkan: its Direct3D 9 calls go to the native mtld3d
  library ([d3d9-bridge.md](d3d9-bridge.md)), which is the default path.
