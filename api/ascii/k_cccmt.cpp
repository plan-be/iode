/**
 * @header4iode
 * 
 * Functions to load and save ascii definitions of IODE CMT objects.
 *
 *    KDB *load_asc(char* filename, int db_global)
 *    int save_asc(KDB* kdb, char* filename)
 *    int save_csv(KDB *kdb, char *filename)
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
static int read_cmt(KDB* kdb, YYFILE* yy, char* name)
{
    int     keyw, pos;
    char    *cmt;

    /* READ A STRING */
    keyw = YY_lex(yy);
    if(keyw == YY_STRING) cmt = K_wrap((char*) yy->yy_text, 60);
    else {
        cmt = SW_nalloc(1);
        cmt[0] = '\0';
        YY_unread(yy);
    }

    /* CONTINUE READING UNTIL END OF VALUES */
    while(1) {
        keyw = YY_lex(yy);
        if(keyw == YY_WORD || keyw == YY_EOF) break;
    }
    YY_unread(yy);

    pos = K_add(kdb, name, cmt);
    if(pos < 0) {
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
 *  
 */
KDB* AsciiComments::load_asc(char* filename, int db_global)
{
    int     cmpt = 0, rc;
    YYFILE  *yy;
    ONAME   name;
    KDB*    kdb = new KDB(COMMENTS, (db_global == 1) ? DB_GLOBAL : DB_STANDALONE);

    /* INIT YY READ */
    YY_CASE_SENSITIVE = 1;

    SCR_strip((unsigned char *) filename);
    yy = YY_open(filename, NULL, 0, (!K_ISFILE(filename)) ? YY_STDIN : YY_FILE);

    if(yy == 0) 
    {
        kerror(0,"Cannot open '%s'", filename);
        return nullptr;
    }

    /* READ FILE */    
    while(1) 
    {
        switch(YY_lex(yy)) 
        {
            case YY_EOF :
                if(cmpt) 
                {
                    char    asc_filename[1024];
                    K_set_ext_asc(asc_filename, filename, COMMENTS);
                    K_set_kdb_fullpath(kdb, (U_ch*)asc_filename); // JMP 03/12/2022
                }
                YY_close(yy);
                return kdb;

            case YY_WORD :
                yy->yy_text[K_MAX_NAME] = 0;
                strcpy(name, (char*) yy->yy_text);
                rc = read_cmt(kdb, yy, name);
                if(rc == 0) cmpt++;
                kmsg("Reading object %d : %s", cmpt, name);
                break;

            default :
                kerror(0, "Incorrect entry : %s", YY_error(yy));
                break;
        }
    }

    YY_close(yy);
    return nullptr;
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
int AsciiComments::save_asc(KDB* kdb, char* filename)
{
    FILE    *fd;
    int     i;
    //CMT     *cmt;
    CMT     cmt; // JMP 30/9/2021

    if(filename[0] == '-') fd = stdout;
    else {
        fd = fopen(filename, "w+");
        if(fd == 0) {
            kerror(0, "Cannot create '%s'", filename);
            return(-1);
        }
    }

    for(i = 0 ; i < kdb->size(); i++) {
        fprintf(fd, "%s ", (char*) kdb->get_name(i).c_str());
        cmt = KCVAL(kdb, i);
        SCR_replace((unsigned char*) cmt, (unsigned char*) "\n", (unsigned char*) " ");  /* JMP 31-10-96 */
        SCR_fprintf_esc(fd, cmt, 1);
        fprintf(fd, "\n");
    }

    if(filename[0] != '-') fclose(fd);
    return(0);
}

/* 
 * Save a KDB of CMTs in a .csv file.
 * NOT IMPLEMENTED.
 */
int AsciiComments::save_csv(KDB *kdb, char *filename, Sample* sample, char** varlist)
{
    return(-1);
}
