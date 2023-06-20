@echo off
:: MAKEPY.BAT
:: ----------
:: Create iode.pyd, the iode module for python, for the active version of python.
:: The generated module is copied in the sub directory /py{vers}, where {vers} is running python version, 
:: for example /py38, py39 or /py310.
:: 
:: The name of the Conda environment used to build iode.pyd must be passed as argument to the present script.
:: For example:
:: > makepy.bat py39
::
:: NOTES: make sure that the conda environment has numpy, pandas, larray and cython librairies installed.
::

if [%1]==[] goto :blank

SETLOCAL

SET DISTUTILS_USE_SDK=1
SET MSSdk=1

:: go to root directory
cd ..

echo conda environment %1

:: activate conda environment
call conda activate %1

:: configure CMake 
call cmake --preset windows-debug

:: Create iode.pyd
call cmake --build --preset windows-debug --target iode_python 

:: Digitally sign the .pyd module
:: DigiCertUtil.exe sign /noinput /sha1 "307c80ca0c69098522a09f40a1299e7c9c32ec85" iode.pyd

:: deactivate conda environment
call conda deactivate

ENDLOCAL

echo Done
exit /B

:blank
echo Failed: Conda environment must be passed
