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
AR      = $(SCRDIR)\bin\scr4_ar
APP     = $(SCRDIR)\bin\scr4w_app
CP      = copy

## -------- SPECIFIC DEFINITIONS FOR WIN 32 BITS / REAL = DOUBLE  ----------
TEECHART = c:\usr\TeeChart
SYS     = DOSW32
O       = obj
FO      = -o$*.$(O)

## suppression de la référence au dir de Embarcadero car intégré par défaut dans bcc32
BCBINCLDIR =
BCBLINKDIR =
X    = .exe
LNK  = bcc32 -v -M -L$(BCBINCLDIR);$(SCRDIR)\l -ea.out
CC   = bcc32 -v -a1 -5 -W -I$(SCRDIR)\i;$(IODEDIR);$(TEECHART)\include -DDOS -DDOSWIN -DWINDOWS -DDOSW32 -DREALD -DSCRPROTO -DIODEWIN -DNOEMS -DNOEMF -w-rvl -O -c -w-pro -w-

.c.$(O):
	$(CC) $(FO) $*.c


iodecmd.exe : iodecmd.$(O) iodecmd.scr $(IODEDIR)\iodeapi.$(L) s_iode.$(O) sb_gui.$(O) k_errorv.$(O) $(SCRDIR)\l\scr4iode.$(L)
	$(LNK) -v iodecmd.$(O) s_iode.$(O) sb_gui.$(O) k_errorv.$(O) $(IODEDIR)\iodeapi.$(L) scr4iode.$(L) odbc32.lib
#	$(APP) a.out ..\dos\iode.msg
 	$(MV) a.out iodecmd$(X)
	$(MV) a.tds iodecmd.tds
	DigiCertUtil.exe sign /noinput /sha1 "c8c28b4300e274f9cf437244f913cc99bfef109b" "iodecmd.exe"

iodecmd.scr : ..\dos\iode.msg
	$(RM) iodecmd.scr
	touch iodecmd.scr
	$(APP) iodecmd.scr ..\dos\iode.msg
