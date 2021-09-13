#include "scr4.h"

extern int FLD_mark[2];

/*NH*/
PGd_write_field_line(attr, line, col, txt, from, lg)
int     attr, line, col, from, lg;
U_ch    *txt;
{
    int mfrom, mlg, mark[2];

    mark[0] = FLD_mark[0];
    mark[1] = FLD_mark[1];
    if(mark[0] > mark[1]) {
	mfrom = mark[1];
	mark[1] = mark[0];
	mark[0] = mfrom;
	}

    SCR_set_attr(attr);
    //SCR_write_xattrs(line, col, 0, lg);
    SCR_write(line, col, txt + from, lg);
    if(mark[0] < 0 || mark[0] >= from + lg) return;

    mfrom = mark[0];
    if(mfrom < from) mfrom = from;
    mlg = 1 + mark[1] - mfrom;
    if(mlg + mfrom > lg + from) mlg = lg + from - mfrom;
    SCR_set_attr(SCR_SELECT_COLOR); /* JMP 15-12-98 */
    //SCR_write_xattrs(line, col + mfrom - from, 0, mlg);
    SCR_write(line, col + mfrom - from, txt + mfrom, mlg);
    SCR_set_attr(attr);
}

