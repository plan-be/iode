#include "scr.h"

/* =+=== MP_SET_REF ============================================
Fixe le champ du record de r‚f‚rence de la MPAGE mp. Cette
fonction a la mˆme syntaxe et les mˆmes limites que IS_set_field() :
aucune v‚rification de type ou de longueur n'est effectu‚e.

Le contenu du record de r‚f‚rence de la MPAGE d‚termine le d‚but
de la recherche pr‚c‚dent l'affichage des records dans la MPAGE.
Lorsque la APG contient des champs, les valeurs de la APG sont
utilis‚es pour fixer les valeurs du record de r‚f‚rence.

Si ce n'est pas le cas, on peut d‚terminer … l'aide de cette fonction
le d‚but de la recherche. Des "defines" sont d‚finis comme dans le cas
des ISAM pour effectuer plus facilement la mˆme op‚ration.

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
MPAGE   *mp;    /* MPAGE … traiter */
int     fld_nb; /* Num‚ro du champ de l'ISAM implicite */
char    *val;   /* Valeur … donner au champ */
{
    char    *is_rec;
    ISAM    *is  = (ISAM *)SCR_PTR(mp->mp_isam);

    is_rec = is->is_rec;
    is->is_rec = mp->mp_ref_rec;
    IS_set_field(is, fld_nb, val);
    is->is_rec = is_rec;
}



