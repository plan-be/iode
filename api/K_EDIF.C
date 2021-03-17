#include "iode.h"

/*
    Dumps a dif format with following format
*/


int EXP_hd_dif(expdef, dbv, dbc, outfile)
EXPDEF  *expdef;
KDB     *dbv, *dbc;
char    *outfile;
{
    int dim, nb, i;

    expdef->exp_fd = fopen(outfile, "w+");
    dim = KSMPL(dbv)->s_nb;
    nb = KNB(dbv);

    fprintf(expdef->exp_fd,
	    "TABLE\n0,1\n\"\"\nVECTORS\n0,%d\n\"\"\nTUPLES\n0,%d\n\"\"\n",
	    dim + 2, nb + 1);
    fprintf(expdef->exp_fd,
	    "DATA\n0,0\n\"\"\n-1,0\nBOT\n1,0\n\"CODE\"\n1,0\n\"COMMENT\"\n");

    for(i = 0; i < dim; i++) {
	fprintf(expdef->exp_fd, "1,0\n\"%s\"\n",
	    PER_pertoa(PER_addper(&(KSMPL(dbv)->s_p1), i), NULL) );
    }
}

int EXP_end_dif(expdef, dbv, dbc)
EXPDEF  *expdef;
KDB     *dbv, *dbc;
{
    fprintf(expdef->exp_fd, "-1,0\nEOD\n");
    fclose(expdef->exp_fd);
}

extern  char    *EXP_addprepost();

char    *EXP_code_dif(name, code)
char    *name, **code;
{

    return(EXP_addprepost("-1,0\nBOT\n1,0\n\"", "\"\n", name, code));
}

char    *EXP_cmt_dif(dbc, name, cmt)
KDB     *dbc;
char    *name, **cmt;
{
    int pos;

    pos = K_find(dbc, name);
    if(pos >= 0)
	return(EXP_addprepost("1,0\n\"", "\"\n", KCVAL(dbc, pos), cmt));
    else
	return(EXP_addprepost("1,0\n\"", "\"\n", "", cmt));
}

char    *EXP_elem_dif(dbv, nb, t, vec)
KDB     *dbv;
int     nb;
char    **vec;
{
    int     lg, olg;
    char    tmp[81], *buf = NULL;

    EXP_val(tmp,  (double) (*KVVAL(dbv, nb, t)) );
    EXP_addprepost("0,", "\nV\n", tmp, &buf);
    lg = strlen(buf) + 1;

    if(*vec == NULL) olg = 0;
    else olg = strlen(*vec);
    *vec = SW_nrealloc(*vec, olg, olg + lg);

    strcat(*vec, buf);
    SW_nfree(buf);
    return(*vec);
}

void EXP_vec_dif(expdef, code, cmt, vec)
EXPDEF  *expdef;
char    *code, *cmt, *vec;
{
    fprintf(expdef->exp_fd, "%s%s%s", code, cmt, vec);
}

EXPDEF EXPDIF = {
    EXP_hd_dif,

    EXP_code_dif,
    EXP_cmt_dif,
    EXP_elem_dif,
    EXP_vec_dif,

    EXP_end_dif,
    NULL
};


