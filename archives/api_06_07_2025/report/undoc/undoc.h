#pragma once

#include "api/constants.h"
#include "api/report/reports.h"
#include "api/gsample/gsample.h"
#include "api/objs/kdb.h"
#include "api/objs/equations.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"

#ifdef __GNUC__
#    define IODE_CDECL __attribute__((cdecl))
#else
#    define IODE_CDECL __cdecl
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* b_api.c */
extern int IodeInit();
extern int IodeEnd();
extern void kmsg_null(const char* msg);
extern void IodeSuppressMsgs();
extern void IodeResetMsgs();

/* b_dde.c */
extern int IodeDdeLocale(char *buf);
extern int IodeDdeUnLocale(char *buf);
extern int IodeDdeType(char *szTopic);
extern char *IodeDdeGetWS(char *szItem);
extern char *IodeDdeCreateSeries(int objnb, int bt);
extern char *IodeDdeCreatePer(int bt);
extern char *ToBase26(int num);
extern char *IodeDdeXlsCell(char *offset, int i, int j, int lg, int hg);
extern char *IodeTblCell(TCELL *cell, COL *cl, int nbdec);
extern char *IodeDdeCreateTbl(int objnb, char *ismpl, int *nc, int *nl, int nbdec);
extern char *IodeDdeCreateObj(int objnb, int type, int *nc, int *nl);
extern char *IodeDdeGetReportRC(char *szItem);
extern char *IodeDdeGetXObj(char *szItem, int type);
extern char *IodeDdeGetItem(char *szTopic, char *szItem);
extern int IodeDdeSetWS(char *szItem, char *szBuffer);
extern int IodeDdePlay(char *szItem, char *szBuffer);
extern int DdeTsfKey(char *key);
extern int IodeDdeSetItem(char *szTopic, char *szItem, char *szBuffer);
extern char *B_ExcelGetItem(char *arg);
extern int B_ExcelSetItem(char *ddeitem, char *ptr, int nc, int nl);
extern int B_ExcelDecimal(char *arg);
extern int B_ExcelThousand(char *arg);
extern int B_ExcelCurrency(char *arg);
extern int B_ExcelLang(char *arg);
extern int B_ExcelGet(char *arg, int type);
extern int B_ExcelSet(char *arg, int type);
extern int B_ExcelExecute(char *arg);
extern int B_ExcelCmd(char *cmd, char *arg);
extern int B_DDEGet(char *arg);
extern int B_ExcelWrite(char *ptr);
extern int B_DDEGet(char *arg);
extern char *B_ExcelGetItem(char *arg);
extern int B_ExcelGet(char *arg, int type);
extern int B_ExcelSet(char *arg, int type);
extern int B_ExcelExecute(char *arg);
extern int B_ExcelCmd(char *cmd, char *arg);
extern int B_ExcelWrite(char *ptr);
extern int B_ExcelOpen(char *arg);
extern int B_ExcelClose(char *arg);
extern int B_ExcelPrint(char *arg);
extern int B_ExcelSave(char *arg);
extern int B_ExcelSaveAs(char *arg);
extern int B_ExcelNew(char *arg);
extern int IodeFmtVal(char *buf, double val);

/* b_ds.c (MSC) */
extern char *IODE_CDECL B_DSPeriod2Date(struct _period *per,char *date,char *freq);
extern int IODE_CDECL B_DSLog(char *log);
extern int IODE_CDECL B_DSTimedOut(char *name);
extern int IODE_CDECL B_DSDate2Period(struct _period *per,char *date,char freq);
extern int IODE_CDECL B_DSCode(char *name);
extern int IODE_CDECL B_DSUpdateCmt(char *name,char *val);
extern int IODE_CDECL B_DSUpdateVar(char *name,struct _period *per,char *val);
extern int IODE_CDECL B_DSInterpret(char *code,char freq,char *contents);
extern int IODE_CDECL B_DSImportDb_1(char *arg,struct _sample *smpl);
extern int IODE_CDECL B_DSImportDb(char *arg);

/* b_global.c */
extern int B_ScrollSet(char *,long *,int ,int );
extern int B_ScrollVarW(char *);
extern int B_ScrollVarN(char *);
extern int B_ScrollVarM(char *);
extern int B_ScrollVarS(char *);
extern int B_ScrollSclW(char *);
extern int B_ScrollSclN(char *);
extern int B_ScrollTblW(char *);
extern int B_ScrollVTW(char *);
extern int B_ScrollVTW0(char *);
extern int B_ScrollVTN(char *);

/* b_eviews.c */
int B_WsImportEviews(char *arg);

/* b_pdest.c */
extern int B_PrintDestFile(char *arg, int newf);
extern int B_PrintDest(char *file);
extern int B_PrintDestNew(char* file);
extern int B_PrintDestExt(char* file, int newf, int type);
extern int B_PrintNbDec(char* nbdec);
extern int B_PrintLang(char* lang);
extern int B_PrintMulti(char* multi);
extern int B_PrintA2mAppend(char* arg);
extern int B_PrintTBreak(char* arg);
extern int B_PrintTPage(char* arg);
extern int B_PrintGPage(char* arg);
extern int B_PrintParaNum(char* arg);
extern int B_PrintPageHeader(char* arg);
extern int B_PrintPageFooter(char* arg);
extern int B_PrintFont(char* arg);
extern int B_PrintTFont(char* arg);
extern int B_PrintTBox(char* arg);
extern int B_PrintTColor(char* arg);
extern int B_PrintTWidth(char* arg);
extern int B_PrintGSize(char* arg);
extern int B_PrintGTheme(char* arg);
extern int B_PrintGBand(char* arg);
extern int B_PrintGBox(char* arg);
extern int B_PrintGBrush(char* arg);
extern int B_GetColor(char* arg);
extern int B_PrintGColor(char* arg);
extern int B_PrintRtfHelp(char* arg);
extern int B_PrintHtmlHelp(char* arg);
extern int B_PrintRtfTitle(char* arg);
extern int B_PrintRtfCopy(char* arg);
extern int B_PrintRtfLevel(char* arg);
extern int B_PrintRtfTopic(char* arg);
extern int B_PrintGdiOrient(char* arg);
extern int B_PrintGdiDuplex(char* arg);
extern int B_PrintGdiPrinter(char* arg);
extern int B_PrintGIFBackColor(char* arg);
extern int B_PrintGIFTransColor(char* arg);
extern int B_PrintGIFInterlaced(char* arg);
extern int B_PrintGIFTransparent(char* arg);
extern int B_PrintGIFFilled(char* arg);
extern int B_PrintGIFFont(char* arg);
extern int B_PrintHtmlStrip(char* arg);
extern int B_PrintHtmlStyle(char* arg);
extern int B_A2mToAll(char* arg, int type);
extern int B_A2mToPrinter(char* arg);
extern int B_A2mToHtml(char* arg);
extern int B_A2mToRtf(char* arg);
extern int B_A2mToMif(char* arg);
extern int B_A2mToCsv(char* arg);
extern int B_A2mSetCol(int *dest, int col);
extern int B_PrintHtmlTableClass(char *table_class);
extern int B_PrintHtmlTRClass(char *tr_class);
extern int B_PrintHtmlTHClass(char *th_class);
extern int B_PrintHtmlTDClass(char *td_class);

/* b_print.c */
extern int B_TBL_TITLE;  
extern int B_EQS_INFOS;
extern int B_EQS_LEC;
extern int B_PrintVal(double );
extern double B_calc_ttest(SCL *);
extern int B_replesc(unsigned char *,unsigned char *);
extern int B_PrintDefGnl(char *,char *);
extern int B_isdef(char *);
extern int B_dump_str(unsigned char *,unsigned char *);
extern int B_PrintObjTblTitle(char *);
extern int B_PrintObjLec(char *);
extern int B_PrintObjEqsInfos(char *);
extern int B_PrintObjDef_1(char *,int *);
extern int B_PrintObjDef(char *,int );
extern int B_PrintObjDefArgs(char *,int );
extern int B_PrintDefTbl(KDB *,int );
extern int B_DumpTblDef(TBL *);
extern int B_CellDef(TCELL *);
extern int B_PrintTblCell(TCELL *,int );
extern int B_PrintDefCmt(KDB *,int );
extern int B_PrintDefLst(KDB *,int );
extern int B_PrintDefIdt(KDB *,int );
extern int B_PrintDefEqs(KDB *,int );
extern int B_PrintLec(char *,char *,CLEC *,int );
extern int B_PrintEqs(char *,EQ *);
extern int B_PrintDefSclPtr(SCL *,char *,int );
extern int B_PrintDefScl(KDB *,int );
extern int B_PrintDefVar(KDB *,int );

/* b_season.c */
extern int B_WsSeasonAdj(char *);
extern int B_season(char *);
extern int DS_test(double *,int ,int *,int *,int ,double *);
extern int DS_vec(double *,double *,double *,double *,int ,int ,double );
extern int DS_smpl(SAMPLE *,SAMPLE *,SAMPLE **,int *);
extern int DS_extr(double *,int ,int ,double *,double );

/* b_sql.c */
//extern KDB  *K_load_odbc(int , char *, int , char **);
extern U_ch *RPS_Open(U_ch** args);
extern U_ch *RPS_Field(U_ch** args);
extern U_ch *RPS_NbFlds(U_ch** args);
extern U_ch *RPS_Record(U_ch** args);
extern U_ch *RPS_Query(U_ch** args);
extern U_ch *RPS_Sql(U_ch** args);
extern U_ch *RPS_Next(U_ch** args);
extern U_ch *RPS_Close(U_ch** args);

/* b_step.c */
extern int B_EqsStepWise(char* arg);

/* b_trend.c */
extern int B_WsTrend(char *);
extern int B_WsTrendStd(char *);
//extern int HP_smpl(SAMPLE *,SAMPLE *,SAMPLE **,int *);
//extern int HP_calc(double *,double *,int ,int );
//extern int HP_calc(double *,double *,int , double);     // JMP 7-3-2019
//extern int HP_calc(double *,double *,int , double, int);  // JMP 12-4-2019
//extern void HP_test(double *,double *,int ,int *,int *);

/* b_view.c */
extern int B_ViewVar(char *);
extern int B_PrintVar(char *);
extern int B_ViewPrintVar(char *,int );
extern int B_ViewByTbl(char *);
extern int B_ViewTbl(char *);
extern int B_PrintTbl(char *);
extern int B_ViewGr(char *);
extern int B_PrintGr(char *);
extern int B_ViewPrintTbl_1(char *,char *);
extern int B_ViewPrintGr_1(char *,char *);
extern int B_ViewPrintTbl(char *,int ,int );
extern int B_ViewTblFile(char *);
extern int B_ViewTblEnd(void);

#ifdef __cplusplus
}
#endif
