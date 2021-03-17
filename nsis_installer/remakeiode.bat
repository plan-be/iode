: Rebuild IODE and create installation programs
@echo on
setlocal

set man=1
set objs=1
if /I [%1] == [-man] set man=0
if /I [%2] == [-man] set man=0
if /I [%1] == [-objs] set objs=0
if /I [%2] == [-objs] set objs=0

set iodepath=c:\usr\iode_src
set scr4path=c:\usr\scr4_src

:: SCR Borland 32
cd %scr4path%
if [%objs%] == [1] (
	del *.io
	del *.w32
	make -DCodeGear scr4iode s32wo
	if %errorlevel% NEQ 0 goto :err
	)

:: API Borland 32
cd %iodepath%\api
if [%objs%] == [1] del *.obj
make
if %errorlevel% NEQ 0 goto :err

:: CMD Borland 32
cd %iodepath%\cmd
if [%objs%] == [1] del *.obj
make
if %errorlevel% NEQ 0 goto :err

:: DOS Borland 32
cd %iodepath%\dos
if [%objs%] == [1] del *.obj
del o_objs.*
make
if %errorlevel% NEQ 0 goto :err

:: MAN
if [%man%] == [1] (
	cd %iodepath%\man
	make readme.htm iode.chm 
	if %errorlevel% NEQ 0 goto :err
	make iode.wiki
	if %errorlevel% NEQ 0 goto :err
	pause
	)

:: IODECOM
cd %iodepath%\iodecom
call make64
if %errorlevel% NEQ 0 goto :err

:: iode.pyd -- python dll 64 bits
:pyiode
setlocal
  call c:\scr\set64.bat

  :: 1. scr4 libs VC64
    cd %scr4path%\vc64
    if [%objs%] == [1] del *.obj
    call nmake s4iode.lib
    if %errorlevel% NEQ 0 goto :err

  :: 2: iodelibs VC64
    cd %iodepath%\api\vc64
    if [%objs%] == [1] del *.obj
    call nmake iodeapi.lib
    if %errorlevel% NEQ 0 goto :err

  :: 3. iode.pyd
    cd %iodepath%\pyiode\vc64

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

:: NSIS
cd %iodepath%\nsis_installer
make inst
if %errorlevel% NEQ 0 goto :err

@echo !!! Success !!!
endlocal
goto :EOF

:err
Echo !!!! Error !!!!
cd %iodepath%\nsis_installer
endlocal
:fin
endlocal
