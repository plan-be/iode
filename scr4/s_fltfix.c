#include "scr.h"


SCR_fix_filetable(nb)
int     nb;
{
    FILE_TYPE   *flt;
    int         i;

    SCR_READ_FILES.tbl_fl = (FILE_TYPE **)SCR_malloc(sizeof(char *) * nb);
    SCR_READ_FILES.tbl_nb_fl = nb;
    flt = (FILE_TYPE *)SCR_malloc(sizeof(FILE_TYPE) * nb);
    for(i = 0 ; i < nb ; i++)
	SCR_READ_FILES.tbl_fl[i] = flt + i;

    return(0);
}

