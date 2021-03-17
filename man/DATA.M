/* DATA */

<Data>
   Data
   ÄÄÄÄ

.~a'mdata.png'


Le menu DATA permet de cr‚er, d‚truire et modifier des objets dans les
diff‚rents WS pr‚sents en m‚moire. Ce menu propose ‚galement des
fonctions utilitaires comme la recherche de textes dans les objets des
WS ou la duplication d'objets.

La partie DATA EDIT se d‚roule de fa‡on semblable pour tous les types
d'objets :

&EN un premier ‚cran permet d'introduire la liste des objets …
    ‚diter

&EN un deuxiŠme ‚cran pr‚sente les objets s‚lectionn‚s sous forme
    d'un tableau dans lequel on pourra se d‚placer

&EN un troisiŠme ‚cran affiche la valeur et permet la modification et la
    cr‚ation des objets

La premiŠre ‚tape peut ˆtre court-circuit‚e en tapant la touche F10 : on
se retrouve alors devant un tableau contenant tous les objets du WS.

Ci-dessous sont expos‚es les parties communes … l'‚dition de tous les
objets : l'introduction des noms d'objets … ‚diter et l'utilisation du
tableau ('grid' ou 'scroll') des objets s‚lectionn‚s.

Dans les chapitres correspondants … l'‚dition de chaque type d'objet, on
trouvera les sp‚cificit‚s de chaque ‚diteur.

Les fonctions sp‚cifiques aux calculs et aux constructions de listes
se trouvent dans le chapitre sur l'‚dition des listes.

La fonction de duplication d'objets termine ce chapitre.

La suite du chapitre est donc compos‚e des sujets suivants :

&EN <S‚lection des objets … ‚diter>
&EN <Edition d'un tableau d'objets>
&EN Edition d'un objet particulier :

&EN2 <Comments  > : ‚dition du WS de commentaires
&EN2 <Equations > : ‚dition du WS d'‚quations
&EN2 <Identities> : ‚dition du WS d'identit‚s
&EN2 <Lists     > : ‚dition du WS de listes
&EN2 <Scalars   > : ‚dition du WS de scalaires
&EN2 <Tables    > : ‚dition du WS de tableaux
&EN2 <Variables > : ‚dition du WS de variables

&EN <Duplicate objects> : copie d'objets … partir de WS sur disque vers les
			WS courants

&NO Tous les objets g‚r‚s par IODE ont une taille limite de 32000
    caractŠres, pour des raisons tenant … l'architecture du systŠme.
    Ce n'est en g‚n‚ral pas trŠs restrictif, sinon parfois dans le cas
    des tableaux, qui ne doivent pas d‚passer individuellement un nombre
    de lignes raisonnable (60 … 100). Il en va de mˆme pour les
    ‚quations et autres formes LEC : le bloc limite de 32000 caractŠres
    doit pouvoir contenir toutes les informations ayant trait … l'objet
    (forme LEC, forme compil‚e, etc).
>
<S‚lection des objets … ‚diter>
    S‚lection des objets … ‚diter
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Plut“t que d'afficher dans un tableau tous les objets d'un type donn‚
actuellement pr‚sents en WS, ce qui rend la visualisation ardue, il est
possible de s‚lectionner une liste d'objets. Un ‚cran de saisie,
identique pour tous les types d'objets, est pr‚vu …
cette fin.

.~a'pdata.png'

Cet ‚cran contient un champ de type EDITOR destin‚ … contenir des noms
d'objets. Ceux-ci peuvent ˆtre entr‚s s‚par‚ment ou sous forme de
listes. Dans les deux cas, les noms seront s‚par‚s par un des
s‚parateurs suivants :

&EN le blanc
&EN la virgule
&EN le point-virgule
&EN le retour … la ligne

Si un nom de liste apparaŒt dans le champ, il doit ˆtre pr‚fix‚ par le
caractŠre $. Ainsi, en supposant que la liste EQS soit d‚finie comme

&CO
    EQ1, EQ2, EQ3, EQ4
&TX

les objets ‚dit‚s, si on introduit comme liste d'objets

&CO
    EQ0, $EQS, EQ5
&TX

seront :

&CO
    EQ0, EQ1, EQ2, EQ3, EQ4, EQ5
&TX

&IT Cas particuliers
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Si la liste est vide, tous les objets du WS sont s‚lectionn‚s pour
l'‚dition.

Si la liste contient des noms d'objets inexistants, aucun message n'est
produit. Les objets non d‚finis ne seront simplement pas repris dans le
tableau d'‚dition.
>

<Edition d'un tableau d'objets>
    Edition d'un tableau d'objets
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

~a'sdata.png'

Le tableau d'‚dition (appel‚ aussi "grid") est compos‚ des ‚l‚ments
suivants :

&EN le nom du WS et le nombre d'objets dans la ligne sup‚rieure
&EN le nom des objets dans la colonne de gauche
&EN la d‚finition (ou une partie de la d‚finition) des objets en regard
    du nom dans la partie de droite
&EN un scrollbar indiquant la position courante dans le tableau et la
    proportion visible du tableau
&EN des ic“nes sur le contour du tableau

L'objet courant est indiqu‚ par le fait que la ligne qui lui correspond
est de couleur diff‚rente des autres lignes.

&TI Touches-fonctions
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Certaines touches vont
d‚clencher des actions soit sur les objets eux-mˆmes, soit sur
l'affichage du tableau :

&IT Sur les objets
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&EN ENTER : ‚dite l'objet courant (cliquer avec la souris a le mˆme effet)
&EN INS : cr‚e un nouvel objet
&EN DEL : d‚truit l'objet courant (aprŠs confirmation)
&EN Alt-F1 : ouvre un sous-tableaux avec les commentaires li‚s … la
    d‚finition de l'objet courant. On peut de la sorte par exemple
    afficher les commentaires de toutes les s‚ries utilis‚es dans la
    d‚finition de l'‚quation courante.
&EN Alt-F2 : idem Alt-F1 avec affichage des Equations
&EN Alt-F3 : idem Alt-F1 avec affichage des Identit‚s
&EN Alt-F4 : idem Alt-F1 avec affichage des Listes
&EN Alt-F5 : idem Alt-F1 avec affichage des Scalaires
&EN Alt-F6 : idem Alt-F1 avec affichage des Tableaux
&EN Alt-F7 : idem Alt-F1 avec affichage des Variables

&IT Sur le choix de l'objet courant
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&EN FlŠche en haut : l'objet pr‚c‚dent devient l'objet courant
&EN FlŠche en bas : l'objet suivant devient l'objet courant
&EN Page Up : le tableau d‚file d'un ‚cran vers le haut. L'objet courant
    s'adapte en cons‚quence
&EN Page Down : le tableau d‚file d'un ‚cran vers le bas. L'objet courant
    s'adapte en cons‚quence
&EN Ctrl-Home : positionnement sur le premier objet du tableau
&EN Ctrl-End : positionnement sur le dernier objet du tableau
&EN Lettre : d‚placement sur l'objet suivant commen‡ant par la lettre
    frapp‚e.

Dans le cas des scalaires, des tableaux et des variables, les flŠches …
droite et … gauche permettent en plus de se d‚placer sur les colonnes …
‚diter (p‚riodes dans le cas des variables).

&IT Sur l'affichage
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&EN Ctrl-O (mOve) : permet de d‚placer le tableau dans l'‚cran
&EN Ctrl-R (Rotate) : inverse lignes et colonnes (pour les variables)
&EN Ctrl-Z (resiZe) : permet de modifier la taille du tableau
&EN Ctrl-X (maXimize) : donne au tableau sa taille maximum

&IT Effet global
ÄÄÄÄÄÄÄÄÄÄÄÄ
&EN F1 AccŠs … ce manuel en ligne
&EN Escape : quitte le tableau
&EN Espace : ouvre le menu local … la fenˆtre

&IT Autres touches fonctions
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Selon le type d'objet affich‚, diff‚rentes touches fonctions peuvent
ˆtre d‚finies. Elles apparaissent dans la ligne de commentaire de
l'‚cran (avant-derniŠre ligne) et dans le menu local ([=]).

&TI Souris
ÄÄÄÄÄÄ
La souris facilite un certain nombre d'op‚rations dans l'utilisation des
grids :

&EN un simple clic permet de positionner le curseur sur l'objet voulu,
    et … la position souhait‚e. Un second clic ‚dite l'objet s‚lectionn‚.

&EN sur le contour de la fenˆtre, des petites ic“nes permettent
    d'effectuer certaines op‚rations :

&EN2 [o] : fermer la fenˆtre (= ESCAPE)
&EN2 [=] : accŠs au menu de la fenˆtre reprenant toutes les touches
	   fonctions d‚finies localement
&EN2 [?] : accŠs au manuel en ligne
&EN2 [o/] : rotation de la fenˆtre (variables)
&EN2 [flŠche] : maximiser et minimiser la taille de la fenˆtre

&EN les touches fonctions sont mises en exergue dans la
    zone de commentaire. La souris permet de "presser" ces touches sans
    avoir recours au clavier.

&EN la fenˆtre peut ˆtre d‚plac‚e en pla‡ant la souris sur son
    contour et en gardant le bouton appuy‚ pendant le d‚placement

&EN la taille de la fenˆtre peut ˆtre modifi‚e en pla‡ant la souris sur
    le coin inf‚rieur droit et en "tirant" tout en gardant enfonc‚ le
    bouton de la souris.

&EN les options du menu d‚roulant sont directement accessibles … l'aide
    de la souris, comme … l'aide des touches Alt-lettre. Attention, cela
    quitte le tableau en cours d'‚dition!

&EN dans les scrollbars (vertical ou horizontal) la souris permet de
    placer la fenˆtre … l'endroit souhait‚ par un simple clic.


&IT Cas particulier
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Si aucun objet n'est d‚fini dans le WS courant, le tableau est affich‚
mais ne contient ‚videmment pas d'objet. Il n'y a donc pas d'objet
courant, mais toutes les fonctions qui ne sont pas li‚es … l'objet
courant sont disponibles (move, create, help, etc).
>

<Comments>
&ALIAS commentaire
&ALIAS commentaires
   Commentaires
   ÄÄÄÄÄÄÄÄÄÄÄÄ

Les commentaires sont, avec les listes, les objets les plus simples : ce
sont simplement des textes libres. Ceux-ci sont ‚dit‚s … l'aide d'un
champ EDITOR, et peuvent donc ˆtre chacun un texte complet,
‚ventuellement de plusieurs pages.

L'‚dition se fait en trois ‚tapes:

&EN S‚lection des objets … ‚diter (voir plus haut)
&EN Edition d'un tableau d'objets (voir plus haut)
&EN Edition des objets individuels

Les deux premiŠres ‚tapes ont ‚t‚ d‚crites plus haut.

Lors de l'‚dition des objets (cr‚ation ou modification), l'‚cran de
saisie se pr‚sente comme suit :

.~a'pcmt.png'

&IT Nom
ÄÄÄ
Le nom de l'objet apparaŒt dans le premier champ de l'‚cran de saisie.
Si le nom d'un objet est modifi‚ alors que celui-ci existe, un nouveau
commentaire est cr‚‚ portant le nouveau nom. L'ancien commentaire existe
toujours, dans son ‚tat d'origine. Cette propri‚t‚ offre une m‚thode
trŠs simple pour copier des commentaires.

&IT Valeur
ÄÄÄÄÄÄ
Le texte du commentaire s'‚dite dans le champ EDITOR de l'‚cran de
saisie. Toutes les fonctions habituelles de ce type de champ sont
disponibles.

&IT Touches fonctions
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Pour sauver l'objet aprŠs encodage ou modification, il suffit de presser
F10. Si l'objet existe dans le WS courant, il faut confirmer le
remplacement de l'ancien objet par la nouvelle d‚finition.

Pour quitter sans sauver, il suffit de presser ESCAPE.
>

<Equations>
&ALIAS ‚quation
   Equations
   ÄÄÄÄÄÄÄÄÄ

Les ‚quations sont, avec les tableaux, les objets les plus complexes
g‚r‚s par le logiciel IODE. Si au d‚part il ne s'agit que d'expressions
LEC, on ne peut les dissocier des informations qui ont permis
l'estimation des coefficients qu'elles contiennent. Ce point du menu
permet non seulement la d‚finition des ‚quations, mais ‚galement
l'estimation des coefficients et l'affichage de graphiques.

Les ‚quations de IODE contiennent donc les informations suivantes :

&EN une ‚quation LEC

&EN un commentaire (ou titre)

&EN une m‚thode et une p‚riode d'estimation

&EN le bloc d'‚quations dont elle fait partie lors de l'estimation

&EN les ‚ventuels instruments (formes LEC) si la m‚thode d'estimation
    retenue en requiert.

Bien s–r, toutes les ‚quations ne n‚cessitent pas l'ensemble de ces
informations. Certaines ‚quations ne sont d'ailleurs que de simples
raccourcis d'‚criture dans les modŠles qui ne font intervenir aucun
coefficient et se ramŠnent donc … de simples formes LEC, accompagn‚es
‚ventuellement d'un commentaire.

&TI Nom des ‚quations
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Une contrainte importante sur le nom des ‚quations est … souligner : le
nom d'une ‚quation est toujours le nom de sa variable endogŠne. Ceci
implique entre autre qu'une seule ‚quation peut d‚finir une variable
endogŠne dans un WS d'‚quations.

Par cons‚quent, les noms des ‚quations r‚pondent aux mˆmes contraintes
que ceux des variables : 10 caractŠres alphanum‚riques maximum (20 … partir
de la version 6.01), commen‡ant par une lettre ou le caractŠre de
soulignement (_).

Cette fa‡on de nommer les ‚quations peut sembler par trop restrictive.
En fait, il s'agit surtout de faciliter la tƒche de l'utilisateur :
lorsqu'il s'agira plus tard de simuler un modŠle, la liste des ‚quations
permettra … elle seule de d‚terminer la liste des variables endogŠnes et
exogŠnes.

&TI Cr‚ation et ‚dition des ‚quations
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

La cr‚ation et l'‚dition des ‚quations se fait en trois ‚tapes:

&EN S‚lection des objets … ‚diter (voir plus haut)
&EN Edition d'un tableau d'objets (voir plus haut)
&EN Cr‚ation et ‚dition des ‚quations individuelles

Les deux premiŠres sont classiques et ont ‚t‚ d‚taill‚es en d‚but de
chapitre.

Pour cr‚er une nouvelle ‚quation, il suffit de presser la touche INS. Un
‚cran vide permet l'encodage des informations utiles. Une alternative
est d'‚diter une ‚quation existante proche de celle … cr‚er et de
modifier le nom de l'‚quation et sa d‚finition dans l'‚cran de saisie.
AprŠs sauvetage, une nouvelle ‚quation est construite.

L'‚cran de d‚finition d'une ‚quation doit au minimum contenir un nom et
une forme LEC.

Pour modifier une ‚quation, il faut d'abord se placer sur celle …
‚diter. Cela se fait … l'aide des touches fl‚ch‚es. En pressant la touche
ENTER, l'‚cran de d‚finition suivant apparaŒt :

.~a'peqs.png'

&IT Name
ÄÄÄÄ

Ce champ peut ˆtre modifi‚. Une nouvelle ‚quation est alors cr‚‚e au
sortir de l'‚dition avec le nouveau nom. Si une ‚quation portait d‚j… ce
nom, elle est remplac‚e.

&IT Estimation method
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Ce champ permet de d‚finir la m‚thode d'estimation … utiliser. Le choix
est offert entre 4 m‚thodes:

&EN LSQ simple
&EN Zellner
&EN Instrumental
&EN GLS

&IT From et To : p‚riode d'estimation
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
La p‚riode d'estimation ‚ventuelle doit tenir compte des lags et leads,
ainsi que des p‚riodes fix‚es dans l'‚quation : si le WS de variables
est d‚fini sur la p‚riode 1960Y1 … 1990Y1, une ‚quation contenant par
exemple "d(X)+Y[+1]" ne peut ˆtre estim‚e que sur la p‚riode
1961Y1 … 1989Y1.

&IT LEC
ÄÄÄ
La forme LEC de l'‚quation est ‚dit‚e … l'aide d'un champ EDITOR.
Toutes les fonctions d'‚dition sont utilisables et notamment la copie de
blocs, y compris en provenance d'autres ‚quations. Une ‚quation a
toujours la forme :

&CO
    lec_expression := lec_expression
&TX

Le nom de la variable endogŠne (et donc de l'‚quation) doit apparaŒtre
au moins une fois dans le texte de l'‚quation. Elle peut apparaŒtre
plusieurs fois et ne doit pas n‚cessairement se trouver dans le membre
de gauche.

La description complŠte du langage LEC - syntaxe, op‚rateurs et
fonctions - se trouve dans un chapitre s‚par‚.

&IT Comment
ÄÄÄÄÄÄÄ
Ce champ EDITOR permet d'encoder un commentaire libre concernant
l'‚quation, son estimation, etc.

&IT Block
ÄÄÄÄÄ
Ce champ permet de sp‚cifier le bloc d'‚quations qui sont … estimer
simultan‚ment. Toutes les m‚thodes d'estimation acceptent d'estimer
plusieurs ‚quations simultan‚es.

Les noms des ‚quations du bloc sont s‚par‚s par des blancs, virgules,
points-virgules ou retour … la ligne.

&IT Instrs
ÄÄÄÄÄÄ
Les instruments utilis‚s dans les m‚thodes d'estimations utilisant une
modification de m‚trique sur base d'instruments doivent ˆtre introduits
dans ce champ. Les instruments sont de simples formes LEC.
Ils sont s‚par‚s par points-virgules.

&TI Touches fonctions
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Une touche fonction est associ‚e … chaque bouton d‚fini dans l'‚cran :

&EN F10 : sauve l'‚quation et quitte son ‚dition

&EN ESCAPE : quitte l'‚quation sans sauver. Une confirmation est
    demand‚e pour ‚viter la perte du travail suite … une fausse
    manoeuvre.

&EN F3 : tests de Dicky-Fuller.

&EN F4 : affiche les valeurs des coefficients pr‚sents dans les
    ‚quations du bloc courant. Si ces coefficients n'existent pas dans
    le WS courant de coefficients, ceux-ci prennent comme valeur 0.9,
    avec 1.0 comme paramŠtre de relaxation.

&EN F5 : lance le processus d'estimation en prenant comme paramŠtres ceux
    affich‚s dans l'‚quation courante (p‚riode, m‚thode et instruments).

&EN F6 : affiche l'‚quation suivante du bloc, s'il y en a une.

&EN F7 : ajustements dynamiques automatiques. Deux m‚thodes sont disponibles :

&EN2 Partial Adjustment (1 coefficient c1):
&CO
	 lhs := rhs ->> d(lhs) := c1 * (rhs -(lhs)[-1])
&TX
&EN2 Error Correction Model (deux coefficients c1 et c2):
&CO
	 lhs := rhs ->> d(lhs) := c1 * d(rhs) + c2 * (rhs-lhs)[-1]
&TX

&EN F8 : affiche un menu permettant de visualiser et d'imprimer les
    r‚sultats de l'estimation : coefficients, matrice de corr‚lation,
    tests statistiques, graphiques des r‚sidus.

.~a'peqsres.png'

En pressant ENTER sur une des options, les valeurs correspondantes sont
affich‚es.


.~a'gest1.png'

&TI R‚sultats de l'estimation
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les r‚sultats de l'estimation (touche F8) ne sont disponibles que
si une estimation vient d'avoir lieu. Les r‚sultats disponibles sont:

&EN Coefficients : affiche dans un tableau les valeurs des coefficients
    estim‚s ainsi que les tests statistiques (standard error et t-stat).

&EN Correlation matrix : affiche dans un tableau la matrice de
    corr‚lation des coefficients.

&EN Results by equation : liste de r‚sultats statistiques associ‚s aux
    ‚quations :

&EN2 Correlation
&EN2 St dev of residuals
&EN2 Mean of YOBS
&EN2 Sum of Squares of Residuals
&EN2 Standard error
&EN2 Standard error in %
&EN2 F-stat
&EN2 R square
&EN2 R square adjusted
&EN2 Durbin-Watson
&EN2 Log Likelihood

&EN Graph of YOBS-YEST : affiche un graphique avec les courbes observ‚es et
    estim‚es. Dans le cas d'un bloc d'‚quations, l'‚cran est divis‚ en
    autant de partie qu'il y a d'‚quations.

&EN Graph of Residuals : affiche un graphique avec la courbe r‚sultant
    de la diff‚rence entre les observations et l'estimation. Dans le cas
    d'un bloc d'‚quations, il y a autant de graphiques que d'‚quations
    dans le bloc.

&EN Print graph : permet d'imprimer un des deux graphiques d‚finis
    ci-dessus. Le langage dans lequel les libell‚s
    doivent ˆtre indiqu‚s peut ˆtre s‚lectionn‚ au niveau de cette page.

&EN Print output : l'‚cran de saisie suivant permet de
    s‚lectionner les diff‚rents paramŠtres n‚cessaires:
    langage pour les titres automatiques, et s‚lection des
    informations optionnelles … imprimer.

~a'peqsout.gif'

Un exemple d'impression est affich‚ ci-dessous :

&CO
     ESTIMATION

     1. INPUT

     1.1 PARAMETERS

       - Estimation period : 1970Y1:1990Y1
       - Estimation method : LSQ
       - Number of observations : 21
       - Number of equations : 1
       - Number of coefficients : 2
       - Number of instruments : 0
       - Max number of iterations : 100
       - Convergence limit : 0.000010

     1.2 EQUATIONS

       - B := c1 + c2 * A

     2. RESULTS

     2.1 COEFFICIENTS AND TESTS

      COEF   VALUE   ST ERR    T-STAT    RELAX
     ------------------------------------------
      c1   1.883107 0.037492 50.226407 1.000000
      c2   0.053122 0.001795 29.601900 1.000000

     2.2 CORRELATION MATRIX OF COEFFICIENTS

	     c1        c2
     -----------------------
      c1  1.000000 -0.957095
      c2 -0.957095  1.000000

     2.3 CORRELATION OF RESIDUALS

	    B
     -----------
      B 1.000000

     2.4 RESULTS BY EQUATION

     2.4.1 EQUATION : B

       - B := c1 + c2 * A

				      VALUE
     ----------------------------------------
      Number of coefficients                2
      Number of observations               21
      Standard deviation on YOBS     0.333167
      Mean of YOBS                   2.945543
     ----------------------------------------
		  TESTS               VALUE
     ----------------------------------------
      Sum of square of residuals     0.047116
      Standard error                 0.049797
      Standard error in %            1.690597
      F-Stat                       876.245980
      R2                             0.978777
      R2 adjusted                    0.977660
      Durbin-Watson test             0.134944
      Log likelihood                34.248870
     ----------------------------------------

		ACTUAL AND FITTED VALUES
     ---------------------------------------------
      PERIOD   YOBS     YCALC     RES       RES%
     ---------------------------------------------
      1970Y1 2.302585 2.414325 -0.111740 -4.852798
      1971Y1 2.397895 2.467447 -0.069551 -2.900520
      1972Y1 2.484907 2.520569 -0.035662 -1.435142
      1973Y1 2.564949 2.573690 -0.008741 -0.340793
      1974Y1 2.639057 2.626812  0.012245  0.463996
      1975Y1 2.708050 2.679934  0.028116  1.038243
      1976Y1 2.772589 2.733056  0.039533  1.425844
      1977Y1 2.833213 2.786178  0.047036  1.660148
      1978Y1 2.890372 2.839300  0.051072  1.766976
      1979Y1 2.944439 2.892421  0.052017  1.766635
      1980Y1 2.995732 2.945543  0.050189  1.675351
      1981Y1 3.044523 2.998665  0.045857  1.506226
      1982Y1 3.091043 3.051787  0.039256  1.269978
      1983Y1 3.135494 3.104909  0.030585  0.975458
      1984Y1 3.178054 3.158031  0.020023  0.630047
      1985Y1 3.218876 3.211152  0.007723  0.239942
      1986Y1 3.258096 3.264274 -0.006178 -0.189615
      1987Y1 3.295837 3.317396 -0.021559 -0.654135
      1988Y1 3.332205 3.370518 -0.038313 -1.149791
      1989Y1 3.367296 3.423640 -0.056344 -1.673273
      1990Y1 3.401197 3.476762 -0.075564 -2.221694

&TX

&TI Mode de sauvetage
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Au cours de l'‚dition d'un bloc d'‚quations ou d'une ‚quation seule,
les valeurs introduites pour modifier les ‚quations ne sont pas sauv‚es
dans le WS d'‚quations. Le sauvetage n'a lieu qu'au moment de quitter
l'‚quation ou le bloc.

Il en va de mˆme pour les valeurs des coefficients qui ne sont sauv‚es
dans le WS courant de coefficients qu'en fin d'estimation aprŠs
confirmation du sauvetage par F10.

Ainsi, des coefficients qui n'existent pas au moment de l'entr‚e en
‚dition d'une ‚quation et qui sont cr‚‚s automatiquement par
l'estimation ne sont pas sauv‚s si l'‚quation ne l'est pas.

&TI Scalaires et Variables r‚sultant de l'estimation
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Les tests r‚sultant de la derniŠre estimation sont sauv‚s dans des
scalaires de fa‡on … permettre diff‚rents calcul par la suite. Il en va
de mˆme pour les r‚sidus, membres de gauche et de droite des ‚quations.

Les tests portent les noms suivants (e0_* pour la premiŠre ‚quation du
bloc, e1_* pour la deuxiŠme, ...) :

&EN ~ce0_n~C         : nombre de p‚riode du sample
&EN ~ce0_k~C         : nombre de coefficients estim‚s
&EN ~ce0_stdev~C     : std dev des r‚sidus
&EN ~ce0_meany~C     : moyenne de Y
&EN ~ce0_ssres~C     : somme du carr‚s des r‚sidus
&EN ~ce0_stderr~C    : std error
&EN ~ce0_stderrp~C   : std error %
&EN ~ce0_fstat~C     : F-Stat
&EN ~ce0_r2~C        : R carr‚
&EN ~ce0_r2adj~C     : R carr‚ ajust‚
&EN ~ce0_dw~C        : Durbin-Watson
&EN ~ce0_loglik~C    : Log Likelihood

Les s‚ries calcul‚es sont ‚galement sauv‚es dans une variable sous les
noms:

&EN  ~c_YCALC0~C pour le membre de droite de la premiŠre ‚quation du
	  bloc, ~c_YCALC1~C pour la deuxiŠme ‚quation, etc.

&EN  ~c_YOBS0~C pour le membre de gauche de la premiŠre ‚quation du
	  bloc, ~c_YOBS1~C pour la deuxiŠme ‚quation, etc.

&EN  ~c_YRES0~C pour les r‚sidus de la premiŠre ‚quation du bloc, etc.

En dehors du sample d'estimation, les valeurs de la s‚rie sont --.

>

<Identities>
   Identit‚s
   ÄÄÄÄÄÄÄÄÄ
&ALIAS identit‚
&ALIAS identit‚s

Les identit‚s sont des expressions LEC, formules utilis‚es pour
construire des s‚ries sur base d'autres variables. Le nom d'une identit‚
est celui de la s‚rie qui sera construite sur base de la formule
d‚finie.

Les identit‚s sont introduites … l'aide d'un champ EDITOR et sont donc
des textes s'‚tendant ‚ventuellement sur plusieurs lignes.

L'‚dition se fait en trois ‚tapes:

&EN S‚lection des objets … ‚diter (voir plus haut)
&EN Edition d'un tableau d'objets (voir plus haut)
&EN Edition des objets individuels

Les deux premiŠres sont standards et ont ‚t‚ d‚taill‚es en d‚but de
chapitre.

A noter que l'ex‚cution des identit‚s se fait via le point "Compute" de la
barre d'actions en s‚lectionnant l'option "Execute identities".

&TI Touches fonctions sp‚cifiques au identit‚s
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&EN F7 dans le tableau d‚roulant des identit‚s ex‚cute l'identit‚
courante sur toute la p‚riode du WS

&EN Shift-F7 lance la fonction ##IdtExecute

&TI Edition des identit‚s
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Lors d'une cr‚ation ou d'une modification des identit‚s, l'‚cran de saisie
est identique … celui des commentaires ou des listes et se pr‚sente
comme suit :

.~a'pidt.png'

&IT Nom
ÄÄÄ
Le nom de l'objet apparaŒt dans le premier champ de l'‚cran de
saisie. Si le nom d'un objet est modifi‚ alors que celui-ci existe, une
nouvelle identit‚ est cr‚‚e portant le nouveau nom. L'ancienne existe
toujours, dans son ‚tat d'origine. Cette propri‚t‚ permet par exemple de
copier des identit‚s sans recourir … la fonction sp‚cifique de copie
d'objet, en op‚rant ‚ventuellement une modification dans la d‚finition
de la nouvelle identit‚. Ainsi, si l'identit‚ X_B est d‚finie comme :

&CO
    X1_B + X2_B + X3_B
&TX
et que l'on veuille cr‚er une identit‚ X_F identique, mais o— les _B
sont remplac‚s par des _F, il suffit d'‚diter cette identit‚ X_B, et

&EN d'en changer le nom : X_F
&EN de remplacer les _B par des _F dans le texte :

&CO
    X1_F + X2_F + X3_F
&TX

En pressant F10, une nouvelle identit‚ est construite sur base de la
premiŠre.

&IT Valeur
ÄÄÄÄÄÄ
Le texte de l'identit‚ s'‚dite dans le champ EDITOR de l'‚cran de
saisie. Toutes les fonctions habituelles de ce type de champ sont
disponibles.

La description complŠte du langage LEC - syntaxe, op‚rateurs et
fonctions - se trouve dans un chapitre s‚par‚.


&IT Touches fonctions
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Pour sauver l'objet aprŠs encodage ou modification, il suffit de presser
F10. Si l'objet existe dans le WS courant, il faut confirmer le
remplacement de l'ancien objet par la nouvelle d‚finition.

En cas d'erreur de syntaxe dans la forme LEC, un message indique
l'erreur et le programme revient en ‚dition. Il est impossible de sauver
une identit‚ dont la forme LEC est syntaxiquement incorrecte.

Pour quitter sans sauver, il suffit de presser ESCAPE.
>

<Lists>
   Listes
   ÄÄÄÄÄÄ

&ALIAS listes
&ALIAS liste


.~a'mlist.png'

Les listes sont des textes libres, repr‚sentant en g‚n‚ral des suites de
noms d'objets. Elles peuvent cependant contenir n'importe quel texte.
Seul le contexte de l'utilisation des listes d‚cidera de leur validit‚.

Les listes sont ‚dit‚es … l'aide d'un champ EDITOR et sont donc
des textes s'‚tendant ‚ventuellement sur plusieurs lignes, voir
plusieurs pages. La limite de 32K, valable pour tous les objets de IODE,
est la seule contrainte.

L'‚dition se fait en trois ‚tapes:

&EN S‚lection des objets … ‚diter (voir plus haut)
&EN Edition d'un tableau d'objets (voir plus haut)
&EN Edition des objets individuels

D'autre part, une s‚rie d'op‚rations peuvent ˆtre r‚alis‚es pour
construire des listes:

&EN <Sort & Expand > : tri et extension des listes
&EN <List Calculus > : op‚rations sur les listes (intersection,
	diff‚rence, union).
&EN <File Contents > : cr‚e une liste avec le contenu du fichier
&EN <File Compare  > : comparaison entre un fichier et le WS
&EN <Search Text   > : recherche de texte dans les WS
&EN <Scan objects  > : recherche des objets r‚f‚renc‚s dans d'autres

Les deux premiŠres ont ‚t‚ d‚taill‚es en d‚but de chapitre.

Lors d'une cr‚ation ou d'une modification de liste, l'‚cran de saisie
se pr‚sente comme suit :

.~a'plist.png'

&IT Nom
ÄÄÄ
Le nom de l'objet apparaŒt dans le premier champ de l'‚cran de
saisie. Si le nom d'un objet est modifi‚ alors que celui-ci existe, une
nouvelle liste est cr‚‚e portant le nouveau nom. L'ancienne
existe toujours, dans son ‚tat d'origine. Cette propri‚t‚
permet par exemple de copier des listes sans recourir … la fonction
sp‚cifique de copie d'objet, en op‚rant ‚ventuellement une modification
dans la d‚finition de la nouvelle liste.

&IT Valeur
ÄÄÄÄÄÄ
Le texte de la liste s'‚dite dans le champ EDITOR de l'‚cran de
saisie. Toutes les fonctions habituelles de ce type de champ sont
disponibles.

&IT Touches fonctions
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Pour sauver l'objet aprŠs encodage ou modification, il suffit de presser
F10. Si l'objet existe dans le WS courant, il faut confirmer le
remplacement de l'ancien objet par la nouvelle d‚finition.

Pour quitter sans sauver, il suffit de presser ESCAPE.
>

<Sort & Expand>
    Sort & Expand
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ
.~a'plsort.png'

Cette fonction permet d'effectuer un tri alphanum‚rique sur le contenu
d'une liste.

Les deux champs … remplir dans la page de saisie permettent de pr‚ciser
le nom de la liste … trier et le nom de la liste r‚sultat.

Si la liste r‚sultat existe d‚j…, elle est remplac‚e par la nouvelle
valeur. Le tri s'effectue de la fa‡on suivante : la liste est d‚coup‚e
en "mots" aux positions des s‚parateurs (espace, virgules et sauts de
lignes). Ces mots sont tri‚s par ordre alphab‚tique et une liste est
reconstruite en s‚parant les mots par des virgules. Les lignes de plus
de soixante caractŠres sont d‚coup‚es.
>

<List Calculus >
    List Calculus
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette fonction permet de r‚aliser des op‚rations simples sur des listes.

.~a'plistc.png'

&EN intersection : calcul des ‚l‚ments communs … deux listes

&EN diff‚rence : calcul des ‚l‚ments d'une liste non pr‚sents dans une
    autre

&EN union : cr‚ation d'un nouvelle liste reprenant de fa‡on unique
    tous les ‚l‚ments de deux listes

Quatre champs doivent ˆtre introduits pour pr‚ciser :

&EN le nom de la liste r‚sultat
&EN le nom de la premiŠre op‚rande
&EN l'op‚rateur … utiliser :
&EN2 + pour l'union
&EN2 - pour la diff‚rence
&EN2 * pour l'intersection
&EN la seconde op‚rande

Le r‚sultat du calcul est plac‚ dans la liste dont le nom est fix‚ par
le premier champ de l'‚cran. Ce r‚sultat ainsi que le nombre d'‚l‚ments
qu'il contient est ‚galement repr‚sent‚ dans la seconde partie de l'‚cran.

>

<File Contents >
    File Contents
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette fonction permet de cr‚er une liste avec les noms d'objets du WS
actif ou d'un fichier qui correspondent … un critŠre donn‚.

.~a'pfcont.png'

&IT Search
ÄÄÄÄÄÄÄÄÄÄ
Indique le critŠre sur le nom des objets … retenir dans la liste. Ce critŠre
peut contenir les caractŠres ~c*~C ou ~c?~C. Pour obtenir toutes les s‚ries
commen‡ant par ~cI~C, on introduira :

&CO
    Search : I*
&TX

On pr‚cisera dans le champ suivant le type d'objet … rechercher.

&IT File
ÄÄÄÄÄÄÄÄ
Indique le nom du fichier o— la recherche doit ˆtre effectu‚e. Si ce nom
est laiss‚ blanc, la recherche a lieu dans le WS actif.

&IT Save result in ...
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Indique le nom de la liste qui doit recevoir le r‚sultat de la recherche.

&IT R‚sultat
ÄÄÄÄÄÄÄÄÄÄÄÄ

La deuxiŠme partie de l'‚cran contient le nombre d'objets trouv‚s (et sauv‚s
dans la liste r‚sultat), ainsi que les premiers ‚l‚ments de cette liste.

>

<File Compare  >
    File Compare
    ÄÄÄÄÄÄÄÄÄÄÄÄ
Le contenu du WS courant peut ˆtre compar‚ … celui d'un fichier. Le
r‚sultat de cette comparaison est compos‚ de 4 listes :

&EN objets trouv‚s dans le WS seulement
&EN objets trouv‚s dans le fichier seulement
&EN objets trouv‚s dans les deux, avec la mˆme d‚finition
&EN objets trouv‚s dans les deux, avec une d‚finition diff‚rente

Pour sp‚cifier les paramŠtres de la requˆte, l'‚cran
contient les champs suivants:

.~a'pfcomp.png'

&IT Compare
ÄÄÄÄÄÄÄ
Sp‚cifie le type d'objet … comparer.

&IT File
ÄÄÄÄ
Ce champ est ici requis. Il indique le fichier dont le contenu doit ˆtre compar‚
… celui du WS actif.

&IT Save results in lists
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Les quatre champs qui suivent contiendront le nom des listes destin‚es
… recevoir les informations calcul‚es.

A droite, on trouvera, aprŠs le calcul, le nombre d'‚l‚ments de chaque
liste.

&TI Fonction de rapports
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
La mˆme fonction peut ˆte utilis‚e dans un rapport :
&CO
    $DataCompareXxx file ONE TWO THREE FOR

	  ONE         in WS only
	  TWO         in file only
	  THREE       in both, equal
	  FOR         in both, different
&TX
>
<Search text>
   Search text
   ÄÄÄÄÄÄÄÄÄÄÄ

Il est souvent utile de savoir dans quels objets intervient un texte,
comme un nom de s‚rie ou de scalaire par exemple. Cette fonction permet
de rechercher dans un des WS courants la liste des objets contenant un
string donn‚.

Une application imm‚diate est de rechercher toutes les ‚quations ou tous
les tableaux qui contiennent dans leur d‚finition un nom de variable.

Il est ‚galement possible de g‚n‚rer automatiquement la liste de toutes
les variables dont le nom r‚pond … certaines caract‚ristiques.

Le r‚sultat de cette recherche est une liste d'objets qui peut au besoin
ˆtre sauv‚e dans le WS courant de listes. Par d‚faut la liste
"_RES" contient le r‚sultat de la derniŠre recherche.

L'‚cran se pr‚sente comme suit:

.~a'psearch.png'

&IT Text
ÄÄÄÄ
Le texte … rechercher peut contenir des caractŠres sp‚ciaux qui
permettent de sp‚cifier les limites de la recherche :

&EN * : n'importe quelle suite de caractŠres (mˆme vide)
&EN ? : un et un seul caractŠre (quelconque)
&EN @ : n'importe quel caractŠre alphanum‚rique
&EN ampersand : n'importe quel caractŠre non alphanum‚rique
&EN | : n'importe quel caractŠre alphanum‚rique ou aucun en d‚but et fin
    de string
&EN ! : n'importe quel caractŠre non alphanum‚rique ou aucun en d‚but et
    fin de string
&EN \ plac‚ devant un des caractŠres sp‚ciaux supprime son interpr‚tation

Ainsi la recherche de

&CO
    A*_B
&TX

aboutira … la s‚lection des ‚quations

&CO
    ln X := c1 + c2 * l A12_BC
			|**||
    A_B  := X + Y
    |||
    Z    := c1 + a * A + c3 * C_B
		     |*********||

&TX

mais pas de

&CO
    ln AB := c1 + c2 * X
&TX

&IT Mot entier/partiel (Whole word)
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Pr‚ciser Yes si la chaŒne … rechercher doit ˆtre un mot entier et non
une partie de mot. Indiquer No si cela n'a pas d'importance.

&IT Majuscule/minuscule (Exact case)
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Pr‚ciser si la recherche doit diff‚rencier majuscules et minuscules
dans la chaŒne … rechercher.

&IT Type d'objet (Object to scan)
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Le type d'objet doit ˆtre indiqu‚ dans ce champ. En pressant TAB, la
liste des types apparaŒt. Il suffit de se placer sur le type choisi et
de presser ENTER.

Les recherches sont diff‚rentes en fonction du type d'objet :

&EN Commentaires : le nom et le texte du commentaire sont analys‚s
&EN Equations    : le nom et la forme LEC de l'‚quation sont analys‚s
&EN Identit‚s    : le nom et la forme LEC de l'identit‚ sont analys‚s
&EN Listes       : le nom et le texte de la liste sont analys‚s
&EN Scalaires    : le nom du scalaire est analys‚
&EN Tableaux     : le nom, les titres et les formes LEC du tableau sont
		   analys‚s
&EN Variables    : le nom de la variable est analys‚

&IT Recherche dans ... (Search in ...)
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Il est possible de restreindre la recherche aux noms d'objets, aux
formules ou au texte libre, ou … une combinaison de plusieurs ‚l‚ments.
Il suffit pour chaque cas de pr‚ciser s'il faut ou non l'analyser.

&IT Liste r‚sultat
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
La liste des objets r‚pondant au critŠre peut ˆtre sauv‚e dans le WS de
listes courant. Le nom de cette liste doit ˆtre indiqu‚ et ˆtre un nom
de liste valide. Si une liste de mˆme nom existe, elle sera remplac‚e
par le r‚sultat du calcul. Le nom "_RES" est propos‚ par d‚faut.

&IT R‚sultat
ÄÄÄÄÄÄÄÄ
Dans le bas de l'‚cran, un champ reprend les premiers objets
satisfaisant au critŠre de recherche.

&IT Exemples
ÄÄÄÄÄÄÄÄÄÄÄÄ

Pour construire la liste de toutes les variables commen‡ant par B_ :

&CO
    Text            : B_*
    Whole word      : No
    Exact case      : Yes
    Objects to scan : Variables
    Search in names : Yes
	   formulas : No
	  Free text : No
&TX

Pour construire la liste de tous les tableaux contenant la variable XXX :

&CO
    Text            : XXX
    Whole word      : Yes
    Exact case      : Yes
    Objects to scan : Tables
    Search in names : No
	   formulas : Yes
	  Free text : No
&TX

Pour construire la liste de tous les commentaires contenant le mot
"export" ou "exportation" :

&CO
    Text            : export
    Whole word      : No
    Exact case      : No
    Objects to scan : Tables
    Search in names : No
	   formulas : No
	  Free text : Yes
&TX

>

<Scan objects     >
    Scan objects
    ÄÄÄÄÄÄÄÄÄÄÄÄ
.~a'pscan.png'

Cette fonction construit deux listes … partir des formules LEC contenues
dans des objets d'un type donn‚.

Elle permet par exemple de construire la liste des variables qui
apparaissent dans des tableaux donn‚s, pour construire … partir de l… un
fichier ne contenant que les variables utiles au calcul de ces tableaux.

Elle fournit en sortie deux listes :

&EN _EXO : liste des noms de variables
&EN _SCAL : liste des noms de scalaires

Le premier champ fournit le type d'objets … scanner. Seuls les tableaux,
identit‚s et ‚quations sont accept‚s, les autres objets ne contenant pas
de formule LEC.

Le second champ (de type EDITOR) permet de limiter le nombre d'objets …
scanner.

Par exemple, pour trouver toutes les variables apparaissant dans les ‚quations
de $maineq, on introduira :

&CO
    Object type to scan : Equations
    List to scan        : $maineq
&TX


Le r‚sultat sera stoqu‚ dans la liste _EXO du workspace courant :


.~a'pscanres.png'

Autre exemple pour rechercher les variables apparaissant dans des tableaux:

&CO
    Object type to scan : Tables
    List to scan        : TAB1, TAB2
&TX

Le r‚sultat remplacera le contenu de la liste _EXO.

>

<Scalars>
   Scalaires
   ÄÄÄÄÄÄÄÄÄ
&ALIAS scalaire
&ALIAS scalaires


.~a'sscal.png'

Les scalaires sont des valeurs utilis‚es de plusieurs fa‡ons :

&EN comme coefficient … estimer dans les ‚quations
&EN comme r‚sultat secondaire d'estimation (tests statistiques, nombre d'it‚rations...)
&EN comme constantes dans les formules

Dans le cas des coefficients … estimer, outre la valeur calcul‚e ou
fix‚e du coefficient, trois autres valeurs pertinentes sont retenues
dans l'objet scalaire : le paramŠtre de relaxation (permettant de
"bloquer" un coefficient), le test en t et la d‚viation standard du
coefficient. Ces deux derniŠres valeurs sont conserv‚es uniquement …
titre de documentation.

L'‚dition se fait en trois ‚tapes:

&EN S‚lection des objets … ‚diter
&EN Edition d'un tableau d'objets
&EN Edition des objets individuels

Les deux premiŠres ont ‚t‚ d‚taill‚es en d‚but de chapitre.


L'‚dition proprement dite des scalaires se fait en d‚pla‡ant un
"curseur" dans le tableau vers la valeur … modifier, en pressant ENTER
et en introduisant la nouvelle valeur dans la fenˆtre affich‚e en
avant-plan (la souris permet le mˆme traitement : il suffit de se placer
sur la cellule … ‚diter et de cliquer 2 fois). Seules sont modifiables
la valeur du scalaire lui-mˆme et le paramŠtre de relaxation de
l'estimation.

&IT Cr‚ation
ÄÄÄÄÄÄÄÄ
La cr‚ation se fait en pressant INS. Une fenˆtre permet alors
d'introduire le nom du scalaire. En pressant F10, le nouveau scalaire
est ajout‚ au WS avec pour valeur 0.9, comme paramŠtre de relaxation 1.0
et -- (Not Available) pour les tests statistiques.

Le nom d'un scalaire est toujours en minuscules pour le distinguer dans
une forme LEC des noms de variables temporelles.

&IT Edition
ÄÄÄÄÄÄÄ
Pour modifier la valeur d'un scalaire, il faut d'abord se placer sur
celui … modifier … l'aide des touches fl‚ch‚es. Lorsque la case souhait‚e
est atteinte, la touche ENTER ouvre une fenˆtre dans laquelle la valeur
est affich‚e et peut ˆtre modifi‚e.

F10 ou ENTER ou une touche fl‚ch‚e (haut, bas, gauche) permet de sauver la
valeur modifi‚e et de se placer directement en ‚dition de la case suivante
(selon la touche tap‚e).
>

<Tables>
   Tables
   ÄÄÄÄÄÄ
&ALIAS tableaux
&ALIAS tableau


.~a'stables.png'

Les tableaux sont des objets destin‚s … pr‚senter des variables et des
scalaires - r‚sultats ou donn‚es - sous forme de tableaux de chiffres ou
de graphiques. Ils sont d‚finis comme un ensemble de lignes de
diff‚rents types (titre, formule, lignes intercalaires, etc). Ces lignes
contiennent des formules qui seront calcul‚es lors de l'impression sur
base des valeurs des s‚ries, par exemple aprŠs une simulation. Les
tableaux ne contiennent donc pas de nombres, mais bien les formules qui
permettront de calculer les valeurs … imprimer dans les tableaux.

L'‚dition et la cr‚ation des tableaux se fait en trois ‚tapes:

&EN S‚lection des objets … ‚diter (voir plus haut)
&EN Edition d'un tableau d'objets (voir plus haut)
&EN Cr‚ation et ‚dition des tableaux individuels

Les deux premiŠres ‚tapes sont standards et ont ‚t‚ d‚taill‚es en d‚but
de chapitre.

Avant de passer … la description de la phase d'‚dition des tableaux, il
est indispensable de bien comprendre le m‚canisme de fonctionnement des
tableaux ainsi que ce qu'ils peuvent contenir.


&TI D‚finition d'un tableau
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Le tableau se pr‚sente comme un ensemble de lignes et de colonnes.
Chaque ligne possŠde un ensemble d'attributs, dont le plus important est
son type. Ce type conditionne la maniŠre dont la ligne sera imprim‚e et
le contenu possible de la d‚finition de la ligne. Les autres attributs
concernent le type de caractŠres … utiliser lors de l'impression et le
cadrage de la ligne. Le tableau contient en outre des attributs graphiques
qui concernent le layout du graphique qui sera ‚ventuellement construit
avec ce tableau.

Les types de lignes suivants sont d‚finis :

&EN ~bTITLE~B : il s'agit d'une ligne de titre. Le texte encod‚ dans la
    premiŠre colonne s'‚tendra lors de l'impression … travers toute la
    largeur du tableau. Une forme LEC dans ce type de ligne n'a pas de
    sens et est rejet‚e par l'‚diteur.

&EN ~bCELL~B : il s'agit d'une ligne "normale" du tableau : la premiŠre
    colonne contient un titre de ligne, la seconde une forme LEC qui,
    une fois calcul‚e pour les p‚riodes demand‚es, fournira les valeurs
    des colonnes 2 et suivantes du tableau. En fait, les deux colonnes
    peuvent ˆtre indiff‚remment du texte ou une forme LEC, mais
    l'int‚rˆt de placer une forme LEC dans la premiŠre colonne n'est pas
    ‚vident. Par contre, on peut placer deux titres dans les deux
    colonnes, ou … tout le moins laisser vide la seconde colonne pour
    marquer une s‚paration dans le tableau.

&EN ~bLINE~B : ce type de ligne ne peut contenir ni texte, ni forme LEC : il
    ne s'agit que d'une ligne de s‚paration au milieu du tableau

&EN ~bFILES~B : cette ligne contiendra, lors de l'impression, le nom des
    fichiers contenant les donn‚es imprim‚es s'‚tendant … travers toute
    la largeur du tableau. Si plusieurs fichiers sont imprim‚s en
    comparaison, cette ligne est automatiquement multipli‚e. Aucune
    donn‚e (texte ou LEC) ne peut ˆtre encod‚e dans les colonnes de ce
    type de ligne.

&EN ~bMODE~B : il s'agit d'une ligne qui d‚pend des op‚rations sur les
    p‚riodes effectu‚es lors de l'impression du tableau. Par exemple, si
    une colonne est imprim‚e en taux de croissance, un report sera
    indiqu‚ dans la ligne correspondant … MODE. Aucune donn‚e ne peut
    ˆtre encod‚e dans les colonnes de ce type de ligne.

&EN ~bDATE~B : une ligne de ce type contiendra la date d'impression du tableau.

&IT Exemple
ÄÄÄÄÄÄÄ

Le tableau suivant est le r‚sultat d'un calcul bas‚ sur 2 s‚ries A et B.

&CO

			     Titre du tableau
--------------------------------------------------------------------
		1970     1971     1972     1973     1974     1975

 S‚rie A         0.00     1.00     2.00     3.00     4.00     5.00
 S‚rie B        70.00    71.00    72.00    73.00    74.00    75.00
 S‚rie A + B    70.00    72.00    74.00    76.00    78.00    80.00
--------------------------------------------------------------------
 [1] : ws.var
 03/05/92

&TX
L'objet tableau ayant produit ce tableau est d‚fini comme suit :

&CO
 ---------------------------------------------------
 Type de ligne  |   Colonne 1         | Colonne 2
 ---------------|---------------------|-------------
 [TITLE]        |  "Titre du tableau" |
 [LINE]         |                     |
 [CELL]         |                     |  "##t"
 [TITLE]        |                     |
 [CELL]         |  "S‚rie A"          |  A
 [CELL]         |  "S‚rie B"          |  B
 [CELL]         |  "S‚rie A + B"      |  A+B
 [LINE]         |                     |
 [MODE]         |                     |
 [FILES]        |                     |
 [DATE]         |                     |
 ---------------------------------------------------
&TX



&TI Cr‚ation d'un tableau
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Pour cr‚er un nouveau tableau, il suffit de presser la touche INS. Une
fenˆtre permet de d‚finir rapidement un nouveau tableau:

.~a'ptblcr.png'

Le premier champ permet d'encoder le nom du tableau … cr‚er.

Le second champ d‚finit le nombre de "colonnes" du tableau. Par d‚faut, le
nombre de colonnes vaut 2, ce qui signifie qu'il y a une colonne de titres
(la premiŠre) et une colonne de formules (la seconde). Cette derniŠre
colonne sera r‚p‚t‚e pour toutes les p‚riodes … calculer lors de
l'exploitation du tableau par le programme d'impression ou de graphique.

Le troisiŠme champ est du type EDITOR. Il permet de construire un tableau de
fa‡on automatique. On introduit dans ce champ les formes LEC qui
constitueront les lignes du tableau. Ces formes LEC sont obligatoirement
s‚par‚es par un point-virgule (p. ex. X; Y; X+Y) ou par un saut de ligne. La
formule LEC sera recopi‚e dans les colonnes du tableau, sous forme de titre
dans la premiŠre colonne et sous forme de LEC dans les colonnes suivantes.

Dans le bas de la page de configuration du tableau, on donne le titre
g‚n‚ral du tableau (plac‚ par d‚faut en tˆte de tableau) et on pr‚cise si
des lignes de type MODE, FILES et DATE devront figurer en footnote.

Une fois le tableau cr‚‚ (F10), on entre en ‚dition d'un tableau. Le
nouveau tableau n'est pas vide : il contient d‚j… un certain nombre de
lignes standards ainsi que les informations de type automatique d‚finies
ci-dessus.


&TI Edition d'un tableau
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
.~a'ptbl.png'

Pour ‚diter un tableau existant, il suffit de se placer sur le tableau
souhait‚ et de presser la touche ENTER. On se retrouve par ailleurs
directement en ‚dition lors de la cr‚ation d'un nouveau tableau.

Une fois le tableau affich‚ … l'‚cran, on peut s'y d‚placer … l'aide des
touches fl‚ch‚es (Up, Down, PgUp, PgDn), de fa‡on … choisir la ligne et
la colonne … modifier ou la position dans le tableau o— on veut op‚rer
une transformation. Ensuite, en pressant une touche fonction, on op‚rera
la modification souhait‚e.

&IT D‚finition des cellules
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Les cellules des lignes de type TITLE et CELL peuvent contenir des
textes. Les cellules des lignes de type CELL peuvent de surcroŒt
contenir des formes LEC. Les cellules des autres types de lignes ne
contiennent aucune information, car aucune impression n'en r‚sulterait.

Pour distinguer une cellule texte d'une cellule LEC, la cellule de texte
devra commencer par les doubles guillemets (qui ne seront pas imprim‚s).
Une cellule ne d‚butant pas par un double guillemet sera consid‚r‚e
comme une forme LEC et syntaxiquement v‚rifi‚e.

&IT Interp‚tation des cellules de texte
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les cellules de texte d'un tableau peuvent contenir des ‚l‚ments
variables qui s'adaptent en fonction du sample lors de l'impression. Ces
variables sont compos‚es du caractŠre ##, suivi d'une lettre et
optionnellement d'un chiffre.

La lettre indique la valeur … reprendre : 'y' pour l'ann‚e par exemple.

Le chiffre indique la p‚riode … prendre en compte. En effet, plusieurs
p‚riodes peuvent intervenir dans le calcul de la colonne (80/79 par
exemple). 80 correspondra … la p‚riode 1, et 79 … la p‚riode 2. Le
chiffre peut donc prendre les valeurs 1 et 2.

Pour certaines variables, le chiffre n'a pas de sens : ##O indique
l'op‚ration effectu‚e sur les ann‚es (taux de croissance par exemple) et
ne doit pas ˆtre suivie de 1 ou de 2.

Si une des variables n'est pas suivie d'un chiffre, celui-ci vaut 1.

Les ‚l‚ments constitutifs d‚finis sont les suivants :

&CO
     Syntaxe     Signification               Exemple
    -------------------------------------------------
    ##y1 ou ##y2   ann‚e de la colonne           80
    ##Y1 ou ##Y2   ann‚e de la colonne           1980
    ##p           p‚riodicit‚                   q
    ##P           p‚riodicit‚                   Q
    ##M1 ou ##M2   sous-p‚riode     Mensuel      F‚vrier
				  Trimestriel  IV
				  Annuel       vide
				  Autre        52
    ##m1 ou ##m2   sous-p‚riode     Mensuel      F‚v
				  Trimestriel  iv
				  Annuel       vide
				  Autre        52
    ##r1 ou ##r2   sous-p‚riode     Mensuel      ix
				  Trimestriel  iv
				  Annuel       vide
				  Autre        52
    ##R1 ou ##R2   sous-p‚riode     Mensuel      IX
				  Trimestriel  IV
				  Annuel       vide
				  Autre        52
    ##n1 ou ##n2   sous-p‚riode                  7
    ##N1 ou ##N2   sous-p‚riode                  11
    ##o           op‚ration sur les p‚riodes    /
    ##O           op‚ration sur les p‚riodes    Growth Rates
    ##F ou ##F     fichiers utilis‚s             [1-2]
&TX

    Pour faciliter l'‚criture, les macros suivantes sont d‚finies :
&CO
    ##t ‚quivaut … ##y1##P1##n1##o1##y2##P2##n2
    ##T     "      ##Y1##P1##n1##o1##Y2##P2##n2
    ##S     "      ##T##F
    ##s     "      ##t##f
&TX

Selon le sample utilis‚, certaines variables sont vides ou non : s'il
n'y a pas d'op‚ration sur les p‚riodes (taux de croissance), toutes les
variables suivies de 2 (##Y2) restent vides, de mˆme que l'op‚ration ##O
et ##o.

Si un seul fichier intervient dans le calcul du tableau, ##f et ##F
restent vides.

Si la p‚riodicit‚ est annuelle, les variables ##P, ##N, ##M, ##R restent
vides, qu'elles soient en majuscules ou minuscules, suivies de 1 ou de
2.

Notez que plusieurs cellules d'un mˆme tableau peuvent contenir ces
‚l‚ments variables et que ceux-ci ne doivent pas ˆtre tous les mˆmes :

&CO
    ligne 1 : "##Y1" l
    igne 2 : "(##O)"
&TX

Donne comme r‚sultat pour le sample 90/89:4
&CO
	1990          1991          1992          1993
    (Diff‚rences) (Diff‚rences) (Diff‚rences) (Diff‚rences)
&TX


&IT Touches fonctions
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&EN ENTER : ouvre une fenˆtre avec le contenu actuel de la cellule sur
    laquelle se trouve le curseur. On peut modifier le contenu de cette
    cellule. Cependant, toutes ne sont pas ‚ditables : les lignes de
    type LINE, DATE, MODE, FILES ne peuvent contenir d'informations dans
    leurs cellules.

&EN INS : ouvre une petite fenˆtre permettant d'indiquer le type de la
    ligne … ins‚rer et la position relative de la cellule (avant ou
    aprŠs la ligne courante). F10 ajoute la ligne dans le tableau.

&EN DEL : d‚truit la ligne courante.

&EN F2 : augmente d'un caractŠre la largeur visible de la premiŠre colonne.

&EN s+F2 : diminue d'un caractŠre la largeur visible de la premiŠre colonne.

&EN F3 : augmente d'un caractŠre la largeur visible des formules.

&EN s+F3 : diminue d'un caractŠre la largeur visible des formules.

&EN F4 : permet de fixer les attributs graphiques du tableau et de la
    ligne courante (voir ci-dessous).

&EN F5 : permet de modifier le cadrage de la cellule courante:
    Center, Left, ou Right.

&EN F6 : permet de modifier les attributs typographiques de la cellule
    courante: Normal, Bold, Italic, ou Bold+Italic

&EN F7 : visualisation test du tableau en cours de d‚finition sur les
    premiŠres p‚riodes. Pour rendre cette op‚ration r‚alisable, il est
    clair que tous les ‚l‚ments apparaissant dans les formules du
    tableau (variables, scalaires) doivent ˆtre pr‚sent en workspace. En
    cours de visualisation du tableau, la touche F4 donne la liste des
    fichiers auxquels il est fait r‚f‚rence.

&EN F8 : visualisation test du tableau sous forme graphique. Pour rendre
    cette op‚ration r‚alisable, il est clair que tous les ‚l‚ments
    apparaissant dans les formules du tableau (variables, scalaires)
    doivent ˆtre pr‚sents en workspace.

&EN F10 : sauve le tableau et quitte son ‚dition. Si le tableau
    existait auparavant, la fonction demande confirmation du
    remplacement de l'ancien tableau.

&EN ESCAPE : quitte le tableau sans sauver. Un confirmation est demand‚e
    pour ‚viter la perte du travail suite … une fausse manoeuvre.

La souris peut ˆtre utilis‚e pour faciliter la plupart des op‚rations de
d‚finition du tableau : s‚lection d'une cellule, copie/collage de
parties de texte ou de formules, d‚placement de la fenˆtre, etc.

De plus, des touches fonctions semblables … celles de l'‚diteur MMT
sont ‚galement exploitables en ‚dition des tableaux. L'‚diteur de
tableaux permet de marquer et copier des blocs de lignes, y compris
entre diff‚rents tableaux.

&EN Ctrl-L : marque la ligne courante ou ‚tend la s‚lection
		 jusqu'… celle-ci

&EN Ctrl-U : supprime les marques

&EN Ctrl-C : copie les marques … l'endroit du curseur

&EN Ctrl-D : d‚truit les marques et place les lignes d‚truites
		 dans le buffer.

&EN Ctrl-K : d‚truit la ligne courante sans demande de confirmation

&EN Ctrl-Y : place les marques dans un buffer pour une copie
		 dans un autre tableau. Ctrl-C et Ctrl-D place
		 ‚galement les lignes marqu‚es dans le buffer. Le
		 contenudu buffer est remplac‚ … chaque fois.

&EN Ctrl-P : copie le buffer … l'endroit du curseur

&IT Attributs graphiques
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
La touche F4 permet de fixer les attributs graphiques du tableau et de
la ligne courante. La page suivante est affich‚e :

.~a'ptblgr.png'

&EN ~bCurrent line~B type offre le choix entre 4 types de courbes pour la ligne
    courante du tableau : Line Chart, Scatter Chart (suite de symboles),
    Bar Chart et Scatter Line (ligne enrichie de symboles).

&EN ~bLeft ou Right~B indique si l'axe des ordonn‚es doit ˆtre … gauche
    ou … droite du graphique.

&EN ~bYMin et Ymax~B permettent de fixer le minimum et le maximum sur l'axe
    des Y. Si les donn‚es sortent de ces valeurs, l'‚chelle de l'axe
    s'adapte aux donn‚es. La valeur -- peut ˆtre fix‚e pour YMin et/ou
    YMax : dans ce cas, le programme graphique calculera une valeur
    d'‚chelle optimale.

&EN ~bY-Grids et X-Grids~B offrent le choix entre trois options de
    quadrillage du graphique en Y et en X : Major qui trace une ligne
    … travers tout le graphique … chaque graduation principale de
    l'axe, Minor qui en trace une … toutes les graduations et None qui
    supprime le quadrillage du graphique.

&EN ~bAxis~B permet de choisir le type des axes : Std Axis (en niveau), Log
    Axis (‚chelle logarithmique), Semi-Log Axis (‚chelle
    semi-logarithmique), Percent Axis (‚chelle en Y en pourcentage de 0
    … 100). Cette option n'est pas impl‚ment‚e.

&EN ~bBox~B indique si le graphique doit ˆtre encadr‚

&EN ~bShadow~B permet de donner un effet d'ombrage au graphique.

>

<Variables>
&ALIAS variable
   Variables
   ÄÄÄÄÄÄÄÄÄ

.~a'svar.png'

Les variables sont des s‚ries de nombres d‚finies sur une p‚riode de
temps d‚termin‚e. Ce sont donc simplement des suites de valeurs qui
peuvent ˆtre visualis‚es et modifi‚es sous la forme d'un grand tableau
de nombres.

L'‚dition se fait en trois ‚tapes:

&EN S‚lection des objets … ‚diter (voir plus haut)
&EN Edition d'un tableau d'objets (voir plus haut)
&EN Edition des valeurs individuelles

Les deux premiŠres ‚tapes ont ‚t‚ d‚taill‚es en d‚but de chapitre.

Il est ‚galement possible d'‚diter des s‚ries en les pr‚sentant sous
la forme de tableaux d‚finis par ailleurs: c'est l'objet de l'option
<Edit Tables>.

L'‚dition proprement dite des s‚ries se fait en d‚pla‡ant un "curseur"
dans le tableau vers la valeur … modifier, en pressant ENTER et en
introduisant la nouvelle valeur dans la fenˆtre affich‚e en avant-plan.

&IT Cr‚ation
ÄÄÄÄÄÄÄÄ
.~a'pvar.png'

La cr‚ation d'une nouvelle variable se fait en pressant INS. Une fenˆtre
permet alors d'introduire le nom de la variable ainsi qu'une expression
en LEC indiquant les valeurs initiales … donner … la variable.


La formule LEC peut contenir n'importe quelle expression, mais les
variables et scalaires n‚cessaires … son calcul doivent imp‚rativement
ˆtre pr‚sents en workspace pour que le calcul soit possible.

Une forme LEC constante comme "ln(3)" est ‚galement valable. En cas
d'erreur de syntaxe, la cr‚ation n'a pas lieu et le programme revient en
‚dition de la formule afin de permettre d'y apporter les corrections
n‚cessaires.

Par exemple :

&CO
   1970 + t
   ln(X)
   c1 * c2 + Y
   2
&TX

En pressant F10, la nouvelle variable est ajout‚e au WS avec pour
valeurs celles r‚sultant du calcul de la formule LEC ‚ventuelle. En
l'absence de forme LEC, la valeur NA est donn‚e … tous les ‚l‚ments de
la s‚rie de nombres.


&NO La formule LEC n'est pas conserv‚e dans la d‚finition de la variable :
    aprŠs son calcul, elle est perdue.

&IT Edition
ÄÄÄÄÄÄÄ
Pour modifier la valeur d'une variable … une p‚riode de temps, il faut
d'abord se placer sur la bonne s‚rie … la bonne p‚riode. Cela se fait …
l'aide des touches fl‚ch‚es (UP, DOWN, PGUP, PGDN) ou en pressant
la premiŠre lettre du nom de la s‚rie.

Lorsque la case souhait‚e est atteinte, la touche ENTER ouvre une
fenˆtre dans laquelle la valeur est affich‚e et peut ˆtre modifi‚e.

Le fait de presser ENTER ou une des touches fl‚ch‚es sauve la valeur
modifi‚e et remet le programme directement en ‚dition de la case
suivante (au sens de la touche fl‚ch‚e utilis‚e).

&IT Touches fonction
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Les touches fonctions standards sont d‚finies pour le tableau de s‚ries.
En plus :

&EN la touche TAB fait d‚filer d'un ‚cran vers la droite

&EN la touche s-TAB fait d‚filer d'un ‚cran vers la gauche

&EN F2 et shift-F2 : respectivement augmente et diminue la largeur
    de la colonne reprenant le nom des variables

&EN F3 et shift-F3 : respectivement augmente et diminue la largeur
    des colonnes de valeurs

&EN F4 et shift-F4 : respectivement augmente et diminue le nombre de
    d‚cimales affich‚es

&EN F5 : le mode de visualisation passe de "Level" (valeur r‚elle), en
    mode "Difference" (diff‚rence p‚riode courante - p‚riode pr‚c‚dente)
    et en mode "Growth Rate" (taux de croissance) … chaque fois que la
    touche F5 est press‚e. Le mode courant est indiqu‚ dans la ligne de
    titre de la fenˆtre.

&EN F8 : affiche le graphique de la s‚rie sur laquelle le curseur se
    trouve avec les propri‚t‚s du plus r‚cent appel … la fonction
    Shift-F8

&EN Shift-F8 permet de grouper plusieurs s‚ries sur le mˆme graphique.
    De plus, cette fonction permet de modifier les propri‚t‚s (Sample,
    type, ...) des futurs graphiques affich‚s … l'aide de F8.

Par exemple pour visualiser deux variables sur un mˆme graphique sans avoir
… contruire un tableau :

.~a'pgraph.png'

Donnecomme r‚sultat :


.~a'gvars1.png'

Rappelons les autres touches principales :

&EN Ctrl-R : Rotation du tableau en inversant lignes et colonnes

&EN Ctrl-O : d‚placement de la fenˆtre dans l'‚cran

&EN Ctrl-Z : modification de la taille de la fenˆtre

&EN Ctrl-X : maximisation de la fenˆtre

&EN lettre : positionne sur la variable suivante dont le nom commence
    par la lettre press‚e

&EN F1 : accŠs au manuel

&EN espace : accŠs au menu de la fenˆtre


La souris peut ˆtre utilis‚e pour faciliter la plupart des op‚rations :
s‚lection d'une cellule, accŠs au menu, copie d'une formule, d‚placement
et agrandissement de la fenˆtre, etc.
>

<Edit Tables>
    Edit Tables
    ÄÄÄÄÄÄÄÄÄÄÄ

Cette fonction, accessible directement … partir de l'‚dition des
tableaux ou … partir du meu Print/View Tables permet la mise
… jour des s‚ries d‚crites par les formules.

En pressant ENTER sur une cellule, la valeur courante peut ˆtre modifi‚e
par l'utilisateur. Plusieurs cas sont possibles, certains rejettant la
modification :

&EN Si la colonne contient une op‚ration (taux de croissance, diff‚rence
    de fichiers, ...), la mise … jour est rejett‚e

&EN Si la formule d‚finissant la cellule commence par ~c0+...~C, cela
    signifie que cette cellule est prot‚g‚e. L'‚dition est rejett‚e

&EN Si la formule fait apparaŒtre plusieurs variables, la premiŠre
    seule est modifi‚e

&EN Si la formule ne fait pas apparaŒtre de variable, la cellule n'est
    pas ‚ditable

&EN Si la formule n'est pas inversible par rapport … la premiŠre
    variable qui y apparaŒt, la m‚thode de la s‚cante (non lin‚aire) est
    appliqu‚e pour tenter de r‚soudre l'‚quation. Si celle-ci n'aboutit
    pas, la valeur reste inchang‚e.

Chaque changement de valeur a un effet dans le WS de s‚ries. Le tableaux
est recalcul‚ … chaque modification, un peu comme dans une feuille de
calcul.
>


<Duplicate objects>
   Duplicate objects
   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les objets d‚finis dans les WS courants peuvent ˆtre copi‚s dans des
objets du mˆme type mais d'un nom diff‚rent. Pour certains objets, il
existe une m‚thode simple pour effectuer cette op‚ration : dans l'‚cran
de d‚finition de l'objet … duplicier, il suffit de changer le nom de
l'objet et de le sauver. Un nouvel objet ayant les mˆmes
caract‚ristiques que l'ancien est ainsi cr‚‚.

La fonction d‚crite ici permet la mˆme op‚ration. Elle fonctionne pour
tous les objets sauf pour les ‚quations. En effet, une ‚quation porte le
nom de sa variable endogŠne. Changer son nom change donc le nom de
l'endogŠne, ce qui en g‚n‚ral n'a pas de sens.

L'‚cran se pr‚sente comme suit:

.~a'pdup.png'

Le type d'objet … duplicier peut ˆtre s‚lectionn‚ en se pla‡ant sur le
champ indiquant le type et en pressant ENTER: un menu reprenant tous les
types d'objets permet d'effectuer la s‚lection.

Les deux autres champs contiennent respectivement le nom de l'objet …
copier et le nom du nouvel objet.

&IT Touches fonctions
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&EN F10 duplicie l'objet
&EN ESCAPE quitte l'‚cran en abandonnant l'op‚ration en cours

&NO Il n'y a pas de v‚rification sur l'existence du nouveau nom. En cas
    d'existence, l'ancien objet est remplac‚ par la copie.
>












