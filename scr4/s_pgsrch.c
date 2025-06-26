#include "scr4.h"

/* Si cette variable est à 0 le programme fait pas de PG_reset_all avant le PG_search */
extern int  SCR_RESET_BEFORE_PGSEARCH; /* JMP 15-01-99 */
extern int SCR_PGSEARCH_NOEDIT;  /* BP_M 05-10-1999 */

/*NH*/
/* =+=== PG_SEARCH ===================================
    Reset a page to default values and allow the user
	to select an index or search.
    Wait for the value of the index and search.

    pg : IN : pointer to the page
    is : IN : isam pointer
    Return  -1 if no index is present in the page
	    -2 if escaped by the user
	    -3 if no record is found
	    -4 if INSERT
	    index chosen else
    =================================================== */

PG_search(pg, is)   /* BP_M 01-10-97 */
PAGE    *pg;
ISAM    *is;
{
    int     cmp_nb;
    char    *order = 0;
    int     cond;
    extern  int SCR_SQL_DIFF;   /* BP_M 21-10-97 */
    extern  int SCR_IS2PG_PGSEARCH;  /* BP_M 26-05-1999 */

    /* BP_M 01-10-97 */
    IS_empty_rec(is);   /* BP_M 02-02-1998 */
    switch(SCR_MODE_SQL) {
	case 0:
	    if(pg->pg_fld_idx != MAX_BFIELD12) {
		PG_edit(pg);
		if(pg->pg_abort || SCR_APP_ABORT) return(-2);
	    }
GMSGC
	    cmp_nb = PG_search_1(pg, is);

	    if(cmp_nb < 0) return(cmp_nb);
	    if(SCR_edit_cmp_page(pg, is, cmp_nb) < 0) {
		return(-2);
	    }
	    if(0 != SCR_search_page_by_cmp(pg, is, cmp_nb) ) return(-3);
	    PG_display_flds_no_cmp(pg);
	    return(cmp_nb);
	case 1:
ag:
GMSGC
	    PG_read_flds(pg);       /* BP_M 22-08-1997 */
	    if(SCR_RESET_BEFORE_PGSEARCH)   /* BP_MODIF 22-12-98 */
		PG_reset_all(pg);
	    PG_display_flds(pg);        /* BP_M 22-08-1997 */
	    PG_edit(pg);
	    if(pg->pg_abort) return(-2); /*JMP 15-05-95*/
	    if(SCR_LKEY == SCR_F3 || SCR_LKEY == SCR_ESCAPE)
		return(-2);
	    if(SCR_app_key()) return(-2);

	    switch(IS_sql_menu_order(is, pg, &order)) {
		case -1: goto ag2;
		case 0: cond = SCR_SQL; break;
		case 1: cond = SCR_SQL1; break;
		case 2: cond = SCR_GE; break;
	    }

	    IS_empty_rec(is);
	    SCR_page_to_isam(pg, is);
	    if(IS_search(is, 0, cond, order) != 0) return(-3);
	    SCR_free(order);
	    if(SCR_IS2PG_PGSEARCH) { /* BP_M 26-05-1999 */
		SCR_isam_to_page(pg, is);
		PG_display_flds_no_cmp(pg);
	    }
	    return(-5);
	case 2:
ag2:
GMSGC
	    if(SCR_PGSEARCH_NOEDIT == 0) {
		PG_read_flds(pg);       /* BP_M 22-08-1997 */
		if(SCR_RESET_BEFORE_PGSEARCH)   /* BP_MODIF 22-12-98 */
		    PG_reset_all(pg);
		PG_display_flds(pg);        /* BP_M 22-08-1997 */
		PG_edit(pg);
		if(pg->pg_abort) return(-2); /*JMP 15-05-95*/
		if(SCR_LKEY == SCR_F3 || SCR_LKEY == SCR_ESCAPE)
		    return(-2);
		if(SCR_app_key()) return(-2);
	    }
	    if(IS_sql_simple_order(is, pg, &order) == 0)
		cond = SCR_SQL;
	    else {
		SCR_record_key(SCR_ESCAPE);
		return(-2);     /* BP_M 13-09-2000 */
	    }

	    IS_empty_rec(is);
	    SCR_page_to_isam(pg, is);
	    SCR_SQL_DIFF = 1; /* SCR_GE */
	    if(IS_search(is, 0, cond, order) != 0) {
		SCR_SQL_DIFF = -1; /* SCR_GE */
		return(-3);
	    }
	    SCR_free(order);
	    SCR_SQL_DIFF = -1; /* SCR_GE */

	    if(SCR_IS2PG_PGSEARCH) { /* BP_M 26-05-1999 */
		SCR_isam_to_page(pg, is);
		PG_display_flds_no_cmp(pg);
	    }
	    return(-5);
    }
}

/*NH*/
PG_search_1(pg, is)
PAGE    *pg;
ISAM    *is;
{
    int     cmp_nb, flag = 0, i, nopts;                 /* JMP 27-12-93 */
    MENU    *mn;

GMSGBC
    if(SCR_RESET_BEFORE_PGSEARCH)  /* BP_M 26-05-1999 */
	PG_reset(pg);
    if(pg->pg_menu == 0) {
	cmp_nb = SCR_select_composite(pg, is) ;
	GMSG("Index : %d\n", cmp_nb);
	}
    else {
	mn = (MENU *)SCR_PTR(pg->pg_menu);
	MN_read_opt(mn);                                /* JMP 27-12-93 */
	cmp_nb = -2;                                    /* JMP 27-12-93 */
	for(i = nopts = 0 ; i < mn->mn_nb_opts ; i++)   /* JMP 27-12-93 */
	    if(mn->mn_opts[i].opt_enable) {             /* JMP 27-12-93 */
		nopts++;                                /* JMP 27-12-93 */
		cmp_nb = i;                             /* JMP 27-12-93 */
		}                                       /* JMP 27-12-93 */
	MN_free_opt(mn);                                /* JMP 27-12-93 */
	if(nopts < 2) GMSGR(cmp_nb);                   /* JMP 27-12-93 */
	if(mn->mn_auto) {
	    if(pg->pg_scr_pos != -1) flag++;
	    mn->mn_line = pg->pg_line + flag;
	    mn->mn_column = pg->pg_column + flag;
	    }
	cmp_nb = MN_edit(mn) - 1;
	if(cmp_nb < 0) cmp_nb = -2;
	}
    GMSGR(cmp_nb);
}















