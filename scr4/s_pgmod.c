#include "scr.h"

/*NH*/
/* =+====== PG_modify_text ======================================
     Modify the text of a field according to its center and case defn.
     fld    : IO : pointer to the field
     edtxt  : IO : pointer to the text
     return : 0 always
   ============================================================== */

int  PG_modify_text(pg, edtxt, fld_nb)
PAGE    *pg;
char    *edtxt;
int     fld_nb;
{
#ifndef __cplusplus
    extern long PG_fdate_to_long(), PG_ftime_to_long();
#endif
    int     i;
    char    fmt[80];
    FIELD   *fld = pg->pg_flds + fld_nb;
    int     type = fld->fld_type;
    int     flag = 0;
/*  int     lg   = fld->fld_nb_line * fld->fld_nb_column; */

    if(type == SCR_EDITOR) return(0);
    fmt[0] = 0;

    if(fld->fld_fmt != 0)
	strcpy(fmt, PG_fld_fmt(pg, fld_nb));
    if(type == SCR_DATE || type == SCR_TIME)
	SCR_fmt_dt(fmt, fmt);

    /* type */
    if(fld->fld_date == 1) {    /* SCR_TODAY */
	strcpy(fmt, PG_fld_fmt(pg, fld_nb));
	switch(type) {
	    case SCR_DATE:
		if(PG_fdate_to_long(edtxt, fmt) == 0) {
		    SCR_long_to_fdate(SCR_current_date(), edtxt, fmt);
		    flag = 1;
		}
		break;
	    case SCR_TIME:
		if(PG_ftime_to_long(edtxt, fmt) == 0) {
		    SCR_long_to_ftime(SCR_current_time(), edtxt, fmt);
		    flag = 1;
		}
		break;
	}
    }
    /* Case */
    switch(fld->fld_case) { /* JMP 25-06-91 */
	case 2 : SCR_upper(edtxt); flag = 1; break;   /* UPPER */
	case 1 : SCR_lower(edtxt); flag = 1; break;   /* LOWER */
	}

    if(fld->fld_nb_line != 1) return(0);

    /* Center */

    switch(fld->fld_center + SCR_ASIS) {
	case SCR_CENTER   : U_center_text(edtxt); flag = 1; break;
	case SCR_LJUST    : U_ljust_text(edtxt);  flag = 1; break; /* JMP 25-06-91 */
	case SCR_RJUST    : U_rjust_text(edtxt);  flag = 1; break;
	case SCR_SQZ      : U_sqz_text(edtxt);    flag = 1; break;
	}
    if(flag)
	PG_set_rec_txt(pg, fld_nb, edtxt);
    return(0);
}

