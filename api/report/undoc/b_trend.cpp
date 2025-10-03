/**
 *  @header4iode
 * 
 *  
 *  Implementation of the **Hodrick-Prescott** method for trend series construction. 
 *  The principle is the same as for deseasonalisation: the series read from a file are imported 
 *  and transformed simultaneously.
 *  
 *  For $WsTrend, the series are log-transformed before the calculation and re-transformed into exp after the calculation. 
 *  They must therefore be strictly positive.
 *  
 *  In $WsTrendStd, the series are not transformed before the calculation. The positivity constraint does therefore not apply.
 *  
 *  List of functions 
 *  -----------------
 *   int B_WsTrend(char* arg)     | $WsTrend VarFilename Lambda series1 series2 ...
 *   int B_WsTrendStd(char* arg)  | $WsTrendStd VarFilename Lambda series1 series2 ...
 *     
 */
#include <string.h>

#include "api/constants.h"
#include "api/b_args.h"
#include "api/b_errors.h"
#include "api/objs/objs.h"
#include "api/objs/variables.h"

#include "api/report/undoc/undoc.h"


static int HP_smpl(Sample* f_smpl, Sample* ws_smpl, Sample** t_smpl, int* shift)
{
    int nbper = get_nb_periods_per_year(ws_smpl->start_period.periodicity);

    if(nbper <= 0) 
    {
        error_manager.append_error("Set periodicity first");
        goto err;
    }

    try
    {
        *t_smpl = new Sample(f_smpl->intersection(*ws_smpl));
        *shift = (*t_smpl)->start_period.difference(f_smpl->start_period);
        return(nbper);
    }
    catch(const std::exception& e)
    {
        error_manager.append_error(e.what());
        goto err;
    }

err:
    if(*t_smpl) 
        delete *t_smpl;
    t_smpl = nullptr;
    return -1;
}


static int B_WsTrendAll(char* arg, int std)
{
    int     lg, nb, rc = 0, i, shift = 0, /* lambda,*/ beg, dim = 0;
    char    file[K_MAX_FILE + 1], **data = NULL;
    double  *t_vec = NULL, *f_vec = NULL, lambda; // JMP 22/1/2019
    KDB*    to = nullptr;
    Sample* t_smpl = nullptr;
    KDB*    from = new KDB(VARIABLES, DB_STANDALONE);

    lg = B_get_arg0(file, arg, 80);

    data = B_ainit_chk(arg + lg, NULL, 0);
    // lambda = atoi(data[0]); // JMP  22/1/2019
    lambda = atof(data[0]); // JMP  22/1/2019
    nb = SCR_tbl_size((unsigned char**) data + 1);
    if(nb == 0) 
        goto done;

    from = K_load(VARIABLES, file, nb, data + 1, 0);
    if(!from) 
    {
        rc = -1;
        goto done;
    }

    if(HP_smpl(from->sample, KV_WS->sample, &t_smpl, &shift) < 0) 
        goto done;

    if(!t_smpl) 
    {
        rc = -1;
        goto done;
    }

    to = new KDB(VARIABLES, DB_STANDALONE);
    nb = t_smpl->nb_periods;
    to->sample = new Sample(*t_smpl);
    t_vec = (double *) SW_nalloc(nb * sizeof(double));
    f_vec = (double *) SW_nalloc(nb * sizeof(double));

    for(i = 0; i < KNB(from); i++) 
    {
        memcpy(f_vec, KVVAL(from, i, 0) + shift, nb * sizeof(double));
        HP_test(f_vec, t_vec, nb, &beg, &dim);
        HP_calc(f_vec + beg, t_vec + beg, dim, lambda, std); //  JMP 12/4/2019
        K_add(to, KONAME(from, i), t_vec, &(nb));
    }

    KV_merge(KV_WS, to, 1);

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
    SW_nfree(f_vec);

    if(rc < 0) return(-1);
    else return(0);
}



// $WsTrend VarFilename Lambda series1 series2 ...
int B_WsTrend(char* arg, int unused)
{
    return(B_WsTrendAll(arg, 0));
}


// $WsTrendStd VarFilename Lambda series1 series2 ...
// Unlike the $WsTrend function, the series are not transformed before the calculation. 
// The positivity constraint does not apply.
int B_WsTrendStd(char* arg, int unused)
{
    return(B_WsTrendAll(arg, 1));
}












