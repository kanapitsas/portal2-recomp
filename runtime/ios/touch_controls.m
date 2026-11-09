// Commandes tactiles de Portal 2 et de Half-Life 2 sur iPhone/iPad.
//
// Les boutons dépendent du jeu du build (game_ios.c, clé P2Game de
// l'Info.plist) : dispositions dans touch_layout.c. Ce qui suit décrit Portal 2 ;
// HL2 garde les sticks et ajoute tir (glisser vise), tir secondaire,
// rechargement, sprint et accroupi en bascule, lampe, armes et sauvegarde rapide.
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
//   nourrie par CoreMotion dans backtap_ios.m (P2_BACKTAP=0 la coupe, réglages
//   dans docs/ios.md) ; visée au gyroscope dans gyro_ios.m.
// Avec une manette ou un clavier connecté, elle se cache jusqu'au premier
// toucher ; avec une vraie manette, la manette virtuelle est retirée pour que
// le jeu ne lise qu'elle. P2_TOUCH_CONTROLS=0 la désactive, =1 l'affiche toujours ; P2_TOUCH_LOOK règle la vitesse
// de la caméra (1 par défaut, stick et trackpad). Mode d'essai sans toucher
// (simulateur, P2_TOUCH_TEST_*) : touch_test.m.
//
// Disposition et dessin inspirés de la manette tactile de TOTK de
// MacNativeGames (games/totk/ios/Sources/touch_pad.m, licence MIT).
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <GameController/GameController.h>
#import <QuartzCore/QuartzCore.h>
#import <UIKit/UIKit.h>
#include <SDL.h>
#include <SDL_syswm.h>
#include <math.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "backtap_ios.h"
#include "env_ios.h"
#include "fps_overlay.h"
#include "game_ios.h"
#include "gyro_ios.h"
#include "touch_layout.h"
#include "touch_test.h"
#include "lifecycle_ios.h"

int rt_ios_game_ui_visible(void);   // game_state.c
extern _Atomic unsigned long rt_ios_frames;  // gl_ios.m : images présentées
extern _Atomic int rt_ios_metal_active;      // metal_ios.m : rendu Direct3D 9 en cours
void rt_ios_move_response_linear(int on);
int rt_ios_move_response(void);

typedef enum { ROLE_MOVE, ROLE_LOOK, ROLE_BUTTON } Role;
typedef rt_touch_action Action;
enum { MOUSE_LEFT = RT_TOUCH_FIRE, MOUSE_RIGHT = RT_TOUCH_ALT_FIRE, ACTION_COUNT = RT_TOUCH_ACTION_COUNT };
typedef struct { Action action; NSString *symbol; UIColor *color; CGPoint at; CGFloat size; unsigned flags; } Button;
enum { MAX_BUTTONS = 16 };

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
// Le jeu range les manettes dans quatre places fixes, une par joueur d'écran
// partagé (inputsystem.so, 0x125a0), et le joueur 1 ne lit que la première.
// Une vraie manette connectée pendant que la virtuelle tient cette place
// prend la deuxième et y reste après le retrait de la virtuelle : elle pilote
// alors les menus (qui écoutent toutes les manettes) mais pas le jeu. On la
// débranche puis rebranche pour le jeu : un SDL_CONTROLLERDEVICEADDED seul ne
// suffit pas, car pour une manette déjà connue le jeu la remet dans la même
// place ; seule une manette inconnue va dans la première place libre. SDL
// notifie le retrait de la virtuelle à l'observateur du jeu avant le retour
// de SDL_JoystickDetachVirtual, donc cette place est déjà libre ici, et les
// deux événements poussés ci-dessous lui parviennent tout de suite, dans cet
// ordre (observateurs appelés par SDL_PushEvent).
static void replug_controllers(void) {
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (!SDL_IsGameController(i)) continue;   // accéléromètre de l'iPhone, entre autres
        SDL_Event event = {0};
        event.type = SDL_CONTROLLERDEVICEREMOVED;
        event.cdevice.which = SDL_JoystickGetDeviceInstanceID(i);
        SDL_PushEvent(&event);
        event.type = SDL_CONTROLLERDEVICEADDED;
        event.cdevice.which = i;
        SDL_PushEvent(&event);
        fprintf(stderr, "[ios] manette %d (%s) rebranchée pour le joueur 1\n", i, SDL_JoystickNameForIndex(i));
    }
}
static void remove_virtual_pad(SDL_Joystick *pad) {
    SDL_JoystickID id = SDL_JoystickInstanceID(pad);
    SDL_JoystickClose(pad);
    for (int i = 0; i < SDL_NumJoysticks(); ++i)
        if (SDL_JoystickGetDeviceInstanceID(i) == id) { SDL_JoystickDetachVirtual(i); break; }
    fprintf(stderr, "[ios] manette virtuelle retirée\n");
    replug_controllers();
}

static const CGFloat kButtonSize = 54;   // boutons : touch_layout.c
static const float kStickLook = 90;      // comptes de souris par image, stick à fond (≈ 360°/s à la sensibilité 3 du jeu)
static const float kTrackpadLook = 3.5f; // comptes de souris par point de glissement
static NSString *const kTrackpadKey = @"P2TouchTrackpad";
static float look_speed(void) {
    static float speed;
    if (!speed) speed = rt_ios_env_float("P2_TOUCH_LOOK", 1, 1e-6f);   // > 0
    return speed;
}

// Accélération du trackpad : gain 1 sous P2_TOUCH_ACCEL_LOW pt/s (visée fine
// inchangée), puis montée en douceur (smoothstep) jusqu'à P2_TOUCH_ACCEL
// (multiplicateur maximal) à P2_TOUCH_ACCEL_HIGH pt/s. P2_TOUCH_ACCEL=1 la coupe.
// La vitesse du doigt est lissée par moyenne exponentielle (P2_TOUCH_ACCEL_SMOOTH,
// poids du nouvel échantillon) pour qu'un échantillon isolé ne fasse pas sauter le gain.
static struct { float max, low, high, smooth; int log; } accel;
static float trackpad_gain(double speed);
static void accel_init(void) {
    if (accel.max) return;
    accel.max = rt_ios_env_float("P2_TOUCH_ACCEL", 2.5f, 1);
    accel.low = rt_ios_env_float("P2_TOUCH_ACCEL_LOW", 300, 0);
    accel.high = rt_ios_env_float("P2_TOUCH_ACCEL_HIGH", 1500, accel.low + 1);
    if (accel.high <= accel.low) accel.high = accel.low + 1;
    accel.smooth = fminf(1, rt_ios_env_float("P2_TOUCH_ACCEL_SMOOTH", 0.5f, 0.01f));
    accel.log = rt_ios_env_flag("P2_TOUCH_ACCEL_LOG", 0);
    fprintf(stderr, "[ios] trackpad : gain %.2f à %g pt/s, %.2f à %g, %.2f à %g, %.2f à 3000 pt/s\n",
        trackpad_gain(100), 100.0, trackpad_gain(600), 600.0, trackpad_gain(1000), 1000.0, trackpad_gain(3000));
}
static float trackpad_gain(double speed) {
    double t = fmin(1, fmax(0, (speed - accel.low) / (accel.high - accel.low)));
    return 1 + (accel.max - 1) * (float)(t * t * (3 - 2 * t));
}

// Dessin : des calques fixes (un CAShapeLayer par stick et par bouton, l'icône
// en sous-calque, le compteur dans un petit calque bitmap) déplacés ou recolorés
// sans animation, au lieu d'une vue plein écran redessinée à l'échelle native
// (×3) à chaque mouvement de doigt. Un changement marque la surcouche
// (setNeedsRender) ; elle est mise à jour une fois par passage de la boucle
// d'événements (observateur de fin de passage, et minuterie step à défaut).
// La boucle de SDL (CFRunLoopRunInMode à délai nul) ne laisse jamais UIKit
// valider ses transactions : la mise à jour se termine par un CATransaction
// flush, qui n'envoie plus que quelques propriétés de calques.

// Vibration légère du Taptic Engine à chaque portail tiré depuis la surcouche
// (bouton ou tape au dos ; une vraie manette ne passe pas par ici).
// P2_HAPTICS=0 (Réglages > Vibrer au tir) la coupe. Générateur préparé
// d'avance (en partie, au plus une fois par seconde, et après chaque tir) :
// sans cela, la première vibration part avec un retard.
static UIImpactFeedbackGenerator *haptic;
static void haptic_prepare(void) {
    static int wanted = -1;
    if (wanted < 0) {
        // Portal 2 seulement : HL2 n'a pas ce réglage (tir tenu, pas de portails).
        wanted = rt_ios_current_game()->layout == RT_IOS_LAYOUT_PORTAL2 && rt_ios_env_flag("P2_HAPTICS", 1);
        fprintf(stderr, "[ios] vibration au tir : %s\n", wanted ? "oui" : "non");
    }
    if (!wanted) return;
    if (!haptic) haptic = [[UIImpactFeedbackGenerator alloc] initWithStyle:UIImpactFeedbackStyleLight];
    [haptic prepare];
}
static void haptic_fire(void) {
    if (!haptic) return;
    [haptic impactOccurred];
    [haptic prepare];
}

// Appui tenu (touche ou bouton de souris) relâché après un délai et au moins
// deux images présentées : vgui ignore un clic dont l'appui et le relâchement
// tombent dans la même image. Tapes au dos et touches brèves (sauvegarde rapide).
typedef struct { int active, code; CFTimeInterval at; unsigned long frames; } Held;
static void held_begin(Held *h, int code) { h->active = 1; h->code = code; h->at = CACurrentMediaTime(); h->frames = rt_ios_frames; }
static BOOL held_due(const Held *h, CFTimeInterval hold) {
    return CACurrentMediaTime() - h->at >= hold && rt_ios_frames >= h->frames + 2;
}
// Capteurs de mouvement en marche ? En partie oui ; sinon encore 2 s (menu,
// surcouche cachée pour une manette ou un clavier), et plus du tout dès que
// l'app est quittée (iOS peut la suspendre avant 2 s). idle_since : depuis
// quand on n'est plus en partie (0 : en partie).
static BOOL sensors_wanted(CFTimeInterval *idle_since, BOOL playing, CFTimeInterval now) {
    if (playing) *idle_since = 0;
    else if (!*idle_since) *idle_since = now;
    return playing || (now - *idle_since < 2 && !rt_ios_app_state());
}

// Doigt suivi : rôle, action (bouton), point de départ, dernier point.
@interface P2Finger : NSObject
@property Role role; @property Action action; @property CGPoint start, at;
@property BOOL toggle, aim;   // bouton à bascule ; bouton qui vise en glissant (tir de HL2)
@property NSTimeInterval stamp; @property double speed;   // trackpad : horodatage, vitesse lissée (pt/s)
@end
@implementation P2Finger
@end

// Ce que montre le compteur : comparé avant de construire textes et bitmap.
typedef struct { long taps, fixes, reported; CGRect area; int fps, metal, lit; } FpsKey;

@interface P2TouchControls : UIView
- (instancetype)initWithWindow:(SDL_Window *)window;
@end

@implementation P2TouchControls {
    SDL_Window *window;
    Uint32 windowID;          // SDL_GetWindowID(window), porté par chaque événement poussé
    NSMapTable<UITouch *, P2Finger *> *fingers;
    int held[ACTION_COUNT];   // nombre de doigts par action
    BOOL toggled[ACTION_COUNT]; // boutons à bascule enfoncés (sprint, accroupi de HL2)
    BOOL wasd[4];             // W, A, S, D enfoncées
    BOOL concealed;           // cachée pour une manette ou un clavier
    BOOL menu;                // interface du jeu affichée : surcouche effacée
    NSTimer *tick;            // caméra : un pas par image tant que le stick est incliné
    float lookCarry[2];       // fractions de comptes de souris reportées
    BOOL trackpad;            // moitié droite en trackpad plutôt qu'en stick
    BOOL physicalPad;         // vraie manette connectée
    int fps;                  // compteur d'images affiché (0 : rien à afficher)
    int fpsHidden;            // masqué : P2_SHOW_FPS=0 (Réglages) ou toucher du coin haut gauche
    CFTimeInterval fpsAt;     // instant du dernier relevé
    long dataTaps, dataFixes, dataReported; // jeu de données des tapes au dos affiché (-1 : rien)
    CFTimeInterval flagAt;    // dernier appui sur ✗ (la ligne DATA s'allume une seconde)
    BOOL flagLit;
    SDL_Joystick *pad;        // manette virtuelle du stick gauche (NULL : W/A/S/D)
    BOOL padRefused;          // création refusée : ne pas réessayer à chaque image
    Sint16 padAxes[2];        // dernières valeurs envoyées
    int padResponse;          // courbe du jeu pour ces valeurs (joy_response_move)
    CGFloat testMove;         // essai (touch_test.m) : inclinaison avant imposée
    Held heldTap;             // tape au dos : bouton de souris tenu (code : Action)
    Held heldKey;             // touche brève tenue (code : SDL_Scancode)
    CFTimeInterval backtapIdleSince; // tapes impossibles depuis cet instant (0 : en partie)
    CFTimeInterval gyroIdleSince;    // visée au gyroscope impossible depuis cet instant (0 : en partie)
    CFTimeInterval hapticPreparedAt; // dernière préparation de la vibration au tir
    BOOL needsRender;         // calques à mettre à jour (renderIfNeeded)
    CAShapeLayer *stickRing[2], *stickKnob[2];   // gauche, droit
    CGFloat ringRadius[2];
    CAShapeLayer *buttonLayer[MAX_BUTTONS];
    CALayer *iconLayer[MAX_BUTTONS];
    CGFloat buttonSize[MAX_BUTTONS], iconSize[MAX_BUTTONS];
    NSString *iconSymbol[MAX_BUTTONS];
    Button layoutButtons[MAX_BUTTONS];   // disposition des boutons, refaite quand la zone ou le mode change
    int layoutCount;
    BOOL layoutValid;
    CFTimeInterval quickSaveAt;   // appui sur la sauvegarde rapide en cours (0 : aucun)
    BOOL quickLoaded;             // appui long : chargement rapide déjà envoyé
    BOOL cancelling;              // touchesCancelled : relâchement sans effet (pas de sauvegarde)
    CALayer *fpsLayer;        // compteur d'images (bitmap de la taille de son cadre)
    FpsKey fpsKey;            // ce qu'il montre : redessiné seulement quand cela change
}

- (instancetype)initWithWindow:(SDL_Window *)sdlWindow {
    if ((self = [super initWithFrame:CGRectZero])) {
        window = sdlWindow;
        windowID = SDL_GetWindowID(window);
        self.multipleTouchEnabled = YES; self.backgroundColor = UIColor.clearColor; self.opaque = NO;
        fingers = [NSMapTable strongToStrongObjectsMapTable];
        trackpad = [NSUserDefaults.standardUserDefaults boolForKey:kTrackpadKey];
        NSNotificationCenter *center = NSNotificationCenter.defaultCenter;
        for (NSNotificationName name in @[GCControllerDidConnectNotification, GCControllerDidDisconnectNotification,
                                          GCKeyboardDidConnectNotification, GCKeyboardDidDisconnectNotification])
            [center addObserver:self selector:@selector(devicesChanged) name:name object:nil];
        [self devicesChanged];
        dataTaps = -1;
        fpsHidden = !rt_ios_env_flag("P2_SHOW_FPS", 1);   // posée par les Réglages (runtime/ios/settings.c)
        rt_ios_backtap_start();
        rt_ios_gyro_start(rt_ios_current_game()->layout == RT_IOS_LAYOUT_HL2);
        [self setupLayers];
        __weak P2TouchControls *weakSelf = self;
        // Fin de chaque passage de la boucle (SDL la fait tourner à chaque
        // lecture d'événements) : les touchers de ce passage sont affichés d'un coup.
        CFRunLoopObserverRef observer = CFRunLoopObserverCreateWithHandler(NULL, kCFRunLoopBeforeWaiting | kCFRunLoopExit, true, 0,
            ^(CFRunLoopObserverRef o, CFRunLoopActivity activity) { (void)o; (void)activity; [weakSelf renderIfNeeded]; });
        CFRunLoopAddObserver(CFRunLoopGetMain(), observer, kCFRunLoopCommonModes);
        CFRelease(observer);
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
    if (rt_ios_env_flag("P2_TOUCH_CONTROLS", 0)) any = NO;   // 1 : jamais cachée (essais sur simulateur)
    if (any != concealed) { concealed = any; [self setNeedsRender]; }
}

// À chaque image : rester devant la vue OpenGL de SDL (ajoutée après la
// fenêtre), suivre la taille de la fenêtre, tourner la caméra.
- (void)step {
    UIView *host = self.superview;
    CFTimeInterval now = CACurrentMediaTime();
    // App inactive (lifecycle_ios.m, pause servant la boucle d'UIKit) : comme en
    // menu, doigts relâchés et plus aucune entrée envoyée au jeu.
    BOOL ui = rt_ios_game_ui_visible() != 0 || rt_ios_app_state();
    if (ui != menu) {
        menu = ui;
        if (menu) [self releaseAll];
        [self setNeedsRender];
    }
    [self updateFPS:now];
    [self syncPad];
    if (pad && rt_ios_move_response() != padResponse) { padResponse = rt_ios_move_response(); [self updateMove]; }
    [self testStep];
    // Capteurs de mouvement : orientation de l'écran lue une fois par image.
    BOOL playing = !menu && !concealed;   // menu vaut aussi pour l'app quittée
    int orient = rt_ios_backtap_available() || rt_ios_gyro_available() ? [self orientationSign] : 0;
    [self backTapStep:orient playing:playing now:now];
    [self gyroStep:orient playing:playing now:now];
    [self quickSaveStep:now];
    if (playing && now - hapticPreparedAt > 1) {
        hapticPreparedAt = now;
        haptic_prepare();
    }
    if (host.subviews.lastObject != self) [host bringSubviewToFront:self];
    if (!CGRectEqualToRect(self.frame, host.bounds)) { self.frame = host.bounds; [self invalidateLayout]; }
    [self renderIfNeeded];
    if (trackpad) return;   // trackpad : mouvements envoyés au glissement (touchesMoved)
    for (P2Finger *f in fingers.objectEnumerator) {
        if (f.role != ROLE_LOOK) continue;
        CGFloat x, y; [self deflectionOf:f x:&x y:&y];
        float scale = kStickLook * look_speed();   // quadratique : lent près du centre pour viser
        [self lookBy:(float)(x * fabs(x)) * scale y:(float)(-y * fabs(y)) * scale];
    }
}
// +1 : axe y de l'appareil vers la droite de l'écran ; -1 : vers la gauche ; 0 : portrait.
- (int)orientationSign {
    UIInterfaceOrientation o = self.window.windowScene.interfaceOrientation;
    return o == UIInterfaceOrientationLandscapeLeft ? 1 : o == UIInterfaceOrientationLandscapeRight ? -1 : 0;
}
// Mouvement de souris relatif, fractions de comptes reportées.
- (void)lookBy:(float)x y:(float)y {
    lookCarry[0] += x; lookCarry[1] += y;
    int dx = (int)lookCarry[0], dy = (int)lookCarry[1];
    lookCarry[0] -= (float)dx; lookCarry[1] -= (float)dy;
    if (!dx && !dy) return;
    rt_ios_backtap_look(labs((long)dx) + labs((long)dy));   // règle « mauvais côté » du jeu de données
    SDL_Event e = {0};
    e.type = SDL_MOUSEMOTION;
    e.motion.xrel = dx; e.motion.yrel = dy;
    [self push:&e];
}
// Tape au dos acceptée : appui, puis relâchement après P2_BACKTAP_HOLD et au
// moins deux images (Held). Passe par action: comme un bouton tenu : si un
// doigt tient déjà ce portail, rien n'est renvoyé au jeu.
- (void)backTapStep:(int)orient playing:(BOOL)playing now:(CFTimeInterval)now {
    if (!rt_ios_backtap_available()) return;
    rt_ios_backtap_set_state(orient, playing);
    rt_ios_backtap_run(sensors_wanted(&backtapIdleSince, playing, now));
    int side = rt_ios_backtap_take();
    if (side && playing && !heldTap.active) {
        held_begin(&heldTap, side < 0 ? MOUSE_LEFT : MOUSE_RIGHT);
        [self action:(Action)heldTap.code down:YES];
        [self setNeedsRender];   // le bouton du portail s'allume
    }
    [self backTapRelease:NO];
}
// Visée au gyroscope : mouvement envoyé comme celui du stick ; hors partie,
// ce qui reste est jeté.
- (void)gyroStep:(int)orient playing:(BOOL)playing now:(CFTimeInterval)now {
    if (!rt_ios_gyro_available()) return;
    rt_ios_gyro_set_orient(orient);
    rt_ios_gyro_run(sensors_wanted(&gyroIdleSince, playing, now));
    double d[2];
    rt_ios_gyro_take(d);
    if (playing) [self lookBy:(float)d[0] y:(float)d[1]];
}
// Sauvegarde rapide : un appui court sauvegarde (F6), un appui d'une demi-seconde
// charge la dernière sauvegarde rapide (F9, « load quick » dans les touches par
// défaut de HL2), avec une vibration. La touche est donc envoyée au relâchement,
// ou au bout du délai, puis tenue 50 ms et au moins deux images, comme une
// tape au dos (Held). Un toucher annulé par iOS (appel, geste système) ou un
// menu qui s'ouvre ne sauvegarde pas.
static const CFTimeInterval kQuickLoadHold = 0.5;
- (void)tapKey:(SDL_Scancode)code {
    if (heldKey.active) [self key:(SDL_Scancode)heldKey.code down:NO];
    [self key:code down:YES];
    held_begin(&heldKey, code);
}
- (void)quickSaveStep:(CFTimeInterval)now {
    if (heldKey.active && held_due(&heldKey, 0.05)) {
        [self key:(SDL_Scancode)heldKey.code down:NO];
        heldKey.active = 0;
    }
    if (quickSaveAt && !quickLoaded && now - quickSaveAt >= kQuickLoadHold) {
        quickLoaded = YES;
        [self tapKey:SDL_SCANCODE_F9];
        haptic_fire();
    }
}
- (void)quickSave:(BOOL)down {
    if (down) { quickSaveAt = CACurrentMediaTime(); quickLoaded = NO; return; }
    if (quickSaveAt && !quickLoaded && !cancelling) [self tapKey:SDL_SCANCODE_F6];
    quickSaveAt = 0;
}
- (void)backTapRelease:(BOOL)now {
    if (!heldTap.active) return;
    if (!now && !held_due(&heldTap, rt_ios_backtap_hold())) return;
    [self action:(Action)heldTap.code down:NO];
    heldTap.active = 0;
    [self setNeedsRender];
}
- (void)toggleLookMode {
    trackpad = !trackpad;
    [NSUserDefaults.standardUserDefaults setBool:trackpad forKey:kTrackpadKey];
    lookCarry[0] = lookCarry[1] = 0;
    [self invalidateLayout];   // l'icône du mode change
}

// Hors partie, les doigts passent à la vue de SDL (clics dans les menus).
- (UIView *)hitTest:(CGPoint)point withEvent:(UIEvent *)event {
    return menu ? nil : [super hitTest:point withEvent:event];
}
- (void)releaseAll {
    quickSaveAt = 0;   // un menu qui s'ouvre n'est pas un appui court : pas de sauvegarde
    for (P2Finger *f in fingers.objectEnumerator) if (f.role == ROLE_BUTTON && !f.toggle) [self action:f.action down:NO];
    for (int a = 0; a < ACTION_COUNT; ++a) [self untoggle:(Action)a];
    [self backTapRelease:YES];
    [fingers removeAllObjects];
    [self updateMove];
}

// ---- événements SDL ----
// Pousse un événement pour la fenêtre du jeu (windowID selon son type).
- (void)push:(SDL_Event *)e {
    switch (e->type) {
    case SDL_KEYDOWN: case SDL_KEYUP: e->key.windowID = windowID; break;
    case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP: e->button.windowID = windowID; break;
    case SDL_MOUSEMOTION: e->motion.windowID = windowID; break;
    case SDL_MOUSEWHEEL: e->wheel.windowID = windowID; break;
    default: break;
    }
    SDL_PushEvent(e);
}
- (void)key:(SDL_Scancode)code down:(BOOL)down {
    SDL_Event e = {0};
    e.type = down ? SDL_KEYDOWN : SDL_KEYUP;
    e.key.state = down ? SDL_PRESSED : SDL_RELEASED;
    e.key.keysym.scancode = code; e.key.keysym.sym = SDL_GetKeyFromScancode(code);
    [self push:&e];
}
// Position en points de la fenêtre SDL (sdl_ios.m la convertit en pixels).
- (void)mouseButton:(Uint8)button down:(BOOL)down at:(CGPoint)p {
    SDL_Event e = {0};
    e.type = down ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;
    e.button.button = button;
    e.button.state = down ? SDL_PRESSED : SDL_RELEASED; e.button.clicks = 1;
    e.button.x = (Sint32)p.x; e.button.y = (Sint32)p.y;
    [self push:&e];
}
// Molette (armes de HL2) : un cran, vers le haut (invprev) ou le bas (invnext).
- (void)wheel:(int)y {
    SDL_Event e = {0};
    e.type = SDL_MOUSEWHEEL;
    e.wheel.y = y; e.wheel.preciseY = (float)y; e.wheel.direction = SDL_MOUSEWHEEL_NORMAL;
    [self push:&e];
}
- (void)action:(Action)action down:(BOOL)down {
    int before = held[action];
    held[action] = MAX(0, held[action] + (down ? 1 : -1));
    if ((before > 0) == (held[action] > 0)) return;
    CGPoint centre = CGPointMake(CGRectGetMidX(self.bounds), CGRectGetMidY(self.bounds));
    switch (action) {
    case RT_TOUCH_JUMP: [self key:SDL_SCANCODE_SPACE down:down]; break;
    case RT_TOUCH_CROUCH: [self key:SDL_SCANCODE_LCTRL down:down]; break;
    case RT_TOUCH_USE: [self key:SDL_SCANCODE_E down:down]; break;
    case RT_TOUCH_PAUSE: [self key:SDL_SCANCODE_ESCAPE down:down]; break;
    case RT_TOUCH_FIRE: [self mouseButton:SDL_BUTTON_LEFT down:down at:centre]; if (down) haptic_fire(); break;
    case RT_TOUCH_ALT_FIRE: [self mouseButton:SDL_BUTTON_RIGHT down:down at:centre]; if (down) haptic_fire(); break;
    case RT_TOUCH_LOOK_MODE: if (down) [self toggleLookMode]; break;
    case RT_TOUCH_FLAG_ERROR: if (down) { rt_ios_backtap_flag(); flagAt = CACurrentMediaTime(); flagLit = YES; [self setNeedsRender]; } break;
    case RT_TOUCH_RELOAD: [self key:SDL_SCANCODE_R down:down]; break;
    case RT_TOUCH_SPRINT: [self key:SDL_SCANCODE_LSHIFT down:down]; break;
    case RT_TOUCH_FLASHLIGHT: [self key:SDL_SCANCODE_F down:down]; break;
    case RT_TOUCH_QUICKSAVE: [self quickSave:down]; break;
    case RT_TOUCH_WEAPON_PREV: if (down) [self wheel:1]; break;
    case RT_TOUCH_WEAPON_NEXT: if (down) [self wheel:-1]; break;
    default: break;
    }
}
- (void)untoggle:(Action)action {
    if (!toggled[action]) return;
    toggled[action] = NO;
    [self action:action down:NO];
    [self setNeedsRender];
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
    // HL2 : stick de la caméra plus au centre, à l'écart de la grappe du tir.
    CGFloat x = right ? rt_touch_look_rest(rt_ios_current_game()->layout) : .16;
    return CGPointMake(CGRectGetMinX(a) + a.size.width * x, CGRectGetMinY(a) + a.size.height * .72);
}
// Les tapes au dos tirent les portails : leurs boutons ne servent que sans
// elles, sauf choix contraire (P2_PORTAL_BUTTONS=1 toujours, =0 jamais).
// Décidé une fois, après le démarrage des capteurs (initWithWindow:).
static BOOL portal_buttons(void) { return RT_IOS_ENV_FLAG("P2_PORTAL_BUTTONS", !rt_ios_backtap_available()); }
// Couleurs et icônes de la surcouche, créées une fois : renderIfNeeded est
// appelé à chaque mouvement de doigt (jusqu'à 120 fois par seconde).
static UIColor *blue, *orange, *ink, *icon_tint, *stick_ink, *line_ink, *red, *flag_red;
static void palette_init(void) {
    if (blue) return;
    blue = [UIColor colorWithRed:.16 green:.55 blue:1 alpha:.6]; orange = [UIColor colorWithRed:1 green:.55 blue:.05 alpha:.6];
    red = [UIColor colorWithRed:.75 green:.12 blue:.08 alpha:.45];          // tir de HL2
    flag_red = [UIColor colorWithRed:.6 green:.08 blue:.08 alpha:.45];      // ✗ du jeu de données
    ink = [UIColor colorWithWhite:0 alpha:.35];   // fond sombre : icônes lisibles sur un mur clair
    icon_tint = [UIColor colorWithWhite:1 alpha:.85];
    stick_ink = [UIColor colorWithWhite:1 alpha:.3]; line_ink = [UIColor colorWithWhite:1 alpha:.45];
}
// Bitmap transparent de cette taille (points), à l'échelle de l'écran.
static UIImage *render_bitmap(CGSize size, void (^draw)(CGContextRef c)) {
    UIGraphicsImageRendererFormat *format = [UIGraphicsImageRendererFormat preferredFormat];
    format.scale = UIScreen.mainScreen.scale; format.opaque = NO;
    format.preferredRange = UIGraphicsImageRendererFormatRangeStandard;
    UIGraphicsImageRenderer *renderer = [[UIGraphicsImageRenderer alloc] initWithSize:size format:format];
    return [renderer imageWithActions:^(UIGraphicsImageRendererContext *context) { draw(context.CGContext); }];
}
// Icône SF Symbols teintée, par (nom, taille en points) : au plus une par
// bouton et par taille (deux pour le mode de la caméra).
static UIImage *icon_image(NSString *symbol, CGFloat point_size) {
    static struct { NSString *symbol; CGFloat size; UIImage *image; } cache[20];
    static int count;
    for (int i = 0; i < count; ++i)
        if (cache[i].size == point_size && [cache[i].symbol isEqualToString:symbol]) return cache[i].image;
    UIImageSymbolConfiguration *config = [UIImageSymbolConfiguration configurationWithPointSize:point_size
                                                                                         weight:UIImageSymbolWeightBold];
    UIImage *tinted = [[UIImage systemImageNamed:symbol withConfiguration:config]
                       imageWithTintColor:icon_tint renderingMode:UIImageRenderingModeAlwaysOriginal];
    // Rendue une fois en bitmap à l'échelle de l'écran : le CGImage d'une icône
    // SF Symbols n'a pas sa teinte, et c'est lui que prend le calque.
    UIImage *image = render_bitmap(tinted.size, ^(CGContextRef c) {
        (void)c; [tinted drawInRect:CGRectMake(0, 0, tinted.size.width, tinted.size.height)];
    });
    if (count < (int)(sizeof(cache) / sizeof(*cache))) cache[count++] = (typeof(cache[0])){symbol, point_size, image};
    return image;
}
// Disposition des boutons dans la zone courante (layout:), calculée une fois
// par changement de zone ou de mode de la caméra.
- (int)buttons:(Button *)b {
    CGRect a = self.area;
    palette_init();
    int count;
    const rt_touch_button *layout = rt_touch_layout(rt_ios_current_game()->layout, &count);
    int n = 0;
    for (int i = 0; i < count && n < MAX_BUTTONS; ++i) {
        const rt_touch_button *t = &layout[i];
        if ((t->flags & RT_TOUCH_PORTAL) && !portal_buttons()) continue;
        if ((t->flags & RT_TOUCH_DATASET) && !rt_ios_backtap_dataset_on()) continue;
        double cx, cy;
        rt_touch_place(t, CGRectGetMinX(a), CGRectGetMinY(a), a.size.width, a.size.height, &cx, &cy);
        UIColor *color = t->color == RT_TOUCH_BLUE ? blue : t->color == RT_TOUCH_ORANGE ? orange
                       : t->color == RT_TOUCH_RED ? red : t->color == RT_TOUCH_FLAG ? flag_red : ink;
        // Mode de la caméra : l'icône montre le mode actuel.
        NSString *symbol = t->action == RT_TOUCH_LOOK_MODE ? (trackpad ? @"hand.draw.fill" : @"r.joystick.fill")
                         : t->symbol ? @(t->symbol) : nil;
        b[n++] = (Button){t->action, symbol, color, CGPointMake(cx, cy), t->size, t->flags};
    }
    return n;
}
- (void)invalidateLayout { layoutValid = NO; [self setNeedsRender]; }
// Boutons de la disposition courante (tableau interne) et leur nombre.
- (int)layout:(Button **)out {
    if (!layoutValid) { layoutCount = [self buttons:layoutButtons]; layoutValid = YES; }
    *out = layoutButtons;
    return layoutCount;
}
// Indice du bouton sous ce point dans la disposition, ou -1.
- (int)buttonAt:(CGPoint)p {
    Button *b; int n = [self layout:&b];
    for (int i = 0; i < n; ++i) if (hypot(p.x - b[i].at.x, p.y - b[i].at.y) < b[i].size * .62) return i;
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
    if (testMove) {   // essai (touch_test.m)
        double angle = rt_ios_touch_test_angle();
        ax = testMove * sin(angle); ay = testMove * cos(angle);
        memset(wanted, 0, sizeof(wanted)); wanted[testMove > 0 ? 0 : 2] = YES;
    }
    if (pad) { [self padX:ax y:ay]; memset(wanted, 0, sizeof(wanted)); }
    [self setDirections:wanted];
}
// Mode d'essai (simulateur, touch_test.m) : clic d'essai, puis inclinaison imposée.
- (void)testStep {
    double x, y;
    switch (rt_ios_touch_test_click(menu, &x, &y)) {
    case 1: {   // survol, puis appui et relâchement sur des images distinctes
        SDL_Event e = {0};
        e.type = SDL_MOUSEMOTION;
        e.motion.x = (Sint32)x; e.motion.y = (Sint32)y;
        [self push:&e];
        break;
    }
    case 2: [self mouseButton:SDL_BUTTON_LEFT down:YES at:CGPointMake(x, y)]; break;
    case 3: [self mouseButton:SDL_BUTTON_LEFT down:NO at:CGPointMake(x, y)]; break;
    default: break;
    }
    double move;
    if (rt_ios_touch_test_step(menu, pad != NULL || padRefused, &move)) { testMove = (CGFloat)move; [self updateMove]; }
}

// ---- manette virtuelle ----
// Présente tant qu'aucune vraie manette n'est connectée, une fois les manettes
// de SDL initialisées par le jeu (inputsystem) : elle lui arrive comme une
// manette branchée en cours de partie.
- (void)syncPad {
    if (rt_ios_touch_test_wasd()) padRefused = YES;   // essai témoin : touches W/A/S/D
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
// Un toucher secoue le téléphone : pas de tape au dos autour (début et fin).
static void backtap_touches(NSSet<UITouch *> *touches) {
    for (UITouch *t in touches) rt_ios_backtap_touch(t.timestamp);
}
- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    (void)event;
    concealed = NO;   // un toucher ramène la surcouche cachée pour une manette
    backtap_touches(touches);
    for (UITouch *t in touches) {
        P2Finger *f = [P2Finger new];
        f.start = f.at = [t locationInView:self];
        // Coin haut gauche : affiche ou masque le compteur d'images, choix gardé
        // dans les réglages (clé P2ShowFPS de la page Réglages).
        if (CGRectContainsPoint([self fpsCorner], f.start)) {
            fpsHidden = !fpsHidden;
            [NSUserDefaults.standardUserDefaults setBool:!fpsHidden forKey:@"P2ShowFPS"];
            fps = 0; fpsAt = 0;
            [self setNeedsRender];
            continue;
        }
        Button *b; [self layout:&b];
        int button = [self buttonAt:f.start];
        BOOL left = f.start.x < CGRectGetMidX(self.bounds);
        if (button >= 0) {
            f.role = ROLE_BUTTON; f.action = b[button].action;
            f.toggle = (b[button].flags & RT_TOUCH_TOGGLE) != 0; f.aim = (b[button].flags & RT_TOUCH_AIM) != 0;
            if (!f.toggle) [self action:f.action down:YES];
            else if (toggled[f.action]) [self untoggle:f.action];
            else { toggled[f.action] = YES; [self action:f.action down:YES]; }
        }
        else if (left && ![self roleTaken:ROLE_MOVE]) f.role = ROLE_MOVE;
        else if (!left && ![self roleTaken:ROLE_LOOK]) f.role = ROLE_LOOK;
        else continue;   // second doigt sur un stick déjà tenu : ignoré
        [fingers setObject:f forKey:t];
    }
    [self updateMove]; [self setNeedsRender];
}
- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    (void)event;
    BOOL changed = NO;   // la surcouche ne dessine que les sticks d'après la position des doigts
    for (UITouch *t in touches) {
        P2Finger *f = [fingers objectForKey:t];
        CGPoint p = [t locationInView:self];
        // Stick gauche, ou droit hors mode trackpad : son bouton suit le doigt.
        // Un doigt en trackpad ou sur un bouton ne change rien à l'écran.
        if (f && (f.role == ROLE_MOVE || (f.role == ROLE_LOOK && !trackpad))) changed = YES;
        // Tir de HL2 : le doigt qui tire vise aussi, comme sur le trackpad.
        if (f && (f.role == ROLE_LOOK ? trackpad : f.role == ROLE_BUTTON && f.aim)) {
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
    [self updateMove];
    if (changed) [self setNeedsRender];
}
- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    (void)event;
    backtap_touches(touches);
    for (UITouch *t in touches) {
        P2Finger *f = [fingers objectForKey:t];
        if (f.role == ROLE_BUTTON && !f.toggle) [self action:f.action down:NO];
        // Sprint en bascule : il s'arrête quand le pouce quitte le stick.
        if (f && f.role == ROLE_MOVE) [self untoggle:RT_TOUCH_SPRINT];
        [fingers removeObjectForKey:t];
    }
    [self updateMove]; [self setNeedsRender];
}
- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    cancelling = YES;
    [self touchesEnded:touches withEvent:event];
    cancelling = NO;
}
- (void)safeAreaInsetsDidChange { [super safeAreaInsetsDidChange]; [self invalidateLayout]; }

// ---- dessin ----
// Compteur d'images : la mesure de runtime/fps_overlay.c (une image par
// SDL_GL_SwapWindow ou Present, sur la dernière demi-seconde), relevée deux
// fois par seconde (redessiner l'incrustation coûte : seulement quand la valeur
// change). P2_SHOW_FPS=0 (Réglages) le masque.
- (void)updateFPS:(CFTimeInterval)now {
    if (fpsHidden || now - fpsAt < .5) return;
    fpsAt = now;
    int value = (int)lround(fps_overlay_value());
    long taps, fixes, reported;
    rt_ios_backtap_counts(&taps, &fixes, &reported);
    BOOL lit = now - flagAt < 1;
    BOOL data = taps != dataTaps || fixes != dataFixes || reported != dataReported || lit != flagLit;
    dataTaps = taps; dataFixes = fixes; dataReported = reported; flagLit = lit;
    if (value != fps || data) { fps = value; [self setNeedsRender]; }
}
// Police pixel 5×7 du compteur F3 (runtime/fps_overlay.c), kPixel points par
// pixel de glyphe, dessinée sans lissage.
static const CGFloat kPixel = 2;   // points par pixel de la police (6 pixels d'écran sur un iPhone ×3)
static void fill_text(CGContextRef c, const char *text, CGPoint at) {
    FpsRect rects[FPS_OVERLAY_MAX_RECTS];
    int n = fps_overlay_text(text, 0, 0, 1, rects, FPS_OVERLAY_MAX_RECTS);
    for (int i = 0; i < n; ++i)
        CGContextFillRect(c, CGRectMake(at.x + rects[i].x * kPixel, at.y + rects[i].y * kPixel, rects[i].w * kPixel, rects[i].h * kPixel));
}
// En haut à gauche, dans la zone sûre (hors de l'arrondi et de l'encoche) :
// rendu sur la première ligne, cadence sur la seconde, et sur la troisième le
// jeu de données des tapes au dos : « DATA tapes/corrections Xsignalées »
// (tapes : exemples étiquetés tapes ; corrections : erreurs devinées ou
// signalées ; X : signalées par le bouton ✗, comptées 4,8 s après le candidat).
- (CGRect)fpsBox:(NSString *)renderer fps:(NSString *)text data:(NSString *)data {
    CGRect a = self.area;
    CGFloat margin = 2 * kPixel;
    NSUInteger chars = MAX(MAX(renderer.length, text.length), data.length);
    return CGRectMake(round(CGRectGetMinX(a) + 8), round(CGRectGetMinY(a) + 8),
                      (chars * 6 - 1) * kPixel + 2 * margin, (data.length ? 25 : 16) * kPixel + 2 * margin);
}
- (NSString *)dataText {
    return dataTaps < 0 ? @"" : [NSString stringWithFormat:@"DATA %ld/%ld X%ld", dataTaps, dataFixes, dataReported];
}
// Zone qui bascule le compteur : le coin jusqu'au compteur le plus large, et
// un peu plus (le doigt couvre mal un si petit carré).
- (CGRect)fpsCorner {
    CGRect box = [self fpsBox:@"OPENGL" fps:@"000 FPS" data:dataTaps < 0 ? @"" : @"DATA 0000/000 X00"];
    return CGRectMake(0, 0, CGRectGetMaxX(box) + 20, CGRectGetMaxY(box) + 20);
}
// Compteur dans le contexte c, aux coordonnées de la vue (cadre box).
- (void)drawFPSIn:(CGContextRef)c box:(CGRect)box renderer:(NSString *)renderer fps:(NSString *)text data:(NSString *)data {
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
    fill_text(c, renderer.UTF8String, CGPointMake(at.x + kPixel / 3, at.y + kPixel / 3));
    fill_text(c, text.UTF8String, CGPointMake(below.x + kPixel / 3, below.y + kPixel / 3));
    CGContextSetFillColorWithColor(c, UIColor.whiteColor.CGColor);
    fill_text(c, renderer.UTF8String, at);
    CGPoint third = CGPointMake(at.x, below.y + 9 * kPixel);
    if (data.length) {
        CGContextSetFillColorWithColor(c, UIColor.blackColor.CGColor);
        fill_text(c, data.UTF8String, CGPointMake(third.x + kPixel / 3, third.y + kPixel / 3));
        CGContextSetFillColorWithColor(c, (flagLit ? [UIColor colorWithRed:1 green:.85 blue:.2 alpha:1]   // appui sur ✗ reçu
                                                   : [UIColor colorWithRed:.55 green:.8 blue:1 alpha:1]).CGColor);
        fill_text(c, data.UTF8String, third);
    }
    CGContextSetFillColorWithColor(c, ink.CGColor);
    fill_text(c, text.UTF8String, below);
    CGContextRestoreGState(c);
}
// Calques, créés une fois (initWithWindow:) : ordre de l'ancien dessin,
// compteur, sticks, puis boutons par-dessus.
static CAShapeLayer *shape_layer(CALayer *parent, UIColor *fill, UIColor *stroke) {
    CAShapeLayer *l = [CAShapeLayer layer];
    l.fillColor = fill.CGColor; l.strokeColor = stroke.CGColor; l.lineWidth = 2;
    l.contentsScale = UIScreen.mainScreen.scale;
    [parent addSublayer:l];
    return l;
}
// Cercle de ce diamètre centré sur la position du calque.
static void set_circle(CAShapeLayer *l, CGFloat diameter) {
    CGPathRef path = CGPathCreateWithEllipseInRect(CGRectMake(-diameter / 2, -diameter / 2, diameter, diameter), NULL);
    l.path = path;
    CGPathRelease(path);
}
- (void)setupLayers {
    palette_init();
    [CATransaction begin]; [CATransaction setDisableActions:YES];
    fpsLayer = [CALayer layer];
    fpsLayer.contentsScale = UIScreen.mainScreen.scale; fpsLayer.hidden = YES;
    [self.layer addSublayer:fpsLayer];
    for (int i = 0; i < 2; ++i) {
        stickRing[i] = shape_layer(self.layer, nil, stick_ink);
        stickKnob[i] = shape_layer(self.layer, stick_ink, nil);
        set_circle(stickKnob[i], kButtonSize);
        stickRing[i].hidden = stickKnob[i].hidden = YES;
    }
    for (int i = 0; i < MAX_BUTTONS; ++i) {
        buttonLayer[i] = shape_layer(self.layer, nil, line_ink);
        buttonLayer[i].hidden = YES;
        iconLayer[i] = [CALayer layer];
        iconLayer[i].anchorPoint = CGPointZero; iconLayer[i].hidden = YES;
        [buttonLayer[i] addSublayer:iconLayer[i]];
    }
    [CATransaction commit];
}
- (void)setNeedsRender { needsRender = YES; }
- (void)renderIfNeeded {
    if (!needsRender) return;
    needsRender = NO;
    [CATransaction begin]; [CATransaction setDisableActions:YES];
    [self renderFPS];
    BOOL shown = !concealed && !menu && !rt_ios_touch_test_active();   // essai : écran dégagé
    for (int right = 0; right < 2; ++right) {
        BOOL hidden = !shown || (right && trackpad);
        stickRing[right].hidden = stickKnob[right].hidden = hidden;
        if (hidden) continue;
        CGFloat r = self.stickRadius;
        CGPoint origin = [self restingStick:right], knob = origin;
        for (P2Finger *f in fingers.objectEnumerator)
            if (f.role == (right ? ROLE_LOOK : ROLE_MOVE)) { origin = f.start; knob = f.at; }
        CGFloat dx = knob.x - origin.x, dy = knob.y - origin.y, dist = hypot(dx, dy);
        if (dist > r) knob = CGPointMake(origin.x + dx * r / dist, origin.y + dy * r / dist);
        if (r != ringRadius[right]) { ringRadius[right] = r; set_circle(stickRing[right], 2 * r); }
        stickRing[right].position = origin; stickKnob[right].position = knob;
    }
    Button *b = NULL;
    int n = shown ? [self layout:&b] : 0;
    CGFloat scale = UIScreen.mainScreen.scale;
    for (int i = 0; i < MAX_BUTTONS; ++i) {
        CAShapeLayer *l = buttonLayer[i];
        l.hidden = i >= n;
        if (i >= n) continue;
        CGFloat s = b[i].size;
        if (s != buttonSize[i]) { buttonSize[i] = s; set_circle(l, s); }
        l.position = b[i].at;
        l.fillColor = (held[b[i].action] ? [b[i].color colorWithAlphaComponent:.9] : b[i].color).CGColor;
        CALayer *icon = iconLayer[i];
        icon.hidden = !b[i].symbol;
        if (!b[i].symbol) continue;
        if (![b[i].symbol isEqualToString:iconSymbol[i]] || iconSize[i] != s) {
            UIImage *image = icon_image(b[i].symbol, s * .38);
            iconSymbol[i] = b[i].symbol; iconSize[i] = s;
            icon.contents = (__bridge id)image.CGImage; icon.contentsScale = image.scale;
            icon.bounds = CGRectMake(0, 0, image.size.width, image.size.height);
        }
        // Centrée sur le bouton, coin calé sur un pixel de l'écran (pas de flou).
        CGSize z = icon.bounds.size;
        CGPoint corner = CGPointMake(round((b[i].at.x - z.width / 2) * scale) / scale,
                                     round((b[i].at.y - z.height / 2) * scale) / scale);
        icon.position = CGPointMake(corner.x - b[i].at.x, corner.y - b[i].at.y);
    }
    [CATransaction commit];
    [CATransaction flush];
}
// Bitmap du compteur, refait seulement quand ce qu'il montre change (FpsKey,
// comparée avant de construire textes et cadre).
- (void)renderFPS {
    fpsLayer.hidden = concealed || fps <= 0;   // le compteur reste dans les menus
    if (fpsLayer.hidden) return;
    FpsKey key;
    memset(&key, 0, sizeof(key));   // rembourrage à zéro : comparée par memcmp
    key.taps = dataTaps; key.fixes = dataFixes; key.reported = dataReported; key.area = self.area;
    key.fps = fps; key.metal = rt_ios_metal_active != 0; key.lit = flagLit;
    if (!memcmp(&key, &fpsKey, sizeof(key))) return;
    fpsKey = key;
    NSString *text = [NSString stringWithFormat:@"%d FPS", fps];
    NSString *renderer = key.metal ? @"METAL" : @"OPENGL";   // runtime/ios/metal_ios.m
    NSString *data = [self dataText];
    CGRect box = [self fpsBox:renderer fps:text data:data];
    UIImage *image = render_bitmap(box.size, ^(CGContextRef c) {
        CGContextTranslateCTM(c, -box.origin.x, -box.origin.y);
        [self drawFPSIn:c box:box renderer:renderer fps:text data:data];
    });
    fpsLayer.contents = (__bridge id)image.CGImage;
    fpsLayer.frame = box;
}
@end

static P2TouchControls *controls;

// Appelé à la création de la fenêtre du jeu (sdl_ios.m), sur le thread principal.
void rt_ios_touch_controls_attach(SDL_Window *window) {
    if (controls || !window || !rt_ios_env_flag("P2_TOUCH_CONTROLS", 1)) return;
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
