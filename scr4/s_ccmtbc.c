#include "scr.h"

BC_calc_cmt(bc)
BC      *bc;
{
    SCR_get_comment(bc->bc_cmt_pos, 0, bc->bc_cmt);
    BC_construct_tbl_cmt(bc);
    SCR_scmt_set_txt();
    return(0);
}

BC_construct_tbl_cmt(bc)
BC      *bc;
{
    SCR_scmt_free();
    SCR_scmt_add_fnk(bc->bc_fnks);
    SCR_scmt_add_key(SCR_F1, SCR_err_txt(517));
    SCR_scmt_add_key(SCR_F2, SCR_err_txt(512));
    SCR_scmt_add_key(SCR_ENTER, SCR_err_txt((bc->bc_modify) ? 505 : 506));
    if(bc->bc_create) SCR_scmt_add_key(SCR_INSERT, SCR_err_txt(507));
    if(bc->bc_delete) SCR_scmt_add_key(SCR_DELETE, SCR_err_txt(508));
    SCR_scmt_add_key(SCR_ESCAPE, SCR_err_txt(509));

    if(bc->bc_no_mark) return(0); /* JMP_M 4.20 01-09-95 */
    SCR_scmt_add_key(SCR_C_L, SCR_err_txt(513));
    SCR_scmt_add_key(SCR_C_U, SCR_err_txt(514));
    SCR_scmt_add_key(SCR_C_C, SCR_err_txt(515));
    SCR_scmt_add_key(SCR_C_D, SCR_err_txt(516));
    return(0);
}

