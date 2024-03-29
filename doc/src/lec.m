

<Le langage LEC>

    Le langage LEC
    컴컴컴컴컴컴컴

Qu'il s'agisse d'굌uations, d'identit굎 de construction ou encore de
graphiques, un langage d'괹riture de formules math굆atiques ad굌uat est
indispensable. Le langage LEC (Langage Econom굏rique Condens�) offre
l'avantage d'늯re � la fois concis dans son 괹riture et particuli둹ement
adapt� aux formules faisant intervenir des s굍ies chronologiques. Il est
utilis� chaque fois qu'une formule est n괹essaire dans le logiciel IODE.

Le LEC est 괾alement "naturel", en ce sens que sa syntaxe est proche de
l'괹riture de formules que l'on peut trouver dans la litt굍ature.

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


On peut classer les 굃굆ents constitutifs du langage en plusieurs classes :


&EN <les constantes du LEC            >
&EN <les variables                    >
&EN <les scalaires                    >
&EN <les op굍ateurs logiques          >
&EN <les op굍ateurs alg괷riques       >
&EN <les fonctions math굆atiques      >
&EN <les fonctions temporelles        >
&EN <les listes ou macros             >
&EN <lags, leads et valeurs de p굍iode>
&EN <les commentaires                 >

En fin de chapitre, les points suivants sont d굏aill굎 :

&EN <priorit� des op굍ateurs    >
&EN <괹riture des 굌uations     >
&EN <r괹apitulatif de la syntaxe>
>
<les constantes du LEC    >
    Les constantes du LEC
    컴컴컴컴컴컴컴컴컴컴컴컴컴컴�

&TI    Les constantes num굍iques
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

Celles-ci sont de simples valeurs qui peuvent 늯re enti둹es ou r괻lles :

Constantes enti둹es :
&CO
    2, 51234, 12345634
&TX

Constantes r괻lles en format flottant :
&CO
    2.234, 0.000123
&TX

Constantes r괻lles en format exponentiel :
&CO
    2.234e-3, 1E5, 1.2E-03
&TX


&TI    Les constantes temporelles
    컴컴컴컴컴컴컴컴컴컴컴컴컴

&IT Variable t
컴컴컴컴컴
Parmi les objets de IODE, les variables, qui sont des s굍ies
chronologiques sont d괽inies sur une p굍iode limit괻 de temps : le
SAMPLE du workspace de variables d굏ermine cette p굍iode. Par exemple,
1970Y1 � 2000Y1 signifie que les s굍ies sont annuelles (Y) et d괽inies
de 1970 � 2000.


On peut utiliser dans le langage LEC des constantes temporelles de format
semblable. Elles sont utilis괻s pour prendre la valeur d'une s굍ie � une
p굍iode fix괻 par exemple.


~uExemples~U :
&CO
    1990Y1
    70Q4
    2010M11
&TX


Elles peuvent 괾alement 늯re utiles dans des
op굍ations du type:

&CO
	X * (t << 1990Y1) + Y * (t >>= 1990Y1)
&TX

Cette derni둹e formule vaut X si la p굍iode en cours d'ex괹ution est
inf굍ieure � 1990 et Y sinon. On verra plus loin que t repr굎ente l'index
de la p굍iode en cours d'ex괹ution.

Les constantes temporelles prennent une valeur enti둹e qui est celle
de l'index dans la s굍ie temporelle de la p굍iode en cours d'ex괹ution.
Supposons que le WS de variables soit d괽ini sur un SAMPLE allant de 1970Y1
� 1995Y1. L'굌uivalent num굍ique de la constante temporelle 1970Y1 est 0.
De m늤e 1980Y1 vaut 10, etc.

Pourquoi ne pas simplement indiquer 0 ou 10 au lieu de 1970Y1 ou 1990Y1 ?
Tout simplement parce que la m늤e formule peut 늯re ex괹ut괻 sur des WS de
SAMPLE diff굍ents, et rester correcte. Si le SAMPLE s'굏end de
1975Y1 � 1995Y1, la valeur de 1980Y1 ne vaut plus 10 comme dans le premier
cas, mais 5. La formule n'a pas 굏� chang괻, mais son ex괹ution s'est
adapt괻 au contexte!


La constante ~ct~C indique donc l'index de la p굍iode en cours d'ex괹ution.
Elle a une valeur enti둹e.

Penons un autre exemple. Supposons que le WS de variables courant soit
d괽ini sur le SAMPLE ~c1990Q1~C - ~c1999Q4~C et qu'une simulation d'un mod둳e soit
en cours sur la p굍iode ~c1992Q1~C � ~c1994Q4.~C La valeur de ~ct~C changera � chaque
p굍iode simul괻 : en ~c1992Q1,~C ~ct~C vaut 8, en ~c1992Q2,~C ~ct~C vaut 9, et ainsi de
suite.

Dans la formule
&CO
    if(t << 1993Q1, 0, 12.3),
&TX

~c1993Q1~C vaut 12 et t vaut 8, 9, 10, 11, ...19, selon la p굍iode en cours de
simulation. On constate que la valeur de la formule reste 0 tant que t est
plus petit que 12, c'est-�-dire jusqu'en 1992Q4, et 12.3 ensuite.

Le m늤e principe peut 늯re appliqu� au calcul d'identit굎 qui elles aussi
sont d괽inies sur un SAMPLE et donc qui font 굒oluer la valeur de t.


&IT Variable i
컴컴컴컴컴�
Cette variable du langage LEC permet de conna똳re lors d'un calcul la
diff굍ence entre l'ann괻 de calcul d'une sous-expression par rapport �
l'ann괻 de calcul de toute la formule. Cette variable s'appelle i et
son comportement s'apparente � celui de t. Elle est donc invariante par
rapport � l'ann괻 de calcul d'une formule LEC.

On calcule toujours une forme LEC pour une valeur du temps t donn괻,
mais dans une sous-expression, t peut 늯re diff굍ent. C'est par exemple
le cas pour les op굍ateurs d(), dln(), sum(), ... On peut, grace � i,
conna똳re la diff굍ence entre le t de calcul de la formule et le t de
calcul de la sous-expression courante.

On peut donc calculer des expressions comme
&CO
    sum(t-2, t-4, A / (1 - i)**2),
&TX
    qui 굌uivaut � :

&CO
    A[-2]/(1-(-2))**2 + A[-3]/(1-(-3))**2 + A[-4]/(1-(-4))**2
&TX

Sans i, une telle expression ne peut 늯re 괹rite en LEC.

Cet op굍ateur a 굏� impl굆ent� pour permettre la traduction de
l'op굍ateur SUM de TROLL.

Ainsi, si on calcule une expression pour un t donn�, i prend les
valeurs suivantes selon les cas. Attention, les expressions indiqu괻s
ci-dessous sont les formules compl둻es, et pas une sous-expression!

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
    컴컴컴컴컴컴컴컴컴컴컴컴�

Trois constantes math굆atiques sont d괽inies dans le langage :

&EN pi qui vaut 3.141592653589
&EN e qui vaut 2.7182818284
&EN euro vaut 40.3399

>
<Les variables>
    Les variables
    컴컴컴컴컴컴�

Les variables repr굎entent des s굍ies num굍iques temporelles. Leur
nom est toujours 괹rit en caract둹es majuscules pour les distinguer des
scalaires (voir ci-dessous).

Les noms des variables peuvent contenir jusqu'� 10 caract둹es (20 � partir de la version 6.01). Ces
caract둹es doivent 늯re alphab굏iques (A-Z) ou num굍iques (0-9), ou encore
le caract둹e de soulignement. Chaque nom de variable doit commencer par un
caract둹e alphab굏ique MAJUSCULE.

&EX
    A, B_PNB, A123456789
&TX

sont des noms corrects de variables.

&EX
    _A1, 1A34, A_123456789, z_AV, A-2, B.X
&TX

sont des noms incorrects (z_AV est un nom de scalaire)

Le fait d'괹rire une variable dans une formule signifie qu'il faut en
prendre la valeur � la p굍iode de temps en cours d'ex괹ution (que ce soit
dans le cadre d'un mod둳e, d'un jeu d'identit굎 ou d'un tableau).

Pour en prendre la valeur � la p굍iode pr괹괺ente (lag), on 괹rira :

&CO
    PNB[-1]
&TX

La valeur � la p굍iode suivante (lead) s'괹rit :

&CO
    PNB[+1]
&TX

La valeur � la p굍iode 1990Y1 (d괾굈굍escence) s'괹rit :

&CO
    PNB[1990Y1]
&TX

Cette syntaxe est plus g굈굍ale et peut s'appliquer � des formules
enti둹es.
>
<Les scalaires>
    Les scalaires
    컴컴컴컴컴컴�

Les scalaires sont des variables sans dimension, donc d괽inies par une
seule valeur. Ce sont par exemple les coefficients estim굎 dans une
굌uation ou des valeurs constantes � travers le temps.

Les noms des scalaires peuvent contenir jusqu'� 10 caract둹es (20 � partir de la version 6.01). Ces
caract둹es doivent 늯re alphab굏iques (A-Z) ou num굍iques (0-9), ou encore
le caract둹e de soulignement. Chaque nom doit commencer par un caract둹e
alphab굏ique minuscule.

&EX
    a, c1, a_123456789
&TX

sont des noms corrects de scalaires.

&EX
    _a1, 1A34, a_123456789, Z_av
&TX

sont des noms incorrects (Z_av est un nom de variable)

Les op굍ations de lag, lead et d괾굈굍escence appliqu괻s � un scalaire sont
sans effet, mais permises.
>
<Les op굍ateurs logiques>
    Les op굍ateurs logiques
    컴컴컴컴컴컴컴컴컴컴컴�

Pour indiquer les op굍ations logiques � effectuer sur les valeurs d괽inies
par les constantes, variables ou scalaires, on dispose des op굍ateurs
suivants, pr굎ent굎 dans leur ordre de priorit� :

&EN not ou ! : n괾ation d'une expression
&EN or       : ou logique entre deux expressions
&EN and      : et logique entre deux expressions
&EN <<        : inf굍ieur
&EN <<=       : inf굍ieur ou 괾al
&EN =        : 괾alit�
&EN != ou <<>> : diff굍ent
&EN >>=       : inf굍ieur ou 괾al
&EN >>        : inf굍ieur

Les op굍ateurs logiques ont une valeur nulle lorsque la condition exprim괻
est fausse et 1 si les valeurs satisfont � la condition.

Notons qu'une fonction if() est 괾alement d괽inie et se trouve dans les
fonctions d괹rites plus bas dans le texte.

&EX
    Soient X = 1
	   Y = 0
	   Z = 2

    A la p굍iode courante

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

<Les op굍ateurs alg괷riques>
    Les op굍ateurs alg괷riques
    컴컴컴컴컴컴컴컴컴컴컴컴컴

Les calculs alg괷riques sont effectu굎 � l'aide des op굍ateurs suivants :

&EN + : addition
&EN - : soustraction
&EN / : division
&EN * : multiplication
&EN ** : exponentiation

Ceux-ci ont les significations et les priorit굎 habituelles.
>

<Les fonctions math굆atiques>
    Les fonctions math굆atiques
    컴컴컴컴컴컴컴컴컴컴컴컴컴�

De nombreuses fonctions math굆atiques sont impl굆ent괻s en LEC. Certaines
fonctions prennent un argument, d'autres plusieurs. Parfois, une fonction
accepte un nombre variable d'argument, les arguments absents pouvant prendre
une valeur par d괽aut.

De mani둹e g굈굍ale, une fonction qui n'a qu'un argument ne demande pas les
parenth둺es. Ainsi

&CO
    ln X + 2        est identique �     ln(X) + 2
    d X             est identique �     d(X)
&TX

Toutes les fonctions ont un nom enti둹ement 괹rit en minuscules. Des
coefficients ne peuvent donc pas prendre un des noms r굎erv굎 aux fonctions.

Dans la syntaxe qui suit, les notations suivantes sont utilis괻s :

&EN expr : n'importe quelle expression LEC (formule)
&EN crochets [] : partie optionnelle
&EN points de suspension  : r굋굏ition du dernier argument autant de fois que
    souhait�

Les fonctions math굆atiques sont :

&EN -expr                : moins unaire
&EN +expr                : plus unaire
&EN log([expr], expr)    : logarithme en base quelconque
&EN ln(expr)             : logarithme n굋굍ien
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
&EN sqrt(expr)           : racine carr괻
&EN int(expr)            : partie enti둹e
&EN rad(expr)            : transforme des degr굎 en radians
&EN if(expr, expr, expr) : condition
&EN sign(expr)           : signe d'une expression
&EN isan(expr)           : retourne 0 si expr est NAN et 1 sinon
&EN lmean(expr, ...)     : retourne la moyenne des expressions
&EN lprod(expr, ...)     : retourne le produit des expressions
&EN lcount(expr, ...)    : retourne le nombre d'expressions
&EN floor(expr) : partie enti둹e de l'expression
&EN ceil(expr) : partie enti둹e de l'expression plus 1
&EN round(expr [, n])  : arrondi de expr � la n둴e d괹imale.
			Si n n'est pas d괽ini, il est fix� � 0.
&EN random(expr)         : fournit un nombre al괶toire compris entre -expr/2 et +expr/2.

Quelques op굍ateurs demandent un compl굆ent d'explication :

&IT Plus unaire
컴컴컴컴컴�

Le plus unaire est sans effet : ++++++X vaut simplement X.

&IT Logarithme
컴컴컴컴컴
ln(X) est le logarithme n굋굍ien de X. log(X) vaut ln(X) tandis que log(10,
X) est le logarithme en base 10 de X.

&IT Exponentielle
컴컴컴컴컴컴�

exp(X) est 굌uivalent � e**X et exp(10, X) vaut 10**X.

&IT Max, min et lsum
컴컴컴컴컴컴컴컴컴컴
max(1, X, Y, Z + 2) prend la valeur maximale de toutes les valeurs pass괻s
comme argument (� la p굍iode courante). La fonction min() en prend le
minimum. La fonction lsum() effectue la somme de toutes les valeurs
pass괻s comme argument.

Dans les trois cas, le nombre d'argument soit 늯re compris entre 2 et 50.

&IT Partie enti둹e
컴컴컴컴컴컴컴
..... int(X)  prend comme valeur la partie enti둹e de X : int(2.23e1) vaut 22.
~cint(X)~C retourne l'entier le plus proche de X :

&CO
 int(2.2) = 2
 int(2.6) = 3
 int(2.5) = 3
&TX

&IT Fonction if
컴컴컴컴컴�
La fonction if permet de simplifier l'괹riture de conditions : le premier
argument est la condition, le second la valeur si la condition est
v굍ifi괻, le troisi둴e la valeur dans le cas contraire. Il faut signaler
que les trois arguments sont calcul굎 et qu'en cas d'erreur dans
l'ex괹ution de l'un d'eux, la formule retourne la valeur NA (not available)
m늤e si c'est l'expression inutile qui a g굈굍� l'erreur.

&EX
    if(t << 1992Y1, 2, X)   vaut 2 avant 1992Y1
				 X � partir de 1992Y1
    if(t << 1992Y1, 2, X/0) vaut NA � partir de 1992Y1
				 2 avant 1992Y1.
&TX
&IT Fonction sign
컴컴컴컴컴컴컴컴�
La fonction sign retourne le signe d'une expression :
&CO
    sign(expr) vaut
		    1 si expr >>= 0
		    -1 si expr << 0
&TX


&IT Fonction random
컴컴컴컴컴컴컴컴컴컴컴
La fonction ~crandom~C fournit un nombre pseudo-al괶toire compris entre
~c-expr/2~C et ~c+expr/2.~C Sa syntaxe est:

&CO
    random(expr)
&TX
>

<Les fonctions temporelles>
    Les fonctions temporelles
    컴컴컴컴컴컴컴컴컴컴컴컴�
Un langage 괹onom굏rique ne m굍iterait pas son nom sans fonctions
temporelles. De nombreuses 굌uations et formules demandent des calculs du
type

&CO
    c1 * (PNB - PNB[-1]) + c2 * (VXA - VXA[-1])
&TX

Les op굍ateurs temporels permettent de simplifier cette 괹riture en la
rempla놹nt par :

&CO
    c1 * d PNB + c2 * d VXA
&TX

Dans la syntaxe qui suit, les notations suivantes sont utilis괻s :

&EN expr : n'importe quelle expression LEC (formule)
&EN []   : partie optionnelle

Le LEC poss둪e des op굍ateurs temporels pour calculer les diff굍ences de
p굍iodes, les taux de croissance, les moyennes, les 괹arts-types, etc. En
voici la liste :

&EN l([expr,] expr)             : lag d'une expression
&EN d([expr,] expr)             : diff굍ence de p굍iodes
&EN r([expr,] expr)             : rapport de p굍iodes
&EN dln([expr,] expr)           : diff굍ence des logarithmes des p굍iodes
&EN grt([expr,] expr)           : taux de croissance
&EN ma([expr,] expr)            : moyenne mobile (Moving Average)
&EN mavg([expr,] expr)          : moyenne mobile (identique � ma)
&EN vmax([expr,[expr,]] expr)   : maximum sur une p굍iode
&EN vmin([expr,[expr,]] expr)   : minimum sur une p굍iode
&EN sum([expr,[expr,]] expr)    : somme sur une p굍iode
&EN prod([expr,[expr,]] expr)   : produit sur une p굍iode
&EN mean([expr,[expr,]] expr)   : moyenne de p굍iode
&EN index([expr,[expr,]] expr1, expr2) : index d'une valeur
&EN acf([expr,[expr,]] expr, expr) : facteur d'autocorr굃ation
&EN var([from [,to],] expr) : variance
&EN covar([from [,to],] expr1, expr2) : covariance
&EN covar0([from [,to],] expr1, expr2) : covariance autour de l'origine
&EN corr([from [,to],] x, y) : corr굃ation
&EN stderr([expr,[expr,]] expr) : d굒iation standard (non biais�)
&EN stddev([from [,to],] expr) : d굒iation standard (biais�)
&EN lastobs([from [,to],] expr) : calcule la derni둹e observation sur une p굍iode donn괻
&EN interpol(expr) : fournit une valeur � expr en t en interpolant ou extrapolant
&EN app(expr1, expr2) : fournit une valeur � expr1 en t en utilisant la
    s굍ie expr2 comme s굍ie apparent괻 par une m굏hode g굊m굏rique
&EN dapp(expr1, expr2) : fournit par diff굍ences une valeur � expr1 en t en
    utilisant la s굍ie expr2 comme s굍ie apparent괻
&EN hp([ [from,]to],expr)        : filtre Hodrick-Prescott avec passage au ~clog~C de ~cexpr~C
&EN hpstd([ [from,]to],expr)     : calcul sans passage au ~clog~C de ~cexpr~C
&EN appdif(expr1, expr2) : alias de dapp()



On trouvera ci-dessous la signification pr괹ise de tous les op굍ateurs
temporels.

&IT Lag d'une expression
컴컴컴컴컴컴컴컴컴컴
L'expression l(expr1, expr2) est 굌uivalente � expr2[expr1]. Cette
formulation permet de calculer ou de param굏rer des lags. Par exemple,
l'expression

&CO
    X[c1 + 2]
&TX
est syntaxiquement fausse, tandis que
&CO
    l(c1 + 2, X)
&TX
est parfaitement correcte et 굌uivalente.

Si expr1 n'est pas d괽inie, elle est remplac괻 par 1. Ainsi

&CO
    l(X) est identique � l(1, X)
&TX

&IT Diff굍ence de p굍iodes
컴컴컴컴컴컴컴컴컴컴컴

L'expression d(expr1, expr2) est 굌uivalente � expr2 - expr2[expr1],
o� le r굎ultat du calcul de expr1 est consid굍� comme un lag s'il est
n괾atif et un lead s'il est positif. Par exemple :

&CO
    d(2, X + Y)  est identique � (X + Y) - (X + Y)[-2]
&TX

expr1 peut 늯re une expression quelconque. Si expr1 n'est pas d괽inie, elle
est remplac괻 par 1. Ainsi

&CO
    d(X) est identique � d(1, X)
&TX

&IT Rapport de p굍iodes
컴컴컴컴컴컴컴컴컴�
L'expression r(expr1, expr2) est 굌uivalente � expr2 / expr2[expr1],
o� le r굎ultat du calcul de expr1 est consid굍� comme un lag s'il est
n괾atif et un lead s'il est positif. Par exemple :

&CO
    r(-2, X + Y)  est identique � (X + Y) / (X + Y)[+2]
&TX

expr1 peut 늯re une expression quelconque. Si expr1 n'est pas d괽inie, elle
est remplac괻 par 1. Ainsi

&CO
    r(X) est identique � r(1, X)
&TX

&IT Diff굍ence des logarithmes des p굍iodes
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
L'expression dln(expr1, expr2) 굌uivaut � ln(expr2) - ln(expr2)[expr1],
o� le r굎ultat du calcul de expr1 est consid굍� comme un lag s'il est
n괾atif et un lead s'il est positif. Par exemple :

&CO
    dln(3 - 2, X + Y) vaut ln(X + Y) - ln(X + Y)[-1]
&TX

expr1 peut 늯re une expression quelconque. Si expr1 n'est pas d괽inie, elle
est remplac괻 par 1. Ainsi

&CO
    dln(X) est identique � dln(1, X)
&TX

&IT Taux de croissance
컴컴컴컴컴컴컴컴컴
L'expression grt(expr1, expr2) 굌uivaut � :

&CO
    100 * (expr2 / expr2[expr1] - 1)
&TX
o� le r굎ultat du calcul de expr1 est consid굍� comme un lag s'il est
n괾atif et un lead s'il est positif. Par exemple :

&CO
    grt(2, X + Y) vaut 100 * ((X + Y) / (X + Y)[-2] - 1)
&TX

expr1 peut 늯re une expression quelconque. Si expr1 n'est pas d괽inie, elle
est remplac괻 par 1. Ainsi

&CO
    grt(X) est identique � grt(1, X)
&TX


&IT Moyenne mobile (Moving Average)
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
L'expression ma(expr1, expr2) donne la moyenne des expr1 derni둹es valeurs
de l'expression expr2. Si expr1 est n괾atif ou nul, la valeur courante de
expr2 est retourn괻 par cette expression.

expr1 peut 늯re une expression quelconque. Si expr1 n'est pas d괽inie, elle
est remplac괻 par 1. Ainsi

&CO
    ma(X) est identique � ma(1, X) et donc � X
&TX

&IT Maximum sur une p굍iode
컴컴컴컴컴컴컴컴컴컴컴�
L'expression vmax(expr1, expr2, expr3) calcule le maximum de l'expression
expr3 sur la p굍iode d괽inie par les indices expr1 et expr2. Ces deux
derni둹es expressions sont quelconques, mais leur r굎ultat est arrondi � la
partie enti둹e et consid굍� comme un index de la s굍ie calcul괻 par expr3.

On utilisera par exemple :

&CO
    vmax(1970Y1, 1990Y1, X + Y)
&TX

pour calculer le maximum de la s굍ie X + Y sur la p굍iode 1970Y1 � 1990Y1.

Utilis� avec deux arguments, le second (absent) est remplac� par t, soit
l'indice actuel dans le cours de l'ex괹ution.

Utilis� avec un seul argument, le premier argument est fix� � 0 (origine de
la s굍ie) et le second � t (indice courant).

Ainsi :

&CO
    vmax(1970Y1, X + Y)  vaut   vmax(1970Y1, t, X + Y)
    vmax(X + Y)          vaut   vmax(0, t, X + Y)
&TX

&IT Minimum sur une p굍iode
컴컴컴컴컴컴컴컴컴컴컴�
L'expression vmin(expr1, expr2, expr3) calcule le minimum de l'expression
expr3 sur la p굍iode d괽inie par les indices expr1 et expr2. Ces deux
derni둹es expressions sont quelconques, mais leur r굎ultat est arrondi � la
partie enti둹e et consid굍� comme un index de la s굍ie calcul괻 par expr3.

On utilisera par exemple :

&CO
    vmin(t - 2, t, X + Y)
&TX

pour calculer le minimum de la s굍ie X + Y sur les trois derni둹es valeurs
de la s굍ie (par rapport � la p굍iode de calcul courante).

Utilis� avec deux arguments, le second (absent) est remplac� par t, soit
l'indice actuel dans le cours de l'ex괹ution.

Utilis� avec un seul argument, le premier argument est fix� � 0 (origine de
la s굍ie) et le second � t (indice courant).

Ainsi :

&CO
    vmin(1970Y1, X + Y)  vaut   vmin(1970Y1, t, X + Y)
    vmin(X + Y)          vaut   vmin(0, t, X + Y)
    vmin(t, t, X + Y)    vaut   (X + Y)
&TX

&IT Somme sur une p굍iode
컴컴컴컴컴컴컴컴컴컴�
L'expression sum(expr1, expr2, expr3) calcule la somme de l'expression
expr3 sur la p굍iode d괽inie par les indices expr1 et expr2. Ces deux
derni둹es expressions sont quelconques, mais leur r굎ultat est arrondi � la
partie enti둹e et consid굍� comme un index de la s굍ie calcul괻 par expr3.

On utilisera par exemple :

&CO
    sum(0, t, X + Y)
&TX

pour calculer le total de la s굍ie X + Y depuis son origine
jusqu'� la p굍iode de calcul courante.

Utilis� avec deux arguments, le second (absent) est remplac� par t, soit
l'indice actuel dans le cours de l'ex괹ution.

Utilis� avec un seul argument, le premier argument est fix� � 0 (origine de
la s굍ie) et le second � t (indice courant).

Ainsi :

&CO
    sum(1970Y1, X + Y)   vaut   sum(1970Y1, t, X + Y)
    sum(X + Y)           vaut   sum(0, t, X + Y)
&TX


&IT Produit sur une p굍iode
컴컴컴컴컴컴컴컴컴컴컴�
L'expression prod(expr1, expr2, expr3) calcule le produit de l'expression
expr3 sur la p굍iode d괽inie par les indices expr1 et expr2. Ces deux
derni둹es expressions sont quelconques, mais leur r굎ultat est arrondi � la
partie enti둹e et consid굍� comme un index de la s굍ie calcul괻 par expr3.

On utilisera par exemple :

&CO
    prod(1985Y1, t - 1, X / X[85Y1])
&TX

pour calculer le produit cumul� de la s굍ie X mise en base 85 depuis
1985 jusqu'� la p굍iode courante - 1.

Utilis� avec deux arguments, le second (absent) est remplac� par t, soit
l'indice actuel dans le cours de l'ex괹ution.

Utilis� avec un seul argument, le premier argument est fix� � 0 (origine de
la s굍ie) et le second � t (indice courant).

Ainsi :

&CO
    prod(1970Y1, X + Y)  vaut   prod(1970Y1, t, X + Y)
    prod(X + Y)          vaut   prod(0, t, X + Y)
&TX


&IT Moyenne de p굍iode
컴컴컴컴컴컴컴컴컴

L'expression mean(expr1, expr2, expr3) calcule la moyenne de l'expression
expr3 sur la p굍iode d괽inie par les indices expr1 et expr2. Ces deux
derni둹es expressions sont quelconques, mais leur r굎ultat est arrondi � la
partie enti둹e et consid굍� comme un index de la s굍ie calcul괻 par expr3.

On utilisera par exemple :

&CO
    mean(0, t, X)
&TX

pour calculer la moyenne de la s굍ie X sur la p굍iode allant de
l'origine � l'ann괻 courante de calcul.

Utilis� avec deux arguments, le second (absent) est remplac� par t, soit
l'indice actuel dans le cours de l'ex괹ution.

Utilis� avec un seul argument, le premier argument est fix� � 0 (origine de
la s굍ie) et le second � t (indice courant).

Ainsi :

&CO
    mean(1970Y1, X + Y)  vaut   mean(1970Y1, t, X + Y)
    mean(X + Y)          vaut   mean(0, t, X + Y)
    mean(t - 1, t, X)    vaut   ma(2, X)
    mean(t, t, X)        vaut   X
&TX

&IT Index d'une valeur dans une s굍ie
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�

    L'expression ~cindex(2.5, X)~C retourne la position de 2.5 dans la
    s굍ie X.

&IT Facteur d'auto-corr굃ation
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
    L'expression ~cacf([expr,[expr,]] expr1, expr2)~C retourne le
    facteur d'autocorr굃ation de l'expression expr2 de degr� expr1 sur
    la p굍iode (optionnelle) pass괻 comme premiers arguments. En l'absence
    de d괽inition la p굍iode est [0, t].

    Le degr� (expr1) ne peut d굋asser le quart du nombre d'observations
    de la p굍iode consid굍괻. Dans le cas contraire, la fonction
    retourne --.

&CO
    acf(1970Y1, X + Y)  vaut   acf(1970Y1, t, X + Y)
    acf(X + Y)          vaut   acf(0, t, X + Y)
&TX

&IT Variance
컴컴컴컴컴컴
    L'expression ~cvar([from [,to],] expr)~C retourne la variance
    de expr sur la p굍iode [from, to].

&CO
     var([from [,to],] X)             == sum((Xi-Xm)**2) / n
     o� Xm est la moyenne de X et n le nombre d'observations.
&TX

    Utilis� avec un seul argument, le premier argument est fix� � 0
    (origine de la s굍ie) et le second � t (indice courant).

    Ainsi :

&CO
    var(1970Y1, X + Y)  vaut   var(1970Y1, t, X + Y)
    var(X + Y)          vaut   var(0, t, X + Y)
&TX

&IT Covariance et covariance autour de l'origine
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
    L'expression ~ccovar([from [,to],] expr1, expr2)~C retourne la
    covariance de expr1 et expr2 sur la p굍iode [from,to].

    ~ccovar0()~C retourne la covariance autour de l'origine.
&CO
     covar([from [,to],] X, Y)  == sum((Xi-Xm)*(Yi-Ym)) / n
     covar0([from [,to],] X, Y) == sum(Xi * Yi) / n

     o� Xm est la moyenne de X et n le nombre d'observations.
	Ym est la moyenne de Y et n le nombre d'observations.
&TX

    Utilis� avec un seul argument, le premier argument est fix� � 0
    (origine de la s굍ie) et le second � t (indice courant).

    Ainsi :

&CO
    covar(1970Y1, X, Y)  vaut   covar(1970Y1, t, X, Y)
    covar(X, Y)          vaut   covar(0, t, X, Y)
&TX

&IT Corr굃ation
컴컴컴컴컴컴컴�
    L'expression ~ccorr([from [,to],] X, Y)~C retourne la corr굃ation
    entre X et Y.

&CO
     corr(X, Y) == covar(X, Y) / sqrt(var(X) * var(Y))
&TX

    Utilis� avec un seul argument, le premier argument est fix� � 0
    (origine de la s굍ie) et le second � t (indice courant).

    Ainsi :

&CO
    corr(1970Y1, X, Y)  vaut   corr(1970Y1, t, X, Y)
    corr(X, Y)          vaut   corr(0, t, X, Y)
&TX


&IT D굒iation standard sur une p굍iode
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
L'expression stderr(expr1, expr2, expr3) calcule la d굒iation standard de
l'expression expr3 sur la p굍iode d괽inie par les indices expr1 et expr2.
Ces deux derni둹es expressions sont quelconques, mais leur r굎ultat est
arrondi � la partie enti둹e et consid굍� comme un index de la s굍ie
calcul괻 par expr3.

On utilisera par exemple :

&CO
    stderr(1970Y1, 1990Y1, X)
&TX

pour calculer l'괹art type de la s굍ie X sur la p굍iode 1970 � 1990.

Utilis� avec deux arguments, le second (absent) est remplac� par t, soit
l'indice actuel dans le cours de l'ex괹ution.

Utilis� avec un seul argument, le premier argument est fix� � 0 (origine
de la s굍ie) et le second � t (indice courant).

Ainsi :

&CO
    stderr(1970Y1, X + Y)  vaut   stderr(1970Y1, t, X + Y)
    stderr(X + Y)          vaut   stderr(0, t, X + Y)
&TX

La m늤e syntaxe est d'application pour la fonction stddev(). A la
diff굍ence de stderr(), stddev() est un estimateur biais�.

&CO
    stddev(x) == sqrt(var(x))
    stderr(x) == sqrt(sum((xi-xm)**2/(n-1)
&TX

&IT Calcul de la derni둹e observation
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
Cette fonction permet d'obtenir la valeur de la derni둹e valeur non NA d'une s굍ie
sur une p굍iode donn괻. Sa syntaxe est:
&CO
    lastobs([from [,to],] expr)
&TX

&IT Calcul d'interpolation
컴컴컴컴컴컴컴컴컴컴컴컴컴

Cette fonction fournit une valeur � expr en t en interpolant ou
extrapolant les valeurs connues. Cette inter ou extrapolation se fait
uniquement de fa뇇n lin괶ire.

&CO
    interpol(expr)
&TX

Si on dispose de A en 90 et 92, ~cinterpol(A)~C en 91 vaudra ~c(A[90Y1]+A[92Y1])/2.~C
Plusieurs valeurs successives peuvent 늯re manquantes. Dans ce cas, une
pond굍ation est utilis괻 en fonction de la proximit� des valeurs
disponibles.

&IT S굍ies apparent괻s
컴컴컴컴컴컴컴컴컴컴컴
La fonction ~capp~C fournit une valeur � une expression lorsque
l'observation est manquante en utilisant une autre s굍ie comme s굍ie
apparent괻. Sa syntaxe est :
&CO
    app(expr1, expr2)
&TX
La calcul se fait de fa뇇n g굊m굏rique en se basant sur les valeurs connues les
plus proches dans la s굍ie � compl굏er.

Le r굎ultat de la fonction
~capp(A,~C B) est obtenu comme suit au
temps t :

&EN Si
~cA[t]~C est d괽ini,
~cA[t]~C est retourn�

&EN Si
~cA[t]~C n'est pas d괽ini, on calcule les valeurs
~ct0~C et
~ct1~C autour de
~ct~C
telles que
~cA[t0]~C et
~cA[t1]~C soient d괽inis et non nuls. Si ni
~ct0~C ni
~ct1~C ne
peuvent 늯re trouv굎, retourne
~cNaN.~C Il s'agit dans ce cas d'un s굍ie
~cA~C
valant ~cNaN~C sur toute la p굍iode.

&EN si seul
~ct0~C est d괽ini, retourne
~cB[t] * (A[t0] / B[t0])~C

&EN Si seul
~ct1~C est d괽ini, retourne
~cB[t] * (A[t1] / B[t1])~C

&EN Si
~ct0~C et
~ct1~C sont d괽inis, calcule d'abord
~cDelta = (A[t1]/A[t0]) / (B[t1]/B[t0])~C
puis retourne
~cA[t0] * (B[t]/B[t0]) * Delta ** ((t-t0)/(t1-t0))~C

&IT Lissage
컴컴컴컴컴�
Les fonctions hp() et hpstd() calculent une s굍ie "liss괻" bas괻 sur la
m굏hode d'Hodrick-Prescot d괹rite ici : <Hodrick-Prescott Filter>.



&CO
    hp([ [from,]to],expr)     : calcul sur base du logarithme de ~cexpr~C
    hpstd([ [from,]to],expr)  : calcul sans passage au logarithme de ~cexpr~C
&TX
Les param둻res sont :
&EN from : p굍iode de d굋art du calcul, premi둹e ann괻 par d괽aut
&EN to : p굍iode de fin de calcul, derni둹e ann괻 par d괽aut
&EN expr : expression LEC quelconque. Doit 늯re strictement positif pour ~chp()~C

>

<Les listes ou macros>
    Les listes ou macros
    컴컴컴컴컴컴컴컴컴컴

Il est possible de param굏rer des expressions LEC en y introduisant des
noms de listes (g굍괻s et d괽inies dans le WS courant de listes). Ces
listes permettent aussi de raccourcir l'괹riture des 굌uations.

On peut 괾alement demander au LEC de calculer une liste en pla놹nt des
굏oiles ou des points d'interrogations. Dans ce cas l'expression
� "굏endre" sera plac괻 entre simples quotes.

&TI Listes nomm괻s
컴컴컴컴컴컴컴
Une liste porte un nom r굋ondant aux m늤es r둮les syntaxiques qu'une
variable. Elle est pr괹괺괻 dans l'굌uation par un dollar ($) pour la
distinguer d'une variable :

Supposons que la liste A ait comme d괽inition

&CO
    PNB * c1 + c2
&TX

L'expression
&CO
    A + B + $A + 2
&TX

sera 굌uivalente �
&CO
    A + B + PNB * c1 + c2 + 2
&TX

&NO
Note : les listes doivent exister au moment de la compilation des formules.
Une fois compil괻s, les formes LEC ne le sont plus tant qu'il n'y a pas de
modification dans la formule LEC elle-m늤e. Les modifications apport괻s aux
d괽initions des listes n'auront donc un impact au niveau des formes LEC
(굌uations, identit굎, tableaux) que si les formes LEC sont recompil괻s!

&TI Listes calcul괻s
컴컴컴컴컴컴컴컴
Lorsque l'op굍ateur le permet ~c(lsum,~C ~cmax~C, etc), on peut
utiliser les wildcards dans le langage LEC. Cependant, pour
distinguer l'굏oile et l'op굍ateur de multiplication, la liste �
굏endre doit 늯re entour괻 de quotes simples('). Ainsi,
&CO
	lsum('A*')
&TX
est 굌uivalent �
&CO
lsum(A1,A2,AX)
&TX
si ~cA1,~C ~cA2~C et ~cAX~C sont les seules s굍ies commen놹nt pas
~cA~C dans le WS courant.

On peut 괾alement utiliser une combinaison de noms de s굍ies :
&CO
	max('*G;*F')
&TX

Le nombre maximum d'op굍andes des op굍ateurs est port� � 255, de
fa뇇n � pouvoir exploiter au mieux les wildcards.

&NO Note
    컴컴
Les noms contenant des wildcards dans les formes LEC sont r굎olus
en fonction du contenu du WS COURANT. La forme LEC compil괻 est
m굆oris괻 avec ces noms. Si on change le contenu du WS, il est
possible que certaines s굍ies n'existent plus ou que de nouvelles
apparaissent. La forme compil괻 n'굏ant pas chang괻
automatiquement, il faut si on souhaite adapter la forme LEC au
nouveau contenu, recompiler la forme en l'괺itant.
>

<Lags, leads et valeurs de p굍iode>
    Utilisation des lags, leads et p굍iodes dans le formules
    컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

Les variables utilis괻s dans une expression LEC ont toujours une dimension
temporelle implicite : l'expression

&CO
    A + PNB
&TX
signifie
&CO
    A[t] + PNB[t]
&TX

Cette derni둹e 괹riture est cependant syntaxiquement fausse.

On peut appliquer � des variables ou plus g굈굍alement � des expressions
trois types de modifications d'indice temporel :

&EN le lag (retard)
&EN le lead (avance)
&EN la fixation de p굍iode

Dans les trois cas, on ajoute � l'expression modifi괻 une valeur entre
crochets repr굎entant le lag, le lead ou la p굍iode.

&CO
    expr[-lag]
    expr[+lead]
    expr[period]
&TX

Un lag et un lead sont n괹essairement des nombres entiers pr괹괺굎 par le
signe - (pour le lag) ou le signe + (pour le lead) indiquant le nombre de
p굍iodes de temps dont on veut d굋lacer l'expression. La p굍iode est une
constante temporelle, soit par exemple 1970Q3.

Des combinaisons de lag, lead et fixations de p굍iode sont possibles. La
r둮le est simple : d둺 qu'une p굍iode est fix괻, les lag et les lead n'ont
plus d'effet sur l'expression. Les lag et lead sont simplement additionn굎.

Le report des lag, lead et fixation de p굍iode s'effectue avant le calcul
des fonctions. Ainsi max(A, B)[-1] est identique � max(A[-1], B[-1]).

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
    mean(1, t, X)[-1]        vaut NA la premi둹e ann괻
    A[1]                     faux
    B[70Y1 + 1]              faux
    C[t-1]                   faux
&TX
>
<Priorit� des op굍ateurs>
    Priorit� des op굍ateurs
    컴컴컴컴컴컴컴컴컴컴컴�

Les priorit굎 d괽inies des op굍ateurs sont classiques. Par ordre de
priorit� croissante :

&EN l'op굍ateur logique or
&EN l'op굍ateur logique and
&EN les op굍ateurs de comparaison <<, <<=, etc
&EN la somme et la soustraction
&EN le produit et la division
&EN l'exposant
&EN les fonctions math굆atiques et temporelles

Par "a priorit� sur", il faut entendre "s'ex괹ute apr둺" dans une formule.
Ainsi

&CO
    2.2 * X << 100
&TX
vaut 1 si (2.2 * X) est plus petit que 100 et 0 sinon.

Les op굍ateurs de priorit굎 괾ales s'ex괹utent de gauche � droite :

&CO
    2 - 1 + 2
&TX
vaut 3 car c'est 굌uivalent � (2 - 1) + 2.

Les parenth둺es peuvent 늯re utilis괻s pour modifier cet ordre :

&CO
    2.2 * (X << 100)
&TX

vaut 2.2 si X est plus petit que 100 et 0 sinon.

Les op굍ateurs alg괷riques ont priorit� sur les fonctions, que celles-ci
soient math굆atiques ou temporelles :

&CO
    ln X + 2
&TX
est 굌uivalent �

&CO
    ln(X) + 2
&TX

&TI
>

<Les commentaires>
    Les commentaires
    컴컴컴컴컴컴컴컴

Des commentaires peuvent 늯re introduits dans une expression LEC. Il suffit
de les placer entre /* et */.

Par exemple :
&CO
    A + B + /* premi둹e partie  */
    C       /* s굍ie temporaire */
    + D     /* suite            */
&TX
L'introduction d'un commentaire permet entre autre de supprimer
temporairement une partie d'굌uation :

&CO
    ln Y := c1 + /* c2 * ln Z + */ c3 * ln Y[-1]
&TX

Une m굏hode alternative est de placer en fin d'expression un point-virgule
suivi d'un texte libre :

&CO
    ln Y := c1 + c2 * ln Y[-1] ; commentaire libre
&TX
A la diff굍ence des /* et */, le point-virgule termine l'굌uation : tout le
texte qui suit est ignor�, y compris sur les lignes suivantes :

&CO
    A + B +
    C       ; * s굍ie temporaire
    + D     /* suite */
&TX

est 굌uivalent � :

&CO
   A + B + C
&TX
>

<Ecriture des 굌uations     >
    Ecriture des 굌uations
    컴컴컴컴컴컴컴컴컴컴컴

Une 굌uation est simplement la juxtaposition de deux expressions LEC,
s굋ar괻s par les caract둹es :=

&CO
    expr1 := expr2
&TX

Toutes les r둮les d괹rites plus haut s'appliquent � chaque terme de
l'굌uation.

Comme une 굌uation porte le nom de la variable endog둵e, celle-ci doit
appara똳re dans l'굌uation. Elle peut y appara똳re plus d'une fois.
>
<R괹apitulatif de la syntaxe>
    R괹apitulatif de la syntaxe
    컴컴컴컴컴컴컴컴컴컴컴컴컴�

Les lignes suivantes reprennent l'ensemble des 굃굆ents du langage et leur
syntaxe.

&CO
constantes num굍iques  : 2, 2.0, 2.12E2 0.001e-03

constantes temporelles : 1990Y1, 80S2, 78Q4, 2003M12

constantes du langage  : pi, e, t

variables              : A, A_1, A123456789

scalaires              : a, a_1, x123456789

op굍ateurs logiques    : not expr
			 expr or expr
			 expr and expr
			 expr << expr
			 expr <<= expr
			 expr = expr
			 expr != expr
			 expr >>= expr
			 expr >> expr

op굍ateurs alg괷riques : expr + expr
			 expr - expr
			 expr / expr
			 expr * expr
			 expr ** expr

fonctions math굆atiques: -expr
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

lags, leads et p굍iodes: [+n] [-n] [1990Y1]

commentaires           : /* Comment */

굌uations              : expr := expr
&TX
>









