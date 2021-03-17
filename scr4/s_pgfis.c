#include "scr.h"

ISAM *PG_find_isam(pg)
PAGE    *pg;
{
    FIELD   *fld;
    int     i;

    if(pg->pg_isam != 0) return((ISAM *)SCR_PTR(pg->pg_isam));
    PG_read_flds(pg);
    for(i = 0, fld = pg->pg_flds; i < pg->pg_nb_flds ; i++, fld++)
	if(fld->fld_nb_isam != 0)
	    return((ISAM *)SCR_PTR(fld->fld_isam[0].if_isam));

    return((ISAM *)0);
}

