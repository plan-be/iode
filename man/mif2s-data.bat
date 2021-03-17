@echo off
:: Rebuild IODE mif manuals and copy to s-data

:: Make mif files
make iode1.mif iode2.mif
if %errorlevel% NEQ 0 goto :err

:: Mount drive on s-data
net use x: /d
net use x: \\s-data\c$\usr\iode\iodeman /user:cic
if %errorlevel% NEQ 0 goto :err

:: Copy files
xcopy /D/Y iode1.mif x:
xcopy /D/Y iode2.mif x:
xcopy /D/Y c:*.bmp x:
xcopy /D/Y c:*.jpg x:
xcopy /D/Y c:*.png x:

:: Unmount
net use x: /d

@echo !!! Success !!!
goto :EOF

:err 
Echo !!!! Error !!!!