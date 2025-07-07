/**
 *  @header4iode
 * 
 *  Functions to import variables from the legacy PRN format (obsolete). 
 *  
 *  PRN VAR format
 *  --------------
 *  	"Name" 1960 1961 ... 1990
 *  	"A" 1 2 3 ... 4 5
 *  	"B" 0.1 0.2 ... -3.14
 *      ...
 *  
 *  PRN CMT format
 *  --------------
 *  	"A" "Comment of A"
 *  	"B" "Comment of B"
 *      ...
 *
 *  List of functions 
 *  -----------------
 *  See k_idif.c for a similar group of functions.
 */
#include "api/constants.h"
#include "api/k_super.h"
#include "api/conversion/import.h"


double PRN_readreal(YYFILE* yy)
{
    double    val;
    int     minus = 1;

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

        case YY_STRING :
        case YY_WORD   :
            if(strcmp("NA", (char*) yy->yy_text) != 0)
                YY_unread(yy);
            return(IODE_NAN);
        default :
            YY_unread(yy);
            return(IODE_NAN);
    }

    return(val);
}

int IMP_vec_prn(YYFILE* yy, char* name, int dim, double* vector)
{
    int i, key;

    while(1) {
        key = YY_lex(yy);
        switch(key)  {
            case YY_EOF :
                return(-1);
            case YY_WORD :
            case YY_STRING :
                if(strcmp((char*) yy->yy_text, "na") == 0) continue;

                SCR_strlcpy((unsigned char*) name, yy->yy_text, 79); /* JMP 13-02-2013 */
                name[80] = '\0';
                if(key == YY_STRING) SCR_asqz((unsigned char*) name, (unsigned char*) "_");

                for(i = 0; i < dim; i++) vector[i] = PRN_readreal(yy);
                return(0);
        }
    }
}

IMPDEF IMP_PRN = {
    NULL,           // imp_keys
    0,              // imp_dim
    NULL,           // imp_hd_fn
    NULL,           // imp_hd_sample_fn
    IMP_vec_prn,    // imp_vec_var_fn
    NULL,           // imp_vec_cmt_fn
    NULL            // imp_elem_fn
};

// COMMENTS

YYFILE  *PYY;

int IMP_hd_cprn(IMPDEF* impdef, char* file, int lang)
{
    SCR_strip((unsigned char*) file);
    PYY = YY_open(file, impdef->imp_keys, impdef->imp_dim, YY_FILE);

    if(PYY == 0) {
        kerror(0,"Cannot open '%s'", file);
        return(-1);
    }

    return(0);
}

int IMP_vec_cprn(char* name, char** cmt)
{
    YYFILE  *yy = PYY;
    int     key;

    key = YY_lex(yy);
    if(key == YY_EOF) goto err;

    if(key == YY_WORD || key == YY_STRING) {
        SCR_strlcpy((unsigned char*) name, yy->yy_text, 79); /* JMP 13-02-2013 */
        name[80] = '\0';
    }
    else goto err;

    key = YY_lex(yy);
    if(key == YY_EOF) return(-1);

    if(key == YY_WORD || key == YY_STRING)
        *cmt = (char*) SCR_stracpy(yy->yy_text);
    else goto err;

    return(0);

err :
    return(-1);

}

IMPDEF IMP_PRN_CMT = {
    NULL,           // imp_keys
    0,              // imp_dim
    IMP_hd_cprn,    // imp_hd_fn
    NULL,           // imp_hd_sample_fn
    NULL,           // imp_vec_var_fn
    IMP_vec_cprn,   // imp_vec_cmt_fn
    NULL,           // imp_elem_fn
    NULL            // imp_end_fn
};
