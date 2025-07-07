#include "scr.h"

/*NH*/
#ifdef __cplusplus
/*NH*/
FILE_TYPE *SCR_realloc_filetable(
char    *filename,
char    *ptr,
int     type
)
#else
FILE_TYPE *SCR_realloc_filetable(filename, ptr, type)
char    *filename;
char    *ptr;
int     type;
#endif

{
    FILE_TYPE   *flt;
    int         i;

    for(i = 1; i < SCR_READ_FILES.tbl_nb_fl ; i++)
	if( SCR_READ_FILES.tbl_fl[i]->fl_ptr == 0) {
	    flt = SCR_READ_FILES.tbl_fl[i];
	    goto end;
	}

    if(SCR_READ_FILES.tbl_nb_fl % 100 == 0) /* JMP 28-10-91 */
	SCR_READ_FILES.tbl_fl = (FILE_TYPE **)SCR_realloc(
			    SCR_READ_FILES.tbl_fl,
			    sizeof(FILE_TYPE *),
			    SCR_READ_FILES.tbl_nb_fl,
			    SCR_READ_FILES.tbl_nb_fl + 100);

    flt = (FILE_TYPE *)SCR_malloc(sizeof(FILE_TYPE));
    SCR_READ_FILES.tbl_fl[SCR_READ_FILES.tbl_nb_fl] = flt;
    SCR_READ_FILES.tbl_nb_fl ++;

end:
    flt->fl_name = (filename == 0) ? 0:(char *)SCR_stracpy(filename);
    flt->fl_ptr  = ptr;
    flt->fl_type = TYPE5(type);
    flt->fl_def  = 0;
    flt->fl_mem  = 1;   /* BP_M 06-04-1993 15:21 */
    flt->fl_pos  = 0;   /* BP_M 06-04-1993 15:21 */

    return(flt);
}

/*NH*/
/* =+======= SCR_ADD_FLT_PTR =============================================
   Stores in SCR_READ_FILES the name of the read file, the pointer and the type
   Allocate the space, if necessary.
    filename : IN : of the file
    ptr      : IN : pointer to the struct corresponding to this file
    type     : IN : type of the file (1 = PAGE, 2 = MENU, 3 = ISAM, 4 = MPAGE)
    return   : 0
   ======================================================================== */

#ifdef __cplusplus
/*NH*/
FILE_TYPE *SCR_add_flt_ptr(
char    *filename,
char    *ptr,
int     type
)
#else
FILE_TYPE *SCR_add_flt_ptr(filename, ptr, type)
char    *filename;
char    *ptr;
int     type;
#endif
{
    if(0 == SCR_READ_FILES.tbl_nb_fl)
	SCR_realloc_filetable("%", (char *)0, 0);

    return(SCR_realloc_filetable(filename, ptr, type));
}



