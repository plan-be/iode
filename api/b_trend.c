#include "iode.h"

/* Hodrick & Prescott Trend filter */

static int HP_smpl(f_smpl, ws_smpl, t_smpl, shift)
SAMPLE  *f_smpl, *ws_smpl, **t_smpl;
int     *shift;
{
    int     nbper;

    if((nbper = PER_nbper(&(ws_smpl->s_p1))) <= 0) {
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


static int B_WsTrendAll(char* arg, int std)
{
    int         lg, nb, rc = 0, i, shift = 0, /* lambda,*/ beg, dim = 0;
    char        file[K_MAX_FILE + 1], **data = NULL;
    IODE_REAL   *t_vec = NULL, *f_vec = NULL, lambda; // JMP 22/1/2019
    KDB         *from = NULL, *to = NULL;
    SAMPLE      *t_smpl = NULL;


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




int B_WsTrend(char* arg)
{
    return(B_WsTrendAll(arg, 0));
}

int B_WsTrendStd(char* arg)
{
    return(B_WsTrendAll(arg, 1));
}












