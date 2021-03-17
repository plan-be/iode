#include "scr.h"

char *SCR_find_load_obj(nu) /* BP_M 06-04-1993 15:25 */
int     nu;
{
    FILE_TYPE   *flt;
    char        buf[10];
    ACTION      *act;

    flt = SCR_READ_FILES.tbl_fl[nu];
    if(flt->fl_mem == 0) {
	if(TYPE16(flt->fl_type) == SCR_TYPE_ACT) {
	    act = (ACTION *)SCR_malloc(sizeof(ACTION));
	    SCR_read_cmt(flt->fl_pos, (char *)act, sizeof(ACTION));
	    if(act->act_nb > 0) {
		act->act_aa = (A_ACTION *)SCR_malloc(act->act_len);
		SCR_read_cmt(flt->fl_pos + (long)sizeof(ACTION), (char *)act->act_aa, act->act_len);
	    }
	    flt->fl_ptr = (char *)act;
	    flt->fl_mem = 1;
	}
    }
    return(flt->fl_ptr);
}

SCR_unload_obj(nu)      /* BP_M 06-04-1993 15:25 */
int     nu;
{
    FILE_TYPE   *flt;
    ACTION      *act;

    flt = SCR_READ_FILES.tbl_fl[nu];
    if(flt->fl_mem == 1) {
	if(TYPE16(flt->fl_type) == SCR_TYPE_ACT) {
	    act = (ACTION *)flt->fl_ptr;
	    if(act->act_nb > 0)
		SCR_free(act->act_aa);
	    SCR_free(act);
	    flt->fl_mem = 0;
	    flt->fl_ptr = 0;
	}
    }
    return(0);
}

