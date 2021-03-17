sys    = 
O      = obj
X      = .exe
L      = lib

incdir = c:\scr\i

COMP     = cl -Zi -Zp1 -c -I$(incdir) -I.. -DDOS -DDOSWIN -DWINDOWS -DDOSW32 -DREALD -DSCRPROTO -DIODEWIN -DNOEMS -DNOEMF -DVC $(ALLOCDOCON) $(TYPESQLODBC)
COMPLINK = cl -Zi -Zp1 -I.. $(TYPESQLODBC) ws2_32.lib gdi32.lib user32.lib advapi32.lib shell32.lib comdlg32.lib comctl32.lib  Winspool.lib legacy_stdio_definitions.lib /Fe:$@
COMPLINKW = cl -Zi -Zp1 -I.. $(TYPESQLODBC) ws2_32.lib gdi32.lib user32.lib advapi32.lib shell32.lib comdlg32.lib comctl32.lib Winspool.lib legacy_stdio_definitions.lib /Fe:$@ /link /subsystem:windows
LINK     = link 
MKLIB    = lib /OUT:$@
RC1      = rc 
SDEFS    = -D DOS DOSWIN DOSW32
TANSI    = -t ansi
ODBCLIBS = odbc32.$(L) odbccp32.$(L) 

{..}.c.obj::
    $(COMP) $<

{..}.c.exe :
    $(COMPLINK) $< 

{..}.rc.res :
    $(RC1) /fo$(*B).res $<
    
!include ../macros.mak
  
all:  s4libs s4exes
