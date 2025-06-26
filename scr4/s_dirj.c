#include "s_strs.h"
#include "s_dir.h"

/* ====================================================================
Retourne dans res le nom absolu du fichier file du directory dir.
Cette fonction évite de placer 2 / dans le nom absolu.

&RT pointeur vers res
&EX
    char    res[SCR_STAT_NAME_LG + 1];

    SCR_dir_join(res, "c:\usr\ode\", "toto") vaut "c:\usr\ode\toto"
    SCR_dir_join(res, "c:\usr\ode", "toto")  vaut "c:\usr\ode\toto"
&TX

&SA structure SCR_dir_std()
======================================================================= */

char *SCR_dir_join(res, dir, file)
char    *res, *dir, *file;
{
    int     lg;

    strcpy(res, dir);
    lg = (int) strlen(res);
    if(!U_is_in(res[lg - 1], "\\/")) strcat(res, DIR_SEPAR);
    strcat(res, file);
    return(res);
}





