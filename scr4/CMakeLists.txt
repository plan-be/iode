# CMakeList.txt : CMake project for scr4, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.16)

project ("scr4")

set (HEADER_IODE 
s_a2m.h
s_alldoc.h
s_args.h
s_calc.h
s_cpp.h
s_date.h
s_dir.h
s_file.h
s_html.h
s_ini.h
s_isc.h
s_lst.h
s_mat.h
s_mmt.h
s_odbc.h
s_proa2m.h
s_proarg.h
s_probas.h
s_probis.h
s_proclc.h
s_procpp.h
s_prodir.h
s_prodt.h
s_profil.h
s_prohtm.h
s_proini.h
s_prolst.h
s_prolzh.h
s_proma.h
s_proscl.h
s_prost.h
s_prosw.h
s_protar.h
s_provf.h
s_prowin.h
s_prowis.h
s_proxdr.h
s_proy.h
s_scrold.h
s_scroll.h
s_strs.h
s_swap.h
s_tar.h
s_vf.h
s_xdr.h
s_yy.h
scr.h
scr4.h
scr4_box.h
scr4_d.h
scr4_key.h
scr4_rd.h
scr4_str.h
scr4g.h
scr4w.h
scr4web.h
)

set (IODE_INI s_rdini.c)

set (SCR_IODE ${IODE_INI})

# Add source to this project's executable.
add_library (scr4iode STATIC ${HEADER_IODE} ${SCR_IODE})

target_include_directories (scr4iode PUBLIC ".")
# TODO: Add tests and install targets if needed.
