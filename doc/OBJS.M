/* OBJS */

<Introduction au logiciel>
    Introduction au logiciel
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Le logiciel IODE a ‚t‚ mis au point avec l'appui scientifique d'une
‚quipe d'‚conomŠtres ayant en charge la construction et l'exploitation
de grands modŠles macro‚conomiques.
C'est en analysant le
travail des membres de cette ‚quipe et en tentant de r‚pondre au mieux …
leurs besoins que IODE, au d‚part confin‚ dans les aspects purement
‚conom‚triques, s'est ‚largi … des fonctionnalit‚s plus larges que celles
n‚cessaires … la construction d'un modŠle et aux simulations.


En effet, si la construction d'un modŠle - r‚daction des ‚quations,
estimation, tests et simulations - est la partie la plus lourde de
l'‚laboration d'un modŠle, l'exploitation en aval des informations
produites par le modŠle repr‚sente une part consid‚rable de l'emploi du
temps des ‚conomŠtres. En amont, la construction des s‚ries de base ou
l'importation de celles-ci … partir d'informations externes repr‚sente
‚galement un travail non n‚gligeable.

IODE s'attaque donc … toutes les ‚tapes de la construction et de
l'exploitation des modŠles :

&EN l'importation et l'exportation des s‚ries vers et … partir d'autres logiciels
    (TSP, Excel, KaA, LArray (Python), E-Views...),
&EN l'automatisation de l a construction des s‚ries … partir de donn‚es provenant de
    plusieurs ensembles de donn‚es de base,
&EN la documentation des bases de donn‚es statistiques,
&EN la r‚daction des ‚quations,
&EN l'estimation des ‚quations par diff‚rentes m‚thodes,
&EN les simulations de sc‚narios,
&EN la recherche d'objectifs (goal seeking),
&EN la production de tableaux et de graphiques,
&EN la g‚n‚ration automatis‚e de rapports int‚grant tableaux, graphiques, texte
    libre, ‚quations, etc,
&EN l'importation des outputs dans des logiciels externes comme Word, Excel, ou Frame Maker,
&EN l'automatisation via un langage de scripting propre.

Pour atteindre ces diff‚rents objectifs, l'utilisateur de IODE doit
d‚finir et utiliser diff‚rents "objets". Ces objets sont manipul‚s par
IODE et stock‚s dans des espaces de travail ("workspaces").

La suite de ce chapitre aborde la d‚finition des objets de IODE, indique
l'organisation des programmes et fournit les notions de base de l'interface
utilisateur. Elle se compose de trois parties :

&EN <Les concepts et objets manipul‚s> par IODE (notion d'objets
    et de workspaces),

&EN <Organisation des fichiers> qui introduit les diff‚rents
    ‚l‚ments et programmes composant le logiciel,

&EN <L'interface utilisateur de IODE> qui d‚crit l'‚cran de base et ses
    diff‚rentes composantes.
>

<Les concepts et objets manipul‚s>
    Les concepts et objets manipul‚s
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

IODE opŠre sur des <objets><Les objets> de diff‚rents types. Ceux-ci sont group‚s dans
des ensembles appel‚s <workspaces><Les Workspaces> (WS).
Ces deux notions sont fondamentales pour la bonne
compr‚hension du logiciel. Elles font l'objet des deux premiŠres sections suivantes.

Diff‚rents types d'objets - par exemple les ‚quations - utilisent des
formules dans leur d‚finition. Un langage standard, sp‚cialement adapt‚
aux expressions ‚conom‚triques, est utilis‚ dans tous ces objets. Ce
langage, le <lec><Le LEC> (pour "Langage Econom‚trique Condens‚"), est
rapidement pr‚sent‚ dans troisiŠme section.

La derniŠre section est consacr‚e aux <rapports>, qui, via un langage de
scripting, permettent d'automatiser les fonctions de IODE.

>
<Les objets>
    Les objets
    ÄÄÄÄÄÄÄÄÄÄ

Avant de d‚tailler le contenu de chaque type d'objet, commen‡ons par
analyser la justification de chacun d'eux. En d'autres termes, essayons
de voir o— interviennent, dans le cadre d'un modŠle, les concepts
auxquels ils sont associ‚s.

Un modŠle est un systŠme d'EQUATIONS qui sont des formules faisant
intervenir des VARIABLES, s‚ries num‚riques temporelles d‚finies sur une
p‚riode de temps donn‚e, avec une fr‚quence d‚termin‚e (annuelles,
trimestrielles, etc). Les ‚quations peuvent contenir des
coefficients - ‚ventuellement estim‚s - qui sont des variables sans
dimension, appel‚es dans IODE des SCALAIRES.

EQUATIONS, VARIABLES et SCALAIRES sont trois types d'objets g‚r‚s par
IODE.

Les variables elles-mˆmes ne sont pas toutes obtenues telles quelles,
mais r‚sultent le plus souvent de calculs bas‚s sur d'autres variables,
provenant ‚ventuellement de plusieurs sources. Ces calculs peuvent ˆtre
par exemple une agr‚gation des secteurs ou une dimension
g‚ographique ou encore des s‚ries mises en base commune. Les formules
utilis‚es pour effectuer le calcul de ces variables contruites sont appel‚es
IDENTITES de construction.

Le nom donn‚ … chaque VARIABLE ne permet en g‚n‚ral
pas d'en indiquer le contenu avec suffisamment de
pr‚cision. IODE permet de cr‚er des COMMENTAIRES dont le nom sera identique
… celui des VARIABLES qu'ils d‚finissent. Ces commentaires sont simlement
des textes libres.

COMMENTAIRES ET IDENTITES sont les quatriŠme et cinquiŠme objets g‚r‚s
par IODE.

Lorsqu'on dispose de variables, il est souvent utile des les pr‚senter -
… l'‚cran ou sur papier - sous forme de tableaux ou de graphiques. IODE
permet de construire … cette fin des TABLEAUX, non pas de valeurs, mais
de formules et de texte.

Ces "tableaux" sont en quelque sorte des
petits programmes qui permettent de construire des tables de nombres
ou des graphiques sur base de VARIABLES contenues dans les fichiers de
variables. Cette fa‡on de proc‚der est trŠs efficace : les mˆmes
tableaux sont r‚utilis‚s pour imprimer diff‚rentes versions des
VARIABLES (aprŠs la simulation d'un sc‚nario par exemple). Le mˆme
tableau peut aussi ˆtre imprim‚ en taux de croissance, en comparaison de
fichiers, etc.

Le TABLEAU est le sixiŠme type d'objet de IODE.

Il n'y a pas de notion de modŠle en tant qu'objet dans IODE : un modŠle
est simplement une LISTE d'‚quations. Pour ‚viter le travail fastidieux
de r‚encodage des listes, celles-ci sont g‚r‚es comme des objets … part
entiŠre et - comme on le verra plus tard - sauv‚es dans des fichiers de
listes. L'utilisation des listes est par ailleurs omnipr‚sente : on les
trouve dans les formules pour en raccourcir l'‚criture, en paramŠtre des
fonctions de rapports pour simplifier l'encodage, etc.

La LISTE termine l'‚num‚ration des objets de IODE. Les objets g‚r‚s par
IODE sont donc au nombre de sept.

Les objets des sept types sont identifi‚s par un nom de 20 caractŠres
maximum  (… partir de la version 6.01)
commen‡ant toujours par une lettre (ou le caractŠre '_'), en
minuscule pour les SCALAIRES et en majuscule pour les autres objets
(ceci afin de distinguer les scalaires et les variables dans les
formules du langage LEC sans ambigit‚).

Chaque type d'objet a donc trouv‚ sa justification. La suite du chapitre
peut ˆtre consacr‚e … une d‚finition plus d‚taill‚e de leur contenu.

&IT Les commentaires
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Les commentaires sont des textes libres. Un ‚diteur int‚gr‚ … IODE
permet de les encoder. Ils servent … documenter des objets de IODE.

&IT Les ‚quations
ÄÄÄÄÄÄÄÄÄÄÄÄÄ
Une ‚quation se pr‚sente comme une ‚galit‚ qui fera partie d'un modŠle.
Chaque ‚quation est compos‚e des ‚l‚ments suivants :

&EN la forme LEC (langage d'‚criture des formules dans IODE)
&EN un commentaire libre (titre de l'‚quation)
&EN la m‚thode par laquelle elle a ‚t‚ estim‚e (s'il y a lieu)
&EN la p‚riode d'estimation ‚ventuelle
&EN les noms des ‚quations estim‚es simultan‚ment
&EN les instruments utilis‚s pour l'estimation

Tous ces ‚l‚ments de d‚finition sont pr‚sents dans chaque ‚quation, mais
‚ventuellement laiss‚s vides s'ils n'ont pas de justification.

Le nom d'une ‚quation est celui de sa variable endogŠne.

&IT Les identit‚s
ÄÄÄÄÄÄÄÄÄÄÄÄÄ
Une identit‚ de construction est une expression en langage LEC qui
permet de construire une nouvelle s‚rie statistique sur base de s‚ries
d‚j… d‚finies. En g‚n‚ral, les identit‚s sont "ex‚cut‚es" en groupe pour
constituer ou mettre … jour un ensemble de variables.

Il ne faut par confondre identit‚s et ‚quations. Les identit‚s au sens
du logiciel TROLL sont des ‚quations (ou parfois des listes) dans IODE.

&IT Les listes
ÄÄÄÄÄÄÄÄÄÄ
Les listes sont, comme les commentaires, des textes libres. Elles sont
utilis‚es pour simplifier l'‚criture en diff‚rentes circonstances :

&EN liste d'‚quations d‚finissant un modŠle
&EN liste de tableaux … imprimer
&EN argument quelconque d'une fonction (p‚riode d'impression par exemple)
&EN macro dans une ‚quation, une identit‚ ou un tableau
&EN etc

&IT Les scalaires
ÄÄÄÄÄÄÄÄÄÄÄÄÄ
Les scalaires sont pour l'essentiel des coefficients estim‚s d'‚quations
‚conom‚triques. Pour cette raison, chaque scalaire contient dans sa
d‚finition :

&EN sa valeur
&EN le paramŠtre de "relaxation", fix‚ … 0 pour bloquer le coefficient
    lors d'une estimation
&EN son t-test, r‚sultat de la derniŠre estimation
&EN sa d‚viation standard, r‚sultat de la derniŠre estimation

Seule la valeur du scalaire a de l'int‚rˆt lors du calcul d'une
expression LEC. Les trois autres valeurs n'ont de sens que pour
l'estimation.

Comme les autres objets, les scalaires sont identifi‚s par un nom de 10
caractŠres maximum  (20 … partir de la version 6.01). Ceux-ci doivent ˆtre en minuscules pour que les
variables soient distinctes des scalaires dans les formules LEC.

&IT Les tableaux
ÄÄÄÄÄÄÄÄÄÄÄÄ
Une des op‚rations le plus souvent effectu‚es dans le cours d'un
exercice de simulation est l'affichage ou l'impression de tableaux et de
graphiques de r‚sultats. Les tableaux ont ‚t‚ imagin‚s pour rendre cette
op‚ration la plus efficace possible.

Chaque tableau est un ensemble de lignes. Une ligne est compos‚e de deux
parties (en g‚n‚ral) :

&EN une partie de texte qui sera le titre de la ligne
&EN une partie formule qui permettra de calculer les valeurs … placer
    dans le tableau

&CO
    TITRE DU TABLEAU
    ----------------
    Produit national brut       PNB
    Ch“mage                     UL
    Balance ext‚rieure          X-I
&TX

Les lignes sont en fait de plusieurs types : titre centr‚ sur la largeur
de la page, lignes de valeurs, lignes de s‚paration, r‚f‚rences, etc.

Un tableau peut ˆtre calcul‚ sur diff‚rentes p‚riodes de temps, d‚crites
par un "sample" du type :

&CO
    1980Y1:10               -->> 10 observations … partir de 1980Y1
    1980Y1, 1985Y1, 1990:5  -->> 1980, 1985, puis 5 observations … partir
				de 1990Y1
    80/79:5                 -->> 5 taux de croissance … partir de 1980
    ...
&TX
Il peut de plus contenir des valeurs en provenance de diff‚rents
fichiers :

&CO
    (1990:5)[1,2,1-2]   -->> valeurs de 1990 … 1994 pour les fichiers
			    1, 2 et pour la diff‚rence entre les deux
			    fichiers.
&TX
Le r‚sultat du calcul peut ˆtre :

&EN affich‚ … l'‚cran
&EN imprim‚
&EN int‚gr‚ dans un rapport

et ce, soit sous forme de tableau de nombres, soit sous forme de
graphique.

&NO Les tableaux peuvent trŠs bien ˆtre utilis‚s dans le cadre d'un
    projet n'int‚grant pas de modŠle ‚conom‚trique : les seules
    informations utilis‚es par les tableaux sont les variables et
    ‚ventuellement les scalaires.

&IT Les variables
ÄÄÄÄÄÄÄÄÄÄÄÄÄ
Les variables sont simplement des s‚ries de nombres. Comme les autres
objets, elles sont organis‚es en workspaces (WS), notion qui est ‚tudi‚e
ci-dessous.

Toutes les variables d'un WS sont d‚finies sur la mˆme p‚riode (sample).
Si des observations sont manquantes, elles prennent la valeur sp‚ciale
NA (Not Available) repr‚sent‚e par --.

Comme les autres objets, les variables sont identifi‚es par un nom de 10
caractŠres maximum (20 … partir de la version 6.01). Ceux-ci doivent ˆtre en majuscules pour que les
variables soient distinctes des scalaires dans les formules LEC.
>

<Les Workspaces>
&ALIAS workspaces

    Les workspaces
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ


On a vu pr‚c‚demment qu'il y avait 7 types d'objets g‚r‚s dans IODE.
Lors d'une session de IODE, l'espace m‚moire est ‚galement divis‚ en 7
parties : chacune est r‚serv‚e … un des types d'objets et est appel‚e
workspace (WS = espace de travail). On a donc en permanence 7 WS
"actifs" au cours d'une session de travail.

Au d‚part, tous les WS sont vides. Des fonctions, ‚tudi‚es dans les
chapitres suivants, permettent de modifier le contenu des WS, soit en
agissant sur des WS entiers, soit sur des objets individuels.

&NO
    Les WS sont stock‚s en m‚moire pendant le dur‚e d'une session IODE. En quittant le
    programme, toutes les donn‚es sont perdues. Pour les
    sauvegarder, il faut utiliser les fonctions de sauvetage de WS.

Parmi les fonctions agissant sur les WS entiers, on trouve notamment :

&EN Load : charge en m‚moire le contenu d'un fichier
&EN Save : stocke l'actuel contenu de WS dans un fichier
&EN Copy : ajoute … un WS des objets copi‚s d'un fichier
&EN Clear : d‚truit tous les objets d'un WS
&EN Sample : change la p‚riode de d‚finition du WS de variables

D'autres fonctions agissent sur la d‚finition des objets : cr‚ation,
modification, destruction, etc.

Les fonctions ‚conom‚triques (estimation, simulation) et la fonction de
construction de s‚ries sur base d'identit‚s agissent indirectement sur
les variables et les scalaires en modifiant leurs valeurs.

&IT R‚sum‚
ÄÄÄÄÄÄ
&EN En d‚but de session, les WS sont charg‚s en m‚moire … partir de
    fichiers

&EN En cours de session, les objets sont modifi‚s, cr‚‚s, d‚truits dans
    les WS

&EN En fin de session (ou … n'importe quel moment), il faut sauver les
    WS dans des fichiers sur disque de fa‡on … pouvoir les r‚cup‚rer
    dans des sessions ult‚rieures.
>


<Le LEC>
Le LEC, langage des formules de IODE
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Qu'il s'agisse d'‚quations, d'identit‚s de construction ou encore de
graphiques, un langage d'‚criture de formules math‚matiques ad‚quat est
indispensable. Le langage LEC (Langage Econom‚trique Condens‚) offre
l'avantage d'ˆtre … la fois concis dans son ‚criture et particuliŠrement
adapt‚ aux formules faisant intervenir des s‚ries chronologiques. Il est
utilis‚ chaque fois qu'une formule est n‚cessaire dans le logiciel IODE.

Le LEC est ‚galement "naturel", en ce sens que sa syntaxe est proche de
l'‚criture de formules que l'on peut trouver dans la litt‚rature.

&CO
    Equation de consommation
    ------------------------
		   Yt
	Ct = a + b -- + c.C                              (texte)
		   Pt      t-1

	C := a + b * Y / P + c * C[-1]                    (LEC)


    Equation de production
    ----------------------
	ln Qt = a ln Kt + (1 - a) ln Lt + c.t + b        (texte)

	ln Q := a * ln K + (1 - a) * ln L + c * t + b     (LEC)
&TX

Le LEC est d‚crit en d‚tail dans un chapitre s‚par‚. Citons
simplement ici quelques caract‚ristiques int‚ressantes dont certaines
apparaissent dans les exemples ci-dessus :

&EN plus de 20 op‚rateurs math‚matiques sont int‚gr‚s dans le langage :
    fonctions trigonom‚triques, hyperboliques, logarithmes,
    exponentielles, max, min, etc

&EN plus de 10 fonctions temporelles : maximum, minimum, somme et
    produit sur une p‚riode, lags, leads, diff‚rences et taux de
    croissance de degr‚ quelconque (y compris calcul‚), moyennes
    mobiles, ‚carts type, etc

&EN les lags, leads et p‚riodes sont ‚crits trŠs simplement, y compris
    sur des expressions et peuvent ˆtre combin‚s :

&CO
	    (A + B)[-1]
	    UL[+1]
	    (X + dln Y)[1985Y1]
&TX

&EN des parties de formules peuvent ˆtre temporairement annul‚es en les
    mettant en commentaire :

&CO
	    ln Q := a * ln K /* + (1 - a) ln (L + Y + Z) */
&TX

&EN des listes (macros) peuvent ˆtre utilis‚es dans des formules :

&CO
	    A + B := c1 + c2 * $LL + c3 * $ZZ
&TX
>
<Rapports>
    Les rapports : scripter IODE
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Un rapport est un fichier ASCII (extension .rep) contenant deux types
d'‚l‚ments :

&EN des instructions (les lignes qui commencent par $ ou ##)
&EN du texte libre (les autres lignes)

L'ex‚cution d'un rapport se traduit d'une part par l'enchaŒnement
d'op‚rations (charger un fichier, estimer des ‚quations, imprimer un
tableau, etc) et d'autre part par l'impression d'un document "fini".

Les instructions commen‡ant par $ sont des instructions qui n'affichent
rien … l'‚cran. Celles commen‡ant par ## font intervenir l'‚cran, soit
pour afficher des informations, soit pour poser des questions.

On trouve plusieurs groupes d'instructions :

&EN celles permettant de contr“ler le d‚roulement d'un rapport (Label,
    GoTo, Ask, Return, Quit, OnError, Foreach, ProcExec, etc)


&EN celles ex‚cutant des fonctions de IODE (WsLoadEqs, WsSaveVar,
    FileDeleteA2m, DataEditVar, PrintTbl, ModelSimulate, ReportExec, etc)

Les instructions du second type permettent en fait
d'effectuer pratiquement toutes les op‚rations effectu‚es normalement …
partir des menus de IODE.

Avant d'ˆtre ex‚cut‚e, chaque ligne de rapport est analys‚e et, si
n‚cessaire, modifi‚e : on peut ainsi utiliser des
macros, remplacer des listes par leur valeur, effectuer des calculs...


&IT Exemples de rapports
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

L'exemple qui suit charge des Workspace IODE de diff‚rents
types. Ensuite, il fixe la destination des impressions (ici dans un fichier de
format interm‚diaire, ‚tudi‚ plus tard). Enfin, un beep sonore est produit
et un message indique que l'environnement est charg‚.

Typiquement, l'ex‚cution de ce rapport d‚bute une session IODE en fixant
un environnement initial.

&CO
    $WsLoadVar fun\fun.var
    $WsLoadEqs fun\fun.eqs
    $WsLoadScl fun\fun.scl

    $PrintDest tmp.a2m A2M
    ##Beep
    ##Show Environnement fun charg‚
&TX

Le rapport suivant d‚finit le fichier output de l'ex‚cution
(bist92\bistelf1.rtf), affiche ensuite un message, construit les
tableaux et les imprime. Du
texte libre est int‚gr‚ dans ce rapport ("TABLEAUX DES HYPOTHESES" par
exemple).

&CO
    $PrintDest bist92\bistelf1.rtf RTF
    ##Show processing french tables
    $PrintNbdec 1
	TABLEAUX DES HYPOTHESES
	-----------------------

    $PrintTbl 89:8 HYPEIR
    $PrintTbl 89/88:8 HYPEIIR

    $PrintNbdec 0
    .page
	TABLEAUX DES RESULTATS
	----------------------

    $PrintTbl 89:8 RESL00

    $PrintNbdec 1
    $PrintTbl 89/88:8 RESL00R
    $PrintTbl 89:8 RESL03
    ##Beep
&TX

&IT Le format A2M
ÄÄÄÄÄÄÄÄÄÄÄÄÄ
Le format A2M est d‚crit en d‚tail dans un chapitre s‚par‚.

En r‚sum‚, un fichier A2M est un texte ASCII entrecoup‚ d'instructions
d'enrichissement typographiques et de structuration d'un document en
paragraphes, tableaux et graphiques. Ce langage est utilis‚ en interne par
IODE comme langage interm‚diaire. Des traducteurs internes … IODE permettent
ensuite d'imprimer un fichier A2M ou de le traduire vers des formats de
fichiers comme Word ou Frame.

Il est ‚galement loisible … l'utilisateur d'enrichir ses impressions
en pla‡ant dans ses rapports des commandes de formattage A2M. Celles-ci
seront interpr‚t‚es par IODE lors de l'envoi vers une imprimante ou vers
un document MIF, HTML, RTF ou CSV.

L'utilisateur peut ‚galement d‚cider de sauvegarder ce format A2M pour
une impression ult‚rieure ou pour produire plusieurs formats diff‚rents.

Les programmes IODE et A2M interprŠtent un fichier au format A2M et
permettent de g‚n‚rer :

&EN une impression sur une des imprimantes d‚finies sous Windows

&EN un fichier Rtf destin‚ … l'importation par exemple dans MS-Word

&EN un fichier Html destin‚ … la publication sur Internet

&EN un fichier Csv destin‚ … l'importation dans MS-Excel ou Lotus

&EN un fichier Mif destin‚ … l'importation dans Frame Maker

Grƒce … cette fonctionnalit‚, le choix de la destination des impressions
permet d'importer ais‚ment les ‚tats (tableaux, graphiques, etc) dans un
programme de traitement de texte ou dans un tableur.

Notons que les fichiers A2M sont exploitables en dehors de IODE, … l'aide
du programme ~cscr4w_at.~C
>









