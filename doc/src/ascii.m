/* formats ascii de IODE */

<Format ASCII de IODE>
    Format ASCII de IODE
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Tous les objets de IODE peuvent ˆtre d‚crits par un format ASCII
appropri‚. L'int‚rˆt en est ‚vident: d'une part il est possible d'‚diter
tous les objets … l'aide d'un simple ‚diteur ASCII, d'autre part, il est
ais‚ de d‚velopper des filtres permettant de convertir les d‚finitions
d'objet vers d'autres langages d'interface.

Les formats ASCII de IODE se composent des ‚l‚ments suivants:

&EN un nom d'objet
&EN des d‚limiteurs de blocs (en g‚n‚ral { et })
&EN des mots cl‚s sp‚cifiques suivis de leurs paramŠtres
&EN des commentaires.

Nous allons d‚crire successivement les formats ASCII des dif‚rents
objets:

&EN <format ASCII des commentaires>
&EN <format ASCII des ‚quations   >
&EN <format ASCII des identit‚s   >
&EN <format ASCII des listes      >
&EN <format ASCII des scalaires   >
&EN <format ASCII des tableaux    >
&EN <format ASCII des variables   >
>


<format ASCII des commentaires>
    Format ASCII des commentaires
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&IT D‚finition
ÄÄÄÄÄÄÄÄÄÄÄ

Le format ASCII d'un fichiers de commentaire se compose successivement
de la d‚finition ASCII individuelle des commentaires. La fin du fichier
signale la fin de la d‚finition des commentaires.

&CO
    d‚but du fichier
    format ASCII commentaire_1
    format ASCII commentaire_2
    format ASCII commentaire_3
    format ASCII commentaire_4
    ...
    fin du fichier
&TX

Le format ASCII d'un commentaire se compose:

&EN du nom du commentaire (un nom valide au sens IODE, c-…-d un nom de
10 caractŠres maximum (20 … partir de la version 6.01) -lettre ou chiffre-
commencant toujours par une lettre -ou le caractŠre '_'- ,en majuscule)
suivi du texte du commentaire entre guillemets.

&EX2
    CMT1 "ceci est le commentaire 1"
    CMT2 "ceci est le commentaire 2"
&TX
>

<format ASCII des identit‚s   >
    Format ASCII des identit‚s
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&IT D‚finition
ÄÄÄÄÄÄÄÄÄÄÄ

Le format ASCII d'un fichiers d'identit‚s se compose successivement
de la d‚finition ASCII individuelle des identit‚s. La fin du fichier
signale la fin de la d‚finition des commentaires.

&CO
    d‚but du fichier
    format ASCII identit‚_1
    format ASCII identit‚_2
    format ASCII identit‚_3
    format ASCII identit‚_4
    ...
    fin du fichier
&TX

Le format ASCII d'une identit‚ se compose:

&EN du nom de l'identit‚ (un nom valide au sens IODE, c-…-d un nom de
10 caractŠres maximum (20 … partir de la version 6.01) -lettre ou chiffre- commencant toujours par une
lettre -ou le caractŠre '_'- ,en majuscule) suivi

&EN de la forme LEC de l'identit‚ entre guillemets.

&EX2
    IDT1 "A + B + C"
    IDT2 "ln( pi * (A - 1) / e) * ( 1 / (C + D))"
    IDT3 "1 / IDT1"
&TX
>

<format ASCII des ‚quations   >
    Format ASCII des ‚quations
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Un fichier ASCII d'‚quations est compos‚ d'une suite de d‚finition
d'‚quations. Pour chaque ‚quation, on trouve son nom suivi, entre
accolades, des informations qui la d‚finissent.

Parmi ces informations, seule la forme LEC (entre guillemets) est
obligatoire. Les autres sont optionnelles et concernent essentiellement
l'estimation.

Les informations optionnelles sont :

&EN un commentaire : COMMENT "texte"

&EN la m‚thode d'estimation pouvant prendre une des valeurs suivantes :
&EN2 LSQ : m‚thode des moindres carr‚s
&EN2 ZELLNER : m‚thode de Zellner
&EN2 INF : m‚thode instrumentale (2 stages)
&EN2 GLS : moindre carr‚s g‚n‚ralis‚s

&EN le sample d'estimation sous la forme SAMPLE from to

&EN la date de derniŠre mise … jour : DATE yyyymmdd

&EN le block auquel appartient l'‚quation sous la forme BLOC "eq1;eq2;..."

&EN les instruments servant dans l'estimation : INSTRUMENTS "lec1;lec2..."

&EN les tests statistiques r‚sultant de l'estimation :
&EN2        standard deviation : STDEV valeur
&EN2        moyenne des observations : MEANY valeur
&EN2        somme des carr‚s des r‚sidus : SSRES valeur
&EN2        standard error : STDERR valeur
&EN2        f-stat : FSTAT valeur
&EN2        r carr‚ : R2 valeur
&EN2        r carr‚ ajust‚ : R2ADJ valeur
&EN2        Durbin-Watson : DW valeur
&EN2        Log likelihood : LOGLIK valeur


&EX2
B {
	"B := c1 + c2 * A"
		BLOCK "B"
	COMMENT " "
	SAMPLE 1970Y1 1990Y1
	DATE 19930226
}

X {
	"X := c1 + c2 * Y"
	LSQ
	BLOCK "X"
	COMMENT " "
	SAMPLE 1960Y1 1980Y1
	DATE 19921018
	STDEV 0.228351
	MEANY 2.68489
	SSRES 0.00182236
	STDERR 0.0142297
	FSTAT 2566.21
	R2 0.996505
	R2ADJ 0.996117
	DW 0.512136
	LOGLIK 32.272
}
&TX
>

<format ASCII des listes      >
    Format ASCII des listes
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&IT D‚finition
ÄÄÄÄÄÄÄÄÄÄÄ

Le format ASCII d'un fichiers de listes se compose successivement
de la d‚finition ASCII individuelle des listes. La fin du fichier
signale la fin de la d‚finition des commentaires.

&CO
    d‚but du fichier
    format ASCII liste_1
    format ASCII liste_2
    format ASCII liste_3
    format ASCII liste_4
    ...
    fin du fichier
&TX

Le format ASCII d'une liste se compose:

&EN du nom de la liste (un nom valide au sens IODE, c-…-d un nom de
10 caractŠres maximum (20 … partir de la version 6.01) -lettre ou chiffre- commencant toujours par une
lettre -ou le caractŠre '_'- ,en majuscule) suivi

&EN du texte de la liste entre guillemets.

&EX2
    BLOCK "VAR3 VAR4"
    LIST1 "VAR1 VAR2 $BLOCK"
    LIST2 "TBL1 TBL2"
&TX
>

<format ASCII des scalaires   >
    Format ASCII des scalaires
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&IT D‚finition
ÄÄÄÄÄÄÄÄÄÄÄ

Le format ASCII d'un fichiers de scalaires se compose successivement
de la d‚finition ASCII individuelle des scalaires. La fin du fichier
signale la fin de la d‚finition des commentaires.

&CO
    d‚but du fichier
    format ASCII scal_1
    format ASCII scal_2
    format ASCII scal_3
    format ASCII scal_4
    ...
    fin du fichier
&TX

Le format ASCII d'un scalaire se compose:

&EN du nom du scalaire (un nom valide au sens IODE, c-…-d un nom de
10 caractŠres maximum (20 … partir de la version 6.01) -lettre ou chiffre- commencant toujours par une
lettre -ou le caractŠre '_'- ,en minuscule) suivi

&EN de la valeur du scalaire, suivi

&EN du paramŠtre de relaxation, suivi des deux r‚sultats de l'estimation
du paramŠtre (‚cart type et test t) ou na (ces derniŠres valeurs n'on
qu'un r“le d'information).


&EX2
    c1 0.9 1 na
    c2 1.4567 0.5 na
    c3 -0.98765 0.8 na
&TX
>

<format ASCII des tableaux    >
    Format ASCII des tableaux
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&IT D‚finition
ÄÄÄÄÄÄÄÄÄÄÄ

&NO
Attention: il s'agit bien de la d‚finition de l'objet tableau et non de
la d‚finition de l'output tableau, celui-ci ‚tant d‚crit par ailleurs.
&TX

Le format ASCII d'un fichier de tableaux de compose de la succession des
d‚finitions des tableaux individuels. La s‚rie des tableaux se termine …
la fin du fichier.

&CO
    d‚but du fichier
    format ASCII tableau_1
    format ASCII tableau_2
    format ASCII tableau_3
    format ASCII tableau_4
    ...
    fin du fichier
&TX


Le format ASCII d'un tableau se compose:

&EN d'un nom de tableau (un nom valide au sens IODE, c-…-d un nom de 10
caractŠres maximum (20 … partir de la version 6.01) -lettre ou chiffre- commencant toujours par une
lettre -ou le caractŠre '_'- ,en majuscule)

&EN et d'un bloc de d‚finition commencant par '{' et se terminant par '}'

&CO
	NOM                             nom du tableau
	{                               d‚but de bloc
	    .....                       bloc de d‚finition
	}                               fin de bloc
&TX

&IT Le bloc de d‚finition contient
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&EN en premiŠre ligne le mot cl‚ DIM suivi de la dimension du tableau.
La dimension du tableau est 1 minimum. La premiŠre colonne est suppos‚e
ˆtre une colonne de titres -elle n'est pas r‚p‚t‚e-, les colonnes
suivantes sont r‚p‚t‚es pour chaque p‚riode du sample. La dimension du
tableau qui sera compil‚ et imprim‚ sera de (1 + (DIM-1) x nb. p‚riodes
du sample).

&EN les attributs graphiques :
&CO
	BOX {0 | 1}
	XGRID {0 | 1 | 2} : None, Minor, Major Grids
	YGRID {0 | 1 | 2} : None, Minor, Major Grids
	ALIGN {LEFT | Center | Right}
	YMIN na ou valeur
	YMAX na ou valeur
	ZMIN na ou valeur
	ZMAX na ou valeur
&TX

&EN en deuxiŠme ligne, optionnellement, les diviseurs. La ligne
des diviseurs est indiqu‚e par le mot-cl‚ DIV. Les diviseurs sont des
formules en LEC (rep‚r‚es par le mot cl‚ LEC suivi de la forme LEC entre
guillemets), valant LEC "1" par d‚faut. Le diviseur de la derniŠre
colonne de la d‚finition de tableau est report‚ sur toutes les colonnes
suivantes.

&EN successivements sur les lignes suivantes le contenu du tableau: des
titres (TITLE), des lignes (LINE), des chaŒnes de caractŠres quelconques
("...") et des formes LEC (LEC "...."). Chaque ligne d‚bute par le signe
'-'.

&EN … n'importe quelle ligne (mais g‚n‚ralement en fin de tableau) le
mot cl‚ FILES qui sert … imprimer les r‚f‚rences de fichiers

&EN … n'importe quelle ligne (mais g‚n‚ralement en fin de tableau) le
mot cl‚ MODES qui sert … imprimer la signification des signes utilis‚s dans
le sample (growth rate, ...).

&EN … n'importe quelle ligne, des commentaires d‚limit‚s par "/*" et "*/"

&EN pour chaque ligne, les informations graphiques :
&CO
    GRLINE | GRBAR | GRSCATTER : type de graphique
    RAXIS | LAXIS : axe de r‚f‚rence

&CO

    NOM                                 nom du tableau
    {                                   d‚but de bloc
    DIM 2                               dimension
    DIV LEC "diviseur1" LEC "diviseur2" diviseurs
    - TITLE "titre principal"           ligne de titre
    - TITLE "sous-titre"                ligne de titre
    - LINE                              ligne s‚paratrice
    - " " LEFT "##s" LEFT                ligne de p‚riodes
    - LINE                              ligne s‚paratrice
    - "..." LEFT LEC "formule1"         ligne de cellules
    - "..." LEFT LEC "formule2"             idem
    - "" ""                                 idem
    - LINE                              ligne s‚paratice
    - FILES                             r‚f‚rences des fichiers
    - MODE                              signes du sample
    /* END OF TABLE */                  commentaire
    }                                   fin de bloc
&TX

&IT La ligne des diviseurs se construit de la fa‡on suivante
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&EN la ligne d‚bute par le mot cl‚ DIV

&EN le mot cl‚ DIV est suivi de cellules, autant par ligne
    que la dimension du tableau. Chaque cellule une forme LEC
    repr‚sent‚e entre guillemets, pr‚c‚d‚e du mot cle LEC (cette formule
    sera calcul‚e pour chaque p‚riode du sample d'impression le r‚sultat
    du calcul sera appliqu‚ en diviseur des cellules … contenu num‚rique
    de la colonne correspondante).

Chaque cellule peut ˆtre suivie d'un mot cl‚ indiquant la justification
de la colonne du tableau … l'impression (les mots cl‚s admis sont LEFT
-option par d‚faut-, RIGHT, CENTER et DECIMAL) et/ou un type
d'enrichissement typographique (sont admis les types BOLD, ITALIC,
UNDERLINE et, NORMAL -par d‚faut-). La justification et l'enrichissement
typographique d‚finis au niveu du diviseur de la colonne sont appliqu‚s
par d‚faut … toutes les cellules de la colonne.


&IT Le "contenu" du tableau est construit de la fa‡on suivante
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&EN chaque ligne d‚bute par le caractŠre '-'

&EN les lignes de titre sont d‚sign‚es par le mot cl‚ TITLE suivi entre
    guillemets de la chaŒne de caractŠre du titre. La chaine de
    caractŠres peut ˆtre suivie d'un mot cl‚ indiquant la justification
    du titre … l'impression (les mots cl‚s admis sont LEFT -option par
    d‚faut-, RIGHT, CENTER et DECIMAL) et/ou un type d'enrichissement
    typographique (sont admis les types BOLD, ITALIC et, NORMAL -par
    d‚faut-). Le titre sera imprim‚ … travers toutes les cellules du
    tableau avec la justification et les enrichissement typographiques
    sp‚cifi‚s.

&EN les lignes s‚paratrices sont d‚sign‚es par le mot cl‚ LINE sans
    paramŠtre. Une ligne transversale sera imprim‚e dans le tableau cet
    endroit.

&EN les lignes ordinaires sont constitu‚es de cellules, autant par ligne
    que la dimension du tableau. Chaque cellule est soit une chaŒne de
    caractŠres d‚limit‚e par des guillemets (cette chaŒne sera imprim‚e
    telle quelle … la bonne place dans le tableau), soit une forme LEC
    repr‚sent‚e entre guillemets, pr‚c‚d‚e du mot cle LEC (cette formule
    sera calcul‚e pour chaque p‚riode du sample d'impression et le
    r‚sultat sera imprim‚). Chaque cellule peut ˆtre suivie d'un ou
    plusieurs mot cl‚ indiquant sa justification dans la colonne du
    tableau … l'impression (les mots cl‚s admis sont LEFT -option par
    d‚faut-, RIGHT, CENTER et DECIMAL) et/ou un type d'enrichissement
    typographique (sont admis les types BOLD, ITALIC, UNDERLINE et,
    NORMAL -par d‚faut-)

&EN une cellule consitu‚e d'une chaŒne de caractŠres peut contenir une
    des macros suivantes: ##s sera remplac‚ … l'impression par le sample
    courrant complet, ##t sera remplac‚ … l'impression par la p‚riode
    courrante, et ##f sera remplac‚ … l'impression par le nom du fichier
    trait‚.

&CO
    exemple : "File ##f - Per. ##s"
&TX

&EX2
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

&IT R‚sum‚ des mots cl‚s
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&EN { et } : d‚limiteurs de bloc

&EN " : d‚limiteur de chaŒne de caractŠre (ou forme LEC)

&EN /* et */ : d‚limiteurs de commentaires

&EN DIM nb : dimension du tableau, nb := nombre entier positif

&EN DIV cellules : ligne de diviseurs, cellules := formes LEC

&EN LEFT, CENTER, RIGHT ou DECIMAL : justification de la cellule

&EN NORMAL, ITALIC, UNDERLINE ou BOLD : enrichissement typographique de
la cellule

&EN TITLE "chaŒne" : ligne de titre, chaŒne := le titre … imprimer

&EN LINE : ligne s‚paratrice

&EN "chaŒne" : chaŒne := contenu non num‚rique d'une cellule ordinaire

&EN LEC "formule" : cellule … contenu num‚rique calcul‚ par la formule
LEC

&EN FILES : r‚f‚rence du/des fichier(s)

&EN MODE : signification des sigles imprim‚s par la macro ##s

&EN ##s, ##t et ##f : macros d‚finissant l'impression du sample, de la
    p‚riode et du ou des fichier(s).


>

<format ASCII des variables   >
    Format ASCII des variables
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&IT D‚finition
ÄÄÄÄÄÄÄÄÄÄÄ

Le format ASCII d'un fichier de variables se compose de la fa‡on
suivante:

&EN sur la premiŠre ligne: le mot cl‚ ~isample~I suivi ~ides p‚riodes
    limites d'‚chantillonnage~I

&EN successivement sur les lignes suivantes et … raison d'une par ligne,
    les variables en commencant par leur ~inom~I (un nom valide au sens
    IODE, c-…-d un nom de 10 caractŠres maximum (20 … partir de la version
    6.01) -lettre ou chiffre- commencant toujours par une lettre -ou le
    caractŠre '_'- ,en majuscule) suivi des ~ivaleurs (ou na)~I pour chaque
    p‚riode. Chaque ‚l‚ment (nom et valeurs) sont s‚par‚s par un blanc.

&EN la fin de fichier signale la fin de d‚finition des variables.

&IT mot(s) cl‚s
ÄÄÄÄÄÄÄÄÄÄÄÄ

Un seul mot cl‚ est utilis‚:

&CO
    sample yyyyPppp  yyyyPppp
		   1         2
&TX

La d‚finition des p‚riodes limites doit r‚pondre … la syntaxe suivante:

&CO
    yyyyPppp    yyyy est l'ann‚e
		P est la p‚riodicit‚
		    P := Y (yearly), Q (quaterly), M (monthly), W (weekly)
		ppp est la p‚riode dans l'ann‚e

&EX2

sample 1970Y1 1996Y1
A0G 621800 644700 805700 ... 874900 939000 1.0538e+06 1.1965e+06
A0GBNB 49500 40800 39600 ... 42900 46100 48400 65500 88900 108600
 ...
A0GCCP 57300 57200 66000 ... 63400 65700 69400 76800 79900 83700 80900
A0GFMI 12400 2400 6100 7300 ... 6600 4700 1500 4100 19500 19800 37000
&TX
>






