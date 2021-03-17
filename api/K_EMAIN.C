#include "iode.h"

extern  char    **IMP_rule;
extern  char    **IMP_pat;
extern  int     IMP_trace;

char    EXP_SEP[11];
char    EXP_NA[11];

void EXP_set(na, sep)
char    *na, *sep;
{
    strcpy(EXP_SEP, sep);
    strcpy(EXP_NA, na);

    SCR_strip(EXP_SEP);
    if(EXP_SEP[0] == 0) strcpy(EXP_SEP, " ");
    SCR_strip(EXP_NA);
    if(EXP_NA[0] == 0) strcpy(EXP_NA, "#N/A");
}

void EXP_val(tmp, val)
char    *tmp;
double  val;
{
    if(L_ISAN(val)) {
	SCR_fmt_dbl(val, tmp, 20, -1);
	SCR_sqz(tmp);
    }
    else strcpy(tmp, EXP_NA);
}


char    *EXP_addprepost(pre, post, src, tg)
char    *pre, *post, *src, **tg;
{
    char    *buf = NULL;

    buf = SCR_stracpy(src);
    SCR_strip(buf);
    *tg = SW_nalloc(strlen(buf) +
	(pre == NULL ? 0 : strlen(pre)) +
	(post == NULL ? 0 : strlen(post)) + 1);
    if(*tg != NULL)
	sprintf(*tg, "%s%s%s",
	    (pre == NULL ? "" : pre),
	    buf,
	    (post == NULL ? "" : post));

    SCR_free(buf);

    return(*tg);
}

char    *EXP_addsep(src, tg)
char    *src, **tg;
{
    return(EXP_addprepost("", EXP_SEP, src, tg));
}

int EXP_Ws(expdef, dbv, dbc, rulefile, outfile, na, sep)
EXPDEF  *expdef;
KDB     *dbv, *dbc;
char    *rulefile, *outfile, *na, *sep;
{
    int     i, j, dim;
    char    *code = NULL, *cmt = NULL, *vec = NULL;
    ONAME   iname;
    char    oname[81];

    EXP_set(na, sep);

    if(dbv == NULL && dbc == NULL) goto err;
    if(IMP_readrule(rulefile) < 0) goto err;

    if(expdef->exp_hd_fn != NULL
	    && (*(expdef->exp_hd_fn))(expdef, dbv, dbc, outfile) < 0) goto err;

    dim = KSMPL(dbv)->s_nb;
    for(i = 0; i < KNB(dbv); i++) {
		strcpy(iname, KONAME(dbv, i));
		if(IMP_change(IMP_rule, IMP_pat, iname, oname) < 0) continue;

		if(expdef->exp_code_fn != NULL)
		    (*(expdef->exp_code_fn))(oname, &code);
		else code = SCR_stracpy(oname);

		if(expdef->exp_cmt_fn != NULL && dbc != NULL)
		    (*(expdef->exp_cmt_fn))(dbc, iname, &cmt) ;

		if(expdef->exp_elem_fn != NULL && dbv != NULL)
		    for(j = 0; j < dim; j++)
				(*(expdef->exp_elem_fn))(dbv, i, j, &vec);

		if(expdef->exp_vec_fn != NULL)
		    (*(expdef->exp_vec_fn))(expdef, code, cmt, vec);

		SW_nfree(code); code = NULL;
		SW_nfree(cmt);  cmt = NULL;
		SW_nfree(vec);  vec = NULL;
    }

    if(expdef->exp_end_fn != NULL
	    && (*(expdef->exp_end_fn))(expdef, dbv, dbc, outfile) < 0) goto err;

    return(0);

err:
    return(-1);
}

int EXP_Rev_Ws(expdef, dbv, dbc, rulefile, outfile, na, sep)
EXPDEF  *expdef;
KDB     *dbv, *dbc;
char    *rulefile, *outfile, *na, *sep;
{
    int     i, j, nl, nc;
    char    *code = NULL;
    ONAME   iname;
    char    oname[81];

    EXP_set(na, sep);

    if(dbv == NULL && dbc == NULL) goto err;
    if(IMP_readrule(rulefile) < 0) goto err;

    if(expdef->exp_hd_fn != NULL
	    && (*(expdef->exp_hd_fn))(expdef, dbv, dbc, outfile) < 0) goto err;

    nl = KSMPL(dbv)->s_nb;
    nc = KNB(dbv);

    if(expdef->exp_vec_fn != NULL)
	    (*(expdef->exp_vec_fn))(expdef, EXP_SEP, 0L, 0L);

    for(i = 0; i < nc; i++) {
		strcpy(iname, KONAME(dbv, i));
		if(IMP_change(IMP_rule, IMP_pat, iname, oname) < 0) continue;

		if(expdef->exp_code_fn != NULL)
		    (*(expdef->exp_code_fn))(oname, &code);
		else code = SCR_stracpy(oname);
		if(expdef->exp_vec_fn != NULL)
		    (*(expdef->exp_vec_fn))(expdef, code, 0L, 0L);
		SW_nfree(code); code = NULL;
    }
    if(expdef->exp_vec_fn != NULL)
		(*(expdef->exp_vec_fn))(expdef, 0L, 0L, 0L);

    for(j = 0; j < nl; j++) {
		sprintf(oname, "%s%s",
		    PER_pertoa(PER_addper(&(KSMPL(dbv)->s_p1), j), NULL), EXP_SEP);
		if(expdef->exp_vec_fn != NULL)
			(*(expdef->exp_vec_fn))(expdef, oname, 0L, 0L);

		for(i = 0; i < nc; i++) {
		    strcpy(iname, KONAME(dbv, i));
		    if(IMP_change(IMP_rule, IMP_pat, iname, oname) < 0) continue;

		    if(expdef->exp_elem_fn != NULL)
		       (*(expdef->exp_elem_fn))(dbv, i, j, &code);
		    if(expdef->exp_vec_fn != NULL)
			(*(expdef->exp_vec_fn))(expdef, code, 0L, 0L);
		    SW_nfree(code); code = NULL;
		}
		if(expdef->exp_vec_fn != NULL)
		    (*(expdef->exp_vec_fn))(expdef, 0L, 0L, 0L);
    }

    if(expdef->exp_end_fn != NULL
	    && (*(expdef->exp_end_fn))(expdef, dbv, dbc, outfile) < 0) goto err;

    return(0);

err:
    return(-1);
}




