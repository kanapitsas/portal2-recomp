#!/bin/bash
# One-shot setup and build: checks the tools, downloads Valve's macOS
# libsteam_api.dylib (inside Steamworks.NET 20.1.0), writes config/local.json,
# recompiles the game and creates "Portal 2 Natif.app" in ~/Applications.
# With --ios, also builds, signs and installs the iPhone app.
# If Rust is installed (https://rustup.rs), it also builds libd3d9metal for the
# Metal renderer; without it the game uses OpenGL. P2_NO_METAL=1 skips it.
#
# usage: ./setup.sh [--ios] ["/path/to/Portal 2" ["/path/to/depot_661"]]
#
# Without paths, the script reuses config/local.json from a previous run, or
# looks where Steam puts them on this Mac (a Portal 2 install, or depots
# fetched with the Steam console's download_depot command).
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
ARGS=()
for a in "$@"; do
    case "$a" in
        --ios) IOS=1 ;;
        -h|--help) sed -n '2,19p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
        -*) echo "unknown option $a" >&2; exit 2 ;;
        *) ARGS+=("$a") ;;
    esac
done
GAME="${ARGS[0]:-}"
LINUX="${ARGS[1]:-}"

STEAMWORKS_URL="https://github.com/rlabrecque/Steamworks.NET/releases/download/20.1.0/Steamworks.NET_20.1.0.unitypackage"
STEAMWORKS_SHA256="3be79e6bdf53c86210267bf39f46362bb839130896006aec26364c5efc37e314"
STEAM_SUPPORT="$HOME/Library/Application Support/Steam"
# Metal renderer: libd3d9metal, built from mtld3d's native host branch (zlib).
MTLD3D_URL="https://github.com/kanapitsas/mtld3d.git"
MTLD3D_REV="101f37d3d45c760e95e20176f758b9d309c23885"   # feat/native-host, pinned

step() { printf '\n==> %s\n' "$*"; }
die() { printf 'error: %s\n' "$*" >&2; exit 1; }
warn() { printf 'warning: %s\n' "$*" >&2; }
need() { command -v "$1" >/dev/null 2>&1 || die "$1 not found. $2"; }
config_get() {  # key -> value from config/local.json, empty if absent
    [ -f "$CONFIG" ] || return 0
    python3 -c 'import json,sys; print(json.load(open(sys.argv[1])).get(sys.argv[2]) or "")' "$CONFIG" "$1"
}
find_depot() {  # depot id -> folder written by the Steam console's download_depot
    [ -d "$STEAM_SUPPORT" ] || return 0
    find "$STEAM_SUPPORT" -maxdepth 9 -type d -path "*/content/app_620/depot_$1" 2>/dev/null | head -1
}

# --- Checks ---------------------------------------------------------------
[ "$(uname -s)" = Darwin ] && [ "$(uname -m)" = arm64 ] || die "this builds for Apple silicon: run it on an arm64 Mac"
step "Checking tools"
xcode-select -p >/dev/null 2>&1 || die "Xcode command line tools missing: xcode-select --install"
need python3 "Install Python 3, e.g. brew install python"
python3 -c 'import sys; sys.exit(sys.version_info < (3, 9))' || die "Python 3.9 or newer is required"
need objdump "It comes with the Xcode command line tools: xcode-select --install"
need curl ""
BREW_PREFIX=/opt/homebrew
for lib in SDL2 freetype fontconfig; do
    ls "$BREW_PREFIX/lib/lib$lib".* >/dev/null 2>&1 || die "lib$lib not found in $BREW_PREFIX/lib: brew install sdl2 freetype fontconfig"
done
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
echo "game data:      $GAME"
echo "Linux binaries: $LINUX"

# --- Steamworks.NET 20.1.0: steam_api.bundle ------------------------------
DYLIB="$(config_get steam_api_dylib)"
DEFAULT_DYLIB="$REPO/../work/downloads/steamworks/steam_api.bundle/Contents/MacOS/libsteam_api.dylib"
[ -z "$DYLIB" ] && [ -f "$DEFAULT_DYLIB" ] && DYLIB="$DEFAULT_DYLIB"
if [ -z "$DYLIB" ] || [ ! -f "$DYLIB" ]; then
    STEAM_BUNDLE="$DEPS/steam_api.bundle"
    DYLIB="$STEAM_BUNDLE/Contents/MacOS/libsteam_api.dylib"
    if [ ! -f "$DYLIB" ]; then
        step "Downloading Steamworks.NET 20.1.0 (Valve's libsteam_api.dylib)"
        mkdir -p "$DEPS"
        SW="$DEPS/Steamworks.NET_20.1.0.unitypackage"
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

# --- Metal renderer: libd3d9metal (optional) -----------------------------
# Built from a pinned commit of mtld3d's native host branch. Any problem here
# only costs the Metal renderer: the game then uses OpenGL.
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
        warn "Rust not found: the game will use OpenGL. For the faster Metal renderer,
  install Rust (https://rustup.rs) and re-run ./setup.sh."
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
if [ -n "${P2_NO_METAL:-}" ] && [ "$P2_NO_METAL" != 0 ]; then
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
python3 - "$CONFIG" "$GAME" "$LINUX" "$DYLIB" "$TEAM" "$D3D9METAL" "$D3D9METAL_IOS" "$METAL_OFF" <<'EOF'
import json, os, sys
from pathlib import Path
path, game, linux, dylib, team, metal, metal_ios, metal_off = sys.argv[1:]
p = Path(path)
config = json.loads(p.read_text()) if p.exists() else {}
fresh = not config
config.update(game_root=os.path.normpath(game), linux_root=os.path.normpath(linux), steam_api_dylib=os.path.normpath(dylib))
# Metal renderer: recorded when built here; P2_NO_METAL=1 removes it; a path
# that still exists (set by hand, or by an earlier run) is otherwise kept.
if metal_off == '1':
    config.pop('d3d9metal_dylib', None); config.pop('d3d9metal_ios', None)
else:
    if metal: config['d3d9metal_dylib'] = metal
    elif config.get('d3d9metal_dylib') and not Path(config['d3d9metal_dylib']).expanduser().is_file():
        config.pop('d3d9metal_dylib')
    if metal_ios:
        ios = config.get('d3d9metal_ios') if isinstance(config.get('d3d9metal_ios'), dict) else {}
        ios['device'] = metal_ios
        config['d3d9metal_ios'] = ios
config.setdefault('steam_appid', 620)
config.setdefault('depot_id', 661)
if fresh:
    config['user_dir'] = str(Path.home() / 'Library/Application Support/Portal 2 Natif/userdata')
if team and not config.get('ios_bundle_id'):
    # Unique to the team: free accounts cannot reuse an identifier taken elsewhere.
    config['ios_bundle_id'] = f'local.portal2recomp.{team.lower()}'
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
step "Creating ~/Applications/Portal 2 Natif.app"
python3 tools/make_app.py

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
        COPIED="build/ios/device/data-copied-$DEVICE"
        if [ ! -f "$COPIED" ]; then
            step "Copying the game data to the iPhone (about 11.5 GB; long, once)"
            P2_DEVICE="$DEVICE" tools/ios_device.sh data
            mkdir -p "$(dirname "$COPIED")" && date > "$COPIED"
        fi
    fi
fi

SAVES_DIR="$(config_get user_dir)"
SAVES_DIR="${SAVES_DIR:-$HOME/Library/Application Support/Portal 2 Natif/userdata}"
D3D9METAL="${D3D9METAL:-$(config_get d3d9metal_dylib)}"
if [ -n "$D3D9METAL" ]; then RENDERER="Metal (libd3d9metal); P2_RENDERER=opengl forces OpenGL"
else RENDERER="OpenGL (install Rust from https://rustup.rs and re-run for Metal)"; fi
cat <<EOF

Done: ~/Applications/Portal 2 Natif.app
- Renderer: ${RENDERER}
- Start Steam and sign in with the account that owns Portal 2, then open the app
  (it starts Steam itself if needed). Log: ~/Library/Logs/Portal2Natif.log
- The app carries its own copy of the game binary and libraries: Homebrew is only
  needed to rebuild. After a rebuild, run python3 tools/make_app.py to update it.
- Keep the build to yourself: it is made from Valve's game.
- The app carries its own prepared modules; saves and settings are in
  ${SAVES_DIR}. The game files stay where Steam put them.
  To remove the app (and, if you confirm, the saves): ./uninstall.sh
EOF
if [ $IOS = 1 ]; then
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
