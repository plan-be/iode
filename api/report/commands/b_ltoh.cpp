/**
 *  @header4iode
 * 
 *  Report functions to transform low periodicity series to high periodicity series (i.e. variables). 
 *  
 *  Two types of series are considered: stock and flow:
 *      - in the first case, the interpolated values are of the same order of magnitude as the original values
 *      - in the latter case, the values of the sub-periods are additive over a period.
 *  
 *  See b_data.c for more details on the report function syntax and logic.
 *  
 *  List of public functions 
 *  ------------------------
 *      int B_WsLtoHStock(char* arg, int unused)    $WsLtoHStock {L|C|S} Filename VarList
 *      int B_WsLtoHFlow(char* arg, int unused)     $WsLtoHFlow  {L|C|S} Filename VarList
 */
#include <math.h>

#include "api/b_args.h"
#include "api/b_errors.h"
#include "api/objs/objs.h"
#include "api/objs/variables.h"

#include "api/report/commands/commands.h"


// Linear Interpolation sub-functions 

/**
 *  Calculates the linear interpolation of y in x (~y[x])
 *  @param [in] double*  y   vector of double
 *  @param [in] double   x   x-value for which y[x] is seached 
 *  @return     double       approximation of y[x] 
 */ 
static double LTOH_ylin(double* y, double x)
{

    double    res, a, b;
    int     hi, lo;

    lo = (int) floor(x);
    hi = (int) (lo + 1.0);

    a = hi - x;
    b = x - lo;
    
    if(IODE_IS_A_NUMBER(y[lo]) && IODE_IS_A_NUMBER(y[hi]))
        res = a*y[lo] + b*y[hi];
    else res = IODE_NAN;
    return(res);
}


/**
 *  Transforms a low periodicity series to high periodicity series by linear interpolation (stock or flow series). 
 *  
 *  If the series is a "flow", the result is the source value divided by the nb of sub-periods. 
 *  If the series is a "stock", the result is a linear interpolation of the 2 surrounding source values.
 *  
 *  @param [in]  int         type   of series: LTOH_FLOW or LTOH_STOCK
 *  @param [in]  double*  f_vec  source vector
 *  @param [in]  int         f_nb   nb of values in the source vector
 *  @param [out] double*  t_vec  destination vector 
 *  @param [in]  int         t_nb   nb of values in the destination vector
 *  @param [in]  int         shift  nb of sub periods in the destination sample
 *  @return      int         0 always       
 */
static int LTOH_lin(int type, double* f_vec, int f_nb, double* t_vec, int t_nb, int shift)
{
    int     dim, j, f, t, beg;
    double    x;

    for(t = 0; t < t_nb; t++) t_vec[t] = IODE_NAN;
    beg = 0;
    while(1) {
        while(beg < f_nb && !IODE_IS_A_NUMBER(f_vec[beg])) beg++;
        if(beg >= f_nb) break;
        for(dim = beg; dim < f_nb && IODE_IS_A_NUMBER(f_vec[dim]); dim++);
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
                        x = (double)(j + 1) / (double) shift;
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


/**
 *  Transforms a low periodicity series to high periodicity series by using step interpolation (stock or flow series). 
 *  
 *  If the series is a "flow", the result is the source value plus a portion of the difference bw the 2 surrounding values in the source
 *  If the series is a "stock", the result has the same value as the source
 *  
 *  @param [in]  int         type   of series: LTOH_FLOW or LTOH_STOCK
 *  @param [in]  double*  f_vec  source vector
 *  @param [in]  int         f_nb   nb of values in the source vector
 *  @param [out] double*  t_vec  destination vector 
 *  @param [in]  int         t_nb   nb of values in the destination vector
 *  @param [in]  int         shift  nb of sub periods in the destination sample
 *  @return      int         0 always       
 */
static int LTOH_step(int type, double* f_vec, int f_nb, double* t_vec, int t_nb, int shift)
{
    int     dim, j, f, t, beg;

    for(t = 0; t < t_nb; t++) t_vec[t] = IODE_NAN;
    beg = 0;
    while(1) {
        while(beg < f_nb && !IODE_IS_A_NUMBER(f_vec[beg])) beg++;
        if(beg >= f_nb) break;
        for(dim = beg; dim < f_nb && IODE_IS_A_NUMBER(f_vec[dim]); dim++);

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

// Cubic Spline sub-functions 

static int LTOH_y2cs(double* y, int n, double* y2)
{
    int     i, dim;
    double    *u, p, s;

    /* "natural" cubic spline */
    u = (double *) SW_nalloc(n * sizeof(double));
    y2[0] = u[0] = 0.0;

    for(i = 1; i < n - 1 && IODE_IS_A_NUMBER(y[i]); i++) {
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


static double LTOH_ycs(double* y, double* y2, double x)
{

    double    res, a, b;
    int     hi, lo;

    if(x < 0) return(IODE_NAN);

    lo = (int) floor(x);
    hi = (int) (lo + 1.0);

    a = hi - x;
    b = x - lo;

    if(IODE_IS_A_NUMBER(y[lo]) && IODE_IS_A_NUMBER(y[hi]))
        res = a*y[lo] + b*y[hi] +
              ((a*a*a - a)*y2[lo] + (b*b*b - b)*y2[hi])/6.0;
    else res = IODE_NAN;
    return(res);
}


static int LTOH_cs(int type, double* f_vec, int f_nb, double* t_vec, int t_nb, int shift)
{
    int     dim, j, f, t, beg;
    double    *y2 = (double *) SW_nalloc(sizeof(double) * f_nb),
                  x, sum, d;

    for(t = 0; t < t_nb; t++) t_vec[t] = IODE_NAN;
    beg = f = 0;
    while(1) {
        while(beg < f_nb && !IODE_IS_A_NUMBER(f_vec[beg])) beg++;
        if(beg >= f_nb) break;

        if(type == LTOH_FLOW) {
            for(f = beg; f < f_nb && IODE_IS_A_NUMBER(f_vec[f]); f++) f_vec[f] = f_vec[f]/shift;
            dim = LTOH_y2cs(f_vec + beg, f_nb - beg, y2);

            for(f = beg, t = beg*shift; f < dim + beg; f++, t+= shift) {
                sum = 0.0;
                for(j = 0; j < shift; j++) {
                    x = (double)(j - 1) / (double) shift;
                    x += f;
                    t_vec[t + j] = LTOH_ycs(f_vec, y2, x);
                    if(IODE_IS_A_NUMBER(t_vec[t + j]) && IODE_IS_A_NUMBER(sum)) sum += t_vec[t + j];
                    else sum = IODE_NAN;
                }

                if(IODE_IS_A_NUMBER(sum)) {
                    d = (f_vec[f] * shift - sum)/shift;
                    for(j = 0; j < shift; j++) t_vec[t + j] += d;
                }
            }
        }
        else {
            dim = LTOH_y2cs(f_vec + beg, f_nb - beg, y2);
            for(f = beg, t = (beg+1)*shift; f < dim + beg; f++, t+= shift) {
                for(j = 0; j < shift; j++) {
                    x = (double)(j + 1) / (double) shift;
                    x += f;
                    t_vec[t + j] = LTOH_ycs(f_vec, y2, x);
                }
            }
        }
        beg += dim + 1;
    }
    return(0);
}


/**
 *  Computes the target Sample of a conversion of variables from a low to a high periodicity.
 *  
 *  @param [in]   Sample*  f_smpl     sample of the input var file
 *  @param [in]   Sample*  ws_smpl    sample of the current WS 
 *  @param [out]  Sample** t_smpl     allocated target sample
 *  @param [out]  int*     skip       nb of periods to skip (i.e. set to IODE_NAN) in the **target** variable 
 *  @param [out]  int*     shift      nb of periods to skip in the **source** variable 
 *  @return       rc                  0 on success, -1 on error (incompatible perriodicity...) 
 */
static int LTOH_smpl(Sample* f_smpl, Sample* ws_smpl, Sample** t_smpl, int* skip, int* shift)
{
    int     f_nbper, ws_nbper;
    long    s, y;
    Period  p1, p2;

    f_nbper = get_nb_periods_per_year(f_smpl->start_period.periodicity);
    ws_nbper = get_nb_periods_per_year(ws_smpl->start_period.periodicity);

    if(ws_nbper <= 0) {
        error_manager.append_error("Set the periodicity first");
        return(-1);
    }

    if(ws_nbper <= f_nbper) {
        error_manager.append_error("File has more observations than the current workspace");
        return(-1);
    }

    p1 = f_smpl->start_period;
    p2 = f_smpl->end_period;

    s = p1.step;
    y = p1.year;
    *shift = ws_nbper/f_nbper;

    p1.periodicity = p2.periodicity = (ws_smpl->start_period).periodicity;
    p1.step = 1;

    p1 = p1.shift((s - 1)/(*shift));
    /*
        *skip = (p1.year - y ) * ws_nbper + (p1.step - 1) * (*shift) - (s - 1);
    */
    *skip = 0;
    p2.step = ws_nbper;

    try
    {
        *t_smpl = new Sample(p1, p2);
    }
    catch(const std::exception& e)
    {
        *t_smpl = nullptr;
        std::string error_msg = "Low to High: invalid sample\n" + std::string(e.what());
        error_manager.append_error(e.what());
        return(-1);
    }

    return(0);
}


/**
 *  Sub-function of the report functions B_WsLtoHStock() and B_WsLtoHFlow().
 *  
 *  @param [in] int     type    type of series: LTOH_FLOW or LTOH_STOCK
 *  @param [in] char*   arg     parameter passed from the calling report function
 *  @return     int             0 on success, not null on error (file not found, incompatible samples...)
 */

static int B_ltoh(int type, char* arg)
{
    int     res, rc = 0, shift, skip;
    char    method[81], file[K_MAX_FILE + 1];
    double  *t_vec = NULL, *f_vec = NULL;
    int     file_type;
    Sample* t_smpl = nullptr;
    std::vector<std::string> v_data;
    KDBVariables* to = nullptr;
    KDBVariables* from = new KDBVariables(false);

    int lg = B_get_arg0(method, arg, 80);
    U_sqz_text((unsigned char*) method);

    lg += B_get_arg0(file, arg + lg, K_MAX_FILE);

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

    res = LTOH_smpl(from->sample, global_ws_var->sample, &t_smpl, &skip, &shift);
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
        switch(method[0]) 
        {
            case LTOH_CS :
                LTOH_cs(type,
                        f_vec, (int) from->sample->nb_periods,
                        t_vec, (int) t_smpl->nb_periods,
                        shift);
                break;

            case LTOH_STEP:
                LTOH_step(type,
                          f_vec, (int) from->sample->nb_periods,
                          t_vec, (int) t_smpl->nb_periods,
                          shift);
                break;

            default       :
            case LTOH_LIN :
                LTOH_lin(type,
                         f_vec, (int) from->sample->nb_periods,
                         t_vec, (int) t_smpl->nb_periods,
                         shift);
                break;
        }
        nb = t_smpl->nb_periods;
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


/**
 *  Syntax: $WsLtoHStock {L|C|S} Filename VarList
 *
 *   where L stands for Linear interpolation
 *         C stands fot Cubic Spliness
 *         S for interpolation by Steps
 *  
 *
 */
int B_WsLtoHStock(char* arg, int unused)
{
    return(B_ltoh(LTOH_STOCK, arg));
}


/**
 *  Syntax: $WsLtoHFlow {L|C|S}  Filename VarList
 *
 *   where L stands for Linear interpolation
 *         C stands fot Cubic Splines
 *         S for interpolation by Steps
 *  
 *
 */
int B_WsLtoHFlow(char* arg, int unused)
{
    return(B_ltoh(LTOH_FLOW, arg));
}
