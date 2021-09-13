#include "scr.h"

/* ======================================================================
Cette fonction modifie le nom du device ou du fichier associ‚ … un
PRINTER. Le nom est stripp‚ par la fonction et remplace l'ancien nom.

&NO Le nom ne peut d‚passer la zone associ‚e au nom dans la d‚finition
    initiale du PRINTER (FILE "tmp" ne suffit pas en g‚n‚ral car
    il n'y a que trois caractŠres disponibles).
    Cette fonction a ‚t‚ modifi‚e le 05-01-96 pour ‚viter certains
    d‚passements de buffer.

&EX
     PR_set_name(my_pr, pg_config_PRNAME);
     PR_select(my_pr);
     ...
     PR_send();
&TX
&SA PR_select(), PR_set_size(), PR_set_device()
------------------------------------------------------------------------ */
PR_set_name(pr, filename)
PR_DEF  *pr;
char    *filename;
{
    char    buf[128];                   /* JMP 4.23 05-01-96 */

    strcpy(buf, filename);              /* JMP 4.23 05-01-96 */
    SCR_strip(buf);                     /* JMP 4.23 05-01-96 */
    strcpy(pr->pr_name, buf);           /* JMP 4.23 05-01-96 */
    return(0);
}
