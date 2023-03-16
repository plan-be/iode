/*SAMPLE*/

<Sample d'impression>

    Définition du sample d'impression
    —————————————————————————————————

La définition du sample d'impression concerne la compilation et
l'impression ou la visualisation des tables et des graphiques.

Les valeurs à imprimer ou à visualiser figurent dans la définition des
tables et des graphiques sous forme d'une formule LEC. Cette forme LEC
ne donne aucune indication sur l'échantillonnage (période(s)
d'impression) ni sur la représentation (en valeur relative, en taux de
croissance,...) de la valeur à imprimer. Ces dernières informations sont
apportées par le sample d'impression.

Le sample d'impression contient des informations sur:

&EN l'échantillonnage des périodes relatives aux variables ou aux valeurs
    calculées à imprimer

&EN les opérations à effectuer sur les variables à imprimer

&EN les fichiers où se trouvent les variables à imprimer ou intervenant
    dans le calcul des valeurs à imprimer.

&TI Syntaxe d'un sample d'impression
————————————————————————————————————

La syntaxe d'un sample répond aux règles décrites ci-dessous. Un sample est
composé de périodes, d'opérations sur des périodes, d'opérations sur des
fichiers, de facteur(s) de répétition.

&IT Syntaxe d'une période
—————————————————————————

&EN une période s'indique comme en LEC : ~cyyPpp~C ou ~cyyyyPpp~C où ~cyyyy~C
    indique l'année, ~cP~C la périodicité et ~cpp~C la sous-période ~c(1990Y1)~C

&EN une période peut être décalée de n périodes vers la gauche ou la droite à l'aide des
    opérateurs ~c<<n~C et ~c>>n~C

&EN When used with a null argument, the shift oprerators have a special meaning :
&EN2 ~c<<0~C means "first period of the year"
&EN2 ~c>>0~C means "last period of the year"

&EN les périodes spéciales ~cBOS,~C ~cEOS~C et ~cNOW~C peuvent être
utilisées pour représenter le début, la fin du sample courant ou la période
actuelle (horloge du PC).

&EN les périodes spéciales ~cBOS1,~C ~cEOS1~C et ~cNOW1~C sont équivalentes aux précédentes à ceci près qu'elles
    sont déplacées à la première sous période de l'année de ~cBOS,~C ~cEOS~C et ~cNOW~C
    respectivement (si ~cNOW~C = ~c2012M5~C, ~cNOW1~C = ~c2012M1~C).

&EN Chaque période est séparée de la suivante par un point-virgule.

&EN Une période ou un groupe des périodes peuvent être répétées : il
    suffit de placer après la définition de la colonne ou du groupe de
    colonnes le caractère double point (:) suivi du nombre de
    répétitions souhaité. La répétition se fait avec un incrément de une
    période, sauf si elle est suivie d'une astérisque et d'une valeur.
    Cette valeur est alors l'incrément de répétition. Elle peut être négative auquel
    cas les périodes se présenteront de manière décroissante.

&EN La répétition, l'incrément et le shift peuvent être les mots ~cPER~C (ou
    ~cP~C) ou ~cSUB~C (ou ~cS~C) qui indiquent respectivement le nombre de
    périodes dans une année du sample courant et la sous période courante.

&EN La définition des fichiers est optionnelle et est placée entre
    crochets. Elle s'applique à toute la définition de période qui précède.

&IT Opérations sur les fichiers
———————————————————————————————

Les opérations possibles sur les fichiers sont :

&EN valeur absolue                        ~c[1]~C
&EN différence                            ~c[1-2]~C
&EN différence en pourcents               ~c[1/2]~C
&EN somme                                 ~c[1+2]~C
&EN moyenne                               ~c[1~~2]~C ou ~c[1^2]~C

&IT Opérations sur les périodes
———————————————————————————————
Les opérations sur les périodes sont :

&EN valeur (75)
&EN taux de croissance sur une ou plusieurs périodes (75/74, 75/70)
&EN taux de croissance moyen (75//70)
&EN différence (75-74, 75-70)
&EN différence moyenne (75--70)
&EN moyenne (75~~74) ou (75^74)
&EN somme de période à période consécutives (70Q1+70Q4)
&EN valeur en indice ou en base (76=70)

La répétition peut s'effectuer avec un incrément supérieur à 1 ou inférieur à 0 : il
suffit de placer une étoile suivie du pas après le nombre de répétitions
(70:3*5 = 70, 75, 80).

&EX
	    70; 75; 80:6
	    = 70:3*5; 81:5
	    = 70; 75; 80; 81; 82; 83; 84; 85

	    70/69:2
	    = 70/69; 71/70

	    (70; 70-69):2
	    = 70; 70-69; 71; 71-70;

	    70[1,2]:2*5
	    = 70[1]; 70[2]; 75[1]; 75[2]

	    (70;75)[1,2-1]
	    = 70[1];75[1];70[2-1];75[2-1]

	    (70;75;(80; 80/79):2)[1,2]
	    = 70[1]; 70[2]; 75[1]; 75[2]; 80[1];
		80[2]; 80/79[1]; 80/79[2] 81[1]; 81[2];
		    81/80[1]; 81/80[2]


	    2000Y1>>5
	    = 2005Y1

	    1999M1>>12
	    = 2000M1

	    EOS<<1
	    = 2019Y1                    (si EOS == 2020Y1)

	    BOS<<1
	    = 1959Y1                    (si BOS == 1960Y1)

	    EOS<<4:5*-1
	    =2016;2017;2018;2019;2020   (si EOS = 2020Y1)
&TX

In may 2012, assuming that ~c1990M6:2020M12~C is the current sample,
the following samples are equivalent :

&CO
    BOS:2;EOS   ==   1990M6;1990M7;2020M12
    EOS;EOS1    ==   2020M12:2020M1
    NOW;NOW1    ==   2012M5;2012M1
    NOW:P       ==   2012M5;2012M6;...;2013M4
    NOW:3*P     ==   2012M5;2013M5;2014M5
    NOW1:SUB    ==   2012M5

    2000Y1>>5      === 2005Y1
    1999M1>>12     === 2000M1
    EOS<<1         === 2020M11
    EOS<<P         === 2019M12
&TX

If the sample is 1960Y1:2020Y1,
&CO
    EOS:5*-1       === 2020;2019,2018,2017,2016
    BOS<<1         === 1959Y1

&TX

In may 2012, if the current sample is 1990Q1:2012Q4,
the following samples are equivalent :

&CO
    NOW;NOW1<<4    === 2012Q2;2011Q1 (for quaterly data)

    NOW<<0;NOW>>0    === 2012Q1;2012Q4
    NOW<<0:P        === 2012Q1;2012Q2;2012Q3;2012Q4
    NOW:3*P        === 2012Q2;2013Q2;2014Q2

    NOW;(NOW<<0>>1/NOW<<0)>>P   === 2012Q2;2013Q2/2013Q1


&TX

>















