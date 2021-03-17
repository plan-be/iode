#include "scr.h"

MP_calc_cmt(mp)
MPAGE   *mp;
{
    SCR_get_comment(mp->mp_cmt_pos, 0, mp->mp_cmt);
    MP_construct_tbl_cmt(mp);
    SCR_scmt_set_txt();
    return(0);
}

/*NH*/
MP_construct_tbl_cmt(mp)
MPAGE   *mp;
{
    SCR_scmt_free();
    SCR_scmt_add_fnk(mp->mp_fnks);
    SCR_scmt_add_key(SCR_ENTER, ((mp->mp_modify) ?  SCR_err_txt(505) : SCR_err_txt(506))); /* JMP_M 4.19 11-05-95 */
    if(mp->mp_create) SCR_scmt_add_key(SCR_INSERT, SCR_err_txt(507));   /* JMP_M 4.19 11-05-95 */
    if(mp->mp_delete) SCR_scmt_add_key(SCR_DELETE, SCR_err_txt(508));   /* JMP_M 4.19 11-05-95 */
    SCR_scmt_add_key(SCR_ESCAPE, SCR_err_txt(509));                     /* JMP_M 4.19 11-05-95 */
    return(0);
}

