/**
 *  @header4iode
 *
 *  Seasonal adjustment
 *  -------------------
 *
 *  Functions to extract seasonal variations from variables.
 *  
 *  @See GB for details
 */

#include "iode.h"

double  SEASON_EPS = 5.0;

// $WsSeasonAdj Filename VarList
int B_WsSeasonAdj(char *arg)
{
    return(B_season(arg));
}

int B_season(char* arg)
{
    int     nb, lg, rc = -1,
                    i, j, shift, beg, dim, nbper;
    char    name[K_MAX_FILE + 1], **data = NULL;
    IODE_REAL    *t_vec = NULL, *c_vec = NULL, *i_vec = NULL,
                  eps, scale, season[12];
    KDB     *from = NULL, *to = NULL;
    SAMPLE  *t_smpl = NULL;

    lg = B_get_arg0(name, arg, 80);
    data = B_ainit_chk(arg + lg, NULL, 0);
    nb = SCR_tbl_size(data);
    if(nb == 0) goto done;

    eps = atof(data[nb - 1]);
    if(L_IS0(eps)) SEASON_EPS = 5.0;
    else {
        SEASON_EPS = eps;
        nb --;
    }

    from = K_load(K_VAR, name, nb, data);
    if(from == NULL || nb != KNB(from)) {
        B_seterror("Empty data set or inexistant variable");
        goto done;
    }

    nbper = DS_smpl(KSMPL(from), KSMPL(KV_WS), &t_smpl, &shift);
    if(nbper < 0) goto done;

    to = K_create(K_VAR, K_UPPER);
    memcpy((SAMPLE *) KDATA(to), t_smpl, sizeof(SAMPLE));
    nb = t_smpl->s_nb;
    t_vec = (IODE_REAL *) SW_nalloc(nb * sizeof(IODE_REAL));
    c_vec = (IODE_REAL *) SW_nalloc(nb * sizeof(IODE_REAL));
    i_vec = (IODE_REAL *) SW_nalloc(nb * sizeof(IODE_REAL));
    for(i = 0; i < nb; i++) c_vec[i] = i_vec[i] = L_NAN;

    for(i = 0; i < KNB(from); i++) {
        beg = 0;   /* GB 23/07/98 */
        dim = nb;  /* GB 23/07/98 */
        for(j = 0; j < nb; j++) c_vec[j] = i_vec[j] = L_NAN;
        memcpy(t_vec,
               KVVAL(from, i, 0) + shift, nb * sizeof(IODE_REAL));


        if(!DS_test(t_vec, nb, &beg, &dim, nbper, &scale)) {
            memcpy(t_vec,
                   KVVAL(from, i, 0) + shift, nb * sizeof(IODE_REAL));
            K_add(to, KONAME(from, i), t_vec, &(nb));
            continue;
        }

        DS_vec(t_vec + beg, c_vec + beg, i_vec + beg, season,
               dim, nbper, scale);
        DS_extr(t_vec + beg + dim, nb - (beg + dim), nbper,
                season, scale);

        K_add(to, KONAME(from, i), t_vec, &(nb));

        sprintf(name, "_C%s", KONAME(from, i));
        K_add(to, name, c_vec, &(nb));

        sprintf(name, "_I%s", KONAME(from, i));
        K_add(to, name, i_vec, &(nb));
    }
    KV_merge(KV_WS, to, 1);
    rc = 0;

done:
    K_free(to);
    K_free(from);
    SCR_free_tbl(data);

    SW_nfree(t_smpl);
    SW_nfree(t_vec);
    SW_nfree(c_vec);
    SW_nfree(i_vec);

    if(rc < 0) return(-1);
    else return(0);
}


int DS_test(IODE_REAL* vec, int nb, int* beg, int* dim, int nbper, IODE_REAL* shift)
{
    int     f, nj, rc = 0, tolag;
    IODE_REAL    ti[12], maxti = 0.0, minti = 150.0;

    for(*beg = 0; *beg < nb && !L_ISAN(vec[*beg]); (*beg)++);
    /*    if(*beg != 0) *beg = ((*beg)/nbper + 1) * nbper;  GB 23/07/98 */

    *shift = 0.0;
    for(*dim = *beg; *dim < nb && L_ISAN(vec[*dim]); (*dim)++)
        *shift = min(*shift, vec[*dim]);


    if(*shift < 0.0) {
        *shift = -1.1 * (*shift);
        for(f = *beg; f < * dim; f++) vec[f] += *shift;
    }
    else *shift = 0.0;

    /*    *dim = ((*dim)/nbper) *nbper - *beg;  GB 23/07/98 */
    tolag = *dim - ((*dim - *beg)/nbper) *nbper;
    *dim = *dim - tolag;
    *beg = tolag;
    for(f = 0; f < *beg; f++) vec[f] = L_NAN;
    /* GB 23/0/98 */

    for(f = 0; f < nbper; f++) ti[f] = 0.0;

    for(f = *beg + 1; f < *dim + *beg - 1; f++) {
        ti[f%nbper] += 200.0*vec[f]/(vec[f - 1]+vec[f + 1]);
    }

    nj = (*dim)/nbper;
    for(f = 0; f < nbper; f++) {
        if(f == 0 || f == 11) ti[f] /= nj -1;
        else ti[f] /= nj;

        maxti = max(maxti, ti[f]);
        minti = min(minti, ti[f]);
    }

    if(fabs(maxti - minti) > SEASON_EPS) rc = 1; /* seasonal */
    return(rc);
}

int DS_vec(IODE_REAL* vec, IODE_REAL* c1, IODE_REAL* i1, IODE_REAL* season, int nb, int nbper, IODE_REAL shift)
{
    int     m, j, f,
            nbj = nb/nbper;

    IODE_REAL   *ai = (IODE_REAL *) SW_nalloc(nb * sizeof(IODE_REAL)),
                *bi = (IODE_REAL *) SW_nalloc(nb * sizeof(IODE_REAL)),
                *ci = (IODE_REAL *) SW_nalloc((nb + 14) * sizeof(IODE_REAL)),
                *m5 = (IODE_REAL *) SW_nalloc(nb * sizeof(IODE_REAL)),
                *ma = (IODE_REAL *) SW_nalloc((nbj + 4) * sizeof(IODE_REAL)),
                *ma7 = (IODE_REAL *) SW_nalloc((nbj + 6) * sizeof(IODE_REAL)),
                s[12], d, di1;

    static IODE_REAL w5[5] = {1.0, 2.0, 3.0, 2.0, 1.0};
    static IODE_REAL w7[7] = {1.0, 2.0, 3.0, 3.0, 3.0, 2.0, 1.0};
    static IODE_REAL w15[15] = {-3.0, -6.0, -5.0, 3.0, 21.0, 46.0, 67.0, 74.0, 67.0, 46.0, 21.0, 3.0, -5.0, -6.0, -3.0};

    /* Ai, Bi */
    for(f = nbper/2; f < nb - nbper/2; f++) {
        for(m = 0; m < nbper - 1; m++) ai[f] += vec[f - (nbper/2 - 1) + m]/nbper;

        ai[f] += 1.0/(nbper * 2) * (vec[f - nbper/2] + vec[f + nbper/2]);

        bi[f] = 100.0 * vec[f]/ai[f];
    }

    /* Si1 */
    for(m = 0; m < nbper; m++) {
        /* maand per maand */
        if(m < nbper/2) {
            ma[0] = ma[1] = 0.5 * (bi[nbper+m]+bi[2*nbper+m]);
            for(j = 1; j < nbj; j++)
                ma[j + 1] = bi[nbper * j  + m];
            ma[nbj+1] = ma[nbj+2] = 0.5 * (bi[nbper*(nbj -2) + m] + bi[nbper*(nbj -1)+m]);
        }
        else {
            ma[0] = ma[1] = 0.5 * (bi[m]+bi[nbper+m]);
            for(j = 0; j < nbj - 1; j++)
                ma[j + 2] = bi[nbper * j + m];
            ma[nbj+1] = ma[nbj+2]
                        = 0.5 * (bi[nbper*(nbj -3) + m] + bi[nbper*(nbj -2)+m]);
        }

        /* voortschrijdend gemiddelde */
        for(j = 0; j < nbj; j++) {
            m5[j] = 0.0;
            for(f = -2; f < 3; f++) m5[j] += 0.2 * ma[j + 2 + f];
        }

        s[m] = 0.0;
        if(m < nbper/2)
            for(j = 1; j < nbj; j++) {
                d = bi[nbper * j + m] - m5[j -1];
                s[m] += d * d;
            }
        else
            for(j = 0; j < nbj - 1; j++) {
                d = bi[nbper * j + m] - m5[j];
                s[m] += d * d;
            }

        s[m] *= (1.0/(nbj - 2.0));
        s[m] = sqrt(s[m]);

        if(m < nbper/2) {
            for(j = 1; j < nbj; j++) {
                d = fabs(bi[nbper * j + m] - m5[j - 1]);

                if(d > 2 * s[m]) {
                    if(j == 0) bi[m] = 1.0/3.0*(bi[m] + bi[nbper + m] + bi[2*nbper + m]);
                    else {
                        if(j == nbj - 1) {
                            bi[nbper * j + m] =
                                1.0/3.0*(bi[nbper * (j-2) + m] + bi[nbper * (j-1) + m] + bi[nbper * j + m]);
                        }
                        else
                            bi[nbper * j + m] =
                                1.0/3.0*(bi[nbper * (j-1) + m] + bi[nbper * j + m] + bi[nbper * (j+1) + m]);
                    }
                }
            }
        }
        else {
            for(j = 0; j < nbj - 1; j++) {
                d = fabs(bi[nbper * j + m] - m5[j]);

                if(d > 2 * s[m]) {
                    if(j == 0) bi[m] = 1.0/3.0*(bi[m] + bi[nbper + m] + bi[2*nbper + m]);
                    else {
                        if(j == nbj - 1) {
                            bi[nbper * j + m] =
                                1.0/3.0*(bi[nbper * (j-2) + m] + bi[nbper * (j-1) + m] + bi[nbper * j + m]);
                        }
                        else
                            bi[nbper * j + m] =
                                1.0/3.0*(bi[nbper * (j-1) + m] + bi[nbper * j + m] + bi[nbper * (j+1) + m]);
                    }
                }
            }
        }
    }

    for(f = 0; f < nbper/2; f++) bi[f] = bi[f + nbper];
    for(f = nb - nbper/2; f < nb; f++) bi[f] = bi[f - nbper];

    for(j = 0; j < nbj; j++) {
        d = 0.0;
        for(m = 0; m < nbper; m++) d+= bi[nbper * j + m];
        d = (nbper*100 - d)/nbper;
        for(m = 0; m < nbper; m++) bi[nbper * j + m] += d;
    }

    /* Eerste benadering Si */
    for(m = 0; m < nbper; m++) {
        /* maand per maand */
        ma[0] = ma[1] = 0.5 * (bi[m]+bi[nbper+m]);
        for(j = 0; j < nbj; j++)
            ma[j + 2] = bi[nbper * j + m];
        ma[j+2] = ma[j+3] = 0.5 * (bi[nbper*(nbj -2) + m] + bi[nbper*(nbj -1)+m]);

        for(j = 0; j < nbj; j++) {
            m5[j] = 0.0;
            for(f = -2; f < 3; f++) m5[j] += w5[f + 2]/9.0 * ma[j + 2 + f];
            bi[nbper * j + m] = m5[j];
        }
    }

    /* ci = vec / bi */
    for(f = 0; f < nb; f++) ci[f + 7] = 100.0 * vec[f] / bi[f];
    for(f = 0; f < 7; f++)
        ci[f] = 0.25 * (ci[7] + ci[8] + ci[9] + ci[10]);
    for(f = nb + 7; f < nb + 14; f++)
        ci[f] = 0.25 * (ci[nb + 6] + ci[nb + 5] + ci[nb + 4] + ci[nb + 3]);

    /* Spencer en toevallige component*/
    for(f = 0; f < nb ; f++) {
        c1[f] = 0.0;
        for(j = -7; j < 8; j++) c1[f] += w15[j + 7]/320.0 * ci[f + j + 7];

        i1[f] = 100.0 * ci[f+7]/c1[f];
    }

    /* gemiddelde amplitude toevallige component */
    di1 = 0.0;
    for(f = 1; f < nb ; f++) di1 += fabs(i1[f] - i1[f-1])/(nb - 1);

    /* Si2 */
    for(f = 0; f < nb ; f++)  bi[f] = 100.0 * vec[f]/c1[f];

    for(m = 0; m < nbper; m++) {
        /* maand per maand */
        ma[0] = ma[1] = 0.5 * (bi[m]+bi[nbper+m]);
        for(j = 0; j < nbj; j++)  ma[j + 2] = bi[nbper * j  + m];
        ma[nbj+2] = ma[nbj+3] = 0.5 * (bi[nbper*(nbj -2) + m] + bi[nbper*(nbj -1)+m]);

        /* voortschrijdend gemiddelde */
        for(j = 0; j < nbj; j++) {
            m5[j] = 0.0;
            for(f = -2; f < 3; f++) m5[j] += 0.2 * ma[j + 2 + f];
        }

        s[m] = 0.0;
        for(j = 0; j < nbj; j++) {
            d = bi[nbper * j + m] - m5[j];
            s[m] += d * d;
        }

        s[m] *= (1.0/(nbj - 1.0));
        s[m] = sqrt(s[m]);

        for(j = 0; j < nbj; j++) {
            d = fabs(bi[nbper * j + m] - m5[j]);

            if(d > 2 * s[m]) {
                if(j == 0) bi[m] = 1.0/3.0*(bi[m] + bi[nbper + m] + bi[2*nbper + m]);
                else {
                    if(j == nbj - 1)
                        bi[nbper * j + m] =
                            1.0/3.0*(bi[nbper * (j-2) + m] + bi[nbper * (j-1) + m] + bi[nbper * j + m]);
                    else
                        bi[nbper * j + m] =
                            1.0/3.0*(bi[nbper * (j-1) + m] + bi[nbper * j + m] + bi[nbper * (j+1) + m]);
                }
            }
        }
    }

    for(j = 0; j < nbj; j++) {
        d = 0.0;
        for(m = 0; m < nbper; m++) d+= bi[nbper * j + m];
        d = (nbper*100 - d)/nbper;
        for(m = 0; m < nbper; m++) bi[nbper * j + m] += d;
    }

    /* Tweede benadering Si */
    for(m = 0; m < nbper; m++) {
        /* maand per maand */
        if(di1 <= 2.0) {
            ma[0] = ma[1] = 0.5 * (bi[m]+bi[nbper+m]);
            for(j = 0; j < nbj; j++) ma[j + 2] = bi[nbper * j + m];
            ma[j+2] = ma[j+3] = 0.5 * (bi[nbper*(nbj -2) + m] + bi[nbper*(nbj -1)+m]);
            for(j = 0; j < nbj; j++) {
                m5[j] = 0.0;
                for(f = -2; f < 3; f++) m5[j] += w5[f + 2]/9.0 * ma[j + 2 + f];
                bi[nbper * j + m] = m5[j];
            }
        }
        else {
            ma7[0] = ma7[1] = ma7[2] = 0.5 * (bi[m]+bi[nbper+m]);
            for(j = 0; j < nbj; j++) ma7[j + 3] = bi[nbper * j + m];
            ma7[j+3] = ma7[j+4] = ma7[j+5] = 0.5 * (bi[nbper*(nbj -2) + m] + bi[nbper*(nbj -1)+m]);

            for(j = 0; j < nbj; j++) {
                m5[j] = 0.0;
                for(f = -3; f < 4; f++) m5[j] += w7[f + 3]/15.0 * ma7[j + 3 + f];
                bi[nbper * j + m] = m5[j];
            }
        }

        if(nbj > 2)
            season[m] = 0.5*(3* bi[nbper * (nbj - 1) + m] - bi[nbper * (nbj - 2) + m]);
        else
            season[m] = bi[nbper * (nbj - 1) + m];
    }

    /* Adjusted */
    for(f = 0; f < nb ; f++) {
        ci[f + 7] = vec[f] = 100.0 * vec[f]/bi[f];
        vec[f] -= shift;
    }
    for(f = 0; f < 7; f++)
        ci[f] = 0.25 * (ci[7] + ci[8] + ci[9] + ci[10]);
    for(f = nb + 7; f < nb + 14; f++)
        ci[f] = 0.25 * (ci[nb + 6] + ci[nb + 5] + ci[nb + 4] + ci[nb + 3]);
    /* Spencer */
    for(f = 0; f < nb ; f++) {
        c1[f] = 0.0;
        for(j = -7; j < 8; j++) c1[f] += w15[j + 7]/320.0 * ci[f + j + 7];

        i1[f] = 100.0 * ci[f+7]/c1[f];

        c1[f] -= shift;
    }

    SW_nfree(ai);
    SW_nfree(bi);
    SW_nfree(ci);
    SW_nfree(m5);
    SW_nfree(ma);
    SW_nfree(ma7);

    return(0);
}

int DS_smpl(SAMPLE* f_smpl, SAMPLE* ws_smpl, SAMPLE** t_smpl, int* shift)
{
    int     nbper;
    PERIOD  p1, p2;


    if((nbper = PER_nbper(&(ws_smpl->s_p1))) <= 0) {
        B_seterror("Set Ws periodicity first");
        return(-1);
    }

    if(nbper != 12 && nbper !=4) {
        B_seterror("Only Monthly and Quarterly series can be seasonally adjusted");
        return(-1);
    }

    *t_smpl = (SAMPLE *) SW_nalloc(sizeof(SAMPLE));
    PER_common_smpl(f_smpl, ws_smpl, *t_smpl);
    memcpy(&p1, &((*t_smpl)->s_p1), sizeof(PERIOD));
    if(p1.p_s != 1) {
        p1.p_y += 1;
        p1.p_s =  1;
    }

    memcpy(&p2, &((*t_smpl)->s_p2), sizeof(PERIOD));
    if(p2.p_s != nbper) {
        p2.p_y -= 1;
        p2.p_s =  nbper;
    }
    SW_nfree(*t_smpl);

    *t_smpl = PER_pertosmpl(&p1, &p2);
    *shift = PER_diff_per(&((*t_smpl)->s_p1), &(f_smpl->s_p1));
    return(nbper);
}



int DS_extr(IODE_REAL* vec, int dim, int nbper, IODE_REAL* bi, IODE_REAL shift)
{
    int i, m;

    for(i = 0; i < dim && L_ISAN(vec[i]); i++) {
        m = i % nbper;
        vec[i]= 100.0 * vec[i]/bi[m];
        vec[i] -= shift;
    }
    return(0);
}

