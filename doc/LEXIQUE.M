/* LEXIQUE */

<Lexique>
    LEXIQUE
    ÄÄÄÄÄÄÄ

Cette partie du manuel reprend la d‚finition de quelques sujets souvent
abord‚s dans le texte. Les explications concernant l'utilisation des
champs, des ‚crans, etc, y sont ‚galement trait‚es.

&EN <Le format A2M          >
&EN <Champ DIR              >
&EN <Champ EDITOR           >
&EN <Champ menu             >
&EN <Editeur MMT            >
&EN <Les "listes"           >
&EN <NA (Not Available)     >
&EN <Tableau d‚roulant      >
>

<Editeur MMT            >
    Editeur MMT
    ÄÄÄÄÄÄÄÄÄÄÄ

L'‚diteur MMT est un ‚diteur ASCII qui permet d'‚diter simultan‚ment
plusieurs textes dans plusieurs fenˆtres visibles … l'‚cran. Il a ‚t‚
d‚velopp‚ par et pour des d‚veloppeurs en vue de rendre performantes les
op‚rations d'‚dition et de modification des fichiers de code source,
comme des rapports de IODE.

L'‚diteur MMT est utilis‚ dans les champs de type ‚diteur ainsi que dans
la fonction d'‚dition des rapports.

Parmi les options int‚ressantes de cet ‚diteur figure la possibilit‚ de
faire du "cut and paste" d'un champ … l'autre d'un programme ou d'un
champ vers un texte, ou d'un texte vers un champ.

Une description complŠte de l'‚diteur MMT figure dans les annexes.
>


<Le format A2M>
    A2M
    ÄÄÄ

A2M est un format d'‚change de documents entre logiciels d‚velopp‚ par
et pour les besoins du Bureau f‚d‚ral du Plan.

Un texte au format A2M contient des caractŠres ASCII du jeu ‚tendu
accentu‚ et ‚ventuellement des commandes et des instructions
d'enrichissement typographique. La d‚coupe du texte en paragraphe, les
sauts de lignes et les lignes blanches revˆtent ‚galement une
signification particuliŠre. Ces instructions et commandes sont toutes
optionnelles, tout texte ASCII ‚tant par d‚finition au format A2M.

Le langage d'interface A2M (Ascii TO Mif) a ‚t‚ d‚velopp‚ initialement
afin de pouvoir taper ou construire un document en utilisant un simple
‚diteur ASCII, et d'obtenir un texte correctement mis en page sur FRAME,
sans intervention au niveau de FRAME, via son format d'interface MIF.
Les premiŠres versions du filtre SCR3_A2M effectuaient la convertion A2M
vers MIF.

L'id‚e a ensuite ‚t‚ ‚largie, A2M est devenu un format d'interface
g‚n‚r‚ par plusieurs logiciels produits par la Cellule Informatique du
Bureau f‚d‚ral du Plan (Iode, Stock, Matrix, Lex, ...). Un format de
tableaux et de graphiques a ‚t‚ impl‚ment‚. Des modules ont ‚t‚
d‚velopp‚s pour transformer le format A2M en d'autres formats d'‚change
(RTF, CSV, HTML en autre) ou directement en format d'impression Windows.
>

<Champ DIR>
Champ DIR
ÄÄÄÄÄÄÄÄÄ

Un champ de type DIR est un champ de saisie permettant la recherche d'un
nom de fichier en fonction d'un masque de saisie. On commence par
encoder dans le champ le masque de fichier exactement comme dans un
champ normal de texte (p.ex. "*.var").

La touche ENTER affiche alors la liste des fichiers correspondant au
masque de saisie. Les sous-r‚pertoires sont ‚galement mentionn‚s. Le nom
du fichier d‚sir‚ peut alors ˆtre s‚lectionn‚ … l'aide des touches
fl‚ch‚es et de sauts de pages haut et bas. Le nom choisi est valid‚ par
la touche ENTER. Ce nom s'inscrit dans la page de saisie … la place du
masque de recherche. Si le nom s‚lectionn‚ est un nom de r‚pertoire, la
recherche se poursuit dans ce r‚pertoire. Ces champs fonctionnent par
ailleurs comme un champ de texte normal.

Si aucun nom affich‚ dans le menu ne doit ˆtre s‚lectionn‚, la touche
ESCAPE permet de quitter le menu sans exporter de nom dans le champ.

Dans le menu, on trouve pour chaque fichier plusieurs informations comme
sa taille, la date de derniŠre modification, ses propri‚t‚s d'accŠs, ...
>

<Champ EDITOR>
Champ EDITOR
ÄÄÄÄÄÄÄÄÄÄÄÄ

Un champ EDITOR est un champ de saisie dont la dimension n'est pas
limit‚e. On entre en ‚dition du champ avec la touche ENTER lorsque le
champ est s‚lectionn‚ (c'est-…-dire apparaŒt en vid‚o inverse). On quitte le
champ avec la touche ESCAPE. Lorsqu'on est en ‚dition, outre les touches
fl‚ch‚es, les touches de saut de page et les touches home et end, les
s‚quences suivantes sont admises:

Les s‚quences de touches de l'‚diteur MMT sont valides, pour autant que
leur effet se limite … une seule ligne.

La position de la fenˆtre visible par rapport au contenu du champ peut ˆtre
repr‚sent‚e par une "scroll bar".

Si on dispose d'une souris, celle-ci offre des facilit‚s pour le
marquage (bouton de gauche) et la fonction copie/collage (bouton de
droite).
>
<Champ menu>
Champ menu
ÄÄÄÄÄÄÄÄÄÄ

Un champ MENU est un champ de saisie dont le contenu est limit‚ aux
options d'un menu. Le menu est accessible avec la touche ENTER, on s'y
d‚place avec les touches fl‚ch‚es et de saut de page haut et bas.
L'option s‚lectionn‚e est valid‚e avec la touche ENTER, le texte
correspondant s'inscrit dans la page de saisie. On peut ‚galement
s‚lectionner le texte de l'option en tapant sa premiŠre lettre ou un
espace pour passer d'option en option.
>



<Les "listes">
Les listes
ÄÄÄÄÄÄÄÄÄÄ

Une liste est l'un des sept objets manipul‚s par le logiciel IODE. Elle
est identifi‚e par un nom de 10 caractŠres (20 … partir de la version 6.01)
majuscules maximum commen‡ant toujours par une lettre (ou le caractŠre '_').

Certaines fonctions acceptent comme paramŠtres des listes.
Il s'agit alors g‚n‚ralement d'une ‚num‚ration dans un champ EDITOR.
Chaque ‚l‚ment de la liste peut ˆtre un objet quelconque (y compris une
liste identifi‚e par le pr‚fixe $), les noms des objets ‚tant s‚par‚s
par un blanc, une virgule, un point-virgule ou un saut de ligne.
>

<NA (Not Available)>
NA (Not Available)
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
La valeur NA est utilis‚e dans les variables et dans les scalaires pour
indiquer une valeur inconnue. Plusieurs raisons peuvent donner lieu …
cette valeur :

&EN lors de la cr‚ation d'une s‚rie, ses valeurs sont NA

&EN lors du calcul d'une s‚rie par identit‚, une valeur NA peut provenir
    d'une erreur de calcul ou d'une information manquante dans les
    variables de r‚f‚rence

&EN lors d'une simulation, une valeur NA peut venir d'une faute de
    calcul (log(-1), A/0, etc).

La valeur NA est repr‚sent‚e par deux tirets (--). Elle peut ˆtre
introduite de cette fa‡on par l'utilisateur dans l'‚diteur de variables.
>
<Sauvetage ou sauvegarde>
Sauvetage ou sauvegarde
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Enregistrement d'un fichier sur disque, disquette ou r‚seau.

Lorsqu'on est en ‚dition d'une page, le sauvetage signifie la validation et
la mise en m‚moire du contenu des champs de la page (g‚n‚ralement par la
touche ENTER pour les pages … un seul champ ou F10 pour les pages …
plusieurs champs).
>
<Scrollbar>
Scrollbar
ÄÄÄÄÄÄÄÄÄ

Un scrollbar peut apparaŒtre dans la partie gauche d'un menu, d'un champ
EDITOR ou d'un tableau d‚roulant pour indiquer la position de la fenˆtre
visible (celle qui apparaŒt … l'‚cran) dans le menu, le champ ou le tableau
complet. On d‚place la fenˆtre visible de haut en bas avec les touches
fl‚ch‚es ou les sauts de page haut et bas.
>

<Tableau d‚roulant>
Tableau d‚roulant ou tableau d'‚dition
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Un tableau d‚roulant (ou tableau SCROLL) est un ‚cran permettant d'afficher
un tableau du type de ceux des tableurs ordinaires et selon le cas d'‚diter
le contenu des cellules. Le tableau peut ˆtre d‚plac‚ sur l'‚cran, invers‚,
agrandi ou r‚duit, etc.

Lorsque le tableau s'affiche, les touches suivantes peuvent ˆtre utilis‚es
(en plus des touches de fonction globales du programme):

&EN ESCAPE : quitte l'‚dition

&EN UP, DOWN : d‚place le curseur vers la cellule sup‚rieure ou
    inf‚rieure

&EN PGUP, PGDN : d‚place le curseur d'un ‚cran vers le haut ou le bas

&EN LEFT, RIGHT : d‚place le curseur vers la cellule de gauche ou de
    droite

&EN TAB et Shift-TAB : d‚place le curseur d'un ‚cran vers la droite ou
    la gauche

&EN HOME : place le curseur en premiŠre colonne de la ligne courante

&EN CTRL + HOME : place le curseur en premiŠre ligne de la colonne
    courante

&EN END : place le curseur en derniŠre colonne de la ligne courante

&EN CTRL + END : place le curseur en derniŠre ligne de la colonne
    courante

&EN ENTER : ‚dite la cellule courante (si autoris‚)

&EN Ctrl-R : inverse le tableau (lignes vs colonnes)

&EN Ctrl-O : d‚place le tableau dans les limites de l'‚cran suivant les
    touches fl‚ch‚es utilis‚es ensuite. L'‚dition reprend aprŠs que ENTER
    ait ‚t‚ press‚.

&EN Ctrl-Z : change la taille du tableau dans les limites de l'‚cran
    suivant les touches fl‚ch‚es utilis‚es ensuite. L'‚dition reprend
    aprŠs que ENTER ait ‚t‚ press‚.

&EN Lettre ou chiffre : positionne le curseur sur la prochaine ligne
    dont le titre commence par le caractŠre press‚.
>

<L'objet "variable">
L'objet "variable"
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Une variable est une s‚rie temporelle de valeurs r‚elles. Les variables
peuvent ˆtre endogŠnes (calcul‚es par le modŠle) ou exogŠnes (donn‚es
d'origine ext‚rieure au modŠle).
>







