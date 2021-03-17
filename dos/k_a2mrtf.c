#include "s_a2m.h"
#include <s_ini.h>

extern FILE   *A2M_fdrtf;

extern int    A2M_RTF_OEMTOANSI;

/*NH*/
A2mRtfPrintGrf(ao)
A2MOBJ  *ao;
{
//#if defined(SCRW32) || defined (DOSW32)
	A2MGRF  *ag = (A2MGRF *)ao->ao_ptr;
	int     hdl, w, h, wgoal, hgoal, old_oem = A2M_RTF_OEMTOANSI,
		parnb;

	w = ag->ag_size[0] * 10;
	h = ag->ag_size[1] * 10;

	wgoal = (ag->ag_size[0] * 1440.0) / 2.54;
	hgoal = (ag->ag_size[1] * 1440.0) / 2.54;
	w = ag->ag_size[0] * 1000.0;
	h = ag->ag_size[1] * 1000.0;

	hdl = ChrtInitEmf(wgoal, hgoal);
	A2mChrtPrintGrf(hdl, (A2MGRF *)ao->ao_ptr);
	if(ag->ag_title) {
	A2M_RTF_OEMTOANSI = 1;
	A2mRtfPrintPar((A2MPAR *) ag->ag_title, 0);
	A2M_RTF_OEMTOANSI = old_oem;
	}
	ChrtEndEmf(hdl);

	parnb = 1 + A2mFindPpr("par_0");
	if(parnb <= 0) parnb = 1;
	fprintf(A2M_fdrtf, "{\\pard\\plain\\s%d ", parnb);

//    fprintf(A2M_fdrtf, "\n{{\\pict\\emfblip\\picw%d\\pich%d\\picwgoal%d\\pichgoal%d\n", w, h, wgoal, hgoal);
	fprintf(A2M_fdrtf, "{\\pict\\emfblip\\picw%d\\pich%d\\picwgoal%d\\pichgoal%d\n", w, h, wgoal, hgoal);
	SCR_dumphexfd("_scr4.emf", A2M_fdrtf, 1);
	fprintf(A2M_fdrtf, "}\n\\par}\n");
	unlink("_scr4.emf");
//#endif
	return(0);
}


