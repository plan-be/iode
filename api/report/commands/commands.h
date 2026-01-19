#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"        
#include "api/objs/equations.h"        
#include "api/report/reports.h"


inline char K_AggrChar;

/* b_data.c */
int B_DataPattern(char* arg, int type);
int B_DataRasVar(char* arg, int unused=-1);
int B_DataCalcVar(char* arg, int unused=-1);
int B_DataCreate_1(char *,int *);
int B_DataCreate(char* arg, int type);
int B_DataDelete_1(char *,int *);
int B_DataDelete(char* arg, int type);
int B_DataRename(char* arg, int type);
int B_DataDuplicate(char* arg, int type);
//int B_DataUpdateEqs(char *,char *,char *,int ,Sample *,char *,char *,float *,int );
//int B_DataUpdateTbl(char *,char *);
int B_DataUpdate(char* arg, int type);
char** B_DataSearchParms(char* name, int word, int ecase, int names, int forms, int texts, int type);
int B_DataSearch(char* arg, int type);
int B_DataEditCnf(char* arg, int unused=-1);
// int my_strcmp(const void *,const void *);
int B_DataListSort(char* arg, int unused=-1);
int B_DataScan(char* arg, int type);
int B_DataExist(char* arg, int type);
int B_DataAppend(char* arg, int type);
int B_DataList(char* arg, int type);
int B_DataCalcLst(char* arg, int unused=-1);
int B_DataListCount(char* arg, int unused=-1);
int B_DataCompareEps(char* arg, int unused=-1);
int B_DataCompare(char* arg, int type);
//int B_DataEditGraph(int ,char *);
int B_DataDisplayGraph(char* arg, int unused=-1);
int B_DataPrintGraph(char* arg, int unused=-1);

/* b_est.c */
int B_EqsEstimate(char* arg, int unused=-1);
int B_EqsEstimateEqs(Sample* smpl, char* pattern);
int B_EqsSetSample(char* arg, int unused=-1);
int B_EqsSetMethod(char* arg, int unused=-1);
int B_EqsSetBloc(char* arg, int unused=-1);
int B_EqsSetCmt(char* arg, int unused=-1);
int B_EqsSetInstrs(char* arg, int unused=-1);

/* b_file.c */
int B_FilePrint(char* arg, int type);
//int B_unlink_1(char *,int *);
int B_FileDelete(char* arg, int type);
int B_FileCopy(char* arg, int type);
int B_FileRename(char* arg, int type);

/* b_fsys.c */
int B_SysRename(char* arg, int unused=-1);
int B_SysCopy(char* arg, int unused=-1);
int B_SysAppend(char* arg, int unused=-1);
int B_SysDelete(char* arg, int unused=-1);
int B_SysOemToUTF8(char* arg, int unused=-1);
int B_SysAnsiToUTF8(char* arg, int unused=-1);
int B_SysAnsiToOem(char* arg, int unused=-1);
int B_SysOemToAnsi(char* arg, int unused=-1);

/* b_htol.c */
int B_WsHtoLLast(char* arg, int unused=-1);
int B_WsHtoLMean(char* arg, int unused=-1);
int B_WsHtoLSum(char* arg, int unused=-1);
//int HTOL_smpl(Sample *,Sample *,Sample **,int *,int *);
//int B_htol(int ,char *);

/* b_idt.c */
int B_IdtExecute(char* arg, int unused=-1);
int B_IdtExecuteIdts(Sample *,char **);
int B_IdtExecuteVarFiles(char* arg, int unused=-1);
int B_IdtExecuteSclFiles(char* arg, int unused=-1);
int B_IdtExecuteTrace(char* arg, int unused=-1);

/* b_ltoh.c */
int B_WsLtoHStock(char* arg, int unused=-1);
int B_WsLtoHFlow(char* arg, int unused=-1);
//int LTOH_smpl(Sample *,Sample *,Sample **,int *,int *);
// int B_ltoh(int ,char *);
//double LTOH_ylin(double *,double );
//int LTOH_lin(int ,double *,int ,double *,int ,int );
//int LTOH_y2cs(double *,int ,double *);
//double LTOH_ycs(double *,double *,double );
//int LTOH_cs(int ,double *,int ,double *,int ,int );

/* b_model.c */
int B_ModelSimulate(char* arg, int unused=-1);
//int B_ModelSimulateEqs(Sample *,char **);
int B_ModelSimulateParms(char* arg, int unused=-1);
int B_ModelExchange(char* arg, int unused=-1);
int B_ModelCompile(char* arg, int unused=-1);
int KE_compile(KDBEquations *);
int B_ModelCalcSCC(char* arg, int unused=-1);
int B_ModelSimulateSCC(char* arg, int unused=-1);
int B_ModelSimulateSaveNIters(char* arg, int unused=-1);
int B_ModelSimulateSaveNorms(char* arg, int unused=-1);

/* b_ras.c */
int RasExecute(char *pattern, char *xdim, char *ydim, Period *rper, Period *cper, int maxit, double eps);

/* b_ws.c */
int B_WsLoad(char* arg, int type);
int B_WsDump(KDB *,char *);
int B_WsSave(char* arg, int type);
int B_WsSaveCmp(char* arg, int type);
int B_WsExport(char* arg, int type);
int B_WsImport(char* arg, int type);
int B_WsSample(char* arg, int unused=-1);
int B_WsClear(char* arg, int type);
int B_WsClearAll(char* arg, int unused=-1);
int B_WsDescr(char* arg, int type);
int B_WsName(char* arg, int type);
int B_WsCopy(char* arg, int type);
int B_WsMerge(char* arg, int type);
int B_WsExtrapolate(char* arg, int unused=-1);
int B_WsAggrChar(char* arg, int unused=-1);
int B_WsAggrSum(char* arg, int unused=-1);
int B_WsAggrProd(char* arg, int unused=-1);
int B_WsAggrMean(char* arg, int unused=-1);
int B_WsAggr(int ,char *);
double *B_StatUnitRoot_1(char* arg, int type);
int B_StatUnitRoot(char* arg, int unused=-1);
int B_CsvSave(char* arg, int type);
int B_CsvNbDec(char *nbdec, int unused=-1);
int B_CsvSep(char *sep, int unused=-1);
int B_CsvNaN(char *nan, int unused=-1);
int B_CsvAxes(char *var, int unused=-1);
int B_CsvDec(char *dec, int unused=-1);

/* b_xode.c */
int B_FileImportCmt(char* arg, int unused=-1);
int B_FileImportVar(char* arg, int unused=-1);
