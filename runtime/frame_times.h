// Per-present frame log for renderer benchmarks (tools/bench_renderer.py).
//
// P2_FRAME_TIMES=<path> writes one line per present, on both renderers:
//   <r> <start_ns> <present_ns> <thread_cpu_ns>
// r: 'g' (SDL_GL_SwapWindow, OpenGL/togl) or 'd' (IDirect3DDevice9::Present
// or IDirect3DSwapChain9::Present, Direct3D 9 bridge); start_ns:
// CLOCK_UPTIME_RAW when the guest called present (frame time = difference
// between two starts); present_ns: time spent inside the call; thread_cpu_ns:
// CPU time of the presenting thread at the end of the call
// (CLOCK_THREAD_CPUTIME_ID). The first line is a "# p2-frame-times v1 ..."
// header. Lines are buffered and written about once a second, so a killed
// process loses at most the last second.
// Off (variable unset): rt_frame_times_enabled() is one predictable branch;
// the Direct3D 9 bridge checks it when it resolves a Present route, so its
// hot path does not change at all.
#pragma once
#include <stdint.h>

int rt_frame_times_enabled(void);
uint64_t rt_frame_times_now(void);                      // CLOCK_UPTIME_RAW, ns
void rt_frame_times_record(char renderer, uint64_t start);
// Tests: (re)open the log at path (NULL or "" turns it off), flush it, and
// count the presents recorded since the process started.
int rt_frame_times_open(const char *path);
void rt_frame_times_flush(void);
uint64_t rt_frame_times_count(void);
