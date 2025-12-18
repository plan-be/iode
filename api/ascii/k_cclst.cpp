/**
 * @header4iode
 * 
 * Functions to load and save ascii definitions of IODE LST objects.
 *
 *      bool load_asc(const std::string& filename)
 *      bool save_asc(const std::string& filename)
 */
#include "api/b_errors.h"
#include "api/k_super.h"
#include "api/objs/objs.h"
#include "api/objs/lists.h"
#include "api/ascii/ascii.h"


/**
 *  Reads on an open YY stream (file or string) the ascii definition of an IODE LST and adds the new LST to kdb. 
 *  Subfn of KC_load_asc().
 *  
 *  Reads the next token on yy. 
 *      If a string (between "") is found, its content becomes the new content of the LST named *name*.
 *      If not, "" is stored in LST *name*
 *  The content of the list is split in max 60-chars lines.
 *  
 *  All other tokens found between the definition and the next name are ignored.
 *  
 *  @param [in, out ]   kdb     KDB*    KDB of LST
 *  @param [in, out]    yy      YYFILE* stream (file or string) to be read from
 *  @param [in]         name    char*   LST name
 *  @return                     int     0 if the LST is read and saved, 
 *                                      -1 if the LST can't be created. (call to kerror() in that case)
 *  
 */
static int read_lst(CKDBLists* kdb, YYFILE* yy, char* name)
{
    bool    success;
    int     keyw;
    char    *lst;

    /* READ A STRING */
    keyw = YY_lex(yy);
    if(keyw == YY_STRING)  
        lst = K_wrap((char*) yy->yy_text, 60);
    else 
    {
        lst = SW_nalloc(1);
        lst[0] = '\0';
        YY_unread(yy);
    }

    /* CONTINUE READING UNTIL END OF VALUES */
    while(1) 
    {
        keyw = YY_lex(yy);
        if(keyw == YY_WORD || keyw == YY_EOF) 
            break;
    }
    YY_unread(yy);

    success = kdb->set_obj(name, lst);
    if(!success) 
    {
        kerror(0, "%s : unable to create %s", YY_error(yy), name);
        SW_nfree(lst);
        return(-1);
    }

    SW_nfree(lst);
    return(0);
}

/**
 *  Loads LSTs from an ASCII file into a new KDB.
 *  
 *  Syntax of LST ascii definitions 
 *  -------------------------------
 *      LSTNAME "list of values separated by space, comma or semi-colon"
 *  
 *  Example
 *  -------
 *      COPY "$COPY0;$COPY1;"
 *      COPY0 "ACAF;ACAG;AOUC;AQC;BENEF;BQY;BVY;CGU;COEFON;COTRES;DPU;
 *               DPUF;DPUG;DPUGO;DPUH;DPUU;DTF;DTH;DTH1;DTH1C;EXC;EXCC;FLF;
 *               FLG;GAP;GOSF;GOSG;GOSH;GOSH_;IDF;IDG;IDH;IFU;IHU;IT;ITCEE"
 *      COPY1 "VMAB;VME;VMK;VMN;VMS;VMT;VS;VS_;VX;VXAB;VXB;VXE;VXK;VXN;VXS;
 *               VXT;WBF;WBF_;WBG;WBGO;WBGP;WBU;WBU_;WCF;WCF_;WDOM;WG;WIND"
 *
 *  Error messages are sent to the function kerror().
 *  
 *  For each read LST, kmsg() is called to send a message to the user. 
 *  
 *  The implementations of kerror() and kmsg() depend on the context (GUI, command line...).
 *  
 *  @param [in] filename     char*   name of the ascii file to be read or 
 *                                   string containing the definition of the identities
 *  @param [in]   db_global  int     1 for DB_GLOBAL, 0 for DB_STANDALONE
 *  @return                  KDB*    new KDB of LST or NULL on error
 *  
 *  TODO: what if read_lst returns an error code ?
 *  
 */
bool CKDBLists::load_asc(const std::string& filename)
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
                    K_set_ext_asc(asc_filename, c_filename, LISTS);
                    K_set_kdb_fullpath(this, (U_ch*) asc_filename);
                }
                YY_close(yy);
                return true;

            case YY_WORD :
                yy->yy_text[K_MAX_NAME] = 0;
                strcpy(name, (char*) yy->yy_text);
                if(read_lst(this, yy, name) == 0) 
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
 *  Saves a KDB of LSTs in an ascii file (.al) or to the stdout.
 *  
 *  @see load_asc() for the syntax. 
 *  
 *  @param [in] kdb         KDB*    KDB of LSTs
 *  @param [in] filename    char*   name of the output file or "-" to write the result on the stdout.
 *  @return                 int     0 on success, -1 if the file cannot be written.
 *  
 */
bool CKDBLists::save_asc(const std::string& filename)
{
    FILE    *fd;
    LIS     lst;

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
        lst = KLVAL(this, handle);
        SCR_fprintf_esc(fd, lst, 1);
        fprintf(fd, "\n");
    }

    if(filename[0] != '-') 
        fclose(fd);

    return true;
}
