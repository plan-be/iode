#include "iode.h"


B_FileCopy(arg, type)
char *arg;
int  type;
{
    char    **args;
    int     rc = 0, ch;
	char    buf_from[1024]; // JMP 15-09-2015
    char    buf_to[1024];	// JMP 15-09-2015
    FILE    *from, *to;

    args = B_ainit_chk(arg, 0L, 2);
    if(args == 0) {
	rc = -1;
	goto fin;
    }

    K_set_ext(buf_from, args[0], type);
    K_set_ext(buf_to, args[1], type);

    if((from = fopen(buf_from, "r+b")) == 0) {
	B_seterror("FileCopy: unable to open file %.80s", buf_from);
	rc = -1;
	goto fin;
    }

    if((to = fopen(buf_to, "w+b")) == 0) {
	B_seterror("FileCopy: unable to create file %.80s", buf_to);
	rc = -1;
	goto fin;
    }

    while((ch = getc(from)) != EOF)
	if(putc(ch, to) == EOF) {
	    B_seterror("FileCopy: error writing %.80s", buf_to);
	    rc = -1;
	    goto fin;
	}
fin:
    if(from != 0) fclose(from);
    if(to != 0) {
	fclose(to);
	if(type <= K_VAR) K_setname(type, buf_to, buf_to);
	}
    A_free(args);
    return(rc);
}



