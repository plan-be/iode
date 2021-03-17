# LIBRARIES 
# =========
# 1. FILE GROUPS
# --------------

# Strings
STRS1 = s_allc.$(O) s_memavl.$(O) s_panic.$(O)  s_stsqz.$(O)  s_ststrd.$(O) s_ststrp.$(O)
STRS2 = s_stacpy.$(O) s_stcmpr.$(O) s_stfmtl.$(O) s_stfmtt.$(O) s_stcvt.$(O) s_debug.$(O) s_iscwb.$(O)
STRS3 = s_stisal.$(O) s_stlcpy.$(O) s_stlfmt.$(O) s_stljus.$(O) s_stchch.$(O) s_sleep.$(O)
STRS4 = s_stlow.$(O)  s_stpad.$(O)  s_stpstr.$(O) s_strjus.$(O) s_stansi.$(O) s_stutf8.$(O) s_sterrc.$(O)
STRS5 = s_stucen.$(O) s_stufml.$(O) s_stufmt.$(O) s_stuind.$(O) s_stgetp.$(O) s_stfnam.$(O)
STRS6 = s_stuisn.$(O) s_stupos.$(O) s_stuppr.$(O) s_stushf.$(O) s_pcipar.$(O) s_pcinfo.$(O)
STRS7 = s_stusqz.$(O) s_stzstr.$(O) s_stvtom.$(O) s_stmtov.$(O) s_stgrep.$(O) s_strcmp.$(O) s_code64.$(O)
STRS8 = s_staddp.$(O) s_sttext.$(O) s_sttxtt.$(O) s_sterrt.$(O) s_stcrpt.$(O) s_stfstr.$(O)
STRS9 = s_stsrcf.$(O) s_stchex.$(O) s_stctex.$(O) s_stfmtd.$(O) s_stchk.$(O) s_strnat.$(O) s_prrep.$(O)
STRS10= s_stpsl.$(O) s_stpst.$(O) s_stun.$(O) s_stinte.$(O) s_stdiff.$(O) s_strdfl.$(O) s_vfld.$(O)
STRS11= s_strepl.$(O) s_strdln.$(O) s_strcpy.$(O) s_stfren.$(O) s_string.$(O) s_stobj.$(O)
STRS12= s_stpc0.$(O)  s_stpcin.$(O) s_stpcl.$(O) s_stpcex.$(O) s_stprf.$(O) s_ststr.$(O) s_isutil.$(O)
STRS13= s_stpak1.$(O) s_stpaka.$(O) s_stpakx.$(O) s_stpakl.$(O) s_std.$(O) s_copy.$(O) s_stdhex.$(O) s_sha1.$(O)
STRS = $(STRS1) $(STRS2) $(STRS3) $(STRS4) $(STRS5) \
	$(STRS6) $(STRS7) $(STRS8) $(STRS9) $(STRS10) \
	$(STRS11) $(STRS12) $(STRS13)

# Matrices
MAT1 = s_maallc.$(O) s_madup.$(O)  s_maerr.$(O)  s_maprod.$(O) s_matran.$(O)
MAT2 = s_mainv.$(O)  s_madet.$(O)  s_masol.$(O)  s_mamisc.$(O) s_maro.$(O)
MAT3 = s_matrac.$(O) s_macalc.$(O) s_makron.$(O) s_mascal.$(O) s_manull.$(O)
MAT4 = s_maextr.$(O) s_maxpx.$(O)  s_maxxp.$(O)  s_maxaxp.$(O) s_maxpax.$(O)
MAT5 = s_maprt.$(O)  s_madump.$(O) s_mascrl.$(O) s_maset.$(O) s_mastat.$(O) s_masol2.$(O)
MAT  = $(MAT1) $(MAT2) $(MAT3) $(MAT4) $(MAT5)

# Calc
CALC = s_calc.$(O) s_caltok.$(O) s_calcc.$(O) s_calerr.$(O) s_calexc.$(O)

# Var files
VF1  = s_vf.$(O)    s_vfhdl.$(O)  s_vfblk.$(O)  s_vferr.$(O)  s_vfanal.$(O)
VF2  = s_vfdel.$(O) s_vfcrea.$(O) s_vfmisc.$(O) s_vflock.$(O) s_vfreb.$(O)
VF   = $(VF1) $(VF2)

# Swap
SWAP1 = s_swallc.$(O) s_swfile.$(O) s_swab.$(O) s_swsegs.$(O)
SWAP2 = s_swems.$(O) s_swmisc.$(O) s_swvars.$(O)

# Lists
LST  = s_lst1.$(O) s_lst2.$(O)

# MMT Editor
MMT  = s_mmted.$(O) s_mmtfns.$(O) s_mmtcr.$(O) s_mmtint.$(O) s_mmtmn.$(O)

# XDR
XDR  = s_xdr.$(O)

# Dates
DATE = s_date.$(O) s_date_.$(O) s_dt.$(O) s_dtcal.$(O) s_dtchkd.$(O) s_dtchkt.$(O)

# Infinite arithmetic
INF  = s_infmis.$(O) s_infadd.$(O) s_infsub.$(O) s_infmul.$(O) s_infdiv.$(O)

# Tar files
TAR1 = s_tarc.$(O) s_tarx.$(O) s_tart.$(O) s_tario.$(O) s_tarfd.$(O)
TAR2 = s_tarh.$(O) s_tardft.$(O) s_tarerr.$(O)

# Compression
LZH = s_lzh.$(O) s_lzhf.$(O) s_lzhs.$(O)

# SCR Images
IMAGE1 = s_image.$(O) s_imwrt.$(O) s_immv.$(O) s_im2im.$(O) s_imfd.$(O)
IMAGE2 = s_imclr.$(O) s_imset.$(O) s_imattr.$(O) s_imallc.$(O)
IMAGE = $(IMAGE1) $(IMAGE2)

# Base for char interface
BASE11 = s_act.$(O) s_actsys.$(O) s_actcr.$(O) s_uact.$(O) s_menu.$(O) s_mnvarg.$(O) s_scrini.$(O)
BASE12 = s_mnch.$(O) s_cnf.$(O) s_cnfed.$(O) s_mnauto.$(O) s_cmtmn.$(O) s_wri.$(O) s_playb.$(O)
BASE21 = s_base.$(O) s_bastat.$(O) s_lock.$(O) s_bamini.$(O) s_caps.$(O) s_dispwd.$(O) s_baend.$(O) s_w32cch.$(O) s_wpsmem.$(O)
BASE22 = s_prtscr.$(O) s_box.$(O) s_box2.$(O) s_dispfn.$(O) s_disped.$(O) s_pgcro.$(O) s_grpbox.$(O) s_disp.$(O) 
BASE23 = s_dispfn2.$(O) s_dispfn3.$(O) s_dispfn4.$(O) s_dispfn5.$(O) s_dispfn6.$(O) 
BASE31 = s_edfl.$(O) s_edfld.$(O) s_edflt.$(O) s_edit.$(O) s_pgcnfo.$(O) s_prtstr.$(O) s_wsockc.$(O) s_pgk.$(O) s_webs.$(O) s_httpget.$(O)
BASE32 = s_edflz.$(O) s_edfmt.$(O) s_getkey.$(O) s_dumppg.$(O) s_pgcol.$(O) s_udfld.$(O) s_pghlp.$(O) s_mnfns.$(O) s_dired.$(O)
BASE4 = s_edmn.$(O) s_mnhlp.$(O) s_edpg.$(O) s_edtpd.$(O) s_env.$(O) s_err.$(O) s_derr.$(O) s_eded.$(O) s_wps.$(O) s_pgnext.$(O)
BASE5 = s_misc.$(O) s_pgcr.$(O) s_pginfo.$(O) s_pgfn.$(O) s_obj.$(O) s_pgadj.$(O) s_wdial.$(O) s_wtimer.$(O) s_wreck.$(O)
BASE51= s_objis.$(O) s_objtxt.$(O) s_objpg.$(O) s_drvget.$(O) s_pgcnf.$(O) s_wprraw.$(O) s_wsys.$(O)  s_wnet.$(O)  s_wvers.$(O)
BASE52= s_flt.$(O) s_fltnam.$(O) s_fltnu.$(O) s_fltfnd.$(O) s_fltfld.$(O) s_fltfix.$(O) s_wddecl.$(O) s_wclipb.$(O)
BASE53= s_fltexi.$(O) s_fltset.$(O) s_edbox.$(O) s_msgbox.$(O) s_keytxt.$(O) s_wddesr.$(O) s_wprocs.$(O) s_smtp.$(O) s_ftp.$(O)
BASE54= s_pgmmt.$(O) s_cmtbp.$(O) s_prof.$(O) s_atof.$(O) s_pgrset.$(O) s_wprt.$(O) s_wprt2.$(O) s_wsys2.$(O) s_wgetms.$(O) s_wdrive.$(O) s_pop3.$(O) # s_wmainh.$(O) 
BASE6 = s_vars.$(O) s_cfn.$(O) s_ab.$(O) s_abhlp.$(O) s_drw.$(O) s_dispf.$(O) s_anum.$(O) s_wtray.$(O) s_whid.$(O)
BASE7 = s_ap.$(O) s_apms.$(O) s_hlp.$(O) s_hlpstk.$(O) s_pgcmt.$(O) s_vers.$(O) s_magic.$(O) s_sys.$(O) s_flock.$(O) s_wsocks.$(O) s_whwndd.$(O)
BASE71= s_cmt.$(O) s_cmtdrw.$(O) s_cmtget.$(O) s_cmtrd.$(O) s_cmts.$(O) s_cmtsed.$(O) s_cmtpg.$(O) s_wuser.$(O)
BASE72= s_cmtscr.$(O) s_pgdft.$(O) s_pgempt.$(O) s_pgmod.$(O) s_pgfmt.$(O) s_pgedit.$(O) s_wprint.$(O) s_dsptch.$(O)
BASE8 = s_aberr.$(O) s_sbar.$(O)  s_edpwd.$(O) s_scincr.$(O) s_pgsld.$(O) s_wlock.$(O) s_sess.$(O) s_apkey.$(O)
BASE9 = s_prt.$(O) s_prmail.$(O) s_prfile.$(O) s_prlab.$(O) s_prlist.$(O) s_cgi.$(O) s_cgi2.$(O)
BASE91= s_prtdev.$(O) s_prtnam.$(O) s_prtsiz.$(O) s_prtpg.$(O) s_prtmv.$(O) s_wserv.$(O) s_wservu.$(O)
BASE92= s_prtatr.$(O) s_prttab.$(O) s_prtact.$(O) s_prterr.$(O) s_drv.$(O)
BASEPR= s_pgtp.$(O) s_pgip.$(O) s_pgcd.$(O) s_pgusr.$(O) s_reckey.$(O) s_pgkey.$(O)
BASE93= s_isfnk.$(O) s_iskey.$(O) s_pgedwd.$(O) s_pgask.$(O) s_rprdev.$(O)
BASE94= s_ccmtpg.$(O) s_ccmtmn.$(O) s_ccmtab.$(O) s_cmtall.$(O) s_prot.$(O) s_protv.$(O)
BASE0 = s_key.$(O) s_sn.$(O) s_ak.$(O) s_cak.$(O) s_uname.$(O) s_cvttab.$(O) s_dbconn.$(O)
BASE = $(BASE11) $(BASE12) $(BASE21) $(BASE22) $(BASE23) $(BASE31) \
	 $(BASE32)  $(BASE4) $(BASE5) \
	 $(BASE51) $(BASE52) $(BASE53)  $(BASE54) \
	 $(BASE6) $(BASE7) $(BASE71) $(BASE72) $(BASE8) $(BASE9) \
	 $(BASE91) $(BASE92) $(BASE94) $(BASEPR) $(BASE93) $(BASE0) \
	 $(STRS) $(DATE) $(IMAGE) $(INF)

# Base for char edition with parser         
RUNA1 = s_fnk.$(O)  s_ract.$(O)  s_rapp.$(O) s_rdbc.$(O) s_rdis.$(O) s_tbl.$(O)
RUNA2 = s_rdmn.$(O) s_rdmp.$(O)  s_rdpg.$(O) s_rdpr.$(O) s_rdrw.$(O) s_rdv.$(O) s_rdtp.$(O)
RUNA3 = s_runa.$(O) s_crobj.$(O) s_mp.$(O)   s_bc.$(O)   s_page.$(O) s_rdtb.$(O)
RUNA4 = s_fld.$(O)  s_rab.$(O)   s_rap.$(O)  s_rdre.$(O) s_rdsmp.$(O) s_rdwpos.$(O)
RUNA  = $(RUNA1) $(RUNA2) $(RUNA3) $(RUNA4)

# Base for char edition with compiled files (.scr, .cmt)
RUNO = s_robj.$(O) s_robjpr.$(O) s_objrd.$(O) s_cint.$(O)

# Executable Args 
ARGS1 = s_ainit.$(O) s_aerr.$(O) s_aexec.$(O) s_aexp.$(O) s_aiexp.$(O) s_acerr.$(O)
ARGS2 = s_aexps.$(O) s_aexpsd.$(O) s_acheck.$(O) s_ausage.$(O) s_achkin.$(O) s_achkac.$(O)
ARGS = $(ARGS1) $(ARGS2)

# YY
YY   = s_ybase.$(O) s_ydef.$(O) s_yincl.$(O) s_ylex.$(O) s_yread.$(O) s_ydebug.$(O) s_yerror.$(O)

# Grid (aka "Scroll")
SCROLL = s_scroll.$(O)

# Directory reading / file type
DIR1 = s_dirspn.$(O) s_dirspd.$(O) s_dirsta.$(O) s_dirrd.$(O) s_dirfmt.$(O) s_dirrp.$(O)
DIR2 = s_dirtbl.$(O) s_dirfp.$(O) s_dircwd.$(O) s_dirj.$(O) s_dirstd.$(O) s_chmod.$(O)
FILE = s_fiam.$(O) s_fiap.$(O) s_fiem.$(O) s_fifm.$(O) s_file.$(O) s_fism.$(O)

# Ini files read/write
INI  = s_rdini.$(O)

# HTML File contruction helper
HTML = s_html.$(O) s_htmlyy.$(O) s_htmltc.$(O) s_htmler.$(O) s_htmlsp.$(O) s_htmlsb.$(O)

# General File parser
CPP  = s_cpp.$(O) s_cppfns.$(O) s_cpperr.$(O)

# A2m Parser / translator
A2M1  = s_a2mrd.$(O) s_a2mseg.$(O) s_a2mhtm.$(O) s_a2mfrm.$(O) s_a2mfrg.$(O) s_a2mmsg.$(O) s_wemfcr.$(O) s_wemf.$(O)
A2M2  = s_a2mmem.$(O) s_a2memf.$(O) s_wprts.$(O) s_wprtp.$(O) s_a2mtc2.$(O) s_a2mtch.$(O) s_a2mdok.$(O)  s_a2mswf.$(O)
A2M3  = s_a2mcsv.$(O) s_a2mrtf.$(O) s_a2mgr1.$(O) s_a2mgdi.$(O) s_a2merr.$(O) s_a2mgrf.$(O)
A2M  = $(A2M1) $(A2M2) $(A2M3)

# Common ISAM usage (BC, LP, PAGE) for WIN and DOSWIN
ISAM =  s_isam.$(O) s_robjis.$(O) s_pgis.$(O) s_actis.$(O) s_pgfis.$(O) s_cmp.$(O) s_scan.$(O)  \
        s_actbc.$(O) s_isdmp.$(O) s_scchk.$(O) s_mpis.$(O) s_mpfn.$(O) s_pgsrch.$(O) s_isdb.$(O) \
        s_bcis.$(O) s_bcl.$(O) s_bcdw.$(O) s_isvarg.$(O) s_scpage.$(O) s_scisck.$(O) s_isauto.$(O) \
        s_ccmtmp.$(O) s_ccmtbc.$(O) s_ediscr.$(O) s_edbc.$(O) s_isnum.$(O) s_hsis.$(O) s_hsi2.$(O) \
        s_isld.$(O) s_mpref.$(O) s_scset.$(O) s_iscp.$(O) s_isa2o.$(O) s_mpbc.$(O)
DISAM = s_bcd.$(O) s_edis.$(O) s_mpd.$(O) s_abis.$(O) s_bcab.$(O) s_cpis.$(O) s_scact.$(O) s_bcfns.$(O)
WISAM = s_wmp2.$(O) s_wbc.$(O) s_wbc2.$(O) s_bcw.$(O) s_wsupi.$(O) s_mpw.$(O) s_wisam.$(O) s_wmp.$(O)        

# Simple GIF files generation
GIF = s_a2mgif.$(O) s_gd.$(O) s_gdftt.$(O) s_gdfts.$(O) s_gdftmb.$(O) s_gdftl.$(O) \
	s_gdftg.$(O) s_gdftgr.$(O)

# GZip file generation        
GZIP1 = s_gzadlr.$(O) s_gzcmpr.$(O) s_gzcrc.$(O) s_gzio.$(O) s_gzucmp.$(O) s_gzdefl.$(O) s_gztree.$(O) s_gzip.$(O)
GZIP2 = s_gzutil.$(O) s_gzinfl.$(O) s_gzinbl.$(O) s_gzintr.$(O) s_gzincd.$(O) s_gzinut.$(O) s_gzinfa.$(O)
GZIP  = $(GZIP1) $(GZIP2)

# Isam Client interface 
ISC   = s_dbvarg.$(O) s_dbcgnl.$(O) s_dbsock.$(O)

# CTREE library
DCTREE1 = ctibld.$(O) ctrbl2.$(O) ctvrc2.$(O) ctdelk.$(O) ctaddk.$(O)
DCTREE2 = ctupdt.$(O) ctsrch.$(O) ctdatf.$(O) ctkeyf.$(O) ctkrnl.$(O)
DCTREE3 = ctcomp.$(O) ctvrcu.$(O) ctinit.$(O) ctdelf.$(O) ctclb2.$(O)
DCTREE4 = ctclib.$(O) ctclb3.$(O) ctsset.$(O) ctisam.$(O) ctvrec.$(O)
DCTREE5 = ctism2.$(O) ctismc.$(O) ctismo.$(O) cticre.$(O) ctiopn.$(O)
DCTREE6 = cticls.$(O) ctifil.$(O) ctismu.$(O) ctdbug.$(O) s_ctree.$(O) s_ctdb.$(O)
DCTREE  = $(DCTREE1) $(DCTREE2) $(DCTREE3) $(DCTREE4) $(DCTREE5) $(DCTREE6)

# ODBC library / SCR usage
DODBC1 = s_odbc.$(O) s_stodbc.$(O) s_sql.$(O) s_odbcls.$(O) s_odbcpr.$(O)
DODBC2 = s_auton.$(O) s_sqlexe.$(O) s_sqlmn.$(O) s_dbis.$(O) # s_sup.$(O)
DODBC = $(DODBC1) $(DODBC2)

# ORACLE
S_ORACLEDO = s_oracle.$(O)

# Informix
S_INFORMDO = # s_inform.$(O)

# Postregsql
S_POSTG     = s_postg.$(O)

# Mysql
S_MYSQL     = s_mysql.$(O)

# Full standalone DB lib (Oracle, Informix, Postgres, mysql)
DDB = s_db.$(O) s_dbvars.$(O) s_dbutil.$(O) s_hsis.$(O) $(S_ORACLEDO) $(S_INFORMDO) $(S_POSTG) $(S_MYSQL) s_repli.$(O) s_checklv.$(O) s_lddll.$(O)

# Full client DB lib (Oracle, Informix, Postgres, mysql)
DDBC = s_dbc.$(O) s_dbvars.$(O) s_dbis.$(O) s_hsis.$(O) s_sqlexe.$(O) s_sqlmn.$(O) s_dbutil.$(O) s_replicl.$(O) s_repli.$(O) s_checklv.$(O)

# ---------------------------------------
# 2. DOSW LIBS : SCRDO SCRDI SCRIDB SCRICL
# ---------------------------------------
SCRDO   = $(BASE) $(RUNO) $(SCROLL) $(YY) $(LST) $(MMT) \
          $(SWAP1) $(SWAP2) $(ARGS) \
          $(RUNA) $(DIR1) $(DIR2) \
          $(CPP) $(A2M) \
          $(XDR) $(VF) $(CALC) \
          $(MAT) $(TAR1) $(TAR2) \
          $(LZH) $(FILE)  $(INI) $(GIF) $(HTML) $(GZIP) $(ISC)
SCRDI   = $(ISAM) $(DISAM)
SCRIDB  = $(DCTREE) $(DODBC) $(DDB)
SCRICL  = $(DDBC)

# ---------------------------
# 3. WIN LIBS : SCRWO & SCRWI
# ---------------------------

SW_BASE11 = s_menu.$(O) s_mnvarg.$(O) s_mnch.$(O) s_panic.$(O) s_hlpstk.$(O) s_edpwd.$(O)
SW_BASE32 = s_pgcol.$(O) s_wri.$(O) s_misc.$(O) s_edtpw.$(O) s_rdwpos.$(O) s_mnhlp.$(O)
SW_BASE5  = s_pgfn.$(O)  s_pgask.$(O) s_prof.$(O) s_abhlp.$(O) s_udfld.$(O)
SW_BASE51 = s_objis.$(O) s_objtxt.$(O) s_objpg.$(O) s_obj.$(O) s_rprdev.$(O) s_wlock.$(O)
SW_BASE52 = s_flt.$(O) s_fltnam.$(O) s_fltnu.$(O) s_fltfnd.$(O) s_fltfld.$(O) s_fltfix.$(O) s_sn.$(O)
SW_BASE53 = s_fltexi.$(O) s_fltset.$(O) s_keytxt.$(O) s_vars.$(O) s_actcr.$(O) s_pgmmt.$(O)
SW_RUNO   = s_robj.$(O) s_robjpr.$(O) s_objrd.$(O) s_cint.$(O) s_wprint.$(O) s_drw.$(O)  s_dsptch.$(O)
SW_BASE71 = s_cmtget.$(O) s_cmtbp.$(O) s_cmtdrw.$(O) s_cmtrd.$(O) s_cmts.$(O) s_cmtscr.$(O) s_iskey.$(O)
SW_BASE72 = s_cmtpg.$(O) s_cmtmn.$(O) s_pgdft.$(O) s_pgempt.$(O)  s_atof.$(O) s_pgmod.$(O) s_pgfmt.$(O) s_pgrset.$(O)
SW_BASE73 = s_edfld.$(O) s_reckey.$(O) s_pgkey.$(O) s_isfnk.$(O) s_edflt.$(O) s_cgi.$(O) s_cgi2.$(O) s_sess.$(O)
SW_BASE8  = s_pgedit.$(O) s_scincr.$(O) s_dispwd.$(O) s_box.$(O) s_box2.$(O) s_dispfn.$(O) s_dired.$(O) s_pghlp.$(O)
SW_BASE81 = s_dispfn2.$(O) s_dispfn3.$(O) s_dispfn4.$(O) s_dispfn5.$(O) s_dispfn6.$(O)
SW_BASE9  = s_prt.$(O) s_prmail.$(O) s_prfile.$(O) s_prlab.$(O) s_prlist.$(O) s_prtstr.$(O)  s_pgusr.$(O)
SW_BASE91 = s_prtdev.$(O) s_prtnam.$(O) s_prtsiz.$(O) s_prtmv.$(O) s_prtpg.$(O) s_pgsld.$(O)
SW_BASE92 = s_prtatr.$(O) s_prttab.$(O) s_prtact.$(O) s_prterr.$(O) s_grpbox.$(O) s_vfld.$(O) s_dbconn.$(O)
SW_BASE94 = s_ccmtpg.$(O) s_ccmtmn.$(O) s_ccmtab.$(O) s_cmtall.$(O) s_baend.$(O) s_w32cch.$(O) s_prot.$(O) s_protv.$(O)
SW_BASE99 = s_anum.$(O) s_ak.$(O) s_cak.$(O) s_magic.$(O) s_key.$(O) s_vers.$(O) s_flock.$(O) s_act.$(O)
SW_BASE = $(SW_RUNO) $(SW_BASE52) $(SW_BASE53) $(SW_BASE99) $(SW_BASE72) \
	$(SW_BASE73) $(SW_BASE5) $(SW_BASE51) $(SW_BASE9) $(SW_BASE91) \
	$(SW_BASE92) $(SW_BASE94) $(SW_BASE32) $(SW_BASE71) $(SW_BASE8) $(SW_BASE81) $(SW_BASE11)

SW_IMAGE1 = s_image.$(O) s_imwrt.$(O) s_immv.$(O) s_im2im.$(O) s_imfd.$(O)
SW_IMAGE2 = s_imclr.$(O) s_imset.$(O) s_imattr.$(O) s_imallc.$(O)
SW_IMAGE = $(SW_IMAGE1) $(SW_IMAGE2)

SW_STRS1 = s_allc.$(O)   s_stsqz.$(O) s_ststrd.$(O) s_ststrp.$(O) s_memavl.$(O) s_debug.$(O)
SW_STRS2 = s_stacpy.$(O) s_stcmpr.$(O) s_stfmtl.$(O) s_stfmtt.$(O) s_stcvt.$(O) s_sleep.$(O)  s_iscwb.$(O)
SW_STRS3 = s_stisal.$(O) s_stlcpy.$(O) s_stlfmt.$(O) s_stljus.$(O) s_stchch.$(O) s_strdfl.$(O)
SW_STRS4 = s_stlow.$(O)  s_stpad.$(O)  s_stpstr.$(O) s_strjus.$(O) s_stansi.$(O) s_stutf8.$(O) s_sterrc.$(O)
SW_STRS5 = s_stucen.$(O) s_stufml.$(O) s_stufmt.$(O) s_stuind.$(O) s_stgetp.$(O) s_stfnam.$(O)
SW_STRS6 = s_stuisn.$(O) s_stupos.$(O) s_stuppr.$(O) s_stushf.$(O) s_pcinfo.$(O) s_pcipar.$(O)
SW_STRS7 = s_stusqz.$(O) s_stzstr.$(O) s_stvtom.$(O) s_stmtov.$(O) s_stgrep.$(O) s_stfstr.$(O) s_code64.$(O)
SW_STRS8 = s_staddp.$(O) s_sttext.$(O) s_sttxtt.$(O) s_sterrt.$(O) s_stcrpt.$(O) s_strcmp.$(O) s_prrep.$(O)
SW_STRS9 = s_stsrcf.$(O) s_stchex.$(O) s_stctex.$(O) s_stfmtd.$(O) s_stchk.$(O) s_strnat.$(O)
SW_STRS10= s_stpsl.$(O) s_stpst.$(O) s_stun.$(O) s_stinte.$(O) s_stdiff.$(O) s_ststr.$(O) s_stdhex.$(O) s_sha1.$(O)
SW_STRS11= s_strepl.$(O) s_strdln.$(O) s_strcpy.$(O) s_stfren.$(O) s_string.$(O) s_copy.$(O) s_isutil.$(O)
SW_STRS12= s_stpc0.$(O)  s_stpcin.$(O) s_stpcl.$(O) s_stpcex.$(O) s_stprf.$(O) s_stobj.$(O)
SW_STRS = $(SW_STRS1) $(SW_STRS2) $(SW_STRS3) $(SW_STRS4) $(SW_STRS5) \
	$(SW_STRS6) $(SW_STRS7) $(SW_STRS8) $(SW_STRS9) $(SW_STRS10) \
	$(SW_STRS11) $(SW_STRS12)

SW_DATE = s_date.$(O) s_date_.$(O) s_dt.$(O) s_dtchkd.$(O) s_dtchkt.$(O)

SW_INF  = s_infmis.$(O) s_infadd.$(O) s_infsub.$(O) s_infmul.$(O) s_infdiv.$(O)

SW_CPP = s_cpp.$(O) s_cppfns.$(O) s_cpperr.$(O)
SW_HTML = s_html.$(O) s_htmlyy.$(O) s_htmltc.$(O) s_htmler.$(O) s_htmlsp.$(O) s_htmlsb.$(O)
SW_A2M1  = s_a2mrd.$(O) s_a2mseg.$(O) s_a2mhtm.$(O) s_a2mfrm.$(O) s_a2mfrg.$(O) s_a2mmsg.$(O) s_a2mcsv.$(O)  s_a2mtc2.$(O) s_a2mswf.$(O)
SW_A2M2  = s_a2mrtf.$(O) s_a2mgr1.$(O) s_a2mgdi.$(O) s_a2merr.$(O) s_a2mgrf.$(O) s_a2mmem.$(O) s_a2memf.$(O) s_a2mtch.$(O) s_a2mdok.$(O)
SW_A2M  = $(SW_A2M1) $(SW_A2M2)

SW_FILE = s_fiam.$(O) s_fiap.$(O) s_fiem.$(O) s_fifm.$(O) s_file.$(O) s_fism.$(O)

SW_ISAM1 = s_isam.$(O) s_robjis.$(O) s_pgis.$(O) s_actis.$(O) s_pgfis.$(O) s_cmp.$(O) s_scan.$(O) s_actbc.$(O) s_isdmp.$(O)
SW_ISAM2 = s_scchk.$(O) s_mpis.$(O) s_mpfn.$(O) s_pgsrch.$(O) s_isdb.$(O) s_bcis.$(O) s_bcl.$(O) s_bcdw.$(O) s_isvarg.$(O)
SW_ISAM3 = s_scpage.$(O) s_scisck.$(O) s_isauto.$(O) s_ccmtmp.$(O) s_ccmtbc.$(O) s_ediscr.$(O) s_edbc.$(O) s_isnum.$(O)
SW_ISAM4 = s_hsis.$(O) s_hsi2.$(O) s_isld.$(O) s_mpref.$(O) s_scset.$(O) s_iscp.$(O)  s_isa2o.$(O)

# SW_WISAM1 = s_wmp2.$(O) s_wbc.$(O) s_wbc2.$(O) s_bcw.$(O)
# SW_WISAM2 = s_wsupi.$(O) s_mpw.$(O) s_wisam.$(O) s_wmp.$(O) s_mpbc.$(O)
# SW_WISAM = $(SW_WISAM1) $(SW_WISAM2)

SW_GIF = s_a2mgif.$(O) s_gd.$(O) s_gdftt.$(O) s_gdfts.$(O) s_gdftmb.$(O) s_gdftl.$(O) s_gdftg.$(O) s_gdftgr.$(O)

SW_GZIP1 = s_gzadlr.$(O) s_gzcmpr.$(O) s_gzcrc.$(O) s_gzio.$(O) s_gzucmp.$(O) s_gzdefl.$(O) s_gztree.$(O) s_gzip.$(O)
SW_GZIP2 = s_gzutil.$(O) s_gzinfl.$(O) s_gzinbl.$(O) s_gzintr.$(O) s_gzincd.$(O) s_gzinut.$(O) s_gzinfa.$(O)
SW_GZIP  = $(SW_GZIP1) $(SW_GZIP2)

SW_ISC   = s_dbvarg.$(O) s_dbcgnl.$(O) s_dbsock.$(O)

SW_WIN1 = s_wmain.$(O)  s_wproc.$(O) s_winit1.$(O) s_wkeys.$(O) s_wreck.$(O) s_wcomp.$(O) s_wdlg.$(O) s_whwndw.$(O)
SW_WIN2 = s_wscrol.$(O) s_wreg.$(O) s_wmisc.$(O) s_wprt.$(O) s_wprt2.$(O) s_wtray.$(O) s_whid.$(O)
SW_WIN3 = s_wpgdlg.$(O) s_whead.$(O) s_wprops.$(O) s_wcmt.$(O) s_wtoolb.$(O) s_wsys2.$(O) s_wgetms.$(O) s_wdrive.$(O)  s_wps.$(O) s_wpsmem.$(O) # s_wmainh.$(O)
SW_WIN4 = s_wsys.$(O) s_wvers.$(O) s_whook.$(O) s_wvars.$(O) s_wobjs.$(O) s_whwnd.$(O) s_wnet.$(O) s_wdial.$(O)
SW_WIN5 = s_wend.$(O) s_wemf.$(O) s_wprts.$(O) s_wprtp.$(O) s_wap.$(O) s_gwmisc.$(O) s_wprocs.$(O) s_wsocks.$(O) s_wsockc.$(O) s_webs.$(O) s_httpget.$(O)
SW_WIN6 = s_wstat.$(O) s_wcomdl.$(O) s_wddecl.$(O) s_wtimer.$(O) s_smtp.$(O) s_pop3.$(O) s_ftp.$(O) s_wddlg.$(O)
SW_WIN7 = s_wsup.$(O) s_wsuppg.$(O) s_wemfcr.$(O) s_wtoolt.$(O) s_wcomp2.$(O) s_wserv.$(O) s_wservu.$(O)
SW_WIN8 = s_wprraw.$(O) s_whlp.$(O) s_wddesr.$(O) s_wclipb.$(O)

SW_WIN = $(SW_WIN1) $(SW_WIN2) $(SW_WIN3) $(SW_WIN4) \
	$(SW_WIN5) $(SW_WIN6) $(SW_WIN7) $(SW_WIN8)

SCRWO  = $(YY) $(LST) $(SWAP1) $(SWAP2) $(XDR) $(ARGS) \
	$(MAT) $(DIR1) $(DIR2) $(LZH) $(SW_WIN) $(SW_STRS) $(SW_DATE) \
	$(SW_BASE) $(SW_IMAGE) $(INI) $(SW_CPP) $(SW_HTML) $(SW_A2M) $(SW_FILE) \
	$(SW_GIF) $(SW_GZIP) $(SW_INF) $(SW_ISC)

SCRWI  = $(ISAM) $(WISAM)

#--------------------------- IODE ----------------------------------------------

IODE_SWAP1  = s_swallc.$(O) s_swfile.$(O) s_swab.$(O) s_swsegs.$(O)
IODE_SWAP2  = s_swems.$(O) s_swmisc.$(O) s_swvars.$(O)
IODE_SWAP   = $(IODE_SWAP1) $(IODE_SWAP2)
IODE_LST    = s_lst1.$(O) s_lst2.$(O)
IODE_BASE   = s_vars.$(O) s_dirrd.$(O) s_dirstd.$(O) s_dirspd.$(O) s_dirsta.$(O) s_dirfp.$(O) s_dircwd.$(O) s_dirj.$(O) s_dirfmt.$(O) s_dirtbl.$(O) s_dirspn.$(O) s_wsockc.$(O) s_httpget.$(O)

IODE_STRS1 = s_allc.$(O) s_stsqz.$(O)  s_ststrd.$(O) s_ststrp.$(O) s_memavl.$(O) s_atof.$(O) s_whlp.$(O)
# s_memavl.$(O) s_panic.$(O)
IODE_STRS2 = s_stacpy.$(O) s_stcmpr.$(O) s_stfmtl.$(O) s_stfmtt.$(O) s_stcvt.$(O) s_debug.$(O)
IODE_STRS3 = s_stisal.$(O) s_stlcpy.$(O) s_stlfmt.$(O) s_stljus.$(O) s_stchch.$(O) s_sleep.$(O)
IODE_STRS4 = s_stlow.$(O)  s_stpad.$(O)  s_stpstr.$(O) s_strjus.$(O) s_stansi.$(O) s_stutf8.$(O) s_sterrc.$(O)
IODE_STRS5 = s_stucen.$(O) s_stufml.$(O) s_stufmt.$(O) s_stuind.$(O) s_stgetp.$(O) s_stfnam.$(O)
IODE_STRS6 = s_stuisn.$(O) s_stupos.$(O) s_stuppr.$(O) s_stushf.$(O) # s_pcipar.$(O) s_pcinfo.$(O)
IODE_STRS7 = s_stusqz.$(O) s_stzstr.$(O) s_stvtom.$(O) s_stmtov.$(O) s_stgrep.$(O) s_strcmp.$(O) # s_code64.$(O)
IODE_STRS8 = s_staddp.$(O) s_sttext.$(O) s_sttxtt.$(O) s_sterrt.$(O) s_stcrpt.$(O) s_stfstr.$(O)
IODE_STRS9 = s_stsrcf.$(O) s_stchex.$(O) s_stctex.$(O) s_stfmtd.$(O) s_stchk.$(O) s_strnat.$(O) # s_iscwb.$(O)
IODE_STRS10= s_stpsl.$(O) s_stpst.$(O) s_stun.$(O) s_stinte.$(O) s_stdiff.$(O) s_strdfl.$(O) # s_vfld.$(O)
IODE_STRS11= s_strepl.$(O) s_strdln.$(O) s_strcpy.$(O) s_stfren.$(O) s_string.$(O) s_stobj.$(O) # s_prrep.$(O)
IODE_STRS12= s_stpcin.$(O) s_stpcl.$(O) s_stpcex.$(O) s_stprf.$(O) s_ststr.$(O) # s_isutil.$(O)
IODE_STRS13= s_stpak1.$(O) s_stpaka.$(O) s_stpakx.$(O) s_stpakl.$(O) s_std.$(O) s_copy.$(O) s_stdhex.$(O) s_sha1.$(O)
IODE_STRS = $(IODE_STRS1) $(IODE_STRS2) $(IODE_STRS3) $(IODE_STRS4) $(IODE_STRS5) \
	$(IODE_STRS6) $(IODE_STRS7) $(IODE_STRS8) $(IODE_STRS9) $(IODE_STRS10) \
	$(IODE_STRS11) $(IODE_STRS12) $(IODE_STRS13)

IODE_DDE  = s_wddesr.$(O) s_wddecl.$(O)

IODE_YY   = s_ybase.$(O) s_ydef.$(O) s_yincl.$(O) s_ylex.$(O) s_yread.$(O) s_ydebug.$(O) s_yerror.$(O)

IODE_MAT1 = s_maallc.$(O) s_madup.$(O)  s_maerr.$(O)  s_maprod.$(O) s_matran.$(O)
IODE_MAT2 = s_mainv.$(O)  s_madet.$(O)  s_masol.$(O)  s_mamisc.$(O) s_maro.$(O)
IODE_MAT3 = s_matrac.$(O) s_macalc.$(O) s_makron.$(O) s_mascal.$(O) s_manull.$(O)
IODE_MAT4 = s_maextr.$(O) s_maxpx.$(O)  s_maxxp.$(O)  s_maxaxp.$(O) s_maxpax.$(O)
IODE_MAT5 = s_maprt.$(O) s_madump.$(O) s_mascrl.$(O) s_maset.$(O) s_mastat.$(O) s_masol2.$(O)
IODE_MAT  = $(IODE_MAT1) $(IODE_MAT2) $(IODE_MAT3) $(IODE_MAT4) $(IODE_MAT5)

IODE_A2M1  = s_a2mrd.$(O) s_a2mseg.$(O)  s_a2mhtm.$(O) s_a2mfrm.$(O) s_a2mfrg.$(O) s_wemfcr.$(O) s_wemf.$(O) s_a2memf.$(O)
IODE_A2M2  = s_a2mmem.$(O) s_a2mtc2.$(O) s_a2mtch.$(O) s_wprts.$(O) s_a2mswf.$(O) # s_a2mgif.$(O)
IODE_A2M3  = s_a2mcsv.$(O) s_a2mrtf.$(O) s_a2mgr1.$(O) s_a2mgdi.$(O) s_a2merr.$(O) s_a2mgrf.$(O)
IODE_A2M  = $(IODE_A2M1) $(IODE_A2M2) $(IODE_A2M3)

IODE_ARGS1 = s_ainit.$(O) s_aerr.$(O) s_aexec.$(O) s_aexp.$(O) s_aiexp.$(O) s_acerr.$(O)
IODE_ARGS2 = s_aexps.$(O) s_aexpsd.$(O) s_acheck.$(O) s_achkin.$(O) s_achkac.$(O) # s_ausage.$(O)
IODE_ARGS = $(IODE_ARGS1) $(IODE_ARGS2)

IODE_DATE = s_date.$(O) s_date_.$(O) s_dt.$(O) s_dtcal.$(O) s_dtchkd.$(O) s_dtchkt.$(O)
IODE_INI  = s_rdini.$(O)
IODE_CPP  = s_cpp.$(O) s_cppfns.$(O) s_cpperr.$(O)

IODE_ODBC = s_odbc.$(O) s_odbcls.$(O) s_odbcpr.$(O)
IODE_GZIP1 = s_gzadlr.$(O) s_gzcmpr.$(O) s_gzcrc.$(O) s_gzio.$(O) s_gzucmp.$(O) s_gzdefl.$(O) s_gztree.$(O) s_gzip.$(O)
IODE_GZIP2 = s_gzutil.$(O) s_gzinfl.$(O) s_gzinbl.$(O) s_gzintr.$(O) s_gzincd.$(O) s_gzinut.$(O) s_gzinfa.$(O)
IODE_GZIP  = $(IODE_GZIP1) $(IODE_GZIP2)

IODE_WSYS = s_wprt.$(O) s_wvers.$(O) s_wsys2.$(O) s_wgetms.$(O) # s_whwndd.$(O)
SCRIODE   = $(IODE_SWAP) $(IODE_STRS) $(IODE_LST) $(IODE_MAT) $(IODE_A2M) $(IODE_DATE) \
            $(IODE_WSYS) $(IODE_ARGS) $(IODE_CPP) \
	    $(IODE_INI) $(IODE_YY) $(IODE_BASE) $(IODE_ODBC) $(IODE_DDE) $(IODE_GZIP)

#------ Include files --------#
H     = s_yy.h s_args.h s_date.h s_odbc.h s_strs.h s_swap.h s_scroll.h s_scrold.h \
	s_mat.h s_calc.h s_vf.h s_lst.h s_mmt.h s_xdr.h scr4.h scr.h scr4_d.h \
	scr4_key.h s_tar.h s_dir.h scr4_box.h scr4_str.h scr4_rd.h s_file.h \
        s_ini.h s_html.h scr4w.h s_cpp.h s_a2m.h s_isc.h scr4g.h s_alldoc.h scr4web.h \
	s_proa2m.h s_proarg.h s_probas.h s_probis.h s_prowis.h s_procpp.h s_prodir.h \
	s_prodt.h s_prohtm.h s_proini.h s_prolst.h s_prolzh.h s_proma.h s_proscl.h \
        s_prost.h s_prosw.h s_prowin.h s_proxdr.h s_provf.h s_proclc.h \
        s_profil.h s_protar.h s_proy.h            
        
#------ All ------------------#            
all : s4libs s4exes

#------ LIBRARY DEFINITIONS AND LISTS -----------------#
s4libs : \
        s4iode.$(L) \
        s4do.$(L)  s4di.$(L) \
        s4wo.$(L)  s4wi.$(L) \
        s4icl.$(L) s4idb.$(L) 

# Librairies
s4iode.$(L) : $(SCRIODE)
       -del $@
       $(MKLIB) $**
s4do.$(L) : $(SCRDO)
        -del $@
        $(MKLIB) $**
s4di.$(L) : $(SCRDI)
        -del $@
        $(MKLIB) $**
s4wo.$(L) : $(SCRWO)
        -del $@
        $(MKLIB) $**
s4wi.$(L) : $(SCRWI)
        -del $@
        $(MKLIB) $**
s4idb.$(L) : $(SCRIDB)
        -del $@
        $(MKLIB) $**
s4icl.$(L) : $(SCRICL)
        -del $@
        $(MKLIB) $**   
        
#------ EXECUTABLES DEFINITIONS AND LISTS -----------------#
#OCIW32LIB = ociw32.$(L)
#ODBCLIBS = odbc32.$(L) odbccp32.$(L) 

s4exes : \
        s4_app$(X)  s4_ar$(X)  s4_drv$(X) \
        s4_h$(X)    s4_hm$(X)   s4_toc$(X) s4_spl$(X) s4_a2g$(X) s4_a2f$(X) s4_a2h$(X) s4_a2d$(X) s4_m2a$(X) s4_sbs$(X) \
        s4_o2t$(X)  s4_o2a$(X)  s4_a2o$(X) s4_d2u$(X) s4_u2d$(X) s4_utf8$(X) s4_low$(X) \
        s4_cl$(X)   s4_sync$(X) \
        s4_smtp$(X) s4_pop3$(X) \
        s4_db$(X)   s4_dbc$(X) s4_od$(X) s4_repli$(X) \
        s4_e$(X) s4_mt$(X)   
        
# SCR engine, compiler, utils
s4_e$(X) : scr_e.$(O)     s_wprtf2.$(O) s_args.$(O) s_extr.$(O) s_dobj.$(O) s_dump.$(O) s_mnprot.$(O) \
	   s_robjis.$(O) s_pgfis.$(O) s_cfns.$(O) s_cfnspr.$(O) s_wextr.$(O) s4do.$(L)
	$(COMPLINK) $** ws2_32.lib gdi32.lib user32.lib advapi32.lib shell32.lib comdlg32.lib
s4_app$(X) : scr_app.$(O) s_wprtf2.$(O) s4wo.$(L)
        $(COMPLINK) $**
s4_ar$(X) : scr_ar.$(O)
	$(COMPLINK) $**
s4_drv$(X) : scr_drv.$(O) s_wprtf2.$(O) s4do.$(L)
	$(COMPLINK) $**

# Help and A2m conversions
s4_h$(X) : scr_h.$(O)     s_wprtf2.$(O) s_herr.$(O) s4wo.$(L)
	$(COMPLINK) $**
s4_hm$(X) : scr_hm.$(O)   s_wprtf2.$(O) s4wo.$(L)
	$(COMPLINK) $**
s4_toc$(X) : scr_toc.$(O) s_wprtf2.$(O) s4wo.$(L)
	$(COMPLINK) $**
s4_sbs$(X) : scr_sbs.$(O) s_wprtf2.$(O) s4wo.$(L)
	$(COMPLINK) $**
s4_spl$(X) : scr_spl.$(O) s_wprtf2.$(O) s4wo.$(L)
	$(COMPLINK) $**
s4_a2g$(X) : scr_a2g.$(O) s_wprtf2.$(O) s4wo.$(L)
	$(COMPLINK) $** 
s4_a2f$(X) : scr_a2f.$(O) s_wprtf2.$(O) s4wo.$(L)
	$(COMPLINK) $**            
s4_a2h$(X) : scr_a2h.$(O) s_wprtf2.$(O) s4wo.$(L)
	$(COMPLINK) $**            
s4_a2d$(X) : scr_a2d.$(O) s_wprtf2.$(O) s4wo.$(L)
	$(COMPLINK) $**            
s4_m2a$(X) : scr_m2a.$(O) s_wprtf2.$(O) s4wo.$(L)
	$(COMPLINK) $**

# Conversions chars
s4_o2t$(X) : scr_o2t.$(O)   s_wprtf2.$(O) s_args.$(O) s4wo.$(L)
	 $(COMPLINK) $**
#s4_o2t$(X) : scr_o2t.$(O) s_args.$(O) s4wo.$(L)
#	$(COMPLINK) $**
s4_o2a$(X) : scr_o2a.$(O)   s_wprtf2.$(O) s_args.$(O) s4wo.$(L)
	$(COMPLINK) $**
s4_a2o$(X) : scr_a2o.$(O)   s_wprtf2.$(O) s_args.$(O) s4wo.$(L)
	$(COMPLINK) $**
s4_d2u$(X) : scr_d2u.$(O)   s_wprtf2.$(O) s_args.$(O) s4wo.$(L)
	$(COMPLINK) $**
s4_u2d$(X) : scr_u2d.$(O)   s_wprtf2.$(O) s_args.$(O) s4wo.$(L)
	$(COMPLINK) $**
s4_utf8$(X) : scr_utf8.$(O) s_wprtf2.$(O) s_args.$(O) s4wo.$(L)
	$(COMPLINK) $**
s4_low$(X) : scr_low.$(O) s4wo.$(L)
	$(COMPLINK) $**

# TCPIP transfers  
s4_cl$(X)  : scr_cl.$(O)    s_wprtf2.$(O) s4wo.$(L)
	$(COMPLINK) $**
s4_sync$(X) : scr_sync.$(O) s_wprtf2.$(O) s4wo.$(L)
	$(COMPLINK) $**

# Mail
s4_smtp$(X) : scr_smtp.$(O) s_wprtf2.$(O) s4wo.$(L)
	$(COMPLINK) $**
s4_pop3$(X) : scr_pop3.$(O) s_wprtf2.$(O) s4wo.$(L)
	$(COMPLINK) $**

# Databases 
s4_db$(X) : scr_db.$(O)   s_wprtf2.$(O) s_sup.$(O)  s_args.$(O) s4do.$(L) s4di.$(L) s4idb.$(L) 
	$(COMPLINK) $** $(ODBCLIBS)
s4_dbc$(X) : scr_db.$(O)  s_wprtf2.$(O) s_sup.$(O)  s_args.$(O) s4do.$(L) s4di.$(L) s4icl.$(L)
	$(COMPLINK) $**       
s4_od$(X) : scr_od.$(O) s_wprtf2.$(O) s4wo.$(L) s4idb.$(L)
	$(COMPLINK) $**  $(ODBCLIBS)
s4_repli$(X) : scr_s4repli.$(O) s4wo.$(L) s4wi.$(L) s4icl.$(L)
	$(COMPLINK) $**

#Misc
s4_mv$(X) : scr_mv.$(O) s4wo.$(L)
	$(COMPLINK) $**
s4_cron$(X) : scr_cron.$(O) s4wo.$(L)
	$(COMPLINK) $**
s4_schd$(X) : scr_schd.$(O) s4wo.$(L)
	$(COMPLINK) $**
s4_nncp$(X) : scr_nncp.$(O) s4wo.$(L)
	$(COMPLINK) $**
s4_lzh$(X) : scr_lzh.$(O) s4wo.$(L)
	$(COMPLINK) $**
s4_cry$(X) : scr_cry.$(O) s4wo.$(L)
	$(COMPLINK) $**
s4_mkh$(X) : scr_mkh.$(O) s_wprtf2.$(O) s4wo.$(L)
	$(COMPLINK) $**

# Specials 
# MT
MTOBJS1 = s_mtbloc.$(O) s_mtbox.$(O) s_mtconf.$(O)
MTOBJS2 = s_mtdisp.$(O) s_mtedit.$(O) s_mterr.$(O) s_mtkey.$(O)
MTOBJS3 = s_mtls.$(O) s_mtmem.$(O) s_mtsc.$(O) s_mttab.$(O) s_mtdel.$(O)
MTOBJS4 = s_mtmove.$(O) s_mtvars.$(O) s_mtmarg.$(O) s_mtimp.$(O)
MTOBJS5 = s_mtins.$(O) s_mthelp.$(O) s_mtnode.$(O) s_args.$(O)
MTOBJS = $(MTOBJS1) $(MTOBJS2) $(MTOBJS3) $(MTOBJS4) $(MTOBJS5)

s_mtobjs.scr : ansi.drv ../s_mtdef.f ../s_mtmn.f ../s_mt.f ../scr.f ../scr4_str.h ../scr4.h 
	s4_e -D NOISAM -pgalign -af ../s_mt.f -o s_mtobjs -1obj -fldmem -drwmem -namemem -I s_mt.h $(SDEFS) $(TANSI)

S4_MT_DEPS = s4_mt.res s_mtobjs.scr 
S4_MT_OBJS = s_mtobjs.$(O) $(MTOBJS) scr_mt.$(O) s_whlp.$(O) s4do.$(L)
s4_mt$(X) : s4_mt.res s_mtobjs.scr s_mtobjs.$(O) $(MTOBJS) scr_mt.$(O) s_whlp.$(O) s4do.$(L)
        $(COMPLINKW) $(S4_MT_OBJS) 
#        $(COMPLINK)s4_mt.exe -W s4_mt.res s_mtobjs.$(O) $(MTOBJS) scr_mt.$(O) s_whlp.$(O) s4do.$(L)
#        ilink32 -c -Tpe -aa -Gn -L. c0w32.obj scr_mt.$(O) s_mtobjs.$(O) s_whlp.$(O) $(MTOBJS),s4_mt,,s4do.$(L) cw32mt import32,,s4_mt.res
#        $(ILINK) scr_mt.$(OBJ) s_mtobjs.$(OBJ) s_whlp.$(OBJ),s32w_mt,,scr_mt.$(L) $(LIBDIR)$(DPRFX)o.$(L) $(LIBDIR)$(DPRFX)icl.$(L) cw32mt import32,,$(RESMT)
#	 $(ILINK) scr_mt.$(OBJ) s_mtobjs.$(OBJ) s_whlp.$(OBJ),s32w_mt,,scr_mt.$(L) $(LIBDIR)$(DPRFX)o.$(L) cw32mt import32,,$(RESMT)
	s4_app s4_mt$(X) s_mtobjs.scr
	s4_app s4_mt$(X) s_mtobjs.cmt