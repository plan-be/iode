::  Creation of iode wiki
::  ---------------------
::  scr4w_ad.exe generates .txt pages in a format suitable for the dokuwiki (https://iode.plan.be) web site.
::  This program reads iodew.m (format scr4 manuals) and creates one file (page) by topic.
::  The resulting pages are saved in a local directory (to be defined below in the local variable WIKIDIR).
::      - %WIKIDIR%\pages = topics
::      - %WIKIDIR%\media\wiki = images
SETLOCAL
@echo off

:: Goto to src dir
cd src

:: Specify the WIKI output directory 
set WIKIDIR=C:\Apache24\htdocs\w-iode\data
:: set WIKIDIR=c:\tmp\wikiiode

:: Create the (sub)directories
if not exist %WIKIDIR%             md %WIKIDIR%
if not exist %WIKIDIR%\pages       md %WIKIDIR%\pages
if not exist %WIKIDIR%\media       md %WIKIDIR%\media
if not exist %WIKIDIR%\media\wiki   md %WIKIDIR%\media\wiki

:: Compile iodew.m and generate intermediate file "tmp.hlp" (scr4 format)
..\bin\scr4w_h -e ; -f . -wm -c iodew.m -o tmp 

:: Translate tmp.hlp (hlp format) into tmp.a2m (a2m format)
..\bin\scr4w_hm -c ~ -i tmp -o tmp.a2m -t Manueldelutilisateur Notestechniques -it
del tmp.hlp

:: Adds some informations (paragraph definition...) on top of the final iode.a2m 
@echo off
echo .pardef parb html=5 color=blue > iode.a2m
echo .pardef pari html=6 color=blue >> iode.a2m
echo .pardef cmd_1 color=black bold=n >> iode.a2m
echo .topicslink case=no>> iode.a2m
type tmp.a2m >> iode.a2m
del tmp.a2m

:: Generate dokuwiki files (*.txt) in tmp dir ./tmp
@echo on
if not exist tmp md tmp
cd tmp
move /Y ..\iode.a2m .
..\..\bin\scr4w_ad -gif2bmp -autolink 1 -paranum 0 -i iode.a2m  -title IODE -ini ..\doku.ini -toc 2
del iode.a2m
cd ..

:: move *.txt files to pages subdir
move /Y tmp\*.txt %WIKIDIR%\pages

:: copy src/start.txt and src/sidebar.txt to pages subdir
copy /Y start.txt %WIKIDIR%\pages
copy /Y sidebar.txt %WIKIDIR%\pages

:: copy images to media\wiki subdir
copy /Y *.jpg %WIKIDIR%\media\wiki
copy /Y *.gif %WIKIDIR%\media\wiki
copy /Y *.png %WIKIDIR%\media\wiki

:: Cleanup 
del /Q tmp\*.*
rd tmp

ENDLOCAL