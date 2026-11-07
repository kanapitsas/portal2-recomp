#!/bin/bash
# One-shot setup and build: checks the tools, downloads Valve's macOS
# libsteam_api.dylib (inside Steamworks.NET 20.1.0), writes config/local.json,
# recompiles the game and creates "Portal 2 Natif.app" in ~/Applications.
# With --ios, also builds, signs and installs the iPhone app.
# Host libraries (SDL2, FreeType, Fontconfig) come from Homebrew when it has
# them all, else from a prebuilt bundle downloaded from this project's GitHub
# releases (SHA-256 checked; tools/build_host_deps.sh makes it).
# P2_HOST_DEPS=homebrew or bundle forces one (P2_HOST_DEPS_URL: another copy). The Metal renderer's libd3d9metal
# is built when Rust (https://rustup.rs) is installed, else taken prebuilt from
# the same bundle. P2_NO_METAL=1 skips it: the game then uses OpenGL.
#
# usage: ./setup.sh [--ios] ["/path/to/Portal 2" ["/path/to/depot_661"]]
#        ./setup.sh --game hl2 ["/path/to/Half-Life 2" ["/path/to/depot_224"]]
#
# Without paths, the script reuses config/local.json from a previous run, or
# looks where Steam puts them on this Mac (a Portal 2 install, or depots
# fetched with the Steam console's download_depot command).
#
# P2_APP_DIR puts the app elsewhere than ~/Applications.
# --game hl2 builds Half-Life 2 instead (experimental, Mac only, docs/hl2.md):
# "Half-Life 2 Natif.app", with Steamworks.NET 2024.8.0's libsteam_api.dylib.
# One checkout builds one game (build/ is shared): clone the repository a
# second time to have both. Later runs remember the game.
#
# Re-running is safe: finished steps are skipped and unchanged code is not
# recompiled. Downloads and iOS libraries go to ./deps (P2_DEPS overrides it).
# Saves go to ~/Library/Application Support/Portal 2 Natif/userdata (config key
# user_dir). ./uninstall.sh removes the app and, if asked, the saves.
set -euo pipefail

REPO="$(cd "$(dirname "$0")" && pwd)"
DEPS="${P2_DEPS:-$REPO/deps}"
CONFIG="$REPO/config/local.json"
IOS=0
GAME_ID=""
ARGS=()
while [ $# -gt 0 ]; do
    a="$1"; shift
    case "$a" in
        --ios) IOS=1 ;;
        --game) [ $# -gt 0 ] || { echo "--game needs a value: portal2 or hl2" >&2; exit 2; }
                GAME_ID="$1"; shift ;;
        --game=*) GAME_ID="${a#--game=}" ;;
        -h|--help) awk 'NR > 1 && !/^#/ {exit} NR > 1 {sub(/^# ?/, ""); print}' "$0"; exit 0 ;;  # leading comment block
        -*) echo "unknown option $a" >&2; exit 2 ;;
        *) ARGS+=("$a") ;;
    esac
done
GAME="${ARGS[0]:-}"
LINUX="${ARGS[1]:-}"

# Which game: --game, else the one this checkout was set up for, else Portal 2.
# Never switch a configured checkout: its build/ and app belong to the other game.
CONFIGURED_GAME=""
[ -f "$CONFIG" ] && CONFIGURED_GAME="$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1])).get("game") or "portal2")' "$CONFIG" 2>/dev/null || true)"
GAME_ID="${GAME_ID:-${CONFIGURED_GAME:-portal2}}"
# Per game: title, Steam app and depots, app name and log (tools/games.py), and
# the Steamworks.NET release whose libsteam_api.dylib has the SDK the game asks for.
case "$GAME_ID" in
    portal2)
        TITLE="Portal 2"; APPID=620; LINUX_DEPOT=661
        STEAMWORKS_VERSION=20.1.0
        STEAMWORKS_SHA256="3be79e6bdf53c86210267bf39f46362bb839130896006aec26364c5efc37e314" ;;
    hl2)
        TITLE="Half-Life 2"; APPID=220; LINUX_DEPOT=224
        STEAMWORKS_VERSION=2024.8.0   # SDK 1.60: HL2's client.so asks for newer interfaces (docs/steam.md)
        STEAMWORKS_SHA256="0a8e9b8c95760e81fcb16477a8962a4c230ea887b3731b1f9bc0690677f6f087" ;;
    *) echo "unknown game $GAME_ID: use --game portal2 or --game hl2" >&2; exit 2 ;;
esac
if [ -n "$CONFIGURED_GAME" ] && [ "$CONFIGURED_GAME" != "$GAME_ID" ]; then
    echo "error: this checkout is set up for $CONFIGURED_GAME (config/local.json), not $GAME_ID.
  One checkout builds one game: clone the repository again into another folder
  and run ./setup.sh --game $GAME_ID there." >&2
    exit 2
fi
APP_NAME="$TITLE Natif"
LOG_NAME="$(printf '%s' "$TITLE" | tr -d ' -')Natif.log"
STEAMWORKS_URL="https://github.com/rlabrecque/Steamworks.NET/releases/download/$STEAMWORKS_VERSION/Steamworks.NET_$STEAMWORKS_VERSION.unitypackage"
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
config_get() {  # key -> value from config/local.json, empty if absent
    [ -f "$CONFIG" ] || return 0
    python3 -c 'import json,sys; print(json.load(open(sys.argv[1])).get(sys.argv[2]) or "")' "$CONFIG" "$1"
}
find_depot() {  # depot id -> folder written by the Steam console's download_depot
    [ -d "$STEAM_SUPPORT" ] && [ -n "$1" ] || return 0
    find "$STEAM_SUPPORT" -maxdepth 9 -type d -path "*/content/app_${APPID}/depot_$1" 2>/dev/null | head -1
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
step "Finding the game"
# A path dragged into Terminal comes with "\ " escapes and a trailing space; inside
# quotes they reach us verbatim. Undo that when the path does not exist as typed.
unescape_path() {
    local p="$1"
    while [ "${p% }" != "$p" ]; do p="${p% }"; done
    [ -e "$p" ] || p="$(printf '%s' "$p" | sed 's/\\\(.\)/\1/g')"
    printf '%s' "$p"
}
[ -n "$GAME" ] && GAME="$(unescape_path "$GAME")"
[ -n "$LINUX" ] && LINUX="$(unescape_path "$LINUX")"
if [ "$GAME_ID" = portal2 ]; then
    if [ -z "$GAME" ]; then
        for c in "$(config_get game_root)" "$STEAM_SUPPORT/steamapps/common/Portal 2" "$(find_depot 621)"; do
            [ -n "$c" ] && [ -d "$c/portal2" ] && { GAME="$c"; break; }
        done
    fi
    [ -n "$GAME" ] || die "Portal 2 data not found. Pass the game folder (the one with portal2/ and platform/), or fetch it:
  open steam://open/console, type: download_depot 620 621   (12.6 GB), then re-run."
    [ -f "$GAME/portal2/pak01_dir.vpk" ] && [ -d "$GAME/platform" ] \
        || die "$GAME does not look like Portal 2 (no portal2/pak01_dir.vpk or platform/)"
    for f in mssmp3.asi mssvoice.asi binkawin.asi; do
        [ -f "$GAME/bin/linux32/$f" ] || warn "$GAME/bin/linux32/$f is missing: some audio (music, voice) may be silent"
    done
    if [ -z "$LINUX" ]; then
        for c in "$(config_get linux_root)" "$(find_depot 661)"; do
            [ -n "$c" ] && [ -f "$c/bin/linux32/engine.so" ] && { LINUX="$c"; break; }
        done
    fi
    [ -n "$LINUX" ] || die "Portal 2's Linux binaries not found. Fetch them with your Steam account:
  open steam://open/console, type: download_depot 620 661   (about 90 MB), wait for
  'Depot download complete', then re-run (or pass that folder as the second argument)."
    [ -f "$LINUX/bin/linux32/engine.so" ] && [ -f "$LINUX/portal2/bin/linux32/server.so" ] \
        || die "$LINUX does not look like depot 661 (no bin/linux32/engine.so or portal2/bin/linux32/server.so)"
else   # hl2: the Mac game from Steam, the Linux binaries from depot 224
    if [ -z "$GAME" ]; then
        for c in "$(config_get game_root)" "$STEAM_SUPPORT/steamapps/common/Half-Life 2"; do
            [ -n "$c" ] && [ -d "$c/hl2_complete" ] && { GAME="$c"; break; }
        done
    fi
    [ -n "$GAME" ] || die "Half-Life 2 not found. Install it with Steam (the current version, with hl2_complete/),
  or pass the game folder (the one with hl2/, hl2_complete/ and platform/)."
    [ -f "$GAME/hl2/hl2_pak_dir.vpk" ] && [ -f "$GAME/hl2_complete/gameinfo.txt" ] && [ -d "$GAME/platform" ] \
        || die "$GAME does not look like Half-Life 2 (no hl2/hl2_pak_dir.vpk, hl2_complete/gameinfo.txt or platform/).
  Only the current Steam version (20th anniversary update, with hl2_complete/) is supported."
    if [ -z "$LINUX" ]; then
        for c in "$(config_get linux_root)" "$(find_depot 224)"; do
            [ -n "$c" ] && [ -f "$c/hl2_linux" ] && { LINUX="$c"; break; }
        done
    fi
    [ -n "$LINUX" ] || die "Half-Life 2's Linux binaries not found. Fetch them with your Steam account:
  open steam://open/console, type: download_depot 220 224   (about 320 MB), wait for
  'Depot download complete', then re-run (or pass that folder as the second argument)."
    [ -f "$LINUX/hl2_linux" ] && [ -f "$LINUX/bin/engine.so" ] && [ -f "$LINUX/hl2_complete/bin/server.so" ] \
        || die "$LINUX does not look like depot 224 (no hl2_linux, bin/engine.so or hl2_complete/bin/server.so)"
    for f in mssmp3.asi binkawin.asi mssmixer.mix; do
        [ -f "$LINUX/bin/linux32/$f" ] || warn "$LINUX/bin/linux32/$f is missing: some audio may be silent"
    done
fi
echo "game data:      $GAME"
echo "Linux binaries: $LINUX"

# --- Steamworks.NET (20.1.0, 2024.8.0 for HL2): steam_api.bundle ----------
DYLIB="$(config_get steam_api_dylib)"
if [ "$GAME_ID" = portal2 ]; then
    DEFAULT_DYLIB="$REPO/../work/downloads/steamworks/steam_api.bundle/Contents/MacOS/libsteam_api.dylib"
    STEAM_BUNDLE="$DEPS/steam_api.bundle"
else
    DEFAULT_DYLIB="$REPO/../work/downloads/steamworks-$STEAMWORKS_VERSION/steam_api.bundle/Contents/MacOS/libsteam_api.dylib"
    STEAM_BUNDLE="$DEPS/steamworks-$STEAMWORKS_VERSION/steam_api.bundle"
fi
[ -z "$DYLIB" ] && [ -f "$DEFAULT_DYLIB" ] && DYLIB="$DEFAULT_DYLIB"
if [ -z "$DYLIB" ] || [ ! -f "$DYLIB" ]; then
    DYLIB="$STEAM_BUNDLE/Contents/MacOS/libsteam_api.dylib"
    if [ ! -f "$DYLIB" ]; then
        step "Downloading Steamworks.NET $STEAMWORKS_VERSION (Valve's libsteam_api.dylib)"
        mkdir -p "$DEPS"
        SW="$DEPS/Steamworks.NET_$STEAMWORKS_VERSION.unitypackage"
        [ -f "$SW" ] || curl -fsSL --retry 3 -o "$SW" "$STEAMWORKS_URL"
        got=$(shasum -a 256 "$SW" | cut -d' ' -f1)
        [ "$got" = "$STEAMWORKS_SHA256" ] || die "$SW: SHA-256 $got, expected $STEAMWORKS_SHA256 (delete it and re-run)"
        T=$(mktemp -d)
        tar -xzf "$SW" -C "$T"
        rm -rf "$STEAM_BUNDLE"
        for d in "$T"/*; do
            [ -f "$d/asset" ] && [ -f "$d/pathname" ] || continue
            p=$(head -1 "$d/pathname")
            case "$p" in
                */Plugins/steam_api.bundle/*)
                    rel="${p#*/Plugins/steam_api.bundle/}"
                    mkdir -p "$STEAM_BUNDLE/$(dirname "$rel")"
                    cp "$d/asset" "$STEAM_BUNDLE/$rel" ;;
            esac
        done
        rm -rf "$T"
        [ -f "$DYLIB" ] || die "steam_api.bundle not found in the Steamworks.NET package"
    fi
fi

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
step "Writing config/local.json"
mkdir -p "$REPO/config"
export P2_SETUP_GAME="$GAME_ID" P2_SETUP_APPID="$APPID" P2_SETUP_DEPOT="$LINUX_DEPOT" P2_SETUP_APP_NAME="$APP_NAME"
python3 - "$CONFIG" "$GAME" "$LINUX" "$DYLIB" "$TEAM" "$D3D9METAL" "$D3D9METAL_IOS" "$METAL_OFF" "$HOST_PREFIX" <<'EOF'
import json, os, sys
from pathlib import Path
path, game, linux, dylib, team, metal, metal_ios, metal_off, host_prefix = sys.argv[1:]
p = Path(path)
config = json.loads(p.read_text()) if p.exists() else {}
fresh = not config
config.update(game_root=os.path.normpath(game), linux_root=os.path.normpath(linux), steam_api_dylib=os.path.normpath(dylib))
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
# The game (tools/games.py) and its Steam ids, from the case at the top of setup.sh.
config['game'] = os.environ['P2_SETUP_GAME']
config.setdefault('steam_appid', int(os.environ['P2_SETUP_APPID']))
config.setdefault('depot_id', int(os.environ['P2_SETUP_DEPOT']))
if fresh:   # each game its own folder (tools/localconfig.py)
    config['user_dir'] = str(Path.home() / 'Library/Application Support' / os.environ['P2_SETUP_APP_NAME'] / 'userdata')
if team and not config.get('ios_bundle_id'):
    # Unique to the team: free accounts cannot reuse an identifier taken elsewhere.
    # Portal 2 keeps its identifier; another game gets its own, so that it does
    # not replace Portal 2's app on the phone.
    game_id = os.environ['P2_SETUP_GAME']
    prefix = 'portal2recomp' if game_id == 'portal2' else f'{game_id}recomp'
    config['ios_bundle_id'] = f'local.{prefix}.{team.lower()}'
p.write_text(json.dumps(config, indent=2) + '\n')
for key, value in config.items():
    print(f'{key}: {value}')
EOF

# --- macOS build ----------------------------------------------------------
cd "$REPO"
step "Preparing the Linux modules"
python3 tools/gameprep.py
step "Recompiling the game (about 10 minutes the first time, seconds afterwards)"
python3 tools/build_game.py --release
APP_DIR="${P2_APP_DIR:-$HOME/Applications}"
step "Creating $APP_DIR/$APP_NAME.app"
python3 tools/make_app.py --dest "$APP_DIR"

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
    python3 tools/build_ios.py --target device --release --deps "$IOS_DEPS/device"
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

SAVES_DIR="$(config_get user_dir)"
SAVES_DIR="${SAVES_DIR:-$HOME/Library/Application Support/$APP_NAME/userdata}"
D3D9METAL="${D3D9METAL:-$(config_get d3d9metal_dylib)}"
if [ -n "$D3D9METAL" ]; then RENDERER="Metal (libd3d9metal); P2_RENDERER=opengl forces OpenGL"
elif [ $METAL_OFF = 1 ]; then RENDERER="OpenGL (P2_NO_METAL set)"
else RENDERER="OpenGL (libd3d9metal unavailable: see the warning above)"; fi
cat <<EOF

Done: $APP_DIR/$APP_NAME.app
- Renderer: ${RENDERER}
- Start Steam and sign in with the account that owns $TITLE, then open the app
  (it starts Steam itself if needed). Log: ~/Library/Logs/$LOG_NAME
- The app carries its own copy of the game binary and libraries; after a rebuild,
  run python3 tools/make_app.py to update it.
- Keep the build to yourself: it is made from Valve's game.
- The app carries its own prepared modules; saves and settings are in
  ${SAVES_DIR}. The game files stay where Steam put them.
  To remove the app (and, if you confirm, the saves): ./uninstall.sh
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
