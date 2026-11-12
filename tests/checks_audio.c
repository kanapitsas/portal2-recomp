// SDL audio (rappel invité depuis le thread audio de SDL) et pont OpenAL.
#include "checks.h"
#include <SDL2/SDL.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define DATA 0x2001d000u
#define AUDIO_FILE "build/sdl-audio-check.raw"
static uint32_t call(CPU *c, const char *name, const uint32_t *args, unsigned count) {
    uint32_t esp = c->esp; c->esp -= 64;
    for (unsigned i = 0; i < count; ++i) W32(c->esp + 4 + 4 * i, args[i]);
    assert(rt_shim(c, name));
    c->esp = esp;
    return c->eax;
}
static void sleep_ms(unsigned ms) { unsigned saved = rt_guest_suspend(); usleep(ms * 1000); rt_guest_resume(saved); }

static atomic_int callbacks;
static uint32_t cb_userdata, cb_length, cb_stream, cb_tid;
static int cb_silent = 1, cb_main_thread;
static uint8_t cb_expected_silence;
static pthread_t main_thread;
static int audio_probe(CPU *c, const char *name) {
    if (strcmp(name, "__test_audio_callback")) return 0;
    cb_userdata = R32(c->esp + 4); cb_stream = R32(c->esp + 8); cb_length = R32(c->esp + 12);
    cb_tid = c->tid;
    if (pthread_equal(pthread_self(), main_thread)) cb_main_thread = 1;
    for (uint32_t i = 0; i < cb_length; ++i) if (R8(cb_stream + i) != cb_expected_silence) cb_silent = 0;
    memset(rt_write_span(c, cb_stream, cb_length), 0x5a, cb_length);   // le flux joué
    atomic_fetch_add(&callbacks, 1);
    return 1;
}
RT_SHIM_PROVIDER(audio_probe)

RT_CHECK(sdl_audio_guest_callback) {
    // Pilote « disk » : le flux produit par le rappel invité est écrit dans un fichier.
    setenv("SDL_AUDIODRIVER", "disk", 1);
    setenv("SDL_AUDIO_DISK_OUTPUT_FILE", AUDIO_FILE, 1);
    setenv("SDL_DISKAUDIOFILE", AUDIO_FILE, 1);
    remove(AUDIO_FILE);
    assert(SDL_InitSubSystem(SDL_INIT_AUDIO) == 0);
    assert(!strcmp(SDL_GetCurrentAudioDriver(), "disk"));
    main_thread = pthread_self();
    CPU c; rt_cpu_reset(&c); c.esp = 0x2001b000; c.tid = 1;
    rt_guest_enter();
    uint32_t desired = DATA, obtained = DATA + 0x40;
    memset(GPTR(desired), 0, 24); memset(GPTR(obtained), 0xcc, 24);
    W32(desired, 44100); W16(desired + 4, AUDIO_S16LSB); W8(desired + 6, 2); W16(desired + 8, 512);
    W32(desired + 16, rt_dynamic_import("__test_audio_callback")); W32(desired + 20, 0x1234);
    // Source 1 asks for SDL_AUDIO_ALLOW_ANY_CHANGE and ignores `obtained`: the shim allows none.
    uint32_t args[5] = {0, 0, desired, obtained, SDL_AUDIO_ALLOW_ANY_CHANGE};
    uint32_t id = call(&c, "SDL_OpenAudioDevice", args, 5);
    assert(id >= 2);
    assert(R32(obtained) == 44100 && R16(obtained + 4) == AUDIO_S16LSB && R8(obtained + 6) == 2);
    assert(R8(obtained + 7) == 0 && R16(obtained + 8) == 512 && R32(obtained + 12) == 2048);
    assert(R32(obtained + 16) == R32(desired + 16) && R32(obtained + 20) == 0x1234);
    sleep_ms(30); assert(atomic_load(&callbacks) == 0);       // ouvert en pause
    args[0] = id; args[1] = 0; call(&c, "SDL_PauseAudioDevice", args, 2);
    struct timespec start, now; clock_gettime(CLOCK_MONOTONIC, &start);
    while (atomic_load(&callbacks) < 4) {
        sleep_ms(2); clock_gettime(CLOCK_MONOTONIC, &now); assert(now.tv_sec - start.tv_sec < 5);
    }
    args[1] = 1; call(&c, "SDL_PauseAudioDevice", args, 2);
    call(&c, "SDL_CloseAudioDevice", args, 1);
    int final = atomic_load(&callbacks);
    sleep_ms(30); assert(atomic_load(&callbacks) == final);
    assert(cb_userdata == 0x1234 && cb_length == 2048 && cb_silent && !cb_main_thread && cb_tid > 1);
    assert(cb_stream >= 0x80000000u);                          // tampon dans le tas invité
    rt_guest_leave();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    // Le fichier contient ce que le code invité a écrit, entouré du silence que
    // le pilote disk émet tant que le périphérique est en pause.
    FILE *file = fopen(AUDIO_FILE, "rb"); assert(file);
    uint8_t block[4096]; size_t played = 0, read;
    while ((read = fread(block, 1, sizeof(block), file)) > 0)
        for (size_t i = 0; i < read; ++i) { assert(block[i] == 0 || block[i] == 0x5a); played += block[i] == 0x5a; }
    fclose(file); remove(AUDIO_FILE);
    assert(played >= 2048u * (unsigned)(final - 1));

    // U8 sans `obtained` : silence (0x80) et taille calculés comme SDL, même si
    // le code invité laisse ces champs à zéro.
    assert(SDL_InitSubSystem(SDL_INIT_AUDIO) == 0);
    rt_guest_enter();
    memset(GPTR(desired), 0, 24);
    W32(desired, 44100); W16(desired + 4, AUDIO_U8); W8(desired + 6, 1); W16(desired + 8, 256);
    W32(desired + 16, rt_dynamic_import("__test_audio_callback")); W32(desired + 20, 0x5678);
    atomic_store(&callbacks, 0); cb_silent = 1; cb_expected_silence = 0x80;
    args[0] = 0; args[1] = 0; args[2] = desired; args[3] = 0; args[4] = 0;
    id = call(&c, "SDL_OpenAudioDevice", args, 5);
    assert(id >= 2);
    args[0] = id; args[1] = 0; call(&c, "SDL_PauseAudioDevice", args, 2);
    clock_gettime(CLOCK_MONOTONIC, &start);
    while (atomic_load(&callbacks) < 2) {
        sleep_ms(2); clock_gettime(CLOCK_MONOTONIC, &now); assert(now.tv_sec - start.tv_sec < 5);
    }
    call(&c, "SDL_CloseAudioDevice", args, 1);
    assert(cb_silent && cb_userdata == 0x5678 && cb_length == 256);
    rt_guest_leave();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    remove(AUDIO_FILE);
    unsetenv("SDL_AUDIODRIVER"); unsetenv("SDL_AUDIO_DISK_OUTPUT_FILE"); unsetenv("SDL_DISKAUDIOFILE");
}

RT_CHECK(openal_bridge) {
    CPU c; rt_cpu_reset(&c); c.esp = 0x2001b000;
    rt_guest_enter();
    uint32_t text = DATA + 0x100, names = DATA + 0x200, value = DATA + 0x300, attrs = DATA + 0x340, pcm = DATA + 0x400;
    uint32_t args[5] = {0};
    // Chemins sans périphérique.
    args[0] = 0x12345678; assert(call(&c, "alcCloseDevice", args, 1) == 0);
    assert(call(&c, "alcMakeContextCurrent", args, 1) == 0);
    assert(call(&c, "alcGetContextsDevice", args, 1) == 0);
    strcpy(GPTR(text), "alSourcePlay"); args[0] = text;
    uint32_t play = call(&c, "alGetProcAddress", args, 1);
    assert(play >= RT_DYNAMIC_IMPORT_BASE && !strcmp(rt_dynamic_import_name(play), "alSourcePlay"));
    strcpy(GPTR(text), "alcSetThreadContext"); assert(call(&c, "alGetProcAddress", args, 1) == 0);
    strcpy(GPTR(text), "AL_FORMAT_STEREO16"); assert(call(&c, "alGetEnumValue", args, 1) == AL_FORMAT_STEREO16);
    args[0] = 0; uint32_t device = call(&c, "alcOpenDevice", args, 1);
    if (!device) {
        fprintf(stderr, "openal_bridge: no OpenAL output device, only error paths checked\n");
        rt_guest_leave(); return;
    }
    assert(device >= RT_HANDLE_BASE);
    W32(attrs, ALC_FREQUENCY); W32(attrs + 4, 44100); W32(attrs + 8, 0);
    args[0] = device; args[1] = attrs; uint32_t context = call(&c, "alcCreateContext", args, 2);
    assert(context >= RT_HANDLE_BASE && context != device);
    args[0] = context; assert(call(&c, "alcMakeContextCurrent", args, 1) == 1);
    assert(call(&c, "alcGetCurrentContext", NULL, 0) == context);
    assert(call(&c, "alcGetContextsDevice", args, 1) == device);
    while (call(&c, "alGetError", NULL, 0) != AL_NO_ERROR) {}
    args[0] = 3; args[1] = names; memset(GPTR(names), 0, 16);
    call(&c, "alGenBuffers", args, 2);
    assert(call(&c, "alGetError", NULL, 0) == AL_NO_ERROR);
    uint32_t b0 = R32(names), b1 = R32(names + 4), b2 = R32(names + 8);
    assert(b0 && b1 && b2 && b0 != b1 && b1 != b2 && !R32(names + 12));
    args[0] = 1; args[1] = names + 12; call(&c, "alGenSources", args, 2);
    uint32_t source = R32(names + 12);
    assert(call(&c, "alGetError", NULL, 0) == AL_NO_ERROR && source);
    memset(GPTR(pcm), 0, 1024);
    for (unsigned i = 0; i < 3; ++i) {
        uint32_t data_args[5] = {R32(names + 4 * i), AL_FORMAT_STEREO16, pcm, 1024, 22050};
        call(&c, "alBufferData", data_args, 5);
    }
    assert(call(&c, "alGetError", NULL, 0) == AL_NO_ERROR);
    args[0] = source; args[1] = 3; args[2] = names; call(&c, "alSourceQueueBuffers", args, 3);
    args[1] = AL_BUFFERS_QUEUED; args[2] = value; call(&c, "alGetSourcei", args, 3);
    assert(R32(value) == 3 && call(&c, "alGetError", NULL, 0) == AL_NO_ERROR);
    args[0] = source; call(&c, "alSourcePlay", args, 1);
    assert(call(&c, "alGetError", NULL, 0) == AL_NO_ERROR);
    // Appel par l'adresse renvoyée par alGetProcAddress, comme le ferait Miles.
    PUSH(&c, source); rt_call(&c, play, RT_RETURN_SENTINEL); c.esp += 4;
    call(&c, "alSourcePause", args, 1); call(&c, "alSourceStop", args, 1);
    args[1] = AL_SOURCE_STATE; args[2] = value; call(&c, "alGetSourcei", args, 3);
    assert(R32(value) == AL_STOPPED);
    args[1] = AL_BUFFERS_PROCESSED; call(&c, "alGetSourcei", args, 3);
    assert(R32(value) == 3);
    memset(GPTR(names + 16), 0, 12);
    args[1] = 3; args[2] = names + 16; call(&c, "alSourceUnqueueBuffers", args, 3);
    assert(call(&c, "alGetError", NULL, 0) == AL_NO_ERROR);
    assert(R32(names + 16) == b0 && R32(names + 20) == b1 && R32(names + 24) == b2);
    // AL_BUFFER sur une source neuve (OpenAL d'Apple rend 0 sur une source de flux).
    args[0] = 1; args[1] = names + 28; call(&c, "alGenSources", args, 2);
    uint32_t still = R32(names + 28);
    args[0] = still; args[1] = AL_BUFFER; args[2] = b1; call(&c, "alSourcei", args, 3);
    args[2] = value; call(&c, "alGetSourcei", args, 3);
    assert(R32(value) == b1 && call(&c, "alGetError", NULL, 0) == AL_NO_ERROR);
    args[2] = 0; call(&c, "alSourcei", args, 3);
    args[0] = 1; args[1] = names + 28; call(&c, "alDeleteSources", args, 2);
    // Erreurs AL réelles de l'implémentation hôte.
    args[0] = 0xdeadbeef; args[1] = AL_SOURCE_STATE; args[2] = value; W32(value, 0x77);
    call(&c, "alGetSourcei", args, 3);
    assert(call(&c, "alGetError", NULL, 0) == AL_INVALID_NAME && R32(value) == 0x77);
    args[0] = b0; args[1] = AL_FORMAT_STEREO16; args[2] = pcm; args[3] = (uint32_t)-4; args[4] = 22050;
    call(&c, "alBufferData", args, 5);
    assert(call(&c, "alGetError", NULL, 0) != AL_NO_ERROR);
    args[0] = 1; args[1] = names + 12; call(&c, "alDeleteSources", args, 2);
    args[0] = 3; args[1] = names; call(&c, "alDeleteBuffers", args, 2);
    assert(call(&c, "alGetError", NULL, 0) == AL_NO_ERROR);
    args[0] = 0; assert(call(&c, "alcMakeContextCurrent", args, 1) == 1);
    assert(call(&c, "alcGetCurrentContext", NULL, 0) == 0);
    args[0] = context; call(&c, "alcDestroyContext", args, 1);
    assert(call(&c, "alcGetContextsDevice", args, 1) == 0);    // handle libéré
    args[0] = device; assert(call(&c, "alcCloseDevice", args, 1) == 1);
    assert(call(&c, "alcCloseDevice", args, 1) == 0);
    rt_guest_leave();
}
