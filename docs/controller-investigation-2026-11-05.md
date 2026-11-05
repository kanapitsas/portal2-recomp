# Controller investigation — 2026-11-05

## Summary

On the Mac, an 8BitDo SN30 Pro (Bluetooth, Switch mode, seen as `Nintendo
Switch Pro Controller` by SDL's HIDAPI driver) is detected by the game but
gives no input at the menu. The leading explanation, not yet confirmed with
the controller connected, is that **Steam Input takes the controller for app
620**: Steam applies a gamepad-emulation template to it, and that emulation
reaches a game on macOS only through the overlay Steam injects when it
launches the game itself. Our app is not launched by Steam, so the game gets
nothing.

If this holds, the fix needs no code: in Steam, *Portal 2 → Properties →
Controller → Disable Steam Input*. On iPhone there is no Steam client (the
build uses a stub that reports no Steam Input controllers), so this problem
does not apply there.

## Evidence

### What the game process sees

- An independent SDL probe, run outside the game, receives both sticks,
  buttons, triggers and D-pad. A separate background app using the same
  bundled SDL libraries also received events while the game ran.
- In the game, `SDL_IsGameController(0)`, `SDL_JoystickOpen(0)` and
  `SDL_GameControllerOpen(0)` succeed and the controller stays attached, but
  at the menu neither event watches, polling nor direct state reads see
  anything (axes and A/B stay 0).
- A polling loop in `src/main.c` **before** `rt_init`, the renderer, the
  engine and Steam initialization received axis and button events in the game
  process. After startup, the same process received nothing, even with a
  handle kept open from `main` (`P2_INPUT_KEEP_OPEN=1`).
- `SDL_JOYSTICK_ALLOW_BACKGROUND_EVENTS=1`, an explicit `SDL_JoystickUpdate()`
  and initializing `SDL_INIT_GAMECONTROLLER` on the main thread changed
  nothing.

### What Steam does

Steam's own logs, `~/Library/Application Support/Steam/logs/controller.txt`
and `controller_ui.txt`, for today's sessions:

- `!! Steam controller device opened for index 0`, `Controller using HIDAPI
  driver, vid=0x057e, pid=0x2009`: the Steam client opens the controller
  itself.
- At every game launch: `Add to Config Cache Request 0 620 - adopting binding
  N`, `Controller 0 uses xinput : true (warmed config for app 620)`, and
  `Loaded Config for Last Resort Path for App ID 620, Controller 0:
  .../controller_base/templates/controller_switch_pro_gamepad_joystick.vdf`.
  Steam Input is active for Portal 2 with a gamepad-emulation ("xinput")
  template.
- `localconfig.vdf` has no per-game Steam Input override for 620, so the
  default applies. `Opted-in Controller Mask for AppId 413080: 1000` shows
  Switch Pro support is enabled in Steam's settings.
- The game also polls Steam Input through `ISteamInput006`
  (`GetDigitalActionData`, slot 17, trapped before the bridge fix below), so
  Steam reports the controller to the game as a Steam Input device.

The timing matches: SDL input works in the process before `SteamAPI_Init`,
and stops after Steam starts managing the controller for app 620.

### What the game's input code does (`inputsystem.so`)

- Joystick init (`0x127f0`) returns early in gamepad-UI mode (`-gamepadui`, or
  the `SteamDeck` environment variable) unless `-forcejoy` is given. Neither
  applies here.
- The `joy_gamecontroller_config` ConVar has a change callback (`0x12b80`)
  that removes the event watch, closes the joystick, calls
  `SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER)` and runs joystick init again.
  The log shows three open sequences during startup. These reopenings are
  not the cause on their own: a handle kept open from `main` did not help.

### SDL 3.4.20 (the library the Mac build uses)

- SDL opens HID devices without seizing them (`hid_darwin_set_open_exclusive(0)`
  in `src/hidapi/SDL_hidapi.c`).
- SDL does not change its behavior when it detects Steam on macOS
  (`SteamVirtualGamepadInfo` is a Linux mechanism).
- Over Bluetooth, the Switch driver drops and reopens the device after 3 s
  without reports, but only in full-report mode (`0x30`), not in simple mode
  (`0x3F`, which reports only on change). The game log shows the controller
  attached for more than 10 s with zero state. So either reports kept arriving
  with neutral values, or the controller was in simple mode, or the reopen
  succeeded each time. This detail does not fit "no reports at all" cleanly
  and is the main reason the explanation is not called confirmed.

An independent review (Codex) agreed that input stops inside the game
process during startup, but rated Steam as the specific cause low-confidence:
the Steam logs show Steam handling the controller, not that it captures it
from other clients. Its suggested A/B test is step 5 below.

## Not yet verified

Whether Steam **seizes** the device (exclusive IOKit access) or only stops it
from reporting to other clients is not known. Both lead to the same fix. The
measurement below settles it.

## Next test (about two minutes, no button presses needed)

With the controller connected over Bluetooth and Steam running:

1. Build the probe:
   `cc -O2 -framework IOKit -framework CoreFoundation experiments/controller/hid_probe.c -o build/hid_probe`.
2. Game closed: `./build/hid_probe 3`. Expect about 60 reports per second
   (the IMU streams even at rest). Note the count: if it is near zero here
   too, the controller is in a report-on-change mode and the counts below
   say nothing.
3. Start the game with `P2_INPUT_LOG=1`, wait for the menu, run
   `./build/hid_probe 3` again. 0 reports or a failed shared open means
   another client has taken the controller. `--seize` adds an exclusive-open
   test; it can briefly disturb other clients, so run it last.
4. The game's log (`~/Library/Logs/Portal2Natif.log`) prints `[input] state
   ... accel=x,y,z` every 500 ms. Changing values mean reports reach SDL in
   the game; frozen or zero values mean they do not.
5. A/B without touching Steam settings: relaunch with `P2_INPUT_LOG=1
   P2_STEAM_INPUT=0`. The game then never initializes Steam Input
   (`SteamInput006::Init` and `GetConnectedControllers` return 0, same for
   `SteamController008`) and reads controllers through SDL only. If the
   accelerometer moves in this run and not in step 4, Steam Input
   initialization is what cuts the reports, and the runtime can keep it off.
   If it stays frozen in both, Steam's per-app binding at launch (or
   something else in startup) is the cause.
6. Disable Steam Input for Portal 2 in Steam (*Properties → Controller*) and
   repeat step 4. Press a button in the menu to confirm whichever variant
   works.

The two results point to different fixes. If only step 6 works, document the
Steam setting (no code can undo a Steam-side capture). If step 5 works, make
`P2_STEAM_INPUT=0` the Mac default, since the game has no usable Steam Input
action set on macOS anyway (Steam falls back to a gamepad-emulation template).
If none works, the cause is inside the game's SDL lifecycle: compare SDL HID
reads before and after `SteamAPI_Init` with the same handle.

## Steam Input bridge fixed

With the controller connected, the app initially showed a black screen and
trapped at `SteamInput006::slot17`, return address `0x13613f49` in
`inputsystem.so`. The generated Steam bridge did not cover methods that
return a structure. `runtime/steam.c` now bridges slots 17
(`GetDigitalActionData`), 21 (`GetAnalogActionData`) and 29 (`GetMotionData`)
with their host return types and the i386 hidden result pointer;
`tests/checks_steam.c` checks the three conversions.

## Diagnostics left in place

Gated by `P2_INPUT_LOG` (now also logs the accelerometer), `P2_STEAM_INPUT=0`
(hides Steam Input from the game, `runtime/steam.c`), `P2_INPUT_PREINIT`,
`P2_INPUT_PRECHECK`, `P2_INPUT_KEEP_OPEN` and `P2_INPUT_FORCE_UPDATE`. `P2_INPUT_PRECHECK=1` with
`P2_INPUT_PREINIT=1` delays startup by three minutes on purpose. Remove the
`src/main.c` ones once the cause is confirmed. The input log also records
frequent window-grab calls and grows quickly.

## iPhone

Not tested with a physical controller yet. The code path exists: the
on-screen controls hide themselves and remove their virtual SDL controller
when a `GCController` with an extended gamepad connects
(`runtime/ios/touch_controls.m`), and SDL 2.30.9's MFi driver then exposes the
physical one to the game. To try it, pair the 8BitDo with the iPhone in
Switch mode (or any MFi, Xbox or DualSense controller) and start the game.
