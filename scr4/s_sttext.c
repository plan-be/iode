#include "s_strs.h"

/* ====================================================================
Construit un tableau de strings … partir du string str
termin‚ par 0. Chaque string g‚n‚r‚ correspond … une ligne de texte
de maximum lg caractŠres termin‚e par un des s‚parateurs d‚finis dans
seps.

Les blancs de d‚but de ligne sont supprim‚s. Les mots plus longs
que lg sont coup‚s sur plusieurs lignes.

La fonction alloue le tableau r‚sultat ainsi que toutes les lignes …
l'aide de SCR_malloc().

Le string input doit se terminer par un caractŠre nul. Le tableau
r‚sultat se termine par un pointeur nul.

&RT pointeur vers le tableau r‚sultat allou‚
&EX
    PrintList(string)
    unsigned char   *string;
    {
	int             i;
	unsigned char   **tbl;

	tbl = SCR_text(texte, ", ;", 15);
	for(i = 0 ; tbl[i] ; i++)
	    printf("\"%s\"\n", tbl[i]);
	SCR_free_tbl(tbl);
    }

    PrintList("Jean, Marc, Pierre, Louis, Marie-Christine-Petit");

    donne :

	"Jean, Marc,"
	"Pierre, Louis,"
	"Marie-Christine"
	"-Petit"
&TX
&SA SCR_vtoms(), SCR_vtom(), SCR_mtov(), SCR_free_tbl();
=======================================================================*/

unsigned char **SCR_text(str, seps, lg)
unsigned char *str,
	      *seps;
int           lg;
{
    U_ch    **tbl = 0, tmp, *astr, *castr;
    int     ntbl = 0, i, prev;


    castr = astr = SCR_stracpy(str); /* JMP 02-02-96 */
    while(1) {
	for(i = 0 ; astr[i] ; i++)
	    if(!SCR_is_space(astr[i])) break; /* SKIP BLANKS */
	if(astr[i] == 0) break;
	astr += i;
	for(prev = 0, i = 0 ; astr[i] && astr[i] != '\n' && i < lg ; i++)
	    if(U_is_in(astr[i], seps)) prev = i;

	if(astr[i] && i >= lg && prev > 0) i = prev;
	else                              i = i - 1;

	tmp = astr[i + 1];
	astr[i + 1] = 0;
	SCR_add_ptr(&tbl, &ntbl, astr);
	astr[i + 1] = tmp;
	if(tmp == 0) break;
	astr += i + 1;
    }

    SCR_add_ptr(&tbl, &ntbl, 0L);
    SCR_free(castr);
    return(tbl);
}

