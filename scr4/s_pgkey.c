#include "scr.h"

/* =+========== PG_IS_FNKEYS ======================================
    Check an fnkey is defined for a field
	pg     : IN : pointer to the page
	fld_nb : IN : nb of the current field
	key    : IN : key pressed
    Return = 0 if the fnkey is not defined, 1 else
    =================================================================== */

PG_is_fnk(pg, fld_nb)
PAGE    *pg;
int     fld_nb;
{
    FNKEYS *fnks;

    if(SCR_LKEY >= ' ' && SCR_LKEY <= 255) return(0);

    if(fld_nb >= 0 && fld_nb < pg->pg_nb_flds) {  /* JMP 20-01-97 */
      fnks = (FNKEYS *)SCR_PTR(pg->pg_flds[fld_nb].fld_fnks);
      if(FNK_is_fnk(fnks, SCR_LKEY) >= 0) return(1);
      }
    fnks = (FNKEYS *)SCR_PTR(pg->pg_fnks);
    if(FNK_is_fnk(fnks, SCR_LKEY) >= 0) return(1);

/*    if(FNK_is_fnk((FNKEYS *)SCR_PTR(pg->pg_flds[fld_nb].fld_fnks), SCR_LKEY) >= 0 ||
       FNK_is_fnk((FNKEYS *)SCR_PTR(pg->pg_fnks), SCR_LKEY) >= 0) return(1);
/* JMP 20-01-97 */

    return(0);
}

/* */
/* =+========== PG_FNKEYS ======================================
    Execute the fnkeys : first those of the field if any, then
    those of the page
	pg     : IN : pointer to the page
	fld_nb : IN : nb of the current field
	key    : IN : key pressed
    Return = 0 if the field must not be leaved, 1 else
    =================================================================== */

PG_fnkeys(pg, fld_nb)
PAGE    *pg;
int     fld_nb;
{
    int     act = 0;
    FIELD   *fld;
    FNKEYS  *fk;
    int     i;

    if(fld_nb >= 0 && fld_nb < pg->pg_nb_flds) {  /* JMP 20-01-97 */
	fld = pg->pg_flds + fld_nb;
	fk = (FNKEYS *)SCR_PTR(fld->fld_fnks);
	i = FNK_is_fnk(fk, SCR_LKEY);
	if(i >= 0) {
	    act = fk->fk_fnks[i].fnk_act;
	    goto found;
	    }
	}

    fk = (FNKEYS *)SCR_PTR(pg->pg_fnks);
    i = FNK_is_fnk(fk, SCR_LKEY);
    if(i < 0) {
	SCR_beep();
	return(0);
    }
    else
	act = fk->fk_fnks[i].fnk_act;

found:
   if(act == 0)
	return(1);

    return(ACT_exec_nu(act));  /* BP_M 25-10-95 */
}







