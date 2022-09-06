#pragma once

/* ********************************************** *
 * copied/pasted from the /cmd/sb_gui.c directory *
 * ********************************************** */

#include "api/iode.h"
#include <s_a2m.h>

 /* sb_pdest.c */
double   A2M_GWIDTH = 9.5;
double   A2M_GHEIGHT = 6.5;
int      A2M_BOXWIDTH = 1;
int      A2M_BACKBRUSH = 50;
int      A2M_BACKCOLOR = 'b';

void B_A2mGetGnlParms() {}

void B_A2mGetRtfParms() { B_A2mGetGnlParms(); }

void B_A2mGetCsvParms() { B_A2mGetGnlParms(); }

void B_A2mGetHtmlParms() { B_A2mGetGnlParms(); }

void B_A2mGetGdiParms() { B_A2mGetGnlParms(); }

void B_A2mGetMifParms() { B_A2mGetGnlParms(); }

void B_A2mGetGIFParms() { }
