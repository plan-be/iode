#include "scr.h"

/* ======================================================================
Change le titre du champ EDITOR fld_nb de la page pg. Ce titre ne doit pas
dépasser 40 caractères.

Ne fonctionne pas dans la version WINDOWS.
&NO
    Il n'y a pas de vérification de type !
------------------------------------------------------------------------- */
SCR_set_field_editor_title(pg, fld_nb, title)
PAGE    *pg;
int     fld_nb;
U_ch    *title;
{
    char    *ptr;

    PG_read_flds(pg);

    ptr = PG_get_text(pg, fld_nb);
    SCR_strlcpy(ptr + 2 * sizeof(char *), title, 40);
    return(0);
}

/* ======================================================================
Retourne la valeur du champ fld_nb de la page pg. Ce
champ doit être du type EDITOR.
&NO
    Il n'y a pas de vérification de type !
&RT un pointeur vers le champ
------------------------------------------------------------------------- */
unsigned char *SCR_get_field_editor(pg, fld_nb)
PAGE    *pg;
int     fld_nb;
{
    char    **ptr;

    PG_read_flds(pg);

    ptr = (char **)PG_get_text(pg, fld_nb);
    return(ptr[0]);
}


/* ======================================================================
Fixe la valeur du champ de PAGE passé comme argument. Cet objet doit être
de type EDITOR. Le pointeur vers le texte passé doit être alloué et pouvoir
être libéré par l'éditeur à l'aide de la fonction SCR_free(). L'ancienne
valeur de ce champ est libérée par la fonction.

&SA SCR_get_editor()
------------------------------------------------------------------------ */
SCR_set_editor(obj, text)
OBJ     obj;
U_ch    *text;
{
    SCR_set_field_editor((PAGE *)obj.obj_ptr, obj.obj_fld, text);
    return(0);
}

/* ======================================================================
Lit la valeur du champ de PAGE passé comme argument. Cet objet doit être
de type EDITOR. Retourne le pointeur vers le texte correspondant.

&SA SCR_set_editor()
------------------------------------------------------------------------ */
unsigned char *SCR_get_editor(obj)
OBJ obj;
{
    return(SCR_get_field_editor((PAGE *)obj.obj_ptr, obj.obj_fld));
}

/* ======================================================================
Libère dans une PAGE la mémoire (allouée) utilisée par les champs EDITOR.
------------------------------------------------------------------------- */
PG_free_editor(pg)
PAGE    *pg;
{
    int     i;
    char    **ptr;

    PG_read_flds(pg);

    for(i = 0 ; i < pg->pg_nb_flds ; i++)
	if(pg->pg_flds[i].fld_type == SCR_EDITOR) {
	    ptr = (char **)PG_get_text(pg, i);
	    SCR_free(ptr[0]);
#ifdef SCRW32
	    SCR_free(ptr[1]); /* JMP 12-05-00 */
#else
	    MMT_free((MMT *)ptr[1]);
#endif
	    ptr[0] = ptr[1] = 0;
	}
    return(0);
}
