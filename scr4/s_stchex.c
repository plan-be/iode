#include "s_strs.h"

/* ====================================================================
Remplace l'extension d'un nom de fichier. Le r‚sultat est sauv‚ dans une
variable pass‚e par l'utilisateur. L'extension est toujours ajout‚e, que
le nom de fichier initial possŠde ou non une extension.

&RT pointeur vers res.
&EX
    char    res[80];

    SCR_change_ext(res, "test", "dat");
    printf("'%s'\n", res);
    SCR_change_ext(res, "test.idx", "dat");
    printf("'%s'\n", res);
    SCR_change_ext(res, "test.", "dat");
    printf("'%s'\n", res);
    SCR_change_ext(res, "test.dat", "");
    printf("'%s'\n", res);

    r‚sultat :

	'test.dat'
	'test.dat'
	'test.dat'
	'test'
&TX
&SA SCR_cat_ext()
=======================================================================*/

char *SCR_change_ext(res, filename, ext)
char    *res, *filename, *ext;
{
    int     i;

    strcpy(res, filename);
    for(i = strlen(res) - 1 ; i >= 0 ; i--)
	if(res[i] == ':' || res[i] == '!' ||        /* JMP 23-06-02 */
	   res[i] == '.' || res[i] == '\\' ||
	   res[i] == '/') break;
    if(i >= 0 && res[i] == '.') res[i] = 0;
    if(ext != 0 && ext[0] != 0) {
	strcat(res, ".");
	strcat(res, ext);
	}
    return(res);
}



