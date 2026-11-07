#!/bin/bash
# One-shot setup and build: checks the tools, downloads Valve's macOS
# libsteam_api.dylib (inside Steamworks.NET), writes config/local.json,
# recompiles each game and creates its app in ~/Applications:
# "Portal 2 Natif.app" and/or "Half-Life 2 Natif.app" (experimental, docs/hl2.md).
# With --ios, also builds, signs and installs the Portal 2 iPhone app.
# Host libraries (SDL2, FreeType, Fontconfig) come from Homebrew when it has
# them all, else from a prebuilt bundle downloaded from this project's GitHub
# releases (SHA-256 checked; tools/build_host_deps.sh makes it).
# P2_HOST_DEPS=homebrew or bundle forces one (P2_HOST_DEPS_URL: another copy). The Metal renderer's libd3d9metal
# is built when Rust (https://rustup.rs) is installed, else taken prebuilt from
# the same bundle. P2_NO_METAL=1 skips it: the game then uses OpenGL.
#
# usage: ./setup.sh [--game portal2|hl2|all] [--ios]
#        ./setup.sh [--ios] ["/path/to/Portal 2" ["/path/to/depot_661"]]
#        ./setup.sh --game hl2 ["/path/to/Half-Life 2" ["/path/to/depot_224"]]
#        ./setup.sh --detect [--game ...]   prints the games it would build, changes nothing
#
# Without --game, every game found on this Mac is built: the ones already in
# config/local.json, and the ones where Steam puts them (the game installed by
# Steam, and its Linux binaries fetched with the Steam console's
# download_depot command). --game all requires both. One checkout holds both
# games (build/game for Portal 2, build/hl2 for Half-Life 2): adding one never
# rebuilds the other. Paths are for one game; without --game, a folder with
# hl2_complete/ is taken for Half-Life 2.
#
# P2_APP_DIR puts the apps elsewhere than ~/Applications.
# Re-running is safe: finished steps are skipped and unchanged code is not
# recompiled. Downloads and iOS libraries go to ./deps (P2_DEPS overrides it).
# Saves go to ~/Library/Application Support/<app name>/userdata (config key
# user_dir). ./uninstall.sh removes the apps and, if asked, the saves.
set -euo pipefail

REPO="$(cd "$(dirname "$0")" && pwd)"
DEPS="${P2_DEPS:-$REPO/deps}"
CONFIG="$REPO/config/local.json"
IOS=0
DETECT=0
GAME_OPT=""
ARGS=()
while [ $# -gt 0 ]; do
    a="$1"; shift
    case "$a" in
        --ios) IOS=1 ;;
        --detect) DETECT=1 ;;
        --game) [ $# -gt 0 ] || { echo "--game needs a value: portal2, hl2 or all" >&2; exit 2; }
                GAME_OPT="$1"; shift ;;
        --game=*) GAME_OPT="${a#--game=}" ;;
        -h|--help) awk 'NR > 1 && !/^#/ {exit} NR > 1 {sub(/^# ?/, ""); print}' "$0"; exit 0 ;;  # leading comment block
        -*) echo "unknown option $a" >&2; exit 2 ;;
        *) ARGS+=("$a") ;;
    esac
done
GIVEN_GAME="${ARGS[0]:-}"
GIVEN_LINUX="${ARGS[1]:-}"
[ ${#ARGS[@]} -le 2 ] || { echo "too many arguments: at most the game folder and the Linux binaries folder" >&2; exit 2; }
case "$GAME_OPT" in
    ""|portal2|hl2|all) ;;
    *) echo "unknown game $GAME_OPT: use --game portal2, hl2 or all" >&2; exit 2 ;;
esac
if [ "$GAME_OPT" = all ] && [ -n "$GIVEN_GAME" ]; then
    echo "paths are for one game: use --game portal2 or --game hl2 with them" >&2; exit 2
fi
if [ $IOS = 1 ] && [ "$GAME_OPT" = hl2 ]; then
    echo "--ios builds Portal 2 only: Half-Life 2 runs on Mac for now" >&2; exit 2
fi

# Per game: title, Steam app and depots (tools/games.py), and the
# Steamworks.NET release whose libsteam_api.dylib has the SDK the game asks for.
game_info() {  # game id -> TITLE APPID LINUX_DEPOT STEAMWORKS_* APP_NAME LOG_NAME
    case "$1" in
    portal2)
        TITLE="Portal 2"; APPID=620; LINUX_DEPOT=661
        STEAMWORKS_VERSION=20.1.0
        STEAMWORKS_SHA256="3be79e6bdf53c86210267bf39f46362bb839130896006aec26364c5efc37e314" ;;
    hl2)
        TITLE="Half-Life 2"; APPID=220; LINUX_DEPOT=224
        STEAMWORKS_VERSION=2024.8.0   # SDK 1.60: HL2's client.so asks for newer interfaces (docs/steam.md)
        STEAMWORKS_SHA256="0a8e9b8c95760e81fcb16477a8962a4c230ea887b3731b1f9bc0690677f6f087" ;;
    esac
    APP_NAME="$TITLE Natif"
    LOG_NAME="$(printf '%s' "$TITLE" | tr -d ' -')Natif.log"
    STEAMWORKS_URL="https://github.com/rlabrecque/Steamworks.NET/releases/download/$STEAMWORKS_VERSION/Steamworks.NET_$STEAMWORKS_VERSION.unitypackage"
}
STEAM_SUPPORT="$HOME/Library/Application Support/Steam"
# Metal renderer: libd3d9metal, built from mtld3d's native host branch (zlib).
MTLD3D_URL="https://github.com/kanapitsas/mtld3d.git"
MTLD3D_REV="101f37d3d45c760e95e20176f758b9d309c23885"   # feat/native-host, pinned
# Prebuilt host libraries and libd3d9metal (built from MTLD3D_REV); see
# tools/build_host_deps.sh, whose BUNDLE_VERSION this matches.
HOST_DEPS_NAME="p2-host-deps-1-macos-arm64"
HOST_DEPS_URL="${P2_HOST_DEPS_URL:-https://github.com/kanapitsas/portal2-recomp/releases/download/host-deps-1/$HOST_DEPS_NAME.tar.xz}"
HOST_DEPS_SHA256="f5454da33fce938b70df1b7523df0a6884eba4249d5c9092898079964bb75b61"

step() { printf '\n==> %s\n' "$*"; }
die() { printf 'error: %s\n' "$*" >&2; exit 1; }
warn() { printf 'warning: %s\n' "$*" >&2; }
need() { command -v "$1" >/dev/null 2>&1 || die "$1 not found. $2"; }
config_get() {  # [game id] key -> value from config/local.json for that game (tools/localconfig.py), empty if absent
    [ -f "$CONFIG" ] || return 0
    local id=portal2
    [ $# -gt 1 ] && { id="$1"; shift; }
    python3 -c 'import sys; sys.path.insert(0, sys.argv[1]); import localconfig
print(localconfig.game_config(localconfig.load(sys.argv[2]), sys.argv[3]).get(sys.argv[4]) or "")' \
        "$REPO/tools" "$CONFIG" "$id" "$1"
}
find_depot() {  # app id, depot id -> folder written by the Steam console's download_depot
    [ -d "$STEAM_SUPPORT" ] && [ -n "$2" ] || return 0
    find "$STEAM_SUPPORT" -maxdepth 9 -type d -path "*/content/app_$1/depot_$2" 2>/dev/null | head -1
}

# --- Checks ---------------------------------------------------------------
[ "$(uname -s)" = Darwin ] && [ "$(uname -m)" = arm64 ] || die "this builds for Apple silicon: run it on an arm64 Mac"
step "Checking tools"
xcode-select -p >/dev/null 2>&1 || die "Xcode command line tools missing: xcode-select --install"
need python3 "It comes with the Xcode command line tools: xcode-select --install"
python3 -c 'import sys; sys.exit(sys.version_info < (3, 9))' || die "Python 3.9 or newer is required"
need objdump "It comes with the Xcode command line tools: xcode-select --install"
need curl ""
BREW_PREFIX=/opt/homebrew
brew_has_libs() {
    local lib
    for lib in SDL2 freetype fontconfig; do
        ls "$BREW_PREFIX/lib/lib$lib".* >/dev/null 2>&1 || return 1
    done
    [ -f "$BREW_PREFIX/include/SDL2/SDL.h" ] && [ -d "$BREW_PREFIX/include/freetype2" ]
}
case "${P2_HOST_DEPS:-auto}" in
    homebrew) brew_has_libs || die "P2_HOST_DEPS=homebrew but Homebrew lacks them: brew install sdl2 freetype fontconfig"
              HOST_LIBS_FROM=homebrew ;;
    bundle)   HOST_LIBS_FROM=bundle ;;
    auto)     if brew_has_libs; then HOST_LIBS_FROM=homebrew; else HOST_LIBS_FROM=bundle; fi ;;
    *) die "P2_HOST_DEPS must be homebrew or bundle, not $P2_HOST_DEPS" ;;
esac
if [ $IOS = 1 ]; then
    need xcodebuild "Install Xcode from the App Store, open it once, then: sudo xcode-select -s /Applications/Xcode.app"
    xcrun --sdk iphoneos --show-sdk-path >/dev/null 2>&1 || die "no iOS SDK: install Xcode (not only the command line tools) and select it with xcode-select -s"
    need xcodegen "brew install xcodegen"
    need cmake "brew install cmake"
    need ninja "brew install ninja"
fi
echo "ok: $(python3 --version), $(objdump --version | head -1)"

# --- Game folders ---------------------------------------------------------
step "Finding the games"
# A path dragged into Terminal comes with "\ " escapes and a trailing space; inside
# quotes they reach us verbatim. Undo that when the path does not exist as typed.
unescape_path() {
    local p="$1"
    while [ "${p% }" != "$p" ]; do p="${p% }"; done
    [ -e "$p" ] || p="$(printf '%s' "$p" | sed 's/\\\(.\)/\1/g')"
    printf '%s' "$p"
}
[ -n "$GIVEN_GAME" ] && GIVEN_GAME="$(unescape_path "$GIVEN_GAME")"
[ -n "$GIVEN_LINUX" ] && GIVEN_LINUX="$(unescape_path "$GIVEN_LINUX")"
# Paths without --game: a Half-Life 2 folder has hl2_complete/.
if [ -z "$GAME_OPT" ] && [ -n "$GIVEN_GAME" ]; then
    if [ -d "$GIVEN_GAME/hl2_complete" ]; then GAME_OPT=hl2; else GAME_OPT=portal2; fi
    [ $IOS = 1 ] && [ $GAME_OPT = hl2 ] && { echo "--ios builds Portal 2 only: Half-Life 2 runs on Mac for now" >&2; exit 2; }
fi

locate() {  # game id [game folder [Linux folder]] -> FOUND_GAME FOUND_LINUX (empty when not found)
    local c
    FOUND_GAME="${2:-}"; FOUND_LINUX="${3:-}"
    if [ "$1" = portal2 ]; then
        if [ -z "$FOUND_GAME" ]; then
            for c in "$(config_get portal2 game_root)" "$STEAM_SUPPORT/steamapps/common/Portal 2" "$(find_depot 620 621)"; do
                # pak01_dir.vpk, not just portal2/: Steam can leave a nearly empty Portal 2 folder.
                [ -n "$c" ] && [ -f "$c/portal2/pak01_dir.vpk" ] && { FOUND_GAME="$c"; break; }
            done
        fi
        if [ -z "$FOUND_LINUX" ]; then
            for c in "$(config_get portal2 linux_root)" "$(find_depot 620 661)"; do
                [ -n "$c" ] && [ -f "$c/bin/linux32/engine.so" ] && { FOUND_LINUX="$c"; break; }
            done
        fi
    else   # hl2: the Mac game from Steam, the Linux binaries from depot 224
        if [ -z "$FOUND_GAME" ]; then
            for c in "$(config_get hl2 game_root)" "$STEAM_SUPPORT/steamapps/common/Half-Life 2"; do
                [ -n "$c" ] && [ -f "$c/hl2_complete/gameinfo.txt" ] && { FOUND_GAME="$c"; break; }
            done
        fi
        if [ -z "$FOUND_LINUX" ]; then
            for c in "$(config_get hl2 linux_root)" "$(find_depot 220 224)"; do
                [ -n "$c" ] && [ -f "$c/hl2_linux" ] && { FOUND_LINUX="$c"; break; }
            done
        fi
    fi
    return 0
}

check_game() {  # game id -> dies unless FOUND_GAME and FOUND_LINUX are that game's
    local f
    if [ "$1" = portal2 ]; then
        [ -n "$FOUND_GAME" ] || die "Portal 2 data not found. Pass the game folder (the one with portal2/ and platform/), or fetch it:
  open steam://open/console, type: download_depot 620 621   (12.6 GB), then re-run."
        [ -f "$FOUND_GAME/portal2/pak01_dir.vpk" ] && [ -d "$FOUND_GAME/platform" ] \
            || die "$FOUND_GAME does not look like Portal 2 (no portal2/pak01_dir.vpk or platform/)"
        for f in mssmp3.asi mssvoice.asi binkawin.asi; do
            [ -f "$FOUND_GAME/bin/linux32/$f" ] || warn "$FOUND_GAME/bin/linux32/$f is missing: some audio (music, voice) may be silent"
        done
        [ -n "$FOUND_LINUX" ] || die "Portal 2's Linux binaries not found. Fetch them with your Steam account:
  open steam://open/console, type: download_depot 620 661   (about 90 MB), wait for
  'Depot download complete', then re-run (or pass that folder as the second argument)."
        [ -f "$FOUND_LINUX/bin/linux32/engine.so" ] && [ -f "$FOUND_LINUX/portal2/bin/linux32/server.so" ] \
            || die "$FOUND_LINUX does not look like depot 661 (no bin/linux32/engine.so or portal2/bin/linux32/server.so)"
    else
        [ -n "$FOUND_GAME" ] || die "Half-Life 2 not found. Install it with Steam (the current version, with hl2_complete/),
  or pass the game folder (the one with hl2/, hl2_complete/ and platform/)."
        [ -f "$FOUND_GAME/hl2/hl2_pak_dir.vpk" ] && [ -f "$FOUND_GAME/hl2_complete/gameinfo.txt" ] && [ -d "$FOUND_GAME/platform" ] \
            || die "$FOUND_GAME does not look like Half-Life 2 (no hl2/hl2_pak_dir.vpk, hl2_complete/gameinfo.txt or platform/).
  Only the current Steam version (20th anniversary update, with hl2_complete/) is supported."
        [ -n "$FOUND_LINUX" ] || die "Half-Life 2's Linux binaries not found. Fetch them with your Steam account:
  open steam://open/console, type: download_depot 220 224   (about 320 MB), wait for
  'Depot download complete', then re-run (or pass that folder as the second argument)."
        [ -f "$FOUND_LINUX/hl2_linux" ] && [ -f "$FOUND_LINUX/bin/engine.so" ] && [ -f "$FOUND_LINUX/hl2_complete/bin/server.so" ] \
            || die "$FOUND_LINUX does not look like depot 224 (no hl2_linux, bin/engine.so or hl2_complete/bin/server.so)"
        for f in mssmp3.asi binkawin.asi mssmixer.mix; do
            [ -f "$FOUND_LINUX/bin/linux32/$f" ] || warn "$FOUND_LINUX/bin/linux32/$f is missing: some audio may be silent"
        done
    fi
    return 0
}

# Which games: --game, else every game whose data and Linux binaries are found.
# A game installed in Steam without its Linux binaries is skipped with a
# warning, unless it is the only one found (then the error says what to fetch).
SELECTED=(); GAME_DIRS=(); LINUX_DIRS=(); SKIPPED=()
case "$GAME_OPT" in
    portal2|hl2) WANTED=("$GAME_OPT") ;;
    all) WANTED=(portal2 hl2) ;;
    *) WANTED=() ;;
esac
if [ ${#WANTED[@]} -gt 0 ]; then
    for id in "${WANTED[@]}"; do
        locate "$id" "$GIVEN_GAME" "$GIVEN_LINUX"
        check_game "$id"
        SELECTED+=("$id"); GAME_DIRS+=("$FOUND_GAME"); LINUX_DIRS+=("$FOUND_LINUX")
    done
else
    for id in portal2 hl2; do
        locate "$id"
        if [ -n "$FOUND_GAME" ] && [ -n "$FOUND_LINUX" ]; then
            check_game "$id"
            SELECTED+=("$id"); GAME_DIRS+=("$FOUND_GAME"); LINUX_DIRS+=("$FOUND_LINUX")
        elif [ -n "$FOUND_GAME" ]; then
            SKIPPED+=("$id")
        fi
    done
    if [ ${#SELECTED[@]} -eq 0 ]; then
        id="${SKIPPED[0]:-portal2}"
        locate "$id"
        [ -n "$FOUND_GAME" ] || die "no game found. Install Portal 2 or Half-Life 2 with Steam and fetch its Linux binaries
  (README.md), or pass the game folder: ./setup.sh \"/path/to/Portal 2\" \"/path/to/depot_661\""
        check_game "$id"   # says what is missing
    fi
fi
if [ $IOS = 1 ] && [ "${SELECTED[0]}" != portal2 ]; then
    die "--ios builds Portal 2 only, and Portal 2 was not found"
fi
for i in "${!SELECTED[@]}"; do
    game_info "${SELECTED[$i]}"
    echo "$TITLE:"
    echo "  game data:      ${GAME_DIRS[$i]}"
    echo "  Linux binaries: ${LINUX_DIRS[$i]}"
done
SKIPPED_NOTE=""
for id in ${SKIPPED[@]+"${SKIPPED[@]}"}; do
    game_info "$id"
    SKIPPED_NOTE="$TITLE is installed but its Linux binaries were not found: to add it, type
  download_depot $APPID $LINUX_DEPOT in the Steam console (open steam://open/console), then re-run."
    warn "$SKIPPED_NOTE"
done
if [ $DETECT = 1 ]; then
    echo "would build: ${SELECTED[*]}"
    exit 0
fi

# --- Steamworks.NET (20.1.0, 2024.8.0 for HL2): steam_api.bundle ----------
steam_dylib() {  # game id -> DYLIB, Valve's libsteam_api.dylib for that game
    game_info "$1"
    local default bundle sw got t d p rel
    DYLIB="$(config_get "$1" steam_api_dylib)"
    if [ "$1" = portal2 ]; then
        default="$REPO/../work/downloads/steamworks/steam_api.bundle/Contents/MacOS/libsteam_api.dylib"
        bundle="$DEPS/steam_api.bundle"
    else
        default="$REPO/../work/downloads/steamworks-$STEAMWORKS_VERSION/steam_api.bundle/Contents/MacOS/libsteam_api.dylib"
        bundle="$DEPS/steamworks-$STEAMWORKS_VERSION/steam_api.bundle"
    fi
    [ -z "$DYLIB" ] && [ -f "$default" ] && DYLIB="$default"
    [ -n "$DYLIB" ] && [ -f "$DYLIB" ] && return 0
    DYLIB="$bundle/Contents/MacOS/libsteam_api.dylib"
    [ -f "$DYLIB" ] && return 0
    step "Downloading Steamworks.NET $STEAMWORKS_VERSION (Valve's libsteam_api.dylib for $TITLE)"
    mkdir -p "$DEPS"
    sw="$DEPS/Steamworks.NET_$STEAMWORKS_VERSION.unitypackage"
    [ -f "$sw" ] || curl -fsSL --retry 3 -o "$sw" "$STEAMWORKS_URL"
    got=$(shasum -a 256 "$sw" | cut -d' ' -f1)
    [ "$got" = "$STEAMWORKS_SHA256" ] || die "$sw: SHA-256 $got, expected $STEAMWORKS_SHA256 (delete it and re-run)"
    t=$(mktemp -d)
    tar -xzf "$sw" -C "$t"
    rm -rf "$bundle"
    for d in "$t"/*; do
        [ -f "$d/asset" ] && [ -f "$d/pathname" ] || continue
        p=$(head -1 "$d/pathname")
        case "$p" in
            */Plugins/steam_api.bundle/*)
                rel="${p#*/Plugins/steam_api.bundle/}"
                mkdir -p "$bundle/$(dirname "$rel")"
                cp "$d/asset" "$bundle/$rel" ;;
        esac
    done
    rm -rf "$t"
    [ -f "$DYLIB" ] || die "steam_api.bundle not found in the Steamworks.NET package"
}
DYLIBS=()
for id in "${SELECTED[@]}"; do
    steam_dylib "$id"
    DYLIBS+=("$DYLIB")
done

# --- Prebuilt host libraries (when Homebrew's or Rust are missing) --------
HOST_DEPS="$DEPS/$HOST_DEPS_NAME"
METAL_WANTED=1
[ -n "${P2_NO_METAL:-}" ] && [ "$P2_NO_METAL" != 0 ] && METAL_WANTED=0
host_deps_complete() {
    local f
    for f in manifest.json etc/fonts/fonts.conf include/SDL2/SDL.h include/freetype2/ft2build.h \
             lib/libSDL2.dylib lib/libSDL3.dylib lib/libfreetype.dylib lib/libfontconfig.dylib lib/libd3d9metal.dylib; do
        [ -e "$HOST_DEPS/$f" ] || return 1
    done
}
fetch_host_deps() {
    host_deps_complete && return 0
    step "Downloading prebuilt host libraries ($HOST_DEPS_NAME, about 4 MB)"
    mkdir -p "$DEPS"
    local archive="$DEPS/$HOST_DEPS_NAME.tar.xz" got tmp
    if [ ! -f "$archive" ]; then
        curl -fsSL --retry 3 -o "$archive.part" "$HOST_DEPS_URL" && mv "$archive.part" "$archive" \
            || { rm -f "$archive.part"; return 1; }
    fi
    got=$(shasum -a 256 "$archive" | cut -d' ' -f1)
    [ "$got" = "$HOST_DEPS_SHA256" ] || { warn "$archive: SHA-256 $got, expected $HOST_DEPS_SHA256"; rm -f "$archive"; return 1; }
    tmp=$(mktemp -d "$DEPS/.host-deps.XXXXXX")
    tar -xJf "$archive" -C "$tmp" && [ -f "$tmp/$HOST_DEPS_NAME/manifest.json" ] || { rm -rf "$tmp"; return 1; }
    rm -rf "$HOST_DEPS" && mv "$tmp/$HOST_DEPS_NAME" "$HOST_DEPS" && rm -rf "$tmp" "$archive"
    host_deps_complete || { warn "$HOST_DEPS is incomplete"; rm -rf "$HOST_DEPS"; return 1; }
}
HOST_PREFIX=""   # empty: Homebrew
step "Host libraries (SDL2, FreeType, Fontconfig)"
if [ $HOST_LIBS_FROM = bundle ]; then
    python3 -c 'import platform, sys; sys.exit(tuple(map(int, platform.mac_ver()[0].split(".")[:2])) < (13, 0))' \
        || die "the prebuilt libraries need macOS 13 or newer; on older macOS: brew install sdl2 freetype fontconfig"
    fetch_host_deps || die "could not get the prebuilt host libraries from $HOST_DEPS_URL.
  Check the connection and re-run, or install Homebrew's: brew install sdl2 freetype fontconfig"
    HOST_PREFIX="$HOST_DEPS"
    echo "host libraries: prebuilt ($HOST_DEPS)"
else
    echo "host libraries: Homebrew ($BREW_PREFIX)"
fi
# The bundle's libd3d9metal, when built from the same mtld3d commit as ours.
prebuilt_d3d9metal() {
    fetch_host_deps || return 1
    python3 -c 'import json,sys; sys.exit(json.load(open(sys.argv[1])).get("mtld3d_rev") != sys.argv[2])' \
        "$HOST_DEPS/manifest.json" "$MTLD3D_REV" || return 1
    D3D9METAL="$HOST_DEPS/lib/libd3d9metal.dylib"
}

# --- Metal renderer: libd3d9metal (optional) -----------------------------
# Built from a pinned commit of mtld3d's native host branch, or prebuilt from
# the same commit without Rust. Any problem here only costs the Metal renderer:
# the game then uses OpenGL.
D3D9METAL=""
D3D9METAL_IOS=""
METAL_SRC="$DEPS/mtld3d-native"        # owned by setup.sh (marker file); never a checkout of yours
fetch_mtld3d() {
    if [ -e "$METAL_SRC" ] && [ ! -f "$METAL_SRC/.p2-setup" ]; then
        warn "$METAL_SRC exists and was not made by setup.sh: leaving it alone"; return 1
    fi
    [ "$(git -C "$METAL_SRC" rev-parse HEAD 2>/dev/null)" = "$MTLD3D_REV" ] && return 0
    mkdir -p "$METAL_SRC" && touch "$METAL_SRC/.p2-setup" && git -C "$METAL_SRC" init -q \
        && git -C "$METAL_SRC" fetch -q --depth 1 "$MTLD3D_URL" "$MTLD3D_REV" \
        && git -C "$METAL_SRC" checkout -q --force FETCH_HEAD
}
build_d3d9metal() {
    if ! command -v cargo >/dev/null 2>&1; then
        if prebuilt_d3d9metal; then
            echo "Metal renderer: prebuilt libd3d9metal (Rust not found)"
        else
            warn "Rust not found and no prebuilt libd3d9metal: the game will use OpenGL. For the
  faster Metal renderer, install Rust (https://rustup.rs) and re-run ./setup.sh."
        fi
        [ $IOS = 1 ] && warn "the iPhone app needs Rust for Metal (https://rustup.rs): it will use OpenGL"
        return 0
    fi
    step "Building libd3d9metal for the Metal renderer (a few minutes the first time)"
    fetch_mtld3d || { warn "could not get mtld3d from $MTLD3D_URL: the game will use OpenGL"; return 0; }
    local target="$METAL_SRC/native/target" out
    out="$target/production/libd3d9metal.dylib"
    if (cd "$METAL_SRC/native" && CARGO_TARGET_DIR="$target" cargo build -q --profile production) && [ -f "$out" ]; then
        D3D9METAL="$out"
    else
        warn "libd3d9metal did not build (mtld3d needs Rust 1.99 or newer: rustup update): the game will use OpenGL"
        return 0
    fi
    if [ $IOS = 1 ]; then
        out="$target/aarch64-apple-ios/production/libd3d9metal.a"
        if { ! command -v rustup >/dev/null 2>&1 || rustup target add aarch64-apple-ios >/dev/null 2>&1; } \
            && (cd "$METAL_SRC/native" && CARGO_TARGET_DIR="$target" cargo build -q --profile production --target aarch64-apple-ios) \
            && [ -f "$out" ]; then
            D3D9METAL_IOS="$out"
        else
            warn "libd3d9metal for iOS did not build: the iPhone app will use OpenGL"
        fi
    fi
}
METAL_OFF=0
if [ $METAL_WANTED = 0 ]; then
    echo "P2_NO_METAL set: OpenGL renderer only"
    METAL_OFF=1
else
    build_d3d9metal
fi

# --- iOS identity (before writing the config) -----------------------------
TEAM=""
if [ $IOS = 1 ]; then
    step "Finding your Apple development team"
    CERT=$(security find-certificate -a -c "Apple Development" -p 2>/dev/null | awk '/BEGIN/{n++} n==1' || true)
    [ -n "$CERT" ] || die "no Apple Development certificate. In Xcode: Settings > Accounts, add your Apple ID
  (a free one works), select its team, Manage Certificates > + > Apple Development. Then re-run."
    TEAM=$(printf '%s\n' "$CERT" | openssl x509 -noout -subject -nameopt multiline 2>/dev/null \
        | awk -F' = ' '/organizationalUnitName/ {print $2; exit}')
    TEAM="${P2_TEAM_ID:-$TEAM}"
    [ -n "$TEAM" ] || die "could not read your team ID from the certificate; set P2_TEAM_ID"
    echo "team: $TEAM"
fi

# --- config/local.json ----------------------------------------------------
# Shared keys at the top level; Portal 2's game keys too (as in single-game
# configs), another game's in "games" (tools/localconfig.py).
step "Writing config/local.json"
mkdir -p "$REPO/config"
GAME_ARGS=()
for i in "${!SELECTED[@]}"; do GAME_ARGS+=("${SELECTED[$i]}" "${GAME_DIRS[$i]}" "${LINUX_DIRS[$i]}" "${DYLIBS[$i]}"); done
python3 - "$REPO/tools" "$CONFIG" "$TEAM" "$D3D9METAL" "$D3D9METAL_IOS" "$METAL_OFF" "$HOST_PREFIX" "${GAME_ARGS[@]}" <<'EOF'
import json, os, sys
from pathlib import Path
sys.path.insert(0, sys.argv[1])
import games, localconfig
path, team, metal, metal_ios, metal_off, host_prefix, *game_args = sys.argv[2:]
p = Path(path)
# An older single-game Half-Life 2 config gets its keys moved into games.hl2.
config = localconfig.migrate(json.loads(p.read_text()) if p.exists() else {})
# Host libraries: the prebuilt bundle's prefix, or none for Homebrew (tools/localconfig.py).
if host_prefix: config['host_prefix'] = host_prefix
else: config.pop('host_prefix', None)
# Metal renderer: recorded when built here; P2_NO_METAL=1 removes it; a path
# that still exists (set by hand, or by an earlier run) is otherwise kept, and
# a stale one is dropped (tools/build_ios.py refuses a missing library instead
# of falling back to OpenGL).
def existing(value):
    return bool(value) and Path(str(value)).expanduser().is_file()
if metal_off == '1':
    config.pop('d3d9metal_dylib', None); config.pop('d3d9metal_ios', None)
else:
    if metal: config['d3d9metal_dylib'] = metal
    elif config.get('d3d9metal_dylib') and not existing(config['d3d9metal_dylib']):
        config.pop('d3d9metal_dylib')
    ios = config.get('d3d9metal_ios') if isinstance(config.get('d3d9metal_ios'), dict) else {}
    ios = {target: path for target, path in ios.items() if existing(path)}
    if metal_ios: ios['device'] = metal_ios
    if ios: config['d3d9metal_ios'] = ios
    else: config.pop('d3d9metal_ios', None)
# Each game: its folders, Valve library and Steam ids (tools/games.py).
for i in range(0, len(game_args), 4):
    name, game, linux, dylib = game_args[i:i + 4]
    known = bool(localconfig.game_config(config, name).get('game_root'))
    sections = config.setdefault('games', {})
    if name == games.DEFAULT and name not in sections:
        target = config   # Portal 2: the top-level keys, unchanged since the first release
    else:
        target = sections.setdefault(name, {})
    target.update(game_root=os.path.normpath(game), linux_root=os.path.normpath(linux),
                  steam_api_dylib=os.path.normpath(dylib))
    target.setdefault('steam_appid', games.GAMES[name]['steam_appid'])
    target.setdefault('depot_id', games.GAMES[name]['depot_id'])
    if not known:   # each game its own folder (tools/localconfig.py)
        target.setdefault('user_dir', str(localconfig.support_dir({'game': name}) / 'userdata'))
if team and not config.get('ios_bundle_id'):
    # Portal 2's iPhone app. Unique to the team: free accounts cannot reuse an
    # identifier taken elsewhere.
    config['ios_bundle_id'] = f'local.portal2recomp.{team.lower()}'
# Sections last: runtime/steam.c reads top-level keys only, but older builds
# took the first match in the file.
sections = config.pop('games', None)
if sections: config['games'] = sections
p.write_text(json.dumps(config, indent=2) + '\n')
for key, value in config.items():
    print(f'{key}: {value}')
EOF

# --- macOS build ----------------------------------------------------------
cd "$REPO"
APP_DIR="${P2_APP_DIR:-$HOME/Applications}"
for id in "${SELECTED[@]}"; do
    game_info "$id"
    MINUTES=10; [ "$id" = hl2 ] && MINUTES=20
    step "Preparing the Linux modules of $TITLE"
    python3 tools/gameprep.py --game "$id"
    step "Recompiling $TITLE (about $MINUTES minutes the first time, seconds afterwards)"
    python3 tools/build_game.py --game "$id" --release
    step "Creating $APP_DIR/$APP_NAME.app"
    python3 tools/make_app.py --game "$id" --dest "$APP_DIR"
done

# --- iOS ------------------------------------------------------------------
if [ $IOS = 1 ]; then
    IOS_DEPS="$DEPS/ios-deps"
    [ -f "$REPO/../work/ios-deps/device/lib/libgl4es.a" ] && [ -z "${P2_DEPS:-}" ] && IOS_DEPS="$REPO/../work/ios-deps"
    if [ ! -f "$IOS_DEPS/device/lib/libgl4es.a" ]; then
        step "Building SDL2, FreeType and gl4es for iOS (a few minutes)"
        P2_IOS_DEPS="$IOS_DEPS" ios/deps/build.sh device
    fi
    BUNDLE_ID=$(python3 tools/ios_bundle_id.py)
    DEVICE=$(xcrun devicectl list devices 2>/dev/null |
        awk '/physical/ && / connected|available \(paired\)/ {for (i = 1; i <= NF; ++i) if ($i ~ /^[0-9A-F]{8}-[0-9A-F]{16}$/) {print $i; exit}}')
    step "Getting a provisioning profile for $BUNDLE_ID"
    [ -n "$DEVICE" ] || warn "no iPhone connected: a free account needs one registered for its first profile"
    (cd ios/signing && P2_BUNDLE_ID="$BUNDLE_ID" xcodegen -q &&
        xcodebuild -quiet -project P2Signing.xcodeproj -scheme Portal2 -destination generic/platform=iOS \
            -allowProvisioningUpdates DEVELOPMENT_TEAM="$TEAM" build >/dev/null) \
        || die "Xcode could not create a profile. Connect and unlock the iPhone, enable Developer Mode
  (Settings > Privacy & Security), trust this Mac, then re-run."
    step "Building the iPhone app (about 10 minutes the first time)"
    python3 tools/build_ios.py --game portal2 --target device --release --deps "$IOS_DEPS/device"
    if [ -n "$DEVICE" ]; then
        step "Installing on the iPhone"
        P2_DEVICE="$DEVICE" tools/ios_device.sh install
        # The marker lives on the iPhone: a reinstalled app has an empty container.
        # Older setups only left a local marker; trust it if the game folder is there.
        OLD_MARK="build/ios/device/data-copied-$DEVICE"
        if P2_DEVICE="$DEVICE" tools/ios_device.sh has-data; then :
        elif [ -f "$OLD_MARK" ] && ! P2_DEVICE="$DEVICE" tools/ios_device.sh has-mark &&
             P2_DEVICE="$DEVICE" tools/ios_device.sh has-game; then :
        else
            step "Copying the game data to the iPhone (about 11.5 GB; long, once)"
            P2_DEVICE="$DEVICE" tools/ios_device.sh data
        fi
    fi
fi

D3D9METAL="${D3D9METAL:-$(config_get d3d9metal_dylib)}"
if [ -n "$D3D9METAL" ]; then RENDERER="Metal (libd3d9metal); P2_RENDERER=opengl forces OpenGL"
elif [ $METAL_OFF = 1 ]; then RENDERER="OpenGL (P2_NO_METAL set)"
else RENDERER="OpenGL (libd3d9metal unavailable: see the warning above)"; fi
printf '\nDone:\n'
for id in "${SELECTED[@]}"; do
    game_info "$id"
    SAVES_DIR="$(config_get "$id" user_dir)"
    SAVES_DIR="${SAVES_DIR:-$HOME/Library/Application Support/$APP_NAME/userdata}"
    cat <<EOF
- $APP_DIR/$APP_NAME.app
  Start Steam and sign in with the account that owns $TITLE, then open the app
  (it starts Steam itself if needed). Log: ~/Library/Logs/$LOG_NAME
  Saves and settings: $SAVES_DIR
EOF
done
[ -n "$SKIPPED_NOTE" ] && printf -- '- Not built: %s\n' "$SKIPPED_NOTE"
cat <<EOF
- Renderer: ${RENDERER}
- Each app carries its own copy of the game binary, libraries and prepared
  modules; after a rebuild, run python3 tools/make_app.py [--game hl2] to update it.
  The game files stay where Steam put them.
- Keep the builds to yourself: they are made from Valve's games.
- To remove the apps (and, if you confirm, the saves): ./uninstall.sh
EOF
if [ $IOS = 1 ]; then
    # Same lookup as tools/build_ios.py: the library it linked, if any.
    METAL_IOS_LIB=$(python3 -c 'import json,sys; e=json.load(open(sys.argv[1])).get("d3d9metal_ios"); print(e.get("device") or "" if isinstance(e, dict) else "")' "$CONFIG")
    if [ -n "$METAL_IOS_LIB" ]; then IOS_RENDERER="Metal (libd3d9metal)"
    elif [ $METAL_OFF = 1 ]; then IOS_RENDERER="OpenGL (P2_NO_METAL set)"
    elif ! command -v cargo >/dev/null 2>&1; then IOS_RENDERER="OpenGL (install Rust from https://rustup.rs and re-run for Metal)"
    elif ! command -v rustup >/dev/null 2>&1; then IOS_RENDERER="OpenGL (Metal needs Rust from https://rustup.rs: Homebrew's rust has no iOS target)"
    else IOS_RENDERER="OpenGL (libd3d9metal for iOS did not build: see the warning above)"; fi
    echo "- iPhone renderer: ${IOS_RENDERER}"
    if [ -n "$DEVICE" ]; then
        cat <<EOF
- iPhone: on the phone, Settings > General > VPN & Device Management, trust your
  developer account, then open Portal 2. Options are in Settings > Portal 2.
  A free account's profile lasts 7 days: re-run ./setup.sh --ios to renew it.
EOF
    else
        echo "- iPhone app built in build/ios/device/Portal2.app; connect the phone and run:"
        echo "  tools/ios_device.sh install && tools/ios_device.sh data"
    fi
fi
