// SDL2 audio devices for engine.so (CAudioDeviceSDLAudio).
// SHIMS: SDL_OpenAudioDevice SDL_PauseAudioDevice SDL_CloseAudioDevice
//
// The guest SDL_AudioSpec (i386, 24 bytes) is translated field by field. The
// guest callback runs on SDL's own audio thread, attached as a foreign thread
// (rt_foreign_thread_attach), and fills a guest buffer copied to SDL's stream.
// SDL holds its device lock while running the callback, which then waits for
// the guest gate: every SDL call that can take that lock (open, pause, close)
// is made with the gate released, otherwise the two would deadlock.
// SDL_AudioDeviceID values are small integers and cross the ABI unchanged.
#include "modules.h"
#include <SDL2/SDL.h>
#include <stdlib.h>

typedef struct AudioDevice {
    SDL_AudioDeviceID id;
    uint32_t callback, userdata, buffer, capacity;
    uint8_t silence;
    int capture;
    struct AudioDevice *next;
} AudioDevice;
static AudioDevice *audio_devices;   // gate held

// Host callback, on SDL's audio thread.
static void rt_sdl_audio_trampoline(void *opaque, Uint8 *stream, int length) {
    AudioDevice *device = opaque;
    CPU *c = rt_foreign_thread_attach();
    rt_guest_enter();
    if (length < 0) rt_trap(c, device->callback, "negative SDL audio callback length");
    if ((uint32_t)length > device->capacity) {
        uint32_t grown = rt_alloc((uint32_t)length);
        if (!grown) rt_trap(c, device->callback, "cannot allocate guest audio buffer");
        rt_free(device->buffer);
        device->buffer = grown; device->capacity = (uint32_t)length;
    }
    uint8_t *guest = rt_write_span(c, device->buffer, (uint32_t)length);
    // SDL2 hands the callback a stream already filled with silence (playback).
    if (device->capture) memcpy(guest, stream, (size_t)length);
    else memset(guest, device->silence, (size_t)length);
    uint32_t args[3] = {device->userdata, device->buffer, (uint32_t)length};
    rt_invoke(c, device->callback, args, 3);
    if (!device->capture) memcpy(stream, GPTR(device->buffer), (size_t)length);
    rt_guest_leave();
}

static void read_spec(CPU *c, uint32_t address, SDL_AudioSpec *spec) {
    const uint8_t *raw = rt_span(c, address, 24);
    memset(spec, 0, sizeof(*spec));
    int32_t freq; uint16_t format, samples, padding; uint32_t size;
    memcpy(&freq, raw, 4); memcpy(&format, raw + 4, 2); memcpy(&samples, raw + 8, 2);
    memcpy(&padding, raw + 10, 2); memcpy(&size, raw + 12, 4);
    spec->freq = freq; spec->format = format; spec->channels = raw[6]; spec->silence = raw[7];
    spec->samples = samples; spec->padding = padding; spec->size = size;
}
static void write_spec(CPU *c, uint32_t address, const SDL_AudioSpec *spec, uint32_t callback, uint32_t userdata) {
    uint8_t *raw = rt_write_span(c, address, 24);
    int32_t freq = spec->freq; uint16_t format = spec->format, samples = spec->samples, padding = spec->padding;
    uint32_t size = spec->size;
    memcpy(raw, &freq, 4); memcpy(raw + 4, &format, 2); raw[6] = spec->channels; raw[7] = spec->silence;
    memcpy(raw + 8, &samples, 2); memcpy(raw + 10, &padding, 2); memcpy(raw + 12, &size, 4);
    memcpy(raw + 16, &callback, 4); memcpy(raw + 20, &userdata, 4);
}

static int audio_shim(CPU *c, const char *name) {
    if (!strcmp(name, "SDL_OpenAudioDevice")) {
        uint32_t device_name = rt_arg32(c, 0), desired_address = rt_arg32(c, 2), obtained_address = rt_arg32(c, 3);
        int capture = (int32_t)rt_arg32(c, 1) != 0;
        // Source 1 passes SDL_AUDIO_ALLOW_ANY_CHANGE, then mixes 44.1 kHz S16 stereo
        // whatever `obtained` says. With SDL2 on iOS the session runs at 48 kHz, so
        // the mix played 8.8 % too fast and too high. Allow nothing: SDL converts.
        // (sdl2-compat, used on macOS, already does this for the same reason.)
        int allowed = 0;
        char *host_name = NULL;
        if (device_name) {
            uint32_t length = rt_string_length(c, device_name);
            host_name = malloc(length + 1);
            if (!host_name) rt_trap(c, c->eip, "out of host memory");
            memcpy(host_name, GPTR(device_name), length + 1);
        }
        SDL_AudioSpec desired, obtained;
        read_spec(c, desired_address, &desired);
        uint32_t callback = R32(desired_address + 16), userdata = R32(desired_address + 20);
        AudioDevice *device = calloc(1, sizeof(*device));
        if (!device) rt_trap(c, c->eip, "out of host memory");
        device->callback = callback; device->userdata = userdata; device->capture = capture;
        desired.callback = callback ? rt_sdl_audio_trampoline : NULL;
        desired.userdata = callback ? device : NULL;
        memset(&obtained, 0, sizeof(obtained));
        unsigned saved = rt_guest_suspend();
        SDL_AudioDeviceID id = SDL_OpenAudioDevice(host_name, capture, &desired, obtained_address ? &obtained : NULL, allowed);
        rt_guest_resume(saved);
        free(host_name);
        if (!id) { free(device); c->eax = 0; return 1; }
        // Without `obtained`, SDL converts to the desired format; silence and size
        // are then derived from it as SDL_CalculateAudioSpec does (the guest
        // need not fill them in).
        if (!obtained_address) {
            obtained = desired;
            obtained.silence = desired.format == AUDIO_U8 ? 0x80 : 0;
            obtained.size = (Uint32)(SDL_AUDIO_BITSIZE(desired.format) / 8) * desired.channels * desired.samples;
        }
        device->id = id; device->silence = obtained.silence;
        device->capacity = obtained.size ? obtained.size : 4096;
        device->buffer = rt_alloc(device->capacity);
        if (!device->buffer) rt_trap(c, c->eip, "cannot allocate guest audio buffer");
        device->next = audio_devices; audio_devices = device;
        if (obtained_address) write_spec(c, obtained_address, &obtained, callback, userdata);
        c->eax = id;
    } else if (!strcmp(name, "SDL_PauseAudioDevice")) {
        SDL_AudioDeviceID id = rt_arg32(c, 0); int pause = (int32_t)rt_arg32(c, 1);
        unsigned saved = rt_guest_suspend();
        SDL_PauseAudioDevice(id, pause);
        rt_guest_resume(saved);
    } else if (!strcmp(name, "SDL_CloseAudioDevice")) {
        SDL_AudioDeviceID id = rt_arg32(c, 0);
        // Joins SDL's audio thread, which may be waiting for the gate.
        unsigned saved = rt_guest_suspend();
        SDL_CloseAudioDevice(id);
        rt_guest_resume(saved);
        AudioDevice **link = &audio_devices;
        while (*link && (*link)->id != id) link = &(*link)->next;
        if (*link) { AudioDevice *device = *link; *link = device->next; rt_free(device->buffer); free(device); }
    } else return 0;
    return 1;
}
RT_SHIM_PROVIDER(audio_shim)
