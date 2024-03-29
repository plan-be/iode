/* PRINT */


<Print/Graph>
   Print/Graph
   컴컴컴컴컴�


.~a'mprint.png'

Un des outils essentiels de la construction, du test et de la
productivit� d'un mod둳e est la visualisation et l'impression de
tableaux et de graphiques. IODE dispose des outils n괹essaires � cette
fin. IODE permet 괾alement d'imprimer tous les objets utilis굎 dans un
mod둳e.

Toutes les impressions peuvent 늯re dirig괻s vers l'imprimante ou
redirig괻s vers un fichier. S'il s'agit d'un fichier, le nom du fichier
d'impression sera demand� au d괷ut de l'impression, pour
permettre 굒entuellement de changer de nom et/ou de format de fichier.

Les parties suivantes d굏aillent les outils disponibles dans ce menu
pour imprimer les d괽initions des objets, les tableaux et les
graphiques:

&EN <Print/View Tables         > : compilation et impression de tables
&EN <Print/View Variables      > : impression des variables (sample donn�)
&EN <Object Definitions        > : impression de la d괽inition des objets
&EN <Graphs from Tables        > : graphiques construits sur des tables
&EN <Graphs from Variables     > : graphiques avec le WS "Variables"
&EN <Print A2M File            > : impression d'un fichier A2M
&EN <Print Report              > : impression de la d괽inition d'un rapport

Dans la suite de ce chapite, il est fait r괾uli둹ement r괽굍ence � la
notion de sample d'impression ou sample g굈굍alis� utilis� pour pr괹iser
le calcul � effectuer pour chaque colonne des tableaux ou chaque courbe
des graphiques. La syntaxe pr괹ise des samples d'impressions peut 늯re
trouv괻 en annexe.

>

<Print/View Tables>
   Impression et visualisation des tables
   컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
~a'pprtbl.png'

Ce point du menu permet le construction, la visualisation et l'impression
des tableaux.

Lorsque plusieurs tableaux doivent 늯re affich굎 (sous forme de
graphique ou de tableau de nombres), un tableau d굍oulant reprenant le
nom de chaque tableau ainsi que son titre est propos�.

Il suffit de presser ENTER sur le tableau souhait� pour afficher le
r굎ultat sous forme de tableau ou de graphique.

La page de saisie  contient les champs suivants:

&EN ~bTable names~B : liste des tableaux s굋ar굎 par des blancs, des
virgules ou des retours � la ligne

&EN ~bSample~B : sample d'impression (voir annexes). Il s'agit d'un
    sample g굈굍alis� acceptant des d괽initions de taux de croissance,
    de moyennes, etc, et la comparaison de fichiers.

&EN ~bFile[1..5]~B : quatre champs actifs afin de
    d괽inir la liste des fichiers d'origine des variables, le fichier
    num굍o 1 굏ant par d괽aut le WS courant. Ces fichiers sont
    r괽굍enc굎 dans le sample g굈굍alis� par des [1], [2], etc.

&EN ~bLanguage~B : le langage utilis� pour les textes automatiques (taux de croissance,
    etc)

&EN ~bNbDec~B : nombre de d괹imales imprim괻s

La liste des tableaux � imprimer doit 늯re d괽inie. Elle peut contenir
des noms de liste ($...) et/ou une 굈um굍ation de noms d'objets s굋ar굎
par un blanc, une virgule, un point-virgule ou un retour � la ligne.


&IT Touches fonctions
컴컴컴컴컴컴컴컴�

Les touches de fonctions applicables au niveau de l'괹ran de saisie
sont:

&EN ~bF3~B ou ESCAPE : quitte l'괹ran

&EN ~bF1~B : affiche l'aide

&EN ~bF6~B : setup de l'impression (choix entre fichier ou imprimante)

&EN ~bF8~B : lance l'impression

&EN ~bF10~B : affiche le r굎ultat � l'괹ran
>

<Print/View Variables>
   Impression de variables
   컴컴컴컴컴컴컴컴컴컴컴

~a'pprvbls.png'

Ce point du menu permet la visualisation et l'impression d'un tableau
contenant des variables sur une p굍iode
donn괻. Il s'agit d'une fonction comparable � celle
d'impression de tableaux, � ceci pr둺 que le tableau est construit
localement sur base des formes LEC donn괻s.

Les formes LEC (ou les noms des variables) sont s굋ar괻s par des
points-virgules ou des retours � la ligne.

La page de saisie contient les champs de saisie
suivants:

&EN ~bVariable names or LEC-forms~B : liste de variables ou de formes LEC
    s굋ar괻s par des points-virgules ou des retours � la ligne

&EN ~bSample~B : sample d'impression (voir annexes). Il s'agit d'un
    sample g굈굍alis� acceptant des d괽initions de taux de croissance,
    de moyennes, etc, et la comparaison de fichiers.

&EN ~bFile[1..5]~B : quatre champs actifs afin de
    d괽inir la liste des fichiers d'origine des variables, le fichier
    num굍o 1 굏ant par d괽aut le WS courant. Ces fichiers sont
    r괽굍enc굎 dans le sample g굈굍alis� par des [1], [2], etc.

&EN ~bLanguage~B : le langage utilis� pour les textes automatiques (taux de croissance,
    etc)

&EN ~bNbDec~B : nombre de d괹imales imprim괻s


&IT Touches fonctions
컴컴컴컴컴컴컴컴�

Les touches de fonctions applicables au niveau de l'괹ran de saisie
sont:

&EN ~bF3~B ou ESCAPE : quitte l'괹ran

&EN ~bF1~B : affiche l'aide

&EN ~bF6~B : setup de l'impression (choix entre fichier ou imprimante)

&EN ~bF8~B : lance l'impression

&EN ~bF10~B : affiche le r굎ultat � l'괹ran

&IT Exemple d'affichage (F10)
컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
~a'pprvbls2.png'
>

<Object Definitions>
   Impression de la d괽inition d'objets
   컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
.~a'pprobjs.png'

Ce point du menu permet l'impression de la d괽inition des
objets. Dans le cas de tableaux, par exemple, les formes LEC, les types
de lignes sont imprim굎, plut뱓 que les valeurs calcul괻s.

La page de saisie contient les champs suivants:

&EN ~bObject type~B  : type d'objet � imprimer,

&EN ~bFor Tables~B : choix entre impression des titres ou de la d괽inition
compl둻e des tableaux

&EN ~bPrint LEC forms~B  :
&EN2 dans leur forme normale
&EN2 avec remplacement des coefficients par leur valeur
&EN2 avec remplacement des coefficients par leur valeur et les t-tests � c뱓� des coefficients

&EN ~bObject names~B : liste des objets � imprimer. Si la liste est vide,
tous les objets du type sp괹ifi�, pr굎ents dans le WS, seront imprim굎. Les
listes ($...) sont admises.

&EN ~bLanguage~B : le langage utilis� pour les textes automatiques (taux de croissance,
    etc)

&EN ~bNbDec~B : nombre de d괹imales imprim괻s


Les objets SCALAIRES, TABLEAUX, VARIABLES sont imprim굎 sous forme de
tableaux, les autres objets (LISTES, COMMENTAIRES, EQUATIONS et IDENTITES)
sont imprim굎 sous forme de texte enrichi.

&IT Touches fonctions
컴컴컴컴컴컴컴컴�

Les touches de fonctions applicables au niveau de l'괹ran de saisie
sont:

&EN ~bF3~B ou ~bESCAPE~B : quitte

&EN ~bF1~B : affiche l'aide

&EN ~bF6~B : setup de l'impression (choix entre fichier ou imprimante)

&EN ~bF8~B : lance l'impression

&EN ~bF10~B : affiche les s굍ies � l'괹ran

&TI Exemple d'output
컴컴컴컴컴컴컴컴컴컴

.~a'pprobjs2.png'

>

<Graphs from Tables>
   Impression et visualisation des graphes sur base d'une table
   컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

.~a'pprgraphs.png'

Ce point du menu permet la construction, la visualisation et
l'impression des graphiques construits au d굋art de tables. L'ensemble
des attributs graphiques aura 굏� sp괹ifi� lors de la construction des
tables.

Les param둻res sont semblables � ceux d괽inis pour l'impression et la
visualisation de tableaux. Les m늤es objets sont en effet exploit굎,
mais pour construire des graphiques au lieu de tableaux de valeurs.

La page de saisie  contient les champs suivants:

&EN ~bTable names~B : liste des tableaux s굋ar굎 par des blancs, des
virgules ou des retours � la ligne

&EN ~bSample~B : sample d'impression (voir annexes). Il s'agit d'un
    sample g굈굍alis� acceptant des d괽initions de taux de croissance,
    de moyennes, etc, et la comparaison de fichiers.

&EN ~bFile[1..5]~B : quatre champs actifs afin de
    d괽inir la liste des fichiers d'origine des variables, le fichier
    num굍o 1 굏ant par d괽aut le WS courant. Ces fichiers sont
    r괽굍enc굎 dans le sample g굈굍alis� par des [1], [2], etc.

&EN ~bLanguage~B : le langage utilis� pour les textes automatiques (taux de croissance,
    etc)

&EN ~bNbDec~B : nombre de d괹imales imprim괻s

La liste des tableaux � imprimer doit 늯re d괽inie. Elle peut contenir
des noms de liste ($...) et/ou une 굈um굍ation de noms d'objets s굋ar굎
par un blanc, une virgule, un point-virgule ou un retour � la ligne.

&IT Touches fonctions
컴컴컴컴컴컴컴컴�

Les touches de fonctions applicables au niveau de l'괹ran de saisie
sont:

&EN F3 ou ESCAPE : quitte

&EN F1 : affiche l'aide

&EN F6 : setup de l'impression (choix entre fichier ou imprimante)

&EN F8 : lance l'impression des tableaux

&EN F10 : affiche les tableaux � l'괹ran

&TI Exemple de r굎ultat
컴컴컴컴컴컴컴컴컴컴컴�

.~a'pprgraphs2.png'
>

<Graphs from Variables>
   Impression et affichage des graphes sur base des Variables
   컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴


.~a'pprgrvars.png'

Ce point du menu permet la construction, la visualisation et
l'impression de graphiques construits au d굋art des variables contenues
dans le WS courant.

La page de saisie permet de param굏rer la
construction du ou des graphiques. Elle contient les champs suivants:

&EN ~bVariable names~B : liste des variables � inclure dans le graphique.
    Les noms (ou liste) peuvent 늯re s굋ar굎 par un blanc, un + ou une
    combinaison des deux. Les variables s굋ar괻s par un blanc seront
    visualis괻s dans des graphiques s굋ar굎 sur la m늤e page, tandis que
    les variables "additionn괻s" seront group괻s dans un m늤e graphique.
    Ainsi, si ce champ contient ~c"X Y+Z T"~C, trois graphiques seront
    construits avec respectivement les variables X (premier graphique),
    Y et Z (group괻s dans le deuxi둴e graphique) et T (dernier
    graphique).

&EN ~bSample (menu)~B : il s'agit d'un champ MENU permettant de pr괹iser si les
    variables doivent 늯re visualis괻s en valeur, en diff굍ence ou en
    taux de croissance

&EN ~bSample~B (from, to) : permet de pr괹iser la p굍iode d'impression (de
    p굍iode_1 � p굍iode_2), par exemple from 19970Y1 to 2010Y1.

&EN ~bLayout~B : diff굍ents param둻res on un effet sur l'apparence du
    graphique r굎ultat :

&EN2 Type de courbes : il s'agit d'un champ MENU permettant de pr괹iser
    le type de graphique � produire: line, scatter, scatter line ou bar

&EN2 Axis : ce champ du type MENU permet de s굃ectionner le type de
    transformation de coordonn괻s � utiliser: valeur, logarithme, ...

&EN2 XGrids et Ygrids : il s'agit 괾alement de champs MENU permettant de
    d괽inir le type de quadrillage en X et en Y (pas de quadrillage,
    quadrillage majeur ou quadrillage mineur)

&EN2 MinY et MaxY : ces deux champs permettent de d괽inir les valeurs
    minimum et maximum de l'axe des Y. Si ces valeurs sont NA (--),
    l'axe est calcul� automatiquement en fonction des valeurs �
    afficher.

&EN ~bLanguage~B : les textes fix굎 par le programme (taux de croissance,
    etc) peuvent 늯re imprim굎 en fran놹is, n괻rlandais ou anglais.

&IT Touches fonctions
컴컴컴컴컴컴컴컴�

Les touches de fonctions applicables au niveau de l'괹ran de saisie
sont:

&EN F3 ou ESCAPE : quitte

&EN F1 : affiche l'aide

&EN F6 : setup de l'impression (choix entre fichier ou imprimante)

&EN F7 : applique les d괽initions pour l'affichage ult굍ieur de s굍ies
    dans le tableau d'괺ition des s굍ies

&EN F8 : lance l'impression

&EN F10 : affiche � l'괹ran


&TI Exemple de r굎ultat
컴컴컴컴컴컴컴컴컴컴컴�

.~a'gprvars.png'
>


<Print A2M File            >
    Print A2M File
    컴컴컴컴컴컴컴
.~a'ppra2m.png'

Cette fonction permet d'imprimer un fichier A2M sans quitter IODE.
Il suffit de pr괹iser dans l'unique champ de la page le nom
du fichier � imprimer.

Le bouton SETUP (ou la touche F6) permet de s굃ectionner la destination
d'impression. Si cette destination est un fichier, le nom du fichier
r굎ultat sera demand� dans une fen늯re au moment du lancement de
l'impression.

Les options d괽inies pour toutes les impressions � travers la fonction
"Print Setup" sont 괾alement d'application dans cette fonction.
>

<Print Report>
    Print Report
    컴컴컴컴컴컴

.~a'pprreport.png'

Cette fonction permet d'imprimer un rapport sans en ex괹uter les commandes.
Elle fournit en quelque sorte le code du rapport.

Notons cependant que la structuration 굒entuelle en paragraphes exploitant
les propri굏굎 du langage A2M est exploit괻.

Le bouton SETUP (ou la touche F6) permet de s굃ectionner la destination
d'impression. Si cette destination est un fichier, le nom du fichier
r굎ultat sera demand� dans une fen늯re au moment du lancement de
l'impression.

Les options d괽inies pour toutes les impressions � travers la fonction
"Print Setup" sont 괾alement d'application dans cette fonction.
>







