#include "scr.h"

/*NH*/
PG_reset_01(pg, flag) /* JMP 20-08-97 */
PAGE    *pg;
int     flag;
{
    TPAGES  *tpg;
    int     i;

    if(SCR_PTR_TYPE(pg) == SCR_TYPE_TPAGES) {
	tpg = (TPAGES *)pg;
	for(i = 0 ; i < tpg->tp_nb ; i++)
	    PG_reset_ch((PAGE *)SCR_PTR(tpg->tp_pgsnbs[i]), flag);
	}
    else
	PG_reset_ch(pg, flag);

    return(0);
}

/* ====================================================================
Vide la PAGE ou la TPAGES de son contenu en remettant toutes les valeurs
des champs à leur valeur par défaut. Ces valeurs sont 0 pour les champs
numériques, date, etc, et vides pour les champs de type caractère.
Cependant, si une valeur par défaut est définie pouru n champ donné,
c'est cette valeur qui y est placée.

Après cette opération, la RESET_FN est exécutée, permettant de modifier
les valeurs en fonction du contexte.

&SA PG_reset_all() et action PG_RESET
=======================================================================*/

PG_reset(pg)
PAGE    *pg;
{
    PG_reset_01(pg, 0);  /* JMP 20-08-97 */
    return(0);
}

/* ====================================================================
Vide la PAGE ou la TPAGES de son contenu en remettant toutes les valeurs
des champs à des valeurs vides ou nulles (à la différence de PG_reset()
qui place les valeurs par défaut s'il y en a). Ces valeurs sont 0 pour
les champs numériques, date, etc, et vides pour les champs de type
caractère.

Après cette opération, la RESET_FN est exécutée, permettant de modifier
les valeurs en fonction du contexte.

&SA PG_reset() et action PG_RESET
=======================================================================*/

PG_reset_all(pg)
PAGE    *pg;
{
    PG_reset_01(pg, 1);  /* JMP 20-08-97 */
    return(0);
}

/*NH*/
/* =+====== PG_RESET_CH ====================================
     Reset the default text of the fields.
     pg    : IO : pointer to the page
     return : pointer to the image or null if error
   ============================================================== */

PG_reset_ch(pg, flag)       /* BP_M 16-01-2003 12:10 */
PAGE    *pg;
int     flag;
{
    int     i;
    FIELD   *fld;

    PG_read_flds(pg);
    fld = pg->pg_flds;

    for(i = 0 ; i < pg->pg_nb_flds ; i++, fld++)
	PG_reset_field(pg, i, flag);

    ACT_exec_nu(pg->pg_reset_act);
    return(0);
}

PG_reset_field(PAGE *pg, int i, int flag)       /* BP_M 16-01-2003 12:10 */
{
    FIELD   *fld;

    fld = pg->pg_flds + i;

    if(flag == 0 && fld->fld_dtext != 0) {
	if(fld->fld_type == SCR_TYPE_MENU)
	    SCR_set_field_short(pg, i,
				SCR_search_menu(MENU_PTR(fld->fld_menu), fld->fld_dtext));
	else
	    PG_set_rec_txt(pg, i, fld->fld_dtext);
    }
    else
	PG_empty_field(pg, i);
    fld->fld_pv = 0;
}


