#!/bin/bash
# Builds "Natif Installer.app" (installer/Installer.swift; Portal 2 and
# Half-Life 2) with a snapshot of the committed repository inside, and a zip
# for a GitHub release:
#   build/installer/Natif Installer.app
#   build/installer/Natif-Installer-<version>.zip
# The name is set once here (INSTALLER_NAME); the bundle identifier stays the
# first release's.
# usage: tools/build_installer.sh [version]   (default: git describe)
# Needs only the Xcode command line tools. The snapshot is `git archive HEAD`:
# uncommitted changes are not in it.
#
# Signing: with a "Developer ID Application" certificate in the keychain (or
# P2_SIGN_IDENTITY naming one), the app is signed with the hardened runtime
# and a timestamp; with a notarytool profile too (P2_NOTARY_PROFILE, default
# p2-notary, made once by `xcrun notarytool store-credentials`), the zip is
# notarized and the app stapled, so Gatekeeper opens it without a warning.
# Without a certificate: ad hoc signature, users click Open Anyway once.
# P2_SIGN_IDENTITY=- forces ad hoc. See docs/usage.md#releases.
set -euo pipefail
REPO="$(cd "$(dirname "$0")/.." && pwd)"
cd "$REPO"
VERSION="${1:-$(git describe --tags --always --dirty)}"
# Bundle versions must be numbers (0.2.0); the full description goes in P2SourceVersion.
NUMERIC=$(printf '%s' "${VERSION#v}" | sed -E 's/^([0-9]+(\.[0-9]+){0,2}).*/\1/')
case "$NUMERIC" in [0-9]*) ;; *) NUMERIC=0.0.0 ;; esac
IDENTITY="${P2_SIGN_IDENTITY:-$(security find-identity -v -p codesigning 2>/dev/null \
    | sed -n 's/.*"\(Developer ID Application: [^"]*\)".*/\1/p' | head -1)}"
IDENTITY="${IDENTITY:--}"
PROFILE="${P2_NOTARY_PROFILE:-p2-notary}"
OUT="$REPO/build/installer"
INSTALLER_NAME="Natif Installer"
APP="$OUT/$INSTALLER_NAME.app"
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
  <key>CFBundleName</key><string>$INSTALLER_NAME</string>
  <key>CFBundleDisplayName</key><string>$INSTALLER_NAME</string>
  <key>CFBundleIdentifier</key><string>io.github.kanapitsas.portal2-recomp.installer</string>
  <key>CFBundleExecutable</key><string>installer</string>
  <key>CFBundlePackageType</key><string>APPL</string>
  <key>CFBundleShortVersionString</key><string>$NUMERIC</string>
  <key>CFBundleVersion</key><string>$NUMERIC</string>
  <key>P2SourceVersion</key><string>$VERSION</string>
  <key>LSMinimumSystemVersion</key><string>13.0</string>
  <key>LSArchitecturePriority</key><array><string>arm64</string></array>
  <key>NSHighResolutionCapable</key><true/>
</dict>
</plist>
EOF
ZIP="$OUT/${INSTALLER_NAME// /-}-$VERSION.zip"
pack() { rm -f "$ZIP" && (cd "$OUT" && ditto -c -k --keepParent "$INSTALLER_NAME.app" "$ZIP"); }
if [ "$IDENTITY" = - ]; then
    echo "signing: ad hoc (no Developer ID Application certificate): users will need Open Anyway"
    codesign --force -s - "$APP"
    pack
else
    echo "signing: $IDENTITY"
    codesign --force --options runtime --timestamp -s "$IDENTITY" "$APP"
    codesign --verify --strict --verbose=1 "$APP"
    pack
    if xcrun notarytool history --keychain-profile "$PROFILE" >/dev/null 2>&1; then
        echo "notarizing with profile $PROFILE (a few minutes)"
        xcrun notarytool submit "$ZIP" --keychain-profile "$PROFILE" --wait
        xcrun stapler staple "$APP"
        pack   # again, with the ticket inside
        spctl --assess --type execute --verbose=2 "$APP"
    else
        echo "warning: no notarytool profile $PROFILE: signed but not notarized, Gatekeeper will still warn.
  Make it once: xcrun notarytool store-credentials $PROFILE --apple-id <Apple ID> --team-id <team ID>" >&2
    fi
fi
shasum -a 256 "$ZIP"
du -h "$ZIP"
