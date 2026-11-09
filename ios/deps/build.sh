#!/bin/bash
# Dépendances hôtes du port iOS (simulateur arm64 par défaut), construites dans
# ../work/ios-deps à côté du dépôt (là où tools/build_ios.py les cherche).
#   ios/deps/build.sh [sim|device]
# Sources à versions fixées : SDL2 2.30.9, FreeType 2.13.3, gl4es (commit
# ci-dessous) avec les correctifs de gl4es-ios.patch.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
WORK="${P2_IOS_DEPS:-$(cd "$HERE/../../.." && pwd)/work/ios-deps}"
TARGET="${1:-sim}"
case "$TARGET" in
  sim)    SYSROOT=iphonesimulator ;;
  device) SYSROOT=iphoneos ;;
  *) echo "usage: $0 [sim|device]"; exit 2 ;;
esac
GL4ES_COMMIT=a025615bc79ac43ec911dced8fe5b8eb45a0b72d
mkdir -p "$WORK/src"
cd "$WORK"

fetch() {  # dossier archive url
  [ -d "src/$1" ] && return
  curl -fL "$3" -o "src/$2"
  mkdir -p "src/$1" && tar -xf "src/$2" -C "src/$1" --strip-components=1
}
fetch SDL2 SDL2-2.30.9.tar.gz https://github.com/libsdl-org/SDL/releases/download/release-2.30.9/SDL2-2.30.9.tar.gz
fetch freetype freetype-2.13.3.tar.xz https://download.savannah.gnu.org/releases/freetype/freetype-2.13.3.tar.xz
if [ ! -d src/gl4es ]; then
  git clone https://github.com/ptitSeb/gl4es.git src/gl4es
  git -C src/gl4es checkout -q "$GL4ES_COMMIT"
fi
# Correctifs : appliqués si l'arbre est propre, vérifiés sinon.
if git -C src/gl4es diff --quiet; then
  git -C src/gl4es apply "$HERE/gl4es-ios.patch"
elif ! git -C src/gl4es diff | cmp -s - "$HERE/gl4es-ios.patch"; then
  echo "src/gl4es modifié et différent de gl4es-ios.patch : à réconcilier" >&2; exit 1
fi

PREFIX="$WORK/$TARGET"
COMMON=(-G Ninja -DCMAKE_SYSTEM_NAME=iOS "-DCMAKE_OSX_SYSROOT=$SYSROOT" -DCMAKE_OSX_ARCHITECTURES=arm64
        -DCMAKE_OSX_DEPLOYMENT_TARGET=17.0 -DCMAKE_BUILD_TYPE=Release "-DCMAKE_INSTALL_PREFIX=$PREFIX"
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5)

build() {  # nom source options...
  local name=$1 src=$2; shift 2
  echo "== $name"
  cmake -S "src/$src" -B "build-$TARGET/$name" "${COMMON[@]}" "$@" > "log-$TARGET-$name.txt" 2>&1
  cmake --build "build-$TARGET/$name" >> "log-$TARGET-$name.txt" 2>&1
}

build freetype freetype -DBUILD_SHARED_LIBS=OFF -DFT_DISABLE_ZLIB=ON -DFT_DISABLE_BZIP2=ON \
  -DFT_DISABLE_PNG=ON -DFT_DISABLE_HARFBUZZ=ON -DFT_DISABLE_BROTLI=ON
cmake --install "build-$TARGET/freetype" >> "log-$TARGET-freetype.txt" 2>&1

build SDL2 SDL2 -DSDL_SHARED=OFF -DSDL_STATIC=ON -DSDL_TEST=OFF
cmake --install "build-$TARGET/SDL2" >> "log-$TARGET-SDL2.txt" 2>&1

# gl4es : OpenGL 2.1 de bureau sur OpenGL ES, statique ; les fonctions GLES lui
# sont fournies par set_getprocaddress (pas d'EGL, pas de chargeur).
build gl4es gl4es -DSTATICLIB=ON -DNOX11=ON -DNOEGL=ON -DNO_LOADER=ON -DNO_GBM=ON -DDEFAULT_ES=2
mkdir -p "$PREFIX/lib" "$PREFIX/include/gl4es"
# gl4es écrit sa bibliothèque dans son arbre source (lib/).
mv src/gl4es/lib/libGL.a "$PREFIX/lib/libgl4es.a"
cp -R src/gl4es/include/. "$PREFIX/include/gl4es/"
ls -la "$PREFIX/lib"
