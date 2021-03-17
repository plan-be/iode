/* SQL */

#include "macros.h"

..esc ~
#include "macros.h"

<Interface tussen Relationele DB's en Iode>
    Interface tussen Relationele DB's en Iode
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

..esc ~
IODE kan gegevens uitwisselen met Databanken bereikbaar via ODBC. ODBC is
een standaard voorgesteld door Microsoft waarin de functies worden
ge‹mplementeerd voor het ondervragen van een Relationele Databank.

Elke databank-constructeur (DB2, Oracle, SQL-server, Access, SAS, ...)
beschikt over een ODBC-driver die de gebruiker toelaat om zo een databank
ontwikkeld in zijn omgeving vanuit een ander programma te raadplegen en
updaten.

Om die databanken te gaan ondervragen wordt de SQL (Structered Query
Language) gebruikt. Zij is min of meer gestandardiseerd (ANSI), elke
constructeur heeft wel zijn eigen dialectversie uitgebracht door er
bijkomende mogelijkheden aan toe te voegen. Het hangt dus van uw
databanksysteem af welk SQL-dialect u moet gebruiken.

IODE beschikt nu over een aantal rapportfuncties die u toe laten gegevens
opgeslaan in een relationele databank te gaan recupereren. Er wordt
verondersteld dat u over de nodige SQL-kennis beschikt om de data te
formateren zodat ze in IODE bruikbaar wordt.

&NO

IODE is slechts een SQL-client en geen SQL-server. Het is momenteel
onmogelijk in andere omgevingen via SQL-commando's IODE data te gaan
ondervragen.


Hier volgen de rapport-commando's waarover u kan beschikken.

&EN <@SqlOpen>
&EN <@SqlQuery>
&EN <@SqlNext>
&EN <@SqlField>
&EN <@SqlRecord>
&EN <@SqlClose>

Als u in een IODE-rapport SQL wil gaan gebruiken moet u in uw rapport een
vaste structuur gebruiken. Eerst moet de ODBC-databank toegankelijk worden
gemaakt (SqlInit), daarna moet de SQL-instructie (SqlQuery) worden
doorgegeven en de resultaten doorlopen (SqlNext) en behandeld (SqlField of
SqlRecord). Vergeet op het einde niet de ODBC-sessie af te sluiten
(SqlClose).

&TI Structuur van een SQL-rapport
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Als u in een IODE-rapport SQL wil gaan gebruiken moet u in uw rapport een
vaste structuur gebruiken. Eerst moet de ODBC-databank toegankelijk worden
gemaakt (SqlInit), daarna moet de SQL-instructie (SqlQuery) worden
doorgegeven en de resultaten doorlopen (SqlNext) en behandeld (SqlField of
SqlRecord). Vergeet op het einde niet de ODBC-sessie af te sluiten
(SqlClose).

&EN SqlOpen() : opent de ODBC-sessie

&EN SqlQuery("SQL-query") : cre‰ert een "Dynaset" met de resultaten

&EN SqlNext() : verplaatst zich in de "Dynaset"

&EN SqlField(n), .... of SqlRecord() : van de "Dynaset" om de gegevens te
recupereren

&EN SqlClose() : sluit de ODBC-sessie af

&EX2
    $goto continue, @SqlOpen(RSZ)
    $Show  ODBC-Databank not opened
    $Return

    $label continue
    $Show  ODBC-Databank opened
    ++++++++++  SAMPLE  +++++++++++++

    @SqlQuery("SELECT DISTINCT TRIM FROM RSZ_bruto;")
    @SqlNext()
    $Define BFLD @SqlField(0)

    $label again_date
    $Show Skipping @SqlField(0)
    $goto again_date, @SqlNext()
    $Define EFLD @SqlField(0)

    $Define BEGIN @replace(%BFLD%,/,Q)
    $Define END   @replace(%EFLD%,/,Q)

    $WsSample %BEGIN% %END%

    ++++++++++  RECORDS +++++++++++++

    $Define PIVOT RETOMA_DBF
    $Define PREP  RT

    $Msg  NBCOLS @SqlQuery("TRANSFORM Sum(RSZ_bruto.%PIVOT%) AS AGGR SELECT RSZ_bruto.CODRED, NaceTbl.NACE FROM NaceTbl INNER JOIN RSZ_bruto ON NaceTbl.FICTIF =RSZ_bruto.FICTIF GROUP BY RSZ_bruto.CODRED, NaceTbl.NACE PIVOT RSZ_bruto.TRIM;")
    $Show  @SqlNext()
    $label again
    $DataUpdateVar %PREP%@SqlField(0)@SqlField(1) %BEGIN% @SqlRecord(2, 8)
    $goto again, @SqlNext()

    $Show  @SqlClose()
&TX
>

<@SqlOpen>
    Fonction @SqlOpen
    ÄÄÄÄÄÄÄ
&DE
Opent een ODBC-sessie met een databank. De naam van de databank (DSN-naam)
die u doorgeeft is de naam waarmee de databank in uw ODBC-omgegeving is
gedefini‰erd (Start >> Settings >> Control Panel >> ODBC 32). Voor sommige
databanken is er een user-naam en een paswoord vereist.

Bij success krijgt u "1" als resultaat anders "0".


&FS
Deze functie heeft geen Fullscreen tegenhanger

&NFS

&SY2
    @SqlOpen(DSN-naam[,User-naam,Paswoord])
&TX

&EX2
    $goto continue, @SqlOpen(RSZ)
    $Show  ODBC-Databank not opened
    $Return

    $label continue
    $Show  ODBC-Databank opened
    ....

    @SqlClose()

&TX
>


<@SqlQuery>
    Fonction @SqlQuery
    ÄÄÄÄÄÄÄÄ
&DE
Dit commando voert een SQL-query opdracht uit op uw databank. Als die
opdracht lukt wordt er een cursor gecr‰eerd op de "Dynaset" die u kan
doorlopen met SqlNext, tot het einde van de beschikbare data. Plaats de
SQL-opdracht tussen quotes zodat er geen verwarring kan zijn als u speciale
tekens zoals "," of ")" gebruikt in uw SQL-opdracht. Groepeer ook de
volledige opdracht in ‚‚n lijn.

Bij success krijgt u het aantal kolommen van de "DynaSet" als resultaat,
anders "0".

&NO
U moet @SqlNext() gebruiken na een @SqlQuery() indien u de velden van een
record wil gebruiken.

&FS
Deze functie heeft geen Fullscreen tegenhanger

&NFS

&SY2
    @SqlQuery("SQL-opdracht")
&TX

&EX2
    $goto continue, @SqlOpen(RSZ)
    $Show  ODBC-Databank not opened
    $Return

    $label continue
    $Show  ODBC-Databank opened
    $Show  @SqlQuery("SELECT DISTINCT TRIM FROM RSZ_bruto;")
    ....

    @SqlClose()

&TX

>

<@SqlNext>
    Fonction @SqlNext
    ÄÄÄÄÄÄÄ
&DE
Dit commando doorloopt de "Dynaset" gecr‰eerd door de laatste SQL-opdracht.
Zolang er nog records beschikbaar zijn in de "Dynaset" krijgt u "1" als
resultaat anders "0".

Bij success krijgt u "1" als resultaat anders "0".

&NO
U moet @SqlNext() gebruiken na een @SqlQuery() indien u de velden van een
record wil gebruiken.

&FS
Deze functie heeft geen Fullscreen tegenhanger

&NFS

&SY2
    @SqlNext()
&TX

&EX2
    $goto continue, @SqlOpen(RSZ)
    $Show  ODBC-Databank not opened
    $Return

    $label continue
    $Show  ODBC-Databank opened
    $Show  @SqlQuery("SELECT DISTINCT TRIM FROM RSZ_bruto;")
    @SqlNext()
    $Define BFLD @SqlField(0)

    $label again_date
    $Show Skipping @SqlField(0)
    $goto again_date, @SqlNext()
    $Define EFLD @SqlField(0)

    $Define BEGIN @replace(%BFLD%,/,Q)
    $Define END   @replace(%EFLD%,/,Q)

    $WsSample %BEGIN% %END%
    ....

    @SqlClose()

&TX

>

<@SqlField>
    Fonction @SqlField
    ÄÄÄÄÄÄÄÄ
&DE
Met dit commando krijgt u de inhoud van het veld "n" van de huidige record.

Bij success krijgt u de inhoud van het veld anders "0".

&NO
U moet @SqlNext() gebruiken voor SqlField() of SqlRecord() werkt.

&FS
Deze functie heeft geen Fullscreen tegenhanger

&NFS

&SY2
    @SqlField(veld-nummer)
&TX

&EX2
    $goto continue, @SqlOpen(RSZ)
    $Show  ODBC-Databank not opened
    $Return

    $label continue
    $Show  ODBC-Databank opened
    $Show  @SqlQuery("SELECT DISTINCT TRIM FROM RSZ_bruto;")
    @SqlNext()
    $Define BFLD @SqlField(0)

    $label again_date
    $Show Skipping @SqlField(0)
    $goto again_date, @SqlNext()
    $Define EFLD @SqlField(0)

    $Define BEGIN @replace(%BFLD%,/,Q)
    $Define END   @replace(%EFLD%,/,Q)

    $WsSample %BEGIN% %END%
    ....

    @SqlClose()

&TX

>

<@SqlRecord>
    Fonction @SqlRecord
    ÄÄÄÄÄÄÄÄÄ
&DE
Met dit commando krijgt u de inhoud van de huidige record. U krijgt alle
velden als u geen argumenten aan de functie doorspeelt, of alle velden vanaf
een bepaalde kolom bij ‚‚n argument of alle velden tussen 2 kolommen als u
twee argumenten doorgeeft.

Bij success krijgt u de inhoud van de velden anders "0".

&NO
U moet @SqlNext() gebruiken voor SqlField() of SqlRecord() werkt.

&FS
Deze functie heeft geen Fullscreen tegenhanger

&NFS

&SY2
    @SqlRecord() : alle velden
    @SqlRecord(veld1-nummer) : alle velden vanaf veld1
    @SqlRecord(veld1-nummer,veld2-nummer) : alle velden tussen veld1 en veld2
&TX

&EX2
    $goto continue, @SqlOpen(RSZ)
    $Show  ODBC-Databank not opened
    $Return

    $label continue
    $Show  ODBC-Databank opened

    $Define PIVOT RETOMA_DBF
    $Define PREP  RT

    $Msg  NBCOLS @SqlQuery("TRANSFORM Sum(RSZ_bruto.%PIVOT%) AS AGGR SELECT RSZ_bruto.CODRED, NaceTbl.NACE FROM NaceTbl INNER JOIN RSZ_bruto ON NaceTbl.FICTIF =RSZ_bruto.FICTIF GROUP BY RSZ_bruto.CODRED, NaceTbl.NACE PIVOT RSZ_bruto.TRIM;")
    $Show  @SqlNext()
    $label again
    $DataUpdateVar %PREP%@SqlField(0)@SqlField(1) %BEGIN% @SqlRecord(2, 8)
    $goto again, @SqlNext()

    ...
    @SqlClose()

&TX

>


<@SqlClose>
    Fonction @SqlClose
    ÄÄÄÄÄÄÄÄ
&DE
Sluit de huidige ODBC-sessie.

Bij success krijgt u "1" als resultaat anders "0".

&FS
Deze functie heeft geen Fullscreen tegenhanger

&NFS

&SY2
    @SqlClose()
&TX

&EX2
    $goto continue, @SqlOpen(RSZ)
    $Show  ODBC-Databank not opened
    $Return

    $label continue
    $Show  ODBC-Databank opened
    ....

    @SqlClose()

&TX

>











