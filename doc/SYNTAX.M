/* syntax */

<Syntaxe du programme IODE>
    Syntaxe du programme IODE
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Le programme IODE peut ˆtre lanc‚ avec ou sans argument.

Les arguments ‚ventuels pass‚s … IODE servent … ex‚cuter certaines
tƒches au lancement de IODE, comme charger un workspace ou lancer
l'ex‚cution d'un rapport.

En utilisant la commande

&CO
    c:\iode>> iode -h
&TX

la syntaxe de IODE est affich‚e.

....~a'iodesyntax.png'


&TI Syntaxe de IODE
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&CO

    OPTION   ARGUMENTS     DESCRIPTION
    ------   ---------     -----------
    -h                     display this message and exit
    -ws                    load ws.* at startup
    -rep      reportname   ex‚cute le rapport indiqu‚
    -erep     reportname   ‚dite le rapport indiqu‚
    -l        wsname       lance IODE et charge un WS
    -p        wsname       lance IODE et imprime un WS
    -nc       number       set the number of columns of the iode window
    -nl       number       set the number of lines of the iode window
    -fontsize nPts         set the character size (in points : [3-50]
    -fontname fontname     set the font (default Courier New)
    -inifile  filename     file containing iode parms

    Advanced parameters
    -------------------
    -seg      segsize      set the allocation segment size (dft 16384)
    -pbf      filename     execute a 'play back' file
    -pbs      string       execute a 'play back' string
    -rec      filename     record keys in a playback file
&TX

&IT L'option -ws
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette option charge les workspaces ws.* pr‚sents dans le directory
d'o— IODE est lanc‚.

&IT L'option -rep
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette option permet de d‚marrer un rapport automatiquement. Cela offre
la possibilit‚ par exemple de cr‚er un environnement de travail ou
encore de d‚marrer une proc‚dure d'encodage ou d'exemple.

On entrera par exemple :

&CO
    c:\usr\iode>> iode -rep simul.rep
&TX

&IT L'option -erep
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette option permet de lancer l'‚diteur de rapports automatiquement.
Contrairement … l'option -rep, celle-ci n'ex‚cute pas le rapport.

On entrera par exemple :

&CO
    c:\usr\iode>> iode -erep simul.rep
&TX

&IT L'option -l
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette option permet de d‚marrer IODE et de charger automatiquent un WS.

On entrera par exemple :

&CO
    c:\usr\iode>> iode -l mydata.var
&TX

&IT L'option -p
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette option permet de d‚marrer IODE et d'imprimer automatiquent la
d‚finition des objets d'un WS.

On entrera par exemple :

&CO
    c:\usr\iode>> iode -p mydata.eqs
&TX

&IT Les options -nl et -nc
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Ces options permettent de changer au d‚marrage la taille de la fenˆtre de IODE.
La taille est exprim‚e en caractŠres.

Elles sont prioritaires par rapport aux d‚finitions du fichier iode.ini.

On entrera par exemple :

&CO
    c:\usr\iode>> iode -nl 35 -nc 100
&TX


&IT Les opions -fontsize et -fontname
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Ces options d‚terminent la taille et la police de caractŠres de la fenˆtre de IODE.

&CO
    c:\iode>> iode -fontsize 10 -fontname Terminal

Elles sont prioritaires par rapport aux d‚finitions du fichier iode.ini.


&IT Les options -rec, -pbf et -pbs
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ


L'option ~c-rec~C enregistre dans un fichier les s‚quences de touches
utilis‚es pendant une session IODE.

&CO
    c:\iode>> iode -rec filename
&TX

Le fichier (ici ~cfilename~C) contiendra des s‚quences de touches codifi‚es
selon la syntaxe d‚crite dans la version 6.27 de iode. Ce fichier peut ˆtre
‚dit‚ … l'aide d'un ‚diteur ascii.

Ce fichier de s‚quences de touches peut ensuite ˆtre "rejou‚"
automatiquement… l'aide de l'option ~c-pbf~C.

&CO
    c:\iode>> iode -pbf reckeys.keys
&TX

Cela permet par exemple :
&EN de lancer des op‚rations r‚p‚titives sans avoir … passer par les rapports
&EN de lancer des proc‚dures de test unitaires

Toutes les op‚rations ne sont cependant pas exploitables : les op‚rations
effectu‚es … l'aide de la souris ne sont pas support‚es. Il faut ‚galement
ˆtre attentif au fait que la position dans un menu ou le contenu d'un ‚cran
de saisie peuvent changer entre deux lancements successifs de IODE.

L'option ~c-pbs~C permet de "jouer" une s‚quence de touches pass‚es comme paramŠtre.

&CO
    c:\iode>> iode -pbs "&aw&ar@R"
&TX


&IT L'option -seg
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Cette option permet de d‚finir la taille des blocs m‚moire allou‚s par IODE.

&CO
    c:\iode>> iode -segsize 32768
&TX
>


<Syntaxe du programme IODECMD>
    Syntaxe du programme IODECMD
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ


&TI Syntaxe
ÄÄÄÄÄÄÄÄÄÄÄ

&CO
    iodecmd [-nbrun n] [-alloclog filename] [-v] [-y] [-h] reportfile [arg1] ... [argn]
&TX

where :
&EN ~c-nbrun n~C : n = nb of repetitions of the report execution (default 1)
&EN ~c-allocdoc filename~C : memory debugging info stored in filename (developers only)
&EN ~c-v~C : verbose version (all messages displayed)
&EN ~c-y~C : answer yes to all questions asked during the report execution
&EN ~c-h~C : display program syntax (this message)
&EN ~creportfile arg1 ... ~C : IODE report to execute including its optional arguments
>











