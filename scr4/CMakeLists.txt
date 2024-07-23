# CMakeList.txt : CMake project for scr4, include source and define
# project specific logic here.
#

message(STATUS "\n---- SCR4 ----\n")

# Header files
set (HEADER_IODE 
		s_a2m.h s_alldoc.h s_args.h s_calc.h  s_cpp.h s_date.h s_dir.h s_file.h s_html.h s_ini.h s_isc.h
		s_lst.h s_mat.h s_mmt.h s_odbc.h s_proa2m.h s_proarg.h s_probas.h s_probis.h s_proclc.h s_procpp.h s_prodir.h s_prodt.h s_profil.h s_prohtm.h s_proini.h
		s_prolst.h s_prolzh.h s_proma.h s_proscl.h s_prost.h s_prosw.h s_protar.h s_provf.h s_prowin.h s_prowis.h s_proxdr.h s_proy.h
		s_scrold.h s_scroll.h s_strs.h s_swap.h s_tar.h s_vf.h s_xdr.h s_yy.h scr.h scr4.h scr4_box.h scr4_d.h scr4_key.h scr4_rd.h scr4_str.h scr4g.h scr4w.h scr4web.h 
)

# Source files
set (IODE_SWAP s_swallc.c s_swfile.c s_swab.c s_swsegs.c s_swems.c s_swmisc.c s_swvars.c)
set (IODE_LST s_lst1.c s_lst2.c)
set (IODE_BASE  s_vars.c s_dirstd.c s_dirspd.c s_dirsta.c s_dirfp.c s_dircwd.c s_dirj.c s_dirfmt.c  s_dirspn.c s_wsockc.c)  # s_dirrd.c s_dirtbl.c
set (IODE_INI s_rdini.c)
set (IODE_STRS 
		s_allc.c s_stsqz.c  s_ststrd.c s_ststrp.c s_memavl.c s_atof.c
		s_stacpy.c s_stcmpr.c s_stfmtl.c s_stfmtt.c s_stcvt.c s_debug.c
		s_stisal.c s_stlcpy.c s_stlfmt.c s_stljus.c s_stchch.c s_sleep.c
		s_stlow.c  s_stpad.c  s_stpstr.c s_strjus.c s_stansi.c s_stutf8.c s_sterrc.c
		s_stucen.c s_stufml.c s_stufmt.c s_stuind.c s_stgetp.c s_stfnam.c
		s_stuisn.c s_stupos.c s_stuppr.c s_stushf.c 
		s_stusqz.c s_stzstr.c s_stvtom.c s_stmtov.c s_stgrep.c s_strcmp.c 
		s_staddp.c s_sttext.c s_sttxtt.c s_sterrt.c s_stcrpt.c s_stfstr.c
		s_stsrcf.c s_stchex.c s_stctex.c s_stfmtd.c s_stchk.c s_strnat.c 
		s_stpsl.c s_stpst.c s_stun.c s_stinte.c s_stdiff.c s_strdfl.c 
		s_strepl.c s_strdln.c s_strcpy.c s_stfren.c s_string.c s_stobj.c 
		s_stpcin.c s_stpcl.c s_stpcex.c s_stprf.c s_ststr.c 
		s_stpak1.c s_stpaka.c s_stpakx.c s_stpakl.c s_std.c s_copy.c s_stdhex.c)	# s_whlp.c s_sha1.c
set (IODE_DDE  s_wddesr.c s_wddecl.c)
set (IODE_YY  s_ybase.c s_ydef.c s_yincl.c s_ylex.c s_yread.c s_ydebug.c s_yerror.c)
set (IODE_MAT 
		s_maallc.c s_madup.c  s_maerr.c  s_maprod.c s_matran.c
		s_mainv.c  s_madet.c  s_masol.c  s_mamisc.c s_maro.c
		s_matrac.c s_macalc.c s_makron.c s_mascal.c s_manull.c
		s_maextr.c s_maxpx.c  s_maxxp.c  s_maxaxp.c s_maxpax.c
		s_maprt.c s_madump.c s_maset.c s_mastat.c s_masol2.c) # s_mascrl.c 
set (IODE_A2M 
		s_a2mrd.c s_a2mseg.c  s_a2mhtm.c s_a2mfrm.c s_a2mfrg.c s_a2mmd.c s_wemfcr.c s_wemf.c s_a2memf.c
		s_a2mmem.c s_a2mtc2.c s_a2mtch.c s_wprtdf.c s_a2mswf.c 
		s_a2mcsv.c s_a2mrtf.c s_a2mgr1.c s_a2mgdi.c s_a2merr.c s_a2mgrf.c
		s_a2mmsg.c)
set (IODE_ARGS 
		s_ainit.c s_aerr.c s_aexec.c s_aexp.c s_aiexp.c s_acerr.c
		s_aexps.c s_aexpsd.c s_acheck.c s_achkin.c s_achkac.c)
set (IODE_DATE s_date.c s_date_.c s_dt.c s_dtcal.c s_dtchkd.c s_dtchkt.c)
set (IODE_INI s_rdini.c)
set (IODE_CPP s_cpp.c s_cppfns.c s_cpperr.c)
set (IODE_ODBC s_odbc.c s_odbcls.c s_odbcpr.c)
set (IODE_GZIP 
		s_gzadlr.c s_gzcmpr.c s_gzcrc.c s_gzio.c s_gzucmp.c s_gzdefl.c s_gztree.c s_gzip.c
		s_gzutil.c s_gzinfl.c s_gzinbl.c s_gzintr.c s_gzincd.c s_gzinut.c s_gzinfa.c)
set (IODE_WSYS s_wprt.c s_wvers.c s_wsys2.c s_wgetms.c)
set (ISC s_iode.c)

set (SCR_IODE 
	${IODE_SWAP} ${IODE_LST} ${IODE_BASE} ${IODE_INI} ${IODE_STRS} ${IODE_DDE}  ${IODE_YY}  ${IODE_MAT}  
	${IODE_A2M}  ${IODE_ARGS}  ${IODE_DATE}  ${IODE_INI}  ${IODE_CPP}  ${IODE_ODBC} ${IODE_GZIP} ${IODE_SYS} ${IODE_WSYS} ${ISC}) 

# Add source to this project's executable.
add_library (iode_scr4 STATIC ${HEADER_IODE} ${SCR_IODE})
# target_include_directories(iode_scr4 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
	target_compile_options(iode_scr4 PRIVATE "-Wno-implicit-int")
	target_compile_options(iode_scr4 PRIVATE "-Wno-implicit-function-declaration")
endif()

if(TARGET iode_scr4)
	message(STATUS "New target iode_scr4 enabled\n")
else()
	message(WARNING "Could not create the target iode_scr4\n")
endif()
