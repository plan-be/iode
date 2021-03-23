/* WS */

<Workspace>
   Workspace
   ÄÄÄÄÄÄÄÄÄ


.~a'mws.png'

La notion de workspace (WS) occupe une place centrale dans le
d‚roulement d'une session IODE. Rappelons que le logiciel opŠre sur les
objets contenus dans les WS courants, c'est-…-dire charg‚s en m‚moire.

Il y a un WS courant par type d'objet. Le menu WORKSPACE offre toutes
les fonctions permettant de charger … partir du disque et de sauver sur
disque les fichiers associ‚s aux workspaces. Il permet de traiter les WS
dans leur ensemble. Le menu DATA permettra quant … lui de travailler au
niveau des objets individuels contenus dans les workspaces.

Un fichier de workspace sur disque porte un nom dont l'extension indique
le type d'objet qu'il contient.

On distingue sept types d'objets dans IODE et donc sept extensions
diff‚rentes aux noms de fichiers associ‚s :

&EN les commentaires : .cmt
&EN les ‚quations    : .eqs
&EN les identit‚s    : .idt
&EN les listes       : .lst
&EN les scalaires    : .scl
&EN les tableaux     : .tbl
&EN les variables    : .var

En plus de ces extensions qui correspondent aux fichiers d'objets en
format interne, on a des extensions associ‚es aux fichiers en format
Ascii :

&EN les commentaires : .ac
&EN les ‚quations    : .ae
&EN les identit‚s    : .ai
&EN les listes       : .al
&EN les scalaires    : .as
&EN les tableaux     : .at
&EN les variables    : .av

Les diff‚rentes options du menu WORKSPACE sont :

&EN <Load Work Space       > : charge des WS depuis le disque
&EN <Save Work Space       > : sauve les WS courants sur disque
&EN <Clear Work Space      > : vide les WS courants

&EN <Copy into Work Space  > : copie les donn‚es s‚lectionn‚es des WS sur
			       disque vers les WS courants
&EN <Merge into Work Space > : fusionne les donn‚es des WS sur
			       disque avec les WS courants
&EN <Describe Work Space   > : change les descriptions des WS courants
&EN <Set Variables Sample  > : change la p‚riode de d‚finition des variables
&EN <Extrapolate Variables > : prolonge les s‚ries par extrapolation
&EN <High to Low           > : modifie la p‚riodicit‚ de s‚ries
&EN <Low to High           > : modifie la p‚riodicit‚ de s‚ries
&EN <Seasonal Adjustment   > : Census XI - method for Seasonal Adjustment
&EN <Trend Correction      > : Filtre de Hodrick-Prescott
>

<Load Work Space>
   Load Workspace
   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

.~a'pwsload.png'

L'op‚ration de chargement (LOAD) d'un workspace remplace le contenu du
WS courant par le contenu d'un fichier disque : tous les objets
actuellement pr‚sents en WS sont d‚truits et remplac‚s par ceux d‚finis
dans le fichier … charger. Dans le cas des variables, le SAMPLE du
fichier charg‚ remplace le SAMPLE courant.

Cette fonction ouvre un ‚cran dans lequel on peut entrer un nom de
fichier par type d'objet. Au d‚part, les noms sont ceux des WS courants.
Tous les champs sont des champs DIR : la touche ENTER permet donc de
s‚lectionner le fichier d‚sir‚ au travers d'un menu reprenant tous les
fichiers correspondants au masque introduit dans le champ.

Les touches F2 … F8 permettent de charger un seul WS, selon la touche
press‚e : F2 pour comments, F3 pour equations, etc. La touche F10 permet
de charger tous les WS dont les noms sont sp‚cifi‚s.

Des boutons correspondant … ces touches sont ‚galement propos‚s.

La fonction n'essayera de charger que les fichiers de WS dont le nom
n'est pas vide. Il suffit donc de laisser blanc les noms correspondant
aux types de WS qu'on ne veut pas modifier et de presser F10 pour
charger un nombre limit‚ de WS.

&NO Si un fichier n'est pas trouv‚, la fonction ne tente pas de charger
les WS suivants. Le contenu du WS correspondant au type de fichier n'est
pas d‚truit.

&TI Extensions des noms de fichiers
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Le programme de chargement de WS est capable non seulement de charger
des fichiers en format interne, mais ‚galement des fichiers en format Ascii.

Il se base pour faire le choix entre les deux formats sur l'extension du
fichier. Si cette extension est une des extensions standardis‚es
(~cmyws.cmt~C) ou s'il n'y a pas d'extension au nom du fichier
(~cmyws~C), le chargement d'un fichier en format interne est effectu‚.
Si l'extension est autre (~cmyws.aaa~C par exemple), la fonction de
chargement d'un fichier en format Ascii est utilis‚e.
>

<Copy into Work Space  >
   Copy into Workspace
   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

.~a'pwscopy.png'

Il est possible d'ajouter ou de remplacer des objets en m‚moire … partir
de WS sauv‚s sur disque. La fonction COPY demande le type d'objet …
copier, le nom du fichier contenant le WS o— se trouvent les objets …
copier et enfin la liste des noms d'objets.

Dans le cas des variables, la p‚riode … copier peut ‚galement ˆtre
sp‚cifi‚e.

Une fois ces informations introduites, les objets indiqu‚s sont
copi‚s dans le WS courant et remplacent les valeurs actuelles. Le
processus s'arrˆte dŠs qu'un objet ne peut ˆtre trouv‚.

Le WS courant garde intacts tous les autres objets.

&IT Un cas particulier : les variables
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Dans le cas des variables, un problŠme suppl‚mentaire se pose : le
SAMPLE de d‚finition du WS courant peut ˆtre diff‚rent de celui du
fichier. Dans ce cas, les variables copi‚es s'adaptent au SAMPLE du WS
courant, soit en perdant des valeurs, soit en ajoutant des valeurs NA
pour compl‚ter les s‚ries.

Si une p‚riode est sp‚cifi‚e dans l'‚cran de saisie, seules les
observations de cette p‚riode sont copi‚es en WS.

&IT Utilisation de listes
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Un nom de liste pr‚c‚d‚ d'un dollar peut ˆtre introduit dans la zone
correspondant aux noms d'objets :

&CO
    $LISTE
&TX

La liste est alors remplac‚e par sa valeur et tous les noms d‚finis dans
la liste sont copi‚s. Une liste peut elle-mˆme ˆtre d‚finie par d'autres
listes : celles-ci sont r‚cursivement remplac‚es par leur valeur.
>

<Merge into Work Space  >
    Merge into Workspace
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

.~a'pwsmerge.png'

Cette fonction permet d'ajouter toutes les donn‚es d'un workspace sur
fichier au workspace actif (en m‚moire). Dans le cas o— un objet existe
d‚j… dans le workspace actif, il est remplac‚ par la valeur du fichier.

Dans le cas de variables, la p‚riode sur laquelle les s‚ries sont
copi‚es est celle d‚finie par le SAMPLE du workspace actif. Si le
workspace actif de variables n'a pas encore de SAMPLE, le SAMPLE du
fichier est utilis‚. Dans ce dernier cas, cette fonction est identique …
un Load.
>

<Describe Work Space>
   Describe Workspace
   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

.~a"pwsdescr.png"

Cette fonction permet de modifier la description associ‚e … chaque
workspace pr‚sent en m‚moire.

A ce niveau, l'effet n'est que local … la session : la modification de
la description d'un WS n'a aucune influence sur le contenu des fichiers
de WS sur disque. Par contre, au prochain sauvetage des workspaces
(Save), les descriptions seront celles introduites lors du dernier appel
… la fonction Describe.

>

<Save Work Space>
   Save Workspace
   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Cette fonction r‚alise l'op‚ration inverse de la fonction LOAD : elle
copie sur disque les WS dans leur ‚tat actuel, en rempla‡ant
‚ventuellement les fichiers existants. Accessoirement, elle permet de
connaŒtre le contenu actuel des diff‚rents WS.

.~a"pwssave.png"

La fenˆtre pr‚sente pour chaque type de WS :

&EN le nom du fichier associ‚ au WS. Par d‚faut, ce nom est ws.type o—
    type indique le type d'objet. Ce nom peut ‚videmment ˆtre modifi‚.
    Si le WS a ‚t‚ charg‚ … partir du disque, le nom du WS est le nom du
    fichier d'origine.

&EN le nombre d'objets actuellement d‚finis dans le WS

Un nom vide ne donnera pas lieu … un sauvetage. Les WS courants (en
m‚moire) ne sont pas modifi‚s par cette fonction.

Les touches F2 … F8 permettent de sauver un seul WS, selon la touche
press‚e : F2 pour comments, F3 pour equations, etc. La touche F10 permet
de sauver tous les WS dont les noms sont sp‚cifi‚s.

Des boutons correspondant … ces touches sont ‚galement propos‚s.

La fonction n'essayera de sauver que les WS dont le nom n'est pas vide.
Il suffit donc de laisser blanc les noms correspondant aux types de WS
qu'on ne veut pas sauver et de presser F10 pour sauver un nombre limit‚
de WS.

&NO La fonction s'arrˆte dŠs qu'un WS ne peut ˆtre sauv‚ (nom
    incorrect, manque de place sur disque, problŠme de m‚moire, etc).
    Les WS suivants ne sont donc pas sauv‚s dans ce cas.

&TI Backup
ÄÄÄÄÄÄÄÄÄÄ
Avant de sauver un WS, une copie de l'ancienne version du fichier (s'il
en existe une) est prise par s‚curit‚. Ce fichier porte le mˆme nom que
l'ancien, avec la mˆme extension sauf la derniŠre lettre qui est remplac‚e
par un $. Ainsi le fichier ~cmytest.cmt~C
est copi‚ sous le nom    ~cmytest.cm$~C

Pour restaurer un fichier, il suffit de copier le backup sous un nom
accept‚ par IODE.

>
<Clear Work Space>
   Clear Workspace
   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

.~a'pwsclear.png'

Lorsqu'on souhaite d‚truire tous les objets contenus dans un WS courant
(en m‚moire), au moins deux m‚thodes globales sont disponibles : charger
un nouveau fichier de WS ou vider … l'aide de la fonction CLEAR le
contenu du WS.

L'‚cran pr‚sente pour chaque type de WS le nombre d'objets actuellement
d‚finis en m‚moire.

Les touches F2 … F8 permettent de vider un seul WS, selon la touche
press‚e : F2 pour comments, F3 pour equations, etc. La touche F10 permet
de vider tous les WS.

Des boutons correspondant … ces touches sont ‚galement propos‚s.

&IT Cas particulier : les variables
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Lorsqu'un WS de variables est "vid‚", la p‚riode de d‚finition des
s‚ries n'est plus d‚finie pour permettre de changer la p‚riodicit‚ du WS.
>

<Set Variables Sample>
   Set Variables Sample
   ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

.~a'pwssample.png'

Le WS de variables est d‚fini sur un "SAMPLE", indiquant les p‚riodes
extrˆmes sur lesquelles les s‚ries peuvent ˆtre d‚finies. Toutes les
s‚ries d'un workspace sont d‚finies sur la mˆme p‚riode.

Il peut se produire que ce sample doive ˆtre modifi‚, par exemple pour
ajouter des observations ou encore pour limiter l'espace requis par le
stockage du WS.

Les p‚riodes ajout‚es prennent comme valeur NA (not available). Les
valeurs correspondantes aux p‚riodes supprim‚es sont d‚truites.

La p‚riodicit‚ ne peut ˆtre modifi‚e, sauf si le sample initial ‚tait
non d‚fini (nul). Pour changer de p‚riodicit‚, il faut utiliser les fonctions
<High to Low> ou <Low To High>.

>


<Extrapolate Variables >
    Extrapolate Variables
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

.~a'pwextrap.png'

Cette fonction modifie les valeurs de s‚ries sur une p‚riode choisie par
extrapolation.

Les premiers champs permettent de sp‚cifier la p‚riode … modifier.

Le deuxiŠme champ offre le choix entre diff‚rentes m‚thodes (identiques
… celles disponibles pour les valeurs de d‚part des simulations) :

&EN Y := Y[-1], if Y null or NA : chaque valeur nulle ou NA
    prend la valeur de la p‚riode pr‚c‚dente

&EN Y := Y[-1], always : chaque valeur prend la valeur de la
    p‚riode pr‚c‚dente

&EN Y := extrapolation, if Y null or NA : chaque valeur nulle ou NA
    prend comme valeur une extrapolation lin‚aire des deux p‚riodes
    pr‚c‚dentes

&EN Y := extrapolation, always : chaque valeur prend comme valeur une
    extrapolation lin‚aire des deux p‚riodes pr‚c‚dentes, qu'elle soit
    nulle ou non au d‚part

&EN Y unchanged : les valeurs ne sont pas modifi‚es.

&EN Y := Y[-1], if Y = NA : chaque valeur NA prend la valeur de la
    p‚riode pr‚c‚dente

&EN Y := extrapolation, if Y = NA : chaque valeur NA
    prend comme valeur une extrapolation lin‚aire des deux p‚riodes
    pr‚c‚dentes


Le dernier champ contient la liste des noms de s‚ries … extrapoler.

La fonction de rapport ‚quivalente est :
&CO
   $WsExtrapolate [method] from to [liste de variables]
&TX
>

<Low to High>
    Low to High
    ÄÄÄÄÄÄÄÄÄÄÄ
.~a'pwsltoh.png'

Construit des s‚ries de p‚riodicit‚ sup‚rieure.

Pour ce faire, la fonction charge en WS la liste de s‚ries du fichier
sp‚cifi‚ et modifie simultan‚ment la p‚riodicit‚ de ces s‚ries. La
nouvelle p‚riodicit‚ est celle actuellement d‚finie dans le WS actif.

Les nouvelles s‚ries sont ajout‚es ou remplacent (pour des noms
existants) celles du WS actif.

Cette proc‚dure existe pour les cas suivants :

&EN annuel vers mensuel
&EN annuel vers trimestriel
&EN trimestriel vers mensuel

Deux m‚thodes sont disponibles, l'une pour les stock, l'autre pour les
flux : dans le cas des stocks, toutes les valeurs assign‚es pour les
sous-p‚riodes sont celles de la p‚riode input.

		A[1980Q{1,2,3,4}] = A[1980Y1]

Dans le cas des flux, la s‚rie est r‚partie sur les sous-p‚riodes :

		A[1980Q1] = 0.25 * A[1980Y1]

AprŠs avoir sp‚cifi‚ le nom du fichier input, deux listes doivent ˆtre
fournies. La premiŠre liste est celles des s‚ries de type "stock", la
deuxiŠme de type "flux".

Enfin, il est possible de sp‚cifier la m‚thode d'interpolation … utiliser
pour calculer les valeurs interm‚diaires :

&EN interpolation lin‚aire
&EN interpolation par "cubic splines"
&EN interpolation par escalier
>

<High to Low>
    High to Low
    ÄÄÄÄÄÄÄÄÄÄÄ
.~a'pwshtol.png'

Construit des s‚ries de p‚riodicit‚ inf‚rieure … partir de s‚ries
stock‚es dans un fichier.

Pour ce faire, la fonction charge en WS la liste de s‚ries du fichier
sp‚cifi‚ et modifie simultan‚ment la p‚riodicit‚ de ces s‚ries. La
nouvelle p‚riodicit‚ est celle actuellement d‚finie dans le WS actif.

Les nouvelles s‚ries sont ajout‚es ou remplacent (pour des noms
existants) celles du WS actif.

Cette proc‚dure existe pour les cas suivants :

&EN mensuel vers annuel (observation annuelle = somme des 12 mois)

&EN trimestriel vers annuel (observation annuelle = somme des 4
    trimestres)

&EN mensuel vers trimestriel (observation trimestrielle = somme des 3
    mois)

AprŠs avoir sp‚cifi‚ le nom du fichier input, trois listes doivent ˆtre
fournies. La premiŠre liste est celles des s‚ries pour lesquelles le
r‚sultat est la somme des sous-p‚riodes, la deuxiŠme la moyenne et la
troisiŠme, la derniŠre observation de la s‚rie input.

En cas de valeur inexistante (NA) pour l'une des sous-p‚riodes, le
r‚sultat est NA.
>

<Seasonal Adjustment>
    Seasonal Adjustment
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

.~a'pwsseas.png'

Elimine les variations saisonniŠres des s‚ries mensuelles. La m‚thode utilis‚e
est celle de Census X11. La m‚thode ne fonctionne que pour les parties
qui sont entiŠrement connues.

En plus de la s‚rie dessaisonnalis‚e (qui porte le nom de la s‚rie dans
le fichier input), on obtient comme r‚sultat deux autres s‚ries :

&EN _Cseries (o— series est le nom de la s‚rie d'origine) qui contient
    la s‚rie de trends cycliques
&EN _Iseries qui contient la composante stochastique

Une fonction de rapport effectue la mˆme op‚ration :
&CO
    $WsSeasonAdj
&TX
>

<Trend Correction>
    Trend Correction
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

.~a'pwstrend.png'

Impl‚mentation de la m‚thode Hodrick-Prescott pour la construction de
s‚ries de trend. Le principe est le mˆme que pour la dessaisonnalisation:
les s‚ries lues dans un fichier sont import‚es et transform‚es
simultan‚ment en conservant leur nom.

Une note technique [Bart Hertveld] d‚crit la m‚thode et son
utilisation. Voir aussi <Hodrick-Prescott Filter>.

Les fonctions ‚quivalentes de rapport ont la syntaxe suivante :

&CO
	$WsTrend    VarFilename Lambda series1 series2 ...
	$WsTrendStd VarFilename Lambda series1 series2 ...
	##WsTrend (interactive)
&TX


La fonction ~c$WsTrend~C transforme les donn‚es en logarithme avant le calcul et
repasse … l'exponentielle aprŠs le calcul. Il est donc n‚cessaire d'avoir
une s‚rie strictement positive pour que la m‚thode produise un r‚sultat.

La fonction ~c$WsTrendStd~C travaille sur les donn‚es telles quelles et n'a donc pas
cette contrainte.


>


<Hodrick-Prescott Filter>
    Hodrick-Prescott Filter
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

&TI Abstract

Technische beschrijving van de procedure voor het bepalen van
een niet-lineaire trend in tijdreeksen.

&TI 1. Inleiding
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Met ingang van de versie 4.52 kan de IODE-gebruiker een beroep doen op de
Hodrick-Prescott filter (in het vervolg: HP-filter). Deze filter ontbindt
een seizoengezuiverde tijdreeks in een trendcomponent en de complementaire
cyclische component. Voor conjunctuuranalyse is een dergelijke decompositie
van de trendcyclische component onontbeerlijk. Enerzijds werd de
incorporatie van deze tijdreekstechniek in IODE dus ingegeven door de
toenemende belangstelling binnen het Planbureau voor het domein van de korte
termijn studies (zie o.a. het opstellen van het Economisch Budget).
Anderzijds biedt de analyse van de trendbewegingen ruime
toepassingsmogelijkheden voor studies die zich meer op lange termijn
richten.

De HP-filter wordt tevens aangewend door het DG II van de Europese
Commissie voor de berekening van de cyclische aanpassing van
begrotingstekorten.

&TI 2. Opdeling van tijdreeksen
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
Klassiek in de tijdreeksanalyse is de opdeling van een basisreeks yt in vier
componenten:
&EN  de trendcomponent (gt);
&EN  de cyclische component (ct);
&EN  de seizoenscomponent (st);
&EN  de volstrekt onvoorspelbare component of toevalscomponent (ut).

Per definitie is de seizoenscomponent onbestaande voor reeksen met een
jaarfrequentie (stroomgrootheden over een periode van 12 maanden,
voorraadgrootheden op een identiek tijdstip per jaar, groeivoet tegenover
zelfde maand van het vorige jaar, groeivoet laatste twaalf maand tegenover
vorige twaalf maand, cumulatieve groeivoet tegenover zelfde periode vorige
jaar,...).

Deze decompositie kan additief of multiplicatief geformuleerd worden

~a'hp0.png'

Deze opdeling van tijdreeksen kan via IODE als volgt bereikt worden.

&EN Stap 1: via de Census X-II methode (beschikbaar in IODE sedert de
versie 4.38) wordt de basisreeks omgezet in een seizoengezuiverde
reeks. Het verschil (additief of multiplicatief gedefinieerd) tussen de
basisreeks en de seizoengezuiverde reeks is de seizoenscomponent st.

&EN Stap 2: toepassing van de HP-filter op de seizoengezuiverde reeks levert
de trendwaarde gt.

&EN  Stap 3: het residu (additief of multiplicatief gedefinieerd) uit stap 2 bevat
in principe nog de cyclische en de toevalscomponent. Eventueel kan de
toevalscomponent worden afgesplitst door afvlakking van de residureeks
(b.v. via voortschrijdende gemiddelden).

&TI 3. De Hodrick-Prescott filter
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
&IT FORMULERING
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

De Hodrick-Prescott trend gt van een seizoengezuiverde reeks yt wordt
gedefinieerd als de oplossing van onderstaand minimaliseringsprobleem:

~a'hp1.png'

&EN T:het aantal gegevens in de tijdreeks;
&EN yt:de logaritme van de basisreeks, dus gegeven;
&EN gt:de logaritme van de trendwaarde, de onbekende waarnaar de uitdrukking
geminimaliseerd wordt.

Door ~ieps~I te bepalen leggen we een voorwaarde op aan de groei van gt. Kiezen
we ~ieps~I = 0, dan kiezen we voor een constante groei van gt en komt de
minimalisering neer op de schatting van een log-lineaire trend.

De oplossing van het probleem is afhankelijk van de waarde van ~ieps.~I Voor gelijk
welke e vindt men een oplossing als men de volgende vorm minimaliseert.

~a'hp2.png'

&EN ~ilambda~I : multiplicator van Lagrange, in deze procedure vrij te kiezen constante
positieve waarde.

Hoe groter ~ilambda~I hoe gladder de resulterende gt. Laten we ~ilambda~I naar oneindig gaan,
dan krijgen we een lineaire trend.

&IT UITWERKING

Berekenen we Min (gt, (2)) dan moeten we (2) T-keer partieel afleiden naar gt.
Zo bekomen we een stelsel van T-vergelijkingen en T onbekenden.


~a'hp3.png'

~a'hp4.png'

De matrix is een symmetrische T x T matrix en heeft als elementen de
gewichten die toelaten de originele basisreeks terug te rekenen uit de
trendreeks (elke rij van de matrix sommeert tot 1, wat de term gewichten
wettigt). De trendreeks wordt uiteindelijk berekend door de inverse van de
matrix te vermenigvuldigen met de vector van de basisreeks.

&IT FILOSOFIE

Bij de raming van een trendbeweging stelt zich steeds een trade-off tussen de
twee volgende eisen:
&EN de trend mag niet te ver afwijken van de originele reeks;
&EN de trend moet een vloeiend (ïsmoothğ) verloop hebben, m.a.w, de
trendmatige groeivoet moet over de tijd heen vrij stabiel1 zijn.

De eerste eis wordt geformaliseerd in de eerste term van [2]: minimalisering
van de som van de gekwadrateerde verschillen in logaritme tussen de
originele waarden en de trendwaarden. Dit betekent concreet dat de
verhouding2 tussen de originele waarde en de trendwaarde op geen enkel
tijdstip ver van de eenheid mag afwijken.

De tweede eis wordt geformaliseerd in de tweede term van [2]: minimalisering
van het verschil in opeenvolgende groeivoeten3 van de trendreeks. Dit
betekent concreet dat de trendmatige groeivoet geen bokkesprongen mag
vertonen en dat de trend dus een vloeiend verloop kent.

Dat tussen beide eisen een trade-off bestaat is duidelijk: hoe beter de
originele reeks benaderd wordt (… la limite: trendwaarde = originele waarde),
hoe minder vloeiend de trendreeks, en, hoe stabieler de trendgroei (… la limite:
constante groeivoet), hoe groter de afwijking met de originele reeks.

Het belang dat de gebruiker in de hierboven geschetste trade-off wil
toekennen aan de eerste, resp. de tweede eis, kan worden vertaald in de
waarde van de parameter l. Hoe groter (kleiner) men l kiest, hoe sterker
(minder sterk) de tweede eis, relatief tegenover de eerste, zal meespelen in
de bepaling van de trendwaarden. De limietwaarden voor l maken een en
ander duidelijk. Voor  vervalt de eerste eis en bekomt men een lineaire
trend (een constante groeivoet is immers de limietwaarde van een stabiele
groeivoet). Voor l = 0 is de cyclische component gelijk aan nul (immers, de
trendwaarde valt in dat geval samen met de originele waarde). In de literatuur
wordt l  = 1600 voor kwartaalreeksen, en l  = 100 voor jaarreeksen
aanbevolen. Deze waarden voor ~ilambda~I werden standaard voorzien in
IODE.1 Wegens het arbitraire karakter van deze keuze is het aangewezen
geval per geval na te gaan in hoeverre de keuze van ~ilambda~I de decompositie
van de trendcyclische component be‹nvloedt.

&IT KANTTEKENINGEN

~uKeuze van lambda~U

Een eerste kanttekening bij de HP-filter betreft de parameter ~ilambda.~I De
keuze van ~ilambda~I is arbitrair en kan dus de ontbinding van de trendcyclische
component be‹nvloeden. Meer bepaald kunnen, binnen het raam van de
conjunctuuranalyse, alternatieve waarden voor ~ilambda~I aanleiding geven tot
tegenstrijdige bevindingen omtrent de datering van de cycli.

Deze kritiek op de HP-filter kan echter genuanceerd worden:

&EN "de" methode voor decompositie van de trendcyclische component
bestaat niet; de onderzoeker wordt dus berhaupt geconfronteerd met
een keuzeprobleem (nl. de keuze van de gehanteerde methode);
&EN de kritiek op de methode kan deels worden genuanceerd door de
stabiliteit van de besluiten te testen voor verschillende (normaal geachte)
waarden van ~ilambda;~I dit pleit dus tegen een blind gebruik van deze
techniek;
&EN de vrije keuze van ~ilambda~I kan worden aangewend als instrument, met
name bij het relateren van kwantitatieve aan kwalitatieve indicatoren:
hierbij kan via de keuze van ~ilambda~I op zoek worden gegaan naar de
systematische verdeling van de totale trendcyclische beweging over
trend en cyclus (onder de veronderstelling dat kwalitatieve indicatoren
uitsluitend deze laatste capteren).

~uStabiliteit aan de uiteinden van de reeks~U
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Uit de constructie van de HP-filter (cf. supra) blijkt dat de begin- en
eindwaarden van de trendreeks gevoelig zijn voor bijkomende observaties in
de basisreeks. Voor historische studies kan dit probleem grotendeels
ondervangen worden door de basisreeks zo lang mogelijk te kiezen en
vervolgens aan beide uiteinden van de berekende trendreeks een aantal
gegevens weg te laten. Voor conjunctuuronderzoek echter zijn precies de
meest recente gegevens van groot belang. In dit geval is het aangewezen de
basisreeks naar de toekomst toe te extrapoleren op basis van de meest
waarschijnlijk geachte evolutie. Op die manier kan men herzieningen van de
trendcyclische opsplitsing als gevolg van bijkomende waarnemingen in de
basisreeks beperken in omvang.


&TI Voorbeeld
ÄÄÄÄÄÄÄÄÄÄÄÄÄ
Het gebruik van de HP-filter wordt hieronder ge‹llustreerd aan de hand van de
reeks voor het Belgische BBP in constante prijzen (ESER-definitie). Voor de
basisreeks beschikken we over observaties voor de periode 1953-1992. De
betrokken reeks bevat uiteraard geen seizoenscomponent en de
toevalscomponent wordt verondersteld gelijk aan nul te zijn.

Onderstaande figuren tonen, voor verschillende waarden van l, telkens drie
trendreeksen voor de groeivoet van het BBP:
&EN BBP92: trend berekend via toepassing van de HP-filter op de basisreeks
van de waarnemingen (1953-92);
&EN  BBP95: trend berekend via toepassing van de HP-filter op de basisreeks,
die voor de periode 1993-95 verlengd werd op basis van het Economisch
Budget 1995 (juli 1994);
&EN  BBP98: trend berekend via toepassing van de HP-filter op de basisreeks,
die voor de periode 1993-98 verlengd werd op basis van de Economische
Vooruitzichten 1994-98 (maart 1994).

Opvallend is dat de variabele BBP92 aan het einde van de reeks een lichte
stijging vertoont, die zich niet of in duidelijk mindere mate voordoet indien de
basisreeks beredeneerd wordt doorgetrokken tot 1995, resp. 1998. Dit doet
vermoeden dat de trendreeks BBP92 aan het einde (onterecht) een deel van
de opgaande cyclische beweging uit het einde van de jaren 80 capteert.

Samengevat kunnen we dus stellen:
&EN bij toepassing van de HP-filter ontstaat een bias aan het begin en het
einde van de basisreeks: de trendreeks capteert er, in vergelijking met de
rest van de periode, een groter deel van de cyclische beweging;
&EN  deze bias is verantwoordelijk voor de instabiliteit bij toevoeging van
additionele observaties;
&EN  deze bias kan worden beperkt door de basisreeks te verlengen op basis
van de meest waarschijnlijk geachte evolutie;
&EN  verschillen in deze extrapolaties (vergelijk BBP95 en BBP98) leiden tot
verschillende trendwaarden, doch deze laatste verschillen vallen relatief
kleiner uit dan de verschillen in de extrapolaties van de basisreeks.

Wat de keuze van ~ilambda~I betreft, kan men zich laten leiden door de volgende
overweging: de trendreeks wordt bepaald en moet dus kunnen worden
verklaard door structurele factoren. Zo is het b.v. duidelijk dat zich sinds de
jaren 60 een aantal structuurwijzigingen hebben voorgedaan die de daling van
de trendmatige groei van het BBP wettigen. Voor ~clambda=100~C stelt men echter
tijdens de jaren 80 opnieuw een, weliswaar lichte, stijging van de trendmatige
groei vast, die zich niet manifesteert bij hogere waarden van ~ilambda.~I Het
antwoord op de vraag of deze stijging structureel te verklaren is, kan een
leidraad vormen voor de keuze van de parameter ~clambda~C.

&IT Trendgroei van het BBP tegen constante prijzen: 1980-92 (l=100)
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

~a'hp5.png'

&IT Trendgroei van het BBP tegen constante prijzen: 1980-92 (l=200)
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

~a'hp6.png'


&IT Trendgroei van het BBP tegen constante prijzen: 1980-92 (l=400)
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

~a'hp7.png'


&IT Groei en trendgroei van het BBP tegen constante prijzen: 1953-92 (l=200)
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

~a'hp8.png'


&TI  Literatuur
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
De Hodrick-Prescott filter wordt o.a. beschreven in:

&EN Hodrick R.J. and E.C. Prescott (1980). ïPost-war U.S. business cycles:
an empirical investigationğ, Carnegie-Mellon University Discussion Paper, nr.
451, Revised November 1980, 27 pp.
&EN Nicoletti G. and L. Reichlin (1993). ïTrends and cycles in labour
productivity in the major OECD countriesğ, OECD Economic Department
Working Papers, nr. 129, 41-42.
&EN Nunes J. and H. Ongena (1994). ïEstimation of trend output in cyclical
adjustment of budget balances: robustness of different methodsğ,
European Commission, Directorate-General for Economic and Financial Affairs, 10 pp.
&EN Prescott E.C. (1986). ïTheory ahead of business-cycle measurementğ,
Carnegie-Rochester Conference Series on Public Policy, nr. 25, 11-44.

>




