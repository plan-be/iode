O           = obj
X           = .exe
L           = lib
V           = -v
SYSVERS     = vc32
DEFS        = -DDOS -DDOSWIN -DDOSW32 -DREALD -DWINDOWS -DSCRPROTO -DIODEWIN -DNOEMS -DNOEMF

!include ../dirs.mak

COMP      = cl -Zi -Zp1 -c -I$(SCRINCDIR) -I$(IODEINCDIR)  $(DEFS) -DVC $(ALLOCDOCON) $(TYPESQLODBC)
COMPLINK  = cl -Zi -Zp1 -I$(SCRINCDIR) -I$(IODEINCDIR)  $(DEFS) -DVC $(ALLOCDOCON) $(TYPESQLODBC) ws2_32.lib gdi32.lib user32.lib advapi32.lib shell32.lib comdlg32.lib comctl32.lib  Winspool.lib legacy_stdio_definitions.lib /Fe:$@
COMPLINKW = cl -Zi -Zp1 -I$(SCRINCDIR) -I$(IODEINCDIR)  $(DEFS) -DVC $(ALLOCDOCON) $(TYPESQLODBC) ws2_32.lib gdi32.lib user32.lib advapi32.lib shell32.lib comdlg32.lib comctl32.lib Winspool.lib legacy_stdio_definitions.lib /Fe:$@ /link /subsystem:windows

MKLIB     = lib /OUT:$@
LINK      = link 
RC1       = rc 

{..}.c.obj:
    $(COMP) $<
    
{..}.c.exe :
    $(COMPLINK) $<

{..}.rc.res :
    $(RC1) $< -fo$(*B).res

!include ../macros.mak
