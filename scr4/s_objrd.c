/*NH*/
/* ==== Functions used when application is not read from ASCII
	definition
   ============================================================= */

#include "scr.h"

/*NH*/
/* =+======= SCR_add_ext ==============================================
   Add extension to filename
     filename : IN : name as given as parameter
     return : pointer (static) to the completed filename
   ==================================================================== */

char *SCR_add_ext(filename)
char    *filename;
{
    static char name[SCR_MAX_FILE_LG + 1];

    strcpy(name, filename);
    if(U_is_in('.', filename) == 0)  {
	/* not '.' in filename and ext required ==> add SCR_LANGUAGE */
	strcat(name, ".");
	strcat(name, SCR_LANGUAGE);
	}
    return(name);
}


/*NH*/
char *OBJ_read(name, type)
char    *name;
int     type;
{
    int     rtype;
    FILE_TYPE *ptr;

    if(type != SCR_TYPE_ISAM)
      name = SCR_add_ext(name);
    ptr = SCR_find_flt(name);
    if(ptr == 0)
	return((char *)0);
    else
	return(ptr->fl_ptr);
}

