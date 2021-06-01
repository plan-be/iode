### ATTENTION : execute \scr\set64.bat to set the correct paths / compiler for 64 bits VC ###

sys    = 
O      = obj
X      = .exe
L      = lib

incdir = c:\scr\i

#COMP     = cl     -Zp1 -c -I$(incdir) -I.. -DDOS -DDOSWIN -DWINDOWS -DDOSW32 -DREALD -DSCRPROTO -DIODEWIN -DNOEMS -DNOEMF -DVC $(ALLOCDOCON) $(TYPESQLODBC)
#COMPLINK = cl     -Zp1 -I$(incdir) -I.. -DDOS -DDOSWIN -DWINDOWS -DDOSW32 -DREALD -DSCRPROTO -DIODEWIN -DNOEMS -DNOEMF -DVC $(ALLOCDOCON) $(TYPESQLODBC) /Fe:
#LINK     = link /MACHINE:X64
#MKLIB    = lib /MACHINE:X64 /OUT:

COMP      = cl -DWINDOWS_IGNORE_PACKING_MISMATCH  -Zi -Zp1 -c -I$(incdir) -I.. -DDOS -DDOSWIN -DWINDOWS -DDOSW32 -DREALD -DSCRPROTO -DIODEWIN -DNOEMS -DNOEMF -DVC $(ALLOCDOCON) $(TYPESQLODBC)
COMPLINK  = cl -Zi -Zp1 -I.. $(TYPESQLODBC) ws2_32.lib gdi32.lib user32.lib advapi32.lib shell32.lib comdlg32.lib comctl32.lib  Winspool.lib legacy_stdio_definitions.lib /Fe:$@
COMPLINKW = cl -Zi -Zp1 -I.. $(TYPESQLODBC) ws2_32.lib gdi32.lib user32.lib advapi32.lib shell32.lib comdlg32.lib comctl32.lib  Winspool.lib legacy_stdio_definitions.lib /Fe:$@ /link /subsystem:windows /MACHINE:X64

#COMP      = cl -Zi -c -I$(incdir) -I.. -DDOS -DDOSWIN -DWINDOWS -DDOSW32 -DREALD -DSCRPROTO -DIODEWIN -DNOEMS -DNOEMF -DVC $(ALLOCDOCON) $(TYPESQLODBC)
#COMPLINK  = cl -Zi -I.. $(TYPESQLODBC) ws2_32.lib gdi32.lib user32.lib advapi32.lib shell32.lib comdlg32.lib comctl32.lib  Winspool.lib legacy_stdio_definitions.lib /Fe:$@
#COMPLINKW = cl -Zi -I.. $(TYPESQLODBC) ws2_32.lib gdi32.lib user32.lib advapi32.lib shell32.lib comdlg32.lib comctl32.lib  Winspool.lib legacy_stdio_definitions.lib /Fe:$@ /link /subsystem:windows /MACHINE:X64


LINK     = link /MACHINE:X64
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
  
all:  $(s4libs) $(s4exes)
      
tests : 
     echo $(SCRDI)    > s4di.txt
     echo $(SCRWI)    > s4wi.txt
     echo $(SCRDIDB)  > s4idb.txt
     echo $(SCRDICL)  > s4icl.txt
     echo $(BASE) $(RUNO) $(SCROLL) $(YY) $(LST) $(MMT)          > s4do.txt
     echo $(SWAP1) $(SWAP2) $(ARGS)                              >> s4do.txt
     echo $(RUNA) $(DIR1) $(DIR2)                                >> s4do.txt
     echo $(CPP) $(A2M)                                          >> s4do.txt
     echo $(XDR) $(VF) $(CALC)                                   >> s4do.txt
     echo $(MAT) $(TAR1) $(TAR2)                                 >> s4do.txt
     echo $(LZH) $(FILE)  $(INI) $(GIF) $(HTML) $(GZIP) $(ISC)   >> s4do.txt
     echo $(YY) $(LST) $(SWAP1) $(SWAP2) $(XDR) $(ARGS)                          >> s4wo.txt       
     echo $(MAT) $(DIR1) $(DIR2) $(LZH) $(SW_WIN) $(SW_STRS) $(SW_DATE)          >> s4wo.txt
     echo $(SW_BASE) $(SW_IMAGE) $(INI) $(SW_CPP) $(SW_HTML) $(SW_A2M) $(SW_FILE)>> s4wo.txt 
     echo $(SW_GIF) $(SW_GZIP) $(SW_INF) $(SW_ISC)                               >> s4wo.txt
     echo $(MAKEDIR) 
      