<datalistsort>
    DataListSort
    컴컴컴컴컴컴
&DE
Cette fonction permet un tri alphanum굍ique sur le contenu d'une liste.

&FS
Les deux champs � remplir permettent de pr괹iser le nom de la liste �
trier et le nom de la liste r굎ultat.

&SY2
    ##DataListSort
&TX

&MU

&NFS
La commande prend deux param둻res, le premier pr괹ise le nom de la liste �
trier et le deuxi둴e le nom de la liste r굎ultat.

&SY2
    $DataListSort liste liste_tri괻
&TX
>
<datacompareeps>
    DataCompareEps
    컴컴컴컴컴컴컴

&TPART
&DE

Cette fonction permet de fixer le
seuil en-de뇚 duquel le test d'괾alit� des variables est consid굍� comme ayant 굏�
satisfait.

Le test de comparaison est :
&CO
    si x1 <<>> 0 :  |(x1 - x2) / x1| << eps
    sinon :       |x2| << eps
&TX

&SY
    $DataCompareEps eps
&TX

L'괹ran de comparaison de WS (Data/List/File Compare) permet 괾alement de
sp괹ifier cette valeur (param둻re Threshold).

&TI Seuil de comparaison par d괽aut
컴컴컴컴컴컴컴컴컴컴�
Le seuil de comparaison est fix� � 1e-7 par d괽aut.

>

<datacompareXxx>
    DataCompareXxx
    컴컴컴컴컴컴컴

&TPART
&DE
Le contenu du WS courant peut 늯re compar� � celui d'un fichier. Le
r굎ultat de cette comparaison est compos� de 4 listes :

&EN objets trouv굎 dans le WS seulement
&EN objets trouv굎 dans le fichier seulement
&EN objets trouv굎 dans les deux, avec la m늤e d괽inition
&EN objets trouv굎 dans les deux, avec une d괽inition diff굍ente

La comparaison s'effectue en fonction du type des objets.

&FS
Les champs � remplir sont de deux types: les deux premiers sp괹ifient
le type de WS � comparer (VAR, etc) et le nom du fichier � comparer
au WS.

Les quatre suivants permettent d'indiquer les noms � donner aux listes
r굎ultat.

&SY2
    ##DataCompare
&TX

&NFS
La fonction prend 5 param둻res (le type d'objet fait partie du nom
de la fonction): le fichier � comparer les 4 listes � construire.

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
    컴컴컴컴컴�
&TPART
&DE
Cette fonction construit une liste de noms d'objets sur base d'un
crit둹e (pattern) de s굃ection sur les noms. Elle peut fonctionner
sur les objets en WS ou sur les objets d'un fichier.

Elle permet par exemple d'obtenir la liste des objets dont le noms
commence par A et se termine par BEL (A*BEL).

&FS
Les 4 champs � remplir permettent de pr괹iser le nom de la liste �
contruire, le crit둹e, le type d'objet et le nom du fichier. Si ce dernier
est laiss� vide, les objets du WS sont utilis굎.

&SY2
    ##DataListXxx
&TX

&MU

&NFS
La commande prend deux ou trois param둻res, le premier pr괹ise le nom
de la liste � cr괻r, le deuxi둴e le pattern et le dernier (optionnel)
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
    컴컴컴컴컴�
&DE
Cette fonction calcule une liste sur base de deux listes en en prenant
la r굑nion, l'intersection ou la diff굍ence.

&FS
Les 4 champs � remplir permettent de pr괹iser
&EN le nom de la liste r굎ultat
&EN le noms des op굍andes (listes)
&EN l'op굍ateur

&SY2
    ##DataCalcLst
&TX

&NFS
La commande prend 4 param둻res, le premier pr괹ise le nom de la liste
r굎ultat, le deuxi둴e celui de la premi둹e liste, le troisi둴e
l'op굍ation et le quatri둴e le nom de la deuxi둴e liste.

&SY2
     $DataCalcLst res lst1 op lst2
	o� op = + : union
		* : intersection
		- : diff굍ence
&TX

&EX2
    $DataCalcLst _RES LST1 * LST2
&TX

Fournit dans la liste _RES les noms pr굎ents dans LST1 et LST2.
>

<dataeditcnf>
    DataEditCnf
    컴컴컴컴컴�

&DE
Cette fonction permet de changer le mode de visualisation du tableau de
s굍ies en "Level" (valeur r괻lle) en mode "Difference" et en mode
"Growth Rate" (taux de croissance). Elle permet aussi de choisir le
nombre de d괹imales � afficher.

&FS
Les deux champs � remplir permettent de pr괹iser le mode de visualisation
et le nombre de d괹imales � afficher.

&SY2
    ##DataEditCnf
&TX
&MU

&NFS
On passe deux param둻res � cette fonction :

&EN le mode de visualisation {Level | Differences | GrowthRates}

&EN le nombre de d괹imales � afficher (-1, 0, 1, 2, ...)

&SY2
    $DataEditCnf {L | D | G} n
    (n : = -1, 0, 1, 2, 3, ...)
&TX
>

<datadisplaygraph>
    DataDisplayGraph
    컴컴컴컴컴컴컴컴

&DE
Cette fonction permet de visualiser des s굍ies sous forme de graphique.

&FS
Les champs � remplir permettent de pr괹iser les noms des variables, le
mode de visualisation, la p굍iode, le type les axes, les grids, ....

&EN Variables name : liste des variables � inclure dans le graphique. Les
    noms (ou liste) peuvent 늯re s굋ar굎 par un blanc, un + ou une
    combinaison des deux. Les variables s굋ar괻s par un blanc seront
    visualis괻s dans des graphiques s굋ar굎 sur la m늤e page, tandis que
    les variables "additionn괻s" seront group괻s dans un m늤e graphique.
    Ainsi, si ce champ contient "X Y+Z T", trois graphiques seront
    construits avec respectivement les variables X (premier graphique),
    Y et Z (group괻s dans le second graphique) et T (dernier graphique).

&EN Mode : il s'agit d'un champ MENU permettant de pr괹iser si les
    variables doivent 늯re visualis괻s en valeur, en diff굍ence ou en
    taux de croissance :
&CO
    L=level
    D=differences
    G=growthrates
    d=YoY differences : diff굍ences sur 1 ann괻
    g=YoY growth rates : taux de croissance sur 1 ann괻

&TX

&EN Chart type : il s'agit d'un champ MENU permettant de pr괹iser le
    type de graphique � produire: line, scatter, scatter line ou bar

&EN Grids : il s'agit 괾alement d'un champ MENU permettant de d괽inir le
    type de quadrillage en X et en Y (pas de quadrillage, quadrillage
    majeur ou quadrillage mineur)

&EN Y scaling: ces deux champs permettent de d괽inir les valeurs minimum et
    maximum de l'axe des Y. Si ces valeurs sont nulles, l'axe est
    calcul� automatiquement en fonction des valeurs � afficher.

&EN Axis : ce champ du type MENU permet de s굃ectionner le type de
    transformation de coordonn괻s � utiliser: valeur, logarithme, ...

&EN Sample : pr괹iser la p굍iode d'impression (de p굍iode_1 �
    p굍iode_2). La syntaxe d'une p굍iode fait l'objet d'un chapitre s굋ar�.

&EN Save graph in file: le graphique peut 늯re sauvegard� dans un
    fichier au format AGL 괺itable par le programme GODE.

&SY2
    ##DataDisplayGraph
&TX

&MU

&NFS
On passe les param둻res suivants � cette fonction :

&SY2
    $DataDisplayGraph {Level | Diff | Grt | diffYoY | grtYoY}
		      {Line | Scatter | Bar | Mixt}
		      {NoXGrids | MinorXGrids | J(MajorXGrids)}
		      {NoYGrids | MinorYGrids | J(MajorYGrids)}
		      {Level | G(Log) | Semi-Log | Percent}
		      {ymin | --} {ymax | --}
		      perfrom perto varname1 varname2 ...

&TX

La signification des param둻res est expliqu괻 plus haut. Il est � noter
que tous les param둻res sont requis et doivent 늯re pass굎 dans l'ordre
indiqu�.
>

<datasavegraph>
    DataSaveGraph
    컴컴컴컴컴컴�

&DE
Cette fonction permet de construire des graphiques � partir de s굍ies
et d'en sauvegarger le r굎ultat dans un fichier AGL.

&FS
Les champs � remplir permettent de pr괹iser les noms des variables, le mode
de visualisation, la p굍iode, le type les axes, les grids, ....

&EN Variables name : liste des variables � inclure dans le graphique. Les
    noms (ou liste) peuvent 늯re s굋ar굎 par un blanc, un + ou une
    combinaison des deux. Les variables s굋ar괻s par un blanc seront
    visualis괻s dans des graphiques s굋ar굎 sur la m늤e page, tandis que
    les variables "additionn괻s" seront group괻s dans un m늤e graphique.
    Ainsi, si ce champ contient "X Y+Z T", trois graphiques seront
    construits avec respectivement les variables X (premier graphique),
    Y et Z (group괻s dans le second graphique) et T (dernier graphique).

&EN Mode : il s'agit d'un champ MENU permettant de pr괹iser si les variables
    doivent 늯re visualis괻s en valeur, en diff굍ence ou en taux de
    croissance

&EN Chart type: il s'agit d'un champ MENU permettant de pr괹iser le type de
    graphique � produire: line, scatter, scatter line ou bar

&EN Grids : il s'agit 괾alement de champs MENU permettant de d괽inir le type
    de quadrillage en X et en Y (pas de quadrillage, quadrillage majeur
    ou quadrillage mineur)

&EN Y scaling: ces deux champs permettent de d괽inir les valeurs minimum et
    maximum de l'axe des Y. Si ces valeurs sont nulles, l'axe est
    calcul� automatiquement en fonction des valeurs � afficher.

&EN Axis : ce champ du type MENU permet de s굃ectionner le type de
    transformation de coordonn괻s � utiliser: valeur, logarithme, ...

&EN Sample : pr괹iser la p굍iode d'impression (de p굍iode_1 �
    p굍iode_2). La syntaxe d'une p굍iode est donn괻s dans le manuel de
    r괽굍ence.

&EN Save graph in file: le graphique peut 늯re sauvegard� dans un
    fichier au format AGL 괺itable par le programme GODE.

&SY2
    ##DataDisplayGraph
&TX

&MU

&NFS
On passe les m늤es param둻res que ceux d괹rits ci-dessus � cette
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

Il est � noter que tous les param둻res sont requis et doivent 늯re
pass굎 dans l'ordre indiqu�.
>

<datacalcvar>
    DataCalcVar
    컴컴컴컴컴�

&DE
Cette fonction permet de calculer une nouvelle s굍ie � partir d'une
formule LEC. Cette formule est ex괹ut괻 sur toute la p굍iode du WS
courant et ce en utilisant les valeurs des variables du WS courant.

&FS
La fonction plein 괹ran 굌uivalente est la fonction standard d'괺ition
des variables.

&NFS
On passe deux param둻res � cette fonction :

&EN le nom de la variable � cr괻r

&EN la formule LEC � calculer

&SY2
    $DataCalcVar nom formule
&TX

&EX2
    $DataCalcVar X    beta + gamma * ln B + alpha * ln C
&TX
>

<datacreate>
    DataCreateXxx
    컴컴컴컴컴컴�
&TPART
&EXTWS
&DE
La fonction permet la cr괶tion d'objets dans le WS courant. Il n'y a pas
de fonction plein 괹ran 굌uivalente, sinon les fonctions standard
d'괺ition d'objets (menu DATA).

&NFS
Si l'objet "nom" existe d굁�, la fonction retourne et signale l'erreur.
Sinon l'objet est cr굚 avec une valeur par d괽aut :

&EN commentaires, listes, identit괻s et tableaux : vide

&EN 굌uation : "NOM : =NOM"

&EN scalaires : 0.9

&EN variables : une s굍ie avec des NA (--) sur toute la p굍iode du WS

&EN tableaux : cr괻 un tableau vide

&SY2
    $DataCreateVar nom
&TX
>

<datadelete>
    DataDeleteXxx
    컴컴컴컴컴컴�
&TPART
&EXTWS
&DE
La fonction permet de d굏ruire des objets dans le WS courant. Il n'y a pas
de fonction plein 괹ran 굌uivalente, sinon la fonction d'괺ition
habituelle (menu Data).

&NFS
Si l'objet "nom" n'existe pas, la fonction retourne et signale l'erreur.

&SY2
    $DataDeleteVar nom
&TX

&NO Depuis la version 5.13, les wildcards sont accept괻s dans le nom
>

<dataexist>
    DataExistXxx
    컴컴컴컴컴컴
&TPART
&EXTWS
&DE
La fonction v굍ifie l'existence d'un objet dans le WS courant.

&FS
&NEQ

&NFS
Si l'objet "nom" n'existe pas, la fonction retourne -1 et produit un message d'erreur.

Si $OnError est fix� � Abort ou Return dans le rapport en cours, l'ex괹ution
du rapport se termine.

&SY2
    $DataExistVar nom
&TX


&EX

&TX
>

<datarename>
    DataRenameXxx
    컴컴컴컴컴컴�
&TPART

Xxx prend l'une des valeurs :
&CO
    cmt | idt | lst | scl | tbl | var
&TX

&DE
La fonction change le nom d'un objet dans le WS courant.

Note: les 굌uations ne peuvent 늯re renomm괻s (car leur endog둵e est aussi leur nom).

&FS
&NEQ
&NFS
Si l'objet "nom" n'existe pas, ou si Xxx correspond � EQS, la fonction retourne et signale l'erreur.
Si l'objet existe, il sera sauv� sous un nouveau nom. S'il y a d굁� un
objet portant ce nouveau nom, il est remplac�.

&SY2
    $DataRenameVar nom nouveau_nom
&TX
>

<dataupdate>
    DataUpdateXxx
    컴컴컴컴컴
&TPART
&EXTWS
&DE
La fonction change le contenu d'un objet dans le WS courant.

&FS
Il n'y a pas de fonction plein 괹ran 굌uivalente, si ce n'est la
fonction habituelle d'괺ition (Menu DATA).

&NFS
Si l'objet "nom" n'existe pas, un objet contenant la nouvelle d괽inition
est cr굚. Si l'objet existe, son contenu est remplac�. La fonction
prend plusieurs arguments. Les arguments diff둹ent selon le type d'objet �
mettre � jour. Le premier argument est toujours le nom de l'objet �
changer.

&IT Commentaires

&CO
    $DataUpdateCmt nom commentaire
&TX

&IT Equations

&CO
    $DataUpdateEqs nom 굌uation_lec
&TX

&IT Identit굎

&CO
    $DataUpdateIdt nom identit�_lec
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
Si title est un nom de commentaire, le commentaire est utilis� comme
titre. De m늤e, si on trouve des noms de variables comme forme lec, et
qu'il existe un commentaire pour ces variables, le titre de la ligne
correspondante est remplac� par la valeur du commentaire.

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
cr괻 le tableau T1 avec la forme suivante :
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
    $DataUpdateVar nom [L,l | D,d | G,g] p굍iode valeur1 valeur2 ...
     o� L,l := en valeur (d괽aut)
	D, d := en diff굍ence
	G, g := en taux de croissance
	p굍iode := la p굍iode du d괷ut de la mise � jour
&TX
>

<dataappend>
    DataAppendXxx
    컴컴컴컴컴
&TPART

Xxx prend l'une des valeurs :
&CO
    cmt | lst
&TX

&DE
La fonction ajoute un texte � la d괽inition d'un objet du WS courant. Seuls les commentaires
et les listes peuvent 늯re prolong괻s par cette fonction.

&FS
&NEQ
&NFS
Si l'objet "nom" n'existe pas, un objet contenant la nouvelle d괽inition
est cr굚. Si l'objet existe, son contenu est compl굏�.

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
    컴컴컴컴컴�
&TPART
&EXTWS
&DE
Cette fonction permet de rechercher dans un des WS courants la liste des
variables et scalaires utilis굎 dans la d괽inition des objets de ce WS.

Le r굎ultat de cette recherche est sauv� dans deux listes, contenant
d'une part les noms des scalaires (liste _SCAL) et d'autre part ceux des
variables (liste _EXO). Si on ne passe pas d'argument � cette fonction,
tout le WS est examin�.

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
    컴컴컴컴컴�
&TPART
&EXTWS
&DE
La fonction permet l'괺ition d'objets du WS courant.

Un 괹ran pr굎ente les objets s굃ectionn굎 sous forme d'un tableau
d굍oulant (ou tableau d'괺ition) dans lequel on pourra se d굋lacer.

Le tableau d'괺ition est compos� des 굃굆ents suivants :

&EN le nom du WS dans la ligne sup굍ieure
&EN le nom des objets dans la colonne de gauche
&EN la d괽inition (ou une partie de la d괽inition) des objets en regard
    du nom dans la partie de droite
&EN un scrollbar indiquant la position courante dans le tableau et la
    proportion visible du tableau

L'objet courant est indiqu� par le fait que la ligne qui lui correspond
est en vid굊 inverse.

Les touches de fonction des tableaux d굍oulant permettent l'affichage de
la valeur, la modification et la cr괶tion des objets.

&FS
La liste des objets � 괺iter est pass괻 en argument de la commande. Les
굃굆ents de la liste sont s굋ar굎 par des blancs, virgules ou
points-virgules. Si la liste est vide (pas d'argument), tous les objets
apparaissent dans le tableau d'괺ition.

&SY2
    ##DataEditXxx [objet [liste d'objets]]
    {Xxx = cmt | eqs | idt | lst | scl | tbl | var}
&TX

&NFS
La fonction n'est exploitable qu'en mode plein 괹ran.
>

<datawidthvar>
    DataWidthVar
    컴컴컴컴컴컴

&DE
Cette fonction permet de sp괹ifier la largeur des colonnes d'괺ition
des s굍ies statistiques. La valeur doit 늯re comprise entre 2 et 12.

&FS
Cette fonction n'est pas disponible en mode plein 괹ran. Les touches
F3 et s-F3 permettent de changer la largeur des colonnes lors de l'괺ition.

&NFS
&SY2
    $DataWidthVar n
    n entre 2 et 12
&TX
>
<datandecvar>
    DataNdecVar
    컴컴컴컴컴�

&DE
Cette fonction permet de sp괹ifier le nombre de d괹imales lors de
l'괺ition des s굍ies statistiques. La valeur doit 늯re comprise entre
-1 (nombre de d괹imales variable) et 6.

&FS
Cette fonction n'est pas disponible en mode plein 괹ran. Les touches
F4 et s-F4 permettent de changer la largeur des colonnes de l'괺ition.

&NFS
&SY2
    $DataNdecVar n
    n entre -1 et 6
&TX
>
<datamodevar>
    DataModeVar
    컴컴컴컴컴�

&DE
Cette fonction permet de choisir le mode d'괺ition des s굍ies statistiques:
Niveau, diff굍ences ou taux de croissance.

&FS
Cette fonction n'est pas disponible en mode plein 괹ran. La touche
F5 permet de changer la largeur des colonnes lors de l'괺ition.

&NFS
&SY2
    $DataModeVar n
    n = 0 pour niveau
    n = 1 pour diff굍ences
    n = 2 pour taux de croissances
&TX

>
<datastartvar>
    DataStartVar
    컴컴컴컴컴컴


&DE
Cette fonction permet de choisir la premi둹e colonne visible lors de
l'괺ition des s굍ies statistiques. La valeur doit 늯re comprise entre
0 (premi둹e colonne) et le nombre de p굍iodes disponibles.

&FS
Cette fonction n'est pas disponible en mode plein 괹ran.

&NFS
&SY2
    $DataStartVar n
    n entre 0 et nombre de p굍iodes
&TX
>
<datawidthtbl>
    DataWidthTbl
    컴컴컴컴컴컴

&DE
Cette fonction permet de sp괹ifier la largeur des colonnes d'괺ition
des tableaux. La valeur doit 늯re comprise entre 2 et 60.

&FS
Cette fonction n'est pas disponible en mode plein 괹ran. Les touches
F3 et s-F3 permettent de changer la largeur des colonnes lors de l'괺ition.

&NFS
&SY2
    $DataWidthTbl n
    n entre 2 et 60
&TX

>
<datawidthscl>
    DataWidthScl
    컴컴컴컴컴컴
&DE
Cette fonction permet de sp괹ifier la largeur des colonnes d'괺ition
des s굍ies scalaires. La valeur doit 늯re comprise entre 2 et 12.

&FS
Cette fonction n'est pas disponible en mode plein 괹ran. Les touches
F3 et s-F3 permettent de changer la largeur des colonnes lors de l'괺ition.

&NFS
&SY2
    $DataWidthScl n
    n entre 2 et 12
&TX
>
<datandecscl>
    DataNdecScl
    컴컴컴컴컴�

&DE
Cette fonction permet de sp괹ifier le nombre de d괹imales lors de
l'괺ition des scalaires. La valeur doit 늯re comprise entre
-1 (nombre de d괹imales variable) et 6.

&FS
Cette fonction n'est pas disponible en mode plein 괹ran. Les touches
F4 et s-F4 permettent de changer la largeur des colonnes de l'괺ition.

&NFS
&SY2
    $DataNdecScl n
    n entre -1 et 6
&TX

>
<datasearch>
     DataSearchXxx
     컴컴컴컴컴컴�
&TPART
&EXTWS
&DE
Cette fonction permet de rechercher dans un des WS courants la liste des
objets contenant un string donn�.

Le r굎ultat de cette recherche est une liste d'objets qui peut au besoin
늯re sauv괻 dans le WS courant de listes. Par d괽aut la liste _RES contient
le r굎ultat de la derni둹e recherche.

&FS
La fonction affiche une page de saisie comprenant les champs suivants :

&EN Texte � rechercher : le texte � rechercher peut contenir des
    caract둹es sp괹iaux qui permettent de sp괹ifier les limites de la
    recherche :
&EN2 * : n'importe quelle suite de caract둹es (m늤e vide)
&EN2 ? : un et un seul caract둹e (quelconque)
&EN2 @ : n'importe quel caract둹e alphanum굍ique
&EN2 ampersand : n'importe quel caract둹e non alphanum굍ique
&EN2 | : n'importe quel caract둹e alphanum굍ique ou aucun en d괷ut et fin
    de string
&EN2 ! : n'importe quel caract둹e non alphanum굍ique ou aucun en d괷ut et
    fin de string
&EN2 \ plac� devant un des caract둹es sp괹iaux supprime son interpr굏ation

&EN Mot entier/partiel (Whole word) : pr괹iser Yes si la cha똭e �
    rechercher doit 늯re un mot entier et non une partie de mot.
    Indiquer No si cela n'a pas d'importance.

&EN Majuscule/minuscule (Exact case) : pr괹iser si la recherche doit
    diff굍encier majuscules et minuscules dans la cha똭e � rechercher.


&EN Type d'objet: le type d'objet doit 늯re indiqu� dans ce champ. En
    pressant TAB, la liste des types appara똳. Il suffit de se placer
    sur le type choisi et de presser ENTER.

Les recherches sont diff굍entes en fonction du type d'objet :

&EN Commentaires : le nom et le texte du commentaire sont analys굎
&EN Equations    : le nom et la forme LEC de l'굌uation sont analys굎
&EN Identit굎    : le nom et la forme LEC de l'identit� sont analys굎
&EN Listes       : le nom et le texte de la liste sont analys굎
&EN Scalaires    : le nom du scalaire est analys�
&EN Tableaux     : le nom, les titres et les formes LEC du tableau sont
		   analys굎
&EN Variables    : le nom de la variable est analys�

&EN Liste r굎ultat: il est possible de sauver la liste des objets
    r굋ondant au crit둹e dans le WS de listes courant. Le nom de cette
    liste doit 늯re indiqu� et 늯re un nom de liste valide. Si une liste
    de m늤e nom existe, elle sera remplac괻 par le r굎ultat du calcul.

&EN R굎ultat: dans le bas de l'괹ran, un champ reprend les premiers
    objets satisfaisant au crit둹e.

La recherche commence lorsque la page est valid괻 avec F10.

&SY2
    ##DataSearchCmt
&TX

&NFS
Sept param둻res doivent 늯re pass굎 � la commande.

&SY2
    $DataSearchXxx mask word ecase in_name in_formula in_text list_result

    (word, ecase, in_name, in_formula, in_text := 0 ou 1)
    (mask := caract둹es et 굒entuellement ?, *, ...)
    (list_result := le nom de la liste r굎ultat)
&TX

&EX2
    $datasearchCmt TE?T 0 0 1 0 1 NEW2
    ##DataEditCMT $NEW2
&TX
>


<dataduplicate>
    DataDuplicateXxx
    컴컴컴컴컴컴�
&TPART
&EXTWS
&DE
Les objets d괽inis dans les WS courants peuvent 늯re copi굎 dans des
objets du m늤e type mais d'un nom diff굍ent. Remarque : il existe une
m굏hode simple pour effectuer cette op굍ation : dans l'괹ran de
d괽inition de l'objet � duplicier, il suffit de changer le nom de
l'objet et de le sauver. Un nouvel objet ayant les m늤es
caract굍istiques que l'ancien est ainsi cr굚.

La fonction d괹rite ici permet la m늤e op굍ation. Elle fonctionne de
la m늤e mani둹e pour tous les objets sauf pour les 굌uations.

&FS
La commande affiche un 괹ran de saisie contenant trois champs.

Le type d'objet � 괺iter peut 늯re s굃ectionn� en se pla놹nt sur le
champ indiquant le type et en pressant TAB : un menu reprenant tous les
types d'objets permet d'effectuer la s굃ection.

Les deux autres champs contiennent respectivement l'ancien et le
nouveau nom de l'objet.

&SY2
    ##DataDuplicateCmt
&TX

&NFS
La fonction prend successivement deux param둻res: l'ancien et le
nouveau nom de l'objet.

&SY2
    $DataDuplicateCmt ancien_nom nouveau_nom
&TX

&EX2
    $DataDuplicateCmt A01 A02
&TX

La fonction DataDuplicateEqs n'est pas impl굆ent괻 et n'a d'ailleurs pas
de signification. En effet, une 굌uation porte le nom de sa variable
endog둵e. Changer son nom change donc le nom de l'endog둵e, ce qui en
g굈굍al n'a pas de sens.

L'utilisateur curieux qui utiliserait malgr� tout cette fonction
recevrait un message d'avertissement.
>


<printdest>
    PrintDest
    컴컴컴컴�

&DE
La fonction permet de sp괹ifier s'il faut imprimer dans un fichier et, si
c'est le cas, le nom et le type du fichier r굎ultat. Cette d괽inition est
valable pour toutes les impressions faites au d굋art d'un menu ou d'un
rapport de IODE, jusqu'� la fin du rapport en ex괹ution ou jusqu'� ce
qu'une autre commande du rapport sp괹ifie une autre destination, auquel
cas la configuration de l'impression est modifi괻 pour toutes les
impressions ult굍ieures.

Le fichier destination est toujours r굀nitialis�, m늤e dans le cas
des fichiers a2m. Ceci est nouveau � partir de la version 5 de IODE.

&FS
Dans chaque 괹ran d'impression les diff굍ents param둻res requis peuvent
늯re sp괹ifi굎 : nombre de d괹imales et langue par d괽aut. Un bouton
Setup permet en outre de d굏erminer s'il faut imprimer dans un fichier
ou sur une imprimante.

&NFS
Le nom du fichier destination est pass� en param둻re de la commande :

&SY2
    $PrintDest [nom_fichier] [format]

	o� format = A2M, MIF, HTML, RTF ou CSV
	par d괽aut, A2M est le format choisi
&TX
Si ~cnom_fichier~C n'est pas sp괹ifi�, l'impression aura lieu sur une
imprimante.


&EX2
    $PrintDest test.mif MIF
&TX
>
<printdestnew>
    PrintDestNew
    컴컴컴컴컴컴
&DE
Cette fonction est identique � $PrintDest, � ceci pr둺 qu'elle d굏ruit
le fichier output.

&NFS
Le nom du fichier destination est pass� en param둻re de la commande :

&SY2
    $PrintDestNew [nom_fichier] [format]

	o� format = A2M, MIF, HTML, RTF ou CSV
	par d괽aut, A2M est le format choisi
&TX
Si ~cnom_fichier~C n'est pas sp괹ifi�, l'impression aura lieu sur une
imprimante.
>

<printnbdec>
    PrintNbDec
    컴컴컴컴컴
&DE
La fonction permet de sp괹ifier le nombre de d괹imales � imprimer lors
des impressions de tableaux ou de variables.

&FS
Dans chaque 괹ran d'impression les diff굍ents param둻res requis sont
sp괹ifi굎.

Un de ces param둻res est "NbDec". Un nombre entier positif sp괹ifie le
nombre de d괹imales qui seront imprim괻s. Si la valeur -1 est sp괹ifi괻,
le nombre de d괹imales est celui de la d괽inition interne de la valeur �
imprimer (en g굈굍al quelconque).

&NFS
Le nombre de d괹imales est pass� en param둻re de la commande :

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
    컴컴컴컴�

&DE
La fonction permet de sp괹ifier la langue d'impression quand on imprime
les tableaux. La langue par d괽aut est l'anglais.

&FS
Dans chaque 괹ran d'impression les diff굍ents param둻res requis sont
sp괹ifi굎. Le champ "Language" est un champ MENU dans lequel on pr괹ise
la langue d'impression.

&NFS
La langue d'impression est pass� en param둻re de la commande :

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
    컴컴컴컴컴컴컴
&TPART
&EXTWS
&DE
Cette commande permet l'impression au format A2M de la d괽inition
des objets du ws courant.

&FS
La fonction affiche une page de saisie dans laquelle on d괽init le ou
les param둻res de la commande.

La page de saisie contient deux champs: le premier est un champ MENU
(voir lexique) : on y s굃ectionne le type d'objet � imprimer (le type
CMT est propos� par d괽aut); le second est un champ EDITOR (voir
lexique): on y sp괹ifie la liste des objets � imprimer.

L'imression se fait sur l'imprimante s굃ectionn괻 lorsqu'on valide la
page avec F10. La d괽inition est imprim� en format A2M.

Si la liste des objets est vide, tous les objets du type sp괹ifi� dans le
WS courant sont imprim굎. La liste peut contenir des noms de listes ($...)
et/ou une 굈um굍ation de noms d'objets s굋ar굎 par un blanc, une virgule ou un
point-virgule.

&MU

&SY2
    ##PrintObjDefCmt
&TX

&NFS
La liste des commentaires � imprimer est pass괻 en param둻re de la
commande.

&SY2
    $PrintObjDefXxx [liste de param둻res]
    o� Xxx = {cmt | idt | lst | scl | tbl | var}.
&TX

&EX2
    $PrintObjDefCmt CMT1 CMT2
&TX
>


<printtbl>
    PrintTbl
    컴컴컴컴

&DE
Cette commande permet le construction et l'impression des tableaux au
format A2M.

&FS
La fonction affiche une page de saisie dans laquelle on d괽init le ou
les param둻res de la commande. Son fonctionnement est d괹rit dans le
manuel de l'utilisateur.

&SY2
    ##PrintTbl
&TX

&NFS
Le sample d'impression suivi de la liste des tableaux � imprimer sont
pass굎 comme param둻res de la commande.

Le dernier appel � $PrintTblFile permet d'imprimer des comparaisons de
fichiers. Cependant, l'appel � $PrintTbl ou $PrintVar "consomme" les
fichiers d괽inis par $PrintTblFile. Si on veut faire appel plusieurs
fois � $PrintTbl pour comparer des fichiers, il faut repr괹iser les
fichiers � comparer avant chaque appel.

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

&NO Il ne peut y avoir d'espace dans l'굈onc� du sample.

&IT Appels successifs

Le dernier appel � ~c$PrintTblFile~C permet d'imprimer des comparaisons de
fichiers. Cependant, l'appel � ~c$PrintTbl~C ou ~c$PrintVar~C "consomme" les
fichiers d괽inis par ~c$PrintTblFile.~C Si on veut faire appel plusieurs
fois � ~c$PrintTbl~C pour comparer des fichiers, il faut repr괹iser les
fichiers � comparer avant chaque appel.

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
    컴컴컴컴컴컴

&DE
Cette commande permet de d괽inir les fichiers � utiliser lors de
l'impression d'un tableau de comparaison entre plusieurs fichiers
(maximum 4). Le sample de l'impression peut contenir des r괽굍ences vers
d'autres fichiers que le WS, plac괻s entre crochets (ex. 90:2^[1;2]). Le
fichier [1] est toujours le WS, les autres peuvent 늯re d괽inis ou dans
la page de saisie ou comme argument dans un rapport.

L'objet de cette fonction est d'indiquer les fichiers r괽굍enc굎 par
[2], [3], etc.

&FS
Il n'existe pas de fonction 굌uivalente en plein 괹ran : la fonction
##PrintTbl affiche une page de saisie dans laquelle on d괽init notamment
les noms de fichiers � utiliser lors de l'impression.

&SY2
    ##PrintTbl
&TX

&NFS
Le num굍o de fichier (2, 3, 4 ou 5) suivi du nom du fichier sont pass굎
en param둻re de la commande.

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

Apr둺 un appel � ~c$PrintTbl,~C il faut red괽inir les fichiers � comparer:
~c$PrintTbl~C "consomme" les d괽initions de fichiers � comparer.

&NO Il ne peut y avoir d'espace dans l'굈onc� du sample.
>


<viewtbl>
    ViewTbl
    컴컴컴�

&DE
La fonction construit et affiche les tables dans un tableau d굍oulant (ou
tableau d'괺ition).

&FS
La fonction affiche une page de saisie dans laquelle on d괽init le ou
les param둻res de la commande. Cette fonction est d괹rite dans le
"Manuel de l'utilisateur" : Menu Print/Graph, Option "Print Tables".

&SY2
    ##ViewTbl
&TX

&NFS
Le sample d'impression suivi de la liste des tableaux � visualiser sont
pass굎 en param둻re de la commande.

&SY2
    $ViewTbl sample tableau [liste de tableaux]
&TX

&EX2

&TX $ViewTbl 90:6 CIIIR0 CIIR

&NO Il ne peut y avoir d'espace dans l'굈onc� du sample.
>

<viewbytbl>
    ViewByTbl
    컴컴컴컴�

&DE
Alias de la fonction $ViewTbl.
>

<viewtblfile>
    ViewTblFile
    컴컴컴컴컴�

&DE
Cette commande permet de d괽inir les fichiers � utiliser lors de la
visualisation d'un tableau de comparaison entre plusieurs fichiers (max
4). Le sample de l'impression peut contenir des r괽굍ences vers d'autres
fichiers que le WS. (ex. 90:2[1;2]). Le fichier [1] est toujours le WS,
les autres peuvent 늯re d괽inis ou dans la page de saisie ou comme
argument dans un rapport.

&FS
Il n'y a pas de fonction directement 굌uivalente en plein 괹ran : la
fonction d'impression de tableaux permet d'encoder les fichiers �
comparer :

&SY2
    ##ViewTbl
&TX

&NFS
Le num굍o de fichier (2, 3, 4 ou 5) suivi du nom du fichier sont pass굎
en param둻re de la commande.

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

Apr둺 un appel � ~c$ViewTbl,~C il faut red괽inir les fichiers � comparer:
~c$ViewTbl~C "consomme" les d괽initions de fichiers � comparer.
>

<modelsimulate>
    ModelSimulate
    컴컴컴컴컴컴�

&DE
La fonction lance la simulation d'un mod둳e.

Pour simuler un mod둳e, il faut que toutes les 굌uations du mod둳e
soient pr굎entes dans le WS d'굌uations, que toutes les variables et
tous les scalaires utilis굎 dans les 굌uations du mod둳e soient d괽inis
dans les WS de variables et de scalaires. Les valeurs des exog둵es et
des scalaires ne peuvent 늯re NA (Not Available) sur la p굍iode utile.

&FS
La fonction affiche une page de saisie dans laquelle on d괽init le ou
les param둻res de simulation. Cette fonction est d괹rite dans "Manuel de
l'utilisateur".

&SY2
    ##ModelSimulate
&TX

&NFS
Les param둻res de la simulation sont d괽inis par la commande annexe
ModelSimulateParms. La simulation proprement dite s'effectue par la
commande ModelSimulate.

Les deux p굍iodes limites (d괷ut et fin) et la liste des 굌uations �
simuler sont pass괻s en argument � la fonction. Si la liste est vide, la
simulation sera effectu괻 pour le mod둳e constitu� de toutes les
굌uations en m굆oire.

Les param둻res compl굆entaires peuvent 늯re d괽inis par la commande
modelsimulateparms d괹rite ci-dessous.

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
    컴컴컴컴컴컴�

&DE
La fonction lance la simulation d'un mod둳e pr괶lablement d괹ompos� en CFC et tri�.

La performance des simulations, sp괹ifiquement lors du d굆arrage (link, sort), est
meilleure avec cette fonction qu'avec $ModelSimulate car le r굊rdonnancement
du mod둳e n'a lieu qu'une seule fois et non � chaque lancement de
simulation.

Le processus de simulation a 굏� divis� en 2 굏apes. La premi둹e
ne s'occupe que du r굊rdonnancement du mod둳e ($ModelCalcSCC), la seconde de la simulation.

&FS
La fonction affiche une page de saisie dans laquelle on d괽init le ou
les param둻res de simulation et les listes qui composent le mod둳e tri�.

&SY2
    ##ModelSimulateSCC
&TX

&NFS
La simulation du mod둳e se base sur trois listes
pr괶lablement construites par la fonction $ModelCalcSCC (ou � la main).

Sa syntaxe est :

&CO
    $ModelSimulateSCC from to pre inter post
&TX
o� :
&EN from et to d굏erminent le sample de simulation
&EN pre, inter et post sont les listes qui d괽inissent l'ordre d'ex괹ution du mod둳e.
>

<modelcalcscc>
    ModelCalcSCC
    컴컴컴컴컴컴
Cette fonction permet de d괹omposer le mod둳e en composantes fortement
connexes (CFC ou SCC pour Strongly Connex Components) et de le r굊rdonnancer.

Trois listes sont donc cr굚es : 굌uations pr굍괹ursives,
interd굋endantes et postr괹ursives.

Lors du r굊rdonnancement du mod둳e, le nombre d'it굍ations de triangulation
(tri) pour le block interd굋endant doit 늯re sp괹ifi�. Cette valeur n'a
굒idemment d'effet que sur la liste des 굌uations interd굋endantes.

Ces 3 listes ne doivent 늯re contruites qu'une seule fois pour une version donn괻 du mod둳e.

&SY
    $ModelCalcSCC nbtri lstpre lstinter lstpost
&TX
o� :

&EN nbtri est le nombre d'it굍ations de triangulation � effectuer
&EN lst* sont les NOMS des listes destin괻s � contenir les r굎ultats du tri des 굌uations


&IT Choix du nombre de tris
컴컴컴컴컴컴컴컴컴컴컴컴컴�

Apr둺 la d괹ompostion en CFC, le bloc interd굋endant est tri� pour augmenter la vitesse
de la simulation. Le nombre de passage de l'algorithme de tri peut 늯re
sp괹ifi� � plusieurs endroits :

&EN Dans l'괹ran de simulation "standard" : param둻re "Passes" fix�
&EN Dans l'괹ran de calcul de la d괹omposition du mod둳e :  param둻re "Triangulation Iterations"
&EN Comme param둻re dans la commande rapport $ModelCalcSCC
&EN Comme dernier param둻re dans la commande rapport $ModelSimulateParms

&NO Dans les versions ant굍ieures, le nombre de passages de la triangulation sp괹ifi� dans
l'괹ran de simulation n'avait pas d'effet

>

<modelsimulateparms>
    ModelSimulateParms
    컴컴컴컴컴컴컴컴컴

&DE
Cette commande permet de sp괹ifier les param둻res compl굆entaires
(essentiellement techniques) d'une simulation. La simulation est
effectivement lanc괻 par la commande ModelSimulate.

Les param둻res que fixe cette commande sont d괹rits dans le "Manuel de
l'utilisateur".

&FS
Il n'y a pas de fonction sp괹ifique plein 괹ran 굌uivalente. L'괹ran de
simulation appel� par ##ModelSimulate contient les param둻res de
simulation.

&SY2
    ##ModelSimulate
&TX

&NFS
Cette fonction fixe les param둻res suivants de simulation :

&EN seuil de convergence (eps)
&EN param둻re de relaxation (relax)
&EN nombre maximum d'it굍ations (maxit)
&EN algorithme de r굊rdonnacement (Connex, Triangulation (= Both) ou None)
&EN valeurs initiales (0 � 4)
&EN2 0 = Y := Y[-1], if Y null or NA
&EN2 1 = Y := Y[-1], always
&EN2 2 = Y := extrapolation, if Y null or NA
&EN2 3 = Y := extrapolation, always
&EN2 4 = Y unchanged
&EN2 5 = Y := Y[-1], if Y is NA
&EN2 6 = Y := extrapolation, if Y is NA
&EN simulation avec debugging
&EN nombre d'it굍ations de tri des composantes connexes
&EN debugging de la sous-it굍ation de Newton-Raphson

&SY2
    $ModelSimulateParms eps relax maxit
			{Connex | Triang | Both | None }
			0 - 4 (starting values)
			{Yes | no }
			nbtri
			{yes | No }
    (eps  := seuil de convergence : nombre r괻l)
    (relax := nombre r괻l compris entre 0.1 et 1)
&TX

&EX2
    $ModelSimulateParms 0.001 0.7 100 Connex 0 Yes 2 No 
&TX




&IT D괹omposition et r굊rdonnancement du mod둳e
컴컴컴컴컴컴컴컴컴컴컴컴컴�
Un algorithme de r굊rdonnacement (Connex, Triangulation (= Both) ou None) peut 늯re appliqu�.
&EN Connex = d괹omposition du mod둳e en "composantes fortement connexes"
&EN Triangulation ou Both = r굊rdonnancement des blocs pour acc굃굍er la convergence
&EN None = le mod둳e est laiss� tel quel.


&IT Choix du nombre de tris
컴컴컴컴컴컴컴컴컴컴컴컴컴�

Apr둺 la d괹ompostion en CFC, le bloc interd굋endant peut 늯re r굊rdonnanc� (Triangulation ou Both) pour augmenter la vitesse
de la simulation.


&IT Option Debug
컴컴컴컴컴컴

L'option debug de la simulation g굈둹e automatiquement des listes
comprenant des 굌uations pr�- et post-r괹ursives.

&NO Version 6.36 : l'option debug activait la g굈굍ation d'un fichier
simul.dbg qui contenait une quantit� 굈orme d'informations. Dans cette
version, seules les listes ~c_PRE, _INTER~C et ~c_POST~C (avec la d괹oupe du
mod둳e) sont g굈굍괻s.


&IT Option Debug sous-it굍ation
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�

Un fichier de trace
est g굈굍� si cette option est fix괻 � "Y". Ce fichier se nomme "simul.dbg" et se trouve dans
le directory d'o� IODE a 굏� lanc�. Le fichier de trace comprend la
liste des 굌uations pour lesquelles une sous-it굍ation (Newton-Raphson)
a 굏� n괹essaire et le nombre de sous-it굍ations.


>


<modelsimulatesaveniters>
    ModelSimulateSaveNiters
    컴컴컴컴컴컴컴컴컴컴컴�

&DE
Cette commande permet de sauver dans une variable le nombre d'it굍ations
qui ont 굏� n괹essaires pour chaque p굍iode lors de la derni둹e simulation.

Le nom de la variable r굎ultat est pass� comme param둻re.

S'il n'y a pas encore eu de simulation dans le cours de la session, la variable est
cr굚e avec des valeurs NA.

Si une simulation a d굁� eu lieu, les valeurs des p굍iodes non simul괻s sont fix괻s � 0 et
celles des p굍iodes simul괻s contiennent le nombre d'it굍ations n괹essaires �
la convergence pour cette p굍iode. S'il n'y a pas de convergence, la valeur est celle du
param둻re MAXIT de la simulation.

&FS
Il n'y a pas de fonction sp괹ifique plein 괹ran 굌uivalente.


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
    컴컴컴컴컴컴컴컴컴컴컴�

&DE
Cette commande permet de sauver dans une variable le seuil de convergence (la norme) atteint
pour chaque p굍iode lors de la derni둹e simulation.

Le nom de la variable r굎ultat est pass� comme param둻re.

S'il n'y a pas encore eu de simulation dans le cours de la session, la variable est
cr굚e avec des valeurs NA.

Si une simulation a d굁� eu lieu, les valeurs des p굍iodes non simul괻s sont fix괻s � 0 et
celles des p굍iodes simul괻s contiennent le seuil de convergence
pour cette p굍iode.

&FS
Il n'y a pas de fonction sp괹ifique plein 괹ran 굌uivalente.

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
    컴컴컴컴컴

&DE
Une identit� est une forme LEC qui indique comment une s굍ie doit 늯re
construite. Le programme d'ex괹ution calcule toutes les identit굎
demand괻s et sauve dans le WS de s굍ies le r굎ultat des calculs.

Ces identit굎 sont ordonnanc괻s dans l'ordre logique d'ex괹ution : si A
d굋end de B, B sera calcul� avant A. L'utilisateur n'a pas � se
pr굊ccuper de cet ordonnancement. Si une boucle est d굏ect괻 dans
les identit굎, l'ex괹ution ne peut commencer et un message est affich�.

&FS
Cette fonction est d괹rite dans le "Manuel de l'utilisateur". Elle est
appel괻 par :

&SY2
    ##IdtExecute
&TX

&NFS
Les deux p굍iodes limites (d괷ut et fin) et la liste des identit굎 �
calculer sont pass괻s en argument � la fonction. Si la liste est vide, le
calcul sera effectu괻 pour toutes les identit굎 en m굆oire.

Les param둻res compl굆entaires peuvent 늯re d괽inis par les commandes
IdtExecuteVarFiles et IdtExecuteSclFiles d괹rites ci-dessous.

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
    컴컴컴컴컴컴컴컴컴

&DE
Cette commande fait partie d'un groupe de commandes permettant de
sp괹ifier les param둻res compl굆entaires (fichiers de recherche) du
calcul des identit굎. Les commandes associ괻s sont IdtExecute et
IdtExecuteSclFiles. Le calcul des identit굎 est effectivement lanc� par
la commande IdtExecute.

La fonction permet d'introduire les noms de fichiers o� les s굍ies
r괽굍enc괻s dans les identit굎 doivent 늯re recherch굎.

Lorsqu'une s굍ie ou un scalaire utile � l'ex괹ution d'une identit� est
rencontr�, les fichiers indiqu굎 sont analys굎 � tour de r뱇e. D둺 que
l'objet recherch� est trouv�, il est amen� en m굆oire pour la dur괻 du
calcul. Il est ensuite d굏ruit, sauf s'il 굏ait initialement en m굆oire.

Si un objet - variable ou scalaire - n'est ni en WS, ni dans un des
fichiers indiqu굎, le processus de calcul s'arr늯e.

Cette fonction n'a tout son sens que lorsque la commande IdtExecute est
utilis괻 en mode ligne de commande.

&NO Si la liste des fichiers est vide, seul le WS courant de variables
    est utilis� pour la recherche. Sinon, il faut sp괹ifier WS (en
    majuscules) dans la liste des fichiers, � la position de recherche
    souhait괻.

&FS
Il n'y a pas de fonction plein 괹ran 굌uivalente : l'괹ran
d'ex괹ution d'identit굎 contient ces param둻res.

&SY2
    ##IdtExecute
&TX

&NFS
La liste des fichiers est pass괻 en param둻re de la fonction.

&SY2
    $IdtExecuteVarFiles fichier [liste de fichiers]
&TX

&EX2
    $IdtExecuteVarFiles maribel.var WS test.var
&TX
>

<idtexecutesclfiles>
    IdtExecuteSclFiles
    컴컴컴컴컴컴컴컴컴

&IT Description
컴컴컴컴컴컴
Cette commande fait partie d'un groupe de commandes permettant de
sp괹ifier les param둻res compl굆entaires (fichiers de recherche)
du calcul des identit굎. Les commandes associ괻s sont IdtExecuteVarFiles
et IdtExecute. Le calcul des identit굎 est effectivement lanc�
par la commande IdtExecute.

La fonction permet d'introduire les noms de fichiers o� les scalaires
r괽굍enc굎 dans les identit굎 doivent 늯re recherch굎.

Lorsqu'un scalaire utile � l'ex괹ution d'une identit� est rencontr�, les
fichiers indiqu굎 sont analys굎 � tour de r뱇e. D둺 que l'objet
recherch� est trouv�, il est amen� en m굆oire pour la dur괻 du calcul.
Il est ensuite d굏ruit, sauf s'il provient du WS actif.

Si un objet - variable ou scalaire - n'est ni en WS, ni dans un des
fichiers indiqu굎, le processus de calcul s'arr늯e.

Cette fonction n'a tout son sens que lorsque la commande IdtExecute est
utilis괻 en mode ligne de commande.

&NO Si la liste des fichiers est vide, seul le WS courant de scalaires
    est utilis� pour la recherche. Sinon, il faut sp괹ifier WS (en
    majuscules) dans la liste des fichiers, � la position de recherche
    souhait괻.

&FS
Il n'y a pas de fonction plein 괹ran 굌uivalente : l'괹ran
d'ex괹ution d'identit굎 contient ces param둻res.

&SY2
    ##IdtExecute
&TX

&NFS
La liste des fichiers est pass� en param둻re de la fonction.

&SY2
    $IdtExecuteSclFiles fichier [liste de fichiers]
&TX

&EX2
    $IdtExecuteSclFiles WS maribel.scl test.scl
&TX
>

<reportexec>
    ReportExec
    컴컴컴컴컴

&DE
La commande provoque l'ex괹ution d'un rapport "fils". La fonction est
r괹ursive et autorise plusieurs niveaux de sous-rapports. Lorsque les
rapports "fils" sont termin굎 sans erreur critique, l'ex괹ution continue
avec les commandes du rapport de niveau sup굍ieur.

&FS
La fonction affiche une page de saisie dans laquelle on d괽init le ou
les param둻res de la commande.

&MU

&SY2
    ##ReportExec
&TX

&NFS
Le rapport � ex괹uter et ses arguments sont pass굎 en param둻re de la
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
re뇇it comme argument les 굃굆ents de la liste $LISTE et permet donc
d'ex괹uter un rapport type sur un nombre initialement inconnu
d'arguments. Par exemple, pour remplacer tous les 0 par des NA pour la
liste $LISTE de variables, il suffit de faire :

&CO
    FICHIER ZEROTONA.REP
    컴컴컴컴컴컴컴컴�
	$label again
	$goto fin {%0%=0}
	##show Series trait괻 : %1%
	$DataCalcVar %1% if(%1%=0, 1/0, %1%)
	$shift
	$goto again
	$label fin
&TX
La commande
&CO
	$ReportExec zerotona $LISTE
&TX
replacera les 0 par des NA pour les s굍ies de la liste $LISTE.
>

<reportedit>
    ReportEdit
    컴컴컴컴컴

&DE
Cette fonction permet de cr괻r ou de modifier un rapport.

&FS
La fonction affiche une page de saisie dans laquelle on d괽init le ou
le nom du rapport � 괺iter.

&MU

&SY2
    ##ReportEdit
&TX

&NFS
Le fichier � 괺iter est pass� en param둻re de la commande. L'괺iteur MMT
est lanc� pour l'괺ition du fichier. L'extension .rep peut 늯re omise.

&SY2
    $ReportEdit filename
&TX

&EX2
    $ReportEdit bist92\bistel.rep
&TX
>

<printvar>
    PrintVar
    컴컴컴컴

&DE
Cette commande permet la construction et l'impression des tableaux de
comparaison de s굍ies en format A2M. Les formes LEC sont admises et
doivent 늯re s굋ar괻s par des points-virgules.

Pour d괽inir les fichiers � comparer, il faut utiliser la fonction
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

&NO Il ne peut y avoir de blancs dans le sample g굈굍alis�.
>

<viewvar>
    ViewVar
    컴컴컴�

&DE
Cette commande permet la visualisation des tableaux de comparaison de
s굍ies en format A2M.

&FS
Cette fonction n'existe pas en mode plein 괹ran.

&NFS
&SY2
    $ViewVar gsample lec1 [lec2 ...]

    o� gsample est un sample g굈굍alis� sans blanc
       lec1 et lec2 sont des formes LEC sans blanc
&TX

&EX2
    $ViewVar (80Y1/79Y1):10  X+Y  Z  ln(X)
&TX
&NO Il ne peut y avoir de blancs dans le sample g굈굍alis�.
>

<ViewGr>
    ViewGr
    컴컴컴

&DE
Cette fonction permet de visualiser des tableaux sous forme de graphique.
Plusieurs graphiques peuvent 늯re affich굎 sur le m늤e 괹ran. Le langage
est d괽ini par $PrintLang.

&FS
Cette fonction est d괹rite en d굏ail dans le "Manuel de l'utilisateur".

&SY2
    ##ViewGr
&TX

&NFS
&SY2
    $ViewGr gsample tbl1[+tbl2] tbl3 ...

    o� gsample est un sample g굈굍alis� sans blanc
       tbl1, tbl2 et tbl3 sont des noms de tableaux
&TX
Lorsque deux tableaux sont associ굎 par le signe +, ils sont affich굎
sur le m늤e 괹ran sous forme de deux graphiques s굋ar굎.

&NO Il ne peut y avoir de blancs dans le sample g굈굍alis�.
>

<PrintGrAll>
    PrintGrAll
    컴컴컴컴컴

Cette fonction est annul괻 � partir de la version 5 de IODE. Elle est
remplac괻 par la fonction ~c$PrintGr~C.
>

<PrintGrData>
    PrintGrData
    컴컴컴컴컴�

Cette fonction est annul괻 � partir de la version 5 de IODE.

>


<PrintGrWin>
    PrintGrWin
    컴컴컴컴컴

Cette fonction est annul괻 � partir de la version 5 de IODE.

>


<modelexchange>
    ModelExchange
    컴컴컴컴컴컴�
&DE
Cette fonction permet de d괽inir ou d'annuler les 괹hanges endog둵es-exog둵es avant
de lancer la simulation d'un mod둳e avec recherche d'objectif (Goal
seeking) :

&FS
Cette fonction fait partie de la fonction de simulation :

&SY2
    ##ModelSimulate
&TX

&NFS
&SY2
    $ModelExchange eqname1-varname1,eqname2-varname2,...

    o� eqname1 et eqname2 sont des noms d'굌uations (et donc
			de variables endog둵es)
       varname1 et varname2 sont des noms d'exog둵es
&TX

Pour annuler l'괹hange ENDO-EXO, il suffit d'appeler la fonction sans param둻re :
&CO
    $ModelExchange
&TX
>
<modelcompile>
    ModelCompile
    컴컴컴컴컴컴
&DE
Cette fonction recompile une liste d'굌uations ou toutes les 굌uations
si aucune liste n'est sp괹ifi괻. Elle est uniquement utile si des
굌uations font appel � des macros dans leur forme LEC.

&FS
&MU
&SY2
    ##ModelCompile
&TX
&NFS
&SY2
    $ModelCompile  [eqname1, eqname2, ... ]

    o� eqname1 et eqname2 sont des noms d'굌uations.
&TX
>
<idtexecutetrace>
    IdtExecuteTrace
    컴컴컴컴컴컴컴�
&DE
La trace de la construction des identit굎 peut 늯re sauv괻 dans le
fichier A2M courant d괽ini par $PrintDest. Les informations utiles (par
exemple, le fichier d'origine d'une variable utilis괻 pour construire
l'identit�) seront 굈um굍괻s dans le fichier "Trace file".

&FS
&NEQ
&NFS
Selon le param둻re, la trace est sauv괻 ou non lors du prochain appel �
la commande $IdtExecute.

&CO
    $IdtExecuteTrace  {Yes | No}
&TX
>

<eqsestimate>
    EqsEstimate
    컴컴컴컴컴�
&DE
Cette fonction r괻stime une 굌uation ou un bloc d'굌uations.

&FS
Cette fonction est d굏aill괻 dans la manuel de l'utilisateur. Elle
permet 괾alement de d괽inir et de modifier une 굌uation.

&SY2
    ##EqsEstimate
&TX

&NFS
Seule la p굍iode peut 늯re adapt괻 dans cette fonction. Les autres
param둻res (m굏hode, instruments, etc) sont ceux d괽inis actuellement
dans l'굌uation. D'autres fonctions permettent de modifier les
instruments, la m굏hode : il s'agit des fonctions EqsSet....

&SY2
    $EqsEstimate perfrom perto eqname1 ...
&TX

&TI R굎ultats explicites
컴컴컴컴컴컴컴컴컴컴
En fin d'estimation, certaines variables et scalaires sont automatiquement
cr괻s si le processus a converg�. Ces variables et scalaires peuvent
늯re exploit굎 � des fins de calculs et, comme ils s'agit d'objets
faisant partie du WS, peuvent 늯re sauv굎 pour une utilisation future.

Il s'agit des tests r굎ultant de la derni둹e estimation sont sauv굎
dans des scalaires. Il en va de m늤e pour les r굎idus, membres de
gauche et de droite des 굌uations.

Les tests portent les noms suivants (e0_* pour la premi둹e 굌uation du
bloc, e1_* pour la deuxi둴e, ...) :

&EN e0_n         : nombre de p굍iode du sample
&EN e0_k         : nombre de coefficients estim굎
&EN e0_stdev     : std dev des r굎idus
&EN e0_meany     : moyenne de Y
&EN e0_ssres     : somme du carr굎 des r굎idus
&EN e0_stderr    : std error
&EN e0_stderrp   : std error %
&EN e0_fstat     : F-Stat
&EN e0_r2        : R carr�
&EN e0_r2adj     : R carr� ajust�
&EN e0_dw        : Durbin-Watson
&EN e0_loglik    : Log Likelihood

    Les s굍ies calcul괻s sont 괾alement sauv괻s dans une variable
    sous les noms :

&EN _YCALC0 pour le membre de droite de la premi둹e 굌uation du
	  bloc, _YCALC1 pour la deuxi둴e 굌uation, etc.

&EN _YOBS0 pour le membre de gauche de la premi둹e 굌uation du
	  bloc, _YOBS1 pour la deuxi둴e 굌uation, etc.

&EN _YRES0 pour les r굎idus de la premi둹e 굌uation du bloc, ...

En dehors du sample d'estimation, les valeurs de la s굍ie sont --.
>

<EqsStepWise>
    EqsStepWise
    컴컴컴컴컴�
&DE
Cette fonction estime un bloc d'굌uations et recherche les meilleurs tests
possibles pour toutes les combinaisons de coefficients possibles.

&SY2
    $EqsStepWise from to eqname leccond {r2|fstat}
	from to : p굍iode d'estimation
	eqname  : 굌uation � estimer
	leccond : condition d'acceptation
&TX
>

<EqsSetMethod>
    EqsSetMethod
    컴컴컴컴컴컴
&DE
Cette fonction fixe la m굏hode d'estimation d'une liste d'굌uations.
Elle est utile pour changer de m굏hode d'estimation dans le cours de
l'ex괹ution d'un rapport.

&FS
&NEQ
&NFS
La fonction modifie la m굏hode pour toutes les 굌uations dont le nom est
pass� en param둻re.

&SY2
    $EqsSetMethod   {0|1|2|3} eqname1 eqname2 ...

	o� 0 indique la m굏hode LSQ (moindres carr굎)
	   1 indique la m굏hode Zellner
	   2 indique la m굏hode INF (2 stages avec instruments)
	   3 indique la m굏hode GLS (3 stages avec instruments)
	   4 indique la m굏hode MAXLIK (Maximum likelihood - BHHH - � partir de la version 6.21)

	   eqname1 .. sont des noms d'굌uations existantes
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
    컴컴컴컴컴컴
&DE
Cette fonction fixe la p굍iode d'estimation d'une liste d'굌uations.
Elle est utile pour changer de p굍iode d'estimation dans le cours de
l'ex괹ution d'un rapport.

&FS
&NEQ
&NFS
La fonction modifie la p굍iode pour toutes les 굌uations dont le nom est
pass� en param둻re.

&SY2
    $EqsSetSample from to eqname1 eqname2 ...

	o� from et to sont les p굍iodes limites du sample
	   eqname1 .. sont des noms d'굌uations existantes
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
    컴컴컴컴컴
&DE
Cette fonction fixe le bloc d'estimation d'une liste d'굌uations.
Elle est utile pour fixer le bloc d'estimation dans le cours de
l'ex괹ution d'un rapport.

&FS
&NEQ
&NFS
La fonction fixe le bloc de toutes les 굌uations dont le nom est
pass� en param둻re. Le bloc est la liste d'굌uations elle-m늤e.

&SY2
    $EqsSetBloc eqname1 eqname2 ...

	o� eqname1 .. sont des noms d'굌uations existantes
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
    컴컴컴컴컴컴
&DE
Cette fonction fixe les instruments � utiliser pour l'estimation d'une
굌uation.

&FS
&NEQ
&NFS
La fonction modifie les instruments pour l'굌uation sp괹ifi괻.

&SY2
    $EqsSetInstrs eqname lec_intr_1;lec_instr_2;...

	o� eqname est le nom de l'굌uation
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
    컴컴컴컴�
&DE
Cette fonction fixe le commentaire d'une 굌uation.

&FS
&NEQ
&NFS

&SY2
    $EqsSetCmt eqname comment

	o� eqname est le nom de l'굌uation
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
    컴컴컴컴컴컴컴
&DE
    Permet de ne pas vider le fichier a2m avant l'impression.
    Par d괽aut, les fichiers r굎ultat des impressions sont
    vid굎 lors d'une nouvelle impression.
    Dans le seul cas des fichiers a2m, il est possible d'굒iter
    ce traitement par d괽aut et d'accumuler de la sorte plusieurs
    impressions successives dans un seul et m늤e fichier.

Cette fonction modifie 괾alement la valeur des champs correspondants
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
Ces deux appels d괽inissent le fichier ~ctest.a2m~C comme fichier
r굎ultat des impressions suivantes et ne vide pas ce fichier.
>

<PrintFont>
    PrintFont
    컴컴컴컴�

&DE
Fixe la police de caract둹es pour l'impression des paragraphes de texte.
En plus de la famille, la taille de base (en points) et l'incr굆ent de
la taille peuvent 괾alement 늯re sp괹ifi굎.

La taille de base correspond aux paragraphes de textes, l'incr굆ent
indique de combien de points chaque niveau de titre doit 늯re augment�.

Cette fonction modifie 괾alement la valeur des champs correspondants
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

Le premier caract둹e du nom de la police suffit.

Une fois cette valeur fix괻, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv괻 dans le panneau de configuration des impressions, elle reste
괾alement valable pour les sessions suivantes de IODE.

&EX2
   $PrintFont Times 10 2
&TX
Fixe la taille des paragraphes par_1, enum_1, etc � 10 points, le
paragraphe tit_2 � 12 points, tit_1 � 14 points et tit_0 � 18 points.
>

<PrintTableFont>
    PrintTableFont
    컴컴컴컴컴컴컴
&DE
Fixe la police de caract둹es pour les tableaux seuls. Cette police
peut donc 늯re diff굍ente de celle du reste du texte, � la fois en taille
et en famille.

Cette fonction modifie 괾alement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&NO Cette fonction n'a d'effet qu'avant la fonction $PrintDest et plus en
    cours de rapport.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff굍ente.

&NFS

&SY2
   $PrintTableFont Times|Helvetica|Courier|Bookman|Palatino [size]
&TX

Une fois cette valeur fix괻, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv괻 dans le panneau de configuration des impressions, elle reste
괾alement valable pour les sessions suivantes de IODE.

&EX2
   $PrintTableFont Palatino 8
&TX
Fixe la police pour tous les tableaux � Palatino, 8 points.

>

<PrintTableBox>
    PrintTableBox
    컴컴컴컴컴컴�
&DE
Fixe la largeur du cadre des tableaux en points. Cette valeur peut
est nulle pour 굒iter les cadres.

Cette fonction modifie 괾alement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff굍ente.

&NFS

&SY2
   $PrintTableBox  n
&TX

Une fois cette valeur fix괻, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv괻 dans le panneau de configuration des impressions, elle reste
괾alement valable pour les sessions suivantes de IODE.

&EX2
   $PrintTableBox  0
&TX
Supprime le cadre des tableaux.
>

<PrintTableColor>
    PrintTableColor
    컴컴컴컴컴컴컴�
&DE
Permet d'utiliser ou non la couleur dans les tableaux.

Cette fonction modifie 괾alement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff굍ente.

&NFS

&SY2
   $PrintTableColor [NO|Yes]
&TX

Une fois cette valeur fix괻, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv괻 dans le panneau de configuration des impressions, elle reste
괾alement valable pour les sessions suivantes de IODE.

&EX2
   $PrintTableColor No
&TX
Supprime la couleur dans l'impression des tableaux.
>

<PrintTableWidth>
    PrintTableWidth
    컴컴컴컴컴컴컴�
&DE
Fixe la largeur des tableaux en Frame et Rtf. Les param둻res doivent
늯re exprim굎 en mm. Trois valeurs peuvent 늯re pass괻s :

&EN la largeur totale
&EN la largeur de la premi둹e colonne
&EN la largeur des colonnes suivantes

Cette fonction modifie 괾alement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff굍ente.

&NFS

&SY2
   $PrintTableWidth width [col1 [coln]]
&TX

Une fois cette valeur fix괻, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv괻 dans le panneau de configuration des impressions, elle reste
괾alement valable pour les sessions suivantes de IODE.

&EX2
   $PrintTableWidth 160 60 15
&TX
>
<PrintTableBreak>
    PrintTableBreak
    컴컴컴컴컴컴컴�
&DE
Permet ou non les coupures des tableaux sur plusieurs pages: si un tableau
est plac� sur la page de telle sorte qu'il doivent 늯re imprim� sur deux
pages, un saut de page automatique est g굈굍� avant le tableau.

Cette fonction modifie 괾alement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff굍ente.

&NFS

&SY2
   $PrintTableBreak [NO|Yes]
&TX

Une fois cette valeur fix괻, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv괻 dans le panneau de configuration des impressions, elle reste
괾alement valable pour les sessions suivantes de IODE.

&EX2
   $PrintTableBreak Yes
&TX
Les coupures des tableaux sont autoris괻s lors de l'impression.
>

<PrintTablePage>
    PrintTablePage
    컴컴컴컴컴컴컴
&DE
Force un saut de page avant chaque tableau.

Cette fonction modifie 괾alement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff굍ente.

&NFS

&SY2
   $PrintTablePage  [NO|Yes]
&TX

Une fois cette valeur fix괻, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv괻 dans le panneau de configuration des impressions, elle reste
괾alement valable pour les sessions suivantes de IODE.

&EX2
   $PrintTablePage Yes
&TX
Force un saut de page pour chaque tableau.
>

<PrintBackground>
    PrintBackground
    컴컴컴컴컴컴컴�

&DE
Fixe la couleur du background (tableaux et graphiques). Un tableau
sera hachur� si la valeur des attributs graphiques "Shadow" est
fix괻 � Yes.

Pour 굒iter les background dans tous les tableaux, on peut utiliser
comme background White.

Cette fonction modifie 괾alement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff굍ente.

&NFS

&SY2
   $PrintBackground Black|Blue|Magenta|Cyan|Red|Green|Yellow|White
&TX

Une fois cette valeur fix괻, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv괻 dans le panneau de configuration des impressions, elle reste
괾alement valable pour les sessions suivantes de IODE.

&EX2
   $PrintBackground Blue
&TX
La couleur de la partie hachur괻 en background des graphiques et tableaux
est fix괻 � bleu.
>
<PrintGraphBox>
    PrintGraphBox
    컴컴컴컴컴컴�
&DE
Fixe la largeur du cadre de graphiques en points.

Cette fonction modifie 괾alement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff굍ente.

&NFS

&SY2
   $PrintGraphBox n
&TX

Une fois cette valeur fix괻, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv괻 dans le panneau de configuration des impressions, elle reste
괾alement valable pour les sessions suivantes de IODE.

&EX2
   $PrintGraphBox 2
&TX
Les graphiques suivants auront un cadre 굋ais.
>
<PrintGraphBrush>
    PrintGraphBrush
    컴컴컴컴컴컴컴�
&DE
Fixe la densit� du background des graphiques. La valeur est exprim괻 en
pourcents.

Cette fonction modifie 괾alement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff굍ente.

&NFS

&SY2
   $PrintGraphBrush pct|Yes
&TX

Une fois cette valeur fix괻, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv괻 dans le panneau de configuration des impressions, elle reste
괾alement valable pour les sessions suivantes de IODE.

&EX2
   $PrintGraphBrush 10
&TX
La densit� de la "brosse" de hachure est fix괻 � 10%. La couleur
est d굏ermin괻 par la fonction $PrintBackground.
>

<PrintGraphSize>
    PrintGraphSize
    컴컴컴컴컴컴컴
&DE
Fixe la taille des graphiques en mm et celle de la police utilis괻.

Cette fonction modifie 괾alement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff굍ente.

&NFS

&SY2
   $PrintGraphSize  width [height] [fontsize]
&TX
o� :
&EN ~cwidth~C et ~cheight~C sont exprim굎 en mm.
&EN ~cfontsize~C  est exprim� en points

Une fois cette valeur fix괻, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv괻 dans le panneau de configuration des impressions, elle reste
괾alement valable pour les sessions suivantes de IODE.

&EX2
   $PrintGraphSize 150 60 8
&TX
Les graphiques suivants occuperont un cadre de 15 cm sur 6 cm et les textes auront
8 points de corps.
>

<PrintGraphPage>
    PrintGraphPage
    컴컴컴컴컴컴컴
&DE
Force un saut de page avant chaque graphique.

Cette fonction modifie 괾alement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff굍ente.

&NFS

&SY2
   $PrintGraphPage [NO|Yes]
&TX

Une fois cette valeur fix괻, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv괻 dans le panneau de configuration des impressions, elle reste
괾alement valable pour les sessions suivantes de IODE.

&EX2
   $PrintGraphPage Yes
&TX
Chaque graphique suivant sera plac� en d괷ut de page.
>

<PrintRtfHelp>
    PrintRtfHelp
    컴컴컴컴컴컴

&DE
G굈둹e un fichier RTF pour un help Windows. Ce fichier pourra 늯re
compil� par la suite � l'aide du compilateur HCW de Microsoft.

Les fichiers RTF g굈굍굎 de la sorte ne sont pas identiques �
ceux g굈굍굎 pour WinWord.

Cette fonction modifie 괾alement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff굍ente.
&NFS

&SY2
   $PrintRtfHelp [YES|No]
&TX

Une fois cette valeur fix괻, elle reste d'application pour toutes les
impressions suivantes, y compris dans les rapports suivants. Comme elle
est sauv괻 dans le panneau de configuration des impressions, elle reste
괾alement valable pour les sessions suivantes de IODE.


&EX2
   $PrintRtfHelp YES
&TX
Le fichier suivant d괽ini par $PrintDest ... RTF sera du type Help
Windows.
>

<PrintRtfTopic>
    PrintRtfTopic
    컴컴컴컴컴컴�
&DE
Cr괻 un nouveau topic pour un fichier d'aide Windows (ou a2m).
Ce topic fera partie de la table des mati둹es automatique g굈굍굎 par
IODE. Le niveau hi굍archique du topic est fix� par la commande
~c$PrintRtfLevel~C.

Le texte du topic sera 괾alement repris comme titre.

&FS
&NEQ
&NFS

&SY2
   $PrintRtfTopic titre du topic
&TX

&EX2
   $PrintRtfTopic Tableaux r괹apitulatifs
&TX
G굈둹e en a2m (voir syntaxe du langage a2m) :

&CO
    .topic [num굍o_auto] [level] Tableaux r괹apitulatifs
    .par1 tit_[level]
    Tableaux r괹apitulatifs

&TX
Si le fichier g굈굍� est un fichier a2m, la commande ~c.topic~C n'est
exploit괻 que dans le cas d'une traduction en Help Windows.
Par contre, ~c.par1~C ~ctit_....~C et le texte du topic g굈굍eront �
l'impression un titre.
>
<PrintRtfLevel>
    PrintRtfLevel
    컴컴컴컴컴컴�
&DE
Change le niveau hi굍archique des topics suivants. Ce changement de
niveau permet de g굈굍er des indentations dans la table de mati둹es du
fichier d'aide r굎ultat.

&FS
&NEQ
&NFS

&SY2
   $PrintRtfLevel [+|-|n]
&TX

&EX2
   $PrintRtfLevel +
&TX
Le niveau du topic suivant sera incr굆ent� de 1, cr괶nt de la sorte
une indentation dans la table des mati둹es g굈굍괻.
>
<PrintRtfTitle>
    PrintRtfTitle
    컴컴컴컴컴컴�
&DE
Fixe le titre du fichier d'aide Windows.

Cette fonction modifie 괾alement la valeur des champs correspondants
dans le panneau ~b"Print Preferences/RTF"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup".
&NFS

&SY2
   $PrintRtfTitle titre du help
&TX

&EX2
   $PrintRtfTitle Mod둳e Hermes
&TX
>
<PrintRtfCopyright>
    PrintRtfCopyright
    컴컴컴컴컴컴컴컴�
&DE
Fixe le texte du copyright de l'aide Windows. Ce texte appara똳 dans
l'option Help du programme Winhelp.

Cette fonction modifie 괾alement la valeur des champs correspondants
dans le panneau ~b"Print Preferences/RTF"~B.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup".
&NFS

&SY2
   $PrintRtfCopyright copyright text
&TX

&EX2
   $PrintRtfCopyright (c) Bureau f괺굍al du Plan - 1998
&TX
>
<PrintParanum>
    PrintParanum
    컴컴컴컴컴컴
&DE
Permet de num굍oter les titres lors de l'impression (1., 1.1, ...).

Cette fonction modifie 괾alement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&NO Cette fonction n'a d'effet qu'avant la fonction $PrintDest et plus en
    cours de rapport.

&FS
    Cette option est modifiable dans le Panneau "Print Preferences",
    accessible dans la fonction "Print Setup". Chaque destination
    peut prendre une valeur diff굍ente.

&NFS

&SY2
   $PrintParanum NO
   $PrintParanum [NO|Yes]
&TX

&EX2
   $PrintParanum NO
&TX
Supprime la num굍otation automatique des titres.
>
<PrintPageHeader>
    PrintPageHeader
    컴컴컴컴컴컴컴�
&DE
Fixe le titre des pages imprim괻s. Les caract둹es %d sont remplac굎 par
le num굍o de la page courante.

Cette fonction modifie 괾alement la valeur des champs correspondants
dans le panneau ~b"Print Preferences"~B.

&FS
    Cette option est modifiable dans le Panneau "Print
    Preferences/Printer", accessible dans la fonction "Print Setup".

&NFS

&SY2
   $PrintPageHeader titre des pages suivantes
&TX

&EX2
   $PrintPageHeader Mod둳e Modtrim
&TX
>
<PrintPageFooter>
    PrintPageFooter
    컴컴컴컴컴컴컴�
&DE
Fixe la footnote des pages imprim괻s. Les caract둹es %d sont remplac굎
par le num굍o de la page courante.

Cette fonction modifie 괾alement la valeur des champs correspondants
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
    컴컴컴컴컴
&DE
Fixe l'imprimante Windows par d괽aut pour les impressions suivantes.

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
    컴컴컴컴컴컴컴컴
&DE
Fixe l'orientation de l'imprimante Windows courante.

Cette fonction modifie 괾alement la valeur des champs correspondants
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
    컴컴컴컴컴�
&DE
Fixe le mode recto-verso de l'imprimante Windows courante.

Cette fonction modifie 괾alement la valeur des champs correspondants
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
    컴컴컴컴컴컴

&DE
Sp괹ifie que la prochaine g굈굍ation de fichier Html devra se faire pour
un HtmlHelp de Windows. Ce fichier pourra 늯re compil� par la suite �
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
    컴컴컴컴컴컴

&DE
Sp괹ifie que la prochaine g굈굍ation de fichier Html ne doit pas inclure
les sections de header et de footer HTML (<<HEAD>>, <<BODY>>, <</BODY>>,
etc). Seul le texte proprement dit est traduit en vue d'늯re incorpor�
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
    컴컴컴컴컴

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
    컴컴컴컴컴컴컴

Deze functie cre뎓rt lijsten met daarin de namen van objecten
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
    컴컴컴컴컴컴컴컴컴
Maakt alle WS-en leeg.
>
