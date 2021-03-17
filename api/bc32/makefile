O           = obj
X           = .exe
L           = lib
V           = -v
SYSVERS     = bc32
DEFS        = -DSCRPROTO -DDOS -DDOSWIN -DDOSW32 -DREALD -DWINDOWS -DIODEWIN -DNOEMS -DNOEMF

!include ../dirs.mak

COMP      = bcc32 $(V) -5 -a1 -W -O -c -w-rvl -w-pro -w- -I$(SCRINCDIR);$(IODEINCDIR);$(TEECHART)\include $(DEFS)
COMPLINK  = bcc32 $(V) -W -L$(SCRLIBDIR);$(IODELIBDIR) $(BCCWARN) $(ALLOCDOCON) $(TYPESQLODBC) $(CODEGEAR) -e$@
COMPLINKW = bcc32 $(V) -tW -L$(SCRLIBDIR);$(IODELIBDIR) $(ALLOCDOCON) -e$@

MKLIB    = tlib /C /P256 /a $@
RC1      = brc32 -r
SDEFS    = -D DOS DOSWIN DOSW32
TANSI    = -t ansi

{..}.c.obj:
    $(COMP) $<
    
{..}.c.exe :
    $(COMPLINK) $<

{..}.rc.res :
    $(RC1) $< -fo$(*B).res

!include ../macros.mak
