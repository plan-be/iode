#include "scr4.h"
#include <s_swap.h>

int      SW_ACTIVE = 0;

/*
    Fonction d'initialisation du swapping. Cette fonction
    alloue tout l'espace EMS disponible s'il y en a et
    utilise cet espace pour y placer les blocs à allouer.

    Si le paramètre ems est null, la mémoire EMS n'est pas
    utilisée.

    Remarques :

&EN il est impératif d'utiliser cette fonction avant
    de commencer à utiliser les fonctions de swapping.

&EN terminez toujours la session de swapping par SW_end().
    Sans cela, la mémoire EMS n'est pas rendue disponible et le fichier
    éventuel de SWAP n'est pas détruit. De plus, toutes les allocations
    internes du système restent actives.

&SA SW_end()
*/

SW_init(ems)
int     ems;
{
    int     i = 0;
    SWSEG   *seg;

    if(SW_ACTIVE) return(0);
    SW_NB_SEGS = SW_NB_BLKS = 0;
#ifndef NOEMS
    SW_NB_EMS = 0;
#endif
    SW_SEGS = 0;
    SW_BLKS = 0;
    SW_FREES = 0;
    SW_ITEMS = 0;
#ifndef NOEMS
    /* CHECK IF EMS PRESENT AND ALLOC ALL PAGES */
    if(ems == 0 || EMS_init() || EMS_DEF.nb_free == 0) goto fin;

    /* INITIALISE */
    SW_NB_EMS = EMS_DEF.nb_free;
    SW_SEGS = (SWSEG *) SW_nalloc(sizeof(SWSEG) * 10 * ((SW_NB_EMS - 1) / 10 + 1));
    SW_BLKS = (SWBLK *) SW_nalloc(sizeof(SWBLK) * 10 * ((SW_NB_EMS - 1) / 10 + 1));
    SW_NB_BLKS = SW_NB_SEGS = SW_NB_EMS;
    if(SW_SEGS == 0 || SW_BLKS == 0) {
	SW_nfree(SW_SEGS);
	SW_nfree(SW_BLKS);
/*        SW_nfree(SW_BUF); */
	EMS_end();
	return(-1);
	}

    seg = SW_SEGS;
    for(i = 0; i < SW_NB_EMS ; i++, seg++) {
	seg->seg_blk = i;
	seg->seg_physp = -1;
	SW_init_blk(i, SW_SEG_SIZE);
	SEG(i) = i;
	}

    for(i = 0 ; i < min(4, SW_NB_EMS) ; i++) {
	if(EMS_map(i, i)) {
	    fprintf(stderr, "EMS Map error : %s\n",
				    EMS_error_text(EMS_DEF.swerrno));
	    return(-1);
	    }
	SW_SEGS[i].seg_physp = i;
	SW_SEGS[i].seg_ptr   = EMS_SEG(i);
	}

fin:
#endif
/*    SW_ITEMS = LST_create(256, s_size + sizeof(short)); /* JMP 16-11-00 */
    SW_ITEMS = LST_create(1024, s_size + sizeof(short));  /* JMP 16-11-00 */
    SW_FREES = LST_create(512, s_hdl);
    SW_ACTIVE = 1;
    SW_alloc(1);  /* Use the item 0 */
    return(0);
}

/* ====================================================================
Fonction indiquant que l'on n'a plus besoin du système EMS. Cette
fonction libère tous les segments de mémoire conventionnelle utilisés
par le système de swapping, détruit le fichier de swap (SWAPFILE.SW) et
libère les pages EMS.

&SA SW_init()
====================================================================== */

void SW_end()
{
    int     i, base = 0;

    if(!SW_ACTIVE) return;
#ifndef NOEMS
    base = SW_NB_EMS;
#endif

    for(i = base ; i < SW_NB_SEGS ; i++) 
        //SW_nfree((char *)(SW_SEGS + i)); /*JMP 30-09-91 */
        SW_nfree(SW_SEGS[i].seg_ptr);
        
    SW_nfree((char *)SW_SEGS);
    SW_nfree((char *)SW_BLKS);
    LST_free(SW_FREES);
    LST_free(SW_ITEMS);
    SW_close();
#ifndef NOEMS
    EMS_end();
#endif
    SW_ACTIVE = 0;
}
/*NH*/
/*
    Cette fonction termine le programme est cas d'erreur de Swap.
    Elle utilise la fonction exit() avec comme paramètre le code
    d'erreur et affiche un message correspondant. Vous pouvez
    remplacer cette fonction par une fonction à vous en n'oubliant
    toutefois pas l'appel à <SW_end> avant de sortir.

    Les différents codes d'erreur sont les suivants :

	code 1 : File creation error

	code 2 : Seek error

	code 3 : Write error

	code 4 : Read error

	code 5 : Conventional memory full

	case 6 : Segment size exceeded

*/

SW_panic(code)
int     code;
{
    char        *text;
    static  int is_panic = 0;
    int         i;

    if(is_panic) {
	for(i = 0 ; i < 50 ; i++) Wprintf("\n");        /* JMP 16-02-97 */
	Wprintf("!! Double panic !!\nProgram stopped.\n"); /* JMP 16-02-97 */
	SW_end();
	exit(1);
	}
    switch(code) {
	case 1 : text = "File creation error"; break;
	case 2 : text = "Seek error"; break;
	case 3 : text = "Write error"; break;
	case 4 : text = "Read error"; break;
	case 5 : text = "Conventional memory full"; break;
	case 6 : text = "Segment size exceeded"; break;
	default: text = "Unknown code"; break;
	}
    is_panic = 1;
    SW_abort(code, text);
    SW_end();
    return(0);
}

/*NH*/
SW_init_blk(blk_nb, size)
int     blk_nb;
SWSIZE  size;
{
    BLKSIZE(blk_nb) = size;
    SPACE(blk_nb)   = size;
    FREESP(blk_nb)  = 0;
    COMPRESS(blk_nb)= 1;
    REF(blk_nb)     = -1;
    return(0);
}

/*NH*/
SW_compress(blk_nb)
int     blk_nb;
{
    char    *ptr;
    SWSIZE  size, gain = 0, shift = 0;
    SWHDL   item, item0 = 0xFFFFFFFF;

    if(COMPRESS(blk_nb)) return(0);
    COMPRESS(blk_nb) = 1;
    if(SPACE(blk_nb) == BLKSIZE(blk_nb)) {
	FREESP(blk_nb) = 0;
	return(0);
	}

    ptr = PTR(blk_nb);

    while(FREESP(blk_nb) > shift) {
	size = *(SWSIZE *)(ptr + shift) + h_size;
	item = *(SWHDL *)(ptr + shift + s_size);
	if(item == item0) {
	    gain += size;
	    memcpy(ptr + shift, ptr + shift + size, (unsigned int)(FREESP(blk_nb) - size - shift));
	    FREESP(blk_nb) -= size;
	    }
	else {
	    SW_set_item(item, blk_nb, SW_get_pos(item) - gain);
	    shift += size;
	    }
	}
    return(0);
}

/*
    Cette fonction retourne le nombre de bytes disponibles dans le
    système de swap. Ce nombre contient la somme de tous les bytes
    restant dans les blocs déjà alloués. Elle contient donc
    l'espace restant en EMS. La partie fichier y est aussi comprise :
    Si un espace disque a été alloué, puis libéré, ce nombre
    de bytes est pris en compte dans le total.
*/

long    SW_coreleft()
{
    int     i;
    long    lg = 0L;

    if(!SW_ACTIVE) return(lg);
    for(i = 0 ; i < SW_NB_BLKS ; i++)
	lg += SPACE(i);

    return(lg);
}

/*
    Cette fonction stocke dans la structure st les informations
    sur l'utilisation de la mémoire à l'aide des fonctions du
    package.
*/

SW_status(st)
SWSTAT  *st;
{
    int     i;
    extern  long SCR_free_mem();

    SW_init(1);
    memset(st, 0, sizeof(SWSTAT));

    st->st_nballoc  = SW_nb_items();
    st->st_nbfree   = SW_nb_frees();
    st->st_nbsegs   = SW_NB_SEGS;
    st->st_segsize  = SW_SEG_SIZE;
    st->st_nbblks   = SW_NB_BLKS;
    st->st_convfree = SCR_free_mem();

#ifndef NOEMS
    for(i = 0 ; i < SW_NB_SEGS ; i++) {
	if(SW_SEGS[i].seg_type == 0) { /* EMS */
	    st->st_emsalloc += SW_SEG_SIZE;
	    st->st_emsafree += SW_BLKS[i].blk_space;
	    }
	else {                         /* CONV */
	    st->st_convalloc += SW_SEG_SIZE;
	    st->st_convafree += SW_BLKS[i].blk_space;
	    }
	}
#else
    st->st_convalloc = (long) SW_NB_SEGS * (long) SW_SEG_SIZE;
    for(i = 0 ; i < SW_NB_SEGS ; i++)
	st->st_convafree += SW_BLKS[i].blk_space;
#endif

    for(i = 0 ; i < SW_NB_BLKS ; i++)
	if(SW_BLKS[i].blk_seg < 0) {
	    st->st_filealloc += SW_SEG_SIZE;
	    st->st_fileafree += SW_BLKS[i].blk_space;
	    }
    return(0);
}

/*NH*/
/* GESTION MEMOIRE DES ITEMS_BLK, ITEMS_POS, FREES */

SW_set_item(item, blk, pos)
SWHDL   item;
int     blk;
SWSIZE  pos;
{
    char    *ptr;

    ptr = LST_get(SW_ITEMS, (long)item);
    *(short *)ptr = blk;
    *(SWHDL *)(ptr + sizeof(short)) = (SWHDL)pos;
    return(0);
}

/*NH*/
SWSIZE SW_get_pos(item)
SWHDL item;
{
    char    *ptr;

    ptr = LST_get(SW_ITEMS, (long)item);
    return((SWSIZE)*(SWHDL *)(ptr + sizeof(short)));
}

/*NH*/
SW_get_blk(item)
SWHDL   item;
{
    char    *ptr;

    ptr = LST_get(SW_ITEMS, (long)item);
    if(ptr == 0) return(-1); /* JMP 24-06-98 */
    //return((int)(*(short *)ptr));
    //return((int)(*(unsigned short *)ptr)); // JMP 15/7/2013
    return((int)(*(short *)ptr)); // JMP 20/06/2022 to get -1 when a item has been freed
}

/*NH*/
SW_add_item()
{
    LST_add_1(SW_ITEMS);
    return(0);
}

/*NH*/
SW_nb_items()
{
    return(SW_ITEMS->nb_els);
}

/*NH*/
SWHDL SW_get_free(item)
SWHDL   item;
{
    SWHDL  *it;

    it = (SWHDL *)LST_get(SW_FREES, (long)item);
    return(it[0]);
}

/*NH*/
SW_add_free(item)
SWHDL   item;
{
    LST_add_1(SW_FREES);
    LST_set(SW_FREES, SW_nb_frees() - 1L, &item);
    return(0);
}

/*NH*/
SW_nb_frees()
{
    return(SW_FREES->nb_els);
}





