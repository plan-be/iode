#include "scr.h"

/* ======================================================================
Retourne le numéro d'un objet dont le pointeur est passé comme argument.
&EX
    int     act_nb;

    act_nb = SCR_find_ptr_nu(editclt);

&TX
&RT le numéro de l'objet ou -1 si le pointeur n'est pas trouvé
&SA SCR_find_ptr_name()
------------------------------------------------------------------------- */

SCR_find_ptr_nu(ptr)
void    *ptr;
{
    int     i;
    FILE_TYPE **flt;  /* BP_M 07-04-1993 12:21 */

    flt = SCR_READ_FILES.tbl_fl;

    for(i = 0 ; i < SCR_READ_FILES.tbl_nb_fl ; i++)
	if(flt[i]->fl_mem == 1 && ptr == flt[i]->fl_ptr)
	    return(i);

    return(-1);
}
