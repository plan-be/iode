#ifndef _S_XDR_
#define _S_XDR_

#ifdef DOS
#define INTEL
#endif
#ifdef SCRW32     /* JMP 17-02-97 */
#define INTEL
#endif
#ifdef XENIX
#define INTEL
#endif
#ifdef UNIX5R4
#define INTEL
#endif

#ifdef INTEL

#define XDR_l2s_s(s, n)
#define XDR_s2l_s(s, n)
#define XDR_l2s_l(s, n)
#define XDR_s2l_l(s, n)
#define XDR_l2s_f(s, n)
#define XDR_s2l_f(s, n)
#define XDR_l2s_d(s, n)
#define XDR_s2l_d(s, n)

#define XDR_wshort(fd, s, n)  fwrite(s, n, sizeof(short), fd)
#define XDR_wlong(fd, s, n)   fwrite(s, n, sizeof(long), fd)
#define XDR_wfloat(fd, s, n)  fwrite(s, n, sizeof(float), fd)
#define XDR_wdouble(fd, s, n) fwrite(s, n, sizeof(double), fd)

#define XDR_rshort(fd, s, n)  fread(s, n, sizeof(short ), fd)
#define XDR_rlong(fd, s, n)   fread(s, n, sizeof(long  ), fd)
#define XDR_rfloat(fd, s, n)  fread(s, n, sizeof(float ), fd)
#define XDR_rdouble(fd, s, n) fread(s, n, sizeof(double), fd)

#else

#include <s_proxdr.h>

#define XDR_l2s_s(s, n)       XDR_rev(s, n, sizeof(short  ))
#define XDR_s2l_s(s, n)       XDR_rev(s, n, sizeof(short  ))
#define XDR_l2s_l(s, n)       XDR_rev(s, n, sizeof(long   ))
#define XDR_s2l_l(s, n)       XDR_rev(s, n, sizeof(long   ))
#define XDR_l2s_f(s, n)       XDR_rev(s, n, sizeof(float  ))
#define XDR_s2l_f(s, n)       XDR_rev(s, n, sizeof(float  ))
#define XDR_l2s_d(s, n)       XDR_rev(s, n, sizeof(double ))
#define XDR_s2l_d(s, n)       XDR_rev(s, n, sizeof(double ))

#define XDR_wshort(fd, s, n)  XDR_write(fd, s, n, sizeof(short ))
#define XDR_wlong(fd, s, n)   XDR_write(fd, s, n, sizeof(long  ))
#define XDR_wfloat(fd, s, n)  XDR_write(fd, s, n, sizeof(float ))
#define XDR_wdouble(fd, s, n) XDR_write(fd, s, n, sizeof(double))

#define XDR_rshort(fd, s, n)  XDR_read(fd, s, n, sizeof(short ))
#define XDR_rlong(fd, s, n)   XDR_read(fd, s, n, sizeof(long  ))
#define XDR_rfloat(fd, s, n)  XDR_read(fd, s, n, sizeof(float ))
#define XDR_rdouble(fd, s, n) XDR_read(fd, s, n, sizeof(double))
#endif

#ifdef SCRPROTO
#include <stdio.h>
#include <s_proxdr.h>
#endif

#endif
