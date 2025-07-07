#include "scr.h"

/*NH*/
#ifdef __cplusplus
FILE_TYPE *SCR_flt_exist(
void    *ptr,
int     type
)
#else
FILE_TYPE *SCR_flt_exist(ptr, type)
void    *ptr;
int     type;
#endif
{
    int     i;
    FILE_TYPE **flt_ptr;

    if(ptr == 0) return(0);
    flt_ptr = SCR_READ_FILES.tbl_fl;

    for(i = 0 ; i < SCR_READ_FILES.tbl_nb_fl ; i++)
	if(flt_ptr[i]->fl_ptr == ptr && flt_ptr[i]->fl_def == 1 && type == TYPE16(flt_ptr[i]->fl_type))
	    return(flt_ptr[i]);
    SCR_yyerror(ERR_ERROR, ERR_UNDEFINED, "(flt)");
    return((FILE_TYPE *)0);
}

