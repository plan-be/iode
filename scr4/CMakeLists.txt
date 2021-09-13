# CMakeList.txt : CMake project for scr4, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.16)

project ("scr4")

set (HEADER_IODE 
	 s_a2m.h s_alldoc.h s_args.h s_calc.h  s_cpp.h s_date.h s_dir.h s_file.h s_html.h s_ini.h s_isc.h
	 s_lst.h s_mat.h s_mmt.h s_odbc.h s_proa2m.h s_proarg.h s_probas.h s_probis.h s_proclc.h s_procpp.h s_prodir.h s_prodt.h s_profil.h s_prohtm.h s_proini.h
	 s_prolst.h s_prolzh.h s_proma.h s_proscl.h s_prost.h s_prosw.h s_protar.h s_provf.h s_prowin.h s_prowis.h s_proxdr.h s_proy.h
	 s_scrold.h s_scroll.h s_strs.h s_swap.h s_tar.h s_vf.h s_xdr.h s_yy.h scr.h scr4.h scr4_box.h scr4_d.h scr4_key.h scr4_rd.h scr4_str.h scr4g.h scr4w.h scr4web.h 
)

set (IODE_SWAP s_swallc.c s_swfile.c s_swab.c s_swsegs.c s_swems.c s_swmisc.c s_swvars.c)
set (IODE_LST s_lst1.c s_lst2.c)
set (IODE_BASE  s_vars.c s_dirstd.c s_dirspd.c s_dirsta.c s_dirfp.c s_dircwd.c s_dirj.c s_dirfmt.c  s_dirspn.c s_wsockc.c)  # s_dirrd.c s_dirtbl.c
set (IODE_INI s_rdini.c)

set (SCR_IODE ${IODE_SWAP} ${IODE_LST} ${IODE_BASE} ${IODE_INI})

# Add source to this project's executable.
add_library (scr4iode STATIC ${HEADER_IODE} ${SCR_IODE})

# Compiler options
add_compile_options(-Zi -Zp1)
add_compile_definitions(DOS VC WINDOWS_IGNORE_PACKING_MISMATCH WINDOWS DOSW32 REALD SCRPROTO IODEWIN NOEMS NOEMF)
target_include_directories (scr4iode PUBLIC ".")

# TODO: Add tests and install targets if needed.
