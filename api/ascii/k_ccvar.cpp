/**
 * @header4iode
 * 
 * Functions to load and save ascii and csv definitions of IODE VAR objects.
 *
 *      - KDB *KV_load_asc(char *filename)
 *      - KV_save_asc(KDB* kdb, char* filename)
 *      - int KV_save_csv(KDB *kdb, char *filename, SAMPLE *smpl, char **varlist)
 */
#include "scr4.h"

#include "api/b_errors.h"
#include "api/k_super.h"
#include "api/objs/objs.h"
#include "api/objs/variables.h"
#include "api/ascii/ascii.h"


/**
 * Table of keywords recognized by YY in the context of an VAR ascii file (.av).
 * See s_yy function group for more informations (http://www.xon.be/scr4/libs1/libs157.htm).
 */

#define KV_SMPL     1

YYKEYS KV_TABLE[] = {
    (unsigned char *) "sample",   KV_SMPL
};


/**
 *  Reads one series on the stream YY. Subfunction of KV_load_yy().
 
 *  The series is read until the next token is neither a number nor the token "na" (not a number).
 *  The missing values till the end of the sample are fixed to na.
 *  
 *  @param [in, out] kdb    KDB*    KDB of vars
 *  @param [in, out] yy     YY*     open stream 
 *  @param [in]      name   char*   name of the variable that will be read .
 *  @return                 int     0 on success, -1 if the var <name> cannot be created.
 *  
 */
static int KV_read_vec(KDB* kdb, YYFILE* yy, char* name)
{
    int     i, keyw, pos, nb;
    double    *vec;
    SAMPLE  *smpl;

    smpl = (SAMPLE *) KDATA(kdb);
    vec = (double *) SW_nalloc(smpl->s_nb * sizeof(double));

    /* READ AT MOST nobs OBSERVATIONS */
    for(i = 0; i < smpl->s_nb; i++)  vec[i] = K_read_real(yy);

    /* CONTINUE READING UNTIL END OF VALUES */
    while(1) {
        keyw = YY_lex(yy);
        if((keyw == YY_WORD && strcmp((char*) yy->yy_text, "na")) || keyw == YY_EOF) break;
    }
    YY_unread(yy);

    nb = smpl->s_nb;
    pos = K_add(kdb, name, vec, &nb);
    if(pos < 0) {
        kerror(0, "%s : unable to create %s", YY_error(yy), name);
        SW_nfree(vec);
        return(-1);
    }

    SW_nfree(vec);
    return(0);
}


/**
 *  Subfn of KV_load_asc().
 *  Reads ascii definitions of IODE variables from an opened YY stream and returns a new KDB*. 
 *  
 *  
 *  The YY stream must be opened and can be a file or a string.
 *  
 *  @see http://www.xon.be/scr4/libs1/libs157.htm for more information on YY.
 *    
 *  @param [in, out]    yy      YY*     YY stream to read data from
 *  @param [in]         ask     int     see KV_load_asc_type_ask() 
 *  @return                     KDB*    a new KDB of IODE vars or NULL on error
 *  
 */
static KDB *KV_load_yy(YYFILE* yy, int ask)
{
    KDB     *kdb = 0;
    int     cmpt = 0;
    ONAME   name;
    //PERIOD  one, two;
    SAMPLE  *smpl, *kasksmpl();

    // Create and empty KDB for vars
    kdb = K_create(VARIABLES, UPPER_CASE);

    // The keyword sample must be the first on the YY stream */
    // if not:
    //    - if ask == 1: the user must provide the sample via a call to kasksmpl()
    //    - else:        the function returns NULL
    //
    if(YY_lex(yy) != KV_SMPL) {
        if(!ask) {
            kerror(0,"%s Expected sample definition", YY_error(yy));
            goto err;
        }
        else {
            YY_unread(yy);
            //smpl = K_ask_smpl();
            smpl = kasksmpl();
        }
    }
    else smpl = K_read_smpl(yy);

    if(smpl == NULL) goto err;
    memcpy((SAMPLE *) KDATA(kdb), smpl, sizeof(SAMPLE));

    /* Loop on var definition 
        NAME1 value ... NAME2 ...
    */
    while(1) {
        switch(YY_lex(yy)) {
            case YY_EOF :
                SW_nfree(smpl);
                return(kdb);

            case YY_WORD :
                if(smpl->s_nb == 0) {
                    kerror(0, "%s : undefined sample", YY_error(yy));
                    goto err;
                }
                yy->yy_text[K_MAX_NAME] = 0;
                strcpy(name, (char*) yy->yy_text);
                KV_read_vec(kdb, yy, name);
                kmsg("Reading object %d : %s", ++cmpt, name);
                break;

            default :
                kerror(0, "Incorrect entry : %s", YY_error(yy));
                break;
        }
    }

err:
    SW_nfree(smpl);
    K_free(kdb);
    return((KDB *)0);
}

/**
 *  Loads variables from an ASCII file or from a string.
 *  
 *  @param [in] file_or_string    char* file_or_string or string to interpret
 *  @param [in] type        int   1 => file_or_string is a string containing the var definitions, 
 *                                0 => file_or_string is a file name 
 *  @param [in] ask         int   1 => if and the sample definition is not present on the YY stream, calls kasksmpl() to get the sample from the user.
 *                                0 => if and the sample definition is not present on the YY stream, quit the function and returns NULL
 *  @return                 KDB*  NULL or new KDB of variables
 *  
 */
KDB *KV_load_asc_type_ask(char *file_or_string, int type, int ask)
{
    static  int sorted;
    KDB     *kdb = 0;
    YYFILE  *yy;
    int		yytype;

    if(type == 0) {
        SCR_strip((unsigned char *) file_or_string);
        yytype = (!K_ISFILE(file_or_string)) ? YY_STDIN : YY_FILE;
    }
    else {
        yytype = YY_MEM;
    }

    /* INIT YY READ */
    YY_CASE_SENSITIVE = 1;

    if(sorted == 0) {
        qsort(KV_TABLE, sizeof(KV_TABLE) / sizeof(YYKEYS), sizeof(YYKEYS), compare);
        sorted = 1;
    }

    yy = YY_open(file_or_string, KV_TABLE, sizeof(KV_TABLE) / sizeof(YYKEYS), yytype);

    if(yy == 0) {
        if(!type) kerror(0,"Cannot open '%s'", file_or_string);
        else      kerror(0,"Cannot open string");
        return(kdb);
    }

    kdb = KV_load_yy(yy, ask);
    YY_close(yy);
    return(kdb);
}

/**
 *  Loads variables from an ASCII file into a new KDB. On success, adds extension to ".av" to 
 *  filename in kd->k_nameptr.
 *  
 *  @param [in] filename    char* ascii file to read
 *  @return                 KDB*  NULL or new KDB of variables
 *  
 */
KDB *KV_load_asc(char *filename)
{
    KDB     *kdb;
    
    kdb = KV_load_asc_type_ask(filename, 0, 0);
    if(kdb && KNB(kdb) > 0) {
        char    asc_filename[1024];
        K_set_ext_asc(asc_filename, filename, VARIABLES);
        K_set_kdb_fullpath(kdb, (U_ch*)asc_filename); // JMP 03/12/2022
    }
   
    return(kdb);
}


/**
 *  Prints the representation of one value on fd. For NaN value, prints "na". 
 *  
 *  @param [in, out]    fd      FILE *      output stream    
 *  @param [in]         val     double   value to print  
 *  @return 
 *  
 */
static void KV_print_val(FILE* fd, double val)
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
int KV_save_asc(KDB* kdb, char* filename)
{
    FILE    *fd;
    int     i, j;
    double    *val;
    SAMPLE  *smpl;

    if(filename[0] == '-') fd = stdout;
    else {
        fd = fopen(filename, "w+");
        if(fd == 0) {
            kerror(0, "Cannot create '%s'", filename);
            return(-1);
        }
    }

    smpl = (SAMPLE *) KDATA(kdb);
    fprintf(fd, "sample %s ", PER_pertoa(&(smpl->s_p1), 0L));
    fprintf(fd, "%s\n", PER_pertoa(&(smpl->s_p2), 0L));

    for(i = 0 ; i < KNB(kdb); i++) {
        fprintf(fd, "%s ", KONAME(kdb, i));
        val = KVVAL(kdb, i, 0);
        for(j = 0 ; j < smpl->s_nb; j++, val++) KV_print_val(fd, *val);
        fprintf(fd, "\n");
    }

    if(filename[0] != '-') fclose(fd);
    return(0);
}



/* --------------- CSV Files -------------------- */

/** 
 *  Parameters specific to csv output files. 
 *  These parameters can be modified via report commands: 
 *    - $csvdigits
 *    - $csvsep
 *    - $csvdec
 *    - $csvnan
 *    - $csvaxes
 *  
 */

char* KV_CSV_SEP = 0;       // cell separator     (default ",")
char* KV_CSV_DEC = 0;       // decimal separator (default ".")
char* KV_CSV_NAN = 0;       // NaN representation (default "")
char* KV_CSV_AXES = 0;      // name of the axis for the variable name (default "var")
int   KV_CSV_NBDEC = 15;     // number of decimals


/**
 *  Saves a KDB of variables in a csv file à la LArray.
 *  
 *  The following global variables can be modified to customize the output :
 *  
 *    - char *KV_CSV_SEP = 0;       // cell separator     (default ",")
 *    - char *KV_CSV_DEC = 0;       // decimal separator (default ".")
 *    - char *KV_CSV_NAN = 0;       // NaN representation (default "")
 *    - char *KV_CSV_AXES = 0;      // name of the axis for the variable name (default "var") or NULL for all variables
 *    - int  KV_CSV_NBDEC = 15;     // number of decimals
 *  
 *  
 *  @param [in] kdb      KDB*       KDB of vars to save
 *  @param [in] filename char*      output file or "-" for stdout 
 *  @param [in] smpl     SAMPLE*    sample to save
 *  @param [in] varlist  char**     list of variables to save in the file
 *  @return 
 *  
 */
int KV_save_csv(KDB *kdb, char *filename, SAMPLE *smpl, char **varlist)
{
    FILE        *fd;
    int         i, j, nb, pos;
    char        fmt[80], buf[256], *sep, *dec, *nan, *axes, **lst;
    double   *val;
    PERIOD      *per;

    // Parameters : sep, dec, nbdec, nan,
    sep = KV_CSV_SEP;
    dec = KV_CSV_DEC;
    nan = KV_CSV_NAN;
    if(sep == 0 || sep[0] == 0) {
        sep = ",";
        sep[0] = SCR_sLIST; // JMP 27/1/2017
    }
    if(dec == 0 || dec[0] == 0) {
        dec = ".";
        dec[0] = SCR_sDECIMAL; // JMP 27/1/2017
    }
    if(KV_CSV_NBDEC < 0 || KV_CSV_NBDEC > 15)
        strcpy(fmt, "%lg");
    else
        sprintf(fmt, "%%.%dlg", KV_CSV_NBDEC);
    if(nan == 0) nan = "";

    axes = KV_CSV_AXES;
    if(axes == 0 || axes[0] == 0) axes = "var";

    // sample : if KV_CSV_SAMPLE == NULL => tout le sample
    if(smpl == 0)
        smpl = (SAMPLE *) KDATA(kdb);

    // Liste
    lst = varlist;
    if(varlist == 0) {
        lst = 0;
        nb = 0;
        for(i = 0; i < KNB(kdb); i++)
            SCR_add_ptr((unsigned char ***) &lst, &nb, (unsigned char*) KONAME(kdb, i));
        SCR_add_ptr((unsigned char ***) &lst, &nb, 0L);
    }

    // Open file
    if(filename[0] == '-') fd = stdout;
    else {
        fd = fopen(filename, "w+");
        if(fd == 0) {
            kerror(0, "Cannot create '%s'", filename);
            return(-1);
        }
    }

    // Ligne 1
    fprintf(fd, "%s\\time", axes);
    for(i = 0 ; i < smpl->s_nb ; i++) {
        if(smpl->s_p1.p_p == 'Y')
            fprintf(fd, "%s%d", sep, smpl->s_p1.p_y + i);
        else {
            per = PER_addper(&(smpl->s_p1), i);
            fprintf(fd, "%s%s", sep, PER_pertoa(per, 0L));
        }
    }
    fprintf(fd, "\n");

    // Lignes suivantes
    for(i = 0 ; lst[i] ; i++) {
        SCR_upper((unsigned char *) lst[i]);
        fprintf(fd, "%s", lst[i]);
        pos = K_find(kdb, lst[i]);
        if(pos >= 0) {
            val = KVVAL(kdb, pos, 0);
            for(j = 0 ; j < smpl->s_nb; j++, val++) {
                if(IODE_IS_A_NUMBER(*val)) {
                    sprintf(buf, fmt, (double)(*val));
                    if(dec[0] != '.') SCR_replace((unsigned char *) buf, (unsigned char *) ".", (unsigned char *) dec);
                }
                else {
                    strcpy(buf, nan);
                }
                fprintf(fd, "%s%s", sep, buf);
            }
        }
        fprintf(fd, "\n");
    }
    if(varlist == 0) SCR_free_tbl((unsigned char **) lst);
    if(filename[0] != '-') fclose(fd);
    return(0);
}

