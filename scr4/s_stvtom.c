#include "s_strs.h"

/* ====================================================================
Vector to Matrix : cr‚e un tableau de pointeurs … partir d'un string
termin‚ par 0. Les s‚parateurs de lignes sont les caractŠres contenus
dans le string seps. La fonction alloue le tableau r‚sultat ainsi que
toutes les lignes … l'aide de SCR_malloc().

Le tableau input doit se terminer par un pointeur nul. Le tableau
r‚sultat se termine par un pointeur nul.

La fonction inverse est SCR_mtov().

&RT pointeur vers le tableau r‚sultat allou‚
&EX
    PrintList(string)
    unsigned char   *string;
    {
	int             i;
	unsigned char   **tbl;

	tbl = SCR_vtoms(string, ", ;");
	for(i = 0 ; tbl[i] ; i++)
	    printf("\"%s\"\n", tbl[i]);
	SCR_free_tbl(tbl);
    }

    PrintList("Jean    Marc;,,Pierre");

    donne :

	"Jean"
	"Marc"
	"Pierre"
&TX
&SA <-SCR_vtom()>, <-SCR_vtoms2()>, <-SCR_vtoms3()>, SCR_mtov(), SCR_free_tbl(), SCR_copy_tbl()
=======================================================================*/

unsigned char **SCR_vtoms(str, seps)
unsigned char    *str,
		 *seps;
{
    U_ch    **tbl = 0, tmp;
    int     ntbl = 0, i;

    if(str == 0) return(tbl);
    while(1) {
	for(i = 0 ; str[i] ; i++)
	    if(!U_is_in(str[i], seps)) break; /* SKIP SEPS */
	if(str[i] == 0) break;
	str += i;
	for(i = 0 ; str[i] ; i++)
	    if(U_is_in(str[i], seps)) break;
	SCR_add_ptr_lg_0(&tbl, &ntbl, str, i, 1); /* JMP 07-06-96 */
	if(str[i] == 0) break; /* JMP 07-06-96 */
	str += i + 1;
    }

    SCR_add_ptr(&tbl, &ntbl, 0L);
    return(tbl);
}


/* ====================================================================
Vector to Matrix : cr‚e un tableau de pointeurs … partir d'un string
termin‚ par 0. Le s‚parateur de lignes est sep. La fonction alloue le
tableau r‚sultat ainsi que toutes les lignes … l'aide de SCR_malloc().

Le string input doit se terminer par un caractŠre nul. Le tableau
r‚sultat se termine par un pointeur nul.

La fonction inverse est SCR_mtov().

&RT pointeur vers le tableau r‚sultat allou‚
&EX
    PrintList(string)
    unsigned char   *string;
    {
	int             i;
	unsigned char   **tbl;

	tbl = SCR_vtom(string, ',');
	for(i = 0 ; tbl[i] ; i++)
	    printf("\"%s\"\n", tbl[i]);
	SCR_free_tbl(tbl);
    }

    PrintList("Jean,Marc,Pierre");

    donne :

	"Jean"
	"Marc"
	"Pierre"
&TX
&SA SCR_vtoms() SCR_mtov(), SCR_free_tbl();
=======================================================================*/

unsigned char **SCR_vtom(str, sep)
unsigned char   *str;
int             sep;
{
    U_ch    seps[2];

    seps[0] = sep;
    seps[1] = 0;

    return(SCR_vtoms(str, seps));
}


/* ====================================================================
Vector to Matrix : cr‚e un tableau de pointeurs … partir d'un string
termin‚ par 0. Les s‚parateurs de lignes sont les caractŠres contenus
dans le string seps. La fonction alloue le tableau r‚sultat ainsi que
toutes les lignes … l'aide de SCR_malloc().

&EN unsigned char *str : tableau de caractŠres
&EN unsigned char *seps : s‚parateurs de liste
&EN int           supdbl : 1 pour que les doubles s‚parateurs
    ne g‚nŠre qu'un seul ‚l‚ment, 0 pour garder une ligne par s‚parateur

Le tableau input doit se terminer par un pointeur nul. Le tableau
r‚sultat se termine par un pointeur nul.

La fonction inverse est SCR_mtov().

&RT pointeur vers le tableau r‚sultat allou‚
&EX
    PrintList(string)
    unsigned char   *string;
    {
	int             i;
	unsigned char   **tbl;

	tbl = SCR_vtoms2(string, ", ;", 0);
	for(i = 0 ; tbl[i] ; i++)
	    printf("\"%s\"\n", tbl[i]);
	SCR_free_tbl(tbl);
    }

    PrintList("Jean    Marc;,,Pierre");

    donne :

	"Jean"
	"Marc"
	""
	""
	"Pierre"
&TX
&SA SCR_vtom(), SCR_vtoms(), SCR_vtoms3(), SCR_mtov(), SCR_free_tbl(), SCR_copy_tbl()
=======================================================================*/

unsigned char **SCR_vtoms2(str, seps, supdbl)
unsigned char    *str,
		 *seps;
int              supdbl;
{
    U_ch    **tbl = 0, tmp;
    int     ntbl = 0, i;

    if(str == 0) return(tbl);
    while(1) {
	if(supdbl) {                           /* JMP 02-09-98 */
	    for(i = 0 ; str[i] ; i++)
		if(!U_is_in(str[i], seps)) break; /* SKIP SEPS */
	    }
	else i = 0;

	if(str[i] == 0) {
	    if(supdbl == 0)
		SCR_add_ptr_lg_0(&tbl, &ntbl, "", 1, 0); /* JMP 07-06-96 */
	    break;
	    }
	str += i;
	for(i = 0 ; str[i] ; i++)
	    if(U_is_in(str[i], seps)) break;
	if(supdbl == 0 && i == 0)
	    SCR_add_ptr_lg_0(&tbl, &ntbl, "", 1, 0); /* JMP 07-06-96 */
	else
	    SCR_add_ptr_lg_0(&tbl, &ntbl, str, i, 1); /* JMP 07-06-96 */
	if(str[i] == 0) break; /* JMP 07-06-96 */
	str += i + 1;
    }

    SCR_add_ptr(&tbl, &ntbl, 0L);
    return(tbl);
}

/*NH*/
SCR_vtoms2ACmpt(str, seps, supdbl)
unsigned char    *str,
		 *seps;
int              supdbl;
{
    int     ntbl = 0, i;

    if(str == 0) return(0);
    while(1) {
	if(supdbl) {
	    for(i = 0 ; str[i] ; i++)
		if(!U_is_in(str[i], seps)) break; /* SKIP SEPS */
	    }
	else i = 0;

	if(str[i] == 0) {
	    if(supdbl == 0) ntbl++;
	    break;
	    }
	str += i;
	for(i = 0 ; str[i] ; i++)
	    if(U_is_in(str[i], seps)) break;
	ntbl++;
	if(str[i] == 0) break; /* JMP 07-06-96 */
	str += i + 1;
    }

    return(ntbl);
}

/*NH*/
unsigned char **SCR_vtoms2A(str, seps, supdbl)
unsigned char    *str,
		 *seps;
int              supdbl;
{
    U_ch    **tbl = 0, tmp;
    int     ntbl = 0, i, intbl = 0;

    if(str == 0) return(tbl);
    ntbl = SCR_vtoms2ACmpt(str, seps, supdbl);
    tbl = (U_ch **) SCR_malloc((ntbl + 1) * sizeof(char *));

    while(1) {
	if(supdbl) {                           /* JMP 02-09-98 */
	    for(i = 0 ; str[i] ; i++)
		if(!U_is_in(str[i], seps)) break; /* SKIP SEPS */
	    }
	else i = 0;

	if(str[i] == 0) {
	    if(supdbl == 0) {
		tbl[intbl] = SCR_stracpy("");
		intbl++;
		//SCR_add_ptr_lg_0(&tbl, &ntbl, "", 1, 0); /* JMP 07-06-96 */
		}
	    break;
	    }
	str += i;
	for(i = 0 ; str[i] ; i++)
	    if(U_is_in(str[i], seps)) break;
	if(supdbl == 0 && i == 0) {
	    tbl[intbl] = SCR_stracpy("");
	    intbl++;
	    //SCR_add_ptr_lg_0(&tbl, &ntbl, "", 1, 0); /* JMP 07-06-96 */
	    }
	else {
	    tbl[intbl] = (U_ch  *) SCR_malloc(i + 1);
	    memcpy(tbl[intbl], str, i);
	    intbl++;
	    //SCR_add_ptr_lg_0(&tbl, &ntbl, str, i, 1); /* JMP 07-06-96 */
	    }
	if(str[i] == 0) break; /* JMP 07-06-96 */
	str += i + 1;
    }

    //SCR_add_ptr(&tbl, &ntbl, 0L);
    return(tbl);
}

/* ====================================================================
Vector to Matrix : cr‚e un tableau de pointeurs … partir d'un string
termin‚ par 0. Les s‚parateurs de lignes sont les caractŠres contenus
dans le string seps. La fonction alloue le tableau r‚sultat ainsi que
toutes les lignes … l'aide de SCR_malloc().

&EN unsigned char *str : tableau de caractŠres
&EN unsigned char *seps : s‚parateurs de liste
&EN int           supdbl : 1 pour que les doubles s‚parateurs
    ne g‚nŠre qu'un seul ‚l‚ment, 0 pour garder une ligne par s‚parateur

Le tableau input doit se terminer par un pointeur nul. Le tableau
r‚sultat se termine par un pointeur nul.

Les strings entre doubles quotes (") ne sont pas s‚par‚s, mˆme s'il
contiennent un s‚parateur.

&RT pointeur vers le tableau r‚sultat allou‚
&EX
    PrintList(string)
    unsigned char   *string;
    {
	int             i;
	unsigned char   **tbl;

	tbl = SCR_vtoms3(string, " ", 0);
	for(i = 0 ; tbl[i] ; i++)
	    printf("\"%s\"\n", tbl[i]);
	SCR_free_tbl(tbl);
    }

    PrintList("CalcList \"1 + 2\" 3 \"4 5\");

    donne :

	"1 + 2"
	"3"
	"4 5"
&TX
&SA SCR_vtom(), SCR_vtoms(), SCR_vtoms2(), SCR_mtov(), SCR_free_tbl(), SCR_copy_tbl()

TODO: ERREUR: à corriger pour les args entre "abc","def" => "abc",NULL,"def" 
=======================================================================*/

unsigned char **SCR_vtoms3(str, seps, supdbl)
unsigned char    *str,
		 *seps;
int              supdbl;
{
    U_ch    **tbl = 0, tmp;
    int     ntbl = 0, i;

    if(str == 0) return(tbl);
    while(1) {
	if(supdbl) {                           /* JMP 02-09-98 */
	    // Saute les s‚parateurs en d‚but de str
	    for(i = 0 ; str[i] ; i++)
		if(!U_is_in(str[i], seps)) break; /* SKIP SEPS */
	    }
	else i = 0;

	if(str[i] == '"') {
	    // lit l'argument complet entre doubles quotes
	    for(i = 1 ; str[i] && str[i] != '"' ; i++);
	    SCR_add_ptr_lg_0(&tbl, &ntbl, str + 1, i - 1, 1); /* JMP 07-06-96 */
	    if(str[i] == 0) break; /* JMP 07-06-96 */
	    str += i + 1;
	    i = 0;
	    continue;
	    }

	/* JMP 13-01-11  : Pq ajouter un argument vide … la fin ?
	if(str[i] == 0) {
	    if(supdbl == 0)
		SCR_add_ptr_lg_0(&tbl, &ntbl, "", 1, 0); /* JMP 07-06-96
	    break;
	    }
	*/

	str += i;
	for(i = 0 ; str[i] ; i++)
	    if(U_is_in(str[i], seps)) break;
	if(supdbl == 0 && i == 0)
	    SCR_add_ptr_lg_0(&tbl, &ntbl, "", 1, 0); /* JMP 07-06-96 */
	else
	    SCR_add_ptr_lg_0(&tbl, &ntbl, str, i, 1); /* JMP 07-06-96 */
	if(str[i] == 0) break; /* JMP 07-06-96 */
	str += i + 1;
    }

    SCR_add_ptr(&tbl, &ntbl, 0L);
    return(tbl);
}
