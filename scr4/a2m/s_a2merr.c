#include "s_a2m.h"

/* =====================================================================
Fonction appelée par les fonctions A2m*() en cas d'erreur. Elle
affiche par défaut dans le stdout (printf) le fichier, la ligne
et le caractère courants, suivi du message msg.

Cette fonction peut être remplacée par une fonction utilisateur pour peu
que l'on garde la même syntaxe.

Il suffit pour ce faire de définir la fonction dans une des sources C.
Lors du link, comme la fonction apparaîtra dans un module objet avant la
librairie, elle aura priorité et remplacera donc la fonction par défaut
de la librairie.

La fonction de la librairie se contente d'imprimer dans le stdout le
message passé comme argument.


&EN af  = descripteur du fichier a2m ouvert
&EN msg = message d'erreur

&EX
    #include <scr4.h>
    #include <s_a2m.h>

    A2mPrintError(af, txt)
    A2MFILE *af;
    U_ch    *txt;
    {
	PG_display_error("%s : %s", A2mError(af), txt);
    }
&TX
&SA A2mToGdi(), A2mToHtml(), A2mToRtf(), A2mToMif(), A2mError()
======================================================================== */

int A2mPrintError(af, msg)
A2MFILE *af;
U_ch    *msg;
{
    printf("%s: %s\n", A2mError(af), msg);
    return(0);
}





