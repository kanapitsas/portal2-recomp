RUNTIME := $(sort $(wildcard runtime/*.c))
CHECKS := $(sort $(wildcard tests/checks_*.c))
HEADERS := $(sort $(wildcard runtime/*.h runtime/d3d9/*.h))
PYTHON ?= python3
CC = clang
CFLAGS ?= -O2 -g -Wall -Wextra -Werror
# Bibliothèques hôtes ARM64 des ponts natifs ; partagées avec tools/build_game.py.
HOST_CFLAGS = -I/opt/homebrew/include -I/opt/homebrew/include/freetype2 -DGL_SILENCE_DEPRECATION -DOPENAL_DEPRECATED= -DAL_SILENCE_DEPRECATION
HOST_LIBS = -L/opt/homebrew/lib -lSDL2 -lfreetype -lfontconfig -liconv -framework OpenGL -framework OpenAL -lobjc
CFLAGS += $(HOST_CFLAGS)
LIFT_FLAGS ?= --allow-unimplemented-imports
INPUT_DIR ?= ../work/portal2-linux/bin
PREP = build/prep
EXPORTS = Plat_IsInBenchmarkMode Plat_SetBenchmarkMode AreAllAssertsDisabled SetAllAssertsDisabled UnitTestCount GetUnitTest EncodeBinaryToString DecodeBinaryFromString Plat_GetMemPageSize ZeroMemory Plat_Alloc Plat_Realloc Plat_Free Plat_DefaultAllocErrorFn UnitTestInstallTestCase LoggingSystem_GetChannelCount LoggingSystem_FindChannel libtier0.so:code:0x157a0 libtier0.so:code:0x15a50 libtier0.so:code:0x15620 libtier0.so:code:0x13dd0 libtier0.so:code:0x13e00 libtier0.so:code:0x13e30 libtier0.so:code:0x13e60 libtier0.so:init:dt libtier0.so:init:0 libtier0.so:init:1 libtier0.so:init:2 libtier0.so:init:3 libtier0.so:init:4 libtier0.so:init:5 libtier0.so:init:6 libtier0.so:init:7 libtier0.so:init:8 libtier0.so:init:9 libtier0.so:init:10 libtier0.so:init:11 libtier0.so:init:12 libtier0.so:init:13 GetCPUInformation

.PHONY: all test clean
all: build/tier0-smoke

$(PREP)/layout.json: tools/elfprep.py $(INPUT_DIR)/libtier0.so $(INPUT_DIR)/libvstdlib.so
	$(PYTHON) tools/elfprep.py --out $(PREP) $(INPUT_DIR)/libtier0.so@0x10000000 $(INPUT_DIR)/libvstdlib.so@0x11000000

build/tier0.c: Makefile tools/lift.py tools/float_lift.py tools/registry.py $(PREP)/layout.json
	$(PYTHON) tools/lift.py $(LIFT_FLAGS) --layout $(PREP)/layout.json --out $@ $(EXPORTS)

build/tier0-smoke: build/tier0.c $(RUNTIME) $(HEADERS) tests/tier0_smoke.c
	$(CC) $(CFLAGS) -Iruntime $(RUNTIME) build/tier0.c tests/tier0_smoke.c $(HOST_LIBS) -o $@

build/abi-fixture.c: tests/generate_abi_fixture.py tools/lift.py tools/float_lift.py tools/registry.py tests/lifter_programs.s
	@mkdir -p build
	$(PYTHON) tests/generate_abi_fixture.py $@

# Greffon PE de test (DLL liée par tests/pe_fixture.py, traduite par lift.generate_pe).
build/pe-fixture.c: tests/generate_pe_fixture.py tests/pe_fixture.py tests/pe_plugin.s tools/lift.py tools/float_lift.py tools/peprep.py
	@mkdir -p build
	$(PYTHON) tests/generate_pe_fixture.py $@

build/abi-smoke: build/abi-fixture.c build/pe-fixture.c tests/abi_smoke.c $(wildcard tests/*.h) $(CHECKS) $(RUNTIME) $(HEADERS)
	$(CC) $(CFLAGS) -Iruntime -Itests $(RUNTIME) build/abi-fixture.c build/pe-fixture.c tests/abi_smoke.c $(CHECKS) $(HOST_LIBS) -o $@

test: build/tier0-smoke build/abi-smoke
	$(PYTHON) -m unittest discover -s tests -p 'test_*.py' -v
	./build/tier0-smoke $(PREP)
	./build/abi-smoke

clean:
	rm -rf build

.PHONY: progress sanitize game
# Jeu complet : toutes les fonctions connues, C réparti et compilé en parallèle
# avec cache d'objets (build/game/obj). GAME_OPT=0 pour un binaire de diagnostic.
GAME_OPT ?= 1
game:
	$(PYTHON) tools/build_game.py --opt $(GAME_OPT)

progress: $(PREP)/layout.json
	$(PYTHON) tools/progress.py --layout $(PREP)/layout.json --out build/progress.json

build/tier0-smoke-ubsan: build/tier0.c $(RUNTIME) $(HEADERS) tests/tier0_smoke.c
	$(CC) $(CFLAGS) -O1 -fsanitize=undefined -fno-sanitize-recover=all -Iruntime $(RUNTIME) build/tier0.c tests/tier0_smoke.c $(HOST_LIBS) -o $@

build/abi-smoke-ubsan: build/abi-fixture.c build/pe-fixture.c $(RUNTIME) $(HEADERS) tests/abi_smoke.c $(wildcard tests/*.h) $(CHECKS)
	$(CC) $(CFLAGS) -O1 -fsanitize=undefined -fno-sanitize-recover=all -Iruntime -Itests $(RUNTIME) build/abi-fixture.c build/pe-fixture.c tests/abi_smoke.c $(CHECKS) $(HOST_LIBS) -o $@

sanitize: build/tier0-smoke-ubsan build/abi-smoke-ubsan
	./build/tier0-smoke-ubsan $(PREP)
	./build/abi-smoke-ubsan

# libd3d9metal seule, liée statiquement comme dans l'app iOS (sans le pont) :
# make d3d9metal-check D3D9METAL_A=…/mtld3d/native/target/release/libd3d9metal.a
.PHONY: d3d9metal-check
D3D9METAL_LIBS = -framework Metal -framework MetalFX -framework QuartzCore -framework CoreGraphics -framework CoreVideo \
	-framework Foundation -framework CoreFoundation -lobjc -liconv
build/d3d9metal-check: tests/d3d9metal_native_check.c runtime/d3d9/d3d9metal.h $(D3D9METAL_A)
	@test -n "$(D3D9METAL_A)" || { echo "D3D9METAL_A=/path/to/libd3d9metal.a (mtld3d: make native)"; exit 2; }
	$(CC) -O1 -g -Wall -Wextra -Werror tests/d3d9metal_native_check.c $(D3D9METAL_A) $(D3D9METAL_LIBS) -o $@
d3d9metal-check: build/d3d9metal-check
	./build/d3d9metal-check
