/**
 *  @header4iode
 *  
 *  IODEAPI.H
 *  ----------
 *  Contains 
 *      - declarations of the functions defined inf b_api.c.
 *  
 *  This file is included in iodebase.h.
 */

#ifndef _IODEAPI_
#define _IODEAPI_

#ifdef SCRCPP
extern "C" {
#endif

extern int IodeInit();
extern int IodeEnd();

extern int IodeClearAll();
extern char **IodeContents(char *pattern, int type);

extern int IodeGetSampleLength();
extern int IodeIsSampleSet();
extern char *IodeGetSampleAsString();
extern char **IodeGetSampleAsPeriods();
extern char **IodeCreateSampleAsPeriods(char* aper_from, char* aper_to);
extern double *IodeGetSampleAsDoubles(int *lg);
extern int IodeSetSampleStr(char* str_from, char* str_to);
extern int IodeSetSample(int from, int to);


extern int IodeDeleteObj(char* obj_name, int obj_type);
extern char *IodeGetCmt(char *name);
extern int IodeSetCmt(char *name, char *cmt);
extern int IodeGetEqs(char *name, char**lec, int *method, char*sample_from, char* sample_to, char**blk, char**instr, float *tests);
extern char *IodeGetEqsLec(char *name);
extern int IodeSetEqs(char *name, char *eqlec);
extern char *IodeGetIdt(char *name);
extern int IodeSetIdt(char *name, char *idt);
extern char *IodeGetLst(char *name);
extern int IodeSetLst(char *name, char *lst);
extern int IodeGetScl(char *name, double* value, double *relax, double *std_err);
extern int IodeSetScl(char *name, double value, double relax, double std);
extern char* IodeGetTbl(char *name, char *gsmpl);
extern char* IodeGetTblTitle(char *name);
extern TBL* IodeGetTblDefinition(char *name);
extern int IodeSetTblFile(int ref, char *filename);

extern int IodeExecuteIdts(char *smpl, char *idt_list, char *var_files, char *scl_files, int trace);

extern int IodeEstimate(char* veqs, char* afrom, char* ato);

extern int IodeModelSimulate(char *per_from, char *per_to, char *eqs_list, char *endo_exo_list, double eps, double relax, int maxit, int init_values, int sort_algo, int nb_passes, int debug, double newton_eps, int newton_maxit, int newton_debug);
extern int IodeModelCalcSCC(int nbtris, char* pre_listname, char* inter_listname, char* post_listname, char *eqs_list);
extern int IodeModelSimulateSCC(char *per_from, char *per_to, 
                         char *pre_eqlist, char *inter_eqlist, char* post_eqlist,
                         double eps, double relax, int maxit, 
                         int init_values, int debug, 
                         double newton_eps, int newton_maxit, int newton_debug);
extern double IodeModelSimNorm(char* period);
extern int IodeModelSimNIter(char* period);
extern int IodeModelSimCpu(char* period);     
extern int IodeModelCpuSort();
extern int IodeModelCpuSCC();
                         
extern int IodeExecArgs(char *filename, char **args);
extern int IodeExec(char *filename);

extern double IodeExecLecT(char* lec, int t);
extern double *IodeExecLec(char* lec);

extern int IodeGetChart(char *name, char *gsmpl);
extern int IodeFreeChart(int hdl);
extern int IodeChartNl(int hdl);
extern int IodeChartNc(int hdl);
extern char IodeChartType(int hdl, int i);
extern int IodeChartAxis(int hdl, int i);
extern char *IodeChartTitle(int hdl, int i);
extern double *IodeChartData(int hdl, int i);

extern void kmsg_null(const char* msg);
extern void IodeSuppressMsgs();
extern void IodeResetMsgs();

void IodeAddErrorMsg(char* msg);
void IodeDisplayErrorMsgs();
void IodeClearErrorMsgs();

extern int IodeSetNbDec(int nbdec);
extern int IodeGetNbDec();

#ifdef SCRCPP
}
#endif
#endif /* _IODEAPI_ */







