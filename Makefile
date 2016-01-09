#CC = clang
CFLAGS=-Os
# Compile for current machine
CFLAGS+=-march=native
# Tuning
CFLAGS+=-fomit-frame-pointer -pipe -flto
# No optimization
#CFLAGS=-O0
# Debug
#CFLAGS=-O0 -ggdb -DDEBUG -DSTDIO -pg -Werror
# Enable all warnings
#CFLAGS+=-W -Wall -Wextra

# Comment out to enable assertions
#DEFINES += -DNDEBUG

# Uncomment to enable SHAPE extension support
DEFINES += -DUSE_SHAPE
EXTRALIBS += -lXext # Required for SHAPE extension

# Titlebar Xft support:
# Warning, Xft impedes performance and leaks memory.
#DEFINES += -DUSE_XFT
#EXTRALIBS += -lXft
#EXTRAINCLUDES += `pkg-config --cflags xft`

# Not necessary except on old NetBSD, for Xft support:
#EXTRAINCLUDES += -I/usr/pkg/include/freetype2 -I/usr/X11R6/include/freetype2

# Uncomment to enable colormap support
# Saves 600 bytes
#DEFINES+= -DUSE_CMAP

# Uncomment to enable parsing command line arguments.
#  Saves ~2030 bytes
#DEFINES += -DUSE_ARGV 

# Uncomment to enable titlebars
DEFINES += -DUSE_TBAR
EXTRASRCS += titlebar.c 

# Uncomment to enable window snapping. (Adds 4k bytes)
DEFINES += -DUSE_SNAP

# Uncomment to enable STDIO
DEFINES += -DSTDIO

# Uncomment to enable EWMH (Adds ~4k bytes)
DEFINES += -DEWMH
EXTRASRCS += ewmh.c

# Uncomment to enable MWM hints
DEFINES += -DMWM

# Uncomment to disable everything for minimal configuration 
#CFLAGS=-Os
#DEFINES=-DNDEBUG
#EXTRASRCS=
#EXTRALIBS=
#EXTRAINCLUDES=

include jbwm.mk
