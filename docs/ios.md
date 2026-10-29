# iOS port

The game runs on an iPhone 15 Pro Max (iOS 27):
menus, gameplay, level transitions, voice lines, on-screen touch controls.

## Rendering pipeline

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

## Metal renderer (experimental)

Instead of gl4es, the app can render through the native Direct3D 9 path
described in [d3d9-bridge.md](d3d9-bridge.md): `shaderapivk.so` → bridge →
libd3d9metal (mtld3d) → Metal. OpenGL stays the default.

```sh
# in the mtld3d checkout (branch feat/native-host), once per mtld3d change:
make native-ios PROFILE=production        # native/target/aarch64-apple-ios/production/libd3d9metal.a
# here: gameprep must have kept shaderapivk.so (it does on this branch)
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

On the phone: Settings > Portal 2 > Rendu > Metal (expérimental), or
`tools/ios_device.sh run 120 P2_RENDERER=d3d9`. `Documents/game.log` (with the
Journal setting) then shows `[ios] renderer: Metal requested (...)`,
`d3d9: libd3d9metal (linked in) ready` and `[ios] Metal view: ...`.

- Device check first (`runtime/ios/metal_ios.m`): Source's compressed
  textures are DXT, and mtld3d creates BC textures with no CPU fallback
  (gl4es decompressed them). A GPU whose `supportsBCTextureCompression` is
  false stays on OpenGL, with a log line.
- Resolution: the Metal view takes the app's render scale (Résolution
  setting), like the GL view, so the drawable is the game's back buffer
  size and mtld3d does not resample.
- Frame rate: `present.maxFps` follows `P2_IOS_MAX_FPS` (60 by default);
  the 120 Hz display request is the same as on OpenGL. The FPS counter and
  back taps count frames on `Present` (`rt_d3d9_on_present`).
- Shader/pipeline cache: `Library/Caches/d3d9metal` (`P2_D3D9_CACHE` to
  move it). Memory caps: the same `MTLD3D_CONFIG` defaults as on macOS.
- Simulator: not available. mtld3d links MetalFX, which the iOS Simulator
  SDK does not have.
- `make d3d9metal-check D3D9METAL_A=…/libd3d9metal.a` drives the library
  alone, linked statically as in the app (macOS build of the library); the
  same source links against the iOS library.

## Building and running

```sh
ios/deps/build.sh device                          # SDL2, FreeType, gl4es (once, or after changing the patch)
python3 tools/build_ios.py --target device --release   # for playing: -O2 without memory checks
tools/ios_device.sh install                       # installs on the connected iPhone
tools/ios_device.sh data                          # copies the game data (~11.5 GB, once)
tools/ios_device.sh run 120 VAR=value…            # launches, waits, pulls back Documents/game.log
```

Simulator: `python3 tools/build_ios.py` then `tools/run_ios_sim.sh <seconds>`
(`P2_SIM_DEVICE=<UDID>` for a different simulator). The simulator renders in
software and interpolates triangles clipped at the near plane incorrectly
(`ios/probe/clip_test.c`): its artifacts do not exist on a real device.

Signing (free account): 7-day profiles obtained through
`xcodebuild -allowProvisioningUpdates` on `ios/signing/project.yml`, then
`ios/signing/sign.py` (called by `build_ios.py`). Launching fails if the
iPhone is locked.

The app identifier must belong to your Apple account: set `ios_bundle_id` in
`config/local.json` (or `P2_BUNDLE_ID`), read by every tool through
`tools/ios_bundle_id.py`; the probe uses the same identifier plus `.probe`.
First profile, once per identifier (then again every 7 days):

```sh
cd ios/signing
export P2_BUNDLE_ID=$(python3 ../../tools/ios_bundle_id.py)
xcodegen
xcodebuild -project P2Signing.xcodeproj -scheme Portal2 -destination generic/platform=iOS \
  -allowProvisioningUpdates DEVELOPMENT_TEAM=YOURTEAMID build
```

Replace `YOURTEAMID` with your team ID (Xcode > Settings > Accounts, or the
`OU` field of your Apple Development certificate).

`Documents/launch.args` in the app's container adds arguments
(one per line), for example `+map` followed by `sp_a2_laser_intro`.

The game language is written into the app's `launch.args` when it is built
(`-language <name>`, chosen as on the Mac: see "Game language" in the README).
To change it, set `"language"` in `config/local.json` and rebuild.

## Settings (iOS Settings app)

"Portal 2" page in the Settings app (`ios/Settings.bundle`, copied into the app
by `tools/build_ios.py`). At launch, `runtime/ios/settings.c` turns each
changed setting into a `P2_*` variable; a variable that is already set (devicectl,
simctl) takes precedence, and a setting that was never touched leaves the code's
default in place. Changes take effect on the next launch.

The setting titles below are the ones shown in the app, which is still in French.

| Setting | Variable | Values |
|---|---|---|
| Résolution (Resolution) | `P2_IOS_RENDER_SCALE` (+ the game's `-w`/`-h`) | native, ×2 (default), ×1.5, ×1 |
| Images par seconde (Frames per second) | `P2_IOS_MAX_FPS` | 60 (default), 120 |
| Moteur de rendu (Renderer) | `P2_RENDERER` | OpenGL (default), Metal: `d3d9`, see "Metal renderer" |
| Compteur d'images (FPS counter) | `P2_SHOW_FPS` | shown by default; tapping the top-left corner shows or hides it (choice is remembered) |
| Commandes à l'écran (On-screen controls) | `P2_TOUCH_CONTROLS` | automatic (hidden when a controller is connected), always, never |
| Caméra en trackpad (Trackpad camera) | key `P2TouchTrackpad` | also toggled by the button at the bottom left |
| Vitesse de la caméra (Camera speed) | `P2_TOUCH_LOOK` | ×0.5 to ×3 |
| Accélération du trackpad (Trackpad acceleration) | `P2_TOUCH_ACCEL` | none (1) to strong (4), normal 2.5 |
| Tapes au dos (Back taps) | `P2_BACKTAP` | enabled by default |
| Sensibilité des tapes (Tap sensitivity) | `P2_BACKTAP_SENSITIVITY` | 0.75 / 1 / 1.33: thresholds are divided by this value |
| Inverser gauche et droite (Swap left and right) | `P2_BACKTAP_INVERT` | |
| Boutons de portail (Portal buttons) | `P2_PORTAL_BUTTONS` | automatic (when back taps are off), always, never |
| Journal (Log) | `P2_LOG_FILE=Documents/game.log` | |

Simulator: `xcrun simctl spawn <device> defaults write <bundle id>
P2ResolutionScale -string 1.5`. `tests/checks_ios_settings.c` checks the
translation and that the keys match between the code and the settings page.

## Diagnostics

| Variable | Effect |
|---|---|
| `P2_LOG_FILE=Documents/game.log` | log file (relative to the container) |
| `P2_FRAME_LOG=1` | frame timing every 60 frames |
| `P2_PROFILE=Documents/profile.txt` (+ `P2_PROFILE_DELAY=60`, `P2_PROFILE_RAW=1`) | built-in profiler; with `RAW`, `tools/ios_symbolize.py` resolves driver functions using the device's symbols (Xcode, iOS DeviceSupport) |
| `P2_TRACE_FILES=<text>` | fopen/open/unlink/rename calls whose path contains the text |
| `P2_TOUCH_CONTROLS=0/1` | touch controls disabled / always shown |
| `P2_TOUCH_ACCEL=2.5` (+ `_LOW=300`, `_HIGH=1500`, `_SMOOTH=0.5`, `_LOG=1`) | trackpad acceleration: gain of 1 below `_LOW` pt/s, ramping up to the `P2_TOUCH_ACCEL` multiplier at `_HIGH` pt/s; speed is smoothed (weight `_SMOOTH`); `=1` disables it, `_LOG` logs speed and gain |
| `P2_BACKTAP=0` | disables back taps (enabled by default if the sensor exists; see "Back taps") |
| `P2_BACKTAP_LOG=1` | logs every candidate (from half the thresholds) and the background noise every 5 s |
| `P2_TOUCH_TEST_CLICK=s,x,y` | left click at (x, y) points, s seconds after the first menu |
| `P2_TOUCH_TEST_MOVE=0.5,1` (+ `_SECONDS`, `_DELAY`, `_ANGLE`, `P2_TOUCH_TEST_WASD`) | forced deflections of the movement stick |
| `P2_IOS_RENDER_SCALE=1/2/native` | render scale (points → pixels); `tools/ios_bench.py` compares antialiasing × scale |
| `P2_GL_STREAM_UNSYNC=0`, `P2_IOS_BGRA=0`, `P2_IOS_SRGB=0` | disable the fast paths or the emulation |
| `P2_IOS_SHADER_DUMP=<directory>`, `P2_IOS_GL_TRACE=1`, `P2_IOS_DRAW_LOG=<frame>` | converted shaders, compilation, draws for one frame |

Game console: `-condebug` writes it to
`Documents/portal2-userdata/portal2/console.log`.

## Back taps

Holding the phone in landscape, tapping the back near the left edge with an
index finger fires the blue portal, near the right edge the orange one
(`runtime/ios/back_tap.c`, tested by `tests/checks_back_tap.c`; CoreMotion in
`touch_controls.m`, on a dedicated queue, no permission required). Back taps
replace the overlay's portal buttons, which only come back without a motion
sensor (simulator) or with `P2_BACKTAP=0`. A candidate opens at half the
thresholds: |user acceleration| and its sample-to-sample change (jerk), or a
sudden change in rotation around x; it is judged 100 ms later against the full
thresholds. Two paths: acceleration (0.10 g, 6 g/s) or gyroscope (0.06 g,
6 g/s, 0.30 rad/s rotation, phone still just before); on the left, the index finger rotates the phone
(0.4 to 0.7 rad/s) while barely accelerating it (0.07 to 0.10 g).
Side: score = 0.5 × brief rotation around the device's x axis (vertical in
landscape, relative to the screen orientation, or to gravity as a fallback) + rotation
around z relative to the screen (the tapping hand pivots the phone within its
own plane: to the left, z > 0, for a left tap, in both landscape
orientations); 79/79 labeled taps, versus 72/79 with x alone (taps near a
corner, phone flipped); a
"lateral" fallback (tap on the edge of the phone) uses the sign of the y acceleration.
Rejections: menu open or overlay hidden (up to 80 ms before the peak), a touch starting or ending within ±80 ms,
agitation or a change in gravity before the peak (a 300 ms window, looking back as far as 600 ms, skipping the 250 ms following the two previous candidates; in the 50 ms just before, their vibration must have dropped below 25% of the peak, otherwise it counts as a jolt), phone lying flat screen-up (gravity z < -0.95). Click:
press, release after 50 ms and at least two frames. Defaults
calibrated on 2026-10-26 on an iPhone 15 Pro Max (`P2_BACKTAP_LOG=1`): 30 out of 30
back taps on the correct side in both landscape orientations; the hand makes the
phone vibrate 50 to 100 ms after the tap, hence the relaxed duration limit;
CoreMotion delivers 100 Hz at most. Taps on the edge of the phone have not been
tried. Hard taps peak 30 to 40 ms after they start and make the
phone vibrate for more than 150 ms: rise-time criterion dropped, vibration excluded for
250 ms (measured: 0.03 to 0.04 g RMS beyond that). Known limitation: the first jolt of a sudden movement from rest
looks like a tap (filtering it out would delay every shot).

Raw recording for tuning the thresholds on the Mac:
`P2_BACKTAP_RAW=Documents/backtap.raw` (samples and touches), then
`cc -O2 -o build/backtap-replay tools/backtap_replay.c -lm` and
`build/backtap-replay backtap.raw` (same `P2_BACKTAP_*` variables). `P2_TOUCH_CONTROLS=0` also disables back taps.

| Variable | Default | Purpose |
|---|---|---|
| `P2_BACKTAP_ACCEL` | 0.10 g | minimum acceleration peak |
| `P2_BACKTAP_JERK` | 6 g/s | minimum jerk |
| `P2_BACKTAP_ROT` | 0.08 rad/s | minimum rotation score to decide the side |
| `P2_BACKTAP_SIDE_X` | 0.5 | weight of rotation around x relative to rotation around z (`100`: x only, the old rule) |
| `P2_BACKTAP_GYRO_ACCEL` | 0.06 g | gyroscope path: minimum acceleration peak |
| `P2_BACKTAP_GYRO_ROT` | 0.30 rad/s | gyroscope path: minimum rotation around x (`99` disables it) |
| `P2_BACKTAP_GYRO_JERK` | 15 rad/s² | gyroscope path: rotation change that opens a candidate |
| `P2_BACKTAP_GYRO_CALM` | 0.03 g | gyroscope path: maximum agitation in the preceding 50 ms (phone still) |
| `P2_BACKTAP_REFRACTORY` | 0.25 s | nothing after a candidate |
| `P2_BACKTAP_TOUCH_GUARD` | 0.08 s | nothing around the start or end of a touch |
| `P2_BACKTAP_FLAT` | -0.95 g | gravity z below this threshold: phone lying flat, ignored (`-2`: never) |
| `P2_BACKTAP_WIDTH` | 0.12 s | maximum peak duration (above 35% of its maximum; measured over 100 ms, so inactive by default) |
| `P2_BACKTAP_AGITATION` | 0.05 g | maximum RMS over the preceding 300 ms (excluding vibration from taps) |
| `P2_BACKTAP_GRAVITY` | 0.12 g | maximum change in gravity over the preceding 200 ms |
| `P2_BACKTAP_INVERT` | 0 | `1`: left and right swapped |
| `P2_BACKTAP_LATERAL` | 1 | tap on the edge of the phone: `-1` reversed direction, `0` ignored |
| `P2_BACKTAP_ZSIGN` | 0 | `±1`: requires this sign of a_z at the start of the peak (back vs. screen) |
| `P2_BACKTAP_HOLD` | 0.05 s | press duration |
| `P2_BACKTAP_HZ` | 200 | rate requested from CoreMotion (100 Hz obtained on iPhone 15 Pro Max) |

Log: `[ios] tape <s> : pic, (x y z), secousse, rotation x y z, montée (délai du maximum),
durée, avant (agitation), gravité (variation, puis vecteur), toucher (écart en
ms), orientation -> côté ou raison du rejet` (the log is in French: tap, peak, (x y z), jerk,
rotation x y z, rise (time to maximum), duration, before (agitation), gravity (change, then
vector), touch (gap in ms), orientation -> side or rejection reason). Findings: a back tap has
a_z < 0 at the peak, a tap on the screen a_z > 0.

## Performance (sp_a2_laser_intro, iPhone 15 Pro Max, native 2796×1290)

| Step | fps |
|---|---|
| starting point (-O1 with checks) | ~5 |
| no more shader re-patching under a clip plane (gl4es) | 7-15 |
| `--release` + optimizations from the Mac work | ~11 |
| unsynchronized `glBufferSubData` | ~19 |
| BGRA colors in the buffer, swizzled by the shader | ~47 |

Uncapped by the display (`tools/ios_bench.py`, `P2_IOS_NO_PRESENT=1`),
sp_a2_laser_stairs at the spawn point: native 2796×1290 90 fps (4× MSAA) or
94 (without); scale 2 (1864×860) 154; 1.5 178. The GPU is the bottleneck: scale 2 is the
default on device. Heavy view of the same room (setpos -188.94 -396.92
-31.97): ~117 fps sustained at scale 2 with 4× MSAA (~150 before the
phone heats up). For playing: 60 Hz by default, steady;
`P2_IOS_MAX_FPS=120` requests 120 Hz (frame rate then between 60 and 120). The ProMotion display rounds the displayed frame rate (120, 60,
40…): a frame taking more than 8.3 ms is shown at 60.

Lessons: the translated game code costs little; the cost is in Apple's ES
driver, which waits for the GPU on any write to an in-flight buffer and
copies every vertex as soon as one attribute comes from client memory.
Profile with `P2_PROFILE_RAW=1` before guessing.

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
- Touch controls: a UIKit view placed on top of the window and force-drawn
  (SDL's loop never commits UIKit transactions).
