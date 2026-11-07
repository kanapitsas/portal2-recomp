#!/bin/bash
# Removes what setup.sh / tools/make_app.py installed outside this folder:
# "Portal 2 Natif.app", its log, the installer app's build folder and, only if
# you confirm separately, the saves
# and settings. Everything is moved to the Trash, not erased.
#
# usage: ./uninstall.sh
#
# In a checkout set up with ./setup.sh --game hl2, the same for
# "Half-Life 2 Natif.app" and its own saves folder.
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
# Source and build unpacked by "Portal 2 Installer.app" (installer/Installer.swift).
BUILD_DIR="$H/Library/Application Support/$NAME Build"
LOG="$H/Library/Logs/Portal2Natif.log"
TRASH="$H/.Trash"
SYSTEM_APPS="${P2_UNINSTALL_HOME:+$H/system}/Applications"   # tests never see /Applications

config_get() {  # key -> value from config/local.json, empty if absent
    [ -f "$CONFIG" ] || return 0
    python3 -c 'import json,sys; print(json.load(open(sys.argv[1])).get(sys.argv[2]) or "")' "$CONFIG" "$1"
}
# Half-Life 2 checkout: its own app, log and data folder (tools/games.py).
TITLE="Portal 2"; GAME_SIZE=" (about 12 GB)"; DEPOT="depot 661 (about 90 MB)"
OTHER_SUPPORT=""   # another game's data folder: never offered from here
if [ "$(config_get game)" = hl2 ]; then
    OTHER_SUPPORT="$SUPPORT"
    NAME="Half-Life 2 Natif"
    SUPPORT="$H/Library/Application Support/$NAME"
    LOG="$H/Library/Logs/HalfLife2Natif.log"
    TITLE="Half-Life 2"; GAME_SIZE=""; DEPOT="depot 224 (about 320 MB)"
    BUILD_DIR=""   # the installer only builds Portal 2: its folder is never offered from here
fi
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
    echo "A recompiled game (portal2-native) is running: quit it first." >&2
    exit 1
fi

APPS=()
for a in "$H/Applications/$NAME.app" "$SYSTEM_APPS/$NAME.app"; do
    [ -e "$a" ] && APPS+=("$a")
done
[ -e "$BUILD_DIR" ] && APPS+=("$BUILD_DIR")
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
            if safe_user_dir "$USER_DIR" "$H" "$(config_get game_root)" "$(config_get linux_root)" "$REPO" "$OTHER_SUPPORT"; then
                SAVES+=("$USER_DIR")
            else
                echo "Not offering to trash user_dir $USER_DIR: it overlaps your home, the game, the depot, this folder or another game's data." >&2
            fi
        fi ;;
esac

echo "App:"
if [ ${#APPS[@]} -eq 0 ] && [ ! -e "$LOG" ]; then echo "  (nothing installed)"; fi
for a in ${APPS[@]+"${APPS[@]}"}; do
    if [ "$a" = "$BUILD_DIR" ]; then echo "  $a (source and build, $(du -sh "$a" 2>/dev/null | cut -f1 | tr -d ' '))"
    else echo "  $a"; fi
done
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
    if ask "Also move your $NAME saves and settings to the Trash?"; then
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
- $TITLE game files$GAME_SIZE: $GAME
  If this is your Steam install, uninstall $TITLE from Steam instead
  (Library > $TITLE > Manage > Uninstall); otherwise delete the folder.
EOF
[ -n "$LINUX" ] && cat <<EOF
- Linux binaries, $DEPOT: $LINUX
  Fetched with download_depot for this project; delete the folder when done with it.
EOF
[ "$TITLE" = "Portal 2" ] && [ -d "$OLD_SAVES" ] && cat <<EOF
- Older saves location (copied to the new one by make_app.py): $(cd "$OLD_SAVES" && pwd)
EOF
[ "$REPO" != "$BUILD_DIR" ] && cat <<EOF
- This folder (code, build/, deps/): $REPO
EOF
cat <<EOF
- On an iPhone, remove the Portal 2 app like any other app.
EOF
