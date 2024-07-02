#include "iodedos.h"
#include <s_scrold.h>
#include <s_mmt.h>
#define ATTR_DFT    SCR_BLUE
#define ATTR_VT     SCR_BLUE
#define ATTR_REV    SCR_YELLOW
#define ATTR_SEL    SCR_WHITE
#define ATTR_DIV    19
#define ATTR_B      30
#define ATTR_I      26
#define ATTR_BI     31

SCROLL  tab_sc;
ONAME   tbl_name;
int     T_SAVE = 0, T_SEL[2];
TBL     *T_BUF = 0;

/* =========== UTILS ============== */

TAB_deselect()
{
    T_SEL[0] = T_SEL[1] = -1;
    return(0);
}

TAB_select(i)
int     i;
{
    if(T_SEL[0] < 0) T_SEL[0] = T_SEL[1] = i;
    else             T_SEL[1] = i;
    return(0);
}

TAB_sel_order(from, to)
int     *from, *to;
{
    if(T_SEL[0] > T_SEL[1]) {
	*from = T_SEL[1];
	*to   = T_SEL[0];
	}
    else {
	*from = T_SEL[0];
	*to   = T_SEL[1];
	}
    return(0);
}

TAB_mark_ins(i, n)
int     i, n;
{
    if(i < T_SEL[0]) T_SEL[0] += n;
    if(i < T_SEL[1]) T_SEL[1] += n;
    return(0);
}

TAB_mark_del(i)
int     i;
{
    if(i < T_SEL[0]) T_SEL[0]--;
    if(i < T_SEL[1]) T_SEL[1]--;
    return(0);
}

TAB_is_sel(i)
int     i;
{
    int     from, to;

    TAB_sel_order(&from, &to);
    if(from < 0) return(0);
    if(i >= from && i <= to) return(1);
    return(0);
}

TAB_copy_lines(tbin, from, to, tbout, pos)
TBL     *tbin, *tbout;
int     from, to, pos;
{
    TLINE   *lin, *lout;
    TCELL   *clin, *clout;
    int     i, j, type;
    char    *tmp;

    for(i = 0 ; i <= to - from  ; i++) {
	lin = tbin->t_line + from + i;
	type = lin->tl_type;
	clin = (TCELL *) lin->tl_val;
	T_insert_line(tbout, pos + i, type, 0);
	lout = tbout->t_line + pos + i + 1;
	clout = (TCELL *) lout->tl_val;
	if(lin) memcpy(lout, lin, sizeof(TLINE));
	lout->tl_val = clout;
	switch(type) {
	    case TABLE_LINE_TITLE :
		tmp = clout->tc_val;
		if(clin) memcpy(clout, clin, sizeof(TCELL));
		clout->tc_val = tmp;
		T_set_string_cell(clout, T_cell_cont(clin, 0));
		break;
	    case TABLE_LINE_CELL  :
		for(j = 0 ; j < T_NC(tbin) && j < T_NC(tbout) ; j++) {
		    tmp = clout[j].tc_val;
		    if(clin) memcpy(clout + j, clin + j, sizeof(TCELL));
		    clout[j].tc_val = tmp;
		    if(clout[j].tc_type == TABLE_CELL_STRING)
			T_set_string_cell(clout + j, T_cell_cont(clin + j, 0));
		    else
			T_set_lec_cell(clout + j, T_cell_cont(clin + j, 0));
		    }
		break;
	    }
	}
    return(0);
}

TAB_buffer(tbl)
TBL     *tbl;
{
    int     from, to;

    TAB_sel_order(&from, &to);
    if(from < 0) return;
    to = min(T_NL(tbl) - 1, to);
    if(T_BUF) T_free(T_BUF);
    T_BUF = T_create(T_NC(tbl));
    TAB_copy_lines(tbl, from, to, T_BUF, -1);
    return(0);
}

TAB_paste(tbl, i)
TBL     *tbl;
int     i;
{
    if(i < -1 || T_BUF == 0) return(0);

    TAB_copy_lines(T_BUF, 0, T_NL(T_BUF) - 1, tbl, i);
    TAB_mark_ins(i, T_NL(T_BUF));
    return(0);

}

TAB_delete(tbl)
TBL     *tbl;
{
    int     i, from, to;

    TAB_sel_order(&from, &to);
    if(from < 0) return;
    to = min(T_NL(tbl) - 1, to);

    TAB_buffer(tbl);
    for(i = from ; i <= to ; i++)
	T_del_line(tbl, from);
    TAB_deselect();
    return(0);
}

TAB_copy(tbl, i)
TBL     *tbl;
int     i;
{
    TAB_buffer(tbl);
    TAB_paste(tbl, i);
    return(0);
}

/* =========== FIN DES FONCTIONS c-L c-U ... ========== */

char    TAB_HLINE[] = "������������������������������������������������������������������";

char *T_get_div(tbl, j)
TBL *tbl;
int j;
{
    char    *ptr = "1.0";
    TCELL   *dcell = (TCELL *)tbl->t_div.tl_val;

    dcell += j;
    if(dcell->tc_val) ptr = T_cell_cont(dcell, 1);
    return(ptr);
}

extern int  ED_quit();

T_mmt_left()
{
    if(ED_CP[1] > 0) {
	ED_CP[1]--;
	return(0);
	}
    return(-1);
}

T_mmt_rec()
{
    SCR_record_key(SCR_LKEY);
    return(-1);
}

MMTFN T_LINE_FNS[] = {
    SCR_ENTER,  ED_quit,    "Quit Line",
    SCR_A_E,    0,         0,
    SCR_F1 ,    T_mmt_rec, 0,
    SCR_F2 ,    T_mmt_rec, 0,
    SCR_F3 ,    T_mmt_rec, 0,
    SCR_F4 ,    T_mmt_rec, 0,
    SCR_F5 ,    T_mmt_rec, 0,
    SCR_F6 ,    T_mmt_rec, 0,
    SCR_F7 ,    T_mmt_rec, 0,
    SCR_F8 ,    T_mmt_rec, 0,
    SCR_F9 ,    T_mmt_rec, 0,
    SCR_F10,    T_mmt_rec, 0,

    SCR_S_F2 ,  T_mmt_rec, 0,
    SCR_S_F3 ,  T_mmt_rec, 0,
    SCR_S_F4 ,  T_mmt_rec, 0,

    SCR_A_F1        , 0L, 0,
    SCR_A_F2        , 0L, 0,
    SCR_A_F3        , 0L, 0,
    SCR_A_F4        , 0L, 0,
    SCR_A_F8        , 0L, 0,
    SCR_A_F9        , 0L, 0,
    SCR_A_F10       , 0L, 0,

    SCR_C_F1,   0,          0,
    SCR_C_F2,   0,          0,
    SCR_C_F3,   0,          0,
    SCR_C_F4,   0,          0,
    SCR_C_F5,   0,          0,
    SCR_C_F6,   0,          0,
    SCR_C_F7,   0,          0,
    SCR_C_F8 ,  0,          0,
    SCR_C_F9 ,  0,          0,
    SCR_C_F10,  0,          0,

    SCR_A_A  ,  0,          0,
    SCR_A_E  ,  0,          0,
    SCR_A_G  ,  0,          0,
    SCR_A_M  ,  0,          0,
    SCR_A_N  ,  0,          0,
    SCR_A_O  ,  0,          0,
    SCR_A_P  ,  0,          0,
    SCR_A_R  ,  0,          0,
    SCR_A_V  ,  0,          0,
    SCR_A_W  ,  0,          0,
    SCR_A_X  ,  0,          0,
    SCR_C_A  ,  0,          0,
    SCR_C_F  ,  0,          0,
    SCR_C_G  ,  0,          0,
    SCR_C_R  ,  0,          0,
    SCR_C_S  ,  0,          0,
    SCR_C_W  ,  0,          0,

    SCR_CSR_LEFT,   T_mmt_left,    "",
    SCR_CSR_UP,     ED_quit,       "Quit",
    SCR_CSR_DOWN,   ED_quit,       "Quit",
    0,              0,              0
};

char *TAB_t(scrl)
SCROLL  *scrl;
{
    TBL     *tbl = (TBL *) scrl->sc_pcl;

    sprintf(STATIC_BUF, "Table %s", tbl_name);
    return(STATIC_BUF);
}

char *TAB_ct(scrl, i)
SCROLL  *scrl;
int i;
{
    TBL     *tbl = (TBL *) scrl->sc_pcl;
    TCELL   *cell = (TCELL *) tbl->t_div.tl_val;

    sprintf(STATIC_BUF, "COL %d : ", i + 1);
    if(i == 0) strcat(STATIC_BUF, "Line Titles");
    else       strcat(STATIC_BUF, "Formulas");
    SCR_pad(STATIC_BUF, TAB_len(scrl, i));
    U_center_text(STATIC_BUF);
    return(STATIC_BUF);
}

char *TAB_lt(scrl, i)
SCROLL  *scrl;
int i;
{
    TBL             *tbl = (TBL *) scrl->sc_pcl;
    TLINE           *line;

    if(i == 0) return("Divis");
    if(i == 1) return(TAB_HLINE);
    i -= 2;
    line = tbl->t_line + i;
    if(line->tl_type == TABLE_LINE_SEP) return(TAB_HLINE);
    switch(line->tl_type) {
	case TABLE_LINE_MODE  : return("Mode");
	case TABLE_LINE_DATE  : return("Date");
	case TABLE_LINE_FILES : return("Files");
	case TABLE_LINE_TITLE : return("Title");
	case TABLE_LINE_CELL  : return("Cells");
/*                       res[0] = 'C';
			res[1] = "LSBM"[line->tl_graph];
			res[2] = 0;
			return(res);
*/
	}
    return(STATIC_BUF);
}

TAB_align_cell(text, cell, j, nb)
unsigned char   *text;
TCELL           *cell;
int             j, nb;
{
    SCR_strlcpy(text, T_cell_cont(cell + j, 1), 160);  /* JMP 02-06-97 */
    return(0);

/*    SCR_pad(text, nb);
    if(cell[j].tc_attr & TABLE_CELL_CENTER) U_center_text(text);
    if(cell[j].tc_attr & TABLE_CELL_RIGHT) U_rjust_text(text);
/* JMP 02-06-97 */
}


TAB_cattr(scrl, i, j)
SCROLL  *scrl;
int i, j;
{
    TBL     *tbl = (TBL *) scrl->sc_pcl;

/*  if(i == 0) return(ATTR_DIV);
    if(i == 1) return(ATTR_DFT);
*/
    if(TAB_is_sel(i - 2)) return(ATTR_SEL);
    return(T_getfont(tbl, i - 2, j, 0));
}

TAB_align(scrl, i, j)
SCROLL  *scrl;
int i, j;
{
    TBL     *tbl = (TBL *) scrl->sc_pcl;

    return(T_getcellalign(tbl, i - 2, j));
}

T_getcellalign(tbl, i, j)
TBL     *tbl;
int     i, j;
{
    TLINE   *line;
    TCELL   *cell;
    int     attr;

    if(i < 0) return(0);
    line = tbl->t_line + i;
    cell = (TCELL *) line->tl_val;
    switch(line->tl_type) {
	case TABLE_LINE_TITLE : if(j > 0) return(0);
	case TABLE_LINE_CELL :
	    if(cell == 0)               return(0);
	    attr = cell[j].tc_attr;
	    if(attr & TABLE_CELL_CENTER)        return(1);
	    else if(attr & TABLE_CELL_RIGHT)    return(2);
	    return(0);
	default : return(0);
	}
}

T_getcellfont(tbl, i, j)
TBL     *tbl;
int     i, j;
{
    TLINE   *line;
    TCELL   *cell;
    int     attr, scrattr = ATTR_DFT;

    if(i < 0) return(0);
    line = tbl->t_line + i;
    cell = (TCELL *) line->tl_val;
    switch(line->tl_type) {
	case TABLE_LINE_TITLE : if(j > 0) return(0);
	case TABLE_LINE_CELL :
	    if(cell == 0)                              return(0);
	    attr = cell[j].tc_attr;
	    if((attr & TABLE_CELL_ITALIC) && (attr & TABLE_CELL_BOLD)) return(3);
	    else if(attr & TABLE_CELL_BOLD)                    return(2);
	    else if(attr & TABLE_CELL_ITALIC)                  return(1);
	    return(0);
	default : return(0);
	}
}

T_getfont(tbl, i, j, type)
TBL     *tbl;
int     i, j, type;
{
    int     attr, scrattr = ATTR_DFT;

    attr = T_getcellfont(tbl, i, j);
    switch(attr) {
	case 1 : scrattr = ATTR_I; break;
	case 2 : scrattr = ATTR_B; break;
	case 3 : scrattr = ATTR_BI; break;
	}
    if(scrattr == ATTR_DFT && type) return(ATTR_VT);
    return(scrattr);
}

/*
T_getfont(tbl, i, j, type)
TBL     *tbl;
int     i, j, type;
{
    TLINE   *line;
    TCELL   *cell;
    int     attr, scrattr = ATTR_DFT;

    if(i < 0) return(scrattr);
    line = tbl->t_line + i;
    cell = (TCELL *) line->tl_val;
    switch(line->tl_type) {
	case TABLE_LINE_TITLE : if(j > 0) break;
	case TABLE_LINE_CELL :
	    if(cell) {
		attr = cell[j].tc_attr;
		if((attr & TABLE_CELL_ITALIC) && (attr & TABLE_CELL_BOLD))
						scrattr = ATTR_BI;
		else if(attr & TABLE_CELL_BOLD)         scrattr = ATTR_B;
		else if(attr & TABLE_CELL_ITALIC)       scrattr = ATTR_I;
		}
	    break;
	}
    if(scrattr == ATTR_DFT && type) return(ATTR_VT);
    return(scrattr);
}
*/

char *TAB_text(scrl, i, j)
SCROLL  *scrl;
int i, j;
{
    TBL     *tbl = (TBL *) scrl->sc_pcl;
    TLINE   *line;
    TCELL   *cell;
    int     rj = j, lg = TAB_len(scrl, j);

    if(i == 0) {
	SCR_strlcpy(STATIC_BUF, T_get_div(tbl, j), lg);
	return(STATIC_BUF);
	}
    if(i == 1) return(TAB_HLINE);

    i -= 2;
    line = tbl->t_line + i;
    cell = (TCELL *) line->tl_val;
    STATIC_BUF[0] = 0;
    switch(line->tl_type) {
	case TABLE_LINE_SEP  : return(TAB_HLINE);
	case TABLE_LINE_MODE  :
	case TABLE_LINE_DATE  :
	case TABLE_LINE_FILES : break;
	case TABLE_LINE_TITLE : if(rj > 0) break;
	case TABLE_LINE_CELL  : TAB_align_cell(STATIC_BUF, cell, rj, lg);
			break;
	}
    return(STATIC_BUF);
}

int TAB_nl(scrl)
SCROLL  *scrl;
{
    TBL     *tbl = (TBL *) scrl->sc_pcl;

    return(2 + T_NL(tbl));
}

int TAB_nc(scrl)
SCROLL  *scrl;
{
    TBL     *tbl = (TBL *) scrl->sc_pcl;

    return(T_NC(tbl));
}

int TAB_len(scrl, i)
SCROLL  *scrl;
int i;
{
    int     nc;

    if(i == 0) nc = scrl->sc_incc /* Windows */ - (int)global_TW - 11;
    else       nc = global_TW;
    if(nc < 0) nc = 5;
    if(nc > scrl->sc_incc /* Windows */ - 16) nc = scrl->sc_incc /* Windows */ - 16;
    return(nc);
}

int TAB_del(scrl, i, j)
SCROLL  *scrl;
int i, j;
{
    TBL     *tbl = (TBL *) scrl->sc_pcl;

    return(TAB_del_qu(tbl, i - 2, 0));
}

int TAB_del_qu(tbl, i, qu)
TBL     *tbl;
int     i, qu;
{
    if(i < 0) return(-1);
    if(qu && ODE_ask(OM_DELETE)) return(-1);
    TAB_mark_del(i);
    return(T_del_line(tbl, i));
}

int TAB_ins(scrl, i, j)
SCROLL  *scrl;
int i, j;
{
    TBL     *tbl = (TBL *) scrl->sc_pcl;

    if(i < 2) return(-1);
    return(T_ins_line(tbl, i - 2));
}

int TAB_ed(scrl, i, j)
SCROLL  *scrl;
int i, j;
{
    return(T_ed_line(scrl, i - 2, j)); /* JMP 11-03-97 */
}

int TAB_disp(scrl, i, j)
SCROLL  *scrl;
int i, j;
{
    TBL     *tbl = (TBL *) scrl->sc_pcl;

    T_attr_msg(tbl, i - 2, j);
    return(0);
}


T_adj_width(scrl, j, n)  /* Windows 19-02-95 */
SCROLL  *scrl;
int     j, n;
{
    global_TW += n;
    if(global_TW > scrl->sc_incc /* Windows */ - 17)  global_TW = scrl->sc_incc /* Windows */ - 17;
    if(global_TW < 5)   global_TW = 5;
    return(-1);
}

int TAB_ffn(scrl, key, i, j)
SCROLL  *scrl;
int     key, i, j;
{
    int     type;
    TBL     *tbl = (TBL *) scrl->sc_pcl;

    switch(key) {
    case SCR_S_F3 :
    case SCR_F2 : T_adj_width(scrl, j, -1); break; /* Windows 19-02-95 */
    case SCR_F3 :
    case SCR_S_F2 : T_adj_width(scrl, j, 1); break;/* Windows 19-02-95 */
    case SCR_F4 : T_attr_tab(tbl, i - 2); break;
    case SCR_F5 : T_attr_cell(tbl, i - 2, j); break;
    case SCR_F6 : T_ed_attr(tbl, i - 2, j); break;
    case SCR_F7 : T_tbl_test(tbl, tbl_name); break;
    case SCR_F8 : T_GraphTest(tbl); break;
    case SCR_F10: T_SAVE = 1; SCRL_end = 1; break;
    case SCR_C_L: TAB_select(i - 2); break;
    case SCR_C_K: TAB_del_qu(tbl, i - 2, 0); break;
    case SCR_C_U: TAB_deselect(); break;
    case SCR_C_D: TAB_delete(tbl); break;
    case SCR_C_C: TAB_copy(tbl, i - 2); break;
    case SCR_C_Y: TAB_buffer(tbl); break;
    case SCR_C_P: TAB_paste(tbl, i - 2); break;
    default     : break;
    }

    return(-1);
}

int TAB_end()
{
    if(T_SAVE == 0 && ODE_ask(OM_SAVE_FIRST) == 0) {
	SCRL_end = 0;
	T_SAVE = 1;
	SCR_record_key(SCR_F10);
	return(-1);
    }
    return(1);
}

YYKEYS TSCRL_SCMTS[] = {
    "Help",       SCR_F1,
    "Title+",     SCR_F2,
    "Title-",     SCR_S_F2,
    "Formulas+",  SCR_F3,
    "Formulas-",  SCR_S_F3,
    "Graph Attrs",SCR_F4,
    "Alignment",  SCR_F5,
    "Font",       SCR_F6,
    "Test",       SCR_F7,
    "Graph",      SCR_F8,
    "Save Table", SCR_F10,
    "�����������������",   0,
    "Modify Cell",SCR_ENTER,
    "Delete Line",SCR_DELETE,
    "New Line",   SCR_INSERT,
    "�����������������",   0,
    "Maximize",   SCR_C_X,
    "Move",       SCR_C_O,
    "Resize",     SCR_C_Z,
    "�����������������",   0,
    "Mark line",  SCR_C_L,
    "Unmark"   ,  SCR_C_U,
    "Copy block", SCR_C_C,
    "Delete block",SCR_C_L,
    "Copy->buffer",SCR_C_Y,
    "Paste Buffer",SCR_C_P,
    "�����������������",   0,
    "Quit",       SCR_ESCAPE,
    0, 0
};

TSCRL_init()
{
    SCRL = &tab_sc;

    DATA = NULL;
    PL = 3;
    PC = 1;
    NLC = 19;
    NCC = 76;
    LC0 = 5;
    RLEN = 8;
    NOROT = 1;
    NOHSBAR = 1;
    ATTR  = ATTR_DFT;
    RATTR  = ATTR_REV;
    SCRL->sc_cmt = "Space=Menu F1=Help F2=Tit+ F3=Forms+ F4=GrAttr F5=Align F6=Font F7=Test F8=Graph";
    SCRL->sc_scmts = TSCRL_SCMTS;
    SCRL->sc_ftitle = TAB_t;
    SCRL->sc_fctitle = TAB_ct;
    SCRL->sc_fltitle = TAB_lt;
    SCRL->sc_fnl = TAB_nl;
    SCRL->sc_fnc = TAB_nc;
    SCRL->sc_ftext = TAB_text;
    SCRL->sc_flen = TAB_len;
    SCRL->sc_fedit = TAB_ed;
    SCRL->sc_fdel = TAB_del;
    SCRL->sc_fins = TAB_ins;
    SCRL->sc_ffn = TAB_ffn;
    SCRL->sc_fbeg = NULL;
    SCRL->sc_fend = TAB_end;
    SCRL->sc_fattr = TAB_cattr;
    SCRL->sc_falign = TAB_align;
    SCRL->sc_fdisp = TAB_disp;

    return(0);
}

TAB_scroll(tbl, name)
TBL     *tbl;
ONAME   name;
{
    SCROLL  *old_sc;

    T_SAVE = 0;
    TAB_deselect();
    old_sc = SCRL;
    SCRL = &tab_sc;
    SCRL->sc_ncl = 1;           /* JMP 30-05-97 */
    SCRL->sc_pcl = (char *) tbl;
    memcpy(tbl_name, name, sizeof(ONAME));

    SCRL_edit(SCRL);
    SCRL_deselect_all();
    SCRL = old_sc;
    return(0);
}

T_ins_line(tbl, nbr)
TBL     *tbl;
int     nbr;
{
    int     type, where, rc;

    if(nbr < 0) return(-1);
    if(ODE_tins(&type, &where) < 0) return(nbr + 2);
    rc = T_insert_line(tbl, nbr, type, where);
    if(rc < 0) return(rc);
    if(where == 1) nbr--;
    TAB_mark_ins(nbr, 1);
    return(rc + 2);
}

T_del_line(tbl, nbr)
TBL     *tbl;
int     nbr;
{
    int     i;
    TLINE   *line = tbl->t_line;
    TCELL   *cell;

    if(nbr < 0 || nbr > T_NL(tbl)) return(-1);

    /* free tl_val of line */
    T_free_line(line + nbr, T_NC(tbl));

    /* shift line + nbr + 1 at pos nbr */
    memcpy(line + nbr, line + (nbr + 1),
	    (T_NL(tbl) - nbr - 1) * sizeof(TLINE));

    T_NL(tbl)--;
    if(T_NL(tbl) > 0) {
	memset(line + T_NL(tbl), 0, sizeof(TLINE));
	if(T_NL(tbl) % KT_CHUNCK == 0)
		//line  = (char *) SW_nrealloc(line,  // JMP 8/4/2015 error !!
		tbl->t_line  = (char *) SW_nrealloc(line, // JMP 8/4/2015 -> corr 
		sizeof(TLINE) * (T_NL(tbl) + KT_CHUNCK), sizeof(TLINE) * T_NL(tbl));
	}
    else {
	SW_nfree(tbl->t_line);
	tbl->t_line = NULL;
	}
    return(nbr + 2);
}


T_ed_getcell(scrl, i, j, cell, type)
SCROLL  *scrl;
int     i, j;
TCELL   **cell;
int     *type;
{
    TBL     *tbl = (TBL *) scrl->sc_pcl; /* JMP 11-03-97 */
    TLINE   *line;

    *cell = 0;
    *type = 0;
    if(i == -2) {
	*type = 1;
	*cell = ((TCELL *)tbl->t_div.tl_val) + j;
	return(0);
	}
    else if(i == -1) return(-1);
    else {
	line = tbl->t_line + i;
	switch(line->tl_type) {
	    case TABLE_LINE_CELL  : *type = 0; break;
	    case TABLE_LINE_TITLE : if(j > 0) return(-1);
			    *type = 2; break;
	    default       : return(-1);
	    }
	*cell = ((TCELL *) line->tl_val) + j;
	}
    return(0);
}

char *T_ed_getcelltxt(scrl, i, j)
SCROLL  *scrl;
int     i, j;
{
    TCELL   *cell;
    int     type;
    char    *val;

    if(T_ed_getcell(scrl, i, j, &cell, &type)) return((char *)0);
    val = T_cell_cont(cell, 1);
    if(val == 0 || val[0] == 0) {
	if(type == 1) val = "1.0";
	else          val = "";
	}
    return(SCR_stracpy(val));
}

T_ed_savecell(scrl, i, j, val)
SCROLL  *scrl;
int     i, j;
char    *val;
{
    TCELL   *cell;
    int     type;

    if(val == 0) return(0);
    if(T_ed_getcell(scrl, i, j, &cell, &type)) return(-1);
    SCR_strip(val);
    if(val[0] && val[0] != '\"') {
	if(type == 2) {
	    kerror(0, "Formula not allowed here");
	    return(-1);
	    }
	if(T_set_lec_cell(cell, val)) return(-1);
	}
    else {
	if(type == 1) {
	    kerror(0, "Text not allowed here");
	    return(-1);
	    }
	T_set_string_cell(cell, val);
	}

    return(0);
}

T_ed_line(scrl, i, j)
SCROLL  *scrl;
int     i, j;
{
    char    *val, *oval;
    int     l, c, lg = TAB_len(scrl, j);

    oval = T_ed_getcelltxt(scrl, i, j);
    if(oval == 0) return(-1);

    SCRL_ccoord(&l, &c);
ag:
    val = T_mmt_edit(oval, l, c, lg, 1);
    SCR_free(oval);
    oval = val;
    if(oval == 0) return(-1);
    if(T_ed_savecell(scrl, i, j, oval)) goto ag;
    SCR_free(oval);
    ODE_edit_rec_scl_var();
    return(-1);
}

/*
T_ed_line(scrl, i, j)
SCROLL  *scrl;
int     i, j;
{
    int     type;
    TBL     *tbl = (TBL *) scrl->sc_pcl;
    TLINE   *line;
    TCELL   *cell;
    char    *val;

    val = T_cell_cont(cell + col, 1);
    if(val == 0 || val[0] == 0) {
	if(type == 1) val = "1.0";
	else          val = "";
	}
    return(SCR_stracpy(val));

    if(i == -2) {
	type = 1;
	cell = (TCELL *)tbl->t_div.tl_val;
	}
    else if(i == -1) return(-1);
    else {
	line = tbl->t_line + i;
	switch(line->tl_type) {
	    case TABLE_LINE_CELL  : type = 0; break;
	    case TABLE_LINE_TITLE : if(j > 0) return(-1);
			    type = 2; break;
	    default       : return(-1);
	    }
	cell = (TCELL *) line->tl_val;
	}
    T_edit_cell(scrl, cell, j, type);
    return(-1);
}

T_edit_cell(scrl, cell, col, type)
SCROLL  *scrl;
TCELL   *cell;
int     col,
	type;               /* type=1 : LEC, 2 : TEXT, 0 : ANY
{
    char    *val, *oval;
    int     l, c, lg = TAB_len(scrl, col);

    val = T_cell_cont(cell + col, 1);
    if(val == 0 || val[0] == 0) {
	if(type == 1) val = "1.0";
	else          val = "";
	}
    oval = SCR_stracpy(val);
    SCRL_ccoord(&l, &c);
ag:
    val = T_mmt_edit(oval, l, c, lg, 1);
    SCR_free(oval);
    oval = val;
    if(oval == 0) return;
    SCR_strip(oval);
    if(oval[0] && oval[0] != '\"') {
	if(type == 2) {
	    kerror(0, "Formula not allowed here");
	    goto ag;
	    }
	if(T_set_lec_cell(cell + col, oval)) goto ag;
	}
    else {
	if(type == 1) {
	    kerror(0, "Text not allowed here");
	    goto ag;
	    }
	T_set_string_cell(cell + col, oval);
	}

    SCR_free(oval);
    ODE_edit_rec_scl_var();
}
*/

T_attr_tab(tbl, i)
TBL     *tbl;
int     i;
{
    TLINE   *line;

    if(i < 0) return(-1);
    line = tbl->t_line + i;
    vktp_tattr_GRAPH = line->tl_graph;
    vktp_tattr_LAXIS  = line->tl_axis;
    vktp_tattr_BOX   = tbl->t_box;
    vktp_tattr_SHAD  = tbl->t_shadow;
    vktp_tattr_XGRID = tbl->t_gridx;
    vktp_tattr_YGRID = tbl->t_gridy;
    vktp_tattr_YMIN  = tbl->t_ymin;
    vktp_tattr_YMAX  = tbl->t_ymax;
    vktp_tattr_ZMIN  = tbl->t_zmin;
    vktp_tattr_ZMAX  = tbl->t_zmax;
    vktp_tattr_AXIS  = tbl->t_axis;
    vktp_tattr_ALIGN = tbl->t_align;

    if(ODE_edit_window(vktp_tattr) < 0) return(-1);

    line->tl_graph = vktp_tattr_GRAPH;
    line->tl_axis = vktp_tattr_LAXIS;
    tbl->t_box  = vktp_tattr_BOX ;
    tbl->t_shadow = vktp_tattr_SHAD;
    tbl->t_gridx = vktp_tattr_XGRID;
    tbl->t_gridy = vktp_tattr_YGRID;
    tbl->t_ymin  = vktp_tattr_YMIN;
    tbl->t_ymax  = vktp_tattr_YMAX;
    tbl->t_zmin  = vktp_tattr_ZMIN;
    tbl->t_zmax  = vktp_tattr_ZMAX;
    tbl->t_axis  = vktp_tattr_AXIS;
    tbl->t_align = vktp_tattr_ALIGN;
}
/*
T_ed_div(tbl, j)
TBL *tbl;
int j;
{
    char    *ptr = "1";
    TCELL   *dcell = (TCELL *)tbl->t_div.tl_val;

    dcell += j;
    if(dcell->tc_val) ptr = T_cell_cont(dcell, 1);
    SCR_strfacpy(&(vktp_div_DIV), ptr);
    vktp_div_N = j + 1;

ag:
    if(ODE_edit_window(vktp_div)) return(-1);
    if(K_ipack(&ptr, vktp_div_DIV) < 0 && L_errno) {
	kerror(0, "Divisor : empty or illegal lec-formula");
	goto ag;
    }
    else {
	T_free_cell(dcell);
	dcell->tc_val = ptr;
    }

    return(0);
}
*/
T_ed_attr(tbl, i, j)
TBL *tbl;
int i, j;
{
    TLINE   *line = tbl->t_line + i;
    TCELL   *cell;
    int     attr;

    if(i < 0) return(-1);
    switch(line->tl_type) {
	case TABLE_LINE_TITLE : if(j > 0) return(-1); /* JMP 11-11-93 */
	case TABLE_LINE_CELL  : break;                /* JMP 11-11-93 */
	default :       return(-1);           /* JMP 11-11-93 */
	}
    cell = (TCELL *)line->tl_val;
    cell += j;

    attr = cell->tc_attr;
    if((attr & TABLE_CELL_BOLD) && (attr & TABLE_CELL_ITALIC)) attr = TABLE_CELL_SETFONT(attr, 0);
    else if(attr & TABLE_CELL_BOLD)        attr = TABLE_CELL_SETFONT(attr, TABLE_CELL_ITALIC);
    else if(attr & TABLE_CELL_ITALIC)      attr = TABLE_CELL_SETFONT(attr, TABLE_CELL_ITALIC | TABLE_CELL_BOLD);
    else attr = TABLE_CELL_SETFONT(attr, TABLE_CELL_BOLD);
    cell->tc_attr = attr;
    return(-1);
}

T_attr_cell(tbl, i, j)
TBL *tbl;
int i, j;
{
    TLINE   *line = tbl->t_line + i;
    TCELL   *cell;
    int     attr;

    if(i < 0) return(-1);
    cell = (TCELL *)line->tl_val;
    cell += j;
    attr = cell->tc_attr;
    if(cell->tc_type == TABLE_CELL_LEC) attr = TABLE_CELL_ALIGN(attr, TABLE_CELL_DECIMAL);
    else if(attr & TABLE_CELL_DECIMAL)  attr = TABLE_CELL_LEFT;

    if(attr & TABLE_CELL_CENTER)        attr = TABLE_CELL_ALIGN(attr, TABLE_CELL_RIGHT);
    else if(attr & TABLE_CELL_RIGHT)    attr = TABLE_CELL_ALIGN(attr, TABLE_CELL_LEFT);
    else if(attr & TABLE_CELL_LEFT)     attr = TABLE_CELL_ALIGN(attr, TABLE_CELL_CENTER);

    cell->tc_attr = attr;
    return(-1);
}


T_attr_msg(tbl, i, j)
TBL *tbl;
int i, j;
{
    TLINE   *line = tbl->t_line + i;
    TCELL   *cell;
    int     attr;
    char    buf[132];

    buf[0] = 0;
    if(i < 0) goto fin;
    cell = (TCELL *)line->tl_val;
    cell += j;
    if(line->tl_type == TABLE_LINE_SEP ||
	line->tl_type == TABLE_LINE_MODE ||
	line->tl_type == TABLE_LINE_DATE ||
	line->tl_type == TABLE_LINE_FILES ) goto fin;
    if(line->tl_type == TABLE_LINE_TITLE && j > 0) goto fin;
    attr = cell->tc_attr;
    sprintf(buf, "[%d, %d] ", i + 1, j + 1);
    if(attr & TABLE_CELL_BOLD)      strcat(buf, "BOLD ");
    if(attr & TABLE_CELL_ITALIC)    strcat(buf, "ITALIC ");
    if(attr & TABLE_CELL_UNDERLINE) strcat(buf, "UNDERLINE ");

    if(attr & TABLE_CELL_CENTER)    strcat(buf, "CENTER ");
    if(attr & TABLE_CELL_DECIMAL)   strcat(buf, "DECIMAL ");
    if(attr & TABLE_CELL_LEFT)      strcat(buf, "LEFT ");
    if(attr & TABLE_CELL_RIGHT)     strcat(buf, "RIGHT ");
fin:
    kmsg(buf);
    return(0);
}

