/**
 *  @header4iode
 * 
 *  Functions to import variables from a DIF formatted ASCII file (obsolete). 
 *  See also k_edif.c for an example of DIF format.
 *  
 *  The DIF format, when viewed in Excel (originally in Lotus 123), is as follows:
 *  
 *  VAR format
 *  ----------
 *	    CODE 1980Y 1981Y1 1982Y1 1983Y1 ....
 *	    A    1     1       2      4
 *	    B    1     1       2      5
 *  
 *  List of functions 
 *  -----------------
 *      int dif_skip_to(YYFILE* yy, int skey)                               Moves forward to the keyword skey or EOF or EOD.
 *      int dif_read_cell(YYFILE* yy, char** str, double* value)                 Reads the next cell which can contain a real or a string.
 *      int read_header(YYFILE* yy, SAMPLE* smpl)                            Reads the header of a DIF file and determines the sample of the content.
 *      int read_variable(YYFILE* yy, char* name, int dim, double* vector) Reads a VAR name and values in a DIF file.
 *      int close()                                                   Cleanup the DIF global variables.
 *
 */
#include "api/utils/yy.h"
#include "api/utils/time.h"
#include "api/conversion/dif.h"
#include "api/conversion/import.h"


/**
 *  Moves forward on the yy stream to the keyword skey or EOF or EOD.
 *  
 *  @param [in] YYFILE* yy      Open YY stream     
 *  @param [in] int     skey    keyword (defined in ImportObjsDIF_KEYS)
 *  @return     int             0 if keyword found, -1 if EOF or EOD reached 
 */
int dif_skip_to(YYFILE* yy, int skey)
{
    int rkey;

    while(1) {
        rkey = YY_lex(yy);
        if(rkey == YY_EOF || rkey == DIF_EOD) return(-1);
        if(rkey == skey) break;
    }
    return(0);
}


/**
 *  Reads the next cell which can contain a real or a string.
 *  
 *  @param [in, out]    YYFILE* yy      open YY stream
 *  @param [out]        char**  str     allocated string with the cell content (if string)
 *  @param [out]        double* value   pointer to the read double (if teh cell contains a number)
 *  @return             int             0 if a value is read, -1 if EOF or EOD is reached
 */
int dif_read_cell(YYFILE* yy, char** str, double* value)
{
    int     type, rtype;

    if(str == NULL) {
        if(value == NULL) return(-1);
        else type = 0;
    }
    else type = 1;

    rtype = K_read_long(yy);

    switch(rtype) {
        case 0  :
            if(dif_skip_to(yy, DIF_COMMA) < 0) return(-1);
            if(rtype == type)
                *value = (double) K_read_real(yy);
            else {
                YY_lex(yy);
                *str = (char*) SCR_stracpy(yy->yy_text);
                if(*str == NULL) return(-1);
            }
            YY_lex(yy);
            break;

        case 1  :
            if(dif_skip_to(yy, DIF_COMMA) < 0) return(-1);
            K_read_long(yy);
            *str = K_read_str(yy);
            if(*str == NULL) return(-1);
            break;

        case -1 :
            if(dif_skip_to(yy, DIF_BOT) < 0) return(-1);
            break;
        default :
            return(-1);
    }

    return(0);
}


/**
 *  Reads the header of a DIF file and determines the sample of the content.
 *  
 *  @param [in, out] YYFILE* yy      open YY stream
 *  @param [out]     SAMPLE* smpl    sample of the DIF file
 *  @return          int             0 if a value is read, -1 if EOF or EOD is reached
 */
int ImportObjsDIF::read_header(YYFILE* yy, SAMPLE* smpl)
{
    double  value;
    char    *str;
    int     i;
    PERIOD  per1, per2, *per;

    if(YY_lex(yy) != DIF_TABLE) return(-1);

    if(dif_skip_to(yy, DIF_VECTORS) < 0) return(-1);
    if(dif_read_cell(yy, NULL, &value) < 0) return(-1) ;
    DIF_ny = (int)value - 1;

    if(dif_skip_to(yy, DIF_TUPLES) < 0) return(-1);
    if(dif_read_cell(yy, NULL, &value) < 0)  return(-1) ;
    DIF_nl = (int)value - 1;

    if(dif_skip_to(yy, DIF_BOT) < 0) return(-1);

    dif_read_cell(yy, &str, NULL);
    SW_nfree(str);
    for(i = 0; i < DIF_ny; i++) {
        dif_read_cell(yy, &str, NULL);
        per = PER_atoper(str); /* no valid samples defined, take input sample */
        if(per == NULL) goto done;
        SW_nfree(str);

        if(i == 0) {
            memcpy(&per1, per, sizeof(PERIOD));
            memcpy(&per2, per, sizeof(PERIOD));
            DIF_freq = per->p_p;
            continue;
        }

        if(per->p_p != DIF_freq) {
            SW_nfree(per);
            return(-1);
        }

        if(PER_diff_per(per, &per1) < 0)
            memcpy(&per1, per, sizeof(PERIOD));

        if(PER_diff_per(&per2, per) < 0)
            memcpy(&per2, per, sizeof(PERIOD));

        SW_nfree(per);
    }

    DIF_smpl = PER_pertosmpl(&per1, &per2);
    if(DIF_smpl != NULL) memcpy(smpl, DIF_smpl, sizeof(SAMPLE));

done :

    if(DIF_ny != smpl->s_nb) return(-1);
    if(dif_skip_to(yy, DIF_BOT) < 0) return(-1);
    return(0);
}


/**
 *  Reads a VAR name and values in a DIF file.
 *  
 *  @param [in, out] YYFILE*    yy      open YY stream
 *  @param [out]     char*      name    name of the series
 *  @param [in]      int        dim     nb of values to read
 *  @param [out]     double* vector  pointer to the values
 *  @return          int             0 if a all values have been read, -1 if EOF or EOD is reached before
 */
int ImportObjsDIF::read_variable(YYFILE* yy, char* name, int dim, double* vector)
{
    int     i, rc = 0;
    char    *str = NULL;
    double  value;

    rc = dif_read_cell(yy, &str, NULL);
    if(rc < 0 || str == NULL) return(-1);

    SCR_strlcpy((unsigned char*) name, (unsigned char*) str, 79); /* JMP 13-02-2013 */
    SW_nfree(str);

    for(i = 0; i < dim; i++) {
        rc = dif_read_cell(yy, NULL, &value);
        if(rc < 0) return(-1);
        vector[i] = (double) value;
    }

    dif_skip_to(yy, DIF_BOT);
    return(0);
}

/**
 *  Cleanup the DIF global variables.
 *  
 *  @return int     0 always
 */
int ImportObjsDIF::close()
{
    SW_nfree(DIF_smpl);
    DIF_smpl = 0;
    return(0);
}
