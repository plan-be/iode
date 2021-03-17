@echo off
:: Rebuild IODE html pages and copy to extra.plan.be
set PLANDIR=\\192.168.10.11\C$\data\apache-docs\html\new_planweb\websites\iodeman
set EXTRADIR=\\192.168.10.11\c$\data\apache-docs\html\extra\admin\uploaded

:: Rebuild html files
make iode.htm readme.htm
if %errorlevel% NEQ 0 goto :err

:: Mount drive on planweb
net use x: /d
net use x: \\192.168.10.11\C$ /user:cic
if %errorlevel% NEQ 0 goto :err

xcopy /D/Y frame.htm %PLANDIR%\index.htm
xcopy /D/Y styleweb.css %PLANDIR%
xcopy /D/Y styletoc.css %PLANDIR%
xcopy /D/Y iode.toc.htm %PLANDIR%
xcopy /D/Y *.gif %PLANDIR%
xcopy /D/Y *.jpg %PLANDIR%
xcopy /D/Y T*.htm %PLANDIR%
xcopy /D/Y readme.htm %PLANDIR%
net use x: /d
if %errorlevel% NEQ 0 goto :err

@echo !!! Success !!!
goto :EOF

:err 
@echo !!!! Error !!!!