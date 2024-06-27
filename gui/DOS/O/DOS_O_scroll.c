#include "iodedos.h"
#include <s_scroll.h>
#include <s_scrold.h>
#include <s_yy.h>

SCROLL  std_sc,
/*
	deb_sc,
*/
	**o_sc = NULL;
int     o_nbsc = 0;

extern  int ODE_sc_end();
extern  int *KSIM_ORDER, *KSIM_POSXK;
extern  KDB *KSIM_DBV;

extern  int     ODE_VIEW;
extern  char    ODE_SMPL[81];

/*********** SELECTION OF VARIABLES **********************/

int     V_LSEL[2];
int     V_CSEL[2];
VAR_deselect()
{
    V_LSEL[0] = V_LSEL[1] = -1;
    V_CSEL[0] = V_CSEL[1] = -1;
    return(0);
}

VAR_select(i, j)
int     i, j;
{
    if(V_LSEL[0] < 0) {
	V_LSEL[0] = V_LSEL[1] = i;
	V_CSEL[0] = V_CSEL[1] = j;
	}
    else {
	V_LSEL[1] = i;
	V_CSEL[1] = j;
	}
    return(0);
}

VAR_sel_order(froml, tol, fromc, toc)
int     *froml, *tol;
int     *fromc, *toc;
{
    if(V_LSEL[0] > V_LSEL[1]) {
	*froml = V_LSEL[1];
	*tol   = V_LSEL[0];
	}
    else {
	*froml = V_LSEL[0];
	*tol   = V_LSEL[1];
	}
    if(V_CSEL[0] > V_CSEL[1]) {
	*fromc = V_CSEL[1];
	*toc   = V_CSEL[0];
	}
    else {
	*fromc = V_CSEL[0];
	*toc   = V_CSEL[1];
	}
    return(0);
}

VAR_is_sel(i, j)
int     i, j;
{
    int     froml, tol, fromc, toc;

    VAR_sel_order(&froml, &tol, &fromc, &toc);
    if(froml < 0) return(0);
    if(i >= froml && i <= tol && j >= fromc && j <= toc) return(1);
    return(0);
}

/************* FIN SELECT ***********************/

char *ODE_t(scrl)
SCROLL  *scrl;
{
    KDB     *kdb = (KDB *) scrl->sc_pcl; /* JMP 10-02-97 */
    extern char *KLG_MODES[][3];

    sprintf(STATIC_BUF, "%s [%d objects]",
	 ((KNAMEPTR(kdb) != 0) ? KNAMEPTR(kdb) : "Noname"), (int) KNB(kdb));

    if(KTYPE(kdb) == K_VAR) {
	strcat(STATIC_BUF, " - ");
	strcat(STATIC_BUF, KLG_MODES[global_VM][0]);
	}
    return(STATIC_BUF);
}

char *ODE_ct(scrl, i)
SCROLL  *scrl;
int i;
{
    static char scl[][9] = { "VALUE", "RELAX", "STD ERR", "T-STAT" };
    int         len = ODE_len(scrl, i), type;
    KDB         *kdb = (KDB *) scrl->sc_pcl; /* JMP 10-02-97 */

    type = KTYPE(kdb);
    switch(type) {
	case K_VAR :
	    PER_pertoa(PER_addper(&(KSMPL(kdb)->s_p1), i), STATIC_BUF);
	    //strcpy(STATIC_BUF, STATIC_BUF + 2);
	    break;
	case COMMENTS :strcpy(STATIC_BUF, "COMMENT");     break;
	case K_EQS :strcpy(STATIC_BUF, "EQUATION");    break;
	case K_IDT :strcpy(STATIC_BUF, "IDENTITY");    break;
	case K_LST :strcpy(STATIC_BUF, "LIST");        break;
	case K_SCL :strcpy(STATIC_BUF, scl[i]);        break;
	case K_TBL :strcpy(STATIC_BUF, "TABLE TITLE"); break;
	}
    if((type == K_VAR && !(scrl->sc_ir)) || type == K_SCL) { /* JMP 10-02-97 */
	SCR_pad(STATIC_BUF, len);
	U_center_text(STATIC_BUF);
	}
    return(STATIC_BUF);
}

char *ODE_lt(scrl, i)
SCROLL  *scrl;
int i;
{
    KDB *kdb;

    kdb = (KDB *) scrl->sc_pcl; /* JMP 10-02-97 */
/*
    if(SCRL == &deb_sc) {
	return(KONAME(KSIM_DBV, KSIM_POSXK[KSIM_ORDER[i]]));
    }
    else  return(KONAME(kdb, i));
*/
    if(KNB(kdb) <= i) return((char *)0); /* JMP 22-06-98 */
    else return(KONAME(kdb, i));         /* JMP 22-06-98 */
}
/* now in b_print.c */
//unsigned char *T_get_title(tbl)     /* JMP 13-11-93 */
//TBL     *tbl;
//{
//    int     k;
//
//    for(k = 0; k < T_NL(tbl); k++)
//        if(tbl->t_line[k].tl_type == KT_TITLE) break;
//    if(k == T_NL(tbl) ||
//       ((TCELL *) tbl->t_line[k].tl_val)->tc_val == 0)
//            strcpy(STATIC_BUF, "No title");
//    else
//        SCR_strlcpy(STATIC_BUF, (char *) ((TCELL *) tbl->t_line[k].tl_val)->tc_val, STATIC_BUF_LG);
//
//    return(STATIC_BUF);
//}

char *ODE_text(scrl, i, j)
SCROLL  *scrl;
int i, j;
{
    KDB             *kdb = (KDB *) scrl->sc_pcl; /* JMP 10-02-97 */
    SCL             *scl;
    double          var;
    TBL             *tbl;
    int             k;

    switch(KTYPE(kdb)) {
    case K_VAR :
	var = KV_get(kdb, i, j, (int)global_VM);
	SCR_fmt_dbl(var, STATIC_BUF, (int)global_VW, (int)global_VN);
	return(STATIC_BUF);

    case K_IDT :
	    return(KILEC(kdb, i));

    case K_EQS :
    /*
	    if(SCRL == &deb_sc)
		return(KELEC(kdb, KSIM_ORDER[i]));
	    else
		return(KELEC(kdb, i));
    */
	    return(KELEC(kdb, i));

    case COMMENTS :
	    return(KCVAL(kdb, i));

    case K_LST :
	    return(KLVAL(kdb, i));

    case K_TBL :
	    tbl = KTVAL(kdb, i);
/*            if(j == 0) sprintf(STATIC_BUF, "%d", tbl->t_nc); */
	    T_get_title(tbl);     /* JMP 13-11-93 */
	    T_free(tbl);
	    return(STATIC_BUF);

    case K_SCL :
	    scl = (SCL *) KSVAL(kdb, i);
	    switch(j) {
		case 0 : var = scl->val; break;
		case 1 : var = scl->relax; break;
		case 2 : var = scl->std; break;
		case 3 : if(L_ISAN(scl->val)
			    && L_ISAN(scl->std)
				&& !L_IS0(scl->std))
				    var = scl->val/scl->std;
			 else var = L_NAN;
		}

	    SCR_fmt_dbl(var, STATIC_BUF, (int)global_SW, (int)global_SN);
	    return(STATIC_BUF);
    }
}

int ODE_nl(scrl)
SCROLL  *scrl;
{
    KDB *kdb;

    kdb = (KDB *) scrl->sc_pcl; /* JMP 10-02-97 */
    return((int) KNB(kdb));
}

int ODE_nc(scrl)
SCROLL  *scrl;
{
    KDB *kdb = (KDB *) scrl->sc_pcl; /* JMP 10-02-97 */

    switch(KTYPE(kdb)) {
	case K_VAR :  return((int) KSMPL(kdb)->s_nb);
	case K_SCL :  return(4);
	default    :  return(1);
    }
}

int ODE_len(scrl, i)
SCROLL  *scrl;
int i;
{
    KDB *kdb = (KDB *) scrl->sc_pcl;  /* JMP 10-02-97 */

    switch(KTYPE(kdb)) {
	case COMMENTS :
	case K_EQS :
	case K_IDT :
	case K_TBL :
	case K_LST :
		    // return(65); /* JMP 09-01-11 */
		    return(SCR_PAGE_SIZE[1] - 10); /* JMP 09-01-11 */
	case K_SCL : return((int)global_SW);
	case K_VAR : return((int)global_VW);
	}
}

int ODE_del(scrl, i, j)
SCROLL  *scrl;
int i, j;
{
    KDB     *kdb = (KDB *) scrl->sc_pcl; /* JMP 10-02-97 */

    VAR_deselect();
    if(ODE_VIEW) return(-1);

    if(ODE_ask(OM_DELETE) != 0) return(-1);

    if(kdb == K_WS[KTYPE(kdb)]) K_del(kdb, i);
    else {
	K_del(K_WS[KTYPE(kdb)], K_find(K_WS[KTYPE(kdb)], KONAME(kdb, i)));
	K_del_entry(kdb, i);
    }
    return(-1);
}

int ODE_ins(scrl, i, j)
SCROLL  *scrl;
int i, j;
{
    KDB         *kdb = (KDB *) scrl->sc_pcl; /* JMP 10-02-97 */
    int         rc;

    VAR_deselect();
    if(ODE_VIEW) return(-1);

    switch(KTYPE(kdb)) {
	case COMMENTS :
	case K_EQS :
	case K_IDT :
	case K_LST :
	case K_VAR :  rc = ODE_edit_obj1(kdb, -1); break;
	case K_SCL :  rc = ODE_edit_scl(kdb, -1, -1); break;
	case K_TBL :  rc = ODE_edit_tbl(kdb, -1, -1); break;
	default    :  return(-1);
	}
#ifndef SCR4W32 /* JMP 10-02-97 */
    SCR_record_key(SCR_C_X);
#endif
    return(rc);
}

int ODE_ed(scrl, i, j)
SCROLL  *scrl;
int i, j;
{
    KDB         *kdb = (KDB *) scrl->sc_pcl;  /* JMP 10-02-97 */

    if(ODE_VIEW) {
	if(KTYPE(kdb) == K_TBL)
	    switch(ODE_VIEW) {
	    case 1: B_ViewPrintTbl_1(KONAME(kdb, i), ODE_SMPL); break;
	    case 2: B_ViewPrintGr_1(KONAME(kdb, i), ODE_SMPL); break;
	    }
	return(-1);
    }

    switch(KTYPE(kdb)) {
    case COMMENTS :
    case K_EQS :
    case K_IDT :
    case K_LST :  return(ODE_edit_obj1(kdb, i));
    case K_SCL :  return(ODE_edit_scl_cell(kdb, i, j));
    case K_TBL :  return(ODE_edit_tbl(kdb, i, -1));
    case K_VAR :  return(ODE_edit_var_cell(kdb, i, j));
    default    :  return(-1);
    }

}

ODE_adj_swidth(n)
int     n;
{
    global_SW += n;
    if(global_SW < 2)  global_SW = 2;
    if(global_SW > 12) global_SW = 12;
    return(0);
}

ODE_adj_sndec(n)
int     n;
{
    global_SN += n;
    if(global_SN < -1)  global_SN = -1;
    if(global_SN > 6)   global_SN = 6;
    return(0);
}

ODE_adj_vndec(n)
int     n;
{
    global_VN += n;
    if(global_VN < -1)  global_VN = -1;
    if(global_VN > 6)   global_VN = 6;
    return(0);
}

ODE_adj_vwidth(scrl, n)
SCROLL  *scrl;                  /* Windows 10-02-95 */
int     n;
{
    global_VW += n;
    if(global_VW < 2)  global_VW = 2;
    if(global_VW > 15) global_VW = 15;
/*    RLEN = (int)global_VW; /* Windows 10-02-95 */
    scrl->sc_irlen = (int)global_VW; /* Windows 10-02-95 */
    return(0);
}

ODE_adj_title(scrl, n)         /* Windows 10-02-95 */
SCROLL  *scrl;
int     n;
{
    scrl->sc_ilc0 += n;
    if(scrl->sc_ilc0 < 2)  scrl->sc_ilc0 = 2;
    if(scrl->sc_ilc0 > K_MAX_NAME) scrl->sc_ilc0 = K_MAX_NAME; /* IODE64K */
    global_LC0 = scrl->sc_ilc0; /* JMP 10-03-11 */
    return(0);
}
ODE_attr(scrl, i, j)
SCROLL  *scrl;
int     i, j;
{
    KDB     *kdb = (KDB *) scrl->sc_pcl; /* JMP 10-02-97 */

    if(KTYPE(kdb) == K_SCL) {
	switch(j) {
	    case 0 : return(SCR_BLUE);
	    case 1 : return(30);
	    default: return(31);
	    }
	}
    else if(KTYPE(kdb) == K_VAR) {
	if(VAR_is_sel(i, j)) return(SCR_RED);
	}
    return(SCR_BLUE);
    // return(SCR_CYAN);
}

ODE_ffn(scrl, key, i, j)
SCROLL  *scrl;
int     key, i, j;
{
    int     type;
    KDB     *kdb;
    TBL     *tbl;
    char    buf[256], per1[20], per2[20];  /* JMP 07-01-2011 */
    ONAME   tbl_name;

    type  = key % 10;
    kdb = scrl->sc_pcl; /* JMP 10-02-97 */
    switch(key) {
	case SCR_F2   : ODE_adj_title(scrl, 1);  break; /* JMP 10-02-97 */
	case SCR_S_F2 : ODE_adj_title(scrl, -1); break; /* JMP 10-02-97 */
	case SCR_F3  :
	    if(KTYPE(kdb) == K_VAR) ODE_adj_vwidth(scrl, 1); /* JMP 10-02-97 */
	    if(KTYPE(kdb) == K_SCL) ODE_adj_swidth(1);
	    break;
	case SCR_S_F3  :
	    if(KTYPE(kdb) == K_VAR) ODE_adj_vwidth(scrl, -1);/* JMP 10-02-97 */
	    if(KTYPE(kdb) == K_SCL) ODE_adj_swidth(-1);
	    break;
	case SCR_F4  :
	    if(KTYPE(kdb) == K_VAR) ODE_adj_vndec(1);
	    if(KTYPE(kdb) == K_SCL) ODE_adj_sndec(1);
	    break;
	case SCR_S_F4  :
	    if(KTYPE(kdb) == K_VAR) ODE_adj_vndec(-1);
	    if(KTYPE(kdb) == K_SCL) ODE_adj_sndec(-1);
	    break;
	case SCR_F5  :
	    if(KTYPE(kdb) == K_VAR) global_VM = (global_VM + 1) % 5;
	    break;
	case SCR_S_F5  :
	    if(KTYPE(kdb) == K_VAR) {
		if(global_VM == 0) global_VM = 4;
		else global_VM--;
		}
	    break;
	case SCR_F6  :
	    if(KTYPE(kdb) == K_VAR) global_PREC = (global_PREC == 8) ? 15 : 8; /* JMP 29-04-98 */
	    break;
	case SCR_F7  :
	    if(KTYPE(kdb) == K_TBL)
		B_ViewPrintTbl_1(KONAME(kdb, i), ODE_SMPL);
	    if(KTYPE(kdb) == K_IDT) {
		sprintf(buf, "%s %s %s",
			PER_pertoa(&(KSMPL(K_WS[K_VAR])->s_p1), per1),
			PER_pertoa(&(KSMPL(K_WS[K_VAR])->s_p2), per2),
			KONAME(kdb, i));
		if(B_IdtExecute(buf)) B_display_last_error();
		else ODE_error(OM_EXEC_ACH);
	    }
	    break;
	case SCR_S_F7  :
	    if(KTYPE(kdb) == K_IDT) SB_IdtExecute();
	    break;
	case SCR_F8  :
	    if(KTYPE(kdb) == K_VAR) C_AutoEditGraph(KONAME(kdb, i), global_VM);
	    if(KTYPE(kdb) == K_TBL) B_ViewPrintGr_1(KONAME(kdb, i), ODE_SMPL);
	    break;

	case SCR_S_F8  :
	    if(KTYPE(kdb) == K_VAR) SB_DataEditGraph();  /* JMP 31-03-99 */
	    break;

	case SCR_S_F9  :            /* Windows JMP 10-02-97 */
	    if(KTYPE(kdb) == K_VAR) scrl->sc_ir = (scrl->sc_ir + 1) % 2;
	    break;

	case SCR_A_F1:
	case SCR_A_F2:
	case SCR_A_F3:
	case SCR_A_F4:
	case SCR_A_F5:
	case SCR_A_F6:
	case SCR_A_F7:
	    ODE_doc(kdb, i, type - 1);
	    break;

	case SCR_C_V:
	    if(KTYPE(kdb) == K_VAR) ODE_settext(kdb, i, j);
	    break;
	case SCR_C_P:
	    if(KTYPE(kdb) == K_VAR) ODE_importtext(kdb, i, j);
	    break;
	case SCR_C_L:
	    if(KTYPE(kdb) == K_VAR) VAR_select(i, j);
	    break;
	case SCR_S_CSR_LEFT:
	case SCR_S_CSR_RIGHT:
	case SCR_S_CSR_UP:
	case SCR_S_CSR_DOWN:
	case SCR_S_CSR_PG_UP:
	case SCR_S_CSR_PG_DN:
	case SCR_S_CSR_HOME:
	case SCR_S_CSR_END:
	    SCR_record_key(SCR_C_L);      /* JMP 10-10-98 */
	    SCR_record_key(key - 100);      /* JMP 10-10-98 */
	    SCR_record_key(SCR_C_L);      /* JMP 10-10-98 */
	    break;
	case SCR_C_C:
	    if(KTYPE(kdb) == K_VAR) ODE_ClipCopy(kdb);
	    break;
	case SCR_C_U:
	    if(KTYPE(kdb) == K_VAR) VAR_deselect();
	    break;

	}

    return(-1);
}

/*
ODE_fdfn(scrl, key, i, j)
SCROLL  *scrl;
int     key, i, j;
{
    int     line, type;
    KDB     *kdb;

    if(!IR) line = i;
    else line = j;

    switch(key) {
    case SCR_HELP_KEY : HLP_edit("simulation du mod�le"); return(-1);
    case SCR_F7 : return(ODE_exec_c());
    case SCR_F8 : return(ODE_exec_a());
    }

    type  = key % 10;
    kdb = scrl->sc_pcl;
    if(SCR_A_F1 <= key && SCR_A_F10 >= key)
	    ODE_doc(kdb, KSIM_ORDER[i], type - 1);
    return(-1);
}
*/

int ODE_bfn(scrl, l, c)
SCROLL  *scrl;
int     *l, *c;
{
    KDB *kdb;

    kdb = scrl->sc_pcl; /* JMP 10-02-97 */
    if(KTYPE(kdb) == K_VAR) *c = global_VS;
    if(KTYPE(kdb) == K_TBL && ODE_SMPL[0] == '\0') {
	sprintf(ODE_SMPL, "%s:%d", PER_pertoa(&(KSMPL(KV_WS)->s_p1), NULL), KSMPL(KV_WS)->s_nb);
    }
    return(0);
}

int ODE_efn(scrl, l, c)
SCROLL  *scrl;
int     l, c;
{
    KDB *kdb;

    kdb = scrl->sc_pcl;  /* JMP 10-02-97 */
    if(KTYPE(kdb) == K_VAR) global_VS = c;
    return(0);
}

YYKEYS OSCRL_GCMTS[] = {
    "Help",       SCR_F1,
    "Name width+",SCR_F2,
    "Name width-",SCR_S_F2,
    "Modify",     SCR_ENTER,
    "Delete",     SCR_DELETE,
    "New",        SCR_INSERT,
    "����������������",   0,
    "Max",        SCR_C_X,
    "Move",       SCR_C_O,
    "Resize",     SCR_C_Z,
    "����������������",   0,
    "View Cmts",  SCR_A_F1,
    "View Eqs",   SCR_A_F2,
    "View Idts",  SCR_A_F3,
    "View Cmts",  SCR_A_F4,
    "View Scls",  SCR_A_F5,
    "View Vars",  SCR_A_F7,
    "����������������",   0,
    "Quit",       SCR_ESCAPE,
    0, 0
};

YYKEYS OSCRL_TABSCMTS[] = {
    "Help",       SCR_F1,
    "Name width+",SCR_F2,
    "Name width-",SCR_S_F2,
    "Test table", SCR_F7,
    "Graphic",    SCR_F8,
    "Modify",     SCR_ENTER,
    "Delete",     SCR_DELETE,
    "New",        SCR_INSERT,
    "����������������",   0,
    "Max",        SCR_C_X,
    "Move",       SCR_C_O,
    "Resize",     SCR_C_Z,
    "����������������",   0,
    "Quit",       SCR_ESCAPE,
    0, 0
};

YYKEYS OSCRL_SCMTS[] = {
    "Help",       SCR_F1,
    "Name width+",SCR_F2,
    "Name width-",SCR_S_F2,
    "Cell width+",SCR_F3,
    "Cell width-",SCR_S_F3,
    "NDec+",      SCR_F4,
    "NDec-",      SCR_S_F4,
    "����������������",   0,
    "Modify",     SCR_ENTER,
    "Delete",     SCR_DELETE,
    "New",        SCR_INSERT,
    "����������������",   0,
    "Maximize",   SCR_C_X,
    "Move",       SCR_C_O,
    "Resize",     SCR_C_Z,
    "����������������",   0,
    "Quit",       SCR_ESCAPE,
    0, 0
};

YYKEYS OSCRL_SIDTS[] = {
    "Help",       SCR_F1,
    "Name width+",SCR_F2,
    "Name width-",SCR_S_F2,
    "Exec Current", SCR_F7,
    "Execute list", SCR_S_F7,
    "����������������",   0,
    "Modify",     SCR_ENTER,
    "Delete",     SCR_DELETE,
    "New",        SCR_INSERT,
    "����������������",   0,
    "Maximize",   SCR_C_X,
    "Move",       SCR_C_O,
    "Resize",     SCR_C_Z,
    "����������������",   0,
    "Quit",       SCR_ESCAPE,
    0, 0
};

YYKEYS OSCRL_VARSCMTS[] = {
    "Help",       SCR_F1,
    "Name+",      SCR_F2,
    "Name-",      SCR_S_F2,
    "Wdth+",      SCR_F3,
    "Wdth-",      SCR_S_F3,
    "NDec+",      SCR_F4,
    "NDec-",      SCR_S_F4,
    "Mode+",      SCR_F5,
    "Mode-",      SCR_S_F5,
    "Precision",  SCR_F6,
    "Graph",      SCR_F8,
    "Graphic Par",SCR_S_F8,
    "Rotate",     SCR_F9,           /* Windows JMP 10-02-97 */
    "���������������",   0,
    "Modify",     SCR_ENTER,
    "Delete",     SCR_DELETE,
    "New",        SCR_INSERT,
    "���������������",   0,
    "Maximize",   SCR_C_X,
    "Move",       SCR_C_O,
    "Resize",     SCR_C_Z,
    "Rotate",     SCR_C_R,
    "���������������",   0,
    "Quit",       SCR_ESCAPE,
    0, 0
};

OSCRL_init()
{
    SCRL = &std_sc;

    VAR_deselect();
    DATA = NULL;
    NLC = 24;   /* JMP 20-10-2003 */
    NCC = 86;  /* JMP 20-10-2003 */
    LC0 = global_LC0;           /* JMP 10-03-11 */
    LC0 = max(LC0, 5);          /* JMP 10-03-11 */
    LC0 = min(LC0, K_MAX_NAME); /* JMP 10-03-11 */
    RLEN = (int)global_VW;/* JMP 27-11-93 */
    ATTR  = SCR_BLUE;
//    ATTR  = SCR_CYAN;
    RATTR  = SCR_REVERSE;
    SCRL->sc_ftitle = ODE_t;
    SCRL->sc_fctitle = ODE_ct;
    SCRL->sc_fltitle = ODE_lt;
    SCRL->sc_fnl = ODE_nl;
    SCRL->sc_fnc = ODE_nc;
    SCRL->sc_ftext = ODE_text;
    SCRL->sc_flen = ODE_len;
    SCRL->sc_fedit = ODE_ed;
    SCRL->sc_fdel = ODE_del;
    SCRL->sc_fins = ODE_ins;
    SCRL->sc_ffn = ODE_ffn;
    SCRL->sc_fbeg = ODE_bfn;
    SCRL->sc_fend = ODE_efn;
    SCRL->sc_cmt = 0;
    SCRL->sc_scmts = OSCRL_GCMTS;
    SCRL->sc_fattr = ODE_attr;

    return(0);
}

ODE_scroll(kdb, lst)
KDB     *kdb;
char    **lst;
{
    static  int ncc[7] = {86, 86, 86, 86, 76, 86, 86};  /* JMP 20-10-2003 */
    static  int pcc[7]  = {1, 1, 1, 1, 7, 1, 1};      /* JMP 16-11-93 */
    int     pl = 3, pc, sim = 0, type;
    KDB     *t_kdb = NULL;
    SCROLL  *scrl;
    extern int OE_CF;


    ncc[0] = ncc[1] = ncc[2] = ncc[3] = ncc[5] = ncc[6] = SCR_PAGE_SIZE[1] - 4;
    type = KTYPE(kdb);
ag:
    if(type == K_VAR && KSMPL(kdb)->s_nb == 0) {
	if(SCR_confirme("Sample undefined. Set it ? (Y/N)")) return(-1);
	SB_WsSample();
	goto ag;
	}

    o_sc = SW_nrealloc(o_sc, o_nbsc * sizeof(SCROLL *),
		  (o_nbsc + 1) * sizeof(SCROLL *));

    scrl = SW_nalloc(sizeof(SCROLL));
    if(scrl == 0 || o_sc == NULL) kerror(1, "Allocation error in scroll");
    o_sc[o_nbsc] = scrl;

    pc = pcc[type];
    if(o_nbsc != 0) {
	SCRL = o_sc[o_nbsc - 1];
	pl = SCRL->sc_ipl + SCRL->sc_icl + 4;
	pl = min(15, pl);
	//pc = SCRL->sc_ipc + SCRL->sc_icc; /* JMP 14-01-11 */
	pc = SCRL->sc_ipc;  /* JMP 14-01-11 */
    }
    if(OE_CF) {  /* COEFF DANS EQUATION */
	pl = 11;
	pc = 6;
	}

    SCRL = scrl;
    o_nbsc++;

    memcpy(SCRL, &std_sc, sizeof(SCROLL));
    SCRL->sc_ipl = pl;
    SCRL->sc_ipc = pc;
    SCRL->sc_incc = ncc[type];
    SCRL->sc_inlc = SCR_PAGE_SIZE[0] - 6; /* JMP 03-02-11 */
    switch(type) {
	case  K_IDT :
	    SCRL->sc_cmt = "Space=Menu F1=Help F2=Name+ F3=Cell+ F7=s-F7=Exec Enter=Edit Ins=Add Del";
	    SCRL->sc_scmts = OSCRL_SIDTS;
	    NOROT   = 1;
	    NOHSBAR = 1;
	    break;
	case  K_SCL :
	    SCRL->sc_cmt = "Space=Menu F1=Help F2=Name+ F3=Cell+ F4=NDec+ Esc=Quit Enter=Edit Ins=Add Delete";
	    SCRL->sc_scmts = OSCRL_SCMTS;
	    NOROT   = 1;
	    NOHSBAR = 1;
	    break;
	case K_VAR :
	    SCRL->sc_cmt = "Space=Menu F1=Help F2=Name+ F3=Cell+ F4=Ndec+ F5=Mode F6=Prec F8=Graph Ins=New";
	    SCRL->sc_scmts = OSCRL_VARSCMTS;
	    NOHSBAR = 1;
	    break;
	case K_TBL :
	    SCRL->sc_cmt = "Space=Menu F1=Hlp F2=Name+ F7=Test F8=Graph Esc=Quit Enter=Edit Ins=Add Delete";
	    SCRL->sc_scmts = OSCRL_TABSCMTS;
	    NOROT   = 1;
	    NOHSBAR = 1;
	    break;
	default :
	    SCRL->sc_cmt = "Space=Menu F1=Hlp F2=Name+ Esc=Quit Enter=Edit Ins=Add Delete aFn=Objs";
	    SCRL->sc_scmts = OSCRL_GCMTS;
	    NOROT   = 1;
	    NOHSBAR = 1;
	    break;
	}

    if(lst == NULL || lst[0] == 0) SCRL->sc_pcl = (char *) kdb;
    else {
	t_kdb = K_refer(kdb, A_nb(lst, 0) + 1, lst);
	SCRL->sc_pcl = (char *) t_kdb;
	SCRL->sc_ncl = 0;               /* JMP 30-05-97 */
    }

//    if(type != K_SCL) SCR_record_key(SCR_C_X); /* JMP 09-01-11 */
    SCRL_edit(SCRL);
    SCRL_deselect_all();
    if(t_kdb != NULL) K_free_kdb(t_kdb);

    SW_nfree(scrl);
    o_nbsc--;
    if(o_nbsc > 0) {
	SCRL = o_sc[o_nbsc - 1];
	}
    else {
	o_nbsc = 0;
	SW_nfree(o_sc);
	o_sc = SCRL = NULL;
    }
}


/**************************************************************\
*                           CLIPBOARD                          +
\**************************************************************/


ODE_settext(kdb, line, col)
KDB     *kdb;
int     line, col;
{
    U_ch    *txt, **tres, **tline;
    int     i, j;
    double  val;

    txt = WscrClipGet();
    if(txt == 0) return;
    IodeDdeUnLocale(txt);  // Inversion des lignes pour traiter le currency sign en ANSI (avant OEM) JMP 27/1/2017
    SCR_AnsiToOem(txt, txt);
    
    SCR_asqz(txt, "\t\r\n-."); // JMP 27/1/2017
    tres = SCR_vtom(txt, '\n');
    if(tres == 0) {
	SCR_free(txt);
	return(0);
	}
    for(i = 0 ; tres[i] ; i++) {
	if(i + line >= KNB(kdb)) break;
	//tline = SCR_vtoms(tres[i], " \t");
        tline = SCR_vtoms2(tres[i], "\t", 0);
	if(tline == 0 || tline[0] == 0) continue;
	for(j = 0 ; tline[j] ; j++) {
	    if(j + col >= KSMPL(kdb)->s_nb) break;
            //Debug("tline[%d]='%s'", j, tline[j]); 
	    if(strncmp(tline[j], "--", 2) == 0 || tline[j][0] == '#' ||
                        tline[j][0] == 0 || tline[j][0] == '\r') // JMP 27/1/2017
		val = SCR_NOT_AVAIL;
	    else
		val = atof(tline[j]);
	    KV_set(kdb, line + i, col + j, (int)global_VM, val);
	    }
	SCR_free_tbl(tline);
	}
    SCR_free_tbl(tres);
    SCR_free(txt);
    return(0);
}

KDB *KV_load_str(string)
char    *string;
{
	return(KV_load_asc_type_ask(string, 1, 1));
}

ODE_importtext(kdb, line, col)
KDB     *kdb;
int     line, col;
{
    KDB     *ikdb;
    U_ch    *txt;

    txt = WscrClipGet();
    if(txt == 0) return(0);
   
    //SCR_AnsiToOem(txt, txt);
    SCR_replace(txt, "#N/A", "na");
    IodeDdeUnLocale(txt);
    SCR_AnsiToOem(txt, txt); // Move AFTER unlocale for CURRENCY
Debug("In txt='%s'\n", txt);
    ikdb = KV_load_str(txt);
    if(ikdb != NULL) KV_merge(kdb, ikdb, 1);
    K_free(ikdb);
    SCR_free(txt);
    return(0);
}


ODE_ClipAddText(KDB *kdb, int i, int j, U_ch ***tline, int *ntline)
{
    double  var;

    var = KV_get(kdb, i, j, (int)global_VM);
    if(L_ISAN(var))
#ifdef REALD
	sprintf(STATIC_BUF, "%.15lg", (double)(var));
#else
	sprintf(STATIC_BUF, "%.8lg", (double)(var));
#endif
    else
	sprintf(STATIC_BUF, "--");

    IodeDdeLocale(STATIC_BUF);
    SCR_add_ptr(tline, ntline, STATIC_BUF);
    return(0);
}


U_ch *ODE_ClipText(KDB *kdb)
{
    int     i, j, lfrom, lto, tmp, cfrom, cto, ntline, ntres;
    U_ch    **tres, *vres, **tline, *ctxt;

    VAR_sel_order(&lfrom, &lto, &cfrom, &cto);
    if(lfrom < 0 || cfrom < 0) return((char *)0);

    tres = 0;
    ntres = 0;
    if(lto >= KNB(kdb) || lfrom >= KNB(kdb)
	|| cto >= KSMPL(kdb)->s_nb || cfrom >= KSMPL(kdb)->s_nb) {
	    VAR_deselect();
	    return((char *)0);
	    }

    for(i = lfrom ; i <= lto ; i++) {
	tline = 0;
	ntline = 0;
	// if(shift) ODE_ClipAddText(kdb, i, -1, &tline, &ntline);
	for(j = cfrom ; j <= cto ; j++)
	    ODE_ClipAddText(kdb, i, j, &tline, &ntline);

	SCR_add_ptr(&tline, &ntline, 0L);
	ctxt = SCR_mtov(tline, '\t');
	SCR_free_tbl(tline);
	SCR_add_ptr(&tres, &ntres, ctxt);
	SCR_free(ctxt);
	}
    SCR_add_ptr(&tres, &ntres, 0L);
    vres = SCR_mtovs(tres, "\r\n");
    SCR_free_tbl(tres);
    return(vres);
}

ODE_ClipCopy(kdb)
KDB     *kdb;
{
    U_ch    *txt;

    txt = ODE_ClipText(kdb);
    if(txt == 0) return(0);
    SCR_OemToAnsi(txt, txt);
    WscrClipSet(txt);
    SCR_free(txt);
    return(0);
}


/**************************************************************\
*                    RESIZE - REPOSITION                       +
\**************************************************************/


ODE_ResizeScrolls() /* JMP 04-02-11 */
{
    if(o_sc != 0) {
	SCR_record_key(SCR_C_X); /* JMP 03-02-11 */
	SCRL_maximize(); /* JMP 03-02-11 */
	//SCRL_adjust(&L0, &L, ML, NL, &nL, (L0 != oL0) ? 1 : 0);
	//SCRL_adjust(&C0, &C, MC, NC, &nC, (C0 != oC0) ? 1 : 0);
	//SCRL_display(1);
	}
}




