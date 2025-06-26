#include "scr.h"

/* ====================================================================
Alloue l'espace nécessaire au stockage d'une image de nl lignes et nc
caractères. L'image est vide.

&RT le pointeur vers l'IMAGE créée
&SA SCR_save_image(), SCR_free_image()
=======================================================================*/

#ifdef SCRCPP
IMAGE *SCR_alloc_image(
int     nl,
int     nc
)
#else
IMAGE *SCR_alloc_image(nl, nc)
int     nl, nc;
#endif
{
    IMAGE   *im;

    im = (IMAGE *) SCR_malloc(sizeof(IMAGE));
    im->im_cells = (unsigned char *)SCR_malloc(nl * nc * 2);
    im->im_xattrs = (unsigned char *)SCR_malloc(nl * nc);
    im->im_nb_line    = nl;
    im->im_nb_column  = nc;
    return(im);
}


/* ====================================================================
Libère l'espace alloué par SCR_save_image() ou SCR_alloc_image().
&EX
    IMAGE   *im;

    im = SCR_save_image(2, 45, 10, 15);
    ...
    SCR_display_image(im);
    SCR_free_image(im);
&TX
&SA SCR_save_image(), SCR_display_image()
=======================================================================*/

SCR_free_image(im)
IMAGE   *im;
{
    int     i;
    extern IMAGE   **PGIM_LIST;
    extern int     NB_PGIM;

    if(im == 0) return(0);

    for(i = 0 ; i < NB_PGIM ; i++)
	if(im == PGIM_LIST[i]) {
	    PG_undisplay(PAGE_PTR((int)PGIM_LIST[i]));
	    PGIM_LIST[i] = 0;
	    return(0);
	    }

    if((unsigned long)im < SCR_READ_FILES.tbl_nb_fl) return(0);

    SCR_free(im->im_cells);
    SCR_free(im->im_xattrs);
    SCR_free(im);
    return(0);
}




