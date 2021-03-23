/* RTF */
#include "macros.h"

<G‚n‚ration d'un fichier d'aide Windows>
    G‚n‚ration d'un fichier d'aide Windows
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

IODE permet de g‚n‚rer des outputs sous diff‚rents formats : fichiers
RTF pour Winword, MIF pour Frame, HTML pour les browser Internet,
CSV pour Excel. D'autres formats pourront encore s'ajouter dans les
versions futures.

Le format RTF (Rich Text Format) est particulier en ce sens qu'il a un
double usage: en plus des nombreux programmes de traitement de texte qui
sont capables d'interpr‚ter ce format, le programme d'aide de Windows,
~cWinhelp.exe~C (ou ~cWinHlp32.exe~C), peut, moyennant une compilation
particuliŠre, exploiter ce langage.

Il est donc possible de construire un fichier d'aide au format Windows,
contenant par exemple des tableaux et des ‚quations de IODE. Fichier
ais‚ment distribuable sur toute machine dot‚e de Windows 9x ou de
Windows NT.

La m‚thode … suivre pour construire et distribuer des fichiers d'aide
Windows consiste en trois ‚tapes:

&EN la <cr‚ation d'un fichier hi‚rarchique RTF-Help>
&EN la <compilation des fichiers RTF-Help>
&EN la <distribution des fichiers Help Windows>

Pour conclure, un <exemple de cr‚ation d'un fichier d'aide> Windows est
pr‚sent‚. Cet exemple imprime toute la d‚finition d'un projet :
‚quations, variables, etc.
>


<Cr‚ation d'un fichier hi‚rarchique RTF-Help>
    Cr‚ation d'un fichier hi‚rarchique RTF-Help
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Toute impression de IODE dans un fichier a2m ou directement dans un
fichier RTF peut ˆtre la source d'un fichier d'aide Windows.
Cela peut se faire aussi bien … partir des points du Menu Print que d'un
Rapport.

Pour obtenir un fichier Rtf pour Aide de Windows, il faut simplement
cocher la case HCW dans le panneau RTF des "Print Preferences" et
choisir comme destination des impressions un fichier RTF (Print Setup).

Cependant, il est pr‚f‚rable pour la lisibilit‚ du fichier d'aide de
d‚composer son fichier en sujets et sous-sujets.

Certaines impressions de IODE g‚nŠrent automatiquement des sous-sujets :
c'est le cas des impressions:

&EN de tableaux
&EN de d‚finitions de tableaux
&EN de d‚finitions d'‚quations avec r‚sultats d'estimation

Dans tous les autres cas, l'impression a lieu dans un sujet.

Pour ajouter une hi‚rarchie … un document, il faudra, soit directement
en langage A2M, soit … travers les commandes de rapport ~c$PrintRtfTopic~C
et ~c$PrintRtfLevel~C, indiquer trois informations :

&EN o— la d‚coupe doit se faire
&EN le titre du topic
&EN le niveau du topic dans la hi‚rarchie

Par exemple, si on r‚alise un fichier d'aide constitu‚ de tableaux on
pourra indiquer :

&CO
    $PrintRtfLevel 0

    $PrintRtfTopic Principales hypothŠses de la projection
    $PrintRtfLevel +
    $PrintNbDec 1
    $PrintTbl %NIV0% HYPE
    $PrintTbl %NIV0% HYPFP
    $PrintTbl %NIV0% HYPSZ DSB

    $PrintRtfLevel -
    $PrintRtfTopic R‚sultats Macro‚conomiques
    $PrintRtfLevel +
    $PrintNbDec 2
    $PrintTbl %GRT0% RESL00R
    $PrintNbDec 1
    $PrintTbl %NIV0% RESL03
&TX

~c$PrintRtfLevel~C permet de d‚terminer le niveau hi‚rarchiques des sujets
suivants :

&EN "Principales hypothŠses ..." est au niveau 0 (sup‚rieur)
&EN Chaque tableau imprim‚ est … un niveau 1 (~c$PrintRtfLevel~C +)
&EN On revient ensuite au niveau 0 (~c$PrintRtfLevel~C -)

Le titre des topics est constitu‚ du paramŠtre de la fonction
~c$PrintRtfTopic~C.
>

<Compilation des fichiers RTF-Help>
    Compilation des fichiers RTF-Help
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Lorsqu'un fichier RTF-Help est construit … l'aide de IODE ou du programme
~ca2m.exe~C, il faut encore le compiler pour obtenir un fichier
exploitable par le programme ~cWinhelp.exe~C.

En fait, trois fichiers sont g‚n‚r‚s par IODE ou A2M:

&EN ~cfilename.rtf~C : texte de l'aide
&EN ~cfilename.hpj~C : fichier de d‚finition du projet d'aide
&EN ~cfilename.cnt~C : fichier contenant la table de matiŠres

La compilation n‚cessite l'installation du programme ~cHCW.EXE~C,
distribu‚ avec la plupart des outils de programmation de Microsoft ou de
Borland.

La commande de compilation est :

&CO
    hcw /C/A nom_du_fichier[.hpj]
&TX

Comme r‚sultat, on obtient le fichier ~cfilename.hlp~C.
>


<Distribution des fichiers Help Windows>
    Distribution des fichiers Help Windows
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Pour fournir une information complŠte, deux fichiers doivent ˆtre
distribu‚s:

&EN ~cfilename.cnt~C : fichier contenant la table de matiŠres
&EN ~cfilename.hlp~C : fichier contenant le texte de l'aide

On peut regrouper ces deux fichiers dans une archive par exemple
… l'aide de la commande ~cpak.exe~C ou ~cpkzip.exe~C.

&CO
    pak a /EXE filename.exe filename.hlp filename.cnt
    pkzip -a filename.zip filename.hlp filename.cnt
&TX

>

<Exemple de cr‚ation d'un fichier d'aide>
    Exemple de cr‚ation d'un fichier d'aide Windows
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

L'exemple suivant imprime la d‚finition de tous les objets pr‚sentes
dans les WS courants.

&CO
    $PrintDest test.rtf RTF
    $PrintRtfLevel 0
    $PrintRtfTitle ModŠle MODTRIM
    $PrintRtfCopyright Bureau f‚d‚ral du Plan (c) 1998
    $PrintFont Helvetica 9 1
    $PrintTableFont Times 8 2

    $PrintObjInfos 2
    $PrintRtfTopic Equations
    $PrintRtfLevel ++
    $PrintObjDefEqs
    $PrintRtfLevel --

    $PrintRtfTopic Comments
    $PrintRtfLevel ++
    $PrintObjDefCmt
    $PrintRtfLevel --

    $PrintRtfTopic Identities
    $PrintRtfLevel ++
    $PrintObjDefIdt
    $PrintRtfLevel --

    $PrintRtfTopic Lists
    $PrintRtfLevel ++
    $PrintObjDefLst
    $PrintRtfLevel --

    $PrintRtfTopic Scalars
    $PrintRtfLevel ++
    $PrintObjDefScl
    $PrintRtfLevel --

    $PrintRtfTopic Tables
    $PrintRtfLevel ++
    $PrintObjDefTbl
    $PrintRtfLevel --

    $PrintRtfTopic Variables
    $PrintRtfLevel ++
    $PrintObjDefVar
    $PrintRtfLevel --

    $PrintRtfLevel 0
&TX

Il reste … traduire le fichier ~ctest.rtf~C en fichier .hlp :

&CO
    HCW /C/A test
&TX
et enfin … le tester :
&CO
    Winhelp test
&TX

Pour le distribuer, on fera par exemple :

&CO
    pak a /EXE test.exe test.hlp test.cnt
&TX
>




