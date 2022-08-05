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
    char    *IODE_DATA_DIR   = "data";
    char    *IODE_OUTPUT_DIR = ".";
#endif


// Fonctions annulées/remplacées temporairement pour passer le link

// Pour tester l'estimation
#ifdef __cplusplus
extern "C" 
{
#endif

    extern char         SCR_NAME_ERR[255 + 1];

    //int o_estgr(char** titles, SAMPLE *smpl, MAT* mlhs, MAT* mrhs, int view, int res) {return(0);}
    //int B_A2mSetRtfTitle(U_ch* title) {return(0);}
    //int B_A2mSetRtfCopy(U_ch* copyr) {return(0);}
    //int B_PrintRtfTopic(char* x) { return(0); }
    int A2mGIF_HTML() {return(0);}
    //int W_printf(char*fmt, ...) {return(0);}
    //void K_load_iode_ini() {}
    //void K_save_iode_ini() {}

#ifdef __cplusplus
}
#endif

 
/**
 *  Fonctions et vars de la lib S4ASSERT 
 *  ------------------------------------ 
 *      int U_cmp_strs(char* str1, char* str2)       : compares 2 strings and return(1) if equal
 *      int U_cmp_tbls(char** tbl1, char* vec)       : compares a table of strings to a list of strings in a semi-colon separated vector.
 *      int U_diff_files(char* file1, char* file2)   : compares the content of 2 files and prints differences
 *  
 *      void S4ASSERT(int success, char* fmt, ...)  : Verifies an assertion, optionally displays a message and opt. exits on error.
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
 *      U_cmp_strs("A B C", "A;B;C") => returns 0 => NOK
 *      U_cmp_strs("A B",   "A B")   => returns 1 => OK
 *  
 *  @param [in] str1    char*   string  
 *  @param [in] str2    char*   string 
 *  @return             int     1 if the content of str1 == that of str2, 0 if not
 *  
 */
int U_cmp_strs(char* str1, char* str2)
{
    if(str1 == NULL && str2 == NULL) return(1);
    if(str1 == NULL || str2 == NULL) return(0);
    return(!strcmp(str1, str2));
}


/**
 *  Compares a table of strings to a list of strings in a semi-colon separated vector.
 *  
 *  Ex. 
 *      U_cmp_tbls((char**)SCR_vtom("A B C", ' '), "A;B;C") => return(1) => OK
 *      U_cmp_tbls((char**)SCR_vtom("A B", ' '),   "A;B;C") => return(0) => NOK
 *  
 *  @param [in] tbl1    char**  table of strings    
 *  @param [in] vec     char*   semi-colon sep vector
 *  @return             int     0 if tbl1 and vec are equivalent, -1 else
 *  
 */
int U_cmp_tbls(char** tbl1, char* vec)
{
    int     i, rc = 0;
    char**  tbl2;

    tbl2 = (char**)SCR_vtoms((unsigned char*) vec, (unsigned char*) " ;,");
    if(tbl1 == NULL) {
        if(tbl2 == NULL) return(-1);
        goto fin;
    }    
    if(tbl2 == NULL) return(0); 
    if(SCR_tbl_size((unsigned char**) tbl1) != SCR_tbl_size((unsigned char**) tbl2)) goto fin;
    for(i = 0 ;  tbl1[i] ; i++)
        if(strcmp(tbl1[i], tbl2[i])) goto fin;
    rc = -1;
    
fin:
    SCR_free_tbl((unsigned char**) tbl2);
    return(rc);

}

char* U_test_read_file(char*filename, long *size)
{
    return((char*)SCR_LoadRawFile(filename, size));
    
//    FILE    *fd;
//    char    *content = 0;
//    
//    *size = 0;
//    fd = fopen(filename, "rb");
//    if(fd == 0) return(NULL);
//    
//    while(!feof(fd)) {
//        if(*size % 1024 == 0) 
//            content = SCR_realloc(content, 1, *size, 1 + 1024 + *size);
//        content[*size] = fgetc(fd);
//        (*size)++;
//    }
//    content[*size] = 0; // Juste pour dire
//    fclose(fd);
//    return(content);
}

/**
 *  Compares 2 files. Returns 0 if the 2 files are ==, -1 if not.
 *  
 *  @return int     -1 if the 2 files are ==, 
 *                  0 if not.
 */
int U_cmp_files(char*file1, char*file2)
{
    int     rc = -1;        // ==
    long    size1, size2;
    char    *content1, *content2;
    
    //printf("Comparing '%s' and '%s'\n", file1, file2);
    content1 = U_test_read_file(file1, &size1);
    //printf("   '%s': size=%ld\n", file1, size1);
    content2 = U_test_read_file(file2, &size2);
    //printf("   '%s': size=%ld\n", file2, size2);
    
    if(size1 != size2) {
        rc = 0;              // !=
        goto fin;                           
    }
    if(content1 == NULL && content2 == NULL) {              // ==
        goto fin;      
    }
    
    if(content1 == NULL || content2 == NULL) {
        rc = 0;
        goto fin;      // !=
    }    
    
    rc = !memcmp(content1, content2, size1);                 
fin:    
    SCR_free(content1);
    SCR_free(content2);
    return(rc);
}


/**
 *  Compares 2 files. Returns 0 if the 2 files are ==, -1 if not.
 *  If files are <>, prints the differences.
 *  
 *  @return int     -1 if the 2 files are ==, 
 *                  0 if not.
 */
int U_diff_files(char*file1, char*file2)
{
    int     rc = -1, i, j, nbdiffs = 0;        // ==
    long    size1, size2;
    char    *content1, *content2;
    char    **tbl1, **tbl2;
    
    //printf("Comparing '%s' and '%s'\n", file1, file2);
    content1 = U_test_read_file(file1, &size1);
    //printf("   '%s': size=%ld\n", file1, size1);
    content2 = U_test_read_file(file2, &size2);
    //printf("   '%s': size=%ld\n", file2, size2);
    
    if(size1 != size2) {
        rc = 0;              // !=
        goto cmp;                           
    }
    if(content1 == NULL && content2 == NULL) {              // ==
        goto fin;      
    }
    
    if(content1 == NULL || content2 == NULL) {
        if(content1 == NULL) printf("File %s not found\n", file1);
        if(content2 == NULL) printf("File %s not found\n", file2);
        rc = 0;
        goto fin;      // !=
    }    
    
    rc = !memcmp(content1, content2, size1);
cmp:    
    if(rc == 0) { // files are different
        tbl1 = (char**) SCR_vtom((U_ch*)content1, '\n');
        tbl2 = (char**) SCR_vtom((U_ch*)content2, '\n');
        for(i = 0; tbl1[i] && tbl2[i]; i++) {
            if(strcmp(tbl1[i], tbl2[i])) {
                nbdiffs++;
                // Print diff bw the 2 lines
                printf("Line %-5d:%s\n           %s\n           ", i + 1, tbl1[i], tbl2[i]);
                for(j = 0; tbl1[i][j] && tbl2[i][j]; j++) {
                    if(tbl1[i][j] == tbl2[i][j]) 
                        printf(" ");
                    else 
                        printf("^");
                }
                printf("\n");
            }
            if(nbdiffs >= 10) break;
        }    
        SCR_free_tbl((U_ch**)tbl1);
        SCR_free_tbl((U_ch**)tbl2);
    }
    
fin:    
    SCR_free(content1);
    SCR_free(content2);
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


//=========================================================================
// U_test_*() => UTILITIES FOR Tests_* functions 
//-------------------------------------------------------------------------


// Print titles
void U_test_print_title(char* title)
{
    int i;

    printf("\n\n%s\n", title);
    for (i = 0; title[i]; i++) printf("-");
    printf("\n");
}


// Output messages

// Function which replaces A2mMessage() when no output is  needed
void kmsg_null(char*msg) 
{
}

// Choose to display a2m messages or not.
void U_test_a2m_msgs(int IsOn)
{
    static int  Current_IsOn = 1;
    static void (*A2mMessage_super_ptr)(char*); 
    
    if(IsOn && !Current_IsOn) { 
        A2mMessage_super = A2mMessage_super_ptr; // (Re-)install default function
        Current_IsOn = 1;
        return;
    }
    else if(!IsOn && Current_IsOn) {
        A2mMessage_super_ptr = A2mMessage_super;  // Save default value before replacing it by kmsg_null
        A2mMessage_super = kmsg_null;             // Suppress output messages
        Current_IsOn = 0;
        return;
    }
}
    
void U_test_suppress_a2m_msgs()
{
    U_test_a2m_msgs(0);
}

void U_test_reset_a2m_msgs()
{
    U_test_a2m_msgs(1);
}


/**
 *  Create some objects for Tests_*().
 *      - k_add() : LIST, VAR
 *      - KLPTR()
 *      - KV_sample()
 */
void U_test_CreateObjects()
{
    char*       lst;
    SAMPLE*     smpl;
    IODE_REAL   A[64], B[64];
    int         nb, i, pos;
    static int  done = 0;
    
    //if(done) return;
    //done = 1;
    
    //U_test_print_title("Tests OBJECTS");

    // Create lists
    pos = K_add(KL_WS, "LST1", "A,B");
    //S4ASSERT(pos >= 0,                    "K_add(\"LST1\") = %d", pos);
    K_add(KL_WS, "LST2", "A,B,A");
    lst = KLPTR("LST1");
    //S4ASSERT(strcmp(lst, "A,B") == 0,     "KLPTR(\"LST1\") = \"%s\"", lst);

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
    //S4ASSERT(K_find(KV_WS, "A") >= 0,  "K_add() + K_find()");
    pos = K_add(KV_WS, "B", B, &nb);
    
    // For B_DataPattern()
    pos = K_add(KL_WS, "AB", "A,B");
    pos = K_add(KL_WS, "BC", "B,C");
    
    pos = K_add(KV_WS, "AB", B, &nb);
    pos = K_add(KV_WS, "AC", B, &nb);
    pos = K_add(KV_WS, "BB", B, &nb);
    pos = K_add(KV_WS, "BC", B, &nb);
}    

int U_test_eq(double v1, double v2)
{
    double diff;
    
    if(fabs(v2) > 1e-8)
        diff = fabs((v2 - v1) / v2);
    else 
        diff = fabs(v2 - v1);
    return(diff < 1e-4);
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
    S4ASSERT(U_test_eq(expected_val, calc_val), "Res=%10.3lf - Expected=%10.3lf %s L_exec(%s) in %s", calc_val, expected_val, title, lec, aper);
}

/**
 *  Calc a LEC expression in t
 */
double U_test_calc_lec(char* lec, int t)
{
    CLEC*   clec;

    clec = L_cc(lec);
    if(clec == NULL) return(L_NAN);
    if(L_link(KV_WS, KS_WS, clec)) return(L_NAN);
    return(L_exec(KV_WS, KS_WS, clec, t));
}

KDB* U_test_K_interpret(int type, char* filename)
{
    char    fullfilename[256];
    KDB     *kdb;
    
    sprintf(fullfilename,  "%s\\%s", IODE_DATA_DIR, filename);
    kdb = K_interpret(type, fullfilename);
    //S4ASSERT(kdb != NULL, "K_interpret(%d, \"%s\")", type, fullfilename);
    return(kdb);
}

void U_test_load_fun_esv(char* filename)
{
    // Frees 3 WS
    K_free(KE_WS);
    K_free(KS_WS);
    K_free(KV_WS);
    
    // Loads 3 WS and check ok
    KE_WS = U_test_K_interpret(K_EQS, filename);
    KS_WS = U_test_K_interpret(K_SCL, filename);
    KV_WS = U_test_K_interpret(K_VAR, filename);
}


//======================================================================
// Tests_<NAME> => unit tests
// ---------------------------------------------------------------------

void Tests_IODEMSG()
{
    char    *msg;
    
    U_test_print_title("Tests IODEMSG");
    msg = B_msg(16); // Sample modified
    S4ASSERT(U_cmp_strs(msg, " Sample modified"), "Message 16 = '%s', should be ' Sample modified'", msg);
    
    //B_seterror(char* fmt, ...)     Formats an error message and adds the text of the message to the global table of last errors.
    //B_seterrn(int n, ...)          Formats a message found in iode.msg and adds the result to the list of last errors.
    //B_display_last_error()         Displays the last recorded errors (in B_ERROR_MSG) using kmsgbox().
    //B_print_last_error()           Displays or prints the last recorded errors (in B_ERROR_MSG) using W_printf().
    //B_clear_last_error()           Resets the list of last messages (B_ERROR_MSG and B_ERROR_NB).
}


void Tests_BUF()
{
    U_test_print_title("Tests BUF");
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
void Tests_OBJECTS()
{
    char*       lst;
    int         pos;
    static int  done = 0;

    U_test_print_title("Tests OBJECTS");
    U_test_CreateObjects();

    // Create lists
    pos = K_find(KL_WS, "LST1");
    S4ASSERT(pos >= 0,                    "K_add & K_find(KL_WS, \"LST1\") = %d", pos);
    lst = KLPTR("LST1");
    S4ASSERT(strcmp(lst, "A,B") == 0,     "KLPTR(\"LST1\") = \"%s\"", lst);

    
    pos = K_find(KV_WS, "A");
    S4ASSERT(K_find(KV_WS, "A") >= 0,  "K_add and K_find(KV_WS, \"A\") = %d", pos);
    
    pos = K_ren(KV_WS, "A", "AAA");
    S4ASSERT(K_find(KV_WS, "AAA") >= 0,  "K_ren(KV_WS, \"A\", \"AAA\")) = %d", pos);
   
}    




/**
 *  Tests some LEC calculations.
 */
void Tests_LEC()
{
    IODE_REAL *A, *B;

    U_test_print_title("Tests LEC");

    // Create objects
    U_test_CreateObjects();
    
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
//    K_upd_eqs("A", "ln A := B + t", NULL, 'L', NULL, NULL, NULL, NULL, NULL);
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
    
    U_test_print_title("Tests ARGS");

    // Create objects
    U_test_CreateObjects();
    
    // A_init
    args = B_ainit_chk("$LST1", NULL, 0);
    S4ASSERT(U_cmp_tbls(args, "A;B"), "B_ainit_chk(\"$LST1\")");
    SCR_free_tbl((unsigned char**) args);
    //args = B_ainit_chk("A*", NULL, 0);
    
    // Test parameters in a file. test.args must exist in the current dir and contain the line
    // A1 A2
    sprintf(filename, "@%s\\test.args", IODE_DATA_DIR);
    args = B_ainit_chk(filename, NULL, 0);
    S4ASSERT(U_cmp_tbls(args, "A1;A2"), "B_ainit_chk(\"%s\")", filename);
    SCR_free_tbl((unsigned char**) args);
}

///**
// *  Tests argument expansion ALD
// */
//void Tests_ARGS_ALD()
//{
//    char **args;
//    char *list[] = {"A1", "A2", 0};
//    char filename[256];
//    int  pos;
//    
//    
//    // Create list
//    pos = K_add(KL_WS, "LST1", "A,B");
// 
//    // A_init
//    args = B_ainit_chk("$LST1", NULL, 0);
//    S4ASSERT(U_cmp_tbls(args, "A;B"), "B_ainit_chk(\"$LST1\")");
//    SCR_free_tbl(args);
//}
//

void Tests_ERRMSGS() 
{
    U_test_print_title("Tests Err Msgs");

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

    U_test_print_title("Tests K_OBJFILE");

    sprintf(in_filename,  "%s\\fun.var", IODE_DATA_DIR);
    sprintf(out_filename, "%s\\fun_copy.var", IODE_OUTPUT_DIR);
    
    kdb_var = K_interpret(K_VAR, in_filename);
    S4ASSERT(kdb_var != NULL, "K_interpret(K_VAR, \"%s\")", in_filename);
    if(kdb_var) {
        S4ASSERT(KNB(kdb_var) == 394, "KNB(\"%s\") == 394", in_filename);
        rc = K_save(kdb_var, out_filename);
        S4ASSERT(rc == 0, "K_save(kdb_var, \"%s\") == 0", out_filename);
    }
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
    
    int     pos;
    TBL*    c_table;
    TCELL*  cells;
   
    U_test_print_title("Tests conversion 32 to 64 bits");

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
    
    // Next lines temporarily deleted because the do not work in VS 64
    //for(col = 0; col < c_table->t_nc; col++) {
    //    //cell_content = T_cell_cont(&cells[col], 1);
    //    cell_content = T_div_cont_tbl(c_table, col, 1);
    //    printf("Cell %d:%s\n",col, cell_content);
    //}
}


void Tests_Simulation()
{
    KDB         *kdbv, 
                *kdbe, 
                *kdbs;
    SAMPLE      *smpl;
    char        *filename = "fun";
    U_ch**      endo_exo;
    int         rc;
    LIS         lst, expected_lst;
    void        (*kmsg_super_ptr)(char*);
    
    U_test_print_title("Tests Simulation");

    // Loads 3 WS and check ok
    U_test_load_fun_esv(filename);

    // Check
    kdbv = KV_WS;
    S4ASSERT(kdbv != NULL, "K_interpret(K_VAR, \"%s\")", filename);
    kdbs = KS_WS;
    S4ASSERT(kdbs != NULL, "K_interpret(K_SCL, \"%s\")", filename);
    kdbe = KE_WS;
    S4ASSERT(kdbe != NULL, "K_interpret(K_EQS, \"%s\")", filename);
    
    // Check list is empty
    lst = KLPTR("_DIVER");
    S4ASSERT(U_cmp_strs(lst, NULL), "_DIVER=NULL", lst);
    
    // Simulation std parameters
    smpl = PER_atosmpl("2000Y1", "2002Y1");
    KSIM_START = KV_INIT_TM1;
    KSIM_EPS = 0.0001;
    KSIM_MAXIT = 100;
    KSIM_RELAX = 0.7;
    KSIM_SORT = SORT_BOTH;
    KSIM_PASSES = 5; 
    KSIM_DEBUG = 1;

    kmsg_super_ptr = kmsg_super;
    kmsg_super = kmsg_null; // Suppress messages at each iteration during simulation
    
    // Test simulation : divergence
    KSIM_MAXIT = 2;
    rc = K_simul(kdbe, kdbv, kdbs, smpl, NULL, NULL);
    S4ASSERT(rc != 0, "K_simul() with only maxit=%d does not converge", KSIM_MAXIT);
    
    // Check _PRE list after simulation (prolog)
    lst = KLPTR("_PRE");
    expected_lst = "BRUGP;DTH1C;EX;ITCEE;ITCR;ITGR;ITI5R;ITIFR;ITIGR;ITMQR;NATY;POIL;PW3;PWMAB;PWMS;PWXAB;PWXS;PXE;QAH;QWXAB;QWXS;QWXSS;SBGX;TFPFHP_;TWG;TWGP;ZZF_;DTH1;PME;PMS;PMT";
    //printf("     '%s'(%d)\n", expected_lst, strlen(expected_lst));
    S4ASSERT(U_cmp_strs(lst, expected_lst), "_PRE=\"%s\"", lst);
    
    // Check _DIVER list
    lst = KLPTR("_DIVER");
    //printf("'%s'\n", lst);
    expected_lst = "SSH3O,WBG,SSF3,YDH,DTH,YDTG,YSFIC,WMIN,WLCP,WBGP,YSEFT2,YSEFT1,YSEFP,SBG,PWBG,W,ZJ,QMT,QI5,QC_,SSFG,YDH_,SG,ACAG,FLG";
    S4ASSERT(U_cmp_strs(lst, expected_lst), "_DIVER=\"%s\"", lst);
    
    // Test with with convergence (increase MAXIT)
    KSIM_MAXIT = 100;
    rc = K_simul(kdbe, kdbv, kdbs, smpl, NULL, NULL);
    S4ASSERT(rc == 0, "K_simul() with maxit=%d does converge",KSIM_MAXIT);

    // Test Endo-exo
    
    // Version avec échange dans une seule équation
    // endo_exo = SCR_vtoms("UY-NIY", ",; ");
    // rc = K_simul(kdbe, kdbv, kdbs, smpl, endo_exo, NULL);
    // S4ASSERT(rc == 0, "Exchange UY-NIY converges on 2000Y1-2002Y1");
    // S4ASSERT(UY[pos2000] == 650.0, "Exchange UY-NIY: UY[2000Y1] == 650.0");
    // S4ASSERT(fabs(NIY[pos2000] - 658.423) < 0.01, "Exchange UY-NIY: NIY[2000Y1] == 658.423");

    // Version avec échange dans min 2 equations
    // Set values of endo UY
    KV_set_at_aper("UY", "2000Y1", 650.0);
    KV_set_at_aper("UY", "2001Y1", 670.0);
    KV_set_at_aper("UY", "2002Y1", 680.0);

    // Simulate with exchange UY - XNATY
    endo_exo = SCR_vtoms((unsigned char*)"UY-XNATY", (unsigned char*)",; ");
    rc = K_simul(kdbe, kdbv, kdbs, smpl, (char**)endo_exo, NULL);
    
    // Check result
    S4ASSERT(rc == 0, "Exchange UY-XNATY converges on 2000Y1-2002Y1");
    S4ASSERT(KV_get_at_aper("UY", "2000Y1") == 650.0, "Exchange UY-XNATY: UY[2000Y1] == 650.0 unchanged");
    S4ASSERT(U_test_eq(KV_get_at_aper("XNATY", "2000Y1"), 0.80071), "Exchange UY-XNATY: XNATY[2000Y1] == 0.80071");

    // Cleanup
    SCR_free_tbl(endo_exo);
    SCR_free(smpl);
    kmsg_super = kmsg_super_ptr; // reset default value
}


void Tests_PrintTablesAndVars()
{
    char    fullfilename[256];
    char    **varlist;
    SAMPLE  *smpl;
    KDB     *kdbv, *kdbt;
    TBL     *tbl;
    int     rc;

    U_test_suppress_a2m_msgs();
    
    U_test_print_title("Tests Print TBL as Tables and Graphs");

    // Load the VAR workspace
    K_RWS[K_VAR][0] = K_WS[K_VAR] = kdbv  = U_test_K_interpret(K_VAR, "fun.var");
    S4ASSERT(kdbv != NULL, "K_interpret(K_VAR, \"%s\")", "fun.var");

    // Load the TBL workspace
    K_WS[K_TBL] = kdbt  = U_test_K_interpret(K_TBL, "fun.tbl");
    S4ASSERT(kdbt != NULL, "K_interpret(K_TBL, \"%s\")", "fun.tbl");

    // Load a second VAR workspace in K_RWS[K_VAR][2]
    sprintf(fullfilename,  "%s\\%s", IODE_DATA_DIR, "fun.var");
    rc = K_load_RWS(2, fullfilename);
    S4ASSERT(rc == 0, "K_load_RWS(2, \"%s\")", fullfilename);
    
    
    // Select a table
    tbl = KTPTR("C8_1");
    S4ASSERT(tbl != NULL, "KTPTR(\"C8_1\") not null.");
    
    // Select Print destination
    //W_dest("test1_tbl.htm", W_HTML);
    W_dest("test1_tbl.a2m", W_A2M);
    //W_dest("", W_GDI);
   
    // Print tbl as table
    rc = T_print_tbl(tbl, "2000:5[1;2]");
    S4ASSERT(rc == 0, "T_print_tbl(tbl, \"2000:5[2]\")");
    
    // Print tbl as a graph
    rc = T_graph_tbl_1(tbl, "2000/1999:15[1;2]", 1);
    S4ASSERT(rc == 0, "T_graph_tbl_1(tbl, \"2000/1999:15[1;2]\", 1)");
    
    // Frees tbl
    T_free(tbl);   

    // Print vars as graphs
    varlist = (char**) SCR_vtoms((U_ch*)"ACAF,ACAG,ACAF+ACAG", (U_ch*)",;");
    smpl = PER_atosmpl("1990Y1", "2010Y1");
    rc = V_graph(0, 0, 0, 1, 1, 0, L_NAN, L_NAN, smpl, varlist);
    S4ASSERT(rc == 0, "V_graph(0, 0, 0, 1, 1, 0, L_NAN, L_NAN, 2000Y1:2020Y1, ACAF;ACAG)");
    SCR_free_tbl((U_ch**)varlist);
    SCR_free(smpl);
    
    // Close the output file
    W_close();
    
    // Cleanup the 2d VAR ws
    K_load_RWS(2, NULL);
    
    // Reset A2M messages
    U_test_reset_a2m_msgs();
}


void Tests_Estimation()
{
    int         rc;
    void        (*kmsg_super_ptr)(char*);
    SAMPLE      *smpl;
    IODE_REAL   r2;

    U_test_suppress_a2m_msgs();
    U_test_print_title("Tests Estimation");

    kmsg_super_ptr = kmsg_super;
    kmsg_super = kmsg_null; // Suppress messages at each iteration during simulation

    // Select output destination
    W_dest("test1_estim.a2m", W_A2M);
    //W_dest("test1_estim.rtf", W_RTF);
    //W_dest("test1_estim.htm", W_HTML);
    //W_dest("test.gdi",        W_GDI); 
        
    U_test_load_fun_esv("fun");
    rc = KE_estim("ACAF", "1980Y1", "1996Y1");
    S4ASSERT(rc == 0, "Estimation : KE_estim(\"ACAF\", \"1980Y1\", \"1996Y1\")");

    //x = U_test_calc_lec("_YRES[1980Y1]", 0);
    //printf("x = %lf\n", x);
    //x = fabs(x + 0.001150);
    S4ASSERT(U_test_eq(U_test_calc_lec("_YRES[1980Y1]", 0), -0.00115), "Estimation : _YRES[1980Y1] == -0.001150");
    
    //x = fabs(K_e_r2(KE_WS, "ACAF") - 0.821815);
    S4ASSERT(U_test_eq(K_e_r2(KE_WS, "ACAF"), 0.821815), "Estimation : R2 == 0.821815");
    
    //TODO:add some tests with other estimation methods / on blocks / with instruments
    
    //W_flush();
    W_close();
 
    
    // E_StepWise
    smpl = PER_atosmpl("1980Y1", "1995Y1");
    r2 = E_StepWise(smpl, "ACAF", "", "r2");
    S4ASSERT(U_test_eq(r2, 0.848519), "E_StepWize(\"1980Y1:1995Y1\", \"ACAF\", \"\", \"r2\") == 0.848519");
    SCR_free(smpl);
    
    // Dickey-Fuller test (E_UnitRoot)
    // TODO: implement a test
    
    // Reset initial kmsg fn
    kmsg_super = kmsg_super_ptr; // Reset initial output to 
    U_test_reset_a2m_msgs();

}

void Tests_ALIGN()
{
    TBL     tbl, *p_tbl = &tbl;
    int     offset;
    
    U_test_print_title("Tests ALIGN");

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



// W_WRT TESTS
// -----------

/**
 *  A2M code:
 *      - sep and esc
 *      - page header and footer
 *      - title
 *      - table
 *      - graph
 */
void U_test_W_printf_cmds() 
{
    W_printf(".sep |\n");
    W_printf(".esc ~\n");
    
    W_print_pg_header("IODE");
    //W_printf(".pghead IODE\n");
    W_print_pg_footer("- page %d - ");
    //W_printf(".pgfoot - page %%d - \n");
    
    W_print_tit(1);
    W_printf("%s\n", "Mon titre");
    
    W_print_enum(1);
    W_printf("This is an enum paragraph\n\n");
    W_printf("This is a normal paragraph\n\n");
    
    W_printf(".tb\n");
    W_printf(".tborder 1\n");
    W_printf(".theader\n");
    W_printf("| Syntax | Description\n");
    W_printf(".tbody\n");
    W_printf("| ~cint L_errno       | Last error number during LEC compilation\n");
    W_printf("| ~cchar* L_error()   | Returns a static buffer containing the last LEC compilation error message.\n");
    W_printf(".te\n");
     
    W_printf(".gb 16.00 10.00\n");
    W_printf(".ggrid TMM\n");
    W_printf(".galign L\n");
    W_printf(".gbox 0 b 0\n");
    W_printf(".gtitle Equation ACAF : observed and fitted values\n");
    W_printf(".gty L \"(ACAF/VAF[-1])  : observed\" 1980Y1  0.011412042  0.016028202  0.011903394  0.012051363  0.010215556  0.010582964  0.0090882893  0.009277778  0.0082268494  0.0051589358  0.0066405193  0.0068489061  0.0075258742  0.0082727193  0.0019340143  -0.0029850522  0.0069569681 \n");
    W_printf(".gty L \"(ACAF/VAF[-1])  : fitted\" 1980Y1  0.012562124  0.012491075  0.012526504  0.011687035  0.011060337  0.010403968  0.0095320575  0.0090522427  0.008714914  0.0076539375  0.0065561227  0.006355248  0.0064942167  0.0062763884  0.0062678674  -0.0029850522  0.0044903364 \n");
    W_printf(".ge\n");
    
}


void U_test_W_printf_1dest(int typeint, char *typeext)
{
    char reffilename[512];
    char filename[512];

    sprintf(filename, "%s\\test1.%s", IODE_OUTPUT_DIR, typeext);
    sprintf(reffilename, "%s\\test1.ref.%s", IODE_DATA_DIR, typeext);
    W_dest(filename, typeint); 
    U_test_W_printf_cmds();
    W_close();
    printf("Comparing ref '%s' and '%s'\n", reffilename, filename);
    //S4ASSERT(U_cmp_files(reffilename, filename), "W_printf -> %s", typeext);
    S4ASSERT(U_diff_files(reffilename, filename), "W_printf -> %s", typeext);
}

void Tests_W_printf()
{
    U_test_print_title("Tests W_printf");

    U_test_suppress_a2m_msgs();

    //W_gdiask = 0;  // 1 means that a popup window will be opened to select the printer => not practical for the automated tests!
    //W_dest("test1", W_GDI); 
    //U_test_W_printf();

    U_test_W_printf_1dest(W_A2M, "a2m");
    U_test_W_printf_1dest(W_RTF, "rtf");
    U_test_W_printf_1dest(W_HTML, "htm");
    U_test_W_printf_1dest(W_CSV, "csv");
    U_test_W_printf_1dest(W_MIF, "mif");
    
    
    W_dest("", W_DUMMY); 
    U_test_W_printf_cmds();
    
    //W_dest("test1", W_DISP); 
    //U_test_W_printf();
         
    W_close(); // Closes the last "print" if any
    
    // Reset initial kmsg fn
    U_test_reset_a2m_msgs();
}

// Test SWAP 
// ---------

// Tests that a second call to SW_free() does not crash (temp)
void Tests_SWAP()
{
    SWHDL   item, item2;
    
    U_test_print_title("Tests SWAP");

    // test 1 : deux frees successifs
    item = SW_alloc(20);
    SW_free(item);
    SW_free(item);
    
    // test 2 :  on réutilise l'espace freeé
    item = SW_alloc(15);
    SW_free(item);
    item2 = SW_alloc(10);
    SW_free(item);
    SW_free(item2);
}



void Tests_B_DATA()
{
    char        *lst, *ptr, buf[512];
    int         rc, i, cond;
    IODE_REAL   *A1, val;
    SAMPLE      *smpl;
    char        *filename = "fun";
    
    U_test_print_title("Tests B_DATA");

    // Clear WS, then loads 3 WS and check ok
    //K_end_ws(0);
    U_test_load_fun_esv(filename);
    
    // (re-)creates vars AA...
    K_clear(KC_WS);
    K_clear(KI_WS);
    K_clear(KL_WS);
    K_clear(KT_WS);
    U_test_CreateObjects();
    
    // B_DataPattern()
    // Foireux. Faut utiliser des listes (avec A;B au lieu de $AB ça marche pas...) => A changer ? Voir B_DataListSort() 
    B_DataPattern("RC xy $AB $BC", K_VAR); 
    lst = KLPTR("RC");
    S4ASSERT(U_cmp_strs(lst, "AB,AC,BB,BC"), "B_DataPattern(\"RC xy $AB $BC\", K_VAR) = \"AB,AC,BB,BC\"");

    // B_DataCalcVar()
    rc = B_DataCalcVar("A1 2 * B");
    A1 = KVPTR("A1");
    cond = (rc == 0) && (K_find(KV_WS, "A1") >= 0) && (A1[1] == 4);
    S4ASSERT(cond == 1, "B_DataCalcVar(\"A1 2 * B\")");
    
    // B_DataCreate(char* arg, int type)
    // B_DataDuplicate(char* arg, int type)
    // B_DataRename(char* arg, int type)
    // B_DataDelete(char* arg, int type)
    for(i = 0; i < 7 ; i++) {
        rc = B_DataCreate("XXX", i);
        cond = (rc == 0) && (K_find(K_WS[i], "XXX") >= 0);
        S4ASSERT(cond == 1,  "B_DataCreate(\"XXX\", %d)", i);
        
        if(i != K_EQS) { // Equations cannot be renamed or duplicated
            rc = B_DataDuplicate("XXX YYY", i);
            cond = (rc == 0) && (K_find(K_WS[i], "YYY") >= 0);
            S4ASSERT(cond == 1,  "B_DataDuplicate(\"XXX YYY\", %d)", i);

            rc = B_DataRename("YYY ZZZ", i);
            cond = (rc == 0) && (K_find(K_WS[i], "ZZZ") >= 0);
            S4ASSERT(cond == 1,  "B_DataRename(\"YYY ZZZ\", %d)", i);
        }
        
        rc = B_DataDelete("XXX", i);
        cond = (rc == 0) && (K_find(K_WS[i], "XXX") < 0);
        S4ASSERT(cond == 1,  "B_DataDelete(\"XXX\", %d)", i);
    }                 

    // B_DataListSort()
    rc = K_add(KL_WS, "LIST1", "A;C;B");
    S4ASSERT(K_find(KL_WS, "LIST1") >= 0,  "K_add(KL_WS, \"LIST1\", \"A;C;B\")");
    rc = B_DataListSort("LIST1 LIST2");
    lst = KLPTR("LIST2");
    S4ASSERT(U_cmp_strs(lst, "A;B;C"), "B_DataListSort(\"LIST1 LIST2\") = \"%s\"", lst);

    // B_DataUpdate()
    rc = B_DataUpdate("U Comment of U"       , K_CMT);
    cond = (rc == 0) && U_cmp_strs(KCPTR("U"), "Comment of U");
    S4ASSERT(cond == 1, "B_DataUpdate(\"U Comment of U\", K_CMT) = \"%s\"", KCPTR("U"));
    
    rc = B_DataUpdate("U U := c1 + c2*Z"     , K_EQS);
    
    rc = B_DataUpdate("U 2 * A"              , K_IDT);
    cond = (rc == 0) && U_cmp_strs(KIPTR("U"), "2 * A");
    S4ASSERT(cond == 1, "B_DataUpdate(\"U 2 * A\", K_IDT) = \"%s\"", KIPTR("U"));
    
    rc = B_DataUpdate("U A,B,C"             , K_LST);
    cond = (rc == 0) && U_cmp_strs(KLPTR("U"), "A,B,C");
    S4ASSERT(cond == 1, "B_DataUpdate(\"U A,B,C\", K_LST) = \"%s\"", KLPTR("U"));
    
    rc = B_DataUpdate("U  1.2 1"             , K_SCL);  
    val = K_s_get_value (KS_WS, "U");
    cond = (rc == 0) && U_test_eq(1.2, val);
    S4ASSERT(cond == 1, "B_DataUpdate(\"U 1.2 1\", K_SCL) = %g", val);
    
    rc = B_DataUpdate("U  Title of U;U;2*U"  , K_TBL);
    smpl = KSMPL(KV_WS);
    rc = B_DataUpdate("U L 2000Y1 2 3.1 4e2" , K_VAR);
    S4ASSERT(rc == 0, "B_DataUpdate(\"U L 2000Y1 2 3.1 4e2\" , K_VAR)");
    
    // B_DataSearch(char* arg, int type)
    rc = B_DataSearch("of 0 0 1 0 1 NEWLIST", K_CMT);
    cond = (rc == 0) && U_cmp_strs(KLPTR("NEWLIST"), "U");
    S4ASSERT(cond == 1, "B_DataSearch(\"of 0 0 1 0 1 NEWLIST\", K_CMT) = \"%s\"", KLPTR("NEWLIST"));
    
    // B_DataScan(char* arg, int type)
    rc = B_DataScan("U", K_EQS);
    cond = (rc == 0) && U_cmp_strs(KLPTR("_SCAL"), "c1;c2");
    S4ASSERT(cond == 1, "B_DataScan(\"U\", K_EQS) = \"%s\"", KLPTR("_SCAL"));
     
    // B_DataExist(char* arg, int type)
    rc = B_DataExist("_SCAL", K_LST);
    S4ASSERT((rc >= 0), "B_DataExist(\"_SCAL\", K_LST) = %d", rc);

    // B_DataAppend(char* arg, int type)
    rc = B_DataAppend("_SCAL XXX,YYY", K_LST);
    cond = (rc == 0) && U_cmp_strs(KLPTR("_SCAL"), "c1;c2,XXX,YYY");
    S4ASSERT(cond == 1, "B_DataAppend(\"_SCAL XXX,YYY\", K_LST) = %d", rc);

    rc = B_DataAppend("U - More comment on U", K_CMT);
    cond = (rc == 0) && U_cmp_strs(KCPTR("U"), "Comment of U - More comment on U");
    S4ASSERT(cond == 1, "B_DataAppend(\"U - More comment on U\", K_CMT) = %d", rc);

    // B_DataList(char* arg, int type)
    rc = B_DataList("LC ac*", K_SCL);
    cond = (rc == 0) && U_cmp_strs(KLPTR("LC"), "acaf1;acaf2;acaf3;acaf4");
    printf("LC = \"%s\"\n", KLPTR("LC"));
    S4ASSERT(cond == 1, "B_DataList(\"LC ac*\", K_SCL); = '%s'", KLPTR("LC"));

    // B_DataCalcLst(char* arg)
    B_DataUpdate("LST1 A,B,C", K_LST);
    B_DataUpdate("LST2 C,D,E", K_LST);
    
    rc = B_DataCalcLst("_RES LST1 + LST2");
    cond = (rc == 0) && U_cmp_strs(KLPTR("_RES"), "A;B;C;D;E");
    S4ASSERT(cond == 1, "B_DataCalcLst(\"_RES LST1 + LST2\") = '%s'", KLPTR("_RES"));
    
    rc = B_DataCalcLst("_RES LST1 * LST2");
    cond = (rc == 0) && U_cmp_strs(KLPTR("_RES"), "C");
    S4ASSERT(cond == 1, "B_DataCalcLst(\"_RES LST1 * LST2\") = '%s'", KLPTR("_RES"));
    
    rc = B_DataCalcLst("_RES LST1 - LST2");
    cond = (rc == 0) && U_cmp_strs(KLPTR("_RES"), "A;B");
    S4ASSERT(cond == 1, "B_DataCalcLst(\"_RES LST1 - LST2\") = '%s'", KLPTR("_RES"));
    
    rc = B_DataCalcLst("_RES LST1 x LST2");
    cond = (rc == 0) && U_cmp_strs(KLPTR("_RES"), "AC;AD;AE;BC;BD;BE;CC;CD;CE");
    S4ASSERT(cond == 1, "B_DataCalcLst(\"_RES LST1 x LST2\") = '%s'", KLPTR("_RES"));

    // B_DataCompare(char* arg, int type)  
    sprintf(buf,  "%s\\fun.lst WS_ONLY FILE_ONLY BOTH_DIFF BOTH_EQ", IODE_DATA_DIR);
    rc = B_DataCompare(buf, K_LST);
    //printf("    WS_ONLY='%s'\n", KLPTR("WS_ONLY"));
    //printf("    FILE_ONLY='%s'\n", KLPTR("FILE_ONLY"));
    //printf("    BOTH_DIFF='%s'\n", KLPTR("BOTH_DIFF"));
    //printf("    BOTH_EQ='%s'\n", KLPTR("BOTH_EQ"));

    cond = (rc == 0) && U_cmp_strs(KLPTR("WS_ONLY"), "AB;BC;LC;LIST1;LIST2;LST1;LST2;NEWLIST;RC;U;ZZZ;_EXO;_RES");
    S4ASSERT(cond == 1, "B_DataCompare()");

    // B_DataPrintGraph()
    //smpl = KSMPL(KV_WS);
    //printf("%d\n", smpl->s_p1);
    rc = B_DataPrintGraph("Grt Line No No Level -- -- 2000Y1 2020Y1 ACAF ACAG ACAF+ACAG");  
    S4ASSERT(rc == 0, "B_DataPrintGraph(\"Grt Line No No Level -- -- 1990Y1 2010Y1 ACAF ACAG ACAF+ACAG\")");
    
    // B_DataDisplayGraph()
    rc = B_DataDisplayGraph("Grt Line No No Level -- -- 2000Y1 2020Y1 ACAF ACAG ACAF+ACAG");  
}




void Tests_B_FILE()
{
    // B_FileDelete(char* arg, int type)

}



// ================================================================================================


void U_test_init()
{
    static int  done = 0;
    
    if(done) return;
    done = 1;
 
    B_IodeMsgPath();            // Set SCR_NAME_ERR to dir(current file) $curdir/iode.msg
    
    IODE_assign_super_API();    // set *_super fn pointers
    // strcpy(SCR_NAME_ERR, "iode.msg");   // message file => temporarily suppressed for GitHub 
    K_init_ws(0);                       // Initialises 7 empty WS
    K_load_iode_ini();
    
}

int main(int argc, char **argv)
{
    int     i;

    // Suppress output messages
    U_test_suppress_a2m_msgs();
        
    for(i = 1 ; i < argc; i++) {
        if(strcmp(argv[i], "-v-") == 0) S4ASSERT_VERBOSE = 0;
        if(strcmp(argv[i], "-v")  == 0) S4ASSERT_VERBOSE = 1;
        if(strcmp(argv[i], "-x-") == 0) S4ASSERT_EXIT_ON_ERROR = 0;
        if(strcmp(argv[i], "-x") == 0)  S4ASSERT_EXIT_ON_ERROR = 1;
        if(strcmp(argv[i], "-h") == 0)   Syntax();
    }
    
    // Initialises super functions / empty WS / error messages
    U_test_init();
    

//    // tests temporaires
//    Tests_ARGS_ALD();   
//    return(0);
    
    Tests_IODEMSG();
    Tests_SWAP(); 
    Tests_ALIGN();
    Tests_ERRMSGS();
    Tests_BUF();
    Tests_OBJECTS();
    Tests_LEC();
    Tests_EQS();
    Tests_ARGS();
    Tests_K_OBJFILE();
    Tests_TBL32_64();
    Tests_Simulation();
    Tests_PrintTablesAndVars();
    Tests_Estimation();
    Tests_W_printf();
    Tests_B_DATA();
    Tests_B_FILE();

   
    //K_save_iode_ini(); // Suppress that call ? Should only be called on demand, not at the end of each IODE session.
    
    // Reset initial kmsg fn
    U_test_reset_a2m_msgs();
    
    return(0);
//    B_ReportLine("$show coucou");
}


/*
REPORT PRIMITIVES
-----------------
    "label",                    RP_label,               RP_label,           0,
    "goto",                     RP_goto,                RP_goto,            0,
    "ask",                      RP_ask,                 RP_ask,             0,
    "abort",                    RP_abort,               RP_abort,           0,
    "quitode",                  RP_quitode,             RP_quitode,         0,
    "quit",                     RP_quitode,             RP_quitode,         0,     
    "onerror",                  RP_onerror,             RP_onerror,         0,
    "return",                   RP_return,              RP_return,          0,
    "show",                     RP_message,             RP_message,         0,
    "msg",                      RP_warning,             RP_warning,         0,
    "beep",                     RP_beep,                RP_beep,            0,
    "settime",                  RP_settime,             RP_settime,         0,
    "incrtime",                 RP_incrtime,            RP_incrtime,        0,
    "shift",                    RP_shift,               RP_shift,           0,
    "define",                   RP_define,              RP_define,          0,
    "debug",                    RP_setdebug,            RP_setdebug,        0, 
    "indent",                   RP_setindent,           RP_setindent,       0, 
    "shellexec",                B_shellexec,            B_shellexec,        0, 
    "repeat",                   RP_repeat,              RP_repeat,          0, 
    "repeatstring",             RP_repeatstring,        RP_repeatstring,    0, 
    "vseps",                    RP_vseps,               RP_vseps,           0, 
    "foreach",                  RP_foreach,             RP_foreach,         0, 
    "next",                     RP_foreach_next,        RP_foreach_next,    0, 
    "procdef",                  RP_procdef,             RP_procdef,         0, 
    "procexec",                 RP_procexec,            RP_procexec,        0, 

    "system",                   RP_system,              RP_system,          0,
    "chdir",                    RP_chdir,               RP_chdir,           0,
    "mkdir",                    RP_mkdir,               RP_mkdir,           0,
    "rmdir",                    RP_rmdir,               RP_rmdir,           0,
    
    // fonctions utilisateur 
    "fileimportvar",            B_FileImportVar,        SB_XodeRuleImport,  0,
    "fileimportcmt",            B_FileImportCmt,        SB_XodeRuleImport,  0,
    "filedelete",               B_FileDelete,           NULL,               4,
    "filerename",               B_FileRename,           NULL,               4,
    "filecopy",                 B_FileCopy,             NULL,               4,
    
    "wssample",                 B_WsSample,             SB_WsSample,        0,
    "wsload",                   B_WsLoad,               SB_WsLoad,          3,
    "wscopy",                   B_WsCopy,               SB_WsCopy,          3,
    "wssave",                   B_WsSave,               SB_WsSave,          3,
    "wssavecmp",                B_WsSaveCmp,            SB_WsSave,          3,
    "wsimport",                 B_WsImport,             SB_WsLoad,          3,
    "wsexport",                 B_WsExport,             SB_WsSave,          3,
    "wsmerge",                  B_WsMerge,              SB_WsMerge,         3,
    "wsclear",                  B_WsClear,              SB_WsClear,         3,
    "wsclearall",               B_WsClearAll,           NULL,               0,
    "wsdescr",                  B_WsDescr,              SB_WsDescr,         0,
    "wsextrapolate",            B_WsExtrapolate,        SB_WsExtrapolate,   0,
    "wsaggrchar",               B_WsAggrChar,           NULL,               0,
    "wsaggrsum",                B_WsAggrSum,            SB_WsAggregate,     0,
    "wsaggrmean",               B_WsAggrMean,           SB_WsAggregate,     0,
    "wsaggrprod",               B_WsAggrProd,           SB_WsAggregate,     0,
    "wshtollast",               B_WsHtoLLast,           SB_WsHtoL,          0,
    "wshtolsum",                B_WsHtoLSum,            SB_WsHtoL,          0,
    "wshtolmean",               B_WsHtoLMean,           SB_WsHtoL,          0,
    "wsltohflow",               B_WsLtoHFlow,           SB_WsLtoH,          0,
    "wsltohstock",              B_WsLtoHStock,          SB_WsLtoH,          0,
    "wsseasonadj",              B_WsSeasonAdj,          SB_WsSeasonAdj,     0,
    "wstrend",                  B_WsTrend,              SB_WsTrend,         0,
    "wstrendstd",               B_WsTrendStd,           SB_WsTrend,         0,
    
    "wsimporteviews",			B_WsImportEviews,       NULL, 		        0,

-   "dataedit",                 NULL,                   SB_DataEditScroll,  1,
X   "dataupdate",               B_DataUpdate,           NULL,               1,
X   "dataexist",                B_DataExist,            NULL,               1,
X   "dataappend",               B_DataAppend,           NULL,               1,
X   "datacreate",               B_DataCreate,           NULL,               1,
X   "datadelete",               B_DataDelete,           NULL,               1,
X   "datarename",               B_DataRename,           NULL,               1,
X   "datasearch",               B_DataSearch,           SB_DataSearch,      1,
X   "dataduplicate",            B_DataDuplicate,        SB_DataDuplicate,   1,
X   "datalist",                 B_DataList,             SB_DataList,        1,
X   "datacompare",              B_DataCompare,          SB_DataCompare,     1,
X   "datacompareeps",           B_DataCompareEps,       SB_DataCompare,     0,
X   "datalistsort",             B_DataListSort,         SB_DataListSort,    0,
X   "dataeditcnf",              B_DataEditCnf,          NULL/,              0,
X   "datacalcvar",              B_DataCalcVar,          NULL,               0,
X   "datacalclst",              B_DataCalcLst,          SB_DataCalcLst,     0,
X   "datarasvar",               B_DataRasVar,           NULL,               0,
X   "datascan",                 B_DataScan,             SB_DataScan,        1,
X   "datapattern",              B_DataPattern,          NULL,               1,
X   "datadisplaygraph",         B_DataDisplayGraph,     SB_DataEditGraph,   0,
X   "dataprintgraph",           B_DataPrintGraph,       SB_DataEditGraph,   0,
    
    "eqsestimate",              B_EqsEstimate,          SB_EqsEstimate,     0,
    "eqsstepwise",              B_EqsStepWise,          NULL,               0,
    "eqssetmethod",             B_EqsSetMethod,         NULL,               0,
    "eqssetbloc",               B_EqsSetBloc,           NULL,               0,
    "eqssetblock",              B_EqsSetBloc,           NULL,               0,
    "eqssetsample",             B_EqsSetSample,         NULL,               0,
    "eqssetinstrs",             B_EqsSetInstrs,         NULL,               0,
    "eqssetcmt",                B_EqsSetCmt,            NULL,               0,
    
    
    "excelget",                 B_ExcelGet,             NULL,               1,
    "excellang",                B_ExcelLang,            NULL,               0,
    "exceldecimal",             B_ExcelDecimal,         NULL,               0,
    "excelthousand",            B_ExcelThousand,        NULL,               0,
    "excelcurrency",            B_ExcelCurrency,        NULL,               0,
    "excelset",                 B_ExcelSet,             NULL,               1,
    "excelexecute",             B_ExcelExecute,         NULL,               0,
    "excelopen",                B_ExcelOpen,            NULL,               0,
    "excelnew",                 B_ExcelNew,             NULL,               0,
    "excelclose",               B_ExcelClose,           NULL,               0,
    "excelprint",               B_ExcelPrint,           NULL,               0,
    "excelsave",                B_ExcelSave,            NULL,               0,
    "excelsaveas",              B_ExcelSaveAs,          NULL,               0,
    "excelwrite",               B_ExcelWrite,           NULL,               0,
        
    "dsimportdb",               B_DSImportDb,           NULL,               0,
        
    "statunitroot",             B_StatUnitRoot,         SB_StatUnitRoot,    0,
        
    "datawidthvar",             B_ScrollVarW,           NULL,               0,
    "datandecvar",              B_ScrollVarN,           NULL,               0,
    "datamodevar",              B_ScrollVarM,           NULL,               0,
    "datastartvar",             B_ScrollVarS,           NULL,               0,
    "datawidthtbl",             B_ScrollTblW,           NULL,               0,
    "datawidthscl",             B_ScrollSclW,           NULL,               0,
    "datandecscl",              B_ScrollSclN,           NULL,               0,
    "viewwidth",                B_ScrollVTW,            NULL,               0,
    "viewwidth0",               B_ScrollVTW0,           NULL,               0,
    "viewndec",                 B_ScrollVTN,            NULL,               0,
    
    "printobjtitle",            B_PrintObjTblTitle,     NULL,               0,
    "printobjlec",              B_PrintObjLec,          NULL,               0,
    "printobjinfos",            B_PrintObjEqsInfos,     NULL,               0,
        
    "printobjdef",              B_PrintObjDef,          SB_PrintObjDef,     1,
    "printdest",                B_PrintDest,            NULL,               0,
    "printdestnew",             B_PrintDestNew,         NULL,               0,
    "printmulti",               B_PrintMulti,           NULL,               0,
    "printnbdec",               B_PrintNbDec,           NULL,               0,
    "printlang",                B_PrintLang,            NULL,               0,
    "printtblfile",             B_ViewTblFile,          NULL,               0,
    "printtbl",                 B_PrintTbl,             SB_ViewPrintTbl,    0,
    "printvar",                 B_PrintVar,             NULL,               0,
            
    "viewtblfile",              B_ViewTblFile,          NULL,               0,
    "viewtbl",                  B_ViewTbl,              SB_ViewPrintTbl,    0,
    "viewvar",                  B_ViewVar,              NULL,               0,
    "viewbytbl",                B_ViewByTbl,            SB_ViewByTbl,       0,
            
    "viewgr",                   B_ViewGr,               SB_ViewPrintGr,     0,

    "graphdefault",             B_GraphDefault,         NULL,               0,
    
    "printgrall",               B_PrintGr,              SB_ViewPrintGr,     0,
    "printgr",                  B_PrintGr,              SB_ViewPrintGr,     0,
    
    "modelsimulate",            B_ModelSimulate,        SB_ModelSimulate,   0,
    "modelsimulateparms",       B_ModelSimulateParms,   NULL,               0,
    "modelexchange",            B_ModelExchange,        NULL,               0,
    "modelcompile",             B_ModelCompile, 	    SB_ModelCompile,    0,
    
    "modelcalcscc",             B_ModelCalcSCC,         NULL,               0,
    "modelsimulatescc",         B_ModelSimulateSCC,     NULL,               0,

    "modelsimulatesaveniters",  B_ModelSimulateSaveNIters,  NULL,           0,
    "modelsimulatesavenorms",   B_ModelSimulateSaveNorms,   NULL,           0,


    "idtexecute",               B_IdtExecute,           SB_IdtExecute,      0,
    "idtexecutevarfiles",       B_IdtExecuteVarFiles,   NULL,               0,
    "idtexecutesclfiles",       B_IdtExecuteSclFiles,   NULL,               0,
    "idtexecutetrace",          B_IdtExecuteTrace,      NULL,               0,
    
    
    "reportexec",               B_ReportExec,           SB_ReportExec,      0,
    "reportedit",               NULL,                   SB_ReportEdit,      0,
    "prompt",                   NULL,                   SB_ReportPrompt,    0,
    "minimize",                 B_WindowMinimize,       NULL,               0,
    "maximize",                 B_WindowMaximize,       NULL,               0,
    "sleep",                    B_Sleep,                NULL,               0,
        
    "printa2mappend",           B_PrintA2mAppend,       NULL,               0,
    "printfont",                B_PrintFont,            NULL,               0,
    "printtablefont",           B_PrintTFont,           NULL,               0,
    "printtablebox",            B_PrintTBox,            NULL,               0,
    "printtablecolor",          B_PrintTColor,          NULL,               0,
    "printtablewidth",          B_PrintTWidth,          NULL,               0,
    "printtablebreak",          B_PrintTBreak,          NULL,               0,
    "printtablepage",           B_PrintTPage,           NULL,               0,

    "printhtmltableclass",      B_PrintHtmlTableClass,  NULL,               0,
    "printhtmltrclass",         B_PrintHtmlTRClass,     NULL,               0,
    "printhtmlthclass",         B_PrintHtmlTHClass,     NULL,               0,
    "printhtmltdclass",         B_PrintHtmlTDClass,     NULL,               0,

    "printbackground",          B_PrintGColor,          NULL,               0,
    "printgraphbox",            B_PrintGBox,            NULL,               0,
    "printgraphbrush",          B_PrintGBrush,          NULL,               0,
    "printgraphsize",           B_PrintGSize,           NULL,               0,
    "printgraphpage",           B_PrintGPage,           NULL,               0,
    "printgraphtheme",          B_PrintGTheme,          NULL,               0,
    "printgraphband",           B_PrintGBand,           NULL,               0,
            
    "printrtfhelp",             B_PrintRtfHelp,         NULL,               0,
    "printhtmlhelp",            B_PrintHtmlHelp,        NULL,               0,
    "printrtftopic",            B_PrintRtfTopic,        NULL,               0,
    "printrtflevel",            B_PrintRtfLevel,        NULL,               0,
    "printrtftitle",            B_PrintRtfTitle,        NULL,               0,
    "printrtfcopyright",        B_PrintRtfCopy,         NULL,               0,
    "printparanum",             B_PrintParaNum,         NULL,               0,
    "printpageheader",          B_PrintPageHeader,      NULL,               0,
    "printpagefooter",          B_PrintPageFooter,      NULL,               0,
    "printorientation",         B_PrintGdiOrient,       NULL,               0,
    "printduplex",              B_PrintGdiDuplex,       NULL,               0,
    "setprinter",               B_PrintGdiPrinter,      NULL,               0,
    
    "printgiftranscolor",       B_PrintGIFTransColor,   NULL,               0,
    "printgifbackcolor",        B_PrintGIFBackColor,    NULL,               0,
    "printgifinterlaced",       B_PrintGIFInterlaced,   NULL,               0,
    "printgiftransparent",      B_PrintGIFTransparent,  NULL,               0,
    "printgiffilled",           B_PrintGIFFilled,       NULL,               0,
    "printgiffont",             B_PrintGIFFont,         NULL,               0,
    
    "printhtmlstrip",           B_PrintHtmlStrip,       NULL,               0,
    "printhtmlstyle",           B_PrintHtmlStyle,       NULL,               0,
            
    "ddeget",                   B_DDEGet,               NULL,               0,
    "sysmovefile",              B_SysRename,            NULL,               0,
    "syscopyfile",              B_SysCopy,              NULL,               0,
    "sysappendfile",            B_SysAppend,            NULL,               0,
    "sysdeletefile",            B_SysDelete,            NULL,               0,
    "sysansitoutf8",            B_SysAnsiToUTF8,        NULL,               0,
    "sysoemtoutf8",             B_SysOemToUTF8,         NULL,               0,
    "sysansitooem",             B_SysAnsiToOem,         NULL,               0,
    "sysoemtoansi",             B_SysOemToAnsi,         NULL,               0,
                
    "a2mtohtml",                B_A2mToHtml,            NULL,               0,
    "a2mtomif",                 B_A2mToMif,             NULL,               0,
    "a2mtocsv",                 B_A2mToCsv,             NULL,               0,
    "a2mtortf",                 B_A2mToRtf,             NULL,               0,
    "a2mtoprinter",             B_A2mToPrinter,         NULL,               0,
                
    "csvsave",                  B_CsvSave,              NULL,               3,
    "csvdigits",                B_CsvNbDec,             NULL,               0,
    "csvsep",                   B_CsvSep,               NULL,               0,
    "csvdec",                   B_CsvDec,               NULL,               0,
    "csvnan",                   B_CsvNaN,               NULL,               0,
    "csvaxes",                  B_CsvAxes,              NULL,               0,




*/