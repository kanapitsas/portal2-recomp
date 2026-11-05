#!/bin/bash
# Portal 2 on a real iOS device, over the cable (xcrun devicectl).
#   tools/ios_device.sh probe [VAR=value...]    probe app (P2_PROBE_CLIP=1: near-plane clipping)
#   tools/ios_device.sh install                  install build/ios/device/Portal2.app
#   tools/ios_device.sh data                     copy the data into Documents (prep, game, linux)
#   tools/ios_device.sh has-data                 exit 0 when the data on the device is complete
#   tools/ios_device.sh has-mark                 exit 0 when a copy state marker exists (used by setup.sh)
#   tools/ios_device.sh has-game                 exit 0 when Documents/game/portal2 exists (older copies)
#   tools/ios_device.sh prep                     copy only Documents/prep again (after gameprep: ~80 MB)
#   tools/ios_device.sh run [seconds] [VAR=value...]   launch (default 180 s), then fetch Documents/game.log
# Device: P2_DEVICE (UDID or name), else the first connected iPhone/iPad.
set -euo pipefail
cd "$(dirname "$0")/.."
DEVICE=${P2_DEVICE:-$(xcrun devicectl list devices 2>/dev/null |
  awk '/physical/ && / connected|available \(paired\)/ {for (i = 1; i <= NF; ++i) if ($i ~ /^[0-9A-F]{8}-[0-9A-F]{16}$/) {print $i; exit}}')}
[ -n "$DEVICE" ] || { echo "aucun appareil connecté (câble, appareil déverrouillé, mode développeur)"; exit 1; }
APP_ID=$(python3 tools/ios_bundle_id.py)
PROBE_ID=$(python3 tools/ios_bundle_id.py --probe)
GAME_ROOT=$(python3 -c "import json;print(json.load(open('config/local.json'))['game_root'])")
LINUX_ROOT=$(python3 -c "import json;print(json.load(open('config/local.json'))['linux_root'])")
dc() {  # sous-commande de trois mots, --device, puis le reste (options et arguments)
  xcrun devicectl "${@:1:3}" --device "$DEVICE" "${@:4}"
}
env_json() {  # VAR=valeur... -> {"VAR":"valeur",...}
  python3 -c 'import json,sys; print(json.dumps(dict(a.split("=",1) for a in sys.argv[1:])))' "$@"
}
DATA_MARK=Documents/.p2-data-state  # « incomplete » pendant « data », « complete » à la fin
mark_data() {  # état écrit sur l'appareil (devicectl ne sait pas supprimer un fichier)
  local mark; mark=$(mktemp); echo "$1" > "$mark"
  copy_to "$APP_ID" "$mark" "$DATA_MARK" >/dev/null; rm -f "$mark"
}
has_file() {  # chemin relatif au conteneur : code 0 s'il existe sur l'appareil
  dc device info files --domain-type appDataContainer --domain-identifier "$APP_ID" \
    --subdirectory "$(dirname "$1")" --no-recurse 2>/dev/null |
    awk -v n="$(basename "$1")" '$1 == n {found = 1} END {exit !found}'
}
copy_to() {  # source destination (relative au conteneur de données)
  dc device copy to --domain-type appDataContainer --domain-identifier "$1" --source "$2" --destination "$3"
}

case "${1:-}" in
probe)
  shift
  dc device install app build/ios/device/probe.app >/dev/null
  dc device process launch --terminate-existing --environment-variables "$(env_json P2_PROBE_LOG=1 "$@")" \
    "$PROBE_ID" >/dev/null
  sleep "${PROBE_SECONDS:-20}"
  mkdir -p build/ios/device
  dc device copy from --domain-type appDataContainer --domain-identifier "$PROBE_ID" \
    --source Documents/probe.log --destination build/ios/device/probe.log >/dev/null
  cat build/ios/device/probe.log
  ;;
install)
  dc device install app build/ios/device/Portal2.app
  ;;
data)
  # Sans la bande son ni sdk_content (inutiles au jeu) : ~11,5 Go.
  mark_data incomplete
  copy_to $APP_ID build/game/prep Documents/prep
  copy_to $APP_ID "$LINUX_ROOT" Documents/linux
  for d in bin platform update portal2 portal2_dlc1 portal2_dlc2; do
    [ -e "$GAME_ROOT/$d" ] && copy_to $APP_ID "$GAME_ROOT/$d" "Documents/game/$d"
  done
  mark_data complete
  ;;
has-data)
  state=$(mktemp)
  dc device copy from --domain-type appDataContainer --domain-identifier "$APP_ID" \
    --source "$DATA_MARK" --destination "$state" >/dev/null 2>&1 && grep -qx complete "$state"
  status=$?; rm -f "$state"; exit $status
  ;;
has-mark)  # un état existe (copie commencée par cette version du script)
  has_file "$DATA_MARK"
  ;;
has-game)  # copies antérieures au marqueur : le dossier du jeu suffit
  has_file Documents/game/portal2
  ;;
prep)
  # Fichier par fichier : chacun remplace son homologue (les adresses des
  # modules suivent build/game/prep, celui avec lequel l'app a été construite).
  for f in build/game/prep/*; do copy_to $APP_ID "$f" "Documents/prep/$(basename "$f")" >/dev/null; done
  echo "Documents/prep : $(ls build/game/prep | wc -l | tr -d ' ') fichiers copiés"
  ;;
run)
  shift
  SECONDS_MAX=180
  case "${1:-}" in ''|*=*) ;; *) SECONDS_MAX=$1; shift ;; esac   # seconds are optional: `run P2_RENDERER=opengl`
  dc device process launch --terminate-existing \
    --environment-variables "$(env_json P2_LOG_FILE=Documents/game.log P2_FRAME_LOG=1 "$@")" $APP_ID
  sleep "$SECONDS_MAX"
  mkdir -p build/ios/device
  dc device copy from --domain-type appDataContainer --domain-identifier $APP_ID \
    --source Documents/game.log --destination build/ios/device/game.log
  echo "journal : build/ios/device/game.log ($(wc -l < build/ios/device/game.log) lignes)"
  ;;
*) awk 'NR > 1 && !/^#/ {exit} NR > 1 {sub(/^# ?/, ""); print}' "$0"; exit 2 ;;
esac
