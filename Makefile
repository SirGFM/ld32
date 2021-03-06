
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
         $(OBJDIR)/audio.o             \
         $(OBJDIR)/camera.o            \
         $(OBJDIR)/collision.o         \
         $(OBJDIR)/global.o            \
         $(OBJDIR)/main.o              \
         $(OBJDIR)/map001.o            \
         $(OBJDIR)/player.o            \
         $(OBJDIR)/playstate.o         \
         $(OBJDIR)/sprite.o            \
         $(OBJDIR)/text.o              \
         $(OBJDIR)/ui.o                
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
#==============================================================================

#==============================================================================
# Define CFLAGS (compiler flags)
#==============================================================================
# Add all warnings and default include path
  CFLAGS := -Wall
# Add the framework includes
  CFLAGS := $(CFLAGS) -I"./lib/GFraMe/include/"
# Add architecture flag
  ARCH := $(shell uname -m)
  ifeq ($(ARCH), x86_64)
    CFLAGS := $(CFLAGS) -m64
  else
    CFLAGS := $(CFLAGS) -m32
  endif
# Add debug flags
  ifneq ($(RELEASE), yes)
    CFLAGS := $(CFLAGS) -g -O0 -DDEBUG
  else
    CFLAGS := $(CFLAGS) -O1
  endif
#==============================================================================

#==============================================================================
# Define LFLAGS (linker flags)
#==============================================================================
# Add the framework library
 LFLAGS := -lGFraMe -lm
# Add dependencies
 ifeq ($(OS), Win)
   LFLAGS := -L./lib/GFraMe/bin/Win -mwindows -lmingw32 $(LFLAGS) -lSDL2main
   ifeq ($(USE_OPENGL), yes)
     LFLAGS := $(LFLAGS) -lopengl32
   endif
 else
   LFLAGS := -L./lib/GFraMe/bin/Linux $(LFLAGS)
   ifeq ($(USE_OPENGL), yes)
     LFLAGS := $(LFLAGS) -lGL
   endif
 endif
 LFLAGS := $(LFLAGS) -lSDL2
#==============================================================================

#==============================================================================
# Define library (to force compilation)
#==============================================================================
 LIB := ./lib/GFraMe/bin/Linux/libGFraMe.a
#==============================================================================

#==============================================================================
# Define the generated icon
#==============================================================================
 WINICON := assets/icon.o
#==============================================================================

#==============================================================================
# Define where source files can be found and where objects & binary are output
#==============================================================================
 VPATH := src
 OBJDIR := obj
 BINDIR := bin
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

$(WINICON):
	windres assets/icon.rc $(WINICON)

$(BINDIR)/$(TARGET): MKDIRS $(LIB) $(OBJS) $(ICON)
	$(CC) $(CFLAGS) -o $(BINDIR)/$(TARGET) $(OBJS) $(ICON) $(LFLAGS)

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(LIB):
	make static --directory=./lib/GFraMe/ USE_OPENGL=$(USE_OPENGL)

MKDIRS: | $(OBJDIR) $(BINDIR)

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(BINDIR):
	@mkdir -p $(BINDIR)

.PHONY: clean mostlyclean

clean:
	@rm -f $(OBJS)
	@rm -f $(BINDIR)/$(TARGET)

mostlyclean:
	@make clean
	@rm -rf $(OBJDIR)
	@rm -rf $(BINDIR)
	@make clean --directory=./lib/GFraMe/

