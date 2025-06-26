#include "scr4.h"
#include "s_html.h"

/*==================================================================
Fonction appelée pour afficher les messages HTML provenant des fonctions
HtmlToc(), HtmlReplace() et HtmlSplit().

Cette fonction peut être remplacée par une fonction utilisateur.

Par défaut, la fonction de librairie vaut simplement:

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


