/**
 *  @header4iode
 *  
 *  IODE API tests.
 *
 *  Syntax: test1 [-x[-]] [-v[-]]
 *      -x:     exits on error
 *      -x-:    continues on error (default)
 *      -v:     verbose (displays message also on success) (default)
 *      -v-:    don't display messages on success
 */

#include <stdarg.h>
#include "iode.h"

// Fonctions annulées temporairement pour passer le link
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
 * Fonctions et vars de la lib S4ASSERT 
 */ 
int S4ASSERT_VERBOSE = 1;
int S4ASSERT_EXIT_ON_ERROR = 0;

/**
 *  Compares a table of strings to a list of strings in a semi-colon separated vector.
 *  
 *  Ex. 
 *      SCR_cmp_tbl(SCR_vtom("A B C", ' '), "A;B;C") => return(1) => OK
 *      SCR_cmp_tbl(SCR_vtom("A B", ' '),   "A;B;C") => return(0) => NOK
 *  
 *  @param [in] tbl1    char**  table of strings    
 *  @param [in] vec     char*   semi-colon sep vector
 *  @return             int     0 if tbl1 and vec are equivalent, -1 else
 *  
 */
int S4ASSERT_cmp_tbl(char** tbl1, char* vec)
{
    int     i, rc = 0;
    char**  tbl2;

    tbl2 = SCR_vtoms(vec, " ;,");
    if(tbl1 == NULL) {
        if(tbl2 == NULL) return(-1);
        goto fin;
    }    
    if(tbl2 == NULL) return(0); 
    if(SCR_tbl_size(tbl1) != SCR_tbl_size(tbl2)) goto fin;
    for(i = 0 ;  tbl1[i] ; i++)
        if(strcmp(tbl1[i], tbl2[i])) goto fin;
    rc = -1;
    
fin:
    SCR_free_tbl(tbl2);
    return(rc);

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
    char*       lst, **args;
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
void TestLEC(char* title, char* lec, int t, IODE_REAL expected_val)
{
    CLEC*   clec;
    char    buf[256];
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
    TestLEC("LEC", "A+B",  2, A[2]+B[2]);
    TestLEC("LEC", "ln A", 2, log(A[2]));
    TestLEC("LEC", "A[2002Y1]",     2, A[2]);
    S4ASSERT(0, "Erreur forcée");
    TestLEC("LEC", "A[2002Y1][-1]", 2, A[2]);
    TestLEC("LEC", "A[-1]",         2, A[1]);
    TestLEC("LEC", "A[-1][2002Y1]", 2, A[1]);
    TestLEC("LEC", "sum(2000Y1, 2010Y1, A)", 2, 55.0);
    TestLEC("LEC", "sum(2000Y1, A)", 2, 3.0);
    
    // Using macros in LEC 
    TestLEC("LEC-MACRO", "1 + vmax($LST1)", 2, 1+B[2]);
    TestLEC("LEC-MACRO", "1 + vmax($LST2)", 2, 1+B[2]);  
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
Tests_ARGS()
{
    char **args;
    char *list[] = {"A1", "A2", 0};
    
    // A_init
    args = B_ainit_chk("$LST1", NULL, 0);
    S4ASSERT(S4ASSERT_cmp_tbl(args, "A;B"), "B_ainit_chk(\"$LST1\")");
    SCR_free_tbl(args);
    //args = B_ainit_chk("A*", NULL, 0);
    
    // Test parameters in a file. test.args must exist in the current dir and contain the line
    // A1 A2
    args = B_ainit_chk("@test.args", NULL, 0);
    S4ASSERT(S4ASSERT_cmp_tbl(args, "A1;A2"), "B_ainit_chk(\"@test.args\")");
    SCR_free_tbl(args);
}

Tests_ERRMSGS() 
{
    B_seterrn(86, "bla bla");
    kerror(0, "Coucou de kerror %s", "Hello");
    kmsg("Coucou de kmsg %s -- %g", "Hello", 3.2);
}
    
    
/* ========================================================*/
int main(int argc, char **argv)
{
    // Initialise 7 KDB
    KDB*        kdbv;
    CLEC*       clec;
    SAMPLE      *smpl;
    IODE_REAL   A[64], B[64];
    int         i, nb;

    IODE_assign_super_API();
    strcpy(SCR_NAME_ERR, "iode.msg");

    for(i = 1 ; i < argc; i++) {
        if(strcmp(argv[i], "-v-") == 0) S4ASSERT_VERBOSE = 0;
        if(strcmp(argv[i], "-v")  == 0) S4ASSERT_VERBOSE = 1;
        if(strcmp(argv[i], "-x-") == 0) S4ASSERT_EXIT_ON_ERROR = 0;
        if(strcmp(argv[i], "-x") == 0)  S4ASSERT_EXIT_ON_ERROR = 1;
    }
    
    // Initialises 7 empty WS
    K_init_ws(0);

    // test B_seterrn()
    Tests_ERRMSGS();
    Tests_BUF();
    Tests_Objects();
    Tests_LEC();
    Tests_EQS();
    Tests_ARGS();

    
//    B_ReportLine("$show coucou");
}


