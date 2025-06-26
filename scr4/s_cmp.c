#include "scr.h"

extern int     SCR_APPLYCMP_IFEMPTY;    /* BP_M 12-12-2009 16:28 */
extern int     SCR_NOT_APPLY_JOIN;  /* BP_M 12-12-2009 16:28 */

/* ======================================================================
Applique les CODES associés à un champ d'une PAGE :

&EN Recherche dans l'ISAM CODE le record correspondant (ou le suivant si la
condition est CIRC ou GE) à la valeur du champ de la PAGE. Si la condition
est EQ, un record égal doit être trouvé (pour ce qui concerne le champ
recherché).

&EN Copie dans la PAGE les valeurs des autres champs de l'ISAM CODE.

&EN Recommence l'opération pour les autres CODES définis dans ce même champ.

En cas d'erreur, un message est affiché (record non trouvé, ISAM vide ou
inaccessible, etc).

&RT 0 en cas de succès, -1 sinon
&SA SCR_apply_all_cmp()
------------------------------------------------------------------------ */

SCR_apply_cmp(pg, fld_nb)
PAGE    *pg;
int     fld_nb;
{
    int     rc, pg_abort;                   /*JMP 29-05-95*/
					    /*JMP 29-05-95*/
    pg_abort = SCR_PG_ABORT;                /*JMP 29-05-95*/
    rc = SCR_apply_cmp_1(pg, fld_nb);       /*JMP 29-05-95*/
    SCR_PG_ABORT = pg_abort;                /*JMP 29-05-95*/
    return(rc);                             /*JMP 29-05-95*/
}

/*NH*/
SCR_apply_cmp_1(pg, fld_nb) /* BP_M 01-10-97 */
PAGE    *pg;
int     fld_nb;
{
    FIELD   *fld, *fld1;
    ISAM    *is, *ispg, *isj;
    int     flag = 0, i = 0, j = 0, cond, ok, k, m, flag_empty = 1, n;
    ISFLD   *isf, *isf1;
    extern  int SCR_SQL_DIFF;

    fld = pg->pg_flds + fld_nb;
    if(!fld->fld_modified) return(0);
    ispg = (ISAM *)SCR_PTR(pg->pg_isam);

    for(i = 0 , isf = fld->fld_isam ; i < fld->fld_nb_isam ; i++, isf++) {
	ok = 0;
	if(isf->if_cmp_nb == MAX_BFIELD6) continue;
	is = (ISAM *)SCR_PTR(isf->if_isam);

	/* BP_M 29-02-2008 */
	if(SCR_NOT_APPLY_JOIN && ispg->is_nb_join) {
	    for(n = 0 ; n < ispg->is_nb_join ; n++)
		if(isf->if_isam == ispg->is_join[n].is) break;
	    if(n != ispg->is_nb_join) continue;
	}
	/* BP_M 26-05-2011 14:05 */

	cond = isf->if_cmp_cond;

	/* COMPOSITE FOUND */
	if(PG_is_fld_empty(pg, fld_nb)) {
	    /* BP_M 26-05-2011 14:05 */
	    if(cond != SCR_EQS)
		IS_empty_rec(is);
	    else
		ok = -1;
	    /* BP_M 01-10-1998 */
	    if(!SCR_APPLYCMP_IFEMPTY) flag_empty = 0;
	}
	else {
	    if(IS_is_open(is) == 0) {           /* open file if not yet opened */
		flag = 1;
		if(0 != IS_open(is, SCR_READ, SCR_NO_LOCK)) {
		    PG_display_error(IS_error_text());
		    return(-1);
		}
	    }
	    IS_empty_rec(is);
/*          SCR_copy_fldcmp_to_is(pg, is, is->is_cmps + isf->if_cmp_nb); */

	    SCR_fld_to_isf(pg, fld_nb, is, is->is_cmps[isf->if_cmp_nb].cmp_flds[0]);

/*          SCR_page_to_isam(pg, is); */


	    if(cond == SCR_CIRC) cond = SCR_GE;
	    ok = IS_search(is, isf->if_cmp_nb, cond, (char *)0);
	    if(ok != 0 && isf->if_cmp_cond == SCR_CIRC) {
		IS_empty_rec(is);
		ok = IS_search(is, isf->if_cmp_nb, SCR_GE, (char *)0);
	    }
	    SCR_SQL_DIFF = -1;
	    if(ok != 0) {
		if(cond != SCR_EQS) PG_display_error(IS_error_text());
		IS_empty_rec(is);
		if(flag == 1) IS_close(is);
	    }
	}
	/* BP_M 26-05-2011 14:05 */
	if(cond != SCR_EQS || ok == 0) {
	    /* SAVE THE ISAM FIELDS IN PAGE FIELDS */
	    for(j = 0, fld1 = pg->pg_flds ; j < pg->pg_nb_flds ; j++, fld1++) {
		if(fld->fld_acmp != 0) {
		    for(m = 0 ; fld->fld_acmp[m] >= 0 ; m++)
			if(fld->fld_acmp[m] == j) break;
		    if(fld->fld_acmp[m] < 0) continue;
		}
		isf1 = fld1->fld_isam;
		for(k = 0 ; k < fld1->fld_nb_isam ; k++, isf1++)
		    if((ISAM *)SCR_PTR(isf1->if_isam) == is) {
			if(fld1->fld_io == 0 && flag_empty == 0) continue;
			SCR_isf_to_fld(pg, j, is, isf1->if_isam_nb);
			break;
		    }

		if(k < fld1->fld_nb_isam) {
		    isf1 = fld1->fld_isam;
		    for(k = 0 ; k < fld1->fld_nb_isam ; k++, isf1++)
			if((ISAM *)SCR_PTR(isf1->if_isam) != is &&
			   isf1->if_cmp_nb != MAX_BFIELD6 && j != fld_nb)
				SCR_apply_cmp(pg, j);
		}
	    }
	}
	if(flag == 1) IS_close(is);
	/* BP_M 26-05-2011 14:05 */
	if(ok && cond != SCR_EQS) return(-1); /* JMP 10-10-91 */
    }
    return(0);
}

/* ======================================================================
Applique la fonction SCR_apply_cmp() à tous les champs de la PAGE contenant
le mot-clé CODE.
&RT 0 en cas de succès, -1 sinon
&SA SCR_apply_cmp(), SCR_display_cmp()
------------------------------------------------------------------------- */

SCR_apply_all_cmp(pg)
PAGE    *pg;
{
    TPAGES  *tpg;
    int     i;

    SCR_NOT_APPLY_JOIN = 1;
    if(SCR_PTR_TYPE(pg) == SCR_TYPE_TPAGES) {
	tpg = (TPAGES *)pg;
	for(i = 0 ; i < tpg->tp_nb ; i++)
	    SCR_apply_all_cmp_1(PAGE_PTR(tpg->tp_pgsnbs[i]));
	}
    else
	SCR_apply_all_cmp_1(pg);
    SCR_NOT_APPLY_JOIN = 0;

    return(0);
}

/*NH*/
SCR_apply_all_cmp_1(pg) /* JMP 20-08-97 */
PAGE    *pg;
{
    int     i;

    for(i = 0 ; i < pg->pg_nb_flds ; i++)
	SCR_apply_cmp(pg, i);
    return(0);
}

/* ======================================================================
Exécute la fonction SCR_display_cmp() pour chaque champ CODE de la PAGE.
&EX
    SCR_apply_all_cmp(pg);     --> exécute tous les CODES de la PAGE
    SCR_display_all_cmp(pg);   --> affiche dans l'écran les champs
				   modifiés

&TX
&SA SCR_apply_all_cmp(), SCR_display_cmp()
------------------------------------------------------------------------- */
SCR_display_all_cmp(pg)
PAGE    *pg;
{
    int     i;

    for(i = 0 ; i < pg->pg_nb_flds ; i++)
	SCR_display_cmp(pg, i);
    return(0);
}

/* ======================================================================
Affiche tous les champs susceptibles d'être modifiés par la fonction
SCR_apply_cmp(pg, fld_nb), c'est-à-dire tous ceux faisant partie d'un des
ISAM dont le champ fld_nb est CODE.
&EX
    SCR_apply_cmp(pg, fld_nb);    --> pas d'affichage
    SCR_display_cmp(pg, fld_nb);  --> affichage des champs modifiés dans
				      la PAGE pg
&TX
&SA SCR_display_all_cmp(), SCR_apply_cmp()
------------------------------------------------------------------------- */

SCR_display_cmp(pg, fld_nb)
PAGE    *pg;
int     fld_nb;
{
    int     i;
    FIELD   *fld;
    ISFLD   *isf;
    int     flag = 0;

    fld = pg->pg_flds + fld_nb;
    isf  = fld->fld_isam;
    for(i = 0 ; i < fld->fld_nb_isam ; i++, isf++) {
	if(isf->if_cmp_nb == MAX_BFIELD6) continue;
	PG_display_isam_flds(pg);
	flag = 1;
	break;
	}
    if(flag == 0) PG_display_field(pg, fld_nb);
    return(0);
}

/*NH*/
/* =+==== PG_display_isam_flds =====================
    Display all the fields linked to any isam
	pg : IN : page ptr
   ================================================= */

PG_display_isam_flds(pg)
PAGE    *pg;
{
    int     i;

    for(i = 0 ; i < pg->pg_nb_flds ; i++)
	if(pg->pg_flds[i].fld_isam != 0)
	    PG_display_field(pg, i);

    return(0);
}


/*NH*/
/* =+========== SCR_search_page_by_cmp =====================================
    Same as SCR_search_page_by_fld, but you give the ISAM pointer and the COMPOSITE
    number in place of the field number (more complicated, but more general).
    The number of the COMPOSITE can be obtained by SCR_select_composite.
	pg     : IO : pointer to the page
	is     : IO : pointer to the isam
	cmp_nb : IN : number of the composite
    Returns : error code or 0
    ======================================================================== */

SCR_search_page_by_cmp(pg, is, cmp_nb)
PAGE    *pg;
ISAM    *is;
int     cmp_nb;
{
    SCR_page_to_isam(pg, is);
    if(0 != IS_search(is, cmp_nb, SCR_GE, (char *)0)) {
	PG_display_error(IS_error_text());
	return(-1);
	}
    SCR_isam_to_page(pg, is);
    return(0);
}
/*NH*/
/* =+========== SCR_next_page_by_cmp =====================================
    Same as SCR_search_page_by_cmp, but find the next record.
	pg     : IO : pointer to the page
	is     : IO : pointer to the isam
    Returns : error code or 0
    ======================================================================== */

SCR_next_page_by_cmp(pg, is)
PAGE    *pg;
ISAM    *is;
{
    if(0 != IS_next(is)) {
	PG_display_error(IS_error_text());
	return(-1);
	}
    SCR_isam_to_page(pg, is);
    return(0);
}
/*NH*/
/* =+========== SCR_prev_page_by_cmp =====================================
    Same as SCR_search_page_by_cmp, but find the prev record.
	pg     : IO : pointer to the page
	is     : IO : pointer to the isam
    Returns : error code or 0
    ======================================================================== */

SCR_prev_page_by_cmp(pg, is)
PAGE    *pg;
ISAM    *is;
{
    if(0 != IS_prev(is)) {
	PG_display_error(IS_error_text());
	return(-1);
	}
    SCR_isam_to_page(pg, is);
    return(0);
}














