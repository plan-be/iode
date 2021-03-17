#include "s_a2m.h"
/*
A2mToChrt(char *a2mfile)
{
    A2MFILE *af;
    A2MOBJ  *ao;
    int     hdl;

    af = A2mOpen(a2mfile, 0);
    if(af == 0) return(-1);

    hdl = A2mChrtInit();
    if(hdl < 0) {
	A2mClose(af);
	return(-2);
	}

    while(1) {
	ao = A2mRead(af);
	if(ao == 0) break;
	A2mChrtPrintObj(hdl, ao);
	}

    A2mChrtEnd(hdl);
    A2mClose(af);
    return(0);
}
*/
/*NH*/
int A2mChrtInit()
{
    return(0);
}

/*NH*/
A2mChrtPrintObj(ao)
A2MOBJ  *ao;
{
    int     hdl;

    if(ao->ao_type == A2M_GRF) {
	hdl = ChrtInit();
	A2mChrtPrintGrf(hdl, (A2MGRF *)ao->ao_ptr);
	ChrtEnd(hdl);
	}

    A2mFreeObj(ao);
    return(0);
}

/*NH*/
int A2mChrtEnd(hdl)
int hdl;
{
    return(0);
}

/*NH*/
int A2mChrtPrintGrf(hdl, grf)
int     hdl;
A2MGRF  *grf;
{
    int     i;
    char    *title = 0;

    if(grf->ag_title) title = grf->ag_title->ap_strs[0]->as_txt;
    ChrtParams(hdl, title,
		    grf->ag_mins[1], grf->ag_maxs[1],
		    grf->ag_mins[2], grf->ag_maxs[2]);
    if(grf->ag_gxy == NULL) return(0);
    for(i = 0; grf->ag_gxy[i]; i++) { /* JMP 25-02-03 */
	title = 0;
	if(grf->ag_gxy[i]->xy_title)
	    title = grf->ag_gxy[i]->xy_title->ap_strs[0]->as_txt;
	ChrtAdd(hdl, title,
		     grf->ag_gxy[i]->xy_axis, grf->ag_gxy[i]->xy_type,
		     grf->ag_gxy[i]->xy_nobs, grf->ag_gxy[i]->xy_vals);
    }
    return(0);
}






