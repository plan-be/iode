#include "scr.h"

/*==================================================================
Fonction appel‚e en cas d'erreur de PR_*() lors de l'ouverture du
fichier ou de l'imprimante.

Cette fonction peut ˆtre replac‚e par une fonction utilisateur,
permettant par exemple d'afficher le message dans une fenˆtre.

La fonction de librairie effectue les op‚rations suivantes :

&CO
    PG_display_error("%s %s", SCR_err_txt(0), pr->pr_name);
&TX
&SA PR_select()
=============================================================== */

PR_error(pr)
PR_DEF  *pr;
{
    PG_display_error("%s %s", SCR_err_txt(0), pr->pr_name);
    return(0);
}
