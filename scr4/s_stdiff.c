#include "scr.h"


/* ====================================================================
Diff�rences de deux tables de pointeurs.

&RT
La fonction retourne un unsigned char ** avec les �l�ments de tbl1
non compris dans tbl2.


&SA SCR_free(), SCR_inter_list(), SCR_union_list(), SCR_dif_list()

======================================================================= */
unsigned char **SCR_dif(tbl1, tbl2)
unsigned char **tbl1;
unsigned char **tbl2;
{
    int i, j, nb = 0;
    unsigned char **tbl = NULL;

    if(tbl1 == 0 || tbl2 == 0) return(tbl);
    for(i = 0; tbl1[i]; i++)
    {
	for(j = 0; tbl2[j]; j++)
	    if(strcmp(tbl1[i], tbl2[j]) == 0) break;
	if(!tbl2[j])
	    if(SCR_add_ptr(&tbl, &nb, tbl1[i]) == -1) goto err;
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
Diff�rences de deux listes.

Les listes sont des "strings" contenant des �l�ments s�par�s par un
caract�re s�parateur. Les deux cha�nes sont pass�es en argument, ainsi
qu'une cha�ne contenant les caract�res s�parateurs.


&RT
La fonction retourne un pointeur de type unsigned char sur une liste allou�e
contenant les �l�ments de str1 non pr�sents dans str2.


&EX
    unsigned char *str1, *str2, *str3;
    unsigned char *string1 = "Martine Fabienne Chantal Sylvie";
    unsigned char *string2 = "Carina Lena Fabienne Martine";

    str3 = SCR_dif_tbl(string1, string2, " ,;");
    printf("Cha�ne 1 : %s\n", string1);
    printf("Cha�ne 2 : %s\n", string2);
    printf("Diff�rence: %s\n\n", str3);
    SCR_free(str3);
&TX
&SA SCR_free(), SCR_inter_tbl(), SCR_union_tbl()

======================================================================= */
unsigned char *SCR_dif_list(str1, str2, seps)
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
    tbl = SCR_dif(tbl1, tbl2);
    str = SCR_mtov(tbl, seps[0]);

    /* freeing tbl */
    SCR_free_tbl(tbl1);
    SCR_free_tbl(tbl2);
    SCR_free_tbl(tbl);

    return(str);
}







