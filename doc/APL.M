
/* APL */
#include "macros.h"

<Interface entre l'Apl et Iode>
    Interface entre l'Apl et Iode : le WS iode.w3
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

iode.w3 is een workspace met functies in de APL programmeertaal. Deze
functies laten toe om objecten uit Iode in de workspace te kopi‰ren, met APL
instructies deze gegevens aan te passen en terug in de Iode omgeving te
brengen.

Het is ook mogelijk om vanuit APL Iode rapportcommando's uit te voeren in de
Iode omgeving. De gegevens worden via het DDE protocol met elkaar
uitgewisseld. Iode en APL moeten opgestart zijn en in APL moet de iode.w3
workspace geladen zijn.

&EN <IodeList>      geeft de lijst van de IODE geladen objecten
&EN <IodeCopy>      kopi‰ert IODE-objecten in APL vectoren
&EN <IodeUpdate>    update IODE-objecten met waarden uit APL vectoren
&EN <IodeRep>       laat IODE een rapport uitvoeren

&EN <Demo><APLDemo>

.~a'apl.gif'
>


<IodeList>
    IodeList
    ÄÄÄÄÄÄÄÄ
&EXTWS

&DE
Geeft een lijst van de objecten van het bepaalde type in de Iode omgeving.
type is C,  E,  I,  L,  S,  T of V


&SY2
    IodeList 'type'
&TX

&EX2
    VARLIST <<- IodeList 'V'
&TX
>

<IodeCopy>
    IodeCopy
    ÄÄÄÄÄÄÄÄ

&EXTWS

&DE
Kopieert de objecten, van het bepaalde type, die genoemd zijn in de lijst
uit Iode naar APL.

&SY2
'type' IodeCopy 'lijst'
&TX
&EX
    'V' IodeCopy 'ZKT VAR2 VAR3'
&TX
>

<IodeUpdate>
    IodeUpdate
    ÄÄÄÄÄÄÄÄÄÄ

&EXTWS

&DE
Objecten van het bepaalde type en die genoemd zijn in de lijst worden in de
Iode omgeving vervangen door de waarde die ze in APL hebben.

&SY2
    'type' IodeUpdate 'lijst'
&TX

&EX2
    'V' IodeUpdate 'ZKT VAR2 VAR3'
&TX
>


<IodeRep>
    IodeRep
    ÄÄÄÄÄÄÄ
&DE
Deze APL functie voert in de Iode omgeving een rapportcommando uit.

&SY2
IodeRep 'rapportcommando'
&TX

&EX2
    IodeRep '$WsSaveVar snacap90'
&TX
>


<APLDemo>
    APLDemo
    ÄÄÄÄÄÄÄ
&EX2
    IodeRep '$WsLoadVar c:\usr\ode\nov.var'
    'V' IodeCopy 'ZZ ZJ'
    ZZ<<- ZZ+ZJ
    'V' IodeUpdate 'ZZ'
    IodeRep 'WsLoadVar c:\usr\ode\dec.var'
&TX

Deze APL-sessie, zorgt ervoor dat IODE de nov.var variable file laadt. De
variabele ZZ en ZJ wordt uit IODE gehaald, opgeteld en als nieuwe waarde
voor ZZ in IODE geladen. De laatste lijn schrijft de aangpaste DB weg in de
nieuwe dec.var variable file.
>

