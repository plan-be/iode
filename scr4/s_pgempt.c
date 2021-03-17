#include "scr.h"

/*NH*/
/* =+====== PG_is_fld_empty =======================================
     Return 1 if field empty, 0 otherwise
     pg    : IN : pointer to the page
     fld   : IN : field nb
   ============================================================== */

PG_is_fld_empty(pg, fld_nb)     /* BP_M 07-11-1998 */
PAGE    *pg;
int     fld_nb;
{
    FIELD   *fld = pg->pg_flds + fld_nb;
    char    buf[80];
    extern  int SCR_SQL_CHECK_TODAY;    /* BP_M 07-11-1998 */
    long    l;

    switch(pg->pg_flds[fld_nb].fld_type) {
	case SCR_NATURAL:
	case SCR_INTEGER:
	    return(SCR_get_field_long(pg, fld_nb) == 0L ? 1:0);
	case SCR_DATE:
	    l = SCR_get_field_long(pg, fld_nb);
	    if(SCR_SQL_CHECK_TODAY && l == SCR_current_date()) return(1);
	    else {
		if(l == 0L) return(1);
		else        return(0);
	    }
	case SCR_TIME:
	    l = SCR_get_field_long(pg, fld_nb);
	    if(SCR_SQL_CHECK_TODAY && l == SCR_current_time()) return(1);
	    else {
		if(l == 0L) return(1);
		else        return(0);
	    }
	case SCR_REAL:
	    return(SCR_get_field_double(pg, fld_nb) == 0.0 ? 1:0);
	case SCR_TYPE_MENU:
	    return(0);
	default:
	    if(!fld->fld_fmt)
		return(SCR_is_empty(PG_get_text(pg, fld_nb)));

	    /* BP_M 03-10-97 */
	    PG_get_rec(pg, fld_nb, buf); buf[FLD_len_in_rec(fld)] = 0;
	    return(PG_is_fmt_empty(buf, PG_fld_fmt(pg, fld_nb)));
    }
}

/*NH*/
SCR_is_empty(text)
char    *text;
{
    while(*text == ' ') text++;
    return(*text == 0 ? 1:0);
}

/*NH*/     /* JMP 09-05-97 */
PG_check_required(pg)
PAGE    *pg;
{
    int     i;
    FIELD   *fld;

    for(i = 0 ; i < pg->pg_nb_flds ; i++) {
	fld = pg->pg_flds + i;
	if((fld->fld_io == 0 || fld->fld_io == SCR_ACCESSIBLE - SCR_INPUT) &&
	    fld->fld_compulsory == SCR_REQUIRED - SCR_NOT_COMPULSORY &&
	    fld->fld_type != SCR_EDITOR &&
	    PG_is_fld_empty(pg, i) ) {
		pg->pg_csr_fld = i;
		return(-1);
		}
	}
    return(0);
}

