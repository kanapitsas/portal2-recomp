# Project rules

- Commit messages in English from 2026-10-29 on (the repo is public). Older
  commits stay in French. Code comments may stay in French; documentation is
  in English.
- No Steam emulator and no Valve code in the repository: the game is built on
  the user's machine from their own copy, Steam goes through Valve's own
  `libsteam_api.dylib` (downloaded by `setup.sh`, never committed), and faking
  ownership or interfaces is out of scope.

## Tests

- `make check`: Python tests and `build/abi-smoke`, no game files needed (what
  CI runs, plus `build/abi-smoke-ubsan`).
- `make test`: the full run, needs the Linux depot (`INPUT_DIR`, derived from
  `linux_root` in `config/local.json` when present, else
  `../work/portal2-linux/bin`). `make sanitize` is the UBSan variant.
- `P2_STEAM_TESTS=1 make test`: live Steam tests, with the client running.
- `make clean` also wipes `build/game/prep` (and `build/hl2`); regenerate it
  with `python3 tools/gameprep.py [--game hl2]` (a few seconds).
- Both games share one checkout: Portal 2 in `build/game` and
  `build/portal2-native`, Half-Life 2 in `build/hl2`; paths in
  `tools/games.py`, config merge in `tools/localconfig.py` (`game_config`).
  Tools take `--game`; never hard-code a game path.

## Conventions for native bridges

- **New imports**: one `runtime/<domain>.c` file per domain. It declares its
  names on `// SHIMS: name1 name2 ...` lines (read by `tools/lift.py`) and
  registers itself with `RT_SHIM_PROVIDER(function)`; the function returns 0
  for an unknown name. Do not extend the `_CORE_SHIMS` list in `lift.py` or
  the `rt_shim` chain in `shims.c` any further.
- **Dynamic addresses** (`SDL_GL_GetProcAddress`, `alGetProcAddress`, `dlsym`
  into a host library): `rt_dynamic_import(name)` returns a stable guest
  address (range `0xf8000000`); a guest call to it ends up in
  `rt_shim(c, name)` just like a static import.
- **Host pointers**: `rt_handle_new/get/find/free` (unmapped range
  `0xe0000000`, typed by an identifier). A structure that guest code reads
  directly (SDL_Surface, FT_Face, SDL_Event...) must be copied into guest
  memory in i386 layout.
- **Floating-point return**: `rt_return_st0(c, value)`. **Guest callback**
  from the host: `rt_invoke` (`modules.h`).
- **Guest callback from a foreign host thread** (SDL audio thread, CoreAudio):
  `CPU *c = rt_foreign_thread_attach()` (idempotent: its own protected stack,
  TCB, TLS, errno), then `rt_guest_enter(); rt_invoke(c, …); rt_guest_leave();`.
  Detach explicitly or automatically on thread exit. Any host call that may
  wait on this thread (SDL device lock, join) must be made with the guest lock
  released (`rt_guest_suspend/resume`), otherwise it deadlocks.
- **Tests**: `tests/checks_<domain>.c` with `RT_CHECK(name) { ... }`, linked
  automatically into `abi-smoke` (and its UBSan variant).
- **Host libraries**: `HOST_CFLAGS`/`HOST_LIBS` in the Makefile, mirrored in
  `tools/localconfig.py` (`host_flags`, used by `tools/build_game.py`): SDL2,
  FreeType, Fontconfig, iconv, OpenGL, OpenAL, under `host_prefix` (Homebrew
  or the prebuilt bundle of `tools/build_host_deps.sh`). A new host library
  goes in both and in the bundle.
