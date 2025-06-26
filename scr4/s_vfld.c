#include "scr4.h"       /* BP_M 11-09-1997 */


IS_set_vfield(is, fld_nb, txt, len)
ISAM    *is;
int     fld_nb;
char    *txt;
long    len;
{
    return(IS_set_vfield_Ex(is, fld_nb, txt, len, 0));
}

IS_rset_vfield(is, fld_nb, txt, len)
ISAM    *is;
int     fld_nb;
char    *txt;
long    len;
{
    return(IS_set_vfield_Ex(is, fld_nb, txt, len, 1));
}

/* ====================================================================
Fixe la valeur d'un champ de longueur variable du record courant d'un
ISAM à partir d'un pointeur.

Aucune vérification n'est effectuée sur le type des
champs.

&EX
    char    name[51];

    strcpy(name, "Durand");
    IS_set_vfield(is_client, 0, name, (long)(strlen(name) + 1));
&TX
&SA IS_get_vfield(), IS_set_field(), SCR_get_*(), accès aux valeurs
directes des ISAM
=======================================================================*/

IS_set_vfield_Ex(is, fld_nb, txt, len, type)
ISAM    *is;
int     fld_nb;
char    *txt;
long    len;
int     type;
{
    ISFIELD *isf;
    int     i, nu_vfld = 0;
    VFIELD  *vfld;

    isf = is->is_flds + fld_nb;
    if(!isf->isf_var) return(0);

    for(i = 0 ; i < fld_nb ; i++)
	if(is->is_flds[i].isf_var) nu_vfld++;

    vfld       = is->is_vfld + nu_vfld;
    if(type == 0)
	vfld->vf_lg = len;
    else
	vfld->vf_rlg = len;

    if(isf->isf_type == SCR_EDITOR || isf->isf_type == SCR_RAW) {

	if(type == 0) {
	    SCR_free(vfld->vf_ptr);
	    vfld->vf_ptr = SCR_malloc(len);
	}
	else {
	    SCR_free(vfld->vf_rptr);
	    vfld->vf_rptr = SCR_malloc(len);
	}

    }
    if(type == 0) {
	if(txt) memcpy(vfld->vf_ptr, txt, len); /* JMP 29-12-98 */
    }
    else {
	if(txt) memcpy(vfld->vf_rptr, txt, len); /* JMP 29-12-98 */
    }
    return(0);
}

/* ====================================================================
Retourne la longueur d'un champ de longueur variable du record courant
d'un ISAM.

&RT longueur du champ ou -1 en cas d'erreur (record de longueur fixe)

&SA IS_get_vfield(), IS_set_vfield(), SCR_get_*()
=======================================================================*/

long IS_get_len_vfield_Ex(is, fld_nb, type)  /* BP_M 13-07-95 */
ISAM    *is;
int     fld_nb;
int     type;
{
    int     i;
    VFIELD  *vfld;

    if(!is->is_flds[fld_nb].isf_var) return(-1);

    vfld = is->is_vfld;
    for(i = 0 ; i < fld_nb ; i++)
	if(is->is_flds[i].isf_var) vfld++;

    if(type == 0)
	return(vfld->vf_lg);
    else
	return(vfld->vf_rlg);
}

long IS_get_len_vfield(is, fld_nb)  /* BP_M 13-07-95 */
ISAM    *is;
int     fld_nb;
{
    return(IS_get_len_vfield_Ex(is, fld_nb, 0));  /* BP_M 13-07-95 */
}

long IS_rget_len_vfield(is, fld_nb)  /* BP_M 13-07-95 */
ISAM    *is;
int     fld_nb;
{
    return(IS_get_len_vfield_Ex(is, fld_nb, 1));  /* BP_M 13-07-95 */
}

/*NH*/
IS_get_nuvfld(is, fld)      /* BP_M 12-08-1997 */
ISAM    *is;
int     fld;
{
    ISFIELD *isf;
    int     i, nu_vfld = 0;


    isf = is->is_flds + fld;
    if(!isf->isf_var) return(-1);
    for(i = 0 ; i < fld ; i++)
	if(is->is_flds[i].isf_var) nu_vfld++;
    return(nu_vfld);
}

/*NH*/
ISFIELD *IS_get_vfld_isf(is, vfld)
ISAM    *is;
int     vfld;
{
    ISFIELD *isf;
    int     i, nu_vfld = 0;


    isf = is->is_flds;
    for(i = 0 ; i < is->is_nb_flds ; i++, isf++) {
	if(isf->isf_var) {
	    if(nu_vfld == vfld) return(isf);
	    nu_vfld++;
	}
    }
    return((ISFIELD *)0);
}

IS_get_vfield(is, fld_nb, ptr)
ISAM    *is;
int     fld_nb;
char    **ptr;
{
    return(IS_get_vfield_Ex(is, fld_nb, ptr, 0));
}

IS_rget_vfield(is, fld_nb, ptr)
ISAM    *is;
int     fld_nb;
char    **ptr;
{
    return(IS_get_vfield_Ex(is, fld_nb, ptr, 1));
}

/* ====================================================================
Retourne dans *ptr le pointeur vers la valeur d'un champ de longueur
variable du record courant d'un ISAM.

Si le record courant a été modifié depuis sa lecture, c'est la valeur
modifiée qui est utilisée.

&RT 0 en cas de succès, -1 en cas d'erreur (ISAM de longueur fixe)

&EX
    char    *name;

    IS_get_vfield(is_client, 0, &name);
&TX
&SA IS_get_field(), IS_set_vfield(), SCR_get_*()
=======================================================================*/

IS_get_vfield_Ex(is, fld_nb, ptr, type)
ISAM    *is;
int     fld_nb;
char    **ptr;
int     type;
{
    ISFIELD *isf;
    int     i;
    short   j;
    VFIELD  *vfld;

    isf = is->is_flds + fld_nb;
    if(!isf->isf_var) return(-1);
    vfld = is->is_vfld;
    for(i = 0 ; i < fld_nb ; i++)
	if(is->is_flds[i].isf_var) vfld++;

    if(vfld->vf_ptr != 0L) {
	if(type == 0)
	    *ptr = vfld->vf_ptr;
	else
	    *ptr = vfld->vf_rptr;
    }
    else
	*ptr = 0L;

    return(0);
}

/*NH*/
IS_compose_vrec(is, lenrec, vrec)
ISAM    *is;
long    *lenrec;
char    **vrec;
{
    int     i;
    long    l, len = 0;
    char    *tmprec, *ptr;
    VFIELD  *vfld;
    short   s;

    if(!is->is_varlen || !is->is_nb_vfld) return(0);

    len = sizeof(short);
    vfld = is->is_vfld;
    for(i = 0 ; i < is->is_nb_vfld ; i++, vfld++) {
	if(vfld->vf_ptr)
	    len += vfld->vf_lg;
	len += sizeof(long);
    }

    tmprec = SCR_malloc(len);
    *lenrec = len;

    s = is->is_nb_vfld;
    *(short *)tmprec = s;

    vfld = is->is_vfld;
    len  = sizeof(short);
    for(i = 0 ; i < is->is_nb_vfld ; i++, vfld++) {
	if(vfld->vf_lg) {
	    *(long *)(tmprec + len) = vfld->vf_lg;
	    memcpy(tmprec + len + sizeof(long), vfld->vf_ptr, vfld->vf_lg);
	}
	else {
	    l = 0;
	    *(long *)(tmprec + len) = l;
	}
	len += vfld->vf_lg + sizeof(long);
    }
    *vrec = tmprec;
    return(0);
}

/*NH*/
IS_decompose_vrec(is, vrec, rec_len)
ISAM    *is;
char    *vrec;
long    rec_len;
{
    int     i;
    short   nb_vfld;
    ISFIELD *isf;
    char    *ptr;
    VFIELD  *vfld;
    long    l, len = 0;

    if(!is->is_varlen) return(0);

    memcpy(&nb_vfld, vrec, sizeof(short));
    vrec += sizeof(short);
    IS_free_vptr(is);       /* BP_M 26-10-95 */
    vfld = is->is_vfld;
    len = sizeof(short);
    for(i = 0 ; i < min(nb_vfld, is->is_nb_vfld) && len < rec_len ; i++, vfld++) { /* JMP 14-01-98 */
/*    for(i = 0 ; i < min(nb_vfld, is->is_nb_vfld) , len < rec_len ; i++, vfld++) { /* JMP 14-01-98 */

	isf = IS_get_vfld_isf(is, i);

	l = *(long *)vrec;
	vfld->vf_lg = l;

	vrec += sizeof(long);

	if(l > 0L) {
	    if(isf->isf_type == SCR_EDITOR || isf->isf_type == SCR_RAW)
		vfld->vf_ptr = SCR_malloc(l);
	    memcpy(vfld->vf_ptr, vrec, l);
	}
	len  += sizeof(long) + l;
	vrec += l;
    }
    return(0);
}

/*NH*/
IS_free_vptr(is)        /* BP_M 26-10-95 */
ISAM    *is;
{
    ISFIELD *isf;
    int     i;
    VFIELD  *vfld;

    isf  = is->is_flds;
    vfld = is->is_vfld;
    for(i = 0 ; i < is->is_nb_flds ; i++, isf++) {
	if(isf->isf_var) {

	    if(isf->isf_type == SCR_EDITOR || isf->isf_type == SCR_RAW) {
		SCR_free(vfld->vf_ptr);
		vfld->vf_ptr = 0;
		vfld->vf_lg = 0;
	    }
	    else
		memset(vfld->vf_ptr, 0, vfld->vf_lg);
	    vfld++;
	}
    }
    return(0);
}

/*NH*/
IS_alloc_vfld(is)
ISAM    *is;
{
    VFIELD  *vfld;
    int     i;
    ISFIELD *isf;
    long    l;

    if(!is->is_varlen) return(0);

    is->is_vfld = (VFIELD *)SCR_malloc(is->is_nb_vfld * sizeof(VFIELD));     /* BP_M 12-09-1997 */

    /* BP_M 12-09-1997 */
    vfld = is->is_vfld;
    for(i = 0, isf = is->is_flds ; i < is->is_nb_flds ; i++, isf++) {
	if(isf->isf_var) {
	    if(isf->isf_type != SCR_EDITOR && isf->isf_type != SCR_RAW) {
		l = isf->isf_length;
		vfld->vf_lg  = l;
		vfld->vf_rlg  = l;
		vfld->vf_ptr = SCR_palloc((unsigned int)l);
		vfld->vf_rptr = SCR_palloc((unsigned int)l);
	    }
	    vfld++;
	}
    }
    return(0);
}
