@echo off
:: MAKEPY.BAT
:: ----------
:: Create iode.pyd, the iode module for python, for the active version of python.
:: The generated module is copied in the sub directory /py{vers}, where {vers} is running python version, 
:: for example /py38, py39 or /py310.
:: 
:: Syntax : makepy [-vs]
::    where -vs indicates the environment where the command is executed 
::     
::   If -vs is passed as arguement, the program is run in the subdir iode/pyiode of the Visual Studio directory 
::      structure. In this case, the libraries iodeapi.lib and scr4iode.lib must have been updated **before**
::      starting the program.
::   
::   If -vs is **not** present, the libs iodeapi.lib and s4iode.lib are updated before cythoning iode.pyx.
::   
:: Note that :
::  - the MSVC 64 bits environment must be initialised before starting this program (set64.bat will do the trick)
::  - the python environment must be initiated by a call to the conda command "activate", for ex. "activate py39".
::      If the python env is "base", the version name is replaced by py39 (change it for future python versions)

SETLOCAL

SET DISTUTILS_USE_SDK=1
SET MSSdk=1

:: The next line sets the environment variable ISVS that will be used in setupiode.py.
SET ISVS=%1%

:: Set the environment vars for MSVC 
::call set64.bat

:: Only for the non Visual Studio version, remake the 64 bits libs iodeapi.lib and s4iode.lib

if "%ISVS%" == "" (
    :: Make iodeapi.lib
    cd ..\api\vc64
    nmake 
    
    :: Make scr4iode.lib
    cd ..\..\..\scr4_src\vc64
    nmake s4iode.lib
    
    :: Return to the current directory
    cd ..\..\iode_src\pyiode
)

:: Define python_name and python_version
set python_name=%CONDA_DEFAULT_ENV%
if "%python_name%" == "base" set python_name=py39
if "%python_name%" == "" (
    echo Python is not activated.  Please call activate [python version.
    goto :EOF
)    
set python_version=%python_name:~2%

:: Create the sub directory for the current python version
md %python_name%

:: Cythonize the file iode.pyx
python.exe cythonize_iode.py build_ext --inplace --compiler=msvc

:: Copy the generated file locally and in py<nn>
set src=iode.cp%python_version%-win_amd64.pyd
set dest=%python_name%\iode.pyd
copy /y %src% %dest%
copy /y %src% iode.pyd

ENDLOCAL