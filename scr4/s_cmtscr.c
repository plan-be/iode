#include "scr.h"

extern FILE    *fd_cmt;

/*NH*/
PG_display_scr(pg)
PAGE    *pg;
{
    int     i, j, nl, bl;
    short   len;
    char    *buf;

    SCR_open_cmt(0);

    fseek(fd_cmt, pg->pg_scr_pos + SCR_CMT_FILE_POS, 0); /* JMP 16-04-95 */
    fread(&len, sizeof(short), 1, fd_cmt);
    buf = SCR_malloc(len);
    fread(&i, 1, 1, fd_cmt);
    fread(buf, len, 1, fd_cmt);

    j = nl = bl = 0;
    SCR_set_attr(pg->pg_back);
    for(i = 0 ; i < len ; i++) {
	if(buf[i] == '\n') {
	    SCR_write_xattrs(pg->pg_line + nl, pg->pg_column, 0, j);
	    SCR_write(pg->pg_line + nl, pg->pg_column, buf + bl, j);
	    j = 0; nl++;
	    bl = i + 1;
	    continue;
	}
	j++;
    }
    SCR_free(buf);
    return(0);
}



