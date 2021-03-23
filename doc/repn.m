
<Les Fonctions de rapports>
    Les Fonctions de rapports
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Ces fonctions permettent de r‚aliser toute un s‚rie d'op‚rations parmi
lesquelles

&EN le traitement de strings ou de listes de strings,
&EN le remplacement de texte,
&EN le comptage d'objets,
&EN le traitement de fichiers ASCII,
&EN l'interrogation de bases de donn‚es relationnelles,
@EN les informations sur les processus de calcul
&EN etc

Ces fonctions s'ex‚cutent dans le cadre d'une ligne de rapport et le
r‚sultat de la fonction est imprim‚ dans l'output du rapport.

Par exemple :
&CO
    Le tableau T1 a pour titre @ttitle(T1). Ce titre en majuscules
    est par cons‚quent @upper(@ttitle(T1)). Ce texte est imprim‚
    le @date() … @time().
&TX
    Pour placer le titre du tableau T1 dans le fichier toto.txt :
&CO
    @fappend(toto.txt, @ttitle(T1))
&TX

La syntaxe g‚n‚rale de ces fonctions est :

&CO
    @function_name(arg1, arg2, ...)
&TX

Les fonctions suivantes sont d‚finies:

&IT Gestion de strings
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&EN <@upper>(texte) : mise d'un texte en majuscules
&EN <@lower>(texte) : mise d'un texte en minuscules
&EN <@replace>(string, from, to) : substitution d'un texte par une autre
&EN <@fmt>(val,fmt) : formatte un entier
&EN <@take>(n,string) : extrait les ~cn~C premiers caractŠres de ~cstring~C. Si ~cn~C est n‚gatif, extrait les n derniers.
&EN <@drop>(n,string) : supprime les ~cn~C premiers caractŠres de ~cstring~C. Si ~cn~C est n‚gatif, supprime les n derniers.
&EN <@count>(string) : retourne le nombre d'‚l‚ments de string (s‚par‚s par des virgules)
&EN <@index>(n,list) : retourne l'‚l‚ment ~cn~C de ~clist~C
&EN <@sqz>(string)   : supprime les blancs de string
&EN <@strip>(string) : supprime les blancs de fin de string
&EN <@ansi>(texte) : transforme un texte Ascii en Ansi
&EN <@equal>(t1,t2) : compare t1 et t2 : retourne 1 si ‚gaux, 0 sinon
&EN <@void>(t1,..) : ne retourne aucun texte, quels que soient les arguments


&IT Gestion de listes de strings
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&EN <@vtake>(n,list) : take the n first elements of the list (or n last elements if n is negative)
&EN <@vdrop>(n,list) : drop the n first elements of the list (or n last elements if n is negative)
&EN <@vcount>(list)  : return the number of elements in the list

&IT Gestion de fichiers
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&EN <@fdelete>(filename) : d‚truit le fichier filename
&EN <@fappend>(filename,string|NL, ...) : ‚crit dans un fichier les textes


&IT Gestion de directories
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&EN <@getdir>() : retourne le directory courant
&EN <@chdir>(dirname) : change le directory courant vers ~idirname~I et retourne le nouveau directory courant
&EN <@mkdir>(dirname) : cr‚e un nouveau directory ~idirname~I
&EN <@rmdir>(dirname) : d‚truit le directory ~idirname~I

&IT Dates et heures
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&EN <@date>([format]) : retourne la date
&EN <@time>([format]) : retourne l'heure
&EN <@month>(mois, langue) : retourne le texte du mois dans la langue donn‚e
&EN <@ChronoReset>() : remet le chrono … 0
&EN <@ChronoGet>() : retourne le temps ‚coul‚ (en msecs) depuis le dernier
		    reset du chrono

&IT Liste d'objets
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&EN <@cexpand>(pattern, ...) : retourne la liste de commentaires correspondant … pattern
&EN <@eexpand>(pattern, ...) : retourne la liste d'‚quations correspondant … pattern
&EN <@iexpand>(pattern, ...) : retourne la liste d'identit‚s correspondant … pattern
&EN <@lexpand>(pattern, ...) : retourne la liste de listes correspondant … pattern
&EN <@sexpand>(pattern, ...) : retourne la liste de scalaires correspondant … pattern
&EN <@texpand>(pattern, ...) : retourne la liste de tableaux correspondant … pattern
&EN <@vexpand>(pattern, ...) : retourne la liste de variables correspondant … pattern

&EN <@vliste>(objname, ...) : retourne la liste des variables dans les eqs objname
&EN <@sliste>(objname, ...) : retourne la liste des scalaires dans les eqs objname

&IT Contenu des objets
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&EN <@ttitle>(tablename,tablename, ...) : retourne les titres des tableaux
&EN <@srelax>(sclname,sclname, ...) : retourne la valeur des relax pour des scalaires
&EN <@sstderr>(sclname,sclname, ...) : retourne la valeur des stderr pour des scalaires
&EN <@cvalue>(cmtname,cmtname, ...) : retourne le texte d'un commentaire
&EN <@vvalue>(varname,varname, ...) : retourne les valeurs de variables sous forme de texte
&EN <@sample>(B|E|) : retourne le texte du sample courant

&IT Contenu des ‚quations
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&EN <@evalue>(eqname,eqname, ...) : retourne le texte LEC d'une ‚quation
&EN <@eqsample>(eqname)     : retourne le sample d'estimation de l'‚quation ~ceqname~C
&EN <@eqsamplefrom>(eqname) : retourne la partie FROM du sample d'estimation
&EN <@eqsampleto>(eqname)   : retourne la partie TO du sample d'estimation
&EN <@eqlhs>(eqname)        : retourne le membre de gauche d'une ‚quations
&EN <@eqrhs>(eqname)        : retourne le membre de droite d'une ‚quations

&IT Interface avec les bases de donn‚es (ODBC)
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les fonctions suivantes permettent d'interroger et/ou d'alimenter des
bases de donn‚es relationnelles dans un rapport IODE:

&EN <@SqlOpen> : ouverture de la base de donn‚es
&EN <@SqlQuery> : recherche/s‚lection/alimentation (SQL)
&EN <@SqlNext> : enregistrement suivant
&EN <@SqlField> : valeur d'un champ
&EN <@SqlRecord> : valeur d'un enregistrement complet
&EN <@SqlClose> : fermeture de la base de donn‚es

&IT Simulations
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Les fonctions suivantes permettent d'obtenir la valeur de certains
paramŠtres de simulation :

&EN <@SimEps>()   : retourne la valeur du critŠre de convergence utilis‚ pour
		  la derniŠre simulation
&EN <@SimRelax>() : retourne la valeur du paramŠtre de relaxation utilis‚ pour
		  la derniŠre simulation
&EN <@SimMaxit>() : retourne la valeur du maximum d'it‚rations utilis‚
		  pour la derniŠre simulation


Les r‚sultats par p‚riode de simulation peuvent ˆtre r‚cup‚r‚s via les fonctions suivantes :
&EN <@SimNiter>(period) : nombre d'it‚rations n‚cessaires … la
		     r‚solution du modŠle … l'ann‚e period
&EN <@SimNorm>(period)  : seuil de convergence atteint … la r‚solution
		     du modŠle … l'ann‚e period
>

<@take>
    Fonction @take
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Extrait les ~cn~C premiers caractŠres de ~cstring~C. Si ~cn~C est n‚gatif, extrait les n derniers.

&SY2
   @take(n,texte)
&TX

&EX2
   @take(3,IODE)
   @take(-3,IODE)
&TX
   r‚sultat
&CO
   IOD
   ODE
&TX
>
<@drop>
    Fonction @drop
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Supprime les ~cn~C premiers caractŠres de ~cstring~C. Si ~cn~C est n‚gatif,
supprime les n derniers.


&SY2
   @drop(n,texte)
&TX

&EX2
   @drop(2,IODE)
   @drop(-2,IODE)
&TX
   r‚sultat
&CO
   DE
   IO
&TX
>
<@count>
    Fonction @count
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Retourne le nombre d'‚l‚ments de string (s‚par‚s par des virgules)
Supprime les ~cn~C premiers caractŠres de ~cstring~C. Si ~cn~C est n‚gatif,
supprime les n derniers. &DE

&SY2
   @count(list)
&TX

&EX2
   @count(A,B,C,E)
   @count(ABC,,,B)
&TX
   r‚sultat
&CO
   4
   2
&TX
>
<@index>
    Fonction @index
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Retourne l'‚l‚ment ~cn~C de ~clist~C.

&SY2
   @index(n,list)
&TX

&EX2
   @index(2,A,B,C,E)
   @index(1,ABC,,,B)
&TX
   r‚sultat
&CO
   B
   ABC
&TX
>
<@upper>
    Fonction @upper
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction met en majuscules le texte pass‚ comme argument.

&SY2
   @upper("texte libre")
&TX

&EX2
   @upper("texte libre")
&TX
   r‚sultat
&CO
   TEXTE LIBRE
&TX
>
<@lower>
    Fonction @lower
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction met en minuscules le texte pass‚ comme argument.

&SY2
   @lower("texte libre")
&TX

&EX2
   @lower("Texte Libre")
&TX
   r‚sultat
&CO
   texte libre
&TX
>
<@replace>
    Fonction @replace
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction remplace un string par un autre. Le remplacement diff‚rencie
majuscules et minuscules.

&SY2
   @replace("texte libre", replacefrom, replaceby)
&TX

&EX2
   @replace("texte libre",texte,cuba)
&TX
   r‚sultat
&CO
   cuba libre
&TX
>
<@vtake>
    Fonction @vtake(n,values)
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Ne conserve que les n premiers strings de ~cvalues.~C
Si ~cn~C est n‚gatif, conserve les n derniers.

Les s‚parateurs entre les strings dans values sont d‚finis par la commande
~c$vseps~C. Par d‚fault, il s'agit de ",", ";" et blanc.

&NO La virgule est
toujours s‚parateurs, mˆme si elle n'est pas reprise dans $vseps.

Le r‚sultat est une liste dont le s‚parateur est le premier de ceux d‚finis par ~c$vseps~C.

&SY2
   @vtake(n,values)
&TX

&EX2
$show @vtake(1,A,B,C)   ->> A
$show @vtake(-1,A,B,C)  ->> C
$show @vdrop(-1,A,B,C)  ->> A,B
$show @vdrop(2,A,B,C)   ->> C
$vseps |
$show @vtake(1,A B|C)   ->> A B
$show @vtake(1,"A,B"|C) ->> A,B
$show @vdrop(-1,A,B,C)  ->> A|B
&TX

&EX2
    $vseps ;
    $define LIST A;B;C;D
    $label next
    $define ELEMENT @vtake(1, %LIST%)
      ... Some operation on element %ELEMENT% ...
    $define LIST @vdrop(1,%LIST%)
    $goto next @vcount(%LIST%)
&TX
>

<@vdrop>
    Fonction @vdrop(n,values)
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Voir <@vtake>
>

<@vcount>
    Fonction @vcount(n,values)
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction retourne le nombre d'‚l‚ments dans une liste de strings.


&SY2
    @vcount({liste de strings}}
&TX

&EX2
    $vseps |
    $show @vcount(A B|C)   ->> 2
    $show @vcount(A;B;C)   ->> 1
&TX

Voir aussi <@vtake>.
>

<@fdelete>
    Fonction @fdelete
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction d‚truit un fichier. Elle ne retourne pas de r‚sultat.

&SY2
   @fdelete(filename)
&TX

&EX2
   @fdelete(test.htm)
&TX
   r‚sultat
&CO
   aucun - Le fichier test.htm est d‚truit.
&TX
>
<@fappend>
    Fonction @fappend
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction ajoute des strings au contenu d'un fichier ASCII. Elle ne retourne
pas de r‚sultat.

&SY2
   @fappend(filename,text|NL,text,...)
   o— NL indique un saut de ligne
&TX

&EX2
    Rapport test.rep
    ----------------
    @fappend(test.htm,"Fichier de donn‚es",NL)
    @fappend(test.htm,"ParamŠtres:",%*%)

    Appel
    -----
    test A B C
&TX
   r‚sultat
&CO
    Fichier test.htm
    ----------------
    Fichier de donn‚es
    ParamŠtres:A B C
&TX
>
<@date>
    Fonction @date
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction retourne la date du jour.

&SY2
   @date([format])
   o— format indique le format de la date
   Par d‚faut, le format est dd-mm-yyyy
&TX

&EX2
    La date du jour est @date()
    Autre format : @date("dd/mm/yy")
&TX
   r‚sultat
&CO
    La date du jour est 31-05-1999
    Autre format : 31/05/99
&TX
>
<@time>
    Fonction @time
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction retourne l'heure.

&SY2
   @time([format])
   o— format indique le format de l'heure
   Par d‚faut, le format est hh:mm:ss
&TX

&EX2
    Il est @time()
    Autre format : @time("hh heures mm minutes")
&TX
   r‚sultat
&CO
    Il est 23:12:55
    Autre format : 23 heures 12 minutes
&TX
>
<@month>
    Fonction @month
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction retourne le nom d'un mois dans une langue donn‚e.

&SY2
   @month(mois[,langue])
   o— langue est F, N ou E (E par d‚faut)
&TX

&EX2
    Le mois num‚ro 3 est : @month(3)
    En fran‡ais et majuscules : @upper(@month(3,F))
    En n‚erlandais et minuscules : @lower(@month(3,N))
&TX
   R‚sultat
&CO
    Le mois num‚ro 3 est : March
    En fran‡ais et majuscules : MARS
    En n‚erlandais et minuscules : maart
&TX
>
<@ansi>
    Fonction @ansi
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction transforme un string cod‚ en Ascii en caractŠres cod‚s en Ansi.
Elle est par exemple utile lorsqu'on g‚n‚re des fichiers output dont le format
doit ˆtre imp‚rativement cod‚ en Ansi (fichiers HTML par exemple).

&SY2
   @ansi(texte)
&TX

&EX2
    <<TC>>@ansi(@month(2,F))<</TC>>
&TX
   R‚sultat
&CO
    <<TC>>F‚vrier<</TC>>
&TX
>
<@cexpand>
    Fonction @cexpand
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction retourne la liste des commentaires dont le nom correspond
… un des strings pass‚s comme argument.

&SY2
   @cexpand(pattern1, pattern2, ...)
&TX

&EX2
    @cexpand(A*,B*,AE)
&TX
   R‚sultat
&CO
    A1;A2;BA;BCXS;AE
&TX
La ligne
&CO
    $Repeat "$DataDuplicateVar _ _1" @vexpand(*)
&TX
ex‚cute la commande DataDuplicateVar sur toutes les variables du WS (les
nouvelles variables portent le nom de la variable d'origine avec 1
comme suffixe).

&SA @eexpand(), @iexpand(), @lexpand(), @sexpand(), @texpand(), @vexpand()
>
<@eexpand>
    Fonction @eexpand
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction retourne la liste des ‚quations dont le nom correspond
… un des strings pass‚s comme argument.

&SY2
   @eexpand(pattern1, pattern2, ...)
&TX

&EX2
    @eexpand(X*)
&TX
   R‚sultat
&CO
    Toutes les ‚quations dont le nom commence par X
&TX
&SA @cexpand(), @iexpand(), @lexpand(), @sexpand(), @texpand(), @vexpand()
>
<@iexpand>
    Fonction @iexpand
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction retourne la liste des identit‚s dont le nom correspond
… un des strings pass‚s comme argument.

&SY2
   @iexpand(pattern1, pattern2, ...)
&TX

&EX2
    @iexpand(*_1)
&TX
   R‚sultat
&CO
    Toutes les identit‚s dont le nom se termine par _1
&TX
&SA @cexpand(), @eexpand(), @lexpand(), @sexpand(), @texpand(), @vexpand()
>
<@lexpand>
    Fonction @lexpand
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction retourne la liste des listes dont le nom correspond
… un des strings pass‚s comme argument.

&SY2
    @lexpand(pattern1, pattern2, ...)
&TX

&EX2
    $DataUpdateLst list1 @lexpand(_*)
&TX
Cr‚e une liste ~clist1~C contenant toutes les listes commen‡ant par _.
&SA @cexpand(), @eexpand(), @iexpand(), @sexpand(), @texpand(), @vexpand()
>
<@sexpand>
    Fonction @sexpand
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction retourne la liste des scalaires dont le nom correspond
… un des strings pass‚s comme argument.

&SY2
    @sexpand(pattern1, pattern2, ...)
&TX

&EX2
    @sexpand(*)
&TX
retourne la liste de tous les scalaires du WS.
&SA @cexpand(), @eexpand(), @iexpand(), @lexpand(), @texpand(), @vexpand()
>
<@texpand>
    Fonction @texpand
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction retourne la liste des tableaux dont le nom correspond
… un des strings pass‚s comme argument.

&SY2
    @texpand(pattern1, pattern2, ...)
&TX

&EX2
    $PrintTbl 1990:5 @texpand(*)
&TX
Imprime tous les tableaux du WS sur le sample 1990 … 1994.

&SA @cexpand(), @eexpand(), @iexpand(), @lexpand(), @sexpand(), @vexpand()
>
<@vexpand>
    Fonction @vexpand
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&DE
Cette fonction retourne la liste des variables dont le nom correspond
… un des strings pass‚s comme argument.

&SY2
    @vexpand(pattern1, pattern2, ...)
&TX

&EX2
    $DataUpdateTbl Newtbl TITRE;@vexpand(BE*)
&TX
Cr‚e un tableau ~cNewtbl~C contenant toutes les s‚ries commen‡ant par BE.
&SA @cexpand(), @eexpand(), @iexpand(), @lexpand(), @sexpand(), @texpand()
>
<@vliste>
    Fonction @vliste
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction retourne la liste des variables utilis‚es dans les ‚quations
dont les noms sont pass‚s comme argument.

&SY2
    @vliste(eq1,eq2,...)
&TX

&EX2
    $DataUpdateLst VarLst @vliste(@eexpand(*))
&TX
Cr‚e la liste ~cVarLst~C contenant toutes les s‚ries utilis‚es dans les ‚quations du WS.
&SA @sliste()
>
<@sliste>
    Fonction @sliste
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction retourne la liste des scalaires utilis‚s dans les ‚quations
dont les noms sont pass‚s comme argument.

&SY2
    @sliste(eq1,eq2,...)
&TX

&EX2
    $DataUpdateLst SCLLST @sliste(@eexpand(*))
&TX
Cr‚e la liste ~cSCLLST~C contenant tous les scalaires utilis‚s dans les
‚quations du WS.

&SA @vliste()
>
<@ttitle>
    Fonction @ttitle
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction retourne les titres des tableaux pass‚s comme arguments.

&SY2
    @ttitle(tbl1,tbl2,...)
&TX

&EX2
    @ttitle(tbl)
&TX
    retourne par exemple
&CO
    Titre du tableau
&TX
>

<@srelax>
    Fonction @srelax
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction retourne les valeurs des paramŠtres de relaxation des
scalaires pass‚s comme arguments.

&SY2
    @srelax(scl1,scl2,...)
&TX

&EX2
    @srelax(scl)
&TX
    retourne par exemple
&CO
    0.9
&TX
>

<@sstderr>
    Fonction @sstderr
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction retourne les valeurs des stderr des scalaires pass‚s comme
arguments.

&SY2
    @sstderr(scl1,scl2,...)
&TX

&EX2
    @sstderr(scl1)
&TX
    retourne par exemple
&CO
    0.001
&TX
>

<@cvalue>
    Fonction @cvalue
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction retourne le texte des commentaires dont les noms sont pass‚s
comme arguments.

&SY2
    @cvalue(cmtname,cmtname, ...)
&TX

&EX2
    @cvalue(CMT1,A)
&TX
    retourne par exemple
&CO
      Commentaire 1;Commentaire de A
&TX

>
<@evalue>
    Fonction @evalue
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction retourne la formule LEC d‚finissant une ‚quation.

&SY2
    @evalue(eqname,eqname, ...)
&TX

&EX2
    @evalue(EQ1)
&TX
    retourne par exemple
&CO
      log(EQ1) := c1 + c2 * V3 + c3 * X
&TX

>
<@vvalue>
    Fonction @vvalue
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction retourne les valeurs formatt‚es des variables dont les noms sont pass‚s
comme arguments.

&SY2
    @vvalue(varname,varname, ...)
&TX

&EX2
    @vvalue(A,B)
&TX
    retourne par exemple
&CO
      na na 1.2342 -1.22323 1000 1001 1002 1003
&TX
>
<@sample>
    Fonction @sample
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction retourne le sample courant.

&SY2
    @sample(B|E|)
	B : premiŠre p‚riode
	E : derniŠre p‚riode
	nil : les deux
&TX

&EX2
    @sample()
    @sample(B)
    @sample(E)
&TX
retourne par exemple :
&CO
      1960Y1 1990Y1
      1960Y1
      1990Y1
&TX
>
<Interpr‚tation des lignes de rapports>
    Interpr‚tation des lignes de rapports
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Une ligne de rapport est interpr‚t‚e avant d'ˆtre (‚ventuellement) ex‚cut‚e.
Cette interpr‚tation se fait de la fa‡on suivante:

L'ordre d'interpr‚tation est le    suivant :

Les caractŠres sp‚ciaux sont : ~b%, { et @.~B

Chaque ligne est interpr‚t‚e de gauche … droite. DŠs qu'un des caractŠres
sp‚ciaux est rencontr‚, un traitement particulier est appliqu‚.

Si on rencontre % :
&EN si le suivant est %, un seul % est conserv‚ comme du texte
&CO
    Exemple : augmentation de 10%% du PNB ->> un seul % reste dans le texte
&TX
&EN sinon, la macro est remplac‚e par sa valeur ou vide si la macro n'existe pas :
&CO
	    Exemple : la variable %VAR% ->> la variable XYZ
&TX

Si on rencontre { :
&EN si le suivant est =, le contenu entre accollades est consid‚r‚ comme une
    r‚f‚rence Excel et remplac‚

&EN si le suivant est {, un seul { est conserv‚ comme du texte le texte est
    lu jusqu'… }, les macros sont remplac‚es

&EN2 si le texte r‚sultat commence par $ ou ##, il s'agit d'une commande
	  de rapport qui est ex‚cut‚e et le r‚sultat (0 ou 1) se retrouve
	  dans le texte.
&EN2 sinon, le texte r‚sultat est calcul‚ comme une formule LEC … la
	  p‚riode courante d‚finie par $SetTime. Si la formule se termine
	  pas @T ou @999.999, le r‚sultat est format‚ en cons‚quence.

Si on rencontre @ :

	si le suivant est @, un seul @ est conserv‚ comme du texte
	le texte est lu jusqu'… la parenthŠse fermante
	la fonction correspondante est ex‚cut‚e. A noter qu'en
	l'absence de parenthŠses, le texte reste inchang‚ (Ex.: gb@plan.be
	reste tel quel).

&CO
	$define VAL 123.123
	$msg {%VAL%@999.9} gb@plan.be
&TX
	Donne :
&CO
	123.1 gb@plan.be
&TX
>

<@fmt>
    Fonction @fmt
    ÄÄÄÄÄÄÄ

    Formatte un entier ~cval~C suivant un format ~cfmt~C donn‚. Le r‚sultat est un
    string transform‚ de longueur ‚gale … celle de ~cfmt.~C

    Les caractŠres reconnus dans le format sont : 'X', 'x', '9', '0'. Ils
    signifient qu'aux seules positions de ces caractŠres seront plac‚s dans
    leur ordre d'apparition les caractŠres r‚sultant du formattage de ~cval.~C
    Seul cas particulier : le caractŠre ~c'0'~C qui sera remplac‚ par un ~c'0'~C si
    le caractŠre correspondant dans le formattage de ~cval~C est ' '.

&SY
    @fmt(val,fmt)
    o—
    val = valeur entiŠre
    fmt = format
&TX

&EX
    @fmt(123,0009)   --->> 0123
    @fmt(123,A0000A)   --->> A00123A
&TX
    Attention, les blancs avant et aprŠs le format sont repris dans le r‚sultat.
>

<@sqz>
    Fonction @sqz
    ÄÄÄÄÄÄÄ
    Supprime les blancs d'un string.
&SY
    @sqz(string)
    o—
    string = string quelconque
&TX

&EX
    @sqz(' ABC D  ')   --->> 'ABCD'
&TX
>

<@strip>
    Fonction @strip
    ÄÄÄÄÄÄÄ
    Supprime les blancs de fin d'un string.
&SY
    @strip(string)
    o—
    string = string quelconque
&TX

&EX
    @strip(' ABC D  ')   --->> ' ABC D'
&TX
>

<@equal>
    Fonction @equal
    ÄÄÄÄÄÄÄ
    V‚rifie si deux valeurs sont identiques.
&SY
    equal(a,b)
    o—
    a et b = textes quelconques
&TX

&EX
    @equal(123,123)   --->> 1
    @equal(123, 123)   --->> 0
    @equal(%i%,10)   --->> 1 (ou 0 selon la valeur de i)
&TX
Attention : les espaces sont compt‚s comme des caractŠres.
>


<@SimEps>
    Fonction @SimEps
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Retourne la valeur du critŠre de convergence utilis‚ pour la derniŠre
simulation.

&IT Exemple
ÄÄÄÄÄÄÄÄÄÄÄ
L'exemple ci-dessous compare les performances selon le nombre de tri de
l'algorithme de simulation.

&CO

  $ Parameters
  $ ----------
  $define modeldir C:\iode\nemesis
  $define model base_neujobs_rd
  $define nbtri 5
  $define simfrom 2011Y1
  $define simto 2012Y1
  $define simper %simfrom% %simto%
  $ModelSimulateParms 0.00001 0.6 500 Both 0 No 1

  $ Output file
  $ -----------
  $PrintDest compare.html HTML

  $ Load fresh files
  $ ----------------
  $WsLoadVar %modeldir%\%model%
  $WsLoadScl %modeldir%\%model%
  $WsLoadEqs %modeldir%\%model%

  $ ==== $ModelCalcSCC ===
  .par1 tit_1
  Comparaison entre les performances selon le nombre de tri

  $ ==== Loop on Tri tests ===
  $define i 0
  $label nexttri

  $ Calcul SCC
  $ ----------
  $show $ModelCalcSCC %i%
  @ChronoReset()
  $ModelCalcSCC %i% pre inter post
  $define cpu1  @ChronoGet()

  $ Simulate
  $ --------
  $ Reload vars for a clean start and modify exo
  $show Reload Vars ...
  $WsLoadVar %modeldir%\%model%
  $DataUpdateVar RPOIL 2011Y1 1.20

  $show $ModelSimulateSCC  %simper% pre inter post
  @ChronoReset()
  $ModelSimulateSCC %simper% pre inter post
  $define cpu2 @ChronoGet()

  $ Reporting
  $ ---------
  .par1 enum_1
  Tris : %i%
  .par1 enum_2
  Calc SCC : %cpu1% msec
  .par1 enum_2
  Simulation (eps = @simeps(); maxit=@simmaxit()) : %cpu2% msec

  $define j {%simfrom%}
  $define totit 0

  $label nextsimper
  .par1 enum_3
  {%j%@T} : conv = @simnorm({%j%@T}), niter = @simniter({%j%@T})
  $define simit @simniter({%j%@T})
  $define totit {%totit% + %simit%}
  $define j {%j% + 1}
  $goto nextsimper {%j% <<= %simto%}
  .par1 enum_3
  Total iterations : %totit%
  $define i {%i% + 1}
  $goto nexttri {%i% <<= %nbtri%}
&TX

>

<@SimRelax>
    Fonction @SimRelax()
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Retourne la valeur du paramŠtre de relaxation utilis‚ pour la derniŠre
simulation.

Voir exemple dans la fonction <@SimEps>.
>

<@SimMaxit>
    Fonction  @SimMaxit()
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Retourne la valeur du maximum d'it‚rations utilis‚ pour la derniŠre
simulation.

Voir exemple dans la fonction <@SimEps>.
>

<@SimNiter>
Fonction @SimNiter(period)
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Retourne le nombre d'it‚rations n‚cessaires … la r‚solution du modŠle … l'ann‚e ~cperiod.~C

Voir exemple dans la fonction <@SimEps>.
>

<@SimNorm>
Fonction @SimNorm(period)
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Retourne le seuil de convergence atteint … la r‚solution du modŠle … l'ann‚e ~cperiod.~C

Voir exemple dans la fonction <@SimEps>.
>


<@ChronoReset>
    Fonction @ChronoReset()
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Un chrono virtuel a ‚t‚ ajout‚ pour permettre de calculer les dur‚es de traitement.

@ChronoReset() remet le chrono … 0.

Voir exemple dans la fonction <@SimEps>.
>
<@ChronoGet>
Fonction @ChronoGet()
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Retourne le temps ‚coul‚ (en msecs) depuis le dernier appel … @ChronoReset.

Voir exemple dans la fonction <@SimEps>.
>


<@eqsample>
   Fonction @eqsample(eqname)
   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Retourne le sample d'estimation de l'‚quation ~ceqname~C
>
<@eqsamplefrom>
    Fonction @eqsamplefrom(eqname)
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Retourne la partie FROM du sample d'estimation.

Dans l'exemple suivant, on extrait la premiŠre ann‚e d'estimation de l'‚quation ENDO1
pour r‚estimer l'‚quation sur un sample prolong‚ … droite par exemple suite … l'obtention .
de nouvelles observations.

&CO
    $EqsEstimate @eqsamplefrom(ENDO1) 2012Y1 ENDO1
&TX

>
<@eqsampleto>
    Fonction @eqsampleto(eqname)
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Retourne la partie TO du sample d'estimation.

Voir exemple dans <@EqSampleFrom>.
>
<@eqlhs>
    Fonction @eqlhs(eqname)
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Retourne le membre de gauche d'une ‚quation.
>
<@eqrhs>
    Fonction @eqrhs(eqname)
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Retourne le membre de droite d'une ‚quation.
>



<@void>
    Fonction @void(args)
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Vide le texte de ses arguments, quels que soient les arguments.

Par exemple, l'appel … @chdir() retourne le nom du nouveau directory courant.
Avec @void() ce texte n'apparait pas dans l'output.

&CO
    Voici de r‚sultat de chdir()  : @chdir(..)
    Voici de r‚sultat avec void() : @void(@chdir(..))
&TX
R‚sultat :
&CO
    Voici de r‚sultat de chdir()  : \usr\iode
    Voici de r‚sultat avec void() :
&TX

>



<@getdir>
    Fonction @getdir()
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Retourne le directory courant.

&EX
    Directory courant : @getdir()
&TX
R‚sultat :
&CO
    Directory courant : c:\usr\iode
&TX
>

<@chdir>
    Fonction @chdir(dirname)
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Change le directory courant vers ~idirname~I et retourne le nouveau directory courant.

&EX
    Dir courant : @getdir()
    Nouveau dir : @chdir(..)
&TX
R‚sultat :
&CO
    Dir courant : c:\usr\iode
    Nouveau dir : c:\usr
&TX

>

<@mkdir>
    Fonction @mkdir(dirname)
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cr‚e un nouveau directory de nom ~idirname.~I

Ne retourne aucune valeur.
&EX
    Dir courant :         @getdir()
    Cr‚ation de subdir    @mkdir(subdir)
    Nouveau dir courant : @getdir()
&TX
R‚sultat :
&CO
    Dir courant :         c:\usr\iode
    Cr‚ation de subdir
    Nouveau dir courant : c:\usr\iode\subdir
&TX
>
<@rmdir>
    Fonction @rmdir(dirname)
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

D‚truit le directory ~idirname~I. Le directory doit ˆtre vide.

Ne retourne aucune valeur.
>













