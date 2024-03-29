/* syntax */

<Syntaxe du programme IODE>
    Syntaxe du programme IODE
    컴컴컴컴컴컴컴컴컴컴컴컴�

Le programme IODE peut 늯re lanc� avec ou sans argument.

Les arguments 굒entuels pass굎 � IODE servent � ex괹uter certaines
t긟hes au lancement de IODE, comme charger un workspace ou lancer
l'ex괹ution d'un rapport.

En utilisant la commande

&CO
    c:\iode>> iode -h
&TX

la syntaxe de IODE est affich괻.

....~a'iodesyntax.png'


&TI Syntaxe de IODE
컴컴컴컴컴컴컴�
&CO

    OPTION   ARGUMENTS     DESCRIPTION
    ------   ---------     -----------
    -h                     display this message and exit
    -ws                    load ws.* at startup
    -rep      reportname   ex괹ute le rapport indiqu�
    -erep     reportname   괺ite le rapport indiqu�
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
컴컴컴컴컴컴컴컴

Cette option charge les workspaces ws.* pr굎ents dans le directory
d'o� IODE est lanc�.

&IT L'option -rep
컴컴컴컴컴컴컴컴�

Cette option permet de d굆arrer un rapport automatiquement. Cela offre
la possibilit� par exemple de cr괻r un environnement de travail ou
encore de d굆arrer une proc괺ure d'encodage ou d'exemple.

On entrera par exemple :

&CO
    c:\usr\iode>> iode -rep simul.rep
&TX

&IT L'option -erep
컴컴컴컴컴컴컴컴�

Cette option permet de lancer l'괺iteur de rapports automatiquement.
Contrairement � l'option -rep, celle-ci n'ex괹ute pas le rapport.

On entrera par exemple :

&CO
    c:\usr\iode>> iode -erep simul.rep
&TX

&IT L'option -l
컴컴컴컴컴컴컴�

Cette option permet de d굆arrer IODE et de charger automatiquent un WS.

On entrera par exemple :

&CO
    c:\usr\iode>> iode -l mydata.var
&TX

&IT L'option -p
컴컴컴컴컴컴컴�

Cette option permet de d굆arrer IODE et d'imprimer automatiquent la
d괽inition des objets d'un WS.

On entrera par exemple :

&CO
    c:\usr\iode>> iode -p mydata.eqs
&TX

&IT Les options -nl et -nc
컴컴컴컴컴컴컴�

Ces options permettent de changer au d굆arrage la taille de la fen늯re de IODE.
La taille est exprim괻 en caract둹es.

Elles sont prioritaires par rapport aux d괽initions du fichier iode.ini.

On entrera par exemple :

&CO
    c:\usr\iode>> iode -nl 35 -nc 100
&TX


&IT Les opions -fontsize et -fontname
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
Ces options d굏erminent la taille et la police de caract둹es de la fen늯re de IODE.

&CO
    c:\iode>> iode -fontsize 10 -fontname Terminal

Elles sont prioritaires par rapport aux d괽initions du fichier iode.ini.


&IT Les options -rec, -pbf et -pbs
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴


L'option ~c-rec~C enregistre dans un fichier les s굌uences de touches
utilis괻s pendant une session IODE.

&CO
    c:\iode>> iode -rec filename
&TX

Le fichier (ici ~cfilename~C) contiendra des s굌uences de touches codifi괻s
selon la syntaxe d괹rite dans la version 6.27 de iode. Ce fichier peut 늯re
괺it� � l'aide d'un 괺iteur ascii.

Ce fichier de s굌uences de touches peut ensuite 늯re "rejou�"
automatiquement� l'aide de l'option ~c-pbf~C.

&CO
    c:\iode>> iode -pbf reckeys.keys
&TX

Cela permet par exemple :
&EN de lancer des op굍ations r굋굏itives sans avoir � passer par les rapports
&EN de lancer des proc괺ures de test unitaires

Toutes les op굍ations ne sont cependant pas exploitables : les op굍ations
effectu괻s � l'aide de la souris ne sont pas support괻s. Il faut 괾alement
늯re attentif au fait que la position dans un menu ou le contenu d'un 괹ran
de saisie peuvent changer entre deux lancements successifs de IODE.

L'option ~c-pbs~C permet de "jouer" une s굌uence de touches pass괻s comme param둻re.

&CO
    c:\iode>> iode -pbs "&aw&ar@R"
&TX


&IT L'option -seg
컴컴컴컴컴컴컴컴컴
Cette option permet de d괽inir la taille des blocs m굆oire allou굎 par IODE.

&CO
    c:\iode>> iode -segsize 32768
&TX
>


<Syntaxe du programme IODECMD>
    Syntaxe du programme IODECMD
    컴컴컴컴컴컴컴컴컴컴컴컴컴컴


&TI Syntaxe
컴컴컴컴컴�

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











