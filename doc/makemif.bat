:: Creation of iode1.mif and iode2.mif in doc/src to create iode.pdf
:: -----------------------------------------------------------------
::
SETLOCAL
@echo off

:: Goto to src dir
cd src

:: Size of the rectangle containing picture in iode1.mif and iode2.mif
set GRECT=13 9.5

:: DPI of the screen captures in iode1.mif and iode2.mif
set GDPI=140

:: -----------------------------------------------------------------
:: STEP 1. iode1.mif
:: -----------------------------------------------------------------

:: Compile iodemif1.m and generate the intermediate file "tmp.hlp" (scr4 help format)
scr4w_h -e ; -f . -wm -c iodewmif1.m -o tmp 

:: Translate tmp.hlp (scr4 help format) into tmp.a2m (scr4 a2m format)
:: Goto to src dir
cd src
scr4w_hm -c ~ -i tmp -o tmp.a2m -t Manueldelutilisateur -it
del tmp.hlp

:: Create iodemif1.a2m by prepending some definitions (paragraphs...) to tmp.a2m
:: echo .pardef parb html=5 color=blue > iode.a2m
:: echo .pardef pari html=6 color=blue >> iode.a2m
@echo off
echo .pardef parb color=blue > iodemif1.a2m
echo .pardef pari color=blue >> iodemif1.a2m
echo .topicslink case=no>> iodemif1.a2m
type tmp.a2m >> iodemif1.a2m
del tmp.a2m
@echo on
:: Create iode1.mif from iodemif1.a2m
scr4w_af -gdpi %GDPI% -grect %GRECT% -gif2bmp -i iodemif1.a2m -o iode1.mif 
del iodemif1.a2m

:: -----------------------------------------------------------------
:: STEP 2. iode2.mif
:: -----------------------------------------------------------------

:: Compile iodemif2.m and generate the intermediate file "tmp.hlp" (scr4 help format)
scr4w_h -e ; -f . -wm -c usermanmif2.m -o tmp 

:: Translate tmp.hlp (scr4 help format) into tmp.a2m (scr4 a2m format)
scr4w_hm -c ~ -i tmp -o tmp.a2m -t Annexes1 -it
del tmp.hlp

:: Create iodemif2.a2m by prepending some definitions (paragraphs...) to tmp.a2m
@echo off
::        echo .pardef parb html=5 color=blue > iode.a2m
::        echo .pardef pari html=6 color=blue >> iode.a2m
echo .pardef parb color=blue > iodemif2.a2m
echo .pardef pari color=blue >> iodemif2.a2m
echo .topicslink case=no>> iodemif2.a2m
type tmp.a2m >> iodemif2.a2m
del tmp.a2m

:: Create iode1.mif from iodemif2.a2m
@echo on
scr4w_af -gdpi %GDPI% -grect %GRECT% -gif2bmp -i iodemif2.a2m -o iode2.mif
del iodemif2.a2m


ENDLOCAL
