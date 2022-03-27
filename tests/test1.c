/**
 *  @header4iode
 *  
 *  IODE API tests.
 *
 *  See function Syntax() .
 */


#include <stdarg.h>
#include "iode.h"

void Syntax() 
{
    printf("Syntax: test1 [-v] [-v-] [-x] [-x-] [-h]\n");
    printf("  [-v]  : verbose (default)\n");
    printf("  [-v-] : silent\n");
    printf("  [-x]  : exit on error (default)\n");
    printf("  [-x-] : continue on error\n");
    printf("  [-h]  : display syntax\n");
    exit(0);
}


#ifdef _MSC_VER
    char    *IODE_DATA_DIR   = "data";
    char    *IODE_OUTPUT_DIR = "output";
#else
    char    *IODE_DATA_DIR   = "..\\fun";
    char    *IODE_OUTPUT_DIR = ".";
#endif


// Fonctions annulées/remplacées temporairement pour passer le link
int W_printf(char* fmt,...)
{
    va_list     myargs;
    char        buf[512];
    
    va_start(myargs, fmt);
#ifdef _MSC_VER   
    vsnprintf_s(buf, sizeof(buf) - 1, _TRUNCATE, fmt, myargs);
#else
    vsnprintf_s(buf, sizeof(buf) - 1, fmt, myargs);
#endif    
    va_end(myargs);
    printf("%s\n", buf);
    return(0);
}

/**
 *  Fonctions et vars de la lib S4ASSERT 
 *  ------------------------------------ 
 *      int U_cmp_str(char* str1, char* str2)  : compares 2 strings and return(1) if equal
 *      int U_cmp_tbl(char** tbl1, char* vec)  : Compares a table of strings to a list of strings in a semi-colon separated vector.
 *      void S4ASSERT(int success, char* fmt, ...)   : Verifies an assertion, optionally displays a message and opt. exits on error.
 *  
 *      int S4ASSERT_VERBOSE            if not null, display all messages, eve, on success
 *      int S4ASSERT_EXIT_ON_ERROR      if not null, exits on the first not satisfied assertion 
 *
 */ 
int S4ASSERT_VERBOSE = 1;           // if not null, display all messages, eve, on success
int S4ASSERT_EXIT_ON_ERROR = 1;     // if not null, exits on the first not satisfied assertion 

/**
 *  Compares 2 strings. 
 *  
 *  Ex. 
 *      U_cmp_str("A B C", "A;B;C") => returns 0 => NOK
 *      U_cmp_str("A B",   "A B")   => returns 1 => OK
 *  
 *  @param [in] str1    char*   string  
 *  @param [in] str2    char*   string 
 *  @return             int     1 if the content of str1 == that of str2, 0 if not
 *  
 */
int U_cmp_str(char* str1, char* str2)
{
    if(str1 == NULL && str2 == NULL) return(1);
    if(str1 == NULL || str2 == NULL) return(0);
    return(!strcmp(str1, str2));
}

/**
 *  Compares a table of strings to a list of strings in a semi-colon separated vector.
 *  
 *  Ex. 
 *      U_cmp_tbl((char**)SCR_vtom("A B C", ' '), "A;B;C") => return(1) => OK
 *      U_cmp_tbl((char**)SCR_vtom("A B", ' '),   "A;B;C") => return(0) => NOK
 *  
 *  @param [in] tbl1    char**  table of strings    
 *  @param [in] vec     char*   semi-colon sep vector
 *  @return             int     0 if tbl1 and vec are equivalent, -1 else
 *  
 */
int U_cmp_tbl(char** tbl1, char* vec)
{
    int     i, rc = 0;
    char**  tbl2;

    tbl2 = (char**)SCR_vtoms((unsigned char*)vec, " ;,");
    if(tbl1 == NULL) {
        if(tbl2 == NULL) return(-1);
        goto fin;
    }    
    if(tbl2 == NULL) return(0); 
    if(SCR_tbl_size((unsigned char**)tbl1) != SCR_tbl_size((unsigned char**)tbl2)) goto fin;
    for(i = 0 ;  tbl1[i] ; i++)
        if(strcmp(tbl1[i], tbl2[i])) goto fin;
    rc = -1;
    
fin:
    SCR_free_tbl((unsigned char**)tbl2);
    return(rc);

}

void kmsg_null(char*msg) {
}


/**
 *  Verifies an assertion. 
 *  Displays a message or not according to S4ASSERT_VERBOSE.
 *  Exits on error if S4ASSERT_EXIT_ON_ERROR.
 *  
 *  @param [in] success int     1 = assertion true, 0 false
 *  @param [in] fmt     char*   format of the message (including %s, %d...)
 *  @param [in] ...             optional params of fmt (%s, %d...)
 *  
 */
void S4ASSERT(int success, char* fmt, ...)
{
    char    buf[256];
    va_list myargs;

    va_start(myargs, fmt);
#ifdef _MSC_VER   
    vsnprintf_s(buf, sizeof(buf) - 1, _TRUNCATE, fmt, myargs);
#else
    vsnprintf_s(buf, sizeof(buf) - 1, fmt, myargs);
#endif    
    va_end(myargs);

    if(success && !S4ASSERT_VERBOSE) return; // No output if success but non verbose
    
    if(success) 
        printf("Passed: %s\n", buf);
    else {
        printf("Error:  %s\n", buf);
        if(S4ASSERT_EXIT_ON_ERROR) {
            exit(1);
            printf("Tests stopped\n");
        }    
    }    
}


/******** IODE TESTS ******************/

void Tests_BUF()
{
    S4ASSERT(BUF_DATA == NULL,            "BUF_DATA is null");
    S4ASSERT(BUF_strcpy("ABCD") != NULL,  "BUF_strcpy(\"ABCD\") is not null");
    S4ASSERT(BUF_alloc(100) != NULL,      "BUF_alloc(100) is not null");
    S4ASSERT(BUF_strcpy("ABCD") != NULL,  "BUF_strcpy(\"ABCD\") is not null");
    BUF_free();
    S4ASSERT(BUF_DATA == NULL,            "BUF_free() => BUF_DATA is null");
}
    
/**
 *  Tests some object functions & macros
 *      - k_add() : LIST, VAR
 *      - KLPTR()
 *      - KV_sample()
 */
void Tests_Objects()
{
    char*       lst;
    SAMPLE*     smpl;
    IODE_REAL   A[64], B[64];
    int         nb, i, pos;
    
    // Create lists
    pos = K_add(KL_WS, "LST1", "A;B");
    S4ASSERT(pos >= 0,                    "K_add(\"LST1\") = %d", pos);
    K_add(KL_WS, "LST2", "A,B");
    lst = KLPTR("LST1");
    S4ASSERT(strcmp(lst, "A;B") == 0,     "KLPTR(\"LST1\") = \"%s\"", lst);

    // Set the sample for the variable WS
    smpl = PER_atosmpl("2000Y1", "2020Y1");
    KV_sample(KV_WS, smpl);
    //SW_nfree(smpl);
    
    // Creates new vars
    nb = smpl->s_nb;
    for(i = 0; i < smpl->s_nb; i++) {
       A[i] = i;
       B[i] = i*2;
    }
    
    pos = K_add(KV_WS, "A", A, &nb);
    S4ASSERT(K_find(KV_WS, "A") >= 0,  "K_add() + K_find()");
    pos = K_add(KV_WS, "B", B, &nb);

}    

/**
 *  Tests (some) LEC functions:
 *      - L_cc()
 *      - L_link()
 *      - L_exec()
 */
void U_test_lec(char* title, char* lec, int t, IODE_REAL expected_val)
{
    CLEC*   clec;
    //char    buf[256];
    double  calc_val;
    PERIOD  *per;
    char    aper[24];
    
    per = PER_addper(&(KSMPL(KV_WS)->s_p1), t);
    PER_pertoa(per, aper);
    
    clec = L_cc(lec);
    //S4ASSERT ("L_cc", clec != 0);
    //S4ASSERT ("L_link", 0 == L_link(KV_WS, KS_WS, clec));
    L_link(KV_WS, KS_WS, clec);
    calc_val = L_exec(KV_WS, KS_WS, clec, t);
    //sprintf(buf, "Res=%10.3lf - Expected=%10.3lf %s L_exec(%s) in %s", calc_val, expected_val, title, lec, aper);
    S4ASSERT (expected_val == calc_val, "Res=%10.3lf - Expected=%10.3lf %s L_exec(%s) in %s", calc_val, expected_val, title, lec, aper);
}

/**
 *  Tests some LEC calculations.
 */
void Tests_LEC()
{
    IODE_REAL *A, *B;
    
    A = (IODE_REAL*)KVPTR("A");
    B = (IODE_REAL*)KVPTR("B");
    // Tests LEC
    U_test_lec("LEC", "A+B",  2, A[2]+B[2]);
    U_test_lec("LEC", "ln A", 2, log(A[2]));
    U_test_lec("LEC", "A[2002Y1]",     2, A[2]);
    //S4ASSERT(0, "Erreur forcée");
    U_test_lec("LEC", "A[2002Y1][-1]", 2, A[2]);
    U_test_lec("LEC", "A[-1]",         2, A[1]);
    U_test_lec("LEC", "A[-1][2002Y1]", 2, A[1]);
    U_test_lec("LEC", "sum(2000Y1, 2010Y1, A)", 2, 55.0);
    U_test_lec("LEC", "sum(2000Y1, A)", 2, 3.0);
    
    // Using macros in LEC 
    U_test_lec("LEC-MACRO", "1 + vmax($LST1)", 2, 1+B[2]);
    U_test_lec("LEC-MACRO", "1 + vmax($LST2)", 2, 1+B[2]);  
}


/**
 *  Tests Equation creation.
 */
void Tests_EQS()
{
//    EQ*     eq;
//    char    lec[521];  
//
//    B_DataUpdateEqs("A", "ln A := B + t", NULL, 'L', NULL, NULL, NULL, NULL, NULL);
//    eq = KEPTR("A");
//    strcpy(lec, eq->lec);
//    S4ASSERT(strcmp(eq->lec, "ln A := B + t") == 0, "EQ %s = %s", "A", lec);
//
}

/**
 *  Tests argument expansion
 */
void Tests_ARGS()
{
    char **args;
    char *list[] = {"A1", "A2", 0};
    char filename[256];
    
    // A_init
    args = B_ainit_chk("$LST1", NULL, 0);
    S4ASSERT(U_cmp_tbl(args, "A;B"), "B_ainit_chk(\"$LST1\")");
    SCR_free_tbl(args);
    //args = B_ainit_chk("A*", NULL, 0);
    
    // Test parameters in a file. test.args must exist in the current dir and contain the line
    // A1 A2
    sprintf(filename, "@%s\\test.args", IODE_DATA_DIR);
    args = B_ainit_chk(filename, NULL, 0);
    S4ASSERT(U_cmp_tbl(args, "A1;A2"), "B_ainit_chk(\"%s\")", filename);
    SCR_free_tbl(args);
}

void Tests_ERRMSGS() 
{
    B_seterrn(86, "bla bla");
    kerror(0, "Coucou de kerror %s", "Hello");
    kmsg("Coucou de kmsg %s -- %g", "Hello", 3.2);
}
    
    
void Tests_K_OBJFILE()
{
    char    in_filename[256];
    char    out_filename[256];
    KDB     *kdb_var;
    int     rc;
    
    sprintf(in_filename,  "%s\\fun.var", IODE_DATA_DIR);
    sprintf(out_filename, "%s\\fun_copy.var", IODE_OUTPUT_DIR);
    
    kdb_var = K_interpret(K_VAR, in_filename);
    S4ASSERT(kdb_var != NULL, "K_interpret(K_VAR, \"%s\")", in_filename);
    if(kdb_var) {
        S4ASSERT(KNB(kdb_var) == 394, "KNB(\"%s\") == 394", in_filename);
        rc = K_save(kdb_var, out_filename);
        S4ASSERT(rc == 0, "K_save(kdb_var, \"%s\") == 0", out_filename);
    }
    
   
    /*
    char *K_set_ext(char* res, char* fname, int type)                               deletes left and right spaces in a filename and changes its extension according to the given type.
    void K_strip(char* filename)                                                    deletes left and right spaces in a filename. Keeps the space inside the filename.
    KDB  *K_load(int ftype, FNAME fname, int no, char** objs)                       loads a IODE object file. 
    int K_filetype(char* filename, char* descr, int* nobjs, SAMPLE* smpl)           retrieves infos on an IODE file: type, number of objects, SAMPLE
    KDB *K_interpret(int type, char* filename): generalisation of K_load()          interprets the content of a file, ascii files included, and try to load ist content into a KDB.
    int K_copy(KDB* kdb, int nf, char** files, int no, char** objs, SAMPLE* smpl)   reads a list of objects from a list of IODE object files and adds them to an existing KDB.
    int K_backup(char* filename)                                                    takes a backup of a file by renaming the file: filename.xxx => filename.xx$.
    int K_save(KDB* kdb, FNAME fname)                                               saves a KDB in an IODE object file. The extension of fname is replaced by the standard one (.cmt, .eqs...).
    int K_save_ws(KDB* kdb)                                                         saves a KDB in an IODE object file called "ws.<ext>" where <ext> is one of (.cmt, .eqs...).
    int K_setname(char* from, char* to)                                             replaces KNAMEPTR(kdb) in an IODE object file.
    */
}
    

/**
 *  Checks:
 *      - K_interpret(): conversion 32/64 bits
 *      - KT_save_asc()
 *      - macro KTVAL()
 *      - T_cell_cont()en 32 and 64 bits
 */    
void Tests_TBL32_64()
{
    char    in_filename[256];
    char    out_filename[256];
    KDB     *kdb_tbl;
    int     rc;
    
    int     pos, col;
    TBL*    c_table;
    TCELL*  cells;
    char    *cell_content;

    
    sprintf(in_filename,  "%s\\fun.tbl", IODE_DATA_DIR);
   
    kdb_tbl = K_interpret(K_TBL, in_filename);
    S4ASSERT(kdb_tbl != NULL, "K_interpret(K_TBL, \"%s\")", in_filename);
    if(kdb_tbl) {
        sprintf(out_filename, "%s\\fun_copy.at", IODE_OUTPUT_DIR);
        rc = KT_save_asc(kdb_tbl, out_filename);
        S4ASSERT(rc == 0, "KT_save_asc(kdb_tbl, \"%s\") == %d", out_filename, rc);
    }
    
    // Plantage ALD 14/02/2022   
    pos = K_find(kdb_tbl, "GFRPC");
    c_table = KTVAL(kdb_tbl, pos);

    // divider
    cells = (TCELL*) c_table->t_div.tl_val;
    printf("Address(cells) =     %0x\nAddress(cells + 1) = %0x\n", cells, cells + 1);
    printf("Diff(cells, cells+1) = %d\n", (char*)(cells + 1) - (char*)(cells));
    
    for(col = 0; col < c_table->t_nc; col++) {
        cell_content = T_cell_cont(&cells[col], 1);
        printf("Cell %d:%s\n",col, cell_content);
    }
}

KDB* Test_K_interpret(int type, char* filename)
{
    char    fullfilename[256];
    KDB     *kdb;
    
    sprintf(fullfilename,  "%s\\%s", IODE_DATA_DIR, filename);
    kdb = K_interpret(type, fullfilename);
    //S4ASSERT(kdb != NULL, "K_interpret(%d, \"%s\")", type, fullfilename);
    return(kdb);
}


void Tests_Simulation()
{
    KDB     *kdbv, 
            *kdbe, 
            *kdbs;
    SAMPLE  *smpl;
    char    *filename = "fun";
    int     rc;
    LIS     lst, expected_lst;
       
    
    // Loads 3 WS and check ok
    K_WS[K_VAR] = kdbv  = Test_K_interpret(K_VAR, filename);
    S4ASSERT(kdbv != NULL, "K_interpret(K_VAR, \"%s\")", filename);
    
    K_WS[K_SCL] = kdbs  = Test_K_interpret(K_SCL, filename);
    S4ASSERT(kdbs != NULL, "K_interpret(K_SCL, \"%s\")", filename);
    
    K_WS[K_EQS] = kdbe  = Test_K_interpret(K_EQS, filename);
    S4ASSERT(kdbe != NULL, "K_interpret(K_EQS, \"%s\")", filename);

    
    // Check list is empty
    lst = KLPTR("_DIVER");
    S4ASSERT(U_cmp_str(lst, NULL), "_DIVER=NULL", lst);
    
    // Simulation std parameters
    smpl = PER_atosmpl("2000Y1", "2002Y1");
    KSIM_EPS = 0.0001;
    KSIM_MAXIT = 100;
    KSIM_RELAX = 0.7;
    KSIM_SORT = SORT_BOTH;
    KSIM_PASSES = 5; 
    KSIM_DEBUG = 1;

    kmsg_super = kmsg_null; // Void itearation messages during simulation
    
    // Test simulation : divergence
    KSIM_MAXIT = 2;
    rc = K_simul(kdbe, kdbv, kdbs, smpl, NULL, NULL);
    S4ASSERT(rc != 0, "K_simul() with only maxit=%d does not converge", KSIM_MAXIT);
    
    // Check _PRE list after simulation (prolog)
    lst = KLPTR("_PRE");
    expected_lst = "BRUGP;DTH1C;EX;ITCEE;ITCR;ITGR;ITI5R;ITIFR;ITIGR;ITMQR;NATY;POIL;PW3;PWMAB;PWMS;PWXAB;PWXS;PXE;QAH;QWXAB;QWXS;QWXSS;SBGX;TFPFHP_;TWG;TWGP;ZZF_;DTH1;PME;PMS;PMT";
    //printf("     '%s'(%d)\n", expected_lst, strlen(expected_lst));
    S4ASSERT(U_cmp_str(lst, expected_lst), "_PRE=\"%s\"", lst);
    
    // Check _DIVER list
    lst = KLPTR("_DIVER");
    //printf("'%s'\n", lst);
    expected_lst = "SSH3O,WBG,SSF3,YDH,DTH,YDTG,YSFIC,WMIN,WLCP,WBGP,YSEFT2,YSEFT1,YSEFP,SBG,PWBG,W,ZJ,QMT,QI5,QC_,SSFG,YDH_,SG,ACAG,FLG";
    S4ASSERT(U_cmp_str(lst, expected_lst), "_DIVER=\"%s\"", lst);
    
    // Test with with convergence (increase MAXIT)
    KSIM_MAXIT = 100;
    rc = K_simul(kdbe, kdbv, kdbs, smpl, NULL, NULL);
    S4ASSERT(rc == 0, "K_simul() with maxit=%d does converge",KSIM_MAXIT);

    // Test Endo-exo
    


    // Cleanup
    SCR_free(smpl);
}


void Tests_ALIGN()
{
    TBL     tbl, *p_tbl = &tbl;
    int     offset;
    
    offset = (char*)(p_tbl + 1) - (char*)p_tbl;
    printf("sizeof(TBL)    = %d -- Offset = %d\n", sizeof(TBL), offset);
    //printf("sizeof(TBL)    = %d\n", sizeof(TBL));
    //printf("sizeof(TLINE)  = %d\n", sizeof(TLINE));
    //printf("sizeof(TCELL)  = %d\n", sizeof(TCELL));
    //                         
    //printf("sizeof(TBL32)  = %d\n", sizeof(TBL32));
    //printf("sizeof(TLINE32)= %d\n", sizeof(TLINE32));
    //printf("sizeof(TCELL32)= %d\n", sizeof(TCELL32));
}

/* ========================================================*/

int main(int argc, char **argv)
{
    int i;
    
    IODE_assign_super_API();
    strcpy(SCR_NAME_ERR, "iode.msg");

    for(i = 1 ; i < argc; i++) {
        if(strcmp(argv[i], "-v-") == 0) S4ASSERT_VERBOSE = 0;
        if(strcmp(argv[i], "-v")  == 0) S4ASSERT_VERBOSE = 1;
        if(strcmp(argv[i], "-x-") == 0) S4ASSERT_EXIT_ON_ERROR = 0;
        if(strcmp(argv[i], "-x") == 0)  S4ASSERT_EXIT_ON_ERROR = 1;
        if(strcmp(argv[i], "-h") == 0)   Syntax();
    }
    
    // Initialises 7 empty WS
    K_init_ws(0);

    // test B_seterrn()
    Tests_ALIGN();
    Tests_ERRMSGS();
    Tests_BUF();
    Tests_Objects();
    Tests_LEC();
    Tests_EQS();
    Tests_ARGS();
    Tests_K_OBJFILE();
    Tests_TBL32_64();
    Tests_Simulation();
    
//    B_ReportLine("$show coucou");
}


