#include <scr.h>

/* OPEN BC AND READ MENU */

BC_start1(bc)
BC  *bc;
{
    /* PREPARE BC */
    if(0 != BC_open(bc)) {
	PG_display_error(IS_error_text());
	return(-1);
	}
    BC_empty(bc);

    /* PREPARE HELP*/
    HLP_stack_add(bc->bc_help); /* JMP_M 4.19 11-06-95 */

    return(0);
}

BC_edit_end(bc)
BC  *bc;
{
    BC_close(bc) ;
    HLP_stack_del(1);
    return(0);
}

