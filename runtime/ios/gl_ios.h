// OpenGL hôte sur iOS : gl4es fournit l'OpenGL 2.1 de bureau qu'attend togl,
// au-dessus d'OpenGL ES 3 (OpenGLES.framework). Inclus par runtime/host_gl.h.
//
// - Les fonctions GL viennent de gl4es_GetProcAddress ; sous Darwin, les
//   en-têtes de gl4es renomment aussi tout appel direct glX en gl4es_glX.
// - CGL (contexte courant, groupe de partage) est remplacé par EAGL.
// - SDL_GL_CreateContext force un contexte ES 3 (16 unités de texture ; le
//   GLSL ES 1.00 de gl4es est réécrit en 3.00, essl3.c) puis initialise gl4es ;
//   SDL_GL_SwapWindow vide d'abord le tampon de gl4es (gl4es_pre_swap).
#pragma once
#include <GL/gl.h>
#include <GL/glext.h>
#include <SDL.h>
#include "sdl_ios.h"   // tailles en pixels pour le jeu

typedef void *CGLContextObj;
typedef void *CGLShareGroupObj;
void *rt_ios_gl_current_context(void);
void *rt_ios_gl_share_group(void *context);
#define CGLGetCurrentContext rt_ios_gl_current_context
#define CGLGetShareGroup rt_ios_gl_share_group

extern _Thread_local int rt_ios_gl_unsync_next;   // gl.c -> gl_ios.m : prochain glBufferSubData d'ES sans synchronisation
void *rt_host_gl_open(void);
void *rt_host_gl_symbol(void *library, const char *name);

SDL_GLContext rt_ios_gl_create_context(SDL_Window *window);
void rt_ios_gl_swap_window(SDL_Window *window);
#define SDL_GL_CreateContext rt_ios_gl_create_context
#define SDL_GL_SwapWindow rt_ios_gl_swap_window
