#pragma once

#include "scr4/s_a2m.h"
#include "scr4/s_strs.h"

#include "api/constants.h"

#ifdef __cplusplus
extern "C" {
#endif

#define W_GDI   A2M_DESTGDIPRT
#define W_A2M   A2M_DESTA2M
#define W_MIF   A2M_DESTFRM
#define W_HTML  A2M_DESTHTML
#define W_RTF   A2M_DESTRTF
#define W_CSV   A2M_DESTCSV
#define W_DUMMY A2M_DESTDUMMY
//#define W_DISP  A2M_DESTGDIEMF
//#define W_DISP  A2M_DESTGDIWMF
#define W_DISP  A2M_DESTTCHRT

extern char     W_filename[];
extern FILE     *W_fd;
extern int      W_gdiask;
extern int      W_a2mapp;
extern int      W_rtfhelp;
extern int      W_htmlhelp;
extern int      W_gdiorient;
extern int      W_gdiduplex;
extern char     W_gdiprinter[80];

extern void B_A2mGetGnlParms();
extern void B_A2mSaveGnlParms();
extern void B_A2mSetRtfTitle(U_ch* title);
extern void B_A2mSetRtfCopy(U_ch* title);
extern void B_A2mGetRtfParms();
extern void B_A2mSaveRtfParms();
extern void B_A2mGetHtmlParms();
extern void B_A2mSaveHtmlParms();
extern void B_A2mGetGIFParms();
extern void B_A2mSaveGIFParms();
extern void B_A2mGetCsvParms();
extern void B_A2mSaveCsvParms();
extern void B_A2mGetMifParms();
extern void B_A2mSaveMifParms();
extern void B_A2mGetGdiParms();
extern void B_A2mSaveGdiParms();
extern void B_A2mGetAllParms();
extern void B_A2mSaveAllParms();
//extern void K_load_iode_ini();
//extern void K_save_iode_ini();

#ifdef __cplusplus
}
#endif
