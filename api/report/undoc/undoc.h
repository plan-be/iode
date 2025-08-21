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


/* b_api.c */
int IodeInit();
int IodeEnd();
void kmsg_null(const char* msg);
void IodeSuppressMsgs();
void IodeResetMsgs();

/* b_dde.c */
int IodeDdeLocale(char *buf);
int IodeDdeUnLocale(char *buf);
int IodeDdeType(char *szTopic);
char *IodeDdeGetWS(char *szItem);
char *IodeDdeCreateSeries(int objnb, int bt);
char *IodeDdeCreatePer(int bt);
char *ToBase26(int num);
char *IodeDdeXlsCell(char *offset, int i, int j, int lg, int hg);
char *IodeTblCell(TCELL *cell, COL *cl, int nbdec);
char *IodeDdeCreateTbl(int objnb, char *ismpl, int *nc, int *nl, int nbdec);
char *IodeDdeCreateObj(int objnb, int type, int *nc, int *nl);
char *IodeDdeGetReportRC(char *szItem);
char *IodeDdeGetXObj(char *szItem, int type);
char *IodeDdeGetItem(char *szTopic, char *szItem);
int IodeDdeSetWS(char *szItem, char *szBuffer);
int IodeDdePlay(char *szItem, char *szBuffer);
int DdeTsfKey(char *key);
int IodeDdeSetItem(char *szTopic, char *szItem, char *szBuffer);
char* B_ExcelGetItem(char *arg);
int B_ExcelSetItem(char *ddeitem, char *ptr, int nc, int nl);
int B_ExcelDecimal(char *arg, int unused=-1);
int B_ExcelThousand(char *arg, int unused=-1);
int B_ExcelCurrency(char *arg, int unused=-1);
int B_ExcelLang(char *arg, int unused=-1);
int B_ExcelGet(char *arg, int type);
int B_ExcelSet(char *arg, int type);
int B_ExcelExecute(char *arg, int unused=-1);
int B_ExcelCmd(char *cmd, char *arg);
int B_DDEGet(char *arg, int unused=-1);
int B_ExcelWrite(char *ptr, int unused=-1);
char* B_ExcelGetItem(char *arg);
int B_ExcelGet(char *arg, int type);
int B_ExcelSet(char *arg, int type);
int B_ExcelCmd(char *cmd, char *arg);
int B_ExcelOpen(char *arg, int unused=-1);
int B_ExcelClose(char *arg, int unused=-1);
int B_ExcelPrint(char *arg, int unused=-1);
int B_ExcelSave(char *arg, int unused=-1);
int B_ExcelSaveAs(char *arg, int unused=-1);
int B_ExcelNew(char *arg, int unused=-1);
int IodeFmtVal(char *buf, double val);

/* b_ds.c (MSC) */
char *IODE_CDECL B_DSPeriod2Date(struct _period *per,char *date,char *freq);
int IODE_CDECL B_DSLog(char *log);
int IODE_CDECL B_DSTimedOut(char *name);
int IODE_CDECL B_DSDate2Period(struct _period *per,char *date,char freq);
int IODE_CDECL B_DSCode(char *name);
int IODE_CDECL B_DSUpdateCmt(char *name,char *val);
int IODE_CDECL B_DSUpdateVar(char *name,struct _period *per,char *val);
int IODE_CDECL B_DSInterpret(char *code,char freq,char *contents);
int IODE_CDECL B_DSImportDb_1(char *arg,struct _sample *smpl);
int IODE_CDECL B_DSImportDb(char *arg, int unused=-1);

/* b_global.c */
int B_ScrollSet(char *,long *,int ,int );
int B_ScrollVarW(char* arg, int unused=-1);
int B_ScrollVarN(char* arg, int unused=-1);
int B_ScrollVarM(char* arg, int unused=-1);
int B_ScrollVarS(char* arg, int unused=-1);
int B_ScrollSclW(char* arg, int unused=-1);
int B_ScrollSclN(char* arg, int unused=-1);
int B_ScrollTblW(char* arg, int unused=-1);
int B_ScrollVTW(char* arg, int unused=-1);
int B_ScrollVTW0(char* arg, int unused=-1);
int B_ScrollVTN(char* arg, int unused=-1);

/* b_eviews.c */
int B_WsImportEviews(char* arg, int unused=-1);

/* b_pdest.c */
int B_PrintDestFile(char *arg, int newf);
int B_PrintDest(char* file, int unused=-1);
int B_PrintDestNew(char* file, int unused=-1);
int B_PrintDestExt(char* file, int newf, int type);
int B_PrintNbDec(char* nbdec, int unused=-1);
int B_PrintLang(char* lang, int unused=-1);
int B_PrintMulti(char* multi, int unused=-1);
int B_PrintA2mAppend(char* arg, int unused=-1);
int B_PrintTBreak(char* arg, int unused=-1);
int B_PrintTPage(char* arg, int unused=-1);
int B_PrintGPage(char* arg, int unused=-1);
int B_PrintParaNum(char* arg, int unused=-1);
int B_PrintPageHeader(char* arg, int unused=-1);
int B_PrintPageFooter(char* arg, int unused=-1);
int B_PrintFont(char* arg, int unused=-1);
int B_PrintTFont(char* arg, int unused=-1);
int B_PrintTBox(char* arg, int unused=-1);
int B_PrintTColor(char* arg, int unused=-1);
int B_PrintTWidth(char* arg, int unused=-1);
int B_PrintGSize(char* arg, int unused=-1);
int B_PrintGTheme(char* arg, int unused=-1);
int B_PrintGBand(char* arg, int unused=-1);
int B_PrintGBox(char* arg, int unused=-1);
int B_PrintGBrush(char* arg, int unused=-1);
int B_GetColor(char* arg, int unused=-1);
int B_PrintGColor(char* arg, int unused=-1);
int B_PrintRtfHelp(char* arg, int unused=-1);
int B_PrintHtmlHelp(char* arg, int unused=-1);
int B_PrintRtfTitle(char* arg, int unused=-1);
int B_PrintRtfCopy(char* arg, int unused=-1);
int B_PrintRtfLevel(char* arg, int unused=-1);
int B_PrintRtfTopic(char* arg, int unused=-1);
int B_PrintGdiOrient(char* arg, int unused=-1);
int B_PrintGdiDuplex(char* arg, int unused=-1);
int B_PrintGdiPrinter(char* arg, int unused=-1);
int B_PrintGIFBackColor(char* arg, int unused=-1);
int B_PrintGIFTransColor(char* arg, int unused=-1);
int B_PrintGIFInterlaced(char* arg, int unused=-1);
int B_PrintGIFTransparent(char* arg, int unused=-1);
int B_PrintGIFFilled(char* arg, int unused=-1);
int B_PrintGIFFont(char* arg, int unused=-1);
int B_PrintHtmlStrip(char* arg, int unused=-1);
int B_PrintHtmlStyle(char* arg, int unused=-1);
int B_A2mToAll(char* arg, int type);
int B_A2mToPrinter(char* arg, int unused=-1);
int B_A2mToHtml(char* arg, int unused=-1);
int B_A2mToRtf(char* arg, int unused=-1);
int B_A2mToMif(char* arg, int unused=-1);
int B_A2mToCsv(char* arg, int unused=-1);
int B_A2mSetCol(int *dest, int col);
int B_PrintHtmlTableClass(char *table_class, int unused=-1);
int B_PrintHtmlTRClass(char *tr_class, int unused=-1);
int B_PrintHtmlTHClass(char *th_class, int unused=-1);
int B_PrintHtmlTDClass(char *td_class, int unused=-1);

/* b_print.c */
inline int B_TBL_TITLE;    // Specify how to print a TABLE 
                           //      0 : print table full definitions
                           //      1 : print only table titles
inline int B_EQS_INFOS;    // Information detail to print (for equations)
                           //    0: equation only 
                           //    1: equation + comment
                           //    2: equation + comment + estimation results
inline int B_EQS_LEC;      // Specify how to print a LEC expression 
                           //    0 : print the LEC form as is
                           //    1 : replace all scalars by their values
                           //    2 : replaced all scalars by their values + t-tests

int B_PrintVal(double );
double B_calc_ttest(Scalar *);
int B_replesc(unsigned char *,unsigned char *);
int B_PrintDefGnl(char *,char *);
int B_isdef(char* arg);
int B_dump_str(unsigned char *,unsigned char *);
int B_PrintObjTblTitle(char* arg, int unused=-1);
int B_PrintObjLec(char* arg, int unused=-1);
int B_PrintObjEqsInfos(char* arg, int unused=-1);
int B_PrintObjDef_1(char *,int *);
int B_PrintObjDef(char* arg, int unused=-1);
int B_PrintObjDefArgs(char* arg, int unused=-1);
int B_PrintDefTbl(KDB *,int );
int B_DumpTblDef(TBL *);
int B_CellDef(TCELL *);
int B_PrintTblCell(TCELL *,int );
int B_PrintDefCmt(KDB *,int );
int B_PrintDefLst(KDB *,int );
int B_PrintDefIdt(KDB *,int );
int B_PrintDefEqs(KDB *,int );
int B_PrintLec(char *,char *,CLEC *,int );
int B_PrintEqs(char *,EQ *);
int B_PrintDefSclPtr(Scalar *,char *,int );
int B_PrintDefScl(KDB *,int );
int B_PrintDefVar(KDB *,int );

/* b_season.c */
int B_WsSeasonAdj(char* arg, int unused=-1);
int B_season(char* arg);
int DS_test(double *,int ,int *,int *,int ,double *);
int DS_vec(double *,double *,double *,double *,int ,int ,double );
int DS_smpl(SAMPLE *,SAMPLE *,SAMPLE **,int *);
int DS_extr(double *,int ,int ,double *,double );

/* b_sql.c */
//KDB  *K_load_odbc(int , char *, int , char **);
U_ch *RPS_Open(U_ch** args);
U_ch *RPS_Field(U_ch** args);
U_ch *RPS_NbFlds(U_ch** args);
U_ch *RPS_Record(U_ch** args);
U_ch *RPS_Query(U_ch** args);
U_ch *RPS_Sql(U_ch** args);
U_ch *RPS_Next(U_ch** args);
U_ch *RPS_Close(U_ch** args);

/* b_step.c */
int B_EqsStepWise(char* arg, int unused=-1);

/* b_trend.c */
int B_WsTrend(char* arg, int unused=-1);
int B_WsTrendStd(char* arg, int unused=-1);
//int HP_smpl(SAMPLE *,SAMPLE *,SAMPLE **,int *);
//int HP_calc(double *,double *,int ,int );
//int HP_calc(double *,double *,int , double);     // JMP 7-3-2019
//int HP_calc(double *,double *,int , double, int);  // JMP 12-4-2019
//void HP_test(double *,double *,int ,int *,int *);

/* b_view.c */
int B_ViewVar(char* arg, int unused=-1);
int B_PrintVar(char* arg, int unused=-1);
int B_ViewPrintVar(char* arg, int unused=-1);
int B_ViewByTbl(char* arg, int unused=-1);
int B_ViewTbl(char* arg, int unused=-1);
int B_PrintTbl(char* arg, int unused=-1);
int B_ViewGr(char* arg, int unused=-1);
int B_PrintGr(char* arg, int unused=-1);
int B_ViewPrintTbl_1(char*, char*);
int B_ViewPrintGr_1(char*, char*);
int B_ViewPrintTbl(char* arg, int, int);
int B_ViewTblFile(char* arg, int unused=-1);
int B_ViewTblEnd(void);
