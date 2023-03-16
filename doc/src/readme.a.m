
START

#include "macros.h"

..esc ~


<Version 6.64> (30/06/2022)>
    Version 6.64 (30/06/2022)
    —————————————————————————
..esc ~


&TI Version IODE pour python
————————————————————————————

Dans le module iode de python (iode.pyd), la fonction python
&CO
    iode.report(filename)
&TX

est corrigée. Elle se comporte comme son équivalent dans les rapport :

&CO
    $ReportExec filename
&TX


&TI DataListSort
————————————————
Cette fonction était erronée depuis la version 6.63: le tri ne se faisait pas correctement.
Ce bug est corrigé.

Par ailleurs, une liste définie avec des ; comme séparateur peut être réordonnancée par
cette fonction, ce qui' n'était pas le cas auparavant.


&TI Reports
———————————

&IT $foreach index
————————————
Si un $define portant le même nom que l'index d'une boucle $foreach existant
avant d'entrer dans une boucle, ce define n'était pas restauré à la fin de
la boucle. Il était erronément remplacé à la fin de la boucle par la liste
de valeurs passée comme argument.

Dorénavant, l'index d'une boucle $foreach est restauré après la boucle si il
était défini avant l'appel.

&EX

$define A XYZ
A is %A%

$foreach A 1 2 3
    A is %A%
$next A

A is %A%
&TX
Affiche :

&CO
A is XYZ
    A is 1
    A is 2
    A is 3
A is XYZ
&TX

&IT Oem to Ansi conversion
———————————
Implementation of $SysAnsiToOem and $SysOemToAnsi.


&IT Parsing of @function()
————————————————————————————
Les noms reconnus des @functions() peuvent dorénavant contenir des chiffres
ou le caractère '_'.

De plus, lorsque le nom d'une @function n'est pas reconnu, le texte est
conservé inchangé dans le résultat du rapport.


&IT @function() avec des paramètres entre quotes
————————————————————————————

Le second argument d'expressions avec 2 paramètres entre quotes comme ~c@fn("abc","def")~C
était ignorés. Cette erreur est corrigée.


&IT Parsing of {@function()}
————————————————————————————
As of now, when an expression between accolades contains @-functions, these
 @-functions are calculed and replaced by their value before the evaluation of {expression}.

&EX
    $define B 1
    $show Value of {1 + %@upper(b)%}   ->> {1 + %B%}  ->> {1 + 1}  ->> 2
&TX



&IT New $multiline command
——————————————————————————
Par défaut, une ligne de rapport peut dorénavant se prolonger sur la ligne suivante si
elle se termine par un espace suivi d'un backslash ~c(\)~C.
Si c'est la cas,
le caractère de retour à la ligne est inséré entre les lignes ~c(\n)~C.

Cela permet par exemple de construire dans un rapport des équations qui se décomposent
sur plusieurs lignes, rendant leur lecture plus simple.

Pour ne pas permettre cette extension (compatilibilié avec les versions
antérieures) il suffit de placer la commande ~c$multiline 0~C avant les
lignes concernées. Sans argument, le multi-lignes est accepté.

&CO
   $multiline [{Nn0}]
&TX

Par exemple, on peut dorénavant écrire :

&CO
    $multiline 1
    $DataUpdateEqs A A := c1 + \
    c2 * t
&TX

La définition de l'équation A sera alors sur deux lignes.

A l'inverse, sans le multiline, une erreur est générée. Ainsi

&CO
    $multiline 0
    $DataUpdateEqs A A := c1 + \
    c2 * t
&TX

essaie de créer une équation terminée par ~c"+ \"~C, ce qui donne le résultat suivant :

&CO
    essais.rep[23] - $multiline 0
    essais.rep[24] - $DataUpdateEqs A A := c1 + \
    (string)[7]:syntax error
    Error : dataupdateeqs A A := c1 + \
&TX

&IT New $noparsing command
——————————————————————————
Par defaut, les textes entre backquotes (`) ne sont pas parsés dans les rapports.
La commande $noparsing permet de modifier ce comportement.

&CO
    $noparsing 0  indique que le parsing doit toujours avoir lieu
    $noparsing 1 indique que le parsing ne doit avoir lieu entre les backquotes (`).
&TX

&EX
     $define C  1
     $noparsing 1
     Example : `{1 + %@upper(c)%}` ->> {1 + %@upper(c)%}
&TX

conserve le texte entre backquotes et parse le reste de la ligne:

&CO
     Example : {1 + %@upper(c)%} ->> 2
&TX


&IT New $undef command
——————————————————————————
Les macros peuvent etre supprimée par la commande $undef ou $undefine.

&CO
    $undef macro_name
    $undefine macro_name
&TX

&EX
    $define toto AAA
    toto = '%toto%'         => toto = 'AAA'
    $define toto
    toto = '%toto%'         => toto = ''
    $undef toto
    toto = '%toto%'         => toto = '%toto%'
&TX

&IT New $shift_args command
———————————————————————————
Alias of ~c$shift~C.

&IT iodecmd: $ask command
———————————————————————————
&CO
    $ask label question
&TX

La commande $ask fonctionne dorénavant dans l'environnement iodecmd. La question est affichée
à l'écran et une réponse est attendue: si un des caractères "OoJjYy1" est entré, la réponse
est positive et le rapport se poursuit au label indiqué.


&IT Noms des fichiers ascii après lecture dans IODE
———————————————————————————————————————————————————
Lors de la lecture de fichiers ascii (~c"test.av"~C par exemple), le nom du fichier
source était remplacé par ~c"ws"~C dans le WS concerné.
Par exemple, si on lit le fichier ~c"test.av",~C le nom du WS est ~c"ws.var"~C.

Dorénavant, lorsqu'un fichier ascii est lu dans IODE, par exemple ~c"test.av",~C 3 cas sont
distingués :
&EN si le fichier existe, le nom du WS conserve le nom avec l'extension ascii (ici ~c"test.av"~C)

&EN si le fichier n'existe pas mais qu'un fichier avec le même nom mais avec l'extension
normale (ici ~c".var"~C) existe, c'est ce fichier ~c"test.var"~C qui est lu et le nom
du ws devient celui du fichier ~c".var"~C

&EN si le fichier n'existe pas et qu'il n'existe pas de fichier ~c".var"~C de même nom,
le nom du ws (vide dans ce cas) devient ~c"ws.var"~C, comme dans les versions
antérieures.


>

<Version 6.63> (30/09/2020)>
    Version 6.63 (30/09/2020)
    —————————————————————————
..esc ~


&TI Authentification des logiciels
——————————————————————————————————
Les programmes ~ciode.exe~C, ~ciodecmd.exe~C et ~ciodecom.exe~C ont été
adaptés afin d'éviter d'être bloqués par McAfee Active Threat Protection
(découpe des exécutables en deux modules : programme et data).

Par ailleurs les programmes sont dorénavant signés avec le certificat du BFP
et donc authentifiés comme tels.


&TI Manuel
——————————
La version Winhelp du manuel n'est plus distribuée car non supportée par les
dernières versions de Windows. Seules les versions locale (HtmlHelp) et Web
(iode.plan.be) sont encore disponibles.



>

<Version 6.62> (29/05/2020)
    Version 6.62 (29/05/2020)
    —————————————————————————
..esc ~


&TI Check Version
—————————————————
La fonction de vérification de version (cfr iode.ini =>> ~c[General] CheckVersion~C) est
désactivée en raison d'un problème de compatibilité avec la nouvelle version
du site iode.plan.be.
>


<Version 6.61> (15/05/2019)
    Version 6.61 (16/05/2019)
    —————————————————————————
..esc ~


&TI Hodrick-Prescott filter
———————————————————————————
La fonction de calcul de trend basée sur le "filtre Hodrick-Prescott"
transforme les séries à traiter en logarithme avant d'effectuer le calcul
d'optimisation proprement dit. Une fois calculées, les séries contenant le
trend sont transformées à nouveau en passant à l'exponentielle.

Ceci a deux conséquences :

&EN seules les parties strictement positives des séries peuvent être traitées
&EN une série purement linéaire, qui ne présente donc aucune déviation par
rapport au trend, devrait être identique après filtrage. Ce n'est plus le cas
si on lui applique une transformation du type log, même si on repasse
ensuite à l'exponentielle.

Une nouvelle fonction plus standard a donc été ajoutée à IODE. Cette fonction
est plus en ligne avec les résultats obtenus avec les logiciels standard comme stata.


&IT Nouvelle fonction de rapport
————————————————————————————————

&CO
    $WsTrendStd filename lambda series1 series1...
&TX

&IT Nouvel opérateur LEC
————————————————————————
&CO
    hpstd([[from,]to],expr)  : calcul sans passage au logarithme de ~cexpr~C
&TX

Les paramètres sont :
&EN from : période de départ du calcul, première année par défaut
&EN to : période de fin de calcul, dernière année par défaut
&EN expr : expression LEC quelconque. Doit être strictement positif pour ~chp()~C

&IT Nouvel écran
————————————————
L'écran Workspace / Trend Hodrick-Prescott filter a été adapté en conséquence en proposant
le choix entre le passage au logarithme ou non.

&TI Taux de croissance moyens
—————————————————————————————
Correction d'un bug qui s'est glissé dans la version 6.58 dans le calcul des
taux de croissance moyens. Ce bug rendait en général les taux de croissance
moyens beaucoup plus élevés que ce qu'ils devaient être.

>

<Version 6.60> (11/04/2019)
    Version 6.60 (11/04/2019)
    —————————————————————————
..esc ~

&TI Rapports : nouvelles fonctions
——————————————————————————————————

Des fonctions de rapport ont été ajoutées :
&EN @mkdir : créer un répertoire
&EN @rmdir : supprimer un répertoire
&EN @getdir : afficher le repertoire courant
&EN @chdir : changer de répertoire courant
&EN @void : supprimer l'output d'une fonction

Par ailleurs les commandes qui changent le directory courant affichent le
nouveau directory dans la barre de titre de la fenêtre.

&IT Exemple
———————————
&CO
  ## Utilisation des commandes de gestion de dirs
  ## --------------------------------------------
  ##
  ## Syntax :
  ##
  $debug 1
  $msg Current dir : @getdir()
  $msg Make dir    : @mkdir(subdir)
  $msg Chdir       : @chdir(subdir)
  $msg Chdir       : @chdir(..)
&TX

Résultat
————————

&CO
  Current dir : c:\usr\iode
  Make dir    :
  Chdir       : c:\usr\iode\subdir
  Chdir       : c:\usr\iode
&TX


&TI Correctif : save WS / CSV format
————————————————————————————————————
Les fichiers de variables peuvent  à nouveau être sauvés en format csv via le menu
"Workspace / Save Workspace" ou via la commande $WsSaveVar dans un rapport.

&CO
## Load a IODE var file and save vars in CSV format
$WsLoadVar ..\dos\test.var
$WsSaveVar test.csv
&TX

&TI Filtre Hodrick-Prescott
———————————————————————————
La description de la méthode a été ajoutée au manuel de IODE dans
la partie "Le programme IODE / Workspace / Trend Correction".


>

<Version 6.59> (13/03/2019)
    Version 6.59 (13/03/2019)
    —————————————————————————
..esc ~

&TI Rapports : nouvelles fonctions $ModelSimulateSave*
——————————————————————————————————————————————————————


Le nombre d'itérations et le seuil de convergence atteint lors de la dernière simulation
peuvent être sauvés dans des ~uvariables~U IODE. Deux nouvelles fonctions de rapport
ont été créées à cette fin :


&EN $ModelSimulateSaveNiters ~ivar_name~I
&EN $ModelSimulateSaveNorms  ~ivar_name~I

Pour rappel, les fonctions de rapport suivantes permettent d'obtenir les mêmes informations :


&EN ~c@SimNiter(period)~C
&EN ~c@SimNorm(period)~C

&IT $ModelSimulateSaveNiters
———————————————————————

Cette commande permet de sauver dans une variable le nombre d'itérations
qui ont été nécessaires pour chaque période lors de la dernière simulation.

Le nom de la variable résultat est passé comme paramètre.

S'il n'y a pas encore eu de simulation dans le cours de la session, la variable est
créée avec des valeurs NA.

Si une simulation a déjà eu lieu, les valeurs des périodes non simulées sont fixées à 0 et
celles des périodes simulées contiennent le nombre d'itérations nécessaires à
la convergence pour cette période. S'il n'y a pas de convergence, la valeur est celle du
paramètre MAXIT de la simulation.


Syntaxe :
&CO
    $ModelSimulateSaveNiters varname
&TX

Exemple :
&CO
    $ModelSimulateSaveNiters SIM_NITERS
&TX

&IT $ModelSimulateSaveNorms
———————————————————————

Cette commande permet de sauver dans une variable le seuil de convergence (la norme) atteint
pour chaque période lors de la dernière simulation.

Le nom de la variable résultat est passé comme paramètre.

S'il n'y a pas encore eu de simulation dans le cours de la session, la variable est
créée avec des valeurs NA.

Si une simulation a déjà eu lieu, les valeurs des périodes non simulées sont fixées à 0 et
celles des périodes simulées contiennent le seuil de convergence
pour cette période.

Syntaxe :
&CO
    $ModelSimulateSaveNorms varname
&TX
Exemple :
&CO
    $ModelSimulateSaveNiters SIM_NORMS
&TX


&TI Filtre Hodrick-Prescott
——————————————————————————
La valeur du paramètre lambda peut dorénavant être un nombre réel. Auparavant,
cette valeur était un nombre entier.

>

<Version 6.58> (20/01/2019)
    Version 6.58 (20/01/2019)
    —————————————————————————
..esc ~

&TI Rapports : $DataCreateGraph
———————————————————————————————
Correction : l'ordre des paramètres de définition des grids est corrigé pour
correspondre au manuel.


&TI Comment exécuter un rapport IODE
————————————————————————————————————
Il est possible d'exécuter un rapport IODE (soit ~cmyreport.rep~C) de
plusieurs façons. En plus de la méthode habituelle à travers l'interface de IODE,
on peut lancer un rapport IODE de 3 façons :

&EN en lançant le programme ~ciode~C suivi du nom du fichier rapport dans un "Command Prompt"
&EN en lançant le programme ~ciodecmd~C suivi du nom du fichier rapport dans un "Command Prompt"
&EN dans l'explorateur Windows en double-cliquant sur le fichier .rep

Pour illustrer cela, voici un exemple de rapport.

&IT LoadWs.rep
——————————————
Le rapport suivant charge 5 WS en mémoire.

&CO
$define file c:\iode\example\fun

$WsLoadVar %file%
$WsLoadEqs %file%
$WsLoadScl %file%
$WsLoadTbl %file%
$WsLoadCmt %file%
&TX

&IT Exécuter un rapport dans un "Command Prompt" avec le programme ~ciode~C
———————————————————————————————————————————————————————————————————————

A partir de la version 6.58, il suffit de lancer la commande iode en lui
passant le nom du rapport comme paramètre. Par exemple :

&CO
  c:\iode>> iode LoadWs.rep
&TX

Iode est alors lancé et LoadWs.rep est exécuté ~udans l'interface de IODE~U.
Si le rapport se termine par ~c$quitode~C, IODE est quitté à la fin du
rapport.

A noter que le directory où a été installé iode doit se trouver dans le PATH pour
que cela fonctionne. Sans cela, Windows ne pourrait savoir où trouver le
programme...

Pour changer le PATH,
&EN 1. aller dans ~cControl Panel\All Control Panel Items\System~C
&EN 2. sélectionnez "Advanced system Settings"
&EN 3. sélectionnez l'onglet "Advanced"
&EN 4. sélectionnez "Environment Variables"
&EN 5. allez aux "System variables"
&EN 6. double-cliquez sur la variable "Path"
&EN 7. ajoutez le directory d'installation de IODE (si cela n'a pas encore été fait)
&EN 8. Cliquez OK (3x) pour confirmer

Le prochain lancement du "Command Prompt", la nouvelle valeur de la variable
~cPath~C sera connue.

&IT Exécuter un rapport dans un "Command Prompt" avec le programme ~ciodecmd~C
———————————————————————————————————————————————————————————————————————————
Le programme iodecmd permet d'exécuter un rapport IODE sans
interface utilisateur. Une fois le rapport terminé, iodecmd rend la main.

Par défaut, le résultat de chaque commande est affiché à l'écran :
&CO
  c:\iode>> iodecmd LoadWs.rep

Loading example\fun
394 objects loaded
Loading example\fun
274 objects loaded
Loading example\fun
161 objects loaded
Loading example\fun
46 objects loaded
Loading example\fun
317 objects loaded
&TX

&IT Lancement d'un rapport dans l'explorateur Windows
—————————————————————————————————————————————————————
Il est possible de lancer un rapport dans l'explorateur Windows en
double-cliquant sur le fichier. Condition préalable:il faut associer les fichiers
~c*.rep~C avec le programme IODE. Suite aux restrictions apportées par Microsoft,
cette association (comme les autres) ne se fait plus automatiquement.

A noter que cela ne peut fonctionner qu'à partir de la version 6.58.

>

<Version 6.57> (02/03/2018)
    Version 6.57 (02/03/2018)
    —————————————————————————
..esc ~


&TI E-Views
———————————
Nouvelles directives dans les fichiers d'équations EVIEWS. Les directives
doivent se trouver seules sur une ligne. Elles peuvent être en majuscules, minuscules ou
un mélange des deux.

Elles déterminent la façon dont les termes des équations ou identités EVIEWS
du type ~cname(1)~C doivent être traduites en IODE.

&CO
  KEEP COEFS:
  DROP COEFS:
&TX

&EN Après KEEP COEFS:, la traduction de ~cname(1)~C ou ~cNAME(1)~C donne ~cname_1~C.
&EN Après DROP COEFS:, la traduction de ~cname(1)~C ou ~cNAME(1)~C donne ~cname~C.

Au début de la lecture fichier, ~cDROP COEFS:~C est la valeur initiale.

&IT Exemple de fichier EVIEWS
—————————————————————————————

&CO
Identities:
===========
KEEP COEFS:

B_QHOA  = b_1(2)  * b_c0122222
B_QHOB  = b_qh_b_14(1)  * B_CO14

DROP COEFS:

B_QHOCADD  = (b_qh_c_1(1)  * B_CO1)  + (b_qh_c_2(1)  * B_CO2)  + (b_qh_c_7(1)  * B_CO7)
B_QHOCR_NC  = b_qh_cr_14(1)  * B_CO14
&TX

&IT Résultat en IODE
————————————————————
&CO
B_QHOA := b_1_2*B_C0122222
B_QHOB := b_qh_b_14_1*B_CO14
B_QHOCADD := (b_qh_c_1*B_CO1)+(b_qh_c_2*B_CO2)+(b_qh_c_7*B_CO7)
B_QHOCR_NC := b_qh_cr_14*B_CO14
&TX

>
<Version 6.56> (27/01/2017)
    Version 6.56 (27/01/2017)
    —————————————————————————
..esc ~


&TI Sauvetage des workspaces (fix)
——————————————————————————
Dans l'écran de sauvetage des workspaces, les ws pour lesquels aucune extension n'est
indiquée sont à nouveau sauvés en format IODE (~c.var~C, ~c.cmt~C , ...). Dans la version
précédente, aucun sauvetage n'était effectué.

Si une extension de format ascii est indiquée, le ws est sauvé en format ascii.

Les commandes de rapport ~c$WsSave*~C opèrent de la même manière.


&TI Génération de fichiers csv
——————————————————————————————

&IT A partir des rapports ($WsSaveCsvVar, $csvsep et $csvdec)
—————————————————————————————————————————————————————————————

Les paramètres de ces commandes (séparateur de liste et caractère décimal)
sont initialisés par les valeurs des "International Settings" définies dans
Windows. De cette façon, les fichiers csv générés sont directement lisibles
en Excel sans avoir à se préoccuper des séparateurs et autres caractères
décimaux.

&IT A partir de l'écran Save Workspace
——————————————————————————————————————————————————————
Dans l'écran "Workspace/Save Workspace",
si l'extension du fichier est ~c.csv~C, le fichier est sauvé en format CSV.
Les paramètres (séparateur et caractère décimal) sont fixés par les
commandes ~c$csvsep~C et ~c$csvdec~C ou, à défaut, par les valeurs des
locales Windows. Les fichiers peuvent donc directement être ouverts en Excel.

&TI Copy/Paste de variables entre Excel et IODE
———————————————————————————————————————————————
&NO Cette nouvelle fonction est encore en phase de test.

La copie de valeurs à partir d'Excel vers l'éditeur de variables de IODE est
amélioré à plusieurs égards :

&EN les virgules décimales sont traitées correctement
&EN les signes de devise (currency) sont ignorés dans les copies vers IODE
&EN les cellules vides sont interprétées comme -- (na)

Attention cependant à ce que le séparateur entre les valeurs doit être la
tabulation. Les blancs sont ignorés.

Si la copie provient
d'un autre logiciel, il faut s'assurer que c'est bien le TAB qui sépare les
valeurs.

&NO Cette fonction utilise les "locale" de Windows pour déterminer les
séparateur de milliers et de décimales. Comme le séparateur de milliers
est parfois le point (1.234,5 par exemple), il faut être attentif que 1.23 est en fait
123 car le signe des milliers est ignoré.

>

<Version 6.55> (05/01/2017)
    Version 6.55 (05/01/2017)
    —————————————————————————
..esc ~


&TI Ouverture d'anciens fichiers
————————————————————————————————
Correction d'un bug lors de l'ouverture des fichiers de version antérieure
(magic number 504d).


&TI Interprétation des macros
————————————————————————————

Lorsqu'une macro (string entre %) n'est pas trouvée, le texte de la macro
est conservé tel quel, % inclus.

Pour rappel, voici un rapport qui indique comment se comportent les macros :


&EX
## IODE : Macro examples
## ---------------------
$msg -----------------------------------------------
$msg Undefined Macros - Normal, uppercase, lowercase
$msg -----------------------------------------------
$msg Normal text   : %bla bla bla%
$msg Uppercase     : %##bla bla bla%
$msg Lowercase     : %!bla bla bla%
$msg ------------------------------------------------------------
$msg Macro modifications : normal, squeezed, uppercase, lowercase
$msg ------------------------------------------------------------
$define macro1 Variable name
$msg %%macro1%%  %macro1%
$msg %%-macro1%% %-macro1%
$msg %%!macro1%% %!macro1%
$msg %%##macro1%% %##macro1%
&TX

Le résultat de ce rapport est :
&CO
-----------------------------------------------
Undefined Macros - Normal, uppercase, lowercase
-----------------------------------------------
Normal text   : %bla bla bla%
Uppercase     : %BLA BLA BLA%
Lowercase     : %bla bla bla%
------------------------------------------------------------
Macro modifications : normal, squeezed, uppercase, lowercase
------------------------------------------------------------
%macro1%  Variable name
%-macro1% Variablename
%!macro1% variable name
%#macro1% VARIABLE NAME
&TX

&TI Orthographe
—————————
Correction d'une erreur d'orthographe en français dans les textes automatiques des
tableaux/graphiques.

&TI iode -fontsize
——————————————————
Le paramètre ~c-fontsize nn~C est prioritaire sur la définition
~c[FONT] FONTSIZE=nn~C dans le fichier ~ciode.ini~C.
>

<Version 6.54> (22/08/2016)
    Version 6.54 (22/08/2016)
    —————————————————————————
..esc ~


&TI Duplex and page orientation printing options
————————————————————————————————————————————————

Pour éviter les problèmes liés aux mises à jour des drivers des imprimantes Windows,
les options d'impression en recto/verso et en portait/landscape ont été annulées:
ces options ne sont plus affichées dans l'écran "Print Setup/Options/Printer" et ne
sont plus actives dans les commandes des rapports.

Pour éviter les problèmes de compatibilités avec des rapports existants, les fonctions

&EN  $PrintOrientation
&EN  $PrintDuplex

restent valides, mais n'ont plus d'effet sur la configuration des imprimantes.

>

<Version 6.53> (18/07/2016)
    Version 6.53 (18/07/2016)
    —————————————————————————
..esc ~


&TI E-Views to IODE
——————————————————
&EN L'importation des coefficients est à nouveau opérationnelle. Cette option
avait été supprimée dans la version précédente.

&EN Les lignes commençant par ~c@INNOV~C sont ignorées.

Pour rappel, la méthode d'importation fonctionne comme suit :

&IT Lignes "Forecasting Equation:" et "Substituted coefficients:"
————————————————————————————————————————————————————
Lorsqu'une ligne ~cForecasting Equation~C est rencontrée, l'équation qui la
suit (après avoir sauté une ligne) est lue et conservée en mémoire jusqu' à la ligne
 ~cSubstituted Coefficients:~C suivante. Cette dernière est alors analysée et les
valeurs des coefficients en sont extraites et placées dans dans scalaires
IODE. Ces scalaires portent le nom de l'endogène de l'équation avec le
suffixe ~c_1~C, ~c_2~C, ...

L'équation initiale est transformée pour remplacer les coefficients ~cC(1)~C... par
les noms des scalaires ~cendo_1~C, ... et elle est finalement sauvée dans IODE.

&IT Lignes "~cEstimation Command:~C" et "~cEstimation Equation:~C"
—————————————————————————————————————————————————————————————————————————————
Ces lignes sont ignorées.


&IT Lignes après "~cIDENTITIES:~C"
————————————————————————————

Les lignes contenant des identités sont interprétées comme équations à partir du moment où :

&EN soit une ligne contenant le texte ~cIdentities:~C est trouvée
&EN soit une première identité préfixée par ~c@IDENTITY~C est trouvée

A partir de ce moment toutes les lignes contenant du texte sont interprétées par
le programme comme des équations IODE.

En cas d'erreur de syntaxe, la lecture s'arrête et un message d'erreur est produit.
>

<Version 6.52> (21/06/2016)
    Version 6.52 (21/06/2016)
    —————————————————————————
..esc ~


&TI E-Views to IODE
——————————————————
La syntaxe des fichiers de transfert E-Views to Iode est légèrement modifiée pour
permettre une plus grande souplesse : les lignes contenant des identités ne
sont plus interprétées qu'à partir du moment où :

&EN soit une ligne contenant le texte ~cIdentities:~C est trouvée
&EN soit une première identité préfixée par ~c@IDENTITY~C est trouvée

A partir de ce moment toutes les lignes contenant du texte sont interprétées par
le programme comme des équations IODE.

En cas d'erreur de syntaxe, la lecture s'arrête et un message d'erreur est produit.


&TI Corrections
———————————————
&EN la lecture et l'écriture de fichiers de profils sont corrigées


&TI IodeCom
———————————
La version précédente ne fonctionnait plus correctement sur certaines machines en raison
de l'absence d'une dll dans la distribution. Ce problème est corrigé.

&TI IodeCmd
———————————
Les messages d'erreurs précis sont à présent affichés dans ~ciodecmd~C. Auparavant
seul le message ~c"Error ... Msg##nn"~C était affiché.

>

<Version 6.51> (04/04/2016)
    Version 6.51 (04/04/2016)
    —————————————————————————
..esc ~


&TI Fonctions de rapport : format LArray
————————————————————————————————————————
De nouvelles fonctions de rapport permettant le sauvetage de séries dans le
format csv adapté au package Python LArray ont été créées :

&EN $WsSaveCsvVar filename [from to] [vars]
&EN $CsvSep
&EN $CsvDec
&EN $CsvDigits (changer)

&IT Exemple
———————————
Le rapport suivant fixe d'abord les paramètres :
&EN le séparateur de cellules (défaut ,)
&EN le nombre de chiffres significatifs (défaut 10)
&EN le caractère décimal (défaut .)
&EN le texte pour les valeurs NaN (défaut vide)

Il lance crée ensuite 4 fichiers selon des modes différents :
&EN toutes les variables sur ou tout le sample
&EN toutes les variables sur la période 2003Y1 2010Y1
&EN les variables commençant par ~cAC~C sur la période 2009Y1 2010Y1
&EN les variables commençant par ~cAC~C ou ~cB~C sur tout le sample du WS

&CO
$wsloadvar ..\fun\fun
$csvsep ;
$csvdigits 10
$csvdec .
$csvnan nan

$WsCsvSaveVar fun
$WsCsvSaveVar fun1 2003y1 2010y1

$csvdigits 5
$WsCsvSaveVar fun2 2009y1 2010y1 ACAF ACAG
$WsCsvSaveVar fun3 AC* B*


&TX

Les fichiers résultant de ce rapports sont :

&TI fun.csv
———————————
&CO
 var\time;1960;1961;1962;1963;1964;1965;1966;1967;1968;1969;1970;...
 ACAF;nan;nan;nan;nan;nan;nan;nan;nan;nan;nan;1.2130001;5.2020001;9.184;...
 ACAG;nan;nan;nan;nan;nan;nan;nan;nan;nan;nan;-11.028999;-15.847;-19.288002;...
 ...
&TX

&TI fun1.csv
———————————
&CO
 var\time;2003;2004;2005;2006;2007;2008;2009;2010
 ACAF;nan;nan;nan;nan;nan;nan;nan;nan
 ACAG;nan;nan;nan;nan;nan;nan;nan;nan
 AOUC;nan;0.2478319161;0.2545676576;0.2637957322;0.2762428533;0.2858059161;...
 ...
&TX

&TI fun2.csv
———————————
&CO
 var\time;2009;2010
 ACAF;nan;nan
 ACAG;nan;nan
 ...
&TX

&TI fun3.csv
———————————
&CO
var\time;1960;1961;1962;1963;1964;1965;1966;1967;1968;1969;1970;1971;...
ACAF;nan;nan;nan;nan;nan;nan;nan;nan;nan;nan;1.213;5.202;9.184;8.079;...
ACAG;nan;nan;nan;nan;nan;nan;nan;nan;nan;nan;-11.029;-15.847;-19.288;...
BENEF;11.665;13.607;12.208;14.795;18.797;20.86;17.28;20.49;26.935;32.601;...
BQY;31.777;30.853;30.353;27.37;26.937;35.435;37.148;39.506;42.183;43.18;...
BRUGP;nan;nan;nan;nan;nan;nan;nan;nan;nan;nan;0;0;0;0;0;0;0.268;6.137;...
BVY;7.2;7.1;7.1;6.6;6.8;9.4;10.3;11.3;12.4;13.3;17.2;17;19.5;19.2;22.3;...
 ...
&TX

&TI Correction DDE
——————————————————
Extension des buffers pour la copie vers Excel en DDE.

>

<Version 6.50> (25/01/2016)
    Version 6.50 (25/01/2016)
    —————————————————————————
..esc ~


&TI Adaptations techniques
————————————————————


&EN Planning Corner : REAL ->> IODE_REAL
&EN Compilateur BCC32 7.10 Embarcadero
>

<Version 6.49> (19/01/2016)
    Version 6.49 (19/01/2016)
    —————————————————————————
..esc ~


&TI WsImportEViews
————————————————————

Cette fonction de rapport a été étendue :

&EN Support des fonctions LOG, DLOG, EXP
&EN Fonction ~c@TREND~C traduite en ~ct~C
&EN La fin des lignes à partir d'une quote (caractère  ') est ignorée
&EN Support des lignes sans préfixe @IDENTITY
&EN Support des coefficients VARNAME(n) ->> varname_1
&EN Les expressions C(n) ne sont plus traitées séparément, mais comme n'importe quelle
autre expression VARNAME(n).
&EN le séparateur entre membre de gauche et de droite de l'équation est le premier
signe = rencontré s'il n'est pas entre parenthèses. Par exemple :

&CO
  A + (TIME=2015) = X + Y
&TX
devient bien :

&CO
  A + (TIME=2015) := X + Y
&TX
>


<Version 6.48> (12/10/2015)
    Version 6.48 (12/10/2015)
    —————————————————————————
..esc ~


&TI Check Last Version
————————————————————

Correctif : la vérification de la dernière version peut être supprimée via
le fichier iode.ini (voir 6.47), paramètre ~c[General] CheckVersion=0~C.

Cependant, l'accès au site ~ciode.plan.be~C était quand-même effectué, même
si ce paramètre était fixé à 0, ce qui bloquait quelques secondes le
démarrage de IODE en cas de blocage du site.

>

<Version 6.47> (06/10/2015)
    Version 6.47 (06/10/2015)
    —————————————————————————
..esc ~


&TI Check Last Version
——————————————————————
La dernière version du logiciel se trouve sur le site ~ciode.plan.be~C. A
chaque démarrage de IODE, la version courante est comparée avec la dernière
version disponible sur le serveur. Si une nouvelle version est disponible,
l'utilisateur est prévenu via un message et via le titre de la fenêtre.

Ce check, et le message qui l'accompagne éventuellement, peuvent être supprimés via le
paramètre ~cGeneral/CheckVersion~C du fichier ~ciode.ini~C.



&TI IODE.INI
————————————
De nouveaux paramètres sont définis :

&CO
[General]
Banner=0
CheckVersion=0
&TX
&EN ~cBanner~C permet de supprimer l'affichage du banner au démarrage de IODE
&EN ~cCheckVersion~C permet de supprimer la recherche de nouvelle version sur le
site ~ciode.plan.be~C et dons de ne plus afficher de message si la version
courante n'est plus la dernière.


&TI Correctifs
————————————————————
Corrections de bugs :
&EN impression de fichiers A2M
&EN édition de tableaux de comparaison de fichiers
&EN affichage des noms de fichiers dans des graphiques

>

<Version 6.46> (16/09/2015)
    Version 6.46 (16/09/2015)
    —————————————————————————
..esc ~


&TI Fichier iode.ini
————————————————————

Un nouveau fichier de paramètres est ajouté à IODE : ~ciode.ini~C.
Ce fichier est lu au démarrage de IODE.


&IT Localisation
————————————————
Le fichier ~ciode.ini~C se trouve par défaut dans le
répertoire d'installation de IODE (par défault ~cc:\iode~C). On peut
spécifier un autre fichier en utilisant le nouveau paramètre ~c-iodeini~C au
lancement de IODE.

Par exemple :
&CO
   c:\usr\mymodel>> iode -iodeini mycolors.ini
&TX


&IT Syntaxe du fichier iode.ini
——————————————————————
La syntaxe du fichier est proche de celle des fichiers ~c.ini~C  :
&EN chaque section débute par un nom entre []. Par exemple :
~c[Fonts]~C.
&EN dans chaque section, les paramètres sont indiqués sous la forme
~cparam=values~C ou ~cparam~C si une valeur ne doit pas être fournie.
&EN Un ligne qui commence par un point-virgule est ignorée

&IT Contenu de iode.ini
———————————————————————

Ce fichier contient actuellement les informations suivantes, mais sera
étendu dans le futur :

Section ~c[Screen]~C
————————————————
&EN Nl : nombre de lignes de la fenêtre Iode (en caractères)
&EN Nc : nombre de colonnes de la fenêtre Iode (en caractères)

&CO
[SCREEN]
NL=35
NC=90
&TX


Section ~c[Font]~C
——————————————————
&EN2 ~cFontName~C : nom
&EN2 ~cFontSize~C : taille (en points)
&EN2 ~cFontSizeMin~C : taille minimum
&EN2 ~cFontSizeMax~C : taille maximum
&EN2 ~cFontSizeIncr~C : incrément en point de la taille des caractères

&CO
[FONT]
;FontName=Terminal
FontName=Courier new
FontSize=13
FontSizeMax=30
FontSizeMin=6
FontSizeIncr=1
&TX


Sections ~i[palette_name]~I
———————————————————————————————

Ces sections contiennent la description RGB des couleurs utilisées dans l'écran.
Seule la palette [Palette_Default] est utilisée par Iode. Si elle est
présente, elle remplace les valeurs par défaut de Iode.

La logique est la suivante :
&EN les trois premières valeurs représentent le code RGB (Red, Green, Blue)
de la couleur de fond (background)

&EN les trois dernières valeurs représentent le code RGB (Red, Green, Blue)
du caractère (foreground).

Ainsi 0 0 0 représente le noir et 255 255 255 représente le blanc. Toute combinaison
permet de créer une palette complète de couleurs.

Pour simplifier l'écriture, on peut utiliser les mots suivants dans le texte :
&EN BLACK    ==    0,   0,   0
&EN WHITE    ==  255, 255, 255
&EN RED      ==  255,   0,   0
&EN GREEN    ==    0, 192,   0
&EN BLUE     ==    0,   0, 191
&EN YELLOW   ==  255, 255,   0
&EN CYAN     ==    0, 255, 255
&EN MAGENTA  ==  255,   0, 255
&EN PINK     ==  127,   0, 127
&EN GREY     ==  127, 127, 127
&EN ROSE     ==  255, 127,   0
&EN VBLUE    ==   15,  15, 255


&CO
[Palette_Default]
Default     =     0,   0,   0,      0, 255, 255
Reverse     =   191, 191, 191,      0,   0,   0
Underline   =   255, 255,   0,    255,   0,   0
Bold        =     0,   0,   0,    255, 255,   0
Blinking    =   255, 255, 255,      0,   0,   0
Dim         =    31,  31,  31,      0,   0,   0
Black       =     0,   0, 192,    255, 255, 255
Blue        =     0,   0, 255,    255, 255, 255
Green       =     0, 255,   0,    255, 255, 255
Cyan        =     0, 255, 255,      0,   0,   0
Red         =   255,   0,   0,    255, 255, 255
Magenta     =   127,   0, 127,      0,   0,   0
White       =   255, 255, 255,      0,   0,   0
Yellow      =   255,   0,   0,    255, 255,   0
Shadow      =    63,   63,  63,     0,   0,   0
Chelp1      =   255, 127,   0,      0,   0,   0
Chelp2      =   191, 191, 191,    127, 127, 127

&TX

&TI Correction de bugs
—————————————————————
&EN Fonctions corrigées :
&EN2 ~c$DataCompare~C : les comparaisons d'équations et d'identités
    donnaient comme différentes des équations/identités identiques.

&EN2 ~c##FileRename, ##FileCopy, ##WsHtolLast, ##WsLtoHFlow, ##WsLtoHStock,
##WsSeasonAdj, ##WsTrend~C : iode pouvait stopper en raison de la longueur
des noms de fichiers.

>

<Version 6.45> (07/09/2015)
    Version 6.45 (07/09/2015)
    —————————————————————————
..esc ~

&TI Aide en ligne
—————————————————
Deux formats de fichiers sont dorénavant supportés : les format ~c  .hlp~C et ~c.chm.~C
Il s'agit du même manuel, mais chaque format offre des fonctions spécifiques
de recherche et d'affichage.

On peut passer d'un format à un autre dans le menu ~cHelp~C.

L'accès au site ~ciode.plan.be~C est également ajouté directement dans la menu Help.

&IT Fichier ~ciode.hlp~C
————————————————————————

Il s'agit de l'ancien système WinHelp de Windows. Cette formule présente
l'avantage de pointer directement sur la page du manuel correspondant au
contexte courant. Dans l'édition des rapports par exemple, en cliquant sur F10 avec
le curseur sur un nom de fonction, on accède directement à la page désirée.

Contrainte : à partir de Vista, il faut avoir préalablement
installé le logiciel WinHelp pour pouvoir exploiter ce format. Ce logiciel
est gratuit et est disponible sur le site de Microsoft.

&IT Fichier ~ciode.chm~C
————————————————————————

Ce fichier est d'un format plus récent et présentent
d'autres fonctions de recherche et d'indexation. Il est proposé par
défaut. Ce manuel n'est cependant pas piloté par iode : une fois ouvert, l'utilisateur
doit rechercher la page qui l'intéresse à travers l'interface du programme lui-même.

Dans les deux cas, le fichier d'aide doit se trouver dans même directory que
le programme ~ciode.exe~C.

&TI Correction de bugs
—————————————————————
&EN Fonctions corrigées :
&EN2 ~c$DataCompareXxx~C
&EN2 ~c$DataDuplicate~C
&EN2 ~c$DataListXxx~C

>

<Version 6.44> (10/04/2015)
    Version 6.44 (10/04/2015)
    —————————————————————————
..esc ~

&TI Sauvetage et relecture des WS en Ascii
——————————————————————————————————————————
Le sauvetage en ascii des fichiers de commentaires, d'équations, de listes
et de tableaux prend en compte les caractères ~c"~C et ~c\~C qui peuvent être
présents dans certains textes (titres, commentaires, ...).

Ces caractères sont précédés au moment du sauvetage par un backslash (~c\~C). Cela permet
de relire ensuite sans problème le fichier ascii généré (~c.ac, .ae, .al, .at~C).

&TI Noms de WS dépassant 63 caractères
————————————————————————————————————————————————————————————————————————
Les noms de WS supérieurs à 63 caractères et/ou contenant des espaces sont à
présent gérés par IODE. Cela permet de placer ses fichiers dans n'importe quel directory comme :
&CO
~cC:\Users\My Name\SharePoint\share sgad\cic\IODE\TresLongNomdeDirectoryPourTesterIode~C
&TX

Les fichiers avec des noms longs et/ou des espaces dans leur nom
sont utilisables dans les situations suivantes :

&EN Ouverture à partir de l'explorateur Windows
&EN Lancement à partir d'un shell dos ("Command Prompt")
&EN Rapports : dans les fonctions ~c$WsLoad*~C, ~c$WsSave*~C, etc.

&TI Caractères accentués en HTML
————————————————————————————————
Tous les caractères minuscules accentués sont traduits dans la forme html du type "&...;".

&TI Enregistrement des touches
——————————————————————————————
Il est possible d'enregistrer dans un fichier les séquences de touches
utilisées pendant une session iode. Il faut pour ce faire lancer ~ciode~C
avec le paramètre ~c-rec~C comme ci-dessous :

&CO
    iode -rec filename
&TX

Le fichier (ici ~cfilename~C) contiendra des séquences de touches codifiées
selon la syntaxe décrite dans la version 6.27 de iode. Ce fichier peut être
édité à l'aide d'un éditeur ascii.

Ces touches peuvent ensuite être "rejouées" automatiquement. Cela permet par exemple :
&EN de lancer des opérations répétitives sans avoir à passer par les rapports
&EN de lancer des procédures de test unitaires

Toutes les opérations ne sont cependant pas exploitables : les opérations
effectuées à l'aide de la souris ne sont pas supportées. Il faut également
être attentif au fait que la position dans un menu ou le contenu d'un écran
de saisie peuvent changer entre deux lancements successifs de iode.

&TI Correction de bugs
—————————————————————
Lors de l'édition des tableaux, une erreur pouvait se produire
lors de la destruction de lignes.


>

<Version 6.43> (24/02/2014)
    Version 6.43 (24/02/2014)
    —————————————————————————
..esc ~

&TI IODECMD : arguments
———————————
Le lancement d'un rapport à l'aide de iodecmd peut contenir les arguments du rapport :
&CO
  c:\>> iodecmd -y -v myrep arg1 arg2
&TX

Les paramètres du programme ~ciodecmd~C doivent se trouver avant le nom du rapport sans
quoi ils sont considérés comme des arguments du rapport et passés comme tels au rapport.

&TI Conversion UTF8
———————————————————
De nouvelles fonctions de rapport permettent de transformer des fichiers codes ANSI ou OEM
en UTF-8 directement depuis des rapports.

&CO
    $SysAnsiToUTF8 inputfile outputfile
    $SysOemToUTF8  inputfile outputfile
&TX


&TI Conversion ANSI-OEM
———————————————————
Les fonctions de rapport suivantes convertissent des fichiers codés ANSI en OEM
et réciproquement.

&CO
    $SysAnsiToOem inputfile outputfile
    $SysOemToAnsi inputfile outputfile
&TX

>

<Version 6.42> (07/10/2013)
    Version 6.42 (07/10/2013)
    —————————————————————————
..esc ~

&TI Génération de tableaux HTML
———————————————————————————————
Lors de la génération de tableau en HTML, des classes par défaut sont utilisées pour les tags suivants :
&EN <<TABLE>> : A2mTable
&EN <<TR>> : A2mTR
&EN <<TH>> : A2mTH
&EN <<TD>> : A2mTD

Ces tags peuvent être remplacés par d'autres via 4 commandes dans les rapports :
&CO
  $PrintHtmlTableClass [myTableClass]
  $PrintHtmlTRClass    [myTRClass]
  $PrintHtmlTHClass    [myTHClass]
  $PrintHtmlTDClass    [myTDClass]
&TX

Les noms de classes vides suppriment la référence à la classe dans le tag html.


>

<Version 6.41> (15/07/2013)
    Version 6.41 (15/07/2013)
    —————————————————————————
..esc ~

&TI Gestion Mémoire
———————————————————
Amélioration  de la gestion des "swaps" pour permettre la gestion de plus
de ~bblocs~B mémoire (>>32767).

On a dans la version courante :
&EN 262144 bytes par bloc mémoire  (ou plus pour des objects plus grands)
&EN 65535 blocs mémoire maximum
>

<Version 6.40> (20/02/2013)
    Version 6.40 (20/02/2013)
    —————————————————————————
..esc ~

&TI Rapports
————————

&IT $indent
———————————
Il est dorénavant possible d'indenter les commandes dans les rapports.
La commande ~c$indent~C sans argument indique qu'à partir de ce moment, les
commandes peuvent ne plus être collées à la marge. Avec l'argument N ou n ou
0, le comportement ancien reprend.


Pour éviter les problèmes de compatibilité entre les versions de IODE, la valeur
par défaut est de ne pas accepter les indentations.

&CO
   $indent [{Nn0}]
&TX

Par exemple, on peut écrire :

&CO
$indent

$procdef print list
$------------------
    $foreach i %list%
	$show print : %i%
    $next i
$procend

$procdef print2 list
$-------------------
    $foreach i %list%
	$show print2 : %i%
	$procexec print 1 2 3
    $next i
$procend

$procexec print2 A B C
&TX
Le résultat produit est le suivant  :

&CO
print2 : A
print : 1
print : 2
print : 3
print2 : B
print : 1
print : 2
print : 3
&TX

&IT $procdef, $procend, $procexec
—————————————————————————————————
Les instructions de ce groupe permettent de construire des procédures, c'est à dire des
listes de commandes qui peuvent être réutilisées et paramétrées.

&CO
 $procdef procname [fparm1 ...]
 ..
 $procend
&TX

où
&EN ~bprocname~B est le nom de la procédure (case sensitive).
&EN ~bfparm1~B est le premier paramètre ~iformel~I de la procédure

&IT Appel d'une procédure
—————————————————————————
L'appel se fait simplement par la commande :

&CO
$procexec nom_proc aparm1 ...
&TX

où
&EN ~bprocname~B est le nom de la procédure (case sensitive).
&EN ~baparm1~B est le premier paramètre ~iactuel~I de la procédure

&IT Paramètres
——————————————
Les paramètres formels sont traités dans la procédure comme des defines
(locaux à la procédure) : ils doivent être appelés par ~c%fparm%~C.

Leurs valeurs sont fixées comme suit :

&EN S'il y a moins de paramètres actuels que de paramètres formels ou si
leur nombre est égal, les valeurs des paramètres actuels sont assignées dans
l'ordre aux premiers paramètres formels. Les paramètres formels
excédentaires sont considérés comme vides.

&EN S'il y a plus de paramètres actuels que de paramètres formels, les
paramètres formels, ~bsauf le dernier~B, reçoivent les valeurs de premiers
paramètres actuels, dans l'ordre de leur passage.
~bLe dernier paramètres formel reçoit la valeur de tous
les paramètres actuels restants~B.

Exemple avec plus de paramètres actuels de formels :
————————————————————————————————————————————————————
&CO
$indent
$procdef print list
    $foreach i %list%
	$show print : %i%
    $next i
$procend
$procexec print A B C
&TX

Résultat :
&CO
print : A
print : B
print : C
&TX
On constate que le paramètre formel ~clist~C reçoit toutes les valeurs
passées à la procédure. La boucle itère donc 3 fois.

Exemple avec moins de paramètres actuels de formels :
————————————————————————————————————————————————————
&CO
$indent
$procdef print titre list
    $show %titre%
    $foreach i %list%
	$show print : %i%
    $next i
$procend
$procexec print "Mon Titre"
&TX

Résultat :
&CO
Mon Titre
&TX
Cette fois, le premier paramètre ~ctitre~C contient ~c"Mon Titre"~C qui est
imprimé avant la boucle. Par contre, la boucle ne s'exécute pas car le
paramètre ~clist~C est vide.

&IT Portée de la définition d'une procédure
———————————————————————————————————————————
1. Les procédures doivent être définies avant de pouvoir être appelées.

2. Une fois définie, une procédure reste appelable au sein de la même session de IODE, même
si le rapport qui l'a définie est terminé. On peut exécuter un rapport qui n'a
d'autre effet que de charger des procédures en mémoire. Ces procédures resteront disponibles
pendant toutes la session IODE.

3. Une procédure peut être remplacée par une autre du même nom à tout moment.

&IT Portée des paramètres formels
—————————————————————————————————
Les paramètres formels sont traités comme des defines dont la portée est
limitée à la procédure courante.

Par conséquent, si un define existe ~uavant l'appelU de la procédure avec le même nom qu'un
des paramètres, ce define ne peut être utilisé au sein de la procédure.
Après la procédure, il reprend sa valeur antérieure.

Exemple :
&CO
$define titre Quotients de mortalité
$show Avant la proc : %titre%

$procdef print titre list
    $show Pendant la proc :%titre%
    $foreach i %list%
	$show print : %i%
    $next i
$procend

$procexec print "Mon Titre"
$show Après la proc :%titre%
&TX

Résultat :
&CO
Avant la proc : Quotients de mortalité
Pendant la proc :Mon Titre
Après la proc :Quotients de mortalité
&TX

&TI E-Views
———————————
Dans la commande ~c$WsImportEviews~C,
la traduction de la fonction de taux de croissance (@PCH) en grt est remplacée par 0.01 * grt.
&CO
    @PCH(X)  ->> 0.01 * grt(X)
&TX

>

<Version 6.39> (13/02/2013)
    Version 6.39 (13/02/2013)
    —————————————————————————
..esc ~

&TI Rapports
————————
Correction de bugs mineurs dans la construction de tableaux dans les rapports à l'aide
de commentaires (fonction ~c$DataUpdateTbl~C).
>

<Version 6.38> (11/02/2013)
    Version 6.38 (11/02/2013)
    —————————————————————————
..esc ~

&TI IODECMD
————————————————
Syntaxe :

&CO
    iodecmd [-nbrun n] [-alloclog filename] [-v] [-y] [-h] reportfile
&TX
where :
&EN -nbrun n : n = nb of repetitions of the report execution (default 1)
&EN -allocdoc filename : memory debugging info stored in filename (developpers only)
&EN -v : verbose version (all messages displayed)
&EN -y : answer yes to all questions asked during the report execution
&EN -h : display program syntax
&EN reportfile : report to execute


&IT Error messages
——————————————————
The error messages are now included in iodecmd (replacing msg like "error
##nnn" by something more readable).


&TI Reports
———————————

&IT $Quit or $QuitOde
—————————————————————
L'utilisation de la fonction $quit dans un rapport iode provoquait une erreur dans iodecmd.
Cette erreur est corrigée.

&IT $EqsEstimate
———————————————
Les coefficients inexistants avant l'estimation sont créés automatiquement.
Cela évite de devoir les créer dans une commande séparée.

&IT $vseps, @vtake(), @vdrop(), @vcount()
—————————————————————————————————————————
New commands and functions that allow an easier manipulation of lists
in the report execution process.

&CO
    $vseps {seperator list}  : specify the new list separator(s) -- default : ",; "
    @vtake({n},{list})       : take the n first elements of the list (or last if n is negative)
    @vdrop({n},{list})       : drop the n first elements of the list (or last if n is negative)
    @vcount({list})          : return the number of elements in the list
&TX

Exemple :
&CO
    $vseps ;
    $define LIST A;B;C;D
    $label next
    $define ELEMENT @vtake(1, %LIST%)
    $msg ... Some operation on element %ELEMENT% ...
    $define LIST @vdrop(1,%LIST%)
    $goto next @vcount(%LIST%)
&TX

&IT Boucles :  $foreach et $next
———————————————————
Cette commande de rapport permet de simplifier l'écriture de
boucles ou de boucles imbriquées.

La commande $foreach permet de spécifier
un index et la liste de valeurs que cet index doit successivement prendre.

La commande $next permet de revenir au point de départ de la boucle
($foreach) et de passer à la valeur suivante de l'index.

&SY2
    $foreach {index} {values}
    ...
    $next {index}
&TX
où
&EN ~c{index}~C est un nom de macro de maximum 10 caractères (par exemple ~ci, idx, PAYS~C , ...)
&EN ~c{values}~C est une liste de valeurs séparées par des virgules, blancs ou
point-virgules. Les séparateurs peuvent être modifiés par la commande ~c$vseps~C

Exemple 1 : boucles imbriquées
&CO
    $foreach I BE BXL VL WAL
	$foreach J H F
	$show [%I%;%J%]
	$next J
    $next I
&TX

Exemple 2 : utilisation de listes
&CO
    $DataUpdateLst MYLIST X,Y,Z
    $Define n 0
    $foreach I @lvalue(MYLIST)
	$Define n {%n% + 1}
	$show Element %n% : [%I%]
    $next I
&TX


&IT $goto
—————————
La fonction goto prend un ou deux arguments :
&EN argument 1 : le label où le programme doit pointer
&EN argument 2 (optionnel) :indique s'il faut ou non aller au label indiqué.
Si cet argument est un nombre différent de 0, le rapport se poursuit au
label indiqué. Sinon le rapport se poursuit à la ligne suivante.

Auparavant, le rapport se poursuivait au label uniquement si l'argument 2 était 1. Toutes les
autres valeurs étaient équivalentes à 0.

&TI Execution of identities
—————————————————————————
Augmentation des performances pour les grands ensembles d'identités et les
identités très longues : cpu diminué d'un facteur 20.

&TI Format Ascii
————————————————
Les valeurs suivantes sont reconnues comme na dans les fichiers ASCII :
&CO
    .
    /
    na
&TX

Attention, deux points collés sont considérés comme deux valeurs na distinctes. Ainsi,

&CO
    1 ./. 2 .. 3
&TX
équivant à :
&CO
    1 na na na 2 na na 3
&TX

&TI  Estimation
———————————————
L'estimation d'équations dont les coefficients ont des valeurs nulles pouvait dans
certains cas être erronnée. Les coefficients nuls sont fixés à 0.1 avant de lancer le
processus d'estimation, ce qui pourrait dans certains cas poser problème.
>

<Version 6.37> (28/09/2012)
    Version 6.37 (28/09/2012)
    —————————————————————————
..esc ~

&TI Memory leaks
————————————————
Différentes corrections ont été apportées pour permettre de gagner ou de
récupérer de l'espace mémoire.

&EN IodeEnd() : suppression des WS
&EN SimulateSCC() : récupération de stracpy() des noms de équations
&EN Repeat() : free(line)
&EN GMacro() : free(tmp)
&EN KI_scalar_list() : déplacement dans la boucle d'un free
&EN Remplacement de malloc, realloc et free par SCR_*().

>

<Version 6.36> (31/05/2012)
    Version 6.36 (31/05/2012)
    —————————————————————————
..esc ~

&TI Genereralized Samples
—————————————————————————
Different elements have been added to the generalized sample syntax :


&EN new macros to
specify the first (BOS = Begin Of Sample) and last (EOS = End Of Sample)
period of the current WS, the current period (NOW), or the first period (with
respect to the current periodicity), the number of sub periods and the current sub period :
&EN2 ~bBOS~B : Begin of Sample
&EN2 ~bBOS1~B : first period in the BOS's year
&EN2 ~bEOS~B : End of Sample
&EN2 ~bEOS1~B : first period in the EOS's year
&EN2 ~bNOW~B : current period (according to the computer clock)
&EN2 ~bNOW1~B : first period in the NOW's year
&EN2 ~bP~B or ~bPER~B : number of periods in one year of the current sample
&EN2 ~bS~B or ~bSUB~B : current subperiod (eg : for a monthly sample, S is equal to 5 in may)

&EN shift operators : ~c<<n~C and ~c>>n~C : concatenated to a period definition,
they shift the period by the specified number of periods.

&EN reverse increments : ~c2000:5*-n~C creates a sample in reverse order : ~c2004;2003;2002;2001;2000~C.

&IT BOS, EOS, NOW, BOS1, EOS1, NOW1, SUB, PER
———————————————
Assuming that ~c1990M6:2010M12~C is the current sample,
the following samples are equivalent to each other :

&CO
    BOS:2;EOS   ==   1990M6;1990M7;2010M12
    EOS;EOS1    ==   2010M12:2010M1
    NOW;NOW1    ==   2012M5;2012M1
    NOW:P       ==   2012M5;2012M6;...;2013M4
    NOW:3*P     ==   2012M5;2013M5;2014M5
    NOW1:SUB    ==   2012M5
&TX

~cNOW~C is thus replaced by the current period (defined by the computer clock).
The sub-period depends on the current periodicity. In the case of Weekly series,
the current week is based on the ISO-8601 standard.

&IT Shift operators <<n and >>n
—————————————————————————————
When the value of the sample is not known in advance, for example when the
variable file is a report parameter or when a report can be used with different
input file, it is
important to be able to indicate not only the last observation, but also the
next-to-last observations or more generally the ~ulast n observations~U. The
same is true for the first observations which can also be unknown at the
moment a report is written.

To specify EOS minus 10 periods, simply write :
&CO
    EOS<<10
&TX

More generally, a specified period can be modified by a left or right shift :
&CO
    2000Y1>>5      === 2005Y1
    1999M1>>12     === 2000M1
    EOS<<1         === 2019Y1 (if EOS == 2020Y1)
    BOS<<1         === 1959Y1 (if BOS == 1960Y1)
    NOW;NOW1<<4    === 2012Q2;2011Q1 (for quaterly data)
&TX

When used with a null argument, the shift oprerators have a special meaning :
&EN <<0 means "first period of the year"
&EN >>0 means "last period of the year"

&CO
    NOW<<0;NOW>>0    === 2012Q1;2012Q4
&TX

The same operators accept the new macros PER (P) and SUB (S) :
&CO
    NOW<<0:P        === 2012Q1;2012Q2;2012Q3;2012Q4
    NOW:3*P        === 2012Q2;2013Q2;2014Q2
&TX

The shift operators can also be applied on expressions :
&CO
    NOW;(NOW<<0>>1/NOW<<0)>>P   === 2012Q2;2013Q2/2013Q1

&TX
&IT Increments
——————————————
A range of periods is defined by :

&EN the first period
&EN the number of repetitions
&EN a optional increment to add more than one period at a time

In the previous versions, increments should be positive. It is now possible
to use negative increments, allowing expressions such as :

&CO
    EOS:5*-1  == 2020;2019,2018,2017,2016
&TX

&TI E-Views to IODE
———————————————————
Object : extraction of equations, scalars and identities from E-Views export data.

&IT Format of E-Views data
————————————————————————————

&CO
Forecasting Equation:
=========================
D(B_PQVC) = C(1)*D(PQVC) + C(2)*(D2001+D2002) + C(3)*D(B_PQVC(-3))

Substituted Coefficients:
=========================
D(B_PQVC) = 0.894661573413*D(PQVC) - 0.0284533462569*(D2001+D2002) + 0.241546373731*D(B_PQVC(-3))

@IDENTITY gro_w_produz_def  = w_produz_def  / w_produz_def(-1)  - 1
&TX


The E-views file is interpreted as decribed below :

&EN the 2 equations following the titles "Forecasting Equation" and "Substituted
Coefficients" are extracted.

&EN the first equation is translated into IODE format :
&EN2  ~cD(...)~C is replaced by ~cd(...)~C
&EN2  ~cC(<<n>>)~C is replaced by the ~cendoname_<<n>>~C
&EN2  ~cExpr(<<lag>>)~C is replaced bu ~cExpr[<<lag>>]~C
&EN2  the first encountered variable is choosen as the endogenous variable
&EN2 the first = sign is considered as the separator between left and right
    members of the equation and therefore replaced by :=

&EN the coefficients values are extracted from the second equation ("Substituted Coefficients").
&EN the lines beginning by ~c@IDENTITY~C are extracted and translated in
    IODE equations with no coefficient.

&EN every equation is added in the current Equations WS.
&EN every detected coefficient is saved in the Scalars WS.

&IT Fonction de rapport
———————————————————————
Pour exploiter ce format, il faut appeler la fonction de rapport suivante :
&CO
    $WsImportEviews filename
&TX

où filename est le nom du fichier à importer. Les WS courants sont augmentés des équations
et scalaires détectés.


&TI Comparaison de WS de variables
——————————————————————————————————
Correction d'un bug qui rendait la comparaison des séries incorrecte (les
premières et dernières valeurs n'étaient pas comparées).

>

<Version 6.35> (28/03/2012)
    Version 6.35 (28/03/2012)
    —————————————————————————
..esc ~

&TI Simulation
——————————————
La performance des simulations, spécifiquement lors du démarrage (link, sort), a été
largement améliorée. Pour atteindre cet objectif, le processus de simulation
a été divisé en 2 étapes. La première ne s'occupe que du réordonnancement du
modèle, la seconde de la simulation.

De plus, des améliorations notables ont été apportées à plusieurs
endroits en terme de vitesse d'exécution dans la phase de démarrage. La
simulation démarre quasi instantanément même avec un modèle dépassant les
100 000 équations.

&IT Découpe de la simulation en deux passages
—————————————————————————————————————————————

Deux nouvelles fonctions de rapport et les deux écrans de saisie
correspondant ont été créés.

~uComposantes Fortement Connexes~U
——————————————————————————————
La première fonction permet de décomposer le modèle en composantes fortement
connexes (CFC ou SCC pour Strongly Connex Components) et de le réordonnancer.
Trois listes sont donc créées : équations prérécursives,
interdépendantes et postrécursives.

Lors du réordonnancement du modèle, le
nombre d'itérations de triangulation (tri) pour le block interdépendant doit
être spécifié. Cette valeur n'a évidemment d'effet que sur la liste des
équations interdépendantes.

Ces 3 listes ne doivent être contruites qu'une seule fois pour une version donnée du modèle.

Cette première fonction a la syntaxe suivante :

&CO
    $ModelCalcSCC nbtri lstpre lstinter lstpost
&TX
où :

&EN nbtri est le nombre d'itérations de triangulation à effectuer
&EN lst* sont les NOMS des listes destinées à contenir les résultats du tri des équations

~uSimulation~U
——————————————
La seconde fonction lance la simulation du modèle sur base des trois listes
préalablement construites par la fonction $ModelCalcSCC (ou à la main).

Sa syntaxe est :

&CO
    $ModelSimulateSCC from to pre inter post
&TX
où :
&EN from et to déterminent le sample de simulation
&EN pre, inter et post sont les listes qui définissent l'ordre d'exécution du modèle.


&IT Choix du nombre de tris
———————————————————————————

Après la décompostion en CFC, le bloc interdépendant est trié pour augmenter la vitesse
de la simulation. Le nombre de passage de l'algorithme de tri peut être
spécifié à plusieurs endroits :

&EN Dans l'écran de simulation "standard" : paramètre "Passes" fixé
&EN Dans l'écran de calcul de la décomposition du modèle :  paramètre "Triangulation Iterations"
&EN Comme paramètre dans la commande rapport $ModelCalcSCC
&EN Comme dernier paramètre dans la commande rapport $ModelSimulateParms

&NO Dans les versions antérieures, le nombre de passages de la triangulation spécifié dans
l'écran de simulation n'avait pas d'effet

&IT Debug
—————————
L'option debug activait la génération d'un fichier simul.dbg qui contenait une quantité énorme
d'informations. Dans cette version, seules les listes ~c_PRE, _INTER~C et
~c_POST~C (avec la découpe du modèle) sont générées.

&IT Commande de rapport $ModelSimulateParms
———————————————————————————————————————————
Cette commande prend un paramètre supplémentaire avec le nombre d'itérations
de triangulation demandées.

&TI Fonctions de rapport
————————————————————————

Les fonctions suivantes permettent d'obtenir la valeur de certains
paramètres de simulation :

&CO
    @SimEps()   : retourne la valeur du critère de convergence utilisé pour
		  la dernière simulation
    @SimRelax() : retourne la valeur du paramètre de relaxation utilisé pour
		  la dernière simulation
    @SimMaxit() : retourne la valeur du maximum d'itérations utilisé
		  pour la dernière simulation
&TX

Les résultats par période de simulation peuvent être récupérés via les fonctions suivantes :
&CO
    @SimNiter(period) : nombre d'itérations nécessaires à la
		     résolution du modèle à l'année period
    @SimNorm(period)  : seuil de convergence atteint à la résolution
		     du modèle à l'année period
&TX

Un chrono virtuel a été ajouté pour permettre de calculer les durées de traitement :

&CO
    @ChronoReset() : remet le chrono à 0
    @ChronoGet()   : retourne le temps écoulé (en msecs) depuis le dernier
		  reset du chrono
&TX


&IT Exemple
———————————
L'exemple ci-dessous compare les performances selon le nombre de tri de
l'algorithme de simulation.

&CO

  $ Parameters
  $ ----------
  $define modeldir C:\iode\nemesis
  $define model base_neujobs_rd
  $define nbtri 5
  $define simfrom 2011Y1
  $define simto 2012Y1
  $define simper %simfrom% %simto%
  $ModelSimulateParms 0.00001 0.6 500 Both 0 No 1

  $ Output file
  $ -----------
  $PrintDest compare.html HTML

  $ Load fresh files
  $ ----------------
  $WsLoadVar %modeldir%\%model%
  $WsLoadScl %modeldir%\%model%
  $WsLoadEqs %modeldir%\%model%

  $ ==== $ModelCalcSCC ===
  .par1 tit_1
  Comparaison entre les performances selon le nombre de tri

  $ ==== Loop on Tri tests ===
  $define i 0
  $label nexttri

  $ Calcul SCC
  $ ----------
  $show $ModelCalcSCC %i%
  @ChronoReset()
  $ModelCalcSCC %i% pre inter post
  $define cpu1  @ChronoGet()

  $ Simulate
  $ --------
  $ Reload vars for a clean start and modify exo
  $show Reload Vars ...
  $WsLoadVar %modeldir%\%model%
  $DataUpdateVar RPOIL 2011Y1 1.20

  $show $ModelSimulateSCC  %simper% pre inter post
  @ChronoReset()
  $ModelSimulateSCC %simper% pre inter post
  $define cpu2 @ChronoGet()

  $ Reporting
  $ ---------
  .par1 enum_1
  Tris : %i%
  .par1 enum_2
  Calc SCC : %cpu1% msec
  .par1 enum_2
  Simulation (eps = @simeps(); maxit=@simmaxit()) : %cpu2% msec

  $define j {%simfrom%}
  $define totit 0

  $label nextsimper
  .par1 enum_3
  {%j%@T} : conv = @simnorm({%j%@T}), niter = @simniter({%j%@T})
  $define simit @simniter({%j%@T})
  $define totit {%totit% + %simit%}
  $define j {%j% + 1}
  $goto nextsimper {%j% <<= %simto%}
  .par1 enum_3
  Total iterations : %totit%
  $define i {%i% + 1}
  $goto nexttri {%i% <<= %nbtri%}
&TX

&TI Comparaison de WS de variables
——————————————————————————————————
La comparaison des WS de varialbles s'effectait sur base d'une stricte égalité binaire.
Par conséquent les résultats de simulation avaient peu de chance d'être considérés comme
égaux, une différence à 1e-15 suffisant à faire échouer le test d'égalité.

Une nouvelle fonction de rapport, ~c$DataCompareEps~C, permet de fixer le
seuil en-deçà duquel le test d'égalité est considéré comme ayant été
satisfait.

Le test de comparaison est :
&CO
    si x1 <<>> 0 :  |(x1 - x2) / x1| << eps
    sinon :       |x2| << eps
&TX

&SY
    $DataCompareEps eps
&TX

L'écran de comparaison de WS (Data/List/File Compare) permet également de
spécifier cette valeur (paramètre Threshold).

&TI Seuil de comparaison par défaut
—————————————————————
Le seuil de comparaison est fixé à 1e-7 par défaut.
>



<Version 6.34> (29/02/2012)
    Version 6.34 (29/02/2012)
    —————————————————————————
..esc ~

&TI Commandes d'impression en rtf et mif
————————————————————————————————————————
Les commandes de numérotation des pages et de définition des headers et footers de pages
ont été implémentées pour les sorties rtf (Word) et mif (Framemaker).

Par exemple :
&CO
    $printdest resfun.rtf RTF
    $printpagefooter Ecopol - @date("dd/mm/yy") - page%d
    $printpageheader Hypothèses et résultats de la projection de base
&TX

Par ailleurs, une partie du catalogue de paragraphes est supprimée : il s'agit par
exemple de par_2, enum_6, ... qui ne sont jamais utilisés.


&TI Hodrick Preskott
————————————————————

Amélioration de la fonction dans le cas où l'expression contient des NaN :
les premières et dernières valeurs NaN sont skippées.


&TI Nouvelles fonctions de rapport
——————————————————————————————
De nouvelles fonctions de rapports permettent d'automatiser plus facilement
certains processus, notamment en extrayant des informations des objets
équations.

Le SAMPLE d'estimation d'une équation peut être retrouvé dans un rapport à l'aide
de la fonction ~c@eqsample~C. La partie "FROM" et la partie "TO" sont obtenues
respectivement par les fonctions @eqsamplefrom et @eqsampleto.

A l'aide de ces fonctions, il devient facile d'automatiser le processus de
réestimation.

&CO
    @eqsample(eqname)     : retourne le sample d'estimation de l'équation ~ceqname~C
    @eqsamplefrom(eqname) : retourne la partie FROM du sample d'estimation
    @eqsampleto(eqname)   : retourne la partie TO du sample d'estimation
    @eqlhs(eqname)        : retourne le membre de gauche d'une équations
    @eqrhs(eqname)        : retourne le membre de droite d'une équations
&TX

Dans l'exemple suivant, on extrait la première année d'estimation de l'équation ENDO1
pour réestimer l'équation sur un sample prolongé à droite.

&CO
    $EqsEstimate @eqsamplefrom(ENDO1) 2012Y1 ENDO1
&TX



&TI Transferts entre Excel et IODE dans les rapports
————————————————————————————————————————————————————
Avant cette version de IODE, pour qu'un transfert de données entre Excel et
IODE via la fonction ~c$ExceLGetVar~C puisse fonctionner, le format des
nombres à transférer ne pouvait contenir de séparateur de milliers ni de
signe de devise comme $ ou £.

A partir de :  cette version, les formats contenant des séparateurs de milliers ou des
devises sont correctement interprétés.

Par défaut, IODE lit la définition des valeurs des séparateurs de milliers
et le caractère des devises dans les paramètres "régionaux" (Regional
Settings). Si ces informations ne conviennent pas, elles peuvent être
modifiées par les commandes de rapport suivantes :

&CO
    $ExcelThousand
    $ExcelCurrency
&TX

&IT $ExcelThousand
—————————————————————
Syntaxe :
&CO
    $ExcelThousand [char]
&TX

La valeur de char remplace celle du séparateur de milliers définie dans les
paramètres de Windows. Lorsque ce caractère est rencontré lors du transfert
d'Excel vers IODE, il est ignoré.

Certaines valeurs spéciales de ~cchar~C ont une interprétation particulière :

&EN d, D, p ou P : le séparateur est le point
&EN c ou C : le séparateur est la virgule
&EN ~cspace~C ou ~cs~C ou ~cSpace~C : le séparateur est l'espace
&EN n, N ou pas d'argument : il n'y a pas de séparateur
&EN tout autre valeur est prise telle quelle comme séparateur

&IT $ExcelCurrency
—————————————————————
Syntaxe :
&CO
    $ExcelCurrency [char]
&TX

La valeur de char remplace celle du caractère indiquant la devise dans les
paramètres de Windows. Lorsque ce caractère est rencontré lors du transfert
d'Excel vers IODE, il est ignoré.

Certaines valeurs spéciales de ~cchar~C ont une interprétation particulière :

&EN d, D : dollar
&EN e ou E : euro
&EN p ou P : sterling pound
&EN pas d'argument : il n'y a pas de signe de devise
&EN tout autre valeur est prise telle quelle comme signe de devise

&IT Exemple
———————————
Fichier Excel :
&CO
  |    B       C       D       E
--|--------------------------------
3 | $10,000 $20,000 $30,000 $40,000
4 | $12,345 $23,456 $34,567 $45,678
&TX

Rapport :

&CO
$WSSample 1989Y1 2000Y1
$ExcelThousand C
$ExcelCurrency D
$ExcelGetVar A 1990Y1 Sheet1!R3C2:R3C10
&TX

>

<Version 6.33> (23/06/2011)
    Version 6.33 (23/06/2011)
    —————————————————————————
..esc ~

&TI Transfert IODE vers Excel dans les rapports : ~c$ExcelLang,~C ~c$ExcelDecimal~C
———————————————————————————————————————————————————————————————————————————
Deux nouvelles fonctions permettent de spécifier dans les rapports la
version linguistique d'Excel et la séparateur décimal qui doit être utilisé dans les
transfert de IODE vers Excel.

&IT $ExcelLang
——————————————
Par défaut, IODE considère que la version linguistique de Excel est
l'anglais. Cela implique que les ~cRanges~C dans Excel doivent s'écrire
~cRnCn~C (par exemple R2C4 pour ligne (Row) 2, colonne (Column) 4).

Dans les autres langues, cette écriture ne convient plus et IODE doit
envoyer les données par exemple vers ~cL2C4~C au lieu de ~cR2C4~C dans la
version française.

Pour assurer qu'un rapport IODE reste valable quelle que soit la langue
d'Excel, la commande ~c$ExcelLang~C a été ajoutée à IODE. Elle a pour
syntaxe :

&CO
    $ExcelLang {F|N}
&TX
Tout autre paramètre (E par exemple) remettra IODE en version anglaise.

Exemple
———————
Dans l'exemple ci-dessous, IODE enverra le contenu de la variable AAA à
partir de la cellule L1C1 du Sheet1 du fichier Excel ouvert.

&CO
    $ExcelLang F
    $ExcelSetVar AAA Sheet1!R1C1
&TX

&NO Il faut noter que dans la commande ~c$ExcelSetXxx~C, ~ble range reste défini
comme ~cR1C1~C~B. C'est uniquement en interne qu'IODE transformera cette
information en ~cL1C1~C.

&IT $ExcelDecimal
——————————————
Par défaut, IODE envoie les données numériques vers Excel en utilisant
 ~ble séparateur décimal défini dans les "Regional Settings" de Windows
 (voir Control Panel)~B.

Cette option ne fonctionne que si le séparateur décimal n'a pas été modifié
dans Excel : on peut en effet décider d'utiliser le ~bpoint décimal~B dans Excel alors
que dans Windows, c'est la ~bvirgule~B qui a été choisie.

Pour permettre de forcer l'envoi vers une version Excel dans laquelle le séparateur décimal
ne serait pas celui de Windows, une nouvelle fonction de rapport a été introduite. Sa syntaxe est :

&CO
    $ExcelDecimal {C}
&TX
L'absence de paramètre C ou tout autre paramètre que C (comma) sélectionne le point décimal.

Exemple
———————
Dans l'exemple ci-dessous, IODE enverra le contenu de la variable AAA avec des virgules comme séparateur décimal
à partir de la cellule L1C1 du Sheet1 du fichier Excel ouvert.

&CO
    $ExcelDecimal C
    $ExcelLang F
    $ExcelSetVar AAA Sheet1!R1C1
&TX

>

<Version 6.32> (17/05/2011)
    Version 6.32 (17/05/2011)
    —————————————————————————
..esc ~

&TI Rapports : $WsExtrapolate, $DataScan, $EqsEstimate
———————————————————————————————————————————
$WsExtrapolate, $DataScan, $EqsEstimate : lorqu'une variable fournie comme
argument n'existait pas, le rapport continuait son exécution même si
~c$OnError~C était fixé à ~cAbort~C ou ~cExit.~C

Ainsi, dans l'exemple qui suit, le message Hello s'affiche même si la
variable ~cUnknownVar~C n'existe pas.

&CO
    $OnError Abort
    $WsExtrapolate 1 1990Y1 2000Y1 UnknowVar
    $echo Hello
&TX


&TI Editeur de rapports
———————————————————————
Les couleurs de l'éditeur de rapport ont été modifiées pour améliorer la lisibilité.

&TI Graphiques
——————————————
&EN Added MULTIBAR PERCENT to HTML
&EN Changed MULTIBAR (HTML, RTF) to category chart (ex 1990,1991,2010, 2020)
&EN Corrected bug when " (double quote) is in the graph title (HTML only)
&EN Corrected bug in legends (RTF ldo example)

&TI Users Manual
————————————————
A new chapter "Methods and algorithms" has been added to the users manual.

>

<Version 6.31> (05/04/2011)
    Version 6.31 (05/04/2011)
    —————————————————————————
..esc ~

&TI IODECOM
————————————
Ajout du programme IodeComServer dans la distribution de IODE.
>

<Version 6.30> (28/03/2011)
    Version 6.30 (28/03/2011)
    —————————————————————————
..esc ~


&TI Corrections
——————————————
L'affichage de messages trop longs plantait IODE : cette erreur est
corrigée, la limite dépendant maintenant de la taille de l'écran.

&IT Graphiques en MIF
—————————————————————
Amélioration des textes dans les graphiques : adaptation de la taille des caractères à la définition a2m

&IT Affichage
—————————————
La taille minimum des caractères est portée à 6 points au lieu de 4 auparavant, ce qui était illisible sur
la plupart des écrans.
>


<Version 6.29> (17/03/2011)
    Version 6.29 (17/03/2011)
    —————————————————————————
..esc ~


&TI Programme d'installation d'un upgrade
——————————————————————————————————————————
Le nouveau programme d'installation de IODE permet d'associer des fichiers à IODE.
Pour ce faire, il faut augmenter les droits de l'utilisateur qui installe le programme en
les plaçant au niveau d'Administrateur.

Après l'installation initiale, et donc après l'association entre
les fichiers iode (.var, .eqs, ...) et le programme lui-même. cette augmentation n'est plus nécessaire
pour remplacer le programme ou les fichiers constituant le logiciel et sa documentation.
Un installateur "upgrade" a donc été construit pour permettre à l'utilisateur d'effectuer lui-même la mise
à niveau de sa version.


&TI Rapports
————————————
&EN La commande ~c$quit~C est ajoutée et est synonime de $quitode
&EN La commande ~c$shift~C peut avoir comme paramètre le nombre de ~c"shift"~C à effectuer

&CO
    $shift 2
&TX


>
<Version 6.28> (14/03/2011)
    Version 6.28 (14/03/2011)
    —————————————————————————
..esc ~


&TI Nouvelle version du programme d'installation
———————————————————————————————————————————————
Cette version intègre les nouveautés suivantes :

&EN restriction d'accès aux comptes administrateurs
&EN rétablissement de la liaison automatique entre les fichiers de IODE et le programme IODE, y compris sous
    Vista et Windows 7.
&EN séléction des composants à installer :
&EN2 Shortcuts dans le menu démarrer (onglets IODE)
&EN2 Thèmes et styles pour les graphiques (nouveau)
&EN2 Modèle fun d'exemple (nouveau)
&EN2 Version COM de IODE (enregistrement automatique), permettant un accès
    optimisé aux fonctions de IODE par exemple en VBA (Excel, ...) ou en APL (nouveau)
&EN2 Configuration de Textpad et de Notepad++ pour l'édition des rapports de
    IODE (colorisation syntaxique et auto-completion) (nouveau)
&EN2 Fichiers Excel et APL d'interfaçage avec IODE


&TI Resize de la fenêtre de IODE
————————————————————————————————
La taille de la fenêtre est mémorisée dans le fichier de profil de IODE. Au prochain démarrage, cette
taille est donc reprise par défaut.


&TI Largeur de la colonne des noms d'objets
———————————————————————————————————————————
La largeur de la première colonne des tableaux d'objets est mémorisée pour
les sessions suiVantes dans le fichier de profil de IODE.

>
<Version 6.27> (03/03/2011)
    Version 6.27 (03/03/2011)
    —————————————————————————
..esc ~

&TI Resize de la fenêtre de IODE
————————————————————————————————

La taille de la fenêtre de IODE peut être adaptée par l'utilisateur :

&EN en tirant sur le bord de la fenêtre
&EN en double-cliquant dans le titre de la fenêtre
&EN en indiquant le nombre de lignes et de colonnes dans la ligne de commande

En combinaison avec une diminution de la taille des caractères, cela permet de visualiser
un nombre beaucoup plus grand d'informations sur l'écran.

&TI Taille de caractères
————————————————————————
La combinaison de touches ~cCtrl+Wheel~c + ~cUp~C ou ~cDown~C augmente ou diminue la taille de la police de
caractères courante (limité entre 3 points et 50 points). La taille de la fenêtre
s'adapte en conséquence.

La taille initiale peut être déterminée au démarrage de IODE via le paramètres ~c-fontsize~C  ~cnb_points~C.

&TI Fermeture de la fenêtre
———————————————————————————
La croix dans le coin supérieur droit de la fenêtre permet de quitter IODE (comme le ferait ~cAlt+X~C).


&TI Déplacement dans les tableaux d'édition
———————————————————————————————————————————
La roulette de la souris ("Mouse wheel") permet de déplacer le tableau d'objets et cours d'édition vers
le haut ou vers le bas.

&TI New parameters
——————————————————
New parameters in the iode command line have been created :

&EN ~c-nl~C ~cnumber~C : number specifies the number of lines of the Iode window
&EN ~c-nc number~C : : number becomes the number of columns of the Iode window
&EN ~c-fontsize points~C : specify the number of points of the characters (3-50).
&EN ~c-fontname "fontname"~C : define the font to use in the iode windows
&EN ~c-pbf file~C : indicates that Iode will "play" at startup the keystrokes present in the file
&EN ~c-pbs "string"~C : same as pbf buf with a string defined on the command line


&TI Syntaxe des séquences ~cplayback~C
——————————————————————————————————
Une séquence ~cPlayback~C consiste en une suite de touches, de commentaires et de messages.

Les règles suivantes s'appliquent :

&EN Tous les caractères d'espacement (nl, return, ff, tab) sont
    ignorés sauf le blanc.

&EN Tous les caractères ascii sont envoyés dans le buffer de lecture, blanc compris
&EN Les séquences suivantes indiquent que le premier caractère qui suit (ou la touche @..) sera
modifié par ~cAlt,~C ~cCtrl~C ou ~cshift~C.
&EN2 ~c&a~C : le prochain caractère sera ~calt+char~C
&EN2 ~c&c~C : le prochain caractère sera ~cctrl+char~C
&EN2 ~c&s~C : le prochain caractère sera ~cshift+char~C

&EN Le délai d'attente entre deux touches peut être modifié à l'aide des séquences suivantes :

&EN2 ~c&d0~C : le délai d'attente entre deux touches est annulé
&EN2 ~c&d-~C : le délai d'attente entre deux chars est diminué de 50 ms
&EN2 ~c&d+~C : le délai d'attente entre deux chars est augmenté de 50 ms

&EN La suite de l'exécution peut être interrompue et un message peut être affiché :
&EN2 ~c&m[Msg]~C : affiche le message Msg et attend qu'on presse une touche
    avant de reprendre. Le message se termine à la fin de la ligne courante.
&EN ~c&n~C : équivalent d'un retour à la ligne dans le message
&EN ~c&autre_char~C : renvoie autre_char

&EN Les séquences suivantes renvoient des touches spéciales :
&EN2 @F1 à @F9 : renvoie SCR_F1 à F9 (ou Shift, Ctrl, Alt en fonction de ce qui précède)
&EN2 @FA à @FJ : renvoie SCR_F10 à F19 (ou Shift, Ctrl, Alt en fonction de ce qui précède)
&EN2 @l : renvoie SCR_CSR_LEFT (ou Shift, Ctrl, Alt en fonction de ce qui précède)
&EN2 @r : renvoie SCR_CSR_RIGHT (ou Shift, Ctrl, Alt en fonction de ce qui précède)
&EN2 @u : renvoie SCR_CSR_UP (ou Shift, Ctrl, Alt en fonction de ce qui précède)
&EN2 @d : renvoie SCR_CSR_DOWN (ou Shift, Ctrl, Alt en fonction de ce qui précède)
&EN2 @p : renvoie SCR_CSR_PG_UP  (ou Shift, Ctrl, Alt en fonction de ce qui précède)
&EN2 @n : renvoie SCR_CSR_PG_DN  (ou Shift, Ctrl, Alt en fonction de ce qui précède)
&EN2 @h : renvoie SCR_CSR_HOME  (ou Shift, Ctrl, Alt en fonction de ce qui précède)
&EN2 @e : renvoie SCR_CSR_PG_END (ou Shift, Ctrl, Alt en fonction de ce qui précède)
&EN2 @I : renvoie SCR_INSERT
&EN2 @D : renvoie SCR_DELETE
&EN2 @R : renvoie SCR_ENTER (ou Shift, Ctrl en fonction de ce qui précède)
&EN2 @E : renvoie SCR_ESCAPE
&EN2 @B : renvoie SCR_BACKSPACE (ou Shift, Ctrl en fonction de ce qui précède)
&EN2 @T : renvoie SCR_TAB (ou Shift, Ctrl en fonction de ce qui précède)
&EN2 @autre_char : renvoie autre_char

&EN Le caractère ## marque le début d'un commentaire qui va jusqu'à la fin de
la ligne

&IT Exemples
———————————
La commande qui suit démarre iode et effectue les opérations suivantes :
&CO
    iode -pbs "@R&adt@R@R"
&TX

&EN ~c@R~C : enter dans l'écran d'accueil
&EN ~c&ad~C : Alt+D : va dans le menu Data
&EN ~ct~C : va sur l'option Tables
&EN ~c@R~C : entre dans le sous-menu
&EN ~c@R~C : ouvre l'option "Edit WS"

Le fichier qui suit reprend l'essentiel des possibilités.
&CO
    ## Diminue le délai des touches
    &d0
    ## Affiche un message et attend la touche ENTER
    &mLoad workspaces : Menu Workspace, function Load Work Space
    ## Entre dans la page Load WS
    @R@r@R@R
    ## Introduit le nom des fichiers à charger (@T = TAB pour passer au suivant)
    ..\ode\fun@T
    ..\ode\fun@T
    ..\ode\fun@T
    ..\ode\fun@T
    ..\ode\fun@T
    ..\ode\fun@T
    ..\ode\fun
    ## Message
    &mPress ENTER to continue
    ## F10 pour loader
    @FA
    ## Delai de 50ms
    @ -------------
    &d0&d+
    ## Alt+D : se déplace dans le menu Data
    &ad
    ## T, Enter, Enter : va sur Tables, et entre dans le scroll d'édition
    t@R@R
    ## Maximise (Ctrl + X)
    &cX
    ## Down 4 x Up 5x
    @d@d@d@d
    @u@u@u@u@u
    ## Edit (Enter, F10, Enter)
    @R@FA@R
    ## Down 4 x
    @d@d@d@d
    ## Edit
    @R@FA@R
    ## Idem avec un délai de 150ms
    ## ---------------------------
    &mMême séquence avec un délai de 150 ms
    &d0&d+&d+&d+
    ## Alt+D : se déplace dans le menu Data
    &ad
    ## T, Enter, Enter : va sur Tables, et entre dans le scroll d'édition
    t@R@R
    ## Maximise (Ctrl + X)
    &cX
    ## Down 4 x Up 5x
    @d@d@d@d
    @u@u@u@u@u
    ## Edit (Enter, F10, Enter)
    @R@FA@R
    ## Down 4 x
    @d@d@d@d
    ## Edit
    @R@FA@R
    ## Quit IODE (alt-X, Enter)
    &mPress ENTER to quit
    &aX@R
&TX



>
<Version 6.26> (04/08/2010)
    Version 6.26 (04/08/2010)
    —————————————————————————
..esc ~

&TI Division par 0 en LEC
—————————————————————————
La nouvelle fonction LEC ~cdiv0(x,y)~C retourne 0 lorsque y vaut 0.
&CO
    div0(x, y) = 0 si y = 0
&CO
    div0(x, y) = x / y sinon
&TX

&TI Corrections LEC
———————————————————
Deux corrections ont été apportées dans le traducteur LEC :

&EN la limite de longueur des expressions LEC après remplacement des macros
    ($NAME) est portée de 4000 à 40000 caractères.

&EN lorsque le nombre d'arguments d'une fonction à nombre d'arguments variable dépasse 255
    l'erreur est correctement signalée. Auparavant, elle était dans certains
    cas ignorée et générait des résultats imprésibles (exemple fonction ~clsum~C).

&TI Correction Rapports
———————————————————————
Dans le cas de certaines erreurs LEC (expressions trop longues), l'exécution
des rapports s'arrêtait. Un message d'erreur est dorénavant affiché dans ce cas.



>

<Version 6.25> (17/06/2010)
    Version 6.25 (17/06/2010)
    —————————————————————————
..esc ~

&TI Graphiques en Flash dans les pages HTML
———————————————————————————————————————————
Les graphiques générés dans le format HTML sont remplacés par une version
Flash (Fusion Chart). Cela signifie que lorsqu'on choisit de générer en
sortie un fichier HTML, ceux-ci contiennent la définition de graphiques
exploitables en flash.

Ces graphiques nécessitent une librairie dont la source est définie dans le
fichier ~cswf.ini~C. Par défaut, ce fichier contient les données suivantes :

&CO
[SWF]
    JSPATH=http:/www.plan.be/FusionCharts/JSClass/FusionCharts.js
    GRAPHPATH=http:/www.plan.be/FusionCharts/Charts
    ANIMATE=0
    ROUNDED=1
    GRADIENT=1
&TX
où

&EN ~cJSPATH~C indiquent où trouver le programme de définition des graphiques

&EN ~cGRAPHPATH~C indiquent où trouver les librairies flash

&EN ~cANIMATE~C vaut 1 pour que l'affichage soit progressif

&EN ~cROUNDED~C vaut 1 pour avoir des coins des cadres et des barres "arrondis"

&EN ~cGRADIENT~C indique si les couleurs doivent être pleines (0) ou progressives (1)


>

<Version 6.24> (15/04/2010)
    Version 6.24 (15/04/2010)
    —————————————————————————
..esc ~

&TI Ouverture de fichiers sur réseau
———————————————————————————————————
Iode se plantait parfois lors de l'ouverture de fichiers sur réseau à partir
de l'Explorateur Windows ou lorsque le répertoire par défaut était un
répertoire réseau. Cette erreur est corrigée.

&TI Limitation des noms de fichiers
———————————————————————————————————
&EN La limite des noms de fichiers à 63 caractères reste
d'application pour des raisons de compatibilité entre les versions
successives des fichiers workspace.

&EN Les noms de fichiers ne doivent si possible pas contenir de séparateur
comme l'espace ou la virgule, y compris dans le nom des répertoires. En
effet, l'ouverture à partir de l'Explorateur Windows ne fonctionne pas dans
ce cas.

>

<Version 6.23> (01/04/2009)
    Version 6.23 (01/04/2009)
    —————————————————————————
..esc ~


&TI Gestion mémoire
——————————————
Révision des allocations dans les rapports :
&EN nouvelle librairie d'allocation de mémoire
&EN optimisation de l'espace mémoire alloué (évite en partie le morcèlement
progressif de la mémoire dans le cas des longs rapports)

&EN nouvelle version des librairies C++
—————————————————
Ces modifications augmentent la performance de certaines fonctions et
permettent de passer à 2GB de RAM utilisée.



&TI Rapports
————————————
Réécriture de la fonction $Repeat pour :

&EN gain (parfois important) de vitesse
&EN résoudre le problème des rapports "croisés" (le fichier temporaire
    ~cws.rep~C n'est plus utilisé)

&TI Graphiques Stacked
——————————————————————
Programmation des graphiques barres "stacked" en Frame (MIF) et à l'écran.

&TI Police de caractères pour les graphiques
————————————————————————————————————————————
Nouveau paramètre optionnel ~cfontsize~C  permettant de préciser la taille des caractères
dans les graphiques.

&CO
    $PrintGraphSize width height [fontsize]

    où :
	width et height représentent la taille du graphique en mm
	fontsize donne la taille de la police du graphique en points
&TX


>

<Version 6.22> (18/11/2008)
    Version 6.22 (18/11/2008)
    —————————————————————————
..esc ~


&TI Graphiques
——————————————
Quelques améliorations ont été apportées dans l'affichage des graphiques :
&EN suppression de "Minor Ticks" sur l'axe des X
&EN modification du placement des "Minor Ticks" sur l'axe des Y et Z

&TI Corrections :
—————————————————
Après le chargement des "thèmes" graphiques (.thm), le directory d'origine
est restauré.

>

<Version 6.21> (27/10/2008)
    Version 6.21 (27/10/2008)
    —————————————————————————
..esc ~


&TI Méthode d'estimation "Maximum likelihood" (BHHH)
————————————————————————————————————————————————————
Une nouvelle méthode d'estimation est proposée dans l'écran de définition
des équations : MaxLik (Berndt, Hall, Hall and Hausman). La méthode est
décrite dans les "Annals of Economic and Social Measument, 3/4, 1974".

Il s'agit en fait d'une généralisation des méthodes existantes qui modifie
la direction de déplacement de la solution à chaque itération et non pas une
seule fois (comme dans les cas des méthodes 3SLS ou Zellner) ou aucune fois
(comme dans les autres cas, soit LSQ ou INSTR).


&TI Format ASCII
————————————
Dans le format de définition des équations (fichiers .ae), la méthode doit
s'écrire MAXLIK (à la place par exemple de LSQ).


&TI Fonction de rapport $EqsSetMethod
—————————————————————————————————————
Le numéro d'ordre de la méthode MAXLIK est 4.

&CO
    $EqsSetMethod   {0|1|2|3|4} eqname1 eqname2 ...
	où 0 indique la méthode LSQ (moindres carrés)
	1 indique la méthode Zellner
	2 indique la méthode INF (2 stages avec instruments)
	3 indique la méthode GLS (3 stages avec instruments)
	4 indique la méthode MAXLIK
	eqname1 .. sont des noms d'équations existantes
&TX


&EN Formats ASCII
—————————————————
Dans les formats des variables et des scalaires, la valeur NaN peut être indiquée
soit par na, soit par /, pour permettre la lecture des données provenant de SAS.

&TI Sample généralisé
—————————————————————
Dans le cas des fichiers, il est possible d'imprimer une colonne en "base"
d'un autre fichier, c'est-à-dire en divisant le fichier courant par un autre et en

Par exemple le sample suivant sur les fichier 1 et 2 :
&CO

    2000[1;2;1=2]
&TX

pourrait donner comme résultat :

..tb 4
..sep @
..ttitle Exemple Base
@1R@1R2000[1]@1R2000[2]@1R2000[1=2]
..tl
@1LX1@1D0.700@1D0.400@1D174.982
@1LX2@1D0.814@1D1.200@1D67.847
..tl
@4L(=) Base year
@4L[1]
@4L[2] c:/USR/ODE/pw.var
@4L27/10/08
..te


&TI Impression des formes LEC avec coefficients
——————————————————————————————————————————————
Dans le cas de l'impression d'équations avec remplacement des coefficients
par leurs valeurs (fonction "Print Objects definition"),
le nombre de décimales demandées dans l'écran ou dans le rapport spécifie le
nombre de décimales de chaque coefficients.

La valeur -1 peut toujours être utilisée pour que le système imprime le
maximum de décimales disponibles.

Ce même nombre de décimales est utilisé pour l'impression des t-tests dans l'équation.

Cette modification a un impact sur l'impression des Equations et des Identités.


&TI Impression des scalaires
————————————————————————————
L'impression des scalaires a été revue :

&EN le nombre de décimales indiquées à l'écran est utilisé pour les valeurs
des coefficients, des standard errors et des tests t

&EN les valeurs na sont imprimées comme -.-


>

<Version 6.20> (07/04/2008)
    Version 6.20 (07/04/2008)
    —————————————————————————
..esc ~


&TI Graphs
——————————
Bug bij het printen naar RTF opgelost.


&TI Nieuwe LEC functies
———————————————————————

&CO
    gamma(x) = gamma functie
&TX

>

<Version 6.19> (07/02/2008)
    Version 6.19 (07/02/2008)
    —————————————————————————
..esc ~

&TI DataCalcLst
———————————————

Een lijst kan worden opgebouwd door het product te maken van twee bestaande
lijsten.

&CO
    $DataCalcLst RES ONE x TWO
&TX

De lijst RES wordt gemaakt bestaande uit alle combinaties (product) van
lijst ONE en TWO.

&IT Voorbeeld
—————————————
&CO
    $DataUpdateLst L1 R1;R2
    $DataUpdateLst L2 C1;C2;C3
    $DataCalcLst L1_L2 L1 x L2

L1_L2 = R1C1;R1C2;R1C3;R2C1;R2C2;R2C3
&TX

&TI Nieuwe LEC functies
———————————————————————


&EN urandom(s) = random uniform verdeeld in het interval [0,s[

&EN grandom(m, s) = random variabele met een normal distributie met gemiddelde m
en standaard afwijking s
>

<Version 6.18> (01/12/2008)
    Version 6.18 (01/12/2008)
    —————————————————————————
..esc ~

&TI Grafieken in Word
—————————————

De grafieken zijn aangepast aan wat er bestond in FrameMaker. Als u
afdrukt in RTF (Word), krijgt u dezelfde grafieken als op het scherm.

&TI MultiBarCharts
——————————————————
Nieuwe BarCharts (Stacked Barchart, Stacked in Percent) zijn nu beschikbaar.

&SY
    $PrintMulti STACKMODE
&TX
met
&CO
    STACKMODE = None(=default), Stacked, Percent
&TX

&TI Graph themes
Bepaal de look van uw grafieken via een themabestand
&SY
    $PrintGraphTheme theme
&TX

Theme bepaalt het gebruikte lettertype voor de titel, legende en assen.
Het bestand bevat ook de kleuren, dikte en type van lijnen, staafjes bij
het maken van grafieken.

Theme is het pad naar het bestand, als het pad niet volledig is kijkt iode
in de directory waar iode wordt opgestart en daarna in de directory waarin
iode geïnstalleerd is.


&IT Voorbeeld
—————————————
&CO
    $PrintGraphTheme c:\usr\ode\bw.thm
&TX
Voorbeeld van theme-bestand
&CO
    TitleFont name=Palatino, style=bold, size=8
    AxisFont name=Arial, style=regular, size=8
    LegendFont name=Arial, style=regular, size=8
    Line color=orange, width=2, style=solid, pointer=nothing
    Line color=blue, width=2, style=solid, pointer=nothing
    Line color=green, width=2, style=solid, pointer=nothing
    Line color=red, width=2, style=solid, pointer=rectangle
    Line color=violet, width=2, style=solid, pointer=circle
    Bar color=blue, style=bdiagonal
    Bar color=orange, style=solid
    Bar color=paleblue, style=solid
    Bar color=red, style=fdiagonal
&TX
waar :
&EN {Title|Axis|Legend}Font met name, style (bold, italic, regular) en grootte in points

&EN Line color={orange, blue, red, violet, black, gray, paleblue, green}, width in punten, style={solid, dot, dash, dahdot}, pointer={none, rectangle, circle, triangle}

&EN Bar color={orange, blue, red, violet, black, gray, paleblue, green}, style={solid, bdiaognal, fdiagonal}

&TI ColorBand
—————————————

Plaats een gekleurde achtergrond (bv. om forcast periode aan te duiden)

&SY
    $PrintGraphBand 2007Y1 2011Y1

    $PrintGraphBand zonder argumenten heft het commando op
&TX

>

<Version 6.17> (20/07/2007)
    Version 6.17 (20/07/2007)
    —————————————————————————
..esc ~

&TI Nouvelles fonctions d'exportations des graphiques
—————————————————————————————————————————————————————
Les fonctions graphiques ont été améliorées dans de nombreux aspects. Ces fonctions
permettent notamment maintenant d'exporter directement des graphiques dans
une dimension adéquate vers Word.

&TI Fonction LEC app()
—————————————————————
La fonction
~capp()~C du LEC a été améliorée pour les cas de séries contenant des
valeurs nulles.

Le résultat de la fonction
~capp(A,~C B) est obtenu comme suit au
temps t :

&EN Si
~cA[t]~C est défini,
~cA[t]~C est retourné

&EN Si
~cA[t]~C n'est pas défini, on calcule les valeurs
~ct0~C et
~ct1~C autour de
~ct~C
telles que
~cA[t0]~C et
~cA[t1]~C soient définis et non nuls. Si ni
~ct0~C ni
~ct1~C ne
peuvent être trouvés, retourne
~cNaN.~C Il s'agit dans ce cas d'un série
~cA~C
valant ~cNaN~C sur toute la période.

&EN si seul
~ct0~C est défini, retourne
~cB[t] * (A[t0] / B[t0])~C

&EN Si seul
~ct1~C est défini, retourne
~cB[t] * (A[t1] / B[t1])~C

&EN Si
~ct0~C et
~ct1~C sont définis, calcule d'abord
~cDelta = (A[t1]/A[t0]) / (B[t1]/B[t0])~C
puis retourne
~cA[t0] * (B[t]/B[t0]) * Delta ** ((t-t0)/(t1-t0))~C

>


<Version 6.16> (14/06/2007)
    Version 6.16 (14/06/2007)
    —————————————————————————
..esc ~

&TI Fonction LEC if()
—————————————————————
Dans le cas d'une condition dont la valeur est NaN, le résultat de la
fonction if(cond, iftrue, iffalse) est iffalse.

Auparavant, cette fonction retournant iftrue.

Exemple :
&CO
    A 1 2 1 -- 2
    if(A=2, 1, 0)  -->> 0, 1, 0, 0, 1
&TX
>
<Version 6.15> (14/03/2007)
    Version 6.15 (14/03/2007)
    —————————————————————————
..esc ~

&TI Gestion mémoire (en test)
—————————————————————
La gestion des objets en mémoire est modifiée pour permettre la création
d'un plus grand nombre d'objets. On constate en effet que, suivant la taille mémoire
disponible, le nombre maximum d'objets d'un même type ne pouvait dépasser
les 300 à 400 000.

Cette version permet de créer plusieurs millions d'objets d'un même type :
des tests ont été menés jusqu'à 2.850.000 objets.

Cette modification a un coût : les blocs mémoire alloués aux objets étant
plus grands, les manipulations (modifications, destructions, créations)
peuvent parfois être un peu ralenties.

&TI Création d'objets
—————————————————————
Lors de la création de nombreux objets à partir d'un rapport, l'ajout des nouveaux
objets dans la tables des objets peut devenir relativement lent en raison du
réordonnancement constant de la table des noms nécessité par cette
opération d'insertion. Ce problème disparaît cependant lorsque les noms sont
créés dans l'ordre alphabétique : le gain de temps pour quelques centaines de milliers
d'objets peut atteindre un rapport de 10.

Dans l'exemple suivant, le nom des objets crées le sont par ordre
alphabétique (la fonction @fmt permet de formater les entiers : on crée donc
dans l'ordre A000000000, A000000001, etc et pas A1, A2, etc.

&CO
$define max 1000000
$define i 0
$WsSample 2000Y1 2050Y1

$label again
$datacreatevar A@fmt(%i%,000000000)
$show var %i%
$define i {%i% + 1}
$goto again {%i% << %max%}

$quit
&TX
>

<Version 6.14> (23/01/2007)
    Version 6.14 (23/01/2007)
    —————————————————————————
..esc ~
&TI Messages d'erreur
—————————————————————
Quelques messages d'erreur n'étaient pas correctement représentés à l'écran (MSG##nnn apparaissait à la place du texte).
Cette erreur est corrigée.

&TI Mémoire disponible
——————————————————————
La quantité de mémoire disponible affichée dans le bas de l'écran de IODE représente
dorénavant la mémoire RAM libre et plus, comme c'était le cas dans les versions
précédentes, toute la mémoire, y compris la mémoire virtuelle.

Cette nouvelle valeur est plus indicative sur la quantité utilement
disponible : la mémoire paginée sur disque est particulièrement lente et
l'utiliser n'est pas réellement praticable.

>
<Version 6.13> (22/11/2006)
    Version 6.13 (22/11/2006)
    —————————————————————————
..esc ~
&TI Longueur des noms de fichiers
—————————————————————————————————

La longueur des noms de fichiers Iode est limitée à 63 caractères.
De façon générale, les noms trop longs sont une source de problèmes dans
IODE. Modifier cette limite
imposerait de transformer tous les WS (qui contiennent en interne le nom sur 63
caractères max).

Les modifications suivantes ont été apportées dans cette version pour éviter les
plantages du programme :
&EN les noms de rapports de plus de 63 caractères sont acceptés dans l'écran de
    lancement des rapports et dans les rapports IODE eux-mêmes
    ~c($ReportExec)~C

&EN Si une tentative de sauvetage de fichiers Iode est effectuée avec un nom de
    fichier de plus de 63 caractères, un message d'erreur est produit et le processus s'arrête.

>
<Version 6.12> (22/06/2006)
    Version 6.12 (22/06/2006)
    —————————————————————————

&TI Grafieken
—————————————

Bij het maken van grafieken werd de periode veelal tot 2010 als de
observaties stopten in bijv 2006. Nu worden enkel de aangegeven periodes
weergegeven. Wil je toch nog de oude manier van werken: druk af in a2m-formaat
en wijzig de lijn
&CO
 .ggrid TMM
&TX
in
&CO
 .ggrid MMM
&TX

&TI Nieuwe rapportfuncties
——————————————————————————

&IT 1. DataRasVar
—————————————————

Vertrekkende van de waarden van variabelen beantwoordend aan pattern,
worden de waarden verdeeld via een RAS methode.

&CO
$DataRasVar pattern X_dimensie Y_dimensie ref_jaar h_jaar [maxit [eps]]
&TX

&EN ~bpattern:~B de variabelen worden gebruikt die voldoen aan de volgende
 criteria x wordt vervangen door alle waarden uit $X, y door die uit $Y

&EN ~bX_dimensie:~B lijst van de waarden die "x" uit het pattern kan aannemen
    OPGELET: laatste uit de lijst is de SOM over de x dimensie

&EN ~bY_dimensie:~B lijst van de waarden die "y" uit het pattern kan aannemen
    OPGELET: laatste uit de lijst is de SOM over de y dimensie

&EN ~bref_jaar:~B referentie jaar: het jaar waarvoor alle gegevens bekend zijn

&EN ~bh_jaar:~B referentie jaar: het jaar waarvoor alleen de sommen bekend zijn

&EN ~bmaxit:~B maximaal iteraties (default=100)

&EN ~beps:~B de drempel (default=0.001)

~uVoorbeeld~U
&CO
    $WsLoadVar ras.av
    $DataUpdateLst X R1;R2;R3;R4;R5;R6;RT
    $DataUpdateLst Y C1;C2;C3;C4;C5;CT
    $DataRasVar xy $X $Y 1980Y1 1981Y1 10 0.00001
&TX
de RAS matrix ziet er dan als volgt uit:
&CO
    R1C1 R1C2 R1C3 R1C4 R1C5 R1C6 | R1CT
    R2C1 R2C2 R2C3 R2C4 R2C5 R2C6 | R2CT
    R3C1 R3C2 R3C3 R3C4 R3C5 R3C6 | R3CT
    R4C1 R4C2 R4C3 R4C4 R4C5 R4C6 | R4CT
    R5C1 R5C2 R5C3 R5C4 R5C5 R5C6 | R5CT
    -------------------------------
    RTC1 RTC2 RTC3 RTC4 RTC5 RTC6 | RTCT
&TX

met de waarden bij het jaar
~cref_jaar~C.
De nieuwe rij en kolom sommen hebben de waarde uit het
~ch_jaar.~C
Als er waarden bekend in
~ch_jaar~C
voor enkele cellen dan worden die gebruikt.

RAS berekent dan de cellen zo dat de nieuwe randvoorwaarden gelden en
overschrijft de onkende waarden (Not Available)

&IT 2. DataPatternXXX
—————————————————————

Deze functie creëert lijsten met daarin de namen van objecten
die voldoen aan een opgegeven patroon.

~uSyntax~U
&CO
$DataPatternXXX lijst pattern X_dimensie [Y_dimensie]
&TX
&EN ~blijst:~B de naam van de lijst met het resultaat

&EN ~bpattern:~B
: patroon waaraan de naam van de objecten moeten voldoen, als
 men "x" vervangt door de elementen uit de X_dimensie en indien
 opgegeven "y" door de elementen uit de Y_dimensie

&EN ~cX_dimensie~C
: lijst met "x"-mogelijkheden

&EN ~c[Y_dimensie~C
: lijst met "y"-mogelijkheden] : optioneel

&NO
Opgelet: alleen bestaande elementen worden in de lijst opgenome.

~uVoorbeeld~U
&CO
    $WsLoadVar ras.av
    $DataUpdateLst X R1;R2;R3
    $DataUpdateLst Y C1;C2
    $DataPatternVar RC xy $X $Y

$RC=R1C1,R1C2,R2C1,R2C2,R3C1,R3C2
inzoverre R1C1,R1C2,R2C1,R2C2,R3C1,R3C2 variabelen zijn in WS
&TX

&IT 3. $WsClearAll
——————————————————
Maakt alle WS-en leeg.

&IT 4. @evalue
——————————————

Geeft de formule van de EQS opgegeven als argument.

>
<Version 6.11> (10/11/2004)
    Version 6.11 (10/11/2004)
    —————————————————————————

&TI Nouvelles fonctions de rapport
——————————————————————————————
&EN ~b@sqz(string)~B : supprime les blancs de string
&EN ~b@strip(string)~B : supprime les blancs de fin de string

>

<Version 6.10> (18/10/2004)
    Version 6.10 (18/10/2004)

..esc ~
&TI Nouvelles fonctions LEC
———————————————————————

Trois nouvelles fonctions sont introduites dans le langage LEC :

&EN  ~cfloor(expr)~C : partie entière de l'expression
&EN  ~cceil(expr)~C : partie entière de l'expression plus 1
&EN  ~cround(expr [, n])~C : arrondi de expr à la nème décimale. Si n n'est pas défini, il est fixé à 0.

&TI Nouvelles fonctions de rapport
——————————————————————————————————

&EN ~c@time([format])~C : renvoie l'heure. Le format contient ~chh~C pour l'heure,
    ~cmm~C pour les minutes et ~css~C pour les secondes. Par défaut, le format est : ~c"hh:mm:ss"~C.

&EN ~c@date([format])~C : renvoie la date. Le format contient ~cdd~C pour le
    jour, ~cmm~C pour le mois et ~cyyyy~C pour l'année. Par défaut, le
    format est ~c"dd-mm-yyyy"~C

&EN ~c@take(n,string)~C : extrait les ~cn~C premiers caractères de ~cstring~C. Si ~cn~C est négatif, extrait les n derniers.
&EN ~c@drop(n,string)~C : supprime les ~cn~C premiers caractères de ~cstring~C. Si ~cn~C est négatif, supprime les n derniers.
&EN ~c@count(string)~C : retourne le nombre d'éléments de string (séparés par des virgules)
&EN ~c@index(n,list)~C : retourne l'élément ~cn~C de ~clist~C

&EX
    @take(3,iode)    : iod
    @take(-3,iode)   : ode
    @drop(1,iode)    : ode
    @take(-3,iode)   : i
    @count(a,b,c,d)  : 4
    @index(2,a,b,c)  : b
&TX

>

<Version 6.09> (06/09/2004)
    Version 6.09 (06/09/2004)
    ———————————————————————————————————————————————————————————————————————————

..esc ~

&TI Correction de la fonction de rapport $DataUpdateVar
———————————————————————————————————————————————————————

Cette fonction interprétait comme ~cna~C les valeurs ~c+0.0~C ou ~c-0.0~C ou
~c.0~C. Cette erreur est corrigée.
>
<Version 6.08> (16/08/2004)
    Version 6.08 (16/08/2004)
    ———————————————————————————————————————————————————————————————————————————

..esc ~

&TI Intégration de valeurs d'Excel dans les rapports
————————————————————————————————————————————————————
Si un worksheet Excel contient des données nécessaires à l'exécution d'un rapport,
il est facile d'intégrer dynamiquement des valeurs ou des ranges de valeurs dans le texte
d'un rapport.

Il suffit de placer la référence aux cellules nécessaires entre accollades
dans une ligne de rapport :

&CO
    $DataUpdateVar {=Sheet1!R2C1} %YEAR% {=Sheet1!R2C2:R3C10}
&TX

La première référence ~c{=Sheet1!R2C1}~C est lue dans Excel (le fichier doit
évidemment être préalablement ouvert) et est replacée dans la ligne de
commande. Dans l'exemple, il s'agira du nom de la série à modifier.


La seconde référence ~c{=Sheet1!R2C2:R3C10}~C est également remplacée par la
valeur dans Excel. S'il y a plusieurs cellules, celles-ci sont séparées par
des blancs.

Dans l'exemple, on aura par exemple :

&CO
    $DataUpdateVar PNB 1990 3.23 2.34 2.56 3.12 3.45
&TX

Après remplacement, la commande est exécutée comme n'importe quelle autre
commande de rapport.


&TI Commande de rapport $ExcelWrite
———————————————————————————————————

Cette fonction permet d'écrire du texte ou des valeurs quelconques dans une feuille Excel.

Remplace les cellules spécifiées par la ou les valeurs données. Plusieurs
colonnes sont séparées par ~c\t~C et plusieurs lignes par ~c\n~C.

&SY2
    $ExcelWrite MySheet!R4C4 texte[\ttexte][\ntexte][...]
&TX

&EX2
    $ExcelOpen c:\usr\iode\test.xls
    $ExcelGetCmt MySheet!R1C1 Titre\n A:\t{A[1970Y1]}\t{A[1980Y1]}\t{A[1990Y1]}
    $ExcelSaveAs c:\usr\iode\test.xls
&TX

>


<Version 6.07> (08/03/2004)
    Version 6.07 (08/03/2004)
    ———————————————————————————————————————————————————————————————————————————

..esc ~

&TI Définition des samples
——————————————————————
    Correction : lorsque une période est incomplètement donnée (~c2000Y~C au
    lieu de ~c2000Y1~C), le ~c1~C est fixé par défaut (au lieu de ~c0~C auparavant).
    De même, ~c2000Q~C vaut ~c2000Q1~C.

&TI Lecture des tables en ASCII
———————————————————————————
    Les valeurs min et max peuvent contenir une valeur négative dans les
    fichiers ASCII de définition des tableaux.

    Par exemple :
&CO
      T1 {
	DIM 2
	ENGLISH
	BOX 0 AXIS 0 XGRID 0 YGRID 0
	ALIGN LEFT
	YMAX ~b-1.000000~B
	ZMAX ~b-2.000000~B
	DIV LEC "1" LEFT ""
	- TITLE "Title" BOLD CENTER
	- LINE
	- "" "##s" CENTER  LAXIS  GRLINE
	- LINE
	- "A" LEFT LEC "A" DECIMAL  LAXIS  GRLINE
	- "B" LEFT LEC "B" DECIMAL  LAXIS  GRLINE
	/* END OF TABLE */
      }
&TX

&TI Impression des tableaux avec fichier manquant
—————————————————————————————————————————————
    Lors du calcul de tableaux ou de graphiques pour l'affichage ou
    l'impression, si un des fichiers source ([1], [2], ...) n'est pas
    défini, IODE ne s'arrête plus à chaque calcul, mais affiche un message
    dans le bas de l'écran et donne des ~c--~C comme résultat pour les colonnes
    correspondantes.

&TI Impressions multiples
—————————————————————
    Une erreur dans la version précédente ne permettait plus d'imprimer
    plusieurs fois au cours de la même session de IODE. Cette erreur est corrigée.

&TI Taux de croissances moyens
——————————————————————————
    Les samples généralisés contenant ~c//~C (taux de croissance moyens) sont à
    nouveau acceptés.


&TI Graphiques
——————————
    Corrections dans les graphiques : Minor et Major grids adaptés
    (uniquement pour l'impression).
>

<Version 6.06> (02/02/2004)
    Version 6.06 (02/02/2004)
    ———————————————————————————————————————————————————————————————————————————

..esc ~

&TI Simulation
——————————
    Lorsque la méthode de réordonnancement (Sort algorithm) est fixée à
    "None" et qu'une liste d'équations non vide est fournie, l'ordre de la
    liste est conservé pour la simulation.

    Cela implique en général un nombre supérieur d'itérations mais évite le
    temps du réordonnancement.

&TI Graphiques
——————————
    Le fichier iode.ini qui contient les données d'affichage des graphiques
    est placé dans le répertoire temporaire de Windows. Ce fichier est donc
    accessible en écriture ce qui évite, lorque les droits de l'utilisateur
    sont limités, que la fenêtre graphique refuse de se fermer.

&TI Report line
——————————
    Le fichier ws.rep qui contient la ligne de rapport à exécuter (fonction
    "Report/Execute a report line") est placé dans le répertoire temporaire
    de Windows. Ce fichier est donc accessible en écriture ce qui permet,
    même si les droits de l'utilisateur sont limités, de créer le fichier.

    Cette fonction est notamment nécessaire dans le cadre de l'utilisation
    de IODE comme objet COM.
>
<Version 6.05> (10/10/2003)
    Version 6.05 (10/10/2003)
    ———————————————————————————————————————————————————————————————————————————

..esc ~

&TI Commande rapport de gestion de directory et de fichiers
———————————————————————————————

    Nouvelles commandes ajoutées aux rapports :

&EN $chdir dirname : change de directory courant
&EN $mkdir dirname : crée un directory
&EN $rmdir dirname : détruit un directory
&EN $SysAppendFile in out : ajoute le fichier in au fichier out
&EN $A2mToPrinter file.a2m : imprime le fichier file.a2m

&TI Formattage d'entiers
————————————————————————
    Nouvelle fonction ~c@fmt(val,fmt)~C qui retourne un string formatté.

    Formatte un entier val suivant un format fmt donné. Le résultat est un
    string transformé de longueur égale à celle de fmt.

    Les caractères reconnus dans le format sont : 'X', 'x', '9', '0'. Ils
    signifient qu'aux seules positions de ces caractères seront placés dans
    leur ordre d'apparition les caractères résultant du formattage de val.
    Seul cas particulier : le caractère '0' qui sera remplacé par une '0' si
    le caractère correspondant dans le formattage de val est ' '.

&SY
    @fmt(val,fmt)
    où
    val = valeur entière
    fmt = format
&TX

&EX
    @fmt(123,0009)   --->> 0123
    @fmt(123,A0000A)   --->> A00123A
&TX
    Attention, les blancs avant et après le format sont repris dans le résultat.

&TI Underflow et Overflow
—————————————————————————
Les calculs non tolérés (exponentielles en particulier) sont prises en
compte plus correctement notamment dans les fonctions d'estimation.

Auparavant, un message du type "Underflow error" était affiché et Iode se
plantait...
>
<Version 6.04> (18/01/2002)
    Version 6.04 (18/01/2002)
    ———————————————————————————————————————————————————————————————————————————

..esc ~

&TI Longueur des lignes de rapports
———————————————————————————————
    La longueur max est portée à 10000 caractères.

&TI Output HTML
———————————
    Cadrage des cellules de titres à droite (au lieu de centré).

&TI Output HELPHTML
———————————————
    Idem HTML et correction des titres de la table des matières.

&TI Output MIF
——————————
    Gestion des sauts de pages dans les rapports (.page).

&TI LEC
———————
    Correction de la fonction exp(a, b). Cette fontion prenait toujours
    l'exponentielle népérienne. Elle permet maintenant de prendre la puissance
    de n'importe quelle formule.

    Notons que
&CO
    exp(a, b)  == a**b
&TX
    D'autre part, les erreurs de calculs résultant de mauvaises valeurs
    des arguments sont mieux prises en charge par IODE :

&CO
    exp(-1, 0.5)
&TX
    retourne NA (--).
>

<Version 6.03> (13/12/2001)
    Version 6.03 (13/12/2001)
    ———————————————————————————————————————————————————————————————————————————
&TI Simulation
——————————
    Correction de la méthode Newton dans le cas du goal-seeking et d'une
    équation non résolue par rapport à l'endogène.

&TI Graphiques
——————————
    Amélioration des routines graphiques.
>

<Version 6.02>
    Version 6.02 (16/11/2000)
    ———————————————————————————————————————————————————————————————————————————

..esc ~

&TI LEC
———
    Deux nouvelles fonctions dans le langage LEC :
&CO
	hp([[from,]to],expr) : filtre Hodrick-Prescott
	appdif(expr1, expr2) : complète la série expr1 sur base de la série
			       apparentée expr2 en tenant compte des
			       différences au lieu des taux de croissance
			       (comme dans app())
&TX
    La fonction app() est modifiée :
&CO
	app(expr1, expr2) : dans le cas où plusieurs observations sont
	    manquantes, le taux de croissance est réparti de manière
	    équilibrée entre les points connus.
&TX
&TI Gestion Mémoire
———————
    Augmentation de l'espace mémoire dans le cas de grands WS
    (variable interne K_CHUNCK=1024).
>

<Version 6.01>
    Version 6.01 (16/10/2000)
    —————————————————————————

..esc ~

&TI Rapports
————————
    Nouvelles @-functions:
&CO
	@cvalue(name, ...) : texte de commentaires
	@vvalue(name, ...) : valeurs de séries
	@sample(B|E|nil)   : sample courant (début, fin, les deux)
&TX
&TI Readme
——————
    Affichage du readme sous forme HTML.

&TI Distribution
————————————
    Distribution de IODE à partir du site ~l"http://www.plan.be/fr/soft/iode.htm"www.plan.be~L

&TI Graphiques
——————————
	Nouvelle librairie graphique (TeeChart) plus étendue pour
	l'affichage et l'impression des graphiques.
>
<Version 5.32>
    Version 5.32 (16/08/2000)
    —————————————————————————

&TI Low to High
——————————
    Lorsqu'une série contient des données manquantes au milieu de la période
    de définition, la transformation se fait pour chaque partie définie.
    Auparavant, seule la première partie était traitée.

&TI Estimation : méthode stepwise
&CO
    $EqsStepWise from to eqname leccond {r2|fstat}
&TX

&TI Rapports
————————
    Nouvelles fonctions de rapports pour l'échange avec Excel:

&CO
    $ExcelSetScl name cell : fixe une valeur dans Excel à partir d'un scalaire IODE
    $ExcelGetScl name cell : récupère une valeur de scalaire à partir d'Excel
    $ExcelGetCmt name cell : récupère une valeur de commentaire à partir d'Excel
&TX

&TI Nouveaux formats et limites de IODE

Cette version supprime la plupart des limites inhérentes au logiciel IODE.
Elle introduit en plus un algorithme de compression pour diminuer la taille
requise par les WS sur disque.

&IT Compatibilité avec les versions antérieures
———————————————————————————————————————————
    Les anciens fichiers sont interprétés automatiquement lors du chargement.
    Les nouveaux ne sont plus lisibles dans les anciennes versions de IODE.

&IT Nombre d'objets
———————————————
    Le nombre maximum d'objets gérables simultanément en WS passe de 65535 à
    2,147,483,647 (si le hardware le permet).

&IT Nom des objets
——————————————
    Le nom des objets peut contenir 20 caractères maximum au lieu de 10.

&IT Taille des objets
—————————————————
    La taille des objets est limitée à 2,147,483,647 bytes au lieu de 32,500.

&IT Nombre de colonnes des tableaux
———————————————————————————————
    Le nombre de colonnes d'un tableau est limité à 99 (au lieu de 20 auparavant).


&IT Compression des fichiers
————————————————————————
    Les différents WS peuvent être comprimés au moment de leur sauvetage.
    Selon le type d'objet et leur définition, la compression peut aller de
    30 à 75%.

    Chaque objet étant comprimé individuellement, le coût au niveau du temps
    de sauvetage peut être important pour les fichiers contenant de nombreux
    objets ou sur des machines lentes (1000 variables de 25 obs. à la sec.
    sur un Pentium III 450 MHz).

    Iode reconnaît et transforme automatiquement les fichiers comprimés.

    Le chargement n'est pratiquement pas pénalisé par la procédure de
    décompression. Un seul cas fait exception : les fichiers de variables
    dont les séries doivent être chargées par une fonction de type
    $WsCopyVar: dans ce cas, la lecture est ralentie par le fait que les
    longueurs des séries comprimées deviennent variables et que le fichier
    doit par conséquent être lu séquentiellement.

    Le panneau de sauvetage présente un checkbox qui permet d'indiquer si on
    souhaite ou non comprimer les fichiers.

&TI Rapports
    ————————
    La fonction $WsSaveCmpXxx permet de sauver les WS en les comprimant.

    Nouvelle fonction ~c@srelax(scl1,scl2,...)~C qui retourne la valeur
    du relax d'un scalaire.

&TI DDE
———
    Correction dans les transferts entre IODE et APL. Les valeurs
    inférieures à 1e-10 plantaient parfois IODE.

&TI LEC
———
    Correction pour les équations du type log(X) :=
>
<Version 5.31>
    Version 5.31 (11/05/2000)
    —————————————————————————

&TI IMPORT GEM
——————————
    La fonction ~cFile/Import~C lit le nouveau format des fichiers GEM (projet
    Nemesis/E3ME).

    La fonction de rapport ~c$FileImportVar~C est également implémentée pour ce
    nouveau format:
&CO
	$FileImportVar GEM rulefile infile outfile from to  [trace]
&TX

&TI Formattage de périodes dans les rapports
————————————————————————————————————————
    L'expression {1980Y1 - 1@@T} dans les rapports ne donne le résultat
    correct - à savoir 1979Y1 - que si le sample courant contient 1979Y1. Si
    le sample était par exemple 1980Y1 2000Y1, le résultat était 1981Y1.
    Cette erreur est corrigée.
>
<Version 5.30>
    Version 5.30 (13/04/2000)
    —————————————————————————
&TI Simulation
——————————
    Le processus de simulation ne s'arrête plus si une valeur NAN est atteinte
    dans le prologue et l'épilogue du modèle (les parties non interdépendantes).
>
<Version 5.29>
    Version 5.29 (17/03/2000)
    —————————————————————————

&TI Rapports
————————

    Une ligne de rapport est interprétée avant d'être (éventuellement)
    exécutée. Cette interprétation se fait de la façon suivante:

    L'ordre d'interprétation est le suivant :

    Les caractères spéciaux sont : ~b%, { et @.~B

    Chaque ligne est interprétée de gauche à droite. Dès qu'un des
    caractères spéciaux est rencontré, un traitement particulier est
    appliqué.

Si on rencontre % :
&EN si le suivant est %, un seul % est conservé comme du texte
&CO
    Exemple : augmentation de 10%% du PNB ->> un seul % reste dans le texte
&TX
&EN sinon, la macro est remplacée par sa valeur ou vide si la macro n'existe pas :
&CO
	    Exemple : la variable %VAR% ->> la variable XYZ
&TX

Si on rencontre { :
&EN si le suivant est {, un seul { est conservé comme du texte, le texte est
    lu jusqu'à }, les macros sont remplacées

&EN2 si le texte résultat commence par $ ou ##, il s'agit d'une commande
	  de rapport qui est exécutée et le résultat (0 ou 1) se retrouve
	  dans le texte.
&EN2 sinon, le texte résultat est calculé comme une formule LEC à la
	  période courante définie par $SetTime. Si la formule se termine
	  pas @T ou @999.999, le résultat est formaté en conséquence.

Si on rencontre @ :

&EN2 si le suivant est @, un seul @ est conservé comme du texte,
	le texte est lu jusqu'à la parenthèse fermante,
	la fonction correspondante est exécutée. A noter qu'en
	l'absence de parenthèses, le texte reste inchangé (Ex.: gb@plan.be
	reste tel quel).

&CO
	$define VAL 123.123
	$msg {%VAL%@999.9} gb@plan.be
&TX
	Donne :
&CO
	123.1 gb@plan.be
&TX

&TI Formattage dans les rapports
————————————————————————————
    Le format dans les formules LEC des rapports peut s'exprimer comme .9, auquel
    cas, le nombre de décimales est fixé et le nombre de positions avant la virgule
    est déterminé par la valeur du nombre lui-même.

    Exemple :
&CO
	{123.456@.99}    ->> 123.46
	{123.456@9999}   ->> 123
	{123.456@99}     ->> **
	{123.456@999.99} ->> 123.46
&TX
>
<Version 5.28>
    Version 5.28 (03/03/2000)

..esc ~

&TI Format Ascii des tableaux
—————————————————————————
    Le format ascii des tableaux comprend maintenant les informations
    concernant les attributs graphiques.

&IT    Exemple
&CO
	TABLE {
	DIM 2
	ENGLISH
	BOX 0 AXIS 0 XGRID 0 YGRID 0
	ALIGN LEFT
	YMIN na
	YMAX 1000.000000
	ZMIN 2.000000
	DIV LEC "1" LEFT ""
	- TITLE "Titel" BOLD CENTER
	- LINE
	- "" "##s" CENTER  LAXIS GRLINE
	- LINE
	- "lijn A" LEFT LEC "A" DECIMAL  LAXIS GRLINE
	- "lijn B" LEFT LEC "B" DECIMAL  LAXIS GRLINE
	- "Totaal" LEFT LEC "A+B" DECIMAL  RAXIS GRBAR
	/* END OF TABLE */
	}
&TX
    La description se trouve dans le manuel de l'utilisateur.
>

<Version 5.27>
    Version  5.27 (25/2/2000)
———————————————————————————————————————————————————————————————————————————
&TI High to Low
———————————
    Correction dans le cas des données non disponibles.
>
<Version 5.26>
    Version 5.26 (7/2/2000)
———————————————————————————————————————————————————————————————————————————
..esc ~

&TI Rapports
————————
    Two new functions to automate the import of variables and comments in
    various external formats.

&CO
    $FileImportCmt format rule infile outfile language [trace]

	format = {Ascii, Rotated_Ascii, DIF, NIS, GEM, PRN, TXT_Belgostat}
	rule = rulefile to use to translate names
	infile = file to import
	outfile = IODE-file with imported series
	language =  {E,F,D}
	trace = debug file (optional)
&TX
&CO
    $FileImportVar format rule infile outfile from to  [trace]

	format = {A, R, D, N, G, P, T}
	rule = rulefile to use to translate names
	infile = file to import
	outfile = IODE-file with imported series
	from = begin of sample
	to   = end of sample
	trace = debug file (optional)
&TX
&IT Examples
    ————————
&CO
    $FileImportCmt TXT bstrule.txt bf-06ser.txt bh\p6d.cmt D p6d.log
    $FileImportVar TXT bstrule.txt bf-06obs.txt bh\p6y.var 1980Y1 2000Y1 p6y.log
&TX
>

<Version 5.25> (1/2/2000)
    Version 5.25 (1/2/2000)

..esc ~

&TI Load WS
———————
    En cas de problème lors de la lecture d'un fichier (fichier incomplet
    par exemple), IODE s'efforce de lire le maximum d'objets.
    Cependant,si un fichier pose problème, un message d'erreur est généré et
    il y a lieu de sauver son travail au plus vite. De plus, il est
    conseillé de sauver les données du fichier à problème dans un format
    Ascii et de le recharger par la suite.

&TI Low to High
———————————
    La fonction Low To High s'est enrichie d'une nouvelle méthode
    d'interpolation en plus des 2 existantes. Cette méthode (Step) conserve
    une valeur constante (celle de l'observation de la série de base,
    éventuellement divisée par le nombre de sous-périodes) pour toutes les
    observations calculées).

    D'autre part, la syntaxe des fonctions de rapport correspondantes est :
&CO
	$WsLToHFlow  {L|C|S} file var1 var2 ...
	$WsLToHStock {L|C|S} file var1 var2 ...
    avec L pour interpolation linéaire
	 C pour interpolation par Cubic Splines
	 S pour interpolation par Steps
&TX

&TI Aide
————
    Le fichier d'aide en format HtmlHelp (plus convivial, plus lisible) est
    intégré dans IODE. Cependant, si le système HtmlHelp n'est pas installé
    sur la machine cible, l'ancien fichier d'aide est utilisé.
>

<Version 5.24> (20/1/2000)
    Version 5.24 (20/1/2000)

..esc ~
&TI Corrections

&EN    Interface ~bDataStream~B : corrections pour les années à partir de 2000

&EN     Interface ~bODBC~B : adaptation aux champs de grande taille (longueur max : 4K)
>
<Version 5.23>
Version 5.23 (13/12/1999)


&TI LEC
———
    La fonction ~capp()~C de calcul de série apparentées est modifiée dans
    le cas d'une rétropolation ou d'une extrapolation sur base d'une
    série apparentée. Le taux de croissance est utilisé pour les valeurs
    en dehors de la période de définition de la série à compléter.
>
<Version 5.22>
    Version 5.22 (19/09/99)

&TI Importations
————————————
    Le format d'échange Belgostat version 2000 est disponible dans le
    panneau d'importation. Les commentaires et séries peuvent être
    importés.

    Ce format remplace dorénavant le format DIF-Belgostat. Les fichiers
    de données ne sont plus disponibles via l'ancienne connexion
    Belgostat, mais sous forme de fichiers complets sur le site Internet
    de la BNB (voir GB pour infos).

&TI Rapports
————————
    Nouvelles fonctions :
&CO
	@sstderr(scalar, ...)
	@equal(str1, str2)
&TX
    Exemple
&CO
	$show Valeur de a1 : @sstderr(a1)
	A est le premier paramètre ? @equal(A,%1%)
&TX
    Résultat
&CO
	Valeur de a1 : 2.23
	A est le premier paramètre ? 0
&TX

    Modification support YoY pour Diff et Grt :
&CO
	$DataUpdateVar varname [{L | D | G | DY | GY}] period values
&TX
    Excel : Correction dans ~c$ExcelSetVar~C : la dernière observation était
    manquante.

&TI Lectures tableaux
—————————————————
    Lors de la lecture d'un tableau en ASCII (fichier .at), si une
    forme LEC est erronée ou vide, la cellule correspondante contient
    la forme LEC sous forme d'un string. Il est alors possible de
    la corriger dans l'éditeur de tableau (si cela est souhaité).
>

<Version 5.21>
    Version 5.21 (10/08/99)

..esc ~

&TI Interface ODBC

    De nouvelles fonctions de rapport permettent d'exécuter des requêtes
    SQL. Ces fonctions permettent par exemple de construire automatiquement
    des séries au format IODE à partir de bases de données Access, Oracle
    ou même de fichiers de texte.

    Les commandes de rapport se présentent sous forme de fonctions:
&CO
	@SqlOpen(ODBC_source[,user,passwd])
	@SqlQuery("SQL Query")
	@SqlNext()
	@SqlField(field_nb)
	@SqlRecord([field_nb[,field_nb[,...]]])
	@SqlClose()
&TX
    Pour pouvoir accéder à une base de données dans IODE, il faut
    préalablement définir une 'source ODBC' (Open DataBase Connectivity),
    ce qui peut être fait très facilement à l'aide du programme ODBC32
    inclus dans le Control Panel de Windows.

    La syntaxe complète et des exemples peuvent être trouvés dans le manuel.

&TI Rapports

    Les titres et pieds de pages introduits par les commandes
    ~c$PrintPageHeader~C et ~c$PrintPageFooter~C sont conservés si le résultat
    de l'impression est sauvegardé dans un fichier a2m.
>

<Version 5.20>
    Version 5.20 (21/06/99)
    ———————————————————————
..esc ~
&TI Installation
————————————————
    Les fichiers nécessaires à l'utilisation de la version HTML Help de l'aide
    de IODE sont intégrés dans le programme d'installation de IODE.
    Ces fichiers ne doivent être installés que pour les versions Windows
    95 et NT 4.0. Ils font partie de Windows 98.

&TI Programme A2M
—————————————————
    Le programme A2M qui permet, à l'extérieur de IODE, d'imprimer ou de
    transformer des fichiers A2M, est corrigé.

&TI Rapports
————————
&EN L'impression des tableaux faisant appel à plusieurs fichiers de
      variables nécessite un appel à la fonction ~c$PrintTblFile~C avant
      chaque appel à ~c$PrintTbl.~C Idem pour les graphiques. Ce n'est pas
      nouveau, mais cela peut surprendre!

&EN Nouvelle fonction ~c$RepeatString~C permettant de choisir le string à
      remplacer dans la commande répétée par ~c$Repeat.~C
&CO
      $RepeatString ++
      $Repeat "$DataDuplicateVar ++ ++_1" A B C
      copie la série A sous A_1, B sous B_1, etc.
&TX

&TI Impression de rapports
——————————————————————
    Les numéros de lignes sont imprimés par la fonction d'impression des
    rapports. Par ailleurs, cette fonction imprime à nouveau l'entièreté
    des rapports.

&TI Simulation
——————————
    L'algorithme de Newton-Raphson a été légèrement amélioré.
>

<Version 5.19>
    Version 5.19 (31/05/99)
———————————————————————————————————————————————————————————————————————————
..esc ~
&TI Rapports
————————
    Les lignes des rapports peuvent dorénavant atteindre une longueur
    maximale de 4095 caractères. Cependant, si la ligne contient une
    forme LEC, il est possible que l'espace nécessaire à la forme
    compilée de cette dernière dépasse la limite du LEC.
>
<Version 5.18>
    Version 5.18 (30/04/99)
———————————————————————————————————————————————————————————————————————————
&TI Rapports
————————

&IT Transformation de strings

    Nouvelles ~c@fonctions~C nécessaires pour certains rapports.

&EN @month(mois, langue)
&EN @ansi(mois, langue)

Exemples :
&CO
    @ansi(@month(2,F)) : Février
    @ansi(@month(2,N)) : Februari
    @ansi(@month(2,E)) : February
&TX

&IT Génération d'un HtmlHelp
&CO
    $PrintHtmlHelp [YES|No] : permet de générer des fichiers HTML Help.
&TX

&IT Fonction de bouclage
&CO
    $Repeat "command" args :
&TX
Cette fonction permet d'exécuter une commande sur une liste d'arguments
sans devoir créer un rapport à cette fin. La position de l'argument dans
la liste est représentée par le caractère de soulignement.

Exemples :

&CO
	1. Tri de plusieurs listes

	    $Repeat "$DataListSort _ _" A B C
		équivaut à

	    $DataListSort A A
	    $DataListSort B B
	    $DataListSort C C

	2. Duplicate Vars

	    Avec les nouvelles fonctions @fn(), on peut copier toutes les
	    variables d'un WS en une seule opération :

	    $Repeat "$DataDuplicateVar _ _1" @vexpand(*)
&TX
>
<Version 5.17>
    Version 5.17 (12/03/99)

..esc ~

&TI Seasonal adjustment
———————————————————
    Nouveau paramètre dans le panneau de démarrage qui permet de libérer
    le critère vérifiant si une influence saisonnière est présente dans
    une série.

    Le même paramètre peut être passé comme argument supplémentaire à la fonction
    de rapport $WsSeasAdj :
&CO
	$WsSeasAdj Filename Varname Varname ... Eps
&TX

&TI LEC
———
    Nouveaux opérateurs :

&EN ~clastobs(from,~C ~cto,~C ~cexpr)~C : calcule la dernière observation sur
	    une période donnée

&EN ~cinterpol(expr)~C : fournit une valeur à expr en t en interpolant ou
	    extrapolant les valeurs connues (actuellement de façon linéaire).

&EN ~capp(expr1,~C ~cexpr2)~C : fournit une valeur à expr1 en t en utilisant
	    la série expr2 comme série apparentée

&EN ~crandom(expr)~C : fournit un nombre aléatoire compris entre -expr/2 et +expr/2.

    Modifications:

	Les opérateurs ~clmean~C et ~clstderr~C ne retournent plus de valeur ~cNA~C
	lorsqu'une des observations est ~cNA:~C ils n'utilisent simplement plus
	les valeurs indisponibles dans les calculs.

&TI Rapports
————————
    Nouvelles fonctions dans le groupe @function() :
&CO
	@ttitle(tablename,tablename, ...) : titre des tableaux

	@cexpand(pattern, ...)
	@eexpand(pattern, ...)
	@iexpand(pattern, ...)
	@lexpand(pattern, ...)
	@sexpand(pattern, ...)
	@texpand(pattern, ...)
	@vexpand(pattern, ...)

	@vliste(objname, ...) : liste des variables dans les eqs objname
	@sliste(objname, ...) : liste des scalaires dans les eqs objname
&TX

&TI Impression de tableaux
——————————————————————
    Lorsqu'une variable n'est pas définie, les tableaux s'affichent ou
    s'impriment avec -- comme valeur pour les cellules concernées.
>
<Version 5.16>
    Version 5.16 (24/02/99)

&TI LEC
———
    Nouvel opérateur :

&EN ~clstderr(expr,~C ~cexpr,~C ~c...)~C : calcule la déviation standard
	    des expressions passées comme paramètre

&TI Rapports
————————
    Nouveau groupe de fonctions dont la syntaxe générale est :
&CO
	@function_name(arg1, arg2, ...)
&TX
    Ce groupe de fonctions va progressivement être étendu selon les demandes
    des utilisateurs. Actuellement, les fonctions définies sont :
&CO
	@upper(texte) : mise d'un texte en majuscules
	@lower(texte) : mise d'un texte en minuscules
	@date([format]) : date
	@time([format[]) : heure
	@replace(string, from, to) : substitution d'un texte par une autre

	@fdelete(filename) : détruit le fichier filename
	@fappend(filename,string|NL, ...) : écrit dans un fichier les textes
				passés comme argument
&TX
    Sont en projet (les idées sont bienvenues) :
&CO
	@ttitle(tablename,tablename, ...) : titre des tableaux
	@tgetnl(tablename) : nombre de lignes d'un tableau
	@tgetnc(tablename) : nombre de colonnes d'un tableau
	@tgetlinetype(tablename,line) : type de ligne
	@tgetcelltype(tablename,line) : type de cellule
	@tgetcell(tablename,line,col) : contenu de la cellule [line, col]

	@cvalue(name, ...) : texte du commentaire
	@evalue(name, ...) : forme LEC de l'équation
	@ivalue(name, ...) : forme LEC de l'identité
	@lvalue(name, ...) : définition de la liste
	@svalue(name, ...) : valeur du scalaire
	@tvalue(name, ...) :
	@vvalue(name, ...) : vecteur formatté des données

	@elistv(objname, ...) : liste des variables dans les eqs objname
	@ilistv(objname, ...) : liste des variables dans les idts objname
	@tlistv(objname, ...) : liste des variables dans les tbls objname
	@elists(objname, ...) : liste des scalaires dans les eqs objname
	@ilists(objname, ...) : liste des scalaires dans les idts objname
	@tlists(objname, ...) : liste des scalaires dans les tbls objname

	@cexpand(pattern, ...)
	@eexpand(pattern, ...)
	@iexpand(pattern, ...)
	@lexpand(pattern, ...)
	@sexpand(pattern, ...)
	@texpand(pattern, ...)
	@vexpand(pattern, ...)
&TX

    Ces fonctions s'exécutent dans le cadre d'une ligne de rapport et le
    résultat de la fonction est imprimé dans l'output du rapport.

    Par exemple :
&CO
	Le tableau T1 a pour titre @ttitle(T1). Ce titre en majuscules
	est par conséquent @upper(@ttitle(T1)). Ce texte est imprimé
	le @date() à @time().
&TX
    Pour placer le titre du tableau T1 dans le fichier toto.txt :
&CO
	@fappend(toto.txt, @ttitle(T1))
&TX
>
<Version 5.15>
    Version 5.15 (25/01/99)

&TI Rapports
————————
    Nouvelles fonctions :
&CO
	$PrintHtmlStrip {Yes | No}
	$A2mToRtf filein fileout
	$A2mToMif filein fileout
	$A2mToCsv filein fileout
	$A2mToHtml filein fileout
&TX
>
<Version 5.14>
    Version 5.14 (05/01/99)

..esc ~

&TI Copy
————
    La fonction de copy d'objet (WorkSpace/Copy Into Workspace) et
    la fonction de rapport équivalente sont corrigées.

&TI Euro
————
    La valeur de conversion euro/FB est fixée à ~c40.3399~C dans le LEC.

    Exemple : si on a une série ~cA~C en FB, a forme ~c(A/euro)~C fournit
    la valeur en euro de cette série. Rappelons qu'un diviseur de colonne
    peut être introduit dans les tableaux, ce qui permet de transformer
    très facilement les tableaux.

&TI Y2K
———
    Les valeurs des samples généralisés (pour les impressions) et des
    périodes dans le LEC sont adaptées pour permettre l'écriture des
    années au-delà de l'an 2000 en deux chiffres.

    Ainsi les années inférieures à 50 sont considérées comme 20xx et celles
    supérieures à 50 comme 19xx. Par exemple :
&CO
	A[0Y1]     == A[2000Y1]
	20Y1:3     == 2020Y1:3
	B[50Y1]    == B[1950Y1]
	A[105Y1]    == B[2005Y1]
&TX
>
<Version 5.13>
    Version 5.13 (24/12/98)

..esc ~

&TI LEC
———
    Nouvel opérateur ~ceuro~C dont la valeur sera correctement fixée le 1/1/99.

&TI Impression de variables
———————————————————————
    Le caractère espace ne peut plus être utilisé comme séparateur de liste
    dans l'écran PRINT/VIEW Variables, afin de permettre d'y placer
    des formules LEC comme d A.

&TI Rapports
————————
    ~c$DataDelete~C accepte les wildcards comme argument. Enfin une façon
    rapide de perdre tout votre travail sans devoir couper le PC.
>

<Version 5.12>
    Version 5.12 (17/12/98)

..esc ~

&TI 'Unit Root' tests
—————————————————
    Les tests de Dickey-Fuller sont disponibles au niveau des rapports
    et du panneau d'estimation d'équations.

    Les tests sont sauvegardés dans des scalaires dont le nom est
    composé du préfixe df_ et du nom de la première série apparaissant
    dans la formule à tester. Par exemple, le test pour la formule
    ~cd(A0GR+A0GF)~C est ~cdf_a0gr.~C

&IT    Dans les rapports
    —————————————————
&CO
	$StatUnitRoot drift trend order expression
&TX
    où
&CO
	drift : 0 ou 1 selon que la formule à estimer doive incorporer
		un terme constant (1) ou non (0)
	trend : 0 ou 1 selon que la formule à estimer doive incorporer
		un terme de trend (1) ou non (0)
	order : l'ordre du polynome à estimer pour obtenir les tests
	expression : forme LEC à tester
&TX
    Par exemple :
&CO
	$StatUnitRoot 1 1 3 A
&TX
    L'équation estimée est :
&CO
	d(A) := df_ * A[-1]+
		    df_d +    /* DRIFT */
		    df_t * t +  /* TREND */
		    df1 * d(A[-1]) + df2*d(A[-2]) + df3*d(A[-3]) /* ORDER */
&TX
    Seuls le test de Dickey-Fuller est sauvegardé dans un scalaire sous
    le nom ~cdf_a~C dans le cas de l'exemple.

&IT Dans le panneau d'estimation

    La touche F3 ou le bouton "Unit Root" permettent de spécifier et de
    tester une ou plusieurs formules. Les résultats sont affichés dans
    la fenêtre.

    Le seul scalaire sauvegardé est celui correspondant à la dernière
    expression testée.

&TI Agrégation de séries
—————————————————————
    De nouvelles fonctions de rapports permettent d'effectuer
    des agrégations, des produits ou des sommes de séries.
    Les séries à traiter peuvent se trouver en WS ou dans un
    fichier externe.

    Quatre nouvelles fonctions de rapport ont été définies à cet effet:
&CO
    $WsAggrChar [char] : définit le caractère à introduire dans le
			 code des séries créées
    $WsAggrSum  pattern [filename] : somme des séries définies par pattern
    $WsAggrMean pattern [filename] : moyenne des séries
    $WsAggrProd pattern [filename] : produit des séries
&TX
    Si filename est défini, les séries du fichier seront utilisées à
    la place de celles du WS.

    ~cpattern~C est défini comme une séquence de parties de noms placées
    entre crochets ou parenthèses. Chaque partie peut contenir des
    caractères alphanumériques ou un point d'interrogation.

    Les parties de noms entre parenthèses ne sont pas agrégées. Celles
    entre crochets le sont.

&IT    Exemple
    ———————
	Soit un WS avec des séries par pays (BE, FR, GE), et par secteur
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

    On peut créer la somme de tous les secteurs pour chaque pays par
    les commandes :
&CO
	$WsAggrChar _
	$WsAggrSum (??)[????]
&TX
    Les séries créées seront :
&CO
	    BE____, FR____, ..., GB____
&TX
    Les points d'interrogations entre () permettent de préciser les
    codes de séries à traiter. Les autres indiquent les parties à
    agréger. Dans ce cas les points d'interrogation sont
    remplacés par des _ (ou un autre caractère selon l'argument
    de ~c$WsAggrChar~C) dans les séries résultats. Ce caractère
    peut être blanc. Dans l'exemple, les séries créées sont alors BE, FR
    et GB.

    On peut également créer la somme de tous les pays par secteur ou
    la somme de tous les pays pour certains secteurs :
&CO
	$WsAggrSum [??](????) : crée __S101, __S102, ...
	$WsAggrSum [??](??)[??] : crée __S1__, __S1__, ...
&TX
    On peut limiter la création à un seul ou à quelques codes :
&CO
	$WsAggrSum (BE)[S??9] : crée BES__9
	$WsAggrSum (BES)[?](?9) : crée BES_09, BES_19, BES_29, ... BES_99
&TX
&IT    Caractère de regroupement
    —————————————————————————
    La commande ~c$WsAggrChar~C permet de spécifier le caractère à
    placer dans les séries générées. Ce caractère peut être blanc.
    Pour éviter que des séries déjà agrégées soient reprises dans une
    agrégation ultérieure, ces séries ne sont pas retenues dans le
    calcul si le caractère d'agrégat courant se trouve à une position
    correspondant dans pattern à un point d'interrogation entre
    crochets. Ainsi, la série ~cBE____~C ne sera pas reprise dans le
    calcul ~c(BE)[????]~C. Par contre elle sera reprise dans le calcul
    ~c[??](????)~C, car dans ce dernier cas elle n'intervient pas dans
    la somme.

    Supposons que les séries ~cBE____~C, ~cFR____~C et ~cGB____~C soient
    définies ainsi que ~cBES101~C, ...

    ~c$WsAggrSum~C ~c(??)[????]~C génère ~cBE____~C, ~cFR____~C, etc.
    Elle n'utilise donc pas les séries contenant ~c_~C après la deuxième
    position, comme ~cBE____~C. En effet, si on les reprenaient, on
    additionnerait deux fois les mêmes séries.

    ~c$WsAggrSum~C ~c[??](????)~C génère ~c______~C en prenant la somme
    de ~cBE____,~C ~cFR____,~C ~cGB____,~C ce qui est correct car les
    autres séries (comme ~cBES101~C) donnent lieu à d'autres séries
    (~c__S101~C).
>
<Version 5.11>
    Version 5.11 (03/11/98)

&TI Support des longs noms de fichiers
——————————————————————————————————
    IODE supporte les noms de plus de 15 caractères dans la limite
    des champs des écrans de saisie. La sélection de fichiers
    dans des répertoires comme ~c\My~C ~cDocuments~C est également possible
    pour des fichiers à longs noms.

&TI Correction Simulation
—————————————————————
    Une petite correction a été apportée au programme de simulation
    dans le cas de valeurs d'endogènes très petites (en général
    dans le cas de récupération de données de la version simple
    précision).
>

<Version 5.10> (14/10/98)
    Version 5.10 (14/10/98)
    ———————————————————————
..esc ~

&TI Rapports
————————

    ~cWsCopyVar~C : correction dans le cas où la copie ne concerne
    qu'une seule année.

&CO
     $WsCopyVar 1970Y1 1970Y1 A* B*
&TX

&TI Estimation
——————————
    Les graphiques ne pouvaient être affichés ou imprimés dans le cas
    où le séparateur entre le membre de gauche et le membre de droite
    de l'équation (:=) se trouvait à la ligne. Ce bug est corrigé.

&TI Langage LEC
———————————
    Nouvel opérateur LEC qui retourne le signe d'une expression:
&CO
    sign(expr) vaut
		    1 si expr >>= 0
		    -1 si expr << 0
&TX

&TI Marquer/Copier/Coller
—————————————————————
    Les opérations de "copier/coller" peuvent être effectuées entre
    IODE et d'autres programmes Windows (Excel, ...) ou entre différents
    écrans de IODE. Le "Clipboard" de Windows est utilisé pour stocker
    les données copiées.

    Les touches de fonction classiques de Windows ont été implémentées
    pour faciliter cet usage : Shift+curseur permet de sélectionner
    ou d'étendre la sélection et la souris permet de marquer
    des parties de texte. Pour placer du texte dans le Clipboard
    et copier ce qui s'y trouve à la position du curseur, on
    utilisera respectivement les touches Ctrl+C et Ctrl+V.

    Cette nouvelle facilité est très pratique dans de nombreux
    cas :

&EN  recopier un nom de fichier à charger dans le panneau Load WS
&EN  copier des parties d'équations dans d'autres équations
&EN  copier des valeurs de séries vers Excel, ou de Excel vers IODE
&EN  copier des séries dans un fichier Ascii ou les récupérer
	  d'un fichier Ascii
&EN  recopier les valeurs d'une année pour plusieurs séries dans
	  une autre année
&EN  copier entre deux instances de IODE qui tournent en même temps
&EN  etc

&IT     1. En édition d'un écran
    ————————————————————————
      Cette partie s'applique par exemple à l'écran Load WorkSpace.

      MARQUER
      ———————
&EN  entrer dans un champ : marque tout le champ
&EN  Ctrl+L : marque tout le champ courant
&EN  Shift+Left : prolonge la marque d'un caractère vers la gauche
&EN  Shift+Right : prolonge la marque d'un caractère vers la droite
&EN  Shift+Home : prolonge la marque jusqu'au début du champ
&EN  Shift+End : prolonge la marque jusqu'à la fin du champ
&EN  cliquer avec la souris et garder le bouton de gauche enfoncé :
	  définit ou prolonge la partie marquée

      DEMARQUER
      —————————
&EN  tout déplacement du curseur supprime les marques (LEFT, RIGHT,
	    HOME, END)
&EN  tout caractère entré supprime (et détruit) les marques
&EN  DEL supprime les marques

      DETRUIRE
      ————————
&EN  n'importe quel caractère remplace la partie marquée
&EN  DEL : détruit la partie marquée

      COPIER
      ——————
&EN  Ctrl+C : copie ce qui est marqué dans le clipboard

      COLLER
      ——————
&EN  Ctrl+V : copie à partir de la position courante
		   les données du clipboard (qui peuvent provenir
		   d'un autre programme) et remplace la marque
		   courante

&IT    2. En édition des variables
    ———————————————————————————

      MARQUER
      ———————
&EN  Ctrl+L : marque la cellule courante ou étend la marque
&EN  Shift+Left, Shift+Right, Shift+Home, Shift+End, Shift+PgDn,
	  Shift+PgUp : marque ou étend la marque

      DEMARQUER
      —————————
&EN  Ctrl+U : supprime les marques

      COPIER
      ——————
&EN  Ctrl+C : copie ce qui est marqué dans le clipboard

      COLLER
      ——————
&EN  Ctrl+V : copie à partir de la position courante
		   les données du clipboard

      Ces fonctions permettent par exemple de copier des colonnes ou
      des lignes de données entre Excel et IODE, ou encore de
      copier une année vers une autre pour un ensemble de variables.

&IT    3. En édition des champs EDITOR (rapports, équations, listes, etc)
    ——————————————————————————————————————————————————————————————————
      Cette partie s'applique par exemple à l'éditeur de rapports,
      à la saisie d'une équation, à la défition d'un sample d'impression.

      MARQUER
      ———————
&EN  Ctrl+L : marque la ligne courante ou prolonge jusqu'à
		   la ligne courante (inchangé)
&EN  Ctrl+T : marque le caractère courant ou prolonge la marque
		   (inchangé)
&EN  Alt+B  : marque la colonne courante ou prolonge la marque
		   (inchangé)
&EN  Shift+Left : prolonge la marque d'un caractère vers la gauche
&EN  Shift+Right : prolonge la marque d'un caractère vers la droite
&EN  Shift+PgUp : prolonge la marque d'une page vers le haut
&EN  Shift+PgDn : prolonge la marque d'une page vers le bas
&EN  Shift+Home : prolonge la marque jusqu'au début du champ
&EN  Shift+End : prolonge la marque jusqu'à la fin du champ
&EN  cliquer avec la souris et garder le bouton de gauche enfoncé :
	  définit ou prolonge la partie marquée (rectangle)

      DEMARQUER
      —————————
&EN  Ctrl+U : supprime les marques (ne détruit pas)

      DETRUIRE
      ————————
&EN  Ctrl+D : détruit ce qui est marqué et supprime les marques

      COPIER
      ——————
&EN  Ctrl+C : copie ce qui est marqué dans le clipboard de Windows

      COLLER
      ——————
&EN  Ctrl+V : copie à partir de la position courante
		   les données du clipboard (qui peuvent provenir
		   d'un autre programme) et remplace la marque
		   courante
>
<Version 5.09> (05/10/98)
    Version 5.09 (05/10/98)
    ———————————————————————
&TI Rapports
————————
    La commande ~c$system~C fonctionne à nouveau dans la version
    Win95 de IODE. Cela permet notamment d'exécuter des fichiers
    de commandes ~c(.bat),~C y compris des programmes 16 bits.

    La commande est bloquante, c'est-à-dire que le programme attend
    la fin de l'exécution de la commande ~c$system~C avant de continuer
    le rapport.

&TI Utilitaire wait.exe
———————————————————

    Pour permettre de synchroniser des programmes Windows successifs,
    l'utilitaire wait.exe est ajouté aux fichiers distribués.

&IT    Exemple d'utilisation
    —————————————————————
&CO
	Rapport test.rep
	----------------
	    $system arima.bat QC hermes.var
	    $printobjvar QC

	Fichier arima.bat
	-----------------
	    wait var2db -v %1 -f %2
	    cd d:\usr\tse
	    l_ax23 d:\usr\igor\src commands.txt %1
	    ax23 d:\usr\igor\src commands.txt
	    cd d:\usr\igor\src
	    wait db2var -v %1 -f %2
&TX
    Sans utiliser ~cwait~C dans le fichier ~carima.bat,~C le programme
    ~cl_ax23~C s'exécute sans attendre la fin de ~cvar2db.~C
>
<Version 5.08> (25/09/98)
    Version 5.08 (25/09/98)
    ———————————————————————
&TI LEC
———
    Nouveaux opérateurs en LEC :

&CO
	isan(expr) : retourne 0 si expr est NAN et 1 sinon
	lmean(expr, expr, ...) : retourne la moyenne d'un
				ensemble d'expressions
	lprod(expr, expr, ...) : retourne le produit des expressions
	lcount(expr, ...)      : retourne le nombre d'expressions
&TX
>
<Version 5.07> (26/08/98)
    Version 5.07 (26/08/98)
    ———————————————————————
&TI Format CSV inversé
——————————————————
    Il est possible d'exporter des séries en format CSV inversé,
    c'est-à-dire avec les séries en lignes et non en colonnes.
    Dans ce format, les commentaires ne sont pas exportés pour
    des raisons de largeur de colonnes insuffisantes.

&TI Rapports
————————
    Macro ~c%-nom%~C supprime dans la macro tous les caractères non
    aplhanumériques ou de soulignement.
&CO
	$define ARG0 Abc[-3]
	$msg %ARG0%
	$msg %-ARG0%
	$msg %!ARG0%
	$msg %#ARG0%
&TX
    Donne comme message :
&CO
	Abc[-3]
	Abc3
	abc[-3]
	ABC[-3]
&TX
>
<Version 5.06>
    Version 5.06 (11/08/98)
    ———————————————————————

&TI Utilisation de wildcards
————————————————————————
    Afin de simplifier l'écriture, il est possible d'utiliser des
    "wildcards". Ces wildcards sont l'étoile (*) ou le point
    d'interrogation (?).

&EN * : indique une suite quelconque de caractères, même vide
&EN ? : indique un caractère unique

    On utilisera des noms contenant des étoiles ou des points
    d'interrogation dans certains écrans, dans les formes LEC et dans
    certaines fonctions de rapport. Ces noms sont remplacés par
    des listes contenant les noms correspondants présents en WS.

    Supposons que les variables A1F, A0F, B1F, B0G, AF, BG et A soient
    en WS. On a alors :

&CO
	A*      == A1F;A0F;A
	*1*     == A1F;B1F
	*F      == A1F;A0F;B1F;AF
	?F      == AF
	A?      == AF
	?       == A
	A?*     == A1F;A0F;AF (mais pas A)
&TX

    Dans cette version de IODE, ce type d'expression peut être utilisé
    dans

&EN l'écran ou la fonction $WsCopyXxx
&EN l'écran ou la fonction de création de tableau $DataUpdateTbl
&EN l'écran ou la fonction d'impression de variables $PrintVar
&EN les formes LEC (entre quotes)

    Dans le cas de la forme LEC, l'expression à étendre doit être
    fournie entre quotes (') pour la distinguer de l'opérateur *.

    Dans le cas des écrans ou des rapports, les parties à ne pas
    étendre doivent être exprimées entre double quotes (").

    Ainsi,
&CO
	A*;"B*A"        sera traduit en A1;A2;...;B*A

	A*;"lsum('A*')" sera traduit en A1;..;lsum('A*')
&TX
    Dans les rapports, les fonction suivantes sont concernées par
    cette modification :
&CO
	$WsCopyXxx filename args
	$DataUpdateTbl
	$DataEditXxx
	$PrintVar
&TX

&TI Extensions LEC
——————————————
    Lorsque l'opérateur le permet (lsum, max et min), on peut
    utiliser les wildcards dans le langage LEC. Cependant, pour
    distinguer l'étoile et l'opérateur fois, la liste à étendre
    doit être entourée de quotes simples('). Ainsi,

      ~clsum('A*')~C
    est équivalent à
      ~clsum(A1,A2,AX)~C si A1, A2 et AX sont
     les seules séries commençant pas A dans le WS courant.

    On peut également utiliser une combinaison de noms de séries :

	~cmax('*G;*F')~C


    Le nombre maximum d'opérandes des opérateurs ~clsum(),~C ~cmax()~C
    et ~cmin()~C est porté à 255, de façon à pouvoir exploiter au mieux
    les wildcards.

&IT    Remarque importante
    ———————————————————
    Les noms contenant des wildcards dans les formes LEC sont résolus en
    fonction du contenu du WS COURANT. La forme LEC compilée est
    mémorisée avec ces noms. Si on change le contenu du WS, il est
    possible que certaines séries n'existent plus ou que de nouvelles
    apparaissent. La forme compilée n'étant pas changée automatiquement,
    il faut si on souhaite adapter la forme LEC au nouveau contenu,
    recompiler la forme en l'éditant.

&TI Load WS
———————
    Correction d'une erreur (rare mais possible) dans le chargement
    de fichiers de scalaires.


&TI Rapports
————————
    Correction dans les messages d'erreurs (plus de précision dans
    le texte).

    Les caractères % et { peuvent être introduits comme texte
    normal en étant doublés. Le texte %%1%% ne sera donc pas interprété
    comme une macro, mais comme le texte %1%.

    Commentaires : une ligne de commentaire vide ne génère plus de
    message d'erreur. Elle peut commencer par $ ou par #.

    Nouvelles fonctions:

&CO
	 $Debug {Yes|No} : affiche le numéro de la ligne et le fichier
			   en cours d'exécution dans les rapports.

	 $SysMoveFile filein fileout : renomme un fichier

	 $SysCopyFile filein fileout : copie un fichier

	 $SysDeleteFile file1 file2 ... : détruit des fichiers
&TX

&TI Edition des variables
—————————————————————
    Deux nouvelles options ont été ajoutées à la touche ~cF5~C lors de
    l'édition des tableaux de variables. Ces options permettent
    d'afficher les valeurs en différences et en taux de croissance
    sur une année (Year On Year). On a donc maintenant dans l'ordre:

&EN level
&EN difference
&EN growth rate
&EN YoY difference
&EN YoY growth rate

    Pour faciliter la navigation, on peut utiliser ~cshift+F5~C pour
    revenir au mode précédent.

    Les graphiques s'affichent en fonction du mode utilisé. Ainsi, si
    l'affichage s'effectue en taux de croissance, la touche ~cF8~C fait
    apparaître le graphique dans le même mode.

&TI HTML
————
    Le format HTML de sortie des impressions est enrichi et permet
    de placer automatiquement des graphiques dans les documents. Les
    fichiers graphiques portent le nom IMGn.GIF et se trouvent dans le
    répertoire de travail. Les graphiques sont automatiquement
    référencés dans le fichier HTML résultat.

    Pour permettre de paramétrer la génération des fichiers GIF,
    un nouveau panneau est intégré dans les "Printer Options". Les options
    qui y sont définies peuvent également être définies par des
    fonctions de rapport.

    Les fonctions de rapports sont :
&CO
	$PrintGIFBackColor color    : définit la couleur de fond des
				      graphiques
	$PrintGIFTransColor color   : définit la couleur considérée comme
				      "transparente"
	$PrintGIFTransparent Yes|No : indique si le fichier GIF doit être au
				      format transparent
	$PrintGIFInterlaced Yes|No  : indique si le fichier GIF doit être au
				      format interlacé
	$PrintGIFFilled Yes|No      : indique s'il faut remplir les barres dans
				      les bar charts
	$PrintGIFFont numéro        : indique si le numéro du font à utiliser
				      pour les labels (voir écran pour
				      les valeurs possibles).
&TX
>
<Version 5.05>
    Version 5.05 (31/07/1998)
    —————————————————————————

&TI Rapports
————————
    La macro ~c%*%~C représente tous les arguments du rapport.
    Il est à noter que la commande ~c$shift~C supprime le premier argument
    de ~c%*%~C.


    Par exemple:

&CO
    Fichier main.rep
    ----------------
    ...
    $ReportExec sub1.rep %*%
    ...
    $Shift
    ...
    $ReportExec sub2.rep %*%
    ...

    Exécution du rapport
    --------------------
    $ReportExec Main.rep A B C D E

&TX
    Le rapport ~csub1.rep~C reçoit comme arguments tous
    les arguments passés à ~cmain.rep,~C soit ~cA B C D E~C.
    Le rapport ~csub2.rep~C reçoit les arguments ~cB C D E~C.


&TI Nouveaux opérateurs LEC
———————————————————————
    Les opérateurs suivants ont été ajoutés au langage LEC :

&CO
	- lsum(expr1, expr2, ...) : calcule la somme d'une liste
	  de séries

	- index([[from,] to,] valeur, expr) : retourne la première période
	  où valeur apparaît dans la série expr entre les périodes
	  from et to. Si la valeur n'apparaît pas dans expr, -- est
	  retourné.

	- acf([[from,] to,] valeur, expr) : retourne la fonction
	  d'auto-correlation de degré valeur de l'expression expr
	  sur le sample from à to.

&TX

    Exemples
&CO
	Soient A et B définies sur 1970Y1-1990Y1
	A = 0 1 2 3 ...
	B = 2 4 6 8 ...

	lsum(A,B) vaut 2 5 8 11 ...

	index(6, B) vaut -- -- 2 2 2 2 ... (ce qui équivaut en LEC
					    à -- -- 1972Y1 1972Y1 ...)
	index(1971Y1, 1990Y1, 6, B) vaut 2 2 2 2 ...
&TX

&TI Impressions
———————————
    Les impressions ne fonctionnaient plus correctement dans la version
    précédente pour certains drivers (qms, hp). Cette erreur est
    corrigée.

    Les tableaux générés en Frame (mif) contiennent le titre
    dans le corps du tableau et plus dans un paragraphe séparé.

&TI Aanpassing van Census II methode
————————————————————————————————
    Om geen informatie te verliezen bij het berekenen van de laatste
    observaties, werd het algoritme aangepast. Bij onvolledige periodes
    worden, krijgen gegevens op het einde van de periode voorang.


&TI Schatten van vgln
—————————————————
    Bij het on-line schatten van vgln worden nu ook de testen als scalars
    weggeschreven.


&TI IODE-XLS interface
———————————————————————————
    Lijsten kunnen worden gebruikt bij het opvragen van gegevens

>
<Version 5.04>
    Version 5.04 (01/04/1998)
    —————————————————————————

&TI ACCESS RESEAU ET NOMS DE FICHIERS
—————————————————————————————————
    Des fichiers d'un autre ordinateur du réseau peuvent être accédés
    directement dans IODE sans avoir à monter un drive réseau.

    Par exemple, on peut dans le panneau de LOAD WS entrer un nom
    comme :

&CO
	\\GB\iode\stat\naccount.var
&TX

    qui fait référence au drive "iode" défini sur la machine "GB".

    La lecture des répertoire est également possible sur un drive
    réseau.

    Pour permettre l'introduction de noms plus longs, les champs des
    pages contenant un ou des noms de fichiers ont été étendus à 64
    caractères.

&TI RTF
———
    Les impressions en RTF sont également valables pour les graphiques.
    Les graphiques créés sont inclus dans le fichier RTF en format
    hexadécimal et sont interprétés par Word ou le compilateur
    d'aide Windows.

    La dimension de ces graphiques est déterminée par les paramètres
    définis dans Print Preferences/A2m ou par la commande de rapport
    $PrintGraphSize.

&TI MIF
———
    La génération des tableaux en MIF est améliorée : la largeur de
    la première colonne est adaptée au contenu et celle des colonnes
    suivantes est proportionnellement répartie en fonction du nombre
    de colonnes.

    Graphiques : les attributs des caractères dans les Labels et Titres
    des graphiques sont corrigés.

&TI PRINTER
———————
    Nouvelles options d'impressions :

&EN Landscape ou Portrait
&EN Simplex, Duplex, Vertical Duplex
&EN Sélection de l'imprimante par défaut.

&TI RAPPORTS
————————
    Trois nouvelles fonctions :

&EN $SetPrinter PrinterName
&EN $PrintOrientation {Portrait | Landscape}
&EN $PrintDuplex {Simplex | Duplex | VerticalDuplex}

&TI PRECISION DES DONNEES
—————————————————————

    Affichage en 8 ou 15 chiffres significatifs à l'édition.
    La touche F6 permet de changer la précision de l'édition des
    variables (8 ou 15 chiffres significatifs).
    Sauvetage en 8 ou 15 chiffres significatifs en .av et .as
    Version double précision pour les Variables et les Scalaires.

&TI Fonctions LEC
———
    Nouveaux opérateurs
&CO
     var([from [,to],] x)             == sum((xi-xm)**2) / n
     covar([from [,to],] x, y)        == sum((xi-xm)*(yi-ym)) / n
     covar0([from [,to],] expr, expr) == sum(xi * yi) / n
     corr([from [,to],] x, y)         == covar(x, y) / sqrt(var(x) * var(y))
     stddev([from [,to],] x, y)       == sqrt(var(x))
&TX
    Modifications
&CO
    stderr : corrigé (estimateur non biaisé) == sqrt(sum((xi-xm)**2/(n-1)
&TX
    L'ancien opérateur stderr est remplacé par stddev.

&TI MESSAGES
————————
    Dans les rapports, les messages envoyés par $show sont affichés dans
    la dernière ligne de l'écran même dans le cours d'une procédure
    longue.

&TI WS/COPY
———————
    Correction dans le cas de listes non définies passées comme argument
    de la fonction.

&TI ESTIMATION DE BLOCS
———————————————————
    Correction dans l'estimation de blocs dans les rapports. Auparavant,
    il fallait passer tous les noms des équations du bloc comme argument
    de la fonction EsqEstimate. Une seule équation suffit maintenant si
    elle contient le bloc.
>
<Version 5.03>
    Version 5.03 (08/02/1998)
    —————————————————————————

&TI Tableaux
————————
    Sélection des lignes à copier ou détruire par Ctrl+L à nouveau
    opérationnelle.

    Options graphiques :

&EN l'alignement des observations par rapport à l'abcisse (année)
	  peut être spécifié par courbe (touche F4).

&EN les options box et shadow sont opérationnelles dans
	  l'affichage et l'impression des graphiques. Shadow
	  indique si on veut ou non un hachurage autour du graphique.

&EN les Grids (Major, minor ou None) sont implémentées

&TI Graphiques
——————————
    Quelques corrections d'affichage ont été opérées dans les graphiques.
    En particulier :

&EN il est possible d'imprimer des graphiques sans titre
&EN les bars charts fonctionnent

    Il est possible de modifier la taille des graphiques à imprimer
    ou à intégrer (Frame) par les Options d'impression (Print Setup/A2M).

    Les options Background Color et Brush + Box sont opérationnelles
    pour les graphiques automatiques (variables et estimations).

    La position des bars ou des lignes par rapport à l'abcisse peut être
    choisie : gauche, centre ou droite (voir TABLEAUX).

&TI ESTIMATION
——————————
    Les graphiques des estimations contiennent le nom de la variable
    endogène.


&TI A2M
———
    La génération des fichiers A2M s'est enrichie d'une commande
    de définition des cadres, couleur et fond des graphiques.
    Cette commande est générée automatiquement en fonction
    des options retenues dans IODE.
&CO
     .gbox box color brush
&TX
    Nouveaux paragraphes pour les tableaux :
&CO
	CellRight, CellLeft, ...
	HeaderRight, ...
	FooterRight, ...
&TX

&TI Properties
——————————
    Une nouvelle option du menu système (Properties) permet de
    visualiser le contenu global des WS, le directory de départ et
    la mémoire disponible.

    Cette option est disponible à la fois lorsque la fenêtre est active
    et lorsqu'elle est minimisée (bouton de droite).

&TI Titre de la fenêtre IODE
———————————————————
    Le titre de la fenêtre contient le nom du directory de lancement
    de IODE et un numéro d'ordre correspondant à la chronologie de
    démarrage des processus IODE.

&TI Taille des caractères
—————————————————————
    La taille des caractères est maintenu pour les sessions suivantes de
    IODE (après modification par le menu Système de la fenêtre IODE).

&TI Génération de fichiers d'aide Windows
—————————————————————————————————————
    La génération des fichiers d'aide de Windows est rendue possible
    dans cette version avec une découpe en sujets (topics).

    Lors des impressions, chaque tableau et chaque équation avec
    résultats d'estimation constituent un nouveau topic du fichier
    d'aide.

    De plus les fonctions de rapport suivantes permettent de spécifier
    les niveaux et le titres des sujets de l'aide.

&TI Fonctions de Rapports
—————————————————————————
    Des nouvelles fonctions ont été introduites dans les rapports pour
    permettre de spécifier les options d'impression dans le corps
    d'un rapport.

&CO
   $PrintA2mAppend [NO|Yes]
   $PrintFont      Time|Helvetica|Courier|Bookman|Palatino [size [incr]]
   $PrintTableFont Time|Helvetica|Courier|Bookman|Palatino [size]
   $PrintTableBox  n
   $PrintTableColor [NO|Yes]
   $PrintTableWidth width [col1 [coln]]
   $PrintTableBreak [NO|Yes]
   $PrintTablePage  [NO|Yes]

   $PrintBackground Black|Blue|Magenta|Cyan|Red|Green|Yellow|White
   $PrintGraphBox   n
   $PrintGraphBrush pct|Yes
   $PrintGraphSize  width_mm [height_mm]
   $PrintGraphPage  [NO|Yes]

   $PrintRtfHelp    [YES|No]
   $PrintRtfTopic   titre du topic
   $PrintRtfLevel   [+|-|n]
   $PrintRtfTitle   titre du help
   $PrintRtfCopyright copyright text
   $PrintParanum    [NO|Yes]
   $PrintPageHeader titre des pages suivantes
   $PrintPageFooter  footnote des pages suivantes
&TX

&TI Echanges EXCEL-IODE
———————————————————
    Lorsque la configuration de la machine définit la virgule comme
    séparateur des chiffres décimaux, les tranferts IODE-Excel
    remplacent les virgules par des points dans les nombres (et
    inversement).

&TI Ligne de commande
—————————————————
    La syntaxe du programme IODE.EXE permet de charger plusieurs
    fichiers en une seule opération :

&CO
    iode monws.var monws.cmt monws.at
    iode monws.*
&TX
    Le tableau d'édition correspondant au dernier fichier de la liste
    (ici monws.tbl) sera ouvert au démarrage de IODE.
    Les fichiers non iode ne sont pas chargés.

    De plus, les fichiers ASCII (.ac, .av, ...) peuvent également
    être chargés de cette façon.

&TI Automatic launch in Netscape
——————————————
    Le programme d'installation de IODE enregistre les types de fichiers
    .var, .cmt, etc de telle sorte que, si un lien vers un fichier IODE
    apparaît dans une page HTML (via Netscape par exemple), il soit
    possible d'ouvrir directement le fichier en question dans IODE.

    IODE est lancé et le fichier référencé est chargé dans le WS
    correspondant.

    Attention, le fichier ainsi chargé est préalablement sauvé par
    Netscape dans un fichier temporaire situé dans le directory
    ~c\Windows\Temp~C.
>

<Version 5.02>
    Version 5.02 (20/12/1997)
    —————————————————————————

&TI Impressions Objets
——————————————————
    Correction dans l'impression des variables.

&TI Rapports
————————
    Création des listes : le point-virgule sert de séparateur (au lieu
    de la virgule antérieurement).

&IT     $DSImportDB
    ———————————
    Importation de données de DataStream.

   Syntaxe :
&CO
	$DSImportDB nom_data_stream1, ...
&TX
    Cette fonction va lire sur le sample courant la ou les variables
    définies par nom_data_stream1, ... et crée dans le WS courant une
    série dont le nom est construit en remplaçant les caractères non
    alphanumériques par des understroke.

   Exemple :
&CO
	$DSImportDB BEGDP..A12
&TX
    crée ~cBEGDP__A12~C dans le WS courant.

    Si le nom_data_stream contient le caractère pourcent (%), la
    fonction ne s'exécute pas car % est un caractère réservé dans les
    rapports. Pour pallier ce problème, il faut créer une liste
    contenant le nom des séries et appeler DSImportDB avec la liste
    comme argument.

&CO
	$DSImportDB $MYLST
&TX
    Note : cette fonction n'est opérationnelle actuellement que sur les machines
	   qui ont un accès à DataStream.

&TI Uninstall
—————————
    Nouvelle fonction de désinstallation de IODE. Cette fonction est
    accessible soit via Control Panel/Install Software, soit dans le
    menu START/Groupe IODE.
>
<Version 5.01>
    Version 5.01 (17/12/1997)
    —————————————————————————

&TI Rapports
————————
    La fonction $EqsEstimate imprime les graphiques des observations
    et des valeurs estimées.
&CO
    $IdtExecuteTrace werkt terug
&TX
&TI Execute IDT's
—————————————
    Bij het uitvoeren van identiteiten wordt er per default geen output
    meer gegenereerd. Alleen als de "Trace" of "Debug" optie wordt gebruikt
    krijgt u een overzicht van de uitgevoerde identiteiten. Dit kan u
    fullscreen, of door het $IdtExecuteTrace commando in rapporten.

&TI Print Files
———————————
    U kan uw a2m-files en rapport-definities vanuit IODE afprinten.
>
<Version 5.0>
    Version 5.0 (15/10/1997)
    ————————————————————————
&TI Direct Printen vanuit IODE
——————————————————————————

    In de vorige versies van IODE, moest een A2M-file worden aangemaakt om
    later af te drukken op een printer of door te sturen naar een ander
    pakket (MIF, RTF, ...). Er bestond ook een AGL-formaat met daarin de
    definitie van de grafieken.

    Vanaf de 4.80-versie kan er direct worden afgedrukt op de "default"
    printer, of indien gewenst uw output laten omzetten naar het formaat
    van uw keuze (MIF,RTF, good-old A2M, ...)

    Het volstaat in de Menu-File "Print Setup" aan te duiden hoe u de output
    van uw printjobs wil krijgen.

&EN    1. Kiest u voor "Windows Printer" dan wordt alles afgedrukt op uw
       "default" Windows-printer.

&EN     2. Wil u de output van uw printjobs in Frame, Word of het WWW gebruiken
       kies dan voor de "File"-optie. Elke keer dat u afdrukt, vraagt IODE
       de naam en het formaat waarin u uw output wil terugvinden.

    U zal merken dat de "windows" waarin u de printopdracht geeft lichtjes
    zijn veranderd. Het veld waarin de naam van de A2M-file stond is om
    evidente redenen verdwenen. Wil u toch nog A2M-files maken, geen nood.
    Eén van de formaten waaronder u kan printen is good-old A2M.

&TI Grafieken in A2M
————————————————

    Vanaf de versie 4.80 van IODE kan u naast tabellen, IODE-objecten en
    tekst ook grafieken afdrukken of omzetten naar MIF. Het A2M-formaat
    werd uitgebreid met een nieuw object, nl. de grafiek.

    Het is nu ook mogelijk om met linker- en rechterassen te werken. U
    bepaalt voor elke lijn in een grafiek naast het type (bar, line, ...)
    ook de te gebruiken as. U oude grafieken zijn nog steeds bruikbaar,
    per "default" wordt alleen de linkeras gebruikt.


    Voor het afdrukken van grafieken via namen van variabelen, gebruikt u
    het $DataPrintGraph commando.
&CO
    $DataPrintGraph   {Level | Diff | Grt}
		      {Line | Scatter | Bar | Mixt}
		      {NoXGrids | MinorXGrids | J(MajorXGrids)}
		      {NoYGrids | MinorYGrids | J(MajorYGrids)}
		      {Level | G(Log) | Semi-Log | Percent}
		      {ymin | --} {ymax | --}
		      perfrom perto varname1 varname2 ...
&TX
    Voor het afdrukken van grafieken via tabellen, gebruikt u
    het $PrintGr commando.
&CO
    $PrintGr gsample table_names
&TX

    Reeds bestaande A2M-bestanden blijven compatibel.

&TI Printen vanuit een Rapport
——————————————————————————

    Bij het uitvoeren van een rapport moet u aangeven onder welke vorm
    en in welke file u uw output wil recuperen. Per default wordt niets
    afgedrukt.

    Om vanuit een rapport gebruik te kunnen maken van de nieuwe print-
    mogelijkheden, werden de "PrintDest" en "PrintDestNew" commando's
    aangepast. U vroegere rapporten moet u niet aanpassen, zij zullen
    A2M-bestanden blijven aanmaken.

&IT    Vroeger
    ———————
&CO
	$PrintDest filename (vb. $PrintDest tabel.a2m)
&TX
&IT    Nu
    ——
&CO
	$PrintDest
&TX
		print alles wat volgt af op uw default-Windows printer
&CO
	$PrintDest filename [format]
&TX
		print alles wat volgt af in "filename" onder het gewenste
		formaat.

		format = A2M, MIF, HTML, RTF, als geen formaat wordt
		gespecifieerd, is A2M het gekozen formaat

&TI Wisselwerking EXCEL en IODE-rapporten
—————————————————————————————————————
    Nieuwe rapportfuncties in IODE laten u toe om IODE-objecten weg
    te schrijven naar Excel, gegevens uit Excel te gebuiken om IODE
    data aan te passen, etc...

&CO
    $ExcelOpen "filename"
&TX
	Opent in Excel de file. Als Excel niet draait en het pad naar
	Excel.exe staat in uw systeemvariabele PATH an wordt Excel
	opgestart.
	(vb: $ExcelOpen "c:\usr\iode\test.xls")
&CO
    $ExcelSave
&TX
	Slaat de openstaande workbook op


&CO
    $ExcelPrint
&TX
	Print het openstaande workbook

&CO
    $ExcelSet{Obj} objnaam [sample] excelrange
&TX
	Schrijft het IODE-obj "objname" in een Excel-spreadsheet
	in de aangeduide range

	(vb: $ExcelSetVar AAA Sheet1!R1C1
	     $ExcelSetCmt AAA Sheet1!R1C1
	     $ExcelSetTbl AAA 90:11 Sheet1!R1C1)

&CO
    $ExcelGetVar varname [begin period] rangname
&TX
	Haalt uit de Excel-range de waarden voor de update van "varname".
	Wordt de "begin period" niet aangegeven, dan wordt de begin periode
	van de sample aangenomen.


&TI XODE in IODE
————————————
    U kan alle functies uit XODE in IODE gebruiken. Als u bij de "Save" of
    de "Load" van een file een ASCII-extensie gebruikt (bijv. ac voor
    commentaarbestanden) dam wordt een IODE-ascii bestand weggeschreven of
    gelezen. U kan ook vanuit een rapport ASCII-bestanden im- of exporteren

&CO
    Syntax : $WsImportObj filename
	     $WsExportObj filename
&TX
	bijv. $WsImportVar test.ac

    Onder het menu-punt "File" vindt de Import en Export (by rule) functies
    van XODE terug.

&TI RAPPORTS
————————
    Nouvelles fonctions dans les rapports :

&EN ~c$Minimize~C : minimise la fenêtre de IODE (par exemple pour
		    afficher des graphiques de Excel générés par
		    la simulation en cours)
&EN ~c$Maximize~C : restaure la fenêtre de IODE
&EN ~c$Sleep~C nn : arrête le processus pendant nn millièmes de seconde.
		    Cela permet à un client ou un serveur DDE de
		    conserver le contrôle en cas de requêtes trop
		    rapides.
&TI CORRECTION
——————————
    Erreur lors de la destruction d'un objet (tableau par exemple)
    corrigée.

&TI RAPPORTS
————————
    La fonction ~c$EqsEstimate~C imprime les graphiques des observations
    et des valeurs estimées.
>

<Version 4.73>
    Version 4.73 (08/08/1997)
    —————————————————————————
&TI Calcul de taux de croissance moyens
———————————————————————————————————
    Lorsqu'une valeur négative résulte de la division de la période
    de fin par la période de début dans un calcul de taux de croissance
    moyen, le calcul de la racine de cette valeur 1/n est impossible.
    Dans ce cas, la valeur retournée est --, alors qu'auparavant, le
    programme avait un comportement erratique.
>
<Version 4.71>
    Version 4.71 (20/05/1997)
    —————————————————————————

&TI IMPRESSIONS
———————————
    Le nombre de caractères prévus pour les valeurs des tableaux et des
    coefficients est augmenté.

&TI *ODEW32
———————
    Amélioration de la gestion de l'utilisation CPU de la machine.
    Cette correction améliore sur certaines configurations les
    performances d'accès aux réseaux.
>
<Version 4.72>
    Version 4.72 (17/06/1997)
    —————————————————————————

La version 4.72 contient trois groupes de fichiers supplémentaires :

&EN Version IODE double précision
&EN WS APL d'interface avec IODE
&EN Programme A2M pour Windows 95

&TI VERSION DOUBLE PRECISION
————————————————————————
    Les données numériques (séries, coefficients, ...) sont gérées en
    IODE en simple précision, c'est-à-dire avec une précision de 8
    chiffres significatifs.

    Dans certains cas, cette limite est "un peu courte". A cette fin,
    une version double précision (16 chiffres significatifs) a été
    développée. Les programmes de cette version (uniquement win32) sont
    IODEW32d, XODEW32d, GODEW32d et TODEW32d.

    Pour pouvoir exploiter ces programmes, il faut d'abord exporter en
    ASCII tous les fichiers nécessaires, à l'exception des commentaires
    et des listes. Utilisez pour cela XODEW32. Les fichiers de
    variables, de tableaux, d'identités et d'équations contenant des
    informations numériques, ils doivent être traduits avant exploitation.

    Il faut ensuite importer ces fichiers en double précision à l'aide
    de XODEW32d. On constatera évidemment que les fichiers peuvent
    doubler de taille.

    Ensuite, IODEW32d peut être utilisé exactement comme IODEW32.


&TI WS APL d'interface avec IODE
————————————————————————————
    Les fonctions de ce WS sont affichées lors de son chargement.

    Elles permettent :

&EN de piloter IODE à partir de l'APL
&EN de charger des données de IODE vers APL
&EN de copier les données de l'APL vers IODE

    Pour que cela fonctionne, il faut démarrer le programme IODEW32 ou
    IODEW32d. Plus d'explications seront fournies dans un document
    séparé.


&TI PROGRAMME A2M pour WIN95
————————————————————————
    Un programme permettant d'imprimer ou de traduire des fichiers a2m
    en différents langages est distribué sur les disquettes de IODE.

    Ce programme, indépendant de IODE, peut être lancé n'importe quand
    et exploité en cours de travail dans IODE. Il ne fonctionne que sous
    Windows 95.

    A2m contient également une série d'utilitaires de manipulation de
    fichiers HTML.

&TI DDE SERVER
——————————
    Un serveur DDE est implémenté dans IODEW32 et IODEW95. Cela
    signifie qu'il est possible d'exploiter les fonctions et les
    données de IODE dans le cadre d'autres programmes, comme
    Excel ou APL.

    La version actuelle est encore une version limitée: toutes les
    possibilités exploitables ne sont donc pas implémentées. En fonction
    des demandes futures, d'autres fonctions pourront être définies.

&IT    Utilisation en Excel
    ————————————————————
    Par exemple, on peut placer dans une cellule d'un tableau Excel
    la référence à une variable de IODE sous le format suivant :
&CO
	=IODE|XVAR!'SER1!Sheet1!R2C2'
&TX
    La valeur de la série sera placée à la position demandée.

&IT    Utilisation en APL
    ————————————————————
    En APL, un WS IODE a été programmé pour permettre de lancer des
    commandes de rapport à partir d'une fonction APL, charger en APL des
    données de IODE et replacer ces données après modification dans
    l'environnement IODE. Une fonction permet de piloter complètement
    IODE à partir de l'APL.

&IT    Fonctions du serveur DDE
    ————————————————————————
    Les données techniques qui suivent permettent d'exploiter le
    serveur DDE dans le cadre d'autres programmes.
&CO
    Service : IODE
    Topics  : WS (Request et Poke), REP (Poke), PLAY (Poke), XVAR, XCMT
	      CMT, EQS, IDT, LST, SCL, TBL, VAR (Request et Poke)

     1. Request
     ----------
	Items de WS
	-----------
	     SAMPLE = sample du WS IODE courant

	     CLIST  = liste des commentaires du WS IODE courant
	     ELIST  = liste des équations du WS IODE courant
	     ILIST  = liste des identités du WS IODE courant
	     LLIST  = liste des listes du WS IODE courant
	     SLIST  = liste des scalaires du WS IODE courant
	     TLIST  = liste des tableaux du WS IODE courant
	     VLIST  = liste des variables du WS IODE courant

	     CNAME  = nom du fichier des commentaires de IODE
	     ENAME  = nom du fichier des équations de IODE
	     INAME  = nom du fichier des identités de IODE
	     LNAME  = nom du fichier des listes de IODE
	     SNAME  = nom du fichier des scalaires de IODE
	     TNAME  = nom du fichier des tableaux  de IODE
	     VNAME  = nom du fichier des variables de IODE

	     CDESCR = description du fichier des commentaires de IODE
	     EDESCR = description du fichier des équations de IODE
	     IDESCR = description du fichier des identités de IODE
	     LDESCR = description du fichier des listes de IODE
	     SDESCR = description du fichier des scalaires de IODE
	     TDESCR = description du fichier des tableaux  de IODE
	     VDESCR = description du fichier des variables de IODE

	     CNB    = nombre de commentaires dans IODE
	     ENB    = nombre de équations dans IODE
	     INB    = nombre de identités dans IODE
	     LNB    = nombre de listes dans IODE
	     SNB    = nombre de scalaires dans IODE
	     TNB    = nombre de tableaux  dans IODE
	     VNB    = nombre de variables dans IODE

	Items de CMT, EQS, IDT, LST, SCL, TBL, VAR
	------------------------------------------
	     L'item est le nom de l'objet. La valeur retournée
	     est la définition de l'objet.

	Items de XVAR
	-------------
	    L'item est composé de trois éléments séparés par des ! :
		- nom de la ou des variables séparés par des virgules ou
		    des blancs. t représente une ligne de périodes
		- nom du sheet où placer le résultat
		- range de la première cellule dans ce sheet

	    Cette requête génère un Poke vers excel avec la définition
	    des variables et leur code.

	    Si aucun nom de série n'est donné, une série avec les
	    valeurs des périodes précède. Sinon, on peut placer
	    les périodes en indiquant la série t (minuscule).

	    S'il n'y a pas de sheet, Sheet1 est choisi comme destination
	    S'il n'y a pas de range, R1C1 est choisi comme destination

	    Exemples
	    --------
		IODE|XVAR!' ' : toutes les séries dans le sheet1, en R1C1
				avec les périodes
		IODE|XVAR!'t,QC0,QAFF!Sheet2!R1C1' : séries QCO et QAFF
				dans le Sheet2 en position R1C1, avec
				les périodes en première ligne
		IODE|XVAR!'QC0!Sheet2!R1C1' : série QCO sans les périodes

	Items de XCMT
	-------------
	    L'item est composé de trois éléments séparés par des ! :
		- nom de la ou des commentaires séparés par des virgules ou
		    des blancs
		- nom du sheet où placer le résultat
		- range de la première cellule dans ce sheet

	    Cette requête génère un Poke vers excel avec la valeur des
	    commentaires et leurs codes.

	    S'il n'y a pas de sheet, Sheet1 est choisi comme destination
	    S'il n'y a pas de range, R1C1 est choisi comme destination

	    Exemples
	    --------
		IODE|XCMT!' ' : tous les commentaires dans le sheet1,
				en R1C1
		IODE|XVAR!'QC0,QAFF!Sheet2!R1C1' : comments QCO et QAFF
				dans le Sheet2 en position R1C1
		IODE|XVAR!'QC0!Sheet2!R5C2' : comment QCO en Sheet2, ligne
				5 colonne 2

     2. Poke
     -------
	Items de WS (Data en fonction du contexte)
	-----------
	     SAMPLE = change le sample du WS IODE courant (format IODE)

	     CNAME  = change le nom du fichier des commentaires de IODE
	     ENAME  = change le nom du fichier des équations de IODE
	     INAME  = change le nom du fichier des identités de IODE
	     LNAME  = change le nom du fichier des listes de IODE
	     SNAME  = change le nom du fichier des scalaires de IODE
	     TNAME  = change le nom du fichier des tableaux  de IODE
	     VNAME  = change le nom du fichier des variables de IODE

	     CDESCR = change la description des commentaires de IODE
	     EDESCR = change la description des équations de IODE
	     IDESCR = change la description des identités de IODE
	     LDESCR = change la description des listes de IODE
	     SDESCR = change la description des scalaires de IODE
	     TDESCR = change la description des tableaux  de IODE
	     VDESCR = change la description des variables de IODE

	     CCLEAR = Clear des commentaires dans IODE
	     ECLEAR = Clear des équations dans IODE
	     ICLEAR = Clear des identités dans IODE
	     LCLEAR = Clear des listes dans IODE
	     SCLEAR = Clear des scalaires dans IODE
	     TCLEAR = Clear des tableaux dans IODE
	     VCLEAR = Clear des variables dans IODE

	     CLOAD = Load un fichier de commentaires dans IODE
	     ELOAD = Load un fichier d'équations dans IODE
	     ILOAD = Load un fichier d'identités dans IODE
	     LLOAD = Load un fichier de listes dans IODE
	     SLOAD = Load un fichier de scalaires dans IODE
	     TLOAD = Load un fichier de tableaux dans IODE
	     VLOAD = Load un fichier de variables dans IODE

	     CSAVE = Sauve le fichier de commentaires de IODE
	     ESAVE = Sauve le fichier d'équations de IODE
	     ISAVE = Sauve le fichier d'identités de IODE
	     LSAVE = Sauve le fichier de listes de IODE
	     SSAVE = Sauve le fichier de scalaires de IODE
	     TSAVE = Sauve le fichier de tableaux de IODE
	     VSAVE = Sauve le fichier de variables de IODE

	Items de PLAY
	-------------
	     TEXT : data = texte à placer dans le buffer clavier
		    de IODE
	     KEYS : data = texte des touches à placer dans le buffer
		    clavier : aA..aZ, aF1..aF10, cA..cZ, cF1..cF10,
			      sF1..sF10, ESC, ENTER, BACKSPACE,
			      HOME, END, LEFT, RIGHT, UP, DOWN,
			      PGUP, PGDN, DEL, TAB

	Items de CMT, EQS, IDT, LST, SCL, TBL, VAR
	------------------------------------------
	    Nom de l'objet à modifier.
	    Data contient la valeur avec des TAB comme séparateur
	    dans le cas des variables.

	Items de REP
	------------
	    La valeur de l'item est sans signification.
	    La valeur de data est une commande de rapport à exécuter
	    (ex. '$WsLoadVar test')
&TX
>

<Version 4.70>
    Version 4.70 (10/03/1997)

&TI Génération de fichiers HTML


&IT   Paramètres de QODE

    De nouveaux paramères se sont ajoutés au programme QODE pour
    affiner la génération des fichiers HTML :

&EN -toc tit_level : indique s'il faut générer une table des
	 matières au début du document et jusqu'à quel niveau de titre

&EN -parb tit_level et

&EN -pari tit_level : indique s'il faut donner un niveau de
	    titre aux paragraphes parb et pari (on donc éventuellement
	    les introduire dans la table des matières

&IT    HTML généré
    ———————————
    En début et en fin du fichier HTML se trouvent deux commentaires
    qui permettent d'effectuer un remplacement automatique dans le
    fichier, afin par exemple de placer une barre navigationnelle
    standardisée en haut et en bas du document.

    Ces commentaires sont :
&CO
	<<!STARTHEADER>>
	<<!ENDHEADER>>

	<<!STARTFOOTER&>>
	<<!ENDFOOTER>>
&TX
    Le programme scr4_sbs permet de substituer ces textes par le contenu
    d'un fichier. La commande à effectuer est :
&CO
	scr4_sbs "<<STARTHEADER>>" "<<!ENDHEADER>>" replacefile
		file.htm
&TX
    Elle a pour effet de remplacer dans le fichier file.htm tout ce qui
    se trouve entre ~c<<!STARTHEADER>>~C et ~c<<!ENDHEADER>>~C,
    ces textes compris, par le contenu du fichier replacefile.

&TI Graphiques
——————
    Correction de l'affichage et de l'impression des graphiques
    à partir des tableaux. Le buffer contenant le titre du tableau est
    alloué dynamiquement: auparavant, un titre de plus de 80 caractères
    plantait le programme.

&TI Fichier printcap for WinNFS de Network Instruments
———————————————————————————————————
    Le fichier printcap.ni contient les instructions nécessaires
    à l'impression via IODE32 ou QODE sous Windows 95. EN effet,
    le logiciel Winsock Companion de Network Instruments est utilisé
    en lieu et place de PC-NFS sous WIndows 95, ce qui exige une autre
    configuration.

    Attention, à la différence des la version PC-NFS, les imprimantes
    doivent être préalablement définies dans l'environnement Windows
    pour pouvoir être utilisées.

&TI Corrections de bugs
———
    Les variables ~c_YOBSi~C, ~c_YCALCi~C et ~c_YRESi~C générées
    automatiquement lors des estimations étaient parfois absentes. Cette
    erreur est corrigée.

&TI QODE : génération de fichier MIF

    La traduction de fichiers a2m en format MIF était impossible
    si la largeur d'une colonne (la première) était supérieure à
    60 caractères. Cette limite est portée à 400 caractères.

    La limite à 60 caractères est cependant conservée pour les outputs
    en fichier formattés Ascii.
>
<Version 4.69>
    Version 4.69 (12/09/1996)
    —————————————————————————

..esc ~

&TI WsCopyVar, HighToLow, LowToHigh, Census X11
———————————————————————————————————————————
    Ces fonctions sont corrigées : en utilisation hors des
    rapports, elles plantaient IODE (toutes versions).

&TI Génération des tableaux
———————————————————————
    Correction dans le calcul lorsque le diviseur est non LEC.

&TI Simulation
——————————
    La version DOS 32 bits a été quelque peu accélérée.

&TI Estimation
——————————
    Les tests résultant de la dernière estimation sont sauvés dans
    des scalaires de façon à permettre différents calcul par la
    suite. Il en va de même pour les résidus, membres de gauche et
    de droite des équations.

    Les tests portent les noms suivants (~ce0_*~C pour la première équation
    du bloc, ~ce1_*~C pour la deuxième, ...) :

&EN        ~ce0_n       ~C  : nombre de période du sample
&EN        ~ce0_k       ~C  : nombre de coefficients estimés
&EN        ~ce0_stdev   ~C  : std dev des résidus
&EN        ~ce0_meany   ~C  : moyenne de Y
&EN        ~ce0_ssres   ~C  : somme du carrés des résidus
&EN        ~ce0_stderr  ~C  : std error
&EN        ~ce0_stderrp ~C  : std error %
&EN        ~ce0_fstat   ~C  : F-Stat
&EN        ~ce0_r2      ~C  : R carré
&EN        ~ce0_r2adj   ~C  : R carré ajusté
&EN        ~ce0_dw      ~C  : Durbin-Watson
&EN        ~ce0_loglik  ~C  : Log Likelihood

    Les séries calculées sont également sauvées dans une variable
    sous les noms :

&EN  ~c_YCALC0~C pour le membre de droite de la première équation du
	  bloc, ~c_YCALC1~C pour la deuxième équation, etc.

&EN  ~c_YOBS0~C pour le membre de gauche de la première équation du
	  bloc, ~c_YOBS1~C pour la deuxième équation, etc.

&EN  ~c_YRES0~C pour les résidus de la première équation du bloc, ...

    En dehors du sample d'estimation, les valeurs de la série sont --.
>

<Version 4.68>
    Version 4.68 (16/07/1996)
    —————————————————————————

&TI HTML
————
&EN Correction dans le cas des caractères spéciaux ayant une
      signification particulière en HTML (<</A>>, ...). Ces caractères
      étaient transformés auparavant.

&EN Le format des tableaux est allégé : les lignes verticales
      et horizontales sont supprimées. De plus, les cellules de
      type LINE génèrent une ligne intercalaire dans le tableau
      en HTML.

&EN L'alignement des nombres se fait à droite et plus sur
      le point décimal. En effet, Netscape ne gère pas correctement
      l'alignement décimal.

&TI Estimation
——————————
    L'impression des résultats des estimations ne génère plus de
    tableaux pour éviter les sauts de pages inutiles en Postscript.

&TI Print/View Variables
————————————————————
    Il est dorénavant possible d'afficher des valeurs résultant de
    formules contenant l'opérateur * ou contenant des virgules (min,
    vmax, ...).
    Cependant, les séparateurs entre les formules sont dorénavant
    limités aux blancs, points-virgules et retour ligne.

&TI Noms de fichiers
————————————————
    A la demande de plusieurs utilisateurs, la longueur des noms de
    fichiers est portée dans la plupart des écrans à un minimum de 40
    caractères.
    Le prix à payer est que les "profiles" des versions antérieures
    ne sont plus compatibles avec la version 4.68.

&TI Fichiers a2m
————————————
    Dans les écrans où l'on permet d'"imprimer" dans un fichier a2m,
    il est possible de spécifier si l'on souhaite que le fichiers
    soient vidés avant l'impression. Cela permet d'éviter que
    des fichiers d'output accumulent des impressions successives.

    Notons que cette option est liée à la fonction de rapport
    $PrintDestNew (voir 4.67).

&TI Execute identities
——————————————————
    Le fichier "trace" optionnel est réinitialisé à chaque exécution.

&TI Simulation Debug
————————————————
    L'option debug de la simulation génère automatiquement des listes
    comprenant des équations pré- et post-récursives.
    Dans cette version, un fichier de trace est également généré. Ce
    fichier se nomme "simul.dbg" et se trouve dans le directory
    d'où IODE a été lancé.
    Le fichier de trace comprend actuellement la liste des équations
    pour lesquelles une sous-itération (Newton-Raphson) a été
    nécessaire et le nombre de sous-itérations.

&TI Remarques
—————————
    Dans les rapports, les expressions du type
&CO
	    $Goto label {X[1975Y1] << 2}
&TX
    ne fonctionnent que si la période est fixée par la fonction
&CO
	    $SetTime
&TX
    Si ce n'est pas le cas, l'expression (même si l'année est fixée
    comme dans l'exemple) ne s'exécutent pas.
>
<Version 4.67> (12/07/1996)
    Version 4.67 (12/07/1996)
    —————————————————————————

&TI Versions Windows
————————————————
&EN Meilleure gestion de la souris.
&EN Stop possible plus souvent.

&TI HTML
————
    Il est possible de spécifier un fichier HTML comme sortie de
      QODE ou TODE. Cette fonction génére du HTML 3.0 (y compris les
      tableaux).

&TI Critère de convergence Newton
—————————————————————————————
    Dans cette version, on a repris deux types de critères dans le
    cas d'une équation non résolue par rapport à l'endogène ou dans
    le cas du goal seeking. Dans la version 4.66, on avait renforcé
    les contraintes de convergence. Dans la version 4.67, en attendant
    un refonte de l'algorithme, on reprend le critère 4.66 (critère
    fort, comme dans KaA), mais en cas de non convergence par cette
    méthode, on reprend le critère antérieur (v.4.65).

&TI Rapports
————————
&IT    Nouvelle fonction $msg
    ——————————————————————

	Elle affiche le texte de l'argument et attend une touche avant
	de continuer. Cela permet de stopper momentanément le rapport
	pour afficher un message à l'intention de l'utilisateur,
	contrairement à la fonction $show qui affiche dans le bas de
	l'écran mais ne stoppe pas.

&CO
	    $msg texte libre
&TX
&IT    Macros
    ——————
	Les macros sont définies entre des pourcents : %FILE%. Si le
	premier pourcent est immédiatement suivi du caractère

&EN            ## : le contenu de la macro est transposé en majuscules
&EN            ! : le contenu de la macro est transposé en minuscules

	Par exemple,
&CO
	    ##define TOTO TBL1
	    Premier  tableau : %!TOTO%
	    Deuxième tableau : %##TOTO%
&TX
	Donnera à l'exécution :
&CO
	    Premier  tableau : tbl1
	    Deuxième tableau : TBL1
&TX
&IT    Noms des fichiers a2m
    —————————————————————
	Pour éviter les problèmes de noms de fichiers entre DOS et
	Unix, les noms des fichiers a2m définis par la commande
	$PrintDest sont toujours transposés en minuscules.

	Le nom des fichiers output peut atteindre 64 caractères au
	lieu de 40 auparavant.

&IT    Fonction FileDelete*
    ————————————————————
	Le fichier destination de l'output est fermé avant toute
	destruction de fichier pour éviter les erreurs dans les
	rapports du type suivant :
&CO
	    $PrintDest res.a2m

	    $Label Again
	    $FileDeleteA2m res        (le fichier est ouvert!)
	    ...
	    $Goto Again
&TX
&IT    Nouvelle fonction $PrintDestNew
    ———————————————————————————————
	Cette fonction détruit le fichier output avant de commencer
	à l'utiliser. Cela évite de créer de gigantesques fichiers
	accumulant tous les résultats successifs.
>

<Version 4.66> (17/01/1996)
    Version 4.66 (17/01/1996)
    —————————————————————————

&TI Critère de convergence Newton
—————————————————————————————
    Lorsqu'une équation n'est pas résolue analytiquement par IODE
    (endogène répétée dans l'équation), un sous-algorithme de Newton est
    utilisé pour obtenir la convergence de cette équation.

    En raison du manque relatif de précision dans les calculs, cette
    sous itération peut dans certains cas ne pas converger correctement.

    Dans cette version, la contrainte de convergence est plus
    restrictive qu'auparavant, s'alignant de la sorte sur celui de KaA.
    Par conséquent, il est possible que certaines équations ne
    convergent plus dans cette nouvelle version.

    Dans une version en préparation, tout ce problème sera revu et de
    nouveaux algorithmes seront proposés.

&TI Noms des fichiers (Load)
————————————————————————
    Un erreur se produisait lors du chargement d'un fichier dont le nom
    était du type "../nom". Le message "file not found or incompatible
    type" était alors produit. Cette erreur est corrigée.

&TI Profiles
————————
    Les profils qui permettent de retrouver d'une session à l'autre les
    mêmes écrans sont standardisés (DOS 16 bits ou 32 bits).
>


<Versions 4.64-65> (17/10/1995)
    Versions 4.64-65 (17/10/1995)
    —————————————————————————————

&TI Qode (Postscript)
—————————————————

Le driver Postscript incorporé dans QODE et QODE32 a été adapté aux
imprimantes HP4m+ et HP4mv.

Bureau du Plan : les fichiers printcap.nfs et printcap sont adaptés
		 aux nouvelles imprimantes réseau.

>

<Version 4.63> (02/10/1995)
    Version 4.63 (02/10/1995)
    —————————————————————————

&TI Tableaux en Mif
———————————————

    Une simplification de l'importation en Frame des tableaux (QODE) a
    été apportée dans cette version : auparavant, il fallait disposer du
    fichier a2m.m4 dans le directory du fichier mif pour pouvoir ouvrir
    avec succès en Frame un fichier MIF contenant un tableau
    (et encore...).
    Typiquement, si ce n'était pas le cas, on obtenait un document Frame
    vide.

    Dorénavant, l'importation des tableaux répond aux règles suivantes :

&EN        il ne faut plus de fichier a2m.m4
&EN        il n'y a plus de lignes en haut et en bas du document,
	  héritage des versions 1.0 et 1.3 de Frame
&EN        les tableaux générés sont de type "Format A", formt défini dans
	  le catalogue de tableaux de tout fichier Frame

    Pratiquement, pour importer des tableaux en Frame, on procède
    comme suit :

&EN        créer les tableaux dans un fichier a2m en IODE ou IODE32
	    (Print Tables)
&EN         "Quick print a2m file" vers un fichier mif
&EN         lancer Frame
&EN         deux possibilités ensuite :

&EN2             ouvrir le fichier mif (File/Open)
&EN2             importer le fichier mif dans un document existant
		(File/Import/File)

    Dans le dernier cas (File/Import), on peut exploiter un format
    prédéfini : il suffit de définir dans un template de Frame
    un type de tableau "Format A" selon ses desiderata.
>


<Version 4.62> (29/09/1995)
    Version 4.62 (29/09/1995)
    —————————————————————————
&TI Versions ?ODE32
———————————————
    Une erreur s'est glissée dans le portage de la version 4.61 en 32 bits.
    L'interprétation des valeurs non entières (dans les fichiers
    ASCII, les équations et les identités) était erronée.
>

<Version 4.61> (02/09/1995)
    Version 4.61 (02/09/1995)
    —————————————————————————
&TI Regroupement de fichiers
————————————————————————
    Le nombre de fichiers distribués dans IODE est considérablement
    diminué : les exécutables de IODE intègrent toutes les informations
    nécessaires à leur fonctionnement, exception faite des fichiers
    d'aide et du fichier printcap de définition des imprimantes locales.
>

<Version 4.60> (15/05/1995)
    Version 4.60 (15/05/1995)
    —————————————————————————
&TI LEC : variable i
————————————————
    Nouvelle variable dans le langage LEC permettant de connaître lors
    d'un calcul la différence entre l'année de calcul d'une
    sous-expression par rapport à l'année de calcul de toute la formule.
    Cette variable s'appelle i et son comportement s'apparente à celui
    de t. Elle est donc invariante par rapport à l'année de calcul d'une
    formule LEC.

    On calcule toujours une forme LEC pour une valeur du temps t donnée,
    mais dans une sous-expression, t peut être différent. C'est par
    exemple le cas pour les opérateurs d(), dln(), sum(), ...
    On peut, grace à i, connaître la différence entre le t de calcul de la
    formule et le t de calcul de la sous-expression courante.

    On peut donc calculer des expressions comme
&CO
	sum(t-2, t-4, A / (1 - i)**2),
&TX
    qui équivaut à :
&CO
	A[-2]/(1-(-2))**2 + A[-3]/(1-(-3))**2 + A[-4]/(1-(-4))**2
&TX
    Sans i, une telle expression ne peut être écrite en LEC.

    Cet opérateur a été implémenté pour permettre la traduction de
    l'opérateur SUM de TROLL.

    Ainsi, si on calcule une expression pour un t donné, i prend les
    valeurs suivantes selon les cas. Attention, les expressions
    indiquées ci-dessous sont les formules complètes, et pas
    une sous-expression!

&CO
	- A + i == A + 0
	- d(A+i) == A + 0 - (A[-1] + -1)
	- l(i+1, A) == "A[-(i+1)]" == "A[-(0+1)]" == A[-1]
	- sum(t-1, t-2, i**2) == (-1)**2 + (-2)**2
	- sum(t-1, t-2, l(i-2, A) / i**2) ==
	    l((-1)-2, A) / (-1)**2 +
	    l((-2)-2, A) / (-2)**2        ==
		l(-3, A) / 1 +
		A[+3] + A[+4] / 4
&TX
&TI High to Low
———————————
    Correction dans le cas des fichiers input dont la dernière observation
    fait partie du calcul des séries output : un fichier 93M1:94M12
    donnait la valeur NA pour l'année 94.

>

<Version 4.59> (06/03/1995)
    Version 4.59 (06/03/1995)
    —————————————————————————
&TI Tableaux
————————
    Correction de la fonction de création automatique (rapports).

&TI Listes
——————
    Toutes les listes générées ont pour séparateur le point-virgule au
    lieu de la virgule.

    Lors de l'importation de listes via XODE, les listes avec
    séparateur point-virgule sont correctement transférées et non
    plus scindées en plusieurs lignes séparées n'importe où.

>

<Version 4.58> (13/02/1995)
    Version 4.58 (13/02/1995)
    —————————————————————————
&TI Simulation
——————————
    Les valeurs initiales des endogènes sont calculées également
    pour les blocs pré- et post-récursifs. Auparavant, seuls les
    endogènes du bloc interdépendant étaient initialisées.

&TI Tableaux
————————
    La création automatique des tableaux s'est enrichie de deux nouvelles
    fonctionnalités : au niveau des rapports et au niveau de la création
    manuelle d'un tableau.

&IT    1. Dans les rapports
    ————————————————————
	La fonction $DataUpdateTbl a la syntaxe suivante :
&CO
	    $DataUpdateTbl table_name title;lec1;lec2;...
&TX
	Si title est un nom de commentaire, le commentaire est utilisé
	comme titre.
	De même, si on trouve des noms de variables comme forme lec, et
	qu'il existe un commentaire pour ces variables, le titre de
	la ligne correspondante est remplacé par la valeur du
	commentaire.

&IT        Exemple
	———————
	Supposons qu'il existe en WS un commentaire pour A et non pour
	B, et un commentaire TIT :
&CO
	    Commentaire A   : "Produit national brut"
	    Commentaire TIT : "Titre de mon tabelo"
&TX
	La ligne
&CO
		$DataUpdateTbl T1 TIT;A;B;A+B
&TX
	crée le tableau T1 avec la forme suivante :
&CO
	    ———————————————————————————————————————————È
	    ­          Titre de mon tabelo              ­
	    ­                                           ­
	    ­  Produit national brut               A    ­
	    ­  B                                   B    ­
	    ­  A+B                                 A+B  ­
	    Ë———————————————————————————————————————————€
&TX
&IT    2. Création manuelle
    ————————————————————
	Les mêmes règles s'appliquent dans la création d'un tableau. De
	plus, il est possible d'utiliser des listes dans le champ de
	définition des formes LEC.


>


<Version 4.57> (25/01/1995)
    Version 4.57 (25/01/1995)
    —————————————————————————
&TI Estimation
——————————
    1. L'écran de définition des équations reprend dorénavant les
       valeurs des tests avec 3 décimales, y compris dans le cas
       des blocs d'équations. Ces mêmes tests sont sauvés dans
       toutes les équations du bloc.

    2. L'impression des coefficients se fait en format décimal dans
       tous les cas, alors qu'auparavant le format dépendait de
       la valeur (1e-03 au lieu de 0.001).

>


<Version 4.56> (02/12/1994)
    Version 4.56 (02/12/1994)
    —————————————————————————
&TI Xode
————
&EN (Ré-)Adaptation de XODE pour la lecture des données des fichiers NIS.
&EN  Correction de la lecture des tableaux ascii.

&TI Iode/Tableaux
—————————————
    Le nombre maximum de colonnes d'un tableau est porté à 20. Rappelons
    cependant que la taille totale d'un objet ne peut dépasser 16000
    bytes (texte, formules LEC, données internes). Plus le nombre de
    colonnes est élévé, plus le nombre de lignes devra être limité !

>

<Version 4.55> (28/10/1994)
    Version 4.55 (28/10/1994)
    —————————————————————————
&TI Xode
————
    Adaptation au nouveau format des fichiers NIS.

&TI Fode
————
    Amélioration de la version batch du programme. La nouvelle syntaxe
    est :
&CO
	fode[32] -i file ... -o outputfile
&TX
    Dans le cas où fode est lancé sans paramètre, il fonctionne
    comme avant en mode interactif.
    Sinon, les fichiers suivant -i sont traités et le résultat
    est placé dans le fichier ASCII défini par le paramètre -o.

    Amélioration de la présentation en colonnes lorsque le texte
    d'une colonne s'étend sur plusieurs lignes.

>

<Version 4.54> (23/08/1994)
    Version 4.54 (23/08/1994)
    —————————————————————————
&TI Editeur MMT
———————————
    Le sauvetage d'un fichier édité à l'aide de l'éditeur intégré MMT
    plaçait des tabulations en début de lignes lorsqu'au moins 8
    blancs étaient présents. Lors de l'impression d'un tel fichier
    (par exemple dans le cadre de FODE), ces blancs de début de lignes
    semblaient - sur certaines imprimantes - être ignorés. En fait,
    les imprimantes ne représentent pas toutes les tabulations de la
    même manière, ce qui explique les éventuels problèmes.

    Pour pallier ces inconvénients, l'éditeur intégré MMT sauve
    dorénavant tous les blancs comme tels, sans remplacer les suites
    de 8 blancs par des tabulations.


    Attention : cette modification n'est valable que dans IODE, TODE,
		FODE, etc, mais pas dans le programme indépendant MMT.

&TI Xode
————
    Rules file : une ligne blanche intercalaire arrêtait le processus de
    lecture. Dorénavant les lignes blanches sont ignorées.

&TI Xode : Export by rule
—————————————————————

    Cette nouvelle option permet de générer des fichiers
    de données en formats externes : Comma separated values (CSV), TSP,
    WKS (Lotus/Excel), DIF.

    Le nom du fichier de variables est requis.

    Le fichier des "rules" permet de limiter les variables à exporter et
    éventuellement d'en changer les noms.

    Si un fichier de commentaires est indiqué, chaque variable est
    accompagnée der son commentaire.

    Si le sample n'est pas précisé, toutes les observations sont
    fournies dans le fichier résultat.

    Dans le cas du format CSV, deux éléments sont libres : le séparateur
    de colonnes (par défaut ;) et la représentation de la valeur
    inconnue (par défaut ##N/A).

    Dans le cas des fichiers WKS, un "Label" est associé à chaque range
    de valeurs, ce label étant identique au code de la série.

&TI Xode
————
    La fonction d'importation de XODE accepte les valeurs des tests
    statistiques de l'estimation sous forme d'un mot-clé et de la
    valeur du test. Par exemple :
&CO
	STDEV 0.228351
	MEANY 2.68489
	SSRES 0.00182236
	STDERR 0.0142297
	FSTAT 2566.21
	R2 0.996505
	R2ADJ 0.996117
	DW 0.512136
	LOGLIK 32.272
&TX
    Ces informations sont générées automatiquement par la fonction
    d'exportation de XODE.
>

<Version 4.53> (06/06/1994)
    Version 4.53 (06/06/1994)
    —————————————————————————
&TI Rapports
————————
    La fonction $ReportExec accepte une liste comme argument :
&CO
	    $ReportExec test $LISTE
&TX
    reçoit comme argument les éléments de la liste $LISTE et permet
    donc d'exécuter un rapport type sur un nombre initialement
    inconnu d'arguments. Par exemple, pour remplacer tous les 0 par
    des NA pour la liste $LISTE de variables, il suffit de faire :


    FICHIER ZTONA.REP
    —————————————————
&CO
	$label again
	$goto fin {%0%=0}
	##show arg0 : %1%
	$DataCalcVar %1% if(%1%=0, 1/0, %1%)
	$shift
	$goto again
	$label fin
&TX
    APPEL
    —————
&CO
	$ReportExec ztona $LISTE
&TX
	ou, dans l'écran d'exécution :
&CO
	Report Name : ztona.rep
	Argument    : $LISTE
&TX

&TI Lec
———
    La fonction moyenne mobile
&CO
	ma(n, expr)
&TX
    n'existait que sous le nom
&CO
	mavg(, expr)
&TX
    suite à une erreur dans le manuel.

    Les deux sont dorénavant disponibles.

&TI Simulation
——————————
    Correction : lorsqu'une liste d'équations est introduite et que
    cette liste n'existe pas, un message d'erreur est produit. Auparavant,
    toutes les équations étaient simulées.

&TI Saisie de valeurs
—————————————————
    Dans l'édition manuelle des données (WS de variables ou via
    tableaux), quelques modifications ont été apportées pour améliorer
    l'interface :

&EN les 8 chiffres significatifs sont affichés
&EN si l'affichage de la valeur dépasse la taille de la cellule,
	  celle-ci est temporairement agrandie.
&EN lors de la saisie via tableau, les 8 chiffres significatifs
	  sont conservés

&TI Rappel
    ——————
	    Il est inutile d'espérer que plus de 8 chiffres significatifs
	    soient conservés dans les valeurs des séries : les valeurs
	    "float" affichent n'importe quoi au-delà du huitième chiffre.
	    Ainsi, la valeur 0.012345678 exploite toute la précision
	    disponible est introduire 0.012345678912 ne modifiera plus la
	    valeur machine du nombre.
>

<Version 4.52> (24/05/1994)
    Version 4.52 (24/05/1994)
    —————————————————————————

&TI Corrections
———————————
&EN Impressions des tableaux : correction de la séparation entre les
      colonnes.
&EN Rapports : comportement bizarre en version 16 bits
&EN Estimation : si le nombre de degrés de liberté est 0, les tests
	    sont corrigés

&TI Trend Correction
————————————————
    Implémentation de la méthode Hodrick-Prescott pour la construction
    de série de trend. Le principe est le même que pour la
    dessaisonnalisation : les séries lues dans un fichier sont importées
    et transformées simultanément.

    Une note technique de Bart Hertveldt décrit la méthode et son
    utilisation.

    Les fonctions de rapport ont la syntaxe suivante :
&CO
	$WsTrend VarFilename Lambda series1 series2 ...
	##WsTrend (interactive)
&TX
>

<Version 4.51> (15/04/1994)
    Version 4.51 (15/04/1994)
    —————————————————————————
&TI Impressions de tableaux
————————————————————
    La séparation entre les colonnes des tableaux en Postscript est
    ajustée pour permettre de placer plus de colonnes sur une même page.

&TI Edition de tableaux
————————————————————
    La dernière ligne de l'écran contient une indication sur les
    attributs de la cellule sélectionnée : cadrage et polices de
    caractères.
>

<Version 4.50> (18/03/1994)
    Version 4.50 (18/03/1994)
    —————————————————————————

&TI Lec
———
    Les formules faisant intervenir des lags négatifs (leads), du
    type l(-2, A), dln(-2, X+Y), retournaient un résultat incorrect en
    raison des arrondis des nombres négatifs.

    Cette erreur est corrigée et, par exemple, si A vaut 1, 2, 3, 4, ...,
    l(2, A) vaut 3, 4, ..., tandis que l(-2, A) vaut --, --, 1, 2, ...
>

<Version 4.49> (23/02/1994)
    Version 4.49 (23/02/1994)
    —————————————————————————

&TI Xode
————
    Correction d'un Bug : l'exportation en ASCII de tableaux dont
    certaines cellules étaient vides étaient incorrecte : ces cellules
    étaient "sautées". Lors de la réimportation de tableaux, il en
    résultait un décalage dans les colonnes.

    Typiquement, la première ligne contenant la période "##s" dans
    la deuxième colonne se voyait modifiée par deux xode successifs :
    avant on avait une première colonne vide, après, la première colonne
    contenait la période :
&CO
	AVANT :        ""            "##s"
	APRES :        "##s"          ""
&TX

>

<Version 4.48> (16/02/1994)
    Version 4.48 (16/02/1994)
    —————————————————————————
&TI Impression de tableaux
—————————
 Option fixed width characters pour les tableaux où l'alignement
      est nécessaire

&TI Lec
———
Les calculs incorporant des périodes dans des fonctions telles
      que sum() ou mean() étaient incorrects lorsque ces périodes
      étaient négatives (mean(-2, X)). Cette erreur est corrigée.

&TI Simulation et extrapolation
———————————————————————————
 Deux options complémentaires permettent de prolonger une série
      ou de choisir le point de départ d'une simulation en prenant la
      valeur de l'année précédente ou une extrapolation des deux années
      précédentes uniquement si la valeur courante est NA.
>

<Version 4.47> (10/02/1994)
    Version 4.47 (10/02/1994)
    —————————————————————————

&TI View Tables and Graphics
————————————————————————
    Lorsque plusieurs tableaux doivent être affichés (sous forme de
    graphique ou de tableau de nombres), un tableau déroulant reprenant le
    nom de chaque tableau ainsi que son titre est proposé.

    Il suffit de presser ENTER sur le tableau souhaité pour afficher le
    résultat (tableau ou graphique).


&TI Edit Objects
————————————
    Lors de l'édition des objets, un sous-menu propose d'éditer
    tout le WS ou seulement une liste. De plus, F10 permet d'entrer
    directement en édition de tout le WS.

    Dans le cas des listes, différentes fonctions ont été regroupées
    à ce niveau du menu et certaines se sont ajoutées :

&EN         Recherche de texte dans les objets
&EN         Tri des listes
&EN         Analyse (Scan) de WS
&EN         Stocker la liste des objets d'un fichier ou du WS
&EN         Effectuer des opérations (intersection, union, différence)
	  sur des listes
&EN         Calculer le nombre d'éléments d'une liste

    Dans le cas des variables, une nouvelle fonction permet la mise
    à jour des séries par le biais de tableaux. Si plusieurs tableaux sont
    demandés, un menu avec les tableaux est affiché, et non plus tous les
    tableaux successivement.

    Les fonctions de rapports correspondantes sont :

&CO
	    - $DataListVar name pattern [file]
	    - $DataCalcLst res lst1 op lst2
		    où op = {+,*,-}

	    - $ViewByTbl list
&TX

&TI Sample
——————
    Les périodes définies dans les écrans DataEditGraph, IdtExecute et
    ModelSimulate sont adaptées lors d'un changement du sample du WS ou
    lors d'un chargement de fichier de variables.

    Cette modification a lieu dans les cas suivants :

&EN         periodicité WS != nouvelle periodicité
&EN         si WS.from >> nouveau from
&EN         si WS.to   << nouveau to

&TI Graph sur base de séries
————————————————————————
&EN F8 dans le tableau déroulant des variables affiche le graphique de
      la série sur laquelle le curseur se trouve avec les propriétés du
      plus récent appel à la fonction DataEditGraph

&EN Shift-F8 dans le même tableau remplace l'ancien F8 et permet donc
      de grouper plusieurs séries sur le même graphique. De plus, cette
      fonction permet de modifier les propriétés (Sample, type, ...) des
      futurs graphiques affichés à l'aide de F8.

&TI Exécution directe d'identités
————————————————————————————
&EN F7 dans le tableau déroulant des identités exécute l'identité
      courante sur toute la période du WS

&EN Shift-F7 lance la fonction IdtExecute

&TI Xode bug (HP)
—————————————
&EN xode -i *.eqs fonctionne
&EN impression en PostScript fonctionne à nouveau

&TI DataCompare
———————————
    Le contenu du WS courant peut être comparé à celui d'un fichier.
    Le résultat de cette comparaison est composé de 4 listes :

&EN         objets trouvés dans le WS seulement
&EN         objets trouvés dans le fichier seulement
&EN         objets trouvés dans les deux, avec la même définition
&EN         objets trouvés dans les deux, avec une définition différente

    Dans un rapport :
&CO
    $DataCompareXxx file ONE TWO THREE FOR

	  ONE         in WS only
	  TWO         in file only
	  THREE       in both, equal
	  FOR         in both, different
&TX

&TI Estimate
————————
    Une fonction de construction automatique d'ajustements dynamiques
    est ajoutée dans le panneau de définition d'équation.
    Deux méthodes sont disponibles :

    Partial Adjustment (1 coefficient c1):
&CO
	lhs := rhs ->> d(lhs) := c1 * (rhs -(lhs)[-1])
&TX
    Error Correction Model (deux coefficients c1 et c2):
&CO
	lhs := rhs ->> d(lhs) := c1 * d(rhs) + c2 * (rhs-lhs)[-1]
&TX
&TI Load et Save de WS
——————————————————
    Lors de la lecture et de l'écriture des WS sur disque, des
    vérifications complémentaires sont effectuées pour s'assurer de la
    bonne fin de l'opération (notamment de le cas de timeout sur
    réseau).

&TI Object Names
————————————
    Les noms des objets sont vérifiés lors de leur création et ne
    peuvent contenir que des lettres, chiffres ou le caractère _.
    En outre, le premier caractère ne peut être un chiffre.

&TI Qode
————
    Nouvelle option -fw pour l'impression des tableaux a2m. Cette option
    remplace les caractères proportionnels standards par des non
    proportionnels. Le but est de conserver l'alignement dans
    les tableaux contenant des blancs.

&TI Impression de tableaux
—————————
    Bug solutionné : l'impression avec un sample erronné donne un
      message d'erreur.

&TI Seasonal Adjustment
———————————————————
    Une message d'erreur est affiché si une variable n'existe pas dans
    le fichier indiqué.

&TI Bug connu
—————————
    Dans la seule version 16 bits, le remplacement récursif des listes
    par leur valeur peut créer une erreur si plus de 3 niveaux
    d'imbrication sont nécessaires.
&CO
	A = $B
	B = $C
	C = $D
&TX
    L'utilisation de $A peut entraîner une erreur.
>

<Version 4.45-4.46> (05/01/1994)
    Version 4.45-4.46 (05/01/1994)
    ——————————————————————————————

&TI Gode
————
    L'impression de graphiques dans IODE32 restait dans le programme
    GODE32. Il fallait donc quitter le programme GODE (a-X) pour revenir
    à IODE32. Cette erreur est à présent corrigée.

&TI Reports
———————
    La macro %0% est le nombre d'arguments du rapport.

&TI Simulation
——————————
    La simulation peut être interrompu par l'utilisateur.

&TI Xode
————
    Nouveau format d'importation : PRN d'AREMOS : le fichier
    des variables se présente comme suit :
&CO
	"VARNAME" val1 val2 ....
	...
&TX

>

<Version 4.44> (04/01/1994)
    Version 4.44 (04/01/1994)
    —————————————————————————

&TI Dessaisonalisation
——————————————————
    Cette fonction est opérationnelle pour les séries comprenant des
    nombres négatifs. (corrigée)

&TI Installation
————————————
    Nouveau programme d'installation, guidant l'utilisateur, notamment
    en indiquant l'espace disponible sur chaque disque, en modifiant
    le fichier autoexec.bat et en permettant d'adapter le fichier
    printcap de définition des imprimantes.

&TI Rapport
———————
    La macro %0% est le nombre d'arguments du rapport.


&TI Xode
————
    Nouveaux formats importés (variables et commentaires) : fichiers de
    type PRN générés par AREMOS.

&TI Simulation
——————————
    Lorsqu'une équation n'est pas définie explicitement par rapport à
    son endogène ou lorsque celle-ci apparaît plusieurs fois dans
    l'équation, l'algorithme de simulation essaie de résoudre cette
    équation par une méthode de Newton. Si cette méthode ne fournit pas
    de résultat, une méthode de type sécante recherche à son tour une
    solution pour l'équation.

    Il n'est cependant pas garanti qu'une solution soit obtenue dans
    tous les cas. Les fonctions non continues (singularités) comme
&CO
	X := a + b * 1 / (X + 1)
&TX
    peuvent poser des problèmes en passant par la valeur -1.
    En cas de problème, la seule solution actuelle est de modifier la
    forme de l'équation :
&CO
	(X - a) * (X + 1) := b
&TX
>

