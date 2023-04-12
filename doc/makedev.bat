:: Creation of the development documentation files 
:: -----------------------------------------------
::
:: Convert *.m help files into markdown format
@echo off
SETLOCAL

:: Create ./build/dev
if not exist build md build
if not exist build\dev md build\dev

:: Goto to src/dev dir
cd src/dev

call :ConvertM_to_MD CONVERSIONS.m
call :ConvertM_to_MD DESCRIPTION.m
call :ConvertM_to_MD ESTIM.m
call :ConvertM_to_MD IDENTITIES.m
call :ConvertM_to_MD LEC.m
call :ConvertM_to_MD MAKE_IODE.m
call :ConvertM_to_MD PRINTING.m
call :ConvertM_to_MD REPORTS.m
call :ConvertM_to_MD SIMUL.m
call :ConvertM_to_MD SOURCES.m
call :ConvertM_to_MD TBL_CALC.m
call :ConvertM_to_MD IODE_TESTS.m
call :ConvertM_to_MD PYIODE.m

move /Y CONVERSIONS.md ..\..\build\dev
move /Y DESCRIPTION.md ..\..\build\dev
move /Y ESTIM.md       ..\..\build\dev
move /Y IDENTITIES.md  ..\..\build\dev
move /Y LEC.md         ..\..\build\dev
move /Y MAKE_IODE.md   ..\..\build\dev
move /Y PRINTING.md    ..\..\build\dev
move /Y REPORTS.md     ..\..\build\dev
move /Y SIMUL.md       ..\..\build\dev
move /Y SOURCES.md     ..\..\build\dev
move /Y TBL_CALC.md    ..\..\build\dev
move /Y IODE_TESTS.md  ..\..\build\dev
move /Y PYIODE.md      ..\..\build\dev

ENDLOCAL

goto :eof

:: Function(s)
:: -----------
:ConvertM_to_MD
:: Syntax: Call ConvertM_to_MD file.m
SETLOCAL
    set file_m=%1%
    set file_o_m=%file_m%.o.m
    set file_md=%file_m%d
   
    echo: 
    echo: 
    echo ===========================================
    echo TRANSLATING %file_m%
    echo -------------------------------------------
    copy %file_m% %file_o_m%
    :: Convert input ascii %file_m% to Code Page OEM850 (in place)
    scr4w_a2o %file_o_m%
    
    :: Compile %file_o_m% and generate intermediate file "tmp.hlp" (scr4 format)
	scr4w_h -e ; -f . -wm -s -c %file_o_m% -o tmp.hlp
    del %file_o_m%
    
    :: Translate tmp.hlp (scr4 help format) into tmp.a2m (scr4 a2m format)
	scr4w_hm -c ~ -e ; -it -i tmp.hlp -o tmp.a2m 
    del tmp.hlp
    
    :: Create tmp1.a2m by prepending some definitions (paragraphs...) to tmp.a2m
    ::@echo off
	echo .pardef parb html=3 > tmp1.a2m
	echo .pardef pari html=4 italic=no >> tmp1.a2m
	echo .pardef tit_0 justify=L html=1 size=12 >> tmp1.a2m
    echo .pardef cmd_1 color=black bold=n family=consolas size=9 >> tmp1.a2m
	type tmp.a2m >> tmp1.a2m
    del tmp.a2m
    ::@echo on
    
    :: Generate %file_md% from tmp1.a2m
	scr4w_amd -i tmp1.a2m -o %file_md% -toc 1 -paranum 0
    del tmp1.a2m
    
    echo:
ENDLOCAL
