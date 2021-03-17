# LIBRARIES 
# =========
# 1. FILE GROUPS
# --------------

# IODE API
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
      $(API10) $(API11) $(API12) $(API13) $(API14) $(API15) $(API16) $(API17) \
      $(API18) $(API19)  $(API20)

iodeapi.$(L) : $(API)
       -del $@
       $(MKLIB) $**