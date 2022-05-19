/**
 * @header4iode 
 
 * Functions to load and save ascii definitions of IODE IDT objects.
 * 
 *     KDB *KI_load_asc(char* filename)
 *     int KI_save_asc(KDB* kdb, char* filename)
 *     int KI_save_csv(KDB *kdb, char *filename)
 *  
 */

#include "iode.h"

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
 *  @param [in] filename    char*   name of the ascii file to be read or 
 *                                  string containing the definition of the identities
 *  @return                 KDB*    new KDB of IDT or NULL on error
 *  
 *  TODO: what if KC_read_cmt returns an error code ?
 *  
 */

KDB *KI_load_asc(char* filename)
{
    char    *lec = NULL;
    int     cmpt = 0;
    KDB     *kdb = 0;
    YYFILE  *yy;
    ONAME   name;

    /* INIT YY READ */
    YY_CASE_SENSITIVE = 1;
    SCR_strip(filename);
    yy = YY_open(filename, 0L, 0,
                 (!K_ISFILE(filename)) ? YY_STDIN : YY_FILE);
    if(yy == 0) {
        kerror(0, "Cannot open '%s'", filename);
        return(kdb);
    }

    /* READ FILE */
    kdb = K_create(K_IDT, K_UPPER);
    while(1) {
        switch(YY_lex(yy)) {
            case YY_EOF :
                YY_close(yy);
                return(kdb);

            case YY_WORD :
                yy->yy_text[K_MAX_NAME] = 0;
                strcpy(name, yy->yy_text);
                if(YY_lex(yy) != YY_STRING) {
                    kerror(0, "%s : identity not defined", YY_error(yy));
                    break;
                }
                lec = K_wrap(yy->yy_text, 60);
                if(K_add(kdb, name, lec) < 0)
                    kerror(0, "%s (%s : %s).",
                           YY_error(yy), name, L_error());
                SW_nfree(lec);
                kmsg("Reading object %d : %s", ++cmpt, name);
                break;

            default :
                kerror(0, "Incorrect entry : %s", YY_error(yy));
                break;
        }
    }

    K_free(kdb);
    YY_close(yy);
    return((KDB *)0);
}

/**
 *  Saves a KDB of IDTs into an ascii file (.ac) or to the stdout.
 *  
 *  @see KI_load_asc() for the syntax. 
 *  
 *  @param [in] kdb         KDB*    KDB of IDTs
 *  @param [in] filename    char*   name of the output file or "-" to write the result on the stdout.
 *  @return                 int     0 on success, -1 if the file cannot be written.
 *  
 */

int KI_save_asc(KDB* kdb, char* filename)
{
    FILE    *fd;
    int     i;

    if(filename[0] == '-') fd = stdout;
    else {
        fd = fopen(filename, "w+");
        if(fd == 0) {
            kerror(0, "Cannot create '%s'", filename);
            return(-1);
        }
    }

    for(i = 0 ; i < KNB(kdb) ; i++) {
        fprintf(fd, "%s ", KONAME(kdb, i));
        fprintf(fd, "\"%s\"\n", KELEC(kdb, i));
    }

    if(filename[0] != '-') fclose(fd);
    return(0);
}

/* 
 * Save a KDB of IDTs in a .csv file.
 * NOT IMPLEMENTED.
 */

int KI_save_csv(KDB *kdb, char *filename)
{
    return(-1);
}
