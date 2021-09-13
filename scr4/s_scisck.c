#include "scr.h"

/* ====================================================================
V‚rifie que le record courant de l'ISAM is satisfait aux conditions
exprim‚es par les deux records limites from et to.

Soit un champ de l'ISAM.

&EN S'il est vide dans from ET dans to, ce champ n'est pas retenu
    pour la v‚rification : quelle qu'en soit la valeur, elle sera
    consid‚r‚e comme bonne.

&EN S'il est d‚fini dans from OU dans to, sa valeur dans from est une
    borne inf‚rieure, sa valeur dans to est la borne sup‚rieure. Si la
    borne inf‚rieure est vide, tous les records inf‚rieurs ou ‚gaux … la
    seconde borne v‚rifient le critŠre. Si la borne sup‚rieure est vide,
    tous les records sup‚rieurs ou ‚gaux … la premiŠre borne seront
    valables. Si les deux bornes sont vides, tous les records seront
    retenus.

&RT 0 s'il y satisfait, -1 sinon
=======================================================================*/

SC_check_crec(is, from, to)
ISAM    *is;
char    *from, *to;
{
    char    *from_def, *to_def;
    int     success;

    /* CHECK IF FIELDS ARE EMPTY */
    SC_save_empty(is, from, to, &from_def, &to_def);

    /* Check le record */
    success = SC_check_rec(is, from, to, from_def, to_def);
    SCR_free(from_def);
    return(success);
}

