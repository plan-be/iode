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
#include "api/io/import.h"


double ImportObjsPRN::read_real(YYFILE* yy)
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

int ImportObjsPRN::read_variable(YYFILE* yy, char* name, int dim, double* vector)
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

                for(i = 0; i < dim; i++) vector[i] = read_real(yy);
                return(0);
        }
    }
}

// COMMENTS

int ImportCommentsPRN::read_header(ImportCmtFromFile* impdef, char* file, int lang)
{
    SCR_strip((unsigned char*) file);
    PYY = YY_open(file, impdef->imp_keys, impdef->imp_dim, YY_FILE);

    if(PYY == 0) {
        kerror(0,"Cannot open '%s'", file);
        return(-1);
    }

    return(0);
}

int ImportCommentsPRN::read_comment(char* name, char** cmt)
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
