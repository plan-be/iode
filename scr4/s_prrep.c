#include "scr4.h"

ScrPrintReport(char *infile, char *outfile, int (*fn)())
{
    FILE    *fd_out, *fd_in;
    char    topic[80], filetxt[50];
    static  char  nameimp[80];
    int     c, i, nc;

ag:
    strcpy(filetxt, infile); SCR_strip(filetxt);

    fd_in = fopen(filetxt, "r");
    if(fd_in == 0) {
	PG_display_error(SCR_err_txt(636), filetxt);
	return(-1);
    }

    strcpy(filetxt, outfile); SCR_strip(filetxt);
    fd_out = fopen(filetxt, "w+");
    if(fd_out == 0) {
	PG_display_error(SCR_err_txt(636), filetxt);
	fclose(fd_in);
	return(-1);
    }

    nc = fn("SKIPBL");
    for(i = 0 ; i < nc ;  i++) fputc('\n', fd_out);

    nc = 0;
    while(!feof(fd_in)) {
	c = fgetc(fd_in);
	if(c == EOF) break;
	if(c != '$') {
	    fputc(c, fd_out);
	    nc++;
	    continue;
	}
	i = 0;
	while(!feof(fd_in)) {
	    c = fgetc(fd_in);
	    if(c == EOF) break;
	    if(U_is_in(c, " \n\t,;.:/\\&\"'()!-?=+[]<>$*@#") != 0 || c > 126) {
		ungetc(c, fd_in);
		break;
	    }
	    topic[i++] = c;
	}
	topic[i] = 0;
	//strcpy(C_topic, topic);
	// Debug("C_topic = '%s'\n", C_topic);
	i = (*fn)(topic, fd_in, fd_out);
	if(i == 1)
	    while(!feof(fd_in)) {
		c = fgetc(fd_in);
		if(c == EOF || c != ' ') break;
	    }
    }
    fprintf(fd_out, "\f");
    fclose(fd_in);
    fclose(fd_out);

    return(0);
}

