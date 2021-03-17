#include "iode.h"
#include <s_a2m.h>

B_FilePrint(filename, a2m)  /* GB 12-97 */
char    *filename;
int     a2m;
{
    int     nr, cmdch, escch, defch, sepch, lfon, blon;
    FILE    *fd;
    unsigned char    buf[4096], cmt[10], head[10];
    long    nl = 0;

    fd = fopen(filename, "r");
    if(fd == NULL) {
	B_seterror("File '%s' not found", filename);
	return(-1);
	}

    C_PrintDestDecLang(0, 1, 0);
    if(a2m == 0) {
	W_record(".pardef cmd_1 color=B size=9 spaceb=2 spacea=0 bold=n\n");
	W_record(".par cmd_1\n");
	W_flush();
	escch = A2M_ESCCH;
	defch = A2M_DEFCH;
	lfon  = A2M_LFON;
	blon  = A2M_BLON;
	cmdch  = A2M_CMDCH; /* JMP 07-06-99 */

	A2M_CMDCH = 0x1;   /* JMP 07-06-99 */
	A2M_DEFCH = 0xff;
	A2M_ESCCH = 2;
	A2M_LFON = 1 ;
	A2M_BLON = 1;
	cmt[0] = '\n';
	cmt[1] = cmt[2] = 0x1;
	cmt[3] = '\n';
	cmt[4] = 0;
    }

    while(1) {
	nr = SCR_read_line(fd, buf, 4095);
	if(nr < 0) break;
	nl++;
	/* nr = fread(buf, 1, 512, fd);
	   buf[nr] = 0;                    /* JMP 11-06-99 */
	if(a2m == 0) {
	    sprintf(head, "%cb%04ld%cB ", 2, nl, 2);
	    W_record(head);
	    }
	W_record(buf);
	W_record("\n");
	if(a2m == 0) W_flush();
	else {
	    if(A2M_CMDCH == buf[0] &&
		(strcmp(buf + 1, "ge") == 0 ||
		 strcmp(buf + 1, "te") == 0 ||
		 memcmp(buf + 1, "par", 3) == 0)) W_flush();
	    }
	}
    W_close();

    if(!a2m) {
	A2M_CMDCH = cmdch;  /* JMP 07-06-99 */
	A2M_DEFCH = defch;
	A2M_ESCCH = escch;

	A2M_LFON = lfon;
	A2M_BLON = blon;
    }
    fclose(fd); /* JMP 10-08-99 */
    return(0);
}
/*
B_FilePrintGdi(filename)
char    *filename;
{
    int     nr;
    FILE    *fd;
    unsigned char buf[4096], head[10];
    long    nl = 0;

    fd = fopen(filename, "r");
    if(fd == NULL) {
	B_seterror("File '%s' not found", filename);
	return(-1);
	}

    if(WprChInit(0L, 0, filename)) {
	fclose(fd);
	B_seterror("Can't open Printer or Job cancelled by the user");
	return(-1);
	}

    while(1) {
	nr = SCR_read_line(fd, buf, 4095);
	if(nr < 0) break;
	nl++;
	sprintf(head, "%04ld: ", nl);
	WprChPrintString(head);
	WprChPrintString(buf);
	WprChPrintString("\n");
	}

    WprChEnd();
    fclose(fd);
    return(0);
}
*/
