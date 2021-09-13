#include "s_strs.h"
#include "s_dir.h"

/* ====================================================================
Divise un nom de fichier en directory et fichier en prenant en
compte les diff‚rences entre le root directory et les autres.

&NO Cette fonction ne v‚rifie pas l'existence du fichier ou du directory.

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

    for(i = strlen(dir) - 1; i >= 0 ; i--)
	if(dir[i] == '\\' || dir[i] == '/' ) break;

    strcpy(file, dir + i + 1);
    dir[i + 1] = 0;
    SCR_dir_std(dir, dir);
    return(0);
}




