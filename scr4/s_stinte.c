#include "scr.h"


/* ====================================================================
Intersection de deux tableaux de pointeurs.

&RT
La fonction retourne un unsigned char ** avec les ‚l‚ments communs
aux deux tableaux tbl1 et tbl2.


&SA SCR_free(), SCR_dif_tbl(), SCR_union(), SCR_inter_list()

======================================================================= */
unsigned char **SCR_inter(tbl1, tbl2)
unsigned char *tbl1[];
unsigned char *tbl2[];
{
    int i, j, nb = 0;
    unsigned char **tbl = NULL;

    if(tbl1 == 0 || tbl2 == 0) return(tbl);
    for(i = 0; tbl1[i]; i++)
	for(j = 0; tbl2[j]; j++)
	    if(strcmp(tbl1[i], tbl2[j]) == 0)
	    {
		if(SCR_add_tbl(&tbl, &nb, tbl1[i]) == -1) goto err; /* JMP 14-08-98 */
		break;
	    }
    if(SCR_add_ptr(&tbl, &nb, NULL) == -1) goto err;
    return(tbl);
err:
    if(nb == 0) return(tbl);
    tbl[nb - 1] = NULL;
    SCR_free_tbl(tbl);
    return(NULL);
}


/* ====================================================================
Intersection de deux listes.

Les listes sont des "strings" contenant des ‚l‚ments s‚par‚s par un
caractŠre s‚parateur. Les deux chaŒnes sont pass‚es en argument, ainsi
qu'une chaŒne contenant les caractŠres s‚parateurs.


&RT
La fonction retourne un pointeur de type unsigned char sur une liste allou‚e
contenant la partie commune des deux listes pass‚es en arguments.


&EX
    unsigned char *str1, *str2, *str3;
    unsigned char *string1 = "Martine Fabienne Chantal Sylvie";
    unsigned char *string2 = "Carina Lena Fabienne Martine";

    str1 = SCR_inter_tbl(string1, string2, " ,;");
    printf("ChaŒne 1 : %s\n", string1);
    printf("ChaŒne 2 : %s\n", string2);
    printf("Intersection : %s\n\n", str1);
    SCR_free(str1);
&TX
&SA SCR_free(), SCR_dif_tbl(), SCR_union_tbl()

======================================================================= */
unsigned char *SCR_inter_list(str1, str2, seps)
unsigned char *str1;
unsigned char *str2;
unsigned char *seps;
{
    unsigned char **tbl1, **tbl2;
    unsigned char **tbl;
    unsigned char *str;

    /* string -> tbl using separators */
    tbl1 = SCR_vtoms(str1, seps);
    tbl2 = SCR_vtoms(str2, seps);

    /* looking for intersection */
    tbl = SCR_inter(tbl1, tbl2);
    str = SCR_mtov(tbl, seps[0]);

    /* freeing tbl */
    SCR_free_tbl(tbl1);
    SCR_free_tbl(tbl2);
    SCR_free_tbl(tbl);

    return(str);
}
