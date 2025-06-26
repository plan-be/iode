#include "scr.h"

/* ====================================================================
Vérifie que le record courant de l'ISAM is satisfait aux conditions
exprimées par les deux records limites from et to.

Soit un champ de l'ISAM.

&EN S'il est vide dans from ET dans to, ce champ n'est pas retenu
    pour la vérification : quelle qu'en soit la valeur, elle sera
    considérée comme bonne.

&EN S'il est défini dans from OU dans to, sa valeur dans from est une
    borne inférieure, sa valeur dans to est la borne supérieure. Si la
    borne inférieure est vide, tous les records inférieurs ou égaux à la
    seconde borne vérifient le critère. Si la borne supérieure est vide,
    tous les records supérieurs ou égaux à la première borne seront
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

