#include "iode.h"

extern int B_GRAPHDEFAULT;

B_GraphDefault(type) /*GB 10/03/2011 */
char    *type;
{
    switch(type[0]) {
	case 'l' :
	case 'L' : B_GRAPHDEFAULT = 0; break;
	case 's' :
	case 'S' : B_GRAPHDEFAULT = 1; break;
	case 'b' :
	case 'B' : B_GRAPHDEFAULT = 2; break;
	default  :
	    B_seterrn(304);
	    return(-1);
	}
    return(0);
}


TBL *T_create(dim)
int dim;
{
    TBL     *tbl = NULL;
    TLINE   line;
    TCELL   *cell;
    int     i;

    tbl = SW_nalloc(sizeof(TBL));
    if(tbl == NULL) {
	kerror(0, "Allocation error");
	return(NULL);
    }
    T_NC(tbl)   = dim;
    T_LANG(tbl) = KT_ENGLISH;

    tbl->t_zmin = L_NAN;
    tbl->t_zmax = L_NAN;
    tbl->t_ymin = L_NAN;
    tbl->t_ymax = L_NAN;

    tbl->t_div.tl_type = KT_CELL;
    tbl->t_div.tl_val  = SW_nalloc(dim * sizeof(TCELL));
    cell               = (TCELL *) tbl->t_div.tl_val;

    for(i = 0; i < dim; i++) {
	cell[i].tc_type = KT_LEC;
	cell[i].tc_attr = KT_LEFT;
    }
    K_ipack(&(cell[0].tc_val), "1");
	/* rest is repetitive if val[i] == 0, val[i] = val[i-1] */

    return(tbl);
}

void T_free(tbl)
TBL *tbl;
{
    int     i;

    T_free_line(&(tbl->t_div), T_NC(tbl));
    for(i = 0; i < T_NL(tbl); i++) T_free_line(tbl->t_line + i, T_NC(tbl));
    SW_nfree(tbl->t_line);
    SW_nfree(tbl);
}

void T_free_line(line, dim)
TLINE   *line;
int     dim;
{
    int     i;
    TCELL   *cell;

    cell = (TCELL *) line->tl_val;
    if(line->tl_type == KT_CELL)
	    for(i = 0; i < dim; i++) T_free_cell(cell + i);

    if(line->tl_type == KT_TITLE) T_free_cell(cell);

    SW_nfree(line->tl_val);
}

void T_free_cell(cell)
TCELL   *cell;
{
    SW_nfree(cell->tc_val);
    cell->tc_val = NULL;
}

int T_add_line(tbl)
TBL     *tbl;
{
    int nl = T_NL(tbl);

    if(nl % KT_CHUNCK == 0)
	T_L(tbl) = (char *) SW_nrealloc(T_L(tbl),
	    sizeof(TLINE) * nl, sizeof(TLINE) * (nl + KT_CHUNCK));
    if(T_L(tbl) == 0) return(-1);

    tbl->t_nl++;
    return(0);
}

TCELL   *T_create_cell(tbl, line)
TBL     *tbl;
TLINE   *line;
{
    int     i, nc = T_NC(tbl);

    line->tl_type = KT_CELL;
    line->tl_val = SW_nalloc(nc * sizeof(TCELL));
    line->tl_graph = B_GRAPHDEFAULT; /* GB 10/03/2011 */
    for(i = 0; i < nc; i++) {
	((TCELL *) line->tl_val + i)->tc_attr = (i > 0) ? KT_DECIMAL : KT_LEFT;
	 /* ((TCELL *) tbl->t_div.tl_val + i)->tc_attr; /* JMP 11-11-93 */
    }

    return((TCELL *) line->tl_val);
}

TCELL   *T_create_title(tbl, line)
TBL     *tbl;
TLINE   *line;
{
    int     i;

    line->tl_type = KT_TITLE;
    line->tl_val = SW_nalloc(sizeof(TCELL));
    ((TCELL *) line->tl_val)->tc_attr = KT_CENTER + KT_BOLD; /* JMP 11-11-93 */

    return((TCELL *) line->tl_val);
}

char    *T_cell_cont(cell, mode)
TCELL   *cell;
int     mode;
{
    char    *buf;

    if(cell->tc_val == NULL) return(""); /* JMP 20-11-93 */
    if(cell->tc_type == KT_LEC)
	return(BUF_strcpy(P_get_ptr(cell->tc_val, 0)));
    buf = BUF_alloc(strlen(cell->tc_val) + 3);
    if(mode) sprintf(buf, "\"%s\"", cell->tc_val);
    else BUF_strcpy(cell->tc_val);
    return(BUF_DATA);
}

T_insert_line(tbl, nbr, type, where)
TBL     *tbl;
int     nbr, type, where;
{
    TLINE   *oline, *nline;

    if(type < 0) return(nbr);
    oline = tbl->t_line;
    nline = (TLINE *)
	SW_nalloc(((T_NL(tbl) + 1)/KT_CHUNCK + 1) * KT_CHUNCK * sizeof(TLINE));

    if(nbr < 0) {
	where = 0;
	nbr = -1;
	}
    if(where == 0) nbr += 1;
    if(T_NL(tbl) > 0) {
	if(nbr > 0)
	    memcpy(nline, oline, nbr * sizeof(TLINE));
	if(T_NL(tbl) > nbr)
	    memcpy(nline + nbr + 1, oline + nbr, (T_NL(tbl) - nbr) * sizeof(TLINE));
    }
    else nbr = 0;

    if(type == KT_CELL)  T_create_cell(tbl, nline + nbr);
    if(type == KT_TITLE) T_create_title(tbl, nline + nbr);

    nline[nbr].tl_type = type;
    tbl->t_line = nline;
    T_NL(tbl)++;
    SW_nfree(oline);
    return(nbr);
}

T_set_lec_cell(cell, lec)
TCELL           *cell;
unsigned char   *lec;
{
    unsigned char   *ptr = 0;

    cell->tc_type = KT_LEC;
    cell->tc_attr = KT_ALIGN(cell->tc_attr, KT_DECIMAL);
    if(K_ipack(&ptr, lec) < 0 && L_errno) {
	kerror(0, "Illegal lec-formula");
	return(-1);
    }
    else {
	T_free_cell(cell);
	cell->tc_val = ptr;
    }
    return(0);
}

void T_set_string_cell(cell, txt)
TCELL           *cell;
unsigned char   *txt;
{
    int     len, attr;

    cell->tc_type = KT_STRING;
/*    cell->tc_attr |= KT_LEFT; /* JMP 11-11-93 */
    attr = cell->tc_attr;
    if(attr & KT_DECIMAL) attr = KT_ALIGN(attr, KT_LEFT);  /* JMP 19-11-93 */
    if(U_is_in('#', txt)) attr = KT_ALIGN(attr, KT_CENTER);  /* JMP 19-11-93 */
    cell->tc_attr = attr;
    if(txt[0] == '\"') txt++;
    len = strlen(txt);
    if(txt[len - 1] == '\"') txt[len - 1] = 0;
    T_free_cell(cell);
    cell->tc_val = SCR_stracpy(txt);
}

/*
T_get_cell_attr(tbl, i, j)
TBL *tbl;
int i, j;
{
    TLINE   *line = tbl->t_line + i;
    TCELL   *cell;

    switch(line->tl_type) {
	case KT_TITLE : if(j > 0) return(KT_BOLD & KT_CENTER);
	case KT_CELL  : break;
	default :       return(KT_BOLD & KT_LEFT);
	}
    cell = (TCELL *)line->tl_val + j;
    return(cell->tc_attr);
}
/* JMP 11-11-93 */

void T_set_cell_attr(tbl, i, j, attr) /* JMP 11-11-93 */
TBL     *tbl;
int     i, j, attr;
{
    TLINE   *line = tbl->t_line + i;
    TCELL   *cell;

    switch(line->tl_type) {
	case KT_TITLE : if(j > 0) return;
	case KT_CELL  : break;
	default :       return;
	}
    cell = (TCELL *)line->tl_val + j;
    cell->tc_attr = attr;
}

T_default(tbl, titg, titls, lecs, mode, files, date)
TBL     *tbl;
char    *titg, **titls, **lecs;
int     mode, files, date;
{
    TLINE   *nline;
    int     i, j;

    if(titg) {
	T_insert_line(tbl, T_NL(tbl) - 1, KT_TITLE, 0);
	SCR_strip(titg);
	if(titg[0])
	    T_set_string_cell((TCELL *)(tbl->t_line[T_NL(tbl) - 1].tl_val), titg);
	T_insert_line(tbl, T_NL(tbl) - 1, KT_LINE, 0);
	}
    T_insert_line(tbl, T_NL(tbl) - 1,  KT_CELL, 0);
    for(j = 1 ; j < T_NC(tbl) ; j++) {
	T_set_string_cell((TCELL *)(tbl->t_line[T_NL(tbl) - 1].tl_val) + j, "\"#S");  /* JMP 24-03-2004 */
	T_set_cell_attr(tbl, T_NL(tbl) - 1, j, KT_CENTER); /* JMP 11-11-93 */
	}
    T_insert_line(tbl, T_NL(tbl) - 1, KT_LINE, 0);

    if(lecs && titls) {
	for(i = 0 ; lecs[i] && titls[i]; i++) {
	    T_insert_line(tbl, T_NL(tbl) - 1,  KT_CELL, 0);
	    T_set_string_cell((TCELL *)(tbl->t_line[T_NL(tbl) - 1].tl_val), titls[i]);
	    for(j = 1 ; j < T_NC(tbl) ; j++)
		T_set_lec_cell((TCELL *)(tbl->t_line[T_NL(tbl) - 1].tl_val) + j, lecs[i]);
	    }
	}
    else T_insert_line(tbl, T_NL(tbl) - 1, KT_CELL, 0);
    if(mode || files || date)
	      T_insert_line(tbl, T_NL(tbl) - 1,  KT_LINE, 0);
    if(mode)  T_insert_line(tbl, T_NL(tbl) - 1,  KT_MODE, 0);
    if(files) T_insert_line(tbl, T_NL(tbl) - 1,  KT_FILES, 0);
    if(date)  T_insert_line(tbl, T_NL(tbl) - 1,  KT_DATE, 0);

    return(0);
}

void T_auto(tbl, def, vars, mode, files, date)
TBL     *tbl;
char    *def, **vars;
int     mode, files, date;
{
    int     i, pos,
	    nb = SCR_tbl_size(vars),
	    nbt = 0, nbl = 0;
    KDB     *kdb = K_WS[K_CMT];
    char    *titg, **titls = NULL, **lecs = NULL;

    pos = K_find(kdb, def);
    if(pos < 0) titg = SCR_stracpy(def);
    else titg = SCR_stracpy(KCVAL(kdb, pos));
    for(i = 0; i < nb; i++) {
	pos = K_find(kdb, vars[i]);
	if(pos < 0)
	    SCR_add_ptr(&titls, &nbt, vars[i]);
	else
	    SCR_add_ptr(&titls, &nbt, KCVAL(kdb, pos));

	SCR_add_ptr(&lecs, &nbl, vars[i]);
    }

    SCR_add_ptr(&titls, &nbt, NULL);
    SCR_add_ptr(&lecs, &nbl, NULL);

    T_default(tbl, titg, titls, lecs, mode, files, date);

    SCR_free(titg);
    SCR_free_tbl(titls);
    SCR_free_tbl(lecs);
}
