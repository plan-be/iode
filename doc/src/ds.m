/* DS */
#include "macros.h"

<Interface entre Datastream et Iode>
    Interface entre Datastream et Iode
    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

La fonction <DSImportDB> permet d'aller chercher des s‚ries statistiques
sur le site de Datastream … travers Internet.
>

<DSImportDB>
    DSImportDB
    ÄÄÄÄÄÄÄÄÄÄÄ
&DE
Cette fonction assure l'importation de donn‚es de DataStream.

&FS Cette fonction n'a pas d'interface ‚cran.

&NFS
&SY2
    $DSImportDB nom_data_stream1, ...
&TX
    Cette fonction va lire sur le sample courant la ou les variables
    d‚finies par ~cnom_data_stream1,~C ... et cr‚e dans le WS courant une
    s‚rie dont le nom est construit en rempla‡ant les caractŠres non
    alphanum‚riques par des understroke.

&EX2
    $DSImportDB BEGDP..A12
&TX
    cr‚e ~cBEGDP__A12~C dans le WS courant.

    Si le nom_data_stream contient le caractŠre pourcent (%), la fonction
    ne s'ex‚cute pas car % est un caractŠre r‚serv‚ dans les rapports.
    Pour pallier ce problŠme, il faut cr‚er une liste contenant le nom des s‚ries
    et appeler DSImportDB avec la liste comme argument.
&CO
    $DSImportDB $MYLST
&TX
&NO Cette fonction n'est op‚rationnelle actuellement que sur les
    machines qui ont un accŠs enregistr‚ … DataStream.
>
