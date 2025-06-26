#include "scr.h"

/* ======================================================================
Place dans le champ EDITOR fld_nb de la page pg le texte text. L'ancien
texte est libéré. Le nouveau n'est cependant pas alloué. Le séparateur de
lignes doit être \n.
&NO
    Il n'y a pas de vérification de type !
------------------------------------------------------------------------- */
SCR_set_field_editor(pg, fld_nb, text)
PAGE    *pg;
int     fld_nb;
U_ch    *text;
{
    char    **ptr;

    PG_read_flds(pg);

    ptr = (char **)PG_get_text(pg, fld_nb);
    SCR_free(ptr[0]);
    ptr[0] = text;
    return(0);
}

