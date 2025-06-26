#include "scr4.h"

/*NH*/
/* =+========== SCR_EDIS_CREATE ========================================
    Loop on the creation of new records.
    pg      : IN : pointer to the PAGE
    is      : IN : ISAM pointer
    return : 0 is ok, error code else.
   =================================================================== */

SCR_edis_create(pg, is)
PAGE    *pg;
ISAM    *is;
{
    int     key;
    IMAGE   *im;

    PG_read_flds(pg);
    IS_empty_rec(is);
    while(1) {
	PG_display_flds_no_cmp(pg);
	PG_edit(pg);
	if(pg->pg_abort) return(0);     /*JMP 15-05-95*/
	key = pg->pg_last_key;
	if(SCR_app_key()) return(0);
	switch(key) {
	    case    SCR_ESCAPE :
	    case    SCR_F3  :
		return(0);

	    case    SCR_F4  :
		PG_reset(pg);
		break;

	    case    SCR_F10 :
		if(SCR_edis_create_F10(pg, is) == 0) return(0); /* JMP 16-04-97 */
		break;

	    default :
		SCR_beep();
		break;
	    }
       }
}

/* ======================================================================
Edite le record courant de l'ISAM is défini à partir de la page pg.
Sauve le record modifié.

Voir ACTION MODIFY_REC pour plus d'explications
&RT 0 en cas de succès, -1 sinon
&SA NEXT_REC, PREV_REC, DELETE_REC, MODIFY_REC
------------------------------------------------------------------------- */

SCR_modify_rec(pg, is)
PAGE    *pg;
ISAM    *is;
{
    if(pg->pg_operation != 3) {     /* BP_M 03-06-2002 10:40 */
	PG_display_error(SCR_err_txt(114));
	return(-1);
    }
    pg->pg_operation = 2;
    PG_edit(pg);
    pg->pg_operation = 3;
    if(pg->pg_abort) return(-1);        /*JMP 15-05-95*/
    if(SCR_app_key()) return(-1);
    SCR_page_to_isam(pg, is);

    if(IS_rewrite(is) != 0) {
	PG_display_error(IS_error_text());
	return(-1);
    }
    return(0);
}


/* ======================================================================
Edite la PAGE pg et crée un nouveau record l'ISAM défini par cette PAGE. Si
la touche de fonction utilisée pour quitter la PAGE est F10, sauve le
record défini dans la PAGE dans l'ISAM.

Voir ACTION PG_CREATE_REC pour plus d'explications
&RT 0 en cas de succès, -1 sinon
&SA PG_CREATE_REC
------------------------------------------------------------------------- */

PG_create_rec(pg)
PAGE    *pg;
{
    ISAM    *is;
    STATUS  *sta;

    PG_read_flds(pg);
    is = PG_find_isam(pg);
    if(is == 0) return(-1);
    if(IS_open_files(is, (ISAM *)0) != 0) return(-1);
    sta = SCR_save_status();
    PG_display_im(pg);

    pg->pg_operation = 1;
    SCR_edis_create(pg, is);
    pg->pg_operation = 0;

    PG_undisplay(pg);
    IS_close(is) ;
    SCR_reset_status(sta);
    return(0);
}

