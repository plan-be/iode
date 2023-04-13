:: IODEAPI unit testing
:: --------------------
@echo off
SETLOCAL

@echo.
@echo =================================================================================
@echo Unit tests for iode api
@echo -----------------------
@echo.

: call mk.bat

:: Set the path according to the current compilation env
set exepath=..\..
if exist ..\exedir.txt set /P exepath=<..\exedir.txt
set exepath=%exepath:/=\%\tests\test_c_api

@echo %exepath%\test1.exe
%exepath%\test1.exe
if %errorlevel% NEQ 0 goto :err

%exepath%\test1cpp.exe
if %errorlevel% NEQ 0 goto :err

:: Delete generated files
:: ----------------------
del fun2.cm$
del fun2.eq$
del fun2.id$
del fun2.ls$
del fun2.sc$
del fun2.tb$
del fun2.va$
del fun2cmp.cm$
del fun2cmp.eq$
del fun2cmp.id$
del fun2cmp.ls$
del fun2cmp.sc$
del fun2cmp.tb$
del fun2cmp.va$
del test1_tbl.a2m
del simul.dbg
del test1_estim.a2m
del test_idt.htm
del fun2.cmt
del fun2.eqs
del fun2.idt
del fun2.lst
del fun2.scl
del fun2.tbl
del fun2.var
del fun2cmp.cmt
del fun2cmp.eqs
del fun2cmp.idt
del fun2cmp.lst
del fun2cmp.scl
del fun2cmp.tbl
del fun2cmp.var
del toto.a2m
del essais.txt

:: All tests succeeded 
:: -------------------
@echo !!! Success !!!
endlocal
goto :EOF

:: Some tests failed
:: -----------------
:err
Echo !!!! Some tests failed !!!!

ENDLOCAL