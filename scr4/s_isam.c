#include "scr.h"
#include <time.h>

extern int SCR_NO_REREAD;

/*NH*/
long win_app32_segment() /* JMP 4.23 07-01-96 */
{
    long    ltime;

    time(&ltime);
    return(SCR_cvt_date(ltime));
}


/*NH*/
IS_search_db(is)
ISAM    *is;
{
    return(IS_search(is, 0, SCR_DB, (char *)0));     /* JMP 11-12-98 */
}

/* ====================================================================
Ouvre l'ISAM is. Le mode d'ouverture (mode) est SCR_READ pour une
lecture sans ‚criture, SCR_UPDATE ou SCR_WRITE pour des op‚rations
READ/WRITE.

Le paramŠtre lock est SCR_LOCK pour bloquer le fichier pour un usage
exclusif ou SCR_NO_LOCK pour une usage partag‚.

La fonction peut ˆtre appel‚e un nombre quelconque de fois : … part le
premiŠre fois, o— une ouverture est r‚ellement effectu‚e, un compteur
du nombre d'ouvertures est tenu … jour dans la structure ISAM. A chaque
IS_open(), ce compteur est incr‚ment‚, … chaque IS_close(), il est
d‚cr‚ment‚. Lorsque le compteur vaut 1, IS_close() ferme r‚ellement le
fichier.

De cette fa‡on, toute fonction peut en toute qui‚tude ouvrir
un ISAM au d‚but et le fermer … la fin, sans avoir d'effet sur les
autres ouvertures pr‚alables.

&RT 0 en cas de succŠs, -1 sinon.
&EX
    if(IS_open(is_clients, SCR_UPDATE, SCR_NO_LOCK))
	return(-1);
    ...
    IS_close(is_clients);
&TX
&SA IS_open_files(), IS_close()
=======================================================================*/

IS_open(is, mode, lock)
ISAM    *is ;
int     mode ;
int     lock ;
{
    int     nb_open = is->is_nb_open;

#ifdef PDEV     /* BP_M 20-10-95 */
    if(PG_pgvr()){
GMSGC;
	return(0);
	}
#endif
//    if(win_app32_segment() > 20041231L) exit(0); /* PROT_DATE */ /* JMP 4.25 14-01-96 */

    /* increment nb opening */
    /* is->is_nb_open++; /* JMP 03-05-00 */

    /* if not already allocated, creates the dependent isam struct */
    /* BP_M 04-11-1998 */
//  if(is->is_isam == 0) IS_create_struct(is);

    switch(is->is_open_type) {
	case SCR_WRITE :
	case SCR_UPDATE :
	    is->is_nb_open++;   /* JMP 03-05-00 */
	    return(0);
	case SCR_READ :
	    if(mode == SCR_READ) {
		is->is_nb_open++; /* JMP 03-05-00 */
		return(0);
		}
	    IS_close_isam(is);
	default :
	    if(IS_open_isam(is, mode, lock) != 0) {
		/* is->is_nb_open-- ; /* JMP 03-05-00 */
		is->is_nb_open = nb_open;
		return(-1);
		}
	    is->is_nb_open = nb_open + 1; /* JMP 03-05-00 */
	    is->is_open_type = mode;
	    is->is_cur_search = -1; /* JMP 11-12-98 */
	    return(0);
	}
}

/* ====================================================================
Ferme l'ISAM is.

La fonction peut ˆtre appel‚e un nombre quelconque de fois : un compteur
du nombre d'ouvertures est tenu … jour dans la structure ISAM de telle
sorte qu'… chaque IS_open(), ce compteur soit incr‚ment‚, et qu'… chaque
IS_close(), il soit d‚cr‚ment‚. Lorsque le compteur vaut 1, IS_close()
ferme r‚ellement le fichier. Les autres fois, IS_close() d‚cr‚mente le
compteur.

De cette fa‡on, toute fonction peut en toute qui‚tude ouvrir
un ISAM au d‚but et le fermer … la fin, sans avoir d'effet sur les
autres ouvertures pr‚alables.

&EX
    if(IS_open(is_clients, SCR_UPDATE, SCR_NO_LOCK))
	return(-1);
    ...
    IS_close(is_clients);
&TX
&SA IS_close_files(), IS_open()
=======================================================================*/

IS_close(is)
ISAM    *is;
{
    if(is->is_nb_open == 0) return(0);      /* JMP 4.23 08-01-96 */
    is->is_nb_open--;
    if(is->is_nb_open <= 0) IS_close_isam(is);
    return(0);                              /* JMP 4.23 08-01-96 */
}

/* ====================================================================
Ferme l'ISAM is physiquement. Le compteur de nombre d'ouvertures est
remis … 0.

&EX
    if(IS_open(is_clients, SCR_UPDATE, SCR_NO_LOCK))
	return(-1);
    ...
    IS_close_isam(is_clients);
&TX
&SA IS_close_files(), IS_close()
=======================================================================*/

IS_close_isam(is)
ISAM    *is;
{
    IS_close_db(is);
    is->is_nb_open = 0;
    is->is_open_type = 0;
    is->is_cur_search = -1;             /* JMP 11-12-98 */
    return(0);
}

/* ====================================================================
Ferme tous les ISAMS objets de SCR (pas ceux cr‚‚s par IS_dup()!) qui
sont actuellement ouverts par un appel … IS_close_isam().

Contrairement … IS_close_all(), ne d‚connecte pas le programme du
serveur (IS_end_database()) dans le cas d'un systŠme client/serveur (y
compris ODBC).

&RT Retourne le nombre d'ISAM ferm‚s.
&SA IS_close(), IS_close_isam(), IS_close_all()
=======================================================================*/
IS_close_all_isam()
{
    int     i, nb_close = 0;
    ISAM    *is;

    for(i = 0; i < SCR_READ_FILES.tbl_nb_fl ; i++)
	if(SCR_READ_FILES.tbl_fl[i]->fl_type == TYPE5(SCR_TYPE_ISAM)) {
	    is = (ISAM *)SCR_READ_FILES.tbl_fl[i]->fl_ptr;
	    if(is->is_nb_open == 0) continue;
	    IS_close_isam(is);
	    is->is_nb_open = 0;
	    is->is_open_type = 0;
	    nb_close++;
	}
    return(nb_close);
}

/* ====================================================================
Ferme tous les ISAMS objets de SCR (pas ceux cr‚‚s par IS_dup()!) qui
sont actuellement ouverts par un appel … IS_close_isam().

D‚connecte ‚galement le programme du serveur (IS_end_database()) dans le
cas d'un systŠme client/serveur (y compris ODBC). La fonction
IS_close_all_isam() ne d‚connecte pas de la database.

Cette fonction est utilis‚e lorsqu'un programme doit ˆtre quitt‚.
SCR_end() utilise cette fonction.

&RT Retourne le nombre d'ISAM ferm‚s.
&SA IS_close(), IS_close_isam(), IS_close_all_isam()
=======================================================================*/
IS_close_all()
{
    int     nb_close;

    nb_close = IS_close_all_isam(); /* JMP 15-12-98 */
    IS_end_database();
    return(nb_close);
}

/*NH*/
IS_is_open(is)
ISAM    *is;
{
    if(is->is_open_type == 0) return(0);
    else return(1);
}


/*NH*/
IS_search_ph(is, nu_index, cond, order)
ISAM     *is;
int      nu_index;
int      cond;
char     *order;
{
    int     err = 0;

    if(is->is_search_catg >= 0 && is->is_search_catg < SCR_USER_CATG) {
	PG_display_error(SCR_err_txt(113));
	return(-1);
	}
    if(IS_is_open(is) == 0) {                /* JMP 20-10-99 */
	if(IS_open_files(is, 0L)) return(-1);
	}
    if(cond == SCR_EQS) cond = SCR_EQ;
    IS_check_all_zstring(is);

    if(is->is_nb_cmps - 1 < nu_index) nu_index = is->is_nb_cmps - 1;
    if(nu_index < 0) cond = SCR_DB;

    if(ACT_exec_nu(is->is_search_act)) { /* JMP 16-02-96 */
	IS_set_err(201);                 /* JMP 16-02-96 */
	return(-1);                      /* JMP 16-02-96 */
	}                                /* JMP 16-02-96 */

    if(nu_index < 0 || is->is_nb_cmps == 0) {                                                 /* JMP_M 4.19 01-06-95 */
	if(0 != IS_search_rec(is, 0, SCR_DB, order)) return(-1);       /* JMP_M 4.19 01-06-95 */
	}                                                              /* JMP_M 4.19 01-06-95 */
    else if(0 != IS_search_rec(is, nu_index, cond, order)) return(-1); /* JMP_M 4.19 01-06-95 */


    return(IS_search_ph_end(is, cond)); /* JMP 04-01-99 */
}

/*NH*/
IS_search_ph_end(is, cond)
ISAM    *is;
int     cond;
{
    int     i = 0;

    IS_save_rec(is);
    IS_check_all_zstring(is);
    while(1)
	switch(ACT_exec_nu(is->is_read_act)) {
	    case 0:
		return(0);
	    case 1:
		if(cond == SCR_EQ) return(-1);

		if(IS_next_rec(is) != 0) {
		    for( ; i >= 0 ; i--)
			if(IS_prev_rec(is) != 0) break; /* BP_M 10-11-2008 11:29 */
		    return(-1);
		}
		i++;
		IS_check_all_zstring(is);
		IS_save_rec(is);
		break;
	    case -1:
		return(-1);
	}
}

/* ====================================================================
Poursuit une recherche dans l'ISAM is. La recherche fournit la suite de
celle entam‚e par dernier IS_search(). Elle a lieu suivant le mˆme
index, avec la mˆme condition de recherche.

Si la recherche aboutit, le record est plac‚ dans l'ISAM et le pointeur
de l'ISAM est plac‚ … cette position de l'ISAM pour les recherches
ult‚rieures (IS_next(), IS_prev()) et pour les rewrites (IS_rewrite()).

La READ_FN de l'ISAM est ex‚cut‚e aprŠs lecture du record.

&RT 0 si un record est trouv‚ et que la READ_FN de l'ISAM retourne 0, -1
sinon.
&EX
    PrintClientsCommune(commune)
    char    *commune;
    {
	char    name[51];

	if(IS_open_files(is_client, 0L) return(-1);
	IS_set_field(is_client, 3, commune);
	if(IS_search(is_client, 1, SCR_EQ, (char *)0) != 0) {
	    IS_close(is_client);
	    return(-1);
	    }
	printf("Clients habitant %s\n\n", commune);

	while(1) {
	    IS_get_field(is_client, 0, name);
	    printf("%s\n", name);
	    if(IS_next(is_client)) break;
	    }

	IS_close(is_client);
	return(0);
    }
&TX
&SA IS_search(), IS_prev()
=======================================================================*/

IS_next(is)
ISAM *is ;
{
    int     i = 0;

    if(ISB_check_search(is, SCR_IS_NEXT)) return(-1); /* JMP 11-12-98 */
    if(0 != IS_next_rec(is)) return(-1);
    IS_check_all_zstring(is);
    IS_save_rec(is);
    while(1)
	switch(ACT_exec_nu(is->is_read_act)) {
	    case 0: return(0);
	    case 1:
		if(IS_next_rec(is) != 0) {  /* BP_M 12-01-2008 */
		    for( ; i >= 0 ; i--)
			if(IS_prev_rec(is) != 0) break; /* BP_M 10-11-2008 11:29 */
		    return(-1);
		}
		i++;
		IS_check_all_zstring(is);
		IS_save_rec(is);
		break;
	    case -1:
		for( ; i >= 0 ; i--)
		    if(IS_prev_rec(is) != 0) break; /* BP_M 10-11-2008 11:30 */
		return(-1);
	}
}

/* ====================================================================
Poursuit une recherche dans l'ISAM is. La recherche fournit le pr‚c‚dent
record dans l'ordre de la recherche entam‚e par dernier IS_search().
Elle a lieu suivant le mˆme index, avec la mˆme condition de recherche.

Si la recherche aboutit, le record est plac‚ dans l'ISAM et le pointeur
de l'ISAM est plac‚ … cette position de l'ISAM pour les recherches
ult‚rieures (IS_next(), IS_prev()) et pour les rewrites (IS_rewrite()).

La READ_FN de l'ISAM est ex‚cut‚e aprŠs lecture du record.

&RT 0 si un record est trouv‚ et que la READ_FN de l'ISAM retourne 0, -1
sinon.
&EX
    PrintClientsCommandes(commande)
    long    commande;
    {
	char    name[51];

	if(IS_open_files(is_client, 0L) return(-1);
	IS_set_field(is_client, 4, &commande);
	if(IS_search(is_client, 2, SCR_LE, (char *)0) != 0) {
	    IS_close(is_client);
	    return(-1);
	    }
	printf("Commandes <= %ld FB\n\n", commande);

	while(1) {
	    IS_get_field(is_client, 0, name);
	    printf("%s\n", name);
	    if(IS_prev(is_client)) break;
	    }

	IS_close(is_client);
	return(0);
    }
&TX
&SA IS_search(), IS_prev()
=======================================================================*/

IS_prev(is)
ISAM *is ;
{
    int     i = 0;  /* BP_M 27-07-2010 13:41 */

//    SCR_wdispatch(); /* JMP 15-05-00 */
    if(ISB_check_search(is, SCR_IS_PREV)) return(-1); /* JMP 11-12-98 */
    if(0 != IS_prev_rec(is)) return(-1);
    IS_check_all_zstring(is);
    IS_save_rec(is);
    while(1)
	switch(ACT_exec_nu(is->is_read_act)) {
	    case 0: return(0);
	    case 1:
		if(IS_prev_rec(is) != 0) {
		    for( ; i >= 0 ; i--)
			IS_next_rec(is);
		    return(-1);
		}
		i++;
		IS_check_all_zstring(is);
		IS_save_rec(is);
		break;
	    case -1:
		for( ; i >= 0 ; i--)
		    IS_next_rec(is);
		IS_check_all_zstring(is);   /* BP_M 08-12-1998 */
		IS_save_rec(is);            /* BP_M 08-12-1998 */
		return(-1);
	}
}


/*NH*/
IS_check_required(is)
ISAM    *is;
{
    int     i;
    ISFIELD *isf;
    extern int IS_ERRNO;
    extern char    IsCheckRequired[255]; /* JMP 02-12-99 */

    IsCheckRequired[0] = 0; /* BP_M 08-07-1999 */
    isf = is->is_flds;
    for(i = 0 ; i < is->is_nb_flds ; i++, isf++) {
	if(isf->isf_required == 1 &&
	   IS_is_fld_empty(is->is_rec + isf->isf_start,
			   isf->isf_type, isf->isf_length)) {
		IS_ERRNO = 200;
		if(is->is_info && is->is_info[i].isi_name) {
		    sprintf(IsCheckRequired, "Field [%s] empty", is->is_info[i].isi_name);
		    Debug("%s\n", IsCheckRequired);
		    }

		return(-1);
	}
    }
    return(0);
}

/* ====================================================================
Ecrit un nouveau record dans l'ISAM is. La valeur du record doit ˆtre
fix‚e … l'aide des fonctions IS_set_field(), SCR_set_*(), etc.

Avant d'‚crire le record, les op‚rations suivantes ont lieu :

&EN V‚rification de la cat‚gorie de l'utilisateur (SCR_USER_CATG)
&EN Fixation … 0 de la fin de tous les champs ZSTRING
&EN Ouverture du fichier s'il ne l'est pas
&EN Fixation des champs r‚f‚rence … 0
&EN Fixation du champ AUTO s'il est nul
&EN Ex‚cution de la WRITE_FN. Si elle retourne -1, stop
&EN V‚rification que tous les champs required sont non nuls
&EN Locking du record
&EN Ecriture du record
&EN Unlocking du record
&EN Incr‚mentation des LINKS

En cas d'erreur dans une de ces op‚rations, la fonction s'arrŠte et
retourne -1.

&REM
Le pointeur de l'ISAM (d‚pendant de la derniŠre recherche par
IS_search() ou IS_next()) n'est pas modifi‚ par l'‚criture du nouveau
record.

&RT 0 en cas de succŠs, -1 sinon.
&EX
    IS_set_field(is_client, 0, name);
    if(IS_write(is_client)) {
	PG_display_error("%s : %s", name, IS_error_text());
	return(-1);
	}
&TX
&SA IS_rewrite(), IS_delete()
=======================================================================*/

IS_write(is)
ISAM *is ;
{
    int     flag = 0, err = 0, rc, j;
    extern int IS_ERRNO;        /*JMP 31-03-95*/
    int     readfn_ex = 0;

    if(is->is_create_catg >= 0 && is->is_create_catg < SCR_USER_CATG) {
	PG_display_error(SCR_err_txt(113));
	IS_set_err(-103);
	return(-1);
	}

    IS_check_all_zstring(is);
    if(IS_is_open(is) == 0) {
	    flag = 1;
	    if(0 != IS_open(is, SCR_WRITE, SCR_LOCK)) {
		PG_display_error(IS_error_text());
		return(-1);
		}
	    }

    IS_set_ref_to_0(is);
    /* BP_M 08-10-2010 17:04
	J'ai d‚plac‚ le test des foreign keys avant le calcul du num‚ro auto
	pour ne pas passer un num‚ro au cas o— il y a une erreur
    */
    if(IS_check_foreign(is, 0))      {err = -1 ; IS_set_err(205); goto fin;} /* JMP 08-11-99 */
    if(IS_calc_auto_num(is) != 0)    {err = -1 ; IS_set_err(204); goto fin;} /*JMP 31-03-95*/
    if(ACT_exec_nu(is->is_write_act) != 0) {
	err = -1 ; IS_set_err(201); goto fin; /*JMP 31-03-95*/
	}
    if(IS_check_required(is))   {err = -1 ; IS_set_err(200); goto fin;} /* JMP 09-05-97 */
    if(IS_lock_rec(is) != 0)    {err = -1 ; IS_set_err(202); goto fin;}
    if(0 != IS_write_rec(is)) {
	err = IS_ERRNO;
	IS_unlock_rec(is);
	IS_set_err(err); /*JMP 31-03-95*/
	err = -1;
	goto fin;
    }
    if(IS_unlock_rec(is) != 0)   {err = -1 ; IS_set_err(207); goto fin;}

    if(!readfn_ex && ScrReadFnWrite) {readfn_ex = 1; ACT_exec_nu(is->is_read_act);}  /* BP_M 29-01-2002 11:38 */

    memcpy(is->is_rrec, is->is_rec, is->is_rec_len);

    for(j = 0 ; j < is->is_nb_lnks ; j++)
	if(IS_incr_link(is, j, 1) != 0)  {err = -1 ; goto fin;}

fin:
    if(!readfn_ex && ScrReadFnWrite) {readfn_ex = 1; ACT_exec_nu(is->is_read_act);}  /* BP_M 29-01-2002 11:38 */
    if(flag == 1) IS_close(is);
    return(err);
}

/* ====================================================================
Remplace le record courant de l'ISAM is. La valeur du record doit ˆtre
fix‚e … l'aide des fonctions IS_set_field(), SCR_set_*(), etc.

Avant d'‚crire le record, les op‚rations suivantes ont lieu :

&EN V‚rification de la cat‚gorie de l'utilisateur (SCR_USER_CATG)
&EN Fixation … 0 de la fin de tous les champs ZSTRING
&EN V‚rification que le fichier est ouvert
&EN V‚rification que les champs r‚f‚rence non nuls ne sont pas modifi‚s
&EN D‚cr‚mentation des champs LINK (dans les autres fichiers)
&EN Ex‚cution de la REWRITE_FN. Si elle retourne -1, stop
&EN V‚rification que tous les champs required sont non nuls
&EN Locking du record
&EN Relecture de l'ancien record et v‚rification qu'il n'a pas ‚t‚
    modifi‚ de l'ext‚rieur
&EN R‚‚criture du record
&EN Unlocking du record
&EN Incr‚mentation des LINKS nouveaux

En cas d'erreur dans une de ces op‚rations, la fonction s'arrŠte et
retourne -1.

&REM
Le pointeur de l'ISAM (d‚pendant de la derniŠre recherche par
IS_search() ou IS_next()) n'est pas modifi‚ par l'‚criture du nouveau
record.

&RT 0 en cas de succŠs, -1 sinon.
&EX
    AddCommande(client, montant)
    char    *client;
    long    montant;
    {
	if(IS_open_files(is_client, 0L) return(-1);
	SCR_set_text(is_client_name, client);
	if(IS_search(is_client, 0, SCR_EQ, (char *)0) != 0) {
	    IS_close(is_client);
	    return(-1);
	    }
	is_client_MONTANT += montant;
	if(IS_rewrite(is_client)) {
	    PG_display_error("%s : %s", name, IS_error_text());
	    IS_close(is_client);
	    return(-1);
	    }
	IS_close(is_client);
	return(0);
	}
&TX
&SA IS_rewrite(), IS_delete()
=======================================================================*/

IS_rewrite(is)
ISAM *is ;
{
    int         j, rc, err;
    extern int  IS_ERRNO;        /*JMP 31-03-95*/
    int         readfn_ex =0;

    IS_ERRNO = 0;   /* BP_M 01-01-2002 19:27 */
    if(ISB_check_search(is, SCR_IS_REWRITE)) return(-1); /* JMP 11-12-98 */
    if(is->is_modify_catg >= 0 && is->is_modify_catg < SCR_USER_CATG) {
	PG_display_error(SCR_err_txt(113));
	return(-1);
	}
    IS_check_all_zstring(is);
    if(IS_is_open(is) == 0) {
	if(IS_open_files(is, 0L)) {
	    PG_display_error(SCR_err_txt(15), is->is_filename);
	    return(-1);
	}
    }
/* check no reference already set is modified */
    for(j = 0 ; j < is->is_nb_cmps ; j++)
	if(IS_check_ref(is, j)) return(-1);

/* delete old references */
    for(j = 0 ; j < is->is_nb_lnks ; j++)
	if(IS_is_mod_link(is, j))
	    if(0 != IS_incr_link(is, j, -1)) return(-1);

/* Execute the rewrite function */
    if(ACT_exec_nu(is->is_rewrite_act) != 0) {
	IS_ERRNO = 201; /*JMP 31-03-95*/
	return(-1);
    }
    else
	IS_ERRNO = 0;   /* BP_M 25-05-2012 14:39 */

    if(IS_check_required(is)) {IS_set_err(201); goto fin;}

/* rewrite record */
    if(IS_lock_rec(is) != 0) {IS_set_err(202); goto fin;} /*JMP 31-03-95*/
    if(SCR_NO_REREAD == 0 &&
	is->is_read_act == 0 && /* JMP 22-03-99 */
	IS_reread_rec(is)) {IS_set_err(203); goto fin;}   /* JMP 01-11-98 */

    IS_check_all_zstring(is);
    if(SCR_NO_REREAD == 0 &&
	is->is_read_act == 0 && /* JMP 22-03-99 */
	memcmp(is->is_rrec, is->is_rrrec, is->is_rec_len) &&
	IS_check_reread_dbl(is)) {                          /* JMP 21-12-02 */
	    IS_debug_reread(is); /* JMP 10-08-99 */
	    IS_set_err(201);
	    goto fin;
    }
    ISB_mod_rec(is);    /* JMP 12-12-98 */

    if(IS_check_foreign(is, 1))    {IS_set_err(205); goto fin;} /* JMP 08-11-99 */

    if(IS_rewrite_rec(is)) {
	IS_set_err(206);        /* BP_M 03-07-2002 16:04 */
	err = IS_ERRNO;
	IS_unlock_rec(is);
	IS_ERRNO = err;
	ISB_del_rec(is);    /* JMP 12-12-98 */
	goto fin;
    }
    else
	IS_ERRNO = 0;

    IS_unlock_rec(is);

    if(!readfn_ex && ScrReadFnWrite) {
	ACT_exec_nu(is->is_read_act); /* BP_M 01-01-2002 19:16 */
	readfn_ex = 1;
    }

    memcpy(is->is_rrec, is->is_rec, is->is_rec_len); /* BP_M 06-02-1999 */

    IS_save_rec(is);

/* add new references */
    for(j = 0 ; j < is->is_nb_lnks ; j++)
	if(IS_is_mod_link(is, j))
	    if(0 != IS_incr_link(is, j, 1)) {
		    IS_set_err(210);
		    goto fin;
	    }

fin:
    if(!readfn_ex && ScrReadFnWrite) {
	ACT_exec_nu(is->is_read_act); /* BP_M 01-01-2002 19:16 */
	readfn_ex = 1;
    }

    return(IS_ERRNO);
}

/* ====================================================================
D‚truit le record courant de l'ISAM is.

Avant de d‚truire le record, les op‚rations suivantes ont lieu :

&EN V‚rification de la cat‚gorie de l'utilisateur (SCR_USER_CATG)
&EN V‚rification que le fichier est ouvert
&EN V‚rification que les champs r‚f‚rence sont tous nuls
&EN D‚cr‚mentation des champs LINK (dans les autres fichiers)
&EN Ex‚cution de la DELETE_FN. Si elle retourne -1, stop
&EN Locking du record
&EN Relecture de l'ancien record et v‚rification qu'il n'a pas ‚t‚
    modifi‚ de l'ext‚rieur
&EN Destruction du record
&EN Unlocking du record

En cas d'erreur dans une de ces op‚rations, la fonction s'arrŠte et
retourne -1.

&REM
Le pointeur de l'ISAM (d‚pendant de la derniŠre recherche par
IS_search() ou IS_next()) n'est pas modifi‚ par la destruction du record
: il se place sur le record suivant.

&RT 0 en cas de succŠs, -1 sinon.
&SA IS_rewrite(), IS_delete()
=======================================================================*/

IS_delete(is)
ISAM *is ;
{
    int     i, rc, err;
    extern int  IS_ERRNO;        /*JMP 31-03-95*/

    if(ISB_check_search(is, SCR_IS_DELETE)) return(-1); /* JMP 11-12-98 */
    if(is->is_delete_catg >= 0 && is->is_delete_catg < SCR_USER_CATG) {
	PG_display_error(SCR_err_txt(113));
	return(-1);
	}
    if(IS_is_open(is) == 0) {
	if(IS_open_files(is, 0L)) {
	    PG_display_error(SCR_err_txt(15), is->is_filename);
	    return(-1);
	}
    }
    if(!IS_ref_is_0(is)) return(-1);
    for(i = 0 ; i < is->is_nb_lnks; i++)
	if(0 != IS_incr_link(is, i, -1)) return(-1);
    rc = ACT_exec_nu(is->is_delete_act);        /* BP_M 12-01-2008 */
    if(rc == 1) return(0);                      /* BP_M 12-01-2008 */
    if(rc != 0) {IS_set_err(201); return(-1);}  /* BP_M 12-01-2008 */
    if(IS_lock_rec(is) != 0) {IS_set_err(202); return(-1);}
    if(SCR_NO_REREAD == 0 &&
	is->is_read_act == 0 && /* JMP 22-03-99 */
	IS_reread_rec(is)) goto err1;   /* JMP 01-11-98 */
    IS_check_all_zstring(is);   /* BP_MODIF 30-10-98 */
    if(SCR_NO_REREAD == 0 &&
	is->is_read_act == 0 && /* JMP 22-03-99 */
	memcmp(is->is_rrec, is->is_rrrec, is->is_rec_len) &&
	IS_check_reread_dbl(is)) {                          /* JMP 21-12-02 */
	IS_ERRNO = 202;                                     /*JMP 24-05-95*/
	IS_debug_reread(is); /* JMP 10-08-99 */
	goto err1;                                          /*JMP 24-05-95*/
	}                                                   /*JMP 24-05-95*/
    if(IS_check_foreign(is, 2))    {IS_set_err(205); return(-1);} /* JMP 08-11-99 */
    ISB_del_rec(is);    /* JMP 12-12-98 */
    if(IS_delete_rec(is)) {                                 /*JMP 24-05-95*/
err1:   err = IS_ERRNO;                                     /*JMP 24-05-95*/
	IS_unlock_rec(is);                                  /*JMP 24-05-95*/
	IS_ERRNO = err;                                     /*JMP 24-05-95*/
	return(-1);                                         /*JMP 24-05-95*/
	}                                                   /*JMP 24-05-95*/

//  if(IS_next(is) != 0) IS_prev(is);   /* BP_M 10-07-2003 10:48 */

    IS_unlock_rec(is);
    return(0);
}

/* BP_M 25-05-2013 14:04 */
// Idem que IS_delete mais n'ex‚cute pas la delete_act

IS_delete_noact(ISAM *is)
{
    int     act, rc;

    act = is->is_delete_act;
    is->is_delete_act = 0;
    rc = IS_delete(is);
    is->is_delete_act = act;

    return(rc);
}

/* BP_M 25-05-2013 14:04 */
// Idem que IS_rewrite mais n'ex‚cute pas la rewrite_act

IS_rewrite_noact(ISAM *is)
{
    int     act, rc;

    act = is->is_rewrite_act;
    is->is_rewrite_act = 0;
    rc = IS_rewrite(is);
    is->is_rewrite_act = act;

    return(rc);
}

/* BP_M 25-05-2013 14:04 */
// Idem que IS_write mais n'ex‚cute pas la write_act

IS_write_noact(ISAM *is)
{
    int     act, rc;

    act = is->is_write_act;
    is->is_write_act = 0;
    rc = IS_write(is);
    is->is_write_act = act;

    return(rc);
}

/*NH*/
/***************** SUB FUNCTIONS ************************************/
IS_set_ref_to_0(is)
ISAM    *is;
{
    int     i;
    int     fld_nb;
    long    bufl = 0L;

    for(i = 0 ; i < is->is_nb_cmps ; i++) {
	    fld_nb = (is->is_cmps + i)->cmp_ref;
	    if(fld_nb != 0) IS_set_field(is, fld_nb, (char *)&bufl);
	    }

    return(0);
}

/*NH*/
IS_ref_is_0(is)
ISAM    *is;
{
    int     i;
    int     fld_nb;
    long    bufl;

    for(i = 0 ; i < is->is_nb_cmps ; i++)
	    fld_nb = (is->is_cmps + i)->cmp_ref;
	    if(fld_nb != 0) {
		IS_get_field(is, fld_nb, (char *)&bufl);
		if(bufl > 0L) return(0);
	    }

    return(1);
}

/*NH*/
IS_check_ref(is, cmp_nb)
ISAM    *is;
int     cmp_nb;
{
    int         j;
    long        bufl;
    COMPOSITE   *cmp;

    cmp = is->is_cmps + cmp_nb;
    if(cmp->cmp_ref == 0) return(0);

    for(j = 0 ; j < cmp->cmp_nb_flds ; j++)
	if(IS_is_mod_field(is, cmp->cmp_flds[j])) break;

    if(j < cmp->cmp_nb_flds) {
	IS_get_field(is, cmp->cmp_ref, (char *)&bufl);
	if(bufl > 0L) return(1);
	}

    return(0);
}

/*NH*/
IS_is_mod_link(is, lnk_nb)
ISAM    *is;
int     lnk_nb;
{
    int     j;
    LINK    *lnk;

    lnk = is->is_lnks + lnk_nb;

    for(j = 0 ; j < lnk->lnk_nb_flds ; j++)
	if(IS_is_mod_field(is, lnk->lnk_flds[j])) return(1);

    return(0);
}

/*NH*/
IS_incr_link(is, lnk_nb, add)
ISAM    *is;
int     lnk_nb;
int     add;
{
    LINK    *lnk;
    ISAM    *lnk_is;
    int     lnk_fld;
    int     flag = 0;
    long    nb_link;
    int     j;

    lnk = is->is_lnks + lnk_nb;
    lnk_is = (ISAM *)SCR_PTR(lnk->lnk_isam);

    /* open linked isam if needed */
    if(IS_is_open(lnk_is) == 0) {
	if(IS_open(lnk_is, SCR_UPDATE, SCR_LOCK) != 0) return(-1);
	flag = 1;
	}

    /* copy index fields to linked isam */
    for(j = 0 ; j < lnk->lnk_nb_flds ; j++) {
	lnk_fld = (lnk_is->is_cmps + lnk->lnk_cmp)->cmp_flds[j];
	IS_copy_field(lnk_is, lnk_fld, 0, is, lnk->lnk_flds[j],
			    (add > 0) ? 0 : 1);
	}

    /* search */
    if(IS_search(lnk_is, lnk->lnk_cmp, SCR_EQ, (char *)0) != 0) {
	if(flag != 0) IS_close(lnk_is);
	return(-1);
	}

    /* increment reference */
    IS_get_field(lnk_is, (lnk_is->is_cmps + lnk->lnk_cmp)->cmp_ref, (char *)&nb_link) ;
    nb_link += add;
    IS_set_field(lnk_is, (lnk_is->is_cmps + lnk->lnk_cmp)->cmp_ref, (char *)&nb_link) ;

    /* rewrite record */
    if(IS_rewrite_rec(lnk_is) != 0) {
	if(flag != 0) IS_close(lnk_is);
	return(-1);
	}

    /* close isam */
    if(flag != 0) IS_close(lnk_is);

    return(0);
}

/* ====================================================================
Compare un champ du record de l'ISAM courant … la valeur du mˆme champ
lors de la lecture. Le champ a en effet pu ˆtre modifi‚ depuis la
lecture.

&RT 0 si les champs sont ‚gaux, une autre valeur sinon.
=======================================================================*/
IS_is_mod_field(is, fld_nb)
ISAM    *is;
int     fld_nb;
{
    ISFIELD *isf;

    isf = is->is_flds + fld_nb;
    return(memcmp(is->is_rec + isf->isf_start,
		  is->is_rrec + isf->isf_start,
		  isf->isf_length));
}

/*NH*/
IS_copy_rec(is, rec1, rec2)
ISAM    *is;
char    *rec1;
char    *rec2;
{
    memcpy(rec1, rec2, is->is_rec_len);
    return(0);
}

/*NH*/
IS_copy_field(is_out, nb_out, b_out, is_in, nb_in, b_in)
ISAM    *is_out;
int     nb_out;
int     b_out;
ISAM    *is_in;
int     nb_in;
int     b_in;
{
    ISFIELD *isf_in;
    ISFIELD *isf_out;
    char    *fld_in;
    char    *fld_out;

    isf_in  = is_in->is_flds + nb_in;
    isf_out = is_out->is_flds + nb_out;

    fld_in  = (b_in == 0)   ? is_in->is_rec  : is_in->is_rrec;
    fld_in += isf_in->isf_start;
    fld_out = (b_out == 0)  ? is_out->is_rec : is_out->is_rrec;
    fld_out += isf_out->isf_start;

    memcpy(fld_out, fld_in, isf_out->isf_length);
    return(0);
}

/* ====================================================================
Duplicie une structure ISAM, permettant ainsi d'‚viter la double
d‚finition d'un ISAM. Les ‚l‚ments variables seuls sont duplici‚s : les
records courants, lus et relus.

Le tableau de caractŠres contenant le nom du fichier doit ˆtre pass‚ et
n'est pas allou‚ dans la structure. Par cons‚quent, ce tableau est soit
statique, soit allou‚ par l'utilisateur. S'il s'agit d'une variable
locale … une fonction, la valeur du pointeur sera perdue dŠs que l'on
quittera la fonction.

Toutes les autres informations de l'ISAM sont conserv‚es sur base du
pointeur contenu dans l'ISAM … copier.


&RT Pointeur vers le nouvel ISAM.
&EX
   is_client2 = IS_dup(is, "is_client2");
   if(IS_open_files(is_client, is_client2, 0L)) return(-1);
&TX
&SA IS_free_dup()
=======================================================================*/

ISAM *IS_dup(is, filename)          /* JMP 05-06-91 */
ISAM    *is;
char    *filename;
{
    ISAM    *is2;
    int     i;

    is2 = (ISAM *) SCR_malloc(sizeof(ISAM));

    memcpy(is2, is, sizeof(ISAM));

    is2->is_filename = SCR_stracpy(filename); /* BP_M 27-02-1998 */
    is2->is_rec     = SCR_malloc(is->is_rec_len);
    is2->is_rrec    = SCR_malloc(is->is_rec_len);
    is2->is_rrrec   = SCR_malloc(is->is_rec_len);

    is2->is_flds    = (ISFIELD *) SCR_malloc(sizeof(ISFIELD) * is->is_nb_flds); /* JMP 04-06-01 */
    memcpy(is2->is_flds, is->is_flds, sizeof(ISFIELD) * is->is_nb_flds);        /* JMP 04-06-01 */
    is2->is_info    = (ISFINFO *) SCR_malloc(sizeof(ISFINFO) * is->is_nb_flds); /* JMP 04-06-01 */
    memcpy(is2->is_info, is->is_info, sizeof(ISFINFO) * is->is_nb_flds);        /* JMP 04-06-01 */

    if(is2->is_nb_isbs) {                                            /* JMP 11-12-98 */
	is2->is_isbs = (ISAMBUF **) SCR_malloc(sizeof(ISAMBUF *) * is->is_nb_isbs);   /* JMP 12-12-98 */
	for(i = 0; i < is2->is_nb_isbs ; i++)                        /* JMP 12-12-98 */
	    is2->is_isbs[i] = (ISAMBUF *)                            /* JMP 12-12-98 */
		SCR_malloc(sizeof(ISAMBUF) + is2->is_rec_len + is2->is_cmp_len); /* JMP 12-12-98 */
	}

    is2->is_nb_open = 0;
    is2->is_open_type = 0;

    /* BP_M 29-09-97 */
    is2->is_isam = 0;
/*  IS_create_struct(is2); */ /* BP_M 27-02-1998 */
    /* BP_M 29-09-97 */

    /* BP_M 26-10-95 */
    /*IS_alloc_vfld(is); ?????*/

    return(is2);
}

/* ====================================================================
LibŠre l'espace allou‚ au nouvel objet ISAM cr‚‚ par IS_dup().

&EX
   is_client2 = IS_dup(is, "is_client2");
   if(IS_open_files(is_client, is_client2, 0L)) return(-1);
    ...
   IS_free_dup(is_client2);
&TX
&SA IS_dup()
=======================================================================*/

IS_free_dup(is)
ISAM    *is;
{
    int     i;

    SCR_free(is->is_rec);
    SCR_free(is->is_rrec);
    SCR_free(is->is_rrec);
    if(is->is_nb_isbs) {                      /* JMP 12-12-98 */
	for(i = 0; i < is->is_nb_isbs ; i++)  /* JMP 12-12-98 */
	    SCR_free(is->is_isbs[i]);         /* JMP 12-12-98 */
	SCR_free(is->is_isbs);                /* JMP 12-12-98 */
	}                                     /* JMP 12-12-98 */
    IS_free_struct(is);
    SCR_free(is);
    return(0);
}

/* ====================================================================
Change le nom du fichier physique associ‚ … un objet ISAM. Le pointeur
vers le nom remplace celui actuellement pr‚sent dans la structure ISAM.
Ce pointeur n'est pas allou‚ et doit donc ˆtre soit statique, soit
rester actif tant que le fichier peut ˆtre utilis‚.

&EX
    IS_set_name(is_res, "jun91");
    if(IS_open_files(is_res, 0L)) return(-1);
&TX
=======================================================================*/

IS_set_name(is, name)
ISAM    *is;
char    *name;
{
    IS_set_name_db(is, name);
    is->is_filename = name;
    return(0);
}


/* ====================================================================
Retourne un message d'erreur correspondant … la derniŠre erreur
rencontr‚e dans l'utilisation d'une base de donn‚es. Le message est
extrait du fichier scr_err.txt.

Le message est compos‚ de deux parties : le nom du fichier et l'erreur
rencontr‚e. Comme un fichier autre que celui concern‚ par l'erreur peut
ˆtre utilis‚ aprŠs l'arriv‚e de l'erreur, le nom du fichier peut dans
certains cas (rares) ne pas ˆtre celui concern‚.

&RT pointeur vers une variable statique contenant le dernier message
d'erreur.
&EX
    if(IS_write(is_client)) {
	PG_display_error(IS_error());
	return(-1);
	}
&TX
=======================================================================*/

char *IS_error()
{
    return(IS_error_text());
}


/*NH*/
IS_set_err(n) /*JMP 31-03-95*/
int     n;
{
    extern int IS_ERRNO;

    if(!IS_ERRNO)
	IS_ERRNO = n;
    return(0);
}

/*NH*/
IS_set_field_txt(is, fld, txt)
ISAM    *is;
int     fld;
char    *txt;
{
    short       s;
    long        l, atol();
    float       f;
    double      d;
    char        c;

    switch(is->is_flds[fld].isf_type) {
	case SCR_ZSTRING :
	case SCR_STRING :
	case SCR_CHAR   :
	    IS_set_field(is, fld, txt);
	    break;
	case SCR_SHORT1 :
	    c = txt[0];
	    IS_set_field(is, fld, &c);
	    break;

	case SCR_CHECK_BOX:     /* BP_M 21-01-97 */
	case SCR_TYPE_RADIO:    /* BP_M 21-01-97 */
	case SCR_SHORT :
	case SCR_MENU  :            /* JMP 05-06-91 */
	    s = atoi(txt);
	    IS_set_field(is, fld, (char *)&s);
	    break;

	case SCR_DATE : /* BP_M 29-03-2001 16:36 */
	    if(txt[0] == '0' && strlen(txt) < 10)   /* BP_M 10-12-2012 11:15 */
		l = 0L;
	    else
		l = SCR_date_to_long(txt);
	    if(l >= 0 && l < 20201231) IS_set_field(is, fld, &l);
	    break;
	case SCR_TIME : /* BP_M 29-03-2001 16:36 */
	    if(txt[0] == '0')
		l = 0L;
	    else
		l = SCR_ftime_to_long(txt, "HH:MM");
	    if(l >= 0 && l < 240000) IS_set_field(is, fld, &l);
	    break;

	case SCR_AUTO :
	case SCR_REF  :
	case SCR_LONG :
	    l = atol(txt);
	    IS_set_field(is, fld, (char *)&l);
	    break;

	case SCR_FLOAT :
	    SCR_replace(txt, ",", "."); /* BP_M 15-03-2010 21:20 */
	    f = SCR_atof(txt);
	    IS_set_field(is, fld, (char *)&f);
	    break;

	case SCR_DOUBLE :
	    SCR_replace(txt, ",", "."); /* BP_M 15-03-2010 21:20 */
	    d = SCR_atof(txt);
	    IS_set_field(is, fld, (char *)&d);
	    break;
    }
    return(0);
}

/* BP_M 11-06-2008 */
/*NH*/
IS_get_field_txt(is, fld, txt)
ISAM    *is;
int     fld;
char    *txt;
{
    short       s;
    long        l, atol();
    float       f;
    double      d;
    char        c;

    switch(is->is_flds[fld].isf_type) {
	case SCR_ZSTRING :
	case SCR_STRING :
	case SCR_CHAR   :
	    IS_get_field(is, fld, txt);
	    txt[is->is_flds[fld].isf_length] = 0;
	    break;
	case SCR_SHORT1 :
	    IS_get_field(is, fld, &c);
	    txt[0] = c;
	    break;

	case SCR_CHECK_BOX:     /* BP_M 21-01-97 */
	case SCR_TYPE_RADIO:    /* BP_M 21-01-97 */
	case SCR_SHORT :
	case SCR_MENU  :            /* JMP 05-06-91 */
	    IS_get_field(is, fld, (char *)&s);
	    sprintf(txt, "%d", s);
	    break;

	case SCR_AUTO :
	case SCR_REF  :
	case SCR_LONG :
	case SCR_DATE : /* BP_M 29-03-2001 16:36 */
	case SCR_TIME : /* BP_M 29-03-2001 16:36 */
	    IS_get_field(is, fld, &l);
	    sprintf(txt, "%ld", l);
	    break;

	case SCR_FLOAT :
	    IS_get_field(is, fld, (char *)&f);
	    sprintf(txt, "%6.02f", f);
	    break;

	case SCR_DOUBLE :
	    IS_get_field(is, fld, (char *)&d);
	    sprintf(txt, "%8.02lf", d);
	    break;
    }
    return(0);
}

/* BP_M 14-03-2013 13:44 */
/*NH*/
IS_get_read_field_txt(is, fld, txt)
ISAM    *is;
int     fld;
char    *txt;
{
    short       s;
    long        l, atol();
    float       f;
    double      d;
    char        c;

    switch(is->is_flds[fld].isf_type) {
	case SCR_ZSTRING :
	case SCR_STRING :
	case SCR_CHAR   :
	    IS_get_read_field(is, fld, txt);
	    txt[is->is_flds[fld].isf_length] = 0;
	    break;
	case SCR_SHORT1 :
	    IS_get_read_field(is, fld, &c);
	    txt[0] = c;
	    break;

	case SCR_CHECK_BOX:     /* BP_M 21-01-97 */
	case SCR_TYPE_RADIO:    /* BP_M 21-01-97 */
	case SCR_SHORT :
	case SCR_MENU  :            /* JMP 05-06-91 */
	    IS_get_read_field(is, fld, (char *)&s);
	    sprintf(txt, "%d", s);
	    break;

	case SCR_AUTO :
	case SCR_REF  :
	case SCR_LONG :
	case SCR_DATE : /* BP_M 29-03-2001 16:36 */
	case SCR_TIME : /* BP_M 29-03-2001 16:36 */
	    IS_get_read_field(is, fld, &l);
	    sprintf(txt, "%ld", l);
	    break;

	case SCR_FLOAT :
	    IS_get_read_field(is, fld, (char *)&f);
	    sprintf(txt, "%6.02f", f);
	    break;

	case SCR_DOUBLE :
	    IS_get_read_field(is, fld, (char *)&d);
	    sprintf(txt, "%8.02lf", d);
	    break;
    }
    return(0);
}

/*NH JMP 16-04-97 */
SCR_edis_create_F10(pg, is)
PAGE    *pg;
ISAM    *is;
{
    IS_empty_rec(is);
    if(SCR_CONFIRME_ON && CONFIRME) return(-1);
    PG_set_auto_num(pg, is);
    SCR_page_to_isam(pg, is);
    if(IS_write(is) == 0) return(0);
    PG_display_error(IS_error_text());
    return(-1);
}

/* ====================================================================
Entame une recherche dans l'ISAM is. nu_index est le num‚ro de l'index
de recherche, cond est la condition de recherche et order l'ordre de
lecture (order n'est pas impl‚ment‚ avec CTREE).

Si nu_index est n‚gatif, la recherche a lieu dans l'ordre physique de la
database.

La recherche commence en fonction de la valeur des champs de l'index de
recherche plac‚s dans le record de l'ISAM. Ces valeurs peuvent ˆtre
fix‚es par les fonctions IS_set_field(), SCR_set_*(), etc.

cond peut prendre les valeurs suivantes :

&EN SCR_GE : recherche les records sup‚rieurs ou ‚gaux
&EN SCR_LE : recherche les records inf‚rieurs ou ‚gaux
&EN SCR_EQ : recherche les records ‚gaux
&EN SCR_EQ1 : recherche les records ‚gaux sur le champ 1 de l'index
&EN SCR_EQ2 : recherche les records ‚gaux sur les champs 1-2 de l'index
&EN SCR_EQ3 : recherche les records ‚gaux sur les champs 1-3 de l'index
&EN SCR_EQ4 : recherche les records ‚gaux sur les champs 1-4 de l'index
&EN SCR_EQ5 : recherche les records ‚gaux sur les champs 1-5 de l'index
&EN SCR_EQ6 : recherche les records ‚gaux sur les champs 1-6 de l'index
&EN SCR_FIRST: recherche le premier record de la database dans l'ordre
    de l'index sans tenir compte de la valeur de l'index
&EN SCR_LAST: recherche le dernier record de la database dans l'ordre
    de l'index sans tenir compte de la valeur de l'index
&EN SCR_DB : recherche le premier record de la database dans l'ordre
    physique des enregistrements (m‚thode la plus rapide pour lire tout un
    fichier).

Si la recherche aboutit, le record est plac‚ dans l'ISAM et le pointeur
de l'ISAM est plac‚ … cette position de l'ISAM pour les recherches
ult‚rieures (IS_next(), IS_prev()) et pour les rewrites (IS_rewrite()).

La READ_FN de l'ISAM est ex‚cut‚e aprŠs lecture du record. Si cette
fonction retourne -1, la lecture est consid‚r‚e comme erron‚e. Si elle
retourne 1, le record suivant est lu jusqu'… ce que leæa READ_FN
retourne 0 ou -1. Si elle retourne 0, la lecture est correcte.

&RT 0 si un record est trouv‚ et que la READ_FN de l'ISAM retourne 0, -1
sinon.
&EX
    PrintClientsCommune(commune)
    char    *commune;
    {
	char    name[51];

	if(IS_open_files(is_client, 0L) return(-1);
	IS_set_field(is_client, 3, commune);
	if(IS_search(is_client, 1, SCR_EQ, (char *)0) != 0) {
	    IS_close(is_client);
	    return(-1);
	    }
	printf("Clients habitant %s\n\n", commune);

	while(1) {
	    IS_get_field(is_client, 0, name);
	    printf("%s\n", name);
	    if(IS_next(is_client)) break;
	    }

	IS_close(is_client);
	return(0);
    }
&TX
&SA IS_next(), IS_prev()
=======================================================================*/

IS_search(is, nu_index, cond, order)
ISAM     *is;
int      nu_index;
int      cond;
char     *order;
{
    int     ns, rc;
    char    buf[256];

//    SCR_wdispatch(); /* JMP 15-05-00 */
    if(cond >= SCR_EQ1 && cond <= SCR_EQ6 &&
	nu_index >= 0 && is->is_cmps[nu_index].cmp_nb_flds == 1)
	    cond = SCR_EQ;

    if(is->is_nb_isbs <= 0
		|| SCR_ISAM_NOBUF
		|| cond >= SCR_MINMAX
		|| cond == SCR_DB
		|| nu_index < 0
		|| is->is_query  /* JMP 16-04-02 */
		|| (SCR_ISAM_NOORDER == 0 && is->is_type_dbd && order && order[0])) {
	is->is_cur_search = -1;
	is->is_isb_search = -1;
	return(IS_search_ph(is, nu_index, cond, order));
	}

    ns = ISB_find_search(is, nu_index, cond, order);
    if(ns < 0) {
	ISB_save_cmp(buf, is, nu_index);
	rc = IS_search_ph(is, nu_index, cond, order);
	if(rc < 0) return(rc);
	ns = ISB_save_search(is, nu_index, cond, buf);
	is->is_cur_search = ns;
	return(0);
	}
    memcpy(is->is_rec, is->is_isbs[ns]->isb_rec, is->is_rec_len);
    memcpy(is->is_rrec, is->is_isbs[ns]->isb_rec, is->is_rec_len);
    is->is_isb_search = ns;
    is->is_cur_search = -1;
    return(0);
}

/*NH*/
ISB_save_search(is, nu_index, cond, buf)
ISAM     *is;
int      nu_index;
int      cond;
char     *buf;
{
    int         i, ns = 0;
    long        ms = is->is_isbs[0]->isb_ms;
    ISAMBUF     *isb;

    for(i = 0 ; i < is->is_nb_isbs ; i++) {
	isb = is->is_isbs[i];
	if(isb->isb_cmp < 0) {
	    ns = i;
	    break;
	    }
	if(isb->isb_ms < ms) {
	    ns = i;
	    ms = isb->isb_ms;
	    }
	}
    isb = is->is_isbs[ns];
    isb->isb_cmp = nu_index;
    isb->isb_cond = cond;
    isb->isb_ms = WscrGetMS();
    memcpy(isb->isb_rec, is->is_rec, is->is_rec_len);
    memcpy(isb->isb_rec + is->is_rec_len, buf, is->is_cmp_len);
    is->is_isb_search = ns;
    return(ns);
}

/*NH*/
ISB_save_cmp(buf, is, nu_index)
ISAM     *is;
int      nu_index;
char     *buf;
{
    COMPOSITE   *cmp;
    ISFIELD     *isf;
    int         i;

    memset(buf, 0, is->is_cmp_len);
    if(nu_index < 0) return;
    cmp = is->is_cmps + nu_index;
    for(i = 0 ; i < cmp->cmp_nb_flds ; i++) {
	isf = is->is_flds + cmp->cmp_flds[i];
	memcpy(buf, is->is_rec + isf->isf_start, cmp->cmp_lgs[i]);
	buf += cmp->cmp_lgs[i];
	}
}

/*NH*/
ISB_find_search(is, nu_index, cond, order)
ISAM     *is;
int      nu_index;
int      cond;
char     *order;
{
    char        buf[512];
    int         i;
    long        ms = WscrGetMS() - 1000L * is->is_isb_sync;
    ISAMBUF     *isb;

    ISB_save_cmp(buf, is, nu_index);
    for(i = 0 ; i < is->is_nb_isbs ; i++) {
	isb = is->is_isbs[i];
	if(isb->isb_cmp  != nu_index) continue;
	if(isb->isb_cond != cond) continue;
	if(is->is_isb_sync && isb->isb_ms < ms) continue;
	if(memcmp(isb->isb_rec + is->is_rec_len, buf,
		    is->is_cmp_len)) continue;
	return(i);
	}
    return(-1);
}

/*NH*/
ISB_check_search(is, act)
ISAM    *is;
int     act;
{
    int     rc, ns = is->is_isb_search;
    ISAMBUF *isb;
    char    *tmprec;

    if(ns < 0 || is->is_nb_isbs <= 0) return(0);
    if(is->is_cur_search == ns) return(0);
    isb = is->is_isbs[ns];
    if(is->is_type_dbd && (act == SCR_IS_REWRITE || act == SCR_IS_DELETE)) return(0);

    /* Sauvegarder le is_rec ‚ventuellement modifi‚ dans is_rrrec */
    tmprec = is->is_rec;
    is->is_rec = is->is_rrrec; /* is_rrrec non utilis‚ */

    ISB_reset_cmp(is, ns);

    rc = IS_search_ph(is, isb->isb_cmp, isb->isb_cond, (char *)0);

    /* Restaurer le is_rec modifi‚ */
    is->is_rrrec = is->is_rec; /* is_rrrec non utilis‚ */
    is->is_rec = tmprec;

    if(rc || memcmp(is->is_rrrec, isb->isb_rec, is->is_rec_len)) { /* JMP 06-01-99 */
	is->is_cur_search = -1;
	is->is_isb_search = -1;
	isb->isb_cond = -1;
	return(-1);
	}
    is->is_cur_search = ns;
    is->is_isb_search = -1;
    return(0);
}

/*NH*/
ISB_reset_cmp(is, ns)
ISAM     *is;
int      ns;
{
    COMPOSITE   *cmp;
    ISFIELD     *isf;
    ISAMBUF     *isb = is->is_isbs[ns];
    char        *buf = isb->isb_rec + is->is_rec_len;
    int         i;

    cmp = is->is_cmps + isb->isb_cmp;
    for(i = 0 ; i < cmp->cmp_nb_flds ; i++) {
	isf = is->is_flds + cmp->cmp_flds[i];
	memcpy(is->is_rec + isf->isf_start, buf, cmp->cmp_lgs[i]);
	buf += cmp->cmp_lgs[i];
	}
}

/*NH*/
ISB_del_rec(is)
ISAM     *is;
{
    int         i;

    for(i = 0 ; i < is->is_nb_isbs ; i++) {
	if(memcmp(is->is_isbs[i]->isb_rec, is->is_rrec, is->is_rec_len) == 0)
	    is->is_isbs[i]->isb_cmp = -1;
	}
    return(0);
}

/*NH*/
ISB_mod_rec(is)
ISAM     *is;
{
    int         i;
    char        buf[512];
    ISAMBUF     *isb;

    for(i = 0 ; i < is->is_nb_isbs ; i++) {
	isb = is->is_isbs[i];
	if(isb->isb_cmp < 0) continue;
	if(memcmp(isb->isb_rec, is->is_rrec, is->is_rec_len) == 0) {
	    ISB_save_cmp(buf, is, isb->isb_cmp);
	    if(memcmp(isb->isb_rec + is->is_rec_len, buf, is->is_cmp_len)) {
		isb->isb_cmp = -1;
		continue;
		}
	    memcpy(isb->isb_rec, is->is_rec, is->is_rec_len);
	    }
	}
    return(0);
}

/* ====================================================================
Identique … la fonction IS_rewrite(), mais sans v‚rifier que le record
n'a pas ‚t‚ modifi‚ depuis sa lecture.

Cette fonction est … utiliser lors de longs scan de fichiers au cours
desquels le record lu est modifi‚. La relecture ne se justifie pas en
g‚n‚ral dans ce cas car les fonctions IS_next() et IS_rewrite() se
suivent imm‚diatement et que le risque de modification par un autre
process du record lu est minime, voire inexistant.

&SA IS_delete_imm(), IS_NO_REREAD
==================================================================== */

IS_rewrite_imm(is)
ISAM    *is;
{
    int     rc, norr = SCR_NO_REREAD;

    SCR_NO_REREAD = 1;
    rc = IS_rewrite(is);
    SCR_NO_REREAD = norr;
    return(rc);
}

/* ====================================================================
Identique … la fonction IS_delete(), mais sans v‚rifier que le record
n'a pas ‚t‚ modifi‚ depuis sa lecture.

Cette fonction est … utiliser lorsque le record lu est directement
d‚truit. Le risque de modification ou de suppression par un autre
process du record lu est alors minime, voire inexistant.

&SA IS_delete_imm(), IS_NO_REREAD
==================================================================== */

IS_delete_imm(is)
ISAM    *is;
{
    int     rc, norr = SCR_NO_REREAD;

    SCR_NO_REREAD = 1;
    rc = IS_delete(is);
    SCR_NO_REREAD = norr;
    return(rc);
}

/* ====================================================================
Resynchronise les enregistrements bufferis‚s dans l'ISAM is. Tous les
r‚f‚rences aux records m‚moris‚s dans l'ISAM sont supprim‚s par cette
fonction.

&SA IS_resync_all()
==================================================================== */

IS_resync(is)
ISAM    *is;
{
    int     i;

    for(i = 0 ; i < is->is_nb_isbs ; i++)
	is->is_isbs[i]->isb_cmp = -1;

    return(0);
}

/* ====================================================================
Resynchronise les enregistrements bufferis‚s dans tous les ISAM d‚finis.

&SA IS_resync()
==================================================================== */

IS_resync_all()
{
    int         i;
    FILE_TYPE   *flt;

    for(i = 1 ; i < SCR_READ_FILES.tbl_nb_fl ; i++) {
	flt = SCR_READ_FILES.tbl_fl[i];
	if(TYPE16(flt->fl_type) != SCR_TYPE_ISAM) continue;
	IS_resync((ISAM *)flt->fl_ptr);
	}

    return(0);
}

/*
IS_update(is, cmp_nb)
ISAM    *is;
int     cmp_nb;
{
    if(is->is_type_dbd == 0) {
	memcmp(is->is_rrrec, is->is_rec, is->is_rec_len);
	if(IS_search(is, cmp_nb, SCR_EQ, (char *)0)) return(-1);
	memcmp(is->is_rec, is->is_rrrec, is->is_rec_len);
	return(IS_rewrite_imm(is));
	}
    else {
	return(IS_rewrite_rec(is));
	}
}
*/

/*NH Version avec bufferisation */
/* Note : is_cur_search : indique le num‚ro du ISAMBUF qui correspond
			  au IS_search_ph courant (physique)
	  is_isb_search : indique le num‚ro du ISAMBUF qui correspond
			  au IS_search courant (virtuel)
    Situations possibles
					cur_search  isb_search
      1.search phys non en cours         -1           num ISB
	i.e. search trouv‚ dans isbs
      2.search phys en cours             num ISB      num ISB
	    au d‚but du search
      3.search phys en cours aprŠs       num ISB      num ISB
	    next, prev, delete
	    ou rewrite

*/

/*NH*/
IS_debug_reread(is)
ISAM    *is;
{
    int     i, j, lg, totlg = 0;

    Debug("Comparaison de rrec et rrrec pour %s\n", is->is_filename);
    for(i = 0 ; i < is->is_nb_flds ; i++) {
	lg = is->is_flds[i].isf_length;
	if(memcmp(is->is_rrec + totlg, is->is_rrrec + totlg, lg)) {
	    Debug(" Field %d : \n    rrec=", i);
	    DebugHexa("", is->is_rrec + totlg, lg);
	    Debug("'\n   rrrec=");
	    DebugHexa("", is->is_rrrec + totlg, lg);
	    Debug("'\n");
	    }
	totlg += lg;
	}
}

/*NH JMP 21-12-02 */
IS_check_reread_dbl(ISAM *is)
{
    int     i;
    char    rbuf[80], rrbuf[80];
    double  rd, rrd;
    ISFIELD *isf;

    for(i = 0 ; i < is->is_nb_flds ; i++) {
	isf = is->is_flds + i;
	if(isf->isf_type != SCR_DOUBLE) continue;
	memcpy((char *)&rd,  is->is_rrec  + isf->isf_start, isf->isf_length);
	memcpy((char *)&rrd, is->is_rrrec + isf->isf_start, isf->isf_length);
	sprintf(rbuf,  "%.14le", rd);
	sprintf(rrbuf, "%.14le", rrd);
	if(strcmp(rbuf, rrbuf)) return(-1);
	}
    return(0);
}


/*NH */
/**************** GESTION DES FOREIGNS KEYS et FIELDS *******************/
/*                                                                      */
/* POUR ANNULER : IS_FOREIGN = 0;                                       */
/************************************************************************/

int     IS_FOREIGN = 1;

IS_check_foreign(is, type) // type = 0 : write, 1:rewrite, 2:delete
ISAM    *is;
int     type;
{
    int     i;

    if(IS_FOREIGN == 0) return(0);

    /* 1. ISREFS */
    if(type != 2) {
	/* Check that all foreign key are defined */
	for(i = 0 ; i < is->is_nb_isrefs ; i++)
	    if(IS_check_isref(is, i, type)) return(-1);
	}

    /* 2. ISRELS */
    /* Check that all isrels key are defined */
    if(type != 0) {
	for(i = 0 ; i < is->is_nb_isrels ; i++)
	    if(IS_check_isrel1(is, i, type)) return(-1);
	}

    return(0);
}
/*
IS_is_isf_empty(is, isf)
ISAM    *is;
ISFIELD *isf;
{
    return(IS_is_fld_empty(is->is_rec + isf->isf_start,
			   isf->isf_type, isf->isf_length));
}
*/

/*NH */
IS_check_isref(is, fld_nb, type)
ISAM    *is;
int     fld_nb;
int     type;       /* 0: IS_write, 1: IS_rewrite */
{
    ISFIELD     *isf, *isf2;
    COMPOSITE   *cmp, *cmp2;
    ISAM        *fis;
    ISREF       *isr;
    int         rc = 0, i, flag = 0;

    isr = is->is_isrefs + fld_nb;
    fis = ISAM_PTR(isr->isr_refisam);
    if(IS_open(fis, SCR_READ, SCR_NO_LOCK)) return(-1);

    /* Verification que le champ etranger existe (code postal existe) */
    if(isr->isr_type == 0) {
	isf = is->is_flds + isr->isr_isf_nb;
	isf2 = fis->is_flds + fis->is_cmps[isr->isr_refidx].cmp_flds[0];
	// Si champ non vide ET modifie (en cas de rewrite), recherche
	if(!IS_is_isf_empty(is, isf)) {
	    if(type && memcmp(is->is_rrec + isf->isf_start, is->is_rec + isf->isf_start, isf->isf_length) == 0)
		goto ended;
	    IS_empty_rec(fis);
	    memcpy(fis->is_rec + isf2->isf_start, is->is_rec + isf->isf_start, isf2->isf_length);
	    if(IS_search(fis, isr->isr_refidx, SCR_EQ1, 0L)) rc = -1;
	}
    }

    /* Verification que l'index etranger existe (code postal existe) */
    if(isr->isr_type == 1) {
	cmp = is->is_cmps + isr->isr_isf_nb;
	cmp2 = fis->is_cmps + isr->isr_refidx;
	IS_empty_rec(fis);
	for(i = 0 ; i < cmp->cmp_nb_flds ; i++) {
	    isf = is->is_flds + cmp->cmp_flds[i];
	    isf2 = fis->is_flds + cmp2->cmp_flds[i];
	    memcpy(fis->is_rec + isf2->isf_start, is->is_rec + isf->isf_start, isf2->isf_length);

	    // Si le champ est non vide :
	    if(!IS_is_isf_empty(is, isf)) {
		// Si write, il faudra faire un search (flag = 1)
		if(type == 0) flag = 1;
		// Si rewrite, il faudra faire un search si un des champs est modifi‚
		else {
		    if(memcmp(is->is_rrec + isf->isf_start,
			     is->is_rec + isf->isf_start, isf->isf_length))
		    flag = 1;
		    }
		}
	}

	if(flag == 0) goto ended;
	if(IS_search(fis, isr->isr_refidx, SCR_EQ, 0L)) rc = -1;
    }

ended:
    IS_close(fis);
    return(rc);

/*
    if(isf->isf_foreign == 0) return(0);
    if(isf->isf_foreign_chk == 0) return(0);
    if(IS_is_isf_empty(is, fld_nb)) return(0);
    IS_empty_rec(fis);
    IS_set_field(fis, isf->isf_foreign_fld, is->is_rec + isf->isf_start);
    if(IS_open(fis, SCR_READ, SCR_NO_LOCK)) return(-1);
    if(IS_search(fis, isf->isf_foreign_cmp, SCR_EQ1, 0L)) rc = -1;
    IS_close(fis);
    return(rc);
*/
}

/*NH*/
IS_check_isrels_rewrite(ISAM *is)
{
    int     rc, cmp_nb;

    cmp_nb = is->is_isrels->isr_refidx; // Premier composite
    rc = IS_check_isrels_rewrite_1(is, cmp_nb);
    return(rc);
}

IS_check_isrels_rewrite_1(ISAM *is, int cmp_nb)
{
    ISFIELD     *isf, *isf2;
    COMPOSITE   *cmp, *cmp2;
    ISAM        *fis;
    ISREF       *isl;
    int         rc = 0, i, flag = 0;

    // V‚rif. champ(s) modifi‚s
    cmp = is->is_cmps + cmp_nb;
    for(i = 0 ; i < cmp->cmp_nb_flds ; i++) {
	isf = is->is_flds + cmp->cmp_flds[i];
	if(memcmp(is->is_rrec + isf->isf_start,
		  is->is_rec + isf->isf_start, isf->isf_length)) break;
	}
    if(i == cmp->cmp_nb_flds) return(0); // No modif pour ce composite

    for(i = 0 ; i < is->is_nb_isrels ; i++) {
	// Si NoRewrite && Count > 0 : stop
	}

}

/*NH
IS_check_foreign_cmp_exist(is, cmp_nb)
ISAM    *is;
int     cmp_nb;
{
    COMPOSITE   *cmp = is->is_cmps + cmp_nb, *fcmp;
    ISFIELD     *isf;
    ISAM        *fis;
    int         rc = 0;

    if(cmp->cmp_foreign == 0) return(0);
    if(cmp->cmp_foreign_chk == 0) return(0);
    fis = ISAM_PTR(cmp->cmp_foreign);
    IS_empty_rec(fis);
    fcmp = fis->is_cmps + cmp->cmp_foreign_idx;
    for(i = 0 ; i < cmp->cmp_nb_flds ; i++) {
	isf = is->is_flds + cmp->cmp_flds[i];
	IS_set_field(fis, cmp->cmp_foreign_flds[i], is->is_rec + isf->isf_start);

    if(IS_open(fis, SCR_READ, SCR_NO_LOCK)) return(-1);
    if(IS_search(fis, isf->isf_foreign_cmp, SCR_EQ1, 0L)) rc = -1;
    IS_close(fis);
    return(rc);
}
*/

IS_cmp_rec(U_ch *rec1, U_ch *rec2, int type, int len)
{
    long    l1, l2;
    float   f1, f2;
    short   s1, s2;
    double  d1, d2;

    switch(type) {
	case SCR_LONG:
	case SCR_TIME:
	case SCR_DATE:
	case SCR_AUTO:
	case SCR_REF:
	    memcpy(&l1, rec1, sizeof(long));
	    memcpy(&l2, rec2, sizeof(long));
	    if(l1 != l2) {
		Debug("ISAM: %ld != %ld\n", l1, l2);
		return(-1);
	    }
	    break;
	case SCR_STRING:
	case SCR_ZSTRING:
	    if(memcmp(rec1, rec2, len) != 0) {
		Debug("ISAM: %s <> %s\n", rec1, rec2);
		return(-1);
	    }
	    break;
	case SCR_SHORT:
	case SCR_MENU:
	case SCR_TYPE_RADIO: /* BP_M 21-01-97 */
	case SCR_CHECK_BOX:  /* BP_M 21-01-97 */
	    memcpy(&s1, rec1, sizeof(short));
	    memcpy(&s2, rec2, sizeof(short));
	    if(s1 != s2) {
		Debug("ISAM: %d != %d\n", s1, s2);
		return(-1);
	    }
	    break;
	case SCR_FLOAT:
	    memcpy(&f1, rec1, sizeof(float));
	    memcpy(&f2, rec2, sizeof(float));
	    if(f1 != f2) {
		Debug("ISAM: %f != %f\n", f1, f2);
		return(-1);
	    }
	    break;
	case SCR_DOUBLE:
	    memcpy(&d1, rec1, sizeof(double));
	    memcpy(&d2, rec2, sizeof(double));
	    if(d1 != d2) {
		Debug("ISAM: %lf != %lf\n", d1, d2);
		return(-1);
	    }
	    break;
	case SCR_SHORT1:
	case SCR_CHAR:
	    if(rec1[0] != rec2[0]) {
		Debug("ISAM: %d != %d\n", rec1[0], rec2[0]);
		return(-1);
	    }
	    break;
    }
    return(0);
}

IS_set_db_type(char *dbstring, int type)
{
    int     i;
    FILE_TYPE *flt;
    char    *buf = SCR_stracpy(dbstring);

    for(i = 1 ; i < SCR_READ_FILES.tbl_nb_fl ; i++) {
	flt = SCR_READ_FILES.tbl_fl[i];
	if(TYPE16(flt->fl_type) != SCR_TYPE_ISAM) continue;

	IS_set_db_type_1((ISAM *)flt->fl_ptr, buf, type);
    }

    return(0);
}


IS_set_db_type_1(ISAM *is, char *dbstring, int type)
{
    is->is_type_dbd = type;
    is->is_db_string = dbstring;

    return(0);
}


