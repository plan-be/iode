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


extern void kmsg_null(const char* msg);
extern void IodeSuppressMsgs();
extern void IodeResetMsgs();

#ifdef SCRCPP
}
#endif
#endif /* _IODEAPI_ */







