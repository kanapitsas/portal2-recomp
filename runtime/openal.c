// OpenAL 1.1 bridge for libMiles.so (guest) onto OpenAL.framework.
// SHIMS: alGenBuffers alDeleteBuffers alBufferData alGenSources alDeleteSources
// SHIMS: alSourcei alGetSourcei alSourcePlay alSourcePause alSourceStop
// SHIMS: alSourceQueueBuffers alSourceUnqueueBuffers alGetError alGetEnumValue alGetProcAddress
// SHIMS: alcOpenDevice alcCloseDevice alcCreateContext alcDestroyContext
// SHIMS: alcMakeContextCurrent alcGetCurrentContext alcGetContextsDevice
//
// AL object names (ALuint) and enums are plain integers shared by the guest and
// the host. ALCdevice*/ALCcontext* are host pointers: they cross the ABI as
// typed handles. alGetProcAddress only returns functions bridged here (as
// rt_dynamic_import addresses); anything else is reported absent (NULL), so the
// guest takes its fallback path instead of jumping to an unbridged function.
// All entry points are cdecl on Linux i386; ALCboolean is returned in AL.
#include "rt.h"
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <stdlib.h>

#define KIND_DEVICE 0x414c4344u   // 'ALCD'
#define KIND_CONTEXT 0x414c4358u  // 'ALCX'
#define MAX_NAMES 4096            // objects per alGen/alDelete call

static const char *const bridged[] = {
    "alGenBuffers", "alDeleteBuffers", "alBufferData", "alGenSources", "alDeleteSources",
    "alSourcei", "alGetSourcei", "alSourcePlay", "alSourcePause", "alSourceStop",
    "alSourceQueueBuffers", "alSourceUnqueueBuffers", "alGetError", "alGetEnumValue", "alGetProcAddress",
    "alcOpenDevice", "alcCloseDevice", "alcCreateContext", "alcDestroyContext",
    "alcMakeContextCurrent", "alcGetCurrentContext", "alcGetContextsDevice",
};
static uint32_t argument(CPU *c, unsigned i) { return R32(c->esp + 4 + 4 * i); }
static const char *guest_string(CPU *c, uint32_t address) { rt_string_length(c, address); return GPTR(address); }

// Names read from / written to a guest ALuint array of n entries. A negative
// n is passed to the host with no array so it sets the AL error; n above
// MAX_NAMES is returned as is and the caller traps (bridge limit).
static ALsizei name_count(CPU *c, uint32_t n, uint32_t array) {
    int32_t count = (int32_t)n;
    if (count <= 0 || count > MAX_NAMES) return count;
    rt_span(c, array, (uint64_t)count * 4);
    return count;
}
static void gen_or_delete(CPU *c, int generate, int sources) {
    uint32_t array = argument(c, 1);
    ALsizei n = name_count(c, argument(c, 0), array);
    if (n > MAX_NAMES) rt_trap(c, c->eip, "alGen/alDelete count above bridge limit");
    ALuint *buffer = n > 0 ? calloc((size_t)n, sizeof(ALuint)) : NULL;
    if (n > 0 && !buffer) rt_trap(c, c->eip, "out of host memory");
    if (generate) {
        if (n > 0) rt_write_span(c, array, (uint64_t)n * 4);
        if (sources) alGenSources(n, buffer); else alGenBuffers(n, buffer);
        if (n > 0) memcpy(GPTR(array), buffer, (size_t)n * 4);
    } else {
        if (n > 0) memcpy(buffer, GPTR(array), (size_t)n * 4);
        if (sources) alDeleteSources(n, buffer); else alDeleteBuffers(n, buffer);
    }
    free(buffer);
}

static int openal_shim(CPU *c, const char *name) {
    if (name[0] != 'a' || name[1] != 'l') return 0;
    if (!strcmp(name, "alGenBuffers")) gen_or_delete(c, 1, 0);
    else if (!strcmp(name, "alDeleteBuffers")) gen_or_delete(c, 0, 0);
    else if (!strcmp(name, "alGenSources")) gen_or_delete(c, 1, 1);
    else if (!strcmp(name, "alDeleteSources")) gen_or_delete(c, 0, 1);
    else if (!strcmp(name, "alBufferData")) {
        uint32_t data = argument(c, 2); int32_t size = (int32_t)argument(c, 3);
        // A negative size reaches the host unchanged so that it reports AL_INVALID_VALUE.
        const void *host = data && size > 0 ? rt_span(c, data, (uint32_t)size) : NULL;
        alBufferData(argument(c, 0), (ALenum)argument(c, 1), host, size, (ALsizei)argument(c, 4));
    } else if (!strcmp(name, "alSourcei")) alSourcei(argument(c, 0), (ALenum)argument(c, 1), (ALint)argument(c, 2));
    else if (!strcmp(name, "alGetSourcei")) {
        uint32_t output = argument(c, 2);
        if (!output) alGetSourcei(argument(c, 0), (ALenum)argument(c, 1), NULL);
        else {
            rt_write_span(c, output, 4);
            ALint value = (ALint)R32(output);   // left unchanged by the host on error
            alGetSourcei(argument(c, 0), (ALenum)argument(c, 1), &value);
            W32(output, (uint32_t)value);
        }
    } else if (!strcmp(name, "alSourcePlay")) alSourcePlay(argument(c, 0));
    else if (!strcmp(name, "alSourcePause")) alSourcePause(argument(c, 0));
    else if (!strcmp(name, "alSourceStop")) alSourceStop(argument(c, 0));
    else if (!strcmp(name, "alSourceQueueBuffers") || !strcmp(name, "alSourceUnqueueBuffers")) {
        int queue = !strcmp(name, "alSourceQueueBuffers");
        uint32_t source = argument(c, 0), array = argument(c, 2);
        ALsizei n = name_count(c, argument(c, 1), array);
        if (n > MAX_NAMES) rt_trap(c, c->eip, "alSource(Un)QueueBuffers count above bridge limit");
        ALuint *buffer = n > 0 ? calloc((size_t)n, sizeof(ALuint)) : NULL;
        if (n > 0 && !buffer) rt_trap(c, c->eip, "out of host memory");
        if (queue) {
            if (n > 0) memcpy(buffer, GPTR(array), (size_t)n * 4);
            alSourceQueueBuffers(source, n, buffer);
        } else {
            if (n > 0) { rt_write_span(c, array, (uint64_t)n * 4); memcpy(buffer, GPTR(array), (size_t)n * 4); }
            alSourceUnqueueBuffers(source, n, buffer);
            if (n > 0) memcpy(GPTR(array), buffer, (size_t)n * 4);
        }
        free(buffer);
    } else if (!strcmp(name, "alGetError")) c->eax = (uint32_t)alGetError();
    else if (!strcmp(name, "alGetEnumValue")) c->eax = (uint32_t)alGetEnumValue(guest_string(c, argument(c, 0)));
    else if (!strcmp(name, "alGetProcAddress")) {
        const char *wanted = guest_string(c, argument(c, 0));
        c->eax = 0;
        for (size_t i = 0; i < sizeof(bridged) / sizeof(*bridged); ++i)
            if (!strcmp(bridged[i], wanted)) c->eax = rt_dynamic_import(wanted);
    } else if (!strcmp(name, "alcOpenDevice")) {
        uint32_t device_name = argument(c, 0);
        ALCdevice *device = alcOpenDevice(device_name ? guest_string(c, device_name) : NULL);
        c->eax = device ? rt_handle_new(device, KIND_DEVICE) : 0;
        if (device && !c->eax) { alcCloseDevice(device); rt_trap(c, c->eip, "handle table full"); }
    } else if (!strcmp(name, "alcCloseDevice")) {
        uint32_t handle = argument(c, 0);
        ALCdevice *device = rt_handle_get(handle, KIND_DEVICE);
        ALCboolean closed = device ? alcCloseDevice(device) : ALC_FALSE;
        if (closed) rt_handle_free(handle, KIND_DEVICE);
        c->eax = closed ? 1 : 0;
    } else if (!strcmp(name, "alcCreateContext")) {
        ALCdevice *device = rt_handle_get(argument(c, 0), KIND_DEVICE);
        uint32_t list = argument(c, 1);
        ALCint attributes[129];
        size_t count = 0;
        if (list) {
            // Zero-terminated key/value pairs.
            for (;;) {
                if (count >= 128) rt_trap(c, c->eip, "ALC attribute list longer than 64 pairs");
                ALCint key = (ALCint)R32(list + (uint32_t)count * 4);
                attributes[count++] = key;
                if (!key) break;
                attributes[count] = (ALCint)R32(list + (uint32_t)count * 4); ++count;
            }
        }
        ALCcontext *context = device ? alcCreateContext(device, list ? attributes : NULL) : NULL;
        c->eax = context ? rt_handle_new(context, KIND_CONTEXT) : 0;
        if (context && !c->eax) { alcDestroyContext(context); rt_trap(c, c->eip, "handle table full"); }
    } else if (!strcmp(name, "alcDestroyContext")) {
        uint32_t handle = argument(c, 0);
        ALCcontext *context = rt_handle_get(handle, KIND_CONTEXT);
        if (context) {
            alcDestroyContext(context);
            // Apple's OpenAL refuses to destroy the current context.
            if (alcGetCurrentContext() != context) rt_handle_free(handle, KIND_CONTEXT);
        }
    } else if (!strcmp(name, "alcMakeContextCurrent")) {
        uint32_t handle = argument(c, 0);
        ALCcontext *context = handle ? rt_handle_get(handle, KIND_CONTEXT) : NULL;
        c->eax = handle && !context ? 0 : alcMakeContextCurrent(context) ? 1 : 0;
    } else if (!strcmp(name, "alcGetCurrentContext")) {
        ALCcontext *context = alcGetCurrentContext();
        c->eax = context ? rt_handle_find(context, KIND_CONTEXT) : 0;
        if (context && !c->eax) rt_trap(c, c->eip, "current OpenAL context was not created by the guest");
    } else if (!strcmp(name, "alcGetContextsDevice")) {
        ALCcontext *context = rt_handle_get(argument(c, 0), KIND_CONTEXT);
        ALCdevice *device = context ? alcGetContextsDevice(context) : NULL;
        c->eax = device ? rt_handle_find(device, KIND_DEVICE) : 0;
    } else return 0;
    return 1;
}
RT_SHIM_PROVIDER(openal_shim)
