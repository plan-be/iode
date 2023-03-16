/* PRINT */


<Print/Graph>
   Print/Graph
   ÄÄÄÄÄÄÄÄÄÄÄ


.~a'mprint.png'

Un des outils essentiels de la construction, du test et de la
productivit‚ d'un modŠle est la visualisation et l'impression de
tableaux et de graphiques. IODE dispose des outils n‚cessaires … cette
fin. IODE permet ‚galement d'imprimer tous les objets utilis‚s dans un
modŠle.

Toutes les impressions peuvent ˆtre dirig‚es vers l'imprimante ou
redirig‚es vers un fichier. S'il s'agit d'un fichier, le nom du fichier
d'impression sera demand‚ au d‚but de l'impression, pour
permettre ‚ventuellement de changer de nom et/ou de format de fichier.

Les parties suivantes d‚taillent les outils disponibles dans ce menu
pour imprimer les d‚finitions des objets, les tableaux et les
graphiques:

&EN <Print/View Tables         > : compilation et impression de tables
&EN <Print/View Variables      > : impression des variables (sample donn‚)
&EN <Object Definitions        > : impression de la d‚finition des objets
&EN <Graphs from Tables        > : graphiques construits sur des tables
&EN <Graphs from Variables     > : graphiques avec le WS "Variables"
&EN <Print A2M File            > : impression d'un fichier A2M
&EN <Print Report              > : impression de la d‚finition d'un rapport

Dans la suite de ce chapite, il est fait r‚guliŠrement r‚f‚rence … la
notion de sample d'impression ou sample g‚n‚ralis‚ utilis‚ pour pr‚ciser
le calcul … effectuer pour chaque colonne des tableaux ou chaque courbe
des graphiques. La syntaxe pr‚cise des samples d'impressions peut ˆtre
trouv‚e en annexe.

>

<Print/View Tables>
   Impression et visualisation des tables
   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
~a'pprtbl.png'

Ce point du menu permet le construction, la visualisation et l'impression
des tableaux.

Lorsque plusieurs tableaux doivent ˆtre affich‚s (sous forme de
graphique ou de tableau de nombres), un tableau d‚roulant reprenant le
nom de chaque tableau ainsi que son titre est propos‚.

Il suffit de presser ENTER sur le tableau souhait‚ pour afficher le
r‚sultat sous forme de tableau ou de graphique.

La page de saisie  contient les champs suivants:

&EN ~bTable names~B : liste des tableaux s‚par‚s par des blancs, des
virgules ou des retours … la ligne

&EN ~bSample~B : sample d'impression (voir annexes). Il s'agit d'un
    sample g‚n‚ralis‚ acceptant des d‚finitions de taux de croissance,
    de moyennes, etc, et la comparaison de fichiers.

&EN ~bFile[1..5]~B : quatre champs actifs afin de
    d‚finir la liste des fichiers d'origine des variables, le fichier
    num‚ro 1 ‚tant par d‚faut le WS courant. Ces fichiers sont
    r‚f‚renc‚s dans le sample g‚n‚ralis‚ par des [1], [2], etc.

&EN ~bLanguage~B : le langage utilis‚ pour les textes automatiques (taux de croissance,
    etc)

&EN ~bNbDec~B : nombre de d‚cimales imprim‚es

La liste des tableaux … imprimer doit ˆtre d‚finie. Elle peut contenir
des noms de liste ($...) et/ou une ‚num‚ration de noms d'objets s‚par‚s
par un blanc, une virgule, un point-virgule ou un retour … la ligne.


&IT Touches fonctions
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les touches de fonctions applicables au niveau de l'‚cran de saisie
sont:

&EN ~bF3~B ou ESCAPE : quitte l'‚cran

&EN ~bF1~B : affiche l'aide

&EN ~bF6~B : setup de l'impression (choix entre fichier ou imprimante)

&EN ~bF8~B : lance l'impression

&EN ~bF10~B : affiche le r‚sultat … l'‚cran
>

<Print/View Variables>
   Impression de variables
   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

~a'pprvbls.png'

Ce point du menu permet la visualisation et l'impression d'un tableau
contenant des variables sur une p‚riode
donn‚e. Il s'agit d'une fonction comparable … celle
d'impression de tableaux, … ceci prŠs que le tableau est construit
localement sur base des formes LEC donn‚es.

Les formes LEC (ou les noms des variables) sont s‚par‚es par des
points-virgules ou des retours … la ligne.

La page de saisie contient les champs de saisie
suivants:

&EN ~bVariable names or LEC-forms~B : liste de variables ou de formes LEC
    s‚par‚es par des points-virgules ou des retours … la ligne

&EN ~bSample~B : sample d'impression (voir annexes). Il s'agit d'un
    sample g‚n‚ralis‚ acceptant des d‚finitions de taux de croissance,
    de moyennes, etc, et la comparaison de fichiers.

&EN ~bFile[1..5]~B : quatre champs actifs afin de
    d‚finir la liste des fichiers d'origine des variables, le fichier
    num‚ro 1 ‚tant par d‚faut le WS courant. Ces fichiers sont
    r‚f‚renc‚s dans le sample g‚n‚ralis‚ par des [1], [2], etc.

&EN ~bLanguage~B : le langage utilis‚ pour les textes automatiques (taux de croissance,
    etc)

&EN ~bNbDec~B : nombre de d‚cimales imprim‚es


&IT Touches fonctions
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les touches de fonctions applicables au niveau de l'‚cran de saisie
sont:

&EN ~bF3~B ou ESCAPE : quitte l'‚cran

&EN ~bF1~B : affiche l'aide

&EN ~bF6~B : setup de l'impression (choix entre fichier ou imprimante)

&EN ~bF8~B : lance l'impression

&EN ~bF10~B : affiche le r‚sultat … l'‚cran

&IT Exemple d'affichage (F10)
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
~a'pprvbls2.png'
>

<Object Definitions>
   Impression de la d‚finition d'objets
   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
.~a'pprobjs.png'

Ce point du menu permet l'impression de la d‚finition des
objets. Dans le cas de tableaux, par exemple, les formes LEC, les types
de lignes sont imprim‚s, plut“t que les valeurs calcul‚es.

La page de saisie contient les champs suivants:

&EN ~bObject type~B  : type d'objet … imprimer,

&EN ~bFor Tables~B : choix entre impression des titres ou de la d‚finition
complŠte des tableaux

&EN ~bPrint LEC forms~B  :
&EN2 dans leur forme normale
&EN2 avec remplacement des coefficients par leur valeur
&EN2 avec remplacement des coefficients par leur valeur et les t-tests … c“t‚ des coefficients

&EN ~bObject names~B : liste des objets … imprimer. Si la liste est vide,
tous les objets du type sp‚cifi‚, pr‚sents dans le WS, seront imprim‚s. Les
listes ($...) sont admises.

&EN ~bLanguage~B : le langage utilis‚ pour les textes automatiques (taux de croissance,
    etc)

&EN ~bNbDec~B : nombre de d‚cimales imprim‚es


Les objets SCALAIRES, TABLEAUX, VARIABLES sont imprim‚s sous forme de
tableaux, les autres objets (LISTES, COMMENTAIRES, EQUATIONS et IDENTITES)
sont imprim‚s sous forme de texte enrichi.

&IT Touches fonctions
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les touches de fonctions applicables au niveau de l'‚cran de saisie
sont:

&EN ~bF3~B ou ~bESCAPE~B : quitte

&EN ~bF1~B : affiche l'aide

&EN ~bF6~B : setup de l'impression (choix entre fichier ou imprimante)

&EN ~bF8~B : lance l'impression

&EN ~bF10~B : affiche les s‚ries … l'‚cran

&TI Exemple d'output
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

.~a'pprobjs2.png'

>

<Graphs from Tables>
   Impression et visualisation des graphes sur base d'une table
   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

.~a'pprgraphs.png'

Ce point du menu permet la construction, la visualisation et
l'impression des graphiques construits au d‚part de tables. L'ensemble
des attributs graphiques aura ‚t‚ sp‚cifi‚ lors de la construction des
tables.

Les paramŠtres sont semblables … ceux d‚finis pour l'impression et la
visualisation de tableaux. Les mˆmes objets sont en effet exploit‚s,
mais pour construire des graphiques au lieu de tableaux de valeurs.

La page de saisie  contient les champs suivants:

&EN ~bTable names~B : liste des tableaux s‚par‚s par des blancs, des
virgules ou des retours … la ligne

&EN ~bSample~B : sample d'impression (voir annexes). Il s'agit d'un
    sample g‚n‚ralis‚ acceptant des d‚finitions de taux de croissance,
    de moyennes, etc, et la comparaison de fichiers.

&EN ~bFile[1..5]~B : quatre champs actifs afin de
    d‚finir la liste des fichiers d'origine des variables, le fichier
    num‚ro 1 ‚tant par d‚faut le WS courant. Ces fichiers sont
    r‚f‚renc‚s dans le sample g‚n‚ralis‚ par des [1], [2], etc.

&EN ~bLanguage~B : le langage utilis‚ pour les textes automatiques (taux de croissance,
    etc)

&EN ~bNbDec~B : nombre de d‚cimales imprim‚es

La liste des tableaux … imprimer doit ˆtre d‚finie. Elle peut contenir
des noms de liste ($...) et/ou une ‚num‚ration de noms d'objets s‚par‚s
par un blanc, une virgule, un point-virgule ou un retour … la ligne.

&IT Touches fonctions
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les touches de fonctions applicables au niveau de l'‚cran de saisie
sont:

&EN F3 ou ESCAPE : quitte

&EN F1 : affiche l'aide

&EN F6 : setup de l'impression (choix entre fichier ou imprimante)

&EN F8 : lance l'impression des tableaux

&EN F10 : affiche les tableaux … l'‚cran

&TI Exemple de r‚sultat
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

.~a'pprgraphs2.png'
>

<Graphs from Variables>
   Impression et affichage des graphes sur base des Variables
   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ


.~a'pprgrvars.png'

Ce point du menu permet la construction, la visualisation et
l'impression de graphiques construits au d‚part des variables contenues
dans le WS courant.

La page de saisie permet de param‚trer la
construction du ou des graphiques. Elle contient les champs suivants:

&EN ~bVariable names~B : liste des variables … inclure dans le graphique.
    Les noms (ou liste) peuvent ˆtre s‚par‚s par un blanc, un + ou une
    combinaison des deux. Les variables s‚par‚es par un blanc seront
    visualis‚es dans des graphiques s‚par‚s sur la mˆme page, tandis que
    les variables "additionn‚es" seront group‚es dans un mˆme graphique.
    Ainsi, si ce champ contient ~c"X Y+Z T"~C, trois graphiques seront
    construits avec respectivement les variables X (premier graphique),
    Y et Z (group‚es dans le deuxiŠme graphique) et T (dernier
    graphique).

&EN ~bSample (menu)~B : il s'agit d'un champ MENU permettant de pr‚ciser si les
    variables doivent ˆtre visualis‚es en valeur, en diff‚rence ou en
    taux de croissance

&EN ~bSample~B (from, to) : permet de pr‚ciser la p‚riode d'impression (de
    p‚riode_1 … p‚riode_2), par exemple from 19970Y1 to 2010Y1.

&EN ~bLayout~B : diff‚rents paramŠtres on un effet sur l'apparence du
    graphique r‚sultat :

&EN2 Type de courbes : il s'agit d'un champ MENU permettant de pr‚ciser
    le type de graphique … produire: line, scatter, scatter line ou bar

&EN2 Axis : ce champ du type MENU permet de s‚lectionner le type de
    transformation de coordonn‚es … utiliser: valeur, logarithme, ...

&EN2 XGrids et Ygrids : il s'agit ‚galement de champs MENU permettant de
    d‚finir le type de quadrillage en X et en Y (pas de quadrillage,
    quadrillage majeur ou quadrillage mineur)

&EN2 MinY et MaxY : ces deux champs permettent de d‚finir les valeurs
    minimum et maximum de l'axe des Y. Si ces valeurs sont NA (--),
    l'axe est calcul‚ automatiquement en fonction des valeurs …
    afficher.

&EN ~bLanguage~B : les textes fix‚s par le programme (taux de croissance,
    etc) peuvent ˆtre imprim‚s en fran‡ais, n‚erlandais ou anglais.

&IT Touches fonctions
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les touches de fonctions applicables au niveau de l'‚cran de saisie
sont:

&EN F3 ou ESCAPE : quitte

&EN F1 : affiche l'aide

&EN F6 : setup de l'impression (choix entre fichier ou imprimante)

&EN F7 : applique les d‚finitions pour l'affichage ult‚rieur de s‚ries
    dans le tableau d'‚dition des s‚ries

&EN F8 : lance l'impression

&EN F10 : affiche … l'‚cran


&TI Exemple de r‚sultat
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

.~a'gprvars.png'
>


<Print A2M File            >
    Print A2M File
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
.~a'ppra2m.png'

Cette fonction permet d'imprimer un fichier A2M sans quitter IODE.
Il suffit de pr‚ciser dans l'unique champ de la page le nom
du fichier … imprimer.

Le bouton SETUP (ou la touche F6) permet de s‚lectionner la destination
d'impression. Si cette destination est un fichier, le nom du fichier
r‚sultat sera demand‚ dans une fenˆtre au moment du lancement de
l'impression.

Les options d‚finies pour toutes les impressions … travers la fonction
"Print Setup" sont ‚galement d'application dans cette fonction.
>

<Print Report>
    Print Report
    ÄÄÄÄÄÄÄÄÄÄÄÄ

.~a'pprreport.png'

Cette fonction permet d'imprimer un rapport sans en ex‚cuter les commandes.
Elle fournit en quelque sorte le code du rapport.

Notons cependant que la structuration ‚ventuelle en paragraphes exploitant
les propri‚t‚s du langage A2M est exploit‚e.

Le bouton SETUP (ou la touche F6) permet de s‚lectionner la destination
d'impression. Si cette destination est un fichier, le nom du fichier
r‚sultat sera demand‚ dans une fenˆtre au moment du lancement de
l'impression.

Les options d‚finies pour toutes les impressions … travers la fonction
"Print Setup" sont ‚galement d'application dans cette fonction.
>







