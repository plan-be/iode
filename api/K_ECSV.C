#include "iode.h"

/*
    Dumps an asci format with following format
	code ;  cmt;1980Y1;1981Y1;... 1990Y1;
	A;comment of A;0.1;0.2;na;
	.....
*/


int EXP_hd_csv(expdef, dbv, dbc, outfile)
EXPDEF  *expdef;
KDB     *dbv, *dbc;
char    *outfile;
{
    int dim, i;

    expdef->exp_fd = fopen(outfile, "w+");
    if(expdef->exp_fd == 0) {
	B_seterror("Cannot create %s", outfile);
	return(-1);
	}

    fprintf(expdef->exp_fd, "code%scomment%s", EXP_SEP, EXP_SEP);
    dim = KSMPL(dbv)->s_nb;
    for(i = 0; i < dim; i++) {
	fprintf(expdef->exp_fd, "%s%s",
	    PER_pertoa(PER_addper(&(KSMPL(dbv)->s_p1), i), NULL), EXP_SEP);
    }
    fprintf(expdef->exp_fd, "\n");
    return(0);
}

int EXP_end_csv(expdef, dbv, dbc)
EXPDEF  *expdef;
KDB     *dbv, *dbc;
{
    fclose(expdef->exp_fd);
}


char    *EXP_code_csv(name, code)
char    *name, **code;
{
    return(EXP_addsep(name, code));
}

char    *EXP_cmt_csv(dbc, name, cmt)
KDB     *dbc;
char    *name, **cmt;
{
    int     pos;
    U_ch    *ccmt;                     /* JMP 19-09-96 */

    pos = K_find(dbc, name);
    if(pos >= 0)  {
	ccmt = KCVAL(dbc, pos);        /* JMP 19-09-96 */
	SCR_replace(ccmt, "\n", " ");  /* JMP 19-09-96 */
	return(EXP_addsep(ccmt, cmt)); /* JMP 19-09-96 */
	}
    else return(EXP_addsep("", cmt));
}

char    *EXP_elem_csv(dbv, nb, t, vec)
KDB     *dbv;
int     nb;
char    **vec;
{
    int     lg, olg;
    char    tmp[81], *buf = NULL;

    EXP_val(tmp,  (double) (*KVVAL(dbv, nb, t)) );
    EXP_addsep(tmp, &buf);
    lg = strlen(buf) + 1;

    if(*vec == NULL) olg = 0;
    else olg = strlen(*vec);
    *vec = SW_nrealloc(*vec, olg, olg + lg);

    strcat(*vec, buf);
    SW_nfree(buf);
    return(*vec);
}

void EXP_vec_csv(expdef, code, cmt, vec)
EXPDEF  *expdef;
char    *code, *cmt, *vec;
{
    fprintf(expdef->exp_fd, "%s %s %s\n",
	    (code == NULL ? "" : code ),
	    (cmt == NULL  ? "" : cmt ),
	    (vec == NULL  ? "" : vec ) );
}

EXPDEF EXPCSV = {
    EXP_hd_csv,

    EXP_code_csv,
    EXP_cmt_csv,
    EXP_elem_csv,
    EXP_vec_csv,

    EXP_end_csv,
    NULL
};

/************* ROTATED CSV **************/

int EXP_hd_rcsv(expdef, dbv, dbc, outfile)
EXPDEF  *expdef;
KDB     *dbv, *dbc;
char    *outfile;
{
    int dim, i;

    expdef->exp_fd = fopen(outfile, "w+");
    if(expdef->exp_fd == 0) {
	B_seterror("Cannot create %s", outfile);
	return(-1);
	}
    return(0);
}

char    *EXP_elem_rcsv(dbv, nb, t, vec)
KDB     *dbv;
int     nb;
char    **vec;
{
    char    tmp[81], *buf = NULL;

    EXP_val(tmp,  (double) (*KVVAL(dbv, nb, t)) );
    EXP_addsep(tmp, &buf);
    if(vec) *vec = buf;
    return(buf);
}

void EXP_vec_rcsv(expdef, code, cmt, vec)
EXPDEF  *expdef;
char    *code, *cmt, *vec;
{
    if(code == 0) {
	fprintf(expdef->exp_fd, "\n");
	return;
	}
    fprintf(expdef->exp_fd, "%s ", code);
}

EXPDEF EXPRCSV = {
    EXP_hd_rcsv,

    EXP_code_csv,
    EXP_cmt_csv,
    EXP_elem_rcsv,
    EXP_vec_rcsv,

    EXP_end_csv,
    NULL
};


