/**
 * @header4iode
 * 
 * Functions to load and save ascii definitions of IODE CMT objects.
 *
 *    KDB *KC_load_asc(char* filename)
 *    int KC_save_asc(KDB* kdb, char* filename)
 *    int KC_save_csv(KDB *kdb, char *filename)
 */

#include "iode.h"

/**
 *  Reads on an open YY stream (file or string) the ascii definition of an IODE CMT and adds the new CMT to kdb. 
 *  Subfn of KC_load_asc().
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
static int KC_read_cmt(KDB* kdb, YYFILE* yy, char* name)
{
    int     i, keyw, pos;
    char    *cmt;

    /* READ A STRING */
    keyw = YY_lex(yy);
    if(keyw == YY_STRING) cmt = K_wrap(yy->yy_text, 60);
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
 *  @param [in] filename    char*   name of the ascii file to be read or 
 *                                  string containing the definition of the comment
 *  @return                 KDB*    new KDB of CMT or NULL on error
 *  
 *  TODO: what if KC_read_cmt returns an error code ?
 *  
 */
KDB *KC_load_asc(char* filename)
{
    KDB     *kdb = 0;
    int     cmpt = 0;
    YYFILE  *yy;
    ONAME   name;

    /* INIT YY READ */
    YY_CASE_SENSITIVE = 1;

    SCR_strip(filename);
    yy = YY_open(filename, NULL, 0, (!K_ISFILE(filename)) ? YY_STDIN : YY_FILE);

    if(yy   == 0) {
        kerror(0,"Cannot open '%s'", filename);
        return(kdb);
    }

    /* READ FILE */
    kdb = K_create(K_CMT, K_ASIS);
    while(1) {
        switch(YY_lex(yy)) {
            case YY_EOF :
                YY_close(yy);
                return(kdb);

            case YY_WORD :
                yy->yy_text[K_MAX_NAME] = 0;
                strcpy(name, yy->yy_text);
                KC_read_cmt(kdb, yy, name);
                kmsg("Reading object %d : %s", ++cmpt, name);
                break;

            default :
                kerror(0, "Incorrect entry : %s", YY_error(yy));
                break;
        }
    }

err:
    YY_close(yy);
    K_free(kdb);
    return((KDB *)0);
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
int KC_save_asc(KDB* kdb, char* filename)
{
    FILE    *fd;
    int     i, j;
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

    for(i = 0 ; i < KNB(kdb); i++) {
        fprintf(fd, "%s ", KONAME(kdb, i));
        cmt = KCVAL(kdb, i);
        SCR_replace(cmt, "\n", " ");  /* JMP 31-10-96 */
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
int KC_save_csv(KDB *kdb, char *filename)
{
    return(-1);
}
