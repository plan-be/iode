#include "scr.h"

/* ======================================================================
Retourne le pointeur vers le texte du ième champ (de type STRING) qui
suit l'OBJ obj passé comme argument. Ce champ est toujours terminé par
un 0.

Les valeurs directes sont plus concises et plus performantes.

&EX
    sprintf(title, "Nom : %s", PG_vtext(pg_client_name, 0));

	équivaut à

    sprintf(title, "Nom : %s", pg_client_NAME);
&TX
&RT pointeur vers le premier caractère du champ
------------------------------------------------------------------------ */
unsigned char *PG_vtext(obj, i)
OBJ obj;
{
    return((unsigned char *)PG_get_text(obj.obj_ptr, obj.obj_fld + i));
}

/* ======================================================================
Change l'attribut input/output du ième champ qui suit l'objet obj d'une
PAGE. type peut valoir :

&EN SCR_INPUT  : champ input
&EN SCR_OUTPUT : champ output
&EN SCR_HIDDEN : champ hidden
&EN SCR_ACCESSIBLE : champ accessible

&EX
    PG_set_vio(pg_client_code, 0, SCR_OUTPUT);
&TX

&NO
    Les champs de la PAGE doivent être en mémoire. Utilisez le mot-clé
    FLD_MEM dans la défition de la page.
------------------------------------------------------------------------ */
PG_set_vio(obj, i, type)
OBJ     obj;
int     i;
int     type;
{
#ifdef SCRW32       /* BP_M 13-02-2003 16:28 */
    if(type == SCR_HIDDEN) type = SCR_OUTPUT;
#endif

    ((PAGE *)obj.obj_ptr)->pg_flds[obj.obj_fld + i].fld_io = type - SCR_INPUT;
    return(0);
}

/* BP_M 07-01-2010 16:01 */
/* ======================================================================
Retourne l'attribut input/output du ième champ qui suit l'objet obj d'une
PAGE.

&EN SCR_INPUT  : champ input
&EN SCR_OUTPUT : champ output
&EN SCR_HIDDEN : champ hidden
&EN SCR_ACCESSIBLE : champ accessible

&EX
    PG_get_vio(pg_client_code, 0);
&TX

&NO
    Les champs de la PAGE doivent être en mémoire. Utilisez le mot-clé
    FLD_MEM dans la défition de la page.
------------------------------------------------------------------------ */
PG_get_vio(obj, i)
OBJ     obj;
int     i;
{
    int     type;

    type = ((PAGE *)obj.obj_ptr)->pg_flds[obj.obj_fld + i].fld_io + SCR_INPUT;
#ifdef SCRW32       /* BP_M 13-02-2003 16:28 */
    if(type == SCR_HIDDEN) type = SCR_OUTPUT;
#endif
    return(type);
}
/* BP_M 07-01-2010 16:01 */

PG_reset_vobj(obj, i)
OBJ     obj;
{
    PG_reset_field((PAGE *)obj.obj_ptr, obj.obj_fld + i, 0);
    return(0);
}

/*NH*/
PG_set_vfield(obj, i, text)
OBJ     obj;
int     i;
char    *text;
{
    SCR_set_field_text((PAGE *)obj.obj_ptr, obj.obj_fld + i, text);
    return(0);
}

/*NH*/
PG_get_vfield(obj, i, text)
OBJ     obj;
int     i;
char    *text;
{
    SCR_get_field_text((PAGE *)obj.obj_ptr, obj.obj_fld + i, text);
    return(0);
}


/* ======================================================================
Change l'attribut couleur du ième champ qui suit l'objet obj d'une
PAGE. attr peut valoir :

&EN DEFAULT
&EN REVERSE
&EN UNDERLINE
&EN BOLD
&EN INVISIBLE
&EN BLINKING
&EN DIM
&EN BLUE
&EN GREEN
&EN CYAN
&EN RED
&EN MAGENTA
&EN YELLOW
&EN WHITE
&EN BLACK

&EX
    PG_set_vattr(pg_client_code, 0, SCR_YELLOW);
&TX

&NO
    Les champs de la PAGE doivent être en mémoire. Utilisez le mot-clé
    FLD_MEM dans la défition de la page.
------------------------------------------------------------------------ */
PG_set_vattr(obj, i, attr)
OBJ     obj;
int     i;
int     attr;
{
    ((PAGE *)obj.obj_ptr)->pg_flds[obj.obj_fld + i].fld_attributes = attr;
    return(0);
}



/* ======================================================================
Affiche le ième champ qui suit l'objet obj d'une PAGE.

&EX
    PG_display_vobj(pg_client_chaff, 2);
&TX
&SA PG_display_field()
------------------------------------------------------------------------ */
PG_display_vobj(obj, i)
OBJ     obj;
int     i;
{
    PG_display_field(((PAGE *)obj.obj_ptr), obj.obj_fld + i);
    return(0);
}

/* ======================================================================
Edite le ième champ qui suit l'objet obj d'une PAGE.

&EX
    PG_edit_vobj(pg_client_chaff, 0);
&TX
&SA PG_edit_field()
------------------------------------------------------------------------ */
PG_edit_vobj(obj, i)
OBJ     obj;
int     i;
{
    int     j, *io;
    PAGE    *pg = (PAGE *)obj.obj_ptr;
    FIELD   *fld;

    if(pg->pg_flds[obj.obj_fld + i].fld_io != 0 /* SCR_INPUT */) return(0);

    io = (int *)SCR_malloc(pg->pg_nb_flds * sizeof(int));
    for(j = 0, fld = pg->pg_flds ; j < pg->pg_nb_flds ; j++, fld++) {
	io[j] = fld->fld_io;
	fld->fld_io = SCR_OUTPUT - SCR_INPUT;
    }
    pg->pg_flds[obj.obj_fld + i].fld_io = 0; /* SCR_INPUT */

    PG_edit(pg);

    for(j = 0, fld = pg->pg_flds ; j < pg->pg_nb_flds ; j++, fld++)
	fld->fld_io = io[j];
    SCR_free(io);
    return(0);
}


/* ======================================================================
Change la valeur par défaut du ième champ qui suit l'objet obj d'une
PAGE. Ce champ doit avoir une valeur par défaut déjà définie dans la
PAGE originale (source SCR). Dans le cas contraire, la fonction est sans
effet.

La copie de text est limitée à la longueur de la valeur par défaut
d'origine.

&EX
    PG_set_vdfld(pg_lex_legis, 0, "1990/1991");
&TX
------------------------------------------------------------------------ */
PG_set_vdfld(obj, i, text)
OBJ             obj;
int             i;
unsigned char   *text;
{
    PAGE    *pg;
    int     len, txtlen;        /* JMP 01-06-91 */
    FIELD   *fld;

    i += obj.obj_fld;
    pg = (PAGE *)obj.obj_ptr;
    fld = pg->pg_flds + i;
    if(fld->fld_dtext == 0) return(0);
    len = fld->fld_nb_column * fld->fld_nb_line;
    txtlen = strlen(text);                      /* JMP 01-06-91 */
    len = min(txtlen, len);                     /* JMP 01-06-91 */
    txtlen = strlen(fld->fld_dtext);            /* JMP 01-06-91 */
    len = min(txtlen, len);                     /* JMP 01-06-91 */

    strncpy(fld->fld_dtext, text, len);
    return(0);
}


/* ======================================================================
Vide le ième champ qui suit l'objet obj d'une PAGE, quelqu'en soit le
type (0 ou blancs).
------------------------------------------------------------------------ */

PG_empty_vobj(obj, i)
OBJ     obj;
int     i;
{
    PG_empty_field((PAGE *)obj.obj_ptr, obj.obj_fld + i);
    return(0);
}



/* ======================================================================
Fixe à type l'attribut input/output de tous les champs du groupe grpnb
de la PAGE pg.
Le numéro du groupe correspond à l'ordre d'apparition dans la définition
de la PAGE. Le groupe 0 correspond aux champs n'appartenant à aucun
groupe.

Les types reconnus sont :

&EN SCR_INPUT  : champ input
&EN SCR_OUTPUT : champ output
&EN SCR_HIDDEN : champ hidden
&EN SCR_ACCESSIBLE : champ accessible

&EX
    PG_set_grpio(pg_client, 1, SCR_OUTPUT);
&TX

&NO
    Les champs de la PAGE doivent être en mémoire. Utilisez le mot-clé
    FLD_MEM dans la défition de la page.
------------------------------------------------------------------------ */
PG_set_grpio(pg, grpnb, type)
PAGE    *pg;
int     grpnb, type;
{
    int     i;

    for(i = 0 ; i < pg->pg_nb_flds ; i++)
	if(pg->pg_flds[i].fld_grp == grpnb)
	    pg->pg_flds[i].fld_io = type - SCR_INPUT;

    return(0);
}











