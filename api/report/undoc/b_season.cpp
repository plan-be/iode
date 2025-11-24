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
#include <math.h>
#include <string.h>

#include "api/constants.h"
#include "api/b_args.h"
#include "api/b_errors.h"
#include "api/objs/objs.h"
#include "api/objs/variables.h"

#include "api/report/undoc/undoc.h"
#include <algorithm>    // for std::min, std::max

double  SEASON_EPS = 5.0;


// $WsSeasonAdj Filename VarList
int B_WsSeasonAdj(char *arg, int unused)
{
    return(B_season(arg));
}

int B_season(char* arg)
{
    int     res, rc = -1, shift, beg, dim, nbper;
    char    name[K_MAX_FILE + 1];
    double  *t_vec = NULL, *c_vec = NULL, *i_vec = NULL,
            eps, scale, season[12];
    KDB*    to = nullptr;
    Sample* t_smpl = nullptr;
    KDB*    from = new KDB(VARIABLES, DB_STANDALONE);

    int lg = B_get_arg0(name, arg, 80);
    char** data = B_ainit_chk(arg + lg, NULL, 0);
    int nb = SCR_tbl_size((unsigned char**) data);
    if(nb == 0) 
        goto done;

    eps = atof(data[nb - 1]);
    if(IODE_IS_0(eps)) 
        SEASON_EPS = 5.0;
    else 
    {
        SEASON_EPS = eps;
        nb --;
    }

    from = K_load(VARIABLES, name, nb, data, 0);
    if(!from || nb != from->size()) 
    {
        error_manager.append_error("Empty data set or inexistent variable");
        goto done;
    }

    nbper = DS_smpl(from->sample, KV_WS->sample, &t_smpl, &shift);
    if(nbper < 0 || t_smpl == nullptr) 
        goto done;
    
    to = new KDB(VARIABLES, DB_STANDALONE);
    to->sample = new Sample(*t_smpl);
    nb = t_smpl->nb_periods;
    t_vec = (double *) SW_nalloc(nb * sizeof(double));
    c_vec = (double *) SW_nalloc(nb * sizeof(double));
    i_vec = (double *) SW_nalloc(nb * sizeof(double));
    for(int i = 0; i < nb; i++) 
        c_vec[i] = i_vec[i] = IODE_NAN;

    for(auto& [from_name, _] : from->k_objs) 
    {
        beg = 0;   /* GB 23/07/98 */
        dim = nb;  /* GB 23/07/98 */
        for(int j = 0; j < nb; j++) 
            c_vec[j] = i_vec[j] = IODE_NAN;
        memcpy(t_vec, KVVAL(from, from_name, 0) + shift, nb * sizeof(double));

        res = DS_test(t_vec, nb, &beg, &dim, nbper, &scale);
        if(!res) 
        {
            memcpy(t_vec, KVVAL(from, from_name, 0) + shift, nb * sizeof(double));
            to->add(from_name, t_vec, nb);
            continue;
        }

        DS_vec(t_vec + beg, c_vec + beg, i_vec + beg, season, dim, nbper, scale);
        DS_extr(t_vec + beg + dim, nb - (beg + dim), nbper, season, scale);

        to->add(from_name, t_vec, nb);

        sprintf(name, "_C%s", from_name.c_str());
        to->add(name, c_vec, nb);

        sprintf(name, "_I%s", from_name.c_str());
        to->add(name, i_vec, nb);
    }
    KV_merge(KV_WS.get(), to, 1);
    rc = 0;

done:
    if(to)
        delete to;
    to = nullptr;
    if(from)
        delete from;
    from = nullptr;
    if(t_smpl)
        delete t_smpl;
    t_smpl = nullptr;

    SCR_free_tbl((unsigned char**) data);
    SW_nfree(t_vec);
    SW_nfree(c_vec);
    SW_nfree(i_vec);

    if(rc < 0) 
        return(-1);
    else 
        return(0);
}


int DS_test(double* vec, int nb, int* beg, int* dim, int nbper, double* shift)
{
    int     f, nj, rc = 0, tolag;
    double    ti[12], maxti = 0.0, minti = 150.0;

    for(*beg = 0; *beg < nb && !IODE_IS_A_NUMBER(vec[*beg]); (*beg)++);
    /*    if(*beg != 0) *beg = ((*beg)/nbper + 1) * nbper;  GB 23/07/98 */

    *shift = 0.0;
    for(*dim = *beg; *dim < nb && IODE_IS_A_NUMBER(vec[*dim]); (*dim)++)
        *shift = std::min(*shift, vec[*dim]);


    if(*shift < 0.0) {
        *shift = -1.1 * (*shift);
        for(f = *beg; f < * dim; f++) vec[f] += *shift;
    }
    else *shift = 0.0;

    /*    *dim = ((*dim)/nbper) *nbper - *beg;  GB 23/07/98 */
    tolag = *dim - ((*dim - *beg)/nbper) *nbper;
    *dim = *dim - tolag;
    *beg = tolag;
    for(f = 0; f < *beg; f++) vec[f] = IODE_NAN;
    /* GB 23/0/98 */

    for(f = 0; f < nbper; f++) ti[f] = 0.0;

    for(f = *beg + 1; f < *dim + *beg - 1; f++) {
        ti[f%nbper] += 200.0*vec[f]/(vec[f - 1]+vec[f + 1]);
    }

    nj = (*dim)/nbper;
    for(f = 0; f < nbper; f++) {
        if(f == 0 || f == 11) ti[f] /= nj -1;
        else ti[f] /= nj;

        maxti = std::max(maxti, ti[f]);
        minti = std::min(minti, ti[f]);
    }

    if(fabs(maxti - minti) > SEASON_EPS) rc = 1; /* seasonal */
    return(rc);
}

int DS_vec(double* vec, double* c1, double* i1, double* season, int nb, int nbper, double shift)
{
    int     m, j, f,
            nbj = nb/nbper;

    double   *ai = (double *) SW_nalloc(nb * sizeof(double)),
                *bi = (double *) SW_nalloc(nb * sizeof(double)),
                *ci = (double *) SW_nalloc((nb + 14) * sizeof(double)),
                *m5 = (double *) SW_nalloc(nb * sizeof(double)),
                *ma = (double *) SW_nalloc((nbj + 4) * sizeof(double)),
                *ma7 = (double *) SW_nalloc((nbj + 6) * sizeof(double)),
                s[12], d, di1;

    static double w5[5] = {1.0, 2.0, 3.0, 2.0, 1.0};
    static double w7[7] = {1.0, 2.0, 3.0, 3.0, 3.0, 2.0, 1.0};
    static double w15[15] = {-3.0, -6.0, -5.0, 3.0, 21.0, 46.0, 67.0, 74.0, 67.0, 46.0, 21.0, 3.0, -5.0, -6.0, -3.0};

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

int DS_smpl(Sample* f_smpl, Sample* ws_smpl, Sample** t_smpl, int* shift)
{
    if(!f_smpl)
        return -1;

    if(!ws_smpl)
        return -1;
    
    int nbper = get_nb_periods_per_year(ws_smpl->start_period.periodicity);
    if(nbper <= 0) 
    {
        error_manager.append_error("Set periodicity first");
        return(-1);
    }

    if(nbper != 12 && nbper !=4) {
        error_manager.append_error("Only Monthly and Quarterly series can be seasonally adjusted");
        return(-1);
    }

    try
    {
        *t_smpl = new Sample(f_smpl->intersection(*ws_smpl));
        Period p1 = (*t_smpl)->start_period;
        if(p1.step != 1) 
        {
            p1.year += 1;
            p1.step =  1;
        }
    
        Period p2 = (*t_smpl)->end_period;
        if(p2.step != nbper) 
        {
            p2.year -= 1;
            p2.step =  nbper;
        }
        delete (*t_smpl);

        *t_smpl = new Sample(p1, p2);
        *shift = (*t_smpl)->start_period.difference(f_smpl->start_period);
    }
    catch(const std::exception& e)
    {
        error_manager.append_error(e.what());
        return(-1);
    }

    return(nbper);
}



int DS_extr(double* vec, int dim, int nbper, double* bi, double shift)
{
    int i, m;

    for(i = 0; i < dim && IODE_IS_A_NUMBER(vec[i]); i++) {
        m = i % nbper;
        vec[i]= 100.0 * vec[i]/bi[m];
        vec[i] -= shift;
    }
    return(0);
}

