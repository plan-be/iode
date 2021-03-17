#ifndef _IODEAPI_
#define _IODEAPI_
#ifdef SCRCPP
extern "C" {
#endif


/* Init */
extern  IodeInit();
extern  IodeEnd();

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
extern  TBL* 	IodeGetTblDefinition(char *name);
extern  char    *IodeGetTblTitle(char *name);
extern  int     IodeSetTblFile(int ref, char *filename);

extern  int     IodeSimulate(char *byear, char *eyear, double eps, double relax, int maxit);

extern  int     IodeGetNbDec();
extern          IodeSetNbDec(int nbdec);

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

#ifdef SCRCPP
}
#endif
#endif /* _IODEAPI_ */






