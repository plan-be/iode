#include "iode.h"

#define START_TM1       0
#define START_TM1_A     1
#define START_EXTRA     2
#define START_EXTRA_A   3
#define START_ASIS      4
#define START_TM1_NA    5
#define START_EXTRA_NA  6

#define SORT_CONNEX    0
#define SORT_BOTH      1
#define SORT_NONE      2

IODE_REAL    *KSIM_NORMS = 0;	// Norme de convergence par année JMP 21/3/2012
int		*KSIM_NITERS = 0; 	// Nombre d'itérations par année JMP 21/3/2012
IODE_REAL    KSIM_EPS = 0.001,
        KSIM_RELAX = 1.0,
        KSIM_NORM, *KSIM_XK, *KSIM_XK1;
int     KSIM_MAXIT= 100,
        KSIM_DEBUG = 0,
        KSIM_MAXDEPTH,
        *KSIM_POSXK, *KSIM_ENDO;
int     KSIM_C, KSIM_T, KSIM_IT, KSIM_NB, KSIM_PASSES = 5;
int     KSIM_SORT = SORT_BOTH,
        KSIM_START = START_TM1;
char    **KSIM_EXO = NULL,
          *KSIM_PATH = NULL;
KDB     *KSIM_DBV, *KSIM_DBS, *KSIM_DBE;

/*
char    *KSIM_VAR_NBITER = NULL, // Nombre d'itérations pour converger (ou pas)
        *KSIM_VAR_EPS = NULL;    // Critère de convergence pour chaque it 
*/
/* ORDER */

int     KSIM_PRE, KSIM_INTER, KSIM_POST, *KSIM_ORDER, *KSIM_PERM;
char    *KSIM_ORDERED;

#define KSIM_VAL(i, t)      *(KVVAL(KSIM_DBV, KSIM_POSXK[i], t))
#define KSIM_NAME(i)        (KONAME(KSIM_DBV, KSIM_POSXK[i]))


/*
    exo == NULL
	    normal simulation;
    exo list of strings "X-Y"
	    goalseeking the equation X will be simulated with
	    Y as endogenous variable
*/
KE_simul_s(dbe, dbv, dbs, smpl, exo, eqs)
KDB     *dbe, *dbv, *dbs;
SAMPLE  *smpl;
char    **exo;
char    **eqs;
{
    int     i, t, j, k, exonb, lg,
            posendo, posexo,
            rc = -1;
    char    **var = NULL;
    IODE_REAL    *x;

    if(KNB(dbe) == 0) {
        B_seterrn(110);
        return(rc);
    }

    KSIM_DBV = dbv;
    KSIM_DBE = dbe;
    KSIM_MAXDEPTH = KNB(dbe);
    KSIM_DBS = dbs;

    t = PER_diff_per(&(smpl->s_p1), &(KSMPL(dbv)->s_p1));
    if(t < 0 || PER_diff_per(&(KSMPL(dbv)->s_p2), &(smpl->s_p2)) < 0) {
        B_seterrn(111);
        return(rc);
    }

    // KSIM_POSXK[i] = num dans dbv de la var endogène de l'équation i
    KSIM_POSXK = (int *) SW_nalloc((int)(sizeof(int) * KNB(dbe)));

 	// Initialise les nouvelles vars pour conserver les résultats de sim 
	SCR_free(KSIM_NORMS);
	SCR_free(KSIM_NITERS);
	KSIM_NORMS = (IODE_REAL *) SCR_malloc(sizeof(IODE_REAL) * KSMPL(dbv)->s_nb);
	KSIM_NITERS = (int *) SCR_malloc(sizeof(int) * KSMPL(dbv)->s_nb);

    /* LINK EQUATIONS + SAVE ENDO POSITIONS */
    kmsg("Linking equations ....");
    for(i = 0 ; i < KNB(dbe); i++) {
        KSIM_POSXK[i] = K_find(dbv, KONAME(dbe,i));
        if(KSIM_POSXK[i] < 0) {
            B_seterrn(112, KONAME(dbe, i));
            rc = -1;
            goto fin;
        }

        rc = L_link(dbv, dbs, KECLEC(dbe, i));
        if(rc) {
            B_seterrn(113, KONAME(dbe, i));
            rc = -1;
            goto fin;
        }
    }

    /* Goal Seeking */
    if(exo != NULL) {
        exonb = SCR_tbl_size(exo);
        KSIM_PATH = SW_nalloc(KSIM_MAXDEPTH);
        for(i = 0; i < exonb; i ++) {
            var = SCR_vtom(exo[i], '-');
            if(var == NULL || SCR_tbl_size(var) != 2) {
                B_seterrn(115, exo[i]);
                rc = -1;
                goto fin;
            }

            posendo = K_find(KSIM_DBV, var[0]);
            posexo = K_find(KSIM_DBV, var[1]);
            /*          fprintf(stdout, "\n====Exchanging %s %s\n====", var[0], var[1]);*/
            if(KE_exo2endo(posendo, posexo) < 0) {
                rc = -1;
                goto fin;
            }

            /*          fprintf(stdout, "\nDone");*/
            SCR_free_tbl(var);
            var = NULL;
        }

    }

    /* ORDERING EQUATIONS */
    KE_order(dbe, eqs);
    if(KSIM_DEBUG) K_lstorder("_PRE", "_INTER", "_POST");
	
    /* SIMULATE */
	KSIM_XK  = (IODE_REAL *) SW_nalloc(sizeof(IODE_REAL) * KSIM_INTER);
    KSIM_XK1 = (IODE_REAL *) SW_nalloc(sizeof(IODE_REAL) * KSIM_INTER);

    for(i = 0; i < smpl->s_nb; i++, t++) {
        if(rc = K_simul_1(t)) goto fin;
        if(exo != NULL) {
            for(k = 0; k < exonb; k ++) {
                var = SCR_vtom(exo[k], '-');
                posexo = K_find(KSIM_DBV, var[1]);

                x = KVVAL(KSIM_DBV, posexo, 0);
                for(j = t + 1; j < KSMPL(dbv)->s_nb; j++)  x[j] = x[t];

                SCR_free_tbl(var);
                var = NULL;
            }
        }
    }

fin:
    SCR_free_tbl(var);
	KE_simul_free();
    return(rc);
}

KE_simul_free()
{
    SW_nfree(KSIM_XK);
    SW_nfree(KSIM_XK1);
    SW_nfree(KSIM_POSXK);
    SW_nfree(KSIM_ORDER);
    SW_nfree(KSIM_PATH);
    KSIM_PATH = NULL;
    KSIM_XK = KSIM_XK1 = KSIM_POSXK = KSIM_ORDER = NULL;
	return(0);
}


/* ==== SIMULATE ====================================================
    Simulate one period
    t : IN : index of period
    return : -1 if no convergence, 0 if converge
    ================================================================= */

K_simul_1(t)
int     t;
{
    extern  int SCR_vtime;
	int     it = 0, rc, conv = 0, ovtime = SCR_vtime; /* JMP 27-09-96 */
    char    buf[10], msg[80];

    K_init_values(t);
	KSIM_NITERS[t] = 0; // JMP 21/3/2012
	KSIM_NORMS[t] = 0;  // JMP 21/3/2012
    if(K_prolog(t)) return(-1);
    SCR_term_vkey(0);
    while(conv == 0 && it++ < KSIM_MAXIT) {
		rc = K_interdep(t);
		KSIM_NITERS[t]++; 			// JMP 21/3/2012
		KSIM_NORMS[t] = KSIM_NORM;	// JMP 21/3/2012
		if(rc) {
            SCR_term_vkey(ovtime);  /* JMP 27-09-96 */
            return(-1);
        }
        PER_pertoa(PER_addper(&(KSMPL(KSIM_DBV)->s_p1), t), buf);
        sprintf(msg, "%s %d iterations ; error = %8.4lg",
                buf, it, KSIM_NORM);
        kmsg("%.80s", msg);
        conv = (KSIM_NORM <= KSIM_EPS) ? 1 : 0;
        if(SCR_hit_key() != 0) {
            SCR_get_key();
            if(!SCR_confirme("Stop Simulation")) {
                K_restore_XK(t);
                SCR_term_vkey(ovtime);  /* JMP 27-09-96 */
                return(-1);
            }
        }
    }
    SCR_term_vkey(ovtime);  /* JMP 27-09-96 */

    if(conv) {
        K_restore_XK(t);
        if(K_epilog(t)) return(-1);
        return(0);
    }
    else {
        B_seterrn(114, KSIM_MAXIT, "_DIVER");
        K_restore_XK(t);
        K_diverge(t, "_DIVER", KSIM_EPS);
    }

    return(-1);
}

K_init_values(t)
int     t;
{
    int         i;
    IODE_REAL        *val;

    if(KSIM_START == START_ASIS) return(0);
    /*
        for(i = KSIM_PRE ; i < KSIM_PRE + KSIM_INTER ; i++) {
    /* JMP 06-02-95 */
    for(i = 0 ; i < KSIM_PRE + KSIM_INTER + KSIM_POST; i++) {
        val = KVVAL(KSIM_DBV, KSIM_POSXK[KSIM_ORDER[i]], 0);
        K_init_values_1(val, t, KSIM_START);
    }
    return(0);
}

K_init_values_1(val, t, method)IODE_REAL    *val;
int     t, method;
{

    switch(method) {
        case START_TM1 :
            if(L_ISAN(val[t]) && !L_IS0(val[t])) return(0);
            goto calc1;
        case START_TM1_NA :
            if(L_ISAN(val[t])) return(0);
            goto calc1;
        case START_TM1_A :
calc1:
            val[t] = 1.0;
            if(t > 0 && L_ISAN(val[t - 1])) val[t] = val[t - 1];
            return(0);
        case START_EXTRA :
            if(L_ISAN(val[t]) && !L_IS0(val[t])) return(0);
            goto calc2;
        case START_EXTRA_NA :
            if(L_ISAN(val[t])) return(0);
            goto calc2;
        case START_EXTRA_A :
calc2:
            val[t] = 1.0;
            if(t > 0 && L_ISAN(val[t - 1])) {
                if(t > 1 && L_ISAN(val[t - 2]))
                    val[t] = 2 * val[t - 1] - val[t - 2];
                else val[t] = val[t - 1];
            }
        case START_ASIS :
            return(0);
    }
    return(0);
}

K_restore_XK(t)
int     t;
{
    int         i, j;

    for(i = KSIM_PRE, j = 0; j < KSIM_INTER; i++, j++)
        KSIM_VAL(KSIM_ORDER[i], t) = KSIM_XK[j];
    return(0);
}
 IODE_REAL K_calc_clec(eqnb, t, varnb, msg)
int     eqnb, t, varnb, msg;
{
    int     lg, eqvarnb = -1;
    CLEC    *clec;
    IODE_REAL    x;
    char    buf[10];

//Debug("Eq %s\n", KONAME(KSIM_DBE, eqnb));
    lg = KECLEC(KSIM_DBE, eqnb)->tot_lg;
    clec = (CLEC*) SW_nalloc(lg);
    memcpy(clec, KECLEC(KSIM_DBE, eqnb), lg);
    eqvarnb = K_find(KSIM_DBV, KONAME(KSIM_DBE, eqnb));
    if(clec->dupendo || varnb != eqvarnb)
        x = L_zero(KSIM_DBV, KSIM_DBS, clec, t, varnb, eqvarnb);
    else
        x = L_exec(KSIM_DBV, KSIM_DBS, clec, t);
    if(!L_ISAN(x) && msg)
        kerror(0, "%s : becomes unavailable at %s%s",
               KONAME(KSIM_DBV, varnb), /* JMP 16-06-99 a la place de eqvarnb */
               PER_pertoa(PER_addper(&(KSMPL(KSIM_DBV)->s_p1), t), buf),
               ((clec->dupendo || varnb != eqvarnb) ? "(Newton)" : "")
              );
    SW_nfree(clec);
    return(x);
}

K_prolog(t)
int     t;
{
    int     i;
    IODE_REAL    x;

    for(i = 0; i < KSIM_PRE; i++)  {
        x = K_calc_clec(KSIM_ORDER[i], t, KSIM_POSXK[KSIM_ORDER[i]], 0);
        KSIM_VAL(KSIM_ORDER[i], t) = x;
        // if(!L_ISAN(x)) return(-1); /* JMP 13-04-00 */
    }
    return(0);
}

K_interdep(t)
int     t;
{
    int         rc;
    double      relax = KSIM_RELAX;

    KSIM_RELAX = fabs(relax);
    if(relax >= 0)      rc = K_interdep_1(t);
    else                rc = K_interdep_2(t);

    KSIM_RELAX = relax;
    return(rc);
}


/* ======================================================================
Compute the interdep part of the model.
For each eq:
&EN save the previous value in KSIM_XK
&EN computes the endo value x and save it in DB (via marco KSIM_VAL) after
     "relaxing" it
&EN computes the "norm" of F(x) in KSIM_NORM (must be close to 0)
========================================================================= */
K_interdep_1(t)
int     t;
{
    int     i, j;
    IODE_REAL    x;
    double  d, pd, fpd1, fpd2;


    KSIM_NORM = 0.0;
    for(i = KSIM_PRE, j = 0; j < KSIM_INTER; i++, j++)  {
        /* save XK first */
        KSIM_XK[j] = KSIM_VAL(KSIM_ORDER[i], t);

        /* execute lec */
        x = K_calc_clec(KSIM_ORDER[i], t, KSIM_POSXK[KSIM_ORDER[i]], 1);
        if(!L_ISAN(x)) return(-1);

        /* Check convergence */
        if(L_ISAN(KSIM_XK[j])) {
            /* ????????????? 2 keer RELAX als x < 1
            d = (KSIM_XK[j] - x) * KSIM_RELAX;
            */

            d = (KSIM_XK[j] - x);
            if(!L_IS0(KSIM_XK[j])) pd = min(fabs(1 - x / KSIM_XK[j]), fabs(d));
            else pd = fabs(d);

            pd *= KSIM_RELAX;
            if(pd > KSIM_NORM) KSIM_NORM = pd;

            /* Store new value and relax it */
            KSIM_VAL(KSIM_ORDER[i], t) = KSIM_RELAX * (x - KSIM_XK[j]) +
                                         KSIM_XK[j];
        }
        else {
            KSIM_NORM = 10;
            KSIM_VAL(KSIM_ORDER[i], t) = x;
        }
    }

    return(0);
}

/* ======================================================================
Compute the interdep part of the model.
Version avec application de lambda après la boucle de calcul et pas dans la boucle
comme dans K_interdep().

Stage 1 :
For each eq :
&EN save the previous value in KSIM_XK
&EN computes the endo value x and save it in KSIM_XK1 (no "relax" at this stage

Stage 2 :
&EN computes the "norm" of F(x) in KSIM_NORM (must be close to 0)

========================================================================= */
K_interdep_2(t)
int     t;
{
    int     i, j;
    IODE_REAL    x;
    double  d, pd, fpd1, fpd2;

    // Stage 1
    for(i = KSIM_PRE, j = 0; j < KSIM_INTER; i++, j++)  {
        /* save XK for further use */
        KSIM_XK[j] = KSIM_VAL(KSIM_ORDER[i], t);

        /* execute lec and save in KSIM_XK1 */
        KSIM_XK1[j] = K_calc_clec(KSIM_ORDER[i], t, KSIM_POSXK[KSIM_ORDER[i]], 1);
        if(!L_ISAN(KSIM_XK1[j])) return(-1); // NaN value --> quit simul
    }

    // Stage 2
    KSIM_NORM = 0.0;
    for(i = KSIM_PRE, j = 0; j < KSIM_INTER; i++, j++)  {
        if(L_ISAN(KSIM_XK[j])) { // Valeur précédente définie
            d = KSIM_XK[j] - KSIM_XK1[j]; // Diff between iterations

            // Calcule la 'norme' = fabs de la différence relative entre 2 it.
            //   ou de la diff entre 2 it.
            if(!L_IS0(KSIM_XK[j]))
                pd = min(fabs(1 - KSIM_XK1[j] / KSIM_XK[j]), fabs(d));
            else
                pd = fabs(d);

            pd *= KSIM_RELAX;
            if(pd > KSIM_NORM) KSIM_NORM = pd; // norme : la plus grande pour le mod.

            /* Store new value and relax it */
            KSIM_VAL(KSIM_ORDER[i], t) =
                KSIM_RELAX * KSIM_XK1[j] + (1 - KSIM_RELAX) * KSIM_XK[j];
        }
        else { // Si pas de valeur à l'it précédente, norme = 10, val = nouvelle val
            KSIM_NORM = 10;
            KSIM_VAL(KSIM_ORDER[i], t) = KSIM_XK1[j];
        }
    }

    return(0);
}


K_epilog(t)
int     t;
{
    int     i, j;
    IODE_REAL    x;


    for(i = KSIM_PRE + KSIM_INTER, j = 0; j < KSIM_POST; i++, j++)  {
        x = K_calc_clec(KSIM_ORDER[i], t, KSIM_POSXK[KSIM_ORDER[i]], 0);
        KSIM_VAL(KSIM_ORDER[i], t) = x;
        // if(!L_ISAN(x)) return(-1);  /* JMP 13-04-00 */
    }

    return(0);
}

/**********************************************************************************************
Ordonnance le modèle avant la simulation.
dbe : IN  : KDB d'équations (limitée si liste des équations à simuler dans l'ordre alpha)
eqs : IN  : char ** avec les équations (idem dbe mais avec l'ordre non alphabétique)

INPUT GLOBAL : KSIM_SORT = SORT_BOTH, SORT_NONE, ...

Résultats :
  KSIM_ORDER = ordre d'exécution des eqs
  KSIM_PRE = # eqs pré
  KSIM_INTER = # eqs interdep
  KSIM_POST = # eqs post

Cas 1 : SORT_NONE
-----------------
 - KSIM_ORDER dans l'ordre des eqs
 - KSIM_PRE = KSIM_POST = 0
 - KSIM_INTER = # eqs tot

Cas 2 : Calcule les CFC (CFC = SCC = Strong Connex Components)
-----------------
 - KSIM_ORDER dans l'ordre de calcul des eqs
 - KSIM_PRE = nombre d'eqs pre
 - KSIM_POST = nombre d'eqs post
 - KSIM_INTER =  = nombre d'eqs interdep

**********************************************************************************************/

KE_order(dbe, eqs)
KDB     *dbe;
char    **eqs;
{
    int     **tmp, **tmpi, *tmp2, i, k, nb;

	nb = KNB(dbe);
	// Pas de réord : on garde l'ordre de eqs et donc tout est interdep
    if(KSIM_SORT == SORT_NONE) {
        KSIM_PRE = KSIM_POST = 0;
        KSIM_INTER = nb;
        KSIM_ORDER = (int *)  SW_nalloc(sizeof(int) * nb);
        if(eqs == 0)
            for(i = 0; i < KSIM_INTER; i++) KSIM_ORDER[i] = i;
        else
            for(i = 0; i < KSIM_INTER; i++) KSIM_ORDER[i] = K_find(dbe, eqs[i]); /* JMP 02-02-2004 */

        return(0);
    }

    kmsg("Sorting equations ....");

	// tmp = liste de pointeur vers des vecteurs contenant la pos de toutes les vars de l'eq i
	// voir preorder
    tmp = (int **) SW_nalloc(sizeof(int *) * nb);
    tmpi = (int **) SW_nalloc(sizeof(int *) * nb);
    KE_preorder(dbe, tmp, tmpi);
    KSIM_PRE = KE_pre(dbe, tmp, 0);
    KSIM_POST = KE_pre(dbe, tmpi, KSIM_PRE);

    /* REVERSE FOR EXECUTION PURPOSE */
    for(i = 0; i < KSIM_POST / 2; i++) {
        k = KSIM_ORDER[KSIM_PRE + i];
        KSIM_ORDER[KSIM_PRE + i] = KSIM_ORDER[KSIM_PRE + (KSIM_POST - 1) - i];
        KSIM_ORDER[KSIM_PRE + (KSIM_POST - 1) - i] = k;
    }

    KSIM_INTER = KE_interdep(dbe, tmp);
    kmsg("#PRE %d - #INTER %d - #POST %d", KSIM_PRE, KSIM_INTER, KSIM_POST);

    /* A AMELIORER !! */
    tmp2 = (int *) SW_nalloc(sizeof(int) * (KSIM_INTER + KSIM_POST));
    memcpy(tmp2, KSIM_ORDER + KSIM_PRE, (KSIM_INTER + KSIM_POST) * sizeof(int));
    memcpy(KSIM_ORDER + KSIM_PRE, tmp2 + KSIM_POST, KSIM_INTER * sizeof(int));
    memcpy(KSIM_ORDER + KSIM_PRE + KSIM_INTER, tmp2, KSIM_POST * sizeof(int));
    SW_nfree(tmp2);

    if(KSIM_SORT == SORT_BOTH) KE_tri(dbe, tmp, KSIM_PASSES);
    KE_postorder(dbe, tmp, tmpi);
    return(0);
}

/********************************************************************************************
Prépare l'ordonnancement du modèle en constituant des vecteurs tmp et tmpi.

INPUT :
	dbe = modèle
	tmp et tmpi = vecteur pré alloué de int *, 1 par eq

OUTPUT : 
	KSIM_ORDERED = vecteur alloué avec un char par eq pour indiqué que eq est 
				   déjà "placée" dans un bloc -> non calculé ici
	KSIM_ORDER   = vecteur alloué avec l'ordre d'exécution du mod. -> non calculé ici
	tmp  = vecteur pour chaque eq avec la liste des eqs dont elle dépend (-1 pour endog)
	tmpi = vecteur pour chaque eq avec la liste des eqs qui en dépendent
	tmp[i] et tmpi[i] : pour chaque eq 
		tmp[i][0] = nb elements
		tmp[i][1...] = numéro de l'eq 
	
*********************************************************************************************/

KE_preorder(dbe, tmp, tmpi)
KDB     *dbe;
int     **tmp, **tmpi;
{
	int     i, j, pos, posj, nb;
    CLEC    *clec;

    nb = KNB(dbe);
    KSIM_ORDER    = (int *)  SW_nalloc(sizeof(int) * nb);
    KSIM_ORDERED  = (char *) SW_nalloc(sizeof(char) * nb);

    for(i = 0; i < nb; i ++) {
        clec = KECLEC(dbe, i);
		tmp[i] = (int *) SW_nalloc(sizeof(int) * (clec->nb_names + 1)); // alloue (nb names + 1) long 

        /* LOG ALL NB AND POS OF ENDO VARS */
		tmp[i][0] = clec->nb_names; // Nbre max de noms (on pourrait améliorer en ne prenant que les vars)
        for(j = 0; j < clec->nb_names; j++) {
            if(L_ISCOEF(clec->lnames[j].name)) continue;
			// Recherche l'eq dont la variable j est endo
			posj = (clec->lnames[j]).pos;
			if(KSIM_POSXK[i] == posj)  // améliore les performances JMP 11/3/2012 -- CHECK!
				pos = -1; // Endo de l'eq courante
			else {
				pos = KE_poseq(posj);
				if(pos >= 0 && pos == i) pos = -1; // si var pos est l'endogène => -1
            }
			tmp[i][j + 1] = pos;
            if(pos >= 0) KE_add_post(tmpi, i, pos);
        }
    }
    return(0);
}

/********************************************************************************************
KE_poseq(varnb) : recherche l'eq dont varnb est l'endogène sur base de KSIM_POSXK

INPUT : 
	KSIM_MAXDEPTH = nb eq dans dbe
	KSIM_POSXK[i] = num de la var endogène de l'equation i 

RETURN : 
	- num de l'éq dont posendo est l'endogène si c'est une endog
	- -1 si exo (non trouvé)
********************************************************************************************/

int KE_poseq(posendo)
int posendo;
{
    int     i;

    if(posendo < KSIM_MAXDEPTH && KSIM_POSXK[posendo] == posendo) return(posendo);

	// Recherche l'eq avec pour endogène posendo ie i tq posendo = KSIM_POSXK[i]
    for(i = 0; i < KSIM_MAXDEPTH; i++)
        if(KSIM_POSXK[i] == posendo) return(i);

    return(-1);
}

/********************************************************************************************
KE_add_post : 
Soit l'eqi suivante : A := B + C. 
Supposons que B soit endogène d'une autre eq, mettons eqj
add_post(tmpi, eqj, eqi) ajoute eqi à la liste tmpi[eqj] des vars de eqj qui sont endog d'autres eqs

A la fin du processus, tmpi contient pour chaque eq toutes celles qui en dépendent. 
C'est l'inverse de tmp qui contient pour chaque eq toutes celles dont elle dépend.

INPUT : 
	tmpi = vecteur de int *, un par eq
	i =  num de l'eq
	pos = pos de l'eq dont l'endog appartient à la formule de l'eq i

********************************************************************************************/

KE_add_post(tmpi, i, pos)
int     **tmpi, i, pos;
{
    int     nb = 1;

    if(tmpi[pos] == 0)
        tmpi[pos] = (int *) SW_nalloc(sizeof(int) * 2);
    else {
        nb = tmpi[pos][0] + 1;
        tmpi[pos] = (int *) SW_nrealloc(tmpi[pos], nb * sizeof(int),(nb + 1) * sizeof(int));
    }
    tmpi[pos][0]  = nb;
    tmpi[pos][nb] = i;
    return(0);
}

/************************************************************************************************
Libère les vecteurs temporaires utilisés pour le réordonnancement du modèle.
**************************************************************************************************/

KE_postorder(dbe, tmp, tmpi)
KDB     *dbe;
int     **tmp, **tmpi;
{
    int     i;

    for(i = 0; i < KNB(dbe); i ++)  {
        SW_nfree(tmp[i]);
        SW_nfree(tmpi[i]);
    }
    SW_nfree(tmp);
    SW_nfree(tmpi);
    SW_nfree(KSIM_ORDERED);
    return(0);
}

/************************************************************************************************
Calcule le bloc pré-récursif (ou post-récursif), càd ne dépendant que d'eq précédemment calculées.

INPUT :
	KDB *dbe  = équations qui forment le modèle à simuler
	int **tmp = vecteur des vecteurs avec les nums des variables de chaque eq de dbe
	int from = première place libre dans KSIM_ORDER
	
OUTPUT : 
	KSIM_ORDERER (IN/OUT) = vecteur avec 1 pour les eqs déjà ordonnées
	KSIM_ORDER   (IN/OUT) = vecteur avec l'ordre d'exécution des eqs 

RETURN : 
	Nombre d'équations dans le bloc récursif calculé
	
Méthode : 
	Pout tout eq i
		Analyser ses vars (dans le vecteur tmp[i]) :
			soit la varj de l'équation = tmp[i][j+1]
			si EXO (varj < 0) ou déjà dans le bloc (KSIM_ORDERED[varj] == 1) : passer à eq i+1
			sinon :
				noter 1 dans KSIM_ORDERED[varj] 
				KSIM_ORDER[from + nb] = i
				mettre c à 1 (c pour continue ?)
	Si aucune eq i n'a été ordered à la fin de la boucle, STOP
	Sinon, on recommence en espérant en placer de nouvelles ne dépendant que des eq déjà "ordered"

**************************************************************************************************/

KE_pre(dbe, tmp, from)
KDB     *dbe;
int     **tmp, from;
{
    int     i, j, pos,
            c = 1, nb = 0;

	// On recommence tant qu'on a ajouté une éq dans le groupe PRE (c == 1)
	// car on a peut être une nouvelle éq qui ne dépendait que de celles qui viennent d'être 
	// ajoutée et qui donc fait aussi partie du groupe
    while(c == 1) {
        c = 0;
        for(i = 0; i < KNB(dbe); i ++) {
			if(KSIM_ORDERED[i] == 1) continue; // Equation déjà classée (i.e. dans le bloc PRE)
            if(tmp[i]) {
				for(j = 0; j < tmp[i][0]; j++) {  	// tmp[i][0] = nb vars de l'éq i
					if(tmp[i][j + 1] < 0) continue; /* VAR j+1 = ENDO de l'eq courante */
					if(KSIM_ORDERED[tmp[i][j + 1]] == 1) continue;  /* Var j+1 = déjà classée (ORDERED) */
					break; // Var j + 1 non EXO et non classée, donc eq ne peut être retenue dans le bloc
                }
            }
			if(tmp[i] == 0 || j == tmp[i][0]) { // Pas de var dans l'eq OU toutes exo OU classées
				c = KSIM_ORDERED[i] = 1;    
                KSIM_ORDER[from + nb] = i;
                nb ++;
            }
        }
    }
    return(nb);
}

/************************************************************************************************
Calcule le bloc interdépendant et place le num des équations à la fin de KSIM_ORDER.
On a donc au final KSIM_ORDER composé comme suit : 
	KSIM_PRE eqs
	KSIM_POST eqs
	KSIM_INTER eqs

INPUT :
	KDB *dbe  = équations qui forment le modèle à simuler
	int **tmp = vecteur des vecteurs avec les nums des variables de chaque eq de dbe
	
OUTPUT : 
	KSIM_ORDER   (IN/OUT) = vecteur avec l'ordre d'exécution des eqs 

RETURN : 
	Nombre d'équations dans le bloc interdépendant
	
Méthode : 
	Pout tout eq i
		si déjà dans le bloc (KSIM_ORDERED[i] == 1) : passer à eq i+1
		sinon :
			KSIM_ORDER[KSIM_PRE + KSIM_POST + nb] = i
**************************************************************************************************/

KE_interdep(dbe, tmp)
KDB     *dbe;
int     **tmp;
{
    int     i, nb = 0;

    for(i = 0; i < KNB(dbe); i++) {
        if(KSIM_ORDERED[i]) continue;
        KSIM_ORDER[KSIM_PRE + KSIM_POST + nb] = i;
        nb ++;
    }
    return(nb);
}

/********************************************************************************
Trie les équations en effectuant passes passages successifs.
Méthode :
 - KSIM_PERM = vecteur des permutations des equations avec -1 au départ

**********************************************************************************/

KE_tri(dbe, tmp, passes)
KDB     *dbe;
int     **tmp;
int     passes;
{
    int     i, var, j;

    KE_tri_begin(dbe);

    for(j = 0 ; j < passes ; j++) {
        for(i = 0 ; i < KSIM_INTER ; i++) {
            var = KSIM_ORDER[KSIM_PRE + i];
            KE_tri_perm1(dbe, var, tmp[var]);
        }
    }

    KE_tri_end(dbe);
    return(0);
}

/********************************************************************************
Init permutations KSIM_PERM = vecteur des permutations des equations avec -1 au départ

	- place -1 pour chaque eq
	- place le numéro d'ordre d'exécution de eqi dans KSIM_PERM[eqi]
**********************************************************************************/

KE_tri_begin(dbe)
KDB     *dbe;
{
    int     i, nb;

    nb = KNB(dbe);
    KSIM_PERM = (int *) SW_nalloc(sizeof(int) * nb);
    for(i = 0 ; i < nb ; i++) KSIM_PERM[i] = -1;
    for(i = 0 ; i < KSIM_INTER ; i++)
        KSIM_PERM[KSIM_ORDER[KSIM_PRE + i]] = i;
    return(0);
}

/********************************************************************************
Pour l'eq i, pour chaque var explicative, on recherche l'éq calculée le plus tard
dans l'ordre du modèle corrigé par KSIM_PERM. 
Soit m cette position.
Si m < KSIM_PERM[i], ok
Sinon, on déplace tout d'un vers l'avant à partir de la position actuelle de eqi
jusqu'à m et on place eqi à la place de m.
**********************************************************************************/

KE_tri_perm1(dbe, i, vars)
KDB     *dbe;
int     i, *vars;
{
    int     j, jm, m = -1, posj;

	// calcul de l'eq jm dont le numéro d'ordre de calcul est le plus grand
    for(j = 1 ; j <= vars[0] ; j++) {
		if(vars[j] < 0) continue; 	// var endogène de l'eq
		posj = KSIM_PERM[vars[j]];  // position actuelle de l'eq j
		
		if(posj > m) {             
            m = posj;
            jm = j;
        }
    }

	// Si le numéro d'ordre max des eq dont dépend la courante est < numéro d'ordre de la courante : ok
    if(m < KSIM_PERM[i]) return;
	
    for(j = 0 ; j < KNB(dbe) ; j++)
        if(KSIM_PERM[j] > KSIM_PERM[i] && KSIM_PERM[j] <= m)
            KSIM_PERM[j]--;

    KSIM_PERM[i] = m;
    return(0);
}

// Reporte dans KSIM_ORDER les changements opérés par le tri et sauvés dans KSIM_PERM

KE_tri_end(dbe)
KDB     *dbe;
{
    int     i;

    for(i = 0 ; i < KNB(dbe) ; i++)
        if(KSIM_PERM[i] >= 0)
            KSIM_ORDER[KSIM_PRE + KSIM_PERM[i]] = i;

    SW_nfree(KSIM_PERM);
    return(0);
}

/******************************************************************************
posendo = pos dans dbv de l'endogène
posexo  = pos dans dbv de l'exogène
******************************************************************************/

KE_exo2endo(posendo, posexo)
int     posendo, posexo;
{
    int     i, endo, exo;
    int     depth = 0;

    endo = KE_poseq(posendo);
    if(endo < 0) {
        B_seterrn(116, KONAME(KSIM_DBV, posendo));
        return(-1);
    }

    if(KE_poseq(posexo) >= 0) {
        B_seterrn(119, KONAME(KSIM_DBV, posexo));
        return(-1);
    }

    /*
        if(KSIM_POSXK[endo] != K_find(KSIM_DBV, KONAME(KSIM_DBE, posendo))) {
    	B_seterrn(118, endo);
    	return(-1);
        }
    */
    /* check if exo in equation */
    memset(KSIM_PATH, 0, KSIM_MAXDEPTH);
    exo = KE_findpath(posendo, posexo, &depth);
    if(exo < 0) {
        B_seterrn(117, KONAME(KSIM_DBV, posendo) , KONAME(KSIM_DBV, posexo));
        return(-1);
    }
    return(0);
}

KE_findpath(posendo, posexo, depth)
int     posendo, posexo, *depth;
{
    int     lg, j, poseq, posseq, rc = -1;
    CLEC    *clec = NULL, *eclec;

    if(posexo < 0 || *depth > KSIM_MAXDEPTH) return(-1);

    /* endo and exo in same equation */
    poseq = KE_poseq(posendo);
    clec = KECLEC(KSIM_DBE, poseq);
    for(j = 0; j < clec->nb_names; j++) {
        if(L_ISCOEF(clec->lnames[j].name)) continue;
        if((clec->lnames[j]).pos == posexo) {
            KSIM_POSXK[poseq] = posexo;
            return(0);
        }
    }

    /* endo and exo in subequation */
    for(j = 0; j < clec->nb_names; j++) {
        eclec = KECLEC(KSIM_DBE, poseq);
        clec = SW_nalloc(eclec->tot_lg);
        memcpy(clec, eclec, eclec->tot_lg);
        if(L_ISCOEF(clec->lnames[j].name)) continue;
        posseq = KE_poseq((clec->lnames[j]).pos);

        /* if same endo, variable exo or endo already exchanged continue */
        if(poseq == posseq || posseq < 0) continue;

        /* if path already examined continue */
        if(KSIM_PATH[posseq] == 1) continue;
        else KSIM_PATH[posseq] = 1;


        (*depth) ++;
        rc = KE_findpath((clec->lnames[j]).pos, posexo, depth);
        if(rc < 0) {
            (*depth) --;
            continue;
        }

        /*      fprintf(stdout, "%s <- ", clec->lnames[j].name); */
        KSIM_POSXK[poseq] = (clec->lnames[j]).pos;

        (*depth) --;
        SW_nfree(clec);
        return(rc);
    }

    SW_nfree(clec);
    return(-1);
}

K_diverge(t, lst, eps)
int     t;
char    *lst; IODE_REAL    eps;
{
    char    buf[81];
    int     i, j;
    IODE_REAL    x;
    double  d, pd, fpd1, fpd2;

    if(B_DataExist(lst, K_LST) >= 0) B_DataDelete(lst, K_LST);
    for(i = KSIM_PRE, j = 0; j < KSIM_INTER; i++, j++)  {
        /* save XK first */
        KSIM_XK[j] = KSIM_VAL(KSIM_ORDER[i], t);

        /* execute lec */
        x = K_calc_clec(KSIM_ORDER[i], t, KSIM_POSXK[KSIM_ORDER[i]], 1);
        if(!L_ISAN(x)) return(-1);

        /* Check convergence */
        if(L_ISAN(KSIM_XK[j])) {
            /* ?????????????
            d = (KSIM_XK[j] - x) * KSIM_RELAX;
            */

            d = (KSIM_XK[j] - x);
            if(!L_IS0(KSIM_XK[j]))
                pd = min(fabs(1 - x / KSIM_XK[j]), fabs(d));
            else pd = fabs(d);

            pd *= KSIM_RELAX;
            if(pd > eps)  {
                sprintf(buf, "%s %s", lst, KSIM_NAME(KSIM_ORDER[i]));
                B_DataAppend(buf, K_LST);
            }
        }
    }

    return(0);
}


/*
	Sous fonction de K_lstorder qui crée une des 3 listes résultat du tri.
*/
K_lstorder_1(lstname, eq1, eqn)
char 	*lstname;
int 	eq1, eqn;
{
	U_ch 	**lst = 0, buf[256];
	int   	i, nlst = 0, nb = eqn - eq1 + 1,  maxl = 1000;
	
	// Détruit la liste cible et les sous-listes 
	sprintf(buf, "%s*", lstname);
	B_DataDelete(buf, K_LST);

	// Crée un tableau avec tous les noms
	for(i = 0; i < nb; i++)  
		SCR_add_ptr(&lst, &nlst, KSIM_NAME(KSIM_ORDER[i + eq1]));
	
	SCR_add_ptr(&lst, &nlst, 0);

	// Génère une liste avec sous-listes si nécessaire
	KL_lst(lstname, lst, maxl);
	SCR_free_tbl(lst);
	return(0);
}

/*
 Sauve dans les listes PRE, POST et INTER le résultat du réordonnancement
 pour les utiliser plus tard
*/

K_lstorder(pre, inter, post)
char    *pre, *inter, *post;
{
	K_lstorder_1(pre,   0, 					   KSIM_PRE - 1);
	K_lstorder_1(inter, KSIM_PRE, 			   KSIM_PRE + KSIM_INTER - 1);
	K_lstorder_1(post,  KSIM_PRE + KSIM_INTER, KSIM_PRE + KSIM_INTER + KSIM_POST - 1);
    return(0);
}

// Version revue le 5/3/2012 pour plantage Nemesis
/*
K_lstorder(pre, inter, post)
char    *pre, *inter, *post;
{
    int     i;
	char    buf[128]; // JMP 5/03/2012

    if(B_DataExist(pre, K_LST) >= 0) B_DataDelete(pre, K_LST);
    if(B_DataExist(inter, K_LST) >= 0) B_DataDelete(inter, K_LST);
    if(B_DataExist(post, K_LST) >= 0) B_DataDelete(post, K_LST);

	// A revoir (plante pour des raisons de manque de mémoire)
	// Faire 1 sous boucle avec max 1000 éléments dans pre_1, pre_2, puis pre = $rep_1 $pre2 ...
    for(i = 0; i < KSIM_PRE; i++)  {
		sprintf(buf, "%s %s", pre, KSIM_NAME(KSIM_ORDER[i]));
		B_DataAppend(buf, K_LST);
    }

    for(;i < KSIM_PRE + KSIM_INTER; i++)  {
	sprintf(buf, "%s %s", inter, KSIM_NAME(KSIM_ORDER[i]));
	B_DataAppend(buf, K_LST);
    }

    for(;i < KSIM_PRE + KSIM_INTER + KSIM_POST; i++)  {
	sprintf(buf, "%s %s", post, KSIM_NAME(KSIM_ORDER[i]));
	B_DataAppend(buf, K_LST);
    }

    return(0);
}
*/

/******************************************************************************************************
Gestion des simulations basées sur un ordre prédéfini ("large" models Nemesis-like)
 ******************************************************************************************************/

// Calcule uniquement les SCC et trie si demandé
KE_ModelCalcSCC(dbe, tris, pre, inter, post)
KDB     *dbe;
int		tris;
char 	*pre, *inter, *post;
{
	int i, 
		opasses = KSIM_PASSES,
		osort = KSIM_SORT;
	
	if(KNB(dbe) == 0) {
        B_seterrn(110);
        return(-1);
    }

    KSIM_DBE = dbe;
	KSIM_DBV = dbe; // Pour reconstruire les listes dans K_lstorder via KSIM_NAME
    KSIM_MAXDEPTH = KNB(dbe);
	KSIM_PASSES = tris;
	
	if(tris > 0) KSIM_SORT = SORT_BOTH;
	else           KSIM_SORT = SORT_CONNEX;

    // KSIM_POSXK[i] = num dans dbv de la var endogène de l'équation i
    KSIM_POSXK = (int *) SW_nalloc((int)(sizeof(int) * KNB(dbe)));

	// PSEUDO LINK EQUATIONS ie set num endo = num eq 
    kmsg("Pseudo-linking equations ....");
    for(i = 0 ; i < KNB(dbe); i++) {
		KSIM_POSXK[i] = i; 
        L_link_endos(dbe, KECLEC(dbe, i));
    }

    /* ORDERING EQUATIONS */
    KE_order(dbe, NULL);
	K_lstorder(pre, inter, post);

fin:
    SW_nfree(KSIM_POSXK);
    SW_nfree(KSIM_ORDER);
    KSIM_POSXK = KSIM_ORDER = NULL;
	KSIM_PASSES = opasses;
	KSIM_SORT = osort;
    return(0);
}

// Simulation (pas d'endo-exo)

KE_simul_init(dbe, dbv, dbs, smpl)
KDB     *dbe, *dbv, *dbs;
SAMPLE  *smpl;
{
    int     i, t, rc = 0;

    if(KNB(dbe) == 0) {
        B_seterrn(110);
        return(-1);
    }

    KSIM_DBV = dbv;
    KSIM_DBE = dbe;
    KSIM_MAXDEPTH = KNB(dbe);
    KSIM_DBS = dbs;

	// Check Sample dans les bornes du WS
    t = PER_diff_per(&(smpl->s_p1), &(KSMPL(dbv)->s_p1));
    if(t < 0 || PER_diff_per(&(KSMPL(dbv)->s_p2), &(smpl->s_p2)) < 0) {
        B_seterrn(111);
        return(-1);
    }

    // KSIM_POSXK[i] = num dans dbv de la var endogène de l'équation i
    KSIM_POSXK = (int *) SW_nalloc(sizeof(int) * KNB(dbe));

	// Initialise les nouvelles vars pour conserver les résultats de sim 
    // NE PAS FREEER KSIM_NORMS ni KSIM_NITERS CAR UTILISES POUR LE REPORTING A POSTERIORI !!
	SCR_free(KSIM_NORMS);
	SCR_free(KSIM_NITERS);
	KSIM_NORMS = (IODE_REAL *) SCR_malloc(sizeof(IODE_REAL) * KSMPL(dbv)->s_nb);
	KSIM_NITERS = (int *) SCR_malloc(sizeof(int) * KSMPL(dbv)->s_nb);

    /* LINK EQUATIONS + SAVE ENDO POSITIONS */
    kmsg("Linking equations ....");
    for(i = 0 ; i < KNB(dbe); i++) {
        KSIM_POSXK[i] = K_find(dbv, KONAME(dbe,i));
        if(KSIM_POSXK[i] < 0) {
            B_seterrn(112, KONAME(dbe, i));
            rc = -1;
            goto fin;
        }

        rc = L_link(dbv, dbs, KECLEC(dbe, i));
        if(rc) {
            B_seterrn(113, KONAME(dbe, i));
            rc = -1;
            goto fin;
        }
    }

	KSIM_XK  = (IODE_REAL *) SW_nalloc(sizeof(IODE_REAL) * KSIM_INTER);
    KSIM_XK1 = (IODE_REAL *) SW_nalloc(sizeof(IODE_REAL) * KSIM_INTER);
	return(0);
	
fin:
	SW_nfree(KSIM_POSXK);
	KSIM_POSXK = 0;
	return(rc);
}


KE_simul_SCC(dbe, dbv, dbs, smpl, pre, inter, post)
KDB     *dbe, *dbv, *dbs;
SAMPLE  *smpl;
char    **pre, **inter, **post;
{
    int     i, t, j, rc = -1;
	
	KSIM_PRE = SCR_tbl_size(pre);
	KSIM_INTER = SCR_tbl_size(inter);
	KSIM_POST = SCR_tbl_size(post);
	
	if(KE_simul_init(dbe, dbv, dbs, smpl)) return(-1);
    
	// Fixe l'ordre d'exécution dans KSIM_ORDER
	KSIM_ORDER = (int *)  SW_nalloc(sizeof(int) * (KSIM_PRE + KSIM_INTER + KSIM_POST));
	for(i = j = 0; i < KSIM_PRE; i++)   KSIM_ORDER[j++] = K_find(dbe, pre[i]);
	for(i = 0;     i < KSIM_INTER; i++) KSIM_ORDER[j++] = K_find(dbe, inter[i]);
	for(i = 0;     i < KSIM_POST; i++)  KSIM_ORDER[j++] = K_find(dbe, post[i]);

	// Simulation 
	t = PER_diff_per(&(smpl->s_p1), &(KSMPL(dbv)->s_p1));
	
    for(i = 0; i < smpl->s_nb; i++, t++) {
        if(rc = K_simul_1(t)) goto fin;
    }

fin:
    KE_simul_free();
    return(rc);
}
