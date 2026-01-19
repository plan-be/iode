/**
 *  @header4iode
 * 
 *  Report functions to transform high periodicity to low periodicity series (variables). 
 *  
 *  See b_data.c for more details on the report function syntax and logic.
 *  
 *  List of functions 
 *  -----------------
 *  int B_WsHtoLLast(char* arg, int unused)     $WsHtoLLast Filename VarList
 *  int B_WsHtoLMean(char* arg, int unused)     $WsHtoLMean Filename VarList
 *  int B_WsHtoLSum(char* arg, int unused)      $WsHtoLSum Filename VarList
 */
#include "api/b_args.h"
#include "api/b_errors.h"
#include "api/objs/objs.h"
#include "api/objs/variables.h"

#include "api/report/commands/commands.h"


// Core functions

/**
 *  Computes the target Sample of a conversion of variables from a high to a low periodicity.
 *  
 *  @param [in]   Sample*  f_smpl     sample of the input var file
 *  @param [in]   Sample*  ws_smpl    sample of the current WS 
 *  @param [out]  Sample** t_smpl     allocated target sample
 *  @param [out]  int*     skip       nb of periods to skip (i.e. set to IODE_NAN) in the **target** variable 
 *  @param [out]  int*     shift      nb of periods to skip in the **source** variable 
 *  @return       rc                  0 on success, -1 on error (incompatible perriodicity...) 
 */
static int HTOL_smpl(Sample *f_smpl, Sample *ws_smpl, Sample **t_smpl, int* skip, int* shift)
{
    int     f_nbper, ws_nbper;
    long    s, y;
    Period  p1, p2;

    f_nbper = get_nb_periods_per_year(f_smpl->start_period.periodicity);
    ws_nbper = get_nb_periods_per_year(ws_smpl->start_period.periodicity);

    if(ws_nbper <= 0 || ws_nbper == 12) 
    {
        error_manager.append_error("Set the periodicity first");
        return(-1);
    }

    if(ws_nbper >= f_nbper) 
    {
        error_manager.append_error("File has less observations than the current workspace");
        return(-1);
    }

    p1 = f_smpl->start_period;
    p2 = f_smpl->end_period;

    s = p1.step;
    y = p1.year;
    *shift = f_nbper/ws_nbper;

    p1.periodicity = p2.periodicity = (ws_smpl->start_period).periodicity;
    p1.step = 1;

    p1 = p1.shift((s - 1)/(*shift));
    *skip = (p1.year - y) * f_nbper + (p1.step - 1) * (*shift) - (s - 1);
    p2.step = ws_nbper;

    if(*skip < 0) 
    {
        *skip += *shift;
        p1.year += 1;
    }

    try
    {
        *t_smpl = new Sample(p1, p2);
    }
    catch(const std::exception& e)
    {
        *t_smpl = nullptr;
        std::string err_msg = "High to Low: invalid sample\n" + std::string(e.what()); 
        error_manager.append_error(err_msg);
        return(-1);
    }

    return(0);
}


/**
 *  Transforms high periodicity variables from an external KDB into low periodicity variables in the current WS. 
 *  For example, monthly data in a file into yearly data in the current WS.
 *  
 *  Three methods are implemented : 
 *   - addition of sub-period data: see WsHtoLSum
 *   - average of sub-period data: see WsHtoLMean
 *   - last sub-period value: see WsHtoLLast
 *  
 *  The source periodicity is that of the source file. The target periodicity is the current WS periodicity.
 *  
 *  @param [in] int     method  aggregation method : HTOL_LAST, HTOL_MEAN or HTOL_SUM
 *  @param [in] char*   arg     parameter of the report function (see B_WsHtoLLast() for an example of the syntax)
 *  @return     int             0 on success, -1 on error (file not found, Samples incompatible...)    
 */

static int B_htol(int method, char* arg)
{
    int     res, rc = 0, f, t, shift, skip;
    char    file[K_MAX_FILE + 1];
    double  *t_vec = NULL, *f_vec = NULL;
    int     file_type;
    Sample* t_smpl = nullptr;
    std::vector<std::string> v_data;
    KDBVariables* to = nullptr;
    KDBVariables* from = new KDBVariables(false);

    int lg = B_get_arg0(file, arg, K_MAX_FILE);

    char** data = B_ainit_chk(arg + lg, NULL, 0);
    int nb = SCR_tbl_size((unsigned char**) data);
    if(nb == 0) 
        goto done;

    for(int i = 0; i < nb; i++) 
        v_data.push_back(std::string(data[i]));
        
    file_type = K_findtype(file, VARIABLES);
    if(file_type < 0) 
        from->load_asc(file);
    else
        from->load_binary(file_type, file, v_data);
    if(!from) 
    {
        rc = -1;
        goto done;
    }

    res = HTOL_smpl(from->sample, global_ws_var->sample, &t_smpl, &skip, &shift); 
    if(res < 0) 
    {
        rc = -1;
        goto done;
    }

    if(!t_smpl)
    {
        rc = -1;
        goto done;
    }

    to = new KDBVariables(false);
    to->sample = new Sample(*t_smpl);
    t_vec = (double *) SW_nalloc((1 + t_smpl->nb_periods) * sizeof(double));
    f_vec = (double *) SW_nalloc((1 + from->sample->nb_periods) * sizeof(double));

    for(const auto& [from_name, handle] : from->k_objs) 
    {
        memcpy(f_vec, from->get_var_ptr(from_name), from->sample->nb_periods * sizeof(double));
        memset(t_vec, 0, t_smpl->nb_periods * sizeof(double));

        for(f = 0, t = 0; f < skip; f++) 
        {
            if(f != 0 && f % shift == 0) 
            {
                t_vec[t] = IODE_NAN;
                t++;
            }
        }

        for(; f + shift <= from->sample->nb_periods; t++, f += shift) 
        {
            if(method == HTOL_LAST)
                t_vec[t] = f_vec[f + shift - 1];
            /* SUM and MEAN */
            else
                for(int j = 0; j < shift; j++) 
                {
                    if(IODE_IS_A_NUMBER(f_vec[f + j])) 
                        t_vec[t] += f_vec[f + j];
                    else 
                    {
                        t_vec[t] = IODE_NAN;
                        break;
                    }
                }

            if(method == HTOL_MEAN && IODE_IS_A_NUMBER(t_vec[t])) 
                t_vec[t] /= shift;
        }

        nb = t_smpl->nb_periods;
        for(; t < nb; t++) 
            t_vec[t] = IODE_NAN;

        to->set_obj(from_name, t_vec);
    }

    KV_merge(global_ws_var.get(), to, 1);

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

    if(rc < 0) 
        return(-1);
    else 
        return(0);
}


// Same function but acting on kdb instead of file and global_ws_var (for pyiode - larray)
// !!! NOT TESTED !!!
KDBVariables* B_htol_kdb(int method, KDBVariables* kdb_from)
{
    int      nb, rc = 0, f, t, shift, skip;
    double   *t_vec = NULL, *f_vec = NULL;
    Sample*  t_smpl = nullptr;
    KDBVariables* kdb_to = nullptr;

    int res = HTOL_smpl(kdb_from->sample, kdb_from->sample, &t_smpl, &skip, &shift);
    if(res < 0) 
    {
        rc = -1;
        goto done;
    }

    if(!t_smpl) 
    {
        rc = -1;
        goto done;
    }

    kdb_to = new KDBVariables(false);
    kdb_to->sample = new Sample(*t_smpl);
    t_vec = (double *) SW_nalloc((1 + t_smpl->nb_periods) * sizeof(double));
    f_vec = (double *) SW_nalloc((1 + kdb_from->sample->nb_periods) * sizeof(double));

    for(const auto& [from_name, handle] : kdb_from->k_objs) 
    {
        memcpy(f_vec, kdb_from->get_var_ptr(from_name), kdb_from->sample->nb_periods * sizeof(double));
        memset(t_vec, 0, t_smpl->nb_periods * sizeof(double));

        for(f = 0, t = 0; f < skip; f++) 
        {
            if(f != 0 && f % shift == 0) 
            {
                t_vec[t] = IODE_NAN;
                t++;
            }
        }

        for(; f + shift <= kdb_from->sample->nb_periods; t++, f += shift) 
        { 
            if(method == HTOL_LAST)
                t_vec[t] = f_vec[f + shift - 1];
            /* SUM and MEAN */
            else
                for(int j = 0; j < shift; j++) 
                {
                    if(IODE_IS_A_NUMBER(f_vec[f + j])) 
                        t_vec[t] += f_vec[f + j];
                    else 
                    {
                        t_vec[t] = IODE_NAN;
                        break;
                    }
                }

            if(method == HTOL_MEAN && IODE_IS_A_NUMBER(t_vec[t])) 
                t_vec[t] /= shift;
        }

        nb = t_smpl->nb_periods;
        for(; t < nb; t++) 
            t_vec[t] = IODE_NAN;

        kdb_to->set_obj(from_name, t_vec);
    }

done:
    SW_nfree(t_smpl);
    SW_nfree(t_vec);
    SW_nfree(f_vec);

    if(rc < 0) 
        return(NULL);
    else 
        return(kdb_to);
}


// Report functions

/**
 *  Syntax: $WsHtoLLast Filename VarList
 *  
 *  @see https://iode.plan.be/doku.php?id=WsHtoLLast
 */
int B_WsHtoLLast(char* arg, int unused)
{
    return(B_htol(HTOL_LAST, arg));
}


/**
 *  Syntax: $WsHtoLMean Filename VarList
 *  
 *  @see https://iode.plan.be/doku.php?id=WsHtoLMean
 */
int B_WsHtoLMean(char* arg, int unused)
{
    return(B_htol(HTOL_MEAN, arg));
}


/**
 *  Syntax: $WsHtoLSum Filename VarList
 *  
 *  @see https://iode.plan.be/doku.php?id=WsHtoLSum
 */
int B_WsHtoLSum(char* arg, int unused)
{
    return(B_htol(HTOL_SUM, arg));
}
