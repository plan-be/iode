#include "scr.h"


/* ====================================================================
Lit l'ISAM is dans l'ordre de l'index cmp_nb … partir du premier record qui
satisfait aux contraintes indiqu‚es par les deux records limites from et to.

Pour chaque record s‚lectionn‚, la fonction utilisateur fn est appel‚e,
permettant un traitement quelconque de ce record. Pour les records
rejet‚s, la fonction est ‚galement appel‚, avec un paramŠtre indiquant que
le record est rejet‚.

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

&TI S‚lection des records
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
La s‚lection des records s'effectue comme suit : tous les champs de
l'ISAM d‚finis par une valeur non vide (nulle ou blanche) dans from ou
dans to servent de "masque", de critŠre de s‚lection.

Soit un champ de l'ISAM.

&EN S'il est vide dans from ET dans to, ce champ ne servira pas de filtre :
    quelle qu'en soit la valeur, elle sera consid‚r‚e comme bonne.

&EN S'il est d‚fini dans from OU dans to, la premiŠre valeur (from) sert de
    borne inf‚rieure de s‚lection, la derniŠre (to) comme borne
    sup‚rieure. Si la borne inf‚rieure est vide, tous les records
    inf‚rieurs ou ‚gaux … la seconde borne seront retenus. Si la borne
    sup‚rieure est vide, tous les records sup‚rieurs ou ‚gaux … la
    premiŠre borne seront repris. Si les deux bornes sont vides, tous
    les records seront retenus.

La s‚lection s'effectue en ne retenant que les records qui satisfont …
TOUS les critŠres d‚finis dans les records from et to.

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
	PR_select(pr1);
	SC_scan_isam_gn(p_clients, 0L, 0L, 0, PrintClients_1,
			SCR_READ, 1, 1, 0)
	PR_send();
    }
&TX

&RT le nombre de records s‚lectionn‚s (long).
&SA SC_scan_isam(), SC_scan_page_gn()
=======================================================================*/

long SC_scan_isam_gn(is, from, to, cmp_nb, fn, opent, disp, stop, rev)
ISAM    *is;
char    *from, *to;
int     cmp_nb;
int     (*fn)();
int     opent, disp, stop, rev;
{
    long    nb_found;
    char    *fromd, *tod;

    SC_save_empty(is, from, to, &fromd, &tod); /* JMP 14-06-91 */
    nb_found = SC_scan_isam_gn_chk(is, from, to, cmp_nb, fn, opent, disp, stop, rev, fromd, tod);
    SCR_free(fromd);
    return(nb_found);
}

/*NH*/
long SC_scan_isam_gn_chk(is, from, to, cmp_nb, fn, opent, disp, stop, rev, fromd, tod)
ISAM    *is;
char    *from, *to, *fromd, *tod;
int     cmp_nb;
int     (*fn)();
int     opent, disp, stop, rev;
{
    long    nb_read = 0L, nb_found = 0L;
    ISFIELD *isf;
    extern  int SCR_vtime;
    int     i, success, vtime = SCR_vtime, rdisp = 0, cond;/* BP_M 16-07-1998 */

    if(IS_open(is, opent, SCR_NO_LOCK)) goto fin;
    if(from != 0)
	memcpy(is->is_rec, from, is->is_rec_len);
    else
	IS_empty_rec(is);

    if(is->is_type_dbd > 0) { /* JMP 02-11-98 */
	if(to != 0)
	    memcpy(is->is_maxrec, to, is->is_rec_len);
	else
	    memset(is->is_maxrec, 0, is->is_rec_len);
    }

    if(cmp_nb < 0) {
	if(IS_search(is, 0, SCR_DB, (char *)0)) goto fin;
    }
    else {
	cond = SCR_GE;
	if(is->is_type_dbd > 0)  /* JMP 02-11-98 */
	    cond += SCR_MINMAX;
	if(IS_search(is, cmp_nb, cond, (char *)0)) goto fin;
    }

    SC_init_display_d(disp);        /* JMP 25-05-97 */
    rdisp = 1;
/*
    if(SC_PG == 0) {
	SC_PG = PG_read("scr_incr");
	PG_reset(SC_PG);
    }
*/

    if(stop) SCR_term_vkey(0);      /* JMP 17-02-92 */
    SCR_stop_alarm();
    while(1) {
	if(stop && SC_hit_key("Stop (O/N) ?")) break;
	nb_read ++;
	if(cmp_nb >= 0 && rev == 0 &&
	   SC_check_index(is, to, tod, cmp_nb, 1)) break; /* JMP 17-02-92 */
	success = SC_check_rec(is, from, to, fromd, tod);
	if(rev) success = !success;   /* JMP 17-02-92 */
	if(success == 0) nb_found++;
	if(fn != 0) { /* JMP 21-06-91 */
	    switch((*fn)(is, nb_read, nb_found, (success == 0) ? 1 : 0))
	    {
		case 1  : if(success == 0) nb_found--; break; /*4.04*/
		case -1 : if(success == 0) nb_found--; goto fin; /* JMP 04-06-05 */
		default : break;
	    }
	}
	if(disp && (nb_read - 1) % disp == 0) /* JMP 06-10-91 */
	    SC_incr_display(nb_read, nb_found);
	if(IS_next(is)) break;
    }

    if(fn != 0) {
	if((*fn)(is, nb_read, nb_found, 2)) goto fin;
    }
fin:
    SCR_restart_alarm();
    if(stop) SCR_term_vkey(vtime);

    IS_close(is);
    if(rdisp && disp) {
	SC_incr_display(nb_read, nb_found); /* JMP 26-06-91 */
	SC_end_display();
	}
    return(nb_found);
}

/* ======================================================================
Fonction de lecture d'un ISAM et de s‚lection de records. Cette fonction
est identique … SC_scan_isam_gn() avec les paramŠtres stop et rev de
cette derniŠre fix‚s … 1 et 0 :
&CO

    SC_scan_isam(is, from, to, cmp_nb, fn, open_type, disp)

	est identique …

    SC_scan_isam_gn(is, from, to, cmp_nb, fn, open_type, disp, 1, 0)
&TX
&RT le nombre de records s‚lectionn‚s (long)
&SA SC_scan_isam_gn(), SC_scan_page()
------------------------------------------------------------------------ */
long SC_scan_isam(is, from, to, cmp_nb, fn, open_type, disp)
ISAM    *is;
char    *from, *to;
int     cmp_nb;
int     (*fn)();
int     open_type, disp;
{
    return(SC_scan_isam_gn(is, from, to, cmp_nb, fn, open_type, disp, 1, 0));
}

/*NH*/
SC_save_empty(is, from, to, from_def, to_def)
ISAM    *is;
char    *from, *to, **from_def, **to_def;
{
    int     i;
    ISFIELD *isf;

    /* CHECK IF FIELDS ARE EMPTY */
    *from_def = SCR_malloc(is->is_nb_flds * 2);
    *to_def = *from_def + is->is_nb_flds;
    isf = is->is_flds;
    if(from != 0 && to != 0) {
	for(i = 0 ; i < is->is_nb_flds ; i++, isf++) {
	    (*from_def)[i] = IS_is_fld_empty(from + isf->isf_start, isf->isf_type, isf->isf_length);
	    (*to_def)[i]   = IS_is_fld_empty(to   + isf->isf_start, isf->isf_type, isf->isf_length);
	    }
	}
    return(0);
}





