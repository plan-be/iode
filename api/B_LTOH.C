#include "iode.h"

#define LTOH_STOCK  0
#define LTOH_FLOW   1

#define LTOH_LIN    'L'
#define LTOH_CS     'C'
#define LTOH_STEP   'S'

B_WsLtoHStock(arg)
char    *arg;
{
    return(B_ltoh(LTOH_STOCK, arg));
}

B_WsLtoHFlow(arg)
char    *arg;
{
    return(B_ltoh(LTOH_FLOW, arg));
}

LTOH_smpl(f_smpl, ws_smpl, t_smpl, skip, shift)
SAMPLE  *f_smpl, *ws_smpl, **t_smpl;
int     *skip, *shift;
{
    int     f_nbper, ws_nbper;
    long    s, y;
    PERIOD  p1, p2;

    f_nbper = PER_nbper(&(f_smpl->s_p1));
    ws_nbper = PER_nbper(&(ws_smpl->s_p1));

    if(ws_nbper <= 0) {
	B_seterror("Set Ws periodicity first");
	return(-1);
    }

    if(ws_nbper <= f_nbper) {
	B_seterror("File has more observartions than the current ws");
	return(-1);
    }

    memcpy(&p1, &(f_smpl->s_p1), sizeof(PERIOD));
    memcpy(&p2, &(f_smpl->s_p2), sizeof(PERIOD));

    s = p1.p_s;
    y = p1.p_y;
    *shift = ws_nbper/f_nbper;

    p1.p_p = p2.p_p = (ws_smpl->s_p1).p_p;
    p1.p_s = 1;

    memcpy(&p1, PER_addper(&p1, (s - 1)/(*shift)), sizeof(PERIOD));
/*
    *skip = (p1.p_y - y ) * ws_nbper + (p1.p_s - 1) * (*shift) - (s - 1);
*/
    *skip = 0;
    p2.p_s = ws_nbper;


    *t_smpl = PER_pertosmpl(&p1, &p2);

    return(0);
}

B_ltoh(type, arg)
int     type;
char    *arg;
{
    int     lg, nb, rc = 0,
	    i, shift, skip;
    char    method[81], file[K_MAX_FILE + 1], **data = NULL;
    IODE_REAL    *t_vec = NULL, *f_vec = NULL;
    KDB     *from = NULL, *to = NULL;
    SAMPLE  *t_smpl = NULL;


    lg = B_get_arg0(method, arg, 80);
    U_sqz_text(method);

    lg += B_get_arg0(file, arg + lg, 80);

    data = B_ainit_chk(arg + lg, NULL, 0);
    nb = SCR_tbl_size(data);
    if(nb == 0) goto done;

    from = K_load(K_VAR, file, nb, data);
    if(from == NULL) {
	rc = -1;
	goto done;
    }

    if(LTOH_smpl(KSMPL(from), KSMPL(KV_WS), &t_smpl, &skip, &shift) < 0) {
	rc = -1;
	goto done;
    }

    to = K_create(K_VAR, K_UPPER);
    memcpy((SAMPLE *) KDATA(to), t_smpl, sizeof(SAMPLE));
    t_vec = (IODE_REAL *) SW_nalloc(t_smpl->s_nb * sizeof(IODE_REAL));
    f_vec = (IODE_REAL *) SW_nalloc(KSMPL(from)->s_nb * sizeof(IODE_REAL));

    for(i = 0; i < KNB(from); i++) {
	memcpy(f_vec, KVVAL(from, i, 0), KSMPL(from)->s_nb * sizeof(IODE_REAL));
	switch(method[0]) {
	    case LTOH_CS :
		LTOH_cs(type,
		    f_vec, (int) KSMPL(from)->s_nb,
		    t_vec, (int) t_smpl->s_nb,
		    shift);
		break;

	    case LTOH_STEP:
		LTOH_step(type,
		    f_vec, (int) KSMPL(from)->s_nb,
		    t_vec, (int) t_smpl->s_nb,
		    shift);
		break;

	    default       :
	    case LTOH_LIN :
		LTOH_lin(type,
		    f_vec, (int) KSMPL(from)->s_nb,
		    t_vec, (int) t_smpl->s_nb,
		    shift);
		break;
	}
	nb = t_smpl->s_nb;
	K_add(to, KONAME(from, i), t_vec, &(nb));
    }
    KV_merge(KV_WS, to, 1);

done:
    K_free(to);
    K_free(from);
    SCR_free_tbl(data);

    SW_nfree(t_smpl);
    SW_nfree(t_vec);
    SW_nfree(f_vec);

    if(rc < 0) return(-1);
    else return(0);
}


/* Linear Interpol */ IODE_REAL    LTOH_ylin(y, x) IODE_REAL    *y, x;
{

    IODE_REAL    res, a, b;
    int     hi, lo;

    lo = floor(x);
    hi = lo + 1.0;

    a = hi - x;
    b = x - lo;

    if(L_ISAN(y[lo]) && L_ISAN(y[hi]))
	res = a*y[lo] + b*y[hi];
    else res = L_NAN;
    return(res);
}

LTOH_lin(type, f_vec, f_nb, t_vec, t_nb, shift) IODE_REAL    *f_vec, *t_vec;
int     type, f_nb, t_nb, shift;
{
    int     dim, j, f, t, beg;
    IODE_REAL    x;

    for(t = 0; t < t_nb; t++) t_vec[t] = L_NAN;
    beg = 0;
    while(1) {
	while(beg < f_nb && !L_ISAN(f_vec[beg])) beg++;
	if(beg >= f_nb) break;
	for(dim = beg; dim < f_nb && L_ISAN(f_vec[dim]); dim++);
	dim -= 1;
	if(dim > beg) {

	    if(type == LTOH_FLOW) {
		for(f = beg, t = beg*shift; f < dim; f++, t+= shift) {
		    for(j = 0; j < shift; j++) {
			t_vec[t + j] = f_vec[f]/shift;
		    }
		}
	    }
	    else {
		for(f = beg, t = (beg+1)*shift; f < dim; f++, t+= shift) {
		    for(j = 0; j < shift; j++) {
			x = (IODE_REAL) (j + 1) / (IODE_REAL) shift;
			x += f;
			t_vec[t + j] = LTOH_ylin(f_vec, x);
		    }
		}
	    }
	}
	beg = dim + 1;
    }

    return(0);
}

LTOH_step(type, f_vec, f_nb, t_vec, t_nb, shift) IODE_REAL    *f_vec, *t_vec;
int     type, f_nb, t_nb, shift;
{
    int     dim, j, f, t, beg;
    IODE_REAL    x;

    for(t = 0; t < t_nb; t++) t_vec[t] = L_NAN;
    beg = 0;
    while(1) {
	while(beg < f_nb && !L_ISAN(f_vec[beg])) beg++;
	if(beg >= f_nb) break;
	for(dim = beg; dim < f_nb && L_ISAN(f_vec[dim]); dim++);

	if(dim > beg) {

	    if(type == LTOH_FLOW) {
		for(f = beg, t = beg*shift; f < dim; f++, t+= shift) {
		    for(j = 0; j < shift; j++) {
			t_vec[t + j] = f_vec[f]/shift;
		    }
		}
	    }
	    else {
		for(f = beg, t = beg*shift; f < dim; f++, t+= shift) {
		    for(j = 0; j < shift; j++) {
			t_vec[t + j] = f_vec[f];
		    }
		}
	    }
	}
	beg = dim + 1;
    }

    return(0);
}

/* Cubic Spline */
int LTOH_y2cs(y, n, y2) IODE_REAL    *y, *y2;
int     n;
{
    int     i, dim;
    IODE_REAL    *u, p, s;

    /* "natural" cubic spline */
    u = (IODE_REAL *) SW_nalloc(n * sizeof(IODE_REAL));
    y2[0] = u[0] = 0.0;

    for(i = 1; i < n - 1 && L_ISAN(y[i]); i++) {
	s = 0.5; /* (x[i] - x[i-1])/(x[i+1] - x[i-1]) */
	p = s * y2[i-1] + 2.0;
	y2[i] = (s - 1)/p;
	u[i] = y[i+1] - 2 * y[i] + y[i-1];
	u[i] = (3.0 * u[i] - s * u[i-1])/p;
    }

    dim = i - 1;

    y2[dim] = 0.0;

    for(i = dim - 1; i >= 0; i--) y2[i] = y2[i] * y2[i + 1] + u[i];
    SW_nfree(u);

    return(dim);
}

 IODE_REAL    LTOH_ycs(y, y2, x) IODE_REAL    *y, *y2, x;
{

    IODE_REAL    res, a, b;
    int     hi, lo;

    if(x < 0) return(L_NAN);

    lo = floor(x);
    hi = lo + 1.0;

    a = hi - x;
    b = x - lo;

    if(L_ISAN(y[lo]) && L_ISAN(y[hi]))
	res = a*y[lo] + b*y[hi] +
	    ((a*a*a - a)*y2[lo] + (b*b*b - b)*y2[hi])/6.0;
    else res = L_NAN;
    return(res);
}

LTOH_cs(type, f_vec, f_nb, t_vec, t_nb, shift) IODE_REAL    *f_vec, *t_vec;
int     type, f_nb, t_nb, shift;
{
    int     dim, j, f, t, beg;
    IODE_REAL    *y2 = SW_nalloc(sizeof(IODE_REAL) * f_nb),
	    x, sum, d;

    for(t = 0; t < t_nb; t++) t_vec[t] = L_NAN;
    beg = f = 0;
    while(1) {
	while(beg < f_nb && !L_ISAN(f_vec[beg])) beg++;
	if(beg >= f_nb) break;

	    if(type == LTOH_FLOW) {
		for(f = beg; f < f_nb && L_ISAN(f_vec[f]); f++) f_vec[f] = f_vec[f]/shift;
		dim = LTOH_y2cs(f_vec + beg, f_nb - beg, y2);

		for(f = beg, t = beg*shift; f < dim + beg; f++, t+= shift) {
		    sum = 0.0;
		    for(j = 0; j < shift; j++) {
			x = (IODE_REAL) (j - 1) / (IODE_REAL) shift;
			x += f;
			t_vec[t + j] = LTOH_ycs(f_vec, y2, x);
			if(L_ISAN(t_vec[t + j]) && L_ISAN(sum)) sum += t_vec[t + j];
			else sum = L_NAN;
		    }

		    if(L_ISAN(sum)) {
			d = (f_vec[f] * shift - sum)/shift;
			for(j = 0; j < shift; j++) t_vec[t + j] += d;
		    }
		}
	    }
	    else {
		dim = LTOH_y2cs(f_vec + beg, f_nb - beg, y2);
		for(f = beg, t = (beg+1)*shift; f < dim + beg; f++, t+= shift) {
		    for(j = 0; j < shift; j++) {
			x = (IODE_REAL) (j + 1) / (IODE_REAL) shift;
			x += f;
			t_vec[t + j] = LTOH_ycs(f_vec, y2, x);
		    }
		}
	    }
	beg += dim + 1;
    }
    return(0);
}























