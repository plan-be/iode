/* COMPUTE */

<Compute>
   Compute
   �������

.~a'mcomp.png'

Ce menu comprend les fonctions principales de calcul :

&EN <Simulation - Goal Seeking> : simulation d'un mod�le et recherche
				d'objectifs

&EN <Compile Model> : recompile une liste d'�quations

&EN <SCC Decomposition> : calcule les composantes fortement connexes du
mod�le et les r�ordonne.


&EN <SCC Simulation> : simule un mod�le pr�alablement d�compos� en
composantes fortement connexes.

&EN <Execute Identities> : calcul d'un groupe d'identit�s de construction et
cr�ation ou modification des s�ries associ�es
>

<Model residual check>
   Model residual check
   ��������������������

Cette fonction n'est pas encore impl�ment�e.
>

<Simulation - Goal Seeking>
   Simulation d'un mod�le
   ����������������������


.~a'psim1.png'

&TI D�finition d'un mod�le
����������������������

Un mod�le, au sens de IODE, est simplement une liste d'�quations. Aucune
autre construction n'est n�cessaire : de cette fa�on, pour modifier un
mod�le, il suffit de modifier la liste qui le d�finit. Par exemple, si
un mod�le est d�coup� logiquement en 5 blocs, on d�finira 5 listes
d'�quations : le mod�le sera d�fini par la liste contenant les 5
"sous-listes".

&CO
    D�finition des listes
    ---------------------
    BLOC1 : A, B, C, D
    BLOC2 : X, Y
    BLOC3 : C1, C2, C3, C4
    BLOC4 : X_A1, X_A2, X_A3
    BLOC5 : R1, R2, R3, S_12

    MODSIM : $BLOC1, $BLOC2, $BLOC3, $BLOC4, $BLOC5
&TX

Pour un mod�le de quelques �quations, il n'est pas indispensable de
d�finir des listes de noms : les noms des �quations peuvent �tre fournis
in extenso dans l'�cran de simulation.

Modifier un mod�le (supprimer ou ajouter un bloc par exemple), revient
donc � changer la liste des �quations d�finissant le mod�le.

&NO
    Comme la variable endog�ne d'une �quation porte le nom de
    l'�quation, il n'est pas possible de placer dans un mod�le
    deux �quations ayant m�me variable endog�ne.

Pour simuler un mod�le, il faut que toutes les �quations du mod�le
soient pr�sentes dans le WS d'�quations, que toutes les variables et
tous les scalaires utilis�s dans les �quations du mod�le soient d�finis
dans les WS de variables et de scalaires. Les valeurs des exog�nes et
des scalaires ne peuvent �tre NA (Not Available) sur la p�riode utile.

&TI D�finition du mod�le, de la p�riode et des param�tres
    �����������������������������������������������������


La premi�re partie de l'�cran permet de sp�cifier la liste des �quations
qui d�finissent le mod�le, la partie inf�rieure de l'�cran d�finit les
param�tres techniques de la simulation.

&IT Liste d'�quations
�����������������
En g�n�ral, on d�finira une liste d'�quations (avec des sous-listes)
plut�t que d'introduire dans l'�cran tous les noms des �quations.

L'ordre d'introduction des �quations dans la liste peut influencer le
comportement de l'algorithme de simulation. En effet, comme il s'agit
d'un algorithme de type Gauss-Seidel, les informations calcul�es sont
directement utilis�es dans la suite du calcul. Si X d�pend de Y, il vaut
donc mieux placer Y avant X dans la liste d'�quations.

Si toutes les �quations du WS font partie du mod�le, on peut laisser la
liste vide.

&IT Exchange Endo-Exo
��������������������
Ce champ permet de sp�cifier un ensemble de couples endog�ne-exog�ne afin
de r�aliser une recherche d'objectif (Goal seeking). Pour chaque paire,
le statut des variables est �chang� : l'endog�ne devient exog�ne et
inversement. Cela permet de faire passer le mod�le par des valeurs
connues des endog�nes et de d�duire les valeurs n�cessaires des exog�nes
associ�es. Les exog�nes prennent la valeur calcul�e sur toute la fin de
la p�riode du WS.

&IT P�riode de simulation
���������������������
La p�riode de simulation est indiqu�e par les deux p�riodes qui en
d�finissent les limites. La p�riode indiqu�e doit faire partie de la
p�riode de d�finition du WS de variables. Les variables exog�nes (non
calcul�es par le mod�le) doivent �tre d�finies (non NA). Il en va de
m�me pour les scalaires et les variables endog�nes "lagg�es".

&IT Convergence
�����������
Indique le seuil de convergence en-de�� duquel la simulation se termine
positivement pour la p�riode courante. En d'autres termes, pour une
p�riode donn�e, lorsque les diff�rences relatives entre deux it�rations
successives (k et k-1)

&CO
    ||Y[k] - Y[k-1]||
    -----------------
       ||Y[k]||
&TX

sont inf�rieures � ce seuil pour toutes les variables endog�nes, on
consid�re que le mod�le a converg�.

&IT Maximum number of iterations
����������������������������
Si apr�s le nombre d'it�rations indiqu�, le mod�le n'a pas converg�, le
processus s'arr�te sur un message d'erreur. Ce param�tre permet d'�viter
les bouclages.

&IT Relaxation parameter
��������������������
Ce param�tre doit �tre compris entre 0 et 1. Il permet de modifier le
comportement de l'algorithme de la fa�on suivante : la variable X,
calcul�e par l'�quation du mod�le associ�e, n'est pas conserv�e telle
quelle pour les it�rations suivantes, mais remplac�e par

&CO
    X[k] = X' * lambda + X[k-1] * (1 - lambda)

       o� X'       est le r�sultat du calcul de l'�quation
	  lambda   est le param�tre de relaxation
	  X[k - 1] est le r�sultat du calcul � l'it�ration pr�c�dente
	  X[k]     est la nouvelle valeur
&TX

On constate que si lambda vaut 1, X[k] est le r�sultat du calcul de
l'�quation sans modification. S'il vaut 0, X ne changera jamais. Entre
les deux, une partie de la valeur de l'it�ration pr�c�dente est
conserv�e dans la nouvelle valeur de X, �vitant ainsi un comportement
parfois trop "brutal" du mod�le.

Ce param�tre est � utiliser en fixant une valeur entre 0.5 et 1
lorsqu'un mod�le ne converge pas ou qu'il converge trop lentement.

&IT Debug
�����

L'option debug g�n�re automatiquement des listes comprenant des
�quations pr�- et post-r�cursives : ~c_PRE~C, ~c_INTER~C et ~c_POST~C.

Le fichier de trace (simul.dbg) comprenant la liste des �quations pour lesquelles une
sous-it�ration (Newton-Raphson) a �t� n�cessaire et le nombre de
sous-it�rations peut �tre g�n�r� via la commande de rapport correspondant.


&IT Sorting algorithm
�����������������
La liste des �quations qui d�finit le mod�le peut �tre utilement
r�organis�e de fa�on � acc�l�rer la simulation. Deux m�thodes sont
utilisables s�par�ment ou en combinaison. Toutes deux font appel � des
algorithmes de th�orie des graphes.

&EN D�composition en composantes fortement connexes : il s'agit de
    d�couper le mod�le en blocs non interd�pendants. Cette m�thode
    permet de limiter le nombre d'�quations � calculer en se basant sur
    les d�pendances des �quations les unes par rapport aux autres. Dans
    l'exemple suivant, les blocs 1 et 3 sont non interd�pendants, ce qui
    signifie qu'un seul calcul est n�cessaire par p�riode. Ils n'entrent
    pas dans la boucle de simulation. Le param�tre de relaxation n'est
    pas utilis� dans ces blocs. Le bloc 2 est la partie interd�pendante
    du mod�le. Il faut it�rer sur ce bloc et sur lui seul. On r�duit
    ainsi le nombre d'�quations � r�soudre � 3 au lieu des 6 qui forment
    en r�alit� le mod�le. La d�composition peut donner lieu � plus de
    trois blocs :

&CO
       Soient les �quations suivantes :

       A = f1(B,C,D)
       B = f2(C,A)
       C = f3(B,X)
       D = f4(X,Y)
       X = f5(Y)
       Z = f6(A,B,C,X)

       Les blocs suivants sont construits (dans cet ordre)

	    1 :   X = f5(Y)       ne d�pend que de Y exog�ne
		  D = f4(X,Y)     d�pend de X

	    2 :   A = f1(B,C,D)   boucle avec B et C
		  B = f2(C,A)     boucle avec A et C
		  C = f3(B,X)     boucle avec B, donc avec A

	    3 :   Z = f6(A,B,C,X) n'est utilis� par aucune �quation
&TX


&EN Pseudo-triangulation : cette m�thode r�organise les �quations �
    l'aide d'un algorithme sp�cifique. En gros, les d�pendances sont
    analys�es et les �quations sont organis�es en cons�quence : si
    B d�pend de A, B est plac� apr�s A, etc. Plusieurs passages sont en
    g�n�ral utiles pour obtenir un r�sultat satisfaisant. Le nombre
    optimal de passages est fonction du mod�le et laiss� au choix de
    l'utilisateur.

Une combinaison des deux m�thodes est possible. Si l'ordre de la liste est
consid�r� comme optimal, on peut supprimer toute m�thode de r�ordonnancement
(Order = 'None') ou se contenter de la m�thode des composantes connexes.

Pour conserver l'ordre d'une liste donn�e, il faut fournir cette liste ET
pr�ciser None comme algorithme de r�ordonnancement.


&IT Pseudo-triangulation passes
���������������������������
Il s'agit du nombre de passages � effectuer � l'aide de l'algorithme de
pseudo-triangulation.

&IT Starting values
���������������
Au d�but de chaque p�riode � simuler, une valeur de d�part doit �tre
choisie pour chaque variable endog�ne. Cette valeur peut �tre :

&EN Y := Y[-1], if Y null or NA : chaque endog�ne nulle ou NA au d�part
    prend la valeur de la p�riode pr�c�dente

&EN Y := Y[-1], always : chaque endog�ne prend au d�part la valeur de la
    p�riode pr�c�dente

&EN Y := extrapolation, if Y null or NA : chaque endog�ne nulle ou NA
    prend comme valeur une extrapolation lin�aire des deux p�riodes
    pr�c�dentes

&EN Y := extrapolation, always : chaque endog�ne prend comme valeur une
    extrapolation lin�aire des deux p�riodes pr�c�dentes, qu'elle soit
    nulle ou non au d�part

&EN Y unchanged : les endog�nes ne sont pas initialis�es. Elles gardent
    leur valeur qu'elle soit nulle ou non.

&EN Y := Y[-1], if Y = NA : chaque valeur NA prend la valeur de la
    p�riode pr�c�dente

&EN Y := extrapolation, if Y = NA : chaque valeur NA
    prend comme valeur une extrapolation lin�aire des deux p�riodes
    pr�c�dentes


&IT Touches fonctions
�����������������
&EN ~cESCAPE~C quitte la page sans effectuer la simulation
&EN ~cF10~C lance la simulation.

&TI Simulation
    ����������

Lorsque la touche F10 est press�e dans l'�cran de simulation, le
processus math�matique de simulation est lanc�.

A la fin de chaque it�ration, la diff�rence entre les deux it�rations
est indiqu�e. Le processus continue jusqu'� convergence, jusqu'au
d�passement du nombre maximum d'it�rations ou jusqu'� une erreur de
calcul �ventuelle.

Si le mod�le ne converge pas ou s'arr�te sur une erreur de calcul, les
valeurs calcul�es pour les variables endog�nes sont conserv�es. Elles
peuvent donc �tre consult�es pour essayer de d�terminer l'origine du
probl�me.

&IT Sous-algorithme de Newton
�������������������������

Lorsqu'une �quation n'est pas d�finie explicitement par rapport � son
endog�ne ou lorsque celle-ci appara�t plusieurs fois dans l'�quation,
l'algorithme de simulation essaie de r�soudre cette �quation par une
m�thode de Newton. Si cette m�thode ne fournit pas de r�sultat, une
m�thode de type s�cante recherche � son tour une solution pour
l'�quation.

Il n'est cependant pas garanti qu'une solution soit obtenue dans tous
les cas. Les fonctions non continues (singularit�s) comme
&CO
	X := a + b * 1 / (X + 1)
&TX
peuvent �tre impossibles � r�soudre autour de leur point singulier.
Dans le cas de ce type de probl�me, la seule solution est de
modifier la forme de l'�quation :
&CO
	(X - a) * (X + 1) := b
&TX

&TI Interruption de la simulation
�����������������������������
La simulation peut �tre interrompue par l'utilisateur en pressant la
touche ESCAPE. L'arr�t ne se produira qu'� la fin d'une
it�ration. En fonction de la taille du mod�le et de la machine sur
laquelle on travaille, cela peut donc prendre jusqu'� quelques secondes.

Un message permet � l'utilisateur de continuer ou de stopper la
simulation. En cas d'arr�t, les valeurs calcul�es des s�ries sont
conserv�es dans les variables.
>


<Compile Model>
    Compile Model
    �������������

.~a'pcompile.png'

Cette fonction recompile une liste d'�quations ou toutes les �quations
si aucune liste n'est sp�cifi�e.

Pour comprendre l'utilit� �ventuelle de ce module, il faut se rappeler
que les �quations stock�es en WS (en m�moire ou sur disque) sont
compos�es de plusieurs �l�ments : le forme LEC, les inputs d'estimation
et - ce qui nous occupe plus particuli�rement ici - la forme compil�e de
l'�quation.

D'autre part, les formes LEC peuvent contenir des macros ($liste). Si
une liste apparaissant dans une �quation est modifi�e, l'�quation
compil�e ne correspond donc plus � la forme LEC. Rien en effet (sauf �
le v�rifier � tout moment) ne permet de savoir qu'une liste donn�e
appartient � la d�finition d'une �quation.

Ce cas peut se pr�senter lorsqu'un nouveau fichier de listes est charg�
en m�moire, lorsqu'une liste est modifi�e manuellement ou copi�e du
disque, ou encore lorsqu'un fichier d'�quations est charg� en m�moire.

En cas de doute, il est facile de "synchroniser" les �quations et leurs
macros en recompilant toutes les �quations � l'aide de la fonction
"Recompile Model".
>


<SCC Decomposition>
    SCC Decomposition
    �����������������

.~a'sccdec.png'

Cette fonction d�compose le mod�le en composantes fortement connexes (CFC ou
SCC pour Strongly Connex Components) et le r�ordonnancer. Trois listes
sont donc cr��es : �quations pr�r�cursives, interd�pendantes et
postr�cursives.

Lors du r�ordonnancement du mod�le, le nombre d'it�rations de triangulation
(tri) pour le block interd�pendant doit �tre sp�cifi�. Cette valeur n'a
�videmment d'effet que sur la liste des �quations interd�pendantes.

Ces 3 listes ne doivent �tre contruites qu'une seule fois pour une version
donn�e du mod�le.


Voir �galement <SCC Simulation> pour la partie simulation..


&IT Choix du nombre de tris
���������������������������

Apr�s la d�compostion en CFC, le bloc interd�pendant est tri� pour augmenter la vitesse
de la simulation. Le nombre de passage de l'algorithme de tri peut �tre
sp�cifi� � plusieurs endroits :

&EN Dans l'�cran de simulation "standard" : param�tre "Passes" fix�
&EN Dans l'�cran de calcul de la d�composition du mod�le :  param�tre "Triangulation Iterations"
&EN Comme param�tre dans la commande rapport $ModelCalcSCC
&EN Comme 7�me param�tre dans la commande rapport $ModelSimulateParms

>

<SCC Simulation>
    SCC Simulation
    ��������������

.~a'sccsim.png'


Cette fonction simule un mod�le pr�alablement d�compos� en composantes
fortement connexes et r�ordonn�.

Pour am�liorer la performance au d�marrage de la simulation, le processus de simulation
a �t� divis� en 2 �tapes. La premi�re ne s'occupe que du r�ordonnancement du
mod�le (voir <SCC Decomposition>, la seconde de la simulation.

Cette seconde �tape lance la simulation du mod�le sur base des trois listes
pr�alablement construites par la fonction $ModelCalcSCC (ou � la main).

Les autres param�tres sont semblables � ceux de la fonction standard de simulation.

La version rapport de la fonction est :

&CO
    $ModelSimulateSCC from to pre inter post
&TX
o� :
&EN from et to d�terminent le sample de simulation
&EN pre, inter et post sont les listes qui d�finissent l'ordre d'ex�cution du mod�le.

>

<Execute Identities>
   Execute Identities
   ������������������

Une identit� est une forme LEC qui indique comment une s�rie doit �tre
construite. Le programme d'ex�cution calcule toutes les identit�s
demand�es et sauve dans le WS de s�ries courant le r�sultat des calculs.

Ces identit�s sont ordonnanc�es dans l'ordre logique d'ex�cution : si A
d�pend de B, B sera calcul� avant A. L'utilisateur n'a pas � se
pr�occuper de cet ordonnancement. Si une boucle est d�tect�e dans
les identit�s, l'ex�cution ne peut commencer et un message est affich�.

L'�cran de saisie permet de d�finir la p�riode � calculer, la liste des
identit�s � ex�cuter et les noms des fichiers o� les objets pourront �tre
recherch�s.

De plus, le calcul peut si on le souhaite ne s'op�rer que sur une
sous-p�riode.

.~a'pexec1.png'

Les touches fonctions d�finies dans cet �cran sont :

&EN ESCAPE : quitte sans ex�cuter
&EN F10 : lance l'ex�cution des identit�s sp�cifi�es

Si la liste des identit�s est laiss�e vide, toutes les identit�s en
m�moire sont ex�cut�es.

Si, parmi les s�ries ou les scalaires utilis�s dans les identit�s, un
ou plusieurs objets ne sont pas d�finis en WS, il faut pr�ciser la liste
des fichiers o� ces informations peuvent �tre recherch�es.

Lorsqu'une s�rie ou un scalaire utile � l'ex�cution d'une identit� ne se
trouve pas en m�moire, les fichiers indiqu�s sont analys�s � tour de
r�le. D�s que l'objet recherch� est trouv�, il est amen� en m�moire pour
la dur�e du calcul et est ensuite d�truit.

Si un objet - variable ou scalaire - n'est ni en WS, ni dans un des
fichiers indiqu�s, le processus de calcul s'arr�te.

L'utilisateur peut suivre la trace de la construction des identit�s en
en sp�cifiant Yes dans le champ "Debug" de la page de saisie. Les
informations utiles (par exemple, le fichier d'origine d'une variable
utilis�e pour construire l'identit�) seront imprim�s � la fin du
processus.

&TI Calcul sur une sous-p�riode
���������������������������
Lors de l'ex�cution d'une identit� sur une p�riode ne couvrant pas toute
celle du WS courant, les valeurs en dehors de la p�riode d'ex�cution
ne sont pas modifi�es.

Avant de commencer le calcul, les valeurs d'origine de la s�rie output
sont fix�es comme suit :

&EN Si la s�rie est en WS, les valeurs du WS sur la p�riode du WS, NA
    pour les autres p�riodes

&EN Si la s�rie n'est pas en WS, mais dans un des fichiers pr�cis�s
    dans la liste des fichiers input, les valeurs du premier fichier
    input qui la contient

&EN Si la s�rie n'est ni en WS, ni dans un des fichiers input, toutes
    ses valeurs sont fix�es � NA.

&TI Choix des fichiers de variables et scalaires
��������������������������������������������

Si aucun nom de fichier n'est sp�cifi�, les WS de variables et de
scalaires sont utilis�s pour les valeurs des objets n�cessaires �
l'ex�cution des identit�s.

Si au moins un nom de fichier est donn�, le WS n'est pas utilis�, sauf
en sp�cifiant comme nom "sp�cial" WS (en majuscule) dans la liste des
fichiers. Il convient de diff�rencier WS de ws : ws repr�sente en fait
le fichier disque "ws.var".

Ceci permet de n'utiliser les donn�es en m�moire (en WS) que si on le
souhaite r�ellement. De plus, la place de WS dans la liste des fichiers
est quelconque : d'autres fichiers peuvent �tre consult�s avant le WS
lui-m�me.
>

