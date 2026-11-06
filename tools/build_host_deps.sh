#!/bin/bash
# Builds the prebuilt host libraries that ./setup.sh downloads when Homebrew's
# are missing (or Rust is, for the Metal renderer). Maintainers run this; users
# never need it. The result is a relocatable prefix:
#   include/  SDL2 (sdl2-compat), SDL3, freetype2, fontconfig headers
#   lib/      libSDL2, libSDL3, libpng16, libfreetype, libfontconfig, libd3d9metal
#             (install names @rpath/<name>, ad hoc signed)
#   etc/fonts Fontconfig configuration (the app points FONTCONFIG_PATH at it)
#   licenses/ one file per library
#   manifest.json  versions, mtld3d commit, minimum macOS
# packed as p2-host-deps-<version>-macos-arm64.tar.xz with its SHA-256.
#
# usage: tools/build_host_deps.sh [work dir]     (default ../work/host-deps)
# Needs: Xcode command line tools, cmake, ninja, pkg-config, Rust 1.99+;
# meson is installed in a venv inside the work dir.
set -euo pipefail

REPO="$(cd "$(dirname "$0")/.." && pwd)"
WORK="${1:-$REPO/../work/host-deps}"
# Bump when anything below changes, then update HOST_DEPS_* in setup.sh.
BUNDLE_VERSION=1
MIN_MACOS=13.0
# Same sources and checksums as Homebrew's formulae (the versions tested on Mac).
SDL3_URL=https://github.com/libsdl-org/SDL/releases/download/release-3.4.20/SDL3-3.4.20.tar.gz
SDL3_SHA=f3bb06f55bed9557227a700af2ea62e49497ebdf58037732a731a5fb3ef8581f
SDL2C_URL=https://github.com/libsdl-org/sdl2-compat/releases/download/release-2.32.74/sdl2-compat-2.32.74.tar.gz
SDL2C_SHA=ec68abde77e2e459c8abc4f9587b8976b5d2a657b2bfda2fb3a079b0f8924588
PNG_URL=https://downloads.sourceforge.net/project/libpng/libpng16/1.6.59/libpng-1.6.59.tar.xz
PNG_SHA=d80dd2a38a37f803cb9b6ac7b14bd6e74ddc3b654780a8380bdf93523fdb4389
FT_URL=https://downloads.sourceforge.net/project/freetype/freetype2/2.15.0/freetype-2.15.0.tar.xz
FT_SHA=93cd47eb719c325107f2d3cf087d06299e944c3b48a40b55537302304dea1dd5
FC_URL=https://gitlab.freedesktop.org/fontconfig/fontconfig/-/archive/2.18.3/fontconfig-2.18.3.tar.gz
FC_SHA=9ae01e1d53acdef56010c5451cd34aa41d325b2faccd8606448d8fa01b2496b3
# The mtld3d commit is setup.sh's, so a prebuilt and a local build match.
MTLD3D_URL=$(sed -n 's/^MTLD3D_URL="\(.*\)"$/\1/p' "$REPO/setup.sh")
MTLD3D_REV=$(sed -n 's/^MTLD3D_REV="\([0-9a-f]*\)".*/\1/p' "$REPO/setup.sh")
[ -n "$MTLD3D_URL" ] && [ -n "$MTLD3D_REV" ] || { echo "MTLD3D_URL/MTLD3D_REV not found in setup.sh" >&2; exit 1; }

for t in cmake ninja pkg-config cargo gperf; do
    command -v $t >/dev/null || { echo "$t not found" >&2; exit 1; }
done
mkdir -p "$WORK/src"
WORK="$(cd "$WORK" && pwd)"
NAME="p2-host-deps-$BUNDLE_VERSION-macos-arm64"
PREFIX="$WORK/$NAME"
rm -rf "$PREFIX" "$WORK/build"
mkdir -p "$PREFIX"/{lib,include,licenses} "$WORK/build"
export MACOSX_DEPLOYMENT_TARGET=$MIN_MACOS

fetch() {  # dir url sha256
    local dir=$1 url=$2 sha=$3 file="$WORK/src/$(basename "$2")"
    [ -f "$file" ] || curl -fsSL "$url" -o "$file"
    echo "$sha  $file" | shasum -a 256 -c --quiet - || { rm -f "$file"; exit 1; }
    rm -rf "$WORK/src/$dir" && mkdir -p "$WORK/src/$dir"
    tar -xf "$file" -C "$WORK/src/$dir" --strip-components=1
}
fetch SDL3 $SDL3_URL $SDL3_SHA
fetch sdl2-compat $SDL2C_URL $SDL2C_SHA
fetch libpng $PNG_URL $PNG_SHA
fetch freetype $FT_URL $FT_SHA
fetch fontconfig $FC_URL $FC_SHA

# Nothing from Homebrew may leak in: CMake ignores its prefixes, pkg-config
# only sees this prefix and the SDK's expat and zlib.
CMAKE=(-G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$PREFIX"
       -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=$MIN_MACOS
       -DCMAKE_INSTALL_NAME_DIR=@rpath -DCMAKE_PREFIX_PATH="$PREFIX"
       "-DCMAKE_IGNORE_PREFIX_PATH=/opt/homebrew;/usr/local")
cmake_build() {  # name options...
    local name=$1; shift
    echo "== $name"
    cmake -S "$WORK/src/$name" -B "$WORK/build/$name" "${CMAKE[@]}" "$@" >"$WORK/build/$name.log" 2>&1
    cmake --build "$WORK/build/$name" >>"$WORK/build/$name.log" 2>&1
    cmake --install "$WORK/build/$name" >>"$WORK/build/$name.log" 2>&1
}
cmake_build SDL3 -DSDL_SHARED=ON -DSDL_STATIC=OFF -DSDL_TESTS=OFF -DSDL_EXAMPLES=OFF
cmake_build sdl2-compat -DSDL2COMPAT_TESTS=OFF -DSDL2COMPAT_STATIC=OFF
# PNG for colour bitmap fonts (Apple Color Emoji, the game's GetCharRGBA path);
# no Brotli (WOFF2), HarfBuzz or BZip2.
cmake_build libpng -DPNG_SHARED=ON -DPNG_STATIC=OFF -DPNG_TESTS=OFF -DPNG_TOOLS=OFF -DPNG_FRAMEWORK=OFF
cmake_build freetype -DBUILD_SHARED_LIBS=ON -DFT_REQUIRE_ZLIB=ON -DFT_REQUIRE_PNG=ON -DFT_DISABLE_BZIP2=ON \
    -DFT_DISABLE_HARFBUZZ=ON -DFT_DISABLE_BROTLI=ON

echo "== fontconfig"
MESON_VERSION=1.12.1   # fontconfig 2.18 needs meson >= 1.11
[ "$("$WORK/venv/bin/meson" --version 2>/dev/null)" = $MESON_VERSION ] \
    || { python3 -m venv "$WORK/venv" && "$WORK/venv/bin/pip" -q --disable-pip-version-check install meson==$MESON_VERSION; }
SDK="$(xcrun --show-sdk-path)"
mkdir -p "$WORK/pc"
sdk_pc() {  # name version: a .pc for a library of the macOS SDK (it ships none)
    printf 'Name: %s\nDescription: macOS SDK %s\nVersion: %s\nLibs: -L%s/usr/lib -l%s\nCflags:\n' \
        "$1" "$1" "$2" "$SDK" "${3:-$1}" >"$WORK/pc/$1.pc"
}
sdk_pc expat 2.6.0
sdk_pc zlib 1.2.12 z   # freetype2.pc requires it
# Fontconfig compiles its prefix in (default configuration, cache): give it a
# neutral one and stage the install. The runtime finds etc/fonts next to the
# library when FONTCONFIG_PATH is not set (runtime/fonts.c).
FC_PREFIX=/opt/p2-host-deps
STAGE="$WORK/build/fontconfig-stage"
(cd "$WORK/src/fontconfig" && PKG_CONFIG_LIBDIR="$PREFIX/lib/pkgconfig:$WORK/pc" \
    "$WORK/venv/bin/meson" setup "$WORK/build/fontconfig" --prefix $FC_PREFIX --buildtype release \
    -Ddoc=disabled -Dtests=disabled -Dtools=disabled -Dnls=disabled -Dcache-build=disabled \
    -Dxml-backend=expat -Diconv=enabled -Ddefault_library=shared \
    -Dc_args="-mmacosx-version-min=$MIN_MACOS" -Dc_link_args="-mmacosx-version-min=$MIN_MACOS" \
    >"$WORK/build/fontconfig.log" 2>&1
 DESTDIR="$STAGE" ninja -C "$WORK/build/fontconfig" install >>"$WORK/build/fontconfig.log" 2>&1) \
    || { tail -30 "$WORK/build/fontconfig.log"; exit 1; }
S="$STAGE$FC_PREFIX"
cp -R "$S/include/fontconfig" "$PREFIX/include/"
cp -P "$S"/lib/libfontconfig*.dylib "$PREFIX/lib/"
mkdir -p "$PREFIX/etc/fonts/conf.d"
cp "$S/etc/fonts/fonts.conf" "$PREFIX/etc/fonts/"
# conf.d holds links into share/: copy their targets instead.
cp -L "$S"/etc/fonts/conf.d/*.conf "$PREFIX/etc/fonts/conf.d/"
# Relative <include> is looked up in FONTCONFIG_PATH; the cache goes to ~/.cache.
sed -i '' -e "s|<include ignore_missing=\"yes\">$FC_PREFIX/etc/fonts/conf.d</include>|<include ignore_missing=\"yes\">conf.d</include>|" \
    -e "\|<cachedir>$FC_PREFIX/|d" "$PREFIX/etc/fonts/fonts.conf"
! grep -n "$FC_PREFIX" "$PREFIX/etc/fonts/fonts.conf" || { echo "fonts.conf still names $FC_PREFIX" >&2; exit 1; }

echo "== libd3d9metal (mtld3d ${MTLD3D_REV:0:10})"
M="$WORK/src/mtld3d"
if [ "$(git -C "$M" rev-parse HEAD 2>/dev/null)" != "$MTLD3D_REV" ]; then
    rm -rf "$M" && git init -q "$M" && git -C "$M" fetch -q --depth 1 "$MTLD3D_URL" "$MTLD3D_REV" \
        && git -C "$M" checkout -q FETCH_HEAD
fi
# No build-machine path in the published library (Rust panic locations, C++
# asserts of snmalloc); the
# native crate sets no rustflags of its own for macOS, so RUSTFLAGS is safe.
CARGO_HOME_DIR="${CARGO_HOME:-$HOME/.cargo}"
(cd "$M/native" && CARGO_TARGET_DIR="$WORK/build/mtld3d" \
    RUSTFLAGS="--remap-path-prefix=$M=mtld3d --remap-path-prefix=$CARGO_HOME_DIR=cargo --remap-path-prefix=$HOME=~" \
    CFLAGS="-ffile-prefix-map=$CARGO_HOME_DIR=cargo -ffile-prefix-map=$HOME=~" \
    CXXFLAGS="-ffile-prefix-map=$CARGO_HOME_DIR=cargo -ffile-prefix-map=$HOME=~" \
    cargo build -q --profile production)
cp "$WORK/build/mtld3d/production/libd3d9metal.dylib" "$PREFIX/lib/"

# --- Relocation: @rpath install names, no absolute path left --------------
cd "$PREFIX/lib"
rm -rf cmake pkgconfig ./*.a ./*.la
for lib in *.dylib; do
    [ -L "$lib" ] && continue
    id=$(otool -D "$lib" | tail -1)
    args=(-id "@rpath/${id##*/}")
    for dep in $(otool -L "$lib" | awk 'NR > 1 {print $1}'); do
        case "$dep" in "$PREFIX"/lib/*|"$FC_PREFIX"/lib/*) args+=(-change "$dep" "@rpath/${dep##*/}") ;; esac
    done
    otool -l "$lib" | grep -q 'path @loader_path ' || args+=(-add_rpath @loader_path)
    install_name_tool "${args[@]}" "$lib" 2>/dev/null
    codesign --force -s - "$lib"
done
# sdl2-compat loads SDL3 with dlopen("@loader_path/libSDL3.dylib") first.
[ -e libSDL3.dylib ] || ln -s libSDL3.0.dylib libSDL3.dylib
bad=$(for lib in *.dylib; do otool -L "$lib" | awk -v l="$lib" 'NR > 1 && $1 !~ /^(@rpath\/|\/usr\/lib\/|\/System\/)/ {print l": "$1}'; done)
[ -z "$bad" ] || { echo "non-relocatable dependencies:"; echo "$bad"; exit 1; } >&2
for lib in *.dylib; do
    [ -L "$lib" ] && continue
    v=$(otool -l "$lib" | awk '/LC_BUILD_VERSION/ {f=1} f && $1 == "minos" {print $2; exit}')
    [ "$v" = "$MIN_MACOS" ] || { echo "$lib: minimum macOS $v, expected $MIN_MACOS" >&2; exit 1; }
done

# --- Fontconfig configuration, licenses, manifest -------------------------
cd "$PREFIX"
rm -rf share var bin
leaks=$(grep -rlI "$HOME\|$WORK" . || true; for f in lib/*.dylib; do [ -L "$f" ] || ! strings "$f" | grep -q "$HOME" || echo "$f"; done)
[ -z "$leaks" ] || { echo "build paths left in:"; echo "$leaks"; exit 1; } >&2
cp "$WORK/src/SDL3/LICENSE.txt" licenses/SDL3.txt
cp "$WORK/src/sdl2-compat/LICENSE.txt" licenses/sdl2-compat.txt
cp "$WORK/src/libpng/LICENSE" licenses/libpng.txt
cp "$WORK/src/freetype/LICENSE.TXT" licenses/freetype.txt
cp "$WORK/src/freetype/docs/FTL.TXT" licenses/freetype-FTL.txt
cp "$WORK/src/fontconfig/COPYING" licenses/fontconfig.txt
cp "$M/LICENSE"* licenses/mtld3d.txt
python3 - "$PREFIX/manifest.json" "$BUNDLE_VERSION" "$MIN_MACOS" "$MTLD3D_REV" <<'EOF'
import json, sys
path, version, minos, rev = sys.argv[1:]
json.dump({'bundle': int(version), 'arch': 'arm64', 'min_macos': minos, 'mtld3d_rev': rev,
           'sdl3': '3.4.20', 'sdl2_compat': '2.32.74', 'libpng': '1.6.59', 'freetype': '2.15.0', 'fontconfig': '2.18.3'},
          open(path, 'w'), indent=2)
EOF

cd "$WORK"
tar --uid 0 --gid 0 --uname root --gname wheel -cJf "$NAME.tar.xz" "$NAME"
shasum -a 256 "$NAME.tar.xz" | tee "$NAME.tar.xz.sha256"
du -h "$NAME.tar.xz"
