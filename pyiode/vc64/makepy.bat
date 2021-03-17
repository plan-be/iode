@echo off
SETLOCAL
SET DISTUTILS_USE_SDK=1
SET MSSdk=1

if "%CONDA_DEFAULT_ENV%" == "base" (
	set src=iode.cp38-win_amd64.pyd
	set dest=py38\iode38.pyd
)

if "%CONDA_DEFAULT_ENV%" == "py35" (
	set src=iode.cp35-win_amd64.pyd
	set dest=py35\iode35.pyd
)

if "%CONDA_DEFAULT_ENV%" == "py36" (
        set src=iode.cp36-win_amd64.pyd
        set dest=py36\iode36.pyd
)

if "%CONDA_DEFAULT_ENV%" == "py37" (
        set src=iode.cp37-win_amd64.pyd
        set dest=py37\iode37.pyd
)

python.exe setup.py build_ext --inplace --compiler=msvc
move /y %src% %dest%
::echo src : %src%
::echo dest : %dest% 
ENDLOCAL