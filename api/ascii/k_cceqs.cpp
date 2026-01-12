/**
 * @header4iode
 * 
 * Functions to load and save ascii definitions of IODE EQ objects.
 *
 *     bool load_asc(const std::string& filename)
 *     bool save_asc(const std::string& filename)
 *
 */
#include "api/constants.h"
#include "api/b_errors.h"
#include "api/k_super.h"
#include "api/utils/utils.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/equations.h"
#include "api/ascii/ascii.h"

/**
 * Table of keywords recognized by YY in the context of an EQS ascii file (.ae).
 * See s_yy function group for more informations (http://www.xon.be/scr4/libs1/libs157.htm).
 */
 
YYKEYS KE_TABLE[] = {
    (unsigned char*) "{",            EQ_ASCII_OPEN,
    (unsigned char*) "}",            EQ_ASCII_CLOSE,
    (unsigned char*) "BLOCK",        EQ_ASCII_BLK,
    (unsigned char*) "COMMENT",      EQ_ASCII_CMT,
    (unsigned char*) "DATE",         EQ_ASCII_DATE,
    (unsigned char*) "INF",          EQ_ASCII_INF,
    (unsigned char*) "INSTRUMENTS",  EQ_ASCII_INSTR,
    (unsigned char*) "GLS",          EQ_ASCII_GLS,
    (unsigned char*) "LSQ",          EQ_ASCII_LSQ,
    (unsigned char*) "SAMPLE",       EQ_ASCII_SMPL,
    (unsigned char*) "ZELLNER",      EQ_ASCII_ZEL,
    (unsigned char*) "MAXLIK",       EQ_ASCII_MAXLIK,

    (unsigned char*) "STDEV",        EQ_ASCII_STDEV,
    (unsigned char*) "MEANY",        EQ_ASCII_MEANY,
    (unsigned char*) "SSRES",        EQ_ASCII_SSRES,
    (unsigned char*) "STDERR",       EQ_ASCII_STDERR,
    (unsigned char*) "FSTAT",        EQ_ASCII_FSTAT,
    (unsigned char*) "R2",           EQ_ASCII_R2,
    (unsigned char*) "R2ADJ",        EQ_ASCII_R2ADJ,
    (unsigned char*) "DW",           EQ_ASCII_DW,
    (unsigned char*) "LOGLIK",       EQ_ASCII_LOGLIK
};


/**
 *  Reads on an opened YY stream (file or string) the ascii definition of an IODE EQ. 
 *  Subfn of load_asc().
 *   
 *  Splits the LEC expression in multiple lines of max 60 chars.
 *  
 *  @param [in, out]    yy      YY*     YY stream to read data from
 *  @return                     Equation*     a new allocated EQ or NULL on error
 *  
 */
static Equation* read_eq(YYFILE* yy, char* name)
{
    int         keyw;
    char*       c_lec;
    Sample*     smpl = NULL;
    long        date = 0L;
    std::string block;
    std::string comment;
    std::string instruments;
    float       tests[EQS_NBTESTS] = { 0.0f };

    std::string endo(name);
    IodeEquationMethod method = EQ_LSQ;
    
    if((keyw = YY_lex(yy)) != EQ_ASCII_OPEN)  
    {
        YY_unread(yy);
        c_lec = K_read_str(yy);
        c_lec = K_wrap(c_lec, 60);  
        std::string lec(c_lec);
        SW_nfree(c_lec);

        if(lec.empty()) 
        {
            kerror(0, "%s: Expected LEC definition for equation '%s', [%s]", 
                   (char*) lec.c_str(), name, YY_error(yy));
            return nullptr;
        }
        
        Equation* eq = new Equation(endo, lec, method, "", "", "", "", "", false);
        return eq;
    }

    c_lec = K_read_str(yy);
    c_lec = K_wrap(c_lec, 60);
    std::string lec(c_lec);
    SW_nfree(c_lec);

    if(lec.empty()) 
    {
        kerror(0, "%s: Expected LEC definition for equation '%s', [%s]", 
               (char*) lec.c_str(), name, YY_error(yy));
        return nullptr;
    }

    bool exit = false;
    while(!exit) 
    {
        keyw = YY_lex(yy);
        switch(keyw) {
            case YY_WORD :
            case YY_EOF  :
                YY_unread(yy);
            case EQ_ASCII_CLOSE:
                exit = true;
                break;
            case EQ_ASCII_DATE :
                date = K_read_long(yy);
                break;

            case EQ_ASCII_LSQ :
                method = EQ_LSQ;
                break;
            case EQ_ASCII_ZEL :
                method = EQ_ZELLNER;
                break;
            case EQ_ASCII_INF :
                method = EQ_INSTRUMENTAL;
                break;
            case EQ_ASCII_GLS :
                method = EQ_GLS;
                break;
            case EQ_ASCII_MAXLIK :
                method = EQ_MAX_LIKELIHOOD;
                break;

            case EQ_ASCII_SMPL :
                smpl = K_read_smpl(yy);
                if(smpl == NULL)
                {
                    kerror(0, "Failed to read sample for equation '%s', [%s]", name, YY_error(yy));
                    return nullptr;
                }
                break;
            case EQ_ASCII_BLK  :
                block = std::string(K_read_str(yy));
                if(block.empty()) 
                {
                    kerror(0, "Missing block value for equation '%s', [%s]", name, YY_error(yy));
                    return nullptr;
                }
                break;
            case EQ_ASCII_CMT  :
                comment = std::string(K_read_str(yy));
                if(comment.empty()) 
                {
                    kerror(0, "Missing comment value for equation '%s', [%s]", name, YY_error(yy));
                    return nullptr;
                }
                break;
            case EQ_ASCII_INSTR:
                instruments = std::string(K_read_str(yy));
                if(instruments.empty()) 
                {
                    kerror(0, "Missing instruments value for equation '%s', [%s]", name, YY_error(yy));
                    return nullptr;
                }
                break;

            case EQ_ASCII_STDEV :
                tests[EQ_STDEV] = (float) K_read_real(yy);
                break;
            case EQ_ASCII_MEANY :
                tests[EQ_MEANY] = (float) K_read_real(yy);
                break;
            case EQ_ASCII_SSRES :
                tests[EQ_SSRES] = (float) K_read_real(yy);
                break;
            case EQ_ASCII_STDERR :
                tests[EQ_STDERR] = (float) K_read_real(yy);
                break;
            case EQ_ASCII_FSTAT :
                tests[EQ_FSTAT] = (float) K_read_real(yy);
                break;
            case EQ_ASCII_R2 :
                tests[EQ_R2] = (float) K_read_real(yy);
                break;
            case EQ_ASCII_R2ADJ :
                tests[EQ_R2ADJ] = (float) K_read_real(yy);
                break;
            case EQ_ASCII_DW :
                tests[EQ_DW] = (float) K_read_real(yy);
                break;
            case EQ_ASCII_LOGLIK :
                tests[EQ_LOGLIK] = (float) K_read_real(yy);
                break;

            default :
                kerror(0, "Incorrect entry for equation '%s':\n%s", name, YY_error(yy));
                break;
        }
    }

    Period from_period = (smpl != NULL) ? smpl->start_period : Period();
    Period to_period = (smpl != NULL) ? smpl->end_period : Period();
    Equation* eq = new Equation(endo, lec, method, from_period.to_string(), 
                                to_period.to_string(), comment, instruments, block, false);
    memcpy(eq->tests.data(), tests, EQS_NBTESTS * sizeof(float));
    eq->date = date;
    return eq;
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
 *        [Sample from to]                estimation sample
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
 *  @param [in] filename     char*   ascii file to be read or
 *                                   string containing the definition of the EQ 
*   @param [in]   db_global  int     1 for DB_GLOBAL, 0 for DB_STANDALONE
 *  @return                  KDB*    NULL or allocated KDB of EQs
 *  
 */
bool CKDBEquations::load_asc(const std::string& filename)
{
    bool       success;
    Equation*  eq = NULL;
    YYFILE*    yy;
    char       name[K_MAX_NAME + 1];
    int        cmpt = 0;

    /* INIT YY READ */
    YY_CASE_SENSITIVE = 1;
    qsort(KE_TABLE, sizeof(KE_TABLE)/sizeof(YYKEYS), sizeof(YYKEYS), ascii_eqs_compare);

    std::string trim_filename = trim(filename);
    char* c_filename = (char*) trim_filename.c_str();
    yy = YY_open(c_filename, KE_TABLE, sizeof(KE_TABLE)/sizeof(YYKEYS),
                 (!K_ISFILE(c_filename)) ? YY_STDIN : YY_FILE);
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
        switch(YY_read(yy)) 
        {
            case YY_COMMENT :
                break;
            case YY_EOF :
                if(cmpt) 
                {
                    char asc_filename[1024];
                    K_set_ext_asc(asc_filename, c_filename, EQUATIONS);
                    K_set_kdb_fullpath(this, (U_ch*) asc_filename); // JMP 03/12/2022
                }
                YY_close(yy);
                return true;

            case YY_WORD :
                yy->yy_text[K_MAX_NAME] = 0;
                strcpy(name, (char*) yy->yy_text);
                eq = read_eq(yy, name);
                if(eq == nullptr) 
                {
                    kwarning("%s : equation not defined", YY_error(yy));
                    goto err;
                }

                if(eq->block.empty()) 
                    eq->block = std::string(name);

                success = this->set_obj(name, eq);
                if(!success)  
                {
                    kwarning("%s : %s", name, L_error());
                    goto err;
                }
                else
                    cmpt++;
                
                kmsg("Reading object %d : %s", cmpt, name);
                delete eq;
                eq = nullptr;
                break;

            default :
                kwarning("Incorrect entry : %s", YY_error(yy));
                break;
        }
    }

err:
    YY_close(yy);
    return false;
}

/**
 *  Prints the value of a statistical test on fd. 
 *  If the value of the test is 0 or NaN , does not print anything. 
 *  
 *  @param [in, out]    fd      FILE*       output stream    
 *  @param [in]         txt     char*       keyword of the test (ex. "DW")
 *  @param [in]         val     double   value of the stat test
 *  @return 
 *  
 */

static void print_test(FILE* fd, char* txt, double val)
{
    if(val != 0 && IODE_IS_A_NUMBER(val)) fprintf(fd, "\t%s %.8lg\n", txt, val); /* JMP 05-09-2022 : .15 -> .8 */
    //if(val != 0 && IODE_IS_A_NUMBER(val)) fprintf(fd, "\t%s %lg\n", txt, val); /* JMP 05-09-2022 */
}

/**
 *  Saves on the stream fd the ascii representation of one EQ.

 *  @see read_eq() the detailed syntax of an ascii equation. 
 *  
 *  @param [in, out]    fd      FILE*       output stream    
 *  @param [in]         eq      Equation*         pointer to the EQ to print  
 *  @param [in]         name    char*       equation endo (i.e. equation name)
 *  
 */
static void print_eq(FILE* fd, Equation* eq, char* name)
{
    fprintf(fd, "{\n\t\"%s\"\n", (char*) eq->lec.c_str());

    std::string comment = trim(eq->comment);
    if(!comment.empty()) 
    {
        fprintf(fd, "\tCOMMENT ");
        SCR_fprintf_esc(fd, (char*) comment.c_str(), 1);
        fprintf(fd, "\n");
    }

    // Estimated equation => Sample not null 30/10/2023
    if(eq->sample.nb_periods != 0) 
    {
        switch(eq->method) {
            case 0 :
                fprintf(fd, "\tLSQ\n");
                break;
            case 1 :
                fprintf(fd, "\tZELLNER\n");
                break;
            case 2 :
                fprintf(fd, "\tINF\n");
                break;
            case 3 :
                fprintf(fd, "\tGLS\n");
                break;
            case 4 :
                fprintf(fd, "\tMAXLIK\n");
                break;
            default: 
                fprintf(fd, "\t/* UNKNOWN ESTIMATION METHOD: %d*/\n", eq->method);
                break;
            }
        
        std::string from = eq->sample.start_period.to_string();
        std::string to   = eq->sample.end_period.to_string();
        fprintf(fd, "\tSAMPLE %s %s\n", (char*) from.c_str(), (char*) to.c_str());

        if((!eq->block.empty()) && eq->block != std::string(name))
            fprintf(fd, "\tBLOCK \"%s\"\n", eq->block.c_str());
        
        if(!eq->instruments.empty())
            fprintf(fd, "\tINSTRUMENTS \"%s\"\n", eq->instruments.c_str());
        
        //if(eq->date != 0L)
        fprintf(fd, "\tDATE %ld\n", eq->date);

        print_test(fd, "STDEV",  eq->tests[EQ_STDEV]);
        print_test(fd, "MEANY",  eq->tests[EQ_MEANY]);
        print_test(fd, "SSRES",  eq->tests[EQ_SSRES]);
        print_test(fd, "STDERR", eq->tests[EQ_STDERR]);
        print_test(fd, "FSTAT",  eq->tests[EQ_FSTAT]);
        print_test(fd, "R2",     eq->tests[EQ_R2]);
        print_test(fd, "R2ADJ",  eq->tests[EQ_R2ADJ]);
        print_test(fd, "DW",     eq->tests[EQ_DW]);
        print_test(fd, "LOGLIK", eq->tests[EQ_LOGLIK]);
    }
    
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
bool CKDBEquations::save_asc(const std::string& filename)
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

    Equation* eq;
    for(auto& [name, _] : k_objs) 
    {
        fprintf(fd, "%s ", name.c_str());
        eq = this->get_obj(name);
        print_eq(fd, eq, (char*) name.c_str());
        delete eq;
        eq = nullptr;
    }

    if(filename[0] != '-') 
        fclose(fd);

    return true;
}
