#include "s_strs.h"

/* ====================================================================
Standardise dans res le nom de directory dir en supprimant le / ou le \ qui
se trouve à la fin sauf dans le cas du root directory (/ ou a:\).
De plus, cette fonction supprime le blancs en début et fin du nom du directory.

dir et res peuvent être confondus. En DOS, le nom est mis en minuscules.

&EX
    SCR_dir_std(res, "  C:\  ") : res vaut "c:\"
    SCR_dir_std(res, "\usr\  ")   : res vaut "\usr"
&TX

&SA SCR_split_name(), SCR_split_dir()
======================================================================= */
int SCR_dir_std(res, dir)
char    *res, *dir;
{
    int     lg;

    strcpy(res, dir);
    /*SCR_sqz(res);     /* JMP 21-04-98 */
    U_ljust_text(res);  /* JMP 21-04-98 */
    SCR_strip(res);     /* JMP 21-04-98 */
#if defined(DOS) && !defined(DOSW32) && !defined(SCRW32)
    SCR_lower(res);
#endif
    lg = (int)strlen(res);
    if(lg != 1 && U_is_in(res[lg - 1], "/\\") &&    /* \ */
       !(lg == 3 && res[1] == ':'))                 /* a:\ */
	res[lg - 1] = 0;

    return(0);
}



