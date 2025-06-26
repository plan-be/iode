#include "scr.h"

extern int  IS_REC_RREC;

/* ======================================================================
Lit la valeur du ième champ qui suit l'objet passé comme argument. Cet
objet peut faire partie d'une PAGE ou d'un ISAM.

Il n'y a pas de vérification de type! Vérifiez donc soigneusement le
type des champs utilisés.

Cette fonction doit avoir connaissance de l'OBJ utilisé. Il faut donc
placer dans le champ en question DEF_OBJ pour en obtenir la définition
dans les fichiers .c générés par scr4_e.

&NO
L'assignation directe est une méthode qui demande moins de code et ne
nécessite pas de définition des OBJ. De plus, les conversions de types
sont gérées par la compilateur C. Par conséquent, on utilisera les
assignations par valeurs directes lorsque c'est possible.

&EX
    GetVals(obj, n, vals)
    OBJ     obj;
    int     n;
    char    *vals;
    {
	int     i;

	for(i = 0 ; i < n ; i++)
		vals[i] = SCR_get_vchar(obj, i);
    }

    SCR_get_vchar(is_client_langue, 0)

	équivaut à

    is_client_LANGUE

&TX
&SA SCR_set_vchar(), SCR_rget_vchar()
------------------------------------------------------------------------ */
SCR_get_vchar(obj, i)
OBJ     obj;
int     i;
{
    unsigned char    c[2];

    SCR_get_vtext(obj, i, c);
    return((int)c[0]);
}

/* ======================================================================
Lit la valeur du ième champ qui suit l'objet passé comme argument. Cet
objet peut faire partie d'une PAGE ou d'un ISAM.

&SA SCR_get_vchar(), SCR_set_vchar(), SCR_rget_vchar()
------------------------------------------------------------------------ */
double SCR_get_vdouble(obj, i)
OBJ     obj;
int     i;
{
    double  d;

    SCR_get_vtext(obj, i, &d);
    return(d);
}

/* ======================================================================
Lit la valeur du ième champ qui suit l'objet passé comme argument. Cet
objet peut faire partie d'une PAGE ou d'un ISAM.

&SA SCR_get_vchar(), SCR_set_vchar(), SCR_rget_vchar()
------------------------------------------------------------------------ */
float SCR_get_vfloat(obj, i)
OBJ     obj;
int     i;
{
    float   f;
    double  d;

    switch(obj.obj_type) {
	case SCR_TYPE_PAGE:
	    SCR_get_vtext(obj, i, &d);
	    f = d;
	    break;
	case SCR_TYPE_ISAM:
	    SCR_get_vtext(obj, i, &f);
	    break;
    }
    return(f);
}

/* ======================================================================
Lit la valeur du ième champ qui suit l'objet passé comme argument. Cet
objet peut faire partie d'une PAGE ou d'un ISAM.

&SA SCR_get_vchar(), SCR_set_vchar(), SCR_rget_vchar()
------------------------------------------------------------------------ */
long SCR_get_vlong(obj, i)
OBJ     obj;
int     i;
{
    long    l;

    SCR_get_vtext(obj, i, &l);
    return(l);
}

/* ======================================================================
Lit la valeur du ième champ qui suit l'objet passé comme argument. Cet
objet peut faire partie d'une PAGE ou d'un ISAM.

&SA SCR_get_vchar(), SCR_set_vchar(), SCR_rget_vchar()
------------------------------------------------------------------------ */
short SCR_get_vshort(obj, i)
OBJ     obj;
int     i;
{
    short   s;
    ISAM    *is;

    obj.obj_fld += i;
    if(obj.obj_type == SCR_TYPE_PAGE)
	s = SCR_get_long(obj);
    else {
	is = (ISAM *) obj.obj_ptr;
	switch(is->is_flds[obj.obj_fld].isf_type) {
	    case SCR_LONG  : s = SCR_get_long(obj); break;
	    case SCR_SHORT : SCR_get_text(obj, &s); break;
	    }
	}
    return(s);
}
/* ======================================================================
Fixe la valeur du ième champ qui suit l'objet passé comme argument. Cet
objet peut faire partie d'une PAGE ou d'un ISAM.

Il n'y a pas de vérification de type! Vérifiez donc soigneusement le
type des champs utilisés.

Cette fonction doit avoir connaissance de l'OBJ utilisé. Il faut donc
placer dans le champ en question DEF_OBJ pour en obtenir la définition
dans les fichiers .c générés par scr4_e.

&NO
L'assignation directe est une méthode qui demande moins de code et ne
nécessite pas de définition des OBJ. De plus, les conversions de types
sont gérées par la compilateur C. Par conséquent, on utilisera les
assignations par valeurs directes lorsque c'est possible.

&EX
    SetVals()
    {
	int     i;

	for(i = 0 ; i < 5 ; i++)
		SCR_set_vchar(pg_opts_opt1, i, 'Y');
    }

    SCR_set_vchar(is_client_langue, 0, 'F')

	équivaut à

    is_client_LANGUE = 'F'

&TX
&SA SCR_get_vchar(), SCR_rget_vchar()
------------------------------------------------------------------------ */
SCR_set_vchar(obj, i, val)
OBJ     obj;
int     i;
int     val;
{
    unsigned char c;        /* JMP 19-11-91 */

    c = val;
    SCR_set_vtext(obj, i, &c);
    return(0);
}

/* ======================================================================
Fixe la valeur du ième champ qui suit l'objet passé comme argument. Cet
objet peut faire partie d'une PAGE ou d'un ISAM.

&SA SCR_get_vchar(), SCR_set_vchar(), SCR_rget_vchar()
------------------------------------------------------------------------ */
SCR_set_vdouble(obj, i, d)
OBJ     obj;
int     i;
double  d;
{
    double dd = d;

    SCR_set_vtext(obj, i, (char *)&dd);
    return(0);
}

/* ======================================================================
Fixe la valeur du ième champ qui suit l'objet passé comme argument. Cet
objet peut faire partie d'une PAGE ou d'un ISAM.

&SA SCR_get_vchar(), SCR_set_vchar(), SCR_rget_vchar()
------------------------------------------------------------------------ */
SCR_set_vfloat(obj, i, f)
OBJ     obj;
int     i;
double  f;
{
    float ff = (float)f;

    if(obj.obj_type == SCR_TYPE_PAGE)   /* JMP 19-11-91 */
	SCR_set_vdouble(obj, i, (double)ff);
    else
	SCR_set_vtext(obj, i, (char *)&ff);

    return(0);
}

/* ======================================================================
Fixe la valeur du ième champ qui suit l'objet passé comme argument. Cet
objet peut faire partie d'une PAGE ou d'un ISAM.

&SA SCR_get_vchar(), SCR_set_vchar(), SCR_rget_vchar()
------------------------------------------------------------------------ */
SCR_set_vlong(obj, i, l)
OBJ     obj;
int     i;
long    l;
{
    long    ll = l;

    SCR_set_vtext(obj, i, &ll);
    return(0);
}

/* ======================================================================
Fixe la valeur du ième champ qui suit l'objet passé comme argument. Cet
objet peut faire partie d'une PAGE ou d'un ISAM.

&SA SCR_get_vchar(), SCR_set_vchar(), SCR_rget_vchar()
------------------------------------------------------------------------ */
SCR_set_vshort(obj, i, s)
OBJ     obj;
int     i;
short   s;
{
    short   ss = s;
    long    l;
    PAGE    *pg;

    obj.obj_fld += i;

    if(obj.obj_type == SCR_TYPE_PAGE) {
	pg = (PAGE *)obj.obj_ptr;
	if(pg->pg_flds[obj.obj_fld].fld_type == SCR_TYPE_MENU)
	    SCR_set_field_short(pg, obj.obj_fld, s);
	else {      /* BP_M 25-10-95 */
	    l = s;
	    SCR_set_long(obj, l);
	}
	return(0);     /* BP_M 25-10-95 */
    }
    SCR_set_text(obj, &ss);
    return(0);
}

/* ======================================================================
Lit la valeur de l'objet de PAGE passé comme argument. Cet objet doit être
un MENU.

&SA SCR_get_short()
------------------------------------------------------------------------ */
SCR_get_menu(obj)
OBJ     obj;
{
    int     i;

    i = SCR_get_field_short((PAGE *)obj.obj_ptr, obj.obj_fld);
    return(i);
}

/* ======================================================================
Fixe la valeur du champ de l'objet passé comme argument. Cet
objet peut faire partie d'une PAGE ou d'un ISAM.

&SA SCR_get_menu()
------------------------------------------------------------------------ */

SCR_set_menu(obj, i)
OBJ     obj;
int     i;
{
    SCR_set_short(obj, (int)i);
    return(0);
}

/* ======================================================================
Change le MENU associé au champ de l'objet passé comme argument. Cet
objet doit faire partie d'une PAGE.

&SA SCR_get_menu()
------------------------------------------------------------------------ */

PG_set_fld_menu(obj, mn)
OBJ     obj;
MENU    *mn;
{
     /* BP_M 03-11-93 12:37 */
    PAGE    *pg = (PAGE *)obj.obj_ptr;
    FIELD   *fld = pg->pg_flds + obj.obj_fld;

    fld->fld_menu = SCR_NU(mn);
    return(0);
}


