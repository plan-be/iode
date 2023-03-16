/* REPORT.M */

<Les Commandes de IODE dans les Rapports>
    Les Commandes de IODE dans les Rapports
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les commandes IODE permettent d'ex‚cuter pratiquement toutes les
fonctions de IODE. Elles sont regroup‚es par fonction:

&EN <Op‚rations sur des fichiers         >
&EN <Op‚rations sur les WS               >
&EN <Op‚rations sur les donn‚es          >
&EN <Op‚rations sp‚cifiques aux ‚quations>
&EN <Configuration de l'imprimante       >
&EN <Impressions d'objets                >
&EN <Compilation et impression de tables >
&EN <Graphiques … partir de tableaux     >
&EN <Op‚rations sur des modŠles          >
&EN <Ex‚cutions d'identit‚s              >
&EN <Op‚rations sur des rapports         >
&EN <Interface Excel                     >
&EN <Interface Datastream                >
&EN <Traduction des fichiers A2M         >
&EN <Autres fonctions de rapports        >

Pour rappel ‚galement, les commandes RAPPORT et les commandes IODE
peuvent ˆtre des commandes plein ‚cran ou des lignes de commande (sans
interface ‚cran).

Les commandes plein ‚cran sont constitu‚es d'un mot cl‚ pr‚c‚d‚ par le
caractŠre ##. Elles ont la syntaxe suivante :

&CO
    ##commande
&TX

Par exemple, la commande ##WsLoadVar provoquera l'affichage d'un ‚cran
de saisie permettant de pr‚ciser le nom du fichier de variables …
charger. Le fonctionnement de ces commandes est en g‚n‚ral d‚crit de
maniŠre d‚taill‚e dans le "Manuel de l'utilisateur". Lorsque c'est le
cas, il y a lieu de se reporter … ce manuel.

Les commandes en ligne sont constitu‚es d'un mot cl‚ pr‚c‚d‚ par le
caractŠre $. Elles ont la syntaxe suivante :

&CO
    $commande options paramŠtres
&TX

Par exemple, la commande
&CO
    $PrintTbl 84:8 TEST
&TX
imprimera le tableau TEST pour le sample indiqu‚.

Les mots cl‚s sont des mn‚moniques des fonctions ex‚cut‚es. Par exemple
le mot cl‚ WsCopyScl effectue une copie (Copy) d'un Work Space (Ws) de
scalaires (Scl). DataDuplicateIdt d‚double (Duplicate) une donn‚e (Data)
du WS des identit‚s (Idt).

Une table r‚sum‚e de la syntaxe de toutes les commandes de rapports peut
ˆtre trouv‚e en annexe.
>

<Op‚rations sur des fichiers>
    Op‚rations sur des fichiers
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les commandes op‚rant sur des fichiers permettent de copier, ‚diter,
effacer, renommer, imprimer, etc, tout fichier d'un type reconnu par
IODE. Les commandes sont les suivantes:

&EN <filelist  >
&EN <fileprint >
&EN <fileedit >
&EN <filedelete>
&EN <filerename>
&EN <filecopy >
&EN <SysCopyFile>
&EN <SysMoveFile>
&EN <SysDeleteFile>
&EN <SysOemToAnsi>
&EN <SysOemToUTF8>
&EN <SysAnsiToOem>
&EN <SysAnsiToUTF8>
&EN <SysAppendFile>
&EN <FileImportVar>
&EN <FileImportCmt>
>
<Op‚rations sur les WS>
    Op‚rations sur les WS
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Ces fonctions permettent de joindre, copier, charger et sauver les
fichiers work spaces :

&EN <WsLoad> :    charge un ws   cmt | eqs | idt | lst | scl | tbl | var
&EN <WsCopy>  :   copie un ws    cmt | eqs | idt | lst | scl | tbl | var
&EN <WsMerge> :   fusionne un ws cmt | eqs | idt | lst | scl | tbl | var
&EN <WsClear> :   d‚truit un ws  cmt | eqs | idt | lst | scl | tbl | var
&EN <WsClearAll> : vide tous les ws
&EN <WsDescr> :   d‚crit un ws   cmt | eqs | idt | lst | scl | tbl | var
&EN <WsSave>  :   sauve un ws    cmt | eqs | idt | lst | scl | tbl | var
&EN <WsSaveCmp> :  sauve un ws en comprimant cmt | eqs | idt | lst | scl | tbl | var
&EN <WsSample> :  change le sample du WS
&EN <WsExtrapolate> : prolonge des s‚ries par extrapolation
&EN <WsLtohStock> :  construit des s‚ries de p‚riodicit‚ sup‚rieure pour des stocks
&EN <WsLtohFlow> : construit des s‚ries de p‚riodicit‚ sup‚rieure pour des flux
&EN <WsHtolSum> : construit des s‚ries de p‚riodicit‚ inf‚rieure (somme)
&EN <WsHtolMean> : construit des s‚ries de p‚riodicit‚ inf‚rieure (moyenne)
&EN <WsHtolLast> : construit des s‚ries de p‚riodicit‚ inf‚rieure (derniŠre obs)
&EN <WsSeasonAdj> : construit des s‚ries d‚saisonnalis‚es
&EN <WsSeasAdj> : s‚lectionne le critŠre de saisonnalisation
&EN <WsTrend> : calcul de s‚ries de trend (Hodrick-Prescott) en passant au log
&EN <WsTrendStd> : calcul de s‚ries de trend (Hodrick-Prescott) par la m‚thode standard
&EN <WsImport> : importation d'un fichier ASCII
&EN <WsExport> : exportation en format ASCII
&EN <WsImportEviews> : importation d'‚quations et scalaires E-Views
&EN <CsvSave> : sauve un ws en format csv
&EN <CsvDigits> : fixe le nombre de d‚cimale dans les fichiers CSV
&EN <CsvSep> : fixe le s‚parateur de cellules dans les fichiers CSV
&EN <CsvDec> : fixe le s‚parateur de d‚cimales dans les fichiers CSV
&EN <CsvNaN> : fixe le texte indiquant une valeurs ind‚finie dans les fichiers CSV
&EN <CsvAxes> : fixe le nom de l'axe des variables dans les fichiers CSV
>

<Op‚rations sur les donn‚es>
    Op‚rations sur les donn‚es
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les commandes op‚rant sur des donn‚es des workspaces actifs (en m‚moire)
permettent de copier, ‚diter, effacer, renommer, imprimer, etc les
donn‚es d'un workspace:

&EN <datacreate> :    cr‚e un objet     (cmt | eqs | idt | lst | scl | tbl | var)
&EN <datadelete> :    d‚truit un objet  (cmt | eqs | idt | lst | scl | tbl | var)
&EN <dataexist>  :    teste un objet    (cmt | eqs | idt | lst | scl | tbl | var)
&EN <dataedit>   :    ‚dite un objet    (cmt | eqs | idt | lst | scl | tbl | var)
&EN <dataupdate> :    modifie un objet  (cmt | eqs | idt | lst | scl | tbl | var)
&EN <dataappend> :    ajoute un objet   (cmt | lst)
&EN <dataduplicate> : d‚double un objet (cmt | idt | lst | scl | tbl | var)
&EN <datarename> :    renomme un objet  (cmt | eqs | idt | lst | scl | tbl | var)

&EN <datasearch> :    recherche un objet  cmt | eqs | idt | lst | scl | tbl | var
&EN <datascan>   :    parcoure les objets eqs | idt | tbl

&EN <datalistXxx> :   cr‚e une liste d'objets dont les noms r‚pondent … un
	       critŠre donn‚  cmt | eqs | idt | lst | scl | tbl | var
&EN <datalistsort> :  trie une liste par ordre alphab‚tique
&EN <datacompareEps> : fixe le seuil d'‚galit‚ pour la comparaison des variables
&EN <datacompareXxx> : compare le WS et un fichier et cr‚e des listes
&EN <datacalclst> :   effectue des op‚rations logiques sur des listes
&EN <datacalcvar> :   calcule une variable sur base d'une forme LEC
&EN <datadisplaygraph> : affiche un graphique sur base de s‚ries (sans tableau)
&EN <datasavegraph> :  sauve un graphique calcul‚ sur base de s‚ries

&EN <datawidthvar> : fixe la largeur des colonnes d'‚dition des s‚ries
&EN <datandecvar>  : fixe le nombre de d‚cimales pour l'‚dition des s‚ries
&EN <datamodevar>  : fixe le mode pour l'‚dition des s‚ries
&EN <datastartvar> : fixe la premiŠre p‚riode pour l'‚dition des s‚ries
&EN <datawidthtbl> : fixe la largeur des colonnes d'‚dition des tableaux
&EN <datawidthscl> : fixe la largeur des colonnes d'‚dition des scalaires
&EN <datandecscl>  : fixe le nombre de d‚cimales pour l'‚dition des scalaires

&EN <dataeditcnf>   : change les options d'‚dition des variables
&EN <datarasvar>   : m‚thode RAS de compl‚tion d'une matrice de s‚ries
&EN <datapatternXXX> : cr‚ation de listes de noms … partir de pattern

&TX
>


<Configuration de l'imprimante>
    Configuration de l'imprimante
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les commandes suivantes sont utilis‚es pour configurer l'imprimante ou
le fichier de r‚ception des impressions:

&EN <printdest> : fixe la destination de l'impression
&EN <printdestnew> : printdest et r‚initialisation du fichier d'impression
&EN <printnbdec> : fixe le nombre de d‚cimales pour l'impression
&EN <printlang> : fixe la langue par d‚faut pour l'impression

&EN <PrintA2mAppend> : permet de ne pas vider le fichier a2m
		      avant l'impression
&EN <PrintFont> : fixe la police de caractŠre pour l'impression
&EN <PrintTableFont> : fixe la police de caractŠres pour les tableaux
&EN <PrintTableBox> : fixe la largeur du cadre des tableaux
&EN <PrintTableColor> : permet d'utiliser ou non la couleur dans les tableaux
&EN <PrintTableWidth> : fixe la largeur des tableaux en Frame et Rtf
&EN <PrintTableBreak> : permet ou non les coupures des tableaux sur
			plusieurs pages
&EN <PrintTablePage> : force un saut de page avant chaque tableau

&EN <PrintBackground> : fixe la couleur du background (tableaux et graphiques)
&EN <PrintGraphBox> : fixe la largeur du cadre de graphiques
&EN <PrintGraphBrush> : fixe la densit‚ du background des graphiques
&EN <PrintGraphSize> : fixe la taille des graphiques
&EN <PrintGraphPage> : force un saut de page avant chaque graphique

&EN <PrintRtfHelp> : g‚nŠre un fichier RTF pour un help Windows
&EN <PrintRtfTopic> : cr‚e un nouveau topic (Help Windows)
&EN <PrintRtfLevel> : change le niveau hi‚rarchique des topics suivants
&EN <PrintRtfTitle> : fixe le titre de l'aide Windows
&EN <PrintRtfCopyright> : fixe le texte du copyright de l'aide Windows

&EN <PrintHtmlHelp> : g‚nŠre un fichier HTML pour un help HtmlHelp
&EN <PrintHtmlStrip> : Lors de la g‚n‚ration de fichier HTML (A2mToHtml), ne g‚nŠre
			pas d'entˆte ni de footer

&EN <PrintParanum> : permet de num‚roter les titres
&EN <PrintPageHeader> : fixe le titre des pages imprim‚es
&EN <PrintPageFooter> : fixe la footnote des pages imprim‚es
&EN <SetPrinter> : fixe l'imprimante par d‚faut
&EN <PrintOrientation> : fixe l'orientation du papier dans l'imprimante
&EN <PrintDuplex> : fixe le mode recto-verso de l'imprimante
&EN <PrintGIFBackColor> : d‚finit la couleur de fond des graphiques
&EN <PrintGIFTransColor>  : d‚finit la couleur consid‚r‚e comme
				      "transparente"
&EN <PrintGIFTransparent> : indique si le fichier GIF doit ˆtre au
				      format transparent
&EN <PrintGIFInterlaced> : indique si le fichier GIF doit ˆtre au
				      format interlac‚
&EN <PrintGIFFilled> : indique s'il faut remplir les barres dans
				      les bar charts
&EN <PrintGIFFont> : indique si le num‚ro du font … utiliser
>

<Impressions d'objets>
    Impressions d'objets
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les commandes suivantes sont utilis‚es pour imprimer les d‚finitions
d'objets :

&EN <printobjdef>  : imprime une d‚finition d'objet cmt | eqs |
		    idt | lst | scl | tbl | var
&EN <printobjtitle> : indique si les titres seuls sont … imprimer
&EN <printobjlec>  : indique le type d'‚quation … imprimer
&EN <printobjinfos> : indique les informations … imprimer

>

<printobjtitle>
    PrintObjTitle
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction s'utilise avant $PrintObjDefTbl. Elle indique si les titres
des tableaux sont seuls … ˆtre imprim‚s ou si toute la d‚finition
doit l'ˆtre.

&FS
Les mˆmes informations sont introduites dans l'‚cran "Print Objects
definition".

&NFS
&SY2
    $PrintObjTitle 0 ou 1
    0 : d‚finition complŠte
    1 : titres seuls
&TX
>
<printobjlec>
    PrintObjLec
    ÄÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction s'utilise avant $PrintObjDefXxx. Elle indique la fa‡on
dont des ‚quations doivent ˆtre imprim‚es: telles quelles ou avec
remplacement des coefficients par leurs valeurs ou encore avec
les t-tests.

&FS
Les mˆmes informations sont introduites dans l'‚cran "Print Objects
definition".

&NFS
&SY2
    $PrintObjLec 0, 1 ou 2
    0 : forme LEC normale
    1 : remplacement des coefs par leur valeur
    2 : remplacement des coefs par leur valeur+t-tests
&TX
>

<printobjinfos>
    PrintObjInfos
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction s'utilise avant $PrintObjDefEqs. Elle indique les
informations … imprimer :
&EN ‚quations seules
&EN ‚quations, commentaires
&EN ‚quations, commentaires et r‚sultats d'estimation

&FS
Les mˆmes informations sont introduites dans l'‚cran "Print Objects
definition".

&NFS
&SY2
    $PrintObjInfos 0, 1 ou 2
    0 : ‚quation seule
    1 : ‚quation + commentaire
    2 : ‚quation + commentaire + estimation
&TX
>

<Compilation et impression de tables >
    Compilation et impression de tables
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les commandes suivantes sont utilis‚es pour compiler des tableaux et les
imprimer ou les visualiser :

&EN <PrintTblFile>
&EN <PrintTbl>
&EN <ViewTblFile>
&EN <ViewTbl>
&EN <ViewByTbl>
&EN <PrintVar>
&EN <ViewVar>
&EN <ViewWidth>
&EN <ViewWidth0>
&EN <ViewNdec>
>
<DataPrintGraph>
    DataPrintGraph
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
    Cette fonction imprime des graphiques construits directement …
    partir de variables (sans passer par des tableaux). Elle correspond
    … l'utilisation de la touche F8 lors de l'‚dition d'un WS de
    variables.

&SY2
    $DataPrintGraph   {Level | Diff | Grt}
		      {Line | Scatter | Bar | Mixt}
		      {NoXGrids | MinorXGrids | J(MajorXGrids)}
		      {NoYGrids | MinorYGrids | J(MajorYGrids)}
		      {Level | G(Log) | Semi-Log | Percent}
		      {ymin | --} {ymax | --}
		      perfrom perto varname1 varname2 ...
&TX
&EX2
    $DataPrintGraph  Level Line No No Level -- -- 1980Y1 1995Y1 X1 Y1 X1+Y1
&TX

>
<PrintGr>
    PrintGr
    ÄÄÄÄÄÄÄ
    Cette fonction imprime un ou des graphiques d‚finis … partir de tableaux.
&SY2
    $PrintGr gsample table_names
&TX

&EX2
    $PrintGr 80/79:15 T1 T2
&TX
>

<ViewWidth>
    ViewWidth
    ÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction permet de sp‚cifier la largeur des colonnes lors de
l'affichage de tableaux (calcul‚s). La valeur doit ˆtre comprise entre
2 et 12.

&FS
Cette fonction n'est pas disponible en mode plein ‚cran. Les touches
F3 et s-F3 permettent de changer la largeur des colonnes lors de
l'affichage.

&NFS
&SY2
    $ViewWidth n
    n entre 2 et 12
&TX
>
<ViewWidth0>
    ViewWidth0
    ÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction permet de sp‚cifier la largeur de la premiŠre colonne
(colonne de titres) lors de l'affichage de tableaux (calcul‚s). La
valeur doit ˆtre comprise entre 2 et 60.

&FS
Cette fonction n'est pas disponible en mode plein ‚cran. Les touches
F2 et s-F2 permettent de changer la largeur de la premiŠre colonne lors
de l'affichage.

&NFS
&SY2
    $ViewWidth0 n
    n entre 2 et 60
&TX
>
<ViewNdec>
    ViewNdec
    ÄÄÄÄÄÄÄÄ
&DE
Cette fonction permet de sp‚cifier le nombre de d‚cimale des valeurs
affich‚es dans les tableaux (calcul‚s). La valeur doit
ˆtre comprise entre -1 (nombre variable) et 6.

&FS
Cette fonction n'est pas disponible en mode plein ‚cran. Les touches
F4 et s-F4 permettent de changer le nombre de d‚cimales lors de
l'affichage.

&NFS
&SY2
    $ViewNdec n
    n entre -1 (variable) et 6
&TX
>

<Graphiques … partir de tableaux     >
    Graphiques … partir de tableaux
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Les commandes suivantes sont utilis‚es pour compiler des tableaux et les
visualiser sous forme de graphiques ou les sauver dans un fichier
A2M.

&EN    <ViewGr>
&EN    <PrintGrAll> (obsolete)
&EN    <PrintGrWin> (obsolete)
&EN    <PrintGrData> (obsolete)
&EN    <PrintGr>
&EN    <DataPrintGraph>
>

<Op‚rations sur des modŠles>
    Op‚rations sur des modŠles
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les commandes suivantes permettent de trier et de simuler un modŠle, d'estimer et de
recompiler des ‚quations :

&EN    <ModelCalcSCC>
&EN    <ModelSimulateParms>
&EN    <ModelSimulate>
&EN    <ModelSimulateSCC>
&EN    <ModelExchange>
&EN    <ModelCompile>
&EN    <ModelSimulateSaveNiters>
&EN    <ModelSimulateSaveNorms>
>
<Op‚rations sp‚cifiques aux ‚quations>
    Op‚rations sp‚cifiques aux ‚quations
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les commandes suivantes permettent de modifier les paramŠtres de
l'estimation et d'estimer des ‚quations :

&EN    <EqsEstimate>
&EN    <EqsStepWise>
&EN    <EqsSetCmt>
&EN    <EqsSetSample>
&EN    <EqsSetMethod>
&EN    <EqsSetInstrs>
&EN    <EqsSetBloc>
>
<Ex‚cutions d'identit‚s>
    Ex‚cutions d'identit‚s
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les commandes suivantes permettent d'ex‚cuter des identit‚s :

&EN    <idtexecute>
&EN    <idtexecutetrace>
&EN    <idtexecutevarfiles>
&EN    <idtexecutesclfiles>
>

<Op‚rations sur des rapports>
    Op‚rations sur des rapports
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les commandes suivantes permettent de manipuler des rapports (ces
fonctions sont r‚cursives):

&EN    <reportexec>
&EN    <reportedit>
>

<filelist     >
    FileList
    ÄÄÄÄÄÄÄÄ

Fonction annul‚e … partir de la version 5 de IODE.
>
/*
<fileprint    >
    FilePrint
    ÄÄÄÄÄÄÄÄÄ

&DE
Cette commande permet d'imprimer un fichier.

Le fichier doit ˆtre un fichier texte.

La fonction imprime le fichier sans interpr‚tation sur l'imprimante
d‚finie (voir Print Destination). Le fichier est simplement ajout‚ au
fichier-imprimante s‚lectionn‚.

&FS
La fonction affiche une page de saisie dans laquelle on d‚finit le ou
les paramŠtres de la commande.

Le nom du fichier est indiqu‚ dans le champ de saisie de la page. Le
champ de saisie est du type DIR. La touche F10 valide l'op‚ration lorsque
la page est compl‚t‚e.

&MU

&SY2
    ##FilePrint
&TX

&NFS
Le nom du fichier (ou un masque) est pass‚ en paramŠtre de la commande.

&SY2
    $FilePrint filename
&TX

&EX2
    $FileDeleteA2m  imp
    $PrintDest      imp.a2m
    $FilePrint      bist92\read.me
    $FileEdit       imp.a2m
    $show           End of report...
&TX
>
*/
<fileedit     >
    FileEdit
    ÄÄÄÄÄÄÄÄ

Fonction annul‚e … partir de la version 5 de IODE.
>

<filedelete>
    FileDeleteXxx
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ
&TPART
&EXTF
&DE
La fonction efface un fichier sur disque portant une des extensions
suivantes :

#include "extl.m1"

&FS
La fonction affiche une page de saisie dans laquelle on d‚finit le ou
les paramŠtres de la commande.

La page de saisie contient deux champs: le premier est un champ MENU
(voir lexique): on y s‚lectionne le type de fichier … effacer (le type
choisi est propos‚ par d‚faut); le second est un champ DIR (voir lexique):
on y sp‚cifie le nom du fichier … effacer.

Le fichier est effac‚ lorque la page est valid‚e avec F10.

&MU
&SY2
    ##FileDeleteCmt
&TX
&NFS
On passe en paramŠtre la liste des fichiers portant l'extension choisie …
effacer.

&SY2
    $FileDeleteCmt fichier [liste de fichiers]
&TX

&EX2
    $FileDeleteA2m  imp
    $PrintDest      imp.a2m

    $PrintObjDefEqs
    $FileEdit       imp.a2m
    $show End of report...
&TX
>


<filerename>
    FileRenameXxx
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ
&TPART
&EXTF
&DE
La fonction renomme un fichier sur disque portant une des extensions :

#include "extl.m1"

&FS
La fonction affiche une page de saisie dans laquelle on d‚finit le ou
les paramŠtres de la commande.

Le type du fichier est sp‚cifi‚ dans le champ sup‚rieur de la page (champ
MENU) le nom du fichier source et le nom du fichier destination sont
sp‚cifi‚s dans les deux autres champs.

La touche F10 valide l'op‚ration lorsque la page est compl‚t‚e.

&MU
&SY2
    ##FileRenameCmt
&TX

&NFS
On passe en paramŠtre le nom du fichier source (ancien nom) et le nom du
fichier destination (nouveau nom).

&SY2
    $FileRenameCmt fichier_source fichier_dest
&TX

&EX2
    $FileRenameCmt  maribel old
&TX
>


<filecopy>
    FileCopyXxx
    ÄÄÄÄÄÄÄÄÄÄÄ
&TPART
&EXTF
&DE
La fonction copie un fichier portant une des extensions suivantes :

#include "extl.m1"

&FS
La fonction affiche une page de saisie dans laquelle on d‚finit le ou
les paramŠtres de la commande.

Le type du fichier est sp‚cifi‚ dans le champ sup‚rieur de la page (champ
MENU) le nom du fichier source et le nom du fichier destination sont
sp‚cifi‚s dans les deux autres champs.

La touche F10 valide l'op‚ration lorsque la page est compl‚t‚e.

&MU
&SY2
    ##FileCopyCmt
&TX

&NFS
On passe en paramŠtre le nom du fichier source (ancien nom) et le nom du
fichier destination (nouveau nom).

&SY2
    $FileCopyCmt fichier_source fichier_dest
&TX

&EX2
    $FileCopyCmt  matibel old
&TX
>

<WsImport>
    WsImportXxx
    ÄÄÄÄÄÄÄÄÄÄÄ

&TPART
&EXTWS
&DE
Cette op‚ration permet d'effectuer un WsLoad … partir d'un fichier ASCII.
Rappelons que l'op‚ration de LOAD d'un workspace remplace le contenu du
WS courant par le contenu d'un fichier disque : tous les objets
actuellement d‚finis sont d‚truits et remplac‚s par ceux d‚finis dans le
fichier … charger.

&FS
Cette fonction ouvre l'‚cran WsLoad dans lequel on peut entrer un nom de
fichier par type d'objet. Si le nom d'un des fichiers a une autre extension
que celle des WS de IODE (.av au lieu de .var par exemple),
l'importation remplace le LOAD.

&SY2
    ##WsImportCmt
    ##WsImportEqs
    ##WsImportIdt
    ##WsImportLst
    ##WsImportScl
    ##WsImportTbl
    ##WsImportVar
&TX

&MU

&NFS
On passe en paramŠtre … la commande le nom du fichier … charger.

&SY2
    $WsImportCmt fichier
    $WsImportEqs fichier
    $WsImportIdt fichier
    $WsImportLst fichier
    $WsImportScl fichier
    $WsImportTbl fichier
    $WsImportVar fichier
&TX

&EX2
    $WsImportCmt myfile.ac
&TX

Si l'on charge un nouveau fichier de variables, la p‚riode
d'‚chantillonnage du WS est remplac‚e par celle du fichier charg‚.
>

<WsExport>
    WsExportXxx
    ÄÄÄÄÄÄÄÄÄ
&TPART
&EXTWS
&DE
Cette fonction r‚alise l'op‚ration inverse de la fonction WsImportXxx : elle
copie sur disque le WS dans son ‚tat actuel en format ASCII.

&FS
Elle est identique … celle de la fonction ##WsSave. Il suffit de placer comme
extension aux fichiers une extension diff‚rente que celle des WS de IODE.

&MU

&SY2
    ##WsExportCmt
    ##WsExportEqs
    ##WsExportIdt
    ##WsExportLst
    ##WsExportScl
    ##WsExportTbl
    ##WsExportVar
&TX

&NFS
On passe en paramŠtre … la commande le nom du fichier … sauver.

&SY2
    $WsExportXxx fichier
    {Xxx = cmt | eqs | idt | lst | scl | tbl | var}
&TX

&EX2
    $WsExportCmt mytest.ac
&TX
>

<wsload>
    WsLoadXxx
    ÄÄÄÄÄÄÄÄÄ

&TPART
&EXTWS
&DE
L'op‚ration de chargement (LOAD) d'un workspace remplace le contenu du
WS courant par le contenu d'un fichier disque : tous les objets
actuellement d‚finis sont d‚truits et remplac‚s par ceux d‚finis dans le
fichier … charger.

&FS
Cette fonction ouvre un ‚cran dans lequel on peut entrer un nom de
fichier par type d'objet. Au d‚part, les noms sont ceux des WS courants.

&SY2
    ##WsLoadCmt
    ##WsLoadEqs
    ##WsLoadIdt
    ##WsLoadLst
    ##WsLoadScl
    ##WsLoadTbl
    ##WsLoadVar
&TX

&MU

&NFS
On passe en paramŠtre … la commande le nom du fichier … charger.

&SY2
    $WsLoadCmt fichier
    $WsLoadEqs fichier
    $WsLoadIdt fichier
    $WsLoadLst fichier
    $WsLoadScl fichier
    $WsLoadTbl fichier
    $WsLoadVar fichier
&TX

&EX2
    $WsLoadCmt maribel
&TX

Si l'on charge un nouveau fichier de variables, la p‚riode
d'‚chantillonnage du WS est remplac‚e par celle du fichier charg‚.
>

<wscopy       >
    WsCopyXxx
    ÄÄÄÄÄÄÄÄÄ

&TPART
&EXTWS
&DE
La fonction permet d'ajouter ou de remplacer des objets … partir de WS
sauv‚s sur disque.

Les objets s‚lectionn‚s sont copi‚s dans le WS courant et remplacent les
valeurs actuelles. Le processus s'arrˆte dŠs qu'un objet ne peut ˆtre
trouv‚.

Le WS courant garde intacts tous les autres objets qui en font partie.

&FS
La fonction affiche une page de saisie dans laquelle on d‚finit le ou
les paramŠtres de la commande.

Les paramŠtres sont le type d'objet … traiter, le nom du ou des fichiers
contenant les WS o— se trouvent les objets … copier, s‚par‚s par une
virgule ou point-virgule (sans blancs) et enfin la liste des noms
d'objets.

S'il s'agit de fichiers de variables, la p‚riode … copier peut
‚galement ˆtre choisie.

&MU

&SY2
    ##WsCopyCmt
    ##WsCopyEqs
    ##WsCopyIdt
    ##WsCopyLst
    ##WsCopyScl
    ##WsCopyTbl
    ##WsCopyVar
&TX

&NFS
On passe en paramŠtre … la commande le nom du fichier WS sur disque et
la liste des objets … copier.

Si on copie des variables (VAR) d'un autre fichier, le sample intervient
de la fa‡on suivante:

&EN si aucun sample n'est d‚fini: celui du fichier … copier est utilis‚

&EN si un ws de variables est en cours d'utilisation, seules les donn‚es
    du nouveau fichier qui se situent dans la p‚riode d'‚chantillonnage
    courante sont copi‚es

&EN on peut ‚galement d‚finir la p‚riode d'‚chantillonnage dans
    laquelle seront copi‚es les donn‚es du fichier.

Si des s‚ries n'existent pas dans le ws courant, elles sont cr‚‚es.

&SY2
    $WsCopyCmt fichier;fichier;.. {objet | liste d'objets}
		       {cmt | eqs | idt | lst | scl | tbl}
&TX

&EX2
    $WsCopyCmt maribel.cmt CMT1 $CMT
&TX


&SY2
    $WsCopyVar file;file;.. [from to] {objet |liste d'objets }
    (from, to := periode)
&TX

&EX2
    $WsCopyVar maribel 1990Y1 2000Y1 A
&TX

copie la variable A du fichier maribel dans le WS pour la p‚riode 1990 …
2000 seulement. Si A n'existe pas, elles est cr‚‚e et seules les
valeurs pour la p‚riode seront fix‚es.

&EX2
    $WsCopyVar maribel;hermes  A B C
&TX
idem, mais sur tout le sample du fichier maribel.
>

<wssave   >
    WsSaveXxx
    ÄÄÄÄÄÄÄÄÄ
&TPART
&EXTWS
&DE
Cette fonction r‚alise l'op‚ration inverse de la fonction LOAD : elle
copie sur disque le WS dans son ‚tat actuel, en rempla‡ant ‚ventuellement
les fichiers existants.

&FS
La fonction affiche une page de saisie dans laquelle on d‚finit le ou
les paramŠtres de la commande.

La fenˆtre pr‚sente pour chaque type de WS

&EN le nom du fichier associ‚ au WS. Par d‚faut, ce nom est ws.type o—
    type indique le type d'objet. Ce nom peut ˆtre modifi‚ par la
    fonction "Name Workspace". Si le WS a ‚t‚ charg‚ … partir du disque,
    le nom du WS est le nom du fichier d'origine.

&EN le nombre d'objets actuellement d‚finis dans le WS

Le nom du fichier peut ˆtre modifi‚ avant le sauvetage. Un nom vide ne
donnera pas lieu … un sauvetage. Les WS courants ne sont pas modifi‚s par
cette fonction.

&MU

&SY2
    ##WsSaveCmt
    ##WsSaveEqs
    ##WsSaveIdt
    ##WsSaveLst
    ##WsSaveScl
    ##WsSaveTbl
    ##WsSaveVar
&TX

&NFS
On passe en paramŠtre … la commande le nom du fichier … sauver.

&SY2
    $WsSaveXxx fichier
    {Xxx = cmt | eqs | idt | lst | scl | tbl | var}
&TX

&EX2
    $wssavecmt  current2
&TX
>

<WsSaveCmp>
    WsSaveCmp
    ÄÄÄÄÄÄÄÄÄ

Les diff‚rents WS peuvent ˆtre comprim‚s au moment de leur sauvetage. Selon
le type d'objet et leur d‚finition, la compression peut aller de 30 … 75%.

Chaque objet ‚tant comprim‚ individuellement, le co–t au niveau du temps de
sauvetage peut ˆtre important pour les fichiers contenant de nombreux objets
ou sur des machines lentes (1000 variables de 25 obs. … la sec. sur un
Pentium III 450 MHz).

Iode reconnaŒt et transforme automatiquement les fichiers comprim‚s.

Le chargement n'est pratiquement pas p‚nalis‚ par la proc‚dure de
d‚compression. Un seul cas fait exception : les fichiers de variables dont
les s‚ries doivent ˆtre charg‚es par une fonction de type $WsCopyVar: dans
ce cas, la lecture est ralentie par le fait que les longueurs des s‚ries
comprim‚es deviennent variables et que le fichier doit par cons‚quent ˆtre
lu s‚quentiellement.

Le panneau de sauvetage pr‚sente un checkbox qui permet d'indiquer si on
souhaite ou non comprimer les fichiers.

Cette fonction est identique … <WsSave> … ceci prŠs qu'elle comprime les
fichiers en sauvant les donn‚es.

&SY2
    $WsSaveCmpXxx fichier
    {Xxx = cmt | eqs | idt | lst | scl | tbl | var}
&TX
>

<wsmerge      >
    WsMergeXxx
    ÄÄÄÄÄÄÄÄÄÄ
&TPART
&EXTWS
&DE
Cette fonction r‚alise l'op‚ration de fusion de plusieurs fichiers avec
le WS courant : elle ajoute dans le WS actif les objets d‚finis dans un
fichier.

&FS
Cette fonction ouvre un ‚cran dans lequel on peut entrer le type d'objet
et le nom du fichier … fusionner.

&SY2
    ##WsMergeCmt
    ##WsMergeEqs
    ##WsMergeIdt
    ##WsMergeLst
    ##WsMergeScl
    ##WsMergeTbl
    ##WsMergeVar
&TX

&MU
&NFS
On passe en paramŠtre … la commande le nom du fichier.

&SY2
    $WsMergeXxx fichier
    {Xxx = cmt | eqs | idt | lst | scl | tbl | var}
&TX

&EX2
    $WsMergeCmt mycmts
&TX
>

<wsdescr   >
    WsDescrXxx
    ÄÄÄÄÄÄÄÄÄÄ
&TPART
&EXTWS
&DE
Cette fonction cr‚e ou remplace la description du fichier. Cette
description sera sauv‚e ult‚rieurement si l'on sauve le fichier.

&FS
La fonction affiche une page de saisie dans laquelle on d‚finit le ou
les paramŠtres de la commande.

La fenˆtre pr‚sente pour chaque type de WS le commentaire associ‚ au WS.
Si le WS a ‚t‚ charg‚ … partir du disque, le commentaire est celui
provenant du fichier d'origine.

&MU

&SY2
    ##WsDescrCmt
    ##WsDescrEqs
    ##WsDescrIdt
    ##WsDescrLst
    ##WsDescrScl
    ##WsDescrTbl
    ##WsDescrVar
&TX

&NFS
On passe en paramŠtre … la description du fichier.

&SY2
    $WsDescrXxx texte
    {Xxx = cmt | eqs | idt | lst | scl | tbl | var}
&TX

&EX2
    $WsDescrCmt  Le fichier de commentaires de MIRABEL
&TX
>

<wsclear      >
    WsClearXxx
    ÄÄÄÄÄÄÄÄÄÄ
&TPART
&EXTWS
&DE
Cette fonction d‚truit tous les objets du type sp‚cifi‚.

&FS
La fonction affiche une page de saisie dans laquelle on d‚finit le ou
les paramŠtres de la commande.

La fenˆtre pr‚sente pour chaque type de WS le nombre d'objets
actuellement d‚finis dans le WS.

Presser sur le bouton correspondant au type souhait‚ ou sur le bouton
"Clear All".

&SY2
    ##WsClearCmt
    ##WsClearEqs
    ##WsClearIdt
    ##WsClearLst
    ##WsClearScl
    ##WsClearTbl
    ##WsClearVar
&TX

&MU

&NFS
On ne passe pas de paramŠtre.

&SY2
    $WsClearXxx
    {Xxx = cmt | eqs | idt | lst | scl | tbl | var}
&TX

&EX2
    $WsClearEqs
&TX
>

<wssample>
    WsSample
    ÄÄÄÄÄÄÄÄ
&DE
Cette fonction permet de modifier le sample du WS.

Si le sample est plus court que le sample courant, les donn‚es situ‚es au
del… sont d‚truites.

Si au contraire le sample est plus long que le sample courant, la valeur
NA (--) est donn‚e aux p‚riodes ajout‚es.

&FS
&MU

&SY2
    ##WsSample
&TX

&NFS
&SY2
    $WsSample from to
    (from, to := IODE periodes)
&TX

&EX2
    $WsSample 1990Y1 2000Y1
&TX
>

<wsextrapolate>
    WsExtrapolate
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction permet de compl‚ter des s‚ries selon une m‚thode au choix
bas‚e sur les p‚riodes pr‚c‚dentes.

Les m‚thodes possibles sont les suivantes :

&EN 0 = Y := Y[-1], if Y null or NA
&EN 1 = Y := Y[-1], always
&EN 2 = Y := extrapolation, if Y null or NA
&EN 3 = Y := extrapolation, always
&EN 4 = Y := unchanged, always
&EN 5 = Y := Y[-1], if Y is NA
&EN 6 = Y := extrapolation, if Y is NA

Il faut fournir la p‚riode (inclue dans celle d‚finie dans le WS
courant) sur laquelle le calcul doit ˆtre effectu‚.

De plus, la liste des variables … adapter peut ‚galement ˆtre pr‚cis‚e.
Si cette liste est laiss‚e vide, toutes les s‚ries du WS sont modifi‚es.

&FS
&SY2
    ##WsExtrapolate
&TX

&MU

&NFS
&SY2
    $WsExtrapolate [method] from to [liste de variables]
    o— method : 0 ... 6
       from, to := p‚riodes de IODE (yyyyPpp)
&TX

&EX2
    $WsExtrapolate 1993Y1 2000Y1 A B C
ou
    $WsExtrapolate 1993Y1 2000Y1
&TX
>

<wsltohstock>
    WsLtoHStock
    ÄÄÄÄÄÄÄÄÄÄÄ

&DE
Construit des s‚ries de p‚riodicit‚ sup‚rieure pour des donn‚es de type
stock (Ch“mage, Dette, ...).

Pour ce faire, la fonction charge en WS la liste de s‚ries du fichier
sp‚cifi‚ et modifie simultan‚ment la p‚riodicit‚ de ces s‚ries. La
nouvelle p‚riodicit‚ est celle actuellement d‚finie dans le WS actif.

Les nouvelles s‚ries sont ajout‚es ou remplacent (pour des noms
existants) celles du WS actif.

Cette proc‚dure existe pour les cas suivants :

&EN annuel vers mensuel
&EN annuel vers trimestriel
&EN trimestriel vers mensuel

Deux m‚thodes sont disponibles, l'une pour les stock, l'autre pour les
flux (WsLtoHFlow).

Dans le cas des stock, La m‚thode d'interpolation est au choix :

&EN lin‚aire : A[1980Q{1,2,3,4}] = A[1979Y1] + i * (A[1980Y1] - A[1979Y1])/4
		i = 1,2,3,4
&EN cubic splines = interpolation cubique
&EN step : A[1980Q{1,2,3,4}] = A[1980Y1]


&FS
&SY2
    ##WsLtoH
&TX

&NFS
&SY2
    $WsLtoHStock {L|C|S} Filename VarList

    avec L pour interpolation lin‚aire
	 C pour interpolation par Cubic Splines
	 S pour interpolation par Steps
&TX

&EX2
    $WsLtoHStock C Fichier1 A B C
&TX
>

<wsltohflow>
    WsLtoHFlow
    ÄÄÄÄÄÄÄÄÄÄ

&DE
Construit des s‚ries de p‚riodicit‚ sup‚rieure pour des donn‚es de type
flux (PNB, D‚ficit, ...).

Pour ce faire, la fonction charge en WS la liste de s‚ries du fichier
sp‚cifi‚ et modifie simultan‚ment la p‚riodicit‚ de ces s‚ries. La
nouvelle p‚riodicit‚ est celle actuellement d‚finie dans le WS actif.

Les nouvelles s‚ries sont ajout‚es ou remplacent (pour des noms
existants) celles du WS actif.

Cette proc‚dure existe pour les cas suivants :

&EN annuel vers mensuel
&EN annuel vers trimestriel
&EN trimestriel vers mensuel

Deux m‚thodes sont disponibles, l'une pour les stock, l'autre pour les
flux.

Dans le cas des flux, la s‚rie est r‚partie sur les sous-p‚riodes :

&EN interpolation lin‚aire   : A[1980Q1] = A[1980Y1] / n
				(ou n = nbre de sous-p‚riodes)
&EN interpolation par splines : interpolation cubique
&EN interpolation par steps   : lin‚aire

&FS
&SY2
    ##WsLtoH
&TX

&NFS
&SY2
    $WsLtoHFlow {L|C|S}  Filename VarList

    avec L pour interpolation lin‚aire
	 C pour interpolation par Cubic Splines
	 S pour interpolation par Steps
&TX

&EX2
    $WsLtoHFlow S Fichier1 A B C
&TX
>

<wshtolsum>
    WsHtoLSum
    ÄÄÄÄÄÄÄÄÄ

&DE
Construit des s‚ries de p‚riodicit‚ inf‚rieure en additionnant les
sous-p‚riodes.

Pour ce faire, la fonction charge en WS la liste de s‚ries du fichier
sp‚cifi‚ et modifie simultan‚ment la p‚riodicit‚ de ces s‚ries. La
nouvelle p‚riodicit‚ est celle actuellement d‚finie dans le WS actif.

Les nouvelles s‚ries sont ajout‚es ou remplacent (pour des noms
existants) celles du WS actif.

Cette proc‚dure existe pour les cas suivants :

&EN mensuel vers annuel (observation annuelle = somme des 12 mois)

&EN trimestriel vers annuel (observation annuelle = somme des 4
    trimestres)

&EN mensuel vers trimestriel (observation trimestrielle = somme des 3
    mois)

Trois m‚thodes sont disponibles :

&EN Addition de sous-p‚riodes (flux) : WsHtoLSum
&EN Moyenne de sous-p‚riodes (stock) : WsHtoLMean
&EN DerniŠre observation (stock) : WsHtoLLast

En cas de valeur inexistante (NA) pour l'une des sous-p‚riodes, le
r‚sultat est NA.

&FS
&SY2
    ##WsHtoL
&TX

&NFS
&SY2
    $WsHtoLSum Filename VarList
&TX

&EX2
    $WsHtoLSum Fichier1 A B C
&TX
>

<wshtolmean>
    WsHtoLMean
    ÄÄÄÄÄÄÄÄÄ

&DE
Construit des s‚ries de p‚riodicit‚ inf‚rieure en prenant la moyenne des
sous-p‚riodes.

Pour ce faire, la fonction charge en WS la liste de s‚ries du fichier
sp‚cifi‚ et modifie simultan‚ment la p‚riodicit‚ de ces s‚ries. La
nouvelle p‚riodicit‚ est celle actuellement d‚finie dans le WS actif.

Les nouvelles s‚ries sont ajout‚es ou remplacent (pour des noms
existants) celles du WS actif.

Cette proc‚dure existe pour les cas suivants :

&EN mensuel vers annuel (observation annuelle = moyenne des 12 mois)

&EN trimestriel vers annuel (observation annuelle = moyenne des 4
    trimestres)

&EN mensuel vers trimestriel (observation trimestrielle = moyenne des 3
    mois)

Trois m‚thodes sont disponibles :

&EN Addition de sous-p‚riodes (flux) : WsHtoLSum
&EN Moyenne de sous-p‚riodes (stock) : WsHtoLMean
&EN DerniŠre observation (stock) : WsHtoLLast

En cas de valeur inexistante (NA) pour l'une des sous-p‚riodes, le
r‚sultat est NA.

&FS
&SY2
    ##WsHtoL
&TX

&NFS
&SY2
    $WsHtoLMean Filename VarList
&TX

&EX2
    $WsHtoLMean Fichier1 A B C
&TX
>
<wshtollast>
    WsHtoLLast
    ÄÄÄÄÄÄÄÄÄ

&DE
Construit des s‚ries de p‚riodicit‚ inf‚rieure en prenant la derniŠre
observation des sous-p‚riodes.

Pour ce faire, la fonction charge en WS la liste de s‚ries du fichier
sp‚cifi‚ et modifie simultan‚ment la p‚riodicit‚ de ces s‚ries. La
nouvelle p‚riodicit‚ est celle actuellement d‚finie dans le WS actif.

Les nouvelles s‚ries sont ajout‚es ou remplacent (pour des noms
existants) celles du WS actif.

Cette proc‚dure existe pour les cas suivants :

&EN mensuel vers annuel (observation annuelle = celle de d‚cembre)

&EN trimestriel vers annuel (observation annuelle = celle du dernier
    trimestre)

&EN mensuel vers trimestriel (observation trimestrielle = celle du
    dernier mois du trimestre)

Trois m‚thodes sont disponibles :

&EN Addition de sous-p‚riodes (flux) : WsHtoLSum
&EN Moyenne de sous-p‚riodes (stock) : WsHtoLMean
&EN DerniŠre observation (stock) : WsHtoLLast

En cas de valeur inexistante (NA) pour l'une des sous-p‚riodes, le
r‚sultat est NA.

&FS
&SY2
    ##WsHtoL
&TX

&NFS
&SY2
    $WsHtoLLast Filename VarList
&TX

&EX2
    $WsHtoLLast Fichier1 A B C
&TX
>

<wsseasonadj>
    WsSeasonAdj
    ÄÄÄÄÄÄÄÄÄÄÄ

&DE
Construit des s‚ries dessaisonnalis‚es … l'aide de la m‚thode Census XI,
ainsi que les composantes de trend cyclique et stochastiques.

Nomenclature :

&EN la s‚rie contenant la composante de trend cyclique se nomme :
&CO
    _Cname o— name est le nom original
&TX
&EN la s‚rie contenant la composante stochastique se nomme :
&CO
    _Iname o— name est le nom original
&TX
&EN la s‚rie dessaisonnalis‚e garde son nom.

Notons que la s‚rie dessaisonnalis‚e est le produit des deux autres.

Pour ce faire, la fonction charge en WS la liste de s‚ries du fichier
sp‚cifi‚ et modifie simultan‚ment les s‚ries s‚lectionn‚es si
n‚cessaire.

Les nouvelles s‚ries sont ajout‚es ou remplacent (pour des noms
existants) celles du WS actif.

En cas de valeur inexistante (NA) pour l'une des p‚riodes, le
r‚sultat est NA pout toute l'ann‚e.

&FS
&SY2
    ##WsSeasonAdj
&TX

&NFS
&SY2
    $WsSeasonAdj Filename VarList
&TX

&EX2
    $WsSeasonAdj Fichier1 A B C
&TX
&SA $WsSeasAdj
>

<wsseasadj>
    WsSeasAdj
    ÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction est identique … $WsSeasonAdj … ceci prŠs qu'elle permet
de fixer le paramŠtre qui permet de lib‚rer le critŠre v‚rifiant si une
influence saisonniŠre est pr‚sente dans une s‚rie.

&FS
&SY2
    ##WsSeasonAdj
&TX
Le paramŠtre peut ˆtre introduit dans le panneau de lancement.

&NFS
&CO
    $WsSeasAdj Filename Varname Varname ... Eps
&TX

&SA $WsSeasonAdj
>

<wstrend>
    WsTrend
    ÄÄÄÄÄÄÄ

&DE
Impl‚mentation de la m‚thode Hodrick-Prescott pour la construction de
s‚rie de trend. Le principe est le mˆme que pour la dessaisonnalisation:
les s‚ries lues dans un fichier sont import‚es et transform‚es
simultan‚ment.

Les s‚ries sont transform‚es en ~clog~C avant le calcul et retransform‚es en
~cexp~C aprŠs le calcul. Elles doivent donc ˆtre strictement positives.

&FS
&SY2
    ##WsTrend
&TX

&NFS
&SY2
    $WsTrend VarFilename Lambda series1 series2 ...
&TX
>

<wstrendstd>
    WsTrendStd
    ÄÄÄÄÄÄÄ

&DE
Impl‚mentation de la m‚thode Hodrick-Prescott pour la construction de
s‚rie de trend. Le principe est le mˆme que pour la dessaisonnalisation:
les s‚ries lues dans un fichier sont import‚es et transform‚es
simultan‚ment.

Contrairement … la fonciton ~c$WsTrend,~C les s‚ries ne sont pas transform‚es avant le calcul.
La contrainte de positivit‚ ne s'applique donc pas.

Un s‚rie lin‚aire (ex. 1..10) le reste aprŠs application du test. Ce n'est pas le cas
pour la fonciton ~c$WsTrend~C.

&FS
&SY2
    ##WsTrendStd
&TX

&NFS
&SY2
    $WsTrendStd VarFilename Lambda series1 series2 ...
&TX



>



<Interface Excel                     >
    Interface Excel
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les fonctions suivantes permettent d'interagir avec Excel … partir
d'un rapport de IODE. Il est ‚galement possible d'utiliser les donn‚es
de IODE dans Excel … travers le serveur DDE de IODE.
Le d‚tail et des exemples sur ce possibilit‚s peuvent ˆtre trouv‚s dans
un chapitre s‚par‚.

&EN <ExcelOpen  >  : lance Excel et ouvre un fichier Excel
&EN <ExcelSave  >  : sauve le fichier courant d'Excel
&EN <ExcelSaveAs>  : sauve le fichier courant d'Excel sous un autre nom
&EN <ExcelClose >  : ferme le fichier courant d'Excel
&EN <ExcelNew   >  : cr‚e un nouveau fichier vide dans Excel
&EN <ExcelPrint >  : imprime le fichier courant d'Excel
&EN <ExcelSetXxx>  : copie des objets de IODE dans Excel
&EN <ExcelWrite >  : ‚crit des informations quelconques dans un sheet Excel
&EN <ExcelGetVar>  : copie dans IODE des variables d‚finies dans Excel
&EN <ExcelGetScl>  : copie dans IODE des scalaires d‚finis dans Excel
&EN <ExcelGetCmt>  : copie dans IODE des commentaires d‚finis dans Excel
&EN <ExcelLang  >  : informe IODE de la langue d'Excel pour formater les ranges
&EN <ExcelDecimal> : informe IODE du s‚parateur d‚cimal utilis‚ par Excel
	pour formater les variables envoy‚es … Excel.
&EN <ExcelCurrency>: informe IODE du symbole de devise utilis‚ par Excel
&EN <ExcelThousand>: informe IODE du s‚parateur de milliers utilis‚ par Excel
>


<Interface DataStream>
    Interface DataStream
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

La fonction suivante permet de charger des donn‚es de Datastream … partir
de la base de donn‚es centrale (Londres). Pour ˆtre op‚rationnelle,
la connexion … Internet est indispensable.

&EN <DSImportDB> : charge des donn‚es … partir du site Datastream
>

<SysAppendFile>
    SysAppendFile
    ÄÄÄÄÄÄÄÄÄÄÄ

Cette fonction permet de copier un fichier … la suite d'une autre.
En cas d'impossibilit‚, la fonction retourne un code d'erreur.

&FS
&NEQ

&NFS
&SY2
    $SysAppendFile filein fileout
&TX
&EX2
    $PrintDest tmp.a2m a2m
    ...
    $PrintDest test a2m               (pour fermer le fichier tmp.a2m)
    $SysAppendFile tmp.a2m result.a2m
    $PrintDest tmp.a2m a2m
    ...
    $PrintDest test a2m               (pour fermer le fichier tmp.a2m)
    $SysAppendFile tmp.a2m result.a2m (accumule les tmp.a2m dans result.a2m)
&TX
>

<SysCopyFile>
    SysCopyFile
    ÄÄÄÄÄÄÄÄÄÄÄ

Cette fonction permet de copier un fichier quelconque sur disque.
Contrairement … la fonction ~cFileCopyXxx,~C l'extension peut ˆtre
quelconque. Attention, le fichier … copier doit ˆtre ferm‚ pour pouvoir
ˆtre copi‚.

En cas d'impossibilit‚, la fonction retourne un code d'erreur.

&FS
&NEQ

&NFS
&SY2
    $SysCopyFile filein fileout
&TX
&EX2
    $PrintDest indprix.a2m a2m
    ...
    $PrintDest test a2m         (pour fermer le fichier indprix.a2m)
    $SysCopyFile indprix.a2m indprix.htm
&TX
>

<SysMoveFile>
    SysMoveFile
    ÄÄÄÄÄÄÄÄÄÄÄ

Cette fonction permet de renommer un fichier quelconque sur disque.
Contrairement … la fonction ~cFileRenameXxx,~C l'extension peut ˆtre
quelconque. Attention, le fichier … renommer doit ˆtre ferm‚. Le fichier
destination ne peut exister.

En cas d'impossibilit‚, la fonction retourne un code d'erreur.

&FS
&NEQ

&NFS
&SY2
    $SysMoveFile filein fileout
&TX
&EX2
    $PrintDest indprix.a2m a2m
    ...
    $PrintDest test a2m         (pour fermer le fichier indprix.a2m)
    $SysMoveFile indprix.a2m indprix.htm
&TX
>

<SysDeleteFile>
    SysDeleteFile
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette fonction permet de d‚truire un fichier quelconque sur disque.
Contrairement … la fonction ~cFileDeleteXxx,~C l'extension peut ˆtre
quelconque.

Si le fichier n'existe pas, la fonction NE retourne PAS de code d'erreur.

&FS
&NEQ

&NFS
&SY2
    $SysDeleteFile file1 file2 ...
&TX
&EX2
    $PrintDest indprix.a2m a2m
    ...
    $PrintDest test a2m         (pour fermer le fichier indprix.a2m)
    $SysDeleteFile indprix.htm
    $SysMoveFile indprix.a2m indprix.htm
&TX
>



<SysOemToAnsi>
SysOemToAnsi
ÄÄÄÄÄÄÄÄÄÄÄÄ

Cette fonction transforme un fichier cod‚ en OEM-850 en Ansi Windows-1252.

&FS
&NEQ

&NFS
&SY2

    $SysOemToAnsi inputfile outputfile
&TX
>

<SysOemToUTF8>
SysOemToUTF8
ÄÄÄÄÄÄÄÄÄÄÄÄ
Cette fonction transforme un fichier cod‚ en OEM-850 en UTF-8.

&FS
&NEQ

&NFS
&SY2

    $SysOemToUTF8 inputfile outputfile
&TX
>

<SysAnsiToOem>
SysAnsiToOem
ÄÄÄÄÄÄÄÄÄÄÄÄ

Cette fonction transforme un fichier cod‚ Ansi Windows-1252 en OEM-850.

&FS
&NEQ

&NFS
&SY2

    $SysAnsiToOem inputfile outputfile
&TX
>

<SysAnsiToUTF8>
SysAnsiToUTF8
ÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette fonction transforme un fichier cod‚ en Ansi Windows-1252 en UTF-8.

&FS
&NEQ

&NFS
&SY2

    $SysAnsiToUTF8 inputfile outputfile
&TX

>

<mkdir>
    mkdir
    ÄÄÄÄÄ

Cette fonction cr‚e un directory s'il n'existe pas.
Si le directory existe, la fonction retourne un code d'erreur.

&FS
&NEQ

&NFS
&SY2
    $mkdir mydir
&TX
&EX2
    $mkdir mydir
    $chdir mydir

    $PrintDest indprix.a2m a2m
    ...
    $chdir ..
    $rmdir mydir
&TX
>

<chdir>
    chdir
    ÄÄÄÄÄ

Cette fonction change le directory courant.
Si le directory n'existe pas, la fonction retourne un code d'erreur.

&FS
&NEQ

&NFS
&SY2
    $chdir mydir
&TX
&EX2
    $mkdir mydir
    $chdir mydir

    $PrintDest indprix.a2m a2m
    ...
    $chdir ..
    $rmdir mydir
&TX
>

<rmdir>
    rmdir
    ÄÄÄÄÄ

Cette fonction d‚truit un directory. Celui-ci doit ˆtre vide (ne plus
contenir de fichier ou de sous-directory). Si le directory n'existe pas ou
s'il n'est pas vide, la fonction retourne un code d'erreur.

&FS
&NEQ

&NFS
&SY2
    $chdir mydir
&TX
&EX2
    $mkdir mydir
    $chdir mydir

    $PrintDest indprix.a2m a2m
    ...
    $chdir ..
    $rmdir mydir
&TX
>

<PrintGIFBackColor>
    PrintGIFBackColor
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
    D‚finit la couleur de fond des graphiques GIF g‚n‚r‚s lors de la
    cr‚ation d'un document HTML.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences/GIF"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff‚rente.

&NFS

&SY2
   $PrintGIFBackColor {Black|Blue|Magenta|Cyan|Red|Green|Yellow|White}
&TX

&EX2
   $PrintGIFBackColor Yellow
&TX
>
<PrintGIFTransColor>
    PrintGIFTransColor
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
 D‚finit la couleur consid‚r‚e comme "transparente" dans les fichiers
 GIF g‚n‚r‚s lors de la cr‚ation d'un document HTML. Cela permet de
 faire en sorte qu'une des couleurs se confonde avec le fond de l'‚cran.
 L'option "Transparente" doit ˆtre d‚finie par la commande
 ~c$PrintGIFTransparent~C ~cYes~C.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences/GIF"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff‚rente.

&NFS

&SY2
   $PrintGIFTransColor {Black|Blue|Magenta|Cyan|Red|Green|Yellow|White}
&TX

&EX2
   $PrintGIFTransColor White
&TX
>

<PrintGIFTransparent>
    PrintGIFTransparent
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Indique si les fichiers GIF g‚n‚r‚s lors de la cr‚ation d'un document
HTML doivent ˆtre au format "transparent". Cela permet de faire en sorte
qu'une des couleurs (… pr‚ciser par la commande $PrintGIFTransColor) se
confonde avec le fond de l'‚cran.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences/GIF"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff‚rente.

&NFS

&SY2
   $PrintGIFTransparent {Yes|No}
&TX

&EX2
   $PrintGIFTransparent No
&TX
>
<PrintGIFInterlaced>
    PrintGIFInterlaced
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Indique si les fichiers GIF g‚n‚r‚s lors de la cr‚ation d'un document
HTML doivent ˆtre au format interlac‚.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences/GIF"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff‚rente.

&NFS

&SY2
   $PrintGIFInterlaced {Yes|No}
&TX

&EX2
   $PrintGIFInterlaced No
&TX
>
<PrintGIFFilled>
    PrintGIFFilled
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Indique s'il faut remplir les barres dans les bar charts des graphiques GIF.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences/GIF"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff‚rente.

&NFS

&SY2
   $PrintGIFilled {Yes|No}
&TX

&EX2
   $PrintGIFFilled No
&TX
>
<PrintGIFFont>
    PrintGIFFont
    ÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
    D‚termine la police de caractŠres … utiliser dans la cr‚ation des graphiques
    en GIF. Les polices disponibles sont :

&EN 0: Tiny
&EN 1: Small
&EN 2: MediumBold
&EN 3: Large
&EN 4: Giant
&EN 5: Grayed

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences/GIF"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff‚rente.

&NFS

&SY2
   $PrintGIFFont FontNb (entre 0 et 5)
&TX

&EX2
   $PrintGIFFont 1
&TX
>


<Traduction des fichiers A2M>
    Traduction des fichiers A2M
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les fonctions suivantes permettent de traduire des fichiers A2M en
diff‚rents formats:

&EN <A2mToHtml> : traduction en HTML
&EN <A2mToRtf> : traduction en Word (Rtf)
&EN <A2mToMif> : traduction en Frame (MIF)
&EN <A2mToCsv> : traduction en format CSV
&EN <A2mToPrinter> : interprŠte et imprime un fichier a2m
>

<A2mToHtml>
    A2mToHtml
    ÄÄÄÄÄÄÄÄÄ

&DE
    Traduit en HTML un fichier A2M. Si un pr‚c‚dent appel … la commande
    $PrintHtmlHelp a ‚t‚ effectu‚, le r‚sultat sera un fichier HTML ou
    plusieurs fichiers permettant de cr‚er un Help Html.


&FS
    Cette fonction n'existe pas sous forme interactive

&NFS
&SY2
    $A2mToHtml filein fileout
&TX
    o—
&CO
    filein est le nom du fichier A2M input
    filout est le nom du fichier r‚sultat
&TX

Dans le cas de la cr‚ation d'un HtmlHelp, plusieurs fichiers r‚sultat seront
cr‚‚s :

&CO
    fileout.hhp
    fileout.hhc
    fileout.hhk
    fileout.htm
    T*.htm
&TX
>


<A2mToRtf>
    A2mToRtf
    ÄÄÄÄÄÄÄÄÄ

&DE
    Traduit en RTF un fichier A2M. Si un pr‚c‚dent appel … la commande
    $PrintRtfHelp a ‚t‚ effectu‚, le r‚sultat sera un fichier RTF
    permettant de cr‚er une Aide Windows.

&FS
    Cette fonction n'existe pas sous forme interactive

&NFS
&SY2
    $A2mToRtf filein fileout
&TX
    o—
&CO
    filein est le nom du fichier A2M input
    filout est le nom du fichier r‚sultat
&TX

Dans le cas de la cr‚ation d'une Aide Windows, plusieurs fichiers r‚sultat seront
cr‚‚s :

&CO
    fileout.rtf
    fileout.hpj
    fileout.cnt
&TX
>

<A2mToMif>
    A2mToMif
    ÄÄÄÄÄÄÄÄ

&DE
    Traduit en MIF un fichier A2M.

&FS
    Cette fonction n'existe pas sous forme interactive

&NFS
&SY2
    $A2mToMif filein fileout
&TX
    o—
&CO
    filein est le nom du fichier A2M input
    filout est le nom du fichier r‚sultat
&TX
>

<A2mToCsv>
    A2mToCsv
    ÄÄÄÄÄÄÄÄ

&DE
    Traduit en CSV un fichier A2M.

&FS
    Cette fonction n'existe pas sous forme interactive

&NFS
&SY2
    $A2mToCsv filein fileout
&TX
    o—
&CO
    filein est le nom du fichier A2M input
    filout est le nom du fichier r‚sultat
&TX
>

<A2mToPrinter>
    A2mToPrinter
    ÄÄÄÄÄÄÄÄ

&DE
    Traduit un fichier a2m et l'imprime sur l'imprimante courante.

&FS
    Cette fonction n'existe pas sous forme interactive

&NFS
&SY2
    $A2mToPrinter file.a2m
&TX
>


<Autres fonctions de rapports        >
    Autres fonctions de rapports
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&EN <StatUnitRoot> : tests de Dickey-Fuller
&EN <WsAggrChar> : d‚finit le caractŠre … introduire dans le code des s‚ries cr‚‚es par WsAggr*
&EN <WsAggrSum> : effectue la somme de s‚ries
&EN <WsAggrMean> : effectue la moyenne de s‚ries
&EN <WsAggrProd> : effectue le produit de s‚ries
>

<StatUnitRoot>
    StatUnitRoot
    ÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
    Les tests de Dickey-Fuller sont disponibles au niveau des rapports
    et du panneau d'estimation d'‚quations.

    Les tests sont sauvegard‚s dans des scalaires dont le nom est
    compos‚ du pr‚fixe ~cdf_~C et du nom de la premiŠre s‚rie apparaissant
    dans la formule … tester. Par exemple, le test pour la formule
    ~cd(A0GR+A0GF)~C est ~cdf_a0gr.~C

&FS
Dans le panneau d'estimation, la touche F3 ou le bouton "Unit Root"
permettent de sp‚cifier et de tester une ou plusieurs formules. Les
r‚sultats sont affich‚s dans la fenˆtre.

Le seul scalaire sauvegard‚ est celui correspondant … la derniŠre
expression test‚e.

&NFS
La commande qui permet d'effectuer le calcul dans les rapports est:

&CO
    $StatUnitRoot drift trend order expression
&TX
    o—
&EN ~cdrift~C : 0 ou 1 selon que la formule … estimer doive incorporer
		un terme constant (1) ou non (0)
&EN ~ctrend~C : 0 ou 1 selon que la formule … estimer doive incorporer
		un terme de trend (1) ou non (0)
&EN ~corder~C : l'ordre du polynome … estimer pour obtenir les tests
&EN ~cexpression~C : forme LEC … tester


Par exemple :
&CO
    $StatUnitRoot 1 1 3 A
&TX
    L'‚quation estim‚e est :
&CO
    d(A) := df_ * A[-1]+
	    df_d +    /* DRIFT */
	    df_t * t +  /* TREND */
	    df1 * d(A[-1]) + df2*d(A[-2]) + df3*d(A[-3]) /* ORDER */
&TX
Seuls le test de Dickey-Fuller est sauvegard‚ dans un scalaire sous le
nom ~cdf_a~C dans le cas de l'exemple.
>

<WsAggrChar> :
    WsAggrChar
    ÄÄÄÄÄÄÄÄÄÄÄ
D‚finit le caractŠre … introduire dans le code des s‚ries cr‚‚es par WsAggr*

Voir <-WsAggrSum>
>
<WsAggrMean>
    WsAggrMean
    ÄÄÄÄÄÄÄÄÄÄ
Effectue la moyenne de s‚ries. Voir <-WsAggrSum>.
>
<WsAggrProd>
    WsAggrProd
    ÄÄÄÄÄÄÄÄÄÄ
Effectue le produit de s‚ries. Voir <-WsAggrSum>.
>

<WsAggrSum>
    WsAggrSum
    ÄÄÄÄÄÄÄÄÄ

    Des fonctions de rapports permettent d'effectuer
    des agr‚gations, des produits ou des sommes de s‚ries.
    Les s‚ries … traiter peuvent se trouver en WS ou dans un
    fichier externe.

    Quatre fonctions de rapport ont ‚t‚ d‚finies … cet effet:
&CO
    $WsAggrChar [char] : d‚finit le caractŠre … introduire dans le
			 code des s‚ries cr‚‚es
    $WsAggrSum  pattern [filename] : somme des s‚ries d‚finies par pattern
    $WsAggrMean pattern [filename] : moyenne des s‚ries
    $WsAggrProd pattern [filename] : produit des s‚ries
&TX

    Si ~cfilename~C est d‚fini, les s‚ries du fichier seront utilis‚es …
    la place de celles du WS.

    ~cpattern~C est d‚fini comme une s‚quence de parties de noms plac‚es
    entre crochets ou parenthŠses. Chaque partie peut contenir des
    caractŠres alphanum‚riques ou un point d'interrogation.

    Les parties de noms entre parenthŠses ne sont pas agr‚g‚es. Celles
    entre crochets le sont.

&EX2
    Soit un WS avec des s‚ries par pays (BE, FR, GE), et par secteur
    (S101..S999):
&CO
	    BES101, BES102 ... BES199
	    BES201, BES202 ... BES299
	    ...
	    BES901, BES902 ... BES999

	    FRS101, FRS102 ... FRS199
	    FRS201, FRS202 ... FRS299
	    ...
	    FRS901, FRS902 ... FRS999

	    ...

	    GBS101, GBS102 ... GBS199
	    GBS201, GBS202 ... GBS299
	    ...
	    GBS901, GBS902 ... GBS999
&TX
    On peut cr‚er la somme de tous les secteurs pour chaque pays par
    les commandes :
&CO
    $WsAggrChar _
    $WsAggrSum (??)[????]
&TX
    Les s‚ries cr‚‚es seront :
&CO
	    BE____, FR____, ..., GB____
&TX
    Les points d'interrogations entre () permettent de pr‚ciser les
    codes de s‚ries … traiter. Les autres indiquent les parties …
    agr‚ger. Dans ce cas les points d'interrogation sont
    remplac‚s par des _ (ou un autre caractŠre selon l'argument
    de ~c$WsAggrChar~C) dans les s‚ries r‚sultats. Ce caractŠre
    peut ˆtre blanc. Dans l'exemple, les s‚ries cr‚‚es sont alors BE, FR
    et GB.

    On peut ‚galement cr‚er la somme de tous les pays par secteur ou
    la somme de tous les pays pour certains secteurs :
&CO
	$WsAggrSum [??](????) : cr‚e __S101, __S102, ...
	$WsAggrSum [??](??)[??] : cr‚e __S1__, __S1__, ...
&TX
    On peut limiter la cr‚ation … un seul ou … quelques codes :
&CO
	$WsAggrSum (BE)[S??9] : cr‚e BES__9
	$WsAggrSum (BES)[?](?9) : cr‚e BES_09, BES_19, BES_29, ... BES_99
&TX
&IT CaractŠre de regroupement
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
    La commande ~c$WsAggrChar~C permet de sp‚cifier le caractŠre …
    placer dans les s‚ries g‚n‚r‚es. Ce caractŠre peut ˆtre blanc.
    Pour ‚viter que des s‚ries d‚j… agr‚g‚es soient reprises dans une
    agr‚gation ult‚rieure, ces s‚ries ne sont pas retenues dans le
    calcul si le caractŠre d'agr‚gat courant se trouve … une position
    correspondant dans pattern … un point d'interrogation entre
    crochets. Ainsi, la s‚rie ~cBE____~C ne sera pas reprise dans le
    calcul ~c(BE)[????]~C. Par contre elle sera reprise dans le calcul
    ~c[??](????)~C, car dans ce dernier cas elle n'intervient pas dans
    la somme.

    Supposons que les s‚ries ~cBE____~C, ~cFR____~C et ~cGB____~C soient
    d‚finies ainsi que ~cBES101~C, ...

    ~c$WsAggrSum~C ~c(??)[????]~C g‚nŠre ~cBE____~C, ~cFR____~C, etc.
    Elle n'utilise donc pas les s‚ries contenant ~c_~C aprŠs la deuxiŠme
    position, comme ~cBE____~C. En effet, si on les reprenaient, on
    additionnerait deux fois les mˆmes s‚ries.

    ~c$WsAggrSum~C ~c[??](????)~C g‚nŠre ~c______~C en prenant la somme
    de ~cBE____,~C ~cFR____,~C ~cGB____,~C ce qui est correct car les
    autres s‚ries (comme ~cBES101~C) donnent lieu … d'autres s‚ries
    (~c__S101~C).
>

<FileImportCmt>
    FileImportCmt
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
    Cette fonction concerne les importations de commentaires en diff‚rents
    formats comme DIF, ASCII, rotated ASCII, DIF Belgostat, TXT Belgostat et
    PRN Aremos.

    Voir <Import> pour plus d'informations.

&FS
    ##FileImportCmt

&NFS
&SY2
    $FileImportCmt format rule infile outfile language [trace]
&TX
    o—
&CO
	format = {Ascii, Rotated_Ascii, DIF, NIS, GEM, PRN, TXT_Belgostat}
	rule = rulefile to use to translate names
	infile = file to import
	outfile = IODE-file with imported series
	language =  {E,F,D}
	trace = debug file (optional)
&TX

&EX
	$FileImportCmt TXT bstrule.txt bf-06ser.txt bh\p6d.cmt D p6d.log
&TX
>

<FileImportVar>
    FileImportVar
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
    Cette fonction concerne les importations
    de VARIABLES en diff‚rents formats comme DIF, ASCII, rotated ASCII, DIF
    Belgostat, TXT Belgostat, PRN Aremos et GEM (Belgostat).

    Voir <Import> pour plus d'informations.

&FS
    ##FileImportVar

&NFS
&SY2
    $FileImportVar format rule infile outfile from to  [trace]
&TX
    o—
&CO
	format = {A, R, D, N, G, P, T}
	rule = rulefile to use to translate names
	infile = file to import
	outfile = IODE-file with imported series
	from = begin of sample
	to   = end of sample
	trace = debug file (optional)
&TX

&EX
	$FileImportVar TXT bstrule.txt bf-06obs.txt bh\p6y.var 1980Y1 2000Y1 p6y.log
	$FileImportVar GEM rule.gem infile.gem iode\test.var 1980Y1 1990Y1
&TX
>


<WsImportEviews>
    WsImportEviews
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Extraction of equations, scalars and identities from E-Views export data.

&IT Format of E-Views data
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&CO
Forecasting Equation:
=========================
D(B_PQVC) = C(1)*D(PQVC) + C(2)*(D2001+D2002) + C(3)*D(B_PQVC(-3))

Substituted Coefficients:
=========================
D(B_PQVC) = 0.894661573413*D(PQVC) - 0.0284533462569*(D2001+D2002) + 0.241546373731*D(B_PQVC(-3))

@IDENTITY gro_w_produz_def  = w_produz_def  / w_produz_def(-1)  - 1

Identities:
===========
KEEP COEFS:
B_QHOA  = b_1(2)  * b_c0122222
B_QHOB  = b_qh_b_14(1)  * B_CO14

DROP COEFS:
B_QHOCADD  = (b_qh_c_1(1)  * B_CO1)  + (b_qh_c_2(1)  * B_CO2)  + (b_qh_c_7(1)  * B_CO7)  + (b_qh_c_13(1)  * B_CO13)  + (b_qh_c_14(1)  * B_CO14)
B_QHOCR_NC  = b_qh_cr_14(1)  * B_CO14

&TX


The E-views file is interpreted as decribed below :

&EN the 2 equations following the titles "Forecasting Equation" and "Substituted
Coefficients" are extracted.

&EN the first equation is translated into IODE format :
&EN2  ~cD(...)~C is replaced by ~cd(...)~C
&EN2  ~cC(<<n>>)~C is replaced by the ~cendoname_<<n>>~C
&EN2  ~cExpr(<<lag>>)~C is replaced by ~cExpr[<<lag>>]~C
&EN2  the first encountered variable is chosen as the endogenous variable
&EN2 the first = sign is considered as the separator between left and right
    members of the equation and therefore replaced by :=

&EN the members NAME(n) are remplaced by name_n or name. If the last directive is ~cKEEP COEFS:~C,
the ~c_n~C is kept. If the last directive is ~cDROP COEFS:~C, ~c_n~C is dropped.

&EN the coefficients values are extracted from the second equation ("Substituted Coefficients").
&EN the lines ~cEstimation Command:~C and ~cEstimation Equation:~C are ignored
&EN the lines beginning with ~c@INNOV~C are skipped.
&EN the lines beginning by ~c@IDENTITY~C are extracted and translated in
    IODE equations with no coefficient.

&EN every equation is added in the current Equations WS.
&EN every detected coefficient is saved in the Scalars WS.

&IT Lines following the line ~cIDENTITIES:~C

Les lignes contenant des identit‚s sont interpr‚t‚es comme ‚quations … partir du moment o— :
&EN soit une ligne contenant le texte ~cIdentities:~C est trouv‚e
&EN soit une premiŠre identit‚ pr‚fix‚e par ~c@IDENTITY~C est trouv‚e

A partir de ce moment toutes les lignes contenant du texte sont interpr‚t‚es par
le programme comme des ‚quations IODE.

En cas d'erreur de syntaxe, la lecture s'arrˆte et un message d'erreur est produit.


&IT Directives KEEP COEFS: et DROP COEFS:
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Ces directives doivent se trouver seules sur une ligne. Elles peuvent ˆtre en
majuscules, minuscules ou un m‚lange des deux.


Elles d‚terminent la fa‡on dont les termes des ‚quations ou identit‚s EVIEWS
du type ~cname(1)~C ou ~cNAME(n)~C doivent ˆtre traduites en IODE.

&CO
  KEEP COEFS:
  DROP COEFS:
&TX

&EN AprŠs KEEP COEFS:, la traduction de ~cname(1)~C ou ~cNAME(1)~C donne ~cname_1~C.
&EN AprŠs DROP COEFS:, la traduction de ~cname(1)~C ou ~cNAME(1)~C donne ~cname~C.

Au d‚but de la lecture fichier, ~cDROP COEFS:~C est la valeur initiale.

~uExemple de fichier EVIEWS~U

&CO
Identities:
===========
KEEP COEFS
B_QHOA  = b_1(2)  * b_c0122222
B_QHOB  = b_qh_b_14(1)  * B_CO14

DROP COEFS
B_QHOCADD  = (b_qh_c_1(1)  * B_CO1)  + (b_qh_c_2(1)  * B_CO2)  + (b_qh_c_7(1)  * B_CO7)
B_QHOCR_NC  = b_qh_cr_14(1)  * B_CO14
&TX

~uR‚sultat en IODE~U

&CO
B_QHOA := b_1_2*B_C0122222
B_QHOB := b_qh_b_14_1*B_CO14
B_QHOCADD := (b_qh_c_1*B_CO1)+(b_qh_c_2*B_CO2)+(b_qh_c_7*B_CO7)
B_QHOCR_NC := b_qh_cr_14*B_CO14
&TX


&IT Fonction de rapport
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Pour exploiter ce format, il faut appeler la fonction de rapport suivante :
&CO
    $WsImportEviews filename
&TX

o— filename est le nom du fichier … importer. Les WS courants sont augment‚s des ‚quations
et scalaires d‚tect‚s.

>




<CsvSave>
CsvSave
ÄÄÄÄÄÄÄÄ

&DE
    Sauve des objets en formet CSV (LArray pour les variables).

&FS
    Cette fonction n'existe pas sous forme interactive

&NFS
&SY2
    pour les variables:      $CsvSaveVar filename [sample] var1 var2...
    pour les autres objets : $CsvSave<<type>> filename name1 name2...
&TX
    o—
&CO
    filename est le nom du fichier csv output
    sample est le sample … sauver si tout le sample du WS ne doit pas ˆtre sauvegard‚
    var1 var2... est le liste des variables. Par d‚faut toutes les variables sont sauv‚es
&TX


&EX

    $wsloadvar ..\fun\fun
    $csvsep ;
    $csvdigits 10
    $csvdec .
    $csvnan nan
    $CsvSaveVar fun
    $CsvSaveVar fun1 2003y1 2010y1
    $csvdigits 5
    $CsvSaveVar fun2 2009y1 2010y1 ACAF ACAG
    $CsvAxes Series
    $CsvSaveVar fun3 AC* B*
&TX

  Les fichiers r‚sultant de ce rapports sont :

&CO
fun.csv
var\time;1960;1961;1962;1963;1964;1965;1966;1967;1968;1969;1970;...
ACAF;nan;nan;nan;nan;nan;nan;nan;nan;nan;nan;1.2130001;5.2020001;9.184;...
ACAG;nan;nan;nan;nan;nan;nan;nan;nan;nan;nan;-11.028999;-15.847;-19.288002;... ...

fun1.csv
 var\time;2003;2004;2005;2006;2007;2008;2009;2010
ACAF;nan;nan;nan;nan;nan;nan;nan;nan ACAG;nan;nan;nan;nan;nan;nan;nan;nan
AOUC;nan;0.2478319161;0.2545676576;0.2637957322;0.2762428533;0.2858059161;... ...

fun2.csv
 var\time;2009;2010 ACAF;nan;nan ACAG;nan;nan ...

fun3.csv
Series\time;1960;1961;1962;1963;1964;1965;1966;1967;1968;1969;1970;1971;...
ACAF;nan;nan;nan;nan;nan;nan;nan;nan;nan;nan;1.213;5.202;9.184;8.079;...
ACAG;nan;nan;nan;nan;nan;nan;nan;nan;nan;nan;-11.029;-15.847;-19.288;...
BENEF;11.665;13.607;12.208;14.795;18.797;20.86;17.28;20.49;26.935;32.601;...
BQY;31.777;30.853;30.353;27.37;26.937;35.435;37.148;39.506;42.183;43.18;...


&TX
>


<CsvSep>

CsvSep
ÄÄÄÄÄÄÄÄ

&DE
    D‚finit le s‚parateur de cellules dans les fichiers CSV … g‚n‚rer dans la suite de la session.

&FS
    Cette fonction n'existe pas sous forme interactive

&NFS
&SY2
    $CsvSep sep
&TX
    o—
&CO
    sep est un caractŠre quelconque
&TX


&EX
Voir $CsvSave
&TX

>


<CsvDigits>

CsvDigits
ÄÄÄÄÄÄÄÄ

&DE
    Fixe le nombre de d‚cimales dans les fichiers CSV … g‚n‚rer dans la suite de la session.

&FS
    Cette fonction n'existe pas sous forme interactive

&NFS
&SY2
    $CsvDigits nn
&TX
    o—
&CO
    nn est un nombre entier << 99.
&TX


&EX
Voir $CsvSave
&TX
>




<CsvDec>

CsvDec
ÄÄÄÄÄÄ

&DE
    Fixe le s‚parateur des d‚cimales dans les fichiers CSV … g‚n‚rer dans la suite de la session.

&FS
    Cette fonction n'existe pas sous forme interactive

&NFS
&SY2
    $CsvDec char
&TX
    o—
&CO
    char est un caractŠre quelconque.
&TX


&EX
Voir $CsvSave
&TX
>



<CsvNaN>

CsvNaN
ÄÄÄÄÄÄ

&DE
    Fixe le texte qui indique un nombre ind‚fini dans les fichiers CSV … g‚n‚rer dans la suite de la session.

&FS
    Cette fonction n'existe pas sous forme interactive

&NFS
&SY2
    $CsvNaN texte
&TX
    o—
&CO
    texte est un string quelconque.
&TX


&EX
Voir $CsvSave
&TX
>


<CsvAxes>

CsvAxes
ÄÄÄÄÄÄ

&DE
    Fixe le nom de l'axe des variables dans les fichiers CSV … g‚n‚rer dans la suite de la session.

&FS
    Cette fonction n'existe pas sous forme interactive

&NFS
&SY2
    $CsvAxes texte
&TX
    o—
&CO
    texte est un string quelconque.
&TX


&EX
Voir $CsvSave
&TX
>







