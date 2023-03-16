/* File */

<File>
   File
   ÄÄÄÄ

.~a'mfile.png'

Les fonctions d'importation et d'exportation de donn‚es en format externe …
IODE sont regroup‚es dans ce menu.



Ce menu permet ‚galement de g‚rer les fichiers de "profils" qui retiennent
les derniŠres valeurs introduites dans les champs des pages de saisie.

Ce paragraphe d‚crit les diff‚rentes manipulations de fichiers accessibles
au d‚part du menu FILE de IODE:

&EN <Import       > : Importation de donn‚es en format externe
&EN <Export       > : Exportation de donn‚es en format externe
&EN <Print Setup  > : S‚lection des options d'impression
&EN <Load profile > : Charger un fichier "profile"
&EN <Save current > : Sauver le "profile" courant
&EN <Reset profile> : R‚initialiser les valeurs par d‚faut du "profile"
		    courant
&EN <Exit (Alt-X) > : Quitter IODE

D'autres manipulations sur les fichiers de donn‚es sont ‚galement possibles
… partir du menu ~iWorkspace~I.
>

<Import>
    Import
    ÄÄÄÄÄÄ

~a'pxmimpr.png'

Cette fonction concerne les importations d'objets, plus particuliŠrement
des VARIABLES en diff‚rents formats actuellement moins utilis‚s comme DIF,
ASCII, rotated ASCII, DIF Belgostat, TXT Belgostat et PRN Aremos.

La description des formats qui peuvent ˆtre interpr‚t‚s par cette
fonction peut ˆtre trouv‚e dans les annexes.

Le processus n'a pas d'effet sur les objets actuellement en WS : le r‚sultat d'une importation
se trouve dans un fichier IODE.

&TI Transcodage de noms de s‚ries
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Un usage particulier de cette fonction d'importation est de
d‚finir des rŠgles de transcodage des noms de s‚ries. Ainsi, une s‚rie
dont le code est par exemple ~c123456789~C dans le fichier ASCII … importer
peut ˆtre remplac‚e par n'importe quel nom au choix de l'utilisateur

Les champs de la page sont d‚crits ci-dessous.

&IT Input type
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Ce champ permet de sp‚cifier le type d'objet … importer.

&IT Input format
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les formats d'importation suivants
peuvent ˆtre interpr‚t‚s par IODE:

&EN Ascii : format Ascii des objets sp‚cifique … IODE
&EN Rotated Ascii : format ascii de variables avec les s‚ries en colonnes
&EN DIF : format DIF (Data Interchange Format)
&EN DIF Belgostat : format (ancien) d'‚change sp‚cifique … Belgostat
&EN NIS : format Ascii (ancien) de l'Institut National de Statistiques
&EN GEM : format Ascii du logiciel Chronos
&EN PRN-Aremos : format Ascii du logiciel Aremos
&EN TXT Belgostat : format (ancien) d'‚change sp‚cifique … Belgostat

&IT Input file
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Ce champ est du type DIR, il sert … pr‚ciser le nom du fichier de
variables … importer.

&IT Rule file
ÄÄÄÄÄÄÄÄÄÄÄÄÄ

Champ DIR optionnel qui sert … pr‚ciser le nom du fichier de rŠgles de
transcodage … utiliser.

&IT Save in IODE file
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Ce champ DIR sert … pr‚ciser le nom du fichier de destination des objets
import‚s.

&IT Output Sample
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Ces deux champs permettent de limiter la p‚riode des variables
… importer (optionnel). Le format est celui d'un sample de IODE. Par
exemple:
&CO
    1980Y1
    1990M11
&TX

&IT Output Language
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Ce champ ne sert que dans le cas o— un texte apparaŒt en plusieurs
langues dans un fichier de commentaires. Actuellement, seul le format
DIF Belgostat exploite cette valeur et permet de choisir la langue
des commentaires extraits.

&IT Debug File
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Ce dernier champ permet d'indiquer un fichier de
trace des op‚rations de transcodage effectu‚es (optionnel).

&TI D‚finition des Rules
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les rŠgles ("rules") sont d‚finies dans un fichier externes et
permettent essentiellement deux choses :

&EN limiter les variables … importer
&EN d‚terminer un nom d'objet … partir de l'information disponible dans le
    format Ascii.

&IT Syntaxe des "rules"
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les rŠgles sont d‚finies dans un fichier ascii. Chaque rŠgle est compos‚e
de deux champs :

&EN le masque (pattern) de s‚lection contenant une description
    des noms concern‚s par la rŠgle. Ce masque se d‚finit … la maniŠre du
    search dans IODE (menu Data), c'est-…-dire en incluant des * et des ?
    pour sp‚cifier des ensembles.

&EN l'algorithme de transcodage du nom pour cette rŠgle qui peut
    contenir :
&EN2  + : indique que le caractŠre doit ˆtre repris dans le nom
&EN2  - : indique que le caractŠre doit ˆtre saut‚
&EN2  n'importe quel autre caractŠre : repris dans le nom

&EX2
    B*    C+-+        transforme B1234 en CB2, BCDEF en CBE, etc
    *X    ++++++++++  garde les noms se terminant par X inchang‚s
&TX


&IT S‚lection via rules
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les rŠgles d‚finies permettent de ne conserver que les s‚ries
dont le nom r‚pond … au moins une des rŠgles. Toutes les autres
sont ‚limin‚es.

En particulier la rŠgle ~c* ++++++++++~C s‚lectionne toutes les s‚ries
en gardant le nom (tronqu‚ … 10 caractŠres, 20 … partir de la version 6.01).
Si le fichier de rŠgles est vide ou non d‚fini, la rŠgle
&CO
	* ++++++++++
&TX
est utilis‚e.

&IT Ordre d'ex‚cution
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les rŠgles s'ex‚cutent dans l'ordre de d‚finition. DŠs qu'une rŠgle
s'applique … un nom, c'est elle qui est utilis‚e. On prendra donc
soin de placer en d‚but de fichier les exceptions et de terminer
par les cas g‚n‚raux.
>

&TI Format des fichiers
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Quelques formats simples sont d‚crits ci-dessous. Plus d'informations
peuvent ˆtre trouv‚es dans les annexes.

&IT ASCII
&CO
    sample 1980Y1 1990Y1
    A 1 2 3 na na 4 5
    B 0.1 0.2 -3.14

&TX

&IT Rotated ASCII
&CO
    sample 1980Y1 1990Y1
    A B C
    1 2 3
    na na 4 5
&TX

&IT DIF (tel que visible en 123)
&CO

    CODE 1980Y1 1981Y1 1982Y1 1983Y1 ....
    A    1      1       2      4
    B    1      1       2      5
&TX

&IT Belgostat DIF (tel que visible en 123)
&CO
123456789012345 1       1980    1             456900     1       1
(series nb)     (freq)  (year)  (per in year) (value)  (status)  (free)
   ........
&TX
>


<Export>
    Export
    ÄÄÄÄÄÄ

~a'pxport.png'

Cette fonction permet de g‚n‚rer des fichiers de donn‚es en
formats externes : Comma separated values (CSV), CSV invers‚,
TSP, WKS (Lotus/Excel), DIF.

Le processus charge temporairement un fichier IODE (Variable File). Il n'y a
donc pas d'effet sur les objets actuellement en WS.

Les champs de l'‚cran de saisie son d‚finis
ci-dessous.


&IT Variable file
ÄÄÄÄÄÄÄÄÄÄÄÄÄ
Ce champ indique le nom du fichier contenant les variables … exporter. Il est
requis.

&IT Comment file
ÄÄÄÄÄÄÄÄÄÄÄÄ
Ce champ permet de sp‚cifier un fichier de commentaires. Les commentaires
seront associ‚s aux variables du mˆme nom dans le fichier r‚sultat lorsque
le format le permet.

&IT Rule file
ÄÄÄÄÄÄÄÄÄ
Le fichier des "rules" permet de limiter les variables … exporter et
‚ventuellement d'en changer les noms.

&IT Sample
ÄÄÄÄÄ
Si le sample n'est pas pr‚cis‚, toutes les observations sont fournies
dans le fichier r‚sultat. Sinon, le fichier r‚sultat sera limit‚ au sample
indiqu‚.

&IT Format
ÄÄÄÄÄÄ
Ce champ MENU permet de choisir le format du fichier output.

Dans le cas des fichiers WKS, un "Label" est associ‚ … chaque range de
valeurs, ce label ‚tant identique au code de la s‚rie.

&IT File
ÄÄÄÄÄÄÄÄ
Ce champ DIR indique le nom du fichier r‚sultat. Il est requis.

&IT NaN et Separator
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Dans le cas du format CSV, deux ‚l‚ments sont libres : le s‚parateur de
colonnes (par d‚faut ~c;~C) et la repr‚sentation de la valeur inconnue (par
d‚faut ~c#N/A~C).

&IT Debug File
ÄÄÄÄÄÄÄÄÄÄ
Ce champ DIR permet de sp‚cifier le nom d'un fichier qui contiendra
une trace de l'exportation, en particulier de l'application des "rules".


&NO Notons qu'il est ‚galement possible de g‚n‚rer des fichiers CSV
    (pour un tableur) … partir d'un fichier a2m ou lors d'une impression
    dans un fichier.

>

<Load profile>
   Load profile
   ÄÄÄÄÄÄÄÄÄÄÄÄ

Ce point du menu charge un fichier "profile" … partir du disque et
r‚initialise les pages de l'application avec les valeurs de ce profil.

.~a'pldprof.png'

Le fichier profile est une image du contenu des champs des pages de
saisie au moment o— l'on quitte IODE ou au moment o— l'on sauve le
profile (voir Save Current Profile). L'utilisateur peut donc sauver la
configuration et le contenu courant des pages de fa‡on … les retrouver
ult‚rieurement au cours d'une autre session de travail. Il peut aussi
changer de configuration en cours de session.

&NO
Le profile courant est sauv‚ automatiquement dans le fichier IODE.PRF
lorsque l'on quitte IODE et charg‚ automatiquement lorsque le programme est
lanc‚.

La page de saisie permet de s‚lectionner le profile … charger. Le champ de
saisie du nom du fichier … charger est du type DIR. La touche F10 valide
l'op‚ration lorsque la page est compl‚t‚e.

Le profile charg‚ remplace le profile courant.
>

<Save current>
   Save current Profile
   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Ce point du menu permet de sauver un fichier profile sur disque.

.~a'psvprof.png'

Voir <Load Profile> pour la description des "profils".

Le profil courant est sauv‚ automatiquement dans le fichier ~cIODE.PRF~C
lorsque l'on quitte IODE et charg‚ automatiquement lorsque le programme
est lanc‚. Le profil est sauvegard‚ dans le directory courant.

La page de saisie permet de donner un nom au profile … sauver. Le champ
de saisie du nom du fichier … charger est du type DIR. La touche F10
valide l'op‚ration lorsque la page est compl‚t‚e.
>

<Reset profile>
   Reset profile
   ÄÄÄÄÄÄÄÄÄÄÄÄÄ
Ce point du menu permet de vider tous les champs de saisie des pages ou de
les remettre … leur valeur par d‚faut.

.~a'presprof.png'

Pour ‚viter une op‚ration involontaire, cette op‚ration - qui ne demande
pas de paramŠtre - doit ˆtre confirm‚e.
>

<Print Setup>
    Print Setup
    ÄÄÄÄÄÄÄÄÄÄÄ
~a'ppsetup.png'

Cette fonction permet de pr‚ciser le type d'impression que l'on souhaite
obtenir : dans un fichier ou sur une imprimante.
Le choix effectu‚ dans cet ‚cran a un effet
sur toutes les impressions.

&IT Print To
ÄÄÄÄÄÄÄÄ
Ce champ offre le choix entre une impression dans un fichier ou sur
l'imprimante courante d‚finie dans Windows.

Dans le second cas, toute impression est simplement dirig‚e vers
l'imprimante. Dans le premier cas, … chaque nouvelle impression, un
‚cran de saisie  demandera le nom et le format du
fichier destin‚ … recevoir le produit de l'impression.

~a'ppdest.png'

&IT Apply
ÄÄÄÄÄ
Ce bouton permet d'enregistrer les choix pour la suite du programme.


&IT Options
ÄÄÄÄÄÄÄ
Ce bouton donne accŠs … toutes les options de traduction et
d'impression. Pour chaque destination d'impression ou de traduction, une
s‚rie d'options sont disponibles: elles sont illustr‚es ci-dessous.

Ces options sont semblables … celles d‚finies dans le programme A2M qui
fait l'objet d'un chapitre s‚par‚ de ce manuel.

&IT Options impression directe (Printer)
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

~a'ppa2m.png'


&IT Options g‚n‚ration fichier A2m
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

~a'paa2m.png'

&IT Options g‚n‚ration fichier MIF
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

~a'pma2m.png'

&IT Options g‚n‚ration fichier RTF
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

~a'pra2m.png'


&IT Options g‚n‚ration fichier HTML
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

~a'pha2m.png'

&IT Options g‚n‚ration fichier GIF
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

~a'pga2m.png'
>

<eXit               Alt-X>
   Exit : quitter le programme
   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette option permet de quitter le programme IODE. Confirmation est d'abord
demand‚e pour ‚viter de quitter brutalement l'application.


~a'pqiode.png'

Elle permet de faire un ultime sauvetage des WS de donn‚es avant de
quitter d‚finitivement IODE.

Cette fonction peut ˆtre d‚clench‚e n'importe o— dans le logiciel en
pressant la touche Alt-X.
>










