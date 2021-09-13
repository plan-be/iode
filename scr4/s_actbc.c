#include "scr.h"

extern long     ACT_find_lg();
extern char     *ACT_find_str();
extern double   ACT_find_db();

/*NH*/
ACT_exec_aa_bc(aa)
A_ACTION    *aa;
{
    BC      *bc = (BC *)SCR_PTR(ACT_find_lg(aa, 0));

    switch(aa->aa_fn) {
	case ACT_BCAB_EDIT_ISAM:BCAB_edit_isam(bc); break;
	case ACT_BC_EDIT_ISAM : BC_edit_isam(bc); break;

	case ACT_BC_RESET  : BC_act_reset(bc); break;
	case ACT_BC_DISPLAY: BC_display(bc); break;
	case ACT_BC_UNDISPLAY : BC_undisplay(bc); break;
	case ACT_BC_CREATE : BC_act_create(bc); break;
	case ACT_BC_OPEN   : return(BC_open(bc));
	case ACT_BC_CLOSE  : return(BC_close(bc));
	case ACT_BC_SEARCH : return(BC_search_bc_lbc(bc));
	case ACT_BC_NEXT   : return(BC_next_bc(bc));
	case ACT_BC_PREV   : return(BC_prev_bc(bc));
	case ACT_BC_MODIFY : BC_modify_bc(bc); break;
	case ACT_BC_DELETE : return(BC_delete_bc(bc));
	case ACT_BC_DISPLAY_FLDS: BC_display_flds(bc); break;

	case SCR_IF_BC_SELECT : if(bc->bc_operation == 2) return(0);
	    return(-1);

	default:
	    break;
    }
    return(0);
}

BC_act_create(bc)
BC  *bc;
{
    bc->bc_operation = 1;
    BC_edis_create(bc);
    bc->bc_operation = 0;
    return(0);
}

BC_act_reset(bc)
BC  *bc;
{
    BC_empty(bc);
#ifndef SCRW32
    BC_display(bc);
#endif
    return(0);
}
