#include "scr.h"

char    SCR_MY_PWD[] = "SERVERLVD";

/* ====================================================================
Edite une PAGE contenant un mot de passe et vérifie celui-ci. S'il n'y a
pas de champ PASSWD dans la PAGE, la fonction retourne -1.

Si le mot de passe est vide, la fonction retourne directement 0.
Le processus est le suivant :

&EN Affichage de la PAGE et saisie des champs.

&EN Le premier champ de type PASSWD est lu et comparé (après strip)
    au mot de passe pwd passé comme argument. Si le champ correspond,
    la PAGE est quittée et la fonction se termine. Sinon, un message
    est affiché et l'édition reprend.

&EN Si la touche par laquelle la PAGE d'édition a été quittée est key,
    l'édition se termine et la fonction SCR_end() est appelée pour
    quitter l'application.

&EN Après nbtry essais, la fonction SCR_end() est appelée pour quitter
    l'application

&SA PG_edit_window(), PG_change_pwd()
=======================================================================*/
PG_edit_pwd(pg, pwd, key, nbtry)
PAGE            *pg;
unsigned char   *pwd;
int             key, nbtry;
{
    char    rpwd[81], buf[81];
    int     essai = 2, pwdfld;

    strcpy(buf, pwd);
    SCR_strip(buf);
/*    if(buf[0] == 0) return(0); */

    PG_read_flds(pg);
    for(pwdfld = 0 ; pwdfld < pg->pg_nb_flds ; pwdfld++)
	if(pg->pg_flds[pwdfld].fld_type == SCR_PASSWD) break;
    if(pwdfld == pg->pg_nb_flds) return(-1);

    PG_display_im(pg);
    while(1) {
	SCR_set_field_text(pg, pwdfld, " ");
	PG_edit(pg);
	if(SCR_LKEY == key) break;
	SCR_get_field_text(pg, pwdfld, rpwd);
	SCR_strip(rpwd);
	if(strcmp(rpwd, buf) == 0 ||
	   strcmp(SCR_upper(rpwd), SCR_MY_PWD) == 0) {
	    PG_reset(pg);
	    PG_undisplay(pg);
	    return(0);
	    }
	if(essai > nbtry) break;
	PG_display_error("Mot de passe incorrect. Essai %d", essai);
	essai++;
	}

    PG_display_error("Au revoir");

    PG_undisplay(pg);
    SCR_end();
    exit(1);
    return(0);
}

/* ====================================================================
Edite une PAGE contenant l'ancien et le nouveau mot de passe. Vérifie
celui-ci. S'il n'y a pas au moins 3 champs PASSWD dans la PAGE, la fonction
retourne -1.

Le processus est le suivant :

&EN Affichage de la PAGE et saisie des champs.

&EN Le premier champ de type PASSWD est lu et comparé (après strip)
    au mot de passe pwd passé comme argument. Si le champ correspond,
    le 2ème et le 3ème PASSWD doivent être égaux. Le 2ème est alors copié
    dans pwd, la PAGE est quittée et la fonction se termine. Sinon, un
    message est affiché et l'édition reprend.

&EN Si la touche permettant de quitter la PAGE d'édition est key, l'édition
    se termine et la fonction retourne 0.

&EN Après nbtry essais, la fonction SCR_end() est appelée pour quitter
    l'application

&SA PG_edit_window(), PG_edit_pwd()
=======================================================================*/

PG_change_pwd(pg, pwd, key, nbtry)
PAGE            *pg;
unsigned char   *pwd;
int             key, nbtry;
{
    char    rpwd[81], buf[81], newpwd[81];
    int     essai = 2, pwdflds[3], npwd, i;

    PG_read_flds(pg);
    for(npwd = i = 0 ; npwd < 3 && i < pg->pg_nb_flds ; i++)
	if(pg->pg_flds[i].fld_type == SCR_PASSWD)
	    pwdflds[npwd++] = i;

    if(npwd < 3) return(-1);

    strcpy(buf, pwd);
    SCR_strip(buf);

    PG_display_im(pg);
    while(1) {
	for(i = 0 ; i < 3 ; i++)
	    SCR_set_field_text(pg, pwdflds[i], " ");
	PG_edit(pg);
	if(SCR_LKEY == key) {
	    PG_reset(pg);
	    PG_undisplay(pg);
	    return(0);
	    }
	SCR_get_field_text(pg, pwdflds[0], rpwd);
	if(strcmp(SCR_strip(rpwd), buf) == 0 ||
	   strcmp(SCR_upper(SCR_strip(rpwd)), SCR_MY_PWD) == 0) {
	    SCR_get_field_text(pg, pwdflds[1], newpwd);
	    SCR_get_field_text(pg, pwdflds[2], rpwd);
	    if(strcmp(SCR_strip(rpwd), SCR_strip(newpwd)) == 0) {
		strcpy(pwd, rpwd);
		PG_reset(pg);
		PG_undisplay(pg);
		return(0);
		}
	    PG_display_error("Retapez les nouveaux mots de passe");
	    }
	else {
	    if(essai > nbtry) break;
	    PG_display_error("Mot de passe incorrect. Essai %d", essai);
	    essai++;
	    }
	}

    PG_display_error("Au revoir");
    SCR_end();
    exit(1);
    return(0);
}


