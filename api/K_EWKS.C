#include "iode.h"

/*
    Dumps an wks format
*/

int WKS_COL = 1, WKS_ROW = 1;

int EXP_hd_wks(expdef, dbv, dbc, outfile)
EXPDEF  *expdef;
KDB     *dbv, *dbc;
char    *outfile;
{
    int dim, nb, i;

    WKS_COL = 1, WKS_ROW = 1;
    dim = KSMPL(dbv)->s_nb;
    nb = KNB(dbv);

    wks_init(outfile, dim + 2, nb + 1);
    wks_cwidth(2, 20);
    for(i = 0, WKS_COL = 3; i < dim; i++, WKS_COL++)
	wks_string(PER_pertoa(PER_addper(&(KSMPL(dbv)->s_p1), i), NULL),
		    WKS_COL, WKS_ROW);
    WKS_ROW ++;
    WKS_COL = 1;
}

int EXP_end_wks(expdef, dbv, dbc)
EXPDEF  *expdef;
KDB     *dbv, *dbc;
{
    wks_end();
}


char    *EXP_code_wks(name, code)
char    *name, **code;
{
    wks_string(name, WKS_COL, WKS_ROW);
    WKS_COL ++;
    *code = SCR_stracpy(name);
    return(*code);
}

char    *EXP_cmt_wks(dbc, name, cmt)
KDB     *dbc;
char    *name, **cmt;
{
    int pos;

    pos = K_find(dbc, name);
    if(pos >= 0)  wks_string(KCVAL(dbc, pos), WKS_COL, WKS_ROW);
    else wks_string(" ", WKS_COL, WKS_ROW);
    WKS_COL ++;

    return(*cmt = NULL);
}

char    *EXP_elem_wks(dbv, nb, t, vec)
KDB     *dbv;
int     nb;
char    **vec;
{
    int     lg, olg;
    char    tmp[81], *buf = NULL;

    wks_value((double) (*KVVAL(dbv, nb, t)), WKS_COL, WKS_ROW);
    WKS_COL ++;

    return(*vec = NULL);
}

void EXP_vec_wks(expdef, code, cmt, vec)
EXPDEF  *expdef;
char    *code, *cmt, *vec;
{
    wks_name(code, 3, WKS_ROW, WKS_COL, WKS_ROW);
    WKS_ROW ++;
    WKS_COL = 1;
}

EXPDEF EXPWKS = {
    EXP_hd_wks,

    EXP_code_wks,
    EXP_cmt_wks,
    EXP_elem_wks,
    EXP_vec_wks,

    EXP_end_wks,
    NULL
};



