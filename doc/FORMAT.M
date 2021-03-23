/* FORMATS */

<Formats ASCII des objets>
    Formats ASCII des objets
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les formats de fichiers suivants sont d‚crits dans ce chapitre :

&EN <Format ASCII de IODE> : reprend la syntaxe de d‚finition des 7 types
			  d'objets g‚r‚s par IODE

&EN <Format DIF de IODE  > : interface (obsolŠte) avec des programmes externes

&EN <Format PRN d'AREMOS > : interface (obsolŠte) avec le logiciel AREMOS
>

#include "ascii.m"

<Le langage A2M>
    Le langage A2M
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&TI D‚finition du langage a2m
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Le langage a2m est un langage "tagg‚" contenant des instructions de
formatage et de structuration d'un texte, y compris de tableaux et des
graphiques.

Un texte au format A2M contient des caractŠres Ascii du jeu ‚tendu accentu‚
et ‚ventuellement des commandes et des instructions d'enrichissement
typographique. Les instructions et commandes sont toutes optionnelles. Tout
texte ASCII est donc, par d‚finition, au format A2M.

&TI     Historique
    ÄÄÄÄÄÄÄÄÄÄ

Le langage d'interface A2M (ASCII TO MIF) a ‚t‚ d‚velopp‚ initialement
afin de pouvoir entrer un texte sur un PC, par exemple, en utilisant un
simple ‚diteur ascii, et d'obtenir un texte correctement mis en page sur
FrameMaker, sans intervention au niveau de FrameMaker, via son format d'interface
MIF. Les premiŠres versions du filtre SCR3_A2M effectuaient la
conversion A2M vers MIF.

L'id‚e a ensuite ‚t‚ ‚largie, A2M est devenu un format d'interface
g‚n‚r‚ par plusieurs logiciels produits par la cellule informatique du
Bureau du Plan (IODE, Matrix, Lex, ...). Un format de
tableaux et de graphiques a ‚t‚ impl‚ment‚. Des modules ont ‚t‚
d‚velopp‚s pour transformer le format A2M en d'autres formats d'‚change
(RTF pour WinWord par exemple, format HtmlHelp...) ou directement en format d'impression pour
imprimantes Windows.

&CO
		     |                   | MIF
		     |                   | RTF
	     Editeur |--->>   A2M   ---->> | CSV
	     IODE    |                   | Imprimantes
	     Lex     |                   | HTML
		     |                   | ASCII
		     |                   | ...
&TX


&TI Syntaxe
ÄÄÄÄÄÄÄÄÄÄÄ

La suite de ce chapitre d‚crit dans le d‚tail la <Syntaxe des fichiers a2m>.

&TI Convertisseurs A2M
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
IODE g‚n‚re des formats A2m et permet de les convertir. <Le programme a2m>
permet d'effectuer les conversions plus quelques autres fonctions sur les
fichier html.


>

<Format DIF de IODE>
    Formats DIF de IODE
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Deux formats DIF diff‚rents sont exploitables via XODE. Le premier
concerne n'importe quel programme (LOTUS, EXCEL, etc) capable de fournir
un output DIF. Le format du tableau export‚ en DIF doit ‚videmment
r‚pondre … des rŠgles, sans quoi XODE ne saurait comment interpr‚ter le
contenu. La premiŠre ligne doit contenir les p‚riodes, les suivantes le
code d'une s‚rie suivie des valeurs.

On trouve donc ci-dessous une description du tableau comme il est vu …
l'‚cran en LOTUS par exemple.


&TI DIF (tel que visible en 123)
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&CO

    CODE 1980Y1 1981Y1 1982Y1 1983Y1 ....
    A    1      1       2      4
    B    1      1       2      5
    ...
&TX

Le second format est g‚n‚r‚ par le logiciel de gestion de tableaux
de Belgostat et se pr‚sente de la fa‡on suivante en LOTUS lorsqu'une
exportation est effectu‚e :


&TI Belgostat DIF (tel que visible en 123)
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&CO
123456789012345 1       1980    1             456900     1       1
(series nb)     (freq)  (year)  (per in year) (value)  (status)  (free)
   ........
&TX
>


<Format PRN d'AREMOS >
    Format PRN d'AREMOS
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Ce format assure l'interface avec le logiciel AREMOS.

Le fichier de variables se pr‚sente comme une r‚p‚tition de lignes au
format suivant:

&CO
	"VARNAME" val1 val2 ....
	...
&TX

>





