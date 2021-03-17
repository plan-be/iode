#include "scr.h"

/* =+=== C_INIT_OBJECTS ========================
    Initialize the mpage and isam in the case of
	a compiled application
   ============================================= */

C_init_objects()
{
    int     i,
	    j;
    ISAM    *is;
    MPAGE   *mp;
    PAGE    *pg;
    FIELD   *fld;
    int     lg;

    /* DEFINITION OF THE OBJECTS */
    for(i = 0 ; i < SCR_READ_FILES.tbl_nb_fl ; i++)
	switch(TYPE16(SCR_READ_FILES.tbl_fl[i]->fl_type)) {
	    case SCR_TYPE_ISAM :
		is = (ISAM *)(SCR_READ_FILES.tbl_fl[i]->fl_ptr);
		is->is_rec   = SCR_malloc(is->is_rec_len + 1);
		is->is_rrec  = SCR_malloc(is->is_rec_len + 1);
		is->is_rrrec = SCR_malloc(is->is_rec_len + 1);
		/* BP_M 02-10-97 */
		is->is_maxrec = SCR_malloc(is->is_rec_len + 1);
		/* BP_M 26-10-95 */
		IS_alloc_vfld(is);
		break;

	    case SCR_TYPE_MPAGE :
		mp = (MPAGE *)(SCR_READ_FILES.tbl_fl[i]->fl_ptr);
		is = (ISAM *)SCR_PTR(mp->mp_isam);
		mp->mp_ref_rec =
		    SCR_malloc(is->is_rec_len);
		break;

	    default :
		break;
	    }

    return(0);
}
