
START

#include "macros.h"

..esc ~


<Version 6.64> (30/06/2022)>
    Version 6.64 (30/06/2022)
    �������������������������
..esc ~


&TI Version IODE pour python
����������������������������

Dans le module iode de python (iode.pyd), la fonction python
&CO
    iode.report(filename)
&TX

est corrig�e. Elle se comporte comme son �quivalent dans les rapport :

&CO
    $ReportExec filename
&TX


&TI DataListSort
����������������
Cette fonction �tait erron�e depuis la version 6.63: le tri ne se faisait pas correctement.
Ce bug est corrig�.

Par ailleurs, une liste d�finie avec des ; comme s�parateur peut �tre r�ordonnanc�e par
cette fonction, ce qui' n'�tait pas le cas auparavant.


&TI Reports
�����������

&IT $foreach index
������������
Si un $define portant le m�me nom que l'index d'une boucle $foreach existant
avant d'entrer dans une boucle, ce define n'�tait pas restaur� � la fin de
la boucle. Il �tait erron�ment remplac� � la fin de la boucle par la liste
de valeurs pass�e comme argument.

Dor�navant, l'index d'une boucle $foreach est restaur� apr�s la boucle si il
�tait d�fini avant l'appel.

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
�����������
Implementation of $SysAnsiToOem and $SysOemToAnsi.


&IT Parsing of @function()
����������������������������
Les noms reconnus des @functions() peuvent dor�navant contenir des chiffres
ou le caract�re '_'.

De plus, lorsque le nom d'une @function n'est pas reconnu, le texte est
conserv� inchang� dans le r�sultat du rapport.


&IT @function() avec des param�tres entre quotes
����������������������������

Le second argument d'expressions avec 2 param�tres entre quotes comme ~c@fn("abc","def")~C
�tait ignor�s. Cette erreur est corrig�e.


&IT Parsing of {@function()}
����������������������������
As of now, when an expression between accolades contains @-functions, these
 @-functions are calculed and replaced by their value before the evaluation of {expression}.

&EX
    $define B 1
    $show Value of {1 + %@upper(b)%}   ->> {1 + %B%}  ->> {1 + 1}  ->> 2
&TX



&IT New $multiline command
��������������������������
Par d�faut, une ligne de rapport peut dor�navant se prolonger sur la ligne suivante si
elle se termine par un espace suivi d'un backslash ~c(\)~C.
Si c'est la cas,
le caract�re de retour � la ligne est ins�r� entre les lignes ~c(\n)~C.

Cela permet par exemple de construire dans un rapport des �quations qui se d�composent
sur plusieurs lignes, rendant leur lecture plus simple.

Pour ne pas permettre cette extension (compatilibili� avec les versions
ant�rieures) il suffit de placer la commande ~c$multiline 0~C avant les
lignes concern�es. Sans argument, le multi-lignes est accept�.

&CO
   $multiline [{Nn0}]
&TX

Par exemple, on peut dor�navant �crire :

&CO
    $multiline 1
    $DataUpdateEqs A A := c1 + \
    c2 * t
&TX

La d�finition de l'�quation A sera alors sur deux lignes.

A l'inverse, sans le multiline, une erreur est g�n�r�e. Ainsi

&CO
    $multiline 0
    $DataUpdateEqs A A := c1 + \
    c2 * t
&TX

essaie de cr�er une �quation termin�e par ~c"+ \"~C, ce qui donne le r�sultat suivant :

&CO
    essais.rep[23] - $multiline 0
    essais.rep[24] - $DataUpdateEqs A A := c1 + \
    (string)[7]:syntax error
    Error : dataupdateeqs A A := c1 + \
&TX

&IT New $noparsing command
��������������������������
Par defaut, les textes entre backquotes (`) ne sont pas pars�s dans les rapports.
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
��������������������������
Les macros peuvent etre supprim�e par la commande $undef ou $undefine.

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
���������������������������
Alias of ~c$shift~C.

&IT iodecmd: $ask command
���������������������������
&CO
    $ask label question
&TX

La commande $ask fonctionne dor�navant dans l'environnement iodecmd. La question est affich�e
� l'�cran et une r�ponse est attendue: si un des caract�res "OoJjYy1" est entr�, la r�ponse
est positive et le rapport se poursuit au label indiqu�.


&IT Noms des fichiers ascii apr�s lecture dans IODE
���������������������������������������������������
Lors de la lecture de fichiers ascii (~c"test.av"~C par exemple), le nom du fichier
source �tait remplac� par ~c"ws"~C dans le WS concern�.
Par exemple, si on lit le fichier ~c"test.av",~C le nom du WS est ~c"ws.var"~C.

Dor�navant, lorsqu'un fichier ascii est lu dans IODE, par exemple ~c"test.av",~C 3 cas sont
distingu�s :
&EN si le fichier existe, le nom du WS conserve le nom avec l'extension ascii (ici ~c"test.av"~C)

&EN si le fichier n'existe pas mais qu'un fichier avec le m�me nom mais avec l'extension
normale (ici ~c".var"~C) existe, c'est ce fichier ~c"test.var"~C qui est lu et le nom
du ws devient celui du fichier ~c".var"~C

&EN si le fichier n'existe pas et qu'il n'existe pas de fichier ~c".var"~C de m�me nom,
le nom du ws (vide dans ce cas) devient ~c"ws.var"~C, comme dans les versions
ant�rieures.


>

<Version 6.63> (30/09/2020)>
    Version 6.63 (30/09/2020)
    �������������������������
..esc ~


&TI Authentification des logiciels
����������������������������������
Les programmes ~ciode.exe~C, ~ciodecmd.exe~C et ~ciodecom.exe~C ont �t�
adapt�s afin d'�viter d'�tre bloqu�s par McAfee Active Threat Protection
(d�coupe des ex�cutables en deux modules : programme et data).

Par ailleurs les programmes sont dor�navant sign�s avec le certificat du BFP
et donc authentifi�s comme tels.


&TI Manuel
����������
La version Winhelp du manuel n'est plus distribu�e car non support�e par les
derni�res versions de Windows. Seules les versions locale (HtmlHelp) et Web
(iode.plan.be) sont encore disponibles.



>

<Version 6.62> (29/05/2020)
    Version 6.62 (29/05/2020)
    �������������������������
..esc ~


&TI Check Version
�����������������
La fonction de v�rification de version (cfr iode.ini =>> ~c[General] CheckVersion~C) est
d�sactiv�e en raison d'un probl�me de compatibilit� avec la nouvelle version
du site iode.plan.be.
>


<Version 6.61> (15/05/2019)
    Version 6.61 (16/05/2019)
    �������������������������
..esc ~


&TI Hodrick-Prescott filter
���������������������������
La fonction de calcul de trend bas�e sur le "filtre Hodrick-Prescott"
transforme les s�ries � traiter en logarithme avant d'effectuer le calcul
d'optimisation proprement dit. Une fois calcul�es, les s�ries contenant le
trend sont transform�es � nouveau en passant � l'exponentielle.

Ceci a deux cons�quences :

&EN seules les parties strictement positives des s�ries peuvent �tre trait�es
&EN une s�rie purement lin�aire, qui ne pr�sente donc aucune d�viation par
rapport au trend, devrait �tre identique apr�s filtrage. Ce n'est plus le cas
si on lui applique une transformation du type log, m�me si on repasse
ensuite � l'exponentielle.

Une nouvelle fonction plus standard a donc �t� ajout�e � IODE. Cette fonction
est plus en ligne avec les r�sultats obtenus avec les logiciels standard comme stata.


&IT Nouvelle fonction de rapport
��������������������������������

&CO
    $WsTrendStd filename lambda series1 series1...
&TX

&IT Nouvel op�rateur LEC
������������������������
&CO
    hpstd([[from,]to],expr)  : calcul sans passage au logarithme de ~cexpr~C
&TX

Les param�tres sont :
&EN from : p�riode de d�part du calcul, premi�re ann�e par d�faut
&EN to : p�riode de fin de calcul, derni�re ann�e par d�faut
&EN expr : expression LEC quelconque. Doit �tre strictement positif pour ~chp()~C

&IT Nouvel �cran
����������������
L'�cran Workspace / Trend Hodrick-Prescott filter a �t� adapt� en cons�quence en proposant
le choix entre le passage au logarithme ou non.

&TI Taux de croissance moyens
�����������������������������
Correction d'un bug qui s'est gliss� dans la version 6.58 dans le calcul des
taux de croissance moyens. Ce bug rendait en g�n�ral les taux de croissance
moyens beaucoup plus �lev�s que ce qu'ils devaient �tre.

>

<Version 6.60> (11/04/2019)
    Version 6.60 (11/04/2019)
    �������������������������
..esc ~

&TI Rapports : nouvelles fonctions
����������������������������������

Des fonctions de rapport ont �t� ajout�es :
&EN @mkdir : cr�er un r�pertoire
&EN @rmdir : supprimer un r�pertoire
&EN @getdir : afficher le repertoire courant
&EN @chdir : changer de r�pertoire courant
&EN @void : supprimer l'output d'une fonction

Par ailleurs les commandes qui changent le directory courant affichent le
nouveau directory dans la barre de titre de la fen�tre.

&IT Exemple
�����������
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

R�sultat
��������

&CO
  Current dir : c:\usr\iode
  Make dir    :
  Chdir       : c:\usr\iode\subdir
  Chdir       : c:\usr\iode
&TX


&TI Correctif : save WS / CSV format
������������������������������������
Les fichiers de variables peuvent  � nouveau �tre sauv�s en format csv via le menu
"Workspace / Save Workspace" ou via la commande $WsSaveVar dans un rapport.

&CO
## Load a IODE var file and save vars in CSV format
$WsLoadVar ..\dos\test.var
$WsSaveVar test.csv
&TX

&TI Filtre Hodrick-Prescott
���������������������������
La description de la m�thode a �t� ajout�e au manuel de IODE dans
la partie "Le programme IODE / Workspace / Trend Correction".


>

<Version 6.59> (13/03/2019)
    Version 6.59 (13/03/2019)
    �������������������������
..esc ~

&TI Rapports : nouvelles fonctions $ModelSimulateSave*
������������������������������������������������������


Le nombre d'it�rations et le seuil de convergence atteint lors de la derni�re simulation
peuvent �tre sauv�s dans des ~uvariables~U IODE. Deux nouvelles fonctions de rapport
ont �t� cr��es � cette fin :


&EN $ModelSimulateSaveNiters ~ivar_name~I
&EN $ModelSimulateSaveNorms  ~ivar_name~I

Pour rappel, les fonctions de rapport suivantes permettent d'obtenir les m�mes informations :


&EN ~c@SimNiter(period)~C
&EN ~c@SimNorm(period)~C

&IT $ModelSimulateSaveNiters
�����������������������

Cette commande permet de sauver dans une variable le nombre d'it�rations
qui ont �t� n�cessaires pour chaque p�riode lors de la derni�re simulation.

Le nom de la variable r�sultat est pass� comme param�tre.

S'il n'y a pas encore eu de simulation dans le cours de la session, la variable est
cr��e avec des valeurs NA.

Si une simulation a d�j� eu lieu, les valeurs des p�riodes non simul�es sont fix�es � 0 et
celles des p�riodes simul�es contiennent le nombre d'it�rations n�cessaires �
la convergence pour cette p�riode. S'il n'y a pas de convergence, la valeur est celle du
param�tre MAXIT de la simulation.


Syntaxe :
&CO
    $ModelSimulateSaveNiters varname
&TX

Exemple :
&CO
    $ModelSimulateSaveNiters SIM_NITERS
&TX

&IT $ModelSimulateSaveNorms
�����������������������

Cette commande permet de sauver dans une variable le seuil de convergence (la norme) atteint
pour chaque p�riode lors de la derni�re simulation.

Le nom de la variable r�sultat est pass� comme param�tre.

S'il n'y a pas encore eu de simulation dans le cours de la session, la variable est
cr��e avec des valeurs NA.

Si une simulation a d�j� eu lieu, les valeurs des p�riodes non simul�es sont fix�es � 0 et
celles des p�riodes simul�es contiennent le seuil de convergence
pour cette p�riode.

Syntaxe :
&CO
    $ModelSimulateSaveNorms varname
&TX
Exemple :
&CO
    $ModelSimulateSaveNiters SIM_NORMS
&TX


&TI Filtre Hodrick-Prescott
��������������������������
La valeur du param�tre lambda peut dor�navant �tre un nombre r�el. Auparavant,
cette valeur �tait un nombre entier.

>

<Version 6.58> (20/01/2019)
    Version 6.58 (20/01/2019)
    �������������������������
..esc ~

&TI Rapports : $DataCreateGraph
�������������������������������
Correction : l'ordre des param�tres de d�finition des grids est corrig� pour
correspondre au manuel.


&TI Comment ex�cuter un rapport IODE
������������������������������������
Il est possible d'ex�cuter un rapport IODE (soit ~cmyreport.rep~C) de
plusieurs fa�ons. En plus de la m�thode habituelle � travers l'interface de IODE,
on peut lancer un rapport IODE de 3 fa�ons :

&EN en lan�ant le programme ~ciode~C suivi du nom du fichier rapport dans un "Command Prompt"
&EN en lan�ant le programme ~ciodecmd~C suivi du nom du fichier rapport dans un "Command Prompt"
&EN dans l'explorateur Windows en double-cliquant sur le fichier .rep

Pour illustrer cela, voici un exemple de rapport.

&IT LoadWs.rep
��������������
Le rapport suivant charge 5 WS en m�moire.

&CO
$define file c:\iode\example\fun

$WsLoadVar %file%
$WsLoadEqs %file%
$WsLoadScl %file%
$WsLoadTbl %file%
$WsLoadCmt %file%
&TX

&IT Ex�cuter un rapport dans un "Command Prompt" avec le programme ~ciode~C
�����������������������������������������������������������������������

A partir de la version 6.58, il suffit de lancer la commande iode en lui
passant le nom du rapport comme param�tre. Par exemple :

&CO
  c:\iode>> iode LoadWs.rep
&TX

Iode est alors lanc� et LoadWs.rep est ex�cut� ~udans l'interface de IODE~U.
Si le rapport se termine par ~c$quitode~C, IODE est quitt� � la fin du
rapport.

A noter que le directory o� a �t� install� iode doit se trouver dans le PATH pour
que cela fonctionne. Sans cela, Windows ne pourrait savoir o� trouver le
programme...

Pour changer le PATH,
&EN 1. aller dans ~cControl Panel\All Control Panel Items\System~C
&EN 2. s�lectionnez "Advanced system Settings"
&EN 3. s�lectionnez l'onglet "Advanced"
&EN 4. s�lectionnez "Environment Variables"
&EN 5. allez aux "System variables"
&EN 6. double-cliquez sur la variable "Path"
&EN 7. ajoutez le directory d'installation de IODE (si cela n'a pas encore �t� fait)
&EN 8. Cliquez OK (3x) pour confirmer

Le prochain lancement du "Command Prompt", la nouvelle valeur de la variable
~cPath~C sera connue.

&IT Ex�cuter un rapport dans un "Command Prompt" avec le programme ~ciodecmd~C
���������������������������������������������������������������������������
Le programme iodecmd permet d'ex�cuter un rapport IODE sans
interface utilisateur. Une fois le rapport termin�, iodecmd rend la main.

Par d�faut, le r�sultat de chaque commande est affich� � l'�cran :
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
�����������������������������������������������������
Il est possible de lancer un rapport dans l'explorateur Windows en
double-cliquant sur le fichier. Condition pr�alable:il faut associer les fichiers
~c*.rep~C avec le programme IODE. Suite aux restrictions apport�es par Microsoft,
cette association (comme les autres) ne se fait plus automatiquement.

A noter que cela ne peut fonctionner qu'� partir de la version 6.58.

>

<Version 6.57> (02/03/2018)
    Version 6.57 (02/03/2018)
    �������������������������
..esc ~


&TI E-Views
�����������
Nouvelles directives dans les fichiers d'�quations EVIEWS. Les directives
doivent se trouver seules sur une ligne. Elles peuvent �tre en majuscules, minuscules ou
un m�lange des deux.

Elles d�terminent la fa�on dont les termes des �quations ou identit�s EVIEWS
du type ~cname(1)~C doivent �tre traduites en IODE.

&CO
  KEEP COEFS:
  DROP COEFS:
&TX

&EN Apr�s KEEP COEFS:, la traduction de ~cname(1)~C ou ~cNAME(1)~C donne ~cname_1~C.
&EN Apr�s DROP COEFS:, la traduction de ~cname(1)~C ou ~cNAME(1)~C donne ~cname~C.

Au d�but de la lecture fichier, ~cDROP COEFS:~C est la valeur initiale.

&IT Exemple de fichier EVIEWS
�����������������������������

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

&IT R�sultat en IODE
��������������������
&CO
B_QHOA := b_1_2*B_C0122222
B_QHOB := b_qh_b_14_1*B_CO14
B_QHOCADD := (b_qh_c_1*B_CO1)+(b_qh_c_2*B_CO2)+(b_qh_c_7*B_CO7)
B_QHOCR_NC := b_qh_cr_14*B_CO14
&TX

>
<Version 6.56> (27/01/2017)
    Version 6.56 (27/01/2017)
    �������������������������
..esc ~


&TI Sauvetage des workspaces (fix)
��������������������������
Dans l'�cran de sauvetage des workspaces, les ws pour lesquels aucune extension n'est
indiqu�e sont � nouveau sauv�s en format IODE (~c.var~C, ~c.cmt~C , ...). Dans la version
pr�c�dente, aucun sauvetage n'�tait effectu�.

Si une extension de format ascii est indiqu�e, le ws est sauv� en format ascii.

Les commandes de rapport ~c$WsSave*~C op�rent de la m�me mani�re.


&TI G�n�ration de fichiers csv
������������������������������

&IT A partir des rapports ($WsSaveCsvVar, $csvsep et $csvdec)
�������������������������������������������������������������

Les param�tres de ces commandes (s�parateur de liste et caract�re d�cimal)
sont initialis�s par les valeurs des "International Settings" d�finies dans
Windows. De cette fa�on, les fichiers csv g�n�r�s sont directement lisibles
en Excel sans avoir � se pr�occuper des s�parateurs et autres caract�res
d�cimaux.

&IT A partir de l'�cran Save Workspace
������������������������������������������������������
Dans l'�cran "Workspace/Save Workspace",
si l'extension du fichier est ~c.csv~C, le fichier est sauv� en format CSV.
Les param�tres (s�parateur et caract�re d�cimal) sont fix�s par les
commandes ~c$csvsep~C et ~c$csvdec~C ou, � d�faut, par les valeurs des
locales Windows. Les fichiers peuvent donc directement �tre ouverts en Excel.

&TI Copy/Paste de variables entre Excel et IODE
�����������������������������������������������
&NO Cette nouvelle fonction est encore en phase de test.

La copie de valeurs � partir d'Excel vers l'�diteur de variables de IODE est
am�lior� � plusieurs �gards :

&EN les virgules d�cimales sont trait�es correctement
&EN les signes de devise (currency) sont ignor�s dans les copies vers IODE
&EN les cellules vides sont interpr�t�es comme -- (na)

Attention cependant � ce que le s�parateur entre les valeurs doit �tre la
tabulation. Les blancs sont ignor�s.

Si la copie provient
d'un autre logiciel, il faut s'assurer que c'est bien le TAB qui s�pare les
valeurs.

&NO Cette fonction utilise les "locale" de Windows pour d�terminer les
s�parateur de milliers et de d�cimales. Comme le s�parateur de milliers
est parfois le point (1.234,5 par exemple), il faut �tre attentif que 1.23 est en fait
123 car le signe des milliers est ignor�.

>

<Version 6.55> (05/01/2017)
    Version 6.55 (05/01/2017)
    �������������������������
..esc ~


&TI Ouverture d'anciens fichiers
��������������������������������
Correction d'un bug lors de l'ouverture des fichiers de version ant�rieure
(magic number 504d).


&TI Interpr�tation des macros
����������������������������

Lorsqu'une macro (string entre %) n'est pas trouv�e, le texte de la macro
est conserv� tel quel, % inclus.

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

Le r�sultat de ce rapport est :
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
���������
Correction d'une erreur d'orthographe en fran�ais dans les textes automatiques des
tableaux/graphiques.

&TI iode -fontsize
������������������
Le param�tre ~c-fontsize nn~C est prioritaire sur la d�finition
~c[FONT] FONTSIZE=nn~C dans le fichier ~ciode.ini~C.
>

<Version 6.54> (22/08/2016)
    Version 6.54 (22/08/2016)
    �������������������������
..esc ~


&TI Duplex and page orientation printing options
������������������������������������������������

Pour �viter les probl�mes li�s aux mises � jour des drivers des imprimantes Windows,
les options d'impression en recto/verso et en portait/landscape ont �t� annul�es:
ces options ne sont plus affich�es dans l'�cran "Print Setup/Options/Printer" et ne
sont plus actives dans les commandes des rapports.

Pour �viter les probl�mes de compatibilit�s avec des rapports existants, les fonctions

&EN  $PrintOrientation
&EN  $PrintDuplex

restent valides, mais n'ont plus d'effet sur la configuration des imprimantes.

>

<Version 6.53> (18/07/2016)
    Version 6.53 (18/07/2016)
    �������������������������
..esc ~


&TI E-Views to IODE
������������������
&EN L'importation des coefficients est � nouveau op�rationnelle. Cette option
avait �t� supprim�e dans la version pr�c�dente.

&EN Les lignes commen�ant par ~c@INNOV~C sont ignor�es.

Pour rappel, la m�thode d'importation fonctionne comme suit :

&IT Lignes "Forecasting Equation:" et "Substituted coefficients:"
����������������������������������������������������
Lorsqu'une ligne ~cForecasting Equation~C est rencontr�e, l'�quation qui la
suit (apr�s avoir saut� une ligne) est lue et conserv�e en m�moire jusqu' � la ligne
 ~cSubstituted Coefficients:~C suivante. Cette derni�re est alors analys�e et les
valeurs des coefficients en sont extraites et plac�es dans dans scalaires
IODE. Ces scalaires portent le nom de l'endog�ne de l'�quation avec le
suffixe ~c_1~C, ~c_2~C, ...

L'�quation initiale est transform�e pour remplacer les coefficients ~cC(1)~C... par
les noms des scalaires ~cendo_1~C, ... et elle est finalement sauv�e dans IODE.

&IT Lignes "~cEstimation Command:~C" et "~cEstimation Equation:~C"
�����������������������������������������������������������������������������
Ces lignes sont ignor�es.


&IT Lignes apr�s "~cIDENTITIES:~C"
����������������������������

Les lignes contenant des identit�s sont interpr�t�es comme �quations � partir du moment o� :

&EN soit une ligne contenant le texte ~cIdentities:~C est trouv�e
&EN soit une premi�re identit� pr�fix�e par ~c@IDENTITY~C est trouv�e

A partir de ce moment toutes les lignes contenant du texte sont interpr�t�es par
le programme comme des �quations IODE.

En cas d'erreur de syntaxe, la lecture s'arr�te et un message d'erreur est produit.
>

<Version 6.52> (21/06/2016)
    Version 6.52 (21/06/2016)
    �������������������������
..esc ~


&TI E-Views to IODE
������������������
La syntaxe des fichiers de transfert E-Views to Iode est l�g�rement modifi�e pour
permettre une plus grande souplesse : les lignes contenant des identit�s ne
sont plus interpr�t�es qu'� partir du moment o� :

&EN soit une ligne contenant le texte ~cIdentities:~C est trouv�e
&EN soit une premi�re identit� pr�fix�e par ~c@IDENTITY~C est trouv�e

A partir de ce moment toutes les lignes contenant du texte sont interpr�t�es par
le programme comme des �quations IODE.

En cas d'erreur de syntaxe, la lecture s'arr�te et un message d'erreur est produit.


&TI Corrections
���������������
&EN la lecture et l'�criture de fichiers de profils sont corrig�es


&TI IodeCom
�����������
La version pr�c�dente ne fonctionnait plus correctement sur certaines machines en raison
de l'absence d'une dll dans la distribution. Ce probl�me est corrig�.

&TI IodeCmd
�����������
Les messages d'erreurs pr�cis sont � pr�sent affich�s dans ~ciodecmd~C. Auparavant
seul le message ~c"Error ... Msg##nn"~C �tait affich�.

>

<Version 6.51> (04/04/2016)
    Version 6.51 (04/04/2016)
    �������������������������
..esc ~


&TI Fonctions de rapport : format LArray
����������������������������������������
De nouvelles fonctions de rapport permettant le sauvetage de s�ries dans le
format csv adapt� au package Python LArray ont �t� cr��es :

&EN $WsSaveCsvVar filename [from to] [vars]
&EN $CsvSep
&EN $CsvDec
&EN $CsvDigits (changer)

&IT Exemple
�����������
Le rapport suivant fixe d'abord les param�tres :
&EN le s�parateur de cellules (d�faut ,)
&EN le nombre de chiffres significatifs (d�faut 10)
&EN le caract�re d�cimal (d�faut .)
&EN le texte pour les valeurs NaN (d�faut vide)

Il lance cr�e ensuite 4 fichiers selon des modes diff�rents :
&EN toutes les variables sur ou tout le sample
&EN toutes les variables sur la p�riode 2003Y1 2010Y1
&EN les variables commen�ant par ~cAC~C sur la p�riode 2009Y1 2010Y1
&EN les variables commen�ant par ~cAC~C ou ~cB~C sur tout le sample du WS

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

Les fichiers r�sultant de ce rapports sont :

&TI fun.csv
�����������
&CO
 var\time;1960;1961;1962;1963;1964;1965;1966;1967;1968;1969;1970;...
 ACAF;nan;nan;nan;nan;nan;nan;nan;nan;nan;nan;1.2130001;5.2020001;9.184;...
 ACAG;nan;nan;nan;nan;nan;nan;nan;nan;nan;nan;-11.028999;-15.847;-19.288002;...
 ...
&TX

&TI fun1.csv
�����������
&CO
 var\time;2003;2004;2005;2006;2007;2008;2009;2010
 ACAF;nan;nan;nan;nan;nan;nan;nan;nan
 ACAG;nan;nan;nan;nan;nan;nan;nan;nan
 AOUC;nan;0.2478319161;0.2545676576;0.2637957322;0.2762428533;0.2858059161;...
 ...
&TX

&TI fun2.csv
�����������
&CO
 var\time;2009;2010
 ACAF;nan;nan
 ACAG;nan;nan
 ...
&TX

&TI fun3.csv
�����������
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
������������������
Extension des buffers pour la copie vers Excel en DDE.

>

<Version 6.50> (25/01/2016)
    Version 6.50 (25/01/2016)
    �������������������������
..esc ~


&TI Adaptations techniques
��������������������


&EN Planning Corner : REAL ->> double
&EN Compilateur BCC32 7.10 Embarcadero
>

<Version 6.49> (19/01/2016)
    Version 6.49 (19/01/2016)
    �������������������������
..esc ~


&TI WsImportEViews
��������������������

Cette fonction de rapport a �t� �tendue :

&EN Support des fonctions LOG, DLOG, EXP
&EN Fonction ~c@TREND~C traduite en ~ct~C
&EN La fin des lignes � partir d'une quote (caract�re  ') est ignor�e
&EN Support des lignes sans pr�fixe @IDENTITY
&EN Support des coefficients VARNAME(n) ->> varname_1
&EN Les expressions C(n) ne sont plus trait�es s�par�ment, mais comme n'importe quelle
autre expression VARNAME(n).
&EN le s�parateur entre membre de gauche et de droite de l'�quation est le premier
signe = rencontr� s'il n'est pas entre parenth�ses. Par exemple :

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
    �������������������������
..esc ~


&TI Check Last Version
��������������������

Correctif : la v�rification de la derni�re version peut �tre supprim�e via
le fichier iode.ini (voir 6.47), param�tre ~c[General] CheckVersion=0~C.

Cependant, l'acc�s au site ~ciode.plan.be~C �tait quand-m�me effectu�, m�me
si ce param�tre �tait fix� � 0, ce qui bloquait quelques secondes le
d�marrage de IODE en cas de blocage du site.

>

<Version 6.47> (06/10/2015)
    Version 6.47 (06/10/2015)
    �������������������������
..esc ~


&TI Check Last Version
����������������������
La derni�re version du logiciel se trouve sur le site ~ciode.plan.be~C. A
chaque d�marrage de IODE, la version courante est compar�e avec la derni�re
version disponible sur le serveur. Si une nouvelle version est disponible,
l'utilisateur est pr�venu via un message et via le titre de la fen�tre.

Ce check, et le message qui l'accompagne �ventuellement, peuvent �tre supprim�s via le
param�tre ~cGeneral/CheckVersion~C du fichier ~ciode.ini~C.



&TI IODE.INI
������������
De nouveaux param�tres sont d�finis :

&CO
[General]
Banner=0
CheckVersion=0
&TX
&EN ~cBanner~C permet de supprimer l'affichage du banner au d�marrage de IODE
&EN ~cCheckVersion~C permet de supprimer la recherche de nouvelle version sur le
site ~ciode.plan.be~C et dons de ne plus afficher de message si la version
courante n'est plus la derni�re.


&TI Correctifs
��������������������
Corrections de bugs :
&EN impression de fichiers A2M
&EN �dition de tableaux de comparaison de fichiers
&EN affichage des noms de fichiers dans des graphiques

>

<Version 6.46> (16/09/2015)
    Version 6.46 (16/09/2015)
    �������������������������
..esc ~


&TI Fichier iode.ini
��������������������

Un nouveau fichier de param�tres est ajout� � IODE : ~ciode.ini~C.
Ce fichier est lu au d�marrage de IODE.


&IT Localisation
����������������
Le fichier ~ciode.ini~C se trouve par d�faut dans le
r�pertoire d'installation de IODE (par d�fault ~cc:\iode~C). On peut
sp�cifier un autre fichier en utilisant le nouveau param�tre ~c-iodeini~C au
lancement de IODE.

Par exemple :
&CO
   c:\usr\mymodel>> iode -iodeini mycolors.ini
&TX


&IT Syntaxe du fichier iode.ini
����������������������
La syntaxe du fichier est proche de celle des fichiers ~c.ini~C  :
&EN chaque section d�bute par un nom entre []. Par exemple :
~c[Fonts]~C.
&EN dans chaque section, les param�tres sont indiqu�s sous la forme
~cparam=values~C ou ~cparam~C si une valeur ne doit pas �tre fournie.
&EN Un ligne qui commence par un point-virgule est ignor�e

&IT Contenu de iode.ini
�����������������������

Ce fichier contient actuellement les informations suivantes, mais sera
�tendu dans le futur :

Section ~c[Screen]~C
����������������
&EN Nl : nombre de lignes de la fen�tre Iode (en caract�res)
&EN Nc : nombre de colonnes de la fen�tre Iode (en caract�res)

&CO
[SCREEN]
NL=35
NC=90
&TX


Section ~c[Font]~C
������������������
&EN2 ~cFontName~C : nom
&EN2 ~cFontSize~C : taille (en points)
&EN2 ~cFontSizeMin~C : taille minimum
&EN2 ~cFontSizeMax~C : taille maximum
&EN2 ~cFontSizeIncr~C : incr�ment en point de la taille des caract�res

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
�������������������������������

Ces sections contiennent la description RGB des couleurs utilis�es dans l'�cran.
Seule la palette [Palette_Default] est utilis�e par Iode. Si elle est
pr�sente, elle remplace les valeurs par d�faut de Iode.

La logique est la suivante :
&EN les trois premi�res valeurs repr�sentent le code RGB (Red, Green, Blue)
de la couleur de fond (background)

&EN les trois derni�res valeurs repr�sentent le code RGB (Red, Green, Blue)
du caract�re (foreground).

Ainsi 0 0 0 repr�sente le noir et 255 255 255 repr�sente le blanc. Toute combinaison
permet de cr�er une palette compl�te de couleurs.

Pour simplifier l'�criture, on peut utiliser les mots suivants dans le texte :
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
���������������������
&EN Fonctions corrig�es :
&EN2 ~c$DataCompare~C : les comparaisons d'�quations et d'identit�s
    donnaient comme diff�rentes des �quations/identit�s identiques.

&EN2 ~c##FileRename, ##FileCopy, ##WsHtolLast, ##WsLtoHFlow, ##WsLtoHStock,
##WsSeasonAdj, ##WsTrend~C : iode pouvait stopper en raison de la longueur
des noms de fichiers.

>

<Version 6.45> (07/09/2015)
    Version 6.45 (07/09/2015)
    �������������������������
..esc ~

&TI Aide en ligne
�����������������
Deux formats de fichiers sont dor�navant support�s : les format ~c  .hlp~C et ~c.chm.~C
Il s'agit du m�me manuel, mais chaque format offre des fonctions sp�cifiques
de recherche et d'affichage.

On peut passer d'un format � un autre dans le menu ~cHelp~C.

L'acc�s au site ~ciode.plan.be~C est �galement ajout� directement dans la menu Help.

&IT Fichier ~ciode.hlp~C
������������������������

Il s'agit de l'ancien syst�me WinHelp de Windows. Cette formule pr�sente
l'avantage de pointer directement sur la page du manuel correspondant au
contexte courant. Dans l'�dition des rapports par exemple, en cliquant sur F10 avec
le curseur sur un nom de fonction, on acc�de directement � la page d�sir�e.

Contrainte : � partir de Vista, il faut avoir pr�alablement
install� le logiciel WinHelp pour pouvoir exploiter ce format. Ce logiciel
est gratuit et est disponible sur le site de Microsoft.

&IT Fichier ~ciode.chm~C
������������������������

Ce fichier est d'un format plus r�cent et pr�sentent
d'autres fonctions de recherche et d'indexation. Il est propos� par
d�faut. Ce manuel n'est cependant pas pilot� par iode : une fois ouvert, l'utilisateur
doit rechercher la page qui l'int�resse � travers l'interface du programme lui-m�me.

Dans les deux cas, le fichier d'aide doit se trouver dans m�me directory que
le programme ~ciode.exe~C.

&TI Correction de bugs
���������������������
&EN Fonctions corrig�es :
&EN2 ~c$DataCompareXxx~C
&EN2 ~c$DataDuplicate~C
&EN2 ~c$DataListXxx~C

>

<Version 6.44> (10/04/2015)
    Version 6.44 (10/04/2015)
    �������������������������
..esc ~

&TI Sauvetage et relecture des WS en Ascii
������������������������������������������
Le sauvetage en ascii des fichiers de commentaires, d'�quations, de listes
et de tableaux prend en compte les caract�res ~c"~C et ~c\~C qui peuvent �tre
pr�sents dans certains textes (titres, commentaires, ...).

Ces caract�res sont pr�c�d�s au moment du sauvetage par un backslash (~c\~C). Cela permet
de relire ensuite sans probl�me le fichier ascii g�n�r� (~c.ac, .ae, .al, .at~C).

&TI Noms de WS d�passant 63 caract�res
������������������������������������������������������������������������
Les noms de WS sup�rieurs � 63 caract�res et/ou contenant des espaces sont �
pr�sent g�r�s par IODE. Cela permet de placer ses fichiers dans n'importe quel directory comme :
&CO
~cC:\Users\My Name\SharePoint\share sgad\cic\IODE\TresLongNomdeDirectoryPourTesterIode~C
&TX

Les fichiers avec des noms longs et/ou des espaces dans leur nom
sont utilisables dans les situations suivantes :

&EN Ouverture � partir de l'explorateur Windows
&EN Lancement � partir d'un shell dos ("Command Prompt")
&EN Rapports : dans les fonctions ~c$WsLoad*~C, ~c$WsSave*~C, etc.

&TI Caract�res accentu�s en HTML
��������������������������������
Tous les caract�res minuscules accentu�s sont traduits dans la forme html du type "&...;".

&TI Enregistrement des touches
������������������������������
Il est possible d'enregistrer dans un fichier les s�quences de touches
utilis�es pendant une session iode. Il faut pour ce faire lancer ~ciode~C
avec le param�tre ~c-rec~C comme ci-dessous :

&CO
    iode -rec filename
&TX

Le fichier (ici ~cfilename~C) contiendra des s�quences de touches codifi�es
selon la syntaxe d�crite dans la version 6.27 de iode. Ce fichier peut �tre
�dit� � l'aide d'un �diteur ascii.

Ces touches peuvent ensuite �tre "rejou�es" automatiquement. Cela permet par exemple :
&EN de lancer des op�rations r�p�titives sans avoir � passer par les rapports
&EN de lancer des proc�dures de test unitaires

Toutes les op�rations ne sont cependant pas exploitables : les op�rations
effectu�es � l'aide de la souris ne sont pas support�es. Il faut �galement
�tre attentif au fait que la position dans un menu ou le contenu d'un �cran
de saisie peuvent changer entre deux lancements successifs de iode.

&TI Correction de bugs
���������������������
Lors de l'�dition des tableaux, une erreur pouvait se produire
lors de la destruction de lignes.


>

<Version 6.43> (24/02/2014)
    Version 6.43 (24/02/2014)
    �������������������������
..esc ~

&TI IODECMD : arguments
�����������
Le lancement d'un rapport � l'aide de iodecmd peut contenir les arguments du rapport :
&CO
  c:\>> iodecmd -y -v myrep arg1 arg2
&TX

Les param�tres du programme ~ciodecmd~C doivent se trouver avant le nom du rapport sans
quoi ils sont consid�r�s comme des arguments du rapport et pass�s comme tels au rapport.

&TI Conversion UTF8
�������������������
De nouvelles fonctions de rapport permettent de transformer des fichiers codes ANSI ou OEM
en UTF-8 directement depuis des rapports.

&CO
    $SysAnsiToUTF8 inputfile outputfile
    $SysOemToUTF8  inputfile outputfile
&TX


&TI Conversion ANSI-OEM
�������������������
Les fonctions de rapport suivantes convertissent des fichiers cod�s ANSI en OEM
et r�ciproquement.

&CO
    $SysAnsiToOem inputfile outputfile
    $SysOemToAnsi inputfile outputfile
&TX

>

<Version 6.42> (07/10/2013)
    Version 6.42 (07/10/2013)
    �������������������������
..esc ~

&TI G�n�ration de tableaux HTML
�������������������������������
Lors de la g�n�ration de tableau en HTML, des classes par d�faut sont utilis�es pour les tags suivants :
&EN <<TABLE>> : A2mTable
&EN <<TR>> : A2mTR
&EN <<TH>> : A2mTH
&EN <<TD>> : A2mTD

Ces tags peuvent �tre remplac�s par d'autres via 4 commandes dans les rapports :
&CO
  $PrintHtmlTableClass [myTableClass]
  $PrintHtmlTRClass    [myTRClass]
  $PrintHtmlTHClass    [myTHClass]
  $PrintHtmlTDClass    [myTDClass]
&TX

Les noms de classes vides suppriment la r�f�rence � la classe dans le tag html.


>

<Version 6.41> (15/07/2013)
    Version 6.41 (15/07/2013)
    �������������������������
..esc ~

&TI Gestion M�moire
�������������������
Am�lioration  de la gestion des "swaps" pour permettre la gestion de plus
de ~bblocs~B m�moire (>>32767).

On a dans la version courante :
&EN 262144 bytes par bloc m�moire  (ou plus pour des objects plus grands)
&EN 65535 blocs m�moire maximum
>

<Version 6.40> (20/02/2013)
    Version 6.40 (20/02/2013)
    �������������������������
..esc ~

&TI Rapports
��������

&IT $indent
�����������
Il est dor�navant possible d'indenter les commandes dans les rapports.
La commande ~c$indent~C sans argument indique qu'� partir de ce moment, les
commandes peuvent ne plus �tre coll�es � la marge. Avec l'argument N ou n ou
0, le comportement ancien reprend.


Pour �viter les probl�mes de compatibilit� entre les versions de IODE, la valeur
par d�faut est de ne pas accepter les indentations.

&CO
   $indent [{Nn0}]
&TX

Par exemple, on peut �crire :

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
Le r�sultat produit est le suivant  :

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
���������������������������������
Les instructions de ce groupe permettent de construire des proc�dures, c'est � dire des
listes de commandes qui peuvent �tre r�utilis�es et param�tr�es.

&CO
 $procdef procname [fparm1 ...]
 ..
 $procend
&TX

o�
&EN ~bprocname~B est le nom de la proc�dure (case sensitive).
&EN ~bfparm1~B est le premier param�tre ~iformel~I de la proc�dure

&IT Appel d'une proc�dure
�������������������������
L'appel se fait simplement par la commande :

&CO
$procexec nom_proc aparm1 ...
&TX

o�
&EN ~bprocname~B est le nom de la proc�dure (case sensitive).
&EN ~baparm1~B est le premier param�tre ~iactuel~I de la proc�dure

&IT Param�tres
��������������
Les param�tres formels sont trait�s dans la proc�dure comme des defines
(locaux � la proc�dure) : ils doivent �tre appel�s par ~c%fparm%~C.

Leurs valeurs sont fix�es comme suit :

&EN S'il y a moins de param�tres actuels que de param�tres formels ou si
leur nombre est �gal, les valeurs des param�tres actuels sont assign�es dans
l'ordre aux premiers param�tres formels. Les param�tres formels
exc�dentaires sont consid�r�s comme vides.

&EN S'il y a plus de param�tres actuels que de param�tres formels, les
param�tres formels, ~bsauf le dernier~B, re�oivent les valeurs de premiers
param�tres actuels, dans l'ordre de leur passage.
~bLe dernier param�tres formel re�oit la valeur de tous
les param�tres actuels restants~B.

Exemple avec plus de param�tres actuels de formels :
����������������������������������������������������
&CO
$indent
$procdef print list
    $foreach i %list%
	$show print : %i%
    $next i
$procend
$procexec print A B C
&TX

R�sultat :
&CO
print : A
print : B
print : C
&TX
On constate que le param�tre formel ~clist~C re�oit toutes les valeurs
pass�es � la proc�dure. La boucle it�re donc 3 fois.

Exemple avec moins de param�tres actuels de formels :
����������������������������������������������������
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

R�sultat :
&CO
Mon Titre
&TX
Cette fois, le premier param�tre ~ctitre~C contient ~c"Mon Titre"~C qui est
imprim� avant la boucle. Par contre, la boucle ne s'ex�cute pas car le
param�tre ~clist~C est vide.

&IT Port�e de la d�finition d'une proc�dure
�������������������������������������������
1. Les proc�dures doivent �tre d�finies avant de pouvoir �tre appel�es.

2. Une fois d�finie, une proc�dure reste appelable au sein de la m�me session de IODE, m�me
si le rapport qui l'a d�finie est termin�. On peut ex�cuter un rapport qui n'a
d'autre effet que de charger des proc�dures en m�moire. Ces proc�dures resteront disponibles
pendant toutes la session IODE.

3. Une proc�dure peut �tre remplac�e par une autre du m�me nom � tout moment.

&IT Port�e des param�tres formels
���������������������������������
Les param�tres formels sont trait�s comme des defines dont la port�e est
limit�e � la proc�dure courante.

Par cons�quent, si un define existe ~uavant l'appelU de la proc�dure avec le m�me nom qu'un
des param�tres, ce define ne peut �tre utilis� au sein de la proc�dure.
Apr�s la proc�dure, il reprend sa valeur ant�rieure.

Exemple :
&CO
$define titre Quotients de mortalit�
$show Avant la proc : %titre%

$procdef print titre list
    $show Pendant la proc :%titre%
    $foreach i %list%
	$show print : %i%
    $next i
$procend

$procexec print "Mon Titre"
$show Apr�s la proc :%titre%
&TX

R�sultat :
&CO
Avant la proc : Quotients de mortalit�
Pendant la proc :Mon Titre
Apr�s la proc :Quotients de mortalit�
&TX

&TI E-Views
�����������
Dans la commande ~c$WsImportEviews~C,
la traduction de la fonction de taux de croissance (@PCH) en grt est remplac�e par 0.01 * grt.
&CO
    @PCH(X)  ->> 0.01 * grt(X)
&TX

>

<Version 6.39> (13/02/2013)
    Version 6.39 (13/02/2013)
    �������������������������
..esc ~

&TI Rapports
��������
Correction de bugs mineurs dans la construction de tableaux dans les rapports � l'aide
de commentaires (fonction ~c$DataUpdateTbl~C).
>

<Version 6.38> (11/02/2013)
    Version 6.38 (11/02/2013)
    �������������������������
..esc ~

&TI IODECMD
����������������
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
������������������
The error messages are now included in iodecmd (replacing msg like "error
##nnn" by something more readable).


&TI Reports
�����������

&IT $Quit or $QuitOde
���������������������
L'utilisation de la fonction $quit dans un rapport iode provoquait une erreur dans iodecmd.
Cette erreur est corrig�e.

&IT $EqsEstimate
���������������
Les coefficients inexistants avant l'estimation sont cr��s automatiquement.
Cela �vite de devoir les cr�er dans une commande s�par�e.

&IT $vseps, @vtake(), @vdrop(), @vcount()
�����������������������������������������
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
�������������������
Cette commande de rapport permet de simplifier l'�criture de
boucles ou de boucles imbriqu�es.

La commande $foreach permet de sp�cifier
un index et la liste de valeurs que cet index doit successivement prendre.

La commande $next permet de revenir au point de d�part de la boucle
($foreach) et de passer � la valeur suivante de l'index.

&SY2
    $foreach {index} {values}
    ...
    $next {index}
&TX
o�
&EN ~c{index}~C est un nom de macro de maximum 10 caract�res (par exemple ~ci, idx, PAYS~C , ...)
&EN ~c{values}~C est une liste de valeurs s�par�es par des virgules, blancs ou
point-virgules. Les s�parateurs peuvent �tre modifi�s par la commande ~c$vseps~C

Exemple 1 : boucles imbriqu�es
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
���������
La fonction goto prend un ou deux arguments :
&EN argument 1 : le label o� le programme doit pointer
&EN argument 2 (optionnel) :indique s'il faut ou non aller au label indiqu�.
Si cet argument est un nombre diff�rent de 0, le rapport se poursuit au
label indiqu�. Sinon le rapport se poursuit � la ligne suivante.

Auparavant, le rapport se poursuivait au label uniquement si l'argument 2 �tait 1. Toutes les
autres valeurs �taient �quivalentes � 0.

&TI Execution of identities
�������������������������
Augmentation des performances pour les grands ensembles d'identit�s et les
identit�s tr�s longues : cpu diminu� d'un facteur 20.

&TI Format Ascii
����������������
Les valeurs suivantes sont reconnues comme na dans les fichiers ASCII :
&CO
    .
    /
    na
&TX

Attention, deux points coll�s sont consid�r�s comme deux valeurs na distinctes. Ainsi,

&CO
    1 ./. 2 .. 3
&TX
�quivant � :
&CO
    1 na na na 2 na na 3
&TX

&TI  Estimation
���������������
L'estimation d'�quations dont les coefficients ont des valeurs nulles pouvait dans
certains cas �tre erronn�e. Les coefficients nuls sont fix�s � 0.1 avant de lancer le
processus d'estimation, ce qui pourrait dans certains cas poser probl�me.
>

<Version 6.37> (28/09/2012)
    Version 6.37 (28/09/2012)
    �������������������������
..esc ~

&TI Memory leaks
����������������
Diff�rentes corrections ont �t� apport�es pour permettre de gagner ou de
r�cup�rer de l'espace m�moire.

&EN IodeEnd() : suppression des WS
&EN SimulateSCC() : r�cup�ration de stracpy() des noms de �quations
&EN Repeat() : free(line)
&EN GMacro() : free(tmp)
&EN KI_scalar_list() : d�placement dans la boucle d'un free
&EN Remplacement de malloc, realloc et free par SCR_*().

>

<Version 6.36> (31/05/2012)
    Version 6.36 (31/05/2012)
    �������������������������
..esc ~

&TI Genereralized Samples
�������������������������
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
���������������
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
�����������������������������
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
��������������
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
�������������������
Object : extraction of equations, scalars and identities from E-Views export data.

&IT Format of E-Views data
����������������������������

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
�����������������������
Pour exploiter ce format, il faut appeler la fonction de rapport suivante :
&CO
    $WsImportEviews filename
&TX

o� filename est le nom du fichier � importer. Les WS courants sont augment�s des �quations
et scalaires d�tect�s.


&TI Comparaison de WS de variables
����������������������������������
Correction d'un bug qui rendait la comparaison des s�ries incorrecte (les
premi�res et derni�res valeurs n'�taient pas compar�es).

>

<Version 6.35> (28/03/2012)
    Version 6.35 (28/03/2012)
    �������������������������
..esc ~

&TI Simulation
��������������
La performance des simulations, sp�cifiquement lors du d�marrage (link, sort), a �t�
largement am�lior�e. Pour atteindre cet objectif, le processus de simulation
a �t� divis� en 2 �tapes. La premi�re ne s'occupe que du r�ordonnancement du
mod�le, la seconde de la simulation.

De plus, des am�liorations notables ont �t� apport�es � plusieurs
endroits en terme de vitesse d'ex�cution dans la phase de d�marrage. La
simulation d�marre quasi instantan�ment m�me avec un mod�le d�passant les
100 000 �quations.

&IT D�coupe de la simulation en deux passages
���������������������������������������������

Deux nouvelles fonctions de rapport et les deux �crans de saisie
correspondant ont �t� cr��s.

~uComposantes Fortement Connexes~U
������������������������������
La premi�re fonction permet de d�composer le mod�le en composantes fortement
connexes (CFC ou SCC pour Strongly Connex Components) et de le r�ordonnancer.
Trois listes sont donc cr��es : �quations pr�r�cursives,
interd�pendantes et postr�cursives.

Lors du r�ordonnancement du mod�le, le
nombre d'it�rations de triangulation (tri) pour le block interd�pendant doit
�tre sp�cifi�. Cette valeur n'a �videmment d'effet que sur la liste des
�quations interd�pendantes.

Ces 3 listes ne doivent �tre contruites qu'une seule fois pour une version donn�e du mod�le.

Cette premi�re fonction a la syntaxe suivante :

&CO
    $ModelCalcSCC nbtri lstpre lstinter lstpost
&TX
o� :

&EN nbtri est le nombre d'it�rations de triangulation � effectuer
&EN lst* sont les NOMS des listes destin�es � contenir les r�sultats du tri des �quations

~uSimulation~U
��������������
La seconde fonction lance la simulation du mod�le sur base des trois listes
pr�alablement construites par la fonction $ModelCalcSCC (ou � la main).

Sa syntaxe est :

&CO
    $ModelSimulateSCC from to pre inter post
&TX
o� :
&EN from et to d�terminent le sample de simulation
&EN pre, inter et post sont les listes qui d�finissent l'ordre d'ex�cution du mod�le.


&IT Choix du nombre de tris
���������������������������

Apr�s la d�compostion en CFC, le bloc interd�pendant est tri� pour augmenter la vitesse
de la simulation. Le nombre de passage de l'algorithme de tri peut �tre
sp�cifi� � plusieurs endroits :

&EN Dans l'�cran de simulation "standard" : param�tre "Passes" fix�
&EN Dans l'�cran de calcul de la d�composition du mod�le :  param�tre "Triangulation Iterations"
&EN Comme param�tre dans la commande rapport $ModelCalcSCC
&EN Comme dernier param�tre dans la commande rapport $ModelSimulateParms

&NO Dans les versions ant�rieures, le nombre de passages de la triangulation sp�cifi� dans
l'�cran de simulation n'avait pas d'effet

&IT Debug
���������
L'option debug activait la g�n�ration d'un fichier simul.dbg qui contenait une quantit� �norme
d'informations. Dans cette version, seules les listes ~c_PRE, _INTER~C et
~c_POST~C (avec la d�coupe du mod�le) sont g�n�r�es.

&IT Commande de rapport $ModelSimulateParms
�������������������������������������������
Cette commande prend un param�tre suppl�mentaire avec le nombre d'it�rations
de triangulation demand�es.

&TI Fonctions de rapport
������������������������

Les fonctions suivantes permettent d'obtenir la valeur de certains
param�tres de simulation :

&CO
    @SimEps()   : retourne la valeur du crit�re de convergence utilis� pour
		  la derni�re simulation
    @SimRelax() : retourne la valeur du param�tre de relaxation utilis� pour
		  la derni�re simulation
    @SimMaxit() : retourne la valeur du maximum d'it�rations utilis�
		  pour la derni�re simulation
&TX

Les r�sultats par p�riode de simulation peuvent �tre r�cup�r�s via les fonctions suivantes :
&CO
    @SimNiter(period) : nombre d'it�rations n�cessaires � la
		     r�solution du mod�le � l'ann�e period
    @SimNorm(period)  : seuil de convergence atteint � la r�solution
		     du mod�le � l'ann�e period
&TX

Un chrono virtuel a �t� ajout� pour permettre de calculer les dur�es de traitement :

&CO
    @ChronoReset() : remet le chrono � 0
    @ChronoGet()   : retourne le temps �coul� (en msecs) depuis le dernier
		  reset du chrono
&TX


&IT Exemple
�����������
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
����������������������������������
La comparaison des WS de varialbles s'effectait sur base d'une stricte �galit� binaire.
Par cons�quent les r�sultats de simulation avaient peu de chance d'�tre consid�r�s comme
�gaux, une diff�rence � 1e-15 suffisant � faire �chouer le test d'�galit�.

Une nouvelle fonction de rapport, ~c$DataCompareEps~C, permet de fixer le
seuil en-de�� duquel le test d'�galit� est consid�r� comme ayant �t�
satisfait.

Le test de comparaison est :
&CO
    si x1 <<>> 0 :  |(x1 - x2) / x1| << eps
    sinon :       |x2| << eps
&TX

&SY
    $DataCompareEps eps
&TX

L'�cran de comparaison de WS (Data/List/File Compare) permet �galement de
sp�cifier cette valeur (param�tre Threshold).

&TI Seuil de comparaison par d�faut
���������������������
Le seuil de comparaison est fix� � 1e-7 par d�faut.
>



<Version 6.34> (29/02/2012)
    Version 6.34 (29/02/2012)
    �������������������������
..esc ~

&TI Commandes d'impression en rtf et mif
����������������������������������������
Les commandes de num�rotation des pages et de d�finition des headers et footers de pages
ont �t� impl�ment�es pour les sorties rtf (Word) et mif (Framemaker).

Par exemple :
&CO
    $printdest resfun.rtf RTF
    $printpagefooter Ecopol - @date("dd/mm/yy") - page%d
    $printpageheader Hypoth�ses et r�sultats de la projection de base
&TX

Par ailleurs, une partie du catalogue de paragraphes est supprim�e : il s'agit par
exemple de par_2, enum_6, ... qui ne sont jamais utilis�s.


&TI Hodrick Preskott
��������������������

Am�lioration de la fonction dans le cas o� l'expression contient des NaN :
les premi�res et derni�res valeurs NaN sont skipp�es.


&TI Nouvelles fonctions de rapport
������������������������������
De nouvelles fonctions de rapports permettent d'automatiser plus facilement
certains processus, notamment en extrayant des informations des objets
�quations.

Le SAMPLE d'estimation d'une �quation peut �tre retrouv� dans un rapport � l'aide
de la fonction ~c@eqsample~C. La partie "FROM" et la partie "TO" sont obtenues
respectivement par les fonctions @eqsamplefrom et @eqsampleto.

A l'aide de ces fonctions, il devient facile d'automatiser le processus de
r�estimation.

&CO
    @eqsample(eqname)     : retourne le sample d'estimation de l'�quation ~ceqname~C
    @eqsamplefrom(eqname) : retourne la partie FROM du sample d'estimation
    @eqsampleto(eqname)   : retourne la partie TO du sample d'estimation
    @eqlhs(eqname)        : retourne le membre de gauche d'une �quations
    @eqrhs(eqname)        : retourne le membre de droite d'une �quations
&TX

Dans l'exemple suivant, on extrait la premi�re ann�e d'estimation de l'�quation ENDO1
pour r�estimer l'�quation sur un sample prolong� � droite.

&CO
    $EqsEstimate @eqsamplefrom(ENDO1) 2012Y1 ENDO1
&TX



&TI Transferts entre Excel et IODE dans les rapports
����������������������������������������������������
Avant cette version de IODE, pour qu'un transfert de donn�es entre Excel et
IODE via la fonction ~c$ExceLGetVar~C puisse fonctionner, le format des
nombres � transf�rer ne pouvait contenir de s�parateur de milliers ni de
signe de devise comme $ ou �.

A partir de :  cette version, les formats contenant des s�parateurs de milliers ou des
devises sont correctement interpr�t�s.

Par d�faut, IODE lit la d�finition des valeurs des s�parateurs de milliers
et le caract�re des devises dans les param�tres "r�gionaux" (Regional
Settings). Si ces informations ne conviennent pas, elles peuvent �tre
modifi�es par les commandes de rapport suivantes :

&CO
    $ExcelThousand
    $ExcelCurrency
&TX

&IT $ExcelThousand
���������������������
Syntaxe :
&CO
    $ExcelThousand [char]
&TX

La valeur de char remplace celle du s�parateur de milliers d�finie dans les
param�tres de Windows. Lorsque ce caract�re est rencontr� lors du transfert
d'Excel vers IODE, il est ignor�.

Certaines valeurs sp�ciales de ~cchar~C ont une interpr�tation particuli�re :

&EN d, D, p ou P : le s�parateur est le point
&EN c ou C : le s�parateur est la virgule
&EN ~cspace~C ou ~cs~C ou ~cSpace~C : le s�parateur est l'espace
&EN n, N ou pas d'argument : il n'y a pas de s�parateur
&EN tout autre valeur est prise telle quelle comme s�parateur

&IT $ExcelCurrency
���������������������
Syntaxe :
&CO
    $ExcelCurrency [char]
&TX

La valeur de char remplace celle du caract�re indiquant la devise dans les
param�tres de Windows. Lorsque ce caract�re est rencontr� lors du transfert
d'Excel vers IODE, il est ignor�.

Certaines valeurs sp�ciales de ~cchar~C ont une interpr�tation particuli�re :

&EN d, D : dollar
&EN e ou E : euro
&EN p ou P : sterling pound
&EN pas d'argument : il n'y a pas de signe de devise
&EN tout autre valeur est prise telle quelle comme signe de devise

&IT Exemple
�����������
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
    �������������������������
..esc ~

&TI Transfert IODE vers Excel dans les rapports : ~c$ExcelLang,~C ~c$ExcelDecimal~C
���������������������������������������������������������������������������
Deux nouvelles fonctions permettent de sp�cifier dans les rapports la
version linguistique d'Excel et la s�parateur d�cimal qui doit �tre utilis� dans les
transfert de IODE vers Excel.

&IT $ExcelLang
��������������
Par d�faut, IODE consid�re que la version linguistique de Excel est
l'anglais. Cela implique que les ~cRanges~C dans Excel doivent s'�crire
~cRnCn~C (par exemple R2C4 pour ligne (Row) 2, colonne (Column) 4).

Dans les autres langues, cette �criture ne convient plus et IODE doit
envoyer les donn�es par exemple vers ~cL2C4~C au lieu de ~cR2C4~C dans la
version fran�aise.

Pour assurer qu'un rapport IODE reste valable quelle que soit la langue
d'Excel, la commande ~c$ExcelLang~C a �t� ajout�e � IODE. Elle a pour
syntaxe :

&CO
    $ExcelLang {F|N}
&TX
Tout autre param�tre (E par exemple) remettra IODE en version anglaise.

Exemple
�������
Dans l'exemple ci-dessous, IODE enverra le contenu de la variable AAA �
partir de la cellule L1C1 du Sheet1 du fichier Excel ouvert.

&CO
    $ExcelLang F
    $ExcelSetVar AAA Sheet1!R1C1
&TX

&NO Il faut noter que dans la commande ~c$ExcelSetXxx~C, ~ble range reste d�fini
comme ~cR1C1~C~B. C'est uniquement en interne qu'IODE transformera cette
information en ~cL1C1~C.

&IT $ExcelDecimal
��������������
Par d�faut, IODE envoie les donn�es num�riques vers Excel en utilisant
 ~ble s�parateur d�cimal d�fini dans les "Regional Settings" de Windows
 (voir Control Panel)~B.

Cette option ne fonctionne que si le s�parateur d�cimal n'a pas �t� modifi�
dans Excel : on peut en effet d�cider d'utiliser le ~bpoint d�cimal~B dans Excel alors
que dans Windows, c'est la ~bvirgule~B qui a �t� choisie.

Pour permettre de forcer l'envoi vers une version Excel dans laquelle le s�parateur d�cimal
ne serait pas celui de Windows, une nouvelle fonction de rapport a �t� introduite. Sa syntaxe est :

&CO
    $ExcelDecimal {C}
&TX
L'absence de param�tre C ou tout autre param�tre que C (comma) s�lectionne le point d�cimal.

Exemple
�������
Dans l'exemple ci-dessous, IODE enverra le contenu de la variable AAA avec des virgules comme s�parateur d�cimal
� partir de la cellule L1C1 du Sheet1 du fichier Excel ouvert.

&CO
    $ExcelDecimal C
    $ExcelLang F
    $ExcelSetVar AAA Sheet1!R1C1
&TX

>

<Version 6.32> (17/05/2011)
    Version 6.32 (17/05/2011)
    �������������������������
..esc ~

&TI Rapports : $WsExtrapolate, $DataScan, $EqsEstimate
�������������������������������������������
$WsExtrapolate, $DataScan, $EqsEstimate : lorqu'une variable fournie comme
argument n'existait pas, le rapport continuait son ex�cution m�me si
~c$OnError~C �tait fix� � ~cAbort~C ou ~cExit.~C

Ainsi, dans l'exemple qui suit, le message Hello s'affiche m�me si la
variable ~cUnknownVar~C n'existe pas.

&CO
    $OnError Abort
    $WsExtrapolate 1 1990Y1 2000Y1 UnknowVar
    $echo Hello
&TX


&TI Editeur de rapports
�����������������������
Les couleurs de l'�diteur de rapport ont �t� modifi�es pour am�liorer la lisibilit�.

&TI Graphiques
��������������
&EN Added MULTIBAR PERCENT to HTML
&EN Changed MULTIBAR (HTML, RTF) to category chart (ex 1990,1991,2010, 2020)
&EN Corrected bug when " (double quote) is in the graph title (HTML only)
&EN Corrected bug in legends (RTF ldo example)

&TI Users Manual
����������������
A new chapter "Methods and algorithms" has been added to the users manual.

>

<Version 6.31> (05/04/2011)
    Version 6.31 (05/04/2011)
    �������������������������
..esc ~

&TI IODECOM
������������
Ajout du programme IodeComServer dans la distribution de IODE.
>

<Version 6.30> (28/03/2011)
    Version 6.30 (28/03/2011)
    �������������������������
..esc ~


&TI Corrections
��������������
L'affichage de messages trop longs plantait IODE : cette erreur est
corrig�e, la limite d�pendant maintenant de la taille de l'�cran.

&IT Graphiques en MIF
���������������������
Am�lioration des textes dans les graphiques : adaptation de la taille des caract�res � la d�finition a2m

&IT Affichage
�������������
La taille minimum des caract�res est port�e � 6 points au lieu de 4 auparavant, ce qui �tait illisible sur
la plupart des �crans.
>


<Version 6.29> (17/03/2011)
    Version 6.29 (17/03/2011)
    �������������������������
..esc ~


&TI Programme d'installation d'un upgrade
������������������������������������������
Le nouveau programme d'installation de IODE permet d'associer des fichiers � IODE.
Pour ce faire, il faut augmenter les droits de l'utilisateur qui installe le programme en
les pla�ant au niveau d'Administrateur.

Apr�s l'installation initiale, et donc apr�s l'association entre
les fichiers iode (.var, .eqs, ...) et le programme lui-m�me. cette augmentation n'est plus n�cessaire
pour remplacer le programme ou les fichiers constituant le logiciel et sa documentation.
Un installateur "upgrade" a donc �t� construit pour permettre � l'utilisateur d'effectuer lui-m�me la mise
� niveau de sa version.


&TI Rapports
������������
&EN La commande ~c$quit~C est ajout�e et est synonime de $quitode
&EN La commande ~c$shift~C peut avoir comme param�tre le nombre de ~c"shift"~C � effectuer

&CO
    $shift 2
&TX


>
<Version 6.28> (14/03/2011)
    Version 6.28 (14/03/2011)
    �������������������������
..esc ~


&TI Nouvelle version du programme d'installation
�����������������������������������������������
Cette version int�gre les nouveaut�s suivantes :

&EN restriction d'acc�s aux comptes administrateurs
&EN r�tablissement de la liaison automatique entre les fichiers de IODE et le programme IODE, y compris sous
    Vista et Windows 7.
&EN s�l�ction des composants � installer :
&EN2 Shortcuts dans le menu d�marrer (onglets IODE)
&EN2 Th�mes et styles pour les graphiques (nouveau)
&EN2 Mod�le fun d'exemple (nouveau)
&EN2 Version COM de IODE (enregistrement automatique), permettant un acc�s
    optimis� aux fonctions de IODE par exemple en VBA (Excel, ...) ou en APL (nouveau)
&EN2 Configuration de Textpad et de Notepad++ pour l'�dition des rapports de
    IODE (colorisation syntaxique et auto-completion) (nouveau)
&EN2 Fichiers Excel et APL d'interfa�age avec IODE


&TI Resize de la fen�tre de IODE
��������������������������������
La taille de la fen�tre est m�moris�e dans le fichier de profil de IODE. Au prochain d�marrage, cette
taille est donc reprise par d�faut.


&TI Largeur de la colonne des noms d'objets
�������������������������������������������
La largeur de la premi�re colonne des tableaux d'objets est m�moris�e pour
les sessions suiVantes dans le fichier de profil de IODE.

>
<Version 6.27> (03/03/2011)
    Version 6.27 (03/03/2011)
    �������������������������
..esc ~

&TI Resize de la fen�tre de IODE
��������������������������������

La taille de la fen�tre de IODE peut �tre adapt�e par l'utilisateur :

&EN en tirant sur le bord de la fen�tre
&EN en double-cliquant dans le titre de la fen�tre
&EN en indiquant le nombre de lignes et de colonnes dans la ligne de commande

En combinaison avec une diminution de la taille des caract�res, cela permet de visualiser
un nombre beaucoup plus grand d'informations sur l'�cran.

&TI Taille de caract�res
������������������������
La combinaison de touches ~cCtrl+Wheel~c + ~cUp~C ou ~cDown~C augmente ou diminue la taille de la police de
caract�res courante (limit� entre 3 points et 50 points). La taille de la fen�tre
s'adapte en cons�quence.

La taille initiale peut �tre d�termin�e au d�marrage de IODE via le param�tres ~c-fontsize~C  ~cnb_points~C.

&TI Fermeture de la fen�tre
���������������������������
La croix dans le coin sup�rieur droit de la fen�tre permet de quitter IODE (comme le ferait ~cAlt+X~C).


&TI D�placement dans les tableaux d'�dition
�������������������������������������������
La roulette de la souris ("Mouse wheel") permet de d�placer le tableau d'objets et cours d'�dition vers
le haut ou vers le bas.

&TI New parameters
������������������
New parameters in the iode command line have been created :

&EN ~c-nl~C ~cnumber~C : number specifies the number of lines of the Iode window
&EN ~c-nc number~C : : number becomes the number of columns of the Iode window
&EN ~c-fontsize points~C : specify the number of points of the characters (3-50).
&EN ~c-fontname "fontname"~C : define the font to use in the iode windows
&EN ~c-pbf file~C : indicates that Iode will "play" at startup the keystrokes present in the file
&EN ~c-pbs "string"~C : same as pbf buf with a string defined on the command line


&TI Syntaxe des s�quences ~cplayback~C
����������������������������������
Une s�quence ~cPlayback~C consiste en une suite de touches, de commentaires et de messages.

Les r�gles suivantes s'appliquent :

&EN Tous les caract�res d'espacement (nl, return, ff, tab) sont
    ignor�s sauf le blanc.

&EN Tous les caract�res ascii sont envoy�s dans le buffer de lecture, blanc compris
&EN Les s�quences suivantes indiquent que le premier caract�re qui suit (ou la touche @..) sera
modifi� par ~cAlt,~C ~cCtrl~C ou ~cshift~C.
&EN2 ~c&a~C : le prochain caract�re sera ~calt+char~C
&EN2 ~c&c~C : le prochain caract�re sera ~cctrl+char~C
&EN2 ~c&s~C : le prochain caract�re sera ~cshift+char~C

&EN Le d�lai d'attente entre deux touches peut �tre modifi� � l'aide des s�quences suivantes :

&EN2 ~c&d0~C : le d�lai d'attente entre deux touches est annul�
&EN2 ~c&d-~C : le d�lai d'attente entre deux chars est diminu� de 50 ms
&EN2 ~c&d+~C : le d�lai d'attente entre deux chars est augment� de 50 ms

&EN La suite de l'ex�cution peut �tre interrompue et un message peut �tre affich� :
&EN2 ~c&m[Msg]~C : affiche le message Msg et attend qu'on presse une touche
    avant de reprendre. Le message se termine � la fin de la ligne courante.
&EN ~c&n~C : �quivalent d'un retour � la ligne dans le message
&EN ~c&autre_char~C : renvoie autre_char

&EN Les s�quences suivantes renvoient des touches sp�ciales :
&EN2 @F1 � @F9 : renvoie SCR_F1 � F9 (ou Shift, Ctrl, Alt en fonction de ce qui pr�c�de)
&EN2 @FA � @FJ : renvoie SCR_F10 � F19 (ou Shift, Ctrl, Alt en fonction de ce qui pr�c�de)
&EN2 @l : renvoie SCR_CSR_LEFT (ou Shift, Ctrl, Alt en fonction de ce qui pr�c�de)
&EN2 @r : renvoie SCR_CSR_RIGHT (ou Shift, Ctrl, Alt en fonction de ce qui pr�c�de)
&EN2 @u : renvoie SCR_CSR_UP (ou Shift, Ctrl, Alt en fonction de ce qui pr�c�de)
&EN2 @d : renvoie SCR_CSR_DOWN (ou Shift, Ctrl, Alt en fonction de ce qui pr�c�de)
&EN2 @p : renvoie SCR_CSR_PG_UP  (ou Shift, Ctrl, Alt en fonction de ce qui pr�c�de)
&EN2 @n : renvoie SCR_CSR_PG_DN  (ou Shift, Ctrl, Alt en fonction de ce qui pr�c�de)
&EN2 @h : renvoie SCR_CSR_HOME  (ou Shift, Ctrl, Alt en fonction de ce qui pr�c�de)
&EN2 @e : renvoie SCR_CSR_PG_END (ou Shift, Ctrl, Alt en fonction de ce qui pr�c�de)
&EN2 @I : renvoie SCR_INSERT
&EN2 @D : renvoie SCR_DELETE
&EN2 @R : renvoie SCR_ENTER (ou Shift, Ctrl en fonction de ce qui pr�c�de)
&EN2 @E : renvoie SCR_ESCAPE
&EN2 @B : renvoie SCR_BACKSPACE (ou Shift, Ctrl en fonction de ce qui pr�c�de)
&EN2 @T : renvoie SCR_TAB (ou Shift, Ctrl en fonction de ce qui pr�c�de)
&EN2 @autre_char : renvoie autre_char

&EN Le caract�re ## marque le d�but d'un commentaire qui va jusqu'� la fin de
la ligne

&IT Exemples
�����������
La commande qui suit d�marre iode et effectue les op�rations suivantes :
&CO
    iode -pbs "@R&adt@R@R"
&TX

&EN ~c@R~C : enter dans l'�cran d'accueil
&EN ~c&ad~C : Alt+D : va dans le menu Data
&EN ~ct~C : va sur l'option Tables
&EN ~c@R~C : entre dans le sous-menu
&EN ~c@R~C : ouvre l'option "Edit WS"

Le fichier qui suit reprend l'essentiel des possibilit�s.
&CO
    ## Diminue le d�lai des touches
    &d0
    ## Affiche un message et attend la touche ENTER
    &mLoad workspaces : Menu Workspace, function Load Work Space
    ## Entre dans la page Load WS
    @R@r@R@R
    ## Introduit le nom des fichiers � charger (@T = TAB pour passer au suivant)
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
    ## Alt+D : se d�place dans le menu Data
    &ad
    ## T, Enter, Enter : va sur Tables, et entre dans le scroll d'�dition
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
    ## Idem avec un d�lai de 150ms
    ## ---------------------------
    &mM�me s�quence avec un d�lai de 150 ms
    &d0&d+&d+&d+
    ## Alt+D : se d�place dans le menu Data
    &ad
    ## T, Enter, Enter : va sur Tables, et entre dans le scroll d'�dition
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
    �������������������������
..esc ~

&TI Division par 0 en LEC
�������������������������
La nouvelle fonction LEC ~cdiv0(x,y)~C retourne 0 lorsque y vaut 0.
&CO
    div0(x, y) = 0 si y = 0
&CO
    div0(x, y) = x / y sinon
&TX

&TI Corrections LEC
�������������������
Deux corrections ont �t� apport�es dans le traducteur LEC :

&EN la limite de longueur des expressions LEC apr�s remplacement des macros
    ($NAME) est port�e de 4000 � 40000 caract�res.

&EN lorsque le nombre d'arguments d'une fonction � nombre d'arguments variable d�passe 255
    l'erreur est correctement signal�e. Auparavant, elle �tait dans certains
    cas ignor�e et g�n�rait des r�sultats impr�sibles (exemple fonction ~clsum~C).

&TI Correction Rapports
�����������������������
Dans le cas de certaines erreurs LEC (expressions trop longues), l'ex�cution
des rapports s'arr�tait. Un message d'erreur est dor�navant affich� dans ce cas.



>

<Version 6.25> (17/06/2010)
    Version 6.25 (17/06/2010)
    �������������������������
..esc ~

&TI Graphiques en Flash dans les pages HTML
�������������������������������������������
Les graphiques g�n�r�s dans le format HTML sont remplac�s par une version
Flash (Fusion Chart). Cela signifie que lorsqu'on choisit de g�n�rer en
sortie un fichier HTML, ceux-ci contiennent la d�finition de graphiques
exploitables en flash.

Ces graphiques n�cessitent une librairie dont la source est d�finie dans le
fichier ~cswf.ini~C. Par d�faut, ce fichier contient les donn�es suivantes :

&CO
[SWF]
    JSPATH=http:/www.plan.be/FusionCharts/JSClass/FusionCharts.js
    GRAPHPATH=http:/www.plan.be/FusionCharts/Charts
    ANIMATE=0
    ROUNDED=1
    GRADIENT=1
&TX
o�

&EN ~cJSPATH~C indiquent o� trouver le programme de d�finition des graphiques

&EN ~cGRAPHPATH~C indiquent o� trouver les librairies flash

&EN ~cANIMATE~C vaut 1 pour que l'affichage soit progressif

&EN ~cROUNDED~C vaut 1 pour avoir des coins des cadres et des barres "arrondis"

&EN ~cGRADIENT~C indique si les couleurs doivent �tre pleines (0) ou progressives (1)


>

<Version 6.24> (15/04/2010)
    Version 6.24 (15/04/2010)
    �������������������������
..esc ~

&TI Ouverture de fichiers sur r�seau
�����������������������������������
Iode se plantait parfois lors de l'ouverture de fichiers sur r�seau � partir
de l'Explorateur Windows ou lorsque le r�pertoire par d�faut �tait un
r�pertoire r�seau. Cette erreur est corrig�e.

&TI Limitation des noms de fichiers
�����������������������������������
&EN La limite des noms de fichiers � 63 caract�res reste
d'application pour des raisons de compatibilit� entre les versions
successives des fichiers workspace.

&EN Les noms de fichiers ne doivent si possible pas contenir de s�parateur
comme l'espace ou la virgule, y compris dans le nom des r�pertoires. En
effet, l'ouverture � partir de l'Explorateur Windows ne fonctionne pas dans
ce cas.

>

<Version 6.23> (01/04/2009)
    Version 6.23 (01/04/2009)
    �������������������������
..esc ~


&TI Gestion m�moire
��������������
R�vision des allocations dans les rapports :
&EN nouvelle librairie d'allocation de m�moire
&EN optimisation de l'espace m�moire allou� (�vite en partie le morc�lement
progressif de la m�moire dans le cas des longs rapports)

&EN nouvelle version des librairies C++
�����������������
Ces modifications augmentent la performance de certaines fonctions et
permettent de passer � 2GB de RAM utilis�e.



&TI Rapports
������������
R��criture de la fonction $Repeat pour :

&EN gain (parfois important) de vitesse
&EN r�soudre le probl�me des rapports "crois�s" (le fichier temporaire
    ~cws.rep~C n'est plus utilis�)

&TI Graphiques Stacked
����������������������
Programmation des graphiques barres "stacked" en Frame (MIF) et � l'�cran.

&TI Police de caract�res pour les graphiques
��������������������������������������������
Nouveau param�tre optionnel ~cfontsize~C  permettant de pr�ciser la taille des caract�res
dans les graphiques.

&CO
    $PrintGraphSize width height [fontsize]

    o� :
	width et height repr�sentent la taille du graphique en mm
	fontsize donne la taille de la police du graphique en points
&TX


>

<Version 6.22> (18/11/2008)
    Version 6.22 (18/11/2008)
    �������������������������
..esc ~


&TI Graphiques
��������������
Quelques am�liorations ont �t� apport�es dans l'affichage des graphiques :
&EN suppression de "Minor Ticks" sur l'axe des X
&EN modification du placement des "Minor Ticks" sur l'axe des Y et Z

&TI Corrections :
�����������������
Apr�s le chargement des "th�mes" graphiques (.thm), le directory d'origine
est restaur�.

>

<Version 6.21> (27/10/2008)
    Version 6.21 (27/10/2008)
    �������������������������
..esc ~


&TI M�thode d'estimation "Maximum likelihood" (BHHH)
����������������������������������������������������
Une nouvelle m�thode d'estimation est propos�e dans l'�cran de d�finition
des �quations : MaxLik (Berndt, Hall, Hall and Hausman). La m�thode est
d�crite dans les "Annals of Economic and Social Measument, 3/4, 1974".

Il s'agit en fait d'une g�n�ralisation des m�thodes existantes qui modifie
la direction de d�placement de la solution � chaque it�ration et non pas une
seule fois (comme dans les cas des m�thodes 3SLS ou Zellner) ou aucune fois
(comme dans les autres cas, soit LSQ ou INSTR).


&TI Format ASCII
������������
Dans le format de d�finition des �quations (fichiers .ae), la m�thode doit
s'�crire MAXLIK (� la place par exemple de LSQ).


&TI Fonction de rapport $EqsSetMethod
�������������������������������������
Le num�ro d'ordre de la m�thode MAXLIK est 4.

&CO
    $EqsSetMethod   {0|1|2|3|4} eqname1 eqname2 ...
	o� 0 indique la m�thode LSQ (moindres carr�s)
	1 indique la m�thode Zellner
	2 indique la m�thode INF (2 stages avec instruments)
	3 indique la m�thode GLS (3 stages avec instruments)
	4 indique la m�thode MAXLIK
	eqname1 .. sont des noms d'�quations existantes
&TX


&EN Formats ASCII
�����������������
Dans les formats des variables et des scalaires, la valeur NaN peut �tre indiqu�e
soit par na, soit par /, pour permettre la lecture des donn�es provenant de SAS.

&TI Sample g�n�ralis�
���������������������
Dans le cas des fichiers, il est possible d'imprimer une colonne en "base"
d'un autre fichier, c'est-�-dire en divisant le fichier courant par un autre et en

Par exemple le sample suivant sur les fichier 1 et 2 :
&CO

    2000[1;2;1=2]
&TX

pourrait donner comme r�sultat :

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
����������������������������������������������
Dans le cas de l'impression d'�quations avec remplacement des coefficients
par leurs valeurs (fonction "Print Objects definition"),
le nombre de d�cimales demand�es dans l'�cran ou dans le rapport sp�cifie le
nombre de d�cimales de chaque coefficients.

La valeur -1 peut toujours �tre utilis�e pour que le syst�me imprime le
maximum de d�cimales disponibles.

Ce m�me nombre de d�cimales est utilis� pour l'impression des t-tests dans l'�quation.

Cette modification a un impact sur l'impression des Equations et des Identit�s.


&TI Impression des scalaires
����������������������������
L'impression des scalaires a �t� revue :

&EN le nombre de d�cimales indiqu�es � l'�cran est utilis� pour les valeurs
des coefficients, des standard errors et des tests t

&EN les valeurs na sont imprim�es comme -.-


>

<Version 6.20> (07/04/2008)
    Version 6.20 (07/04/2008)
    �������������������������
..esc ~


&TI Graphs
����������
Bug bij het printen naar RTF opgelost.


&TI Nieuwe LEC functies
�����������������������

&CO
    gamma(x) = gamma functie
&TX

>

<Version 6.19> (07/02/2008)
    Version 6.19 (07/02/2008)
    �������������������������
..esc ~

&TI DataCalcLst
���������������

Een lijst kan worden opgebouwd door het product te maken van twee bestaande
lijsten.

&CO
    $DataCalcLst RES ONE x TWO
&TX

De lijst RES wordt gemaakt bestaande uit alle combinaties (product) van
lijst ONE en TWO.

&IT Voorbeeld
�������������
&CO
    $DataUpdateLst L1 R1;R2
    $DataUpdateLst L2 C1;C2;C3
    $DataCalcLst L1_L2 L1 x L2

L1_L2 = R1C1;R1C2;R1C3;R2C1;R2C2;R2C3
&TX

&TI Nieuwe LEC functies
�����������������������


&EN urandom(s) = random uniform verdeeld in het interval [0,s[

&EN grandom(m, s) = random variabele met een normal distributie met gemiddelde m
en standaard afwijking s
>

<Version 6.18> (01/12/2008)
    Version 6.18 (01/12/2008)
    �������������������������
..esc ~

&TI Grafieken in Word
�������������

De grafieken zijn aangepast aan wat er bestond in FrameMaker. Als u
afdrukt in RTF (Word), krijgt u dezelfde grafieken als op het scherm.

&TI MultiBarCharts
������������������
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
iode ge�nstalleerd is.


&IT Voorbeeld
�������������
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
�������������

Plaats een gekleurde achtergrond (bv. om forcast periode aan te duiden)

&SY
    $PrintGraphBand 2007Y1 2011Y1

    $PrintGraphBand zonder argumenten heft het commando op
&TX

>

<Version 6.17> (20/07/2007)
    Version 6.17 (20/07/2007)
    �������������������������
..esc ~

&TI Nouvelles fonctions d'exportations des graphiques
�����������������������������������������������������
Les fonctions graphiques ont �t� am�lior�es dans de nombreux aspects. Ces fonctions
permettent notamment maintenant d'exporter directement des graphiques dans
une dimension ad�quate vers Word.

&TI Fonction LEC app()
���������������������
La fonction
~capp()~C du LEC a �t� am�lior�e pour les cas de s�ries contenant des
valeurs nulles.

Le r�sultat de la fonction
~capp(A,~C B) est obtenu comme suit au
temps t :

&EN Si
~cA[t]~C est d�fini,
~cA[t]~C est retourn�

&EN Si
~cA[t]~C n'est pas d�fini, on calcule les valeurs
~ct0~C et
~ct1~C autour de
~ct~C
telles que
~cA[t0]~C et
~cA[t1]~C soient d�finis et non nuls. Si ni
~ct0~C ni
~ct1~C ne
peuvent �tre trouv�s, retourne
~cNaN.~C Il s'agit dans ce cas d'un s�rie
~cA~C
valant ~cNaN~C sur toute la p�riode.

&EN si seul
~ct0~C est d�fini, retourne
~cB[t] * (A[t0] / B[t0])~C

&EN Si seul
~ct1~C est d�fini, retourne
~cB[t] * (A[t1] / B[t1])~C

&EN Si
~ct0~C et
~ct1~C sont d�finis, calcule d'abord
~cDelta = (A[t1]/A[t0]) / (B[t1]/B[t0])~C
puis retourne
~cA[t0] * (B[t]/B[t0]) * Delta ** ((t-t0)/(t1-t0))~C

>


<Version 6.16> (14/06/2007)
    Version 6.16 (14/06/2007)
    �������������������������
..esc ~

&TI Fonction LEC if()
���������������������
Dans le cas d'une condition dont la valeur est NaN, le r�sultat de la
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
    �������������������������
..esc ~

&TI Gestion m�moire (en test)
���������������������
La gestion des objets en m�moire est modifi�e pour permettre la cr�ation
d'un plus grand nombre d'objets. On constate en effet que, suivant la taille m�moire
disponible, le nombre maximum d'objets d'un m�me type ne pouvait d�passer
les 300 � 400 000.

Cette version permet de cr�er plusieurs millions d'objets d'un m�me type :
des tests ont �t� men�s jusqu'� 2.850.000 objets.

Cette modification a un co�t : les blocs m�moire allou�s aux objets �tant
plus grands, les manipulations (modifications, destructions, cr�ations)
peuvent parfois �tre un peu ralenties.

&TI Cr�ation d'objets
���������������������
Lors de la cr�ation de nombreux objets � partir d'un rapport, l'ajout des nouveaux
objets dans la tables des objets peut devenir relativement lent en raison du
r�ordonnancement constant de la table des noms n�cessit� par cette
op�ration d'insertion. Ce probl�me dispara�t cependant lorsque les noms sont
cr��s dans l'ordre alphab�tique : le gain de temps pour quelques centaines de milliers
d'objets peut atteindre un rapport de 10.

Dans l'exemple suivant, le nom des objets cr�es le sont par ordre
alphab�tique (la fonction @fmt permet de formater les entiers : on cr�e donc
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
    �������������������������
..esc ~
&TI Messages d'erreur
���������������������
Quelques messages d'erreur n'�taient pas correctement repr�sent�s � l'�cran (MSG##nnn apparaissait � la place du texte).
Cette erreur est corrig�e.

&TI M�moire disponible
����������������������
La quantit� de m�moire disponible affich�e dans le bas de l'�cran de IODE repr�sente
dor�navant la m�moire RAM libre et plus, comme c'�tait le cas dans les versions
pr�c�dentes, toute la m�moire, y compris la m�moire virtuelle.

Cette nouvelle valeur est plus indicative sur la quantit� utilement
disponible : la m�moire pagin�e sur disque est particuli�rement lente et
l'utiliser n'est pas r�ellement praticable.

>
<Version 6.13> (22/11/2006)
    Version 6.13 (22/11/2006)
    �������������������������
..esc ~
&TI Longueur des noms de fichiers
���������������������������������

La longueur des noms de fichiers Iode est limit�e � 63 caract�res.
De fa�on g�n�rale, les noms trop longs sont une source de probl�mes dans
IODE. Modifier cette limite
imposerait de transformer tous les WS (qui contiennent en interne le nom sur 63
caract�res max).

Les modifications suivantes ont �t� apport�es dans cette version pour �viter les
plantages du programme :
&EN les noms de rapports de plus de 63 caract�res sont accept�s dans l'�cran de
    lancement des rapports et dans les rapports IODE eux-m�mes
    ~c($ReportExec)~C

&EN Si une tentative de sauvetage de fichiers Iode est effectu�e avec un nom de
    fichier de plus de 63 caract�res, un message d'erreur est produit et le processus s'arr�te.

>
<Version 6.12> (22/06/2006)
    Version 6.12 (22/06/2006)
    �������������������������

&TI Grafieken
�������������

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
��������������������������

&IT 1. DataRasVar
�����������������

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
���������������������

Deze functie cre�ert lijsten met daarin de namen van objecten
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
������������������
Maakt alle WS-en leeg.

&IT 4. @evalue
��������������

Geeft de formule van de EQS opgegeven als argument.

>
<Version 6.11> (10/11/2004)
    Version 6.11 (10/11/2004)
    �������������������������

&TI Nouvelles fonctions de rapport
������������������������������
&EN ~b@sqz(string)~B : supprime les blancs de string
&EN ~b@strip(string)~B : supprime les blancs de fin de string

>

<Version 6.10> (18/10/2004)
    Version 6.10 (18/10/2004)

..esc ~
&TI Nouvelles fonctions LEC
�����������������������

Trois nouvelles fonctions sont introduites dans le langage LEC :

&EN  ~cfloor(expr)~C : partie enti�re de l'expression
&EN  ~cceil(expr)~C : partie enti�re de l'expression plus 1
&EN  ~cround(expr [, n])~C : arrondi de expr � la n�me d�cimale. Si n n'est pas d�fini, il est fix� � 0.

&TI Nouvelles fonctions de rapport
����������������������������������

&EN ~c@time([format])~C : renvoie l'heure. Le format contient ~chh~C pour l'heure,
    ~cmm~C pour les minutes et ~css~C pour les secondes. Par d�faut, le format est : ~c"hh:mm:ss"~C.

&EN ~c@date([format])~C : renvoie la date. Le format contient ~cdd~C pour le
    jour, ~cmm~C pour le mois et ~cyyyy~C pour l'ann�e. Par d�faut, le
    format est ~c"dd-mm-yyyy"~C

&EN ~c@take(n,string)~C : extrait les ~cn~C premiers caract�res de ~cstring~C. Si ~cn~C est n�gatif, extrait les n derniers.
&EN ~c@drop(n,string)~C : supprime les ~cn~C premiers caract�res de ~cstring~C. Si ~cn~C est n�gatif, supprime les n derniers.
&EN ~c@count(string)~C : retourne le nombre d'�l�ments de string (s�par�s par des virgules)
&EN ~c@index(n,list)~C : retourne l'�l�ment ~cn~C de ~clist~C

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
    ���������������������������������������������������������������������������

..esc ~

&TI Correction de la fonction de rapport $DataUpdateVar
�������������������������������������������������������

Cette fonction interpr�tait comme ~cna~C les valeurs ~c+0.0~C ou ~c-0.0~C ou
~c.0~C. Cette erreur est corrig�e.
>
<Version 6.08> (16/08/2004)
    Version 6.08 (16/08/2004)
    ���������������������������������������������������������������������������

..esc ~

&TI Int�gration de valeurs d'Excel dans les rapports
����������������������������������������������������
Si un worksheet Excel contient des donn�es n�cessaires � l'ex�cution d'un rapport,
il est facile d'int�grer dynamiquement des valeurs ou des ranges de valeurs dans le texte
d'un rapport.

Il suffit de placer la r�f�rence aux cellules n�cessaires entre accollades
dans une ligne de rapport :

&CO
    $DataUpdateVar {=Sheet1!R2C1} %YEAR% {=Sheet1!R2C2:R3C10}
&TX

La premi�re r�f�rence ~c{=Sheet1!R2C1}~C est lue dans Excel (le fichier doit
�videmment �tre pr�alablement ouvert) et est replac�e dans la ligne de
commande. Dans l'exemple, il s'agira du nom de la s�rie � modifier.


La seconde r�f�rence ~c{=Sheet1!R2C2:R3C10}~C est �galement remplac�e par la
valeur dans Excel. S'il y a plusieurs cellules, celles-ci sont s�par�es par
des blancs.

Dans l'exemple, on aura par exemple :

&CO
    $DataUpdateVar PNB 1990 3.23 2.34 2.56 3.12 3.45
&TX

Apr�s remplacement, la commande est ex�cut�e comme n'importe quelle autre
commande de rapport.


&TI Commande de rapport $ExcelWrite
�����������������������������������

Cette fonction permet d'�crire du texte ou des valeurs quelconques dans une feuille Excel.

Remplace les cellules sp�cifi�es par la ou les valeurs donn�es. Plusieurs
colonnes sont s�par�es par ~c\t~C et plusieurs lignes par ~c\n~C.

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
    ���������������������������������������������������������������������������

..esc ~

&TI D�finition des samples
����������������������
    Correction : lorsque une p�riode est incompl�tement donn�e (~c2000Y~C au
    lieu de ~c2000Y1~C), le ~c1~C est fix� par d�faut (au lieu de ~c0~C auparavant).
    De m�me, ~c2000Q~C vaut ~c2000Q1~C.

&TI Lecture des tables en ASCII
���������������������������
    Les valeurs min et max peuvent contenir une valeur n�gative dans les
    fichiers ASCII de d�finition des tableaux.

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
���������������������������������������������
    Lors du calcul de tableaux ou de graphiques pour l'affichage ou
    l'impression, si un des fichiers source ([1], [2], ...) n'est pas
    d�fini, IODE ne s'arr�te plus � chaque calcul, mais affiche un message
    dans le bas de l'�cran et donne des ~c--~C comme r�sultat pour les colonnes
    correspondantes.

&TI Impressions multiples
���������������������
    Une erreur dans la version pr�c�dente ne permettait plus d'imprimer
    plusieurs fois au cours de la m�me session de IODE. Cette erreur est corrig�e.

&TI Taux de croissances moyens
��������������������������
    Les samples g�n�ralis�s contenant ~c//~C (taux de croissance moyens) sont �
    nouveau accept�s.


&TI Graphiques
����������
    Corrections dans les graphiques : Minor et Major grids adapt�s
    (uniquement pour l'impression).
>

<Version 6.06> (02/02/2004)
    Version 6.06 (02/02/2004)
    ���������������������������������������������������������������������������

..esc ~

&TI Simulation
����������
    Lorsque la m�thode de r�ordonnancement (Sort algorithm) est fix�e �
    "None" et qu'une liste d'�quations non vide est fournie, l'ordre de la
    liste est conserv� pour la simulation.

    Cela implique en g�n�ral un nombre sup�rieur d'it�rations mais �vite le
    temps du r�ordonnancement.

&TI Graphiques
����������
    Le fichier iode.ini qui contient les donn�es d'affichage des graphiques
    est plac� dans le r�pertoire temporaire de Windows. Ce fichier est donc
    accessible en �criture ce qui �vite, lorque les droits de l'utilisateur
    sont limit�s, que la fen�tre graphique refuse de se fermer.

&TI Report line
����������
    Le fichier ws.rep qui contient la ligne de rapport � ex�cuter (fonction
    "Report/Execute a report line") est plac� dans le r�pertoire temporaire
    de Windows. Ce fichier est donc accessible en �criture ce qui permet,
    m�me si les droits de l'utilisateur sont limit�s, de cr�er le fichier.

    Cette fonction est notamment n�cessaire dans le cadre de l'utilisation
    de IODE comme objet COM.
>
<Version 6.05> (10/10/2003)
    Version 6.05 (10/10/2003)
    ���������������������������������������������������������������������������

..esc ~

&TI Commande rapport de gestion de directory et de fichiers
�������������������������������

    Nouvelles commandes ajout�es aux rapports :

&EN $chdir dirname : change de directory courant
&EN $mkdir dirname : cr�e un directory
&EN $rmdir dirname : d�truit un directory
&EN $SysAppendFile in out : ajoute le fichier in au fichier out
&EN $A2mToPrinter file.a2m : imprime le fichier file.a2m

&TI Formattage d'entiers
������������������������
    Nouvelle fonction ~c@fmt(val,fmt)~C qui retourne un string formatt�.

    Formatte un entier val suivant un format fmt donn�. Le r�sultat est un
    string transform� de longueur �gale � celle de fmt.

    Les caract�res reconnus dans le format sont : 'X', 'x', '9', '0'. Ils
    signifient qu'aux seules positions de ces caract�res seront plac�s dans
    leur ordre d'apparition les caract�res r�sultant du formattage de val.
    Seul cas particulier : le caract�re '0' qui sera remplac� par une '0' si
    le caract�re correspondant dans le formattage de val est ' '.

&SY
    @fmt(val,fmt)
    o�
    val = valeur enti�re
    fmt = format
&TX

&EX
    @fmt(123,0009)   --->> 0123
    @fmt(123,A0000A)   --->> A00123A
&TX
    Attention, les blancs avant et apr�s le format sont repris dans le r�sultat.

&TI Underflow et Overflow
�������������������������
Les calculs non tol�r�s (exponentielles en particulier) sont prises en
compte plus correctement notamment dans les fonctions d'estimation.

Auparavant, un message du type "Underflow error" �tait affich� et Iode se
plantait...
>
<Version 6.04> (18/01/2002)
    Version 6.04 (18/01/2002)
    ���������������������������������������������������������������������������

..esc ~

&TI Longueur des lignes de rapports
�������������������������������
    La longueur max est port�e � 10000 caract�res.

&TI Output HTML
�����������
    Cadrage des cellules de titres � droite (au lieu de centr�).

&TI Output HELPHTML
���������������
    Idem HTML et correction des titres de la table des mati�res.

&TI Output MIF
����������
    Gestion des sauts de pages dans les rapports (.page).

&TI LEC
�������
    Correction de la fonction exp(a, b). Cette fontion prenait toujours
    l'exponentielle n�p�rienne. Elle permet maintenant de prendre la puissance
    de n'importe quelle formule.

    Notons que
&CO
    exp(a, b)  == a**b
&TX
    D'autre part, les erreurs de calculs r�sultant de mauvaises valeurs
    des arguments sont mieux prises en charge par IODE :

&CO
    exp(-1, 0.5)
&TX
    retourne NA (--).
>

<Version 6.03> (13/12/2001)
    Version 6.03 (13/12/2001)
    ���������������������������������������������������������������������������
&TI Simulation
����������
    Correction de la m�thode Newton dans le cas du goal-seeking et d'une
    �quation non r�solue par rapport � l'endog�ne.

&TI Graphiques
����������
    Am�lioration des routines graphiques.
>

<Version 6.02>
    Version 6.02 (16/11/2000)
    ���������������������������������������������������������������������������

..esc ~

&TI LEC
���
    Deux nouvelles fonctions dans le langage LEC :
&CO
	hp([[from,]to],expr) : filtre Hodrick-Prescott
	appdif(expr1, expr2) : compl�te la s�rie expr1 sur base de la s�rie
			       apparent�e expr2 en tenant compte des
			       diff�rences au lieu des taux de croissance
			       (comme dans app())
&TX
    La fonction app() est modifi�e :
&CO
	app(expr1, expr2) : dans le cas o� plusieurs observations sont
	    manquantes, le taux de croissance est r�parti de mani�re
	    �quilibr�e entre les points connus.
&TX
&TI Gestion M�moire
�������
    Augmentation de l'espace m�moire dans le cas de grands WS
    (variable interne K_CHUNCK=1024).
>

<Version 6.01>
    Version 6.01 (16/10/2000)
    �������������������������

..esc ~

&TI Rapports
��������
    Nouvelles @-functions:
&CO
	@cvalue(name, ...) : texte de commentaires
	@vvalue(name, ...) : valeurs de s�ries
	@sample(B|E|nil)   : sample courant (d�but, fin, les deux)
&TX
&TI Readme
������
    Affichage du readme sous forme HTML.

&TI Distribution
������������
    Distribution de IODE � partir du site ~l"http://www.plan.be/fr/soft/iode.htm"www.plan.be~L

&TI Graphiques
����������
	Nouvelle librairie graphique (TeeChart) plus �tendue pour
	l'affichage et l'impression des graphiques.
>
<Version 5.32>
    Version 5.32 (16/08/2000)
    �������������������������

&TI Low to High
����������
    Lorsqu'une s�rie contient des donn�es manquantes au milieu de la p�riode
    de d�finition, la transformation se fait pour chaque partie d�finie.
    Auparavant, seule la premi�re partie �tait trait�e.

&TI Estimation : m�thode stepwise
&CO
    $EqsStepWise from to eqname leccond {r2|fstat}
&TX

&TI Rapports
��������
    Nouvelles fonctions de rapports pour l'�change avec Excel:

&CO
    $ExcelSetScl name cell : fixe une valeur dans Excel � partir d'un scalaire IODE
    $ExcelGetScl name cell : r�cup�re une valeur de scalaire � partir d'Excel
    $ExcelGetCmt name cell : r�cup�re une valeur de commentaire � partir d'Excel
&TX

&TI Nouveaux formats et limites de IODE

Cette version supprime la plupart des limites inh�rentes au logiciel IODE.
Elle introduit en plus un algorithme de compression pour diminuer la taille
requise par les WS sur disque.

&IT Compatibilit� avec les versions ant�rieures
�������������������������������������������
    Les anciens fichiers sont interpr�t�s automatiquement lors du chargement.
    Les nouveaux ne sont plus lisibles dans les anciennes versions de IODE.

&IT Nombre d'objets
���������������
    Le nombre maximum d'objets g�rables simultan�ment en WS passe de 65535 �
    2,147,483,647 (si le hardware le permet).

&IT Nom des objets
��������������
    Le nom des objets peut contenir 20 caract�res maximum au lieu de 10.

&IT Taille des objets
�����������������
    La taille des objets est limit�e � 2,147,483,647 bytes au lieu de 32,500.

&IT Nombre de colonnes des tableaux
�������������������������������
    Le nombre de colonnes d'un tableau est limit� � 99 (au lieu de 20 auparavant).


&IT Compression des fichiers
������������������������
    Les diff�rents WS peuvent �tre comprim�s au moment de leur sauvetage.
    Selon le type d'objet et leur d�finition, la compression peut aller de
    30 � 75%.

    Chaque objet �tant comprim� individuellement, le co�t au niveau du temps
    de sauvetage peut �tre important pour les fichiers contenant de nombreux
    objets ou sur des machines lentes (1000 variables de 25 obs. � la sec.
    sur un Pentium III 450 MHz).

    Iode reconna�t et transforme automatiquement les fichiers comprim�s.

    Le chargement n'est pratiquement pas p�nalis� par la proc�dure de
    d�compression. Un seul cas fait exception : les fichiers de variables
    dont les s�ries doivent �tre charg�es par une fonction de type
    $WsCopyVar: dans ce cas, la lecture est ralentie par le fait que les
    longueurs des s�ries comprim�es deviennent variables et que le fichier
    doit par cons�quent �tre lu s�quentiellement.

    Le panneau de sauvetage pr�sente un checkbox qui permet d'indiquer si on
    souhaite ou non comprimer les fichiers.

&TI Rapports
    ��������
    La fonction $WsSaveCmpXxx permet de sauver les WS en les comprimant.

    Nouvelle fonction ~c@srelax(scl1,scl2,...)~C qui retourne la valeur
    du relax d'un scalaire.

&TI DDE
���
    Correction dans les transferts entre IODE et APL. Les valeurs
    inf�rieures � 1e-10 plantaient parfois IODE.

&TI LEC
���
    Correction pour les �quations du type log(X) :=
>
<Version 5.31>
    Version 5.31 (11/05/2000)
    �������������������������

&TI IMPORT GEM
����������
    La fonction ~cFile/Import~C lit le nouveau format des fichiers GEM (projet
    Nemesis/E3ME).

    La fonction de rapport ~c$FileImportVar~C est �galement impl�ment�e pour ce
    nouveau format:
&CO
	$FileImportVar GEM rulefile infile outfile from to  [trace]
&TX

&TI Formattage de p�riodes dans les rapports
����������������������������������������
    L'expression {1980Y1 - 1@@T} dans les rapports ne donne le r�sultat
    correct - � savoir 1979Y1 - que si le sample courant contient 1979Y1. Si
    le sample �tait par exemple 1980Y1 2000Y1, le r�sultat �tait 1981Y1.
    Cette erreur est corrig�e.
>
<Version 5.30>
    Version 5.30 (13/04/2000)
    �������������������������
&TI Simulation
����������
    Le processus de simulation ne s'arr�te plus si une valeur NAN est atteinte
    dans le prologue et l'�pilogue du mod�le (les parties non interd�pendantes).
>
<Version 5.29>
    Version 5.29 (17/03/2000)
    �������������������������

&TI Rapports
��������

    Une ligne de rapport est interpr�t�e avant d'�tre (�ventuellement)
    ex�cut�e. Cette interpr�tation se fait de la fa�on suivante:

    L'ordre d'interpr�tation est le suivant :

    Les caract�res sp�ciaux sont : ~b%, { et @.~B

    Chaque ligne est interpr�t�e de gauche � droite. D�s qu'un des
    caract�res sp�ciaux est rencontr�, un traitement particulier est
    appliqu�.

Si on rencontre % :
&EN si le suivant est %, un seul % est conserv� comme du texte
&CO
    Exemple : augmentation de 10%% du PNB ->> un seul % reste dans le texte
&TX
&EN sinon, la macro est remplac�e par sa valeur ou vide si la macro n'existe pas :
&CO
	    Exemple : la variable %VAR% ->> la variable XYZ
&TX

Si on rencontre { :
&EN si le suivant est {, un seul { est conserv� comme du texte, le texte est
    lu jusqu'� }, les macros sont remplac�es

&EN2 si le texte r�sultat commence par $ ou ##, il s'agit d'une commande
	  de rapport qui est ex�cut�e et le r�sultat (0 ou 1) se retrouve
	  dans le texte.
&EN2 sinon, le texte r�sultat est calcul� comme une formule LEC � la
	  p�riode courante d�finie par $SetTime. Si la formule se termine
	  pas @T ou @999.999, le r�sultat est format� en cons�quence.

Si on rencontre @ :

&EN2 si le suivant est @, un seul @ est conserv� comme du texte,
	le texte est lu jusqu'� la parenth�se fermante,
	la fonction correspondante est ex�cut�e. A noter qu'en
	l'absence de parenth�ses, le texte reste inchang� (Ex.: gb@plan.be
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
����������������������������
    Le format dans les formules LEC des rapports peut s'exprimer comme .9, auquel
    cas, le nombre de d�cimales est fix� et le nombre de positions avant la virgule
    est d�termin� par la valeur du nombre lui-m�me.

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
�������������������������
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
���������������������������������������������������������������������������
&TI High to Low
�����������
    Correction dans le cas des donn�es non disponibles.
>
<Version 5.26>
    Version 5.26 (7/2/2000)
���������������������������������������������������������������������������
..esc ~

&TI Rapports
��������
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
    ��������
&CO
    $FileImportCmt TXT bstrule.txt bf-06ser.txt bh\p6d.cmt D p6d.log
    $FileImportVar TXT bstrule.txt bf-06obs.txt bh\p6y.var 1980Y1 2000Y1 p6y.log
&TX
>

<Version 5.25> (1/2/2000)
    Version 5.25 (1/2/2000)

..esc ~

&TI Load WS
�������
    En cas de probl�me lors de la lecture d'un fichier (fichier incomplet
    par exemple), IODE s'efforce de lire le maximum d'objets.
    Cependant,si un fichier pose probl�me, un message d'erreur est g�n�r� et
    il y a lieu de sauver son travail au plus vite. De plus, il est
    conseill� de sauver les donn�es du fichier � probl�me dans un format
    Ascii et de le recharger par la suite.

&TI Low to High
�����������
    La fonction Low To High s'est enrichie d'une nouvelle m�thode
    d'interpolation en plus des 2 existantes. Cette m�thode (Step) conserve
    une valeur constante (celle de l'observation de la s�rie de base,
    �ventuellement divis�e par le nombre de sous-p�riodes) pour toutes les
    observations calcul�es).

    D'autre part, la syntaxe des fonctions de rapport correspondantes est :
&CO
	$WsLToHFlow  {L|C|S} file var1 var2 ...
	$WsLToHStock {L|C|S} file var1 var2 ...
    avec L pour interpolation lin�aire
	 C pour interpolation par Cubic Splines
	 S pour interpolation par Steps
&TX

&TI Aide
����
    Le fichier d'aide en format HtmlHelp (plus convivial, plus lisible) est
    int�gr� dans IODE. Cependant, si le syst�me HtmlHelp n'est pas install�
    sur la machine cible, l'ancien fichier d'aide est utilis�.
>

<Version 5.24> (20/1/2000)
    Version 5.24 (20/1/2000)

..esc ~
&TI Corrections

&EN    Interface ~bDataStream~B : corrections pour les ann�es � partir de 2000

&EN     Interface ~bODBC~B : adaptation aux champs de grande taille (longueur max : 4K)
>
<Version 5.23>
Version 5.23 (13/12/1999)


&TI LEC
���
    La fonction ~capp()~C de calcul de s�rie apparent�es est modifi�e dans
    le cas d'une r�tropolation ou d'une extrapolation sur base d'une
    s�rie apparent�e. Le taux de croissance est utilis� pour les valeurs
    en dehors de la p�riode de d�finition de la s�rie � compl�ter.
>
<Version 5.22>
    Version 5.22 (19/09/99)

&TI Importations
������������
    Le format d'�change Belgostat version 2000 est disponible dans le
    panneau d'importation. Les commentaires et s�ries peuvent �tre
    import�s.

    Ce format remplace dor�navant le format DIF-Belgostat. Les fichiers
    de donn�es ne sont plus disponibles via l'ancienne connexion
    Belgostat, mais sous forme de fichiers complets sur le site Internet
    de la BNB (voir GB pour infos).

&TI Rapports
��������
    Nouvelles fonctions :
&CO
	@sstderr(scalar, ...)
	@equal(str1, str2)
&TX
    Exemple
&CO
	$show Valeur de a1 : @sstderr(a1)
	A est le premier param�tre ? @equal(A,%1%)
&TX
    R�sultat
&CO
	Valeur de a1 : 2.23
	A est le premier param�tre ? 0
&TX

    Modification support YoY pour Diff et Grt :
&CO
	$DataUpdateVar varname [{L | D | G | DY | GY}] period values
&TX
    Excel : Correction dans ~c$ExcelSetVar~C : la derni�re observation �tait
    manquante.

&TI Lectures tableaux
�����������������
    Lors de la lecture d'un tableau en ASCII (fichier .at), si une
    forme LEC est erron�e ou vide, la cellule correspondante contient
    la forme LEC sous forme d'un string. Il est alors possible de
    la corriger dans l'�diteur de tableau (si cela est souhait�).
>

<Version 5.21>
    Version 5.21 (10/08/99)

..esc ~

&TI Interface ODBC

    De nouvelles fonctions de rapport permettent d'ex�cuter des requ�tes
    SQL. Ces fonctions permettent par exemple de construire automatiquement
    des s�ries au format IODE � partir de bases de donn�es Access, Oracle
    ou m�me de fichiers de texte.

    Les commandes de rapport se pr�sentent sous forme de fonctions:
&CO
	@SqlOpen(ODBC_source[,user,passwd])
	@SqlQuery("SQL Query")
	@SqlNext()
	@SqlField(field_nb)
	@SqlRecord([field_nb[,field_nb[,...]]])
	@SqlClose()
&TX
    Pour pouvoir acc�der � une base de donn�es dans IODE, il faut
    pr�alablement d�finir une 'source ODBC' (Open DataBase Connectivity),
    ce qui peut �tre fait tr�s facilement � l'aide du programme ODBC32
    inclus dans le Control Panel de Windows.

    La syntaxe compl�te et des exemples peuvent �tre trouv�s dans le manuel.

&TI Rapports

    Les titres et pieds de pages introduits par les commandes
    ~c$PrintPageHeader~C et ~c$PrintPageFooter~C sont conserv�s si le r�sultat
    de l'impression est sauvegard� dans un fichier a2m.
>

<Version 5.20>
    Version 5.20 (21/06/99)
    �����������������������
..esc ~
&TI Installation
����������������
    Les fichiers n�cessaires � l'utilisation de la version HTML Help de l'aide
    de IODE sont int�gr�s dans le programme d'installation de IODE.
    Ces fichiers ne doivent �tre install�s que pour les versions Windows
    95 et NT 4.0. Ils font partie de Windows 98.

&TI Programme A2M
�����������������
    Le programme A2M qui permet, � l'ext�rieur de IODE, d'imprimer ou de
    transformer des fichiers A2M, est corrig�.

&TI Rapports
��������
&EN L'impression des tableaux faisant appel � plusieurs fichiers de
      variables n�cessite un appel � la fonction ~c$PrintTblFile~C avant
      chaque appel � ~c$PrintTbl.~C Idem pour les graphiques. Ce n'est pas
      nouveau, mais cela peut surprendre!

&EN Nouvelle fonction ~c$RepeatString~C permettant de choisir le string �
      remplacer dans la commande r�p�t�e par ~c$Repeat.~C
&CO
      $RepeatString ++
      $Repeat "$DataDuplicateVar ++ ++_1" A B C
      copie la s�rie A sous A_1, B sous B_1, etc.
&TX

&TI Impression de rapports
����������������������
    Les num�ros de lignes sont imprim�s par la fonction d'impression des
    rapports. Par ailleurs, cette fonction imprime � nouveau l'enti�ret�
    des rapports.

&TI Simulation
����������
    L'algorithme de Newton-Raphson a �t� l�g�rement am�lior�.
>

<Version 5.19>
    Version 5.19 (31/05/99)
���������������������������������������������������������������������������
..esc ~
&TI Rapports
��������
    Les lignes des rapports peuvent dor�navant atteindre une longueur
    maximale de 4095 caract�res. Cependant, si la ligne contient une
    forme LEC, il est possible que l'espace n�cessaire � la forme
    compil�e de cette derni�re d�passe la limite du LEC.
>
<Version 5.18>
    Version 5.18 (30/04/99)
���������������������������������������������������������������������������
&TI Rapports
��������

&IT Transformation de strings

    Nouvelles ~c@fonctions~C n�cessaires pour certains rapports.

&EN @month(mois, langue)
&EN @ansi(mois, langue)

Exemples :
&CO
    @ansi(@month(2,F)) : F�vrier
    @ansi(@month(2,N)) : Februari
    @ansi(@month(2,E)) : February
&TX

&IT G�n�ration d'un HtmlHelp
&CO
    $PrintHtmlHelp [YES|No] : permet de g�n�rer des fichiers HTML Help.
&TX

&IT Fonction de bouclage
&CO
    $Repeat "command" args :
&TX
Cette fonction permet d'ex�cuter une commande sur une liste d'arguments
sans devoir cr�er un rapport � cette fin. La position de l'argument dans
la liste est repr�sent�e par le caract�re de soulignement.

Exemples :

&CO
	1. Tri de plusieurs listes

	    $Repeat "$DataListSort _ _" A B C
		�quivaut �

	    $DataListSort A A
	    $DataListSort B B
	    $DataListSort C C

	2. Duplicate Vars

	    Avec les nouvelles fonctions @fn(), on peut copier toutes les
	    variables d'un WS en une seule op�ration :

	    $Repeat "$DataDuplicateVar _ _1" @vexpand(*)
&TX
>
<Version 5.17>
    Version 5.17 (12/03/99)

..esc ~

&TI Seasonal adjustment
�������������������
    Nouveau param�tre dans le panneau de d�marrage qui permet de lib�rer
    le crit�re v�rifiant si une influence saisonni�re est pr�sente dans
    une s�rie.

    Le m�me param�tre peut �tre pass� comme argument suppl�mentaire � la fonction
    de rapport $WsSeasAdj :
&CO
	$WsSeasAdj Filename Varname Varname ... Eps
&TX

&TI LEC
���
    Nouveaux op�rateurs :

&EN ~clastobs(from,~C ~cto,~C ~cexpr)~C : calcule la derni�re observation sur
	    une p�riode donn�e

&EN ~cinterpol(expr)~C : fournit une valeur � expr en t en interpolant ou
	    extrapolant les valeurs connues (actuellement de fa�on lin�aire).

&EN ~capp(expr1,~C ~cexpr2)~C : fournit une valeur � expr1 en t en utilisant
	    la s�rie expr2 comme s�rie apparent�e

&EN ~crandom(expr)~C : fournit un nombre al�atoire compris entre -expr/2 et +expr/2.

    Modifications:

	Les op�rateurs ~clmean~C et ~clstderr~C ne retournent plus de valeur ~cNA~C
	lorsqu'une des observations est ~cNA:~C ils n'utilisent simplement plus
	les valeurs indisponibles dans les calculs.

&TI Rapports
��������
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
����������������������
    Lorsqu'une variable n'est pas d�finie, les tableaux s'affichent ou
    s'impriment avec -- comme valeur pour les cellules concern�es.
>
<Version 5.16>
    Version 5.16 (24/02/99)

&TI LEC
���
    Nouvel op�rateur :

&EN ~clstderr(expr,~C ~cexpr,~C ~c...)~C : calcule la d�viation standard
	    des expressions pass�es comme param�tre

&TI Rapports
��������
    Nouveau groupe de fonctions dont la syntaxe g�n�rale est :
&CO
	@function_name(arg1, arg2, ...)
&TX
    Ce groupe de fonctions va progressivement �tre �tendu selon les demandes
    des utilisateurs. Actuellement, les fonctions d�finies sont :
&CO
	@upper(texte) : mise d'un texte en majuscules
	@lower(texte) : mise d'un texte en minuscules
	@date([format]) : date
	@time([format[]) : heure
	@replace(string, from, to) : substitution d'un texte par une autre

	@fdelete(filename) : d�truit le fichier filename
	@fappend(filename,string|NL, ...) : �crit dans un fichier les textes
				pass�s comme argument
&TX
    Sont en projet (les id�es sont bienvenues) :
&CO
	@ttitle(tablename,tablename, ...) : titre des tableaux
	@tgetnl(tablename) : nombre de lignes d'un tableau
	@tgetnc(tablename) : nombre de colonnes d'un tableau
	@tgetlinetype(tablename,line) : type de ligne
	@tgetcelltype(tablename,line) : type de cellule
	@tgetcell(tablename,line,col) : contenu de la cellule [line, col]

	@cvalue(name, ...) : texte du commentaire
	@evalue(name, ...) : forme LEC de l'�quation
	@ivalue(name, ...) : forme LEC de l'identit�
	@lvalue(name, ...) : d�finition de la liste
	@svalue(name, ...) : valeur du scalaire
	@tvalue(name, ...) :
	@vvalue(name, ...) : vecteur formatt� des donn�es

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

    Ces fonctions s'ex�cutent dans le cadre d'une ligne de rapport et le
    r�sultat de la fonction est imprim� dans l'output du rapport.

    Par exemple :
&CO
	Le tableau T1 a pour titre @ttitle(T1). Ce titre en majuscules
	est par cons�quent @upper(@ttitle(T1)). Ce texte est imprim�
	le @date() � @time().
&TX
    Pour placer le titre du tableau T1 dans le fichier toto.txt :
&CO
	@fappend(toto.txt, @ttitle(T1))
&TX
>
<Version 5.15>
    Version 5.15 (25/01/99)

&TI Rapports
��������
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
����
    La fonction de copy d'objet (WorkSpace/Copy Into Workspace) et
    la fonction de rapport �quivalente sont corrig�es.

&TI Euro
����
    La valeur de conversion euro/FB est fix�e � ~c40.3399~C dans le LEC.

    Exemple : si on a une s�rie ~cA~C en FB, a forme ~c(A/euro)~C fournit
    la valeur en euro de cette s�rie. Rappelons qu'un diviseur de colonne
    peut �tre introduit dans les tableaux, ce qui permet de transformer
    tr�s facilement les tableaux.

&TI Y2K
���
    Les valeurs des samples g�n�ralis�s (pour les impressions) et des
    p�riodes dans le LEC sont adapt�es pour permettre l'�criture des
    ann�es au-del� de l'an 2000 en deux chiffres.

    Ainsi les ann�es inf�rieures � 50 sont consid�r�es comme 20xx et celles
    sup�rieures � 50 comme 19xx. Par exemple :
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
���
    Nouvel op�rateur ~ceuro~C dont la valeur sera correctement fix�e le 1/1/99.

&TI Impression de variables
�����������������������
    Le caract�re espace ne peut plus �tre utilis� comme s�parateur de liste
    dans l'�cran PRINT/VIEW Variables, afin de permettre d'y placer
    des formules LEC comme d A.

&TI Rapports
��������
    ~c$DataDelete~C accepte les wildcards comme argument. Enfin une fa�on
    rapide de perdre tout votre travail sans devoir couper le PC.
>

<Version 5.12>
    Version 5.12 (17/12/98)

..esc ~

&TI 'Unit Root' tests
�����������������
    Les tests de Dickey-Fuller sont disponibles au niveau des rapports
    et du panneau d'estimation d'�quations.

    Les tests sont sauvegard�s dans des scalaires dont le nom est
    compos� du pr�fixe df_ et du nom de la premi�re s�rie apparaissant
    dans la formule � tester. Par exemple, le test pour la formule
    ~cd(A0GR+A0GF)~C est ~cdf_a0gr.~C

&IT    Dans les rapports
    �����������������
&CO
	$StatUnitRoot drift trend order expression
&TX
    o�
&CO
	drift : 0 ou 1 selon que la formule � estimer doive incorporer
		un terme constant (1) ou non (0)
	trend : 0 ou 1 selon que la formule � estimer doive incorporer
		un terme de trend (1) ou non (0)
	order : l'ordre du polynome � estimer pour obtenir les tests
	expression : forme LEC � tester
&TX
    Par exemple :
&CO
	$StatUnitRoot 1 1 3 A
&TX
    L'�quation estim�e est :
&CO
	d(A) := df_ * A[-1]+
		    df_d +    /* DRIFT */
		    df_t * t +  /* TREND */
		    df1 * d(A[-1]) + df2*d(A[-2]) + df3*d(A[-3]) /* ORDER */
&TX
    Seuls le test de Dickey-Fuller est sauvegard� dans un scalaire sous
    le nom ~cdf_a~C dans le cas de l'exemple.

&IT Dans le panneau d'estimation

    La touche F3 ou le bouton "Unit Root" permettent de sp�cifier et de
    tester une ou plusieurs formules. Les r�sultats sont affich�s dans
    la fen�tre.

    Le seul scalaire sauvegard� est celui correspondant � la derni�re
    expression test�e.

&TI Agr�gation de s�ries
���������������������
    De nouvelles fonctions de rapports permettent d'effectuer
    des agr�gations, des produits ou des sommes de s�ries.
    Les s�ries � traiter peuvent se trouver en WS ou dans un
    fichier externe.

    Quatre nouvelles fonctions de rapport ont �t� d�finies � cet effet:
&CO
    $WsAggrChar [char] : d�finit le caract�re � introduire dans le
			 code des s�ries cr��es
    $WsAggrSum  pattern [filename] : somme des s�ries d�finies par pattern
    $WsAggrMean pattern [filename] : moyenne des s�ries
    $WsAggrProd pattern [filename] : produit des s�ries
&TX
    Si filename est d�fini, les s�ries du fichier seront utilis�es �
    la place de celles du WS.

    ~cpattern~C est d�fini comme une s�quence de parties de noms plac�es
    entre crochets ou parenth�ses. Chaque partie peut contenir des
    caract�res alphanum�riques ou un point d'interrogation.

    Les parties de noms entre parenth�ses ne sont pas agr�g�es. Celles
    entre crochets le sont.

&IT    Exemple
    �������
	Soit un WS avec des s�ries par pays (BE, FR, GE), et par secteur
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

    On peut cr�er la somme de tous les secteurs pour chaque pays par
    les commandes :
&CO
	$WsAggrChar _
	$WsAggrSum (??)[????]
&TX
    Les s�ries cr��es seront :
&CO
	    BE____, FR____, ..., GB____
&TX
    Les points d'interrogations entre () permettent de pr�ciser les
    codes de s�ries � traiter. Les autres indiquent les parties �
    agr�ger. Dans ce cas les points d'interrogation sont
    remplac�s par des _ (ou un autre caract�re selon l'argument
    de ~c$WsAggrChar~C) dans les s�ries r�sultats. Ce caract�re
    peut �tre blanc. Dans l'exemple, les s�ries cr��es sont alors BE, FR
    et GB.

    On peut �galement cr�er la somme de tous les pays par secteur ou
    la somme de tous les pays pour certains secteurs :
&CO
	$WsAggrSum [??](????) : cr�e __S101, __S102, ...
	$WsAggrSum [??](??)[??] : cr�e __S1__, __S1__, ...
&TX
    On peut limiter la cr�ation � un seul ou � quelques codes :
&CO
	$WsAggrSum (BE)[S??9] : cr�e BES__9
	$WsAggrSum (BES)[?](?9) : cr�e BES_09, BES_19, BES_29, ... BES_99
&TX
&IT    Caract�re de regroupement
    �������������������������
    La commande ~c$WsAggrChar~C permet de sp�cifier le caract�re �
    placer dans les s�ries g�n�r�es. Ce caract�re peut �tre blanc.
    Pour �viter que des s�ries d�j� agr�g�es soient reprises dans une
    agr�gation ult�rieure, ces s�ries ne sont pas retenues dans le
    calcul si le caract�re d'agr�gat courant se trouve � une position
    correspondant dans pattern � un point d'interrogation entre
    crochets. Ainsi, la s�rie ~cBE____~C ne sera pas reprise dans le
    calcul ~c(BE)[????]~C. Par contre elle sera reprise dans le calcul
    ~c[??](????)~C, car dans ce dernier cas elle n'intervient pas dans
    la somme.

    Supposons que les s�ries ~cBE____~C, ~cFR____~C et ~cGB____~C soient
    d�finies ainsi que ~cBES101~C, ...

    ~c$WsAggrSum~C ~c(??)[????]~C g�n�re ~cBE____~C, ~cFR____~C, etc.
    Elle n'utilise donc pas les s�ries contenant ~c_~C apr�s la deuxi�me
    position, comme ~cBE____~C. En effet, si on les reprenaient, on
    additionnerait deux fois les m�mes s�ries.

    ~c$WsAggrSum~C ~c[??](????)~C g�n�re ~c______~C en prenant la somme
    de ~cBE____,~C ~cFR____,~C ~cGB____,~C ce qui est correct car les
    autres s�ries (comme ~cBES101~C) donnent lieu � d'autres s�ries
    (~c__S101~C).
>
<Version 5.11>
    Version 5.11 (03/11/98)

&TI Support des longs noms de fichiers
����������������������������������
    IODE supporte les noms de plus de 15 caract�res dans la limite
    des champs des �crans de saisie. La s�lection de fichiers
    dans des r�pertoires comme ~c\My~C ~cDocuments~C est �galement possible
    pour des fichiers � longs noms.

&TI Correction Simulation
���������������������
    Une petite correction a �t� apport�e au programme de simulation
    dans le cas de valeurs d'endog�nes tr�s petites (en g�n�ral
    dans le cas de r�cup�ration de donn�es de la version simple
    pr�cision).
>

<Version 5.10> (14/10/98)
    Version 5.10 (14/10/98)
    �����������������������
..esc ~

&TI Rapports
��������

    ~cWsCopyVar~C : correction dans le cas o� la copie ne concerne
    qu'une seule ann�e.

&CO
     $WsCopyVar 1970Y1 1970Y1 A* B*
&TX

&TI Estimation
����������
    Les graphiques ne pouvaient �tre affich�s ou imprim�s dans le cas
    o� le s�parateur entre le membre de gauche et le membre de droite
    de l'�quation (:=) se trouvait � la ligne. Ce bug est corrig�.

&TI Langage LEC
�����������
    Nouvel op�rateur LEC qui retourne le signe d'une expression:
&CO
    sign(expr) vaut
		    1 si expr >>= 0
		    -1 si expr << 0
&TX

&TI Marquer/Copier/Coller
���������������������
    Les op�rations de "copier/coller" peuvent �tre effectu�es entre
    IODE et d'autres programmes Windows (Excel, ...) ou entre diff�rents
    �crans de IODE. Le "Clipboard" de Windows est utilis� pour stocker
    les donn�es copi�es.

    Les touches de fonction classiques de Windows ont �t� impl�ment�es
    pour faciliter cet usage : Shift+curseur permet de s�lectionner
    ou d'�tendre la s�lection et la souris permet de marquer
    des parties de texte. Pour placer du texte dans le Clipboard
    et copier ce qui s'y trouve � la position du curseur, on
    utilisera respectivement les touches Ctrl+C et Ctrl+V.

    Cette nouvelle facilit� est tr�s pratique dans de nombreux
    cas :

&EN  recopier un nom de fichier � charger dans le panneau Load WS
&EN  copier des parties d'�quations dans d'autres �quations
&EN  copier des valeurs de s�ries vers Excel, ou de Excel vers IODE
&EN  copier des s�ries dans un fichier Ascii ou les r�cup�rer
	  d'un fichier Ascii
&EN  recopier les valeurs d'une ann�e pour plusieurs s�ries dans
	  une autre ann�e
&EN  copier entre deux instances de IODE qui tournent en m�me temps
&EN  etc

&IT     1. En �dition d'un �cran
    ������������������������
      Cette partie s'applique par exemple � l'�cran Load WorkSpace.

      MARQUER
      �������
&EN  entrer dans un champ : marque tout le champ
&EN  Ctrl+L : marque tout le champ courant
&EN  Shift+Left : prolonge la marque d'un caract�re vers la gauche
&EN  Shift+Right : prolonge la marque d'un caract�re vers la droite
&EN  Shift+Home : prolonge la marque jusqu'au d�but du champ
&EN  Shift+End : prolonge la marque jusqu'� la fin du champ
&EN  cliquer avec la souris et garder le bouton de gauche enfonc� :
	  d�finit ou prolonge la partie marqu�e

      DEMARQUER
      ���������
&EN  tout d�placement du curseur supprime les marques (LEFT, RIGHT,
	    HOME, END)
&EN  tout caract�re entr� supprime (et d�truit) les marques
&EN  DEL supprime les marques

      DETRUIRE
      ��������
&EN  n'importe quel caract�re remplace la partie marqu�e
&EN  DEL : d�truit la partie marqu�e

      COPIER
      ������
&EN  Ctrl+C : copie ce qui est marqu� dans le clipboard

      COLLER
      ������
&EN  Ctrl+V : copie � partir de la position courante
		   les donn�es du clipboard (qui peuvent provenir
		   d'un autre programme) et remplace la marque
		   courante

&IT    2. En �dition des variables
    ���������������������������

      MARQUER
      �������
&EN  Ctrl+L : marque la cellule courante ou �tend la marque
&EN  Shift+Left, Shift+Right, Shift+Home, Shift+End, Shift+PgDn,
	  Shift+PgUp : marque ou �tend la marque

      DEMARQUER
      ���������
&EN  Ctrl+U : supprime les marques

      COPIER
      ������
&EN  Ctrl+C : copie ce qui est marqu� dans le clipboard

      COLLER
      ������
&EN  Ctrl+V : copie � partir de la position courante
		   les donn�es du clipboard

      Ces fonctions permettent par exemple de copier des colonnes ou
      des lignes de donn�es entre Excel et IODE, ou encore de
      copier une ann�e vers une autre pour un ensemble de variables.

&IT    3. En �dition des champs EDITOR (rapports, �quations, listes, etc)
    ������������������������������������������������������������������
      Cette partie s'applique par exemple � l'�diteur de rapports,
      � la saisie d'une �quation, � la d�fition d'un sample d'impression.

      MARQUER
      �������
&EN  Ctrl+L : marque la ligne courante ou prolonge jusqu'�
		   la ligne courante (inchang�)
&EN  Ctrl+T : marque le caract�re courant ou prolonge la marque
		   (inchang�)
&EN  Alt+B  : marque la colonne courante ou prolonge la marque
		   (inchang�)
&EN  Shift+Left : prolonge la marque d'un caract�re vers la gauche
&EN  Shift+Right : prolonge la marque d'un caract�re vers la droite
&EN  Shift+PgUp : prolonge la marque d'une page vers le haut
&EN  Shift+PgDn : prolonge la marque d'une page vers le bas
&EN  Shift+Home : prolonge la marque jusqu'au d�but du champ
&EN  Shift+End : prolonge la marque jusqu'� la fin du champ
&EN  cliquer avec la souris et garder le bouton de gauche enfonc� :
	  d�finit ou prolonge la partie marqu�e (rectangle)

      DEMARQUER
      ���������
&EN  Ctrl+U : supprime les marques (ne d�truit pas)

      DETRUIRE
      ��������
&EN  Ctrl+D : d�truit ce qui est marqu� et supprime les marques

      COPIER
      ������
&EN  Ctrl+C : copie ce qui est marqu� dans le clipboard de Windows

      COLLER
      ������
&EN  Ctrl+V : copie � partir de la position courante
		   les donn�es du clipboard (qui peuvent provenir
		   d'un autre programme) et remplace la marque
		   courante
>
<Version 5.09> (05/10/98)
    Version 5.09 (05/10/98)
    �����������������������
&TI Rapports
��������
    La commande ~c$system~C fonctionne � nouveau dans la version
    Win95 de IODE. Cela permet notamment d'ex�cuter des fichiers
    de commandes ~c(.bat),~C y compris des programmes 16 bits.

    La commande est bloquante, c'est-�-dire que le programme attend
    la fin de l'ex�cution de la commande ~c$system~C avant de continuer
    le rapport.

&TI Utilitaire wait.exe
�������������������

    Pour permettre de synchroniser des programmes Windows successifs,
    l'utilitaire wait.exe est ajout� aux fichiers distribu�s.

&IT    Exemple d'utilisation
    ���������������������
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
    ~cl_ax23~C s'ex�cute sans attendre la fin de ~cvar2db.~C
>
<Version 5.08> (25/09/98)
    Version 5.08 (25/09/98)
    �����������������������
&TI LEC
���
    Nouveaux op�rateurs en LEC :

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
    �����������������������
&TI Format CSV invers�
������������������
    Il est possible d'exporter des s�ries en format CSV invers�,
    c'est-�-dire avec les s�ries en lignes et non en colonnes.
    Dans ce format, les commentaires ne sont pas export�s pour
    des raisons de largeur de colonnes insuffisantes.

&TI Rapports
��������
    Macro ~c%-nom%~C supprime dans la macro tous les caract�res non
    aplhanum�riques ou de soulignement.
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
    �����������������������

&TI Utilisation de wildcards
������������������������
    Afin de simplifier l'�criture, il est possible d'utiliser des
    "wildcards". Ces wildcards sont l'�toile (*) ou le point
    d'interrogation (?).

&EN * : indique une suite quelconque de caract�res, m�me vide
&EN ? : indique un caract�re unique

    On utilisera des noms contenant des �toiles ou des points
    d'interrogation dans certains �crans, dans les formes LEC et dans
    certaines fonctions de rapport. Ces noms sont remplac�s par
    des listes contenant les noms correspondants pr�sents en WS.

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

    Dans cette version de IODE, ce type d'expression peut �tre utilis�
    dans

&EN l'�cran ou la fonction $WsCopyXxx
&EN l'�cran ou la fonction de cr�ation de tableau $DataUpdateTbl
&EN l'�cran ou la fonction d'impression de variables $PrintVar
&EN les formes LEC (entre quotes)

    Dans le cas de la forme LEC, l'expression � �tendre doit �tre
    fournie entre quotes (') pour la distinguer de l'op�rateur *.

    Dans le cas des �crans ou des rapports, les parties � ne pas
    �tendre doivent �tre exprim�es entre double quotes (").

    Ainsi,
&CO
	A*;"B*A"        sera traduit en A1;A2;...;B*A

	A*;"lsum('A*')" sera traduit en A1;..;lsum('A*')
&TX
    Dans les rapports, les fonction suivantes sont concern�es par
    cette modification :
&CO
	$WsCopyXxx filename args
	$DataUpdateTbl
	$DataEditXxx
	$PrintVar
&TX

&TI Extensions LEC
��������������
    Lorsque l'op�rateur le permet (lsum, max et min), on peut
    utiliser les wildcards dans le langage LEC. Cependant, pour
    distinguer l'�toile et l'op�rateur fois, la liste � �tendre
    doit �tre entour�e de quotes simples('). Ainsi,

      ~clsum('A*')~C
    est �quivalent �
      ~clsum(A1,A2,AX)~C si A1, A2 et AX sont
     les seules s�ries commen�ant pas A dans le WS courant.

    On peut �galement utiliser une combinaison de noms de s�ries :

	~cmax('*G;*F')~C


    Le nombre maximum d'op�randes des op�rateurs ~clsum(),~C ~cmax()~C
    et ~cmin()~C est port� � 255, de fa�on � pouvoir exploiter au mieux
    les wildcards.

&IT    Remarque importante
    �������������������
    Les noms contenant des wildcards dans les formes LEC sont r�solus en
    fonction du contenu du WS COURANT. La forme LEC compil�e est
    m�moris�e avec ces noms. Si on change le contenu du WS, il est
    possible que certaines s�ries n'existent plus ou que de nouvelles
    apparaissent. La forme compil�e n'�tant pas chang�e automatiquement,
    il faut si on souhaite adapter la forme LEC au nouveau contenu,
    recompiler la forme en l'�ditant.

&TI Load WS
�������
    Correction d'une erreur (rare mais possible) dans le chargement
    de fichiers de scalaires.


&TI Rapports
��������
    Correction dans les messages d'erreurs (plus de pr�cision dans
    le texte).

    Les caract�res % et { peuvent �tre introduits comme texte
    normal en �tant doubl�s. Le texte %%1%% ne sera donc pas interpr�t�
    comme une macro, mais comme le texte %1%.

    Commentaires : une ligne de commentaire vide ne g�n�re plus de
    message d'erreur. Elle peut commencer par $ ou par #.

    Nouvelles fonctions:

&CO
	 $Debug {Yes|No} : affiche le num�ro de la ligne et le fichier
			   en cours d'ex�cution dans les rapports.

	 $SysMoveFile filein fileout : renomme un fichier

	 $SysCopyFile filein fileout : copie un fichier

	 $SysDeleteFile file1 file2 ... : d�truit des fichiers
&TX

&TI Edition des variables
���������������������
    Deux nouvelles options ont �t� ajout�es � la touche ~cF5~C lors de
    l'�dition des tableaux de variables. Ces options permettent
    d'afficher les valeurs en diff�rences et en taux de croissance
    sur une ann�e (Year On Year). On a donc maintenant dans l'ordre:

&EN level
&EN difference
&EN growth rate
&EN YoY difference
&EN YoY growth rate

    Pour faciliter la navigation, on peut utiliser ~cshift+F5~C pour
    revenir au mode pr�c�dent.

    Les graphiques s'affichent en fonction du mode utilis�. Ainsi, si
    l'affichage s'effectue en taux de croissance, la touche ~cF8~C fait
    appara�tre le graphique dans le m�me mode.

&TI HTML
����
    Le format HTML de sortie des impressions est enrichi et permet
    de placer automatiquement des graphiques dans les documents. Les
    fichiers graphiques portent le nom IMGn.GIF et se trouvent dans le
    r�pertoire de travail. Les graphiques sont automatiquement
    r�f�renc�s dans le fichier HTML r�sultat.

    Pour permettre de param�trer la g�n�ration des fichiers GIF,
    un nouveau panneau est int�gr� dans les "Printer Options". Les options
    qui y sont d�finies peuvent �galement �tre d�finies par des
    fonctions de rapport.

    Les fonctions de rapports sont :
&CO
	$PrintGIFBackColor color    : d�finit la couleur de fond des
				      graphiques
	$PrintGIFTransColor color   : d�finit la couleur consid�r�e comme
				      "transparente"
	$PrintGIFTransparent Yes|No : indique si le fichier GIF doit �tre au
				      format transparent
	$PrintGIFInterlaced Yes|No  : indique si le fichier GIF doit �tre au
				      format interlac�
	$PrintGIFFilled Yes|No      : indique s'il faut remplir les barres dans
				      les bar charts
	$PrintGIFFont num�ro        : indique si le num�ro du font � utiliser
				      pour les labels (voir �cran pour
				      les valeurs possibles).
&TX
>
<Version 5.05>
    Version 5.05 (31/07/1998)
    �������������������������

&TI Rapports
��������
    La macro ~c%*%~C repr�sente tous les arguments du rapport.
    Il est � noter que la commande ~c$shift~C supprime le premier argument
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

    Ex�cution du rapport
    --------------------
    $ReportExec Main.rep A B C D E

&TX
    Le rapport ~csub1.rep~C re�oit comme arguments tous
    les arguments pass�s � ~cmain.rep,~C soit ~cA B C D E~C.
    Le rapport ~csub2.rep~C re�oit les arguments ~cB C D E~C.


&TI Nouveaux op�rateurs LEC
�����������������������
    Les op�rateurs suivants ont �t� ajout�s au langage LEC :

&CO
	- lsum(expr1, expr2, ...) : calcule la somme d'une liste
	  de s�ries

	- index([[from,] to,] valeur, expr) : retourne la premi�re p�riode
	  o� valeur appara�t dans la s�rie expr entre les p�riodes
	  from et to. Si la valeur n'appara�t pas dans expr, -- est
	  retourn�.

	- acf([[from,] to,] valeur, expr) : retourne la fonction
	  d'auto-correlation de degr� valeur de l'expression expr
	  sur le sample from � to.

&TX

    Exemples
&CO
	Soient A et B d�finies sur 1970Y1-1990Y1
	A = 0 1 2 3 ...
	B = 2 4 6 8 ...

	lsum(A,B) vaut 2 5 8 11 ...

	index(6, B) vaut -- -- 2 2 2 2 ... (ce qui �quivaut en LEC
					    � -- -- 1972Y1 1972Y1 ...)
	index(1971Y1, 1990Y1, 6, B) vaut 2 2 2 2 ...
&TX

&TI Impressions
�����������
    Les impressions ne fonctionnaient plus correctement dans la version
    pr�c�dente pour certains drivers (qms, hp). Cette erreur est
    corrig�e.

    Les tableaux g�n�r�s en Frame (mif) contiennent le titre
    dans le corps du tableau et plus dans un paragraphe s�par�.

&TI Aanpassing van Census II methode
��������������������������������
    Om geen informatie te verliezen bij het berekenen van de laatste
    observaties, werd het algoritme aangepast. Bij onvolledige periodes
    worden, krijgen gegevens op het einde van de periode voorang.


&TI Schatten van vgln
�����������������
    Bij het on-line schatten van vgln worden nu ook de testen als scalars
    weggeschreven.


&TI IODE-XLS interface
���������������������������
    Lijsten kunnen worden gebruikt bij het opvragen van gegevens

>
<Version 5.04>
    Version 5.04 (01/04/1998)
    �������������������������

&TI ACCESS RESEAU ET NOMS DE FICHIERS
���������������������������������
    Des fichiers d'un autre ordinateur du r�seau peuvent �tre acc�d�s
    directement dans IODE sans avoir � monter un drive r�seau.

    Par exemple, on peut dans le panneau de LOAD WS entrer un nom
    comme :

&CO
	\\GB\iode\stat\naccount.var
&TX

    qui fait r�f�rence au drive "iode" d�fini sur la machine "GB".

    La lecture des r�pertoire est �galement possible sur un drive
    r�seau.

    Pour permettre l'introduction de noms plus longs, les champs des
    pages contenant un ou des noms de fichiers ont �t� �tendus � 64
    caract�res.

&TI RTF
���
    Les impressions en RTF sont �galement valables pour les graphiques.
    Les graphiques cr��s sont inclus dans le fichier RTF en format
    hexad�cimal et sont interpr�t�s par Word ou le compilateur
    d'aide Windows.

    La dimension de ces graphiques est d�termin�e par les param�tres
    d�finis dans Print Preferences/A2m ou par la commande de rapport
    $PrintGraphSize.

&TI MIF
���
    La g�n�ration des tableaux en MIF est am�lior�e : la largeur de
    la premi�re colonne est adapt�e au contenu et celle des colonnes
    suivantes est proportionnellement r�partie en fonction du nombre
    de colonnes.

    Graphiques : les attributs des caract�res dans les Labels et Titres
    des graphiques sont corrig�s.

&TI PRINTER
�������
    Nouvelles options d'impressions :

&EN Landscape ou Portrait
&EN Simplex, Duplex, Vertical Duplex
&EN S�lection de l'imprimante par d�faut.

&TI RAPPORTS
��������
    Trois nouvelles fonctions :

&EN $SetPrinter PrinterName
&EN $PrintOrientation {Portrait | Landscape}
&EN $PrintDuplex {Simplex | Duplex | VerticalDuplex}

&TI PRECISION DES DONNEES
���������������������

    Affichage en 8 ou 15 chiffres significatifs � l'�dition.
    La touche F6 permet de changer la pr�cision de l'�dition des
    variables (8 ou 15 chiffres significatifs).
    Sauvetage en 8 ou 15 chiffres significatifs en .av et .as
    Version double pr�cision pour les Variables et les Scalaires.

&TI Fonctions LEC
���
    Nouveaux op�rateurs
&CO
     var([from [,to],] x)             == sum((xi-xm)**2) / n
     covar([from [,to],] x, y)        == sum((xi-xm)*(yi-ym)) / n
     covar0([from [,to],] expr, expr) == sum(xi * yi) / n
     corr([from [,to],] x, y)         == covar(x, y) / sqrt(var(x) * var(y))
     stddev([from [,to],] x, y)       == sqrt(var(x))
&TX
    Modifications
&CO
    stderr : corrig� (estimateur non biais�) == sqrt(sum((xi-xm)**2/(n-1)
&TX
    L'ancien op�rateur stderr est remplac� par stddev.

&TI MESSAGES
��������
    Dans les rapports, les messages envoy�s par $show sont affich�s dans
    la derni�re ligne de l'�cran m�me dans le cours d'une proc�dure
    longue.

&TI WS/COPY
�������
    Correction dans le cas de listes non d�finies pass�es comme argument
    de la fonction.

&TI ESTIMATION DE BLOCS
�������������������
    Correction dans l'estimation de blocs dans les rapports. Auparavant,
    il fallait passer tous les noms des �quations du bloc comme argument
    de la fonction EsqEstimate. Une seule �quation suffit maintenant si
    elle contient le bloc.
>
<Version 5.03>
    Version 5.03 (08/02/1998)
    �������������������������

&TI Tableaux
��������
    S�lection des lignes � copier ou d�truire par Ctrl+L � nouveau
    op�rationnelle.

    Options graphiques :

&EN l'alignement des observations par rapport � l'abcisse (ann�e)
	  peut �tre sp�cifi� par courbe (touche F4).

&EN les options box et shadow sont op�rationnelles dans
	  l'affichage et l'impression des graphiques. Shadow
	  indique si on veut ou non un hachurage autour du graphique.

&EN les Grids (Major, minor ou None) sont impl�ment�es

&TI Graphiques
����������
    Quelques corrections d'affichage ont �t� op�r�es dans les graphiques.
    En particulier :

&EN il est possible d'imprimer des graphiques sans titre
&EN les bars charts fonctionnent

    Il est possible de modifier la taille des graphiques � imprimer
    ou � int�grer (Frame) par les Options d'impression (Print Setup/A2M).

    Les options Background Color et Brush + Box sont op�rationnelles
    pour les graphiques automatiques (variables et estimations).

    La position des bars ou des lignes par rapport � l'abcisse peut �tre
    choisie : gauche, centre ou droite (voir TABLEAUX).

&TI ESTIMATION
����������
    Les graphiques des estimations contiennent le nom de la variable
    endog�ne.


&TI A2M
���
    La g�n�ration des fichiers A2M s'est enrichie d'une commande
    de d�finition des cadres, couleur et fond des graphiques.
    Cette commande est g�n�r�e automatiquement en fonction
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
����������
    Une nouvelle option du menu syst�me (Properties) permet de
    visualiser le contenu global des WS, le directory de d�part et
    la m�moire disponible.

    Cette option est disponible � la fois lorsque la fen�tre est active
    et lorsqu'elle est minimis�e (bouton de droite).

&TI Titre de la fen�tre IODE
�������������������
    Le titre de la fen�tre contient le nom du directory de lancement
    de IODE et un num�ro d'ordre correspondant � la chronologie de
    d�marrage des processus IODE.

&TI Taille des caract�res
���������������������
    La taille des caract�res est maintenu pour les sessions suivantes de
    IODE (apr�s modification par le menu Syst�me de la fen�tre IODE).

&TI G�n�ration de fichiers d'aide Windows
�������������������������������������
    La g�n�ration des fichiers d'aide de Windows est rendue possible
    dans cette version avec une d�coupe en sujets (topics).

    Lors des impressions, chaque tableau et chaque �quation avec
    r�sultats d'estimation constituent un nouveau topic du fichier
    d'aide.

    De plus les fonctions de rapport suivantes permettent de sp�cifier
    les niveaux et le titres des sujets de l'aide.

&TI Fonctions de Rapports
�������������������������
    Des nouvelles fonctions ont �t� introduites dans les rapports pour
    permettre de sp�cifier les options d'impression dans le corps
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
�������������������
    Lorsque la configuration de la machine d�finit la virgule comme
    s�parateur des chiffres d�cimaux, les tranferts IODE-Excel
    remplacent les virgules par des points dans les nombres (et
    inversement).

&TI Ligne de commande
�����������������
    La syntaxe du programme IODE.EXE permet de charger plusieurs
    fichiers en une seule op�ration :

&CO
    iode monws.var monws.cmt monws.at
    iode monws.*
&TX
    Le tableau d'�dition correspondant au dernier fichier de la liste
    (ici monws.tbl) sera ouvert au d�marrage de IODE.
    Les fichiers non iode ne sont pas charg�s.

    De plus, les fichiers ASCII (.ac, .av, ...) peuvent �galement
    �tre charg�s de cette fa�on.

&TI Automatic launch in Netscape
��������������
    Le programme d'installation de IODE enregistre les types de fichiers
    .var, .cmt, etc de telle sorte que, si un lien vers un fichier IODE
    appara�t dans une page HTML (via Netscape par exemple), il soit
    possible d'ouvrir directement le fichier en question dans IODE.

    IODE est lanc� et le fichier r�f�renc� est charg� dans le WS
    correspondant.

    Attention, le fichier ainsi charg� est pr�alablement sauv� par
    Netscape dans un fichier temporaire situ� dans le directory
    ~c\Windows\Temp~C.
>

<Version 5.02>
    Version 5.02 (20/12/1997)
    �������������������������

&TI Impressions Objets
������������������
    Correction dans l'impression des variables.

&TI Rapports
��������
    Cr�ation des listes : le point-virgule sert de s�parateur (au lieu
    de la virgule ant�rieurement).

&IT     $DSImportDB
    �����������
    Importation de donn�es de DataStream.

   Syntaxe :
&CO
	$DSImportDB nom_data_stream1, ...
&TX
    Cette fonction va lire sur le sample courant la ou les variables
    d�finies par nom_data_stream1, ... et cr�e dans le WS courant une
    s�rie dont le nom est construit en rempla�ant les caract�res non
    alphanum�riques par des understroke.

   Exemple :
&CO
	$DSImportDB BEGDP..A12
&TX
    cr�e ~cBEGDP__A12~C dans le WS courant.

    Si le nom_data_stream contient le caract�re pourcent (%), la
    fonction ne s'ex�cute pas car % est un caract�re r�serv� dans les
    rapports. Pour pallier ce probl�me, il faut cr�er une liste
    contenant le nom des s�ries et appeler DSImportDB avec la liste
    comme argument.

&CO
	$DSImportDB $MYLST
&TX
    Note : cette fonction n'est op�rationnelle actuellement que sur les machines
	   qui ont un acc�s � DataStream.

&TI Uninstall
���������
    Nouvelle fonction de d�sinstallation de IODE. Cette fonction est
    accessible soit via Control Panel/Install Software, soit dans le
    menu START/Groupe IODE.
>
<Version 5.01>
    Version 5.01 (17/12/1997)
    �������������������������

&TI Rapports
��������
    La fonction $EqsEstimate imprime les graphiques des observations
    et des valeurs estim�es.
&CO
    $IdtExecuteTrace werkt terug
&TX
&TI Execute IDT's
�������������
    Bij het uitvoeren van identiteiten wordt er per default geen output
    meer gegenereerd. Alleen als de "Trace" of "Debug" optie wordt gebruikt
    krijgt u een overzicht van de uitgevoerde identiteiten. Dit kan u
    fullscreen, of door het $IdtExecuteTrace commando in rapporten.

&TI Print Files
�����������
    U kan uw a2m-files en rapport-definities vanuit IODE afprinten.
>
<Version 5.0>
    Version 5.0 (15/10/1997)
    ������������������������
&TI Direct Printen vanuit IODE
��������������������������

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
    E�n van de formaten waaronder u kan printen is good-old A2M.

&TI Grafieken in A2M
����������������

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
��������������������������

    Bij het uitvoeren van een rapport moet u aangeven onder welke vorm
    en in welke file u uw output wil recuperen. Per default wordt niets
    afgedrukt.

    Om vanuit een rapport gebruik te kunnen maken van de nieuwe print-
    mogelijkheden, werden de "PrintDest" en "PrintDestNew" commando's
    aangepast. U vroegere rapporten moet u niet aanpassen, zij zullen
    A2M-bestanden blijven aanmaken.

&IT    Vroeger
    �������
&CO
	$PrintDest filename (vb. $PrintDest tabel.a2m)
&TX
&IT    Nu
    ��
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
�������������������������������������
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
������������
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
��������
    Nouvelles fonctions dans les rapports :

&EN ~c$Minimize~C : minimise la fen�tre de IODE (par exemple pour
		    afficher des graphiques de Excel g�n�r�s par
		    la simulation en cours)
&EN ~c$Maximize~C : restaure la fen�tre de IODE
&EN ~c$Sleep~C nn : arr�te le processus pendant nn milli�mes de seconde.
		    Cela permet � un client ou un serveur DDE de
		    conserver le contr�le en cas de requ�tes trop
		    rapides.
&TI CORRECTION
����������
    Erreur lors de la destruction d'un objet (tableau par exemple)
    corrig�e.

&TI RAPPORTS
��������
    La fonction ~c$EqsEstimate~C imprime les graphiques des observations
    et des valeurs estim�es.
>

<Version 4.73>
    Version 4.73 (08/08/1997)
    �������������������������
&TI Calcul de taux de croissance moyens
�����������������������������������
    Lorsqu'une valeur n�gative r�sulte de la division de la p�riode
    de fin par la p�riode de d�but dans un calcul de taux de croissance
    moyen, le calcul de la racine de cette valeur 1/n est impossible.
    Dans ce cas, la valeur retourn�e est --, alors qu'auparavant, le
    programme avait un comportement erratique.
>
<Version 4.71>
    Version 4.71 (20/05/1997)
    �������������������������

&TI IMPRESSIONS
�����������
    Le nombre de caract�res pr�vus pour les valeurs des tableaux et des
    coefficients est augment�.

&TI *ODEW32
�������
    Am�lioration de la gestion de l'utilisation CPU de la machine.
    Cette correction am�liore sur certaines configurations les
    performances d'acc�s aux r�seaux.
>
<Version 4.72>
    Version 4.72 (17/06/1997)
    �������������������������

La version 4.72 contient trois groupes de fichiers suppl�mentaires :

&EN Version IODE double pr�cision
&EN WS APL d'interface avec IODE
&EN Programme A2M pour Windows 95

&TI VERSION DOUBLE PRECISION
������������������������
    Les donn�es num�riques (s�ries, coefficients, ...) sont g�r�es en
    IODE en simple pr�cision, c'est-�-dire avec une pr�cision de 8
    chiffres significatifs.

    Dans certains cas, cette limite est "un peu courte". A cette fin,
    une version double pr�cision (16 chiffres significatifs) a �t�
    d�velopp�e. Les programmes de cette version (uniquement win32) sont
    IODEW32d, XODEW32d, GODEW32d et TODEW32d.

    Pour pouvoir exploiter ces programmes, il faut d'abord exporter en
    ASCII tous les fichiers n�cessaires, � l'exception des commentaires
    et des listes. Utilisez pour cela XODEW32. Les fichiers de
    variables, de tableaux, d'identit�s et d'�quations contenant des
    informations num�riques, ils doivent �tre traduits avant exploitation.

    Il faut ensuite importer ces fichiers en double pr�cision � l'aide
    de XODEW32d. On constatera �videmment que les fichiers peuvent
    doubler de taille.

    Ensuite, IODEW32d peut �tre utilis� exactement comme IODEW32.


&TI WS APL d'interface avec IODE
����������������������������
    Les fonctions de ce WS sont affich�es lors de son chargement.

    Elles permettent :

&EN de piloter IODE � partir de l'APL
&EN de charger des donn�es de IODE vers APL
&EN de copier les donn�es de l'APL vers IODE

    Pour que cela fonctionne, il faut d�marrer le programme IODEW32 ou
    IODEW32d. Plus d'explications seront fournies dans un document
    s�par�.


&TI PROGRAMME A2M pour WIN95
������������������������
    Un programme permettant d'imprimer ou de traduire des fichiers a2m
    en diff�rents langages est distribu� sur les disquettes de IODE.

    Ce programme, ind�pendant de IODE, peut �tre lanc� n'importe quand
    et exploit� en cours de travail dans IODE. Il ne fonctionne que sous
    Windows 95.

    A2m contient �galement une s�rie d'utilitaires de manipulation de
    fichiers HTML.

&TI DDE SERVER
����������
    Un serveur DDE est impl�ment� dans IODEW32 et IODEW95. Cela
    signifie qu'il est possible d'exploiter les fonctions et les
    donn�es de IODE dans le cadre d'autres programmes, comme
    Excel ou APL.

    La version actuelle est encore une version limit�e: toutes les
    possibilit�s exploitables ne sont donc pas impl�ment�es. En fonction
    des demandes futures, d'autres fonctions pourront �tre d�finies.

&IT    Utilisation en Excel
    ��������������������
    Par exemple, on peut placer dans une cellule d'un tableau Excel
    la r�f�rence � une variable de IODE sous le format suivant :
&CO
	=IODE|XVAR!'SER1!Sheet1!R2C2'
&TX
    La valeur de la s�rie sera plac�e � la position demand�e.

&IT    Utilisation en APL
    ��������������������
    En APL, un WS IODE a �t� programm� pour permettre de lancer des
    commandes de rapport � partir d'une fonction APL, charger en APL des
    donn�es de IODE et replacer ces donn�es apr�s modification dans
    l'environnement IODE. Une fonction permet de piloter compl�tement
    IODE � partir de l'APL.

&IT    Fonctions du serveur DDE
    ������������������������
    Les donn�es techniques qui suivent permettent d'exploiter le
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
	     ELIST  = liste des �quations du WS IODE courant
	     ILIST  = liste des identit�s du WS IODE courant
	     LLIST  = liste des listes du WS IODE courant
	     SLIST  = liste des scalaires du WS IODE courant
	     TLIST  = liste des tableaux du WS IODE courant
	     VLIST  = liste des variables du WS IODE courant

	     CNAME  = nom du fichier des commentaires de IODE
	     ENAME  = nom du fichier des �quations de IODE
	     INAME  = nom du fichier des identit�s de IODE
	     LNAME  = nom du fichier des listes de IODE
	     SNAME  = nom du fichier des scalaires de IODE
	     TNAME  = nom du fichier des tableaux  de IODE
	     VNAME  = nom du fichier des variables de IODE

	     CDESCR = description du fichier des commentaires de IODE
	     EDESCR = description du fichier des �quations de IODE
	     IDESCR = description du fichier des identit�s de IODE
	     LDESCR = description du fichier des listes de IODE
	     SDESCR = description du fichier des scalaires de IODE
	     TDESCR = description du fichier des tableaux  de IODE
	     VDESCR = description du fichier des variables de IODE

	     CNB    = nombre de commentaires dans IODE
	     ENB    = nombre de �quations dans IODE
	     INB    = nombre de identit�s dans IODE
	     LNB    = nombre de listes dans IODE
	     SNB    = nombre de scalaires dans IODE
	     TNB    = nombre de tableaux  dans IODE
	     VNB    = nombre de variables dans IODE

	Items de CMT, EQS, IDT, LST, SCL, TBL, VAR
	------------------------------------------
	     L'item est le nom de l'objet. La valeur retourn�e
	     est la d�finition de l'objet.

	Items de XVAR
	-------------
	    L'item est compos� de trois �l�ments s�par�s par des ! :
		- nom de la ou des variables s�par�s par des virgules ou
		    des blancs. t repr�sente une ligne de p�riodes
		- nom du sheet o� placer le r�sultat
		- range de la premi�re cellule dans ce sheet

	    Cette requ�te g�n�re un Poke vers excel avec la d�finition
	    des variables et leur code.

	    Si aucun nom de s�rie n'est donn�, une s�rie avec les
	    valeurs des p�riodes pr�c�de. Sinon, on peut placer
	    les p�riodes en indiquant la s�rie t (minuscule).

	    S'il n'y a pas de sheet, Sheet1 est choisi comme destination
	    S'il n'y a pas de range, R1C1 est choisi comme destination

	    Exemples
	    --------
		IODE|XVAR!' ' : toutes les s�ries dans le sheet1, en R1C1
				avec les p�riodes
		IODE|XVAR!'t,QC0,QAFF!Sheet2!R1C1' : s�ries QCO et QAFF
				dans le Sheet2 en position R1C1, avec
				les p�riodes en premi�re ligne
		IODE|XVAR!'QC0!Sheet2!R1C1' : s�rie QCO sans les p�riodes

	Items de XCMT
	-------------
	    L'item est compos� de trois �l�ments s�par�s par des ! :
		- nom de la ou des commentaires s�par�s par des virgules ou
		    des blancs
		- nom du sheet o� placer le r�sultat
		- range de la premi�re cellule dans ce sheet

	    Cette requ�te g�n�re un Poke vers excel avec la valeur des
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
	     ENAME  = change le nom du fichier des �quations de IODE
	     INAME  = change le nom du fichier des identit�s de IODE
	     LNAME  = change le nom du fichier des listes de IODE
	     SNAME  = change le nom du fichier des scalaires de IODE
	     TNAME  = change le nom du fichier des tableaux  de IODE
	     VNAME  = change le nom du fichier des variables de IODE

	     CDESCR = change la description des commentaires de IODE
	     EDESCR = change la description des �quations de IODE
	     IDESCR = change la description des identit�s de IODE
	     LDESCR = change la description des listes de IODE
	     SDESCR = change la description des scalaires de IODE
	     TDESCR = change la description des tableaux  de IODE
	     VDESCR = change la description des variables de IODE

	     CCLEAR = Clear des commentaires dans IODE
	     ECLEAR = Clear des �quations dans IODE
	     ICLEAR = Clear des identit�s dans IODE
	     LCLEAR = Clear des listes dans IODE
	     SCLEAR = Clear des scalaires dans IODE
	     TCLEAR = Clear des tableaux dans IODE
	     VCLEAR = Clear des variables dans IODE

	     CLOAD = Load un fichier de commentaires dans IODE
	     ELOAD = Load un fichier d'�quations dans IODE
	     ILOAD = Load un fichier d'identit�s dans IODE
	     LLOAD = Load un fichier de listes dans IODE
	     SLOAD = Load un fichier de scalaires dans IODE
	     TLOAD = Load un fichier de tableaux dans IODE
	     VLOAD = Load un fichier de variables dans IODE

	     CSAVE = Sauve le fichier de commentaires de IODE
	     ESAVE = Sauve le fichier d'�quations de IODE
	     ISAVE = Sauve le fichier d'identit�s de IODE
	     LSAVE = Sauve le fichier de listes de IODE
	     SSAVE = Sauve le fichier de scalaires de IODE
	     TSAVE = Sauve le fichier de tableaux de IODE
	     VSAVE = Sauve le fichier de variables de IODE

	Items de PLAY
	-------------
	     TEXT : data = texte � placer dans le buffer clavier
		    de IODE
	     KEYS : data = texte des touches � placer dans le buffer
		    clavier : aA..aZ, aF1..aF10, cA..cZ, cF1..cF10,
			      sF1..sF10, ESC, ENTER, BACKSPACE,
			      HOME, END, LEFT, RIGHT, UP, DOWN,
			      PGUP, PGDN, DEL, TAB

	Items de CMT, EQS, IDT, LST, SCL, TBL, VAR
	------------------------------------------
	    Nom de l'objet � modifier.
	    Data contient la valeur avec des TAB comme s�parateur
	    dans le cas des variables.

	Items de REP
	------------
	    La valeur de l'item est sans signification.
	    La valeur de data est une commande de rapport � ex�cuter
	    (ex. '$WsLoadVar test')
&TX
>

<Version 4.70>
    Version 4.70 (10/03/1997)

&TI G�n�ration de fichiers HTML


&IT   Param�tres de QODE

    De nouveaux param�res se sont ajout�s au programme QODE pour
    affiner la g�n�ration des fichiers HTML :

&EN -toc tit_level : indique s'il faut g�n�rer une table des
	 mati�res au d�but du document et jusqu'� quel niveau de titre

&EN -parb tit_level et

&EN -pari tit_level : indique s'il faut donner un niveau de
	    titre aux paragraphes parb et pari (on donc �ventuellement
	    les introduire dans la table des mati�res

&IT    HTML g�n�r�
    �����������
    En d�but et en fin du fichier HTML se trouvent deux commentaires
    qui permettent d'effectuer un remplacement automatique dans le
    fichier, afin par exemple de placer une barre navigationnelle
    standardis�e en haut et en bas du document.

    Ces commentaires sont :
&CO
	<<!STARTHEADER>>
	<<!ENDHEADER>>

	<<!STARTFOOTER&>>
	<<!ENDFOOTER>>
&TX
    Le programme scr4_sbs permet de substituer ces textes par le contenu
    d'un fichier. La commande � effectuer est :
&CO
	scr4_sbs "<<STARTHEADER>>" "<<!ENDHEADER>>" replacefile
		file.htm
&TX
    Elle a pour effet de remplacer dans le fichier file.htm tout ce qui
    se trouve entre ~c<<!STARTHEADER>>~C et ~c<<!ENDHEADER>>~C,
    ces textes compris, par le contenu du fichier replacefile.

&TI Graphiques
������
    Correction de l'affichage et de l'impression des graphiques
    � partir des tableaux. Le buffer contenant le titre du tableau est
    allou� dynamiquement: auparavant, un titre de plus de 80 caract�res
    plantait le programme.

&TI Fichier printcap for WinNFS de Network Instruments
�����������������������������������
    Le fichier printcap.ni contient les instructions n�cessaires
    � l'impression via IODE32 ou QODE sous Windows 95. EN effet,
    le logiciel Winsock Companion de Network Instruments est utilis�
    en lieu et place de PC-NFS sous WIndows 95, ce qui exige une autre
    configuration.

    Attention, � la diff�rence des la version PC-NFS, les imprimantes
    doivent �tre pr�alablement d�finies dans l'environnement Windows
    pour pouvoir �tre utilis�es.

&TI Corrections de bugs
���
    Les variables ~c_YOBSi~C, ~c_YCALCi~C et ~c_YRESi~C g�n�r�es
    automatiquement lors des estimations �taient parfois absentes. Cette
    erreur est corrig�e.

&TI QODE : g�n�ration de fichier MIF

    La traduction de fichiers a2m en format MIF �tait impossible
    si la largeur d'une colonne (la premi�re) �tait sup�rieure �
    60 caract�res. Cette limite est port�e � 400 caract�res.

    La limite � 60 caract�res est cependant conserv�e pour les outputs
    en fichier formatt�s Ascii.
>
<Version 4.69>
    Version 4.69 (12/09/1996)
    �������������������������

..esc ~

&TI WsCopyVar, HighToLow, LowToHigh, Census X11
�������������������������������������������
    Ces fonctions sont corrig�es : en utilisation hors des
    rapports, elles plantaient IODE (toutes versions).

&TI G�n�ration des tableaux
�����������������������
    Correction dans le calcul lorsque le diviseur est non LEC.

&TI Simulation
����������
    La version DOS 32 bits a �t� quelque peu acc�l�r�e.

&TI Estimation
����������
    Les tests r�sultant de la derni�re estimation sont sauv�s dans
    des scalaires de fa�on � permettre diff�rents calcul par la
    suite. Il en va de m�me pour les r�sidus, membres de gauche et
    de droite des �quations.

    Les tests portent les noms suivants (~ce0_*~C pour la premi�re �quation
    du bloc, ~ce1_*~C pour la deuxi�me, ...) :

&EN        ~ce0_n       ~C  : nombre de p�riode du sample
&EN        ~ce0_k       ~C  : nombre de coefficients estim�s
&EN        ~ce0_stdev   ~C  : std dev des r�sidus
&EN        ~ce0_meany   ~C  : moyenne de Y
&EN        ~ce0_ssres   ~C  : somme du carr�s des r�sidus
&EN        ~ce0_stderr  ~C  : std error
&EN        ~ce0_stderrp ~C  : std error %
&EN        ~ce0_fstat   ~C  : F-Stat
&EN        ~ce0_r2      ~C  : R carr�
&EN        ~ce0_r2adj   ~C  : R carr� ajust�
&EN        ~ce0_dw      ~C  : Durbin-Watson
&EN        ~ce0_loglik  ~C  : Log Likelihood

    Les s�ries calcul�es sont �galement sauv�es dans une variable
    sous les noms :

&EN  ~c_YCALC0~C pour le membre de droite de la premi�re �quation du
	  bloc, ~c_YCALC1~C pour la deuxi�me �quation, etc.

&EN  ~c_YOBS0~C pour le membre de gauche de la premi�re �quation du
	  bloc, ~c_YOBS1~C pour la deuxi�me �quation, etc.

&EN  ~c_YRES0~C pour les r�sidus de la premi�re �quation du bloc, ...

    En dehors du sample d'estimation, les valeurs de la s�rie sont --.
>

<Version 4.68>
    Version 4.68 (16/07/1996)
    �������������������������

&TI HTML
����
&EN Correction dans le cas des caract�res sp�ciaux ayant une
      signification particuli�re en HTML (<</A>>, ...). Ces caract�res
      �taient transform�s auparavant.

&EN Le format des tableaux est all�g� : les lignes verticales
      et horizontales sont supprim�es. De plus, les cellules de
      type LINE g�n�rent une ligne intercalaire dans le tableau
      en HTML.

&EN L'alignement des nombres se fait � droite et plus sur
      le point d�cimal. En effet, Netscape ne g�re pas correctement
      l'alignement d�cimal.

&TI Estimation
����������
    L'impression des r�sultats des estimations ne g�n�re plus de
    tableaux pour �viter les sauts de pages inutiles en Postscript.

&TI Print/View Variables
��������������������
    Il est dor�navant possible d'afficher des valeurs r�sultant de
    formules contenant l'op�rateur * ou contenant des virgules (min,
    vmax, ...).
    Cependant, les s�parateurs entre les formules sont dor�navant
    limit�s aux blancs, points-virgules et retour ligne.

&TI Noms de fichiers
����������������
    A la demande de plusieurs utilisateurs, la longueur des noms de
    fichiers est port�e dans la plupart des �crans � un minimum de 40
    caract�res.
    Le prix � payer est que les "profiles" des versions ant�rieures
    ne sont plus compatibles avec la version 4.68.

&TI Fichiers a2m
������������
    Dans les �crans o� l'on permet d'"imprimer" dans un fichier a2m,
    il est possible de sp�cifier si l'on souhaite que le fichiers
    soient vid�s avant l'impression. Cela permet d'�viter que
    des fichiers d'output accumulent des impressions successives.

    Notons que cette option est li�e � la fonction de rapport
    $PrintDestNew (voir 4.67).

&TI Execute identities
������������������
    Le fichier "trace" optionnel est r�initialis� � chaque ex�cution.

&TI Simulation Debug
����������������
    L'option debug de la simulation g�n�re automatiquement des listes
    comprenant des �quations pr�- et post-r�cursives.
    Dans cette version, un fichier de trace est �galement g�n�r�. Ce
    fichier se nomme "simul.dbg" et se trouve dans le directory
    d'o� IODE a �t� lanc�.
    Le fichier de trace comprend actuellement la liste des �quations
    pour lesquelles une sous-it�ration (Newton-Raphson) a �t�
    n�cessaire et le nombre de sous-it�rations.

&TI Remarques
���������
    Dans les rapports, les expressions du type
&CO
	    $Goto label {X[1975Y1] << 2}
&TX
    ne fonctionnent que si la p�riode est fix�e par la fonction
&CO
	    $SetTime
&TX
    Si ce n'est pas le cas, l'expression (m�me si l'ann�e est fix�e
    comme dans l'exemple) ne s'ex�cutent pas.
>
<Version 4.67> (12/07/1996)
    Version 4.67 (12/07/1996)
    �������������������������

&TI Versions Windows
����������������
&EN Meilleure gestion de la souris.
&EN Stop possible plus souvent.

&TI HTML
����
    Il est possible de sp�cifier un fichier HTML comme sortie de
      QODE ou TODE. Cette fonction g�n�re du HTML 3.0 (y compris les
      tableaux).

&TI Crit�re de convergence Newton
�����������������������������
    Dans cette version, on a repris deux types de crit�res dans le
    cas d'une �quation non r�solue par rapport � l'endog�ne ou dans
    le cas du goal seeking. Dans la version 4.66, on avait renforc�
    les contraintes de convergence. Dans la version 4.67, en attendant
    un refonte de l'algorithme, on reprend le crit�re 4.66 (crit�re
    fort, comme dans KaA), mais en cas de non convergence par cette
    m�thode, on reprend le crit�re ant�rieur (v.4.65).

&TI Rapports
��������
&IT    Nouvelle fonction $msg
    ����������������������

	Elle affiche le texte de l'argument et attend une touche avant
	de continuer. Cela permet de stopper momentan�ment le rapport
	pour afficher un message � l'intention de l'utilisateur,
	contrairement � la fonction $show qui affiche dans le bas de
	l'�cran mais ne stoppe pas.

&CO
	    $msg texte libre
&TX
&IT    Macros
    ������
	Les macros sont d�finies entre des pourcents : %FILE%. Si le
	premier pourcent est imm�diatement suivi du caract�re

&EN            ## : le contenu de la macro est transpos� en majuscules
&EN            ! : le contenu de la macro est transpos� en minuscules

	Par exemple,
&CO
	    ##define TOTO TBL1
	    Premier  tableau : %!TOTO%
	    Deuxi�me tableau : %##TOTO%
&TX
	Donnera � l'ex�cution :
&CO
	    Premier  tableau : tbl1
	    Deuxi�me tableau : TBL1
&TX
&IT    Noms des fichiers a2m
    ���������������������
	Pour �viter les probl�mes de noms de fichiers entre DOS et
	Unix, les noms des fichiers a2m d�finis par la commande
	$PrintDest sont toujours transpos�s en minuscules.

	Le nom des fichiers output peut atteindre 64 caract�res au
	lieu de 40 auparavant.

&IT    Fonction FileDelete*
    ��������������������
	Le fichier destination de l'output est ferm� avant toute
	destruction de fichier pour �viter les erreurs dans les
	rapports du type suivant :
&CO
	    $PrintDest res.a2m

	    $Label Again
	    $FileDeleteA2m res        (le fichier est ouvert!)
	    ...
	    $Goto Again
&TX
&IT    Nouvelle fonction $PrintDestNew
    �������������������������������
	Cette fonction d�truit le fichier output avant de commencer
	� l'utiliser. Cela �vite de cr�er de gigantesques fichiers
	accumulant tous les r�sultats successifs.
>

<Version 4.66> (17/01/1996)
    Version 4.66 (17/01/1996)
    �������������������������

&TI Crit�re de convergence Newton
�����������������������������
    Lorsqu'une �quation n'est pas r�solue analytiquement par IODE
    (endog�ne r�p�t�e dans l'�quation), un sous-algorithme de Newton est
    utilis� pour obtenir la convergence de cette �quation.

    En raison du manque relatif de pr�cision dans les calculs, cette
    sous it�ration peut dans certains cas ne pas converger correctement.

    Dans cette version, la contrainte de convergence est plus
    restrictive qu'auparavant, s'alignant de la sorte sur celui de KaA.
    Par cons�quent, il est possible que certaines �quations ne
    convergent plus dans cette nouvelle version.

    Dans une version en pr�paration, tout ce probl�me sera revu et de
    nouveaux algorithmes seront propos�s.

&TI Noms des fichiers (Load)
������������������������
    Un erreur se produisait lors du chargement d'un fichier dont le nom
    �tait du type "../nom". Le message "file not found or incompatible
    type" �tait alors produit. Cette erreur est corrig�e.

&TI Profiles
��������
    Les profils qui permettent de retrouver d'une session � l'autre les
    m�mes �crans sont standardis�s (DOS 16 bits ou 32 bits).
>


<Versions 4.64-65> (17/10/1995)
    Versions 4.64-65 (17/10/1995)
    �����������������������������

&TI Qode (Postscript)
�����������������

Le driver Postscript incorpor� dans QODE et QODE32 a �t� adapt� aux
imprimantes HP4m+ et HP4mv.

Bureau du Plan : les fichiers printcap.nfs et printcap sont adapt�s
		 aux nouvelles imprimantes r�seau.

>

<Version 4.63> (02/10/1995)
    Version 4.63 (02/10/1995)
    �������������������������

&TI Tableaux en Mif
���������������

    Une simplification de l'importation en Frame des tableaux (QODE) a
    �t� apport�e dans cette version : auparavant, il fallait disposer du
    fichier a2m.m4 dans le directory du fichier mif pour pouvoir ouvrir
    avec succ�s en Frame un fichier MIF contenant un tableau
    (et encore...).
    Typiquement, si ce n'�tait pas le cas, on obtenait un document Frame
    vide.

    Dor�navant, l'importation des tableaux r�pond aux r�gles suivantes :

&EN        il ne faut plus de fichier a2m.m4
&EN        il n'y a plus de lignes en haut et en bas du document,
	  h�ritage des versions 1.0 et 1.3 de Frame
&EN        les tableaux g�n�r�s sont de type "Format A", formt d�fini dans
	  le catalogue de tableaux de tout fichier Frame

    Pratiquement, pour importer des tableaux en Frame, on proc�de
    comme suit :

&EN        cr�er les tableaux dans un fichier a2m en IODE ou IODE32
	    (Print Tables)
&EN         "Quick print a2m file" vers un fichier mif
&EN         lancer Frame
&EN         deux possibilit�s ensuite :

&EN2             ouvrir le fichier mif (File/Open)
&EN2             importer le fichier mif dans un document existant
		(File/Import/File)

    Dans le dernier cas (File/Import), on peut exploiter un format
    pr�d�fini : il suffit de d�finir dans un template de Frame
    un type de tableau "Format A" selon ses desiderata.
>


<Version 4.62> (29/09/1995)
    Version 4.62 (29/09/1995)
    �������������������������
&TI Versions ?ODE32
���������������
    Une erreur s'est gliss�e dans le portage de la version 4.61 en 32 bits.
    L'interpr�tation des valeurs non enti�res (dans les fichiers
    ASCII, les �quations et les identit�s) �tait erron�e.
>

<Version 4.61> (02/09/1995)
    Version 4.61 (02/09/1995)
    �������������������������
&TI Regroupement de fichiers
������������������������
    Le nombre de fichiers distribu�s dans IODE est consid�rablement
    diminu� : les ex�cutables de IODE int�grent toutes les informations
    n�cessaires � leur fonctionnement, exception faite des fichiers
    d'aide et du fichier printcap de d�finition des imprimantes locales.
>

<Version 4.60> (15/05/1995)
    Version 4.60 (15/05/1995)
    �������������������������
&TI LEC : variable i
����������������
    Nouvelle variable dans le langage LEC permettant de conna�tre lors
    d'un calcul la diff�rence entre l'ann�e de calcul d'une
    sous-expression par rapport � l'ann�e de calcul de toute la formule.
    Cette variable s'appelle i et son comportement s'apparente � celui
    de t. Elle est donc invariante par rapport � l'ann�e de calcul d'une
    formule LEC.

    On calcule toujours une forme LEC pour une valeur du temps t donn�e,
    mais dans une sous-expression, t peut �tre diff�rent. C'est par
    exemple le cas pour les op�rateurs d(), dln(), sum(), ...
    On peut, grace � i, conna�tre la diff�rence entre le t de calcul de la
    formule et le t de calcul de la sous-expression courante.

    On peut donc calculer des expressions comme
&CO
	sum(t-2, t-4, A / (1 - i)**2),
&TX
    qui �quivaut � :
&CO
	A[-2]/(1-(-2))**2 + A[-3]/(1-(-3))**2 + A[-4]/(1-(-4))**2
&TX
    Sans i, une telle expression ne peut �tre �crite en LEC.

    Cet op�rateur a �t� impl�ment� pour permettre la traduction de
    l'op�rateur SUM de TROLL.

    Ainsi, si on calcule une expression pour un t donn�, i prend les
    valeurs suivantes selon les cas. Attention, les expressions
    indiqu�es ci-dessous sont les formules compl�tes, et pas
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
�����������
    Correction dans le cas des fichiers input dont la derni�re observation
    fait partie du calcul des s�ries output : un fichier 93M1:94M12
    donnait la valeur NA pour l'ann�e 94.

>

<Version 4.59> (06/03/1995)
    Version 4.59 (06/03/1995)
    �������������������������
&TI Tableaux
��������
    Correction de la fonction de cr�ation automatique (rapports).

&TI Listes
������
    Toutes les listes g�n�r�es ont pour s�parateur le point-virgule au
    lieu de la virgule.

    Lors de l'importation de listes via XODE, les listes avec
    s�parateur point-virgule sont correctement transf�r�es et non
    plus scind�es en plusieurs lignes s�par�es n'importe o�.

>

<Version 4.58> (13/02/1995)
    Version 4.58 (13/02/1995)
    �������������������������
&TI Simulation
����������
    Les valeurs initiales des endog�nes sont calcul�es �galement
    pour les blocs pr�- et post-r�cursifs. Auparavant, seuls les
    endog�nes du bloc interd�pendant �taient initialis�es.

&TI Tableaux
��������
    La cr�ation automatique des tableaux s'est enrichie de deux nouvelles
    fonctionnalit�s : au niveau des rapports et au niveau de la cr�ation
    manuelle d'un tableau.

&IT    1. Dans les rapports
    ��������������������
	La fonction $DataUpdateTbl a la syntaxe suivante :
&CO
	    $DataUpdateTbl table_name title;lec1;lec2;...
&TX
	Si title est un nom de commentaire, le commentaire est utilis�
	comme titre.
	De m�me, si on trouve des noms de variables comme forme lec, et
	qu'il existe un commentaire pour ces variables, le titre de
	la ligne correspondante est remplac� par la valeur du
	commentaire.

&IT        Exemple
	�������
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
	cr�e le tableau T1 avec la forme suivante :
&CO
	    ���������������������������������������������
	    �          Titre de mon tabelo              �
	    �                                           �
	    �  Produit national brut               A    �
	    �  B                                   B    �
	    �  A+B                                 A+B  �
	    ˗�������������������������������������������
&TX
&IT    2. Cr�ation manuelle
    ��������������������
	Les m�mes r�gles s'appliquent dans la cr�ation d'un tableau. De
	plus, il est possible d'utiliser des listes dans le champ de
	d�finition des formes LEC.


>


<Version 4.57> (25/01/1995)
    Version 4.57 (25/01/1995)
    �������������������������
&TI Estimation
����������
    1. L'�cran de d�finition des �quations reprend dor�navant les
       valeurs des tests avec 3 d�cimales, y compris dans le cas
       des blocs d'�quations. Ces m�mes tests sont sauv�s dans
       toutes les �quations du bloc.

    2. L'impression des coefficients se fait en format d�cimal dans
       tous les cas, alors qu'auparavant le format d�pendait de
       la valeur (1e-03 au lieu de 0.001).

>


<Version 4.56> (02/12/1994)
    Version 4.56 (02/12/1994)
    �������������������������
&TI Xode
����
&EN (R�-)Adaptation de XODE pour la lecture des donn�es des fichiers NIS.
&EN  Correction de la lecture des tableaux ascii.

&TI Iode/Tableaux
�������������
    Le nombre maximum de colonnes d'un tableau est port� � 20. Rappelons
    cependant que la taille totale d'un objet ne peut d�passer 16000
    bytes (texte, formules LEC, donn�es internes). Plus le nombre de
    colonnes est �l�v�, plus le nombre de lignes devra �tre limit� !

>

<Version 4.55> (28/10/1994)
    Version 4.55 (28/10/1994)
    �������������������������
&TI Xode
����
    Adaptation au nouveau format des fichiers NIS.

&TI Fode
����
    Am�lioration de la version batch du programme. La nouvelle syntaxe
    est :
&CO
	fode[32] -i file ... -o outputfile
&TX
    Dans le cas o� fode est lanc� sans param�tre, il fonctionne
    comme avant en mode interactif.
    Sinon, les fichiers suivant -i sont trait�s et le r�sultat
    est plac� dans le fichier ASCII d�fini par le param�tre -o.

    Am�lioration de la pr�sentation en colonnes lorsque le texte
    d'une colonne s'�tend sur plusieurs lignes.

>

<Version 4.54> (23/08/1994)
    Version 4.54 (23/08/1994)
    �������������������������
&TI Editeur MMT
�����������
    Le sauvetage d'un fichier �dit� � l'aide de l'�diteur int�gr� MMT
    pla�ait des tabulations en d�but de lignes lorsqu'au moins 8
    blancs �taient pr�sents. Lors de l'impression d'un tel fichier
    (par exemple dans le cadre de FODE), ces blancs de d�but de lignes
    semblaient - sur certaines imprimantes - �tre ignor�s. En fait,
    les imprimantes ne repr�sentent pas toutes les tabulations de la
    m�me mani�re, ce qui explique les �ventuels probl�mes.

    Pour pallier ces inconv�nients, l'�diteur int�gr� MMT sauve
    dor�navant tous les blancs comme tels, sans remplacer les suites
    de 8 blancs par des tabulations.


    Attention : cette modification n'est valable que dans IODE, TODE,
		FODE, etc, mais pas dans le programme ind�pendant MMT.

&TI Xode
����
    Rules file : une ligne blanche intercalaire arr�tait le processus de
    lecture. Dor�navant les lignes blanches sont ignor�es.

&TI Xode : Export by rule
���������������������

    Cette nouvelle option permet de g�n�rer des fichiers
    de donn�es en formats externes : Comma separated values (CSV), TSP,
    WKS (Lotus/Excel), DIF.

    Le nom du fichier de variables est requis.

    Le fichier des "rules" permet de limiter les variables � exporter et
    �ventuellement d'en changer les noms.

    Si un fichier de commentaires est indiqu�, chaque variable est
    accompagn�e der son commentaire.

    Si le sample n'est pas pr�cis�, toutes les observations sont
    fournies dans le fichier r�sultat.

    Dans le cas du format CSV, deux �l�ments sont libres : le s�parateur
    de colonnes (par d�faut ;) et la repr�sentation de la valeur
    inconnue (par d�faut ##N/A).

    Dans le cas des fichiers WKS, un "Label" est associ� � chaque range
    de valeurs, ce label �tant identique au code de la s�rie.

&TI Xode
����
    La fonction d'importation de XODE accepte les valeurs des tests
    statistiques de l'estimation sous forme d'un mot-cl� et de la
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
    Ces informations sont g�n�r�es automatiquement par la fonction
    d'exportation de XODE.
>

<Version 4.53> (06/06/1994)
    Version 4.53 (06/06/1994)
    �������������������������
&TI Rapports
��������
    La fonction $ReportExec accepte une liste comme argument :
&CO
	    $ReportExec test $LISTE
&TX
    re�oit comme argument les �l�ments de la liste $LISTE et permet
    donc d'ex�cuter un rapport type sur un nombre initialement
    inconnu d'arguments. Par exemple, pour remplacer tous les 0 par
    des NA pour la liste $LISTE de variables, il suffit de faire :


    FICHIER ZTONA.REP
    �����������������
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
    �����
&CO
	$ReportExec ztona $LISTE
&TX
	ou, dans l'�cran d'ex�cution :
&CO
	Report Name : ztona.rep
	Argument    : $LISTE
&TX

&TI Lec
���
    La fonction moyenne mobile
&CO
	ma(n, expr)
&TX
    n'existait que sous le nom
&CO
	mavg(, expr)
&TX
    suite � une erreur dans le manuel.

    Les deux sont dor�navant disponibles.

&TI Simulation
����������
    Correction : lorsqu'une liste d'�quations est introduite et que
    cette liste n'existe pas, un message d'erreur est produit. Auparavant,
    toutes les �quations �taient simul�es.

&TI Saisie de valeurs
�����������������
    Dans l'�dition manuelle des donn�es (WS de variables ou via
    tableaux), quelques modifications ont �t� apport�es pour am�liorer
    l'interface :

&EN les 8 chiffres significatifs sont affich�s
&EN si l'affichage de la valeur d�passe la taille de la cellule,
	  celle-ci est temporairement agrandie.
&EN lors de la saisie via tableau, les 8 chiffres significatifs
	  sont conserv�s

&TI Rappel
    ������
	    Il est inutile d'esp�rer que plus de 8 chiffres significatifs
	    soient conserv�s dans les valeurs des s�ries : les valeurs
	    "float" affichent n'importe quoi au-del� du huiti�me chiffre.
	    Ainsi, la valeur 0.012345678 exploite toute la pr�cision
	    disponible est introduire 0.012345678912 ne modifiera plus la
	    valeur machine du nombre.
>

<Version 4.52> (24/05/1994)
    Version 4.52 (24/05/1994)
    �������������������������

&TI Corrections
�����������
&EN Impressions des tableaux : correction de la s�paration entre les
      colonnes.
&EN Rapports : comportement bizarre en version 16 bits
&EN Estimation : si le nombre de degr�s de libert� est 0, les tests
	    sont corrig�s

&TI Trend Correction
����������������
    Impl�mentation de la m�thode Hodrick-Prescott pour la construction
    de s�rie de trend. Le principe est le m�me que pour la
    dessaisonnalisation : les s�ries lues dans un fichier sont import�es
    et transform�es simultan�ment.

    Une note technique de Bart Hertveldt d�crit la m�thode et son
    utilisation.

    Les fonctions de rapport ont la syntaxe suivante :
&CO
	$WsTrend VarFilename Lambda series1 series2 ...
	##WsTrend (interactive)
&TX
>

<Version 4.51> (15/04/1994)
    Version 4.51 (15/04/1994)
    �������������������������
&TI Impressions de tableaux
��������������������
    La s�paration entre les colonnes des tableaux en Postscript est
    ajust�e pour permettre de placer plus de colonnes sur une m�me page.

&TI Edition de tableaux
��������������������
    La derni�re ligne de l'�cran contient une indication sur les
    attributs de la cellule s�lectionn�e : cadrage et polices de
    caract�res.
>

<Version 4.50> (18/03/1994)
    Version 4.50 (18/03/1994)
    �������������������������

&TI Lec
���
    Les formules faisant intervenir des lags n�gatifs (leads), du
    type l(-2, A), dln(-2, X+Y), retournaient un r�sultat incorrect en
    raison des arrondis des nombres n�gatifs.

    Cette erreur est corrig�e et, par exemple, si A vaut 1, 2, 3, 4, ...,
    l(2, A) vaut 3, 4, ..., tandis que l(-2, A) vaut --, --, 1, 2, ...
>

<Version 4.49> (23/02/1994)
    Version 4.49 (23/02/1994)
    �������������������������

&TI Xode
����
    Correction d'un Bug : l'exportation en ASCII de tableaux dont
    certaines cellules �taient vides �taient incorrecte : ces cellules
    �taient "saut�es". Lors de la r�importation de tableaux, il en
    r�sultait un d�calage dans les colonnes.

    Typiquement, la premi�re ligne contenant la p�riode "##s" dans
    la deuxi�me colonne se voyait modifi�e par deux xode successifs :
    avant on avait une premi�re colonne vide, apr�s, la premi�re colonne
    contenait la p�riode :
&CO
	AVANT :        ""            "##s"
	APRES :        "##s"          ""
&TX

>

<Version 4.48> (16/02/1994)
    Version 4.48 (16/02/1994)
    �������������������������
&TI Impression de tableaux
���������
 Option fixed width characters pour les tableaux o� l'alignement
      est n�cessaire

&TI Lec
���
Les calculs incorporant des p�riodes dans des fonctions telles
      que sum() ou mean() �taient incorrects lorsque ces p�riodes
      �taient n�gatives (mean(-2, X)). Cette erreur est corrig�e.

&TI Simulation et extrapolation
���������������������������
 Deux options compl�mentaires permettent de prolonger une s�rie
      ou de choisir le point de d�part d'une simulation en prenant la
      valeur de l'ann�e pr�c�dente ou une extrapolation des deux ann�es
      pr�c�dentes uniquement si la valeur courante est NA.
>

<Version 4.47> (10/02/1994)
    Version 4.47 (10/02/1994)
    �������������������������

&TI View Tables and Graphics
������������������������
    Lorsque plusieurs tableaux doivent �tre affich�s (sous forme de
    graphique ou de tableau de nombres), un tableau d�roulant reprenant le
    nom de chaque tableau ainsi que son titre est propos�.

    Il suffit de presser ENTER sur le tableau souhait� pour afficher le
    r�sultat (tableau ou graphique).


&TI Edit Objects
������������
    Lors de l'�dition des objets, un sous-menu propose d'�diter
    tout le WS ou seulement une liste. De plus, F10 permet d'entrer
    directement en �dition de tout le WS.

    Dans le cas des listes, diff�rentes fonctions ont �t� regroup�es
    � ce niveau du menu et certaines se sont ajout�es :

&EN         Recherche de texte dans les objets
&EN         Tri des listes
&EN         Analyse (Scan) de WS
&EN         Stocker la liste des objets d'un fichier ou du WS
&EN         Effectuer des op�rations (intersection, union, diff�rence)
	  sur des listes
&EN         Calculer le nombre d'�l�ments d'une liste

    Dans le cas des variables, une nouvelle fonction permet la mise
    � jour des s�ries par le biais de tableaux. Si plusieurs tableaux sont
    demand�s, un menu avec les tableaux est affich�, et non plus tous les
    tableaux successivement.

    Les fonctions de rapports correspondantes sont :

&CO
	    - $DataListVar name pattern [file]
	    - $DataCalcLst res lst1 op lst2
		    o� op = {+,*,-}

	    - $ViewByTbl list
&TX

&TI Sample
������
    Les p�riodes d�finies dans les �crans DataEditGraph, IdtExecute et
    ModelSimulate sont adapt�es lors d'un changement du sample du WS ou
    lors d'un chargement de fichier de variables.

    Cette modification a lieu dans les cas suivants :

&EN         periodicit� WS != nouvelle periodicit�
&EN         si WS.from >> nouveau from
&EN         si WS.to   << nouveau to

&TI Graph sur base de s�ries
������������������������
&EN F8 dans le tableau d�roulant des variables affiche le graphique de
      la s�rie sur laquelle le curseur se trouve avec les propri�t�s du
      plus r�cent appel � la fonction DataEditGraph

&EN Shift-F8 dans le m�me tableau remplace l'ancien F8 et permet donc
      de grouper plusieurs s�ries sur le m�me graphique. De plus, cette
      fonction permet de modifier les propri�t�s (Sample, type, ...) des
      futurs graphiques affich�s � l'aide de F8.

&TI Ex�cution directe d'identit�s
����������������������������
&EN F7 dans le tableau d�roulant des identit�s ex�cute l'identit�
      courante sur toute la p�riode du WS

&EN Shift-F7 lance la fonction IdtExecute

&TI Xode bug (HP)
�������������
&EN xode -i *.eqs fonctionne
&EN impression en PostScript fonctionne � nouveau

&TI DataCompare
�����������
    Le contenu du WS courant peut �tre compar� � celui d'un fichier.
    Le r�sultat de cette comparaison est compos� de 4 listes :

&EN         objets trouv�s dans le WS seulement
&EN         objets trouv�s dans le fichier seulement
&EN         objets trouv�s dans les deux, avec la m�me d�finition
&EN         objets trouv�s dans les deux, avec une d�finition diff�rente

    Dans un rapport :
&CO
    $DataCompareXxx file ONE TWO THREE FOR

	  ONE         in WS only
	  TWO         in file only
	  THREE       in both, equal
	  FOR         in both, different
&TX

&TI Estimate
��������
    Une fonction de construction automatique d'ajustements dynamiques
    est ajout�e dans le panneau de d�finition d'�quation.
    Deux m�thodes sont disponibles :

    Partial Adjustment (1 coefficient c1):
&CO
	lhs := rhs ->> d(lhs) := c1 * (rhs -(lhs)[-1])
&TX
    Error Correction Model (deux coefficients c1 et c2):
&CO
	lhs := rhs ->> d(lhs) := c1 * d(rhs) + c2 * (rhs-lhs)[-1]
&TX
&TI Load et Save de WS
������������������
    Lors de la lecture et de l'�criture des WS sur disque, des
    v�rifications compl�mentaires sont effectu�es pour s'assurer de la
    bonne fin de l'op�ration (notamment de le cas de timeout sur
    r�seau).

&TI Object Names
������������
    Les noms des objets sont v�rifi�s lors de leur cr�ation et ne
    peuvent contenir que des lettres, chiffres ou le caract�re _.
    En outre, le premier caract�re ne peut �tre un chiffre.

&TI Qode
����
    Nouvelle option -fw pour l'impression des tableaux a2m. Cette option
    remplace les caract�res proportionnels standards par des non
    proportionnels. Le but est de conserver l'alignement dans
    les tableaux contenant des blancs.

&TI Impression de tableaux
���������
    Bug solutionn� : l'impression avec un sample erronn� donne un
      message d'erreur.

&TI Seasonal Adjustment
�������������������
    Une message d'erreur est affich� si une variable n'existe pas dans
    le fichier indiqu�.

&TI Bug connu
���������
    Dans la seule version 16 bits, le remplacement r�cursif des listes
    par leur valeur peut cr�er une erreur si plus de 3 niveaux
    d'imbrication sont n�cessaires.
&CO
	A = $B
	B = $C
	C = $D
&TX
    L'utilisation de $A peut entra�ner une erreur.
>

<Version 4.45-4.46> (05/01/1994)
    Version 4.45-4.46 (05/01/1994)
    ������������������������������

&TI Gode
����
    L'impression de graphiques dans IODE32 restait dans le programme
    GODE32. Il fallait donc quitter le programme GODE (a-X) pour revenir
    � IODE32. Cette erreur est � pr�sent corrig�e.

&TI Reports
�������
    La macro %0% est le nombre d'arguments du rapport.

&TI Simulation
����������
    La simulation peut �tre interrompu par l'utilisateur.

&TI Xode
����
    Nouveau format d'importation : PRN d'AREMOS : le fichier
    des variables se pr�sente comme suit :
&CO
	"VARNAME" val1 val2 ....
	...
&TX

>

<Version 4.44> (04/01/1994)
    Version 4.44 (04/01/1994)
    �������������������������

&TI Dessaisonalisation
������������������
    Cette fonction est op�rationnelle pour les s�ries comprenant des
    nombres n�gatifs. (corrig�e)

&TI Installation
������������
    Nouveau programme d'installation, guidant l'utilisateur, notamment
    en indiquant l'espace disponible sur chaque disque, en modifiant
    le fichier autoexec.bat et en permettant d'adapter le fichier
    printcap de d�finition des imprimantes.

&TI Rapport
�������
    La macro %0% est le nombre d'arguments du rapport.


&TI Xode
����
    Nouveaux formats import�s (variables et commentaires) : fichiers de
    type PRN g�n�r�s par AREMOS.

&TI Simulation
����������
    Lorsqu'une �quation n'est pas d�finie explicitement par rapport �
    son endog�ne ou lorsque celle-ci appara�t plusieurs fois dans
    l'�quation, l'algorithme de simulation essaie de r�soudre cette
    �quation par une m�thode de Newton. Si cette m�thode ne fournit pas
    de r�sultat, une m�thode de type s�cante recherche � son tour une
    solution pour l'�quation.

    Il n'est cependant pas garanti qu'une solution soit obtenue dans
    tous les cas. Les fonctions non continues (singularit�s) comme
&CO
	X := a + b * 1 / (X + 1)
&TX
    peuvent poser des probl�mes en passant par la valeur -1.
    En cas de probl�me, la seule solution actuelle est de modifier la
    forme de l'�quation :
&CO
	(X - a) * (X + 1) := b
&TX
>

