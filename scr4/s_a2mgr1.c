#include "s_a2m.h"
#include <s_ini.h>

extern FILE    *A2M_fdrtf;

/*NH*/
A2mRtfPrintGrf(ao)
A2MOBJ  *ao;
{
#if defined(SCRW32) || defined (DOSW32)
#ifndef NOEMF // Suppression des EMF/WMF pour iodew
    A2MGRF  *ag = (A2MGRF *)ao->ao_ptr;
    int     w, h;

    w = ag->ag_size[0] * 10;
    h = ag->ag_size[1] * 10;
    if(A2mGdiWMFInit("_scr4.wmf", w, h)) return(-1);

//    w = (ag->ag_size[0] * 1440.0) / 2.54;
//    h = (ag->ag_size[1] * 1440.0) / 2.54;
    w = ag->ag_size[0] * 1000.0;
    h = ag->ag_size[1] * 1000.0;

    A2mGdiPrintObj(ao); /* !!!! Free ao !!! */
    A2mGdiPrintObj((A2MOBJ *)0);
    A2mGdiWMFEnd();
    fprintf(A2M_fdrtf, "\n{{\\pict\\wmetafile8\\picw%d\\pich%d\n", w, h);
    SCR_dumphexfd("_scr4.wmf", A2M_fdrtf, 1);
    fprintf(A2M_fdrtf, "}\n\\par}\n");
    unlink("_scr4.wmf");
#endif    
#endif
    return(0);
}

