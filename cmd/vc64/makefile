O           = obj
X           = .exe
L           = lib
V           = -v
SYSVERS     = vc64
DEFS        = -DDOS -DDOSWIN -DDOSW32 -DREALD -DWINDOWS -DSCRPROTO -DIODEWIN -DNOEMS -DNOEMF

!include ../../dirs.mak

COMP      = cl -DWINDOWS_IGNORE_PACKING_MISMATCH -Zi -Zp1 -c -I$(SCRINCDIR) -I$(IODEINCDIR)  $(DEFS) -DVC $(ALLOCDOCON) $(TYPESQLODBC)
COMPLINK  = cl -Zi -Zp1 -I.. $(TYPESQLODBC) ws2_32.lib gdi32.lib user32.lib advapi32.lib shell32.lib comdlg32.lib comctl32.lib  Winspool.lib legacy_stdio_definitions.lib /Fe:$@
COMPLINKW = cl -Zi -Zp1 -I.. $(TYPESQLODBC) ws2_32.lib gdi32.lib user32.lib advapi32.lib shell32.lib comdlg32.lib comctl32.lib Winspool.lib legacy_stdio_definitions.lib /Fe:$@ /link /subsystem:windows
LINK      = link 
MKLIB     = lib /OUT:$@
RC1       = rc 
ODBCLIBS = odbc32.lib odbccp32.lib 

{..}.c.obj:
    $(COMP) $<
    
{..}.c.exe :
    $(COMPLINK) $<

{..}.rc.res :
    $(RC1) $< -fo$(*B).res

!include ../macros.mak
