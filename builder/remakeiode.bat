: Rebuild IODE and create installation programs
@echo on
setlocal

REM this script allows 5 optional arguments:
REM - objs: do not recompile the source files (only create .lib and .exe)
REM - cmd: compile only command line version of IODE
REM - man: compile the IODE documentation
REM - py: compile pyiode only
REM - vc64: use vc64 compiler instead of bc (Borland)
REM - vc32: use vc32

REM defaults to rebuild all source files
set objs=1
REM TODO: find a better way to do this (not duplicate %X for X=1,2,...)
if /I [%1] == [-objs] set objs=0
if /I [%2] == [-objs] set objs=0
if /I [%3] == [-objs] set objs=0
if /I [%4] == [-objs] set objs=0
if /I [%5] == [-objs] set objs=0
if /I [%6] == [-objs] set objs=0

REM defaults to build all (command line only = false)
set cmd_only=0
if /I [%1] == [-cmd] set cmd_only=1
if /I [%2] == [-cmd] set cmd_only=1
if /I [%3] == [-cmd] set cmd_only=1
if /I [%4] == [-cmd] set cmd_only=1
if /I [%5] == [-cmd] set cmd_only=1
if /I [%6] == [-cmd] set cmd_only=1

REM defaults to not build the documentation
set man=0
if /I [%1] == [-man] set man=1
if /I [%2] == [-man] set man=1
if /I [%3] == [-man] set man=1
if /I [%4] == [-man] set man=1
if /I [%5] == [-man] set man=1
if /I [%6] == [-man] set man=1

REM defaults to not only build pyiode (pyiode only = false)
set python_only=0
if /I [%1] == [-py] set python_only=1
if /I [%2] == [-py] set python_only=1
if /I [%3] == [-py] set python_only=1
if /I [%4] == [-py] set python_only=1
if /I [%5] == [-py] set python_only=1
if /I [%6] == [-py] set python_only=1

REM defaults to use Borland compiler
set compiler=vc64
if /I [%1] == [-bc] set compiler=bc
if /I [%2] == [-bc] set compiler=bc
if /I [%3] == [-bc] set compiler=bc
if /I [%4] == [-bc] set compiler=bc
if /I [%5] == [-bc] set compiler=bc
if /I [%6] == [-bc] set compiler=bc

if /I [%1] == [-vc32] set compiler=vc32
if /I [%2] == [-vc32] set compiler=vc32
if /I [%3] == [-vc32] set compiler=vc32
if /I [%4] == [-vc32] set compiler=vc32
if /I [%5] == [-vc32] set compiler=vc32
if /I [%6] == [-vc32] set compiler=vc32

REM --- old paths below ---
REM set iodepath=c:\usr\iode_src
REM set scr4path=c:\usr\scr4_src

set iodepath=%~dp0\..
set scr4path=%iodepath%\scr4


if [%python_only%] == [1] goto :pyiode


if [%compiler%] == [vc32] call %iodepath%\set32.bat
if [%compiler%] == [vc64] call %iodepath%\set64.bat


:: SCR
if [%objs%] == [1] (
    @echo ======== BUILD SCR ========
    if [%compiler%] == [bc] (
        cd %scr4path%
        del *.io
	    del *.w32
	    make -DCodeGear scr4iode s32wo
        if %errorlevel% NEQ 0 goto :err
    ) else (
        cd %scr4path%\%compiler%
        del *.obj
	    nmake s4iode.lib
	    if %errorlevel% NEQ 0 goto :err
        nmake s4wo.lib
        if %errorlevel% NEQ 0 goto :err
        nmake s4_app.exe
        if %errorlevel% NEQ 0 goto :err
    )
)


:: API
@echo ======== BUILD API ========
if [%compiler%] == [bc] (
    cd %iodepath%\api
    if [%objs%] == [1] del *.obj
    make
    if %errorlevel% NEQ 0 goto :err
) else (
    cd %iodepath%\api\%compiler%
    if [%objs%] == [1] del *.obj
    nmake
    if %errorlevel% NEQ 0 goto :err
)


:: CMD
@echo ======== BUILD CMD ========
if [%compiler%] == [bc] (
    cd %iodepath%\cmd
    if [%objs%] == [1] del *.obj
    make
    if %errorlevel% NEQ 0 goto :err
) else (
    cd %iodepath%\cmd\%compiler%
    if [%objs%] == [1] del *.obj
    nmake
    if %errorlevel% NEQ 0 goto :err
)


:: DOC
if [%man%] == [1] (
    @echo ======== BUILD DOC ========
    if [%compiler%] == [bc] (
	    cd %iodepath%\doc
	    make readme.htm iode.chm 
	    if %errorlevel% NEQ 0 goto :err
	    make iode.wiki
	    if %errorlevel% NEQ 0 goto :err
	    pause
    ) else (
        goto :err
    )
)


if [%cmd_only%] == [1] goto :succes 


:: DOS
@echo ======== BUILD DOS ========
if [%compiler%] == [bc] (
    cd %iodepath%\dos
    if [%objs%] == [1] del *.obj
    del o_objs.*
    make
    if %errorlevel% NEQ 0 goto :err
) else (
    goto :err
)


:: IODECOM
@echo ======== BUILD IODECOM ========
if [%compiler%] == [bc] (
    cd %iodepath%\iodecom
    call make64
    if %errorlevel% NEQ 0 goto :err
) else (
    goto :err
)


:: BUILD (requires NSIS tool to be installed !)
@echo ======== FINALIZE BUILD ========
cd %iodepath%\builder
if [%compiler%] == [bc] (
    make inst
) else (
    nmake inst
)
if %errorlevel% NEQ 0 goto :err


REM TODO: move all this below to makepy.bat
:: iode.pyd -- python dll 64 bits
:pyiode
setlocal
  @echo ======== BUILD PYIODE ========

  if [%compiler%] == [vc32] call %iodepath%\set32.bat
  if [%compiler%] == [vc64] call %iodepath%\set64.bat

  :: 1. scr4 libs
    @echo -------- BUILD S4IODE.LIB --------
    cd %scr4path%\%compiler%
    if [%objs%] == [1] del *.obj
    call nmake s4iode.lib
    if %errorlevel% NEQ 0 goto :err

  :: 2: iodelibs
    @echo -------- BUILD IODEAPI.LIB --------
    cd %iodepath%\api\%compiler%
    if [%objs%] == [1] del *.obj
    call nmake iodeapi.lib
    if %errorlevel% NEQ 0 goto :err

  :: 3. iode.pyd
    cd %iodepath%\pyiode\%compiler%

   @echo -------- BUILD IODE.PYD --------
   :: 3.1 python 3.8 => pyiode\vc64\py38\iode.pyd
     call activate base
     call makepy.bat
     if %errorlevel% NEQ 0 goto :err

   :: 3.2 python 3.6 => pyiode\vc64\py36\iode.pyd
    call activate py36
    call makepy.bat
    if %errorlevel% NEQ 0 goto :err
	
   :: 3.3 python 3.7 => pyiode\vc64\py37\iode.pyd
    call activate py37
    call makepy.bat
    if %errorlevel% NEQ 0 goto :err

endlocal


:succes
@echo !!! Success !!!
endlocal
goto :EOF

:err
@echo !!!! Error !!!!
cd %iodepath%\builder
endlocal
:fin
endlocal
