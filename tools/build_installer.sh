#!/bin/bash
# Builds "Portal 2 Installer.app" (installer/Installer.swift) with a snapshot of
# the committed repository inside, and a zip for a GitHub release:
#   build/installer/Portal 2 Installer.app
#   build/installer/Portal-2-Installer-<version>.zip
# usage: tools/build_installer.sh [version]   (default: git describe)
# Needs only the Xcode command line tools. The snapshot is `git archive HEAD`:
# uncommitted changes are not in it.
set -euo pipefail
REPO="$(cd "$(dirname "$0")/.." && pwd)"
cd "$REPO"
VERSION="${1:-$(git describe --tags --always --dirty)}"
OUT="$REPO/build/installer"
APP="$OUT/Portal 2 Installer.app"
git diff --quiet HEAD -- || echo "warning: uncommitted changes are not in the installer's snapshot" >&2

rm -rf "$APP" && mkdir -p "$APP/Contents/MacOS" "$APP/Contents/Resources"
swiftc -O -parse-as-library -swift-version 5 -target arm64-apple-macos13.0 \
    installer/Installer.swift -o "$APP/Contents/MacOS/installer"
git archive --format=tar.gz -o "$APP/Contents/Resources/source.tar.gz" HEAD
cat >"$APP/Contents/Info.plist" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>CFBundleName</key><string>Portal 2 Installer</string>
  <key>CFBundleDisplayName</key><string>Portal 2 Installer</string>
  <key>CFBundleIdentifier</key><string>io.github.kanapitsas.portal2-recomp.installer</string>
  <key>CFBundleExecutable</key><string>installer</string>
  <key>CFBundlePackageType</key><string>APPL</string>
  <key>CFBundleShortVersionString</key><string>${VERSION#v}</string>
  <key>CFBundleVersion</key><string>${VERSION#v}</string>
  <key>LSMinimumSystemVersion</key><string>13.0</string>
  <key>LSArchitecturePriority</key><array><string>arm64</string></array>
  <key>NSHighResolutionCapable</key><true/>
</dict>
</plist>
EOF
codesign --force -s - "$APP"
ZIP="$OUT/Portal-2-Installer-$VERSION.zip"
rm -f "$ZIP"
(cd "$OUT" && ditto -c -k --keepParent "Portal 2 Installer.app" "$ZIP")
shasum -a 256 "$ZIP"
du -h "$ZIP"
