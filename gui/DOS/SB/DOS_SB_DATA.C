#include <iodedos.h>

SB_DataSearch()
{
    vkp_search_RES[0] = 0;
    PG_edit_window(vkp_search);
    return(0);
}

void C_DataSearch()
{
    int     pos;
	char    buf[1024]; /* JMP 30-09-2015 */

    sprintf(buf, "%s %d %d %d %d %d %s",
	vkp_search_NAME,
	vkp_search_WORD,
	vkp_search_CASE,
	vkp_search_NAMES,
	vkp_search_FORMS,
	vkp_search_TEXTS,
	vkp_search_OUT);

    vkp_search_RES[0] = 0;
    if(B_DataSearch(buf, (int)vkp_search_TYPE))
	B_display_last_error();

    else {
	pos = K_find(K_WS[LISTS], vkp_search_OUT);
	if(pos >= 0)
	    SCR_strlcpy(vkp_search_RES, KLVAL(K_WS[LISTS], pos), 80);
	}
    PG_display_flds(vkp_search);
}

/* Duplicate */
SB_DataDuplicate()
{
    PG_edit_window(vkp_dupl);
    return(0);
}

C_DataDuplicate()
{
    int     pos;
	char    buf[512]; // JMP 26/08/2015

    sprintf(buf, "%s %s", vkp_dupl_FROM, vkp_dupl_TO);
    if(B_DataDuplicate(buf, (int) vkp_dupl_TYPE)) {
	B_display_last_error();
	return(-1);
	}

    return(0);
}


/* ============ EDIT ======================== */

SB_DataEdit(arg, type)
char    *arg;
int     type;
{
    if(arg == 0 || arg[0] == 0) {
	vkp_list_TYPE = type;
	PG_edit_window(vkp_list);
	if(SCR_LKEY == SCR_ESCAPE) return(0);
	arg = vkp_list_LIST;
    }

    return(SB_DataEditScroll(arg, type));
}

SB_DataEditScroll(arg, type)
char    *arg;
int     type;
{
    int     rc;
    char    **args,
	    *lst = NULL,
	    * K_expand();

    if(arg == 0 || arg[0] == 0) {
	ODE_scroll(K_WS[type], NULL);
	return(0);
    }

    lst = K_expand(type, NULL, arg, '*');
    args = B_ainit_chk(lst, NULL, 0);
    if(args == 0) {
	B_display_last_error();
	return(-1);
	}
    ODE_scroll(K_WS[type], args);

    A_free(args);
    SCR_free(lst);
    return(0);
}


SB_DataEditObj1(name, type)
char    *name;
int     type;
{
    switch(type) {
	case COMMENTS :
	case IDENTITIES :
	case K_VAR :
	case LISTS : return(SB_DataEditCil1(name, type));
	case EQUATIONS : return(SB_DataEditEqs1(name));
	case K_TBL :
	case SCALARS : return(0);
	}

    return(0);
}

SB_DataEditCil1(name, type)
char    *name;
int     type;
{
    int     pos = K_find(K_WS[type], name);
    char    *val;

    PG_reset(vkp_edit);
    vkp_edit_TYPE = type;
    val = "";
    if(pos >= 0) {
	SCR_strlcpy(vkp_edit_NAME, name, K_MAX_NAME);  /* IODE64K */
	if(type != K_VAR) val = KCVAL(K_WS[type], pos);
    }
/*
    SCR_strfacpy(&(vkp_edit_VAL), val);
*/
    SCR_free(vkp_edit_VAL);
    vkp_edit_VAL = K_wrap(val, 60);

    PG_edit_window(vkp_edit);
    return(0);
}

C_DataEditCil1()
{
    char    *txt;
    int     rc, type;
    ONAME   name;

    type = vkp_edit_TYPE;
    strcpy(name, vkp_edit_NAME);
    SCR_sqz(name);
    if(K_find(K_WS[type], name) >= 0 &&
       ODE_ask(OM_REPLACE)) return(-1);

    txt = SW_nalloc(K_MAX_NAME + 10 + strlen(vkp_edit_VAL)); /* IODE64K */
    sprintf(txt, "%s %s", name, vkp_edit_VAL);
    if(type == K_VAR)
	rc = B_DataCalcVar(txt);
    else
	rc = B_DataUpdate(txt, type);
    SCR_free(txt);
    if(rc) B_display_last_error();
    return(rc);
}

SB_DataEditEqs1(name)
char    *name;
{
    int     pos = K_find(K_WS[EQUATIONS], name);
    char    per[10];
    EQ      *eq;

    PG_reset(vkp_eqs);
    if(pos >= 0) {
	SCR_strlcpy(vkp_eqs_NAME, name, K_MAX_NAME); /* IODE64K */
	eq = KEVAL(K_WS[EQUATIONS], pos);

	vkp_eqs_METHOD = eq->method;
/*        vkp_eqs_DATE   = eq->date; */

	vkp_eqs_FSTAT   = eq->tests[6];
	vkp_eqs_RSQUARE = eq->tests[8];
	vkp_eqs_DW      = eq->tests[9];
	vkp_eqs_LLIK    = eq->tests[10];

	SCR_strfacpy(&(vkp_eqs_CMT),    eq->cmt);
	SCR_strfacpy(&(vkp_eqs_BLK),    eq->blk);
	SCR_strfacpy(&(vkp_eqs_LEC),    eq->lec);
	SCR_strfacpy(&(vkp_eqs_INSTRS), eq->instr);

	PER_pertoa(&(eq->smpl.s_p1), per);
	SCR_strlcpy(vkp_eqs_FROM, per, 8);
	PER_pertoa(&(eq->smpl.s_p2), per);
	SCR_strlcpy(vkp_eqs_TO, per, 8);

	E_free(eq);
    }
    else
	SCR_strfacpy(&(vkp_eqs_LEC), "");

    PG_edit_window(vkp_eqs);
    return(0);
}

C_DataEditEqs1()
{
    int     rc;
    ONAME   name;
    SAMPLE  *smpl = NULL;

    strcpy(name, vkp_eqs_NAME);
    SCR_sqz(name);
    if(K_find(K_WS[EQUATIONS], name) >= 0 && ODE_ask(OM_REPLACE)) return(-1);

    smpl = PER_atosmpl(vkp_eqs_FROM, vkp_eqs_TO);
    if(smpl == 0) {
	B_seterrn(56);
	rc = -1;
    }
    else {
	rc = B_DataUpdateEqs(name, vkp_eqs_LEC,
			 vkp_eqs_CMT,
			 vkp_eqs_METHOD + KE_LSQ,
			 smpl,
			 vkp_eqs_INSTRS,
			 vkp_eqs_BLK,
			 NULL,
			 0);
	SW_nfree(smpl);
    }

    if(rc) B_display_last_error();
    return(rc);
}
/*
SB_DataEditCnf()
{
    vkp_dataeditcnf_MODE = KVAR_MODE;

    PG_edit_window(vkp_dataeditcnf);
    return(0);
}

C_DataEditCnf()
{
    char    buf[25];

    sprintf(buf, "%c %ld", "LDG"[vkp_dataeditcnf_MODE],
			      vkp_dataeditcnf_NDEC);
    return(B_DataEditCnf(buf));
}
*/

SB_DataListSort()
{
    vkp_listsort_CNT = 0;
    PG_edit_window(vkp_listsort);
    return(0);
}

C_DataListSort()
{
    int     pos;
    char    buf[81];

    sprintf(buf, "%s %s", vkp_listsort_IN, vkp_listsort_OUT);
    if(B_DataListSort(buf)) {
	B_display_last_error();
	return(-1);
    }
    vkp_listsort_CNT = B_DataListCount(vkp_listsort_OUT);
    PG_display_flds(vkp_listsort);
    return(0);
}


SB_DataEditGraph()
{
    PG_edit_window(vkp_dataeditgraph);
    return(0);
}

C_AutoEditGraph(names, mode) /* GB 08-97 */
char    *names;
int     mode;
{
    char    buf[4096]; /* JMP 30-09-2015 */
    int     rc = 0;

    sprintf(buf, "%c %c %c %c %c %lg %lg %s %s %s",
		"LDGdg"[mode], /* GB 10/08/98 */
		"LSBM"[vkp_dataeditgraph_CHART],
		"MNJ"[vkp_dataeditgraph_XGRID],
		"MNJ"[vkp_dataeditgraph_YGRID],
		"LGSP"[vkp_dataeditgraph_AXIS],
		vkp_dataeditgraph_YMIN,
		vkp_dataeditgraph_YMAX,
		vkp_dataeditgraph_FROM, vkp_dataeditgraph_TO,
		names);

    B_LANG = vkp_dataeditgraph_LANG;
    B_DataDisplayGraph(buf);

    if(rc) B_display_last_error();
    return(rc);
}

C_DataEditGraph(dest) /* GB 08-97 */
int     dest;
{
    char    buf[1024], *names = vkp_dataeditgraph_NAMES;
    int     i, rc = 0;

    sprintf(buf, "%c %c %c %c %c %lg %lg %s %s %s",
		"LDGdg"[vkp_dataeditgraph_MODE],
		"LSBM"[vkp_dataeditgraph_CHART],
		"MNJ"[vkp_dataeditgraph_XGRID],
		"MNJ"[vkp_dataeditgraph_YGRID],
		"LGSP"[vkp_dataeditgraph_AXIS],
		vkp_dataeditgraph_YMIN,
		vkp_dataeditgraph_YMAX,
		vkp_dataeditgraph_FROM, vkp_dataeditgraph_TO,
		(names == 0) ? "" : names);

    B_LANG = vkp_dataeditgraph_LANG;

    if(dest == 0) rc = B_DataDisplayGraph(buf);
    else { /* GB 12/97 */
	if(B_dest == 1)
	     if(PG_edit_window(vkp_fprintfile) < 0) return(-1);
	if(B_PrintDest(B_outfile) < 0) return(-1);

	rc = B_DataPrintGraph(buf); /* GB 08-97 */

    }

    if(rc) B_display_last_error();
    else if(dest) ODE_error(OM_TBLS_PRINTED); /* JMP 16-12-93 */
    return(rc);
}

SB_DataScan()
{
    PG_edit_window(vkip_scan);
    return(0);
}

C_DataScan()
{
    if(B_DataScan(vkip_scan_LST, vkip_scan_TYPE)) {
	B_display_last_error();
	return(-1);
    }

    return(0);
}

SB_DataList()
{
    vkp_dlist_RES[0] = 0;
    vkp_dlist_CNT    = 0;
    PG_edit_window(vkp_dlist);
    return(0);
}

void C_DataList()
{
    int     pos;
	char    buf[1024]; // JMP 26/08/2015

    sprintf(buf, "%s %s %s",
	vkp_dlist_NAME,
	vkp_dlist_PATTERN,
	vkp_dlist_FILE);

    if(B_DataList(buf, (int) vkp_dlist_TYPE))
	B_display_last_error();

    else {
	pos = K_find(K_WS[LISTS], vkp_dlist_NAME);
	if(pos >= 0)
	    SCR_strlcpy(vkp_dlist_RES, KLVAL(K_WS[LISTS], pos), 160);
	    vkp_dlist_CNT = B_DataListCount(vkp_dlist_NAME);
	}

    PG_display_flds(vkp_dlist);
}

SB_DataCalcLst()
{
    vkp_calclist_RES[0] = 0;
    vkp_calclist_CNT    = 0;
    PG_edit_window(vkp_calclist);
    return(0);
}

void C_DataCalcLst()
{
    int     pos;
    char    buf[101];

    sprintf(buf, "%s %s %c %s",
	vkp_calclist_OUT,
	vkp_calclist_LIST1,
	"+*-"[vkp_calclist_OP],
	vkp_calclist_LIST2);

    vkp_calclist_RES[0] = 0;
    if(B_DataCalcLst(buf)) B_display_last_error();

    else {
	pos = K_find(K_WS[LISTS], vkp_calclist_OUT);
	if(pos >= 0) {
	    SCR_strlcpy(vkp_calclist_RES, KLVAL(K_WS[LISTS], pos), 160);
	    vkp_calclist_CNT = B_DataListCount(vkp_calclist_OUT);
	}
    }

    PG_display_flds(vkp_calclist);
}


SB_DataCompare()
{
    vkp_clist_CNT1 = 0;
    vkp_clist_CNT2 = 0;
    vkp_clist_CNT3 = 0;
    vkp_clist_CNT4 = 0;

    PG_edit_window(vkp_clist);
    return(0);
}

C_DataCompare()
{
    int     pos;
	char    buf[1024]; // JMP 26/8/2015

	// Set Vars threshold
	sprintf(buf, "%g", vkp_clist_EPS);
	B_DataCompareEps(buf);

    sprintf(buf, "%s %s %s %s %s",
	vkp_clist_FILE,
	vkp_clist_ONE,
	vkp_clist_TWO,
	vkp_clist_THREE,
	vkp_clist_FOR);

    if(B_DataCompare(buf, (int) vkp_clist_TYPE)) B_display_last_error();
    else {
	vkp_clist_CNT1 = B_DataListCount(vkp_clist_ONE);
	vkp_clist_CNT2 = B_DataListCount(vkp_clist_TWO);
	vkp_clist_CNT3 = B_DataListCount(vkp_clist_THREE);
	vkp_clist_CNT4 = B_DataListCount(vkp_clist_FOR);
    }

    PG_display_flds(vkp_clist);
    return(0);
}



