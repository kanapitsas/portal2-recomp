RUNTIME := $(sort $(wildcard runtime/*.c))
CHECKS := $(sort $(wildcard tests/checks_*.c))
PYTHON ?= python3
CC = clang
CFLAGS ?= -O2 -g -Wall -Wextra -Werror
# config/local.json, read once (tools/localconfig.py --make: KEY=value words,
# spaces encoded as %20): host_prefix (P2_HOST_PREFIX, the config's prebuilt
# bundle, else Homebrew) and Portal 2's linux_root.
LOCAL := $(shell $(PYTHON) tools/localconfig.py --make host_prefix linux_root --game portal2 2>/dev/null)
local_value = $(subst %20, ,$(patsubst $(1)=%,%,$(filter $(1)=%,$(LOCAL))))
# Bibliothèques hôtes ARM64 des ponts natifs ; partagées avec tools/build_game.py
# (localconfig.host_flags).
HOST_PREFIX := $(or $(P2_HOST_PREFIX),$(call local_value,host_prefix),/opt/homebrew)
HOST_CFLAGS = -I"$(HOST_PREFIX)/include" -I"$(HOST_PREFIX)/include/freetype2" -DGL_SILENCE_DEPRECATION -DOPENAL_DEPRECATED= -DAL_SILENCE_DEPRECATION
HOST_LIBS = -L"$(HOST_PREFIX)/lib" -Wl,-rpath,"$(HOST_PREFIX)/lib" -lSDL2 -lfreetype -lfontconfig -liconv -framework OpenGL -framework OpenAL -lobjc
CFLAGS += $(HOST_CFLAGS)
LIFT_FLAGS ?= --allow-unimplemented-imports
# Linux binaries of the game (libtier0.so, libvstdlib.so): linux_root of
# config/local.json when it exists, else the historical private path.
# INPUT_DIR_DEP is the same path with spaces escaped for prerequisites.
LOCAL_LINUX_ROOT := $(call local_value,linux_root)
INPUT_DIR ?= $(if $(LOCAL_LINUX_ROOT),$(LOCAL_LINUX_ROOT)/bin/linux32,../work/portal2-linux/bin)
space := $(subst ,, )
INPUT_DIR_DEP := $(subst $(space),\ ,$(INPUT_DIR))
PREP = build/prep
EXPORTS = Plat_IsInBenchmarkMode Plat_SetBenchmarkMode AreAllAssertsDisabled SetAllAssertsDisabled UnitTestCount GetUnitTest EncodeBinaryToString DecodeBinaryFromString Plat_GetMemPageSize ZeroMemory Plat_Alloc Plat_Realloc Plat_Free Plat_DefaultAllocErrorFn UnitTestInstallTestCase LoggingSystem_GetChannelCount LoggingSystem_FindChannel libtier0.so:code:0x157a0 libtier0.so:code:0x15a50 libtier0.so:code:0x15620 libtier0.so:code:0x13dd0 libtier0.so:code:0x13e00 libtier0.so:code:0x13e30 libtier0.so:code:0x13e60 libtier0.so:init:dt libtier0.so:init:0 libtier0.so:init:1 libtier0.so:init:2 libtier0.so:init:3 libtier0.so:init:4 libtier0.so:init:5 libtier0.so:init:6 libtier0.so:init:7 libtier0.so:init:8 libtier0.so:init:9 libtier0.so:init:10 libtier0.so:init:11 libtier0.so:init:12 libtier0.so:init:13 GetCPUInformation

# Objects: one per C file under build/obj (build/obj-ubsan for the UBSan
# variant), with clang's dependency files (-MMD), so `make -j8 check` compiles
# in parallel and only what changed. Same flags as the former one-shot links.
OBJ := build/obj
OBJ_UBSAN := build/obj-ubsan
UBSAN_FLAGS := -O1 -fsanitize=undefined -fno-sanitize-recover=all
ABI_SOURCES := $(RUNTIME) build/abi-fixture.c build/pe-fixture.c tests/abi_smoke.c $(CHECKS)
TIER0_SOURCES := $(RUNTIME) build/tier0.c tests/tier0_smoke.c
ABI_OBJECTS := $(patsubst %.c,$(OBJ)/%.o,$(ABI_SOURCES))
TIER0_OBJECTS := $(patsubst %.c,$(OBJ)/%.o,$(TIER0_SOURCES))
ABI_OBJECTS_UBSAN := $(patsubst %.c,$(OBJ_UBSAN)/%.o,$(ABI_SOURCES))
TIER0_OBJECTS_UBSAN := $(patsubst %.c,$(OBJ_UBSAN)/%.o,$(TIER0_SOURCES))

.DEFAULT_GOAL := check
.PHONY: all test check clean
all: build/tier0-smoke

# build/obj/flags holds the compiler flags of the current build: a change of
# HOST_PREFIX or CFLAGS (setup.sh switching to the prebuilt bundle) recompiles
# every object, which -MMD alone would not notice.
FLAGS_STAMP := $(OBJ)/flags
$(shell mkdir -p $(OBJ); [ "$$(cat $(FLAGS_STAMP) 2>/dev/null)" = "$(CC) $(CFLAGS) $(UBSAN_FLAGS)" ] || echo "$(CC) $(CFLAGS) $(UBSAN_FLAGS)" > $(FLAGS_STAMP))
$(OBJ)/%.o: %.c $(FLAGS_STAMP)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -Iruntime -Itests -MMD -MP -c $< -o $@
$(OBJ_UBSAN)/%.o: %.c $(FLAGS_STAMP)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(UBSAN_FLAGS) -Iruntime -Itests -MMD -MP -c $< -o $@
-include $(ABI_OBJECTS:.o=.d) $(TIER0_OBJECTS:.o=.d) $(ABI_OBJECTS_UBSAN:.o=.d) $(TIER0_OBJECTS_UBSAN:.o=.d)

# $(PREP)/input-dir records the INPUT_DIR in use, so a changed depot regenerates the images.
.PHONY: .FORCE
.FORCE:
$(PREP)/input-dir: .FORCE
	@mkdir -p $(PREP); [ "$$(cat $@ 2>/dev/null)" = "$(INPUT_DIR)" ] || echo "$(INPUT_DIR)" > $@
$(PREP)/layout.json: tools/elfprep.py $(INPUT_DIR_DEP)/libtier0.so $(INPUT_DIR_DEP)/libvstdlib.so $(PREP)/input-dir
	$(PYTHON) tools/elfprep.py --out $(PREP) "$(INPUT_DIR)/libtier0.so@0x10000000" "$(INPUT_DIR)/libvstdlib.so@0x11000000"

build/tier0.c: Makefile tools/lift.py tools/float_lift.py tools/registry.py $(PREP)/layout.json
	$(PYTHON) tools/lift.py $(LIFT_FLAGS) --layout $(PREP)/layout.json --out $@ $(EXPORTS)

build/tier0-smoke: $(TIER0_OBJECTS)
	$(CC) $(CFLAGS) $^ $(HOST_LIBS) -o $@

build/abi-fixture.c: tests/generate_abi_fixture.py tools/lift.py tools/float_lift.py tools/registry.py tests/lifter_programs.s
	@mkdir -p build
	$(PYTHON) tests/generate_abi_fixture.py $@

# Greffon PE de test (DLL liée par tests/pe_fixture.py, traduite par lift.generate_pe).
build/pe-fixture.c: tests/generate_pe_fixture.py tests/pe_fixture.py tests/pe_plugin.s tools/lift.py tools/float_lift.py tools/peprep.py
	@mkdir -p build
	$(PYTHON) tests/generate_pe_fixture.py $@

build/abi-smoke: $(ABI_OBJECTS)
	$(CC) $(CFLAGS) $^ $(HOST_LIBS) -o $@

# Full run: needs the game's Linux binaries (INPUT_DIR) for tier0-smoke.
test: build/tier0-smoke build/abi-smoke
	$(PYTHON) -m unittest discover -s tests -p 'test_*.py' -v
	./build/tier0-smoke $(PREP)
	./build/abi-smoke

# What CI runs (make -j check): Python tests and the runtime against generated fixtures, no game files needed.
check: build/abi-smoke
	$(PYTHON) -m unittest discover -s tests -p 'test_*.py'
	./build/abi-smoke

clean:
	rm -rf build

.PHONY: progress sanitize game
# Whole game: every known function, C split and compiled in parallel with an
# object cache (build/game/obj; build/hl2/obj for GAME=hl2). GAME_OPT=0 for a
# diagnostic binary. GAME=portal2|hl2 (default: P2_GAME, then "game" of
# config/local.json, then portal2; tools/games.py).
GAME_OPT ?= 1
GAME ?=
game:
	$(PYTHON) tools/build_game.py --opt $(GAME_OPT) $(if $(GAME),--game $(GAME))

progress: $(PREP)/layout.json
	$(PYTHON) tools/progress.py --layout $(PREP)/layout.json --out build/progress.json

build/tier0-smoke-ubsan: $(TIER0_OBJECTS_UBSAN)
	$(CC) $(CFLAGS) $(UBSAN_FLAGS) $^ $(HOST_LIBS) -o $@

build/abi-smoke-ubsan: $(ABI_OBJECTS_UBSAN)
	$(CC) $(CFLAGS) $(UBSAN_FLAGS) $^ $(HOST_LIBS) -o $@

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
