.PHONY: linux linux-32 linux-64 linux-debug linux-debug-32 linux-debug-64 \
	win win-32 win-64 win-debug win-debug-32 win-debug-64 \
	web \
	tests \
	all game clean


# =========================================================================
# General output configuration.

# Base name for the output binary.
OUTPUT_BIN := game

# Name of the tileset with type information.
TYPE_TILESET := 8x8
# =========================================================================


# =========================================================================
# Configure the OS/arch variables
# and ensure that only a single target was specified.
undefine TGT
ifneq ($(filter linux%,$(MAKECMDGOALS)),)
  OS := linux
  TGT := $(TGT)linux
endif
ifneq ($(filter win%,$(MAKECMDGOALS)),)
  OS := win
  EXT := .exe
  TGT := $(TGT)win
endif
ifneq ($(filter web,$(MAKECMDGOALS)),)
  OS := emscript
  EXT := .ignore
  TGT := $(TGT)web
endif

ifneq ($(filter tests,$(MAKECMDGOALS)),)
  OUTPUT_BIN := unit-tests
  OS := linux
  ARCH := x86_64
  TGT := $(TGT)tests
endif

ifneq ($(filter-out linux win web tests,$(TGT)),)
  $(error multiple targets were specified!)
endif

undefine TGT
ifneq ($(filter %-32,$(MAKECMDGOALS)),)
  ARCH := i686
  TGT := $(TGT)32
endif
ifneq ($(filter %-64,$(MAKECMDGOALS)),)
  ARCH := x86_64
  TGT := $(TGT)64
endif

ifneq ($(filter-out 32 64,$(TGT)),)
  $(error multiple architectures were specified!)
endif
# =========================================================================


# =========================================================================
# Automatically configure the build environment.
-include /gframe-dev/Makefile.env
# =========================================================================


# =========================================================================
# Configure the flags for debug mode.
MODE := release
ifneq ($(filter %-debug,$(MAKECMDGOALS)),)
  MODE := debug
endif
ifeq ($(OUTPUT_BIN), unit-tests)
    MODE := tests
endif

ifeq ($(filter-out debug tests,$(MODE)),)
  OLEVEL := -O0 -g
  CFLAGS := $(CFLAGS) $(OLEVEL) -DDEBUG
  LIB_SUFFIX := _dbg
endif

# =========================================================================

# =========================================================================
# Configure the required libs and include paths
CFLAGS := $(CFLAGS) -Iinclude

# Also add test headers, if necessary.
ifeq ($(OUTPUT_BIN), unit-tests)
  CFLAGS := $(CFLAGS) -Itests/include -Itests/vendor/acutest/include
endif

LDLIBS := -lGFraMe$(LIB_SUFFIX) -lCSynth

ifeq (win, $(OS))
  LDFLAGS := $(LDFLAGS) -mwindows
  LDLIBS := $(LDLIBS) -lSDL2 -lopengl32
endif
ifeq (emscript, $(OS))
  CFLAGS := $(CFLAGS)
  LDLIBS := -s USE_SDL=2 -s WASM=1 $(LIB_DIR)/libGFraMe.bc $(LIB_DIR)/libCSynth.bc
endif
# =========================================================================


# =========================================================================
# Recursive wildcard from https://stackoverflow.com/a/18258352
#
# To find every .c file in src:
#   FILES := $(call rwildcard, , *.c)
# To find all the .c and .h files in src:
#   FILES := $(call rwildcard, src/, *.c *.h)
rwildcard=$(foreach d,$(wildcard $1*), \
	$(call rwildcard,$d/,$2) \
	$(filter $(subst *,%,$2),$d))
# =========================================================================


# =========================================================================
# Prepare a unique output directory based on the OS/arch
TGT_DIR := $(OS)-$(ARCH)-$(MODE)
# =========================================================================


# =========================================================================
# Set the default rule.
all: game
# =========================================================================


# =========================================================================
# Find every input file in ./src and convert it to a target object.
# Additionally, for Windows, also add the icon to the list of objects.
SOURCES := $(call rwildcard,src,*.c)

# Also add test sources, if necessary.
ifeq ($(OUTPUT_BIN), unit-tests)
  SOURCES := $(filter-out src/main.c,$(SOURCES))
  SOURCES := $(SOURCES) $(call rwildcard,tests,*.c)
endif

SOURCE_OBJECTS := $(SOURCES:%.c=obj/$(TGT_DIR)/%.o)

ifeq (win,$(OS))
  ICONS := $(call rwildcard,assets,*.rc)
  ICON_OBJECTS := $(ICONS:%.rc=obj/$(TGT_DIR)/%.o)
endif

OBJECTS := $(SOURCE_OBJECTS) $(ICON_OBJECTS)

MAPS := $(call rwildcard,assets,*tmx)
MAP_OBJECTS := $(MAPS:%.tmx=obj/%.map)

WEB_ASSETS_BMP := $(call rwildcard,assets,*bmp)
WEB_ASSETS := $(WEB_ASSETS_BMP)
# =========================================================================


# =========================================================================
# Track dependencies in source files.
# This is only executed if any rule other than maps and clean is specified.
ifneq ($(filter-out maps clean,$(MAKECMDGOALS)),)
-include $(SOURCE_OBJECTS:%.o=%.d)
endif
# =========================================================================


# =========================================================================
# Create directories as needed.
%.mkdir:
	@ echo -e '\tmkdir $(@D)'
	@ mkdir -p $(@D)
	@ touch $@
# =========================================================================


# =========================================================================
# Build the final game.
# Instead of manually defining paths, use $(TGT_DIR) to simplify the rules.
linux linux-32 linux-64 linux-debug linux-debug-32 linux-debug-64 \
	win win-32 win-64 win-debug win-debug-32 win-debug-64: game

web: bin/$(TGT_DIR)/$(OUTPUT_BIN).html
game: bin/$(TGT_DIR)/$(OUTPUT_BIN)$(EXT)
maps: obj/maps.generated

bin/$(TGT_DIR)/$(OUTPUT_BIN)$(EXT): $(OBJECTS) | bin/$(TGT_DIR)/$(OUTPUT_BIN).mkdir
	@ echo -e '\t[ CC] Release target: $@'
	@ $(CC) $(OLEVEL) $(LDFLAGS) -o $@ $^ $(LDLIBS)

bin/$(TGT_DIR)/$(OUTPUT_BIN).html: bin/$(TGT_DIR)/$(OUTPUT_BIN).bc $(WEB_ASSETS)
	@ echo -e '\t[ CC] Release target: $@'
	@ $(CC) -o $@ $< $(LDLIBS) $(foreach FILE,$(WEB_ASSETS),--preload-file $(FILE)@/$(FILE:assets/%=%))

bin/$(TGT_DIR)/$(OUTPUT_BIN).bc: $(OBJECTS) | bin/$(TGT_DIR)/$(OUTPUT_BIN).mkdir
	@ echo -e '\t[ CC] Release target: $@'
	@ $(CC) $(OLEVEL) -shared -o $@ $^

obj/maps.generated: $(MAP_OBJECTS)
	@ date > $@

tests: bin/$(TGT_DIR)/$(OUTPUT_BIN)$(EXT)
	@ cp bin/$(TGT_DIR)/$(OUTPUT_BIN)$(EXT) tests/
# =========================================================================


# =========================================================================
# Build object files from source files.
obj/$(TGT_DIR)/%.o: %.rc | obj/$(TGT_DIR)/%.mkdir
	@ echo -e '\t[WRC] $< -> $@'
	@ $(WINDRES) $< $@

obj/$(TGT_DIR)/%.o: %.c | obj/$(TGT_DIR)/%.mkdir
	@ echo -e '\t[ CC] $< -> $@'
	@ $(CC) $(CFLAGS) -o $@ -c $<

obj/$(TGT_DIR)/%.d: %.c | obj/$(TGT_DIR)/%.mkdir
	@ echo -e '\t[DEP] $< -> $@'
	@ gcc $(CFLAGS) -MM -MG -MT "$@ $(@:%.d=%.o)" $< > $@

obj/%.map: %.tmx | obj/%.mkdir
	@ echo -e '\t[MAP] assets/maps/$(*F)'
	@ python3 ./tools/map-converter.py -t $(TYPE_TILESET) --rm $< ./assets/maps/$(*F)
	@ date > $@
# =========================================================================


# =========================================================================
# Clean up.
clean:
	rm -rf ./obj/ ./bin/
# =========================================================================
