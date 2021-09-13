#include "scr.h"

extern FILE *fd_cmt;

/*NH*/
SCR_get_cmt(flag, pos)
int     flag;
long    pos;
{
    if(flag == 0) return(-1);

    SCR_read_cmt_txt(pos);

    return(0);
}

/*NH*/
SCR_get_cmt_ele(pos1, pos2)
long    pos1;
int     pos2;
{
    if(pos2 < 0 || pos1 < 0) return(-1);  /* JMP 29-09-91 */
    SCR_read_cmt_txt(pos1 + pos2);

    return(0);
}

/*NH*/
SCR_read_cmt_txt(pos)
long    pos;
{
    int     i = 0;


    SCR_open_cmt(0);
    fseek(fd_cmt, pos + SCR_CMT_FILE_POS, 0); /* JMP 16-04-95 */
    while(!feof(fd_cmt) && i < SCR_CMT.c_nc) { /* JMP41 07-01-93 */
	 SCR_CMT_TXT[i] = fgetc(fd_cmt);
	 if(SCR_CMT_TXT[i] == 0) break;
	 i++;
	}
    SCR_CMT_TXT[i] = 0;

    return(0);
}

/*NH*/
SCR_get_comment(pos1, pos2, flag)
long    pos1;
int     pos2;
int     flag;
{
    if(SCR_get_cmt_ele(pos1, pos2) && SCR_get_cmt(flag, pos1))
	SCR_CMT_TXT[0] = 0;

    return(0);
}

