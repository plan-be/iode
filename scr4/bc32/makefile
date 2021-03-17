sys    = 
O      = obj
X      = .exe
L      = lib
V      = -v
#incdir  = c:\scr\i

COMP      = bcc32 $(V) -5 -a1 -W -O -c -w-use -w-par -w-pro -w-aus -I.. -DSCRPROTO -DDOS -DDOSWIN -DDOSW32 -DWINDOWS -DCodeGear -DBC551 $(BCCWARN) $(ALLOCDOCON) $(TYPESQLODBC) $(CODEGEAR) 
COMPLINK  = bcc32 $(V)     -L. $(BCCWARN) $(ALLOCDOCON) $(TYPESQLODBC) $(CODEGEAR) -e$@
COMPLINKW = bcc32 $(V) -tW -L. $(BCCWARN) $(ALLOCDOCON) $(TYPESQLODBC) $(CODEGEAR) -e$@

LINK     = ilink32 -c -Tpe -aa -Gn -L$(LIBDIR)\scr\L c0w32.obj
MKLIB    = tlib /C /P256 /a $@
RC1      = brc32 -r
RC2      = brc32
SDEFS    = -D DOS DOSWIN DOSW32
TANSI    = -t ansi



{..}.c.obj:
    $(COMP) $<
    
{..}.c.exe :
    $(COMPLINK) $<

{..}.rc.res :
    $(RC1) $< -fo$(*B).res

all:    s4libs s4exes

!include ../macros.mak
ODBCLIBS = odbc32.lib #odbccp32.lib odbcbcp.lib

#s4_mt$(X) : $(S4_MT_DEPS) $(S4_MT_OBJS)
#      $(LINK) $(S4_MT_OBJS),$@,,s4do.$(L) cw32mt import32,,s4_mt.res
#      # $(COMPLINK)$@ -tW $(S4_MT_OBJS) 
#       $(COMPLINK)$@ $(S4_MT_OBJS) s4_mt.res /link /subsystem:windows
#        s4_app s4_mt$(X) s_mtobjs.scr
#	s4_app s4_mt$(X) s_mtobjs.cmt