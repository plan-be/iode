/**
 * @header4iode
 * 
 * Functions to load and save ascii and csv definitions of IODE VAR objects.
 *
 *      - KDB *load_asc(char *filename)
 *      - save_asc(KDB* kdb, char* filename)
 *      - int save_csv(KDB *kdb, char *filename, Sample *smpl, char **varlist)
 */
#include "scr4.h"

#include "api/b_errors.h"
#include "api/k_super.h"
#include "api/objs/objs.h"
#include "api/objs/variables.h"
#include "api/ascii/ascii.h"


char* AsciiVariables::CSV_SEP = 0;
char* AsciiVariables::CSV_DEC = 0;
char* AsciiVariables::CSV_NAN = 0;
char* AsciiVariables::CSV_AXES = 0;
int   AsciiVariables::CSV_NBDEC = 15;

/**
 *  Reads one series on the stream YY. Subfunction of load_yy().
 
 *  The series is read until the next token is neither a number nor the token "na" (not a number).
 *  The missing values till the end of the sample are fixed to na.
 *  
 *  @param [in, out] kdb    KDB*    KDB of vars
 *  @param [in, out] yy     YY*     open stream 
 *  @param [in]      name   char*   name of the variable that will be read .
 *  @return                 int     0 on success, -1 if the var <name> cannot be created.
 *  
 */
static int read_vec(KDB* kdb, YYFILE* yy, char* name)
{
    bool    success;
    int     i, keyw, nb;
    double  *vec;
    Sample  *smpl;

    smpl = kdb->sample;
    if(!smpl) 
    {
        kerror(0, "%s : undefined sample", YY_error(yy));
        return(-1);
    }

    vec = (double *) SW_nalloc(smpl->nb_periods * sizeof(double));

    /* READ AT MOST nobs OBSERVATIONS */
    for(i = 0; i < smpl->nb_periods; i++)  
        vec[i] = K_read_real(yy);

    /* CONTINUE READING UNTIL END OF VALUES */
    while(1) 
    {
        keyw = YY_lex(yy);
        if((keyw == YY_WORD && strcmp((char*) yy->yy_text, "na")) || keyw == YY_EOF) 
            break;
    }
    YY_unread(yy);

    nb = smpl->nb_periods;
    success = K_add(kdb, name, vec, &nb);
    if(!success) 
    {
        kerror(0, "%s : unable to create %s", YY_error(yy), name);
        SW_nfree(vec);
        return(-1);
    }

    SW_nfree(vec);
    return(0);
}

/**
 *  Subfn of load_asc().
 *  Reads ascii definitions of IODE variables from an opened YY stream and returns a new KDB*. 
 *  
 *  
 *  The YY stream must be opened and can be a file or a string.
 *  
 *  @see http://www.xon.be/scr4/libs1/libs157.htm for more information on YY.
 *    
 *  @param [in, out]    yy         YY*     YY stream to read data from
 *  @param [in]         ask        int     see load_asc_type_ask() 
 *  @param [in]         db_global  int     1 for DB_GLOBAL, 0 for DB_STANDALONE
 *  @return                        KDB*    a new KDB of IODE vars or NULL on error
 *  
 */
static KDB* load_yy(YYFILE* yy, int ask, int db_global)
{
    int     cmpt = 0;
    ONAME   name;
    Sample* smpl;
    KDB*    kdb = new KDB(VARIABLES, (db_global == 1) ? DB_GLOBAL : DB_STANDALONE);

    // The keyword sample must be the first on the YY stream */
    // if not:
    //    - if ask == 1: the user must provide the sample via a call to kas->sample
    //    - else:        the function returns NULL
    if(YY_lex(yy) != SMPL) 
    {
        if(!ask) 
        {
            kerror(0,"%s Expected sample definition", YY_error(yy));
            goto err;
        }
        else 
        {
            YY_unread(yy);
            smpl = kasksmpl();
        }
    }
    else 
        smpl = K_read_smpl(yy);

    if(!smpl) 
        goto err;
    
    kdb->sample = new Sample(*smpl);

    /* Loop on var definition 
        NAME1 value ... NAME2 ...
    */
    while(1) 
    {
        switch(YY_lex(yy)) 
        {
            case YY_EOF :
                delete smpl;
                return kdb;

            case YY_WORD :
                if(smpl->nb_periods == 0) 
                {
                    kerror(0, "%s : undefined sample", YY_error(yy));
                    goto err;
                }
                yy->yy_text[K_MAX_NAME] = 0;
                strcpy(name, (char*) yy->yy_text);
                read_vec(kdb, yy, name);
                kmsg("Reading object %d : %s", ++cmpt, name);
                break;

            default :
                kerror(0, "Incorrect entry : %s", YY_error(yy));
                break;
        }
    }

err:
    if(smpl) delete smpl;
    if(kdb) delete kdb;
    return nullptr;
}

/**
 *  Loads variables from an ASCII file or from a string.
 *  
 *  @param [in] file_or_string char*  file or string to interpret
 *  @param [in] type        int    1 => file_or_string is a string containing the var definitions, 
 *                                 0 => file_or_string is a file name 
 *  @param [in] ask         int    1 => if and the sample definition is not present on the YY stream, calls kas->sample to get the sample from the user.
 *                                 0 => if and the sample definition is not present on the YY stream, quit the function and returns NULL
 *  @param [in] db_global    int   1 for DB_GLOBAL, 0 for DB_STANDALONE
 *  @return                  KDB*  NULL or new KDB of variables
 *  
 */
KDB* AsciiVariables::load_asc_type_ask(char *file_or_string, int type, int ask, int db_global)
{
    static  int sorted;
    YYFILE  *yy;
    int		yytype;

    if(type == 0) 
    {
        SCR_strip((unsigned char *) file_or_string);
        yytype = (!K_ISFILE(file_or_string)) ? YY_STDIN : YY_FILE;
    }
    else
        yytype = YY_MEM;

    /* INIT YY READ */
    YY_CASE_SENSITIVE = 1;

    if(sorted == 0) 
    {
        qsort(TABLE, sizeof(TABLE) / sizeof(YYKEYS), sizeof(YYKEYS), ascii_compare);
        sorted = 1;
    }

    yy = YY_open(file_or_string, TABLE, sizeof(TABLE) / sizeof(YYKEYS), yytype);

    if(yy == 0) 
    {
        if(!type) 
            kerror(0,"Cannot open '%s'", file_or_string);
        else      
            kerror(0,"Cannot open string");
        return nullptr;
    }

    KDB* kdb = load_yy(yy, ask, db_global);
    YY_close(yy);
    return kdb;
}

/**
 *  Loads variables from an ASCII file into a new KDB. On success, adds extension to ".av" to 
 *  filename in kd->filepath.
 *  
 *  @param [in] filename    char* ascii file to read
 *  @param [in] db_global   int   1 for DB_GLOBAL, 0 for DB_STANDALONE
 *  @return                 KDB*  NULL or new KDB of variables
 *  
 */
KDB* AsciiVariables::load_asc(char *filename, int db_global)
{   
    KDB* kdb = load_asc_type_ask(filename, 0, 0, db_global);
    if(kdb == nullptr || kdb->size() == 0)
        return nullptr;
    
    char asc_filename[1024];
    K_set_ext_asc(asc_filename, filename, VARIABLES);
    K_set_kdb_fullpath(kdb, (U_ch*)asc_filename); // JMP 03/12/2022
   
    return kdb;
}

/**
 *  Prints the representation of one value on fd. For NaN value, prints "na". 
 *  
 *  @param [in, out]    fd      FILE *      output stream    
 *  @param [in]         val     double   value to print  
 *  @return 
 *  
 */
static void print_val(FILE* fd, double val)
{
    if(IODE_IS_A_NUMBER(val))
#ifdef REALD
        fprintf(fd, "%.15lg ", (double)(val)); /* JMP 09-04-98 */
#else
        fprintf(fd, "%.8lg ", (double)(val)); /* JMP 09-04-98 */
#endif
    else
        fprintf(fd, "na ");
}

/**
 *  Saves a KDB of vars into a ascii file (.av).
 *  
 *  @param [in] kdb         KDB*    KDB of vars to save in ascii.
 *  @param [in] filename    char*   name of the output file or "-" to write the result to the stdout.
 *  @return                 int     0 on success, -1 if the file cannot be written.
 *  
 */
int AsciiVariables::save_asc(KDB* kdb, char* filename)
{
    FILE    *fd;
    int     j;
    double  *val;
    Sample  *smpl;

    if(filename[0] == '-') fd = stdout;
    else 
    {
        fd = fopen(filename, "w+");
        if(fd == 0) 
        {
            kerror(0, "Cannot create '%s'", filename);
            return(-1);
        }
    }

    smpl = kdb->sample;
    if(!smpl) 
    {
        kwarning("Cannot save the Variables to an ascii file -> sample is empty");
        if(filename[0] != '-') fclose(fd);
        return -1;
    }
    fprintf(fd, "sample %s ", (char*) smpl->start_period.to_string().c_str());
    fprintf(fd, "%s\n", (char*) smpl->end_period.to_string().c_str());

    for(auto& [name, _] : kdb->k_objs) 
    {
        fprintf(fd, "%s ", name.c_str());
        val = KVVAL(kdb, name, 0);
        for(j = 0 ; j < smpl->nb_periods; j++, val++) 
            print_val(fd, *val);
        fprintf(fd, "\n");
    }

    if(filename[0] != '-') 
        fclose(fd);
    
    return(0);
}

/* --------------- CSV Files -------------------- */

/**
 *  Saves a KDB of variables in a csv file à la LArray.
 *  
 *  The following global variables can be modified to customize the output :
 *  
 *    - char *CSV_SEP = 0;       // cell separator     (default ",")
 *    - char *CSV_DEC = 0;       // decimal separator (default ".")
 *    - char *CSV_NAN = 0;       // NaN representation (default "")
 *    - char *CSV_AXES = 0;      // name of the axis for the variable name (default "var") or NULL for all variables
 *    - int  CSV_NBDEC = 15;     // number of decimals
 *  
 *  
 *  @param [in] kdb      KDB*       KDB of vars to save
 *  @param [in] filename char*      output file or "-" for stdout 
 *  @param [in] smpl     Sample*    sample to save
 *  @param [in] varlist  char**     list of variables to save in the file
 *  @return 
 *  
 */
int AsciiVariables::save_csv(KDB *kdb, char *filename, Sample *smpl, char **varlist)
{
    FILE        *fd;
    int         i, j, nb;
    char        fmt[80], buf[256], *sep, *dec, *nan, *axes, **lst;
    double      *val;

    // Parameters : sep, dec, nbdec, nan,
    sep = this->CSV_SEP;
    dec = this->CSV_DEC;
    nan = this->CSV_NAN;
    if(sep == 0 || sep[0] == 0) {
        sep = ",";
        sep[0] = SCR_sLIST; // JMP 27/1/2017
    }
    if(dec == 0 || dec[0] == 0) {
        dec = ".";
        dec[0] = SCR_sDECIMAL; // JMP 27/1/2017
    }
    if(this->CSV_NBDEC < 0 || this->CSV_NBDEC > 15)
        strcpy(fmt, "%lg");
    else
        sprintf(fmt, "%%.%dlg", this->CSV_NBDEC);
    if(nan == 0) nan = "";

    axes = this->CSV_AXES;
    if(axes == 0 || axes[0] == 0) axes = "var";

    // sample : if CSV_Sample == NULL => tout le sample
    if(!smpl)
        smpl = kdb->sample;
    
    if(!smpl) 
    {
        kwarning("Cannot save the Variables to a CSV file -> sample is empty");
        return -1;
    }

    // Liste
    lst = varlist;
    if(varlist == 0) 
    {
        lst = 0;
        nb = 0;
        for(i = 0; i < kdb->size(); i++)
            SCR_add_ptr((unsigned char ***) &lst, &nb, (unsigned char*) kdb->get_name(i).c_str());
        SCR_add_ptr((unsigned char ***) &lst, &nb, 0L);
    }

    // Open file
    if(filename[0] == '-') fd = stdout;
    else 
    {
        fd = fopen(filename, "w+");
        if(fd == 0) 
        {
            kerror(0, "Cannot create '%s'", filename);
            return(-1);
        }
    }

    // Ligne 1
    fprintf(fd, "%s\\time", axes);
    for(i = 0 ; i < smpl->nb_periods ; i++) 
    {
        if(smpl->start_period.periodicity == 'Y')
            fprintf(fd, "%s%d", sep, smpl->start_period.year + i);
        else 
        {
            Period period = smpl->start_period.shift(i);
            fprintf(fd, "%s%s", sep, (char*) period.to_string().c_str());
        }
    }
    fprintf(fd, "\n");

    // next lines
    std::string var_name;
    for(i = 0; lst[i]; i++) 
    {
        SCR_upper((unsigned char *) lst[i]);
        fprintf(fd, "%s", lst[i]);
        var_name = std::string(lst[i]);
        if(kdb->contains(var_name)) 
        {
            val = KVVAL(kdb, var_name, 0);
            for(j = 0; j < smpl->nb_periods; j++, val++) 
            {
                if(IODE_IS_A_NUMBER(*val)) 
                {
                    sprintf(buf, fmt, (double)(*val));
                    if(dec[0] != '.') SCR_replace((unsigned char *) buf, (unsigned char *) ".", (unsigned char *) dec);
                }
                else 
                    strcpy(buf, nan);
                fprintf(fd, "%s%s", sep, buf);
            }
        }
        fprintf(fd, "\n");
    }
    if(varlist == 0) 
        SCR_free_tbl((unsigned char **) lst);
    if(filename[0] != '-') 
        fclose(fd);
    
    return 0;
}
