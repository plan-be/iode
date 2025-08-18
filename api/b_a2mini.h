#pragma once

#include "scr4/s_a2m.h"

#include "api/constants.h"
#include "api/write/write.h"

#define W_GDI       A2M_DESTGDIPRT
#define W_A2M       A2M_DESTA2M
#define W_MIF       A2M_DESTFRM
#define W_HTML      A2M_DESTHTML
#define W_RTF       A2M_DESTRTF
#define W_CSV       A2M_DESTCSV
#define W_DUMMY     A2M_DESTDUMMY
//#define W_DISP  A2M_DESTGDIEMF
//#define W_DISP  A2M_DESTGDIWMF
#define W_DISP      A2M_DESTTCHRT

void B_A2mGetGnlParms();
void B_A2mSaveGnlParms();
void B_A2mSetRtfTitle(U_ch* title);
void B_A2mSetRtfCopy(U_ch* title);
void B_A2mGetRtfParms();
void B_A2mSaveRtfParms();
void B_A2mGetHtmlParms();
void B_A2mSaveHtmlParms();
void B_A2mGetGIFParms();
void B_A2mSaveGIFParms();
void B_A2mGetCsvParms();
void B_A2mSaveCsvParms();
void B_A2mGetMifParms();
void B_A2mSaveMifParms();
void B_A2mGetGdiParms();
void B_A2mSaveGdiParms();
void B_A2mGetAllParms();
void B_A2mSaveAllParms();
//void K_load_iode_ini();
//void K_save_iode_ini();
