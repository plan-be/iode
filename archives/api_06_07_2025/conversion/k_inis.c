/**
 *  @header4iode
 * 
 *  Functions to import variables from the legacy INS-NIS format (obsolete). 
 *  
 *  NIS VAR format
 *  --------------
 *      0000 XXX 91 1    one   two   three  for   five  six
 *      0-3  6-8 11-15   16-25 27-36 38-47 49-58 60-61 71-80
 *
 *  List of functions 
 *  -----------------
 *  See k_idif.c for a similar group of functions.
 */
#include "api/constants.h"
#include "api/utils/time.h"
#include "api/conversion/import.h"


int IMP_hd_nis(yy, smpl)
YYFILE  *yy;
SAMPLE  *smpl;
{
    return(0);
}


int IMP_vec_nis(yy, name, dim, vector)
YYFILE  *yy;
char    *name;
int     dim;
double    *vector;
{
    char    line[85], value[11], *ptr;
    int     i, lg, nb;

    nb = min(6, dim);
    if(YY_read_to_char(yy, '\n') == YY_EOF) return(-1);

    lg = (int)strlen(yy->yy_text);

    strcpy(line, yy->yy_text);
    SCR_strlcpy(name, line, 4);                 /* JMP 13-02-2013 */
    SCR_strlcpy(name + 4, line + 5, 3);         /* JMP 13-02-2013 */
    name[7] = 0;

    ptr = line + 15;
    lg -= 15;
    for(i = 0; i < nb && lg > 0; i++) {
        SCR_strlcpy(value, ptr, 10);            /* JMP 13-02-2013 */
        value[10] = 0;
        SCR_sqz(value);
        if(value[0] == '\0') vector[i] = IODE_NAN;
        else vector[i] = atof(value);
        ptr += 11;
        lg -= 11;
    }

    while(i < nb) {
        vector[i] = IODE_NAN;
        i++;
    }

    return(0);
}

IMPDEF IMP_NIS = {
    NULL,
    0,
    IMP_hd_nis,
    IMP_vec_nis,
    NULL,
    NULL
};
