// Plafond d'images par seconde et économie d'énergie automatique (power_ios.h).
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <Foundation/Foundation.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include "env_ios.h"
#include "power_ios.h"

enum { POWER_SAVE_FPS = 30 };
static _Atomic int cap;   // 0 : pas encore calculé
static int metal_renderer;

static int requested(void) {
    const char *e = getenv("P2_IOS_MAX_FPS");
    return e && atoi(e) > 0 ? atoi(e) : 60;
}
// Raison de brider la cadence, ou NULL.
static const char *power_reason(void) {
    if (!rt_ios_env_flag("P2_IOS_POWER_SAVE", 1)) return NULL;
    NSProcessInfo *info = NSProcessInfo.processInfo;
    if (info.lowPowerModeEnabled) return "mode économie d'énergie";
    if (info.thermalState == NSProcessInfoThermalStateCritical) return "état thermique critique";
    if (info.thermalState == NSProcessInfoThermalStateSerious) return "état thermique sérieux";
    return NULL;
}
static int update(int launch) {
    int wanted = requested(), value = wanted;
    const char *why = power_reason();
    if (why && value > POWER_SAVE_FPS) value = POWER_SAVE_FPS;
    int old = atomic_exchange(&cap, value);
    if (old == value) return value;
    if (launch && value == wanted) return value;
    if (value < wanted)
        fprintf(stderr, "[ios] économie d'énergie (%s) : %d i/s au lieu de %d%s\n", why, value, wanted,
                launch || !metal_renderer ? "" : " au prochain lancement (Metal : plafond fixé au démarrage)");
    else
        fprintf(stderr, "[ios] économie d'énergie terminée : %d i/s%s\n", value,
                metal_renderer ? " au prochain lancement (Metal)" : "");
    return value;
}

int rt_ios_max_fps(void) {
    int value = atomic_load(&cap);
    return value ? value : update(1);
}

void rt_ios_power_watch(int metal) {
    static atomic_flag done = ATOMIC_FLAG_INIT;
    if (atomic_flag_test_and_set(&done)) return;
    metal_renderer = metal;
    rt_ios_max_fps();
    // Notifications postées sur un thread quelconque : update n'écrit qu'un atomique et le journal.
    NSNotificationCenter *center = NSNotificationCenter.defaultCenter;
    for (NSNotificationName name in @[NSProcessInfoThermalStateDidChangeNotification, NSProcessInfoPowerStateDidChangeNotification])
        [center addObserverForName:name object:nil queue:nil usingBlock:^(NSNotification *n) { (void)n; update(0); }];
}
#endif
