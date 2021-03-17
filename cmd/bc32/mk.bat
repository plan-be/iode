@echo off
set scrdir=\usr\scr4_src
set iodedir=\usr\iode_src
set compiler=bc32

cd %scrdir%\%compiler%
make s4iode.lib
cd %iodedir%\api\%compiler%
make
cd %iodedir%\cmd\%compiler%
make