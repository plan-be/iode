
<Les Fonctions de rapports>
    Les Fonctions de rapports
    컴컴컴컴컴컴컴컴컴컴�

Ces fonctions permettent de r괶liser toute un s굍ie d'op굍ations parmi
lesquelles

&EN le traitement de strings ou de listes de strings,
&EN le remplacement de texte,
&EN le comptage d'objets,
&EN le traitement de fichiers ASCII,
&EN l'interrogation de bases de donn괻s relationnelles,
@EN les informations sur les processus de calcul
&EN etc

Ces fonctions s'ex괹utent dans le cadre d'une ligne de rapport et le
r굎ultat de la fonction est imprim� dans l'output du rapport.

Par exemple :
&CO
    Le tableau T1 a pour titre @ttitle(T1). Ce titre en majuscules
    est par cons굌uent @upper(@ttitle(T1)). Ce texte est imprim�
    le @date() � @time().
&TX
    Pour placer le titre du tableau T1 dans le fichier toto.txt :
&CO
    @fappend(toto.txt, @ttitle(T1))
&TX

La syntaxe g굈굍ale de ces fonctions est :

&CO
    @function_name(arg1, arg2, ...)
&TX

Les fonctions suivantes sont d괽inies:

&IT Gestion de strings
컴컴컴컴컴컴컴컴컴컴컴
&EN <@upper>(texte) : mise d'un texte en majuscules
&EN <@lower>(texte) : mise d'un texte en minuscules
&EN <@replace>(string, from, to) : substitution d'un texte par une autre
&EN <@fmt>(val,fmt) : formatte un entier
&EN <@take>(n,string) : extrait les ~cn~C premiers caract둹es de ~cstring~C. Si ~cn~C est n괾atif, extrait les n derniers.
&EN <@drop>(n,string) : supprime les ~cn~C premiers caract둹es de ~cstring~C. Si ~cn~C est n괾atif, supprime les n derniers.
&EN <@count>(string) : retourne le nombre d'굃굆ents de string (s굋ar굎 par des virgules)
&EN <@index>(n,list) : retourne l'굃굆ent ~cn~C de ~clist~C
&EN <@sqz>(string)   : supprime les blancs de string
&EN <@strip>(string) : supprime les blancs de fin de string
&EN <@ansi>(texte) : transforme un texte Ascii en Ansi
&EN <@equal>(t1,t2) : compare t1 et t2 : retourne 1 si 괾aux, 0 sinon
&EN <@void>(t1,..) : ne retourne aucun texte, quels que soient les arguments


&IT Gestion de listes de strings
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
&EN <@vtake>(n,list) : take the n first elements of the list (or n last elements if n is negative)
&EN <@vdrop>(n,list) : drop the n first elements of the list (or n last elements if n is negative)
&EN <@vcount>(list)  : return the number of elements in the list

&IT Gestion de fichiers
컴컴컴컴컴컴컴컴컴컴컴�
&EN <@fdelete>(filename) : d굏ruit le fichier filename
&EN <@fappend>(filename,string|NL, ...) : 괹rit dans un fichier les textes


&IT Gestion de directories
컴컴컴컴컴컴컴컴컴컴컴�
&EN <@getdir>() : retourne le directory courant
&EN <@chdir>(dirname) : change le directory courant vers ~idirname~I et retourne le nouveau directory courant
&EN <@mkdir>(dirname) : cr괻 un nouveau directory ~idirname~I
&EN <@rmdir>(dirname) : d굏ruit le directory ~idirname~I

&IT Dates et heures
컴컴컴컴컴컴컴컴컴�
&EN <@date>([format]) : retourne la date
&EN <@time>([format]) : retourne l'heure
&EN <@month>(mois, langue) : retourne le texte du mois dans la langue donn괻
&EN <@ChronoReset>() : remet le chrono � 0
&EN <@ChronoGet>() : retourne le temps 괹oul� (en msecs) depuis le dernier
		    reset du chrono

&IT Liste d'objets
컴컴컴컴컴컴컴컴컴

&EN <@cexpand>(pattern, ...) : retourne la liste de commentaires correspondant � pattern
&EN <@eexpand>(pattern, ...) : retourne la liste d'굌uations correspondant � pattern
&EN <@iexpand>(pattern, ...) : retourne la liste d'identit굎 correspondant � pattern
&EN <@lexpand>(pattern, ...) : retourne la liste de listes correspondant � pattern
&EN <@sexpand>(pattern, ...) : retourne la liste de scalaires correspondant � pattern
&EN <@texpand>(pattern, ...) : retourne la liste de tableaux correspondant � pattern
&EN <@vexpand>(pattern, ...) : retourne la liste de variables correspondant � pattern

&EN <@vliste>(objname, ...) : retourne la liste des variables dans les eqs objname
&EN <@sliste>(objname, ...) : retourne la liste des scalaires dans les eqs objname

&IT Contenu des objets
컴컴컴컴컴컴컴컴컴컴컴
&EN <@ttitle>(tablename,tablename, ...) : retourne les titres des tableaux
&EN <@srelax>(sclname,sclname, ...) : retourne la valeur des relax pour des scalaires
&EN <@sstderr>(sclname,sclname, ...) : retourne la valeur des stderr pour des scalaires
&EN <@cvalue>(cmtname,cmtname, ...) : retourne le texte d'un commentaire
&EN <@vvalue>(varname,varname, ...) : retourne les valeurs de variables sous forme de texte
&EN <@sample>(B|E|) : retourne le texte du sample courant

&IT Contenu des 굌uations
컴컴컴컴컴컴컴컴컴컴컴
&EN <@evalue>(eqname,eqname, ...) : retourne le texte LEC d'une 굌uation
&EN <@eqsample>(eqname)     : retourne le sample d'estimation de l'굌uation ~ceqname~C
&EN <@eqsamplefrom>(eqname) : retourne la partie FROM du sample d'estimation
&EN <@eqsampleto>(eqname)   : retourne la partie TO du sample d'estimation
&EN <@eqlhs>(eqname)        : retourne le membre de gauche d'une 굌uations
&EN <@eqrhs>(eqname)        : retourne le membre de droite d'une 굌uations

&IT Interface avec les bases de donn괻s (ODBC)
컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

Les fonctions suivantes permettent d'interroger et/ou d'alimenter des
bases de donn괻s relationnelles dans un rapport IODE:

&EN <@SqlOpen> : ouverture de la base de donn괻s
&EN <@SqlQuery> : recherche/s굃ection/alimentation (SQL)
&EN <@SqlNext> : enregistrement suivant
&EN <@SqlField> : valeur d'un champ
&EN <@SqlRecord> : valeur d'un enregistrement complet
&EN <@SqlClose> : fermeture de la base de donn괻s

&IT Simulations
컴컴컴컴컴컴컴�

Les fonctions suivantes permettent d'obtenir la valeur de certains
param둻res de simulation :

&EN <@SimEps>()   : retourne la valeur du crit둹e de convergence utilis� pour
		  la derni둹e simulation
&EN <@SimRelax>() : retourne la valeur du param둻re de relaxation utilis� pour
		  la derni둹e simulation
&EN <@SimMaxit>() : retourne la valeur du maximum d'it굍ations utilis�
		  pour la derni둹e simulation


Les r굎ultats par p굍iode de simulation peuvent 늯re r괹up굍굎 via les fonctions suivantes :
&EN <@SimNiter>(period) : nombre d'it굍ations n괹essaires � la
		     r굎olution du mod둳e � l'ann괻 period
&EN <@SimNorm>(period)  : seuil de convergence atteint � la r굎olution
		     du mod둳e � l'ann괻 period
>

<@take>
    Fonction @take
    컴컴컴컴컴컴컴�

&DE
Extrait les ~cn~C premiers caract둹es de ~cstring~C. Si ~cn~C est n괾atif, extrait les n derniers.

&SY2
   @take(n,texte)
&TX

&EX2
   @take(3,IODE)
   @take(-3,IODE)
&TX
   r굎ultat
&CO
   IOD
   ODE
&TX
>
<@drop>
    Fonction @drop
    컴컴컴컴컴컴컴�

&DE
Supprime les ~cn~C premiers caract둹es de ~cstring~C. Si ~cn~C est n괾atif,
supprime les n derniers.


&SY2
   @drop(n,texte)
&TX

&EX2
   @drop(2,IODE)
   @drop(-2,IODE)
&TX
   r굎ultat
&CO
   DE
   IO
&TX
>
<@count>
    Fonction @count
    컴컴컴컴컴컴컴�

Retourne le nombre d'굃굆ents de string (s굋ar굎 par des virgules)
Supprime les ~cn~C premiers caract둹es de ~cstring~C. Si ~cn~C est n괾atif,
supprime les n derniers. &DE

&SY2
   @count(list)
&TX

&EX2
   @count(A,B,C,E)
   @count(ABC,,,B)
&TX
   r굎ultat
&CO
   4
   2
&TX
>
<@index>
    Fonction @index
    컴컴컴컴컴컴컴�

Retourne l'굃굆ent ~cn~C de ~clist~C.

&SY2
   @index(n,list)
&TX

&EX2
   @index(2,A,B,C,E)
   @index(1,ABC,,,B)
&TX
   r굎ultat
&CO
   B
   ABC
&TX
>
<@upper>
    Fonction @upper
    컴컴컴컴컴컴컴�

&DE
Cette fonction met en majuscules le texte pass� comme argument.

&SY2
   @upper("texte libre")
&TX

&EX2
   @upper("texte libre")
&TX
   r굎ultat
&CO
   TEXTE LIBRE
&TX
>
<@lower>
    Fonction @lower
    컴컴컴컴컴컴컴�

&DE
Cette fonction met en minuscules le texte pass� comme argument.

&SY2
   @lower("texte libre")
&TX

&EX2
   @lower("Texte Libre")
&TX
   r굎ultat
&CO
   texte libre
&TX
>
<@replace>
    Fonction @replace
    컴컴컴컴컴컴컴�

&DE
Cette fonction remplace un string par un autre. Le remplacement diff굍encie
majuscules et minuscules.

&SY2
   @replace("texte libre", replacefrom, replaceby)
&TX

&EX2
   @replace("texte libre",texte,cuba)
&TX
   r굎ultat
&CO
   cuba libre
&TX
>
<@vtake>
    Fonction @vtake(n,values)
    컴컴컴컴컴컴컴�

&DE
Ne conserve que les n premiers strings de ~cvalues.~C
Si ~cn~C est n괾atif, conserve les n derniers.

Les s굋arateurs entre les strings dans values sont d괽inis par la commande
~c$vseps~C. Par d괽ault, il s'agit de ",", ";" et blanc.

&NO La virgule est
toujours s굋arateurs, m늤e si elle n'est pas reprise dans $vseps.

Le r굎ultat est une liste dont le s굋arateur est le premier de ceux d괽inis par ~c$vseps~C.

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
    컴컴컴컴컴컴컴�

&DE
Voir <@vtake>
>

<@vcount>
    Fonction @vcount(n,values)
    컴컴컴컴컴컴컴�

&DE
Cette fonction retourne le nombre d'굃굆ents dans une liste de strings.


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
    컴컴컴컴컴컴컴�

&DE
Cette fonction d굏ruit un fichier. Elle ne retourne pas de r굎ultat.

&SY2
   @fdelete(filename)
&TX

&EX2
   @fdelete(test.htm)
&TX
   r굎ultat
&CO
   aucun - Le fichier test.htm est d굏ruit.
&TX
>
<@fappend>
    Fonction @fappend
    컴컴컴컴컴컴컴�

&DE
Cette fonction ajoute des strings au contenu d'un fichier ASCII. Elle ne retourne
pas de r굎ultat.

&SY2
   @fappend(filename,text|NL,text,...)
   o� NL indique un saut de ligne
&TX

&EX2
    Rapport test.rep
    ----------------
    @fappend(test.htm,"Fichier de donn괻s",NL)
    @fappend(test.htm,"Param둻res:",%*%)

    Appel
    -----
    test A B C
&TX
   r굎ultat
&CO
    Fichier test.htm
    ----------------
    Fichier de donn괻s
    Param둻res:A B C
&TX
>
<@date>
    Fonction @date
    컴컴컴컴컴컴컴�

&DE
Cette fonction retourne la date du jour.

&SY2
   @date([format])
   o� format indique le format de la date
   Par d괽aut, le format est dd-mm-yyyy
&TX

&EX2
    La date du jour est @date()
    Autre format : @date("dd/mm/yy")
&TX
   r굎ultat
&CO
    La date du jour est 31-05-1999
    Autre format : 31/05/99
&TX
>
<@time>
    Fonction @time
    컴컴컴컴컴컴컴�

&DE
Cette fonction retourne l'heure.

&SY2
   @time([format])
   o� format indique le format de l'heure
   Par d괽aut, le format est hh:mm:ss
&TX

&EX2
    Il est @time()
    Autre format : @time("hh heures mm minutes")
&TX
   r굎ultat
&CO
    Il est 23:12:55
    Autre format : 23 heures 12 minutes
&TX
>
<@month>
    Fonction @month
    컴컴컴컴컴컴컴�

&DE
Cette fonction retourne le nom d'un mois dans une langue donn괻.

&SY2
   @month(mois[,langue])
   o� langue est F, N ou E (E par d괽aut)
&TX

&EX2
    Le mois num굍o 3 est : @month(3)
    En fran놹is et majuscules : @upper(@month(3,F))
    En n괻rlandais et minuscules : @lower(@month(3,N))
&TX
   R굎ultat
&CO
    Le mois num굍o 3 est : March
    En fran놹is et majuscules : MARS
    En n괻rlandais et minuscules : maart
&TX
>
<@ansi>
    Fonction @ansi
    컴컴컴컴컴컴컴�

&DE
Cette fonction transforme un string cod� en Ascii en caract둹es cod굎 en Ansi.
Elle est par exemple utile lorsqu'on g굈굍e des fichiers output dont le format
doit 늯re imp굍ativement cod� en Ansi (fichiers HTML par exemple).

&SY2
   @ansi(texte)
&TX

&EX2
    <<TC>>@ansi(@month(2,F))<</TC>>
&TX
   R굎ultat
&CO
    <<TC>>F굒rier<</TC>>
&TX
>
<@cexpand>
    Fonction @cexpand
    컴컴컴컴컴컴컴�

&DE
Cette fonction retourne la liste des commentaires dont le nom correspond
� un des strings pass굎 comme argument.

&SY2
   @cexpand(pattern1, pattern2, ...)
&TX

&EX2
    @cexpand(A*,B*,AE)
&TX
   R굎ultat
&CO
    A1;A2;BA;BCXS;AE
&TX
La ligne
&CO
    $Repeat "$DataDuplicateVar _ _1" @vexpand(*)
&TX
ex괹ute la commande DataDuplicateVar sur toutes les variables du WS (les
nouvelles variables portent le nom de la variable d'origine avec 1
comme suffixe).

&SA @eexpand(), @iexpand(), @lexpand(), @sexpand(), @texpand(), @vexpand()
>
<@eexpand>
    Fonction @eexpand
    컴컴컴컴컴컴컴�

&DE
Cette fonction retourne la liste des 굌uations dont le nom correspond
� un des strings pass굎 comme argument.

&SY2
   @eexpand(pattern1, pattern2, ...)
&TX

&EX2
    @eexpand(X*)
&TX
   R굎ultat
&CO
    Toutes les 굌uations dont le nom commence par X
&TX
&SA @cexpand(), @iexpand(), @lexpand(), @sexpand(), @texpand(), @vexpand()
>
<@iexpand>
    Fonction @iexpand
    컴컴컴컴컴컴컴�

&DE
Cette fonction retourne la liste des identit굎 dont le nom correspond
� un des strings pass굎 comme argument.

&SY2
   @iexpand(pattern1, pattern2, ...)
&TX

&EX2
    @iexpand(*_1)
&TX
   R굎ultat
&CO
    Toutes les identit굎 dont le nom se termine par _1
&TX
&SA @cexpand(), @eexpand(), @lexpand(), @sexpand(), @texpand(), @vexpand()
>
<@lexpand>
    Fonction @lexpand
    컴컴컴컴컴컴컴�

&DE
Cette fonction retourne la liste des listes dont le nom correspond
� un des strings pass굎 comme argument.

&SY2
    @lexpand(pattern1, pattern2, ...)
&TX

&EX2
    $DataUpdateLst list1 @lexpand(_*)
&TX
Cr괻 une liste ~clist1~C contenant toutes les listes commen놹nt par _.
&SA @cexpand(), @eexpand(), @iexpand(), @sexpand(), @texpand(), @vexpand()
>
<@sexpand>
    Fonction @sexpand
    컴컴컴컴컴컴컴�

&DE
Cette fonction retourne la liste des scalaires dont le nom correspond
� un des strings pass굎 comme argument.

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
    컴컴컴컴컴컴컴�

&DE
Cette fonction retourne la liste des tableaux dont le nom correspond
� un des strings pass굎 comme argument.

&SY2
    @texpand(pattern1, pattern2, ...)
&TX

&EX2
    $PrintTbl 1990:5 @texpand(*)
&TX
Imprime tous les tableaux du WS sur le sample 1990 � 1994.

&SA @cexpand(), @eexpand(), @iexpand(), @lexpand(), @sexpand(), @vexpand()
>
<@vexpand>
    Fonction @vexpand
    컴컴컴컴컴컴컴�

&DE
Cette fonction retourne la liste des variables dont le nom correspond
� un des strings pass굎 comme argument.

&SY2
    @vexpand(pattern1, pattern2, ...)
&TX

&EX2
    $DataUpdateTbl Newtbl TITRE;@vexpand(BE*)
&TX
Cr괻 un tableau ~cNewtbl~C contenant toutes les s굍ies commen놹nt par BE.
&SA @cexpand(), @eexpand(), @iexpand(), @lexpand(), @sexpand(), @texpand()
>
<@vliste>
    Fonction @vliste
    컴컴컴컴컴컴컴컴
&DE
Cette fonction retourne la liste des variables utilis괻s dans les 굌uations
dont les noms sont pass굎 comme argument.

&SY2
    @vliste(eq1,eq2,...)
&TX

&EX2
    $DataUpdateLst VarLst @vliste(ACAF,ACAG)
&TX
Cr괻 la liste ~cVarLst~C contenant toutes les s굍ies utilis괻s dans les 굌uations ACAF et ACAG.
&SA @sliste()

>
<@sliste>
    Fonction @sliste
    컴컴컴컴컴컴컴컴
&DE
Cette fonction retourne la liste des scalaires utilis굎 dans les 굌uations
dont les noms sont pass굎 comme argument.

&SY2
    @sliste(eq1,eq2,...)
&TX

&EX2
    $DataUpdateLst SCLLST @sliste(@eexpand(*))
&TX
Cr괻 la liste ~cSCLLST~C contenant tous les scalaires utilis굎 dans les
굌uations du WS.

&SA @vliste()
>
<@ttitle>
    Fonction @ttitle
    컴컴컴컴컴컴컴컴
&DE
Cette fonction retourne les titres des tableaux pass굎 comme arguments.

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
    컴컴컴컴컴컴컴컴
&DE
Cette fonction retourne les valeurs des param둻res de relaxation des
scalaires pass굎 comme arguments.

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
    컴컴컴컴컴컴컴컴
&DE
Cette fonction retourne les valeurs des stderr des scalaires pass굎 comme
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
    컴컴컴컴컴컴컴컴
&DE
Cette fonction retourne le texte des commentaires dont les noms sont pass굎
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
    컴컴컴컴컴컴컴컴
&DE
Cette fonction retourne la formule LEC d괽inissant une 굌uation.

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
    컴컴컴컴컴컴컴컴
&DE
Cette fonction retourne les valeurs formatt괻s des variables dont les noms sont pass굎
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
    컴컴컴컴컴컴컴컴
&DE
Cette fonction retourne le sample courant.

&SY2
    @sample(B|E|)
	B : premi둹e p굍iode
	E : derni둹e p굍iode
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
<Interpr굏ation des lignes de rapports>
    Interpr굏ation des lignes de rapports
    컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�

Une ligne de rapport est interpr굏괻 avant d'늯re (굒entuellement) ex괹ut괻.
Cette interpr굏ation se fait de la fa뇇n suivante:

L'ordre d'interpr굏ation est le    suivant :

Les caract둹es sp괹iaux sont : ~b%, { et @.~B

Chaque ligne est interpr굏괻 de gauche � droite. D둺 qu'un des caract둹es
sp괹iaux est rencontr�, un traitement particulier est appliqu�.

Si on rencontre % :
&EN si le suivant est %, un seul % est conserv� comme du texte
&CO
    Exemple : augmentation de 10%% du PNB ->> un seul % reste dans le texte
&TX
&EN sinon, la macro est remplac괻 par sa valeur ou vide si la macro n'existe pas :
&CO
	    Exemple : la variable %VAR% ->> la variable XYZ
&TX

Si on rencontre { :
&EN si le suivant est =, le contenu entre accollades est consid굍� comme une
    r괽굍ence Excel et remplac�

&EN si le suivant est {, un seul { est conserv� comme du texte le texte est
    lu jusqu'� }, les macros sont remplac괻s

&EN2 si le texte r굎ultat commence par $ ou ##, il s'agit d'une commande
	  de rapport qui est ex괹ut괻 et le r굎ultat (0 ou 1) se retrouve
	  dans le texte.
&EN2 sinon, le texte r굎ultat est calcul� comme une formule LEC � la
	  p굍iode courante d괽inie par $SetTime. Si la formule se termine
	  pas @T ou @999.999, le r굎ultat est format� en cons굌uence.

Si on rencontre @ :

	si le suivant est @, un seul @ est conserv� comme du texte
	le texte est lu jusqu'� la parenth둺e fermante
	la fonction correspondante est ex괹ut괻. A noter qu'en
	l'absence de parenth둺es, le texte reste inchang� (Ex.: gb@plan.be
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
    컴컴컴�

    Formatte un entier ~cval~C suivant un format ~cfmt~C donn�. Le r굎ultat est un
    string transform� de longueur 괾ale � celle de ~cfmt.~C

    Les caract둹es reconnus dans le format sont : 'X', 'x', '9', '0'. Ils
    signifient qu'aux seules positions de ces caract둹es seront plac굎 dans
    leur ordre d'apparition les caract둹es r굎ultant du formattage de ~cval.~C
    Seul cas particulier : le caract둹e ~c'0'~C qui sera remplac� par un ~c'0'~C si
    le caract둹e correspondant dans le formattage de ~cval~C est ' '.

&SY
    @fmt(val,fmt)
    o�
    val = valeur enti둹e
    fmt = format
&TX

&EX
    @fmt(123,0009)   --->> 0123
    @fmt(123,A0000A)   --->> A00123A
&TX
    Attention, les blancs avant et apr둺 le format sont repris dans le r굎ultat.
>

<@sqz>
    Fonction @sqz
    컴컴컴�
    Supprime les blancs d'un string.
&SY
    @sqz(string)
    o�
    string = string quelconque
&TX

&EX
    @sqz(' ABC D  ')   --->> 'ABCD'
&TX
>

<@strip>
    Fonction @strip
    컴컴컴�
    Supprime les blancs de fin d'un string.
&SY
    @strip(string)
    o�
    string = string quelconque
&TX

&EX
    @strip(' ABC D  ')   --->> ' ABC D'
&TX
>

<@equal>
    Fonction @equal
    컴컴컴�
    V굍ifie si deux valeurs sont identiques.
&SY
    equal(a,b)
    o�
    a et b = textes quelconques
&TX

&EX
    @equal(123,123)   --->> 1
    @equal(123, 123)   --->> 0
    @equal(%i%,10)   --->> 1 (ou 0 selon la valeur de i)
&TX
Attention : les espaces sont compt굎 comme des caract둹es.
>


<@SimEps>
    Fonction @SimEps
    컴컴컴컴컴컴컴컴
Retourne la valeur du crit둹e de convergence utilis� pour la derni둹e
simulation.

&IT Exemple
컴컴컴컴컴�
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
    컴컴컴컴컴컴컴컴컴컴
Retourne la valeur du param둻re de relaxation utilis� pour la derni둹e
simulation.

Voir exemple dans la fonction <@SimEps>.
>

<@SimMaxit>
    Fonction  @SimMaxit()
    컴컴컴컴컴컴컴컴컴컴�

Retourne la valeur du maximum d'it굍ations utilis� pour la derni둹e
simulation.

Voir exemple dans la fonction <@SimEps>.
>

<@SimNiter>
Fonction @SimNiter(period)
컴컴컴컴컴컴컴컴컴컴컴컴컴

Retourne le nombre d'it굍ations n괹essaires � la r굎olution du mod둳e � l'ann괻 ~cperiod.~C

Voir exemple dans la fonction <@SimEps>.
>

<@SimNorm>
Fonction @SimNorm(period)
컴컴컴컴컴컴컴컴컴컴컴컴�

Retourne le seuil de convergence atteint � la r굎olution du mod둳e � l'ann괻 ~cperiod.~C

Voir exemple dans la fonction <@SimEps>.
>


<@ChronoReset>
    Fonction @ChronoReset()
    컴컴컴컴컴컴컴컴컴컴컴�

Un chrono virtuel a 굏� ajout� pour permettre de calculer les dur괻s de traitement.

@ChronoReset() remet le chrono � 0.

Voir exemple dans la fonction <@SimEps>.
>
<@ChronoGet>
Fonction @ChronoGet()
컴컴컴컴컴컴컴컴컴컴�

Retourne le temps 괹oul� (en msecs) depuis le dernier appel � @ChronoReset.

Voir exemple dans la fonction <@SimEps>.
>


<@eqsample>
   Fonction @eqsample(eqname)
   컴컴컴컴컴컴컴컴컴

Retourne le sample d'estimation de l'굌uation ~ceqname~C
>
<@eqsamplefrom>
    Fonction @eqsamplefrom(eqname)
    컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

Retourne la partie FROM du sample d'estimation.

Dans l'exemple suivant, on extrait la premi둹e ann괻 d'estimation de l'굌uation ENDO1
pour r괻stimer l'굌uation sur un sample prolong� � droite par exemple suite � l'obtention .
de nouvelles observations.

&CO
    $EqsEstimate @eqsamplefrom(ENDO1) 2012Y1 ENDO1
&TX

>
<@eqsampleto>
    Fonction @eqsampleto(eqname)
    컴컴컴컴컴컴컴컴컴컴컴컴컴컴

Retourne la partie TO du sample d'estimation.

Voir exemple dans <@EqSampleFrom>.
>
<@eqlhs>
    Fonction @eqlhs(eqname)
    컴컴컴컴컴컴컴컴컴컴컴�

Retourne le membre de gauche d'une 굌uation.
>
<@eqrhs>
    Fonction @eqrhs(eqname)
    컴컴컴컴컴컴컴컴컴컴컴�

Retourne le membre de droite d'une 굌uation.
>



<@void>
    Fonction @void(args)
    컴컴컴컴컴컴컴컴컴컴

Vide le texte de ses arguments, quels que soient les arguments.

Par exemple, l'appel � @chdir() retourne le nom du nouveau directory courant.
Avec @void() ce texte n'apparait pas dans l'output.

&CO
    Voici de r굎ultat de chdir()  : @chdir(..)
    Voici de r굎ultat avec void() : @void(@chdir(..))
&TX
R굎ultat :
&CO
    Voici de r굎ultat de chdir()  : \usr\iode
    Voici de r굎ultat avec void() :
&TX

>



<@getdir>
    Fonction @getdir()
    컴컴컴컴컴컴컴컴컴

Retourne le directory courant.

&EX
    Directory courant : @getdir()
&TX
R굎ultat :
&CO
    Directory courant : c:\usr\iode
&TX
>

<@chdir>
    Fonction @chdir(dirname)
    컴컴컴컴컴컴컴컴컴컴컴컴

Change le directory courant vers ~idirname~I et retourne le nouveau directory courant.

&EX
    Dir courant : @getdir()
    Nouveau dir : @chdir(..)
&TX
R굎ultat :
&CO
    Dir courant : c:\usr\iode
    Nouveau dir : c:\usr
&TX

>

<@mkdir>
    Fonction @mkdir(dirname)
    컴컴컴컴컴컴컴컴컴컴컴컴

Cr괻 un nouveau directory de nom ~idirname.~I

Ne retourne aucune valeur.
&EX
    Dir courant :         @getdir()
    Cr괶tion de subdir    @mkdir(subdir)
    Nouveau dir courant : @getdir()
&TX
R굎ultat :
&CO
    Dir courant :         c:\usr\iode
    Cr괶tion de subdir
    Nouveau dir courant : c:\usr\iode\subdir
&TX
>
<@rmdir>
    Fonction @rmdir(dirname)
    컴컴컴컴컴컴컴컴컴컴컴컴

D굏ruit le directory ~idirname~I. Le directory doit 늯re vide.

Ne retourne aucune valeur.
>













