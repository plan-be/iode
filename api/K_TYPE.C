#include "iode.h"

int isfile(argc, argv)
int     argc;
char    *argv[];
{
    int     i;
    FILE    *fd;

    for(i = 1; i < argc; i++) {
	fd = fopen(argv[i], "r");
	if(fd == NULL) {
	    kerror(0, "%s does not exist", argv[i]);
	    return(-1);
	}
	close(fd);
    }
    return(0);
}

int isname(argc, argv)
int     argc;
char    *argv[];
{
    int     i;

    for(i = 1; i < argc; i++) {
	if(strlen(argv[i]) > K_MAX_NAME) {
	    kerror(0, "%s : name too long", argv[i]);
	    return(-1);
	}
	if(!isalpha(argv[i][0]) && argv[i][0] != '$') {
	    kerror(0, "%s first char must be a letter", argv[i]);
	    return(-1);
	}
    }
    return(0);
}

int isper(argc, argv)
int     argc;
char    *argv[];
{
    int     i;
    PERIOD  *per;

    for(i = 1; i < argc; i++) {
	per = PER_atoper(argv[i]);
	if(per == NULL || per->p_y == 0 || per->p_s == 0 || per->p_p == 0) {
	    kerror(0, "%s invalid period", argv[i]);
	    return(-1);
	}
    }
    return(0);
}

int isreal(argc, argv)
int     argc;
char    *argv[];
{
    return(0);
}

int isint(argc, argv)
int     argc;
char    *argv[];
{
    return(0);
}

int ismethod(argc, argv)
int     argc;
char    *argv[];
{
    return(0);
}

int isyon(argc, argv)
int     argc;
char    *argv[];
{
    return(0);
}





