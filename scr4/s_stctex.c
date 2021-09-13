#include "s_strs.h"

/* ====================================================================
Remplace l'extension d'un nom de fichier si le nom n'en possŠde pas. Le
r‚sultat est sauv‚ dans une variable pass‚e par l'utilisateur.

&RT pointeur vers res.
&EX
    char    res[80];

    SCR_cat_ext(res, "test", "dat");
    printf("'%s'\n", res);
    SCR_cat_ext(res, "test.idx", "dat");
    printf("'%s'\n", res);
    SCR_cat_ext(res, "test.", "dat");
    printf("'%s'\n", res);

    r‚sultat :

	'test.dat'
	'test.idx'
	'test.'
&TX
&SA SCR_change_ext()
=======================================================================*/

char *SCR_cat_ext(res, filename, ext)
char    *res, *filename, *ext;
{
    int     i;

    strcpy(res, filename);
/*
    for(i = 0 ; res[i] != 0 ; i++)
	if(res[i] == '.') break;

    if(res[i] == '.') return(res);
/* JMP 04-02-96 */
    for(i = strlen(res) - 1 ; i >= 0 ; i--)                         /* JMP 04-02-96 */
	if(res[i] == '.' || res[i] == '\\' || res[i] == '/') break; /* JMP 04-02-96 */
    if(i >= 0 && res[i] == '.') return(res);                        /* JMP 04-02-96 */

    return(SCR_change_ext(res, filename, ext));
}
