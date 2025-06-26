#include "s_strs.h"

#define LINE_BLOCK 20
int SCR_ADD_PTR_CHUNCK = 1; /* JMP 13-11-2012 */

/* ====================================================================
Ajoute un pointeur dans une table tbl de ntbl pointeurs en réallouant
cette table et alloue l'espace requis pour le stockage du string txt
(non nécessairement terminé par un zéro) de longueur lg additionné
de n bytes nuls.

Si lg ou txt sont nuls, ajoute un pointeur NULL en fin de table.

Les valeurs de tbl et ntbl sont modifiées par le programme.

Le tableau ainsi alloué peut être libéré par SCR_free_tbl().

&RT -1 en cas d'erreur (Memroy full). 0 sinon.
&EX

    U_ch  **ReadFile(filename)
    char    *filename;
    {
	char    **tbl = NULL;
	int     ntbl = 0;
	FILE    *fd;
	long    vec[20];

	fd = fopen(filename, "r");
	if(fd == 0) return(-1);

	while(!feof(fd)) {
	    fread(vec, 20, sizeof(long), fd);
	    SCR_add_ptr_lg(&tbl, &ntbl, vec, 20 * sizeof(long));
	    }

	fclose(fd);
	SCR_add_ptr(&tbl, &ntbl, NULL);
	return(tbl);
    }
&TX
&SA SCR_free_tbl(), SCR_add_ptr(), SCR_tbl_size(), SCR_add_ptr_lg()
=======================================================================*/

SCR_add_ptr_lg_0(tbl, ntbl, txt, lg, z)
U_ch    ***tbl;
int     *ntbl, lg, z;
U_ch    *txt;
{
    /*    char    *SCR_malloc(), *SCR_realloc(); /* JMP 04-01-98 */
    U_ch    **tmp;

    if(SCR_ADD_PTR_CHUNCK == 0) SCR_ADD_PTR_CHUNCK = 1;

    if((*ntbl) % SCR_ADD_PTR_CHUNCK == 0) {
        //tmp = (U_ch  **) SCR_realloc(*tbl, sizeof(char *), *ntbl, (*ntbl) + 1);
        tmp = (U_ch  **) SCR_realloc(*tbl, sizeof(char *), *ntbl, (*ntbl) + SCR_ADD_PTR_CHUNCK); /* JMP 13-11-2012 */
        if(tmp == 0) return(-1);
        *tbl = tmp;
    }
    else {
        tmp = *tbl;
    }

    if(txt != 0 && lg > 0) {
        tmp[*ntbl] = (U_ch  *) SCR_malloc(lg + z); /* JMP 07-06-96 */
        if(tmp[*ntbl] == 0) return(-1);
        memcpy(tmp[*ntbl], txt, lg);
    }
    (*ntbl)++;
    return(0);
}

/* ====================================================================
Ajoute un pointeur dans une table tbl de ntbl pointeurs en réallouant
cette table et alloue l'espace requis pour le stockage du string txt
(non nécessairement terminé par un zéro) de longueur lg.

Si lg ou txt sont nuls, ajoute un pointeur NULL en fin de table.

Les valeurs de tbl et ntbl sont modifiées par le programme.

Le tableau ainsi alloué peut être libéré par SCR_free_tbl().

&RT -1 en cas d'erreur (Memroy full). 0 sinon.
&EX

    U_ch  **ReadFile(filename)
    char    *filename;
    {
	char    **tbl = NULL;
	int     ntbl = 0;
	FILE    *fd;
	long    vec[20];

	fd = fopen(filename, "r");
	if(fd == 0) return(-1);

	while(!feof(fd)) {
	    fread(vec, 20, sizeof(long), fd);
	    SCR_add_ptr_lg(&tbl, &ntbl, vec, 20 * sizeof(long));
	    }

	fclose(fd);
	SCR_add_ptr(&tbl, &ntbl, NULL);
	return(tbl);
    }
&TX
&SA SCR_free_tbl(), SCR_add_ptr(), SCR_tbl_size()
=======================================================================*/

SCR_add_ptr_lg(tbl, ntbl, txt, lg)
U_ch    ***tbl;
int     *ntbl, lg;
U_ch    *txt;
{
    return(SCR_add_ptr_lg_0(tbl, ntbl, txt, lg, 0));
}

/* ====================================================================
Ajoute un pointeur dans une table tbl de ntbl pointeurs en réallouant
cette table et alloue l'espace requis pour le stockage du string txt
terminé par un zéro.

Si txt est null, ajoute un pointeur NULL en fin de table.

Les valeurs de tbl et ntbl sont modifiées par le programme.

Le tableau ainsi alloué peut être libéré par SCR_free_tbl().

&RT -1 en cas d'erreur (Memroy full). 0 sinon.
&EX

    U_ch  **ReadFile(filename)
    char    *filename;
    {
	char    **tbl = NULL, buf[121];
	int     ntbl = 0;
	FILE    *fd;

	fd = fopen(filename, "r");
	if(fd == 0) return(-1);

	while(!feof(fd))
	    SCR_add_ptr(&tbl, &ntbl, fgets(buf, 120, fd));

	fclose(fd);
	SCR_add_ptr(&tbl, &ntbl, NULL);
	return(tbl);
    }
&TX
&SA SCR_free_tbl(), SCR_tbl_size(), SCR_add_ptr_lg(), SCR_add_ptr_lg_0()
=======================================================================*/

SCR_add_ptr(tbl, ntbl, txt)
U_ch    ***tbl;
int     *ntbl;
U_ch    *txt;
{
    int     lg;

    if(txt) lg = 1 + (int)strlen(txt);
    else    lg = 0;
    return(SCR_add_ptr_lg_0(tbl, ntbl, txt, lg, 0));
}


/* ====================================================================
Libère l'espace alloué pour un tableau généré à l'aide de SCR_add_ptr(),
SCR_vtom(), SCR_vtoms() et SCR_text().

&EX
    PrintList(string)
    unsigned char   *string;
    {
	int             i;
	unsigned char   *tbl;

	tbl = SCR_vtom(string, ',');
	for(i = 0 ; tbl[i] ; i++)
	    printf("\"%s\"\n", tbl[i]);
	SCR_free_tbl(tbl);
    }
&TX
&SA SCR_vtoms() SCR_vtom(), SCR_text(), SCR_add_ptr(), SCR_tbl_size()
=======================================================================*/

int SCR_free_tbl(tbl)
U_ch    **tbl;
{
    int i;

    if(tbl == 0) return(0);
    for(i = 0 ; tbl[i] != 0 ; i++)
        SCR_free(tbl[i]);
    SCR_free(tbl);
    return(0);
}


/* ====================================================================
Compte le nombre de lignes d'un tableau de pointeurs (alloué par
SCR_add_ptr).

&SA SCR_add_ptr_lg(), SCR_add_ptr(), SCR_free_tbl()
=======================================================================*/

SCR_tbl_size(tbl)
U_ch    **tbl;
{
    int i;

    if(tbl == 0) return(0);
    for(i = 0 ; tbl[i] != 0 ; i++);
    return(i);
}

/* ====================================================================
Copie un tableau de pointeurs vers des strings. Les strings sont
également copiés (et alloués).

&SA SCR_add_ptr(), SCR_free_tbl(), SCR_tbl_size(), SCR_stracpy()
=======================================================================*/

U_ch **SCR_copy_tbl(opts)
U_ch    **opts;
{
    int     i, nb;
    U_ch    **tbl = 0;

    if(opts == 0) return(tbl);
    nb = SCR_tbl_size(opts);
    tbl = (U_ch **) SCR_malloc((nb + 1) * sizeof(U_ch *));
    for(i = 0 ; i < nb ; i++)
        tbl[i] = SCR_stracpy(opts[i]);

    return(tbl);
}



