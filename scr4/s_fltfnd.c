#include "scr.h"

/*NH*/
/* =+======= SCR_FIND_FLT ================================================
   Search for a filename in the list of already read files.
    filename : IN : of the file to be searched for
   ======================================================================== */

#ifdef __cplusplus
FILE_TYPE *SCR_find_flt(
char    *filename
)
#else
FILE_TYPE *SCR_find_flt(filename)
char    *filename;
#endif
{
    int     i;
    FILE_TYPE **flt_ptr;

    flt_ptr = SCR_READ_FILES.tbl_fl;

    for(i = 0 ; i < SCR_READ_FILES.tbl_nb_fl ; i++)
	if( flt_ptr[i]->fl_name != 0 && 0 == strcmp(filename, flt_ptr[i]->fl_name))
	    return(flt_ptr[i]);

    return((FILE_TYPE *)0);
}

