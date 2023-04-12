:: PYIODE unit testing
:: --------------------
@echo off
SETLOCAL

@echo.
@echo =================================================================================
@echo Testing pyiode
@echo --------------
@echo.

:: Testing py39/iode.pyd 
@echo.
@echo.
@echo Testing test_iode.py/py39
@echo --------------------------
call activate py39
if %errorlevel% NEQ 0 goto :err
python test_iode.py

:: Testing py320/iode.pyd 
@echo.
@echo.
@echo Testing test_iode.py/py310
@echo --------------------------
call activate py310
if %errorlevel% NEQ 0 goto :err
python test_iode.py

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