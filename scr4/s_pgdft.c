#include "scr.h"

/*NH*/
/* =+==== PG_set_dft_field ===================
    If field empty, set the default or auto
    value.
	pg : IN : pointer to the page
	fld_nb : IN : field nb
   ========================================= */
PG_set_dft_field(pg, fld_nb)
PAGE    *pg;
int     fld_nb;
{
    int     i;
    int     size;
    FIELD   *fld;
    long    num;
    ISAM    *is;

    fld = pg->pg_flds + fld_nb;

    /* check field is empty */
    if(!PG_is_fld_empty(pg, fld_nb)) return(0);

    /* set default text */
    if(fld->fld_dtext != 0) {
	if(fld->fld_type == SCR_MENU) {
	    MN_read_opt(MENU_PTR(fld->fld_menu));
	    SCR_set_field_short(pg, fld_nb,
				SCR_search_menu(MENU_PTR(fld->fld_menu), fld->fld_dtext));
	    MN_free_opt(MENU_PTR(fld->fld_menu));
	}
	else
	    PG_set_rec_txt(pg, fld_nb, fld->fld_dtext);
	return(0);
    }

    return(0);
}

