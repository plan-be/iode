#include "iode.h"

char    **KEXEC_VFILES = NULL,
	  **KEXEC_SFILES = NULL;

int     KEXEC_TRACE = 0;

int KI_strcmp(const char *pa, const char *pb)
{
    char *a = *(char **)pa, *b = *(char **)pb;

    if(a == 0) return -1;
    else if(b == 0) return 1;
    else return(strcmp(a, b));
}

KDB *KI_series_list(dbi)
KDB  *dbi;
{
    int     i, j, nb_names, ntbl;
    KDB     *dbv;
    LNAME   *lname;
    CLEC    *clec;
    char    **tbl;
    int     o_add_ptr_chunck = SCR_ADD_PTR_CHUNCK;

    SCR_ADD_PTR_CHUNCK = 1000;
    tbl = 0;
    ntbl = 0;

    // Ajoute dans un tableau toutes les noms de vars rencontrés sans vérifier les doublons
    for(i = 0; i < KNB(dbi); i++) {
	SCR_add_ptr(&tbl, &ntbl, KONAME(dbi, i));
	clec = KICLEC(dbi, i);
	lname    = &(clec->lnames[0]);
	nb_names = clec->nb_names;

	for(j = 0; j < nb_names; j++) {
	    if(L_ISCOEF(lname[j].name)) continue;
	    SCR_add_ptr(&tbl, &ntbl, lname[j].name);
	}
    }
    SCR_add_ptr(&tbl, &ntbl, 0);

    // Trie
    qsort(tbl, ntbl - 1, sizeof(char *), KI_strcmp);

    // Ajoute à la fin de la nouvelle dbv
    dbv = K_create(K_VAR, K_UPPER);
    for(i = 1; i < ntbl; i++) {
	if(tbl[i] == 0 || strcmp(tbl[i], tbl[i -1]))
	    K_add_entry(dbv, tbl[i - 1]);
    }

    SCR_free_tbl(tbl);
    SCR_ADD_PTR_CHUNCK = o_add_ptr_chunck;

    return(dbv);
}

/*
    Calcule la liste des scalaires utiles au calcul de identit‚s
    de la liste dbi. Retourne un pointeur vers une
    data base de scalaires.
*/
KDB *KI_scalar_list(dbi)
KDB  *dbi;
{
    int     i, j, nb_names;
    KDB     *dbs;
    LNAME   *lname;
    CLEC    *clec, *tclec;

    dbs = K_create(K_SCL, K_LOWER);
    for(i = 0; i < KNB(dbi); i++) {
	clec = KICLEC(dbi, i);
	tclec = SW_nalloc(clec->tot_lg);
	memcpy(tclec, clec, clec->tot_lg);
	lname    = &(tclec->lnames[0]);
	nb_names = tclec->nb_names;
	for(j = 0 ; j < nb_names ; j++) {
	    if(!L_ISCOEF(lname[j].name)) continue;
	    K_add_entry(dbs, lname[j].name);
	}
	SW_nfree(tclec); // JMP 26/8/2012
    }
    return(dbs);
}


/*
    Executes the identity DB using the input series of dbv
    or collects them from nv vfiles and its scalars in an
    analog way
*/
KDB *KI_exec(dbi, dbv, nv, vfiles, dbs, ns, sfiles, smpl)
KDB     *dbi, *dbv, *dbs;
int     nv, ns;
char    *vfiles[], *sfiles[];
SAMPLE  *smpl;
{
    KDB     *dbv_i, *dbs_i;
    int     *order;
    int     *KI_reorder();
    char    buf[80];

    if(smpl == 0) {
	B_seterrn(93);
	return((KDB *)0);
    }
    if(KSMPL(KV_WS)->s_nb != 0 &&
	    (PER_diff_per(&(KSMPL(KV_WS)->s_p2), &(smpl->s_p2)) < 0
	     || PER_diff_per(&(smpl->s_p1), &(KSMPL(KV_WS)->s_p1)) < 0) ) {
	B_seterrn(93);
	return((KDB *)0);
    }
    if(KNB(dbi) == 0) {
	B_seterrn(94);
	return((KDB *)0);
    }

    order = KI_reorder(dbi);
    if(order == 0) {
	B_seterrn(95);
	return((KDB *)0);
    }

    dbv_i = KI_series_list(dbi);
    if(KSMPL(KV_WS)->s_nb == 0) memcpy(KDATA(dbv_i), smpl, sizeof(SAMPLE));
    else  memcpy(KDATA(dbv_i), KSMPL(KV_WS), sizeof(SAMPLE));

    if(KEXEC_TRACE) {
	W_printf("\n.par1 tit_0\nIDENTITIES EXECUTION\n");
	W_printf(".par1 tit_1\nParameters\n");
	/*        W_printf(".par1 par_1\nList of identities :\n"); */
	W_printf(".par1 par_1\nExecution sample : %s\n",
		 PER_smpltoa(smpl, buf));
	W_printf(".par1 tit_1\nVariables load\n");
    }
    if(KI_read_vars(dbi, dbv_i, dbv, nv, vfiles)) {
	SW_nfree(order);
	K_free(dbv_i);
	return((KDB *)0);
    }

    dbs_i = KI_scalar_list(dbi);
    if(KEXEC_TRACE) W_printf(".par1 tit_1\nScalars load\n");
    if(KI_read_scls(dbs_i, dbs, ns, sfiles)) {
	SW_nfree(order);
	K_free(dbv_i);
	K_free(dbs_i);
	return((KDB *)0);
    }
    /*    if(KEXEC_TRACE) W_close();*/
    if(KEXEC_TRACE) W_flush();


    /*    KI_dbi_to_dbv(dbv_i, dbi); */
    KI_execute(dbv_i, dbs_i, dbi, order, smpl);
    //KI_extract(dbv_i, dbi);
    KI_quick_extract(dbv_i, dbi); // JMP 19/11/2012
    SW_nfree(order);
    K_free(dbs_i);
    return(dbv_i);
}

int *KI_reorder(dbi)
KDB   *dbi;
{
    int     i, j, nb, *order,
	    nb_order = 0,
	    nb_names, mod, pos;
    char    *mark;
    LNAME   *lname;
    CLEC    *clec;

    nb = KNB(dbi);
    order = (int *)SW_nalloc(sizeof(int) * nb);
    mark  = SW_nalloc(nb);

    while(nb_order < nb) {
	mod = 0;
	for(i = 0; i < nb ; i++) {
	    if(mark[i]) continue;
	    clec = KICLEC(dbi, i);
	    lname    = clec->lnames;
	    nb_names = clec->nb_names;
	    for(j = 0 ; j < nb_names ; j++) {
		if(strcmp(KONAME(dbi, i), lname[j].name) == 0) continue;
		pos = K_find(dbi, lname[j].name);
		if(pos < 0) continue;
		if(mark[pos]) continue;
		break;
	    }

	    if(j == nb_names) {
		mark[i] = mod = 1;
		order[nb_order++] = i;
	    }
	}

	if(mod == 0) {
	    /* IDENTITIES LOOP */
	    if(KEXEC_TRACE) { /* GB TRACE */
		for(i = 0; i < nb; i++) {
		    if(mark[i])
			W_printfEx(1, ".par1 enum_1\nIdt %s Ok\n", KONAME(dbi, i));
		    else
			W_printfEx(1, ".par1 enum_1\nIdt %s Circular\n", KONAME(dbi, i));
		}
	    }
	    SW_nfree(order);
	    SW_nfree(mark);
	    return((int *)0);
	}
    }

    SW_nfree(mark);
    return(order);
}


/*
    Lecture des s‚ries de base dans une liste de fichiers.
    Le sample est d‚termin‚ par dbv.

    Return -1 si erreur de fichier non trouv‚.
	   -2 si pas tout trouv‚.
	   -3 wrong sample
*/
KI_read_vars_db(dbv, dbv_tmp)
KDB  *dbv, *dbv_tmp;
{
    int     j, pos, nb_found = 0,
		    start, start_tmp;
    SAMPLE  smpl, *vsmpl, *tsmpl;

    vsmpl = (SAMPLE *) KDATA(dbv);
    tsmpl = (SAMPLE *) KDATA(dbv_tmp);

    PER_common_smpl(vsmpl, tsmpl, &smpl);
    if(smpl.s_nb > 0) {
	start     = PER_diff_per(&(smpl.s_p1), &(vsmpl->s_p1)); /* always >= 0 */
	start_tmp = PER_diff_per(&(smpl.s_p1), &(tsmpl->s_p1)); /* always >= 0 */
    }
    else return(-3);

    if(KEXEC_TRACE) W_printfEx(1, ".par1 enum_1\nFile %s : ", KNAMEPTR(dbv_tmp)); /* JMP 19-10-99 */
    for(j = 0 ; j < KNB(dbv); j++) {
	if(KSOVAL(dbv, j) != 0) continue;  /* series already present */

	pos = K_find(dbv_tmp, KONAME(dbv, j));
	if(pos < 0) continue;
	KSOVAL(dbv, j) = KV_alloc_var(vsmpl->s_nb);
	memcpy(KVVAL(dbv, j, start), KVVAL(dbv_tmp, pos, start_tmp),
	       sizeof(IODE_REAL) * smpl.s_nb);
	if(KEXEC_TRACE)
	    W_printf("%s ", KONAME(dbv, j));
	nb_found++;
    }

    if(KEXEC_TRACE) W_printf("\n");
    return(nb_found);
}

int KI_read_vars_file(dbv, file)
KDB     *dbv;
char    *file;
{
    KDB     *kdb;
    char    **vars = NULL;
    int     j, nbv = 0, nbf,
	    o_add_ptr_chunck = SCR_ADD_PTR_CHUNCK;

    SCR_ADD_PTR_CHUNCK = 1000;
    for(j = 0 ; j < KNB(dbv); j++) {
	if(KSOVAL(dbv, j) != 0) continue;
	SCR_add_ptr(&vars, &nbv, KONAME(dbv, j));
    }
    SCR_add_ptr(&vars, &nbv, NULL);
    SCR_ADD_PTR_CHUNCK = o_add_ptr_chunck;

    kdb = K_load(K_VAR, file, nbv, vars);
    if(kdb == 0) {
	B_seterrn(96, file);
	return(-1);
    }

    nbf = KI_read_vars_db(dbv, kdb);
    SCR_free_tbl(vars);
    K_free(kdb);

    return(nbf);
}

KI_read_vars(dbi, dbv, dbv_ws, nb, files)
KDB     *dbi, *dbv, *dbv_ws;
int     nb;
char    *files[];
{
    int     i, j, dim, nbf, nb_found = 0;

    if(nb == 0) {
	nbf = KI_read_vars_db(dbv, dbv_ws);
	if(nbf < 0) return(-1);
	nb_found += nbf;
    }
    else {
	for(i = 0;  i < nb && nb_found < KNB(dbv); i++) {
	    if(strcmp(files[i], "WS") == 0)
		nbf = KI_read_vars_db(dbv, dbv_ws);
	    else
		nbf = KI_read_vars_file(dbv, files[i]);

	    if(nbf < 0) return(-1);
	    nb_found += nbf;
	}
    }

    if(nb_found < KNB(dbv)) {
	dim = KSMPL(dbv)->s_nb;
	for(i = 0, j = 0 ; i < KNB(dbv) && j < 10; i++) {
	    if(KSOVAL(dbv, i) != 0) continue;  /* series already present */
	    if(K_find(dbi, KONAME(dbv, i)) >= 0) {
		K_add(dbv, KONAME(dbv, i), NULL, &dim);
		continue;
	    }
	    j++;
	    B_seterrn(90, KONAME(dbv, i));
	}
	if(j == 0) return(0);
	if(j == 10) B_seterrn(91);
	return(-2);
    }
    return(0);
}

/*
    Lecture des scalaires de base dans une liste de fichiers.

    Return -1 si erreur de fichier non trouv‚.
	   -2 si pas tout trouv‚.
*/

KI_read_scls_db(dbs, dbs_tmp)
KDB   *dbs, *dbs_tmp;
{
    char    filename[K_MAX_FILE + 1];
    int     j, pos, nb_found = 0;

    strcpy(filename, KNAMEPTR(dbs_tmp));      /* JMP 29-09-2015 */
    SCR_replace(filename, "\\", "\\\\");

    if(KEXEC_TRACE) W_printfEx(1, ".par1 enum_1\nFile %s : ", filename); /* JMP 19-10-99 */
    for(j = 0 ; j < KNB(dbs); j++) {
	if(KSOVAL(dbs, j) != 0) continue;

	pos = K_find(dbs_tmp, KONAME(dbs, j));
	if(pos < 0) continue;

	KSOVAL(dbs, j) = KS_alloc_scl();
	memcpy(KSVAL(dbs, j), KSVAL(dbs_tmp, pos), sizeof(SCL));
	if(KEXEC_TRACE) W_printf("%s ", KONAME(dbs, j));
	nb_found++;
    }

    if(KEXEC_TRACE) W_printf("\n");
    return(nb_found);
}

int KI_read_scls_file(dbs, file)
KDB     *dbs;
char    *file;
{
    KDB     *kdb;
    char    **scls = NULL;
    int     j, nbs = 0, nbf,
	    o_add_ptr_chunck = SCR_ADD_PTR_CHUNCK;

     SCR_ADD_PTR_CHUNCK = 1000;
     for(j = 0 ; j < KNB(dbs); j++) {
	if(KSOVAL(dbs, j) != 0) continue;
	SCR_add_ptr(&scls, &nbs, KONAME(dbs, j));
    }
    SCR_add_ptr(&scls, &nbs, NULL);
    SCR_ADD_PTR_CHUNCK = o_add_ptr_chunck;

    kdb = K_load(K_SCL, file, nbs, scls);
    if(kdb == 0) {
	B_seterrn(96, file);
	return(-1);
    }

    nbf = KI_read_scls_db(dbs, kdb);
    SCR_free_tbl(scls);
    K_free(kdb);

    return(nbf);
}

KI_read_scls(dbs, dbs_ws, nb, files)
KDB     *dbs, *dbs_ws;
int     nb;
char    *files[];
{
    int     i, j, nbf, nb_found = 0;

    if(nb == 0) {
	nbf = KI_read_scls_db(dbs, dbs_ws);
	if(nbf < 0) return(-1);
	nb_found += nbf;
    }
    else {
	for(i = 0;  i < nb && nb_found < KNB(dbs); i++) {
	    if(strcmp(files[i], "WS") == 0)
		nbf = KI_read_scls_db(dbs, dbs_ws);
	    else
		nbf = KI_read_scls_file(dbs, files[i]);

	    if(nbf < 0) return(-1);
	    nb_found += nbf;
	}
    }

    if(nb_found < KNB(dbs)) {
	for(i = 0, j = 0 ; i < KNB(dbs) && j < 10; i++) {
	    if(KSOVAL(dbs, i) != 0) continue;  /* series already present */
	    j++;
	    B_seterrn(92, KONAME(dbs, i));
	}
	if(j == 10) B_seterrn(91);
	return(-2);
    }
    return(0);
}


/*
    Adds to dbv series to be calculated (from dbi)
*/
/*
KI_dbi_to_dbv(dbv, dbi)
KDB     *dbv,  *dbi;
{
    int     i, nb, pos;

    nb = KSMPL(dbv)->s_nb;
    for(i = 0; i < KNB(dbi); i++)  {
	if(K_find(dbv, KONAME(dbi, i)) < 0) {
	    K_add(dbv, KONAME(dbi, i), NULL, &nb);
	    }
    }

}
*/
/*
    Link and execute dbi
*/

KI_execute(dbv, dbs, dbi, order, smpl)
KDB     *dbv, *dbs, *dbi;
int     *order;
SAMPLE  *smpl;
{
    int     i, t, pos, tot_lg, start;
    char    *tmp;
    double  d;

    start = PER_diff_per(&(smpl->s_p1), &(KSMPL(dbv)->s_p1));
    if(start < 0) start = 0;

    for(i = 0; i < KNB(dbi); i++) {
	tot_lg = KICLEC(dbi, order[i])->tot_lg;
	tmp = SW_nalloc(tot_lg);
	memcpy(tmp, KICLEC(dbi, order[i]), tot_lg);
	if(L_link(dbv, dbs, tmp)) return(-1);
	pos = K_find(dbv, KONAME(dbi, order[i]));
	for(t = start ; t < start + smpl->s_nb ; t++) {
	    d = L_exec(dbv, dbs, tmp, t);
	    *(KVVAL(dbv, pos, t)) = d;
	}
	SW_nfree(tmp);
    }

    return(0);
}

/*********************************************************************
Delete from dbv the variables not existing in dbi. The result is thus
a KDB dbv containing the vars computed from the  identities in dbi.
(identities have the name of the output var).
=> Old version replaced by KI_quick_extract() (see below)
**********************************************************************/
KI_extract(dbv, dbi)
KDB   *dbv, *dbi;
{
    int     i, pos;

    //for(i = KNB(dbv) - 1; i >= 0 ; i--) {
    for(i = 0 ; i < KNB(dbv) ; i++) {
	pos = K_find(dbi, KONAME(dbv, i));
	if(pos < 0) {
	    K_del(dbv, i);
	    i--;
	}
    }
    return(0);
}

/********************************************************************
Reconstruct dbv with the variables whose names are found in dbi. The result is thus
a KDB dbv containing the vars computed from the identities in dbi.
(identities have the name of the output var).
********************************************************************/
KI_quick_extract(dbv, dbi) // JMP 19/11/2012
KDB   *dbv, *dbi;
{
    int     i, j, pos, *objsnb, nbres;
    KOBJ    *newobjs;

    // Store in objsnb the nb of the objects to delete
    nbres = 0;
    objsnb = (long *) SW_nalloc(sizeof(long) * KNB(dbv));
    for(i = 0 ; i < KNB(dbv); i++) {
	pos = K_find(dbi, KONAME(dbv, i));
	if(pos >= 0) {
	    objsnb[i] = 1;
	    nbres++;
	}
    }

    // Copy left objs in a new tmp table of KOBJ and delete the others
    newobjs = (KOBJ *) SW_nalloc((unsigned int)(sizeof(KOBJ) * K_CHUNCK * (1 + (nbres - 1) / K_CHUNCK)));
    for(i = j = 0 ; i < KNB(dbv); i++) {
	if(objsnb[i]) {
	    memcpy(newobjs + j, KOBJS(dbv) + i, sizeof(KOBJ));
	    j++;
	}
	else {
	    SW_free(KSOVAL(dbv, i));
	}
    }

    SW_nfree(KOBJS(dbv));
    KOBJS(dbv) = newobjs;
    KNB(dbv) = nbres;
    SW_nfree(objsnb);
    return(0);
}










