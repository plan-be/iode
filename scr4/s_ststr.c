#include "scr4.h"       /* BP_M 23-01-97 */

TBLSTR  *TBL_STR = 0;
int     NB_TBL_STR = 0;


#define NB_ALLOC    20

short SCR_str_add(txt, len)
char    *txt;
int     len;
{
    short   i;
    TBLSTR  *tb;

    if(len == 0) len = (int)strlen(txt) + 1;
    i = SCR_str_find_str(txt, len);
    if(i >= 0) return(i);
    if(NB_TBL_STR == 0)
	TBL_STR = (TBLSTR *)SCR_malloc(sizeof(TBLSTR) * NB_ALLOC);
    NB_TBL_STR++;
    if(NB_TBL_STR % NB_ALLOC == 0)
	TBL_STR = (TBLSTR *)SCR_realloc(TBL_STR, sizeof(TBLSTR),
					NB_TBL_STR - NB_ALLOC,
					NB_TBL_STR);

    i = NB_TBL_STR - 1;
    tb = TBL_STR + i;
    tb->ts_len = len;
    tb->ts_ptr = SCR_malloc(len);
    memcpy(tb->ts_ptr, txt, len);
    return(i);
}

short SCR_str_find_str(txt, len)
char    *txt;
int     len;
{
    short   i;
    TBLSTR  *tb;

    if(len == 0) len = (int)strlen(txt) + 1;
    for(i = 0 , tb = TBL_STR ; i < NB_TBL_STR ; i++, tb++)
	if(memcmp(txt, tb->ts_ptr, _min_(len, tb->ts_len)) == 0)
	    return(i);

    return(-1);
}

char *SCR_str_find(nb)
int     nb;
{
    if(nb < NB_TBL_STR)
	return((TBL_STR + nb)->ts_ptr);
    else
    return((char *)0); /* JMP 04-01-98 */
}
