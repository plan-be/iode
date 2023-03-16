/* REP0 */

<D‚finition>
    D‚finition d'un rapport
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Un rapport est un texte ASCII r‚dig‚ par l'utilisateur et comprenant un
ensemble d'instructions, de macros et de texte libre, dont
l'interpr‚tation et l'ex‚cution se traduisent d'une part par
l'enchaŒnement d'op‚rations (charger un fichier, estimer, simuler, ...)
et d'autre part par l'impression d'un document "fini" au format A2M
(voir le programme TODE) contenant le r‚sultat des op‚rations
ex‚cut‚es. Le rapport est imprim‚ sur l'imprimante s‚lectionn‚e (un
fichier a2m en g‚n‚ral).

Le langage a2m est un langage de description de document qui sera
interpr‚t‚ par un programme annexe (Tode et Qode). Le r‚sultat de
l'interpr‚tation sera un document sur papier ou un fichier lisible par
un programme de traitement de texte (Word, Frame, etc).
>
<La structure des rapports>
    La structure des rapports
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Un rapport est un texte ASCII qui peut ˆtre ‚dit‚ avec n'importe quel
‚diteur ASCII. Les ‚diteurs MT et MMT seront utilis‚s de pr‚f‚rence pour
leur simplicit‚ et leur convivialit‚. MMT fait partie int‚grante de IODE.

Le texte du rapport contient les types d'informations suivants:

&EN du texte libre: titres, commentaires, etc
&EN des commandes A2M (formatage du texte)
&EN les commandes d'ex‚cution des rapports
&EN les macros
&EN les expressions LEC
&EN les fonctions de rapports
&EN des commandes IODE

Les commandes RAPPORT et les commandes IODE peuvent ˆtre des commandes
"plein ‚cran" ou des lignes de commande (sans interface ‚cran).

&TI Commande "plein ‚cran"
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Les commandes "plein ‚cran" sont constitu‚es d'un mot cl‚ pr‚c‚d‚ par le
caractŠre ## et ont la syntaxe suivante:
&CO
    ##commande
&TX
Par exemple : la commande
&CO
    ##WsLoadVar ou ##WsLoad
&TX
provoquera l'affichage d'un ‚cran de saisie permettant de pr‚ciser le
nom du fichier de variables … charger.

&TI Commande "en ligne"
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Les commandes en ligne sont constitu‚es d'un mot cl‚ pr‚c‚d‚ par le
caractŠre $ et ont la syntaxe suivante:
&CO
    $commande [options] [paramŠtres]
&TX
Par exemple, la commande
&CO
    $PrintTbl 84:8 TEST
&TX
imprimera le tableau TEST pour le sample indiqu‚.

Le mot cl‚ des commandes en ligne comprend souvent deux parties (c'est
le cas de toutes les commandes en ligne qui manipulent des objets) : la
commande proprement dite (en g‚n‚ral identique … la commande plein
‚cran) et un suffixe qualifiant l'objet manipul‚. Par exemple … la
commande plein ‚cran ##WsLoad correspondent les commandes en ligne
$WsLoadVar, $WsLoadIdt, etc... respectivement pour les objets variable
et identit‚. Les suffixes valides sont les suivants (tous ne sont pas
valables pour toutes les fonctions) :

#include "ext.m1"

&TI RŠgles syntaxiques
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Le caractŠre $ ou ## indiquant une commande DOIT commencer la ligne et
ˆtre suivi DIRECTEMENT par le mot cl‚ de la commande (s'il s'agit
d'une commande … ex‚cuter). Un caractŠre $ ou ## situ‚ plus loin dans la
ligne sera imprim‚ tel quel. Il ne peut donc y avoir qu'une commande par
ligne de rapport.

Le mot cl‚ peut ˆtre ‚crit indiff‚remment en minuscules, en majuscules
ou les deux. Par exemple :

&CO
    WsLoadVar == wsloadvar == WSLOADVAR
&TX

La liste des mots cl‚s est reprise plus loin dans le texte.

Les commandes dont le r‚sultat (erreur ou nom) doit ˆtre ignor‚ sont
indiqu‚es avec le signe - entre le $ (ou ##) et le mot cl‚. Par
exemple, si la commande

&CO
    $-WsLoadTbl
&TX

n'aboutit pas, son r‚sultat sera ignor‚ (aucune action ne sera ex‚cut‚e
quelle que soit la commande de $OnError).

Les commandes dont le signe $ ou ## est r‚p‚t‚ deux fois ($$ ou ####)
seront imprim‚es dans le rapport sans ˆtre ex‚cut‚es.

&TI Commentaires
ÄÄÄÄÄÄÄÄÄÄÄÄ

Les lignes dont le signe $ ou ## est suivi par un espace sont des lignes
de commentaires, non imprim‚es et non ex‚cut‚es.

&TI Imbrications
ÄÄÄÄÄÄÄÄÄÄÄÄ

Les rapports peuvent ˆtre imbriqu‚s (nombre non limit‚ d'imbrications),
c'est-…-dire qu'un rapport peut en ex‚cuter un autre.

&TI R‚sultat de l'impression
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Le rapport est "imprim‚" dans le fichier a2m s‚lectionn‚ (en ajout au
fichier existant s'il existe). La commande $PrintDest permet de sp‚cifier
le nom de ce fichier.

&TI Exemple
ÄÄÄÄÄÄÄ

Le rapport suivant charge un fichier de d‚finition de tableaux et un
fichier de s‚ries, affiche un message, construit les tableaux, les
imprime et termine l'ex‚cution par un beep sonore.

&CO
    $wsloadvar bist92\\bistel
    $wsloadtbl bist92\\tbistelf

    $printdest bist92\\bistelf1.a2m
    ##show processing french tables  file 1/2
    $printnbdec 1
    $PrintTbl 89:8 HYPEIR
    $PrintTbl 89/88:8 HYPEIIR

    $printnbdec 0
    $PrintTbl 89:8 RESL00

    $printnbdec 1
    $PrintTbl 89/88:8 RESL00R
    $PrintTbl 89:8 RESL03
    ##beep
&TX

Le rapport produit contient ‚galement les ‚ventuels messages d'erreur.
>

<Les commandes d'ex‚cution des rapports>
    Les commandes d'ex‚cution des rapports
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Ces commandes sont indiff‚remment pr‚c‚d‚es par les caractŠres $ ou ##,
elles contr“lent le d‚roulement de l'interpr‚tation du rapport.

Ces commandes (mots cl‚s) sont les suivantes:
&EN <$abort>    : interruption du rapport (courant et niveaux sup‚rieurs)
&EN <$ask>      : question et branchement conditionnel vers un label
&EN <$beep>     : production d'un beep sonore
&EN <$chdir>    : change le directory courant
&EN <$debug>    : indique le fichier et la ligne en cours d'ex‚cution
&EN <$define>   : d‚finition d'une macro
&EN <$foreach>  : boucle sur une liste de valeurs
&EN <$goto>     : branchement (in)conditionnel vers un label
&EN <$incrtime> : incr‚mente la variable t (par d‚faut de 1) pour le calcul
		des formules LEC d‚finies dans le rapport
&EN <$indent>   : indentation des instructions accept‚es ou non
&EN <$label>    : localisation d'un point de branchement
&EN <$maximize> : restaure la fenˆtre de IODE
&EN <$minimize> : minimise la fenˆtre de IODE
&EN <$mkdir>    : cr‚e un nouveau directory
&EN <$multiline>: permet ou non l'‚criture d'une commande sur plusieurs lignes
&EN <$msg>      : affichage d'un message et attente d'un touche
&EN <$next>     : fin de boucle
&EN <$noparsing>: active ou d‚sactive le parsing des textes entre backquotes
&EN <$procdef>  : d‚finition d'une proc‚dure
&EN <$procend>  : fin de d‚finition d'une proc‚dure
&EN <$procexec> : ex‚cution d'une proc‚dure
&EN <$onerror>  : action … mener en cas d'erreur
&EN <$prompt>   : d‚finition d'une macro par question … l'utilisateur
&EN <$quit>     : quitte IODE
&EN <$quitode>  : quitte IODE
&EN <$repeat>   : boucle sur les arguments
&EN <$repeatstring> : d‚finition des caractŠres … remplacer dans $repeat
&EN <$return>   : quitte le rapport courant
&EN <$rmdir>    : d‚truit un directory vide
&EN <$settime>  : definition de la variable t pour le calcul des formules LEC
		  d‚finies dans le rapport
&EN <$shift>    : d‚cale les arguments du rapport d'un vers la gauche
&EN <$show>     : affichage d'un message dans le bas de l'‚cran
&EN <$sleep>    : stoppe le processus pendant un courte p‚riode
&EN <$system>   : ex‚cution d'une commande systŠme
&EN <$vseps>    : red‚finit les s‚parateurs utilis‚s par les fonctions
		~c@vdrop(),~C ~c@vtake()~C et ~c@vcount().~C
>

<$minimize>
    Commande $MINIMIZE
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette commande minimise la fenˆtre de IODE (par exemple pour afficher
des graphiques de Excel g‚n‚r‚s par la simulation en cours).

&SY2
    $Minimize
&TX
>

<$maximize>
    Commande $MAXIMIZE
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette commande restaure la fenˆtre de IODE.

&SY2
    $Maximize
&TX
>
<$sleep>
    Commande $SLEEP
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Arrˆte le processus pendant nn milliŠmes de seconde. Cela permet … un
client ou un serveur DDE de conserver le contr“le en cas de requˆtes
trop rapides.

&SY2
    $Sleep nn

	o— nn = nombre de millisecondes d'arrˆt
&TX
>

<$abort>
    Commande $ABORT
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette instruction indique que l'ex‚cution du rapport doit ˆtre
interrompue. Si le rapport courant est un sous-rapport, les rapports
de niveau sup‚rieur doivent ‚galement ˆtre quitt‚s.

&SY2
    $Abort
&TX

&EX2
    ...
    $GoTo continue {$DataExistVar A}
    $Show Variable A does not exist
    $Abort
    ....
    $Label continue
    ...
&TX
>

<$quit>
    Commande $QUIT
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette instruction indique que l'ex‚cution du rapport doit ˆtre
interrompue et que le programme IODE doit ˆtre quitt‚.

&SY2
    $Quit
&TX

&EX2
    $WsLoadVar refsim
    $ModelSimulate 1997Y1 1998Y1
    $WsSaveVar myws
    $Quit
&TX

Cette fonction est notamment utile lorsqu'un rapport est lanc‚ … partir
d'un fichier de commandes (DOS ou Unix). En permettant de quitter
IODE, elle rend la main au fichier de commandes qui peut enchaŒner
avec un autre programme (impression, autre simulation, etc).
>

<$quitode>
    Commande $QUITODE
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Voir <$quit>.
>

<$return>
    Commande $RETURN
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette instruction indique que l'ex‚cution du rapport courant est
termin‚e. S'il s'agit d'un sous-rapport, le rapport appelant se poursuit.

&SY2
    $Return
&TX

&EX2
    Rapport createvar.rep
    ---------------------
    ## Create series %1%
    $GoTo exist {$DataExistVar %1%}
    $DataCalcVar %1% 0.9
    $Show Variable %1% created
    $Return
    $Label exist
    $Show Variable %1% exists

    Appel
    -----
    $ExecRep createvar A
&TX
>

<$shift>
    Commande $SHIFT
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette fonction d‚cale les arguments du rapport d'un ‚l‚ment
vers la gauche. Par la mˆme occasion, le premier argument est
perdu.

Cette fonctionnalit‚ permet d'ex‚cuter des op‚rations sur un
nombre d'‚l‚ments inconnus. En effet, comme %0% repr‚sente le nombre
d'argument du rapport, aprŠs $shift, cette valeur est d‚cr‚ment‚e.
De la sorte, un simple goto calcul‚ comme dans l'exemple ci-dessous,
permet de quitter la boucle dŠs que %0% est nul.

&EX2
    $label again
    $goto fin {%0%=0}
    ##show arg0 : %1%
    $DataCalcVar %1% if(%1%=0, 1/0, %1%)
    $shift
    $goto again
    $label fin
&TX
>

<$label>
    Commande $LABEL
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette instruction indique et nomme un point de branchement dans un rapport.
Ce point de branchement pourra par la suite ˆtre atteint par une
instruction comme $goto ou $ask.

&EX2
   $label again
   $goto fin {%0%=0}
   ##show arg0 : %1%
   $DataCalcVar %1% if(%1%=0, 1/0, %1%)
   $shift
   $goto again
   $label fin
&TX

Dans cet exemple, on remplace tous les 0 par des NA pour la liste de
variables (inconnue au d‚part) pass‚es comme paramŠtre au rapport.
Pour m‚moire, %0% est le nombre d'arguments, %1% le premier argument,
etc.
>

<$goto>
    Commande $goto
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette commande provoque un branchement vers un label dans le rapport
courant.

&SY2
    $GoTo label [{condition}]
    label := point de branchement indiqu‚ par la commande Label
    condition := expression LEC ou commande de rapport
&TX

Si le label n'existe pas, l'ex‚cution du rapport (et de tous les rapports
de niveaux sup‚rieurs) est interrompue.

&EX2
    ....
    $Label boucle
    ...
    $GoTo boucle
    ...
&TX

Il est ‚galement possible de construire certains tests pour diriger
la suite de l'ex‚cution du rapport.

Si on place une condition aprŠs le nom du label, celle-ci est ex‚cut‚e
en premier lieu. Le condition peut ˆtre soit une commande IODE (par
exemple $DataExistVar) ou une formule LEC (par exemple "t != 1993Y1").
La commande doit ˆtre plac‚e entre accollades.

Le Goto a lieu si la commande IODE peut ˆtre valablement ex‚cut‚e ou si
le r‚sultat de la formule LEC est non nul. Pour diff‚rencier commande
IODE et formule LEC, la commande doit commencer (comme toujours) par le
caractŠre $.

&EX2
    ...
    $GoTo exist {$DataExistVar A}
    $Show Variable A does not exist
    $Return
    ....
    $Label exist
    $Show Variable A exists
    $Return
    ...

ou
    ...
    $GoTo end {X + Y >> 1000}
    ...
    $Return
    ....
    $Label end
    $Show X + Y exceeds the limit of 1000
    $Return
    ...
&TX


Il est ‚galement possible d'inverser le r‚sultat d'une commande IODE,
exactement comme celui d'une formule LEC. Il suffit pour cela de placer
un point d'exclamation (!) aprŠs le $. L'exemple qui suit indique
comment proc‚der :

&EX2
    ...
    $GoTo notexist {$!DataExistVar A}
    $Show Variable A does exist
    $Return
    ....
    $Label notexist
    $Show Variable A does not exist
    $Return
    ...
&TX

Les formules LEC peuvent quant … elles ˆtre ais‚ment adapt‚es pour
retourner la valeur souhait‚e (op‚rateur !).
>

<$repeat>
    Commande $REPEAT
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ


Cette fonction permet d'ex‚cuter une commande sur une liste d'arguments
sans devoir cr‚er un rapport … cette fin. La position de l'argument dans
la commande … r‚p‚ter est repr‚sent‚e par le string d‚fini par $repeatstring.

Par d‚faut, le caractŠre _ est le caractŠre de remplacement.

&SY2
    $repeat commande arg1 arg2 arg3
&TX

~ccommande~C est ex‚cut‚e pour chaque argument en rempla‡ant ~c_~C par ~carg1~C,
puis ~carg2~C, ...

&EX2

1. Tri de plusieurs listes

&CO
    $RepeatString ++
    $Repeat "$DataListSort ++ ++" A B C
&TX
    ‚quivaut aux trois commandes
&CO
    $DataListSort A A
    $DataListSort B B
    $DataListSort C C
&TX

2. Duplicate Vars

Avec les fonctions @fn(), on peut copier toutes les variables d'un WS en
une seule op‚ration :

&CO
    $Repeat "$DataDuplicateVar _ _1" @vexpand(*)
&TX
>

<$repeatstring>
    Commande $REPEATSTRING
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette fonction permet de pr‚ciser les caractŠres … remplacer dans le
premier argument de $repeat. Par d‚faut, ce string est compos‚ du seul
caractŠre de soulignement (~c_~C).

&SY2
    $RepeatString string
&TX

&EX2
    $RepeatString --
    $Repeat "$DataListSort -- --" A B C
&TX
&SA $Repeat
>

<$vseps>

&DE
Cette commande red‚finit les s‚parateurs utilis‚s par les fonctions
~c@vdrop(),~C ~c@vtake()~C et ~c@vcount()~C ainsi que dans ~c$foreach~C et
~c$next~C.

&SY2
   $vseps {seps}
&TX

&EX2
   $vseps ;|
&TX

Voir aussi @vtake().
>

<$foreach>
    Commande $FOREACH
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet de simplifier l'‚criture de boucles ou de boucles
imbriqu‚es.

La commande ~c$foreach~C permet de sp‚cifier un index et la liste de valeurs
que cet index doit successivement prendre.

La commande ~c$next~C permet de revenir au point de d‚part de la boucle
~c$foreach~C et de passer … la valeur suivante de l'index.

&SY2
    $foreach {index} {values}
    ...
    $next {index}
&TX

o—
&EN ~c{index}~C est un nom de macro de maximum 10 caractŠres (par exemple ~ci, idx, PAYS~C , ...)
&EN ~c{values}~C est une liste de valeurs s‚par‚es par des virgules, blancs ou
point-virgules. Les s‚parateurs peuvent ˆtre modifi‚s par la commande ~c$vseps~C

Exemple 1 : boucles imbriqu‚es
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
>

<$next>
    Commande $NEXT
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ
Voir <$foreach>.
>


<$procdef>
    Commande $PROCDEF
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande indique le d‚but de la d‚finition d'une proc‚dure IODE. Elle
est li‚e aux commandes ~c$procdef, $procend et $procexec~C.

Les instructions de ce groupe permettent de construire des proc‚dures, c'est … dire des
listes de commandes qui peuvent ˆtre r‚utilis‚es et param‚tr‚es.

&CO
 $procdef procname [fparm1 ...]
 ..
 $procend
&TX

o—
&EN ~bprocname~B est le nom de la proc‚dure (case sensitive).
&EN ~bfparm1~B est le premier paramŠtre ~iformel~I de la proc‚dure

&IT Appel d'une proc‚dure
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
L'appel se fait simplement par la commande :

&CO
$procexec nom_proc aparm1 ...
&TX

o—
&EN ~bprocname~B est le nom de la proc‚dure (case sensitive).
&EN ~baparm1~B est le premier paramŠtre ~iactuel~I de la proc‚dure

&IT ParamŠtres
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Les paramŠtres formels sont trait‚s dans la proc‚dure comme des $~cdefines~C
(locaux … la proc‚dure) : ils doivent ˆtre appel‚s par ~c%fparm%~C.

Leurs valeurs sont fix‚es comme suit :

&EN S'il y a moins de paramŠtres actuels que de paramŠtres formels ou si
leur nombre est ‚gal, les valeurs des paramŠtres actuels sont assign‚es dans
l'ordre aux premiers paramŠtres formels. Les paramŠtres formels
exc‚dentaires sont consid‚r‚s comme vides.

&EN S'il y a plus de paramŠtres actuels que de paramŠtres formels, les
paramŠtres formels, ~bsauf le dernier~B, re‡oivent les valeurs de premiers
paramŠtres actuels, dans l'ordre de leur passage.
~bLe dernier paramŠtres formel re‡oit la valeur de tous
les paramŠtres actuels restants~B.

Exemple avec plus de paramŠtres actuels de formels :
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&CO
$indent
$procdef print list
    $foreach i %list%
	$show print : %i%
    $next i
$procend
$procexec print A B C
&TX

R‚sultat :
&CO
print : A
print : B
print : C
&TX

On constate que le paramŠtre formel ~clist~C re‡oit toutes les valeurs
pass‚es … la proc‚dure. La boucle itŠre donc 3 fois.

Exemple avec moins de paramŠtres actuels de formels :
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
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

R‚sultat :
&CO
Mon Titre
&TX
Cette fois, le premier paramŠtre ~ctitre~C contient ~c"Mon Titre"~C qui est
imprim‚ avant la boucle. Par contre, la boucle ne s'ex‚cute pas car le
paramŠtre ~clist~C est vide.

&IT Port‚e de la d‚finition d'une proc‚dure
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
1. Les proc‚dures doivent ˆtre d‚finies avant de pouvoir ˆtre appel‚es.

2. Une fois d‚finie, une proc‚dure reste appelable au sein de la mˆme session de IODE, mˆme
si le rapport qui l'a d‚finie est termin‚. On peut ex‚cuter un rapport qui n'a
d'autre effet que de charger des proc‚dures en m‚moire. Ces proc‚dures resteront disponibles
pendant toutes la session IODE.

3. Une proc‚dure peut ˆtre remplac‚e par une autre du mˆme nom … tout moment.

&IT Port‚e des paramŠtres formels
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Les paramŠtres formels sont trait‚s comme des defines dont la port‚e est
limit‚e … la proc‚dure courante.

Par cons‚quent, si un define existe ~uavant l'appelU de la proc‚dure avec le mˆme nom qu'un
des paramŠtres, ce define ne peut ˆtre utilis‚ au sein de la proc‚dure.
AprŠs la proc‚dure, il reprend sa valeur ant‚rieure.

Exemple :
&CO
$define titre Quotients de mortalit‚
$show Avant la proc : %titre%

$procdef print titre list
    $show Pendant la proc :%titre%
    $foreach i %list%
	$show print : %i%
    $next i
$procend

$procexec print "Mon Titre"
$show AprŠs la proc :%titre%
&TX

R‚sultat :
&CO
Avant la proc : Quotients de mortalit‚
Pendant la proc :Mon Titre
AprŠs la proc :Quotients de mortalit‚
&TX

>
<$procend>
    Commande $PROCEND
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande indique la fin de d‚finition d'une proc‚dure.
Voir <$procdef> pour plus de d‚tail.
>
<$procexec>
    Commande $PROCEXEC
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet l'ex‚cution d'une proc‚dure.
Voir <$procdef> pour plus de d‚tail.
>
<$indent>
    Commande $INDENT
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Par d‚faut, les commandes (~c$commande~C ou ~c##commande~C) doivent ˆtre coll‚es … la marge.
Si ce n'est pas le cas, elles sont consid‚r‚es comme du texte simple.

Pour pouvoir indenter les commandes dans les rapports, il faut l'indiquer par la
commande ~c$indent~C. Sans argument, elle indique qu'… partir de ce moment, les
commandes ne doivent plus ˆtre coll‚es … la marge. Avec l'argument N ou n ou
0, les commandes doivent ˆtre coll‚es … la marge.


Pour ‚viter les problŠmes de compatibilit‚ avec les rapports cr‚es dans
les versions anciennes de IODE, la valeur par d‚faut est de ne pas accepter les
indentations.

&CO
   $indent [{Nn0}]
&TX

Par exemple, on peut ‚crire :

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
Le r‚sultat produit est le suivant  :

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
>

<$multiline>
    Commande $MULTILINE
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Par d‚faut, une ligne de rapport peut se prolonger sur la ligne suivante si
elle se termine par un espace suivi d'un backslash (\). Si c'est la cas,
le caractŠre de retour … la ligne est ins‚r‚ entre les lignes (\n).

Cela permet par exemple de construire dans un rapport des ‚quations qui se d‚composent
sur plusieurs lignes, rendant leur lecture plus simple.

Pour ne pas permettre cette extension (compatilibili‚ avec les versions
ant‚rieures) il suffit de placer la commande ~c$multiline 0~C avant les
lignes concern‚es. Sans argument, le multi-lignes est accept‚.

&CO
   $multiline [{Nn0}]
&TX

Par exemple, on peut dor‚navant ‚crire :

&CO
    $multiline 1
    $DataUpdateEqs A A := c1 + \
    c2 * t
&TX
La d‚finition de l'‚quation A sera alors sur deux lignes.

A l'inverse, sans le multiline, une erreur est g‚n‚r‚e. Ainsi
&CO
    $multiline 0
    $DataUpdateEqs A A := c1 + \
    c2 * t
&TX
essaie de cr‚er une ‚quation termin‚e par + \, ce qui donne le r‚sultat suivant :

&CO
    essais.rep[23] - $multiline 0
    essais.rep[24] - $DataUpdateEqs A A := c1 + \
    (string)[7]:syntax error
    Error : dataupdateeqs A A := c1 + \
&TX
>
<$noparsing>
    Commande $noparsing
    ÄÄÄÄÄÄÄÄÄÄ
Par defaut, les textes entre backquotes (`) ne sont pas pars‚s dans les rapports.
La commande $noparsing permet de modifier ce comportement.

&CO
    $noparsing 0 indique que le parsing doit toujours avoir lieu, mˆme entre les backquotes
    $noparsing 1 indique que le parsing ne doit pas avoir lieu entre les backquotes.
&TX

&EX
     $define C  1
     $noparsing 1
     Example : `{1 + %@upper(c)%}` ->> {1 + %@upper(c)%}
     $noparsing 0
     Example : `{1 + %@upper(c)%}` ->> {1 + %@upper(c)%}
&TX
donne 2 r‚sultats diff‚rents :
&CO
     Example : {1 + %@upper(c)%} ->> 2
     Example : `2` ->> 2
&TX
>



<$ask>
    Commande $ASK
    ÄÄÄÄÄÄÄÄÄÄÄÄ

Cette commande provoque l'affichage d'une question dont la r‚ponse peut
ˆtre Yes ou No. Si la r‚ponse est non, le rapport continue … la ligne
suivante. Si la r‚ponse est oui, l'ex‚cution du rapport se poursuit …
partir du label indiqu‚. Si le label n'existe pas, l'ex‚cution du
rapport (et de tous les rapports de niveaux sup‚rieurs) est interrompue.

La question et le label sont s‚par‚s par une virgule ou un point-virgule.

&SY2
    $Ask label,question

    o— label    := point de branchement indiqu‚ par la commande Label
       question := texte quelconque
&TX

&EX2
    ....
    $Label boucle
    ...
    $Ask boucle,retour … boucle ?
    ...
&TX
>

<$onerror>
    Commande $ONERROR
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette commande indique l'action … mener en cas d'erreur rencontr‚e dans
une commande IODE, si cette erreur ne doit pas ˆtre ignor‚e (signe -
aprŠs le $ ou le ##).

Les actions peuvent ˆtre: RETURN, ABORT, QUIT, DISPLAY (par d‚faut),
PRINT, NOPRINT, et IGNORE (par d‚faut).

L'action IGNORE est l'action par d‚faut: ignore l'erreur et poursuit
l'ex‚cution du rapport.

L'action RETURN provoque l'interruption du rapport courant, avec retour
au niveau sup‚rieur ‚ventuel.

L'action ABORT provoque l'interruption du rapport courant et de tous les
rapports de niveaux sup‚rieurs.

L'action QUIT termine le programme IODE. Elle est utile notamment en
combinaison avec l'option -rep du programme iode.

L'action DISPLAY est active par d‚faut, elle provoque l'impression dans
le rapport d'un message d'erreur correspondant … l'erreur rencontr‚e
lors de l'ex‚cution de la derniŠre commande IODE. Elle signale ‚galement
… l'‚cran tous les messages d'erreur correspondant. L'action PRINT se
contente d'imprimer les messages d'erreur. L'action NOPRINT interdit
l'impression du message.

Les actions RETURN, ABORT, QUIT et IGNORE sont exclusives entre elles,
l'action ex‚cut‚e est celle qui correspond … la derniŠre commande
ONERROR rencontr‚e. Les actions DISPLAY, PRINT et NOPRINT sont ‚galement
exclusives mais peuvent ˆtre combin‚es avec une des quatre instructions
ci-dessus.

Une seule action peut figurer par ligne de commande.

&SY2
    $OnError action
    action := RETURN, ABORT, QUIT, IGNORE, DISPLAY, PRINT ou NOPRINT
&TX

&EX2
    $OnError noprint
    $OnError ignore
    ...
    $OnError noprint
    $OnErrot abort
    ....
&TX
>

<$show>
    Commande $SHOW
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette commande provoque un beep sonore et l'affichage d'un message dans
la fenˆtre de commentaire du programme IODE pendant l'ex‚cution du
rapport.

&SY2
    $Show message
    message := texte quelconque
&TX
>

<$msg>
    Commande $MSG
    ÄÄÄÄÄÄÄÄÄÄÄÄ

Cette commande affiche le texte de l'argument et attend une touche avant
de continuer. Cela permet de stopper momentan‚ment le rapport pour
afficher un message … l'intention de l'utilisateur, contrairement … la
fonction $show qui affiche dans le bas de l'‚cran mais ne stoppe pas.

&SY2
    $Msg message
    message := texte quelconque
&TX
>

<$debug>
    Commande $DEBUG
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette commande affiche dans la ligne de commentaire de l'‚cran le nom du
rapport en cours et la ligne courante. Elle permet de la sorte de
connaŒtre ais‚ment une ligne litigieuse en cas d'erreur par exemple.

&SY2
    $debug {Yes|No}
&TX
>

<$beep>
    Commande $BEEP
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette commande provoque un beep sonore pendant l'ex‚cution du rapport.

&SY2
    $Beep
&TX
>

<$system>
    Commande $SYSTEM
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette commande de rapport est utilis‚e pour ex‚cuter une commande
systŠme. La commande ‚tant ex‚cut‚e, le rapport se poursuit … la ligne
suivante.

La commande systŠme (suivi de ses ‚ventuels paramŠtres) est pass‚e en
paramŠtre. Plusieurs commandes peuvent d'enchaŒner, elles doivent ˆtre
s‚par‚es par un point-virgule (;).

Avant l'ex‚cution de la commande systŠme, le terminal est remis en mode
standard et l'‚cran est vid‚. AprŠs l'ex‚cution de la commande, l'‚cran
est r‚affich‚ dans son ‚tat initial.

&IT Attention

&EN aucune v‚rification n'est faite quant … la validit‚ ou l'opportunit‚
    de la commande systŠme. Une commande comme "del *.*" ou "format c:"
    est accept‚e...

&EN l'espace m‚moire est limit‚ pour l'ex‚cution de la commande systŠme
    car IODE reste charg‚ en m‚moire (version DOS)

&EN il peut ˆtre int‚ressant de terminer la liste des commandes sytŠme …
    ex‚cuter par la commande PAUSE (en DOS) de fa‡on … maintenir
    momentan‚ment le terminal en mode standard (visualisation des
    r‚sultats de la commande systŠme).

&SY2
    $System commande1 [;commande2;...]
    commande := commande valide du systŠme d'exploitation [+ arguments]
&TX

&EX2
    $System qode -d qms result.a2m
&TX
>

<$chdir>
    Commande $CHDIR
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette commande de rapport change le r‚pertoire courant.

&EN les fichiers sont toujours charg‚s relativement au r‚pertoire courant. A la fin
    du rapport, le r‚pertoire pr‚c‚dent n'est PAS restaur‚.

&SY2
    $chdir dirname
    dirname := nom de r‚pertoire (relatif ou absolu)
&TX

&EX2
    $chdir ..
    $chdir c:\usr\iode
&TX
>

<$mkdir>
    Commande $MKDIR
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette commande de rapport cr‚e un nouveau r‚pertoire.

&SY2
    $mkdir dirname
    dirname := nom de r‚pertoire (relatif ou absolu)
&TX

&EX2
    $mkdir ..\test
    $mkdir c:\usr\iode\test
&TX
>

<$rmdir>
    Commande $RMDIR
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette commande de rapport d‚truit un directory. Ce directory doir ˆtre vide.

&SY2
    $rmdir dirname
    dirname := nom de r‚pertoire (relatif ou absolu)
&TX

&EX2
    $rmdir ..\test
    $rmdir c:\usr\iode\test
&TX
>

<$define>
    Commande $DEFINE
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet de d‚finir des macros qui pourront ˆtre utilis‚es
plus tard dans le rapport et ses sous-rapports. Une macro est une sorte
de variable globale qui reste d‚finie durant toute la dur‚e d'ex‚cution
du rapport courant.

&SY2
    $Define name text
    name := nom utilis‚ pour faire r‚f‚rence au contenu (texte) de la
	    macro (le nom doit commencer par une lettre et ne pas
	    d‚passer 10 caractŠres)


&TX

Le nom est r‚f‚renc‚ dans un rapport en l'encadrant de signes %.

&EX2
    $Define a Hello World!
    $Show %a%
&TX

&EX2
    $Define VAR A
    $GoTo exist {$DataExistVar %VAR%}
    $Show Variable %VAR% does not exist
    $Return
    ....
    $Label exist
    $Show Variable %VAR% exists
    $Return
    ...
&TX

Si un rapport possŠde des arguments, ceux-ci sont utilisables comme des
macros dont le nom est %n%, o— n est le num‚ro de l'argument. La valeur
sp‚ciale %0% contient le nombre d'arguments pass‚s lors de l'appel au
rapport.

Le rapport "exist.rep" qui suit

&EX2
    $Define VAR %1%
    $GoTo exist {$DataExistVar %VAR%}
    $Show Variable %VAR% does not exist
    $Return
    ....
    $Label exist
    $Show Variable %VAR% exists
    $Return
    ...
&TX
ex‚cut‚ avec A comme argument, v‚rifie si la variable A existe.

On peut ‚galement conserver des valeurs calcul‚es dans des macros.

&EX2
    $Define PNB {PNB}
    $Define GPNB {r PNB * 100}
    La valeur du PNB est %PNB% (diff‚rence de %GPNB% pourcents)
&TX

&IT Mode majuscules/minuscules
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Si le pourcent est imm‚diatement suivi du caractŠre

&EN ## : le contenu de la macro est transpos‚ en majuscules
&EN ! : le contenu de la macro est transpos‚ en minuscules
&EN - : le caractŠres non alphanum‚rique sont supprim‚s de la macro

Par exemple,
&CO
	    $define TOTO TBL1
	    $define ARG0 Abc[-3]
	    Premier  tableau : %!TOTO%
	    DeuxiŠme tableau : %##TOTO%
	    ARG1 : %ARG0O%
	    ARG2 : %-ARG0%
&TX
Donnera … l'ex‚cution :
&CO
	    Premier  tableau : tbl1
	    DeuxiŠme tableau : TBL1
	    ARG1 : Abc[-3]
	    ARG2 : Abc3
&TX
>

<$prompt>
    Commande $PROMPT
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet de d‚finir des macros qui pourront ˆtre utilis‚es
plus tard dans le rapport et ses sous-rapports. Une macro est une sorte
de variable globale qui reste d‚finie durant toute la dur‚e d'ex‚cution
du rapport courant.

La fonction Prompt pose une question … l'utilisateur du rapport et stocke
sa r‚ponse dans une variable exploitable dans la suite du rapport.

&SY2
    ##Prompt VarName Question
    Question := question pos‚e
    VarName := nom utilis‚ pour faire r‚f‚rence au contenu (texte) de la
	       macro (le nom doit commencer par une lettre et ne pas
	       d‚passer 10 caractŠres)


&TX

Le nom est r‚f‚renc‚ dans un rapport en l'encadrant de signes %.

&EX2
    ##Prompt Begin P‚riode de d‚but ?
    $SetTime %Begin%
&TX
>

<$settime>
    Commande $settime
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande fixe la p‚riode (t) pour laquelle les formules LEC
utilis‚es dans les rapports doivent ˆtre calcul‚es.

&NO Elle DOIT ˆtre ex‚cut‚e avant de calculer une formule LEC
    dans un rapport, sans quoi la p‚riode d'ex‚cution est inconnue et
    la formule ne s'ex‚cute pas.

&SY2
    $SetTime period
    period := la valeur de p‚riode … donner … t
&TX

&EX2
    $SetTime 1991Y1
    La valeur du PNB pour l'ann‚e {t@T} est de {PNB} milliards.
&TX
donne en sortie :
&EX2
    La valeur du PNB pour l'ann‚e 1991Y1 est de 4000.32 milliards.
&TX
>

<$incrtime>
    Commande $INCRTIME
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande augmente la p‚riode (t) d'autant d'unit‚s qu'indiqu‚.
Sans argument, l'incr‚ment en fix‚ … 1.

&SY2
    $IncrTime [step]
    step := nombre de p‚riodes … ajouter
&TX

&EX2
    $SetTime 1990Y1
    $Label print
    La valeur du PNB pour l'ann‚e {t@T} est de {PNB} milliards.
    $IncrTime 5
    $goto print {t << 2000Y1}
    $Return
&TX
    fournit comme r‚sultat :
&EX2
    La valeur du PNB pour l'ann‚e 1990Y1 est de 4132.32 milliards.
    La valeur du PNB pour l'ann‚e 1995Y1 est de 4240.32 milliards.
&TX
>


<commandes IODE>
    Les commandes d'ex‚cution de IODE
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Le mot cl‚ des commandes en ligne comprend souvent deux parties (c'est
le cas de toutes les commandes en ligne qui manipulent des objets) : la
commande proprement dite (en g‚n‚ral identique … la commande plein
‚cran) et un suffixe qualifiant l'objet manipul‚: par exemple, … la
commande plein ‚cran ##WsLoad correspondent les commandes en ligne
$WsLoadVar, $WsLoadIdt, etc... respectivement pour les objets variable
et identit‚.

Pour les fonctions "plein ‚cran" [suffixe] peut ˆtre omis, pour les
fonctions en ligne, remplacer [suffixe] par une des extensions suivantes:

#include "ext.m1"

&IT Op‚rations sur des fichiers
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&CO
    filelist,            fileprint,           fileedit,
    filedelete[suffixe], filerename[suffixe], filecopy[suffixe]
&TX

Pour les fonctions en ligne, [suffixe] peut prendre une des valeurs
suivantes:

#include "ext.m1"

&IT Op‚rations sur le WS
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&CO
    wsstatus,

    wsload[suffixe],  wscopy[suffixe],  wsmerge[suffixe], wssave[suffixe],
    wsdescr[suffixe], wsclear[suffixe], wssample, wsextrapolate
&TX

Pour les fonctions en ligne [suffixe] peut prendre une des valeurs
suivantes:

#include "extws.m1"

&IT Op‚rations sur les donn‚es
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&CO
dataupdate[suffixe], datacreate[suffixe], datadelete[suffixe],
dataexist[suffixe], datarename[suffixe], dataduplicate[suffixe],
datasearch[suffixe], dataappend[{cmt|lst}], datascan[{eqs|idt|tbl}]
datalistsort, dataeditcnf, datacalcvar, datadisplaygraph
&TX

Pour les fonctions en ligne, [suffixe] peut prendre une des valeurs
suivantes:

#include "extws.m1"

&IT Impressions d'objets
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&CO
printobjdef[suffixe], printtblfile, printtbl, printvar, viewtblfile,
viewtbl, viewvar, viewgr, printgrall, printgrdata, printgrwin
&TX

Pour les fonctions en ligne [suffixe] peut prendre une des valeurs
suivantes:

#include "extws.m1"

&IT D‚finition des outputs
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&CO
printdest, printdestnew, printnbdec, printlang
&TX

&IT Simulation
ÄÄÄÄÄÄÄÄÄÄ

&CO
modelsimulate, modelsimulateparms, modelexchange, modelcompile
&TX


&IT Ex‚cution d'identit‚s
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&CO
idtexecute, idtexecutevarfiles, idtexecutesclfiles, idtexecutetrace
&TX


&IT Estimation
ÄÄÄÄÄÄÄÄÄÄ

&CO
eqsestimate
&TX

&IT Op‚rations sur des rapports
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&CO
reportexec, reportedit
&TX

>

<Les macros dans les rapports>
    Les macros dans les rapports
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&ALIAS Macro
&ALIAS Macros

Deux sortes de macros peuvent ˆtre utilis‚es dans les rapports IODE.

Les premiŠres sont les macros locales qui sont uniquement connues du
rapport en cours d'ex‚cution. Il s'agit des arguments du rapport (%n%).
On peut utiliser ces valeurs pour peu qu'on connaisse l'ordre dans
lequel elles sont pass‚es. Un sous-rapport re‡oit bien-s–r de nouvelles
d‚finitions pour les variables %n%. Lors du retour dans le rapport de
niveau sup‚rieur, les valeurs de %n% sont remises … leur ‚tat d'origine.

Si le caractŠre % doit se trouver dans le texte, il faut le doubler. Ainsi,

&CO
    %%1%%
&TX

n'est pas consid‚r‚ comme une macro mais remplac‚ dans l'output par le
texte %1%.

&SY2
    %n%
    n := position de l'argument
    (%0% := nombre d'arguments pass‚s au rapport)
    (%*% := tous les arguments pass‚s au rapport)
&TX

Soit le rapport invert.rep :
&EX2
    $show %2% %1%
    $show %*%
    $shift
    $show %*%
    $Return
&TX
L'appel
&EX2
    $ReportExec invert een twee
&TX
donne :
&EX2
    twee een
    een twee
    twee
&TX

Le deuxiŠme type de macros sont globales : elles sont connues dŠs leur
d‚finition et utilisables dans tous les sous-rapports via leur nom.
Leur contenu peut ˆtre modifi‚ pendant l'ex‚cution du rapport (ou des
sous-rapports) par l'instruction $Define avec le mˆme nom. La derniŠre
d‚finition recontr‚e annule les pr‚c‚dentes.

&SY2
    %naam%
    naam := nom de la macro (cfr. $Define)
&TX

On peut ‚galement les utiliser pour globaliser une variable locale et
par l… exploiter un paramŠtre de rapport dans un sous-rapport :

&EX2
    $Define FILE %1%
    $Define VAR %2%
    $WsCopyVar %FILE% %VAR%
&TX
>

<Les expressions LEC dans les rapports>
    Les expressions LEC dans les rapports
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

On peut entrelarder le texte et les commandes de formules LEC : cela permet
par exemple de baser des conditions de $goto sur les valeurs des
variables ou des scalaires. Si des variables sont utilis‚es, la p‚riode
de calcul correspond … la valeur actuelle de t, fix‚ par les commandes
$SetTime et $IncrTime.

Pour indiquer une formule LEC … calculer dans un rapport, il suffit de
placer celle-ci entre accollades. On peut en formatter le r‚sultat comme
suit :

&SY2
    {LEC}       le r‚sultat est formatt‚ de maniŠre automatique
    {LEC@T}     le r‚sultat est formatt‚ sous forme de p‚riode (1990Y1)
    {LEC@99.9}  le r‚sultat est formatt‚ sur 4 positions avec une d‚cimale
    {LEC@.99}   le r‚sultat est formatt‚ sur avec deux d‚cimales
&TX

&EX2
    $SetTime 1990Y1
    Le PNB en {t@T} vaut {PNB@9999.9} milliards de FB.
&TX

donne comme r‚sultat :

&EX2
    Le PNB en 1990Y1 vaut 3089.0 milliards de FB.
&TX

Si le caractŠre ~c'{'~C doit se trouver dans le texte, il faut le
doubler. Ainsi,

&CO
    {{exemple de formule lec}
&TX

n'est pas consid‚r‚ comme une formule LEC … calculer mais est remplac‚ dans
l'output par le texte :

&CO
    {exemple de formule lec}
&TX
>

<Valeurs d'un worksheet Excel dans les rapports>
    Valeurs d'un worksheet Excel dans les rapports
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Si un worksheet Excel contient des donn‚es n‚cessaires … l'ex‚cution d'un rapport,
il est facile d'int‚grer dynamiquement des valeurs ou des ranges de valeurs dans le texte
d'un rapport.

Il suffit de placer la r‚f‚rence aux cellules n‚cessaires entre accollades
dans une ligne de rapport :

&CO
    $DataUpdateVar {=Sheet1!R2C1} %YEAR% {=Sheet1!R2C2:R3C10}
&TX

La premiŠre r‚f‚rence ~c{=Sheet1!R2C1}~C est lue dans Excel (le fichier doit
‚videmment ˆtre pr‚alablement ouvert) et est replac‚e dans la ligne de
commande. Dans l'exemple, il s'agira du nom de la s‚rie … modifier.


La seconde r‚f‚rence ~c{=Sheet1!R2C2:R3C10}~C est ‚galement remplac‚e par la
valeur dans Excel. S'il y a plusieurs cellules, celles-ci sont s‚par‚es par
des blancs.

Dans l'exemple, on aura par exemple :

&CO
    $DataUpdateVar PNB 1990 3.23 2.34 2.56 3.12 3.45
&TX

AprŠs remplacement, la commande est ex‚cut‚e comme n'importe quelle autre
commande de rapport.
>



#include "repn.m"


