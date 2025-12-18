/**
 * @header4iode
 * 
 * Functions to load and save ascii definitions of IODE CMT objects.
 *
 *    bool load_asc(const std::string& filename)
 *    bool save_asc(const std::string& filename)
 */
#include "api/b_errors.h"
#include "api/k_super.h"
#include "api/objs/objs.h"
#include "api/objs/comments.h"
#include "api/ascii/ascii.h"


/**
 *  Reads on an open YY stream (file or string) the ascii definition of an IODE CMT and adds the new CMT to kdb. 
 *  Subfn of load_asc().
 *  
 *  Reads the next token on yy. 
 *      If a string (bw "") is found, its content becomes the new content of the CMT name.
 *      If not, "" is stored in CMT name 
 *  
 *  @param [in, out ]   kdb     KDB*    KDB of CMTs
 *  @param [in, out]    yy      YYFILE* stream (file or string) to be read on
 *  @param [in]         name    char*   CMT name
 *  @return                     int     0 if the CMT is read and saved, -1 if the CMT can't be created.
 *  
 */
static int read_cmt(CKDBComments* kdb, YYFILE* yy, char* name)
{
    int     keyw;
    char    *cmt;
    bool    success;

    /* READ A STRING */
    keyw = YY_lex(yy);
    if(keyw == YY_STRING)   
        cmt = K_wrap((char*) yy->yy_text, 60);
    else 
    {
        cmt = SW_nalloc(1);
        cmt[0] = '\0';
        YY_unread(yy);
    }

    /* CONTINUE READING UNTIL END OF VALUES */
    while(1) 
    {
        keyw = YY_lex(yy);
        if(keyw == YY_WORD || keyw == YY_EOF) break;
    }
    YY_unread(yy);

    success = kdb->set_obj(name, cmt);
    if(!success) 
    {
        kerror(0, "%s : unable to create %s", YY_error(yy), name);
        SW_nfree(cmt);
        return(-1);
    }

    SW_nfree(cmt);
    return(0);
}

/**
 *  Loads CMTs from an ASCII file into a new KDB.
 *  
 *  Syntax of CMT ascii definitions 
 *  -------------------------------
 *      CMTNAME "comment content"
 *  
 *  Example
 *  -------
 *      CMTNAME1 "text of name 1"
 *      CMTNAME2 "text of name 2
 *                ...
 *                end of text of name 2"
 *  
 *  Error messages are sent to the function kerror().
 *  For each read CMT, kmsg() is called to send a message to the user. 
 *  
 *  The implementations of kerror() and kmsg() depend on the context.
 *  
 *  @param [in] filename     char*   name of the ascii file to be read or 
 *                                   string containing the definition of the comment
 *  @param [in]   db_global  int     1 for DB_GLOBAL, 0 for DB_STANDALONE
 *  @return                  KDB*    new KDB of CMT or NULL on error
 *  
 *  TODO: what if read_cmt returns an error code ?
 */
bool CKDBComments::load_asc(const std::string& filename)
{
    int     cmpt = 0, rc;
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
    while(1) 
    {
        switch(YY_lex(yy)) 
        {
            case YY_EOF :
                if(cmpt) 
                {
                    char asc_filename[1024];
                    K_set_ext_asc(asc_filename, c_filename, COMMENTS);
                    K_set_kdb_fullpath(this, (U_ch*) asc_filename);
                }
                YY_close(yy);
                return true;

            case YY_WORD :
                yy->yy_text[K_MAX_NAME] = 0;
                strcpy(name, (char*) yy->yy_text);
                rc = read_cmt(this, yy, name);
                if(rc == 0) 
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
 *  Saves a KDB of CMTs into an ascii file (.ac) or to the stdout.
 *  
 *  @see KC_load_asc() for the syntax. 
 *  
 *  @param [in] kdb         KDB*    KDB of CMTs
 *  @param [in] filename    char*   name of the output file or "-" to write the result on the stdout.
 *  @return                 int     0 on success, -1 if the file cannot be written.
 *  
 */
bool CKDBComments::save_asc(const std::string& filename)
{
    FILE    *fd;
    CMT     cmt;

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
        cmt = KCVAL(this, handle);
        SCR_replace((unsigned char*) cmt, (unsigned char*) "\n", (unsigned char*) " ");  /* JMP 31-10-96 */
        SCR_fprintf_esc(fd, cmt, 1);
        fprintf(fd, "\n");
    }

    if(filename[0] != '-') 
        fclose(fd);

    return true;
}
