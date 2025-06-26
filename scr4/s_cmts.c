#include "scr.h"

#define SCR_SCMT_MAX    20

int     SCR_scmt_nb = 0;
int     SCR_scmt_keys[SCR_SCMT_MAX];
char    *SCR_scmt_txts[SCR_SCMT_MAX];

/* ====================================================================
Définit le texte du commentaire.

Si txt est NULL, le commentaire annulé est effacé.
&EX
    SCR_scmt_txt("F3=Quitter F10=Sauver");
&TX
&SA SCR_comment(), SCR_scmt_add_key(), SCR_scmt_free(), SCR_scmt_display()
======================================================================= */

SCR_scmt_txt(txt)
char    *txt;
{
    SCR_strlcpy(SCR_CMT_TXT, txt, SCR_CMT.c_nc);
    return(0);
}

/* ====================================================================
Libère l'espace alloué par des appels précédents à SCR_scmt_add_key().
Cette fonction est appelée par SCR_comment().

&SA SCR_scmt_txt(), SCR_scmt_display(), SCR_scmt_add_key()
======================================================================= */

SCR_scmt_free()
{
    int     i;

    for(i = 0 ; i < SCR_scmt_nb ; i++) SCR_free(SCR_scmt_txts[i]);
    SCR_scmt_nb = 0;
    return(0);
}

/* ====================================================================
Ajoute une touche fonction dans la liste des touches définies. A cette
touche peut être associé un commentaire.

La fonction SCR_scmt_display() affiche le commentaire courant et
exploite les infos définies par SCR_scmt_add_key pour afficher
en video inverse le texte des touches fonctions.

La fonction SCR_scmt_menu() affiche les touches sous forme d'un
menu et recorde l'option choisie.

&SA SCR_scmt_txt(), SCR_scmt_display(), SCR_scmt_free(), SCR_scmt_menu(),
    SCR_scmt_set_keys()
======================================================================= */

SCR_scmt_add_key(key, txt)
int     key;
char    *txt;
{
    int     i;

    if(SCR_scmt_nb >= SCR_SCMT_MAX) return(0);
    for(i = 0 ; i < SCR_scmt_nb ; i++)
	if(key != 0 && SCR_scmt_keys[i] == key) return(0);
    SCR_scmt_keys[i] = key;
    SCR_scmt_txts[i] = SCR_stracpy(txt);
    SCR_scmt_nb++;
    return(0);
}

/* ====================================================================
Définit la table de touches fonctions et de commentaires structurés.
La table doit être terminée par un 0.

La fonction SCR_scmt_display() affiche le commentaire courant et
exploite les infos définies par SCR_scmt_add_keys pour afficher
en video inverse le texte des touches fonctions.

La fonction SCR_scmt_menu() affiche les touches sous forme d'un
menu et recorde l'option choisie.

&SA SCR_scmt_txt(), SCR_scmt_display(), SCR_scmt_free(), SCR_scmt_menu(),
    SCR_scmt_add_key()
======================================================================= */

SCR_scmt_set_keys(keys)
YYKEYS   *keys;
{
    int     i;

    for(i = 0 ; keys[i].yk_word ; i++)
	SCR_scmt_add_key(keys[i].yk_def, keys[i].yk_word);

    return(0);
}

/*NH*/
SCR_scmt_add_fnk(fnknu)
int             fnknu;
{
    FNKEYS  *fk;
    FNKEY   *fnk;
    int     j;

    if(fnknu <= 0) return(0);
    fk = (FNKEYS *)SCR_PTR(fnknu);
    fnk = fk->fk_fnks;
    for(j = 0 ; j < fk->fk_nb_fnks ; j++, fnk++)
	SCR_scmt_add_key(fnk->fnk_key, fnk->fnk_cmt);

    return(0);
}

/*NH*/
SCR_scmt_set_txt()
{
    int     i, lgs[SCR_SCMT_MAX], pos[SCR_SCMT_MAX];
    //char    buf[90];
    char    buf[1024];

    if(SCR_CMT.c_line < 0) return(0);

    if(SCR_CMT_TXT[0] == 0) {
	for(i = 0 ; i < SCR_scmt_nb ; i++) {
	    if(SCR_scmt_keys[i] == 0) continue;
	    SCR_scmt_txt_1(buf, SCR_scmt_keys[i], SCR_scmt_txts[i]);
	    if(strlen(SCR_CMT_TXT) + strlen(buf) < SCR_CMT.c_nc) {
		strcat(SCR_CMT_TXT, " ");
		strcat(SCR_CMT_TXT, buf);
		}
	    }
	}

    else {
	SCR_scmt_lgs_pos(lgs, pos);
	for(i = 0 ; i < SCR_scmt_nb ; i++) {
	    if(pos[i] >= 0) continue;
	    SCR_scmt_txt_1(buf, SCR_scmt_keys[i], SCR_scmt_txts[i]); /* BP_M 05-11-93 09:42 */
	    if(strlen(SCR_CMT_TXT) + strlen(buf) < SCR_CMT.c_nc) {
		strcat(SCR_CMT_TXT, " ");
		strcat(SCR_CMT_TXT, buf);
		}
	    }
	}

    return(0);
}

/*NH*/
SCR_scmt_txt_1(buf, key, txt)
int     key;
char    *txt, *buf;
{
    strcpy(buf, SCR_fnk_txt(key));
    if(txt) sprintf(buf + strlen(buf), "=%s", txt);
    return(0);
}

/*NH
    Calcule les positions et les longueurs des champs.
    Retourne le nombre affichable dans la fenêtre de comment.
*/

SCR_scmt_lgs_pos(lgs, pos)
int     *lgs, *pos;
{
    int     i;
    //char    buf[20], buf1[100];
    char    buf[512], buf1[1024]; /* JMP 10-01-11 */

    SCR_strlcpy(buf1, SCR_CMT_TXT, 90);
    SCR_upper(buf1);
    for(i = 0 ; i < SCR_scmt_nb ; i++) {
	if(SCR_scmt_keys[i] == 0) {
	    pos[i] = lgs[i] = -1;
	    }
	else {
	    strcpy(buf, SCR_fnk_txt(SCR_scmt_keys[i]));
	    SCR_upper(buf);
	    pos[i] = U_index(buf1, buf);
	    if(pos[i] < 0) lgs[i] = -1;
	    else           lgs[i] = strlen(buf);
	    }
	}
    return(0);
}










