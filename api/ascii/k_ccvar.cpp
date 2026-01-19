/**
 * @header4iode
 * 
 * Functions to load and save ascii and csv definitions of IODE VAR objects.
 *
 *      bool load_asc(const std::string& filename)
 *      bool save_asc(const std::string& filename)
 *      bool save_vars_csv(const std::string& filename, const std::vector<std::string>& varlist, Sample* smpl)
 */
#include "scr4.h"

#include "api/b_errors.h"
#include "api/k_super.h"
#include "api/objs/objs.h"
#include "api/objs/variables.h"
#include "api/ascii/ascii.h"


char* KDBVariables::CSV_SEP = 0;
char* KDBVariables::CSV_DEC = 0;
char* KDBVariables::CSV_NAN = 0;
char* KDBVariables::CSV_AXES = 0;
int   KDBVariables::CSV_NBDEC = 15;

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
static int read_vec(KDBVariables* kdb, YYFILE* yy, char* name)
{
    bool    success;
    int     i, keyw;
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

    success = kdb->set_obj(name, vec);
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
static bool load_yy(KDBVariables* kdb, YYFILE* yy, int ask)
{
    int     cmpt = 0;
    ONAME   name;
    Sample* smpl = nullptr;

    if(!kdb)
        return false;

    kdb->clear();  /* clear KDB */

    // The keyword sample must be the first on the YY stream */
    // if not:
    //    - if ask == 1: the user must provide the sample via a call to kas->sample
    //    - else:        the function returns NULL
    if(YY_lex(yy) != SMPL) 
    {
        if(!ask) 
        {
            kwarning("%s Expected sample definition", YY_error(yy));
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
                return true;

            case YY_WORD :
                if(smpl->nb_periods == 0) 
                {
                    kwarning("%s : undefined sample", YY_error(yy));
                    goto err;
                }
                yy->yy_text[K_MAX_NAME] = 0;
                strcpy(name, (char*) yy->yy_text);
                read_vec(kdb, yy, name);
                kmsg("Reading object %d : %s", ++cmpt, name);
                break;

            default :
                kwarning("Incorrect entry : %s", YY_error(yy));
                break;
        }
    }

err:
    if(smpl) delete smpl;
    if(kdb) kdb->clear();
    return false;
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
bool KDBVariables::load_asc_type_ask(const std::string& file_or_string, int type, int ask)
{
    static  int sorted;
    YYFILE  *yy;
    int		yytype;

    char* c_filename = (char*) file_or_string.c_str();
    if(type == 0) 
    {
        yytype = (!K_ISFILE(c_filename)) ? YY_STDIN : YY_FILE;
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

    yy = YY_open(c_filename, TABLE, sizeof(TABLE) / sizeof(YYKEYS), yytype);

    if(yy == 0) 
    {
        if(!type) 
            kwarning("Cannot open '%s'", file_or_string);
        else      
            kwarning("Cannot open string");
        return false;
    }

    bool success = load_yy(this, yy, ask);
    YY_close(yy);
    return success;
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
bool KDBVariables::load_asc(const std::string& filename)
{   
    char asc_filename[1024];

    std::string trim_filename = trim(filename);
    char* c_filename = (char*) trim_filename.c_str();
    K_set_ext_asc(asc_filename, c_filename, VARIABLES);

    bool success = load_asc_type_ask(std::string(asc_filename), 0, 0);

    if(success)
        K_set_kdb_fullpath(this, (U_ch*) asc_filename);

    return success;
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
bool KDBVariables::save_asc(const std::string& filename)
{
    FILE    *fd;
    int     j;
    double  *val;
    Sample  *smpl;

    if(filename[0] == '-') 
        fd = stdout;
    else 
    {
        std::string trim_filename = trim(filename);
        char* c_filename = (char*) trim_filename.c_str();
        fd = fopen(c_filename, "w+");
        if(fd == 0)
        {
            std::string error_msg = "Cannot create '" + trim_filename + "'";
            kwarning(error_msg.c_str());
            return false;
        }
    }

    smpl = this->sample;
    if(!smpl) 
    {
        kwarning("Cannot save the Variables to an ascii file -> sample is empty");
        if(filename[0] != '-')
            fclose(fd);
        return false;
    }
    fprintf(fd, "sample %s ", (char*) smpl->start_period.to_string().c_str());
    fprintf(fd, "%s\n", (char*) smpl->end_period.to_string().c_str());

    for(auto& [name, _] : k_objs) 
    {
        fprintf(fd, "%s ", name.c_str());
        val = this->get_var_ptr(name);
        for(j = 0 ; j < smpl->nb_periods; j++, val++) 
            print_val(fd, *val);
        fprintf(fd, "\n");
    }

    if(filename[0] != '-') 
        fclose(fd);

    return true;
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
bool KDBVariables::save_csv(const std::string& filename, const std::vector<std::string>& varlist, Sample* smpl)
{
    FILE        *fd;
    char        fmt[80], buf[256], *sep, *dec, *nan, *axes;
    double      *val;

    // Parameters : sep, dec, nbdec, nan,
    sep = this->CSV_SEP;
    dec = this->CSV_DEC;
    nan = this->CSV_NAN;
    if(sep == 0 || sep[0] == 0) 
    {
        sep = ",";
        sep[0] = SCR_sLIST; // JMP 27/1/2017
    }
    if(dec == 0 || dec[0] == 0) 
    {
        dec = ".";
        dec[0] = SCR_sDECIMAL; // JMP 27/1/2017
    }
    if(this->CSV_NBDEC < 0 || this->CSV_NBDEC > 15)
        strcpy(fmt, "%lg");
    else
        sprintf(fmt, "%%.%dlg", this->CSV_NBDEC);
    if(nan == 0) 
        nan = "";

    axes = this->CSV_AXES;
    if(axes == 0 || axes[0] == 0) 
        axes = "var";

    // sample : if CSV_Sample == NULL => tout le sample
    if(!smpl)
        smpl = this->sample;
    
    if(!smpl) 
    {
        kwarning("Cannot save the Variables to a CSV file -> sample is empty");
        return false;
    }

    // Open file
    if(filename[0] == '-') 
        fd = stdout;
    else 
    {
        std::string trim_filename = trim(filename);
        char* c_filename = (char*) trim_filename.c_str();
        fd = fopen(c_filename, "w+");
        if(fd == 0)
        {
            std::string error_msg = "Cannot create '" + trim_filename + "'";
            kwarning(error_msg.c_str());
            return false;
        }
    }

    // line 0
    fprintf(fd, "%s\\time", axes);
    for(int i = 0 ; i < smpl->nb_periods ; i++) 
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

    // list of variables
    std::set<std::string> lst;
    if(varlist.size() == 0)
        lst = this->get_names();
    else
    {
        for(const std::string& var_name: varlist) 
            lst.insert(to_upper(var_name));
    }

    // next lines
    for(const std::string& var_name: lst) 
    {
        fprintf(fd, "%s", var_name.c_str());
        if(this->contains(var_name)) 
        {
            val = this->get_var_ptr(var_name);
            for(int j = 0; j < smpl->nb_periods; j++, val++) 
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

    if(filename[0] != '-') 
        fclose(fd);

    return true;
}
