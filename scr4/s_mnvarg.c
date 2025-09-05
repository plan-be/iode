#define SCRSUPPROTO
#include "scr.h"

/* ====================================================================
Insère une OPTION dans un MENU créé à l'aide d'une des fonctions
d'allocation de MENU MN_create(), MN_auto_create(), etc.

L'option se place en position opt_nb. Pour ajouter en début de MENU,
utiliser opt_nb = 0. Le texte de l'option est alloué dans la fonction.
La fonction sprintf est utilisée pour construire le texte.

&NO Cette fonction peut être utilisée en cours d'édition du MENU, mais
jamais sur un MENU défini comme objet SCR dans un fichier source.

&EX
    MN_insert_option(mn, 0, "%20s - %ld", client, montant);
&TX
======================================================================= */

OPTION *MN_insert_option(mn, opt_nb, text, s2, s3, s4, s5, s6, s7)
MENU    *mn;
int     opt_nb;
char    *text, *s2, *s3, *s4, *s5, *s6, *s7;
{
    int     i;

    opt_nb = _min_(opt_nb, mn->mn_nb_opts);
    opt_nb = _max_(opt_nb, 0);
    MN_add_option(mn);
    for(i = mn->mn_nb_opts - 1 ; i > opt_nb; i--)
	memcpy(mn->mn_opts + i, mn->mn_opts + i - 1, sizeof(OPTION));

    MN_set_option_text(mn, opt_nb, text, s2, s3, s4, s5, s6, s7);
    return(mn->mn_opts + opt_nb);       /* JMP 4.23 19-12-95 */
}


/* ====================================================================
Ajoute une OPTION à la fin d'un MENU créé à l'aide d'une des fonctions
d'allocation de MENU MN_create(), MN_auto_create(), etc.

Le texte de l'option est alloué dans la fonction. La fonction sprintf
est utilisée pour construire le texte.

&NO Cette fonction peut être utilisée en cours d'édition du MENU, mais
jamais sur un MENU défini comme objet SCR dans un fichier source.

&EX
    MN_add_set_option_text(mn, "%20s - %ld", client, montant);
&TX
&SA MN_insert_option(), MN_del_option(), MN_create(), MN_free()
======================================================================= */

MN_add_set_option_text(mn, s1, s2, s3, s4, s5, s6, s7)
MENU    *mn;
char    *s1, *s2, *s3, *s4, *s5, *s6, *s7;
{
    MN_add_option(mn);
    MN_set_option_text(mn, mn->mn_nb_opts - 1, s1, s2, s3, s4, s5, s6, s7);
    return(0);
}
/* =+====== MN_SET_OPTION_TEXT ====================================
Change le texte d'une OPTION d'un MENU créé à l'aide d'une des
fonctions d'allocation de MENU MN_create(), MN_auto_create(), etc.

Le texte de l'option est alloué dans la fonction. La fonction sprintf
est utilisée pour construire le texte.

&NO Cette fonction peut être utilisée en cours d'édition du MENU, mais
jamais sur un MENU défini comme objet SCR dans un fichier source.

&EX
    MN_set_option_text(mn, 3, "%20s - %ld", client, montant);
&TX
&SA MN_insert_option(), MN_del_option(), MN_create()
======================================================================= */

MN_set_option_text(mn, opt_nb, s1, s2, s3, s4, s5, s6, s7)
MENU    *mn;
int     opt_nb;
char    *s1, *s2, *s3, *s4, *s5, *s6, *s7;
{
    OPTION  *opt;
    char    buf[4096];

    opt = mn->mn_opts + opt_nb;
    SCR_free(opt->opt_text);
    sprintf(buf, s1, s2, s3, s4, s5, s6, s7);
    opt->opt_text = (unsigned char *)SCR_stracpy(buf);
    return(0);
}

/* =+====== MN_set_title ==============================================
    Change ou fixe le titre d'un MENU créé à l'aide d'une des fonctions
    d'allocation de MENU MN_create(), MN_auto_create(), etc.

    L'espace mémoire de l'ancien titre est libéré.
    La fonction fait appel à sprintf() pour construire le titre.

&NO Cette fonction peut être utilisée en cours d'édition du MENU, mais
jamais sur un MENU défini comme objet SCR dans un fichier source.

&EX
    MN_set_title(mn, "Name : %s, %s", name, firstname);
&TX
&SA MN_set_footnote()
====================================================================== */

MN_set_title(mn, s1, s2, s3, s4, s5, s6, s7)
MENU    *mn;
char    *s1, *s2, *s3, *s4, *s5, *s6, *s7;
{
    char    buf[1024]; /* JMP 10-01-11 */

    SCR_free(mn->mn_title);
    sprintf(buf, s1, s2, s3, s4, s5, s6, s7);
    mn->mn_title = (unsigned char *)SCR_stracpy(buf);
    return(0);
}

/* =+====== MN_set_footnote =====================================
    Change ou fixe la footnote d'un MENU créé à l'aide d'une des
    fonctions d'allocation de MENU MN_create(), MN_auto_create(), etc.

    L'espace mémoire de l'ancienne footnote est libéré.
    La fonction fait appel à sprintf() pour construire le texte.

&NO Cette fonction peut être utilisée en cours d'édition du MENU, mais
jamais sur un MENU défini comme objet SCR dans un fichier source.

&EX
    MN_set_footnote(mn, "Total : %ld lignes", nb_lines);
&TX
&SA MN_set_title()
================================================================== */

MN_set_footnote(mn, s1, s2, s3, s4, s5, s6, s7)
MENU    *mn;
char    *s1, *s2, *s3, *s4, *s5, *s6, *s7;
{
    char    buf[1024];

    SCR_free(mn->mn_foot);                          /* JMP38 02-10-92 */
    sprintf(buf, s1, s2, s3, s4, s5, s6, s7);
    mn->mn_foot = (unsigned char *)SCR_stracpy(buf);
    return(0);
}
