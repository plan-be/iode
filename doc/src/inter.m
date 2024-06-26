/* */

<L'interface utilisateur de IODE>
    L'interface utilisateur de IODE
    컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�

Cette section d괹rit les diff굍ents 굃굆ents qui composent l'괹ran de
base de IODE et les moyens dont dispose l'utilisateur pour interagir
avec le logiciel.

Le lexique qui se trouve en fin de manuel fournit 괾alement certaines
informations sur l'interface.

&TI L'괹ran de base
컴컴컴컴컴컴컴�
~a'piode.png'

En allant de la premi둹e � la derni둹e ligne de l'괹ran, on recontre successivement:
les 굃굆ents suivants :

&IT Premi둹e ligne : version du logiciel
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
La premi둹e ligne indique le num굍o de version et le nom du logiciel.

&IT Deuxi둴e ligne : menu d굍oulant
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
La seconde ligne contient un menu.
Chaque option concerne un groupe de fonctions :
"File" donne acc둺 aux actions
concernant les fichiers, "WS" � celles concernant les workspaces, etc.
Pour acc괺er � une de ces options, il suffit de presser la touche ALT
associ괻 � la premi둹e lettre de l'option (ALT+F donne acc둺 au menu
FILE), ou de presser la souris sur l'option choisie.

Dans le coin sup굍ieur droit de l'괹ran est affich� un mot qui peut
prendre deux valeurs:

&EN INPUT qui indique que le programme attent
&EN WAIT qui indique que le programme est en train d'ex괹uter une
    fonction

&IT Centre de l'괹ran : les donn괻s et les 괹rans de saisie
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
En fonction de l'option s굃ectionn괻 dans les menus, s'afficheront dans le
centre de l'괹ran les donn괻s demand괻s ou un 괹ran de saisie. Dans
l'exemple ci-dessus, l'option "Load Workspace" du menu "Workspace" a 굏�
s굃ectionn괻 et l'괹ran permet de sp괹ifier les fichiers � charger en
m굆oire.

&IT Avant-derni둹e ligne de l'괹ran : aide rapide
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
Dans l'avant-derni둹e ligne de l'괹ran est affich� un message. Celui-ci
est un commentaire indiquant les diff굍entes touches utilisables et leur
fonction ou simplement l'effet de la commande actuellement s굃ectionn괻.

&IT Derni둹e ligne de l'괹ran : message, printer/file, heure
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
La derni둹e ligne de l'괹ran contient, de gauche � droite, les
informations suivantes :

&EN un message indiquant soit une erreur, soit l'굏at d'avancement du
    d굍oulement d'une fonction (simulation, estimation, $show dans un
    rapport, etc)


&EN la destination de l'impression qui peut 늯re soit FILE pour
    rediriger toute impression vers un fichier, soit PRINTER.

&EN l'heure


&TI Les 괹rans de saisie
컴컴컴컴컴컴컴컴컴컴
Le fonctionnement des 괹rans de saisie est assez intuitif. Quelques
informations utiles sont reprises ci-dessous.

&IT Touches sp괹iales
컴컴컴컴컴컴컴컴컴컴�

&EN ~cCtrl+U~C : vide le champ de son contenu
&EN ~cCtrl+C~C : copie la partie s굃ectionn괻 du champ
&EN ~cCtrl+V~C : colle la s굃ection pr괹괺emment copi괻 (dans n'importe quelle application)

&EN ENTER : selon le type de champ, le comportement varie :
&EN2 : champ bouton : ex괹ute l'action associ괻 au bouton
&EN2 : champ DIR : affiche les fichiers r굋ondant au masque du champ
&EN2 : champ EDITOR : entre en 괺ition du texte du champ.
&EN2 : champ MENU : ouvre le menu pour effectuer une s굃ection
&EN F10 : 굌uivaut � presser le bouton Ok de la fen늯re.
&EN ESC ou F3 : 굌uivaut � presser le bouton Escape de la fen늯re.

&IT Champ menu
컴컴컴컴컴
Le menu s'ouvre en pressant la touche ENTER ou en cliquant sur la souris. On
s'y d굋lace avec les touches fl괹h괻s et de sauts de page haut et bas.
L'option s굃ectionn괻 est valid괻 avec la touche ENTER et le texte
correspondant s'inscrit dans la page de saisie. On peut 괾alement
s굃ectionner une option en tapant sa premi둹e lettre ou un espace pour
passer d'option en option.

&IT Champ DIR
컴컴컴컴�
Un champ de type DIR est un champ de saisie permettant la recherche d'un
nom de fichier en fonction d'un masque de saisie. On commence par
encoder dans le champ le masque de fichier exactement comme dans un
champ normal de texte (p.ex. "*.var").

La touche ENTER affiche alors la liste des fichiers correspondant au
masque de saisie. Les sous-r굋ertoires sont 괾alement mentionn굎. Le nom
du fichier d굎ir� peut alors 늯re s굃ectionn� � l'aide des touches
fl괹h괻s et de sauts de pages haut et bas. Le nom choisi est valid� par
la touche ENTER. Ce nom s'inscrit dans la page de saisie � la place du
masque de recherche. Si le nom s굃ectionn� est un nom de r굋ertoire, la
recherche se poursuit dans ce r굋ertoire. Ces champs fonctionnent par
ailleurs comme un champ de texte normal.

Si aucun nom affich� dans le menu ne doit 늯re s굃ectionn�, la touche
ESCAPE permet de quitter le menu sans exporter de nom dans le champ.

Dans le menu, on trouve pour chaque fichier plusieurs informations comme
sa taille, la date de derni둹e modification, ses propri굏굎 d'acc둺, ...

&IT Champ EDITOR
컴컴컴컴컴컴
Les champs EDITOR sont reconnaissables au "scrollbar" qui appara똳 en
g굈굍al � leur gauche. Il sont le plus souvent encadr굎.

Pour entrer en 괺ition d'un champ EDITOR, il faut presser ENTER. On
dispose alors d'un 괺iteur de texte complet, avec fonctions de
copie/collage, formattage de texte, etc. Ce type de champ n'a pas
de limite de taille. Il est en particulier utile pour la saisie
de formules.
Les principales fonctions sont cit괻s ici :

&EN Pour quitter l'괺iteur (et revenir au niveau de la PAGE de saisie)
    il suffit de presser ESCAPE ou Alt-Q.

&EN Pour marquer un bloc de texte (� copier), on utilisera les touches
    Alt-B, Alt-L ou Alt-T selon que le bloc � marquer est un rectangle
    (B), un groupe de lignes (L) ou une partie de texte (T).

&EN Pour copier le bloc marqu� (en vue d'une copie), on
    utilisera ~cCtrl+C.~C Pour coller le contenu du clipboard �
    l'endroit du curseur, on pressera ~cCtrl+V.~C Cette copie peut se
    faire dans un autre texte que le texte courant (copie d'une 굌uation
    � une autre par exemple).

&EN Pour d굏ruire le bloc marqu� et le placer dans le tampon, on
    utilisera ~cCtrl+D.~C

Si on dispose d'une souris, celle-ci offre des facilit굎 pour le
marquage (bouton de gauche) et la fonction copie/collage (bouton de
droite).

&TI Les touches fonctions
컴컴컴컴컴컴컴컴컴컴�

Pour sp괹ifier une action � effectuer, il y a plusieurs fa뇇ns de
proc괺er :

&EN une premi둹e est de presser la touche fonction (F<<n>>, ...)
    ad굌uate,

&EN une deuxi둴e est de presser ENTER ou de cliquer sur le bouton
    d'un 괹ran de saisie (qui correspond toujours � une touche
    fonction),

&EN une troisi둴e enfin est de cliquer avec la souris dans la zone de
    commentaire (avant-derni둹e ligne de l'괹ran) sur le texte de la
    touche correspondant � la fonction d굎ir괻. Le texte de la touche
    se distingue du reste du commentaire par sa couleur.


Les touches fonctions (F1 � F10, Alt+F1 � Alt+F10, etc) sont de trois
types : les touches locales � l'endroit de l'application o� on se
trouve, les touches globales qui s'ex괹utent en interrompant
temporairement l'ex괹ution du processus en cours et les touches
permettant un acc둺 rapide au menu d굍oulant.

&IT Touches fonctions locales
컴컴컴컴컴컴컴컴컴컴컴컴�
Un cas 굒ident o� les touches fonctions locales sont indispensables
est celui des 괹rans de saisie: une fois les donn괻s encod괻s,
l'utilisateur doit pouvoir soit confirmer le traitement � effectuer, soit
d괹ider d'abandonner la page en cours sans ex괹ution. Ce choix est
indiqu� au programme par une touche fonction (en g굈굍al F10
confirme et ESCAPE abandonne).

Toujours dans un 괹ran de saisie, une touche peut permettre d'introduire
des informations compl굆entaires ou de visualiser des donn괻s utiles.
C'est le cas dans l'괹ran d'impression de tableaux : F8 permet
d'imprimer, tandis que F10 affiche � l'괹ran les valeurs du tableau.

En toutes circonstances, les touches fonctions locales sont indiqu괻s
avec l'op굍ation qu'elles d괹lenchent dans la ligne de commentaire de
l'괹ran (avant-derni둹e ligne).

&IT Touches fonctions globales
컴컴컴컴컴컴컴컴컴컴컴컴컴
D'autres touches de fonctions ont un effet global : o� que l'on se
trouve dans l'application, ces touches interrompent le programme et
ex괹utent une action d굏ermin괻. Le processus en cours � l'origine
reprend apr둺 la fin de l'action associ괻 � la touche fonction globale.

Dans le module IODE, la touche fonction globale F9 permet � tout moment,
de voir l'굏at d'utilisation de la m굆oire sans avoir � quitter
l'endroit actuel de l'application.

Toutes les touches fonctions d괽inies sont expos괻s en d굏ail en
fonction du contexte.

&IT Touches fonctions du menu d굍oulant
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
D'autres touches globales sont celles permettant de se positionner
directement dans une fonction du menu d굍oulant. La touche Alt associ괻
� la premi둹e lettre d'une option du menu d굍oulant a pour effet de
quitter la fonction en cours et de positionner l'application sur le
point du menu correspondant (Alt + F = menu File).

Ces touches abandonnent par cons굌uent l'action en cours s'il y en a une.

Dans les sous-menus du menu d굍oulant, on trouve � droite de certaines
options l'indication d'une touche fonction. Le fait de presser la touche
en question abandonne l'action en cours et ex괹ute directement la
fonction indiqu괻 dans le menu.

Par exemple, dans le menu "Misc", la touche Alt-X est associ괻 �
l'option "Quit". Presser Alt + X � n'importe quel moment permet de
quitter le programme.

&TI La souris
컴컴컴컴�
La souris facilite un certain nombre d'op굍ations dans l'utilisation de
IODE :

&EN dans les 괹rans de saisie, un simple clic permet de positionner
le curseur dans le champ voulu, et � la position souhait괻.

&EN sur les contours des fen늯res de saisie, d'aide ou de scroll, des
    petites ic뱊es permettent d'effectuer certaines op굍ations :

&EN2 [o] : fermer la fen늯re (= ESCAPE)
&EN2 [=] : acc둺 au menu de la fen늯re reprenant toutes les touches
	    fonctions d괽inies localement
&EN2 [?] : acc둺 au manuel en ligne
&EN2 d'autres ic뱊es permettent d'effectuer une rotation dans les
    scrolls, un d굋lacement sur les sujets pr괹괺ents ou suivants dans
    les 괹rans d'aide, de maximiser et minimiser la taille de la
    fen늯re, etc.

&EN dans l'괺iteur MMT (rapports, champs EDITOR), elle permet une s굃ection
    de bloc ou un acc둺 � un menu simplifi� pour les op굍ations de copie
    et collage (bouton de droite).

&EN lorsque certaines touches fonctions sont mises en exergue dans la
    zone de commentaire, la souris permet de "presser" ces touches sans
    avoir recours au clavier

&EN les fen늯res (pages, scroll, help, mmt) peuvent 늯re d굋lac괻s
    en pla놹nt la souris sur le contour et en gardant le bouton appuy�
    pendant le d굋lacement

&EN la taille des m늤es fen늯res peut 늯re modifi괻 en pla놹nt la souris
    sur le coin inf굍ieur droit et en "tirant" tout en gardant enfonc�
    le bouton de la souris.

&EN les options du menu d굍oulant sont directement accessibles � l'aide
    de la souris, comme � l'aide des touches Alt-lettre.

&EN dans les objets ayant un scrollbar (MMT, champs EDITOR, Help, Menus,
    Scrolls) la souris permet de placer le curseur � l'endroit souhait�
    par un simple clic.
>











