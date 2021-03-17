#define SCRSUPPROTO
#include "scr4.h"

#ifdef SCRPROTO
extern int Wprintf(char *, ...);
#endif

SCR_ask(type, text, c1, c2, c3, c4, c5, c6, c7) /* JMP 24-01-96 */
int     type;
U_ch    *text, *c1, *c2, *c3, *c4, *c5, *c6, *c7;
{
    U_ch    buffer[256], yes[10], no[10], mid[10], *rep;
    U_ch    *buts[4];
    int     rc;

    if(ERR_TYPE == 0) return(0); /* JMP 13-02-2004 */
    if(text == 0) text = SCR_err_txt(117);
    sprintf(buffer, text, c1, c2, c3, c4, c5, c6, c7);

    if(ERR_TYPE == 0) {
	Wprintf("%s", buffer);  /* JMP 17-01-98 */
	return(0);
	}

    strcpy(yes, " Yes ");
    strcpy(no,  " No ");
    strcpy(mid, " ? ");
    rep = SCR_err_txt(502);
    if(rep) {
	rep[6] = 0;
	sprintf(yes, " %s ", rep);
	}
    rep = SCR_err_txt(503);
    if(rep) {
	rep[6] = 0;
	sprintf(no, " %s ", rep);
	}

    buts[0] = yes;
    buts[1] = no;
    buts[2] = buts[3] = 0;
    switch(type) {
	case 0:
	    return(SCR_MessageBox("", buffer, buts));
	case 1:
	    return(SCR_MessageBox1("", buffer, buts, 1, 0));
	case 2:
	    buts[1] = mid;
	    buts[2] = no;
	    while(1) {
		rc = SCR_MessageBox1("", buffer, buts, 1, 1);
		if(type != 2) return(rc);
		if(rc == 0) return(0);
		if(rc == 2) return(1);
		}
	default : return(0); /* JMP 06-01-98 */
	}
}

