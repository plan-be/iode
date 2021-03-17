#include "iode.h"

/* Hodrick & Prescott Trend filter */

B_WsTrendAll(arg, std) 
char    *arg;
int     std; // JMP 12/4/2019
{
    int     lg, nb, rc = 0,
                    i, j, f, t, shift = 0, /* lambda,*/ beg, dim = 0;
    char    file[K_MAX_FILE + 1], **data = NULL;
    IODE_REAL    *t_vec = NULL, *f_vec = NULL, lambda; // JMP 22/1/2019
    KDB     *from = NULL, *to = NULL;
    SAMPLE  *t_smpl = NULL;


    lg = B_get_arg0(file, arg, 80);

    data = B_ainit_chk(arg + lg, NULL, 0);
    // lambda = atoi(data[0]); // JMP  22/1/2019
    lambda = atof(data[0]); // JMP  22/1/2019
    nb = SCR_tbl_size(data + 1);
    if(nb == 0) goto done;

    from = K_load(K_VAR, file, nb, data + 1);
    if(from == NULL) {
        rc = -1;
        goto done;
    }

    if(HP_smpl(KSMPL(from), KSMPL(KV_WS), &t_smpl, &shift) < 0) goto done;

    to = K_create(K_VAR, K_UPPER);
    nb = t_smpl->s_nb;
    memcpy((SAMPLE *) KDATA(to), t_smpl, sizeof(SAMPLE));
    t_vec = (IODE_REAL *) SW_nalloc(nb * sizeof(IODE_REAL));
    f_vec = (IODE_REAL *) SW_nalloc(nb * sizeof(IODE_REAL));

    for(i = 0; i < KNB(from); i++) {
        memcpy(f_vec,
               KVVAL(from, i, 0) + shift, nb * sizeof(IODE_REAL));

        HP_test(f_vec, t_vec, nb, &beg, &dim);
        HP_calc(f_vec + beg, t_vec + beg, dim, lambda, std); //  JMP 12/4/2019 
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


int HP_smpl(f_smpl, ws_smpl, t_smpl, shift)
SAMPLE  *f_smpl, *ws_smpl, **t_smpl;
int     *shift;
{
    int     nbper;

    if((nbper = PER_nbper(&(ws_smpl->s_p1))) <= 0 ) {
        B_seterror("Set Ws periodicity first");
        goto err;
    }

    *t_smpl = (SAMPLE *) SW_nalloc(sizeof(SAMPLE));
    if(PER_common_smpl(f_smpl, ws_smpl, *t_smpl) < 0) goto err;
    *shift = PER_diff_per(&((*t_smpl)->s_p1), &(f_smpl->s_p1));
    return(nbper);

err:
    SW_nfree(*t_smpl);
    t_smpl = NULL;
    return(-1);
}

B_WsTrend(arg)
char    *arg;
{
    return(B_WsTrendAll(arg, 0));
}    

B_WsTrendStd(arg)
char    *arg;
{
    return(B_WsTrendAll(arg, 1));
}    

int HP_calc(f_vec, t_vec, nb, lambda, std)
IODE_REAL   *f_vec, *t_vec;
int         nb, std; //, lambda; // JMP 22/1/2019
IODE_REAL   lambda; // JMP 22/1/2019
{

    static  IODE_REAL y1[3] = {1.0, -2.0, 1.0},
                      y2[4] = {-2.0, 5.0, -4.0, 1.0},
                      yn[5] = {1.0, -4.0, 6.0, -4.0, 1.0};
    int     i, j;
    MAT     *yt = NULL, *gt = NULL, *a = NULL;

	// nb must be ge 4 (0 <= nb - 1 - j avec j = 3) // JMP 5/7/2017
	if (nb < 4) {
		for (i = 0; i < nb; i++) t_vec[i] = L_NAN;
		return(-1);
	}
	// JMP 5/7/2017

    yt = M_alloc(nb, 1);
    gt = M_alloc(nb, 1);
    a  = M_alloc(nb, nb);

    if(yt == NULL || gt == NULL  || a == NULL) goto err;

    // Prépare a matrix (weights)
    for(j = 0; j < 3; j ++) {
        MATE(a, 0, j) = lambda * y1[j];
        MATE(a, nb - 1, nb - 1 - j) = lambda * y1[j];
    }

    for(j = 0; j < 4; j ++) {
        MATE(a, 1, j) = lambda * y2[j];
        MATE(a, nb - 2, nb - 1 - j) = lambda * y2[j];
    }

    for(i = 2; i < nb - 2; i++) {
        for(j = 0; j < 5; j++) MATE(a, i, j + i - 2) = lambda * yn[j];
    }
    for(i = 0; i < nb; i++) MATE(a, i, i) = MATE(a, i, i) + 1;

    // Compute the vector yt to smooth
    for(i = 0; i < nb; i++) 
            MATE(yt, i, 0) = f_vec[i];

    // Compute log(yt) if std == 0
    if(std == 0) {
        for(i = 0; i < nb; i++) {
            if(f_vec[i] <= 0) goto err;    // JMP 26-07-11 
            MATE(yt, i, 0) = log(MATE(yt, i, 0));
        }
    }
    
    // Solve the system yt = a . gt and save result in t_vec
    M_solve(gt, a, yt);
    for(i = 0; i < nb; i++) 
        t_vec[i] = MATE(gt, i, 0);
   
    // exp(t_vec) if std == 0
    if(std == 0) {
        for(i = 0; i < nb; i++) 
            t_vec[i] = exp(t_vec[i]);
    }    

    // Free mem
    M_free(yt);
    M_free(gt);
    M_free(a);
    return(0);

err:
    M_free(yt);
    M_free(gt);
    M_free(a);
    for(i = 0; i < nb; i++) t_vec[i] = L_NAN; /* JMP 26-07-11 */
    return(-1);
}


/*
Prépare HP_calc. Calcule :
  - *beg = la première position non NAN dans f_vec et
  - *dim = le nombre de positions non NAN à partir de *beg
Place des NAN dans t_vec.
*/

void HP_test(f_vec, t_vec, nb, beg, dim)
IODE_REAL    *f_vec, *t_vec;
int     nb, *beg, *dim;
{
    int     i;

    for(*beg = 0; *beg < nb && !L_ISAN(f_vec[*beg]); (*beg)++)
        t_vec[*beg] = L_NAN;
    for(*dim = *beg; *dim < nb && L_ISAN(f_vec[*dim]); (*dim)++);

    for(i = *dim; i < nb; i++) t_vec[i] = L_NAN;
    *dim -= *beg; /* JMP 26-07-11 */
}













