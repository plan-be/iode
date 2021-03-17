#include "iode.h"

char     **EST_ARGS;

 IODE_REAL    KEST_EPS = 0.00001;
char    KEST_METHOD = 0;
int     KEST_MAXIT = 100;
extern  int E_T, E_NCE;


KE_est_s(dbe, dbv, dbs, smpl, nb, names)
KDB     *dbe, *dbv, *dbs;
SAMPLE  *smpl;
int     nb;
char    **names;
{
static  char met[6] = {"LZIGM"};
    int     i, j, k, pos,
	    method, error = 0,
	    nbl = 0, nbe = 0, nblk;
    SAMPLE  eq_smpl;
    U_ch    **endos = 0;
    U_ch    **blk = 0;
    U_ch    **lecs = 0;
    U_ch    **instrs = 0;
    float   tests[EQS_NBTESTS]; /* FLOAT 12-04-98 */
    EQ      *eq;

    for(i = 0; i < nb; i++) {
	pos = K_find(dbe, names[i]);
	if (pos < 0) {
	    B_seterrn(120, names[i]);
	    goto err;
	    }

	if(KEST_METHOD == 0) method = KEMETH(dbe, pos);
	else method = 0;

	if(smpl == 0) {
	    memcpy(&eq_smpl, &(KESMPL(dbe, pos)), sizeof(SAMPLE));
	    smpl = &eq_smpl;
	    if(eq_smpl.s_nb < 1) {
		B_seterrn(121, names[i]);
		goto err;
		}
	    }

	if(KEINSTR(dbe, pos) == NULL) instrs = NULL;
	else instrs =  SCR_vtoms(KEINSTR(dbe, pos), ",;");

	blk =  SCR_vtoms(KEBLK(dbe, pos), " ,;");
	nblk = SCR_tbl_size(blk);

	if(nblk == 0)  {
	    SCR_add_ptr(&lecs, &nbl, KELEC(dbe, pos));
	    SCR_add_ptr(&endos, &nbe, names[i]);
	    }
	else {
	    /* add elements of block */
	    for(j = 0; j < nblk; j++) {
		SCR_sqz(blk[j]);
		pos = K_find(KE_WS, blk[j]);
		if(pos < 0) {
		    B_seterrn(120, blk[j]);
		    goto err;
		    }
		SCR_add_ptr(&lecs, &nbl, KELEC(KE_WS, pos));
		SCR_add_ptr(&endos, &nbe, KONAME(KE_WS, pos));
		}
	    }

	SCR_add_ptr(&lecs, &nbl, 0L);
	SCR_add_ptr(&endos, &nbe, 0L);

	error = E_est(endos, lecs,
		    dbv, dbs, smpl,
		    method, instrs, KEST_MAXIT, KEST_EPS);

	if(error == 0) {
	    E_print_results(1, 1, 1, 1, 1);  /* JMP 23-03-98 */

	    for(j = 0; j < nbe - 1; j++) {
		tests[0] = MATE(E_MCORRU,      0, j);
		tests[1] = MATE(E_STDEV,       0, j);
		tests[2] = MATE(E_MEAN_Y,      0, j);
		tests[3] = MATE(E_SSRES,       0, j);
		tests[4] = MATE(E_STDERR,      0, j);
		tests[5] = MATE(E_STD_PCT,     0, j);
		tests[6] = MATE(E_FSTAT,       0, j);
		tests[7] = MATE(E_RSQUARE,     0, j);
		tests[8] = MATE(E_RSQUARE_ADJ, 0, j);
		tests[9] = MATE(E_DW,          0, j);
		tests[10]= MATE(E_LOGLIK,      0, j);

		B_DataUpdateEqs(endos[j], lecs[j], NULL, method, smpl,
		    NULL, NULL, tests, 1);
		pos = K_find(KE_WS, endos[j]);   /* JMP 24-06-98 */
		eq = KEVAL(KE_WS, pos);          /* JMP 24-06-98 */
		E_tests2scl(eq, j, E_T, E_NCE);  /* JMP 27-09-96 */
		E_free(eq); eq = NULL;           // GB 14/11/2012
		E_savevar("_YCALC", j, E_RHS);   /* JMP 27-09-96 */
		E_savevar("_YOBS", j, E_LHS);    /* JMP 27-09-96 */
		E_savevar("_YRES", j, E_U);      /* JMP 27-09-96 */
		}
	    }

	/* endos, lecs and instruments freed in E_est */
	SCR_free_tbl(blk);

	lecs = endos = instrs = blk = NULL;
	nbl = nbe = 0;

	E_free_work();

	if(error != 0) return(-1);
	}

    return(0);

err :
/*    SCR_free_tbl(lecs);
    SCR_free_tbl(instrs);
    SCR_free_tbl(endos);
*/
    SCR_free_tbl(blk);
    return(-1);
}

/* GB 27/9/96 */
void E_tests2scl(eq, j, n, k)
EQ      *eq;
int     j, n, k;
{
    if(eq == NULL) return;

    E_savescl((double)n, j, "n");
    E_savescl((double)k, j, "k");
    E_savescl(eq->tests[1], j, "stdev");
    E_savescl(eq->tests[2], j, "meany");
    E_savescl(eq->tests[3], j, "ssres");
    E_savescl(eq->tests[4], j, "stderr");
    E_savescl(eq->tests[5], j, "sderrp");
    E_savescl(eq->tests[6], j, "fstat");
    E_savescl(eq->tests[7], j, "r2");
    E_savescl(eq->tests[8], j, "r2adj");
    E_savescl(eq->tests[9], j, "dw");
    E_savescl(eq->tests[10], j, "loglik");
}

void E_savescl(val, eqnb, txt)         /* JMP 27-09-96 */
double  val;
int     eqnb;
char    *txt;
{
    char    buf[20];
    static SCL  scl = {0.9, 1.0, L_NAN};

    scl.val = val;
    sprintf(buf, "e%d_%s", eqnb, txt);
    K_add(KS_WS, buf, &scl);
}

void E_savevar(name, eqnb, mat)        /* JMP 27-09-96 */
char    *name;
int     eqnb;
MAT     *mat;
{
    int     k;
    char    buf[60], c_date[30];
    PERIOD  *per;

    sprintf(buf, "%s%d 1/0", name, eqnb);     /* JMP 06-02-97 */
    B_DataCalcVar(buf);
    for(k = 0; k < E_T; k++) {
	per = PER_addper(&(E_SMPL->s_p1), k);
	PER_pertoa(per, c_date),
	sprintf(buf, "%s%d %s %lf", name, eqnb, c_date,
	    (double) MATE(mat, eqnb, k));
	B_DataUpdate(buf, K_VAR);
    }
}





