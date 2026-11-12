# iOS port

Portal 2 runs on an iPhone 15 Pro Max (iOS 27): menus, gameplay, level
transitions, voice lines, on-screen touch controls. Half-Life 2 has had a
first short test on the same phone, at 60 fps; see [Half-Life 2](#half-life-2).

## Metal renderer (default)

Instead of gl4es, the app can render through the native Direct3D 9 path
described in [d3d9-bridge.md](d3d9-bridge.md): `shaderapivk.so` → bridge →
libd3d9metal (mtld3d) → Metal. It is the default when the app is built with the
library and the GPU supports BC textures; otherwise the app uses gl4es. It
is about 20–25 % faster than OpenGL on an iPhone 15 Pro Max
([measurements](../STATUS.md#measurements)) and fixes the too-dark image of
the gl4es path.

`./setup.sh --ios` builds and links the library when Rust from rustup is
installed (Homebrew's `rust` has no iOS target; the app then uses OpenGL). By hand:

```sh
# in the mtld3d checkout (branch feat/native-host), once per mtld3d change:
make native-ios PROFILE=production        # native/target/aarch64-apple-ios/production/libd3d9metal.a
# here: gameprep must have kept shaderapivk.so
python3 tools/build_ios.py --target device --release \
    --d3d9metal ../mtld3d/native/target/aarch64-apple-ios/production/libd3d9metal.a
tools/ios_device.sh install
tools/ios_device.sh prep                  # the app's module layout includes shaderapivk.so now
```

`"d3d9metal_ios": {"device": "/path/to/libd3d9metal.a"}` in
`config/local.json` replaces `--d3d9metal`. The library is linked
statically (iOS does not `dlopen` a dylib outside the signed app):
`runtime/d3d9.c` is compiled with `RT_D3D9METAL_STATIC`. Without it the app is
OpenGL only and the setting below falls back to OpenGL.

On the phone: Settings > Portal 2 (or Half-Life 2) > Renderer chooses Metal (default) or OpenGL;
`P2_RENDERER=opengl` or `=d3d9` in `tools/ios_device.sh run` overrides it. `Documents/game.log` (with the
Log setting) then shows `[ios] renderer: Metal (...)`,
`d3d9: libd3d9metal (linked in) ready` and `[ios] Metal view: ...`.

- Device check first (`runtime/ios/metal_ios.m`): Source's compressed
  textures are DXT, and mtld3d creates BC textures with no CPU fallback
  (gl4es decompressed them). A GPU whose `supportsBCTextureCompression` is
  false stays on OpenGL, with a log line.
- Resolution: the Metal view takes the app's render scale (Resolution
  setting), like the GL view, so the drawable is the game's back buffer
  size and mtld3d does not resample.
- Frame rate: `present.maxFps` follows `P2_IOS_MAX_FPS` (60 by default) and
  auto power saving, see "Frame rate cap"; the 120 Hz display request is the
  same as on OpenGL. The FPS counter and
  back taps count frames on `Present` (`rt_d3d9_on_present`).
- Upscaling (MetalFX): Settings > Upscaling puts `render.scale=0.75` or
  `0.5` in `MTLD3D_CONFIG` (`P2_IOS_UPSCALE`; a `render.scale` you set in
  `MTLD3D_CONFIG` yourself comes later and wins). mtld3d then rasterizes the
  scene at that fraction of the game's back buffer and enlarges it with the
  MetalFX spatial scaler, with a texture LOD bias so textures keep their
  detail (`render.lodBias`, on by default). It multiplies on top of
  Resolution: the back buffer is the screen in points × the Resolution scale,
  so ×2 with Quality renders 1398×645 on an iPhone 15 Pro Max, upscaled to
  1864×860, which the display then stretches to 2796×1290; Native with
  Quality renders 2097×968 (about the pixel count of ×2.25) and MetalFX
  produces the full native image. The interface is scaled with the world.
  Ignored with OpenGL (`[ios] MetalFX : agrandissement … ignoré`); the log
  says `[ios] MetalFX : rendu à 75 % du tampon arrière` when it is active.
  Not measured yet on the phone.
- Shader/pipeline cache: `Library/Caches/d3d9metal` (`P2_D3D9_CACHE` to
  move it). Memory caps: the same `MTLD3D_CONFIG` defaults as on macOS.
- Simulator: not available. mtld3d links MetalFX, which the iOS Simulator
  SDK does not have.
- `make d3d9metal-check D3D9METAL_A=…/libd3d9metal.a` drives the library
  alone, linked statically as in the app (macOS build of the library); the
  same source links against the iOS library.

## OpenGL renderer (fallback)

togl (D3D9 → OpenGL 2.1) → `runtime/gl.c` → `runtime/ios/gl_ios.m` → gl4es
(static, `ios/deps/gl4es-ios.patch`) → OpenGL ES 3 (OpenGLES.framework).

- `runtime/ios/essl3.c` rewrites every GLSL ES 1.00 shader emitted by gl4es as
  ES 3.00: sRGB emulation (decode per texture unit, encode per output),
  texture borders, and BGRA color swizzling for the `vN` attributes.
- `runtime/ios/srgb.c` tracks sRGB/border state between togl and gl4es and sets
  the uniforms before each draw.
- `gl_ios.m`, at the gl4es → ES boundary: ES 3 sized formats, 16-bit textures
  as half floats, unsynchronized `glBufferSubData` writes
  (`glMapBufferRange` + `GL_MAP_UNSYNCHRONIZED_BIT`).

## Building and running

What `./setup.sh --ios` runs, step by step (`python3 tools/gameprep.py`
first if it was not done for macOS):

```sh
P2_IOS_DEPS=deps/ios-deps ios/deps/build.sh device      # SDL2, FreeType, gl4es (once, or after changing the patch)
python3 tools/build_ios.py --target device --release --deps deps/ios-deps/device   # for playing: -O2 without memory checks
tools/ios_device.sh install                       # installs on the connected iPhone (unlocked, Developer Mode on)
tools/ios_device.sh data                          # copies the game data (~11.5 GB, once)
tools/ios_device.sh run [seconds] [VAR=value…]    # launches with P2_FRAME_LOG=1, waits (180 s by default), pulls back Documents/game.log into build/ios/device/
```

`ios/deps/build.sh` writes to `P2_IOS_DEPS` (default `../work/ios-deps`) and
`build_ios.py --deps` reads the matching `<prefix>/device`; `setup.sh` uses
`deps/ios-deps` (`P2_DEPS` moves it). `P2_DEVICE=<UDID or name>` picks the
iPhone for `tools/ios_device.sh` when several are connected.

Simulator: `ios/deps/build.sh sim`, `python3 tools/build_ios.py`, then
`tools/run_ios_sim.sh <seconds>` (`P2_SIM_DEVICE=<UDID>` for a different
simulator; `P2_SIM_AUDIO=1` to hear the sound on the Mac's speakers, muted
otherwise). The simulator renders in software and interpolates triangles
clipped at the near plane incorrectly (`ios/probe/clip_test.c`): its
artifacts do not exist on a real device. The Metal renderer is not available
in the simulator.

Signing (free account): 7-day profiles obtained through
`xcodebuild -allowProvisioningUpdates` on `ios/signing/project.yml`, then
`ios/signing/sign.py` (called by `build_ios.py`). Launching fails if the
iPhone is locked.

The app identifier must belong to your Apple account: set `ios_bundle_id` in
`config/local.json` (or `P2_BUNDLE_ID`, which applies to both games, so
`setup.sh --ios` refuses it when it builds both), read by every tool through
`tools/ios_bundle_id.py`; the probe uses the same identifier plus `.probe`.
First profile, once per identifier (then again every 7 days):

```sh
cd ios/signing
export P2_BUNDLE_ID=$(python3 ../../tools/ios_bundle_id.py)   # --game hl2 for Half-Life 2
xcodegen
xcodebuild -project P2Signing.xcodeproj -scheme Portal2 -destination generic/platform=iOS \
  -allowProvisioningUpdates DEVELOPMENT_TEAM=YOURTEAMID build
```

Replace `YOURTEAMID` with your team ID (Xcode > Settings > Accounts, or the
`OU` field of your Apple Development certificate). `setup.sh` takes the team
of the first Apple Development certificate in the keychain: with several Apple
accounts in Xcode, set `P2_TEAM_ID` to the one you want.

Saves: `tools/ios_device.sh saves status|pull|push` copies them between the Mac
and the iPhone over the cable ([details](usage.md#saves-between-mac-and-iphone-portal-2));
Portal 2 only for now.

`Documents/launch.args` in the app's container adds arguments
(one per line), for example `+map` followed by `sp_a2_laser_intro`.

The game language is written into the app's `launch.args` when it is built
(`-language <name>`, chosen as on the Mac: see [Game language](usage.md#game-language)).
To change it, set `"language"` in `config/local.json` and rebuild.

## Touch controls (Portal 2)

Hidden in menus and when a controller connects (`runtime/ios/touch_layout.c`).

| Where | Button |
|---|---|
| left half | move stick |
| right half | camera stick or trackpad |
| right edge | jump (middle), use (bottom), crouch (left of them) |
| top left and right | blue and orange portals, only when back taps are off |
| top right, small | pause; ✗ to report a wrong back tap while recording taps |
| bottom left, small | camera mode (stick or trackpad) |

Portals are fired by tapping the back of the phone (see [Back taps](#back-taps)).

## Settings (iOS Settings app)

The game's page in the Settings app ("Portal 2" or "Half-Life 2"; `ios/Settings.bundle`, copied into the app
by `tools/build_ios.py`). At launch, `runtime/ios/settings.c` turns each
changed setting into a `P2_*` variable; a variable that is already set (devicectl,
simctl) takes precedence, and a setting that was never touched leaves the code's
default in place. Changes take effect on the next launch.

The page is in English, and in French on a device set to French
(`ios/Settings.bundle/fr.lproj/Root.strings`).

| Setting | Variable | Values |
|---|---|---|
| Resolution | `P2_IOS_RENDER_SCALE` (+ the game's `-w`/`-h`) | native, ×2 (default), ×1.5, ×1 |
| Frames per second | `P2_IOS_MAX_FPS` | 30, 40, 60 (default), 120; see "Frame rate cap" |
| Auto power saving | `P2_IOS_POWER_SAVE` | on by default: at most 30 fps in Low Power Mode or when the phone is hot |
| Renderer | `P2_RENDERER` | Metal: `d3d9` (default), `opengl`; see "Metal renderer" |
| Upscaling (MetalFX) | `P2_IOS_UPSCALE` | Off (1, default), Quality (0.75), Performance (0.5); Metal only, see "Metal renderer" |
| FPS counter | `P2_SHOW_FPS` | shown by default; tapping the top-left corner shows or hides it (choice is remembered) |
| On-screen controls | `P2_TOUCH_CONTROLS` | automatic (hidden when a controller is connected), always, never |
| Trackpad camera | key `P2TouchTrackpad` | also toggled by the button at the bottom left (top left in Half-Life 2) |
| Camera speed | `P2_TOUCH_LOOK` | ×0.5 to ×3 |
| Trackpad acceleration | `P2_TOUCH_ACCEL` | none (1) to strong (4), normal 2.5 |
| Gyro aiming | `P2_GYRO` | on by default in Half-Life 2, off in Portal 2; see "Gyro aiming" |
| Gyro speed | `P2_GYRO_SPEED` | ×0.5 to ×2, normal 1 |
| Back taps | `P2_BACKTAP` | enabled by default |
| Vibrate when firing | `P2_HAPTICS` | on by default: a light Taptic Engine tap for each portal fired from the overlay (button or back tap) |
| Tap sensitivity | `P2_BACKTAP_SENSITIVITY` | 0.75 / 1 / 1.33: thresholds are divided by this value |
| Swap left and right | `P2_BACKTAP_INVERT` | |
| Portal buttons | `P2_PORTAL_BUTTONS` | automatic (when back taps are off), always, never |
| Record taps | `P2_BACKTAP_RAW=Documents/backtap/` | off; see [backtap.md](backtap.md) |
| Log | `P2_LOG_FILE=Documents/game.log` | |

### Frame rate cap

`P2_IOS_MAX_FPS` (Frames per second) is the ceiling, read at launch by
`runtime/ios/power_ios.m`:

- Metal: `present.maxFps=<cap>` in `MTLD3D_CONFIG`; mtld3d holds presents to it.
- OpenGL: 60 and 120 are paced by the display; under 60 the runtime waits
  before each present (`gl_ios.m`, `limit_frame_rate`).
- Display: 120 asks the ProMotion panel for 120 Hz, as before; so does 40,
  which then lands on every third refresh (on a 60 Hz panel 40 fps would
  alternate one and two refreshes; on a phone without ProMotion choose 30).

Auto power saving (on by default, `P2_IOS_POWER_SAVE=0` turns it off) lowers
the cap to 30 when Low Power Mode is on or the thermal state is serious or
critical, and logs `[ios] économie d'énergie (<reason>) : 30 i/s au lieu de
<cap>`. mtld3d reads its configuration once, at initialization, and has no way
to change `present.maxFps` afterwards: with Metal the check happens at launch
only (a later change is logged as applying at the next launch); with OpenGL
the cap follows thermal and power changes during play.

Simulator: `xcrun simctl spawn <device> defaults write <bundle id>
P2ResolutionScale -string 1.5`. `tests/checks_ios_settings.c` checks the
translation and that the keys match between the code and the settings page.

## Gyro aiming

Turning the phone turns the camera, added to the touch camera: the thumb for
large turns, the wrists for aiming (`runtime/ios/gyro_aim.c`, tested by
`tests/checks_gyro_aim.c`; CoreMotion in `touch_controls.m`). Yaw follows the
real vertical, so turning on the spot turns the view however the phone is
tilted; pitch follows the screen's horizontal axis (top of the screen toward
you: look up). Rotations slower than `P2_GYRO_TIGHTEN` (0.05 rad/s) are damped
in proportion, so a phone held still does not drift. At Normal speed the camera
turns 1.5 times the phone's rotation at the game's default mouse sensitivity
(3); the game's `sensitivity` scales it like the mouse. The sensors stop after
2 s in a menu or with the overlay hidden for a controller, and at once when
the app is left.

On by default in Half-Life 2. Off in Portal 2, where each back tap would shake
the camera; `P2_GYRO=1` or the setting turns it on there too.

## Leaving the app

When the app stops being active (app switcher, home gesture, Control Center
or Notification Center pulled down, incoming call, screen locked), the game
pauses at its next frame (`runtime/ios/lifecycle_ios.m`): the main thread,
which is the game's, stops in `SDL_PollEvent`, finishes pending OpenGL ES
work (`glFinish`; iOS kills an app that uses the GPU through OpenGL ES in the
background, and refuses Metal submissions), suspends and detaches the OpenAL
context, then waits, serving UIKit's run loop, until the app is active again
(`UIApplicationDidBecomeActive`). Other game threads keep running (the guest
lock is released while it waits). The touch overlay treats the pause like a
menu (held fingers released, no input sent) and the back-tap sensors stop at
once. A frame presented by another thread while in the background is
dropped without any OpenGL call. Log lines: `[ios] app inactive : jeu en
pause`, `[ios] app en arrière-plan`, `[ios] app active : reprise après N s`.

| Variable | Default | Purpose |
|---|---|---|
| `P2_IOS_BACKGROUND_PAUSE` | 1 | `0`: no pause at all (the old behavior) |
| `P2_IOS_BACKGROUND_AUDIO` | 1 | `0`: the pause leaves the OpenAL context alone |

To check on the phone (with the Log setting on): in game, open the app
switcher and come back; go to the home screen, wait 30 s, reopen; pull down
Control Center over the game, then dismiss it; lock the screen and unlock.
Each time the game must come back where it was (no restart: a restart means
iOS killed it), with sound, and `game.log` must show the pause and resume
lines. Also try during a level load, and with the OpenGL renderer once.

## Half-Life 2

The same checkout builds Half-Life 2's iPhone app (see [hl2.md](hl2.md)):
`tools/build_ios.py --game hl2` reads the game profile (`tools/games.py`) for
the lift roots, the program (`hl2_linux`), the build paths (`build/hl2`, app
in `build/hl2/ios/<target>/HalfLife2.app`), the app name and icon, and writes
`P2Game` = `hl2` into the Info.plist. At launch
`runtime/ios/game_ios.c` reads that key (nothing is guessed): saves in
`Documents/hl2-userdata`, `SteamAppId` 220 for the Steam stub, `-game
hl2_complete`, back taps off, the HL2 touch layout.

```sh
./setup.sh --game hl2 --ios                # Mac build, then the iPhone app (--ios alone: every game)
# or by hand, as for Portal 2:
python3 tools/build_ios.py --game hl2 --target device --release --deps deps/ios-deps/device
tools/ios_device.sh --game hl2 install && tools/ios_device.sh --game hl2 data   # ~10 GB: bin platform hl2 episodic ep2 lostcoast hl2_complete
tools/run_ios_sim.sh --game hl2 120        # simulator, headless: build/hl2/ios/sim/game.log
```

The app identifier must differ from Portal 2's, or installing HL2 replaces
Portal 2 on the phone: `setup.sh` derives `local.hl2recomp.<team>` (Portal 2
keeps `local.portal2recomp.<team>`), stored per game: Portal 2's
`ios_bundle_id` at the top level of `config/local.json`, HL2's in
`games.hl2.ios_bundle_id` (`tools/ios_bundle_id.py --game hl2` prints it;
`P2_BUNDLE_ID` overrides both). It needs its own provisioning profile (first
`setup.sh --ios` run, or the `xcodebuild` command above with the HL2
identifier). With a free account the two apps take 2 of the 3 sideloaded-app
slots, and both need `./setup.sh --ios` again every 7 days. The name on the
home screen is "Half-Life 2".

Settings page: the same as Portal 2 without the Portals group.

### Touch controls

Same style and behaviour as Portal 2 (hidden in menus and with a controller,
move stick on the left half, camera stick or trackpad on the right half; the
camera stick's resting ring sits nearer the centre, clear of the fire cluster), with
HL2's commands (`runtime/ios/touch_layout.c`):

| Where | Button | Key | Notes |
|---|---|---|---|
| bottom right, largest | fire | left click | dragging the finger while firing also aims (trackpad speed) |
| left of fire | alt-fire | right click | aims while dragged too |
| above fire | jump, reload | Space, R | |
| below fire | crouch | Ctrl | toggle: tap to crouch, tap again to stand; jump while it is lit to crouch-jump |
| bottom, left of crouch | use | E | |
| left, above the stick | sprint | Shift | toggle; stops when the thumb leaves the move stick |
| top centre, small | previous / next weapon | mouse wheel | the game's weapon list opens; fire picks the weapon (lines `+hud_fastswitch` and `1` in `Documents/launch.args` switch at once) |
| top right, small | flashlight, quick save, pause | F, F6, Escape | hold quick save for half a second to quick load (F9), with a vibration |
| top left, after the FPS counter, small | camera mode | | stick or trackpad, as in Portal 2 (moved off the HUD's health) |

Gyro aiming is on: turning the phone turns the camera, on top of the stick or
trackpad (see [Gyro aiming](#gyro-aiming)).

Why toggles: on a phone each thumb is already busy (one on the move stick, one
on the camera or fire), so a held Shift or Ctrl would stop you from steering or
aiming; crawling through vents or sprinting down a corridor needs both.

Back taps are off: they produce a 50 ms click, while HL2's automatic weapons
fire as long as the button is held, and an accidental shot wastes ammo or
alerts enemies. `P2_BACKTAP=1` turns them back on (left tap: fire, right tap:
alt-fire); the Settings page has no back-tap entries for HL2.

### Status

Checked on an iPhone 15 Pro Max (8 Nov. 2026, short session): install and
signing with the HL2 identifier, data copy (~10 GB, about 5 minutes over the
cable), Metal, 60 fps, moving, firing, flashlight. Earlier, the iOS Simulator
(software rendering, 2–6 fps) checked the menu, the stick, aiming, the weapon
list and quick save.

Not verified yet: memory and thermal behaviour, a long session, sound,
`setup.sh --ios` end to end. The ammo readout at the bottom right sits partly
under the crouch and use buttons.

HL2-specific iOS pieces: the Steam stub answers HL2's SDK 1.60 start-up
(`SteamInternal_SteamAPI_Init`) and the interfaces it calls by vtable slot
(`tools/gen_steam_stub.py` generates a table per version); the minimal
fontconfig accepts the game's own fonts (`FcConfigAppFontAddFile`); HL2
always gets `-w`/`-h` at the render size (it would start at 640×480).
Steam Input reloads its action manifest every few seconds in game (one small
file read; harmless, not fixed).

## Portal

The same checkout also builds Portal's iPhone app (see
[portal1.md](portal1.md)): `tools/build_ios.py --game portal1` reads the game
profile (`tools/games.py`), writes `P2Game` = `portal1` into the Info.plist
and builds `build/portal1/ios/<target>/Portal.app`. At launch
`runtime/ios/game_ios.c` reads that key: saves in `Documents/portal-userdata`,
`SteamAppId` 400 for the Steam stub, `-game portal`, back taps on (they fire
the portal) and **Portal 2's touch layout**, since it already has both portal
buttons.

```sh
./setup.sh --game portal1 --ios            # Mac build, then the iPhone app
# or by hand, as for Portal 2:
python3 tools/build_ios.py --game portal1 --target device --release --deps deps/ios-deps/device
tools/ios_device.sh --game portal1 install && tools/ios_device.sh --game portal1 data   # ~4 GB: platform portal hl2
tools/run_ios_sim.sh --game portal1 120    # simulator, headless
```

The identifier is `local.portal1recomp.<team>`, so it sits next to Portal 2's
and Half-Life 2's apps (`games.portal1.ios_bundle_id` in
`config/local.json`; `tools/ios_bundle_id.py --game portal1` prints it). The
name on the home screen is "Portal". The Settings page is Portal 2's: the
Portals group (back taps, portal buttons, vibration) and gyro aiming off by
default, as for Portal 2. `tools/ios_device.sh saves` is still Portal 2 only,
so Portal's saves are copied by hand for now.

Status (12 November 2026): installs, starts and renders on an iPhone 17 Pro
Max (short session, menu and touch layout; no chamber played). The devices
were locked for the last run of this checkout, so the app was not exercised
again. Same experimental status as the Mac app
([portal1.md](portal1.md#what-was-tested)).

## Diagnostics

iOS-specific variables; `P2_FRAME_LOG`, `P2_GATE_TIMING`, `P2_TRACE_*`,
`P2_NATIVE_<NAME>=0`, `P2_GL_STREAM_UNSYNC=0` and the other runtime variables
work as on the Mac ([usage.md](usage.md#diagnostics)). The `P2_IOS_GL_TRACE`,
`P2_IOS_DEBUG_UV`, `P2_IOS_SHADER_DUMP`, `P2_IOS_SRGB_TRACE` and
`P2_IOS_SKIP_DRAWS` family needs a debug build (`P2_IOS_GL_DEBUG=1` when
building with `tools/build_ios.py`). `P2_TOUCH_TEST_*` is for the simulator
(no finger on the screen).

| Variable | Effect |
|---|---|
| `P2_LOG_FILE=Documents/game.log` | log file (relative to the container) |
| `P2_PROFILE_DELAY=60`, `P2_PROFILE_RAW=1` | built-in profiler (with `P2_FRAME_LOG=1`); with `RAW`, `tools/ios_symbolize.py` resolves driver functions using the device's symbols (Xcode, iOS DeviceSupport) |
| `P2_TOUCH_CONTROLS=0/1` | touch controls disabled / always shown |
| + `_LOW=300`, `_HIGH=1500`, `_SMOOTH=0.5`, `_LOG=1` | trackpad acceleration: gain of 1 below `_LOW` pt/s, ramping up to the `P2_TOUCH_ACCEL` multiplier at `_HIGH` pt/s; speed is smoothed (weight `_SMOOTH`); `=1` disables it, `_LOG` logs speed and gain |
| `P2_BACKTAP=0` | disables back taps (enabled by default if the sensor exists; see "Back taps") |
| `P2_BACKTAP_LOG=1` | logs every candidate (from half the thresholds) and the background noise every 5 s |
| `P2_TOUCH_TEST_CLICK=s,x,y` | simulator: left click at (x, y) points, s seconds after the first menu |
| `P2_TOUCH_TEST_MOVE=f,s` + `_SECONDS`, `_DELAY`, `_ANGLE`, `P2_TOUCH_TEST_WASD` | simulator: forced deflections of the movement stick |
| `P2_IOS_RENDER_SCALE=1/2/native` | render scale (points → pixels); `tools/ios_bench.py` compares antialiasing × scale (`--renderer opengl` by default, `--renderer d3d9` for Metal) |
| `P2_IOS_NO_PRESENT=1` | frames rendered but never presented, so the display cap does not limit the measured rate (OpenGL only; Metal holds presents to `P2_IOS_MAX_FPS`) |
| `P2_IOS_AUTOEXEC="cmd; convar=value; pause 5; …"` | console commands, convar settings and pauses (seconds) run in order once the menu is gone, starting 5 s later |
| `P2_IOS_BGRA=0`, `P2_IOS_SRGB=0` | OpenGL: disable the BGRA fast path or the sRGB emulation |
| `P2_IOS_SHADER_DUMP=<directory>` | debug build: converted shaders |
| `P2_IOS_DRAW_LOG=<frame>` | draws for one frame |
| `P2_IOS_GL_TRACE=1` | debug build: logs each distinct texture upload combination once (function, target, level, formats, GL error) and traces shader compilation and program linking |
| `P2_IOS_PROBE_FRAMES=1` | every 120 frames, reads a few pixels of the system framebuffer before presenting (all black = nothing drawn) |
| `P2_IOS_DEBUG_UV=1` | debug build: every fragment shader that receives `oT0` shows `fract(oT0.xy)` instead (texture coordinate check) |
| `P2_IOS_SKIP_DRAWS=<n>` or `<a>-<b>` | debug build: skips draw n (or a up to b, excluded) of every frame, counted as in `P2_IOS_DRAW_LOG` |
| `P2_IOS_SKIP_PROGRAMS=45,802` | skips every draw made with these GL program ids |
| `P2_IOS_PROGRAM_DUMP=<directory>` | writes the shader sources of the programs described by `P2_IOS_DRAW_LOG` |
| `P2_IOS_SRGB_TRACE=1` | debug build: sRGB and border-state statistics every 2 000 draws |
| `P2_FONT_DIRS=<dir>:<dir>` | font directories indexed by the minimal fontconfig (default `/System/Library/Fonts`) |
| `P2_FONT_LOG=1` | logs every font match (by default only the first request per family) |

Game console: `-condebug` writes it to
`Documents/portal2-userdata/portal2/console.log` (Half-Life 2: under
`Documents/hl2-userdata/`).

## Back taps

Holding the phone in landscape, tapping the back near the left edge with an
index finger fires the blue portal, near the right edge the orange one
(`runtime/ios/back_tap.c`, tested by `tests/checks_back_tap.c`; CoreMotion in
`touch_controls.m`, on a dedicated queue, no permission required). Back taps
replace the overlay's portal buttons, which only come back without a motion
sensor (simulator) or with `P2_BACKTAP=0`.

Settings > Portal 2 has the back-tap entries (on/off, sensitivity, swap left
and right, record taps). How the detector works, its variables, raw
recordings and the tap dataset: [backtap.md](backtap.md).

## Performance

60 fps by default, steady on an iPhone 15 Pro Max; `P2_IOS_MAX_FPS=120`
requests 120 Hz (the frame rate then lands between 60 and 120, since the
ProMotion display shows a frame taking more than 8.3 ms at 60). The GPU is the
bottleneck, so the render scale is ×2 by default. `tools/ios_bench.py`
measures a scene (`--renderer d3d9` for Metal). Metal against OpenGL and the
OpenGL optimisation history: [STATUS.md](../STATUS.md#measurements).

## iOS-specific pitfalls

- The ES linker rejects a program whose fragment shader reads an output
  that the vertex shader does not write (desktop GL accepts it):
  the skipped draws broke the view through portals and froze the
  Options menu. `essl3.c` initializes these outputs.

- Case-sensitive file system: the engine writes and reads in
  lowercase (`portal2/save/…`); `runtime/paths.c` and
  `runtime/ios/case_path.c` recover the actual case.
- SDL never sees the game "capture" the mouse: the menu/gameplay state comes from the
  engine (`IEngineVGui::IsGameUIVisible`, `runtime/ios/game_state.c`).
- Touch controls: a UIKit view placed on top of the window, made of fixed
  layers (one `CAShapeLayer` per stick and button, a small bitmap for the FPS
  counter) moved without animation and flushed once per run-loop pass (SDL's
  loop never commits UIKit transactions).
