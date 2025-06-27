#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"        
#include "api/report/reports.h"

#ifdef __cplusplus
extern "C" {
#endif

extern char K_AggrChar;

/* b_data.c */
extern int B_DataPattern(char* arg,int type);
extern int B_DataRasVar(char* arg);
extern int B_DataCalcVar(char *);
extern int B_DataCreate_1(char *,int *);
extern int B_DataCreate(char *,int );
extern int B_DataDelete_1(char *,int *);
extern int B_DataDelete(char *,int );
extern int B_DataRename(char *,int );
extern int B_DataDuplicate(char *,int );
//extern int B_DataUpdateEqs(char *,char *,char *,int ,SAMPLE *,char *,char *,float *,int );
//extern int B_DataUpdateTbl(char *,char *);
extern int B_DataUpdate(char *,int );
extern char** B_DataSearchParms(char* name, int word, int ecase, int names, int forms, int texts, int type);
extern int B_DataSearch(char *,int );
extern int B_DataEditCnf(char *);
// extern int my_strcmp(const void *,const void *);
extern int B_DataListSort(char *);
extern int B_DataScan(char *,int );
extern int B_DataExist(char *,int );
extern int B_DataAppend(char *,int );
extern int B_DataList(char *,int );
extern int B_DataCalcLst(char *);
extern int B_DataListCount(char *);
extern int B_DataCompareEps(char* arg);
extern int B_DataCompare(char *,int );
//extern int B_DataEditGraph(int ,char *);
extern int B_DataDisplayGraph(char *);
extern int B_DataPrintGraph(char *);

/* b_est.c */
//extern char **B_EqsSplitSmplName(char *,SAMPLE **);
extern int B_EqsEstimate(char *);
extern int B_EqsEstimateEqs(SAMPLE *,char **);
extern int B_EqsSetSample(char *);
extern int B_EqsSetMethod(char *);
extern int B_EqsSetBloc(char *);
extern int B_EqsSetCmt(char *);
extern int B_EqsSetInstrs(char *);

/* b_file.c */
extern int B_FilePrint(char *,int );
//extern int B_unlink_1(char *,int *);
extern int B_FileDelete(char *,int );
extern int B_FileCopy(char *,int );
extern int B_FileRename(char *,int );

/* b_fsys.c */
extern int B_SysRename(char* arg);
extern int B_SysCopy(char* arg);
extern int B_SysAppend(char* arg);
extern int B_SysDelete(char* arg);
extern int B_SysOemToUTF8(char *arg);
extern int B_SysAnsiToUTF8(char *arg);
extern int B_SysAnsiToOem(char *arg);
extern int B_SysOemToAnsi(char *arg);

/* b_htol.c */
extern int B_WsHtoLLast(char *);
extern int B_WsHtoLMean(char *);
extern int B_WsHtoLSum(char *);
//extern int HTOL_smpl(SAMPLE *,SAMPLE *,SAMPLE **,int *,int *);
//extern int B_htol(int ,char *);

/* b_idt.c */
extern int B_IdtExecute(char *);
extern int B_IdtExecuteIdts(SAMPLE *,char **);
extern int B_IdtExecuteVarFiles(char *);
extern int B_IdtExecuteSclFiles(char *);
extern int B_IdtExecuteTrace(char *);

/* b_ltoh.c */
extern int B_WsLtoHStock(char *);
extern int B_WsLtoHFlow(char *);
//extern int LTOH_smpl(SAMPLE *,SAMPLE *,SAMPLE **,int *,int *);
// extern int B_ltoh(int ,char *);
//extern double LTOH_ylin(double *,double );
//extern int LTOH_lin(int ,double *,int ,double *,int ,int );
//extern int LTOH_y2cs(double *,int ,double *);
//extern double LTOH_ycs(double *,double *,double );
//extern int LTOH_cs(int ,double *,int ,double *,int ,int );

/* b_model.c */
extern int B_ModelSimulate(char *);
//extern int B_ModelSimulateEqs(SAMPLE *,char **);
extern int B_ModelSimulateParms(char *);
extern int B_ModelExchange(char *);
extern int B_ModelCompile(char *);
extern int KE_compile(KDB *);
extern int B_ModelCalcSCC(char *arg);
extern int B_ModelSimulateSCC(char *arg);
extern int B_ModelSimulateSaveNIters(char *arg);
extern int B_ModelSimulateSaveNorms(char *arg);

/* b_ras.c */
extern int RasExecute(char *pattern, char *xdim, char *ydim, PERIOD *rper, PERIOD *cper, int maxit, double eps);

/* b_ws.c */
extern int B_WsLoad(char *,int );
extern int X_findtype(char *);
extern int B_WsDump(KDB *,char *);
extern int B_WsSave(char *,int );
extern int B_WsSaveCmp(char *,int );
extern int B_WsExport(char *,int );
extern int B_WsImport(char *,int );
extern int B_WsSample(char *);
extern int B_WsClear(char *,int );
extern int B_WsClearAll(char* arg);
extern int B_WsDescr(char *,int );
extern int B_WsName(char *,int );
extern int B_WsCopy(char *,int );
extern int B_WsMerge(char *,int );
extern int B_WsExtrapolate(char *);
extern int B_WsAggrChar(char *);
extern int B_WsAggrSum(char *);
extern int B_WsAggrProd(char *);
extern int B_WsAggrMean(char *);
extern int B_WsAggr(int ,char *);
extern double *B_StatUnitRoot_1(char *,int );
extern int B_StatUnitRoot(char *);
extern int B_CsvSave(char* arg, int type);
extern int B_CsvNbDec(char *nbdec);
extern int B_CsvSep(char *sep);
extern int B_CsvNaN(char *nan);
extern int B_CsvAxes(char *var);
extern int B_CsvDec(char *dec);

/* b_xode.c */
extern int B_FileImportCmt(char* arg);
extern int B_FileImportVar(char* arg);

#ifdef __cplusplus
}
#endif
