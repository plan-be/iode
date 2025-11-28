/**
 * @header4iode
 *
 * Functions to load and save ascii definitions of IODE Scalar objects.
 *
 *      bool load_asc(const std::string& filename)
 *      bool save_asc(const std::string& filename)
 */
#include "api/b_errors.h"
#include "api/k_super.h"
#include "api/objs/objs.h"
#include "api/objs/scalars.h"
#include "api/ascii/ascii.h"


/**
 *  Reads on an open YY stream (file or string) the ascii definition of an IODE Scalar and adds the new Scalar to kdb. 
 *  Subfn of load_asc().
 *  
 *  Reads 3 values on yy. Each token must be a number (long or double). If not : 
 *      the first value is replaced by 0.9 (value of the scalar)
 *      the second value (relax) is replaced by 1.0
 *      the 3d value (stderr) is replaced by IODE_NAN
 *  
 *  @param [in, out ]   kdb     KDB*    KDB of Scalars
 *  @param [in, out]    yy      YYFILE* stream (file or string) to be read 
 *  @param [in]         name    char*   Scalar name
 *  @return                     int     0 if the Scalar is read and saved, -1 if it can't be created.
 *  
 */
static int read_scl(KDB* kdb, YYFILE* yy, char* name)
{
    bool     success;
    int      keyw;
    Scalar   scl;

    /* READ AT MOST 3 REALS */
    scl.value = K_read_real(yy);
    if(!IODE_IS_A_NUMBER(scl.value)) 
        scl.value = 0.9;
    scl.relax = K_read_real(yy);
    if(!IODE_IS_A_NUMBER(scl.relax)) 
        scl.relax = 1.0;
    scl.std = K_read_real(yy);

    /* CONTINUE READING UNTIL END OF VALUES */
    while(1) 
    {
        keyw = YY_lex(yy);
        if(keyw == YY_WORD || keyw == YY_EOF) 
            break;
    }
    YY_unread(yy);

    success = kdb->set(name, (char*) &scl);
    if(!success) 
    {
        kerror(0, "%s : unable to create %s", YY_error(yy), name);
        return(-1);
    }

    return(0);
}

/**
 *  Loads Scalars from an ASCII file and saves the Scalars into a new KDB.
 *  
 *  Syntax of Scalar ascii definitions 
 *  -------------------------------
 *      sclname [value [relax [stderr]]]
 *  
 *  Example
 *  -------
 *      acaf1 0.0157684069126844 1 0.00136871379800141 
 *      acaf2 -7.9615019785706e-06 1 1.48283402268135e-06 
 *      acaf3 2.50255966186523 1 0.873010456562042 
 *      acaf4 -0.00850518420338631 1 0.00208330410532653 
 *      dlnpaf 
 *      dpuh_1 0.0182728692889214 
 *  
 *  Error messages are sent to the function kerror().
 *  For each read CMT, kmsg() is called to send a message to the user. 
 *  
 *  The implementations of kerror() and kmsg() depend on the context.
 *  
 *  @param [in] filename     char*   name of the ascii file to be read or 
 *                                   string containing the definition of the Scalars
 *  @param [in]   db_global  int     1 for DB_GLOBAL, 0 for DB_STANDALONE
 *  @return                  KDB*    new KDB of Scalar or NULL on error
 *  
 *  TODO: what if read_cmt returns an error code ?
 *  
 */
bool KDB::load_asc_scl(const std::string& filename)
{
    int     cmpt = 0;
    YYFILE  *yy;
    ONAME   name;

    /* INIT YY READ */
    YY_CASE_SENSITIVE = 1;

    std::string trim_filename = trim(filename);
    char* c_filename = (char*) trim_filename.c_str();
    yy = YY_open(c_filename, NULL, 0, (!K_ISFILE(c_filename)) ? YY_STDIN : YY_FILE);

    if(yy == 0)
    {
        std::string error_msg = "Cannot open file '" + trim_filename + "'";
        kwarning(error_msg.c_str());
        return false;
    }

    clear();  /* clear current KDB */

    /* READ FILE */ 
    K_set_kdb_fullpath(this, (U_ch*) c_filename);
    while(1) 
    {
        switch(YY_lex(yy)) 
        {
            case YY_EOF :
                if(cmpt) 
                {
                    char asc_filename[1024];
                    K_set_ext_asc(asc_filename, c_filename, SCALARS);
                    K_set_kdb_fullpath(this, (U_ch*) asc_filename);
                }            
                YY_close(yy);
                return true;

            case YY_WORD :
                yy->yy_text[K_MAX_NAME] = 0;
                strcpy(name, (char*) yy->yy_text);
                if(read_scl(this, yy, name) == 0) 
                    cmpt++;
                kmsg("Reading object %d : %s", cmpt, name);
                break;

            default :
                kwarning("Incorrect entry : %s", YY_error(yy));
                break;
        }
    }

    YY_close(yy);
    return false;
}

/**
 *  Writes the definition of a Scalar to a file. IODE_NAN are represented by na.
 *  
 *  @param [in] fd  pointer to a FILE
 *  @param [in] scl pointer to the Scalar
 *  
 */
static void print_scl(FILE* fd, Scalar* scl)
{
    if(IODE_IS_A_NUMBER(scl->value)) fprintf(fd, "%.14lg ", (double)(scl->value)); /* JMP 06/10/2022 */ 
    else fprintf(fd, "na ");

    if(IODE_IS_A_NUMBER(scl->relax)) fprintf(fd, "%.14lg ", (double)(scl->relax)); /* JMP 06/10/2022 */
    else fprintf(fd, "na ");

    if(IODE_IS_A_NUMBER(scl->std)) fprintf(fd, "%.14lg ", (double)(scl->std));     /* JMP 06/10/2022 */
    else fprintf(fd, "na ");
}

/**
 *  Saves a KDB of Scalars into an ascii file (.as) or to the stdout.
 *  
 *  @see load_asc() for the syntax. 
 *  
 *  @param [in] kdb         KDB*    KDB of Scalars
 *  @param [in] filename    char*   name of the output file or "-" to write the result on the stdout.
 *  @return                 int     0 on success, -1 if the file cannot be written.
 *  
 */
bool KDB::save_asc_scl(const std::string& filename)
{
    FILE*    fd;
    Scalar*  scl;

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

    for(auto& [name, handle] : this->k_objs) 
    {
        fprintf(fd, "%s ", (char*) name.c_str());
        scl = KSVAL(this, handle);
        print_scl(fd, scl);
        fprintf(fd, "\n");
    }

    if(filename[0] != '-') 
        fclose(fd);

    return true;
}
