// The present source file has been generated from the source
// file test1.c (Google Test is not available with Borland).
// DO NOT MODIFY IT!
// Modify test1.c instead and run convert_tests.bat/sh


#include "pch.h"
#include <math.h>
#include <regex>
#include <filesystem>


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


// BEGIN_KEEP
#ifdef __GNUC__
    char    *SEPARATOR = "/";
    char    *IODE_DATA_DIR   = "../data/";
    char    *IODE_OUTPUT_DIR = "../data/output/";
    char    *IODE_REPORT_DIR = "../data/reports/";
    #else
    char    *SEPARATOR = "\\";
    char    *IODE_DATA_DIR   = "..\\data\\";
    char    *IODE_OUTPUT_DIR = "..\\data\\output\\";
    char    *IODE_REPORT_DIR = "..\\data\\reports\\";
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
//    //int o_estgr(char** titles, Sample *smpl, MAT* mlhs, MAT* mrhs, int view, int res) {return(0);}
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




class IodeCAPITest : public ::testing::Test 
{
protected:
    char build_dir[256];
	char input_test_dir[256];
	char output_test_dir[256];

public:
    void SetUp() override 
    {
        // NOTE: we assume that: 
		//       - current path is binaryDir/tests/test_cpp_api
		//       - data directory has been copied in binaryDir/tests (see CMakeLists.txt in root directory)
		std::filesystem::path cwd = std::filesystem::current_path();
		std::filesystem::path tests_dir = cwd.parent_path();
		std::filesystem::path build = tests_dir.parent_path();
        std::filesystem::path data_dir = tests_dir / "data";
        std::filesystem::path output_dir = tests_dir / "output";
#ifdef __GNUC__
		std::string separator = "/";
#else
        std::string separator = "\\";
#endif
        strcpy(build_dir, build.string().c_str());
		strcpy(input_test_dir, (data_dir.string() + separator).c_str());
		strcpy(output_test_dir, (output_dir.string() + separator).c_str());

        // other initializations
        U_test_init();
        kpause_continue = 1;
    }

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

	int U_cmp_files(char*file1, char*file2)
	{
	    int     rc = -1;        // ==
	    long    size1, size2;
	    char    *content1, *content2;
	    char curdir[512];
	
	    //printf("Comparing '%s' and '%s'\n", file1, file2);
#ifdef __GNUC__
	    getcwd(curdir, 511);
#else
        _getcwd(curdir, 511);
#endif
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

	void U_test_print_title(char* title)
	{
	    int i;
	
	    printf("\n\n%s\n", title);
	    for (i = 0; title[i]; i++) printf("-");
	    printf("\n");
	}

	void U_test_suppress_a2m_msgs()
	{
	    A2mMessage_toggle(0);
	}

	void U_test_reset_a2m_msgs()
	{
	    A2mMessage_toggle(1);
	}

	void U_test_suppress_kmsg_msgs()
	{
	    kmsg_toggle(0);
	}

	void U_test_reset_kmsg_msgs()
	{
	    kmsg_toggle(1);
	}

	void U_test_CreateObjects()
	{
	    char*       lst;
	    Sample*     smpl;
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
	    smpl = new Sample("2000Y1", "2020Y1");
	    KV_sample(KV_WS, smpl);
	    //SW_nfree(smpl);
	
	    // Creates new vars
	    nb = smpl->nb_periods;
	    for(i = 0; i < smpl->nb_periods; i++) {
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

	void U_test_lec(char* title, char* lec, int t, double expected_val)
	{
	    CLEC*   clec;
	    double  calc_val;
	    int     rc;
	
	    Period per = KSMPL(KV_WS)->start_period.shift(t);
	
	    clec = L_cc(lec);
	    //S4ASSERT ("L_cc", clec != 0);
	    //S4ASSERT ("L_link", 0 == L_link(KV_WS, KS_WS, clec));
	    rc = L_link(KV_WS, KS_WS, clec);
	    EXPECT_EQ(rc, 0);
	    calc_val = L_exec(KV_WS, KS_WS, clec, t);
	    SCR_free(clec);
	    //sprintf(buf, "Res=%10.3lf - Expected=%10.3lf %s L_exec(%s) in %s", calc_val, expected_val, title, lec, aper);
	    EXPECT_DOUBLE_EQ(round(expected_val * 1e6) / 1e6, round(calc_val * 1e6) / 1e6);
	}

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
	
	    sprintf(fullfilename,  "%s%s", IODE_DATA_DIR, filename);
	    kdb = K_interpret(type, fullfilename);
	    //S4ASSERT(kdb != NULL, "K_interpret(%d, \"%s\")", type, fullfilename);
	    return(kdb);
	}

	void U_test_load_fun_esv(char* filename)
	{
        char filename_eqs[256];
        char filename_scl[256];
        char filename_var[256];

	    // Frees 3 WS
	    K_free(KE_WS);
	    K_free(KS_WS);
	    K_free(KV_WS);

        sprintf(filename_eqs, "%s.ae", filename);
        sprintf(filename_scl, "%s.as", filename);
        sprintf(filename_var, "%s.av", filename);
	
	    // Loads 3 WS and check ok
	    KE_RWS = KE_WS = U_test_K_interpret(EQUATIONS, filename_eqs);
	    KS_RWS = KS_WS = U_test_K_interpret(SCALARS, filename_scl);
	    KV_RWS = KV_WS = U_test_K_interpret(VARIABLES, filename_var);
	}

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
	
	    sprintf(filename, "%stest1.%s", IODE_OUTPUT_DIR, typeext);
	    sprintf(reffilename, "%stest1.ref.%s", IODE_OUTPUT_DIR, typeext);
	    W_dest(filename, typeint);
	    U_test_W_printf_cmds();
	    W_close();
	    //printf("Comparing ref '%s' and '%s'\n", reffilename, filename);
	    //S4ASSERT(U_cmp_files(reffilename, filename), "W_printf -> %s", typeext);
	    EXPECT_NE(U_diff_files(reffilename, filename), 0);
	}

	int U_test_compare_outfile_to_reffile(char* outfile, char* reffile)
	{
	    char reffilename[512];
	    char filename[512];
	    char curdir[512];
	
	    sprintf(filename, "%s%s", IODE_OUTPUT_DIR, outfile);
	    sprintf(reffilename, "%s%s", IODE_DATA_DIR, reffile);
#ifdef __GNUC__
	    //getcwd(curdir, 500);
	    getcwd(curdir, 511);
#else
	    //_getcwd(curdir, 500);
        _getcwd(curdir, 511);
#endif
	    printf("Current dir   : '%s'\n", curdir);
	    printf("Output    file: '%s'\n", filename);
	    printf("Reference file: '%s'\n", reffilename);
	
	    //printf("Comparing ref '%s' and '%s'\n", reffilename, filename);
	    //return(U_cmp_files(reffilename, filename));
	    return(U_diff_files(reffilename, filename));
	}

	bool U_test_compare_localfile_to_reffile(char* outfile, char* reffile)
	{
	    char reffilename[512];
	    char filename[512];
	
	    sprintf(filename, ".%s%s", SEPARATOR, outfile);
	    sprintf(reffilename, "%s%s", IODE_OUTPUT_DIR, reffile);
	    return U_diff_files(reffilename, filename) == 1;
	}

	void U_test_create_a_file(char* filename, int type)
	{
	    //  Create a file
	    U_test_suppress_a2m_msgs();
	    W_dest(filename, type);
	    W_printf("This is a paragraph with accents: éàâêë\n"); // the current source file (test1.c) is ANSI coded
	    W_close();
	}

	bool U_test_file_exists(char* filename, char* msg )
	{
	    int     rc;
	
	    //  Check that the file exists
	    rc = _access(filename, 0);
	    EXPECT_EQ(rc, 0);
	    return rc == 0;
	}

	bool U_test_file_not_exists(char* filename, char* msg)
	{
	    int     rc;
	
	    //  Check that the file exists
	    rc = _access(filename, 0);
	    EXPECT_NE(rc, 0);
	    return rc != 0;
	}

	bool U_test_B_WsLoad(char*filename, int type, int expected_nb_objects)
	{
	    char    fullfilename[256];
	    int     rc;
	
	    sprintf(fullfilename,  "%s%s", IODE_DATA_DIR, filename);
	    rc = B_WsLoad(fullfilename, type);
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(K_WS[type]->k_nb, expected_nb_objects);
	    return rc == 0 && K_WS[type]->k_nb == expected_nb_objects;
	}

	bool U_test_B_WsSave(char* source_file, char* out_file, int type, int expected_nb_objects)
	{
	    char    fullfilename[256];
	    int     rc;
	
	    _unlink(out_file);
	    sprintf(fullfilename,  "%s%s", IODE_DATA_DIR, source_file);
	    rc = B_WsLoad(fullfilename, type);
	    EXPECT_EQ(rc, 0);
	    rc = B_WsSave(out_file, type);
	    EXPECT_EQ(rc, 0);
	    B_WsClear("", type);
	    rc = B_WsLoad(out_file, type);
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(K_WS[type]->k_nb, expected_nb_objects);
	    return rc == 0 && K_WS[type]->k_nb == expected_nb_objects;
	}

	bool U_test_B_WsSaveCmp(char* source_file, char* out_file, int type, int expected_nb_objects)
	{
	    char    fullfilename[256];
	    int     rc;
	
	    _unlink(out_file);
	    sprintf(fullfilename,  "%s%s", IODE_DATA_DIR, source_file);
	    rc = B_WsLoad(fullfilename, type);
	    EXPECT_EQ(rc, 0);
	    rc = B_WsSaveCmp(out_file, type);
	    EXPECT_EQ(rc, 0);
	    B_WsClear("", type);
	    rc = B_WsLoad(out_file, type);
	    EXPECT_EQ(rc, 0);
	    EXPECT_EQ(K_WS[type]->k_nb, expected_nb_objects);
	    return rc == 0 && K_WS[type]->k_nb == expected_nb_objects;
	}

	bool U_test_B_WsExport(char* source_file, char* out_file, int type)
	{
	    char    full_source_file[256];
	    char    full_out_file[256];
	    char    ref_out_file[256];
	    int     rc;
	
	    sprintf(full_source_file,  "%s%s", IODE_DATA_DIR, source_file);
	    sprintf(full_out_file,  "%s%s", IODE_OUTPUT_DIR, out_file);
	    sprintf(ref_out_file,  "%s%s", IODE_DATA_DIR, out_file);
	
	    rc = B_WsLoad(full_source_file, type);
	    EXPECT_EQ(rc, 0);
	
	    _unlink(full_out_file);
	    rc = B_WsExport(full_out_file, type);
	    EXPECT_EQ(rc, 0);
	    rc = U_diff_files(ref_out_file, full_out_file);
	    EXPECT_NE(rc, 0);
	    return rc != 0;
	}

	bool U_test_B_WsImport(char* source_file, int type, int expected_nb_objects)
	{
	    char    fullfilename[256];
	    int     rc;
	
	    B_WsClear("", type);
	    sprintf(fullfilename, "%s%s", IODE_DATA_DIR, source_file);
	    rc = B_WsImport(fullfilename, type);
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(K_WS[type]->k_nb, expected_nb_objects);
	    return rc == 0 && K_WS[type]->k_nb == expected_nb_objects;
	}

	bool U_test_B_WsClear(int type)
	{
	    int     rc;
	
	    rc = B_WsClear("", type);
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(K_WS[type]->k_nb, 0);
	    return K_WS[type]->k_nb == 0;
	}

	bool U_test_B_WsDescr(char* descr, int type)
	{
	    int     rc;
	
	    rc = B_WsDescr(descr, type);
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(std::string(KDESC(K_WS[type])), descr);
	    return rc == 0 && std::string(KDESC(K_WS[type])) == descr;
	}

	bool U_test_B_WsName(char* name, int type)
	{
	    int     rc;
	    char    *nameptr;
	
	    rc = B_WsName(name, type);
        EXPECT_EQ(rc, 0);
	    nameptr = K_get_kdb_nameptr(K_WS[type]);
	    EXPECT_EQ(std::string(nameptr), std::string(name));
	    return rc == 0 && std::string(nameptr) == std::string(name);
	}

	bool U_test_B_WsCopyVar()
	{
	    char    arg[256];
	    int     rc, nb, pos;
	    double *ACAF, ACAF91, ACAF92, ACAG90, ACAG92;
	
	    // 1. Copy full VAR file (Att: * required)
	    B_WsClearAll("");
	    sprintf(arg,  "%sfun.av *", IODE_DATA_DIR);
	    rc = B_WsCopy(arg, VARIABLES);
	    ACAF92 = U_test_calc_lec("ACAF[1992Y1]", 0);
	    ACAG92 = U_test_calc_lec("ACAG[1992Y1]", 0);
        EXPECT_EQ(rc, 0);
	    EXPECT_DOUBLE_EQ(ACAF92, 30.159000);
	    EXPECT_DOUBLE_EQ(ACAG92, -40.285998999999997);
	
	    // 2. Copy partial WS (92-93) on an existing one
	    // 2.1 Set Sample + create ACAF
	    B_WsClearAll("");
	    B_WsSample("1990Y1 2000Y1");
	    // Create ACAF = 0 1 2...
	    nb = 11;
	    ACAF = L_cc_link_exec("t", KV_WS, KS_WS);
	    pos = K_add(KV_WS, "ACAF", ACAF, &nb);
	
	    // 2.2 Copy ACAF and ACAG on 1992 & 1993 (does not replace 1991 for example)
	    sprintf(arg,  "%sfun.av 1992Y1 1993Y1 ACAF ACAG", IODE_DATA_DIR);
	    rc = B_WsCopy(arg, VARIABLES);
	
	    // 2.3 Tests
	    ACAF91 = U_test_calc_lec("ACAF[1991Y1]", 0);
	    ACAF92 = U_test_calc_lec("ACAF[1992Y1]", 0);
	    ACAG90 = U_test_calc_lec("ACAG[1990Y1]", 0);
	    ACAG92 = U_test_calc_lec("ACAG[1992Y1]", 0);
        EXPECT_EQ(rc, 0);
	    EXPECT_DOUBLE_EQ(ACAF91, 1.0);
	    EXPECT_DOUBLE_EQ(ACAF92, 30.159000);
	    EXPECT_DOUBLE_EQ(ACAG90, IODE_NAN);	    
        EXPECT_DOUBLE_EQ(ACAG92, -40.285998999999997);
	
	    // 3. Copy partial WS on an existing one w/o specifying the sample
	    // 3.1 Set Sample
	    B_WsClearAll("");
	    B_WsSample("1990Y1 2000Y1");
	
	    // Copy ACAF and ACAG (does not specify a sample)
	    sprintf(arg,  "%sfun.av ACAF ACAG", IODE_DATA_DIR);
	    rc = B_WsCopy(arg, VARIABLES);
	    ACAF92 = U_test_calc_lec("ACAF[1992Y1]", 0);
	    ACAG92 = U_test_calc_lec("ACAG[1992Y1]", 0);
        EXPECT_EQ(rc, 0);
	    EXPECT_DOUBLE_EQ(ACAF92, 30.159000);
	    EXPECT_DOUBLE_EQ(ACAG92, -40.285998999999997);
	
	    return true;
	}

	bool U_test_B_WsCopy(char* filename, int type, int expected_nb_objects)
	{
	    char    arg[256];
	    int     rc;
	
	    // 1. Copy entire file (Att: * required)
	    B_WsClear("", type);
	    sprintf(arg,  "%s%s *", IODE_DATA_DIR, filename);
	    rc = B_WsCopy(arg, type);
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(K_WS[type]->k_nb, expected_nb_objects);
	    return rc == 0 && K_WS[type]->k_nb == expected_nb_objects;
	}

	bool U_test_B_WsMergeVar()
	{
	    int         rc, nb, pos;
	    char        arg[256];
	    double   *ACAF, ACAF92, ACAF00, ACAF16, ACAG92, ACAG00;
	
	    // 1. Merge into an empty WS
	    B_WsClearAll("");
	    sprintf(arg,  "%sfun.av", IODE_DATA_DIR);
	    rc = B_WsMerge(arg, VARIABLES);
	    ACAF92 = U_test_calc_lec("ACAF[1992Y1]", 0);
	    ACAG92 = U_test_calc_lec("ACAG[1992Y1]", 0);
        EXPECT_EQ(rc, 0);
	    EXPECT_DOUBLE_EQ(ACAF92, 30.159000);
	    EXPECT_DOUBLE_EQ(ACAG92, -40.285998999999997);
	
	    // 2. Merge into an existing WS inb a different Sample
	    B_WsClearAll("");
	    B_WsSample("2000Y1 2020Y1");
	    // Create ACAF = 0 1 2...
	    nb = 21;
	    ACAF = L_cc_link_exec("t", KV_WS, KS_WS);
	    pos = K_add(KV_WS, "ACAF", ACAF, &nb);
	    // Merge
	    sprintf(arg,  "%sfun.av", IODE_DATA_DIR);
	    rc = B_WsMerge(arg, VARIABLES);
	    //Check
	    ACAF00 = U_test_calc_lec("ACAF[2000Y1]", 0);
	    ACAF16 = U_test_calc_lec("ACAF[2016Y1]", 0);
	    ACAG00 = U_test_calc_lec("ACAG[2000Y1]", 0);
        EXPECT_EQ(rc, 0);
	    EXPECT_DOUBLE_EQ(round(ACAF00 * 1e8) / 1e8, 10.04661079);
	    EXPECT_DOUBLE_EQ(ACAF16, 16.0);
	    EXPECT_DOUBLE_EQ(round(ACAG00 * 1e8) / 1e8, -41.53478657);

	    return true;
	}

	bool U_test_B_WsMerge(char* filename, int type, int expected_nb_objects)
	{
	    char    arg[256];
	    int     rc;
	
	    // 1. Copy entire file (Att: * required)
	    B_WsClear("", type);
	    sprintf(arg,  "%s%s *", IODE_DATA_DIR, filename);
	    rc = B_WsMerge(arg, type);
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(K_WS[type]->k_nb, expected_nb_objects);
	    return rc == 0 && K_WS[type]->k_nb == expected_nb_objects;
	}

	bool U_test_B_WsExtrapolate(int method, double expected_value)
	{
	    double   *ACAF, ACAF2002;
	    char        arg[512];
	    int         pos, rc, nb;
	
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
        EXPECT_EQ(rc, 0);
	    EXPECT_DOUBLE_EQ(ACAF2002, expected_value);
	    return true;
	}

	bool U_test_create_var(char*name, char*lec)
	{
	    double  *A;
	    int     nb;
	
	    if(KV_WS == NULL) 
            return false;
        
	    nb = KSMPL(KV_WS)->nb_periods;
	    A = L_cc_link_exec(lec, KV_WS, KS_WS);
	    K_add(KV_WS, name, A, &nb);
	    SCR_free(A);
	    return true;
	}

	bool U_test_B_WsAggregate()
	{
	    double   A_2000, B_2000, AC_2000;
	    char        arg[512];
	    int         rc;
	
	    B_WsClearAll("");
	    B_WsSample("1995Y1 2020Y1");
	
	    // Create VARS A1, A2, B1, B2
	    U_test_create_var("A1", "t");
	    U_test_create_var("A2", "t*2");
	    U_test_create_var("B1", "t**2");
	    U_test_create_var("B2", "t**3");
	
	    //  $WsAggrChar [char]
	    rc = B_WsAggrChar("");
	    EXPECT_EQ(rc, 0);
	
	    // $WsAggrSum  pattern
	    strcpy(arg, "(?)[?]");
	    rc = B_WsAggrSum(arg);
	    A_2000 = U_test_calc_lec("A[2000Y1]", 0);
	    B_2000 = U_test_calc_lec("B[2000Y1]", 0);
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(A_2000, 5 + 5*2); 
        EXPECT_EQ(B_2000, 5 * 5 + 5 * 5 * 5);
	
	    // $WsAggrProd  pattern
	    strcpy(arg, "(?)[?]");
	    rc = B_WsAggrProd(arg);
	    A_2000 = U_test_calc_lec("A[2000Y1]", 0);
	    B_2000 = U_test_calc_lec("B[2000Y1]", 0);
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(A_2000, (5) * (5*2));
        EXPECT_EQ(B_2000, (5 * 5) * (5 * 5 * 5));
	
	    // $WsAggrMean  pattern
	    strcpy(arg, "(?)[?]");
	    rc = B_WsAggrMean(arg);
	    A_2000 = U_test_calc_lec("A[2000Y1]", 0);
	    B_2000 = U_test_calc_lec("B[2000Y1]", 0);
        EXPECT_EQ(rc, 0);
	    EXPECT_DOUBLE_EQ(A_2000, 7.5); 
        EXPECT_DOUBLE_EQ(B_2000, 75.0);
	
	    // $WsAggrSum  pattern filename
	    sprintf(arg,  "(AC)[??] %sfun.av", IODE_DATA_DIR);
	    rc = B_WsAggrSum(arg);
	    AC_2000 = U_test_calc_lec("AC[2000Y1]", 0);
        EXPECT_EQ(rc, 0);
	    EXPECT_DOUBLE_EQ(round(AC_2000 * 1e8) / 1e8, -31.48817578);
	
	    return true;
	}

	bool U_test_B_StatUnitRoot(int drift, int trend, int order, char* varname, double expected_df)
	{
	    int     rc;
	    double  df;
	    char    arg[256], scalar[30];
	
	    // Load needed data
	    //U_test_B_WsLoad("fun", SCALARS, 161);
	    //U_test_B_WsLoad("fun", VARIABLES, 394);
	
	    // Dickey-Fuller test (E_UnitRoot)
	    // int B_StatUnitRoot(char* arg, int unused)                     $StatUnitRoot drift trend order expression
	    sprintf(arg, "%d %d %d %s", drift, trend, order, varname);
	    rc = B_StatUnitRoot(arg);
	    sprintf(scalar, "df_%s", varname);
	    df = U_test_calc_lec(scalar, 0);
        EXPECT_EQ(rc, 0);
	    EXPECT_DOUBLE_EQ(df, expected_df);
	    return true;
	}

	bool U_test_B_Csv()
	{
	    int     rc;
	    char    arg[512];
	
	    // int B_CsvNbDec(char *nbdec, int unused)                       $CsvNbDec nn
	    // int B_CsvSep(char *sep, int unused)                           $CsvSep char
	    // int B_CsvNaN(char *nan, int unused)                           $CsvNaN text
	    // int B_CsvAxes(char *var, int unused)                          $CsvAxes AxisName
	    // int B_CsvDec(char *dec, int unused)                           $CsvDec char
	    // int B_CsvSave(char* arg, int type)                $CsvSave<type> file name1 name2 ...
	    rc = B_CsvNbDec("7");
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(AsciiVariables::CSV_NBDEC, 7);
	
	    rc = B_CsvSep(";");
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(AsciiVariables::CSV_SEP[0], ';');
	
	    rc = B_CsvNaN("--");
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(std::string(AsciiVariables::CSV_NAN), "--");
	
	    rc = B_CsvAxes("Name");
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(std::string(AsciiVariables::CSV_AXES), "Name");
	
	    rc = B_CsvDec(".");
        EXPECT_EQ(rc, 0);
	    EXPECT_EQ(std::string(AsciiVariables::CSV_DEC), ".");
	
	    U_test_B_WsLoad("fun", VARIABLES, 394);
	    sprintf(arg, "%s\\funcsv.csv A* *G", IODE_OUTPUT_DIR);
	    rc = B_CsvSave(arg, VARIABLES);
        EXPECT_EQ(rc, 0);
	    U_test_compare_outfile_to_reffile("funcsv.csv", "funcsv.csv");
	
	    return true;
	}

	void U_test_init()
	{
	    static int  done = 0;
	
	    if(done) return;
	    done = 1;
		
	    IODE_assign_super_API();    // set *_super fn pointersressed for GitHub
	    K_init_ws(0);               // Initialises 7 empty WS
	    B_A2mGetAllParms();
	
	}

	//void TearDown() override {}

};


TEST_F(IodeCAPITest, Tests_BUF)
{
    U_test_print_title("Tests BUF");
    EXPECT_EQ(BUF_DATA, nullptr);
    EXPECT_NE(BUF_strcpy("ABCD"), nullptr);
    EXPECT_NE(BUF_alloc(100), nullptr);
    EXPECT_NE(BUF_strcpy("ABCD"), nullptr);
    BUF_free();
    EXPECT_EQ(BUF_DATA, nullptr);
}


TEST_F(IodeCAPITest, Tests_OBJECTS)
{
    char*       lst;
    int         pos;
    static int  done = 0;

    U_test_print_title("Tests OBJECTS");
    U_test_CreateObjects();

    // Create lists
    pos = K_find(KL_WS, "LST1");
    EXPECT_TRUE(pos >= 0);
    lst = KLPTR("LST1");
    EXPECT_EQ(strcmp(lst, "A,B"), 0);


    pos = K_find(KV_WS, "A");
    EXPECT_TRUE(K_find(KV_WS, "A") >= 0);

    pos = K_ren(KV_WS, "A", "AAA");
    EXPECT_TRUE(K_find(KV_WS, "AAA") >= 0);

    // Test KV_sample()
    std::string asmpl1 = KSMPL(KV_WS)->to_string();
    KV_sample(KV_WS, NULL);
    std::string asmpl2 = KSMPL(KV_WS)->to_string();
    EXPECT_EQ(asmpl1, asmpl2);
}


TEST_F(IodeCAPITest, Tests_TBL_ADD_GET)
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
    EXPECT_EQ(tbl->t_lang, extracted_tbl->t_lang);
    EXPECT_EQ(tbl->t_free, extracted_tbl->t_free);
    EXPECT_EQ(tbl->t_nc, extracted_tbl->t_nc);
    EXPECT_EQ(tbl->t_nl, extracted_tbl->t_nl);
    EXPECT_EQ(tbl->t_zmin, extracted_tbl->t_zmin);
    EXPECT_EQ(tbl->t_zmax, extracted_tbl->t_zmax);
    EXPECT_EQ(tbl->t_ymin, extracted_tbl->t_ymin);
    EXPECT_EQ(tbl->t_ymax, extracted_tbl->t_ymax);
    EXPECT_EQ(tbl->t_attr, extracted_tbl->t_attr);
    EXPECT_EQ(tbl->t_box, extracted_tbl->t_box);
    EXPECT_EQ(tbl->t_shadow, extracted_tbl->t_shadow);
    EXPECT_EQ(tbl->t_gridx, extracted_tbl->t_gridx);
    EXPECT_EQ(tbl->t_gridy, extracted_tbl->t_gridy);
    EXPECT_EQ(tbl->t_axis, extracted_tbl->t_axis);
    EXPECT_EQ(tbl->t_align, extracted_tbl->t_align);

    // ----- check div line
    EXPECT_EQ(tbl->t_div.tl_type, extracted_tbl->t_div.tl_type);
    EXPECT_EQ(tbl->t_div.tl_graph, extracted_tbl->t_div.tl_graph);
    EXPECT_EQ(tbl->t_div.tl_axis, extracted_tbl->t_div.tl_axis);
    EXPECT_EQ(tbl->t_div.tl_pbyte, extracted_tbl->t_div.tl_pbyte);
    cells_0 = (TCELL*) tbl->t_div.tl_val;
    cells_1 = (TCELL*) extracted_tbl->t_div.tl_val;

    for(j = 0; j < tbl->t_nc; j++)
    {
        EXPECT_EQ(cells_0[j].tc_type, cells_1[j].tc_type);
        EXPECT_EQ(cells_0[j].tc_attr, cells_1[j].tc_attr);
    }

    // ----- check all lines
    for(i = 0; i < tbl->t_nl; i++)
    {
        line_0 = tbl->t_line + i;
        line_1 = extracted_tbl->t_line + i;

        EXPECT_EQ(line_0->tl_type, line_1->tl_type);
        EXPECT_EQ(line_0->tl_graph, line_1->tl_graph);
        EXPECT_EQ(line_0->tl_axis, line_1->tl_axis);
        EXPECT_EQ(line_0->tl_pbyte, line_1->tl_pbyte);

        cells_0 = (TCELL*) line_0->tl_val;
        cells_1 = (TCELL*) line_1->tl_val;
        switch (line_0->tl_type)
        {
          case TABLE_LINE_TITLE:
            cell_cont_0 = (char*) SCR_stracpy((unsigned char*)T_cell_cont(cells_0, 0));
            cell_cont_1 = (char*) SCR_stracpy((unsigned char*)T_cell_cont(cells_1, 0));
            EXPECT_EQ(std::string(cell_cont_0), std::string(cell_cont_1));
            SCR_free(cell_cont_0);
            SCR_free(cell_cont_1);
            break;
          case TABLE_LINE_CELL:
            for(j = 0; j < tbl->t_nc; j++)
            {
                EXPECT_EQ(cells_0[j].tc_type, cells_1[j].tc_type);
                EXPECT_EQ(cells_0[j].tc_attr, cells_1[j].tc_attr);
                cell_cont_0 = (char*)SCR_stracpy((unsigned char*)T_cell_cont(cells_0, j));
                cell_cont_1 = (char*)SCR_stracpy((unsigned char*)T_cell_cont(cells_1, j));
                EXPECT_EQ(std::string(cell_cont_0), std::string(cell_cont_1));
                SCR_free(cell_cont_0);
                SCR_free(cell_cont_1);
            }
            break;
          default:
            EXPECT_TRUE(cells_0 == NULL); 
            EXPECT_TRUE(cells_1 == NULL);
            break;
        }
    }

    // --- free memory
    T_free(tbl);
    T_free(extracted_tbl);
}


TEST_F(IodeCAPITest, Tests_LEC)
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


TEST_F(IodeCAPITest, Tests_EQS)
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


TEST_F(IodeCAPITest, Tests_ARGS)
{
    char **args;
    char *list[] = {"A1", "A2", 0};
    char filename[256];

    U_test_print_title("Tests ARGS");

    // Create objects
    U_test_CreateObjects();

    // A_init
    args = B_ainit_chk("A B;C,D", NULL, 0); // => "A" "B;C" "D"
    EXPECT_TRUE(U_cmp_tbls(args, "A|B;C|D"));
    SCR_free_tbl((unsigned char**) args);

    // A_init
    args = B_ainit_chk("$LST1", NULL, 0);
    EXPECT_TRUE(U_cmp_tbls(args, "A|B"));
    SCR_free_tbl((unsigned char**) args);
    //args = B_ainit_chk("A*", NULL, 0);

    // Test parameters in a file. test.args must exist in the current dir and contain the line
    // A1 A2
    sprintf(filename, "@%stest.args", IODE_DATA_DIR);
    args = B_ainit_chk(filename, NULL, 0);
    EXPECT_TRUE(U_cmp_tbls(args, "A1|A2"));
    SCR_free_tbl((unsigned char**) args);
}


TEST_F(IodeCAPITest, Tests_ERRMSGS)
{
    U_test_print_title("Tests Err Msgs");

    error_manager.append_error("bla bla: incorrect period");
    kerror(0, "Coucou de kerror %s", "Hello");
    kmsg("Coucou de kmsg %s -- %g", "Hello", 3.2);
}


TEST_F(IodeCAPITest, Tests_K_OBJFILE)
{
    char    in_filename[256];
    char    out_filename[256];
    KDB     *kdb_var;
    int     rc;

    U_test_print_title("Tests K_OBJFILE");

    sprintf(in_filename,  "%sfun.av", IODE_DATA_DIR);
    sprintf(out_filename, "%sfun_copy.av", IODE_OUTPUT_DIR);

    kdb_var = K_interpret(VARIABLES, in_filename);
    EXPECT_NE(kdb_var, nullptr);
    if(kdb_var) {
        EXPECT_EQ(KNB(kdb_var), 394);
        rc = K_save(kdb_var, out_filename);
        EXPECT_EQ(rc, 0);
    }
}


TEST_F(IodeCAPITest, Tests_TBL32_64)
{
    char    in_filename[256];
    char    out_filename[256];
    KDB     *kdb_tbl;
    int     rc;

    int     pos;
    TBL*    c_table;
    TCELL*  cells;

#ifdef _MSC_VER
    U_test_print_title("Tests conversion 32 to 64 bits");

    sprintf(in_filename,  "%sfun.tbl", IODE_DATA_DIR);

    kdb_tbl = K_interpret(TABLES, in_filename);
    EXPECT_NE(kdb_tbl, nullptr);
    if(kdb_tbl) {
        sprintf(out_filename, "%s\\fun_copy.at", IODE_OUTPUT_DIR);
        rc = ascii_handlers[TABLES]->save_asc(kdb_tbl, out_filename);
        EXPECT_EQ(rc, 0);
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
#endif
}


TEST_F(IodeCAPITest, Tests_Simulation)
{
    KDB         *kdbv,
                *kdbe,
                *kdbs;
    Sample      *smpl;
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
    EXPECT_NE(kdbv, nullptr);
    kdbs = KS_WS;
    EXPECT_NE(kdbs, nullptr);
    kdbe = KE_WS;
    EXPECT_NE(kdbe, nullptr);

    // Check list is empty
    lst = KLPTR("_DIVER");
    EXPECT_TRUE(lst == NULL);

    // Simulation instance
    CSimulation simu;

    // Simulation parameters
    smpl = new Sample("2000Y1", "2002Y1");
    CSimulation::KSIM_START = VAR_INIT_TM1;
    CSimulation::KSIM_EPS = 0.0001;
    CSimulation::KSIM_MAXIT = 100;
    CSimulation::KSIM_RELAX = 0.7;
    CSimulation::KSIM_SORT = SORT_BOTH;
    CSimulation::KSIM_PASSES = 5;
    CSimulation::KSIM_DEBUG = 1;

    kmsg_super_ptr = kmsg_super;
    kmsg_super = kmsg_null; // Suppress messages at each iteration during simulation

    // Test simulation : divergence
    CSimulation::KSIM_MAXIT = 2;
    rc = simu.K_simul(kdbe, kdbv, kdbs, smpl, NULL, NULL);
    EXPECT_NE(rc, 0);

    // Check _PRE list after simulation (prolog)
    lst = KLPTR("_PRE");
    expected_lst = "BRUGP;DTH1C;EX;ITCEE;ITCR;ITGR;ITI5R;ITIFR;ITIGR;ITMQR;NATY;POIL;PW3;PWMAB;PWMS;PWXAB;PWXS;PXE;QAH;QWXAB;QWXS;QWXSS;SBGX;TFPFHP_;TWG;TWGP;ZZF_;DTH1;PME;PMS;PMT";
    //printf("     '%s'(%d)\n", expected_lst, strlen(expected_lst));
    EXPECT_EQ(std::string(lst), std::string(expected_lst));

    // Check _DIVER list
    lst = KLPTR("_DIVER");
    //printf("'%s'\n", lst);
    expected_lst = "SSH3O,WBG,SSF3,YDH,DTH,YDTG,YSFIC,WMIN,WLCP,WBGP,YSEFT2,YSEFT1,YSEFP,SBG,PWBG,W,ZJ,QMT,QI5,QC_,SSFG,YDH_,SG,ACAG,FLG";
    EXPECT_EQ(std::string(lst), std::string(expected_lst));

    // Test with with convergence (increase MAXIT)
    CSimulation::KSIM_MAXIT = 100;
    rc = simu.K_simul(kdbe, kdbv, kdbs, smpl, NULL, NULL);
    EXPECT_EQ(rc, 0);
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
    rc = simu.K_simul(kdbe, kdbv, kdbs, smpl, (char**)endo_exo, NULL);

    // Check result
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(KV_get_at_aper("UY", "2000Y1"), 650.0);
    XNATY_2000Y1 = KV_get_at_aper("XNATY", "2000Y1");
    //printf("XNATY_2000Y1 = %lg\n", XNATY_2000Y1);
    EXPECT_DOUBLE_EQ(round(KV_get_at_aper("XNATY", "2000Y1") * 1e6) / 1e6, 0.800703);

    // Cleanup
    SCR_free_tbl(endo_exo);
    delete smpl;
    kmsg_super = kmsg_super_ptr; // reset default value
}


TEST_F(IodeCAPITest, Tests_PrintTablesAndVars)
{
    char    fullfilename[256];
    char    **varlist;
    Sample  *smpl;
    KDB     *kdbv, *kdbt;
    TBL     *tbl;
    int     rc;

    U_test_suppress_a2m_msgs();

    U_test_print_title("Tests Print TBL as Tables and Graphs");

    // Load the VAR workspace
    K_RWS[VARIABLES][0] = K_WS[VARIABLES] = kdbv  = U_test_K_interpret(VARIABLES, "fun.av");
    EXPECT_NE(kdbv, nullptr);

    // Load the TBL workspace
    K_RWS[TABLES][0] = K_WS[TABLES] = kdbt  = U_test_K_interpret(TABLES, "fun.at");
    EXPECT_NE(kdbt, nullptr);

    // Load a second VAR workspace in K_RWS[VARIABLES][2]
    sprintf(fullfilename,  "%s%s", IODE_DATA_DIR, "fun.av");
    rc = K_load_RWS(2, fullfilename);
    EXPECT_EQ(rc, 0);


    // Select a table
    tbl = KTPTR("C8_1");
    EXPECT_NE(tbl, nullptr);

    // Select Print destination
    //W_dest("test1_tbl.htm", W_HTML);
    W_dest("test1_tbl.a2m", W_A2M);
    //W_dest("", W_GDI);

    // Print tbl as table
    rc = T_print_tbl(tbl, "2000:5[1;2]");
    EXPECT_EQ(rc, 0);

    // Print tbl as a graph
    rc = T_graph_tbl_1(tbl, "2000/1999:15[1;2]", 1);
    EXPECT_EQ(rc, 0);

    // Frees tbl
    T_free(tbl);

    // Print vars as graphs
    varlist = (char**) SCR_vtoms((U_ch*)"ACAF,ACAG,ACAF+ACAG", (U_ch*)",;");
    smpl = new Sample("1990Y1", "2010Y1");
    rc = V_graph(0, 0, 0, 1, 1, 0, IODE_NAN, IODE_NAN, smpl, varlist);
    EXPECT_EQ(rc, 0);
    SCR_free_tbl((U_ch**)varlist);
    delete smpl;

    // Close the output file
    W_close();

    // Cleanup the 2d VAR ws
    K_load_RWS(2, NULL);

    // Reset A2M messages
    U_test_reset_a2m_msgs();
}


TEST_F(IodeCAPITest, Tests_Estimation)
{
    int         rc;
    void        (*kmsg_super_ptr)(const char*);
    Sample      *smpl;
    double      r2, *df;

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
    Estimation est("ACAF");
    rc = est.estimate("1980Y1", "1996Y1");
    EXPECT_EQ(rc, 0);

    //x = U_test_calc_lec("_YRES0[1980Y1]", 0);
    //printf("x = %lf\n", x);
    //x = fabs(x + 0.001150);
    EXPECT_DOUBLE_EQ(round(U_test_calc_lec("_YRES0[1980Y1]", 0) * 1e8) / 1e8, -0.00115008);

    //x = fabs(K_e_r2(KE_WS, "ACAF") - 0.821815);
    EXPECT_DOUBLE_EQ(round(K_e_r2(KE_WS, "ACAF") * 1e6) / 1e6, 0.821815);

    //TODO:add some tests with other estimation methods / on blocks / with instruments

    //W_flush();
    W_close();


    // estimate_step_wise
    smpl = new Sample("1980Y1", "1995Y1");
    r2 = estimate_step_wise(smpl, "ACAF", "", "r2");
    EXPECT_DOUBLE_EQ(round(r2 * 1e6) / 1e6, 0.848519);
    delete smpl;

    // B_EqsStepWise
    for(int i = 0; i < 4; i++)
    {
        KSVAL(KS_WS, i)->value = 0.9;
        KSVAL(KS_WS, i)->relax = 1.0;
    }
    rc = B_EqsStepWise("1980Y1 1995Y1 ACAF 1 r2");
    EXPECT_EQ(rc, 0);

    for(int i = 0; i < 4; i++)
    {
        KSVAL(KS_WS, i)->value = 0.9;
        KSVAL(KS_WS, i)->relax = 1.0;
    }
    rc = B_EqsStepWise("1980Y1 1995Y1 ACAF 1 fstat");
    EXPECT_EQ(rc, 0);

    for(int i = 0; i < 4; i++)
    {
        KSVAL(KS_WS, i)->value = 0.9;
        KSVAL(KS_WS, i)->relax = 1.0;
    }
    rc = B_EqsStepWise("1980Y1 1995Y1 ACAF \"acaf2 > 0\" r2");
    EXPECT_EQ(rc, 0);

    for(int i = 0; i < 4; i++)
    {
        KSVAL(KS_WS, i)->value = 0.9;
        KSVAL(KS_WS, i)->relax = 1.0;
    }
    rc = B_EqsStepWise("1980Y1 1995Y1 ACAF \"acaf2 > 0\" fstat");
    EXPECT_EQ(rc, 0);

    // Dickey-Fuller test (E_UnitRoot)
    df = E_UnitRoot("ACAF+ACAG", 0, 0, 0);
    EXPECT_DOUBLE_EQ(round(df[2] * 1e6) / 1e6, -1.602170);
    df = E_UnitRoot("ACAF+ACAG", 1, 0, 0);
    EXPECT_DOUBLE_EQ(round(df[2] * 1e6) / 1e6, -2.490054);
    df = E_UnitRoot("ACAF+ACAG", 1, 1, 0);
    EXPECT_DOUBLE_EQ(round(df[2] * 1e6) / 1e6, -2.638717);
    df = E_UnitRoot("ACAF+ACAG", 0, 0, 1);
    EXPECT_DOUBLE_EQ(round(df[2] * 1e6) / 1e6, -1.300049);


    // Reset initial kmsg fn
    kmsg_super = kmsg_super_ptr; // Reset initial output to
    U_test_reset_a2m_msgs();

}


TEST_F(IodeCAPITest, Tests_ALIGN)
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


TEST_F(IodeCAPITest, Tests_W_printf)
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


TEST_F(IodeCAPITest, Tests_SWAP)
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


TEST_F(IodeCAPITest, Tests_B_DATA)
{
    char        *lst, buf[512];
    int         rc, i;
    double   *A1, val;
    Sample      *smpl;
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
    EXPECT_EQ(std::string(lst), "AB,AC,BB,BC");

    // B_DataCalcVar()
    rc = B_DataCalcVar("A1 2 * B");
    A1 = KVPTR("A1");
    EXPECT_EQ(rc, 0);
    EXPECT_TRUE(K_find(KV_WS, "A1") >= 0);
    EXPECT_EQ(A1[1], 4);

    // B_DataCreate(char* arg, int type)
    // B_DataDuplicate(char* arg, int type)
    // B_DataRename(char* arg, int type)
    // B_DataDelete(char* arg, int type)
    for(i = 0; i < 7 ; i++) {
        rc = B_DataCreate("XXX", i);
        EXPECT_EQ(rc, 0);
        EXPECT_TRUE(K_find(K_WS[i], "XXX") >= 0);

        if(i != EQUATIONS) { // Equations cannot be renamed or duplicated
            rc = B_DataDuplicate("XXX YYY", i);
            EXPECT_EQ(rc, 0);
            EXPECT_TRUE(K_find(K_WS[i], "YYY") >= 0);

            rc = B_DataRename("YYY ZZZ", i);
            EXPECT_EQ(rc, 0);
            EXPECT_TRUE(K_find(K_WS[i], "ZZZ") >= 0);
        }

        rc = B_DataDelete("XXX", i);
        EXPECT_EQ(rc, 0);
        EXPECT_TRUE(K_find(K_WS[i], "XXX") < 0);
    }

    // B_DataListSort()
    rc = K_add(KL_WS, "LIST1", "A;C;B");
    EXPECT_TRUE(K_find(KL_WS, "LIST1") >= 0);
    rc = B_DataListSort("LIST1 LIST2");
    lst = KLPTR("LIST2");
    EXPECT_EQ(std::string(lst), "A;B;C");

    // B_DataListSort() Example 2
    K_add(KL_WS, "L1", "C;B;$L2;$L3");
    K_add(KL_WS, "L2", "X Z Y");
    K_add(KL_WS, "L3", "A B D");
    rc = B_DataListSort("L1 RES");
    lst = KLPTR("RES");
    EXPECT_EQ(std::string(lst), "A;B;B;C;D;X;Y;Z");

    // B_DataUpdate()
    rc = B_DataUpdate("U Comment of U"       , COMMENTS);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(KCPTR("U")), "Comment of U");

    rc = B_DataUpdate("U U := c1 + c2*Z"     , EQUATIONS);

    rc = B_DataUpdate("U 2 * A"              , IDENTITIES);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(KIPTR("U")), "2 * A");

    rc = B_DataUpdate("U A,B,C"             , LISTS);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(KLPTR("U")), "A,B,C");

    rc = B_DataUpdate("U  1.2 1"             , SCALARS);
    val = K_s_get_value (KS_WS, "U");
    EXPECT_EQ(rc, 0);
    EXPECT_DOUBLE_EQ(val, 1.2);

    rc = B_DataUpdate("U  Title of U;U;2*U"  , TABLES);
    smpl = KSMPL(KV_WS);
    rc = B_DataUpdate("U L 2000Y1 2 3.1 4e2" , VARIABLES);
    EXPECT_EQ(rc, 0);

    // B_DataSearch(char* arg, int type)
    rc = B_DataSearch("of 0 0 1 0 1 NEWLIST", COMMENTS);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(KLPTR("NEWLIST")), "U");

    // B_DataScan(char* arg, int type)
    rc = B_DataScan("U", EQUATIONS);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(KLPTR("_SCAL")), "c1;c2");

    // B_DataExist(char* arg, int type)
    rc = B_DataExist("_SCAL", LISTS);
    EXPECT_TRUE((rc >= 0));

    // B_DataAppend(char* arg, int type)
    rc = B_DataAppend("_SCAL XXX,YYY", LISTS);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(KLPTR("_SCAL")), "c1;c2,XXX,YYY");

    rc = B_DataAppend("U - More comment on U", COMMENTS);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(KCPTR("U")), "Comment of U - More comment on U");

    // B_DataList(char* arg, int type)
    rc = B_DataList("LC ac*", SCALARS);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(KLPTR("LC")), "acaf1;acaf2;acaf3;acaf4");
    printf("LC = \"%s\"\n", KLPTR("LC"));

    // B_DataCalcLst(char* arg, int unused)
    B_DataUpdate("LST1 A,B,C", LISTS);
    B_DataUpdate("LST2 C,D,E", LISTS);

    rc = B_DataCalcLst("_RES LST1 + LST2");
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(KLPTR("_RES")), "A;B;C;D;E");

    rc = B_DataCalcLst("_RES LST1 * LST2");
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(KLPTR("_RES")), "C");

    rc = B_DataCalcLst("_RES LST1 - LST2");
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(KLPTR("_RES")), "A;B");

    rc = B_DataCalcLst("_RES LST1 x LST2");
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(KLPTR("_RES")), "AC;AD;AE;BC;BD;BE;CC;CD;CE");

    // B_DataCompare(char* arg, int type)
    sprintf(buf,  "%sfun.al WS_ONLY FILE_ONLY BOTH_DIFF BOTH_EQ", IODE_DATA_DIR);
    rc = B_DataCompare(buf, LISTS);
    //printf("    WS_ONLY='%s'\n", KLPTR("WS_ONLY"));
    //printf("    FILE_ONLY='%s'\n", KLPTR("FILE_ONLY"));
    //printf("    BOTH_DIFF='%s'\n", KLPTR("BOTH_DIFF"));
    //printf("    BOTH_EQ='%s'\n", KLPTR("BOTH_EQ"));

    EXPECT_EQ(rc, 0);
    EXPECT_EQ(std::string(KLPTR("WS_ONLY")), "AB;BC;L1;L2;L3;LC;LIST1;LIST2;LST1;LST2;NEWLIST;RC;RES;U;ZZZ;_EXO;_RES");
    //printf("%s\n", KLPTR("WS_ONLY"));

    // B_DataPrintGraph()
    //smpl = KSMPL(KV_WS);
    //printf("%d\n", smpl->start_period);
    rc = B_DataPrintGraph("Grt Line No No Level -- -- 2000Y1 2020Y1 ACAF ACAG ACAF+ACAG");
    EXPECT_EQ(rc, 0);

    // B_DataDisplayGraph()
    rc = B_DataDisplayGraph("Grt Line No No Level -- -- 2000Y1 2020Y1 ACAF ACAG ACAF+ACAG");
}


TEST_F(IodeCAPITest, Tests_B_EQS)
{
    int     rc, pos;
    Sample  *smpl;
    char    cmd_B_EqsEstimate[] = "1980Y1 1996Y1 ACAF";
    char    cmd_B_EqsSetSample[] = "1981Y1 1995Y1 ACAF";

    U_test_print_title("Tests B_EQS");
    U_test_suppress_kmsg_msgs();

    // (Re-)loads 3 WS and check ok
    U_test_load_fun_esv("fun");

    // B_EqsEstimate()
    rc = B_EqsEstimate(cmd_B_EqsEstimate);
    EXPECT_EQ(rc, 0);

    EXPECT_DOUBLE_EQ(round(K_e_r2(KE_WS, "ACAF") * 1e8) / 1e8, 0.82181543);
    EXPECT_DOUBLE_EQ(round(K_e_fstat(KE_WS, "ACAF") * 1e8) / 1e8, 32.28510666);

    // B_EqsSetSample()
    rc = B_EqsSetSample(cmd_B_EqsSetSample);
    pos = K_find(KE_WS, "ACAF");
    smpl = &KESMPL(KE_WS, pos);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(smpl->start_period.year, 1981);

    // TODO: implement next utests with the same canevas
        //B_EqsSetMethod(char* arg, int unused)
        //B_EqsSetBloc(char* arg, int unused)
        //B_EqsSetCmt(char* arg, int unused)
        //B_EqsSetInstrs(char* arg, int unused)

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_FILE)
{
    int     rc;

    U_test_print_title("Tests B_FILE");

    // Cleanup files
    _unlink("test.a2m");
    _unlink("tata.a2m");
    _unlink("tutu.a2m");

    //  Create a file
    U_test_create_a_file("test", W_A2M);

    //  Check that the file exists
    U_test_file_exists("test.a2m", "File %s exists");

    //  Call B_FileRename()
    rc = B_FileRename("test tata", FILE_A2M);
    rc = U_test_file_exists("tata.a2m", "B_FileRename(\"test tata\", FILE_A2M)");

    //  Call B_FileCopy()
    rc = B_FileCopy("tata tutu", FILE_A2M);
    rc = U_test_file_exists("tutu.a2m", "B_FileCopy(\"tata tutu \", FILE_A2M)");

    //  Call B_FileDelete()
    rc = B_FileDelete("tata tutu", FILE_A2M);
    rc = U_test_file_not_exists("tata.a2m", "B_FileDelete(\"tata tutu\", FILE_A2M)");

/*
    "fileimportvar",            B_FileImportVar,        SB_XodeRuleImport,  0,
    "fileimportcmt",            B_FileImportCmt,        SB_XodeRuleImport,  0,
*/

    U_test_reset_a2m_msgs();
}


TEST_F(IodeCAPITest, Tests_B_FSYS)
{
    int     rc;
    char    arg[512];

    U_test_print_title("Tests B_FSYS");

    // Cleanup files
    _unlink("test.a2m");
    _unlink("test.a2m.oem");
    _unlink("test.a2m.ansi");
    _unlink("testdbl.a2m.ansi");
    _unlink("brol.a2m.ansi");
    _unlink("brol2.a2m.ansi");


    //  Create test.a2m -> ansi-coded file
    // sprintf(arg, "%s\\test", IODE_OUTPUT_DIR);
    // U_test_create_a_file("test", W_A2M); // Ansi-coded file
    // => Pb with conversions => files created by test1.c differ from those created by test_c_api.cpp.
    // Therefore, we copy data\test.a2m into local test.a2m
    sprintf(arg, "%stest.a2m test.a2m", IODE_DATA_DIR);
    rc = B_SysCopy(arg);
    EXPECT_EQ(rc, 0);
    U_test_compare_localfile_to_reffile("test.a2m", "test.a2m");

    // B_SysAnsiToOem() : translate ansi to oem -> test.a2m.oem
    sprintf(arg, "test.a2m test.a2m.oem");
    rc = B_SysAnsiToOem(arg);
    EXPECT_EQ(rc, 0);
    U_test_compare_localfile_to_reffile("test.a2m.oem", "test.a2m.oem.ref");

    // B_SysAnsiToUTF8() : translate ansi to utf8 -> test.a2m.utf8
    sprintf(arg, "test.a2m test.a2m.utf8");
    rc = B_SysAnsiToUTF8(arg);
    EXPECT_EQ(rc, 0);
    U_test_compare_localfile_to_reffile("test.a2m.utf8", "test.a2m.utf8.ref");

    // B_SysOemToAnsi() : translate oem to ansi -> test.a2m.ansi
    sprintf(arg, "test.a2m.oem test.a2m.ansi");
    rc = B_SysOemToAnsi(arg);
    EXPECT_EQ(rc, 0);
    U_test_compare_localfile_to_reffile("test.a2m.ansi", "test.a2m.ansi.ref");

    // B_SysOemToUTF8() : translate ansi to utf8 -> test.a2m.utf8
    sprintf(arg, "test.a2m.oem test.a2m.utf8");
    rc = B_SysOemToUTF8("test.a2m.oem test.a2m.utf8");
    EXPECT_EQ(rc, 0);
    U_test_compare_localfile_to_reffile("test.a2m.utf8", "test.a2m.utf8.ref");

    // B_SysRename(char* arg, int unused) : rename test.a2m.ansi -> brol.a2m.ansi
    sprintf(arg, "test.a2m.ansi brol.a2m.ansi");
    EXPECT_EQ(rc, 0);
    rc = B_SysRename(arg);
    U_test_compare_localfile_to_reffile("brol.a2m.ansi", "test.a2m.ansi.ref");

    // B_SysCopy(char* arg, int unused) : copy brol.a2m.ansi dans testdbl.a2m.ansi
    sprintf(arg, "brol.a2m.ansi testdbl.a2m.ansi");
    rc = B_SysCopy(arg);
    EXPECT_EQ(rc, 0);
    U_test_compare_localfile_to_reffile("testdbl.a2m.ansi", "test.a2m.ansi.ref");

    // B_SysAppend(char* arg, int unused) : append testdbl.a2m.ansi to brol.a2m.ansi
    sprintf(arg, "brol.a2m.ansi testdbl.a2m.ansi");
    rc = B_SysAppend(arg);
    EXPECT_EQ(rc, 0);
    U_test_compare_localfile_to_reffile("testdbl.a2m.ansi", "testdbl.a2m.ansi.ref");

    // B_SysDelete(char* arg, int unused) : delete totdbl.a2m.ansi and brol.a2m.ansi
    rc = B_SysDelete("brol.a2m.ansi testdbl.a2m.ansi tutu.a2m test.a2m test.a2m.oem test.a2m.utf8");
    rc = U_test_file_not_exists("brol.a2m.ansi",  "B_SysDelete(\"brol.a2m.ansi\")");
    rc = U_test_file_not_exists("brol2.a2m.ansi", "B_SysDelete(\"testdbl.a2m.ansi\")");

    U_test_reset_a2m_msgs();
}


TEST_F(IodeCAPITest, Tests_B_IDT)
{
    char        filename[256];
    char        idtexec[] = "2002Y1 2007Y1 C D";
    char        idtexec2[] = "2002Y1 2007Y1 C D";
    int         rc;
    double      *C, *D;

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
    sprintf(filename,  "%sfun", IODE_DATA_DIR);
    rc = B_IdtExecuteVarFiles(filename);
    EXPECT_EQ(rc, 0);

    // Compute the idts of a partial sample
    rc = B_IdtExecute(idtexec);
    EXPECT_NE(rc, 0);

    // Second trial with WS in filenames
    K_clear(KV_WS);
    U_test_CreateObjects(); // Create vars on 2000Y1:2010Y1 => A=[0, 1...], B=[0, 2, 4...], BC...

    sprintf(filename,  "WS %sfun", IODE_DATA_DIR);
    rc = B_IdtExecuteVarFiles(filename);
    EXPECT_EQ(rc, 0);

    // Compute the idts of a partial sample
    rc = B_IdtExecute(idtexec2);
    EXPECT_EQ(rc, 0);

    // Check the values
    C = (double*)KVPTR("C");
    D = (double*)KVPTR("D");

    EXPECT_DOUBLE_EQ(D[1], IODE_NAN);
    EXPECT_DOUBLE_EQ(D[2], 2.0 + 4.0);
    EXPECT_DOUBLE_EQ(round(C[2] * 1e8) / 1e8, 6.0 * 2 - 0.92921251);
}


TEST_F(IodeCAPITest, Tests_B_IDT_EXECUTE)
{
    char    **idts;
    Sample  *smpl = NULL;
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

    // Sample (null => full sample, see K_exec())
    smpl = new Sample("1961Y1", "2015Y1");   
    idts = (char**) SCR_vtoms((U_ch*) "AOUC", (U_ch*) " ,;\t");
    rc = B_IdtExecuteIdts(smpl, idts);
    printf("rc=%d\n", rc);

    //AOUC = iode.get_var("AOUC")
    //test_eq(f"AOUC[1961Y1]", 0.24783192, AOUC[1])

    //varname = "AOUC"
    //res = iode.exec_lec(f"{varname}[1961Y1]", 0)
    //test_eq(f"{varname}[1961Y1]", 0.24783192, res)
    EXPECT_DOUBLE_EQ(round(AOUC[1] * 1e8) / 1e8, 0.24783192);
}


TEST_F(IodeCAPITest, Tests_IMP_EXP)
{
    char    outfile[256];
    char    reffile[256];
    char    varfile[256];
    char    cmtfile[256];
    char    rulefile[256];
    char    trace[] = " ";
    int     rc;

    U_test_print_title("Tests EXP: Export CSV and rcsv");

    U_test_suppress_kmsg_msgs();

    // Export
    // Exports VAR files into external formats.
    // int EXP_RuleExport(" "char* trace, NULL, char* out, char* vfile, char* cfile, char* from, char* to, char* na, char* sep, int fmt)

    sprintf(varfile, "%sfun.av", IODE_DATA_DIR);
    sprintf(cmtfile, "%sfun.ac", IODE_DATA_DIR);

    sprintf(outfile, "%sfun_xode.csv", IODE_OUTPUT_DIR);
    sprintf(reffile, "%sfun_xode.csv.ref", IODE_DATA_DIR);
    rc = EXP_RuleExport(trace, NULL, outfile, varfile, cmtfile, "2000Y1", "2010Y1", "#N/A", ";", EXPORT_CSV);
    EXPECT_EQ(rc, 0);
    U_test_compare_outfile_to_reffile("fun_xode.csv", "fun_xode.csv.ref");

    sprintf(outfile, "%sfun_xode.rcsv", IODE_OUTPUT_DIR);
    sprintf(reffile, "%sfun_xode.rcsv.ref", IODE_DATA_DIR);
    rc = EXP_RuleExport(trace, NULL, outfile, varfile, cmtfile, "2000Y1", "2010Y1", "#N/A", ";", EXPORT_RCSV);
    EXPECT_EQ(rc, 0);
    U_test_compare_outfile_to_reffile("fun_xode.rcsv", "fun_xode.rcsv.ref");

    // Export with rules (partial /+ change names)
    sprintf(outfile, "%sfun2.tsp", IODE_OUTPUT_DIR);
    sprintf(reffile, "%sfun2.ref.tsp", IODE_DATA_DIR);
    sprintf(rulefile, "%srules.txt", IODE_DATA_DIR);
    rc = EXP_RuleExport(trace, rulefile, outfile, varfile, cmtfile, "1995Y1", "2005Y1", "#N/A", ";", EXPORT_TSP);
    EXPECT_EQ(rc, 0);
    U_test_compare_outfile_to_reffile("fun2.tsp", "fun2.ref.tsp");

    U_test_print_title("Tests IMP VAR: Import Ascii Variables");

    sprintf(reffile, "%sfun_xode.av.ref", IODE_DATA_DIR);
    sprintf(outfile, "%sfun_xode.var", IODE_OUTPUT_DIR);
    rc = IMP_RuleImport(VARIABLES, trace, NULL, outfile, reffile, "2000Y1", "2010Y1", IMPORT_ASCII, 0);
    EXPECT_EQ(rc, 0);

    KV_RWS = KV_WS = K_interpret(VARIABLES, outfile);
    U_test_lec("ACAF[2002Y1]", "ACAF[2002Y1]", 0, -0.92921251);


    U_test_print_title("Tests IMP CMT: Import Ascii Comments");

    sprintf(reffile, "%sfun_xode.ac.ref", IODE_DATA_DIR);
    sprintf(outfile, "%sfun_xode.cmt", IODE_OUTPUT_DIR);
    rc = IMP_RuleImport(COMMENTS, trace, rulefile, outfile, reffile, NULL, NULL, IMPORT_ASCII, 0);
    EXPECT_EQ(rc, 0);

    if(rc == 0) 
    {
        KC_RWS = KC_WS = K_interpret(COMMENTS, outfile);
        EXPECT_TRUE(KC_WS != NULL);
        EXPECT_EQ(std::string(KCPTR("KK_AF")), "Ondernemingen: ontvangen kapitaaloverdrachten.");
    }

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_XODE)
{
    char    outfile[256];
    char    reffile[256];
    char    rulefile[256];
    char    cmd[512];
    char    trace[] = " ";
    int     rc;

    U_test_print_title("Tests XODE: Import ASCII via report function");
    U_test_suppress_kmsg_msgs();

    sprintf(reffile, "%sfun_xode.av.ref", IODE_DATA_DIR);
    sprintf(outfile, "%sfun_xode.var", IODE_OUTPUT_DIR);
    sprintf(rulefile, "%srules.txt", IODE_DATA_DIR);
    sprintf(cmd, "ASCII \"%s\" %s %s 2000Y1 2010Y1", rulefile, reffile, outfile);

    rc = B_FileImportVar(cmd);
    EXPECT_EQ(rc, 0);

    KV_RWS = KV_WS = K_interpret(VARIABLES, outfile);
    U_test_lec("KK_AF[2002Y1]", "KK_AF[2002Y1]", 0, -0.92921251);

    U_test_reset_kmsg_msgs();

}


TEST_F(IodeCAPITest, Tests_B_LTOH)
{
    char    cmd[512];
    char    varfile[256];
    Sample  *smpl;
    int     rc;

    U_test_print_title("Tests B_LTOH: convert low periodicity to high periodicity");
    U_test_suppress_kmsg_msgs();

    // Clear the vars and set the sample for the variable WS
    K_clear(KV_WS);
    smpl = new Sample("2010Q1", "2020Q4");
    KV_sample(KV_WS, smpl);
    delete smpl;

    sprintf(varfile, "%sfun.av", IODE_DATA_DIR);

    // Linear interpolation / stock
    sprintf(cmd, "L %s ACAF", varfile);
    rc = B_WsLtoHStock(cmd);
    EXPECT_EQ(rc, 0);
    U_test_lec("ACAF[2014Q3]", "ACAF[2014Q3]", 0, -79.729132);

    // Linear interpolation / flow
    sprintf(cmd, "L %s ACAG", varfile);
    rc = B_WsLtoHFlow(cmd);
    EXPECT_EQ(rc, 0);
    U_test_lec("ACAG[2014Q3]", "ACAG[2014Q3]", 0, 8.105075);

    // Cubic Splines / stock
    sprintf(cmd, "C %s ACAF", varfile);
    rc = B_WsLtoHStock(cmd);
    EXPECT_EQ(rc, 0);
    U_test_lec("ACAF[2012Q3]", "ACAF[2012Q3]", 0, -52.805666);

    // Cubic splines / flow
    sprintf(cmd, "C %s ACAG", varfile);
    rc = B_WsLtoHFlow(cmd);
    EXPECT_EQ(rc, 0);
    U_test_lec("ACAG[2012Q3]", "ACAG[2012Q3]", 0, 7.6135768);

    // Step / stock
    sprintf(cmd, "S %s ACAF", varfile);
    rc = B_WsLtoHStock(cmd);
    EXPECT_EQ(rc, 0);
    U_test_lec("ACAF[2014Q3]", "ACAF[2014Q3]", 0, -83.340625);

    // Step / flow
    sprintf(cmd, "S %s ACAG", varfile);
    rc = B_WsLtoHFlow(cmd);
    EXPECT_EQ(rc, 0);
    U_test_lec("ACAG[2014Q3]", "ACAG[2014Q3]", 0, 8.1050747);

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_HTOL)
{
    char    cmd[512];
    char    varfile[256];
    Sample  *smpl;
    int     rc;
    //char    current_dir[256];

    //getcwd(current_dir, sizeof(current_dir));
    //printf("%s\n", current_dir);

    U_test_print_title("Tests B_HTOL: convert high periodicity to low periodicity");
    U_test_suppress_kmsg_msgs();

    // Clear the vars and set the sample for the variable WS
    K_clear(KV_WS);
    smpl = new Sample("2000Y1", "2020Y1");
    KV_sample(KV_WS, smpl);
    delete smpl;

    sprintf(varfile, "%sfun_q.var", IODE_DATA_DIR);

    // Last Obs
    sprintf(cmd, "%s ACAF", varfile);
    rc = B_WsHtoLLast(cmd);
    EXPECT_EQ(rc, 0);
    U_test_lec("ACAF[2014Y1]", "ACAF[2014Y1]", 0, -83.340625);

    // Mean
    sprintf(cmd, "%s ACAG", varfile);
    rc = B_WsHtoLMean(cmd);
    EXPECT_EQ(rc, 0);
    U_test_lec("ACAG[2014Y1]", "ACAG[2014Y1]", 0, 8.1050747);

    // Sum
    sprintf(cmd, "%s AOUC", varfile);
    rc = B_WsHtoLSum(cmd);
    EXPECT_EQ(rc, 0);
    U_test_lec("AOUC[2014Y1]", "AOUC[2014Y1]", 0, 1.423714 );

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_MODEL)
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
    // X int B_ModelSimulateParms(char* arg, int unused)                         $ModelSimulateParms eps relax maxit {Connex | Triang | None } 0 - 4 (starting values) {Yes | no } {yes | No } nbtri
    // X int B_ModelExchange(char* arg, int unused)                              $ModelExchange eqname1-varname1,eqname2-varname2,...
    // X int B_ModelCompile(char* arg, int unused)                               $ModelCompile  [eqname1, eqname2, ... ]
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
    EXPECT_NE(kdbv, nullptr);
    kdbs = KS_WS;
    EXPECT_NE(kdbs, nullptr);
    kdbe = KE_WS;
    EXPECT_NE(kdbe, nullptr);

    // B_ModelSimulateParms()
    CSimulation::KSIM_START = VAR_INIT_TM1;
    CSimulation::KSIM_EPS = 0.00001;
    CSimulation::KSIM_MAXIT = 1000;
    CSimulation::KSIM_RELAX = 1.0;
    CSimulation::KSIM_SORT = 0;
    CSimulation::KSIM_PASSES = 3;
    CSimulation::KSIM_DEBUG = 1;
    rc = B_ModelSimulateParms("0.0001 0.7 100 Triang 0 no 5 no");
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(CSimulation::KSIM_EPS, 0.0001);
    EXPECT_EQ(CSimulation::KSIM_MAXIT, 100);
    EXPECT_EQ(CSimulation::KSIM_RELAX, 0.7);
    EXPECT_EQ(CSimulation::KSIM_DEBUG, 0);
    EXPECT_EQ(CSimulation::KSIM_PASSES, 5);


    // B_ModelSimulate()
    rc = B_ModelSimulate("2000Y1 2002Y1");
    EXPECT_EQ(rc, 0);
    // TODO: check result of one ENDO
    EXPECT_DOUBLE_EQ(round(KV_get_at_aper("ACAF", "2002Y1") * 1e6) / 1e6, -1.274623);

    // B_ModelExchange()

    // Reloads 3 WS
    U_test_load_fun_esv(filename);

    // Set values of endo UY
    KV_set_at_aper("UY", "2000Y1", 650.0);
    KV_set_at_aper("UY", "2001Y1", 670.0);
    KV_set_at_aper("UY", "2002Y1", 680.0);

    // Exchange
    rc = B_ModelExchange("UY-XNATY");
    EXPECT_EQ(rc, 0);

    // Simulate
    rc = B_ModelSimulate("2000Y1 2002Y1");
    EXPECT_EQ(rc, 0);

    // Check some results
    EXPECT_EQ(KV_get_at_aper("UY", "2000Y1"), 650.0);
    XNATY_2000Y1 = KV_get_at_aper("XNATY", "2000Y1");
    //printf("XNATY_2000Y1 = %lg\n", XNATY_2000Y1);
    EXPECT_DOUBLE_EQ(round(KV_get_at_aper("XNATY", "2000Y1") * 1e6) / 1e6, 0.800673);

    // B_ModelCompile(char* arg, int unused)
    rc = B_ModelCompile("");
    EXPECT_EQ(rc, 0);

    // B_ModelCalcSCC(char *arg) $ModelCalcSCC nbtris prename intername postname [eqs]
    rc = B_ModelCalcSCC("5 _PRE2 _INTER2 _POST2");
    EXPECT_EQ(rc, 0);
    rc = strcmp(KLPTR("_PRE2"), "BRUGP;DTH1C;EX;ITCEE;ITCR;ITGR;ITI5R;ITIFR;ITIGR;ITMQR;NATY;POIL;PW3;PWMAB;PWMS;PWXAB;PWXS;PXE;QAH;QWXAB;QWXS;QWXSS;SBGX;TFPFHP_;TWG;TWGP;ZZF_;DTH1;PME;PMS;PMT");
    EXPECT_EQ(rc, 0);

    // int B_ModelSimulateSCC(char *arg)                           $ModelSimulateSCC from to pre inter post
    //  1. Annuler Exchange
    rc = B_ModelExchange("");
    EXPECT_EQ(rc, 0);

    //  2. ReLoads 3 WS to reset EXO XNATY to its original value
    U_test_load_fun_esv(filename);

    //  3. Simulate & compare
    rc = B_ModelSimulateSCC("2000Y1 2002Y1 _PRE2 _INTER2 _POST2");
    EXPECT_EQ(rc, 0);
    EXPECT_DOUBLE_EQ(round(KV_get_at_aper("ACAF", "2002Y1") * 1e6) / 1e6, -1.274623);

    // B_ModelSimulateSaveNIters(char *arg)                    $ModelSimulateSaveNiters varname



    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_WS)
{
    int     rc;
    Sample  *smpl;

    // List of tested report functions:
    //   - int B_WsLoad(char* arg, int type)                 $WsLoad<type> filename
    //   - int B_WsSave(char* arg, int type)                 $WsSave<type> filename
    //   - int B_WsSaveCmp(char* arg, int type)              $WsSaveCmp<type> filename
    //   - int B_WsExport(char* arg, int type)               $WsExport<type> filename
    //   - int B_WsImport(char* arg, int type)               $WsImport<type> filename
    //   - int B_WsSample(char* arg, int unused)                         $WsSample period_from period_to
    //   - int B_WsClear(char* arg, int type)                $WsClear<type>
    //   - int B_WsClearAll(char* arg, int unused)                       $WsClearAll
    //   - int B_WsDescr(char* arg, int type)                $WsDescr<type> free text
    //   - int B_WsName(char* arg, int type)                 Sets the WS name. Obsolete as report function.
    //   - int B_WsCopy(char* arg, int type)                 $WsCopy<type> fichier;fichier;.. obj1 obj2... or $WsCopyVar file;file;.. [from to] obj1 obj2...
    //   - int B_WsMerge(char* arg, int type)                $WsMerge<type> filename
    //   - int B_WsExtrapolate(char* arg, int unused)                    $WsExtrapolate [method] from to [variable list]
    //   - int B_WsAggrChar(char* arg, int unused)                       $WsAggrChar char
    //   - int B_WsAggrSum(char* arg, int unused)                        $WsAggrSum pattern filename
    //   - int B_WsAggrProd(char* arg, int unused)                       $WsAggrProd pattern filename
    //   - int B_WsAggrMean(char* arg, int unused)                       $WsAggrMean pattern filename
    //   - int B_StatUnitRoot(char* arg, int unused)                     $StatUnitRoot drift trend order expression
    //   - int B_CsvSave(char* arg, int type)                $CsvSave<type> file name1 name2 ...
    //   - int B_CsvNbDec(char *nbdec, int unused)                       $CsvNbDec nn
    //   - int B_CsvSep(char *sep, int unused)                           $CsvSep char
    //   - int B_CsvNaN(char *nan, int unused)                           $CsvNaN text
    //   - int B_CsvAxes(char *var, int unused)                          $CsvAxes AxisName
    //   - int B_CsvDec(char *dec, int unused)                           $CsvDec char

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

    // check equation->endo == equation name
    for(int i = 0; i < KNB(KE_WS); i++)
        ASSERT_EQ(std::string(KEVAL(KE_WS, i)->endo), std::string(KONAME(KE_WS, i)));

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
    U_test_B_WsImport("fun2.ae", EQUATIONS, 274);
    U_test_B_WsImport("fun2.ai", IDENTITIES, 48);
    U_test_B_WsImport("fun2.al", LISTS, 17);
    U_test_B_WsImport("fun2.as", SCALARS, 161);
    U_test_B_WsImport("fun2.at", TABLES, 46);
    U_test_B_WsImport("fun2.av", VARIABLES, 394);
    // TODO : correct fun.eqs (W) and fun.idt (NAWRU)

    // check equation->endo == equation name
    for(int i = 0; i < KNB(KE_WS); i++)
        ASSERT_EQ(std::string(KEVAL(KE_WS, i)->endo), std::string(KONAME(KE_WS, i)));

    // int B_WsSample(char* arg, int unused)                         $WsSample period_from period_to
    U_test_print_title("B_WsSample()");
    rc = B_WsSample("1965Y1 2020Y1");
    smpl = new Sample("1965Y1", "2020Y1");
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(*KSMPL(KV_WS), *smpl);
    delete smpl;

    // int B_WsClearAll(char* arg, int unused)                       $WsClearAll
    U_test_print_title("B_WsClearAll()");
    rc = B_WsClearAll("");
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(KNB(KC_WS), 0);
    EXPECT_EQ(KNB(KE_WS), 0);
    EXPECT_EQ(KNB(KI_WS), 0);
    EXPECT_EQ(KNB(KL_WS), 0);
    EXPECT_EQ(KNB(KS_WS), 0);
    EXPECT_EQ(KNB(KT_WS), 0);
    EXPECT_EQ(KNB(KV_WS), 0);

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

    // int B_WsExtrapolate(char* arg, int unused)                    $WsExtrapolate [method] from to [variable list]
    U_test_print_title("B_WsExtrapolate");
    U_test_B_WsExtrapolate(0, 6.0);
    U_test_B_WsExtrapolate(1, 4.0);
    U_test_B_WsExtrapolate(2, 7.0);
    U_test_B_WsExtrapolate(3, 7.0);
    U_test_B_WsExtrapolate(4, IODE_NAN);
    U_test_B_WsExtrapolate(5, 6.0);
    U_test_B_WsExtrapolate(6, 7.0);

    // int B_WsAggrChar(char* arg, int unused)                       $WsAggrChar char
    // int B_WsAggrSum(char* arg, int unused)                        $WsAggrSum pattern filename
    // int B_WsAggrProd(char* arg, int unused)                       $WsAggrProd pattern filename
    // int B_WsAggrMean(char* arg, int unused)                       $WsAggrMean pattern filename
    U_test_print_title("B_WsAggregate");
    U_test_B_WsAggregate();

    // int B_StatUnitRoot(char* arg, int unused)                     $StatUnitRoot drift trend order expression
    U_test_print_title("B_StatUnitRoot");
    U_test_B_WsLoad("fun", SCALARS, 161);
    U_test_B_WsLoad("fun", VARIABLES, 394);

    U_test_B_StatUnitRoot(0, 0, 0, "ACAF", 0.958325);
    U_test_B_StatUnitRoot(1, 0, 0, "ACAF", 1.117498);
    U_test_B_StatUnitRoot(1, 1, 0, "ACAF", -0.798686);
    U_test_B_StatUnitRoot(0, 0, 2, "ACAF", 1.419631);

    // int B_CsvNbDec(char *nbdec, int unused)                       $CsvNbDec nn
    // int B_CsvSep(char *sep, int unused)                           $CsvSep char
    // int B_CsvNaN(char *nan, int unused)                           $CsvNaN text
    // int B_CsvAxes(char *var, int unused)                          $CsvAxes AxisName
    // int B_CsvDec(char *dec, int unused)                           $CsvDec char
    // int B_CsvSave(char* arg, int type)                $CsvSave<type> file name1 name2 ...
    U_test_print_title("B_Csv*");
    U_test_B_Csv();

    U_test_reset_kmsg_msgs();

}


TEST_F(IodeCAPITest, Tests_B_REP_LINE)
{
    int     rc;
    char    fullfilename[256], cmd[1024];


    U_test_print_title("Tests B_ReportLine()");
    U_test_suppress_kmsg_msgs();

    // Simple test of a call to B_ReportLine(). More elaborate commands are tested with B_ReportExec()
    sprintf(fullfilename,  "%s%s", IODE_DATA_DIR, "fun.av");
    sprintf(cmd, "\n"
                 "$WsClearVar\n"
                 "$WsLoadVar %s\n", fullfilename);

    rc = B_ReportLine(cmd, 1);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(KV_WS->k_nb, 394);

    rc = B_ReportLine("$settime 2000Y1", 0);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(RP_PER.to_string(), "2000Y1");

    rc = B_ReportLine("$incrtime 2", 0);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(RP_PER.to_string(), "2002Y1");

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_REP_ENGINE)
{
    int     rc;
    char    cmd[1024];

    U_test_print_title("Tests B_ReportExec(\"rep_expand.rep\")");
    U_test_suppress_kmsg_msgs();

    // Calls to B_ReportExec(reportfile)
    // Report rep_expand.rep: expand %% {lec}, {$cmd}, {$!cmd} and @fn().

    sprintf(cmd,  "%srep_expand.rep %s %s", IODE_REPORT_DIR, IODE_DATA_DIR, IODE_OUTPUT_DIR);
    rc = B_ReportExec(cmd); // TODO: check that the output file is closed !!
    EXPECT_EQ(rc, 0);
    U_test_compare_outfile_to_reffile("rep_expand.a2m", "rep_expand.ref.a2m");

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_REP_FNS)
{
    int     rc;
    char    cmd[1024];

    U_test_print_title("Tests B_ReportExec(\"rep_fns.rep\")");
    U_test_suppress_kmsg_msgs();

    // Execution of the report rep_fns.rep
    sprintf(cmd,  "%srep_fns.rep %s %s", IODE_REPORT_DIR, IODE_DATA_DIR, IODE_OUTPUT_DIR);
    rc = B_ReportExec(cmd);
    EXPECT_EQ(rc, 0);
    U_test_compare_outfile_to_reffile("rep_fns.a2m", "rep_fns.ref.a2m");

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_B_REP_PROC)
{
    int     rc;
    char    cmd[1024];

    U_test_print_title("Tests B_ReportExec(\"rep_proc.rep\")");
    U_test_suppress_kmsg_msgs();

    // Execution of the report rep_fns.rep
    sprintf(cmd,  "%srep_proc.rep %s %s", IODE_REPORT_DIR, IODE_DATA_DIR, IODE_OUTPUT_DIR);
    rc = B_ReportExec(cmd);
    EXPECT_EQ(rc, 0);
    U_test_compare_outfile_to_reffile("rep_proc.a2m", "rep_proc.ref.a2m");

    U_test_reset_kmsg_msgs();
}


TEST_F(IodeCAPITest, Tests_RAS_EXECUTE)
{
    char* pattern = "xy";
    char* xdim = "$X";
    char* ydim = "$Y";
    Period* ref_period = new Period("2000Y1");
    Period* sum_period = new Period("2001Y1");
    int maxit = 100;
    double eps = 0.0001;

    int pos;
    int res;

    B_WsClear("Var", VARIABLES);
    B_WsSample("2000Y1 2001Y1");

    pos = KV_add(KV_WS, "R1C1");
    *KVVAL(KV_WS, pos, 0) = 5.0;
    pos = KV_add(KV_WS, "R1C2");
    *KVVAL(KV_WS, pos, 0) = 3.0;
    pos = KV_add(KV_WS, "R1C3");
    *KVVAL(KV_WS, pos, 0) = 5.0;
    pos = KV_add(KV_WS, "R1C4");
    *KVVAL(KV_WS, pos, 0) = 7.0;
    *KVVAL(KV_WS, pos, 1) = 5.0;
    pos = KV_add(KV_WS, "R1CT");
    *KVVAL(KV_WS, pos, 0) = 20.0;
    *KVVAL(KV_WS, pos, 1) = 20.0;

    pos = KV_add(KV_WS, "R2C1");
    *KVVAL(KV_WS, pos, 0) = 1.0;
    pos = KV_add(KV_WS, "R2C2");
    *KVVAL(KV_WS, pos, 0) = 1.0;
    *KVVAL(KV_WS, pos, 1) = 2.0;
    pos = KV_add(KV_WS, "R2C3");
    *KVVAL(KV_WS, pos, 0) = 4.0;
    pos = KV_add(KV_WS, "R2C4");
    *KVVAL(KV_WS, pos, 0) = 4.0;
    pos = KV_add(KV_WS, "R2CT");
    *KVVAL(KV_WS, pos, 0) = 10.0;
    *KVVAL(KV_WS, pos, 1) = 10.0;

    pos = KV_add(KV_WS, "R3C1");
    *KVVAL(KV_WS, pos, 0) = 3.0;
    pos = KV_add(KV_WS, "R3C2");
    *KVVAL(KV_WS, pos, 0) = 1.0;
    pos = KV_add(KV_WS, "R3C3");
    *KVVAL(KV_WS, pos, 0) = 3.0;
    *KVVAL(KV_WS, pos, 1) = 2.0;
    pos = KV_add(KV_WS, "R3C4");
    *KVVAL(KV_WS, pos, 0) = 3.0;
    pos = KV_add(KV_WS, "R3CT");
    *KVVAL(KV_WS, pos, 0) = 10.0;
    *KVVAL(KV_WS, pos, 1) = 10.0;

    pos = KV_add(KV_WS, "R4C1");
    *KVVAL(KV_WS, pos, 0) = 1.0;
    *KVVAL(KV_WS, pos, 1) = 0.0;
    pos = KV_add(KV_WS, "R4C2");
    *KVVAL(KV_WS, pos, 0) = 2.0;
    pos = KV_add(KV_WS, "R4C3");
    *KVVAL(KV_WS, pos, 0) = 1.0;
    pos = KV_add(KV_WS, "R4C4");
    *KVVAL(KV_WS, pos, 0) = 1.0;
    pos = KV_add(KV_WS, "R4CT");
    *KVVAL(KV_WS, pos, 0) = 5.0;
    *KVVAL(KV_WS, pos, 1) = 5.0;

    pos = KV_add(KV_WS, "RTC1");
    *KVVAL(KV_WS, pos, 0) = 10.0;
    *KVVAL(KV_WS, pos, 1) = 10.0;
    pos = KV_add(KV_WS, "RTC2");
    *KVVAL(KV_WS, pos, 0) = 7.0;
    *KVVAL(KV_WS, pos, 1) = 7.0;
    pos = KV_add(KV_WS, "RTC3");
    *KVVAL(KV_WS, pos, 0) = 13.0;
    *KVVAL(KV_WS, pos, 1) = 13.0;
    pos = KV_add(KV_WS, "RTC4");
    *KVVAL(KV_WS, pos, 0) = 15.0;
    *KVVAL(KV_WS, pos, 1) = 15.0;

    pos = KV_add(KV_WS, "RTCT");
    *KVVAL(KV_WS, pos, 0) = 90.0;
    *KVVAL(KV_WS, pos, 1) = 90.0;

    K_add(KL_WS, "X", "R1,R2,R3,R4,RT");
    K_add(KL_WS, "Y", "C1,C2,C3,C4,CT");

    pos = K_find(KL_WS, "X");
    EXPECT_NE(pos, -1);
    pos = K_find(KL_WS, "Y");
    EXPECT_NE(pos, -1);

    // Note: ref_period and sum_period are freed in RasExecute()
    res = RasExecute(pattern, xdim, ydim, ref_period, sum_period, maxit, eps);
    delete ref_period;
    delete sum_period;

    EXPECT_EQ(res, 0);
}
