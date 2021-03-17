/* mmt.m */
<L'‚diteur MMT>
    L'‚diteur MMT
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ

Ce chapitre d‚crit les fonctionnalit‚s de l'‚diteur MMT. Cet ‚diteur est
utilis‚ dans IODE pour l'‚dition des fichiers (rapports, fichiers ASCII,
fichiers A2M, etc).

C'est ‚galement MMT qui est exploit‚ pour la saisie des ‚quations et de
fa‡on plus g‚n‚rale pour tous les champs de taille variable dans les
‚crans de IODE.

Ce chapitre se compose des sections suivantes :

&EN <G‚n‚ralit‚s sur MMT>
&EN <Fonctionnalit‚s de MMT>
&EN <La commande de lancement de MMT>
&EN <Les touches de fonction de MMT>
&EN <La souris dans MMT>
&EN <Les champs EDITOR>
>

<G‚n‚ralit‚s sur MMT>
    G‚n‚ralit‚s sur MMT
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

L'‚diteur MMT est un ‚diteur ASCII qui permet d'‚diter simultan‚ment
plusieurs textes dans plusieurs fenˆtres visibles … l'‚cran. Il a ‚t‚
d‚velopp‚ par et pour des d‚veloppeurs en vue de rendre performantes les
op‚rations d'‚dition et de modification des fichiers de code source.

Il gŠre une souris compatible Microsoft sans n‚cessit‚ d'installer un
driver de souris.

Il gŠre en outre (en DOS) la m‚moire ‚tendue (type EMS) si elle est
pr‚sente, ainsi que le swapping sur disque si n‚cessaire. Des textes
ASCII de grandes tailles peuvent donc ˆtre charg‚s.

MMT est d‚velopp‚ en C, sous DOS et UNIX, en utilisant les librairies du
4GL SCR/AL1. MMT fait partie int‚grante de SCR/AL1.

Auteurs: Jean-Marc Paul et Hubert Castelain, Cellule Informatique du
Bureau du Plan.

Mars 1993.
>


<Fonctionnalit‚s de MMT>
    Fonctionnalit‚s de MMT
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

L'‚diteur MMT offre les principales fonctionnalit‚s suivantes:

&EN L'‚diteur est pilot‚ soit avec la souris soit avec des touches de
fonction.

&EN Copy, Cut and Paste d'un bloc (rectangle, lignes ou texte) au sein
    d'un texte et d'un texte … l'autre. Ces fonctions sont ‚tendues …
    tous les champs EDITOR d'une application SCR/AL1 : copy, cut and
    paste d'un champ … l'autre, d'un champ vers un texte et d'un texte
    vers un champ. Un "clipboard" permet de visualiser … tout endroit le
    bloc en cours de transfert.

&EN Gestion de fenˆtre: les textes peuvent se pr‚senter en cascade, en
    fenˆtres juxtapos‚es ou en plein ‚cran. Les fenˆtres peuvent ˆtre
    d‚plac‚es et chang‚es de tailles, ou encore "zoom‚es", tout cela avec
    la souris ou le clavier..

&EN Gestion de la souris: marquage de bloc, d‚placement du curseur,
    d‚placement du "focus" sur un autre texte, copy-cut-paste d'un bloc
    marqu‚, etc... Le driver de souris utilis‚ est le driver interne de
    SCR/AL1 qui r‚pond … une souris compatible Microsoft.

&EN Edition de texte: outre le marquage et le d‚placement de blocs de
    texte (en insersion ou en superposition), les principales fonctions
    suivantes sont actives: recherche d'un texte, remplacement d'un
    texte, remplissage d'un bloc, d‚placement dans tout l'‚cran,
    reconstruction de ligne, visualisation de background, suppression ou
    insertion de lignes, effacement de d‚but ou fin de ligne, ...

&EN Trac‚ de cadres (caractŠres graphiques ou non)

&EN Aide en ligne, "screen save" automatique, etc...
>

<La commande de lancement de MMT>
    La commande de lancement de MMT
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

La commande suivante permet d'‚diter (ou de cr‚er s'il est inexistant) un
ou plusieurs (100 maximum) texte(s) avec MMT :

&CO
    MMT file1 [file2 file3 ...]
&TX

Les touches de fonction ALT + Q et ALT + X permettent de quitter le
programme.

Si plusieurs textes sont ouverts, le texte actif est celui dont le titre
est "r‚tro-‚clair‚".
>

<Les touches de fonction de MMT>
    Les touches de fonction de MMT
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&TI Remarques:

&EN les touches de fonctions sont susceptibles d'ˆtre modifi‚es au
    gr‚ des versions de MMT. Un r‚sum‚ des touches de fonctions est
    accessible en cours d'ex‚cution de MMT soit via la touche F1 soit
    via le menu dynamique du programme (ALT + M)

&EN toutes les fonctions sont accessibles dans mmt par menu.
    Les menus sont activ‚s par la touche ALT+M.

&EN dans le cas des champs EDITOR d'une application, les touches de
    l'application ont priorit‚ sur les touches de fonction de MMT. Dans
    ce cas, il est n‚cessaire d'acc‚der aux fonctionnalit‚s de MMT via
    les menus ou le bouton droit de la souris.

&EN sauf quelques exceptions, les touches de l'‚diteur MMT sont
    compatibles avec celles de l'‚diteur MT.

Dans les paragraphes qui suivent, les abr‚viations suivantes sont
utilis‚es:

&EN A_  touche ALT enfonc‚e
&EN C_  touche CTRL enfonc‚e
&EN S_  touche SHIFT enfonc‚e
&EN CSR touche curseur


Les touches de fonction sont group‚es par sujets dans les sections qui
suivent :

&EN <Ouvrir/quitter/sauver>
&EN <D‚placement dans le texte>
&EN <Op‚rations de bloc>
&EN <Op‚rations sur une ligne>
&EN <Op‚rations sur le texte>
&EN <Gestion du multi-fenˆtrage>
&EN <Touches diverses>
&EN <R‚sum‚ des touches de fonction>

La touche F1 affiche ce pr‚sent manuel en plus d'un r‚sum‚ des touches
fonctions utiles.
>

<Ouvrir/quitter/sauver>
    Ouvrir/quitter/sauver
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&EN F4      charger un fichier
&EN A_F1    sauver et quitter le fichier courant
&EN A_F2    sauver tous les textes en ‚dition
&EN A_F3    changer le nom du fichier
&EN A_Q     quitter le texte courant
&EN A_X     quitter tous les textes
&EN A_W     sauver le fichier courant
&EN ESCAPE  quitter le texte courant

>

<D‚placement dans le texte>
    D‚placement dans le texte
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&EN CSR_LEFT        curseur … gauche
&EN CSR_RIGHT       curseur … droite
&EN CSR_UP          curseur au dessus
&EN CSR_DOWN        curseur en dessous
&EN CSR_HOME        curseur en d‚but de ligne
&EN CSR_END         curseur en fin de ligne
&EN CSR_PG_UP       curseur un ‚cran plus haut
&EN CSR_PG_DN       curseur un ‚cran plus bas
&EN C_CSR_HOME      curseur en d‚but de texte
&EN C_CSR_END       curseur en fin de texte
&EN C_CSR_LEFT      curseur sur le mot pr‚c‚dent
&EN C_CSR_RIGHT     curseur sur le mot suivant
&EN TAB             curseur quatre caractŠres plus loin
&EN INSERT          mode insertion/remplacement
&EN DELETE          effacer le caractŠre courant
&EN BACKSPACE       effacer le caractŠre pr‚c‚dent
&EN C_BACKSPACE     effacer le mot courant
&EN ENTER           passer … la ligne suivante
&EN Shift-F1        marque la position courante (drapeau)
&EN Shift-F2        retourne au dernier drapeau

>

<Op‚rations de bloc>
    Op‚rations de bloc
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&EN C_Y   Copy to clipboard
&EN C_P   Paste clipboard
&EN A_O   Overlay clipboard
&EN A_C   Copy block
&EN A_D   Delete (cut) block
&EN A_B   Mark Block
&EN A_L   Mark Lines
&EN A_T   Mark Text
&EN A_U   Unmark block
&EN A_S   Show clipboard
&EN A_F5  Lower block
&EN A_F6  Upper block
&EN A_F7  Fill block
&EN F3    Outline block (box)
&EN F5    Shift left (lines)
&EN F6    Shift right (lines)
>

<Op‚rations sur une ligne>
    Op‚rations sur une ligne
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&EN A_G   Goto line
&EN A_I   Insert line
&EN A_J   Join next line
&EN A_K   Kill cur.line
&EN A_N   New line(split)
&EN A_Z   Del.end of line
&EN C_J   Insert Line
&EN C_K   Del end of line
&EN C_U   Del.beg.of line

>

<Op‚rations sur le texte>
    Op‚rations sur le texte
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&EN C_F,  Find text
&EN C_R,  Replace text
&EN F2,   Underline text
&EN A_I,  Insert line
&EN A_K,  Kill line
&EN A_N,  New line (split)
&EN A_J,  Join line
&EN A_G,  Go to line
&EN A_Z,  Delete end of line
&EN C_U,  Delete beg.of line
&EN C_F8, Right margin
&EN C_F9, Activate margin
&EN A_P,  Reflow paragraph

>

<Gestion du multi-fenˆtrage>
    Gestion du multi-fenˆtrage
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&EN A_F8,    Cascade
&EN A_F9,    Little box
&EN A_F10,   Full screen
&EN C_Z,     Zoom text
&EN A_E,     Edit next text
&EN A_S,     Show clipboard
&EN C_V,     Redisplay
&EN F7,      Move box
&EN F8,      Resize box
&EN C_B,     Show dos screen
&EN C_A,     Screen colors

>

<Touches diverses>
    Touches diverses
    ÄÄÄÄÄÄ

&EN F1,   MMT on line help
&EN F10,  SCR4/AL1 topics
&EN F7 ,  Edition d'un hypertext
&EN C_F1, System Shell
&EN C_B,  Show dos screen
&EN C_A,  Screen colors
&EN A_A,  About MMT
&EN A_M,  Menu d‚roulant de MMT
>

<R‚sum‚ des touches de fonction>
    R‚sum‚ des touches de fonction
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les touches de fonctions sont susceptibles d'ˆtre modifi‚es au gr‚ des
versions de MMT. Un r‚sum‚ des touches de fonctions est accessible en
cours d'ex‚cution de MMT soit via la touche F1 soit via le menu dynamique
du programme (ALT + M).
>

<La souris dans MMT>
    La souris dans MMT
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les op‚rations g‚r‚es par la souris sont les suivantes:

Le bouton de gauche:

&EN <La souris dans le texte>
&EN <La souris sur le scrollbar>
&EN <La souris sur le cadre>

&EN <Le bouton de droite de la souris>
>

<La souris dans le texte>
    La souris dans le texte
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Dans le texte, la souris permet de:

&EN Se positionner … un endroit donn‚: "clic" … cet endroit
&EN Marquer un bloc: "clic" au d‚but de bloc, tirer en laissant le bouton
		     enfonc‚.

>

<La souris sur le scrollbar>
    La souris sur le scrollbar
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

On peut soit:

&EN Se positionner … un endroit du texte: "clic" dans le scrollbar
&EN Faire d‚filer le texte: "clic" dans le scrollbar, tirer en laissant le
			    bouton
&EN Faire d‚filer le texte ligne par ligne vers le haut ou vers le bas:
"clic" sur les flŠches haut et bas du scrollbar.

>
<La souris sur le cadre>
    La souris sur le cadre
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Le cadre d'une fenˆtre contient des caractŠres sp‚ciaux entre crochets :
un carr‚ en haut … gauche du cadre, une liste … c“t‚ du carr‚ , une
double flŠche en haut … droite du cadre et un double coin en bas …
droite du cadre.

Un clic sur ces caractŠres sp‚ciaux ou sur le cadre aura les
cons‚quences suivantes:

&EN Le carr‚ en haut … gauche: quitter le texte courant
&EN La liste : affichage d'un menu principal
&EN La double flŠche : zoom
&EN Le point d'interrogation: aide en ligne
&EN Le coin inf‚rieur droit : modification de la taille de la fenˆtre
    (tirer le coin avec la souris en laissant le bouton enfonc‚). &EN Le
    cadre : d‚placement de la fenˆtre (tirer le cadre avec la souris en
    laissant le bouton enfonc‚).
>

<Le bouton de droite de la souris>
    Le bouton de droite de la souris
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Le bouton de droite affiche un menu qui offre les options suivantes:

&EN CUT (to clipboard)     ‚quivallent … la touche de fonction (a-D)
&EN COPY (to clipboard)    ‚quivallent … la touche de fonction (c-Y)
&EN PASTE (from clipboard) ‚quivallent … la touche de fonction (c-P)
&EN SHOW clipboard         ‚quivallent … la touche de fonction (a-S)
&EN UnMark                 ‚quivallent … la touche de fonction (a-U)
&EN Mark Block             ‚quivallent … la touche de fonction (a-B)
&EN Mark Lines             ‚quivallent … la touche de fonction (a-L)
&EN Mark Text              ‚quivallent … la touche de fonction (a-T)
&EN Fill block             ‚quivallent … la touche de fonction (a-F7)
&EN Outline block          ‚quivallent … la touche de fonction (F3)


>

<Les champs EDITOR>
    Les champs EDITOR
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les champs de saisie EDITOR sont en r‚alit‚ des textes MMT. L'‚dition
des ces champs offre donc toutes les possibilit‚s de l'‚dition d'un
texte MMT. Toutes les fonction y sont actives sauf: F4 (chargement d'un
fichier) et les op‚rations de d‚placement et de modification de taille
de la fenˆtre (le zoom reste cependant actif).

En particulier, les champs EDITOR supportent toutes les fonctions de
copie via le clipboard. Le clipboard ‚tant connu de l'ensemble de
l'application SCR en cours d'exploitation, il est possible de copier un
champ dans un autre, un texte MMT dans un champ, un champ dans un texte,
etc...

&NO dans le cas des champs EDITOR d'une application, les touches de
l'application ont priorit‚ sur les touches de fonction de MMT. Dans ce
cas, il est n‚cessaire d'acc‚der aux fonctionnalit‚s de MMT via les
menus ou le bouton droit de la souris.
>




