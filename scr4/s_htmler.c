#include "scr4.h"
#include "s_html.h"

/*==================================================================
Fonction appel‚e pour afficher les messages HTML provenant des fonctions
HtmlToc(), HtmlReplace() et HtmlSplit().

Cette fonction peut ˆtre remplac‚e par une fonction utilisateur.

Par d‚faut, la fonction de librairie vaut simplement:

&CO
    SCR_comment(msg);
&TX

&SA HtmlToc(), HtmlReplace() et HtmlSplit()
=============================================================== */

int HtmlMessage(msg)
char    *msg;
{
    SCR_comment(msg);
    return(0);
}


