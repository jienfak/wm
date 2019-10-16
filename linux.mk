# Configuration for GNU/Linux systems.
# Standard programs.
RM      = rm -rfv
INSTALL = install -v
TAR     = tar -vcf
GZIP    = gzip -vf
MKDIR   = mkdir -pv
CP      = cp -vR
STRIP   = strip -v
ECHO    = echo
SED     = sed
DATE    = date
MAKE    = make
# Flags.
OUTFLAG = -o
OBJFLAG = -c
# Extensions.
OBJEXT  = .o
CEXT    = .c
GZEXT   = .gz
TAREXT  = .tar
EXEEXT  = 
# Directories and prefixes.
ROOTDIR       = 
DIRPREFIX     = usr/local
BINPREFIX     = bin
DESTDIR       = $(ROOTDIR)/$(DIRPREFIX)
# For programs with manual configuration.
MANPREFIX     = share/man
MANSTDSECT    = 1
MANSECTPREFIX = man$(MANSTDSECT)
MANUAL        = $(PROGNAME).$(MANSTDSECT)
PATH2MAN      = $(DESTDIR)/$(MANPREFIX)/$(MANSECTPREFIX)/$(MANUAL)
