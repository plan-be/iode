#include "scr.h"

/* ====================================================================
Copie la database is2 dans is1, record par record, sur la longueur la
plus petite des deux fichiers. Le record de is1 est initialement fixé à
0 pour éviter que des résidus de buffer se placent dans les records.

Le paramètre stop décide si le processus s'arrête (1) ou non (0) en cas
d'erreur. Un message est affiché avant l'arrêt éventuel.

Le paramètre disp permet d'afficher (1) ou non (0) un compteur avec le
nombre de records copiés.

Le pointeur de fonction fn permet de filtrer les records avant de les
écrire. La fonction fn est appelée comme suit :
&CO
    fn(is1, is2);
&TX
Si fn retourne 0, le record est copié. Sinon, il est sauté.

La lecture de is2 se fait dans l'ordre physique du fichier.

&TI Utilisation
───────────

Cette fonction est utilisée pour agrandir ou diminuer la taille des
records d'un ISAM, ou encore pour recréer une database contenant des
index DUP qui ne devraient pas s'y trouver (après une modification des
INDEX par exemple).

&RT Nombre de records copiés correctement.
&EX
    long nb_recs;

    nb_recs = IS_copy_fn(is_newprod, is_prod, 0, 0, select);
    printf("%ld records de prod copiés dans newprod\n", nb_recs);
&TX
&SA IS_copy_fn(), IS_create_dbase()
=======================================================================*/


long IS_copy_fn(is1, is2, stop, disp, fn)
ISAM    *is1, *is2;
int     stop, disp;
int     (*fn)();
{
    int     i, lg;
    long    nb_read = 0, nb_found = 0;

    if(IS_open(is1, SCR_UPDATE, SCR_NO_LOCK) != 0) {
	PG_display_error(IS_error_text());
	return(-1);
    }
    if(IS_open(is2, SCR_READ, SCR_NO_LOCK) != 0) {
	PG_display_error(IS_error_text());
	IS_close(is1);
	return(-1);
    }
    if(disp) SC_init_display();

    if(IS_search(is2, 0, SCR_DB, (char *)0) != 0) {
	PG_display_error(IS_error_text());
	goto ended;
    }

    IS_empty_rec(is1);                              /* JMP 17-06-91 */
    lg = _min_(is1->is_rec_len, is2->is_rec_len); /* JMP 17-06-91 */

    while(1) {
	nb_read++;
	if(disp) SC_incr_display(nb_read, nb_found);
	if(fn == 0 || (*fn)(is1, is2) == 0) {
	    memcpy(is1->is_rec, is2->is_rec, lg);
	    if(IS_write(is1)) {
		if(stop) {
		    PG_display_error(IS_error_text());
		    goto ended;
		    }
		}
	    else {
		nb_found++;
		if(disp) SC_incr_display(nb_read, nb_found);
	    }
	}
	if(IS_next(is2)) break;
    }
ended:
    if(disp) SC_end_display();
    IS_close(is1);
    IS_close(is2);

    return(nb_found);
}

/* ====================================================================
Copie la database is2 dans is1, record par record, sur la longueur la
plus petite des deux fichiers. Le record de is1 est initialement fixé à
0 pour éviter que des résidus de buffer se placent dans les records.

Le paramètre stop décide si le processus s'arrête (1) ou non (0) en cas
d'erreur. Un message est affiché avant l'arrêt éventuel.

Le paramètre disp permet d'afficher (1) ou non (0) un compteur avec le
nombre de records copiés.

&TI Utilisation
───────────
Cette fonction est utilisée pour agrandir ou diminuer la taille des
records d'un ISAM, ou encore pour recréer une database contenant des
index DUP qui ne devraient pas s'y trouver (après une modification des
INDEX par exemple).

&RT  Nombre de records copiés correctement.
&EX
    long nb_recs;

    nb_recs = IS_copy(is_newprod, is_prod, 0, 0);
    printf("%ld records de prod copiés dans newprod\n", nb_recs);
&TX
&SA IS_copy_fn(), IS_create_dbase()
=======================================================================*/

long IS_copy(is1, is2, stop, disp)
ISAM    *is1, *is2;
int     stop, disp;
{
    return(IS_copy_fn(is1, is2, stop, disp, 0L));
}

/* ====================================================================
Duplicie la database is dans un ISAM de même structure et de nom toname.

Le paramètre stop décide si le processus s'arrête (1) ou non (0) en cas
d'erreur. Un message est affiché avant l'arrêt éventuel.

Le paramètre disp permet d'afficher (1) ou non (0) un compteur avec le
nombre de records copiés.

&TI Utilisation
───────────
Cette fonction est utilisée pour agrandir ou diminuer la taille des
records d'un ISAM, ou encore pour recréer une database contenant des
index DUP qui ne devraient pas s'y trouver (après une modification des
INDEX par exemple).

&RT  Nombre de records copiés correctement.
&EX
    long nb_recs;

    nb_recs = IS_dup_dbase(is_prod, "newprod", 0, 0);
    printf("%ld records de prod copiés dans newprod\n", nb_recs);
&TX
&SA IS_copy_fn(), IS_create_dbase()
=======================================================================*/

long IS_dup_dbase(is, toname, stop, disp)
ISAM    *is;
char    *toname;
int     stop, disp;
{
    ISAM    *is2;
    long    rc;

    is2 = IS_dup(is, toname);
    if(IS_create_dbase(is2)) {
	IS_free_dup(is2);
	return(-1L);
	}
    rc = IS_copy_fn(is2, is, stop, disp, 0L);
    IS_free_dup(is2);
    return(rc);
}

/*NH*/
IS_copy_cmp(is, f1, f2)         /* BP_M 15-09-1997 */
ISAM    *is;
char    *f1, *f2;
{
    ISAM    *is2;
    int     i, idx;
    long    nbm = 0;

    for(i = 0 ; i < is->is_nb_cmps ; i++)
	if(is->is_cmps[i].cmp_dup == 1) break;
    if(i == is->is_nb_cmps) {
	PG_display_error("No noduplicate key in ISAM");
	return(-1);
    }
    idx = i;

    is2 = IS_dup(is, f2);
    IS_set_name(is , f1);
    IS_set_name(is2, f2);

    if(IS_open(is, SCR_READ, SCR_NO_LOCK) != 0) {
	PG_display_error("Cannot open file '%s'", is->is_filename);
	return(-2);
    }

    if(IS_open(is2, SCR_UPDATE, SCR_NO_LOCK) != 0) {
	IS_close(is);
	PG_display_error("Cannot open file '%s'", is2->is_filename);
	return(-3);
    }

    if(IS_search(is, 0, SCR_DB, (char *)0) == 0) {
	while(1) {
	    memcpy(is2->is_rec, is->is_rec, is->is_rec_len);
	    if(IS_search(is2, idx, SCR_EQ, (char *)0) != 0) {
		memcpy(is2->is_rec, is->is_rec, is->is_rec_len);
		if(IS_write(is2) != 0)
		    PG_display_error(IS_error_text());
		else
		    nbm++;
	    }
	    if(IS_next(is)) break;
	}
    }

    IS_close_files(is, is2, 0L);
    PG_display_error("Nombre de modif : %ld\n", nbm);
    return(0);
}




