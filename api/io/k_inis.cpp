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
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/io/import.h"

#undef min
#undef max
#include <algorithm>    // for std::min, std::max


int ImportObjsNIS::read_header(YYFILE* yy, Sample* smpl)
{
    return(0);
}

int ImportObjsNIS::read_variable(YYFILE* yy, char* name, int dim, double* vector)
{
    char    line[85], value[11], *ptr;
    int     i, lg, nb;

    nb = std::min(6, dim);
    if(YY_read_to_char(yy, '\n') == YY_EOF) return(-1);

    lg = (int)strlen((char*) yy->yy_text);

    strcpy(line, (char*) yy->yy_text);
    SCR_strlcpy((unsigned char*) name, (unsigned char*) line, 4);                 /* JMP 13-02-2013 */
    SCR_strlcpy((unsigned char*) name + 4, (unsigned char*) line + 5, 3);         /* JMP 13-02-2013 */
    name[7] = 0;

    ptr = line + 15;
    lg -= 15;
    for(i = 0; i < nb && lg > 0; i++) {
        SCR_strlcpy((unsigned char*) value, (unsigned char*) ptr, 10);            /* JMP 13-02-2013 */
        value[10] = 0;
        SCR_sqz((unsigned char*) value);
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
