#include "iodedos.h"
#include <s_scrold.h>

#define ATTR_DFT    SCR_BLUE
#define ATTR_VT     SCR_BLUE
#define ATTR_REV    SCR_YELLOW
#define ATTR_DIV    19
#define ATTR_B      30
#define ATTR_I      26
#define ATTR_BI     31

SCROLL  vtab_sc;
COLS    *VT_cls;
int     tbl_dim;
int     *VT_val;
extern char   TAB_HLINE[];

char *VT_t(scrl)
SCROLL  *scrl;
{
    TBL     *tbl = (TBL *) DATA;

    sprintf(STATIC_BUF, "Table %s", tbl_name);
    return(STATIC_BUF);
}
/*
char *VT_ct(scrl, i)
SCROLL  *scrl;
int i;
{
    TBL     *tbl = (TBL *) DATA;
    COL     *cl = VT_cls->cl_cols;
    char    *res, *COL_text();

    if(tbl->t_free == 0) cl += 1 + i + i/(T_NC(tbl) - 1);
    else cl += 1 + i;

    res = COL_text(cl, "#y1#P1#N1", 1);
    strcpy(STATIC_BUF, res);
    SCR_free(res);
    SCR_pad(STATIC_BUF, (int)global_VTW);
    U_center_text(STATIC_BUF);
    return(STATIC_BUF);
}
*/
VT_cattr(scrl, i, j)
SCROLL  *scrl;
int i, j;
{
    TBL     *tbl = (TBL *) DATA;

    return(T_getfont(tbl, i, 1, 1));
}

VT_ltattr(scrl, i)
SCROLL  *scrl;
int     i;
{
    TBL     *tbl = (TBL *) DATA;

    return(T_getfont(tbl, i, 0, 1));
}


char *VT_text(scrl, i, j)
SCROLL  *scrl;
int i;
{
    int     v;
    TBL     *tbl = (TBL *) DATA;
    COL     *cl = VT_cls->cl_cols;
    TLINE   *line;
    TCELL   *cell;
    double    *vec = NULL;
    char    *COL_text(), *str;

    line = tbl->t_line + i;
    if(tbl->t_free == 0)
	cl += 1 + j + j/(T_NC(tbl) - 1);
    else
	cl += 1 + j;


    cell = (TCELL *) line->tl_val;
/*  cell += 1;                       /* JMP 28-11-93 */
    cell += 1 + j % (T_NC(tbl) - 1); /* JMP 28-11-93 */

    if(VT_val[i] != 0) vec = (double *) SW_getptr(VT_val[i]);
    v = 1 + j;

    switch(line->tl_type) {
	case KT_LINE  : return(TAB_HLINE);

	case KT_CELL  :
	    if (cell->tc_type == KT_STRING) {
		str = COL_text(cl, T_cell_cont(cell, 0), 2);
		strcpy(STATIC_BUF, str);
		SW_nfree(str);
		}
	    else SCR_fmt_dbl(vec[v], STATIC_BUF, (int)global_VTW, (int)global_VTN);

	    SCR_pad(STATIC_BUF, (int)global_VTW);
	    if(cell->tc_attr & KT_CENTER) U_center_text(STATIC_BUF);
	    else if(cell->tc_attr & (KT_RIGHT | KT_DECIMAL))
		    U_rjust_text(STATIC_BUF);

	    return(STATIC_BUF);

	case KT_TITLE :
	case KT_MODE  :
	case KT_DATE  :
	case KT_FILES :
	default       : return(" ");
    }
}

char *VT_lt(scrl, i)
SCROLL  *scrl;
int i;
{
    TBL     *tbl = (TBL *) DATA;
    TLINE   *line;
    TCELL   *cell;

    line = tbl->t_line + i;
    cell = (TCELL *) line->tl_val;

    switch(line->tl_type) {
    case KT_LINE  :
	return(TAB_HLINE);
    case KT_TITLE :
    tit:
	SCR_strlcpy(STATIC_BUF, T_cell_cont(cell, 0), (int)global_VT0W);
	SCR_pad(STATIC_BUF, (int)global_VT0W);
	if(cell->tc_attr & KT_CENTER) U_center_text(STATIC_BUF);
	if(cell->tc_attr & KT_RIGHT) U_rjust_text(STATIC_BUF);
	return(STATIC_BUF);
    case KT_MODE  :
	return("Mode ");
    case KT_DATE  :
	SCR_long_to_fdate(SCR_current_date(), STATIC_BUF, "dd/mm/yy");
	return(STATIC_BUF);
    case KT_FILES :
	return("Files ");
    case KT_CELL  :
	if(cell->tc_type == KT_STRING) goto tit;
    default :
	return(" ");
    }
}

int VT_nl(scrl)
SCROLL  *scrl;
{
    TBL     *tbl = (TBL *) DATA;

    return((int) T_NL(tbl));
}

int VT_nc(scrl)
SCROLL  *scrl;
{
    TBL     *tbl = (TBL *) DATA;

    return(tbl_dim - 1);
}

int VT_len(scrl, i)
SCROLL  *scrl;
int i;
{
    return((int)global_VTW);
}

VT_adj_width(j, n)
int     j, n;
{
    if(j < 0) {
	global_VT0W += n;
	if(global_VT0W > 56) global_VT0W = 56;
	if(global_VT0W < 5)  global_VT0W = 5;
	LC0 = global_VT0W;
	return(-1);
	}
    global_VTW += n;
    if(global_VTW > 15) global_VTW = 15;
    if(global_VTW < 2)  global_VTW = 2;
    return(-1);
}

int VT_adj_ndec(n)
int     n;
{
    int     vtn = (int)global_VTN;

    vtn += n;
    if(vtn < -1) vtn = -1;
    if(vtn > 6)  vtn = 6;
    global_VTN = vtn;
    return(-1);
}

int VT_ffn(scrl, key, i, j)
SCROLL  *scrl;
int     key, i, j;
{
    switch(key) {
	case SCR_F2   : VT_adj_width(-1, 1); break;
	case SCR_S_F2 : VT_adj_width(-1, -1); break;
	case SCR_F3   : VT_adj_width(j, 1); break;
	case SCR_S_F3 : VT_adj_width(j, -1); break;
	case SCR_F4   : VT_adj_ndec(1); break;
	case SCR_S_F4 : VT_adj_ndec(-1); break;
	case SCR_F5   : VT_files(); break;
    }

    return(-1);
}


VT_calc(lec, dlec, name, val, varnb, t)
char    *lec, *dlec, *name;
double  *val;
int     varnb, t;
{
    char    *tmp;
    CLEC    *clec;
    double  res;

    tmp = SW_nalloc(strlen(lec) + strlen(dlec) + 40);

    /* INVERT */
    /*
    if(strcmp(lec, name) == 0) {        /* JMP 28-10-94
	clec = L_cc(dlec);
	if(clec == 0) goto err;
	L_link(KV_WS, KS_WS, clec);
	res = L_exec(KV_WS, KS_WS, clec, t);
	SW_nfree(clec);
	if(!L_ISAN(res)) goto err;
	res *= *val;
	goto ok;
	}
    */
    sprintf(tmp, "%s := %20.8lg * (%s)", lec, *val, dlec); /* JMP 06-07-94 */
    clec = L_solve(tmp, name);
    if(clec == 0) goto newton;
    if(clec->dupendo) {
	SW_nfree(clec);
	goto newton;
	}
    L_link(KV_WS, KS_WS, clec);
    res = L_exec(KV_WS, KS_WS, clec, t);
    SW_nfree(clec);
    if(L_ISAN(res)) goto ok;

newton:
/*    sprintf(tmp, "(%s) - %lg * (%s)", lec, *val, dlec); /* JMP 09-04-98 */
    sprintf(tmp, "(%s) - %.15lg * (%s)", lec, *val, dlec);   /* JMP 09-04-98 */
    clec = L_cc(tmp);
    if(clec == 0) goto err;
    L_link(KV_WS, KS_WS, clec);
    res = L_zero(KV_WS, KS_WS, clec, t, varnb, varnb);
    SW_nfree(clec);
    if(L_ISAN(res)) goto ok;
    goto err;

ok:
    *val = res;
    SW_nfree(tmp);
    return(0);

err:
    SW_nfree(tmp);
    return(-1);
}

VT_edit(scrl, i, j)
SCROLL  *scrl;
int     i, j;
{
    TBL     *tbl = (TBL *) DATA;
    TLINE   *line;
    TCELL   *cell, *dcell;
    CLEC    *clec;
    ONAME   name;
    char    *lec, *dlec;
    COL     *cl = VT_cls->cl_cols;
    double    *vec = NULL, old;
    int     k, v, varnb = -1, t;
    double  val;

    if(tbl->t_free == 0) cl += 1 + j + j/(T_NC(tbl) - 1);
    else                 cl += 1 + j;
    if(cl->cl_opy != COL_NOP || cl->cl_opf != COL_NOP ||
	cl->cl_fnb[0] != 1) goto rec;
    t = PER_diff_per(&(cl->cl_per[0]), &(KSMPL(KV_WS)->s_p1));

    line = tbl->t_line + i;
    cell = (TCELL *) line->tl_val;
    cell += 1 + j % (T_NC(tbl) - 1); /* JMP 28-11-93 */
    if(line->tl_type != KT_CELL || cell->tc_type != KT_LEC) goto rec;
    dcell = (TCELL *) (tbl->t_div).tl_val;
    dcell += 1 + j % (T_NC(tbl) - 1);
    if(dcell->tc_val) dlec = P_get_ptr(dcell->tc_val, 0);
    else dlec = "1";

    lec = P_get_ptr(cell->tc_val, 0);
    if(strncmp(lec, "0+", 2) == 0) goto rec;
    clec = (CLEC *) P_get_ptr(cell->tc_val, 1);
    for(k = 0 ; k < clec->nb_names ; k++) {
	strcpy(name, clec->lnames[k].name);
	if(L_ISCOEF(name)) continue;
	varnb = K_find(KV_WS, name);
	if(varnb >= 0) break;
	goto rec;
	}
    if(varnb < 0) goto rec;

    if(VT_val[i] != 0) vec = (double *) SW_getptr(VT_val[i]);
    val =  vec[j + 1];

    if(ODE_edit_val(&val, (int)global_VTW)) goto rec;
    ODE_edit_rec_scl_var();
    if(!L_ISAN(val)) goto rec;

    old = *KVVAL(KV_WS, varnb, t);
    if(VT_calc(lec, dlec, name, &val, varnb, t) || !L_ISAN(val)) val = old;
    *KVVAL(KV_WS, varnb, t) = val;
    T_calc_val(tbl, name);
err:
    return(-1);
rec:
/*    SCR_record_key(SCR_CSR_RIGHT); */
    return(-1);
}

YYKEYS VTSCRL_SCMTS[] = {
    "Help",       SCR_F1,
    "Title+",     SCR_F2,
    "Title-",     SCR_S_F2,
    "Cell+",      SCR_F3,
    "Cell-",      SCR_S_F3,
    "NDec+",      SCR_F4,
    "NDec-",      SCR_S_F4,
    "Files",      SCR_F5,
    "�������������",   0,
    "Maximize",   SCR_C_X,
    "Move",       SCR_C_O,
    "Resize",     SCR_C_Z,
    "�������������",   0,
    "Quit",       SCR_ESCAPE,
    0, 0
};


VT_init()
{
    SCRL = &vtab_sc;

    DATA = NULL;
    PL = 3;
    PC = 1;
    NLC = 19;
    NCC = 77;
    LC0 = (int)global_VT0W;
    RLEN = 8;
    NOROT = 1;
    NOHSBAR = 1;
    NOTITC = 1;
    ATTR  = ATTR_VT;
    RATTR  = ATTR_REV;
    SCRL->sc_cmt = "Space=Menu F1=Help F2=Name+ F3=Cell+ F4=NDec+ F5=Files Esc=Quit Enter=Edit";
    SCRL->sc_scmts = VTSCRL_SCMTS;
    SCRL->sc_help = "Print/View Tables";
    SCRL->sc_ftitle = VT_t;
	SCRL->sc_fctitle = NULL; //VT_ct;
    SCRL->sc_fltitle = VT_lt;
    SCRL->sc_fnl = VT_nl;
    SCRL->sc_fnc = VT_nc;
    SCRL->sc_ftext = VT_text;
    SCRL->sc_flen = VT_len;
    SCRL->sc_fedit = VT_edit;
    SCRL->sc_fdel = NULL;
    SCRL->sc_fins = NULL;
    SCRL->sc_ffn = VT_ffn;
    SCRL->sc_fbeg = NULL;
    SCRL->sc_fend = NULL;
    SCRL->sc_fattr = VT_cattr;
    SCRL->sc_flattr = VT_ltattr;

    return(0);
}

VT_scroll(tbl, name)
TBL     *tbl;
char    *name;
{
    SCROLL  *old_sc;

    old_sc = SCRL;
    SCRL = &vtab_sc;
    SCRL->sc_pcl = (char *) tbl;
    SCRL->sc_ncl = 2;               /* JMP 30-05-97 */
    memcpy(tbl_name, name, sizeof(ONAME));

    LC0 = (int)global_VT0W;
    SCRL_edit(SCRL);
    SCRL_deselect_all();
    SCRL = old_sc;
    return(0);
}

int T_tbl_test(tbl, name)
TBL     *tbl;
char    *name;
{
    char    gsmpl[20];
    SAMPLE  *smpl = (SAMPLE *) KDATA(KV_WS);

    sprintf(gsmpl, "%s:%d",
		    PER_pertoa(&(smpl->s_p1), NULL), smpl->s_nb); /* JMP 28-11-93 */
    T_view_tbl(tbl, gsmpl, name);
}

int T_alloc_val(tbl, smpl)
TBL     *tbl;
char    *smpl;
{
    int     i, nl;
    COLS    *cls;
    TLINE   *line;

    tbl_dim = T_prep_cls(tbl, smpl, &cls);
    if(tbl_dim < 0) return(-1);

    nl = T_NL(tbl);
    VT_val = (int *) SW_nalloc(nl * sizeof(int));
    VT_cls = cls;

    for(i = 0; i < nl; i++) {
	line = T_L(tbl) + i;
	if(line->tl_type == KT_CELL) {
	    VT_val[i] = SW_alloc(sizeof(double) * tbl_dim);
	    if(VT_val[i] < 0) return(-1);
	    }
	}
    return(0);
}

T_free_val(tbl)
TBL     *tbl;
{
    int     i, nl = T_NL(tbl);

    COL_free(VT_cls);
    VT_cls = NULL;
    if(VT_val != NULL) {
	for(i = 0; i < nl ; i++) if(VT_val[i] != 0) SW_free(VT_val[i]);
	SW_nfree(VT_val);
    }
    VT_val = NULL;
    return(0);
}

T_name_inline(cell, nc, name)
TCELL   *cell;
int     nc;
char    *name;
{
    int     j, d;
    CLEC    *clec;

    if(cell == 0) return(0);
    if(name == 0) return(1);
    for(d = 0; d < nc; d++) {
	if(cell[d].tc_type != KT_LEC) continue;
	if(cell[d].tc_val == 0) continue;
	clec = (CLEC *) P_get_ptr(cell[d].tc_val, 1);
	for(j = 0 ; j < clec->nb_names ; j++)
	    if(strcmp(clec->lnames[j].name, name) == 0) return(1);
	}
    return(0);
}


T_calc_val(tbl, name)
TBL     *tbl;
char    *name;
{
    int     i, nl = T_NL(tbl);
    TLINE   *line;

    for(i = 0; i < nl; i++) {
	line = T_L(tbl) + i;
	if(line->tl_type == KT_CELL &&
	   (T_name_inline(line->tl_val, T_NC(tbl), name) ||
	    T_name_inline(tbl->t_div.tl_val, T_NC(tbl), name)))
		if(T_calc_line(tbl, i, VT_cls, VT_val) < 0) return(-1);
	}
    return(0);
}

int T_view_tbl(tbl, smpl, name)
TBL     *tbl;
char    *smpl, *name;
{
    int     rc = 0;

    rc = T_alloc_val(tbl, smpl);
    if(rc == 0) {
	rc = T_calc_val(tbl, 0L);
	if(rc == 0) VT_scroll(tbl, name);
	}

    T_free_val(tbl);
    return(rc);
}

int T_calc_line(tbl, i, cls, vals)
TBL     *tbl;
int     i;
COLS    *cls;
int     *vals;
{
    double    *vec;
    int     d, v, j, rc, nc;

    COL_clear(cls);
    if(COL_exec(tbl, i, cls) < 0) return(-1);
    vec = (double *)SW_getptr(vals[i]);

    nc = T_NC(tbl);
    for(j = 0, v = 0; j < cls->cl_nb && v < tbl_dim; j++) {
	d = j % nc;
	if(tbl->t_free == 0 && d == 0 && j != 0) continue;
	vec[v] = (cls->cl_cols + j)->cl_res;
	v++;
    }
    return(0);
}


VT_files()
{
    int     i, nfiles = 0;
	char    **files = NULL, buf[2014]; /* JMP 30/9/2015 */
    MENU    *mn = NULL;
    KDB     *kdb;

    for(i = 0; i < K_MAX_FREF; i++) {
	kdb = K_RWS[VARIABLES][i];
	if(kdb == NULL) continue;

	sprintf(buf, "[%d] %s", i + 1, KNAMEPTR(kdb));
	SCR_add_ptr(&files, &nfiles, buf);
    }
    SCR_add_ptr(&files, &nfiles, NULL);

    mn = MN_auto_create(files, 5, 10, nfiles, 2, SCR_REVERSE);
    MN_edit(mn);

    MN_free(mn);
    SCR_free_tbl(files);

    return(0);
}





