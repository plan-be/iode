/* FORMATS */

<Formats ASCII des objets>
    Formats ASCII des objets
    컴컴컴컴컴컴컴컴컴컴컴컴

Les formats de fichiers suivants sont d괹rits dans ce chapitre :

&EN <Format ASCII de IODE> : reprend la syntaxe de d괽inition des 7 types
			  d'objets g굍굎 par IODE

&EN <Format DIF de IODE  > : interface (obsol둻e) avec des programmes externes

&EN <Format PRN d'AREMOS > : interface (obsol둻e) avec le logiciel AREMOS
>

#include "ascii.m"

<Le langage A2M>
    Le langage A2M
    컴컴컴컴컴컴컴

&TI D괽inition du langage a2m
    컴컴컴컴컴컴컴컴컴컴컴컴�

Le langage a2m est un langage "tagg�" contenant des instructions de
formatage et de structuration d'un texte, y compris de tableaux et des
graphiques.

Un texte au format A2M contient des caract둹es Ascii du jeu 굏endu accentu�
et 굒entuellement des commandes et des instructions d'enrichissement
typographique. Les instructions et commandes sont toutes optionnelles. Tout
texte ASCII est donc, par d괽inition, au format A2M.

&TI     Historique
    컴컴컴컴컴

Le langage d'interface A2M (ASCII TO MIF) a 굏� d굒elopp� initialement
afin de pouvoir entrer un texte sur un PC, par exemple, en utilisant un
simple 괺iteur ascii, et d'obtenir un texte correctement mis en page sur
FrameMaker, sans intervention au niveau de FrameMaker, via son format d'interface
MIF. Les premi둹es versions du filtre SCR3_A2M effectuaient la
conversion A2M vers MIF.

L'id괻 a ensuite 굏� 굃argie, A2M est devenu un format d'interface
g굈굍� par plusieurs logiciels produits par la cellule informatique du
Bureau du Plan (IODE, Matrix, Lex, ...). Un format de
tableaux et de graphiques a 굏� impl굆ent�. Des modules ont 굏�
d굒elopp굎 pour transformer le format A2M en d'autres formats d'괹hange
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
컴컴컴컴컴�

La suite de ce chapitre d괹rit dans le d굏ail la <Syntaxe des fichiers a2m>.

&TI Convertisseurs A2M
컴컴컴컴컴컴컴컴컴컴컴
IODE g굈굍e des formats A2m et permet de les convertir. <Le programme a2m>
permet d'effectuer les conversions plus quelques autres fonctions sur les
fichier html.


>

<Format DIF de IODE>
    Formats DIF de IODE
    컴컴컴컴컴컴컴컴컴�

Deux formats DIF diff굍ents sont exploitables via XODE. Le premier
concerne n'importe quel programme (LOTUS, EXCEL, etc) capable de fournir
un output DIF. Le format du tableau export� en DIF doit 굒idemment
r굋ondre � des r둮les, sans quoi XODE ne saurait comment interpr굏er le
contenu. La premi둹e ligne doit contenir les p굍iodes, les suivantes le
code d'une s굍ie suivie des valeurs.

On trouve donc ci-dessous une description du tableau comme il est vu �
l'괹ran en LOTUS par exemple.


&TI DIF (tel que visible en 123)
컴컴컴컴컴컴컴컴컴컴컴컴컴컴
&CO

    CODE 1980Y1 1981Y1 1982Y1 1983Y1 ....
    A    1      1       2      4
    B    1      1       2      5
    ...
&TX

Le second format est g굈굍� par le logiciel de gestion de tableaux
de Belgostat et se pr굎ente de la fa뇇n suivante en LOTUS lorsqu'une
exportation est effectu괻 :


&TI Belgostat DIF (tel que visible en 123)
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

&CO
123456789012345 1       1980    1             456900     1       1
(series nb)     (freq)  (year)  (per in year) (value)  (status)  (free)
   ........
&TX
>


<Format PRN d'AREMOS >
    Format PRN d'AREMOS
    컴컴컴컴컴컴컴컴컴�

Ce format assure l'interface avec le logiciel AREMOS.

Le fichier de variables se pr굎ente comme une r굋굏ition de lignes au
format suivant:

&CO
	"VARNAME" val1 val2 ....
	...
&TX

>





