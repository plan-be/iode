/**
 *  @header4iode
 * 
 *  Functions to import variables from the legacy GEM (Chronos) format (obsolete). 
 *  
 *  GEM VAR format
 *  -----------------
 *     nobs nser rub .....
 *     subrub[nser1][nobs1] subrub[nser2][nobs1] ...
 *     ....
 *     subrub[nser1][nobs2] subrub[nser1][nobs2] ...
 *
 *  List of functions 
 *  -----------------
 *  See k_idif.c for a similar group of functions.
 */
#include <math.h>

#include "api/k_super.h"
#include "api/constants.h"
#include "api/utils/yy.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/io/dif.h"
#include "api/io/import.h"


/*  NEW GEM FORMAT
    Reads a GEM file: CRONOS format

    nobs nser rub .....
    subrub[nser1][nobs1] subrub[nser2][nobs1] ...
    ....
    subrub[nser1][nobs2] subrub[nser1][nobs2] ...
*/


int ImportObjsGEM::read_header(YYFILE* yy, Sample* smpl)
{
    memcpy(&GEM_smpl, smpl, sizeof(Sample));
    GEM_freq = (GEM_smpl.start_period).periodicity;
    GEM_nbper = get_nb_periods_per_year(GEM_smpl.start_period.periodicity);
    if(GEM_nbper < 0) 
    {
        kerror(0, "Please specify FROM and TO period");
        return(-1);
    }

    return(0);
}

double ImportObjsGEM::GEM_read_real(YYFILE* yy)
{
    double    val;
    int     minus = 1, p = 0;

ag:
    switch(YY_lex(yy)) {
        case YY_DOUBLE :
            val = minus * yy->yy_double;
            break;
        case YY_LONG   :
            val = minus * yy->yy_long;
            break;
        case YY_SPECIAL:
            if(yy->yy_text[0] == '-') {
                minus = -1;
                goto ag;
            }
            YY_unread(yy);
            return(IODE_NAN);

        case YY_WORD   :
            if(strcmp("na", (char*) yy->yy_text) != 0)
                YY_unread(yy);
            return(IODE_NAN);
        default :
            YY_unread(yy);
            return(IODE_NAN);
    }

    if(YY_lex(yy) == YY_WORD) {
        // YY_lex(yy); /* JMP 16-05-00 */
        p = K_read_long(yy);
        val *= pow(10.0, (double) p);
    }
    else YY_unread(yy);
    return(val);
}

int ImportObjsGEM::read_variable(YYFILE* yy, char* name, int dim, double* vector)
{
    int     i, from;

    if(GEM_cser == GEM_nser) {
        GEM_cser = 0;
        GEM_nobs = K_read_long(yy);
        GEM_nser = K_read_long(yy);

        if(GEM_nser * GEM_nobs == 0)  return(-1);

        if(GEM_mat) SCR_free(GEM_mat);
        GEM_mat = (double *) SCR_malloc(GEM_nser * GEM_nobs * sizeof(double));

        GEM_readrubr(yy);
        YY_skip_to_eol(yy);
        for(i = 0; i < GEM_nobs * GEM_nser; i++) GEM_mat[i] = GEM_read_real(yy);
    }

    GEM_name(name);
    from = GEM_smpl.nb_periods - GEM_nobs;                            /* JMP 16-05-00 */
    if(from < 0) from = 0;                                      /* JMP 16-05-00 */
    for(i = 0; i < from; i++) vector[i] = IODE_NAN;                /* JMP 16-05-00 */
    for(i = from; i < GEM_smpl.nb_periods; i++) {     /* JMP 16-05-00 */
        if(i < dim) vector[i] = GEM_mat[GEM_cser + (i - from) *(GEM_nser)];
    }

    GEM_cser++;

    return(0);
}

int ImportObjsGEM::close()
{
    return(0);
}

int ImportObjsGEM::GEM_readrubr(YYFILE* yy)
{
    YY_lex(yy);
    strcpy(GEM_rubr, (char*) yy->yy_text);
    return(0);
}

int ImportObjsGEM::GEM_name(char* name)
{
    if(GEM_cser < 9) sprintf(name, "%s_0%d", GEM_rubr, GEM_cser + 1);
    else             sprintf(name, "%s_%d", GEM_rubr, GEM_cser + 1);
    return(0);
}
