

<Le langage LEC>

    Le langage LEC
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Qu'il s'agisse d'‚quations, d'identit‚s de construction ou encore de
graphiques, un langage d'‚criture de formules math‚matiques ad‚quat est
indispensable. Le langage LEC (Langage Econom‚trique Condens‚) offre
l'avantage d'ˆtre … la fois concis dans son ‚criture et particuliŠrement
adapt‚ aux formules faisant intervenir des s‚ries chronologiques. Il est
utilis‚ chaque fois qu'une formule est n‚cessaire dans le logiciel IODE.

Le LEC est ‚galement "naturel", en ce sens que sa syntaxe est proche de
l'‚criture de formules que l'on peut trouver dans la litt‚rature.

&CO
    Equation de consommation
    ------------------------
		   Yt
	Ct = a + b -- + c.C                             (texte)
		   Pt      t-1

	C := a + b * Y / P + c * C[-1]                  (LEC)


    Equation de production
    ----------------------
	ln Qt = a ln Kt + (1 - a) ln Lt + c.t + b       (texte)

	ln Q := a * ln K + (1 - a) ln L + c * t + b     (LEC)
&TX


On peut classer les ‚l‚ments constitutifs du langage en plusieurs classes :


&EN <les constantes du LEC            >
&EN <les variables                    >
&EN <les scalaires                    >
&EN <les op‚rateurs logiques          >
&EN <les op‚rateurs alg‚briques       >
&EN <les fonctions math‚matiques      >
&EN <les fonctions temporelles        >
&EN <les listes ou macros             >
&EN <lags, leads et valeurs de p‚riode>
&EN <les commentaires                 >

En fin de chapitre, les points suivants sont d‚taill‚s :

&EN <priorit‚ des op‚rateurs    >
&EN <‚criture des ‚quations     >
&EN <r‚capitulatif de la syntaxe>
>
<les constantes du LEC    >
    Les constantes du LEC
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&TI    Les constantes num‚riques
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Celles-ci sont de simples valeurs qui peuvent ˆtre entiŠres ou r‚elles :

Constantes entiŠres :
&CO
    2, 51234, 12345634
&TX

Constantes r‚elles en format flottant :
&CO
    2.234, 0.000123
&TX

Constantes r‚elles en format exponentiel :
&CO
    2.234e-3, 1E5, 1.2E-03
&TX


&TI    Les constantes temporelles
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&IT Variable t
ÄÄÄÄÄÄÄÄÄÄ
Parmi les objets de IODE, les variables, qui sont des s‚ries
chronologiques sont d‚finies sur une p‚riode limit‚e de temps : le
SAMPLE du workspace de variables d‚termine cette p‚riode. Par exemple,
1970Y1 … 2000Y1 signifie que les s‚ries sont annuelles (Y) et d‚finies
de 1970 … 2000.


On peut utiliser dans le langage LEC des constantes temporelles de format
semblable. Elles sont utilis‚es pour prendre la valeur d'une s‚rie … une
p‚riode fix‚e par exemple.


~uExemples~U :
&CO
    1990Y1
    70Q4
    2010M11
&TX


Elles peuvent ‚galement ˆtre utiles dans des
op‚rations du type:

&CO
	X * (t << 1990Y1) + Y * (t >>= 1990Y1)
&TX

Cette derniŠre formule vaut X si la p‚riode en cours d'ex‚cution est
inf‚rieure … 1990 et Y sinon. On verra plus loin que t repr‚sente l'index
de la p‚riode en cours d'ex‚cution.

Les constantes temporelles prennent une valeur entiŠre qui est celle
de l'index dans la s‚rie temporelle de la p‚riode en cours d'ex‚cution.
Supposons que le WS de variables soit d‚fini sur un SAMPLE allant de 1970Y1
… 1995Y1. L'‚quivalent num‚rique de la constante temporelle 1970Y1 est 0.
De mˆme 1980Y1 vaut 10, etc.

Pourquoi ne pas simplement indiquer 0 ou 10 au lieu de 1970Y1 ou 1990Y1 ?
Tout simplement parce que la mˆme formule peut ˆtre ex‚cut‚e sur des WS de
SAMPLE diff‚rents, et rester correcte. Si le SAMPLE s'‚tend de
1975Y1 … 1995Y1, la valeur de 1980Y1 ne vaut plus 10 comme dans le premier
cas, mais 5. La formule n'a pas ‚t‚ chang‚e, mais son ex‚cution s'est
adapt‚e au contexte!


La constante ~ct~C indique donc l'index de la p‚riode en cours d'ex‚cution.
Elle a une valeur entiŠre.

Penons un autre exemple. Supposons que le WS de variables courant soit
d‚fini sur le SAMPLE ~c1990Q1~C - ~c1999Q4~C et qu'une simulation d'un modŠle soit
en cours sur la p‚riode ~c1992Q1~C … ~c1994Q4.~C La valeur de ~ct~C changera … chaque
p‚riode simul‚e : en ~c1992Q1,~C ~ct~C vaut 8, en ~c1992Q2,~C ~ct~C vaut 9, et ainsi de
suite.

Dans la formule
&CO
    if(t << 1993Q1, 0, 12.3),
&TX

~c1993Q1~C vaut 12 et t vaut 8, 9, 10, 11, ...19, selon la p‚riode en cours de
simulation. On constate que la valeur de la formule reste 0 tant que t est
plus petit que 12, c'est-…-dire jusqu'en 1992Q4, et 12.3 ensuite.

Le mˆme principe peut ˆtre appliqu‚ au calcul d'identit‚s qui elles aussi
sont d‚finies sur un SAMPLE et donc qui font ‚voluer la valeur de t.


&IT Variable i
ÄÄÄÄÄÄÄÄÄÄÄ
Cette variable du langage LEC permet de connaŒtre lors d'un calcul la
diff‚rence entre l'ann‚e de calcul d'une sous-expression par rapport …
l'ann‚e de calcul de toute la formule. Cette variable s'appelle i et
son comportement s'apparente … celui de t. Elle est donc invariante par
rapport … l'ann‚e de calcul d'une formule LEC.

On calcule toujours une forme LEC pour une valeur du temps t donn‚e,
mais dans une sous-expression, t peut ˆtre diff‚rent. C'est par exemple
le cas pour les op‚rateurs d(), dln(), sum(), ... On peut, grace … i,
connaŒtre la diff‚rence entre le t de calcul de la formule et le t de
calcul de la sous-expression courante.

On peut donc calculer des expressions comme
&CO
    sum(t-2, t-4, A / (1 - i)**2),
&TX
    qui ‚quivaut … :

&CO
    A[-2]/(1-(-2))**2 + A[-3]/(1-(-3))**2 + A[-4]/(1-(-4))**2
&TX

Sans i, une telle expression ne peut ˆtre ‚crite en LEC.

Cet op‚rateur a ‚t‚ impl‚ment‚ pour permettre la traduction de
l'op‚rateur SUM de TROLL.

Ainsi, si on calcule une expression pour un t donn‚, i prend les
valeurs suivantes selon les cas. Attention, les expressions indiqu‚es
ci-dessous sont les formules complŠtes, et pas une sous-expression!

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
&TI Les constantes du langage
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Trois constantes math‚matiques sont d‚finies dans le langage :

&EN pi qui vaut 3.141592653589
&EN e qui vaut 2.7182818284
&EN euro vaut 40.3399

>
<Les variables>
    Les variables
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les variables repr‚sentent des s‚ries num‚riques temporelles. Leur
nom est toujours ‚crit en caractŠres majuscules pour les distinguer des
scalaires (voir ci-dessous).

Les noms des variables peuvent contenir jusqu'… 10 caractŠres (20 … partir de la version 6.01). Ces
caractŠres doivent ˆtre alphab‚tiques (A-Z) ou num‚riques (0-9), ou encore
le caractŠre de soulignement. Chaque nom de variable doit commencer par un
caractŠre alphab‚tique MAJUSCULE.

&EX
    A, B_PNB, A123456789
&TX

sont des noms corrects de variables.

&EX
    _A1, 1A34, A_123456789, z_AV, A-2, B.X
&TX

sont des noms incorrects (z_AV est un nom de scalaire)

Le fait d'‚crire une variable dans une formule signifie qu'il faut en
prendre la valeur … la p‚riode de temps en cours d'ex‚cution (que ce soit
dans le cadre d'un modŠle, d'un jeu d'identit‚s ou d'un tableau).

Pour en prendre la valeur … la p‚riode pr‚c‚dente (lag), on ‚crira :

&CO
    PNB[-1]
&TX

La valeur … la p‚riode suivante (lead) s'‚crit :

&CO
    PNB[+1]
&TX

La valeur … la p‚riode 1990Y1 (d‚g‚n‚rescence) s'‚crit :

&CO
    PNB[1990Y1]
&TX

Cette syntaxe est plus g‚n‚rale et peut s'appliquer … des formules
entiŠres.
>
<Les scalaires>
    Les scalaires
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les scalaires sont des variables sans dimension, donc d‚finies par une
seule valeur. Ce sont par exemple les coefficients estim‚s dans une
‚quation ou des valeurs constantes … travers le temps.

Les noms des scalaires peuvent contenir jusqu'… 10 caractŠres (20 … partir de la version 6.01). Ces
caractŠres doivent ˆtre alphab‚tiques (A-Z) ou num‚riques (0-9), ou encore
le caractŠre de soulignement. Chaque nom doit commencer par un caractŠre
alphab‚tique minuscule.

&EX
    a, c1, a_123456789
&TX

sont des noms corrects de scalaires.

&EX
    _a1, 1A34, a_123456789, Z_av
&TX

sont des noms incorrects (Z_av est un nom de variable)

Les op‚rations de lag, lead et d‚g‚n‚rescence appliqu‚es … un scalaire sont
sans effet, mais permises.
>
<Les op‚rateurs logiques>
    Les op‚rateurs logiques
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Pour indiquer les op‚rations logiques … effectuer sur les valeurs d‚finies
par les constantes, variables ou scalaires, on dispose des op‚rateurs
suivants, pr‚sent‚s dans leur ordre de priorit‚ :

&EN not ou ! : n‚gation d'une expression
&EN or       : ou logique entre deux expressions
&EN and      : et logique entre deux expressions
&EN <<        : inf‚rieur
&EN <<=       : inf‚rieur ou ‚gal
&EN =        : ‚galit‚
&EN != ou <<>> : diff‚rent
&EN >>=       : inf‚rieur ou ‚gal
&EN >>        : inf‚rieur

Les op‚rateurs logiques ont une valeur nulle lorsque la condition exprim‚e
est fausse et 1 si les valeurs satisfont … la condition.

Notons qu'une fonction if() est ‚galement d‚finie et se trouve dans les
fonctions d‚crites plus bas dans le texte.

&EX
    Soient X = 1
	   Y = 0
	   Z = 2

    A la p‚riode courante

    Les formules                  Valent
    ------------------------------------
    !X                              0
    X and !Y                        1
    X or !Y                         1
    !(X + Y)                        0
    !(2.32 + X)                     0
    X == 0 and Y == 0 or Z == 2     1
    Z << 1 * 3                       1
&TX
>

<Les op‚rateurs alg‚briques>
    Les op‚rateurs alg‚briques
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les calculs alg‚briques sont effectu‚s … l'aide des op‚rateurs suivants :

&EN + : addition
&EN - : soustraction
&EN / : division
&EN * : multiplication
&EN ** : exponentiation

Ceux-ci ont les significations et les priorit‚s habituelles.
>

<Les fonctions math‚matiques>
    Les fonctions math‚matiques
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

De nombreuses fonctions math‚matiques sont impl‚ment‚es en LEC. Certaines
fonctions prennent un argument, d'autres plusieurs. Parfois, une fonction
accepte un nombre variable d'argument, les arguments absents pouvant prendre
une valeur par d‚faut.

De maniŠre g‚n‚rale, une fonction qui n'a qu'un argument ne demande pas les
parenthŠses. Ainsi

&CO
    ln X + 2        est identique …     ln(X) + 2
    d X             est identique …     d(X)
&TX

Toutes les fonctions ont un nom entiŠrement ‚crit en minuscules. Des
coefficients ne peuvent donc pas prendre un des noms r‚serv‚s aux fonctions.

Dans la syntaxe qui suit, les notations suivantes sont utilis‚es :

&EN expr : n'importe quelle expression LEC (formule)
&EN crochets [] : partie optionnelle
&EN points de suspension  : r‚p‚tition du dernier argument autant de fois que
    souhait‚

Les fonctions math‚matiques sont :

&EN -expr                : moins unaire
&EN +expr                : plus unaire
&EN log([expr], expr)    : logarithme en base quelconque
&EN ln(expr)             : logarithme n‚p‚rien
&EN exp([expr,] expr)    : exponentielle en base quelconque
&EN max(expr, ...)       : maximum d'une liste de valeurs
&EN min(expr, ...)       : minimum d'une liste de valeurs
&EN lsum(expr, expr, ...): somme de formules
&EN sin(expr)            : sinus
&EN cos(expr)            : cosinus
&EN acos(expr)           : arccosinus
&EN asin(expr)           : arcsinus
&EN tan(expr)            : tangente
&EN atan(expr)           : arctangente
&EN tanh(expr)           : tangente hyperbolique
&EN sinh(expr)           : sinus hyperbolique
&EN cosh(expr)           : cosinus hyperbolique
&EN abs(expr)            : valeur absolue
&EN sqrt(expr)           : racine carr‚e
&EN int(expr)            : partie entiŠre
&EN rad(expr)            : transforme des degr‚s en radians
&EN if(expr, expr, expr) : condition
&EN sign(expr)           : signe d'une expression
&EN isan(expr)           : retourne 0 si expr est NAN et 1 sinon
&EN lmean(expr, ...)     : retourne la moyenne des expressions
&EN lprod(expr, ...)     : retourne le produit des expressions
&EN lcount(expr, ...)    : retourne le nombre d'expressions
&EN floor(expr) : partie entiŠre de l'expression
&EN ceil(expr) : partie entiŠre de l'expression plus 1
&EN round(expr [, n])  : arrondi de expr … la nŠme d‚cimale.
			Si n n'est pas d‚fini, il est fix‚ … 0.
&EN random(expr)         : fournit un nombre al‚atoire compris entre -expr/2 et +expr/2.

Quelques op‚rateurs demandent un compl‚ment d'explication :

&IT Plus unaire
ÄÄÄÄÄÄÄÄÄÄÄ

Le plus unaire est sans effet : ++++++X vaut simplement X.

&IT Logarithme
ÄÄÄÄÄÄÄÄÄÄ
ln(X) est le logarithme n‚p‚rien de X. log(X) vaut ln(X) tandis que log(10,
X) est le logarithme en base 10 de X.

&IT Exponentielle
ÄÄÄÄÄÄÄÄÄÄÄÄÄ

exp(X) est ‚quivalent … e**X et exp(10, X) vaut 10**X.

&IT Max, min et lsum
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
max(1, X, Y, Z + 2) prend la valeur maximale de toutes les valeurs pass‚es
comme argument (… la p‚riode courante). La fonction min() en prend le
minimum. La fonction lsum() effectue la somme de toutes les valeurs
pass‚es comme argument.

Dans les trois cas, le nombre d'argument soit ˆtre compris entre 2 et 50.

&IT Partie entiŠre
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
..... int(X)  prend comme valeur la partie entiŠre de X : int(2.23e1) vaut 22.
~cint(X)~C retourne l'entier le plus proche de X :

&CO
 int(2.2) = 2
 int(2.6) = 3
 int(2.5) = 3
&TX

&IT Fonction if
ÄÄÄÄÄÄÄÄÄÄÄ
La fonction if permet de simplifier l'‚criture de conditions : le premier
argument est la condition, le second la valeur si la condition est
v‚rifi‚e, le troisiŠme la valeur dans le cas contraire. Il faut signaler
que les trois arguments sont calcul‚s et qu'en cas d'erreur dans
l'ex‚cution de l'un d'eux, la formule retourne la valeur NA (not available)
mˆme si c'est l'expression inutile qui a g‚n‚r‚ l'erreur.

&EX
    if(t << 1992Y1, 2, X)   vaut 2 avant 1992Y1
				 X … partir de 1992Y1
    if(t << 1992Y1, 2, X/0) vaut NA … partir de 1992Y1
				 2 avant 1992Y1.
&TX
&IT Fonction sign
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
La fonction sign retourne le signe d'une expression :
&CO
    sign(expr) vaut
		    1 si expr >>= 0
		    -1 si expr << 0
&TX


&IT Fonction random
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
La fonction ~crandom~C fournit un nombre pseudo-al‚atoire compris entre
~c-expr/2~C et ~c+expr/2.~C Sa syntaxe est:

&CO
    random(expr)
&TX
>

<Les fonctions temporelles>
    Les fonctions temporelles
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Un langage ‚conom‚trique ne m‚riterait pas son nom sans fonctions
temporelles. De nombreuses ‚quations et formules demandent des calculs du
type

&CO
    c1 * (PNB - PNB[-1]) + c2 * (VXA - VXA[-1])
&TX

Les op‚rateurs temporels permettent de simplifier cette ‚criture en la
rempla‡ant par :

&CO
    c1 * d PNB + c2 * d VXA
&TX

Dans la syntaxe qui suit, les notations suivantes sont utilis‚es :

&EN expr : n'importe quelle expression LEC (formule)
&EN []   : partie optionnelle

Le LEC possŠde des op‚rateurs temporels pour calculer les diff‚rences de
p‚riodes, les taux de croissance, les moyennes, les ‚carts-types, etc. En
voici la liste :

&EN l([expr,] expr)             : lag d'une expression
&EN d([expr,] expr)             : diff‚rence de p‚riodes
&EN r([expr,] expr)             : rapport de p‚riodes
&EN dln([expr,] expr)           : diff‚rence des logarithmes des p‚riodes
&EN grt([expr,] expr)           : taux de croissance
&EN ma([expr,] expr)            : moyenne mobile (Moving Average)
&EN mavg([expr,] expr)          : moyenne mobile (identique … ma)
&EN vmax([expr,[expr,]] expr)   : maximum sur une p‚riode
&EN vmin([expr,[expr,]] expr)   : minimum sur une p‚riode
&EN sum([expr,[expr,]] expr)    : somme sur une p‚riode
&EN prod([expr,[expr,]] expr)   : produit sur une p‚riode
&EN mean([expr,[expr,]] expr)   : moyenne de p‚riode
&EN index([expr,[expr,]] expr1, expr2) : index d'une valeur
&EN acf([expr,[expr,]] expr, expr) : facteur d'autocorr‚lation
&EN var([from [,to],] expr) : variance
&EN covar([from [,to],] expr1, expr2) : covariance
&EN covar0([from [,to],] expr1, expr2) : covariance autour de l'origine
&EN corr([from [,to],] x, y) : corr‚lation
&EN stderr([expr,[expr,]] expr) : d‚viation standard (non biais‚)
&EN stddev([from [,to],] expr) : d‚viation standard (biais‚)
&EN lastobs([from [,to],] expr) : calcule la derniŠre observation sur une p‚riode donn‚e
&EN interpol(expr) : fournit une valeur … expr en t en interpolant ou extrapolant
&EN app(expr1, expr2) : fournit une valeur … expr1 en t en utilisant la
    s‚rie expr2 comme s‚rie apparent‚e par une m‚thode g‚om‚trique
&EN dapp(expr1, expr2) : fournit par diff‚rences une valeur … expr1 en t en
    utilisant la s‚rie expr2 comme s‚rie apparent‚e
&EN hp([ [from,]to],expr)        : filtre Hodrick-Prescott avec passage au ~clog~C de ~cexpr~C
&EN hpstd([ [from,]to],expr)     : calcul sans passage au ~clog~C de ~cexpr~C
&EN appdif(expr1, expr2) : alias de dapp()



On trouvera ci-dessous la signification pr‚cise de tous les op‚rateurs
temporels.

&IT Lag d'une expression
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
L'expression l(expr1, expr2) est ‚quivalente … expr2[expr1]. Cette
formulation permet de calculer ou de param‚trer des lags. Par exemple,
l'expression

&CO
    X[c1 + 2]
&TX
est syntaxiquement fausse, tandis que
&CO
    l(c1 + 2, X)
&TX
est parfaitement correcte et ‚quivalente.

Si expr1 n'est pas d‚finie, elle est remplac‚e par 1. Ainsi

&CO
    l(X) est identique … l(1, X)
&TX

&IT Diff‚rence de p‚riodes
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

L'expression d(expr1, expr2) est ‚quivalente … expr2 - expr2[expr1],
o— le r‚sultat du calcul de expr1 est consid‚r‚ comme un lag s'il est
n‚gatif et un lead s'il est positif. Par exemple :

&CO
    d(2, X + Y)  est identique … (X + Y) - (X + Y)[-2]
&TX

expr1 peut ˆtre une expression quelconque. Si expr1 n'est pas d‚finie, elle
est remplac‚e par 1. Ainsi

&CO
    d(X) est identique … d(1, X)
&TX

&IT Rapport de p‚riodes
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
L'expression r(expr1, expr2) est ‚quivalente … expr2 / expr2[expr1],
o— le r‚sultat du calcul de expr1 est consid‚r‚ comme un lag s'il est
n‚gatif et un lead s'il est positif. Par exemple :

&CO
    r(-2, X + Y)  est identique … (X + Y) / (X + Y)[+2]
&TX

expr1 peut ˆtre une expression quelconque. Si expr1 n'est pas d‚finie, elle
est remplac‚e par 1. Ainsi

&CO
    r(X) est identique … r(1, X)
&TX

&IT Diff‚rence des logarithmes des p‚riodes
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
L'expression dln(expr1, expr2) ‚quivaut … ln(expr2) - ln(expr2)[expr1],
o— le r‚sultat du calcul de expr1 est consid‚r‚ comme un lag s'il est
n‚gatif et un lead s'il est positif. Par exemple :

&CO
    dln(3 - 2, X + Y) vaut ln(X + Y) - ln(X + Y)[-1]
&TX

expr1 peut ˆtre une expression quelconque. Si expr1 n'est pas d‚finie, elle
est remplac‚e par 1. Ainsi

&CO
    dln(X) est identique … dln(1, X)
&TX

&IT Taux de croissance
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
L'expression grt(expr1, expr2) ‚quivaut … :

&CO
    100 * (expr2 / expr2[expr1] - 1)
&TX
o— le r‚sultat du calcul de expr1 est consid‚r‚ comme un lag s'il est
n‚gatif et un lead s'il est positif. Par exemple :

&CO
    grt(2, X + Y) vaut 100 * ((X + Y) / (X + Y)[-2] - 1)
&TX

expr1 peut ˆtre une expression quelconque. Si expr1 n'est pas d‚finie, elle
est remplac‚e par 1. Ainsi

&CO
    grt(X) est identique … grt(1, X)
&TX


&IT Moyenne mobile (Moving Average)
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
L'expression ma(expr1, expr2) donne la moyenne des expr1 derniŠres valeurs
de l'expression expr2. Si expr1 est n‚gatif ou nul, la valeur courante de
expr2 est retourn‚e par cette expression.

expr1 peut ˆtre une expression quelconque. Si expr1 n'est pas d‚finie, elle
est remplac‚e par 1. Ainsi

&CO
    ma(X) est identique … ma(1, X) et donc … X
&TX

&IT Maximum sur une p‚riode
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
L'expression vmax(expr1, expr2, expr3) calcule le maximum de l'expression
expr3 sur la p‚riode d‚finie par les indices expr1 et expr2. Ces deux
derniŠres expressions sont quelconques, mais leur r‚sultat est arrondi … la
partie entiŠre et consid‚r‚ comme un index de la s‚rie calcul‚e par expr3.

On utilisera par exemple :

&CO
    vmax(1970Y1, 1990Y1, X + Y)
&TX

pour calculer le maximum de la s‚rie X + Y sur la p‚riode 1970Y1 … 1990Y1.

Utilis‚ avec deux arguments, le second (absent) est remplac‚ par t, soit
l'indice actuel dans le cours de l'ex‚cution.

Utilis‚ avec un seul argument, le premier argument est fix‚ … 0 (origine de
la s‚rie) et le second … t (indice courant).

Ainsi :

&CO
    vmax(1970Y1, X + Y)  vaut   vmax(1970Y1, t, X + Y)
    vmax(X + Y)          vaut   vmax(0, t, X + Y)
&TX

&IT Minimum sur une p‚riode
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
L'expression vmin(expr1, expr2, expr3) calcule le minimum de l'expression
expr3 sur la p‚riode d‚finie par les indices expr1 et expr2. Ces deux
derniŠres expressions sont quelconques, mais leur r‚sultat est arrondi … la
partie entiŠre et consid‚r‚ comme un index de la s‚rie calcul‚e par expr3.

On utilisera par exemple :

&CO
    vmin(t - 2, t, X + Y)
&TX

pour calculer le minimum de la s‚rie X + Y sur les trois derniŠres valeurs
de la s‚rie (par rapport … la p‚riode de calcul courante).

Utilis‚ avec deux arguments, le second (absent) est remplac‚ par t, soit
l'indice actuel dans le cours de l'ex‚cution.

Utilis‚ avec un seul argument, le premier argument est fix‚ … 0 (origine de
la s‚rie) et le second … t (indice courant).

Ainsi :

&CO
    vmin(1970Y1, X + Y)  vaut   vmin(1970Y1, t, X + Y)
    vmin(X + Y)          vaut   vmin(0, t, X + Y)
    vmin(t, t, X + Y)    vaut   (X + Y)
&TX

&IT Somme sur une p‚riode
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
L'expression sum(expr1, expr2, expr3) calcule la somme de l'expression
expr3 sur la p‚riode d‚finie par les indices expr1 et expr2. Ces deux
derniŠres expressions sont quelconques, mais leur r‚sultat est arrondi … la
partie entiŠre et consid‚r‚ comme un index de la s‚rie calcul‚e par expr3.

On utilisera par exemple :

&CO
    sum(0, t, X + Y)
&TX

pour calculer le total de la s‚rie X + Y depuis son origine
jusqu'… la p‚riode de calcul courante.

Utilis‚ avec deux arguments, le second (absent) est remplac‚ par t, soit
l'indice actuel dans le cours de l'ex‚cution.

Utilis‚ avec un seul argument, le premier argument est fix‚ … 0 (origine de
la s‚rie) et le second … t (indice courant).

Ainsi :

&CO
    sum(1970Y1, X + Y)   vaut   sum(1970Y1, t, X + Y)
    sum(X + Y)           vaut   sum(0, t, X + Y)
&TX


&IT Produit sur une p‚riode
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
L'expression prod(expr1, expr2, expr3) calcule le produit de l'expression
expr3 sur la p‚riode d‚finie par les indices expr1 et expr2. Ces deux
derniŠres expressions sont quelconques, mais leur r‚sultat est arrondi … la
partie entiŠre et consid‚r‚ comme un index de la s‚rie calcul‚e par expr3.

On utilisera par exemple :

&CO
    prod(1985Y1, t - 1, X / X[85Y1])
&TX

pour calculer le produit cumul‚ de la s‚rie X mise en base 85 depuis
1985 jusqu'… la p‚riode courante - 1.

Utilis‚ avec deux arguments, le second (absent) est remplac‚ par t, soit
l'indice actuel dans le cours de l'ex‚cution.

Utilis‚ avec un seul argument, le premier argument est fix‚ … 0 (origine de
la s‚rie) et le second … t (indice courant).

Ainsi :

&CO
    prod(1970Y1, X + Y)  vaut   prod(1970Y1, t, X + Y)
    prod(X + Y)          vaut   prod(0, t, X + Y)
&TX


&IT Moyenne de p‚riode
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

L'expression mean(expr1, expr2, expr3) calcule la moyenne de l'expression
expr3 sur la p‚riode d‚finie par les indices expr1 et expr2. Ces deux
derniŠres expressions sont quelconques, mais leur r‚sultat est arrondi … la
partie entiŠre et consid‚r‚ comme un index de la s‚rie calcul‚e par expr3.

On utilisera par exemple :

&CO
    mean(0, t, X)
&TX

pour calculer la moyenne de la s‚rie X sur la p‚riode allant de
l'origine … l'ann‚e courante de calcul.

Utilis‚ avec deux arguments, le second (absent) est remplac‚ par t, soit
l'indice actuel dans le cours de l'ex‚cution.

Utilis‚ avec un seul argument, le premier argument est fix‚ … 0 (origine de
la s‚rie) et le second … t (indice courant).

Ainsi :

&CO
    mean(1970Y1, X + Y)  vaut   mean(1970Y1, t, X + Y)
    mean(X + Y)          vaut   mean(0, t, X + Y)
    mean(t - 1, t, X)    vaut   ma(2, X)
    mean(t, t, X)        vaut   X
&TX

&IT Index d'une valeur dans une s‚rie
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

    L'expression ~cindex(2.5, X)~C retourne la position de 2.5 dans la
    s‚rie X.

&IT Facteur d'auto-corr‚lation
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
    L'expression ~cacf([expr,[expr,]] expr1, expr2)~C retourne le
    facteur d'autocorr‚lation de l'expression expr2 de degr‚ expr1 sur
    la p‚riode (optionnelle) pass‚e comme premiers arguments. En l'absence
    de d‚finition la p‚riode est [0, t].

    Le degr‚ (expr1) ne peut d‚passer le quart du nombre d'observations
    de la p‚riode consid‚r‚e. Dans le cas contraire, la fonction
    retourne --.

&CO
    acf(1970Y1, X + Y)  vaut   acf(1970Y1, t, X + Y)
    acf(X + Y)          vaut   acf(0, t, X + Y)
&TX

&IT Variance
ÄÄÄÄÄÄÄÄÄÄÄÄ
    L'expression ~cvar([from [,to],] expr)~C retourne la variance
    de expr sur la p‚riode [from, to].

&CO
     var([from [,to],] X)             == sum((Xi-Xm)**2) / n
     o— Xm est la moyenne de X et n le nombre d'observations.
&TX

    Utilis‚ avec un seul argument, le premier argument est fix‚ … 0
    (origine de la s‚rie) et le second … t (indice courant).

    Ainsi :

&CO
    var(1970Y1, X + Y)  vaut   var(1970Y1, t, X + Y)
    var(X + Y)          vaut   var(0, t, X + Y)
&TX

&IT Covariance et covariance autour de l'origine
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
    L'expression ~ccovar([from [,to],] expr1, expr2)~C retourne la
    covariance de expr1 et expr2 sur la p‚riode [from,to].

    ~ccovar0()~C retourne la covariance autour de l'origine.
&CO
     covar([from [,to],] X, Y)  == sum((Xi-Xm)*(Yi-Ym)) / n
     covar0([from [,to],] X, Y) == sum(Xi * Yi) / n

     o— Xm est la moyenne de X et n le nombre d'observations.
	Ym est la moyenne de Y et n le nombre d'observations.
&TX

    Utilis‚ avec un seul argument, le premier argument est fix‚ … 0
    (origine de la s‚rie) et le second … t (indice courant).

    Ainsi :

&CO
    covar(1970Y1, X, Y)  vaut   covar(1970Y1, t, X, Y)
    covar(X, Y)          vaut   covar(0, t, X, Y)
&TX

&IT Corr‚lation
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
    L'expression ~ccorr([from [,to],] X, Y)~C retourne la corr‚lation
    entre X et Y.

&CO
     corr(X, Y) == covar(X, Y) / sqrt(var(X) * var(Y))
&TX

    Utilis‚ avec un seul argument, le premier argument est fix‚ … 0
    (origine de la s‚rie) et le second … t (indice courant).

    Ainsi :

&CO
    corr(1970Y1, X, Y)  vaut   corr(1970Y1, t, X, Y)
    corr(X, Y)          vaut   corr(0, t, X, Y)
&TX


&IT D‚viation standard sur une p‚riode
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
L'expression stderr(expr1, expr2, expr3) calcule la d‚viation standard de
l'expression expr3 sur la p‚riode d‚finie par les indices expr1 et expr2.
Ces deux derniŠres expressions sont quelconques, mais leur r‚sultat est
arrondi … la partie entiŠre et consid‚r‚ comme un index de la s‚rie
calcul‚e par expr3.

On utilisera par exemple :

&CO
    stderr(1970Y1, 1990Y1, X)
&TX

pour calculer l'‚cart type de la s‚rie X sur la p‚riode 1970 … 1990.

Utilis‚ avec deux arguments, le second (absent) est remplac‚ par t, soit
l'indice actuel dans le cours de l'ex‚cution.

Utilis‚ avec un seul argument, le premier argument est fix‚ … 0 (origine
de la s‚rie) et le second … t (indice courant).

Ainsi :

&CO
    stderr(1970Y1, X + Y)  vaut   stderr(1970Y1, t, X + Y)
    stderr(X + Y)          vaut   stderr(0, t, X + Y)
&TX

La mˆme syntaxe est d'application pour la fonction stddev(). A la
diff‚rence de stderr(), stddev() est un estimateur biais‚.

&CO
    stddev(x) == sqrt(var(x))
    stderr(x) == sqrt(sum((xi-xm)**2/(n-1)
&TX

&IT Calcul de la derniŠre observation
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette fonction permet d'obtenir la valeur de la derniŠre valeur non NA d'une s‚rie
sur une p‚riode donn‚e. Sa syntaxe est:
&CO
    lastobs([from [,to],] expr)
&TX

&IT Calcul d'interpolation
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette fonction fournit une valeur … expr en t en interpolant ou
extrapolant les valeurs connues. Cette inter ou extrapolation se fait
uniquement de fa‡on lin‚aire.

&CO
    interpol(expr)
&TX

Si on dispose de A en 90 et 92, ~cinterpol(A)~C en 91 vaudra ~c(A[90Y1]+A[92Y1])/2.~C
Plusieurs valeurs successives peuvent ˆtre manquantes. Dans ce cas, une
pond‚ration est utilis‚e en fonction de la proximit‚ des valeurs
disponibles.

&IT S‚ries apparent‚es
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
La fonction ~capp~C fournit une valeur … une expression lorsque
l'observation est manquante en utilisant une autre s‚rie comme s‚rie
apparent‚e. Sa syntaxe est :
&CO
    app(expr1, expr2)
&TX
La calcul se fait de fa‡on g‚om‚trique en se basant sur les valeurs connues les
plus proches dans la s‚rie … compl‚ter.

Le r‚sultat de la fonction
~capp(A,~C B) est obtenu comme suit au
temps t :

&EN Si
~cA[t]~C est d‚fini,
~cA[t]~C est retourn‚

&EN Si
~cA[t]~C n'est pas d‚fini, on calcule les valeurs
~ct0~C et
~ct1~C autour de
~ct~C
telles que
~cA[t0]~C et
~cA[t1]~C soient d‚finis et non nuls. Si ni
~ct0~C ni
~ct1~C ne
peuvent ˆtre trouv‚s, retourne
~cNaN.~C Il s'agit dans ce cas d'un s‚rie
~cA~C
valant ~cNaN~C sur toute la p‚riode.

&EN si seul
~ct0~C est d‚fini, retourne
~cB[t] * (A[t0] / B[t0])~C

&EN Si seul
~ct1~C est d‚fini, retourne
~cB[t] * (A[t1] / B[t1])~C

&EN Si
~ct0~C et
~ct1~C sont d‚finis, calcule d'abord
~cDelta = (A[t1]/A[t0]) / (B[t1]/B[t0])~C
puis retourne
~cA[t0] * (B[t]/B[t0]) * Delta ** ((t-t0)/(t1-t0))~C

&IT Lissage
ÄÄÄÄÄÄÄÄÄÄÄ
Les fonctions hp() et hpstd() calculent une s‚rie "liss‚e" bas‚e sur la
m‚thode d'Hodrick-Prescot d‚crite ici : <Hodrick-Prescott Filter>.



&CO
    hp([ [from,]to],expr)     : calcul sur base du logarithme de ~cexpr~C
    hpstd([ [from,]to],expr)  : calcul sans passage au logarithme de ~cexpr~C
&TX
Les paramŠtres sont :
&EN from : p‚riode de d‚part du calcul, premiŠre ann‚e par d‚faut
&EN to : p‚riode de fin de calcul, derniŠre ann‚e par d‚faut
&EN expr : expression LEC quelconque. Doit ˆtre strictement positif pour ~chp()~C

>

<Les listes ou macros>
    Les listes ou macros
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Il est possible de param‚trer des expressions LEC en y introduisant des
noms de listes (g‚r‚es et d‚finies dans le WS courant de listes). Ces
listes permettent aussi de raccourcir l'‚criture des ‚quations.

On peut ‚galement demander au LEC de calculer une liste en pla‡ant des
‚toiles ou des points d'interrogations. Dans ce cas l'expression
… "‚tendre" sera plac‚e entre simples quotes.

&TI Listes nomm‚es
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Une liste porte un nom r‚pondant aux mˆmes rŠgles syntaxiques qu'une
variable. Elle est pr‚c‚d‚e dans l'‚quation par un dollar ($) pour la
distinguer d'une variable :

Supposons que la liste A ait comme d‚finition

&CO
    PNB * c1 + c2
&TX

L'expression
&CO
    A + B + $A + 2
&TX

sera ‚quivalente …
&CO
    A + B + PNB * c1 + c2 + 2
&TX

&NO
Note : les listes doivent exister au moment de la compilation des formules.
Une fois compil‚es, les formes LEC ne le sont plus tant qu'il n'y a pas de
modification dans la formule LEC elle-mˆme. Les modifications apport‚es aux
d‚finitions des listes n'auront donc un impact au niveau des formes LEC
(‚quations, identit‚s, tableaux) que si les formes LEC sont recompil‚es!

&TI Listes calcul‚es
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Lorsque l'op‚rateur le permet ~c(lsum,~C ~cmax~C, etc), on peut
utiliser les wildcards dans le langage LEC. Cependant, pour
distinguer l'‚toile et l'op‚rateur de multiplication, la liste …
‚tendre doit ˆtre entour‚e de quotes simples('). Ainsi,
&CO
	lsum('A*')
&TX
est ‚quivalent …
&CO
lsum(A1,A2,AX)
&TX
si ~cA1,~C ~cA2~C et ~cAX~C sont les seules s‚ries commen‡ant pas
~cA~C dans le WS courant.

On peut ‚galement utiliser une combinaison de noms de s‚ries :
&CO
	max('*G;*F')
&TX

Le nombre maximum d'op‚randes des op‚rateurs est port‚ … 255, de
fa‡on … pouvoir exploiter au mieux les wildcards.

&NO Note
    ÄÄÄÄ
Les noms contenant des wildcards dans les formes LEC sont r‚solus
en fonction du contenu du WS COURANT. La forme LEC compil‚e est
m‚moris‚e avec ces noms. Si on change le contenu du WS, il est
possible que certaines s‚ries n'existent plus ou que de nouvelles
apparaissent. La forme compil‚e n'‚tant pas chang‚e
automatiquement, il faut si on souhaite adapter la forme LEC au
nouveau contenu, recompiler la forme en l'‚ditant.
>

<Lags, leads et valeurs de p‚riode>
    Utilisation des lags, leads et p‚riodes dans le formules
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les variables utilis‚es dans une expression LEC ont toujours une dimension
temporelle implicite : l'expression

&CO
    A + PNB
&TX
signifie
&CO
    A[t] + PNB[t]
&TX

Cette derniŠre ‚criture est cependant syntaxiquement fausse.

On peut appliquer … des variables ou plus g‚n‚ralement … des expressions
trois types de modifications d'indice temporel :

&EN le lag (retard)
&EN le lead (avance)
&EN la fixation de p‚riode

Dans les trois cas, on ajoute … l'expression modifi‚e une valeur entre
crochets repr‚sentant le lag, le lead ou la p‚riode.

&CO
    expr[-lag]
    expr[+lead]
    expr[period]
&TX

Un lag et un lead sont n‚cessairement des nombres entiers pr‚c‚d‚s par le
signe - (pour le lag) ou le signe + (pour le lead) indiquant le nombre de
p‚riodes de temps dont on veut d‚placer l'expression. La p‚riode est une
constante temporelle, soit par exemple 1970Q3.

Des combinaisons de lag, lead et fixations de p‚riode sont possibles. La
rŠgle est simple : dŠs qu'une p‚riode est fix‚e, les lag et les lead n'ont
plus d'effet sur l'expression. Les lag et lead sont simplement additionn‚s.

Le report des lag, lead et fixation de p‚riode s'effectue avant le calcul
des fonctions. Ainsi max(A, B)[-1] est identique … max(A[-1], B[-1]).

Voici quelques exemples :

&CO
    Expression               Equivalent ou signification
    ----------------------------------------------------
    X[-1]                    X[t-1] (faux syntaxiquement)
    (X + Y + c1)[1970Y1]     X[1970Y1] + Y[1970Y1] + c1
    (A + B[+1])[-2]          A[-2] + B[-1]
    (A[70Y1] + B)[-1][-2]    A[70Y1] + B[-3]
    (A[+1] + B[70Y1])[80Y1]  A[81Y1] + B[70Y1]
    (a + 2)[-1]              a + 2
    d X[-2]                  X[-2] - X[-3]
    (grt(X))[-2]             X[-2] / X[-3]
    max(A, B)[1970Y1]        max(A[70Y1], B[70Y1])
    d(A)[1970Y1]             0 (A[70Y1] - A[70Y1])
    t[-1]                    t (constante)
    mean(X)[-1]              NA (hors sample)
    mean(1, t, X)[-1]        vaut NA la premiŠre ann‚e
    A[1]                     faux
    B[70Y1 + 1]              faux
    C[t-1]                   faux
&TX
>
<Priorit‚ des op‚rateurs>
    Priorit‚ des op‚rateurs
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les priorit‚s d‚finies des op‚rateurs sont classiques. Par ordre de
priorit‚ croissante :

&EN l'op‚rateur logique or
&EN l'op‚rateur logique and
&EN les op‚rateurs de comparaison <<, <<=, etc
&EN la somme et la soustraction
&EN le produit et la division
&EN l'exposant
&EN les fonctions math‚matiques et temporelles

Par "a priorit‚ sur", il faut entendre "s'ex‚cute aprŠs" dans une formule.
Ainsi

&CO
    2.2 * X << 100
&TX
vaut 1 si (2.2 * X) est plus petit que 100 et 0 sinon.

Les op‚rateurs de priorit‚s ‚gales s'ex‚cutent de gauche … droite :

&CO
    2 - 1 + 2
&TX
vaut 3 car c'est ‚quivalent … (2 - 1) + 2.

Les parenthŠses peuvent ˆtre utilis‚es pour modifier cet ordre :

&CO
    2.2 * (X << 100)
&TX

vaut 2.2 si X est plus petit que 100 et 0 sinon.

Les op‚rateurs alg‚briques ont priorit‚ sur les fonctions, que celles-ci
soient math‚matiques ou temporelles :

&CO
    ln X + 2
&TX
est ‚quivalent …

&CO
    ln(X) + 2
&TX

&TI
>

<Les commentaires>
    Les commentaires
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Des commentaires peuvent ˆtre introduits dans une expression LEC. Il suffit
de les placer entre /* et */.

Par exemple :
&CO
    A + B + /* premiŠre partie  */
    C       /* s‚rie temporaire */
    + D     /* suite            */
&TX
L'introduction d'un commentaire permet entre autre de supprimer
temporairement une partie d'‚quation :

&CO
    ln Y := c1 + /* c2 * ln Z + */ c3 * ln Y[-1]
&TX

Une m‚thode alternative est de placer en fin d'expression un point-virgule
suivi d'un texte libre :

&CO
    ln Y := c1 + c2 * ln Y[-1] ; commentaire libre
&TX
A la diff‚rence des /* et */, le point-virgule termine l'‚quation : tout le
texte qui suit est ignor‚, y compris sur les lignes suivantes :

&CO
    A + B +
    C       ; * s‚rie temporaire
    + D     /* suite */
&TX

est ‚quivalent … :

&CO
   A + B + C
&TX
>

<Ecriture des ‚quations     >
    Ecriture des ‚quations
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Une ‚quation est simplement la juxtaposition de deux expressions LEC,
s‚par‚es par les caractŠres :=

&CO
    expr1 := expr2
&TX

Toutes les rŠgles d‚crites plus haut s'appliquent … chaque terme de
l'‚quation.

Comme une ‚quation porte le nom de la variable endogŠne, celle-ci doit
apparaŒtre dans l'‚quation. Elle peut y apparaŒtre plus d'une fois.
>
<R‚capitulatif de la syntaxe>
    R‚capitulatif de la syntaxe
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les lignes suivantes reprennent l'ensemble des ‚l‚ments du langage et leur
syntaxe.

&CO
constantes num‚riques  : 2, 2.0, 2.12E2 0.001e-03

constantes temporelles : 1990Y1, 80S2, 78Q4, 2003M12

constantes du langage  : pi, e, t

variables              : A, A_1, A123456789

scalaires              : a, a_1, x123456789

op‚rateurs logiques    : not expr
			 expr or expr
			 expr and expr
			 expr << expr
			 expr <<= expr
			 expr = expr
			 expr != expr
			 expr >>= expr
			 expr >> expr

op‚rateurs alg‚briques : expr + expr
			 expr - expr
			 expr / expr
			 expr * expr
			 expr ** expr

fonctions math‚matiques: -expr
			 +expr
			 log([expr], expr)
			 ln(expr)
			 exp([expr,] expr)
			 max(expr, ...)
			 min(expr, ...)
			 lsum(expr, expr, ...)
			 sin(expr)
			 cos(expr)
			 acos(expr)
			 asin(expr)
			 tan(expr)
			 atan(expr)
			 tanh(expr)
			 sinh(expr)
			 cosh(expr)
			 abs(expr)
			 sqrt(expr)
			 int(expr)
			 rad(expr)
			 if(expr, expr, expr)
			 sign(expr)
			 isan(expr)
			 lmean(expr, ...)
			 lprod(expr, ...)
			 lcount(expr, ...)
			 floor(expr)
			 ceil(expr)
			 round(expr [, n])
			 random(expr)


fonctions temporelles  : l([expr,] expr)
			 d([expr,] expr)
			 r([expr,] expr)
			 dln([expr,] expr)
			 grt([expr,] expr)
			 ma([expr,] expr)
			 mavg([expr,] expr)
			 vmax([expr,[expr,]] expr)
			 vmin([expr,[expr,]] expr)
			 sum([expr,[expr,]] expr)
			 prod([expr,[expr,]] expr)
			 mean([expr,[expr,]] expr)
			 index([expr,[expr,]] expr1, expr2)
			 acf([expr,[expr,]] expr, expr)
			 var([from [,to],] expr)
			 covar([from [,to],] expr1, expr2)
			 covar0([from [,to],] expr1, expr2)
			 corr([from [,to],] x, y)
			 stderr([expr,[expr,]] expr)
			 stddev([from [,to],] expr)
			 lastobs([from [,to],] expr)
			 interpol(expr)
			 app(expr1, expr2)
			 dapp(expr1, expr2)

listes ou macros       : $LISTNAME

lags, leads et p‚riodes: [+n] [-n] [1990Y1]

commentaires           : /* Comment */

‚quations              : expr := expr
&TX
>









