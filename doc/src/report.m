/* REPORT */

<Report>
   Report
   ÄÄÄÄÄÄ

.~a'mrep.png'

En utilisant les rapports, les gains de productivit‚ peuvent ˆtre
consid‚rables. IODE dispose des outils n‚cessaires pour automatiser
l'enchaŒnement d'une s‚rie d'op‚rations et imprimer le rapport
correspondant.

Il est possible de r‚aliser de r‚els programmes … l'aide des
rapports de IODE. Le d‚roulement d'un rapport peut ˆtre contr“l‚ de
maniŠre interactive (fonctions ASK et PROMPT) ou calcul‚e (GOTO). Par
l'utilisation des macros (define) et des arguments des rapports, un
mˆme rapport peut ˆtre utilis‚ dans diff‚rentes situations.

La th‚orie sur la construction des rapports peut ˆtre trouv‚e dans le
chapitre consacr‚ aux rapports.

La suite du chapitre d‚taille les op‚rations n‚cessaires … la production
et … l'exploitation des rapports.

&EN <Edit report           > : ‚dition ou modification d'un rapport
&EN <Execute report        > : ex‚cution d'un rapport
&EN <Execute a report line > : ex‚cution d'une ligne de rapport
>
<Edit report>
   Edit report
   ÄÄÄÄÄÄÄÄÄÄÄ

.~a'pedrep.png'

Cette fonction permet de cr‚er ou de modifier un rapport.

Le nom du fichier … ‚diter ou … cr‚er est indiqu‚ dans le champ de
saisie de la page. La touche ~bF10~B lance l'‚dition du fichier avec
l'‚diteur MMT (voir Annexes).

.~a'mmtrep.png'

La touche ~bENTER~B permet de consulter la liste des fichiers pr‚sents sur
le disque.


Des ‚diteurs comme Notepad++ ou Textpad peuvent remplacer avantageusement
l'‚diteur de rapport. Des fichiers de configuration adapt‚s aux rapports
sont disponibles pour ces deux ligiciels sur le site iode.plan.be.

>

<Execute report>
   Execute report
   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
.~a'pexrep.png'

Cette fonction lance l'interpr‚tation, l'ex‚cution et l'impression d'un
rapport.

La page de saisie  comprend les champs suivants:

&EN ~bReport name~B : nom du fichier de commandes … ex‚cuter

&EN ~bReport parameters~B : paramŠtres du rapport (r‚ferenc‚s … l'int‚rieur
    du rapport par %n%). Les paramŠtres sont s‚par‚s par des blancs, des
    virgules ou des points-virgules. Ils doivent tous ˆtre plac‚s sur la
    mˆme ligne.

&EN ~bLanguage~B : permet de sp‚cifier la langue dans laquelle les ‚ventuels
    textes fixes (dans les tableaux, par exemple) devront ˆtre imprim‚s.

&EN ~bNb Dec~B : nombre de d‚cimales par d‚faut lors de l'impression des
    variables ou de tableaux.


Les impressions se font avec le nombre de d‚cimales et dans le langage
d‚fini dans les derniers champs de la page, sauf si ces valeurs sont
modifi‚es dans le rapport.

Si des messages appropri‚s ont ‚t‚ pr‚vus (commande show et/ou beep),
l'utilisateur peut suivre le d‚roulement des op‚rations par les messages
qui s'affichent dans la fenˆtre de commentaires de IODE. Si des commandes
"plein ‚cran" sont impl‚ment‚es dans le rapport, l'utilisateur est amen‚
en cours d'ex‚cution … compl‚ter les pages de saisie correspondantes.
>


<Execute a report line >
    Execute a report line
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

.~a'pexrepl.png'

Cette fonction lance l'interpr‚tation, l'ex‚cution et l'impression d'une
ligne de commande. Les commandes valides sont toutes les commandes de
rapport (voir le chapitre sur les Rapports).

IODE ex‚cute la commande indiqu‚e puis rend la main … l'utilisateur.

L'‚cran permet en outre de pr‚ciser deux valeurs:

&EN ~bLanguage~B : permet de sp‚cifier la langue dans laquelle les ‚ventuels
    textes fixes (dans les tableaux, par exemple) devront ˆtre imprim‚s.

&EN ~bNb Dec~B : nombre de d‚cimales par d‚faut lors de l'impression des
    variables ou de tableaux.
>










