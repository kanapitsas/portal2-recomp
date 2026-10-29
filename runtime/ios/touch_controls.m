// Commandes tactiles de Portal 2 sur iPhone/iPad.
//
// Une surcouche UIKit posée sur la fenêtre de SDL, affichée en partie
// seulement : quand l'interface du jeu est visible (menus, pause, écrans de
// chargement ; game_state.c le demande au moteur), elle s'efface et laisse les
// doigts à SDL, pour qui un toucher est un clic. En partie, elle traduit les
// doigts en ce que le jeu comprend par défaut, clavier et souris :
// - stick gauche : déplacement analogique, par une manette SDL virtuelle
//   (axe gauche) que le jeu lit comme une manette Xbox : direction continue,
//   vitesse proportionnelle à l'inclinaison. À défaut de manette virtuelle
//   (création refusée par SDL), touches W/A/S/D en 8 directions ;
// - stick droit : caméra, mouvements de souris relatifs à vitesse
//   proportionnelle à l'inclinaison (courbe quadratique pour viser finement) ;
//   chaque stick se centre là où le doigt se pose, dans sa moitié d'écran ;
//   un bouton bascule la moitié droite en trackpad : le glissement du doigt
//   devient directement le mouvement de souris (choix retenu entre parties,
//   P2TouchTrackpad dans les réglages de l'app) ;
// - portails : tapes au dos (ci-dessous) ; sans capteur de mouvement
//   (simulateur) ou avec P2_BACKTAP=0, une paire de boutons bleu (clic gauche)
//   / orange (clic droit) dans chaque coin du haut ;
// - icônes : sauter (espace), s'accroupir (Ctrl gauche), utiliser (E),
//   pause (Échap) ;
// - tapes au dos ou sur les côtés du téléphone (index derrière, en paysage) :
//   à gauche portail bleu, à droite orange ; détection dans back_tap.c,
//   nourrie ici par CoreMotion (P2_BACKTAP=0 la coupe, réglages dans
//   docs/ios.md).
// Avec une manette ou un clavier connecté, elle se cache jusqu'au premier
// toucher ; avec une vraie manette, la manette virtuelle est retirée pour que
// le jeu ne lise qu'elle. P2_TOUCH_CONTROLS=0 la désactive, =1 l'affiche toujours ; P2_TOUCH_LOOK règle la vitesse
// de la caméra (1 par défaut, stick et trackpad).
//
// Disposition et dessin inspirés de la manette tactile de TOTK de
// MacNativeGames (games/totk/ios/Sources/touch_pad.m, licence MIT).
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <CoreMotion/CoreMotion.h>
#import <GameController/GameController.h>
#import <QuartzCore/QuartzCore.h>
#import <UIKit/UIKit.h>
#include <SDL.h>
#include <SDL_syswm.h>
#include <math.h>
#include <os/lock.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "back_tap.h"

int rt_ios_game_ui_visible(void);   // game_state.c
extern _Atomic unsigned long rt_ios_frames;  // gl_ios.m : images présentées
extern volatile int rt_ios_metal_active;      // metal_ios.m : rendu Direct3D 9 en cours
void rt_ios_move_response_linear(int on);
int rt_ios_move_response(void);
void rt_ios_console_command(const char *name);   // game_state.c

typedef enum { ROLE_MOVE, ROLE_LOOK, ROLE_BUTTON } Role;
typedef enum { KEY_SPACE, KEY_CTRL, KEY_E, KEY_ESCAPE, MOUSE_LEFT, MOUSE_RIGHT, LOOK_MODE, ACTION_COUNT } Action;
typedef struct { Action action; NSString *symbol; UIColor *color; CGPoint at; CGFloat size; } Button;
enum { MAX_BUTTONS = 10 };

// Manette virtuelle : deux axes seulement (stick gauche), aucun bouton, pour
// qu'elle ne porte aucune autre commande que le déplacement.
static SDL_Joystick *virtual_pad(void) {
    SDL_VirtualJoystickDesc desc = {0};
    desc.version = SDL_VIRTUAL_JOYSTICK_DESC_VERSION;
    desc.type = SDL_JOYSTICK_TYPE_GAMECONTROLLER;
    desc.naxes = 2;
    desc.axis_mask = (1u << SDL_CONTROLLER_AXIS_LEFTX) | (1u << SDL_CONTROLLER_AXIS_LEFTY);
    desc.name = "Commandes tactiles";
    int index = SDL_JoystickAttachVirtualEx(&desc);
    SDL_Joystick *pad = index < 0 ? NULL : SDL_JoystickOpen(index);
    if (index >= 0 && !pad) SDL_JoystickDetachVirtual(index);
    fprintf(stderr, "[ios] manette virtuelle : %s\n", pad ? "créée" : SDL_GetError());
    return pad;
}
static void remove_virtual_pad(SDL_Joystick *pad) {
    SDL_JoystickID id = SDL_JoystickInstanceID(pad);
    SDL_JoystickClose(pad);
    for (int i = 0; i < SDL_NumJoysticks(); ++i)
        if (SDL_JoystickGetDeviceInstanceID(i) == id) { SDL_JoystickDetachVirtual(i); break; }
    fprintf(stderr, "[ios] manette virtuelle retirée\n");
}

static const CGFloat kButtonSize = 54, kPortalSize = 60;
static const float kStickLook = 90;      // comptes de souris par image, stick à fond (≈ 360°/s à la sensibilité 3 du jeu)
static const float kTrackpadLook = 3.5f; // comptes de souris par point de glissement
static NSString *const kTrackpadKey = @"P2TouchTrackpad";
static float look_speed(void) {
    static float speed;
    if (!speed) { const char *e = getenv("P2_TOUCH_LOOK"); speed = e && atof(e) > 0 ? (float)atof(e) : 1.0f; }
    return speed;
}

// Accélération du trackpad : gain 1 sous P2_TOUCH_ACCEL_LOW pt/s (visée fine
// inchangée), puis montée en douceur (smoothstep) jusqu'à P2_TOUCH_ACCEL
// (multiplicateur maximal) à P2_TOUCH_ACCEL_HIGH pt/s. P2_TOUCH_ACCEL=1 la coupe.
// La vitesse du doigt est lissée par moyenne exponentielle (P2_TOUCH_ACCEL_SMOOTH,
// poids du nouvel échantillon) pour qu'un échantillon isolé ne fasse pas sauter le gain.
static float env_float(const char *name, float def, float min) {
    const char *e = getenv(name); return e && atof(e) >= min ? (float)atof(e) : def;
}
static struct { float max, low, high, smooth; int log; } accel;
static float trackpad_gain(double speed);
static void accel_init(void) {
    if (accel.max) return;
    accel.max = env_float("P2_TOUCH_ACCEL", 2.5f, 1);
    accel.low = env_float("P2_TOUCH_ACCEL_LOW", 300, 0);
    accel.high = env_float("P2_TOUCH_ACCEL_HIGH", 1500, accel.low + 1);
    if (accel.high <= accel.low) accel.high = accel.low + 1;
    accel.smooth = fminf(1, env_float("P2_TOUCH_ACCEL_SMOOTH", 0.5f, 0.01f));
    accel.log = getenv("P2_TOUCH_ACCEL_LOG") != NULL;
    fprintf(stderr, "[ios] trackpad : gain %.2f à %g pt/s, %.2f à %g, %.2f à %g, %.2f à 3000 pt/s\n",
        trackpad_gain(100), 100.0, trackpad_gain(600), 600.0, trackpad_gain(1000), 1000.0, trackpad_gain(3000));
}
static float trackpad_gain(double speed) {
    double t = fmin(1, fmax(0, (speed - accel.low) / (accel.high - accel.low)));
    return 1 + (accel.max - 1) * (float)(t * t * (3 - 2 * t));
}

// La boucle de SDL (CFRunLoopRunInMode à délai nul) ne laisse jamais UIKit
// valider ses transactions : un dessin demandé n'apparaîtrait pas. Forcé ici.
static void redraw(UIView *view) {
    [view setNeedsDisplay];
    [view.layer displayIfNeeded];
    [CATransaction flush];
}

// ---- tapes au dos (CoreMotion) ----
// Échantillons de mouvement (accélération utilisateur, rotation, gravité) sur
// une file dédiée, jugés par back_tap.c ; une tape acceptée est déposée dans
// backtap_pending et cliquée par la minuterie de la surcouche (step), qui
// connaît l'état menu/jeu. Les débuts et fins de toucher arrivent du thread
// principal : verrou autour du détecteur. Pas d'autorisation à demander pour
// ces capteurs (seule l'activité physique, CMMotionActivityManager, en exige une).
static CMMotionManager *backtap_motion;
static NSOperationQueue *backtap_queue;
static os_unfair_lock backtap_lock = OS_UNFAIR_LOCK_INIT;
static rt_backtap backtap_detector;
static _Atomic int backtap_orient;    // +1 : axe y de l'appareil vers la droite de l'écran
static _Atomic int backtap_active;    // en partie, surcouche affichée
static _Atomic int backtap_pending;   // tape à cliquer : -1 gauche, +1 droite
static int backtap_log;               // P2_BACKTAP_LOG=1 : chaque candidat, et le bruit toutes les 5 s
static float backtap_hold;            // s : durée de l'appui
// P2_BACKTAP_RAW=<fichier> (relatif à HOME) : échantillons et touchers bruts,
// rejoués sur le Mac par tools/backtap_replay.c pour régler les seuils.
static FILE *backtap_raw;
// Journal, sur la file des capteurs seulement : origine des temps, bruit de fond.
static struct { double origin, window, last_t, sq; float last_a[3], max_a, max_jerk, max_rot; long samples; } backtap_noise;

static int env_sign(const char *name, int def) {
    const char *e = getenv(name);
    return !e || !*e ? def : atoi(e) > 0 ? 1 : atoi(e) < 0 ? -1 : 0;
}

static void backtap_log_report(const rt_backtap_report *r, double origin) {
    char touch[32], verdict[80];
    if (fabs(r->touch_dt) < 10) snprintf(touch, sizeof(touch), "%+.0f ms", r->touch_dt * 1000);
    else snprintf(touch, sizeof(touch), "aucun");
    if (r->reject) snprintf(verdict, sizeof(verdict), "rejet : %s", r->reject);
    else snprintf(verdict, sizeof(verdict), "%s (%s%s)", r->side < 0 ? "GAUCHE, portail bleu" : "DROITE, portail orange",
                  r->method, r->gyro ? ", gyroscope" : "");
    fprintf(stderr, "[ios] tape %.3f s : pic %.2f g (x %+.2f y %+.2f z %+.2f), secousse %.0f g/s, "
            "rotation x %+.2f y %+.2f z %+.2f rad/s, montée %.0f ms, durée %.0f ms, avant %.3f g, "
            "gravité %.2f g (x %+.2f y %+.2f z %+.2f), "
            "toucher %s, orientation %+d -> %s\n",
            r->t - origin, r->peak, r->acc[0], r->acc[1], r->acc[2], r->jerk, r->rot[0], r->rot[1], r->rot[2],
            r->rise * 1000, r->width * 1000, r->pre_rms, r->gravity, r->grav[0], r->grav[1], r->grav[2], touch,
            r->orient, verdict);
}

// Mode journal : chaque candidat jugé, et toutes les 5 s le bruit de fond
// (maxima et moyenne quadratique) pour placer les seuils au-dessus du jeu normal.
static void backtap_log_sample(const rt_backtap_sample *s, const rt_backtap_report *r) {
    typeof(backtap_noise) *n = &backtap_noise;
    if (!n->origin) { n->origin = n->window = s->t; fprintf(stderr, "[ios] tapes au dos : origine des temps %.3f s\n", s->t); }
    if (r) backtap_log_report(r, n->origin);
    float mag = sqrtf(s->a[0] * s->a[0] + s->a[1] * s->a[1] + s->a[2] * s->a[2]);
    if (n->samples && s->t > n->last_t) {
        float da[3] = {s->a[0] - n->last_a[0], s->a[1] - n->last_a[1], s->a[2] - n->last_a[2]};
        n->max_jerk = fmaxf(n->max_jerk, sqrtf(da[0] * da[0] + da[1] * da[1] + da[2] * da[2]) / (float)(s->t - n->last_t));
    }
    n->max_a = fmaxf(n->max_a, mag); n->max_rot = fmaxf(n->max_rot, fabsf(s->w[0])); n->sq += (double)mag * mag;
    memcpy(n->last_a, s->a, sizeof(n->last_a)); n->last_t = s->t; ++n->samples;
    if (s->t - n->window < 5) return;
    fprintf(stderr, "[ios] tapes au dos, bruit %.0f-%.0f s : %.0f Hz, acc max %.3f g (moy. quadr. %.3f), "
            "secousse max %.1f g/s, rotation x max %.2f rad/s, %s\n",
            n->window - n->origin, s->t - n->origin, n->samples / (s->t - n->window), n->max_a, sqrt(n->sq / n->samples),
            n->max_jerk, n->max_rot, atomic_load(&backtap_active) ? "en partie" : "menu");
    n->window = s->t; n->sq = 0; n->max_a = n->max_jerk = n->max_rot = 0; n->samples = 0;
}

// Démarre les capteurs (thread principal). Sans capteur (simulateur) : rien.
static void backtap_start(void) {
    const char *off = getenv("P2_BACKTAP");
    if (backtap_motion || (off && *off == '0')) return;
    CMMotionManager *motion = [CMMotionManager new];
    if (!motion.deviceMotionAvailable) {
        fprintf(stderr, "[ios] tapes au dos : pas de capteur de mouvement (simulateur ?)\n");
        return;
    }
    rt_backtap_params p;
    rt_backtap_defaults(&p);
    // Sensibilité (Réglages : 0,75 / 1 / 1,33) : seuils de détection divisés
    // d'autant ; les variables par seuil, ci-dessous, priment.
    float k = env_float("P2_BACKTAP_SENSITIVITY", 1, 0.1f);
    p.accel_min /= k; p.jerk_min /= k; p.gyro_accel_min /= k; p.gyro_rot_min /= k;
    p.accel_min = env_float("P2_BACKTAP_ACCEL", p.accel_min, 0.001f);
    p.jerk_min = env_float("P2_BACKTAP_JERK", p.jerk_min, 0);
    p.rot_min = env_float("P2_BACKTAP_ROT", p.rot_min, 0.001f);
    p.side_x_weight = env_float("P2_BACKTAP_SIDE_X", p.side_x_weight, 0);
    p.gyro_accel_min = env_float("P2_BACKTAP_GYRO_ACCEL", p.gyro_accel_min, 0.001f);
    p.gyro_rot_min = env_float("P2_BACKTAP_GYRO_ROT", p.gyro_rot_min, 0.001f);
    p.gyro_jerk_min = env_float("P2_BACKTAP_GYRO_JERK", p.gyro_jerk_min, 0);
    p.gyro_calm_max = env_float("P2_BACKTAP_GYRO_CALM", p.gyro_calm_max, 0);
    p.refractory = env_float("P2_BACKTAP_REFRACTORY", p.refractory, 0);
    p.touch_guard = env_float("P2_BACKTAP_TOUCH_GUARD", p.touch_guard, 0);
    p.max_width = env_float("P2_BACKTAP_WIDTH", p.max_width, 0.001f);
    p.flat_z = env_float("P2_BACKTAP_FLAT", p.flat_z, -2);
    p.pre_rms_max = env_float("P2_BACKTAP_AGITATION", p.pre_rms_max, 0.001f);
    p.gravity_max = env_float("P2_BACKTAP_GRAVITY", p.gravity_max, 0.001f);
    p.lateral_sign = env_sign("P2_BACKTAP_LATERAL", p.lateral_sign);
    p.z_sign = env_sign("P2_BACKTAP_ZSIGN", p.z_sign);
    p.invert = env_sign("P2_BACKTAP_INVERT", 0) > 0;
    backtap_log = getenv("P2_BACKTAP_LOG") && *getenv("P2_BACKTAP_LOG") != '0';
    backtap_hold = env_float("P2_BACKTAP_HOLD", 0.05f, 0);
    const char *raw = getenv("P2_BACKTAP_RAW");
    if (raw && *raw) {
        char path[1024];
        snprintf(path, sizeof(path), "%s%s%s", raw[0] == '/' ? "" : getenv("HOME"), raw[0] == '/' ? "" : "/", raw);
        backtap_raw = fopen(path, "w");
        fprintf(stderr, "[ios] tapes au dos : enregistrement brut %s%s\n", path, backtap_raw ? "" : " impossible");
        if (backtap_raw) setvbuf(backtap_raw, NULL, _IOLBF, 0);   // tout est écrit si le jeu est tué
        if (backtap_raw) fprintf(backtap_raw, "# m t ax ay az wx wy wz gx gy gz orient active | t t_toucher\n");
    }
    float hz = env_float("P2_BACKTAP_HZ", 200, 10);
    rt_backtap_init(&backtap_detector, &p);

    backtap_queue = [NSOperationQueue new];
    backtap_queue.name = @"p2.backtap";
    backtap_queue.maxConcurrentOperationCount = 1;
    backtap_queue.qualityOfService = NSQualityOfServiceUserInteractive;
    motion.deviceMotionUpdateInterval = 1.0 / hz;
    fprintf(stderr, "[ios] tapes au dos : %g Hz demandés ; seuils %.3g g, %.3g g/s, rotation %.3g rad/s ; "
            "gyroscope %.3g g, %.3g rad/s, %.3g rad/s² ; "
            "réfractaire %.0f ms, toucher ±%.0f ms, à plat sous %.2f g, durée %.0f ms, agitation %.3g g, gravité %.3g g ; "
            "latéral %+d, z %+d, inversé %d, appui %.0f ms%s\n",
            hz, p.accel_min, p.jerk_min, p.rot_min, p.gyro_accel_min, p.gyro_rot_min, p.gyro_jerk_min, p.refractory * 1e3, p.touch_guard * 1e3, p.flat_z, p.max_width * 1e3,
            p.pre_rms_max, p.gravity_max, p.lateral_sign, p.z_sign, p.invert, backtap_hold * 1e3,
            backtap_log ? ", journal" : "");
    [motion startDeviceMotionUpdatesToQueue:backtap_queue withHandler:^(CMDeviceMotion *m, NSError *error) {
        if (!m) { if (error) fprintf(stderr, "[ios] tapes au dos : %s\n", error.localizedDescription.UTF8String); return; }
        CMAcceleration a = m.userAcceleration, g = m.gravity;
        CMRotationRate w = m.rotationRate;
        rt_backtap_sample s = {m.timestamp, {(float)a.x, (float)a.y, (float)a.z},
                               {(float)w.x, (float)w.y, (float)w.z}, {(float)g.x, (float)g.y, (float)g.z}};
        rt_backtap_report r;
        os_unfair_lock_lock(&backtap_lock);
        if (backtap_raw)
            fprintf(backtap_raw, "m %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %d %d\n", s.t, s.a[0], s.a[1], s.a[2],
                    s.w[0], s.w[1], s.w[2], s.g[0], s.g[1], s.g[2], atomic_load(&backtap_orient), atomic_load(&backtap_active));
        int judged = rt_backtap_feed(&backtap_detector, &s, atomic_load(&backtap_orient), atomic_load(&backtap_active), &r);
        os_unfair_lock_unlock(&backtap_lock);
        if (judged && !r.reject) atomic_store(&backtap_pending, r.side);
        if (backtap_log) backtap_log_sample(&s, judged ? &r : NULL);
    }];
    backtap_motion = motion;
}
static void backtap_touch(NSSet<UITouch *> *touches) {
    if (!backtap_motion) return;
    os_unfair_lock_lock(&backtap_lock);
    for (UITouch *t in touches) {
        rt_backtap_touch(&backtap_detector, t.timestamp);
        if (backtap_raw) fprintf(backtap_raw, "t %.4f\n", t.timestamp);
    }
    os_unfair_lock_unlock(&backtap_lock);
}

// Doigt suivi : rôle, action (bouton), point de départ, dernier point.
@interface P2Finger : NSObject
@property Role role; @property Action action; @property CGPoint start, at;
@property NSTimeInterval stamp; @property double speed;   // trackpad : horodatage, vitesse lissée (pt/s)
@end
@implementation P2Finger
@end

@interface P2TouchControls : UIView
- (instancetype)initWithWindow:(SDL_Window *)window;
@end

@implementation P2TouchControls {
    SDL_Window *window;
    NSMapTable<UITouch *, P2Finger *> *fingers;
    int held[ACTION_COUNT];   // nombre de doigts par action
    BOOL wasd[4];             // W, A, S, D enfoncées
    BOOL concealed;           // cachée pour une manette ou un clavier
    BOOL menu;                // interface du jeu affichée : surcouche effacée
    NSTimer *tick;            // caméra : un pas par image tant que le stick est incliné
    float lookCarry[2];       // fractions de comptes de souris reportées
    BOOL trackpad;            // moitié droite en trackpad plutôt qu'en stick
    BOOL physicalPad;         // vraie manette connectée
    int fps;                  // compteur d'images affiché (0 : rien à afficher)
    int fpsHidden;            // masqué : P2_SHOW_FPS=0 (Réglages) ou toucher du coin haut gauche
    unsigned long fpsFrames;  // images au dernier relevé
    CFTimeInterval fpsAt;     // instant du dernier relevé
    SDL_Joystick *pad;        // manette virtuelle du stick gauche (NULL : W/A/S/D)
    BOOL padRefused;          // création refusée : ne pas réessayer à chaque image
    Sint16 padAxes[2];        // dernières valeurs envoyées
    int padResponse;          // courbe du jeu pour ces valeurs (joy_response_move)
    CGFloat testMove;         // essai : inclinaison avant imposée (P2_TOUCH_TEST_MOVE)
    Uint64 testNext;          // essai : prochaine étape (ms de SDL)
    int testPhase;            // essai : étape en cours
    int backtapHeld;          // tape au dos : bouton enfoncé (Action), -1 aucun
    CFTimeInterval backtapAt; // tape au dos : instant de l'appui
    unsigned long backtapFrames; // tape au dos : images présentées à l'appui
}

- (instancetype)initWithWindow:(SDL_Window *)sdlWindow {
    if ((self = [super initWithFrame:CGRectZero])) {
        window = sdlWindow;
        self.multipleTouchEnabled = YES; self.backgroundColor = UIColor.clearColor; self.opaque = NO;
        self.contentMode = UIViewContentModeRedraw;
        fingers = [NSMapTable strongToStrongObjectsMapTable];
        trackpad = [NSUserDefaults.standardUserDefaults boolForKey:kTrackpadKey];
        NSNotificationCenter *center = NSNotificationCenter.defaultCenter;
        for (NSNotificationName name in @[GCControllerDidConnectNotification, GCControllerDidDisconnectNotification,
                                          GCKeyboardDidConnectNotification, GCKeyboardDidDisconnectNotification])
            [center addObserver:self selector:@selector(devicesChanged) name:name object:nil];
        [self devicesChanged];
        backtapHeld = -1;
        const char *show = getenv("P2_SHOW_FPS");   // posée par les Réglages (runtime/ios/settings.c)
        fpsHidden = show && !strcmp(show, "0");
        backtap_start();
        __weak P2TouchControls *weakSelf = self;
        tick = [NSTimer scheduledTimerWithTimeInterval:1.0 / 60 repeats:YES block:^(NSTimer *timer) {
            (void)timer; [weakSelf step];
        }];
    }
    return self;
}

- (void)devicesChanged {
    BOOL gamepad = NO;
    for (GCController *c in GCController.controllers) gamepad |= c.extendedGamepad != nil;
    // Manette que le jeu verra (sauf pilote MFi de SDL coupé, SDL_JOYSTICK_MFI=0).
    physicalPad = gamepad && SDL_GetHintBoolean(SDL_HINT_JOYSTICK_MFI, SDL_TRUE);
    BOOL any = gamepad || GCKeyboard.coalescedKeyboard != nil;
    const char *force = getenv("P2_TOUCH_CONTROLS");   // 1 : jamais cachée (essais sur simulateur)
    if (force && *force == '1') any = NO;
    if (any != concealed) { concealed = any; redraw(self); }
}

// À chaque image : rester devant la vue OpenGL de SDL (ajoutée après la
// fenêtre), suivre la taille de la fenêtre, tourner la caméra.
- (void)step {
    UIView *host = self.superview;
    BOOL ui = rt_ios_game_ui_visible() != 0;
    if (ui != menu) {
        menu = ui;
        if (menu) [self releaseAll];
        redraw(self);
    }
    [self updateFPS];
    [self syncPad];
    if (pad && rt_ios_move_response() != padResponse) { padResponse = rt_ios_move_response(); [self updateMove]; }
    [self testStep];
    [self backTapStep];
    if (host.subviews.lastObject != self) [host bringSubviewToFront:self];
    if (!CGRectEqualToRect(self.frame, host.bounds)) { self.frame = host.bounds; redraw(self); }
    if (trackpad) return;   // trackpad : mouvements envoyés au glissement (touchesMoved)
    for (P2Finger *f in fingers.objectEnumerator) {
        if (f.role != ROLE_LOOK) continue;
        CGFloat x, y; [self deflectionOf:f x:&x y:&y];
        float scale = kStickLook * look_speed();   // quadratique : lent près du centre pour viser
        [self lookBy:(float)(x * fabs(x)) * scale y:(float)(-y * fabs(y)) * scale];
    }
}
// Mouvement de souris relatif, fractions de comptes reportées.
- (void)lookBy:(float)x y:(float)y {
    lookCarry[0] += x; lookCarry[1] += y;
    int dx = (int)lookCarry[0], dy = (int)lookCarry[1];
    lookCarry[0] -= (float)dx; lookCarry[1] -= (float)dy;
    if (!dx && !dy) return;
    SDL_Event e = {0};
    e.type = SDL_MOUSEMOTION; e.motion.windowID = SDL_GetWindowID(window);
    e.motion.xrel = dx; e.motion.yrel = dy;
    SDL_PushEvent(&e);
}
// Tape au dos acceptée : appui, puis relâchement après P2_BACKTAP_HOLD et au
// moins deux images (vgui ignore un clic dont l'appui et le relâchement
// tombent dans la même image). Passe par action: comme un bouton tenu : si un
// doigt tient déjà ce portail, rien n'est renvoyé au jeu.
- (void)backTapStep {
    if (!backtap_motion) return;
    UIInterfaceOrientation o = self.window.windowScene.interfaceOrientation;
    atomic_store(&backtap_orient, o == UIInterfaceOrientationLandscapeLeft ? 1 : o == UIInterfaceOrientationLandscapeRight ? -1 : 0);
    atomic_store(&backtap_active, !menu && !concealed);
    int side = atomic_exchange(&backtap_pending, 0);
    if (side && !menu && !concealed && backtapHeld < 0) {
        backtapHeld = side < 0 ? MOUSE_LEFT : MOUSE_RIGHT;
        backtapAt = CACurrentMediaTime(); backtapFrames = rt_ios_frames;
        [self action:(Action)backtapHeld down:YES];
        redraw(self);   // le bouton du portail s'allume
    }
    [self backTapRelease:NO];
}
- (void)backTapRelease:(BOOL)now {
    if (backtapHeld < 0) return;
    if (!now && (CACurrentMediaTime() - backtapAt < backtap_hold || rt_ios_frames < backtapFrames + 2)) return;
    [self action:(Action)backtapHeld down:NO];
    backtapHeld = -1;
    redraw(self);
}
- (void)toggleLookMode {
    trackpad = !trackpad;
    [NSUserDefaults.standardUserDefaults setBool:trackpad forKey:kTrackpadKey];
    lookCarry[0] = lookCarry[1] = 0;
}

// Hors partie, les doigts passent à la vue de SDL (clics dans les menus).
- (UIView *)hitTest:(CGPoint)point withEvent:(UIEvent *)event {
    return menu ? nil : [super hitTest:point withEvent:event];
}
- (void)releaseAll {
    for (P2Finger *f in fingers.objectEnumerator) if (f.role == ROLE_BUTTON) [self action:f.action down:NO];
    [self backTapRelease:YES];
    [fingers removeAllObjects];
    [self updateMove];
}

// ---- événements SDL ----
- (void)key:(SDL_Scancode)code down:(BOOL)down {
    SDL_Event e = {0};
    e.type = down ? SDL_KEYDOWN : SDL_KEYUP;
    e.key.windowID = SDL_GetWindowID(window); e.key.state = down ? SDL_PRESSED : SDL_RELEASED;
    e.key.keysym.scancode = code; e.key.keysym.sym = SDL_GetKeyFromScancode(code);
    SDL_PushEvent(&e);
}
// Position en points de la fenêtre SDL (sdl_ios.m la convertit en pixels).
- (void)mouseButton:(Uint8)button down:(BOOL)down at:(CGPoint)p {
    SDL_Event e = {0};
    e.type = down ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;
    e.button.windowID = SDL_GetWindowID(window); e.button.button = button;
    e.button.state = down ? SDL_PRESSED : SDL_RELEASED; e.button.clicks = 1;
    e.button.x = (Sint32)p.x; e.button.y = (Sint32)p.y;
    SDL_PushEvent(&e);
}
- (void)action:(Action)action down:(BOOL)down {
    int before = held[action];
    held[action] = MAX(0, held[action] + (down ? 1 : -1));
    if ((before > 0) == (held[action] > 0)) return;
    CGPoint centre = CGPointMake(CGRectGetMidX(self.bounds), CGRectGetMidY(self.bounds));
    switch (action) {
    case KEY_SPACE: [self key:SDL_SCANCODE_SPACE down:down]; break;
    case KEY_CTRL: [self key:SDL_SCANCODE_LCTRL down:down]; break;
    case KEY_E: [self key:SDL_SCANCODE_E down:down]; break;
    case KEY_ESCAPE: [self key:SDL_SCANCODE_ESCAPE down:down]; break;
    case MOUSE_LEFT: [self mouseButton:SDL_BUTTON_LEFT down:down at:centre]; break;
    case MOUSE_RIGHT: [self mouseButton:SDL_BUTTON_RIGHT down:down at:centre]; break;
    case LOOK_MODE: if (down) [self toggleLookMode]; break;
    default: break;
    }
}
- (void)setDirections:(const BOOL *)wanted {
    static const SDL_Scancode codes[4] = {SDL_SCANCODE_W, SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D};
    for (int i = 0; i < 4; ++i)
        if (wanted[i] != wasd[i]) { wasd[i] = wanted[i]; [self key:codes[i] down:wanted[i]]; }
}

// ---- disposition (points, dans la zone sûre) ----
- (CGRect)area { return UIEdgeInsetsInsetRect(self.bounds, self.safeAreaInsets); }
- (CGFloat)stickRadius { return MIN(66, self.area.size.height * .15); }
- (CGPoint)restingStick:(BOOL)right {
    CGRect a = self.area;
    return CGPointMake(CGRectGetMinX(a) + a.size.width * (right ? .80 : .16), CGRectGetMinY(a) + a.size.height * .72);
}
static BOOL portal_buttons(void) {
    const char *e = getenv("P2_PORTAL_BUTTONS");
    if (e && *e == '1') return YES;
    if (e && *e == '0') return NO;
    return backtap_motion == nil;
}
- (int)buttons:(Button *)b {
    CGRect a = self.area; CGFloat x = CGRectGetMinX(a), y = CGRectGetMinY(a), w = a.size.width, h = a.size.height;
    UIColor *blue = [UIColor colorWithRed:.16 green:.55 blue:1 alpha:.6], *orange = [UIColor colorWithRed:1 green:.55 blue:.05 alpha:.6];
    UIColor *ink = [UIColor colorWithWhite:0 alpha:.35];   // fond sombre : icônes lisibles sur un mur clair
    CGFloat top = y + h * .19, gap = kPortalSize * 1.15;   // sous le compteur d'images
    Button all[] = {
        // Portails, en miroir : bleu à l'extérieur, orange à l'intérieur, des deux côtés.
        {MOUSE_LEFT, nil, blue, {x + w * .06, top}, kPortalSize}, {MOUSE_RIGHT, nil, orange, {x + w * .06 + gap, top}, kPortalSize},
        {MOUSE_LEFT, nil, blue, {x + w * .94, top}, kPortalSize}, {MOUSE_RIGHT, nil, orange, {x + w * .94 - gap, top}, kPortalSize},
        {KEY_SPACE, @"arrow.up", ink, {x + w * .94, y + h * .50}, kButtonSize},
        {KEY_CTRL, @"arrow.down", ink, {x + w * .80, y + h * .40}, kButtonSize},
        {KEY_E, @"hand.point.up.left.fill", ink, {x + w * .94, y + h * .80}, kButtonSize},
        {KEY_ESCAPE, @"pause.fill", ink, {x + w - kButtonSize * .45, y + kButtonSize * .45}, kButtonSize * .75},
        // Mode de la caméra, dans le coin en bas à gauche : l'icône montre le mode actuel.
        {LOOK_MODE, trackpad ? @"hand.draw.fill" : @"r.joystick.fill", ink,
         {x + kButtonSize * .45, y + h - kButtonSize * .45}, kButtonSize * .75},
    };
    int n = 0;
    for (int i = 0; i < (int)(sizeof(all) / sizeof(*all)) && n < MAX_BUTTONS; ++i) {
        // Les tapes au dos tirent les portails : leurs boutons ne servent que sans
        // elles, sauf choix contraire (P2_PORTAL_BUTTONS=1 toujours, =0 jamais).
        if ((all[i].action == MOUSE_LEFT || all[i].action == MOUSE_RIGHT) && !portal_buttons()) continue;
        b[n++] = all[i];
    }
    return n;
}
- (int)buttonAt:(CGPoint)p {
    Button b[MAX_BUTTONS]; int n = [self buttons:b];
    for (int i = 0; i < n; ++i) if (hypot(p.x - b[i].at.x, p.y - b[i].at.y) < b[i].size * .62) return (int)b[i].action;
    return -1;
}

// ---- doigts ----
// Inclinaison du stick tenu par ce doigt, de -1 à 1 par axe (y vers le haut),
// zone morte de 15 % retranchée.
- (void)deflectionOf:(P2Finger *)f x:(CGFloat *)x y:(CGFloat *)y {
    CGFloat dx = f.at.x - f.start.x, dy = f.start.y - f.at.y, r = self.stickRadius, dist = hypot(dx, dy);
    if (dist > r) { dx *= r / dist; dy *= r / dist; dist = r; }
    CGFloat dead = r * .15;
    if (dist < dead) { *x = *y = 0; return; }
    CGFloat k = (dist - dead) / (r - dead) / dist;
    *x = dx * k; *y = dy * k;
}
- (void)updateMove {
    BOOL wanted[4] = {NO, NO, NO, NO};
    CGFloat ax = 0, ay = 0;
    for (P2Finger *f in fingers.objectEnumerator) {
        if (f.role != ROLE_MOVE) continue;
        CGFloat x, y; [self deflectionOf:f x:&x y:&y];
        ax = x; ay = y;
        if (hypot(x, y) < .2) continue;
        CGFloat angle = atan2(y, x);   // secteurs de 45° : les diagonales enfoncent deux touches
        wanted[0] |= angle > M_PI / 8 && angle < M_PI - M_PI / 8;
        wanted[2] |= angle < -M_PI / 8 && angle > -M_PI + M_PI / 8;
        wanted[3] |= fabs(angle) < 3 * M_PI / 8;
        wanted[1] |= fabs(angle) > 5 * M_PI / 8;
    }
    if (testMove) {   // essai (testStep)
        static double angle = -1;
        if (angle < 0) { const char *e = getenv("P2_TOUCH_TEST_ANGLE"); angle = e ? atof(e) * M_PI / 180 : 0; }
        ax = testMove * sin(angle); ay = testMove * cos(angle);
        memset(wanted, 0, sizeof(wanted)); wanted[testMove > 0 ? 0 : 2] = YES;
    }
    if (pad) { [self padX:ax y:ay]; memset(wanted, 0, sizeof(wanted)); }
    [self setDirections:wanted];
}

// Mode d'essai, sans toucher l'écran (simulateur) : P2_TOUCH_TEST_MOVE="0.5,1"
// incline le stick de chaque valeur (négative : vers l'arrière) pendant
// P2_TOUCH_TEST_SECONDS (2 par défaut), après P2_TOUCH_TEST_DELAY secondes de
// partie (20 par défaut), trois secondes de repos entre deux ; direction
// P2_TOUCH_TEST_ANGLE (degrés, 0 vers l'avant, 90 à droite). La surcouche n'est
// alors pas dessinée. Vitesse lue à l'écran avec « +cl_showpos 1 » ; avec
// « +sv_cheats 1 +host_framerate 0.0166667 », la physique est celle de 60
// images/s malgré la lenteur du simulateur. P2_TOUCH_TEST_WASD=1 garde les
// touches W/A/S/D (témoin). Sur simulateur, SDL_JOYSTICK_MFI=0 écarte la
// manette MFi « Gamepad » qu'il expose et que le jeu prendrait avant celle-ci.
// P2_TOUCH_TEST_CLICK="s,x,y" : clic gauche (points de la fenêtre SDL) s secondes
// après le premier affichage d'un menu, pour ouvrir un écran sans toucher.
- (void)testClick {
    static double at = -1, x, y; static Uint64 when; static int step;
    if (at == -1) { const char *e = getenv("P2_TOUCH_TEST_CLICK");
        if (!e || sscanf(e, "%lf,%lf,%lf", &at, &x, &y) != 3 || at <= 0) at = -2; }
    if (at <= 0 || !menu) return;
    Uint64 now = SDL_GetTicks64();
    if (!when) { when = now + (Uint64)(at * 1000); return; }
    if (now < when) return;
    CGPoint p = CGPointMake(x, y);
    if (step == 0) {   // survol, puis appui et relâchement sur des images distinctes
        SDL_Event e = {0};
        e.type = SDL_MOUSEMOTION; e.motion.windowID = SDL_GetWindowID(window);
        e.motion.x = (Sint32)x; e.motion.y = (Sint32)y;
        SDL_PushEvent(&e);
    } else if (step == 1) [self mouseButton:SDL_BUTTON_LEFT down:YES at:p];
    else { [self mouseButton:SDL_BUTTON_LEFT down:NO at:p]; at = -2; }
    fprintf(stderr, "[ios] essai clic %d à (%.0f, %.0f), %llu ms\n", step, x, y, (unsigned long long)now);
    ++step; when = now + 300;
}
- (void)testStep {
    [self testClick];
    static const char *plan; static double seconds, delay; static int parsed;
    if (!parsed) {
        parsed = 1; plan = getenv("P2_TOUCH_TEST_MOVE");
        const char *e = getenv("P2_TOUCH_TEST_SECONDS"); seconds = e && atof(e) > 0 ? atof(e) : 2;
        e = getenv("P2_TOUCH_TEST_DELAY"); delay = e && atof(e) >= 0 ? atof(e) : 20;
    }
    if (!plan || !*plan) return;
    Uint64 now = SDL_GetTicks64();
    if (menu || (!pad && !padRefused)) { testNext = 0; return; }   // en partie, manette prête
    if (!testNext) { testNext = now + (Uint64)(delay * 1000); testPhase = 0; return; }
    if (now < testNext) return;
    // Étapes paires : début d'inclinaison ; impaires : fin et repos.
    const char *at = plan;
    for (int i = 0; i < testPhase / 2 && at; ++i) { at = strchr(at, ','); if (at) ++at; }
    if (!at || !*at) { if (testMove) { testMove = 0; [self updateMove]; } return; }
    BOOL start = testPhase % 2 == 0;
    testMove = start ? atof(at) : 0;
    fprintf(stderr, "[ios] essai stick : %s inclinaison %.2f à %llu ms\n", start ? "début" : "fin", atof(at),
            (unsigned long long)now);
    [self updateMove];
    testNext = now + (Uint64)((start ? seconds : 3.0) * 1000);
    ++testPhase;
}

// ---- manette virtuelle ----
// Présente tant qu'aucune vraie manette n'est connectée, une fois les manettes
// de SDL initialisées par le jeu (inputsystem) : elle lui arrive comme une
// manette branchée en cours de partie.
- (void)syncPad {
    if (getenv("P2_TOUCH_TEST_WASD")) padRefused = YES;   // essai témoin : touches W/A/S/D
    BOOL wanted = !physicalPad && !padRefused && SDL_WasInit(SDL_INIT_GAMECONTROLLER);
    if (wanted && !pad) {
        if (!(pad = virtual_pad())) padRefused = YES;
        rt_ios_move_response_linear(pad != NULL);
        padAxes[0] = padAxes[1] = 0;
        [self updateMove];
    } else if (!wanted && pad) {
        remove_virtual_pad(pad);
        pad = NULL;
        rt_ios_move_response_linear(0);
        [self updateMove];   // le stick repasse aux touches
    }
}
// Inclinaison (-1..1 après zone morte, y vers le haut) -> axe gauche de SDL
// (y vers le bas). Le jeu (in_joystick.cpp du client) calcule par axe
// t = (|axe| - 0,15) / 0,85 (seuils joy_forwardthreshold/joy_sidethreshold),
// applique la courbe joy_response_move puis avance de 175 × t unités/s
// (mesuré dans le simulateur). On inverse ce calcul pour que la vitesse suive
// l'inclinaison, de 40 à 175 unités/s : sous 40, le frottement du sol de
// Source l'emporte sur l'accélération (sv_friction × sv_stopspeed /
// sv_accelerate) et le joueur ne fait que glisser.
static const CGFloat kMinWalk = 40.0 / 175, kAxisThreshold = .15;
// Composante de vitesse voulue (-1..1, en vitesse de marche) -> axe SDL.
static Sint16 axis_value(CGFloat speed, int response) {
    CGFloat s = fmin(1, fabs(speed)), t = s;
    if (s < 1e-3) return 0;
    // Réponse 5 (si le passage en linéaire a échoué) : t réduit de moitié
    // jusqu'à 0,6 ; les vitesses entre 30 et 60 % sont inaccessibles, la plus
    // proche est prise.
    if (response != 0) t = s <= .3 ? 2 * s : s < .45 ? .6 : fmax(s, .601);
    return (Sint16)lround(copysign(kAxisThreshold + (1 - kAxisThreshold) * fmin(1, t), speed) * 32767);
}
- (void)padX:(CGFloat)x y:(CGFloat)y {
    CGFloat d = fmin(1, hypot(x, y)), m = d > 0 ? (kMinWalk + (1 - kMinWalk) * d) / d : 0;
    int response = rt_ios_move_response();
    Sint16 v[2] = {axis_value(x * m, response), axis_value(-y * m, response)};
    for (int i = 0; i < 2; ++i)
        if (v[i] != padAxes[i]) { padAxes[i] = v[i]; SDL_JoystickSetVirtualAxis(pad, i, v[i]); }
}
- (BOOL)roleTaken:(Role)role {
    for (P2Finger *f in fingers.objectEnumerator) if (f.role == role) return YES;
    return NO;
}
- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    (void)event;
    concealed = NO;   // un toucher ramène la surcouche cachée pour une manette
    backtap_touch(touches);   // un toucher secoue le téléphone : pas de tape au dos autour
    for (UITouch *t in touches) {
        P2Finger *f = [P2Finger new];
        f.start = f.at = [t locationInView:self];
        // Coin haut gauche : affiche ou masque le compteur d'images, choix gardé
        // dans les réglages (clé P2ShowFPS de la page Réglages).
        if (CGRectContainsPoint([self fpsCorner], f.start)) {
            fpsHidden = !fpsHidden;
            [NSUserDefaults.standardUserDefaults setBool:!fpsHidden forKey:@"P2ShowFPS"];
            fps = 0; fpsAt = 0;
            redraw(self);
            continue;
        }
        int button = [self buttonAt:f.start];
        BOOL left = f.start.x < CGRectGetMidX(self.bounds);
        if (button >= 0) { f.role = ROLE_BUTTON; f.action = (Action)button; [self action:f.action down:YES]; }
        else if (left && ![self roleTaken:ROLE_MOVE]) f.role = ROLE_MOVE;
        else if (!left && ![self roleTaken:ROLE_LOOK]) f.role = ROLE_LOOK;
        else continue;   // second doigt sur un stick déjà tenu : ignoré
        [fingers setObject:f forKey:t];
    }
    [self updateMove]; redraw(self);
}
- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    (void)event;
    for (UITouch *t in touches) {
        P2Finger *f = [fingers objectForKey:t];
        CGPoint p = [t locationInView:self];
        if (f && f.role == ROLE_LOOK && trackpad) {
            accel_init();
            double dx = p.x - f.at.x, dy = p.y - f.at.y, dt = t.timestamp - f.stamp;
            // dt borné : premier mouvement (stamp nul) ou doigt resté immobile longtemps
            if (f.stamp > 0 && dt > 1e-4) f.speed += accel.smooth * (fmin(hypot(dx, dy) / fmin(dt, 0.1), 1e4) - f.speed);
            f.stamp = t.timestamp;
            float k = kTrackpadLook * look_speed() * trackpad_gain(f.speed);
            if (accel.log) fprintf(stderr, "[ios] trackpad %.0f pt/s gain %.2f\n", f.speed, k / (kTrackpadLook * look_speed()));
            [self lookBy:(float)dx * k y:(float)dy * k];
        }
        f.at = p;
    }
    [self updateMove]; redraw(self);
}
- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    (void)event;
    backtap_touch(touches);
    for (UITouch *t in touches) {
        P2Finger *f = [fingers objectForKey:t];
        if (f.role == ROLE_BUTTON) [self action:f.action down:NO];
        [fingers removeObjectForKey:t];
    }
    [self updateMove]; redraw(self);
}
- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event { [self touchesEnded:touches withEvent:event]; }
- (void)safeAreaInsetsDidChange { [super safeAreaInsetsDidChange]; redraw(self); }

// ---- dessin ----
- (void)drawStick:(CGContextRef)c right:(BOOL)right ink:(UIColor *)ink {
    CGFloat r = self.stickRadius;
    CGPoint origin = [self restingStick:right], knob = origin;
    for (P2Finger *f in fingers.objectEnumerator)
        if (f.role == (right ? ROLE_LOOK : ROLE_MOVE)) { origin = f.start; knob = f.at; }
    CGFloat dx = knob.x - origin.x, dy = knob.y - origin.y, dist = hypot(dx, dy);
    if (dist > r) knob = CGPointMake(origin.x + dx * r / dist, origin.y + dy * r / dist);
    CGContextSetStrokeColorWithColor(c, ink.CGColor); CGContextSetFillColorWithColor(c, ink.CGColor);
    CGContextStrokeEllipseInRect(c, CGRectMake(origin.x - r, origin.y - r, 2 * r, 2 * r));
    CGContextFillEllipseInRect(c, CGRectMake(knob.x - kButtonSize / 2, knob.y - kButtonSize / 2, kButtonSize, kButtonSize));
}
// Compteur d'images, relevé deux fois par seconde (redessiner l'incrustation
// coûte : seulement quand la valeur change). P2_SHOW_FPS=0 (Réglages) le masque.
- (void)updateFPS {
    if (fpsHidden) return;
    CFTimeInterval now = CACurrentMediaTime();
    if (!fpsAt) { fpsAt = now; fpsFrames = rt_ios_frames; return; }
    if (now - fpsAt < .5) return;
    int value = (int)lround((double)(rt_ios_frames - fpsFrames) / (now - fpsAt));
    fpsAt = now; fpsFrames = rt_ios_frames;
    if (value != fps) { fps = value; redraw(self); }
}
// Police pixel 5×7 du compteur : chiffres et « FPS », une ligne par octet
// (bit 4 : colonne de gauche), dessinée carré par carré sans lissage.
static const char kPixelChars[] = "0123456789FPSMETALOGN";
static const uint8_t kPixelGlyphs[][7] = {
    {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E}, {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E},
    {0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F}, {0x1F, 0x02, 0x04, 0x02, 0x01, 0x11, 0x0E},
    {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02}, {0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E},
    {0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E}, {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08},
    {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E}, {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C},
    {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10}, {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10},
    {0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E},
    {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11}, {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F},   // M E
    {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}, {0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11},   // T A
    {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F}, {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E},   // L O
    {0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0F}, {0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11},   // G N
};
static const CGFloat kPixel = 2;   // points par pixel de la police (6 pixels d'écran sur un iPhone ×3)
static void draw_pixel_text(CGContextRef c, const char *text, CGPoint at) {
    for (int i = 0; text[i]; ++i) {
        const char *glyph = strchr(kPixelChars, text[i]);
        if (!glyph || !text[i]) continue;   // espace
        const uint8_t *rows = kPixelGlyphs[glyph - kPixelChars];
        for (int y = 0; y < 7; ++y)
            for (int x = 0; x < 5; ++x)
                if (rows[y] & (0x10 >> x)) CGContextFillRect(c, CGRectMake(at.x + (i * 6 + x) * kPixel, at.y + y * kPixel, kPixel, kPixel));
    }
}
// En haut à gauche, dans la zone sûre (hors de l'arrondi et de l'encoche) :
// rendu sur la première ligne, cadence sur la seconde.
- (CGRect)fpsBox:(NSString *)renderer fps:(NSString *)text {
    CGRect a = self.area;
    CGFloat margin = 2 * kPixel;
    NSUInteger chars = MAX(renderer.length, text.length);
    return CGRectMake(round(CGRectGetMinX(a) + 8), round(CGRectGetMinY(a) + 8),
                      (chars * 6 - 1) * kPixel + 2 * margin, 16 * kPixel + 2 * margin);
}
// Zone qui bascule le compteur : le coin jusqu'au compteur le plus large, et
// un peu plus (le doigt couvre mal un si petit carré).
- (CGRect)fpsCorner {
    CGRect box = [self fpsBox:@"OPENGL" fps:@"000 FPS"];
    return CGRectMake(0, 0, CGRectGetMaxX(box) + 20, CGRectGetMaxY(box) + 20);
}
- (void)drawFPS {
    if (fps <= 0) return;
    NSString *text = [NSString stringWithFormat:@"%d FPS", fps];
    NSString *renderer = rt_ios_metal_active ? @"METAL" : @"OPENGL";   // runtime/ios/metal_ios.m
    CGRect box = [self fpsBox:renderer fps:text];
    CGContextRef c = UIGraphicsGetCurrentContext();
    CGContextSaveGState(c);
    CGContextSetShouldAntialias(c, NO);
    CGContextSetFillColorWithColor(c, [UIColor colorWithWhite:0 alpha:.5].CGColor);
    CGContextFillRect(c, box);
    // Vert au-dessus de 55 i/s, jaune au-dessus de 30, rouge en dessous.
    UIColor *ink = fps >= 55 ? [UIColor colorWithRed:.45 green:1 blue:.35 alpha:1]
                 : fps >= 30 ? [UIColor colorWithRed:1 green:.85 blue:.2 alpha:1]
                             : [UIColor colorWithRed:1 green:.35 blue:.3 alpha:1];
    CGPoint at = CGPointMake(box.origin.x + 2 * kPixel, box.origin.y + 2 * kPixel);
    CGPoint below = CGPointMake(at.x, at.y + 9 * kPixel);
    CGContextSetFillColorWithColor(c, UIColor.blackColor.CGColor);   // ombre d'un pixel
    draw_pixel_text(c, renderer.UTF8String, CGPointMake(at.x + kPixel / 3, at.y + kPixel / 3));
    draw_pixel_text(c, text.UTF8String, CGPointMake(below.x + kPixel / 3, below.y + kPixel / 3));
    CGContextSetFillColorWithColor(c, UIColor.whiteColor.CGColor);
    draw_pixel_text(c, renderer.UTF8String, at);
    CGContextSetFillColorWithColor(c, ink.CGColor);
    draw_pixel_text(c, text.UTF8String, below);
    CGContextRestoreGState(c);
}
- (void)drawRect:(CGRect)rect {
    (void)rect;
    if (!concealed) [self drawFPS];   // aussi dans les menus
    if (concealed || menu || getenv("P2_TOUCH_TEST_MOVE")) return;   // essai : écran dégagé
    CGContextRef c = UIGraphicsGetCurrentContext();
    UIColor *ink = [UIColor colorWithWhite:1 alpha:.3], *line = [UIColor colorWithWhite:1 alpha:.45];
    CGContextSetLineWidth(c, 2);
    [self drawStick:c right:NO ink:ink];
    if (!trackpad) [self drawStick:c right:YES ink:ink];
    Button b[MAX_BUTTONS]; int n = [self buttons:b];
    for (int i = 0; i < n; ++i) {
        CGFloat s = b[i].size;
        CGRect box = CGRectMake(b[i].at.x - s / 2, b[i].at.y - s / 2, s, s);
        UIColor *fill = held[b[i].action] ? [b[i].color colorWithAlphaComponent:.9] : b[i].color;
        CGContextSetFillColorWithColor(c, fill.CGColor); CGContextFillEllipseInRect(c, box);
        CGContextSetStrokeColorWithColor(c, line.CGColor); CGContextStrokeEllipseInRect(c, box);
        if (!b[i].symbol) continue;
        UIImageSymbolConfiguration *config = [UIImageSymbolConfiguration configurationWithPointSize:s * .38
                                                                                             weight:UIImageSymbolWeightBold];
        UIImage *icon = [[UIImage systemImageNamed:b[i].symbol withConfiguration:config]
                         imageWithTintColor:[UIColor colorWithWhite:1 alpha:.85] renderingMode:UIImageRenderingModeAlwaysOriginal];
        CGSize z = icon.size;
        [icon drawInRect:CGRectMake(b[i].at.x - z.width / 2, b[i].at.y - z.height / 2, z.width, z.height)];
    }
}
@end

static P2TouchControls *controls;

// Appelé à la création de la fenêtre du jeu (sdl_ios.m), sur le thread principal.
void rt_ios_touch_controls_attach(SDL_Window *window) {
    const char *off = getenv("P2_TOUCH_CONTROLS");
    if (controls || !window || (off && *off == '0')) return;
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    if (!SDL_GetWindowWMInfo(window, &info) || info.subsystem != SDL_SYSWM_UIKIT) {
        fprintf(stderr, "[ios] commandes tactiles : fenêtre UIKit introuvable (%s)\n", SDL_GetError());
        return;
    }
    // Sur la fenêtre elle-même : SDL remplace la vue de son contrôleur par la vue
    // OpenGL à la création du contexte, ce qui retirerait une sous-vue posée là.
    UIView *host = info.info.uikit.window;
    if (!host) return;
    controls = [[P2TouchControls alloc] initWithWindow:window];
    controls.frame = host.bounds;
    [host addSubview:controls];
}
#endif
