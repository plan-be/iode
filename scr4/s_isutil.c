#include "scr4.h"

/*NH*/
/************ Ensemble de fonctions utilitaires sur les ISAM extraites
    de s_isam.c et placées dans scr4o
*********************************************************************/

IS_get_max_field(is, fld_nb, ptr)
ISAM    *is;
int     fld_nb;
char    *ptr;
{
    ISFIELD *isf;

    isf = is->is_flds + fld_nb;
    memcpy(ptr, is->is_maxrec + isf->isf_start, isf->isf_length);
    return(0);
}


/* ====================================================================
Copie la valeur d'un champ du record courant d'un ISAM dans un pointeur.
Aucune vérification n'est effectuée sur la longueur ou le type des
champs.

Si le record courant a été modifié depuis sa lecture, c'est la valeur
modifiée qui est utilisée.

&EX
    char    name[51];

    IS_get_field(is_client, 0, name);
&TX
&SA IS_get_read_field(), IS_set_field(), SCR_get_*(), accès aux valeurs
directes des ISAM
=======================================================================*/

char *IS_get_field(is, fld_nb, ptr) /* BP_M 15-12-1999  Modif pour compatibilité v2 */
ISAM    *is;
int     fld_nb;
char    *ptr;
{
    ISFIELD *isf;

    isf = is->is_flds + fld_nb;
    memcpy(ptr, is->is_rec + isf->isf_start, isf->isf_length);
    return(ptr);
}

/* ====================================================================
Copie la valeur d'un champ du record courant d'un ISAM dans un pointeur.
Aucune vérification n'est effectuée sur la longueur ou le type des
champs.

Même si le record courant a été modifié depuis sa lecture, c'est la valeur
du fichier qui est utilisée.


&EX
    char    name[51];

    IS_get_read_field(is_client, 0, name);
&TX
&SA IS_get_field(), IS_set_field(), SCR_get_*(), accès aux valeurs
directes des ISAM
=======================================================================*/

IS_get_read_field(is, fld_nb, ptr)
ISAM    *is;
int     fld_nb;
char    *ptr;
{
    ISFIELD *isf;

    isf = is->is_flds + fld_nb;
    memcpy(ptr, is->is_rrec + isf->isf_start, isf->isf_length);
    return(0);
}

/* ====================================================================
Fixe la valeur d'un champ du record courant d'un ISAM à partir d'un
pointeur.

Aucune vérification n'est effectuée sur la longueur ou le type des
champs. Si l'espace du pointeur est trop petit, on peut en UNIX obtenir
un "segmentation violation".

&EX
    char    name[51];

    strcpy(name, "Durand");
    IS_set_field(is_client, 0, name);
&TX
&SA IS_get_read_field(), IS_get_field(), SCR_get_*(), accès aux valeurs
directes des ISAM
=======================================================================*/

IS_set_field(is, fld_nb, ptr)
ISAM    *is;
int     fld_nb;
char    *ptr;
{
    ISFIELD *isf;

    isf = is->is_flds + fld_nb;
    /* BP_M 25-09-97 */
    if(isf->isf_var) {
	IS_set_vfield(is, fld_nb, ptr, isf->isf_length);
	return(0);
    }
    memcpy(is->is_rec + isf->isf_start, ptr, isf->isf_length);
    if(isf->isf_type == SCR_ZSTRING) IS_check_zstring(is, fld_nb);
    return(0);
}

/*NH BP_M 02-10-97 */
IS_set_max_field(is, fld_nb, ptr)
ISAM    *is;
int     fld_nb;
char    *ptr;
{
    ISFIELD *isf;

    isf = is->is_flds + fld_nb;
    /* BP_M 25-09-97 */
    if(isf->isf_var) {
	IS_set_vfield(is, fld_nb, ptr, isf->isf_length);
	return(0);
    }
    memcpy(is->is_maxrec + isf->isf_start, ptr, isf->isf_length);
    if(isf->isf_type == SCR_ZSTRING) IS_check_max_zstring(is, fld_nb);
    return(0);
}

/*NH*/
IS_check_all_zstring(is)
ISAM    *is;
{
    int     i;
    ISFIELD *isf = is->is_flds;

    for(i = 0 ; i < is->is_nb_flds ; i++, isf++)
	if(isf->isf_type == SCR_ZSTRING)
	    IS_check_zstring(is, i);

    return(0);
}

/*NH*/
IS_check_zstring(is, isf_nb)
ISAM    *is;
int     isf_nb;
{
    ISFIELD *isf;

    isf = is->is_flds + isf_nb;

    SCR_zstrip(is->is_rec    + isf->isf_start, isf->isf_length);
    SCR_zstrip(is->is_rrec   + isf->isf_start, isf->isf_length);
    SCR_zstrip(is->is_rrrec  + isf->isf_start, isf->isf_length);    /* BP_MODIF 27-10-98 */
    SCR_zstrip(is->is_maxrec + isf->isf_start, isf->isf_length);
    return(0);
}

/*NH*/
IS_check_all_string(is) /* JMP 29-12-01 */
ISAM    *is;
{
    int     i;
    ISFIELD *isf = is->is_flds;

    for(i = 0 ; i < is->is_nb_flds ; i++, isf++)
	if(isf->isf_type == SCR_STRING)
	    IS_check_string(is, i);

    return(0);
}

/*NH*/
IS_check_string(is, isf_nb) /* JMP 29-12-01 */
ISAM    *is;
int     isf_nb;
{
    ISFIELD *isf;

    isf = is->is_flds + isf_nb;

    SCR_strip_lg(is->is_rec    + isf->isf_start, isf->isf_length);
    SCR_strip_lg(is->is_rrec   + isf->isf_start, isf->isf_length);
    SCR_strip_lg(is->is_rrrec  + isf->isf_start, isf->isf_length);
    SCR_strip_lg(is->is_maxrec + isf->isf_start, isf->isf_length);
    return(0);
}


/*NH*/
IS_check_max_zstring(is, isf_nb)
ISAM    *is;
int     isf_nb;
{
    ISFIELD *isf;

    isf = is->is_flds + isf_nb;

    SCR_zstrip(is->is_maxrec + isf->isf_start, isf->isf_length);
    return(0);
}

/*NH*/
IS_save_rec(is)
ISAM    *is;
{
    VFIELD  *vfld;
    ISFIELD *isf;
    int     i;

    memcpy(is->is_rec, is->is_rrec, is->is_rec_len);

    if(is->is_varlen == 0) return(0);

    vfld = is->is_vfld;
    for(i = 0, isf = is->is_flds ; i < is->is_nb_flds ; i++, isf++) {
	if(isf->isf_var) {
	    if(vfld->vf_ptr && vfld->vf_rptr) {
		if(isf->isf_type == SCR_EDITOR ||isf->isf_type == SCR_RAW)
		    if(vfld->vf_lg < vfld->vf_rlg) {
			SCR_free(vfld->vf_ptr);
			vfld->vf_ptr = SCR_malloc(vfld->vf_rlg);
			vfld->vf_lg = vfld->vf_rlg;
		    }
		memcpy(vfld->vf_ptr, vfld->vf_rptr, vfld->vf_rlg);
	    }
	    vfld++;
	}
    }
    return(0);
}


/* ====================================================================
Annule tous les champs du record de l'ISAM is en fixant toutes les
valeurs des champs à 0. Les champs de type caractère sont également
fixés à 0 et non à blanc.

&SA IS_empty_field()
=======================================================================*/
IS_empty_rec(is)    /* BP_M 08-10-97 */
ISAM    *is;
{
    memset(is->is_rec, 0, is->is_rec_len);
    /* BP_M 02-10-97 */
    IS_empty_max_rec(is);
    if(is->is_varlen) IS_free_vptr(is);      /* BP_M 26-10-95 */
    return(0);
}

/*NH BP_M 08-10-97 */
IS_empty_max_rec(is)
ISAM    *is;
{
    memset(is->is_maxrec, 0, is->is_rec_len);
    return(0);
}

/* ====================================================================
Annule le champ isf_nb du record de l'ISAM is en fixant la valeur
de ce champ à 0. Les champs de type caractère sont également
fixés à 0 et non à blanc.

&SA IS_empty_rec()
=======================================================================*/
IS_empty_field(is, isf_nb)
ISAM    *is;
int     isf_nb;
{
    ISFIELD *isf;

    isf = is->is_flds + isf_nb;
    if(!isf->isf_var)       /* BP_M 26-10-95 */
	memset(is->is_rec + isf->isf_start, 0, isf->isf_length);

    return(0);
}

/*NH*/
IS_is_fld_empty(ptr, type, lg)
char    *ptr;
int     type, lg;
{
    int     j;
    char    ch;

    if(type == SCR_ZSTRING) {   /* BP_M 09-04-2007 */
	if(strlen(ptr) == 0) return(1);
	else                 return(0);
    }
    if(type == SCR_STRING || type == SCR_CHAR ||
			     type == SCR_ZSTRING) {
	for(j = 0 ; j < lg ; j++) {
	    if(type == SCR_ZSTRING && ptr[j] == 0) return(1);  /* BP_M 25/1/2001 */
	    if(ptr[j] != 0 && ptr[j] != ' ') return(0);
	    }
	}
    else {
	for(j = 0 ; j < lg ; j++)
	    if(ptr[j] != 0) return(0);
	}

    return(1);
}

/*NH*/
IS_is_isf_empty(is, isf)
ISAM    *is;
ISFIELD *isf;
{
    return(IS_is_fld_empty(is->is_rec + isf->isf_start,
			   isf->isf_type, isf->isf_length));
}









