@echo off
call makechm.bat
if %errorlevel% NEQ 0 goto :err

call makereadme.bat
if %errorlevel% NEQ 0 goto :err

call makewiki.bat
if %errorlevel% NEQ 0 goto :err

call makemif.bat
if %errorlevel% NEQ 0 goto :err

call makedev.bat
if %errorlevel% NEQ 0 goto :err

@echo !!! Success !!!
goto :fin

:err
Echo !!!! Error !!!!

:fin
endlocal

