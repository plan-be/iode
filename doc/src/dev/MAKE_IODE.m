
<IODE: rebuilding IODE installer>
IODE: rebuilding IODE installer
������������������������
..sep |
..esc ~
..ignore �

&EN <STEP 1 : Changer le num�ro de version> 
&EN <STEP 2 : Mise � niveau des sources des manuels>
&EN <STEP 3 : G�n�ration du manuel en pdf (optionnel)>
&EN <STEP 4 : pr�parer les pages start.txt et sidebar.txt pour site web de IODE>
&EN <STEP 5 : rebuild iode, manuals et installateurs>
&EN <STEP 6 : transf�rer la nouvelle version sur le site web de IODE>
>

<STEP 1 : Changer le num�ro de version>
STEP 1 : Changer le num�ro de version
��������������������������������������
La localisation des num�ros de version et des dates � modifier se trouve 
dans le fichier ~ciode_src/nsis/chvers~C.

On peut �diter chvers avec l'�diteur ~ce.bat~C (~cs32w_mt.exe~C) qui permet 
d'ouvrir les fichiers concern�s � la bonne ligne avec la touche ~cCtrl-F4~C. 

&CO
c:/>> cd /usr/iode_src/nsis
c:/usr/iode_src/nsis>> e chvers
&TX

~bFichier chvers mars 2023~B
������������������������������
Liste des fichiers suivi du num�ro de la ligne � modifier:

&CO
    ..\\api\\vers.h  1
    ..\\api\\iode.H  53       #define IODE_VERSION "IODE Modelling Software 4.44 - (c) 1994 Planning Office - Brussels"
    ..\\api\\iode.H  54
    ..\\api\\iode.H  55
    ..\\dos\\COMMON.F        4       #define VERSION "I.O.D.E. 4.44"
    ..\\dos\\COMMON.F        13          APPL_NAME   "I.O.D.E. Econometric development tools 4.44 (c) BUP 1994"
    ..\\dos\\COMMON.F        88        ... Version 4.44 - 01/94 
    ..\\dos\\COMMON.F        119          C_FN SCR_lock_screen("I.O.D.E. 4.44");
    ..\\iodecom\\frmMain.cpp   197    mLog->>Lines->>Add("IodeComServer v6.64 27/02/2023");
    ..\\doc\\src\\intro.m1 80
    iode_mui.nsi 5
    iode_mui.nsi 21
    iode_mui_upd.nsi 5
    iode_mui_upd.nsi 21
    iode.rtf
    iode_upd.rtf
    makeinst.bat 7
    ..\\doc\\src\\start.txt 19 =>> changer les versions + liens
    copy2ovh.bat 52     echo mput iode664*.exe                      >> upload_iode_ovh.ftp
&TX

>


<STEP 2 : Mise � niveau des sources des manuels>
STEP 2 : Mise � niveau des sources des manuels
�������������������������������������������������
Aller dans le dir iode_src/doc :

&CO
c:/>> cd /usr/iode_src/doc
&TX

Mise � niveau des fichiers sources du manuel dans ~cdoc/src/*.m~C et ~c*.m1~C:

&EN adapter ~creadme.m~C : ajouter les commentaires de la version + reporter dans le manuel si n�cessaire
Recopier par exemple une version pr�c�dente pour d�marrer la section de la nouvelle version.
&EN adapter ~ciodew.m~C : au d�but du fichier, ajouter une entr�e avec la nouvelle version) comme pour les versions pr�c�dentes.
&EN si le manuel en pdf va �tre reconstruit, adapter la date de publication dans intro.m1 (voir aussi chvers plus haut)
&EN si des fonctions ont �t� ajout�es piu modifi�es, les d�crire dans les fichiers sources (.m) correspondants.
Eventuellement ajout�er
&EN si certains �crans ont �t� ajout�s / modifi�s, faire les screen captures (en png) et les inclure dans les sources du manuel (.m)
>

<STEP 3 : g�n�ration du manuel en pdf (optionnel)>
STEP 3 : g�n�ration du manuel en pdf (optionnel)
�����������������������������������������������
Cette �tape n�cessite le programme Adobe/Framemaker.

En r�sum� :
&EN cr�er les fichiers mif dans ~cdoc/src~C
&EN �diter en FrameMaker
&EN sauver en PDF (si distiller install�) ou imprimer en PDF
&EN recopier iode6.pdf dans doc\\build\\iode.pdf

&TI 1. Cr�er le fichier mif (maker interchange format) dans ~cdoc/src~C
������������������������������������������������������
Dans iode_src/doc, lancer la commande :
&CO
 c:/usr/iode_src/doc>> makemif.bat
&TX

Cette commande va cr�er deux parties du manuel en format mif : iode1.mif et iode2.mif.
Ces deux fichiers seront cr��s dans doc/src.

&TI 2. Cr�er iode6.pdf
����������������������
&NO Note pr�alable

La d�coupe du manuel en pdf est un peu diff�rente de celle des fichiers iode.chm et du site wiki :
&EN on a un chapitre 8 suppl�mentaire qui contient les algorithmes utilis�s. 
&EN une partie "fixe" contient tout ce qui est �crans et graphiques (a2m, excel, apl, algo).
&EN la syntaxe des a2m est d�plac�e dans les annexes

Les fichiers .m et .m1 sont donc diff�rents pour l'output mif.

~uListe des fichiers n�cessaires � la g�n�ration du manuel IODE~U

&EN *.bmp files added (for Frame)
&EN iode-1.fm and iode2-.fn: most recent version of these chapters in Frame format (fm)�
&EN iode6.book: Frame book file
&EN iode6.ref.fm: catalog of paragraphs, fonts, pages...
&EN iode6Chapt10.fm: fixed chapter 10 (not generated)
&EN iode6TOC.fm: last version of the TOC of the document 
&EN iodea2m-int.fm: chapter on A2M program
&EN iodecover.fm: final manual cover page(s) 

~uMode d'emploi~U

Apr�s avoir ex�cut� la commande makemif.bat:
&EN cd doc/src
&EN Start FrameMaker
&EN Open iode/doc/src/iode6.book
&EN open iode/doc/src/iode6.ref.fm
&EN open iode/doc/src/iodecover.fm
&EN changer la date du cover
&EN Open iode/doc/src/iode1.mif et iode/doc/src/iode2.mif
&EN Dans iode1.mif : ajustements manuels
&EN2 revoir le chapitre "Trend Correction" : pour les graphiques formules, 
    supprimer le cadre et diminuer la taille du frame pour ajuster � la formule.
    Pour cela, 1/ ouvrir Graphics/Tools + Set Pen Pattern to "None" et 2/ View Borders 
    
&EN2 idem pour le chapitre "Impression de la d�finition d�objets"    
&EN Dans iode2.mif :
&EN2 supprimer la premi�re page avec Annexes (Delete text)
&EN2 forcer un saut de page pour l'annexe 1 (Special / Page Break / Top of the next available Page)
&EN Pour iode1.mif et iode2.mif :
&EN2 Alt+fio (File/Import/Formats) : choisir iode6.ref.fm comme formats de r�f�rence
&EN2 Alt+olm (Format/Page Layout/Master Page Usage) : Right /Left + Apply to All Pages
&EN Alt+fa (Save As) iode1.mif en iode6-1.fm et iode2.mif en iode6-2.fm
&EN Pour iode6.book : Alt+eb (book?Edit/Update) : appliquer les modifs dans toc
&EN sauver book, iodecover.fm, iode6toc.fm, iode6-1.fm, iode6-2.fm
&EN si distiller install� : save book as pdf (niveaux tit_1 � tit_4) ->> lent !
&EN sinon: print vers pdf file (dans ce cas, pas de TOC dans les lecteurs PDF)
&EN copier le pdf dans le directory des builds ~ccopy iode6.pdf ../build/iode.pdf~C
>

<STEP 4 : pr�parer les pages start.txt et sidebar.txt pour site web de IODE>
STEP 4 : pr�parer les pages start.txt et sidebar.txt pour site web de IODE
�����������������������������������
Avant de transf�rer sur le site, il faut adapter manuellement les fichiers suivants.

~bdoc/src/start.txt~B
������������������������
&EN Ajouter les liens vers les nouveaux installateurs dans la section "Download"
&EN Si d'application, ajouter dans la section "IODE for python" la version iode.pyd pour une nouvelle version de python
 
~bdoc/src/sidebar.txt~B
������������������������
Revoir cette section et y ajouter �ventuellement de nouveaux chapitres (par exemple pour le manuel de iode.pyd).
>

<STEP 5 : rebuild iode, manuals et installateurs>
STEP 5 : rebuild iode, manuals et installateurs
����������������������������������������������������

&NO Note pr�alable : il faut v�rifier que les makefiles qui cr�ent des ex�cutables (iode.exe, iodecmd.exe, iodecom.exe...)
soient bien ~usign�s~U avant de lancer la proc�dure de recompilation (on supprime en g�n�ral la signature pendant les d�veloppements pour 
que le debugger td32 puisse fonctionner).

Se placer dans ~c./nsis~C pour recontruire 
&EN les libs 
&EN les ex�cutables 
&EN les diff�rentes versions des manuels (sauf pdf) :

&CO
c:>> cd /usr/iode_src/nsis
c:/usr/iode_src/nsis>> remakeiode [-objs] [-man]
&TX

Les options -objs et -man permettent d'�viter la recompilation de toutes les sources et la g�n�ration des manuels, 
par exemple pendant une phase de mise au point.
&EN ~c-man~C : ne g�n�re par les manuels (readme.htm, iode.chm, pages du site wiki)
&EN ~c-objs~C : ne force pas la recompilation des sources .c et .cpp

Le programme g�n�re d'abord les librairies n�cessaires en 32 et 64 bits (64 bits uniquement pour pyiode). 

En sortie, les fichiers suivants auront �t� reg�n�r�s :

&EN  ./cmd/iodecmd.exe
&EN  ./dos/iode.exe
&EN  ./iodecom/iodecom.exe
&EN  ./pyiode/py39/iode.pyd
&EN  ./pyiode/py310/iode.pyd
&EN  ./doc/build/readme.htm 
&EN  ./doc/build/iode.chm
&EN  ./nsis/iode6xx.exe
&EN  ./nsis/iode6xx_upd.exe
&EN  ./<<local_htdocs>>/w-iode/data/pages/* 
&EN  ./<<local_htdocs>>/w-iode/data/media/* 

~bFichier remakeiode.bat (mars 2023)~B
�����������������������������������������
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
    
    set iodepath=c:\\usr\\iode_src
    ;set pyiodepath= %USERPROFILE%\\source\\repos\\iode\\pyiode
    set pyiodepath= %iodepath%\\pyiode
    set scr4path=c:\\usr\\scr4_src
    
    :: SCR Borland 32
    cd %scr4path%
    if [%objs%] == [1] (
        del *.io
        del *.w32
        make -DCodeGear scr4iode s32wo
        if %errorlevel% NEQ 0 goto :err
        )
    
    :: API Borland 32
    cd %iodepath%\\api
    if [%objs%] == [1] del *.obj
    make
    if %errorlevel% NEQ 0 goto :err
    
    :: CMD Borland 32
    cd %iodepath%\\cmd
    if [%objs%] == [1] del *.obj
    make
    if %errorlevel% NEQ 0 goto :err
    
    :: DOS Borland 32
    cd %iodepath%\\dos
    if [%objs%] == [1] del *.obj
    del o_objs.*
    make
    if %errorlevel% NEQ 0 goto :err
    
    :MAN
    if [%man%] == [1] (
        cd %iodepath%\\doc
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
    cd %iodepath%\\iodecom
    call make64
    if %errorlevel% NEQ 0 goto :err
    
    :: goto :EOF
    
    :: iode.pyd -- python dll 64 bits
    :pyiode
    call c:\\scr\\set64.bat
    
    :: 1. scr4 libs VC64
    cd %scr4path%\\vc64
    if [%objs%] == [1] del *.obj
    call nmake s4iode.lib
    if %errorlevel% NEQ 0 goto :err
    
    :: 2: iodelibs VC64
    cd %iodepath%\\api\\vc64
    if [%objs%] == [1] del *.obj
    call nmake iode_c_api.lib
    if %errorlevel% NEQ 0 goto :err
    
    :: 3. iode.pyd
    cd %iodepath%\\pyiode
    del iode.c
    
    :: python 3.9 =>> pyiode\\vc64\\py39\\iode.pyd
    call activate py39
    call makepy.bat
    if %errorlevel% NEQ 0 goto :err
    
    :: python 3.10 =>> pyiode\\vc64\\py10\\iode.pyd
    call activate py10
    call makepy.bat
    if %errorlevel% NEQ 0 goto :err
    
    :: NSIS
    cd %iodepath%\\nsis
    call makeinst.bat
    if %errorlevel% NEQ 0 goto :err
    
    @echo !!! Success !!!
    endlocal
    goto :EOF
    
    :err
    Echo !!!! Error !!!!
    cd %iodepath%\\nsis
    endlocal
    :fin
    endlocal
&TX
>


<STEP 6 : transf�rer la nouvelle version sur le site web de IODE>
STEP 6 : transf�rer la nouvelle version sur le site web de IODE
�����������������������������������
Pour cette �tape, le path local suivants est utilis� :
&EN WIKI_DIR=/apache24/htdocs/w-iode = localisation des fichiers g�n�r�s par ./doc/makewiki.bat.

Si n�cessaire, il faut l'adapter au d�but du fichier copy2ovh.bat list� ci-dessous.

Dans c:/usr/iode_src/nsis, lancer la commande :
&CO
    c:/usr/iode_src/nsis>> copy2ovh.bat
&TX

~bFichier copy2ovh.bat (mars 2023)~B
�����������������������������������������
&CO
@echo off
SETLOCAL
:: Copie le site IODE vers iode.plan.be

:: Define local source directory
set IODE_DIR=..
set PYIODE_DIR=%IODE_DIR%/pyiode
set WIKI_DIR=/apache24/htdocs/w-iode

:: Ouverture et mode
echo open ftp.cluster011.ovh.net    >> upload_iode_ovh.ftp
echo ibfpbevvwp                     >>>> upload_iode_ovh.ftp
echo <<ibfp password, cfr DvdW>>    >>>> upload_iode_ovh.ftp
echo binary                         >>>> upload_iode_ovh.ftp
echo prompt                         >>>> upload_iode_ovh.ftp


::goto pages

:: Python modules on wiki server
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

:: Installer
echo cd /www/w-iode/data/media/download         >>>> upload_iode_ovh.ftp
echo lcd %IODE_DIR%/nsis              >>>> upload_iode_ovh.ftp
echo mput iode665*.exe                          >>>> upload_iode_ovh.ftp

:: Manuals
echo cd /www/w-iode/data/media/download         >>>> upload_iode_ovh.ftp
echo lcd %IODE_DIR%/doc/build                   >>>> upload_iode_ovh.ftp
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

:images
echo cd /www/w-iode/data/media                  >>>> upload_iode_ovh.ftp
echo mkdir wiki                                 >>>> upload_iode_ovh.ftp
echo cd wiki                                    >>>> upload_iode_ovh.ftp
echo lcd %IODE_DIR%/doc/src                     >>>> upload_iode_ovh.ftp
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