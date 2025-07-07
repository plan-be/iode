#define SWAP

#ifdef SWAP
#include "s_swap.h"
#endif

#include "s_lst.h"
#include "s_strs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char   *LST_BUF = 0;
//unsigned int    LST_BUF_LEN = 0;             // JMP 30/11/2022
int    LST_BUF_LEN = 0;             // JMP 30/11/2022

/*NH*/
LST_set_buf(ch, pos)
int     ch, pos;
{
    unsigned char   *buf;

    if(pos >= LST_BUF_LEN) {
	//buf = (unsigned char *)malloc(LST_BUF_LEN + 1024);
	buf = (unsigned char *)SCR_malloc(LST_BUF_LEN + 1024); /* JMP 22-08-2012 */
	if(buf == 0) return(-1);
	if(LST_BUF_LEN) {
	    memcpy(buf, LST_BUF, LST_BUF_LEN);
	    //free(LST_BUF);
	    SCR_free(LST_BUF); /* JMP 22-08-2012 */
	    }
	LST_BUF = buf;
	LST_BUF_LEN += 1024;
	}
    LST_BUF[pos] = ch;
    return(0);
}

/*NH*/

#ifdef SCRPROTO
/*NH*/
LST *LST_read_in(
int             (*fn_next)(FILE *),
FILE            *fd,
unsigned char   *seps
)
#else
LST *LST_read_in(fn_next, fd, seps)
int             (*fn_next)();
FILE            *fd;
unsigned char   *seps;
#endif

{
    LST             *lst = 0;
    int             len = 0, ch, i;

#ifdef SWAP
    SWHDL   swptr;

    lst = LST_create(100, sizeof(SWHDL));
#else
    unsigned char   *ptr;

    lst = LST_create(100, sizeof(char *));
#endif

    if(lst == 0) return(lst);
    while(1) {
	ch = (*fn_next)(fd);
	for(i = 0 ; seps[i] ; i++) if(ch == seps[i]) break;
	if(seps[i] || ch == EOF) {
	    LST_set_buf(0, len);
	    SCR_strip(LST_BUF);     /* JMP 09-03-92 */
	    len = (int)strlen(LST_BUF);  /* JMP 09-03-92 */
	    LST_add_1(lst);
#ifdef SWAP
	    swptr = 0;
	    if(len > 0) {
		swptr = SW_alloc(len + 1);
		if(swptr) strcpy(SW_getptr(swptr), LST_BUF);
		}
	    LST_set(lst, lst->nb_els - 1, &swptr);
#else
	    ptr = 0;
	    if(len > 0) {
		//ptr = malloc(len + 1);
		ptr = SCR_malloc(len + 1); /* JMP 22-08-2012 */
		if(ptr) strcpy(ptr, LST_BUF);
		}
	    LST_set(lst, lst->nb_els - 1, &ptr);
#endif
	    if(ch == EOF) {
		if(LST_BUF) {
		    //free(LST_BUF);
		    SCR_free(LST_BUF); /* JMP 22-08-2012 */
		    LST_BUF_LEN = 0;
		    }
		return(lst);
		}
	    len = 0;
	    }

	else if(ch == '\t') {
	    for(i = len ; i < len + 8 - len%8 ; i++)
		LST_set_buf(' ', i);
	    len = i;                /* JMP 03/93 */
	    }

	else {
	    LST_set_buf(ch, len);
	    len++;
	    }
	}
}

int     LST_POS;

/*NH*/
int LST_nextch(fdvec)
FILE    *fdvec;
{
    int             ch;
    unsigned char   *vec = (unsigned char *)fdvec;

    if(vec != 0) {
	ch = vec[LST_POS++];
	if(ch) return(ch);
	}
    return(EOF);
}


/* ======================================================================
Transforme un vecteur de texte en une liste de strings alloués. Les
sépérateurs de lignes sont contenus dans seps : dès qu'un de ces caractères
est rencontré, une ligne de texte est stockée (et allouée) dans la liste.

&PRO <s_lst.h>
&RT pointeur vers la liste créée
&SA LST_ltov(), LST_free_text(), LST_get(), LST_set()
------------------------------------------------------------------------- */

#ifdef __cplusplus
LST *LST_vtol(
unsigned char   *vec,
unsigned char   *seps
)
#else
LST *LST_vtol(vec, seps)
unsigned char   *vec, *seps;
#endif

{
    LST_POS = 0;
    return(LST_read_in(LST_nextch, (FILE *)vec, seps));
}

/* ======================================================================
Transforme une liste lst de strings (terminés par des 0) en un seul string,
en séparant les strings originaux par seps.

&PRO <s_lst.h>
&RT pointeur vers le string créé et alloué dans la fonction
&SA LST_vtol(), LST_free_text(), LST_get(), LST_set()
------------------------------------------------------------------------- */

unsigned char *LST_ltov(lst, seps)
LST             *lst;
unsigned char   *seps;
{
    unsigned char   *ptr, *vec = 0;
    long            i;
    int             lg = 0, seps_lg = (int)strlen(seps), len;
#ifdef SWAP
    SWHDL           swptr;
#endif

    if(lst) {
	for(i = 0 ; i < lst->nb_els ; i++) {
	    if(i != 0) lg += seps_lg;
#ifdef SWAP
	    swptr = *(SWHDL *) LST_get(lst, (long)i);
	    if(swptr) ptr = SW_getptr(swptr);
	    else      ptr = 0;
#else
	    ptr = *(unsigned char **) LST_get(lst, (long)i);
#endif
	    if(ptr) lg += (int)strlen(ptr);
	    }
	}

    lg++;
    //vec = (unsigned char *)malloc(lg);
    vec = (unsigned char *) SCR_malloc(lg); /* JMP 22-08-2012 */
    if(lg == 1) {
	vec[0] = 0;
	return(vec);
	}

    for(i = lg = 0 ; i < lst->nb_els ; i++) {
	if(i != 0) {
	    strcpy(vec + lg, seps);
	    lg += seps_lg;
	    }
#ifdef SWAP
	swptr = *(SWHDL *) LST_get(lst, (long)i);
	if(swptr) ptr = SW_getptr(swptr);
	else      ptr = 0;
#else
	ptr = *(unsigned char **) LST_get(lst, (long)i);
#endif
	len = 0;
	if(ptr) {
	    len = (int)strlen(ptr);
	    strcpy(vec + lg, ptr);
	    lg += len;
	    }
	}

    return(vec);
}

/* ======================================================================
Libère une liste de strings alloués en SWAP.
&PRO <s_lst.h>
&EX
    PrintFile(filename)
    char    *filename;
    {
	LST     *lst;
	long    i;

	lst = LST_read_file("myfile");
	if(lst == 0) return;
	for(i = 0 ; i < lst->nb_els ; i++)
	    printf("%ld : %s\n", i, SW_getptr(LST_get(lst, i)));
	LST_free_text(lst);
    }
&TX
&SA LST_read_file(), SW_getptr()
------------------------------------------------------------------------- */
int LST_free_text(lst)
LST     *lst;
{
    long            i;

#ifdef SWAP
    SWHDL           swptr;
#else
    unsigned char   *ptr;    
#endif

    if(lst == 0) return(0);
    for(i = lst->nb_els - 1 ; i >= 0 ; i--) { /* JMP 08-03-92 */
#ifdef SWAP
	    swptr = *(SWHDL *) LST_get(lst, i);
	    if(swptr) SW_free(swptr);
#else
	    ptr = *(unsigned char **) LST_get(lst, (long)i);
	    //if(ptr) free(ptr);
	    if(ptr) SCR_free(ptr); /* JMP 22-08-2012 */
#endif
	}

    LST_free(lst);
    return(0);
}

/* ======================================================================
Lit le fichier filename et retourne une liste de strings alloués en SWAP, à
raison d'un élément de liste par ligne du fichier.
&PRO <s_lst.h>
&EX
    Voir LST_free_text()
&TX
&RT la liste allouée ou 0 si le fichier n'est pas accessible
&SA LST_save_file() LST_free_text()
------------------------------------------------------------------------- */

#ifdef __cplusplus
LST *LST_read_file(
char   *filename
)
#else
LST *LST_read_file(filename)
char   *filename;
#endif

{
    FILE            *fd;
    LST             *lst = 0;
/*    extern int      fgetc(); */

    fd = fopen(filename, "r");
    if(fd == NULL) return(lst);

    lst = LST_read_in(fgetc, fd, (unsigned char *)"\n\f");
    fclose(fd);
    return(lst);
}

/* ======================================================================
Sauve dans le fichier filename la liste de strings lst (strings en SWAP).

&EX
    LST     *lst;

    lst = LST_vtol("Titre,Nom,Prénom", ",");
    LST_save_file("test", lst);
    LST_free_text(lst);
&TX
&RT 0 si le fichier est correctement créé, -1 sinon
&SA LST_read_file()
------------------------------------------------------------------------- */
LST_save_file(filename, lst)
char    *filename;
LST     *lst;
{
    FILE            *fd;
    unsigned char   *ptr;
    long            i;
#ifdef SWAP
    SWHDL           swptr;
#endif

    if(filename == 0 || filename[0] == 0 || lst->nb_els == 0) return(-1);
    fd = fopen(filename, "w+");
    if(fd == NULL) return(-1);

    for(i = 0 ; i < lst->nb_els ; i++) {
#ifdef SWAP
	swptr = *(SWHDL *) LST_get(lst, (long)i);
	if(swptr) ptr = SW_getptr(swptr);
	else      ptr = 0;
#else
	ptr = *(unsigned char **) LST_get(lst, (long)i);
#endif
	if(ptr) LST_fwrite(fd, ptr);
	fputc('\n', fd);
	}

    fclose(fd);
    return(0);
}
/*NH*/
int LST_NO_TABS = 0;

/*NH*/
int LST_fwrite(fd, ptr)
FILE            *fd;
unsigned char   *ptr;
{
    int         i, nbl = 0, flag = LST_NO_TABS;

    for(i = 0 ; ptr[i] ; i++) {
	if(flag || ptr[i] != ' ') {
	    for( ; nbl > 0 ; nbl--) fputc(' ', fd);
	    fputc(ptr[i], fd);
	    flag = 1;
	    }
	else {
	    if((i + 1) % 8 == 0) {
		fputc('\t', fd);
		nbl = 0;
		}
	    else nbl++;
	    }
	}
    for( ; nbl > 0 ; nbl--) fputc(' ', fd);
    return(0);
}



