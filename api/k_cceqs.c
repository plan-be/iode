/**
 * @header4iode
 * 
 * Functions to load and save ascii definitions of IODE EQ objects.
 *
 *     KDB *KE_load_asc(char* filename)
 *     int KE_save_asc(KDB* kdb, char* filename)
 *     int KE_save_csv(KDB *kdb, char *filename) : not implemented
 *
 */

#include "iode.h"

/**
 * Table of keywords recognized by YY in the context of an EQS ascii file (.ae).
 * See s_yy function group for more informations (http://www.xon.be/scr4/libs1/libs157.htm).
 */
 
YYKEYS KE_TABLE[] = {
    "{",            KE_OPEN,
    "}",            KE_CLOSE,
    "BLOCK",        KE_BLK,
    "COMMENT",      KE_CMT,
    "DATE",         KE_DATE,
    "INF",          KE_INF,
    "INSTRUMENTS",  KE_INSTR,
    "GLS",          KE_GLS,
    "LSQ",          KE_LSQ,
    "SAMPLE",       KE_SMPL,
    "ZELLNER",      KE_ZEL,
    "MAXLIK",       KE_MAXLIK,

    "STDEV",        KE_STDEV,
    "MEANY",        KE_MEANY,
    "SSRES",        KE_SSRES,
    "STDERR",       KE_STDERR,
    "FSTAT",        KE_FSTAT,
    "R2",           KE_R2,
    "R2ADJ",        KE_R2ADJ,
    "DW",           KE_DW,
    "LOGLIK",       KE_LOGLIK
};


/**
 *  Reads on an opened YY stream (file or string) the ascii definition of an IODE EQ. 
 *  Subfn of KE_load_asc().
 *   
 *  Splits the LEC expression in multiple lines of max 60 chars.
 *  
 *  @param [in, out]    yy      YY*     YY stream to read data from
 *  @return                     EQ*     a new allocated EQ or NULL on error
 *  
 */
static EQ* KE_read_eq(YYFILE* yy)
{
    int     keyw;
    char    *lec;
    EQ      *eq = NULL;
    SAMPLE  *smpl = NULL;


    eq = (EQ *) SW_nalloc(sizeof(EQ));
    eq->method = 0;
    if((keyw = YY_lex(yy)) != KE_OPEN)  {
        YY_unread(yy);
        lec = K_read_str(yy);
        eq->lec = K_wrap(lec, 60);  
        SW_nfree(lec);

        if(eq->lec == NULL) {
            kerror(0, "%s: Expected LEC definition, [%s]", lec, YY_error(yy));
            return(NULL);
        }
        return(eq);
    }

    lec = K_read_str(yy);
    eq->lec = K_wrap(lec, 60);
    SW_nfree(lec);

    if(eq->lec == NULL) {
        kerror(0, "%s: Expected LEC definition, [%s]", lec, YY_error(yy));
        return(NULL);
    }

    while(1) {
        keyw = YY_lex(yy);
        switch(keyw) {
            case YY_WORD :
            case YY_EOF  :
                YY_unread(yy);
            case KE_CLOSE:
                return(eq);
            case KE_DATE :
                eq->date = K_read_long(yy);
                break;

            case KE_LSQ :
                eq->method = 0;
                break;
            case KE_ZEL :
                eq->method = 1;
                break;
            case KE_INF :
                eq->method = 2;
                break;
            case KE_GLS :
                eq->method = 3;
                break;
            case KE_MAXLIK :
                eq->method = 4;
                break;

            case KE_SMPL :
                smpl = K_read_smpl(yy);
                if(smpl == NULL) goto err;
                memcpy(&(eq->smpl), smpl, sizeof(SAMPLE));
                break;
            case KE_BLK  :
                eq->blk = K_read_str(yy);
                if(eq->blk == NULL) goto err;
                break;
            case KE_CMT  :
                eq->cmt = K_read_str(yy);
                if(eq->cmt == NULL) goto err;
                break;
            case KE_INSTR:
                eq->instr = K_read_str(yy);
                if(eq->instr == NULL) goto err;
                break;

            case KE_STDEV :
                eq->tests[1] = K_read_real(yy);
                break;
            case KE_MEANY :
                eq->tests[2] = K_read_real(yy);
                break;
            case KE_SSRES :
                eq->tests[3] = K_read_real(yy);
                break;
            case KE_STDERR :
                eq->tests[4] = K_read_real(yy);
                break;
            case KE_FSTAT :
                eq->tests[6] = K_read_real(yy);
                break;
            case KE_R2 :
                eq->tests[7] = K_read_real(yy);
                break;
            case KE_R2ADJ :
                eq->tests[8] = K_read_real(yy);
                break;
            case KE_DW :
                eq->tests[9] = K_read_real(yy);
                break;
            case KE_LOGLIK :
                eq->tests[10] = K_read_real(yy);
                break;

            default :
                kerror(0, "Incorrect entry : %s", YY_error(yy));
                break;
        }
    }

err :
    E_free(eq);
    return(NULL);

}


/**
 *  Loads EQs from an ASCII file into a new KDB.
 *  
 *  Syntax of EQ ascii definitions 
 *  ------------------------------
 *  
 *     EQNAME {
 *        "LEC expression := LEC expression"
 *        [COMMENT "text" ]       free comment
 *        [{LSQ|ZELLNER|INF|GLS}] estimation method
 *        [SAMPLE from to]                estimation sample
 *        [DATE yyyymmdd]                 date of estimation
 *        [BLOC "eq1;eq2;..."]            bloc of simultaneous estimated equations
 *        [INSTRUMENTS "lec1;lec2..."]    instruments for INF method
 *        [STDEV double]                  standard deviation
 *        [MEANY double]                  mean of left side term 
 *        [SSRES double]                  Sum of squares of residuals
 *        [STDERR double]                 Standard error
 *        [FSTAT double]                  F-Stat
 *        [R2 double]                     R-square
 *        [R2ADJ double]                  R-square adj
 *        [DW double]                     Durbin-Watson : 
 *        [LOGLIK double]                 Log likelihood : 
 *     }
 *  
 *  Example
 *  -------
 *  
 *  
 *    ACAF {
 *      	"(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+
 *             acaf4*(TIME=1995)"
 *      	LSQ
 *      	BLOCK "ACAF"
 *      	SAMPLE 1980Y1 1996Y1
 *      	DATE 19980612
 *      	STDEV 0.00426990026608109
 *      	MEANY 0.00818466581404209
 *      	SSRES 5.19944878760725e-05
 *      	STDERR 0.00192714610602707
 *      	FSTAT 32.273193359375
 *      	R2 0.8217613697052
 *      	R2ADJ 0.796298682689667
 *      	DW 2.32934594154358
 *      	LOGLIK 83.8075256347656
 *    }
 *    
 *    ACAG {
 *      	"ACAG := ACAG[-1]+r VBBP[-1]+(0.006*VBBP[-1]*(TIME=2001)-0.008*(TIME=2008))"
 *      	LSQ
 *      	BLOCK "ACAG"
 *      	COMMENT " "
 *     }
 *  
 *  
 *  Errors are displayed by a call to the function kerror().
 *  For each read equation, kmsg() is called to send a message to the user. 
 *  
 *  The implementations of kerror() and kmsg() depend on the context.
 *  
 *  @param [in] filename    char*   ascii file to be read or
 *                                  string containing the definition of the EQ 
 *  @return                 KDB*    NULL or allocated KDB of EQs
 *  
 */
KDB *KE_load_asc(char* filename)
{

    KDB     *kdb = NULL;
    EQ      *eq = NULL;
    YYFILE  *yy;
    char    name[K_MAX_NAME + 1];
    int     cmpt = 0, pos, K_compare();

    /* INIT YY READ */
    YY_CASE_SENSITIVE = 1;
    qsort(KE_TABLE, sizeof(KE_TABLE)/sizeof(YYKEYS), sizeof(YYKEYS), K_compare);
    SCR_strip(filename);
    yy = YY_open(filename, KE_TABLE, sizeof(KE_TABLE)/sizeof(YYKEYS),
                 (!K_ISFILE(filename)) ? YY_STDIN : YY_FILE);
    if(yy == 0) {
        kerror(0, "Cannot open '%s'", filename);
        return(kdb);
    }

    /* READ FILE */
    kdb = K_create(K_EQS, K_UPPER);
    while(1) {
        switch(YY_read(yy)) {
            case YY_COMMENT :
                break;
            case YY_EOF :
                YY_close(yy);
                return(kdb);

            case YY_WORD :
                yy->yy_text[K_MAX_NAME] = 0;
                strcpy(name, yy->yy_text);
                if((eq = KE_read_eq(yy)) == NULL) {
                    kerror(0, "%s : equation not defined", YY_error(yy));
                    goto err;
                }

                if(eq->blk == NULL) eq->blk = SCR_stracpy(name);

                pos = K_add(kdb, name, eq, name);
                if(pos < 0)  {
                    kerror(0, "%s : %s", name, L_error());
                    if(pos == -1) goto err;
                }
                kmsg("Reading object %d : %s", ++cmpt, name);
                E_free(eq);
                break;

            default :
                kerror(0, "Incorrect entry : %s", YY_error(yy));
                break;
        }
    }

err:
    K_free(kdb);
    YY_close(yy);
    return((KDB *) 0);
}


/**
 *  Prints the value of a statistical test on fd. 
 *  If the value of the test is 0 or NaN , does not print anything. 
 *  
 *  @param [in, out]    fd      FILE*       output stream    
 *  @param [in]         txt     char*       keyword of the test (ex. "DW")
 *  @param [in]         val     IODE_REAL   value of the stat test
 *  @return 
 *  
 */

static void KE_print_test(FILE* fd, char* txt, IODE_REAL val)
{
    if(val != 0 && L_ISAN(val)) fprintf(fd, "\t%s %.15lg\n", txt, val); /* JMP 09-04-98 */
}


/**
 *  Saves on the stream fd the ascii representation of one EQ.

 *  @see KE_read_eq() the detailed syntax of an ascii equation. 
 *  
 *  @param [in, out]    fd      FILE*       output stream    
 *  @param [in]         eq      EQ*         pointer to the EQ to print  
 *  
 */
static void KE_print_eq(FILE* fd, EQ* eq)
{
    char    from[21], to[21], *SCR_long_to_date();

    fprintf(fd, "{\n\t\"%s\"\n\t", eq->lec);
    switch(eq->method) {
        case 0 :
            fprintf(fd, "LSQ\n");
            break;
        case 1 :
            fprintf(fd, "ZELLNER\n");
            break;
        case 2 :
            fprintf(fd, "INF\n");
            break;
        case 3 :
            fprintf(fd, "GLS\n");
            break;
        case 4 :
            fprintf(fd, "MAXLIK\n");
            break;
    }
    if(eq->blk != NULL && eq->blk[0] != 0)
        fprintf(fd, "\tBLOCK \"%s\"\n", eq->blk);
    if(eq->instr != NULL && eq->instr[0] != 0)
        fprintf(fd, "\tINSTRUMENTS \"%s\"\n", eq->instr);
    if(eq->cmt != NULL && eq->cmt[0] != 0) { // JMP 9/4/2015
        fprintf(fd, "\tCOMMENT ");
        SCR_fprintf_esc(fd, eq->cmt, 1);
        fprintf(fd, "\n");
    }
    if((eq->smpl).s_nb != 0)
        fprintf(fd, "\tSAMPLE %s %s\n",
                PER_pertoa(&(eq->smpl.s_p1), from),
                PER_pertoa(&(eq->smpl.s_p2), to)
               );
    if(eq->date != 0L)
        fprintf(fd, "\tDATE %ld\n", eq->date);
    KE_print_test(fd, "STDEV",  eq->tests[1]);
    KE_print_test(fd, "MEANY",  eq->tests[2]);
    KE_print_test(fd, "SSRES",  eq->tests[3]);
    KE_print_test(fd, "STDERR", eq->tests[4]);
    KE_print_test(fd, "FSTAT",  eq->tests[6]);
    KE_print_test(fd, "R2",     eq->tests[7]);
    KE_print_test(fd, "R2ADJ",  eq->tests[8]);
    KE_print_test(fd, "DW",     eq->tests[9]);
    KE_print_test(fd, "LOGLIK", eq->tests[10]);
    fprintf(fd, "}\n");
}


/**
 *  Saves a KDB of EQs into an ascii file (.ae) or to the stdout.
 *  
 *  @see KE_load_asc() for the syntax. 
 *  
 *  @param [in] kdb         KDB*    KDB of EQs
 *  @param [in] filename    char*   name of the output file or "-" to write the result on the stdout.
 *  @return                 int     0 on success, -1 if the file cannot be written.
 *  
 */
int KE_save_asc(KDB* kdb, char* filename)
{
    FILE    *fd;
    int     i;
    EQ      *eq;

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
        eq = KEVAL(kdb, i);
        KE_print_eq(fd, eq);
        E_free(eq);
    }

    if(filename[0] != '-') fclose(fd);
    return(0);
}



/* 
 * Save a KDB of EQs in a .csv file.
 * NOT IMPLEMENTED.
 */
int KE_save_csv(KDB *kdb, char *filename)
{
    return(-1);
}
