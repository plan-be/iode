#include "scr.h"
#include <stdlib.h>

int             AR_BUFSIZE = 8192;
int             AR_ERRNO;
unsigned char   AR_CH = '\xfe';
char            AR_MAGIC[] = "JPAK1 1.0\032";

char *AR_ERRORS[] = {
    "Success",
    "Cannot access archive",
    "Not archive file or old version",
    "Cannot access data file",
    "Unexpected EOF"
};

/* ════════════════════════════════════════════════════════════════════
Fonction retournant le message associé à la dernière erreur
recontrée dans les fonctions AR_*(). Cette erreur est
définie par la variable globale AR_ERRNO.

&RT buffer statique contenant le message
&SA AR_add(), AR_extract(), AR_list()
─────────────────────────────────────────────────────────────────────── */

char *AR_error()
{
    if(AR_ERRNO < 0 || AR_ERRNO >= sizeof(AR_ERRORS) / sizeof(char *))
	return("");
    return(AR_ERRORS[AR_ERRNO]);
}

/*NH*/
FILE *AR_open(arname, mode)
char    *arname;
int     mode;
{
    FILE    *fd;
    int     new = 0;
    char    buf[40];

    SCR_strip(arname);
#ifdef DOS
    fd = fopen(arname, "rb+");
#else
    fd = fopen(arname, "r+");
#endif
    if(fd == 0 && mode) {
#ifdef DOS
	fd = fopen(arname, "wb+");
#else
	fd = fopen(arname, "w+");
#endif
	new = 1;
	}
    if(fd == 0) {
	AR_ERRNO = AR_ACCESS_ARCH_ERR;
	return(fd);
	}
    setvbuf(fd, 0L, _IOFBF, AR_BUFSIZE);
    if(new == 0) {
	fseek(fd, 0L, 0);
	fread(buf, strlen(AR_MAGIC), 1, fd);
	if(memcmp(buf, AR_MAGIC, strlen(AR_MAGIC))) {
	    AR_ERRNO = AR_ARCH_VERS_ERR;
	    fclose(fd);
	    return((FILE *)0);
	    }
	}
    else
	fwrite(AR_MAGIC, strlen(AR_MAGIC), 1, fd);
    return(fd);
}

/*NH*/
AR_readnext(fd, arl)
FILE    *fd;
ARLIST  *arl;
{
    char    trsize[10], tpsize[10], tdate[10];
    long    atol();

    if(fread(trsize, 8, 1, fd) < 1 ||
       fread(tpsize, 8, 1, fd) < 1 ||
       fread(tdate,  8, 1, fd) < 1 ||
       fread(arl->ar_name, 50, 1, fd) < 1) return(-1);

    arl->ar_name[50] = 0;
    SCR_strip(arl->ar_name);
    arl->ar_psize = atol(tpsize);
    arl->ar_rsize = atol(trsize);
    arl->ar_date  = atol(tdate);
    return(0);
}
