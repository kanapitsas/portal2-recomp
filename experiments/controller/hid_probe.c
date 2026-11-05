// Sonde HID pour manettes, indépendante de SDL et du jeu.
//
// Répond, sans qu'on touche la manette, à deux questions :
//  1. les rapports d'entrée arrivent-ils jusqu'à un processus ordinaire ?
//     (une manette Switch en mode complet 0x30 en envoie ~60 par seconde
//     même au repos, à cause de la centrale inertielle) ;
//  2. quelqu'un (Steam) a-t-il saisi le périphérique en exclusif ?
//     (IOHIDDeviceOpen renvoie alors kIOReturnExclusiveAccess).
//
// À lancer pendant que le jeu tourne au menu, puis jeu fermé, et comparer.
//   cc -O2 -framework IOKit -framework CoreFoundation hid_probe.c -o hid_probe
//   ./hid_probe [secondes] [--seize]
// --seize tente aussi une ouverture exclusive (rendue aussitôt) : elle échoue
// si un autre client tient la manette, mais réussie elle peut couper un
// instant les autres clients ; à éviter pendant une mesure comparative.
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDManager.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { IOHIDDeviceRef device; long reports; uint8_t first_id; uint8_t buffer[512]; } Probe;

static void on_report(void *context, IOReturn result, void *sender, IOHIDReportType type,
                      uint32_t id, uint8_t *report, CFIndex length) {
    (void)result; (void)sender; (void)type; (void)report; (void)length;
    Probe *p = context;
    if (!p->reports++) p->first_id = (uint8_t)id;
}

static long number(IOHIDDeviceRef d, CFStringRef key) {
    CFTypeRef v = IOHIDDeviceGetProperty(d, key);
    long n = 0;
    if (v && CFGetTypeID(v) == CFNumberGetTypeID()) CFNumberGetValue(v, kCFNumberLongType, &n);
    return n;
}

static void text(IOHIDDeviceRef d, CFStringRef key, char *out, size_t size) {
    CFTypeRef v = IOHIDDeviceGetProperty(d, key);
    out[0] = 0;
    if (v && CFGetTypeID(v) == CFStringGetTypeID()) CFStringGetCString(v, out, (CFIndex)size, kCFStringEncodingUTF8);
}

static const char *status(IOReturn r) {
    switch (r) {
    case kIOReturnSuccess: return "ok";
    case kIOReturnExclusiveAccess: return "exclusive access held by another client";
    case kIOReturnNotPermitted: return "not permitted";
    case kIOReturnNotPrivileged: return "not privileged";
    default: return "error";
    }
}

int main(int argc, char **argv) {
    double seconds = 3.0;
    int try_seize = 0;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--seize")) try_seize = 1;
        else seconds = atof(argv[i]);
    }
    IOHIDManagerRef manager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    // Manettes et joysticks (page Generic Desktop, usages 4 et 5), plus tout
    // périphérique Nintendo (vendeur 0x057e) quel que soit son usage déclaré.
    int page = 1, joystick = 4, gamepad = 5, nintendo = 0x057e;
    CFMutableArrayRef criteria = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
    int pairs[3][2] = {{page, joystick}, {page, gamepad}, {0, 0}};
    for (int i = 0; i < 3; ++i) {
        CFMutableDictionaryRef m = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks,
                                                             &kCFTypeDictionaryValueCallBacks);
        if (i < 2) {
            CFNumberRef a = CFNumberCreate(NULL, kCFNumberIntType, &pairs[i][0]);
            CFNumberRef b = CFNumberCreate(NULL, kCFNumberIntType, &pairs[i][1]);
            CFDictionarySetValue(m, CFSTR(kIOHIDDeviceUsagePageKey), a);
            CFDictionarySetValue(m, CFSTR(kIOHIDDeviceUsageKey), b);
            CFRelease(a); CFRelease(b);
        } else {
            CFNumberRef v = CFNumberCreate(NULL, kCFNumberIntType, &nintendo);
            CFDictionarySetValue(m, CFSTR(kIOHIDVendorIDKey), v);
            CFRelease(v);
        }
        CFArrayAppendValue(criteria, m);
        CFRelease(m);
    }
    IOHIDManagerSetDeviceMatchingMultiple(manager, criteria);
    CFRelease(criteria);
    CFSetRef set = IOHIDManagerCopyDevices(manager);
    CFIndex count = set ? CFSetGetCount(set) : 0;
    if (!count) { puts("no game controller found"); return 1; }
    IOHIDDeviceRef *devices = calloc((size_t)count, sizeof(*devices));
    CFSetGetValues(set, (const void **)devices);
    Probe *probes = calloc((size_t)count, sizeof(*probes));

    for (CFIndex i = 0; i < count; ++i) {
        IOHIDDeviceRef d = devices[i];
        char name[128], transport[32];
        text(d, CFSTR(kIOHIDProductKey), name, sizeof(name));
        text(d, CFSTR(kIOHIDTransportKey), transport, sizeof(transport));
        printf("[%ld] %s  vid=%04lx pid=%04lx transport=%s\n", (long)i, name,
               number(d, CFSTR(kIOHIDVendorIDKey)), number(d, CFSTR(kIOHIDProductIDKey)), transport);
        // Ouverture saisie (option) : refusée si un autre client tient déjà
        // l'exclusivité. Réussie, on la rend aussitôt.
        if (try_seize) {
            IOReturn seize = IOHIDDeviceOpen(d, kIOHIDOptionsTypeSeizeDevice);
            printf("    seize test: %s (0x%08x)\n", status(seize), seize);
            if (seize == kIOReturnSuccess) IOHIDDeviceClose(d, kIOHIDOptionsTypeSeizeDevice);
        }
        IOReturn shared = IOHIDDeviceOpen(d, kIOHIDOptionsTypeNone);
        printf("    shared open: %s (0x%08x)\n", status(shared), shared);
        if (shared != kIOReturnSuccess) continue;
        probes[i].device = d;
        IOHIDDeviceRegisterInputReportCallback(d, probes[i].buffer, sizeof(probes[i].buffer), on_report, &probes[i]);
        IOHIDDeviceScheduleWithRunLoop(d, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    }

    CFRunLoopRunInMode(kCFRunLoopDefaultMode, seconds, false);
    for (CFIndex i = 0; i < count; ++i) {
        if (!probes[i].device) continue;
        printf("[%ld] %ld input reports in %.1f s (%.0f/s), first report id 0x%02x\n", (long)i, probes[i].reports,
               seconds, probes[i].reports / seconds, probes[i].first_id);
        IOHIDDeviceClose(probes[i].device, kIOHIDOptionsTypeNone);
    }
    return 0;
}
