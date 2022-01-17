#include "scr4.h"
#include "s_a2m.h"


void (*A2mMessage_super)(char*); // if not null, replaces the default implementation A2mMessage()

/*==================================================================
Fonction appel‚e pour afficher les messages en cours d'impression des
fichiers A2M provenant des fonctions A2mToMif(), etc.

Cette fonction peut ˆtre remplac‚e par une fonction utilisateur.

Par d‚faut, la fonction de librairie vaut simplement:

&CO
    SCR_comment(msg);
&TX

&SA A2mToMif(), A2mToRtf(), A2mToGdi(), A2mToHtml()
=============================================================== */

void A2mMessage(char *msg)
{
    if(A2mMessage_super)
        (*A2mMessage_super)(msg);
    else
        printf("\r%s", msg);
    
    // SCR_comment(msg);
}





