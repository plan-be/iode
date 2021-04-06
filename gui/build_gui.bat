@echo on

set compiler=msvc
if /I [%1] == [-mingw] set compiler=mingw
if /I [%2] == [-mingw] set compiler=mingw
if /I [%3] == [-mingw] set compiler=mingw

set architecture=x86
if /I [%1] == [-x64] set architecture=x64
if /I [%2] == [-x64] set architecture=x64
if /I [%3] == [-x64] set architecture=x64

:: wether or not to (re)generate the Makefile 
set rebuild_exe=0
if /I [%1] == [-exe] set rebuild_exe=1
if /I [%2] == [-exe] set rebuild_exe=1
if /I [%3] == [-exe] set rebuild_exe=1

set iodepath=%~dp0\..
:: Note: modify the path below according to your Qt installation
set qtpath=C:\Qt

:: WARNING: from version 6, Qt does not longer support 32 bit architecture
if [%architecture%] == [x64] (
	set version_dir=6.0.3
) else (
	set version_dir=5.15.2
)

if [%compiler%] == [msvc] (
	if [%architecture%] == [x64] (
		call %iodepath%\set64.bat
		set compiler_dir=msvc2019_64
	) else ( 
		call %iodepath%\set32.bat
		set compiler_dir=msvc2019
	)
	set make=nmake
) else (
	if [%architecture%] == [x64] (
		set compiler_dir=mingw81_64
	) else (
		set compiler_dir=mingw81_32
	)
	set make=C:\MinGW\bin\mingw32-make.exe
)

:: Setting up environment for Qt usage
call %qtpath%\%version_dir%\%compiler_dir%\bin\qtenv2.bat
:: qtenv2.bat set current working directory to Qt installation directory
cd /d %~dp0

if [%rebuild_exe%] == [1] goto :build

:generate
:: generate the Makefile from the .pro file using QMake
qmake -o Makefile gui_iode.pro

:build
:: build executable
%make%
:: deploy for windows
windeployqt .\exe\gui_iode.exe