START
#include "macros.h"

<Syntaxe des fichiers a2m>
    Syntaxe des fichiers a2m
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

La syntaxe reprise ci-dessous correspond … la version d'avril 97 du
langage a2m. Il s'agit d'une refonte importante du langage, compatible
avec la version ant‚rieure, mais contenant de nombreux nouveaux tags,
parmi lesquels des commandes de d‚finition de tableaux, de graphiques et
de sujets pour les fichiers d'aide de Windows 95, la prise en compte de
la couleur, etc.

Un fichier en format A2M peut ˆtre traduit en diff‚rents formats :

&EN GDI, format d'impression directe sous Windows
&EN RTF, interpr‚table par Word, WordPerfect, etc
&EN MIF, langage structur‚ des documents Frame Maker
&EN RTF-HLP, source d'un fichier d'aide de Windows 95
&EN HTML, format des documents Internet
&EN CSV, format simplifi‚ de tables de calcul (Excel)

Selon la destination de l'impression, des tags seront soit interpr‚t‚s
diff‚remment, soit inop‚rants. Ainsi par exemple, les images ne peuvent
ˆtre incorpor‚es dans des fichiers CSV. De mˆme, les sujets d'aide n'ont
de signification que dans le cadre des fichiers d'aide.

La syntaxe des fichiers A2M est structur‚e comme suit :

&EN <CaractŠres r‚serv‚s>
&EN <Directives>
&EN <Blancs et sauts de lignes>
&EN <Paragraphes>
&EN <Commandes de mise en page>
&EN <D‚finition des tableaux>
&EN <D‚finition des graphiques>
&EN <Insertion d'images>
&EN <Enrichissements typographiques>
&EN <D‚finition des topics d'aide>
&EN <Hyperliens>
>
<CaractŠres r‚serv‚s>
CaractŠres r‚serv‚s
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Quatre caractŠres ont une signification sp‚ciale dans un fichier A2M.
Chacun peut ˆtre red‚fini.

Ces caractŠres sont :

&EN le point(.) qui d‚bute une ligne de commande. Ce caractŠre n'est pas
    interpr‚t‚ s'il n'est pas le premier d'une ligne.

&EN le backslash (\) qui d‚bute une s‚quence d'enrichissement
    typographique ou un caractŠre sp‚cial. Il peut se trouver n'importe
    o— dans le texte.

&EN le caractŠre de d‚but d'une macro (define). Ce caractŠre est
    interpr‚t‚ uniquement si le mot qui le suit est une macro. Si ce n'est
    pas le cas, il est imprim‚ comme tel.

&EN l'ampersand (&) qui indique le d‚but d'une cellule dans un tableau.

&EN le signe ## (NUMBER) qui indique le d‚but d'une directive. Il n'est
    interpr‚t‚ qu'en d‚but de ligne.

Les quatre premiers caractŠres peuvent ˆtre d‚finis pas une des commandes
suivantes:

&CO
    <.cmd> c    (Default .)
    <.esc> c    (Default \)
    <.def> c    (Default &)
    <.sep> c    (Default &)
&TX

&EX2
    .cmd .
    .esc $
    .defch &
    .sep @
&TX
>

<.cmd>
    Commande ~c.cmd~C
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet de d‚finir le caractŠre qui d‚bute une commande
dans la suite du texte. Par d‚faut, ce caractŠre est le point.

Ce caractŠre est interpr‚t‚ uniquement s'il est le premier d'une ligne.
Pour imprimer ce caractŠre en d‚but de ligne, il faut le pr‚c‚der du
caractŠre esc (voir <.esc>).

&SY
    .cmd ch
&TX

&EX
    .cmd #
&TX
>
<.esc>
    Commande ~c.esc~C
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet de d‚finir le caractŠre qui d‚bute une s‚quence
d'attribut typographiques, d'insertion d'objet ou d'hyperlien dans la
suite du texte. Par d‚faut, ce caractŠre est le BACKSLASH (\).

Ce caractŠre est interpr‚t‚ o— qu'il se trouve dans le texte. Pour
imprimer ce caractŠre, il faut le doubler.

&SY
    .esc ch
&TX

&EX
    .esc &
&TX

>
<.def>
    Commande ~c.def~C
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet de d‚finir le caractŠre qui d‚bute une r‚f‚rence …
une macro dans la suite du texte. Par d‚faut, ce caractŠre est le
AMPERSAND (&).

Ce caractŠre est interpr‚t‚ o— qu'il se trouve dans le texte. Pour
imprimer ce caractŠre, il faut le faire suivre d'un point-virgule (;).

Si la macro dont le nom suit n'existe pas, ou s'il n'y a pas de macro
qui suit ce caractŠre, il est rendu … l'impression tel quel, de mˆme que
le nom de la macro.

&SY
    .def ch
&TX

&EX
    .def $
    .define LST par1, par2, par3, par4
    Liste : $LST
    Liste : $ LST
    Liste : $;LST
    Liste : $LIST
&TX
sera interpr‚t‚ comme suit:
&CO
    Liste : par1, par2, par3, par4
    Liste : $ LST
    Liste : $LST
    Liste : $LIST
&TX

>
<.sep>
    Commande ~c.sep~C
    ÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande indique le caractŠre qui d‚bute une cellule dans la
d‚finition d'un tableau. Par d‚faut, ce caractŠre est le AMPERSAND (&).

&EX
    .sep @
    .tb 3
    @1CTitre 1  @1CTitre 2   @1CTitre 3
    @1LCell 2,1 @1LCell 2,2  @1LCell 2,3
    .te
&TX
>

<Directives>
    Directives
    ÄÄÄÄÄÄÄÄÄÄ
Les directives suivantes sont interpr‚t‚es :

&EN <##include> "filename"
&EN <##define > MacroName MacroDefinition
&EN <##undef  > MacroName
&EN <##ifdef  > MacroName
&EN <##else   >
&EN <##endif  >

Pour qu'une telle ligne ne soit pas interpr‚t‚e, il suffit de placer
un point-virgule (;) aprŠs le caractŠre #.
>

<##include>
    La directive ~c##include~C
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette command permet d'include un fichier dans le fichier en cours de
lecture. Tout se passe comme si le contenu du fichier inclus ‚tait
int‚gr‚ dans le fichier courant.

&SY
    ##include "filename"
&TX

>
<##define>
    La directive ~c##define~C
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet de d‚finir des raccourcis d'‚criture (macros).

Le texte d'une macro peut s'‚tendre sur plusieurs lignes: il suffit pour
cela que le dernier caractŠre de la ligne de d‚finition (et des
suivantes au besoin) soit l'AMPERSAND (&).

&SY
    ##define MACRONAME texte libre
&TX

Le nom des macros peuvent ˆtre quelconque mais ne doit pas contenir
de caractŠres autres qu'alphanum‚riques ou de soulignement (_).

L'utilisation des macros se fait … l'aide du caractŠre & suivi du
nom de la macro :

&CO
    ##define UINT  unsigned short
    ...
    &UINT i;
&TX

Pour annuler l'interpr‚tation d'une macro, il suffit de placer un
point-virgule (;) aprŠs le caractŠre &.

Le s‚parateur qui suit le nom de la macro peut ˆtre quelconque. S'il
s'agit d'un point-virgule (;), ce caractŠre est consomm‚.

&CO
    ##define TX unsigned short
    &;;TX x,y
    &TX x,y
    &TX;x,y
&TX
Donne … l'interpr‚tation
&CO
    &;TX x,y
    unsigned short x,y
    unsigned shortx,y
&TX

Pour prolonger le texte d'une macro sur plusieurs lignes, il suffit de
placer en fin de ligne le caractŠre ##. La ligne suivante ne sera pas
interpr‚t‚e (sauf en ce qui concerne les "sous-macros" utilis‚es) et fera
partie de la macro.

Pour placer le caractŠre ## en fin de macro, il suffit de le doubler en
fin de ligne.

&CO
    ##define PAR1 .par1 par_1##

    ##define PAR2 .par1 par_2####
    &PAR1 texte1
    &PAR2 texte2
&TX
Donne … l'interpr‚tation
&CO
    .par1 par_1
     texte1
    .par1 par2## texte2
&TX

Si une macro n'est pas d‚finie, le texte du fichier n'est pas
interpr‚t‚. Ainsi:

&CO
    ##define A AVAL
    &A
    &B
&TX
Donne … l'interpr‚tation
&CO
    AVAL
    &B
&TX

>
<##undef>
    La directive ~c##undef~C
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette directive supprime la d‚finition d'une macro.
&SY
    ##undef MACRONAME
&TX
>

<##ifdef>
    La directive ~c##ifdef~C
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette directive permet de ne traiter une partie du texte que si une macro
est d‚finie.

&SY
    ##ifdef MACRONAME
&TX

Si MACRONAME n'est pas d‚finie, la suite du texte, jusqu'au prochain
##endif ou ##else n'est pas trait‚e.


>
<##else>
    La directive ~c##else~C
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette directive permet de ne traiter la partie suivante du texte que si
le pr‚c‚dent ##ifdef annule le texte situ‚ aprŠs le ##ifdef.

&SY
    ##else
&TX

La port‚e du ##else s'‚tend jusqu'au ##endif suivant.

>
<##endif>
    La directive ~c##endif~C
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Termine une suite ##ifdef .. ##else .. ##endif.

&SY
    ##endif
&TX
>

<Blancs et sauts de lignes>
Blancs et sauts de lignes
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Toute suite d'espaces dans le texte est normalement ramen‚e … un espace
unique. On peut modifier ce comportement par la commande ~cbl_on~C. Pour
revenir au traitement normal, on utilisera la commande ~cbl_off~C.

Ces commandes peuvent se trouver n'importe o— dans le fichier. Elles
doivent ˆtre coll‚es … la marge.

&CO
    .bl_on      --->> conserver les doubles blancs
    .bl_off     --->> supprimer les blancs exc‚dentaires
&TX

Un paragraphe est d‚fini comme une suite de lignes de texte contigu‰s.
Une ligne de commande ou une ligne blanche indique la fin d'un
paragraphe.

Le paragraphe est reformatt‚ en fonction de la destination et les sauts
de ligne ne se situent pas n‚cessairement … la mˆme place que dans la
source. La commande ~clf_on~C permet de conserver les sauts de lignes.
La commande ~clf_off~C revient au traitement par d‚faut.

&CO
    .lf_on       --->> conserver les sauts de lignes
    .lf_off      --->> reformatter le paragraphe
&TX

&IT Exemple
ÄÄÄÄÄÄÄ
Le texte suivant
&CO
    .bl_on
    .lf_on
    La cigalle ayant chant‚
	tout l'‚t‚,
	    se trouva fort d‚pourvue
		quand la bise fut venue !
&TX

est interpr‚t‚ comme suit :
&CO
    La cigalle ayant chant‚
	tout l'‚t‚,
	    se trouva fort d‚pourvue
		quand la bise fut venue !
&TX
Par contre,
&CO
    .bl_off
    .lf_off
    La cigalle ayant chant‚
	tout l'‚t‚,
	    se trouva fort d‚pourvue
		quand la bise fut venue !
&TX

est interpr‚t‚ comme suit :
..par1 cmd_1
La cigalle ayant chant‚ tout l'‚t‚, se trouva fort d‚pourvue quand la
bise fut venue !
&TX

>
<Paragraphes>
Paragraphes
ÄÄÄÄÄÄÄÄÄÄÄ
Lors de la g‚n‚ration du fichier destination, les propri‚t‚s des
paragraphes influencent la pr‚sentation.

De nouveaux paragraphes peuvent ˆtre cr‚es dans le cours du texte.Les
paragraphes peuvent ˆtre (re-)d‚finis … n'importe quel endroit du
fichier. Les modifications prennent cours aprŠs la commande.

Deux commandes permettent de fixer le tag des paragraphes suivants. Un
commande est associ‚e … la (re-)d‚finition des paragraphes.

&CO
    <.par> partag
    <.par1> partag
    <.pardef> parm=val ...
&TX

Les paragraphes suivants sont d‚finis par d‚faut:

&IT Titres
ÄÄÄÄÄÄ
&CO
    .pardef tit_7 copy=tit_3 html=5 number=5
    .pardef tit_6 copy=tit_3 html=5 number=5
    .pardef tit_5 copy=tit_3 html=5 number=5
    .pardef tit_4 copy=tit_3 html=5 number=5
    .pardef tit_3 copy=tit_2 html=4 number=4
    .pardef tit_2 copy=tit_1 html=3 spaceb=12 spacea=6 size=12 number=3
    .pardef tit_1 html=2 bold=y spaceb=14 spacea=7 keepwn=y size=14
		  number=2 lmargin=16
    .pardef tit_0 html=1 spaceb=18 spacea=9 bold=y keepwn=y size=18
		  number=1 justify=center newpage=y
    .pardef pari copy=par_0 italic=y underline=y keepwn=y spaceb=12
		 spacea=6 size=12
    .pardef parb2 copy=parb
    .pardef parb copy=par_0 bold=y keepwn=y spaceb=12 spacea=6 size=12
&TX

&IT Texte
ÄÄÄÄÄ
&CO
    .pardef par_7 copy=par_0
    .pardef par_6 copy=par_0
    .pardef par_5 copy=par_0
    .pardef par_4 copy=par_0
    .pardef par_3 copy=par_0
    .pardef par_2 copy=par_0
    .pardef par_1 copy=par_0
    .pardef par_0 spaceb=2 spacea=4 size=10 lmargin=16
&TX

&IT Enum‚rations
ÄÄÄÄÄÄÄÄÄÄÄÄ
&CO
    .pardef enum_3 copy=enum_0 lmargin=48
    .pardef enum_2 copy=enum_0 lmargin=40
    .pardef enum_1 copy=enum_0 lmargin=32
    .pardef enum_0 copy=par_0 lmargin=24 bullet=y
&TX

&IT Courier
ÄÄÄÄÄÄ
&CO
    .pardef cmd_4 copy=cmd_0
    .pardef cmd_3 copy=cmd_0
    .pardef cmd_2 copy=cmd_0
    .pardef cmd_1 copy=cmd_0
    .pardef cmd_0 spaceb=2 spacea=4 size=10 color=green lmargin=16
		  family=courier bold=y
&TX

&IT Autres
ÄÄÄÄÄÄ
&CO
    .pardef note spaceb=6 spacea=6 size=10 family=Times
		 color=red lmargin=8
    .pardef PageNum italic=y lmargin=0 size=8 spacea=0
		 spaceb=0 family=Times
&TX

&IT Graphiques
ÄÄÄÄÄÄÄÄÄÄ
&CO
    .pardef GraphFooter spaceb=0 spacea=0 lmargin=0 size=8
    .pardef GraphHeader spaceb=0 spacea=0 lmargin=0 size=10 bold=yes
    .pardef GraphLegend spaceb=0 spacea=0 lmargin=0 size=8 bold=yes
    .pardef GraphLabel spaceb=0 spacea=0 lmargin=0 size=8
    .pardef GraphTitle copy=TableTitle
    .pardef GraphFootnote copy=TableFootnote
&TX

&IT Tableaux
ÄÄÄÄÄÄÄÄ
&CO
    .pardef TableTitle bold=y lmargin=0 size=12 spacea=12 spaceb=12
		       family=Times
    .pardef TableFootnote italic=y lmargin=0 size=10 spacea=10
		       spaceb=5 family=Times
    .pardef TableFooter copy=TableCell bold=y italic=yes
    .pardef TableHeader copy=TableCell bold=y
    .pardef TableCell lmargin=0 size=8 spacea=0 spaceb=0
&TX

>
<.par>
    La commande ~c.par~C
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette instruction fixe le type de tous les paragraphes suivants. Sa
port‚e s'arrˆte lorsqu'une nouvelle commande ~c.par~C est rencontr‚e.

&SY
    .par ~ipartag~I
&TX


&IT Exemple
ÄÄÄÄÄÄÄÄ
&CO
   .par enum_1
&TX
>
<.par1>
    La commande ~c.par1~C
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette instruction fixe le type du seul paragraphe qui suit la commande.
Sa port‚e s'arrˆte dŠs la fin du paragraphe suivant.

&SY
    .par1 ~ipartag~I
&TX

&IT Exemple
ÄÄÄÄÄÄÄÄ
&CO
    .par par_1
    .par1 tit_1
    TITRE en format tit_1

    Texte du paragraphe en format par_1
&TX
>

<.pardef>
    La commande ~c.pardef~C
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette commande permet de d‚finir de nouveaux paragraphes et de modifier
les propri‚t‚s de paragraphes existants. Plusieurs propri‚t‚s peuvent
ˆtre modifi‚es dans la mˆme commande.

&SY
    .pardef ~ipartag~I ~i~bprop~B=~bval~B~I ...

    Valeur de prop
    --------------
	copy     = partag
	lmargin  = nn
	spacea   = nn
	spaceb   = nn
	number   = nn
	html     = nn
	justify  = Left | Center | Right | Decimal | Justify
	keepwn   = Yes | No
	newpage  = Yes | No
	bullet   = Yes | No
	color    = Black | white | red | green | blue |
		   cyan | magenta | yellow
	family   = Arial | Times | Courier
	size     = nn
	italic   = Yes | No
	bold     = Yes | No
	underline= Yes | No
&TX

Si ~cpartag~C correspond … un nom de paragraphe d‚j… connu, les
propri‚t‚s de ce dernier sont modifi‚es par la commande. Sinon, un
nouveau paragraphe est cr‚‚.

&TI Propri‚t‚s par d‚faut
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Lors de la cr‚ation d'un nouveau pragraphe, les propri‚t‚s suivantes sont
d‚finies par d‚faut:

&CO
	lmargin  = 0
	spacea   = 2
	spaceb   = 4
	number   = 0
	html     = 0
	justify  = Left
	keepwn   = No
	newpage  = No
	bullet   = No
	color    = Black
	family   = Arial
	size     = 10
	italic   = No
	bold     = No
	underline= No
&TX

&TI ~ccopy=partag~C
ÄÄÄÄÄÄÄÄ
Tous les attributs du paragraphe ~c~ipartag~I~C sont copi‚es dans ce
paragraphe. La position du paramŠtre a de l'importance: si cette
propri‚t‚ apparaŒt aprŠs une autre, la premiŠre est sans effet.

&EX2
    .pardef par_1 copy=par_0 lmargin=20
    .pardef par_2 copy=par_1 lmargin=30
&TX

&TI ~clmargin=nn~C
ÄÄÄÄÄÄÄÄÄÄ
La valeur de nn indique en points la marge de gauche du paragraphe.
Un point vaut 1/72e de pouce. Par d‚faut, cette valeur est fix‚e … 0.

&TI ~cspacea=nn~C
ÄÄÄÄÄÄÄÄÄ
La valeur de nn indique en points l'espace blanc avant le paragraphe.
Un point vaut 1/72e de pouce. Par d‚faut, cette valeur est fix‚e … 2
pts.

&TI ~cspaceb=nn~C
ÄÄÄÄÄÄÄÄÄ
La valeur de nn indique en points l'espace blanc aprŠs le paragraphe.
Un point vaut 1/72e de pouce. Par d‚faut, cette valeur est fix‚e … 4
pts.

&TI ~cnumber=nn~C
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
La valeur de nn indique le niveau de num‚rotation du paragraphe.

&EN nn=0 si le paragraphe ne fait pas partie d'une suite de paragraphes
    num‚rot‚s (d‚faut)
&EN nn=1 si le paragraphe n'est pas num‚rot‚ mais que les num‚ros
    doivent ˆtre r‚initialis‚s (par exemple tit_0).

&EN nn>>1 si le paragraphe est pr‚fix‚ par nn - 1 num‚ros
    automatiquement incr‚ment‚s. Le dernier nombre est incr‚ment‚, les
    autres restant … leur valeur ant‚rieure.

&IT Exemples
ÄÄÄÄÄÄÄÄÄ
&CO
    .par1 tit_0
    TITRE niveau 0
    .par1 tit_1
    TITRE niveau 1
    .par1 tit_1
    TITRE niveau 1
    .par1 tit_2
    TITRE niveau 2
    .par1 tit_2
    TITRE niveau 2
    .par1 tit_1
    TITRE niveau 1
    .par1 tit_0
    TITRE niveau 0
    .par1 tit_1
    TITRE niveau 1
&TX
Donne aprŠs interpr‚tation:
&CO
    TITRE niveau 0
      1. TITRE niveau 1
      2. TITRE niveau 1
	2.1 TITRE niveau 2
	2.2 TITRE niveau 2
      3.TITRE niveau 1
    TITRE niveau 0
      1. TITRE niveau 1
&TX

&TI ~chtml=nn~C
ÄÄÄÄÄÄÄ
Si nn est sup‚rieur … 0, indique que ce paragraphe doit ˆtre transpos‚
en HTML sous forme d'un tag <<Hnn>>. Cette propri‚t‚ n'a pas d'effet
dans les autres destinations.

&TI ~cjustify=L|C|R|J|D~C
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette propri‚t‚ permet de choisir la justification du paragraphe. Cinq
possibilit‚s sont disponibles, mais toutes ne sont pas op‚rationnelles
pour toutes les destinations.

&EN Left=cadr‚ … gauche (d‚faut)
&EN Center=centr‚
&EN Right=cadr‚ … droite
&EN Decimal=cadr‚ sur le point d‚cimal (en g‚n‚ral, cadr‚ … droite)
&EN Justify=cadr‚ … gauche et … droite (pas op‚rationnel en GDI)


&TI ~ckeepwn~C
ÄÄÄÄÄÄÄÄÄ
Force un saut de page s'il n'y a pas assez de place sur la page pour
imprimer le paragraphe courant ET le suivant. Si plusieurs paragraphes
successifs ont cette propri‚t‚, un saut de page a lieu si tous ces
paragraphes ne peuvent se trouver sur la mˆme page.

&EN keepwn=yes : force un saut de page au besoin
&EN keepwn=no : ne force pas de saut de page (d‚faut)

&TI ~cnewpage~C
ÄÄÄÄÄÄÄÄ
Force un saut de page avant le paragraphe.

&EN newpage=yes : force un saut de page
&EN newpage=no : ne force pas de saut de page (d‚faut)

&TI ~cbullet~C
ÄÄÄÄÄÄÄÄÄ
Place un carr‚ avant le paragraphe (‚num‚ration). Le carr‚ est d‚cal‚
de 8 pts par rapport … la marge de gauche du paragraphe.

&EN bullet=yes : place un bullet
&EN bullet=no : ne place pas un bullet (d‚faut)

&TI ~ccolor~C
ÄÄÄÄÄÄÄÄÄ
Fixe la couleur du texte du paragraphe. Seule la premiŠre lettre est
interpr‚t‚e. Les couleurs suivantes sont disponibles:

&EN color=Black (d‚faut)
&EN color=white
&EN color=red
&EN color=green
&EN color=blue
&EN color=cyan
&EN color=magenta
&EN color=yellow

&NO Black commence par une majuscule, blue par une minuscule!


&TI ~cfamily~C
ÄÄÄÄÄ
D‚termine la famille de caractŠres utilis‚e pour le paragraphe. Trois
possibilit‚s sont disponibles:

&EN family=Arial (d‚faut)
&EN family=Times
&EN family=Courier

&TI ~csize~C
ÄÄÄÄÄ
Fixe la taille en points de la police. Par d‚faut, cette valeur est
fix‚e … 10.

&TI ~citalic~C
ÄÄÄÄÄ
D‚termine si la police doit ˆtre en italic ou non.

&EN italic=Yes
&EN italic=No (d‚faut)

&TI ~cbold~C
ÄÄÄÄÄ
D‚termine si la police doit ˆtre en gras ou non.

&EN bold=Yes
&EN bold=No (d‚faut)

&TI ~cunderline~C
ÄÄÄÄÄ
D‚termine si la police doit ˆtre soulign‚e ou non.

&EN underline=Yes
&EN underline=No (d‚faut)
>

<Commandes de mise en page>
Commandes de mise en page
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Les commandes de mise en page suivantes sont disponibles:
&CO
    <.page>
    <.line1>
    <.line2>
    <.pghead> text
    <.pgfoot> text
&TX
>

<.page>
    La commande ~c.page~C
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande force un saut … la page.
>
<.line1>
    La commande ~c.line1~C
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande insŠre une ligne horizontale dans le texte.
>
<.line2>
    La commande ~c.line2~C
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande insŠre une ligne horizontale de double ‚paisseur dans le
texte.
>
<.pghead>
    La commande ~c.pghead~C
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande d‚finit le texte qui devra se trouver en haut de page
lors de l'impression. Elle n'est exploit‚e que pour les impressions
directes (GDI).

&SY
   .pghead texte libre
&TX
Si on souhaite ins‚rer le num‚ro de la page courante, il faut introduire
dans le texte %d.
>
<.pgfoot>
    La commande ~c.pgfoot~C
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande d‚finit le texte qui devra se trouver en bas de page
lors de l'impression. Elle n'est exploit‚e que pour les impressions
directes (GDI).

&SY
   .pgfoot texte libre
&TX
Si on souhaite ins‚rer le num‚ro de la page courante, il faut introduire
dans le texte %d.
>
<D‚finition des tableaux>
    D‚finition des tableaux
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les tableaux commencent par la commande <.tb> et se terminent par <.te>.
Chaque tableau peut contenir 5 sections distinctes:

&EN un titre (<.ttitle>) en dehors du corps du tableau
&EN une footnote (<.tfootnote>) en dehors du corps du tableau
&EN des lignes d'entˆte (<.theader>)
&EN des lignes de fin (<.tfooter>)
&EN des lignes centrales (<.tbody>)

Des attributs sp‚cifiques peuvent ˆtre d‚finis ‚galement, comme la
largeur en cm des colonnes (<.twidth>) ou les couleurs de pourcentages
d'hachurage des diff‚rentes sections.

&SY
    <.tb> [nc]
    <.tpage> 0|1
    <.tbreak> 0|1
    <.tshading> HFColor HFShading BodyColor BodyShading
	HFcolor et BodyColor = B : black
	HFcolor et BodyColor = r : red
	HFcolor et BodyColor = g : green
	HFcolor et BodyColor = b : blue
	HFcolor et BodyColor = y : yellow
	HFcolor et BodyColor = m : magenta
	HFcolor et BodyColor = c : cyan
	HFcolor et BodyColor = w : white
	HFShading et BodyShading=0..100

    <.tborder> 1
    <.twidth> col1 col2 ...
    <.ttitle> text
    <.tfootnote> text
    <.theader>
    &cell_definition &cell_definition ...
	...
    <.tbody>
    &cell_definition &cell_definition ...
	...
    <.tfooter>
    &cell_definition &cell_definition ...
	...
    <.tl>
    <.te>
&TX

&IT Cellule d'un tableau
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Les lignes de chaque section du tableau se pr‚sentent de fa‡on
semblable. Elles sont constitu‚es d'un ensemble de cellules commen‡ant
par le caractŠre d‚fini par la commande <.sep>. Par d‚faut ce caractŠre
est &.

Chaque cellule se compose d'une description de ses caract‚ristiques
suivie du texte de la cellule. Les blancs en fin de cellule sont
ignor‚s. Cela permet un ‚ventuel alignement sans dommage pour le
r‚sultat obtenu. Les blancs en d‚but de cellule sont conserv‚s pour
la seule premiŠre colonne.

&CO
    sep span justification [partag] text
&TX
o—

&EN sep est le caractŠre d‚fini par <.sep>
&EN span est le nombre de colonnes couvertes par la cellule
&EN justification est
&EN2 L pour justification … gauche
&EN2 C pour centrer le texte dans la cellule
&EN2 R pour justification … droite
&EN2 D pour justification sur le point d‚cimal
&EN2 J pour justification … gauche et … droite
&EN partag est le paragraphe de la cellule (opt.)
&EN text est le texte de la cellule

Les champs ~cspan~C et ~justification~C sont optionnels sauf si le texte
de la cellule commence par un chiffre. Par d‚faut, les cellules sont
align‚es … gauche dans le corps du tableau et centr‚e dans le header et
le footer.

Exemples :
&CO
    &2C<<Title>>Titre du tableau
    &1L\iTexte italic\I
    &Cellule normale&DeuxiŠme colonne
&TX

&IT Ligne d'un tableau
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Une ligne est compos‚e de cellules :
&CO
    CellDefn CellDefn ...
&TX

Un tableau pet comporter autant de lignes que souhait‚. En cas de report
… la page, les lignes d'entˆte du tableau sont reproduites.

&TI Exemple 1
ÄÄÄÄÄÄÄÄÄ
&CO
     .tb 2
     .sep @
     .twidth 3 3
     .tshading b 30 b 3
     .ttitle La section GLOBAL
     .theader
     @2CGLOBAL
     @1LKeyword@1LValues
     .tbody
     @1L\cDATE_FORMAT     @1L\cformat_defn
     @1L\cTIME_FORMAT     @1L\cformat_defn
     @1L\cNB_DEC          @1L\cnn
     @1L\c                @1L\c
     @1L\cCLOCK_POS       @1L\cline col
     @1L\cDATE_POS        @1L\cline col
     @1L\cMEM_POS         @1L\cline col
     @1L\cKEY_POS         @1L\cline col
     .te
&TX
.Cet exemple donne le r‚sultat suivant (diffŠre selon la destination):
..tb 2
..sep @
..twidth 3 3
..tshading b 30 b 3
..ttitle La section GLOBAL
..theader
.@2CGLOBAL
.@1LKeyword@1LValues
..tbody
..tl
.@1L~cDATE_FORMAT     @1L~cformat_defn
.@1L~cTIME_FORMAT     @1L~cformat_defn
.@1L~cNB_DEC          @1L~cnn
.@1L~c                @1L~c
.@1L~cCLOCK_POS       @1L~cline col
.@1L~cDATE_POS        @1L~cline col
.@1L~cMEM_POS         @1L~cline col
.@1L~cKEY_POS         @1L~cline col
..te

&TI Exemple 2
ÄÄÄÄÄÄÄÄÄ
&CO
  .tb 5
  .sep &
  &5L\bCompte de l'ensemble des administrations publiques \B
  &1L\i(En pour-cent du PIB)\I&1L&1L&1L&1L
  .tl
  &1L &1C90&1C91&1C92&1C93
  .tl
  &1L\bRecettes courantes\B&1D46.26&1D46.36&1D46.42&1D47.15
  &1L&1L&1L&1L&1L
  &1L       1. Exc‚dent brut d'exploitation&1D0.37&1D0.38&1D0.37&1D0.37
  &1L       2. Imp“ts directs&1D16.95&1D16.55&1D16.45&1D16.53
  &1L          particuliers&1D14.49&1D14.13&1D14.28&1D14.07
  &1L          soci‚t‚s&1D2.46&1D2.42&1D2.17&1D2.46
  &1L       3. Imp“ts indirects&1D10.86&1D10.78&1D10.88&1D11.25
  &1L       4. Cotisations de S‚curit‚ sociale&1D17.11&1D17.72&1D17.94&1D18.30
  &1L       5. Revenus de le propri‚t‚&1D1.49&1D1.57&1D1.45&1D1.42
  &1L          dont: ch“mage (C.C.I.)&1D1.11&1D1.21&1D1.34&1D1.54
  &1L       6. Autres recettes nettes&1D-0.50&1D-0.63&1D-0.67&1D-0.73
  &1L\bD‚penses courantes\B&1D50.08&1D50.99&1D51.54&1D52.36
  &1L       1. Consommation publique&1D14.10&1D14.48&1D14.27&1D14.78
  &1L       2. Subventions&1D2.24&1D2.26&1D2.14&1D2.05
  &1L       3. Transferts aux m‚nages&1D22.94&1D23.82&1D24.08&1D24.50
  &1L       4. Int‚rˆts effectifs&1D10.79&1D10.43&1D11.05&1D11.04
  &1L\bEpargne courante\B&1D-3.82&1D-4.62&1D-5.12&1D-5.22
  &1L\bSolde des op‚rations en capital\B&1D-1.81&1D-1.89&1D-2.05&1D-2.24
  &1L  dont: Investissements&1D1.20&1D1.26&1D1.34&1D1.43
  &1L\bCapacit‚ de financement\B&1D-5.63&1D-6.51&1D-7.17&1D-7.46
  &1L  dont: Solde primaire&1D5.16&1D3.91&1D3.87&1D3.58
  &1L\bOp‚rations financiŠres\B&1D1.92&1D1.07&1D0.69&1D2.04
  &1L\bSolde net … financer\B&1D-7.55&1D-7.58&1D-7.87&1D-9.50
  &1L\bDette publique\B&1D127.49&1D129.33&1D130.64&1D137.03
  .tl
  &5L09/05/97
  .tl
  .te
&TX

.Cet exemple donne le r‚sultat suivant (diffŠre selon la destination):
..tb 5
..sep &
.&5L~bCompte de l'ensemble des administrations publiques ~B
.&1L~i(En pour-cent du PIB)~I&1L&1L&1L&1L
..tl
.&1L &1C90&1C91&1C92&1C93
..tl
.&1L~bRecettes courantes~B&1D46.26&1D46.36&1D46.42&1D47.15
.&1L&1L&1L&1L&1L
.&1L       1. Exc‚dent brut d'exploitation&1D0.37&1D0.38&1D0.37&1D0.37
.&1L       2. Imp“ts directs&1D16.95&1D16.55&1D16.45&1D16.53
.&1L          particuliers&1D14.49&1D14.13&1D14.28&1D14.07
.&1L          soci‚t‚s&1D2.46&1D2.42&1D2.17&1D2.46
.&1L       3. Imp“ts indirects&1D10.86&1D10.78&1D10.88&1D11.25
.&1L       4. Cotisations de S‚curit‚ sociale&1D17.11&1D17.72&1D17.94&1D18.30
.&1L       5. Revenus de le propri‚t‚&1D1.49&1D1.57&1D1.45&1D1.42
.&1L          dont: ch“mage (C.C.I.)&1D1.11&1D1.21&1D1.34&1D1.54
.&1L       6. Autres recettes nettes&1D-0.50&1D-0.63&1D-0.67&1D-0.73
.&1L~bD‚penses courantes~B&1D50.08&1D50.99&1D51.54&1D52.36
.&1L       1. Consommation publique&1D14.10&1D14.48&1D14.27&1D14.78
.&1L       2. Subventions&1D2.24&1D2.26&1D2.14&1D2.05
.&1L       3. Transferts aux m‚nages&1D22.94&1D23.82&1D24.08&1D24.50
.&1L       4. Int‚rˆts effectifs&1D10.79&1D10.43&1D11.05&1D11.04
.&1L~bEpargne courante~B&1D-3.82&1D-4.62&1D-5.12&1D-5.22
.&1L~bSolde des op‚rations en capital~B&1D-1.81&1D-1.89&1D-2.05&1D-2.24
.&1L  dont: Investissements&1D1.20&1D1.26&1D1.34&1D1.43
.&1L~bCapacit‚ de financement~B&1D-5.63&1D-6.51&1D-7.17&1D-7.46
.&1L  dont: Solde primaire&1D5.16&1D3.91&1D3.87&1D3.58
.&1L~bOp‚rations financiŠres~B&1D1.92&1D1.07&1D0.69&1D2.04
.&1L~bSolde net … financer~B&1D-7.55&1D-7.58&1D-7.87&1D-9.50
.&1L~bDette publique~B&1D127.49&1D129.33&1D130.64&1D137.03
..tl
.&5L09/05/97
..tl
..te
>
<.tb>
    La commande .tb
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette instruction d‚bute un nouveau tableau. L'unique paramŠtre indique
le nombre de colonnes du tableau. Certaines lignes peuvent ne pas
comporter pas assez de colonnes. Si plus de colonnes qu'indiqu‚
apparaissent lors de la lecture, le nombre de colonnes s'adapte pour
toutes les lignes. Dans les versions ant‚rieure, trop de colonnes
g‚nŠrait un message d'erreur.

&SY
    .tb [nc]
&TX
>

<.tshading>
    La commande .tshading
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet de pr‚ciser le type de hachurage souhait‚ dans le
tableau courant ou dans les suivants.

&SY
    .tshading HFColor HFShading BodyColor BodyShading
	HFcolor et BodyColor = B : black
	HFcolor et BodyColor = r : red
	HFcolor et BodyColor = g : green
	HFcolor et BodyColor = b : blue
	HFcolor et BodyColor = y : yellow
	HFcolor et BodyColor = m : magenta
	HFcolor et BodyColor = c : cyan
	HFcolor et BodyColor = w : white
	HFShading et BodyShading=0..100
&TX

Quatre paramŠtres doivent ˆtre pass‚s :

&EN HFColor = couleur des header et footer
&EN HFShading = pourcentage du hachurage des header et footer
&EN BodyColor = couleur des lignes du corps du tableau
&EN HFShading = pourcentage du hachurage des lignes du corps du tableau

Les pourcentages font l'objet d'une approximation en fonction du systŠme
destination.

&EX
    .tshading r 30 b 3  --->> rouge 30% et bleu 3%
&TX
>
<.twidth>
    La commande .twidth
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet de d‚terminer la largeur des colonnes du tableau.
Les valeurs sont exprim‚es en cm.

&SY
    .twidth col1 col2 ...
&TX

La derniŠre valeur est r‚p‚t‚e pour les colonnes suivantes.

Par d‚faut,

&EN GDI : les valeurs sont calcul‚es en fonction du texte avec un
maximum de 45% de la largeur du papier pour la premiŠre colonne, sauf
s'il n'y a qu'une seule colonne.

&EN dans les autres systŠmes : 6 cm pour la premiŠre colonne, 1.5 pour
les suivantes.

&EX
    .twidth 6.5 1.5   --->> 6,5 cm pour la colonne 1
			    1,5 pour les autres colonnes

&TX
>
<.tborder>
    La commande .tborder
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet de sp‚cifier la largeur du cadre du tableau.
Les valeurs sont exprim‚es en points.

&SY
    .tborder nn
&TX

Il existe une valeur par d‚faut pour chacune des destinations
d'impression. Cette valeur est d‚termin‚e lors de l'impression. En RTF,
cette commande n'a pas d'effet dans la version actuelle.

&EX
    .tborder 0 --->> pas de cadre
&TX
>
<.tpage>
    La commande .tpage
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet de forcer ou non un saut de page … chaque d‚but de
tableau. Elle peut ˆtre introduite n'importe o— dans le fichier a2m et
prend effet dŠs le tableau en cours ou le tableau suivant. Elle est
valable jusqu'… la fin du fichier. Si plusieurs impressions ont lieu au
sein du mˆme programme, la derniŠre valeur rencontr‚e aura effet pour
les fichiers suivants.

&SY
    .tpage 0 | 1
&TX

&EX
    .tpage 1 --->> force un saut de page
&TX
>
<.tbreak>
    La commande .tbreak
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet de forcer ou non un tableau … ˆtre entiŠrement sur
la mˆme page (pour peu que la page soit assez grande). Elle peut ˆtre
introduite n'importe o— dans le fichier a2m et prend effet dŠs le
tableau en cours ou le tableau suivant. Elle est valable jusqu'… la fin
du fichier. Si plusieurs impressions ont lieu au sein du mˆme programme,
la derniŠre valeur rencontr‚e aura effet pour les fichiers suivants.

&SY
    .tbreak 0 | 1
&TX

&EX
    .tbreak 1 --->> tout le tableau se trouvera sur la mˆme page
&TX
>
<.ttitle>
    La commande .ttitle
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande fixe le texte du titre du tableau. Ce titre ne fait pas
partie du corps du tableau mais est le paragraphe qui le pr‚cŠde.

Le paragraphe est de type ~cTableTitle~C.

Dans les systŠmes que le permettent, le tableau est accroch‚ … ce
paragraphe (anchored).

Le titre peut ˆtre compos‚ de plusieurs lignes, chacune faisant l'objet
d'une commande diff‚rente.

&SY
    .ttitle texte
    .ttitle suite du texte
&TX

&EX
    .ttitre Production par secteur
    .ttitre \s(en % du PNB)\S
&TX
>
<.tfootnote>
    La commande .tfootnote
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande fixe le texte de la footnote du tableau. Ce texte ne fait
pas partie du corps du tableau mais est le paragraphe qui le suit.
Ce paragraphe se trouve sur la mˆme page que le tableau.

Le paragraphe est de type ~cTableFootnote~C.

Le texte peut ˆtre compos‚ de plusieurs lignes, chacune faisant l'objet
d'une commande diff‚rente.

&SY
    .tfootnote texte
    .tfootnote suite du texte
&TX

&EX
    .tfootnote Source: Bureau f‚d‚ral du Plan, INS
&TX
>
<.theader>
    La commande .theader
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande indique que les lignes suivantes du tableau font partie
des lignes d'entˆte (Heading) du tableau. Toutes les lignes jusqu'au
prochain <.tfooter> ou <.tbody> en feront partie.

Les paragraphes de ces cellules sont par d‚faut de type ~cTableHeader~C.

&SY
    .theader
    &line definition
	...
&TX
>
<.tbody>
    La commande .tbody
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande indique que les lignes suivantes du tableau font partie
des lignes de corps (Body) du tableau. Toutes les lignes jusqu'au
prochain <.tfooter> ou <.theader> en feront partie.

Les paragraphes de ces cellules sont par d‚faut de type ~cTableCell~C.

&SY
    .tbody
    &line definition
	...
&TX
>
<.tfooter>
    La commande .tfooter
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande indique que les lignes suivantes du tableau font partie
des lignes de fin (Footer) du tableau. Toutes les lignes jusqu'au
prochain <.tbody> ou <.tbody> en feront partie.

Les paragraphes de ces cellules sont par d‚faut de type ~cTableFooter~C.

&SY
    .tfooter
    &line definition
	...
&TX
>
<.tl>
    La commande .tl
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande insŠre une ligne horizontale dans le tableau.

&SY
    .tl
&TX
>
<.te>
    La commande .te
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette commande indique la fin du tableau.
&SY
    .te
&TX
>
<D‚finition des graphiques>
D‚finition des graphiques
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Le langage de d‚finition des graphiques en A2M est suffisamment complet
pour permettre une vaste gamme de dessins diff‚rents.

Une graphique commence toujours par la commande <.gb> et se termine
par <.ge>. Comme pour les tableaux, un titre et une footnote ext‚rieurs
au dessin lui-mˆme permettent d'int‚grer le graphique dans le texte.

Les courbes sont repr‚sent‚es par des couples (x,y), ce qui offre un
maximum de possibilit‚s. Elles peuvent ˆtre de trois types: ligne,
histogramme ou points.

Deux axes verticaux sont permis. Les minima et maximum peuvent ˆtre
choisis pour chacun des axes (<.gxmin>, <.gxmax>, <.gymin>, <.gymax>,
 <.gzmin>, <.gzmax>).

Une grille peut ˆtre dessin‚e soit automatiquement (<.ggrid>), soit en
fonction des desiderata de l'utilisateur (<.gxgrids>, <.gygrids>,
 <.gzgrids>), soit encore des deux maniŠres combin‚es.

Une l‚gende peut ˆtre ajout‚e sur chaque axe (<.gxlegend>, <.gylegend>,
 <.gzlegend>).


&CO
    <.gb> width height   (en centimŠtres)
    <.gpage> 0 | 1
    <.gtitle> text       (peut ˆtre r‚p‚t‚)
    <.gfootnote> text    (id.)
    <.gheader> text      (id.)
    <.gfooter> text      (id.)

    <.gxlegend> text     (id.)
    <.gylegend> text     (id.)
    <.gzlegend> text     (id.)

    <.gxmin> val
    <.gxmax> val
    <.gymin> val
    <.gymax> val
    <.gzmin> val
    <.gzmax> val

    <.ggrid> None | Minor | Major

    <.gxgrids> [type] xval [type] xval ...
    <.gygrids> [type] yval [type] yval ...
    <.gzgrids> [type] zval [type] zval ...

    <.gxlabs> (xval "text") (xval "text) ...
    <.gylabs> (yval "text") (yval "text) ...
    <.gzlabs> (zval "text") (zval "text) ...

    <.gxy> L|B|S|P "Title" (xval, yval) (xval, yval) ...
    <.gxz> L|B|S|P "Title" (xval, zval) (xval, zval) ...

    <.ge>
&TX

&EX
    .gb 8 5
    .gtitle Consommation priv‚e
    .gfootnote Source: INR, Bureau f‚d‚ral du Plan
    .ggrid Major
    .gxy L "LGDP" 60 2049 61 2151 62 2263 63 2362 64 2526 65 2616
		  66 2698 67 2803 68 2921 69 3115 70 3313 71 3434
		  72 3616 73 3830 74 3986 75 3927 76 4146 77 4165
		  78 4279 79 4371 80 4559 81 4514 82 4582 83 4603
		  84 4702 85 4741 86 4805 87 4901 88 5144 89 5330
		  90 5503 91 5601 92 5679 93 5586 94 5616 95 5691
    .ge

    .gb 16 5
    .gtitle Kwartaal verloop priv‚-verbruik in constante prijzen
    .gfootnote Bron : INR, federaal Planbureau
    .gxlegend in % van het BNP
    .gylegend (duizendtallen)
    .gzlegend (percenten)
    .ggrid Major
    .gxMin 59
    .gxMax 95
    .gxlabs 85 "Break-point" 92 "Turn-point"
    .gylabs 2500 "Y-Pnt"
    .gzlabs 2.25 "Z-Pnt"
    .gxy B "LTREND" 60 2057 61 2157 62 2263 63 2373 64 2488 65 2608
		    66 2734 67 2864 68 3000 69 3141 70 3284 71 3428
		    72 3571 73 3708 74 3839 75 3961 76 4074 77 4179
		    78 4276 79 4366 80 4450 81 4530 82 4607 83 4685
		    84 4766 85 4851 86 4942 87 5038 88 5139 89 5242
		    90 5344 91 5442 92 5535 93 5625 94 5712 95 5800
    .gxz L "TREND"  60 1.408142 61 1.476598 62 1.549162 63 1.624463
		    64 1.703188 65 1.785335 66 1.87159 67 1.960583
		    68 2.053683 69 2.150206 70 2.248099 71 2.346675
		    72 2.444568 73 2.538353 74 2.62803 75 2.711546
		    76 2.788902 77 2.860781 78 2.927183 79 2.988794
		    80 3.046297 81 3.101062 82 3.153773 83 3.207169
		    84 3.262618 85 3.320806 86 3.383101 87 3.448819
		    88 3.517959 89 3.588469 90 3.658294 91 3.725381
		    92 3.789046 93 3.850656 94 3.910213 95 3.970454
    .ge
&TX
Contrairement … ce que semble indiquer l'exemple, chaque courbe doit se
trouver sur une seule ligne.
>

<.gb>
    La commande .gb
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette commande d‚bute un graphique. Elle indique la taille de celui-ci
en cm.

&SY
    .gb width height (en centimŠtres)
&TX
>
<.gpage>
    La commande .gpage
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet de forcer ou non un saut de page … chaque d‚but de
graphique. Elle peut ˆtre introduite n'importe o— dans le fichier a2m et
prend effet dŠs le graphique en cours ou le graphique suivant. Elle est
valable jusqu'… la fin du fichier. Si plusieurs impressions ont lieu au
sein du mˆme programme, la derniŠre valeur rencontr‚e aura effet pour
les fichiers suivants.

&SY
    .gpage 0 | 1
&TX

&EX
    .gpage 1 --->> force un saut de page avant le graphique
&TX
>
<.gtitle>
    La commande .gtitle
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande indique le titre du graphique. Ce titre, ‚ventuellement
compos‚ par plusieurs commandes ~c.gtitle~C successives fait l'objet du
paragraphe qui pr‚cŠde le graphique.

Ce paragraphe est de type ~cGraphTitle~C.

Dans les systŠmes que le permettent, le graphique est accroch‚ … ce
paragraphe (anchored).

&SY
    .gtitle texte
    .gtitle suite du texte
&TX

&EX
    .gtitre Production par secteur
    .gtitre \s(en % du PNB)\S
&TX
>
<.gfootnote>
    La commande .gfootnote
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande indique la footnote du graphique. Ce texte,
‚ventuellement compos‚ par plusieurs commandes ~C.gfootnote~C
successives fait l'objet du paragraphe qui suit le graphique.

Ce paragraphe est de type ~cGraphFootnote~C.

La footnote se trouve sur la mˆme page que le graphique.

&SY
    .gfootnote text
&TX
>
<.gheader>
    La commande .gheader
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande indique la ou les premiŠres lignes du titre du graphique
inclus dans l'image. Ce texte sera suivi des textes et lignes
r‚f‚ren‡ant les courbes du graphique d‚finies via les commandes
~c<.gxy>~C ou ~c<.gxz>~C.

&SY
    .gheader text
&TX
Le texte sera imprim‚ avec les caract‚ristique du paragraphe
~cGraphHeader~C.
>
<.gfooter>
    La commande .gfooter
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande indique la ou les derniŠre lignes du titre du graphique
inclus dans l'image. Ce texte est pr‚c‚d‚ des textes et lignes
r‚f‚ren‡ant les courbes du graphique d‚finies via les commandes
~c<.gxy>~C ou ~c<.gxz>~C.

&SY
    .gfooter text
&TX
Le texte sera imprim‚ avec les caract‚ristique du paragraphe
~cGraphFooter~C.
>
<.gxlegend>
    La commande .gxlegend
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande indique le texte … placer en l‚gende de l'axe des X.

&SY
    .gxlegend text
&TX
Le texte sera imprim‚ avec les caract‚ristique du paragraphe
~cGraphLegend~C.
>
<.gylegend>
    La commande .gylegend
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande indique le texte … placer en l‚gende de l'axe des Y.

&SY
    .gylegend text
&TX

Le texte sera imprim‚ avec les caract‚ristique du paragraphe
~cGraphLegend~C.
>
<.gzlegend>
    La commande .gzlegend
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande indique le texte … placer en l‚gende de l'axe des Z.
L'axe des Z est plac‚ verticalement … droite du graphique.

&SY
    .gzlegend text
&TX
Le texte sera imprim‚ avec les caract‚ristique du paragraphe
~cGraphLegend~C.
>
<.gxmin>
    La commande .gxmin
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande fixe le minimum de l'axe des X. Si ce minimum ne permet
pas d'inclure toutes les observations, il est automatiquement adapt‚.

&SY
    .gxmin val
&TX
>
<.gxmax>
    La commande .gxmax
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande fixe le maximum de l'axe des X. Si cette valeur ne permet
pas d'inclure toutes les observations, elle est automatiquement adapt‚e.

&SY
    .gxmax val
&TX
>
<.gymin>
    La commande .gymin
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande fixe le minimum de l'axe des Y. Si cette valeur ne permet
pas d'inclure toutes les observations, elle est automatiquement adapt‚e.
&SY
    .gymin val
&TX
>
<.gymax>
    La commande .gymax
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande fixe le maximum de l'axe des Y. Si cette valeur ne permet
pas d'inclure toutes les observations, elle est automatiquement adapt‚e.

&SY
    .gymax val
&TX
>
<.gzmin>
    La commande .gzmin
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande fixe le minimum de l'axe des Z. Si cette valeur ne permet
pas d'inclure toutes les observations, elle est automatiquement adapt‚e.
&SY
    .gzmin val
&TX
>
<.gzmax>
    La commande .gzmax
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande fixe le maximum de l'axe des Z. Si cette valeur ne permet
pas d'inclure toutes les observations, elle est automatiquement adapt‚e.
&SY
    .gzmax val
&TX
>
<.ggrid>
    La commande .ggrid
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande d‚termine la grille qui doit apparaŒtre … travers le
graphique. Cette grille est constitut‚e de droites traversant le
graphique de part en part en prolongation des graduations des axes.

&SY
    .ggrid None | Minor | Major
&TX

&EN None = pas de grille
&EN Minor = une droite pour chaque graduation
&EN Major = une droite pour chaque graduation principale

Par d‚faut, les "Major" grids sont retenus.

>
<.gxgrids>
    La commande .gxgrids
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet d'ajouter des droites verticales traversant le
graphiques … des positions libres sur l'axe des X.

&SY
    .gxgrids [type] xval [type] xval ...
&TX
&EN type = ‚paisseur du trait (1-3)
&EN xval = abscisse du trait

>
<.gygrids>
    La commande .gygrids
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet d'ajouter des droites horizontales traversant le
graphiques … des positions libres sur l'axe des Y.

&SY
    .gygrids [type] yval [type] yval ...
&TX
&EN type = ‚paisseur du trait (1-3)
&EN yval = ordonn‚e du trait
>
<.gzgrids>
    La commande .gzgrids
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet d'ajouter des droites horizontales traversant le
graphiques … des positions libres sur l'axe des Z.

&SY
    .gzgrids [type] zval [type] zval ...
&TX
&EN type = ‚paisseur du trait (1-3)
&EN zval = valeur sur l'axe des Z de la position du trait
>
<.gxlabs>
    La commande .gxlabs
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet d'ajouter des textes et des droites verticales
traversant le graphiques … des positions libres sur l'axe des X.

Les parenthŠses sont ignor‚es et permettent simplement de clarifier
la source A2M si c'est souhait‚.

&SY
    .gxlabs (xval "text") (xval "text) ...
&TX

&EN xval = abscisse du texte et du trait
&EN "text" = texte … indiquer sur l'axe

Le texte sera imprim‚ avec les caract‚ristique du paragraphe
~cGraphLabel~C.
&EX
    .gxlabs 97 Simulation
&TX
>
<.gylabs>
    La commande .gylabs
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet d'ajouter des textes et des droites horizontales
traversant le graphiques … des positions libres sur l'axe des Y.

&SY
    .gylabs (yval "text") (yval "text) ...
&TX

&EN yval = ordonn‚e du texte et du trait
&EN "text" = texte … indiquer sur l'axe

Les parenthŠses sont ignor‚es et permettent simplement de clarifier
la source A2M si c'est souhait‚.

Le texte sera imprim‚ avec les caract‚ristique du paragraphe
~cGraphLabel~C.
>
<.gzlabs>
    La commande .gzlabs
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet d'ajouter des textes et des droites horizontales
traversant le graphiques … des positions libres sur l'axe des Z.

&SY
    .gzlabs (zval "text") (zval "text) ...
&TX

Les parenthŠses sont ignor‚es et permettent simplement de clarifier
la source A2M si c'est souhait‚.

&EN zval = valeur en Z de la position du texte et du trait
&EN "text" = texte … indiquer sur l'axe

Le texte sera imprim‚ avec les caract‚ristique du paragraphe
~cGraphLabel~C.
>
<.gxy>
    La commande .gxy
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet de d‚finir une courbe en X-Y. Elle est constitu‚e
de trois parties:

&EN type de dessin courbe
&EN titre de la s‚rie
&EN couples de valeurs

&SY
    .gxy type "titre" (xval, yval) (xval, yval) ...
&TX

&EN type indique le type de la courbe. On peut choisir entre
&EN2 Line : courbe continue
&EN2 Bar : histogramme
&EN2 Scatter : points

&EN titre indique le titre qui sera repris avec la r‚f‚rence … la courbe
    dans le bas du graphique. Ce texte doit se trouver entre doubles
    quotes.

&EN couples de valeurs repr‚sentant les points de passage de la courbe.

Les parenthŠses sont ignor‚es et permettent simplement de clarifier
la source A2M si c'est souhait‚.

Le valeurs peuvent ˆtre repr‚sent‚es dans tous les formats, avec ou sans
d‚cimales. Les notations scientifiques sont support‚es.

Une valeur non d‚finie doit ˆtre repr‚sent‚e par --. La courbe subit
une rupture … cette position.

Le texte sera imprim‚ avec les caract‚ristique du paragraphe
~cGraphHeader~C.
&EX
    .gxy B "Pommes de terre" 60 20.57E2 70 2157 80 -- 90 .2373E4
&TX
>
<.gxz>
    La commande .gxz
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet de d‚finir une courbe en X-Z. L'axe Z est un axe
alternatf qui est positionn‚ … gauche du graphique. Il permet de placer
sur un mˆme graphique des donn‚es en diff‚rentes unit‚s.

La commande ~c.gxz~C est constitu‚e de trois parties:

&EN type de dessin courbe
&EN titre de la s‚rie
&EN couples de valeurs

&SY
    .gxz type "titre" (xval, zval) (xval, zval) ...
&TX

&EN type indique le type de la courbe. On peut choisir entre
&EN2 Line : courbe continue
&EN2 Bar : histogramme
&EN2 Scatter : points

&EN titre indique le titre qui sera repris avec la r‚f‚rence … la courbe
    dans le bas du graphique. Ce texte doit se trouver entre doubles
    quotes.

&EN couples de valeurs repr‚sentant les points de passage de la courbe.

Les parenthŠses sont ignor‚es et permettent simplement de clarifier
la source A2M si c'est souhait‚.

Le valeurs peuvent ˆtre repr‚sent‚es dans tous les formats, avec ou sans
d‚cimales. Les notations scientifiques sont support‚es.

Une valeur non d‚finie doit ˆtre repr‚sent‚e par --. La courbe subit
une rupture … cette position.

Le texte sera imprim‚ avec les caract‚ristique du paragraphe
~cGraphHeader~C.
&EX
    .gxz B "Pommes de terre (%PNB)" 60 0.12E1 70 -- 80 1.1 90 1.3E0
&TX
>
<.ge>
    La commande .ge
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande indique la fin de la d‚finition 'un graphique.
&SY
    .ge
&TX
>

<D‚finition des topics d'aide>
    D‚finition des topics d'aide
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Un fichier a2m peut servir de source … la constitution d'un fichier
d'aide de Windows. En g‚n‚ral, les commandes n‚cessaires sont g‚n‚r‚es
automatiquement … partir d'un fichier .hlp (scr4_hm), mais rien
n'interdit de le faire manuellement.

Pour construire un fichier d'aide avec des hyperliens, il suffit de
d‚finir le d‚but et la fin des sujets ainsi que les r‚f‚rences dans le
texte.

En a2m, la fin d'un sujet co‹ncide avec le d‚but du suivant et est
indiqu‚e par la commande ~c<.topic>~C.

Les r‚f‚rences se feront par les s‚quences ~c<\h>~C ~c...\H~C explicit‚es plus
bas.

Le programme scr4w_ah qui permet de construire des fichiers HTML ou
les fichiers n‚cessaires … la construction des fichiers .chm (fichier
HtmlHelp) offre en plus la facult‚ d'automatiser les r‚f‚rences entre
les topics.

Les commandes associ‚es … la d‚finition du mode automatique sont:
&EN <.topiclink> : modification des paramŠtres pour le topic courant
&EN <.topicslink> : modification des paramŠtres pour tous les topics … partir du suivant.

De plus, il est possible d'associer un mot ou un groupe de mot … un topic donn‚ …
l'aide de la commande <.topicalias>.
>

<.topic>
    La commande .topic
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
La commande ~c.topic~C d‚termine le d‚but d'un sujet et les informations
n‚cessaires … son exploitation.

&SY
    .topic nn level title
&TX

&EN nn est le num‚ro de r‚f‚rence du sujet, utilis‚ par les s‚quences
    ~c\h...H~C.

&EN level est le niveau dans la table des matiŠres du sujet. La table
    des matiŠres de l'aide de Windows sera constitu‚e sur base de cete
    information.

&EN title est le titre du sujet. Ce titre sera exploit‚ pour cr‚er un
    index de recherche sous Windows et pour construire une base de
    mots-cl‚s.

Pour pouvoir utiliser a2m pour cr‚er une aide Windows avec un Sommaire,
il faut imp‚rativement que les sujets soient class‚s dans le bon ordre
(ce qui est normalement le cas).

&EX
    .topicslink case=no
    .topic 0 0 Le programme scr4w_am
    .topicalias scr4w_am
	...
	\h<<1>>Syntaxe des fichiers a2m\H
	...
    .topic 1 1 Syntaxe des fichiers a2m
    .topicalias a2m
	...
	\h<<2>>CaractŠres r‚serv‚s\H
	\h<<5>>Directives\H
	...
    .topic 2 2 CaractŠres r‚serv‚s
	...
	\h<<3>>.cmd\H
	\h<<4>>.esc\H
	...
    .topic 3 3 .cmd
	...
    .topic 4 3 .esc
	...
    .topic 5 2 Directives
	...
&TX

La commande ~c.topic~C et les r‚f‚rences aux sujets ~c\h..\H~C n'ont pas
d'effet sur le fichier r‚sultat, sauf si une aide Windows est demand‚e
(fichier rtf).
>
<.topiclink>
    La commande .topiclink
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet de changer pour le topic en cours ou le dernier
alias les paramŠtres qui d‚terminerant les liens automatiques qui seront
g‚n‚r‚s par scr4w_ah.

&SY
   .topiclink case=yes|no auto=yes|no partial=yes|no
&TX

&EN ~ccase=yes~C : le remplacement ne s'effectue que si le texte du topic ou
    de l'alias sont trouv‚s exactement. ~ccase=no~C ne diff‚rencie pas
    majuscules et minuscules.

&EN ~cauto=yes~C : indique que le texte du topic ou de l'alias, s'ils sont trouv‚s
    ailleurs dans le document, doivent ˆtre li‚s vers le topic courant. ~cauto=no~C
    exclut le topic ou l'alias de ce traitement.

&EN ~cpartial=no~C : indique que le lien vers le topic n'est effectu‚ que si
    le texte trouv‚ est pr‚c‚d‚ et suivi de caractŠres non
    alphanum‚riques. ~cpartial=yes~C permet les remplacements mˆme si le
    texte du topic fait partie d'un mot.

&EX
    .topic 2 1 L'objet ISAM
    .topicalias ISAM
    .topiclink case=no partial=yes auto=yes
    ....
    .topic ....
    Les Isams sont utilis‚s par la fonction SCR_page_to_isam().
&TX
Le texte de la derniŠre ligne sera remplac‚ par
&CO
    Les \h<<2>>Isam\Hs sont utilis‚s par la fonction SCR_page_to_\h<<2>>isam\H().
&TX

Avec ~cpartial=no~C, SCR_page_to_isam() serait rest‚ tel quel.

Si la fonction SCR_page_to_isam est un topic du document, le lien vers
cette fonction sera ‚tabli avant celui vers ISAM (les sujets les plus
longs sont remplac‚s prioritairement) et on aura :

&CO
    Les \h<<2>>Isam\Hs sont utilis‚s par la fonction
    \h<<234>>SCR_page_to_isam\H().
&TX
>

<.topicslink>
    La commande .topicslink
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande est presqu'identique … ~c.topiclink~C … ceci prŠs qu'elle
agit sur tous les topics … partir du suivant (pas le courant!). Elle
agit en quelque sorte en fixant les valeurs par d‚faut pour la suite du
document.

&SY
   .topicslink case=yes|no auto=yes|no partial=yes|no
&TX
>

<.topicalias>
    La commande .topicalias
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette commande permet de d‚finir les mots qui, s'ils sont recontr‚s
ailleurs dans le document, doivent pointer automatiquement vers le topic
courant.

&SY
   .topicalias texte libre
&TX

Pour un exemple, voir <-.topiclink>.
>

<\h>
    La s‚quence \h...\H
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
La s‚quence ~c\h<<nn>>...texte...\H~C indique que le texte doit ˆtre li‚ …
un sujet de l'aide d‚termin‚ par le num‚ro qui suit ~c\h~C.

Lorsque l'utilisateur de l'aide s‚lectionnera ce texte, il pourra se
d‚placer directement sur le sujet r‚f‚renc‚.

&SY
    \h<<nn>> ...\H
&TX

Les caractŠres << et >> doivent ˆtre pr‚sents en contiennent le num‚ro
du sujet r‚f‚renc‚, d‚fini … l'aide de la commande ~c.topic~C.

&EX
   La liste des \h<<2>>caractŠres r‚serv‚s\H se trouve ci-dessous.
&TX
>
<Hyperliens>
    Hyperliens
    ÄÄÄÄÄÄÄÄÄÄ

Les hyperliens peuvent ˆtre introduits dans le texte par la s‚quence
\l"lien"texte\L.

Ces liens renvoient … des pages HTML et sont ignor‚s dans le cadre des
g‚n‚rations vers d'autres formats.

D'autres hyperliens sont possibles … l'aide de \h ...\H. Dans ce dernier cas,
il s'agit de liens internes au document, renvoyant … d'autres topic d'aide.
>

<\l>
    La s‚quence \l
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

La s‚quence ~c\l"filename"...texte...\L~C indique que le nom du lien et
le texte … afficher dans la page HTML g‚n‚r‚e.

Lorsque l'utilisateur de l'aide s‚lectionnera ce texte, il pourra se
d‚placer directement sur le sujet r‚f‚renc‚.

&SY
    \l"lien hypertexte"texte … afficher\L
&TX


&EX
    \l"http://www.xon.be"Cliquez ici le site de X-ON\L
&TX
>

<Enrichissements typographiques>
Enrichissements typographiques
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Ces s‚quences peuvent se retrouver n'importe o— dans le texte.

L'objet de ces s‚quence est de changer, … l'int‚rieur d'un paragraphe,
les attributs typographiques des caractŠres. A partir de l'endroit o— la
s‚quence de caractŠres est rencontr‚e, et jusqu'… la s‚quence qui
l'annule ou jusqu'… la fin du paragraphe, l'attribut sera d'application.

Les attributs sont cumulatifs : on peut donc mettre un mot en soulign‚
italique, exposant, taille plus petite si on le souhaite.

Ces s‚quences peuvent se retrouver n'importe o— dans le texte, y compris
dans une cellule de tableau ou dans les textes d'un graphique.

Les modifications d'attributs, toujours pr‚fix‚es par un \ ou le
caractŠre d‚fini par la commande .esc, sont :

&CO
    <\b> ...\B      Bold       On ... Off
    <\i> ...\I      Italic     On ... Off
    <\u> ...\U      Underline  On ... Off
    <\o> ...\O      Strikethrough On ... Off
    <\c> ...\C      Courier    On ... Off
    <\e>            suppression de Bold, Italic et Underline
    <\s>            Size--
    <\S>            Size++
    <\+>            Normal
    <\->            Indice
    <\=>            Exposant
    <\p>            Newline
    <\t>            Tab
    <\k>color
	color=B : black
	color=r : red
	color=g : green
	color=b : blue
	color=y : yellow
	color=m : magenta
	color=c : cyan
	color=w : white
&TX
>

<\b>
    La s‚quence \b ... \B
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette s‚quence permet de mettre en caractŠres gras le texte qu'elle
encadre.

&SY
    \b ...\B
&TX
>
<\i>
    La s‚quence \i ... \I
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette s‚quence permet de mettre en caractŠres italiques le texte qu'elle
encadre.

&SY
    \i ...\I
&TX
>
<\u>
    La s‚quence \u ... \U
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette s‚quence permet de souligner le texte qu'elle encadre.

&SY
    \u ...\U
&TX
>

<\o>
    La s‚quence \o ... \O
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette s‚quence permet de barrer le texte qu'elle encadre.

&SY
    \o ...\O
&TX
>
<\c>
    La s‚quence \c ... \C
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette s‚quence permet de mettre en caractŠres non proportionnels le
texte qu'elle encadre. De plus, ce texte est imprim‚ en vert.
Elle est surtout utilis‚e pour des exemples.

&SY
    \c ...\C
&TX
>
<\e>
    La s‚quence \e
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette s‚quence permet d'annuler les attributs typographiques gras,
soulign‚ et italique en cours.

&SY
    \e
&TX
>
<\s>
    La s‚quence \s
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&TCASE
Cette s‚quence permet de diminuer de deux points la taille des
caractŠres qui suivent.

&SY
    \s
&TX
>
<\S>
    La s‚quence \S
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&TCASE
Cette s‚quence permet d'augmenter de deux points la taille des caractŠres
qui suivent.

&SY
    \S
&TX
>
<\+>
    La s‚quence \+
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette s‚quence permet de mettre en exposant les caractŠres qui suivent.

&SY
    \+
&TX
>
<\->
    La s‚quence \-
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette s‚quence permet de mettre en indice les caractŠres qui suivent.

&SY
    \-
&TX
>
<\=>
    La s‚quence \=
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette s‚quence permet de mettre les caractŠres qui suivent sur la ligne
de base (annule \+ et \-).

&SY
    \=
&TX
>
<\p>
    La s‚quence \p
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette s‚quence force un saut … la ligne.

&SY
    \p
&TX
>
<\t>
    La s‚quence \t
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette s‚quence insŠre une tabulation dans le texte.

&SY
    \t
&TX
>
<\k>
    La s‚quence \k
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette s‚quence permet de changer la couleur des caractŠres qui suivent.

&SY
    \kcolor
&TX
Les couleurs disponibles sont:
&EN color=B : black
&EN color=r : red
&EN color=g : green
&EN color=b : blue
&EN color=y : yellow
&EN color=m : magenta
&EN color=c : cyan
&EN color=w : white

&EX
    Voici le mot \kbBLEU\kB, ... en bleu!
&TX
donne :

    Voici le mot ~kbBLEU~kB, ... en bleu!
>

<Insertion d'images>
    Insertion d'images
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Il est possible d'ins‚rer des images dans le texte, du moins pour
certaines destinations, comme Frame, RTF ou HTML.

Tous les types de fichiers ne sont pas utilisables. On utisera les
fichiers .gif ou .bmp pour frame, .bmp pour rtf.

Cette fonction sera g‚n‚ralis‚e plus tard.

&CO
    <\a>"filename"
    <\a>'filename'
    <\A>"filename"
    <\A>'filename'
&TX

Dans le premier cas, l'image est "flottante" (FLOAT) en Frame, dans
le second elle est li‚e … la ligne o— elle est d‚finie.
>

<\A>
    La s‚quence \A
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&TCASE
Cette s‚quence insŠre … l'endroit du texte o— elle se trouve un point
d'ancrage pour un fichier image.

Le nom du fichier doit se trouver entre doubles ou simples quotes.
&SY
    \A"filename"
    \A'filename'
&TX

Dans le texte lui-mˆme, les mots "Figure nn" remplacent la s‚quence,
permettant de faire r‚f‚rence … l'image qui peut ne pas se situer juste
sous la ligne. Si on ne souhaite pas ce texte, il faut utiliser \a.

&IT Frame Maker
ÄÄÄÄÄÄÄÄÄÄÄ
Un Frame de la largeur de la page est cr‚‚ pour contenir l'image. Ce
frame est "flottant" si le nom du fichier est entre doubles quotes et fix‚
sous la ligne s'il est entre simples quotes.

&EX
    Comme on le voit sur la \A"ecran1.gif", ...
&TX
Dans le texte on aura :

&CO
    Comme on le voit sur la Figure 1 ...

      ------------------------------------------------------------
     |                     Figure 1                               |
     |                                                            |
     |                                                            |
     |                     I M A G E                              |
     |                                                            |
      ------------------------------------------------------------
&TX
et l'image elle-mˆme suivra avec les mots "Figure 1" comme titre.
>

<\a>
    La s‚quence \a
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&TCASE
Cette s‚quence insŠre … l'endroit du texte o— elle se trouve un point
d'ancrage pour un fichier image.

Le nom du fichier doit se trouver entre doubles ou simples quotes.
&SY
    \a"filename"
    \a'filename'
&TX

Il n'y a pas de texte de remplacement de cette s‚quence contrairement
… \A. L'image n'a pas de titre.

&IT Frame Maker
ÄÄÄÄÄÄÄÄÄÄÄ
Un Frame de la largeur de la page est cr‚‚ pour contenir l'image. Ce
frame est "flottant" si le nom du fichier est entre doubles quotes et
fix‚ sous la ligne s'il est entre simples quotes.

&EX
    Comme on le voit sur la figure suivante, \a'ecran1.gif' ...
&TX
Dans le texte on aura :

&CO
    Comme on le voit sur la figure suivante, ...

      ------------------------------------------------------------
     |                                                            |
     |                     I M A G E                              |
     |                                                            |
      ------------------------------------------------------------
&TX
et l'image elle-mˆme suivra.
>







