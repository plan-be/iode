#include "s_strs.h"
#include <stdio.h>
#include <string.h>

SCR_write_magic(fd, txt)
FILE    *fd;
U_ch    *txt;
{
    fwrite(txt, strlen(txt), 1, fd);
    return(0);
}

SCR_check_magic(fd, txt)
FILE    *fd;
U_ch    *txt;
{
    char    buf[80];
    int     lg = strlen(txt);

    fread(buf, lg, 1, fd);
    return(memcmp(txt, buf, lg));
}

