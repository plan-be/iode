#include "iode.h"
 IODE_REAL    *E_UnitRoot(varname, drift, trend, order)
char    *varname;
int     drift, trend, order;
{
    int     i, pos, rc = -1;
    char    buf[1024], scl[11];
    SAMPLE  smpl;
    IODE_REAL    *res = NULL;


    KV_GetSmpl(&smpl, varname);
    memcpy(&(smpl.s_p1), PER_addper(&(smpl.s_p1), 1), sizeof(PERIOD));
    smpl.s_nb--;
    if(smpl.s_nb < (drift + trend + order + 1) * 2) {
	B_seterror("Sample to small for this test");
	return(res);
    }

    /* Dickey Fuller */
    sprintf(buf, "d(%s) := df_ * %s[-1]", varname, varname);
    B_DataCreate("df_", K_SCL);

    if(drift) {
	sprintf(buf + strlen(buf), "+ df_d");
	B_DataCreate("df_d", K_SCL);
    }

    if(trend) {
	sprintf(buf + strlen(buf), "+ df_t*t");
	B_DataCreate("df_t", K_SCL);
    }

    for(i = 1 ; i <= order ; i++) {
	sprintf(scl, "df%d", i);
	B_DataCreate(scl, K_SCL);
	}

    if(order) {
	for(i = 1;  i <= order ; i++) {
	    sprintf(buf + strlen(buf), " + df%d*d(%s[-%d])", i, varname, i);
	}
    }

    memcpy(&(smpl.s_p1), PER_addper(&(smpl.s_p1), order), sizeof(PERIOD));
    smpl.s_nb -= order;
    rc = E_UnitRoot_1(&smpl, buf);


    if(rc == 0)
	res = SCR_malloc((drift + trend + order + 1)* 3 * sizeof(IODE_REAL));
    else
	res = NULL;

    pos = 0;
    if(res) E_SclToReal("df_", res + pos);
    pos += 3;
    B_DataDelete("df_", K_SCL);

    if(drift) {
	if(res) E_SclToReal("df_d", res + pos);
	pos += 3;
	B_DataDelete("df_d", K_SCL);
    }

    if(trend) {
	if(res) E_SclToReal("df_t", res + pos);
	pos += 3;
	B_DataDelete("df_t", K_SCL);
    }

    for(i = 1 ; i <= order ; i++) {
	sprintf(buf, "df%d", i);
	if(res) E_SclToReal(buf, res + pos);
	pos += 3;
	B_DataDelete(buf, K_SCL);
    }

    return(res);
}


E_UnitRoot_1(smpl, buf)
SAMPLE  *smpl;
char    *buf;
{
    int     rc = -1, neqs = 0, pos;
    char    **eqs = NULL;
    SCL     *scl;

    SCR_add_ptr(&eqs, &neqs, "_DF");
    SCR_add_ptr(&eqs, &neqs, NULL);

    rc = B_DataUpdateEqs("_DF", buf, 0L, 0, 0L, 0L, 0L, 0L, 0);
    if(rc) return(rc);
    rc = B_EqsEstimateEqs(smpl, eqs);

    B_DataDelete("_DF", K_EQS);
    SCR_free_tbl(eqs);
    return(rc);
}

void E_SclToReal(name, res)
char    *name; IODE_REAL    *res;
{
    int pos;
    SCL *scl;

    pos = K_find(KS_WS, name);
    if(pos < 0) return;

    scl = KSVAL(KS_WS, pos);
    res[0] = scl->val;
    res[1] = scl->std;
    if(!L_IS0(scl->std)) res[2] = scl->val/scl->std;
}

void E_PrintDF(lec, res, drift, trend, order)
char    *lec; IODE_REAL    *res;
int     drift, trend, order;
{
    int     i, pos = 0;

    W_print_tit(2);
    W_printf("Augmented Dickey Fuller Test\n");
    W_print_enum(1);
    W_printf("ADF(%s, drift = %d, trend = %d, order = %d) = %lf\n",
	lec, drift, trend, order, res[2]);

    E_BeginTbl("Coefficients and tests", 4);
    W_printf("&1C &1CValue&1CStandard Error&1CT-Statistic\n");
    W_printf(".tl\n");

    W_printf("&1L%s&1D%lf&1D%lf&1D%lf\n",
		"ADF", res[pos], res[pos + 1], res[pos + 2]);
    pos += 3;

    if(drift) {
	W_printf("&1L%s&1D%lf&1D%lf&1D%lf\n",
		"Drift", res[pos], res[pos + 1], res[pos + 2]);
	pos += 3;
    }

    if(trend) {
	W_printf("&1L%s&1D%lf&1D%lf&1D%lf\n",
		"Trend", res[pos], res[pos + 1], res[pos + 2]);
	pos += 3;
    }

    for(i = 1 ; i <= order ; i++) {
	W_printf("&1LOrder %d&1D%lf&1D%lf&1D%lf\n",
		i, res[pos], res[pos + 1], res[pos + 2]);
	pos += 3;
    }
    W_printf(".te\n");
}

E_GetLecName(lec, name)
char    *lec, *name;
{
    int     j;
    CLEC    *clec = 0;

    name[0] = 0;
    clec = L_cc(lec);
    if(clec == 0) return(-1);
    for(j = 0 ; j < clec->nb_names ; j++) {
	strcpy(name, clec->lnames[j].name);
	if(strcmp(name, "_DF") && !L_ISCOEF(name)) break;
	}

    SW_nfree(clec);
    return(0);
}





