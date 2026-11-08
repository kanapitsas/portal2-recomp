# Controller investigation — 2026-11-05

## Result

On the Mac, an 8BitDo SN30 Pro (Bluetooth, Switch mode, seen by SDL's HIDAPI
driver as `Nintendo Switch Pro Controller`) was detected by the game but gave
no input. **Cause: Steam Input.** When the game starts, Steam takes the
controller for app 620 and applies a gamepad-emulation template to it; on
macOS that emulation reaches only games Steam launches itself (through its
injected overlay), so this app receives nothing.

**Fix, no code:** in Steam, *Portal 2 → Properties → Controller → Disable
Steam Input*. Documented in [usage.md](usage.md#controllers). On iPhone there
is no Steam client, so the problem does not exist there.

## How it was established

Three launches on the same build, controller connected over Bluetooth, Steam
running, `P2_INPUT_LOG=1` (the log prints the controller state every 500 ms,
accelerometer included, so reports can be seen arriving without pressing
anything):

| Steam Input for Portal 2 | Game's own Steam Input init | Result |
|---|---|---|
| On (default) | normal | axes, buttons and accelerometer stay at 0 |
| Off | normal | works: sticks, A button, accelerometer changing |
| On | suppressed (temporary `P2_STEAM_INPUT=0` switch) | no input, accelerometer at 0 |

So the game's own `ISteamInput::Init` is not what cuts the input: Steam's
per-app binding at launch is. The temporary switch was removed afterwards
since it cannot help. During the working run, a separate process
([hid_probe.c](../experiments/controller/hid_probe.c)) counted 66 input
reports per second (report `0x30`) and could open the controller in shared
mode.

Steam's logs (`~/Library/Application Support/Steam/logs/controller.txt` and
`controller_ui.txt`) show the switch: with Steam Input on, each launch prints
`Controller 0 uses xinput : true (warmed config for app 620)` and `Loaded
Config for Last Resort Path for App ID 620 ...
controller_switch_pro_gamepad_joystick.vdf`; turning it off prints `Opted-in
Controller Mask Forced Off` and loads `controller_base/empty.vdf` for 620.

Earlier observations that fit: a polling loop in `main` received input before
`SteamAPI_Init` in the same process, but not after; keeping a handle open,
`SDL_JOYSTICK_ALLOW_BACKGROUND_EVENTS`, explicit `SDL_JoystickUpdate()` and
main-thread initialization changed nothing.

Not established: whether Steam seizes the device (exclusive IOKit access) or
stops its reports some other way. It does not change the fix.

## Other findings

- **Steam Input bridge.** With the controller connected, the game trapped at
  `SteamInput006::slot17` (`GetDigitalActionData`, return address `0x13613f49`
  in `inputsystem.so`): methods that return a structure had no bridge.
  `runtime/steam.c` now bridges slots 17, 21 (`GetAnalogActionData`) and 29
  (`GetMotionData`) with the host return types and the i386 hidden result
  pointer, checked by `tests/checks_steam.c`.
- **Game input code (`inputsystem.so`).** Joystick init (`0x127f0`) returns
  early in gamepad-UI mode (`-gamepadui` or the `SteamDeck` environment
  variable) unless `-forcejoy` is given. The `joy_gamecontroller_config`
  ConVar's change callback (`0x12b80`) removes the event watch, closes the
  joystick, calls `SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER)` and runs init
  again. The game opens the controller three times during startup; harmless.
- **SDL 3.4.20** (Mac build, through sdl2-compat) opens HID devices without
  seizing them and has no Steam-specific behavior on macOS.

## Diagnostics kept

`P2_INPUT_LOG=1` logs joystick and controller events, including those seen by
the game's event watch, and the controller state with the accelerometer. The
temporary `P2_INPUT_PREINIT`, `P2_INPUT_PRECHECK`, `P2_INPUT_KEEP_OPEN`,
`P2_INPUT_FORCE_UPDATE` and `P2_STEAM_INPUT` switches were removed.

## Still open

- iPhone with a physical controller: tested after PR #14 (the virtual
  controller is unplugged and plugged back so the physical one gets player 1's
  slot; see [usage.md](usage.md#controllers)).
- Gameplay with the controller (camera sensitivity, button prompts, default
  bindings) has not been evaluated beyond the menu.
