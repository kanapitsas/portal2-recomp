#include "modules.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Programme principal du jeu (tools/games.py) ; build_game.py le définit.
#ifndef RT_GAME_PROGRAM
#define RT_GAME_PROGRAM "portal2_linux"
#endif

static uint32_t guest_string(const char *value) {
    size_t length = strlen(value) + 1;
    uint32_t address = rt_alloc((uint32_t)length);
    if (!address) { fputs("guest argument allocation failed\n", stderr); exit(2); }
    memcpy(GPTR(address), value, length);
    return address;
}
int rt_d3d9_available(void);   // runtime/d3d9.c

int rt_renderer_request(const char *value);   // runtime/d3d9.c

// Rendu Direct3D 9 natif par défaut : le jeu prend alors shaderapivk.so
// (-vulkan), dont le Direct3D 9 vient de libd3d9metal (runtime/d3d9.c).
// La bibliothèque est chargée et initialisée ici : absente ou refusant
// d3d9metal_init, on reste en OpenGL plutôt que d'échouer au démarrage.
// P2_RENDERER=opengl force OpenGL ; P2_RENDERER=d3d9 le signale s'il manque.
int rt_renderer_plan(int request, int vulkan_given, int (*available)(void));   // runtime/d3d9.c
static int wants_d3d9(int first, int argc, char **argv) {
    int request = rt_renderer_request(getenv("P2_RENDERER")), given = 0;
    for (int i = first; i < argc; ++i) if (!strcmp(argv[i], "-vulkan")) given = 1;
    int plan = rt_renderer_plan(request, given, rt_d3d9_available);
    if (plan > 0) fputs("renderer: Direct3D 9 (native library), adding -vulkan\n", stderr);
    else if (plan < 0) fputs("renderer: OpenGL (P2_RENDERER), ignoring -vulkan\n", stderr);
    else if (request < 0) fputs("renderer: OpenGL (P2_RENDERER)\n", stderr);
    else if (!given) fputs(request > 0 ? "renderer: P2_RENDERER=d3d9 but libd3d9metal is missing or failed to initialize, staying on OpenGL\n"
                                       : "renderer: OpenGL (no usable libd3d9metal)\n", stderr);
    return plan;
}

int main(int argc, char **argv) {
    // Qualité de service d'un jeu au premier plan : sans elle (lancé depuis un
    // terminal), macOS place souvent le thread principal sur un cœur économie
    // (cadence bimodale mesurée : ~200 ou ~240 images/s selon les lancements).
    pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0);
    setvbuf(stdout, NULL, _IOLBF, 0);   // journal lisible même si le processus meurt
    const char *images = NULL, *game = NULL, *linux = NULL, *user = NULL;
    int first = 1;
    while (first < argc) {
        if (!strcmp(argv[first], "--images") && first + 1 < argc) { images = argv[first + 1]; first += 2; }
        else if (!strcmp(argv[first], "--game-dir") && first + 1 < argc) { game = argv[first + 1]; first += 2; }
        else if (!strcmp(argv[first], "--linux-root") && first + 1 < argc) { linux = argv[first + 1]; first += 2; }
        else if (!strcmp(argv[first], "--user-dir") && first + 1 < argc) { user = argv[first + 1]; first += 2; }
        else if (!strcmp(argv[first], "--")) { ++first; break; }
        else break;
    }
    if (!images || !game) { fprintf(stderr, "usage: %s --images /absolute/prep --game-dir /path/to/Portal2 -- [game arguments]\n", argv[0]); return 2; }
    if (rt_init() || rt_load_images(images) || rt_map(0x70000000, 0x800000)) {
        fputs("native runtime initialization failed\n", stderr); return 2;
    }
    rt_paths_set_program(RT_GAME_PROGRAM);
    if (linux && user && rt_paths_init(game, linux, user)) { perror("game paths"); rt_destroy(); return 2; }
    if (chdir(game)) { perror(game); rt_destroy(); return 2; }
    CPU cpu; rt_cpu_reset(&cpu); cpu.esp = 0x70800000;
    int renderer = wants_d3d9(first, argc, argv);
    // OpenGL : sans sampler objects sur macOS, togl doit filtrer par texture (runtime/gl.c).
    const char *sampler = renderer > 0 ? NULL : rt_gl_sampler_argument(argc - first, argv + first);
    uint32_t count = 1, vector = rt_alloc(((uint32_t)(argc - first) + 4) * 4);
    if (!vector) { rt_destroy(); return 2; }
    W32(vector, guest_string(RT_GAME_PROGRAM));
    for (int i = first; i < argc; ++i)
        if (renderer >= 0 || strcmp(argv[i], "-vulkan")) W32(vector + count++ * 4, guest_string(argv[i]));
    if (renderer > 0) W32(vector + count++ * 4, guest_string("-vulkan"));
    if (sampler) { W32(vector + count++ * 4, guest_string(sampler)); fprintf(stderr, "renderer: OpenGL without sampler objects, adding %s\n", sampler); }
    W32(vector + count * 4, 0);
    {   // /proc/self/cmdline : arguments invités séparés par des NUL
        size_t total = 0;
        for (uint32_t i = 0; i < count; ++i) total += strlen((const char *)GPTR(R32(vector + i * 4))) + 1;
        char *joined = malloc(total ? total : 1), *at = joined;
        for (uint32_t i = 0; joined && i < count; ++i) {
            size_t n = strlen((const char *)GPTR(R32(vector + i * 4))) + 1;
            memcpy(at, GPTR(R32(vector + i * 4)), n); at += n;
        }
        if (joined) { rt_procfs_set_cmdline(joined, total); free(joined); }
    }
    rt_loader_set_arguments(count, vector, 0);
    const GuestModule *program = rt_find_module(RT_GAME_PROGRAM);
    uint32_t entry = program ? rt_module_symbol(program, "main") : 0;
    if (!entry) { fputs("real " RT_GAME_PROGRAM " main is absent\n", stderr); rt_destroy(); return 2; }
    if (rt_initialize_module(&cpu, program)) { rt_destroy(); return 2; }
    uint32_t args[] = {count, vector, 0};
    fprintf(stderr, "entering recompiled " RT_GAME_PROGRAM " main at %#x\n", entry);
    int result = (int32_t)rt_invoke(&cpu, entry, args, 3);
    fprintf(stderr, RT_GAME_PROGRAM " main returned %d\n", result);
    rt_destroy();
    return result;
}
