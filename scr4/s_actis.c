#include "scr.h"

/* ======================================================================
Edite la PAGE pg et crée un nouveau record l'ISAM défini par cette PAGE. Si
la touche de fonction utilisée pour quitter la PAGE est F10, sauve le
record défini dans la PAGE dans l'ISAM.

A la différence de PG_create_rec(), cette fonction n'affiche pas la PAGE pg
et demande le pointeur vers l'ISAM à éditer. La PAGE reste affichée après
la fonction.

Voir ACTION PG_CREATE_REC pour plus d'explications

&RT 0 en cas de succès, -1 sinon
&SA PG_CREATE_REC
------------------------------------------------------------------------- */

SCR_pg_create_rec(pg, is)
PAGE    *pg;
ISAM    *is;
{
    STATUS  *sta;

    sta = SCR_save_status();

    pg->pg_operation = 1;
    SCR_edis_create(pg, is);
    pg->pg_operation = 0;

    SCR_reset_status(sta);
    return(0);
}


/* ======================================================================
Recherche le record suivant dans l'ordre courant de recherche dans l'ISAM
is.  Affiche le nouveau record dans la PAGE pg.

Voir ACTION NEXT_REC pour plus d'explications
&RT 0 en cas de succès, -1 sinon
&SA NEXT_REC, PREV_REC
------------------------------------------------------------------------- */

SCR_next_rec(pg, is)
PAGE    *pg;
ISAM    *is;
{
    if(pg->pg_operation != 3) {             /* BP_M 06-05-2002 11:43 */
	PG_display_error(SCR_err_txt(114));
	return(-1);
    }
    if(IS_next(is) != 0) {
	PG_display_error(IS_error_text());
	return(-1);
    }
    PG_reset(pg);
    SCR_isam_to_page(pg, is);
    PG_display_flds_no_cmp(pg);

    return(0);
}


/* ======================================================================
Recherche le record précédent dans l'ordre courant de recherche dans l'ISAM
is.  Affiche le nouveau record dans la PAGE pg.

Voir ACTION PREV_REC pour plus d'explications
&RT 0 en cas de succès, -1 sinon
&SA NEXT_REC, PREV_REC
------------------------------------------------------------------------- */

SCR_prev_rec(pg, is)
PAGE    *pg;
ISAM    *is;
{
    if(pg->pg_operation != 3) {         /* BP_M 06-05-2002 11:43 */
	PG_display_error(SCR_err_txt(114));
	return(-1);
    }
    if(IS_prev(is) != 0) {
	PG_display_error(IS_error_text());
	return(-1);
    }
    PG_reset(pg);
    SCR_isam_to_page(pg, is);
    PG_display_flds_no_cmp(pg);

    return(0);
}


/* ======================================================================
Détruit le record suivant dans l'ordre courant de recherche dans l'ISAM
is.

Voir ACTION DELETE_REC pour plus d'explications
&RT 0 en cas de succès, -1 sinon
&SA NEXT_REC, PREV_REC, DELETE_REC
------------------------------------------------------------------------- */

SCR_delete_rec(pg, is)
PAGE    *pg;
ISAM    *is;
{
    if(pg->pg_operation != 3) {         /* BP_M 06-05-2002 11:43 */
	PG_display_error(SCR_err_txt(114));
	return(-1);
    }
    if(CONFIRME) return(-1);

/*  pg->pg_operation = 0; */ /* BP_M 25-11-1998 */
    if(IS_delete(is) != 0) {
	PG_display_error(IS_error_text());
	return(-1);
    }
    PG_reset(pg);
    PG_display_flds_no_cmp(pg);

    return(0);
}



/*NH*/
PG_check_nodup(pg, fld) /* BP_M 01-10-97 */
PAGE    *pg;
FIELD   *fld;
{
    ISAM    *is;
    int     is_cmp, i, j, fld_is;
    char    *rec;
    COMPOSITE *cmp;

    if(SCR_MODE_SQL == 0 && SCR_PG_CHECK_NODUP) {
	is = (ISAM *)SCR_PTR(fld->fld_isam[0].if_isam);
    /*    if(fld->fld_isam[0].if_cmp_nb != 4095) return(0); */
	if(fld->fld_isam[0].if_cmp_nb != MAX_BFIELD6) return(0);  /* BP_M */
	fld_is = fld->fld_isam[0].if_isam_nb;
	cmp = is->is_cmps;
	for(i = 0 ; i < is->is_nb_cmps ; i++, cmp++) {
	    if(cmp->cmp_nb_flds > 1) continue;  /* BP_M 22-02-2000 */
	    for(j = 0 ; j < cmp->cmp_nb_flds ; j++)
		if(fld_is == cmp->cmp_flds[j] && cmp->cmp_dup == 1 /* NODUP */) break;
	    if(j != cmp->cmp_nb_flds) break;
	}
	if(i == is->is_nb_cmps) return(0);

	if(IS_open(is, SCR_UPDATE, SCR_NO_LOCK) != 0) {
	    PG_display_error(IS_error_text());
	    return(-1);
	}
	rec = SCR_malloc(is->is_rec_len);
	memcpy(rec, is->is_rec, is->is_rec_len);
	IS_empty_rec(is);

	SCR_page_to_isam(pg, is);
	i = IS_search(is, i, SCR_EQ, (char *)0);  /* JMP 12-12-98 */
	IS_close(is);
	memcpy(is->is_rec, rec, is->is_rec_len);
	SCR_free(rec);

	if(i == 0) {
	    PG_display_error(SCR_err_txt(115));
	    return(-1);
	}
    }
    return(0);
}

extern long     ACT_find_lg();
extern char     *ACT_find_str();
extern double   ACT_find_db();

/*NH*/
extern char    SCR_EXPORT_TXT[SCR_MAX_FIELD_LENGTH + 1];    /* BP_M 12-12-2009 13:22 */

/*NH*/
ACT_exec_aa_isam(aa)
A_ACTION    *aa;
{
    char    *ptr = SCR_PTR(ACT_find_lg(aa, 0));
    MPAGE   *mp = (MPAGE *)ptr;
    APPL    *ap = (APPL *)ptr;
    PAGE    *pg = (PAGE *)ptr;

    switch(aa->aa_fn) {
	case ACT_APPLY_CMP :    SCR_apply_all_cmp(pg); break;
	case ACT_CREATE_REC:    PG_create_rec(pg); break;
	case ACT_AB_EDIT_ISAM : AB_edit_isam(pg); break;
	case ACT_PG_SCAN_ISAM : PG_scan_isam(pg); break;
	case ACT_MP_EDIT_ISAM : MP_edit_isam2(mp); break;
	case ACT_PG_EDIT_ISAM : PG_edit_isam(pg); break;
	case SCR_AP_EDIT_ISAM : AP_edit_isam(pg); break;    /* BP_M 04-10-1999 */

	case ACT_PG_CREATE_REC: return(IS_act_pg_create_rec(pg));
	case ACT_SEARCH_REC:    return(IS_act_pg_search_rec(pg));
	case ACT_MODIFY_REC:    return(IS_act_pg_modify_rec(pg));
	case ACT_NEXT_REC:      return(IS_act_pg_next_rec(pg));
	case ACT_PREV_REC:      return(IS_act_pg_prev_rec(pg));
	case ACT_DELETE_REC:    return(IS_act_pg_delete_rec(pg));

	case ACT_OPEN_ISAM:  return(IS_open((ISAM *)ptr, SCR_UPDATE, SCR_NO_LOCK));
	case ACT_CLOSE_ISAM: return(IS_close((ISAM *)ptr));
	case ACT_SCAN_ISAM :
	    SC_scan_page((PAGE *)ptr, ISAM_PTR(ACT_find_lg(aa, 1)),
			 (int)ACT_find_lg(aa, 2), 0L,
			 (int)ACT_find_lg(aa, 3));
	    break;

	case SCR_EXPORT:
	    IS_get_field((ISAM *)ptr, (int)ACT_find_lg(aa, 1), SCR_EXPORT_TXT);
	    SCR_EXPORT_YN = 1;
	    break;

	case SCR_MP_SET_CBUF: MP_set_cbuf(mp); break;

	case SCR_MP_ADD: MP_add(mp); break;

	case SCR_MP_IMPORT:
	    IS_act_mp_import(mp, PAGE_PTR(ACT_find_lg(aa, 1)));
	    break;

	case SCR_MP_IMPORT_FLD:     /* BP_M 17-06-2001 17:25 */
	    IS_act_mp_import_fld(mp, PAGE_PTR(ACT_find_lg(aa, 1)), ACT_find_lg(aa, 2));
	    break;

	case SCR_PG_IMPORT:
	    IS_act_pg_import((PAGE *)ptr, PAGE_PTR(ACT_find_lg(aa, 1)));
	    break;

	case SCR_AP_IMPORT:
	    IS_act_ap_import(ap, PAGE_PTR(ACT_find_lg(aa, 1)));
	    break;

	case SCR_IS_SEARCH:
	    return(IS_act_is_search((ISAM *)ptr, (int)ACT_find_lg(aa, 1)));

	case SCR_IS_NEXT    : return(IS_next((ISAM *)ptr));
	case SCR_IS_PREV    : return(IS_prev((ISAM *)ptr));
	case SCR_IS_DELETE  : return(IS_delete((ISAM *)ptr));
	case SCR_IS_REWRITE : return(IS_rewrite((ISAM *)ptr));
	case SCR_IS_WRITE   : return(IS_write((ISAM *)ptr)); /* BP_M 15-07-1999 */
	case SCR_IS_CLOSE   : return(IS_close((ISAM *)ptr));

	case SCR_PAGE_TO_ISAM:
	    SCR_page_to_isam(pg, ISAM_PTR(ACT_find_lg(aa, 1)));
	    break;

	case SCR_ISAM_TO_PAGE:
	    SCR_isam_to_page(pg, ISAM_PTR(ACT_find_lg(aa, 1)));
	    break;

	case SCR_MP_DELETE : MP_act_delete(mp); break;
	case SCR_MP_MODIFY : MP_act_modify(mp); break;
	case SCR_MP_VIEW   : MP_view_modify(mp, 0); break;
	case SCR_MP_CREATE : MP_add(mp); break;

	case SCR_IF_PG_SELECT : if(pg->pg_operation == 3) return(0);
	    return(-1);

	case SCR_INIT_HISTORY:  IS_init_history(); break;
	case SCR_END_HISTORY:   IS_end_history(); break;

	case SCR_BUF_SYNC : IS_resync_all(); break; /* JMP 17-12-98 */

	default: return(ACT_exec_aa_bc(aa));
    }
    return(0);
}

/*NH*/
MP_act_delete(mp)
MPAGE   *mp;
{
    if(mp->mp_delete == 0) {
	SCR_beep();
	return(0);
	}
    if(CONFIRME) return(0);
    MP_delete(mp);
    return(0);
}

/*NH*/
MP_act_modify(mp)
MPAGE   *mp;
{
    if(mp->mp_modify) MP_view_modify(mp, 2);    /* JMP_M 4.20 16-06-95 */
    else              MP_view_modify(mp, 0);    /* JMP_M 4.20 16-06-95 */

    return(0);
}


/*NH*/
IS_act_pg_create_rec(pg)
PAGE    *pg;
{
    return(SCR_pg_create_rec(pg, (ISAM *)SCR_PTR(pg->pg_isam)));
}

/*NH*/
IS_act_pg_search_rec(pg)
PAGE    *pg;
{
    int     ret = 0;

    PG_reset(pg);
    PG_display_flds_no_cmp(pg);
    ret = SCR_search_rec(pg, (ISAM *)SCR_PTR(pg->pg_isam));
    if(ret && ret != -2)  /* BP_M 03-10-2000 */
	PG_display_error(IS_error_text());
    pg->pg_operation = 3;  /* BP_M 03-06-2002 10:40 */
    return(ret);
}

/*NH*/
IS_act_pg_modify_rec(pg)
PAGE    *pg;
{
    return(SCR_modify_rec(pg, (ISAM *)SCR_PTR(pg->pg_isam)));
}

/*NH*/
IS_act_pg_next_rec(pg)
PAGE    *pg;
{
    return(SCR_next_rec(pg, (ISAM *)SCR_PTR(pg->pg_isam)));
}

/*NH*/
IS_act_pg_prev_rec(pg)
PAGE    *pg;
{
    return(SCR_prev_rec(pg, (ISAM *)SCR_PTR(pg->pg_isam)));
}

/*NH*/
IS_act_pg_delete_rec(pg)
PAGE    *pg;
{
    return(SCR_delete_rec(pg, (ISAM *)SCR_PTR(pg->pg_isam)));
}

/*NH*/
IS_act_mp_import(mp, pg)
MPAGE   *mp;
PAGE    *pg;
{
#ifdef WINDOWS
    MP_edit_isam2(mp);
#else
    MP_edit_isam(mp);
#endif
    if(SCR_LKEY != SCR_F3) {
	SCR_isam_to_page(pg, ISAM_PTR(mp->mp_isam));
	PG_display_flds_no_cmp(pg);
	}
    return(0);
}

/*NH*/
IS_act_mp_import_fld(mp, pg, fld)       /* BP_M 17-06-2001 17:26 */
MPAGE   *mp;
PAGE    *pg;
long    fld;
{
#ifdef WINDOWS
    MP_edit_isam2(mp);
#else
    MP_edit_isam(mp);
#endif
    if(SCR_LKEY != SCR_F3) {
	SCR_rec_to_page_1fld(pg, ISAM_PTR(mp->mp_isam), fld);
	PG_display_field(pg, fld);
	SCR_apply_cmp(pg, fld);
	}
    return(0);
}

/*NH*/
IS_act_pg_import(pg1, pg2)
PAGE    *pg1, *pg2;
{
    PG_edit_isam(pg1);
    SCR_isam_to_page(pg2, PG_find_isam(pg1));
    PG_display_flds_no_cmp(pg2);
    return(0);
}

/*NH*/
IS_act_ap_import(ap, pg)
APPL    *ap;
PAGE    *pg;
{
    BC  *bc;

    AP_display(ap);
    AP_edit(ap);
    AP_undisplay(ap);
    bc = (BC *)SCR_PTR(ap->ap_ptr);
    SCR_isam_to_page(pg,
	(ap->ap_type == SCR_TYPE_PAGE) ?
	    PG_find_isam(PAGE_PTR(ap->ap_ptr)) : ISAM_PTR(bc->bc_is_bc));
    PG_display_flds_no_cmp(pg);
    return(0);
}

/*NH*/
IS_act_is_search(is, cmp_nb)
ISAM    *is;
int     cmp_nb;
{
    if(IS_open(is, SCR_UPDATE, SCR_NO_LOCK) != 0 ||
       IS_search(is, cmp_nb, SCR_GE, (char *)0) != 0) { /* JMP 14-01-98 */
		PG_display_error(IS_error_text());
		return(-1);
	    }
    return(0);
}


/* ======================================================================
Edite les champs index de l'ISAM is définis dans la PAGE pg pour :

&EN déterminer l'index de recherche
&EN indiquer la valeur à trouver

Recherche le record correspondant aux informations introduites.

Voir ACTION SEARCH_REC pour plus d'explications

&RT 0 en cas de succès, -1 sinon
&SA NEXT_REC, PREV_REC, MODIFY_REC, PREV_REC
------------------------------------------------------------------------- */

SCR_search_rec(pg, is)  /* BP_M 07-10-97 */
PAGE    *pg;
ISAM    *is;
{
    int     i;

    pg->pg_operation = 4;
    i = PG_search(pg, is);
    SCR_isam_to_page(pg, is);
    PG_display_flds(pg);
    if(i < 0 && i != -5) {
	pg->pg_operation = 0;
	return(i);      /* 03-10-2000 BP_M */
	}
    pg->pg_operation = 3;       /* BP_M 19-06-2002 09:58 */

    return(0);
}

/* ===================================================
Edite un ISAM à l'aide de l'objet scr_ap_isam défini dans scr.f.
 =================================================== */

AP_edit_isam(pg)
PAGE    *pg;
{
    ISAM    *is;
    APPL    *ap;
    PAGE    *pgap;

    ap = (APPL *)AP_read("scr_ap_isam");
    if(ap == 0) {
	PG_display_error("Attention : L'application scr_ap_isam n'est pas définie");
	return(-1);
    }

    if(pg->pg_isam == 0) {
	PG_display_error("Attention : Pas d'ISAM par défault");
	return(-1);
    }

    is = ISAM_PTR(pg->pg_isam);
    if(IS_open_files(is, 0L) != 0) {
	PG_display_error(IS_error_text());
	return(-1);
    }
    SCR_READ_FILES.tbl_fl[ap->ap_ptr]->fl_ptr = (char *)pg;
    ap->ap_type = SCR_TYPE_PAGE;

    AP_run(ap);

    IS_close(is);

    return(0);
}




