#include "scr.h"

/* ======================================================================
Cette fonction modifie le nom du device ou du fichier associé à un
PRINTER. Le nom est strippé par la fonction et remplace l'ancien nom.

&NO Le nom ne peut dépasser la zone associée au nom dans la définition
    initiale du PRINTER (FILE "tmp" ne suffit pas en général car
    il n'y a que trois caractères disponibles).
    Cette fonction a été modifiée le 05-01-96 pour éviter certains
    dépassements de buffer.

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
