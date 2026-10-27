#!/usr/bin/env python3
"""Génère runtime/steam_generated.c : tables des interfaces Steam (SDK 1.53a) pour le pont invité.

Sources, toutes locales :
- emplacements de vtable : désassemblage ARM64 de la `libsteam_api.dylib` officielle. Chaque
  fonction « flat » `SteamAPI_ISteamX_Méthode` y est un appel terminal `ldr x8,[x0]; ldr xN,[x8,#off];
  br xN` : l'emplacement est off/8 (ABI Itanium, même ordre qu'en i386 GCC), et N, premier registre
  libre, donne le nombre d'arguments entiers (vérification croisée des signatures) ;
- signatures : `NativeMethods.cs` de Steamworks.NET 20.1.0 (MIT, même paquet), et le type de
  retour des enveloppes C# (CSteamID/CGameID rendus par valeur, chaînes, interfaces) ;
- ISteamController n'a pas d'équivalent C# : ses méthodes reprennent la signature de la
  méthode homonyme d'ISteamInput lorsque le nombre d'arguments du désassemblage concorde.

Une méthode dont le marshalling ne se déduit pas sûrement des types reste un piège nommé.
Le fichier généré ne contient que des noms, emplacements et conversions d'arguments.
"""
import argparse
import json
import re
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

# Versions demandées par Portal 2 (docs/steam.md), identiques aux accesseurs du SDK 1.53a.
VERSIONS = {
    'ISteamClient': 'SteamClient020', 'ISteamUser': 'SteamUser021', 'ISteamFriends': 'SteamFriends017',
    'ISteamUtils': 'SteamUtils010', 'ISteamMatchmaking': 'SteamMatchMaking009',
    'ISteamGameSearch': 'SteamMatchGameSearch001', 'ISteamMatchmakingServers': 'SteamMatchMakingServers002',
    'ISteamUserStats': 'STEAMUSERSTATS_INTERFACE_VERSION012', 'ISteamApps': 'STEAMAPPS_INTERFACE_VERSION008',
    'ISteamNetworking': 'SteamNetworking006', 'ISteamRemoteStorage': 'STEAMREMOTESTORAGE_INTERFACE_VERSION016',
    'ISteamScreenshots': 'STEAMSCREENSHOTS_INTERFACE_VERSION003', 'ISteamHTTP': 'STEAMHTTP_INTERFACE_VERSION003',
    'ISteamController': 'SteamController008', 'ISteamUGC': 'STEAMUGC_INTERFACE_VERSION016',
    'ISteamAppList': 'STEAMAPPLIST_INTERFACE_VERSION001', 'ISteamMusic': 'STEAMMUSIC_INTERFACE_VERSION001',
    'ISteamMusicRemote': 'STEAMMUSICREMOTE_INTERFACE_VERSION001',
    'ISteamHTMLSurface': 'STEAMHTMLSURFACE_INTERFACE_VERSION_005', 'ISteamInventory': 'STEAMINVENTORY_INTERFACE_V003',
    'ISteamVideo': 'STEAMVIDEO_INTERFACE_V002', 'ISteamParentalSettings': 'STEAMPARENTALSETTINGS_INTERFACE_VERSION001',
    'ISteamInput': 'SteamInput006', 'ISteamGameServer': 'SteamGameServer014',
    'ISteamGameServerStats': 'SteamGameServerStats001',
}
# Scalaires C# -> (type C, octets sur la pile i386).
SCALARS = {'int': ('int32_t', 4), 'uint': ('uint32_t', 4), 'short': ('int16_t', 4), 'ushort': ('uint16_t', 4),
           'byte': ('uint8_t', 4), 'sbyte': ('int8_t', 4), 'bool': ('bool', 4), 'long': ('int64_t', 8),
           'ulong': ('uint64_t', 8), 'float': ('float', 4), 'double': ('double', 8)}
# Structures sans pointeur, pack(4) sur Linux comme sur macOS (Packsize.cs) : même disposition.
PLAIN_STRUCTS = {'FriendGameInfo_t': 24, 'LeaderboardEntry_t': 28}
# Tableaux de taille fixe documentée (STEAM_INPUT_MAX_COUNT, _MAX_ACTIVE_LAYERS, _MAX_ORIGINS).
FIXED_ARRAYS = {'GetConnectedControllers': 16, 'GetActiveActionSetLayers': 16,
                'GetDigitalActionOrigins': 8, 'GetAnalogActionOrigins': 8}
# ISteamController -> méthode équivalente d'ISteamInput (Init/RunFrame n'y prennent aucun argument).
CONTROLLER_TWINS = {'TriggerHapticPulse': 'Legacy_TriggerHapticPulse',
                    'TriggerRepeatedHapticPulse': 'Legacy_TriggerRepeatedHapticPulse',
                    'GetControllerBindingRevision': 'GetDeviceBindingRevision',
                    'GetGlyphForActionOrigin': 'GetGlyphForActionOrigin_Legacy'}


# Tampons lus par Steam (méthode, paramètre) ; tous les autres sont écrits par Steam et
# vérifiés en écriture côté invité.
INPUT_BUFFERS = {
    ('FileWrite', 'pvData'), ('FileWriteAsync', 'pvData'), ('FileWriteStreamWriteChunk', 'pvData'),
    ('WriteScreenshot', 'pubRGB'), ('BeginAuthSession', 'pAuthTicket'), ('DecompressVoice', 'pCompressed'),
    ('RequestEncryptedAppTicket', 'pDataToInclude'), ('DownloadLeaderboardEntriesForUsers', 'prgUsers'),
    ('UploadLeaderboardScore', 'pScoreDetails'), ('DownloadClanActivityCounts', 'psteamIDClans'),
    ('HandleIncomingPacket', 'pData'), ('SendUserConnectAndAuthenticate_DEPRECATED', 'pvAuthBlob'),
    ('GetItemsByID', 'pInstanceIDs'), ('DeserializeResult', 'pBuffer'), ('SendLobbyChatMsg', 'pvMsgBody'),
    ('SetPNGIcon_64x64', 'pvBuffer'), ('UpdateCurrentEntryCoverArt', 'pvBuffer'), ('SendP2PPacket', 'pubData'),
    ('SendDataOnSocket', 'pubData'), ('SendMessageToUser', 'pubData'), ('SendMessageToConnection', 'pData'),
    ('ReceivedRelayAuthTicket', 'pvTicket'), ('ReceivedP2PCustomSignal', 'pMsg'), ('SetCertificate', 'pCertificate'),
    ('SendSignal', 'pMsg'), ('SendRejectionSignal', 'pMsg'),
}


def natural_size(scalar):
    if not scalar:
        return None
    return {'bool': 1, 'int8_t': 1, 'uint8_t': 1, 'int16_t': 2, 'uint16_t': 2}.get(scalar[0], scalar[1])


SIZE_NAME = re.compile(r'^(cub|cb|cch|un?Max|c[A-Z]|n\w*Size|\w*BufferSize|\w*Max\w*|\w*Count\w*|un\w*Size\w*)')


def sdk_files(pkg):
    files = {}
    for entry in Path(pkg).iterdir():
        name = entry / 'pathname'
        if name.is_file() and (entry / 'asset').is_file():
            files[name.read_text().splitlines()[0]] = entry / 'asset'
    return files


def typedefs(files):
    """Types enveloppes (AppId_t...) -> scalaire C# ; énumérations -> int."""
    result = {}
    for path, asset in files.items():
        if '/Runtime/types/' in path and path.endswith('.cs'):
            text = asset.read_text()
            struct = re.search(r'public struct (\w+)', text)
            fields = re.findall(r'public (\w+) m_\w+;', text)
            if struct and len(fields) == 1 and fields[0] in SCALARS:
                result[struct.group(1)] = fields[0]
    enums = files['Assets/com.rlabrecque.steamworks.net/Runtime/autogen/SteamEnums.cs'].read_text()
    for name, base in re.findall(r'public enum (\w+) : (\w+)', enums):
        result[name] = base
    return result


def disassemble(dylib):
    with tempfile.TemporaryDirectory() as tmp:
        thin = Path(tmp) / 'arm64.dylib'
        subprocess.run(['lipo', '-thin', 'arm64', str(dylib), '-output', str(thin)], check=True)
        text = subprocess.run(['objdump', '-d', str(thin)], check=True, capture_output=True, text=True).stdout
    bodies, current = {}, None
    for line in text.splitlines():
        m = re.match(r'^[0-9a-f]+ <_(\w+)>:', line)
        if m:
            current = m.group(1); bodies[current] = []; continue
        parts = line.split('\t')
        if current and len(parts) >= 3:
            bodies[current].append((parts[1].strip(), parts[2].strip()))
    slots = {}
    for name, body in bodies.items():
        if not name.startswith('SteamAPI_ISteam') or len(body) != 3:
            continue
        a, b, c = body
        m = re.fullmatch(r'x(\d+), \[x8(?:, #0x([0-9a-f]+))?\]', b[1])
        if a == ('ldr', 'x8, [x0]') and b[0] == 'ldr' and m and c == ('br', f'x{m.group(1)}'):
            slots[name] = (int(m.group(2) or '0', 16) // 8, int(m.group(1)))
    return slots, set(bodies)


def native_methods(files):
    text = files['Assets/com.rlabrecque.steamworks.net/Runtime/autogen/NativeMethods.cs'].read_text()
    result = {}
    for m in re.finditer(r'EntryPoint = "(SteamAPI_ISteam\w+)"[^\n]*\n((?:\s*\[return:[^\n]*\n)?)\s*public static extern (\S+) \w+\((.*)\);\s*$', text, re.M):
        params = []
        text_params = re.sub(r'\[MarshalAs\(UnmanagedType\.I1\)\]\s*', '', m.group(4)).replace('[In, Out] ', 'inout ')
        for raw in [p.strip() for p in text_params.split(',')][1:]:
            *kind, name = raw.split()
            params.append((' '.join(kind), name))
        result[m.group(1)] = (m.group(3), params)
    return result


def wrapper_returns(files):
    result = {}
    for path, asset in files.items():
        if '/Runtime/autogen/isteam' not in path:
            continue
        text = asset.read_text()
        for m in re.finditer(r'public static (\S+) \w+\([^)]*\) \{(.*?)\n\t\t\}', text, re.S):
            for call in re.findall(r'NativeMethods\.(ISteam\w+)\(', m.group(2)):
                result.setdefault('SteamAPI_' + call, m.group(1))
    return result



# Méthodes écrites à la main dans runtime/steam.c (rappels invités, etc.).
MANUAL = {
    'SteamAPI_ISteamClient_SetWarningMessageHook': 'steam_manual_client_warning_hook',
    'SteamAPI_ISteamUtils_SetWarningMessageHook': 'steam_manual_utils_warning_hook',
}

class Unsupported(Exception):
    pass


def plan_method(flat, method, ret, params, wrapper, types, gprs):
    """Renvoie (corps C, sret) ; lève Unsupported avec la raison."""
    def scalar(t):
        t = types.get(t, t)
        return SCALARS.get(t)
    count = 1 + sum(1 for kind, _ in params if not (scalar(kind) and scalar(kind)[0] in ('float', 'double')))
    if count != gprs:
        raise Unsupported(f'signature C# ({count} entiers) != dylib ({gprs})')
    lines, call, offset, post = [], ['self'], 0, []
    proto = ['void *']
    names = [n for _, n in params]
    for index, (kind, name) in enumerate(params):
        var = f'a{index}'
        at = f'args + {offset}u'
        if scalar(kind):
            ctype, size = scalar(kind)
            read = {'float': f'RF({at})', 'double': f'RD({at})', 'bool': f'(R8({at}) != 0)'}.get(
                ctype, f'({ctype})R64({at})' if size == 8 else f'({ctype})R32({at})')
            lines.append(f'{ctype} {var} = {read};')
            proto.append(ctype); offset += size
        elif kind in ('CSteamID', 'CGameID'):
            lines.append(f'uint64_t {var} = R64({at});'); proto.append('uint64_t'); offset += 8
        elif kind == 'InteropHelp.UTF8StringHandle':
            lines.append(f'const char *{var} = steam_in_string(c, R32({at}));'); proto.append('const char *'); offset += 4
        elif kind == 'out IntPtr' and name.startswith('ppch'):
            # char ** rempli par Steam (GetUGCDetails) : la chaîne hôte est recopiée, internée,
            # en mémoire invitée et l'invité reçoit son adresse.
            lines.append(f'void *{var}_slot = steam_out(c, R32({at}), 4u); const char *{var}_text = NULL;')
            lines.append(f'const char **{var} = {var}_slot ? &{var}_text : NULL;')
            post.append(f'if ({var}_slot) {{ uint32_t {var}_guest = steam_guest_string(c, {var}_text); '
                        f'memcpy({var}_slot, &{var}_guest, 4); }}')
            proto.append('const char **'); offset += 4
        elif kind.startswith(('out ', 'ref ')):
            inner = kind.split()[1]
            if scalar(inner):
                size = natural_size(scalar(inner))
            elif inner in ('CSteamID', 'CGameID'):
                size = 8
            elif inner in PLAIN_STRUCTS:
                size = PLAIN_STRUCTS[inner]
            else:
                raise Unsupported(f'paramètre {kind} {name}')
            lines.append(f'void *{var} = steam_out(c, R32({at}), {size}u);'); proto.append('void *'); offset += 4
        elif kind in ('byte[]', 'IntPtr') or (kind.startswith('inout ') and kind.endswith('[]')):
            following = params[index + 1] if index + 1 < len(params) else None
            esize = 1
            if kind.startswith('inout '):
                element = kind[len('inout '):-2]
                esize = 8 if element in ('CSteamID', 'CGameID') else natural_size(scalar(element))
                if not esize:
                    raise Unsupported(f'tableau {kind} {name}')
            helper = 'steam_in_buffer' if (method, name) in INPUT_BUFFERS else 'steam_out_buffer'
            if method in FIXED_ARRAYS and index == len(params) - 1 and kind.startswith('inout '):
                lines.append(f'void *{var} = {helper}(c, R32({at}), {FIXED_ARRAYS[method]}, {esize}u);')
            elif following and scalar(following[0]) and scalar(following[0])[1] == 4 and SIZE_NAME.match(following[1]):
                lines.append(f'void *{var} = {helper}(c, R32({at}), (int64_t)(int32_t)R32(args + {offset + 4}u), {esize}u);')
            else:
                raise Unsupported(f'tampon {kind} {name} sans taille')
            proto.append('void *'); offset += 4
        else:
            raise Unsupported(f'paramètre {kind} {name}')
        call.append(var)
    sret = False
    rtype = types.get(ret, ret)
    if ret == 'void':
        rproto, store = 'void', None
    elif wrapper in ('CSteamID', 'CGameID'):
        rproto, store, sret = 'uint64_t', 'steam_ret_sret(c, sret, r);', True
    elif rtype in SCALARS:
        ctype, size = SCALARS[rtype]
        rproto = ctype
        store = {'bool': 'c->eax = r ? 1u : 0u;', 'float': 'rt_return_st0(c, r);', 'double': 'rt_return_st0(c, r);'}.get(
            ctype, 'steam_ret64(c, (uint64_t)r);' if size == 8 else 'c->eax = (uint32_t)r;')
    elif ret == 'IntPtr' and wrapper == 'string':
        rproto, store = 'const char *', 'c->eax = steam_guest_string(c, r);'
    elif ret == 'IntPtr' and flat.startswith('SteamAPI_ISteamClient_GetISteam') and 'pchVersion' in names:
        rproto, store = 'void *', f'c->eax = steam_proxy(c, r, a{names.index("pchVersion")});'
    else:
        raise Unsupported(f'retour {ret} ({wrapper})')
    body = [f'typedef {rproto} (*F)({", ".join(proto)});', 'static void *cache;',
            'void *fn = __atomic_load_n(&cache, __ATOMIC_RELAXED);',
            f'if (!fn) __atomic_store_n(&cache, fn = steam_flat(c, "{flat}"), __ATOMIC_RELAXED);', *lines]
    body.append(f'{"" if rproto == "void" else rproto + " r = "}((F)fn)({", ".join(call)});')
    body.extend(post)
    if store:
        body.append(store)
    return body, sret


def generate(pkg, dylib):
    files = sdk_files(pkg)
    types = typedefs(files)
    slots, symbols = disassemble(dylib)
    natives = native_methods(files)
    wrappers = wrapper_returns(files)
    # ISteamController : signatures empruntées à ISteamInput (voir docstring).
    for flat in list(slots):
        if flat.startswith('SteamAPI_ISteamController_'):
            method = flat[len('SteamAPI_ISteamController_'):]
            twin = 'SteamAPI_ISteamInput_' + CONTROLLER_TWINS.get(method, method)
            if twin in natives:
                ret, params = natives[twin]
                natives[flat] = (ret, [] if method in ('Init', 'RunFrame') else params)
                wrappers.setdefault(flat, wrappers.get(twin))
    methods, interfaces, stats = [], [], {}
    for klass, version in VERSIONS.items():
        prefix = f'SteamAPI_{klass}_'
        own = sorted((s, f) for f, s in slots.items() if f.startswith(prefix))
        if not own:
            raise SystemExit(f'{klass}: aucune fonction flat trouvée')
        count = max(s[0] for s, _ in own) + 1
        interfaces.append((version, klass, count))
        seen = {}
        for (slot, gprs), flat in own:
            method = flat[len(prefix):]
            if slot in seen:
                raise SystemExit(f'{klass}: emplacement {slot} partagé par {seen[slot]} et {method}')
            seen[slot] = method
            entry = dict(name=f'{version}::{method}', iface=len(interfaces) - 1, slot=slot, flat=flat)
            if flat in MANUAL:
                entry['fn'] = MANUAL[flat]
                methods.append(entry)
                continue
            try:
                if flat not in natives:
                    raise Unsupported('signature absente de Steamworks.NET')
                ret, params = natives[flat]
                entry['body'], entry['sret'] = plan_method(flat, method, ret, params, wrappers.get(flat), types, gprs)
            except Unsupported as exc:
                entry['why'] = str(exc)
            methods.append(entry)
        stats[version] = (len(own), sum(('body' in m or 'fn' in m) for m in methods if m['iface'] == len(interfaces) - 1), count)
    methods.sort(key=lambda m: m['name'])
    out = ['/* Généré par tools/gen_steam.py depuis la libsteam_api.dylib officielle (SDK 1.53a) et',
           ' * Steamworks.NET 20.1.0 (MIT). Ne pas modifier : relancer le générateur. */',
           '#include "steam.h"', '']
    out.extend(f'void {fn}(CPU *c, void *self, uint32_t args, uint32_t sret);' for fn in sorted(set(MANUAL.values())))
    for index, m in enumerate(methods):
        if 'body' in m:
            m['fn'] = f'm{index}'
            out.append(f'/* {m["name"]} */')
            out.append(f'static void m{index}(CPU *c, void *self, uint32_t args, uint32_t sret) {{')
            out.append('    (void)args; (void)sret;')
            out.extend('    ' + line for line in m['body'])
            out.append('}')
    out.append('const SteamInterfaceInfo steam_interfaces[] = {')
    out.extend(f'    {{{json.dumps(v)}, {json.dumps(k)}, {n}u}},' for v, k, n in interfaces)
    out.append('};')
    out.append(f'const size_t steam_interface_count = {len(interfaces)};')
    out.append('const SteamMethodInfo steam_methods[] = {')
    for m in methods:
        why = json.dumps(m.get('why')) if 'why' in m else 'NULL'
        out.append(f'    {{{json.dumps(m["name"])}, {m["iface"]}u, {m["slot"]}u, {int(m.get("sret", False))}, '
                   f'{m.get("fn", "NULL")}, {why}}},')
    out.append('};')
    out.append(f'const size_t steam_method_count = {len(methods)};')
    return '\n'.join(out) + '\n', stats, methods


def main():
    config = json.loads((ROOT / 'config/local.json').read_text()) if (ROOT / 'config/local.json').exists() else {}
    default_dylib = config.get('steam_api_dylib', str(ROOT.parent / 'work/downloads/steamworks/steam_api.bundle/Contents/MacOS/libsteam_api.dylib'))
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--sdk', type=Path, default=ROOT.parent / 'work/downloads/steamworks/pkg')
    parser.add_argument('--dylib', type=Path, default=Path(default_dylib))
    parser.add_argument('--out', type=Path, default=ROOT / 'runtime/steam_generated.c')
    args = parser.parse_args()
    code, stats, methods = generate(args.sdk, args.dylib)
    args.out.write_text(code)
    total = sum(s[0] for s in stats.values()); done = sum(s[1] for s in stats.values())
    for version, (n, ok, slots) in stats.items():
        print(f'{version:45} {ok:3}/{n:3} méthodes, {slots} emplacements')
    print(f'total : {done}/{total} méthodes marshallées', file=sys.stderr)


if __name__ == '__main__':
    main()
