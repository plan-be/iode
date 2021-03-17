#include "iode.h"

#define E_printf W_printf
void E_print_enum()
{
    W_print_enum(1);
}

void E_BeginTbl(title, nc)
char    *title;
int     nc;
{

/*
    W_print_tit(2);
    E_printf("%s\n", title);
*/
    E_printf(".tb %d\n", nc);
    E_printf(".sep &\n");
    E_printf(".ttitle %s\n", title);
}

void E_print_parms()
{
    char    from[20], to[20];

    E_BeginTbl("Parameters", 2);

    PER_pertoa(&(E_SMPL->s_p1), from);
    PER_pertoa(&(E_SMPL->s_p2), to);

    E_printf("&1NEstimation period        &1L%s:%s\n", from, to);
    E_printf("&1NEstimation method        &1L%c\n", "LZIG"[E_MET]);
    E_printf("&1LNumber of observations   &1L%d\n", E_T);
    E_printf("&1LNumber of equations      &1L%d\n", E_NEQ);
    E_printf("&1LNumber of coefficients   &1L%d\n", E_NCE);
    E_printf("&1LNumber of instruments    &1L%d\n", E_NINSTR);
    E_printf("&1LMax number of iterations &1L%d\n", E_MAXIT);
    E_printf("&1LConvergence limit        &1L%lf\n", (double)E_EPS);

    E_printf(".te\n");
}

void E_print_eqs()
{
    int     i;

    W_print_tit(2);
    E_printf("Equations\n");
    for(i = 0 ; i < E_NEQ ; i++) {
	E_print_enum();
	E_printf("%s\n", E_LECS[i]);
	}
}

void E_print_instrs()
{
    int     i;

    if(E_NINSTR == 0 || E_MET == 0 || E_MET == 1) return;
    W_print_tit(2);
    E_printf("Instruments\n");
    for(i = 0 ; i < E_NINSTR ; i++) {
	E_print_enum();
	E_printf("%s\n", E_INSTRS[i]);
	}
}

void E_print_coefs()
{
    int     i;
    SCL     *scl;

    E_BeginTbl("Coefficients and tests", 5);
    E_printf("&1CName&1CValue&1CStandard Error&1CT-Statistic&1CRelax\n");
    E_printf(".tl\n");
    for(i = 0 ; i < E_NC ; i++) {
	scl = KSVAL(E_DBS, E_C_NBS[i]);
     //   if(scl->relax == 0) continue; /* JMP 12-03-98 */
	E_printf("&1L%s&1D%lf&1D%lf&1D%lf&1D%lf\n",
	    KONAME(E_DBS, E_C_NBS[i]),
	    (double) scl->val,
	    (double) scl->std,
	    (double) E_div_0(scl->val, scl->std),
	    (double) scl->relax);
    }
    E_printf(".te\n");
}

void E_print_mcorr()
{
    int     i, j, ic, jc;

    E_BeginTbl("Correlation matrix of coefficients", E_NCE + 1);
    E_printf(".twidth 2 2\n");

    E_printf("&1C ");
    for(i = 0 ; i < E_NC ; i++)
	if(MATE(E_SMO, i, 0)) E_printf("&1C%s", KONAME(E_DBS, E_C_NBS[i]));

    E_printf("\n.tl\n");
    for(i = 0, ic = 0 ; i < E_NC ; i++) {
	if(MATE(E_SMO, i, 0) == 0) continue;
	E_printf("&1L%s", KONAME(E_DBS, E_C_NBS[i]));
	for(j = 0, jc = 0 ; j < E_NC ; j++) {
	    if(MATE(E_SMO, j, 0) == 0) continue;
	    E_printf("&1D%lf", (double) MATE(E_MCORR, ic, jc));
	    jc++;
	    }
	E_printf("\n");
	ic++;
    }
    E_printf(".te\n");
}

void E_print_mcorru()
{
    int     i, j;

    E_BeginTbl("Correlation of residuals", E_NEQ + 1);

    E_printf("&1C ");
    for(i = 0 ; i < E_NEQ ; i++) E_printf("&1C%s", E_ENDOS[i]);
    E_printf("\n.tl\n");
    for(i = 0 ; i < E_NEQ ; i++) {
	E_printf("&1L%s", E_ENDOS[i]);
	for(j = 0 ; j < E_NEQ ; j++) E_printf("&1D%lf", (double) MATE(E_MCORRU, i, j));
	E_printf("\n");
    }
    E_printf(".te\n");
/* JMP 13-07-96 */
}

void E_print_eqres_1(i)
int     i;
{
    E_BeginTbl("", 2);

    E_printf("&1C &1CValue\n");
    E_printf(".tl\n");
    E_printf("&1LNumber of coefficients      &1D%d\n", (int)MATE(E_NBCE, 0, i));
    E_printf("&1LNumber of observations      &1D%d\n", E_T);
    E_printf("&1LStandard deviation on YOBS  &1D%lf\n", (double) MATE(E_STDEV, 0, i));
    E_printf("&1LMean of YOBS                &1D%lf\n", (double) MATE(E_MEAN_Y, 0, i));
    E_printf(".tl\n");

    E_printf("&1CTests&1CValue\n");
    E_printf(".tl\n");
    E_printf("&1LSum of square of residuals  &1D%lf\n", (double) MATE(E_SSRES, 0, i));
    E_printf("&1LStandard error              &1D%lf\n", (double) MATE(E_STDERR, 0, i));
    E_printf("&1LStandard error in %%        &1D%lf\n", (double) MATE(E_STD_PCT, 0, i));
    E_printf("&1LF-Stat                      &1D%lf\n", (double) MATE(E_FSTAT, 0, i));
    E_printf("&1LR2                          &1D%lf\n", (double) MATE(E_RSQUARE, 0, i));
    E_printf("&1LR2 adjusted                 &1D%lf\n", (double) MATE(E_RSQUARE_ADJ, 0, i));
    E_printf("&1LDurbin-Watson test          &1D%lf\n", (double) MATE(E_DW, 0, i));
    E_printf("&1LLog likelihood              &1D%lf\n", (double) MATE(E_LOGLIK, 0, i));
    E_printf(".tl\n");
    E_printf(".te\n");
/* JMP 13-07-96 */
}

void E_print_eqres_2(eq_nb)
int     eq_nb;
{
    int     i;
    char    c_date[21];
    PERIOD  *per;
    double  respct;

    E_BeginTbl("Actual and fitted values", 5);
    E_printf("&1CPeriod&1CObservations&1CFitted Values&1CResiduals&1CResiduals (%%)\n");
    E_printf(".tl\n");
    for(i = 0 ; i < E_T ; i++) {
	per = PER_addper(&(E_SMPL->s_p1), i);
	PER_pertoa(per, c_date);
	respct = 100 * E_div_0(MATE(E_U, eq_nb, i), MATE(E_LHS, eq_nb, i));
	E_printf("&1L%s&1C%lf&1C%lf&1C%lf&1C%lf\n",
	    c_date,
	    (double) MATE(E_LHS, eq_nb, i),
	    (double) MATE(E_RHS, eq_nb, i),
	    (double) MATE(E_U, eq_nb, i),
	    respct);
	}

    E_printf(".te\n");
/* JMP 13-07-96 */
}
/*
void E_print_eqres_3(eq_nb)
int     eq_nb;
{
    double  miny, maxy, minr, maxr, diffr, diffy,
	    meanr = 0.0, std_error, lhsi, rhsi, ui;
    int     i, j, lg = 38, lgr = 18;
    char    line[121], buf[20], c_date[21];
    PERIOD  *per;

    memset(line, 0, sizeof(line));

    miny = maxy = MATE(E_LHS, eq_nb, 0);
    minr = maxr = MATE(E_U, eq_nb, 0);

    for(i = 0 ; i < E_T ; i++) {
	lhsi = MATE(E_LHS, eq_nb, i);
	rhsi = MATE(E_RHS, eq_nb, i);
	ui   = MATE(E_U, eq_nb, i);
	miny = (miny > lhsi) ? lhs[i] : miny;
	miny = (miny > rhsi) ? rhs[i] : miny;
	maxy = (maxy < lhsi) ? lhs[i] : maxy;
	maxy = (maxy < rhsi) ? rhs[i] : maxy;
	minr = (minr > ui)   ? ui : minr;
	maxr = (maxr < ui)   ? ui : maxr;
	meanr += ui;
	}

    meanr /= E_T;
    for(i = 0 ; i < E_T ; i++) {
	ui   = MATE(E_U, eq_nb, i);
	std_error += (ui - meanr) * (ui - meanr);
	}
    std_error = sqrt(std_error / (double)(E_T - 1));
    minr = (minr > (-std_error)) ? (-std_error) : minr;
    maxr = (maxr < std_error) ? std_error : maxr;

    diffy = (double)(lg - 1) / (maxy - miny);
    diffr = (double)(lgr - 1) / (maxr - minr);

    W_print_tit(2);
    E_printf("PLOT OF ACTUAL (o) AND FITTED (x) VALUESn\n");

    for(i = 0 ; i < E_T ; i++) {
	for(j = 0 ; j < 12 + lg + lgr ; j++) line[j] = ' ';
	line[10 + lg + (int)(1.5 + diffr * (meanr - minr))] = '|';
	line[10 + lg + (int)(1.5 + diffr * (- std_error - minr))] = ':';
	line[10 + lg + (int)(1.5 + diffr * (std_error - minr))] = ':';

	lhsi = MATE(E_LHS, eq_nb, i);
	rhsi = MATE(E_RHS, eq_nb, i);
	ui   = MATE(E_U, eq_nb, i);
	line[(int)(0.5 + diffy * (lhsi - miny))] = 'o';
	line[(int)(0.5 + diffy * (rhsi - miny))] = 'x';
	line[10 + lg + (int)(1.5 + diffr * (ui - minr))] = 'o';
	sprintf(buf, "%10.3le", ui);
	for(j = 0 ; j < 10 ; j++) line[lg + j] = buf[j];
	per = PER_addper(&(E_SMPL->s_p1), i);
	PER_pertoa(per, c_date);
	E_printf("%8s:%70s\n", c_date, line);
	}
}
*/
void E_print_eqres(obs)
int     obs;
{
    int     i;

    W_print_tit(2);
    E_printf("Results by equation\n");
    for(i = 0 ; i < E_NEQ ; i++) {
	W_print_tit(3);
	E_printf("Equation : %s\n", E_ENDOS[i]);
	W_print_par(3);
	E_printf("%s\n", E_LECS[i]);
	E_print_eqres_1(i);
	if(obs) E_print_eqres_2(i);
/*        E_print_eqres_3(i); */
	}
}

E_print_results(corr, corru, obs, grobs, grres) /* JMP 23-03-98 */
int     corr, corru, obs, grobs, grres;
{
    int     i, j;

    W_print_tit(1);
    E_printf("Estimation\n");

    E_print_eqs();
    E_print_parms();
    E_print_instrs();

    if(E_CONV == 0) {
	E_printf(".par1 parb\nTHE PROCESS DOES NOT CONVERGE\n");
	return(0);
	}

    E_print_coefs();
    if(corr) E_print_mcorr();           /* JMP38 27-09-92 */
    if(corru) E_print_mcorru();         /* JMP38 27-09-92 */
    E_print_eqres(obs);
    if(grobs) o_estgr(E_ENDOS, E_SMPL, E_LHS, E_RHS, 0, 0);  /* JMP 23-03-98 */
    if(grres) o_estgr(E_ENDOS, E_SMPL, E_LHS, E_RHS, 0, 1);  /* JMP 23-03-98 */

    W_flush();
    return(0);
}








