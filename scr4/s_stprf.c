#define SCRSUPPROTO
#include <stdio.h>
#include <s_strs.h>

/* ======================================================================
Crée le fichier filename et y stocke le string construit avec
fprintf(a1, a2, ...).

&EX
    SCR_printf_file("date", "%ld", SCR_current_date());
&TX
&RT -1 en cas d'erreur, 0 sinon.
------------------------------------------------------------------------- */

SCR_printf_file(filename, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
char    *filename, *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9, *a10;
{
    FILE    *fd;

    fd = fopen(filename, "w+");
    if(fd == 0) return(-1);
    fprintf(fd, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
    fclose(fd);
    return(0);
}

/* ======================================================================
Crée le fichier filename et y stocke le tableau de strings tbl.
Ce tableau est terminé par un pointeur nul.

Si le tableau ne contient aucun string, le fichier n'est pas créé et la valeur
de retour est 0;

&EN filename : fichier résultat
&EN tbl : tableau de pointeurs vers des strings
&EN fmt : format d'impression

&EX
    tbl = SCR_vtom("toto,tata,titi", ',');
    SCR_printf_tbl("myfile", tbl, "'%s'\n");
&TX
&RT -1 en cas d'erreur, 0 sinon (même si le fichier n'est pas créé).
------------------------------------------------------------------------- */

SCR_printf_tbl(filename, tbl, fmt)
char    *filename;
unsigned char **tbl;
char    *fmt;
{
    FILE    *fd;
    int     i;

    if(SCR_tbl_size(tbl) == 0) return(0);
    fd = fopen(filename, "w+");
    if(fd == 0) return(-1);
    for(i = 0 ; tbl[i] ; i++) {
	fprintf(fd, fmt, tbl[i]);
	}
    fclose(fd);
    return(0);
}



/* ======================================================================
Copie  le contenu du fichier filename dans le stream fd.

&EN fd : FILE * (output)
&EN filename : fichier source
&RT -1 en cas d'erreur, 0 sinon (même si le fichier n'est pas créé).
------------------------------------------------------------------------- */

SCR_dumpf_file(fd, filename)
FILE    *fd;
char    *filename;
{
    FILE    *fdi = fopen(filename, "r");
    int     ch;

    if(fdi == 0) return(-1);

    while(1) {
	ch = getc(fdi);
	if(ch == EOF) break;
	putc(ch, fd);
	}
    fclose(fdi);
    return(0);
}

