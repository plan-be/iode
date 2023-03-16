
<Le programme IODECMD>
    Le programme iodecmd
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Ce programme permet d'ex‚cuter un rapport IODE sans passer par l'interface de IODE.

A l'aide du langage de commandes des rapports IODE, il est ainsi possible d'automatiser
la plupart des op‚rations effectu‚es normalement dans l'interface de IODE.


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


&IT -nbrun
ÄÄÄÄÄÄÄÄÄÄÄ
Cet option permet de relancer plusieurs fois un mˆme rapport. Cela peut servir …
d‚terminer une instabilit‚, un "memory leak" ou encore … effectuer des tests de performance.


&IT -alloclog filename
ÄÄÄÄÄÄÄÄÄÄÄ
Le fichier filename contiendra en fin de traitement la liste des allocations m‚moire non
d‚sallou‚es. Cette option a ‚t‚ cr‚‚e … des fins de debugging de IODE.

&IT -y ou --forceyes
ÄÄÄÄÄÄÄÄÄÄÄ
Cette option permet de continuer automatiquement l'ex‚cution lorsqu'une question est pos‚e
… l'utilisateur. La r‚ponse automatique est toujours positive.


&IT -v ou --verbose
ÄÄÄÄÄÄÄÄÄÄÄ
Cette option donne des informations sur l'‚tat de la m‚moire aprŠs chaque it‚ration.

Exemple :
&CO
    c:\>> iodecmd -nbrun 2 -v example.foreach.rep
&TX

R‚sultat:
&CO
    IODE Modeling Software 6.56 - (c) 1990-2017 Planning Office - Brussels
    PIB[1990Y1] = 0 +  + 0 =
    PIB[1991Y1] = 1 + 0 + 0.84147098 = 1.841471
    ....
    PIB[1999Y1] = 9 + 2.1972246 + 0.41211849 = 11.609343
    PIB[2000Y1] = 10 + 2.3025851 + -0.54402111 = 11.758564
    *****Run 1/2 *****
    rc = 0 -- total residual allocation : 4010740
    PIB[1990Y1] = 0 +  + 0 =
    PIB[1991Y1] = 1 + 0 + 0.84147098 = 1.841471
    ...
    PIB[1999Y1] = 9 + 2.1972246 + 0.41211849 = 11.609343
    PIB[2000Y1] = 10 + 2.3025851 + -0.54402111 = 11.758564
    *****Run 2/2 *****
    rc = 0 -- total residual allocation : 6124268
    After last run : 6124268 bytes allocated
&TX

&IT -h ou --help
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Affiche la syntaxe du programme.

>










