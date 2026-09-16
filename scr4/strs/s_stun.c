#include "scr.h"

/*NH*/
int SCR_add_tbl(tbl, nb, str)                  /* JMP 07-02-97 */
unsigned char ***tbl, *str;
int           *nb;
{
    int     j;


    for(j = 0; j < *nb; j++)
		if(strcmp((*tbl)[j], str) == 0) break;
    if(j == *nb) return(SCR_add_ptr(tbl, nb, str));
    return(0);
}


/* ====================================================================
Réunion de deux tableaux de ptrs vers des char **. Les éléments
communs ne sont repris qu'une seule fois dans le tableau résultat.

&RT
La fonction retourne un unsigned char **.

&SA SCR_free(), SCR_dif_list(), SCR_inter_list(), SCR_union_list()

======================================================================= */
unsigned char **SCR_union(tbl1, tbl2)
unsigned char **tbl1;
unsigned char **tbl2;
{
    int i, nb = 0;
    unsigned char **tbl = NULL;

    if(tbl1 == 0 || tbl2 == 0) return(tbl);
    for(i = 0; tbl1[i]; i++)
		if(SCR_add_tbl(&tbl, &nb, tbl1[i]) == -1) goto err;

    for(i = 0; tbl2[i]; i++)
		if(SCR_add_tbl(&tbl, &nb, tbl2[i]) == -1) goto err;

    if(SCR_add_ptr(&tbl, &nb, NULL) == -1) goto err;
    return(tbl);

err:
    if(nb == 0) return(tbl);
    tbl[nb - 1] = NULL;
    SCR_free_tbl(tbl);
    return(NULL);
}

/* ====================================================================
Réunion de deux tableaux de ptrs vers des char **. Si des éléments
sont communs aux deux listes, ils sont repris tous les deux.

&RT
La fonction retourne un unsigned char **.

&SA SCR_free(), SCR_dif_list(), SCR_inter_list(), SCR_union_list()
======================================================================= */
unsigned char **SCR_union_quick(tbl1, tbl2)
unsigned char **tbl1;
unsigned char **tbl2;
{
	int i, nb1, nb2;
    unsigned char **tbl = NULL;

	nb1 = SCR_tbl_size(tbl1);
	nb2 = SCR_tbl_size(tbl2);
	
	// ALloc res tbl
	tbl = (unsigned char **) SCR_malloc(sizeof(char *) * (nb1 + nb2 + 1));
	if(tbl == 0) return(tbl);
	
	if(nb1 > 0)  {
		for(i = 0; tbl1[i]; i++) 
			tbl[i] = SCR_stracpy(tbl1[i]);
	}		

    if(nb2 > 0)  {
		for(i = 0; tbl2[i]; i++)
			tbl[nb1 + i] = SCR_stracpy(tbl2[i]);
	}
	
	return(tbl);
}



/* ====================================================================
Réunion de deux listes.

Les listes sont des "strings" contenant des éléments séparés par un
caractère séparateur. Les deux chaînes sont passées en argument, ainsi
qu'une chaîne contenant les caractères séparateurs.


&RT
La fonction retourne un pointeur de type unsigned char sur une liste allouée
contenant la réunion des éléments des deux listes passées en arguments.


&EX
    unsigned char *str1, *str2, *str3;
    unsigned char *string1 = "Martine Fabienne Chantal Sylvie";
    unsigned char *string2 = "Carina Lena Fabienne Martine";

    str2 = SCR_union_tbl(string1, string2, " ,;");
    printf("Chaîne 1 : %s\n", string1);
    printf("Chaîne 2 : %s\n", string2);
    printf("Réunion  : %s\n\n", str2);
    SCR_free(str2);
&TX
&SA SCR_free(), SCR_dif_tbl(), SCR_inter_tbl()

======================================================================= */
unsigned char *SCR_union_list(str1, str2, seps)
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
    tbl = SCR_union(tbl1, tbl2);
    str = SCR_mtov(tbl, seps[0]);

    /* freeing tbl */
    SCR_free_tbl(tbl1);
    SCR_free_tbl(tbl2);
    SCR_free_tbl(tbl);

    return(str);
}

