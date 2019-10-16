# Includable for c1-like makefiles.
PROGNAME= $(notdir $(patsubst %/,%,$(dir $(abspath $(firstword $(MAKEFILE_LIST)) ))))
VERSION = 0.1
# Artifacts.
TGT     = $(PROGNAME)$(EXEEXT)
SRC     = $(wildcard *.c) $(ADDSRC)
HDR     = $(wildcard *.h) $(ADDHDR)
ADDSRC  =
ADDHDR  =
OBJ     = $(SRC:.c=.o)
# Tarball.
TARDIR  = $(TGT)-$(VERSION)
TARARC  = $(TARDIR)$(TAREXT)
TARGZARC  = $(TARARC)$(GZEXT)

# Manual.
MANSTDSECT = 1
MANUAL  = $(TGT).$(MANSECT)
# Directories.
# Includes.
INCFLAGS = -I/usr/X11R6/include
# Dynamic/static libraries.
LIBFLAGS = -L$(PREFIX)/lib -lX11
# CPPreprocessor.
CPPFLAGS = -DVERSION=\"$(VERSION)\"
# Warnings.
WRNFLAGS =
# Optimiziation flags.
OPTFLAGS = -O3
# Compilation Flags.
DBGFLAGS = -g $(INCFLAGS) -O0         -Wall       $(DEFFLAGS) -DDBG
CFLAGS   =    $(INCFLAGS) $(OPTFLAGS) $(WRNFLAGS) $(DEFFLAGS)
LDFLAGS  = $(LIBFLAGS) 

# Compiler and linker.
CC = tcc
LD = $(CC)
