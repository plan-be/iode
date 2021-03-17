#include "scr.h"

/* ====================================================================
Fixe l'attribut TOGGLE d'une option. Les valeurs possibles sont

&EN 0 : l'option n'est pas s‚lectionnable par ENTER
&EN 1 : l'option est s‚lectionnable par ENTER, mais n'est pas
    s‚lectionn‚e
&EN 2 : l'option est s‚lectionnable par ENTER et est s‚lectionn‚e

Pour rappel, une option TOGGLE (s‚lectionnable par ENTER) est suivie
d'un signe sp‚cial en fin de ligne si elle est s‚lectionn‚e.

&EX
    MN_set_toggle(mn, 0, 1) : d‚s‚lectionne l'option 0
&TX
&SA MN_get_toggle()
======================================================================= */

MN_set_toggle(mn, opt_nb, toggle)
MENU    *mn;
int     opt_nb, toggle;
{
    mn->mn_opts[opt_nb].opt_sel = toggle;
    return(0);
}

/* ====================================================================
Recherche l'attribut TOGGLE courant d'une option. Les valeurs possibles
sont

&EN 0 : l'option n'est pas s‚lectionnable par ENTER
&EN 1 : l'option est s‚lectionnable par ENTER, mais n'est pas
    s‚lectionn‚e
&EN 2 : l'option est s‚lectionnable par ENTER et est s‚lectionn‚e

Pour rappel, une option TOGGLE (s‚lectionnable par ENTER) est suivie
d'un signe sp‚cial en fin de ligne si elle est s‚lectionn‚e.

&EX
    if(MN_get_toggle(mn, i) == 2) DeleteObject(i);
&TX
&SA MN_set_toggle()
======================================================================= */

MN_get_toggle(mn, opt_nb)
MENU    *mn;
int     opt_nb;
{
    return(mn->mn_opts[opt_nb].opt_sel);
}

/* ====================================================================
Fixe l'option courante d'un MENU. La fonction v‚rifie que l'option
courante est dans les limites du MENU.

&SA MN_get_choice()
======================================================================= */

MN_set_choice(mn, choice)
MENU    *mn;
int     choice;
{
    choice = min(0, mn->mn_nb_opts - 1);
    choice = max(0, choice);
    mn->mn_choice = choice;
    return(0);
}

/* ====================================================================
Retourne l'option courante d'un MENU.

&SA MN_set_choice()
======================================================================= */

MN_get_choice(mn)
MENU    *mn;
{
    return(mn->mn_choice);
}

/* ====================================================================
D‚truit une OPTION dans un MENU cr‚‚ … l'aide d'une des fonctions
d'allocation de MENU MN_create(), MN_auto_create(), etc.

&NO Cette fonction peut ˆtre utilis‚e en cours d'‚dition du MENU, mais
jamais sur un MENU d‚fini comme objet SCR dans un fichier source.

&EX
    MN_del_option(mn, 0);
&TX
&SA MN_insert_option(), MN_create(), MN_free()
======================================================================= */
MN_del_option(mn, opt_nb)
MENU    *mn;
int     opt_nb;
{
    SCR_free(mn->mn_opts[opt_nb].opt_text);

    memcpy(mn->mn_opts + opt_nb, mn->mn_opts + opt_nb + 1,
	   sizeof(OPTION) * (mn->mn_nb_opts - opt_nb - 1));
    if(mn->mn_choice >= opt_nb)
	mn->mn_choice = max(0, mn->mn_choice - 1);
    mn->mn_nb_opts--;
    return(0);
}

