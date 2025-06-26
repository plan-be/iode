#include "scr.h"
#include <stdlib.h>

extern unsigned char  AR_CH;

/* ====================================================================
Extrait une liste de fichier (files) au nombre de n d'un fichier arname
créé à l'aide de AR_add(). Le pointeur vers fonction fn permet de
passer à une fonction utilisateur les informations sur le déroulement
du processus.

fn est appelée à chaque caractère lu dans un fichier. Sa syntaxe est :

&CO
    fn(char *filename, long nb_read, long nb_total)
&TX

Le fichier d'archive est balayé jusqu'à la fin. Si aucun fichier n'est
spécifié, tous les fichiers de l'archive sont extraits. Si un fichier
se présente plusieurs fois, c'est le dernier qui est restauré.

&RT 0 en cas de succès, -1 en cas d'erreur. La variable AR_ERRNO
    indique le code d'erreur et la fonction AR_error() fournit
    un texte.
&EX
    AR_extract("myarch.ar", NULL, 0, NULL);

    files = SCR_vtoms("file1,file2,file3", ",");
    AR_extract("myarch.ar", files, SCR_tbl_size(files), NULL);


    myfn(char *filename, long cur, long tot)
    {
	if(cur == 0) {
	    printf("Extracting %s", filename);
	    return;
	    }
	if(tot == cur) {
	    printf("End\n");
	    return;
	    }
    }
    AR_extract("myarch.ar", NULL, 0, myfn);
&TX
&SA AR_add(), AR_error(), AR_list()
======================================================================= */
AR_extract(arname, files, n, fn)
char    *arname;
char    **files;
int     n;
int     (*fn)();
{
    FILE    *fd, *AR_open();
    int     i;
    ARLIST  arl;

    AR_ERRNO = 0;
    fd = AR_open(arname, 0);
    if(fd == 0) return(-1);

    while(1) {
	if(AR_readnext(fd, &arl)) break;
	for(i = 0 ; i < n ; i++)
	    if(strcmp(files[i], arl.ar_name) == 0) break;
	if(n == 0 || i < n) {
	    if(AR_uncat(fd, arl.ar_name, arl.ar_psize, fn)) break;
	    }
	else
	    fseek(fd, arl.ar_psize, 1);
	}
    fclose(fd);
    return(0);
}

/*NH*/
#ifdef SCRPROTO
/*NH*/
AR_uncat(
FILE    *fdar,
char    *filename,
long    lg,
int     (*fn)(char *, long, long)
)
#else
AR_uncat(fdar, filename, lg, fn)
FILE    *fdar;
char    *filename;
long    lg;
int     (*fn)();
#endif
{
    FILE    *fd2;
    long    i;
    int     ch, ch1, j, nbchars;

#ifdef DOS
    fd2 = fopen(filename, "wb+");
#else
    fd2 = fopen(filename, "w+");
#endif
    if(fd2 == 0) {
	AR_ERRNO = AR_ACCESS_FILE_ERR;
	return(-1);
	}
    setvbuf(fd2, 0L, _IOFBF, AR_BUFSIZE); /* JMP 03-12-97 */

    for(i = 0; i < lg; i++) {
	if(fn) (*fn)(filename, i, lg);
	ch = getc(fdar);
	if(feof(fdar)) {
	    AR_ERRNO = AR_EOF_ERR;
	    fclose(fd2);
	    return(-1);
	    }

	if(ch == AR_CH) {
	    ch1     = getc(fdar);
	    nbchars = getc(fdar);
	    i += 2;
	    for(j = 0 ; j < nbchars ; j++) putc(ch1, fd2);
	    }
	else
	    putc(ch, fd2);
	}
    if(fn) (*fn)(filename, lg, lg);
    fclose(fd2);
    return(0);
}

