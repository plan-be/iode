#include "s_a2m.h"

int ChrtInit(void);
int ChrtEnd(int hdl);
int ChrtParams(int hdl, char *txt, double ym, double yM, double xm, double xM);
int ChrtAdd(int hdl, char *title, int axis, int type, int nobs, double *vals);

/*NH*/
int A2mChrtInit(char* outfile)
{
    return(0);
}

/*NH*/
int A2mChrtPrintObj(A2MOBJ *ao)
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
int A2mChrtEnd(int hdl)
{
    return(0);
}

/*NH*/
int A2mChrtPrintGrf(int hdl, A2MGRF* grf)
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






