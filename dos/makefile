#####################################################################
#          PARTIE A ADAPTER EN FONCTION DU SYSTEME CIBLE             #
######################################################################
#
#   DOS : make          : dos 16 bits
#         make -DDOSWIN : WIN 16 bits
#         make -DDOSW32 : WIN 32 bits
#         make -DWC     : DOS 32 bits (Watcom)
#         make -DSCRW32 : WIN32 (Borland)
#
#   UNIX : 1. Comment the DOS SECTION (70 lines)
#          2. Uncomment the target system section (4-5 lines)
#
############################## DOS - START ######################################
#
# --------- COMMON AND DEFAULT DEFINITIONS FOR DOS ----------
EMV     = -copy
RM      = -del
MV      = copy
X       = .exe
FO      = -o$*.$(O)
OBJ     = obj
L       = lib
RANLIB  = echo
CHMOD   = echo
SCRDIR  = c:\scr
IODEDIR = c:\usr\iode_src\api
CHARTDIR = c:\usr\iode_src\chart
AR      = $(SCRDIR)\bin\scr4_ar
APP     = $(SCRDIR)\bin\scr4w_app
STRIP   = echo
RC1     = echo
RC2     = echo
RES     =
MAIN    = std
CP      = copy
## -------- SPECIFIC DEFINITIONS FOR WIN 32 BITS / REAL = DOUBLE  ----------
TEECHART = c:\usr\TeeChart
SYS     = DOSW32
O       = obj
LB      = lib
FO      = -o$*.$(O)
CCDIR   = $(BCB)
X       = .exe
SCRE    = $(SCRDIR)\bin\scr4w_e -t ansi -D DOSW32D
SCRO    = s32wo.lib s32widb.lib
GLIBS   =
LNK     = bcc32 -v -W -M -L$(CCDIR)\L;$(SCRDIR)\l -ea.out
LNKNW   = bcc32 -v -M -L$(CCDIR)\L;$(SCRDIR)\l -ea.out
CC      = bcc32 -v -a1 -W -I$(SCRDIR)\i;$(IODEDIR);.;$(TEECHART)\include -DDOS -DDOSW32 -DREALD -DSCRPROTO -w-rvl -O -c -w-pro -w-
BCBLNK  = bcc32 -W -M -L$(SCRDIR)\l;$(IODEDIR);$(TEECHART)\Lib;$(BCB)\Lib\Release -ea.out
CCDEV   = echo
RC1     = brc32 -r -ic:\bc5\i
RC2     = brc32
RES     = iode.res
RC      = iode.rc

####(127 lines)################ DOS - END #################################

######################################################################
#                       GLOBAL DEFINITIONS                           #
######################################################################

AR1     = -$(AR) $(SYS) 1
AR2     = -$(AR) $(SYS) 2
AR3     = -$(AR) $(SYS) 3

STD     = stdexec.$(O) odelinux.$(O)

SBRICK1 = sb_file.$(O) sb_rep.$(O) sb_xode.$(O) sb_global.$(O) sb_gui.$(O)
SBRICK2 = sb_pdest.$(O) sb_print.$(O) sb_view.$(O) sb_model.$(O) \
	sb_idt.$(O) sb_est.$(O)
SBRICK3 = sb_data.$(O) sb_ws.$(O) sb_prof.$(O)

SBRICKS = $(SBRICK1) $(SBRICK2)  $(SBRICK3)

ODE2    = o_edit.$(O) o_misc.$(O) o_miscv.$(O) o_scroll.$(O) o_scr.$(O) k_lang.$(O)
ODE3    = o_tab.$(O) o_vt.$(O) o_est.$(O) o_gr.$(O) o_abort.$(O) 
ODE     = $(ODE2) $(ODE3)

LIBS    = ode.$(L)
###############################################################
#                   MAIN DEPENDENCIES                         #
###############################################################

all      : iode$(X)

###############################################################
#                       ALIASES                               #
###############################################################

iode        :   $(EXEDIR)iode$(X)

#############################################################
##                      LIBS FILES                         ##
#############################################################

iodedos.$(LB) : $(SBRICKS) $(ODE2) $(ODE3) #$(IODEDOS)
	$(RM)     iodedos.$(L)
	$(RM)     iodedos.$(LB)
	$(AR1)    iodedos.$(L) $(SBRICK1)
	$(AR2)    iodedos.$(L) $(SBRICK2)
	$(AR2)    iodedos.$(L) $(SBRICK3)
    $(AR2)    iodedos.$(L) $(ODE2)
	$(AR2)    iodedos.$(L) $(ODE3)
	$(AR3)    iodedos.$(L)
	$(RANLIB) iodedos.$(L)
#       $(EMV)    iodedos.$(L) iodedos.$(LB)

#############################################################
#                       EXECUTABLES                         #
#############################################################

o_objs.c : ode.f odefile.f common.f odeprof.f odews.f odedata.f \
	odecomp.f odeprint.f oderep.f odetsf.f odexode.f scr.f
	$(SCRE) -af ode -o o_objs -I $(IODEDIR)\iode.h -1obj -noobj -w iodew95 -actmem -optmem

ode$(MAIN).c : ode.c
	touch ode$(MAIN).c

$(RES) : o_objs.c $(RC)
	$(RC1) $(RC)

CFLAG1 = -Ic:\scr\i;$(TEECHART)\Include @cpp.mak

.c.$(O):
	$(CC) $(FO) $*.c

.c.obj:
	$(CC) $*.c

.cpp.obj:
	bcc32 $(CFLAG1) $*.cpp

chart.cpp : $(CHARTDIR)\chart.dfm $(CHARTDIR)\chart.cpp
	copy $(CHARTDIR)\chart.cpp chart.cpp
	copy $(CHARTDIR)\chart.h chart.h
	copy $(CHARTDIR)\chart.dfm chart.dfm

#pref.cpp : pref.dfm
#       touch pref.cpp

#debug.cpp : debug.dfm
#       touch debug.cpp

## Unsigned version => append .scr .cmt and iode.msg to iode.exe
#$(EXEDIR)iode.exe : o_objs.c $(RES) iodedos.$(LB) s_iode.$(O) k_a2mrtf.$(O) ode$(MAIN).c o_objs.$(O) ode$(MAIN).$(O) \
#	o_help.$(OBJ) chart.$(O) $(SCRDIR)\l\scr4iode.lib $(SCRDIR)\l\s32wo.lib $(SCRDIR)\l\s32widb.lib ..\api\iodeapi.lib
#	ilink32 -v -aa -Tpe -Gn -Lc:\scr\l;$(IODEDIR);$(TEECHART)\Lib;$(CHARTDIR) ode$(MAIN).$(OBJ) o_help.$(OBJ) @ilink32onew.mak
##        $(APP) iode$(X) o_objs.scr
##        $(APP) iode$(X) iode.msg
##        $(APP) iode$(X) o_objs.cmt

## Signed version => create iode.scr with .scr .cmt and iode.msg 
$(EXEDIR)iode.exe : $(EXEDIR)iode.scr o_objs.c $(RES) iodedos.$(LB) s_iode.$(O) k_a2mrtf.$(O) ode$(MAIN).c o_objs.$(O) ode$(MAIN).$(O) \
	o_help.$(OBJ) chart.$(O) $(SCRDIR)\l\scr4iode.lib $(SCRDIR)\l\s32wo.lib $(SCRDIR)\l\s32widb.lib ..\api\iodeapi.lib
	ilink32 -v -aa -Tpe -Gn -Lc:\scr\l;$(IODEDIR);$(TEECHART)\Lib;$(CHARTDIR) ode$(MAIN).$(OBJ) o_help.$(OBJ) @ilink32onew.mak
#	DigiCertUtil.exe sign /noinput /sha1 "c8c28b4300e274f9cf437244f913cc99bfef109b" "iode.exe"
	DigiCertUtil.exe sign /noinput /sha1 "307c80ca0c69098522a09f40a1299e7c9c32ec85" "iode.exe"
$(EXEDIR)iode.scr : o_objs.c o_objs.scr o_objs.cmt iode.msg
	$(RM) iode.scr
	touch iode.scr
	$(APP) iode.scr o_objs.scr 
    $(APP) iode.scr iode.msg
    $(APP) iode.scr o_objs.cmt
	
wikiiode = \\192.168.10.11\c$\data\apache-docs\html\w-iode
iodeversion : 
		copy version.txt $(wikiiode)
