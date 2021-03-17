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
X       = .exe
LNK     = bcc32 -v -W -M -L$(BCBLINKDIR);$(SCRDIR)\l -ea.out
LNKNW   = bcc32 -v -M -L$(BCBINCLDIR);$(SCRDIR)\l -ea.out
CC      = bcc32 -v -a1 -5 -W -I$(SCRDIR)\i;$(TEECHART)\include -DDOS -DDOSWIN -DWINDOWS -DDOSW32 -DREALD -DSCRPROTO -DIODEWIN -DNOEMS -DNOEMF -w-rvl -O -c -w-pro -w-

all : iodeapi.lib

CFLAG1 = -I$(BCBINCLDIR);c:\scr\i;$(TEECHART)\Include;$(BCBINCLDIR) @cpp.mak

.c.$(O):
	$(CC) $(FO) $*.c

########### IODEAPI ###########################
AR1     = -$(AR) $(SYS) 1
AR2     = -$(AR) $(SYS) 2
AR3     = -$(AR) $(SYS) 3

API1 =   k_ws.$(O) k_pack.$(O) k_type.$(O) k_cmp.$(O) k_vers.$(O)
API2 =   k_lec.$(O) pack.$(O) k_descr.$(O) objs.$(O) buf.$(O) objsv.$(O)  per.$(O)
API3 =   yy.$(O) k_xdr.$(O) l_token.$(O) l_cc1.$(O) l_alloc.$(O) l_err.$(O) l_vars.$(O)
API4 =   l_cc2.$(O) l_link.$(O) l_exec.$(O) l_eqs.$(O) l_newton.$(O) l_debug.$(O)
API5 =   l_secant.$(O) e_est.$(O) e_step.$(O) e_tests.$(O) e_prep.$(O)
API6 =   e_error.$(O) e_errorv.$(O) e_print.$(O) e_stat.$(O) k_var.$(O) k_val.$(O)
API7 =   k_lst.$(O) k_eqs.$(O) k_tbl.$(O) k_est.$(O) k_ccvar.$(O)
API8 =   k_cccmt.$(O) k_ccscl.$(O) k_ccidt.$(O) k_cclst.$(O) k_cceqs.$(O)
API9 =   k_cctbl.$(O) k_iprn.$(O) k_imain.$(O) k_iasc.$(O) k_lang.$(O)
API10=   k_irasc.$(O) k_idif.$(O) k_ibst.$(O) k_inis.$(O) k_itxt.$(O) k_igem.$(O)
API11=   k_emain.$(O) k_wks.$(O) k_ecsv.$(O) k_edif.$(O) k_etsp.$(O)
API12=   k_ewks.$(O) c_cc.$(O) c_calc.$(O) b_base.$(O) b_dir.$(O)
API13=   b_file.$(O) b_fdel.$(O) b_fcopy.$(O) b_fren.$(O)
API14=   b_rep.$(O) b_rep2.$(O) b_proc.$(O) b_defs.$(O) b_data.$(O) b_ws.$(O) b_prof.$(O) b_ras.$(O) b_eviews.$(O)
API15=   b_readme.$(O) b_pdest.$(O) b_pnogui.$(O) b_model.$(O) b_print.$(O) b_view.$(O)
API16=   b_idt.$(O) b_est.$(O) b_htol.$(O) b_ltoh.$(O) b_xode.$(O) b_api.$(O)
API17=   b_season.$(O) b_trend.$(O) k_sim.$(O) k_exec.$(O) k_print.$(O)
API18=   k_graph.$(O) k_grep.$(O) b_dde.$(O) b_basev.$(O) b_sql.$(O) b_ds.$(O)
API19=   w_wrt.$(O) w_wrtv.$(O) w_wrt1.$(O)
API20=  odelinux.$(O) l_rand.$(O) m_debug.$(O) nochart.obj
API = $(API1) $(API2) $(API3) $(API4) $(API5) $(API6) $(API7) $(API8) $(API9) \
      $(API10) $(API11) $(API12) $(API13) $(API14) $(API15) $(API16) $(API17) $(API18) $(API19) $(API20)

iodeapi.$(L) :$(API1) $(API2) $(API3) $(API4) $(API5) $(API6) $(API7) $(API8) $(API9) \
	  $(API10) $(API11) $(API12) $(API13) $(API14) $(API15) $(API16) $(API17) $(API18) $(API19) $(API20)
	$(RM)     iodeapi.$(L)
	$(AR1)    iodeapi.$(L) $(API1)
	$(AR2)    iodeapi.$(L) $(API2)
	$(AR2)    iodeapi.$(L) $(API3)
	$(AR2)    iodeapi.$(L) $(API4)
	$(AR2)    iodeapi.$(L) $(API5)
	$(AR2)    iodeapi.$(L) $(API6)
	$(AR2)    iodeapi.$(L) $(API7)
	$(AR2)    iodeapi.$(L) $(API8)
	$(AR2)    iodeapi.$(L) $(API9)
	$(AR2)    iodeapi.$(L) $(API10)
	$(AR2)    iodeapi.$(L) $(API11)
	$(AR2)    iodeapi.$(L) $(API12)
	$(AR2)    iodeapi.$(L) $(API13)
	$(AR2)    iodeapi.$(L) $(API14)
	$(AR2)    iodeapi.$(L) $(API15)
	$(AR2)    iodeapi.$(L) $(API16)
	$(AR2)    iodeapi.$(L) $(API17)
	$(AR2)    iodeapi.$(L) $(API18)
	$(AR2)    iodeapi.$(L) $(API19)
	$(AR2)    iodeapi.$(L) $(API20)
	$(AR3)    iodeapi.$(L)
	$(RANLIB) iodeapi.$(L)


eviews.exe : b_eviews.obj
	bcc32 -v -Lc:\scr\l -eeviews.exe b_eviews.obj iodeapi.lib scr4iode.lib
