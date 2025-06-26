#include "scr.h"

/* ===================================================================
Fixe la couleur de tous les champs d'une PAGE ainsi que le fond de PAGE.

&NO La PAGE concernée doit avoir ses champs en mémoire (FLD_MEM) pour
    garder ses attributs lors de l'affichage. De plus, il n'est plus
    possible de revenir aux valeurs antérieures des attributs.

=================================================================== */

PG_set_color(pg, attr)
PAGE    *pg;
int     attr;
{
    int     i;

    if(!pg->pg_fld_mem) PG_read_flds(pg);

    pg->pg_back = attr;
    for(i = 0 ; i < pg->pg_nb_flds; i++)
	pg->pg_flds[i].fld_attributes = attr;

    return(0);
}



