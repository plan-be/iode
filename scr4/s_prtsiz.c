#include "scr.h"

/* ======================================================================
Cette fonction modifie la taille de la page d'impression d'un
PRINTER. Cette fonction ne peut être utilisée en cours d'utilisation
du PRINTER!

&EX
     PR_set_name(my_pr, pg_config_PRNAME);
     PR_set_size(my_pr, 70, 90);
     PR_select(my_pr);
     ...
     PR_send();
&TX
&SA PR_select(), PR_set_name()
------------------------------------------------------------------------ */
PR_set_size(pr, nl, nc)
PR_DEF  *pr;
int     nl, nc;
{
    pr->pr_nl = nl;
    pr->pr_nc = nc;
    return(0);
}

