/* REPORT.M */

<Les Commandes de IODE dans les Rapports>
    Les Commandes de IODE dans les Rapports
    컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�

Les commandes IODE permettent d'ex괹uter pratiquement toutes les
fonctions de IODE. Elles sont regroup괻s par fonction:

&EN <Op굍ations sur des fichiers         >
&EN <Op굍ations sur les WS               >
&EN <Op굍ations sur les donn괻s          >
&EN <Op굍ations sp괹ifiques aux 굌uations>
&EN <Configuration de l'imprimante       >
&EN <Impressions d'objets                >
&EN <Compilation et impression de tables >
&EN <Graphiques � partir de tableaux     >
&EN <Op굍ations sur des mod둳es          >
&EN <Ex괹utions d'identit굎              >
&EN <Op굍ations sur des rapports         >
&EN <Interface Excel                     >
&EN <Interface Datastream                >
&EN <Traduction des fichiers A2M         >
&EN <Autres fonctions de rapports        >

Pour rappel 괾alement, les commandes RAPPORT et les commandes IODE
peuvent 늯re des commandes plein 괹ran ou des lignes de commande (sans
interface 괹ran).

Les commandes plein 괹ran sont constitu괻s d'un mot cl� pr괹괺� par le
caract둹e ##. Elles ont la syntaxe suivante :

&CO
    ##commande
&TX

Par exemple, la commande ##WsLoadVar provoquera l'affichage d'un 괹ran
de saisie permettant de pr괹iser le nom du fichier de variables �
charger. Le fonctionnement de ces commandes est en g굈굍al d괹rit de
mani둹e d굏aill괻 dans le "Manuel de l'utilisateur". Lorsque c'est le
cas, il y a lieu de se reporter � ce manuel.

Les commandes en ligne sont constitu괻s d'un mot cl� pr괹괺� par le
caract둹e $. Elles ont la syntaxe suivante :

&CO
    $commande options param둻res
&TX

Par exemple, la commande
&CO
    $PrintTbl 84:8 TEST
&TX
imprimera le tableau TEST pour le sample indiqu�.

Les mots cl굎 sont des mn굆oniques des fonctions ex괹ut괻s. Par exemple
le mot cl� WsCopyScl effectue une copie (Copy) d'un Work Space (Ws) de
scalaires (Scl). DataDuplicateIdt d괺ouble (Duplicate) une donn괻 (Data)
du WS des identit굎 (Idt).

Une table r굎um괻 de la syntaxe de toutes les commandes de rapports peut
늯re trouv괻 en annexe.
>

<Op굍ations sur des fichiers>
    Op굍ations sur des fichiers
    컴컴컴컴컴컴컴컴컴컴컴컴컴�

Les commandes op굍ant sur des fichiers permettent de copier, 괺iter,
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
<Op굍ations sur les WS>
    Op굍ations sur les WS
    컴컴컴컴컴컴컴컴컴컴�

Ces fonctions permettent de joindre, copier, charger et sauver les
fichiers work spaces :

&EN <WsLoad> :    charge un ws   cmt | eqs | idt | lst | scl | tbl | var
&EN <WsCopy>  :   copie un ws    cmt | eqs | idt | lst | scl | tbl | var
&EN <WsMerge> :   fusionne un ws cmt | eqs | idt | lst | scl | tbl | var
&EN <WsClear> :   d굏ruit un ws  cmt | eqs | idt | lst | scl | tbl | var
&EN <WsClearAll> : vide tous les ws
&EN <WsDescr> :   d괹rit un ws   cmt | eqs | idt | lst | scl | tbl | var
&EN <WsSave>  :   sauve un ws    cmt | eqs | idt | lst | scl | tbl | var
&EN <WsSaveCmp> :  sauve un ws en comprimant cmt | eqs | idt | lst | scl | tbl | var
&EN <WsSample> :  change le sample du WS
&EN <WsExtrapolate> : prolonge des s굍ies par extrapolation
&EN <WsLtohStock> :  construit des s굍ies de p굍iodicit� sup굍ieure pour des stocks
&EN <WsLtohFlow> : construit des s굍ies de p굍iodicit� sup굍ieure pour des flux
&EN <WsHtolSum> : construit des s굍ies de p굍iodicit� inf굍ieure (somme)
&EN <WsHtolMean> : construit des s굍ies de p굍iodicit� inf굍ieure (moyenne)
&EN <WsHtolLast> : construit des s굍ies de p굍iodicit� inf굍ieure (derni둹e obs)
&EN <WsSeasonAdj> : construit des s굍ies d굎aisonnalis괻s
&EN <WsSeasAdj> : s굃ectionne le crit둹e de saisonnalisation
&EN <WsTrend> : calcul de s굍ies de trend (Hodrick-Prescott) en passant au log
&EN <WsTrendStd> : calcul de s굍ies de trend (Hodrick-Prescott) par la m굏hode standard
&EN <WsImport> : importation d'un fichier ASCII
&EN <WsExport> : exportation en format ASCII
&EN <WsImportEviews> : importation d'굌uations et scalaires E-Views
&EN <CsvSave> : sauve un ws en format csv
&EN <CsvDigits> : fixe le nombre de d괹imale dans les fichiers CSV
&EN <CsvSep> : fixe le s굋arateur de cellules dans les fichiers CSV
&EN <CsvDec> : fixe le s굋arateur de d괹imales dans les fichiers CSV
&EN <CsvNaN> : fixe le texte indiquant une valeurs ind괽inie dans les fichiers CSV
&EN <CsvAxes> : fixe le nom de l'axe des variables dans les fichiers CSV
>

<Op굍ations sur les donn괻s>
    Op굍ations sur les donn괻s
    컴컴컴컴컴컴컴컴컴컴컴컴컴

Les commandes op굍ant sur des donn괻s des workspaces actifs (en m굆oire)
permettent de copier, 괺iter, effacer, renommer, imprimer, etc les
donn괻s d'un workspace:

&EN <datacreate> :    cr괻 un objet     (cmt | eqs | idt | lst | scl | tbl | var)
&EN <datadelete> :    d굏ruit un objet  (cmt | eqs | idt | lst | scl | tbl | var)
&EN <dataexist>  :    teste un objet    (cmt | eqs | idt | lst | scl | tbl | var)
&EN <dataedit>   :    괺ite un objet    (cmt | eqs | idt | lst | scl | tbl | var)
&EN <dataupdate> :    modifie un objet  (cmt | eqs | idt | lst | scl | tbl | var)
&EN <dataappend> :    ajoute un objet   (cmt | lst)
&EN <dataduplicate> : d괺ouble un objet (cmt | idt | lst | scl | tbl | var)
&EN <datarename> :    renomme un objet  (cmt | eqs | idt | lst | scl | tbl | var)

&EN <datasearch> :    recherche un objet  cmt | eqs | idt | lst | scl | tbl | var
&EN <datascan>   :    parcoure les objets eqs | idt | tbl

&EN <datalistXxx> :   cr괻 une liste d'objets dont les noms r굋ondent � un
	       crit둹e donn�  cmt | eqs | idt | lst | scl | tbl | var
&EN <datalistsort> :  trie une liste par ordre alphab굏ique
&EN <datacompareEps> : fixe le seuil d'괾alit� pour la comparaison des variables
&EN <datacompareXxx> : compare le WS et un fichier et cr괻 des listes
&EN <datacalclst> :   effectue des op굍ations logiques sur des listes
&EN <datacalcvar> :   calcule une variable sur base d'une forme LEC
&EN <datadisplaygraph> : affiche un graphique sur base de s굍ies (sans tableau)
&EN <datasavegraph> :  sauve un graphique calcul� sur base de s굍ies

&EN <datawidthvar> : fixe la largeur des colonnes d'괺ition des s굍ies
&EN <datandecvar>  : fixe le nombre de d괹imales pour l'괺ition des s굍ies
&EN <datamodevar>  : fixe le mode pour l'괺ition des s굍ies
&EN <datastartvar> : fixe la premi둹e p굍iode pour l'괺ition des s굍ies
&EN <datawidthtbl> : fixe la largeur des colonnes d'괺ition des tableaux
&EN <datawidthscl> : fixe la largeur des colonnes d'괺ition des scalaires
&EN <datandecscl>  : fixe le nombre de d괹imales pour l'괺ition des scalaires

&EN <dataeditcnf>   : change les options d'괺ition des variables
&EN <datarasvar>   : m굏hode RAS de compl굏ion d'une matrice de s굍ies
&EN <datapatternXXX> : cr괶tion de listes de noms � partir de pattern

&TX
>


<Configuration de l'imprimante>
    Configuration de l'imprimante
    컴컴컴컴컴컴컴컴컴컴컴컴컴컴�

Les commandes suivantes sont utilis괻s pour configurer l'imprimante ou
le fichier de r괹eption des impressions:

&EN <printdest> : fixe la destination de l'impression
&EN <printdestnew> : printdest et r굀nitialisation du fichier d'impression
&EN <printnbdec> : fixe le nombre de d괹imales pour l'impression
&EN <printlang> : fixe la langue par d괽aut pour l'impression

&EN <PrintA2mAppend> : permet de ne pas vider le fichier a2m
		      avant l'impression
&EN <PrintFont> : fixe la police de caract둹e pour l'impression
&EN <PrintTableFont> : fixe la police de caract둹es pour les tableaux
&EN <PrintTableBox> : fixe la largeur du cadre des tableaux
&EN <PrintTableColor> : permet d'utiliser ou non la couleur dans les tableaux
&EN <PrintTableWidth> : fixe la largeur des tableaux en Frame et Rtf
&EN <PrintTableBreak> : permet ou non les coupures des tableaux sur
			plusieurs pages
&EN <PrintTablePage> : force un saut de page avant chaque tableau

&EN <PrintBackground> : fixe la couleur du background (tableaux et graphiques)
&EN <PrintGraphBox> : fixe la largeur du cadre de graphiques
&EN <PrintGraphBrush> : fixe la densit� du background des graphiques
&EN <PrintGraphSize> : fixe la taille des graphiques
&EN <PrintGraphPage> : force un saut de page avant chaque graphique

&EN <PrintRtfHelp> : g굈둹e un fichier RTF pour un help Windows
&EN <PrintRtfTopic> : cr괻 un nouveau topic (Help Windows)
&EN <PrintRtfLevel> : change le niveau hi굍archique des topics suivants
&EN <PrintRtfTitle> : fixe le titre de l'aide Windows
&EN <PrintRtfCopyright> : fixe le texte du copyright de l'aide Windows

&EN <PrintHtmlHelp> : g굈둹e un fichier HTML pour un help HtmlHelp
&EN <PrintHtmlStrip> : Lors de la g굈굍ation de fichier HTML (A2mToHtml), ne g굈둹e
			pas d'ent늯e ni de footer

&EN <PrintParanum> : permet de num굍oter les titres
&EN <PrintPageHeader> : fixe le titre des pages imprim괻s
&EN <PrintPageFooter> : fixe la footnote des pages imprim괻s
&EN <SetPrinter> : fixe l'imprimante par d괽aut
&EN <PrintOrientation> : fixe l'orientation du papier dans l'imprimante
&EN <PrintDuplex> : fixe le mode recto-verso de l'imprimante
&EN <PrintGIFBackColor> : d괽init la couleur de fond des graphiques
&EN <PrintGIFTransColor>  : d괽init la couleur consid굍괻 comme
				      "transparente"
&EN <PrintGIFTransparent> : indique si le fichier GIF doit 늯re au
				      format transparent
&EN <PrintGIFInterlaced> : indique si le fichier GIF doit 늯re au
				      format interlac�
&EN <PrintGIFFilled> : indique s'il faut remplir les barres dans
				      les bar charts
&EN <PrintGIFFont> : indique si le num굍o du font � utiliser
>

<Impressions d'objets>
    Impressions d'objets
    컴컴컴컴컴컴컴컴컴컴

Les commandes suivantes sont utilis괻s pour imprimer les d괽initions
d'objets :

&EN <printobjdef>  : imprime une d괽inition d'objet cmt | eqs |
		    idt | lst | scl | tbl | var
&EN <printobjtitle> : indique si les titres seuls sont � imprimer
&EN <printobjlec>  : indique le type d'굌uation � imprimer
&EN <printobjinfos> : indique les informations � imprimer

>

<printobjtitle>
    PrintObjTitle
    컴컴컴컴컴컴�

&DE
Cette fonction s'utilise avant $PrintObjDefTbl. Elle indique si les titres
des tableaux sont seuls � 늯re imprim굎 ou si toute la d괽inition
doit l'늯re.

&FS
Les m늤es informations sont introduites dans l'괹ran "Print Objects
definition".

&NFS
&SY2
    $PrintObjTitle 0 ou 1
    0 : d괽inition compl둻e
    1 : titres seuls
&TX
>
<printobjlec>
    PrintObjLec
    컴컴컴컴컴�
&DE
Cette fonction s'utilise avant $PrintObjDefXxx. Elle indique la fa뇇n
dont des 굌uations doivent 늯re imprim괻s: telles quelles ou avec
remplacement des coefficients par leurs valeurs ou encore avec
les t-tests.

&FS
Les m늤es informations sont introduites dans l'괹ran "Print Objects
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
    컴컴컴컴컴컴�

&DE
Cette fonction s'utilise avant $PrintObjDefEqs. Elle indique les
informations � imprimer :
&EN 굌uations seules
&EN 굌uations, commentaires
&EN 굌uations, commentaires et r굎ultats d'estimation

&FS
Les m늤es informations sont introduites dans l'괹ran "Print Objects
definition".

&NFS
&SY2
    $PrintObjInfos 0, 1 ou 2
    0 : 굌uation seule
    1 : 굌uation + commentaire
    2 : 굌uation + commentaire + estimation
&TX
>

<Compilation et impression de tables >
    Compilation et impression de tables
    컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�

Les commandes suivantes sont utilis괻s pour compiler des tableaux et les
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
    컴컴컴컴컴컴컴
&DE
    Cette fonction imprime des graphiques construits directement �
    partir de variables (sans passer par des tableaux). Elle correspond
    � l'utilisation de la touche F8 lors de l'괺ition d'un WS de
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
    컴컴컴�
    Cette fonction imprime un ou des graphiques d괽inis � partir de tableaux.
&SY2
    $PrintGr gsample table_names
&TX

&EX2
    $PrintGr 80/79:15 T1 T2
&TX
>

<ViewWidth>
    ViewWidth
    컴컴컴컴�
&DE
Cette fonction permet de sp괹ifier la largeur des colonnes lors de
l'affichage de tableaux (calcul굎). La valeur doit 늯re comprise entre
2 et 12.

&FS
Cette fonction n'est pas disponible en mode plein 괹ran. Les touches
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
    컴컴컴컴컴
&DE
Cette fonction permet de sp괹ifier la largeur de la premi둹e colonne
(colonne de titres) lors de l'affichage de tableaux (calcul굎). La
valeur doit 늯re comprise entre 2 et 60.

&FS
Cette fonction n'est pas disponible en mode plein 괹ran. Les touches
F2 et s-F2 permettent de changer la largeur de la premi둹e colonne lors
de l'affichage.

&NFS
&SY2
    $ViewWidth0 n
    n entre 2 et 60
&TX
>
<ViewNdec>
    ViewNdec
    컴컴컴컴
&DE
Cette fonction permet de sp괹ifier le nombre de d괹imale des valeurs
affich괻s dans les tableaux (calcul굎). La valeur doit
늯re comprise entre -1 (nombre variable) et 6.

&FS
Cette fonction n'est pas disponible en mode plein 괹ran. Les touches
F4 et s-F4 permettent de changer le nombre de d괹imales lors de
l'affichage.

&NFS
&SY2
    $ViewNdec n
    n entre -1 (variable) et 6
&TX
>

<Graphiques � partir de tableaux     >
    Graphiques � partir de tableaux
    컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
Les commandes suivantes sont utilis괻s pour compiler des tableaux et les
visualiser sous forme de graphiques ou les sauver dans un fichier
A2M.

&EN    <ViewGr>
&EN    <PrintGrAll> (obsolete)
&EN    <PrintGrWin> (obsolete)
&EN    <PrintGrData> (obsolete)
&EN    <PrintGr>
&EN    <DataPrintGraph>
>

<Op굍ations sur des mod둳es>
    Op굍ations sur des mod둳es
    컴컴컴컴컴컴컴컴컴컴컴컴컴

Les commandes suivantes permettent de trier et de simuler un mod둳e, d'estimer et de
recompiler des 굌uations :

&EN    <ModelCalcSCC>
&EN    <ModelSimulateParms>
&EN    <ModelSimulate>
&EN    <ModelSimulateSCC>
&EN    <ModelExchange>
&EN    <ModelCompile>
&EN    <ModelSimulateSaveNiters>
&EN    <ModelSimulateSaveNorms>
>
<Op굍ations sp괹ifiques aux 굌uations>
    Op굍ations sp괹ifiques aux 굌uations
    컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

Les commandes suivantes permettent de modifier les param둻res de
l'estimation et d'estimer des 굌uations :

&EN    <EqsEstimate>
&EN    <EqsStepWise>
&EN    <EqsSetCmt>
&EN    <EqsSetSample>
&EN    <EqsSetMethod>
&EN    <EqsSetInstrs>
&EN    <EqsSetBloc>
>
<Ex괹utions d'identit굎>
    Ex괹utions d'identit굎
    컴컴컴컴컴컴컴컴컴컴컴

Les commandes suivantes permettent d'ex괹uter des identit굎 :

&EN    <idtexecute>
&EN    <idtexecutetrace>
&EN    <idtexecutevarfiles>
&EN    <idtexecutesclfiles>
>

<Op굍ations sur des rapports>
    Op굍ations sur des rapports
    컴컴컴컴컴컴컴컴컴컴컴컴컴�

Les commandes suivantes permettent de manipuler des rapports (ces
fonctions sont r괹ursives):

&EN    <reportexec>
&EN    <reportedit>
>

<filelist     >
    FileList
    컴컴컴컴

Fonction annul괻 � partir de la version 5 de IODE.
>
/*
<fileprint    >
    FilePrint
    컴컴컴컴�

&DE
Cette commande permet d'imprimer un fichier.

Le fichier doit 늯re un fichier texte.

La fonction imprime le fichier sans interpr굏ation sur l'imprimante
d괽inie (voir Print Destination). Le fichier est simplement ajout� au
fichier-imprimante s굃ectionn�.

&FS
La fonction affiche une page de saisie dans laquelle on d괽init le ou
les param둻res de la commande.

Le nom du fichier est indiqu� dans le champ de saisie de la page. Le
champ de saisie est du type DIR. La touche F10 valide l'op굍ation lorsque
la page est compl굏괻.

&MU

&SY2
    ##FilePrint
&TX

&NFS
Le nom du fichier (ou un masque) est pass� en param둻re de la commande.

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
    컴컴컴컴

Fonction annul괻 � partir de la version 5 de IODE.
>

<filedelete>
    FileDeleteXxx
    컴컴컴컴컴컴�
&TPART
&EXTF
&DE
La fonction efface un fichier sur disque portant une des extensions
suivantes :

#include "extl.m1"

&FS
La fonction affiche une page de saisie dans laquelle on d괽init le ou
les param둻res de la commande.

La page de saisie contient deux champs: le premier est un champ MENU
(voir lexique): on y s굃ectionne le type de fichier � effacer (le type
choisi est propos� par d괽aut); le second est un champ DIR (voir lexique):
on y sp괹ifie le nom du fichier � effacer.

Le fichier est effac� lorque la page est valid괻 avec F10.

&MU
&SY2
    ##FileDeleteCmt
&TX
&NFS
On passe en param둻re la liste des fichiers portant l'extension choisie �
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
    컴컴컴컴컴컴�
&TPART
&EXTF
&DE
La fonction renomme un fichier sur disque portant une des extensions :

#include "extl.m1"

&FS
La fonction affiche une page de saisie dans laquelle on d괽init le ou
les param둻res de la commande.

Le type du fichier est sp괹ifi� dans le champ sup굍ieur de la page (champ
MENU) le nom du fichier source et le nom du fichier destination sont
sp괹ifi굎 dans les deux autres champs.

La touche F10 valide l'op굍ation lorsque la page est compl굏괻.

&MU
&SY2
    ##FileRenameCmt
&TX

&NFS
On passe en param둻re le nom du fichier source (ancien nom) et le nom du
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
    컴컴컴컴컴�
&TPART
&EXTF
&DE
La fonction copie un fichier portant une des extensions suivantes :

#include "extl.m1"

&FS
La fonction affiche une page de saisie dans laquelle on d괽init le ou
les param둻res de la commande.

Le type du fichier est sp괹ifi� dans le champ sup굍ieur de la page (champ
MENU) le nom du fichier source et le nom du fichier destination sont
sp괹ifi굎 dans les deux autres champs.

La touche F10 valide l'op굍ation lorsque la page est compl굏괻.

&MU
&SY2
    ##FileCopyCmt
&TX

&NFS
On passe en param둻re le nom du fichier source (ancien nom) et le nom du
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
    컴컴컴컴컴�

&TPART
&EXTWS
&DE
Cette op굍ation permet d'effectuer un WsLoad � partir d'un fichier ASCII.
Rappelons que l'op굍ation de LOAD d'un workspace remplace le contenu du
WS courant par le contenu d'un fichier disque : tous les objets
actuellement d괽inis sont d굏ruits et remplac굎 par ceux d괽inis dans le
fichier � charger.

&FS
Cette fonction ouvre l'괹ran WsLoad dans lequel on peut entrer un nom de
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
On passe en param둻re � la commande le nom du fichier � charger.

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

Si l'on charge un nouveau fichier de variables, la p굍iode
d'괹hantillonnage du WS est remplac괻 par celle du fichier charg�.
>

<WsExport>
    WsExportXxx
    컴컴컴컴�
&TPART
&EXTWS
&DE
Cette fonction r괶lise l'op굍ation inverse de la fonction WsImportXxx : elle
copie sur disque le WS dans son 굏at actuel en format ASCII.

&FS
Elle est identique � celle de la fonction ##WsSave. Il suffit de placer comme
extension aux fichiers une extension diff굍ente que celle des WS de IODE.

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
On passe en param둻re � la commande le nom du fichier � sauver.

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
    컴컴컴컴�

&TPART
&EXTWS
&DE
L'op굍ation de chargement (LOAD) d'un workspace remplace le contenu du
WS courant par le contenu d'un fichier disque : tous les objets
actuellement d괽inis sont d굏ruits et remplac굎 par ceux d괽inis dans le
fichier � charger.

&FS
Cette fonction ouvre un 괹ran dans lequel on peut entrer un nom de
fichier par type d'objet. Au d굋art, les noms sont ceux des WS courants.

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
On passe en param둻re � la commande le nom du fichier � charger.

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

Si l'on charge un nouveau fichier de variables, la p굍iode
d'괹hantillonnage du WS est remplac괻 par celle du fichier charg�.
>

<wscopy       >
    WsCopyXxx
    컴컴컴컴�

&TPART
&EXTWS
&DE
La fonction permet d'ajouter ou de remplacer des objets � partir de WS
sauv굎 sur disque.

Les objets s굃ectionn굎 sont copi굎 dans le WS courant et remplacent les
valeurs actuelles. Le processus s'arr늯e d둺 qu'un objet ne peut 늯re
trouv�.

Le WS courant garde intacts tous les autres objets qui en font partie.

&FS
La fonction affiche une page de saisie dans laquelle on d괽init le ou
les param둻res de la commande.

Les param둻res sont le type d'objet � traiter, le nom du ou des fichiers
contenant les WS o� se trouvent les objets � copier, s굋ar굎 par une
virgule ou point-virgule (sans blancs) et enfin la liste des noms
d'objets.

S'il s'agit de fichiers de variables, la p굍iode � copier peut
괾alement 늯re choisie.

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
On passe en param둻re � la commande le nom du fichier WS sur disque et
la liste des objets � copier.

Si on copie des variables (VAR) d'un autre fichier, le sample intervient
de la fa뇇n suivante:

&EN si aucun sample n'est d괽ini: celui du fichier � copier est utilis�

&EN si un ws de variables est en cours d'utilisation, seules les donn괻s
    du nouveau fichier qui se situent dans la p굍iode d'괹hantillonnage
    courante sont copi괻s

&EN on peut 괾alement d괽inir la p굍iode d'괹hantillonnage dans
    laquelle seront copi괻s les donn괻s du fichier.

Si des s굍ies n'existent pas dans le ws courant, elles sont cr굚es.

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

copie la variable A du fichier maribel dans le WS pour la p굍iode 1990 �
2000 seulement. Si A n'existe pas, elles est cr굚e et seules les
valeurs pour la p굍iode seront fix괻s.

&EX2
    $WsCopyVar maribel;hermes  A B C
&TX
idem, mais sur tout le sample du fichier maribel.
>

<wssave   >
    WsSaveXxx
    컴컴컴컴�
&TPART
&EXTWS
&DE
Cette fonction r괶lise l'op굍ation inverse de la fonction LOAD : elle
copie sur disque le WS dans son 굏at actuel, en rempla놹nt 굒entuellement
les fichiers existants.

&FS
La fonction affiche une page de saisie dans laquelle on d괽init le ou
les param둻res de la commande.

La fen늯re pr굎ente pour chaque type de WS

&EN le nom du fichier associ� au WS. Par d괽aut, ce nom est ws.type o�
    type indique le type d'objet. Ce nom peut 늯re modifi� par la
    fonction "Name Workspace". Si le WS a 굏� charg� � partir du disque,
    le nom du WS est le nom du fichier d'origine.

&EN le nombre d'objets actuellement d괽inis dans le WS

Le nom du fichier peut 늯re modifi� avant le sauvetage. Un nom vide ne
donnera pas lieu � un sauvetage. Les WS courants ne sont pas modifi굎 par
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
On passe en param둻re � la commande le nom du fichier � sauver.

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
    컴컴컴컴�

Les diff굍ents WS peuvent 늯re comprim굎 au moment de leur sauvetage. Selon
le type d'objet et leur d괽inition, la compression peut aller de 30 � 75%.

Chaque objet 굏ant comprim� individuellement, le co뻯 au niveau du temps de
sauvetage peut 늯re important pour les fichiers contenant de nombreux objets
ou sur des machines lentes (1000 variables de 25 obs. � la sec. sur un
Pentium III 450 MHz).

Iode reconna똳 et transforme automatiquement les fichiers comprim굎.

Le chargement n'est pratiquement pas p굈alis� par la proc괺ure de
d괹ompression. Un seul cas fait exception : les fichiers de variables dont
les s굍ies doivent 늯re charg괻s par une fonction de type $WsCopyVar: dans
ce cas, la lecture est ralentie par le fait que les longueurs des s굍ies
comprim괻s deviennent variables et que le fichier doit par cons굌uent 늯re
lu s굌uentiellement.

Le panneau de sauvetage pr굎ente un checkbox qui permet d'indiquer si on
souhaite ou non comprimer les fichiers.

Cette fonction est identique � <WsSave> � ceci pr둺 qu'elle comprime les
fichiers en sauvant les donn괻s.

&SY2
    $WsSaveCmpXxx fichier
    {Xxx = cmt | eqs | idt | lst | scl | tbl | var}
&TX
>

<wsmerge      >
    WsMergeXxx
    컴컴컴컴컴
&TPART
&EXTWS
&DE
Cette fonction r괶lise l'op굍ation de fusion de plusieurs fichiers avec
le WS courant : elle ajoute dans le WS actif les objets d괽inis dans un
fichier.

&FS
Cette fonction ouvre un 괹ran dans lequel on peut entrer le type d'objet
et le nom du fichier � fusionner.

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
On passe en param둻re � la commande le nom du fichier.

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
    컴컴컴컴컴
&TPART
&EXTWS
&DE
Cette fonction cr괻 ou remplace la description du fichier. Cette
description sera sauv괻 ult굍ieurement si l'on sauve le fichier.

&FS
La fonction affiche une page de saisie dans laquelle on d괽init le ou
les param둻res de la commande.

La fen늯re pr굎ente pour chaque type de WS le commentaire associ� au WS.
Si le WS a 굏� charg� � partir du disque, le commentaire est celui
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
On passe en param둻re � la description du fichier.

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
    컴컴컴컴컴
&TPART
&EXTWS
&DE
Cette fonction d굏ruit tous les objets du type sp괹ifi�.

&FS
La fonction affiche une page de saisie dans laquelle on d괽init le ou
les param둻res de la commande.

La fen늯re pr굎ente pour chaque type de WS le nombre d'objets
actuellement d괽inis dans le WS.

Presser sur le bouton correspondant au type souhait� ou sur le bouton
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
On ne passe pas de param둻re.

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
    컴컴컴컴
&DE
Cette fonction permet de modifier le sample du WS.

Si le sample est plus court que le sample courant, les donn괻s situ괻s au
del� sont d굏ruites.

Si au contraire le sample est plus long que le sample courant, la valeur
NA (--) est donn괻 aux p굍iodes ajout괻s.

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
    컴컴컴컴컴컴�

&DE
Cette fonction permet de compl굏er des s굍ies selon une m굏hode au choix
bas괻 sur les p굍iodes pr괹괺entes.

Les m굏hodes possibles sont les suivantes :

&EN 0 = Y := Y[-1], if Y null or NA
&EN 1 = Y := Y[-1], always
&EN 2 = Y := extrapolation, if Y null or NA
&EN 3 = Y := extrapolation, always
&EN 4 = Y := unchanged, always
&EN 5 = Y := Y[-1], if Y is NA
&EN 6 = Y := extrapolation, if Y is NA

Il faut fournir la p굍iode (inclue dans celle d괽inie dans le WS
courant) sur laquelle le calcul doit 늯re effectu�.

De plus, la liste des variables � adapter peut 괾alement 늯re pr괹is괻.
Si cette liste est laiss괻 vide, toutes les s굍ies du WS sont modifi괻s.

&FS
&SY2
    ##WsExtrapolate
&TX

&MU

&NFS
&SY2
    $WsExtrapolate [method] from to [liste de variables]
    o� method : 0 ... 6
       from, to := p굍iodes de IODE (yyyyPpp)
&TX

&EX2
    $WsExtrapolate 1993Y1 2000Y1 A B C
ou
    $WsExtrapolate 1993Y1 2000Y1
&TX
>

<wsltohstock>
    WsLtoHStock
    컴컴컴컴컴�

&DE
Construit des s굍ies de p굍iodicit� sup굍ieure pour des donn괻s de type
stock (Ch뱈age, Dette, ...).

Pour ce faire, la fonction charge en WS la liste de s굍ies du fichier
sp괹ifi� et modifie simultan굆ent la p굍iodicit� de ces s굍ies. La
nouvelle p굍iodicit� est celle actuellement d괽inie dans le WS actif.

Les nouvelles s굍ies sont ajout괻s ou remplacent (pour des noms
existants) celles du WS actif.

Cette proc괺ure existe pour les cas suivants :

&EN annuel vers mensuel
&EN annuel vers trimestriel
&EN trimestriel vers mensuel

Deux m굏hodes sont disponibles, l'une pour les stock, l'autre pour les
flux (WsLtoHFlow).

Dans le cas des stock, La m굏hode d'interpolation est au choix :

&EN lin괶ire : A[1980Q{1,2,3,4}] = A[1979Y1] + i * (A[1980Y1] - A[1979Y1])/4
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

    avec L pour interpolation lin괶ire
	 C pour interpolation par Cubic Splines
	 S pour interpolation par Steps
&TX

&EX2
    $WsLtoHStock C Fichier1 A B C
&TX
>

<wsltohflow>
    WsLtoHFlow
    컴컴컴컴컴

&DE
Construit des s굍ies de p굍iodicit� sup굍ieure pour des donn괻s de type
flux (PNB, D괽icit, ...).

Pour ce faire, la fonction charge en WS la liste de s굍ies du fichier
sp괹ifi� et modifie simultan굆ent la p굍iodicit� de ces s굍ies. La
nouvelle p굍iodicit� est celle actuellement d괽inie dans le WS actif.

Les nouvelles s굍ies sont ajout괻s ou remplacent (pour des noms
existants) celles du WS actif.

Cette proc괺ure existe pour les cas suivants :

&EN annuel vers mensuel
&EN annuel vers trimestriel
&EN trimestriel vers mensuel

Deux m굏hodes sont disponibles, l'une pour les stock, l'autre pour les
flux.

Dans le cas des flux, la s굍ie est r굋artie sur les sous-p굍iodes :

&EN interpolation lin괶ire   : A[1980Q1] = A[1980Y1] / n
				(ou n = nbre de sous-p굍iodes)
&EN interpolation par splines : interpolation cubique
&EN interpolation par steps   : lin괶ire

&FS
&SY2
    ##WsLtoH
&TX

&NFS
&SY2
    $WsLtoHFlow {L|C|S}  Filename VarList

    avec L pour interpolation lin괶ire
	 C pour interpolation par Cubic Splines
	 S pour interpolation par Steps
&TX

&EX2
    $WsLtoHFlow S Fichier1 A B C
&TX
>

<wshtolsum>
    WsHtoLSum
    컴컴컴컴�

&DE
Construit des s굍ies de p굍iodicit� inf굍ieure en additionnant les
sous-p굍iodes.

Pour ce faire, la fonction charge en WS la liste de s굍ies du fichier
sp괹ifi� et modifie simultan굆ent la p굍iodicit� de ces s굍ies. La
nouvelle p굍iodicit� est celle actuellement d괽inie dans le WS actif.

Les nouvelles s굍ies sont ajout괻s ou remplacent (pour des noms
existants) celles du WS actif.

Cette proc괺ure existe pour les cas suivants :

&EN mensuel vers annuel (observation annuelle = somme des 12 mois)

&EN trimestriel vers annuel (observation annuelle = somme des 4
    trimestres)

&EN mensuel vers trimestriel (observation trimestrielle = somme des 3
    mois)

Trois m굏hodes sont disponibles :

&EN Addition de sous-p굍iodes (flux) : WsHtoLSum
&EN Moyenne de sous-p굍iodes (stock) : WsHtoLMean
&EN Derni둹e observation (stock) : WsHtoLLast

En cas de valeur inexistante (NA) pour l'une des sous-p굍iodes, le
r굎ultat est NA.

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
    컴컴컴컴�

&DE
Construit des s굍ies de p굍iodicit� inf굍ieure en prenant la moyenne des
sous-p굍iodes.

Pour ce faire, la fonction charge en WS la liste de s굍ies du fichier
sp괹ifi� et modifie simultan굆ent la p굍iodicit� de ces s굍ies. La
nouvelle p굍iodicit� est celle actuellement d괽inie dans le WS actif.

Les nouvelles s굍ies sont ajout괻s ou remplacent (pour des noms
existants) celles du WS actif.

Cette proc괺ure existe pour les cas suivants :

&EN mensuel vers annuel (observation annuelle = moyenne des 12 mois)

&EN trimestriel vers annuel (observation annuelle = moyenne des 4
    trimestres)

&EN mensuel vers trimestriel (observation trimestrielle = moyenne des 3
    mois)

Trois m굏hodes sont disponibles :

&EN Addition de sous-p굍iodes (flux) : WsHtoLSum
&EN Moyenne de sous-p굍iodes (stock) : WsHtoLMean
&EN Derni둹e observation (stock) : WsHtoLLast

En cas de valeur inexistante (NA) pour l'une des sous-p굍iodes, le
r굎ultat est NA.

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
    컴컴컴컴�

&DE
Construit des s굍ies de p굍iodicit� inf굍ieure en prenant la derni둹e
observation des sous-p굍iodes.

Pour ce faire, la fonction charge en WS la liste de s굍ies du fichier
sp괹ifi� et modifie simultan굆ent la p굍iodicit� de ces s굍ies. La
nouvelle p굍iodicit� est celle actuellement d괽inie dans le WS actif.

Les nouvelles s굍ies sont ajout괻s ou remplacent (pour des noms
existants) celles du WS actif.

Cette proc괺ure existe pour les cas suivants :

&EN mensuel vers annuel (observation annuelle = celle de d괹embre)

&EN trimestriel vers annuel (observation annuelle = celle du dernier
    trimestre)

&EN mensuel vers trimestriel (observation trimestrielle = celle du
    dernier mois du trimestre)

Trois m굏hodes sont disponibles :

&EN Addition de sous-p굍iodes (flux) : WsHtoLSum
&EN Moyenne de sous-p굍iodes (stock) : WsHtoLMean
&EN Derni둹e observation (stock) : WsHtoLLast

En cas de valeur inexistante (NA) pour l'une des sous-p굍iodes, le
r굎ultat est NA.

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
    컴컴컴컴컴�

&DE
Construit des s굍ies dessaisonnalis괻s � l'aide de la m굏hode Census XI,
ainsi que les composantes de trend cyclique et stochastiques.

Nomenclature :

&EN la s굍ie contenant la composante de trend cyclique se nomme :
&CO
    _Cname o� name est le nom original
&TX
&EN la s굍ie contenant la composante stochastique se nomme :
&CO
    _Iname o� name est le nom original
&TX
&EN la s굍ie dessaisonnalis괻 garde son nom.

Notons que la s굍ie dessaisonnalis괻 est le produit des deux autres.

Pour ce faire, la fonction charge en WS la liste de s굍ies du fichier
sp괹ifi� et modifie simultan굆ent les s굍ies s굃ectionn괻s si
n괹essaire.

Les nouvelles s굍ies sont ajout괻s ou remplacent (pour des noms
existants) celles du WS actif.

En cas de valeur inexistante (NA) pour l'une des p굍iodes, le
r굎ultat est NA pout toute l'ann괻.

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
    컴컴컴컴컴�

&DE
Cette fonction est identique � $WsSeasonAdj � ceci pr둺 qu'elle permet
de fixer le param둻re qui permet de lib굍er le crit둹e v굍ifiant si une
influence saisonni둹e est pr굎ente dans une s굍ie.

&FS
&SY2
    ##WsSeasonAdj
&TX
Le param둻re peut 늯re introduit dans le panneau de lancement.

&NFS
&CO
    $WsSeasAdj Filename Varname Varname ... Eps
&TX

&SA $WsSeasonAdj
>

<wstrend>
    WsTrend
    컴컴컴�

&DE
Impl굆entation de la m굏hode Hodrick-Prescott pour la construction de
s굍ie de trend. Le principe est le m늤e que pour la dessaisonnalisation:
les s굍ies lues dans un fichier sont import괻s et transform괻s
simultan굆ent.

Les s굍ies sont transform괻s en ~clog~C avant le calcul et retransform괻s en
~cexp~C apr둺 le calcul. Elles doivent donc 늯re strictement positives.

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
    컴컴컴�

&DE
Impl굆entation de la m굏hode Hodrick-Prescott pour la construction de
s굍ie de trend. Le principe est le m늤e que pour la dessaisonnalisation:
les s굍ies lues dans un fichier sont import괻s et transform괻s
simultan굆ent.

Contrairement � la fonciton ~c$WsTrend,~C les s굍ies ne sont pas transform괻s avant le calcul.
La contrainte de positivit� ne s'applique donc pas.

Un s굍ie lin괶ire (ex. 1..10) le reste apr둺 application du test. Ce n'est pas le cas
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
    컴컴컴컴컴컴컴�

Les fonctions suivantes permettent d'interagir avec Excel � partir
d'un rapport de IODE. Il est 괾alement possible d'utiliser les donn괻s
de IODE dans Excel � travers le serveur DDE de IODE.
Le d굏ail et des exemples sur ce possibilit굎 peuvent 늯re trouv굎 dans
un chapitre s굋ar�.

&EN <ExcelOpen  >  : lance Excel et ouvre un fichier Excel
&EN <ExcelSave  >  : sauve le fichier courant d'Excel
&EN <ExcelSaveAs>  : sauve le fichier courant d'Excel sous un autre nom
&EN <ExcelClose >  : ferme le fichier courant d'Excel
&EN <ExcelNew   >  : cr괻 un nouveau fichier vide dans Excel
&EN <ExcelPrint >  : imprime le fichier courant d'Excel
&EN <ExcelSetXxx>  : copie des objets de IODE dans Excel
&EN <ExcelWrite >  : 괹rit des informations quelconques dans un sheet Excel
&EN <ExcelGetVar>  : copie dans IODE des variables d괽inies dans Excel
&EN <ExcelGetScl>  : copie dans IODE des scalaires d괽inis dans Excel
&EN <ExcelGetCmt>  : copie dans IODE des commentaires d괽inis dans Excel
&EN <ExcelLang  >  : informe IODE de la langue d'Excel pour formater les ranges
&EN <ExcelDecimal> : informe IODE du s굋arateur d괹imal utilis� par Excel
	pour formater les variables envoy괻s � Excel.
&EN <ExcelCurrency>: informe IODE du symbole de devise utilis� par Excel
&EN <ExcelThousand>: informe IODE du s굋arateur de milliers utilis� par Excel
>


<Interface DataStream>
    Interface DataStream
    컴컴컴컴컴컴컴컴컴컴

La fonction suivante permet de charger des donn괻s de Datastream � partir
de la base de donn괻s centrale (Londres). Pour 늯re op굍ationnelle,
la connexion � Internet est indispensable.

&EN <DSImportDB> : charge des donn괻s � partir du site Datastream
>

<SysAppendFile>
    SysAppendFile
    컴컴컴컴컴�

Cette fonction permet de copier un fichier � la suite d'une autre.
En cas d'impossibilit�, la fonction retourne un code d'erreur.

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
    컴컴컴컴컴�

Cette fonction permet de copier un fichier quelconque sur disque.
Contrairement � la fonction ~cFileCopyXxx,~C l'extension peut 늯re
quelconque. Attention, le fichier � copier doit 늯re ferm� pour pouvoir
늯re copi�.

En cas d'impossibilit�, la fonction retourne un code d'erreur.

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
    컴컴컴컴컴�

Cette fonction permet de renommer un fichier quelconque sur disque.
Contrairement � la fonction ~cFileRenameXxx,~C l'extension peut 늯re
quelconque. Attention, le fichier � renommer doit 늯re ferm�. Le fichier
destination ne peut exister.

En cas d'impossibilit�, la fonction retourne un code d'erreur.

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
    컴컴컴컴컴컴�

Cette fonction permet de d굏ruire un fichier quelconque sur disque.
Contrairement � la fonction ~cFileDeleteXxx,~C l'extension peut 늯re
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
컴컴컴컴컴컴

Cette fonction transforme un fichier cod� en OEM-850 en Ansi Windows-1252.

&FS
&NEQ

&NFS
&SY2

    $SysOemToAnsi inputfile outputfile
&TX
>

<SysOemToUTF8>
SysOemToUTF8
컴컴컴컴컴컴
Cette fonction transforme un fichier cod� en OEM-850 en UTF-8.

&FS
&NEQ

&NFS
&SY2

    $SysOemToUTF8 inputfile outputfile
&TX
>

<SysAnsiToOem>
SysAnsiToOem
컴컴컴컴컴컴

Cette fonction transforme un fichier cod� Ansi Windows-1252 en OEM-850.

&FS
&NEQ

&NFS
&SY2

    $SysAnsiToOem inputfile outputfile
&TX
>

<SysAnsiToUTF8>
SysAnsiToUTF8
컴컴컴컴컴컴�
Cette fonction transforme un fichier cod� en Ansi Windows-1252 en UTF-8.

&FS
&NEQ

&NFS
&SY2

    $SysAnsiToUTF8 inputfile outputfile
&TX

>

<mkdir>
    mkdir
    컴컴�

Cette fonction cr괻 un directory s'il n'existe pas.
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
    컴컴�

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
    컴컴�

Cette fonction d굏ruit un directory. Celui-ci doit 늯re vide (ne plus
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
    컴컴컴컴컴컴컴컴�
&DE
    D괽init la couleur de fond des graphiques GIF g굈굍굎 lors de la
    cr괶tion d'un document HTML.

Cette fonction modifie 괾alement la valeur des champs correspondants
dans le panneau ~b"Print Preferences/GIF"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff굍ente.

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
    컴컴컴컴컴컴컴컴컴
&DE
 D괽init la couleur consid굍괻 comme "transparente" dans les fichiers
 GIF g굈굍굎 lors de la cr괶tion d'un document HTML. Cela permet de
 faire en sorte qu'une des couleurs se confonde avec le fond de l'괹ran.
 L'option "Transparente" doit 늯re d괽inie par la commande
 ~c$PrintGIFTransparent~C ~cYes~C.

Cette fonction modifie 괾alement la valeur des champs correspondants
dans le panneau ~b"Print Preferences/GIF"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff굍ente.

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
    컴컴컴컴컴컴컴컴컴�
&DE
Indique si les fichiers GIF g굈굍굎 lors de la cr괶tion d'un document
HTML doivent 늯re au format "transparent". Cela permet de faire en sorte
qu'une des couleurs (� pr괹iser par la commande $PrintGIFTransColor) se
confonde avec le fond de l'괹ran.

Cette fonction modifie 괾alement la valeur des champs correspondants
dans le panneau ~b"Print Preferences/GIF"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff굍ente.

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
    컴컴컴컴컴컴컴컴컴
&DE
Indique si les fichiers GIF g굈굍굎 lors de la cr괶tion d'un document
HTML doivent 늯re au format interlac�.

Cette fonction modifie 괾alement la valeur des champs correspondants
dans le panneau ~b"Print Preferences/GIF"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff굍ente.

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
    컴컴컴컴컴컴컴
&DE
Indique s'il faut remplir les barres dans les bar charts des graphiques GIF.

Cette fonction modifie 괾alement la valeur des champs correspondants
dans le panneau ~b"Print Preferences/GIF"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff굍ente.

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
    컴컴컴컴컴컴
&DE
    D굏ermine la police de caract둹es � utiliser dans la cr괶tion des graphiques
    en GIF. Les polices disponibles sont :

&EN 0: Tiny
&EN 1: Small
&EN 2: MediumBold
&EN 3: Large
&EN 4: Giant
&EN 5: Grayed

Cette fonction modifie 괾alement la valeur des champs correspondants
dans le panneau ~b"Print Preferences/GIF"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff굍ente.

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
    컴컴컴컴컴컴컴컴컴컴컴컴컴�

Les fonctions suivantes permettent de traduire des fichiers A2M en
diff굍ents formats:

&EN <A2mToHtml> : traduction en HTML
&EN <A2mToRtf> : traduction en Word (Rtf)
&EN <A2mToMif> : traduction en Frame (MIF)
&EN <A2mToCsv> : traduction en format CSV
&EN <A2mToPrinter> : interpr둻e et imprime un fichier a2m
>

<A2mToHtml>
    A2mToHtml
    컴컴컴컴�

&DE
    Traduit en HTML un fichier A2M. Si un pr괹괺ent appel � la commande
    $PrintHtmlHelp a 굏� effectu�, le r굎ultat sera un fichier HTML ou
    plusieurs fichiers permettant de cr괻r un Help Html.


&FS
    Cette fonction n'existe pas sous forme interactive

&NFS
&SY2
    $A2mToHtml filein fileout
&TX
    o�
&CO
    filein est le nom du fichier A2M input
    filout est le nom du fichier r굎ultat
&TX

Dans le cas de la cr괶tion d'un HtmlHelp, plusieurs fichiers r굎ultat seront
cr굚s :

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
    컴컴컴컴�

&DE
    Traduit en RTF un fichier A2M. Si un pr괹괺ent appel � la commande
    $PrintRtfHelp a 굏� effectu�, le r굎ultat sera un fichier RTF
    permettant de cr괻r une Aide Windows.

&FS
    Cette fonction n'existe pas sous forme interactive

&NFS
&SY2
    $A2mToRtf filein fileout
&TX
    o�
&CO
    filein est le nom du fichier A2M input
    filout est le nom du fichier r굎ultat
&TX

Dans le cas de la cr괶tion d'une Aide Windows, plusieurs fichiers r굎ultat seront
cr굚s :

&CO
    fileout.rtf
    fileout.hpj
    fileout.cnt
&TX
>

<A2mToMif>
    A2mToMif
    컴컴컴컴

&DE
    Traduit en MIF un fichier A2M.

&FS
    Cette fonction n'existe pas sous forme interactive

&NFS
&SY2
    $A2mToMif filein fileout
&TX
    o�
&CO
    filein est le nom du fichier A2M input
    filout est le nom du fichier r굎ultat
&TX
>

<A2mToCsv>
    A2mToCsv
    컴컴컴컴

&DE
    Traduit en CSV un fichier A2M.

&FS
    Cette fonction n'existe pas sous forme interactive

&NFS
&SY2
    $A2mToCsv filein fileout
&TX
    o�
&CO
    filein est le nom du fichier A2M input
    filout est le nom du fichier r굎ultat
&TX
>

<A2mToPrinter>
    A2mToPrinter
    컴컴컴컴

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
    컴컴컴컴컴컴컴컴컴컴컴컴컴컴

&EN <StatUnitRoot> : tests de Dickey-Fuller
&EN <WsAggrChar> : d괽init le caract둹e � introduire dans le code des s굍ies cr굚es par WsAggr*
&EN <WsAggrSum> : effectue la somme de s굍ies
&EN <WsAggrMean> : effectue la moyenne de s굍ies
&EN <WsAggrProd> : effectue le produit de s굍ies
>

<StatUnitRoot>
    StatUnitRoot
    컴컴컴컴컴컴
&DE
    Les tests de Dickey-Fuller sont disponibles au niveau des rapports
    et du panneau d'estimation d'굌uations.

    Les tests sont sauvegard굎 dans des scalaires dont le nom est
    compos� du pr괽ixe ~cdf_~C et du nom de la premi둹e s굍ie apparaissant
    dans la formule � tester. Par exemple, le test pour la formule
    ~cd(A0GR+A0GF)~C est ~cdf_a0gr.~C

&FS
Dans le panneau d'estimation, la touche F3 ou le bouton "Unit Root"
permettent de sp괹ifier et de tester une ou plusieurs formules. Les
r굎ultats sont affich굎 dans la fen늯re.

Le seul scalaire sauvegard� est celui correspondant � la derni둹e
expression test괻.

&NFS
La commande qui permet d'effectuer le calcul dans les rapports est:

&CO
    $StatUnitRoot drift trend order expression
&TX
    o�
&EN ~cdrift~C : 0 ou 1 selon que la formule � estimer doive incorporer
		un terme constant (1) ou non (0)
&EN ~ctrend~C : 0 ou 1 selon que la formule � estimer doive incorporer
		un terme de trend (1) ou non (0)
&EN ~corder~C : l'ordre du polynome � estimer pour obtenir les tests
&EN ~cexpression~C : forme LEC � tester


Par exemple :
&CO
    $StatUnitRoot 1 1 3 A
&TX
    L'굌uation estim괻 est :
&CO
    d(A) := df_ * A[-1]+
	    df_d +    /* DRIFT */
	    df_t * t +  /* TREND */
	    df1 * d(A[-1]) + df2*d(A[-2]) + df3*d(A[-3]) /* ORDER */
&TX
Seuls le test de Dickey-Fuller est sauvegard� dans un scalaire sous le
nom ~cdf_a~C dans le cas de l'exemple.
>

<WsAggrChar> :
    WsAggrChar
    컴컴컴컴컴�
D괽init le caract둹e � introduire dans le code des s굍ies cr굚es par WsAggr*

Voir <-WsAggrSum>
>
<WsAggrMean>
    WsAggrMean
    컴컴컴컴컴
Effectue la moyenne de s굍ies. Voir <-WsAggrSum>.
>
<WsAggrProd>
    WsAggrProd
    컴컴컴컴컴
Effectue le produit de s굍ies. Voir <-WsAggrSum>.
>

<WsAggrSum>
    WsAggrSum
    컴컴컴컴�

    Des fonctions de rapports permettent d'effectuer
    des agr괾ations, des produits ou des sommes de s굍ies.
    Les s굍ies � traiter peuvent se trouver en WS ou dans un
    fichier externe.

    Quatre fonctions de rapport ont 굏� d괽inies � cet effet:
&CO
    $WsAggrChar [char] : d괽init le caract둹e � introduire dans le
			 code des s굍ies cr굚es
    $WsAggrSum  pattern [filename] : somme des s굍ies d괽inies par pattern
    $WsAggrMean pattern [filename] : moyenne des s굍ies
    $WsAggrProd pattern [filename] : produit des s굍ies
&TX

    Si ~cfilename~C est d괽ini, les s굍ies du fichier seront utilis괻s �
    la place de celles du WS.

    ~cpattern~C est d괽ini comme une s굌uence de parties de noms plac괻s
    entre crochets ou parenth둺es. Chaque partie peut contenir des
    caract둹es alphanum굍iques ou un point d'interrogation.

    Les parties de noms entre parenth둺es ne sont pas agr괾괻s. Celles
    entre crochets le sont.

&EX2
    Soit un WS avec des s굍ies par pays (BE, FR, GE), et par secteur
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
    On peut cr괻r la somme de tous les secteurs pour chaque pays par
    les commandes :
&CO
    $WsAggrChar _
    $WsAggrSum (??)[????]
&TX
    Les s굍ies cr굚es seront :
&CO
	    BE____, FR____, ..., GB____
&TX
    Les points d'interrogations entre () permettent de pr괹iser les
    codes de s굍ies � traiter. Les autres indiquent les parties �
    agr괾er. Dans ce cas les points d'interrogation sont
    remplac굎 par des _ (ou un autre caract둹e selon l'argument
    de ~c$WsAggrChar~C) dans les s굍ies r굎ultats. Ce caract둹e
    peut 늯re blanc. Dans l'exemple, les s굍ies cr굚es sont alors BE, FR
    et GB.

    On peut 괾alement cr괻r la somme de tous les pays par secteur ou
    la somme de tous les pays pour certains secteurs :
&CO
	$WsAggrSum [??](????) : cr괻 __S101, __S102, ...
	$WsAggrSum [??](??)[??] : cr괻 __S1__, __S1__, ...
&TX
    On peut limiter la cr괶tion � un seul ou � quelques codes :
&CO
	$WsAggrSum (BE)[S??9] : cr괻 BES__9
	$WsAggrSum (BES)[?](?9) : cr괻 BES_09, BES_19, BES_29, ... BES_99
&TX
&IT Caract둹e de regroupement
    컴컴컴컴컴컴컴컴컴컴컴컴�
    La commande ~c$WsAggrChar~C permet de sp괹ifier le caract둹e �
    placer dans les s굍ies g굈굍괻s. Ce caract둹e peut 늯re blanc.
    Pour 굒iter que des s굍ies d굁� agr괾괻s soient reprises dans une
    agr괾ation ult굍ieure, ces s굍ies ne sont pas retenues dans le
    calcul si le caract둹e d'agr괾at courant se trouve � une position
    correspondant dans pattern � un point d'interrogation entre
    crochets. Ainsi, la s굍ie ~cBE____~C ne sera pas reprise dans le
    calcul ~c(BE)[????]~C. Par contre elle sera reprise dans le calcul
    ~c[??](????)~C, car dans ce dernier cas elle n'intervient pas dans
    la somme.

    Supposons que les s굍ies ~cBE____~C, ~cFR____~C et ~cGB____~C soient
    d괽inies ainsi que ~cBES101~C, ...

    ~c$WsAggrSum~C ~c(??)[????]~C g굈둹e ~cBE____~C, ~cFR____~C, etc.
    Elle n'utilise donc pas les s굍ies contenant ~c_~C apr둺 la deuxi둴e
    position, comme ~cBE____~C. En effet, si on les reprenaient, on
    additionnerait deux fois les m늤es s굍ies.

    ~c$WsAggrSum~C ~c[??](????)~C g굈둹e ~c______~C en prenant la somme
    de ~cBE____,~C ~cFR____,~C ~cGB____,~C ce qui est correct car les
    autres s굍ies (comme ~cBES101~C) donnent lieu � d'autres s굍ies
    (~c__S101~C).
>

<FileImportCmt>
    FileImportCmt
    컴컴컴컴컴컴�

&DE
    Cette fonction concerne les importations de commentaires en diff굍ents
    formats comme DIF, ASCII, rotated ASCII, DIF Belgostat, TXT Belgostat et
    PRN Aremos.

    Voir <Import> pour plus d'informations.

&FS
    ##FileImportCmt

&NFS
&SY2
    $FileImportCmt format rule infile outfile language [trace]
&TX
    o�
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
    컴컴컴컴컴컴�

&DE
    Cette fonction concerne les importations
    de VARIABLES en diff굍ents formats comme DIF, ASCII, rotated ASCII, DIF
    Belgostat, TXT Belgostat, PRN Aremos et GEM (Belgostat).

    Voir <Import> pour plus d'informations.

&FS
    ##FileImportVar

&NFS
&SY2
    $FileImportVar format rule infile outfile from to  [trace]
&TX
    o�
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
    컴컴컴컴컴컴컴

&DE
Extraction of equations, scalars and identities from E-Views export data.

&IT Format of E-Views data
컴컴컴컴컴컴컴컴컴컴컴컴컴컴

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

Les lignes contenant des identit굎 sont interpr굏괻s comme 굌uations � partir du moment o� :
&EN soit une ligne contenant le texte ~cIdentities:~C est trouv괻
&EN soit une premi둹e identit� pr괽ix괻 par ~c@IDENTITY~C est trouv괻

A partir de ce moment toutes les lignes contenant du texte sont interpr굏괻s par
le programme comme des 굌uations IODE.

En cas d'erreur de syntaxe, la lecture s'arr늯e et un message d'erreur est produit.


&IT Directives KEEP COEFS: et DROP COEFS:
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�

Ces directives doivent se trouver seules sur une ligne. Elles peuvent 늯re en
majuscules, minuscules ou un m굃ange des deux.


Elles d굏erminent la fa뇇n dont les termes des 굌uations ou identit굎 EVIEWS
du type ~cname(1)~C ou ~cNAME(n)~C doivent 늯re traduites en IODE.

&CO
  KEEP COEFS:
  DROP COEFS:
&TX

&EN Apr둺 KEEP COEFS:, la traduction de ~cname(1)~C ou ~cNAME(1)~C donne ~cname_1~C.
&EN Apr둺 DROP COEFS:, la traduction de ~cname(1)~C ou ~cNAME(1)~C donne ~cname~C.

Au d괷ut de la lecture fichier, ~cDROP COEFS:~C est la valeur initiale.

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

~uR굎ultat en IODE~U

&CO
B_QHOA := b_1_2*B_C0122222
B_QHOB := b_qh_b_14_1*B_CO14
B_QHOCADD := (b_qh_c_1*B_CO1)+(b_qh_c_2*B_CO2)+(b_qh_c_7*B_CO7)
B_QHOCR_NC := b_qh_cr_14*B_CO14
&TX


&IT Fonction de rapport
컴컴컴컴컴컴컴컴컴컴컴�
Pour exploiter ce format, il faut appeler la fonction de rapport suivante :
&CO
    $WsImportEviews filename
&TX

o� filename est le nom du fichier � importer. Les WS courants sont augment굎 des 굌uations
et scalaires d굏ect굎.

>




<CsvSave>
CsvSave
컴컴컴컴

&DE
    Sauve des objets en formet CSV (LArray pour les variables).

&FS
    Cette fonction n'existe pas sous forme interactive

&NFS
&SY2
    pour les variables:      $CsvSaveVar filename [sample] var1 var2...
    pour les autres objets : $CsvSave<<type>> filename name1 name2...
&TX
    o�
&CO
    filename est le nom du fichier csv output
    sample est le sample � sauver si tout le sample du WS ne doit pas 늯re sauvegard�
    var1 var2... est le liste des variables. Par d괽aut toutes les variables sont sauv괻s
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

  Les fichiers r굎ultant de ce rapports sont :

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
컴컴컴컴

&DE
    D괽init le s굋arateur de cellules dans les fichiers CSV � g굈굍er dans la suite de la session.

&FS
    Cette fonction n'existe pas sous forme interactive

&NFS
&SY2
    $CsvSep sep
&TX
    o�
&CO
    sep est un caract둹e quelconque
&TX


&EX
Voir $CsvSave
&TX

>


<CsvDigits>

CsvDigits
컴컴컴컴

&DE
    Fixe le nombre de d괹imales dans les fichiers CSV � g굈굍er dans la suite de la session.

&FS
    Cette fonction n'existe pas sous forme interactive

&NFS
&SY2
    $CsvDigits nn
&TX
    o�
&CO
    nn est un nombre entier << 99.
&TX


&EX
Voir $CsvSave
&TX
>




<CsvDec>

CsvDec
컴컴컴

&DE
    Fixe le s굋arateur des d괹imales dans les fichiers CSV � g굈굍er dans la suite de la session.

&FS
    Cette fonction n'existe pas sous forme interactive

&NFS
&SY2
    $CsvDec char
&TX
    o�
&CO
    char est un caract둹e quelconque.
&TX


&EX
Voir $CsvSave
&TX
>



<CsvNaN>

CsvNaN
컴컴컴

&DE
    Fixe le texte qui indique un nombre ind괽ini dans les fichiers CSV � g굈굍er dans la suite de la session.

&FS
    Cette fonction n'existe pas sous forme interactive

&NFS
&SY2
    $CsvNaN texte
&TX
    o�
&CO
    texte est un string quelconque.
&TX


&EX
Voir $CsvSave
&TX
>


<CsvAxes>

CsvAxes
컴컴컴

&DE
    Fixe le nom de l'axe des variables dans les fichiers CSV � g굈굍er dans la suite de la session.

&FS
    Cette fonction n'existe pas sous forme interactive

&NFS
&SY2
    $CsvAxes texte
&TX
    o�
&CO
    texte est un string quelconque.
&TX


&EX
Voir $CsvSave
&TX
>







