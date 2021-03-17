#include "scr.h"

/* =+=== BC_FREE_LBCS ==============================================
    Free the BCLIST
	bc : IO : pointer ot the BC
   ================================================================== */

BC_free_lbcs(bc)
BC   *bc;
{
    BCL_free(bc->bc_list);
    bc->bc_list = 0;
    bc->bc_c_line = 0;
    bc->bc_line0 = 0;
    bc->bc_nb_lbc = 0;
    IS_empty_rec(ISAM_PTR(bc->bc_is_lbc));
    return(0);
}

/* =+=== BCL_free ============================================
    Free a BCLIST bcl_rec and bcl_info
     bc : IO : BC *
     return : pinter to the allocated BCLIST
   ============================================================ */

BCL_free(bcl)
BCLIST  *bcl;
{
    BCLIST  *next;

    if(bcl == 0) return(0);
    while(bcl != 0) {
	next = bcl->bcl_next;
	SCR_free(bcl->bcl_rec);
	SCR_free(bcl->bcl_pgrec);
	SCR_free(bcl);
	bcl = next;
    }
    return(0);
}

/* =+=== BCL_LOCATE ============================================
    Locate the nth BCLIST in a BCLIST
     bcl : IN : pointer to the first
     n   : IN : Position of the BCLIST to be found
     return : pointer to the nth BCLIST
   ============================================================= */

BCLIST *BCL_locate(bcl, n)
BCLIST  *bcl;
int     n;
{
    int     i = 0;

    while(i < n && bcl->bcl_next != 0) {
	bcl = bcl->bcl_next;
	i++;
	}
    return(bcl);
}
/**/
/* =+=== BC_ADD_BCL ============================================
    ADD A BCLIST AFTER THE NTH
     BC : IO : BC *
     N  : IN : POSITION AFTER WHICH THE BCLIST IS TO BE ADDED
     RETURN : POINTER TO THE ALLOCATED BCLIST
   ============================================================= */

BCLIST *BC_add_bcl(bc, n)
BC      *bc;
int     n;
{
    BCLIST  *bcl;

    bcl = (BCLIST *) SCR_malloc(sizeof(BCLIST));
    bc->bc_nb_lbc++;
    bcl->bcl_rec = SCR_malloc(((ISAM *)SCR_PTR(bc->bc_is_lbc))->is_rec_len);
    if(bc->bc_lpg_mem)
	bcl->bcl_pgrec = SCR_malloc(BC_pgrec_len(bc));

    /* IF N < 0 SET BCL AS BC_LIST */
    if(n < 0) {
	bcl->bcl_next = bc->bc_list;
	bcl->bcl_prev = 0;
	bc->bc_list = bcl;
	return(bcl);
	}

    /* IF NO BCL DEFINED, SET BCL IN BC_LIST */
    if(bc->bc_list == 0) {
	bc->bc_list = bcl;
	bcl->bcl_next = 0;
	bcl->bcl_prev = 0;
	return(bcl);
	}

    /* LOCATE Nth BCL AND SET BCL IN LIST */
    bcl->bcl_prev = BCL_locate(bc->bc_list, n);
    bcl->bcl_next = bcl->bcl_prev->bcl_next;
    bcl->bcl_prev->bcl_next = bcl;
    if(bcl->bcl_next != 0) bcl->bcl_next->bcl_prev = bcl;
    return(bcl);
}
/**/
/* =+=== BC_DELETE_BCL ============================================
    Delete the nth BCLIST
     bc : IO : BC *
     n  : IN : Position of the BCLIST to be removed
   ============================================================= */

BC_delete_bcl(bc, n)
BC      *bc;
int     n;
{
    BCLIST  *bcl;

    bcl = BCL_locate(bc->bc_list, n);
    if(bcl == 0) return(0);
    SCR_free(bcl->bcl_rec);
    SCR_free(bcl->bcl_pgrec);
    if(bcl->bcl_prev == 0)
	bc->bc_list = bcl->bcl_next;
    else
	bcl->bcl_prev->bcl_next = bcl->bcl_next;
    if(bcl->bcl_next != 0) bcl->bcl_next->bcl_prev = bcl->bcl_prev;
    SCR_free(bcl);
    bc->bc_nb_lbc--;
    return(0);
}



