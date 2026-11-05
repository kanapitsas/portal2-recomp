# Steam for recompiled Portal 2 — investigation (October 25, 2026)

> **Note (5 November 2026).** Sections 1 to 6 are the investigation of
> 25 October, kept as written: section 2 ("Our runtime today") and the
> estimates of section 5 describe the state before the bridge existed. The
> bridge of section 7 is in production: it is what the Mac app uses to talk to
> Steam. `setup.sh` downloads Valve's `libsteam_api.dylib` inside the
> Steamworks.NET 20.1.0 package (SHA-256 checked, unpacked into
> `deps/steam_api.bundle`, path recorded as `steam_api_dylib` in
> `config/local.json`); nothing from Valve is committed.

**Conclusion.** The game does not start without Steam: it has to talk to the real client.
This can be done cleanly with the **official `libsteam_api.dylib` (Valve, ARM64)**
already available locally. It exposes exactly the interface versions that
Portal 2 asks for. A native `SteamAPI_Init` with `SteamAppId=620` **succeeds** against the
running Mac client, and the client confirms ownership
(`BIsSubscribedApp(620)=1`). All that remains is to write an i386 → ARM64 bridge.

## 1. Steam initialization in the Linux build

- Modules linked against `libsteam_api.so` (`NEEDED`): `engine`, `inputsystem`, `localize`,
  `launcher`, `dedicated`, `client`, `matchmaking`, `server`. Together they import
  **25 functions**: `SteamAPI_InitSafe`, `SteamAPI_RunCallbacks`,
  `SteamAPI_(Un)RegisterCallback/CallResult`, `SteamAPI_GetHSteamUser/Pipe`,
  `SteamInternal_ContextInit`, `SteamInternal_FindOrCreateUserInterface`,
  `SteamInternal_CreateInterface`, `SteamAPI_RestartAppIfNecessary` (in `server`
  only), `SteamGameServer_*`, breakpad… `launcher.so` imports nothing from Steam.
- This is the modern `SteamInternal_ContextInit` accessor API, SDK version
  **1.53a**. `engine.so` (function `0x3c6bb0`, called from `0x3c5e30`, which
  calls `SteamAPI_InitSafe` without checking the result) fills a context of
  23 interfaces. It returns `false` as soon as a single one is missing: `SteamClient020`,
  `SteamUser021`, `SteamFriends017`, `SteamUtils010`, `SteamMatchMaking009`,
  `SteamMatchGameSearch001`, `SteamMatchMakingServers002`, `USERSTATS012`, `APPS008`,
  `SteamNetworking006`, `REMOTESTORAGE016`, `SCREENSHOTS003`, `HTTP003`,
  `SteamController008`, `UGC016`, `APPLIST001`, `MUSIC001`, `MUSICREMOTE001`,
  `HTMLSURFACE_005`, `INVENTORY_V003`, `VIDEO_V002`, `PARENTALSETTINGS001`,
  `SteamInput006`. `client`/`server` also request `SteamGameServer014`,
  `SteamGameServerStats001`, `SteamGameCoordinator001` and `SteamGameStats001`.
- **Failure = the game exits.** In `SV_InitGameDLL` (`engine.so` `0x55cc4c`), if the
  context's `+0x1c` field (`STEAMAPPS_INTERFACE_VERSION008`) is null, the game calls
  `Error("Steam is not running. You must start Steam in order to play this game.")`.
  `server.so` (`0x8048b2`) calls `SteamAPI_RestartAppIfNecessary(GetAppID())` and
  fails `DLLInit` if the call returns true. There is no `-nosteam` option;
  `-insecure` only affects VAC. **So there is no degraded single-player mode.**
- Guest `libsteam_api.so`: `getenv("HOME")`, `"%s/.steam/%s"` (the string
  `sdk`/`32` is built on the stack, hence `~/.steam/sdk32/steamclient.so`), then
  falls back to a local `steamclient.so`. It also reads `steam.pid`, `steam_appid.txt` and
  `SteamAppId`/`SteamGameId`, and uses `dlopen`/`dlmopen`/`kill`.

## 2. Our runtime today

`rt_loader_shim` (`runtime/modules.c`) only resolves `dlopen` to recompiled guest
modules, by base name. `steamclient.so` is not one of them:
`dlopen` returns `NULL` ("guest module unavailable"). `kill` and `dlmopen` are
not shimmed yet. The Steam path has never been executed: startup stops
earlier, at `SDL_SetHint`.

## 3. On the native Mac side: what exists

| File | Architectures | Findings |
|---|---|---|
| `Steam.AppBundle/.../steamclient.dylib` | x86_64 + **arm64**, signed by Valve | 38 C exports (`CreateInterface`, `Steam_BGetCallback`…). Still serves all the old versions (`SteamClient006…023`, `SteamUser004…023`) and **27/27** of the strings requested by the game. |
| `work/downloads/steamworks/steam_api.bundle/.../libsteam_api.dylib` (Steamworks.NET 20.1.0 = SDK **1.53a**) | i386 + x86_64 + **arm64**, signed by Valve | Exports **25/25** of the functions imported by the game, plus the C "flat" API (`SteamAPI_ISteamUser_GetSteamID`…) and `SteamAPI_ManualDispatch_*`. Its `SteamAPI_SteamXxx_vNNN` accessors match the game's versions **exactly**. |

No Mac Portal 2 is installed (`steamapps/common/Portal 2` only contains a save).

## 4. Native test (`experiments/steam/steam_init_probe.c`)

The Mac Steam client **was already running** (`steam_osx`, `ipcserver`); I did not start it.

```
sans SteamAppId : Loaded '.../steamclient.dylib' OK. → "SteamAPI_Init() failed; no appID found." → 0
SteamAppId=620 : SteamAPI_Init=1, ISteamUser021/Apps008/Utils010 ok,
                 BLoggedOn=1, GetAppID=620, BIsSubscribedApp(620)=1, langue=english
```

(`sans SteamAppId` = without `SteamAppId`; `langue` = language.)

The probe prints neither the SteamID nor the persona name. Visible side effect: for about 1 s, the
profile may have shown "in game: Portal 2". **Not tested**: client stopped or offline
mode. The strings `create pipe failed` and `connect to global user failed`
show that init requires a running, logged-in client.

## 5. Options and recommendation

**A — recommended: replace the guest `libsteam_api.so` with a host module**
`runtime/steam.c` that loads the official dylib. It consists of:
1. The 25 imports, forwarded as is. Exception: `UseBreakpad*`/`WriteMiniDump`
   are no-ops, logged, so that breakpad does not install its
   signal handlers on top of ours.
2. Guest proxy objects: one vtable per interface, made of
   `rt_dynamic_import("steam:ISteamUser021:N")`. The host maps each slot N to the
   corresponding flat function. The table comes from the declaration order of SDK
   1.53a (Steamworks.NET's autogenerated files, MIT, are in the same package).
   Slots implemented on demand; an unknown slot triggers an `rt_trap`.
3. Callbacks through `SteamAPI_ManualDispatch_*`: host-side registry of guest
   `CCallbackBase` objects, copy of `pubParam` into guest memory, then
   `rt_invoke` of `Run`. Alignment is 4 bytes on Linux i386 as on
   macOS, so structures without pointers can be copied byte for byte.

ABI pitfalls: on i386, `CSteamID` is returned through a hidden pointer (`ret 4`) and passed
on the stack (8 bytes); `bool` is 8 bits; returned `const char*` values
must be copied into a guest cache.

Scope: the accessors found statically yield at least 22 distinct (interface, slot)
pairs. Calls made through the `client`/`server` context
escape this heuristic; expect **80 to 150 methods** actually
used, out of about 550 in these interfaces. **Estimated effort**: 4 to 7 days
for startup and single-player (init, user/apps/utils/friends, `UserStats` achievements,
`RemoteStorage` for cloud saves, callbacks). Add 3 to 5 days for co-op, the
workshop and the server browser: these functions pass guest C++ response
objects to the client, which requires reverse proxies. Risks: wrongly typed
signatures (plan per-slot tests), callbacks containing pointers, and
re-entrancy during `RunCallbacks` under the guest execution lock.

**B — keep the guest `libsteam_api.so` and have it load a "steamclient" bridge.**
This needs the same vtable marshalling, but toward the native ARM64 C++ objects
(no flat API). On top of that come the Linux specifics to emulate (`~/.steam/sdk32`,
`steam.pid` + `kill`, `dlmopen`, `/proc/self/cmdline`) and the low-level
`Steam_BGetCallback` protocol. Longer (+30 to 50%) and more fragile, with no benefit.

**Ruled out**: returning fake interfaces or forcing `BIsSubscribed`. That would be a
Steam emulator, which the project's rules forbid.

## 6. Decisions (approved by the user)

Option A; official dylib loaded by path, never committed; tests with
`SteamAppId=620` accepted; Steam client started and logged in by the user;
single-player first, co-op/workshop/server browser later.

## 7. Implementation (branch `feat/steam-bridge`, merged)

**Files.** `runtime/steam.c` (imports, proxies, callbacks), `runtime/steam.h`,
`runtime/steam_generated.c` (generated by `tools/gen_steam.py`, do not edit),
`tests/checks_steam.c`, `tools/steam_callsites.py` (static measurement).

**Dylib path**: `P2_STEAM_API_DYLIB` variable, otherwise the `steam_api_dylib` key in
`config/local.json` (read relative to the executable: `<exe>/../config/local.json`), otherwise
`../work/downloads/steamworks/steam_api.bundle/Contents/MacOS/libsteam_api.dylib`.
Dylib not found on the first Steam call: explicit `rt_trap` (no fake `false`).
`SteamAPI_InitSafe` sets `SteamAppId`/`SteamGameId` from the `steam_appid` key in
`config/local.json` if they are not in the environment.

**Import resolution** (`tools/elfprep.py`, 5 lines). The guest `libsteam_api.so` **cannot**
be removed: it bundles libsupc++ (`__cxa_guard_*`, `__cxa_throw`,
`__gxx_personality_v0`, `typeinfo` vtables), which almost every module links against. Only its
`SteamAPI_*`, `SteamInternal_*`, `SteamGameServer_*` exports become host imports. Without
that, `libsteam_api_linux.so` (an old SDK that nothing loads) would capture these names. Its
internal references (`SteamAPI_Init`, `SteamAPI_IsSteamRunning`…) also become imports:
without a shim, they trap if its code ever reaches them. Module addresses are unchanged.

**Imports (25).** Forwarded to the dylib, except:
- `SteamAPI_RunCallbacks` / `SteamGameServer_RunCallbacks`: pump `SteamAPI_ManualDispatch_*`
  (enabled after a successful `InitSafe`/`GameServer_Init`), without re-entrancy;
- `SteamAPI_(Un)RegisterCallback/CallResult`: host registry of guest `CCallbackBase` objects;
  `RegisterCallback` sets `k_ECallbackFlagsRegistered` and `m_iCallback` the way the dylib does;
- `SteamInternal_ContextInit`: one host double per guest block; when the real dylib calls
  its `pFn` (init counter changed), we call the guest `pFn` on the guest context;
- breakpad (`UseBreakpadCrashHandler`, `SetMiniDumpComment`, `WriteMiniDump`): no-ops,
  logged once, so that breakpad's signal handlers are not installed.

**Interfaces.** Guest proxy = `[guest vtable][host handle]`; each slot is
`rt_dynamic_import("SteamUser021::GetSteamID")`. Slots: ARM64 disassembly of the dylib's flat
functions (`ldr x8,[x0]; ldr xN,[x8,#off]; br xN` → off/8, same Itanium ABI as i386
GCC); N also gives the number of integer arguments, checked against the signature. Signatures:
Steamworks.NET 20.1.0's `NativeMethods.cs` (MIT) + the return type of the C# wrappers
(CSteamID/CGameID by value → hidden pointer and `ret 4`, strings → interned guest copy,
`GetISteam*` → proxy). `ISteamController` (absent from Steamworks.NET) reuses the
signatures of the same-named `ISteamInput` methods (`Legacy_*`, `Init`/`RunFrame` with no arguments),
checked against the argument count. Buffers: a pointer followed by a size (`cub*`, `cch*`…) or arrays of
documented size (`STEAM_INPUT_MAX_*`). **721/772 methods** of the 25 interfaces are translated;
the rest, and the slots with no flat function (`DEPRECATED` methods), trap with their name,
slot and reason. `SteamGameCoordinator001` and `SteamGameStats001` (not in the SDK) have no
flat functions: `virtual_bridge` in `steam.c` calls the host object's virtual methods directly
(ARM64 C++ ABI) for `SendMessage`/`IsMessageAvailable`/`RetrieveMessage` and for the 13
`ISteamGameStats` methods; the same path serves `ISteamUtils::GetCSERIPPort` (slot 7, no flat
function). `SetWarningMessageHook` (`ISteamClient`, `ISteamUtils`) is bridged: the host hook
copies the message into guest memory, masks SteamIDs and calls the guest function.

Notable trapping methods: `ISteamMatchmakingServers` (16/17, guest C++ response objects —
server browser), part
of `ISteamUGC`/`ISteamInventory`/`ISteamHTMLSurface` (string arrays, structures),
`ISteamNetworking::GetP2PSessionState`, `ISteamUser::GetAvailableVoice`.

**Callbacks.** `GetNextCallback` → copy of `pubParam` into guest memory → `Run(param)`
(slot 0) or, for `SteamAPICallCompleted_t`, `GetAPICallResult` then
`Run(param, bIOFailure, hCall)` (slot 1). Before each call, the guest object's `GetCallbackSizeBytes`
(slot 2) must equal the host size, otherwise `rt_trap`: this guards against
a diverging layout (pointers). Slot order verified statically: 32 vtables
`[code, code, mov eax,imm; ret]` in `engine.so`.

**Privacy.** `steamclient.dylib` writes the SteamID64 to stderr during init: stderr
is captured during `InitSafe`/`GameServer_Init` and any SteamID64 is masked (`XXXX…`).

**Measurement.** `tools/steam_callsites.py` (lower bound: contexts stored as object members escape
the heuristic) finds 13 (interface, method) pairs — all translated. Dynamic measurement
waits until the game gets past the SDL stop.

**Tests** (`make test`, `make sanitize`): generated table, proxies, callback
registration, masking, and full i386 marshalling through the dylib's real flat functions
to test host objects (CSteamID `ret 4`, `uint64` in edx:eax, strings, out parameters,
`float`, returned interface). `P2_STEAM_TESTS=1 make test` (client running, otherwise "skip"):
real `SteamAPI_InitSafe` with `SteamAppId=620`, `BLoggedOn`, `GetSteamID` (validated without
printing it), `GetAppID`=620, `BIsSubscribedApp(620)`, `UserStatsReceived_t` callback (20 bytes)
and `GlobalAchievementPercentagesReady_t` call result, then `SteamAPI_Shutdown`.

**Remaining** (as written on 25 October; single-player has run since, and `SetWarningMessageHook`
is bridged). Single-player: measure dynamically as soon as the game gets past SDL; translate the methods that
trap at that point. Blocking Steam calls (IPC) keep holding the guest execution lock.
Co-op/workshop/servers: reverse proxies for `ISteamMatchmaking*Response`, UGC string
arrays. LAN co-op works without them ([usage.md](usage.md#lan-co-op)); Steam invites and
lobbies are not tested.
