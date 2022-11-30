#include "s_strs.h"

/* ====================================================================
Matrix to Vector : cr‚e un vecteur avec la matrice de pointeurs tbl. Le
s‚parateur de lignes est sep. Le vecteur est constitu‚ de toutes les
lignes du tableau s‚par‚es par sep. La fonction alloue le vecteur
r‚sultat … l'aide de SCR_malloc().

Le tableau input doit se terminer par un pointeur nul.

La fonction inverse est SCR_vtom().

&RT pointeur vers la vecteur r‚sultat allou‚
&EX
    char    *tbl[] = {
	"Jean",
	"Marc",
	"Pierre",
	NULL
	};

    printf("\"%s\"\n", SCR_mtov(tbl, ','));

    donne :

	"Jean,Marc,Pierre"
&TX
&SA SCR_mtovs(), SCR_vtom(), SCR_free_tbl();
=======================================================================*/

unsigned char *SCR_mtov(tbl, sep)
unsigned char   **tbl;
int             sep;
{
    U_ch    buf[2];

    buf[0] = sep;
    buf[1] = 0;
    return(SCR_mtovs(tbl, buf));
}

/* ====================================================================
Matrix to Vector : cr‚e un vecteur avec la matrice de pointeurs tbl. Le
s‚parateur de lignes est seps et peut contenir plusieurs caractŠres. Le
vecteur est constitu‚ de toutes les lignes du tableau s‚par‚es par seps.
La fonction alloue le vecteur r‚sultat … l'aide de SCR_malloc().

Le tableau input doit se terminer par un pointeur nul.

&RT pointeur vers la vecteur r‚sultat allou‚
&EX
    char    *tbl[] = {
	"Jean",
	"Marc",
	"Pierre",
	NULL
	};

    printf("\"%s\"\n", SCR_mtovs(tbl, '+++'));

    donne :

	"Jean+++Marc+++Pierre"
&TX
&SA SCR_vtom(), SCR_vtoms(), SCR_free_tbl(), SCR_mtov()
=======================================================================*/

unsigned char *SCR_mtovs(tbl, seps)
unsigned char    **tbl,
		 *seps;
{
    U_ch    *res = 0;
/*    char    *SCR_malloc(); /* JMP 04-01-98 */
    int     len = 0, sepslen, i, pos = 0, lg;  /* JMP 03-10-2003 */

    if(tbl == 0) return(res);
    sepslen = (int)strlen(seps);

    /* Count space */
    for(i = 0 ; tbl[i] != 0 ; i++)
	len += sepslen + (int)strlen(tbl[i]);

    /* Allocate and create output vector */
    res = (U_ch  *)SCR_malloc(len + 1);

    for(i = 0 ; tbl[i] != 0 ; i++)  {
	//strcat(res, tbl[i]);      /* JMP 03-10-2003 */
	lg = (int)strlen(tbl[i]);
	memcpy(res + pos, tbl[i], lg);  /* JMP 03-10-2003 */
	pos += lg;
	if(tbl[i + 1]) {
	    //strcat(res, seps);    /* JMP 03-10-2003 */
	    memcpy(res + pos, seps, sepslen);      /* JMP 03-10-2003 */
	    pos += sepslen;
	    }
	}

    res[pos] = 0; /* JMP 03-10-2003 */
    return(res);
}




