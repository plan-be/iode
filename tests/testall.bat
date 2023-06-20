:: IODE tests
:: ----------
:: Call all unit tests from ./test_c_api, ./cmd and ./pyiode
:: The result is stored in the log file testall.log
:: To rebuild the executables, call ./mk.bat

@echo off

SETLOCAL
set logfile=testall.log
if exist %logfile% del %logfile%

@echo IODE TESTS > %logfile%
@echo ---------- >> %logfile%
@echo.           >> %logfile%

@echo Rebuild iode_c_api, iodecmd and iode.exe.           >> %logfile%

:: tests/cmd
cd cmd
call unittests.bat >> ..\%logfile% 2>>&1 
if %errorlevel% NEQ 0 goto :err
cd ..

:: tests/test_c_api
cd test_c_api
call unittests.bat >> ..\%logfile% 2>>&1
if %errorlevel% NEQ 0 goto :err
cd ..

:: tests/pyiode
cd pyiode
call unittests.bat >> ..\%logfile% 2>>&1
if %errorlevel% NEQ 0 goto :err
cd ..

@echo.                      >> ..\%logfile% 2>>&1
@echo -------------------   >> ..\%logfile% 2>>&1
@echo All tests succeeded   >> ..\%logfile% 2>>&1
@echo -------------------   >> ..\%logfile% 2>>&1
@echo All tests succeeded  
endlocal
goto :EOF

:err
@echo.                      >> ..\%logfile% 2>>&1
@echo -----------------     >> ..\%logfile% 2>>&1
@echo Some tests failed     >> ..\%logfile% 2>>&1
@echo -----------------     >> ..\%logfile% 2>>&1
@echo Some tests failed  
ENDLOCAL