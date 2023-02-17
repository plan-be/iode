@echo off
SETLOCAL
SET DISTUTILS_USE_SDK=1
SET MSSdk=1

:: Current default value (python 3.9)
set src=iode.cp39-win_amd64.pyd
set dest=py39\iode.pyd

::if "%CONDA_DEFAULT_ENV%" == "base" (
::		set src=iode.cp38-win_amd64.pyd
::		set dest=py38\iode.pyd
::)

if "%CONDA_DEFAULT_ENV%" == "py35" (
		set src=iode.cp35-win_amd64.pyd
		set dest=py35\iode.pyd
)

if "%CONDA_DEFAULT_ENV%" == "py36" (
        set src=iode.cp36-win_amd64.pyd
        set dest=py36\iode.pyd
)

if "%CONDA_DEFAULT_ENV%" == "py37" (
        set src=iode.cp37-win_amd64.pyd
        set dest=py37\iode.pyd
)

if "%CONDA_DEFAULT_ENV%" == "py39" (
        set src=iode.cp39-win_amd64.pyd
        set dest=py39\iode.pyd
)

python.exe setup.py build_ext --inplace --compiler=msvc
copy /y %src% %dest%
del iode.pyd
move /y %src% iode.pyd

::echo src : %src%
::echo dest : %dest% 
ENDLOCAL