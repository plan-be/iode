:: Creation of iode.chm
:: --------------------
::
:: Compile iodew.m and generate the intermediate file "tmp.hlp" (scr4 help format)
:: Requires HelpHelp Workshop package: see https://www.helpandmanual.com/downloads_mscomp.html
SETLOCAL
@echo off

:: Goto to src dir
cd src

:: Compile iodew.m and generate intermediate file "tmp.hlp" (scr4 format)
scr4w_h -e ; -f . -wm -c iodew.m -o tmp -log tmp.lg

:: Translate tmp.hlp (scr4 help format) into tmp.a2m (scr4 a2m format)
scr4w_hm -c ~ -i tmp -o tmp.a2m -t Manueldelutilisateur Notestechniques -it
del tmp.hlp

:: Create iode.a2m by prepending some definitions (paragraphs...) to tmp.a2m
@echo off
echo .pardef parb html=5 color=blue > iode.a2m
echo .pardef pari html=6 color=blue >> iode.a2m
echo .pardef cmd_1 color=black bold=n >> iode.a2m
echo .topicslink case=no>> iode.a2m
type tmp.a2m >> iode.a2m
del tmp.a2m

:: Generate iode.htm from iode.a2m
@echo on
scr4w_ah -gif2bmp -i iode.a2m -o iode.htm -tcolor 0 -tborder 0 -hcw -title IODE -style stylechm.css -font Times

:: Generate iode.chm
"C:\Program Files (x86)\HTML Help Workshop\hhc" iode.hhp
move /Y iode.chm ..\build

:: Cleanup
@echo off
del T????.htm
del tmp.*
del iode.htm
del iode.a2m
del iode.hhp
del iode.hhk
del iode.hhc
del iode.toc.htm

ENDLOCAL