:: IODECMD basic tests
:: -------------------
@echo off
SETLOCAL

@echo.
@echo =================================================================================
@echo Testing iodecmd
@echo ---------------
@echo.

:: call mk.bat
:: if %errorlevel% NEQ 0 goto :err

del unittests.log

call unittests1 args.rep 10 
if %errorlevel% NEQ 0 goto :err

call unittests1 example.bug.rep
if %errorlevel% NEQ 0 goto :err            

call unittests1 example.convert.rep        
if %errorlevel% NEQ 0 goto :err

call unittests1 example.csv.rep            
if %errorlevel% NEQ 0 goto :err

call unittests1 example.foreach.rep           
if %errorlevel% NEQ 0 goto :err

call unittests1 example.foreach2.rep          
if %errorlevel% NEQ 0 goto :err

call unittests1 example.proc.rep              
if %errorlevel% NEQ 0 goto :err

@echo.
@echo COMPARAISON
@echo -----------
fc unittests.log unittests.ref.log
if %errorlevel% NEQ 0 goto :err

::@echo Delete generated files
::@echo -----------------------
del ex.ansi.utf8
del ex.oem.utf8
del ex.ansi.oem
del ex.oem.ansi
del testcsv.csv
del test.csv
del test.log
del unittests.log

:: All tests succeeded 
:: -------------------
@echo.
@echo.
@echo -------------------
@echo All tests succeeded
@echo -------------------
endlocal
goto :EOF

:: Some tests failed
:: -----------------
:err
@echo.
@echo.
@echo -----------------
@echo Some tests failed
@echo -----------------

ENDLOCAL

:: call iodecmd -y -alloclog test.log resfun.rep                  
::call iodecmd -y -alloclog test.log example.excel.rep           
::call iodecmd -y -alloclog test.log testlong.rep                
