#include <stdio.h>

/* ===================================================================
Dumps file filein in hexadecimal.

&EN filein : input file name
&EN fileout : output file name
&EN nl : newline all 32 characters (64 in hex format)

&RT
&EN 0 : success
&EN -1 : cannot open filein
&EN -2 : cannot create fileout

&SA scr4_hex, SCR_dumphexfd()
=================================================================== */

SCR_dumphex(filein, fileout, nl)
char    *filein, *fileout;
int     nl;
{
    int     rc;
    FILE    *fd2;

    fd2 = fopen(fileout, "w+");
    if(fd2 == 0) return(-2);
/*    setvbuf(fd2, 0L, _IOFBF, 8192); */
    rc = SCR_dumphexfd(filein, fd2, nl);
    fclose(fd2);
    return(rc);
}


/* ===================================================================
Dumps file filein in hexadecimal in fdout.

&EN filein : input file name
&EN fdout : output FILE *
&EN nl : newline all 32 characters (64 in hex format)

&RT
&EN 0 : success
&EN -1 : cannot open filein

&SA scr4_hex, SCR_dumphex()
=================================================================== */

SCR_dumphexfd(filein, fdout, nl)
char    *filein;
FILE    *fdout;
int     nl;
{
    int     c;
    FILE    *fd1;
    long    pos = 0L;

#ifdef DOS
    fd1 = fopen(filein, "rb");
#else
    fd1 = fopen(filein, "r");
#endif

    if(fd1 == 0) return(-1);

/*    setvbuf(fd1, 0L, _IOFBF, 8192); */

    while(!feof(fd1)) {
	c = getc(fd1);
	if(c == EOF) break;
	fprintf(fdout, "%02x", c);
	pos++;
	if(nl && pos % 32L == 0) fprintf(fdout, "\n");
	}
    fclose(fd1);
    return(0);
}

/* ===================================================================
Dump a string and escape " and \ : replace " by \" and \ by \\.
Add quotes if addquotes <> 0.

&EN fd : output stream
&EN str : input null terminated string
&EN addquotes : 0 if no beginning and ending quotes to be added

&RT
&EN 0 : success
&EN -1 : fd == 0
=================================================================== */

SCR_fprintf_esc(FILE *fd, char *str, int addquotes)
{
    int     i;
	
	if(fd == 0) return(-1);
	if(addquotes) fprintf(fd, "\"");
		
	if(str) for(i = 0 ; str[i] ; i++) {
		if(U_is_in(str[i], "\"\\")) fprintf(fd, "\\");
		fprintf(fd, "%c", str[i]);
	}
    
	if(addquotes) fprintf(fd, "\"");
	
	return(0);
}










