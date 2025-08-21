/**
 * @header4iode
 *
 * Functions to load and save ascii definitions of IODE Scalar objects.
 *
 *      KDB *load_asc(char* filename)
 *      int save_asc(KDB* kdb, char* filename)
 *      int save_csv(KDB *kdb, char *filename)
 *
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
    int     keyw, pos;
    Scalar     scl;

    /* READ AT MOST 3 REALS */
    scl.value = K_read_real(yy);
    if(!IODE_IS_A_NUMBER(scl.value)) scl.value = 0.9;
    scl.relax = K_read_real(yy);
    if(!IODE_IS_A_NUMBER(scl.relax)) scl.relax = 1.0;
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
 *  @param [in] filename    char*   name of the ascii file to be read or 
 *                                  string containing the definition of the Scalars
 *  @return                 KDB*    new KDB of Scalar or NULL on error
 *  
 *  TODO: what if read_cmt returns an error code ?
 *  
 */
KDB* AsciiScalars::load_asc(char* filename)
{
    int     cmpt = 0;
    KDB     *kdb = 0;
    YYFILE  *yy;
    ONAME   name;

    /* INIT YY READ */
    YY_CASE_SENSITIVE = 1;

    SCR_strip((unsigned char *) filename);
    yy = YY_open(filename, NULL, 0, (!K_ISFILE(filename)) ? YY_STDIN : YY_FILE);

    if(yy   == 0) {
        kerror(0,"Cannot open '%s'", filename);
        return(kdb);
    }

    /* READ FILE */
    kdb = K_create(SCALARS, LOWER_CASE); 
    K_set_kdb_fullpath(kdb, (U_ch*)filename); // JMP 30/11/2022
    while(1) {
        switch(YY_lex(yy)) {
            case YY_EOF :
                if(cmpt) {
                    char    asc_filename[1024];
                    K_set_ext_asc(asc_filename, filename, SCALARS);
                    K_set_kdb_fullpath(kdb, (U_ch*)asc_filename); // JMP 03/12/2022
                }            
                YY_close(yy);
                return(kdb);

            case YY_WORD :
                yy->yy_text[K_MAX_NAME] = 0;
                strcpy(name, (char*) yy->yy_text);
                if(read_scl(kdb, yy, name) == 0) cmpt++;
                kmsg("Reading object %d : %s", cmpt, name);
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
int AsciiScalars::save_asc(KDB* kdb, char* filename)
{
    FILE    *fd;
    int     i;
    Scalar     *scl;

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
        print_scl(fd, scl);
        fprintf(fd, "\n");
    }

    if(filename[0] != '-') fclose(fd);
    return(0);
}

/* 
 * Save a KDB of Scalar in a .csv file.
 * NOT IMPLEMENTED.
 */
int AsciiScalars::save_csv(KDB *kdb, char *filename, Sample* sample, char** varlist)
{
    return(-1);
}
