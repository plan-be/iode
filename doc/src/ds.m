/* DS */
#include "macros.h"

<Interface entre Datastream et Iode>
    Interface entre Datastream et Iode
    컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

La fonction <DSImportDB> permet d'aller chercher des s굍ies statistiques
sur le site de Datastream � travers Internet.
>

<DSImportDB>
    DSImportDB
    컴컴컴컴컴�
&DE
Cette fonction assure l'importation de donn괻s de DataStream.

&FS Cette fonction n'a pas d'interface 괹ran.

&NFS
&SY2
    $DSImportDB nom_data_stream1, ...
&TX
    Cette fonction va lire sur le sample courant la ou les variables
    d괽inies par ~cnom_data_stream1,~C ... et cr괻 dans le WS courant une
    s굍ie dont le nom est construit en rempla놹nt les caract둹es non
    alphanum굍iques par des understroke.

&EX2
    $DSImportDB BEGDP..A12
&TX
    cr괻 ~cBEGDP__A12~C dans le WS courant.

    Si le nom_data_stream contient le caract둹e pourcent (%), la fonction
    ne s'ex괹ute pas car % est un caract둹e r굎erv� dans les rapports.
    Pour pallier ce probl둴e, il faut cr괻r une liste contenant le nom des s굍ies
    et appeler DSImportDB avec la liste comme argument.
&CO
    $DSImportDB $MYLST
&TX
&NO Cette fonction n'est op굍ationnelle actuellement que sur les
    machines qui ont un acc둺 enregistr� � DataStream.
>
