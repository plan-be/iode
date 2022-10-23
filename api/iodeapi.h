#ifndef _IODEAPI_
#define _IODEAPI_
#ifdef SCRCPP
extern "C" {
#endif


/* Init */
extern  int IodeInit();
extern  int IodeEnd();

/* File Operations & Objects */
extern  int  IodeLoad(char *name, int type);
extern  int  IodeSave(char *name, int type);
extern  int  IodeClearWs();
extern  char **IodeContents(char *pattern, int type);

/* Mode
#define K_LEVEL 0
#define K_DIFF  1
#define K_GRT   2
#define K_DIFFY 3
#define K_GRTY  4
*/

/* Added JMP 2/5/2017 */
extern  double  *IodeGetVector(char *name, int *lg);     
extern  double  *IodeGetSampleVector(int *lg);           
extern  char    *IodeGetLst(char *name); 
extern  double  *IodeGetScls(char *name);
extern  char    *IodeGetSampleString();
extern  char    *IodeContentsStr(char *pattern, int type);
extern  char    *IodeVersion();
/* End added JMP */

extern  double  IodeGetVar(char *name, int t, int mode);
extern  int     IodeSetVar(char *name, int t, int mode, double value);
extern  char    *IodeGetCmt(char *name);
extern  double  IodeGetScl(char *name);
extern  int     IodeSetScl(char *name, double value);

extern  char    *IodeGetEqs(char *name);
extern  char    *IodeGetTbl(char *name, char *smpl);
extern  TBL		*IodeGetTblDefinition(char *name);
extern  char    *IodeGetTblTitle(char *name);
extern  int     IodeSetTblFile(int ref, char *filename);

extern  int     IodeSimulate(char *byear, char *eyear, double eps, double relax, int maxit);

extern  int     IodeGetNbDec();
extern  int     IodeSetNbDec(int nbdec);

extern  int     IodeGetMaxt();
extern  char    *IodeGetPeriod(int t);
extern  int     IodeGett(char *period);

extern  int     IodeGetChart(char *name, char *smpl);
extern  int     IodeChartNl(int hdl);
extern  int     IodeChartNc(int hdl);
extern  char    IodeChartType(int hdl, int i);
extern  int     IodeChartAxis(int hdl, int i);
extern  char    *IodeChartTitle(int hdl, int i);
extern  double  *IodeChartData(int hdl, int i);
extern  int     IodeFreeChart(int hdl);

/* Extract from b_api.c

int IodeInit();
int IodeEnd();
char *IodeVersion();
int IodeLoadVar(char *name);
int IodeLoad(char *name, int type);
int IodeSaveVar(char *name);
int IodeSave(char *name, int type);
int IodeClearWs();
char **IodeContents(char *pattern, int type);
char *IodeContentsStr(char *pattern, int type);
double IodeGetVar(char *name, int t, int mode);
double *IodeGetVector(char *name, int *lg);
int IodeSetVector(char *name, double *value, int len);
double *IodeGetSampleVector(int *lg);
char *IodeGetSampleString();
int IodeSetVar(char *name, int t, int mode, double value);
char *IodeGetCmt(char *name);
char *IodeGetLst(char *name);
char *IodeGetEqs(char *name);
double IodeGetScl(char *name);
double *IodeGetScls(char *name);
int IodeSetScl(char *name, double value);
int IodeSetNbDec(int nbdec);
int IodeGetNbDec();
char* IodeGetTbl(char *name, char *gsmpl);
char* IodeGetTblTitle(char *name);
TBL* IodeGetTblDefinition(char *name);
int IodeSetTblFile(int ref, char *filename);
int IodeSimulate(char *byear, char *eyear, double eps, double relax, int maxit);
int IodeGetMaxt();
char *IodeGetPeriod(int t);
int IodeGett(char *period);
int IodeGetChart(char *name, char *gsmpl);
int IodeFreeChart(int hdl);
int IodeChartNl(int hdl);
int IodeChartNc(int hdl);
char IodeChartType(int hdl, int i);
int IodeChartAxis(int hdl, int i);
char *IodeChartTitle(int hdl, int i);
double *IodeChartData(int hdl, int i);
int IodeExecArgs(char *filename, char **args);
int IodeExec(char *filename);
*/

#ifdef SCRCPP
}
#endif
#endif /* _IODEAPI_ */






