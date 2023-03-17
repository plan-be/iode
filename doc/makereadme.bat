:: Creation of the readme.htm file (IODE version summary)
:: ------------------------------------------------------
@echo off
SETLOCAL

:: Goto to src dir
cd src

:: Compile readme.m and generate intermediate file "tmp.hlp" (scr4 format)
scr4w_h -e ; -f . -wm -c readme.m -o tmp.hlp

:: Translate tmp.hlp (hlp format) into tmp.a2m (a2m format)
scr4w_hm -c ~ -i tmp

:: Adds some informations (paragraph definitions...) at the top of the result file "tmp1.a2m"
del tmp.hlp
echo .pardef parb html=2 bold=yes > tmp1.a2m
echo .pardef pari html=3 bold=yes >> tmp1.a2m
type tmp.a2m >> tmp1.a2m

:: Generate tmp1.htm file (=tmp1.a2m in html) 
scr4w_ah -i tmp1.a2m -o tmp1.htm -fs 8 -title "IODE Last Updates" -style stylereadme.css -font T -strip 1
del tmp1.a2m

:: Extract the TOC from tmp1.htm and save in toc.htm
scr4w_toc -i tmp1.htm -o toc.htm -l 1

:: Copy the template readme.ht1 into readme.htm
copy readme.ht1 readme.htm

:: Optionally substitute in readme.htm the section "<!STARTTOC>" - "<!ENDTOC>" by the content of toc.htm
:: ..\bin\s4sbs -s "<!STARTTOC>" "<!ENDTOC>" toc.htm readme.htm
del toc.htm

:: Substitute in readme.htm the section between "<!STARTTEXT>" and "<!ENDTEXT>" by the generated file tmp1.htm 
..\bin\s4sbs -s "<!STARTTEXT>" "<!ENDTEXT>" tmp1.htm readme.htm
del tmp1.htm
del scr4_sbs.$$$
move /Y readme.htm ..\build

ENDLOCAL