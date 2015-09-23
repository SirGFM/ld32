
CC=gcc
.SUFFIXES=.c .o

#==============================================================================
# Define compilation target
#==============================================================================
  TARGET := game
#==============================================================================

#==============================================================================
# Define every object required by compilation
#==============================================================================
 OBJS =                                \
         $(OBJDIR)/common.o            \
         $(OBJDIR)/main.o              \
         $(OBJDIR)/player.o            \
         $(OBJDIR)/playstate.o         
         #$(OBJDIR)/audio.o             \
         #$(OBJDIR)/camera.o            \
         #$(OBJDIR)/collision.o         \
         #$(OBJDIR)/global.o            \
         #$(OBJDIR)/map001.o            \
         #$(OBJDIR)/sprite.o            \
         #$(OBJDIR)/text.o              \
         #$(OBJDIR)/ui.o                
#==============================================================================

#==============================================================================
# Set OS flag
#==============================================================================
  OS := $(shell uname)
  ifeq ($(OS), MINGW32_NT-6.1)
    OS := Win
#   Also, set the icon
    ICON = $(WINICON)
  endif
  ifeq ($(CC), emcc)
    OS := emscript
  endif
#==============================================================================

#==============================================================================
# Define CFLAGS (compiler flags)
#==============================================================================
# Add all warnings and default include path
  CFLAGS := -Wall -I"./include/"
  ifeq ($(OS), emscript)
    CFLAGS := $(CFLAGS) -I"$(EMSCRIPTEN)/system/include/"
  endif
# Add architecture flag
  ARCH := $(shell uname -m)
  ifeq ($(OS), emscript)
    CFLAGS := $(CFLAGS) -m32
  else
    ifeq ($(ARCH), x86_64)
      CFLAGS := $(CFLAGS) -m64
    else
      CFLAGS := $(CFLAGS) -m32
    endif
  endif
# Add debug flags
  ifeq ($(OS), emscript)
    CFLAGS := $(CFLAGS) -O2
  else
    ifneq ($(RELEASE), yes)
      CFLAGS := $(CFLAGS) -g -O0 -DDEBUG
    else
      CFLAGS := $(CFLAGS) -O3
    endif
  endif
# Set flags required by OS
  ifeq ($(OS), Win)
    CFLAGS := $(CFLAGS) -I"/d/windows/mingw/include" -I/c/GFraMe/include
  else
    CFLAGS := $(CFLAGS) -fPIC
  endif
# Set a flag so we know it's compiling for EMCC
  ifeq ($(OS), emscript)
    CFLAGS := $(CFLAGS) -DEMSCRIPT
  endif
#==============================================================================

#==============================================================================
# Define LFLAGS (linker flags)
#==============================================================================
# Add the framework
  ifeq ($(RELEASE), yes)
    LFLAGS := -lGFraMe
  else
    LFLAGS := -lGFraMe_dbg
  endif
# Add libs and paths required by an especific OS
  ifeq ($(OS), Win)
    LFLAGS := -mwindows -lmingw32 $(LFLAGS) -lSDL2main
    LFLAGS := -L/d/windows/mingw/mingw32/lib $(LFLAGS)
# Prepend the framework search path
    LFLAGS := -L/c/GFraMe/lib/ $(LFLAGS)
# TODO Add OpenGL
  else
# Prepend the framework search path
    LFLAGS := -L/usr/lib/GFraMe/ $(LFLAGS)
# TODO Add OpenGL
  endif
#==============================================================================

#==============================================================================
# Define where source files can be found and where objects & binary are output
#==============================================================================
 VPATH := new-src
 OBJDIR := obj/$(OS)
 BINDIR := bin/$(OS)
#==============================================================================

#==============================================================================
# Define the generated icon
#==============================================================================
 WINICON := assets/icon.o
#==============================================================================

#==============================================================================
# Make the objects list constant (and the icon, if any)
#==============================================================================
 OBJS := $(OBJS)
 ICON := $(ICON)
#==============================================================================

#==============================================================================
# Define default compilation rule
#==============================================================================
all: MKDIRS $(BINDIR)/$(TARGET)
	date
#==============================================================================

#==============================================================================
# Define a rule to generated the icon
#==============================================================================
$(WINICON):
	windres assets/icon.rc $(WINICON)
#==============================================================================

#==============================================================================
# Rule for actually building the game
#==============================================================================
$(BINDIR)/$(TARGET): MKDIRS $(OBJS) $(ICON)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(ICON) $(LFLAGS)
#==============================================================================

#==============================================================================
# Rule for compiling any .c in its object
#==============================================================================
$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
#==============================================================================


#==============================================================================
# Rule for checking if any directory should be created
#==============================================================================
MKDIRS: | $(OBJDIR) $(BINDIR)
#==============================================================================

#==============================================================================
# Rules for actually creating every directory
#==============================================================================
$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(BINDIR):
	@mkdir -p $(BINDIR)
#==============================================================================

.PHONY: clean mostlyclean

clean:
	@rm -f $(OBJS)
	@rm -f $(BINDIR)/$(TARGET)

mostlyclean:
	@make clean
	@rm -rf $(OBJDIR)
	@rm -rf $(BINDIR)

