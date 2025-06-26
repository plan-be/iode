#include "s_strs.h"
#include "s_dir.h"

/* ====================================================================
Divise un nom de fichier en directory et fichier en prenant en
compte les différences entre le root directory et les autres.

&NO Cette fonction ne vérifie pas l'existence du fichier ou du directory.

&EX
    strcpy(dir, "\\usr\\ode\\toto");
    SCR_split_dir(dir, file)   : dir = "\\usr\\ode"    file = "toto"
    strcpy(dir, "c:\\toto");
    SCR_split_dir(dir, file)   : dir = "c:\\"         file = "toto"
&TX

&SA SCR_dir_std(), SCR_split_name
======================================================================= */

int SCR_split_dir(dir, file)
char    *dir, *file;
{
    int     i;
    char    locdir[1024 + 1];

    strcpy(locdir, dir);
    for(i = (int)strlen(locdir) - 1; i >= 0 ; i--)
	if(locdir[i] == '\\' || locdir[i] == '/' ) break;

    strcpy(file, locdir + i + 1);
    locdir[i + 1] = 0;
    SCR_dir_std(dir, locdir);
    return(0);
}




