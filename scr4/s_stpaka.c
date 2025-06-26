#include "scr.h"
#include <stdlib.h>

extern unsigned char  AR_CH;

/* ====================================================================
Ajoute n fichiers (contenus dans le tableau files) dans le fichier
d'archive arname.

&RT 0 en cas de succès, -1 en cas d'erreur. La variable AR_ERRNO
    indique le code d'erreur et la fonction AR_error() fournit
    un texte.
&EX
    files = SCR_vtoms("file1,file2,file3", ",");
    AR_add("back.ar", files, SCR_tbl_size(files));


    AR_add("back.ar", argv + 1, argc - 1);
&TX
&SA AR_error(), AR_extract(), AR_list()
======================================================================= */

AR_add(arname, files, n)
char    *arname;
char    **files;
int     n;
{
    FILE    *fd, *AR_open();
    int     i;

    AR_ERRNO = 0;
    fd = AR_open(arname, 1);
    if(fd == 0) return(-1);

    for(i = 0 ; i < n ; i++) {
	fseek(fd, 0L, 2);
	if(AR_add_1(fd, files[i])) {
	    fclose(fd);
	    return(-1);
	    }
	}

    fclose(fd);
    return(0);
}
/*NH*/
AR_add_1(fdar, filename)
FILE    *fdar;
char    *filename;
{
    long    rsize, psize, pos;
    FILE    *fdin;

#ifdef DOS
    fdin = fopen(filename, "rb");
#else
    fdin = fopen(filename, "r");
#endif
    if(fdin == 0) {
	AR_ERRNO = AR_ACCESS_FILE_ERR;
	return(-1);
	}
    setvbuf(fdin, 0L, _IOFBF, AR_BUFSIZE);

    pos = ftell(fdar);
    fprintf(fdar, "%8.8ld%8.8ld%8.8ld%-50.50s", 0L, 0L, 0L, filename);
    AR_dump(fdin, fdar, &rsize, &psize);
    fseek(fdar, pos, 0);
    fprintf(fdar, "%8.8ld%8.8ld%8.8ld%-50.50s", rsize, psize,
					    SCR_current_date(), filename);
    fclose(fdin);
    return(0);
}

/*NH*/
AR_series(fd, ch)
FILE    *fd;
int     ch;
{
    int     chn, nb = 1;

    while(1) {
	if(nb > 254) return(nb);
	chn = getc(fd);
	if(feof(fd)) return(nb);
	if(chn != ch) {
	    ungetc(chn, fd);
	    return(nb);
	    }
	nb++;
	}
}
/*NH*/
AR_dump(fd, fd2, rsize, psize)
FILE    *fd, *fd2;
long    *psize, *rsize;
{
    int     ch, suite, i;

    *rsize = *psize = 0;
    for(;;) {
	ch = getc(fd);
	if(feof(fd)) break;
	suite = AR_series(fd, ch);
	*rsize += suite;
	if(suite > 3 || ch == AR_CH) {
	    putc(AR_CH, fd2);
	    putc(ch, fd2);
	    putc(suite, fd2);
	    *psize += 3;
	    }
	else {
	    for(i = 0 ; i < suite ; i++) putc(ch, fd2);
	    *psize += suite;
	    }
	}
    return(0);
}






