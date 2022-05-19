/**
 * @header4iode
 *
 * Functions to load and save ascii definitions of IODE SCL objects.
 *
 *      KDB *KS_load_asc(char* filename)
 *      int KS_save_asc(KDB* kdb, char* filename)
 *      int KS_save_csv(KDB *kdb, char *filename)
 *
 */

#include "iode.h"

/**
 *  Reads on an open YY stream (file or string) the ascii definition of an IODE SCL and adds the new SCL to kdb. 
 *  Subfn of KS_load_asc().
 *  
 *  Reads 3 values on yy. Each token must be a number (long or double). If not : 
 *      the first value is replaced by 0.9 (value of the scalar)
 *      the second value (relax) is replaced by 1.0
 *      the 3d value (stderr) is replaced by L_NAN
 *  
 *  @param [in, out ]   kdb     KDB*    KDB of SCLs
 *  @param [in, out]    yy      YYFILE* stream (file or string) to be read 
 *  @param [in]         name    char*   SCL name
 *  @return                     int     0 if the SCL is read and saved, -1 if it can't be created.
 *  
 */

static int KS_read_scl(KDB* kdb, YYFILE* yy, char* name)
{
    int     keyw, pos;
    SCL     scl;

    /* READ AT MOST 3 REALS */
    scl.val = K_read_real(yy);
    if(!L_ISAN(scl.val)) scl.val = 0.9;
    scl.relax = K_read_real(yy);
    if(!L_ISAN(scl.relax)) scl.relax = 1.0;
    scl.std = K_read_real(yy);

    /* CONTINUE READING UNTIL END OF VALUES */
    while(1) {
        keyw = YY_lex(yy);
        if(keyw == YY_WORD || keyw == YY_EOF) break;
    }
    YY_unread(yy);

    pos = K_add(kdb, name, &scl);
    if(pos < 0) {
        kerror(0, "%s : unable to create %s", YY_error(yy), name);
        return(-1);
    }

    return(0);
}


/**
 *  Loads SCLs from an ASCII file and saves the SCLs into a new KDB.
 *  
 *  Syntax of SCL ascii definitions 
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
 *  @param [in] filename    char*   name of the ascii file to be read or 
 *                                  string containing the definition of the SCLs
 *  @return                 KDB*    new KDB of SCL or NULL on error
 *  
 *  TODO: what if KS_read_cmt returns an error code ?
 *  
 */

KDB *KS_load_asc(char* filename)
{
    int     cmpt = 0;
    KDB     *kdb = 0;
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
    kdb = K_create(K_SCL, K_LOWER);
    while(1) {
        switch(YY_lex(yy)) {
            case YY_EOF :
                YY_close(yy);
                return(kdb);

            case YY_WORD :
                yy->yy_text[K_MAX_NAME] = 0;
                strcpy(name, yy->yy_text);
                KS_read_scl(kdb, yy, name);
                kmsg("Reading object %d : %s", ++cmpt, name);
                break;

            default :
                kerror(0, "Incorrect entry : %s", YY_error(yy));
                break;
        }
    }

    YY_close(yy);
    K_free(kdb);
    return((KDB *)0);
}


/**
 *  Writes the definition of a SCL to a file. L_NAN are represented by na.
 *  
 *  @param [in] fd  pointer to a FILE
 *  @param [in] scl pointer to the SCL
 *  
 */
static void KS_print_scl(FILE* fd, SCL* scl)
{
    if(L_ISAN(scl->val)) fprintf(fd, "%.15lg ", (double)(scl->val)); /* JMP 09-04-98 */
    else fprintf(fd, "na ");

    if(L_ISAN(scl->relax)) fprintf(fd, "%.15lg ", (double)(scl->relax)); /* JMP 09-04-98 */
    else fprintf(fd, "na ");

    if(L_ISAN(scl->std)) fprintf(fd, "%.15lg ", (double)(scl->std));     /* JMP 09-04-98 */
    else fprintf(fd, "na ");
}

/**
 *  Saves a KDB of SCLs into an ascii file (.as) or to the stdout.
 *  
 *  @see KS_load_asc() for the syntax. 
 *  
 *  @param [in] kdb         KDB*    KDB of SCLs
 *  @param [in] filename    char*   name of the output file or "-" to write the result on the stdout.
 *  @return                 int     0 on success, -1 if the file cannot be written.
 *  
 */
int KS_save_asc(KDB* kdb, char* filename)
{
    FILE    *fd;
    int     i;
    SCL     *scl;

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
        scl = KSVAL(kdb, i);
        KS_print_scl(fd, scl);
        fprintf(fd, "\n");
    }

    if(filename[0] != '-') fclose(fd);
    return(0);
}


/* 
 * Save a KDB of SCL in a .csv file.
 * NOT IMPLEMENTED.
 */

int KS_save_csv(KDB *kdb, char *filename)
{
    return(-1);
}
