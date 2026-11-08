// Visée au gyroscope (gyro_aim.c) : voir gyro_ios.h.
// Rotation du téléphone ajoutée à la caméra tactile : le pouce pour les grands
// mouvements, le poignet pour viser. Par défaut dans Half-Life 2 seulement :
// dans Portal 2, chaque tape au dos secouerait la caméra (P2_GYRO=1 l'active).
// Échantillons intégrés sur leur file, vidés à chaque image par la surcouche ;
// capteurs arrêtés hors partie comme ceux des tapes au dos. Gestionnaire
// CoreMotion à part : celui des tapes au dos peut ne pas exister (HL2) et a sa
// propre file.
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <CoreMotion/CoreMotion.h>
#include <os/lock.h>
#include <stdatomic.h>
#include <stdio.h>
#include "gyro_aim.h"
#include "gyro_ios.h"

static CMMotionManager *gyro_motion;
static NSOperationQueue *gyro_queue;
static os_unfair_lock gyro_lock = OS_UNFAIR_LOCK_INIT;
static double gyro_pending[2];            // comptes de souris pas encore envoyés (sous gyro_lock)
static _Atomic int gyro_generation;       // marche en cours : un échantillon d'une marche précédente est jeté
static _Atomic int gyro_orient;
static rt_gyro_params gyro_params;
static int gyro_running;

void rt_ios_gyro_start(int on_by_default) {
    if (gyro_motion || !rt_gyro_enabled(on_by_default)) return;
    CMMotionManager *motion = [CMMotionManager new];
    if (!motion.deviceMotionAvailable) { fprintf(stderr, "[ios] gyroscope : pas de capteur (simulateur ?)\n"); return; }
    rt_gyro_params_from_env(&gyro_params);
    gyro_queue = [NSOperationQueue new];
    gyro_queue.name = @"p2.gyro";
    gyro_queue.maxConcurrentOperationCount = 1;
    gyro_queue.qualityOfService = NSQualityOfServiceUserInteractive;
    motion.deviceMotionUpdateInterval = 1.0 / 100;
    gyro_motion = motion;
    fprintf(stderr, "[ios] gyroscope : visée active, %.0f comptes/rad, atténuation sous %.3g rad/s\n",
            gyro_params.counts_per_rad, gyro_params.tighten);
}
int rt_ios_gyro_available(void) { return gyro_motion != nil; }
void rt_ios_gyro_set_orient(int orient) { atomic_store(&gyro_orient, orient); }

void rt_ios_gyro_run(int on) {
    if (!gyro_motion || on == gyro_running) return;
    gyro_running = on;
    // Chaque marche a son numéro et son propre horodatage précédent (capturé par
    // le bloc, lu et écrit sur la file des capteurs seulement) : un échantillon
    // encore en file après un arrêt n'ajoute rien à la marche suivante.
    int generation = atomic_fetch_add(&gyro_generation, 1) + 1;
    if (on) {
        __block double last = 0;
        [gyro_motion startDeviceMotionUpdatesToQueue:gyro_queue withHandler:^(CMDeviceMotion *m, NSError *error) {
            (void)error;
            if (!m || atomic_load(&gyro_generation) != generation) return;
            double w[3] = {m.rotationRate.x, m.rotationRate.y, m.rotationRate.z};
            double g[3] = {m.gravity.x, m.gravity.y, m.gravity.z};
            double dt = last ? m.timestamp - last : 0, d[2];
            last = m.timestamp;
            if (!rt_gyro_delta(&gyro_params, w, g, atomic_load(&gyro_orient), dt, d)) return;
            os_unfair_lock_lock(&gyro_lock);
            if (atomic_load(&gyro_generation) == generation) { gyro_pending[0] += d[0]; gyro_pending[1] += d[1]; }
            os_unfair_lock_unlock(&gyro_lock);
        }];
    } else [gyro_motion stopDeviceMotionUpdates];
}
void rt_ios_gyro_take(double out[2]) {
    os_unfair_lock_lock(&gyro_lock);
    out[0] = gyro_pending[0]; out[1] = gyro_pending[1];
    gyro_pending[0] = gyro_pending[1] = 0;
    os_unfair_lock_unlock(&gyro_lock);
}
#endif
