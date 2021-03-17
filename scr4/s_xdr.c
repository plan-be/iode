#include "s_xdr.h"
#include <stdio.h>

void XDR_rev(buf, n, width)
unsigned char   *buf;
int             n, width;
{
    int     i, j, k, l, m;

    l = width / 2 ;
    for(i = 0 ; i < n ; i++, buf += width)
	for(j = 0 ; j < l ; j++) {
	    m = width - j - 1;
	    k = buf[j];
	    buf[j] = buf[m];
	    buf[m] = k;
	    }
}

XDR_write(fd, in, n, width)
FILE            *fd;
unsigned char   *in;
int             n, width;
{
    int     rc;

    XDR_rev(in, n, width);
    rc = fwrite(in, width, 1, fd);
    XDR_rev(in, n, width);
    return(rc);
}

XDR_read(fd, out, n, width)
FILE            *fd;
unsigned char   *out;
int             n, width;
{
    int     rc;

    rc = fread(out, width, 1, fd);
    XDR_rev(out, n, width);
    return(rc);
}



