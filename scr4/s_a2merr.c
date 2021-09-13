#include "s_a2m.h"

/* =====================================================================
Fonction appel‚e par les fonctions A2m*() en cas d'erreur. Elle
affiche par d‚faut dans le stdout (printf) le fichier, la ligne
et le caractŠre courants, suivi du message msg.

Cette fonction peut ˆtre remplac‚e par une fonction utilisateur pour peu
que l'on garde la mˆme syntaxe.

Il suffit pour ce faire de d‚finir la fonction dans une des sources C.
Lors du link, comme la fonction apparaŒtra dans un module objet avant la
librairie, elle aura priorit‚ et remplacera donc la fonction par d‚faut
de la librairie.

La fonction de la librairie se contente d'imprimer dans le stdout le
message pass‚ comme argument.


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





