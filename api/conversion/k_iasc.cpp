/**
 *  @header4iode
 * 
 *  Functions to import variables and comments in ASCII format. 
 *  
 *  VAR format (sample is required)
 *  -------------------------------
 *	    sample 1980Y1 1990Y1
 *	    A 1 2 3 na na 4 5
 *	    B 0.1 0.2 -3.14
 *	    .....
 *
 *  CMT format (sample is required)
 *  -------------------------------
 *      A "Description of A" 
 *      B "..."
 *      ...
 *  
 *  List of functions 
 *  -----------------
 *      int read_header(YYFILE* yy, SAMPLE* smpl)                            Reads the sample (required) in an ASCII variable file.
 *      int read_variable(YYFILE* yy, char* name, int dim, double* vector) Reads a VAR name and values in an ASCII variable file format. 
 *      int read_header(ImportFromFile* impdef, char* file, int lang)               Opens an ASCII comment file for reading with the YY library functions. 
 *      int read_comment(char* name, char** cmt)                            Reads one comment on the open YY stream.
 */
#include "api/k_super.h"
#include "api/utils/yy.h"
#include "api/utils/time.h"
#include "api/conversion/import.h"


/**
 *  Reads the sample (required) in an ASCII variable file.
 *  
 *  @param [in, out] YYFILE*    yy   open YY stream 
 *  @param [out]     SAMPLE*    smpl read SAMPLE on the YY stream
 *  @return          int        0 on success, -1 if there is an error in the sample
 */
int ImportObjsASCII::read_header(YYFILE* yy, SAMPLE* smpl)
{
    SAMPLE  *rsmpl;

    if(YY_lex(yy) != ASC_SMPL)  return(-1);

    rsmpl = K_read_smpl(yy);
    if(rsmpl == NULL) return(-1);

    memcpy(smpl, rsmpl, sizeof(SAMPLE));
    SW_nfree(rsmpl);
    return(0);
}

/**
 *  Reads a VAR name and values in an ASCII variable file format. 
 *       name val1 val2... 
 *  where val<n> is a double or the string na for IODE_NAN values.
 *   
 *  @param [in, out]    YYFILE*     yy      open YY stream 
 *  @param [out]        char*       name    name of the variable (read on yy)
 *  @param [in]         int         dim     nb of elements to read (=sample size)
 *  @param [in, out]    double*  vector  read values (IODE_NAN for na values)
 *  @return             int                 0 on success, -1 if EOF is reached before the VAR name
 */
int ImportObjsASCII::read_variable(YYFILE* yy, char* name, int dim, double* vector)
{
    int i;

    while(1) {
        switch(YY_lex(yy))  {
            case YY_EOF :
                return(-1);
            case YY_WORD :
                if(strcmp((char*) yy->yy_text, "na") == 0) continue;

                SCR_strlcpy((unsigned char*) name, yy->yy_text, 79); /* JMP 13-02-2013 */
                name[80] = '\0';
                for(i = 0; i < dim; i++) vector[i] = K_read_real(yy);
                return(0);
        }
    }
} 


// Ascii comments 

/**
 *  Opens an ASCII comment file for reading with the YY library functions. 
 *  
 *  @param [in] ImportCmtFromFile* impdef  struct containing the fn pointers to interpret the content of the ascii file (see iode.h)
 *  @param [in] char*   file       input filename
 *  @param [in] int     lang       language (unused)
 *  
 *  @return     int             0 on success, -1 on error
 */
int ImportCommentsASCII::read_header(ImportCmtFromFile* impdef, char* file, int lang)
{
    SCR_strip((unsigned char*) file);
    AYY = YY_open(file, impdef->imp_keys, impdef->imp_dim, YY_FILE);

    if(AYY == 0) {
        kerror(0,"Cannot open '%s'", file);
        return(-1);
    }

    return(0);
}

/**
 *  Reads one comment on the open YY stream.
 *  
 *  @param [out] char*       name   read name of the comment
 *  @param [out] char**      cmt    allocated read comment
 *  @globals     YYFILE*     AYY    YY stream (opened by read_header())
 *  @return      int                0 on success, -1 on error   
 */
int ImportCommentsASCII::read_comment(char* name, char** cmt)
{
    YYFILE  *yy = AYY;
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
