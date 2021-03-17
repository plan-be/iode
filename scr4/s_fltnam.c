#include "scr.h"

/* ======================================================================
Retourne le nom d'un objet dont le pointeur est pass‚ comme argument.
Ce nom contient l'extension de langage (.f par d‚faut) sans dans le cas des
ISAM.
&EX
    char    name[80];

    sprintf(name, "Nom de l'objet : %s", SCR_find_ptr_name(pg_client));

&TX
&RT le num‚ro de l'objet ou -1 si le pointeur n'est pas trouv‚
&SA SCR_find_ptr_nu()
------------------------------------------------------------------------- */
char *SCR_find_ptr_name(ptr)
void *ptr;
{
    int     i;

    i = SCR_find_ptr_nu(ptr);  /* JMP 23-06-91 */
    if(i < 0) return((char *)0);
    return((SCR_READ_FILES.tbl_fl)[i]->fl_name);
}

