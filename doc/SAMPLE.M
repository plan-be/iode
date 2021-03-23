/*SAMPLE*/

<Sample d'impression>

    D‚finition du sample d'impression
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

La d‚finition du sample d'impression concerne la compilation et
l'impression ou la visualisation des tables et des graphiques.

Les valeurs … imprimer ou … visualiser figurent dans la d‚finition des
tables et des graphiques sous forme d'une formule LEC. Cette forme LEC
ne donne aucune indication sur l'‚chantillonnage (p‚riode(s)
d'impression) ni sur la repr‚sentation (en valeur relative, en taux de
croissance,...) de la valeur … imprimer. Ces derniŠres informations sont
apport‚es par le sample d'impression.

Le sample d'impression contient des informations sur:

&EN l'‚chantillonnage des p‚riodes relatives aux variables ou aux valeurs
    calcul‚es … imprimer

&EN les op‚rations … effectuer sur les variables … imprimer

&EN les fichiers o— se trouvent les variables … imprimer ou intervenant
    dans le calcul des valeurs … imprimer.

&TI Syntaxe d'un sample d'impression
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

La syntaxe d'un sample r‚pond aux rŠgles d‚crites ci-dessous. Un sample est
compos‚ de p‚riodes, d'op‚rations sur des p‚riodes, d'op‚rations sur des
fichiers, de facteur(s) de r‚p‚tition.

&IT Syntaxe d'une p‚riode
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&EN une p‚riode s'indique comme en LEC : ~cyyPpp~C ou ~cyyyyPpp~C o— ~cyyyy~C
    indique l'ann‚e, ~cP~C la p‚riodicit‚ et ~cpp~C la sous-p‚riode ~c(1990Y1)~C

&EN une p‚riode peut ˆtre d‚cal‚e de n p‚riodes vers la gauche ou la droite … l'aide des
    op‚rateurs ~c<<n~C et ~c>>n~C

&EN When used with a null argument, the shift oprerators have a special meaning :
&EN2 ~c<<0~C means "first period of the year"
&EN2 ~c>>0~C means "last period of the year"

&EN les p‚riodes sp‚ciales ~cBOS,~C ~cEOS~C et ~cNOW~C peuvent ˆtre
utilis‚es pour repr‚senter le d‚but, la fin du sample courant ou la p‚riode
actuelle (horloge du PC).

&EN les p‚riodes sp‚ciales ~cBOS1,~C ~cEOS1~C et ~cNOW1~C sont ‚quivalentes aux pr‚c‚dentes … ceci prŠs qu'elles
    sont d‚plac‚es … la premiŠre sous p‚riode de l'ann‚e de ~cBOS,~C ~cEOS~C et ~cNOW~C
    respectivement (si ~cNOW~C = ~c2012M5~C, ~cNOW1~C = ~c2012M1~C).

&EN Chaque p‚riode est s‚par‚e de la suivante par un point-virgule.

&EN Une p‚riode ou un groupe des p‚riodes peuvent ˆtre r‚p‚t‚es : il
    suffit de placer aprŠs la d‚finition de la colonne ou du groupe de
    colonnes le caractŠre double point (:) suivi du nombre de
    r‚p‚titions souhait‚. La r‚p‚tition se fait avec un incr‚ment de une
    p‚riode, sauf si elle est suivie d'une ast‚risque et d'une valeur.
    Cette valeur est alors l'incr‚ment de r‚p‚tition. Elle peut ˆtre n‚gative auquel
    cas les p‚riodes se pr‚senteront de maniŠre d‚croissante.

&EN La r‚p‚tition, l'incr‚ment et le shift peuvent ˆtre les mots ~cPER~C (ou
    ~cP~C) ou ~cSUB~C (ou ~cS~C) qui indiquent respectivement le nombre de
    p‚riodes dans une ann‚e du sample courant et la sous p‚riode courante.

&EN La d‚finition des fichiers est optionnelle et est plac‚e entre
    crochets. Elle s'applique … toute la d‚finition de p‚riode qui pr‚cŠde.

&IT Op‚rations sur les fichiers
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les op‚rations possibles sur les fichiers sont :

&EN valeur absolue                        ~c[1]~C
&EN diff‚rence                            ~c[1-2]~C
&EN diff‚rence en pourcents               ~c[1/2]~C
&EN somme                                 ~c[1+2]~C
&EN moyenne                               ~c[1~~2]~C ou ~c[1^2]~C

&IT Op‚rations sur les p‚riodes
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Les op‚rations sur les p‚riodes sont :

&EN valeur (75)
&EN taux de croissance sur une ou plusieurs p‚riodes (75/74, 75/70)
&EN taux de croissance moyen (75//70)
&EN diff‚rence (75-74, 75-70)
&EN diff‚rence moyenne (75--70)
&EN moyenne (75~~74) ou (75^74)
&EN somme de p‚riode … p‚riode cons‚cutives (70Q1+70Q4)
&EN valeur en indice ou en base (76=70)

La r‚p‚tition peut s'effectuer avec un incr‚ment sup‚rieur … 1 ou inf‚rieur … 0 : il
suffit de placer une ‚toile suivie du pas aprŠs le nombre de r‚p‚titions
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















