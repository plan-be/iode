/* A2M */

LINE 2 COL 1 NL 20 NC 78
START

..esc ~

#include "macros.h"

<Le programme A2M>
    Le programme A2M
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

..esc ~

.~a'a2m.gif'

Le programme ~ca2m~C convertit des textes ASCII comprenant ou non des commandes
A2M en divers autres formats et permet de lancer l'impression de
fichiers sur divers types d'imprimantes selon des proc‚dures d‚finies
par l'utilisateur.

&EN <Introduction>
&EN <L'interface utilisateur> : le panneau principal
&EN <Le menu File>            : quitter l'application
&EN <Le menu Options>         : les options de conversion
&EN <Le menu HTML Tools>      : op‚rations sur des fichiers HTML
&EN <Le menu Help>            : l'aide en ligne

>


<Introduction>
    Introduction
    ÄÄÄÄÄÄÄÄÄÄÄÄ

Les commandes A2M sont des commandes de formatage de paragraphes ou de
textes, elle permettent ‚galement d'inclure et de formater des graphiques,
des tableaux, de g‚rer les couleurs du document, etc...

Un fichier en format A2M peut ˆtre traduit en diff‚rents formats :

&EN GDI, format d'impression directe sous Windows
&EN RTF, interpr‚table par Word, WordPerfect, etc
&EN MIF, langage structur‚ des documents Frame Maker
&EN RTF-HLP, source d'un fichier d'aide de Windows 95
&EN HTML, format des documents Internet (Web)
&EN CSV, comma separated value

Selon la destination de l'impression, des tags ou commandes seront soit
interpr‚t‚s diff‚remment, soit inop‚rants. Ainsi par exemple, les images ne
peuvent ˆtre incorpor‚es dans des fichiers ASCII ou les sujets d'aide n'ont
de signification que dans le cadre des fichiers d'aide.

>


<L'interface utilisateur>
    L'interface utilisateur
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

L'interface utilisateur comprend, outre les ‚l‚ments classiques d'une
fenˆtre Windows:

&EN le menu d‚roulant permetant notamment de d‚finir les options de
conversion. Les points du menu sont d‚crite ci-aprŠs.

&EN le bouton "Translate" qui lance la conversion du document A2M

&EN le bouton "Exit" qui permet de quitter l'application

&EN le bouton "Help" qui donne accŠs … l'aide en ligne

&EN le menu d‚roulant "Destination" qui permet de d‚finir le type de
conversion … op‚rer sur le document A2M. Un clic dans la flŠche vers le bas
de ce champ donne la liste des destinations possibles.

&EN le champ "Input file" qui doit contenir le nom du fichier … convertir.
Le bouton "..." … droite du champ permet de parcourir le systŠme de fichier
afin de localiser ce fichier si n‚cessaire.

&EN le champ "Destination file" qui doit contenir (sauf dans le cas d'une
impression directe sur une imprimante windows) le nom du fichier de
destination, r‚sultat de la conversion. Le bouton "..." … droite du champ
permet de parcourir le systŠme de fichier afin de pr‚ciser la localisation
de ce fichier.

Les touches de fonction F10, Esc et F1 permettent respectivement de
lancer la conversion, de quitter l'application ou d'appeler l'aide en ligne
de l'application. D'autres touches de fonction sont ‚ventuellement actives
dans les autres pages, leur liste apparait en commentaire en bas de fenˆtre.

Pendant la conversion, un message en bas de fenˆtre informe l'utilisateur du
d‚roulement des op‚rations (nombre d'objet lus et interpr‚t‚s, nombre de
pages imprim‚es).

Enfin, le programme peut ˆtre lanc‚ en cliquant sur un fichier A2M identifi‚
par l'icone appropri‚e … l'aide de l'exploreur de Windows. Dans ce cas, le
nom du fichier s‚lectionn‚ est automatiquement import‚ dans le champ "Input
File" de l'application.

>


<Le menu File>
    Le menu File
    ÄÄÄÄÄÄÄÄÄÄÄÄ

Le menu File permet de quitter l'application. On peut quitter l'application
‚galement avec le bouton "Exit" du panneau principal ou en tapant
simultan‚ment les touches ALT et X en n'importe quel endroit du programme.

>
<Le menu Options>
    Le menu Options
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

.~a'a2mopt.gif'

Le menu option donne accŠs aux paramŠtres de d‚finition du fichier A2M
d'origine (panneau "General") ainsi qu'aux paramŠtres pouvant modifer le
r‚sultat final de la conversion en diff‚rents formats (imprimante, MIF, RTF,
HTML, ....).

Les options standard sont correctes dans la grandes majorit‚ des cas, et
l'utilisateur n'a pas en principe … les modifier.

Les modifications sont conserv‚es lors des utilisations ult‚rieures du
programme.

Les paramŠtres suivants sont accessibles:

&EN <la fiche "General">        : structure du fichier A2M d'origine
&EN <la fiche "Windows printer">: particularit‚s d'une impression
&EN <la fiche "MIF">            : output dans un fichier MIF
&EN <la fiche "RTF">            : output dans un fichier RTF
&EN <la fiche "HTML">           : output dans un fichier HTML
&EN <la fiche "CSV">            : output dans un fichier CSV


>

<la fiche "General">
    La fiche "General"
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
.~a'a2mgnl.png'

Nous renvoyons au manuel de r‚f‚rence pour les d‚tails de la syntaxe de A2M.

&EN Le caractŠre d'‚chappement (Escape character) permet de d‚finir le
caractŠre qui d‚bute une s‚quence d'attribut typographiques, d'insertion
d'objet ou d'hyperlien dans la suite du texte. Par d‚faut, ce caractŠre est
le BACKSLASH (\). La commande ".esc" dans le fichier A2M permet de modifier
cette option.

&EN Le caractŠre de commande est celui qui annonce, en d‚but de ligne, une
commande de mise en page du document.

&EN Le caractŠre de d‚finition est le caractŠre de d‚but d'une macro
(define). Ce caractŠre est interpr‚t‚ uniquement si le mot qui le suit est
une macro. Si ce n'est pas le cas, il est imprim‚ comme tel.

>

<la fiche "Windows printer">
    la fiche "Windows printer"
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

La fiche permet de configurer certaines options de filtrages appliqu‚es sur
le fichier A2M avant l'envoi sur le pilote d'impression de Windows.

.~a'a2mwin.gif'

Pour le texte, l'utilisateur peut choisir de conserver les attributs de
couleur, la num‚rotation des paragraphes, la police et la taille du
caractŠre et l'incr‚ment utilis‚ par la commande d'attribut typographique S.

Pour la page, l'utilisateur peut d‚finir les marges et les r‚f‚rences de
haut et de bas de page.

Pour les tableaux et les graphiques, l'utilisateur peut choisir diverses
options de d‚coupage et de mise en page.


>
<la fiche "CSV">
    La fiche "CSV"
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

La fiche CSV permet de paramŠtrer la conversion de tableaux en format A2M en
format CSV (comma saparated value).

.~a'a2mcsv.gif'

On peut d‚finir le s‚parateur de cellules, le caractŠre "quote" et le
remplacement ‚ventuel du contenu "Not Available".


>
<la fiche "MIF">
    La fiche "MIF"
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

La fiche permet de configurer certaines options de filtrages appliqu‚es sur
le fichier A2M lors de la transformation en fichier MIF (Maker Interchange
Format).

.~a'a2mmif.gif'

L'utilisateur peut choisir la police et la taille des caractŠres … utiliser
dans le texte et les tableaux.


Pour les tableaux l'utilisateur peut choisir diverses
options de d‚coupage et de formatage (taille des colonnes et quadrillage).

On peut enfin inclure ou non l'ancrage d'images (commande .a dans le fichier
A2M).

>
<la fiche "RTF">
    La fiche "RTF"
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

La fiche permet de configurer certaines options de filtrages appliqu‚es sur
le fichier A2M lors de la transformation en fichier RTF (Rich Text Format).

.~a'a2mrtf.gif'

Pour le texte, l'utilisateur peut choisir de conserver la num‚rotation des
paragraphes, la police et la taille du caractŠre et l'incr‚ment utilis‚ par
la commande d'attribut typographique S.

Pour les tableaux, l'utilisateur peut choisir diverses
options de d‚coupage et de mise en page (taille des colonnes et
quadrillage).

Enfin, certaines options peuvent ˆtre d‚finies en vue de la pr‚paration d'un
fichier prˆt … ˆtre compil‚ par compilateur d'aide de Windows (Help Compiler
for Windows).



>
<la fiche "HTML">
    La fiche "HTML"
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

La fiche permet de configurer certaines options de filtrages appliqu‚es sur
le fichier A2M lors de la transformation en fichier HTML (HyperText Markup
Language, version 2.0 standardis‚e par la note RFC1866).

.~a'a2mhtm.gif'

L'utilisateur peut choisir la police et la taille des caractŠres … utiliser
dans le texte et les tableaux.


Pour les tableaux l'utilisateur peut choisir un filtrage couleur et
l'‚paisseur de l'encadrement.

Enfin, l'utilisateur peut sp‚cifier diverses options propres au langage
HTML comme un titre ou des options paticuliŠres … la commande BODY.


>


<Le menu HTML Tools>
    Le menu HTML Tools
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

.~a'a2mhtml.gif'

Le menu HTML Tools donne accŠs … une s‚rie d'outils permettant de modifier
un fichier HTML aprŠs sa cr‚ation:

&EN <Split HTML file>           : d‚coupe un fichier HTML
&EN <Generate Table of Contents>: g‚nŠre une table des matiŠres
&EN <Replace a HTML section>    : remplace une section de fichier

Les r‚f‚rences internes dans le fichier et entre ‚l‚ments des nouveaux
fichiers sont conh‚rentes.


>
<Split HTML file>
    Split HTML file
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Ce programme d‚coupe un fichier HTML en sous fichiers. Les d‚coupes
ont lieu sur les tags Hn qui indiquent les niveaux de chapitres
dans les fichiers HTML.

A partir d'un fichier unique, ‚ventuellement de grande taille, un
nombre quelconque de fichiers sont g‚n‚r‚s, selon le niveau de d‚coupe
demand‚.

Le titre de chaque sous-fichier est repris dans le fichier de niveau
sup‚rieur avec le lien vers le sous-fichier.


Les noms des fichiers g‚n‚r‚s sont constitu‚s en ajoutant un
num‚ro d'ordre … la racine du fichier output.

La page permet de sp‚cifier les paramŠtres utiles … la d‚coupe:

&EN Le nom du fichier source et
&EN le niveau n de la d‚coupe.
&EN La racine du nom du fichier output
&EN Le titre, l'image du background et l'icone de retour … la page
    pr‚cedente.
>

<Generate Table of Contents>
    Generate Table of Contents
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Ce programme analyse un fichier HTML et ses sous fichiers. Il g‚n‚re
simultan‚ment une table des matiŠres sur base des tags <<Hn>> (qui
indiquent les niveaux de chapitres dans les fichiers HTML).

Le titre de chaque chapitre est repris avec le lien vers le
sous-fichier et si un tag NAME est pr‚sent, vers la position
dans ce sous-fichier.

Le fichier r‚sultat n'est pas un fichier HTML complet dans la mesure o—
il est par la suite destinn‚ … ˆtre int‚gr‚ dans un fichier plus vaste.

Le fichier d‚bute par un tag sp‚cial !STARTTOC et se termine par
un autre: !ENDTOC. Ces deux tags pourront par la suite servir
d'indication au sous-programme de substitution (voir "Replace a HTML
section").

.~a'a2mhtm2.gif'

La page permet de sp‚cifier le fichier source, le fichier r‚sultat et le
niveau de chapitre … inclure dans la table des matiŠres.


>
<Replace a HTML section>
    Replace a HTML section
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Ce programme recherche deux tags sp‚ciaux (from TAG et to TAG) dans un
fichier et remplace tout ce qui se trouve entre ces deux tags par le contenu
d'un autre fichier.

Cette fonction est particuliŠrement utile dans le cas des fichiers HTML:
elle permet par exemple de remplacer en une seule op‚ration les barres
de navigation dans tous les fichiers d'un site, ou d'inclure une table
des matiŠres (!STARTTOC et !ENDTOC).


.~a'a2mhtm3.gif'

La page permet de d‚finir les tags de d‚but et de fin de substitution (par
d‚faut !STARTHEADER et !STOPHEADER), le fichier contenant le texte …
substituer et le fichier o— doit s'op‚rer la substitution. Il est possible
(et recommand‚) de sauvegarder le texte d'origine (backup) et de parcourir
r‚curcivement un arbre de r‚pertoires.

>




<Le menu Help>
    Le menu Help
    ÄÄÄÄÄÄÄÄÄÄÄÄ

Ce point de la barre d'action permet d'appeler, au mˆme titre que la touche
F1 ou le bouton Help, une aide conceptuelle en ligne avec table des matiŠres
et index.

>
















