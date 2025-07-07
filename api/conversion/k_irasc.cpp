/**
 *  @header4iode
 * 
 *  Functions to import variables in rotated ASCII format.
 *  
 *  VAR format (sample is required)
 *  -------------------------------
 *  	sample 1980Y1 1990Y1
 *  	A B C
 *  	1 2 3
 *      4.2 na 5.4
 *  	na na 4 
 *  	.....
 *  
 *  List of functions 
 *  -----------------
 *      int IMP_hd_rasc(YYFILE* yy, SAMPLE* smpl)                               Reads the sample (required) and the list of VARs in a rotated ASCII variable file.
 *      int IMP_elem_rasc(YYFILE* yy, char* name, int* shift, double* value) Reads one value in an ASCII variable file format. 
 *      int IMP_end_rasc()                                                      Frees the allocated vars during the rotated ASCII file import session.
 */
#include "api/utils/yy.h"
#include "api/utils/time.h"
#include "api/conversion/import.h"


YYKEYS IMP_RASC_KEYS[] = {
    (unsigned char*) "sample",   ASC_SMPL
};

char    **RASC_toc = NULL;  // List of VARs in the rotated ASCII file (following the sample defn).
int     RASC_cv = 0;        // current variable
int     RASC_cy = 0;        // current period

/**
 *  Reads the sample (required) and the list of VARs in a rotated ASCII variable file.
 *  Stores the list of VAR names (to be read) in RASC_toc.
 *    
 *  @param [in, out] YYFILE*    yy   open YY stream 
 *  @param [out]     SAMPLE*    smpl read SAMPLE on the YY stream
 *  @return          int        0 on success, -1 if there is an error in the sample
 */

int IMP_hd_rasc(YYFILE* yy, SAMPLE* smpl)
{
    int     done = 0, nbtoc = 0;
    SAMPLE  *rsmpl;

    if(YY_lex(yy) != ASC_SMPL)  return(-1);

    rsmpl = K_read_smpl(yy);
    if(rsmpl == NULL) return(-1);
    memcpy(smpl, rsmpl, sizeof(SAMPLE));

    while(!done) {
        switch(YY_lex(yy)) {
            case YY_EOF :
                SCR_free_tbl((unsigned char**) RASC_toc);
                RASC_toc = 0;
                return(-1);
            case YY_WORD :
                SCR_add_ptr((unsigned char***) &RASC_toc, &nbtoc, yy->yy_text);
                break;
            default :
                SCR_add_ptr((unsigned char***) &RASC_toc, &nbtoc, NULL);
                YY_unread(yy);
                done = 1;
                break;
        }
    }

    SW_nfree(rsmpl);
    return(0);
}


/**
 *  Reads one value in an ASCII variable file format. 
 *      RASC_cv gives the position of the current VAR in RASC_toc table of names.
 *      RASC_cy gives the position of the current period 
 *  
 *  These 2 vars are adapted at each call.
 *   
 *  @param [in, out]    YYFILE*     yy      open YY stream 
 *  @param [out]        char*       name    name of the variable (= RASC_toc[RASC_cv])
 *  @param [out]        int*        shift   read period position 
 *  @param [in, out]    double*  value   read value (IODE_NAN for na values)
 *  @return             int                 0 on success, -1 if EOF is reached before the first value
 */
 
int IMP_elem_rasc(YYFILE* yy, char* name, int* shift, double* value)
{
    if(YY_lex(yy) == YY_EOF) return(-1);
    else YY_unread(yy);

    if(RASC_cv >= SCR_tbl_size((unsigned char**) RASC_toc)) {
        RASC_cv = 0;
        RASC_cy ++;
        *shift = RASC_cy;
    }
    strcpy(name, RASC_toc[RASC_cv]);
    *value = K_read_real(yy);
    RASC_cv ++;

    return(0);
}


/**
 *  Frees the allocated vars during the rotated ASCII file import session.
 *  
 *  @return     int     O always
 *  
 */
int IMP_end_rasc()
{
    SCR_free_tbl((unsigned char**) RASC_toc);
    RASC_toc = 0;
    RASC_cv = RASC_cy = 0;
    return(0);
}

IMPDEF IMP_RASC = {
    IMP_RASC_KEYS,      // imp_keys
    1,                  // imp_dim
    NULL,               // imp_hd_fn
    IMP_hd_rasc,        // imp_hd_sample_fn
    NULL,               // imp_vec_var_fn
    NULL,               // imp_vec_cmt_fn
    IMP_elem_rasc,      // imp_elem_fn
    IMP_end_rasc        // imp_end_fn
};
