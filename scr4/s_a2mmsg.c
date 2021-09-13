#include "scr4.h"
#include "s_a2m.h"

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

A2mMessage(msg)
char    *msg;
{
    SCR_comment(msg);
    return(0);
}





