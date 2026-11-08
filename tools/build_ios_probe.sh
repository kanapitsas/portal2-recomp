#!/bin/bash
# Construit la sonde iOS (ios/probe) : <ios_dir>/<cible>/probe.app du jeu
# (build/ios/<cible>/probe.app, build/hl2/ios/<cible>/probe.app ; tools/ios_game.py).
#   tools/build_ios_probe.sh [--game portal2|hl2] [sim|device]
# Identifier: the game's plus ".probe" (tools/ios_bundle_id.py), the one
# tools/ios_device.sh [--game ...] probe launches.
# Appareil : signée par ios/signing/sign.py (profil obtenu par Xcode).
# Lancement : P2_PROBE_CLIP=1 (test de découpe au plan proche, ES 3 brut).
set -euo pipefail
cd "$(dirname "$0")/.."
GAME_OPT=()
case "${1:-}" in
  --game) GAME_OPT=(--game "${2:?--game needs a value}"); shift 2 ;;
  --game=*) GAME_OPT=(--game "${1#--game=}"); shift ;;
esac
TARGET=${1:-sim}
case "$TARGET" in
  sim)    SDK=iphonesimulator; TRIPLE=arm64-apple-ios17.0-simulator; PLATFORM=ios-simulator; PLIST=iPhoneSimulator ;;
  device) SDK=iphoneos; TRIPLE=arm64-apple-ios17.0; PLATFORM=ios; PLIST=iPhoneOS ;;
  *) echo "usage: $0 [sim|device]"; exit 2 ;;
esac
DEPS=${P2_IOS_DEPS:-$(cd .. && pwd)/work/ios-deps}/$TARGET
# IOS_DIR (<ios_dir>/<cible>) et PROBE_ID du jeu (tools/ios_game.py).
VARS=$(python3 tools/ios_game.py ${GAME_OPT[@]+"${GAME_OPT[@]}"} --target "$TARGET") || exit 2
eval "$VARS"
APP="$IOS_DIR/probe.app"
mkdir -p "$APP"
xcrun --sdk $SDK clang -target $TRIPLE -isysroot "$(xcrun --sdk $SDK --show-sdk-path)" -O1 -DGLES_SILENCE_DEPRECATION \
  -I"$DEPS/include/SDL2" -I"$DEPS/include/gl4es" ios/probe/probe.c ios/probe/clip_test.c \
  "$DEPS/lib/libSDL2main.a" "$DEPS/lib/libSDL2.a" "$DEPS/lib/libgl4es.a" \
  $(for f in UIKit Foundation CoreGraphics QuartzCore OpenGLES Metal AVFoundation AudioToolbox CoreAudio CoreMotion \
             GameController CoreHaptics CoreBluetooth; do echo -framework $f; done) \
  -Wl,-platform_version,$PLATFORM,17.0,18.0 -o "$APP/probe"
sed -e 's/@EXECUTABLE@/probe/' -e "s|@IDENTIFIER@|$PROBE_ID|" -e 's/@NAME@/P2 Probe/' -e 's/@GAME@/probe/' \
    -e "s/@PLATFORM@/$PLIST/" ios/Info.plist.in > "$APP/Info.plist"
if [ "$TARGET" = sim ]; then codesign -s - -f "$APP" >/dev/null 2>&1; else python3 ios/signing/sign.py "$APP"; fi
echo "$APP"
