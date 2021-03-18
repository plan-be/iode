O           = obj
X           = .exe
L           = lib
V           = -v
SYSVERS     = bc32
DEFS        = -DSCRPROTO -DDOS -DDOSWIN -DDOSW32 -DREALD -DWINDOWS -DSCRPROTO -DIODEWIN -DNOEMS -DNOEMF

!include ../../dirs.mak

COMP      = bcc32 $(V) -a1 -W -O -c -w-use -w-par -w-pro -w-aus -I$(SCRINCDIR);$(IODEINCDIR) $(DEFS)
COMPLINK  = bcc32 $(V) -W -L$(SCRLIBDIR);$(IODELIBDIR) $(BCCWARN) $(ALLOCDOCON) $(TYPESQLODBC) $(CODEGEAR) -e$@
COMPLINKW = bcc32 $(V) -tW -L$(SCRLIBDIR);$(IODELIBDIR) $(ALLOCDOCON) -e$@

MKLIB    = tlib /C /P256 /a $@
RC1      = brc32 -r
SDEFS    = -D DOS DOSWIN DOSW32
TANSI    = -t ansi

ODBCLIBS = odbc32.lib #odbccp32.lib odbcbcp.lib

{..}.c.obj:
    $(COMP) $<
    
{..}.c.exe :
    $(COMPLINK) $<

{..}.rc.res :
    $(RC1) $< -fo$(*B).res

!include ../macros.mak
