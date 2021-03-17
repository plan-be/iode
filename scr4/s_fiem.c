#include "s_filein.h"
#include "s_file.h"

int FI_extmagic(inname, cmt)
char    *inname, *cmt;
{
    int     i, type = FI_DATA;
    char    *p;

    p = strrchr(inname, '.');
    if(p != NULL) {
	for(i = 0; i < FI_next && type == FI_DATA; i++)
	    if(!strcmp(p, FI_ext[i].fe_ext)) {
		type = FI_ext[i].fe_nb;
		FI_strcat(cmt, FI_ext[i].fe_desc);
	    }
    }
    return(type);
}
