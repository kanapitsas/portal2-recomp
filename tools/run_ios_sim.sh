#!/bin/bash
# Lance build/ios/sim/Portal2.app dans un simulateur iOS démarré sans fenêtre
# et écrit le journal dans build/ios/sim/launch.log.
#   tools/run_ios_sim.sh [secondes] [arguments du jeu...]
set -uo pipefail
cd "$(dirname "$0")/.."
# Simulateur : P2_SIM_DEVICE (UDID), sinon le premier démarré, sinon le premier iPhone disponible.
DEVICE=${P2_SIM_DEVICE:-$(xcrun simctl list devices booted | grep -oE '[0-9A-F-]{36}' | head -1)}
DEVICE=${DEVICE:-$(xcrun simctl list devices available | grep iPhone | grep -oE '[0-9A-F-]{36}' | head -1)}
APP_ID=$(python3 tools/ios_bundle_id.py)
SECONDS_MAX=${1:-120}; shift || true
MOD=$(python3 -c 'import sys; sys.path.insert(0, "tools"); import games, localconfig; print(games.profile(localconfig.load())["mod"])')
GAME_ARGS=("$@"); [ ${#GAME_ARGS[@]} -eq 0 ] && GAME_ARGS=(-game "$MOD" -novid -fullscreen)
GAME_ROOT=$(python3 -c "import json;print(json.load(open('config/local.json'))['game_root'])")
LINUX_ROOT=$(python3 -c "import json;print(json.load(open('config/local.json'))['linux_root'])")
xcrun simctl bootstatus "$DEVICE" -b >/dev/null 2>&1 || xcrun simctl boot "$DEVICE"
xcrun simctl terminate "$DEVICE" "$APP_ID" >/dev/null 2>&1 && sleep 2
xcrun simctl install "$DEVICE" build/ios/sim/Portal2.app
export SIMCTL_CHILD_P2_LOG_FILE="$PWD/build/ios/sim/game.log"
# Le simulateur expose une fausse manette MFi que le jeu prendrait avant la
# manette virtuelle du stick tactile (touch_controls.m).
export SIMCTL_CHILD_SDL_JOYSTICK_MFI=${SIMCTL_CHILD_SDL_JOYSTICK_MFI:-0}
# Le simulateur joue le son sur les haut-parleurs du Mac : muet sauf P2_SIM_AUDIO=1.
[ "${P2_SIM_AUDIO:-0}" = 1 ] || export SIMCTL_CHILD_SDL_AUDIODRIVER=dummy
perl -e "alarm $SECONDS_MAX; exec @ARGV" xcrun simctl launch --console-pty --terminate-running-process "$DEVICE" \
  "$APP_ID" --images "$PWD/build/game/prep" --game-dir "$GAME_ROOT" --linux-root "$LINUX_ROOT" \
  -- "${GAME_ARGS[@]}" > build/ios/sim/launch.log 2>&1
status=$?
xcrun simctl terminate "$DEVICE" "$APP_ID" >/dev/null 2>&1
echo "statut $status ; journal build/ios/sim/game.log ($(wc -l < build/ios/sim/game.log) lignes)"
