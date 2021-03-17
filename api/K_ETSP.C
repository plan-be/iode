#include "iode.h"

/*
    Dumps to TSP format
*/

int EXP_hd_tsp(expdef, dbv, dbc, outfile)
EXPDEF  *expdef;
KDB     *dbv, *dbc;
char    *outfile;
{
    int     freq;

    expdef->exp_fd = fopen(outfile, "w+");

    freq = PER_nb((KSMPL(dbv)->s_p1).p_p);
    switch(freq) {
	case 1: fprintf(expdef->exp_fd,
			"FREQ A;\nSMPL %ld %ld ;\n",
			(KSMPL(dbv)->s_p1).p_y, (KSMPL(dbv)->s_p2).p_y );
		break;

	case 4: fprintf(expdef->exp_fd,
			"FREQ Q;\nSMPL %ld:%ld %ld:%ld ;\n",
			(KSMPL(dbv)->s_p1).p_y, (KSMPL(dbv)->s_p1).p_s,
			(KSMPL(dbv)->s_p2).p_y, (KSMPL(dbv)->s_p2).p_s );
		break;

	case 12:fprintf(expdef->exp_fd,
			"FREQ M;\nSMPL %ld:%ld %ld:%ld ;\n",
			(KSMPL(dbv)->s_p1).p_y, (KSMPL(dbv)->s_p1).p_s,
			(KSMPL(dbv)->s_p2).p_y, (KSMPL(dbv)->s_p2).p_s);
		break;
    }
}

int EXP_end_tsp(expdef, dbv, dbc)
EXPDEF  *expdef;
KDB     *dbv, *dbc;
{
    fclose(expdef->exp_fd);
}

char    *EXP_code_tsp(name, code)
char    *name, **code;
{
    return(EXP_addprepost("LOAD ", " ;\n", name, code));
}

char    *EXP_cmt_tsp(dbc, name, cmt)
KDB     *dbc;
char    *name, **cmt;
{
    int pos;

    pos = K_find(dbc, name);
    if(pos >= 0)  return(*cmt = SCR_stracpy(KCVAL(dbc, pos)) );
    else return(*cmt = SCR_stracpy(" "));
}

char    *EXP_elem_tsp(dbv, nb, t, vec)
KDB     *dbv;
int     nb;
char    **vec;
{
    int     lg, olg;
    char    tmp[81], *buf = NULL;

    EXP_val(tmp,  (double) (*KVVAL(dbv, nb, t)) );
    EXP_addprepost("", " ", tmp, &buf);
    lg = strlen(buf) + 1;

    if(*vec == NULL) olg = 0;
    else olg = strlen(*vec);
    *vec = SW_nrealloc(*vec, olg, olg + lg);

    strcat(*vec, buf);
    SW_nfree(buf);
    return(*vec);
}

void EXP_vec_tsp(expdef, code, cmt, vec)
EXPDEF  *expdef;
char    *code, *cmt, *vec;
{
    int     i;
    char    **text;

    fprintf(expdef->exp_fd, "%s \n", code);
    if(cmt) { /* JMP 04-03-99 */
	text = SCR_text(cmt, " ", 75);
	for(i = 0; text[i]; i++)  fprintf(expdef->exp_fd, "? %s\n", text[i]);
	SCR_free_tbl(text);
	}

    text = SCR_text(vec, " ", 80);
    for(i = 0; text[i]; i++)  fprintf(expdef->exp_fd, "%s\n", text[i]);
    fprintf(expdef->exp_fd, " ; \n");
    SCR_free_tbl(text);

}

EXPDEF EXPTSP = {
    EXP_hd_tsp,

    EXP_code_tsp,
    EXP_cmt_tsp,
    EXP_elem_tsp,
    EXP_vec_tsp,

    EXP_end_tsp,
    NULL
};






