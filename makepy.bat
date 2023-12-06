@echo off
:: MAKEPY.BAT
:: ----------
:: Create a Python wheel file for the active version of python.
:: NOTES: make sure that the conda environment has numpy, pandas, larray and cython librairies installed.
::

SETLOCAL

SET DISTUTILS_USE_SDK=1
SET MSSdk=1

:: configure CMake 
call cmake --preset windows-release

:: Create a wheel file
call pip wheel .

:: install iode package from the wheel file
for %f in (iode*.whl) do ( 
    set filename=%f 
    echo %filename% 
    call pip install %filename% 
)

:: Test iode.pyd (using pytest -> see tests/test_python/test_iode.py)
call pytest

:: Digitally sign the .pyd module
:: DigiCertUtil.exe sign /noinput /sha1 "307c80ca0c69098522a09f40a1299e7c9c32ec85" iode.pyd

ENDLOCAL

echo Done
exit /B
