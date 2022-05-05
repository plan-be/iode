#include <iodedos.h>
#include <s_scrold.h>

extern  ONAME   tbl_name;

ODE_edit_obj1(kdb, pos)
KDB     *kdb;
int     pos;
{
    ONAME   name;
    int     p1, p2, type = KTYPE(kdb);

    if(pos < 0) name[0] = 0;
    else strcpy(name, KONAME(kdb, pos));
    if(SB_DataEditObj1(name, type) < 0) return(-1);
    if(type == K_EQS) strcpy(name, vkp_eqs_NAME);
    else              strcpy(name, vkp_edit_NAME);
    p1 = K_find(K_WS[type], name);
    if(p1 < 0) return(-1);
    if(kdb == K_WS[type]) return(p1);
    p2 = K_add_entry(kdb, name);
    KSOVAL(kdb, p2) = KSOVAL(K_WS[type], p1);
    return(p2);
}

ODE_edit_scl(kdb, i, j)
KDB     *kdb;
int     i, j;
{
    ONAME   name;
    int     p1, p2;

    if(i >= 0) strcpy(vkvp_create_NAME, KONAME(kdb, i));
    else vkvp_create_NAME[0] = 0;

    if (ODE_edit_window(vkvp_create) < 0) return(-1);
    strcpy(name, vkvp_create_NAME);
    SCR_strip(name);

    if(kdb == K_WS[KTYPE(kdb)]) {
	if (K_find(kdb, name) < 0)  return(K_add(kdb, name, NULL));
	else
	    if(ODE_ask(OM_REPLACE) == 0)
		return(K_add(K_WS[KTYPE(kdb)], name, NULL));
    }

    if (K_find(K_WS[KTYPE(kdb)], name) >= 0)
	if (ODE_ask(OM_REPLACE) != 0) return(-1);

    p1 = K_add(K_WS[KTYPE(kdb)], name, NULL);
    p2 = K_add_entry(kdb, name);
    KSOVAL(kdb, p2) = KSOVAL(K_WS[KTYPE(kdb)], p1);
    return(p2);
}

ODE_edit_tbl(kdb, i, j)
KDB     *kdb;
int     i, j;
{
    TBL     *tbl;
    char    **lecs;
    int     p1, p2 = -1;
    char    *oldseps = A_SEPS, *lst;
    extern  char T_SEPS[];

    if(i < 0) {
	if (ODE_edit_window(vktp_create) < 0) return(-1);
	strcpy(tbl_name, vktp_create_NAME);
	SCR_strip(tbl_name);

	/* if(vktp_create_DIM > 1 && vktp_create_DIM < 20) { /* JMP 01-12-94 */
	if(vktp_create_DIM > 1 && vktp_create_DIM < 100) { /* JMP 22-06-00 */
	    tbl = T_create(vktp_create_DIM);
	    if(tbl == NULL) {
		kerror(0, "Memory error");
		return(-1);
	    }
	    A_SEPS = T_SEPS;

	    lst = K_expand(K_VAR, NULL, vktp_create_LEC, '*');
	    lecs = B_ainit_chk(lst, NULL, 0);
	    SCR_free(lst);

/*          lecs = B_ainit_chk(vktp_create_LEC, 0L, 0);*/
	    A_SEPS = oldseps;
/*
	    lecs = SCR_vtoms(vktp_create_LEC, ";\n\t");
	    T_default(tbl, vktp_create_TITLE, lecs, lecs,
		     vktp_create_MODE, vktp_create_FILES, vktp_create_DATE);
*/

	    T_auto(tbl, SCR_strip(vktp_create_TITLE), lecs, /* GB 13-02-95 */
		     vktp_create_MODE, vktp_create_FILES, vktp_create_DATE);
	    SCR_free_tbl(lecs);
	}
	else {
	    kerror(0, "%d = wrong dimension", (int)vktp_create_DIM); /* JMP 01-12-94 */
	    return(-1);
	}
	T_LANG(tbl) = KT_ENGLISH; /* + vktp_create_LANG JMP38 28-09-92 */
    }
    else {
	tbl = KTVAL(kdb, i);
	memcpy(tbl_name, KONAME(kdb, i), sizeof(ONAME));
    }

    TAB_scroll(tbl, tbl_name);
    if(SCR_LKEY != SCR_F10) goto einde;
    if(kdb == K_WS[KTYPE(kdb)]) {
	if(K_find(kdb, tbl_name) < 0) {
	    p2 = K_add(kdb, tbl_name, tbl);
	    goto einde;
	}
	else
	    if (ODE_ask(OM_REPLACE) == 0) {
		p2 = K_add(kdb, tbl_name, tbl);
		goto einde;
	    }
	    else goto einde;
    }

    if (K_find(K_WS[KTYPE(kdb)], tbl_name) >= 0)
	if (ODE_ask(OM_REPLACE) != 0) goto einde;

    p1 = K_add(K_WS[KTYPE(kdb)], tbl_name, tbl);
    p2 = K_add_entry(kdb, tbl_name);
    KSOVAL(kdb, p2) = KSOVAL(K_WS[KTYPE(kdb)], p1);

einde :
    T_free(tbl);
    return(p2);
}

ODE_edit_rec_scl_var()
{
    switch(SCR_LKEY) {
	case SCR_CSR_LEFT :
	case SCR_CSR_RIGHT :
	case SCR_CSR_UP :
	case SCR_CSR_DOWN :
	case SCR_ENTER :
	    SCR_record_key(SCR_ENTER);
	    SCR_record_key((SCR_LKEY == SCR_ENTER) ?
			    SCR_CSR_RIGHT : SCR_LKEY);
	    break;
	}
    return(0);
}

ODE_set_pos(pg)
PAGE    *pg;
{
    int     l, c;

    SCRL_ccoord(&l, &c);
    pg->pg_line = l;
    pg->pg_column = c - 1;
    return(0);
}

ODE_edit_var_cell(kdb, i, j)
KDB     *kdb;
int     i, j;
{
    double      val;

    if(KSMPL(kdb)->s_nb == 0) {
	kerror(0, "Change nil sample, and try again");
	return(-1);
    }

    val = KV_get(kdb, i, j, (int)global_VM);

    if(ODE_edit_val(&val, (int)global_VW) == 0)
	KV_set(kdb, i, j , (int)global_VM, val);
    ODE_edit_rec_scl_var();
    return(i);
}

ODE_edit_scl_cell(kdb, i, j)
KDB     *kdb;
int     i, j;
{
    double  val;
    SCL     *scl;

    if(j > 1 ) return(i);

    kmsg("ESC : Quit - F10 : Save");
    scl = KSVAL(kdb, i);
    switch(j) {
	case 0 : val = scl->val; break;
	case 1 : val = scl->relax; break;
	}

/*    ODE_set_pos(vkvp_edit);
    vkvp_edit_VAL = val;

    if(ODE_edit_window(vkvp_edit) == 0) {
	val = vkvp_edit_VAL;
	switch(j) {
	    case 0 : scl->val    = val ; break;
	    case 1 : scl->relax  = val ; break;
	    }
	}
*/
    if(ODE_edit_val(&val, (int)global_SW) == 0) {
	switch(j) {
	    case 0 : scl->val    = val ; break;
	    case 1 : scl->relax  = val ; break;
	    }
	}
    ODE_edit_rec_scl_var();
    return(i);
}

ODE_edit_val(val, lg)
double  *val;
int     lg;
{
    char    *res, buf[256], buf1[80];  /* JMP 30-09-2015 */
    int     l, c, lgr = lg;
    double  d;
#ifndef WATCOM
    double  atof();
#endif

    SCRL_ccoord(&l, &c);
/*    SCR_fmt_dbl(*val, buf, 20, -1); /* JMP 09-04-98 */
/*  SCR_fmt_dbl(*val, buf, 20, 15);   /* JMP 09-04-98 */
    if(L_ISAN(*val)) {
#ifdef REALD
	if(global_PREC == 0) global_PREC = 8;
	sprintf(buf1, "%%.%ldlg", global_PREC);
	sprintf(buf, buf1, *val);  /* JMP 29-04-98 */
#else
	sprintf(buf, "%.8lg", *val); /* JMP 09-04-98 */
#endif
	}
    else
	strcpy(buf, "--");          /* JMP 09-04-98 */
    SCR_sqz(buf);             /* JMP 06-07-94 */
    if(strlen(buf) > lgr) {   /* JMP 06-07-94 */
	lgr = strlen(buf);
	if(c + lgr > PC + NCC) c = PC + NCC - lgr - 1; /* JMP 06-07-94 */
	}
    res = T_mmt_edit(buf, l, c, lgr, 1); /* JMP 06-07-94 */
    if(res == 0) return(-1);
    if(strncmp(res, "--", 2) == 0) *val = SCR_NOT_AVAIL;
    else                           *val = atof(res);
/*    Debug("%20.12lg\n", *val); */
    SCR_free(res);
    return(0);
}










