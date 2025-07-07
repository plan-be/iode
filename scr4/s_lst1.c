#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "s_lst.h"

/* ======================================================================
Crée une structure LST destinée à contenir une liste d'éléments
quelconques de taille el_size.

La liste créée contient 0 élément. Elle est organisée en tableaux,
chacun contenant tbl_size éléments, occupant donc el_size * tbl_size
éléments contigus en mémoire.

La liste peut être désallouée par LST_free().

&EX


&TX
&PRO <s_lst.h>
&RT pointeur vers la structure LST allouée
&SA LST_free(), LST_add(), LST_set(), LST_del(), LST_get(), LST_move()
------------------------------------------------------------------------- */

#ifdef __cplusplus
LST *LST_create(
int     tbl_size,
int     el_size
)
#else
LST *LST_create(tbl_size, el_size)
int     tbl_size, el_size;
#endif

{
    LST     *lst;
    char    *ptr;

    //ptr = (char *)malloc(sizeof(LST) + el_size);
    ptr = (char *)SCR_malloc(sizeof(LST) + el_size); /* JMP 22-08-2012 */
    lst = (LST *)ptr;
    if(lst == 0) return(lst);
    memset(lst, 0, sizeof(LST));
    lst->tmp_el   = ptr + sizeof(LST);
    lst->tbl_size = tbl_size;
    lst->el_size  = el_size;
    return(lst);
}

/* ======================================================================
Désalloue une structure LST.

Tous les sous-tableaux sont désalloués. Les éléments de la liste ne sont
pas traités.

&PRO <s_lst.h>
&SA LST_create(), LST_add(), LST_set(), LST_del(), LST_get(), LST_move()
------------------------------------------------------------------------- */

LST_free(lst)
LST     *lst;
{
    int     i;

    if(lst == 0) return(0);
    if(lst->tbls) {
	for(i = 0 ; i < lst->nb_tbls ; i++) {
	    // if(lst->tbls[i]) free(lst->tbls[i]);
	    if(lst->tbls[i]) SCR_free(lst->tbls[i]); /* JMP 22-08-2012 */
	    }
	//free(lst->tbls);
	SCR_free(lst->tbls); /* JMP 22-08-2012 */
	}
    //free(lst);
    SCR_free(lst);
    return(0);
}

/* ======================================================================
Ajoute un élément à la fin de la liste lst. Cet élément est nul.

&PRO <s_lst.h>
&SA LST_create(), LST_add(), LST_set(), LST_del(), LST_get(), LST_move()
------------------------------------------------------------------------- */

LST_add_1(lst)
LST     *lst;
{
    char    **tbls;

    if(lst->nb_tbls * lst->tbl_size <= lst->nb_els) {
	//tbls = (char **) malloc((lst->nb_tbls + 1) * sizeof(char *));
	tbls = (char **) SCR_malloc((lst->nb_tbls + 1) * sizeof(char *)); /* JMP 22-08-2012 */
	if(tbls == 0) return(-1);
	if(lst->tbls) {
	    memcpy(tbls, lst->tbls, lst->nb_tbls * sizeof(char *));
	    //free(lst->tbls);
	    SCR_free(lst->tbls); /* JMP 22-08-2012 */
	    }
	lst->tbls = tbls;
	//tbls[lst->nb_tbls] = malloc(lst->tbl_size * lst->el_size);
	tbls[lst->nb_tbls] = SCR_malloc(lst->tbl_size * lst->el_size); /* JMP 22-08-2012 */
	if(tbls[lst->nb_tbls] == 0) return(-1);
	lst->nb_tbls++;
	}
    lst->nb_els++;
    return(0);
}

/* ======================================================================
Ajoute n éléments après l'élément after de la liste lst.
Tous les éléments sont initialisés à 0.

Pour ajouter au début de la liste, after peut valoir -1L.

&PRO <s_lst.h>
&SA LST_create(), LST_add1(), LST_set(), LST_del(), LST_get(), LST_move()
------------------------------------------------------------------------- */

LST_add(lst, after, n)
LST     *lst;
long    after, n;
{
    long    i;

    if(after < -1 || n <= 0) return(-1);
    if(after >= lst->nb_els) return(-1);
    for(i = 0 ; i < n ; i++)
	if(LST_add_1(lst)) return(-1);
    for(i = lst->nb_els - 1 ; i > after + n ; i--)
	LST_set(lst, i, LST_get(lst, i - n));
    for(i = 0 ; i < n ; i++)
	memset(LST_get(lst, i + after + 1), 0, lst->el_size);

    return(0);
}

/* ======================================================================
Détruit n éléments à partir de l'élément from.

&PRO <s_lst.h>
&SA LST_create(), LST_add(), LST_set(), LST_get(), LST_move()
------------------------------------------------------------------------- */

LST_del(lst, from, n)
LST     *lst;
long    from, n;
{
    long    i;
    int     nb_tbls;

    if(from + n > lst->nb_els) n = lst->nb_els - from;
    if(from < 0 || n <= 0) return(-1);
    if(from >= lst->nb_els) return(-1);
    for(i = from ; i < lst->nb_els - n ; i++)
	memcpy(LST_get(lst, i), LST_get(lst, i + n), lst->el_size);
    nb_tbls = 1 + (lst->nb_els - n - 1) / lst->tbl_size;
    if(lst->nb_els == n) nb_tbls = 0;
    if(nb_tbls < lst->nb_tbls) {
	for(i = nb_tbls ; i < lst->nb_tbls ; i++) {
	    //free(lst->tbls[i]);
	    SCR_free(lst->tbls[i]); /* JMP 22-08-2012 */
	    }
	lst->nb_tbls = nb_tbls;
	}

    lst->nb_els -= n;
    return(0);
}

/* ======================================================================
Déplace les n éléments à partir de from après l'élémént after.

&PRO <s_lst.h>
&SA LST_create(), LST_add(), LST_set(), LST_del(), LST_get()
------------------------------------------------------------------------- */

LST_move(lst, from, after, n)
LST     *lst;
long    from, after, n;
{
    long    i;

    if(after < -1 || n <= 0) return(-1);
    if(after >= lst->nb_els) return(-1);
    if(from + n > lst->nb_els) return(-1);
    if(after >= from && after < from + n) return(-1);
    LST_add(lst, after, n);
    if(from > after) from += n;
    for(i = 0 ; i < n ; i++)
	LST_set(lst, after + i + 1, LST_get(lst, from + i));
    LST_del(lst, from, n);
    return(0);
}

/* ======================================================================
Retourne un pointeur vers l'élément n de la liste lst.

&PRO <s_lst.h>
&SA LST_create(), LST_add(), LST_set(), LST_del(), LST_move()
------------------------------------------------------------------------- */

void *LST_get(lst, n)
LST     *lst;
long    n;
{
    int     blk_nb, el_nb;

    if(n >= lst->nb_els) return((char *)0);
    blk_nb = n / lst->tbl_size;
    el_nb  = n % lst->tbl_size;
    return(lst->tbls[blk_nb] + el_nb * lst->el_size);
}

/* ======================================================================
Fixe la valeur de l'élément n de la liste lst. ptr est un pointeur vers
cet élément, et non sa valeur. Ainsi, dans le cas d'une liste de char *
par exemple, on aura :

&EX
    char    *txt;

    txt = GetAllocText();
    LST_set(lst, i, &txt);

    ..

    printf("%s\n", *(char **)LST_get(lst, i));
&TX

&PRO <s_lst.h>
&SA LST_create(), LST_add(), LST_get(), LST_del(), LST_move()
------------------------------------------------------------------------- */

LST_set(lst, n, ptr)
LST     *lst;
long    n;
void    *ptr;
{
    char    *el_ptr;

    el_ptr = LST_get(lst, n);
    if(el_ptr == 0) return(-1);
    memcpy(el_ptr, ptr, lst->el_size);
    return(0);
}

/* ======================================================================
Supprime les éléments de fin de liste qui sont nuls : si le dernier de la
liste est nul, le supprime, et ainsi de suite jusqu'à avoir avoir rencontré
un élément non nul.

&PRO <s_lst.h>
&SA LST_create(), LST_add(), LST_get(), LST_del(), LST_move()
------------------------------------------------------------------------- */

LST_strip(lst)
LST     *lst;
{
    long    i;
    int     j;
    char    *ptr;

    for(i = lst->nb_els - 1 ; i >= 0 ; i--) {
	ptr = LST_get(lst, i);
	for(j = 0 ; j < lst->el_size ; j++)
	    if(ptr[j]) break;
	if(j < lst->el_size) break;
	}
    if(i == lst->nb_els - 1) return(0);
    LST_del(lst, i + 1, lst->nb_els - i - 1);
    return(0);
}

/* ======================================================================
Retourne le nombre d'éléments de la liste lst.
&EX
    if(LST_nb(lst) == 0) return;
    LST_save_file("test", lst);
&TX
&RT le nombre d'éléments actuel de la liste
------------------------------------------------------------------------- */
LST_nb(lst)
LST *lst;
{
    return(lst->nb_els);
}
