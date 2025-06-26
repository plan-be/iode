#include "scr.h"

extern int  IS_REC_RREC;

/* ======================================================================
Lit la valeur lue dans le record ISAM du ième champ qui suit l'objet
passé comme argument. Cet objet doit faire partie d'un ISAM. Cela permet
par exemple de vérifier si un champ a été modifié (en mémoire) depuis la
lecture du record.

Il n'y a pas de vérification de type! Vérifiez donc soigneusement le
type des champs utilisés.

Cette fonction doit avoir connaissance de l'OBJ utilisé. Il faut donc
placer dans le champ en question DEF_OBJ pour en obtenir la définition
dans les fichiers .c générés par scr4_e.

&SA SCR_get_vchar(), SCR_set_vchar()
------------------------------------------------------------------------ */
SCR_rget_vchar(obj, i)
OBJ     obj;
int     i;
{
    IS_REC_RREC = 1;
    return(SCR_get_vchar(obj, i));
}

/* ======================================================================
Lit la valeur lue dans le record ISAM du ième champ qui suit l'objet
passé comme argument. Cet objet doit faire partie d'un ISAM. Cela permet
par exemple de vérifier si un champ a été modifié (en mémoire) depuis la
lecture du record.

&SA SCR_rget_vchar()
------------------------------------------------------------------------ */
double SCR_rget_vdouble(obj, i)
OBJ     obj;
int     i;
{
    IS_REC_RREC = 1;
    return(SCR_get_vdouble(obj, i));
}

/* ======================================================================
Lit la valeur lue dans le record ISAM du ième champ qui suit l'objet
passé comme argument. Cet objet doit faire partie d'un ISAM. Cela permet
par exemple de vérifier si un champ a été modifié (en mémoire) depuis la
lecture du record.

&SA SCR_rget_vchar()
------------------------------------------------------------------------ */
float SCR_rget_vfloat(obj, i)
OBJ     obj;
int     i;
{
    IS_REC_RREC = 1;
    return(SCR_get_vfloat(obj, i));
}

/* ======================================================================
Lit la valeur lue dans le record ISAM du ième champ qui suit l'objet
passé comme argument. Cet objet doit faire partie d'un ISAM. Cela permet
par exemple de vérifier si un champ a été modifié (en mémoire) depuis la
lecture du record.

&SA SCR_rget_vchar()
------------------------------------------------------------------------ */
long SCR_rget_vlong(obj, i)
OBJ     obj;
int     i;
{
    IS_REC_RREC = 1;
    return(SCR_get_vlong(obj, i));
}


/* ======================================================================
Lit la valeur lue dans le record ISAM du ième champ qui suit l'objet
passé comme argument. Cet objet doit faire partie d'un ISAM. Cela permet
par exemple de vérifier si un champ a été modifié (en mémoire) depuis la
lecture du record.

&SA SCR_rget_vchar()
------------------------------------------------------------------------ */
short SCR_rget_vshort(obj, i)
OBJ     obj;
int     i;
{
    IS_REC_RREC = 1;
    return(SCR_get_vshort(obj, i));
}

/* ======================================================================
Lit la valeur lue dans le record ISAM du ième champ qui suit l'objet
passé comme argument. Cet objet doit faire partie d'un ISAM. Cela permet
par exemple de vérifier si un champ a été modifié (en mémoire) depuis la
lecture du record.

&SA SCR_rget_vchar()
------------------------------------------------------------------------ */
char *SCR_rget_vtext(obj, i, txt)
OBJ     obj;
int     i;
char    *txt;
{
    IS_REC_RREC = 1;
    return(SCR_get_vtext(obj, i, txt));
}



/* ====================================================================
Entame une recherche dans l'ISAM dont fait partie l'objet obj. Cet objet
doit être un index de l'ISAM. La recherceh s'effectue suivant l'ordre de cet
index.

cond est la condition de recherche.

La recherche commence en fonction de la valeur des champs de l'index de
recherche placés dans le record de l'ISAM. Ces valeurs peuvent être
fixées par les fonctions IS_set_field(), SCR_set_*(), etc.

cond peut prendre les valeurs suivantes :

&EN SCR_GE : recherche les records supérieurs ou égaux
&EN SCR_LE : recherche les records inférieurs ou égaux
&EN SCR_EQ : recherche les records égaux
&EN SCR_EQ1 : recherche les records dont le premier champ de la clé
	      est identique
&EN SCR_EQ2 : recherche les records dont les 2 premiers champ de la clé
	      est identique
&EN SCR_EQ3 : recherche les records dont les 3 premiers champ de la clé
	      est identique
&EN SCR_EQ4 : recherche les records dont les 4 premiers champ de la clé
	      est identique
&EN SCR_EQ5 : recherche les records dont les 5 premiers champ de la clé
	      est identique
&EN SCR_FIRST: recherche le premier record de la database dans l'ordre
    de l'index sans tenir compte de la valeur de l'index
&EN SCR_LAST: recherche le dernier record de la database dans l'ordre
    de l'index sans tenir compte de la valeur de l'index
&EN SCR_DB : recherche le premier record de la database dans l'ordre
    physique des enregistrements (méthode la plus rapide pour lire tout un
    fichier).

Si la recherche aboutit, le record est placé dans l'ISAM et le pointeur
de l'ISAM est placé à cette position de l'ISAM pour les recherches
ultérieures (IS_next(), IS_prev()) et pour les rewrites (IS_rewrite()).

La READ_FN de l'ISAM est exécutée après lecture du record.

&RT 0 si un record est trouvé et que la READ_FN de l'ISAM retourne 0, -1
sinon.
&EX
    PrintClientsCommune(commune)
    char    *commune;
    {
	char    name[51];

	if(IS_open_files(is_client, 0L) return(-1);
	IS_set_field(is_client, 3, commune);
	if(OBJ_search(is_client_commune_i, SCR_EQ) != 0) {
	    IS_close(is_client);
	    return(-1);
	    }
	printf("Clients habitant %s\n\n", commune);

	while(1) {
	    IS_get_field(is_client, 0, name);
	    printf("%s\n", name);
	    if(IS_next(is_client)) break;
	    }

	IS_close(is_client);
	return(0);
    }
&TX
&SA IS_next(), IS_prev(), IS_search()
=======================================================================*/

OBJ_search(obj, cond)
OBJ     obj;
int     cond;
{
    /* BP_M 03-10-97 */
    return(IS_search((ISAM *)obj.obj_ptr, obj.obj_fld, cond, (char *)0));
}


/* ======================================================================
Retourne un pointeur vers la valeur du champ d'ISAM défini par l'objet obj.
La valeur est la valeur du record courant de l'ISAM.
&EX
    printf("Nom : %s\n", IS_text(is_client_name));
&TX
&RT pointeur vers le champ dans le record de l'ISAM
&SA PG_text(), SCR_get_text()
------------------------------------------------------------------------- */
char *IS_text(obj)
OBJ     obj;
{
    ISAM    *is;

    is = ((ISAM *)obj.obj_ptr);
    return(is->is_rec + is->is_flds[obj.obj_fld].isf_start);
}



/* ======================================================================
Remplace un lien entre le champ d'une PAGE et un champ d'un ISAM. obj est
l'objet définissant le champ de la PAGE, isf celui définissant le champ
de l'ISAM. Un champ d'une page pouvant être lié à plusieurs champs
d'ISAM, nb en indique la position.

&NO Il s'agit du remplacement d'un lien par un autre! Un lien doit donc
déjà exister au départ dans les sources de la PAGE.

&EX
    page.f :

    PAGE pg_art {
	...
	FIELD ISAM is_art taille NAME taille DEF_OBJ
	...
	}

    source C :

    ShiftTaille(i)
    int     i;
    {
	OBJ     iobj = is_art_taille;

	iobj.obj_fld += i;
	FLD_set_isam(pg_art_taille, 0, iobj);
    }
&TX
&SA FLD_change_isf()
------------------------------------------------------------------------ */
FLD_set_isam(obj, nb, isf)
OBJ     obj;
int     nb;
OBJ     isf;
{
    FIELD   *fld = ((PAGE *)obj.obj_ptr)->pg_flds + obj.obj_fld;

    fld->fld_isam[nb].if_isam    = SCR_NU(isf.obj_ptr);
    fld->fld_isam[nb].if_isam_nb = isf.obj_fld;
    return(0);
}

/* ======================================================================
Remplace le type de vérification à opérer sur un champ lors du SCAN
d'une PAGE dans un ISAM. Trois types de recherche sont valables :

&EN SCR_ISF_CHECK       : vérifie uniquement si le champ n'est pas vide
&EN SCR_ISF_CHECKALWAYS : vérifie toujours
&EN SCR_ISF_CHECKNEVER  : ne vérifie jamais

&NO Les champs doivent se trouver en mémoire pour que cette opération
    puisse avoir lieu, soit via FLD_MEM dans la définition, soit
    en cours d'édition de la PAGE, soit via la fonction PG_read_flds(pg);

&EX
    page.f :

    PAGE pg_art {
	...
	FIELD ISAM is_art taille NAME taille DEF_OBJ ISF_CHECKALWAYS
	...
	}

    source C :

    DoNotCheckTaille()
    {
	FLD_set_check(pg_art_taille, 0, SCR_ISF_CHECKNEVER);
    }
&TX
&SA FLD_change_isf()
------------------------------------------------------------------------ */
FLD_set_check(obj, nb, chk)
OBJ     obj;
int     nb, chk;
{
    FIELD   *fld = ((PAGE *)obj.obj_ptr)->pg_flds + obj.obj_fld;

    fld->fld_check = chk - SCR_ISF_CHECK;
    return(0);
}



