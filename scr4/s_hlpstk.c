#include "scr4.h"

int     HLP_NB_STTBL = 0;
U_ch    **HLP_STTBL = 0;

/*NH*/
HLP_stack_add(help)
char    *help;
{
    SCR_add_ptr(&HLP_STTBL, &HLP_NB_STTBL, help);
    return(0);
}

/*NH*/
HLP_stack_del(n)
int     n;
{
    for( ; n > 0 && HLP_NB_STTBL > 0; n--) {
	HLP_NB_STTBL--;
	SCR_free(HLP_STTBL[HLP_NB_STTBL]);
	HLP_STTBL[HLP_NB_STTBL] = 0;
	}
    if(HLP_NB_STTBL == 0) {
	SCR_free(HLP_STTBL);
	HLP_STTBL = 0;
	}
    return(0);
}

/*NH*/
HLP_next_fname()
{
    U_ch    **tbl, *tmp;
    int     i, nl;

    tbl = SCR_vtom(HLP_NAME, ';');
    nl = SCR_tbl_size(tbl);
    if(nl < 2) return(0);
    tmp = tbl[0];
    for(i = 1 ; i < nl ; i++)
	tbl[i - 1] = tbl[i];
    tbl[nl - 1] = tmp;
    tmp = SCR_mtov(tbl, ';');
    strcpy(HLP_NAME, tmp);
    SCR_free(tmp);
    SCR_free_tbl(tbl);
    return(0);
}

/*NH*/
HLP_fname(fname, msg)
U_ch    *fname;
int     msg;
{
    U_ch    **tbl;

    tbl = SCR_vtom(HLP_NAME, ';');
    if(SCR_tbl_size(tbl) == 0) { /* JMP 13-01-02 */
	if(msg) PG_display_error(SCR_err_txt(102));
	return(-1);
	}
    strcpy(fname, tbl[0]);   /* JMP 13-01-02 */
    SCR_free_tbl(tbl);       /* JMP 13-01-02 */
    SCR_cat_ext(fname, fname, "hlp");  /* JMP 13-01-02 */
    return(0);
}







