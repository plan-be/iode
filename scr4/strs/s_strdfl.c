#include <stdio.h>
#include "s_strs.h"

/* =============================================================
Reads a file and store its contents in a global buffer. The pointer to
the buffer is returned by the function and allocated by malloc().

&EN count       = file size (= buffer size)
&EN filename    = file to be read

&RT pointer to the allocated buffer or NULL in case of failure
================================================================ */


U_ch *SCR_LoadRawFile(char *filename, long *count)
{
    FILE    *fd;
    U_ch    *buf = 0;

    fd = fopen(filename, "rb");
    if(fd == 0) return(buf);
    fseek(fd, 0L, 2);
    *count = ftell(fd);
    if(*count <= 0) goto fin;
    buf = (U_ch *)SCR_malloc(1 + *count);
    if(buf) {
	fseek(fd, 0L, 0);
	fread((char *)buf, *count, 1, fd);
	}

fin:
    fclose(fd);
    return(buf);
}



