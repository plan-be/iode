#include "scr.h"

/* ====================================================================
Lit tout le fichier is dans l'ordre de l'index cmp_nb … partir du
premier record qui satisfait aux contraintes indiqu‚es dans la page pg.
Pour chaque record s‚lectionn‚, la fonction utilisateur fn est appel‚e,
permettant un traitement quelconque de ce record. Pour les records
rejet‚s, la fonction est ‚galement appel‚, avec un paramŠtre indiquant
que le record est rejet‚.

Si disp n'est pas nul, un compteur indiquant le nombre de records lus et
s‚lectionn‚s apparaŒt dans le bas de l'‚cran. Ce compteur est rafraŒchi
toutes les disp lectures. Si disp vaut 0, ce compteur n'apparaŒt pas.

Si stop vaut 1, la frappe de n'importe quelle touche en cours de
traitement arrˆte celui-ci et il est demand‚ … l'utilisateur s'il faut
ou non continuer. Si stop vaut 0, le processus se poursuit jusqu'… la
fin du fichier.

rev permet, s'il vaut 1, d'inverser la s‚lection de records : tous ceux
qui r‚pondent aux critŠres sont rejet‚s, tous les autres sont
s‚lectionn‚s.

La PAGE pg peut contenir un nombre quelconque de champs de l'ISAM is.
Pour chaque champ de is, on trouvera 0, 1 ou 2 occurences dans la PAGE.

La s‚lection des records s'effectue comme suit : tous les champs de
l'ISAM pr‚sents dans la PAGE servent de "masque", de critŠre de s‚lection.

Soit un champ de l'ISAM.

&EN S'il n'apparaŒt pas dans la PAGE, ce champ ne servira pas de filtre :
    quelle qu'en soit la valeur, elle sera consid‚r‚e comme bonne.

&EN S'il apparaŒt une fois, soit il est sp‚cifi‚, soit il ne l'est pas
    (valeur nulle ou blancs). Lorsqu'il est sp‚cifi‚, seuls les records
    ayant la mˆme valeur pour le champ seront s‚lectionn‚s. S'il n'est
    pas sp‚cifi‚, toute valeur est bonne pour ce champ.

&EN S'il apparaŒt 2 fois, la premiŠre valeur sert de borne inf‚rieure de
    s‚lection, la derniŠre comme borne sup‚rieure. Si la borne
    inf‚rieure est vide, tous les records inf‚rieurs ou ‚gaux … la
    seconde borne seront retenus. Si la borne sup‚rieure est vide, tous
    les records sup‚rieurs ou ‚gaux … la premiŠre borne seront repris.
    Si les deux bornes sont vides, tous les records seront retenus.

La s‚lection s'effectue en ne retenant que les records qui satisfont …
TOUS les critŠres d‚finis dans la PAGE pg.

La fonction utilisateur fn est du type suivant :
&CO
    int fn(is, nbread, nbfound, success)
    ISAM    *is;
    long    nbread, nbfound;
    int     success;
    {
	return(0);
    }
&TX
Si le pointeur de fonction fn est nul, il n'y a pas d'appel de fonction.

&EN is est le pointeur vers l'ISAM trait‚.
&EN nbread et nbfound sont respectivement le nombre de records lus et
    s‚lectionn‚s.
&EN success est 0 en cas de rejet du record, 1 en cas de succŠs, 2 … la
    fin du fichier.

La fonction fn est appel‚e aprŠs la lecture de chaque record,
s‚lectionn‚ ou non. De plus, elle est appel‚e aprŠs le dernier record,
permettant au programme de fermer des fichiers, de terminer une
impression, etc.

&TI Valeur de retour de fn
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

La fonction fn peut retourner 3 valeurs diff‚rentes :

&EN 0 : la lecture peut continuer
&EN 1 : la lecture continue, mais la fonction a rejet‚ la s‚lection. Le
    compteur s'adaptera en cons‚quence.
&EN toute autre valeur indique que la lecture soit s'arrˆter.

&EX
    PrintClients_1(is, nbread, nbfound, success)
    ISAM    *is;
    long    nbread, nb_found;
    int     success;
    {
	switch(success) {
	    case 2 :
		PR_print("Total : %ld clients\n", nbfound);
		return(0);

	    case 1 :
		if(is_clients_MONTANT < 1000) return(1);
		if(nbfound % 55 == 1) {
		    if(nbfound > 1) PR_formfeed();
		    PR_string("Nom             Chiffre Affaire\n\n");
		    }
		PR_string("%15.15s %ld FB\n",
			    is_clients_NAME, is_clients_MONTANT);
		return(0);

	    default : return(0);
	    }
    }

    PrintClients()
    {
	PG_edit_window(p_clients);
	if(SCR_LKEY != SCR_F10) return;
	PR_select(pr1);
	SC_scan_page(p_clients, is_clients, 0, PrintClients_1, 1);
	PR_send();
    }
&TX

&RT le nombre de records s‚lectionn‚s (long).
&SA SC_scan_isam()
=======================================================================*/

long SC_scan_page_gn(pg, is, cmp_nb, fn, disp, stop, rev)
PAGE    *pg;
ISAM    *is;
int     cmp_nb;
int     (*fn)();
int     disp, stop, rev;
{
    long    total = 0L;
    char    *from, *to, *buf, *fromd, *tod;

    /* SAVE CRIT INTO FROM AND TO */
    SC_save_crit(pg, is, &from, &to);           /* JMP 14-06-91 */

    /* SCAN FILE */
    SC_save_empty(is, from, to, &fromd, &tod); /* JMP 14-06-91 */
    SC_pg_setcheck2(pg, is, fromd);
    SC_pg_setcheck(pg, is, tod);
    total = SC_scan_isam_gn_chk(is, from, to, cmp_nb, fn, SCR_UPDATE, disp, stop, rev, fromd, tod);
    SCR_free(fromd);

    /* FREE */
    SCR_free(from);
    SCR_free(to);

    return(total);
}

/* ======================================================================
Fonction de lecture d'un ISAM et de s‚lection de records. Cette fonction
est identique … SC_scan_page_gn() avec les paramŠtres stop et rev de
cette derniŠre fix‚s … 1 et 0 :
&CO

    SC_scan_page(pg, is, cmp_nb, fn, disp)

	est identique …

    SC_scan_page_gn(pg, is, cmp_nb, fn, disp, 1, 0)
&TX
&RT le nombre de records s‚lectionn‚s (long)
&SA SC_scan_page_gn(), SC_scan_isam()
------------------------------------------------------------------------ */
long SC_scan_page(pg, is, cmp_nb, fn, disp)
PAGE    *pg;
ISAM    *is;
int     cmp_nb;
int     (*fn)();
int     disp;
{
    return(SC_scan_page_gn(pg, is, cmp_nb, fn, disp, 1, 0));
}

/*NH*/
SC_save_crit(pg, is, from, to)
PAGE    *pg;
ISAM    *is;
char    **from, **to;
{
    char    *buf;

    /* SAVE CRIT INTO FROM AND TO */
    *from = SCR_malloc(is->is_rec_len);
    *to   = SCR_malloc(is->is_rec_len);
    buf = is->is_rec;
    is->is_rec = *from;
    SC_page_to_isam2(pg, is);
    is->is_rec = *to;
    SCR_page_to_isam(pg, is);
    is->is_rec = buf;
    return(0);
}


/* ====================================================================
V‚rifie que le record courant de l'ISAM is satisfait aux conditions
exprim‚es dans la PAGE pg. Voir SC_scan_page_gn() pour une discussion
sur la s‚lection des records en fonction des valeurs des champs de la
PAGE pg.

&RT 0 s'il y satisfait, -1 sinon
&SA SC_check_crec(), SC_scan_page_gn()
=======================================================================*/

SC_check_pg_crec(pg, is)
PAGE    *pg;
ISAM    *is;
{
    char    *from, *to, *from_def, *to_def;
    int     success;

    SC_save_crit(pg, is, &from, &to);
    SC_save_empty(is, from, to, &from_def, &to_def);          /* JMP 23-05-97 */
    SC_pg_setcheck2(pg, is, from_def);                        /* JMP 23-05-97 */
    SC_pg_setcheck(pg, is, to_def);                           /* JMP 23-05-97 */
    success = SC_check_rec(is, from, to, from_def, to_def);   /* JMP 23-05-97 */
    SCR_free(from_def);                                       /* JMP 23-05-97 */
    SCR_free(from);
    SCR_free(to);
    return(success);
}

/*NH*/
SC_page_to_isam2(pg, is)
PAGE    *pg;
ISAM    *is;
{
    FIELD   *fld;
    ISFLD   *isf;
    int     i;
    int     j;

    for(i = pg->pg_nb_flds - 1 ; i >= 0 ; i--) {
	fld = pg->pg_flds + i;
	isf  = fld->fld_isam;
	for(j = 0 ; j < fld->fld_nb_isam ; j++, isf++)
	    if(SCR_PTR(isf->if_isam) == (char *)is)
		SCR_fld_to_isf(pg, i, is, isf->if_isam_nb);
	}

    return(0);
}

/*NH*/
SC_pg_setcheck2(pg, is, def)
PAGE    *pg;
ISAM    *is;
char    *def;
{
    int     i;

    for(i = pg->pg_nb_flds - 1 ; i >= 0 ; i--)
	SC_fld_setcheck(pg, is, def, i);
    return(0);
}

/*NH*/
SC_pg_setcheck(pg, is, def)
PAGE    *pg;
ISAM    *is;
char    *def;
{
    int     i;

    for(i = 0 ; i < pg->pg_nb_flds ; i++)
	SC_fld_setcheck(pg, is, def, i);
    return(0);
}

/*NH*/
SC_fld_setcheck(pg, is, def, fld_nb)    /* JMP 23-05-97 */
PAGE    *pg;
ISAM    *is;
int     fld_nb;
char    *def;
{
    FIELD   *fld;
    ISFLD   *isf;
    int     j, isf_nb;

    fld = pg->pg_flds + fld_nb;
    isf = fld->fld_isam;
    for(j = 0 ; j < fld->fld_nb_isam ; j++, isf++) {
	if(SCR_PTR(isf->if_isam) != (char *)is) continue;
	isf_nb = isf->if_isam_nb;
	def[isf_nb] &= 1;
	def[isf_nb] += 2 * fld->fld_check;
	}
    return(0);
}


