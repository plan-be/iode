#include "scr.h"

/*NH*/
PG_set_auto_num(pg, is1)
PAGE    *pg;
ISAM    *is1;
{
    FIELD   *fld = pg->pg_flds;
    int     i;
    ISAM    *is2;
    long    num;

    for(i = 0 ; i < pg->pg_nb_flds ; i++, fld++) {
	if(fld->fld_nb_isam == 0) continue;
	is2 = (ISAM *)SCR_PTR(fld->fld_isam[0].if_isam);
	if(is1 == is2 &&
	   (is2->is_flds + fld->fld_isam[0].if_isam_nb)->isf_type == SCR_AUTO &&
	   SCR_get_field_long(pg, i) == 0) {
		IS_calc_auto_num(is1);
		IS_get_field(is1, fld->fld_isam[0].if_isam_nb, &num);
		SCR_set_field_long(pg, i, num);
		PG_display_field(pg, i);
	}
    }

    return(0);
}





