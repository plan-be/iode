#include "iode.h"

B_ReadMe()
{
    char    *filename;

    filename = SCR_search_file("readme.ode");
    if(filename == 0) {
	B_seterrn(84);
	B_display_last_error();
	return(-1);
	}
    MMT_show_file(filename, 2, 0, SCR_PAGE_SIZE[0] - 4, SCR_PAGE_SIZE[1]);
    return(0);
}

