/**
 *  @header4iode
 *  
 *  IODE API tests.
 *
 *  See function Syntax() .
 */

#include <stdarg.h>
#include <io.h>


//#include <unistd.h>

/* 
    Important note for code that must be compiled in both C and C++
    ---------------------------------------------------------------
    See https://learn.microsoft.com/fr-fr/cpp/cpp/extern-cpp?view=msvc-170.
    
    Assume 
        1. we want to use a variable from a library **compiled with a C compiler** 
        2. this variable is **declared** in a .h file without the extern "C" attribute
    
    Then the C++ compiler will "decorate" this variable in C++ style because it has no other 
    indication.
    
    Therefore, the linker will not be able to find this "decorated" variable in the 
    library compiled in C.
    
    How can we still use this variable in C++?
    
    Solution 1 (applied here): declare the variable as extern "C" BEFORE including the file that 
        contains its declaration. Indeed, as it is the FIRST declaration that counts for the compiler, 
        the following declarations are useless.
        
    Solution 2: declare all variables and functions as extern "C" in the include file, but only for the 
        C++ compiler because C does not allow extern "C" declarations. 
        For example:
        
            #ifdef __cplusplus      // On compile en C++
            extern "C" {
            #endif    
                extern "C" int  A2M_SEPCH;
                extern "C" char SCR_NAME_ERR[255 + 1];
            
            #ifdef __cplusplus 
            }
            #endif
*/

// // BEGIN_KEEP
// #ifdef __cplusplus
// extern "C" 
// {
// #endif
//     // extern int  A2M_SEPCH; // already extern "C" in s_a2m.h (new version)
//     // extern char SCR_NAME_ERR[255 + 1];
// #ifdef __cplusplus
// }
// #endif
// // END_KEEP

// iode.h is NOT TO BE INCLUDED in test_c_api.cpp because:
//  - api/iode.h is included in pch.h.
//  - iode.h is not in the include path
#include "iode.h"  

// BEGIN_KEEP
//#ifdef __cplusplus
//extern "C" 
//{
//#endif
//    int  A2mGIF_HTML(A2MGRF * go, U_ch * filename) { return(0); }
//#ifdef __cplusplus
//}
//#endif
// END_KEEP



// ============================================================================

void Syntax() 
{
    printf("Syntax: test1 [-v] [-v-] [-x] [-x-] [-h] [-vc64]\n");
    printf("  [-v]  : verbose (default)\n");
    printf("  [-v-] : silent\n");
    printf("  [-x]  : exit on error (default)\n");
    printf("  [-x-] : continue on error\n");
    printf("  [-data_dir] : data directory (default ..\\data)\n");
    printf("  [-output_dir] : output directory (default ..\\output)\n");
    printf("  [-h]  : display syntax\n");
    printf("Example: test1 -data_dir ..\\..\\data -output_dir ..\\..\\output\n");
    
    exit(0);
}

// BEGIN_KEEP
#ifdef _MSC_VER
//  #if defined(_M_X64)  || defined(__x86_64__) || defined(__amd64__) || defined(_WIN64)
    #ifdef _WIN64 
    char    *IODE_DATA_DIR   = "..\\data";
    char    *IODE_OUTPUT_DIR = "..\\output";
  #else
    char    *IODE_DATA_DIR   = "..\\data";
    char    *IODE_OUTPUT_DIR = "..\\output";
  #endif
#else
//    char    *IODE_DATA_DIR   = "..\\..\\api\\data";
//    char    *IODE_OUTPUT_DIR = "..\\..\\api\\output";
    char    *IODE_DATA_DIR   = "..\\data";
    char    *IODE_OUTPUT_DIR = "..\\output";

#endif

// Temporarily removed fns (to allow linking)

// Pour tester l'estimation
//#ifdef __cplusplus
//extern "C" 
//{
//#endif
//
//    extern char    SCR_NAME_ERR[255 + 1];
//    int     A2M_SEPCH;
//
//    //int o_estgr(char** titles, SAMPLE *smpl, MAT* mlhs, MAT* mrhs, int view, int res) {return(0);}
//    //int B_A2mSetRtfTitle(U_ch* title) {return(0);}
//    //int B_A2mSetRtfCopy(U_ch* copyr) {return(0);}
//    //int B_PrintRtfTopic(char* x) { return(0); }
int A2mGIF_HTML(A2MGRF *go, U_ch* filename) {return(0);} 
//    //int W_printf(char*fmt, ...) {return(0);}
//    //void K_load_iode_ini() {}
//    //void K_save_iode_ini() {}
//
//#ifdef __cplusplus
//}
//#endif

// END_KEEP
 
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
 *  Compares a table of strings to a list of strings in a | separated vector.
 *  
 *  Ex. 
 *      U_cmp_tbls((char**)SCR_vtom("A B C", ' '), "A|B|C") => return(1) => OK
 *      U_cmp_tbls((char**)SCR_vtom("A B", ' '),   "A|B|C") => return(0) => NOK
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

    tbl2 = (char**)SCR_vtoms((unsigned char*) vec, (unsigned char*) "|");
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
 *  Compares 2 files. 
 *  
 *  @return int     -1 if the 2 files are ==, 
 *                  0 if not.
 */
int U_cmp_files(char*file1, char*file2)
{
    int     rc = -1;        // ==
    long    size1, size2;
    char    *content1, *content2;
    char curdir[512];
    
    //printf("Comparing '%s' and '%s'\n", file1, file2);
    SCR_dosgetcwd(curdir, 511);
    printf("Current dir   : '%s'\n", curdir);    
    
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
 *  @return int     -1 if the 2 files are equal 
 *                  0 if not or if they don't exist
 */
int U_diff_files(char*file1, char*file2)
{
    int     rc = -1, i, j, nbdiffs = 0;        // ==
    long    size1, size2;
    char    *content1, *content2;
    char    **tbl1, **tbl2;
    
    printf("Comparing '%s' and '%s'\n", file1, file2);
    content1 = U_test_read_file(file1, &size1);
    printf("   '%s': size=%ld\n", file1, size1);
    content2 = U_test_read_file(file2, &size2);
    printf("   '%s': size=%ld\n", file2, size2);
    
    if(content1 == NULL && content2 == NULL) {              // ==
        rc = 0;     // JMP 23/10/2022
        goto fin;      
    }
    
    if(content1 == NULL || content2 == NULL) {
        if(content1 == NULL) printf("File %s not found\n", file1);
        if(content2 == NULL) printf("File %s not found\n", file2);
        rc = 0;
        goto fin;      // !=
    }    
    
    if(size1 != size2) {
        rc = 0;              // !=
        goto cmp;                           
    }

    rc = !memcmp(content1, content2, size1);
cmp:    
    if(rc == 0) { // files are different
        printf("Comparing '%s' and '%s'\n", file1, file2);
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
            if(nbdiffs >= 10) {
                printf("......\n");
                break;
            }    
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

//// Function which replaces A2mMessage() when no output is  needed
//void kmsg_null(char*msg) 
//{
//}
//
//// Choose to display a2m messages or not.
//void U_test_a2m_msgs(int IsOn)
//{
//    static int  Current_IsOn = 1;
//    static void (*A2mMessage_super_ptr)(char*); 
//    
//    if(IsOn && !Current_IsOn) { 
//        A2mMessage_super = A2mMessage_super_ptr; // (Re-)install default function
//        Current_IsOn = 1;
//        return;
//    }
//    else if(!IsOn && Current_IsOn) {
//        A2mMessage_super_ptr = A2mMessage_super;  // Save default value before replacing it by kmsg_null
//        A2mMessage_super = kmsg_null;             // Suppress output messages
//        Current_IsOn = 0;
//        return;
//    }
//}
//    
void U_test_suppress_a2m_msgs()
{
    A2mMessage_toggle(0);
}

void U_test_reset_a2m_msgs()
{
    A2mMessage_toggle(1);
}

 
// Choose to display kmsg messages or not.
//void U_test_kmsg_msgs(int IsOn)
//{
//    static int  Current_IsOn = 1;
//    static void (*kmsg_super_ptr)(char*); 
//    
//    
//    if(IsOn && !Current_IsOn) { 
//        kmsg_super = kmsg_super_ptr;
//        Current_IsOn = 1;
//        return;
//    }
//    else if(!IsOn && Current_IsOn) {
//        kmsg_super_ptr = kmsg_super;
//        kmsg_super = kmsg_null; 
//        Current_IsOn = 0;
//        return;
//    }
//}
//
    
void U_test_suppress_kmsg_msgs()
{
    kmsg_toggle(0);
}

void U_test_reset_kmsg_msgs()
{
    kmsg_toggle(1);
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
    double   A[64], B[64];
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
    
 #ifndef __cplusplus
    if(diff >= 1e-5 && S4ASSERT_VERBOSE) 
        printf("%lg != %lg\n", v1, v2);
#endif
    
    return(diff < 1e-5); 
}


/**
 *  Tests (some) LEC functions:
 *      - L_cc()
 *      - L_link()
 *      - L_exec()
 */
void U_test_lec(char* title, char* lec, int t, double expected_val)
{
    CLEC*   clec;
    //char    buf[256];
    double  calc_val;
    PERIOD  *per;
    char    aper[24];
    int     rc;
    
    per = PER_addper(&(KSMPL(KV_WS)->s_p1), t);
    PER_pertoa(per, aper);
    
    clec = L_cc(lec);
    //S4ASSERT ("L_cc", clec != 0);
    //S4ASSERT ("L_link", 0 == L_link(KV_WS, KS_WS, clec));
    rc = L_link(KV_WS, KS_WS, clec);
    S4ASSERT(rc == 0, "L_link(\"%s\") == 0", lec);
    calc_val = L_exec(KV_WS, KS_WS, clec, t);
    SCR_free(clec);
    //sprintf(buf, "Res=%10.3lf - Expected=%10.3lf %s L_exec(%s) in %s", calc_val, expected_val, title, lec, aper);
    S4ASSERT(U_test_eq(expected_val, calc_val), "Res=%10.3lf - Expected=%10.3lf %s L_exec(%s) in %s", calc_val, expected_val, title, lec, aper);
}

/**
 *  Calc a LEC expression in t
 */
double U_test_calc_lec(char* lec, int t)
{
    CLEC*   clec;
    double  res;

    clec = L_cc(lec);
    if(clec == NULL) return(IODE_NAN);
    if(L_link(KV_WS, KS_WS, clec)) return(IODE_NAN);
    res = L_exec(KV_WS, KS_WS, clec, t);
    SCR_free(clec);
    return(res);
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
    KE_RWS = KE_WS = U_test_K_interpret(EQUATIONS, filename);
    KS_RWS = KS_WS = U_test_K_interpret(SCALARS, filename);
    KV_RWS = KV_WS = U_test_K_interpret(VARIABLES, filename);
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
    char        asmpl1[80], asmpl2[80];

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
        
    // Test KV_sample()
    PER_smpltoa(KSMPL(KV_WS), asmpl1);
    KV_sample(KV_WS, NULL);
    PER_smpltoa(KSMPL(KV_WS), asmpl2);
    S4ASSERT(U_cmp_strs(asmpl1, asmpl2), "KV_sample(kdb, NULL) does not modify the current sample");
    
}    


/**
 *  Checks:
 *      - K_add() vs KTVAL() 
 */    
void Tests_TBL_ADD_GET()
{
    TBL     *tbl, *extracted_tbl;
    TCELL   *cells_0;
    TCELL   *cells_1;
    TLINE   *line_0;
    TLINE   *line_1;
    
    int     nb_columns = 2;
    char    *def = "A title";
    char    *vars = "GOSG,YDTG,DTH,DTF,IT,YSSG+COTRES,RIDG,OCUG"; // Note that semi-colon are not accepted by B_ainit_chk() (see b_args.c)
    char    **lecs;
    char    *name;
    char    *cell_cont_0;
    char    *cell_cont_1;
    int     mode = 1;
    int     files = 1;
    int     date = 1;
    int     pos, i, j;
    int     cond;
    
    U_test_print_title("Tests TBL: compare KTVAL() and T_create() results");
    
    // --- create a C struct TBL via T_auto()
    lecs = B_ainit_chk(vars, NULL, 0);
    tbl = T_create(nb_columns);
	T_auto(tbl, def, lecs, mode, files, date);
	SCR_free_tbl((unsigned char**) lecs);

    // --- add the table to the Tables KDB
    name = "c_table";
    K_add(KT_WS, name, tbl);

    // --- extract the table from the Table KDB
    pos = K_find(KT_WS, name);
    extracted_tbl = KTVAL(KT_WS, pos);


    // --- check that both table are exactly the same
    // ----- check all attributes that are not of type TLINE 
    S4ASSERT(tbl->t_lang    == extracted_tbl->t_lang   , "Comparing t_lang   ");
    S4ASSERT(tbl->t_free    == extracted_tbl->t_free   , "Comparing t_free   ");
    S4ASSERT(tbl->t_nc      == extracted_tbl->t_nc     , "Comparing t_nc     ");
    S4ASSERT(tbl->t_nl      == extracted_tbl->t_nl     , "Comparing t_nl     ");
    S4ASSERT(tbl->t_zmin    == extracted_tbl->t_zmin   , "Comparing t_zmin   ");
    S4ASSERT(tbl->t_zmax    == extracted_tbl->t_zmax   , "Comparing t_zmax   ");
    S4ASSERT(tbl->t_ymin    == extracted_tbl->t_ymin   , "Comparing t_ymin   ");
    S4ASSERT(tbl->t_ymax    == extracted_tbl->t_ymax   , "Comparing t_ymax   ");
    S4ASSERT(tbl->t_attr    == extracted_tbl->t_attr   , "Comparing t_attr   ");
    S4ASSERT(tbl->t_box     == extracted_tbl->t_box    , "Comparing t_box    ");
    S4ASSERT(tbl->t_shadow  == extracted_tbl->t_shadow , "Comparing t_shadow ");
    S4ASSERT(tbl->t_gridx   == extracted_tbl->t_gridx  , "Comparing t_gridx  ");
    S4ASSERT(tbl->t_gridy   == extracted_tbl->t_gridy  , "Comparing t_gridy  ");
    S4ASSERT(tbl->t_axis    == extracted_tbl->t_axis   , "Comparing t_axis   ");
    S4ASSERT(tbl->t_align   == extracted_tbl->t_align  , "Comparing t_align  ");

    // ----- check div line 
    S4ASSERT(tbl->t_div.tl_type  == extracted_tbl->t_div.tl_type    , "Comparing t_div.tl_type ");
    S4ASSERT(tbl->t_div.tl_graph == extracted_tbl->t_div.tl_graph   , "Comparing t_div.tl_graph");
    S4ASSERT(tbl->t_div.tl_axis  == extracted_tbl->t_div.tl_axis    , "Comparing t_div.tl_axis ");
    S4ASSERT(tbl->t_div.tl_pbyte == extracted_tbl->t_div.tl_pbyte   , "Comparing t_div.tl_pbyte");
    cells_0 = (TCELL*) tbl->t_div.tl_val;
    cells_1 = (TCELL*) extracted_tbl->t_div.tl_val;
    
    for(j = 0; j < tbl->t_nc; j++)
    {
        S4ASSERT(cells_0[j].tc_type == cells_1[j].tc_type,  "Comparing div: cells_0[%d].tc_type == cells_1[%d].tc_type == %d", j, j, cells_1[j].tc_type);
        S4ASSERT(cells_0[j].tc_attr == cells_1[j].tc_attr,  "Comparing div: cells_0[%d].tc_attr == cells_1[%d].tc_attr == %d", j, j, cells_1[j].tc_attr);
    }

    // ----- check all lines 
    for(i = 0; i < tbl->t_nl; i++)
    {
        line_0 = tbl->t_line + i;
        line_1 = extracted_tbl->t_line + i;
    
        S4ASSERT(line_0->tl_type  == line_1->tl_type  , "line %d: line_0->tl_type  == line_1->tl_type  == %d", i, line_0->tl_type);
        S4ASSERT(line_0->tl_graph == line_1->tl_graph , "line %d: line_0->tl_graph == line_1->tl_graph == %d", i, line_0->tl_graph);
        S4ASSERT(line_0->tl_axis  == line_1->tl_axis  , "line %d: line_0->tl_axis  == line_1->tl_axis  == %d", i, line_0->tl_axis );
        S4ASSERT(line_0->tl_pbyte == line_1->tl_pbyte , "line %d: line_0->tl_pbyte == line_1->tl_pbyte == %d", i, line_0->tl_pbyte);
    
        cells_0 = (TCELL*) line_0->tl_val;
        cells_1 = (TCELL*) line_1->tl_val;
        switch (line_0->tl_type)
        {
          case KT_TITLE:
            cell_cont_0 = (char*) SCR_stracpy((unsigned char*)T_cell_cont(cells_0, 0));
            cell_cont_1 = (char*) SCR_stracpy((unsigned char*)T_cell_cont(cells_1, 0));
            S4ASSERT(U_cmp_strs(cell_cont_0, cell_cont_1) == 1, "line %d, KT_TITLE: cell contents == '%s'", i, cell_cont_1);
            SCR_free(cell_cont_0);
            SCR_free(cell_cont_1);
            break;
          case KT_CELL:
            for(j = 0; j < tbl->t_nc; j++)
            {
                S4ASSERT(cells_0[j].tc_type == cells_1[j].tc_type, "line %d, col %d: tc_type == %d", i, j, cells_1[j].tc_type);
                S4ASSERT(cells_0[j].tc_attr == cells_1[j].tc_attr, "line %d, col %d: tc_attr == %d", i, j, cells_1[j].tc_attr);
                cell_cont_0 = (char*)SCR_stracpy((unsigned char*)T_cell_cont(cells_0, j));
                cell_cont_1 = (char*)SCR_stracpy((unsigned char*)T_cell_cont(cells_1, j));
                S4ASSERT(U_cmp_strs(cell_cont_0, cell_cont_1) == 1, "line %d, col %d: cell contents == '%s'", i, j, cell_cont_1);
                SCR_free(cell_cont_0);
                SCR_free(cell_cont_1);
            }
            break;
          default:
            cond = cells_0 == NULL && cells_1 == NULL;
            S4ASSERT(cond != 0, "Other line: cells NULL by default");
            break;
        }
    }

    // --- free memory
    T_free(tbl);
    T_free(extracted_tbl);
}


/**
 *  Tests some LEC calculations.
 */
void Tests_LEC()
{
    double *A, *B;

    U_test_print_title("Tests LEC");

    // Create objects
    U_test_CreateObjects();
    
    A = (double*)KVPTR("A");
    B = (double*)KVPTR("B");
    // Tests LEC
    U_test_lec("LEC", "A+B",  2, A[2]+B[2]);
    U_test_lec("LEC", "ln A", 2, log(A[2]));
    U_test_lec("LEC", "A[2002Y1]",     2, A[2]);
    //S4ASSERT(0, "Generated error");
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
    args = B_ainit_chk("A B;C,D", NULL, 0); // => "A" "B;C" "D"
    S4ASSERT(U_cmp_tbls(args, "A|B;C|D"), "B_ainit_chk(\"A B;C,D\")");
    SCR_free_tbl((unsigned char**) args);
    
    // A_init
    args = B_ainit_chk("$LST1", NULL, 0);
    S4ASSERT(U_cmp_tbls(args, "A|B"), "B_ainit_chk(\"$LST1\")");
    SCR_free_tbl((unsigned char**) args);
    //args = B_ainit_chk("A*", NULL, 0);
    
    // Test parameters in a file. test.args must exist in the current dir and contain the line
    // A1 A2
    sprintf(filename, "@%s\\test.args", IODE_DATA_DIR);
    args = B_ainit_chk(filename, NULL, 0);
    S4ASSERT(U_cmp_tbls(args, "A1|A2"), "B_ainit_chk(\"%s\")", filename);
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
    
    kdb_var = K_interpret(VARIABLES, in_filename);
    S4ASSERT(kdb_var != NULL, "K_interpret(VARIABLES, \"%s\")", in_filename);
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
   
    kdb_tbl = K_interpret(TABLES, in_filename);
    S4ASSERT(kdb_tbl != NULL, "K_interpret(TABLES, \"%s\")", in_filename);
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
    //printf("Address(cells) =     %0x\nAddress(cells + 1) = %0x\n", cells, cells + 1);
    printf("Address(cells) =     %p\nAddress(cells + 1) = %p\n", cells, cells + 1);
    printf("Diff(cells, cells+1) = %d\n", (int)((char*)(cells + 1) - (char*)(cells)));
    
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
    void        (*kmsg_super_ptr)(const char*);
    double      XNATY_2000Y1;
    
    
    U_test_print_title("Tests Simulation");

    // Loads 3 WS and check ok
    U_test_load_fun_esv(filename);

    // Check
    kdbv = KV_WS;
    S4ASSERT(kdbv != NULL, "K_interpret(VARIABLES, \"%s\")", filename);
    kdbs = KS_WS;
    S4ASSERT(kdbs != NULL, "K_interpret(SCALARS, \"%s\")", filename);
    kdbe = KE_WS;
    S4ASSERT(kdbe != NULL, "K_interpret(EQUATIONS, \"%s\")", filename);
    
    // Check list is empty
    lst = KLPTR("_DIVER");
    S4ASSERT(U_cmp_strs(lst, NULL), "_DIVER=NULL", lst);
    
    // Simulation std parameters
    smpl = PER_atosmpl("2000Y1", "2002Y1");
    KSIM_START = VAR_INIT_TM1;
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
    //S4ASSERT(U_test_eq(KV_get_at_aper("ACAF", "2002Y1"), -1.2747388), "ACAF[2002Y1] = -1.27474");

    // Test Endo-exo
    
    // Version with exchange in one equation only
    // endo_exo = SCR_vtoms("UY-NIY", ",; ");
    // rc = K_simul(kdbe, kdbv, kdbs, smpl, endo_exo, NULL);
    // S4ASSERT(rc == 0, "Exchange UY-NIY converges on 2000Y1-2002Y1");
    // S4ASSERT(UY[pos2000] == 650.0, "Exchange UY-NIY: UY[2000Y1] == 650.0");
    // S4ASSERT(fabs(NIY[pos2000] - 658.423) < 0.01, "Exchange UY-NIY: NIY[2000Y1] == 658.423");

    // Version with exchange in at least 2 equations
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
    XNATY_2000Y1 = KV_get_at_aper("XNATY", "2000Y1"); 
    //printf("XNATY_2000Y1 = %lg\n", XNATY_2000Y1);
    S4ASSERT(U_test_eq(KV_get_at_aper("XNATY", "2000Y1"), 0.800703), "Exchange UY-XNATY: XNATY[2000Y1] == 0.800703");

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
    K_RWS[VARIABLES][0] = K_WS[VARIABLES] = kdbv  = U_test_K_interpret(VARIABLES, "fun.var");
    S4ASSERT(kdbv != NULL, "K_interpret(VARIABLES, \"%s\")", "fun.var");

    // Load the TBL workspace
    K_RWS[TABLES][0] = K_WS[TABLES] = kdbt  = U_test_K_interpret(TABLES, "fun.tbl");
    S4ASSERT(kdbt != NULL, "K_interpret(TABLES, \"%s\")", "fun.tbl");

    // Load a second VAR workspace in K_RWS[VARIABLES][2]
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
    rc = V_graph(0, 0, 0, 1, 1, 0, IODE_NAN, IODE_NAN, smpl, varlist);
    S4ASSERT(rc == 0, "V_graph(0, 0, 0, 1, 1, 0, IODE_NAN, IODE_NAN, 2000Y1:2020Y1, ACAF;ACAG)");
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
    void        (*kmsg_super_ptr)(const char*);
    SAMPLE      *smpl;
    double   r2, *df;

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

    //x = U_test_calc_lec("_YRES0[1980Y1]", 0);
    //printf("x = %lf\n", x);
    //x = fabs(x + 0.001150);
    S4ASSERT(U_test_eq(U_test_calc_lec("_YRES0[1980Y1]", 0), -0.00115008), "Estimation : _YRES0[1980Y1] == -0.001150");
    
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
    df = E_UnitRoot("ACAF+ACAG", 0, 0, 0);
    S4ASSERT(U_test_eq(df[2], -1.602170), "E_UnitRoot(\"ACAF+ACAG\", 0, 0, 0) == -1.602170");
    df = E_UnitRoot("ACAF+ACAG", 1, 0, 0);
    S4ASSERT(U_test_eq(df[2], -2.490054), "E_UnitRoot(\"ACAF+ACAG\", 1, 0, 0) == -2.49005");
    df = E_UnitRoot("ACAF+ACAG", 1, 1, 0);
    S4ASSERT(U_test_eq(df[2], -2.638717), "E_UnitRoot(\"ACAF+ACAG\", 1, 1, 0) == -2.638717");
    df = E_UnitRoot("ACAF+ACAG", 0, 0, 1);
    S4ASSERT(U_test_eq(df[2], -1.300049), "E_UnitRoot(\"ACAF+ACAG\", 0, 0, 1) == -1.300049");
        
    
    // Reset initial kmsg fn
    kmsg_super = kmsg_super_ptr; // Reset initial output to 
    U_test_reset_a2m_msgs();

}

void Tests_ALIGN()
{
    TBL     tbl, *p_tbl = &tbl;
    int     offset;
    
    U_test_print_title("Tests ALIGN");

    offset = (int) ((char*)(p_tbl + 1) - (char*)p_tbl);
    printf("sizeof(TBL)    = %d -- Offset = %d\n", (int)sizeof(TBL), offset);
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
    //printf("Comparing ref '%s' and '%s'\n", reffilename, filename);
    //S4ASSERT(U_cmp_files(reffilename, filename), "W_printf -> %s", typeext);
    S4ASSERT(U_diff_files(reffilename, filename) != 0, "W_printf -> %s", typeext);
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
    
    // test 2 :  reusing freed space
    item = SW_alloc(15);
    SW_free(item);
    item2 = SW_alloc(10);
    SW_free(item);
    SW_free(item2);
}



void Tests_B_DATA()
{
    char        *lst, buf[512];
    int         rc, i, cond;
    double   *A1, val;
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
    // Foireux. Faut utiliser des listes (avec A;B au lieu de $AB ca marche pas...) => A changer ? Voir B_DataListSort() 
    B_DataPattern("RC xy $AB $BC", VARIABLES); 
    lst = KLPTR("RC");
    S4ASSERT(U_cmp_strs(lst, "AB,AC,BB,BC"), "B_DataPattern(\"RC xy $AB $BC\", VARIABLES) = \"AB,AC,BB,BC\"");

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
        
        if(i != EQUATIONS) { // Equations cannot be renamed or duplicated
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

    // B_DataListSort() Example 2
    K_add(KL_WS, "L1", "C;B;$L2;$L3");
    K_add(KL_WS, "L2", "X Z Y");
    K_add(KL_WS, "L3", "A B D");
    rc = B_DataListSort("L1 RES");
    lst = KLPTR("RES");
    S4ASSERT(U_cmp_strs(lst, "A;B;B;C;D;X;Y;Z"), "B_DataListSort(\"L1 RES\") = \"%s\"", lst);
    
    // B_DataUpdate()
    rc = B_DataUpdate("U Comment of U"       , COMMENTS);
    cond = (rc == 0) && U_cmp_strs(KCPTR("U"), "Comment of U");
    S4ASSERT(cond == 1, "B_DataUpdate(\"U Comment of U\", COMMENTS) = \"%s\"", KCPTR("U"));
    
    rc = B_DataUpdate("U U := c1 + c2*Z"     , EQUATIONS);
    
    rc = B_DataUpdate("U 2 * A"              , IDENTITIES);
    cond = (rc == 0) && U_cmp_strs(KIPTR("U"), "2 * A");
    S4ASSERT(cond == 1, "B_DataUpdate(\"U 2 * A\", IDENTITIES) = \"%s\"", KIPTR("U"));
    
    rc = B_DataUpdate("U A,B,C"             , LISTS);
    cond = (rc == 0) && U_cmp_strs(KLPTR("U"), "A,B,C");
    S4ASSERT(cond == 1, "B_DataUpdate(\"U A,B,C\", LISTS) = \"%s\"", KLPTR("U"));
    
    rc = B_DataUpdate("U  1.2 1"             , SCALARS);  
    val = K_s_get_value (KS_WS, "U");
    cond = (rc == 0) && U_test_eq(1.2, val);
    S4ASSERT(cond == 1, "B_DataUpdate(\"U 1.2 1\", SCALARS) = %g", val);
    
    rc = B_DataUpdate("U  Title of U;U;2*U"  , TABLES);
    smpl = KSMPL(KV_WS);
    rc = B_DataUpdate("U L 2000Y1 2 3.1 4e2" , VARIABLES);
    S4ASSERT(rc == 0, "B_DataUpdate(\"U L 2000Y1 2 3.1 4e2\" , VARIABLES)");
    
    // B_DataSearch(char* arg, int type)
    rc = B_DataSearch("of 0 0 1 0 1 NEWLIST", COMMENTS);
    cond = (rc == 0) && U_cmp_strs(KLPTR("NEWLIST"), "U");
    S4ASSERT(cond == 1, "B_DataSearch(\"of 0 0 1 0 1 NEWLIST\", COMMENTS) = \"%s\"", KLPTR("NEWLIST"));
    
    // B_DataScan(char* arg, int type)
    rc = B_DataScan("U", EQUATIONS);
    cond = (rc == 0) && U_cmp_strs(KLPTR("_SCAL"), "c1;c2");
    S4ASSERT(cond == 1, "B_DataScan(\"U\", EQUATIONS) = \"%s\"", KLPTR("_SCAL"));
     
    // B_DataExist(char* arg, int type)
    rc = B_DataExist("_SCAL", LISTS);
    S4ASSERT((rc >= 0), "B_DataExist(\"_SCAL\", LISTS) = %d", rc);

    // B_DataAppend(char* arg, int type)
    rc = B_DataAppend("_SCAL XXX,YYY", LISTS);
    cond = (rc == 0) && U_cmp_strs(KLPTR("_SCAL"), "c1;c2,XXX,YYY");
    S4ASSERT(cond == 1, "B_DataAppend(\"_SCAL XXX,YYY\", LISTS) = %d", rc);

    rc = B_DataAppend("U - More comment on U", COMMENTS);
    cond = (rc == 0) && U_cmp_strs(KCPTR("U"), "Comment of U - More comment on U");
    S4ASSERT(cond == 1, "B_DataAppend(\"U - More comment on U\", COMMENTS) = %d", rc);

    // B_DataList(char* arg, int type)
    rc = B_DataList("LC ac*", SCALARS);
    cond = (rc == 0) && U_cmp_strs(KLPTR("LC"), "acaf1;acaf2;acaf3;acaf4");
    printf("LC = \"%s\"\n", KLPTR("LC"));
    S4ASSERT(cond == 1, "B_DataList(\"LC ac*\", SCALARS); = '%s'", KLPTR("LC"));

    // B_DataCalcLst(char* arg)
    B_DataUpdate("LST1 A,B,C", LISTS);
    B_DataUpdate("LST2 C,D,E", LISTS);
    
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
    rc = B_DataCompare(buf, LISTS);
    //printf("    WS_ONLY='%s'\n", KLPTR("WS_ONLY"));
    //printf("    FILE_ONLY='%s'\n", KLPTR("FILE_ONLY"));
    //printf("    BOTH_DIFF='%s'\n", KLPTR("BOTH_DIFF"));
    //printf("    BOTH_EQ='%s'\n", KLPTR("BOTH_EQ"));

    cond = (rc == 0) && U_cmp_strs(KLPTR("WS_ONLY"), "AB;BC;L1;L2;L3;LC;LIST1;LIST2;LST1;LST2;NEWLIST;RC;RES;U;ZZZ;_EXO;_RES");
    //printf("%s\n", KLPTR("WS_ONLY"));
    S4ASSERT(cond == 1, "B_DataCompare()");

    // B_DataPrintGraph()
    //smpl = KSMPL(KV_WS);
    //printf("%d\n", smpl->s_p1);
    rc = B_DataPrintGraph("Grt Line No No Level -- -- 2000Y1 2020Y1 ACAF ACAG ACAF+ACAG");  
    S4ASSERT(rc == 0, "B_DataPrintGraph(\"Grt Line No No Level -- -- 1990Y1 2010Y1 ACAF ACAG ACAF+ACAG\")");
    
    // B_DataDisplayGraph()
    rc = B_DataDisplayGraph("Grt Line No No Level -- -- 2000Y1 2020Y1 ACAF ACAG ACAF+ACAG");  
}


void Tests_B_EQS()
{
    int     rc, cond, pos;
    SAMPLE  *smpl;
    char    cmd_B_EqsEstimate[] = "1980Y1 1996Y1 ACAF";
    char    cmd_B_EqsSetSample[] = "1981Y1 1995Y1 ACAF";
    
    U_test_print_title("Tests B_EQS");
    U_test_suppress_kmsg_msgs();

    // (Re-)loads 3 WS and check ok
    U_test_load_fun_esv("fun");
   
    // B_EqsEstimate()
    rc = B_EqsEstimate(cmd_B_EqsEstimate);
    
    cond = (rc == 0) && U_test_eq(K_e_r2(KE_WS, "ACAF"), 0.821815);
    S4ASSERT(cond == 1, "B_EqsEstimate(\"1980Y1 1996Y1 ACAF\") => R2 == 0.821815");
    cond = (rc == 0) && U_test_eq(K_e_fstat(KE_WS, "ACAF"), 32.285108);
    S4ASSERT(cond == 1, "B_EqsEstimate(\"1980Y1 1996Y1 ACAF\") => fstat == 32.285108");
      
    // B_EqsSetSample()    
    rc = B_EqsSetSample(cmd_B_EqsSetSample);
    pos = K_find(KE_WS, "ACAF");
    smpl = &KESMPL(KE_WS, pos);
    cond = (rc ==0) && (smpl->s_p1.p_y == 1981);
    S4ASSERT(cond == 1, "rc = B_EqsSetSample(\"%s\")", cmd_B_EqsSetSample);
    
    // TODO: implement next utests with the same canevas
        //B_EqsSetMethod(char* arg)   
        //B_EqsSetBloc(char* arg)  
        //B_EqsSetCmt(char* arg)   
        //B_EqsSetInstrs(char* arg)
          
    U_test_reset_kmsg_msgs();
}


/**
 *  Compares a file generated by the program in the IODE_OUTPUT_DIR
 *  to a reference file in the IODE_DATA_DIR.
 *  
 *  @param [in] char*   outfile     filename (in output dir)
 *  @param [in] char*   reffile     reference filename (in data dir)
 *  @return     int                 -1 if the 2 files are equal 
 *                                  0 if not.
 */
int U_test_compare_outfile_to_reffile(char* outfile, char* reffile)
{
    char reffilename[512];
    char filename[512];
    char curdir[512];

    sprintf(filename, "%s\\%s", IODE_OUTPUT_DIR, outfile);
    sprintf(reffilename, "%s\\%s", IODE_DATA_DIR, reffile);
    SCR_dosgetcwd(curdir, 511);
    //getcwd(curdir, 500);
    printf("Current dir   : '%s'\n", curdir);
    printf("Output    file: '%s'\n", filename);
    printf("Reference file: '%s'\n", reffilename);
    
    //printf("Comparing ref '%s' and '%s'\n", reffilename, filename);
    //return(U_cmp_files(reffilename, filename));
    return(U_diff_files(reffilename, filename));
}


/**
 *  Compares a file generated by the program in the IODE_OUTPUT_DIR
 *  to a reference file in the IODE_DATA_DIR.
 *  
 *  @param [in] char*   outfile     filename (in output dir)
 *  @param [in] char*   reffile     reference filename (in data dir)
 *  @return     int                 -1 if the 2 files are equal 
 *                                  0 if not.
 */
int U_test_compare_localfile_to_reffile(char* outfile, char* reffile)
{
    char reffilename[512];
    char filename[512];

    sprintf(filename, ".\\%s", outfile);
    sprintf(reffilename, "%s\\%s", IODE_DATA_DIR, reffile);
    return(U_diff_files(reffilename, filename));
}


void U_test_create_a_file(char* filename, int type)
{
    //  Create a file
    U_test_suppress_a2m_msgs();
    W_dest(filename, type); 
    W_printf("This is a paragraph with accents: �����\n"); // the current source file (test1.c) is ANSI coded
    W_close();
}


int U_test_file_exists(char* filename, char* msg )
{
    int     rc;
    
    //  Check that the file exists
    rc = _access(filename, 0);
    S4ASSERT(rc == 0, msg, filename);
    return(rc == 0);
}

int U_test_file_not_exists(char* filename, char* msg)
{
    int     rc;
    
    //  Check that the file exists
    rc = _access(filename, 0);
    S4ASSERT(rc != 0, msg, filename);
    return(rc != 0);
}


void Tests_B_FILE()
{
    int     rc;
    
    U_test_print_title("Tests B_FILE");

    // Cleanup files
    _unlink("toto.a2m");
    _unlink("tata.a2m");
    _unlink("tutu.a2m");
    
    //  Create a file
    U_test_create_a_file("toto", W_A2M); 
    
    //  Check that the file exists
    U_test_file_exists("toto.a2m", "File %s exists");

    //  Call B_FileRename()
    rc = B_FileRename("toto tata", K_A2M);
    rc = U_test_file_exists("tata.a2m", "B_FileRename(\"toto tata\", K_A2M)");
    
    //  Call B_FileCopy()
    rc = B_FileCopy("tata tutu", K_A2M);
    rc = U_test_file_exists("tutu.a2m", "B_FileCopy(\"tata tutu \", K_A2M)");
    
    //  Call B_FileDelete()
    rc = B_FileDelete("tata tutu", K_A2M);
    rc = U_test_file_not_exists("tata.a2m", "B_FileDelete(\"tata tutu\", K_A2M)");

/*
    "fileimportvar",            B_FileImportVar,        SB_XodeRuleImport,  0,
    "fileimportcmt",            B_FileImportCmt,        SB_XodeRuleImport,  0,
*/
   
    U_test_reset_a2m_msgs();
}

void Tests_B_FSYS()
{
    int     rc, cond;
    char    arg[512];
    
    U_test_print_title("Tests B_FSYS");
 
    // Cleanup files
    _unlink("toto.a2m");
    _unlink("toto.a2m.oem");
    _unlink("toto.a2m.ansi");
    _unlink("totodbl.a2m.ansi");
    _unlink("brol.a2m.ansi");
    _unlink("brol2.a2m.ansi");
    
    
    //  Create toto.a2m -> ansi-coded file
    // sprintf(arg, "%s\\toto", IODE_OUTPUT_DIR);
    // U_test_create_a_file("toto", W_A2M); // Ansi-coded file 
    // => Pb with conversions => files created by test1.c differ from those created by test_c_api.cpp. 
    // Therefore, we copy data\toto.a2m into local toto.a2m
    sprintf(arg, "%s\\toto.a2m toto.a2m", IODE_DATA_DIR);
    rc = B_SysCopy(arg);
    cond = (rc == 0) && U_test_compare_localfile_to_reffile("toto.a2m", "toto.a2m");
    S4ASSERT(cond == 1, "B_SysCopy(\"%s\")", arg);

    
    // B_SysAnsiToOem() : translate ansi to oem -> toto.a2m.oem
    sprintf(arg, "toto.a2m toto.a2m.oem");
    rc = B_SysAnsiToOem(arg); 
    cond = (rc == 0) && U_test_compare_localfile_to_reffile("toto.a2m.oem", "toto.a2m.oem.ref");
    S4ASSERT(cond == 1, "B_SysAnsiToOem(\"%s\")", arg);
    
    // B_SysAnsiToUTF8() : translate ansi to utf8 -> toto.a2m.utf8
    sprintf(arg, "toto.a2m toto.a2m.utf8");
    rc = B_SysAnsiToUTF8(arg); 
    cond = (rc == 0) && U_test_compare_localfile_to_reffile("toto.a2m.utf8", "toto.a2m.utf8.ref");
    S4ASSERT(cond == 1, "B_SysAnsiToUTF8(\"%s\")", arg);

    // B_SysOemToAnsi() : translate oem to ansi -> toto.a2m.ansi
    sprintf(arg, "toto.a2m.oem toto.a2m.ansi");
    rc = B_SysOemToAnsi(arg);
    cond = (rc == 0) && U_test_compare_localfile_to_reffile("toto.a2m.ansi", "toto.a2m.ansi.ref");
    S4ASSERT(cond == 1, "B_SysOemToAnsi(\"%s\")", arg);
  
    // B_SysOemToUTF8() : translate ansi to utf8 -> toto.a2m.utf8
    sprintf(arg, "toto.a2m.oem toto.a2m.utf8");
    rc = B_SysOemToUTF8("toto.a2m.oem toto.a2m.utf8"); 
    cond = (rc == 0) && U_test_compare_localfile_to_reffile("toto.a2m.utf8", "toto.a2m.utf8.ref");
    S4ASSERT(cond == 1, "B_SysOemToUTF8(\"%s\")", arg);

    // B_SysRename(char* arg) : rename toto.a2m.ansi -> brol.a2m.ansi
    sprintf(arg, "toto.a2m.ansi brol.a2m.ansi");
    rc = B_SysRename(arg);
    cond = (rc == 0) && U_test_compare_localfile_to_reffile("brol.a2m.ansi", "toto.a2m.ansi.ref");
    S4ASSERT(cond == 1, "B_SysRename(\"%s\")", arg);
    
    // B_SysCopy(char* arg) : copy brol.a2m.ansi dans totodbl.a2m.ansi
    sprintf(arg, "brol.a2m.ansi totodbl.a2m.ansi");
    rc = B_SysCopy(arg);
    cond = (rc == 0) && U_test_compare_localfile_to_reffile("totodbl.a2m.ansi", "toto.a2m.ansi.ref");
    S4ASSERT(cond == 1, "B_SysCopy(\"%s\")", arg);

    // B_SysAppend(char* arg) : append totodbl.a2m.ansi to brol.a2m.ansi 
    sprintf(arg, "brol.a2m.ansi totodbl.a2m.ansi");
    rc = B_SysAppend(arg);
    cond = (rc == 0) && U_test_compare_localfile_to_reffile("totodbl.a2m.ansi", "totodbl.a2m.ansi.ref");
    S4ASSERT(cond == 1, "B_SysAppend(\"%s\")", arg);


    // B_SysDelete(char* arg) : delete totdbl.a2m.ansi and brol.a2m.ansi
    rc = B_SysDelete("brol.a2m.ansi totodbl.a2m.ansi tutu.a2m toto.a2m toto.a2m.oem toto.a2m.utf8");
    rc = U_test_file_not_exists("brol.a2m.ansi",  "B_SysDelete(\"brol.a2m.ansi\")");
    rc = U_test_file_not_exists("brol2.a2m.ansi", "B_SysDelete(\"totodbl.a2m.ansi\")");
      
    U_test_reset_a2m_msgs();
}


// Test of identities calculation (b_idt.c).
// k_exec.c is implicitly tested via the calls to b_idt.c functions.

void Tests_B_IDT()
{
    char        filename[256];
    char        idtexec[] = "2002Y1 2007Y1 C D";
    char        idtexec2[] = "2002Y1 2007Y1 C D";
    int         rc;
    double   *C, *D;
   
    U_test_print_title("Tests B_IDT");

    // Init=> clear ws
    K_clear(KC_WS);
    K_clear(KI_WS);
    K_clear(KL_WS);
    K_clear(KT_WS);
    
    U_test_CreateObjects(); // Create vars on 2000Y1:2010Y1 => A=[0, 1...], B=[0, 2, 4...], BC...
    K_add(KI_WS, "C", "D*2+ACAF");
    K_add(KI_WS, "D", "A+B");
   
       
    // Trace the execution
    W_dest("test_idt", W_HTML);
    rc = B_IdtExecuteTrace("Yes");
    
    // Erroneously define input filenames (WS forgotten !!)
    sprintf(filename,  "%s\\fun", IODE_DATA_DIR);
    rc = B_IdtExecuteVarFiles(filename);
    S4ASSERT(rc == 0, "B_IdtExecuteVarFiles(\"%s\")", filename);
    
    // Compute the idts of a partial sample 
    rc = B_IdtExecute(idtexec);
    S4ASSERT(rc != 0, "B_IdtExecute(\"%s\") != 0 without WS", idtexec);

    // Second trial with WS in filenames
    K_clear(KV_WS);
    U_test_CreateObjects(); // Create vars on 2000Y1:2010Y1 => A=[0, 1...], B=[0, 2, 4...], BC...

    sprintf(filename,  "WS %s\\fun", IODE_DATA_DIR);
    rc = B_IdtExecuteVarFiles(filename);
    S4ASSERT(rc == 0, "B_IdtExecuteVarFiles(\"%s\")", filename);
    
    // Compute the idts of a partial sample 
    rc = B_IdtExecute(idtexec2);
    S4ASSERT(rc == 0, "B_IdtExecute(\"%s\") == 0 with WS", idtexec2);


    // Check the values
    C = (double*)KVPTR("C");
    D = (double*)KVPTR("D");
    
    S4ASSERT(U_test_eq(D[1], IODE_NAN), "(A+B)[1] is NaN");        // 1 == 2001Y1
    S4ASSERT(U_test_eq(D[2], 2.0 + 4.0), "(A+B)[2] == 6.0");    // 2 == 2002Y1
    S4ASSERT(U_test_eq(C[2], 6.0*2 - 0.92921251 ), "C[2] ==6.0*2 - 0.92921251");
}


// Test of identities execution for Python (b_api.c).
void Tests_B_IDT_EXECUTE()
{
    double  *AOUC;
    int     rc;
    
    U_test_print_title("Tests B_IDT_EXECUTE");
    
    K_free(KV_WS);
    K_free(KI_WS);
    //K_free(KS_WS);
        
    // Loads 3 WS and check ok
    KI_RWS = KI_WS = U_test_K_interpret(IDENTITIES, "fun");
    KV_RWS = KV_WS = U_test_K_interpret(VARIABLES, "fun");
    //KS_RWS = KS_WS = U_test_K_interpret(SCALARS, "fun");
    
    //iode.ws_load_idt(f"{IODE_DATA_DIR}fun")
    //iode.ws_load_var(f"{IODE_DATA_DIR}fun")
    //iode.ws_load_scl(f"{IODE_DATA_DIR}fun")
    
    AOUC = KVPTR("AOUC");
    AOUC[1] = 0.1;
    //iode.set_var("AOUC", AOUC)
    
    //rc = iode.idt_execute("1961:2015", "AOUC")
    //print(f"rc={rc}")
    rc = IodeExecuteIdts("1961Y1:2015Y1", "AOUC", NULL, NULL, 1);
    printf("rc=%d\n", rc);
    
    //AOUC = iode.get_var("AOUC")
    //test_eq(f"AOUC[1961Y1]", 0.24783192, AOUC[1])
    
    //varname = "AOUC"
    //res = iode.exec_lec(f"{varname}[1961Y1]", 0)
    //test_eq(f"{varname}[1961Y1]", 0.24783192, res)
    S4ASSERT(U_test_eq(AOUC[1], 0.24783192), "AOUC[1961Y1] == 0.24783192");   
    
}

void Tests_IMP_EXP()
{
    char    outfile[256];
    char    reffile[256];
    char    varfile[256];
    char    cmtfile[256];
    char    rulefile[256];
    char    trace[] = " ";    
    int     cond, rc;
    
    U_test_print_title("Tests EXP: Export CSV and rcsv");

    U_test_suppress_kmsg_msgs();

    // Export
    // Exports VAR files into external formats. 
    // int EXP_RuleExport(" "char* trace, NULL, char* out, char* vfile, char* cfile, char* from, char* to, char* na, char* sep, int fmt)  
    
    sprintf(varfile, "%s\\fun.var", IODE_DATA_DIR);
    sprintf(cmtfile, "%s\\fun.cmt", IODE_DATA_DIR);
    
    sprintf(outfile, "%s\\fun_xode.csv", IODE_OUTPUT_DIR);
    sprintf(reffile, "%s\\fun_xode.csv.ref", IODE_DATA_DIR);
    rc = EXP_RuleExport(trace, NULL, outfile, varfile, cmtfile, "2000Y1", "2010Y1", "#N/A", ";", EXPORT_CSV);
    cond = (rc == 0) && U_test_compare_outfile_to_reffile("fun_xode.csv", "fun_xode.csv.ref");
    S4ASSERT(cond == 1, "EXP_RuleExport(\" \", NULL, \"%s\", \"%s\", \"%s\", \"2000Y1\", \"2010Y1\", \"#N/A\", \";\", EXPORT_CSV)", outfile, varfile, cmtfile);
    
    sprintf(outfile, "%s\\fun_xode.rcsv", IODE_OUTPUT_DIR);
    sprintf(reffile, "%s\\fun_xode.rcsv.ref", IODE_DATA_DIR);
    rc = EXP_RuleExport(trace, NULL, outfile, varfile, cmtfile, "2000Y1", "2010Y1", "#N/A", ";", EXPORT_RCSV);
    cond = (rc == 0) && U_test_compare_outfile_to_reffile("fun_xode.rcsv", "fun_xode.rcsv.ref");
    S4ASSERT(cond == 1, "EXP_RuleExport(\" \", NULL, \"%s\", \"%s\", \"%s\", \"2000Y1\", \"2010Y1\", \"#N/A\", \";\", EXPORT_RCSV)", outfile, varfile, cmtfile);

    // Export with rules (partial /+ change names)
    sprintf(outfile, "%s\\fun2.tsp", IODE_OUTPUT_DIR);
    sprintf(reffile, "%s\\fun2.ref.tsp", IODE_DATA_DIR);
    sprintf(rulefile, "%s\\rules.txt", IODE_DATA_DIR);
    rc = EXP_RuleExport(trace, rulefile, outfile, varfile, cmtfile, "1995Y1", "2005Y1", "#N/A", ";", EXPORT_TSP);
    cond = (rc == 0) && U_test_compare_outfile_to_reffile("fun2.tsp", "fun2.ref.tsp");
    S4ASSERT(cond == 1, "EXP_RuleExport(\" \", \"%s\", \"%s\", \"%s\", \"%s\", \"1995Y1\", \"2005Y1\", \"#N/A\", \";\", EXPORT_RCSV)", rulefile, outfile, varfile, cmtfile);


    U_test_print_title("Tests IMP VAR: Import Ascii Variables");
    
    sprintf(reffile, "%s\\fun_xode.av.ref", IODE_DATA_DIR);
    sprintf(outfile, "%s\\fun_xode.var", IODE_OUTPUT_DIR);
    rc = IMP_RuleImport(VARIABLES, trace, NULL, outfile, reffile, "2000Y1", "2010Y1", IMPORT_ASCII, 0);
    S4ASSERT(rc == 0, "IMP_RuleImport(VARIABLES, trace, NULL, \"%s\", \"%s\", \"2000Y1\", \"2010Y1\", IMPORT_ASCII, 0)", outfile, reffile);
    
    KV_RWS = KV_WS = K_interpret(VARIABLES, outfile);  
    U_test_lec("ACAF[2002Y1]", "ACAF[2002Y1]", 0, -0.92921251);


    U_test_print_title("Tests IMP CMT: Import Ascii Comments");
   
    sprintf(reffile, "%s\\fun_xode.ac.ref", IODE_DATA_DIR);
    sprintf(outfile, "%s\\fun_xode.cmt", IODE_OUTPUT_DIR);
    rc = IMP_RuleImport(COMMENTS, trace, rulefile, outfile, reffile, NULL, NULL, IMPORT_ASCII, 0);
    S4ASSERT(rc == 0, "IMP_RuleImport(COMMENTS, trace, \"%s\", \"%s\", \"%s\", NULL, NULL, IMPORT_ASCII, 0)", rulefile, outfile, reffile);
    
    if(rc == 0) {
        KC_RWS = KC_WS = K_interpret(COMMENTS, outfile);  
        cond = (KC_WS != NULL) && U_cmp_strs(KCPTR("KK_AF"), "Ondernemingen: ontvangen kapitaaloverdrachten.");
        S4ASSERT(cond == 1, "KK_AF == \"Ondernemingen: ontvangen kapitaaloverdrachten.\"");
    }
    
    U_test_reset_kmsg_msgs(); 
}


void Tests_B_XODE()
{
    char    outfile[256];
    char    reffile[256];
    char    rulefile[256];
    char    cmd[512];
    char    trace[] = " ";
    int     rc;
    
    U_test_print_title("Tests XODE: Import ASCII via report function");
    U_test_suppress_kmsg_msgs();

    sprintf(reffile, "%s\\fun_xode.av.ref", IODE_DATA_DIR);
    sprintf(outfile, "%s\\fun_xode.var", IODE_OUTPUT_DIR);
    sprintf(rulefile, "%s\\rules.txt", IODE_DATA_DIR);
    sprintf(cmd, "ASCII \"%s\" %s %s 2000Y1 2010Y1", rulefile, reffile, outfile);

    rc = B_FileImportVar(cmd);
    S4ASSERT(rc == 0, "B_FileImportVar(\"%s\")", cmd);
    
    KV_RWS = KV_WS = K_interpret(VARIABLES, outfile);  
    U_test_lec("KK_AF[2002Y1]", "KK_AF[2002Y1]", 0, -0.92921251);
    
    U_test_reset_kmsg_msgs(); 

}


void Tests_B_LTOH()
{
    char    cmd[512];
    char    varfile[256];
    SAMPLE  *smpl;
    int     rc;

    U_test_print_title("Tests B_LTOH: convert low periodicity to high periodicity");
    U_test_suppress_kmsg_msgs();

    // Clear the vars and set the sample for the variable WS
    K_clear(KV_WS);
    smpl = PER_atosmpl("2010Q1", "2020Q4");
    KV_sample(KV_WS, smpl);
    SCR_free(smpl);
    
    sprintf(varfile, "%s\\fun.var", IODE_DATA_DIR);
    
    // Linear interpolation / stock
    sprintf(cmd, "L %s ACAF", varfile);
    rc = B_WsLtoHStock(cmd);
    S4ASSERT(rc == 0, "B_WsLtoHStock(\"%s\")", cmd);
    U_test_lec("ACAF[2014Q3]", "ACAF[2014Q3]", 0, -79.729132);
    
    // Linear interpolation / flow
    sprintf(cmd, "L %s ACAG", varfile);
    rc = B_WsLtoHFlow(cmd);
    S4ASSERT(rc == 0, "B_WsLtoHFlow(\"%s\")", cmd);
    U_test_lec("ACAG[2014Q3]", "ACAG[2014Q3]", 0, 8.105075);
        
    // Cubic Splines / stock
    sprintf(cmd, "C %s ACAF", varfile);
    rc = B_WsLtoHStock(cmd);
    S4ASSERT(rc == 0, "B_WsLtoHStock(\"%s\")", cmd);
    U_test_lec("ACAF[2012Q3]", "ACAF[2012Q3]", 0, -52.805666 );
    
    // Cubic splines / flow
    sprintf(cmd, "C %s ACAG", varfile);
    rc = B_WsLtoHFlow(cmd);
    S4ASSERT(rc == 0, "B_WsLtoHFlow(\"%s\")", cmd);
    U_test_lec("ACAG[2012Q3]", "ACAG[2012Q3]", 0, 7.6135768);
    
    // Step / stock
    sprintf(cmd, "S %s ACAF", varfile);
    rc = B_WsLtoHStock(cmd);
    S4ASSERT(rc == 0, "B_WsLtoHStock(\"%s\")", cmd);
    U_test_lec("ACAF[2014Q3]", "ACAF[2014Q3]", 0, -83.34062);
    
    // Step / flow
    sprintf(cmd, "S %s ACAG", varfile);
    rc = B_WsLtoHFlow(cmd);
    S4ASSERT(rc == 0, "B_WsLtoHFlow(\"%s\")", cmd);
    U_test_lec("ACAG[2014Q3]", "ACAG[2014Q3]", 0, 8.1050747);

    U_test_reset_kmsg_msgs(); 
}

void Tests_B_HTOL()
{
    char    cmd[512];
    char    varfile[256];
    SAMPLE  *smpl;
    int     rc;
    //char    current_dir[256];
    
    //getcwd(current_dir, sizeof(current_dir));
    //printf("%s\n", current_dir);

    U_test_print_title("Tests B_HTOL: convert high periodicity to low periodicity");
    U_test_suppress_kmsg_msgs();

    // Clear the vars and set the sample for the variable WS
    K_clear(KV_WS);
    smpl = PER_atosmpl("2000Y1", "2020Y1");
    KV_sample(KV_WS, smpl); 
    SCR_free(smpl);
    
    sprintf(varfile, "%s\\fun_q.var", IODE_DATA_DIR);
    
    // Last Obs
    sprintf(cmd, "%s ACAF", varfile);
    rc = B_WsHtoLLast(cmd);
    S4ASSERT(rc == 0, "B_WsHtoLLast(\"%s\")", cmd);
    U_test_lec("ACAF[2014Y1]", "ACAF[2014Y1]", 0, -83.340625);
    
    // Mean
    sprintf(cmd, "%s ACAG", varfile);
    rc = B_WsHtoLMean(cmd);
    S4ASSERT(rc == 0, "B_WsHtoLMean(\"%s\")", cmd);
    U_test_lec("ACAG[2014Y1]", "ACAG[2014Y1]", 0, 8.1050747);
        
    // Sum
    sprintf(cmd, "%s AOUC", varfile);
    rc = B_WsHtoLSum(cmd);
    S4ASSERT(rc == 0, "B_WsLtoHStock(\"%s\")", cmd);
    U_test_lec("AOUC[2014Y1]", "AOUC[2014Y1]", 0, 1.423714 );
    
    U_test_reset_kmsg_msgs();   
}

void Tests_B_MODEL()
{
    KDB         *kdbv, 
                *kdbe, 
                *kdbs;
    char        *filename = "fun";
    int         rc;
    double      XNATY_2000Y1;
    
    // B_Model*() tests
    // ----------------
    // X int B_ModelSimulate(char *arg)                              $ModelSimulate per_from per_to equation_list
    // X int B_ModelSimulateParms(char* arg)                         $ModelSimulateParms eps relax maxit {Connex | Triang | None } 0 - 4 (starting values) {Yes | no } {yes | No } nbtri
    // X int B_ModelExchange(char* arg)                              $ModelExchange eqname1-varname1,eqname2-varname2,...
    // X int B_ModelCompile(char* arg)                               $ModelCompile  [eqname1, eqname2, ... ]
    // X int B_ModelCalcSCC(char *arg)                               $ModelCalcSCC nbtris prename intername postname [eqs]
    // X int B_ModelSimulateSCC(char *arg)                           $ModelSimulateSCC from to pre inter post
    // int B_ModelSimulateSaveNIters(char *arg)                    $ModelSimulateSaveNiters varname
    // int B_ModelSimulateSaveNorms(char *arg)                     $ModelSimulateSaveNorms varname

    U_test_print_title("Tests B_Model*(): simulation parameters and model simulation");
    U_test_suppress_kmsg_msgs();
    
    
    // Loads 3 WS and check ok
    U_test_load_fun_esv(filename);

    // Check
    kdbv = KV_WS;
    S4ASSERT(kdbv != NULL, "K_interpret(VARIABLES, \"%s\")", filename);
    kdbs = KS_WS;
    S4ASSERT(kdbs != NULL, "K_interpret(SCALARS, \"%s\")", filename);
    kdbe = KE_WS;
    S4ASSERT(kdbe != NULL, "K_interpret(EQUATIONS, \"%s\")", filename);
    
    // B_ModelSimulateParms()
    KSIM_START = VAR_INIT_TM1;
    KSIM_EPS = 0.00001;
    KSIM_MAXIT = 1000;
    KSIM_RELAX = 1.0;
    KSIM_SORT = 0;
    KSIM_PASSES = 3; 
    KSIM_DEBUG = 1;
    rc = B_ModelSimulateParms("0.0001 0.7 100 Triang 0 no 5 no");
    S4ASSERT(rc == 0, "B_ModelSimulateParms(\"0.0001 0.7 100 Both 0 no 5 no\") == 0");
    S4ASSERT(KSIM_EPS == 0.0001, "B_ModelSimulateParms(\"0.0001 0.7 100 Both 0 no 5 no\") => KSIM_EPS == 0.0001");
    S4ASSERT(KSIM_MAXIT == 100, "B_ModelSimulateParms(\"0.0001 0.7 100 Both 0 no 5 no\") => KSIM_MAXIT == 100");
    S4ASSERT(KSIM_RELAX == 0.7, "B_ModelSimulateParms(\"0.0001 0.7 100 Both 0 no 5 no\") => KSIM_RELAX == 0.7");
    S4ASSERT(KSIM_DEBUG == 0, "B_ModelSimulateParms(\"0.0001 0.7 100 Both 0 no 5 no\") => KSIM_DEBUG == 0");
    S4ASSERT(KSIM_PASSES == 5, "B_ModelSimulateParms(\"0.0001 0.7 100 Both 0 no 5 no\") => KSIM_PASSES == 5");


    // B_ModelSimulate()
    rc = B_ModelSimulate("2000Y1 2002Y1");
    S4ASSERT(rc == 0, "B_ModelSimulate(\"2000Y1 2002Y1\") == 0");
    // TODO: check result of one ENDO
    S4ASSERT(U_test_eq(KV_get_at_aper("ACAF", "2002Y1"), -1.27462), "ACAF[2002Y1] = -1.27474");
    
    // B_ModelExchange()
    
    // Reloads 3 WS 
    U_test_load_fun_esv(filename);

    // Set values of endo UY
    KV_set_at_aper("UY", "2000Y1", 650.0);
    KV_set_at_aper("UY", "2001Y1", 670.0);
    KV_set_at_aper("UY", "2002Y1", 680.0);

    // Exchange
    rc = B_ModelExchange("UY-XNATY");
    S4ASSERT(rc == 0, "B_ModelExchange(\"UY-XNATY\") == 0");
    
    // Simulate
    rc = B_ModelSimulate("2000Y1 2002Y1");
    S4ASSERT(rc == 0, "B_ModelSimulate(\"2000Y1 2002Y1\") == 0");

    // Check some results
    S4ASSERT(KV_get_at_aper("UY", "2000Y1") == 650.0, "Exchange UY-XNATY: UY[2000Y1] == 650.0 unchanged");
    XNATY_2000Y1 = KV_get_at_aper("XNATY", "2000Y1"); 
    //printf("XNATY_2000Y1 = %lg\n", XNATY_2000Y1);
    S4ASSERT(U_test_eq(KV_get_at_aper("XNATY", "2000Y1"), 0.800673), "Exchange UY-XNATY: XNATY[2000Y1] == 0.800673");
    
    // B_ModelCompile(char* arg)
    rc = B_ModelCompile("");
    S4ASSERT(rc == 0, "B_ModelCompile(\"\") == 0");

    // B_ModelCalcSCC(char *arg) $ModelCalcSCC nbtris prename intername postname [eqs]
    rc = B_ModelCalcSCC("5 _PRE2 _INTER2 _POST2");
    S4ASSERT(rc == 0, "B_ModelCalcSCC(\"5 _PRE2 _INTER2 _POST2\") == 0");
    rc = strcmp(KLPTR("_PRE2"), "BRUGP;DTH1C;EX;ITCEE;ITCR;ITGR;ITI5R;ITIFR;ITIGR;ITMQR;NATY;POIL;PW3;PWMAB;PWMS;PWXAB;PWXS;PXE;QAH;QWXAB;QWXS;QWXSS;SBGX;TFPFHP_;TWG;TWGP;ZZF_;DTH1;PME;PMS;PMT");
    S4ASSERT(rc == 0, "_PRE2 == BRUGP;DTH1C;EX;ITCEE;ITCR;ITGR;ITI5R;ITIFR;ITIGR;ITMQR;NATY;POIL;PW3;PWMAB;PWMS;PWXAB;PWXS;PXE;QAH;QWXAB;QWXS;QWXSS;SBGX;TFPFHP_;TWG;TWGP;ZZF_;DTH1;PME;PMS;PMT");

    // int B_ModelSimulateSCC(char *arg)                           $ModelSimulateSCC from to pre inter post
    //  1. Annuler Exchange
    rc = B_ModelExchange("");
    S4ASSERT(rc == 0, "B_ModelExchange(\"\") == 0");

    //  2. ReLoads 3 WS to reset EXO XNATY to its original value
    U_test_load_fun_esv(filename);
    
    //  3. Simulate & compare
    rc = B_ModelSimulateSCC("2000Y1 2002Y1 _PRE2 _INTER2 _POST2");
    S4ASSERT(rc == 0, "B_ModelSimulateSCC(\"2000Y1 2002Y1 _PRE2 _INTER2 _POST2\") == 0");
    S4ASSERT(U_test_eq(KV_get_at_aper("ACAF", "2002Y1"), -1.274623), "ACAF[2002Y1] = -1.274623");
        
    // B_ModelSimulateSaveNIters(char *arg)                    $ModelSimulateSaveNiters varname
    
   

    U_test_reset_kmsg_msgs();      
}


// int B_WsLoad(char* arg, int type)                 $WsLoad<type> filename
int U_test_B_WsLoad(char*filename, int type, int expected_nb_objects)
{
    char    fullfilename[256];
    int     rc, cond;

    sprintf(fullfilename,  "%s\\%s", IODE_DATA_DIR, filename);
    rc = B_WsLoad(fullfilename, type);
    cond = (rc == 0) && (K_WS[type]->k_nb == expected_nb_objects); 
    S4ASSERT(cond != 0, "B_WsLoad(\"%s\", %d) == 0 -- nb objects=%d", fullfilename, type, expected_nb_objects);
    return(cond);
}


int U_test_B_WsSave(char* source_file, char* out_file, int type, int expected_nb_objects)
{
    char    fullfilename[256];
    int     rc, cond;

    _unlink(out_file);
    sprintf(fullfilename,  "%s\\%s", IODE_DATA_DIR, source_file);
    rc = B_WsLoad(fullfilename, type);
    S4ASSERT(rc == 0, "B_WsLoad(\"%s\", %d) == 0", fullfilename, type);
    rc = B_WsSave(out_file, type);
    S4ASSERT(rc == 0, "B_WsSave(\"%s\", %d) == 0", out_file, type);
    B_WsClear("", type);
    rc = B_WsLoad(out_file, type);
    cond = (rc == 0) && (K_WS[type]->k_nb == expected_nb_objects); 
    S4ASSERT(cond != 0, "B_WsLoad(\"%s\", %d) == 0 -- nb objects=%d", out_file, type, expected_nb_objects);
    return(cond);
}


int U_test_B_WsSaveCmp(char* source_file, char* out_file, int type, int expected_nb_objects)
{
    char    fullfilename[256];
    int     rc, cond;

    _unlink(out_file);
    sprintf(fullfilename,  "%s\\%s", IODE_DATA_DIR, source_file);
    rc = B_WsLoad(fullfilename, type);
    S4ASSERT(rc == 0, "B_WsLoad(\"%s\", %d) == 0", fullfilename, type);
    rc = B_WsSaveCmp(out_file, type);
    S4ASSERT(rc == 0, "B_WsSaveCmp(\"%s\", %d) == 0", out_file, type);
    B_WsClear("", type);
    rc = B_WsLoad(out_file, type);
    cond = (rc == 0) && (K_WS[type]->k_nb == expected_nb_objects); 
    S4ASSERT(cond != 0, "B_WsLoad(\"%s\", %d) == 0 -- nb objects=%d", out_file, type, expected_nb_objects);
    return(cond);
}


int U_test_B_WsExport(char* source_file, char* out_file, int type)
{
    char    full_source_file[256];
    char    full_out_file[256];
    char    ref_out_file[256];
    int     rc;
    
    sprintf(full_source_file,  "%s\\%s", IODE_DATA_DIR, source_file);
    sprintf(full_out_file,  "%s\\%s", IODE_OUTPUT_DIR, out_file);
    sprintf(ref_out_file,  "%s\\%s", IODE_DATA_DIR, out_file);
    
    rc = B_WsLoad(full_source_file, type);
    S4ASSERT(rc == 0, "B_WsLoad(\"%s\", %d) == 0", full_source_file, type);
    
    _unlink(full_out_file);
    rc = B_WsExport(full_out_file, type);
    S4ASSERT(rc == 0, "B_WsExport(\"%s\", %d) == 0", full_out_file, type);
    rc = U_diff_files(ref_out_file, full_out_file);
    S4ASSERT(rc != 0, "B_WsExport(\"%s\", %d) => %s == %s", full_out_file, type, ref_out_file, full_out_file);
    return(rc);
}


int U_test_B_WsImport(char* source_file, int type, int expected_nb_objects)
{
    char    fullfilename[256];
    int     rc, cond;

    B_WsClear("", type);
    sprintf(fullfilename, "%s\\%s", IODE_DATA_DIR, source_file);
    rc = B_WsImport(fullfilename, type);
    cond = (rc == 0) && (K_WS[type]->k_nb == expected_nb_objects); 
    S4ASSERT(cond != 0, "B_WsImport(\"%s\", %d) => -- nb objects = %d", fullfilename, type, expected_nb_objects);
    return(rc);
}

int U_test_B_WsClear(int type)
{
    int     rc, cond;

    rc = B_WsClear("", type);
    cond = (rc == 0) && (K_WS[type]->k_nb == 0); 
    S4ASSERT(cond != 0, "B_WsClear(\"%d\") == 0", type);
    return(cond);
}

int U_test_B_WsDescr(char* descr, int type)
{
    int     rc, cond;

    rc = B_WsDescr(descr, type);
    cond = (rc == 0) && (strcmp(KDESC(K_WS[type]), descr) == 0); 
    S4ASSERT(cond != 0, "B_WsDescr(\"%s\", %d) == 0", descr, type);
    return(cond);
}

int U_test_B_WsName(char* name, int type)
{
    int     rc, cond;
    char    *nameptr;

    rc = B_WsName(name, type);
    nameptr = K_get_kdb_nameptr(K_WS[type]);
    //cond = (rc == 0) && (strcmp(KNAMEPTR(K_WS[type]), name) == 0); 
    cond = (rc == 0) && (strcmp(nameptr, name) == 0); 
    S4ASSERT(cond != 0, "B_WsName(\"%s\", %d) == 0", name, type);
    return(cond);
}

int U_test_B_WsCopyVar()
{
    char    arg[256];
    int     rc, cond, nb, pos;
    double *ACAF, ACAF91, ACAF92, ACAG90, ACAG92;

    // 1. Copy full VAR file (Att: * required)
    B_WsClearAll("");
    sprintf(arg,  "%s\\fun.var *", IODE_DATA_DIR);
    rc = B_WsCopy(arg, VARIABLES);    
    ACAF92 = U_test_calc_lec("ACAF[1992Y1]", 0);
    ACAG92 = U_test_calc_lec("ACAG[1992Y1]", 0);
    cond =  (rc == 0) && 
            (U_test_eq(ACAF92, 30.159000) != 0) &&
            (U_test_eq(ACAG92, -40.286) != 0);
    S4ASSERT(cond != 0, "B_WsCopy(\"%s\", VARIABLES) == 0", arg);

    // 2. Copy partial WS (92-93) on an existing one
    // 2.1 Set Sample + create ACAF
    B_WsClearAll("");
    B_WsSample("1990Y1 2000Y1");
    // Create ACAF = 0 1 2...
    nb = 11;
    ACAF = L_cc_link_exec("t", KV_WS, KS_WS);
    pos = K_add(KV_WS, "ACAF", ACAF, &nb);
    
    // 2.2 Copy ACAF and ACAG on 1992 & 1993 (does not replace 1991 for example)
    sprintf(arg,  "%s\\fun.var 1992Y1 1993Y1 ACAF ACAG", IODE_DATA_DIR);
    rc = B_WsCopy(arg, VARIABLES);    
        
    // 2.3 Tests    
    ACAF91 = U_test_calc_lec("ACAF[1991Y1]", 0);
    ACAF92 = U_test_calc_lec("ACAF[1992Y1]", 0);
    ACAG90 = U_test_calc_lec("ACAG[1990Y1]", 0);
    ACAG92 = U_test_calc_lec("ACAG[1992Y1]", 0);
    cond =  (rc == 0) && 
            (U_test_eq(ACAF91, 1.0) != 0) &&
            (U_test_eq(ACAF92, 30.159000) != 0) &&
            (U_test_eq(ACAG90, IODE_NAN) != 0) && 
            (U_test_eq(ACAG92, -40.286) != 0);
            
    S4ASSERT(cond != 0, "B_WsCopy(\"%s\", VARIABLES) == 0", arg);

    // 3. Copy partial WS on an existing one w/o specifying the sample
    // 3.1 Set Sample 
    B_WsClearAll("");
    B_WsSample("1990Y1 2000Y1");
    
    // Copy ACAF and ACAG (does not specify a sample)
    sprintf(arg,  "%s\\fun.var ACAF ACAG", IODE_DATA_DIR);
    rc = B_WsCopy(arg, VARIABLES);    
    ACAF92 = U_test_calc_lec("ACAF[1992Y1]", 0);
    ACAG92 = U_test_calc_lec("ACAG[1992Y1]", 0);
        
    cond =  (rc == 0) && 
            (U_test_eq(ACAF92, 30.159000) != 0) &&
            (U_test_eq(ACAG92, -40.286) != 0);
            
    S4ASSERT(cond != 0, "B_WsCopy(\"%s\", VARIABLES) == 0", arg);
    
    return(cond);
}

int U_test_B_WsCopy(char* filename, int type, int expected_nb_objects)
{
    char    arg[256];
    int     rc, cond;

    // 1. Copy entire file (Att: * required)
    B_WsClear("", type);
    sprintf(arg,  "%s\\%s *", IODE_DATA_DIR, filename);
    rc = B_WsCopy(arg, type);
    cond = (rc == 0) && (K_WS[type]->k_nb == expected_nb_objects); 
    S4ASSERT(cond != 0, "B_WsCopy(\"%s\", %d) == 0 -- nb objects=%d", arg, type, expected_nb_objects);
    
    return(cond);
}

int U_test_B_WsMergeVar()
{    
    int         rc, cond, nb, pos;
    char        arg[256];
    double   *ACAF, ACAF92, ACAF00, ACAF16, ACAG92, ACAG00;

    // 1. Merge into an empty WS
    B_WsClearAll("");
    sprintf(arg,  "%s\\fun.var", IODE_DATA_DIR);
    rc = B_WsMerge(arg, VARIABLES);
    ACAF92 = U_test_calc_lec("ACAF[1992Y1]", 0);
    ACAG92 = U_test_calc_lec("ACAG[1992Y1]", 0);
    cond =  (rc == 0) && 
            (U_test_eq(ACAF92, 30.159000) != 0) &&
            (U_test_eq(ACAG92, -40.286) != 0);
    S4ASSERT(cond != 0, "B_WsMerge(\"%s\", VARIABLES) == 0", arg);
    
    // 2. Merge into an existing WS inb a different SAMPLE
    B_WsClearAll("");
    B_WsSample("2000Y1 2020Y1");
    // Create ACAF = 0 1 2...
    nb = 21;
    ACAF = L_cc_link_exec("t", KV_WS, KS_WS);
    pos = K_add(KV_WS, "ACAF", ACAF, &nb);
    // Merge
    sprintf(arg,  "%s\\fun.var", IODE_DATA_DIR);
    rc = B_WsMerge(arg, VARIABLES);
    //Check 
    ACAF00 = U_test_calc_lec("ACAF[2000Y1]", 0);
    ACAF16 = U_test_calc_lec("ACAF[2016Y1]", 0);
    ACAG00 = U_test_calc_lec("ACAG[2000Y1]", 0);
    cond =  (rc == 0) && 
            (U_test_eq(ACAF00, 10.046611) != 0) &&
            (U_test_eq(ACAF16, 16.0) != 0) &&
            (U_test_eq(ACAG00, -41.534787) != 0);
    S4ASSERT(cond != 0, "B_WsMerge(\"%s\", VARIABLES) == 0", arg);  
    return(cond);
}


int U_test_B_WsMerge(char* filename, int type, int expected_nb_objects)
{
    char    arg[256];
    int     rc, cond;

    // 1. Copy entire file (Att: * required)
    B_WsClear("", type);
    sprintf(arg,  "%s\\%s *", IODE_DATA_DIR, filename);
    rc = B_WsMerge(arg, type);
    cond = (rc == 0) && (K_WS[type]->k_nb == expected_nb_objects); 
    S4ASSERT(cond != 0, "B_WsMerge(\"%s\", %d) == 0 -- nb objects=%d", arg, type, expected_nb_objects);
    
    return(cond);
}

int U_test_B_WsExtrapolate(int method, double expected_value)
{
    double   *ACAF, ACAF2002;
    char        arg[512];
    int         pos, rc, nb,cond;
    
    B_WsClearAll("");
    B_WsSample("1995Y1 2020Y1");
    
    // Create ACAF = 0 1 IODE_NAN...
    nb = 11;
    ACAF = L_cc_link_exec("t", KV_WS, KS_WS);
    ACAF[7] = IODE_NAN;
    pos = K_add(KV_WS, "ACAF", ACAF, &nb);

    // $WsExtrapolate [method] from to [variable list]
    sprintf(arg, "%d 2000Y1 2010Y1 ACAF", method);
    rc = B_WsExtrapolate(arg);
    ACAF2002 = U_test_calc_lec("ACAF[2002Y1]", 0);
    cond =  (rc == 0) && (U_test_eq(ACAF2002, expected_value) != 0);
    S4ASSERT(cond != 0, "B_WsExtrapolate(\"%s\") == 0; ACAF[2002Y1] == %lf", arg, ACAF2002);
    return(cond);    
}


int U_test_create_var(char*name, char*lec)
{
    double  *A;
    int     nb;
    
    if(KV_WS == NULL) return(-1);
    nb = KSMPL(KV_WS)->s_nb;
    A = L_cc_link_exec(lec, KV_WS, KS_WS);
    K_add(KV_WS, name, A, &nb);
    SCR_free(A);
    return(0);
}

int U_test_B_WsAggregate()
{
    double   A_2000, B_2000, AC_2000;
    char        arg[512];
    int         rc, cond;
    
    B_WsClearAll("");
    B_WsSample("1995Y1 2020Y1");
       
    // Create VARS A1, A2, B1, B2
    U_test_create_var("A1", "t");
    U_test_create_var("A2", "t*2");
    U_test_create_var("B1", "t**2");
    U_test_create_var("B2", "t**3");

    //  $WsAggrChar [char]
    rc = B_WsAggrChar("");
    S4ASSERT(rc == 0, "B_WsAggrChar(\"\") == 0");
 
    // $WsAggrSum  pattern 
    strcpy(arg, "(?)[?]"); 
    rc = B_WsAggrSum(arg);
    A_2000 = U_test_calc_lec("A[2000Y1]", 0);
    B_2000 = U_test_calc_lec("B[2000Y1]", 0);
    cond = (rc  == 0) && (A_2000 == 5 + 5*2) && (B_2000 == 5 * 5 + 5 * 5 * 5);
    S4ASSERT(cond != 0, "B_WsAggrSum(\"%s\") == 0, A[2000Y1] = %lf", arg, A_2000);
    
    // $WsAggrProd  pattern 
    strcpy(arg, "(?)[?]"); 
    rc = B_WsAggrProd(arg);
    A_2000 = U_test_calc_lec("A[2000Y1]", 0);
    B_2000 = U_test_calc_lec("B[2000Y1]", 0);
    cond = (rc  == 0) && (A_2000 == (5) * (5*2)) && (B_2000 == (5 * 5) * (5 * 5 * 5));
    S4ASSERT(cond != 0, "B_WsAggrProd(\"%s\") == 0, A[2000Y1] = %lf", arg, A_2000);

    // $WsAggrMean  pattern 
    strcpy(arg, "(?)[?]"); 
    rc = B_WsAggrMean(arg);
    A_2000 = U_test_calc_lec("A[2000Y1]", 0);
    B_2000 = U_test_calc_lec("B[2000Y1]", 0);
    cond = (rc  == 0) && (U_test_eq(A_2000, 7.5) != 0) && (U_test_eq(B_2000, 75.0) != 0);
    S4ASSERT(cond != 0, "B_WsAggrMean(\"%s\") == 0, A[2000Y1] = %lf", arg, A_2000);
    
    // $WsAggrSum  pattern filename
    sprintf(arg,  "(AC)[??] %s\\fun.var", IODE_DATA_DIR);
    rc = B_WsAggrSum(arg);
    AC_2000 = U_test_calc_lec("AC[2000Y1]", 0);
    cond = (rc  == 0) && (U_test_eq(AC_2000, -31.488176) != 0);
    S4ASSERT(cond != 0, "B_WsAggrSum(\"%s\") == 0, AC[2000Y1] = %lf", arg, AC_2000);
    
    return(cond);    
}


int U_test_B_StatUnitRoot(int drift, int trend, int order, char* varname, double expected_df)
{
    int     rc, cond;
    double  df;
    char    arg[256], scalar[30];

    // Load needed data
    //U_test_B_WsLoad("fun", SCALARS, 161);
    //U_test_B_WsLoad("fun", VARIABLES, 394);
    
    // Dickey-Fuller test (E_UnitRoot)
    // int B_StatUnitRoot(char* arg)                     $StatUnitRoot drift trend order expression
    sprintf(arg, "%d %d %d %s", drift, trend, order, varname);
    rc = B_StatUnitRoot(arg);
    sprintf(scalar, "df_%s", varname);
    df = U_test_calc_lec(scalar, 0);
    cond = (rc == 0) && (U_test_eq(df, expected_df) != 0);
    S4ASSERT(cond != 0, "B_StatUnitRoot(\"%s\") == %lf (expected %lf)", arg, df, expected_df);
    return(cond);
}

int U_test_B_Csv()
{
    int     rc, cond;
    char    arg[512];
  
    // int B_CsvNbDec(char *nbdec)                       $CsvNbDec nn
    // int B_CsvSep(char *sep)                           $CsvSep char
    // int B_CsvNaN(char *nan)                           $CsvNaN text
    // int B_CsvAxes(char *var)                          $CsvAxes AxisName
    // int B_CsvDec(char *dec)                           $CsvDec char
    // int B_CsvSave(char* arg, int type)                $CsvSave<type> file name1 name2 ...
    rc = B_CsvNbDec("7");
    cond = (rc == 0) && (KV_CSV_NBDEC == 7);
    S4ASSERT(cond != 0, "B_CsvNbDec(\"7\")");

    rc = B_CsvSep(";");
    cond = (rc == 0) && (KV_CSV_SEP[0] == ';');
    S4ASSERT(cond != 0, "B_CsvSep(\";\") = \"%s\"", KV_CSV_SEP);
    
    rc = B_CsvNaN("--");
    cond = (rc == 0) && (strcmp(KV_CSV_NAN, "--") == 0);
    S4ASSERT(cond != 0, "B_CsvNaN(\"--\") == \"%s\"", KV_CSV_NAN);

    rc = B_CsvAxes("Name");
    cond = (rc == 0) && (strcmp(KV_CSV_AXES, "Name") == 0);
    S4ASSERT(cond != 0, "B_CsvAxes(\"Name\") == \"%s\"", KV_CSV_AXES);

    rc = B_CsvDec(".");
    cond = (rc == 0) && (strcmp(KV_CSV_DEC, ".") == 0);
    S4ASSERT(cond != 0, "B_CsvDec(\".\") == \"%s\"", KV_CSV_DEC);

    U_test_B_WsLoad("fun", VARIABLES, 394);
    sprintf(arg, "%s\\funcsv.csv A* *G", IODE_OUTPUT_DIR);
    rc = B_CsvSave(arg, VARIABLES);
    
    cond = (rc == 0) && U_test_compare_outfile_to_reffile("funcsv.csv", "funcsv.csv");
    S4ASSERT(cond != 0, "B_CsvSave(\"funcsv.csv\")");
    
    return(cond);
}


void Tests_B_WS()
{
    int     rc, cond;
    SAMPLE  *smpl;

    // List of tested report functions:
    //   - int B_WsLoad(char* arg, int type)                 $WsLoad<type> filename
    //   - int B_WsSave(char* arg, int type)                 $WsSave<type> filename
    //   - int B_WsSaveCmp(char* arg, int type)              $WsSaveCmp<type> filename
    //   - int B_WsExport(char* arg, int type)               $WsExport<type> filename
    //   - int B_WsImport(char* arg, int type)               $WsImport<type> filename
    //   - int B_WsSample(char* arg)                         $WsSample period_from period_to
    //   - int B_WsClear(char* arg, int type)                $WsClear<type> 
    //   - int B_WsClearAll(char* arg)                       $WsClearAll
    //   - int B_WsDescr(char* arg, int type)                $WsDescr<type> free text
    //   - int B_WsName(char* arg, int type)                 Sets the WS name. Obsolete as report function.
    //   - int B_WsCopy(char* arg, int type)                 $WsCopy<type> fichier;fichier;.. obj1 obj2... or $WsCopyVar file;file;.. [from to] obj1 obj2...
    //   - int B_WsMerge(char* arg, int type)                $WsMerge<type> filename
    //   - int B_WsExtrapolate(char* arg)                    $WsExtrapolate [method] from to [variable list]
    //   - int B_WsAggrChar(char* arg)                       $WsAggrChar char
    //   - int B_WsAggrSum(char* arg)                        $WsAggrSum pattern filename
    //   - int B_WsAggrProd(char* arg)                       $WsAggrProd pattern filename
    //   - int B_WsAggrMean(char* arg)                       $WsAggrMean pattern filename
    //   - int B_StatUnitRoot(char* arg)                     $StatUnitRoot drift trend order expression
    //   - int B_CsvSave(char* arg, int type)                $CsvSave<type> file name1 name2 ...
    //   - int B_CsvNbDec(char *nbdec)                       $CsvNbDec nn
    //   - int B_CsvSep(char *sep)                           $CsvSep char
    //   - int B_CsvNaN(char *nan)                           $CsvNaN text
    //   - int B_CsvAxes(char *var)                          $CsvAxes AxisName
    //   - int B_CsvDec(char *dec)                           $CsvDec char

    U_test_print_title("Tests B_Ws*(): report functions $Ws*");
    U_test_suppress_kmsg_msgs();

    // int B_WsLoad(char* arg, int type)                 $WsLoad<type> filename
    U_test_print_title("B_WsLoad()");
    U_test_B_WsLoad("fun", COMMENTS, 317);
    U_test_B_WsLoad("fun", EQUATIONS, 274);
    U_test_B_WsLoad("fun", IDENTITIES, 48);
    U_test_B_WsLoad("fun", LISTS, 17);
    U_test_B_WsLoad("fun", SCALARS, 161);
    U_test_B_WsLoad("fun", TABLES, 46);
    U_test_B_WsLoad("fun", VARIABLES, 394);
    
    // int B_WsSave(char* arg, int type)                 $WsSave<type> filename
    U_test_print_title("B_WsSave()");
    U_test_B_WsSave("fun", "fun2", COMMENTS, 317);
    U_test_B_WsSave("fun", "fun2", EQUATIONS, 274);
    U_test_B_WsSave("fun", "fun2", IDENTITIES, 48);
    U_test_B_WsSave("fun", "fun2", LISTS, 17);
    U_test_B_WsSave("fun", "fun2", SCALARS, 161);
    U_test_B_WsSave("fun", "fun2", TABLES, 46);
    U_test_B_WsSave("fun", "fun2", VARIABLES, 394);
           
    // int B_WsSaveCmp(char* arg, int type)              $WsSaveCmp<type> filename
    U_test_print_title("B_WsSaveCmp()");
    U_test_B_WsSaveCmp("fun", "fun2cmp", COMMENTS, 317);
    U_test_B_WsSaveCmp("fun", "fun2cmp", EQUATIONS, 274);
    U_test_B_WsSaveCmp("fun", "fun2cmp", IDENTITIES, 48);
    U_test_B_WsSaveCmp("fun", "fun2cmp", LISTS, 17);
    U_test_B_WsSaveCmp("fun", "fun2cmp", SCALARS, 161);
    U_test_B_WsSaveCmp("fun", "fun2cmp", TABLES, 46);
    U_test_B_WsSaveCmp("fun", "fun2cmp", VARIABLES, 394);
    
    // int B_WsExport(char* arg, int type)               $WsExport<type> filename
    U_test_print_title("B_WsExport()");
    U_test_B_WsExport("fun.cmt", "fun2.ac", COMMENTS); 
    U_test_B_WsExport("fun.eqs", "fun2.ae", EQUATIONS); 
    U_test_B_WsExport("fun.idt", "fun2.ai", IDENTITIES);
    U_test_B_WsExport("fun.lst", "fun2.al", LISTS);
    U_test_B_WsExport("fun.scl", "fun2.as", SCALARS); 
    U_test_B_WsExport("fun.tbl", "fun2.at", TABLES);
    U_test_B_WsExport("fun.var", "fun2.av", VARIABLES);

    // int B_WsClear(char* arg, int type)                $WsClear<type> 
    U_test_print_title("B_WsClear()");
    U_test_B_WsClear(COMMENTS);
    U_test_B_WsClear(EQUATIONS);
    U_test_B_WsClear(IDENTITIES);
    U_test_B_WsClear(LISTS);
    U_test_B_WsClear(SCALARS);
    U_test_B_WsClear(TABLES);
    U_test_B_WsClear(VARIABLES);
     
    // int B_WsImport(char* arg, int type)               $WsImport<type> filename
    U_test_print_title("B_WsImport()");
    U_test_B_WsImport("fun2.ac", COMMENTS, 317);
    U_test_B_WsImport("fun2.ae", EQUATIONS, 273);  // scalar gamma in EQ W is illegal since the implementation th gamma function in LEC
    U_test_B_WsImport("fun2.ai", IDENTITIES, 47);   // Idem in IDT NAWRU
    U_test_B_WsImport("fun2.al", LISTS, 17);
    U_test_B_WsImport("fun2.as", SCALARS, 161);
    U_test_B_WsImport("fun2.at", TABLES, 46);
    U_test_B_WsImport("fun2.av", VARIABLES, 394);
    // TODO : correct fun.eqs (W) and fun.idt (NAWRU)
    
    // int B_WsSample(char* arg)                         $WsSample period_from period_to
    U_test_print_title("B_WsSample()");
    rc = B_WsSample("1965Y1 2020Y1");
    smpl = PER_atosmpl("1965Y1", "2020Y1");
    cond = rc == 0 && (memcmp((char*)KSMPL(KV_WS), (char*)smpl, sizeof(SAMPLE)) == 0);
    S4ASSERT(cond != 0, "B_WsSample(\"1965Y1 2020Y1\")");
    SCR_free(smpl);
    
    // int B_WsClearAll(char* arg)                       $WsClearAll
    U_test_print_title("B_WsClearAll()");
    rc = B_WsClearAll("");
    cond = (rc == 0) && 
           (KNB(KC_WS) == 0) &&
           (KNB(KE_WS) == 0) &&
           (KNB(KI_WS) == 0) &&
           (KNB(KL_WS) == 0) &&
           (KNB(KS_WS) == 0) &&
           (KNB(KT_WS) == 0) &&
           (KNB(KV_WS) == 0);
                            
    S4ASSERT(cond != 0, "B_WsClearAll(\"\")");
    
    // int B_WsDescr(char* arg, int type)                $WsDescr<type> free text
    U_test_print_title("B_WsDescr()");
    U_test_B_WsDescr("Ws content description", COMMENTS);
    U_test_B_WsDescr("Ws content description", EQUATIONS);
    U_test_B_WsDescr("Ws content description", IDENTITIES);
    U_test_B_WsDescr("Ws content description", LISTS);
    U_test_B_WsDescr("Ws content description", SCALARS);
    U_test_B_WsDescr("Ws content description", TABLES);
    U_test_B_WsDescr("Ws content description", VARIABLES);
    
    // int B_WsName(char* arg, int type)                 Sets the WS name. Obsolete as report function.
    // Test skipped: alignment pb with Google Tests (k_nameptr aligned on 60 bytes, => not 8 bytes) 
    // Should be reviewed for C++ w/o /Zp1
    U_test_print_title("B_WsName()");
    U_test_B_WsName("funtest", COMMENTS);
    U_test_B_WsName("funtest", EQUATIONS);
    U_test_B_WsName("funtest", IDENTITIES);
    U_test_B_WsName("funtest", LISTS);
    U_test_B_WsName("funtest", SCALARS);
    U_test_B_WsName("funtest", TABLES);
    U_test_B_WsName("funtest", VARIABLES);
    
    // int B_WsCopy(char* arg, int type)                 $WsCopy<type> fichier;fichier;.. obj1 obj2... or $WsCopyVar file;file;.. [from to] obj1 obj2...
    U_test_print_title("B_WsCopy() - VARIABLES");
    U_test_B_WsCopyVar();

    U_test_print_title("B_WsCopy() - other objects");
    U_test_B_WsCopy("fun", COMMENTS, 317);
    U_test_B_WsCopy("fun", EQUATIONS, 274);  // scalar gamma in EQ W is illegal since the implementation th gamma function in LEC
    U_test_B_WsCopy("fun", IDENTITIES, 48);   // Idem in IDT NAWRU
    U_test_B_WsCopy("fun", LISTS, 17);
    U_test_B_WsCopy("fun", SCALARS, 161);
    U_test_B_WsCopy("fun", TABLES, 46);
    
    
    // int B_WsMerge(char* arg, int type)                $WsMerge<type> filename
    U_test_print_title("B_WsMerge() - VARIABLES");
    U_test_B_WsMergeVar();
    
    U_test_print_title("B_WsMerge() - other objects");
    U_test_B_WsMerge("fun", COMMENTS, 317);
    U_test_B_WsMerge("fun", EQUATIONS, 274);  
    U_test_B_WsMerge("fun", IDENTITIES, 48);   
    U_test_B_WsMerge("fun", SCALARS, 161);
    U_test_B_WsMerge("fun", TABLES, 46);
    
    // int B_WsExtrapolate(char* arg)                    $WsExtrapolate [method] from to [variable list]
    U_test_print_title("B_WsExtrapolate");
    U_test_B_WsExtrapolate(0, 6.0);
    U_test_B_WsExtrapolate(1, 4.0);
    U_test_B_WsExtrapolate(2, 7.0);
    U_test_B_WsExtrapolate(3, 7.0);
    U_test_B_WsExtrapolate(4, IODE_NAN);
    U_test_B_WsExtrapolate(5, 6.0);
    U_test_B_WsExtrapolate(6, 7.0);
    
    // int B_WsAggrChar(char* arg)                       $WsAggrChar char
    // int B_WsAggrSum(char* arg)                        $WsAggrSum pattern filename
    // int B_WsAggrProd(char* arg)                       $WsAggrProd pattern filename
    // int B_WsAggrMean(char* arg)                       $WsAggrMean pattern filename 
    U_test_print_title("B_WsAggregate");
    U_test_B_WsAggregate();
       
    // int B_StatUnitRoot(char* arg)                     $StatUnitRoot drift trend order expression
    U_test_print_title("B_StatUnitRoot");
    U_test_B_WsLoad("fun", SCALARS, 161);
    U_test_B_WsLoad("fun", VARIABLES, 394);

    U_test_B_StatUnitRoot(0, 0, 0, "ACAF", 0.958325);
    U_test_B_StatUnitRoot(1, 0, 0, "ACAF", 1.117498);
    U_test_B_StatUnitRoot(1, 1, 0, "ACAF", -0.798686);
    U_test_B_StatUnitRoot(0, 0, 2, "ACAF", 1.419631);
    
    // int B_CsvNbDec(char *nbdec)                       $CsvNbDec nn
    // int B_CsvSep(char *sep)                           $CsvSep char
    // int B_CsvNaN(char *nan)                           $CsvNaN text
    // int B_CsvAxes(char *var)                          $CsvAxes AxisName
    // int B_CsvDec(char *dec)                           $CsvDec char
    // int B_CsvSave(char* arg, int type)                $CsvSave<type> file name1 name2 ...
    U_test_print_title("B_Csv*");
    U_test_B_Csv();
       
    U_test_reset_kmsg_msgs();      
    
}

void Tests_B_REP_LINE()
{
    int     rc, cond;
    char    fullfilename[256], cmd[1024];

    
    U_test_print_title("Tests B_ReportLine()");
    U_test_suppress_kmsg_msgs();

    // Simple test of a call to B_ReportLine(). More elaborate commands are tested with B_ReportExec()
    sprintf(fullfilename,  "%s\\%s", IODE_DATA_DIR, "fun.var");
    sprintf(cmd, "\n"
                 "$WsClearVar\n"
                 "$WsLoadVar %s\n", fullfilename);
                 
    rc = B_ReportLine(cmd);
    cond = (rc == 0) && (KV_WS->k_nb == 394);
    S4ASSERT(cond != 0, "B_ReportLine(\"%s\") == 0 -- nb objects=394", cmd); 
    
    U_test_reset_kmsg_msgs();         
}    


void Tests_B_REP_ENGINE()
{
    int     rc, cond;
    char    cmd[1024];

    U_test_print_title("Tests B_ReportExec(\"rep_expand.rep\")");
    U_test_suppress_kmsg_msgs();

    // Calls to B_ReportExec(reportfile)
    // Report rep_expand.rep: expand %% {lec}, {$cmd}, {$!cmd} and @fn().  
    
    sprintf(cmd,  "%s\\rep_expand.rep %s %s", IODE_DATA_DIR, IODE_DATA_DIR, IODE_OUTPUT_DIR);
    rc = B_ReportExec(cmd); // TODO: check that the output file is closed !!
    cond = (rc == 0) && U_test_compare_outfile_to_reffile("rep_expand.a2m", "rep_expand.ref.a2m");
    S4ASSERT(cond != 0, "B_ReportExec(\"%s\") == 0", cmd); 
    
    U_test_reset_kmsg_msgs();         
}    


void Tests_B_REP_FNS()
{
    int     rc, cond;
    char    cmd[1024];

    U_test_print_title("Tests B_ReportExec(\"rep_fns.rep\")");
    U_test_suppress_kmsg_msgs();

    // Execution of the report rep_fns.rep
    sprintf(cmd,  "%s\\rep_fns.rep %s %s", IODE_DATA_DIR, IODE_DATA_DIR, IODE_OUTPUT_DIR);
    rc = B_ReportExec(cmd); 
    cond = (rc == 0) && U_test_compare_outfile_to_reffile("rep_fns.a2m", "rep_fns.ref.a2m");
    S4ASSERT(cond != 0, "B_ReportExec(\"%s\") == 0", cmd); 
    
    U_test_reset_kmsg_msgs();         
}    

void Tests_B_REP_PROC()
{
    int     rc, cond;
    char    cmd[1024];

    U_test_print_title("Tests B_ReportExec(\"rep_proc.rep\")");
    U_test_suppress_kmsg_msgs();

    // Execution of the report rep_fns.rep
    sprintf(cmd,  "%s\\rep_proc.rep %s %s", IODE_DATA_DIR, IODE_DATA_DIR, IODE_OUTPUT_DIR);
    rc = B_ReportExec(cmd); 
    cond = (rc == 0) && U_test_compare_outfile_to_reffile("rep_proc.a2m", "rep_proc.ref.a2m");
    S4ASSERT(cond != 0, "B_ReportExec(\"%s\") == 0", cmd); 
    
    U_test_reset_kmsg_msgs();         
}    

//void Tests_B_PrintObjsDef()
//{
//    char reffilename[512];
//    char filename[512];
//
//    U_test_print_title("Tests B_PrintObjDef");
//    U_test_suppress_a2m_msgs();
//
//    // Define filenames
//    sprintf(filename, "%s\\test_printobj.htm", IODE_OUTPUT_DIR);
//    sprintf(reffilename, "%s\\test_printobj.ref.htm", IODE_DATA_DIR);
//
//    // Test cell separator '@'
//    W_dest(filename, W_HTML);       // Read A2M_SEPCH ini file
//    A2M_SEPCH = '@';                // Change A2M_SEPCH
//    B_PrintObjDef("C8_1", TABLES);   // Print TBL definition
//    W_close();                      // Close the printing session
//    //S4ASSERT(U_test_compare_outfile_to_reffile("test_printobj.htm", "test_printobj.ref.htm"), "B_PrintObjDef(%c)", A2M_SEPCH);
//    S4ASSERT(U_diff_files(reffilename, filename) != 0, "B_PrintObjDef(%c)", A2M_SEPCH);
//    
//    // Test cell separator '|'
//    W_dest(filename, W_HTML);       // Read A2M_SEPCH ini file
//    A2M_SEPCH = '|';                // Change A2M_SEPCH
//    B_PrintObjDef("C8_1", TABLES);   // Print TBL definition
//    W_close();                      // Close the printing session
//    U_test_compare_outfile_to_reffile("test_printobj.htm", "test_printobj.ref.htm");
//    //S4ASSERT(U_test_compare_outfile_to_reffile("test_printobj.htm", "test_printobj.ref.htm"), "B_PrintObjDef(%c)", A2M_SEPCH);
//    S4ASSERT(U_diff_files(reffilename, filename) != 0, "B_PrintObjDef(%c)", A2M_SEPCH);
//    
//    // Test cell separator '&'
//    W_dest(filename, W_HTML);       // Read A2M_SEPCH ini file
//    A2M_SEPCH = '&';                // Change A2M_SEPCH
//    B_PrintObjDef("C8_1", TABLES);   // Print TBL definition
//    W_close();                      // Close the printing session
//    //S4ASSERT(U_test_compare_outfile_to_reffile("test_printobj.htm", "test_printobj.ref.htm"), "B_PrintObjDef(%c)", A2M_SEPCH);
//    S4ASSERT(U_diff_files(reffilename, filename) != 0, "B_PrintObjDef(%c)", A2M_SEPCH);
//    // Reset initial kmsg fn
//    
//    U_test_reset_a2m_msgs();
//}


// void Tests_VTOMS()
// {
//     int             i;
// 	unsigned char   **tbl, *str;
// 
// 	str = "equal(\"abc \",,,\"abc \"";
//     
//     tbl = SCR_vtoms3(str, "(,", 1);
//     printf("str=\'%s'\n", str);
//     printf("SCR_vtoms3()\n");
// 	for(i = 0 ; tbl[i] ; i++)
// 	    printf("%d: \"%s\"\n", i, tbl[i]);
// 	SCR_free_tbl(tbl);
// 
//     tbl = SCR_vtomsq(str, "(,", '"');
//     printf("SCR_vtomsq()\n");
// 	for(i = 0 ; tbl[i] ; i++)
// 	    printf("%d: \"%s\"\n", i, tbl[i]);
// 	SCR_free_tbl(tbl);
// 
// }

// ================================================================================================


void U_test_init()
{
    static int  done = 0;
    
    if(done) return;
    done = 1;
 
    //B_IodeMsgPath();            // Set SCR_NAME_ERR to dir(current file) $curdir/iode.msg
    
    IODE_assign_super_API();    // set *_super fn pointers
    // strcpy(SCR_NAME_ERR, "iode.msg");   // message file => temporarily suppressed for GitHub 
    K_init_ws(0);                       // Initialises 7 empty WS
    B_A2mGetAllParms();
    
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

        if(strcmp(argv[i], "-data_dir") == 0) {    
            IODE_DATA_DIR   = argv[i + 1];
            i++;
        }    
        if(strcmp(argv[i], "-output_dir") == 0) {    
            IODE_OUTPUT_DIR = argv[i + 1];
            i++;
        }    
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
    Tests_TBL_ADD_GET();
    Tests_LEC();
    Tests_EQS();
    Tests_ARGS();
    Tests_K_OBJFILE();
    Tests_TBL32_64();
    Tests_Simulation();
    Tests_PrintTablesAndVars();
    Tests_Estimation();
    Tests_W_printf();
    Tests_B_IDT();
    Tests_B_DATA();
    Tests_B_EQS();
    Tests_B_FILE();
    Tests_B_FSYS();
    Tests_IMP_EXP();
    
    Tests_B_XODE();
    Tests_B_LTOH();
    Tests_B_HTOL();
    
    Tests_B_MODEL();
    Tests_B_WS();
    
    Tests_B_REP_LINE();
    Tests_B_REP_ENGINE();
    Tests_B_REP_FNS();
    Tests_B_REP_PROC();
    Tests_B_IDT_EXECUTE();
    
    // Tests_B_PrintObjsDef();
  
    // Tests_VTOMS(); Tests des fonctions SCR_vtoms*()
    //K_save_iode_ini(); // Suppress that call ? Should only be called on demand, not at the end of each IODE session.
    
    // Reset initial kmsg fn
    U_test_reset_a2m_msgs();
    
    return(0);
}


/*
REPORT PRIMITIVES
-----------------
X   "label",                    RP_label,               RP_label,           0,
X   "goto",                     RP_goto,                RP_goto,            0,
X   "ask",                      RP_ask,                 RP_ask,             0,
X   "abort",                    RP_abort,               RP_abort,           0,
X   "quitode",                  RP_quitode,             RP_quitode,         0,
X   "quit",                     RP_quitode,             RP_quitode,         0,     
X   "onerror",                  RP_onerror,             RP_onerror,         0,
X   "return",                   RP_return,              RP_return,          0,
X   "show",                     RP_message,             RP_message,         0,
X   "msg",                      RP_warning,             RP_warning,         0,
X   "beep",                     RP_beep,                RP_beep,            0,
X   "settime",                  RP_settime,             RP_settime,         0,
X   "incrtime",                 RP_incrtime,            RP_incrtime,        0,
X   "shift",                    RP_shift,               RP_shift,           0,
X   "define",                   RP_define,              RP_define,          0,
X   "debug",                    RP_setdebug,            RP_setdebug,        0, 
X   "indent",                   RP_setindent,           RP_setindent,       0, 
X   "shellexec",                B_shellexec,            B_shellexec,        0, 
X   "repeat",                   RP_repeat,              RP_repeat,          0, 
X   "repeatstring",             RP_repeatstring,        RP_repeatstring,    0, 
X   "vseps",                    RP_vseps,               RP_vseps,           0, 
X   "foreach",                  RP_foreach,             RP_foreach,         0, 
X   "next",                     RP_foreach_next,        RP_foreach_next,    0, 
X   "procdef",                  RP_procdef,             RP_procdef,         0, 
X   "procexec",                 RP_procexec,            RP_procexec,        0, 

X   "system",                   RP_system,              RP_system,          0,
X   "chdir",                    RP_chdir,               RP_chdir,           0,
X   "mkdir",                    RP_mkdir,               RP_mkdir,           0,
X   "rmdir",                    RP_rmdir,               RP_rmdir,           0,
    
    // fonctions utilisateur 
X   "fileimportvar",            B_FileImportVar,        SB_XodeRuleImport,  0,
    "fileimportcmt",            B_FileImportCmt,        SB_XodeRuleImport,  0,
X   "filedelete",               B_FileDelete,           NULL,               4,
X   "filerename",               B_FileRename,           NULL,               4,
X   "filecopy",                 B_FileCopy,             NULL,               4,
   
X   "wssample",                 B_WsSample,             SB_WsSample,        0,
X   "wsload",                   B_WsLoad,               SB_WsLoad,          3,
X   "wscopy",                   B_WsCopy,               SB_WsCopy,          3,
X   "wssave",                   B_WsSave,               SB_WsSave,          3,
X   "wssavecmp",                B_WsSaveCmp,            SB_WsSave,          3,
X   "wsimport",                 B_WsImport,             SB_WsLoad,          3,
X   "wsexport",                 B_WsExport,             SB_WsSave,          3,
X   "wsmerge",                  B_WsMerge,              SB_WsMerge,         3,
X   "wsclear",                  B_WsClear,              SB_WsClear,         3,
X   "wsclearall",               B_WsClearAll,           NULL,               0,
X   "wsdescr",                  B_WsDescr,              SB_WsDescr,         0,
X   "wsextrapolate",            B_WsExtrapolate,        SB_WsExtrapolate,   0,
X   "wsaggrchar",               B_WsAggrChar,           NULL,               0,
X   "wsaggrsum",                B_WsAggrSum,            SB_WsAggregate,     0,
X   "wsaggrmean",               B_WsAggrMean,           SB_WsAggregate,     0,
X   "wsaggrprod",               B_WsAggrProd,           SB_WsAggregate,     0,
X   "wshtollast",               B_WsHtoLLast,           SB_WsHtoL,          0,
X   "wshtolsum",                B_WsHtoLSum,            SB_WsHtoL,          0,
X   "wshtolmean",               B_WsHtoLMean,           SB_WsHtoL,          0,
X   "wsltohflow",               B_WsLtoHFlow,           SB_WsLtoH,          0,
X   "wsltohstock",              B_WsLtoHStock,          SB_WsLtoH,          0,
X   "csvsave",                  B_CsvSave,              NULL,               3,
X   "csvdigits",                B_CsvNbDec,             NULL,               0,
X   "csvsep",                   B_CsvSep,               NULL,               0,
X   "csvdec",                   B_CsvDec,               NULL,               0,
X   "csvnan",                   B_CsvNaN,               NULL,               0,
X   "csvaxes",                  B_CsvAxes,              NULL,               0,
X   "statunitroot",             B_StatUnitRoot,         SB_StatUnitRoot,    0,

    "wsseasonadj",              B_WsSeasonAdj,          SB_WsSeasonAdj,     0,
    "wstrend",                  B_WsTrend,              SB_WsTrend,         0,
    "wstrendstd",               B_WsTrendStd,           SB_WsTrend,         0,
    
    "wsimporteviews",			B_WsImportEviews,       NULL, 		        0,





// b_data.c 
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
X   "datapattern",              B_DataPattern,          NULL,               1,
X   "datadisplaygraph",         B_DataDisplayGraph,     SB_DataEditGraph,   0,
X   "dataprintgraph",           B_DataPrintGraph,       SB_DataEditGraph,   0,
X   "datascan",                 B_DataScan,             SB_DataScan,        1,

// b_ras.c
X   "datarasvar",               B_DataRasVar,           NULL,               0,
 
// b_est.c
X   "eqsestimate",              B_EqsEstimate,          SB_EqsEstimate,     0,
    "eqssetmethod",             B_EqsSetMethod,         NULL,               0,
    "eqssetbloc",               B_EqsSetBloc,           NULL,               0,
    "eqssetblock",              B_EqsSetBloc,           NULL,               0,
X    "eqssetsample",             B_EqsSetSample,         NULL,               0,
    "eqssetinstrs",             B_EqsSetInstrs,         NULL,               0,
    "eqssetcmt",                B_EqsSetCmt,            NULL,               0,

// b_step.c
    "eqsstepwise",              B_EqsStepWise,          NULL,               0,
    
    
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
    
X   "modelsimulate",            B_ModelSimulate,        SB_ModelSimulate,   0,
X   "modelsimulateparms",       B_ModelSimulateParms,   NULL,               0,
X   "modelexchange",            B_ModelExchange,        NULL,               0,
X   "modelcompile",             B_ModelCompile, 	    SB_ModelCompile,    0,
    
X   "modelcalcscc",             B_ModelCalcSCC,         NULL,               0,
X   "modelsimulatescc",         B_ModelSimulateSCC,     NULL,               0,

-    "modelsimulatesaveniters",  B_ModelSimulateSaveNIters,  NULL,           0,
-    "modelsimulatesavenorms",   B_ModelSimulateSaveNorms,   NULL,           0,

X   "idtexecute",               B_IdtExecute,           SB_IdtExecute,      0,
X   "idtexecutevarfiles",       B_IdtExecuteVarFiles,   NULL,               0,
X   "idtexecutesclfiles",       B_IdtExecuteSclFiles,   NULL,               0,
X   "idtexecutetrace",          B_IdtExecuteTrace,      NULL,               0,
    
X   "reportexec",               B_ReportExec,           SB_ReportExec,      0,
    "reportedit",               NULL,                   SB_ReportEdit,      0,
    "prompt",                   NULL,                   SB_ReportPrompt,    0,
    "minimize",                 B_WindowMinimize,       NULL,               0,
    "maximize",                 B_WindowMaximize,       NULL,               0,
X   "sleep",                    B_Sleep,                NULL,               0,
        
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

X    "sysmovefile",              B_SysRename,            NULL,               0,
X    "syscopyfile",              B_SysCopy,              NULL,               0,
X    "sysappendfile",            B_SysAppend,            NULL,               0,
X    "sysdeletefile",            B_SysDelete,            NULL,               0,
X    "sysansitoutf8",            B_SysAnsiToUTF8,        NULL,               0,
X    "sysoemtoutf8",             B_SysOemToUTF8,         NULL,               0,
X    "sysansitooem",             B_SysAnsiToOem,         NULL,               0,
X    "sysoemtoansi",             B_SysOemToAnsi,         NULL,               0,
                
    "a2mtohtml",                B_A2mToHtml,            NULL,               0,
    "a2mtomif",                 B_A2mToMif,             NULL,               0,
    "a2mtocsv",                 B_A2mToCsv,             NULL,               0,
    "a2mtortf",                 B_A2mToRtf,             NULL,               0,
    "a2mtoprinter",             B_A2mToPrinter,         NULL,               0,

*/