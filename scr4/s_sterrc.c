#include <stdio.h>

void SCR_close_err_txt()
{

    extern FILE *fd_msg;
    if(fd_msg == 0) return;
    fclose(fd_msg);
    fd_msg = 0;
}

SCR_end_cmt()
{
    extern FILE    *fd_cmt;

    if(fd_cmt != 0) fclose(fd_cmt);
    fd_cmt = 0;
    return(0);
}
