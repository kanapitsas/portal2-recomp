#!/bin/bash
# Removes what setup.sh / tools/make_app.py installed outside this folder, for
# each game (Portal 2, Half-Life 2): its app ("Portal 2 Natif.app",
# "Half-Life 2 Natif.app"), its log and, only if you confirm separately, its
# saves and settings; then the installer app's build folder, on its own yes.
# Everything is moved to the Trash, not erased.
#
# usage: ./uninstall.sh
#
# The Steam game files and the Linux depots are never touched: they may be your
# real Steam install. Their paths (from config/local.json) are printed with
# what to do about them. This folder itself (build/, deps/) is left too.
set -euo pipefail

REPO="$(cd "$(dirname "$0")" && pwd)"
H="${P2_UNINSTALL_HOME:-$HOME}"   # override for tests only
CONFIG="${P2_CONFIG:-$REPO/config/local.json}"
SUPPORT_ROOT="$H/Library/Application Support"
# Source and build unpacked by the installer app (installer/Installer.swift),
# shared by both games; the folder name is the one of the first release.
BUILD_DIR="$SUPPORT_ROOT/Portal 2 Natif Build"
TRASH="$H/.Trash"
SYSTEM_APPS="${P2_UNINSTALL_HOME:+$H/system}/Applications"   # tests never see /Applications
GAME_IDS=(portal2 hl2)

game_info() {  # game id -> TITLE NAME LOG GAME_SIZE DEPOT (tools/games.py)
    case "$1" in
        portal2) TITLE="Portal 2"; LOG_NAME=Portal2Natif.log; GAME_SIZE=" (about 12 GB)"; DEPOT="depot 661 (about 90 MB)" ;;
        hl2) TITLE="Half-Life 2"; LOG_NAME=HalfLife2Natif.log; GAME_SIZE=""; DEPOT="depot 224 (about 320 MB)" ;;
    esac
    NAME="$TITLE Natif"
    SUPPORT="$SUPPORT_ROOT/$NAME"
    LOG="$H/Library/Logs/$LOG_NAME"
}
config_get() {  # game id, key -> value from config/local.json for that game, empty if absent
    [ -f "$CONFIG" ] || return 0
    python3 - "$CONFIG" "$1" "$2" <<'PY'
import json, sys
config = json.load(open(sys.argv[1]))
name, key = sys.argv[2], sys.argv[3]
# Same rules as tools/localconfig.py (game_config): Portal 2's keys at the top
# level, another game's in "games"; an older Half-Life 2 config ("game": "hl2")
# had its keys at the top level.
legacy = config.get('game') not in (None, 'portal2') and config.get('game') not in (config.get('games') or {})
top = config.get('game', 'portal2') if legacy else 'portal2'
section = (config.get('games') or {}).get(name)
if isinstance(section, dict) and key in section: value = section[key]
elif name == top: value = config.get(key)
else: value = None
print(value or '')
PY
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
size() { du -sh "$1" 2>/dev/null | cut -f1 | tr -d ' '; }

# Every game's executable (tools/games.py EXECUTABLES). Tests use a fake home:
# a game running for real does not concern it.
if [ -z "${P2_UNINSTALL_HOME:-}" ] && pgrep -qx 'portal2-native|hl2-native' 2>/dev/null; then
    echo "A recompiled game (portal2-native or hl2-native) is running: quit it first." >&2
    exit 1
fi

# A custom user_dir is only trashed if it is safe: not the home folder or one of
# its ancestors, and neither inside nor around a game, a depot, this repo or
# another game's data folder.
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
PROTECTED=("$H" "$REPO")
for id in "${GAME_IDS[@]}"; do
    PROTECTED+=("$(config_get "$id" game_root)" "$(config_get "$id" linux_root)")
done

ANYTHING=0
for id in "${GAME_IDS[@]}"; do
    game_info "$id"
    APPS=()
    for a in "$H/Applications/$NAME.app" "$SYSTEM_APPS/$NAME.app"; do
        [ -e "$a" ] && APPS+=("$a")
    done
    SAVES=()
    [ -e "$SUPPORT" ] && SAVES+=("$SUPPORT")
    USER_DIR="$(config_get "$id" user_dir)"
    USER_DIR="${USER_DIR/#\~/$H}"
    OTHERS=()   # the other games' data folders: never offered from this game
    for other in "${GAME_IDS[@]}"; do
        [ "$other" = "$id" ] && continue
        OTHERS+=("$SUPPORT_ROOT/$(game_info "$other"; printf '%s' "$NAME")")
    done
    case "$USER_DIR" in
        ""|"$SUPPORT"|"$SUPPORT"/*) ;;
        *)
            if [ -e "$USER_DIR" ]; then
                if safe_user_dir "$USER_DIR" "${PROTECTED[@]}" "${OTHERS[@]}"; then
                    SAVES+=("$USER_DIR")
                else
                    echo "Not offering to trash user_dir $USER_DIR: it overlaps your home, a game, a depot, this folder or another game's data." >&2
                fi
            fi ;;
    esac
    [ ${#APPS[@]} -eq 0 ] && [ ! -e "$LOG" ] && [ ${#SAVES[@]} -eq 0 ] && continue
    ANYTHING=1

    echo "$TITLE:"
    for a in ${APPS[@]+"${APPS[@]}"}; do echo "  $a"; done
    [ -e "$LOG" ] && echo "  $LOG"
    [ ${#SAVES[@]} -gt 0 ] && echo "  Saves and settings (kept unless you say yes below):"
    for s in ${SAVES[@]+"${SAVES[@]}"}; do echo "    $s ($(size "$s"))"; done
    if [ ${#APPS[@]} -gt 0 ] || [ -e "$LOG" ]; then
        if ask "Move $NAME.app and its log to the Trash?"; then
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
    echo
done

if [ -e "$BUILD_DIR" ] && [ "$REPO" != "$BUILD_DIR" ]; then
    ANYTHING=1
    echo "Installer app's copy of the code and build:"
    echo "  $BUILD_DIR (source and build, $(size "$BUILD_DIR"))"
    if ask "Move it to the Trash (the installer can make it again)?"; then
        trash "$BUILD_DIR"
    else
        echo "build folder kept"
    fi
    echo
fi
[ $ANYTHING = 0 ] && echo "(nothing installed)"

OLD_SAVES="$REPO/../work/portal2-userdata"
cat <<EOF

Not touched (delete them yourself if you no longer need them):
EOF
for id in "${GAME_IDS[@]}"; do
    game_info "$id"
    GAME="$(config_get "$id" game_root)"
    LINUX="$(config_get "$id" linux_root)"
    [ -n "$GAME" ] && cat <<EOF
- $TITLE game files$GAME_SIZE: $GAME
  If this is your Steam install, uninstall $TITLE from Steam instead
  (Library > $TITLE > Manage > Uninstall); otherwise delete the folder.
EOF
    [ -n "$LINUX" ] && cat <<EOF
- $TITLE Linux binaries, $DEPOT: $LINUX
  Fetched with download_depot for this project; delete the folder when done with it.
EOF
done
[ -d "$OLD_SAVES" ] && cat <<EOF
- Older Portal 2 saves location (copied to the new one by make_app.py): $(cd "$OLD_SAVES" && pwd)
EOF
[ "$REPO" != "$BUILD_DIR" ] && cat <<EOF
- This folder (code, build/, deps/): $REPO
EOF
cat <<EOF
- On an iPhone, remove the Portal 2 app like any other app.
EOF
