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

#ifdef SCRCPP
}
#endif
#endif /* _IODEAPI_ */







