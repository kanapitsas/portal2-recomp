#!/bin/bash
# Lance l'app iOS du jeu (build/ios/sim/Portal2.app, build/hl2/ios/sim/HalfLife2.app)
# dans un simulateur iOS démarré sans fenêtre et écrit le journal dans
# <ios_dir>/sim/launch.log et game.log.
#   tools/run_ios_sim.sh [--game portal2|hl2] [secondes] [arguments du jeu...]
# Jeu : --game, sinon P2_GAME, sinon config/local.json (tools/games.py selected).
set -uo pipefail
cd "$(dirname "$0")/.."
GAME_OPT=()
case "${1:-}" in
  --game) GAME_OPT=(--game "${2:?--game needs a value}"); shift 2 ;;
  --game=*) GAME_OPT=(--game "${1#--game=}"); shift ;;
esac
# Chemins et identifiant du jeu (tools/ios_game.py) : GAME_ROOT, LINUX_ROOT, PREP, IOS_DIR, IOS_APP, APP_ID, MOD...
VARS=$(python3 tools/ios_game.py ${GAME_OPT[@]+"${GAME_OPT[@]}"} --target sim) || exit 2
eval "$VARS"
# Simulateur : P2_SIM_DEVICE (UDID), sinon le premier démarré, sinon le premier iPhone disponible.
DEVICE=${P2_SIM_DEVICE:-$(xcrun simctl list devices booted | grep -oE '[0-9A-F-]{36}' | head -1)}
DEVICE=${DEVICE:-$(xcrun simctl list devices available | grep iPhone | grep -oE '[0-9A-F-]{36}' | head -1)}
SECONDS_MAX=${1:-120}; shift || true
GAME_ARGS=("$@"); [ ${#GAME_ARGS[@]} -eq 0 ] && GAME_ARGS=(-game "$MOD" -novid -fullscreen)
[ -d "$IOS_APP" ] || { echo "$IOS_APP absent : python3 tools/build_ios.py --game $GAME"; exit 2; }
xcrun simctl bootstatus "$DEVICE" -b >/dev/null 2>&1 || xcrun simctl boot "$DEVICE"
xcrun simctl terminate "$DEVICE" "$APP_ID" >/dev/null 2>&1 && sleep 2
xcrun simctl install "$DEVICE" "$IOS_APP"
export SIMCTL_CHILD_P2_LOG_FILE="$PWD/$IOS_DIR/game.log"
# Le simulateur expose une fausse manette MFi que le jeu prendrait avant la
# manette virtuelle du stick tactile (touch_controls.m).
export SIMCTL_CHILD_SDL_JOYSTICK_MFI=${SIMCTL_CHILD_SDL_JOYSTICK_MFI:-0}
# Le simulateur joue le son sur les haut-parleurs du Mac : muet sauf P2_SIM_AUDIO=1.
[ "${P2_SIM_AUDIO:-0}" = 1 ] || export SIMCTL_CHILD_SDL_AUDIODRIVER=dummy
perl -e "alarm $SECONDS_MAX; exec @ARGV" xcrun simctl launch --console-pty --terminate-running-process "$DEVICE" \
  "$APP_ID" --images "$PWD/$PREP" --game-dir "$GAME_ROOT" --linux-root "$LINUX_ROOT" \
  -- "${GAME_ARGS[@]}" > "$IOS_DIR/launch.log" 2>&1
status=$?
xcrun simctl terminate "$DEVICE" "$APP_ID" >/dev/null 2>&1
echo "statut $status ; journal $IOS_DIR/game.log ($(wc -l < "$IOS_DIR/game.log") lignes)"
