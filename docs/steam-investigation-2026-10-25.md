# Steam for recompiled Portal 2 — investigation (October 25, 2026)

Kept as written on 25 October, before the bridge existed; the bridge itself is
described in [steam.md](steam.md).

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

