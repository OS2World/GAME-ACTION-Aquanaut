#******************************************************************************
#  Dot directive definition area (usually just suffixes)
#******************************************************************************

.SUFFIXES:
.SUFFIXES: .c .asm .obj .lst .def .lnk .exe .h .inc

#
# Conditional Setup Area and User Defines Macros
#

DEBUG=

!ifndef LANG
LANG=us
!endif

#
# Compiler Location w/ includes, libs and tools
#
# HOME=1
!ifdef HOME
ROOT=E:
TOOLSPATH=$(ROOT)\r207\tools
!else
ROOT=D:
TOOLSPATH=$(ROOT)\tools
!endif

#
# CSET/2 specific defines
#
!ifdef HOME
LOCALE  =$(ROOT)\ibmc\clocale
IBMCINC =$(ROOT)\ibmh
IBMCLIB =$(ROOT)\ibmc\clib
IBMCBIN =$(ROOT)\ibmc\cbin
IBMCHELP=$(ROOT)\ibmc\chelp
!else
LOCALE  =$(ROOT)\ibmc\locale
IBMCINC =$(ROOT)\ibmc\include
IBMCLIB =$(ROOT)\ibmc\lib
IBMCBIN =$(ROOT)\ibmc\bin
IBMCHELP=$(ROOT)\ibmc\help
!endif

MMPM2   = $(ROOT)\MMPM21TK
CINC    = -I. -I.. -I$(IBMH) -I$(H386) -I$(H)
LIB386  = os2386.lib
LIBDDE  = $(IBMCLIB)\dde4mbs
LIBS    = $(LIB386) $(LIBDDE)
DPATH   = $(LOCALE);$(IBMCHELP);

!ifdef DEBUG
OBJEXT  = obd
!else
OBJEXT  = obj
!endif

#
# Since the compiler/linker and other tools use environment
# variables ( INCLUDE, LIB, etc ) in order to get the location of files,
# the following line will check the environment for the LIFE of the
# makefile and will be specific to this set of instructions. All MAKEFILES
# are requested to use this format to insure that they are using the correct
# level of files and tools.
#

#!if [SET HELP=$(IBMCHELP)] || \
#    [SET LIB=$(IBMCLIB);$(LIB)] || \
#    [SET PATH=$(IBMCBIN);$(TOOLSPATH)] || \
#    [SET INCLUDE=$(IBMCINC);$(IBMH);$(H386);$(H);$(INC);..\]
#!endif


#
# Compiler/tools Macros
#

CC=     icc
LINK=   link386

#
# Compiler and Linker Options
#
CTARG      = /G4s
COMPILE    = /C+ /W3 /Ki- /Kb- /Ss+ /Gd- /Ms
#DEBUG      = /Ti+ /O-
DEBUG      = /Ti- /O+
CM_THREAD  = /Gm+

CFLAGS  = $(COMPILE) $(CTARG) $(CM_THREAD) $(DEBUG)
LFLAGS  = /nod /noe /de


all: aquanaut.exe aquanaut.hlp

aquanaut.exe : def.obj defmove.obj defgbls.obj defcmnd.obj defsound.obj \
          def.def def.res makefile
 $(LINK) def.obj defmove.obj defgbls.obj defcmnd.obj defsound.obj, \
 					aquanaut.exe, $(LFLAGS) /ST:36000, $(LIBS), def.def
 rc def.res aquanaut.exe

aquanaut.hlp : aquanaut.ipf
 ipfc aquanaut.ipf
 copy aquanaut.hlp c:\os2\help

def.obj : def.c def.h defid.h defproto.h deftypes.h defgbls.h defmacro.h \
		  makefile
 $(CC) $(CFLAGS) def.c

defgbls.obj : defgbls.c def.h defid.h deftypes.h makefile
 $(CC) $(CFLAGS) defgbls.c

defcmnd.obj : defcmnd.c def.h defid.h deftypes.h makefile
 $(CC) $(CFLAGS) defcmnd.c

defmove.obj : defmove.c def.h defid.h defproto.h deftypes.h defgbls.h \
			  defmacro.h makefile
 $(CC) $(CFLAGS) defmove.c

defsound.obj: defsound.c deftypes.h defgbls.h makefile
 $(CC) $(CFLAGS) defsound.c

# Need to list all the bitmaps here....
def.res : def.rc defid.h makefile \
              DEF.ICO \
              SMILE.ICO \
              SOB.ICO \
			  MUTHORSL.BMP \
			  MUTHORSR.BMP \
			  HUMAN.BMP \
			  SHIPR.BMP \
			  SHIPL.BMP \
			  FLAMEL0.BMP \
			  FLAMEL1.BMP \
			  JELLY1.BMP \
			  JELLY2.BMP \
			  JELLY3.BMP \
			  SWARMER.BMP \
			  SKIMMERL.BMP \
			  SKIMMERR.BMP \
			  BOMBERL.BMP \
			  BOMBERR.BMP \
			  BULLET.BMP \
			  BOMB.BMP \
			  FLAMEL2.BMP \
			  SEAHORSL.BMP \
			  SEAHORSR.BMP \
			  SEAHORSC.BMP \
			  N8.BMP \
			  N3.BMP \
			  N5.BMP \
			  N2.BMP \
			  N9.BMP \
			  N7.BMP \
			  N6.BMP \
			  N0.BMP \
			  N1.BMP \
			  N4.BMP \
			  SCORE.BMP \
			  FLAMEL3.BMP \
			  FLAMER3.BMP \
			  FLAMER0.BMP \
			  FLAMER1.BMP \
			  FLAMER2.BMP \
			  SMART.BMP \
			  HISCORE.BMP \
			  LIFE.BMP \
			  CLRWHITE.BMP \
			  CLRBLACK.BMP \
			  CLRGREEN.BMP \
			  CLRPINK.BMP \
			  CLRYELLO.BMP \
			  CLRPGRAY.BMP \
			  CLRRED.BMP \
			  CLRCYAN.BMP \
			  LOGO.BMP \
			  ERASE.BMP \
			  BUBBLE.BMP \
			  MERMAID1.BMP \
			  MERMAID2.BMP \
			  MERMAID3.BMP \
			  MERMAID4.BMP \
			  MERMAID5.BMP \
			  RING.BMP \
			  RINGN.BMP \
			  RINGS.BMP \
			  RINGE.BMP \
			  RINGW.BMP \
			  RINGNW.BMP \
			  RINGNE.BMP \
			  RINGSW.BMP \
			  RINGSE.BMP \
			  SHIPINVL.BMP \
			  SHIPINVR.BMP
   rc -r def


#******************************************************************************
#   Setup the inference rules for compiling and assembling source code to
#   object code.
#******************************************************************************

.asm.obj:
        $(ASM) $(AFLAGS) $(AINC) $*.asm,$@;

.asm.obd:
        $(ASM) $(AFLAGS) -DDEBUG $(AINC) $*.asm,$@;

.asm.lst:
        $(ASM) -l -n $(AFLAGS) $(AINC) $*.asm,nul,$@;

.c.i:
        $(CC) $(CFLAGS) /P+ $*.c

.c.obj:
        $(CC) $(CFLAGS) /Fo$@ $*.c

.c.obd:
        $(CC) $(CFLAGS) /DDEBUG /Fo$@ $*.c

.c.cod:
        $(CC) $(CFLAGS) /Fo$*.obj /Fa$@ $*.c

.h.inc:
        tabify -d $*.h | sed -f $(TOOLSPATH)\h2inc32.sed | tabify >$*.inc




