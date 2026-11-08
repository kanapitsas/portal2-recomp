#!/usr/bin/env python3
"""Generate runtime/ios/steam_stub_generated.c: a host definition of every flat
libsteam_api function the bridge (runtime/steam_generated.c) can call, for the
Steam stub of the iOS build (runtime/ios/steam_stub.c).

By default a function returns 0, false, NULL or ""; RESPONSES gives the answers
of a logged-in, offline user who owns the game of the build (SteamAppId, set by
ios/main_ios.c from the game profile: 620 for Portal 2, 220 for Half-Life 2). A function returning a
pointer from an interface version returns the stub's interface object.
Functions already hand-written in steam_stub.c are skipped.

Interfaces of the newer SDKs (Half-Life 2: SteamUser023, SteamClient021,
STEAMUGC_INTERFACE_VERSION020...) are called by vtable slot, not through flat
functions: for those the stub gets a vtable per version whose slots hold
functions with the same answers (steam_stub_vtable).

    python3 tools/gen_steam_stub.py [output.c]
"""
import argparse
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
APP_ID = 'steam_stub_app_id()'   # runtime/ios/steam_stub.c
STEAM_ID = 76561197960265729   # compte individuel public valide, univers 1

# Réponses explicites (corps de fonction C), par nom de fonction plate ;
# a0 est l'objet d'interface (self), a1 le premier argument de la méthode.
RESPONSES = {
    'SteamAPI_ISteamUser_BLoggedOn': 'return true;',
    'SteamAPI_ISteamUser_GetSteamID': f'return {STEAM_ID}ull;',
    'SteamAPI_ISteamUser_GetHSteamUser': 'return 1;',
    'SteamAPI_ISteamApps_BIsSubscribed': 'return true;',
    'SteamAPI_ISteamApps_BIsSubscribedApp': f'return a1 == {APP_ID};',
    'SteamAPI_ISteamApps_BIsAppInstalled': f'return a1 == {APP_ID};',
    'SteamAPI_ISteamApps_BIsDlcInstalled': 'return false;',
    'SteamAPI_ISteamApps_GetCurrentGameLanguage': 'return "english";',
    'SteamAPI_ISteamApps_GetAvailableGameLanguages': 'return "english";',
    'SteamAPI_ISteamApps_GetAppOwner': f'return {STEAM_ID}ull;',
    'SteamAPI_ISteamUtils_GetAppID': f'return {APP_ID};',
    'SteamAPI_ISteamUtils_GetIPCountry': 'return "FR";',
    'SteamAPI_ISteamUtils_GetServerRealTime': 'return (uint32_t)time(NULL);',
    'SteamAPI_ISteamUtils_GetCurrentBatteryPower': 'return 255;',
    'SteamAPI_ISteamUtils_GetConnectedUniverse': 'return 1;',
    'SteamAPI_ISteamUtils_GetSteamUILanguage': 'return "english";',
    'SteamAPI_ISteamFriends_GetPersonaName': 'return "Player";',
    'SteamAPI_ISteamFriends_GetPersonaState': 'return 1;',
    'SteamAPI_ISteamFriends_GetFriendPersonaName': 'return "Player";',
    # Steam Input « prêt » sans manette : sinon HL2 rappelle SteamAPI_InitEx et
    # recharge son manifeste d'actions en boucle pendant la partie.
    'SteamAPI_ISteamInput_Init': 'return true;',
    # Connexion au serveur local (aussi en solo) : ticket factice non vide.
    'SteamAPI_ISteamUser_InitiateGameConnection_DEPRECATED': 'return steam_stub_ticket(a1, a2);',
    'SteamAPI_ISteamUser_GetAuthSessionTicket':
        'int32_t n = steam_stub_ticket(a1, a2); if (a3) *(uint32_t *)a3 = (uint32_t)n; return n > 0 ? 1u : 0u;',
    'SteamAPI_ISteamGameServer_GetAuthSessionTicket':
        'int32_t n = steam_stub_ticket(a1, a2); if (a3) *(uint32_t *)a3 = (uint32_t)n; return n > 0 ? 1u : 0u;',
    'SteamAPI_ISteamGameServer_SendUserConnectAndAuthenticate_DEPRECATED':
        f'if (a4) *(uint64_t *)a4 = {STEAM_ID}ull; return true;',
}


def signatures():
    text = (ROOT / 'runtime/steam_generated.c').read_text()
    found = re.findall(r'typedef ([^;]*?) \(\*F\)\(([^)]*)\);\s*static void \*cache;.*?steam_flat\(c, "([A-Za-z0-9_]+)"\)',
                       text, re.S)
    out = {}
    for ret, args, name in found:
        out.setdefault(name, (ret.strip(), [a.strip() for a in args.split(',') if a.strip()]))
    return out


def slot_methods():
    """{version: (class, {slot: (method, return type, parameter types)})} of the
    methods runtime/steam_generated.c calls by vtable slot."""
    text = (ROOT / 'runtime/steam_generated.c').read_text()
    classes = dict(re.findall(r'\{"([A-Za-z0-9_]+)", "(I[A-Za-z0-9]+)", \d+u\}', text))
    found = re.findall(r'/\* ([A-Za-z0-9_]+)::([A-Za-z0-9_]+) \*/\s*static void m\d+\([^)]*\) \{[^}]*?'
                       r'typedef ([^;]*?) \(\*F\)\(([^)]*)\);\s*void \*fn = steam_slot\(c, self, (\d+)u\);', text, re.S)
    out = {}
    for version, method, ret, args, slot in found:
        if version not in classes:
            raise SystemExit(f'{version}: interface class unknown')
        out.setdefault(version, (classes[version], {}))[1][int(slot)] = (method, ret.strip(), [a.strip() for a in args.split(',') if a.strip()])
    return out


def body_for(name, ret, args):
    """Answer of a stub function: RESPONSES, the stub interface for a
    GetISteam* accessor, else the default for the return type."""
    body = RESPONSES.get(name)
    if body is None and ret == 'void *' and args and args[-1] == 'const char *' and 'GetISteam' in name:
        body = f'return steam_stub_interface(a{len(args) - 1});'
    return default(ret) if body is None else body


def function(ret, name, args, body):
    params = ', '.join(f'{t}{"" if t.endswith("*") else " "}a{i}' for i, t in enumerate(args)) or 'void'
    unused = ' '.join(f'(void)a{i};' for i in range(len(args)))
    return f'{ret}{"" if ret.endswith("*") else " "}{name}({params}) {{ {unused} {body} }}'


def handwritten():
    text = (ROOT / 'runtime/ios/steam_stub.c').read_text()
    return set(re.findall(r'^EXPORT [^(]*?\b([A-Za-z0-9_]+)\(', text, re.M))


def default(ret):
    if ret == 'void':
        return ''
    if ret == 'const char *':
        return 'return "";'
    if ret.endswith('*'):
        return 'return NULL;'
    if ret == 'bool':
        return 'return false;'
    return 'return 0;'


def generate():
    """(source text, generated function count, hand-written function count)."""
    skip = handwritten()
    lines = ['/* Généré par tools/gen_steam_stub.py depuis runtime/steam_generated.c. Ne pas modifier. */',
             '/* Stub Steam du build iOS seulement : voir runtime/ios/steam_stub.c. */',
             '#include <TargetConditionals.h>', '#if TARGET_OS_IPHONE',
             '#include <stdbool.h>', '#include <stdint.h>', '#include <stddef.h>', '#include <string.h>', '#include <time.h>',
             '#define EXPORT __attribute__((visibility("default"), used))',
             'void *steam_stub_interface(const char *version);',
             'int32_t steam_stub_ticket(void *buffer, int32_t capacity);',
             'uint32_t steam_stub_app_id(void);']
    count = 0
    for name, (ret, args) in sorted(signatures().items()):
        if name in skip:
            continue
        lines.append('EXPORT ' + function(ret, name, args, body_for(name, ret, args)))
        count += 1
    # Tables des interfaces appelées par emplacement : même réponse que la
    # fonction plate de même nom ; emplacements sans méthode connue : 0.
    lines += ['', '/* Interfaces appelées par emplacement de vtable (SDK récents). */',
              'static unsigned long long zero_slot(void) { return 0; }']
    tables = []
    for version, (klass, methods) in sorted(slot_methods().items()):
        ident = re.sub(r'\W', '_', version)
        size = max(methods) + 1
        for slot, (method, ret, args) in sorted(methods.items()):
            flat = f'SteamAPI_{klass}_{method}'
            lines.append('static ' + function(ret, f'v_{ident}_{slot}', args, body_for(flat, ret, args)))
        entries = ', '.join(f'(void *)v_{ident}_{i}' if i in methods else '(void *)zero_slot' for i in range(size))
        lines.append(f'static void *vt_{ident}[{size}] = {{{entries}}};')
        tables.append((version, ident))
        count += len(methods)
    lines.append('void **steam_stub_vtable(const char *version) {')
    for version, ident in tables:
        lines.append(f'    if (!strcmp(version, "{version}")) return vt_{ident};')
    lines += ['    return NULL;', '}', '#endif', '']
    return '\n'.join(lines), count, len(skip)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('output', nargs='?', type=Path, default=ROOT / 'runtime/ios/steam_stub_generated.c')
    out = parser.parse_args().output
    text, count, skipped = generate()
    out.write_text(text)
    print(f'{count} flat functions generated ({skipped} hand-written) -> {out}')


if __name__ == '__main__':
    main()
