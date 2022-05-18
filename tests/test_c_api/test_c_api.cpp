// The present source file has been generated from the source
// file test1.c (Google Test is not available with Borland).
// DO NOT MODIFY IT!
// Modify test1.c instead and run convert_tests.bat/sh


#include "pch.h"
#include <filesystem>


#ifdef _MSC_VER
#endif
#ifdef __cplusplus
extern "C"
{
#endif

    extern char         SCR_NAME_ERR[255 + 1];

    //int o_estgr(char** titles, SAMPLE *smpl, MAT* mlhs, MAT* mrhs, int view, int res) {return(0);}
    int B_A2mSetRtfTitle(U_ch* title) {return(0);}
    int B_A2mSetRtfCopy(U_ch* copyr) {return(0);}
    int B_PrintRtfTopic(char* x) { return(0); }
    int A2mGIF_HTML() {return(0);}
    int W_printf(char*fmt, ...) {return(0);}
    void K_load_iode_ini() {}
    void K_save_iode_ini() {}

#ifdef __cplusplus
}

#endif
void kmsg_null(char*msg)
{
}

#ifdef _MSC_VER
#endif


class IodeCAPITest : public ::testing::Test 
{
protected:
	char input_test_dir[256];
	char output_test_dir[256];

public:
    void SetUp() override 
    {
        // C++ consider all passed string value of the kind "..." to C function as CONSTANT
        char buf1[64];
        char buf2[64];

        // NOTE: we assume that: 
		//       - current path is binaryDir/tests/test_cpp_api
		//       - data directory has been copied in binaryDir/tests (see CMakeLists.txt in root directory)
		std::filesystem::path cwd = std::filesystem::current_path();
		std::string str_path = cwd.parent_path().string() + "\\";
		strcpy_s(input_test_dir, (str_path + "data\\").c_str());
		strcpy_s(output_test_dir, (str_path + "output\\").c_str());

        // other initializations
        U_test_init();
    }

	int U_cmp_str(char* str1, char* str2)
	{
	    if(str1 == NULL && str2 == NULL) return(1);
	    if(str1 == NULL || str2 == NULL) return(0);
	    return(!strcmp(str1, str2));
	}

	int U_cmp_tbl(char** tbl1, char* vec)
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

	void U_tests_Objects()
	{
	    char*       lst;
	    SAMPLE*     smpl;
	    IODE_REAL   A[64], B[64];
	    int         nb, i, pos;
	    static int  done = 0;
	
	    if(done) return;
	    done = 1;
	
	    // Create lists
	    pos = K_add(KL_WS, "LST1", "A,B");
	    EXPECT_TRUE(pos >= 0);
	    K_add(KL_WS, "LST2", "A,B,A");
	    lst = KLPTR("LST1");
	    EXPECT_EQ(strcmp(lst, "A,B"), 0);
	
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
	    EXPECT_TRUE(K_find(KV_WS, "A") >= 0);
	    pos = K_add(KV_WS, "B", B, &nb);
	
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
	    EXPECT_TRUE(U_test_eq(expected_val, calc_val));
	}

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
	
	    sprintf(fullfilename,  "%s\\%s", input_test_dir, filename);
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

	void U_test_init()
	{
	    static int  done = 0;
	
	    if(done) return;
	    done = 1;
	
	    IODE_assign_super_API();            // set *_super fn pointers
	    // strcpy(SCR_NAME_ERR, "iode.msg");   // message file => temporarily suppressed for GitHub
	    K_init_ws(0);                       // Initialises 7 empty WS
	    K_load_iode_ini();
	
	}


	//void TearDown() override {}

};


TEST_F(IodeCAPITest, Tests_BUF)
{
    EXPECT_EQ(BUF_DATA, nullptr);
    EXPECT_NE(BUF_strcpy("ABCD"), nullptr);
    EXPECT_NE(BUF_alloc(100), nullptr);
    EXPECT_NE(BUF_strcpy("ABCD"), nullptr);
    BUF_free();
    EXPECT_EQ(BUF_DATA, nullptr);
}


TEST_F(IodeCAPITest, Tests_LEC)
{
    IODE_REAL *A, *B;

    // Create objects
    U_tests_Objects();

    A = (IODE_REAL*)KVPTR("A");
    B = (IODE_REAL*)KVPTR("B");
    // Tests LEC
    U_test_lec("LEC", "A+B",  2, A[2]+B[2]);
    U_test_lec("LEC", "ln A", 2, log(A[2]));
    U_test_lec("LEC", "A[2002Y1]",     2, A[2]);
    //S4ASSERT(0, "Erreur forcÃ©e");
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
//    B_DataUpdateEqs("A", "ln A := B + t", NULL, 'L', NULL, NULL, NULL, NULL, NULL);
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

    // Create objects
    U_tests_Objects();

    // A_init
    args = B_ainit_chk("$LST1", NULL, 0);
    EXPECT_TRUE(U_cmp_tbl(args, "A;B"));
    SCR_free_tbl((unsigned char**) args);
    //args = B_ainit_chk("A*", NULL, 0);

    // Test parameters in a file. test.args must exist in the current dir and contain the line
    // A1 A2
    sprintf(filename, "@%s\\test.args", input_test_dir);
    args = B_ainit_chk(filename, NULL, 0);
    EXPECT_TRUE(U_cmp_tbl(args, "A1;A2"));
    SCR_free_tbl((unsigned char**) args);
}


TEST_F(IodeCAPITest, Tests_ERRMSGS)
{
    B_seterrn(86, "bla bla");
    kerror(0, "Coucou de kerror %s", "Hello");
    kmsg("Coucou de kmsg %s -- %g", "Hello", 3.2);
}


TEST_F(IodeCAPITest, Tests_K_OBJFILE)
{
    char    in_filename[256];
    char    out_filename[256];
    KDB     *kdb_var;
    int     rc;

    sprintf(in_filename,  "%s\\fun.var", input_test_dir);
    sprintf(out_filename, "%s\\fun_copy.var", output_test_dir);

    kdb_var = K_interpret(K_VAR, in_filename);
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

    int     pos, col;
    TBL*    c_table;
    TCELL*  cells;
    char    *cell_content;


    sprintf(in_filename,  "%s\\fun.tbl", input_test_dir);

    kdb_tbl = K_interpret(K_TBL, in_filename);
    EXPECT_NE(kdb_tbl, nullptr);
    if(kdb_tbl) {
        sprintf(out_filename, "%s\\fun_copy.at", output_test_dir);
        rc = KT_save_asc(kdb_tbl, out_filename);
        EXPECT_EQ(rc, 0);
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


TEST_F(IodeCAPITest, Tests_Simulation)
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
    EXPECT_TRUE(U_cmp_str(lst, NULL));

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
    EXPECT_NE(rc, 0);

    // Check _PRE list after simulation (prolog)
    lst = KLPTR("_PRE");
    expected_lst = "BRUGP;DTH1C;EX;ITCEE;ITCR;ITGR;ITI5R;ITIFR;ITIGR;ITMQR;NATY;POIL;PW3;PWMAB;PWMS;PWXAB;PWXS;PXE;QAH;QWXAB;QWXS;QWXSS;SBGX;TFPFHP_;TWG;TWGP;ZZF_;DTH1;PME;PMS;PMT";
    //printf("     '%s'(%d)\n", expected_lst, strlen(expected_lst));
    EXPECT_TRUE(U_cmp_str(lst, expected_lst));

    // Check _DIVER list
    lst = KLPTR("_DIVER");
    //printf("'%s'\n", lst);
    expected_lst = "SSH3O,WBG,SSF3,YDH,DTH,YDTG,YSFIC,WMIN,WLCP,WBGP,YSEFT2,YSEFT1,YSEFP,SBG,PWBG,W,ZJ,QMT,QI5,QC_,SSFG,YDH_,SG,ACAG,FLG";
    EXPECT_TRUE(U_cmp_str(lst, expected_lst));

    // Test with with convergence (increase MAXIT)
    KSIM_MAXIT = 100;
    rc = K_simul(kdbe, kdbv, kdbs, smpl, NULL, NULL);
    EXPECT_EQ(rc, 0);

    // Test Endo-exo

    // Version avec Ã©change dans une seule Ã©quation
    // endo_exo = SCR_vtoms("UY-NIY", ",; ");
    // rc = K_simul(kdbe, kdbv, kdbs, smpl, endo_exo, NULL);
    // S4ASSERT(rc == 0, "Exchange UY-NIY converges on 2000Y1-2002Y1");
    // S4ASSERT(UY[pos2000] == 650.0, "Exchange UY-NIY: UY[2000Y1] == 650.0");
    // S4ASSERT(fabs(NIY[pos2000] - 658.423) < 0.01, "Exchange UY-NIY: NIY[2000Y1] == 658.423");

    // Version avec Ã©change dans min 2 equations
    // Set values of endo UY
    KV_set_at_aper("UY", "2000Y1", 650.0);
    KV_set_at_aper("UY", "2001Y1", 670.0);
    KV_set_at_aper("UY", "2002Y1", 680.0);

    // Simulate with exchange UY - XNATY
    endo_exo = SCR_vtoms((unsigned char*)"UY-XNATY", (unsigned char*)",; ");
    rc = K_simul(kdbe, kdbv, kdbs, smpl, (char**)endo_exo, NULL);

    // Check result
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(KV_get_at_aper("UY", "2000Y1"), 650.0);
    EXPECT_TRUE(U_test_eq(KV_get_at_aper("XNATY", "2000Y1"), 0.80071));

    // Cleanup
    SCR_free_tbl(endo_exo);
    SCR_free(smpl);
    kmsg_super = kmsg_super_ptr; // reset default value
}


TEST_F(IodeCAPITest, Tests_PrintTables)
{
    char    fullfilename[256];
    KDB     *kdbv, *kdbt;
    TBL     *tbl;
    int     rc;

    // Load the VAR workspace
    K_WS[K_VAR] = kdbv  = U_test_K_interpret(K_VAR, "fun.var");
    EXPECT_NE(kdbv, nullptr);

    // Load the TBL workspace
    K_WS[K_TBL] = kdbt  = U_test_K_interpret(K_TBL, "fun.tbl");
    EXPECT_NE(kdbt, nullptr);

    // Load a second VAR workspace in K_RWS[K_VAR][2]
    sprintf(fullfilename,  "%s\\%s", input_test_dir, "fun.var");
    rc = K_load_RWS(2, fullfilename);
    EXPECT_EQ(rc, 0);

    // Print
    tbl = KTPTR("C8_1");
    EXPECT_NE(tbl, nullptr);
    rc = T_print_tbl(tbl, "2000:5[1;2]");
    EXPECT_EQ(rc, 0);

    // Cleanup
    T_free(tbl);
    K_load_RWS(2, NULL);
}


TEST_F(IodeCAPITest, Tests_ALIGN)
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


