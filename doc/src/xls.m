/* XLS */
#include "macros.h"

<Interface entre Excel et Iode>
    Interface tussen Excel en Iode
    컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

IODE kan gegevens uitwisselen met andere Windows Programma's. Dit gebeurt
via het DDE (Dynamic Data Exchange) protocol. DDE laat twee programma's toe
met elkaar te "spreken" en zo gegevens uit te wisselen. DDE automatiseert
in feite het "cut" en "paste" proces tussen programma's.

Het programma dat de gegevens verstrekt is de DDE-server. De applicatie die
de gegevens opvraagt is de DDE-client. Beide programma's moeten
tegelijkertijd draaien en kunnen alleen de gegevens uitwisselen die op dat
ogenblik geladen zijn. IODE is een DDE-server (IODE-data naar Excel of APL)
en een DDE-client (Excel-data of APL-vectoren naar IODE).

Data opgeslaan in IODE is gekend via het type (VAR, IDT, ...) en de naam
van het gewenste object. Data opgeslaan in Excel wordt gereferenceerd via
een "range". Een range is ofwel een "named range, ofwel een samenstelling
van de naam van het werkblad en de namen van de linkerboven cel en de
rechteronder cel. Zo duidt "Sheet1!R1C1:R2C4" de eerste vier cellen van de
eerste rij en de tweede rij aan. Als we nu een variable A vanuit IODE naar
Excel willen kopi뎣en dan moeten wij in Excel een range gebruiken die groot
genoeg is om de gegevens op te slaan.

Niet alle data kan worden uitgewisseld naar Excel. Wij beperken ons tot
variabelen, commentaren, lijsten en berekende tabellen. Identiteiten,
scalairen en vergelijkingen hebben geen vergelijkbare tegenhanger in Excel.
In de andere richting kunnen alleen IODE-variabelen worden ge걈datet met
gegevens uit Excel.

&NO Gebruikt u IODE als DDE-server, laat dan slechts een instantie van het
    programma lopen. Als er meerdere gelijktijdige sessies zijn, kan men niet
    voorspellen welke sessie zal antwoorden op de gestelde vragen.

&NO IODE gebruikt het decimaal punt ipv de komma. Om gebruik te maken van de
    DDE-mogelijkheden moet uw Windows-systeem ook het decimaal punt gebruiken.
    Pas dit in uw "Control Panel" aan. Indien niet krijgt u bizarre resultaten.


Dit hoofdstuk beschrijft in detail de manier waarop u data tussen IODE en
Excel kan uitwisselen:

&EN <Vanuit IODE-rapporten>

&EN <Vanuit Excel>

&EN Technisch addendum <DDE-server functies>
>

<Vanuit IODE-rapporten>
    Vanuit IODE-rapporten
    컴컴컴컴컴컴컴컴컴컴�

Een IODE-rapport kan niet alleen gegevens uitwisselen met Excel, u kan
Excel ook enkele van zijn kunstjes laten vertonen. U kan Excel een Workbook
van disk laten laden, enkele aanpassingen doen en zo live uw
voorgedefini뎓rde grafieken zien veranderen, uw resultaten laten afdrukken
of wegschrijven voor later gebruik.

De Excel-commando's bruikbaar vanuit rapporten zijn de volgende
&EN Gegevens uitwisselen
&EN2 <ExcelGetVar>
&EN2 <ExcelGetScl>
&EN2 <ExcelGetCmt>
&EN2 <ExcelSetXxx>
&EN2 <ExcelWrite>
&EN2 <ExcelLang>
&EN2 <ExcelDecimal>
&EN2 <ExcelThousand>
&EN2 <ExcelCurrency>

&EN Excel commando's uitvoeren
&EN2 <ExcelOpen>
&EN2 <ExcelNew>
&EN2 <ExcelClose>
&EN2 <ExcelPrint>
&EN2 <ExcelSave>
&EN2 <ExcelSaveas>

&EN <Demo><ExcelDemo>
>

<ExcelOpen>
    ExcelOpen
    컴컴컴컴컴

&DE
Excel opent de file FileName. In FileName geeft u het volledig pad aan
waar Excel u file kan vinden.

&NO Als Excel niet draait en het pad naar Excel.exe
    staat in uw systeemvariabele PATH dan wordt Excel opgestart. Indien
    niet dan krijgt u een foutmelding.

&FS
Deze functie heeft geen Fullscreen tegenhanger


&NFS

&SY2

    $ExcelOpen FileName
&TX

&EX2
	$ExcelOpen c:\usr\iode\test.xls
	$ExcelSetTbl INF 90:11 Sheet1!R1C1
	$ExcelSaveAs c:\usr\iode\inflatie.xls
&TX
>

<ExcelSave>
    ExcelSave
    컴컴컴컴컴

&DE
Slaat de openstaande workbook op onder haar oorpsronkelijke naam.

&FS
Deze functie heeft geen Fullscreen tegenhanger

&NFS

&SY2
    $ExcelSave
&TX

&EX2
	$ExcelOpen c:\usr\iode\test.xls
	$ExcelSetTbl INF 90:11 Sheet1!R1C1
	$ExcelSave
&TX
>

<ExcelNew>
    ExcelNew
    컴컴컴컴�
&DE
Excel cre뎓rt een nieuw spreadsheet, afhankelijk van de versie en de
configuratie van uw Excel-omgeving heeft dit werkbook 3 (Excel 8) of 14
werkbladen (Excel 7)

&NO Als Excel niet draait en het pad naar Excel.exe
    staat in uw systeemvariabele PATH dan wordt Excel opgestart. Indien
    niet dan krijgt u een foutmelding.

&FS
Deze functie heeft geen Fullscreen tegenhanger


&NFS

&SY2

    $ExcelNew
&TX

&EX2
	$ExcelNew
	$ExcelSetTbl INF 90:11 Sheet1!R1C1
	$ExcelSaveAs c:\usr\iode\inflatie.xls
&TX

>

<ExcelClose>
    ExcelClose
    컴컴컴컴컴�

&DE
Excel opent de file FileName. In FileName geeft u het volledige pad aan
waar Excel uw file kan vinden.

&NO Als Excel niet draait en het pad naar Excel.exe
    staat in uw systeemvariabele PATH dan wordt Excel opgestart. Indien
    niet dan krijgt u een foutmelding.

&FS
Deze functie heeft geen Fullscreen tegenhanger


&NFS

&SY2

    $ExcelOpen FileName
&TX

&EX2
	$ExcelOpen c:\usr\iode\test.xls
	$ExcelSetTbl INF 90:11 Sheet1!R1C1
	$ExcelSaveAs c:\usr\iode\inflatie.xls
&TX
>

<ExcelSaveAs>
    ExcelSaveAs
    컴컴컴컴컴컴
&DE
Excel opent de file FileName. In FileName geeft u het volledig pad aan
waar Excel u file kan vinden.

&NO Als FileName al bestaat krijgt u de gewone Excel-foutmeldingen

&NO Als Excel niet draait en het pad naar Excel.exe
    staat in uw systeemvariabele PATH dan wordt Excel opgestart. Indien
    niet dan krijgt u een foutmelding.

&FS
Deze functie heeft geen Fullscreen tegenhanger


&NFS

&SY2

    $ExcelSaveAs FileName
&TX

&EX2
	$ExcelOpen c:\usr\iode\test.xls
	$ExcelSetTbl INF 90:11 Sheet1!R1C1
	$ExcelSaveAs c:\usr\iode\inflatie.xls
&TX
>

<ExcelPrint>
    ExcelPrint
    컴컴컴컴컴
&DE
Print het openstaande workbook op uw Windows-default printer.

&FS
Deze functie heeft geen Fullscreen tegenhanger


&NFS

&SY2
    $ExcelPrint
&TX

&EX2
	$ExcelOpen c:\usr\iode\test.xls
	$ExcelSetTbl INF 90:11 Sheet1!R1C1
	$ExcelPrint
&TX
>

<ExcelSetXxx>
    ExcelSetXxx
    컴컴컴컴컴컴
&EXTWS
&DE
Deze functie plaatst IODE-objecten in een openstaand Excel-werkblad. Zij is
alleen beschikbaar voor variabelen, scalairen, commentaren, lijsten en berekende
tabellen.

&FS
Deze functie heeft geen Fullscreen tegenhanger

&NFS
ExcelSetXxx neemt een aantal argumenten, afhankelijk van het type van
het object. Het eerste argument is altijd de naam van het IODE-object.
Het laatste geeft de plaats (range) aan waar u in Excel de informatie
terugvindt.

Een range in Excel bestaat uit de naam van het werkblad gevolgd door de
namen van de linkerboven cel en de rechteronder cel. Zo duidt
"Sheet1!R1C1:R2C4" de eerste vier cellen van de eerste rij en de tweede rij
aan. Om het u gemakkelijker te maken volstaat het de naam van de linkerboven
cel te geven, IODE vult de rest aan naargelang de grootte van het te
transfereren object.


&IT Commentaren
&CO
    $ExcelSetCmt naam excelrange
&TX

&EX2
	$ExcelNew
	$ExcelSetCmt A0G Sheet1!R1C1
&TX

&IT Tabellen
&CO
    $ExcelSetTbl naam {gsample} excelrange
&TX
gsample is een generalised sample zoals u dit kent van het PrintTbl commando.
Vult u hier echter geen gsample in dan wordt de tabel afgedrukt over de
gehele sample van de workspace.

&EX2
	$ExcelNew
	$ExcelSetTbl INF 90:11 Inflation!R1C1
	$ExcelPrint
&TX

&IT Variabelen
&CO
    $ExcelSetVar naam excelrange
&TX

&EX2
	$ExcelNew
	$ExcelSetVar A0G Sheet1!R1C1
&TX
De gevraagde variabele wordt over de gehele workspace door gegeven.
>

<ExcelGetVar>
    ExcelGetVar
    컴컴컴컴컴컴

&DE
Deze functie haalt waarden uit een Excel spreadsheet en doet een update
van de variable in IODE.

&FS
Deze functie heeft geen Fullscreen tegenhanger


&NFS
Bestaat de variabele al dan worden de oude waarden overschreven met die uit
Excel. Bestaat ze nog niet dan wordt de variabele gemaakt en krijgt als
waarde de waarden uit het Excel-spreadsheet.

Deze functie gelijkt sterk op het DataUpdateVar commando, alleen worden hier
de nieuwe waarden via een Excel-range gedefinieerd.

&SY2
    $ExcelGetVar VARNAME [periode] excelrange
&TX

	Haalt uit de Excel-range de waarden voor de update van "varname".
	Wordt de "begin period" niet aangegeven, dan wordt de begin periode
	van de sample aangenomen.

&EX2
	$ExcelOpen c:\usr\iode\test.xls
	$ExcelGetVar A 1980Y1 Sheet1!R1C1:R2C10
	$ExcelSaveAs c:\usr\iode\inflatie.xls
&TX
>

<ExcelGetScl>
    ExcelGetScl
    컴컴컴컴컴컴

&DE
Deze functie haalt waarden uit een Excel spreadsheet en doet een update
van de scalaire in IODE.

&FS
Deze functie heeft geen Fullscreen tegenhanger

&NFS
Bestaat de scalaire al dan worden de oude waarde overschreven met die uit
Excel. Bestaat ze nog niet dan wordt de scalaire gemaakt en krijgt als
waarde de waarde uit het Excel-spreadsheet.

&SY2
    $ExcelGetVar SCLNAME excelrange
&TX

&EX2
	$ExcelOpen c:\usr\iode\test.xls
	$ExcelGetScl c1 Sheet1!R1C1:R1C1
	$ExcelSaveAs c:\usr\iode\inflatie.xls
&TX
>

<ExcelGetCmt>
    ExcelGetCmt
    컴컴컴컴컴컴

&DE
Deze functie haalt text uit een Excel spreadsheet en doet een update
van de commentaar in IODE.

&FS
Deze functie heeft geen Fullscreen tegenhanger

&NFS
Bestaat de commentaar al dan wordt de oude waarde overschreven met die uit
Excel. Bestaat ze nog niet dan wordt de commentaar gemaakt en krijgt als
waarde de text uit het Excel-spreadsheet.

&SY2
    $ExcelGetCmt CMTNAME excelrange
&TX

&EX2
	$ExcelOpen c:\usr\iode\test.xls
	$ExcelGetCmt A Sheet1!R1C1:R1C1
	$ExcelSaveAs c:\usr\iode\inflatie.xls
&TX
>

<ExcelWrite>
    ExcelWrite
    컴컴컴컴컴컴

&DE
Cette fonction permet d'괹rire du texte ou des valeurs quelconques dans une feuille Excel.

&FS
N'existe pas sous forme Fullscreen.

&NFS
Remplace les cellules sp괹ifi괻s par la ou les valeurs donn괻s. Plusieurs
colonnes sont s굋ar괻s par ~c\t~C et plusieurs lignes par ~c\n~C.

&SY2
    $ExcelWrite MySheet!R4C4 texte[\ttexte][\ntexte][...]
&TX

&EX2
	$ExcelOpen c:\usr\iode\test.xls
	$ExcelGetCmt MySheet!R1C1 Titre\n A:\t{A[1970Y1]}\t{A[1980Y1]}\t{A[1990Y1]}
	$ExcelSaveAs c:\usr\iode\test.xls
&TX
>

<ExcelLang>
    ExcelLang
    컴컴컴컴�
&DE
Par d괽aut, IODE consid둹e que la version linguistique de Excel est l'anglais. Cela implique que les ~cRanges~C
dans Excel doivent s'괹rire ~cRnCn~C (par exemple R2C4 pour ligne (Row) 2, colonne (Column) 4).

Dans les autres langues, cette 괹riture ne convient plus et IODE doit
envoyer les donn괻s par exemple vers ~cL2C4~C au lieu de ~cR2C4~C dans la version fran놹ise.

Pour assurer qu'un rapport IODE reste valable quelle que soit la langue
d'Excel, la commande ~c$ExcelLang~C a 굏� ajout괻 � IODE.

&FS
N'existe pas sous forme Fullscreen.

&NFS

&SY2
    $ExcelLang {F|N}
&TX
Tout autre param둻re (E par exemple) remettra IODE en version anglaise.


Dans l'exemple ci-dessous, IODE enverra le contenu de la variable AAA �
partir de la cellule L1C1 du Sheet1 du fichier Excel ouvert.

&EX2
    $ExcelLang F
    $ExcelSetVar AAA Sheet1!R1C1
&TX

&NO Note

Il faut noter que dans la commande ~c$ExcelSetXxx~C, ~ble range reste d괽ini
comme ~cR1C1~C~B. C'est uniquement en interne qu'IODE transformera cette
information en ~cL1C1~C.
>

<ExcelDecimal>
    ExcelDecimal
    컴컴컴컴컴컴컴
&DE
Par d괽aut, IODE envoie les donn괻s num굍iques vers Excel en utilisant
 ~ble s굋arateur d괹imal d괽ini dans les "Regional Settings" de Windows
 (voir Control Panel)~B.

Cette option ne fonctionne que si le s굋arateur d괹imal n'a pas 굏� modifi�
dans Excel : on peut en effet d괹ider d'utiliser le ~bpoint d괹imal~B dans Excel alors
que dans Windows, c'est la ~bvirgule~B qui a 굏� choisie.

Pour permettre de forcer l'envoi vers une version Excel dans laquelle le s굋arateur d괹imal
ne serait pas celui de Windows, une nouvelle fonction de rapport a 굏� introduite.

&FS
N'existe pas sous forme Fullscreen.

&NFS
&SY2
    $ExcelDecimal {C}
&TX
L'absence de param둻re C ou tout autre param둻re que C (comma) s굃ectionne le point d괹imal.

Dans l'exemple ci-dessous, IODE enverra le contenu de la variable AAA avec des virgules comme s굋arateur d괹imal
� partir de la cellule L1C1 du Sheet1 du fichier Excel ouvert.

&EX2
    $ExcelDecimal C
    $ExcelLang F
    $ExcelSetVar AAA Sheet1!R1C1
&TX

>

<ExcelThousand>
    ExcelThousand
    컴컴컴컴컴컴컴�
&DE
Par d괽aut, IODE lit la d괽inition des valeurs des s굋arateurs de milliers
dans les param둻res "r괾ionaux" (Regional
Settings). Si cette valeur ne convient pas, elle peut 늯re
modifi괻 par la commande de rapport $ExcelThousand.

&FS
N'existe pas sous forme Fullscreen.

&NFS
&SY2
    $ExcelThousand [char]
&TX

La valeur de char remplace celle du s굋arateur de milliers d괽inie dans les
param둻res de Windows. Lorsque ce caract둹e est rencontr� lors du transfert
d'Excel vers IODE, il est ignor�.

Certaines valeurs sp괹iales de ~cchar~C ont une interpr굏ation particuli둹e :

&EN d, D, p ou P : le s굋arateur est le point
&EN c ou C : le s굋arateur est la virgule
&EN n, N ou pas d'argument : il n'y a pas de s굋arateur
&EN ~cspace~C ou ~cs~C ou ~cSpace~C : le s굋arateur est l'espace
&EN tout autre valeur est prise telle quelle comme s굋arateur

&EX2
&CO
  |    B       C       D       E
--|--------------------------------
3 | $10,000 $20,000 $30,000 $40,000
4 | $12,345 $23,456 $34,567 $45,678
&TX

Rapport :

&CO
$WSSample 1989Y1 2000Y1
$ExcelThousand C
$ExcelCurrency D
$ExcelGetVar A 1990Y1 Sheet1!R3C2:R3C10
&TX

>

<ExcelCurrency>
    ExcelCurrency
&DE
Par d괽aut, IODE lit la d괽inition de la devise
dans les param둻res "r괾ionaux" (Regional
Settings). Si cette valeur ne convient pas, elle peut 늯re
modifi괻 par la commande de rapport $ExcelCurrency.

&FS
N'existe pas sous forme Fullscreen.

&NFS
&SY2
    $ExcelCurrency [char]
&TX

La valeur de char remplace celle du caract둹e indiquant la devise dans les
param둻res de Windows. Lorsque ce caract둹e est rencontr� lors du transfert
d'Excel vers IODE, il est ignor�.

Certaines valeurs sp괹iales de ~cchar~C ont une interpr굏ation particuli둹e :

&EN d, D : dollar
&EN e ou E : euro
&EN p ou P : sterling pound
&EN pas d'argument : il n'y a pas de signe de devise
&EN tout autre valeur est prise telle quelle comme signe de devise

&EX2
&CO
  |    B       C       D       E
--|--------------------------------
3 | $10,000 $20,000 $30,000 $40,000
4 | $12,345 $23,456 $34,567 $45,678
&TX

Rapport :

&CO
$WSSample 1989Y1 2000Y1
$ExcelThousand C
$ExcelCurrency D
$ExcelGetVar A 1990Y1 Sheet1!R3C2:R3C10
&TX

>

<Excel Demo>
    Demo: bestuur Excel vanuit een IODE rapport
    컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
&EX
    Load the variable file
    $WsLoadVar c:\usr\mt\anl\deval\dip\nov

    Load the table file
    $WsLoadTbl c:\usr\mt\anl\deval\dip\dip

    Open in Excel the spreadsheet with the defined graphics
    $ExcelOpen c:\usr\mt\anl\deval\dip\dip.xls

    Place the table(s) with calculated series, base version
    $ExcelSetTbl DIP1 90:11 Growth!R2C12
    $ExcelSetTbl DIP2 90:11 Unemployment!R2C12
    $ExcelSetTbl DIP3 90:11 CurrentAccount!R2C12
    $ExcelSetTbl DIP4 90:11 GovtAccount!R2C12
    $ExcelSetTbl DIP5 90:11 Inflation!R2C12
    $ExcelSetTbl DIP6 90:11 RealDisposableIncome!R2C12

    Place the table(s) with calculated series, simulated version
    $ExcelSetTbl DIP1 90:11 Growth!R9C12
    $ExcelSetTbl DIP2 90:11 Unemployment!R9C12
    $ExcelSetTbl DIP3 90:11 CurrentAccount!R9C12
    $ExcelSetTbl DIP4 90:11 GovtAccount!R9C12
    $ExcelSetTbl DIP5 90:11 Inflation!R9C12
    $ExcelSetTbl DIP6 90:11 RealDisposableIncome!R9C12

    $Ask simulation, Start Simulation?
    $return

    $Label simulation

    Change the hypothesis and see what heapens

    $WsLoadVar oeso2.var
    $WsLoadIdt NOTE
    $WsSample 1970Y1 2000Y1
    $WsExtrapolate 0 1998Y1 2000Y1

    Change the Exchange Rate BF/$

    $DataUpdateIdt BELVAR7 BELVAR7*1.15

    Change the Effective exchange rate

    $DataUpdateIdt BELVAR6 BELVAR6*.9
    $IdtExecute 1993Y1 2000Y1 BELVAR7 BELVAR6

    $IdtExecute 1991Y1 2000Y1 PWXDAN PWXDEU PWXESP PWXFRA PWXGRE PWXIRL
    $IdtExecute 1991Y1 2000Y1 QWXAB QWXS PWXAB PWXS PWMAB EX EER RLGY RSGY

    $WsSaveVar oeso2test.var

    $WsClearIdt Note
    $WsLoadEqs nov
    $WsLoadVar nov
    $WsLoadScl note
    $WsCopyVar oeso2test QWXAB PWXAB PWMAB PWMS PWXS QWXS EX EER RLGY

    Initiliase the first year and simulate a year at a year
    Your predefined graphics in Excel change at the same year

    $SetTime 1992Y1
    $Label again
    $IncrTime 1

    $ModelSimulateParms .001 0.5 100 C 4 N 2 N
    $modelsimulate {t@T} {t@T}


    Visualize the changes in Excel
    $ExcelSetTbl DIP1 90:11 Growth!R9C12
    $ExcelSetTbl DIP2 90:11 Unemployment!R9C12
    $ExcelSetTbl DIP3 90:11 CurrentAccount!R9C12
    $ExcelSetTbl DIP4 90:11 GovtAccount!R9C12
    $ExcelSetTbl DIP5 90:11 Inflation!R9C12
    $ExcelSetTbl DIP6 90:11 RealDisposableIncome!R9C12

    $goto continue {t >> 1999Y1}
    $WsCopyVar nov {t+1@T} 2000Y1 QBBP PC VBBP FLG VX VM YDH U NAT
    $label continue

    Loop until 2000 reached
    $goto again {t << 2000Y1}

&TX
>

<Vanuit Excel>
    Vanuit Excel
    컴컴컴컴컴컴

U kan ook vanuit Excel IODE besturen. Niet alleen data is uitwisselbaar, ook
IODE-commando's en rapporten kunnen vanuit Excel worden uitgevoerd.

Er zijn twee manieren om vanuit Excel IODE-data en commando's te gebruiken.
Eerst is er de DDE-interface die u in gelijk welke cel van een
Excel-werkblad invult. Daar moet u zich wel strikt houden aan de in IODE
geprogrammeerde DDE-calls. De syntax is niet zo eenvoudig en is beschreven
in het technisch addendum <DDE-server functies> .

Een tweede manier is gebruik te maken van het <Iode.xls werkblad> dat bij uw
software wordt geleverd. Daar is alles voorgeprogrammeerd en is zelfs door
een leek te gebruiken.

&NO Gebruikt u IODE als DDE-server, laat dan slechts een instantie van het
    programma lopen. Als er meerdere gelijktijdige sessies zijn, kan men niet
    voorspellen welke sessie zal antwoorden op de gestelde vragen.

&NO IODE gebruikt het decimaal punt ipv de komma. Om gebruik te maken van de
    DDE-mogelijkheden moet uw Windows-systeem ook het decimaal punt gebruiken.
    Pas dit in uw "Control Panel" aan. Indien niet krijgt u bizarre resultaten.
>

<Iode.xls werkblad>
    Iode.xls werkblad
    컴컴컴컴컴컴컴컴�

Als u bij de IODE-installatie ook Excel interface heeft gekozen vindt u bij
het "IODE 5" menu een submenu "Excel-IODE interface". Infeite gaat het hier
om een werkboek met enkele Visual Basic procedures. Zij maken gebruik van de
verder beschreven DDE-functies in IODE. Alleen zijn zij verpakt in
gemakkelijk te gebruiken interface.

Als u in Excel het werkblad "Iode.xls" opent, wordt een verbinding gelegd
met een openstaande IODE-sessie. In het "IODE Status" werkblad krijgt u een
overzicht van de workspaces die op dat ogenblik geladen zijn.


.~a'xlsstat.gif'

Daarnaast vindt u de werbladen "VAR", "CMT", "LST", waarin u de IODE-data
zal visualiseren.

U ziet dat de MenuBar van Excel met een "Iode" menu is uitgebreid. Hier
vindt u alle IODE-mogelijkheden waarover u kan beschikken:

&EN Execute IODE commands
&EN2 <Load IODE objects>
&EN2 <Execute IODE report>
&EN2 <Execute report commands>

&EN Get IODE Objects
&EN2 <Comments><GetComments>
&EN2 <Lists><GetLists>
&EN2 <Tables><GetTables>
&EN2 <Variables><GetVariables>
>

<Load IODE objects>
    Load IODE objects
    컴컴컴컴컴컴컴컴�

.~a'xlsload.gif'

&DE
Dit menu laadt IODE-data files, voor gebruik in Excel in.


&TX
Excel opent een dialoog-venster waarin u de filename invult met de data
waarin u in deze Excel-sessie wil over beschikken.

U vult alleen de filename in van de gegevens in waarover u wil beschikken.
Excel vraagt IODE de files in te laden. Als ui de prompt krijgt voor

&NO Geef het volledig pad, en vergeet de object extensie (bijv. var) niet.

>

<Execute IODE report>
    Execute IODE report
    컴컴컴컴컴컴컴컴컴�
.~a'xlsrep.gif'
&DE
Dit menu laat u een IODE-rapport uitvoeren

&TX
Excel opent een dialoog-venster waarin u de filename van het rapport invult
dat u wil uitvoeren. Het gaat hier over gelijk wel geldig IODE-rapport. U
kan dus ook alle $ExcelXxx commando's van IODE gebruiken.

&NO Geef het volledig pad, en vergeet de extensie (rep) niet.
>

<Execute report commands>
    Execute report commands
    컴컴컴컴컴컴컴컴컴컴컴�
.~a'xlscmd.gif'
&DE
Dit menu laat u een IODE-commando uitvoeren

&TX
Excel opent een dialoog-venster waarin u de filename van het IODE-commando
invult dat u wil uitvoeren. Het gaat hier over gelijk wel geldig
IODE-commando. U kan dus ook alle $ExcelXxx commando's van IODE gebruiken.

&NO U kan alleen die objecten bekijken die op dat ogenblijk in IODE geladen
    zijn.
>

<GetLists>
    Lists
    컴컴�
&DE
Dit menu laat u een IODE-lijsten zien in uw "LST"-werkblad

&TX
Excel opent een dialoog-venster waarin u de naam van de IODE-list kan
invullen. In uw "LST"-werkblad verschijnt de naam en de inhoud van uw
gevraagde lijst.

Wil u meerdere zien tegelijkertijd; vul de namen in afgewisseld met een
blanco of komma.

Vult u niets in en drukt u op de "OK"-toets dan wordt de gehele LST-databank
vanuit IODE naar Excel geschreven.

&NO U kan alleen die objecten bekijken die op dat ogenblijk in IODE geladen
    zijn.
>

<GetComments>
    Comments
    컴컴컴컴
&DE
Dit menu laat u een IODE-commentaren zien in uw "CMT"-werkblad

&TX
Excel opent een dialoog-venster waarin u de naam van de IODE-commentaar kan
invullen. In uw "CMT-werkblad verschijnt de naam en de inhoud van de
gevraagde commentaar.
Wil u meerdere zien tegelijkertijd; vul de namen in afgewisseld met een
blanco of komma.

Vult u niets in en drukt u op de "OK"-toets dan wordt de
gehele CMT-databank vanuit IODE naar Excel geschreven.

&NO U kan alleen die objecten bekijken die op dat ogenblijk in IODE geladen
    zijn.
>

<GetTables>
    Tables
    컴컴컴
&DE
Dit menu laat u een IODE-tabellen zien. Voor elke tabel wordt een werkblad
"Tbl tabelnaam" gecre뎓rd, waarin u de tabel ziet berekend over de gehele
sample van de geladen VAR-databank

&TX
Excel opent een dialoog-venster waarin u de naam van de IODE-tabel kan
invullen. In uw "Tbl tabelnaam"-werkblad verschijnt de naam en de inhoud van uw
gevraagde tabel.

&NO U kan alleen die tabelen bekijken die op dat ogenblijk in IODE geladen
    zijn en waarvoor alle nodige variabelen aanwezig zijn voor het berekenen van
    de tabel.
>

<GetVariables>
    Variables
    컴컴컴컴�
.~a'xlsvar.gif'
&DE
Dit menu laat u een IODE-variabelen zien.

.~a'xlsrvar.gif'

&TX
Excel opent een dialoog-venster waarin u de naam van de IODE-variabele kan
invullen. In uw "VAR"-werkblad verschijnt de naam en de inhoud van uw
gevraagde variabele over de gehele sample van de gealden var-file. Op de
eerste lijn vindt u aanduiding van de sample.

Wil u meerdere zien tegelijkertijd; vul de namen in afgewisseld met een
blanco of komma.

Vult u niets in en drukt u op de "OK"-toets dan wordt de gehele VAR-databank
vanuit IODE naar Excel geschreven.

&NO U kan alleen die objecten bekijken die op dat ogenblijk in IODE geladen
    zijn.
>

<DDE-server functies>

    Fonctions du serveur DDE
    컴컴컴컴컴컴컴컴컴컴컴컴

Les donn괻s techniques qui suivent permettent d'exploiter le
serveur DDE dans le cadre d'autres programmes.

Service : IODE

Topics  : WS (Request et Poke), REP (Poke), PLAY (Poke), XVAR, XCMT
	      CMT, EQS, IDT, LST, SCL, TBL, VAR (Request et Poke)

&EN <Request>
&EN <Poke>
>

<Request>
    Request
    컴컴컴�

&TI Items de WS

&EN SAMPLE = sample du WS IODE courant

&EN List
&EN2 CLIST  = liste des commentaires du WS IODE courant
&EN2 ELIST  = liste des 굌uations du WS IODE courant
&EN2 ILIST  = liste des identit굎 du WS IODE courant
&EN2 LLIST  = liste des listes du WS IODE courant
&EN2 SLIST  = liste des scalaires du WS IODE courant
&EN2 TLIST  = liste des tableaux du WS IODE courant
&EN2 VLIST  = liste des variables du WS IODE courant

&EN Name
&EN2 CNAME  = nom du fichier des commentaires de IODE
&EN2 ENAME  = nom du fichier des 굌uations de IODE
&EN2 INAME  = nom du fichier des identit굎 de IODE
&EN2 LNAME  = nom du fichier des listes de IODE
&EN2 SNAME  = nom du fichier des scalaires de IODE
&EN2 TNAME  = nom du fichier des tableaux  de IODE
&EN2 VNAME  = nom du fichier des variables de IODE

&EN2 CDESCR = description du fichier des commentaires de IODE
&EN2 EDESCR = description du fichier des 굌uations de IODE
&EN2 IDESCR = description du fichier des identit굎 de IODE
&EN2 LDESCR = description du fichier des listes de IODE
&EN2 SDESCR = description du fichier des scalaires de IODE
&EN2 TDESCR = description du fichier des tableaux  de IODE
&EN2 VDESCR = description du fichier des variables de IODE

&EN Nb
&EN2 CNB    = nombre de commentaires dans IODE
&EN2 ENB    = nombre de 굌uations dans IODE
&EN2 INB    = nombre de identit굎 dans IODE
&EN2 LNB    = nombre de listes dans IODE
&EN2 SNB    = nombre de scalaires dans IODE
&EN2 TNB    = nombre de tableaux  dans IODE
&EN2 VNB    = nombre de variables dans IODE

&TI Items de CMT, EQS, IDT, LST, SCL, TBL, VAR

L'item est le nom de l'objet. La valeur retourn괻 est la d괽inition de
l'objet.

&TI Items de XVAR

L'item est compos� de trois 굃굆ents s굋ar굎 par des ! :

&EN nom de la ou des variables s굋ar굎 par des virgules ou des blancs. t
    repr굎ente une ligne de p굍iodes

&EN nom du sheet o� placer le r굎ultat

&EN range de la premi둹e cellule dans ce sheet

Cette requ늯e g굈둹e un Poke vers excel avec la d괽inition des variables et
leur code.

Si aucun nom de s굍ie n'est donn�, une s굍ie avec les valeurs des p굍iodes
pr괹둪e. Sinon, on peut placer les p굍iodes en indiquant la s굍ie t
(minuscule).

S'il n'y a pas de sheet, Sheet1 est choisi comme destination S'il n'y a pas
de range, R1C1 est choisi comme destination

&EX2

       IODE|XVAR!' ' : toutes les s굍ies dans le sheet1, en R1C1
		       avec les p굍iodes
       IODE|XVAR!'t,QC0,QAFF!Sheet2!R1C1' : s굍ies QCO et QAFF
		       dans le Sheet2 en position R1C1, avec
		       les p굍iodes en premi둹e ligne
       IODE|XVAR!'QC0!Sheet2!R1C1' : s굍ie QCO sans les p굍iodes
&TX

&TI Items de XCMT

L'item est compos� de trois 굃굆ents s굋ar굎 par des ! :

&EN nom de la ou des commentaires s굋ar굎 par des virgules ou des blancs

&EN nom du sheet o� placer le r굎ultat

&EN range de la premi둹e cellule dans ce sheet

Cette requ늯e g굈둹e un Poke vers excel avec la valeur des commentaires et
leurs codes.

S'il n'y a pas de sheet, Sheet1 est choisi comme destination S'il n'y a pas
de range, R1C1 est choisi comme destination

&EX2
       IODE|XCMT!' ' : tous les commentaires dans le sheet1,
		       en R1C1
       IODE|XVAR!'QC0,QAFF!Sheet2!R1C1' : comments QCO et QAFF
		       dans le Sheet2 en position R1C1
       IODE|XVAR!'QC0!Sheet2!R5C2' : comment QCO en Sheet2, ligne
				5 colonne 2
&TX
>

<Poke>
    Poke
    컴컴

&TI Items de WS (Data en fonction du contexte)

&EN SAMPLE = change le sample du WS IODE courant (format IODE)

&EN Name
&EN2 CNAME  = change le nom du fichier des commentaires de IODE
&EN2 ENAME  = change le nom du fichier des 굌uations de IODE
&EN2 INAME  = change le nom du fichier des identit굎 de IODE
&EN2 LNAME  = change le nom du fichier des listes de IODE
&EN2 SNAME  = change le nom du fichier des scalaires de IODE
&EN2 TNAME  = change le nom du fichier des tableaux  de IODE
&EN2 VNAME  = change le nom du fichier des variables de IODE

&EN Descr
&EN2 CDESCR = change la description des commentaires de IODE
&EN2 EDESCR = change la description des 굌uations de IODE
&EN2 IDESCR = change la description des identit굎 de IODE
&EN2 LDESCR = change la description des listes de IODE
&EN2 SDESCR = change la description des scalaires de IODE
&EN2 TDESCR = change la description des tableaux  de IODE
&EN2 VDESCR = change la description des variables de IODE

&EN Clear
&EN2 CCLEAR = Clear des commentaires dans IODE
&EN2 ECLEAR = Clear des 굌uations dans IODE
&EN2 ICLEAR = Clear des identit굎 dans IODE
&EN2 LCLEAR = Clear des listes dans IODE
&EN2 SCLEAR = Clear des scalaires dans IODE
&EN2 TCLEAR = Clear des tableaux dans IODE
&EN2 VCLEAR = Clear des variables dans IODE

&EN Load
&EN2 CLOAD = Load un fichier de commentaires dans IODE
&EN2 ELOAD = Load un fichier d'굌uations dans IODE
&EN2 ILOAD = Load un fichier d'identit굎 dans IODE
&EN2 LLOAD = Load un fichier de listes dans IODE
&EN2 SLOAD = Load un fichier de scalaires dans IODE
&EN2 TLOAD = Load un fichier de tableaux dans IODE
&EN2 VLOAD = Load un fichier de variables dans IODE

&EN Save
&EN2 CSAVE = Sauve le fichier de commentaires de IODE
&EN2 ESAVE = Sauve le fichier d'굌uations de IODE
&EN2 ISAVE = Sauve le fichier d'identit굎 de IODE
&EN2 LSAVE = Sauve le fichier de listes de IODE
&EN2 SSAVE = Sauve le fichier de scalaires de IODE
&EN2 TSAVE = Sauve le fichier de tableaux de IODE
&EN2 VSAVE = Sauve le fichier de variables de IODE

&TI Items de PLAY

&EN TEXT : data = texte � placer dans le buffer clavier de IODE

&EN KEYS : data = texte des touches � placer dans le buffer
		    clavier : aA..aZ, aF1..aF10, cA..cZ, cF1..cF10,
			      sF1..sF10, ESC, ENTER, BACKSPACE,
			      HOME, END, LEFT, RIGHT, UP, DOWN,
			      PGUP, PGDN, DEL, TAB

&TI Items de CMT, EQS, IDT, LST, SCL, TBL, VAR

Nom de l'objet � modifier. Data contient la valeur avec des TAB comme
s굋arateur dans le cas des variables.

&TI Items de REP

La valeur de l'item est sans signification. La valeur de data est une
commande de rapport � ex괹uter (ex. '$WsLoadVar test')
>




