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
::   If -vs is passed as arguement:
::      - the program is run in the subdir iode/pyiode of the Visual Studio directory 
::        structure. In this case, the libraries iodeapi.lib and **scr4iode.lib** must have been updated **before**
::        starting the program.
::      - the environment variable ISVS is set to -vs to indicate to cythonize.py that the environment is VS (s4iode.lib...)
::              
::   
::   If -vs is **not** present, the libs iodeapi.lib and **s4iode.lib** are updated before cythoning iode.pyx.
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

:: Only for the non Visual Studio version, remake the 64 bits libs iodeapi.lib and s4iode.lib

:: Set the environment vars for MSVC 
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" (
	call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
)

if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
	call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
)

if %errorlevel% NEQ 0 goto :EOF

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

:: Rename the generated file, sign it and copy it to py<nn>
set src=iode.cp%python_version%-win_amd64.pyd
move /Y %src% iode.pyd

:: Digitally sign the .pyd module
DigiCertUtil.exe sign /noinput /sha1 "307c80ca0c69098522a09f40a1299e7c9c32ec85" iode.pyd

:: Copy in subdir py<vers>
copy /y iode.pyd %python_name%


:: Take a local copy of iode.pyd
::copy /y %src% iode.pyd

ENDLOCAL