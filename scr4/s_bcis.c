#include <scr4.h>

/*NH*/
BC_edis_create1(bc, retry)
BC      *bc;
int     retry;
{
    PAGE    *apg = (PAGE *)SCR_PTR(bc->bc_apg);

    if(retry == 0) {
	BC_empty(bc);
	BC_display_flds(bc);
	}
    apg->pg_operation = 1;
    bc->bc_operation = 1;
    return(0);
}

/*NH*/
BC_edis_create2(bc)
BC      *bc;
{
    PAGE    *apg = (PAGE *)SCR_PTR(bc->bc_apg);

    bc->bc_operation = 0;
    apg->pg_operation = 0;
    if(SCR_app_key()) return(0); /* JMP 13-09-91 */
    if(SCR_LKEY != SCR_F10) return(0);
    if(BC_rewrite_bc(bc) != 0) return(1);
    return(0);
}


/*NH*/
BC_next_bc(bc)
BC      *bc;
{
    if(bc->bc_operation != 2) {
	PG_display_error(SCR_err_txt(114));
	return(-1);
    }
    if(BC_next(bc) != 0) {
	PG_display_error(IS_error_text());
	return(-1);
    }
    BC_display_flds(bc);
    return(0);
}

/*NH*/
BC_prev_bc(bc)
BC      *bc;
{
    if(bc->bc_operation != 2) {
	PG_display_error(SCR_err_txt(114));
	return(-1);
    }
    if(BC_prev(bc) != 0) {
	PG_display_error(IS_error_text());
	return(-1);
    }
    BC_display_flds(bc);
    return(0);
}

/*NH*/
BC_modify_bc(bc)
BC      *bc;
{
    PAGE    *apg = (PAGE *)SCR_PTR(bc->bc_apg);

GMSGC;
    if(bc->bc_operation != 2) {
	PG_display_error(SCR_err_txt(114));
	return(-1);
    }
    apg->pg_operation = 3;
    BC_edit(bc);
    apg->pg_operation = 2;

    return(0);
}

/*NH*/
BC_delete_bc(bc)
BC      *bc;
{
    PAGE    *apg = (PAGE *)SCR_PTR(bc->bc_apg);

    if(bc->bc_operation != 2) {
	PG_display_error(SCR_err_txt(114));
	return(-1);
    }
    if(CONFIRME) return(-1);
    if(BC_delete(bc) != 0) {
	PG_display_error(IS_error_text());
	return(-1);
    }
    BC_empty(bc);
    BC_display_flds(bc);
    apg->pg_operation = 0;

    return(0);
}

/* ====================================================================
Retourne le nombre de lignes du BC bc courant.

&EX
    AD_bc_link()
    {
	int     i;
	long    tot = 0L, totg = 0L;

	for(i = 0 ; i < BC_nb_lbcs(bc_adr) ; i++) {
	    tot += bc_adr_NB(i);
	    totg += bc_adr_pNB(i);
	    }
	ap_bc_adr_TOTAL = tot;
	ap_bc_adr_TOTG  = totg;
	PG_display_flds_no_cmp(ap_bc_adr);
    }
&TX
&SA BC_get_lbc_field(), accès aux valeurs directes des BC
=======================================================================*/

BC_nb_lbcs(bc)
BC  *bc;
{
    return(bc->bc_nb_lbc);
}


/* ====================================================================
Retourne le pointeur vers le record ISAM mémorisé correspondant à la
ligne i du BC bc.

Si la ligne i n'existe pas, retourne un pointeur nul.

&SA BC_get_lbc_field(), BC_lpg_rec(), accès aux valeurs directes des BC
=======================================================================*/

char *BC_lbc_rec(bc, i)
BC      *bc;
int     i;
{
    BCLIST  *bcl;
    if(i >= bc->bc_nb_lbc) return((char *)0);

    bcl = BCL_locate(bc->bc_list, i);
    return(bcl->bcl_rec);
}

/* ====================================================================
Retourne le pointeur vers le record de la LPG mémorisé correspondant à
la ligne i du BC bc.

Si la ligne i n'existe pas, retourne un pointeur nul.

&SA BC_get_lbc_field(), BC_lbc_rec(), accès aux
    valeurs directes des BC
=======================================================================*/

char *BC_lpg_rec(bc, i)
BC      *bc;
int     i;
{
    BCLIST  *bcl;
    if(i >= bc->bc_nb_lbc) return((char *)0);

    bcl = BCL_locate(bc->bc_list, i);
    return(bcl->bcl_pgrec);
}

/* ====================================================================
Copie à l'adresse ptr la valeur du champ fld_nb du record ISAM
correspondant à la ligne line_nb du BC bc. Aucune vérification n'est
effectuée sur la longueur ou le type du champ.

Si la ligne line_nb n'existe pas, il n'y a pas de copie effectuée.

&EX
    char    name[51];

    BC_get_lbc_field(bc_client, 0, 0, name);
&TX
&SA IS_get_field(), IS_set_field(), SCR_get_*(), accès aux valeurs
    directes des BC
=======================================================================*/

BC_get_lbc_field(bc, line_nb, fld_nb, ptr)
BC      *bc;
int     line_nb, fld_nb;
char    *ptr;
{
    ISAM    *is;
    char    *is_rec;

    if(line_nb >= bc->bc_nb_lbc) return(0);

    is = (ISAM *)SCR_PTR(bc->bc_is_lbc);
    is_rec = is->is_rec;
    is->is_rec = BC_lbc_rec(bc, line_nb);
    IS_get_field(is, fld_nb, ptr);
    is->is_rec = is_rec;
    return(0);
}



