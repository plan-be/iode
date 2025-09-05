#undef ALLOCDOCNULL

#include "s_strs.h"
#include <stdio.h>
#include <stdlib.h>

// WARNING: using min and max as macro names makes conflicts with the GNU implementation 
//          of the C++ standard library
#ifndef _min_
    #define _min_(x, y)     (((x) < (y)) ? (x) : (y))
#endif

#define SCR_CHUNCK      5
#define SCR_ALLC_MAGIC  1234567890L

/*extern char *malloc();
extern char *realloc();
Replaced by stdlib JMP 04-01-98 */

long        SCR_TOTAL_ALLOC = 0;            // Total remaining memory allocation in bytes
int         SCR_ALLOC_DOC = 0;              // Log the remaining allocations 
char        *SCR_ALLOC_DOC_LOGFILE = 0;     // Log file for remaining allocations 

/* =========================================================================
ALLOC DOC : maintain information on the allocations made by
SCR_malloc, SCR_realloc and SCR_free.

If SCR_ALLOC_DOC == 1 :

  - add a struct ALLOCDOC in front of each allocated pointer
  - save in SCR_ALLOCDOC_LAST the last allocated struct
========================================================================= */

ALLOCDOC *SCR_ALLOCDOC_LAST = 0;

void AllocDocNew(ALLOCDOC *ad)
{
    ad->next = ad->prev = 0;
    // ad->freed = 0;
    if(SCR_ALLOCDOC_LAST == 0)
        SCR_ALLOCDOC_LAST = ad;
    else {
        ad->prev = SCR_ALLOCDOC_LAST;
        SCR_ALLOCDOC_LAST->next = ad;
        SCR_ALLOCDOC_LAST = ad;
    }
}
/*
AllocDocFile(char *ptr, char *file, int line)
{
    ALLOCDOC    *ad;

    if(ptr == 0) return(ptr);
    if(SCR_ALLOC_DOC == 0) return(ptr);
    ad = ptr - sizeof(ALLOCDOC);
    ad->file = file;
    ad->line = line;

#ifdef ALLOCDOCNULL
     if(file == 0) {
	printf("File 0");
	}
#endif

    return(ptr);
}
*/
AllocDocFree(ALLOCDOC *ad)
{
    if(ad == 0) return(0);

/* BUg ???
    // Check non dejé freed 
    ad->freed++; // JMP 15/2/2013    
    if(ad->freed > 1) {
        printf("Double free !!! %d x !\n", ad->freed); 
        if(ad->file)
            printf("%s[%d] : %ld = '%s'\n", ad->file, ad->line, ad->size, (char *)(ad + 1));
        else
            printf("???[???] : %ld = '%s'\n", ad->size, (char *)(ad + 1));
        return;
        }
*/            

    // A la fin
    if(ad->next == 0) {
        SCR_ALLOCDOC_LAST = ad->prev;
        if(SCR_ALLOCDOC_LAST) 
            SCR_ALLOCDOC_LAST->next = 0;
    }

    // Au début (mais pas à la fin déjé géré)
    else if(ad->prev == 0) {
        ad->next->prev = 0;
    }

    // Au milieu
    else {
        ad->prev->next = ad->next;
        ad->next->prev = ad->prev;
    }
    if(SCR_ALLOCDOC_LAST &&  SCR_ALLOCDOC_LAST->prev == SCR_ALLOCDOC_LAST)
        printf("Erreur : référence cyclique\n");
    
    return(0);
}

/***************************************************************************
Rapport des allocations non libérées dans le stdout ou dans le fichier dont le
nom se trouve dans la variable SCR_ALLOC_DOC_LOGFILE.

&EN Si SCR_ALLOC_DOC == 0, il n'y a pas d'écriture.
&EN Si SCR_ALLOC_DOC_LOGFILE == 0, rapport dans le stdout

Le rapport contient la liste des allocations non libérées et le total de la
mémoire non libérée.

Si une allocation ne référence pas de fichier, le nom du fichier est remplacé par des ???.
***************************************************************************/

int AllocDocLoop()
{
    ALLOCDOC        *ad = SCR_ALLOCDOC_LAST;
    unsigned long   tot = 0L, count = 0, maxprint = 1000;
    FILE            *fd;

    if(SCR_ALLOC_DOC == 0) return(0);
    // Output in file or stdout ?
    if(SCR_ALLOC_DOC_LOGFILE) {
        fd = fopen(SCR_ALLOC_DOC_LOGFILE, "w+");
        if(fd == 0) return(-1);
    }
    else
        fd = stdout;

    fprintf(fd, "Allocation/Deallocation report\n");
    fprintf(fd, "==============================\n");
    fprintf(fd, "Not freed : ");
    while(ad && count < maxprint) {
        if(ad->file)
            fprintf(fd, "%s[%d] : %ld = '%s'\n", ad->file, ad->line, ad->size, (char *)(ad + 1));
        else
            fprintf(fd, "???[???] : %ld = '%s'\n", ad->size, (char *)(ad + 1));
        tot += ad->size;
        if(ad == ad->prev) {
            fprintf(fd, "\n!!!!!! Infinite loop on on allocations.\n");
            break;
            }
        ad = ad->prev;
        count++;
    }
    if(ad)
        fprintf(fd, "\n.... Report stopped after %d blocks.\n", maxprint);

    fprintf(fd, "\nTotal : %lu\n", tot);
    if(SCR_ALLOC_DOC_LOGFILE) fclose(fd);

    return(0);
}


char        *SCR_ALLOC_BUF;
int         SCR_ALLOC_BUF_SIZE = 0;



/* =====================================================================
Alloue un espace de lg bytes en mémoire et fixe la valeur de chacun des
bytes à 0.

Si l'espace demandé est indisponible et que panic est non nul, la fonction
appelle la fonction SCR_panic() qui par défaut affiche une message d'erreur
et quitte le programme avec un code retour 2.

Si panic est nul, SCR_malloc_chk() retourne un pointeur nul et n'appelle
pas la fonction SCR_panic().

SCR_panic() peut étre replacée par une fonction utilisateur.

La fonction malloc() est utilisée pour les allocations.

&EX
    txt = SCR_malloc_chk(20, 0);
    if(txt == 0) return(-1);
&TX
&SA SCR_malloc(), SCR_realloc(), SCR_free(), SCR_palloc(), SCR_panic()
========================================================================*/

char *SCR_malloc_chk(unsigned int lg, int panic)
{
    char    *ptr;

    if(lg == 0) return((char *)0);
    //ptr = (char *)malloc(lg + SCR_ALLOC_DOC * sizeof(long) * 2);
    ptr = (char *)malloc(lg + SCR_ALLOC_DOC * sizeof(ALLOCDOC));
    if(ptr == 0) {
        if(panic) SCR_panic();
        return(ptr);
    }
    if(SCR_ALLOC_DOC) {
        ALLOCDOC *ad = (ALLOCDOC *) ptr;
        ad->size = lg;
        ad->file = 0;
        AllocDocNew(ad);
        ptr += sizeof(ALLOCDOC);
    }

    memset(ptr, 0, lg);
    SCR_TOTAL_ALLOC += lg;

    return(ptr);
}

/*NH*/
char *SCR_malloc_orig(unsigned int lg)
{
    return(SCR_malloc_chk(lg, 1));
}


/* =====================================================================
Alloue un espace de lg bytes en mémoire et fixe la valeur de chacun des
bytes à 0.

Si l'espace demandé est indisponible, la fonction appelle la fonction
SCR_panic() qui par défaut affiche une message d'erreur et quitte le
programme avec un code retour 2.

SCR_panic() peut étre replacée par une fonction utilisateur.

La fonction malloc() est utilisée pour les allocations.

&EX
    txt = SCR_malloc(20);
&TX
&SA SCR_malloc_chk(), SCR_realloc(), SCR_free(), SCR_palloc(), SCR_panic()
========================================================================*/

char *SCR_malloc_doc(unsigned int lg, char *file, int line)
{
    char        *ptr = SCR_malloc_chk(lg, 1);
    ALLOCDOC    *ad;

    if(ptr == 0) return(ptr);
    if(SCR_ALLOC_DOC == 0) return(ptr);
    ad = (ALLOCDOC *)(ptr - sizeof(ALLOCDOC));

#ifdef ALLOCDOCNULL
    if(file == 0) {
        printf("File 0");
    }
#endif

    ad->file = file;
    ad->line = line;
    return(ptr);
}


/* =====================================================================
Réalloue un espace précédemment alloué en mémoire et fixe la valeur de
chacun des bytes ajoutés à 0.

Si l'espace demandé est indisponible et que panic est non nul, la fonction
appelle la fonction SCR_panic() qui par défaut affiche une message d'erreur
et quitte le programme avec un code retour 2.

Si panic est nul, SCR_realloc_chk() retourne un pointeur nul et n'appelle
pas la fonction SCR_panic().

SCR_panic() peut étre replacée par une fonction utilisateur.

La fonction demande 4 paramétres :

&EN l'ancien pointeur
&EN la taille unitaire d'un élément (sizeof(int) par exemple)
&EN l'ancien nombre d'éléments alloués
&EN le nouveau nombre d'éléments alloués

Si l'ancien pointeur est nul, SCR_malloc() est appelé.

Dans les autres cas, l'actuelle valeur est d'abord copiée dans un buffer
global, plus libérée. Ensuite, la nouvelle taille est allouée et remplie
de 0, puis le buffer est copié dans le nouvel espace alloué.

Cette faéon de procéder permet de gagner une place énorme lorsque des
réallocations successives d'un méme pointeur ont lieu.

Si le nouvel espace est plus petit que l'ancien, le méme processus est
effectué, ce qui assure une récupération réelle de l'espace.

La fonction standard realloc() n'est pas utilisée.

&EX
    txt = SCR_realloc(txt, sizeof(char *), 20, 25);
&TX
&SA SCR_malloc(), SCR_free(), SCR_panic()
========================================================================*/

char *SCR_realloc_chk(void* old_ptr, unsigned int el_size, unsigned int old_count, unsigned int new_count, int panic)
{
    char    *ptr;

    old_count *= el_size;
    new_count *= el_size;
    if(old_count == new_count) return(old_ptr);
    if(old_ptr == 0 || old_count <= 0)
        return(SCR_malloc_chk(new_count, panic));

    /* IF NOT ENOUGH SPACE TO SAVE OLD VALUES, REALLOC SCR_ALLOC_BUF */
    if(SCR_ALLOC_BUF_SIZE < old_count) {
        if(SCR_ALLOC_BUF_SIZE != 0) SCR_free(SCR_ALLOC_BUF);
        SCR_ALLOC_BUF_SIZE = 1024 * (1 + (int)((old_count - 1) / 1024));
        SCR_ALLOC_BUF = SCR_malloc_chk(SCR_ALLOC_BUF_SIZE, panic); /* JMP 06-02-97 */
        if(SCR_ALLOC_BUF == 0) return((char *)0);
    }

    /* SAVE OLD VALUES IN BUFFER */
    memcpy(SCR_ALLOC_BUF, old_ptr, _min_(old_count, new_count));

    /* FREE OLD PTR */
    SCR_free(old_ptr);

    /* ALLOCATES NEW SPACE */
    ptr = SCR_malloc_chk(new_count, panic); /* JMP 06-02-97 */
    if(ptr == 0) return(ptr);

    /* SAVE VALUES */
    memcpy(ptr, SCR_ALLOC_BUF, _min_(old_count, new_count));
    return(ptr);
}

/*NH*/
char *SCR_realloc_orig(void* old_ptr, unsigned int el_size, unsigned int old_count, unsigned int new_count)
{
    //return(SCR_realloc_orig(old_ptr, el_size, old_count, new_count));
    return(SCR_realloc_chk(old_ptr, el_size, old_count, new_count, 1)); // JMP 30/10/2022
}

/* =====================================================================
Réalloue un espace précédemment alloué en mémoire et fixe la valeur de
chacun des bytes ajoutés à 0.

Si l'espace demandé est indisponible, la fonction appelle la fonction
SCR_panic() qui par défaut affiche une message d'erreur et quitte le
programme avec un code retour 2.

SCR_panic() peut étre replacée par une fonction utilisateur.

La fonction demande 4 paramétres :

&EN l'ancien pointeur
&EN la taille unitaire d'un élément (sizeof(int) par exemple)
&EN l'ancien nombre d'éléments alloués
&EN le nouveau nombre d'éléments alloués

Si l'ancien pointeur est nul, SCR_malloc() est appelé.

Dans les autres cas, l'actuelle valeur est d'abord copiée dans un buffer
global, plus libérée. Ensuite, la nouvelle taille est allouée et remplie
de 0, puis le buffer est copié dans le nouvel espace alloué.

Cette faéon de procéder permet de gagner une place énorme lorsque des
réallocations successives d'un méme pointeur ont lieu.

Si le nouvel espace est plus petit que l'ancien, le méme processus est
effectué, ce qui assure une récupération réelle de l'espace.

La fonction standard realloc() n'est pas utilisée.

&EX
    txt = SCR_realloc(txt, sizeof(char *), 20, 25);
&TX
&SA SCR_malloc(), SCR_free(), SCR_panic()
========================================================================*/


char *SCR_realloc_doc(void* old_ptr, unsigned int el_size, unsigned int old_count, unsigned int new_count, char* file, int line)
{
    char        *ptr = SCR_realloc_chk(old_ptr, el_size, old_count, new_count, 1);
    ALLOCDOC    *ad;

    if(ptr == 0) return(ptr);
    if(SCR_ALLOC_DOC == 0) return(ptr);
    ad = (ALLOCDOC *)(ptr - sizeof(ALLOCDOC));
    /*
        if(file == 0) {
    	printf("File 0");
    	}
    */
    /*    if(el_size * new_count == 4) {
    	Debug("Realloc 4");
        }
    */
    ad->file = file;
    ad->line = line;
    return(ptr);
}


/*NH*/
SCR_alloc_chunck(int nb)
{
    int     i;

    if(nb <= 0) return(0);
    i = SCR_CHUNCK * (int)(1 + (nb - 1) / SCR_CHUNCK);
    return(i);
}

/* =====================================================================
Libére l'espace alloué pour un pointeur donné. Si le pointeur est
nul, la fonction n'a pas d'effet.

La fonction de librairie free() est utilisée pour cette fonction.

&EX
    txt = SCR_malloc(20);
    ...
    SCR_free(txt);
    txt = NULL;
    SCR_free(txt);    ---> pas d'effet car txt == NULL

&TX
&SA SCR_realloc(), SCR_malloc()
========================================================================*/


int SCR_free(void* ptr)
{
    char    *ptr2 = ptr;

    if(ptr2 == 0) return(0);

    if(SCR_ALLOC_DOC) {
        ALLOCDOC *ad = (ALLOCDOC *)(ptr2 - sizeof(ALLOCDOC));
        SCR_TOTAL_ALLOC -= ad->size;
        AllocDocFree(ad);
        free(ad);
    }
    else free(ptr);

    return(0);
}

#define OBJ_MAX_ALLOC   3072

/* =============================================================
Cette fonction alloue des buffers successifs de 3K. A chaque appel,
elle consomme une partie du dernier buffer alloué et retourne l'adresse
du premier byte disponible dans ce buffer. Il ne s'agit donc pas d'une
allocation au sens commun du terme. Lorsque le dernier buffer est plein
ou ne suffit plus pour la nouvelle allocation demandée, un nouveau
buffer est alloué.

L'intérét de cette fonction réside dans le fait qu'elle évite le
morcellement de la mémoire et la perte de 8 bytes concédée normalement
pour chaque allocation par malloc() ou SCR_malloc(). Elle est
notamment utilisée pour charger en mémoire les objets compilés dans un
fichier .scr.

Son inconvénient est qu'elle ne permet pas de libérer l'espace alloué.

Pour éviter les problémes d'alignement, chaque pointeur retourné est
aligné sur a bytes. Pour des textes, a vaut 1. Pour des int par contre,
il faut donner à a la valeur
&CO
	sizeof(int)
&TX

Si l'espace demandé est indisponible, la fonction appelle la fonction
SCR_panic() qui par défaut affiche une message d'erreur et quitte le
programme avec un code retour 2.

SCR_panic() peut étre replacée par une fonction utilisateur.

&EX
    for(i = 0 ; i < 30 ; i++)
	txt[i] = SCR_palloc(81);   ---> une seule allocation
					au lieu de 30

&TX
&SA SCR_realloc(), SCR_malloc(), SCR_palloc()

============================================================== */

char *SCR_palloca(unsigned int len, unsigned int a)
{
    static  char    *all = 0;
    static  int     pos = 0;
    char    *text;

    if(len > OBJ_MAX_ALLOC) return(SCR_malloc(len));
    if(pos % a) pos += a - pos % a;
    if(all == 0 || pos + len > OBJ_MAX_ALLOC) {
        /*        if(all) all = realloc(all, pos); */
        all = SCR_malloc(OBJ_MAX_ALLOC);
        pos = 0;
    }
    text = all + pos;
    pos += len;
    return(text);
}

/* =============================================================
Cette fonction appelle la fonction SCR_palloca(). Elle fonction alloue des
buffers successifs de 3K. A chaque appel, elle consomme une partie du
dernier buffer alloué et retourne l'adresse du premier byte disponible dans
ce buffer. Il ne s'agit donc pas d'une allocation au sens commun du terme.
Lorsque le dernier buffer est plein ou ne suffit plus pour la nouvelle
allocation demandée, un nouveau buffer est alloué.

L'intérét de cette fonction réside dans le fait qu'elle évite le
morcellement de la mémoire et la perte de 8 bytes concédée normalement
pour chaque allocation par malloc() ou SCR_malloc(). Elle est
notamment utilisée pour charger en mémoire les objets compilés dans un
fichier .scr.

Son inconvénient est qu'elle ne permet pas de libérer l'espace alloué.

Pour éviter les problémes d'alignement, chaque pointeur retourné est
aligné sur 8 bytes.

Si l'espace demandé est indisponible, la fonction appelle la fonction
SCR_panic() qui par défaut affiche une message d'erreur et quitte le
programme avec un code retour 2.

SCR_panic() peut étre replacée par une fonction utilisateur.

&EX
    for(i = 0 ; i < 30 ; i++)
	txt[i] = SCR_palloc(81);   ---> une seule allocation
					au lieu de 30

&TX
&SA SCR_realloc(), SCR_malloc(), SCR_palloca()

============================================================== */

char *SCR_palloc(unsigned int len)
{
    return(SCR_palloca(len, 8));
}


/**** ANNULE
char *SCR_malloc_0(lg)
unsigned int     lg;
{
#ifdef ALLOCDOCNULL
    printf("File 0");
#endif
    return(SCR_malloc_chk(lg, 1));
}
******/

/****
char *SCR_realloc_0(old_ptr, el_size, old_count, new_count)
void            *old_ptr;
unsigned int    el_size,
	 old_count,
	 new_count;
{
#ifdef ALLOCDOCNULL
    printf("File 0");
#endif
    return(SCR_realloc_chk(old_ptr, el_size, old_count, new_count, 1));
}
****/
