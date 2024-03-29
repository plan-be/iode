/* formats ascii de IODE */

<Format ASCII de IODE>
    Format ASCII de IODE
    컴컴컴컴컴컴컴컴컴컴

Tous les objets de IODE peuvent 늯re d괹rits par un format ASCII
appropri�. L'int굍늯 en est 굒ident: d'une part il est possible d'괺iter
tous les objets � l'aide d'un simple 괺iteur ASCII, d'autre part, il est
ais� de d굒elopper des filtres permettant de convertir les d괽initions
d'objet vers d'autres langages d'interface.

Les formats ASCII de IODE se composent des 굃굆ents suivants:

&EN un nom d'objet
&EN des d굃imiteurs de blocs (en g굈굍al { et })
&EN des mots cl굎 sp괹ifiques suivis de leurs param둻res
&EN des commentaires.

Nous allons d괹rire successivement les formats ASCII des dif굍ents
objets:

&EN <format ASCII des commentaires>
&EN <format ASCII des 굌uations   >
&EN <format ASCII des identit굎   >
&EN <format ASCII des listes      >
&EN <format ASCII des scalaires   >
&EN <format ASCII des tableaux    >
&EN <format ASCII des variables   >
>


<format ASCII des commentaires>
    Format ASCII des commentaires
    컴컴컴컴컴컴컴컴컴컴컴컴컴컴�

&IT D괽inition
컴컴컴컴컴�

Le format ASCII d'un fichiers de commentaire se compose successivement
de la d괽inition ASCII individuelle des commentaires. La fin du fichier
signale la fin de la d괽inition des commentaires.

&CO
    d괷ut du fichier
    format ASCII commentaire_1
    format ASCII commentaire_2
    format ASCII commentaire_3
    format ASCII commentaire_4
    ...
    fin du fichier
&TX

Le format ASCII d'un commentaire se compose:

&EN du nom du commentaire (un nom valide au sens IODE, c-�-d un nom de
10 caract둹es maximum (20 � partir de la version 6.01) -lettre ou chiffre-
commencant toujours par une lettre -ou le caract둹e '_'- ,en majuscule)
suivi du texte du commentaire entre guillemets.

&EX2
    CMT1 "ceci est le commentaire 1"
    CMT2 "ceci est le commentaire 2"
&TX
>

<format ASCII des identit굎   >
    Format ASCII des identit굎
    컴컴컴컴컴컴컴컴컴컴컴컴컴

&IT D괽inition
컴컴컴컴컴�

Le format ASCII d'un fichiers d'identit굎 se compose successivement
de la d괽inition ASCII individuelle des identit굎. La fin du fichier
signale la fin de la d괽inition des commentaires.

&CO
    d괷ut du fichier
    format ASCII identit�_1
    format ASCII identit�_2
    format ASCII identit�_3
    format ASCII identit�_4
    ...
    fin du fichier
&TX

Le format ASCII d'une identit� se compose:

&EN du nom de l'identit� (un nom valide au sens IODE, c-�-d un nom de
10 caract둹es maximum (20 � partir de la version 6.01) -lettre ou chiffre- commencant toujours par une
lettre -ou le caract둹e '_'- ,en majuscule) suivi

&EN de la forme LEC de l'identit� entre guillemets.

&EX2
    IDT1 "A + B + C"
    IDT2 "ln( pi * (A - 1) / e) * ( 1 / (C + D))"
    IDT3 "1 / IDT1"
&TX
>

<format ASCII des 굌uations   >
    Format ASCII des 굌uations
    컴컴컴컴컴컴컴컴컴컴컴컴컴

Un fichier ASCII d'굌uations est compos� d'une suite de d괽inition
d'굌uations. Pour chaque 굌uation, on trouve son nom suivi, entre
accolades, des informations qui la d괽inissent.

Parmi ces informations, seule la forme LEC (entre guillemets) est
obligatoire. Les autres sont optionnelles et concernent essentiellement
l'estimation.

Les informations optionnelles sont :

&EN un commentaire : COMMENT "texte"

&EN la m굏hode d'estimation pouvant prendre une des valeurs suivantes :
&EN2 LSQ : m굏hode des moindres carr굎
&EN2 ZELLNER : m굏hode de Zellner
&EN2 INF : m굏hode instrumentale (2 stages)
&EN2 GLS : moindre carr굎 g굈굍alis굎

&EN le sample d'estimation sous la forme SAMPLE from to

&EN la date de derni둹e mise � jour : DATE yyyymmdd

&EN le block auquel appartient l'굌uation sous la forme BLOC "eq1;eq2;..."

&EN les instruments servant dans l'estimation : INSTRUMENTS "lec1;lec2..."

&EN les tests statistiques r굎ultant de l'estimation :
&EN2        standard deviation : STDEV valeur
&EN2        moyenne des observations : MEANY valeur
&EN2        somme des carr굎 des r굎idus : SSRES valeur
&EN2        standard error : STDERR valeur
&EN2        f-stat : FSTAT valeur
&EN2        r carr� : R2 valeur
&EN2        r carr� ajust� : R2ADJ valeur
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
    컴컴컴컴컴컴컴컴컴컴컴�

&IT D괽inition
컴컴컴컴컴�

Le format ASCII d'un fichiers de listes se compose successivement
de la d괽inition ASCII individuelle des listes. La fin du fichier
signale la fin de la d괽inition des commentaires.

&CO
    d괷ut du fichier
    format ASCII liste_1
    format ASCII liste_2
    format ASCII liste_3
    format ASCII liste_4
    ...
    fin du fichier
&TX

Le format ASCII d'une liste se compose:

&EN du nom de la liste (un nom valide au sens IODE, c-�-d un nom de
10 caract둹es maximum (20 � partir de la version 6.01) -lettre ou chiffre- commencant toujours par une
lettre -ou le caract둹e '_'- ,en majuscule) suivi

&EN du texte de la liste entre guillemets.

&EX2
    BLOCK "VAR3 VAR4"
    LIST1 "VAR1 VAR2 $BLOCK"
    LIST2 "TBL1 TBL2"
&TX
>

<format ASCII des scalaires   >
    Format ASCII des scalaires
    컴컴컴컴컴컴컴컴컴컴컴컴컴

&IT D괽inition
컴컴컴컴컴�

Le format ASCII d'un fichiers de scalaires se compose successivement
de la d괽inition ASCII individuelle des scalaires. La fin du fichier
signale la fin de la d괽inition des commentaires.

&CO
    d괷ut du fichier
    format ASCII scal_1
    format ASCII scal_2
    format ASCII scal_3
    format ASCII scal_4
    ...
    fin du fichier
&TX

Le format ASCII d'un scalaire se compose:

&EN du nom du scalaire (un nom valide au sens IODE, c-�-d un nom de
10 caract둹es maximum (20 � partir de la version 6.01) -lettre ou chiffre- commencant toujours par une
lettre -ou le caract둹e '_'- ,en minuscule) suivi

&EN de la valeur du scalaire, suivi

&EN du param둻re de relaxation, suivi des deux r굎ultats de l'estimation
du param둻re (괹art type et test t) ou na (ces derni둹es valeurs n'on
qu'un r뱇e d'information).


&EX2
    c1 0.9 1 na
    c2 1.4567 0.5 na
    c3 -0.98765 0.8 na
&TX
>

<format ASCII des tableaux    >
    Format ASCII des tableaux
    컴컴컴컴컴컴컴컴컴컴컴컴�

&IT D괽inition
컴컴컴컴컴�

&NO
Attention: il s'agit bien de la d괽inition de l'objet tableau et non de
la d괽inition de l'output tableau, celui-ci 굏ant d괹rit par ailleurs.
&TX

Le format ASCII d'un fichier de tableaux de compose de la succession des
d괽initions des tableaux individuels. La s굍ie des tableaux se termine �
la fin du fichier.

&CO
    d괷ut du fichier
    format ASCII tableau_1
    format ASCII tableau_2
    format ASCII tableau_3
    format ASCII tableau_4
    ...
    fin du fichier
&TX


Le format ASCII d'un tableau se compose:

&EN d'un nom de tableau (un nom valide au sens IODE, c-�-d un nom de 10
caract둹es maximum (20 � partir de la version 6.01) -lettre ou chiffre- commencant toujours par une
lettre -ou le caract둹e '_'- ,en majuscule)

&EN et d'un bloc de d괽inition commencant par '{' et se terminant par '}'

&CO
	NOM                             nom du tableau
	{                               d괷ut de bloc
	    .....                       bloc de d괽inition
	}                               fin de bloc
&TX

&IT Le bloc de d괽inition contient
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�

&EN en premi둹e ligne le mot cl� DIM suivi de la dimension du tableau.
La dimension du tableau est 1 minimum. La premi둹e colonne est suppos괻
늯re une colonne de titres -elle n'est pas r굋굏괻-, les colonnes
suivantes sont r굋굏괻s pour chaque p굍iode du sample. La dimension du
tableau qui sera compil� et imprim� sera de (1 + (DIM-1) x nb. p굍iodes
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

&EN en deuxi둴e ligne, optionnellement, les diviseurs. La ligne
des diviseurs est indiqu괻 par le mot-cl� DIV. Les diviseurs sont des
formules en LEC (rep굍괻s par le mot cl� LEC suivi de la forme LEC entre
guillemets), valant LEC "1" par d괽aut. Le diviseur de la derni둹e
colonne de la d괽inition de tableau est report� sur toutes les colonnes
suivantes.

&EN successivements sur les lignes suivantes le contenu du tableau: des
titres (TITLE), des lignes (LINE), des cha똭es de caract둹es quelconques
("...") et des formes LEC (LEC "...."). Chaque ligne d괷ute par le signe
'-'.

&EN � n'importe quelle ligne (mais g굈굍alement en fin de tableau) le
mot cl� FILES qui sert � imprimer les r괽굍ences de fichiers

&EN � n'importe quelle ligne (mais g굈굍alement en fin de tableau) le
mot cl� MODES qui sert � imprimer la signification des signes utilis굎 dans
le sample (growth rate, ...).

&EN � n'importe quelle ligne, des commentaires d굃imit굎 par "/*" et "*/"

&EN pour chaque ligne, les informations graphiques :
&CO
    GRLINE | GRBAR | GRSCATTER : type de graphique
    RAXIS | LAXIS : axe de r괽굍ence

&CO

    NOM                                 nom du tableau
    {                                   d괷ut de bloc
    DIM 2                               dimension
    DIV LEC "diviseur1" LEC "diviseur2" diviseurs
    - TITLE "titre principal"           ligne de titre
    - TITLE "sous-titre"                ligne de titre
    - LINE                              ligne s굋aratrice
    - " " LEFT "##s" LEFT                ligne de p굍iodes
    - LINE                              ligne s굋aratrice
    - "..." LEFT LEC "formule1"         ligne de cellules
    - "..." LEFT LEC "formule2"             idem
    - "" ""                                 idem
    - LINE                              ligne s굋aratice
    - FILES                             r괽굍ences des fichiers
    - MODE                              signes du sample
    /* END OF TABLE */                  commentaire
    }                                   fin de bloc
&TX

&IT La ligne des diviseurs se construit de la fa뇇n suivante
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�

&EN la ligne d괷ute par le mot cl� DIV

&EN le mot cl� DIV est suivi de cellules, autant par ligne
    que la dimension du tableau. Chaque cellule une forme LEC
    repr굎ent괻 entre guillemets, pr괹괺괻 du mot cle LEC (cette formule
    sera calcul괻 pour chaque p굍iode du sample d'impression le r굎ultat
    du calcul sera appliqu� en diviseur des cellules � contenu num굍ique
    de la colonne correspondante).

Chaque cellule peut 늯re suivie d'un mot cl� indiquant la justification
de la colonne du tableau � l'impression (les mots cl굎 admis sont LEFT
-option par d괽aut-, RIGHT, CENTER et DECIMAL) et/ou un type
d'enrichissement typographique (sont admis les types BOLD, ITALIC,
UNDERLINE et, NORMAL -par d괽aut-). La justification et l'enrichissement
typographique d괽inis au niveu du diviseur de la colonne sont appliqu굎
par d괽aut � toutes les cellules de la colonne.


&IT Le "contenu" du tableau est construit de la fa뇇n suivante
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�

&EN chaque ligne d괷ute par le caract둹e '-'

&EN les lignes de titre sont d굎ign괻s par le mot cl� TITLE suivi entre
    guillemets de la cha똭e de caract둹e du titre. La chaine de
    caract둹es peut 늯re suivie d'un mot cl� indiquant la justification
    du titre � l'impression (les mots cl굎 admis sont LEFT -option par
    d괽aut-, RIGHT, CENTER et DECIMAL) et/ou un type d'enrichissement
    typographique (sont admis les types BOLD, ITALIC et, NORMAL -par
    d괽aut-). Le titre sera imprim� � travers toutes les cellules du
    tableau avec la justification et les enrichissement typographiques
    sp괹ifi굎.

&EN les lignes s굋aratrices sont d굎ign괻s par le mot cl� LINE sans
    param둻re. Une ligne transversale sera imprim괻 dans le tableau cet
    endroit.

&EN les lignes ordinaires sont constitu괻s de cellules, autant par ligne
    que la dimension du tableau. Chaque cellule est soit une cha똭e de
    caract둹es d굃imit괻 par des guillemets (cette cha똭e sera imprim괻
    telle quelle � la bonne place dans le tableau), soit une forme LEC
    repr굎ent괻 entre guillemets, pr괹괺괻 du mot cle LEC (cette formule
    sera calcul괻 pour chaque p굍iode du sample d'impression et le
    r굎ultat sera imprim�). Chaque cellule peut 늯re suivie d'un ou
    plusieurs mot cl� indiquant sa justification dans la colonne du
    tableau � l'impression (les mots cl굎 admis sont LEFT -option par
    d괽aut-, RIGHT, CENTER et DECIMAL) et/ou un type d'enrichissement
    typographique (sont admis les types BOLD, ITALIC, UNDERLINE et,
    NORMAL -par d괽aut-)

&EN une cellule consitu괻 d'une cha똭e de caract둹es peut contenir une
    des macros suivantes: ##s sera remplac� � l'impression par le sample
    courrant complet, ##t sera remplac� � l'impression par la p굍iode
    courrante, et ##f sera remplac� � l'impression par le nom du fichier
    trait�.

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

&IT R굎um� des mots cl굎
컴컴컴컴컴컴컴컴컴컴�

&EN { et } : d굃imiteurs de bloc

&EN " : d굃imiteur de cha똭e de caract둹e (ou forme LEC)

&EN /* et */ : d굃imiteurs de commentaires

&EN DIM nb : dimension du tableau, nb := nombre entier positif

&EN DIV cellules : ligne de diviseurs, cellules := formes LEC

&EN LEFT, CENTER, RIGHT ou DECIMAL : justification de la cellule

&EN NORMAL, ITALIC, UNDERLINE ou BOLD : enrichissement typographique de
la cellule

&EN TITLE "cha똭e" : ligne de titre, cha똭e := le titre � imprimer

&EN LINE : ligne s굋aratrice

&EN "cha똭e" : cha똭e := contenu non num굍ique d'une cellule ordinaire

&EN LEC "formule" : cellule � contenu num굍ique calcul� par la formule
LEC

&EN FILES : r괽굍ence du/des fichier(s)

&EN MODE : signification des sigles imprim굎 par la macro ##s

&EN ##s, ##t et ##f : macros d괽inissant l'impression du sample, de la
    p굍iode et du ou des fichier(s).


>

<format ASCII des variables   >
    Format ASCII des variables
    컴컴컴컴컴컴컴컴컴컴컴컴컴

&IT D괽inition
컴컴컴컴컴�

Le format ASCII d'un fichier de variables se compose de la fa뇇n
suivante:

&EN sur la premi둹e ligne: le mot cl� ~isample~I suivi ~ides p굍iodes
    limites d'괹hantillonnage~I

&EN successivement sur les lignes suivantes et � raison d'une par ligne,
    les variables en commencant par leur ~inom~I (un nom valide au sens
    IODE, c-�-d un nom de 10 caract둹es maximum (20 � partir de la version
    6.01) -lettre ou chiffre- commencant toujours par une lettre -ou le
    caract둹e '_'- ,en majuscule) suivi des ~ivaleurs (ou na)~I pour chaque
    p굍iode. Chaque 굃굆ent (nom et valeurs) sont s굋ar굎 par un blanc.

&EN la fin de fichier signale la fin de d괽inition des variables.

&IT mot(s) cl굎
컴컴컴컴컴컴

Un seul mot cl� est utilis�:

&CO
    sample yyyyPppp  yyyyPppp
		   1         2
&TX

La d괽inition des p굍iodes limites doit r굋ondre � la syntaxe suivante:

&CO
    yyyyPppp    yyyy est l'ann괻
		P est la p굍iodicit�
		    P := Y (yearly), Q (quaterly), M (monthly), W (weekly)
		ppp est la p굍iode dans l'ann괻

&EX2

sample 1970Y1 1996Y1
A0G 621800 644700 805700 ... 874900 939000 1.0538e+06 1.1965e+06
A0GBNB 49500 40800 39600 ... 42900 46100 48400 65500 88900 108600
 ...
A0GCCP 57300 57200 66000 ... 63400 65700 69400 76800 79900 83700 80900
A0GFMI 12400 2400 6100 7300 ... 6600 4700 1500 4100 19500 19800 37000
&TX
>






