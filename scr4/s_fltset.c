#include "scr.h"

/*NH*/
SCR_flt_set_def(flt_nb, name, ptr, type)
int     flt_nb;
char    *name;
char    *ptr;
int     type;
{
    FILE_TYPE   *flt;

    flt = SCR_READ_FILES.tbl_fl[flt_nb];
    flt->fl_name = name;
    flt->fl_ptr  = ptr;
    flt->fl_type = TYPE5(type);
    flt->fl_def  = 1;

    flt->fl_mem  = 1;       /* BP_M */
    return(0);
}

