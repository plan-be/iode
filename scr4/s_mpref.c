#include "scr.h"

/* =+=== MP_SET_REF ============================================
Fixe le champ du record de référence de la MPAGE mp. Cette
fonction a la même syntaxe et les mêmes limites que IS_set_field() :
aucune vérification de type ou de longueur n'est effectuée.

Le contenu du record de référence de la MPAGE détermine le début
de la recherche précédent l'affichage des records dans la MPAGE.
Lorsque la APG contient des champs, les valeurs de la APG sont
utilisées pour fixer les valeurs du record de référence.

Si ce n'est pas le cas, on peut déterminer à l'aide de cette fonction
le début de la recherche. Des "defines" sont définis comme dans le cas
des ISAM pour effectuer plus facilement la même opération.

&EX
    ISAM is_cmds {
	...
	FIELD long  NAME num_cli
	...
    }
    MPAGE mp_cmds {
	...
	ISAM is_cmds num_cli
	...
    }

    EditCommandesClient(num_cli)
    long    num_cli;
    {
	MP_set_ref(mp_cmds, 0, &num_cli);
	(ou mp_cmds_NUM_CLI = num_cli;)

	MP_edit_isam(mp_cmds);
    }
&TX

&SA IS_set_field()
============================================================= */

MP_set_ref(mp, fld_nb, val)
MPAGE   *mp;    /* MPAGE à traiter */
int     fld_nb; /* Numéro du champ de l'ISAM implicite */
char    *val;   /* Valeur à donner au champ */
{
    char    *is_rec;
    ISAM    *is  = (ISAM *)SCR_PTR(mp->mp_isam);

    is_rec = is->is_rec;
    is->is_rec = mp->mp_ref_rec;
    IS_set_field(is, fld_nb, val);
    is->is_rec = is_rec;
}



