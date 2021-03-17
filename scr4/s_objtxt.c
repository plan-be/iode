#include "scr.h"

int  IS_REC_RREC = 0;

/* ======================================================================
Lit la valeur du iŠme champ qui suit l'objet pass‚ comme argument. Cet
objet peut faire partie d'une PAGE ou d'un ISAM.

Si le texte est plus long que le champ, seules le nombre de caractŠres
du champ est copi‚. S'il est plus court, le champ est prolong‚ par des
blancs.

&SA SCR_get_vchar(), SCR_set_vchar(), SCR_rget_vchar()
------------------------------------------------------------------------ */
char *SCR_get_vtext(obj, i, val)
OBJ     obj;
int     i;
void    *val;
{
    ISAM    *is;
    int     fld_nb = obj.obj_fld + i;
    char    *txt = val;  /* JMP 16-01-98 */

    if(obj.obj_type == SCR_TYPE_PAGE)
	PG_get_rec((PAGE *)obj.obj_ptr, fld_nb, val);
    else {
	is = (ISAM *)obj.obj_ptr;
	if(IS_REC_RREC == 0)
	    IS_get_field(is, fld_nb, val);
	else {
	    IS_get_read_field(is, fld_nb, val);
	    IS_REC_RREC = 0;
	}
	if(is->is_flds[fld_nb].isf_type == SCR_STRING)
	    txt[is->is_flds[fld_nb].isf_length] = 0;  /* JMP 16-01-98 */
    }
    return(txt);   /* JMP 16-01-98 */
}


/* ======================================================================
Place un texte dans un objet. L'objet peut ˆtre un champ d'ISAM ou de
PAGE. Si le texte est trop grand pour le champ, il est tronqu‚. S'il est
trop court, il est prolong‚ avec des blancs.

&TX
&SA SCR_get_vchar(), SCR_set_vchar(), SCR_rget_vchar()
------------------------------------------------------------------------ */
SCR_set_vtext(obj, i, txt)
OBJ     obj;
int     i;
void    *txt;
{
    if(obj.obj_type == SCR_TYPE_PAGE)
	PG_set_rec((PAGE *)obj.obj_ptr, obj.obj_fld + i, txt);
    else
	IS_set_field((ISAM *)obj.obj_ptr, obj.obj_fld + i, txt);

    return(0);
}


