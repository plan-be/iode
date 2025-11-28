/**
 * @header4iode 
 
 * Functions to load and save ascii definitions of IODE IDT objects.
 * 
 *     bool load_asc(const std::string& filename)
 *     bool save_asc(const std::string& filename)
 */
#include "api/b_errors.h"
#include "api/k_super.h"
#include "api/objs/objs.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/ascii/ascii.h"


/**
 *  Loads IDTs from an ASCII file into a new KDB.
 *  
 *  Syntax of IDT ascii definitions 
 *  -------------------------------
 *      IDTNAME "<valid lec expression>"
 *  
 *  Example
 *  -------
 *      
 *      AOUC    "((WCRH/QL)/(WCRH/QL)[1990Y1])*(VAFF/(VM+VAFF))[-1]+PM*(VM/(VM+VAFF))[-1]"
 *      AOUC_   "exp(ln(((WCF/NFYH)/QL)+PKF/(QAFF/KNFFY))*(QAFF/(QX+QAFF)+.05)[-1]+
 *                  ln PM*(QM/(QAFF+QM)-0.05)[-1])"
 *  
 *  Error messages are sent to the function kerror().
 *  For each read CMT, kmsg() is called to send a message to the user. 
 *  
 *  The implementations of kerror() and kmsg() depend on the context (GUI, command line...).
 *  
 *  @param [in] filename     char*   name of the ascii file to be read or 
 *                                   string containing the definition of the identities
 *  @param [in]   db_global  int     1 for DB_GLOBAL, 0 for DB_STANDALONE
 *  @return                  KDB*    new KDB of IDT or NULL on error
 *  
 *  TODO: what if KC_read_cmt returns an error code ?
 *  
 */
bool KDB::load_asc_idt(const std::string& filename)
{
    char    *lec = NULL;
    int     cmpt = 0;
    YYFILE  *yy;
    ONAME   name;

    /* INIT YY READ */
    YY_CASE_SENSITIVE = 1;

    std::string trim_filename = trim(filename);
    char* c_filename = (char*) trim_filename.c_str();
    yy = YY_open(c_filename, 0L, 0,
                 (!K_ISFILE(c_filename)) ? YY_STDIN : YY_FILE);
    if(yy == 0)
    {
        std::string error_msg = "Cannot open file '" + trim_filename + "'";
        kwarning(error_msg.c_str());
        return false;
    }

    clear();  /* clear current KDB */

    /* READ FILE */
    K_set_kdb_fullpath(this, (U_ch*) c_filename); // JMP 28/11/2022
    while(1) 
    {
        switch(YY_lex(yy)) 
        {
            case YY_EOF :
                if(cmpt) 
                {
                    char asc_filename[1024];
                    K_set_ext_asc(asc_filename, c_filename, IDENTITIES);
                    K_set_kdb_fullpath(this, (U_ch*) asc_filename); // JMP 03/12/2022
                }
                YY_close(yy);
                return true;

            case YY_WORD :
                yy->yy_text[K_MAX_NAME] = 0;
                strcpy(name, (char*) yy->yy_text);
                if(YY_lex(yy) != YY_STRING) 
                {
                    kwarning("%s : identity not defined", YY_error(yy));
                    break;
                }
                lec = K_wrap((char*) yy->yy_text, 60);
                if(!this->set(name, lec))
                    kwarning("%s (%s : %s).", YY_error(yy), name, L_error());
                else 
                    cmpt++;
                SW_nfree(lec);
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
 *  Saves a KDB of IDTs into an ascii file (.ac) or to the stdout.
 *  
 *  @see load_asc() for the syntax. 
 *  
 *  @param [in] kdb         KDB*    KDB of IDTs
 *  @param [in] filename    char*   name of the output file or "-" to write the result on the stdout.
 *  @return                 int     0 on success, -1 if the file cannot be written.
 *  
 */
bool KDB::save_asc_idt(const std::string& filename)
{
    FILE* fd;

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
        fprintf(fd, "\"%s\"\n", KILEC(this, handle));
    }

    if(filename[0] != '-') 
        fclose(fd);

    return true;
}
