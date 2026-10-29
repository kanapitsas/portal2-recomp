#!/bin/bash
# Removes what setup.sh / tools/make_app.py installed outside this folder:
# "Portal 2 Natif.app", its log and, only if you confirm separately, the saves
# and settings. Everything is moved to the Trash, not erased.
#
# usage: ./uninstall.sh
#
# The Steam game files and the Linux depot are never touched: they may be your
# real Steam install. Their paths (from config/local.json) are printed with
# what to do about them. This folder itself (build/, deps/) is left too.
set -euo pipefail

REPO="$(cd "$(dirname "$0")" && pwd)"
H="${P2_UNINSTALL_HOME:-$HOME}"   # override for tests only
CONFIG="${P2_CONFIG:-$REPO/config/local.json}"
NAME="Portal 2 Natif"
SUPPORT="$H/Library/Application Support/$NAME"
LOG="$H/Library/Logs/Portal2Natif.log"
TRASH="$H/.Trash"
SYSTEM_APPS="${P2_UNINSTALL_HOME:+$H/system}/Applications"   # tests never see /Applications

config_get() {  # key -> value from config/local.json, empty if absent
    [ -f "$CONFIG" ] || return 0
    python3 -c 'import json,sys; print(json.load(open(sys.argv[1])).get(sys.argv[2]) or "")' "$CONFIG" "$1"
}
ask() {  # question -> 0 only for an explicit yes
    local answer=""
    printf '%s [y/N] ' "$1"
    read -r answer || true
    case "$answer" in y|Y|yes|YES|o|O|oui) return 0 ;; *) return 1 ;; esac
}
trash() {  # path -> moved into the Trash under a free name
    local base dest
    base="$(basename "$1")"
    dest="$TRASH/$base"
    [ -e "$dest" ] && dest="$TRASH/$base $(date +%Y-%m-%d\ %H.%M.%S)"
    mkdir -p "$TRASH"
    mv "$1" "$dest"
    echo "moved to the Trash: $1"
}

if pgrep -qx portal2-native 2>/dev/null; then
    echo "Portal 2 Natif is running: quit the game first." >&2
    exit 1
fi

APPS=()
for a in "$H/Applications/$NAME.app" "$SYSTEM_APPS/$NAME.app"; do
    [ -e "$a" ] && APPS+=("$a")
done
SAVES=()
[ -e "$SUPPORT" ] && SAVES+=("$SUPPORT")
USER_DIR="$(config_get user_dir)"
USER_DIR="${USER_DIR/#\~/$H}"
# A custom user_dir is only trashed if it is safe: not the home folder or one of
# its ancestors, and neither inside nor around the game, the depot or this repo.
safe_user_dir() {  # path home protected... -> 0 if the path may be trashed
    python3 - "$@" <<'PY'
import os, sys
real = lambda p: os.path.realpath(os.path.expanduser(p))
path, home = real(sys.argv[1]), real(sys.argv[2])
inside = lambda a, b: a == b or a.startswith(b.rstrip('/') + '/')
bad = inside(home, path) or any(inside(path, p) or inside(p, path) for p in map(real, filter(None, sys.argv[3:])))
sys.exit(1 if bad else 0)
PY
}
case "$USER_DIR" in
    ""|"$SUPPORT"|"$SUPPORT"/*) ;;
    *)
        if [ -e "$USER_DIR" ]; then
            if safe_user_dir "$USER_DIR" "$H" "$(config_get game_root)" "$(config_get linux_root)" "$REPO"; then
                SAVES+=("$USER_DIR")
            else
                echo "Not offering to trash user_dir $USER_DIR: it overlaps your home, the game, the depot or this folder." >&2
            fi
        fi ;;
esac

echo "App:"
if [ ${#APPS[@]} -eq 0 ] && [ ! -e "$LOG" ]; then echo "  (nothing installed)"; fi
for a in ${APPS[@]+"${APPS[@]}"}; do echo "  $a"; done
[ -e "$LOG" ] && echo "  $LOG"
echo "Saves and settings (kept unless you say yes below):"
[ ${#SAVES[@]} -eq 0 ] && echo "  (none)"
for s in ${SAVES[@]+"${SAVES[@]}"}; do echo "  $s ($(du -sh "$s" 2>/dev/null | cut -f1 | tr -d ' '))"; done
echo

if [ ${#APPS[@]} -gt 0 ] || [ -e "$LOG" ]; then
    if ask "Move the app and its log to the Trash?"; then
        for a in ${APPS[@]+"${APPS[@]}"}; do trash "$a"; done
        [ -e "$LOG" ] && trash "$LOG"
    else
        echo "app kept"
    fi
fi
if [ ${#SAVES[@]} -gt 0 ]; then
    if ask "Also move your Portal 2 Natif saves and settings to the Trash?"; then
        for s in "${SAVES[@]}"; do trash "$s"; done
    else
        echo "saves kept"
    fi
fi

GAME="$(config_get game_root)"
LINUX="$(config_get linux_root)"
OLD_SAVES="$REPO/../work/portal2-userdata"
cat <<EOF

Not touched (delete them yourself if you no longer need them):
EOF
[ -n "$GAME" ] && cat <<EOF
- Portal 2 game files (about 12 GB): $GAME
  If this is your Steam install, uninstall Portal 2 from Steam instead
  (Library > Portal 2 > Manage > Uninstall); otherwise delete the folder.
EOF
[ -n "$LINUX" ] && cat <<EOF
- Linux binaries, depot 661 (about 90 MB): $LINUX
  Fetched with download_depot for this project; delete the folder when done with it.
EOF
[ -d "$OLD_SAVES" ] && cat <<EOF
- Older saves location (copied to the new one by make_app.py): $(cd "$OLD_SAVES" && pwd)
EOF
cat <<EOF
- This folder (code, build/, deps/): $REPO
- On an iPhone, remove the Portal 2 app like any other app.
EOF
