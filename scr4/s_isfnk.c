#include "scr4_str.h"

FNK_is_fnk(fks, key)
FNKEYS  *fks;
int     key;
{
    int     i;

    if(fks == 0) return(-1);
    for(i = 0 ; i < fks->fk_nb_fnks ; i++) {
	if(SCR_check_prot_vt(fks->fk_fnks[i].fnk_prot)) continue;
	if(fks->fk_fnks[i].fnk_key == key) return(i);
    }
    return(-1);
}

