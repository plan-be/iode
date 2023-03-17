
<IODE: rebuilding IODE installer>
IODE: rebuilding IODE installer
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..sep |
..esc ~
..ignore ¯

&EN <STEP 1 : Changer le numéro de version> 
&EN <STEP 2 : préparation des manuels (chm et wiki)>
&EN <STEP 3 : génération du manuel en pdf (optionnel)>
&EN <STEP 4 : préparer les pages start.txt et sidebar.txt pour site web de IODE>
&EN <STEP 5 : rebuild iode, manuals et installateurs>
&EN <STEP 6 : transférer la nouvelle version sur le site web de IODE>
>

<STEP 1 : Changer le numéro de version>
STEP 1 : Changer le numéro de version
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
La localisation des numéros de version et des dates à modifier se trouve dans le fichier ~ciode_src/nsis_installer/chvers~C.

On peut éditer chvers avec l'éditeur e.bat (mt) qui permet d'ouvrir les fichiers concernés à la bonne ligne 
avec la touche ~cCtrl-F4~C. 

&CO
c:/>> cd /usr/iode_src/nsis_installer
c:/usr/iode_src/nsis_installer>> e chvers
&TX

~bFichier chvers en mars 2023~B
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯

&CO
    ..\api\vers.h  1
    ..\api\iode.H  37       #define IODE_VERSION "IODE Modelling Software 4.44 - (c) 1994 Planning Office - Brussels"
    ..\api\iode.H  38
    ..\api\iode.H  39
    ..\dos\COMMON.F        4       #define VERSION "I.O.D.E. 4.44"
    ..\dos\COMMON.F        13          APPL_NAME   "I.O.D.E. Econometric development tools 4.44 (c) BUP 1994"
    ..\dos\COMMON.F        88      ?++  ¯_ ++ ?++ ?++ ?++_¦ ++ ?¯--_ ++   Version 4.44 - 01/94 ?
    ..\dos\COMMON.F        119          C_FN SCR_lock_screen("I.O.D.E. 4.44");
    ..\dos\version.txt     1      IODE 6.46
    ..\iodecom\frmMain.cpp   197    mLog->>Lines->>Add("IodeComServer v6.64 27/02/2023");
    ..\man\src\intro.m1 80
    iode_mui.nsi 5
    iode_mui.nsi 21
    iode_mui_upd.nsi 5
    iode_mui_upd.nsi 21
    iode.rtf
    iode_upd.rtf
    makeinst.bat 7
    ..\man\src\start.txt 19 =>> changer les versions + liens
    copy2ovh.bat 52     echo mput iode664*.exe                      >> upload_iode_ovh.ftp
&TX
>


<STEP 2 : Préparation des manuels (chm et wiki)>
STEP 2 : Préparation des manuels (chm et wiki)
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Aller dans le dir iode_src/man :

&CO
c:/>> cd /usr/iode_src/man
&TX

Préparation de la mise à niveau du manuel (fichiers .m)
&EN adapter readme.m : ajouter les commentaires de la version + reporter dans le manuel si nécessaire
&EN adapter iodew.m (ajouter au début du fichier la ligne avec la nouvelle version
&EN adapter la date de publication dans intro.m1 (voir aussi chvers plus haut)
&EN créer les images des nouveaux écrans iode si nécessaire, sauver en bmp ? ou png ? et les inclure dans les sources du manuel (.m)
>

<STEP 3 : génération du manuel en pdf (optionnel)>
STEP 3 : génération du manuel en pdf (optionnel)
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Cette étape nécessite le programme Adobe/Framemaker.

En résumé :
&EN créer les fichiers mif
&EN éditer en Frame
&EN sauver en PDF
&EN recopier dans man

&TI 1. Créer le fichier mif (maker interchange format) 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
**** En cours de révision ****

Dans iode_src/man, lancer la commande :
&CO
 c:/usr/iode_src/man>> makemif.bat
 c:/usr/iode_src/man>> copy iode?.mif iodeman
&TX

&TI 2. Créer iode6.pdf
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Note préalable

La découpe du manuel en pdf est un peu différente de celle en chm et en wiki :
&EN on a un chapitre 8 avec les algorithmes
&EN une partie "fixe" contient tout ce qui est écrans et graphiques (a2m, excel, apl, algo).
&EN la syntaxe des a2m est déplacée dans les annexes

Les fichiers .m et .m1 sont donc différents entre la sortie mif et les autres.

~uMode d'emploi~U

Dans usr/iode_src/man/iodeman
&EN Open Frame
&EN Open c:/iode/iodeman/iode6.book
&EN open iode6.ref.fm
&EN open iodecover.fm
&EN changer la date du cover
&EN Open iode1.mif et iode2.mif
&EN Dans iode2.mif :
&EN supprimer la première page avec Annexes1
&EN forcer un saut de page pour l'annexe 1
&EN Pour les deux docs :
&EN2 Alt+fio (File/Import/Formats) : choisir iode6.ref.fm
&EN2 Alt+olm (Format/Page Layout/Master Page Usage) : Right /Left + Apply to All Pages
&EN Alt+fa (Save As) iode1.mif en iode6-1.fm et iode2.mif en iode6-2.fm
&EN Pour iode6.book : Alt+eb (book?Edit/Update) : appliquer les modifs dans toc
&EN sauver book, iodecover.fm, iode6toc.fm, iode6-1.fm, iode6-2.fm
&EN si distiller installé : save book as pdf (niveaux tit_1 à tit_4) ->> lent !
&EN sinon: print vers pdf file (dans ce cas, pas de TOC dans les lecteurs PDF)
&EN recopier iode6.pdf dans man sous le nom iode.pdf : ~ccopy iode6.pdf ../iode.pdf~C
>

<STEP 4 : préparer les pages start.txt et sidebar.txt pour site web de IODE>
STEP 4 : préparer les pages start.txt et sidebar.txt pour site web de IODE
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Avant de transférer sur le site, il faut adapter manuellement les fichiers suivants.

~bman/src/start.txt~B
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&EN Ajouter les liens vers les nouveaux installateurs dans la section "Download"
&EN Si d'application, ajouter dans la section "IODE for python" la version iode.pyd pour une nouvelle version de python
 
~bman/src/sidebar.txt~B
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Revoir cette section et y ajouter éventuellement de nouveaux chapitres (par exemple pour le manuel de iode.pyd).
>

<STEP 5 : rebuild iode, manuals et installateurs>
STEP 5 : rebuild iode, manuals et installateurs
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯

&NO Note préalable : il faut vérifier que les makefiles qui créent des exécutables (iode.exe, iodecmd.exe, iodecom.exe...)
soient bien signés avant de lancer la procédure de recompilation (on supprime la signature pendant les développements pour 
que le debugger td32 puisse fonctionner).

Se placer dans ~ciode_src/nsis_installer~C pour recontruire 
&EN les libs 
&EN les exécutables 
&EN les différentes versions des manuels (sauf pdf) :

&CO
c:>> cd /usr/iode_src/nsis_installer
c:/usr/iode_src/nsis_installer>> remakeiode [-objs] [-man]
&TX

Les options -objs et -man permettent d'éviter la recompilation de toutes les sources et la génération des manuels, 
par exemple pendant une phase de mise au point.
&EN -man : ne génére par le manuel (iode.chm) et les pages du wiki
&EN -objs : ne compile pas les fichiers sources inchangés 

Le programme génère d'abord les librairies nécessaires en 32 et 64 bits (64 bits uniquement pour pyiode). 

En sortie, les fichiers suivants auront été regénérés :

&EN  cmd/iodecmd.exe
&EN  dos/iode.exe
&EN  iodecom/iodecom.exe
&EN  pyiode/py39/iode.pyd
&EN  pyiode/py310/iode.pyd
&EN  man/readme.htm 
&EN  man/iode.chm
&EN  nsis_installer/iode6xx.exe
&EN  nsis_installer/iode6xx_upd.exe
&EN  <<local_htdocs>>/w-iode/data/pages/* 
&EN  <<local_htdocs>>/w-iode/data/media/* 

~bFichier remakeiode.bat (mars 2023)~B
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&CO
    :: Rebuild IODE and create installation programs
    :: 
    SETLOCAL
    
    set man=1
    set objs=1
    if /I [%1] == [-man] set man=0
    if /I [%2] == [-man] set man=0
    if /I [%1] == [-objs] set objs=0
    if /I [%2] == [-objs] set objs=0
    
    set iodepath=c:\usr\iode_src
    ;set pyiodepath= %USERPROFILE%\source\repos\iode\pyiode
    set pyiodepath= %iodepath%\pyiode
    set scr4path=c:\usr\scr4_src
    
    :: goto :MAN
    
    :: SCR Borland 32
    cd %scr4path%
    if [%objs%] == [1] (
        del *.io
        del *.w32
        make -DCodeGear scr4iode s32wo
        if %errorlevel% NEQ 0 goto :err
        )
    
    :: API Borland 32
    cd %iodepath%\api
    if [%objs%] == [1] del *.obj
    make
    if %errorlevel% NEQ 0 goto :err
    
    :: CMD Borland 32
    cd %iodepath%\cmd
    if [%objs%] == [1] del *.obj
    make
    if %errorlevel% NEQ 0 goto :err
    
    :: DOS Borland 32
    cd %iodepath%\dos
    if [%objs%] == [1] del *.obj
    del o_objs.*
    make
    if %errorlevel% NEQ 0 goto :err
    
    :MAN
    if [%man%] == [1] (
        cd %iodepath%\man
        ::make readme.htm iode.chm 
        call makereadme.bat
        if %errorlevel% NEQ 0 goto :err
        call makechm.bat
        if %errorlevel% NEQ 0 goto :err
        ::make iode.wiki
        call makewiki.bat
        if %errorlevel% NEQ 0 goto :err
        )
    
    :: IODECOM
    cd %iodepath%\iodecom
    call make64
    if %errorlevel% NEQ 0 goto :err
    
    :: goto :EOF
    
    :: iode.pyd -- python dll 64 bits
    :pyiode
    call c:\scr\set64.bat
    
    :: 1. scr4 libs VC64
    cd %scr4path%\vc64
    if [%objs%] == [1] del *.obj
    call nmake s4iode.lib
    if %errorlevel% NEQ 0 goto :err
    
    :: 2: iodelibs VC64
    cd %iodepath%\api\vc64
    if [%objs%] == [1] del *.obj
    call nmake iodeapi.lib
    if %errorlevel% NEQ 0 goto :err
    
    :: 3. iode.pyd
    cd %iodepath%\pyiode
    del iode.c
    
    :: python 3.9 =>> pyiode\vc64\py39\iode.pyd
    call activate py39
    call makepy.bat
    if %errorlevel% NEQ 0 goto :err
    
    :: python 3.10 =>> pyiode\vc64\py10\iode.pyd
    call activate py10
    call makepy.bat
    if %errorlevel% NEQ 0 goto :err
    
    :: NSIS
    cd %iodepath%\nsis_installer
    call makeinst.bat
    if %errorlevel% NEQ 0 goto :err
    
    @echo !!! Success !!!
    endlocal
    goto :EOF
    
    :err
    Echo !!!! Error !!!!
    cd %iodepath%\nsis_installer
    endlocal
    :fin
    endlocal
&TX
>


<STEP 6 : transférer la nouvelle version sur le site web de IODE>
STEP 6 : transférer la nouvelle version sur le site web de IODE
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Dans iode_src/nsis_installer, lancer la commande :
&CO
    c:/usr/iode_src/nsis_installer>> copy2ovh.bat
&TX

~bFichier copy2ovh.bat (mars 2023)~B
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&CO
@echo off
SETLOCAL
:: Copie le site IODE vers iode.plan.be

:: Define local source directory
set IODE_DIR=/usr/iode_src
set PYIODE_DIR=%IODE_DIR%/pyiode
set WIKI_DIR=/apache24/htdocs/w-iode

:: Ouverture et mode
echo open ftp.cluster011.ovh.net    >> upload_iode_ovh.ftp
echo ibfpbevvwp                     >>>> upload_iode_ovh.ftp
echo <<ibfp password, cfr DvdW>>    >>>> upload_iode_ovh.ftp
echo binary                         >>>> upload_iode_ovh.ftp
echo prompt                         >>>> upload_iode_ovh.ftp


::goto pages

:: Python modules
echo cd /www/w-iode/data/media/download         >>>> upload_iode_ovh.ftp
echo mkdir py36                                 >>>> upload_iode_ovh.ftp
echo mkdir py37                                 >>>> upload_iode_ovh.ftp
echo mkdir py38                                 >>>> upload_iode_ovh.ftp
echo mkdir py39                                 >>>> upload_iode_ovh.ftp
echo mkdir py310                                >>>> upload_iode_ovh.ftp

echo lcd %PYIODE_DIR%/py36                      >>>> upload_iode_ovh.ftp
echo cd /www/w-iode/data/media/download/py36    >>>> upload_iode_ovh.ftp
echo mput iode.pyd                              >>>> upload_iode_ovh.ftp

echo lcd %PYIODE_DIR%/py37                      >>>> upload_iode_ovh.ftp
echo cd /www/w-iode/data/media/download/py37    >>>> upload_iode_ovh.ftp
echo mput iode.pyd                              >>>> upload_iode_ovh.ftp

echo lcd %PYIODE_DIR%/py38                      >>>> upload_iode_ovh.ftp
echo cd /www/w-iode/data/media/download/py38    >>>> upload_iode_ovh.ftp
echo mput iode.pyd                              >>>> upload_iode_ovh.ftp

echo lcd %PYIODE_DIR%/py39                      >>>> upload_iode_ovh.ftp
echo cd /www/w-iode/data/media/download/py39    >>>> upload_iode_ovh.ftp
echo mput iode.pyd                              >>>> upload_iode_ovh.ftp

echo lcd %PYIODE_DIR%/py310                     >>>> upload_iode_ovh.ftp
echo cd /www/w-iode/data/media/download/py310   >>>> upload_iode_ovh.ftp
echo mput iode.pyd                              >>>> upload_iode_ovh.ftp

::goto fini

:: Installer
echo cd /www/w-iode/data/media/download         >>>> upload_iode_ovh.ftp
echo lcd %IODE_DIR%/nsis_installer              >>>> upload_iode_ovh.ftp
echo mput iode665*.exe                          >>>> upload_iode_ovh.ftp

:: Manuals
echo cd /www/w-iode/data/media/download         >>>> upload_iode_ovh.ftp
echo lcd %IODE_DIR%/man                         >>>> upload_iode_ovh.ftp
echo put iode.chm                               >>>> upload_iode_ovh.ftp
echo put iode.pdf                               >>>> upload_iode_ovh.ftp

:: Goodies
echo cd /www/w-iode/data/media/download         >>>> upload_iode_ovh.ftp
echo lcd %IODE_DIR%/goodies                     >>>> upload_iode_ovh.ftp
echo mput *                                     >>>> upload_iode_ovh.ftp

:pages

:: Wiki pages
echo cd /www/w-iode/data/pages                  >>>> upload_iode_ovh.ftp
echo lcd %WIKI_DIR%/data/pages                  >>>> upload_iode_ovh.ftp
echo mput *.txt                                 >>>> upload_iode_ovh.ftp

::goto fini

:images
echo cd /www/w-iode/data/media                  >>>> upload_iode_ovh.ftp
echo mkdir wiki                                 >>>> upload_iode_ovh.ftp
echo cd wiki                                    >>>> upload_iode_ovh.ftp
echo lcd %IODE_DIR%/man/src                     >>>> upload_iode_ovh.ftp
echo mput *.jpg                                 >>>> upload_iode_ovh.ftp
echo mput *.gif                                 >>>> upload_iode_ovh.ftp
echo mput *.png                                 >>>> upload_iode_ovh.ftp
            
:: Version          
echo cd /www/w-iode                             >>>> upload_iode_ovh.ftp
echo lcd %IODE_DIR%/dos                         >>>> upload_iode_ovh.ftp
echo put version.txt                            >>>> upload_iode_ovh.ftp
            
:fini           
echo quit                                       >>>> upload_iode_ovh.ftp


@echo on
ftp -s:upload_iode_ovh.ftp >> ftp_upload_iode_ovh.log

ENDLOCAL
&TX

That's all, folks !
>