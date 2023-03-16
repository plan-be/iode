<datalistsort>
    DataListSort
    ÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction permet un tri alphanum‚rique sur le contenu d'une liste.

&FS
Les deux champs … remplir permettent de pr‚ciser le nom de la liste …
trier et le nom de la liste r‚sultat.

&SY2
    ##DataListSort
&TX

&MU

&NFS
La commande prend deux paramŠtres, le premier pr‚cise le nom de la liste …
trier et le deuxiŠme le nom de la liste r‚sultat.

&SY2
    $DataListSort liste liste_tri‚e
&TX
>
<datacompareeps>
    DataCompareEps
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&TPART
&DE

Cette fonction permet de fixer le
seuil en-de‡… duquel le test d'‚galit‚ des variables est consid‚r‚ comme ayant ‚t‚
satisfait.

Le test de comparaison est :
&CO
    si x1 <<>> 0 :  |(x1 - x2) / x1| << eps
    sinon :       |x2| << eps
&TX

&SY
    $DataCompareEps eps
&TX

L'‚cran de comparaison de WS (Data/List/File Compare) permet ‚galement de
sp‚cifier cette valeur (paramŠtre Threshold).

&TI Seuil de comparaison par d‚faut
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Le seuil de comparaison est fix‚ … 1e-7 par d‚faut.

>

<datacompareXxx>
    DataCompareXxx
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&TPART
&DE
Le contenu du WS courant peut ˆtre compar‚ … celui d'un fichier. Le
r‚sultat de cette comparaison est compos‚ de 4 listes :

&EN objets trouv‚s dans le WS seulement
&EN objets trouv‚s dans le fichier seulement
&EN objets trouv‚s dans les deux, avec la mˆme d‚finition
&EN objets trouv‚s dans les deux, avec une d‚finition diff‚rente

La comparaison s'effectue en fonction du type des objets.

&FS
Les champs … remplir sont de deux types: les deux premiers sp‚cifient
le type de WS … comparer (VAR, etc) et le nom du fichier … comparer
au WS.

Les quatre suivants permettent d'indiquer les noms … donner aux listes
r‚sultat.

&SY2
    ##DataCompare
&TX

&NFS
La fonction prend 5 paramŠtres (le type d'objet fait partie du nom
de la fonction): le fichier … comparer les 4 listes … construire.

&SY2
    $DataCompareXxx filename ONE TWO THREE FOR

	  ONE         in WS only
	  TWO         in file only
	  THREE       in both, equal
	  FOR         in both, different
&TX

&EX2
    $DataCompareVar myws WS FILE EQ DIFF
&TX
>

<datalistXxx>
    DataListXxx
    ÄÄÄÄÄÄÄÄÄÄÄ
&TPART
&DE
Cette fonction construit une liste de noms d'objets sur base d'un
critŠre (pattern) de s‚lection sur les noms. Elle peut fonctionner
sur les objets en WS ou sur les objets d'un fichier.

Elle permet par exemple d'obtenir la liste des objets dont le noms
commence par A et se termine par BEL (A*BEL).

&FS
Les 4 champs … remplir permettent de pr‚ciser le nom de la liste …
contruire, le critŠre, le type d'objet et le nom du fichier. Si ce dernier
est laiss‚ vide, les objets du WS sont utilis‚s.

&SY2
    ##DataListXxx
&TX

&MU

&NFS
La commande prend deux ou trois paramŠtres, le premier pr‚cise le nom
de la liste … cr‚er, le deuxiŠme le pattern et le dernier (optionnel)
le nom du fichier.

&SY2
    $DataListXxx listname pattern [filename]
&TX

&EX2
    $DataListVar _BEL_ *BEL*
&TX

Fournit dans la liste _BEL_ toutes les variables sont le nom contient
le texte BEL: AX1BEL, BEL1, MIRABELLE, etc.
>

<datacalclst>
    DataCalcLst
    ÄÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction calcule une liste sur base de deux listes en en prenant
la r‚union, l'intersection ou la diff‚rence.

&FS
Les 4 champs … remplir permettent de pr‚ciser
&EN le nom de la liste r‚sultat
&EN le noms des op‚randes (listes)
&EN l'op‚rateur

&SY2
    ##DataCalcLst
&TX

&NFS
La commande prend 4 paramŠtres, le premier pr‚cise le nom de la liste
r‚sultat, le deuxiŠme celui de la premiŠre liste, le troisiŠme
l'op‚ration et le quatriŠme le nom de la deuxiŠme liste.

&SY2
     $DataCalcLst res lst1 op lst2
	o— op = + : union
		* : intersection
		- : diff‚rence
&TX

&EX2
    $DataCalcLst _RES LST1 * LST2
&TX

Fournit dans la liste _RES les noms pr‚sents dans LST1 et LST2.
>

<dataeditcnf>
    DataEditCnf
    ÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction permet de changer le mode de visualisation du tableau de
s‚ries en "Level" (valeur r‚elle) en mode "Difference" et en mode
"Growth Rate" (taux de croissance). Elle permet aussi de choisir le
nombre de d‚cimales … afficher.

&FS
Les deux champs … remplir permettent de pr‚ciser le mode de visualisation
et le nombre de d‚cimales … afficher.

&SY2
    ##DataEditCnf
&TX
&MU

&NFS
On passe deux paramŠtres … cette fonction :

&EN le mode de visualisation {Level | Differences | GrowthRates}

&EN le nombre de d‚cimales … afficher (-1, 0, 1, 2, ...)

&SY2
    $DataEditCnf {L | D | G} n
    (n : = -1, 0, 1, 2, 3, ...)
&TX
>

<datadisplaygraph>
    DataDisplayGraph
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction permet de visualiser des s‚ries sous forme de graphique.

&FS
Les champs … remplir permettent de pr‚ciser les noms des variables, le
mode de visualisation, la p‚riode, le type les axes, les grids, ....

&EN Variables name : liste des variables … inclure dans le graphique. Les
    noms (ou liste) peuvent ˆtre s‚par‚s par un blanc, un + ou une
    combinaison des deux. Les variables s‚par‚es par un blanc seront
    visualis‚es dans des graphiques s‚par‚s sur la mˆme page, tandis que
    les variables "additionn‚es" seront group‚es dans un mˆme graphique.
    Ainsi, si ce champ contient "X Y+Z T", trois graphiques seront
    construits avec respectivement les variables X (premier graphique),
    Y et Z (group‚es dans le second graphique) et T (dernier graphique).

&EN Mode : il s'agit d'un champ MENU permettant de pr‚ciser si les
    variables doivent ˆtre visualis‚es en valeur, en diff‚rence ou en
    taux de croissance :
&CO
    L=level
    D=differences
    G=growthrates
    d=YoY differences : diff‚rences sur 1 ann‚e
    g=YoY growth rates : taux de croissance sur 1 ann‚e

&TX

&EN Chart type : il s'agit d'un champ MENU permettant de pr‚ciser le
    type de graphique … produire: line, scatter, scatter line ou bar

&EN Grids : il s'agit ‚galement d'un champ MENU permettant de d‚finir le
    type de quadrillage en X et en Y (pas de quadrillage, quadrillage
    majeur ou quadrillage mineur)

&EN Y scaling: ces deux champs permettent de d‚finir les valeurs minimum et
    maximum de l'axe des Y. Si ces valeurs sont nulles, l'axe est
    calcul‚ automatiquement en fonction des valeurs … afficher.

&EN Axis : ce champ du type MENU permet de s‚lectionner le type de
    transformation de coordonn‚es … utiliser: valeur, logarithme, ...

&EN Sample : pr‚ciser la p‚riode d'impression (de p‚riode_1 …
    p‚riode_2). La syntaxe d'une p‚riode fait l'objet d'un chapitre s‚par‚.

&EN Save graph in file: le graphique peut ˆtre sauvegard‚ dans un
    fichier au format AGL ‚ditable par le programme GODE.

&SY2
    ##DataDisplayGraph
&TX

&MU

&NFS
On passe les paramŠtres suivants … cette fonction :

&SY2
    $DataDisplayGraph {Level | Diff | Grt | diffYoY | grtYoY}
		      {Line | Scatter | Bar | Mixt}
		      {NoXGrids | MinorXGrids | J(MajorXGrids)}
		      {NoYGrids | MinorYGrids | J(MajorYGrids)}
		      {Level | G(Log) | Semi-Log | Percent}
		      {ymin | --} {ymax | --}
		      perfrom perto varname1 varname2 ...

&TX

La signification des paramŠtres est expliqu‚e plus haut. Il est … noter
que tous les paramŠtres sont requis et doivent ˆtre pass‚s dans l'ordre
indiqu‚.
>

<datasavegraph>
    DataSaveGraph
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction permet de construire des graphiques … partir de s‚ries
et d'en sauvegarger le r‚sultat dans un fichier AGL.

&FS
Les champs … remplir permettent de pr‚ciser les noms des variables, le mode
de visualisation, la p‚riode, le type les axes, les grids, ....

&EN Variables name : liste des variables … inclure dans le graphique. Les
    noms (ou liste) peuvent ˆtre s‚par‚s par un blanc, un + ou une
    combinaison des deux. Les variables s‚par‚es par un blanc seront
    visualis‚es dans des graphiques s‚par‚s sur la mˆme page, tandis que
    les variables "additionn‚es" seront group‚es dans un mˆme graphique.
    Ainsi, si ce champ contient "X Y+Z T", trois graphiques seront
    construits avec respectivement les variables X (premier graphique),
    Y et Z (group‚es dans le second graphique) et T (dernier graphique).

&EN Mode : il s'agit d'un champ MENU permettant de pr‚ciser si les variables
    doivent ˆtre visualis‚es en valeur, en diff‚rence ou en taux de
    croissance

&EN Chart type: il s'agit d'un champ MENU permettant de pr‚ciser le type de
    graphique … produire: line, scatter, scatter line ou bar

&EN Grids : il s'agit ‚galement de champs MENU permettant de d‚finir le type
    de quadrillage en X et en Y (pas de quadrillage, quadrillage majeur
    ou quadrillage mineur)

&EN Y scaling: ces deux champs permettent de d‚finir les valeurs minimum et
    maximum de l'axe des Y. Si ces valeurs sont nulles, l'axe est
    calcul‚ automatiquement en fonction des valeurs … afficher.

&EN Axis : ce champ du type MENU permet de s‚lectionner le type de
    transformation de coordonn‚es … utiliser: valeur, logarithme, ...

&EN Sample : pr‚ciser la p‚riode d'impression (de p‚riode_1 …
    p‚riode_2). La syntaxe d'une p‚riode est donn‚es dans le manuel de
    r‚f‚rence.

&EN Save graph in file: le graphique peut ˆtre sauvegard‚ dans un
    fichier au format AGL ‚ditable par le programme GODE.

&SY2
    ##DataDisplayGraph
&TX

&MU

&NFS
On passe les mˆmes paramŠtres que ceux d‚crits ci-dessus … cette
fonction :

&SY2
    $DataSaveGraph    aglfilename
		      {Level | Diff | Grt}
		      {Line | Scatter | Bar | Mixt}
		      {NoXGrids | MinorXGrids | J(MajorXGrids)}
		      {NoYGrids | MinorYGrids | J(MajorYGrids)}
		      {Level | G(Log) | Semi-Log | Percent}
		      {ymin | --} {ymax | --}
		      perfrom perto varname1 varname2 ...

&TX

Il est … noter que tous les paramŠtres sont requis et doivent ˆtre
pass‚s dans l'ordre indiqu‚.
>

<datacalcvar>
    DataCalcVar
    ÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction permet de calculer une nouvelle s‚rie … partir d'une
formule LEC. Cette formule est ex‚cut‚e sur toute la p‚riode du WS
courant et ce en utilisant les valeurs des variables du WS courant.

&FS
La fonction plein ‚cran ‚quivalente est la fonction standard d'‚dition
des variables.

&NFS
On passe deux paramŠtres … cette fonction :

&EN le nom de la variable … cr‚er

&EN la formule LEC … calculer

&SY2
    $DataCalcVar nom formule
&TX

&EX2
    $DataCalcVar X    beta + gamma * ln B + alpha * ln C
&TX
>

<datacreate>
    DataCreateXxx
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ
&TPART
&EXTWS
&DE
La fonction permet la cr‚ation d'objets dans le WS courant. Il n'y a pas
de fonction plein ‚cran ‚quivalente, sinon les fonctions standard
d'‚dition d'objets (menu DATA).

&NFS
Si l'objet "nom" existe d‚j…, la fonction retourne et signale l'erreur.
Sinon l'objet est cr‚‚ avec une valeur par d‚faut :

&EN commentaires, listes, identit‚es et tableaux : vide

&EN ‚quation : "NOM : =NOM"

&EN scalaires : 0.9

&EN variables : une s‚rie avec des NA (--) sur toute la p‚riode du WS

&EN tableaux : cr‚e un tableau vide

&SY2
    $DataCreateVar nom
&TX
>

<datadelete>
    DataDeleteXxx
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ
&TPART
&EXTWS
&DE
La fonction permet de d‚truire des objets dans le WS courant. Il n'y a pas
de fonction plein ‚cran ‚quivalente, sinon la fonction d'‚dition
habituelle (menu Data).

&NFS
Si l'objet "nom" n'existe pas, la fonction retourne et signale l'erreur.

&SY2
    $DataDeleteVar nom
&TX

&NO Depuis la version 5.13, les wildcards sont accept‚es dans le nom
>

<dataexist>
    DataExistXxx
    ÄÄÄÄÄÄÄÄÄÄÄÄ
&TPART
&EXTWS
&DE
La fonction v‚rifie l'existence d'un objet dans le WS courant.

&FS
&NEQ

&NFS
Si l'objet "nom" n'existe pas, la fonction retourne -1 et produit un message d'erreur.

Si $OnError est fix‚ … Abort ou Return dans le rapport en cours, l'ex‚cution
du rapport se termine.

&SY2
    $DataExistVar nom
&TX


&EX

&TX
>

<datarename>
    DataRenameXxx
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ
&TPART

Xxx prend l'une des valeurs :
&CO
    cmt | idt | lst | scl | tbl | var
&TX

&DE
La fonction change le nom d'un objet dans le WS courant.

Note: les ‚quations ne peuvent ˆtre renomm‚es (car leur endogŠne est aussi leur nom).

&FS
&NEQ
&NFS
Si l'objet "nom" n'existe pas, ou si Xxx correspond … EQS, la fonction retourne et signale l'erreur.
Si l'objet existe, il sera sauv‚ sous un nouveau nom. S'il y a d‚j… un
objet portant ce nouveau nom, il est remplac‚.

&SY2
    $DataRenameVar nom nouveau_nom
&TX
>

<dataupdate>
    DataUpdateXxx
    ÄÄÄÄÄÄÄÄÄÄ
&TPART
&EXTWS
&DE
La fonction change le contenu d'un objet dans le WS courant.

&FS
Il n'y a pas de fonction plein ‚cran ‚quivalente, si ce n'est la
fonction habituelle d'‚dition (Menu DATA).

&NFS
Si l'objet "nom" n'existe pas, un objet contenant la nouvelle d‚finition
est cr‚‚. Si l'objet existe, son contenu est remplac‚. La fonction
prend plusieurs arguments. Les arguments diffŠrent selon le type d'objet …
mettre … jour. Le premier argument est toujours le nom de l'objet …
changer.

&IT Commentaires

&CO
    $DataUpdateCmt nom commentaire
&TX

&IT Equations

&CO
    $DataUpdateEqs nom ‚quation_lec
&TX

&IT Identit‚s

&CO
    $DataUpdateIdt nom identit‚_lec
&TX

&IT Listes

&CO
    $DataUpdateLst nom liste
&TX

&IT Scalaires

&CO
    $DataUpdateScl nom valeur [relax]
&TX

&IT Tableaux

&CO
    $DataUpdateTbl table_name title;lec1;lec2;...
&TX
Si title est un nom de commentaire, le commentaire est utilis‚ comme
titre. De mˆme, si on trouve des noms de variables comme forme lec, et
qu'il existe un commentaire pour ces variables, le titre de la ligne
correspondante est remplac‚ par la valeur du commentaire.

Supposons qu'il existe en WS un commentaire pour A et non pour B, et un
commentaire TIT :
&CO
    Commentaire A   : "Produit national brut"
    Commentaire TIT : "Titre de mon tabelo"
&TX
La ligne
&CO
    $DataUpdateTbl T1 TIT;A;B;A+B
&TX
cr‚e le tableau T1 avec la forme suivante :
&CO
    +-------------------------------------------+
    |          Titre de mon tabelo              |
    |                                           |
    |  Produit national brut               A    |
    |  B                                   B    |
    |  A+B                                 A+B  |
    +-------------------------------------------+
&TX

&IT Variables

&CO
    $DataUpdateVar nom [L,l | D,d | G,g] p‚riode valeur1 valeur2 ...
     o— L,l := en valeur (d‚faut)
	D, d := en diff‚rence
	G, g := en taux de croissance
	p‚riode := la p‚riode du d‚but de la mise … jour
&TX
>

<dataappend>
    DataAppendXxx
    ÄÄÄÄÄÄÄÄÄÄ
&TPART

Xxx prend l'une des valeurs :
&CO
    cmt | lst
&TX

&DE
La fonction ajoute un texte … la d‚finition d'un objet du WS courant. Seuls les commentaires
et les listes peuvent ˆtre prolong‚es par cette fonction.

&FS
&NEQ
&NFS
Si l'objet "nom" n'existe pas, un objet contenant la nouvelle d‚finition
est cr‚‚. Si l'objet existe, son contenu est compl‚t‚.

&EN Commentaires

&CO
    $DataAppendCmt nom commentaire
&TX

&EN Listes

&CO
    $DataAppendLst nom list
&TX
>

<datascan>
    DataScanXxx
    ÄÄÄÄÄÄÄÄÄÄÄ
&TPART
&EXTWS
&DE
Cette fonction permet de rechercher dans un des WS courants la liste des
variables et scalaires utilis‚s dans la d‚finition des objets de ce WS.

Le r‚sultat de cette recherche est sauv‚ dans deux listes, contenant
d'une part les noms des scalaires (liste _SCAL) et d'autre part ceux des
variables (liste _EXO). Si on ne passe pas d'argument … cette fonction,
tout le WS est examin‚.

&FS
&MU
&NFS
&CO
    $DataScanEqs [nom1,nom2,...]
    $DataScanIdt [nom1,nom2,...]
    $DataScanTbl [nom1,nom2,...]
&TX
>

<dataedit>
    DataEditXxx
    ÄÄÄÄÄÄÄÄÄÄÄ
&TPART
&EXTWS
&DE
La fonction permet l'‚dition d'objets du WS courant.

Un ‚cran pr‚sente les objets s‚lectionn‚s sous forme d'un tableau
d‚roulant (ou tableau d'‚dition) dans lequel on pourra se d‚placer.

Le tableau d'‚dition est compos‚ des ‚l‚ments suivants :

&EN le nom du WS dans la ligne sup‚rieure
&EN le nom des objets dans la colonne de gauche
&EN la d‚finition (ou une partie de la d‚finition) des objets en regard
    du nom dans la partie de droite
&EN un scrollbar indiquant la position courante dans le tableau et la
    proportion visible du tableau

L'objet courant est indiqu‚ par le fait que la ligne qui lui correspond
est en vid‚o inverse.

Les touches de fonction des tableaux d‚roulant permettent l'affichage de
la valeur, la modification et la cr‚ation des objets.

&FS
La liste des objets … ‚diter est pass‚e en argument de la commande. Les
‚l‚ments de la liste sont s‚par‚s par des blancs, virgules ou
points-virgules. Si la liste est vide (pas d'argument), tous les objets
apparaissent dans le tableau d'‚dition.

&SY2
    ##DataEditXxx [objet [liste d'objets]]
    {Xxx = cmt | eqs | idt | lst | scl | tbl | var}
&TX

&NFS
La fonction n'est exploitable qu'en mode plein ‚cran.
>

<datawidthvar>
    DataWidthVar
    ÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction permet de sp‚cifier la largeur des colonnes d'‚dition
des s‚ries statistiques. La valeur doit ˆtre comprise entre 2 et 12.

&FS
Cette fonction n'est pas disponible en mode plein ‚cran. Les touches
F3 et s-F3 permettent de changer la largeur des colonnes lors de l'‚dition.

&NFS
&SY2
    $DataWidthVar n
    n entre 2 et 12
&TX
>
<datandecvar>
    DataNdecVar
    ÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction permet de sp‚cifier le nombre de d‚cimales lors de
l'‚dition des s‚ries statistiques. La valeur doit ˆtre comprise entre
-1 (nombre de d‚cimales variable) et 6.

&FS
Cette fonction n'est pas disponible en mode plein ‚cran. Les touches
F4 et s-F4 permettent de changer la largeur des colonnes de l'‚dition.

&NFS
&SY2
    $DataNdecVar n
    n entre -1 et 6
&TX
>
<datamodevar>
    DataModeVar
    ÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction permet de choisir le mode d'‚dition des s‚ries statistiques:
Niveau, diff‚rences ou taux de croissance.

&FS
Cette fonction n'est pas disponible en mode plein ‚cran. La touche
F5 permet de changer la largeur des colonnes lors de l'‚dition.

&NFS
&SY2
    $DataModeVar n
    n = 0 pour niveau
    n = 1 pour diff‚rences
    n = 2 pour taux de croissances
&TX

>
<datastartvar>
    DataStartVar
    ÄÄÄÄÄÄÄÄÄÄÄÄ


&DE
Cette fonction permet de choisir la premiŠre colonne visible lors de
l'‚dition des s‚ries statistiques. La valeur doit ˆtre comprise entre
0 (premiŠre colonne) et le nombre de p‚riodes disponibles.

&FS
Cette fonction n'est pas disponible en mode plein ‚cran.

&NFS
&SY2
    $DataStartVar n
    n entre 0 et nombre de p‚riodes
&TX
>
<datawidthtbl>
    DataWidthTbl
    ÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction permet de sp‚cifier la largeur des colonnes d'‚dition
des tableaux. La valeur doit ˆtre comprise entre 2 et 60.

&FS
Cette fonction n'est pas disponible en mode plein ‚cran. Les touches
F3 et s-F3 permettent de changer la largeur des colonnes lors de l'‚dition.

&NFS
&SY2
    $DataWidthTbl n
    n entre 2 et 60
&TX

>
<datawidthscl>
    DataWidthScl
    ÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction permet de sp‚cifier la largeur des colonnes d'‚dition
des s‚ries scalaires. La valeur doit ˆtre comprise entre 2 et 12.

&FS
Cette fonction n'est pas disponible en mode plein ‚cran. Les touches
F3 et s-F3 permettent de changer la largeur des colonnes lors de l'‚dition.

&NFS
&SY2
    $DataWidthScl n
    n entre 2 et 12
&TX
>
<datandecscl>
    DataNdecScl
    ÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction permet de sp‚cifier le nombre de d‚cimales lors de
l'‚dition des scalaires. La valeur doit ˆtre comprise entre
-1 (nombre de d‚cimales variable) et 6.

&FS
Cette fonction n'est pas disponible en mode plein ‚cran. Les touches
F4 et s-F4 permettent de changer la largeur des colonnes de l'‚dition.

&NFS
&SY2
    $DataNdecScl n
    n entre -1 et 6
&TX

>
<datasearch>
     DataSearchXxx
     ÄÄÄÄÄÄÄÄÄÄÄÄÄ
&TPART
&EXTWS
&DE
Cette fonction permet de rechercher dans un des WS courants la liste des
objets contenant un string donn‚.

Le r‚sultat de cette recherche est une liste d'objets qui peut au besoin
ˆtre sauv‚e dans le WS courant de listes. Par d‚faut la liste _RES contient
le r‚sultat de la derniŠre recherche.

&FS
La fonction affiche une page de saisie comprenant les champs suivants :

&EN Texte … rechercher : le texte … rechercher peut contenir des
    caractŠres sp‚ciaux qui permettent de sp‚cifier les limites de la
    recherche :
&EN2 * : n'importe quelle suite de caractŠres (mˆme vide)
&EN2 ? : un et un seul caractŠre (quelconque)
&EN2 @ : n'importe quel caractŠre alphanum‚rique
&EN2 ampersand : n'importe quel caractŠre non alphanum‚rique
&EN2 | : n'importe quel caractŠre alphanum‚rique ou aucun en d‚but et fin
    de string
&EN2 ! : n'importe quel caractŠre non alphanum‚rique ou aucun en d‚but et
    fin de string
&EN2 \ plac‚ devant un des caractŠres sp‚ciaux supprime son interpr‚tation

&EN Mot entier/partiel (Whole word) : pr‚ciser Yes si la chaŒne …
    rechercher doit ˆtre un mot entier et non une partie de mot.
    Indiquer No si cela n'a pas d'importance.

&EN Majuscule/minuscule (Exact case) : pr‚ciser si la recherche doit
    diff‚rencier majuscules et minuscules dans la chaŒne … rechercher.


&EN Type d'objet: le type d'objet doit ˆtre indiqu‚ dans ce champ. En
    pressant TAB, la liste des types apparaŒt. Il suffit de se placer
    sur le type choisi et de presser ENTER.

Les recherches sont diff‚rentes en fonction du type d'objet :

&EN Commentaires : le nom et le texte du commentaire sont analys‚s
&EN Equations    : le nom et la forme LEC de l'‚quation sont analys‚s
&EN Identit‚s    : le nom et la forme LEC de l'identit‚ sont analys‚s
&EN Listes       : le nom et le texte de la liste sont analys‚s
&EN Scalaires    : le nom du scalaire est analys‚
&EN Tableaux     : le nom, les titres et les formes LEC du tableau sont
		   analys‚s
&EN Variables    : le nom de la variable est analys‚

&EN Liste r‚sultat: il est possible de sauver la liste des objets
    r‚pondant au critŠre dans le WS de listes courant. Le nom de cette
    liste doit ˆtre indiqu‚ et ˆtre un nom de liste valide. Si une liste
    de mˆme nom existe, elle sera remplac‚e par le r‚sultat du calcul.

&EN R‚sultat: dans le bas de l'‚cran, un champ reprend les premiers
    objets satisfaisant au critŠre.

La recherche commence lorsque la page est valid‚e avec F10.

&SY2
    ##DataSearchCmt
&TX

&NFS
Sept paramŠtres doivent ˆtre pass‚s … la commande.

&SY2
    $DataSearchXxx mask word ecase in_name in_formula in_text list_result

    (word, ecase, in_name, in_formula, in_text := 0 ou 1)
    (mask := caractŠres et ‚ventuellement ?, *, ...)
    (list_result := le nom de la liste r‚sultat)
&TX

&EX2
    $datasearchCmt TE?T 0 0 1 0 1 NEW2
    ##DataEditCMT $NEW2
&TX
>


<dataduplicate>
    DataDuplicateXxx
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ
&TPART
&EXTWS
&DE
Les objets d‚finis dans les WS courants peuvent ˆtre copi‚s dans des
objets du mˆme type mais d'un nom diff‚rent. Remarque : il existe une
m‚thode simple pour effectuer cette op‚ration : dans l'‚cran de
d‚finition de l'objet … duplicier, il suffit de changer le nom de
l'objet et de le sauver. Un nouvel objet ayant les mˆmes
caract‚ristiques que l'ancien est ainsi cr‚‚.

La fonction d‚crite ici permet la mˆme op‚ration. Elle fonctionne de
la mˆme maniŠre pour tous les objets sauf pour les ‚quations.

&FS
La commande affiche un ‚cran de saisie contenant trois champs.

Le type d'objet … ‚diter peut ˆtre s‚lectionn‚ en se pla‡ant sur le
champ indiquant le type et en pressant TAB : un menu reprenant tous les
types d'objets permet d'effectuer la s‚lection.

Les deux autres champs contiennent respectivement l'ancien et le
nouveau nom de l'objet.

&SY2
    ##DataDuplicateCmt
&TX

&NFS
La fonction prend successivement deux paramŠtres: l'ancien et le
nouveau nom de l'objet.

&SY2
    $DataDuplicateCmt ancien_nom nouveau_nom
&TX

&EX2
    $DataDuplicateCmt A01 A02
&TX

La fonction DataDuplicateEqs n'est pas impl‚ment‚e et n'a d'ailleurs pas
de signification. En effet, une ‚quation porte le nom de sa variable
endogŠne. Changer son nom change donc le nom de l'endogŠne, ce qui en
g‚n‚ral n'a pas de sens.

L'utilisateur curieux qui utiliserait malgr‚ tout cette fonction
recevrait un message d'avertissement.
>


<printdest>
    PrintDest
    ÄÄÄÄÄÄÄÄÄ

&DE
La fonction permet de sp‚cifier s'il faut imprimer dans un fichier et, si
c'est le cas, le nom et le type du fichier r‚sultat. Cette d‚finition est
valable pour toutes les impressions faites au d‚part d'un menu ou d'un
rapport de IODE, jusqu'… la fin du rapport en ex‚cution ou jusqu'… ce
qu'une autre commande du rapport sp‚cifie une autre destination, auquel
cas la configuration de l'impression est modifi‚e pour toutes les
impressions ult‚rieures.

Le fichier destination est toujours r‚initialis‚, mˆme dans le cas
des fichiers a2m. Ceci est nouveau … partir de la version 5 de IODE.

&FS
Dans chaque ‚cran d'impression les diff‚rents paramŠtres requis peuvent
ˆtre sp‚cifi‚s : nombre de d‚cimales et langue par d‚faut. Un bouton
Setup permet en outre de d‚terminer s'il faut imprimer dans un fichier
ou sur une imprimante.

&NFS
Le nom du fichier destination est pass‚ en paramŠtre de la commande :

&SY2
    $PrintDest [nom_fichier] [format]

	o— format = A2M, MIF, HTML, RTF ou CSV
	par d‚faut, A2M est le format choisi
&TX
Si ~cnom_fichier~C n'est pas sp‚cifi‚, l'impression aura lieu sur une
imprimante.


&EX2
    $PrintDest test.mif MIF
&TX
>
<printdestnew>
    PrintDestNew
    ÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction est identique … $PrintDest, … ceci prŠs qu'elle d‚truit
le fichier output.

&NFS
Le nom du fichier destination est pass‚ en paramŠtre de la commande :

&SY2
    $PrintDestNew [nom_fichier] [format]

	o— format = A2M, MIF, HTML, RTF ou CSV
	par d‚faut, A2M est le format choisi
&TX
Si ~cnom_fichier~C n'est pas sp‚cifi‚, l'impression aura lieu sur une
imprimante.
>

<printnbdec>
    PrintNbDec
    ÄÄÄÄÄÄÄÄÄÄ
&DE
La fonction permet de sp‚cifier le nombre de d‚cimales … imprimer lors
des impressions de tableaux ou de variables.

&FS
Dans chaque ‚cran d'impression les diff‚rents paramŠtres requis sont
sp‚cifi‚s.

Un de ces paramŠtres est "NbDec". Un nombre entier positif sp‚cifie le
nombre de d‚cimales qui seront imprim‚es. Si la valeur -1 est sp‚cifi‚e,
le nombre de d‚cimales est celui de la d‚finition interne de la valeur …
imprimer (en g‚n‚ral quelconque).

&NFS
Le nombre de d‚cimales est pass‚ en paramŠtre de la commande :

&SY2
    $PrintNbDec  nb
&TX

&EX2
    $wsloadvar bist92\bistel
    $wsloadtbl bist92\tbistelf

    $printdest bist92\bistelf1.a2m
    ##show processing french tables  file 1/2
    $printnbdec 1
    $PrintTbl 89:8 HYPEIR
    $PrintTbl 89/88:8 HYPEIIR
&TX
>

<printlang>
    PrintLang
    ÄÄÄÄÄÄÄÄÄ

&DE
La fonction permet de sp‚cifier la langue d'impression quand on imprime
les tableaux. La langue par d‚faut est l'anglais.

&FS
Dans chaque ‚cran d'impression les diff‚rents paramŠtres requis sont
sp‚cifi‚s. Le champ "Language" est un champ MENU dans lequel on pr‚cise
la langue d'impression.

&NFS
La langue d'impression est pass‚ en paramŠtre de la commande :

&SY2
    $PrintLang {English | French | Dutch}
&TX

&EX2
    $wsloadvar bist92\bistel
    $wsloadtbl bist92\tbistelf

    $printdest bist92\bistelf1.a2m
    ##show processing dutch tables  file 1/2
    $printnbdec 1
    $printlang Dutch
    $PrintTbl 89:8 HYPEIR
    $PrintTbl 89/88:8 HYPEIIR
&TX
>

<printobjdef>
    PrintObjDefXxx
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&TPART
&EXTWS
&DE
Cette commande permet l'impression au format A2M de la d‚finition
des objets du ws courant.

&FS
La fonction affiche une page de saisie dans laquelle on d‚finit le ou
les paramŠtres de la commande.

La page de saisie contient deux champs: le premier est un champ MENU
(voir lexique) : on y s‚lectionne le type d'objet … imprimer (le type
CMT est propos‚ par d‚faut); le second est un champ EDITOR (voir
lexique): on y sp‚cifie la liste des objets … imprimer.

L'imression se fait sur l'imprimante s‚lectionn‚e lorsqu'on valide la
page avec F10. La d‚finition est imprim‚ en format A2M.

Si la liste des objets est vide, tous les objets du type sp‚cifi‚ dans le
WS courant sont imprim‚s. La liste peut contenir des noms de listes ($...)
et/ou une ‚num‚ration de noms d'objets s‚par‚s par un blanc, une virgule ou un
point-virgule.

&MU

&SY2
    ##PrintObjDefCmt
&TX

&NFS
La liste des commentaires … imprimer est pass‚e en paramŠtre de la
commande.

&SY2
    $PrintObjDefXxx [liste de paramŠtres]
    o— Xxx = {cmt | idt | lst | scl | tbl | var}.
&TX

&EX2
    $PrintObjDefCmt CMT1 CMT2
&TX
>


<printtbl>
    PrintTbl
    ÄÄÄÄÄÄÄÄ

&DE
Cette commande permet le construction et l'impression des tableaux au
format A2M.

&FS
La fonction affiche une page de saisie dans laquelle on d‚finit le ou
les paramŠtres de la commande. Son fonctionnement est d‚crit dans le
manuel de l'utilisateur.

&SY2
    ##PrintTbl
&TX

&NFS
Le sample d'impression suivi de la liste des tableaux … imprimer sont
pass‚s comme paramŠtres de la commande.

Le dernier appel … $PrintTblFile permet d'imprimer des comparaisons de
fichiers. Cependant, l'appel … $PrintTbl ou $PrintVar "consomme" les
fichiers d‚finis par $PrintTblFile. Si on veut faire appel plusieurs
fois … $PrintTbl pour comparer des fichiers, il faut repr‚ciser les
fichiers … comparer avant chaque appel.

&SY2
    $PrintTbl sample tableau1 [tableaux suivants]
&TX
&EX2
    $wsloadvar bist92\bistel
    $wsloadtbl bist92\tbistelf

    $printdest bist92\bistelf1.a2m
    ##show processing french tables  file 1/2
    $printnbdec 1
    $PrintTbl 89:8 HYPEIR
    $PrintTbl 89/88:8 HYPEIIR
    $PrintTbl 89:8 RENAT01 RENAT0D RESL3R RESL31R RESL2
&TX

&NO Il ne peut y avoir d'espace dans l'‚nonc‚ du sample.

&IT Appels successifs

Le dernier appel … ~c$PrintTblFile~C permet d'imprimer des comparaisons de
fichiers. Cependant, l'appel … ~c$PrintTbl~C ou ~c$PrintVar~C "consomme" les
fichiers d‚finis par ~c$PrintTblFile.~C Si on veut faire appel plusieurs
fois … ~c$PrintTbl~C pour comparer des fichiers, il faut repr‚ciser les
fichiers … comparer avant chaque appel.

&EX2
    $PrintTblFile 2 ref.var
    $PrintTblFile 3 var1.var
    $PrintTbl 89:8[1;2;3] HYPEIR
    $PrintTblFile 2 ref.var
    $PrintTbl 89/88:8[1/2] HYPEIIR
&TX
>

<printtblfile>
    PrintTblFile
    ÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette commande permet de d‚finir les fichiers … utiliser lors de
l'impression d'un tableau de comparaison entre plusieurs fichiers
(maximum 4). Le sample de l'impression peut contenir des r‚f‚rences vers
d'autres fichiers que le WS, plac‚es entre crochets (ex. 90:2^[1;2]). Le
fichier [1] est toujours le WS, les autres peuvent ˆtre d‚finis ou dans
la page de saisie ou comme argument dans un rapport.

L'objet de cette fonction est d'indiquer les fichiers r‚f‚renc‚s par
[2], [3], etc.

&FS
Il n'existe pas de fonction ‚quivalente en plein ‚cran : la fonction
##PrintTbl affiche une page de saisie dans laquelle on d‚finit notamment
les noms de fichiers … utiliser lors de l'impression.

&SY2
    ##PrintTbl
&TX

&NFS
Le num‚ro de fichier (2, 3, 4 ou 5) suivi du nom du fichier sont pass‚s
en paramŠtre de la commande.

&SY2
    $PrintTblFile n varfilename
    (n := 2, 3, 4, 5)
&TX
&EX2
    $wsloadvar bist92\bistel
    $wsloadtbl bist92\tbistelf

    $printdest bist92\bistelf1.a2m
    ##show processing french tables  file 1/2
    $PrintNbDec 1
    $PrintTblFile 2 variante
    $PrintTbl 89:8[1;2] HYPEIR
&TX

AprŠs un appel … ~c$PrintTbl,~C il faut red‚finir les fichiers … comparer:
~c$PrintTbl~C "consomme" les d‚finitions de fichiers … comparer.

&NO Il ne peut y avoir d'espace dans l'‚nonc‚ du sample.
>


<viewtbl>
    ViewTbl
    ÄÄÄÄÄÄÄ

&DE
La fonction construit et affiche les tables dans un tableau d‚roulant (ou
tableau d'‚dition).

&FS
La fonction affiche une page de saisie dans laquelle on d‚finit le ou
les paramŠtres de la commande. Cette fonction est d‚crite dans le
"Manuel de l'utilisateur" : Menu Print/Graph, Option "Print Tables".

&SY2
    ##ViewTbl
&TX

&NFS
Le sample d'impression suivi de la liste des tableaux … visualiser sont
pass‚s en paramŠtre de la commande.

&SY2
    $ViewTbl sample tableau [liste de tableaux]
&TX

&EX2

&TX $ViewTbl 90:6 CIIIR0 CIIR

&NO Il ne peut y avoir d'espace dans l'‚nonc‚ du sample.
>

<viewbytbl>
    ViewByTbl
    ÄÄÄÄÄÄÄÄÄ

&DE
Alias de la fonction $ViewTbl.
>

<viewtblfile>
    ViewTblFile
    ÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette commande permet de d‚finir les fichiers … utiliser lors de la
visualisation d'un tableau de comparaison entre plusieurs fichiers (max
4). Le sample de l'impression peut contenir des r‚f‚rences vers d'autres
fichiers que le WS. (ex. 90:2[1;2]). Le fichier [1] est toujours le WS,
les autres peuvent ˆtre d‚finis ou dans la page de saisie ou comme
argument dans un rapport.

&FS
Il n'y a pas de fonction directement ‚quivalente en plein ‚cran : la
fonction d'impression de tableaux permet d'encoder les fichiers …
comparer :

&SY2
    ##ViewTbl
&TX

&NFS
Le num‚ro de fichier (2, 3, 4 ou 5) suivi du nom du fichier sont pass‚s
en paramŠtre de la commande.

&SY2
    $ViewTblFile n varfilename
    (n := 2, 3, 4, 5)
&TX
&EX2
    $wsloadvar bist92\bistel
    $wsloadtbl bist92\tbistelf

    $PrintNbDec 1
    $ViewTblFile 2 variante
    $ViewTbl 89:8[1;2] HYPEIR
&TX

AprŠs un appel … ~c$ViewTbl,~C il faut red‚finir les fichiers … comparer:
~c$ViewTbl~C "consomme" les d‚finitions de fichiers … comparer.
>

<modelsimulate>
    ModelSimulate
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
La fonction lance la simulation d'un modŠle.

Pour simuler un modŠle, il faut que toutes les ‚quations du modŠle
soient pr‚sentes dans le WS d'‚quations, que toutes les variables et
tous les scalaires utilis‚s dans les ‚quations du modŠle soient d‚finis
dans les WS de variables et de scalaires. Les valeurs des exogŠnes et
des scalaires ne peuvent ˆtre NA (Not Available) sur la p‚riode utile.

&FS
La fonction affiche une page de saisie dans laquelle on d‚finit le ou
les paramŠtres de simulation. Cette fonction est d‚crite dans "Manuel de
l'utilisateur".

&SY2
    ##ModelSimulate
&TX

&NFS
Les paramŠtres de la simulation sont d‚finis par la commande annexe
ModelSimulateParms. La simulation proprement dite s'effectue par la
commande ModelSimulate.

Les deux p‚riodes limites (d‚but et fin) et la liste des ‚quations …
simuler sont pass‚es en argument … la fonction. Si la liste est vide, la
simulation sera effectu‚e pour le modŠle constitu‚ de toutes les
‚quations en m‚moire.

Les paramŠtres compl‚mentaires peuvent ˆtre d‚finis par la commande
modelsimulateparms d‚crite ci-dessous.

&SY2
    $ModelSimulate periode1 periode2 [liste_d_equations]
&TX

&EX2
    $-filedeletea2m ode.a2m
    $printdest ode.a2m
    ##modelsimulate
    $modelsimulate 1990Y1 1995Y1
    $show test1
    $modelsimulate 1990Y1 1995Y1 PC0 PIF
    $show test2
    $modelsimulate 1990Y1 1995Y1 $LIST
    $show end of report
&TX

>

<modelsimulatescc>
    ModelSimulateSCC
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
La fonction lance la simulation d'un modŠle pr‚alablement d‚compos‚ en CFC et tri‚.

La performance des simulations, sp‚cifiquement lors du d‚marrage (link, sort), est
meilleure avec cette fonction qu'avec $ModelSimulate car le r‚ordonnancement
du modŠle n'a lieu qu'une seule fois et non … chaque lancement de
simulation.

Le processus de simulation a ‚t‚ divis‚ en 2 ‚tapes. La premiŠre
ne s'occupe que du r‚ordonnancement du modŠle ($ModelCalcSCC), la seconde de la simulation.

&FS
La fonction affiche une page de saisie dans laquelle on d‚finit le ou
les paramŠtres de simulation et les listes qui composent le modŠle tri‚.

&SY2
    ##ModelSimulateSCC
&TX

&NFS
La simulation du modŠle se base sur trois listes
pr‚alablement construites par la fonction $ModelCalcSCC (ou … la main).

Sa syntaxe est :

&CO
    $ModelSimulateSCC from to pre inter post
&TX
o— :
&EN from et to d‚terminent le sample de simulation
&EN pre, inter et post sont les listes qui d‚finissent l'ordre d'ex‚cution du modŠle.
>

<modelcalcscc>
    ModelCalcSCC
    ÄÄÄÄÄÄÄÄÄÄÄÄ
Cette fonction permet de d‚composer le modŠle en composantes fortement
connexes (CFC ou SCC pour Strongly Connex Components) et de le r‚ordonnancer.

Trois listes sont donc cr‚‚es : ‚quations pr‚r‚cursives,
interd‚pendantes et postr‚cursives.

Lors du r‚ordonnancement du modŠle, le nombre d'it‚rations de triangulation
(tri) pour le block interd‚pendant doit ˆtre sp‚cifi‚. Cette valeur n'a
‚videmment d'effet que sur la liste des ‚quations interd‚pendantes.

Ces 3 listes ne doivent ˆtre contruites qu'une seule fois pour une version donn‚e du modŠle.

&SY
    $ModelCalcSCC nbtri lstpre lstinter lstpost
&TX
o— :

&EN nbtri est le nombre d'it‚rations de triangulation … effectuer
&EN lst* sont les NOMS des listes destin‚es … contenir les r‚sultats du tri des ‚quations


&IT Choix du nombre de tris
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

AprŠs la d‚compostion en CFC, le bloc interd‚pendant est tri‚ pour augmenter la vitesse
de la simulation. Le nombre de passage de l'algorithme de tri peut ˆtre
sp‚cifi‚ … plusieurs endroits :

&EN Dans l'‚cran de simulation "standard" : paramŠtre "Passes" fix‚
&EN Dans l'‚cran de calcul de la d‚composition du modŠle :  paramŠtre "Triangulation Iterations"
&EN Comme paramŠtre dans la commande rapport $ModelCalcSCC
&EN Comme dernier paramŠtre dans la commande rapport $ModelSimulateParms

&NO Dans les versions ant‚rieures, le nombre de passages de la triangulation sp‚cifi‚ dans
l'‚cran de simulation n'avait pas d'effet

>

<modelsimulateparms>
    ModelSimulateParms
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette commande permet de sp‚cifier les paramŠtres compl‚mentaires
(essentiellement techniques) d'une simulation. La simulation est
effectivement lanc‚e par la commande ModelSimulate.

Les paramŠtres que fixe cette commande sont d‚crits dans le "Manuel de
l'utilisateur".

&FS
Il n'y a pas de fonction sp‚cifique plein ‚cran ‚quivalente. L'‚cran de
simulation appel‚ par ##ModelSimulate contient les paramŠtres de
simulation.

&SY2
    ##ModelSimulate
&TX

&NFS
Cette fonction fixe les paramŠtres suivants de simulation :

&EN seuil de convergence (eps)
&EN paramŠtre de relaxation (relax)
&EN nombre maximum d'it‚rations (maxit)
&EN algorithme de r‚ordonnacement (Connex, Triangulation (= Both) ou None)
&EN valeurs initiales (0 … 4)
&EN2 0 = Y := Y[-1], if Y null or NA
&EN2 1 = Y := Y[-1], always
&EN2 2 = Y := extrapolation, if Y null or NA
&EN2 3 = Y := extrapolation, always
&EN2 4 = Y unchanged
&EN2 5 = Y := Y[-1], if Y is NA
&EN2 6 = Y := extrapolation, if Y is NA
&EN simulation avec debugging
&EN nombre d'it‚rations de tri des composantes connexes
&EN debugging de la sous-it‚ration de Newton-Raphson

&SY2
    $ModelSimulateParms eps relax maxit
			{Connex | Triang | Both | None }
			0 - 4 (starting values)
			{Yes | no }
			nbtri
			{yes | No }
    (eps  := seuil de convergence : nombre r‚el)
    (relax := nombre r‚el compris entre 0.1 et 1)
&TX

&EX2
    $ModelSimulateParms 0.001 0.7 100 Connex 0 Yes 2 No 
&TX




&IT D‚composition et r‚ordonnancement du modŠle
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Un algorithme de r‚ordonnacement (Connex, Triangulation (= Both) ou None) peut ˆtre appliqu‚.
&EN Connex = d‚composition du modŠle en "composantes fortement connexes"
&EN Triangulation ou Both = r‚ordonnancement des blocs pour acc‚l‚rer la convergence
&EN None = le modŠle est laiss‚ tel quel.


&IT Choix du nombre de tris
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

AprŠs la d‚compostion en CFC, le bloc interd‚pendant peut ˆtre r‚ordonnanc‚ (Triangulation ou Both) pour augmenter la vitesse
de la simulation.


&IT Option Debug
ÄÄÄÄÄÄÄÄÄÄÄÄ

L'option debug de la simulation g‚nŠre automatiquement des listes
comprenant des ‚quations pr‚- et post-r‚cursives.

&NO Version 6.36 : l'option debug activait la g‚n‚ration d'un fichier
simul.dbg qui contenait une quantit‚ ‚norme d'informations. Dans cette
version, seules les listes ~c_PRE, _INTER~C et ~c_POST~C (avec la d‚coupe du
modŠle) sont g‚n‚r‚es.


&IT Option Debug sous-it‚ration
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Un fichier de trace
est g‚n‚r‚ si cette option est fix‚e … "Y". Ce fichier se nomme "simul.dbg" et se trouve dans
le directory d'o— IODE a ‚t‚ lanc‚. Le fichier de trace comprend la
liste des ‚quations pour lesquelles une sous-it‚ration (Newton-Raphson)
a ‚t‚ n‚cessaire et le nombre de sous-it‚rations.


>


<modelsimulatesaveniters>
    ModelSimulateSaveNiters
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette commande permet de sauver dans une variable le nombre d'it‚rations
qui ont ‚t‚ n‚cessaires pour chaque p‚riode lors de la derniŠre simulation.

Le nom de la variable r‚sultat est pass‚ comme paramŠtre.

S'il n'y a pas encore eu de simulation dans le cours de la session, la variable est
cr‚‚e avec des valeurs NA.

Si une simulation a d‚j… eu lieu, les valeurs des p‚riodes non simul‚es sont fix‚es … 0 et
celles des p‚riodes simul‚es contiennent le nombre d'it‚rations n‚cessaires …
la convergence pour cette p‚riode. S'il n'y a pas de convergence, la valeur est celle du
paramŠtre MAXIT de la simulation.

&FS
Il n'y a pas de fonction sp‚cifique plein ‚cran ‚quivalente.


&NFS

&SY2
    $ModelSimulateSaveNiters varname
&TX

&EX2
    $ModelSimulateSaveNiters SIM_NITERS
&TX
>


<modelsimulatesaveNorms>
    ModelSimulateSaveNorms
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette commande permet de sauver dans une variable le seuil de convergence (la norme) atteint
pour chaque p‚riode lors de la derniŠre simulation.

Le nom de la variable r‚sultat est pass‚ comme paramŠtre.

S'il n'y a pas encore eu de simulation dans le cours de la session, la variable est
cr‚‚e avec des valeurs NA.

Si une simulation a d‚j… eu lieu, les valeurs des p‚riodes non simul‚es sont fix‚es … 0 et
celles des p‚riodes simul‚es contiennent le seuil de convergence
pour cette p‚riode.

&FS
Il n'y a pas de fonction sp‚cifique plein ‚cran ‚quivalente.

&NFS

&SY2
    $ModelSimulateSaveNorms varname
&TX

&EX2
    $ModelSimulateSaveNiters SIM_NORMS
&TX
>

<idtexecute>
    IdtExecute
    ÄÄÄÄÄÄÄÄÄÄ

&DE
Une identit‚ est une forme LEC qui indique comment une s‚rie doit ˆtre
construite. Le programme d'ex‚cution calcule toutes les identit‚s
demand‚es et sauve dans le WS de s‚ries le r‚sultat des calculs.

Ces identit‚s sont ordonnanc‚es dans l'ordre logique d'ex‚cution : si A
d‚pend de B, B sera calcul‚ avant A. L'utilisateur n'a pas … se
pr‚occuper de cet ordonnancement. Si une boucle est d‚tect‚e dans
les identit‚s, l'ex‚cution ne peut commencer et un message est affich‚.

&FS
Cette fonction est d‚crite dans le "Manuel de l'utilisateur". Elle est
appel‚e par :

&SY2
    ##IdtExecute
&TX

&NFS
Les deux p‚riodes limites (d‚but et fin) et la liste des identit‚s …
calculer sont pass‚es en argument … la fonction. Si la liste est vide, le
calcul sera effectu‚e pour toutes les identit‚s en m‚moire.

Les paramŠtres compl‚mentaires peuvent ˆtre d‚finis par les commandes
IdtExecuteVarFiles et IdtExecuteSclFiles d‚crites ci-dessous.

&SY2
    $IdtExecute periode1 periode2 [liste_d_identites]
&TX

&EX2
    $IdtExecute 1953Y1 1996Y1 IDT1 IDT2
    $IdtExecute 1953Y1 1996Y1 $IDTLIST
&TX
>

<idtexecutevarfiles>
    IdtExecuteVarfiles
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette commande fait partie d'un groupe de commandes permettant de
sp‚cifier les paramŠtres compl‚mentaires (fichiers de recherche) du
calcul des identit‚s. Les commandes associ‚es sont IdtExecute et
IdtExecuteSclFiles. Le calcul des identit‚s est effectivement lanc‚ par
la commande IdtExecute.

La fonction permet d'introduire les noms de fichiers o— les s‚ries
r‚f‚renc‚es dans les identit‚s doivent ˆtre recherch‚s.

Lorsqu'une s‚rie ou un scalaire utile … l'ex‚cution d'une identit‚ est
rencontr‚, les fichiers indiqu‚s sont analys‚s … tour de r“le. DŠs que
l'objet recherch‚ est trouv‚, il est amen‚ en m‚moire pour la dur‚e du
calcul. Il est ensuite d‚truit, sauf s'il ‚tait initialement en m‚moire.

Si un objet - variable ou scalaire - n'est ni en WS, ni dans un des
fichiers indiqu‚s, le processus de calcul s'arrˆte.

Cette fonction n'a tout son sens que lorsque la commande IdtExecute est
utilis‚e en mode ligne de commande.

&NO Si la liste des fichiers est vide, seul le WS courant de variables
    est utilis‚ pour la recherche. Sinon, il faut sp‚cifier WS (en
    majuscules) dans la liste des fichiers, … la position de recherche
    souhait‚e.

&FS
Il n'y a pas de fonction plein ‚cran ‚quivalente : l'‚cran
d'ex‚cution d'identit‚s contient ces paramŠtres.

&SY2
    ##IdtExecute
&TX

&NFS
La liste des fichiers est pass‚e en paramŠtre de la fonction.

&SY2
    $IdtExecuteVarFiles fichier [liste de fichiers]
&TX

&EX2
    $IdtExecuteVarFiles maribel.var WS test.var
&TX
>

<idtexecutesclfiles>
    IdtExecuteSclFiles
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&IT Description
ÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande fait partie d'un groupe de commandes permettant de
sp‚cifier les paramŠtres compl‚mentaires (fichiers de recherche)
du calcul des identit‚s. Les commandes associ‚es sont IdtExecuteVarFiles
et IdtExecute. Le calcul des identit‚s est effectivement lanc‚
par la commande IdtExecute.

La fonction permet d'introduire les noms de fichiers o— les scalaires
r‚f‚renc‚s dans les identit‚s doivent ˆtre recherch‚s.

Lorsqu'un scalaire utile … l'ex‚cution d'une identit‚ est rencontr‚, les
fichiers indiqu‚s sont analys‚s … tour de r“le. DŠs que l'objet
recherch‚ est trouv‚, il est amen‚ en m‚moire pour la dur‚e du calcul.
Il est ensuite d‚truit, sauf s'il provient du WS actif.

Si un objet - variable ou scalaire - n'est ni en WS, ni dans un des
fichiers indiqu‚s, le processus de calcul s'arrˆte.

Cette fonction n'a tout son sens que lorsque la commande IdtExecute est
utilis‚e en mode ligne de commande.

&NO Si la liste des fichiers est vide, seul le WS courant de scalaires
    est utilis‚ pour la recherche. Sinon, il faut sp‚cifier WS (en
    majuscules) dans la liste des fichiers, … la position de recherche
    souhait‚e.

&FS
Il n'y a pas de fonction plein ‚cran ‚quivalente : l'‚cran
d'ex‚cution d'identit‚s contient ces paramŠtres.

&SY2
    ##IdtExecute
&TX

&NFS
La liste des fichiers est pass‚ en paramŠtre de la fonction.

&SY2
    $IdtExecuteSclFiles fichier [liste de fichiers]
&TX

&EX2
    $IdtExecuteSclFiles WS maribel.scl test.scl
&TX
>

<reportexec>
    ReportExec
    ÄÄÄÄÄÄÄÄÄÄ

&DE
La commande provoque l'ex‚cution d'un rapport "fils". La fonction est
r‚cursive et autorise plusieurs niveaux de sous-rapports. Lorsque les
rapports "fils" sont termin‚s sans erreur critique, l'ex‚cution continue
avec les commandes du rapport de niveau sup‚rieur.

&FS
La fonction affiche une page de saisie dans laquelle on d‚finit le ou
les paramŠtres de la commande.

&MU

&SY2
    ##ReportExec
&TX

&NFS
Le rapport … ex‚cuter et ses arguments sont pass‚s en paramŠtre de la
commande.

&SY2
    $ReportExec rapport [arguments]
&TX

&EX2
    $ReportExec mysimul.rep 1990Y1 1992Y1
&TX

La fonction $ReportExec accepte une liste comme argument :
&CO
    $ReportExec test $LISTE
&TX
re‡oit comme argument les ‚l‚ments de la liste $LISTE et permet donc
d'ex‚cuter un rapport type sur un nombre initialement inconnu
d'arguments. Par exemple, pour remplacer tous les 0 par des NA pour la
liste $LISTE de variables, il suffit de faire :

&CO
    FICHIER ZEROTONA.REP
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
	$label again
	$goto fin {%0%=0}
	##show Series trait‚e : %1%
	$DataCalcVar %1% if(%1%=0, 1/0, %1%)
	$shift
	$goto again
	$label fin
&TX
La commande
&CO
	$ReportExec zerotona $LISTE
&TX
replacera les 0 par des NA pour les s‚ries de la liste $LISTE.
>

<reportedit>
    ReportEdit
    ÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction permet de cr‚er ou de modifier un rapport.

&FS
La fonction affiche une page de saisie dans laquelle on d‚finit le ou
le nom du rapport … ‚diter.

&MU

&SY2
    ##ReportEdit
&TX

&NFS
Le fichier … ‚diter est pass‚ en paramŠtre de la commande. L'‚diteur MMT
est lanc‚ pour l'‚dition du fichier. L'extension .rep peut ˆtre omise.

&SY2
    $ReportEdit filename
&TX

&EX2
    $ReportEdit bist92\bistel.rep
&TX
>

<printvar>
    PrintVar
    ÄÄÄÄÄÄÄÄ

&DE
Cette commande permet la construction et l'impression des tableaux de
comparaison de s‚ries en format A2M. Les formes LEC sont admises et
doivent ˆtre s‚par‚es par des points-virgules.

Pour d‚finir les fichiers … comparer, il faut utiliser la fonction
$<PrintTblFile> comme pour les tableaux et graphiques.


&FS
&MU

&SY2
    ##PrintVar
&TX

&NFS
&SY2
    $PrintVar gsample var1;var2
&TX

&EX2
    $PrintVar (80Y1/79Y1):10 X+Y;Z;ln(X)
&TX

&NO Il ne peut y avoir de blancs dans le sample g‚n‚ralis‚.
>

<viewvar>
    ViewVar
    ÄÄÄÄÄÄÄ

&DE
Cette commande permet la visualisation des tableaux de comparaison de
s‚ries en format A2M.

&FS
Cette fonction n'existe pas en mode plein ‚cran.

&NFS
&SY2
    $ViewVar gsample lec1 [lec2 ...]

    o— gsample est un sample g‚n‚ralis‚ sans blanc
       lec1 et lec2 sont des formes LEC sans blanc
&TX

&EX2
    $ViewVar (80Y1/79Y1):10  X+Y  Z  ln(X)
&TX
&NO Il ne peut y avoir de blancs dans le sample g‚n‚ralis‚.
>

<ViewGr>
    ViewGr
    ÄÄÄÄÄÄ

&DE
Cette fonction permet de visualiser des tableaux sous forme de graphique.
Plusieurs graphiques peuvent ˆtre affich‚s sur le mˆme ‚cran. Le langage
est d‚fini par $PrintLang.

&FS
Cette fonction est d‚crite en d‚tail dans le "Manuel de l'utilisateur".

&SY2
    ##ViewGr
&TX

&NFS
&SY2
    $ViewGr gsample tbl1[+tbl2] tbl3 ...

    o— gsample est un sample g‚n‚ralis‚ sans blanc
       tbl1, tbl2 et tbl3 sont des noms de tableaux
&TX
Lorsque deux tableaux sont associ‚s par le signe +, ils sont affich‚s
sur le mˆme ‚cran sous forme de deux graphiques s‚par‚s.

&NO Il ne peut y avoir de blancs dans le sample g‚n‚ralis‚.
>

<PrintGrAll>
    PrintGrAll
    ÄÄÄÄÄÄÄÄÄÄ

Cette fonction est annul‚e … partir de la version 5 de IODE. Elle est
remplac‚e par la fonction ~c$PrintGr~C.
>

<PrintGrData>
    PrintGrData
    ÄÄÄÄÄÄÄÄÄÄÄ

Cette fonction est annul‚e … partir de la version 5 de IODE.

>


<PrintGrWin>
    PrintGrWin
    ÄÄÄÄÄÄÄÄÄÄ

Cette fonction est annul‚e … partir de la version 5 de IODE.

>


<modelexchange>
    ModelExchange
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction permet de d‚finir ou d'annuler les ‚changes endogŠnes-exogŠnes avant
de lancer la simulation d'un modŠle avec recherche d'objectif (Goal
seeking) :

&FS
Cette fonction fait partie de la fonction de simulation :

&SY2
    ##ModelSimulate
&TX

&NFS
&SY2
    $ModelExchange eqname1-varname1,eqname2-varname2,...

    o— eqname1 et eqname2 sont des noms d'‚quations (et donc
			de variables endogŠnes)
       varname1 et varname2 sont des noms d'exogŠnes
&TX

Pour annuler l'‚change ENDO-EXO, il suffit d'appeler la fonction sans paramŠtre :
&CO
    $ModelExchange
&TX
>
<modelcompile>
    ModelCompile
    ÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction recompile une liste d'‚quations ou toutes les ‚quations
si aucune liste n'est sp‚cifi‚e. Elle est uniquement utile si des
‚quations font appel … des macros dans leur forme LEC.

&FS
&MU
&SY2
    ##ModelCompile
&TX
&NFS
&SY2
    $ModelCompile  [eqname1, eqname2, ... ]

    o— eqname1 et eqname2 sont des noms d'‚quations.
&TX
>
<idtexecutetrace>
    IdtExecuteTrace
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
La trace de la construction des identit‚s peut ˆtre sauv‚e dans le
fichier A2M courant d‚fini par $PrintDest. Les informations utiles (par
exemple, le fichier d'origine d'une variable utilis‚e pour construire
l'identit‚) seront ‚num‚r‚es dans le fichier "Trace file".

&FS
&NEQ
&NFS
Selon le paramŠtre, la trace est sauv‚e ou non lors du prochain appel …
la commande $IdtExecute.

&CO
    $IdtExecuteTrace  {Yes | No}
&TX
>

<eqsestimate>
    EqsEstimate
    ÄÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction r‚estime une ‚quation ou un bloc d'‚quations.

&FS
Cette fonction est d‚taill‚e dans la manuel de l'utilisateur. Elle
permet ‚galement de d‚finir et de modifier une ‚quation.

&SY2
    ##EqsEstimate
&TX

&NFS
Seule la p‚riode peut ˆtre adapt‚e dans cette fonction. Les autres
paramŠtres (m‚thode, instruments, etc) sont ceux d‚finis actuellement
dans l'‚quation. D'autres fonctions permettent de modifier les
instruments, la m‚thode : il s'agit des fonctions EqsSet....

&SY2
    $EqsEstimate perfrom perto eqname1 ...
&TX

&TI R‚sultats explicites
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
En fin d'estimation, certaines variables et scalaires sont automatiquement
cr‚es si le processus a converg‚. Ces variables et scalaires peuvent
ˆtre exploit‚s … des fins de calculs et, comme ils s'agit d'objets
faisant partie du WS, peuvent ˆtre sauv‚s pour une utilisation future.

Il s'agit des tests r‚sultant de la derniŠre estimation sont sauv‚s
dans des scalaires. Il en va de mˆme pour les r‚sidus, membres de
gauche et de droite des ‚quations.

Les tests portent les noms suivants (e0_* pour la premiŠre ‚quation du
bloc, e1_* pour la deuxiŠme, ...) :

&EN e0_n         : nombre de p‚riode du sample
&EN e0_k         : nombre de coefficients estim‚s
&EN e0_stdev     : std dev des r‚sidus
&EN e0_meany     : moyenne de Y
&EN e0_ssres     : somme du carr‚s des r‚sidus
&EN e0_stderr    : std error
&EN e0_stderrp   : std error %
&EN e0_fstat     : F-Stat
&EN e0_r2        : R carr‚
&EN e0_r2adj     : R carr‚ ajust‚
&EN e0_dw        : Durbin-Watson
&EN e0_loglik    : Log Likelihood

    Les s‚ries calcul‚es sont ‚galement sauv‚es dans une variable
    sous les noms :

&EN _YCALC0 pour le membre de droite de la premiŠre ‚quation du
	  bloc, _YCALC1 pour la deuxiŠme ‚quation, etc.

&EN _YOBS0 pour le membre de gauche de la premiŠre ‚quation du
	  bloc, _YOBS1 pour la deuxiŠme ‚quation, etc.

&EN _YRES0 pour les r‚sidus de la premiŠre ‚quation du bloc, ...

En dehors du sample d'estimation, les valeurs de la s‚rie sont --.
>

<EqsStepWise>
    EqsStepWise
    ÄÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction estime un bloc d'‚quations et recherche les meilleurs tests
possibles pour toutes les combinaisons de coefficients possibles.

&SY2
    $EqsStepWise from to eqname leccond {r2|fstat}
	from to : p‚riode d'estimation
	eqname  : ‚quation … estimer
	leccond : condition d'acceptation
&TX
>

<EqsSetMethod>
    EqsSetMethod
    ÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction fixe la m‚thode d'estimation d'une liste d'‚quations.
Elle est utile pour changer de m‚thode d'estimation dans le cours de
l'ex‚cution d'un rapport.

&FS
&NEQ
&NFS
La fonction modifie la m‚thode pour toutes les ‚quations dont le nom est
pass‚ en paramŠtre.

&SY2
    $EqsSetMethod   {0|1|2|3} eqname1 eqname2 ...

	o— 0 indique la m‚thode LSQ (moindres carr‚s)
	   1 indique la m‚thode Zellner
	   2 indique la m‚thode INF (2 stages avec instruments)
	   3 indique la m‚thode GLS (3 stages avec instruments)
	   4 indique la m‚thode MAXLIK (Maximum likelihood - BHHH - … partir de la version 6.21)

	   eqname1 .. sont des noms d'‚quations existantes
&TX

&EX2
    $DataUpdateEqs E E := c1 + c2 * F
    $EqsSetCmt E Ceci est le commentaire de E
    $EqsSetSample 1971Y1 1990Y1 E
    $EqsSetMethod 1 B E
    $EqsSetInstrs E instruments
    $EqsSetBloc B E
&TX
>

<EqsSetSample>
    EqsSetSample
    ÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction fixe la p‚riode d'estimation d'une liste d'‚quations.
Elle est utile pour changer de p‚riode d'estimation dans le cours de
l'ex‚cution d'un rapport.

&FS
&NEQ
&NFS
La fonction modifie la p‚riode pour toutes les ‚quations dont le nom est
pass‚ en paramŠtre.

&SY2
    $EqsSetSample from to eqname1 eqname2 ...

	o— from et to sont les p‚riodes limites du sample
	   eqname1 .. sont des noms d'‚quations existantes
&TX

&EX2
    $DataUpdateEqs E E := c1 + c2 * F
    $EqsSetCmt E Ceci est le commentaire de E
    $EqsSetSample 1971Y1 1990Y1 E
    $EqsSetMethod 1 B E
    $EqsSetInstrs E instruments
    $EqsSetBloc B E
&TX
&TX
>

<EqsSetBloc>
    EqsSetBloc
    ÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction fixe le bloc d'estimation d'une liste d'‚quations.
Elle est utile pour fixer le bloc d'estimation dans le cours de
l'ex‚cution d'un rapport.

&FS
&NEQ
&NFS
La fonction fixe le bloc de toutes les ‚quations dont le nom est
pass‚ en paramŠtre. Le bloc est la liste d'‚quations elle-mˆme.

&SY2
    $EqsSetBloc eqname1 eqname2 ...

	o— eqname1 .. sont des noms d'‚quations existantes
&TX

&EX2
    $DataUpdateEqs E E := c1 + c2 * F
    $EqsSetCmt E Ceci est le commentaire de E
    $EqsSetSample 1971Y1 1990Y1 E
    $EqsSetMethod 1 B E
    $EqsSetInstrs E instruments
    $EqsSetBloc B E
&TX
&TX
>

<EqsSetInstrs>
    EqsSetInstrs
    ÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction fixe les instruments … utiliser pour l'estimation d'une
‚quation.

&FS
&NEQ
&NFS
La fonction modifie les instruments pour l'‚quation sp‚cifi‚e.

&SY2
    $EqsSetInstrs eqname lec_intr_1;lec_instr_2;...

	o— eqname est le nom de l'‚quation
	   lec_instr_1, ... sont les formes lec des instruments
&TX

&EX2
    $DataUpdateEqs E E := c1 + c2 * F
    $EqsSetCmt E Ceci est le commentaire de E
    $EqsSetSample 1971Y1 1990Y1 E
    $EqsSetMethod 1 B E
    $EqsSetInstrs E instruments
    $EqsSetBloc B E
&TX
>

<EqsSetCmt>
    EqsSetCmt
    ÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction fixe le commentaire d'une ‚quation.

&FS
&NEQ
&NFS

&SY2
    $EqsSetCmt eqname comment

	o— eqname est le nom de l'‚quation
	   comment est une texte libre
&TX

&EX2
    $DataUpdateEqs E E := c1 + c2 * F
    $EqsSetCmt E Ceci est le commentaire de E
    $EqsSetSample 1971Y1 1990Y1 E
    $EqsSetMethod 1 B E
    $EqsSetInstrs E instruments
    $EqsSetBloc B E
&TX
>

<PrintA2mAppend>
    PrintA2mAppend
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
    Permet de ne pas vider le fichier a2m avant l'impression.
    Par d‚faut, les fichiers r‚sultat des impressions sont
    vid‚s lors d'une nouvelle impression.
    Dans le seul cas des fichiers a2m, il est possible d'‚viter
    ce traitement par d‚faut et d'accumuler de la sorte plusieurs
    impressions successives dans un seul et mˆme fichier.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences/A2M",
    accessible dans la fonction "Print Setup".

&NFS

&SY2
    $PrintA2mAppend [NO|Yes]
&TX
&EX2
    $PrintA2mAppend Yes
    $PrintDest test.a2m A2M
&TX
Ces deux appels d‚finissent le fichier ~ctest.a2m~C comme fichier
r‚sultat des impressions suivantes et ne vide pas ce fichier.
>

<PrintFont>
    PrintFont
    ÄÄÄÄÄÄÄÄÄ

&DE
Fixe la police de caractŠres pour l'impression des paragraphes de texte.
En plus de la famille, la taille de base (en points) et l'incr‚ment de
la taille peuvent ‚galement ˆtre sp‚cifi‚s.

La taille de base correspond aux paragraphes de textes, l'incr‚ment
indique de combien de points chaque niveau de titre doit ˆtre augment‚.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&NO Cette valeur n'a d'effet qu'avant la fonction $PrintDest et plus en
    cours de rapport.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup".

&NFS

&SY2
   $PrintFont Times|Helvetica|Courier|Bookman|Palatino [size [incr]]

&TX

Les polices Bookman et Palatino ne sont pas exploitables pour toutes
les destinations d'impression.

Le premier caractŠre du nom de la police suffit.

Une fois cette valeur fix‚e, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv‚e dans le panneau de configuration des impressions, elle reste
‚galement valable pour les sessions suivantes de IODE.

&EX2
   $PrintFont Times 10 2
&TX
Fixe la taille des paragraphes par_1, enum_1, etc … 10 points, le
paragraphe tit_2 … 12 points, tit_1 … 14 points et tit_0 … 18 points.
>

<PrintTableFont>
    PrintTableFont
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Fixe la police de caractŠres pour les tableaux seuls. Cette police
peut donc ˆtre diff‚rente de celle du reste du texte, … la fois en taille
et en famille.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&NO Cette fonction n'a d'effet qu'avant la fonction $PrintDest et plus en
    cours de rapport.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff‚rente.

&NFS

&SY2
   $PrintTableFont Times|Helvetica|Courier|Bookman|Palatino [size]
&TX

Une fois cette valeur fix‚e, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv‚e dans le panneau de configuration des impressions, elle reste
‚galement valable pour les sessions suivantes de IODE.

&EX2
   $PrintTableFont Palatino 8
&TX
Fixe la police pour tous les tableaux … Palatino, 8 points.

>

<PrintTableBox>
    PrintTableBox
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Fixe la largeur du cadre des tableaux en points. Cette valeur peut
est nulle pour ‚viter les cadres.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff‚rente.

&NFS

&SY2
   $PrintTableBox  n
&TX

Une fois cette valeur fix‚e, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv‚e dans le panneau de configuration des impressions, elle reste
‚galement valable pour les sessions suivantes de IODE.

&EX2
   $PrintTableBox  0
&TX
Supprime le cadre des tableaux.
>

<PrintTableColor>
    PrintTableColor
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Permet d'utiliser ou non la couleur dans les tableaux.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff‚rente.

&NFS

&SY2
   $PrintTableColor [NO|Yes]
&TX

Une fois cette valeur fix‚e, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv‚e dans le panneau de configuration des impressions, elle reste
‚galement valable pour les sessions suivantes de IODE.

&EX2
   $PrintTableColor No
&TX
Supprime la couleur dans l'impression des tableaux.
>

<PrintTableWidth>
    PrintTableWidth
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Fixe la largeur des tableaux en Frame et Rtf. Les paramŠtres doivent
ˆtre exprim‚s en mm. Trois valeurs peuvent ˆtre pass‚es :

&EN la largeur totale
&EN la largeur de la premiŠre colonne
&EN la largeur des colonnes suivantes

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff‚rente.

&NFS

&SY2
   $PrintTableWidth width [col1 [coln]]
&TX

Une fois cette valeur fix‚e, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv‚e dans le panneau de configuration des impressions, elle reste
‚galement valable pour les sessions suivantes de IODE.

&EX2
   $PrintTableWidth 160 60 15
&TX
>
<PrintTableBreak>
    PrintTableBreak
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Permet ou non les coupures des tableaux sur plusieurs pages: si un tableau
est plac‚ sur la page de telle sorte qu'il doivent ˆtre imprim‚ sur deux
pages, un saut de page automatique est g‚n‚r‚ avant le tableau.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff‚rente.

&NFS

&SY2
   $PrintTableBreak [NO|Yes]
&TX

Une fois cette valeur fix‚e, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv‚e dans le panneau de configuration des impressions, elle reste
‚galement valable pour les sessions suivantes de IODE.

&EX2
   $PrintTableBreak Yes
&TX
Les coupures des tableaux sont autoris‚es lors de l'impression.
>

<PrintTablePage>
    PrintTablePage
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Force un saut de page avant chaque tableau.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff‚rente.

&NFS

&SY2
   $PrintTablePage  [NO|Yes]
&TX

Une fois cette valeur fix‚e, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv‚e dans le panneau de configuration des impressions, elle reste
‚galement valable pour les sessions suivantes de IODE.

&EX2
   $PrintTablePage Yes
&TX
Force un saut de page pour chaque tableau.
>

<PrintBackground>
    PrintBackground
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Fixe la couleur du background (tableaux et graphiques). Un tableau
sera hachur‚ si la valeur des attributs graphiques "Shadow" est
fix‚e … Yes.

Pour ‚viter les background dans tous les tableaux, on peut utiliser
comme background White.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff‚rente.

&NFS

&SY2
   $PrintBackground Black|Blue|Magenta|Cyan|Red|Green|Yellow|White
&TX

Une fois cette valeur fix‚e, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv‚e dans le panneau de configuration des impressions, elle reste
‚galement valable pour les sessions suivantes de IODE.

&EX2
   $PrintBackground Blue
&TX
La couleur de la partie hachur‚e en background des graphiques et tableaux
est fix‚e … bleu.
>
<PrintGraphBox>
    PrintGraphBox
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Fixe la largeur du cadre de graphiques en points.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff‚rente.

&NFS

&SY2
   $PrintGraphBox n
&TX

Une fois cette valeur fix‚e, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv‚e dans le panneau de configuration des impressions, elle reste
‚galement valable pour les sessions suivantes de IODE.

&EX2
   $PrintGraphBox 2
&TX
Les graphiques suivants auront un cadre ‚pais.
>
<PrintGraphBrush>
    PrintGraphBrush
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Fixe la densit‚ du background des graphiques. La valeur est exprim‚e en
pourcents.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff‚rente.

&NFS

&SY2
   $PrintGraphBrush pct|Yes
&TX

Une fois cette valeur fix‚e, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv‚e dans le panneau de configuration des impressions, elle reste
‚galement valable pour les sessions suivantes de IODE.

&EX2
   $PrintGraphBrush 10
&TX
La densit‚ de la "brosse" de hachure est fix‚e … 10%. La couleur
est d‚termin‚e par la fonction $PrintBackground.
>

<PrintGraphSize>
    PrintGraphSize
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Fixe la taille des graphiques en mm et celle de la police utilis‚e.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff‚rente.

&NFS

&SY2
   $PrintGraphSize  width [height] [fontsize]
&TX
o— :
&EN ~cwidth~C et ~cheight~C sont exprim‚s en mm.
&EN ~cfontsize~C  est exprim‚ en points

Une fois cette valeur fix‚e, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv‚e dans le panneau de configuration des impressions, elle reste
‚galement valable pour les sessions suivantes de IODE.

&EX2
   $PrintGraphSize 150 60 8
&TX
Les graphiques suivants occuperont un cadre de 15 cm sur 6 cm et les textes auront
8 points de corps.
>

<PrintGraphPage>
    PrintGraphPage
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Force un saut de page avant chaque graphique.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff‚rente.

&NFS

&SY2
   $PrintGraphPage [NO|Yes]
&TX

Une fois cette valeur fix‚e, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv‚e dans le panneau de configuration des impressions, elle reste
‚galement valable pour les sessions suivantes de IODE.

&EX2
   $PrintGraphPage Yes
&TX
Chaque graphique suivant sera plac‚ en d‚but de page.
>

<PrintRtfHelp>
    PrintRtfHelp
    ÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
G‚nŠre un fichier RTF pour un help Windows. Ce fichier pourra ˆtre
compil‚ par la suite … l'aide du compilateur HCW de Microsoft.

Les fichiers RTF g‚n‚r‚s de la sorte ne sont pas identiques …
ceux g‚n‚r‚s pour WinWord.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff‚rente.
&NFS

&SY2
   $PrintRtfHelp [YES|No]
&TX

Une fois cette valeur fix‚e, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv‚e dans le panneau de configuration des impressions, elle reste
‚galement valable pour les sessions suivantes de IODE.


&EX2
   $PrintRtfHelp YES
&TX
Le fichier suivant d‚fini par $PrintDest ... RTF sera du type Help
Windows.
>

<PrintRtfTopic>
    PrintRtfTopic
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Cr‚e un nouveau topic pour un fichier d'aide Windows (ou a2m).
Ce topic fera partie de la table des matiŠres automatique g‚n‚r‚s par
IODE. Le niveau hi‚rarchique du topic est fix‚ par la commande
~c$PrintRtfLevel~C.

Le texte du topic sera ‚galement repris comme titre.

&FS
&NEQ
&NFS

&SY2
   $PrintRtfTopic titre du topic
&TX

&EX2
   $PrintRtfTopic Tableaux r‚capitulatifs
&TX
G‚nŠre en a2m (voir syntaxe du langage a2m) :

&CO
    .topic [num‚ro_auto] [level] Tableaux r‚capitulatifs
    .par1 tit_[level]
    Tableaux r‚capitulatifs

&TX
Si le fichier g‚n‚r‚ est un fichier a2m, la commande ~c.topic~C n'est
exploit‚e que dans le cas d'une traduction en Help Windows.
Par contre, ~c.par1~C ~ctit_....~C et le texte du topic g‚n‚reront …
l'impression un titre.
>
<PrintRtfLevel>
    PrintRtfLevel
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Change le niveau hi‚rarchique des topics suivants. Ce changement de
niveau permet de g‚n‚rer des indentations dans la table de matiŠres du
fichier d'aide r‚sultat.

&FS
&NEQ
&NFS

&SY2
   $PrintRtfLevel [+|-|n]
&TX

&EX2
   $PrintRtfLevel +
&TX
Le niveau du topic suivant sera incr‚ment‚ de 1, cr‚ant de la sorte
une indentation dans la table des matiŠres g‚n‚r‚e.
>
<PrintRtfTitle>
    PrintRtfTitle
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Fixe le titre du fichier d'aide Windows.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences/RTF"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup".
&NFS

&SY2
   $PrintRtfTitle titre du help
&TX

&EX2
   $PrintRtfTitle ModŠle Hermes
&TX
>
<PrintRtfCopyright>
    PrintRtfCopyright
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Fixe le texte du copyright de l'aide Windows. Ce texte apparaŒt dans
l'option Help du programme Winhelp.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences/RTF"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup".
&NFS

&SY2
   $PrintRtfCopyright copyright text
&TX

&EX2
   $PrintRtfCopyright (c) Bureau f‚d‚ral du Plan - 1998
&TX
>
<PrintParanum>
    PrintParanum
    ÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Permet de num‚roter les titres lors de l'impression (1., 1.1, ...).

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&NO Cette fonction n'a d'effet qu'avant la fonction $PrintDest et plus en
    cours de rapport.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff‚rente.

&NFS

&SY2
   $PrintParanum NO
   $PrintParanum [NO|Yes]
&TX

&EX2
   $PrintParanum NO
&TX
Supprime la num‚rotation automatique des titres.
>
<PrintPageHeader>
    PrintPageHeader
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Fixe le titre des pages imprim‚es. Les caractŠres %d sont remplac‚s par
le num‚ro de la page courante.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print
    Preferences/Printer", accessible dans la fonction "Print Setup".

&NFS

&SY2
   $PrintPageHeader titre des pages suivantes
&TX

&EX2
   $PrintPageHeader ModŠle Modtrim
&TX
>
<PrintPageFooter>
    PrintPageFooter
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Fixe la footnote des pages imprim‚es. Les caractŠres %d sont remplac‚s
par le num‚ro de la page courante.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print
    Preferences/Printer", accessible dans la fonction "Print Setup".

&NFS

&SY2
   $PrintPageFooter  footnote des pages suivantes
&TX

&EX2
   $PrintPageFooter - Page %d -
&TX
>

<SetPrinter>
    SetPrinter
    ÄÄÄÄÄÄÄÄÄÄ
&DE
Fixe l'imprimante Windows par d‚faut pour les impressions suivantes.

&FS
    Cette option est modifiable dans Windows (Control Panel).
&NFS

&SY2
   $SetPrinter printer_name
&TX

&EX2
   $SetPrinter 6_qms
&TX
>

<PrintOrientation>
    PrintOrientation
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Fixe l'orientation de l'imprimante Windows courante.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print
    Preferences/Printer", accessible dans la fonction "Print Setup".

&NFS

&SY2
   $PrintOrientation {Portrait|Landscape}
&TX

&EX2
   $PrintOrientation Portrait
&TX
>

<PrintDuplex>
    PrintDuplex
    ÄÄÄÄÄÄÄÄÄÄÄ
&DE
Fixe le mode recto-verso de l'imprimante Windows courante.

Cette fonction modifie ‚galement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print
    Preferences/Printer", accessible dans la fonction "Print Setup".

&NFS

&SY2
   $PrintDuplex {Simplex|Duplex|VerticalDuplex}
&TX

&EX2
   $PrintDuplex Duplex
&TX
>

<PrintHtmlHelp>
    PrintHtmlHelp
    ÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Sp‚cifie que la prochaine g‚n‚ration de fichier Html devra se faire pour
un HtmlHelp de Windows. Ce fichier pourra ˆtre compil‚ par la suite …
l'aide du compilateur HHC de Microsoft.

&FS
    Cette fonction n'existe pas sous forme interactive.


&NFS
&SY2
   $PrintHtmlHelp [YES|No]
&TX

&EX2
   $PrintHtmlHelp YES
   $A2mToHtml myfile.a2m myfile.htm
&TX
>

<PrintHtmlStrip>
    PrintHtmlStrip
    ÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Sp‚cifie que la prochaine g‚n‚ration de fichier Html ne doit pas inclure
les sections de header et de footer HTML (<<HEAD>>, <<BODY>>, <</BODY>>,
etc). Seul le texte proprement dit est traduit en vue d'ˆtre incorpor‚
plus tard manuellement dans un fichier HTML.

&FS
    Cette fonction n'existe pas sous forme interactive.


&NFS
&SY2
   $PrintHtmlStrip [YES|No]
&TX

&EX2
   $PrintHtmlStrip YES
   $PrintDest test.htm HTML
   $PrintTbl ....
&TX
>

<DataRasVar>
    DataRasVar
    ÄÄÄÄÄÄÄÄÄÄ

Vertrekkende van de waarden van variabelen beantwoordend aan pattern,
worden de waarden verdeeld via een RAS methode.

&FS
    Cette fonction n'existe pas sous forme interactive.

&NFS
&SY2
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
>


<DataPatternXXX>
    DataPatternXXX
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Deze functie cre‰ert lijsten met daarin de namen van objecten
die voldoen aan een opgegeven patroon.

&FS
    Cette fonction n'existe pas sous forme interactive.

&NFS
&SY2
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
>

<WsClearAll>
    WsClearAll
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Maakt alle WS-en leeg.
>
