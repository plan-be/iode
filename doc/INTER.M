/* */

<L'interface utilisateur de IODE>
    L'interface utilisateur de IODE
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette section d‚crit les diff‚rents ‚l‚ments qui composent l'‚cran de
base de IODE et les moyens dont dispose l'utilisateur pour interagir
avec le logiciel.

Le lexique qui se trouve en fin de manuel fournit ‚galement certaines
informations sur l'interface.

&TI L'‚cran de base
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
~a'piode.png'

En allant de la premiŠre … la derniŠre ligne de l'‚cran, on recontre successivement:
les ‚l‚ments suivants :

&IT PremiŠre ligne : version du logiciel
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
La premiŠre ligne indique le num‚ro de version et le nom du logiciel.

&IT DeuxiŠme ligne : menu d‚roulant
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
La seconde ligne contient un menu.
Chaque option concerne un groupe de fonctions :
"File" donne accŠs aux actions
concernant les fichiers, "WS" … celles concernant les workspaces, etc.
Pour acc‚der … une de ces options, il suffit de presser la touche ALT
associ‚e … la premiŠre lettre de l'option (ALT+F donne accŠs au menu
FILE), ou de presser la souris sur l'option choisie.

Dans le coin sup‚rieur droit de l'‚cran est affich‚ un mot qui peut
prendre deux valeurs:

&EN INPUT qui indique que le programme attent
&EN WAIT qui indique que le programme est en train d'ex‚cuter une
    fonction

&IT Centre de l'‚cran : les donn‚es et les ‚crans de saisie
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
En fonction de l'option s‚lectionn‚e dans les menus, s'afficheront dans le
centre de l'‚cran les donn‚es demand‚es ou un ‚cran de saisie. Dans
l'exemple ci-dessus, l'option "Load Workspace" du menu "Workspace" a ‚t‚
s‚lectionn‚e et l'‚cran permet de sp‚cifier les fichiers … charger en
m‚moire.

&IT Avant-derniŠre ligne de l'‚cran : aide rapide
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Dans l'avant-derniŠre ligne de l'‚cran est affich‚ un message. Celui-ci
est un commentaire indiquant les diff‚rentes touches utilisables et leur
fonction ou simplement l'effet de la commande actuellement s‚lectionn‚e.

&IT DerniŠre ligne de l'‚cran : message, printer/file, heure
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
La derniŠre ligne de l'‚cran contient, de gauche … droite, les
informations suivantes :

&EN un message indiquant soit une erreur, soit l'‚tat d'avancement du
    d‚roulement d'une fonction (simulation, estimation, $show dans un
    rapport, etc)


&EN la destination de l'impression qui peut ˆtre soit FILE pour
    rediriger toute impression vers un fichier, soit PRINTER.

&EN l'heure


&TI Les ‚crans de saisie
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Le fonctionnement des ‚crans de saisie est assez intuitif. Quelques
informations utiles sont reprises ci-dessous.

&IT Touches sp‚ciales
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&EN ~cCtrl+U~C : vide le champ de son contenu
&EN ~cCtrl+C~C : copie la partie s‚lectionn‚e du champ
&EN ~cCtrl+V~C : colle la s‚lection pr‚c‚demment copi‚e (dans n'importe quelle application)

&EN ENTER : selon le type de champ, le comportement varie :
&EN2 : champ bouton : ex‚cute l'action associ‚e au bouton
&EN2 : champ DIR : affiche les fichiers r‚pondant au masque du champ
&EN2 : champ EDITOR : entre en ‚dition du texte du champ.
&EN2 : champ MENU : ouvre le menu pour effectuer une s‚lection
&EN F10 : ‚quivaut … presser le bouton Ok de la fenˆtre.
&EN ESC ou F3 : ‚quivaut … presser le bouton Escape de la fenˆtre.

&IT Champ menu
ÄÄÄÄÄÄÄÄÄÄ
Le menu s'ouvre en pressant la touche ENTER ou en cliquant sur la souris. On
s'y d‚place avec les touches fl‚ch‚es et de sauts de page haut et bas.
L'option s‚lectionn‚e est valid‚e avec la touche ENTER et le texte
correspondant s'inscrit dans la page de saisie. On peut ‚galement
s‚lectionner une option en tapant sa premiŠre lettre ou un espace pour
passer d'option en option.

&IT Champ DIR
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

&IT Champ EDITOR
ÄÄÄÄÄÄÄÄÄÄÄÄ
Les champs EDITOR sont reconnaissables au "scrollbar" qui apparaŒt en
g‚n‚ral … leur gauche. Il sont le plus souvent encadr‚s.

Pour entrer en ‚dition d'un champ EDITOR, il faut presser ENTER. On
dispose alors d'un ‚diteur de texte complet, avec fonctions de
copie/collage, formattage de texte, etc. Ce type de champ n'a pas
de limite de taille. Il est en particulier utile pour la saisie
de formules.
Les principales fonctions sont cit‚es ici :

&EN Pour quitter l'‚diteur (et revenir au niveau de la PAGE de saisie)
    il suffit de presser ESCAPE ou Alt-Q.

&EN Pour marquer un bloc de texte (… copier), on utilisera les touches
    Alt-B, Alt-L ou Alt-T selon que le bloc … marquer est un rectangle
    (B), un groupe de lignes (L) ou une partie de texte (T).

&EN Pour copier le bloc marqu‚ (en vue d'une copie), on
    utilisera ~cCtrl+C.~C Pour coller le contenu du clipboard …
    l'endroit du curseur, on pressera ~cCtrl+V.~C Cette copie peut se
    faire dans un autre texte que le texte courant (copie d'une ‚quation
    … une autre par exemple).

&EN Pour d‚truire le bloc marqu‚ et le placer dans le tampon, on
    utilisera ~cCtrl+D.~C

Si on dispose d'une souris, celle-ci offre des facilit‚s pour le
marquage (bouton de gauche) et la fonction copie/collage (bouton de
droite).

&TI Les touches fonctions
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Pour sp‚cifier une action … effectuer, il y a plusieurs fa‡ons de
proc‚der :

&EN une premiŠre est de presser la touche fonction (F<<n>>, ...)
    ad‚quate,

&EN une deuxiŠme est de presser ENTER ou de cliquer sur le bouton
    d'un ‚cran de saisie (qui correspond toujours … une touche
    fonction),

&EN une troisiŠme enfin est de cliquer avec la souris dans la zone de
    commentaire (avant-derniŠre ligne de l'‚cran) sur le texte de la
    touche correspondant … la fonction d‚sir‚e. Le texte de la touche
    se distingue du reste du commentaire par sa couleur.


Les touches fonctions (F1 … F10, Alt+F1 … Alt+F10, etc) sont de trois
types : les touches locales … l'endroit de l'application o— on se
trouve, les touches globales qui s'ex‚cutent en interrompant
temporairement l'ex‚cution du processus en cours et les touches
permettant un accŠs rapide au menu d‚roulant.

&IT Touches fonctions locales
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Un cas ‚vident o— les touches fonctions locales sont indispensables
est celui des ‚crans de saisie: une fois les donn‚es encod‚es,
l'utilisateur doit pouvoir soit confirmer le traitement … effectuer, soit
d‚cider d'abandonner la page en cours sans ex‚cution. Ce choix est
indiqu‚ au programme par une touche fonction (en g‚n‚ral F10
confirme et ESCAPE abandonne).

Toujours dans un ‚cran de saisie, une touche peut permettre d'introduire
des informations compl‚mentaires ou de visualiser des donn‚es utiles.
C'est le cas dans l'‚cran d'impression de tableaux : F8 permet
d'imprimer, tandis que F10 affiche … l'‚cran les valeurs du tableau.

En toutes circonstances, les touches fonctions locales sont indiqu‚es
avec l'op‚ration qu'elles d‚clenchent dans la ligne de commentaire de
l'‚cran (avant-derniŠre ligne).

&IT Touches fonctions globales
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
D'autres touches de fonctions ont un effet global : o— que l'on se
trouve dans l'application, ces touches interrompent le programme et
ex‚cutent une action d‚termin‚e. Le processus en cours … l'origine
reprend aprŠs la fin de l'action associ‚e … la touche fonction globale.

Dans le module IODE, la touche fonction globale F9 permet … tout moment,
de voir l'‚tat d'utilisation de la m‚moire sans avoir … quitter
l'endroit actuel de l'application.

Toutes les touches fonctions d‚finies sont expos‚es en d‚tail en
fonction du contexte.

&IT Touches fonctions du menu d‚roulant
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
D'autres touches globales sont celles permettant de se positionner
directement dans une fonction du menu d‚roulant. La touche Alt associ‚e
… la premiŠre lettre d'une option du menu d‚roulant a pour effet de
quitter la fonction en cours et de positionner l'application sur le
point du menu correspondant (Alt + F = menu File).

Ces touches abandonnent par cons‚quent l'action en cours s'il y en a une.

Dans les sous-menus du menu d‚roulant, on trouve … droite de certaines
options l'indication d'une touche fonction. Le fait de presser la touche
en question abandonne l'action en cours et ex‚cute directement la
fonction indiqu‚e dans le menu.

Par exemple, dans le menu "Misc", la touche Alt-X est associ‚e …
l'option "Quit". Presser Alt + X … n'importe quel moment permet de
quitter le programme.

&TI La souris
ÄÄÄÄÄÄÄÄÄ
La souris facilite un certain nombre d'op‚rations dans l'utilisation de
IODE :

&EN dans les ‚crans de saisie, un simple clic permet de positionner
le curseur dans le champ voulu, et … la position souhait‚e.

&EN sur les contours des fenˆtres de saisie, d'aide ou de scroll, des
    petites ic“nes permettent d'effectuer certaines op‚rations :

&EN2 [o] : fermer la fenˆtre (= ESCAPE)
&EN2 [=] : accŠs au menu de la fenˆtre reprenant toutes les touches
	    fonctions d‚finies localement
&EN2 [?] : accŠs au manuel en ligne
&EN2 d'autres ic“nes permettent d'effectuer une rotation dans les
    scrolls, un d‚placement sur les sujets pr‚c‚dents ou suivants dans
    les ‚crans d'aide, de maximiser et minimiser la taille de la
    fenˆtre, etc.

&EN dans l'‚diteur MMT (rapports, champs EDITOR), elle permet une s‚lection
    de bloc ou un accŠs … un menu simplifi‚ pour les op‚rations de copie
    et collage (bouton de droite).

&EN lorsque certaines touches fonctions sont mises en exergue dans la
    zone de commentaire, la souris permet de "presser" ces touches sans
    avoir recours au clavier

&EN les fenˆtres (pages, scroll, help, mmt) peuvent ˆtre d‚plac‚es
    en pla‡ant la souris sur le contour et en gardant le bouton appuy‚
    pendant le d‚placement

&EN la taille des mˆmes fenˆtres peut ˆtre modifi‚e en pla‡ant la souris
    sur le coin inf‚rieur droit et en "tirant" tout en gardant enfonc‚
    le bouton de la souris.

&EN les options du menu d‚roulant sont directement accessibles … l'aide
    de la souris, comme … l'aide des touches Alt-lettre.

&EN dans les objets ayant un scrollbar (MMT, champs EDITOR, Help, Menus,
    Scrolls) la souris permet de placer le curseur … l'endroit souhait‚
    par un simple clic.
>











