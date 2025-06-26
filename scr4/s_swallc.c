#include <s_swap.h>
#include <s_strs.h>
#include <string.h>

void *SW_nrealloc_doc(void *old_ptr,unsigned int old_count,unsigned int new_count, char *file, int line);
//char *SW_nrealloc_doc(void *old_ptr, unsigned int old_count, unsigned int new_count, char *file, int line);

#undef ALLOCDOCNULL

/*
Fonction d'allocation d'un bloc de mémoire de size bytes. La taille
maximum d'un bloc est de 16K - 4 (SW_BLK_SIZE - 4), soit 16380 bytes.
size est arrondi au nombre pair de bytes supérieur.


Pour obtenir le pointeur en mémoire, utilisez SW_getptr.

Le nombre maximum d'allocation est de 32000.

Si la machine dispose de mémoire étendue et que le driver EMS est
installé, la mémoire EMS est utilisée en premier pour les allocations.
Ensuite, la mémoire conventionnelle est utilisée tant qu'il reste
SW_MIN_MEM bytes libres. Cette variable peut être modifiée par
l'utilisateur.

Enfin, un fichier est créé sur disque pour contenir les blocs
supplémentaires. La variable d'environnement SWAPFILE contient le nom de
ce fichier qui peut se trouver sur n'importe quel disque. Si SWAPFILE
n'est pas défini, le fichier se trouvera sur le disque courant sous le
nom SWAPFILE.SW.

&RT SWHDL identifiant la partie de mémoire contenant l'espace.

&TI Remarques

&EN pour utiliser les fonctions d'allocation et de free, il faut
    initialiser le système à l'aide de SW_init

&EN pour terminer, utilisez la fonction SW_end. Si cela n'est pas
    fait, la mémoire EMS n'est plus utilisable avant d'avoir rebooté le
    système et le fichier SWAPFILE reste sur disque.

&EN en cas de dépassement de mémoire disque, le système s'arrête et
    affiche le message "Panic : memory full".

&EN utilisez SW_getptr à chaque fois que vous devez utiliser un bloc
    mémoire, ceux-ci pouvant à tout moment être déplacés sur disque ou
    en mémoire étendue.

&SA SW_realloc(), SW_free(), SW_nalloc()
*/


SWHDL SW_alloc(size)
SWSIZE     size;
{
    SWSIZE  lost, size2;
    int     blk;
    SWHDL   item;
    char    *buf;

    if(size == 0) return(0);
    if(SW_init(1)) return(0);
    size2 = size;
    if(size2 % SW_ALIGN)                   /* JMP 23-12-91 */
        size2 +=  SW_ALIGN - size2 % SW_ALIGN;      /* JMP 23-12-91 */
//    if(size2 + h_size > SW_SEG_SIZE) SW_panic(6); /* JMP 23-05-00 */
    blk  = SW_find_free_blk(size2);
    item = SW_find_free_item();

    SW_set_item(item, blk, FREESP(blk) + h_size);
    buf = PTR(blk) + FREESP(blk);
    *(SWSIZE *)buf = size2;
    *(SWHDL *)(buf + s_size) = item;  /* !!! alignement */
    memset(buf + h_size, 0, (unsigned int)size2);
    lost = size2 + h_size;
    SPACE(blk) -= lost;
    FREESP(blk)  += lost;

//DebugForce("SW_SEG_SIZE=%d\n", SW_SEG_SIZE);

    return(item);
}


/*NH*/
SW_find_free_blk(size)
SWSIZE  size;
{
    int     i, j = -1;

    size += h_size;
    for(i = 0 ; i < SW_NB_BLKS ; i++) {
        // if(SW_SEG_SIZE >= FREESP(i) + size) break; /* JMP 23-05-00 */
        if(BLKSIZE(i) >= FREESP(i) + size) break; /* JMP 23-05-00 */
        if(SPACE(i) >= size) j = i;
    }
    if(i < SW_NB_BLKS) {
        SW_restore(i);
        return(i);
    }
    if(j >= 0) {
        SW_restore(j);
        SW_compress(j);
        return(j);
    }
    SW_create_blk(size); /* JMP 23-05-00 */
    return(i);
}

/*NH*/
/* Fonction de restauration d'un bloc */
int SW_restore(blk_nb)
int     blk_nb;
{
    int     seg_nb;

    seg_nb = SEG(blk_nb);
    if(seg_nb < 0) {
        seg_nb = SW_oldest_seg();
        SW_swap_ems(seg_nb);
        SW_swap(SW_SEGS[seg_nb].seg_ptr, REF(blk_nb));
        SEG(SW_SEGS[seg_nb].seg_blk) = -1;
        SEG(blk_nb)                  = seg_nb;
        REF(SW_SEGS[seg_nb].seg_blk) = REF(blk_nb);
        SW_SEGS[seg_nb].seg_blk      = blk_nb;
    }
    else
        SW_swap_ems(seg_nb);

    AGE(seg_nb) = SW_AGE++;
    BLKAGE(blk_nb) = SW_AGE;
    return(0);
}

/*NH*/
int SW_create_blk(size)
SWSIZE      size;
{
    int     seg_nb;

    if(SW_NB_BLKS % 10 == 0)
//      SW_BLKS = (SWBLK *) SW_nrealloc(SW_BLKS, sizeof(SWBLK) * SW_NB_BLKS,
//                                      sizeof(SWBLK) * (SW_NB_BLKS + 10));

        SW_BLKS = (SWBLK *)SW_nrealloc_doc(SW_BLKS,
                                           sizeof(SWBLK) * SW_NB_BLKS,
                                           sizeof(SWBLK) * (SW_NB_BLKS + 10),
                                           __FILE__,
                                           __LINE__);


    if(SW_BLKS == 0) SW_panic(5);
    size = max(size, SW_SEG_SIZE); /* JMP 23-05-00 */
    SW_init_blk(SW_NB_BLKS, size);

    /* TRY TO CREATE NEW SEG */
    if(SW_create_seg(size) == 0)  /* SUCCESS */
        seg_nb = SW_NB_SEGS - 1;

    else { /* NO MEMORY */
        if(SW_NB_SEGS < SW_MIN_SEGS) SW_panic(5); /* JMP 08-03-92 */
        seg_nb = SW_oldest_seg();
        SW_swap_ems(seg_nb);
        SW_append_file(SW_SEGS[seg_nb].seg_ptr);
        SEG(SW_SEGS[seg_nb].seg_blk) = -1;
        REF(SW_SEGS[seg_nb].seg_blk) = SW_FILE_POS - 1;
    }

    SEG(SW_NB_BLKS) = seg_nb;
    SW_SEGS[seg_nb].seg_blk = SW_NB_BLKS;
    AGE(seg_nb) = SW_AGE++;
    BLKAGE(SW_NB_BLKS) = SW_AGE;

    SW_NB_BLKS++;
    return(0);
}

/*NH*/
SWHDL SW_find_free_item() {
    SWHDL   n;

    if(SW_nb_frees() > 0) {
        n = SW_get_free(SW_nb_frees() - 1);
        LST_del(SW_FREES, SW_nb_frees() - 1L, 1L);
        return(n);
    }
    SW_add_item();
    return((SWHDL)(SW_nb_items() - 1));
}

/*
Cette fonction libère un espace alloué par SW_alloc. Deux appels à cette
fonction pour le même item ne posent pas de problème.

item est l'identifiant retourné par SW_alloc.

Il n'est pas utile d'appeler cette fonction avant SW_end : le fait de
terminer la session libère tous les blocs alloués, que ce soit en
mémoire conventionnelle, EMS ou fichier.

&SA SW_alloc(), SW_realloc()
*/

int SW_free(item)
SWHDL   item;
{
    int     blk;
    SWSIZE  size, lost;
    char    *buf;

    if(!SW_ACTIVE) return(0);
    if(item <= 0) return(0);
    blk = SW_get_blk(item);
    if(blk < 0) return(0);
    buf = SW_getptr(item) - h_size;
    size = *(SWSIZE *)buf;
    lost = size + h_size;
    SPACE(blk) += lost;
    if(SW_get_pos(item) + size == FREESP(blk)) /* LAST ITEM OF BLOCK */
        FREESP(blk) -= lost;
    else {
        COMPRESS(blk) = 0;
        *(SWHDL *)(buf + s_size) = 0xFFFFFFFF;
    }
    SW_add_free(item);
    SW_set_item(item, -1, (SWSIZE)0);
    return(0);
}

/*
Fonction de réallocation fonctionnant suivant le même principe que la
fonction standard realloc. item est le numéro de l'item déjà alloué et
size le nouveau nombre de bytes.

L'ancien bloc de mémoire est sauvé dans un bloc temporaire, libéré et un
nouveau bloc est alloué dans lequel est recopié le bloc temporaire. Le
numéro de l'item est en général le même que le précédent, mais le
pointeur retourné par SW_getptr peut changer, même si le nouveau nombre
de bytes est inférieur ou égal à l'ancien.

&RT nouvel item correspondant à la nouvelle allocation

&SA SW_alloc(), SW_free(), SW_getptr()
*/

#ifdef SCRCPP
SWHDL SW_realloc(
    SWHDL   item,
    SWSIZE  size
)
#else
SWHDL SW_realloc(item, size)
SWHDL   item;
SWSIZE  size;
#endif

{
    char    *buf, *tmp;
    SWSIZE  osize, allc = 0;
    SWHDL   hnew;

    if(size <= 0) {
        SW_free(item);
        return(0);
    }
    if(SW_init(1)) return(0);
    buf = SW_getptr(item);
    osize = *(SWSIZE *)(buf - h_size);
    if(size == osize) return(item);
    osize = min(osize, size);
    if(SW_MIN_SEGS < 2) {  /* PEUT ETRE AMELIORE */
        // tmp = (char *)malloc(osize);
        tmp = (char *)SCR_malloc(osize);
        if(tmp == 0) return(item);
        memcpy(tmp, buf, (unsigned int)osize);
        SW_free(item);
        allc = 1;
    }
    hnew = SW_alloc(size);
    buf = SW_getptr(hnew);
    if(allc) {
        memcpy(buf, tmp, (unsigned int)osize);
        //free(tmp);
        SCR_free(tmp);
    }
    else {
        memcpy(buf, SW_getptr(item), (unsigned int)osize);
        SW_free(item);
    }
    return(hnew);
}

/*
Cette fonction retourne un char * pointant vers la zône mémoire
contenant le premier byte de item.

Il est indispensable de faire appel à cette fonction à chaque
utilisation de item, car la zône mémoire peut à tout moment être
déplacée sur fichier ou en mémoire étendue.

On a cependant la garantie qu'au moins les 2 derniers blocs mémoire
appelés sont réellement en mémoire, ce qui permet des copies de blocs.

La fonction procède comme suit : si le bloc mémoire auquel appartient
l'item est en mémoire, elle retourne simplement le pointeur. Si ce n'est
pas le cas, le bloc le plus ancien est reporté en EMS ou sur fichier et
le bloc contenant l'item demandé est ramené à sa place en mémoire
conventionnelle ou dans le Page Frame de l'EMS. Le pointeur peut alors
être retourné.

&SA SW_alloc(), SW_realloc()
*/

char *SW_getptr(item)
SWHDL   item;
{
    int     blk, seg_nb;

    blk = SW_get_blk(item);
    if(blk < 0) return((char *)0); /* JMP 24-06-98 */
    seg_nb = SEG(blk);
    if(seg_nb < 0) SW_restore(blk);
    else {
        AGE(seg_nb) = SW_AGE++;
        BLKAGE(blk) = SW_AGE;
    }

#ifndef NOEMS
    if(SW_SEGS[seg_nb].seg_type  == 0 &&
            SW_SEGS[seg_nb].seg_physp < 0)
        SW_swap_ems(seg_nb);
#endif

    return(PTR(blk) + SW_get_pos(item));
}

/*
Cette fonction effectue une allocation standard en mémoire
conventionnelle à l'aide de malloc. S'il n'y a pas assez de mémoire
disponible, les blocs créés par SW_alloc() sont déplacés en EMS ou sur
fichier et libérés, pour permettre d'allouer quand-même la mémoire
demandée.

La mémoire allouée est mise à zéro.

&SA SW_alloc(), SW_nrealloc(), SW_nfree()
*/

char *SW_nalloc_orig(size)
unsigned int    size;
{
    char    *ptr = 0;

    if(size == 0) return(ptr);
    while(1) {
        //ptr = malloc(size);
        ptr = SCR_malloc(size);
        if(ptr != 0) {
            //memset(ptr, 0, size); /* JMP 28-08-2012 */
            return(ptr);
        }
        SW_Debug(size);
        //Debug("nalloc\n");
        if(SW_init(1)) return(ptr);
        if(SW_del_seg()) {
            SW_panic(5);
            return(ptr);
        }
    }
}

char *SW_nalloc_doc(unsigned int size, char *file, int line) {
    char        *ptr = SW_nalloc_orig(size);
    ALLOCDOC    *ad;

    if(ptr == 0) return(ptr);
    if(SCR_ALLOC_DOC == 0) return(ptr);

    ad = (ALLOCDOC *)(ptr - sizeof(ALLOCDOC)); // JMP 12/02/2017
    ad->file = file;
    ad->line = line;
#ifdef ALLOCDOCNULL
    if(file == 0) {
        printf("File 0");
    }
#endif
    return(ptr);
}

/*
Cette fonction effectue une réallocation standard en mémoire
conventionnelle à l'aide de realloc(). S'il n'y a pas assez de mémoire
disponible, les blocs créés par SW_alloc sont déplacés en EMS ou sur
fichier et libérés, pour permettre d'allouer quand-même la mémoire
demandée.

La mémoire supplémentaire allouée est mise à zéro.

&SA SW_nalloc(), SW_nfree(), SW_realloc()
*/

char *SW_nrealloc_orig(old_ptr, old_count, new_count)
void            *old_ptr;
unsigned int    old_count,
         new_count;
{
    char    *ptr;

    if(old_count == new_count) return(old_ptr);
    if(old_ptr == 0 || old_count <= 0)
        return(SW_nalloc_orig(new_count));

    while(1) {
        //ptr = realloc(old_ptr, new_count);
        ptr = SCR_realloc(old_ptr, 1, old_count, new_count); /* JMP 22-08-2012 */
        if(ptr != 0) {
            if(old_count >= new_count) return(ptr);
            memset(ptr + old_count, 0, new_count - old_count);
            return(ptr);
        }
        if(SW_init(1)) return(ptr);
        SW_Debug(new_count);
        if(SW_del_seg()) {
            SW_panic(5);
            return(ptr);
        }
    }
}

void *SW_nrealloc_doc(old_ptr, old_count, new_count, file, line)
//char *SW_nrealloc_doc(old_ptr, old_count, new_count, file, line)
void            *old_ptr;
unsigned int    old_count,
         new_count;
char            *file;
int             line;
{
    char        *ptr = SW_nrealloc_orig(old_ptr, old_count, new_count);
    ALLOCDOC    *ad;

    if(ptr == 0) return(ptr);
    if(SCR_ALLOC_DOC == 0) return(ptr);
    ad = (ALLOCDOC*) (ptr - sizeof(ALLOCDOC));
    ad->file = file;
    ad->line = line;
#ifdef ALLOCDOCNULL
    if(file == 0) {
        printf("File 0");
    }
#endif

    return(ptr);
}

/*
Cette fonction libère une allocation standard en mémoire conventionnelle
à l'aide de free, sauf si le pointeur passé est nul.
*/

int SW_nfree(ptr)
//char    *ptr;
void *ptr;
{
    if(ptr == 0) return(0);
    //free(ptr);
    SCR_free(ptr); /* JMP 22-08-2012 */
    return(0);
}
