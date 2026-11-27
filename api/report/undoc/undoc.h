#pragma once
#include "api/pch.h"
#include "api/report/reports.h"
#include "api/gsample/gsample.h"
#include "api/objs/kdb.h"
#include "api/objs/comments.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"
#include "api/objs/variables.h"

#ifdef __GNUC__
#    define IODE_CDECL __attribute__((cdecl))
#else
#    define IODE_CDECL __cdecl
#endif


/* b_api.c */
int IodeInit();
int IodeEnd();
void kmsg_null(const char* msg);

/* b_dde.c */
int IodeDdeLocale(char *buf);
int IodeDdeUnLocale(char *buf);
int IodeDdeType(char *szTopic);
char *IodeDdeGetWS(char *szItem);
char *IodeDdeCreateSeries(const std::string& var_name, int bt);
char *IodeDdeCreatePer(int bt);
char *ToBase26(int num);
char *IodeDdeXlsCell(char *offset, int i, int j, int lg, int hg);
char *IodeTblCell(TableCell *cell, COL *cl, int nbdec);
char *IodeDdeGetReportRC(char *szItem);
char *IodeDdeGetXObj(char *szItem, int type);
char *IodeDdeGetItem(char *szTopic, char *szItem);
int IodeDdeSetWS(char *szItem, char *szBuffer);
int IodeDdePlay(char *szItem, char *szBuffer);
int DdeTsfKey(char *key);
int IodeDdeSetItem(char *szTopic, char *szItem, char *szBuffer);
char* B_ExcelGetItem(char *arg);
int B_ExcelSetItem(char *ddeitem, char *ptr, int nc, int nl);
int B_ExcelDecimal(const std::string& arg, int unused=-1);
int B_ExcelThousand(const std::string& arg, int unused=-1);
int B_ExcelCurrency(const std::string& arg, int unused=-1);
int B_ExcelLang(const std::string& arg, int unused=-1);
int B_ExcelGet(const std::string& arg, int type);
int B_ExcelSet(const std::string& arg, int type);
int B_ExcelExecute(const std::string& arg, int unused=-1);
int B_ExcelCmd(const std::string& cmd, const std::string& arg);
int B_DDEGet(const std::string& arg, int unused=-1);
int B_ExcelWrite(const std::string& arg, int unused=-1);
int B_ExcelOpen(const std::string& arg, int unused=-1);
int B_ExcelClose(const std::string& arg, int unused=-1);
int B_ExcelPrint(const std::string& arg, int unused=-1);
int B_ExcelSave(const std::string& arg, int unused=-1);
int B_ExcelSaveAs(const std::string& arg, int unused=-1);
int B_ExcelNew(const std::string& arg, int unused=-1);
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
int IODE_CDECL B_DSImportDb(const std::string& arg, int unused=-1);

/* b_global.c */
int B_ScrollSet(const std::string& arg, long *plong, int inf, int sup);
int B_ScrollVarW(const std::string& arg, int unused=-1);
int B_ScrollVarN(const std::string& arg, int unused=-1);
int B_ScrollVarM(const std::string& arg, int unused=-1);
int B_ScrollVarS(const std::string& arg, int unused=-1);
int B_ScrollSclW(const std::string& arg, int unused=-1);
int B_ScrollSclN(const std::string& arg, int unused=-1);
int B_ScrollTblW(const std::string& arg, int unused=-1);
int B_ScrollVTW(const std::string& arg, int unused=-1);
int B_ScrollVTW0(const std::string& arg, int unused=-1);
int B_ScrollVTN(const std::string& arg, int unused=-1);

/* b_eviews.c */
int B_WsImportEviews(const std::string& arg, int unused=-1);

/* b_pdest.c */
int B_PrintDestFile(const std::string& arg);
int B_PrintDest(const std::string& file, int unused=-1);
int B_PrintDestNew(const std::string& file, int unused=-1);
int B_PrintDestExt(char* file, int type);
int B_PrintNbDec(const std::string& nbdec, int unused=-1);
int B_PrintLang(const std::string& lang, int unused=-1);
int B_PrintMulti(const std::string& multi, int unused=-1);
int B_PrintA2mAppend(const std::string& arg, int unused=-1);
int B_PrintTBreak(const std::string& arg, int unused=-1);
int B_PrintTPage(const std::string& arg, int unused=-1);
int B_PrintGPage(const std::string& arg, int unused=-1);
int B_PrintParaNum(const std::string& arg, int unused=-1);
int B_PrintPageHeader(const std::string& arg, int unused=-1);
int B_PrintPageFooter(const std::string& arg, int unused=-1);
int B_PrintFont(const std::string& arg, int unused=-1);
int B_PrintTFont(const std::string& arg, int unused=-1);
int B_PrintTBox(const std::string& arg, int unused=-1);
int B_PrintTColor(const std::string& arg, int unused=-1);
int B_PrintTWidth(const std::string& arg, int unused=-1);
int B_PrintGSize(const std::string& arg, int unused=-1);
int B_PrintGTheme(const std::string& arg, int unused=-1);
int B_PrintGBand(const std::string& arg, int unused=-1);
int B_PrintGBox(const std::string& arg, int unused=-1);
int B_PrintGBrush(const std::string& arg, int unused=-1);
int B_GetColor(const std::string& arg, int unused=-1);
int B_PrintGColor(const std::string& arg, int unused=-1);
int B_PrintRtfHelp(const std::string& arg, int unused=-1);
int B_PrintHtmlHelp(const std::string& arg, int unused=-1);
int B_PrintRtfTitle(const std::string& arg, int unused=-1);
int B_PrintRtfCopy(const std::string& arg, int unused=-1);
int B_PrintRtfLevel(const std::string& arg, int unused=-1);
int B_PrintRtfTopic(const std::string& arg, int unused=-1);
int B_PrintGdiOrient(const std::string& arg, int unused=-1);
int B_PrintGdiDuplex(const std::string& arg, int unused=-1);
int B_PrintGdiPrinter(const std::string& arg, int unused=-1);
int B_PrintGIFBackColor(const std::string& arg, int unused=-1);
int B_PrintGIFTransColor(const std::string& arg, int unused=-1);
int B_PrintGIFInterlaced(const std::string& arg, int unused=-1);
int B_PrintGIFTransparent(const std::string& arg, int unused=-1);
int B_PrintGIFFilled(const std::string& arg, int unused=-1);
int B_PrintGIFFont(const std::string& arg, int unused=-1);
int B_PrintHtmlStrip(const std::string& arg, int unused=-1);
int B_PrintHtmlStyle(const std::string& arg, int unused=-1);
int B_A2mToAll(const std::string& arg, int type);
int B_A2mToPrinter(const std::string& arg, int unused=-1);
int B_A2mToHtml(const std::string& arg, int unused=-1);
int B_A2mToRtf(const std::string& arg, int unused=-1);
int B_A2mToMif(const std::string& arg, int unused=-1);
int B_A2mToCsv(const std::string& arg, int unused=-1);
int B_A2mSetCol(int *dest, int col);
int B_PrintHtmlTableClass(const std::string& table_class, int unused=-1);
int B_PrintHtmlTRClass(const std::string& tr_class, int unused=-1);
int B_PrintHtmlTHClass(const std::string& th_class, int unused=-1);
int B_PrintHtmlTDClass(const std::string& td_class, int unused=-1);

/* b_print.c */
int B_PrintVal(double val);
int B_replesc(unsigned char* out, unsigned char* in);
int B_isdef(char* arg);
int B_PrintObjTblTitle(const std::string& arg, int unused=-1);
int B_PrintObjLec(const std::string& arg, int unused=-1);
int B_PrintObjEqsInfos(const std::string& arg, int unused=-1);
int B_PrintObjDef_1(const std::string& arg, int type);
int B_PrintObjDef(const std::string& arg, int type);
int B_PrintObjDefArgs(const std::string& arg, int type);

/* b_season.c */
int B_WsSeasonAdj(const std::string& arg, int unused=-1);
int B_season(const std::string& arg);
int DS_test(double* vec, int nb, int* beg, int* dim, int nbper, double* shift);
int DS_vec(double* vec, double* c1, double* i1, double* season, int nb, int nbper, double shift);
int DS_smpl(const std::shared_ptr<Sample> f_smpl, const std::shared_ptr<Sample> ws_smpl, 
            std::shared_ptr<Sample>& t_smpl, int& shift);
int DS_extr(double* vec, int dim, int nbper, double* bi, double shift);

/* b_sql.c */
std::string RPS_Open(const std::vector<std::string>& args);
std::string RPS_Field(const std::vector<std::string>& args);
std::string RPS_NbFlds(const std::vector<std::string>& args);
std::string RPS_Record(const std::vector<std::string>& args);
std::string RPS_Query(const std::vector<std::string>& args);
std::string RPS_Sql(const std::vector<std::string>& args);
std::string RPS_Next(const std::vector<std::string>& args);
std::string RPS_Close(const std::vector<std::string>& args);

/* b_step.c */
int B_EqsStepWise(const std::string& arg, int unused=-1);

/* b_trend.c */
int B_WsTrend(const std::string& arg, int unused=-1);
int B_WsTrendStd(const std::string& arg, int unused=-1);

/* b_view.c */
int B_ViewVar(const std::string& arg, int unused=-1);
int B_PrintVar(const std::string& arg, int unused=-1);
int B_ViewPrintVar(const std::string& arg, int mode);
int B_ViewByTbl(const std::string& arg, int unused=-1);
int B_ViewTbl(const std::string& arg, int unused=-1);
int B_PrintTbl(const std::string& arg, int unused=-1);
int B_ViewGr(const std::string& arg, int unused=-1);
int B_PrintGr(const std::string& arg, int unused=-1);
int B_ViewPrintTbl_1(const std::string& c_name, const std::string& smpl);
int B_ViewPrintGr_1(const std::string& names, const std::string& gsmpl);
int B_ViewPrintTbl(const std::string& arg, int type, int mode);
int B_ViewTblFile(const std::string& arg, int unused=-1);
int B_ViewTblEnd();
