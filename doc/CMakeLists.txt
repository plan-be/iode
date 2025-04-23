cmake_minimum_required (VERSION 3.16)

message(STATUS "\n---- DOC ----\n")

if(SKBUILD)
	message(STATUS "Skip generating documentation\n")
else()
    
# ---- generate conf.py (Python Sphinx) ----
# configure_file() substitutes variable values referenced as @VAR@ or ${VAR} in the input file content. 
# Each variable reference will be replaced with the current value of the variable, or the empty string 
# if the variable is not defined.
# see https://cmake.org/cmake/help/latest/command/configure_file.html#command:configure_file 
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/source/conf.py.in ${CMAKE_CURRENT_SOURCE_DIR}/source/conf.py)

add_custom_target(iode_doc
    COMMAND make html
    DEPENDS source/conf.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building Python (Sphinx) documentation using Python ${Python_VERSION}.\n"
)

add_subdirectory(keyboard_shortcuts)

    ## Add source to this project's executable.
    #set (MAN_SOURCES
    #	src/a2m.m            src/algo.m           src/apl.m
    #    src/ascii.m          src/bugs.m           src/compute.m        src/data.m
    #    src/dbin.m           src/dbout.m          src/demo.m           src/demo.m1
    #    src/ds.m             src/ecrans.m1        src/ext.m1           src/extl.m1
    #    src/extws.m1         src/file.m           src/filetype.m1      src/fiml.m
    #    src/format.m         src/help.m           src/in.m             src/inter.m
    #    src/interfaces.m     src/intro.m1         src/iode.m           src/iodecmd.m
    #    src/iodew.m          src/iodew95.m        src/iodewmif1.m      src/iodewmif2.m
    #    src/lec.m            src/lexique.m        src/licence.m
    #    src/licence.m1       src/methods.m        src/misc.m           src/mmt.m
    #    src/objs.m           src/organ.m          src/out.m            src/pode.m
    #    src/prereq.m1        src/print.m          src/printcap.m       src/qode.m
    #    src/readme.a.m       src/readme.m         src/readme.o.m       src/refman.m
    #    src/refman.m1        src/rep0.m           src/rep1.m           src/rep2.m
    #    src/repman.m         src/repman.m1        src/repn.m           src/report.m
    #    src/reports.m        src/repsyn.m         src/rtf.m            src/sample.a.m
    #    src/sample.m         src/sql.m            src/start.m          src/start1.m
    #    src/start2.m         src/start3.m         src/support.m        src/syntax.m
    #    src/s_a2msyn.m       src/tsf.m            src/userman.m        src/userman.m1
    #    src/usermanmif1.m    src/usermanmif1.m1   src/usermanmif2.m    src/usermanmif2.m1
    #    src/wspace.m         src/xls.m                                 
    #)
    #
    #set (DEV_SOURCES 
    #    src/dev/CONVERSIONS.m
    #    src/dev/DESCRIPTION.m
    #    src/dev/ESTIM.m
    #    src/dev/IDENTITIES.m
    #    src/dev/LEC.m
    #    src/dev/MAKE_IODE.m
    #    src/dev/PRINTING.m
    #    src/dev/REPORTS.m
    #    src/dev/SIMUL.m
    #    src/dev/SOURCES.m
    #    src/dev/TBL_CALC.m
    #)
    #
    #set (PNGS
    #    src/a2mgnl.png       src/gest1.png        src/gprvars.png      src/gvars1.png
    #    src/hp0.png          src/hp1.png          src/hp2.png          src/hp3.png
    #    src/hp4.png          src/hp5.png          src/hp6.png          src/hp7.png
    #    src/hp8.png          src/iodesyntax.png   src/mcomp.png        src/mdata.png
    #    src/mdata1.png       src/mdata2.png       src/mfile.png        src/mhelp.png
    #    src/miode.png        src/mlist.png        src/mmtrep.png       src/mprint.png
    #    src/mrep.png         src/mws.png          src/paa2m.png        src/pcmt.png
    #    src/pcompile.png     src/pdata.png        src/pdup.png         src/pedrep.png
    #    src/peqs.png         src/peqsres.png      src/pexec1.png       src/pexrep.png
    #    src/pexrepl.png      src/pfcomp.png       src/pfcont.png       src/pga2m.png
    #    src/pgraph.png       src/pha2m.png        src/pidt.png         src/piode.png
    #    src/pldprof.png      src/plist.png        src/plistc.png       src/plsort.png
    #    src/pma2m.png        src/ppa2m.png        src/ppdest.png       src/ppra2m.png
    #    src/pprgraphs.png    src/pprgraphs2.png   src/pprgrvars.png    src/pprobjs.png
    #    src/pprobjs2.png     src/pprreport.png    src/pprtbl.png       src/pprvbls.png
    #    src/pprvbls2.png     src/ppsetup.png      src/pqiode.png       src/pra2m.png
    #    src/presprof.png     src/pscan.png        src/pscanres.png     src/psearch.png
    #    src/psim1.png        src/psvprof.png      src/ptbl.png         src/ptblcr.png
    #    src/ptblgr.png       src/pvar.png         src/pwextrap.png     src/pwsclear.png
    #    src/pwscopy.png      src/pwsdescr.png     src/pwshtol.png      src/pwsload.png
    #    src/pwsltoh.png      src/pwsmerge.png     src/pwssample.png    src/pwssave.png
    #    src/pwsseas.png      src/pwstrend.png     src/pxmimpr.png      src/pxport.png
    #    src/sccdec.png       src/sccsim.png       src/sdata.png        src/sscal.png
    #    src/stables.png      src/svar.png         src/wsdescr.png      
    #)
    #
    #set (JPEGS 
    #    src/kaa1.jpg        src/kaa10.jpg       src/kaa11.jpg       src/kaa12.jpg
    #    src/kaa13.jpg       src/kaa14.jpg       src/kaa15.jpg       src/kaa16.jpg
    #    src/kaa17.jpg       src/kaa18.jpg       src/kaa19.jpg       src/kaa2.jpg
    #    src/kaa20.jpg       src/kaa21.jpg       src/kaa22.jpg       src/kaa23.jpg
    #    src/kaa24.jpg       src/kaa25.jpg       src/kaa26.jpg       src/kaa27.jpg
    #    src/kaa28.jpg       src/kaa293031.jpg   src/kaa3.jpg        src/kaa32.jpg
    #    src/kaa33.jpg       src/kaa34.jpg       src/kaa35.jpg       src/kaa36.jpg
    #    src/kaa36b.jpg      src/kaa37.jpg       src/kaa38.jpg       src/kaa4.jpg
    #    src/kaa5.jpg        src/kaa6.jpg        src/kaa7.jpg        src/kaa8.jpg
    #    src/kaa9.jpg        src/kaa_0.jpg       src/kaa_1.jpg       src/kaa_10.jpg
    #    src/kaa_11.jpg      src/kaa_12.jpg      src/kaa_13.jpg      src/kaa_14.jpg
    #    src/kaa_15.jpg      src/kaa_16.jpg      src/kaa_17.jpg      src/kaa_18.jpg
    #    src/kaa_19.jpg      src/kaa_2.jpg       src/kaa_20.jpg      src/kaa_21.jpg
    #    src/kaa_22.jpg      src/kaa_23.jpg      src/kaa_24.jpg      src/kaa_25.jpg
    #    src/kaa_26.jpg      src/kaa_27.jpg      src/kaa_28.jpg      src/kaa_29.jpg
    #    src/kaa_3.jpg       src/kaa_30.jpg      src/kaa_31.jpg      src/kaa_32.jpg
    #    src/kaa_33.jpg      src/kaa_34.jpg      src/kaa_35.jpg      src/kaa_35a.jpg
    #    src/kaa_36.jpg      src/kaa_37.jpg      src/kaa_38.jpg      src/kaa_39.jpg
    #    src/kaa_4.jpg       src/kaa_40.jpg      src/kaa_41.jpg      src/kaa_42.jpg
    #    src/kaa_43.jpg      src/kaa_44.jpg      src/kaa_45.jpg      src/kaa_46.jpg
    #    src/kaa_47.jpg      src/kaa_48.jpg      src/kaa_49.jpg      src/kaa_5.jpg
    #    src/kaa_50.jpg      src/kaa_6.jpg       src/kaa_7.jpg       src/kaa_8.jpg
    #    src/kaa_9.jpg       src/mdata.jpg                          
    #)
    #
    #set (GIFS src/listing.gif  src/title.gif)
    #set (CSS src/style.css src/stylechm.css src/stylereadme.css src/styletoc.css src/styleweb.css)
    #set (MISC src/macros.h src/readme.ht1 src/doku.ini)
    #set (SCRIPTS makechm.bat makereadme.bat makewiki.bat makemif.bat makedev.bat makeall.bat)
    #set (BINS 
    #    bin/s4sbs.exe bin/scr4w_ad.exe bin/scr4w_af.exe bin/scr4w_ah.exe 
    #    bin/scr4w_amd.exe bin/scr4w_h.exe bin/scr4w_hm.exe bin/scr4w_toc.exe
    #    bin/scr4w_o2a.exe bin/scr4w_a2o.exe
    #)
    #
    #add_custom_target(doc ALL SOURCES ${MAN_SOURCES} ${DEV_SOURCES} ${PNGS} ${JPEGS} ${GIFS} ${CSS} ${SCRIPTS} ${MISC})

endif()