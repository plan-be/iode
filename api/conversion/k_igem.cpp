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
#include "api/utils/time.h"
#include "api/conversion/dif.h"
#include "api/conversion/import.h"

/*  OLD GEM FORMAT
    Reads a GEM file: CRONOS format

    m x  {nser nobs rub {nser x subrub } {nser x nobs} values}

SAMPLE  GEM_smpl;
char    GEM_freq;
int     GEM_nbper;

char    GEM_rubr[81];
char    **GEM_srubr = NULL;
int     GEM_nser = 0, GEM_cser, GEM_nobs = 0,
	GEM_s1, GEM_s2;
PERIOD  GEM_begin;

int IMP_hd_gem(yy, smpl)
YYFILE  *yy;
SAMPLE  *smpl;
{
    int     type;
    double  value;

    memcpy(&GEM_smpl, smpl, sizeof(SAMPLE));
    GEM_freq = (GEM_smpl.s_p1).p_p;
    GEM_nbper = PER_nbper(&(GEM_smpl.s_p1));
    if(GEM_nbper < 0) {
	kerror(0, "Please specify FROM and TO period");
	return(-1);
    }

    return(0);
}


int IMP_vec_gem(yy, name, dim, vector)
YYFILE  *yy;
char    *name;
int     dim;
 double    *vector;
{
    int     i;
    double    val;

    if(GEM_cser == GEM_nser) {
	GEM_cser = 0;
	GEM_nser = K_read_long(yy);
	GEM_nobs = K_read_long(yy);

	if(GEM_nser * GEM_nobs == 0)  return(-1);

	GEM_readrubr(yy);
	GEM_readbegin(yy);
	GEM_readsrubr(yy);

	GEM_s1 = PER_diff_per(&GEM_begin, &(GEM_smpl.s_p1));
	if(GEM_s1 < 0) {
	    GEM_s2 = PER_diff_per(&(GEM_smpl.s_p1), &GEM_begin);
	    GEM_s1 = 0;
	}
	else GEM_s2 = 0;
    }

    GEM_name(name, GEM_rubr, GEM_srubr[GEM_cser]);

    for(i = 0; i < GEM_s2; i++) K_read_real(yy);
    for(i = 0; i < min(dim - GEM_s1, GEM_nobs - GEM_s2); i++) {
	val =  K_read_real(yy);
	if(strcmp(yy->yy_text, "9999999")) vector[i + GEM_s1] = val;
    }
    for(; i < GEM_nobs - GEM_s2; i++) K_read_real(yy);

    GEM_cser++;

    return(0);
}

IMP_end_gem()
{
    SCR_free_tbl(GEM_srubr);
    GEM_srubr = 0;
    return(0);
}

IMPDEF IMP_GEM = {
    NULL,           // imp_keys
    0,              // imp_dim
    NULL,           // imp_hd_fn
    IMP_hd_gem,     // imp_hd_sample_fn
    IMP_vec_gem,    // imp_vec_var_fn
    NULL,           // imp_vec_cmt_fn
    NULL,           // imp_elem_fn   
    IMP_end_gem     // imp_end_fn
};


GEM_readrubr(yy)
YYFILE  *yy;
{
    YY_lex(yy);
    YY_lex(yy);
    strcpy(GEM_rubr, yy->yy_text);
    YY_lex(yy);
    while(yy->yy_text[0] == '+') {
	strcat(GEM_rubr, "+");
	YY_lex(yy);
    }
    strcat(GEM_rubr, yy->yy_text);
    return(0);
}

GEM_readbegin(yy)
YYFILE  *yy;
{
    char    buf[30];
    PERIOD  *tmp = NULL;

    buf[0] = 0;
    if(YY_lex(yy) != YY_LONG) goto err;
    strcpy(buf, yy->yy_text);
    strcat(buf, "Y1");

    tmp = PER_atoper(buf);
    memcpy(&GEM_begin, tmp, sizeof(PERIOD));

    YY_lex(yy); // ;
    SCR_free(tmp);
    return(0);
err:
    return(-1);
}

GEM_readsrubr(yy)
YYFILE  *yy;
{
    int     i;

    SCR_free_tbl(GEM_srubr);
    GEM_srubr = 0;
    for(i = 0; i < GEM_nser;) {
	YY_lex(yy);
	SCR_add_ptr(&GEM_srubr, &i, yy->yy_text);
    }

    SCR_add_ptr(&GEM_srubr, &i, NULL);
    return(0);
}

GEM_name(name, rub, srub)
char    *name, *rub, *srub;
{
    strcpy(name, rub);
    SCR_replace(name, "+++", srub);
    return(0);
}
*/




/*  NEW GEM FORMAT
    Reads a GEM file: CRONOS format

    nobs nser rub .....
    subrub[nser1][nobs1] subrub[nser2][nobs1] ...
    ....
    subrub[nser1][nobs2] subrub[nser1][nobs2] ...
*/

SAMPLE  GEM_smpl;
char    GEM_freq;
int     GEM_nbper;

char    GEM_rubr[81];
int     GEM_nser = 0, GEM_cser, GEM_nobs = 0;
double    *GEM_mat = NULL;


int IMP_hd_gem(YYFILE* yy, SAMPLE* smpl)
{

    memcpy(&GEM_smpl, smpl, sizeof(SAMPLE));
    GEM_freq = (GEM_smpl.s_p1).p_p;
    GEM_nbper = PER_nbper(&(GEM_smpl.s_p1));
    if(GEM_nbper < 0) {
        kerror(0, "Please specify FROM and TO period");
        return(-1);
    }

    return(0);
}

double GEM_read_real(YYFILE* yy)
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

int IMP_vec_gem(YYFILE* yy, char* name, int dim, double* vector)
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
    from = GEM_smpl.s_nb - GEM_nobs;                            /* JMP 16-05-00 */
    if(from < 0) from = 0;                                      /* JMP 16-05-00 */
    for(i = 0; i < from; i++) vector[i] = IODE_NAN;                /* JMP 16-05-00 */
    for(i = from; i < GEM_smpl.s_nb; i++) {     /* JMP 16-05-00 */
        if(i < dim) vector[i] = GEM_mat[GEM_cser + (i - from) *(GEM_nser)];
    }

    GEM_cser++;

    return(0);
}

int IMP_end_gem()
{
    return(0);
}

IMPDEF IMP_GEM = {
    NULL,               // imp_keys
    0,                  // imp_dim
    NULL,               // imp_hd_fn
    IMP_hd_gem,         // imp_hd_sample_fn
    IMP_vec_gem,        // imp_vec_var_fn
    NULL,               // imp_vec_cmt_fn
    NULL,               // imp_elem_fn   
    IMP_end_gem         // imp_end_fn
};


int GEM_readrubr(YYFILE* yy)
{
    YY_lex(yy);
    strcpy(GEM_rubr, (char*) yy->yy_text);
    return(0);
}

int GEM_name(char* name)
{
    if(GEM_cser < 9) sprintf(name, "%s_0%d", GEM_rubr, GEM_cser + 1);
    else             sprintf(name, "%s_%d", GEM_rubr, GEM_cser + 1);
    return(0);
}











