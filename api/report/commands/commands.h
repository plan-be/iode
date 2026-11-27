#pragma once

#include "api/pch.h"
#include "api/objs/kdb.h"        
#include "api/objs/equations.h"        
#include "api/report/reports.h"


inline char K_AggrChar;

/* b_data.c */
int B_DataPattern(const std::string& arg, int type);
int B_DataRasVar(const std::string& arg, int unused=-1);
int B_DataCalcVar(const std::string& arg, int unused=-1);
int B_DataCreate_1(const std::string& arg, int type);
int B_DataCreate(const std::string& arg, int type);
int B_DataDelete_1(const std::string& arg, int type);
int B_DataDelete(const std::string& arg, int type);
int B_DataRename(const std::string& arg, int type);
int B_DataDuplicate(const std::string& arg, int type);
int B_DataUpdate(const std::string& arg, int type);
std::vector<std::string> B_DataSearchParms(const std::string& name, bool word, bool ecase,
    bool names, bool forms, bool texts, int type);
int B_DataSearch(const std::string& arg, int type);
int B_DataEditCnf(const std::string& arg, int unused=-1);
int B_DataListSort(const std::string& arg, int unused=-1);
int B_DataScan(const std::string& arg, int type);
int B_DataExist(const std::string& arg, int type);
int B_DataAppend(const std::string& arg, int type);
int B_DataList(const std::string& arg, int type);
int B_DataCalcLst(const std::string& arg, int unused=-1);
int B_DataListCount(char* arg, int unused=-1);
int B_DataCompareEps(const std::string& arg, int unused=-1);
int B_DataCompare(const std::string& arg, int type);
int B_DataDisplayGraph(const std::string& arg, int unused=-1);
int B_DataPrintGraph(const std::string& arg, int unused=-1);

/* b_est.c */
int B_EqsEstimate(const std::string& arg, int unused=-1);
int B_EqsEstimateEqs(Sample* smpl, char* pattern);
int B_EqsSetSample(const std::string& arg, int unused=-1);
int B_EqsSetMethod(const std::string& arg, int unused=-1);
int B_EqsSetBloc(const std::string& arg, int unused=-1);
int B_EqsSetCmt(const std::string& arg, int unused=-1);
int B_EqsSetInstrs(const std::string& arg, int unused=-1);

/* b_file.c */
int B_FilePrint(char* arg, int type);
int B_FileDelete(const std::string& arg, int type);
int B_FileCopy(const std::string& arg, int type);
int B_FileRename(const std::string& arg, int type);

/* b_fsys.c */
int B_SysRename(const std::string& arg, int unused=-1);
int B_SysCopy(const std::string& arg, int unused=-1);
int B_SysAppend(const std::string& arg, int unused=-1);
int B_SysDelete(const std::string& arg, int unused=-1);
int B_SysOemToUTF8(const std::string& arg, int unused=-1);
int B_SysAnsiToUTF8(const std::string& arg, int unused=-1);
int B_SysAnsiToOem(const std::string& arg, int unused=-1);
int B_SysOemToAnsi(const std::string& arg, int unused=-1);

/* b_htol.c */
int B_WsHtoLLast(const std::string& arg, int unused=-1);
int B_WsHtoLMean(const std::string& arg, int unused=-1);
int B_WsHtoLSum(const std::string& arg, int unused=-1);

/* b_idt.c */
int B_IdtExecute(const std::string& arg, int unused=-1);
int B_IdtExecuteIdts(Sample* smpl, const std::vector<std::string>& idts);
int B_IdtExecuteVarFiles(const std::string& arg, int unused=-1);
int B_IdtExecuteSclFiles(const std::string& arg, int unused=-1);
int B_IdtExecuteTrace(const std::string& arg, int unused=-1);

/* b_ltoh.c */
int B_WsLtoHStock(const std::string& arg, int unused=-1);
int B_WsLtoHFlow(const std::string& arg, int unused=-1);

/* b_model.c */
int B_ModelSimulate(const std::string& arg, int unused=-1);
int B_ModelSimulateParms(const std::string& arg, int unused=-1);
int B_ModelExchange(const std::string& arg, int unused=-1);
int B_ModelCompile(const std::string& arg, int unused=-1);
int B_ModelCalcSCC(const std::string& arg, int unused=-1);
int B_ModelSimulateSCC(const std::string& arg, int unused=-1);
int B_ModelSimulateSaveNIters(const std::string& arg, int unused=-1);
int B_ModelSimulateSaveNorms(const std::string& arg, int unused=-1);

/* b_ras.c */
int RasExecute(char *pattern, char *xdim, char *ydim, Period *rper, Period *cper, int maxit, double eps);

/* b_ws.c */
int B_WsLoad(const std::string& arg, int type);
int B_WsDump(KDB* kdb, char* filename);
int B_WsSave(const std::string& arg, int type);
int B_WsSaveCmp(const std::string& arg, int type);
int B_WsExport(const std::string& arg, int type);
int B_WsImport(const std::string& arg, int type);
int B_WsSample(const std::string& arg, int unused=-1);
int B_WsClear(const std::string& arg, int type);
int B_WsClearAll(const std::string& arg, int unused=-1);
int B_WsDescr(const std::string& arg, int type);
int B_WsName(char* arg, int type);
int B_WsCopy(const std::string& arg, int type);
int B_WsMerge(const std::string& arg, int type);
int B_WsExtrapolate(const std::string& arg, int unused=-1);
int B_WsAggrChar(const std::string& arg, int unused=-1);
int B_WsAggrSum(const std::string& arg, int unused=-1);
int B_WsAggrProd(const std::string& arg, int unused=-1);
int B_WsAggrMean(const std::string& arg, int unused=-1);
int B_WsAggr(int method, const std::string& arg);
double *B_StatUnitRoot_1(const std::string& arg, int print);
int B_StatUnitRoot(const std::string& arg, int unused=-1);
int B_CsvSave(const std::string& arg, int type);
int B_CsvNbDec(const std::string& nbdec, int unused=-1);
int B_CsvSep(const std::string& sep, int unused=-1);
int B_CsvNaN(const std::string& nan, int unused=-1);
int B_CsvAxes(const std::string& var, int unused=-1);
int B_CsvDec(const std::string& dec, int unused=-1);

/* b_xode.c */
int B_FileImportCmt(const std::string& arg, int unused=-1);
int B_FileImportVar(const std::string& arg, int unused=-1);
