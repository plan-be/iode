#include "iode.h"

char    K_AggrChar;

char    **K_grep(kdb, str, ecase, names, forms, texts, all)
KDB     *kdb;
char    *str;
int     ecase, names, forms, texts, all;
{
    int     i, j, k, n = 0, found;
    char    **lst = NULL;
    TBL     *tbl;
    TLINE   *tline;
    TCELL   *tcell;


    for(i = 0; i < KNB(kdb); i++) {
	found = 0;
	if(names) found = !SCR_grep_gnl(str, KONAME(kdb, i), ecase, all);

	if(!found)
	{
	    switch(KTYPE(kdb)) {
	    case K_CMT :
		if(texts) found = !SCR_grep_gnl(str, KCVAL(kdb, i), ecase, all);
		break;
	    case K_LST :
		if(texts) found = !SCR_grep_gnl(str, KLVAL(kdb, i), ecase, all);
		break;
	    case K_IDT :
		if(forms) found = !SCR_grep_gnl(str, KILEC(kdb, i), ecase, all);
		break;
	    case K_EQS :
		 if(forms) found = !SCR_grep_gnl(str, KELEC(kdb, i), ecase, all);
		 if(!found && texts)
		    found = !SCR_grep_gnl(str, KECMT(kdb, i), ecase, all);
		 break;

	    case K_TBL:
		 tbl = KTVAL(kdb, i);
		 for(k = 0; k < T_NL(tbl) && !found; k++) {
		    tline = tbl->t_line + k;
		    tcell = (TCELL *) tline->tl_val;
		    switch(tline->tl_type) {
			case KT_LINE  :
			case KT_MODE  :
			case KT_DATE  :
			case KT_FILES : break;
			case KT_TITLE :
			    if(texts) found = !SCR_grep_gnl(str, T_cell_cont(tcell, 1), ecase, all);
			    break;
			case KT_CELL  :
			    for(j = 0; j < T_NC(tbl) && !found; j++)
				if((texts && tcell[j].tc_type == KT_STRING) ||
				   (forms && tcell[j].tc_type == KT_LEC))
					found = !SCR_grep_gnl(str, T_cell_cont(tcell + j, 1), ecase, all);
			    break;
			}
		    }
		    T_free(tbl);
		    break;
	    }
	}

	if(found) SCR_add_ptr(&lst, &n, KONAME(kdb, i));
    }

    if(lst != NULL) SCR_add_ptr(&lst, &n, NULL);
    return(lst);
}

char    *K_expand(type, file, pattern, all)
int     type;
char    *file, *pattern;
int     all;
{

    int     i, np;
    char    **ptbl, **tbl, *lst = NULL;
    KDB     *kdb;

    if(pattern == 0 || pattern[0] == 0 ||
	    type < K_CMT || type > K_VAR) return(NULL);

    if(file == NULL) kdb = K_WS[type];
    else {
	kdb = K_interpret(type, file);
	if(kdb == NULL) return(lst);  /* JMP 05-01-99 */
    }

    ptbl = SCR_vtoms(pattern, A_SEPS); /* JMP 14-08-98 */
    np = SCR_tbl_size(ptbl);
    for(i = 0; i < np; i++) {
	if(ptbl[i][0] != '"' &&
		(U_is_in(all, ptbl[i]) || U_is_in('?', ptbl[i]))) {
	    tbl = K_grep(kdb, ptbl[i], 0, 1, 0, 0, all);
	    SCR_free(ptbl[i]);
	    if(tbl != NULL) {
		ptbl[i] = SCR_mtov(tbl, ';');
		SCR_free_tbl(tbl);
	    }
	    else ptbl[i] = SCR_stracpy("");
	}
    }

    lst = SCR_mtov(ptbl, ';');
    SCR_free_tbl(ptbl);

    if(file != NULL) K_free(kdb);
    return(lst);
}


int K_aggr(pattern, ename, nname)
char    *pattern, *ename, *nname;
{
    int     rc = -1, e, n, p, lg, inp = 0, inb = 0;

    lg = strlen(pattern);
    for(e = 0, n = 0, p = 0; p < lg; p++) {
	if(ename[e] == 0 && !U_is_in(pattern[p], "])"))
	    goto done; /* premature end of name */

	switch(pattern[p]) {
	    case '(' : inp = 1; break;
	    case ')' : inp = 0; break;
	    case '[' : inb = 1; break;
	    case ']' : inb = 0; break;

	    case '?' : if(inb) nname[n] = K_AggrChar;
		       else nname[n] = ename[e];
		       if(inb && ename[e] == K_AggrChar) goto done;
		       n++; e++;
		       break;
	    default :
		       if(pattern[p] == ename[e]) nname[n] = ename[e];
		       else goto done;
		       n++; e++;
	}

    }
    if(ename[e]) goto done; /* premature end of pattern */

    nname[n] = 0;
    SCR_sqz(nname);
    rc = 0;
done:
    return(rc);
}




