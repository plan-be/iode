#include "scr.h"

extern FILE    *fd_cmt;

/*NH*/
SCR_read_cmt_len(pos, txt)      /* BP_MODIF 14-04-95 */
long    pos;
char    *txt;
{
    short   len;

    if(pos <= 0L) return(0);
    SCR_open_cmt(0);
    fseek(fd_cmt, pos + SCR_CMT_FILE_POS, 0);
    fread(&len, sizeof(short), 1, fd_cmt);
    fread(txt, len, 1, fd_cmt);

    return(0);
}

/*NH*/
SCR_read_cmt(pos, txt, len)      /* BP_M 06-04-1993 16:06 */
long    pos;
char    *txt;
int     len;
{
    if(pos <= 0L) return(0);
    SCR_open_cmt(0);
    fseek(fd_cmt, pos + SCR_CMT_FILE_POS, 0);
    fread(txt, len, 1, fd_cmt);

    return(0);
}


