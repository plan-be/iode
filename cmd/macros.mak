# IODECMD
# =======

IODEMODULES = iodecmd.$(O)  s_iode.$(O) sb_gui.$(O) k_errorv.$(O) \
        $(IODEAPIDIR)\$(SYSVERS)\iodeapi.$(L) \
        $(SCRLIBDIR)\s4iode.$(L) 
#        $(SCRLIBDIR)\odbc32.$(L) 

iodecmd$(X) : $(IODEMODULES) $(IODEDOSDIR)\iode.msg
	$(COMPLINK) $(IODEMODULES) $(ODBCLIBS)
	$(SCRLIBDIR)\s4_app iodecmd$(X) $(IODEDOSDIR)\iode.msg
